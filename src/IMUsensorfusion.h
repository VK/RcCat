#ifndef RCAT_IMU_SENSOR_FUSION_H
#define RCAT_IMU_SENSOR_FUSION_H

//----------------------------------------------------------------------------------------------------
// Variable declaration

extern volatile float q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame

//---------------------------------------------------------------------------------------------------
// Function declarations

void fuseIMU(float gx, float gy, float gz, float ax, float ay, float az, float duration, float twoKp);
float invSqrt(float x);

#endif
//=====================================================================================================
// End of file
//=====================================================================================================
