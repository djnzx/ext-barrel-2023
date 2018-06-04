byte sensorPin       = 3; //2, 3, 18, 19, 20, 21
const float factor = 4.5; // 4.5 pulses per second per litre/minute of flow. 

volatile int pulseCount;  
float flow;
unsigned int flowMl;
unsigned long totalMl;
unsigned long oldTime;

void setup(){
  Serial.begin(38400);
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount = 0;
  flow       = 0.0;
  flowMl     = 0;
  totalMl    = 0;
  oldTime    = 0;

  attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
}

String toReadable(float val) {
  int roun = int(val);
  int frac = (val - int(val)) * 10;
  return String(roun)+"."+String(frac);
}

void loop() {
    long current = millis();
    if((current - oldTime) > 1000) {
        detachInterrupt(digitalPinToInterrupt(sensorPin));
        float quantTime = 1000.0 / (current - oldTime);
        flow = quantTime * pulseCount / factor; // L/min
        
        flowMl = (flow / 60) * 1000;
        totalMl += flowMl;
 
        Serial.println("Flow rate: "+toReadable(flow)+" L/min Added "+String(flowMl)+" ml, Total: "+totalMl+"ml");
        oldTime = current;
        pulseCount = 0;
        attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
    }
}

void pulseCounter() {
  pulseCount++;
}
