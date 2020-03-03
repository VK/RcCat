#include "../Controller.h"
#include "../IMU.h"

namespace RcCat
{
  int saveModeChangeCounter;

  void Controller::updateElevated()
  {
    imu.setKp(1.0, 1.0);

    if(saveMode && blinkCounter > 20 && blinkCounter < 27)  digitalWrite(13, HIGH);
    if(saveMode && blinkCounter > 27)  digitalWrite(13, LOW);
    if(blinkCounter > 30)
    {
      blinkCounter = 0;
      blinkState = true;
      digitalWrite(13, HIGH);
    }


    max_flip_time =  abs((float)pitch_av * 70.0f);
    max_flip_time += timer_mem[MEMORY_LENGTH-1];
    start_fly_pitch = pitch_av;
    
    acceleration.writeMicroseconds(1500);
    steering.writeMicroseconds(1400);


    if(acceleration_receiver < -400)
    {
      saveModeChangeCounter++;

      if(saveModeChangeCounter > 200)
      {
        saveMode = false;
      }
    } else if (acceleration_receiver > 400)
    {
      saveModeChangeCounter++;
      if(saveModeChangeCounter > 200)
      {
        saveMode = true;
      }
    } else {
      saveModeChangeCounter = 0;
    }


  }

}
