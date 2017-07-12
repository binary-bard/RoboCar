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

import argparse
import RPi.GPIO as GPIO

STEERING_PIN=23
THROTTLE_PIN=24

def set_mode(mode):
  GPIO.setmode(GPIO.BCM)
  GPIO.setup(STEERING_PIN, GPIO.OUT, initial=GPIO.LOW)
  GPIO.setup(THROTTLE_PIN, GPIO.OUT, initial=GPIO.LOW)
  st_val = mode & 1
  th_val = mode & 2
  if st_val:
    GPIO.output(STEERING_PIN, GPIO.HIGH)
  else:
    GPIO.output(STEERING_PIN, GPIO.LOW)
  if th_val:
    GPIO.output(THROTTLE_PIN, GPIO.HIGH)
  else:
    GPIO.output(THROTTLE_PIN, GPIO.LOW)

if __name__ == "main":
  parser = argparse.ArgumentParser()
  parser.add_argument('mode', type=int, help="Mode to set")
  args = parser.parse_args()
  set_mode(args.mode)