#include "dispST7735.h"
#include "font_6x6.h"
#include "font_8p.h"
#include "font_16p.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Display  ST7735 128x128 - Управление цветным дисплеем ST7735 с матрицей 128x128px
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DispST7735::DispST7735() {
	init();
}

void DispST7735::init() {
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

	_bg_color = BG_COLOR;

	_textNewLine = false;

	hspi = new SPIClass(HSPI);

	spiSettings = SPISettings(6000000, MSBFIRST, SPI_MODE0);
	hspi->setBitOrder(MSBFIRST);
	hspi->setDataMode(SPI_MODE0);

	hspi->begin();

	digitalWrite(CS_PIN, LOW);

	sendCommand(ST7735_SWRESET);	//программный ресет
	delayMs(120);
	
	sendCommand(ST7735_SLPOUT);		//будим после ресета
	delayMs(120);
	
	sendCommand(ST7735_DISPON);		//включаем экран
	
	sendCommand(ST7735_MADCTL);		//меняем вывод цветов с GBR на RGB
	sendData(0b00001000);
	
	sendCommand(ST7735_COLMOD);		//меняем кодировку цветопередачи на 2-байтную (64К цветов)
	sendData(0b00000101);			// 5 = 101 - RGB565

	setRotation(ROTATE_LEFT_TOP);

	fillScreen(0);
	
}

void DispST7735::setBgColor(uint16_t color) {
	_bg_color = color;
}

void DispST7735::restoreBgColor() {
	_bg_color = BG_COLOR;
}


void DispST7735::drawImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t img[]) {
	setAddrWindow(x, y, x+w-1, y+h-1);
	int dataSize = w * h*2;
	for (int i=0; i<dataSize; i +=2) {
		uint8_t lo = img[i];
		uint8_t hi = img[i+1];
		sendData(hi);
		sendData(lo);
	}
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
	uint8_t bg_hi = (_bg_color >> 8) & 0xFF, bg_lo = _bg_color & 0xFF;

	for (uint8_t k=0; k<8; k++) {
		uint8_t line = font_8p[ch-32][k];
		uint8_t b = 0b10000000;
		for (uint8_t j=0; j<w; j++) {
			if ((line & b) != 0) {
				sendData(fg_hi);
				sendData(fg_lo);
			} else {
				sendData(bg_hi);
				sendData(bg_lo);
			}

			b = b >> 1;
		}
	}
}

void DispST7735::drawFont6Char(uint8_t x, uint8_t y, char ch, uint16_t color) {
	setAddrWindow(x, y, x + 5, y + 5);
	uint8_t fg_hi = (color >> 8) & 0xFF, fg_lo = color & 0xFF;
	uint8_t bg_hi = (_bg_color >> 8) & 0xFF, bg_lo = _bg_color & 0xFF;
	
	for (uint8_t k=0; k<6; k++) {
		uint8_t line = font_6x6[ch-32][k];

		if ((line & 0b10000000) != 0) {
			sendData(fg_hi);
			sendData(fg_lo);
		} else {
			sendData(bg_hi);
			sendData(bg_lo);
		}
		
		if ((line & 0b01000000) != 0) {
			sendData(fg_hi);
			sendData(fg_lo);
		} else {
			sendData(bg_hi);
			sendData(bg_lo);
		}
		
		if ((line & 0b00100000) != 0) {
			sendData(fg_hi);
			sendData(fg_lo);
		} else {
			sendData(bg_hi);
			sendData(bg_lo);
		}
		
		if ((line & 0b00010000) != 0) {
			sendData(fg_hi);
			sendData(fg_lo);
		} else {
			sendData(bg_hi);
			sendData(bg_lo);
		}
		
		if ((line & 0b00001000) != 0) {
			sendData(fg_hi);
			sendData(fg_lo);
		} else {
			sendData(bg_hi);
			sendData(bg_lo);
		}

		if ((line & 0b00000100) != 0) {
			sendData(fg_hi);
			sendData(fg_lo);
		} else {
			sendData(bg_hi);
			sendData(bg_lo);
		}
	}
}

void DispST7735::drawFontChar16P(uint8_t x, uint8_t y, uint8_t w, char ch, uint16_t color) {
	setAddrWindow(x, y, x + w - 1, y + 16 - 1);
	uint8_t fg_hi = (color >> 8) & 0xFF, fg_lo = color & 0xFF;
	uint8_t bg_hi = (_bg_color >> 8) & 0xFF, bg_lo = _bg_color & 0xFF;

	if (w < 9) {
		for (uint8_t k=0; k<32; k+=2) {
			uint8_t line = font_16p[ch-32][k];
			uint8_t b = 0b10000000;
			for (uint8_t j=0; j<w; j++) {
				if ((line & b) != 0) {
					sendData(fg_hi);
					sendData(fg_lo);
				} else {
					sendData(bg_hi);
					sendData(bg_lo);
				}
				b = b >> 1;
			}
		}
	} else {
		for (uint8_t k=0; k<32; k++) {
			uint8_t line = font_16p[ch-32][k];
			uint8_t b = 0b10000000;
			for (uint8_t j=0; j<8; j++) {
				if ((line & b) != 0) {
					sendData(fg_hi);
					sendData(fg_lo);
				} else {
					sendData(bg_hi);
					sendData(bg_lo);
				}
				b = b >> 1;
			}
			k++;
			uint8_t line2 = font_16p[ch-32][k];
			uint8_t b2 = 0b10000000;
			for (uint8_t j=0; j<w-8; j++) {
				if ((line2 & b2) != 0) {
					sendData(fg_hi);
					sendData(fg_lo);
				} else {
					sendData(bg_hi);
					sendData(bg_lo);
				}
				b2 = b2 >> 1;
			}
		}
	}
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
	sendData((color >> 8) & 0xFF);
	sendData(color & 0xFF);
}

void DispST7735::displayOn() {
	digitalWrite(LED_PIN, HIGH);
}

void DispST7735::displayOff() {
	digitalWrite(LED_PIN, LOW);
}

void DispST7735::clearScreen() {
	fillRect(0, 0, _width, _height, _bg_color);
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

	for(y=h; y>0; y--) {
		for(x=w; x>0; x--) {
			sendData(hi);
			sendData(lo);
		}
	}

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

void DispST7735::sendDataPacket(uint8_t *sData, uint8_t sDataLen) {
	for(int i = 0; i < sDataLen; i++) {
		sendData(sData[i]);
	}
}
