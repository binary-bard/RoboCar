#!/usr/bin/env python3

import serial
from threading import Thread
from time import sleep

ser = serial.Serial('/dev/ttyACM0',115200)
inputAvailable = False
entry = ""

def output_function():
  while True:
    read_serial=ser.readline()
    print(read_serial)
    sleep(.02)

thread = Thread(target = output_function)
thread.start()
try:
  while True:
    entry = input("Print value to send: ");
    if len(entry):
      ser.write(entry.encode())
      entry = ""

  thread.join()
except KeyboardInterrupt:
  pass

print('Done')
