#ifndef MIDIKNOB_H_
#define MIDIKNOB_H_

class MidiKnob {
  public:
    static bool InNoMansLand(uint16_t potValue);
    static uint8_t MidiFromPot(uint16_t potValue, uint8_t midiValue);
};

#endif /*MIDIKNOB_H_*/
