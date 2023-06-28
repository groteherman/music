/*
    Envelope generator for Arduino DUE 

    Copyright (C) 2023  Herman de Groot

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#define MAXDAC 4095
#define GATEPORT 2
#define ALMOSTZERO 0.001

double stepAttack = 0.000013; //0.0001 ~ 120ms;  step = 0.012 ms / t (ms)
double factorDecay = 0.999; //~80ms
double sustainLevel = 0.5;
double factorRelease = 0.999; //~80ms @ sustainLevel 0.5

double currentFactor;
double envelope = 0.0;
bool gate, previousGate;

enum EnvelopePhase { Attack, Decay, Sustain, Release, Stopped };
EnvelopePhase envelopePhase = Stopped;

void setup() 
{
  analogWriteResolution(12);
  pinMode(GATEPORT, OUTPUT);
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
    analogWrite(DAC1, envelope * MAXDAC);
}
