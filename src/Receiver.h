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

#ifndef RcCat_Receiver_h
#define RcCat_Receiver_h

#include <Arduino.h>

namespace RcCat {

// specify the number of receiver channels here, since some
// fixed size arrays are used to handle the data collection
#define RECEIVER_MAX_CHANNELS 4

// Receiver class to handle multiple attached PWM signals.
// Internally the RcCat::Timer is used to measure with a 0.5 microseconds
// accuracy the signal length and fire callbacks once the high state ended.

class Receiver {
public:
  Receiver();

  // register pins to track pwm receiver signals
  void attach(int pinID, volatile void (*callback)(int microseconds));

  // register pins to register RPM signal
  void attachSpeed(int pinID);

  // get the length of the last pwm pulse at a certain pin
  unsigned long getMicroseconds(int pinID);

  // get the speed calculated from the rpm signal
  float getSpeed();
  // get the change of speed of the rpm signal
  float getDSpeed();

  // called by the pinstate changed interrupts
  void pinChangeIntISR();

private:
  byte registered_channels;

  // for all pwm channels
  byte input_pin_bitMask[RECEIVER_MAX_CHANNELS];
  volatile byte *p_input_pin_register[RECEIVER_MAX_CHANNELS];

  volatile unsigned long pulseCounts[RECEIVER_MAX_CHANNELS];
  volatile unsigned long pd[RECEIVER_MAX_CHANNELS];

  volatile boolean pin_state_new[RECEIVER_MAX_CHANNELS];
  volatile boolean pin_state_old[RECEIVER_MAX_CHANNELS];

  volatile unsigned long t_start[RECEIVER_MAX_CHANNELS];     // units of 0.5us
  volatile unsigned long t_start_old[RECEIVER_MAX_CHANNELS]; // units of 0.5us

  volatile void (*callback_functions[RECEIVER_MAX_CHANNELS])(int microseconds);
  // end pwm channels

  // for the rpm sensors
  byte speed_input_pin_bitMask;
  volatile byte *speed_p_input_pin_register;

  volatile unsigned long speed_pulseCounts;
  volatile unsigned long speed_pd;

  volatile boolean speed_pin_state_new;
  volatile boolean speed_pin_state_old;

  volatile unsigned long speed_t_start;     // units of 0.5us
  volatile unsigned long speed_t_start_old; // units of 0.5us
  volatile float speed;
  volatile float dspeed;
  // end for the rpm sensor
};

// a globally used instance of the receiver class
extern Receiver receiver;

} // namespace RcCat

#endif
