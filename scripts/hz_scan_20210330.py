import numpy as np
import datetime as dt
import time

import ocs
from ocs import matched_client
from multiprocessing import Process, Event
from threading import Thread
from chopper_client import Chopper

hz_start = 1  # chopping frequency hz_eff = 4*hz
hz_end = 60   # 60 hz = 240 hz_eff
num_pts = 60

smurf_script_dir = '/sodetlib/scratch/latrt/'
run_script = 'stream_data.py'
stream_time = 30
chop_time = round(1.5*stream_time)
setup_script = 'setup_stream.py' 
pysmurf = matched_client.MatchedClient('pysmurf-controller-s2', args=[])
chop_ctrl = Chopper(timeout=chop_time+10)
log_file = open('/data/test_logs/hz_scan_run_{}.txt'.format(int(dt.datetime.now().timestamp())), "a")

def setup_data():
    smurf_args = []
    print(f'Running {setup_script}')
    pysmurf.run.start(script=smurf_script_dir+setup_script, args=smurf_args)
    response= pysmurf.run.wait()
    if not response[2]['success']:
        raise ValueError('setup failed!')      

def take_timestream(vel):
    #event.wait()
    wait_time = (chop_time-stream_time)/2
    time.sleep(wait_time)
    print('{} Taking Data for {} secs'.format(dt.datetime.now(), stream_time))
    print_freq(hz)
    smurf_args = ['--time', f'{stream_time}']
    print(f'Running {run_script}')
    pysmurf.run.start(script=smurf_script_dir+run_script, args=smurf_args)
    response= pysmurf.run.wait()
    if not response[2]['success']:
        print('Script Failed!')
        log_file.write('## data streaming failed \n')
        log_file.flush()

def chop( vel, time):
    #event.wait()
    print('{} Running chopper at {} Hz for {} secs'.format(dt.datetime.now(),vel,time))
    print(chop_ctrl.chop_hz(vel, time))
    #resp = chop_ctrl.wait_for_response()
    #print(resp)

def print_freq(hz,log=True):
    out_str = '{} ,  {},'.format(dt.datetime.now().timestamp(), hz) 
    if log:
        log_file.write( out_str + '\n')
        log_file.flush()
    print(out_str)


hz_range = np.linspace(hz_start,hz_end,num_pts)
for hz in hz_range:
    #e = Event()
    #pChop = Process(target=chop, args=(hz, chop_time, e))
    #pChop.start()

    #pData = Process(target=take_timestream, args=(hz,e))
    #pData.start()

    #e.set()

    tChop = Thread(target=chop, args=([hz, chop_time]))
    tData = Thread(target=take_timestream, args=([hz]))
    threads = [tChop, tData]
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()
    #tChop.start()
    #tData.start()
    #time.sleep(chop_time + 5)
 
print('Script complete!')
log_file.close()
chop_ctrl.close()
