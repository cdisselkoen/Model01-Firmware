// -*- mode: c++ -*-
// Copyright 2016 Keyboardio, inc. <jesse@keyboard.io>
// See "LICENSE" for license details

#define DEBUG_SERIAL false

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "locally built"
#endif


#include "Kaleidoscope.h"
#include "Kaleidoscope-MouseKeys.h"
#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-MacrosOnTheFly.h"
#include "Kaleidoscope-LEDControl.h"
#include "Kaleidoscope-Numlock.h"
#include "Kaleidoscope-OneShot.h"
#include "Kaleidoscope-Escape-OneShot.h"

#include "LED-Off.h"
#include "Kaleidoscope-LEDEffect-BootGreeting.h"
#include "Kaleidoscope-LEDEffect-SolidColor.h"
#include "Kaleidoscope-LEDEffect-Breathe.h"
#include "Kaleidoscope-LEDEffect-Chase.h"
#include "Kaleidoscope-LEDEffect-Rainbow.h"
#include "Kaleidoscope-LED-Stalker.h"
#ifndef ARDUINO_VIRTUAL
#include "Kaleidoscope-LED-LetterGuesser.h"
#endif
#include "Kaleidoscope-LED-ActiveModColor.h"

#define MACRO_VERSION_INFO 1
#define MACRO_ANY 2
#define MACRO_LEDTOGGLE 3
#define MACRO_RAPIDFIRECLICK 4

enum { QWERTY, FUNCTION, NUMPAD };  // layers

const Key keymaps[][ROWS][COLS] PROGMEM = {
  [QWERTY] = KEYMAP_STACKED
  (___,          Key_1, Key_2, Key_3, Key_4, Key_5, Key_LEDEffectNext,
   Key_Backtick, Key_Q, Key_W, Key_E, Key_R, Key_T, Key_MacroRec,
   Key_Tab,      Key_A, Key_S, Key_D, Key_F, Key_G,
   Key_Home,     Key_Z, Key_X, Key_C, Key_V, Key_B, Key_End,
   Key_Backspace, OSM(LeftShift), OSM(LeftControl), OSL(2),
   Key_Keymap1_Momentary,

   M(MACRO_ANY),       Key_6, Key_7, Key_8,     Key_9,      Key_0,         Key_KeypadNumLock,
   Key_MacroPlay,      Key_Y, Key_U, Key_I,     Key_O,      Key_P,         Key_Equals,
                       Key_H, Key_J, Key_K,     Key_L,      Key_Semicolon, Key_Quote,
   M(MACRO_LEDTOGGLE), Key_N, Key_M, Key_Comma, Key_Period, Key_Slash,     Key_Minus,
   OSM(LeftAlt), OSM(LeftGui), Key_Spacebar, Key_Delete,
   Key_Keymap1_Momentary),

  [FUNCTION] = KEYMAP_STACKED
  (___,      Key_F1,           Key_F2,      Key_F3,        Key_F4,        Key_F5,           XXX,
   Key_Tab,  Key_mouseBtnR,    Key_mouseUp, Key_mouseBtnL, Key_mouseWarpNW, Key_mouseWarpNE, Key_PageDown,
   ___,      Key_mouseL,       Key_mouseDn, Key_mouseR,    Key_mouseWarpSW, Key_mouseWarpSE,
   M(MACRO_RAPIDFIRECLICK), Key_PrintScreen, Key_Insert, Key_mouseBtnM, Key_mouseWarpEnd, ___, Key_PageUp,
   Key_Delete, OSM(LeftShift), OSM(LeftControl), ___,
                                                              ___,

   Consumer_ScanPreviousTrack, Key_F6,                   Key_F7,                   Key_F8,                Key_F9,           Key_F10,        Key_F11,
   Consumer_PlaySlashPause,    Consumer_ScanNextTrack,   Key_LeftCurlyBracket,     Key_RightCurlyBracket, Key_LeftParen,    Key_RightParen, Key_F12,
                               Key_LeftArrow,            Key_DownArrow,            Key_UpArrow,           Key_RightArrow,   Key_Pipe,       ___,
   Consumer_Mute,              Consumer_VolumeIncrement, Consumer_VolumeDecrement, Key_LeftBracket,       Key_RightBracket, Key_Backslash,  ___,
   OSM(LeftAlt), OSM(LeftGui), Key_Enter, Key_Escape,
   ___),

  [NUMPAD] = KEYMAP_STACKED
  (___, ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___,
   ___,

   M(MACRO_VERSION_INFO), ___, Key_Keypad7, Key_Keypad8, Key_Keypad9,   Key_KeypadSubtract, ___,
   ___,                   ___, Key_Keypad4, Key_Keypad5, Key_Keypad6,   Key_KeypadAdd,      ___,
                          ___, Key_Keypad1, Key_Keypad2, Key_Keypad3,   Key_KeypadMultiply, Key_Quote,
   ___,                   ___, Key_Keypad0, Key_Comma,   Key_KeypadDot, Key_KeypadDivide,   Key_Enter,
   ___, ___, ___, ___,
   ___)
};

static kaleidoscope::LEDSolidColor solidYellow(130, 100, 0);

const macro_t *macroAction(uint8_t macroIndex, uint8_t keyState) {
  if (macroIndex == MACRO_VERSION_INFO && keyToggledOn(keyState)) {
    Macros.type(PSTR("Keyboardio Model 01 - Kaleidoscope "));
    Macros.type(PSTR(BUILD_INFORMATION));
  } else if (macroIndex == MACRO_ANY) {
    static Key lastKey;
    if (keyToggledOn(keyState))
      lastKey.keyCode = Key_A.keyCode + (uint8_t)(millis() % 36);

    if (keyIsPressed(keyState))
      kaleidoscope::hid::pressKey(lastKey);
  } else if (macroIndex == MACRO_LEDTOGGLE) {
    if(keyToggledOn(keyState)) {
      if(LEDControl.get_mode() == &LEDOff) LEDControl.next_mode();
      else LEDOff.activate();
    }
  } else if (macroIndex == MACRO_RAPIDFIRECLICK) {
    if(keyIsPressed(keyState)) {  // continuously while pressed
      static unsigned timer = 0;
      if(timer == 0) {
        timer = 10;
        return MACRO(Tr(Key_mouseBtnL));
      }
      timer--;
    }
  }
  return MACRO_NONE;
}

void setup() {
  Kaleidoscope.setup();
  Kaleidoscope.use(&BootGreetingEffect,
                   &LEDControl, &LEDOff,
                   &LEDRainbowEffect, &LEDRainbowWaveEffect, &LEDChaseEffect,
                   &solidYellow,
                   &LEDBreatheEffect,
                   &StalkerEffect,
#ifndef ARDUINO_VIRTUAL
                   &LetterGuesserEffect,
#endif
                   &NumLock,

                   &OneShot,
                   &EscapeOneShot,
                   &Macros,
                   &MacrosOnTheFly,
                   &MouseKeys,
                   &ActiveModColorEffect,
                   NULL);

  NumLock.numPadLayer = NUMPAD;
  LEDRainbowEffect.brightness(120);
  LEDRainbowWaveEffect.brightness(120);
  StalkerEffect.variant = STALKER(BlazingTrail);
  LEDRainbowEffect.activate();
}


void loop() {
  Kaleidoscope.loop();
}
