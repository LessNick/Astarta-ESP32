#ifndef NET_h
#define NET_h

#include "config.h"
#include <Arduino.h>

#include "dispST7735.h"

#include <time.h> 
#include <WiFi.h>
#include <SD.h>

#include "message.h"

// #ifdef FLIP_RGB_BGR
// #include "iconsRSSI_bgr.h"
// #else
#include "iconsRSSI_rgb.h"
// #endif

#include "configParser.h"

#define FTP_CTRL_PORT		21		// Command port on wich server is listening
#define FTP_DATA_PORT_PASV	50009	// Data port in passive mode

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Структура данных дата + время
struct NetDateTime {
	byte day;		// +00 День
	byte month;		// +01 Месяц
	byte year;		// +02 Год % 100
	byte hour;		// +03 Часы
	byte minute;	// +04 Минуты
	byte second;	// +05 Секунды
};

////////////////////////////////////////////////////////////////////////////////////////
//// Уровень сигнала

const byte RSSI_NONE		= 0x00;	// not connected
const byte RSSI_EXCELENT	= 0x01;	// -30 dBm = Excellent - Max achievable signal strength.
const byte RSSI_VERYGOOD	= 0x02;	// -67 dBm = Very Good - Minimum signal strength for most business applications
const byte RSSI_OKAY		= 0x03;	// -70 dBm = Okay - Minimum signal strength required for a decent packet delivery.
const byte RSSI_NOTGOOD		= 0x04;	// -80 dBm = Not Good - Minimum signal strength for basic connectivity. Packet delivery may be unreliable.
const byte RSSI_UNUSABLE	= 0x05;	// -90 dBm = Unusable - Approaching or drowning in the background transmissions (or noise floor)

////////////////////////////////////////////////////////////////////////////////////////

class Net {
	public:
		Net();
    void  setSize(uint8_t w, uint8_t h);
		void 				init(DispST7735 *disp, Message *winMsg, ConfigParser *cp, uint16_t mColor, uint16_t mColorBg);
		bool				checkWiFi();
		bool				update(bool force);
		NetDateTime*		getDateTime();
		bool				isTimeInited();

		void 				tPrint(String str);

	private:
    uint8_t   _width;
		uint8_t   _height;
		DispST7735			*_disp;
		Message				*_winMsg;

		ConfigParser		*_cp;

		String				ssid;
		String				password;
		String				hostname;

		String				ftpLogin;
		String				ftpPass;

		WiFiServer			*ftpServer;
		WiFiServer			*dataServer;

		String				rootPath	= "/";
		String				renamePath	= "";

		const String		mNames[12]	= {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

		const uint8_t		posX		= 128-2-(5*6)+4-12-5;
		const uint8_t		posY		= 1;

		long				timeZone	= 3; 
		byte				daySaveTime = 0;

		short int			checkCount	= 0;
		unsigned short int	timeCount	= 0;
		struct tm			tmStruct;

		NetDateTime			ntd;		// Структура с текущими данными (Дата и Время)
		bool				checkFTP();
		void				parseCmd(WiFiClient client, String currentCmd, String currentArgs);

		byte				_lastMin;
		byte				_lastRSSI;

		uint8_t				aPos;
		const uint8_t		aSize = 8;
		const uint8_t *		aList[8]	= { unusable, notGood, okay, veryGood, excellent, veryGood, okay, notGood};

		uint8_t				tryCount;

		bool				timeInited;

    uint8_t _xPos;
    uint8_t _yPos;

    uint16_t _mColor;
    uint16_t _mColorBg;

};

#endif
