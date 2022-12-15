#ifndef KEYBOARD_h
#define KEYBOARD_h

#include "config.h"
#include <Arduino.h>

#define KEY_NONE		0x00
#define KEY_ESC			0x01
#define KEY_UP			0x02
#define KEY_DOWN		0x03
#define KEY_ENTER		0x04
#define KEY_ACTION_1	0x05
#define KEY_ACTION_2	0x06

class KeyBoard {
	public:
		KeyBoard();
		uint8_t	refresh();
		void	reset();

	private:
		bool	_btnPressedEsc;
		bool	_btnPressedUp;
		bool	_btnPressedDown;
		bool	_btnPressedEnter;

		int		_tsEsc;
		int		_tsUp;
		int		_tsDown;
		int		_tsEnter;
};

#endif
