////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AstartA Project v2.00 pre-alpha
// Copyright 2022 © LessNick aka breeze/fishbonce crew
// https://github.com/LessNick/Astarta-ESP32
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Required: ESP32-WROOM + DISPLAY ST7735 + SD CARD & 4 BUTTONS =)
// Supported: ATR (Read/Write), XFD, PRO, XEX & CAS (read only) + APE TIME
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "config.h"
#include "scene.h"

Scene s;
void setup() {

#ifdef DEBUG
	LOG.begin(115200);
#endif

	ATARI_SIO.begin(19200);

	s.showStart();		// Start with logo
	//s.checkSD();		// Start 

}

void loop() {
	s.refresh();
}
