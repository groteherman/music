#include <unity.h>
#include "HandleNotes.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_handleNoteOn_1_note(){
    noot_struct nootjes = { 0, 1, {}, {}, 0, 0, 0 };
        
    uint8_t midiNote = 50;
    handleNoteOn(1, midiNote, 128, &nootjes);
    TEST_ASSERT_EQUAL_UINT8(1, nootjes.numberOfNotes);
    TEST_ASSERT_EQUAL_UINT8(midiNote, nootjes.notesInOrder[0]);
    TEST_ASSERT_EQUAL_UINT8(0, nootjes.notesInOrder[1]);
}

void test_handleNoteOn_2_notes(){
    noot_struct nootjes = { 0, 1, {}, {}, 0, 0, 0 };
        
    uint8_t midiNote = 50;
    handleNoteOn(1, midiNote, 128, &nootjes);
    handleNoteOn(1, midiNote + 1, 128, &nootjes);
    TEST_ASSERT_EQUAL_UINT8(2, nootjes.numberOfNotes);
    TEST_ASSERT_EQUAL_UINT8(midiNote, nootjes.notesInOrder[0]);
    TEST_ASSERT_EQUAL_UINT8(midiNote + 1, nootjes.notesInOrder[1]);
    TEST_ASSERT_EQUAL_UINT8(0, nootjes.notesInOrder[2]);
}

void test_handleNoteOn_same_note_2x(){
    noot_struct nootjes = { 0, 1, {}, {}, 0, 0, 0 };
        
    uint8_t midiNote = 50;
    handleNoteOn(1, midiNote, 128, &nootjes);
    handleNoteOn(1, midiNote, 128, &nootjes);
    TEST_ASSERT_EQUAL_UINT8(1, nootjes.numberOfNotes);
    TEST_ASSERT_EQUAL_UINT8(midiNote, nootjes.notesInOrder[0]);
    TEST_ASSERT_EQUAL_UINT8(0, nootjes.notesInOrder[1]);
}

void test_handleNoteOn_2_notes_1off(){
    noot_struct nootjes = { 0, 1, {}, {}, 0, 0, 0 };
        
    uint8_t midiNote = 50;
    handleNoteOn(1, midiNote, 128, &nootjes);
    handleNoteOn(1, midiNote + 1, 128, &nootjes);
    handleNoteOff(1, midiNote, 0, &nootjes);
    TEST_ASSERT_EQUAL_UINT8(1, nootjes.numberOfNotes);
    TEST_ASSERT_EQUAL_UINT8(midiNote + 1, nootjes.notesInOrder[0]);
}

void test_handleNotesPlayingOn_1_note(){
    noot_struct nootjes = { 0, 1, {}, {}, 0, 0, 0 };
        
    uint8_t midiNote = 50;
    handleNoteOn(1, midiNote, 128, &nootjes);
    uint8_t toTurnOff = handleNotesPlayingOff(&nootjes);
    uint8_t sut = handleNotesPlayingOn(&nootjes);
    TEST_ASSERT_EQUAL_UINT8(0, sut);
}

void test_handleNotesPlayingOn_2_notes(){
    noot_struct nootjes = { 0, 1, {}, {}, 0, 0, 0 };
        
    uint8_t midiNote = 50;
    handleNoteOn(1, midiNote, 128, &nootjes);
    handleNotesPlayingOff(&nootjes);
    uint8_t sut = handleNotesPlayingOn(&nootjes);
    TEST_ASSERT_EQUAL_UINT8(0, sut);
    TEST_ASSERT_EQUAL_UINT8(midiNote, nootjes.notesPlaying[0]);

    handleNoteOn(1, midiNote + 1, 128, &nootjes);
    handleNotesPlayingOff(&nootjes);
    sut = handleNotesPlayingOn(&nootjes);
    TEST_ASSERT_EQUAL_UINT8(0, sut);
    TEST_ASSERT_EQUAL_UINT8(midiNote + 1, nootjes.notesPlaying[0]);

    handleNoteOff(1, midiNote + 1, 128, &nootjes);
    handleNotesPlayingOff(&nootjes);
    sut = handleNotesPlayingOn(&nootjes);
    TEST_ASSERT_EQUAL_UINT8(0, sut);
    TEST_ASSERT_EQUAL_UINT8(midiNote, nootjes.notesPlaying[0]);
}

void test_handleNotesPlayingOn_3_notes(){
    noot_struct nootjes = { 0, 3, {}, {}, 0, 0, 0 };
        
    uint8_t midiNote = 50;
    handleNoteOn(1, midiNote, 128, &nootjes);
    handleNotesPlayingOff(&nootjes);
    uint8_t sut = handleNotesPlayingOn(&nootjes);

    handleNoteOn(1, midiNote + 1, 128, &nootjes);
    handleNotesPlayingOff(&nootjes);
    sut = handleNotesPlayingOn(&nootjes);

    handleNoteOn(1, midiNote + 2, 128, &nootjes);
    sut = handleNotesPlayingOff(&nootjes);
    TEST_ASSERT_EQUAL_UINT8(255, sut);
    sut = handleNotesPlayingOn(&nootjes);

    handleNoteOn(1, midiNote + 3, 128, &nootjes);
    sut = handleNotesPlayingOff(&nootjes);
    TEST_ASSERT_EQUAL_UINT8(0, sut);

    sut = handleNotesPlayingOn(&nootjes);

    TEST_ASSERT_EQUAL_UINT8(4, nootjes.numberOfNotes);
    TEST_ASSERT_EQUAL_UINT8(0, sut);
    TEST_ASSERT_EQUAL_UINT8(midiNote + 3, nootjes.notesPlaying[0]);

    handleNoteOff(1, midiNote + 1, 128, &nootjes);
    TEST_ASSERT_EQUAL_UINT8(3, nootjes.numberOfNotes);
    sut = handleNotesPlayingOff(&nootjes);
    TEST_ASSERT_EQUAL_UINT8(1, sut);

    sut = handleNotesPlayingOn(&nootjes);

    TEST_ASSERT_EQUAL_UINT8(1, sut);

    TEST_ASSERT_EQUAL_UINT8(midiNote, nootjes.notesInOrder[0]);
    TEST_ASSERT_EQUAL_UINT8(midiNote + 2, nootjes.notesInOrder[1]);
    TEST_ASSERT_EQUAL_UINT8(midiNote + 3, nootjes.notesInOrder[2]);

    TEST_ASSERT_EQUAL_UINT8(midiNote + 3, nootjes.notesPlaying[0]);
    TEST_ASSERT_EQUAL_UINT8(midiNote, nootjes.notesPlaying[1]);
    TEST_ASSERT_EQUAL_UINT8(midiNote + 2, nootjes.notesPlaying[2]);
}

void test_determinePitchBend(){
    TEST_ASSERT_EQUAL_INT32(0, determinePitchBend(0, 64));
    TEST_ASSERT_EQUAL_INT32(PITCH_FACTOR, determinePitchBend(1, 64));
    TEST_ASSERT_EQUAL_INT32(-PITCH_FACTOR/2, determinePitchBend(127, 63));
}

void test_determineFrequency_no_detune_no_pitchbend(){
    noot_struct nootjes = { 0, 1, {}, {}, 0, 0, 0 };
    int32_t pitchBend = determinePitchBend(0, 64);
    long frequency = determineFrequency(pitchBend, nootjes.detune0);
    TEST_ASSERT_EQUAL_INT32(100 * FREQUENCY, frequency);
}

void test_determineFrequency_detune_no_pitchbend(){
    noot_struct nootjes = { 0, 1, {}, {}, 1, 0, 0 };
    int32_t pitchBend = determinePitchBend(0, 64);
    long frequency = determineFrequency(pitchBend, nootjes.detune0);
    TEST_ASSERT_EQUAL_INT32(100 * FREQUENCY + DETUNE_FACTOR, frequency);
}

void test_determineFrequency_no_detune_pitchbend(){
    noot_struct nootjes = { 0, 1, {}, {}, 0, 0, 0 };
    int32_t pitchBend = determinePitchBend(1, 64);
    long frequency = determineFrequency(pitchBend, nootjes.detune0);
    TEST_ASSERT_EQUAL_INT32(100 * FREQUENCY + pitchBend, frequency);
}

void test_determineFrequency_detune_pitchbend(){
    noot_struct nootjes = { 0, 1, {}, {}, 1, 0, 0 };
    int32_t pitchBend = determinePitchBend(1, 64);
    long frequency = determineFrequency(pitchBend, nootjes.detune0);
    TEST_ASSERT_EQUAL_INT32(100 * FREQUENCY + DETUNE_FACTOR * nootjes.detune0 + pitchBend, frequency);
}

int main( int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_handleNoteOn_1_note);
    RUN_TEST(test_handleNoteOn_2_notes);
    RUN_TEST(test_handleNoteOn_same_note_2x);
    RUN_TEST(test_handleNoteOn_2_notes_1off);

    RUN_TEST(test_handleNotesPlayingOn_1_note);
    RUN_TEST(test_handleNotesPlayingOn_2_notes);
    RUN_TEST(test_handleNotesPlayingOn_3_notes);

    RUN_TEST(test_determinePitchBend);
    RUN_TEST(test_determineFrequency_no_detune_no_pitchbend);
    RUN_TEST(test_determineFrequency_detune_no_pitchbend);
    RUN_TEST(test_determineFrequency_no_detune_pitchbend);
    RUN_TEST(test_determineFrequency_detune_pitchbend);

    UNITY_END();
}