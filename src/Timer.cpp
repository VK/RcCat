#include "Timer.h"

namespace RcCat
{

  RcCat::Timer timer;

  //internal counter for the interrupts increased every 128 microseconds
  volatile unsigned long rcCat_Timer_interrupt_count;

  //interrupt_count increase once timer overflow
  ISR(TIMER2_OVF_vect)
  {
    rcCat_Timer_interrupt_count++;
  }





  Timer::Timer()
  {
    rcCat_Timer_interrupt_count = 0;
    count = 0;
  }



  void Timer::setup()
  {
    //make timer2 faster
    TCCR2B = (TCCR2B & 0b11111000) | 0x02;

    //Enable the overflow interrupt for timer2
    TIMSK2 |= 0b00000001;

    TCCR2A &= 0b11111100;
    TCCR2B &= 0b11110111;
  }






  //get total count for Timer2
  unsigned long Timer::getCount()
  {
    uint8_t interrupt_Status = SREG;
    noInterrupts(); //switch off interrupts
    uint8_t timer2_cnt = TCNT2; //get counter from timer2

    if (bitRead(TIFR2,0)) { //if there was an overflow in  the meantime
      timer2_cnt = TCNT2;
      rcCat_Timer_interrupt_count++;
      TIFR2 |= 0b00000001;
    }
    count = rcCat_Timer_interrupt_count*256 + timer2_cnt;

    SREG = interrupt_Status;
    return count;
  }



}
