import subprocess
import sys

import datetime as dt

### NEED TO FINISH THIS LATER (4/9/2021)
script, VEL, TIME = sys.argv
fn = r'C:\scripts\motor_sandbox\x64\Debug\motor_sandbox.exe'

T_max = 14400  # max allowed duration (in secs) for a single motor chop step 

if TIME > T_max:
    start_time = dt.datetime.now()
    end_time = start_time + dt.timedelta(seconds = TIME)
    VEL = str(VEL)
    TIME = str(int(TIME))
    print(VEL, TIME)
    while dt.datetime.now() < end_time:
        print('Spinning for a {:0.1f} block, more if needed...'.format(T_max/(60**2)))
        proc = subprocess.Popen(fn, stdin=subprocess.Pipe, text=True)
        proc.communicate(vel + ' ' + time)
    

VEL = str(VEL)
TIME = str(int(TIME))
print(VEL, TIME)

end_time = dt.datetime.now() +dt.timedelta(hours=16)

# rotation speed in [Hz]
vel = 2

while dt.datetime.now() < end_time:
    # duration [s]
    time = int(2*60*60)
    print('Spinning another 2 hours')
    vel = str(vel)
    time = str(time)
    proc = subprocess.Popen(fn, stdin=subprocess.PIPE, text=True)
    proc.communicate(vel + ' ' + time)

proc = subprocess.Popen(fn, stdin=subprocess.PIPE, text=True)
proc.communicate(VEL + ' ' + TIME)