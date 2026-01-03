/*
 * Teensy Vocoder
 * Adapted from Cylon Voice
 * Coded my Mark Donners, aka The Electronic Engineer 
 *
 * code revised by Herman de Groot, should be working with midi instead of real knobs and buttons
 * 
 * rest of the comments are obsolete
 * 
 * This sketch contains three files: CylonVocoder2.ino Soundfile.h and Vocoder.h
 * All files have to be in the same folder for this to work
 * The name of that folder has to be CylonVocoder2
 * 
 * This code was tested on a Teensy 4.0 with Audio Shield using the following libraries
 * Audio at version 1.3 Part of the Teensy Framework
 * SPI at version 1.0 Part of the Teensy Framework 
 * SD at version 2.0.0 Part of the Teensy Framework
 * SdFat at version 2.1.2 Part of the Teensy Framework 
 * SerialFlash at version 0.5 Part of the Teensy Framework
 * Wire at version 1.0 Part of the Teensy Framework 
 * Using library EasyButton at version 2.0.1 You need to install this using the library manager
 * 
 * Code stucture:
 * Includes, variable and definitions
 * Setup function-> initialize audio chip, 
 *                  check for SD card, 
 *                  Define filter centre frequencies,
 *                  Initialize all audio components to specific values
 *                  initialize button function
 *            
 * Main function--> Read Button and handle button flag if set,
 *                  Read Peak value of input signal is ready for readout
 *                   Mute internal tone generator if no input signal present
 *                  Read and handle user interface every few mSec, as defined in periodDuration
 *                  Play mp3 file when player is available
 *                
 * playFile funtion--> handle the actual playback of the mp3 file
 * 
 */

#include <Arduino.h>
#include "Vocoder.h"

char buffer1[22];
char buffer2[22];
char buffer3[22];
char buffer4[22];

#define squelch    0.2    // treshold to mute internal tone generator when no input 0.4

#define VolOutPot    22// 14      // volume Potmeter Connection
#define ModSoundPot   16   // Potmeter for vocoded sound output wet
#define PitchPot      15      // Pitch Potmeter Connection for internal tone generator
#define FeedTroughPot 17  // Potmeter for audio troughput  dry
#define mp3Pot        14        // Potmeter for volume internal mp3 player
#define BUTTON_PIN 9      // Pin for button to select mode of internal tone generator

#define NUMBER_OF_MIXERS 10
#define NUMBER_OF_FILTERS 16

int buttonvalue = 0;
char wavename[10];
// below a few variables we need
boolean ButtonFlag = true;
unsigned long lastPeriodStart; // for repeat function
const int periodDuration = 100; //msec between each readout of userinterface
int songlength=0;

AudioMixer4 mixers[NUMBER_OF_MIXERS] = { mixer1, mixer2, mixer3, mixer4, mixer5,  mixer6, mixer7, mixer9, mixer10, };
AudioFilterStateVariable filters[NUMBER_OF_FILTERS] = {filter1, filter2, filter3, filter4, filter5, filter6, filter7, filter8, filter9, filter10, filter11, filter12, filter13, filter14, filter15, filter16};
AudioFilterStateVariable Bfilters[NUMBER_OF_FILTERS] = {Bfilter1, Bfilter2, Bfilter3, Bfilter4, Bfilter5, Bfilter6, Bfilter7, Bfilter8, Bfilter9, Bfilter10, Bfilter11, Bfilter12, Bfilter13, Bfilter14, Bfilter15, Bfilter16};
AudioFilterStateVariable filtersB[NUMBER_OF_FILTERS] = {filter1B, filter2B, filter3B, filter4B, filter5B, filter6B, filter7B, filter8B, filter9B, filter10B, filter11B, filter12B, filter13B, filter14B, filter15B, filter16B};
AudioFilterStateVariable Cfilters[NUMBER_OF_FILTERS] = {Cfilter1, Cfilter2, Cfilter3, Cfilter4, Cfilter5, Cfilter6, Cfilter7, Cfilter8, Cfilter9, Cfilter10, Cfilter11, Cfilter12, Cfilter13, Cfilter14, Cfilter15, Cfilter16};
AudioFilterBiquad biquads[NUMBER_OF_FILTERS] = {biquad1, biquad2, biquad3, biquad4, biquad5, biquad6, biquad7, biquad8, biquad9, biquad10, biquad11, biquad12, biquad13, biquad14, biquad15, biquad16};

void setup() {
  sgtl5000_1.enable();
  // sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.muteHeadphone();
  sgtl5000_1.lineInLevel(12);
  sgtl5000_1.lineOutLevel(25);
  sgtl5000_1.micGain(0);
  sgtl5000_1.adcHighPassFilterEnable();
  
// below are the centre frequencies of each filter
  const float res = 5;              // q
  const float freq[16] = {
    100,
    165,
    218,
    288,
    380,
    500,
    660,
    870,
    1150,
    1515,
    2000,
    2640,
    3480,
    4600,
    6060,
    8000
  };

  AudioMemory(128);                  // allocate some memory for audio library

  Serial.begin(115200);             // initialize serial communication

  noise1.amplitude(0.5);
  delay1.delay(0, 65);

  for (unsigned int i = 0; i < NUMBER_OF_MIXERS; i++) {
    mixers[i].gain(0, 1);
    mixers[i].gain(1, 1);
    mixers[i].gain(2, 1);
    mixers[i].gain(3, 1);
  }

  //overrule
  mixer1.gain(0, 0.7);                // I2S left input level
  mixer1.gain(1, 0.7 );              // I2S right input level
  mixer5.gain(3, 0.2); //noise
  mixer7.gain(0, 1); //wet signal
  mixer7.gain(1, 0); //dry signal
  mixer7.gain(2, 0); //delay, turned off
  mixer9.gain(0, 0.7);
  mixer9.gain(1, 0.7);

  for (unsigned int i = 0; i < NUMBER_OF_FILTERS; i++) {
    filters[i].resonance(res); 
    Bfilters[i].resonance(res); 
    filtersB[i].resonance(res); 
    Cfilters[i].resonance(res);

    filters[i].frequency(freq[i]); 
    Bfilters[i].frequency(freq[i]); 
    filtersB[i].frequency(freq[i]); 
    Cfilters[i].frequency(freq[i]);

    biquads[i].setLowpass(0, 200, 0.53);
    biquads[i].setLowpass(1, 200, 0.707);
    biquads[i].setLowpass(2, 60, 0.53);
    biquads[i].setLowpass(3, 160, 0.707);
  }
  //overrule
  biquad1.setLowpass(0, 90, 0.53);
  biquad1.setLowpass(1, 90, 0.707);
  biquad1.setLowpass(3, 80, 0.707);
}

void loop() {
  /*
  Serial.print(AudioProcessorUsage());
  Serial.print("/");
  Serial.print(AudioProcessorUsageMax());
  Serial.println("");
  AudioProcessorUsageMaxReset();
  delay(200);
  */
  /*
  if (millis() - lastPeriodStart >= periodDuration) {
    lastPeriodStart += periodDuration;
    float Dry_VOL = map(analogRead(FeedTroughPot), 0, 1024, 0,1024);
    Dry_VOL /= 50;
    mixer7.gain(1, Dry_VOL);

    float Wet_VOL = map(analogRead(ModSoundPot), 0, 1024, 0, 100);
    Wet_VOL /= 100;
    mixer7.gain(0, Wet_VOL);

    float Vol = map(analogRead(VolOutPot), 0, 1024, 0, 100);
    Vol /= 100;
    sgtl5000_1.volume(Vol);
    float VolLine = map(analogRead(VolOutPot), 0, 1024, 31, 13);
    sgtl5000_1.lineOutLevel(VolLine,VolLine);

    float Volmp3 = map(analogRead(mp3Pot), 0, 1024, 1, 1024);
    float VolSet = 0;
    
    if (Volmp3 < 100)VolSet = 0;
    else if (Volmp3 < 120); //do nothing
    else if (Volmp3 < 200)VolSet = 0.05;
    else if (Volmp3 < 220); //do nothing
    else if (Volmp3 < 400)VolSet = 0.1;
    else if (Volmp3 < 420); //do nothing
    else if (Volmp3 < 600)VolSet = 0.15;
    else if (Volmp3 < 620); //do nothing
    else if (Volmp3 < 800)VolSet = 0.2;
    else if (Volmp3 < 220); //do nothing
    else if (Volmp3 > 850)VolSet = 0.3;
    mixer9.gain(1, VolSet);
   //Serial.printf("Vol: %f pitch: %d  mp3: %f Volvoc %f  volwet  %f\n",Vol,Pitch,VolSet,Wet_VOL,Dry_VOL);
  
   sprintf(buffer1,"%-7s Freq: %3d ",wavename,Pitch);
   sprintf(buffer2,"Wet: %3.1f Dry: %3.1f ",Dry_VOL,Wet_VOL);
   sprintf(buffer3,"Mp3:: %3.1f Vol:%3.1f",VolSet,Vol); 
  }
   */
} //loop
