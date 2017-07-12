#!/usr/bin/env python3

import cv2, argparse

parser = argparse.ArgumentParser()
parser.add_argument('-v', '--videofile', help="Video file to use")
parser.add_argument('-l', '--logfile', default='', help="Log file to use for labels")
parser.add_argument('-i', '--imagedir', default='images', help="Image directory to save images to")
args = parser.parse_args()

print(args)

cap = cv2.VideoCapture(args.videofile)
count = 0
success = True
while cap.isOpened() and success:
  success, image = cap.read()
  if(success):
    imageFile = args.imagedir + "/frame%d.jpg" % count
    #print(imageFile)
    cv2.imwrite(imageFile, image)                 # save frame as JPEG file
    count += 1
  if cv2.waitKey(10) == 27:                     # exit if Escape is hit
      break

