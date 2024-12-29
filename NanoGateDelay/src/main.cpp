#include <Arduino.h>
#include <digitalWriteFast.h>
#include "NanoGateDelay.h"

#define INT_DIVIDER_MIN 150
uint32_t actualDivider = 0;

uint8_t delayArr[MAXARRAY] = {};
uint16_t maxDelay = MAXARRAY * 8;
uint16_t actualDelay = maxDelay;
volatile uint16_t bitIndex;
volatile bool gateOut;
bool justPressed;


ISR(TIMER1_COMPA_vect)
{
  digitalWriteFast(LED_PIN, HIGH);

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

  digitalWriteFast(LED_PIN, LOW);
}

void setup() {
  Serial.begin(9600);

  pinMode(GATEIN_PIN, INPUT);
  pinMode(GATEOUT_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  justPressed = false;
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
  bool isPressed = !digitalRead(SWITCH_PIN);
  if (isPressed){
    if(!justPressed){
      justPressed = true;
      if (actualDelay > 4){
        actualDelay = actualDelay >> 2;
      } else {
        actualDelay = maxDelay;
      }
    }
  } else {
    justPressed = false;
  }

  uint32_t newDivider = 0;
  for (byte i=0; i<16; i++){
    newDivider += analogRead(A0);
  }
  if (abs(newDivider - actualDivider) > 16){
    actualDivider = newDivider;
    OCR1A = INT_DIVIDER_MIN + ((actualDivider * actualDivider) >> 14);
    Serial.println(actualDivider);
  }
}