#ifndef MESSAGE_h
#define MESSAGE_h

#include "config.h"
#include <Arduino.h>

#include "dispST7735.h"
#include "icons24_rgb.h"

#define MSG_TYPE_OK       0
#define MSG_TYPE_ERROR    1
#define MSG_TYPE_STOP     2
#define MSG_TYPE_INFO     3
#define MSG_TYPE_WARNING  4
#define MSG_TYPE_QUESTION	5
#define MSG_TYPE_PROCESS	6
#define MSG_TYPE_CUSTOM		7

#define MSG_ICON_POS_X		8
#define MSG_ICON_POS_Y		19
#define MSG_ICON_WIDTH		24
#define MSG_ICON_HEIGHT		24

class Message {
	public:
		Message();
		void	init(uint16_t mSceneBg, uint16_t mWinFg, uint16_t mWinBg, uint16_t mBtnFg, uint16_t mBtnBg, uint16_t mBtnB1, uint16_t mBtnB2, uint16_t mBtnB3, uint16_t mBtnB4);		// Инициализация
		void	setSize(uint8_t w, uint8_t h);
		void	show(DispST7735	&_disp, uint8_t msgType, String title, String msg);       // Инициализация
		void	showCustom(DispST7735 &_disp,String title, String msg, const uint8_t *icon, bool btnPresentEsc, bool btnPresentEnter, uint8_t wOffset);
		void	updateMsg(DispST7735 &_disp, String msg);

	private:
		void	drawWindow(DispST7735 &_disp, uint8_t wOffset);
		void	drawText(DispST7735 &_disp, String msg, uint8_t wOffset);
		void	drawButton(DispST7735 &_disp, uint8_t x, uint8_t y, String btnName);

		uint8_t	_width;
		uint8_t	_height;

		bool	_btnPresentEsc;
		bool	_btnPresentEnter;
		uint8_t _wOffset;

		uint8_t _xPos;
		uint8_t _yPos;

		uint16_t _mSceneBg;
		uint16_t _mWinFg;
		uint16_t _mWinBg;
		uint16_t _mBtnFg;
		uint16_t _mBtnBg;

		uint16_t _mBtnB1;
		uint16_t _mBtnB2;
		uint16_t _mBtnB3;
		uint16_t _mBtnB4;
};

#endif
