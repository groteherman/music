#include <Arduino.h>
#include <M5StickCPlus2.h>
#include <Midi.h>

// Use HardwareSerial 2 (GPIO 32/33 or 0/26 depending on M5StickC version)
// For M5StickC Hat port, often G0 and G36 are used.
// We will use Serial2 for MIDI output (GPIO 0 as TX).

MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

byte data[6]; 

void WriteToDisplay(char type, int d1, int d2, int d3) {
    M5.Lcd.clearDisplay();
    M5.Lcd.setCursor(10, 85);
    M5.Lcd.printf("%c:%3d:%3d:%3d:\r\n", type, d1, d2, d3);
}

void setup() {
    M5.begin();
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(1.5);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.println("MIDI MON");
    Serial2.begin(31250, SERIAL_8N1, 33, 32);
    MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
   int note, velocity, channel, d1, d2;
   if (MIDI.read()) {
     byte type = MIDI.getType();
     switch (type) {
       case midi::NoteOn:
         note = MIDI.getData1();
         velocity = MIDI.getData2();
         channel = MIDI.getChannel();
         if (velocity > 0) {
           WriteToDisplay('+', channel, note, velocity);
         } else {
           WriteToDisplay('-', channel, note, 0);
         }
         break;
       case midi::NoteOff:
         note = MIDI.getData1();
         channel = MIDI.getChannel();
         WriteToDisplay('-', channel, note, 0);
         break;
       case midi::ProgramChange:
       default:
         d1 = MIDI.getData1();
         d2 = MIDI.getData2();
         WriteToDisplay('P', type, d1, d2);
     }
   }
}
