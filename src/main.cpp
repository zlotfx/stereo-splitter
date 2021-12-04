/*
 * Stereo signal splitter.
 * Uses Arduino platform.
 *
 */
#include <Arduino.h>
#include <OneButton.h>
#include "EEPROM.h"

#define DEBUG 0
// #define ENABLE_OPTO 1

#if DEBUG
  #define dprintinit(x)     Serial.begin(x)
  #define dprintln(x)       Serial.println(x)
#else 
  #define dprintinit(x)
  #define dprintln(x)
#endif

OneButton * gLeftButton;
OneButton * gRightButton;
OneButton * gStereoButton;

bool gEnableAllLEDsInStereo = false;

const int PIN_RELAY_L = 9;
const int PIN_RELAY_R = 2;

const int PIN_BUTTON_L = 3;
const int PIN_BUTTON_R = 4;
const int PIN_BUTTON_S = 5;

const int PIN_LED_L = 10;
const int PIN_LED_R = 11;
const int PIN_LED_S = 12;

#if ENABLE_OPTO
const int PIN_OPTO_L = 10;
const int PIN_OPTO_R = 11;
const int MUTE_DURATION_MS = 10;
#endif

const int EEPROM_ADDR_LEFT_ENABLE = 0;
const int EEPROM_ADDR_RIGHT_ENABLE = 1;
const int EEPROM_ADDR_UI_MODE = 2;


void fSwitch(int aLeftEnable, int aRightEnable)
{
#if ENABLE_OPTO
  digitalWrite(PIN_OPTO_L, HIGH);
  digitalWrite(PIN_OPTO_R, HIGH);
  delay(MUTE_DURATION_MS);
#endif
  digitalWrite(PIN_RELAY_L, !aLeftEnable);
  digitalWrite(PIN_RELAY_R, !aRightEnable);
#if ENABLE_OPTO
  delay(MUTE_DURATION_MS);
  digitalWrite(PIN_OPTO_L, LOW);
  digitalWrite(PIN_OPTO_R, LOW);
#endif
  EEPROM.write(EEPROM_ADDR_LEFT_ENABLE, aLeftEnable);
  EEPROM.write(EEPROM_ADDR_RIGHT_ENABLE, aRightEnable);

  // UI
  digitalWrite(PIN_LED_L, gEnableAllLEDsInStereo ? aLeftEnable : (aLeftEnable && !aRightEnable));
  digitalWrite(PIN_LED_R, gEnableAllLEDsInStereo ? aRightEnable : (!aLeftEnable && aRightEnable));
  digitalWrite(PIN_LED_S, aLeftEnable && aRightEnable);
}


void setup() {
  // put your setup code here, to run once:
  dprintinit(9600);
  dprintln(F("BEGIN."));

  pinMode(PIN_RELAY_L, OUTPUT);
  pinMode(PIN_RELAY_R, OUTPUT);

  pinMode(PIN_LED_L, OUTPUT);
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_S, OUTPUT);
#if ENABLE_OPTO
  pinMode(PIN_OPTO_L, OUTPUT);
  pinMode(PIN_OPTO_R, OUTPUT);
#endif

  // set UI mode
  pinMode(PIN_BUTTON_S, INPUT_PULLUP);
  if(digitalRead(PIN_BUTTON_S) == LOW) {
    // toggle UI mode
    EEPROM.write(EEPROM_ADDR_UI_MODE, !EEPROM.read(EEPROM_ADDR_UI_MODE));
  }

  gEnableAllLEDsInStereo = EEPROM.read(EEPROM_ADDR_UI_MODE);
  dprintln(F("UI MODE"));
  dprintln(gEnableAllLEDsInStereo ? F("ALL") : F("SINGLE"));

  gLeftButton = new OneButton(PIN_BUTTON_L);
  gRightButton = new OneButton(PIN_BUTTON_R);
  gStereoButton = new OneButton(PIN_BUTTON_S);

  gLeftButton->attachClick([]() { 
    dprintln(F("LEFT"));
    fSwitch(true, false);
  });
  gRightButton->attachClick([]() { 
    dprintln(F("RIGHT"));
    fSwitch(false, true);
  });
  gStereoButton->attachClick([]() { 
    dprintln(F("STEREO"));
    fSwitch(true, true);
  });

  fSwitch(EEPROM.read(EEPROM_ADDR_LEFT_ENABLE), EEPROM.read(EEPROM_ADDR_RIGHT_ENABLE));
}


void loop() {
  gLeftButton->tick();
  gRightButton->tick();
  gStereoButton->tick();
}