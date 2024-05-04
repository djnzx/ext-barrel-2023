#include "functions.h"

// actually, first digit after comma
// 5.73 => 7
int fractional(float x) {
  return int (x * 10) - int (x) * 10;
}

// produces String value in format XX.X
String floatToReadable(float val) {
  int rounded = int (val);
  String first = rounded < 10 ? " " : "";
  return
    first +
    String(rounded)
     + "." +
    String(fractional(val)); // last digit
}

// we use bool special
// for special marker '*' to divide fractional part
// to indicate where cursor now is
String intToReadable(unsigned int value, bool special) {
  int digit1 = value / 100;
  int digit2 = (value % 100) / 10;
  int digit3 = value % 10;
  String delimiter = special ? "*" : ".";
  return String(digit1) + delimiter + String(digit2) + String(digit3);
}

String mk_line1() {
  return is_empty ? IS_EMPTY_LINE1 :
    intToReadable(pressure_low, cursor_position == CUR_PRESS_LOW)
    + "/" +
    intToReadable(pressure_high, cursor_position == CUR_PRESS_HIGH)
    + " C:" +
    intToReadable(volt_to_press(pressure_current_volt), false)
    + " ";
}

String mk_line2() {
  return is_empty ? IS_EMPTY_LINE2 :
    "F:" +
    floatToReadable(flow)
    + "lm " +
    String(use_sensor ? "A" : "M")
    + " V" +
    String(pressure_current_volt)
    + " ";
}

void display(LCD_I2C lcd0) {
  lcd0.setCursor(0, 0); lcd0.print(mk_line1());
  lcd0.setCursor(0, 1); lcd0.print(mk_line2());
  lcd0.setCursor(0, 2); lcd0.print(is_manual() ? "manual": "      ");
}
