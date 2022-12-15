#include "message.h"

Message::Message() {}

void Message::show(DispST7735 &_disp, uint8_t msgType, String title, String msg) {
	_btnPresentEsc = false;
	_btnPresentEnter = false;

	drawWindow(_disp);

	switch (msgType) {
		case MSG_TYPE_OK: {
			_disp.drawImage(MSG_ICON_POS_X, MSG_ICON_POS_Y, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, iconOk);
			drawButton(_disp, 40,90, "OK");
			_btnPresentEnter = true;
			break;
		}
		case MSG_TYPE_ERROR: {
			_disp.drawImage(MSG_ICON_POS_X, MSG_ICON_POS_Y, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, iconError);
			drawButton(_disp, 40,90, "OK");
			_btnPresentEnter = true;
			break;
		}
		case MSG_TYPE_STOP: {
			_disp.drawImage(MSG_ICON_POS_X, MSG_ICON_POS_Y, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, iconStop);
			drawButton(_disp, 40,90, "REBOOT");
			_btnPresentEnter = true;
			break;
		}
		case MSG_TYPE_INFO: {
			_disp.drawImage(MSG_ICON_POS_X, MSG_ICON_POS_Y, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, iconInfo);
			drawButton(_disp, 40,90, "OK");
			_btnPresentEnter = true;
			break;
		}
		case MSG_TYPE_WARNING: {
			_disp.drawImage(MSG_ICON_POS_X, MSG_ICON_POS_Y, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, iconWarning);
			drawButton(_disp, 40,90, "OK");
			_btnPresentEnter = true;
			break;
		}
		case MSG_TYPE_QUESTION: {
			_disp.drawImage(MSG_ICON_POS_X, MSG_ICON_POS_Y, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, iconQuestion);
			drawButton(_disp, 10,90, "OK");
			drawButton(_disp, 66,90, "CANCEL");
			_btnPresentEnter = true;
			_btnPresentEsc = true;
			break;
		}
		default: {
			break;
		}
	}

	_disp.setBgColor(MSG_BG_COLOR);
	_disp.drawFontString16P(37, 25, title, COLOR(0,0,0));
	drawText(_disp, msg);
}

void Message::showCustom(DispST7735 &_disp, String title, String msg, const uint8_t *icon, bool btnPresentEsc, bool btnPresentEnter) {
	_btnPresentEsc = btnPresentEsc;
	_btnPresentEnter = btnPresentEnter;

	drawWindow(_disp);

	_disp.drawImage(MSG_ICON_POS_X, MSG_ICON_POS_Y, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, icon);
	if (_btnPresentEsc && _btnPresentEnter) {
		drawButton(_disp, 10,90, "OK");
		drawButton(_disp, 66,90, "CANCEL");
	} else if (_btnPresentEsc) {
		drawButton(_disp, 40,90, "CANCEL");
	} else if (_btnPresentEnter) {
		drawButton(_disp, 40,90, "OK");
	}

	_disp.setBgColor(MSG_BG_COLOR);
	_disp.drawFontString16P(37, 25, title, COLOR(0,0,0));
	drawText(_disp, msg);
}

void Message::drawText(DispST7735 &_disp, String msg) {
	uint8_t mLinesCount = 1;
	for (int i=0; i<msg.length(); i++) {
		if (msg.charAt(i) == '\n') mLinesCount++;
	}
	String mLines[mLinesCount];
	uint8_t mLinePos=0;
	for (int i=0; i<msg.length(); i++) {
		if (msg.charAt(i) == '\n') {
			mLinePos++;
		} else {
			mLines[mLinePos] += msg.charAt(i);
		}
	}
	for (int i=0; i<mLinesCount; i++) {
		_disp.drawFontCenter8P(3, 51+(i*9), 128-6, mLines[i], COLOR(0,0,0));
	}
}

void Message::drawWindow(DispST7735 &_disp) {
	_disp.drawRect(0, 11, 128, 106, COLOR(191,213,251));
	_disp.drawRect(1, 12, 126, 104, COLOR(0,0,0));
	_disp.fillRect(2, 13, 124, 102, MSG_BG_COLOR);

}

void Message::drawButton(DispST7735 &_disp, uint8_t x, uint8_t y, String btnName) {
	uint8_t w = 48, h = 17;
	_disp.fillRect(x+2, y+2, w-4, h-4, MSG_BTN_BG_COLOR);

	for (uint8_t px=x; px<x+w; px++) {
		_disp.drawPixel(px, y, COLOR(69,97,202));
		_disp.drawPixel(px, y+h-1, COLOR(47,70,172));
	}

	for (uint8_t py=y+1; py<y+h-1; py++) {
		_disp.drawPixel(x, py, COLOR(69,97,202));
		_disp.drawPixel(x+w-1, py, COLOR(47,70,172));
	}

	for (uint8_t px=x+1; px<x+w-1; px++) {
		_disp.drawPixel(px, y+1, COLOR(148,180,242));
		_disp.drawPixel(px, y+h-2, COLOR(101,139,227));
	}

	for (uint8_t py=y+2; py<y+h-2; py++) {
		_disp.drawPixel(x+1, py, COLOR(148,180,242));
		_disp.drawPixel(x+w-2, py, COLOR(101,139,227));
	}

	_disp.setBgColor(MSG_BTN_BG_COLOR);
	_disp.drawFont6String(x+(48 - btnName.length()*6)/2+1, y+6, btnName, COLOR(0,0,0));

}
