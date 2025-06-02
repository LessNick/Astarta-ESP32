#ifndef CONFIG_h
#define CONFIG_h

#define	__VERSION__	"v2.44"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Конфигурационный файл для настройки проекта
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Разрешить вывод отладочной инфомации в терминал (глобально)
//#define DEBUG true

// Разрешить вывод отладочной инфомации при работе с образом ATXв терминал
#define DEBUG_ATX true

// Разрешить вывод отладочной инфомации при работе навигацией
#define DEBUG_NAV true

// Разрешить вывод отладочной инфомации при работе кнопками
#define DEBUG_BTNS true

// Разрешить вывод отладочной инфомации о сценах
#define DEBUG_SCENES true

// Разрешить вывод отладочной инфомации об окошках
#define DEBUG_MSG true

// Включить при сборке код отвечающий за работу с WiFi(Time, FTP)
#define ENABLE_WIFI true
// Поменять местами бит переключения между RGB и BGR цветовыми режимами
// Хорактерно, если цвета отображаются неверно
// #define FLIP_RGB_BGR true

#define MAX_NAV_ITEMS	384	// Reserved for 384 Navigator (Menu) items

#define LOG		Serial	// Отладочная консоль

#define LED_PIN		32	// PIN дисплея к которому подключена подсветка
#define SCK_PIN		14	// PIN дисплея к которому подключен Clock
#define DC_PIN		4	// PIN дисплея к которому подключен DC
#define MOSI_PIN	13	// PIN дисплея к которому подключен MOSI
#define RES_PIN		2	// PIN дисплея к которому подключен Reset
#define CS_PIN		15	// PIN дисплея к которому подключен CS

#define ATARI_CMD_PIN	21	// ESP32 pin IO21 Номер пина к которому подключен 7й конктакт ATARI (Command)
#define ATARI_CMD_DELAY	5	// Задержка перед приёмом кода команды

#define ATARI_SIO	Serial2	// Порт для подключения Atari

#define DEBOUNCE_DELAY	20	// Задержка в милисекундах для исключения «дребезга» контактов

#define BTN_PIN_ESC	33	// ESP32 pin IO33 Номер пина к которому подключена кнопка Отмена
#define BTN_PIN_UP	25	// ESP32 pin IO25 Номер пина к которому подключена кнопка Вверх
#define BTN_PIN_DOWN	26	// ESP32 pin IO26 Номер пина к которому подключена кнопка Вниз
#define BTN_PIN_ENTER	27	// ESP32 pin IO27 Номер пина к которому подключена кнопка Ок

#define BTN_C_PLAY	BTN_PIN_ENTER // На нажатие какой кнопки будет срабатывать запуск воспроизведения кассеты

// System Colors Set
#define MENU_TOP_COLOR			COLOR(255,255,0)	// Цвет в верхней панели меню
#define MENU_TOP_COLOR_TIME		COLOR(255,255,255)	// Цвет часов в верхней панели меню
#define MENU_TOP_COLOR_BG		COLOR(26,42,130)	// Цвет фона верхней панели меню
#define MENU_BOTTOM_COLOR		COLOR(255,255,0)	// Цвет в нижней панели меню
#define MENU_BOTTOM_COLOR_SEC_NUM	COLOR(255,255,255)	// Цвет номера сектора в нижней панели меню
#define MENU_BOTTOM_COLOR_SEC_R		COLOR(0,255,0)		// Цвет чтение сектора в нижней панели меню
#define MENU_BOTTOM_COLOR_SEC_W		COLOR(255,0,0)		// Цвет запись сектора в нижней панели меню
#define MENU_BOTTOM_COLOR_SEC_S		COLOR(255,175,0)	// Цвет статус сектора в нижней панели меню
#define MENU_BOTTOM_COLOR_SEC_F		COLOR(255,0,0)		// Цвет формат сектора в нижней панели меню
#define MENU_BOTTOM_COLOR_DP		COLOR(192,0,255)	// Цвет dev pool в нижней панели меню
#define MENU_BOTTOM_COLOR_CI		COLOR(0,255,255)	// Цвет chunk info в нижней панели меню
#define MENU_BOTTOM_COLOR_CD		COLOR(0,255,0)		// Цвет chunk data в нижней панели меню
#define MENU_BOTTOM_COLOR_NR		COLOR(0,175,255)	// Цвет notify run в нижней панели меню
#define MENU_BOTTOM_COLOR_D		COLOR(0,155,255)	// Цвет по умолчанию в нижней панели меню
#define MENU_BOTTOM_COLOR_BG		COLOR(47,70,172)	// Цвет фона нижней панели меню

#define START_PROJECT_COLOR		COLOR(255,255,0)	// Цвет надписи «Project» на начальном экране
#define START_VERSION_COLOR		COLOR(0,255,0)		// Цвет надписи «Version» на начальном экране

#define SCENE_COLOR_BG			COLOR(0,0,0)		// Цвет фона

#define MESSAGE_COLOR			COLOR(0,0,0)		// Цвет
#define MESSAGE_COLOR_BG		COLOR(148,180,242)	// Цвет фона окна с сообщениями
#define MESSAGE_BUTTON_COLOR		COLOR(0,0,0)		// Цвет
#define MESSAGE_BUTTON_COLOR_BG		COLOR(191,213,251)	// Цвет фона кнопок окна с сообщениями
#define MESSAGE_BUTTON_COLOR_B1		COLOR(69,97,202)	// Цвет 1 элемента рамки кнопок окна с сообщениями
#define MESSAGE_BUTTON_COLOR_B2		COLOR(47,70,172)	// Цвет 2 элемента рамки кнопок окна с сообщениями
#define MESSAGE_BUTTON_COLOR_B3		COLOR(148,180,242)	// Цвет 3 элемента рамки кнопок окна с сообщениями
#define MESSAGE_BUTTON_COLOR_B4		COLOR(101,139,227)	// Цвет 4 элемента рамки кнопок окна с сообщениями

#endif
