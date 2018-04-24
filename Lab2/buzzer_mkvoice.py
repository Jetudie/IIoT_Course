#!/usr/bin/env python3
"""
This is a program for simple buzzer frequency control on RasberryPi

@author: Jetudie
@argv[1]: Pin for using
@argv[2]: format "FREQ1:FREQ2:....", Frequencies for display
"""

import RPi.GPIO as GPIO
import time
import sys

def initEnv():
    GPIO.setmode(GPIO.BOARD)

def initPin(pin):
    GPIO.setup(pin, GPIO.OUT)

def startVoice(pin):
    v = GPIO.PWM(pin, 50)
    v.start(50)
    return v

def mkVoice(v, f, d):
    v.ChangeFrequency(f)
    time.sleep(d)

def endVoice(v):
    v.stop()

def main():
    print("Env Init")
    PIN      = int(sys.argv[1])
    DELAY    = 0.1
    FREQ_NUM = []
    for s in sys.argv[2].split(":"):
        FREQ_NUM.append(int(s))
    print("Frequency: ", FREQ_NUM)                  
    initEnv()
    
    print("GPIO Init")
    initPin(PIN)
    v = startVoice(PIN)
    
    print("Playing")
    for f in FREQ_NUM:
        if f > 0:
            print(f)
            mkVoice(v, f, DELAY)

    print("Ending")
    endVoice(v)
    GPIO.cleanup()

if __name__ == "__main__":
    main()