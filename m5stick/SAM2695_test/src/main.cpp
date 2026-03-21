#include <Arduino.h>
/*#include <M5Unified.h>*/
#include <M5StickCPlus2.h>
/*
#include <M5Stack.h>
*/
#include <M5_SAM2695.h>
#include "melody.h"

M5_SAM2695 midi;
M5GFX display;
M5Canvas canvas(&display);

char *instruments[128] = {
    "GrandPiano_1",
    "BrightPiano_2",
    "ElGrdPiano_3",
    "HonkyTonkPiano",
    "ElPiano1",
    "ElPiano2",
    "Harpsichord",
    "Clavi",
    "Celesta",
    "Glockenspiel",
    "MusicBox",
    "Vibraphone",
    "Marimba",
    "Xylophone",
    "TubularBells",
    "Santur",
    "DrawbarOrgan",
    "PercussiveOrgan",
    "RockOrgan",
    "ChurchOrgan",
    "ReedOrgan",
    "AccordionFrench",
    "Harmonica",
    "TangoAccordion",
    "AcGuitarNylon",
    "AcGuitarSteel",
    "AcGuitarJazz",
    "AcGuitarClean",
    "AcGuitarMuted",
    "OverdrivenGuitar",
    "DistortionGuitar",
    "GuitarHarmonics",
    "AcousticBass",
    "FingerBass",
    "PickedBass",
    "FretlessBass",
    "SlapBass1",
    "SlapBass2",
    "SynthBass1",
    "SynthBass2",
    "Violin",
    "Viola",
    "Cello",
    "Contrabass",
    "TremoloStrings",
    "PizzicatoStrings",
    "OrchestralHarp",
    "Timpani",
    "StringEnsemble1",
    "StringEnsemble2",
    "SynthStrings1",
    "SynthStrings2",
    "ChoirAahs",
    "VoiceOohs",
    "SynthVoice",
    "OrchestraHit",
    "Trumpet",
    "Trombone",
    "Tuba",
    "MutedTrumpet",
    "FrenchHorn",
    "BrassSection",
    "SynthBrass1",
    "SynthBrass2",
    "SopranoSax",
    "AltoSax",
    "TenorSax",
    "BaritoneSax",
    "Oboe",
    "EnglishHorn",
    "Bassoon",
    "Clarinet",
    "Piccolo",
    "Flute",
    "Recorder",
    "PanFlute",
    "BlownBottle",
    "Shakuhachi",
    "Whistle",
    "Ocarina",
    "Lead1Square",
    "Lead2Sawtooth",
    "Lead3Calliope",
    "Lead4Chiff",
    "Lead5Charang",
    "Lead6Voice",
    "Lead7Fifths",
    "Lead8BassLead",
    "Pad1Fantasia",
    "Pad2Warm",
    "Pad3PolySynth",
    "Pad4Choir",
    "Pad5Bowed",
    "Pad6Metallic",
    "Pad7Halo",
    "Pad8Sweep",
    "FX1Rain",
    "FX2Soundtrack",
    "FX3Crystal",
    "FX4Atmosphere",
    "FX5Brightness",
    "FX6Goblins",
    "FX7Echoes",
    "FX8SciFi",
    "Sitar",
    "Banjo",
    "Shamisen",
    "Koto",
    "Kalimba",
    "BagPipe",
    "Fiddle",
    "Shanai",
    "TinkleBell",
    "Agogo",
    "SteelDrums",
    "Woodblock",
    "TaikoDrum",
    "MelodicTom",
    "SynthDrum",
    "ReverseCymbal",
    "GtFretNoise",
    "BreathNoise",
    "Seashore",
    "BirdTweet",
    "TelephRing",
    "Helicopter",
    "Applause",
    "Gunshot"
};

// change this to make the song slower or faster
int tempo = 80;
// sizeof gives the number of bytes, each int value is composed of two bytes (16
// bits) there are two values per note (pitch and duration), so for each note
// there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;
int instrument = 0;

void ReadButtons(){
  M5.delay(1);
  M5.update();
  if (M5.BtnA.wasPressed()){
    instrument++;  
    if (instrument>127){
      instrument = 0;
    }
    midi.setInstrument(0, 0, instrument);
    M5.Lcd.clearDisplay();
    M5.Lcd.setCursor(10, 85);
    M5.Lcd.printf(instruments[instrument]);
  }
  if (M5.BtnB.wasPressed()){
    instrument--;  
    if (instrument< 0){
      instrument = 127;
    }
    midi.setInstrument(0, 0, instrument);
    M5.Lcd.clearDisplay();
    M5.Lcd.setCursor(10, 85);
    M5.Lcd.printf(instruments[instrument]);
  }
}

void play() {
    // iterate over the notes of the melody.
    // Remember, the array is twice the number of notes (notes + durations)
    for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
        ReadButtons();
        // calculates the duration of each note
        divider = melody[thisNote + 1];
        if (divider > 0) {
            // regular note, just proceed
            noteDuration = (wholenote) / divider;
        } else if (divider < 0) {
            // dotted notes are represented with negative durations!!
            noteDuration = (wholenote) / abs(divider);
            noteDuration *= 1.5;  // increases the duration in half for dotted notes
        }

        // we only play the note for 90% of the duration, leaving 10% as a pause
        midi.setNoteOn(0, melody[thisNote], 127);  // noteDuration * 0.9);
        // Wait for the specief duration before playing the next note.
        delay(noteDuration);
        // stop the waveform generation before the next note.
        midi.setNoteOff(0, melody[thisNote], 127);
    }
}

void setup() {
    M5.begin();
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.println("FurElise");
    midi.begin(&Serial2, MIDI_BAUD, 33, 32);
    midi.setInstrument(0, 0, 0);
    midi.reset();
    M5.Lcd.setTextColor(WHITE);
    //midi.setAllNotesOff(0);
    //midi.setChorus(0,0,0,0,0);
    //midi.setReverb(0,0,0,0);
}

void loop() {
  play();
  delay(1000);
}

