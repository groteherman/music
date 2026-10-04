#include <Arduino.h>
#include <DacESP32.h>

#define FREQ_MIN 500           // Hz
#define FREQ_MAX  5000
#define FREQ_STEP  250
#define CHANGE_DELAY_MS 200  

DacESP32 dac1(DAC_CHAN0_GPIO_NUM);

int freq = FREQ_MIN;
float dac_voltage = 0.0;



// Define frequencies for the C4 Major Scale
float notes[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};


void setup() {
  // Initialize DAC on Channel 1 (GPIO 25)
    dac1.outputCW(notes[0]);
}

void loop() {
  for (int i = 0; i < 8; i++) {
    // The library will automatically calculate the closest hardware registers
    dac1.outputCW(notes[i]);
    delay(500); // Play each note for half a second
  }
  dac1.disable();
  delay(100);
}

/*
void setup() {
  Serial.begin(115200);
  dac1.outputCW(freq);
}

void loop() {

  for (int i = 0; i < 255; i++){
    dacWrite(DAC_CH1, i);
  }
    dacWrite(DAC_CH1, 0);
    dacWrite(DAC_CH1, 255);
for (int deg = 0; deg < 360; deg++) {
    // Calculate sine and write to DAC
    dacWrite(DAC_CH1, int(128 + 64 * sin(deg * PI / 180)));
  }

  if (dac1.setCwFrequency(freq) != ESP_OK) {
    Serial.printf("Error: setCwFrequency(%d)\n", freq);
  }
  delay(CHANGE_DELAY_MS);
  freq += FREQ_STEP;
  if (freq > FREQ_MAX){
    freq = FREQ_MIN;
  }
//  dac1.outputVoltage(dac_voltage);
//  dac_voltage += 0.1;
//  if (dac_voltage > CHANNEL_VOLTAGE_MAX){
//    dac_voltage = 0.0;
//  }

}
*/