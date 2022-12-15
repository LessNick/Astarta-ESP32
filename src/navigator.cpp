#include "navigator.h"

Navigator::Navigator() {}

void Navigator::init(DispST7735 &_disp, uint16_t mMenyBg, uint16_t mInfoBg, String title, MenuList *mList, uint16_t mListLen) {
	_mMenyBg = mMenyBg;
	_mInfoBg = mInfoBg;

	_title = title;
	
	_mList = new MenuList[mListLen];
	
	for(int i = 0; i < mListLen; i++) {
		_mList[i] = mList[i];
	}

	_mSize = mListLen;
	_mPos = 0;
	_mOffset = 0;
	redraw(_disp);
};

void Navigator::redraw(DispST7735 &_disp) {

	_disp.setBgColor(0);
	_disp.clearScreen();
	_disp.fillRect(0, 0, 128, 10, _mMenyBg);
	_disp.fillRect(0, 118, 128, 10, _mInfoBg);

	_disp.setBgColor(_mMenyBg);
	_disp.drawFontString8P(2, 1, _title, COLOR(255,255,0));

	_disp.restoreBgColor();
	
	drawList(_disp, _mOffset);
	drawCursor(_disp, _mPos);
}

void Navigator::drawList(DispST7735 &_disp, uint16_t startPos) {  
	uint16_t endPos = startPos + 7;
	if (endPos > _mSize) endPos = _mSize;

	uint8_t yPos = 0;
	for (uint16_t i=startPos; i<endPos; i++) {
		MenuList item = _mList[i];
		_disp.drawImage(2, 13 + (15*yPos), 12, 12, item.mIcon);
		_disp.fillRect(17, 16 + (15*yPos), 128-17-1, 8, BG_COLOR);
		_disp.drawFontString8P(17, 16 + (15*yPos), item.mName, item.mColor);
		yPos++;
	}
}

void Navigator::eraseCursor(DispST7735 &_disp) {
	_disp.drawRect(0, 11 + (15*_mPos), 128, 16, BG_COLOR);
}

void Navigator::drawCursor(DispST7735 &_disp, uint16_t newPos) {
	_mPos = newPos;

	_disp.drawRect(0, 11 + (15*_mPos), 128, 16, COLOR(149,170,237));

	_disp.setBgColor(_mInfoBg);
	_disp.fillRect(0, 118, 128, 10, _mInfoBg);
	_disp.drawFontString8P(2, 119, _mList[_mPos + _mOffset].mDis, COLOR(0,255,0));
	_disp.restoreBgColor();
}

void Navigator::cursorUp(DispST7735 &_disp) {
	if (_mOffset + _mPos - 1 >= 0) {
		if (_mPos == 0) {
			_mOffset--;
			drawList(_disp, _mOffset);
			drawCursor(_disp, _mPos);
		} else {
			eraseCursor(_disp);
			drawCursor(_disp, _mPos - 1);
		}
	}
}

void Navigator::cursorDown(DispST7735 &_disp) {
	if (_mSize > 7) {
		if (_mOffset + 1 < _mSize-6) {
			if (_mPos == 6) {
				_mOffset++;
				drawList(_disp, _mOffset);
				drawCursor(_disp, _mPos);
			
			} else {
				eraseCursor(_disp);
				drawCursor(_disp, _mPos + 1);
			}
		}
	} else {
		if (_mPos < _mSize-1) {
			eraseCursor(_disp);
			drawCursor(_disp, _mPos + 1);
		}
	}
}

MenuList Navigator::getCurrentItem() {
	return _mList[_mPos + _mOffset];
}

