// INTERRUPT-RELATED
void flowCounterService() {
  pulse_count++;
}

void attachHandler() {
  attachInterrupt(INT_COUNTER, flowCounterService, FALLING);
}

void detachHandler() {
  detachInterrupt(INT_COUNTER);
}
