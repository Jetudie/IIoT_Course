# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import numpy as np
import time
import cv2

# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (640, 480)
camera.framerate = 32
rawCapture = PiRGBArray(camera, size=(640, 480))
 
# allow the camera to warmup
time.sleep(0.1)
 
# capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    # grab the raw NumPy array representing the image, then initialize the timestamp
    # and occupied/unoccupied text
    imageBGR = frame.array
    image = cv2.cvtColor(imageBGR, cv2.COLOR_BGR2HSV)
    sensitivity = 20
    # load the image
    #lower_blue = [106,60,90]
    #upper_blue = [124,255,255]
    lower_blue = [120-sensitivity,50,50]
    upper_blue = [120+sensitivity,255,255]

    lower_green = [60-sensitivity,100,100]
    upper_green = [60+sensitivity,255,255]

    lower_red = [0,70,50] #170
    upper_red = [10,255,255] #180

    # loop over the boundaries
    lower_B = np.array(lower_blue, dtype = "uint8")
    upper_B = np.array(upper_blue, dtype = "uint8")
    # find the colors within the specified boundaries and apply the mask
    mask_B = cv2.inRange(image, lower_B, upper_B)
    output_B = cv2.bitwise_and(image, image, mask = mask_B)

    # loop over the boundaries
    lower_G = np.array(lower_green, dtype = "uint8")
    upper_G = np.array(upper_green, dtype = "uint8")
    # find the colors within the specified boundaries and apply the mask
    mask_G = cv2.inRange(image, lower_G, upper_G)
    output_G = cv2.bitwise_and(image, image, mask = mask_G)

    # loop over the boundaries
    lower_R = np.array(lower_red, dtype = "uint8")
    upper_R = np.array(upper_red, dtype = "uint8")
    # find the colors within the specified boundaries and apply the mask
    mask_R = cv2.inRange(image, lower_R, upper_R)
    output_R = cv2.bitwise_and(image, image, mask = mask_R)
    
    # show the frame
    #cv2.imshow("images", np.hstack([imageBGR, output_R, output_G, output_B))
    cv2.imshow("images", imageBGR)
    cv2.imshow("red", output_R)
    cv2.imshow("green", output_G)
    cv2.imshow("blue", output_B)
    #cv2.imshow("Frame", image)
    key = cv2.waitKey(1) & 0xFF

    # clear the stream in preparation for the next frame
    rawCapture.truncate(0)

    # if the `q` key was pressed, break from the loop
    if key == ord("q"):
                break
