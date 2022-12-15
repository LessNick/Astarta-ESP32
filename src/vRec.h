#ifndef VRECORDER_h
#define VRECORDER_h

#include <Arduino.h>
#include <SD.h>

#include "dispST7735.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class vRecorder {
	public:
		vRecorder();
		void			startPlay(DispST7735 &_disp, String filePath);

	private:
		File			imageFile;		// Объект для чтения/записи образа диска
		int				m_imageSize;	// Размер образа
};

#endif
