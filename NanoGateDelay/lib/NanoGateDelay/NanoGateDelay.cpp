#include <stdint.h>
#include "NanoGateDelay.h"

bool getAndSetBit(int16_t index, bool value, uint8_t *delayArr) {
    uint16_t byteIndex = getByteIndex(index);
    uint8_t bitIndex = getBitIndex(index);

    uint8_t mask = 1 << bitIndex;
    uint8_t val = value << bitIndex;
    uint8_t *arrVal = &delayArr[byteIndex];

    bool retval = *arrVal & mask;
    *arrVal |= val;

    return retval;
}