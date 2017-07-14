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

import argparse, atexit
import RPi.GPIO as GPIO

# These pin controls whether Arduino will pass steering/throttle from Pi
# or the RC remote to the car. HIGH means it will pass Pi values
STEERING_PIN=23
THROTTLE_PIN=24
# This pin controls start and stop of motors. If low, Arduino will not
# pass 0 throttle and steering to car thus stopping it right away.
START_PIN=25

def setup_pins():
  GPIO.setmode(GPIO.BCM)
  GPIO.setup(STEERING_PIN, GPIO.OUT, initial=GPIO.LOW)
  GPIO.setup(THROTTLE_PIN, GPIO.OUT, initial=GPIO.LOW)
  GPIO.setup(START_PIN, GPIO.OUT, initial=GPIO.LOW)

def set_mode(mode):
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

def start_motors():
  GPIO.output(START_PIN, GPIO.HIGH)
  
def stop_motors():
  GPIO.output(START_PIN, GPIO.LOW)

def cleanup():
  print("Resetting arduino mode to training and stopping motors")
  set_mode(0)
  stop_motors()
  GPIO.cleanup([THROTTLE_PIN, STEERING_PIN, START_PIN])

setup_pins()
atexit.register(cleanup)

if __name__ == "main":
  parser = argparse.ArgumentParser()
  parser.add_argument('mode', type=int, help="Mode to set")
  args = parser.parse_args()
  set_mode(args.mode)
