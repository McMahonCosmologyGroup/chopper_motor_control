import socket
import sys
import subprocess
import json
import datetime as dt

HOST = '192.168.10.8'
PORT = 3800

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (HOST, PORT)

print(sys.stderr, 'starting up on {} port {}'.format(*server_address) )
s.bind(server_address)

fn = r'C:/scripts/motor_control.py'
size = 1024
backlog = 1
timeout = 60

s.listen(1)

while True:
    client, address = s.accept()
    print("Client accepted")
    while True:
        #data = client.recv(size).rstrip()
        data = client.recv(size).rstrip()
        data = json.loads(data)
        #if 'error' in resp:
         #   raise Exception(resp['error'])
        if not data:
            continue
        print("Received command: {}".format(data) )
        if data == 'disconnect':
            print("Client disconnected.")
            client.send(data)
            client.close()
            break
        if data == 'exit':
            print("Client asked server to quit")
            client.send(data)
            client.close()
        
        print('{} Running chopper at {} Hz for {} secs'.format(dt.datetime.now(),data['vel'], data['time']) )
        VEL = str(data['vel'])
        TIME = str(data['time'])
        try:
            proc = subprocess.Popen(['python',fn,VEL,TIME], stdin=subprocess.PIPE, stdout=subprocess.PIPE, 
                        stderr=subprocess.PIPE, text=True)
            output, errors = proc.communicate()
            proc.wait()
            print(output, errors)
            resp = {'resp':'Chop ending...'}
            msg = json.dumps(resp)
            msg = bytes(msg,'utf-8')
            client.sendall(msg)
        except:
            print('Error occured while executing command {}'.format(data))
            msg = json.dumps({'error':'An error has occured!'})
            msg = bytes(msg,'utf-8')
            client.sendall(msg)