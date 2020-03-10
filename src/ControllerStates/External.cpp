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
#include <math.h>

namespace RcCat {

unsigned long last_receiver_input = 0;

void Controller::commandExternal(int steer, int accel) {

  if (timer_mem[MEMORY_LENGTH - 1] > last_receiver_input + 10000000) {
    driveState = external;
    int next_steer =
        1400 - min(max(-500, (steer - 128) * 4), 500); // server problem => 1400
    int next_accel = min(max(-500, (accel - 128) * 4), 500) + 1500;

    controller.steering.writeMicroseconds(next_steer);
    controller.acceleration.writeMicroseconds(next_accel);
  }
}

void Controller::updateExternal() {

  if (abs(steering_receiver) > 100 || abs(acceleration_receiver) > 100) {
    last_receiver_input = timer_mem[MEMORY_LENGTH - 1];
    if (driveState == external) {
      driveState = normal;
    }
  }
}
} // namespace RcCat
