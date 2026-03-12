#include "gnss.h"
#include "config.h"
#include <SD.h>

bool sd_initialized = false;

// Forward declaration
bool loadGimbalSDData(const char* filename);

// Helper to stringify mode
static const char* modeToString(GimbalState s) {
  switch (s) {
    case STOW:       return "STOW";
    case MONOSTATIC: return "MONOSTATIC";
    case BISTATIC:   return "BISTATIC";
    default:         return "UNKNOWN";
  }
}

void initSD() {
  // Wait for SD card to be inserted
  Serial.print("Waiting for SD card... ");
  while (!SD.begin(SD_CS)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nSD card detected.");

  int count = 0;
  while (SD.exists(file_name)) {
    count++;
    snprintf(file_name, sizeof(file_name), "%s%d.TXT", file_prefix.c_str(), count);
  }

  File file = SD.open(file_name, FILE_WRITE);
  if (file) {
    file.println("System initialized: GNSS, IMU, SD, Servo");
    file.println("GNSS_UTC_Time,Mode,Latitude,Longitude,Altitude,Yaw_Ant,Pitch_Ant,Roll_Ant,Yaw_Gim,Pitch_Gim,Roll_Gim,RadarRelay");
    file.close();
  }

  sd_initialized = true;
  loadGimbalSDData("mission.txt");
  last_sd_write_time = millis();
}

void writeToSDCard() {
  if (!sd_initialized) return;

  if (millis() - last_sd_write_time > SD_CARD_PERIOD_MS) {
    File file = SD.open(file_name, FILE_WRITE);
    if (file) {
      // --- Time fallback ---
      String time_to_write;
      if (gnss_fix > 0 && gnss_utc_time.length() == 8) {
        time_to_write = gnss_utc_time;
      } else {
        time_to_write = "00:00:00";
      }

      const char* mode_str = modeToString(current_state);

      char buf[300];
      snprintf(buf, sizeof(buf), "%s,%s,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d",
               time_to_write.c_str(), mode_str,
               gnss_lat, gnss_lon, gnss_alt,
               yaw_A, pitch_A, roll_A,
               yaw_G, pitch_G, roll_G,
               radarrelay ? 1 : 0);

      file.println(buf);
      file.close();
    }
    last_sd_write_time = millis();
  }
}

bool loadGimbalSDData(const char* filename) {
  File file = SD.open(filename);
  if (!file) {
    Serial.println("Gimbal SD data file not found.");
    return false;
  }

  if (file.available()) {
    String header = file.readStringUntil('\n');
    header.trim();
    int spaceIndex = header.indexOf(' ');
    if (spaceIndex != -1) {
      gimbal_color = header.substring(0, spaceIndex);
    } else {
      gimbal_color = header;
    }
    gimbal_color.toUpperCase();
    Serial.print("Gimbal color: ");
    Serial.println(gimbal_color);
  }

  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();

    if (line.startsWith("IMU_CAL=")) {
      int eq1 = line.indexOf('=', 8);
      if (eq1 != -1) {
        pitch_G_offset = line.substring(8, eq1).toFloat();
        roll_G_offset = line.substring(eq1 + 1).toFloat();
        Serial.print("IMU Calibration: Pitch Offset=");
        Serial.print(pitch_G_offset);
        Serial.print(", Roll Offset=");
        Serial.println(roll_G_offset);
      }
    } else if (line.startsWith("TARGET_ANGLE=")) {
      TARGET_PITCH = line.substring(13).toFloat();
      Serial.print("target_angle ");
      Serial.println(TARGET_PITCH);
    } else if (line.startsWith("TARGET_LAT=")) {
      TARGET_LAT = line.substring(11).toFloat();
    } else if (line.startsWith("TARGET_LON=")) {
      TARGET_LON = line.substring(11).toFloat();
    } else if (line.startsWith("TARGET_RELATIVE_TO_SEA_M=")) {
      TARGET_RELATIVE_TO_SEA_M = line.substring(25).toFloat();
    } else if (line.startsWith("SEA_LEVEL_REFERENCE_M=")) {
      SEA_LEVEL_REFERENCE_M = line.substring(22).toFloat();
    } else if (line.startsWith("STOW_ANGLE=")) {
      STOW_ANGLE = line.substring(11).toInt();
      Serial.print("stowangle ");
      Serial.println(STOW_ANGLE);
    } else if (line.startsWith("OPERATION_MODE=")) {
      String mode = line.substring(15);
      mode.trim(); mode.toUpperCase();
      if (mode == "MONOSTATIC") {
        operation_mode = MONOSTATIC;
      } else if (mode == "BISTATIC") {
        operation_mode = BISTATIC;
      } else {
        operation_mode = STOW;
      }
    } else if (line.startsWith("RF_ENABLE=")) {
      String val = line.substring(10);
      val.trim(); val.toUpperCase();
      rf_enabled = (val == "YES");
    }
  }

  file.close();
  Serial.println("Gimbal SD data loaded.");
  return true;
}

// used for wireless mission changes
void updateMissionFile(String field, String value){
  File file = SD.open("mission.txt", FILE_READ);

  if (!file){
    Serial.println("Failed to open mission.txt");
  }

  String newFile = "";

  while (file.available()){
    String line = file.readStringUntil('\n');

    if (line.startsWith(field + "=")){
      line = field + "=" + value;

      Serial.print("Updated ");
      Serial.print(field);
      Serial.print(" to ");
      Serial.println(value);
    }

    newFile += line + "\n";
  }

  file.close();

  // create a new file with temporary name
  File tempFile = SD.open("mission_tmp.txt", FILE_WRITE);

  if (!tempFile) {
    Serial.println("Failed to create temp file");
    return;
  }

  tempFile.print(newFile);
  tempFile.close();
   
  //delete old file
  SD.remove("mission.txt");

  //rename temp file
  if (!SD.rename("mission_tmp.txt", "mission.txt")) {
    Serial.println("Rename failed!");
    return;
  }
  

  Serial.println("mission.txt updated");
}