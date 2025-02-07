/*
Copyright (c) <2025> <Herman de Groot>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <Arduino.h>
#include <digitalWriteFast.h>
#include "GateDelay.h"

#define INT_DIVIDER_MIN 150
#define ANALOG_COURSE A8
#define ANALOG_FINE A9

uint32_t actualDivider = 0;

uint8_t delayArr[MAXARRAY] = {};
uint16_t maxDelay = MAXARRAY * 8;

uint16_t delayValues[8] = {1, 4, 16, 64, 256, 1024, 4096, 8192 };
uint8_t delayIndex = 7;
uint16_t actualDelay = delayValues[delayIndex];

volatile uint16_t bitIndex;
volatile bool gateOut;

ISR(TIMER1_COMPA_vect)
{
  if (gateOut) {
    digitalWriteFast(GATEOUT_PIN, HIGH);
  } else {
    digitalWriteFast(GATEOUT_PIN, LOW);
  }

  bitIndex++;
  if (bitIndex >= actualDelay){
    bitIndex = 0;
  }
  bool gateIn = digitalReadFast(GATEIN_PIN);
  gateOut = getAndSetBit(bitIndex, gateIn, delayArr);
}

void setup() {
//  Serial.begin(9600);

  pinMode(GATEIN_PIN, INPUT);
  pinMode(GATEOUT_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  gateOut = false;
  bitIndex = 0;
  // set up Timer 1
  TCCR1A = 0;          // normal operation
  //125 µS, giving a frequency of 1/0.000125 = 8 KHz
  TCCR1B = bit(WGM12) | bit(CS10);   // CTC, no pre-scaling
  OCR1A = 15984;       // compare A register value (1000 * clock speed / 1024)   15984 = 1kHz
  TIMSK1 = bit (OCIE1A);             // interrupt on Compare A Match
}

void loop() {

  uint16_t newVal = 0;
  for (byte i = 0; i < 4; i++){
    newVal += analogRead(ANALOG_COURSE);
  }
  newVal = newVal >> 9;
  if (newVal != delayIndex){
    delayIndex = newVal;
    actualDelay = delayValues[delayIndex];
    //Serial.print("delayIndex: ");
    //Serial.println(delayIndex);
  }

  uint32_t newDivider = 0;
  for (byte i=0; i<16; i++){
    newDivider += analogRead(ANALOG_FINE);
  }
  if (abs(newDivider - actualDivider) > 64){
    actualDivider = newDivider;
    uint32_t temp = INT_DIVIDER_MIN + ((actualDivider * actualDivider) >> 14);
    OCR1A = temp;
    //Serial.print("P_interrupt: ");
    //Serial.println(temp);
  }
  if (gateOut){
    Serial.println("1");
  } else {
    Serial.println("0");
  }
}