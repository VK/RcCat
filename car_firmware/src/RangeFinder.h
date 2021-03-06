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

#ifndef RCAT_RANGEFINDER_H
#define RCAT_RANGEFINDER_H

#include "Arduino.h"

namespace RcCat
{

  class RangeFinder
  {
  public:
    RangeFinder();
    //setup the rangefinde with a i2c address
    void setup(int in_address);

    //call periodically to update the distance
    void loop();

    //get the current distance
    uint8_t getDistance();


  private:
    //called internally to setup the communication with the range finder
    uint8_t internalSetup();

    //address used for the i2c communicating
    int address;

    //time to schedule range sensing interval
    unsigned long last_timer;

    //internal memory of the last distance
    uint8_t distance;
    //internal mmemory of the last distance
    uint8_t old_distance;
    //counter to increase if the distance was exactly the same
    int dist_not_changed;
  };

  extern RangeFinder rangeFinder;
}
#endif
