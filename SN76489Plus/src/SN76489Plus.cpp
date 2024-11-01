/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Arduino.h>
#include "si5351.h"
#include "Wire.h"
#include "SN76489.h"
#include "Midi.h"

/***************************************************************************
***** Directly interface SN76489 IC with the following PIN definitions *****
***** and by calling 8-bit constractor                                 *****
***** The SN76489 pinout considered for this library is as follows:    *****
*****                                                                  *****
*****                        ========                                  *****
*****        D2       --> [ 1]  ()  [16] <-- VCC                       *****
*****        D1       --> [ 2]      [15] <-- D3                        *****
*****        D0       --> [ 3]  7   [14] <-- CLOCK OSC                 *****
*****     READY       <-- [ 4]  6   [13] <-- D4                        *****
*****    NOT WE       --> [ 5]  4   [12] <-- D5                        *****
*****    NOT CE       --> [ 6]  8   [11] <-- D6                        *****
***** AUDIO OUT       <-- [ 7]  9   [10] <-- D7                        *****
*****       GND       --> [ 8]      [ 9] --- N.C.                      *****
*****                        ========                                  *****
***************************************************************************/

//#define FREQUENCY 4286819ULL
#define FREQUENCY 2143409ULL 
#define DETUNE_FACTOR 107170ULL //FREQ / 20
#define PITCH_FACTOR 26167ULL //100 * FREQ / 8191
#define GATE 13

#define NOVATION_DETUNE 41
#define NOVATION_LEVEL 72
#define NOVATION_PWM 45

#define PIN_NotCE0 8
#define PIN_NotCE1 2
#define PIN_NotCE2 14

#define PIN_NotWE 9
#define PIN_D0 11
#define PIN_D1 12
#define PIN_D2 10
#define PIN_D3 7
#define PIN_D4 6
#define PIN_D5 5
#define PIN_D6 4
#define PIN_D7 3

//impliciet bij een NANO
//Si5351_SDA 18; A4
//Si5351_SCL 19; A5
//MIDI_IN RX 0

//pins not used: 1 (TX) A6 and A7 (analogue in only), 15, 16, 17 (used before by TM1638)

SN76489 mySN76489 = SN76489(PIN_NotWE, PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6, PIN_D7, FREQUENCY);
byte whichSN[3] = {PIN_NotCE0, PIN_NotCE1, PIN_NotCE2};

Si5351 si5351;
MIDI_CREATE_DEFAULT_INSTANCE();

#define MIDI_NUMBER 53
#define MIDI_LOW 48
//C#-code to calculate midinote => divider
//freq = Math.Pow(2, (midiNote-69.0)/12.0) * 440.0
//notediv[midiNote - MIDI_LOW] = Math.Round(FREQUENCY/(32.0 * freq), MidpointRounding.AwayFromZero)
uint16_t noteDiv[MIDI_NUMBER] = {
1024,967,912,861,813,767,724,683,645,609,575,542
,512,483,456,431,406,384,362,342,323,304,287,271
,256,242,228,215,203,192,181,171,161,152,144,136
,128,121,114,108,102,96,91,85,81,76,72,68
,64,60,57,54,51};

#define MAX_POLYPHONY 9
#define MAX_NOTES 10

volatile byte numberOfNotes = 0;
volatile byte notesPlaying[MAX_POLYPHONY];
volatile byte notesInOrder[MAX_NOTES];

#define MAX_PROGRAMCONFIGS 4

class ProgramConfig {
  private:
    byte MidiChannel;
    byte Polyphony;
    int Detune0;
    int Detune1;
    int Detune2;

    int DetermineDetune(int detune){
      if (detune > 100){
        return 100;
      } else if (detune < -100) {
        return -100;
      } else {
        return detune;
      }
    }

  public:
    void SetMidiChannel(byte channel){
      MidiChannel = min(channel, 15);
    }
    byte GetMidiChannel(){
      return MidiChannel;
    }

    void SetPolyphony(byte poly){
      if (poly == 9) {
        Polyphony = 9;
      } else if (poly == 3){
        Polyphony = 3;
      } else {
        Polyphony = 1;
      }
    }
    byte GetPolyphony(){
      return Polyphony;
    }

    void SetDetune0(int detune){
      Detune0 = DetermineDetune(detune);
    }
    int GetDetune0(){
      return Detune0;
    }

    void SetDetune1(int detune){
      Detune1 = DetermineDetune(detune);
    }
    int GetDetune1(){
      return Detune1;
    }

    void SetDetune2(int detune){
      Detune2 = DetermineDetune(detune);
    }
    int GetDetune2(){
      return Detune2;
    }

    ProgramConfig(byte midiChannel, byte polyphony, int detune0, int detune1, int detune2){
      SetMidiChannel(midiChannel);
      SetPolyphony(polyphony);
      SetDetune0(detune0);
      SetDetune1(detune1);
      SetDetune2(detune2);
    }
};

ProgramConfig Program = ProgramConfig(1, 1, 0, 0, 0);

void noteOn(byte index, byte msg){
  if (Program.GetPolyphony() > 3){
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

void noteOff(byte index){
  if (Program.GetPolyphony() > 3){
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

void AllOff(){
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
    notesPlaying[i] = 0;
  }
  for(byte i=0; i < MAX_NOTES; i++){
    notesInOrder[i] = 0;
  }
  numberOfNotes = 0;
  digitalWrite(GATE, false);
}

void handleNotesPlaying(){
  int firstNote = numberOfNotes - Program.GetPolyphony();
  if (firstNote < 0){
    firstNote = 0;
  }
  for (byte j = 0; j < Program.GetPolyphony(); j++){
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
    }
  }
  if (numberOfNotes == 0){
    digitalWrite(GATE, false);
  }
  for(byte i = firstNote; i < numberOfNotes; i++){
    bool isPlaying = false;
    for (byte j = 0; j < Program.GetPolyphony(); j++){
      if (notesPlaying[j] == notesInOrder[i]){
        isPlaying = true;
        break;
      }
    }
    if (!isPlaying){
      for (byte j = 0; j < Program.GetPolyphony(); j++){
        if (notesPlaying[j] == 0){
          digitalWrite(GATE, false);
          notesPlaying[j] = notesInOrder[i];
          noteOn(j, notesPlaying[j]);
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
      numberOfNotes--;
    }
    handleNotesPlaying();
  }
}

void handlePitchBend(byte channel, int bend){
  long pitchBend;
  if (bend >= 0) {
    pitchBend = PITCH_FACTOR * (bend);
  } else {
    pitchBend = (PITCH_FACTOR * (bend)) >> 1;
  }
  si5351.set_freq(100 * FREQUENCY + DETUNE_FACTOR * Program.GetDetune0() + pitchBend, SI5351_CLK0);
  si5351.set_freq(100 * FREQUENCY + DETUNE_FACTOR * Program.GetDetune1() + pitchBend, SI5351_CLK1);
  si5351.set_freq(100 * FREQUENCY + DETUNE_FACTOR * Program.GetDetune2() + pitchBend, SI5351_CLK2);
}

void handleControlChange(byte channel, byte byte1, byte byte2){
  switch (byte1) {
    case NOVATION_DETUNE :
        Program.SetDetune0(byte2 - 64);
        si5351.set_freq(100 * FREQUENCY + DETUNE_FACTOR * Program.GetDetune0(), SI5351_CLK0);
        break;
    case NOVATION_LEVEL :
        Program.SetDetune1(byte2 - 64);
        si5351.set_freq(100 * FREQUENCY + DETUNE_FACTOR * Program.GetDetune1(), SI5351_CLK1);
        break;
    case NOVATION_PWM :
        Program.SetDetune2(byte2 - 64);
        si5351.set_freq(100 * FREQUENCY + DETUNE_FACTOR * Program.GetDetune2(), SI5351_CLK2);
        break;
    }
}

void handleProgramChange(byte channel, byte program){
  switch (program) {
    case 0 :
      Program = ProgramConfig(1, 1, 0, 0, 0);
    case 1 :
      Program = ProgramConfig(1, 3, 0, 0, 0);
    case 2 :
      Program = ProgramConfig(1, 9, 0, 0, 0);
  }
  handleControlChange(Program.GetMidiChannel(), NOVATION_DETUNE, 64);
  handleControlChange(Program.GetMidiChannel(), NOVATION_LEVEL, 64);
  handleControlChange(Program.GetMidiChannel(), NOVATION_PWM, 64);
  AllOff();
}

void setup()
{
  bool i2c_found;
  //Serial.begin(9600);
  //Serial.println("");
  i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  if(!i2c_found)
  {
    //Serial.println("Device not found on I2C bus!");
  }
  pinMode(PIN_NotCE0, OUTPUT); 
  pinMode(PIN_NotCE1, OUTPUT); 
  pinMode(PIN_NotCE2, OUTPUT); 
  pinMode(GATE, OUTPUT); 

  digitalWrite(PIN_NotCE0, false);
  mySN76489.setDivider(0, noteDiv[0]);
  delay(100);
  mySN76489.setDivider(0, noteDiv[11]);
  delay(100);
  mySN76489.setDivider(0, noteDiv[23]);
  delay(100);
  mySN76489.setAttenuation(0, 0xF);

  AllOff();
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandlePitchBend(handlePitchBend);
  MIDI.setHandleControlChange(handleControlChange);
  MIDI.setHandleProgramChange(handleProgramChange);
}

void loop()
{
  MIDI.read();
}
