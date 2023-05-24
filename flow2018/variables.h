// current cursor position
unsigned int cursor_position;
// global state for motor relay
bool is_motor_on;
// current pressures for on(low) / off(high)
unsigned int pressure_low;
unsigned int pressure_high;
// used for 'mute' the sensor
// if low=high we don't use the sensor's value for relay control
// we work based on SELECT button
bool useSensor;
// buffer to average pressures being read
const unsigned int TAIL_SIZE = 5;
unsigned int p_tail[TAIL_SIZE];
unsigned int p_position;

// TODO: document and clean-up
unsigned int pressure_low_volt;
unsigned int pressure_high_volt;
unsigned int pressure_current_volt;
// flow
volatile int pulseCount;
float flow;
unsigned int flowMl;
unsigned long totalMl;
unsigned long oldTime;
