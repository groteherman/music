#include <unity.h>
#include "NanoGateDelay.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_getAndSetBit(){
    bool delayArr[2];
    delayArr[0] = 0;
    delayArr[1] = 0;
    uint16_t maxDelay = 2;
    
    for (int16_t i = 0; i < maxDelay; i++){
        bool sut = getAndSetBit(i, i & 1, delayArr);
        TEST_ASSERT_FALSE(sut);
    }

    for (int16_t i = 0; i < maxDelay; i++){
        bool sut = getAndSetBit(i, i & 1, delayArr);
        TEST_ASSERT(sut == (i & 1));
    }
}

int main( int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_getAndSetBit);

    UNITY_END();
}