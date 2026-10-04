#include <Arduino.h>

// Exemple pour ESP32 avec écran 320*240px
// F1ATB - January 2026
// Version V3

#include "FS.h"
#include <SdFat.h>
#include <MD_MIDIFile.h>
#include "Ecran.h"
#include "CST820.h"

#define MIDI_BAUDRATE 31250
#define MIDI_RX_PIN 21
#define MIDI_TX_PIN 22

#define RED_LED 4  //Gek en zoiets staat ook al in ecran.h
#define RED_GREEN 16
#define RED_BLUE 17

#define DAC_PIN 26

#define DO_DEBUG  1

#define MAXFILES 30
#define MAXFILENAMELENGTH 20

#define HOLD_PRESS 500
#define FONT_SIZE 2

#define TEXT_ROW_1 20
#define BUTTON_ROW_1 60
#define TEXT_ROW_2 125
#define BUTTON_ROW_2 160
#define BUTTON_ROW_3 260

#define BUTTON_COLUMN_1 20
#define BUTTON_COLUMN_2 110
#define BUTTON_COLUMN_3 200

#define BUTTON_WIDTH 70
#define BUTTON_HEIGHT 50

#define SD_CS_PIN SS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(16))

#define DEBUG(s, x)  do { Serial.print(F(s)); Serial.print(x); } while(false)
#define DEBUGX(s, x) do { Serial.print(F(s)); Serial.print(F("0x")); Serial.print(x, HEX); } while(false)
#define DEBUGS(s)    do { Serial.print(F(s)); } while (false)

//int rotation = 1; //landscape, usb to the left
int rotation = 2; //portrait, usb at bottom

byte LEDs[] = { 4, 16, 17 };  //Pour les S024 RGB ILI9341. 4,17,16 pour ST7789
LGFX* lcd = nullptr;
CST820* touch = nullptr;

//SD_FAT_TYPE == 3
SdFs sd;
FsFile file;
FsFile root;

MD_MIDIFile SMF;

uint8_t fileCounter = 0;
uint8_t fileIndex = 0;

char fileName[MAXFILENAMELENGTH];
String fileNames[MAXFILES];

bool isPressed = false;
long lastPress;
bool splitChannel10 = false;

LGFX_Button buttonBack, buttonForward, buttonPlay, buttonStop, buttonSplitChannel10;


enum State { S_IDLE, S_STARTING, S_PLAYING, S_ENDING };

class MidiState {
  State state;
  public: 
    void SetState(State inState);
    State GetState();
};

void MidiState::SetState(State inState) {
  Serial.print("State change: ");
  Serial.print(state);
  Serial.print(" => ");
  Serial.println(inState);
 
  state = inState;
}

State MidiState::GetState(){
  return state;
}

MidiState state;

void lcd_init(ScreenType type) {
  touch = new CST820(I2C_SDA, I2C_SCL, TP_RST, TP_INT);
  touch->begin();

  lcd = new LGFX(type);
  lcd->init();
  lcd->setRotation((rotation + 2) % 4);
  lcd->fillScreen(TFT_NAVY);
  lcd->setTextColor(TFT_WHITE, TFT_NAVY);
  lcd->setTextSize(3);

/*
  int W = lcd->textWidth("F1ATB") / 2;
  int X = lcd->width() / 2;
  lcd->setCursor(X - W, lcd->height() / 2);
  lcd->println("F1ATB");
  lcd->drawRect(10, 10, lcd->width() - 20, lcd->height() - 20, TFT_YELLOW);
*/
  lcd->setBrightness(255);
}

void PrintString(String S, int err, int X, int Y, float Sz) {
  lcd->setTextSize(Sz);
  lcd->setCursor(X, Y);
  if (err == 0) {
    lcd->setTextColor(TFT_WHITE, TFT_NAVY);
    lcd->print(S);
  } else {
    lcd->setTextColor(TFT_WHITE, TFT_RED);
    lcd->print(S + ": " + String(err));
  }
}

void drawButtons(){
    buttonBack.drawButton();
    buttonForward.drawButton();
    buttonStop.drawButton();
    buttonPlay.drawButton();
    buttonSplitChannel10.drawButton();
}

void setButtonPressFalse(){
  buttonBack.press(false);
  buttonForward.press(false);
  buttonStop.press(false);
  buttonPlay.press(false);
}

void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{
  if ((pev->data[0] >= 0x80) && (pev->data[0] <= 0xe0))
  {
    if (pev->channel == 10 && splitChannel10) {
      Serial1.write(pev->data[0] | pev->channel);
      Serial1.write(&pev->data[1], pev->size-1);

    } else {
      Serial1.write(pev->data[0] | pev->channel);
      Serial1.write(&pev->data[1], pev->size-1);
    }
  }
#if DO_DEBUG
  Serial.print(".");
/*
  DEBUG("\n", millis());
  DEBUG("\tM T", pev->track);
  DEBUG(":  Ch ", pev->channel+1);
  DEBUGS(" Data");
  for (uint8_t i=0; i<pev->size; i++){
    DEBUGX(" ", pev->data[i]);
  }
*/
#endif
}

void sysexCallback(sysex_event *pev)
// Called by the MIDIFile library when a system Exclusive (sysex) file event needs 
// to be processed through the midi communications interface. Most sysex events cannot 
// really be processed, so we just ignore it here.
// This callback is set up in the setup() function.
{}

void midiSilence(void)
// Turn everything off on every channel.
// Some midi files are badly behaved and leave notes hanging, so between songs turn
// off all the notes and sound
{
  midi_event ev;

  // All sound off
  // When All Sound Off is received all oscillators will turn off, and their volume
  // envelopes are set to zero as soon as possible.
  ev.size = 0;
  ev.data[ev.size++] = 0xb0;
  ev.data[ev.size++] = 120;
  ev.data[ev.size++] = 0;

  for (ev.channel = 0; ev.channel < 16; ev.channel++)
    midiCallback(&ev);
}

void tickMetronome(void)
// flash a LED to the beat
{
  static uint32_t lastBeatTime = 0;
  static boolean  inBeat = false;
  uint16_t  beatTime;

  beatTime = 60000/SMF.getTempo(); // msec/beat = ((60sec/min)*(1000 ms/sec))/(beats/min)
  if (!inBeat) {
    uint32_t currentMillis = millis(); 
    if ((currentMillis - lastBeatTime) >= beatTime) {
      lastBeatTime = currentMillis;
      digitalWrite(RED_LED, LOW);
      inBeat = true;
    }
  } else {
    if ((millis() - lastBeatTime) >= 100)	{ // keep the flash on for 100ms only
      digitalWrite(RED_LED, HIGH);
      inBeat = false;
    }
  }
}

void PrintTitle() {
    PrintString("MrMIDI", 0, BUTTON_COLUMN_1, TEXT_ROW_1, FONT_SIZE);
}

void setup() {
  Serial.begin(115200);
  lcd_init(JC2432W328_C_ST7789_BL27);
  
//  if (!sd.cardBegin(SD_CONFIG)) {
  if (!sd.begin(SD_CONFIG)) {
    Serial.println("Failed to open SD card");
  }

  if (!root.open("/")) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }
  while (file.openNext(&root, O_RDONLY)) {
    if (fileCounter < MAXFILES) {
      file.getName(fileName, MAXFILENAMELENGTH);
      fileNames[fileCounter] = fileName;
      Serial.print(fileCounter);
      Serial.print(": ");
      Serial.println(fileNames[fileCounter]);
      fileCounter++;
    }
  }

  buttonBack.initButtonUL(lcd, BUTTON_COLUMN_1, BUTTON_ROW_1, BUTTON_WIDTH, BUTTON_HEIGHT, TFT_RED, TFT_YELLOW, TFT_BLACK, "<<<", FONT_SIZE, FONT_SIZE);
  buttonForward.initButtonUL(lcd, BUTTON_COLUMN_2, BUTTON_ROW_1, BUTTON_WIDTH, BUTTON_HEIGHT, TFT_RED, TFT_YELLOW, TFT_BLACK, ">>>", FONT_SIZE, FONT_SIZE);
  buttonStop.initButtonUL(lcd, BUTTON_COLUMN_1, BUTTON_ROW_2, BUTTON_WIDTH, BUTTON_HEIGHT, TFT_RED, TFT_YELLOW, TFT_BLACK, "O", FONT_SIZE, FONT_SIZE);
  buttonPlay.initButtonUL(lcd, BUTTON_COLUMN_2, BUTTON_ROW_2, BUTTON_WIDTH, BUTTON_HEIGHT, TFT_RED, TFT_YELLOW, TFT_BLACK, ">", FONT_SIZE, FONT_SIZE);
  buttonSplitChannel10.initButtonUL(lcd, BUTTON_COLUMN_1, BUTTON_ROW_3, BUTTON_WIDTH, BUTTON_HEIGHT, TFT_RED, TFT_YELLOW, TFT_BLACK, "CH10", FONT_SIZE, FONT_SIZE);

  setButtonPressFalse();
  drawButtons();

  for (int i = 0; i < 3; i++) {
    pinMode(LEDs[i], OUTPUT);
    digitalWrite(LEDs[i], HIGH);
  }
  for (int i = 0; i < 3; i++) {
    digitalWrite(LEDs[i], LOW);
    delay(100);
    digitalWrite(LEDs[i], HIGH);
  }

  SMF.begin(&sd);
  SMF.setMidiHandler(midiCallback);
  SMF.setSysexHandler(sysexCallback);

  Serial1.begin(MIDI_BAUDRATE, SERIAL_8N1, MIDI_RX_PIN, MIDI_TX_PIN);

  state.SetState(S_IDLE);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  

  digitalWrite(RED_LED, HIGH);
  digitalWrite(RED_GREEN, HIGH);
  digitalWrite(RED_BLUE, HIGH);

  midiSilence();
  PrintTitle();
}


uint32_t dacTeller = 0;
uint8_t dacMin = 0;
uint8_t dacMax = 255;
uint8_t dacOut = dacMin;

void loop() {
  uint8_t gesture;
  uint16_t touchX, touchY, temp;
  
  //if (millis() - dacTeller > 10){
  //  dacTeller = millis();  
    dacWrite(DAC_PIN, dacOut);
    if (dacOut == dacMin) {
      dacOut == dacMax;
    } else {
      dacOut = dacMin;
    }
  //}
/*
  if (millis() - lastPress > HOLD_PRESS && isPressed){
    isPressed = false;
    setButtonPressFalse();
    drawButtons();
  }

  if (touch->getTouch(&touchX, &touchY, &gesture) && !isPressed) {
    switch (rotation) {
      case 0:
        touchX = 239 - touchX;
        touchY = 319 - touchY;
        break;
      case 1:
        temp = touchX;
        touchX = 319 - touchY;
        touchY = temp;
        break;
      case 3:
        temp = touchX;
        touchX = touchY;
        touchY = 239 - temp;
        break;
    }
    lcd->fillScreen(TFT_NAVY);
  
    if (buttonBack.contains(touchX, touchY)){
      buttonBack.press(true);
      isPressed = true;
      lastPress = millis();
      if (fileIndex > 0) {
        fileIndex--;
      }
      if (state.GetState() == S_PLAYING) {
        state.SetState(S_ENDING);
      }
      buttonPlay.setFillColor(TFT_YELLOW);
    }

    if (buttonForward.contains(touchX, touchY)){
      buttonForward.press(true);
      isPressed = true;
      lastPress = millis();
      if (fileIndex < fileCounter - 1){
        fileIndex++;
      }
      if (state.GetState() == S_PLAYING) {
        state.SetState(S_ENDING);
      }
      buttonPlay.setFillColor(TFT_YELLOW);
    }

    if (buttonPlay.contains(touchX, touchY)){
      buttonPlay.press(true);
      isPressed = true;
      lastPress = millis();
      if (state.GetState() == S_IDLE){
        state.SetState(S_STARTING);
      }
      buttonPlay.setFillColor(TFT_RED);
    }

    if (buttonStop.contains(touchX, touchY)){
      buttonStop.press(true);
      isPressed = true;
      lastPress = millis();
      buttonPlay.setFillColor(TFT_YELLOW);
      if (state.GetState() == S_PLAYING){
        state.SetState(S_ENDING);
      }
    }

    if (buttonSplitChannel10.contains(touchX, touchY)){
      buttonSplitChannel10.press(true);
      isPressed = true;
      lastPress = millis();
      splitChannel10 = !splitChannel10;
      if (splitChannel10) {
        buttonSplitChannel10.setFillColor(TFT_RED);
      } else {
        buttonSplitChannel10.setFillColor(TFT_YELLOW);
      }
    }
    
    PrintTitle();
    PrintString(fileNames[fileIndex], 0, BUTTON_COLUMN_1, TEXT_ROW_2, FONT_SIZE);
    drawButtons();

    Serial.print(fileIndex);
    Serial.println(" touched");

    //lcd->drawFastHLine(0, touchY, 319, TFT_RED);
    //lcd->drawFastVLine(touchX, 0, 239, TFT_RED);

    
    //analogWrite(LED_RED, map(touchX, 10, 309, 255, 0));
    //analogWrite(LED_GREEN, map(touchY, 10, 229, 255, 0));
  }

  switch (state.GetState()) {
    case S_IDLE:
      break;
    case S_STARTING:
      int err;

      err = SMF.load(fileNames[fileIndex].c_str());
      if (err != MD_MIDIFile::E_OK) {
        state.SetState(S_IDLE);
        Serial.print("Error opening ");
        Serial.print(fileNames[fileIndex]);
        Serial.print(" error: ");
        Serial.println(err);
        buttonPlay.setFillColor(TFT_YELLOW);
      } else {
        state.SetState(S_PLAYING);
      }
      PrintString(fileNames[fileIndex], err, BUTTON_COLUMN_1, TEXT_ROW_2, FONT_SIZE);
      break;

    case S_PLAYING:
      if (!SMF.isEOF()) {
        if (SMF.getNextEvent()) {
          tickMetronome();
        }
      } else {
        state.SetState(S_ENDING);
      }
      break;

    case S_ENDING:
      SMF.close();
      midiSilence();
      state.SetState(S_IDLE);
      buttonPlay.setFillColor(TFT_YELLOW);
      drawButtons();
      //PrintFileName(fileNames[fileIndex], 0, BUTTON_COLUMN_1, 80, FONT_SIZE);
      break;
  }
      */
}
