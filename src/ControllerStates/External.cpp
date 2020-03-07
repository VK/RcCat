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

namespace RcCat {

int last_external_steer = 1400;
int target_external_acc = 1500;
int target_external_cruise = 1500;
bool handle_task = false;

unsigned long start_external_drive = 0;
unsigned long stop_external_drive = 0;
unsigned long stop_external_task = 0;

void Controller::commandExternal(int value) {

  if (handle_task) {
    return;
  }
  handle_task = true;

  // find the next steering value
  int next_steer = 1400;
  if (value == 113 || value == 97) {
    next_steer = 1800;
  }
  if (value == 101 || value == 100) {
    next_steer = 1000;
  }

  // set the next steering value to the servo
  controller.steering.writeMicroseconds(next_steer);

  // w => 119
  // q => 113
  // e => 101
  if (value == 119 || value == 113 || value == 101) {
    target_external_acc = 1620;
    target_external_cruise = 1550;
  }

  // a => 97
  // s => 115
  // d => 100
  if (value == 97 || value == 115 || value == 100) {
    target_external_acc = 1340;
    target_external_cruise = 1390;
  }

  // decide if the next steering needs an extra wait time for the acceleration
  if (next_steer == last_external_steer) {
    start_external_drive = timer_mem[MEMORY_LENGTH - 1];
    stop_external_drive = timer_mem[MEMORY_LENGTH - 1] + 500000;
    stop_external_task = timer_mem[MEMORY_LENGTH - 1] + 1500000;
  } else {
    start_external_drive = timer_mem[MEMORY_LENGTH - 1] + 300000;
    stop_external_drive = timer_mem[MEMORY_LENGTH - 1] + 800000;
    stop_external_task = timer_mem[MEMORY_LENGTH - 1] + 1800000;
  }
  last_external_steer = next_steer;
}

void Controller::updateExternal() {

  // check if an  acceleratio is allowed
  if (start_external_drive < timer_mem[MEMORY_LENGTH - 1] &&
      timer_mem[MEMORY_LENGTH - 1] < stop_external_drive) {

    if (speed < 0.1) {
      controller.acceleration.writeMicroseconds(target_external_acc);
    } else {
      controller.acceleration.writeMicroseconds(target_external_cruise);
    }
  } else {
    controller.acceleration.writeMicroseconds(1500);
    if (timer_mem[MEMORY_LENGTH - 1] > stop_external_task)
      handle_task = false;
  }
}

// w => 119
// q => 113
// e => 101
// a => 97
// s => 115
// d => 100

// groß => -32

// esc => 27
// enter => 10
} // namespace RcCat
