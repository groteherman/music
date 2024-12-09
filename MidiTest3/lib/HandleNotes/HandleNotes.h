#ifndef HANDLENOTES_H_
#define HANDLENOTES_H_

//#define FREQUENCY 4286819ULL
#define FREQUENCY 2143409ULL 
#define DETUNE_FACTOR 107170ULL //FREQ / 20
#define PITCH_FACTOR 26167ULL //100 * FREQ / 8191
#define GATE 13 //same as LED_BUILTIN

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

#define MIDI_NUMBER 53
#define MIDI_LOW 48

#define MAX_POLYPHONY 9
#define MAX_NOTES 10
#define CHANNEL 0

struct noot_struct {
  uint8_t numberOfNotes;
  uint8_t polyphony;
  uint8_t notesPlaying[MAX_POLYPHONY];
  uint8_t notesInOrder[MAX_NOTES];
};

uint8_t handleNotesPlayingOff(noot_struct*);
uint8_t handleNotesPlayingOn(noot_struct*);
void handleNoteOn(uint8_t, uint8_t, uint8_t, noot_struct*);
void handleNoteOff(uint8_t, uint8_t, uint8_t, noot_struct*);

#endif /* HANDLENOTES_H_ */
