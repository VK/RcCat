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

#ifndef RCAT_CONTROLLER_H
#define RCAT_CONTROLLER_H


#include <Arduino.h>
#include <Servo.h>

namespace RcCat
{


  //configuration for running averages
  #define MEMORY_LENGTH 3
  #define MINIMAL_ABSOLUTE_GRAVITY 20
  #define MINIMAL_GROUND_DISTANCE 65


  class Controller
  {
  public:
    Controller();

  public:
    void setup(int steering_pin, int acceleration_pin);
    void loop();

    void writeHeadData();

  public:
    enum DriveStateType { normal = 1, falling = 2, elevated = 3, jumping = 4, external=5};
    DriveStateType driveState;
    Servo steering;
    Servo acceleration;
    float steering_ratio;
    float acceleration_ratio;
    FILE serial_stdout;

  public:
    static volatile void updateSteering(int microseconds);
    static volatile void updateAcceleration(int microseconds);

  private:
    int a_tot_mem[MEMORY_LENGTH];
    int ax_mem[MEMORY_LENGTH];
    int ay_mem[MEMORY_LENGTH];
    int az_mem[MEMORY_LENGTH];

    int pitch_mem[MEMORY_LENGTH];
    int roll_mem[MEMORY_LENGTH];
    int groundDist_mem[MEMORY_LENGTH];
    unsigned long timer_mem[MEMORY_LENGTH];


    unsigned long max_flip_time;
    int start_fly_pitch;


  private:
    bool blinkState;
    int blinkCounter;
    bool saveMode;

  private:
    int roll_ch;
    int roll_av;
    int pitch_ch;
    int pitch_av;
    int a_tot;
    float speed;
    float dspeed;


  private:
    int steering_receiver;
    int acceleration_receiver;


  private:
    float p_001_normal_pitch_min = 1000.0f;
    float p_002_normal_pitch_ratio = 30.0f;
    float p_003_normal_acc_offset = 1.2f;
    float p_004_normal_acc_min = 0.2f;
    

  private:

    void collectData();

    void updateDriveState();

    //the following functions are moved to extra files in the ControllerStates folder!
    void updateNormal();
    void updateFalling();
    void updateElevated();
    void updateJumping();

    void setParameter(int id, int value);
    void commandExternal(int steer, int accel);
    void updateExternal();

    void startNormal();
    void startJumping();
    void startFalling();

    void writeData();
    void readData();


  };

  extern Controller controller;

}
#endif
