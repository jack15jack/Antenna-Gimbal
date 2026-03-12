#include "HardwareSerial.h"
#include <Arduino.h>
#include "config.h"
#include "gnss.h"

String readSentence;
String gnss_utc_time = "";
unsigned long last_gnss_blink_time = 0;
bool led1_on = false;

// GNSS global variables


void initGNSS() {
  Serial.begin(115200);
  Serial2.begin(38400);
  Serial.println("Initializing GNSS...");

  // Non-blocking: set a short timeout window to detect activity
  unsigned long start_time = millis();
  bool gnss_active = false;

  while (millis() - start_time < 2000) {  // check for up to 2 seconds
    if (Serial2.available()) {
      gnss_active = true;
      break;
    }
  }

  if (gnss_active) {
    Serial.println("GNSS is active and sending data.");
  } else {
    Serial.println("Warning: GNSS not responding yet.");
  }
}

void updateGNSS() {
  while (Serial2.available()) {
    char c = Serial2.read();
    if (c == '\n') {
      if (readSentence.startsWith("$GNGGA") || readSentence.startsWith("$GPGGA")) {
        parseGGA(readSentence);
      }
      readSentence = "";
    } else if (c != '\r') {
      readSentence += c;
    }
  }
  //Serial.print(gnss_fix);
  //Serial.print("\n");
  // LED flash logic
  static unsigned long last_gnss_blink_time = 0;
  static bool leds_on = true;
  unsigned long current_time = millis();

  if (gnss_fix > 1) {
    // At the start of a new second: turn LEDs OFF briefly
    if (current_time - last_gnss_blink_time >= 1000) {
      digitalWrite(LED1_PIN, LOW);
      Serial.println("blink");
      leds_on = false;
      last_gnss_blink_time = current_time;
    }
    // After 100ms: turn LEDs back ON
    else if (!leds_on && current_time - last_gnss_blink_time >= 100) {
      digitalWrite(LED1_PIN, HIGH);
      leds_on = true;
    }
  }
}



String formatGNSSUTC(const String& rawTime) {
  if (rawTime.length() < 6) return "00:00:00";
  String hh = rawTime.substring(0, 2);
  String mm = rawTime.substring(2, 4);
  String ss = rawTime.substring(4, 6);
  return hh + ":" + mm + ":" + ss;
}


void parseGGA(const String& sentence) {
  int idx = 0;
  int lastIdx = 0;
  int fieldNum = 0;
  String fields[20];

  while ((idx = sentence.indexOf(',', lastIdx)) != -1 && fieldNum < 19) {
    fields[fieldNum++] = sentence.substring(lastIdx, idx);
    lastIdx = idx + 1;
  }
  fields[fieldNum] = sentence.substring(lastIdx);

  if (fields[6].toInt() > 0) {
    // GNSS time (field 1)
    gnss_utc_time = formatGNSSUTC(fields[1]);  // format to HH:MM:SS

    String rawLat = fields[2];
    String rawLon = fields[4];

    float latDeg = rawLat.substring(0, 2).toFloat();
    float latMin = rawLat.substring(2).toFloat();
    gnss_lat = latDeg + latMin / 60.0;
    if (fields[3] == "S") gnss_lat *= -1;

    float lonDeg = rawLon.substring(0, 3).toFloat();
    float lonMin = rawLon.substring(3).toFloat();
    gnss_lon = lonDeg + lonMin / 60.0;
    if (fields[5] == "W") gnss_lon *= -1;

    gnss_alt = fields[9].toFloat();
    gnss_fix = fields[6].toInt();

    //Serial.print("Fix: ");
    //Serial.print(gnss_fix);
    //Serial.print("  Time: ");
    //Serial.print(gnss_utc_time);
    //Serial.print("  Lat: ");
    //Serial.print(gnss_lat, 6);
    //Serial.print("  Lon: ");
    //Serial.print(gnss_lon, 6);
    //Serial.print("  Alt: ");
    //erial.println(gnss_alt, 2);
  } else {
    Serial.println("No GNSS fix");
  }
}


double get_distance(double lat1, double lon1, double lat2, double lon2) {
  const double d2r = 3.141592653589793 / 180.0;
  double dlat = (lat2 - lat1) * d2r;
  double dlon = (lon2 - lon1) * d2r;
  lat1 *= d2r;
  lat2 *= d2r;

  double a = pow(sin(dlat / 2), 2) +
             pow(sin(dlon / 2), 2) * cos(lat1) * cos(lat2);
  return 6378100.0 * 2.0 * atan2(sqrt(a), sqrt(1-a));  // Earth radius in meters
  //return 6378100.0 * 2.0 * asin(sqrt(a));  // Earth radius in meters
}