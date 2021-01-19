#define DATA_START 26
#define ADDR_START 36

void addrAs(byte mode) {
  for(int i = 0; i < 16; i++) pinMode(ADDR_START+i, mode);
}

void dataAs(byte mode) {
  for(int i = 0; i < 8; i++) pinMode(DATA_START+i, mode);
}

void initBus() {
  highImpedance();
}

void highImpedance() {
  dataAs(INPUT);
  addrAs(INPUT);
}

byte readData() {
  dataAs(INPUT);
  byte data = 0;
  for(int i = 0; i < 8; i++) { 
    data = (data<<1) | digitalRead(DATA_START+7-i);
  }
  return data;
}

void setData(byte data) {
  dataAs(OUTPUT);
  for(int i = 0; i < 8; i++) { 
    digitalWrite(DATA_START+i, data&1);
    data >>= 1;
  }
}

void setAddrLow(byte addrLow) {
  addrAs(OUTPUT);
  for (int i = 0; i < 8; i++) {
    digitalWrite(ADDR_START+i, addrLow&1);
    addrLow >>=1 ;
  }
}

void setAddrHigh(byte addrHigh) {
  addrAs(OUTPUT);
  for (int i = 8; i < 16; i++) {
    digitalWrite(ADDR_START+i, addrHigh&1);
    addrHigh >>=1 ;
  }
}

byte readAddrHigh() {
  addrAs(INPUT);
  byte addr = 0;
  for(int i = 0; i < 8; i++) {
    addr = (addr<<1) | digitalRead(ADDR_START+15-i);
  }
  return addr;
}

byte readAddrLow() {
  addrAs(INPUT);
  byte addr = 0;
  for(int i = 8; i < 16; i++) {
    addr = (addr<<1) | digitalRead(ADDR_START+15-i);
  }
  return addr;
}
