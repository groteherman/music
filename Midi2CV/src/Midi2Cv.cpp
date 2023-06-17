/* 
 *    MIDI2CV 
 *    Copyright (C) 2017  Larry McGovern
 *  
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License <http://www.gnu.org/licenses/> for more details.

  For Arduino Nano
  In IDE: Processor: AT Mega 328p (old bootloader)
  Steve Woodward, 2010
  most code borrowed from
  http://mrbook.org/blog/2008/11/22/controlling-a-gakken-sx-150-synth-with-arduino/
  adc: mcp4922
  
  +5v           > 4922 pin 1
  Ard pin 4     > 4922 pin 3   (SS - slave select)
  Ard pin 19    > 4922 pin 4   (SCK - clock)
  Ard pin 17    > 4922 pin 5   (MOSI - data out)
  Ground        > 4922 pin 8   (LDAC)
  +5v           > 4922 pin 11  (voltage ref DAC B)
  Ground        > 4922 pin 12
  +5v           > 4922 pin 13  (voltage ref DAC A)
  4922 pin 14 DAC A > 1k resistor > synth CV in
  4922 pin 10 DAC B > 1k resistor > 2nd CV

  Yamaha SHS-10: lowest note 53, highest note 84
                 0V              2 7/12V = 2.5833333 V
                 DA: 0           DA: 2.58333/ V(~5V) * 4095 = 2115.75; step = 2115.75 / 32 =~ 66
                 toDA = (midiNote - 53) * 66
*/
 
#include <MIDI.h>
#include <SPI.h>

#define GATE  2
#define TRIG  3
#define CLOCK 6
#define DAC1  4 

#define NOTE_SF 47.069f // This value can be tuned if CV output isn't exactly 1V/octave

MIDI_CREATE_DEFAULT_INSTANCE();

bool notes[88] = {0}; 
int8_t noteOrder[20] = {0}, orderIndx = {0};
unsigned long trigTimer = 0;

void setVoltage(int dacpin, bool channel, bool gain, unsigned int mV)
{
  // setVoltage -- Set DAC voltage output
  // dacpin: chip select pin for DAC.  Note and velocity on DAC1, pitch bend and CC on DAC2
  // channel: 0 (A) or 1 (B).  Note and pitch bend on 0, velocity and CC on 2.
  // gain: 0 = 1X, 1 = 2X.  
  // mV: integer 0 to 4095.  If gain is 1X, mV is in units of half mV (i.e., 0 to 2048 mV).
  // If gain is 2X, mV is in units of mV
  unsigned int command = channel ? 0x9000 : 0x1000;

  command |= gain ? 0x0000 : 0x2000;
  command |= (mV & 0x0FFF);
  
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(dacpin,LOW);
  SPI.transfer(command>>8);
  SPI.transfer(command&0xFF);
  digitalWrite(dacpin,HIGH);
  SPI.endTransaction();
}

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

void commandNote(int noteMsg) {
  digitalWrite(GATE,HIGH);
  digitalWrite(TRIG,HIGH);
  trigTimer = millis();
  
  //unsigned int mV = (unsigned int) ((float) noteMsg * NOTE_SF + 0.5); 
  unsigned int mV = (unsigned int) (noteMsg - 21) * 35; //was 69 zonder opamp
  setVoltage(DAC1, 0, 0, mV);  // DAC1, channel 0, gain = 1X
}

void commandLastNote()
{

  int8_t noteIndx;
  
  for (int i=0; i<20; i++) {
    noteIndx = noteOrder[ mod(orderIndx-i, 20) ];
    if (notes[noteIndx]) {
      commandNote(noteIndx);
      return;
    }
  }
  digitalWrite(GATE,LOW);  // All notes are off
}

// Rescale 88 notes to 4096 mV:
//    noteMsg = 0 -> 0 mV 
//    noteMsg = 87 -> 4096 mV
// DAC output will be (4095/87) = 47.069 mV per note, and 564.9655 mV per octive
// Note that DAC output will need to be amplified by 1.77X for the standard 1V/octave 



void setup()
{
 pinMode(GATE, OUTPUT);
 pinMode(TRIG, OUTPUT);
 pinMode(CLOCK, OUTPUT);
 pinMode(DAC1, OUTPUT);
 digitalWrite(GATE,LOW);
 digitalWrite(TRIG,LOW);
 digitalWrite(CLOCK,LOW);
 digitalWrite(DAC1,HIGH);

 SPI.begin();

 MIDI.begin(1);
 setVoltage(DAC1, 0, 0, 0);
}

void loopx()
{
  digitalWrite(GATE,LOW);
  delay(200);
  digitalWrite(GATE,HIGH);
  delay(200);
}

void loop()
{
  int type, noteMsg, velocity, channel, d1, d2;
  static unsigned long clock_timer=0, clock_timeout=0;
  static unsigned int clock_count=0;

  if ((trigTimer > 0) && (millis() - trigTimer > 20)) { 
    digitalWrite(TRIG,LOW); // Set trigger low after 20 msec 
    trigTimer = 0;  
  }

  if ((clock_timer > 0) && (millis() - clock_timer > 20)) { 
    digitalWrite(CLOCK,LOW); // Set clock pulse low after 20 msec 
    clock_timer = 0;  
  }
  
  if (MIDI.read()) {                    
    byte type = MIDI.getType();
    switch (type) {
      case midi::NoteOn: 
      case midi::NoteOff:
        noteMsg = MIDI.getData1();// - 21; // A0 = 21, Top Note = 108
        channel = MIDI.getChannel();
        
        if ((noteMsg < 0) || (noteMsg > 87)) break; // Only 88 notes of keyboard are supported

        if (type == midi::NoteOn) velocity = MIDI.getData2();
        else velocity  = 0;  

        if (velocity == 0)  {
          notes[noteMsg] = false;
        }
        else {
          notes[noteMsg] = true;
          // velocity range from 0 to 4095 mV  Left shift d2 by 5 to scale from 0 to 4095, 
          // and choose gain = 2X
          setVoltage(DAC1, 1, 1, velocity<<5);  // DAC1, channel 1, gain = 2X
        }

        if (notes[noteMsg]) {  // If note is on and using last note priority, add to ordered list
          orderIndx = (orderIndx+1) % 20;
          noteOrder[orderIndx] = noteMsg;                 
        }
        commandLastNote();         
        break;
        
      case midi::Clock:
        if (millis() > clock_timeout + 300) clock_count = 0; // Prevents Clock from starting in between quarter notes after clock is restarted!
        clock_timeout = millis();
        
        if (clock_count == 0) {
          digitalWrite(CLOCK,HIGH); // Start clock pulse
          clock_timer=millis();    
        }
        clock_count++;
        if (clock_count == 24) {  // MIDI timing clock sends 24 pulses per quarter note.  Sent pulse only once every 24 pulses
          clock_count = 0;  
        }
        break;
        
      case midi::ActiveSensing: 
        break;
        
      default:
        d1 = MIDI.getData1();
        d2 = MIDI.getData2();
    }
  }
}

