import subprocess
import sys

import datetime as dt


script, VEL, TIME = sys.argv
fn = r'C:\scripts\motor_sandbox\x64\Debug\motor_sandbox.exe'

VEL = str(VEL)
TIME = str(int(TIME))
print(VEL, TIME)

proc = subprocess.Popen(fn, stdin=subprocess.PIPE, text=True)
proc.communicate(VEL + ' ' + TIME)