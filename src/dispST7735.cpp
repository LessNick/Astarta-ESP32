#include "dispST7735.h"
#include "font_6x6.h"
#include "font_8p.h"
#include "font_16p.h"

#define LORA_MISO 12
#define LORA_MOSI 13
#define LORA_CLK  14
  
#define LORA_NSS  21

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Display  ST7735 128x128 - Управление цветным дисплеем ST7735 с матрицей 128x128px
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DispST7735::DispST7735() {}

void DispST7735::init(uint16_t mSceneBg) {
	pinMode(LED_PIN,	OUTPUT);
	pinMode(SCK_PIN,	OUTPUT);
	pinMode(DC_PIN,		OUTPUT);
	pinMode(MOSI_PIN,	OUTPUT);
	pinMode(RES_PIN,	OUTPUT);
	pinMode(CS_PIN,		OUTPUT);
  
	digitalWrite(CS_PIN,	LOW);
	digitalWrite(RES_PIN,	HIGH);
	digitalWrite(LED_PIN,	HIGH);

	_width  = DISP_WIDTH;
	_height = DISP_HEIGHT;

	// _bg_color = BG_COLOR;
  _mSceneBg = mSceneBg;
  _bgColor = _mSceneBg;

	_textNewLine = false;

	hspi = new SPIClass(HSPI);

//	spiSettings = SPISettings(6000000, MSBFIRST, SPI_MODE0);
	spiSettings = SPISettings(8000000, MSBFIRST, SPI_MODE0);
	hspi->setBitOrder(MSBFIRST);
	hspi->setDataMode(SPI_MODE0);

	//hspi->begin();
  hspi->begin(SCK_PIN, -1, MOSI_PIN, -1);

	digitalWrite(CS_PIN, LOW);

	sendCommand(ST7735_SWRESET);	//программный ресет
	delayMs(120);
	
	sendCommand(ST7735_SLPOUT);		//будим после ресета
	delayMs(120);
	
	sendCommand(ST7735_DISPON);		//включаем экран
	
	sendCommand(ST7735_MADCTL);		//меняем вывод цветов с GBR на RGB
  sendData(ST7735_MADCTL_RGB | ST7735_MADCTL_LR | ST7735_MADCTL_TB);
	
	sendCommand(ST7735_COLMOD);		//меняем кодировку цветопередачи на 2-байтную (64К цветов)
  sendData(ST7735_COLMOD_RGB565);

	setRotation(ROTATE_LEFT_TOP);

	fillScreen(0);
	
}

void DispST7735::setSize(uint8_t w, uint8_t h) {
  _width  = w;
  _height = h;
}

void DispST7735::setDispMode(uint8_t mode) {
	sendCommand(ST7735_MADCTL);
  //sendData(mode | ST7735_MADCTL_LR | ST7735_MADCTL_TB);
  // Не важны дополнительные настройки, так как отдельно задаётся поворот экрана
  sendData(mode);

  sendCommand(ST7735_COLMOD);
  sendData(ST7735_COLMOD_RGB565);
}

void DispST7735::setBgColor(uint16_t color) {
	_bgColor = color;
}

void DispST7735::restoreBgColor() {
	// _bgColor = BG_COLOR;
  _bgColor = _mSceneBg;
}

void DispST7735::drawImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t img[]) {
	setAddrWindow(x, y, x+w-1, y+h-1);
	int dataSize = w * h*2;
  sendDataPacketC(img, (size_t)dataSize);
}

void DispST7735::drawFont6String(uint8_t x, uint8_t y, String str, uint16_t color) {
	for (int i=0; i<str.length(); i++) {
		char c = str.charAt(i);
		if (c < 32 || c > 127) c = '?';
		drawFont6Char(x, y, c, color);
		x += 6;	
		if (x > _width - 6) {
			if (y > _height - 6) break;
			x = 0;
			y += 6;
		}
	}
}

void DispST7735::drawFontCenter8P(uint8_t x, uint8_t y, uint8_t w, String str, uint16_t color) {
	uint8_t strLen = 0;
	for (int i=0; i<str.length(); i++) {
		char c = str.charAt(i);
		if (c < 32 || c > 127) c = '?';
		if (strLen + font_8p_s[c-32] > w) {
			break;
		} else {
			strLen += font_8p_s[c-32];
		}
	}
	
	x = x + ((w - strLen)/2);
	drawFontString8P(x, y, str, color);
}

void DispST7735::drawFontRight8P(uint8_t x, uint8_t y, uint8_t w, String str, uint16_t color) {
	uint8_t strLen = 0;
	for (int i=0; i<str.length(); i++) {
		char c = str.charAt(i);
		if (c < 32 || c > 127) c = '?';
		if (strLen + font_8p_s[c-32] > w) {
			break;
		} else {
			strLen += font_8p_s[c-32];
		}
	}
	
	x = w - x - strLen;
	drawFontString8P(x, y, str, color);
}

void DispST7735::drawFontString8P(uint8_t x, uint8_t y, String str, uint16_t color) {
	for (int i=0; i<str.length(); i++) {
		char c = str.charAt(i);
		if (c < 32 || c > 127) c = '?';
		uint8_t w = font_8p_s[c-32];
		drawFontChar8P(x, y, w, c, color);
		x += w;
		if (x > _width - 8) {
			if (_textNewLine) {
				if (y > _height - 8) break;
				x = 0;
				y += 8;
			} else {
				break;
			}
		}
	}
}

void DispST7735::drawFontChar8P(uint8_t x, uint8_t y, uint8_t w, char ch, uint16_t color) {
	setAddrWindow(x, y, x + w - 1, y + 8 - 1);
	uint8_t fg_hi = (color >> 8) & 0xFF, fg_lo = color & 0xFF;
  uint8_t bg_hi = (_bgColor >> 8) & 0xFF, bg_lo = _bgColor & 0xFF;
  uint16_t color_fg = (fg_lo * 256 + fg_hi);
  uint16_t color_bg = (bg_lo * 256 + bg_hi);

  int sDataLen = 2 * w * 8; // 2 байтцвет * ширину * высоту
  void *buf = malloc(sDataLen*2);
  int count = sDataLen;

  uint16_t *buf_ = (uint16_t *)buf;
  uint8_t xP=0;
  uint8_t *line = (uint8_t *)font_8p[ch-32];
  
  uint8_t b = 0b10000000;
  uint8_t bb = 0;
  while(count--) {
    if ((line[bb] & b) > 0) {
      *buf_ = color_fg;
    } else {
      *buf_ = color_bg;
    }
    xP++;
    b = b >> 1;
    *buf_++;
    if (xP == w) {
      bb++;
      b = 0b10000000;
      xP = 0;
    }
  }
  sendDataPacket(buf, sDataLen);
  free(buf);
}

void DispST7735::drawFont6Char(uint8_t x, uint8_t y, char ch, uint16_t color) {
	setAddrWindow(x, y, x + 5, y + 5);

  uint8_t fg_hi = (color >> 8) & 0xFF, fg_lo = color & 0xFF;
  uint8_t bg_hi = (_bgColor >> 8) & 0xFF, bg_lo = _bgColor & 0xFF;
  uint16_t color_fg = (fg_lo * 256 + fg_hi);
  uint16_t color_bg = (bg_lo * 256 + bg_hi);

  int sDataLen = 2 * 6 * 6; // 2 байтцвет * ширину * высоту
  void *buf = malloc(sDataLen*2);
  int count = sDataLen;

  uint16_t *buf_ = (uint16_t *)buf;
  uint8_t xP=0;
  uint8_t *line = (uint8_t *)font_6x6[ch-32];
  
  uint8_t b = 0b10000000;
  uint8_t bb = 0;
  while(count--) {
    if ((line[bb] & b) > 0) {
      *buf_ = color_fg;
    } else {
      *buf_ = color_bg;
    }
    xP++;
    b = b >> 1;
    *buf_++;
    if (xP == 6) {
      bb++;
      b = 0b10000000;
      xP = 0;
    }
  }
  sendDataPacket(buf, sDataLen);
  free(buf);
}

void DispST7735::drawFontChar16P(uint8_t x, uint8_t y, uint8_t w, char ch, uint16_t color) {
	uint8_t fg_hi = (color >> 8) & 0xFF, fg_lo = color & 0xFF;
  uint8_t bg_hi = (_bgColor >> 8) & 0xFF, bg_lo = _bgColor & 0xFF;
  uint16_t color_fg = (fg_lo * 256 + fg_hi);
  uint16_t color_bg = (bg_lo * 256 + bg_hi);
  
  setAddrWindow(x, y, x + w - 1, y + 16 - 1);
  int sDataLen = 2 * w * 16; // 2 байтцвет * ширину * высоту
  
  void *buf = malloc(sDataLen*2);
  int count = sDataLen;
  uint16_t *buf_ = (uint16_t *)buf;

  uint8_t xP=0;
  uint8_t *line = (uint8_t *)font_16p[ch-32];
  uint8_t b = 0b10000000;
  uint8_t bb = 0;
  while(count--) {
    if ((line[bb] & b) > 0) {
      *buf_ = color_fg;
    } else {
      *buf_ = color_bg;
    }
    xP++;
    b = b >> 1;
    *buf_++;
    if (w > 8) {
      if (xP == 8) {
        bb++;
        b = 0b10000000;
      } else if (xP == w) {
        bb++;
        b = 0b10000000;
        xP = 0;
      }
    } else {
      if (xP == w) {
        bb++;
        bb++;
        b = 0b10000000;
        xP = 0;
      }
    }

  }
  sendDataPacket(buf, sDataLen);
  free(buf);

}

void DispST7735::drawFontString16P(uint8_t x, uint8_t y, String str, uint16_t color) {
	for (int i=0; i<str.length(); i++) {
		char c = str.charAt(i);
		if (c < 32 || c > 127) c = '?';
		uint8_t w = font_16p_s[c-32];
		drawFontChar16P(x, y, w, c, color);
		x += w;
		if (x > _width - 16) {
			if (_textNewLine) {
				if (y > _height - 16) break;
				x = 0;
				y += 17;
			} else {
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DispST7735::setRotation(uint8_t rotate) {
	
	sendCommand(ST7735_MADCTL);
	switch (rotate) {
		case ROTATE_RIGHT_BOTTOM:
			sendData(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
			_width  = DISP_WIDTH;
			_height = DISP_HEIGHT;
			_offset_x = 2;
			_offset_y = 3;
			break;
		case ROTATE_LEFT_BOTTOM:
			sendData(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
			_width  = DISP_HEIGHT;
			_height = DISP_WIDTH;
			_offset_x = 3;
			_offset_y = 2;
			break;
		case ROTATE_LEFT_TOP:
			sendData(MADCTL_RGB);
			_width  = DISP_WIDTH;
			_height = DISP_HEIGHT;
			_offset_x = 2;
			_offset_y = 1;
			break;
		case ROTATE_RIGHT_TOP:
			sendData(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
			_width  = DISP_HEIGHT;
			_height = DISP_WIDTH;
			_offset_x = 1;
			_offset_y = 2;
			break;
	}
}

void DispST7735::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
	sendCommand(ST7735_CASET); // Column addr set
	sendData(0x00);
	sendData(x0 + _offset_x);
	sendData(0x00);
	sendData(x1 + _offset_x);
	
	sendCommand(ST7735_RASET); // Row addr set
	sendData(0x00);
	sendData(y0 + _offset_y);
	sendData(0x00);
	sendData(y1 + _offset_y);
	
	sendCommand(ST7735_RAMWR); // write to RAM
}

void DispST7735::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
	if((x >= _width) || (y >= _height)) return;
	
	if((x + w - 1) >= _width)  w = _width  - x;
	if((y + h - 1) >= _height) h = _height - y; 

	for (uint8_t px=x; px<x+w; px++) {
		drawPixel(px, y, color);
		drawPixel(px, y+h-1, color);
	}

	for (uint8_t py=y; py<y+h; py++) {
		drawPixel(x, py, color);
		drawPixel(x+w-1, py, color);
	}
}

void DispST7735::drawPixel(uint8_t x, uint8_t y, uint16_t color) {
	if((x < 0) ||(x >= _width + _offset_x) || (y < 0) || (y >= _height + _offset_y)) return;
	setAddrWindow(x, y, x+1, y+1);
  sendData16(color);
}

void DispST7735::displayOn() {
	digitalWrite(LED_PIN, HIGH);
}

void DispST7735::displayOff() {
	digitalWrite(LED_PIN, LOW);
}

void DispST7735::clearScreen() {
	fillRect(0, 0, _width, _height, _bgColor);
}

void DispST7735::fillScreen(uint16_t color) {
	fillRect(0, 0, _width, _height, color);
}

void DispST7735::fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
	if((x >= _width) || (y >= _height)) return;
	if((x + w - 1) >= _width)  w = _width  - x;
	if((y + h - 1) >= _height) h = _height - y;

	setAddrWindow(x, y, x+w-1, y+h-1);

	uint8_t hi = (color >> 8) & 0xFF, lo = color & 0xFF;
  uint16_t color_r = (lo * 256 + hi);

  int sDataLen = h * w * 2;
  void *buf = malloc(sDataLen);
  int count = h * w;
  uint16_t *buf_ = (uint16_t *)buf;
  while(count--) *buf_++ = color_r;

  hspi->beginTransaction(spiSettings);
	digitalWrite(DC_PIN, HIGH);
	digitalWrite(CS_PIN, LOW);

	digitalWrite(RES_PIN, HIGH);

	hspi->transfer (buf, sDataLen);

	digitalWrite(DC_PIN, HIGH);
	digitalWrite(CS_PIN, HIGH);

	hspi->endTransaction();

  free(buf);

}

////////////////////////////////////////////////////////////////

void DispST7735::delayMs(uint32_t d) {
	uint32_t m = millis();
	while (millis() - m < d) ;
}

void DispST7735::sendCommand(uint8_t sCmd) {
	hspi->beginTransaction(spiSettings);

	digitalWrite(DC_PIN, LOW);
	digitalWrite(CS_PIN, LOW);

	digitalWrite(RES_PIN, HIGH);
	 
	hspi->transfer(sCmd);

	digitalWrite(DC_PIN, HIGH);
	digitalWrite(CS_PIN, HIGH);
	
	hspi->endTransaction();
}

void DispST7735::sendData(uint8_t sData) {
	hspi->beginTransaction(spiSettings);
	digitalWrite(DC_PIN, HIGH);
	digitalWrite(CS_PIN, LOW);

	digitalWrite(RES_PIN, HIGH);

	hspi->transfer(sData);

	digitalWrite(DC_PIN, HIGH);
	digitalWrite(CS_PIN, HIGH);

	hspi->endTransaction();
}

void DispST7735::sendData16(uint16_t sData) {
	hspi->beginTransaction(spiSettings);
	digitalWrite(DC_PIN, HIGH);
	digitalWrite(CS_PIN, LOW);

	digitalWrite(RES_PIN, HIGH);

	hspi->transfer16(sData);

	digitalWrite(DC_PIN, HIGH);
	digitalWrite(CS_PIN, HIGH);

	hspi->endTransaction();
}

void DispST7735::sendDataPacketC(const uint8_t *sData, size_t sDataLen) {
  void *buf = malloc(sDataLen);
  memcpy(buf, sData, sDataLen);
  sendDataPacket(buf, sDataLen);
  free(buf);
}

void DispST7735::sendDataPacket(void *sData, size_t sDataLen) {
  hspi->beginTransaction(spiSettings);
	digitalWrite(DC_PIN, HIGH);
	digitalWrite(CS_PIN, LOW);

	digitalWrite(RES_PIN, HIGH);

	hspi->transfer (sData, sDataLen);

	digitalWrite(DC_PIN, HIGH);
	digitalWrite(CS_PIN, HIGH);

	hspi->endTransaction();
}


uint8_t DispST7735::getWidth() {
  return _width;
}

uint8_t DispST7735::getHeight() {
  return _height;
}
