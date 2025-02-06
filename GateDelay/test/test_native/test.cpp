#include <unity.h>
#include "GateDelay.h"

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

void test_getAndSetBit_empty(){
    uint8_t delayArr[2];
    uint16_t maxDelay = 2 * 8;
   
    delayArr[0] = 0b00000000;
    delayArr[1] = 0b00000000;

    bool val = 0;
    
    for (int16_t i = 0; i < maxDelay; i++){
        val = getAndSetBit(i, val, delayArr);
    }

    TEST_ASSERT(val == 0);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, delayArr[0]);
    TEST_ASSERT_EQUAL_UINT8(0b00000000, delayArr[1]);
}

void test_getAndSetBit_set_to_false(){
    uint8_t delayArr[1];
    uint16_t maxDelay = 8;
   
    delayArr[0] = 0b11110000;

    bool val2 = getAndSetBit(maxDelay - 1, 0, delayArr);

    TEST_ASSERT(val2 == 1);
    TEST_ASSERT_EQUAL_UINT8(0b01110000, delayArr[0]);
}

void test_getAndSetBit_set_to_true(){
    uint8_t delayArr[1];
    uint16_t maxDelay = 8;
   
    delayArr[0] = 0b01110000;

    bool val2 = getAndSetBit(maxDelay - 1, 1, delayArr);

    TEST_ASSERT(val2 == 0);
    TEST_ASSERT_EQUAL_UINT8(0b11110000, delayArr[0]);
}

void test_getAndSetBit_shift_1_position(){
    uint8_t delayArr[2];
    uint16_t maxDelay = 2 * 8;
   
    delayArr[0] = 0b11110000;
    delayArr[1] = 0b10101010;

    bool val = 0;
    
    for (int16_t i = 0; i < maxDelay; i++){
        val = getAndSetBit(i, val, delayArr);
    }

    TEST_ASSERT(val == 1);
    TEST_ASSERT_EQUAL_UINT8(0b11100000, delayArr[0]);
    TEST_ASSERT_EQUAL_UINT8(0b01010101, delayArr[1]);
}

void test_getAndSet_SetBits(){
    uint8_t delayArr[2];
    uint16_t maxDelay = 2 * 8;
   
    uint8_t char0 = 0b01001001;
    uint8_t char1 = 0b10110110;

    bool val;
    val = getAndSetBit(0, 1, delayArr);
    val = getAndSetBit(1, 0, delayArr);
    val = getAndSetBit(2, 0, delayArr);
    val = getAndSetBit(3, 1, delayArr);
    val = getAndSetBit(4, 0, delayArr);
    val = getAndSetBit(5, 0, delayArr);
    val = getAndSetBit(6, 1, delayArr);
    val = getAndSetBit(7, 0, delayArr);
    val = getAndSetBit(8, 0, delayArr);
    val = getAndSetBit(9, 1, delayArr);
    val = getAndSetBit(10, 1, delayArr);
    val = getAndSetBit(11, 0, delayArr);
    val = getAndSetBit(12, 1, delayArr);
    val = getAndSetBit(13, 1, delayArr);
    val = getAndSetBit(14, 0, delayArr);
    val = getAndSetBit(15, 1, delayArr);
    
    TEST_ASSERT_EQUAL_UINT8(char0, delayArr[0]);
    TEST_ASSERT_EQUAL_UINT8(char1, delayArr[1]);
}

void test_getAndSet_GetBits(){
    uint8_t delayArr[2];
    uint16_t maxDelay = 2 * 8;
   
    delayArr[0] = 0b01001001; 
    delayArr[1] = 0b10110110;

    bool val;
    val = getAndSetBit(0, 1, delayArr);
    TEST_ASSERT(val == 1);
    val = getAndSetBit(1, 0, delayArr);
    TEST_ASSERT(val == 0);
    val = getAndSetBit(2, 0, delayArr);
    TEST_ASSERT(val == 0);
    val = getAndSetBit(3, 1, delayArr);
    TEST_ASSERT(val == 1);
    val = getAndSetBit(4, 0, delayArr);
    TEST_ASSERT(val == 0);
    val = getAndSetBit(5, 0, delayArr);
    TEST_ASSERT(val == 0);
    val = getAndSetBit(6, 1, delayArr);
    TEST_ASSERT(val == 1);
    val = getAndSetBit(7, 0, delayArr);
    TEST_ASSERT(val == 0);
    val = getAndSetBit(8, 0, delayArr);
    TEST_ASSERT(val == 0);
    val = getAndSetBit(9, 1, delayArr);
    TEST_ASSERT(val == 1);
    val = getAndSetBit(10, 1, delayArr);
    TEST_ASSERT(val == 1);
    val = getAndSetBit(11, 0, delayArr);
    TEST_ASSERT(val == 0);
    val = getAndSetBit(12, 1, delayArr);
    TEST_ASSERT(val == 1);
    val = getAndSetBit(13, 1, delayArr);
    TEST_ASSERT(val == 1);
    val = getAndSetBit(14, 0, delayArr);
    TEST_ASSERT(val == 0);
    val = getAndSetBit(15, 1, delayArr);
}

int main( int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_byteIndex);
    RUN_TEST(test_bitIndex);
    RUN_TEST(test_getAndSetBit_init);
    RUN_TEST(test_getAndSetBit_empty);
    RUN_TEST(test_getAndSetBit_set_to_false);
    RUN_TEST(test_getAndSetBit_set_to_true);
    RUN_TEST(test_getAndSetBit_shift_1_position);
    RUN_TEST(test_getAndSet_SetBits);
    RUN_TEST(test_getAndSet_GetBits);

    UNITY_END();
}