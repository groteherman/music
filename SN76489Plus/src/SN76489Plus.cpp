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
#include "TM1638lite.h"

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
#define GATE 13

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

#define TM_STROBE 15
#define TM_CLOCK 16
#define TM_DATA 17

//impliciet bij een NANO
//Si5351_SDA 18; A4
//Si5351_SCL 19; A5
//MIDI_IN RX 0

//pins not used: 1 (TX) A6 and A7 (analogue in only)

SN76489 mySN76489 = SN76489(PIN_NotWE, PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6, PIN_D7, FREQUENCY);
byte whichSN[3] = {PIN_NotCE0, PIN_NotCE1, PIN_NotCE2};

Si5351 si5351;
TM1638lite tm(TM_STROBE, TM_CLOCK, TM_DATA);
bool pressed1 = false, pressed2 = false, pressed4 = false, pressed8 = false;
byte polyValue[3] = {1, 3, 9};

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

byte detuneToggle = 1;

#define MAX_POLYPHONY 9
#define MAX_NOTES 10
#define KEY_PRESS_DELAY 300

volatile byte numberOfNotes = 0;
volatile byte notesPlaying[MAX_POLYPHONY];
volatile byte notesInOrder[MAX_NOTES];
char buffer[9];

struct Config {
  byte midiChannel;
  byte polyIndex;
  int deTune0;
  int deTune1;
  int deTune2;
};

struct Config myConfig;

void displayNoteOn(int input1, int input2, int msg){
  //sprintf(buffer, "* %d %d %d", input1, input2, msg);
  //tm.displayText(buffer);
  tm.setLED(input1, true);
}

void displayNoteOff(int input1, int input2){
  //sprintf(buffer, "- %d %d", input1, input2);
  //tm.displayText(buffer);
  tm.setLED(input1, false);
}

void noteOn(byte index, byte msg, byte polyphony){
  if (polyphony > 3){
    digitalWrite(whichSN[index % 3], false);
    mySN76489.setDivider(index / 3, noteDiv[msg - MIDI_LOW]);
    mySN76489.setAttenuation(index / 3, 0x0);
    digitalWrite(whichSN[index % 3], true);
    displayNoteOn(index, index /3, msg);
  } else {
    digitalWrite(whichSN[0], false);
    digitalWrite(whichSN[1], false);
    digitalWrite(whichSN[2], false);
    mySN76489.setDivider(index % 3, noteDiv[msg - MIDI_LOW]);
    mySN76489.setAttenuation(index % 3, 0x0);
    digitalWrite(whichSN[0], true);
    digitalWrite(whichSN[1], true);
    digitalWrite(whichSN[2], true);
    displayNoteOn(index % 3, index /3, msg);
  }
}

void noteOff(byte index, byte polyphony){
  if (polyphony > 3){
    digitalWrite(whichSN[index % 3], false);
    mySN76489.setAttenuation(index / 3, 0xf);
    digitalWrite(whichSN[index % 3], true);
    displayNoteOff(index, index / 3);
} else {
    digitalWrite(whichSN[0], false);
    digitalWrite(whichSN[1], false);
    digitalWrite(whichSN[2], false);
    mySN76489.setAttenuation(index % 3, 0xf);
    digitalWrite(whichSN[0], true);
    digitalWrite(whichSN[1], true);
    digitalWrite(whichSN[2], true);
    displayNoteOff(index % 3, index / 3);
  }
}

void AllOff(){
  strcpy(buffer, "ALL OFF");
  tm.displayText(buffer);
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
}

void handleNotesPlaying(){
  int firstNote = numberOfNotes - polyValue[myConfig.polyIndex];
  if (firstNote < 0){
    firstNote = 0;
  }
  for (byte j = 0; j < polyValue[myConfig.polyIndex]; j++){
    bool turnOff = true;
    for(byte i = firstNote; i < numberOfNotes; i++){
      if (notesPlaying[j] == notesInOrder[i] && notesPlaying[j] != 0){
        turnOff = false;
        break;
      }
    }
    if (turnOff && notesPlaying[j] != 0){
      notesPlaying[j] = 0;
      noteOff(j, polyValue[myConfig.polyIndex]);
    }
  }
  for(byte i = firstNote; i < numberOfNotes; i++){
    bool isPlaying = false;
    for (byte j = 0; j < polyValue[myConfig.polyIndex]; j++){
      if (notesPlaying[j] == notesInOrder[i]){
        isPlaying = true;
        break;
      }
    }
    if (!isPlaying){
      for (byte j = 0; j < polyValue[myConfig.polyIndex]; j++){
        if (notesPlaying[j] == 0){
          notesPlaying[j] = notesInOrder[i];
          noteOn(j, notesPlaying[j], polyValue[myConfig.polyIndex]);
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
        if (numberOfNotes == 1){
          digitalWrite(GATE, true);
        }
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
    if (numberOfNotes == 0){
      digitalWrite(GATE, false);
    }
    handleNotesPlaying();
  }
}

void handlePitchBend(byte channel, int bend){
  int normalizedBend = bend - 64;
  if (normalizedBend > 0){
    si5351.set_freq(100 * FREQUENCY + normalizedBend * 100 * FREQUENCY / 64, SI5351_CLK0);
  } else {
    si5351.set_freq(100 * FREQUENCY + normalizedBend * 1000000, SI5351_CLK0);
  }
}

void readButtons(){
  uint8_t buttons = tm.readButtons();
  if (buttons == 0) {
    pressed1 = false;
    pressed2 = false;
    pressed4 = false;
    pressed8 = false;
  } else {
    //sprintf(buffer, "BUT %d", buttons);
    //tm.displayText(buffer);
    switch (buttons) {
    case 1 :
      if (!pressed1){
        pressed1 = true;
        myConfig.polyIndex++;
        if (myConfig.polyIndex > 2){
          myConfig.polyIndex = 0;
        }
        sprintf(buffer, "POLY %d", polyValue[myConfig.polyIndex]);
        tm.displayText(buffer);
      }
      break;
    case 2 :
      if (!pressed2){
        pressed2 = true;
        if (detuneToggle == 1) {
          detuneToggle = 2;
        } else {
          detuneToggle = 1;
        }
        sprintf(buffer, "DETUNE %d", detuneToggle);
        tm.displayText(buffer);
      }
      break;
    case 4 :
      if (!pressed4){
        delay(KEY_PRESS_DELAY);
        pressed4 = true;
      }
      if (detuneToggle == 1){
        myConfig.deTune1--;
        si5351.set_freq(100 * FREQUENCY + 100 * myConfig.deTune1, SI5351_CLK1);
      } else{
        myConfig.deTune2--;
        si5351.set_freq(100 * FREQUENCY + 100 * myConfig.deTune2, SI5351_CLK2);
      }
      sprintf(buffer, "%d %d", myConfig.deTune1, myConfig.deTune2);
      tm.displayText(buffer);
      break;
    case 8 :
      if (!pressed8){
        delay(KEY_PRESS_DELAY);
        pressed8 = true;
      }
      if (detuneToggle == 1){
        myConfig.deTune1++;
        si5351.set_freq(100 * FREQUENCY + 100 * myConfig.deTune1, SI5351_CLK1);
      } else{
        myConfig.deTune2++;
        si5351.set_freq(100 * FREQUENCY + 100 * myConfig.deTune2, SI5351_CLK2);
      }
      sprintf(buffer, "%d %d", myConfig.deTune1, myConfig.deTune2);
      tm.displayText(buffer);
      break;
    case 12: //buttons 4 and 8 at the same time
      myConfig.deTune1 = 0;
      myConfig.deTune2 = 0;
      si5351.set_freq(100 * FREQUENCY, SI5351_CLK1);
      si5351.set_freq(100 * FREQUENCY, SI5351_CLK2);
      strcpy(buffer, "DET RST");
      tm.displayText(buffer);
      delay(300);
      break;
    case 64 :
      tm.sendCommand(ACTIVATE);
      break;
    case 128 :
      AllOff();
      tm.sendCommand(DISPLAY_OFF);
      break;
    }
  }
}

void setup()
{
  bool i2c_found;
  //Serial.begin(115200);
  //Serial.println("");
  i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  if(!i2c_found)
  {
    //Serial.println("Device not found on I2C bus!");
  }

  si5351.set_freq(100 * FREQUENCY, SI5351_CLK0);
  si5351.set_freq(101 * FREQUENCY, SI5351_CLK1);
  si5351.set_freq(102 * FREQUENCY, SI5351_CLK2);

  pinMode(PIN_NotCE0, OUTPUT); 
  pinMode(PIN_NotCE1, OUTPUT); 
  pinMode(PIN_NotCE2, OUTPUT); 

  //AllOff();
  pinMode(GATE, OUTPUT); 
  digitalWrite(GATE, false);

  tm.setLED(1, true);
  digitalWrite(PIN_NotCE0, false);
  mySN76489.setDivider(0, noteDiv[0]);
  delay(100);

  tm.setLED(2, true);
  mySN76489.setDivider(0, noteDiv[11]);
  delay(100);
  tm.setLED(3, true);
  mySN76489.setDivider(0, noteDiv[23]);
  delay(100);

  tm.setLED(4, true);
  mySN76489.setAttenuation(0, 0xF);

  MIDI.begin(MIDI_CHANNEL_OMNI);
  for (byte i = 0; i < 8; i++ ){
    tm.setLED(i, false);
  }
  AllOff();
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandlePitchBend(handlePitchBend);

  myConfig.midiChannel = 1;
  myConfig.deTune0 = 0;
  myConfig.deTune1 = 0;
  myConfig.deTune2 = 0;
  myConfig.polyIndex = 1; //poly=3
}

void loop()
{
  MIDI.read();
  readButtons();
}
