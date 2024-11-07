#include <Arduino.h>
#include "Midi.h"
#include "TM1638lite.h"

#define TM_DATA 15
#define TM_CLOCK 16
#define TM_STROBE 17
#define LEDPIN D13

char buffer[9];

TM1638lite tm(TM_STROBE, TM_CLOCK, TM_DATA);
MIDI_CREATE_DEFAULT_INSTANCE();

void ToTm(byte pitch, byte velocity)
{
  sprintf(buffer, "        ");
  tm.displayText(buffer);
  sprintf(buffer, "%d %d", pitch, velocity);
  tm.displayText(buffer);
}

void handleNoteOn(byte channel, byte pitch, byte velocity){
  digitalWrite(LED_BUILTIN, HIGH);
  ToTm(pitch, velocity);
}

void handleNoteOff(byte channel, byte pitch, byte velocity){
  digitalWrite(LED_BUILTIN, LOW);
  ToTm(pitch, velocity);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  tm.reset();
  for(byte i=0; i < 8; i++){
    tm.setLED(i, 1);
    delay(200);
  }

  for(byte i=0; i < 8; i++){
    tm.setLED(i, 0);
  }

  for(byte i=0; i < 5; i++){
    sprintf(buffer, "MIDITEST");
    tm.displayText(buffer);
    delay(200);
    sprintf(buffer, "        ");
    tm.displayText(buffer);
    delay(200);
  }
  
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  MIDI.read();
}
