#include "imu.h"
#include "config.h"
#include <Wire.h>
#include <math.h>
#include "SparkFun_BNO080_Arduino_Library.h"

BNO080 gimbalIMU;
BNO080 antennaIMU;

static inline bool sane(float v) { return isfinite(v); }

void initIMUs() {
  Wire1.begin();
  Wire1.setTimeout(5);   // prevent I²C lockup if line hangs

  if (gimbalIMU.begin(0x4A, Wire1)) {
    gimbalIMU.enableRotationVector(10); // 100 Hz
    gimbalConnected = false;
    Serial.println("Gimbal IMU connected.");
  } else {
    gimbalConnected = false;
    Serial.println("Gimbal IMU not detected.");
  }

  if (antennaIMU.begin(0x4B, Wire1)) {
    antennaIMU.enableRotationVector(10); // 100 Hz
    antennaConnected = true;
    Serial.println("Antenna IMU connected.");
  } else {
    antennaConnected = false;
    Serial.println("Antenna IMU not detected.");
  }
}

void updateIMUs() {
  // --- gimbal ---
  /*
  if (0) {    //gimbalConnected && gimbalIMU.dataAvailable(
    float r = gimbalIMU.getRoll();
    float p = gimbalIMU.getPitch();
    float y = gimbalIMU.getYaw();
    if (sane(r) && sane(p) && sane(y)) {
      roll_G  = r * RAD_TO_DEG;
      pitch_G = p * RAD_TO_DEG;
      yaw_G   = y * RAD_TO_DEG;
    }
  }
  */

  // --- antenna ---
  if (antennaConnected && antennaIMU.dataAvailable()) {
    float r = antennaIMU.getRoll();
    float p = antennaIMU.getPitch();
    float y = antennaIMU.getYaw();
    if (sane(r) && sane(p) && sane(y)) {
      roll_A  = r * RAD_TO_DEG;
      pitch_A = p * RAD_TO_DEG;
      yaw_A   = y * RAD_TO_DEG;
    }
  }
}
