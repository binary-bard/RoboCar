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
uint16_t aveDSValue = 0;
const uint8_t nDSSamples = 16;
const uint8_t ledPin = PC13;

const uint8_t flPlus = PA2, flNeg = PA3, flPWM  = PB7;
const uint8_t rlPlus = PA0, rlNeg = PA1, rlPWM  = PB6;
const uint8_t frPlus = PA4, frNeg = PA5, frPWM = PB8;
const uint8_t rrPlus = PA6, rrNeg = PA7, rrPWM = PB9;

const int testInterval = 4000;
const int printInterval = 2000;
const int blinkInterval = 200;

int8_t iStep, nSteps = 16;
uint8_t stepDelta = 128 / nSteps;

int nLoops = 0;

// We don't need to hold a lot of entries
const uint8_t BUF_LENGTH = 32;
char lineBuf[BUF_LENGTH];

int8_t curThrottle = 0;
int8_t curSteering = 0;

unsigned long startTime, lastPrintTime, lastBlinkTime;

L298MotorControl flMC, frMC, rlMC, rrMC;
FourWheelDrive myCar;

uint8_t blinkCount = 0;
void blinkLed(uint8_t count)
{
  // Set count
  blinkCount = count;
  lastBlinkTime = millis();
}

void doBlinkLed()
{
  static bool lastBlinkVal = LOW;
  if (blinkCount && millis() - lastBlinkTime > blinkInterval) {
    lastBlinkVal = !lastBlinkVal; // toggle
    digitalWrite(ledPin, lastBlinkVal);
    lastBlinkTime = millis();
    if (lastBlinkVal == LOW)
      --blinkCount;
  }
}

void testThrottle() {
  if (millis() - startTime >= testInterval) {
    ++iStep;
    if (iStep > nSteps || (iStep == nSteps && (128 % nSteps == 0)))
      iStep = -nSteps;
    if (iStep < 0)
      blinkLed(-iStep);
    else if (iStep > 0)
      blinkLed(iStep);
    Serial.print("Throttle is ");
    Serial.println(iStep * stepDelta);
    myCar.setThrottle(iStep * stepDelta);
    startTime = millis();
  }
}

void printStatus() {
  Serial.print("nLoops in ");
  Serial.print(printInterval / 1000);
  Serial.print("s = ");
  Serial.print(nLoops);
  Serial.print(", DS=");
  Serial.println(aveDSValue);

  Serial.print("TH=");
  Serial.print(myCar.throttle());
  Serial.print(", ST=");
  Serial.println(myCar.steering());
  myCar.printMotorSpeeds(Serial);

  lastPrintTime = millis();
  nLoops = 0;
}

void setup()
{
  // We will use serial to communicate with Raspberry Pi
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  flMC.setup(flPlus, flNeg, flPWM);
  frMC.setup(frPlus, frNeg, frPWM);
  rlMC.setup(rlPlus, rlNeg, rlPWM);
  rrMC.setup(rrPlus, rrNeg, rrPWM);
  myCar.setup(flMC, frMC, rlMC, rrMC);
  blinkLed(4);
  while (blinkCount)
    doBlinkLed();
  iStep = nSteps;
  lastBlinkTime = lastPrintTime = startTime = millis();
  myCar.setThrottle(0);
  printStatus();
}

void loop()
{
  bool showStatus = false;
  ++nLoops;
  doBlinkLed();
  // STM32F103C has 12bit ADC so range is upto 4096
  uint16_t dsRead = analogRead(distSensor);
  // Keep running mean without keeping as many entries
  aveDSValue = (dsRead + (nDSSamples - 1) * aveDSValue) / nDSSamples;
  showStatus = millis() - lastPrintTime > printInterval;

  // Assumption: We can read and process faster than the data arrives on the link
  // Format is TH = val or ST = val
  uint8_t nLines = 0;
  while (Serial.available() > 0 && nLines < 4) {
    // read the incoming values for steering and throttle
    uint8_t n = Serial.readBytesUntil('=', lineBuf, BUF_LENGTH - 1);
    bool isThrottle = false;
    bool isSteering = false;
    for (uint8_t i = 0; i < n - 1; ++i) {
      // Check signature whether it is throttle or steering
      char c1 = lineBuf[i];
      char c2 = lineBuf[i + 1];
      if (c1 == 't' || c1 == 'T') {
        isThrottle = (c2 == 'h' || c2 == 'H');
      } else if (c1 == 's' || c1 == 'S') {
        isSteering = (c2 == 't' || c2 == 'T');
      }
    }
    n = Serial.readBytesUntil('\n', lineBuf, BUF_LENGTH - 1);
    if (n > 0) {
      lineBuf[n] = 0;
      int val = atoi(lineBuf);
      showStatus = true;
      if (isThrottle) {
        curThrottle = val;
      } else if (isSteering) {
        curSteering = val;
      }
    }
    ++nLines;
    if (nLines > 3)
      Serial.println("Inputs too fast");
  }

  // dsRead is sensor Value but not distance
  if (aveDSValue > stopSenseValue) {
    // We are pretty close to obstruction, stop all motors
    myCar.brake();
    blinkLed(5);
  } else {
    myCar.setThrottle(curThrottle);
    myCar.setSteering(curSteering);
  }
  if (showStatus)
    printStatus();
}
