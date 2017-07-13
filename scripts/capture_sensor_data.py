#!/usr/bin/env python3

import serial
from threading import Thread
from time import sleep, time
import argparse, logging, subprocess
import arduino_mode

parser = argparse.ArgumentParser()
parser.add_argument('-d', '--device', default='/dev/serial0', help="Device to use for serial connection")
parser.add_argument('-l', '--logfile', default=None, help="Log file to use")
parser.add_argument('-r', "--record_cmd", default=None, help="Command with arguments to record videos")
args = parser.parse_args()

try:
  ser = serial.Serial(args.device, 115200)
  sleep(1)
  ser.flushInput()
except:
  print("Failed to open serial port", args.device)
  quit()

if args.logfile is not None:
  logging.basicConfig(filename=args.logfile, filemode='w', level=logging.DEBUG,
                      format='%(relativeCreated)d %(message)s')
  #logging.Formatter(fmt='%(asctime)s.%(msecs)03d %(message)s', datefmt='%H:%M:%S')

#0 - Pass throttle and steering values from remote to the car and Pi
#1 - Pass throttle and steering values from Pi to the car, ignore remote
#2 - Pass steering values only from remote to the car, good for manual pushing during training
#3 - Pass steering values only from Pi to the car, pass throttle from remote - good for testing

# Set to training mode
#ser.write('m=2'.encode())
#ser.flush()
arduino_mode.set_mode(0)
logging.info(b'{"mode":0, "throttle":0, "steering":0}\n')

bCont = True

def output_function():
  global bCont
  while bCont:
    try:
      read_serial=ser.readline()
      if len(read_serial):
        if args.logfile is not None:
          logging.info(read_serial)
        else:
          print(read_serial)

      #sleep(.02)
    except serial.SerialException:
      pass
    except KeyboardInterrupt:
      bCont = False

thread = Thread(target = output_function)
thread.start()

#Run script to capture videos
try:
  subprocess.call(args.record_cmd.split())
except KeyboardInterrupt:
  pass

bCont = False
# Wait for the other thread to finish
thread.join()
logging.shutdown()
ser.close()

