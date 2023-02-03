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
#define DETUNE_FACTOR 500000
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
byte polyValue[3] = {1, 3, 9};
byte polyphony;

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
#define KEY_LONG_DELAY 500
#define KEY_SHORT_DELAY 10

volatile byte numberOfNotes = 0;
volatile byte notesPlaying[MAX_POLYPHONY];
volatile byte notesInOrder[MAX_NOTES];
char buffer[9];
uint8_t previousButtons = 0;

#define MENUS 5
byte menuIndex = 0;
const char menu_0[] = "CH";
const char menu_1[] = "PO";
const char menu_2[] = "D0";
const char menu_3[] = "D1";
const char menu_4[] = "D2";
const char *const menuTable[] = {menu_0, menu_1, menu_2, menu_3, menu_4};
int config[MENUS] =     { 0, 1,    0,    0, 0};
const int configMin[] = { 0, 0, -100, -100, -100};
const int configMax[] = {15, 2,  100,  100,  100};

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

void noteOn(byte index, byte msg){
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

void noteOff(byte index){
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
  int firstNote = numberOfNotes - polyphony;
  if (firstNote < 0){
    firstNote = 0;
  }
  for (byte j = 0; j < polyphony; j++){
    bool turnOff = true;
    for(byte i = firstNote; i < numberOfNotes; i++){
      if (notesPlaying[j] == notesInOrder[i] && notesPlaying[j] != 0){
        turnOff = false;
        break;
      }
    }
    if (turnOff && notesPlaying[j] != 0){
      notesPlaying[j] = 0;
      noteOff(j);
    }
  }
  for(byte i = firstNote; i < numberOfNotes; i++){
    bool isPlaying = false;
    for (byte j = 0; j < polyphony; j++){
      if (notesPlaying[j] == notesInOrder[i]){
        isPlaying = true;
        break;
      }
    }
    if (!isPlaying){
      for (byte j = 0; j < polyphony; j++){
        if (notesPlaying[j] == 0){
          notesPlaying[j] = notesInOrder[i];
          noteOn(j, notesPlaying[j]);
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
  //TODO for all SN...; respect detune
  int normalizedBend = bend - 64;
  if (normalizedBend > 0){
    si5351.set_freq(100 * FREQUENCY + normalizedBend * 10 * FREQUENCY / 64, SI5351_CLK0);
  } else {
    si5351.set_freq(100 * FREQUENCY + normalizedBend * 100000, SI5351_CLK0);
  }
}

void readButtons(){
  uint8_t buttons = tm.readButtons();
  if (buttons > 0) {
    //sprintf(buffer, "BUT %d", buttons);
    //tm.displayText(buffer);
    switch (buttons) {
    case 1 :
      if (previousButtons != 1){
        if (menuIndex > 0){
          menuIndex--;
        } else {
          menuIndex = MENUS -1;
        }
      }
      break;
    case 2 :
      if (previousButtons != 2){
        if (menuIndex < MENUS - 1){
          menuIndex++;
        } else {
          menuIndex = 0;
        }
      }
      break;
    case 4 :
      if (config[menuIndex] > configMin[menuIndex]){
        config[menuIndex]--;
      }
      break;
    case 8 :
      if (config[menuIndex] < configMax[menuIndex]){
        config[menuIndex]++;
      }
      break;
    case 64 :
      tm.sendCommand(ACTIVATE);
      break;
    case 128 :
      AllOff();
      tm.sendCommand(DISPLAY_OFF);
      break;
    }
    if (buttons < 16){
        sprintf(buffer, "%s %d", menuTable[menuIndex], config[menuIndex]);
        tm.displayText(buffer);
    }
    if (buttons > 2 && buttons < 16){
      switch (menuIndex) {
        case 0 : //midi channel
          break;
        case 1 : //poly
          polyphony = polyValue[config[1]];
          break;
        case 2 : //detune0
          si5351.set_freq(100 * FREQUENCY + DETUNE_FACTOR * config[2], SI5351_CLK0);
          break;
        case 3 : //detune1
          si5351.set_freq(100 * FREQUENCY + DETUNE_FACTOR * config[3], SI5351_CLK1);
          break;
        case 4 : //detune2
          si5351.set_freq(100 * FREQUENCY + DETUNE_FACTOR * config[4], SI5351_CLK2);
          break;
      }
      if (previousButtons != buttons) {
        delay(KEY_LONG_DELAY);
      } else {
        delay(KEY_SHORT_DELAY);
      }
    }
  }
  previousButtons = buttons;
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

  polyphony = polyValue[config[1]];
}

void loop()
{
  MIDI.read();
  readButtons();
}
