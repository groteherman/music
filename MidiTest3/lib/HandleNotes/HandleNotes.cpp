#include <stdint.h>
#include "HandleNotes.h"

uint8_t handleNotesPlayingOff(struct noot_struct *nootjes){
  uint8_t retval = 255;
  int firstNote = nootjes->numberOfNotes - nootjes->polyphony;
  if (firstNote < 0){
    firstNote = 0;
  }
  for (uint8_t j = 0; j < nootjes->polyphony; j++){
    bool turnOff = true;
    for(uint8_t i = firstNote; i < nootjes->numberOfNotes; i++){
      if (nootjes->notesPlaying[j] == nootjes->notesInOrder[i] && nootjes->notesPlaying[j] != 0){
        turnOff = false;
        break;
      }
    }
    if (turnOff && nootjes->notesPlaying[j] != 0){
      nootjes->notesPlaying[j] = 0;
      retval = j;
    }
  }
  return retval;
}

uint8_t handleNotesPlayingOn(struct noot_struct *nootjes){
  uint8_t retval = 255;
  int firstNote = nootjes->numberOfNotes - nootjes->polyphony;
  if (firstNote < 0){
    firstNote = 0;
  }
  for(uint8_t i = firstNote; i < nootjes->numberOfNotes; i++){
    bool isPlaying = false;
    for (uint8_t j = 0; j < nootjes->polyphony; j++){
      if (nootjes->notesPlaying[j] == nootjes->notesInOrder[i]){
        isPlaying = true;
        break;
      }
    }
    if (!isPlaying){
      for (uint8_t j = 0; j < nootjes->polyphony; j++){
        if (nootjes->notesPlaying[j] == 0){
          nootjes->notesPlaying[j] = nootjes->notesInOrder[i];
          retval = j;
          break;
        }
      }
    }
  }
  return retval;
}

void handleNoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity, struct noot_struct *nootjes){
  if (MIDI_LOW <= pitch && pitch < MIDI_LOW + MIDI_NUMBER){
    bool noteFound = false;
    for (uint8_t i = 0; i < nootjes->numberOfNotes; i++){
      if (nootjes->notesInOrder[i] == pitch){
        noteFound = true;
        break;
      }
    }              
    if (!noteFound) {
      if (nootjes->numberOfNotes < MAX_NOTES) {
        nootjes->notesInOrder[nootjes->numberOfNotes++] = pitch;
      } else {
        //alles 1 opschuiven
        for (uint8_t i = 0; i < MAX_NOTES - 1; i++){
          nootjes->notesInOrder[i] = nootjes->notesInOrder[i+1];
        }
        nootjes->notesInOrder[MAX_NOTES - 1] = pitch;
      }
    }
  }
}

void handleNoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity, struct noot_struct *nootjes){
  if (MIDI_LOW <= pitch && pitch < MIDI_LOW + MIDI_NUMBER){
    //note ertussenuit halen
    bool noteFound = false;
    for (uint8_t i = 0; i < nootjes->numberOfNotes; i++){
      if (nootjes->notesInOrder[i] == pitch){
        noteFound = true;
      }
      if (noteFound && i < nootjes->numberOfNotes - 1){
        nootjes->notesInOrder[i] = nootjes->notesInOrder[i+1];
      }
    }
    if (noteFound){
      if (nootjes->numberOfNotes > 0){
        nootjes->numberOfNotes--;
      }
      //notesInOrder[numberOfNotes] = 0;
    }
  }
}

int32_t determinePitchBend(uint8_t lsb, uint8_t msb){
  int16_t bend = (msb << 7) + lsb - 8192;
  int32_t pitchBend;
  if (bend >= 0) {
    pitchBend =  PITCH_FACTOR * bend;
  } else {
    pitchBend =  PITCH_FACTOR * bend / 2;
  }
  return pitchBend;
}

long determineFrequency(int32_t pitchBend, int8_t detune){
  return 100 * FREQUENCY + DETUNE_FACTOR * detune + pitchBend;
}
