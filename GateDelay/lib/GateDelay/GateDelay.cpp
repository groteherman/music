#include <stdint.h>
#include "GateDelay.h"

bool getAndSetBit(int16_t index, bool value, uint8_t *delayArr) {
    uint16_t byteIndex = getByteIndex(index);
    uint8_t bitIndex = getBitIndex(index);

    uint8_t *arrVal = &delayArr[byteIndex];

    bool retval = (*arrVal >> bitIndex) & (uint8_t)1;
    *arrVal = (*arrVal & ~((uint8_t)1 << bitIndex)) | ((uint8_t)value << bitIndex);

    return retval;
}