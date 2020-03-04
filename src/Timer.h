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

#ifndef RcCat_Timer_h
#define RcCat_Timer_h

#include <Arduino.h>

namespace RcCat {

/// Timer class using timer2 on a 0.5 microseconds interval. This helps to
/// record the interrupts triggered by the radio control more accurately.
class Timer {
public:
  Timer();

  /// setup the timer2 to 0.5 microseconds
  void setup();

  /// get tue current Timer counter
  unsigned long getCount();

private:
  /// the internal timer counter
  unsigned long count;
};

/// a single global timer instance
extern Timer timer;
} // namespace RcCat
#endif
