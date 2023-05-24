// HIGH = we are in open  position and pulled-up to +5V
// LOW  = we are in close position and short-circuited to the GROUND
bool read_is_empty() {
  return digitalRead(PORT_BARREL_IS_EMPTY) == HIGH;
}
