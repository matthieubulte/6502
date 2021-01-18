#define CLOCK 24
#define CLOCK_LEN 500

#define RWB 25

#define DATA_START 26
#define ADDR_START 36

bool pauseClock;

byte readData() {
  byte data = 0;
  for(int i = 0; i < 8; i++) { 
    data = (data<<1) | digitalRead(DATA_START+7-i);
  }
  return data;
}

unsigned int readAddr() {
  unsigned int addr = 0;
  for(int i = 0; i < 16; i++) {
    addr = (addr<<1) | digitalRead(ADDR_START+15-i);
  }
  return addr;
}

void printByteHex(byte b) {
  char buf[2];
  sprintf(buf, "%02x", b);
  Serial.print(buf);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(CLOCK, OUTPUT);
  pinMode(RWB, INPUT);
  
  digitalWrite(CLOCK, LOW);

  for(int i = 0; i < 8; i++) pinMode(DATA_START+i, INPUT);
  for(int i = 0; i < 16; i++) pinMode(ADDR_START+i, INPUT);

  pauseClock = true;
}

void printStatus() {
  unsigned int addr = readAddr();
  Serial.print("0x");
  printByteHex((addr >> 8) & 0xFF);  printByteHex(addr & 0xFF);

  byte data = readData();
  Serial.print("\t 0x");
  printByteHex(data);

  byte rwb = digitalRead(RWB);
  if (rwb) Serial.println(" r");
  else Serial.println(" w");
}

void tick() {
  digitalWrite(CLOCK, HIGH);
  printStatus();

  delay(CLOCK_LEN);

  digitalWrite(CLOCK, LOW);
  delay(CLOCK_LEN);
}

void playPauseLoop() {
  if (Serial.available()) {
    Serial.read();
    if (pauseClock) Serial.println("Resume");
    else Serial.println("Pause");
    
    pauseClock = !pauseClock;
  }

  if (pauseClock) {
    delay(100);
    return;
  }
  
  tick();
}

void manualLoop() {
  while (!Serial.available()) {
    delay(100);
  }
  Serial.read();
  
  tick();
}

void loop() {
  playPauseLoop();

}
