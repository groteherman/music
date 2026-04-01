#include <Arduino.h>
#include <M5StickCPlus2.h>
#include <M5_SAM2695.h>
#include <MidiInstruments.h>

M5_SAM2695 midi;
M5GFX display;
int instrument = 0;

void ReadButtons(){
  //M5.delay(1);
  M5.update();
  if (M5.BtnA.wasPressed()){
    instrument++;  
    if (instrument>127){
      instrument = 0;
    }
    midi.setInstrument(0, 0, instrument);
    M5.Lcd.clearDisplay();
    M5.Lcd.setCursor(10, 85);
    M5.Lcd.printf(MidiInstruments[instrument]);
  }
  if (M5.BtnB.wasPressed()){
    instrument--;  
    if (instrument< 0){
      instrument = 127;
    }
    midi.setInstrument(0, 0, instrument);
    M5.Lcd.clearDisplay();
    M5.Lcd.setCursor(10, 85);
    M5.Lcd.printf(MidiInstruments[instrument]);
  }
}

void setup() {
    M5.begin();
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.println("MIDI MIRROR");
    midi.begin(&Serial2, MIDI_BAUD, 33, 32);
    midi.reset();
    midi.setInstrument(0, 0, 0);
    M5.Lcd.setTextColor(WHITE);
}

void loop() {
  if (Serial2.available()) {
    char data = Serial2.read();
    Serial2.write(data);
  }
  ReadButtons();
}

