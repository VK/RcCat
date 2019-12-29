#include "RcCat.h"




void setup() {
  Serial.begin(115200);

  delay(200);
  //start the inertial measurement unit (IMU)
  imu.setup();

  delay(20);
  //start the range finder
  RcCat::rangeFinder.setup(0x29);

  //initialize the timer
  RcCat::timer.setup();

  //start the controller with the servo and acceleration output pins
  RcCat::controller.setup(6,5);

  //initialize the receiver input and the rpm sensor
  RcCat::receiver.attach(9, RcCat::controller.updateSteering);
  RcCat::receiver.attach(8, RcCat::controller.updateAcceleration);
  RcCat::receiver.attachSpeed(7);

  RcCat::controller.writeHeadData();
}

void loop() {

  RcCat::rangeFinder.loop();

  imu.loop();

  RcCat::controller.loop();
}
