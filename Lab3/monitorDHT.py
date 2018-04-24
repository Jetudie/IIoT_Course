#!/usr/bin/env python3
"""
This program is to get DHT11's data with python3 on Raspberry Pi3.
Notice that you have to install Adafruit_DHT module first with bellow:

    $ git clone https://github.com/adafruit/Adafruit_Python_DHT.git
    $ cd Adafruit_DHT
    $ sudo apt-get update
    $ sudo apt-get install build-essential python-dev
    $ sudo python3 setup.py install

@author: Jetudie
@argv[1]: BCM pin id
@argv[2]: Trigger condition
@argv[3]: monitor time
@argv[4]: monitor delay
"""
import subprocess
import time
import sys
import os
import Adafruit_DHT as DHT


def main():
    BCM_PIN      = int(sys.argv[1])
    CONDITION    = str(sys.argv[2])
    MONITOR_TIME = float(sys.argv[3])
    CALLBACK     = str(sys.argv[4])
    DELAY        = 1

    iter_num = int(MONITOR_TIME / DELAY)

    for i in range(iter_num):
        h, t = DHT.read_retry(11, BCM_PIN)
        
        if(eval(CONDITION)):
            sp = subprocess.Popen(CALLBACK, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell = True)
            FILE = open("log.txt", "a")
            out, err = sp.communicate()
            
            if out:
                ou = out.decode("utf-8")
                print("stdout: \n" + ou)
                FILE.write("stdout: \n")
                FILE.write(ou)
            if err:
                er = err.decode("utf-8")
                print("stderr: \n" + er)
                FILE.write("stderr: \n")
                FILE.write(er)
            
            print('return value: ')
            print(sp.returncode)
            FILE.write("return value: " + str(sp.returncode) + '\n')

        print('Temp={0:0.1f}*C  Humidity={1:0.1f}%'.format(t, h))
        time.sleep(DELAY)

if __name__ == "__main__":
    main()
