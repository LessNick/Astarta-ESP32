#ifndef DISP_ST7735_h
#define DISP_ST7735_h

#include "config.h"
#include <Arduino.h>
#include <SPI.h>

#define COLOR(R,G,B)		((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3)

#define DISP_WIDTH			128		// Ширина дисплея
#define DISP_HEIGHT			128		// Высота дисплея

#define MADCTL_MY			0x80
#define MADCTL_MX			0x40
#define MADCTL_MV			0x20
#define MADCTL_ML			0x10
#define MADCTL_RGB			0x08
#define MADCTL_MH			0x04

#define ROTATE_RIGHT_BOTTOM	0
#define ROTATE_LEFT_BOTTOM	1
#define ROTATE_LEFT_TOP		2
#define ROTATE_RIGHT_TOP	3

// #define BG_COLOR			0x0000

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Управляющие команды SPI
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Управляющие команды ST7735
// Системные
#define ST7735_NOP			0x00	// Нет команды
#define ST7735_SWRESET		0x01	// Software Reset
#define ST7735_RDDID		0x04	// Read Display ID
#define ST7735_RDDST		0x09	// Read Display Status
#define ST7735_RDDPM		0x0a	// Read Display Power Mode
#define ST7735_RDDMADCTL	0x0b	// Read Display MADCTL
#define ST7735_RDDCOLMOD	0x0c	// Read Display Pixel Format
#define ST7735_RDDIM		0x0d	// Read Display Image Mode
#define ST7735_RDDSM		0x0e	// Read Display Signal Mode
#define ST7735_RDDSDR		0x0f	// Read Display Self-Diagnostic Result
#define ST7735_SLPIN		0x10	// Sleep In
#define ST7735_SLPOUT		0x11	// Sleep Out
#define ST7735_PTLON		0x12	// Partial Display Mode On
#define ST7735_NORON		0x13	// Partial Display Mode Off (Normal Mode)
#define ST7735_INVOFF		0x20	// Display Inversion Off
#define ST7735_INVON		0x21	// Display Inversion On
#define ST7735_GAMSET		0x26	// Gamma curve select
#define ST7735_DISPOFF		0x28	// Display Off
#define ST7735_DISPON		0x29	// Display On
#define ST7735_CASET		0x2a	// Column Address Set
#define ST7735_RASET		0x2b	// Row Address Set
#define ST7735_RAMWR		0x2c	// Memory Write
#define ST7735_RGBSET		0x2d	// Color Setting 4k, 65k, 262k
#define ST7735_RAMRD		0x2e	// Memory Read
#define ST7735_PTLAR		0x30	// Partial Area (start/end address set)
#define ST7735_SCRLAR		0x33	// Scroll Area Set
#define ST7735_TEOFF		0x34	// Tearing Effect Line OFF
#define ST7735_TEON			0x35	// Tearing Effect Line ON
#define ST7735_MADCTL		0x36	// Memory Data Access Control
#define ST7735_VSCSAD		0x37	// Vertical Scroll Start Address of RAM
#define ST7735_IDMOFF		0x38	// Idle Mode Off
#define ST7735_IDMON		0x39	// Idle Mode On
#define ST7735_COLMOD		0x3a	// Interface Pixel Format + 1 byte (011 - RGB444, 101 - RGB565, 110 - RGB666)
#define ST7735_RDID1		0xda	// Read ID1 Value
#define ST7735_RDID2		0xdb	// Read ID2 Value
#define ST7735_RDID3		0xdc	// Read ID3 Value

// дисплея
#define ST7735_FRMCTR1		0xb1	// Frame Rate Control in normal mode, full colors
#define ST7735_FRMCTR2		0xb2	// Frame Rate Control in idle mode, 8 colors
#define ST7735_FRMCTR3		0xb3	// Frame Rate Control in partial mode, full colors
#define ST7735_INVCTR		0xb4	// Display Inversion Control
#define ST7735_PWCTR1		0xc0	// Power Control 1
#define ST7735_PWCTR2		0xc1	// Power Control 2
#define ST7735_PWCTR3		0xc2	// Power Control 3 in normal mode, full colors
#define ST7735_PWCTR4		0xc3	// Power Control 4 in idle mode 8colors
#define ST7735_PWCTR5		0xc4	// Power Control 5 in partial mode, full colors
#define ST7735_VMCTR1		0xc5	// VCOM Control 1
#define ST7735_VMOFCTR		0xc7	// VCOM Offset Control
#define ST7735_WRID2		0xd1	// Write ID2 Value (Set LCM version code)
#define ST7735_WRID3		0xd2	// Write ID3 Value (Set customer project code)
#define ST7735_NVFCTR1		0xd9	// NVM Control Status
#define ST7735_NVFCTR2		0xde	// NVM Read Command
#define ST7735_NVFCTR3		0xdf	// NVM Write Command
#define ST7735_GMCTRP1		0xe0	// Gamma '+'Polarity Correction Characteristics Setting
#define ST7735_GMCTRN1		0xe1	// Gamma '-'Polarity Correction Characteristics Setting
#define ST7735_GCV			0xfc	// Gate Pump Clock Frequency Variable

//                          76543210
#ifdef FLIP_RGB_BGR
  #define ST7735_MADCTL_RGB 0b00000000
  #define ST7735_MADCTL_BGR 0b00001000
#else
  #define ST7735_MADCTL_RGB 0b00001000
  #define ST7735_MADCTL_BGR 0b00000000
#endif

#define ST7735_MADCTL_LR  0b00000100  // Left to Right
#define ST7735_MADCTL_RL  0b00000000  // Right to Left
#define ST7735_MADCTL_TB  0b00000000  // Top to Borttom
#define ST7735_MADCTL_BT  0b00010000  // Bottom tp Top


#define ST7735_COLMOD_RGB444  0b00000011
#define ST7735_COLMOD_RGB565  0b00000101
#define ST7735_COLMOD_RGB666  0b00000110

class DispST7735 {
	public:
		DispST7735();
		void			init(uint16_t mSceneBg);                                                       // Инициализация
    void      setSize(uint8_t w, uint8_t h);
    uint8_t   getWidth();
    uint8_t   getHeight();
    void      setDispMode(uint8_t mode);
  
		void			setRotation(uint8_t m);
		void			setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
		void			drawPixel(uint8_t x, uint8_t y, uint16_t color);

		void			clearScreen();
		void			setBgColor(uint16_t color);
		void			restoreBgColor();

		void			fillScreen(uint16_t color);
		void			fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
		void			drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);

		void			drawImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t img[]);

		void			drawFont6Char(uint8_t x, uint8_t y, char ch, uint16_t color);
		void			drawFont6String(uint8_t x, uint8_t y, String str, uint16_t color);

		void			drawFontChar8P(uint8_t x, uint8_t y, uint8_t w, char ch, uint16_t color);
		void			drawFontString8P(uint8_t x, uint8_t y, String str, uint16_t color);
		void			drawFontCenter8P(uint8_t x, uint8_t y, uint8_t w, String str, uint16_t color);
		void			drawFontRight8P(uint8_t x, uint8_t y, uint8_t w, String str, uint16_t color);

		void			drawFontChar16P(uint8_t x, uint8_t y, uint8_t w, char ch, uint16_t color);
		void			drawFontString16P(uint8_t x, uint8_t y, String str, uint16_t color);

		void			delayMs(uint32_t d);								// Задержка в милисекундах

	private:
		void			displayOn();
		void			displayOff();

		void			sendCommand(uint8_t sCmd);							// Отправить команду дисплею
		void			sendData(uint8_t sData);							// Отправить байт данных дисплею
    void			sendData16(uint16_t sData);							// Отправить 2 байта данных дисплею
    void			sendDataPacket(void *sData, size_t sDataLen);
		void			sendDataPacketC(const uint8_t *sData, size_t sDataLen);

		// unsigned short	displayMemory[DISP_HEIGHT][DISP_WIDTH];				// Память дисплея (2 байта на точку)
		SPISettings		spiSettings;

		SPIClass		*hspi;

		uint8_t			_width;
		uint8_t			_height;
		uint8_t			_offset_x;
		uint8_t			_offset_y;

    uint16_t	  _mSceneBg;
		uint16_t		_bgColor;

		bool			_textNewLine;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
