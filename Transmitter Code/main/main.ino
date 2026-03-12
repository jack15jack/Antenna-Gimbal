#include <SPI.h>
#include <RH_RF95.h>

// MOSI 11
// MISO 12
// SCK 13

#define RFM95_CS 10
#define RFM95_RST 4
#define RFM95_INT 2
#define RF95_FREQ 915.0

unsigned long lastPing = 0;
const unsigned long PING_INTERVAL = 20000;
const char* pingmsg = "ORANGEBLACK:ping";

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Base Station Starting");

  // reset radio
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init())
  {
    Serial.println("LoRa radio init failed");
    while(1);
  }

  if (!rf95.setFrequency(RF95_FREQ))
  {
    Serial.println("setFrequency failed");
    while(1);
  }

  Serial.print("Frequency set to: ");
  Serial.println(RF95_FREQ);  

  rf95.setTxPower(23, false); // max power for RFM9
}

void loop() {

  // send ping periodically
  if (millis() - lastPing > PING_INTERVAL)
  {
    lastPing = millis();

    const char* msg = "ORANGEBLACK:ping";

    rf95.send((uint8_t*)msg, strlen(msg));
    rf95.waitPacketSent();

    Serial.println("Ping sent to gimbals");
  }

  // listen for response
  if (rf95.available())
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len))
    {
      buf[len] = '\0';

      Serial.print("Reply received: ");
      Serial.println((char*)buf);

      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi());
    }
  }
  
  // send command
  if (Serial.available())
  {
    String cmd = Serial.readStringUntil('\n');

    cmd.trim();

    if (cmd.length() == 0) return;

    Serial.print("Sending: ");
    Serial.println(cmd);

    rf95.send((uint8_t*)cmd.c_str(), cmd.length());
    rf95.waitPacketSent();

    Serial.println("Packet Sent");
  }

}
