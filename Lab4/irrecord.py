#!/usr/bin/env python3
"""
This is a program for recording your IR-device signal.

@author: FATESAIKOU
@argv[1]: data input pin(BOARD)
@argv[2]: the output filename for key_map
"""

import RPi.GPIO as GPIO
import time
import sys
import json

def initEnv(pin):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setwarnings(False)

    GPIO.setup(pin, GPIO.IN)

def endEnv():
    GPIO.cleanup()


def getSignal(pin):
    start, stop = 0, 0

    signals = []

    while True:
        while GPIO.input(pin) == 0:
            None
    
        start = time.time()
    
        while GPIO.input(pin) == 1:
            stop = time.time()
            duringUp = stop - start
            if duringUp > 0.1 and len(signals) > 0:
                return signals[1:]
    
        signals.append(duringUp)
    

def main():
    PIN = int(sys.argv[1])
    OUT_FILE = sys.argv[2]

    initEnv(PIN)

    keys = {}
    while True:
        key_name = input('Please input key name(exit for terminating this program):')
        if key_name == 'exit':
            break

        keys[key_name] = getSignal(PIN)

    endEnv()
    
    src = open(OUT_FILE, 'w')
    src.write(json.dumps(keys))
    src.close()

if __name__ == "__main__":
    main()
