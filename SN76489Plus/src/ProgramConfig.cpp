#include <Arduino.h>
#include "ProgramConfig.h"

int ProgramConfig::DetermineDetune(int detune){
    if (detune > 100){
    return 100;
    } else if (detune < -100) {
    return -100;
    } else {
    return detune;
    }
}

void ProgramConfig::SetMidiChannel(byte channel){
    MidiChannel = min(channel, 15);
}

byte ProgramConfig::GetMidiChannel(){
    return MidiChannel;
}

void ProgramConfig::SetPolyphony(byte poly){
    if (poly == 9) {
    Polyphony = 9;
    } else if (poly == 3){
    Polyphony = 3;
    } else {
    Polyphony = 1;
    }
}

byte ProgramConfig::GetPolyphony(){
    return Polyphony;
}

void ProgramConfig::SetDetune0(int detune){
    Detune0 = DetermineDetune(detune);
}

int ProgramConfig::GetDetune0(){
    return Detune0;
}

void ProgramConfig::SetDetune1(int detune){
    Detune1 = DetermineDetune(detune);
}

int ProgramConfig::GetDetune1(){
    return Detune1;
}

void ProgramConfig::SetDetune2(int detune){
    Detune2 = DetermineDetune(detune);
}

int ProgramConfig::GetDetune2(){
    return Detune2;
}

ProgramConfig::ProgramConfig(byte midiChannel, byte polyphony, int detune0, int detune1, int detune2){
    SetMidiChannel(midiChannel);
    SetPolyphony(polyphony);
    SetDetune0(detune0);
    SetDetune1(detune1);
    SetDetune2(detune2);
}
