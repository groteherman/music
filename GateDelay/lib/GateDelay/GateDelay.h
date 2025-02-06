#ifndef GATEDELAY_H_
#define GATEDELAY_H_

#define LED_PIN 13
#define GATEIN_PIN 3
#define GATEOUT_PIN 10
#define SWITCH_PIN 21
#define MAXARRAY 1024

#define getByteIndex(t) ((t) >> 3)
#define getBitIndex(t) ((t) & 7)

bool getAndSetBit(int16_t, bool, uint8_t*);

#endif /*GATEDELAY_H_*/
