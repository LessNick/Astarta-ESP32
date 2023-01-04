#ifndef CONFIG_h
#define CONFIG_h

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Конфигурационный файл для настройки проекта
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOG					Serial		// Отладочная консоль

#define LED_PIN				32			// PIN дисплея к которому подключена подсветка
#define SCK_PIN				14			// PIN дисплея к которому подключен Clock
#define DC_PIN				4			// PIN дисплея к которому подключен DC
#define MOSI_PIN			13			// PIN дисплея к которому подключен MOSI
#define RES_PIN				2			// PIN дисплея к которому подключен Reset
#define CS_PIN				15			// PIN дисплея к которому подключен CS

#define ATARI_CMD_PIN		21			// Номер пина к которому подключен 7й конктакт ATARI (Command)
#define ATARI_CMD_DELAY		5			// Задержка перед приёмом кода команды

#define ATARI_SIO			Serial2		// Порт для подключения Atari

#define DEBOUNCE_DELAY		20			// Задержка в милисекундах для исключения «дребезга» контактов

#define BTN_PIN_ESC			33			// ESP32 pin IO33
#define BTN_PIN_UP			25			// ESP32 pin IO25
#define BTN_PIN_DOWN		26			// ESP32 pin IO26
#define BTN_PIN_ENTER		27			// ESP32 pin IO27

#define BTN_C_PLAY			BTN_PIN_ENTER

//#define DEBUG				true

#define MENU_TOP_COLOR		COLOR(26,42,130)
#define MENU_BOTTOM_COLOR	COLOR(47,70,172)

#endif
