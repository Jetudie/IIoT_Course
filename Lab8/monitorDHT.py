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
@argv[2]: monitor time
"""
import time
import sys
import os
import Adafruit_DHT as DHT


def main():
    BCM_PIN      = int(sys.argv[1])
    MONITOR_TIME = float(sys.argv[2])
    DELAY        = 1

    iter_num = int(MONITOR_TIME / DELAY)

    for i in range(iter_num):
        h, t = DHT.read_retry(11, BCM_PIN)
        
    FILE = open("log.txt", "w")
            
    FILE.write(str(h)+"\t"+str(t)+"\n")

    FILE.close()


    print('Temp={0:0.1f}*C  Humidity={1:0.1f}%'.format(t, h))
    time.sleep(DELAY)

if __name__ == "__main__":
    main()
