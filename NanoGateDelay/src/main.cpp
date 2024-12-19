#include <Arduino.h>
#include "NanoGateDelay.h"

uint8_t delayArr[MAXARRAY] = {};
uint16_t maxDelay = MAXARRAY * 8;
uint16_t actualDelay = maxDelay;
volatile uint16_t bitIndex;

ISR(TIMER1_COMPA_vect)
{
  digitalWrite (LED_PIN, HIGH);

  bitIndex++;
  if (bitIndex >= actualDelay){
    bitIndex = 0;
  }
  bool gateIn = digitalRead(GATEIN_PIN);
  bool gateOut = getAndSetBit(bitIndex, gateIn, delayArr);
  digitalWrite(GATEOUT_PIN, gateOut);

  digitalWrite (LED_PIN, LOW);
}

void setup() {
  Serial.begin(9600);

  pinMode(GATEIN_PIN, INPUT);
  pinMode(GATEOUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  bitIndex = 0;
  // set up Timer 1
  TCCR1A = 0;          // normal operation
  //125 µS, giving a frequency of 1/0.000125 = 8 KHz
  TCCR1B = bit(WGM12) | bit(CS10);   // CTC, no pre-scaling
  OCR1A =  15984;       // compare A register value (1000 * clock speed / 1024)   15984 = 1kHz
  TIMSK1 = bit (OCIE1A);             // interrupt on Compare A Match
}

void loop() {
  unsigned long sum = 0;

  for (byte i=0; i<16; i++){
    sum += analogRead(A0);
  }

  uint16_t newDelay = sum>>1;
  if (abs(newDelay - actualDelay) > 8){
    actualDelay = newDelay;
  }

  Serial.println(actualDelay);
}
