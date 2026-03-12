#include "gnss.h"
#include "imu.h"
#include "monostatic.h"
#include "bistatic.h"
#include "config.h"
#include "sdcard.h"
#include "relay.h"
#include "stow.h"
#include <Wire.h>
#include "rf.h"


void setup() {
  initLEDs();
  Serial.begin(115200);
  // Wire.begin();
  Serial.println("booting");
  
  pinMode(relay_pin_in1, INPUT_PULLDOWN);
  pinMode(relay_pin_in2, INPUT_PULLDOWN);

   initGNSS();
   initIMUs();
   initSD();
   initServos();
   if (rf_enabled){
     initRF();
   }
}

void loop() {
   updateRelay1();
   updateRelay2();
   updateIMUs();
  
   if (rf_enabled ){
     updateRF();
   }
   updateGNSS();
  
   switch (current_state) {
       case STOW:
         handleStowState();
         //Serial.println("Stow");
         //Serial.println(gnss_fix);
         break;

       case MONOSTATIC:
         handleMonostaticState();
         //Serial.println("Mono");
         break;

       case BISTATIC:
         //Serial.println("Bistatic");
         //Serial.println(gnss_fix);
         //if (gnss_fix < 3) break;  
        // else {
             handleBistaticState();
         //}
         break;
  }
  writeToSDCard();

  // Serial.println(digitalRead(relay_pin_in1));
}


