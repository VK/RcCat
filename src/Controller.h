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

  public:
    enum DriveStateType { normal = 1, falling = 2, elevated = 3, jumping = 4};
    DriveStateType driveState;
    Servo steering;
    Servo acceleration;
    float steering_ratio;
    float acceleration_ratio;

  public:
    static volatile void updateSteering(int microseconds);
    static volatile void updateAcceleration(int microseconds);

  private:
    int a_tot_mem[MEMORY_LENGTH];
    int pitch_mem[MEMORY_LENGTH];
    int roll_mem[MEMORY_LENGTH];
    int groundDist_mem[MEMORY_LENGTH];
    unsigned long timer_mem[MEMORY_LENGTH];
    int ax_mem;
    int ay_mem;
    int az_mem;

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

    void collectData();

    void updateDriveState();

    //the following functions are moved to extra files in the ControllerStates folder!
    void updateNormal();
    void updateFalling();
    void updateElevated();
    void updateJumping();

    void startNormal();
    void startJumping();
    void startFalling();

    void writeData();

  };

  extern Controller controller;

}
#endif
