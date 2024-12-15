//#define DODEBUG 1

#include <Arduino.h>
#include "Midi.h"
#include "si5351.h"
#include "SN76489.h"
#include "HandleNotes.h"

#define TM_STROBE 15 //A1
#define TM_CLOCK 16  //A2
#define TM_DATA 17   //A3

#ifdef DODEBUG
#include "TM1638lite.h"
TM1638lite tm(TM_STROBE, TM_CLOCK, TM_DATA);
#endif

MIDI_CREATE_DEFAULT_INSTANCE();
byte whichSN[3] = {PIN_NotCE0, PIN_NotCE1, PIN_NotCE2};

Si5351 si5351;
SN76489 mySN76489 = SN76489(PIN_NotWE, PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6, PIN_D7, FREQUENCY);

uint16_t noteDiv[MIDI_NUMBER] = {
1024,967,912,861,813,767,724,683,645,609,575,542
,512,483,456,431,406,384,362,342,323,304,287,271
,256,242,228,215,203,192,181,171,161,152,144,136
,128,121,114,108,102,96,91,85,81,76,72,68
,64,60,57,54,51};

noot_struct nootjes = { 0, 1, {}, {} };

#ifdef DODEBUG
char buffer[9];

void ToTm(byte byte0, byte byte1, byte byte2)
{
  sprintf(buffer, "        ");
  tm.displayText(buffer);
  sprintf(buffer, "%02X %02X %02X", byte0, byte1, byte2);
  tm.displayText(buffer);
}
#endif

void noteOn(byte index, byte msg, byte polyphony){
//  if (Program.GetPolyphony() > 3){
  if (polyphony > 3){
    digitalWrite(whichSN[index % 3], false);
    mySN76489.setDivider(index / 3, noteDiv[msg - MIDI_LOW]);
    mySN76489.setAttenuation(index / 3, 0x0);
    digitalWrite(whichSN[index % 3], true);
  } else {
    digitalWrite(whichSN[0], false);
    digitalWrite(whichSN[1], false);
    digitalWrite(whichSN[2], false);
    mySN76489.setDivider(index % 3, noteDiv[msg - MIDI_LOW]);
    mySN76489.setAttenuation(index % 3, 0x0);
    digitalWrite(whichSN[0], true);
    digitalWrite(whichSN[1], true);
    digitalWrite(whichSN[2], true);
  }
}

void noteOff(byte index, byte polyphony){
  //if (Program.GetPolyphony() > 3){
  if (polyphony > 3){
    digitalWrite(whichSN[index % 3], false);
    mySN76489.setAttenuation(index / 3, 0xf);
    digitalWrite(whichSN[index % 3], true);
} else {
    digitalWrite(whichSN[0], false);
    digitalWrite(whichSN[1], false);
    digitalWrite(whichSN[2], false);
    mySN76489.setAttenuation(index % 3, 0xf);
    digitalWrite(whichSN[0], true);
    digitalWrite(whichSN[1], true);
    digitalWrite(whichSN[2], true);
  }
}

void AllOff(noot_struct *nootjes){
  digitalWrite(PIN_NotCE0, false);
  digitalWrite(PIN_NotCE1, false);
  digitalWrite(PIN_NotCE2, false);
  mySN76489.setAttenuation(0, 0xF);
  mySN76489.setAttenuation(1, 0xF);
  mySN76489.setAttenuation(2, 0xF);
  mySN76489.setAttenuation(3, 0xF);
  digitalWrite(PIN_NotCE0, true);
  digitalWrite(PIN_NotCE1, true);
  digitalWrite(PIN_NotCE2, true);
  for(byte i=0; i < MAX_POLYPHONY; i++){
    nootjes->notesPlaying[i] = 0;
  }
  for(byte i=0; i < MAX_NOTES; i++){
    nootjes->notesInOrder[i] = 0;
  }
  nootjes->numberOfNotes = 0;
  digitalWrite(GATE, false);
}

//handleNoteOn can lead to another note needing to turn off as well as a new note turning on
//handleNoteOff can lead to turn this note off but then some other note turning on
void handleNotesPlaying(noot_struct *nootjes){
  byte toTurnOff = handleNotesPlayingOff(nootjes);
  if (toTurnOff < 255){
    //noteOff(toTurnOff, nootjes->polyphony);
  }
  if (nootjes->numberOfNotes == 0){
    digitalWrite(GATE, false);
  }
  byte toTurnOn = handleNotesPlayingOn(nootjes);
  if (toTurnOn < 255){
    noteOn(toTurnOn, nootjes->notesPlaying[toTurnOn], nootjes->polyphony);
    digitalWrite(GATE, true);
  }
  #ifdef DODEBUG
      ToTm(j, notesPlaying[j], numberOfNotes);
  #endif
}

void handlePitchBend(uint8_t byte1, uint8_t byte2, noot_struct *nootjes){
  int32_t pitchBend = determinePitchBend(byte1, byte2);
  si5351.set_freq(determineFrequency(pitchBend, nootjes->detune0), SI5351_CLK0);
  si5351.set_freq(determineFrequency(pitchBend, nootjes->detune1), SI5351_CLK1);
  si5351.set_freq(determineFrequency(pitchBend, nootjes->detune2), SI5351_CLK2);
}

void handleControlChange(uint8_t channel, uint8_t controller, uint8_t value, noot_struct *nootjes){
  switch (controller) {
    case NOVATION_DETUNE :
        nootjes->detune0 = value - 64;
        si5351.set_freq(determineFrequency(0, nootjes->detune0), SI5351_CLK0);
        break;
    case NOVATION_LEVEL :
        nootjes->detune1 = value - 64;
        si5351.set_freq(determineFrequency(0, nootjes->detune1), SI5351_CLK1);
        break;
    case NOVATION_PWM :
        nootjes->detune2 = value - 64;
        si5351.set_freq(determineFrequency(0, nootjes->detune2), SI5351_CLK2);
        break;
    }
}


void setup() {
  pinMode(GATE, OUTPUT);
  digitalWrite(GATE, HIGH);

  pinMode(PIN_NotCE0, OUTPUT); 
  pinMode(PIN_NotCE1, OUTPUT); 
  pinMode(PIN_NotCE2, OUTPUT); 

#ifdef DODEBUG
  tm.reset();
  for(byte i=0; i < 8; i++){
    tm.setLED(i, 1);
    delay(50);
  }
  for(byte i=0; i < 8; i++){
    tm.setLED(i, 0);
    delay(50);
  }
#endif  

  bool i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  if(!i2c_found)
  {
    //Serial.println("Device not found on I2C bus!");
  }  
  si5351.set_freq(100 * FREQUENCY, SI5351_CLK0);
  si5351.set_freq(100 * FREQUENCY, SI5351_CLK1);
  si5351.set_freq(100 * FREQUENCY, SI5351_CLK2);

  AllOff(&nootjes);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  digitalWrite(GATE, LOW);
}

void loop() {
  if (MIDI.read()) {                    
    byte type = MIDI.getType();
    byte byte1 = MIDI.getData1();
    byte byte2 = MIDI.getData2();
#ifdef DODEBUG
    ToTm(type, byte1, byte2);
#endif
    switch (type) {
      case midi::NoteOn: 
        handleNoteOn(CHANNEL, byte1, byte2, &nootjes);
        handleNotesPlaying(&nootjes);
        break;
      case midi::NoteOff: 
        handleNoteOff(CHANNEL, byte1, byte2, &nootjes);
        handleNotesPlaying(&nootjes);
        break;
      case midi::PitchBend:
        handlePitchBend(byte1, byte2, &nootjes);
        break;
      case midi::ControlChange:
        handleControlChange(CHANNEL, byte1, byte2, &nootjes);
        break;

    }
  }
}