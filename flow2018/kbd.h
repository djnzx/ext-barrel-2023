int read_button() {
  unsigned int in = analogRead(PORT_LCD_BUTTONS); // 0 - port, which used to read buttons
  if (in < 50)   return btnRIGHT;   // 0
  if (in < 150)  return btnUP;      // 97
  if (in < 300)  return btnDOWN;    // 253
  if (in < 450)  return btnLEFT;    // 405
  if (in < 700)  return btnSELECT;  // 638
  return btnNONE;
}
