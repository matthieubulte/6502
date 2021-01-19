#define CLOCK_LO 0xf0
#define CLOCK_HI 0xf1
#define READ_ADDR 0xf2
#define READ_DATA 0xf3
#define READ_RWB 0xf4
#define READ_EEPROM 0xf5
#define WRITE_EEPROM 0xf6

byte command, addrLow, addrHigh, data, state;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1);

  initEEPROM();
  initClock();
  initBus();
  initCPU();
}

void ok() {
  Serial.write(0x1);
}

void nok() {
  Serial.write(0x2);
}

byte serialRead() {
  while (!Serial.available());
  return Serial.read();
}

void loop() {
  command = serialRead();
    
  switch (command) {
    case CLOCK_LO:
      ok();
      clockLow();
      break;

    case CLOCK_HI:
      ok();
      clockHigh();
      break;

    case READ_ADDR:
      ok();
      addrLow = readAddrLow();
      addrHigh = readAddrHigh();
      Serial.write(addrLow);
      Serial.write(addrHigh);
      break;

    case READ_DATA:
      ok();
      data = readData();
      Serial.write(data);
      break;

    case READ_RWB:
      ok();
      state = readRWB();
      Serial.write(state);
      break;

    case READ_EEPROM:
      addrLow = serialRead();
      addrHigh = serialRead();
      ok();

      data = eepromRead(addrLow, addrHigh);
      Serial.write(data);
      break;

    case WRITE_EEPROM:
      addrLow = serialRead();
      addrHigh = serialRead();
      data = serialRead();
      ok();
      
      eepromWrite(addrLow, addrHigh, data);
      break;
      
    default:
      nok();
  }
}
