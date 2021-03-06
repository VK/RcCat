/* This file is part of VK RcCat.
 *
 * Copyright 2020 Viktor Krueckl (viktor@krueckl.de). All Rights Reserved.
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

namespace RcCat {
// Define Variables we'll be connecting to
// double save_target_speed, save_speed,  save_accel_value;
double last_acceleration_receiver;

int last_drive_direction = 0;

void Controller::startNormal() {}

void Controller::updateNormal() {

  if (saveMode && blinkCounter > 80 && blinkCounter < 87)
    digitalWrite(13, HIGH);
  if (saveMode && blinkCounter > 87)
    digitalWrite(13, LOW);
  if (blinkCounter > 100) {
    blinkCounter = 0;
    blinkState = true;
    digitalWrite(13, HIGH);
  }

  if (saveMode) {

    // optimize acceleration

    acceleration_ratio =
        0.175 + 0.125 * tanh(2.0 - speed) - 0.1 * tanh(1.0 - speed * 2);

    if (acceleration_receiver > 50 && pitch_av > 1000.0f) {
      acceleration_ratio =
          min(max(min(1, 1.1 - pitch_ch / 10.0f), 0.2), acceleration_ratio);
    } else if (acceleration_receiver < -50 && pitch_av < -1000.0f) {
      acceleration_ratio =
          min(max(min(1, 1.1 + pitch_ch / 10.0f), 0.2), acceleration_ratio);
    }
    if (speed < 0.07) {
      acceleration_ratio = 1.0;
    }

    steering_ratio = min(1.0, max(0.3, 1.2 - 0.1 * speed));

    // acceleration_ratio =  min(acceleration_ratio,  0.575 + 0.425 * tanh( 2.0
    // - speed ));

  } else {

    steering_ratio = 1.0;

    // optimize acceleration
    if (acceleration_receiver > 50 && pitch_av > p_001_normal_pitch_min) {
      acceleration_ratio = max(
          min(1, p_003_normal_acc_offset - pitch_ch / p_002_normal_pitch_ratio),
          p_004_normal_acc_min);
    } else if (acceleration_receiver < -50 &&
               pitch_av < -p_001_normal_pitch_min) {
      acceleration_ratio = max(
          min(1, p_003_normal_acc_offset + pitch_ch / p_002_normal_pitch_ratio),
          p_004_normal_acc_min);
    } else {
      acceleration_ratio = 1.0;
    }
  }

  if (speed > 0.5 && (dspeed > 0.5 || last_drive_direction == 0)) {
    if (last_acceleration_receiver > 30)
      last_drive_direction = 1;

    if (last_acceleration_receiver < -30)
      last_drive_direction = -1;
  }

  if (speed < 0.01) {
    last_drive_direction = 0;
  }

  if (acceleration_receiver * last_drive_direction < 0) {
    acceleration_ratio = 1.0;
  }

  last_acceleration_receiver = acceleration_receiver;
}

} // namespace RcCat
