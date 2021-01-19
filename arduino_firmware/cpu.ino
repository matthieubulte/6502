#define RWB_PIN 25
#define BE_PIN 22

void initCPU() {
  pinMode(RWB_PIN, INPUT);
  pinMode(BE_PIN, OUTPUT);

  digitalWrite(BE_PIN, HIGH);
}

byte readRWB() {
  return digitalRead(RWB_PIN);
}

void cpuDisconnectFromBus() {
  digitalWrite(BE_PIN, LOW);
}

void cpuConnectToBus() {
  digitalWrite(BE_PIN, HIGH);  
}
