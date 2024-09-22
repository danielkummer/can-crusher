# Can Crusher

This can crusher uses a liner motor to slowly but surely crush your tin can.

## Bill of Materials

### Screws
* 17x M3 screws (different lengths)
* 10x M4 screws 10mm
* 4x M5 screws 15mm

### Hardware
* 1x Linear shaft
* 1x Stepper motor with gearbox
* 2x push button sensors
* 1x RGB LED push button
* 1x WS2811S led driver
* 1x stepper motor driver 2A
* 1x Arduino Nano every
* 1x 12V 2A power supply
* 1x power supply connector
* connection cables of different lengths

### Stepper Motor

You'll need a stepper motor with a gearbox to produce a lot of torque.
I've found one here: https://eu.robotshop.com/de/products/12v-17a-667-oz-in-nema-17-bipolarer-schrittmotor 

![](images/12v-17a-667oz-in-nema-23-bipolar-stepper-motor.webp?raw=true)

It's a 12 V, 1,7 A, 667 oz-in NEMA-17 bipolar stepper motor with a 991044/2057 :1 planetary gear ratio.
The important metric however is the **48kg/cm torque** it can produce.

### Linear shaft

This will be a bit a hassle to find probably. I have found mine here: https://techstudio.ch/products/zeberoxyz-150mm-effektive-lnge-linear-schienenfhrung-kugelumlaufspindel-sfu1605-mit-nema17-schrittmotor-fr-diy-cnc-router-teile-x-y-z-linear-stage-aktuatorgrsse150mm-nema17 

![](images/linear-shaft.webp?raw=true)

It has an effective length of 150mm which should be enough for most normal sized tin cans.

![](images/linear-shaft-data-sheet.webp?raw=true)

The shaft came with a Nema 17 stepper motor but it was ungeared and way too weak.

### Push button

The push button is coming from adafruit: https://www.adafruit.com/product/3350 

Diameter: 16mm

### Sensors

I'm using a microswitch breakout board obtained here: https://www.play-zone.ch/de/microswitch-breakout-endstop-active-low.html

![](images/microswitch-breakout.jpg?raw=true)

