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
int saveModeChangeCounter;

void Controller::updateElevated() {

  // handle blinking of the internal led to signal drive state
  // note: the save mode looks more like a heatbeat
  if (saveMode && blinkCounter > 20 && blinkCounter < 27)
    digitalWrite(13, HIGH);
  if (saveMode && blinkCounter > 27)
    digitalWrite(13, LOW);
  if (blinkCounter > 30) {
    blinkCounter = 0;
    blinkState = true;
    digitalWrite(13, HIGH);
  }

  // part to switch beween save mode
  if (acceleration_receiver < -400) {
    saveModeChangeCounter++;

    if (saveModeChangeCounter > 200) {
      // disable save mode
      saveMode = false;
    }
  } else if (acceleration_receiver > 400) {
    saveModeChangeCounter++;
    if (saveModeChangeCounter > 200) {
      // enable save mode
      saveMode = true;
    }
  } else {
    saveModeChangeCounter = 0;
  }
}

} // namespace RcCat
