#include <Arduino.h>
#include <RotaryEncoder.h>
#include <digitalWriteFast.h>
#include "NanoGateDelay.h"

#define PIN_IN1 A2
#define PIN_IN2 A3
#define LEDSTRIP1_PIN 9

#define INT_DIVIDER_100K 134
#define INT_DIVIDER_1K 15984

uint8_t delayArr[MAXARRAY] = {};
uint16_t maxDelay = MAXARRAY * 8;
uint16_t actualDelay = maxDelay;
volatile uint16_t bitIndex;
bool justPressed;

RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

ISR(TIMER1_COMPA_vect)
{
  digitalWriteFast(LED_PIN, HIGH);

  bitIndex++;
  if (bitIndex >= actualDelay){
    bitIndex = 0;
  }
  bool gateIn = digitalReadFast(GATEIN_PIN);
  bool gateOut = getAndSetBit(bitIndex, gateIn, delayArr);
  if (gateOut) {
    digitalWriteFast(GATEOUT_PIN, HIGH);
  } else {
    digitalWriteFast(GATEOUT_PIN, LOW);
  }

  digitalWriteFast(LED_PIN, LOW);
}

void setup() {
  Serial.begin(9600);

  pinMode(GATEIN_PIN, INPUT);
  pinMode(GATEOUT_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LEDSTRIP1_PIN, OUTPUT);

  bitIndex = 0;
  // set up Timer 1
  TCCR1A = 0;          // normal operation
  //125 µS, giving a frequency of 1/0.000125 = 8 KHz
  TCCR1B = bit(WGM12) | bit(CS10);   // CTC, no pre-scaling
  OCR1A = 15984;       // compare A register value (1000 * clock speed / 1024)   15984 = 1kHz
  TIMSK1 = bit (OCIE1A);             // interrupt on Compare A Match

  justPressed = false;
}

void loop() {
  static int pos = 0;
  static int divider = INT_DIVIDER_1K /2;

  encoder.tick();

  bool isPressed = !digitalRead(SWITCH_PIN);
  if (isPressed){
    if(!justPressed){
      digitalWrite (LEDSTRIP1_PIN, HIGH);
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

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    Serial.print("pos:");
    Serial.print(newPos);
    Serial.print(" dir:");
    Serial.println((int)(encoder.getDirection()));
    pos = newPos;
    divider = INT_DIVIDER_1K + 50 * pos;
    if (divider > INT_DIVIDER_1K) {
      OCR1A = INT_DIVIDER_1K;
    } else if (divider < INT_DIVIDER_100K) {
      OCR1A = INT_DIVIDER_100K;
    } else {
      OCR1A = divider;
    }
  }
  //Serial.println(actualDelay);
  digitalWrite (LEDSTRIP1_PIN, LOW);
}
