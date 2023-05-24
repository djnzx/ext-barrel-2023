// ports
#define PORT_LCD_BUTTONS     A0 // A
#define PORT_PRESSURE_SENSOR A1 // A
#define PORT_SENSOR_FLOW      2 // D as interrupt 0
#define PORT_RELAY_PUMP       3 // D

// buttons - just aliases
#define btnNONE   0
#define btnRIGHT  1
#define btnUP     2
#define btnDOWN   3
#define btnLEFT   4
#define btnSELECT 5

// min / max values can be set through the UI (1.00 .. 4.00)
#define MIN_VALUE 100
#define MAX_VALUE 400

// min / max default values (on: 2.4 / off: 3.9)
// with those settings we have 31 sec work / 23 sec idle cycle
#define DEFAULT_MIN_VALUE 240
#define DEFAULT_MAX_VALUE 390
// delta per button click means 0.1
#define STEP 10

// cursor position
#define CUR_PRESS_LOW  1
#define CUR_PRESS_HIGH 2

// voltage to pressure mapping
const unsigned int voltage_val[] = { 110, 220, 337, 454, 570, 690, 810, 920 };
const unsigned int pressure_val[] = {  0, 100, 200, 300, 400, 500, 600, 700 };
const unsigned int a_length = sizeof(voltage_val) / sizeof(int);

// interrupt for counter
// arduino UNO has two interrupts
// pin 2 - interrupt 0
// pin 3 - interrupt 1
// digitalPinToInterrupt does this mapping
const int INT_COUNTER = digitalPinToInterrupt(PORT_SENSOR_FLOW);

// used to convert pulses to liters
// https://m.hi-ip.com/en/industrial-automation/sensors/flow-sensors/datchik-rashoda-zhidkosti-fs400a-g1-1-60-lmin.html
// 1 liter / minute = 4.8 pulses / second
// TODO: seems not very accurate, needs to be adjusted.
const float factor = 4.8;
