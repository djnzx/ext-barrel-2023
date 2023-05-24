#include "constants.h"
#include "variables.h"
#include "pump.h"
#include "kbd.h"
#include "pressure.h"
#include "display.h"
#include "counter.h"

////////////////////////////////////////////////////////////////////////
void setup() {
  // init display
  lcd.begin(16, 2);
  // initial cursor position
  cursor_position = CUR_PRESS_LOW;
  // initial motor state
  pumpOff();
  // initial pressure values
  pressure_low = DEFAULT_MIN_VALUE;
  pressure_high = DEFAULT_MAX_VALUE;
  // port modes
  pinMode(PORT_SENSOR_FLOW, INPUT); // used as interrupt
  pinMode(PORT_RELAY_PUMP, OUTPUT);
  init_pressure_averager();
  // flow
  attachHandler();
  // debug purposes
  Serial.begin(38400);
}

// DO PUMP CONTROL BASED ON THE PRESSURE READ //////////////////////////
void do_pump_control() {
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

////////////////////////////////////////////////////////////////////////
void read_sensors() {
  pressure_current_volt = read_pressure_avg();
}

// HANDLE KEYBOARD, UPDATE global variables, DO PUMP SWITCH IN MANUAL MODE
void handle_keyboard() {
  switch (read_button()) {
    case btnRIGHT:  selectHighP(); break;
    case btnLEFT:   selectLowP();  break;
    case btnUP:     buttonUp();    break;
    case btnDOWN:   buttonDown();  break;
    case btnSELECT: pumpSwitch();  break;
  }
  // update sensor value
  use_sensor = pressure_low < pressure_high;
  // update voltage values for pressures
  pressure_low_volt  = press_to_volt(pressure_low);
  pressure_high_volt = press_to_volt(pressure_high);
}

void loop() {
  read_sensors();
  handle_keyboard();
  do_pump_control();
  handle_flow();
  display(lcd); // relies on `flow` variable
  delay(200);
}
