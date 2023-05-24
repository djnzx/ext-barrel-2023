#include <Arduino.h>
// PUMP OPERATIONS

void pumpOn() {
  if (is_empty) return;
  digitalWrite(PORT_RELAY_PUMP, HIGH);
  is_motor_on = true;
}

void pumpOff() {
  digitalWrite(PORT_RELAY_PUMP, LOW);
  is_motor_on = false;
}

void pumpSwitch() {
  if (is_motor_on) pumpOff(); 
  else             pumpOn();
}
