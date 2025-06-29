#include <unity.h>
#include "MidiKnob.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_InNoMansLand(void) {
    TEST_ASSERT_FALSE(InNoMansLand(0));
    TEST_ASSERT_FALSE(InNoMansLand(3));
    TEST_ASSERT_TRUE(InNoMansLand(4));
    TEST_ASSERT_TRUE(InNoMansLand(7));
    TEST_ASSERT_FALSE(InNoMansLand(8));
    TEST_ASSERT_FALSE(InNoMansLand(9));
}

void test_MidiFromPot(void) {
    TEST_ASSERT_EQUAL(0, MidiFromPot(0, 0));
    TEST_ASSERT_EQUAL(0, MidiFromPot(0, 14));
    TEST_ASSERT_EQUAL(0, MidiFromPot(7, 0));
    TEST_ASSERT_EQUAL(0, MidiFromPot(7, 28));
    TEST_ASSERT_EQUAL(1, MidiFromPot(8, 0));
    TEST_ASSERT_EQUAL(1, MidiFromPot(7, 1));
    TEST_ASSERT_EQUAL(127, MidiFromPot(1023, 0));
}

int main( int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_InNoMansLand);
    RUN_TEST(test_MidiFromPot);

    UNITY_END();
}