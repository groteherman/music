/* Testing new synth module Rectifier
 *  https://forum.pjrc.com/threads/61384?p=243547&viewfull=1#post243547
 *
 *  Copyright 2020 Bradley Sanders. This is free software under GPL2.
 *  Based on Revalogics vocoder and incorporating new Rectifier module
 *  to generate dc control voltages
 *
 *  Teensy 4.0 or higher is required to run this example.  It uses
 *  about 31% of the CPU time on Teensy 4.0 when running at 600 MHz.
 */
#include <Audio.h>
#include "defs.h"

#define NUM_STAT_VAR_FILTERS 78
#define NUM_BIQUAD_FILTERS 19

const int myInput = AUDIO_INPUT_LINEIN;

AudioFilterStateVariable statVarfilters[NUM_STAT_VAR_FILTERS] = {
  filter1,filter2,filter3,filter4,filter5,filter6,filter7,filter8,filter9,filter10,
  filter11,filter12,filter13,filter14,filter15,filter16,filter17,filter18,filter19,filter20,
  filter21,filter22,filter23,filter24,filter25,filter26,filter27,filter28,filter29,filter30,
  filter31,filter32,filter33,filter34,filter35,filter36,filter37,filter38,filter39,filter40,
  filter41,filter42,filter43,filter44,filter45,filter46,filter47,filter48,filter49,filter50,
  filter51,filter52,filter53,filter54,filter55,filter56,filter57,filter58,filter59,filter60,
  filter61,filter62,filter63,filter64,filter65,filter66,filter67,filter68,filter69,filter70,
  filter71,filter72,filter73,filter74,filter75,filter76,filter77,filter78
};

AudioFilterBiquad biquadFilters[NUM_BIQUAD_FILTERS] {
  biquad3, biquad4, biquad5, biquad6, biquad7, biquad8, biquad9, biquad10, biquad11, biquad12, 
  biquad13, biquad14, biquad15, biquad16, biquad17, biquad18, biquad19, biquad20, biquad21
};

void setup() {
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.7);

  const float res = 5;              // this is used as resonance value of all state variable filters
                                    // modulators
  const float freq[37] = {          // filter frequency table, tuned to specified musical notes
    110.0000000,  // A2   freq[0]
    123.4708253,  // B2   freq[1]
    138.5913155,  // C#3  freq[2]
    155.5634919,  // D#3  freq[3]
    174.6141157,  // F3   freq[4]
    195.9977180,  // G3   freq[5]
    220.0000000,  // A3   freq[6]
    246.9416506,  // B3   freq[7]
    277.1826310,  // C#4  freq[8]
    311.1269837,  // D#4  freq[9]
    349.2282314,  // F4   freq[10]
    391.9954360,  // G4   freq[11]
    440.0000000,  // A4   freq[12]
    493.8833013,  // B4   freq[13]
    554.3652620,  // C#5  freq[14]
    622.2539674,  // D#5  freq[15]
    698.4564629,  // F5   freq[16]
    783.9908720,  // G5   freq[17]
    880.0000000,  // A5   freq[18]
    987.7666025,  // B5   freq[19]
    1108.730524,  // C#6  freq[20]
    1244.507935,  // D#6  freq[21]
    1396.912926,  // F6   freq[22]
    1567.981744,  // G6   freq[23]
    1760.000000,  // A6   freq[24]
    1975.533205,  // B6   freq[25]
    2217.461048,  // C#7  freq[26]
    2489.015870,  // D#7  freq[27]
    2793.825851,  // F7   freq[28]
    3135.963488,  // G7   freq[29]
    3520.000000,  // A7   freq[30]
    3951.066410,  // B7   freq[31]
    4434.922096,  // C#8  freq[32]
    4978.031740,  // D#8  freq[33]
    5587.651703,  // F8   freq[34]
    6271.926976,  // G8   freq[35]
    7040.000000   // A8   freq[36]
  };

  AudioMemory(64);                  // allocate some memory for audio library
  Serial.begin(115200);             // initialize serial communication
  noise1.amplitude(0.7);            // controls sibilance (hiss syllables)
  mixer1.gain(0, 1);                // I2S left input level
  mixer2.gain(0, 0.7);              // I2S right input level
  mixer2.gain(1, 0.7);              // noise right input level
  mixer11.gain(0, 0.7);             // vocoder output 1 level (low-mid freq)
  mixer11.gain(1, 0.7);             // vocoder output 2 level (high freq)
  mixer11.gain(2, 0);               // instrument to output mix level
  mixer3.gain(0,1);
  mixer3.gain(1,1);
  mixer3.gain(2,1);
  mixer3.gain(3,1);
  mixer4.gain(0,1);
  mixer4.gain(1,1);
  mixer4.gain(2,1);
  mixer5.gain(0,1);
  mixer5.gain(1,1);
  mixer5.gain(2,1);
  mixer5.gain(3,1);
  mixer6.gain(0,1);
  mixer6.gain(1,1);
  mixer6.gain(2,1);
  mixer7.gain(0,1);
  mixer7.gain(1,1);
  mixer7.gain(2,1);
  mixer7.gain(3,1);
  mixer8.gain(0,1);
  mixer8.gain(1,1);
  mixer9.gain(0,1);
  mixer9.gain(1,1);
  mixer9.gain(2,1);
  mixer9.gain(3,1);
  mixer10.gain(0,1);
  mixer10.gain(1,1);

  for(int i = 0; i < NUM_STAT_VAR_FILTERS; i++){
    statVarfilters[i].resonance(res);
  }
  for(int i = 0; i < NUM_STAT_VAR_FILTERS - 2; i++){
    statVarfilters[i].frequency(freq[i/2]);
  }
  filter77.frequency(freq[36]);     // last pair of filters are used for sibilance
  filter78.frequency(freq[36]);     // input is a white noise, instead of instrument/synth

  for(int i = 0 ; i < NUM_BIQUAD_FILTERS; i++) {
    biquadFilters[i].setLowpass(0, 90, 0.53);
    biquadFilters[i].setLowpass(1, 90, 0.707);
    biquadFilters[i].setLowpass(2, 60, 0.53);
    biquadFilters[i].setLowpass(3, 80, 0.707);
  }
}

void loop() {
  Serial.print(AudioProcessorUsage());
  Serial.print("/");
  Serial.print(AudioProcessorUsageMax());
  Serial.println("");
  AudioProcessorUsageMaxReset();
  delay(200);
}