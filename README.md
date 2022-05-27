# Holt Experiential - Prezenz-Q

## Scope

PrezenzQ is a sensor or button-triggered experience to queue and play videos on a PC via bluetooth connection.

This project was broken down into two sections:
- [Controllers](#controllers)
- [Video Queueing Application](#video-queueing-application)

## [Controllers](https://github.com/Holt-Environments/Prezenz-Q/tree/master/Controller)

We developed a single controller and replicated it 5 times so that 5 different videos could be controlled/queued.

### Specifications
- Power Requirements (Each controller)
  - 24V, 2A max
  - Because there are five 24V controllers that need powered at once, a special power cord was developed for this that ends in 5 barrel jacks. These 5 barrel jacks connect to the barrel jacks on the power cables that connect to the controller via screw terminals. 
  - Screw terminal polarity:
    - when looking at the device, the left terminal is positive.

## Video Queueing Application

## Resources

- [VL53L1X API User Manual](https://www.pololu.com/file/0J1507/VL53L1X-UM2356.pdf)
