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

#include "Controller.h"

#include "IMU.h"
#include "RangeFinder.h"
#include "Receiver.h"
#include "Timer.h"

int serial_putchar(char c, FILE *f) {
  if (c == '\n')
    serial_putchar('\r', f);
  return Serial.write(c) == 1 ? 0 : 1;
}

namespace RcCat {

Controller controller;

Controller::Controller() {
  driveState = normal;

  speed = 0.0;
  dspeed = 0.0;

  for (int i = 0; i < MEMORY_LENGTH; i++) {
    a_tot_mem[i] = 100.0;
    groundDist_mem[i] = 0;
    timer_mem[i] = 0;
  }

  // setup output
  // Set up stdout
  fdev_setup_stream(&serial_stdout, serial_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = &serial_stdout;
}

void Controller::setup(int steering_pin, int acceleration_pin) {
  steering_ratio = 1.0;
  acceleration_ratio = 1.0;
  pinMode(13, OUTPUT);
  steering.attach(steering_pin);
  acceleration.attach(acceleration_pin);
  saveMode = false;
}

void Controller::updateDriveState() {

  if (driveState == external)
    return;

  // decide driveState
  bool a_tot_smaller_threshold = true;
  bool groundDist_smaller_threshold = false;

  for (int i = 0; i < MEMORY_LENGTH; i++) {
    if (a_tot_mem[i] > MINIMAL_ABSOLUTE_GRAVITY) {
      a_tot_smaller_threshold = false;
    }
    if (groundDist_mem[i] < MINIMAL_GROUND_DISTANCE) {
      groundDist_smaller_threshold = true;
    }
  }

  if (driveState == falling || driveState == jumping) {
    if (groundDist_smaller_threshold) {
      startNormal();
      driveState = normal;
    }
  } else {

    if (driveState == elevated &&
        a_tot_mem[MEMORY_LENGTH - 1] < MINIMAL_ABSOLUTE_GRAVITY) {
      driveState = falling;
      startFalling();
    } else if (groundDist_smaller_threshold) {
      startNormal();
      driveState = normal;
    } else {

      if (speed > .5) {
        driveState = jumping;
        startJumping();
      } else {
        driveState = elevated;
      }
    }

  } // if flying
}

volatile void Controller::updateSteering(int microseconds) {
  // initial bridge input
  // controller.steering.writeMicroseconds(1400 - (microseconds - 1500));
  if (controller.driveState == DriveStateType::normal) {
    controller.steering.writeMicroseconds(1400 - controller.steering_ratio *
                                                     (microseconds - 1500));
  }
}

volatile void Controller::updateAcceleration(int microseconds) {
  // initial bridge input
  // controller.acceleration.writeMicroseconds(microseconds);
  if (controller.driveState == DriveStateType::normal) {
    if (controller.acceleration_ratio == 1.0f) {
      controller.acceleration.writeMicroseconds(microseconds);
    } else {
      controller.acceleration.writeMicroseconds(
          1500 + controller.acceleration_ratio * (microseconds - 1500));
    }
  }
}

void Controller::collectData() {
  // update floating memory
  for (int i = 0; i < MEMORY_LENGTH - 1; i++) {
    a_tot_mem[i] = a_tot_mem[i + 1];
    ax_mem[i] = ax_mem[i + 1];
    ay_mem[i] = ay_mem[i + 1];
    az_mem[i] = az_mem[i + 1];
    groundDist_mem[i] = groundDist_mem[i + 1];
    roll_mem[i] = roll_mem[i + 1];
    pitch_mem[i] = pitch_mem[i + 1];
    timer_mem[i] = timer_mem[i + 1];
  }

  // save new values
  a_tot_mem[MEMORY_LENGTH - 1] = imu.getAtot() * 100;
  ax_mem[MEMORY_LENGTH - 1] = imu.getAx() * 100;
  ay_mem[MEMORY_LENGTH - 1] = imu.getAy() * 100;
  az_mem[MEMORY_LENGTH - 1] = imu.getAz() * 100;
  groundDist_mem[MEMORY_LENGTH - 1] = rangeFinder.getDistance();
  timer_mem[MEMORY_LENGTH - 1] = timer.getCount();
  pitch_mem[MEMORY_LENGTH - 1] = 10000 * imu.getPitch();
  roll_mem[MEMORY_LENGTH - 1] = 10000 * imu.getRoll();

  int helper;
  roll_av = 0.5 * roll_mem[MEMORY_LENGTH - 1];
  roll_ch = roll_av;
  helper = 0.5 * roll_mem[MEMORY_LENGTH - 2];
  roll_av += helper;
  roll_ch -= helper;

  pitch_av = 0.5 * pitch_mem[MEMORY_LENGTH - 1];
  pitch_ch = pitch_av;
  helper = 0.5 * pitch_mem[MEMORY_LENGTH - 2];
  pitch_av += helper;
  pitch_ch -= helper;

  speed = receiver.getSpeed();
  dspeed = receiver.getDSpeed();

  steering_receiver = receiver.getMicroseconds(0) - 1500;
  acceleration_receiver = receiver.getMicroseconds(1) - 1500;
}

void Controller::writeHeadData() {

  Serial.println("#Output meas data");
  // receiver_input
  Serial.print("#steering_receiver");
  Serial.print("\t");
  Serial.print("acceleration_receiver");
  Serial.print("\t");

  /// last values
  ///=================
  // acceleration_directed
  Serial.print("ax_0");
  Serial.print("\t");
  Serial.print("ay_0");
  Serial.print("\t");
  Serial.print("az_0");
  Serial.print("\t");
  // gyro_input
  Serial.print("a_tot_0");
  Serial.print("\t");
  Serial.print("pitch_0");
  Serial.print("\t");
  Serial.print("roll_0");
  Serial.print("\t");

  // ground distance is very slow, so we only use the last value
  Serial.print("groundDist");
  Serial.print("\t");

  // motor_speed
  Serial.print("speed");
  Serial.print("\t");

  // servo_last_values
  Serial.print("steering_out");
  Serial.print("\t");
  Serial.print("acceleration_out");
  Serial.print("\t");

  // drive_state
  Serial.println("driveState");
}

void Controller::writeData() {
  printf("%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\n",
         steering_receiver, acceleration_receiver, ax_mem[MEMORY_LENGTH - 1],
         ay_mem[MEMORY_LENGTH - 1], az_mem[MEMORY_LENGTH - 1],
         a_tot_mem[MEMORY_LENGTH - 1], pitch_mem[MEMORY_LENGTH - 1],
         roll_mem[MEMORY_LENGTH - 1], groundDist_mem[MEMORY_LENGTH - 1],
         int(speed * 10), 1400 - steering.readMicroseconds(),
         acceleration.readMicroseconds() - 1500, driveState);
}

void Controller::readData() {
  int value = Serial.read();

  if (value != -1) {

    if (value == 120) {
      driveState = normal;
    } else {

      if (value == 115) {
        int steer = Serial.read();
        int accel = Serial.read();

        if (steer != -1 && accel != -1) {
          commandExternal(steer, accel);
        }
      }

      if (value == 112) {
        int id = Serial.read();
        int value = Serial.read();

        if (id != -1 && value != -1) {
          setParameter(id, value);
        }
      }
    }
  }
}

void Controller::setParameter(int id, int value) {
  // note value ranges form 0 to 255
  // value = 100 shoud deliver values close to optimum
  if (id == 1) {
    p_001_normal_pitch_min = 10.0f * value;
  }
  if (id == 2) {
    p_002_normal_pitch_ratio = 0.3f * value;
  }
  if (id == 3) {
    p_003_normal_acc_offset = 0.012f * value;
  }
  if (id == 4) {
    p_004_normal_acc_min = 0.002f * value;
  }
}

void Controller::loop() {

  collectData();

  updateDriveState();

  // switch the blinking off, if needed
  if (blinkState) {
    blinkState = false;
    digitalWrite(13, LOW);
  } else {
    blinkCounter++;
  }

  // switch the controll algorithm
  if (driveState == normal) {
    updateNormal();
  } else if (driveState == jumping) {
    updateJumping();
  } else if (driveState == falling) {
    updateFalling();
  } else if (driveState == elevated) {
    updateElevated();
  }
  updateExternal();

  writeData();
  readData();
}

} // namespace RcCat
