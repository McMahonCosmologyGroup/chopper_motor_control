import numpy as np
import time
import datetime as dt
import scipy.interpolate as spint
import argparse

import ocs
from ocs import matched_client
import gevent.monkey
gevent.monkey.patch_all(aggressive=False, thread=False)
from ocs.ocs_widgets import TaskWidget, ProcessWidget
from twisted.python import log
from twisted.logger import formatEvent, FileLogObserver

from threading import Thread
from chopper_client import Chopper

print('I am assuming we are in the middle of the motion')

smurf_script_dir = '/sodetlib/scratch/latrt/'
run_script = 'stream_data.py'
stream_time = 156
#setup_script = 'setup_stream.py' 
chop_vel = 2 #hz
chop_time = round(1.25*stream_time)
#chop_ctrl = Chopper(timeout=chop_time+10*1e3)

TEST_MOTION = False

x_vel = 0.5
y_vel = 0.5

total_distance_x = 10 #cm
total_distance_y = 20 #cm

N_pts_x = 11
N_pts_y = 21

### only called if TEST_OTION==True

wait_time = chop_time - stream_time

def setup_data():
    smurf_args = []
    print(f'Running {setup_script}')
    pysmurf.run.start(script=smurf_script_dir+setup_script, args=smurf_args)
    response= pysmurf.run.wait()
    if not response[2]['success']:
        raise ValueError('setup failed!')      

def take_timestream():
    time.sleep(wait_time)
    print('Taking Data')
    smurf_args = ['--time', f'{stream_time}']
    print(f'"Running" {run_script}')
    time.sleep(stream_time)
    #pysmurf.run.start(script=smurf_script_dir+run_script, args=smurf_args)
    #response= pysmurf.run.wait()
    #if not response[2]['success']:
      #  print('Script Failed!')
       # log_file.write('## data streaming failed \n')
       # log_file.flush()

def move_x( dist, vel):
    #print('Moving X {} cm at {} cm/s'.format(dist,vel))
    xy_stage.move_x_cm.start(distance=dist, velocity=vel)
    xy_stage.move_x_cm.wait()

def move_y( dist, vel):
    #print('Moving Y {} cm at {} cm/s'.format(dist,vel))
    xy_stage.move_y_cm.start(distance=dist, velocity=vel)
    xy_stage.move_y_cm.wait()

def print_position(log=True):
    out = xy_stage.acq.status().session.get('data')
    pos = out['x'], out['y']
    out_str = '{} ,  {},  {}'.format(dt.datetime.now().timestamp(),
                                pos[0], pos[1]) 
    if log:
        log_file.write( out_str + '\n')
        log_file.flush()
    print(out_str)

def chop( vel, time):
    print('{} Running chopper at {} Hz for {} secs'.format(dt.datetime.now(),vel,time))
    #print(chop_ctrl.chop_hz(vel, time))

#pysmurf = matched_client.MatchedClient('pysmurf-controller-s2', args=[])
#setup_data()
xy_stage = matched_client.MatchedClient('XYWing', args=[])

if N_pts_x > 1:
    x_step = total_distance_x/(N_pts_x-1) #cm
else:
    x_step = 0

if N_pts_y > 1:
    y_step = total_distance_y/(N_pts_y-1) #cm
else:
    y_step = 0

total_x_move = x_step*(N_pts_x-1)
total_y_move = y_step*(N_pts_y-1)

print('Total Motions: {}, {}'.format(total_x_move, total_y_move))
print('Assuming I am starting in the middle')
#xy_stage.position = [0,0]

if np.mod(N_pts_x, 2) == 0 or np.mod(N_pts_y, 2)== 0:
    raise ValueError("I only know how to deal with an odd number of data point")

print('Moving to start position')

if total_x_move > 0:
    move_x( -(N_pts_x-1)*x_step/2, x_vel )
if total_y_move > 0:
    move_y( -(N_pts_y-1)*y_step/2, y_vel )


log_file = open('/data/test_logs/xy_stage_run_{}.txt'.format(int(dt.datetime.now().timestamp())), "a")


for y in range(N_pts_y):
    if y > 0:
        move_y( y_step, y_vel)

    for x in range(N_pts_x):
        if x > 0:
            move_x(x_step, x_vel)
            
        print_position()  
        tChop = Thread(target=chop, args=([chop_vel, chop_time]))
        tData = Thread(target=take_timestream, args=([]))
        threads = [tChop, tData]
        for thread in threads:
            thread.start()
        for thread in threads:
            thread.join()
           # take_timestream()
    
    if y < N_pts_y-1: 
        move_x( -x_step*(N_pts_x-1), x_vel)

if total_x_move > 0:
    move_x( -(N_pts_x-1)*x_step/2, x_vel )
if total_y_move > 0:
    move_y( -(N_pts_y-1)*y_step/2, y_vel )

print_position(log=False)
log_file.close()
