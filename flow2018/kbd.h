int read_button() {
  unsigned int in = analogRead(PORT_LCD_BUTTONS); // 0 - port, which used to read buttons
  if (in < 50)   return btnRIGHT;   // 0
  if (in < 150)  return btnUP;      // 97
  if (in < 300)  return btnDOWN;    // 253
  if (in < 450)  return btnLEFT;    // 405
  if (in < 700)  return btnSELECT;  // 638
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

