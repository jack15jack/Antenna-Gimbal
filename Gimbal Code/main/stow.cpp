//stow.cpp
#include "config.h"
#include "sdcard.h"

void handleStowState() {
  if (!stow_flag) {
    pitch_servo.write(servo_center_angle);
    roll_servo.write(STOW_ANGLE);
    delay(servo_delay);
    stow_flag = true;  
    writeToSDCard();
  }
}
