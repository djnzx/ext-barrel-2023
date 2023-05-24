unsigned int map1(unsigned int val, unsigned int from[], unsigned int to[]) {
  if (val < from[0]) { return to[0]; } // out of range (less)

  for (int i = 0; i < a_length; i++) {
    if (val == from[i]) {
      return to[i];
    } else if (val < from[i]) {
      int deltaFrom = from[i] - from[i-1];
      int deltaTo = to[i] - to[i-1];

      int shiftFrom = val - from[i-1];
      float k = (float) shiftFrom / deltaFrom;
      return to[i-1] + deltaTo * k;
    }
  }

  return to[a_length - 1]; // out of range (more)
}

int volt_to_press(int volt) {
  return map1(volt, voltage_val, pressure_val);
}

int press_to_volt(int pres){
  return map1(pres, pressure_val, voltage_val);
}
