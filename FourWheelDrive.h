/*
  Copyright (c) 2017 - Ajay Guleria
  This is a free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This software is provided WITHOUT ANY WARRANTY; stated or implied. See the
  GNU General Public License for more details. <http://www.gnu.org/licenses/>.
*/

// This class manages the four motors in a four wheel drive
// Motors use L298MotorControl or it could be any generic MotorControl
#include "L298MotorControl.h"

// Results of throttle testing:
// Dead zone 0 - 32

// We have 4 wheel drive
class FourWheelDrive
{
    L298MotorControl *_frontLeft, *_frontRight, *_rearLeft, *_rearRight;
    // _curDir is -ve for left and +ve for right
    int8_t _curThrottle, _curSteering;
    //static int8_t _deadZone = 32;

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
    void setThrottle(int8_t throttle)
    {
      _frontLeft->setSpeed(throttle);
      _frontRight->setSpeed(throttle);
      _rearLeft->setSpeed(throttle);
      _rearRight->setSpeed(throttle);
      _curThrottle = throttle;
      // don't change steering
    }

    // Steering is set between -128 to 127
    void setSteering(int8_t steering)
    {
      // Don't change throttle
      _curSteering = steering;
      _frontLeft->adjust(-steering / 2);
      _frontRight->adjust(steering / 2);
      _rearLeft->adjust(-steering / 2);
      _rearRight->adjust(steering / 2);
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

