int read_button(LCD_I2C lcd0) {
  unsigned int in = analogRead(PORT_LCD_BUTTONS); // 0 - port, which used to read buttons
  //lcd0.setCursor(0, 3); lcd0.print(in); lcd0.print("   "); // calibration purposes
  lcd0.setCursor(19, 3);
  if (in < 50)   { lcd0.print("R"); return btnRIGHT; }   // 0
  if (in < 170)  { lcd0.print("U"); return btnUP;    }  // 131
  if (in < 370)  { lcd0.print("D"); return btnDOWN;  }  // 307
  if (in < 570)  { lcd0.print("L"); return btnLEFT;  }  // 480
  if (in < 850)  { lcd0.print("S"); return btnSELECT;}  // 721
  lcd0.print(" ");                                      // 1023 (pull-up 2k 5v)
  return btnNONE;
}

// KEYBOARD OPERATIONS AFFECT ONLY: pressure_low, pressure_high, cursor_position
void dec_low()  { if (pressure_low > MIN_VALUE)         pressure_low  -= STEP; }
void inc_low()  { if (pressure_high - pressure_low > 1) pressure_low  += STEP; }
void dec_high() { if (pressure_high - pressure_low > 1) pressure_high -= STEP; }
void inc_high() { if (pressure_high < MAX_VALUE)        pressure_high += STEP; }

void handleBtnRight() {
  cursor_position = CUR_PRESS_HIGH;
}

void handleBtnLeft() {
  cursor_position = CUR_PRESS_LOW;
}

void handleBtnUp() {
  if (cursor_position == CUR_PRESS_LOW) inc_low();
  else                                  inc_high();
}

void handleBtnDown() {
  if (cursor_position == CUR_PRESS_LOW) dec_low();
  else                                  dec_high();
}

