#include "message.h"

Message::Message() {}

void Message::init(uint16_t mSceneBg, uint16_t mWinFg, uint16_t mWinBg, uint16_t mBtnFg, uint16_t mBtnBg, uint16_t mBtnB1, uint16_t mBtnB2, uint16_t mBtnB3, uint16_t mBtnB4) {
	_mSceneBg = mSceneBg;
	_mWinFg = mWinFg;
	_mWinBg = mWinBg;
	_mBtnFg = mBtnFg;
	_mBtnBg = mBtnBg;

	_mBtnB1 = mBtnB1;
	_mBtnB2 = mBtnB2;
	_mBtnB3 = mBtnB3;
	_mBtnB4 = mBtnB4;
}

void Message::setSize(uint8_t w, uint8_t h) {
	_width  = w;
	_height = h;

	_xPos = (w - 128) / 2;
	_yPos = (h - 128) / 2;
}

void Message::show(DispST7735 &_disp, uint8_t msgType, String title, String msg) {
	_btnPresentEsc = false;
	_btnPresentEnter = false;

	uint8_t wOffset = 0;

#if defined(DEBUG) && defined(DEBUG_MSG)
	LOG.println("Show Message Window:");
	LOG.print("Type: ");
#endif

	if (msgType == MSG_TYPE_PROCESS) {
		wOffset = 20;
	}
	
	drawWindow(_disp, wOffset);

	switch (msgType) {
		case MSG_TYPE_OK: {
			_disp.drawImage(_xPos + MSG_ICON_POS_X, _yPos + MSG_ICON_POS_Y + wOffset, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, iconOk);
			drawButton(_disp, _xPos + 40, _yPos + 90, "OK");
			_btnPresentEnter = true;

#if defined(DEBUG) && defined(DEBUG_MSG)
			LOG.println("OK");
#endif
			break;
		}
		case MSG_TYPE_ERROR: {
			_disp.drawImage(_xPos + MSG_ICON_POS_X, _yPos + MSG_ICON_POS_Y + wOffset, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, iconError);
			drawButton(_disp, _xPos + 40, _yPos + 90, "OK");
			_btnPresentEnter = true;

#if defined(DEBUG) && defined(DEBUG_MSG)
			LOG.println("ERROR");
#endif
			break;
		}
		case MSG_TYPE_STOP: {
			_disp.drawImage(_xPos + MSG_ICON_POS_X, _yPos + MSG_ICON_POS_Y + wOffset, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, iconStop);
			drawButton(_disp, _xPos + 40, _yPos + 90, "REBOOT");
			_btnPresentEnter = true;
#if defined(DEBUG) && defined(DEBUG_MSG)
			LOG.println("STOP");
#endif
			break;
		}
		case MSG_TYPE_INFO: {
			_disp.drawImage(_xPos + MSG_ICON_POS_X, _yPos + MSG_ICON_POS_Y + wOffset, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, iconInfo);
			drawButton(_disp, _xPos + 40, _yPos + 90, "OK");
			_btnPresentEnter = true;

#if defined(DEBUG) && defined(DEBUG_MSG)
			LOG.println("INFO");
#endif
			break;
		}
		case MSG_TYPE_PROCESS: {
			_disp.drawImage(_xPos + MSG_ICON_POS_X, _yPos + MSG_ICON_POS_Y + wOffset, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, iconInfo);
#if defined(DEBUG) && defined(DEBUG_MSG)
			LOG.println("PROCESS");
#endif
			break;
		}
		case MSG_TYPE_WARNING: {
			_disp.drawImage(_xPos + MSG_ICON_POS_X, _yPos + MSG_ICON_POS_Y + wOffset, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, iconWarning);
			drawButton(_disp, _xPos + 40, _yPos + 90, "OK");
			_btnPresentEnter = true;
#if defined(DEBUG) && defined(DEBUG_MSG)
			LOG.println("WARNING");
#endif
			break;
		}
		case MSG_TYPE_QUESTION: {
			_disp.drawImage(_xPos + MSG_ICON_POS_X, _yPos + MSG_ICON_POS_Y + wOffset, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, iconQuestion);
			drawButton(_disp, _xPos + 10, _yPos + 90, "OK");
			drawButton(_disp, _xPos + 66, _yPos + 90, "CANCEL");
			_btnPresentEnter = true;
			_btnPresentEsc = true;

#if defined(DEBUG) && defined(DEBUG_MSG)
			LOG.println("QUESTION");
#endif
			break;
		}
		default: {
			break;
		}
	}

#if defined(DEBUG) && defined(DEBUG_MSG)
      LOG.print("Message: ");
      LOG.println(msg);
#endif

	_disp.setBgColor(_mWinBg);
	_disp.drawFontString16P(_xPos + 37, _yPos + 25 + wOffset, title, _mWinFg);
	drawText(_disp, msg, wOffset);
	_wOffset = wOffset;
}

void Message::showCustom(DispST7735 &_disp, String title, String msg, const uint8_t *icon, bool btnPresentEsc, bool btnPresentEnter, uint8_t wOffset) {
	_btnPresentEsc = btnPresentEsc;
	_btnPresentEnter = btnPresentEnter;

#if defined(DEBUG) && defined(DEBUG_MSG)
	LOG.println("Show Message Window:");
	LOG.println("Type: CUSTOM");
	LOG.print("Message: ");
	LOG.println(msg);
#endif

	drawWindow(_disp, wOffset);
	_disp.drawImage(_xPos + MSG_ICON_POS_X, _yPos + MSG_ICON_POS_Y + wOffset, MSG_ICON_WIDTH, MSG_ICON_HEIGHT, icon);

	if (_btnPresentEsc && _btnPresentEnter) {
		drawButton(_disp, _xPos + 10, _yPos + 90 + wOffset, "OK");
		drawButton(_disp, _xPos + 66, _yPos + 90 + wOffset, "CANCEL");

	} else if (_btnPresentEsc) {
		drawButton(_disp, _xPos + 40, _yPos + 90 + wOffset, "CANCEL");

	} else if (_btnPresentEnter) {
		drawButton(_disp, _xPos + 40, _yPos + 90 + wOffset, "OK");
	}

	_disp.setBgColor(_mWinBg);
	_disp.drawFontString16P(_xPos + 37, _yPos + 25, title, _mWinFg);
	drawText(_disp, msg, wOffset);
}

void Message::updateMsg(DispST7735 &_disp, String msg) {
	drawText(_disp, msg, _wOffset);
}

void Message::drawText(DispST7735 &_disp, String msg, uint8_t wOffset) {
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
		_disp.drawFontCenter8P(_xPos + 3, _yPos + 51 + (i*9) + wOffset, _width - 6, mLines[i], _mWinFg);
	}
}

void Message::drawWindow(DispST7735 &_disp, uint8_t wOffset) {
	_disp.drawRect(_xPos + 0, _yPos + 11 + wOffset, 128, 106 - (wOffset * 2), _mWinBg);
	_disp.drawRect(_xPos + 1, _yPos + 12 + wOffset, 126, 104 - (wOffset * 2), _mSceneBg);
	_disp.fillRect(_xPos + 2, _yPos + 13 + wOffset, 124, 102 - (wOffset * 2), _mWinBg);

}

void Message::drawButton(DispST7735 &_disp, uint8_t x, uint8_t y, String btnName) {
	uint8_t w = 48, h = 17;
	_disp.fillRect(x+2, y+2, w-4, h-4, _mBtnBg);
	for (uint8_t px=x; px<x+w; px++) {
		_disp.drawPixel(px, y, _mBtnB1);
		_disp.drawPixel(px, y+h-1, _mBtnB2);
	}

	for (uint8_t py=y+1; py<y+h-1; py++) {
		_disp.drawPixel(x, py, _mBtnB1);
		_disp.drawPixel(x+w-1, py, _mBtnB2);
	}

	for (uint8_t px=x+1; px<x+w-1; px++) {
		_disp.drawPixel(px, y+1, _mBtnB3);
		_disp.drawPixel(px, y+h-2, _mBtnB4);

	}

	for (uint8_t py=y+2; py<y+h-2; py++) {
		_disp.drawPixel(x+1, py, _mBtnB3);
		_disp.drawPixel(x+w-2, py, _mBtnB4);
  }

	_disp.setBgColor(_mBtnBg);
	_disp.drawFont6String(x+(48 - btnName.length()*6)/2+1, y+6, btnName, _mBtnFg);
}
