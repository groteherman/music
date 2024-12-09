#include <Arduino.h>

#ifndef PROGRAMCONFIG_H_
#define PROGRAMCONFIG_H_

class ProgramConfig {
  private:
    byte MidiChannel;
    byte Polyphony;
    int Detune0;
    int Detune1;
    int Detune2;
    int DetermineDetune(int detune);

  public:
    void SetMidiChannel(byte channel);
    byte GetMidiChannel();
    void SetPolyphony(byte poly);
    byte GetPolyphony();
    void SetDetune0(int detune);
    int GetDetune0();
    void SetDetune1(int detune);
    int GetDetune1();
    void SetDetune2(int detune);
    int GetDetune2();
    ProgramConfig(byte midiChannel, byte polyphony, int detune0, int detune1, int detune2);
};
#endif /* PROGRAMCONFIG_H_ */