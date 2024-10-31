#include <Arduino.h>

#define LED_PIN 13
#define VACTROL_PIN 2
#define DELAY_MS 20

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, true);
  digitalWrite(VACTROL_PIN, true);
  delay(DELAY_MS);
  digitalWrite(LED_PIN, false);
  digitalWrite(VACTROL_PIN, false);
  delay(DELAY_MS);
}
