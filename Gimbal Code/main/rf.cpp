#include "rf.h"
#include "config.h"
#include "sdcard.h"
#include <SPI.h>
#include <RH_RF95.h>

static RH_RF95 rf95(RFM95_CS, RFM95_INT); // Pins must be defined in config.h

void initRF() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  delay(100);
  Serial.println("Arduino LoRa RX Test!");

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init()) {
    Serial.println("LoRa radio init failed");
  }
  else{
  Serial.println("LoRa radio initialized");
    if (!rf95.setFrequency(RF95_FREQ)) {
      Serial.println("setFrequency failed");
      while (1);
  }
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  rf95.setTxPower(23, false);
  rf95.setModeRx();
  Serial.println("LoRa module set to RX mode.");
}
}

void updateRF() {
  
  // receive command
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
      buf[len] = '\0';
      String msg = String((char*)buf);
      msg.trim();

      // should gimbal send a packet acknowledgement or not
      bool sendAck = true; 

      Serial.print("Packet Received: ");
      Serial.println(msg);

      String prefix = gimbal_color + ":";
      prefix.toUpperCase();

      String msg_upper = msg;
      msg_upper.toUpperCase();

      // commands require prefix of gimbal color or a mix of both colors
      if (!(msg_upper.startsWith(prefix) || msg_upper.startsWith("ORANGEBLACK:") || msg_upper.startsWith("BLACKORANGE:"))) {
        Serial.print("Ignored message: ");
        Serial.println(msg);
        return;
      }

      int colonIndex = msg.indexOf(':');
      if (colonIndex == -1) {
        Serial.println("Malformed message (no colon):");
        Serial.println(msg);
        return;
      }

      msg = msg.substring(colonIndex + 1);
      msg.trim();

      // change operation mode to monostatic
      // <prefix>:mono
      if (msg == "mono") {
        operation_mode = MONOSTATIC;
      }

      // change operation mode to bistatic
      // <prefix>:bistatic
      else if (msg == "bistatic") {
        operation_mode = BISTATIC;
      }

      // change operation mode to stow
      // <prefix>:stow
      else if (msg == "stow"){
        operation_mode = STOW;
      }

      // reply with the target lat and lon      
      // <prefix>:latlontarget:request
      else if (msg == "latlontarget:request") {
        char reply[64];
        snprintf(reply, sizeof(reply), "Lat=%.6f,Lon=%.6f", TARGET_LAT, TARGET_LON);
        rf95.send((uint8_t*)reply, strlen(reply));
        rf95.waitPacketSent();
        rf95.setModeRx();

        Serial.print("Sent response: ");
        Serial.println(reply);

        sendAck = false;
      }

      // reset Teensy
      // <prefix>:reset
      else if (msg == "reset") {
        Serial.println("Resetting Teensy...");
        delay(100);
        SCB_AIRCR = 0x05FA0004;
      }

      // set target lat-lon for bistatic
      // <prefix>:latlontarget:set:<lat>:<lon>
      else if (msg.startsWith("latlontarget:set:")) {
        int thirdColon = msg.indexOf(':', 17);
        if (thirdColon != -1) {
          String latStr = msg.substring(17, thirdColon);
          String lonStr = msg.substring(thirdColon + 1);

          TARGET_LAT = latStr.toFloat();
          TARGET_LON = lonStr.toFloat();

          Serial.print("TARGET_LAT updated to: ");
          Serial.println(TARGET_LAT, 6);
          Serial.print("TARGET_LON updated to: ");
          Serial.println(TARGET_LON, 6);
        } else {
          Serial.println("Invalid latlontarget:set format.");
        }
      }

      // set stow angle      
      // <prefix>:stowangle:set:<value>
      else if (msg.startsWith("stowangle:set:")) {
        String angleStr = msg.substring(14);
        STOW_ANGLE = angleStr.toFloat();

        Serial.print("STOW_ANGLE updated to: ");
        Serial.println(STOW_ANGLE);
      }

      // set target angle for monostatic
      // <prefix>:targetangle:set:<value>
      else if (msg.startsWith("targetangle:set:")) {
        String angleStr = msg.substring(16);
        TARGET_PITCH = angleStr.toFloat();

        Serial.print("TARGET_ANGLE updated to: ");
        Serial.println(TARGET_PITCH);
      }

      // change mission.txt file
      // <prefix>:mission:set:<field>:<value>
      else if (msg.startsWith("mission:set:")){
        int fieldStart = 12;
        int colonIndex = msg.indexOf(':', fieldStart);

        if (colonIndex == -1){
          Serial.println("Invalid mission:set format");
          return;
        }

        String field = msg.substring(fieldStart, colonIndex);
        String value = msg.substring(colonIndex + 1);

        field.trim();
        value.trim();

        updateMissionFile(field, value);
      }

      // request current target angles
      // <prefix>:targetangle:request
      else if (msg == "targetangle:request") {
          char reply[64];
          snprintf(reply, sizeof(reply), "%s:TARGET_ANGLE=%.2f", gimbal_color.c_str(), TARGET_PITCH);
          rf95.send((uint8_t*)reply, strlen(reply));
          rf95.waitPacketSent();
          rf95.setModeRx();

          Serial.print("Sent TARGET_ANGLE: ");
          Serial.println(TARGET_PITCH);
      }

      // request stow angle
      // <prefix>:stowangle:request
      else if (msg == "stowangle:request") {
          char reply[64];
          snprintf(reply, sizeof(reply), "%s:STOW_ANGLE=%.2f", gimbal_color.c_str(), STOW_ANGLE);
          rf95.send((uint8_t*)reply, strlen(reply));
          rf95.waitPacketSent();
          rf95.setModeRx();

          Serial.print("Sent STOW_ANGLE: ");
          Serial.println(STOW_ANGLE);
      }

      // handshake with base station
      // <prefix>:ping
      else if (msg == "ping") {
        Serial.println("Packet Received");

        char reply[64];
        snprintf(reply, sizeof(reply), "%s:Connected to base-station", gimbal_color.c_str());

        rf95.send((uint8_t*)reply, strlen(reply));
        rf95.waitPacketSent();
        rf95.setModeRx();

        Serial.print("Sent: ");
        Serial.println(reply);

        sendAck = false;
      }

      if (sendAck) {
        char ack[64];
        snprintf(ack, sizeof(ack), "%s:Packet Received", gimbal_color.c_str());

        rf95.send((uint8_t*)ack, strlen(ack));
        rf95.waitPacketSent();
        rf95.setModeRx();

        Serial.print("ACK sent: ");
        Serial.println(ack);
      }
    }
  }
}

