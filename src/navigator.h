#ifndef NAVIGATOR_h
#define NAVIGATOR_h

#include "config.h"
#include <Arduino.h>

#include "dispST7735.h"
#include "icons_rgb.h"

#define MTYPE_OK		0x00
#define MTYPE_D1		0x01
#define MTYPE_D2		0x02
#define MTYPE_D3		0x03
#define MTYPE_D4		0x04
#define MTYPE_CAS		0x05
#define MTYPE_TOOLS		0x06
#define MTYPE_SETTINGS		0x07
#define MTYPE_DIR_UP		0x08
#define MTYPE_DIR		0x09
#define MTYPE_DISKETTE		0x0a
#define MTYPE_CASSETTE		0x0b
#define MTYPE_XEX		0x0c
#define MTYPE_UNKNOWN		0xff

//// Структура данных для меню
struct MenuList {
	const uint8_t	*mIcon;
	uint8_t		mType;
	uint16_t	mColor;
	String		mName;
	String		mDis;
	String		mPath;
};

class Navigator {
	public:
		Navigator();
		// Инициализация
		void		init(uint16_t mListLen);
		void		setup(uint16_t mSceneBg, uint16_t mMenuFg, uint16_t mMenuBg, uint16_t mInfoFg, uint16_t mInfoBg, String title, uint16_t mListLen);
		void		addItem(MenuList mItem);
		void		changeItem(uint16_t mPos, MenuList mItem);
		String		getItemName(uint16_t mPos);
		String		getItemPath(uint16_t mPos);

		void		setSize(uint8_t w, uint8_t h);
		void		drawList(DispST7735 &_disp, uint16_t startPos);
		void		drawItem(DispST7735 &_disp, uint8_t yPos, MenuList item);
		void		eraseCursor(DispST7735 &_disp);
		void		drawCursor(DispST7735 &_disp, uint16_t newPos);

		void		cursorUp(DispST7735 &_disp);
		void		cursorDown(DispST7735 &_disp);

		void		redraw(DispST7735 &_disp);

		MenuList	getCurrentItem();

	private:
		uint8_t		_width;
		uint8_t		_height;

		uint16_t	_mSceneBg;
		uint16_t	_mMenuFg;
		uint16_t	_mMenuBg;
		uint16_t	_mInfoFg;
		uint16_t	_mInfoBg;
		MenuList	*_mList;

		uint16_t	_mPos;
		uint16_t	_mAddPos;
		uint16_t	_mSize;
		uint16_t	_mOffset;

		String		_title;

		uint8_t		_mMaxItems;
		uint8_t		_mItemHeight;

		uint8_t		_mMenuHeight;
		uint8_t		_mInfoHeight;

};

#endif
