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
