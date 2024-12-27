#include <stdint.h>
#include "NanoGateDelay.h"

bool getAndSetBit(int16_t index, bool value, bool *delayArr) {
    bool retval = delayArr[index];
    delayArr[index] = value;
    return retval;
}