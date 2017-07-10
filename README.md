A Robocar
==========

A version of self driving car that can go around a track on its own based on [DIY Robocars] (https://diyrobocars.com/)

# Hardware #
Here is the *bill of materials* for for this car
1 [1/16 2.4Ghz Exceed RC Magnet EP Electric RTR Off Road Truck Sava Blue - $80] (https://www.amazon.com/gp/product/9269803775). Good thing about this car is that it is pretty modular and you can unplug and move the ESC, switch and RC receiver. 
2 [Spare Lipo batteries - $28] (https://www.amazon.com/gp/product/B01M29YK5U/) - Requires changing the connectors to fit the car.
3 [Raspberry Pi3 - $37] (https://www.amazon.com/Raspberry-Model-A1-2GHz-64-bit-quad-core/dp/B01CD5VC92/) with [PiCam - $15] (https://www.amazon.com/gp/product/B012V1HEP4/)
4 [USB Power pack - $20] (https://www.amazon.com/gp/product/B0188WLTI8/) Will replace it using [Lipo Battery XT60 Plug to USB 5V 2A Charger Converter - $3.6] (http://www.ebay.com/itm/2S-6S-Lipo-Battery-XT60-Plug-to-USB-5V-2A-Charger-Converter-Adapter-for-Phone-PC/112360613018)
5 [Arduino Uno R3 Microcontroller - $19] (https://www.amazon.com/gp/product/B006H06TVG/). It could possible be replaced with this [STM32F103C8T6 as replacement for Arduino - $5] (http://www.ebay.com/itm/STM32F103C8T6-ARM-STM32-Minimum-System-Development-Board-Module-Arduino-M73-/). It has the advantage that it has got lot of ADC and PWM pins
6 3D printed platform to hold the Raspberry Pi, Arduino etc.
7 [GP2Y0A21YK0F Sharp IR Analog Distance Sensor - $8] (http://www.ebay.com/itm/GP2Y0A21YK0F-Sharp-IR-Analog-Distance-Sensor-Distance-Cable-10-80CM-For-Arduino-/)
8 Breadboard for making connections ($2)
9 [Addicore 2 Pack MPU-9250 9-DOF 3-Axis Accelerometer, Gyroscope, & Magnetometer - $20] (https://www.amazon.com/gp/product/B01M8MXSTI/)
10 Wheel encoders (still to buy)

# Software #
For software, I am starting with [donkey car software] (https://github.com/wroscoe/donkey). But, since I am using an Arduino, some modifications are needed to control the motors via arduino. For this, I am using a serial interface between RPi and Arduino. The changes needed are in [my fork] (https://github.com/binary-bard/donkey). This repo contains the scripts and Arduino sketches that do not have a place in the donkey car.
