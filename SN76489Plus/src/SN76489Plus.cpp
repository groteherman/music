#define DODEBUG = 1
#define POLYPHONY 1

#include <Arduino.h>
#include "const.h"
 #include "ProgramConfig.h"
 #include "si5351.h"
//#include "Wire.h"
#include "SN76489.h"
#include "Midi.h"

#define TM_STROBE 15 //A1
#define TM_CLOCK 16  //A2
#define TM_DATA 17   //A3

#ifdef DODEBUG
#include "TM1638lite.h"
#endif

ProgramConfig Program = ProgramConfig(1, 1, 0, 0, 0);
SN76489 mySN76489 = SN76489(PIN_NotWE, PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6, PIN_D7, FREQUENCY);
byte whichSN[3] = {PIN_NotCE0, PIN_NotCE1, PIN_NotCE2};
Si5351 si5351;

#ifdef DODEBUG
TM1638lite tm(TM_STROBE, TM_CLOCK, TM_DATA);
#endif

MIDI_CREATE_DEFAULT_INSTANCE();

byte numberOfNotes = 0;
byte notesPlaying[MAX_POLYPHONY];
byte notesInOrder[MAX_NOTES];
int bend;

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

void noteOn(byte index, byte msg){
//  if (Program.GetPolyphony() > 3){
  if (POLYPHONY > 3){
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
  //if (Program.GetPolyphony() > 3){
  if (POLYPHONY > 3){
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
  //int firstNote = numberOfNotes - Program.GetPolyphony();
  int firstNote = numberOfNotes - POLYPHONY;
  if (firstNote < 0){
    firstNote = 0;
  }
//  for (byte j = 0; j < Program.GetPolyphony(); j++){
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
#ifdef DODEBUG      
      ToTm(j, notesPlaying[j], numberOfNotes);
#endif
    }
  }
  if (numberOfNotes == 0){
    digitalWrite(GATE, false);
  }
  for(byte i = firstNote; i < numberOfNotes; i++){
    bool isPlaying = false;
//    for (byte j = 0; j < Program.GetPolyphony(); j++){
    for (byte j = 0; j < POLYPHONY; j++){
      if (notesPlaying[j] == notesInOrder[i]){
        isPlaying = true;
        break;
      }
    }
    if (!isPlaying){
      //for (byte j = 0; j < Program.GetPolyphony(); j++){
      for (byte j = 0; j < POLYPHONY; j++){
        if (notesPlaying[j] == 0){
          digitalWrite(GATE, false);
          notesPlaying[j] = notesInOrder[i];
          //noteOn(j, notesPlaying[j]);
#ifdef DODEBUG      
          ToTm(j, notesPlaying[j], numberOfNotes);
#endif
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
        //Program.SetDetune0(byte2 - 64);
        //si5351.set_freq(100 * FREQUENCY + DETUNE_FACTOR * Program.GetDetune0(), SI5351_CLK0);
        break;
    case NOVATION_LEVEL :
        //Program.SetDetune1(byte2 - 64);
        //si5351.set_freq(100 * FREQUENCY + DETUNE_FACTOR * Program.GetDetune1(), SI5351_CLK1);
        break;
    case NOVATION_PWM :
        //Program.SetDetune2(byte2 - 64);
        //si5351.set_freq(100 * FREQUENCY + DETUNE_FACTOR * Program.GetDetune2(), SI5351_CLK2);
        break;
    }
}

void handleProgramChange(byte channel, byte program){
  /*
  switch (program) {
    case 0 :
      Program = ProgramConfig(1, 1, 0, 0, 0);
    case 1 :
      Program = ProgramConfig(1, 3, 0, 0, 0);
    case 2 :
      Program = ProgramConfig(1, 9, 0, 0, 0);
  }
  */
  //Program = ProgramConfig(1, 1, 0, 0, 0);
  //handleControlChange(Program.GetMidiChannel(), NOVATION_DETUNE, 64);
  //handleControlChange(Program.GetMidiChannel(), NOVATION_LEVEL, 64);
  //handleControlChange(Program.GetMidiChannel(), NOVATION_PWM, 64);
  handleControlChange(1, NOVATION_DETUNE, 64);
  handleControlChange(1, NOVATION_LEVEL, 64);
  handleControlChange(1, NOVATION_PWM, 64);
  AllOff();
}

void setup()
{
  //Serial.begin(9600);
  //Serial.println("");
  
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

  pinMode(PIN_NotCE0, OUTPUT); 
  pinMode(PIN_NotCE1, OUTPUT); 
  pinMode(PIN_NotCE2, OUTPUT); 
  pinMode(GATE, OUTPUT); 

  //mySN76489.setAttenuation(0, 0x0);
  digitalWrite(PIN_NotCE0, false);
  //mySN76489.setDivider(0, noteDiv[0]);
  delay(100);
  //mySN76489.setDivider(0, noteDiv[11]);
  delay(100);
  //mySN76489.setDivider(0, noteDiv[23]);
  delay(100);
  AllOff();

  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop()
{
  digitalWrite(GATE, LOW);
  for (byte i = MIDI_LOW; i < MIDI_LOW + MIDI_NUMBER; i++){
    noteOn(0,i);
    delay(50);
  }
  digitalWrite(GATE, HIGH);
  for (byte i = MIDI_LOW; i < MIDI_LOW + MIDI_NUMBER; i++){
    noteOn(0,i);
    delay(50);
  }

/*
  MIDI.read();
  if (MIDI.read()) {                    
    byte type = MIDI.getType();
    switch (type) {
      case midi::NoteOn: 
        handleNoteOn(CHANNEL, MIDI.getData1(), MIDI.getData2());
        break;
      case midi::NoteOff: 
        handleNoteOff(CHANNEL, MIDI.getData1(), MIDI.getData2());
        break;
      case midi::ProgramChange:
        handleProgramChange(CHANNEL, MIDI.getData1());
        break;
      case midi::PitchBend:
        bend = (MIDI.getData1() << 7) + MIDI.getData2();
        handlePitchBend(CHANNEL, bend);
        break;
      case midi::ControlChange:
        handleControlChange(CHANNEL, MIDI.getData1(), MIDI.getData2());
        break;
    }
  }*/
}