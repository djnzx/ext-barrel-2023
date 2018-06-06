/*
 * A0, D4..D9 used for LED+buttons
 * A1 used for Pressure sensor (Chrysler 4.7 5.7 Oil pressure sensor)
 * 
 * D3 used for control the pump relay to provide desired pressure in given range. 
 * D2 used for hall-based flow sensor (PS400A G1")
 * 
 * second line shows the current flow if the sensor connected
 * ----------------
 * 2.40/3*80 P:3.25
 * F:15.2lm A V1024  
 * ----------------
 */
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // D
#define PORT_LCD_BUTTONS A0

#define btnNONE   0
#define btnRIGHT  1
#define btnUP     2
#define btnDOWN   3
#define btnLEFT   4
#define btnSELECT 5

#define MIN_VALUE 100
#define MAX_VALUE 400
// with those settings we have 31 sec work / 23 sec idle cycle
#define DEFAULT_MIN_VALUE 240
#define DEFAULT_MAX_VALUE 390
#define PRESS_LOW 1
#define PRESS_HIGH 2

#define PORT_PRESSURE_SENSOR A1 // A
#define PORT_SENSOR_FLOW 2 // D
#define PORT_RELAY_PUMP  3 // D
 /* 
  *  used to switch relay ON/OFF
  *  outside module uses `HIGH`
  *  inside module uses `LOW`
  */
#define RELAY_SWITCH_ON_TYPE HIGH
#define RELAY_SWITCH_OFF_TYPE LOW

const float factor = 4.5; // 4.5 pulses per second per litre/minute of flow. 

unsigned int pressure_low;
unsigned int pressure_high;
unsigned int pressure_low_volt;
unsigned int pressure_high_volt;
unsigned int pressure_current_volt;
unsigned int current_position;
bool statusRelay;
// used for 'mute' the sensor, if low=high we don't use the sensor's value for relay control.
bool useSensor;

// checked, used: 5149062AA, PS317
// not checked: 5149062AB, 56028807AA, 56028807AB, 5080472AA, 5093908AA, 56044777AA, 68060337AA, 1S7937,  5149064AA, 514906AA, PS401, PS598, PS701, 1S6755, 1S10853, 
unsigned int voltage_val[] = { 110, 220, 337, 454, 570, 690, 810, 920 };
unsigned int pressure_val[] = { 00, 100, 200, 300, 400, 500, 600, 700 };
unsigned int array_length = sizeof(voltage_val)/sizeof(int);
const unsigned int TAIL_SIZE=5;
unsigned int p_tail[TAIL_SIZE];
unsigned int p_position;

// flow
volatile int pulseCount;  
float flow;
unsigned int flowMl;
unsigned long totalMl;
unsigned long oldTime;

void update_pressures_volt() {
  pressure_low_volt = press_to_volt(pressure_low);
  pressure_high_volt = press_to_volt(pressure_high);
}

void update_useSensor() {
  useSensor = (pressure_low != pressure_high);
}

unsigned int map(unsigned int val, unsigned int from[], unsigned int to[]) {
  if (val<from[0]) {
    // to avoid unexpected behavior in this case
    return to[0];
  }
  for (int i=0; i<array_length; i++) {
    if (val == from[i]) {
      return to[i];
    } else if (val < from[i]) {
      int deltaFrom=from[i]-from[i-1];
      int deltaTo=to[i]-to[i-1];

      int shiftFrom=val-from[i-1];
      float k=(float)shiftFrom/deltaFrom;
      return to[i-1]+deltaTo*k;
    }
  }
  // to avoid unexpected behavior in this case
  return to[array_length-1]; // out of range
}

int volt_to_press(int volt) {
  return map(volt, voltage_val, pressure_val);
}

int press_to_volt(int pres){
  return map(pres, pressure_val, voltage_val);
}

void dec_low() {
  if (pressure_low > MIN_VALUE) {
    pressure_low-=10;
    update_pressures_volt();
  }
}

void inc_low() {
  if (pressure_high - pressure_low > 1) {
    pressure_low+=10;
    update_pressures_volt();
  }
}

void dec_high() {
  if (pressure_high - pressure_low > 1) {
    pressure_high-=10;
    update_pressures_volt();
  }
}

void inc_high() {
  if (pressure_high < MAX_VALUE) {
    pressure_high+=10;
    update_pressures_volt();
  }
}

String floatToReadable(float val) {
  unsigned int roun = int(val);
  unsigned int frac=int(val*10)-int(val)*10;
  return
    (roun<10 ? " " : "") + 
    String(roun) + 
    "." + 
    String(frac);
}

/*
 * we use bool special
 * for special marker '*' to divide fractional part
 * to indicate where cursow now is
 */
String intToReadable(unsigned int value, bool special) {
  int digit1 = value / 100;
  int digit2 = (value % 100) / 10;
  int digit3 = value % 10;
  return String(digit1) +
    (special ? "*" : ".") +
    String(digit2) + String(digit3);
}

String getLineContent1() {
   return
     intToReadable(pressure_low, current_position==PRESS_LOW) +
     "/" +
     intToReadable(pressure_high, current_position==PRESS_HIGH) +
     " C:" +
     intToReadable(volt_to_press(pressure_current_volt), false) + " ";
}

String getLineContent2() {
   return
     "F:" + 
     floatToReadable(flow) + 
     "lm " +
     String(useSensor ? "A" : "M") +
     " V" +
     String(pressure_current_volt) +
     " ";
}

void selectLow() {
  current_position = PRESS_LOW;
}

void selectHigh() {
  current_position = PRESS_HIGH;
}

void buttonUp() {
  if (current_position == PRESS_LOW) {
    inc_low();
  } else {
    inc_high();
  }
}

void buttonDown() {
  if (current_position == PRESS_LOW) {
    dec_low();
  } else {
    dec_high();
  }
}

void switchPump() {
  if (isRelayOn()) {
    relayOff();
  } else {
    relayOn();
  }
}

void relayOn() {
  digitalWrite(PORT_RELAY_PUMP, RELAY_SWITCH_ON_TYPE);
  statusRelay = true;
}

void relayOff() {
  digitalWrite(PORT_RELAY_PUMP, RELAY_SWITCH_OFF_TYPE);
  statusRelay = false;
}

bool isRelayOn() {
  return statusRelay;
}

int read_button() {
  unsigned int in = analogRead(PORT_LCD_BUTTONS); // 0 - port, which used to read buttons
  if (in >= 700) return btnNONE; 
  if (in < 50)   return btnRIGHT;   // 0  
  if (in < 150)  return btnUP;      // 97
  if (in < 300)  return btnDOWN;    // 253
  if (in < 450)  return btnLEFT;    // 405
  if (in < 700)  return btnSELECT;  // 638
}

void attachService() {
  attachInterrupt(digitalPinToInterrupt(PORT_SENSOR_FLOW), interruptService, FALLING);
}

void detachService() {
  detachInterrupt(digitalPinToInterrupt(PORT_SENSOR_FLOW));
}

void init_pressure_averager() {
  unsigned int p = read_pressure();
  for (int i=0;i<TAIL_SIZE;i++) {
      p_tail[i]=p;
  }
  p_position=0;
}

void setup() {
  pinMode(PORT_RELAY_PUMP, OUTPUT);
  pinMode(PORT_SENSOR_FLOW, INPUT);
  digitalWrite(PORT_SENSOR_FLOW, HIGH);
  digitalWrite(PORT_PRESSURE_SENSOR, HIGH);
  lcd.begin(16, 2);
  pressure_low = DEFAULT_MIN_VALUE;
  pressure_high = DEFAULT_MAX_VALUE;
  update_pressures_volt();
  current_position = PRESS_LOW;
  useSensor=true;
  relayOff();
  pulseCount = 0;
  flow       = 0.0;
  flowMl     = 0;
  totalMl    = 0;
  oldTime    = 0;
  init_pressure_averager();
  attachService();
  Serial.begin(38400); // debug purposes only
}

int read_pressure() {
  p_tail[p_position % TAIL_SIZE]=analogRead(PORT_PRESSURE_SENSOR);
  if (p_position+1<TAIL_SIZE) {
    p_position++;
  } else {
    p_position=0;
  }
  unsigned int s=0;
  for (int i = 0; i < TAIL_SIZE; ++i) {
    s+=p_tail[i];
  }
  return s/TAIL_SIZE;
}

void doPumpControl() {
  // volt range 230..570
  pressure_current_volt = read_pressure();
  update_useSensor();
  if (useSensor) {
    if (pressure_current_volt >= pressure_high_volt) {
      relayOff();
    }
    if (pressure_current_volt <= pressure_low_volt && !isRelayOn()) {
      relayOn();
    }
  }
}

void display() {
  lcd.setCursor(0,0);
  lcd.print(getLineContent1());  
  lcd.setCursor(0,1);
  lcd.print(getLineContent2());  
}

void processKeys() {
  switch (read_button()) {
    case btnRIGHT: {
       selectHigh();
       break;
    }
    case btnLEFT: {
       selectLow();
       break;
    }
    case btnUP: {
       buttonUp();
       break;
    }
    case btnDOWN: {
       buttonDown();
       break;
    }
    case btnSELECT: {
       switchPump();
       break;
    }
    case btnNONE: {
       doPumpControl();
    }
  }
}

void processFlow() {
    long current = millis();
    if((current - oldTime) > 1000) {
      detachService();
        float quantTime = 1000.0 / (current - oldTime);
        flow = quantTime * pulseCount / factor; // L/min
        
        // debug purposes only
        // flowMl = (flow / 60) * 1000;
        // totalMl += flowMl;
        // Serial.println("Flow rate: "+toReadable(flow)+" L/min Added "+String(flowMl)+" ml, Total: "+totalMl+"ml");
        oldTime = current;
        pulseCount = 0;
        attachService();
    }
}

void loop() {
  display();
  processKeys();
  processFlow();
  delay(200);
}

void interruptService() {
    pulseCount++;
}

