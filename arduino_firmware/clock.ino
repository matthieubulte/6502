#define CLOCK_PIN 24

void initClock() {
  pinMode(CLOCK_PIN, OUTPUT);
}

void clockLow() {
  digitalWrite(CLOCK_PIN, LOW);
}

void clockHigh() {
  digitalWrite(CLOCK_PIN, HIGH);
}
