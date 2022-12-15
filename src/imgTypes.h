#ifndef IMGTYPES_h
#define IMGTYPES_h

#include <Arduino.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Известные типы файлов (образы)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const byte IMG_TYPE_NONE	= 0x00;		// Тип образа NONE (Не известный формат)
const byte IMG_TYPE_ATR		= 0x01;		// Тип образа ATR (Atari Disk Image format)
const byte IMG_TYPE_XFD		= 0x02;		// Тип образа XFD (Xformer Disk Image)
const byte IMG_TYPE_DCM		= 0x03;		// Тип образа DCM (Compressed Disk Communicator image)
const byte IMG_TYPE_SCP		= 0x04;		// Тип образа SCP (Spartados Compressed Disk Image)
const byte IMG_TYPE_PRO		= 0x05;		// Тип образа PRO (Proprietary image format)
const byte IMG_TYPE_XEX		= 0x06;		// Тип образа XEX (Исполняемый файл)
const byte IMG_TYPE_CAS		= 0x07;		// Тип образа CAS (Cassette image)
const byte IMG_TYPE_ATX		= 0x08;		// Тип образа ATX (Atari Disk Image extented format)

#endif
