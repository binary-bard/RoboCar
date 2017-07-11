/*
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
*/

// This class manages the four motors in a four wheel drive
// Motors use L298MotorControl or it could be any generic MotorControl
#include "L298MotorControl.h"

// Results of throttle testing:
// Dead zone 0 - 32

// We have 4 wheel drive but front wheels can turn at an angle
class FourWheelDrive
{
    L298MotorControl *_frontLeft, *_frontRight, *_rearLeft, *_rearRight;
    // _curDir is -ve for left and +ve for right
    int8_t _curThrottle, _curSteering;
    //static int8_t _deadZone = 32;

    // Throttle is set between -128 to 127
    void _setMotorThrottles()
    {
      const int8_t MaxThrottle = 127;
      int leftTh = _curThrottle + _curSteering / 3;
      int rightTh = _curThrottle - _curSteering / 3;
      if (_curSteering < 0)
        rightTh -= _curSteering / 3;
      else
        leftTh += _curSteering / 3;

      // We need to bound the throttles of 2 sides carefully
      if (leftTh > MaxThrottle) {
        // left side is at full throttle ==> implies a right turn
        rightTh -= leftTh - MaxThrottle; // We must adjust the difference to right side
        if (rightTh < -MaxThrottle)
          rightTh = -MaxThrottle;
        leftTh = MaxThrottle;
      } else if (leftTh < -MaxThrottle) {
        // left side is at full reverse throttle ==> implies a left turn
        // This should be a rare condition
        rightTh += -MaxThrottle - leftTh; // Increase the right throttle
        if (rightTh > MaxThrottle)
          rightTh = MaxThrottle;
        leftTh = -MaxThrottle;
      } else if (rightTh > MaxThrottle) {
        leftTh -= rightTh - MaxThrottle;
        if (leftTh < -MaxThrottle)
          leftTh = -MaxThrottle;
        rightTh = MaxThrottle;
      } else if (rightTh < -MaxThrottle) {
        leftTh += -MaxThrottle - rightTh;
        if (leftTh > MaxThrottle)
          leftTh = MaxThrottle;
        rightTh = -MaxThrottle;
      }
      // Change speed in n intervals
      for (uint8_t i = 1; i < 4; ++i) {
        int8_t sp = _frontLeft->speed();
        _frontLeft->setSpeed(sp + i * (leftTh - sp) / 4);
        sp = _rearLeft->speed();
        _rearLeft->setSpeed(sp + i * (leftTh - sp) / 4);
        sp = _frontRight->speed();
        _frontRight->setSpeed(sp + i * (rightTh - sp) / 4);
        sp = _rearRight->speed();
        _rearRight->setSpeed(sp + i * (rightTh - sp) / 4);
        delay(1);
      }
      _frontLeft->setSpeed(leftTh);
      _rearLeft->setSpeed(leftTh);
      _frontRight->setSpeed(rightTh);
      _rearRight->setSpeed(rightTh);
    }

  public:
    FourWheelDrive() : _frontLeft(0), _frontRight(0), _rearLeft(0)
      , _rearRight(0) , _curThrottle(0) {}

    // Don't delete anything you didn't allocate
    ~FourWheelDrive() {}

    void setup(L298MotorControl &fl, L298MotorControl &fr,
               L298MotorControl &rl, L298MotorControl &rr)
    {
      _frontLeft = &fl;
      _frontRight = &fr;
      _rearLeft = &rl;
      _rearRight = &rr;
      _curThrottle = 0;
      _curSteering = 0;
    }

    // Throttle is set between -128 to 127
    void setLeftThrottle(int8_t throttle)
    {
      // Set left throttles
      _frontLeft->setSpeed(throttle);
      _rearLeft->setSpeed(throttle);
      // What is our steering and throttle?
      _curThrottle = (throttle + _frontRight->speed()) / 2;
      _curSteering = (throttle - _frontRight->speed()) / 2;
    }

    // Throttle is set between -128 to 127
    void setRightThrottle(int8_t throttle)
    {
      // Set left throttles
      _frontRight->setSpeed(throttle);
      _rearRight->setSpeed(throttle);
      // What is our steering and throttle?
      _curThrottle = (_frontLeft->speed() + throttle) / 2;
      _curSteering = (_frontLeft->speed() - throttle) / 2;
    }

    // Throttle is set between -128 to 127
    void setThrottle(int8_t throttle)
    {
      _curThrottle = throttle;
      _setMotorThrottles();
      // don't change steering
    }

    // Steering is set between -128 to 127
    void setSteering(int8_t steering)
    {
      // Don't change throttle but limit steering to throttle value
      int8_t stLimit = abs(_curThrottle);
      if (abs(steering) > stLimit) {
        if (steering < 0)
          steering = -stLimit;
        else
          steering = stLimit;
      }
      _curSteering = steering;
      _setMotorThrottles();
    }

    // To brake, we just change the direction, speed stays same
    void brake()
    {
      _frontLeft->brake();
      _frontRight->brake();
      _rearLeft->brake();
      _rearRight->brake();
      _curThrottle = 0;
      _curSteering = 0;
    }

    uint8_t throttle()
    {
      return _curThrottle;
    }

    uint8_t steering()
    {
      return _curSteering;
    }

    void printMotorSpeeds(Stream& st)
    {
      st.print("FL=");
      st.print(_frontLeft->speed());
      st.print(", FR=");
      st.print(_frontRight->speed());
      st.print(", RL=");
      st.print(_rearLeft->speed());
      st.print(", RR=");
      st.println(_rearRight->speed());
    }

    void testControls()
    {
      const int testInterval = 4000;
      unsigned long ts = millis();
      uint8_t delta = 42;

      // Move forward for test interval, at 1/3 throttle increments
      for (uint8_t i = 1; i < 4; ++i) {
        setThrottle(i * delta);
        while (millis() - ts < testInterval);
        ts = millis();
      }
      brake();

      // Move back for test interval, at 1/3 throttle increments
      for (uint8_t i = 1; i < 4; ++i) {
        setThrottle(i * delta);
        while (millis() - ts < testInterval);
        ts = millis();
      }

      // Left & right turns at 1/3 throttle increments
      for (uint8_t i = 1; i < 4; ++i) {
        for (uint8_t j = 1; j < 4; ++j) {
          brake();
          setThrottle(i * delta);
          // Left, neutral and right turns
          for (uint8_t k = -1; k < 2; ++k) {
            setSteering(k * j * delta);
            while (millis() - ts < testInterval);
            ts = millis();
          }
        }
      }
      brake();
    }
};

