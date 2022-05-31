# Holt Experiential - Prezenz-Q

## Scope

PrezenzQ is a sensor or button-triggered experience to queue and play videos on a PC via bluetooth connection.

This project is intended to be used with Windows only.

## Overview

The final state of this research is to have developed 5 controllers that are each connected to an LED strip, a time of flight sensor, and a manual button. The controllers use the time of flight sensor or the manual button to trigger communication with the listening PC via bluetooth. The video queue software running on the PC is connected to all 5 devices at once via bluetooth as serial 'COM' ports. Once the PC receives an 'on' or 'off' communication from a controller, it will queue or dequeue a video to be played. After, the state of the video queued is evaluated (not playing, waiting to play, or playing) and that state value is sent back to the controller. Upon receiving this state value the controller updates the LED strip that is connected to the controller accordingly. 

This project was broken down into two sections:
- [Controllers](#controllers)
  - [Parts](#parts)
  - [Schematics](#schematics)
  - [Notes](#notes)
  - [Specifications](#specifications)
- [Video Queueing Application](#video-queueing-application)

## [Controllers](https://github.com/Holt-Environments/Prezenz-Q/tree/master/Controller)

We developed a single controller and replicated it 5 times so that 5 different videos could be controlled/queued.

For any information in this section going forward, assume this information is only for a single controller, so multiply by however many you need.

### Specifications
- Power 
  - 24V, 2A max
- Communication
  - Serial Bluetooth via HC05 module
- Sensing
  - Teyleten Robot VL53L1X time of flight sensor
- 24V LED strip single channel control

### Parts

- [ELEGOO Nano Board (Arduino Nano)](https://www.amazon.com/ELEGOO-Arduino-ATmega328P-Without-Compatible/dp/B0713XK923/ref=sr_1_5?crid=1YHU7IFWDWYM3&keywords=arduino+nano&qid=1653673195&sprefix=arduino+nano%2Caps%2C80&sr=8-5)
- [HiLetgo HC-05 Wireless Bluetooth RF Transceiver](https://www.amazon.com/HiLetgo-Wireless-Bluetooth-Transceiver-Arduino/dp/B071YJG8DR/ref=sr_1_3?crid=1FC1381EFVF72&keywords=HiLetgo+HC05&qid=1653673262&sprefix=hiletgo+hc05%2Caps%2C72&sr=8-3)
- [Teyleten Robot VL53L1X Sensor](https://www.amazon.com/VL53L1X-Ranging-Distance-Measurement-Extension/dp/B08J1K9T5P/ref=sr_1_1?crid=3AWIUJVVLHADQ&keywords=teyleten+vl53l1x&qid=1653673310&sprefix=teyleten+vl53l1x%2Caps%2C55&sr=8-1)
- [eBoot Mini MP1584EN DC-DC Buck Converter](https://www.amazon.com/MP1584EN-DC-DC-Converter-Adjustable-Module/dp/B01MQGMOKI/ref=sr_1_5?crid=WJY7GMACODM2&keywords=24v+buck+converter&qid=1653673379&sprefix=24v+buck+converte%2Caps%2C69&sr=8-5)
- [Horizontal Slide Switch](https://www.mouser.com/ProductDetail/CK/JS202011AQN?qs=LgMIjt8LuD%252Bmsz6wAeWWtQ%3D%3D)
- [FQU20N06L N-Channel Mosfet](https://www.mouser.com/ProductDetail/onsemi-Fairchild/FQU20N06LTU?qs=bfRUmXT2lZisvT2ROxlO3Q%3D%3D&utm_source=digipart&utm_medium=aggregator&utm_campaign=FQU20N06LTU&utm_term=FQU20N06&utm_content=onsemi) (x2)
- [5 Pin LED Extension and Connector](https://www.amazon.com/Connector-Extension-Conenctor-SIM-NAT/dp/B07D8QCZQL) (x2)
- [22mm Momentary Push Button w/ 24V LED](https://www.amazon.com/Button-Momentary-Latching-Waterproof-Stainless/dp/B09DYHHDK3/ref=sr_1_4?qid=1653502775&refinements=p_n_feature_twenty-five_browse-bin%3A19149019011&rnid=19149011011&s=industrial&sr=1-4&th=1)
- [4 Pin Female Header](https://www.amazon.com/Glarks-Straight-Connector-Assortment-Prototype/dp/B076GZXW3Z/ref=sr_1_5?crid=1UXZ8IESXXOV&keywords=4+pin+female+header&qid=1653674366&s=industrial&sprefix=4+pin+female+header%2Cindustrial%2C58&sr=1-5)
- [24V .8A (.2A per channel) RGBW LED Strip (*We retrofited L-X520436L LED strips purchased from NexmoSphere for this purpose)]()
- [PCB Prototype Board (4x6 cm)](https://www.amazon.com/Smraza-Soldering-Electronic-Compatible-Prototype/dp/B07NM68FXK/ref=sr_1_6?crid=3OZ9J6NGYW6OP&keywords=pcb+prototype+board&qid=1653674398&s=industrial&sprefix=pcb+prototype+board%2Cindustrial%2C62&sr=1-6)
- [2-Channel Screw Terminal](https://www.amazon.com/KeeYees-60pcs-Terminal-Connector-Arduino/dp/B07H5G7GC6/ref=sr_1_5?crid=1RWDX2T8R34GC&keywords=2+channel+screw+terminal&qid=1653674505&s=industrial&sprefix=2+channel+screw+termianl%2Cindustrial%2C64&sr=1-5)
- Resistors
  - 10k Ω (x3)
  - 1k Ω (x2)
  - 576 Ω (x2)
  - 2k Ω
- Wire

### Schematics

The schematics were created with KiCad and can be found in the controller directory of the repo. The schematics are incomplete in that they were designed solely for documentation of the controller creation process. Given the controllers were prototyped/assembled/soldered by hand, the schematics may not be using the appropriate symbols for some of the devices, or may not have any PCB footprint attributed to it (This is something that will be added in the future so that the boards can be printed and shipped).

### Assembly

All of the devices were soldered and assembled by hand.

### Operation

The devices have 2 states: NORMAL and DEBUG. 
- In the NORMAL state, the device operates as it should, detecting sensor or button press, sending a command to the PC, receiving a command from the PC and chaning the LED lighting state, etc. 
- In the DEBUG state, with the Nano on the controller plugged to a PC via usb cable, via serial communication the device's HC05 module can be calibrated using AT commands.

### Notes

- Because there are five 24V controllers that need powered at once, a special power cord was developed for this that ends in 5 barrel jacks. These 5 barrel jacks connect to the barrel jacks on the power cables that connect to the controller via screw terminals. As for screw terminal polarity, when looking at the device with the screw terminal facing you, the left terminal is positive.

- the 24V for the LED isn't techinally 'necessary' and another voltage LED strip could be used. We only went with 24V because that was the rating for the Nexmosphere LED strips we had at the time.
- We only went with screw terminals because we had them on hand. It would be best to have a non-


## [Video Queueing Application](https://github.com/Holt-Environments/Prezenz-Q/tree/arduino_refactor/Queue/Windows%20x64)

The video queueing application was written in C++ using the OpenFrameworks framework so that videos could be loaded, queued and played.

Initially, we wanted to be able to queue and play videos through IntuiFace but that wasn't as responsive as we wanted and we ran into some issues early on that changed our development direction. Our plan was to use Raspberry PI's for the video playing device to keep it small, but even the new Raspberry PI 4's weren't able to keep up when we tried, so we decided to develop this little app ourself to see if it could handle being played on the PI and to also try and eliminate any 'unknowns' that might arise from interfacing with code/applications that we cant control. 

## Resources

- [VL53L1X API User Manual](https://www.pololu.com/file/0J1507/VL53L1X-UM2356.pdf)
