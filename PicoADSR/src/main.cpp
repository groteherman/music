#include <Arduino.h>
#include <SPI.h>

#define DAC_CS 17

void setVoltage(int dacpin, bool channel, bool gain, unsigned int mV)
{
  // setVoltage -- Set DAC voltage output
  // dacpin: chip select pin for DAC.  Note and velocity on DAC1, pitch bend and CC on DAC2
  // channel: 0 (A) or 1 (B).  Note and pitch bend on 0, velocity and CC on 2.
  // gain: 0 = 1X, 1 = 2X.  
  // mV: integer 0 to 4095.  If gain is 1X, mV is in units of half mV (i.e., 0 to 2048 mV).
  // If gain is 2X, mV is in units of mV
  unsigned int command = channel ? 0x9000 : 0x1000;

  command |= gain ? 0x0000 : 0x2000;
  command |= (mV & 0x0FFF);
  
  SPI.beginTransaction(SPISettings(20000000L, MSBFIRST, SPI_MODE0));
  digitalWrite(dacpin,LOW);
  SPI.transfer(command>>8);
  SPI.transfer(command&0xFF);
  digitalWrite(dacpin,HIGH);
  SPI.endTransaction();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  SPI.begin();
  setVoltage(DAC_CS, 0, 0, 0);
  setVoltage(DAC_CS, 1, 0, 0);
}
   
void loop() {
/*  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);    
  delay(500);*/
  for (int i = 0; i < 4096; i++){
    setVoltage(DAC_CS, 0, 0, i);
    //int y = analogRead(PIN_A0);
    setVoltage(DAC_CS, 1, 0, 4095 - i);
  }
}