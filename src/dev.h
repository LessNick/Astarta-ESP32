#ifndef DEV_h
#define DEV_h

#include <Arduino.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Список идентификаторов устройств, подключаемых к ATARI SIO
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const byte DEV_D1   = 0x31;		// Floppy Drive D1
const byte DEV_D2   = 0x32;		// Floppy Drive D2
const byte DEV_D3   = 0x33;		// Floppy Drive D3
const byte DEV_D4   = 0x34;		// Floppy Drive D4
const byte DEV_D5   = 0x35;		// Floppy Drive D5
const byte DEV_D6   = 0x36;		// Floppy Drive D6
const byte DEV_D7   = 0x37;		// Floppy Drive D7
const byte DEV_D8   = 0x38;		// Floppy Drive D8
const byte DEV_D9   = 0x39;		// Floppy Drive D9
const byte DEV_DJ   = 0x3a;		// Floppy Drive J
const byte DEV_DK   = 0x3b;		// Floppy Drive K
const byte DEV_DL   = 0x3c;		// Floppy Drive L
const byte DEV_DM   = 0x3d;		// Floppy Drive M
const byte DEV_DN   = 0x3e;		// Floppy Drive N
const byte DEV_DO   = 0x3f;		// Floppy Drive O

const byte DEV_P1   = 0x40;		// Printer P1
const byte DEV_P2   = 0x41;		// Printer P2
const byte DEV_P3   = 0x42;		// Printer P3
const byte DEV_P4   = 0x43;		// Printer P4

const byte DEV_RTC  = 0x45;		// APE Time/Date

const byte DEV_BUS  = 0x4f;		// Устройство шина

const byte DEV_R1   = 0x50;		// RS232 R1
const byte DEV_R2   = 0x51;		// RS232 R2
const byte DEV_R3   = 0x52;		// RS232 R3
const byte DEV_R4   = 0x53;		// RS232 R4

const byte DEV_C    = 0x60;		// Cassette C

const byte DEV_SDRIVE	= 0x71;		// SDrive

#endif
