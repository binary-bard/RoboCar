#!/usr/bin/env python3

'''
MIT License

Copyright (c) 2017 - Ajay Guleria

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
'''

import serial
from threading import Thread
from time import sleep
import argparse, logging

parser = argparse.ArgumentParser()
parser.add_argument('-d', '--device', default='/dev/serial0', help="Device to use for serial connection")
parser.add_argument('-l', '--logfile', default=None, help="Log file to use")
args = parser.parse_args()

try:
  ser = serial.Serial(args.device, 115200)
except:
  print("Failed to open serial port", args.device)
  quit()

if args.logfile is not None:
  logging.basicConfig(filename=args.logfile, level=logging.DEBUG)
  #logging.basicConfig(filename=args.logfile, level=logging.DEBUG, format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')
  logging.Formatter(fmt='%(asctime)s.%(msecs)03d %(message)s', datefmt='%H:%M:%S')

if ser.is_open:
  print(ser.name, "opened")

sleep(2)
ser.flushInput()
inputAvailable = False
entry = ""
bCont = True

def run_test():
  ser.write('m=1\n'.encode())
  for s in [0, 1000, 1600, 1200, 1700, 1300, 1800, 1400, 1900, 0]:
    sstr = 's=' + str(s) + '\n'
    for t in [0, 1200, 0, 1200, 1600, 0, 1700, 1800, 1300, 0, 1300, 0]:
      tstr = 't=' + str(t) + '\n'
      ser.write(sstr.encode())
      ser.write(tstr.encode())
      #Arduino should continue to send out these pulses
      sleep(1)
 

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
      print("Exception happened")
      pass
    except KeyboardInterrupt:
      bCont = False

thread = Thread(target = output_function)
thread.start()
while bCont:
  try:
    entry = input("Print value to send: ");
    if len(entry):
      if entry == 'test':
        run_test()
      else:
        ser.write(entry.encode())
      entry = ""

  except KeyboardInterrupt:
    bCont = False
    ser.write('t=0'.encode())
    ser.write('s=0'.encode())

thread.join()
ser.close()
print('Done')
