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
int buttonvalue = 0;
char wavename[10];
// below a few variables we need
boolean ButtonFlag = true;
unsigned long lastPeriodStart; // for repeat function
const int periodDuration = 100; //msec between each readout of userinterface
int songlength=0;


void setup() {
  sgtl5000_1.enable();
  // sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.micGain(40);
  sgtl5000_1.volume(0.9);
  
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
  //waveform1.begin(WAVEFORM_SAWTOOTH);
  //waveform1.amplitude(0.8);
  //waveform1.frequency(200);
  noise1.amplitude(0.5);
  delay1.delay(0, 65);
  mixer1.gain(0, 0.7);                // I2S left input level
  mixer1.gain(1, 0.7 );              // I2S right input level
  mixer2.gain(0, 1);
  mixer2.gain(1, 1);
  mixer2.gain(2, 1);
  mixer2.gain(3, 1);
  mixer3.gain(0, 1);
  mixer3.gain(1, 1);
  mixer3.gain(2, 1);
  mixer3.gain(3, 1);
  mixer4.gain(0, 1);
  mixer4.gain(1, 1);
  mixer4.gain(2, 1);
  mixer4.gain(3, 1);
  mixer5.gain(0, 1);
  mixer5.gain(1, 1);
  mixer5.gain(2, 1);
  mixer5.gain(3, 1);
  mixer6.gain(0, 1);
  mixer6.gain(1, 1);
  mixer6.gain(2, 1);
  mixer6.gain(3, 0); //noise off
  mixer7.gain(1, 0.2);
  mixer7.gain(2, 0.2);
  mixer7.gain(3, 0.0);
  mixer9.gain(0, 0.7);
  mixer9.gain(1, 0.7);
  mixer10.gain(0, 1);

  // filters
  filter1.resonance(res);                                       // set the resonance of the filters
  filter2.resonance(res);
  filter3.resonance(res);
  filter4.resonance(res);
  filter5.resonance(res);
  filter6.resonance(res);
  filter7.resonance(res);
  filter8.resonance(res);
  filter9.resonance(res);
  filter10.resonance(res);
  filter11.resonance(res);
  filter12.resonance(res);
  filter13.resonance(res);
  filter14.resonance(res);
  filter15.resonance(res);
  filter16.resonance(res);
  filter1B.resonance(res);                                       // set the resonance of the filters
  filter2B.resonance(res);
  filter3B.resonance(res);
  filter4B.resonance(res);
  filter5B.resonance(res);
  filter6B.resonance(res);
  filter7B.resonance(res);
  filter8B.resonance(res);
  filter9B.resonance(res);
  filter10B.resonance(res);
  filter11B.resonance(res);
  filter12B.resonance(res);
  filter13B.resonance(res);
  filter14B.resonance(res);
  filter15B.resonance(res);
  filter16B.resonance(res);
  Bfilter1.resonance(res);                                       // set the resonance of the filters
  Bfilter2.resonance(res);
  Bfilter3.resonance(res);
  Bfilter4.resonance(res);
  Bfilter5.resonance(res);
  Bfilter6.resonance(res);
  Bfilter7.resonance(res);
  Bfilter8.resonance(res);
  Bfilter9.resonance(res);
  Bfilter10.resonance(res);
  Bfilter11.resonance(res);
  Bfilter12.resonance(res);
  Bfilter13.resonance(res);
  Bfilter14.resonance(res);
  Bfilter15.resonance(res);
  Bfilter16.resonance(res);
  Cfilter1.resonance(res);                                       // set the resonance of the filters
  Cfilter2.resonance(res);
  Cfilter3.resonance(res);
  Cfilter4.resonance(res);
  Cfilter5.resonance(res);
  Cfilter6.resonance(res);
  Cfilter7.resonance(res);
  Cfilter8.resonance(res);
  Cfilter9.resonance(res);
  Cfilter10.resonance(res);
  Cfilter11.resonance(res);
  Cfilter12.resonance(res);
  Cfilter13.resonance(res);
  Cfilter14.resonance(res);
  Cfilter15.resonance(res);
  Cfilter16.resonance(res);
  Bfilter1.frequency(freq[0]);
  Bfilter2.frequency(freq[1]);
  Bfilter3.frequency(freq[2]);
  Bfilter4.frequency(freq[3]);
  Bfilter5.frequency(freq[4]);
  Bfilter6.frequency(freq[5]);
  Bfilter7.frequency(freq[6]);
  Bfilter8.frequency(freq[7]);
  Bfilter9.frequency(freq[8]);
  Bfilter10.frequency(freq[9]);
  Bfilter11.frequency(freq[10]);
  Bfilter12.frequency(freq[11]);
  Bfilter13.frequency(freq[12]);
  Bfilter14.frequency(freq[13]);
  Bfilter15.frequency(freq[14]);
  Bfilter16.frequency(freq[15]);
  Cfilter1.frequency(freq[0]);
  Cfilter2.frequency(freq[1]);
  Cfilter3.frequency(freq[2]);
  Cfilter4.frequency(freq[3]);
  Cfilter5.frequency(freq[4]);
  Cfilter6.frequency(freq[5]);
  Cfilter7.frequency(freq[6]);
  Cfilter8.frequency(freq[7]);
  Cfilter9.frequency(freq[8]);
  Cfilter10.frequency(freq[9]);
  Cfilter11.frequency(freq[10]);
  Cfilter12.frequency(freq[11]);
  Cfilter13.frequency(freq[12]);
  Cfilter14.frequency(freq[13]);
  Cfilter15.frequency(freq[14]);
  Cfilter16.frequency(freq[15]);
  filter1.frequency(freq[0]);
  filter2.frequency(freq[1]);
  filter3.frequency(freq[2]);
  filter4.frequency(freq[3]);
  filter5.frequency(freq[4]);
  filter6.frequency(freq[5]);
  filter7.frequency(freq[6]);
  filter8.frequency(freq[7]);
  filter9.frequency(freq[8]);
  filter10.frequency(freq[9]);
  filter11.frequency(freq[10]);
  filter12.frequency(freq[11]);
  filter13.frequency(freq[12]);
  filter14.frequency(freq[13]);
  filter15.frequency(freq[14]);
  filter16.frequency(freq[15]);
  filter1B.frequency(freq[0]);
  filter2B.frequency(freq[1]);
  filter3B.frequency(freq[2]);
  filter4B.frequency(freq[3]);
  filter5B.frequency(freq[4]);
  filter6B.frequency(freq[5]);
  filter7B.frequency(freq[6]);
  filter8B.frequency(freq[7]);
  filter9B.frequency(freq[8]);
  filter10B.frequency(freq[9]);
  filter11B.frequency(freq[10]);
  filter12B.frequency(freq[11]);
  filter13B.frequency(freq[12]);
  filter14B.frequency(freq[13]);
  filter15B.frequency(freq[14]);
  filter16B.frequency(freq[15]);

  //biquad
  biquad1.setLowpass(0, 90, 0.53);
  biquad1.setLowpass(1, 90, 0.707);
  biquad1.setLowpass(2, 60, 0.53);
  biquad1.setLowpass(3, 80, 0.707);

  biquad2.setLowpass(0, 200, 0.53);
  biquad2.setLowpass(1, 200, 0.707);
  biquad2.setLowpass(2, 60, 0.53);
  biquad2.setLowpass(3, 160, 0.707);

  biquad3.setLowpass(0, 200, 0.53);
  biquad3.setLowpass(1, 200, 0.707);
  biquad3.setLowpass(2, 60, 0.53);
  biquad3.setLowpass(3, 160, 0.707);

  biquad4.setLowpass(0, 200, 0.53);
  biquad4.setLowpass(1, 200, 0.707);
  biquad4.setLowpass(2, 60, 0.53);
  biquad4.setLowpass(3, 160, 0.707);

  biquad5.setLowpass(0, 200, 0.53);
  biquad5.setLowpass(1, 200, 0.707);
  biquad5.setLowpass(2, 60, 0.53);
  biquad5.setLowpass(3, 160, 0.707);

  biquad6.setLowpass(0, 200, 0.53);
  biquad6.setLowpass(1, 200, 0.707);
  biquad6.setLowpass(2, 60, 0.53);
  biquad6.setLowpass(3, 160, 0.707);

  biquad7.setLowpass(0, 200, 0.53);
  biquad7.setLowpass(1, 200, 0.707);
  biquad7.setLowpass(2, 60, 0.53);
  biquad7.setLowpass(3, 160, 0.707);

  biquad8.setLowpass(0, 200, 0.53);
  biquad8.setLowpass(1, 200, 0.707);
  biquad8.setLowpass(2, 60, 0.53);
  biquad8.setLowpass(3, 160, 0.707);

  biquad9.setLowpass(0, 200, 0.53);
  biquad9.setLowpass(1, 200, 0.707);
  biquad9.setLowpass(2, 60, 0.53);
  biquad9.setLowpass(3, 160, 0.707);

  biquad10.setLowpass(0, 200, 0.53);
  biquad10.setLowpass(1, 200, 0.707);
  biquad10.setLowpass(2, 60, 0.53);
  biquad10.setLowpass(3, 160, 0.707);

  biquad11.setLowpass(0, 200, 0.53);
  biquad11.setLowpass(1, 200, 0.707);
  biquad11.setLowpass(2, 60, 0.53);
  biquad11.setLowpass(3, 160, 0.707);

  biquad12.setLowpass(0, 200, 0.53);
  biquad12.setLowpass(1, 200, 0.707);
  biquad12.setLowpass(2, 60, 0.53);
  biquad12.setLowpass(3, 160, 0.707);

  biquad13.setLowpass(0, 200, 0.53);
  biquad13.setLowpass(1, 200, 0.707);
  biquad13.setLowpass(2, 60, 0.53);
  biquad13.setLowpass(3, 160, 0.707);

  biquad14.setLowpass(0, 200, 0.53);
  biquad14.setLowpass(1, 200, 0.707);
  biquad14.setLowpass(2, 60, 0.53);
  biquad14.setLowpass(3, 160, 0.707);

  biquad15.setLowpass(0, 200, 0.53);
  biquad15.setLowpass(1, 200, 0.707);
  biquad15.setLowpass(2, 60, 0.53);
  biquad15.setLowpass(3, 160, 0.707);


  biquad16.setLowpass(0, 200, 0.53);
  biquad16.setLowpass(1, 200, 0.707);
  biquad16.setLowpass(2, 60, 0.53);
  biquad16.setLowpass(3, 160, 0.707);


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
