#include "keyboard.h"

KeyBoard::KeyBoard() {
	pinMode(BTN_PIN_ESC,	INPUT_PULLUP);	// set ESP32 pin to input pull-up mode
	pinMode(BTN_PIN_UP,		INPUT_PULLUP);	// set ESP32 pin to input pull-up mode
	pinMode(BTN_PIN_DOWN,	INPUT_PULLUP);	// set ESP32 pin to input pull-up mode
	pinMode(BTN_PIN_ENTER,	INPUT_PULLUP);	// set ESP32 pin to input pull-up mode

	reset();
}

uint8_t KeyBoard::refresh() {
	uint8_t st = KEY_NONE;

	if (digitalRead(BTN_PIN_ESC) == LOW) {
		_btnPressedEsc = true;
		_tsEsc = millis();
	} else {
		if (_btnPressedEsc && (millis() - _tsEsc > DEBOUNCE_DELAY)) {
			_btnPressedEsc = false;
			st = KEY_ESC;

#if defined(DEBUG) && defined(DEBUG_BTNS)
			LOG.println("Key ESC released!");
#endif

		}
	}
	
	if (digitalRead(BTN_PIN_UP) == LOW) {
#if defined(DEBUG) && defined(DEBUG_BTNS)
		if (_btnPressedUp != true) {
			LOG.println("Key UP pressed!");
		}
#endif
		_btnPressedUp = true;
		_tsUp = millis();
	} else {
		if (_btnPressedUp && (millis() - _tsUp > DEBOUNCE_DELAY)) {
			_btnPressedUp = false;
			if (_btnPressedEnter == true) {
				st = KEY_ACTION_1;
				_btnPressedEnter = false;
				_tsEnter = millis();
#if defined(DEBUG) && defined(DEBUG_BTNS)
				LOG.println("Key ACTION_1 released!");
#endif
			} else {
				st = KEY_UP;
#if defined(DEBUG) && defined(DEBUG_BTNS)
				LOG.println("Key UP released!");
#endif
			}
		}
	}
	
	if (digitalRead(BTN_PIN_DOWN) == LOW) {
#if defined(DEBUG) && defined(DEBUG_BTNS)
		if (_btnPressedDown != true) {
			LOG.println("Key DOWN pressed!");
		}
#endif
		_btnPressedDown = true;
		_tsDown = millis();
	} else {
		if (_btnPressedDown && (millis() - _tsDown > DEBOUNCE_DELAY)) {
			_btnPressedDown = false;
			if (_btnPressedEnter == true) {
				st = KEY_ACTION_2;
				_btnPressedEnter = false;
				_tsEnter = millis();
			} else {
				st = KEY_DOWN;

#if defined(DEBUG) && defined(DEBUG_BTNS)
				LOG.println("Key DOWN released!");
#endif

			}
		}
	}
	
	if (digitalRead(BTN_PIN_ENTER) == LOW) {
#if defined(DEBUG) && defined(DEBUG_BTNS)
		if (_btnPressedEnter != true) {
			LOG.println("Key ENTER pressed!");
		}
#endif
		_btnPressedEnter = true;
		_tsEnter = millis();
	} else {
		if (_btnPressedEnter && (millis() - _tsEnter > DEBOUNCE_DELAY)) {
			_btnPressedEnter = false;
			if (_btnPressedUp == true) {
				st = KEY_ACTION_1;
				_btnPressedUp = false;
				_tsEnter = millis();
#if defined(DEBUG) && defined(DEBUG_BTNS)
				LOG.println("Key ACTION_1 released!");
#endif
			} else {
			  st = KEY_ENTER;

#if defined(DEBUG) && defined(DEBUG_BTNS)
			LOG.println("Key ENTER released!");
#endif
			}
		}
	}

	return st;
}

void KeyBoard::reset() {
	_btnPressedEsc = false;
	_btnPressedUp = false;
	_btnPressedDown = false;
	_btnPressedEnter = false;

	_tsEsc = 0;
	_tsUp = 0;
	_tsDown = 0;
	_tsEnter = 0;
}
