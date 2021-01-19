#define N_WE_PIN 23
#define N_OE_PIN 52

void initEEPROM() {
  pinMode(N_WE_PIN, OUTPUT);
  pinMode(N_OE_PIN, OUTPUT);
  
  digitalWrite(N_WE_PIN, HIGH);
  digitalWrite(N_OE_PIN, LOW);
}

void eepromWrite(byte addrLow, byte addrHigh, byte data) {
  cpuDisconnectFromBus();

  digitalWrite(N_OE_PIN, HIGH);
  setAddrLow(addrLow);
  setAddrHigh(addrHigh);
  
  digitalWrite(N_WE_PIN, LOW);
  setData(data);
  
  delayMicroseconds(1);
  digitalWrite(N_WE_PIN, HIGH);
  
  delay(10); // make sure that we don't initate a page write cycle, this is not handled yet

  digitalWrite(N_OE_PIN, LOW);
  
  dataAs(INPUT);
  cpuConnectToBus();
}

byte eepromRead(byte addrLow, byte addrHigh) {
  cpuDisconnectFromBus();

  setAddrLow(addrLow);
  setAddrHigh(addrHigh);

  dataAs(INPUT);
  digitalWrite(N_OE_PIN, LOW);
  
  byte data = readData();
  
  highImpedance();
  cpuConnectToBus();

  
  return data;
}
