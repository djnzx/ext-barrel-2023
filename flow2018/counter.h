// INTERRUPT-RELATED
void flowCounterService() { pulse_count++; }
void attachHandler() { attachInterrupt(counter_interrupt, flowCounterService, FALLING); }
void detachHandler() { detachInterrupt(counter_interrupt);                             }
