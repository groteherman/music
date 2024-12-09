#include <Arduino.h>
#include "Midi.h"
#include "TM1638lite.h"

#define TM_STROBE 15
#define TM_CLOCK 16
#define TM_DATA 17

#define GATE 13 //same as LED_BUILTIN
#define MAX_POLYPHONY 9
#define POLYPHONY 1

#define MAX_NOTES 10
#define CHANNEL 0
#define MIDI_NUMBER 53
#define MIDI_LOW 48

char buffer[9];

byte numberOfNotes = 0;
byte notesPlaying[MAX_POLYPHONY];
byte notesInOrder[MAX_NOTES];

TM1638lite tm(TM_STROBE, TM_CLOCK, TM_DATA);
MIDI_CREATE_DEFAULT_INSTANCE();

void ToTm(byte byte0, byte byte1, byte byte2)
{
  sprintf(buffer, "        ");
  tm.displayText(buffer);
  sprintf(buffer, "%02X %02X %02X", byte0, byte1, byte2);
  tm.displayText(buffer);
}

void handleNotesPlaying(){
  int firstNote = numberOfNotes - POLYPHONY;
  if (firstNote < 0){
    firstNote = 0;
  }
  for (byte j = 0; j < POLYPHONY; j++){
    bool turnOff = true;
    for(byte i = firstNote; i < numberOfNotes; i++){
      if (notesPlaying[j] == notesInOrder[i] && notesPlaying[j] != 0){
        turnOff = false;
        break;
      }
    }
    if (turnOff && notesPlaying[j] != 0){
      notesPlaying[j] = 0;
      //noteOff(j);
      ToTm(j, notesPlaying[j], numberOfNotes);
    }
  }
  if (numberOfNotes == 0){
    digitalWrite(GATE, false);
  }
  for(byte i = firstNote; i < numberOfNotes; i++){
    bool isPlaying = false;
    for (byte j = 0; j < POLYPHONY; j++){
      if (notesPlaying[j] == notesInOrder[i]){
        isPlaying = true;
        break;
      }
    }
    if (!isPlaying){
      for (byte j = 0; j < POLYPHONY; j++){
        if (notesPlaying[j] == 0){
          digitalWrite(GATE, false);
          notesPlaying[j] = notesInOrder[i];
          ToTm(j, notesPlaying[j], numberOfNotes);
          digitalWrite(GATE, true);
          break;
        }
      }
    }
  }
}

void handleNoteOn(byte channel, byte pitch, byte velocity){
  if (MIDI_LOW <= pitch && pitch < MIDI_LOW + MIDI_NUMBER){
    bool noteFound = false;
    for (byte i = 0; i < numberOfNotes; i++){
      if (notesInOrder[i] == pitch){
        noteFound = true;
        break;
      }
    }              
    if (!noteFound) {
      if (numberOfNotes < MAX_NOTES) {
        notesInOrder[numberOfNotes++] = pitch;
      } else {
        //alles 1 opschuiven
        for (byte i = 0; i < MAX_NOTES - 1; i++){
          notesInOrder[i] = notesInOrder[i+1];
        }
        notesInOrder[MAX_NOTES - 1] = pitch;
      }
    }
    handleNotesPlaying();
  }
}

void handleNoteOff(byte channel, byte pitch, byte velocity){
  if (MIDI_LOW <= pitch && pitch < MIDI_LOW + MIDI_NUMBER){
    //note ertussenuit halen
    bool noteFound = false;
    for (byte i = 0; i < numberOfNotes; i++){
      if (notesInOrder[i] == pitch){
        noteFound = true;
      }
      if (noteFound && i < numberOfNotes - 1){
        notesInOrder[i] = notesInOrder[i+1];
      }
    }
    if (noteFound){
      if (numberOfNotes > 0){
        numberOfNotes--;
      }
      //notesInOrder[numberOfNotes] = 0;
    }
    handleNotesPlaying();
  }
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
  
  MIDI.begin(MIDI_CHANNEL_OMNI);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  if (MIDI.read()) {                    
      byte type = MIDI.getType();
      byte byte1 = MIDI.getData1();
      byte byte2 = MIDI.getData2();
      //ToTm(type, byte1, byte2);
      switch (type) {
      case midi::NoteOn: 
        handleNoteOn(CHANNEL, byte1, byte2);
        break;
      case midi::NoteOff: 
        handleNoteOff(CHANNEL, byte1, byte2);
        break;
    }
  }
}