import socket
import sys
import time
import json

class Chopper:
    def __init__(self, ip_address='192.168.10.8', port=3800, timeout = 10):
        self.ip_address = ip_address
        self.port = port

        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((ip_address, port))
        self.s.settimeout(timeout)

    def send(self, message):
        try:
            msg = json.dumps(message)
            msg = bytes(msg, 'utf-8')
        except:
            msg = json.dumps(message)
        self.s.sendall(msg)
        return self.wait_for_response()

    def close(self):
        self.s.close()

    def wait_for_response(self):
        while True:
            try:
                data = self.s.recv(1024)
                resp = json.loads(data)
                if 'error' in resp:
                    raise Exception(resp['error'])
                return resp['resp']                
            except socket.timeout:
                continue

    def chop_hz(self, velocity, time):
       msg = {'vel': velocity, 'time': time} 
       return self.send(msg)

