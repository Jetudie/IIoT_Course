#!/usr/bin/env python3
"""
This is a program for recognizing infrared signal.

@argv[1]: data input pin(BOARD)
@argv[2]: led output pin(BOARD)
@argv[3]: signal key map
"""

import RPi.GPIO as GPIO
import time
import sys
import json

def initEnv(pin):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setwarnings(False)
    GPIO.setup(pin, GPIO.IN)

def initLed(pin):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setwarnings(False)
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, False)

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


def compairSignal(s1, s2, rang):
    min_len = min(len(s1), len(s2))

    for i in range(min_len):
        if abs(s1[i] - s2[i]) > rang:
            return False
    
    return True

def decodeSingal(s, signal_map, rang):
    for name in signal_map.keys():
        if compairSignal(s, signal_map[name], rang):
            return name

    return None
    
def setupPin(led):
    GPIO.output(led, True)
def setoffPin(led):
    GPIO.output(led, False)

def runPwm(led):
    freq = 100
    duty = 1
    t = 0
    mode = 1
    t_pwm = GPIO.PWM(led, freq)
    while(t < 10):
        t_pwm.start(duty)
        
        duty = duty + mode

        if(duty is 60 or duty is 0):
            mode = -mode

        t = t + 0.01
        time.sleep(0.01)


def main():
    PIN_IR = int(sys.argv[1])
    PIN_LED = int(sys.argv[2])
    SIGNAL_MAP = sys.argv[3]

    src = open(SIGNAL_MAP, 'r')
    signal_map = json.loads(src.read())
    src.close()

    initEnv(PIN_IR)
    initLed(PIN_LED)

    while True:
        print("Please press key")
        s = getSignal(PIN_IR)
        name = decodeSingal(s, signal_map, 0.001)
        print("Youy press: %s" %name)
        #if name is "8":
        #    thread.start_new_thread(blink, args=(LED_PIN), name='123')
        #elif thread
            
        if name is "1":
            setupPin(PIN_LED)
        elif name is "0":
            setoffPin(PIN_LED)
        elif name is "2":
            runPwm(PIN_LED)


    endEnv()

if __name__ == "__main__":
    main()
