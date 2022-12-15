#ifndef NAVIGATOR_h
#define NAVIGATOR_h

#include "config.h"
#include <Arduino.h>

#include "dispST7735.h"
#include "icons.h"

#define MTYPE_OK			0x00
#define MTYPE_D1			0x01
#define MTYPE_D2			0x02
#define MTYPE_D3			0x03
#define MTYPE_D4			0x04
#define MTYPE_CAS			0x05
#define MTYPE_TOOLS			0x06
#define MTYPE_SETTINGS		0x07
#define MTYPE_DIR_UP		0x08
#define MTYPE_DIR			0x09
#define MTYPE_DISKETTE		0x0a
#define MTYPE_CASSETTE		0x0b
#define MTYPE_XEX			0x0c
#define MTYPE_UNKNOWN		0xff

//// Структура данных для меню
struct MenuList {
	const uint8_t	*mIcon;
	uint8_t			mType;
	uint16_t		mColor;
	String			mName;
	String			mDis;
	String			mPath;
};

class Navigator {
	public:
		Navigator();
		void		init(DispST7735 &_disp, uint16_t mMenyBg, uint16_t mInfoBg, String title, MenuList *mList, uint16_t mListLen);		// Инициализация
		void		drawList(DispST7735 &_disp, uint16_t startPos);
		void		eraseCursor(DispST7735 &_disp);
		void		drawCursor(DispST7735 &_disp, uint16_t newPos);

		void		cursorUp(DispST7735 &_disp);
		void		cursorDown(DispST7735 &_disp);

		void		redraw(DispST7735 &_disp);

		MenuList	getCurrentItem();

	private:
		uint16_t	_mMenyBg;
		uint16_t	_mInfoBg;
		MenuList	*_mList;

		uint16_t	_mPos;
		uint16_t	_mSize;
		uint16_t	_mOffset;

		String	_title;

};

#endif
