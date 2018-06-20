
import RPi.GPIO as GPIO
import random
import time

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
    initEnv()
    if argv[1] == 0:
        setupPin([3,5])
        setoffPin([7])
    if argv[1] == 1:
        setupPin([3,7])
        setoffPin([5])
    if argv[1] == 2:
        setupPin([5,7])
        setoffPin([3])

    time.sleep(10)
    setupPin(3, 5, 7)
    
    GPIO.cleanup()


if __name__ == '__main__':
    main()
