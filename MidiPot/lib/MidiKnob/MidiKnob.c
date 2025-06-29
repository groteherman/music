#include <stdint.h>
#include <stdbool.h>
#include "MidiKnob.h"

bool InNoMansLand(uint16_t potValue) {
  return potValue & 4;
}

uint8_t MidiFromPot(uint16_t potValue, uint8_t midiValue) {
  uint8_t midiFromPot = potValue >> 3;
  if (InNoMansLand(potValue) && (midiFromPot == midiValue || midiFromPot + 1 == midiValue)) {
    return midiValue;
  }
  return midiFromPot;
}
