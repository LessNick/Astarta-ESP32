#ifndef FONT_8P_h
#define FONT_8P_h

static uint8_t const font_8p_s[96] = {
	//   ! " # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?
		3,2,4,8,6,9,6,3,3,3,5,6,3,6,2,4,5,3,5,5,6,5,5,5,5,5,2,3,4,6,4,5,

	//  @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \ ] ^ _
		8,6,6,6,6,5,5,6,6,2,6,6,5,8,6,6,6,6,6,6,6,6,6,8,6,6,6,3,4,3,4,9,

	//  ` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~ ©
		3,5,5,4,5,5,5,6,5,2,3,5,3,6,5,5,5,5,4,5,4,5,6,6,5,5,4,4,2,4,6,9,
};

static uint8_t const font_8p[96][8] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x00 },
	{ 0x50, 0x50, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x14, 0x14, 0x7E, 0x28, 0xFC, 0x50, 0x50, 0x00 },
	{ 0x20, 0x78, 0xA0, 0x70, 0x28, 0xF0, 0x20, 0x00 },
	{ 0x40, 0xA4, 0x48, 0x10, 0x24, 0x4A, 0x04, 0x00 },
	{ 0x00, 0x60, 0x90, 0x60, 0x98, 0x90, 0x68, 0x00 },
	{ 0x00, 0x40, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00 },
	{ 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40 },
	{ 0x80, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x80 },
	{ 0x00, 0x20, 0x70, 0x20, 0x50, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x20, 0x20, 0xF8, 0x20, 0x20, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x80 },
	{ 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00 },
	{ 0x20, 0x20, 0x40, 0x40, 0x40, 0x80, 0x80, 0x00 },
	{ 0x60, 0x90, 0x90, 0x90, 0x90, 0x90, 0x60, 0x00 },
	{ 0x40, 0xC0, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00 },
	{ 0x60, 0x90, 0x10, 0x20, 0x40, 0x80, 0xF0, 0x00 },
	{ 0x60, 0x90, 0x10, 0x20, 0x10, 0x90, 0x60, 0x00 },
	{ 0x10, 0x30, 0x50, 0x50, 0x90, 0xF8, 0x10, 0x00 },
	{ 0xF0, 0x80, 0xE0, 0x10, 0x10, 0x90, 0x60, 0x00 },
	{ 0x60, 0x90, 0x80, 0xE0, 0x90, 0x90, 0x60, 0x00 },
	{ 0xF0, 0x10, 0x20, 0x20, 0x40, 0x40, 0x80, 0x00 },
	{ 0x60, 0x90, 0x90, 0x60, 0x90, 0x90, 0x60, 0x00 },
	{ 0x60, 0x90, 0x90, 0x70, 0x10, 0x90, 0x60, 0x00 },
	{ 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00 },
	{ 0x00, 0x00, 0x40, 0x00, 0x00, 0x40, 0x40, 0x80 },
	{ 0x00, 0x00, 0x20, 0x40, 0x80, 0x40, 0x20, 0x00 },
	{ 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00 },
	{ 0x00, 0x00, 0x80, 0x40, 0x20, 0x40, 0x80, 0x00 },
	{ 0x60, 0x90, 0x10, 0x20, 0x40, 0x00, 0x40, 0x00 },
	{ 0x38, 0x44, 0x9A, 0xAA, 0xB4, 0x40, 0x3C, 0x00 },
	{ 0x20, 0x20, 0x50, 0x50, 0x70, 0x88, 0x88, 0x00 },
	{ 0xE0, 0x90, 0x90, 0xF0, 0x88, 0x88, 0xF0, 0x00 },
	{ 0x70, 0x88, 0x80, 0x80, 0x80, 0x88, 0x70, 0x00 },
	{ 0xE0, 0x90, 0x88, 0x88, 0x88, 0x88, 0xF0, 0x00 },
	{ 0xF0, 0x80, 0x80, 0xE0, 0x80, 0x80, 0xF0, 0x00 },
	{ 0xF0, 0x80, 0x80, 0xE0, 0x80, 0x80, 0x80, 0x00 },
	{ 0x70, 0x88, 0x80, 0xB8, 0x88, 0x98, 0x68, 0x00 },
	{ 0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x88, 0x00 },
	{ 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00 },
	{ 0x08, 0x08, 0x08, 0x08, 0x08, 0x88, 0x70, 0x00 },
	{ 0x88, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x88, 0x00 },
	{ 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF0, 0x00 },
	{ 0x82, 0xC6, 0xAA, 0x92, 0x82, 0x82, 0x82, 0x00 },
	{ 0x88, 0x88, 0xC8, 0xA8, 0x98, 0x88, 0x88, 0x00 },
	{ 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00 },
	{ 0xF0, 0x88, 0x88, 0x88, 0xF0, 0x80, 0x80, 0x00 },
	{ 0x70, 0x88, 0x88, 0x88, 0xA8, 0x90, 0x68, 0x00 },
	{ 0xF0, 0x88, 0x88, 0x88, 0xF0, 0x88, 0x88, 0x00 },
	{ 0x70, 0x88, 0x80, 0x70, 0x08, 0x88, 0x70, 0x00 },
	{ 0xF8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00 },
	{ 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00 },
	{ 0x88, 0x88, 0x88, 0x50, 0x50, 0x20, 0x20, 0x00 },
	{ 0x82, 0x92, 0x92, 0xAA, 0x6C, 0x44, 0x44, 0x00 },
	{ 0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00 },
	{ 0x88, 0x88, 0x50, 0x20, 0x20, 0x20, 0x20, 0x00 },
	{ 0xF8, 0x08, 0x10, 0x20, 0x40, 0x80, 0xF8, 0x00 },
	{ 0xC0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xC0 },
	{ 0x80, 0x80, 0x40, 0x40, 0x40, 0x20, 0x20, 0x00 },
	{ 0xC0, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xC0 },
	{ 0x00, 0x40, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF },
	{ 0x80, 0x80, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x60, 0x10, 0x70, 0x90, 0x70, 0x00 },
	{ 0x80, 0x80, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0x00 },
	{ 0x00, 0x00, 0x60, 0x80, 0x80, 0x80, 0x60, 0x00 },
	{ 0x10, 0x10, 0x70, 0x90, 0x90, 0x90, 0x70, 0x00 },
	{ 0x00, 0x00, 0x60, 0x90, 0xF0, 0x80, 0x60, 0x00 },
	{ 0x00, 0x30, 0x40, 0xE0, 0x40, 0x40, 0x40, 0x00 },
	{ 0x00, 0x00, 0x68, 0x90, 0x90, 0x70, 0x10, 0xE0 },
	{ 0x80, 0x80, 0xE0, 0x90, 0x90, 0x90, 0x90, 0x00 },
	{ 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00 },
	{ 0x40, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x80 },
	{ 0x80, 0x80, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x00 },
	{ 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x00 },
	{ 0x00, 0x00, 0xD0, 0xA8, 0xA8, 0xA8, 0xA8, 0x00 },
	{ 0x00, 0x00, 0xA0, 0xD0, 0x90, 0x90, 0x90, 0x00 },
	{ 0x00, 0x00, 0x60, 0x90, 0x90, 0x90, 0x60, 0x00 },
	{ 0x00, 0x00, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0x80 },
	{ 0x00, 0x00, 0x70, 0x90, 0x90, 0x90, 0x70, 0x10 },
	{ 0x00, 0x00, 0xA0, 0xC0, 0x80, 0x80, 0x80, 0x00 },
	{ 0x00, 0x00, 0x70, 0x80, 0x60, 0x10, 0xE0, 0x00 },
	{ 0x00, 0x40, 0xE0, 0x40, 0x40, 0x40, 0x20, 0x00 },
	{ 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x70, 0x00 },
	{ 0x00, 0x00, 0x88, 0x88, 0x50, 0x50, 0x20, 0x00 },
	{ 0x00, 0x00, 0xA8, 0xA8, 0xA8, 0x50, 0x50, 0x00 },
	{ 0x00, 0x00, 0x90, 0x90, 0x60, 0x90, 0x90, 0x00 },
	{ 0x00, 0x00, 0x90, 0x90, 0x90, 0x70, 0x10, 0x60 },
	{ 0x00, 0x00, 0xE0, 0x20, 0x40, 0x80, 0xE0, 0x00 },
	{ 0x20, 0x40, 0x40, 0x80, 0x40, 0x40, 0x40, 0x20 },
	{ 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 },
	{ 0x80, 0x40, 0x40, 0x20, 0x40, 0x40, 0x40, 0x80 },
	{ 0x00, 0x00, 0x00, 0x68, 0xB0, 0x00, 0x00, 0x00 },
	{ 0x3C, 0x42, 0x99, 0xA5, 0xA1, 0x99, 0x42, 0x3C }
};

#endif
