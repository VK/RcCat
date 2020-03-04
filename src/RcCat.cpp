/* This file is part of VK RcCat.
 *
 * Copyright 2019 Viktor Krueckl (viktor@krueckl.de). All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ==============================================================================
 */

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
