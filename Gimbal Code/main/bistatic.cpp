//bistatic.cpp
#include "bistatic.h"
#include "gnss.h"
#include "config.h"
#include "sdcard.h"
#include "imu.h"
#include <SD.h>
static unsigned long lastUpdateTime = 0;
void handleBistaticState() {
  static const unsigned long MOVE_INTERVAL_MS = 100; // 1 second
  float target_absolute_elevation = SEA_LEVEL_REFERENCE_M + TARGET_RELATIVE_TO_SEA_M;
  float vertical_offset = gnss_alt - target_absolute_elevation;

  distance = get_distance(gnss_lat, gnss_lon, TARGET_LAT, TARGET_LON);
  target_pitch = atan2(vertical_offset, distance) * 180.0 / M_PI;
  target_roll = 0;

  pitch_input = target_pitch;
  roll_input = target_roll;

   // debug
  Serial.print("Target Pitch: ");
  Serial.print(target_pitch, 2);
  Serial.print(" | Target Roll: ");
  Serial.print(target_roll, 2);
  Serial.print(" | IMU Pitch: ");
  Serial.print(distance, 2);
  Serial.print(" | Height: ");
  Serial.print(vertical_offset, 2);
  Serial.print(" | Pitch Error: ");
  Serial.print(pitch_input, 2);
  Serial.print(" | Roll Error: ");
  Serial.println(roll_input, 2);
  Serial.println(gnss_lat, 2);
  Serial.println(gnss_lon, 2);
  Serial.println(TARGET_LAT, 2);
  Serial.println(TARGET_LON, 2);
  
  pitch_input = (90 - target_pitch + roll_A);
  roll_input = pitch_A - target_roll;

  //int pitch_cmd = 90 - pitch_input;
  int roll_err = target_pitch + (roll_A - 90);
 // int pitch_cmd =   - target_pitch - roll_err;//- roll_A;
  int pitch_cmd =    90- roll_err;//- roll_A;
  int roll_cmd  = 90 - roll_input;
 
if (millis() - lastUpdateTime >= MOVE_INTERVAL_MS) {
lastUpdateTime = millis();
  roll_servo.write(pitch_cmd);
  pitch_servo.write(roll_cmd);
}
  Serial.print("Pitch Servo: ");
  Serial.print(pitch_cmd);
  Serial.print(" | Roll Servo: ");
  Serial.println(roll_cmd);
Serial.print(" | Distance: ");
  Serial.println(distance);
  Serial.print(" | roll: ");
  Serial.println(roll_A);
  
  writeToSDCard();

}
