/*
    Copyright (c) 2017 - Ajay Guleria
	This is a free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This software is provided WITHOUT ANY WARRANTY; stated or implied. See the
	GNU General Public License for more details. <http://www.gnu.org/licenses/>.
*/

// This code is tested on STM32F103C which has 12bit ADC
// and different pin names/numbers. See https://www.stm32duino.com

#include "FourWheelDrive.h"

// Analog pin to which the sensor is connected
const uint8_t distSensor = PB0;
// stopSenseValue is the analog value on pin and not real distance
// Corresponds to about 3ft and higher values mean closer
const uint16_t stopSenseValue = 1000;
const uint8_t ledPin = PC13;

const uint8_t flPlus = PA2, flNeg = PA3, flPWM  = PB7;
const uint8_t rlPlus = PA0, rlNeg = PA1, rlPWM  = PB6;
const uint8_t frPlus = PA4, frNeg = PA5, frPWM = PB8;
const uint8_t rrPlus = PA6, rrNeg = PA7, rrPWM = PB9;

const int testInterval = 4000;
int8_t i, n = 8;
uint8_t delta = 128 / n;

const unsigned long startTime = millis();

L298MotorControl flMC, frMC, rlMC, rrMC;
FourWheelDrive myCar;

void blinkLed(uint8_t count)
{
  for (uint8_t i = 0; i < count; ++i) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  flMC.setup(flPlus, flNeg, flPWM);
  frMC.setup(frPlus, frNeg, frPWM);
  rlMC.setup(rlPlus, rlNeg, rlPWM);
  rrMC.setup(rrPlus, rrNeg, rrPWM);
  myCar.setup(flMC, frMC, rlMC, rrMC);
  blinkLed(4);
  i = n;
}

void loop()
{
  // STM32F103C has 12bit ADC so range is upto 4096
  uint16_t dsRead = analogRead(distSensor);
  Serial.print("Sensor value = ");
  Serial.println(dsRead);
  // dsRead is sensor Value but not distance
  if (dsRead > stopSenseValue) {
    // We are pretty close to obstruction, stop all motors
    myCar.brake();
    Serial.println("Stopped");
    blinkLed(5);
  } else {
    // Move forward for test interval, at 1/3 throttle increments
    ++i;
    Serial.print("Throttle is delta * ");
    Serial.println(i);
    if (i > n || (i == n && (128 % n == 0)))
      i = -n;
    if(i < 0)
      blinkLed(-i);
    else if(i > 0)
      blinkLed(i);
    Serial.print("Throttle is ");
    Serial.println(i * delta);
    myCar.setThrottle(i * delta);
    delay(testInterval);
  }
}
