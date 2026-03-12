//config.h
#pragma once

#include <Arduino.h>
#include <Servo.h>
#include <stdint.h>

// GNSS
extern float gnss_lat;
extern float gnss_lon;
extern uint8_t gnss_fix;
extern float gnss_alt;
extern float previousMillis;
extern float interval;
extern float currentMillis;

// IMU
extern float roll_G, pitch_G, yaw_G, pitch_G_offset;
extern float roll_A, pitch_A, yaw_A, roll_G_offset;

// Flags
extern bool gimbalConnected;
extern bool antennaConnected;
extern bool relay_flag;
extern bool stow_flag;
extern bool  radarrelay;

// Servo pitch constraints
#define servo_pitch_min 0
#define servo_pitch_max 90
#define servo_roll_min 0
#define servo_roll_max 90

// Parameters loaded from SD
extern String gimbal_color;
extern float TARGET_PITCH;
extern float TARGET_LAT;
extern float TARGET_LON;
extern float TARGET_RELATIVE_TO_SEA_M;
extern float SEA_LEVEL_REFERENCE_M;
extern float STOW_ANGLE;
extern bool rf_enabled;

// State enum
enum GimbalState { STOW, MONOSTATIC, BISTATIC };
extern GimbalState current_state;
extern GimbalState operation_mode;

// Servo setup
extern Servo roll_servo;
extern Servo pitch_servo;
void initServos();
extern double bearing;
extern double distance;
extern float target_pitch;
extern float target_roll;
extern float pitch_input;
extern float roll_input;


#define servo_center_angle 90
#define servo_delay 100

// SD
#define SD_CS BUILTIN_SDCARD
extern unsigned long last_sd_write_time;
#define SD_CARD_PERIOD_MS 100
extern char file_name[20];
extern String file_prefix;
extern String gnss_utc_time;

// Relay
#define relay_pin_in1 21
#define relay_pin_in2 20
#define debounce_threshold 10

// RF Module
#define RFM95_CS 10
#define RFM95_RST 35
#define RFM95_INT 34
#define RF95_FREQ 915.0

// LEDs 
void initLEDs();
#define LED1_PIN 19