/* This file is part of VK RcCat.
 *
 * Based on Madgwick's implementation of Mayhony's AHRS algorithm.
 * See: http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/
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
