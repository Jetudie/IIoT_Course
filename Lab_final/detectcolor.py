# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import numpy as np
import time
import cv2
import sys

# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (640, 480)
camera.framerate = 32
rawCapture = PiRGBArray(camera, size=(640, 480))
 
# allow the camera to warmup
sensitivity = 20
# load the image
lower_BGR = [(120-sensitivity,50,50), (60-sensitivity,100,100),  (170,70,50)]#170
upper_BGR = [(120+sensitivity,255,255), (60+sensitivity,255,255), (180,255,255)] #180
color = ["Blue", "Green", "Red", "None"]
time.sleep(0.1)
answer = 3
# capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    # grab the raw NumPy array representing the image, then initialize the timestamp
    # and occupied/unoccupied text
    answer = 3
    imageBGR = frame.array
    image = cv2.cvtColor(imageBGR, cv2.COLOR_BGR2HSV)
    for i in range(3): 
        # loop over the boundaries
        lower = np.array(lower_BGR[i], dtype = "uint8")
        upper = np.array(upper_BGR[i], dtype = "uint8")
        # find the colors within the specified boundaries and apply the mask
        mask = cv2.inRange(image, lower, upper)
        output1 = cv2.bitwise_and(image, image, mask = mask)
        output = cv2.cvtColor(output1, cv2.COLOR_HSV2BGR)
        # show the frame
        #cv2.imshow("images", np.hstack([imageBGR, output_R, output_G, output_B))
        cv2.imshow("images", imageBGR)
        cv2.imshow("color", output)
        s0 = sum(sum(output)[250])
        s1 = sum(sum(output)[300])
        s2 = sum(sum(output)[450])
        if s0+s1+s2 > 300:
            answer = i
            break
    #cv2.imshow("Frame", image)
    key = cv2.waitKey(1) & 0xFF
    # clear the stream in preparation for the next frame
    rawCapture.truncate(0)
    print(color[answer])

    # if the `q` key was pressed, break from the loop
    if key == ord("q"):
        break
    if answer != 3:
        break

sys.exit(answer)
