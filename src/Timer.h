#ifndef RcCat_Timer_h
#define RcCat_Timer_h

#include <Arduino.h>

namespace RcCat
{

  /***
  Timer2 class using timer2 on a higher frequency to measure Rc interrupts more
  accurately.
  The count is increased every 0.5 microseconds!
  */
  class Timer
  {
  public:
    Timer();

    void setup();
    unsigned long getCount();

  private:
    unsigned long count;
  };


  extern Timer timer;
}
#endif
