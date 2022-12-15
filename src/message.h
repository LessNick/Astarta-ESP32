#ifndef MESSAGE_h
#define MESSAGE_h

#include "config.h"
#include <Arduino.h>

#include "dispST7735.h"
#include "icons24.h"

#define MSG_TYPE_OK			0
#define MSG_TYPE_ERROR		1
#define MSG_TYPE_STOP		2
#define MSG_TYPE_INFO		3
#define MSG_TYPE_WARNING	4
#define MSG_TYPE_QUESTION	5
#define MSG_TYPE_CUSTOM		6

#define MSG_BG_COLOR		COLOR(148,180,242)
#define MSG_BTN_BG_COLOR	COLOR(191,213,251)

#define MSG_ICON_POS_X		8
#define MSG_ICON_POS_Y		19
#define MSG_ICON_WIDTH		24
#define MSG_ICON_HEIGHT		24

class Message {
	public:
		Message();

		void	show(DispST7735	&_disp, uint8_t msgType, String title, String msg);       // Инициализация
		void	showCustom(DispST7735 &_disp,String title, String msg, const uint8_t *icon, bool btnPresentEsc, bool btnPresentEnter);

	private:

		void	drawWindow(DispST7735 &_disp);
		void	drawText(DispST7735 &_disp, String msg);
		void	drawButton(DispST7735 &_disp, uint8_t x, uint8_t y, String btnName);

		bool	_btnPresentEsc;
		bool	_btnPresentEnter;
};

#endif
