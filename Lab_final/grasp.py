import RPi.GPIO as GPIO
import random
import time
import sys

def initEnv():
    GPIO.setmode(GPIO.BOARD)
    GPIO.setwarnings(False)

def initPin(pins):
    for ledPin in pins:
        GPIO.setup(ledPin, GPIO.OUT)
def initSwitch(pin):
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, False)

def setupPins(pins):
    for ledPin in pins:
        GPIO.output(ledPin, True)

def setupPin(pin):
    GPIO.output(pin, True)

def setoffPin(ledPin):
    GPIO.output(ledPin, False)

def main():
    P = [3,5,7]
    Switch = 11
    initEnv()
    initPin(P)
    initSwitch(Switch)

    print(sys.argv[1])
    if int(sys.argv[1]) == 0:
        setupPins([P[0],P[1]])
        setoffPin(P[2])
    if int(sys.argv[1]) == 1:
        setupPins([P[0],P[2]])
        setoffPin(P[1])
    if int(sys.argv[1]) == 2:
        setupPins([P[1],P[2]])
        setoffPin(P[0])

    setupPin(Switch)
    time.sleep(0.1)
    setoffPin(Switch)
    time.sleep(3.5)
    
    #GPIO.cleanup()


if __name__ == '__main__':
    main()
