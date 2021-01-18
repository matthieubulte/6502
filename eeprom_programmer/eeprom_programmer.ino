
#define N_WE 24
#define N_OE 34
#define N_CE 38

int data[] = {43, 45, 47, 48, 46, 44, 42, 40};
int addr[] = {41, 39, 37, 35, 33, 31, 29, 27, 28, 30, 36, 32, 25, 26, 23};

void dataAs(byte mode) {
  for (int i = 0; i < 8; i++) pinMode(data[i], mode);
}

void addrAs(byte mode) {
  for (int i = 0; i < 15; i++) pinMode(addr[i], mode);
}

void setData(byte d) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(data[i], d&1);
    d>>=1;
  }
}

void setAddr(unsigned int a) {
  for (int i = 0; i < 15; i++) {
    digitalWrite(addr[i], a&1);
    a >>=1 ;
  }
}


byte readData() {
  byte d = 0;
  for (int i = 0; i < 8; i++) d = (d << 1) | digitalRead(data[7-i]);
  return d;
}

void initPins() {
  dataAs(INPUT);
  addrAs(OUTPUT);

  pinMode(N_CE, OUTPUT); digitalWrite(N_CE, LOW);
  pinMode(N_WE, OUTPUT); digitalWrite(N_WE, HIGH);
  pinMode(N_OE, OUTPUT); digitalWrite(N_OE, HIGH);
}

void print16Bits(unsigned int base) {
  byte data[16];
  for (unsigned int offset = 0; offset <= 15; offset += 1) {
    data[offset] = readEEPROM(base + offset);
  }


  char buf[80];
  sprintf(buf, "%04x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
          base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
          data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

  Serial.println(buf);
}

byte readEEPROM(unsigned int addr) {
  setAddr(addr);
  dataAs(INPUT);
  digitalWrite(N_OE, LOW);
  return readData();
}

void writeEEPROM(unsigned int addr, byte data) {
  digitalWrite(N_OE, HIGH);
  setAddr(addr);

  digitalWrite(N_WE, LOW);
  dataAs(OUTPUT);
  setData(data);

  delayMicroseconds(1);
  digitalWrite(N_WE, HIGH);

  delay(10); // make sure that we don't initate a page write cycle, this is not handled yet
}

void setup() {
  Serial.begin(9600);

  Serial.println("Press any key to start uploading your program...");
  while (!Serial.available()) {
    delay(100);
  }
  
  

  initPins();

#define PROG_SIZE 4
  byte program[PROG_SIZE] = {
    0xEA, /* NOP */
    0x4C, /* JMP */
    0x00, /* ADDR[LOW] */
    0x80  /* ADDR[HIGH] */
  };

  // write program
  Serial.println("Clear EEPROM...");
  for (unsigned int i = 0; i < 0x8000; i++) {
    if (i % 1000 == 0) {
      Serial.print(i); Serial.println(" / 32k");
    }
    writeEEPROM(i, 0xEA);
  }


  Serial.println("Upload program...");
  for (unsigned int addr = 0; addr < PROG_SIZE; addr++) {
    writeEEPROM(addr, program[addr]);
  }

  // write reset vector
  writeEEPROM(0x7FFC, 0x00);
  writeEEPROM(0x7FFD, 0x80);


  Serial.println("Dump EEPROM memory...");
  print16Bits(0);
  print16Bits(0x7FFC);
}

void loop() {}
