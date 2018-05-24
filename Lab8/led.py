#!/usr/bin/env python3
"""
This is a program for simple led control on Rasberry Pi3

@author: yaching, Jetudie
@argv[1]: Lighting mode
        (0: All Off, 1: BOARD Pin 3, 2: BOARD Pin 5, 3: BOARD Pin 7)
"""
import RPi.GPIO as GPIO
import sys
import time

def initEnv():
    GPIO.setmode(GPIO.BOARD)

def initPin(pins):
    for ledPin in pins:
        GPIO.setup(ledPin, GPIO.OUT)
        GPIO.output(ledPin, False)

def setupPin(ledPin):
    GPIO.output(ledPin, True)

def setoffPin(pins):
    for ledPin in pins:
        GPIO.output(ledPin, False)

def main():
    ledPins = [3, 5, 7]
    setPin = int(sys.argv[1])
    
    # Initialize environment
    if (setPin == 1 or setPin == 2 or setPin == 3 or setPin == 0):
        initEnv()
        GPIO.setwarnings(False)  
        print("Init Env finished")
        initPin(ledPins)
        if setPin == 0:
            setoffPin(ledPins)
            print("All leds off")
        elif (setPin == 1 or setPin == 2 or setPin == 3):
            setoffPin(ledPins)
            setupPin(ledPins[setPin-1])
            print("Led ", ledPins[setPin-1], " on")
    else:
        print("No pins changed")

    #GPIO.cleanup()

if __name__ == '__main__':
    main()
