#pragma once
#include <Arduino.h>

// existing extern globals for roll_G, pitch_G, yaw_G etc.
extern float roll_G, pitch_G, yaw_G;
extern float roll_A, pitch_A, yaw_A;

// new extern flags so monostatic.cpp can check connection state
extern bool gimbalConnected;
extern bool antennaConnected;

// function prototypes
void initIMUs();
void updateIMUs();
