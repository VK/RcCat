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

#include "../Controller.h"
#include "../IMU.h"
#include "../PID.h"

namespace RcCat {

double flying_pitch, flying_accel_value, flying_target_pitch;

int startDirection = 0;
int stop_acceleration = 1500;
unsigned long max_break_time = 0;

PID falling_PID(&flying_pitch, &flying_accel_value, &flying_target_pitch, 20.0,
                200.0, .00, DIRECT);
void Controller::startFalling() {

  if (start_fly_pitch > 2000.0f) {

    acceleration.writeMicroseconds(900);
    stop_acceleration = 1700;
    flying_accel_value = -500;
    startDirection = -1;
  } else if (start_fly_pitch < -2000.0f) {

    acceleration.writeMicroseconds(2100);
    stop_acceleration = 1300;
    flying_accel_value = +500;
    startDirection = +1;
  } else {
    acceleration.writeMicroseconds(1500);
    stop_acceleration = 1500;
    flying_accel_value = 0;
  }

  flying_pitch = pitch_av;
  flying_target_pitch = 0.0;
  falling_PID.SetOutputLimits(-500, 500);
  falling_PID.SetMode(AUTOMATIC);
}

void Controller::updateFalling() {

  if (blinkCounter > 1) {
    blinkCounter = 0;
    blinkState = true;
    digitalWrite(13, HIGH);
  }

  if (startDirection != 0 && timer_mem[MEMORY_LENGTH - 1] < max_flip_time) {
    flying_pitch = pitch_mem[MEMORY_LENGTH - 1];
    falling_PID.Compute(timer_mem[MEMORY_LENGTH - 1]);
    max_break_time = timer_mem[MEMORY_LENGTH - 1] + speed * 10000;
    if (startDirection * flying_accel_value > 50) {
      acceleration.writeMicroseconds(1500 + flying_accel_value);
    } else {
      acceleration.writeMicroseconds(stop_acceleration);
      startDirection = 0;
    }

  } else {
    if (speed > 0.5 && timer_mem[MEMORY_LENGTH - 1] < max_break_time) {
      acceleration.writeMicroseconds(stop_acceleration);
    } else {
      acceleration.writeMicroseconds(1500);
    }
  }
}
} // namespace RcCat
