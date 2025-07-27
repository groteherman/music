#include <Arduino.h>
#include "MidiKnob.h"

int analogPin = A0;
uint16_t val = 0;
uint8_t midiVal = 0;

void setup() {
  Serial.begin(9600);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

void loop() {
  val = analogRead(analogPin);
  midiVal = MidiKnob::MidiFromPot(val, midiVal);
  Serial.print(val);
  Serial.print(" -> MIDI Value: ");
  Serial.print(midiVal);
  Serial.print(" A1(light): ");
  Serial.print(analogRead(A1));
  Serial.print(" A2(temp): ");
  Serial.println(analogRead(A2));
  digitalWrite(9, midiVal & 1);
  digitalWrite(10, midiVal & 2);
  digitalWrite(11, midiVal & 4);
  delay(200);
}