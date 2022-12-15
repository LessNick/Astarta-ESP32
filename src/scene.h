#ifndef SCENE_h
#define SCENE_h

#include "config.h"
#include <Arduino.h>

#include "icons.h"
#include "icons24.h"
#include "logo.h"
#include "font_16p.h"

#include "dispST7735.h"
#include "navigator.h"
#include "message.h"
#include "keyboard.h"

#include <SD.h>

#include "config.h"

#include "sio.h"
#include "vRec.h"
#include "vDrive.h"

#include "net.h"

////////////////////////////////////////////////////////////////////////////////////////
#define SCENE_START			0	// Режим отображения стартового экрана
#define SCENE_REBOOT		1	// Режим отображения окна с ошибкой SD-карты
#define SCENE_MAINMENU		2	// Режим отображения главного меню
#define SCENE_SELECT_D1		3
#define SCENE_SELECT_D2		4
#define SCENE_SELECT_D3		5
#define SCENE_SELECT_D4		6
#define SCENE_SELECT_CAS	7

#define SCENE_MSG_OK		8	// Режим отображения окна с сообщением OK
#define SCENE_MSG_CONF		9	// Режим отображения окна с сообщением OK / CANCEL
#define SCENE_SEL_DIR		10	// Режим отображения меню выбора файла
#define SCENE_TOOLS			11	// Режим отображения меню tools
#define SCENE_SETTINGS		12	// Режим отображения меню settings

#define SCENE_INFO_WINDOW	13

#define SCENE_EJECT_D1		14
#define SCENE_EJECT_D2		15
#define SCENE_EJECT_D3		16
#define SCENE_EJECT_D4		17
#define SCENE_EJECT_CAS		18

#define SCENE_START_CAS		19
#define SCENE_PLAY_CAS		20


////////////////////////////////////////////////////////////////////////////////////////
#define S_NO_DISK	"No Disk"		// Для написания и сравнения
#define S_NO_CAS	"No Cassette"	// Для написания и сравнения
#define S_DEV_D1	"D1:"			// Для написания и сравнения
#define S_DEV_D2	"D2:"			// Для написания и сравнения
#define S_DEV_D3	"D3:"			// Для написания и сравнения
#define S_DEV_D4	"D4:"			// Для написания и сравнения
#define S_DEV_CAS	"C:"			// Для написания и сравнения

////////////////////////////////////////////////////////////////////////////////////////

#define C_MITEM_DEFAULT		COLOR(245,245,245)	// Цвет обычного пункта меню
#define C_MITEM_EMPTY		COLOR(200,200,200)	// Цвет пункта меню для пустого (No Disk/No Cas)
#define C_MITEM_ACTIVE		COLOR(255,255,255)	// Цвет пункта меню для вставленного

#define C_MITEM_DIR			COLOR(255,235,154)	// Цвет пункта меню для DIR
#define C_MITEM_DIR_UP		COLOR(200,200,200)	// Цвет пункта меню для DIR UP
#define C_MITEM_FILE		COLOR(245,245,245)	// Цвет пункта меню для FILE

class Scene {
	public:
		Scene();
		void		checkSD();
		void		refresh();
		
		void		showStart();
		void		showMainMenu();

	private:
		void		showNoSD();

		void		showSelectD1();
		void		showSelectD2();
		void		showSelectD3();
		void		showSelectD4();
		void		showSelectCAS();
		void		showSelect(String devName);

		void		showWrongType(uint8_t _bs);
		
		void		showEjectD1();
		void		showEjectD2();
		void		showEjectD3();
		void		showEjectD4();
		void		showEjectCAS();
		void		showEject(String devName);

		void		showSelectByDevId(String devId);

		void		showSelectProcessing(MenuList ml, String devId);

		void		showStartCas();
		void		showPlayCas(String casName);
		
		uint8_t		getIndexById(String devId);

		void		buildName(char *buff);

		String		rootPath;

		DispST7735	_disp;

		Navigator	nav;
		Message		winMsg;

		KeyBoard	kbd;

		Net			_net;
		bool		netInited = false;
		bool		netIniting = false;

		uint8_t		currentScene;
		uint8_t		backScene;
		uint8_t		actionScene;

		MenuList	mList[7];

		SIO			atariSio;
		vRecorder	vr;				// Виртуальный магнитофон
		
		bool		atariCMD;
		bool		_forceUpdate = false;
};

#endif
