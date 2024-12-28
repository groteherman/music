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

void test_getAndSetBit_init(){
    uint8_t delayArr[2];
    uint16_t maxDelay = 2 * 8;
    
    for (int16_t i = 0; i < maxDelay; i++){
        bool sut = getAndSetBit(i, 1, delayArr);
        TEST_ASSERT_FALSE(sut);
    }

    for (int16_t i = 0; i < maxDelay; i++){
        bool sut = getAndSetBit(i, 1, delayArr);
        TEST_ASSERT_TRUE(sut);
    }
}

void test_getAndSetBit_value(){
    uint8_t delayArr[2];
    uint16_t maxDelay = 2 * 8;
   
    delayArr[0] = 0b11110000;
    delayArr[1] = 0b10101010;

    bool val = 0;
    
    for (int16_t i = 0; i < maxDelay; i++){
        val = getAndSetBit(i, val, delayArr);
    }

    TEST_ASSERT(val = 1);
    TEST_ASSERT(delayArr[0] = 0b11100000);
    TEST_ASSERT(delayArr[1] = 0b01010101);
}

int main( int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_byteIndex);
    RUN_TEST(test_bitIndex);
    RUN_TEST(test_getAndSetBit_init);
    RUN_TEST(test_getAndSetBit_value);

    UNITY_END();
}