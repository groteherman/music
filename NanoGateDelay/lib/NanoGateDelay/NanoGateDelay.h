#ifndef NANOGATEDELAY_H_
#define NANOGATEDELAY_H_

#define LED_PIN 13
#define GATEIN_PIN 3
#define GATEOUT_PIN 4
#define SWITCH_PIN 5
#define MAXARRAY 1024

#define getByteIndex(t) ((t) >> 3)
#define getBitIndex(t) ((t) & 7)

bool getAndSetBit(int16_t, bool, uint8_t*);

#endif /*NANOGATEDELAY_H_*/
