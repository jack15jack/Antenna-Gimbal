#include "monostatic.h"
#include "gnss.h"
#include "config.h"
#include "sdcard.h"
#include "imu.h"
#include <SD.h>

// Timing for incremental moves
static unsigned long lastUpdateTime = 0;
static const unsigned long MOVE_INTERVAL_MS = 100; // 1 second

// Current offsets we apply to the servo
static float pitch_offset = 0.0f;
static float roll_offset  = 0.0f;

void handleMonostaticState() {
  float pitch_cmd = 90.0f;
  float roll_cmd  = 90.0f;

  // ---------------- ANTENNA IMU ONLY ----------------
  if (antennaConnected) {
    if (millis() - lastUpdateTime >= MOVE_INTERVAL_MS) {
      lastUpdateTime = millis();

      // Errors between target and antenna IMU
      float pitch_err = TARGET_PITCH - pitch_A;
      float roll_err  = target_roll  - roll_A;

      // Move halfway toward the error
      pitch_offset += pitch_err * 0.5f;
      roll_offset  += roll_err  * 0.5f;
    }

    // Apply offsets
    pitch_cmd += pitch_offset;
    roll_cmd  += roll_offset;
  }

  // ------------- Allain's 20 degree test -----------

  //  pitch_cmd = 110;   //20 degree pitch
  //  roll_cmd = 90;    //0 degree roll

  // Clamp to servo range
  if (pitch_cmd < 0)   pitch_cmd = 0;
  if (pitch_cmd > 180) pitch_cmd = 180;
  if (roll_cmd < 0)    roll_cmd = 0;
  if (roll_cmd > 180)  roll_cmd = 180;

  // Write to servos
  pitch_servo.write(pitch_cmd);
  roll_servo.write(roll_cmd);

  // No delay — loop runs as fast as possible
}
