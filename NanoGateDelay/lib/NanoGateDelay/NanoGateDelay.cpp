#include <stdint.h>
#include "NanoGateDelay.h"

uint16_t getByteIndex(uint16_t index) {
    return index >> 3;
}

uint8_t getBitIndex(uint16_t index) {
    return index & 7;
}

bool getAndSetBit(int16_t index, bool value, uint8_t *delayArr) {
    uint16_t byteIndex = getByteIndex(index);
    uint8_t bitIndex = getBitIndex(index);

    uint8_t mask = 1 << bitIndex;
    bool retval = delayArr[byteIndex] & mask;

    uint8_t val = value << bitIndex;
    delayArr[byteIndex] |= val;

    return retval;
}