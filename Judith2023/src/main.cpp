#include <Arduino.h>

#define PINPLAY A5
#define PINMOTION 8
#define PINLED 13
#define PINLEDA DD5
#define PINLEDB DD6


void Links(){
  digitalWrite(PINLEDA, HIGH);
  digitalWrite(PINLEDB, LOW);
}

void Rechts(){
  digitalWrite(PINLEDA, LOW);
  digitalWrite(PINLEDB, HIGH);
}

void Uit(){
  digitalWrite(PINLEDA, LOW);
  digitalWrite(PINLEDB, LOW);
}

void setup() {
  pinMode(PINPLAY, OUTPUT);
  pinMode(PINMOTION, INPUT);
  pinMode(PINLED, OUTPUT);
  pinMode(PINLEDA, OUTPUT);
  pinMode(PINLEDB, OUTPUT);
  digitalWrite(PINPLAY, LOW);
  digitalWrite(PINLED, LOW);
  digitalWrite(PINLEDA, LOW);
  digitalWrite(PINLEDB, LOW);
}

void loop() {
  if (digitalRead(PINMOTION)) {
    digitalWrite(PINLED, HIGH);
    digitalWrite(PINPLAY, HIGH);
    delay(50);
    digitalWrite(PINPLAY, LOW);
    digitalWrite(PINLED, LOW);

    for(int i = 0; i< 10; i++) {
      Links();
      delay(250);
      Rechts();
      delay(250);
    }
    Uit();
  } else {
    digitalWrite(PINLED, LOW);
  }
}