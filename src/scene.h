#ifndef SCENE_h
#define SCENE_h

#include "config.h"
#include <Arduino.h>

#include "font_16p.h"

//#ifdef FLIP_RGB_BGR
// #include "icons_bgr.h"
// #include "icons24_bgr.h"
// #include "logo_bgr.h"
// #else
#include "icons_rgb.h"
#include "icons24_rgb.h"
#include "logo_rgb.h"
// #endif

#include "dispST7735.h"
#include "navigator.h"
#include "message.h"
#include "keyboard.h"

#include <SD.h>

#include "config.h"

#include "sio.h"
#include "vRec.h"
#include "vDrive.h"

#ifdef ENABLE_WIFI
#include "net.h"
#endif

#include "configParser.h"

////////////////////////////////////////////////////////////////////////////////////////
#define SCENE_START		0	// Режим отображения стартового экрана
#define SCENE_CONNECT		1	// Режим отображения окна подключение к WiFi
#define SCENE_REBOOT		2	// Режим отображения окна с ошибкой SD-карты
#define SCENE_MAINMENU		3	// Режим отображения главного меню
#define SCENE_SELECT_D1		4
#define SCENE_SELECT_D2		5
#define SCENE_SELECT_D3		6
#define SCENE_SELECT_D4		7
#define SCENE_SELECT_CAS	8

#define SCENE_MSG_OK		9	// Режим отображения окна с сообщением OK
#define SCENE_MSG_CONF		10	// Режим отображения окна с сообщением OK / CANCEL
#define SCENE_SEL_DIR		11	// Режим отображения меню выбора файла
#define SCENE_TOOLS		12	// Режим отображения меню tools
#define SCENE_SETTINGS		13	// Режим отображения меню settings

#define SCENE_INFO_WINDOW	14

#define SCENE_EJECT_D1		15
#define SCENE_EJECT_D2		16
#define SCENE_EJECT_D3		17
#define SCENE_EJECT_D4		18
#define SCENE_EJECT_CAS		19

#define SCENE_START_CAS		20
#define SCENE_PLAY_CAS		21
#define SCENE_STOP_CAS		22


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

#define C_MITEM_DIR		COLOR(255,235,154)	// Цвет пункта меню для DIR
#define C_MITEM_DIR_UP		COLOR(200,200,200)	// Цвет пункта меню для DIR UP
#define C_MITEM_FILE		COLOR(245,245,245)	// Цвет пункта меню для FILE

class Scene {
	public:
		Scene();
    void    setSize(uint8_t w, uint8_t h);
		void		checkSD();
		void		refresh();
		
		void		loadConfig();
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

		void		showTools();
		void		showSettings();
		
		uint8_t		getIndexById(String devId);

		void		buildName(char *buff);

		uint8_t		_width;
		uint8_t		_height;

		String		rootPath;

		DispST7735	_disp;

		Navigator	nav;
		Message		winMsg;

		KeyBoard	kbd;

#ifdef ENABLE_WIFI
		Net		_net;
		bool		netInited = false;
		bool		netIniting = false;
#endif

		uint8_t		currentScene;
		uint8_t		backScene;
		uint8_t		actionScene;

		SIO		atariSio;
		vRecorder	vr;				// Виртуальный магнитофон
		
		bool		atariCMD;
		bool		_forceUpdate = false;

		ConfigParser _cp;
};

#endif
