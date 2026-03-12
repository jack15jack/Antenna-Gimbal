//gnss.h
#pragma once

#include <Arduino.h>  // for String
#include <stdint.h>   // for uint8_t

void initGNSS();
void updateGNSS();
String formatGNSSUTC(const String& rawTime);
void parseGGA(const String& sentence);

double get_distance(double lat1, double lon1, double lat2, double lon2);