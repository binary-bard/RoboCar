/*
Copyright (c) 2017 - Ajay Guleria
This is a free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This software is provided WITHOUT ANY WARRANTY; stated or implied. See the
GNU General Public License for more details. <http://www.gnu.org/licenses/>.
*/

// Helper class for L298N Dual H-Bridge Motor Controller
class L298MotorControl
{
  // L298N Dual H-Bridge Motor Controller has 2 pins to control direction
  // and 1 PWM pin to control speed
    uint8_t _posDirPin;
    uint8_t _negDirPin;
    uint8_t _pwmPin;
    // We will use -127 to 127 for speed
    // -ve speed means direction is reverse
    // 0 indicates neutral
    int8_t _curSpeed;

  public:
    L298MotorControl()
      : _posDirPin(0), _negDirPin(0)
      , _pwmPin(0), _curSpeed(0)
    {}

    void setup(const uint8_t pos, const uint8_t neg, const uint8_t pwm)
    {
      _posDirPin = pos;
      _negDirPin = neg;
      _pwmPin = pwm;
      // Only neg and pos are digital pins
      pinMode(pos, OUTPUT);
      pinMode(neg, OUTPUT);
      _curSpeed = 0;
    }

    // We will use -ve speed to go backward
    void setSpeed(int8_t speed)
    {
      _curSpeed = speed; // Save the value
      bool curDir = HIGH;
      if (speed < 0) {
        curDir = LOW;
        speed *= -1; // Make speed +ve
      }
      digitalWrite(_posDirPin, curDir);
      digitalWrite(_negDirPin, !curDir);
      // Use the full 8-bit unsigned range for PWM
      uint8_t range = speed*2;
      analogWrite(_pwmPin, range);
    }

    // Incremental speed changes to adjust for direction
    void adjust(int8_t speed)
    {
      int newSpeed = _curSpeed + speed;
      if(newSpeed > 127) newSpeed = 127;
      if(newSpeed < -127) newSpeed = -127;
      setSpeed(newSpeed);
    }

    // Reduce speed to 0
    // This would need a speed encoder to be successful
    void brake()
    {
      while(_curSpeed) {
        setSpeed(_curSpeed/4);
        delay(5); // Give motors a chance to respond
      }
    }

    bool isStopped() {
      return (_curSpeed == 0);
    }

    int8_t speed()
    {
      return _curSpeed;
    }
};

