////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AstartA Project v2.44 pre-alpha
// Copyright 2022, 2025 © LessNick aka breeze/fishbonce crew
// https://github.com/LessNick/Astarta-ESP32
// https://codeberg.org/LessNick/Astarta-ESP32
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Required: ESP-WROOM-32 + DISPLAY ST7735 + SD CARD & 4 BUTTONS =)
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
	LOG.println("-------------------");
	LOG.println("AstartA Starting...");
	LOG.println("-------------------");

	LOG.print("Init ATARI SIO... ");
#endif

	ATARI_SIO.begin(19200);

#ifdef DEBUG
	LOG.println("[ OK ]");
	LOG.print("Init Display Size... ");
#endif
	s.setSize(128,128);   // Set display width, height

#ifdef DEBUG
	LOG.println("[ OK ]");
	LOG.print("Show Logo... ");
#endif

	s.showStart();

#ifdef DEBUG

	LOG.println("[ OK ]");

	int atariCMD = digitalRead(ATARI_CMD_PIN);
	LOG.println("Atari CMD Pin Status:" + String(atariCMD));

	int btnESC = digitalRead(BTN_PIN_ESC);
	LOG.println("Astarta ESC Pin Status:" + String(btnESC));
	int btnUP = digitalRead(BTN_PIN_UP);
	LOG.println("Astarta UP Pin Status:" + String(btnUP));
	int btnDOWN = digitalRead(BTN_PIN_DOWN);
	LOG.println("Astarta DOWN Pin Status:" + String(btnDOWN));
	int btnEnter = digitalRead(BTN_PIN_ENTER);
	LOG.println("Astarta ENTER Pin Status:" + String(btnEnter));

	LOG.print("\nWait press any key... ");
#endif

}

void loop() {
	s.refresh();
}
