//relay.cpp
#include "config.h"
#include "relay.h"


void updateRelay1() {
  static bool last_stable_state = false;
  static bool last_read_state = false;
  static unsigned long last_debounce_time = 0;
  const unsigned long debounce_delay = 50;

  bool current_input = digitalRead(relay_pin_in1);

  if (current_input != last_read_state) {
    last_debounce_time = millis();
  }

  if ((millis() - last_debounce_time) > debounce_delay) {
    if (current_input != last_stable_state) {
      last_stable_state = current_input;
      relay_flag = current_input;

      if (relay_flag) {
        // Leaving STOW → going into operation mode
        current_state = operation_mode;

        // Force servos straight down once when leaving STOW
        pitch_servo.write(90);
        roll_servo.write(90);
        delay(2000);
      } else {
        // Entering STOW
        current_state = STOW;
        stow_flag = false;
      }
    }
  }

  last_read_state = current_input;
}

void updateRelay2() {
  static bool last_stable_state_r2 = false;
  static bool last_read_state_r2 = false;
  static unsigned long last_debounce_time_r2 = 0;
  const unsigned long debounce_delay_r2 = 50;

  bool current_input_r2 = digitalRead(relay_pin_in2);

  if (current_input_r2 != last_read_state_r2) {
    last_debounce_time_r2 = millis();
  }

  if ((millis() - last_debounce_time_r2) > debounce_delay_r2) {
    if (current_input_r2 != last_stable_state_r2) {
      last_stable_state_r2 = current_input_r2;
    }
  }

  // Immediately reflect actual state
  if (last_stable_state_r2) {
    radarrelay = true;
  } else {
    radarrelay = false;
  }

  last_read_state_r2 = current_input_r2;
}


