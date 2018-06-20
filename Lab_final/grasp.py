
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

def setupPin(ledPin):
    #for ledPin in pins:
        GPIO.output(ledPin, True)

def setoffPin(ledPin):
    #for ledPin in pins:
        GPIO.output(ledPin, False)
def main():
    P = [3,5,7]
    initEnv()
    initPin(P)
    if int(sys.argv[1]) == 0:
        setupPin([P[0],P[1]])
        setoffPin(P[2])
    elif int(sys.argv[1]) == 1:
        setupPin([P[0],P[2]])
        setoffPin(P[1])
    elif int(sys.argv[1]) == 2:
        setupPin([P[1],P[2]])
        setoffPin(P[0])

    time.sleep(5)
    setoffPin(P)
    
    GPIO.cleanup()


if __name__ == '__main__':
    main()
