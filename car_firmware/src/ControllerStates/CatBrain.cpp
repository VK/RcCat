/* This file is part of VK RcCat.
 *
 * Copyright 2020 Viktor Krueckl (viktor@krueckl.de). All Rights Reserved.
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

#include "../Controller.h"
#include "../IMU.h"

#define LAYER_SIZE 20

namespace RcCat
{

  int const brain_wA[5][LAYER_SIZE] PROGMEM = {
      {27, -13, 15, 19, -25, 16, -15, -5, 2, 3, -48, 48, -1, -24, 14, 48, 3, -1, 5, -3},
      {-30, 2, 4, 2, -1, -17, 0, -5, -7, 49, -1, 8, -3, 0, 0, 1, -29, -7, 50, 52},
      {-3, 13, 7, -13, -6, -18, 3, 7, -7, 1, 7, 9, -13, 6, -10, -6, 0, 1, 0, 0},
      {8, 0, -1, -5, 0, 5, -1, 5, 5, -10, 1, -3, 0, 1, 0, 0, 7, 1, -10, -14},
      {15, -20, -57, -27, -19, -25, -2, 17, -32, -30, -1, -7, -16, 3, -38, 4, -5, 15, -6, 7}};
  int const brain_bA[LAYER_SIZE] PROGMEM = {-780, -195, -80, -433, -1112, -1496, 219, 748, 55, 91, 76, -1613, 220, 30, 495, 107, 163, 61, 83, 25};
  int const brain_wB[LAYER_SIZE][LAYER_SIZE] PROGMEM = {
      {19, -11, -31, 4, 8, 7, -14, -29, 16, 4, -28, -72, -14, -8, -5, 7, -43, -7, -1, -17},
      {-23, -137, -22, -40, -7, -16, -2, -9, -70, -63, 10, 20, -1, 6, 39, -35, -13, 23, -2, 5},
      {-22, -2, 4, -12, 17, -18, 11, 39, -24, 43, 26, -10, 39, -23, -1, -11, 12, -7, -8, 18},
      {-45, 0, -27, 10, 2, -17, 17, -25, -16, -82, 7, 18, -7, -31, -7, 4, -17, -37, -50, 19},
      {-27, -42, -108, -25, 42, -1, -5, -10, -27, -51, -17, 52, -33, 3, 2, -11, -9, 34, 6, 10},
      {-92, -64, -47, 11, -25, -1, -60, -32, -78, 52, 34, -13, -20, -118, -25, 7, -55, -49, 17, 63},
      {18, 14, 15, -33, -56, -9, -7, -4, -6, 15, 7, 9, 2, 2, -14, -65, -16, 11, -21, -6},
      {13, -6, 12, 11, -124, -3, -107, 10, 16, 5, 16, -1, 19, 7, -8, 19, 4, -5, 7, -8},
      {-16, -6, -30, -30, 10, -41, -29, 13, -11, 35, 2, 3, -13, -13, 10, -23, 6, -24, -11, -4},
      {5, -30, -75, 13, 14, 18, 5, 0, -45, 2, -1, -16, -3, -12, -21, 8, -25, -10, -7, -18},
      {-1, -54, 9, -79, -14, -84, 14, 2, -5, -45, -4, 16, 5, 16, -11, -151, 13, -2, 8, 9},
      {-71, -32, 8, 5, -16, 10, -34, -67, -14, 31, -18, 5, -80, -32, -4, 2, 3, 63, -75, 19},
      {-8, -2, 2, -9, 13, -34, 17, -1, 19, -9, 7, -25, -12, 6, 18, -5, -3, 16, 7, -5},
      {-10, -32, -34, 16, 47, 20, -11, -4, 1, 9, -51, -13, -18, -20, 14, -5, -31, -13, 1, 5},
      {19, 7, 24, 11, -4, 28, 6, -19, -21, 5, -59, -34, -49, 5, 8, 13, 3, -4, -36, 9},
      {9, 1, -6, -11, 1, 0, -137, -8, -1, -1, -7, 2, -16, -170, 15, -10, -8, -58, 4, -23},
      {-23, -15, 1, -13, 10, -81, 18, -71, -8, -14, -5, -15, -2, 5, 24, -14, 0, 11, -13, 9},
      {0, -37, -2, 2, -16, -12, -2, -4, 8, -66, -8, 2, 6, 3, 29, 7, 16, 4, 6, 10},
      {-3, 17, -168, 0, 17, 8, 12, -4, 17, -3, 3, 20, 5, 13, -9, 3, 22, -1, -93, 49},
      {-2, -75, 13, -12, -37, -13, -11, -13, -20, 4, 0, -4, 0, 0, 22, -7, -219, 6, 1, -79}};
  int const brain_bB[LAYER_SIZE] PROGMEM = {380, 509, 225, 795, 720, 404, -34, 622, -63, 223, -244, -35, 218, 650, -57, 581, 706, 50, 168, 134};

  int const brain_wD[LAYER_SIZE][2] PROGMEM = {{6, -13},
                                               {-17, 41},
                                               {46, -18},
                                               {18, 7},
                                               {18, -35},
                                               {7, -15},
                                               {70, -25},
                                               {-7, -13},
                                               {-11, -11},
                                               {8, 13},
                                               {-25, 18},
                                               {2, 21},
                                               {-35, -6},
                                               {-38, 10},
                                               {27, -13},
                                               {21, 2},
                                               {-30, 35},
                                               {-42, -6},
                                               {19, -20},
                                               {3, 12}};
  int const brain_bD[2] PROGMEM = {114, -130};

  void Controller::updateCatBrain()
  {
    float dt = (float)(timer_mem[2] - timer_mem[1]) * 5e-7;
    float d_pitch = (float)(pitch_mem[1] - pitch_mem[2]) / dt;
    float d_roll = (float)(roll_mem[1] - roll_mem[2]) / dt;

    long v[5] = {
        (long)((float)(pitch_mem[2]) * -0.0032),
        (long)((float)(roll_mem[2]) * -0.0032),
        (long)(d_pitch * -0.0032),
        (long)(d_roll * -0.0032),
        (long)(speed * 0.32)};

    int i, k;

    // first layer
    int w[LAYER_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (i = 0; i < 5; i++)
    {
      for (k = 0; k < LAYER_SIZE; k++)
      {
        w[k] += v[i] * pgm_read_word(&brain_wA[i][k]);
      }
    }

    for (k = 0; k < LAYER_SIZE; k++)
    {
      w[k] += pgm_read_word(&brain_bA[k]);
      w[k] = max(w[k], 0);
      w[k] /= 32;
    }

    // second layer
    int x[LAYER_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (i = 0; i < LAYER_SIZE; i++)
    {
      for (k = 0; k < LAYER_SIZE; k++)
      {
        x[k] += pgm_read_word(&brain_wB[i][k]) * w[i];
      }
    }
    for (k = 0; k < LAYER_SIZE; k++)
    {
      x[k] += pgm_read_word(&brain_bB[k]);
      x[k] = max(x[k], 0);
      x[k] /= 32;
    }

    // output layer
    int y[2] = {0, 0};
    for (i = 0; i < LAYER_SIZE; i++)
    {
      for (k = 0; k < 2; k++)
      {
        y[k] += pgm_read_word(&brain_wD[i][k]) * x[i];
      }
    }
    for (k = 0; k < 2; k++)
    {
      y[k] += pgm_read_word(&brain_bD[k]);

      y[k] /= 2;
      y[k] = min(500, max(y[k], -500));
    }

    //override manual steer
    y[1] = max(min(1.0, roll_mem[2] * 0.01), -1.0);

    //set the new values for the servo controllers
    controller.steering.writeMicroseconds(1400 - y[1]);
    controller.acceleration.writeMicroseconds(1500 + y[0]);
  }

} // namespace RcCat
