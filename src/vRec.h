#ifndef VRECORDER_h
#define VRECORDER_h

#include <Arduino.h>
#include <SD.h>

#include "dispST7735.h"
#include "keyboard.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class vRecorder {
	public:
		vRecorder();
		void	init(KeyBoard	*kbd, uint16_t mSceneBg, uint16_t mWinFg);
		void	setSize(uint8_t w, uint8_t h);
		void	startPlay(DispST7735 &_disp, String filePath);

		bool	playComplete();

	private:
		uint8_t		_width;
		uint8_t		_height;

		uint16_t	_mSceneBg;
		uint16_t	_mWinFg;

		uint8_t		_xPos;
		uint8_t		_yPos;
    
		File		imageFile;		// Объект для чтения/записи образа диска
		int		m_imageSize;		// Размер образа
    
		KeyBoard	*_kbd;

		bool		isComplete;
		bool		isAborted;
};

#endif
