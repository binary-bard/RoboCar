#!/usr/bin/env python3

import cv2, argparse, re, ast, os

def parseLine(log_fh, line_re):
  currentDict = {}
  match = None
  while match is None:
    # Format of log
    # 7549 b'{"mode": 0, "throttle": 0, "steering": 0}\n'
    line = log_fh.readline()
    #print(line, line_re)
    if len(line) == 0:
      return currentDict
    match = line_re.match(line)

  dictStr = '{"msSince": %s, %s}' % (match.group(1), match.group(2))
  currentDict = ast.literal_eval(dictStr)
  #print(currentDict)

  return currentDict

parser = argparse.ArgumentParser()
parser.add_argument('-v', '--videofile', help="Video file to use")
parser.add_argument('-l', '--logfile', default='', help="Log file to use for labels")
parser.add_argument('-i', '--imagedir', default='./images', help="Image directory to save images to")
parser.add_argument('-o', '--offset', type=int, default=0, help="Time offset between logging and video start")
args = parser.parse_args()

#logfile is created before videofile, get offset in ms
offset = int(1000*(os.path.getmtime(args.logfile) - os.path.getmtime(args.videofile))) + args.offset
print(offset)

cap = cv2.VideoCapture(args.videofile)
count = 0
success = True
line_re = re.compile('\s*([0-9]+)\sb\'{([^}]+)}')
with open(args.logfile) as fl:
  logDict = parseLine(fl, line_re)
  # Log is ahead of video file by a few ms
  logts = logDict['msSince'] - offset
  while cap.isOpened() and success:
    success, image = cap.read()
    framets = cap.get(cv2.CAP_PROP_POS_MSEC)
    while(framets > logts):
      prev_logts = logts
      logDict = parseLine(fl, line_re)
      logts = logDict['msSince'] - offset
    print(framets, logts)
    if(success):
      imageFile = args.imagedir + "/im%05d_s%s_t%s.jpg" % (count, logDict["steering"], logDict["throttle"])
      cv2.imwrite(imageFile, image)                 # save frame as JPEG file
      count += 1
    if cv2.waitKey(10) == 27:                     # exit if Escape is hit
        break

