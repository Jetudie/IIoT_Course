#!/usr/bin/env python3
"""
This is a program for simple led control on RasberryPi

@author: Jetudie
@argv["1:2:3" "..."]: 1:PIN, 2:DUTY, 3:FREQ...
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


def initPin(pins, freq):
    pwms = []
    for n in range(len(pins)):
        GPIO.setup(pins[n], GPIO.OUT)
        t_pwm = GPIO.PWM(pins[n], freq[n])
        t_pwm.start(100)
        pwms.append(t_pwm)

    return pwms
        

def setPin(pwms, duties):
    for i in range(len(pwms)):
        pwms[i].ChangeDutyCycle(duties[i])


def endShining(pwms):
    for pwm in pwms:
        pwm.stop()


def main():
    PINS = []
    DUTIES = []
    FREQ = []
    # Categorize argv
    for s in sys.argv[1:]:
        PINS.append(int(s.split(":")[0]))
        DUTIES.append(int(s.split(":")[1]))
        FREQ.append(int(s.split(":")[2]))

    # Initialize environment
    initEnv()
    print("Init Env finished")

    # Set Pins to corresponding pwm frequency
    pwms = initPin(PINS, FREQ)

    signal.signal(signal.SIGINT, end_handler)

    while shining:
        print("Setup Pins")
        setPin(pwms, DUTIES)

    
    print("Setoff Pins")
    endShining(pwms)
    GPIO.cleanup()


if __name__ == '__main__':
    main()
