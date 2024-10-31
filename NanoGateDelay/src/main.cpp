#include <Arduino.h>

#define GATEIN_PIN 3
#define GATEOUT_PIN 4
#define MAXARRAY 1024

byte delayArr[MAXARRAY];
unsigned int maxDelay = MAXARRAY -1;

void setup() {
    Serial.begin(115200);
    pinMode(GATEIN_PIN, INPUT);
    pinMode(GATEOUT_PIN, OUTPUT);
}

void loop() {
  for (unsigned int i = 0; i < maxDelay; i++){
    unsigned int byteIndex = i >> 3;
    byte bitIndex = i & 7;
    Serial.print(i);
    Serial.print(";");
    Serial.print(byteIndex);
    Serial.print(";");
    Serial.println(bitIndex);
/*
    digitalWrite(GATEOUT_PIN, delayArr[byteIndex] & 1 << bitIndex);
    if (digitalRead(GATEIN_PIN)) {
      bitSet(delayArr[byteIndex], bitIndex);
    } else {
      bitClear(delayArr[byteIndex], bitIndex);
    }
    */
  }
}
