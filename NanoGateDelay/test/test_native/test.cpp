#include <unity.h>
#include "NanoGateDelay.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_byteIndex(){
    TEST_ASSERT_EQUAL_INT16(getByteIndex(0), 0); 
    TEST_ASSERT_EQUAL_INT16(getByteIndex(1), 0); 
    TEST_ASSERT_EQUAL_INT16(getByteIndex(7), 0); 
    TEST_ASSERT_EQUAL_INT16(getByteIndex(8), 1); 
}

void test_bitIndex(){
    TEST_ASSERT_EQUAL_INT16(getBitIndex(0), 0); 
    TEST_ASSERT_EQUAL_INT16(getBitIndex(1), 1); 
    TEST_ASSERT_EQUAL_INT16(getBitIndex(7), 7); 
    TEST_ASSERT_EQUAL_INT16(getBitIndex(8), 0); 
}

void test_getAndSetBit(){
    uint8_t delayArr[2];
    delayArr[0] = 0;
    delayArr[1] = 0;
    uint16_t maxDelay = 2 * 8;
    
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

    RUN_TEST(test_byteIndex);
    RUN_TEST(test_bitIndex);
    RUN_TEST(test_getAndSetBit);

    UNITY_END();
}