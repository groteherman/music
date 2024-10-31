#include <Arduino.h>

int led = 13;

void setup() {                
  pinMode(led, OUTPUT);
}

void loop() {
  for (int i = 0; i < 3; i++){
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
  }
  for (int i = 0; i < 3; i++){
    digitalWrite(led, HIGH);
    delay(200);
    digitalWrite(led, LOW);
    delay(200);
  }
  for (int i = 0; i < 3; i++){
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
  }
  delay(1000);
}