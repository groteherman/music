/*
SPI (nano): MOSI:11 (MISO:12, NC) SCLK:13 CS:5

MCP4822

 VDD+ 1  8 VoutA
  _CS 2  7 VSS-
 SCLK 3  6 VoutB
   SD 4  5 _LDAC
*/

#include <Arduino.h>
#include <MCP48xx.h>

#define GATE_PIN 2
#define DEBUG_PIN 6
#define CS_PIN 5
#define MAXDAC 4095
#define MAXDACD 4095.0
#define ALMOSTZERO 0.01 //0.001

bool gate, previousGate;
byte adcCounter = 0;
bool debugOut = false;

enum EnvelopePhase { Attack, Decay, Sustain, Release, Stopped };

struct DacParams{
  EnvelopePhase envelopePhase;
  double envelope;
  double stepAttack;
  double factorDecay;
  double sustainLevel;
  double factorRelease;
};

DacParams dacParamsA = {
  Stopped
  , 0.0
  , 0.002 //0.0013 //0.0001 ~ 120ms;  step = 0.012 ms / t (ms)
  , 0.98 //0.999 //~80ms
  , 0.5
  , 0.98 // 0.999 //~80ms @ sustainLevel 0.5
};

DacParams dacParamsB = {
  Stopped
  , 0.0
  , 0.00013 //0.0001 ~ 120ms;  step = 0.012 ms / t (ms)
  , 0.999 //~80ms
  , 0.5
  , 0.999 //~80ms @ sustainLevel 0.5
};


MCP4822 dac(CS_PIN);

double TransformAttack(int value){
  return 1.0/(value + 1);
}

double TransformDecay(int value){
  return 1.0/(value + 1);
}

double TransformSustain(int value){
  return value/1024.0;
}

double TransformRelease(int value){
  return 1.0/(value + 1);
}

void UpdateAdcs(DacParams &dacParamsA, DacParams &dacParamsB){
  switch (adcCounter){
    case 0:
      dacParamsA.stepAttack = TransformAttack(analogRead(A0));
      break;
    case 1:
      dacParamsA.factorDecay = TransformDecay(analogRead(A1));
      break;
    case 2:
      dacParamsA.sustainLevel = TransformSustain(analogRead(A2));
      break;
    case 3:
      dacParamsA.factorRelease = TransformRelease(analogRead(A3));
      break;
/*
    case 4:
      dacParamsB.stepAttack = TransformAttack(analogRead(A4));
      break;
    case 5:
      dacParamsB.factorDecay = TransformDecay(analogRead(A5));
      break;
    case 6:
      dacParamsB.sustainLevel = TransformSustain(analogRead(A6));
      break;
    case 7:
      dacParamsB.factorRelease = TransformRelease(analogRead(A7));
      break;
  */
  }
  adcCounter++;
  if (adcCounter > 3){
    adcCounter = 0;
  }
  Serial.print(dacParamsA.stepAttack);
  Serial.print("; ");
  Serial.print(dacParamsA.factorDecay);
  Serial.print("; ");
  Serial.print(dacParamsA.sustainLevel);
  Serial.print("; ");
  Serial.print(dacParamsA.factorRelease);
  Serial.print("; ");
  Serial.println(dacParamsA.envelope);
}

void UpdateEnvelope(DacParams &dacParams, bool gate, bool previousGate){
  if (gate && !previousGate){
    dacParams.envelopePhase = Attack;
  }
  if (dacParams.envelopePhase == Attack){
    dacParams.envelope += dacParams.stepAttack;
    if (dacParams.envelope >= 1.0){
      dacParams.envelopePhase = Decay;
      dacParams.envelope = 1.0;
    }
  }
  if (dacParams.envelopePhase == Decay){
    dacParams.envelope = (dacParams.envelope - dacParams.sustainLevel) * dacParams.factorDecay + dacParams.sustainLevel;
    if (dacParams.envelope <= dacParams.sustainLevel + ALMOSTZERO){
      dacParams.envelopePhase = Sustain;
    }
  }
  if (dacParams.envelopePhase == Sustain){
    dacParams.envelope = dacParams.sustainLevel;
  }
  if (dacParams.envelopePhase != Release && dacParams.envelopePhase != Stopped && !gate){
      dacParams.envelopePhase = Release;
  }
  if (dacParams.envelopePhase == Release){
    dacParams.envelope *= dacParams.factorRelease;
    if (dacParams.envelope <= ALMOSTZERO) {
      dacParams.envelopePhase = Stopped;
      dacParams.envelope = 0.0;
    }
  }
  //Serial.print(dacParams.envelopePhase);
  //Serial.println(dacParams.envelope);

}

void setup() {
  Serial.begin(115200);
  
  dac.init();
  dac.turnOnChannelA();
  //dac.turnOnChannelB();
  dac.setGainA(MCP4822::High);
  //dac.setGainB(MCP4822::Low);
  //adcAttachPin(A0);
  //analogReadResolution(12);
  pinMode(GATE_PIN, INPUT);
  pinMode(DEBUG_PIN, OUTPUT);
  gate = false;
  previousGate = false;
}


void loop() {
//  for (int i = 0; i <= MAXDAC; i++){
    //only dac: loop in ~80ms; 46855/s
    //dac + digitalWrite: 38700/s
    //dac + digitalRead: 100ms; 40960/s
    //dac + digitalRead + analogRead: 560ms; 7314/s
//    debugOut = !debugOut;
//    digitalWrite(DEBUG_PIN, debugOut);
//    gate = digitalRead(GATE_PIN);
//    dac.setVoltageA(i);
//    dac.updateDAC();
//    UpdateAdcs(dacParamsA, dacParamsB);
//  }
  previousGate = gate;
  gate = digitalRead(GATE_PIN);
  gate = (millis() % 1000 < 500); //generate internal gate
  //digitalWrite(DEBUG_PIN, gate);
  UpdateEnvelope(dacParamsA, gate, previousGate);
  //UpdateEnvelope(dacParamsB, gate, previousGate);
  dac.setVoltageA(int(dacParamsA.envelope * MAXDACD));
  //dac.setVoltageB(dacParamsB.envelope * MAXDAC);
  dac.updateDAC();
  UpdateAdcs(dacParamsA, dacParamsB);
}
