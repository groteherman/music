#include <Arduino.h>
#include <MCP48xx.h>

//VSPI:	MOSI: GPIO23 MISO: GPIO19 SCLK:GPIO18 CS:GPIO5
#define GATE_PIN 2
#define CS_PIN 5
#define MAXDAC 4095
#define ALMOSTZERO 0.001

double stepAttack = 0.00013; //0.0001 ~ 120ms;  step = 0.012 ms / t (ms)
double factorDecay = 0.999; //~80ms
double sustainLevel = 0.5;
double factorRelease = 0.999; //~80ms @ sustainLevel 0.5

double currentFactor;
double envelope = 0.0;
bool gate, previousGate;

enum EnvelopePhase { Attack, Decay, Sustain, Release, Stopped };
EnvelopePhase envelopePhase = Stopped;

MCP4822 dac(CS_PIN);

bool isHigh = true;

void setup() {
  //Serial.begin(115200);
  dac.init();
  dac.turnOnChannelA();
  dac.turnOnChannelB();
  dac.setGainA(MCP4822::Low);
  dac.setGainB(MCP4822::Low);
  //adcAttachPin(A0);

  pinMode(GATE_PIN, INPUT);
  gate = false;
  previousGate = false;
}

void loop() {
  previousGate = gate;
  gate = (millis() % 2000 < 1000); //generate internal gate
  //gate = digitalRead(GATEPORT);
  if (gate && !previousGate){
    envelopePhase = Attack;
  }
  if (envelopePhase == Attack){
    envelope += stepAttack;
    if (envelope >= 1.0){
      envelopePhase = Decay;
      envelope = 1.0;
    }
  }
  if (envelopePhase == Decay){
    envelope = (envelope - sustainLevel) * factorDecay + sustainLevel;
    if (envelope <= sustainLevel + ALMOSTZERO){
      envelopePhase = Sustain;
    }
  }
  if (envelopePhase == Sustain){
    envelope = sustainLevel;
  }
  if (envelopePhase != Release && envelopePhase != Stopped && !gate){
      envelopePhase = Release;
  }
  if (envelopePhase == Release){
    envelope *= factorRelease;
    if (envelope <= ALMOSTZERO) {
      envelopePhase = Stopped;
      envelope = 0.0;
    }
  }
  dac.setVoltageA(envelope * MAXDAC);
  dac.updateDAC();
}
