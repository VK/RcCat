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
    void setup(int i2caddress);

    //call periodically to update the distance
    void loop();

    //get the current distance
    uint8_t getDistance();


  private:
    //called internally to setup the communication with the range finder
    uint8_t internalSetup();

    //address used for the i2c communicating
    int _i2caddress;

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
