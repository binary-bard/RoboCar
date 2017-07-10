My Robocar
==========

This is my attempt to build a self driving car and participate in [DIY Robocars] (https://diyrobocars.com/)

# Hardware #
Here is the *bill of materials* for my car with a total cost of $164
1 [Makerfire 4-wheel Robot Smart Car Chassis Kit - $24] (https://www.amazon.com/gp/product/B00NAT3VF4/)
  * Comes with encoders for arduino
2 [L298N Motor Drive Controller - $10] (https://www.amazon.com/gp/product/B01M29YK5U/)
3 [Raspberry Pi3 - $37] (https://www.amazon.com/Raspberry-Model-A1-2GHz-64-bit-quad-core/dp/B01CD5VC92/) with [PiCam - $15] (https://www.amazon.com/gp/product/B012V1HEP4/)
4 [USB Power pack - $20] (https://www.amazon.com/gp/product/B0188WLTI8/)
5 [1.2V 2200mAh Ni-Cd Sub-C Rechargeable Batteries - $23] (https://www.amazon.com/gp/product/B01FS2EIAW) assembled with 3D printed holders
6 [STM32F103C8T6 as replacement for Arduino - $5] (http://www.ebay.com/itm/STM32F103C8T6-ARM-STM32-Minimum-System-Development-Board-Module-Arduino-M73-/)
  * Has got lot of ADC and PWM pins
7 [GP2Y0A21YK0F Sharp IR Analog Distance Sensor - $8] (http://www.ebay.com/itm/GP2Y0A21YK0F-Sharp-IR-Analog-Distance-Sensor-Distance-Cable-10-80CM-For-Arduino-/)
8 Breadboard for making connections ($2)
9 [Addicore 2 Pack MPU-9250 9-DOF 3-Axis Accelerometer, Gyroscope, & Magnetometer - $20] (https://www.amazon.com/gp/product/B01M8MXSTI/)
10 Wheel encoders (still to buy)

# Software #
Since this is my custom car from scratch, and my chassis is a 4-wheel drive with fixed wheels, I have to calibrate it to run straight or make
appropriate turns via software. I am using STM32F103C8T6 to control the wheels. This is pretty powerful yet cheap chip with [ARM Cortex-M3 MCU 
with 64 Kbytes Flash, 72 MHz CPU] (http://www.st.com/en/microcontrollers/stm32f103c8.html) but is a bit tricky to get started with. You can
find documentation and help at http://www.stm32duino.com.
