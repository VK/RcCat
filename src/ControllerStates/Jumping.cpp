#include "../Controller.h"
#include "../PID.h"

namespace RcCat
{

  //Define Variables we'll be connecting to
  double jump_target_pitch, jump_pitch,  jump_accel_value;

  unsigned long start_jump_time;

  //Specify the links and initial tuning parameters
  PID jump_speed_PID(&jump_pitch, &jump_accel_value, &jump_target_pitch,.00005,0.0,0.0002, P_ON_E, REVERSE);

  void Controller::startJumping()
  {
    start_jump_time = timer_mem[MEMORY_LENGTH-1];
    jump_pitch = pitch_mem[MEMORY_LENGTH-1];
    jump_pitch *= pitch_mem[MEMORY_LENGTH-1];
    jump_target_pitch = 0.0;
    jump_accel_value = 0;

    jump_speed_PID.SetOutputLimits(-500, 500);

    //turn the PID on
    jump_speed_PID.SetMode(AUTOMATIC);
    jump_speed_PID.Initialize();
  }

  void Controller::updateJumping()
  {

    if(blinkCounter > 10)
    {
      blinkCounter = 0;
      blinkState = true;
      digitalWrite(13, HIGH);
    }


  /*  if(saveMode)
    {
      acceleration.writeMicroseconds(1500);
    }  else*/
   {


      //try to controll the jump if we are not in save mode
      jump_pitch = pitch_mem[MEMORY_LENGTH-1];
      jump_pitch *= pitch_mem[MEMORY_LENGTH-1];
      if(pitch_mem[MEMORY_LENGTH-1] < 0)
      {
        jump_pitch *= -1.0;
      }
      jump_speed_PID.Compute(timer_mem[MEMORY_LENGTH-1]);


      /*Serial.print(jump_pitch);
      Serial.print("\t");
      Serial.println(jump_accel_value);
      */

      steering.writeMicroseconds(1400);



      if(timer_mem[MEMORY_LENGTH-1] - start_jump_time > 2000000)
      {
        acceleration.writeMicroseconds(1500);
      } else {
        acceleration.writeMicroseconds(1500 + jump_accel_value);
      }

    }//if not save mode

  }


}
