#include <LiquidCrystal.h>
#include "constants.h"
#include "variables.h"
#include "display.h"
#include "kbd.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void update_pressures_volt() {
  pressure_low_volt  = press_to_volt(pressure_low);
  pressure_high_volt = press_to_volt(pressure_high);
}

void update_useSensor() { useSensor = (pressure_low != pressure_high); }

void dec_low()  { if (pressure_low > MIN_VALUE)         { pressure_low  -= STEP; update_pressures_volt(); } }
void inc_low()  { if (pressure_high - pressure_low > 1) { pressure_low  += STEP; update_pressures_volt(); } }
void dec_high() { if (pressure_high - pressure_low > 1) { pressure_high -= STEP; update_pressures_volt(); } }
void inc_high() { if (pressure_high < MAX_VALUE)        { pressure_high += STEP; update_pressures_volt(); } }
void selectLowP()  { cursor_position = CUR_PRESS_LOW;  }
void selectHighP() { cursor_position = CUR_PRESS_HIGH; }
void buttonUp() {
  if (cursor_position == CUR_PRESS_LOW) { inc_low();  } 
  else                                  { inc_high(); }
}
void buttonDown() {
  if (cursor_position == CUR_PRESS_LOW) { dec_low();  } 
  else                                  { dec_high(); }
}

void motorOn()  { digitalWrite(PORT_RELAY_PUMP, HIGH); is_motor_on = true;  }
void motorOff() { digitalWrite(PORT_RELAY_PUMP, LOW);  is_motor_on = false; }

void switchPump() {
  if (is_motor_on) motorOff(); 
  else             motorOn();
}

void flowCounterService() { pulseCount++; }
void attachService() { attachInterrupt(couter_interrupt, flowCounterService, CHANGE); }
void detachService() { detachInterrupt(couter_interrupt);                             }

void init_pressure_averager() {
  unsigned int p = read_pressure();
  for (int i = 0; i < TAIL_SIZE; i++) {
      p_tail[i] = p;
  }
  p_position = 0;
}

int read_pressure() {
  p_tail[p_position % TAIL_SIZE] = analogRead(PORT_PRESSURE_SENSOR);
  if (p_position + 1 < TAIL_SIZE) p_position++;
  else                            p_position = 0;
  
  unsigned int s = 0;
  for (int i = 0; i < TAIL_SIZE; ++i) {
    s += p_tail[i];
  }
  return s / TAIL_SIZE;
}

void doPumpControl() {
  pressure_current_volt = read_pressure();
  update_useSensor();
  if (useSensor) {
    if (pressure_current_volt >= pressure_high_volt)                motorOff();
    if (pressure_current_volt <= pressure_low_volt && !is_motor_on) motorOn();
  }
}

void processKeys() {
  switch (read_button()) {
    case btnRIGHT:  selectHighP();  break;
    case btnLEFT:   selectLowP();   break;
    case btnUP:     buttonUp();     break;
    case btnDOWN:   buttonDown();   break;
    case btnSELECT: switchPump();   break;
    case btnNONE:   doPumpControl();
  }
}

void processFlow() {
  long curTime = millis();
  long spent = curTime - oldTime;

  if (spent > 1000) {
    detachService();
    float quantTime = 1000.0 / spent;
    flow = quantTime * pulseCount / factor; // L/min
    // debug purposes
    // flowMl = (flow / 60) * 1000;
    // totalMl += flowMl;
    // Serial.println("Flow rate: "+toReadable(flow)+" L/min Added "+String(flowMl)+" ml, Total: "+totalMl+"ml");
    oldTime = curTime;
    pulseCount = 0;
    attachService();
  }
}

void setup() {
  // init display
  lcd.begin(16, 2);
  // initial cursor position
  cursor_position = CUR_PRESS_LOW;
  // initial motor state
  motorOff();
  // initial pressure values
  pressure_low = DEFAULT_MIN_VALUE;
  pressure_high = DEFAULT_MAX_VALUE;
  // port modes
  pinMode(PORT_SENSOR_FLOW, INPUT_PULLUP); // used as interrupt
  pinMode(PORT_RELAY_PUMP, OUTPUT);
  //digitalWrite(PORT_SENSOR_FLOW, HIGH);
  //digitalWrite(PORT_PRESSURE_SENSOR, HIGH);
  update_pressures_volt();
  useSensor = true;
  pulseCount = 0;
  flow       = 0.0;
  flowMl     = 0;
  totalMl    = 0;
  oldTime    = 0;
  init_pressure_averager();
  attachService();
  //Serial.begin(38400); // debug purposes
}

void loop() {
  display(lcd);
  processKeys();
  processFlow();
  delay(100);
}
