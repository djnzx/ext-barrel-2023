#include <Arduino.h>
#include "constants.h"
#include "globals.h"
#include "pump.h"
#include "kbd.h"
#include "pressure.h"
#include "display.h"
#include "counter.h"
#include "levels.h"

////////////////////////////////////////////////////////////////////////
void setup() {
  // idnit display
  lcd.begin();
  lcd.backlight();
  // lcd.begin(16, 2);
  // initial cursor position
  cursor_position = CUR_PRESS_LOW;
  // initial motor state
  pumpOff();
  // initial pressure values
  pressure_low = DEFAULT_MIN_VALUE;
  pressure_high = DEFAULT_MAX_VALUE;
  // port modes
  pinMode(PORT_BARREL_IS_EMPTY, INPUT);
  pinMode(PORT_SENSOR_FLOW, INPUT);
  pinMode(PORT_RELAY_PUMP, OUTPUT);
  init_pressure_averager();
  // flow
  attachHandler();
  // debug purposes
  Serial.begin(38400);
}

////////////////////////////////////////////////////////////////////////
void read_sensors() {
  is_empty = read_is_empty();
  pressure_current_volt = read_pressure_avg();
}

// HANDLE KEYBOARD, UPDATE global variables, DO PUMP SWITCH IN MANUAL MODE
void handle_keyboard(LCD_I2C lcd0) {
  switch (read_button(lcd0)) {
    case btnRIGHT:  handleBtnRight(); break;
    case btnLEFT:   handleBtnLeft();  break;
    case btnUP:     handleBtnUp();    break;
    case btnDOWN:   handleBtnDown();  break;
    case btnSELECT: pumpSwitch();  break;
  }
  // update sensor value
  use_sensor = pressure_low < pressure_high;
  // update voltage values for pressures
  pressure_low_volt  = press_to_volt(pressure_low);
  pressure_high_volt = press_to_volt(pressure_high);
}

// DO PUMP CONTROL BASED ON THE PRESSURE READ //////////////////////////
void do_pump_control() {
  if (is_empty) {
    pumpOff();
    return;
  }
  if (is_manual_mode()) return;
  if (pressure_current_volt >= pressure_high_volt)                pumpOff();
  if (pressure_current_volt <= pressure_low_volt && !is_motor_on) pumpOn();
}

float calc_lpm(long spent) {
  double actual_seconds = double(spent) / 1000;
  double pulses_per_second = pulse_count / actual_seconds;
  return pulses_per_second / factor;
}

////////////////////////////////////////////////////////////////////////
void handle_flow() {
  long curTime = millis();
  long spent = curTime - oldTime;
  if (spent > 1000) {
    detachHandler();
    flow = calc_lpm(spent);
    String msg = "Time spent: " + String(double(spent) / 1000) + "s, pulse count: " + String(pulse_count) + ", litres/minute: " + floatToReadable(flow);
    Serial.println(msg);
    oldTime = curTime;
    pulse_count = 0;
    attachHandler();
  }
}

void loop() {
  read_sensors();
  handle_keyboard(lcd);
  do_pump_control();
  handle_flow();
  display(lcd); // relies on the `flow` variable
  delay(200);
}
