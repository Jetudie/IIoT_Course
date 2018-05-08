#!/usr/bin/env python3
"""
This program is a simple rfcomm server based on bluetooth connection.
Please check your BlueTooth connection before you start your testing.

@author: Jetudie
@argv[1]: server name
@argv[2]: pins with format "pin1:pin2:pin3"
"""
import RPi.GPIO as GPIO
import signal
import sys
from bluetooth import *


def initEnv():
    GPIO.setmode(GPIO.BOARD)

def initPin(pins):
    pwms = []
    for n in range(len(pins)):
        GPIO.setup(pins[n], GPIO.OUT)
        t_pwm = GPIO.PWM(pins[n], 60)
        t_pwm.start(100)
        pwms.append(t_pwm)
    return pwms

def setPin(pwms, duties):
    for i in range(len(pwms)):
        pwms[i].ChangeDutyCycle(duties[i])


# Service ending handler
def end_service(signal, frame):
    global service_on
    print('[INFO] Ctrl+C captured, shutdown service.')
    service_on = False
    sys.exit(0)


# Client request handler
def handler(sock, info, pwms, duties):
    global service_on
    print("[INFO] Accepted connection from", info)

    try:
        while service_on:
            data_origin = sock.recv(1024)
            if len(data_origin) == 0: break
            
            data = data_origin.decode('ascii')

            Command = str(data.split(" ")[0])
            if Command in 'set':
                Rest = data.split(" ")[1]
                duties.clear()
                duties.append(int(Rest.split(":")[0]))
                duties.append(int(Rest.split(":")[1]))
                duties.append(int(Rest.split(":")[2]))
                setPin(pwms, duties)
                sock.send('Successfully changed!')
            
            if Command in 'get':
                sock.send('Current Values: ' + str(duties[0])+ ":"+ str(duties[1])+":"+str(duties[2]))
                print('Current Values: ' + str(duties[0])+ ":"+ str(duties[1])+":"+str(duties[2]))
            print("[RECV] %s" % data)
    except IOError:
        pass
        

# Env init
UUID = '94f39d29-7d6d-437d-973b-fba39e49d4ee'
# UUID = 'B8:27:EB:FC:D6:8B'
SERVER_NAME = sys.argv[1]
PIN = [int(sys.argv[2].split(":")[0]), int(sys.argv[2].split(":")[1]), int(sys.argv[2].split(":")[2])]
DUTY = [50, 50, 50]
service_on = True

# set environment
initEnv()

# set PINs to DUTYs, respectively
pwms = initPin(PIN)
setPin(pwms, DUTY)

# Add terminator
signal.signal(signal.SIGINT, end_service)


# Create service socket
server_sock = BluetoothSocket(RFCOMM)
server_sock.bind( ('', PORT_ANY) )
server_sock.listen(1)
port = server_sock.getsockname()[1]


# Advertise your service
advertise_service(
    server_sock,
    SERVER_NAME,
    service_id      = UUID,
    service_classes = [ UUID, SERIAL_PORT_CLASS ],
    profiles        = [ SERIAL_PORT_PROFILE ]
)


# Start service
print('[INFO] Service listening at port %d' % port)

while service_on:
    c_sock, c_info = server_sock.accept()
    print('[INFO] Start of connection')
    handler(c_sock, c_info, pwms, DUTY)
    print('[INFO] End of connection')
    c_sock.close()
    
server_sock.close()
print('[INFO] Service shutdown')
