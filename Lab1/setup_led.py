#!/usr/bin/env python3
"""
This is a program for simple led control on RasberryPi

@author: Jetudie
@argv[1:]: ALL Pins for using
"""

import RPi.GPIO as GPIO
import sys
import random
import time
import signal

shining = True

def end_handler(signal, frame):
    global shining
    print("end of shining")
    shining = False

def initEnv():
    GPIO.setmode(GPIO.BOARD)

def initPin(pins):
    for ledPin in pins:
        GPIO.setup(ledPin, GPIO.OUT)

def setupPin(ledPin):
    #for ledPin in pins:
        GPIO.output(ledPin, True)

def setoffPin(ledPin):
    #for ledPin in pins:
        GPIO.output(ledPin, False)

def main():
    
    # Create ON and OFF array \
    # to differetiate the PINs by True or False in argv
    ON = []
    OFF = []
    for s in sys.argv[1:]:
        if(s.split(":")[1] == "True"):
            ON.append(int(s.split(":")[0]))
        elif(s.split(":")[1] == "False"):
            OFF.append(int(s.split(":")[0]))
    
    # Initialize environment
    initEnv()
    print("Init Env finished")
    
    # Set Pins
    initPin(ON)
    initPin(OFF)
    print("ON: ", ON)
    print("OFF: ", OFF)
    signal.signal(signal.SIGINT, end_handler)

    while shining:
        setupPin(ON)
        setoffPin(OFF)
            
    GPIO.cleanup()     

if __name__ == '__main__':
    main()
