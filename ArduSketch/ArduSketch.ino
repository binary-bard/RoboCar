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

#include <Servo.h>

enum TokenType : uint8_t {
  NOTOKEN = 0,
  //RUN_TEST,
  //SET_MODE,
  //PRINT_STATUS,
  THROTTLE,
  STEERING,
  //LEFT_THROTTLE,
  //RIGHT_THROTTLE,
  VALERROR
};

// Steering is channel 1 and throttle is channel 2
byte STEERING_INPIN = A4;
byte THROTTLE_INPIN = A5;
byte STEERING_OUTPIN = 9;
byte THROTTLE_OUTPIN = 11;
byte LED_PIN = 13;

const int ST_ZERO = 1465; // Somewhere around this range
const int TH_ZERO = 1465; // somewhere around this range
const int ST_MAX = 2000;
const int TH_MAX = 2100;
const int ST_MIN = 1000;
const int TH_MIN = 900;
const int TH_SLOW_ZONE = 120;
const int TH_ZERO_ZONE = 30;
const float scale = 1.0;

// Let's use 10 pulses to do PWM, this typically gives a cycle time over 200ms considering
// 1 drive pulse as 20ms. We can decide which pulse goes high or low
const int PWM_PULSES = 10; // use a number that is easily divisible by many

Servo throttle, steering;

/* Mode for our Arduino
    0 - Pass throttle and steering values from remote to the car and Pi, good for training
    1 - Pass steering values only from Pi to the car, pass throttle from remote - good for testing
    2 - Pass steering values only from remote to the car, good when manually pushing
    3 - Pass throttle and steering values from Pi to the car, ignore remote
*/
int iMode = 0;
// Use these pins to control the mode
// 0 - pass value from RC, 1 - pass value from RPi
byte MODE1_PIN = 4; // Controls steering passthrough
byte MODE2_PIN = 7; // Controls throttle passthrough

int iThrottle = TH_ZERO;
int iSteering = ST_ZERO;

bool ledVal = false;
bool send2Pi = true;
bool usePWM = true;
bool debug2Serial = false;
unsigned long lastLedTime;
unsigned long lastPassTime;
unsigned long lastDriveTime;
int nLoops = 0;

const int BUF_LENGTH = 64; // Typical buffer in arduino is 64bits
char readBuf[BUF_LENGTH];
char writeBuf[BUF_LENGTH];

void setThrottle(int thr)
{
  static int lastThrottle = 0;
  static int tPulseCount = 0;

  if (thr == 0)
    thr = TH_ZERO;

  if (!usePWM) {
    throttle.writeMicroseconds(thr);
    return;
  }

  int diffFromLast = thr - lastThrottle;
  if (abs(diffFromLast) > 20)
    tPulseCount = 0; // reset pulse count

  lastThrottle = thr;
  int absThDiff = abs(thr - TH_ZERO);
  int sign = absThDiff / (thr - TH_ZERO); // +1 or -1
  if (absThDiff < TH_ZERO_ZONE) {
    throttle.writeMicroseconds(TH_ZERO);
  } else if (absThDiff >= TH_SLOW_ZONE) {
    // Min and max should be handled by Servo
    // We don't want to run too fast so modulate some of these
    if (tPulseCount % PWM_PULSES < 5)
      throttle.writeMicroseconds(thr);
    else
      throttle.writeMicroseconds(TH_ZERO + sign * TH_SLOW_ZONE);
  } else {
    // We need to use another PWM if throttle value falls in slow zone
    ++tPulseCount; // we get pulse every 5ms
    // We need to see how many PWM_PULSES we need to hold high
    // Since we removed 0 zone, highPulses is at least 1
    int highPulses = absThDiff * PWM_PULSES / TH_SLOW_ZONE;
    // since absThDiff is always less than TH_SLOW_ZONE, there is at least 1 low pulse
    int lowPulses = PWM_PULSES - highPulses;
    if (tPulseCount == 1) {
      // Print first time
      sprintf(writeBuf, "HP=%d, LP=%d\n", highPulses, lowPulses);
      Serial.println(writeBuf);
    }
    // First let's try a simple PWM
    if (tPulseCount < 4 || tPulseCount % PWM_PULSES < highPulses)
      throttle.writeMicroseconds(TH_ZERO + sign * TH_SLOW_ZONE * scale);
    else
      throttle.writeMicroseconds(TH_ZERO + sign * TH_ZERO_ZONE * scale);

    // Current pulse group count
    // int pgCount = tPulseCount * PWM_PULSES / 50; // We assume 50 pulses in 1s or 10 in 200ms
    // Now we need to spread these pulses over the total cycle so determine if this
    // pulse group count should be high or low. To do that, we see how many high
    // pulse groups we can do in PWM_PULSES.
    // We should have first high and then repeat after highPulses/hpg
    //
  }
}

// Steering is handled by servo and we don't have a problem so we let it be as it is
void setSteering(int str)
{
  if (str == 0)
    str = ST_ZERO;
  // Min and max are being handled by Servo
  steering.writeMicroseconds(str);
}

void sendValues(int thr, int str)
{
  // Should RPi control steering or throttle?
  bool passRCSteering = !digitalRead(MODE1_PIN);
  bool passRCThrottle = !digitalRead(MODE2_PIN);

  iMode = (passRCThrottle ? 0 : 2) + (passRCSteering ? 0 : 1);
  if (passRCSteering)
    str = pulseIn(STEERING_INPIN, HIGH);

  usePWM = false;
  if (passRCThrottle) {
    thr = pulseIn(THROTTLE_INPIN, HIGH);
    usePWM = true;
  }

  // Check values every 5ms
  if (millis() - lastDriveTime > 5) {
    setThrottle(thr);
    setSteering(str);
    lastDriveTime = millis();
  }
  // Pass these back to RPi for logging
  if (send2Pi && millis() > lastPassTime + 20) {
    // Use json format
    sprintf(writeBuf, "{\"mode\": %d, \"throttle\": %d, \"steering\": %d}\n", iMode, thr, str);
    Serial.print(writeBuf);
    Serial.flush();
    lastPassTime = millis();
  }
}

bool checkInputLine(TokenType& tt, int& iVal)
{
  tt = NOTOKEN;
  if (Serial.available() < 1)
    return false;

  // read the incoming values for steering and throttle, only one line at a time
  byte n = Serial.readBytesUntil('\n', readBuf, BUF_LENGTH - 1);
  if (n == 0)
    return false;

  /*
    Windows conclusion of this code is that '\n' is read but not included in the string
  */
  char c1 = Serial.peek();
  char c2 = readBuf[n];
  readBuf[n] = 0; // replace '\n' with 0

  // Skip any white space in the beginning
  bool bCont = true;
  uint8_t i;
  for (i = 0; i < n && bCont; ++i) {
    // Check signature whether it is throttle or steering
    // Only one char is sufficient for now
    char c1 = readBuf[i];
    bCont = false;
    if (c1 == 'd' || c1 == 'D') {
      debug2Serial = true;
    } else if (c1 == 'n' || c1 == 'N') {
      debug2Serial = false;
    } else if (c1 == 'i' || c1 == 'I') {
      send2Pi = true;
    } else if (c1 == 'x' || c1 == 'X') {
      send2Pi = false;
    } else if (c1 == 't' || c1 == 'T') {
      tt = THROTTLE;
    } else if (c1 == 's' || c1 == 'S') {
      tt = STEERING;
    } else {
      bCont = true;
    }
  }

  // Skip upto '=' or end of line, whichever comes first
  while (readBuf[i] != '=' && readBuf[i] != 0)
    ++i;

  if (i == n || readBuf[i] != '=')
    // We are done if we processed the whole line or didn't find '='
    return false;

  // process chars after '='
  char* ptr = readBuf + i + 1;
  iVal = atoi(ptr);
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Ready");
  pinMode(THROTTLE_INPIN, INPUT);
  pinMode(STEERING_INPIN, INPUT);
  pinMode(MODE1_PIN, INPUT);
  pinMode(MODE2_PIN, INPUT);

  throttle.attach(THROTTLE_OUTPIN, TH_MIN, TH_MAX);
  steering.attach(STEERING_OUTPIN, ST_MIN, ST_MAX);

  // Write the neutral value
  throttle.writeMicroseconds(TH_ZERO);
  steering.writeMicroseconds(ST_ZERO);
  // We will use it to indicate mode
  pinMode(LED_PIN, OUTPUT);
  lastDriveTime = lastPassTime = lastLedTime = millis();
}

void loop() {
  ++nLoops;

  TokenType tt = NOTOKEN;
  int tokenVal = 0;
  bool bIn = checkInputLine(tt, tokenVal);
  if (bIn) {
    switch (tt) {
      case THROTTLE:
        //iThrottle = TH_MIN + tokenVal*(TH_MAX - TH_MIN)/256;
        iThrottle = tokenVal;
        break;
      case STEERING:
        //iSteering =  ST_MIN + tokenVal*(ST_MAX - ST_MIN)/256;
        iSteering =  tokenVal;
        break;
      case VALERROR:
        Serial.println("Error reading value");
        break;
      case NOTOKEN:
      default:
        Serial.println("Invalid token");
        break;
    }
  }
  // see iMode comments at definition
  sendValues(iThrottle, iSteering);
  if (millis() - lastLedTime > 100 * (1 + iMode)) {
    digitalWrite(LED_PIN, ledVal);
    ledVal = !ledVal;
    lastLedTime = millis();
    //Serial.print("Average loops = ");
    //Serial.println(nLoops);
    nLoops = 0;
  }
}

