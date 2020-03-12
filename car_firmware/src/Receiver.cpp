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

#include "Receiver.h"
#include "Timer.h"

namespace RcCat {

Receiver receiver;

Receiver::Receiver() {
  speed = 0.0;
  dspeed = 0.0;
  registered_channels = 0;
}

void Receiver::attach(int pinID, volatile void (*f)(int microseconds)) {

  volatile byte *p_PCMSK = (volatile byte *)digitalPinToPCMSK(pinID);
  *p_PCMSK |= _BV(digitalPinToPCMSKbit(pinID));

  volatile byte *p_PCICR = (volatile byte *)digitalPinToPCICR(pinID);
  *p_PCICR |= _BV(digitalPinToPCICRbit(pinID));

  pinMode(pinID, INPUT_PULLUP);

  input_pin_bitMask[registered_channels] = digitalPinToBitMask(pinID);
  p_input_pin_register[registered_channels] =
      portInputRegister(digitalPinToPort(pinID));
  pin_state_new[registered_channels] = LOW;
  pin_state_old[registered_channels] = LOW;
  pulseCounts[registered_channels] = 0;
  pd[registered_channels] = 0;

  t_start[registered_channels] = 0;
  t_start_old[registered_channels] = 0;

  callback_functions[registered_channels] = f;

  Serial.print("register Channel: ");
  Serial.println(registered_channels);
  Serial.print("id: ");
  Serial.println(input_pin_bitMask[registered_channels]);

  registered_channels++;
}

void Receiver::attachSpeed(int pinID) {

  volatile byte *p_PCMSK = (volatile byte *)digitalPinToPCMSK(pinID);
  *p_PCMSK |= _BV(digitalPinToPCMSKbit(pinID));

  volatile byte *p_PCICR = (volatile byte *)digitalPinToPCICR(pinID);
  *p_PCICR |= _BV(digitalPinToPCICRbit(pinID));

  pinMode(pinID, INPUT_PULLUP);

  speed_input_pin_bitMask = digitalPinToBitMask(pinID);
  speed_p_input_pin_register = portInputRegister(digitalPinToPort(pinID));
  speed_pin_state_new = LOW;
  speed_pin_state_old = LOW;
  speed_pulseCounts = 0;
  speed_pd = 0;

  speed_t_start = 0;
  speed_t_start_old = 0;
}

unsigned long Receiver::getMicroseconds(int pinID) {
  uint8_t interrupt_Status = SREG;
  noInterrupts();

  unsigned long pulseCountsCopy = pulseCounts[pinID]; // 0.5us units

  SREG = interrupt_Status;

  return pulseCountsCopy / 2;
}

void Receiver::pinChangeIntISR() {

  unsigned long get_timer = timer.getCount();

  // rpm pin change region
  speed_pin_state_new =
      *speed_p_input_pin_register & speed_input_pin_bitMask ? HIGH : LOW;

  if (speed_pin_state_old != speed_pin_state_new) {
    speed_pin_state_old = speed_pin_state_new; // update the state
    if (speed_pin_state_new == HIGH) {
      speed_t_start = get_timer; // 0.5us units
      speed_pd = speed_t_start -
                 speed_t_start_old; // 0.5us units, the incoming pulse period
      dspeed = -speed;
      speed = 120000.0 / speed_pd;
      dspeed += speed;
      speed_t_start_old = speed_t_start; // 0.5us units; update
    } else                               // pin_state_new == LOW
    {
      speed_pulseCounts = get_timer - speed_t_start; // 0.5us units
    }
  }

  if (get_timer > speed_t_start_old + 3 * speed_pd) {
    speed = 0.0;
    dspeed = 0.0;
  }
  // end rpm pin change region

  for (int pinNr = 0; pinNr < registered_channels; pinNr++) {

    pin_state_new[pinNr] =
        *p_input_pin_register[pinNr] & input_pin_bitMask[pinNr] ? HIGH : LOW;

    if (pin_state_old[pinNr] != pin_state_new[pinNr]) {
      // if the pin state actualy changed, & it was not just noise lasting <
      // ~2~4us
      pin_state_old[pinNr] = pin_state_new[pinNr]; // update the state
      if (pin_state_new[pinNr] == HIGH) {
        t_start[pinNr] = get_timer; // 0.5us units
        pd[pinNr] =
            t_start[pinNr] -
            t_start_old[pinNr]; // 0.5us units, the incoming pulse period
        t_start_old[pinNr] = t_start[pinNr]; // 0.5us units; update
      } else                                 // pin_state_new == LOW
      {
        pulseCounts[pinNr] = get_timer - t_start[pinNr]; // 0.5us units

        if (pulseCounts[pinNr] < 5000 && pulseCounts[pinNr] > 1000) {
          callback_functions[pinNr](pulseCounts[pinNr] / 2);
        }
      }
    }
  }
}

float Receiver::getSpeed() { return speed; }

float Receiver::getDSpeed() { return dspeed; }

// PCINT0_vect is for pins D8 to D13
ISR(PCINT0_vect) { receiver.pinChangeIntISR(); }

// PCINT1_vect is for pins A0 to A5
ISR(PCINT1_vect) { receiver.pinChangeIntISR(); }

// PCINT2_vect is for pins D0 to D7
ISR(PCINT2_vect) { receiver.pinChangeIntISR(); }

} // namespace RcCat
