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

