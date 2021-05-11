import numpy as np
import datetime as dt
import time

import ocs
from ocs import matched_client
from multiprocessing import Process, Event
from threading import Thread
from chopper_client import Chopper

chop_hz = 2  # chopping frequency hz_eff = 4*hz
# 60 hz = 240 hz_eff

chop_time = 60
chop_ctrl = Chopper(timeout=chop_time+10)


def chop( vel, time):
    #event.wait()
    print('{} Running chopper at {} Hz for {} secs'.format(dt.datetime.now(),vel,time))
    print(chop_ctrl.chop_hz(vel, time))
    #resp = chop_ctrl.wait_for_response()
    #print(resp)

tChop = Thread(target=chop, args=([hz, chop_time]))
threads = [tChop]
for thread in threads:
    thread.start()
for thread in threads:
    thread.join()
print('Script complete!')

chop_ctrl.close()
