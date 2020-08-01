<a href="#"><img src="../../../images/logo_small.png" width="128" height="135" align="right"/></a>

# RcCat Controller States

The `RcCat::Controller` knows 5 different state types, which are automatically chosen depending on sensor data. 

```cpp
enum DriveStateType {
     normal   = 1,
     falling  = 2,
     elevated = 3,
     jumping  = 4,
     external = 5
     };
```
Depending on the status of the `RcCat::Controller`, either the commands from the remote control are passed (directly or modified) to the steering servo or the speed controller, or the control of the car is completely taken over by the `RcCat::Controller`. 
Below is a minimalistic overview of the different drive states.

## Normal
The `DriveStateType::normal` is always active when the car is sufficiently close to the ground and no external commands are triggered.
There is a so-called `saveMode` which capps the the maximum speed with the data of the rpm sensor.
This `saveMode` can be enabled/dissabled by giving full trottle/break once the  car is elevated. This helps to easily pass the car to curious kids.
Outside of the `saveMode` the pitch data of the gyro is used to reduce the throttle to give maximal acceleration without wheelies or rollovers.


## Elevated
The `DriveStateType::elevated` is entered when the ground distance is high and the speed of the car almost  zero. In this state the car steers straight ahead and stops the main motor to prepare for the possible upcoming `DriveStateType::falling`.

## Falling
Once the car is in the `DriveStateType::falling` the main is used to bring the car the level befor reaching the ground. This is done via a small neural network. The next state is `DriveStateType::normal` once the car reaches the ground.


## Jumping
The `DriveStateType::jumping` is entered when the ground distance is higher than a few centimeters and the speed of the car is sufficiently high.
In this drive state the control of the car is completely handeled by the `RcCat::Controller`, which tries to level the car with a throttle interventions.
The controler goes back to `DriveStateType::normal` once the groud distance is small again. 


## External
The `DriveStateType::external` can be entered by sending a command to the microcontroller over the serial bus. The state is only enterd if the radio input was close to zero for the last 5 seconds, and goes back to `DriveStateType::normal` if a radio signal with different drive commands are received.





