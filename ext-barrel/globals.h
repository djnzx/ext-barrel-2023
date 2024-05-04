// LCD
#include <LCD_I2C.h>
LCD_I2C lcd(0x27, 20, 4);

// safety stuff, start from empty barrel
bool is_empty = true;

// current cursor position
int cursor_position;

// global state for motor relay
bool is_motor_on;

// current pressures for on(low) / off(high), being set via UI
int pressure_low;
int pressure_high;
bool is_manual() { return pressure_low == pressure_high; }

// the same in volage, because sensor returns the voltage
int pressure_low_volt;
int pressure_high_volt;

// current pressure, being read on each iteration
int pressure_current_volt;

// used for 'mute' the sensor
// if low=high we don't use the sensor's value for relay control
// we work based on SELECT button
bool use_sensor = true;
bool is_manual_mode() { return !use_sensor; }

// buffer to average pressures being read
const int TAIL_SIZE = 5;
int p_tail[TAIL_SIZE];
int p_position = 0;

// pulse counter
volatile unsigned int pulse_count = 0;

// flow, liters per minute
float flow = 0.0;

// the most recent time flow was calculated
long oldTime = 0;
