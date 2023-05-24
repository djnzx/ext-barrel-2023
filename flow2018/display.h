#include "functions.h"

String floatToReadable(float val) {
  unsigned int roun = int (val);
  unsigned int frac = int (val * 10) - int (val) * 10;
  return
    (roun < 10 ? " " : "") +
    String(roun)
     + "." +
    String(frac);
}

// we use bool special
// for special marker '*' to divide fractional part
// to indicate where cursor now is
String intToReadable(unsigned int value, bool special) {
  int digit1 = value / 100;
  int digit2 = (value % 100) / 10;
  int digit3 = value % 10;
  return
    String(digit1)
    + (special ? "*" : ".") +
    String(digit2)
    +
    String(digit3);
}

String getLine1Content() {
   return
     intToReadable(pressure_low, cursor_position == CUR_PRESS_LOW)
     + "/" +
     intToReadable(pressure_high, cursor_position == CUR_PRESS_HIGH)
     + " C:" +
     intToReadable(volt_to_press(pressure_current_volt), false)
     + " ";
}

String getLine2Content() {
   return
     "F:" +
     floatToReadable(flow)
     + "lm " +
     String(useSensor ? "A" : "M")
     + " V" +
     String(pressure_current_volt)
     + " ";
}

void display(LiquidCrystal lcd0) {
  lcd0.setCursor(0, 0); lcd0.print(getLine1Content());
  lcd0.setCursor(0, 1); lcd0.print(getLine2Content());
}
