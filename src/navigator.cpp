#include "navigator.h"

Navigator::Navigator() {}

void Navigator::init(uint16_t mListLen) {
	_mSize = mListLen;
	_mList = new MenuList[_mSize];
}

void Navigator::setup(uint16_t mSceneBg, uint16_t mMenuFg, uint16_t mMenuBg, uint16_t mInfoFg, uint16_t mInfoBg, String title, uint16_t mListLen) {
  _mSize = mListLen;
	_mSceneBg = mSceneBg;
	_mMenuFg = mMenuFg;
	_mMenuBg = mMenuBg;
	_mInfoFg = mInfoFg;
	_mInfoBg = mInfoBg;

	_title = title;
	_mPos = 0;
	_mAddPos = 0;
	_mOffset = 0;

};

void Navigator::addItem(MenuList mItem) {
#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.println("Navigator addItem... ");
#endif
	_mList[_mAddPos] = mItem;
	_mAddPos += 1;
}

void Navigator::changeItem(uint16_t mPos, MenuList mItem) {
#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.println("Navigator changeItem... ");
#endif
	_mList[mPos] = mItem;
}

String Navigator::getItemName(uint16_t mPos) {
	return _mList[mPos].mName;
}

String Navigator::getItemPath(uint16_t mPos) {
	return _mList[mPos].mPath;
}

void Navigator::setSize(uint8_t w, uint8_t h) {
	_width  = w;
	_height = h;

	_mItemHeight = 15;
	_mMenuHeight = 10;
	_mInfoHeight = 10;

	_mMaxItems = (_height - _mMenuHeight - _mInfoHeight) / _mItemHeight;

#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.print("Max Items: ");
	LOG.println(_mMaxItems);
#endif

}

void Navigator::redraw(DispST7735 &_disp) {

#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.println("Navigator redraw... ");
#endif
	_disp.setBgColor(_mSceneBg);
	_disp.clearScreen();
	_disp.fillRect(0, 0, _width, _mMenuHeight, _mMenuBg);
	_disp.fillRect(0, _height-_mInfoHeight, _width, _mInfoHeight, _mInfoBg);
	_disp.setBgColor(_mMenuBg);
	_disp.drawFontString8P(2, 1, _title, _mMenuFg);
	_disp.restoreBgColor();
	
	drawList(_disp, _mOffset);
	drawCursor(_disp, _mPos);
}

void Navigator::drawList(DispST7735 &_disp, uint16_t startPos) {

#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.print("Navigator drawList... ");
#endif

	uint16_t endPos = startPos + 7;
	if (endPos > _mSize) endPos = _mSize;

	uint8_t yPos = 0;
	for (uint16_t i=startPos; i<endPos; i++) {
		MenuList item = _mList[i];
		drawItem(_disp, yPos, item);
		yPos++;
	}

#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.println("[ OK ]");
#endif

}

void Navigator::drawItem(DispST7735 &_disp, uint8_t yPos, MenuList item) {
	_disp.drawImage(2, 13 + (_mItemHeight * yPos), 12, 12, item.mIcon);
	_disp.fillRect(17, 16 + (_mItemHeight * yPos), 128-17-1, 8, _mSceneBg);
	_disp.drawFontString8P(17, 16 + (_mItemHeight * yPos), item.mName, item.mColor);
}

void Navigator::eraseCursor(DispST7735 &_disp) {
#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.print("Navigator eraseCursor... ");
#endif

	_disp.drawRect(0, 11 + (_mItemHeight * _mPos), 128, 16, _mSceneBg);

#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.println("[ OK ]");
#endif

}

void Navigator::drawCursor(DispST7735 &_disp, uint16_t newPos) {

#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.print("_mInfoBg:");
	LOG.print(_mInfoBg);
	LOG.print(", R=");
	LOG.print((_mInfoBg & 0xF800) >> 8);
	LOG.print(", G=");
	LOG.print((_mInfoBg & 0x07e0) >> 5);
	LOG.print(", B=");
	LOG.println(_mInfoBg & 0x001f);

	LOG.print("Navigator drawCursor... ");
#endif

	_mPos = newPos;
	_disp.drawRect(0, 11 + (_mItemHeight * _mPos), 128, 16, COLOR(149,170,237));
	_disp.setBgColor(_mInfoBg);
	
	_disp.fillRect(0, _height-_mInfoHeight, _width, _mInfoHeight, _mInfoBg);
	_disp.drawFontString8P(2, 119, _mList[_mPos + _mOffset].mDis, COLOR(0,255,0));
	_disp.restoreBgColor();

#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.println("[ OK ]");
#endif
}

void Navigator::cursorUp(DispST7735 &_disp) {

#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.print("Navigator cursorUp... ");
#endif

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

#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.println("[ OK ]");
#endif    

}

void Navigator::cursorDown(DispST7735 &_disp) {

#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.print("Navigator cursorDown... ");
#endif

	if (_mSize > _mMaxItems) {
		if (_mOffset + 1 < _mSize-(_mMaxItems-1)) {
			if (_mPos == _mMaxItems-1) {
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

#if defined(DEBUG) && defined(DEBUG_NAV)
	LOG.println("[ OK ]");
#endif

}

MenuList Navigator::getCurrentItem() {
	return _mList[_mPos + _mOffset];
}
