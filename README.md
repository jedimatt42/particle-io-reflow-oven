# particle-io-reflow-oven
Toaster reflow oven controller based on particle.io core.

## Warning: This is dangerous. 
You are responsible for choosing this code, reviewing this code, installation and operation of this code. 
It is offered here as a record of what I have chosen to do. I make no warranty or guarantee that it is fit for any purpose. 
Use may result in death. Do not leave unattended. 

## Introduction
This is particle.io code for a 'core' or 'photon' that controls a toaster oven to perform a typical lead solder paste reflow. 

Parts:

* Adafruit - RGB LCD SHIELD KIT W/ 16X2 CHARACTER DISPLAY
* Adafruit - Analog Output K-Type Thermocouple Amplifier - AD8495 
* Thermocouple Type-K Glass Braid Insulated Stainless Steel 
* Buzzer 5V - Breadboard friendly
* Particle Photon or Core
* Solid State Relay for AC ( suitably rated for your toaster oven )
* Toaster oven

The Core is connected with I2C to (d0 & d1) to an Adafruit RGB LCD Shield to provide buttons and 16x2 display.
The library for controlling this is in the particle.io IDE library resources called Adafruit_RGBLCDShield.

The reflow temperature profile interpolation code was taken and adapted from http://reprap.org/wiki/Toaster_Oven_Reflow_Technique

## Operation
Upon powerup, the LCD will show, the mode on the first line, and the second line will display the measured temperature, 
followed by the target temperature. Such as:

```
Ready
20C -> 0C
```

Two buttons are active on the shield. 

The 'Select' changes the mode, rotating between: Ready, Running, and Cooling. 
If in Cooling mode, the program will not allow going to Ready mode until the current temperature has reduced to sufficiently.

The 'Left' button resets the display, useful in the event that there is an I2C communication error. 

When placed into Running mode, there will be a brief single beep. The top line of the display will update to show the time.

```
Running time:230
147C -> 150C
```

The system acts like a thermostat, following the reflow profile coded in as the time, temp sequence in to reflow.ino file.
Different toaster oven elements heat or cool more quickly than others. A slow element will create more lag and overshoot. 
The profile should be adjusted to make sure adequate time is spent at each of the 3 temperature stages.

Once the time has exceeded the profile, the target temperature goes to zero, and the device enters 'Cooling' mode.
Two beeps will indicate the transition.

At that stage, reflow should be complete. 

## Warning
This code is provided as-is, as a record of what I've done, and an illustration for others if they choose to take full responsibility
for their own consequences. This code is not in any way guaranteed to be fit for any purposes. Use at your own risk. 

Seriously, you are playing with fire. Use caution. Be safe. 


