#include "config.h"

// Servo objects
Servo roll_servo;
Servo pitch_servo;

// GNSS
float gnss_lat = 0;
float gnss_lon = 0;
uint8_t gnss_fix = 0;
float gnss_alt = 0;

// IMU
float roll_G = 0, pitch_G = 0,yaw_G = 0 ,  pitch_G_offset = 0;
float roll_A = 0, pitch_A = 0, yaw_A = 0, roll_G_offset = 0;

// Flags
bool gimbalConnected  = false;
bool antennaConnected = false;
bool relay_flag = false;
bool stow_flag = false;
bool radarrelay =false;

// Gimbal state
GimbalState current_state = STOW;
GimbalState operation_mode = STOW;

// Gimbal Inputs
double bearing;
double distance;
float target_pitch = 0;
float target_roll = 0;
float pitch_input = 0;
float roll_input = 0;

// GNSS Timing
float previousMillis = 0.0;
float interval = 1000.0;
float currentMillis = 0;

// SD card
unsigned long last_sd_write_time = 0;
char file_name[20] = "LOG0.TXT";
String file_prefix = "LOG";

// Mission Parameters (loaded from mission.txt)
String gimbal_color = "UNKNOWN";
float TARGET_PITCH = 0;
float TARGET_LAT = 0;
float TARGET_LON = 0;
float TARGET_RELATIVE_TO_SEA_M = 0;
float SEA_LEVEL_REFERENCE_M = 0;
float STOW_ANGLE = 0;
bool rf_enabled = false;


// Servo Init
void initServos() {
  pitch_servo.attach(36);   
  roll_servo.attach(37); 
  pitch_servo.write(servo_center_angle);
  roll_servo.write(STOW_ANGLE);
}

//LEDS
void initLEDs() {
  pinMode(LED1_PIN, OUTPUT);
  digitalWrite(LED1_PIN, HIGH);
}
