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

#include "Timer.h"

namespace RcCat {

RcCat::Timer timer;

// internal counter for the interrupts increased every 128 microseconds
volatile unsigned long rcCat_Timer_interrupt_count;

// interrupt_count increase once timer overflow
ISR(TIMER2_OVF_vect) { rcCat_Timer_interrupt_count++; }

Timer::Timer() {
  rcCat_Timer_interrupt_count = 0;
  count = 0;
}

void Timer::setup() {
  // make timer2 faster
  TCCR2B = (TCCR2B & 0b11111000) | 0x02;

  // Enable the overflow interrupt for timer2
  TIMSK2 |= 0b00000001;

  TCCR2A &= 0b11111100;
  TCCR2B &= 0b11110111;
}

// get total count for Timer2
unsigned long Timer::getCount() {
  uint8_t interrupt_Status = SREG;
  noInterrupts();             // switch off interrupts
  uint8_t timer2_cnt = TCNT2; // get counter from timer2

  if (bitRead(TIFR2, 0)) { // if there was an overflow in  the meantime
    timer2_cnt = TCNT2;
    rcCat_Timer_interrupt_count++;
    TIFR2 |= 0b00000001;
  }
  count = rcCat_Timer_interrupt_count * 256 + timer2_cnt;

  SREG = interrupt_Status;
  return count;
}

} // namespace RcCat
