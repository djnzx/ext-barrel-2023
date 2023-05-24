// READ UNPROCESSED PRESSURE (VOLTS, ANALOG: 0..1023)
int read_pressure_raw() {
  return analogRead(PORT_PRESSURE_SENSOR);
}

void init_pressure_averager() {
  int p = read_pressure_raw();
  for (int i = 0; i < TAIL_SIZE; i++) p_tail[i] = p;
}

int average(int data[], int size) {
  int sum = 0;
  for (int i = 0; i < size; i++) sum += data[i];
  return sum / size;
}

// READ CURRENT PRESSURE AVERAGED to the last 5 values
int read_pressure_avg() {
  p_tail[p_position % TAIL_SIZE] = read_pressure_raw();
  // move index
  if (p_position + 1 < TAIL_SIZE) p_position++;
  else                            p_position = 0;
  // calculate average
  return average(p_tail, TAIL_SIZE);
}
