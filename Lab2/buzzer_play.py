#!/usr/bin/env python3
"""
This is a program for simple buzzer frequency control on RasberryPi

@author: Jetudie
@argv[1]: Pin for using
@argv[2]: File of frequency 
"""
import RPi.GPIO as GPIO
import time
import json
import sys
import numpy as np

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
    FILE_NAME = sys.argv[2]
    FREQ_NUM = []
    DELAY = []

    initEnv()
    with open(FILE_NAME, 'r') as reader:
        jf = json.loads(reader.read())
    for i in jf:
        FREQ_NUM.append(int(i[0]))
        DELAY.append(float(i[1]))
    print("Frequency: ", FREQ_NUM)
    print("Delay: ", DELAY)
    print("GPIO Init")
    initPin(PIN)
    v = startVoice(PIN)
    
    print("Playing")
    for n in range(len(FREQ_NUM)):
        if(FREQ_NUM[n] > 0):
            print(FREQ_NUM[n])
            mkVoice(v, FREQ_NUM[n], DELAY[n])

    print("Ending")
    endVoice(v)
    GPIO.cleanup()

if __name__ == "__main__":
    main()
