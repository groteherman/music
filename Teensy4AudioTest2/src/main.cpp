/*
 * A simple hardware test which receives audio from the audio shield
 * Line-In pins and send it to the Line-Out pins and headphone jack.
 *
 * This example code is in the public domain.
 */
 #include <Arduino.h>
 #include <MIDI.h>
 #include <Audio.h>
 #include <Wire.h>
 #include <SPI.h>
 #include <SD.h>
 #include <SerialFlash.h>
 #include <Novation.h>

 #define LEDPIN 13
 
 // GUItool: begin automatically generated code
 AudioInputI2S            i2sInput;           //xy=200,69
 AudioOutputI2S           i2sOutput;           //xy=365,94
 AudioEffectReverb         delay1;
 AudioSynthWaveformSine   sine1;          //xy=203,233
 //AudioSynthWaveform       waveform;
 AudioConnection          patchCord1(sine1, 0, i2sOutput, 0);
 AudioConnection          patchCord2(sine1, 0, i2sOutput, 1);
 //AudioConnection          patchCord1(i2sInput, 0, i2sOutput, 0);
 
 //AudioConnection          patchCord2(i2sInput, 0, i2sOutput, 0);
 //AudioConnection          patchCord3(i2sInput, 1, i2sOutput, 1);
// AudioConnection          patchCord1(i2sInput, 1, delay1, 0);
// AudioConnection          patchCord2(delay1, 0, i2sOutput, 0);

 AudioControlSGTL5000     sgtl5000_1;     //xy=302,184
 // GUItool: end automatically generated code
 
 
 const int myInput = AUDIO_INPUT_LINEIN;
 //const int myInput = AUDIO_INPUT_MIC;
 
 MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI);
 
 void setup() {
   // Audio connections require memory to work.  For more
   // detailed information, see the MemoryAndCpuUsage example
   AudioMemory(12);

   // Enable the audio shield, select input, and enable output
   sgtl5000_1.enable();
   sgtl5000_1.inputSelect(myInput);
   sgtl5000_1.volume(0.5);

   sine1.amplitude(1.0);
   sine1.frequency(110);

   //waveform.begin(WAVEFORM_SINE);
   //waveform.amplitude(0.8);
   //waveform.frequency(440);

   MIDI.begin(MIDI_CHANNEL_OMNI);
   pinMode(LEDPIN, OUTPUT);
   digitalWrite(LEDPIN, HIGH);
   delay(100);
   digitalWrite(LEDPIN, LOW);
   delay(100);
   digitalWrite(LEDPIN, HIGH);
   delay(100);
   digitalWrite(LEDPIN, LOW);
   delay(100);
   digitalWrite(LEDPIN, HIGH);
   delay(100);
   digitalWrite(LEDPIN, LOW);
   delay(100);
   digitalWrite(LEDPIN, HIGH);
   delay(100);
   digitalWrite(LEDPIN, LOW);
   delay(100);
  }
 
 elapsedMillis volmsec=0;
 float vol = 0.0;
 float freq = 110;

 float MidiToFrequency(uint8_t midiNote)
 {
     return 8.1757989156 * pow(2.0, midiNote/12.0);
 };
 
 void loop() {
  u_int8_t note, controller, value, velocity;
  if (MIDI.read()) {                    // Is there a MIDI message incoming ?
    if (digitalRead(LEDPIN)) {
      digitalWrite(LEDPIN, LOW);
    } else {
      digitalWrite(LEDPIN, HIGH);
    }
    byte type = MIDI.getType();
    switch (type) {
      case midi::NoteOn:
        note = MIDI.getData1();
        velocity = MIDI.getData2();
        //channel = MIDI.getChannel();
        sine1.frequency(MidiToFrequency(note));
        sine1.amplitude(velocity / 127.0);
        break;
      case midi::NoteOff:
        sine1.amplitude(0.0);
        break;
      case midi::ControlChange:
        controller = MIDI.getData1();
        value = MIDI.getData2();
        switch(controller){
          case NOVATION_FILTER_FREQUENCY:
            sgtl5000_1.volume(value / 127.0);
            break;
          case NOVATION_FILTER_RESONANCE:
            sgtl5000_1.dacVolume(value / 127.0);
            break; 
        }
        break;
    }
  }
 }