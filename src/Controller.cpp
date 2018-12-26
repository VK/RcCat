#include "Controller.h"

#include "IMU.h"
#include "RangeFinder.h"
#include "Timer.h"
#include "Receiver.h"

namespace RcCat
{



  Controller controller;


  Controller::Controller()
  {
    driveState = normal;

    speed = 0.0;
    dspeed = 0.0;

    for(int i = 0; i < MEMORY_LENGTH; i++)
    {
      a_tot_mem[i] = 100.0;
      groundDist_mem[i] = 0;
      timer_mem[i] = 0;
    }
  }



  void Controller::setup(int steering_pin, int acceleration_pin)
  {
    steering_ratio = 1.0;
    acceleration_ratio = 1.0;
    pinMode(13, OUTPUT);
    steering.attach(steering_pin);
    acceleration.attach(acceleration_pin);
    saveMode = true;

  }



  void Controller::updateDriveState()
  {
    //decide driveState
    bool a_tot_smaller_threshold = true;
    bool groundDist_smaller_threshold =  false;

    for(int i = 0; i < MEMORY_LENGTH; i++)
    {
      if(a_tot_mem[i] > MINIMAL_ABSOLUTE_GRAVITY)
      {
        a_tot_smaller_threshold = false;
      }
      if(groundDist_mem[i] < MINIMAL_GROUND_DISTANCE)
      {
        groundDist_smaller_threshold = true;
      }
    }


    if(driveState == falling || driveState == jumping)
    {
      if(groundDist_smaller_threshold)
      {
        startNormal();
        driveState = normal;
      }
    } else {

      if(driveState == elevated && a_tot_mem[MEMORY_LENGTH -1 ] < MINIMAL_ABSOLUTE_GRAVITY)
      {
        driveState = falling;
        startFalling();
      } else if(groundDist_smaller_threshold)
      {
        startNormal();
        driveState = normal;
      } else {

          if(speed > .5)
          {
            driveState = jumping;
            startJumping();
          } else {
            driveState = elevated;
          }

      }

    }  //if flying



  }


  volatile void Controller::updateSteering(int microseconds)
  {
    //initial bridge input
    controller.steering.writeMicroseconds(1400 - (microseconds-1500));
    /*if(controller.driveState == DriveStateType::normal )
    {
      controller.steering.writeMicroseconds(1400 - controller.steering_ratio * (microseconds-1500));
    }*/
  }

  volatile void Controller::updateAcceleration(int microseconds)
  {
    //initial bridge input
    controller.acceleration.writeMicroseconds(microseconds);
    /*if(controller.driveState == DriveStateType::normal)
    {
      if(controller.acceleration_ratio == 1.0f)
      {
        controller.acceleration.writeMicroseconds(microseconds);
      } else {
        controller.acceleration.writeMicroseconds(1500 + controller.acceleration_ratio * (microseconds-1500));
      }
    }
    */
  }

  void Controller::collectData()
  {
    //update floating memory
    for(int i = 0; i < MEMORY_LENGTH-1; i++)
    {
      a_tot_mem[i] = a_tot_mem[i+1];
      groundDist_mem[i] = groundDist_mem[i+1];
      roll_mem[i] = roll_mem[i+1];
      pitch_mem[i] = pitch_mem[i+1];
      timer_mem[i] = timer_mem[i+1];
    }

    //add new values
    a_tot =  imu.getAtot()*100;
    ax_mem = imu.getAx()*100;
    ay_mem = imu.getAy()*100;
    az_mem = imu.getAz()*100;
    a_tot_mem[MEMORY_LENGTH-1] = a_tot;
    groundDist_mem[MEMORY_LENGTH-1] = rangeFinder.getDistance();
    timer_mem[MEMORY_LENGTH-1] = timer.getCount();
    pitch_mem[MEMORY_LENGTH-1] = 10000*imu.getPitch();
    roll_mem[MEMORY_LENGTH-1] = 10000*imu.getRoll();


    int helper;
    roll_av = 0.5* roll_mem[MEMORY_LENGTH-1];
    roll_ch = roll_av;
    helper = 0.5*roll_mem[MEMORY_LENGTH-2];
    roll_av += helper;
    roll_ch -= helper;

    pitch_av = 0.5* pitch_mem[MEMORY_LENGTH-1];
    pitch_ch = pitch_av;
    helper = 0.5*pitch_mem[MEMORY_LENGTH-2];
    pitch_av += helper;
    pitch_ch -= helper;


    speed = receiver.getSpeed();
    dspeed = receiver.getDSpeed();

    steering_receiver = receiver.getMicroseconds(0)-1500;
    acceleration_receiver = receiver.getMicroseconds(1)-1500;

  }


  void Controller::writeData()
  {
    //time
    //Serial.print(timer_mem[MEMORY_LENGTH-1]);
    //Serial.print("\t");

    //receiver_input
    Serial.print(steering_receiver);
    Serial.print("\t");
    Serial.print(acceleration_receiver);
    Serial.print("\t");

    //acceleration_directed
    Serial.print(ax_mem);
    Serial.print("\t");
    Serial.print(ay_mem);
    Serial.print("\t");
    Serial.print(az_mem);
    Serial.print("\t");

    //gyro_input
    Serial.print(a_tot_mem[MEMORY_LENGTH-1]);
    Serial.print("\t");
    Serial.print(pitch_mem[MEMORY_LENGTH-1]);
    Serial.print("\t");
    Serial.print(roll_mem[MEMORY_LENGTH-1]);
    Serial.print("\t");

    //pitch and roll change
    Serial.print("\t");
    Serial.print(pitch_mem[MEMORY_LENGTH-1] - pitch_mem[MEMORY_LENGTH-2]);
    Serial.print("\t");
    Serial.print(roll_mem[MEMORY_LENGTH-1] - roll_mem[MEMORY_LENGTH-2]);
    Serial.print("\t");


    //dist_sensor_input
    Serial.print(groundDist_mem[MEMORY_LENGTH-1]);
    Serial.print("\t");


    //motor_speed
    Serial.print(  speed );
    Serial.print("\t");


    //servo_last_values
    Serial.print(  1400 - steering.readMicroseconds()  );
    Serial.print("\t");
    Serial.print(  acceleration.readMicroseconds() - 1500);
    Serial.print("\t");


    //drive_state
    Serial.println(driveState);





  }

  void Controller::loop()
  {

    collectData();


    updateDriveState();



    //switch the blinking off, if needed
    if(blinkState)
    {
      blinkState = false;
      digitalWrite(13, LOW);
    } else {
      blinkCounter++;
    }


    //switch the controll algorithm
    if(driveState == normal)
    {
      updateNormal();
    }
    else  if(driveState == jumping)
    {
      updateJumping();
    }
    else if(driveState == falling)
    {
      updateFalling();
    }
    else if(driveState == elevated)
    {
      updateElevated();
    }



    writeData();



  }

}
