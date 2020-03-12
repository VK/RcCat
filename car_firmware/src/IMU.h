#ifndef RCAT_IMU_H
#define RCAT_IMU_H

#include "complementary_filter.h"
#include <Arduino.h>



// Set initial input parameters
enum Ascale {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

enum Gscale {
  GFS_250DPS = 0,
  GFS_500DPS,
  GFS_1000DPS,
  GFS_2000DPS
};

class IMU
{
  public:
    IMU();

  public:
    void setup(uint8_t  gscale = GFS_2000DPS, uint8_t ascale = AFS_8G);
    bool loop();

    ComplementaryFilter filter;


  private:
    uint8_t Gscale = GFS_2000DPS;
    uint8_t Ascale = AFS_8G;

    float aRes;      // scale resolution for the accelerometer
    float gRes;      // scale resolution per rotation
    float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0};      // Bias corrections for gyro and accelerometer
    float ax, ay, az, gx, gy, gz;
    int16_t accelCount[3];  // Stores the 16-bit signed accelerometer sensor output
    int16_t gyroCount[3];   // Stores the 16-bit signed gyro sensor output
    float deltat = 0.0f;          // integration interval for both filter schemes
    uint32_t lastUpdate = 0; // used to calculate integration interval
    uint32_t Now = 0;                         // used to calculate integration interval

  private:
    void calcAres();
    void calcGres();
    void runSelfTest(float * destination);
    void runCalibration(float * dest1, float * dest2);
    void runInit();



  private:
    float pitch;
    float roll;
    float a_tot;
    float a_x;
    float a_y;
    float a_z;

  public:
     float getPitch();
     float getRoll();
     float getAtot();
     float getAx();
     float getAy();
     float getAz();


};


extern IMU imu;
#endif
