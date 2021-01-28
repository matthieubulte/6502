#define RAM_N_OE_PIN 53

void initRAM() {
  pinMode(RAM_N_OE_PIN , OUTPUT);
  digitalWrite(RAM_N_OE_PIN, LOW);
}
