#include "config.h"

#include "vRec.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Virtual Recorder - виртуальный магнитофон
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vRecorder::vRecorder() {};

void vRecorder::startPlay(DispST7735 &_disp, String filePath) {

	String sp = "     ";

	imageFile = SD.open(filePath);

	if (!imageFile) {
		//TODO: MSG ERR

#ifdef DEBUG
		LOG.println("Error opent file \"" + filePath + "\"");
#endif
	} else {

		m_imageSize = imageFile.size();
		
		unsigned int nAddr = 0;

		byte tData[8];
		while (nAddr < m_imageSize) {

			char cName[4+1];		// Название блока
			unsigned short  cSize;	// Размер блока
			unsigned short  cAux;	// Доп. данные

			imageFile.seek(nAddr);
			imageFile.read(tData, 8);
	
			memcpy(cName, tData, 4);
			cName[4] = 0x00;
		
			cSize = tData[4] + tData[5]*256;
			cAux = tData[6] + tData[7]*256;

#ifdef DEBUG
			LOG.print("Name:");
			LOG.println(cName);

			LOG.print("Size:");
			LOG.println(cSize);

			LOG.print("Aux:");
			LOG.println(cAux);
#endif 

			nAddr += 8;

			if(strcmp(cName, "FUJI") == 0) {

#ifdef DEBUG
				LOG.println("Cassette format detected!");
#endif

			} else if(strcmp(cName, "baud") == 0) {

#ifdef DEBUG
				LOG.print("Change port speed to: ");
				LOG.println(cAux);
#endif
				_disp.drawFontCenter8P(3, 51+(1*9), 128-6, sp + "Port Speed: " + String(cAux) + sp, COLOR(0,0,0));
				ATARI_SIO.begin(cAux);

			} else if(strcmp(cName, "data") == 0) {

#ifdef DEBUG
				LOG.print("Wait (GAP, ms): ");
				LOG.println(cAux);
	#endif

				if (cAux > 1000) {
					_disp.drawFontCenter8P(3, 51+(2*9), 128-6, sp + "Wait (GAP, ms): " + String(cAux) + sp, COLOR(0,0,0));
					
					int gapCount = cAux/1000;
					for (int gc=0; gc<gapCount; gc++) {
						_disp.drawFontCenter8P(3, 51+(3*9), 128-6, sp + String(gc) + " / " + String(gapCount) + sp, COLOR(0,0,0));
						delay(1000);
					}
					if (cAux > gapCount*1000) {
						delay(cAux-gapCount*1000);
					}
				
				} else {
					delay(cAux);
				}
		
				_disp.drawFontCenter8P(3, 51+(2*9), 128-6, sp + "  Loading data:  " + sp, COLOR(0,0,0));
				_disp.drawFontCenter8P(3, 51+(3*9), 128-6, sp + String(nAddr) + " / " + String(m_imageSize) + sp, COLOR(0,0,0));

				byte sData[cSize];

				imageFile.seek(nAddr);
				imageFile.read(sData, cSize);

#ifdef DEBUG
				String sendResult = "";
#endif

				byte* b = sData;
				for (int i=0; i < cSize; i++) {
	
#ifdef DEBUG
					LOG.print("Send byte: ");
					LOG.println(*b, HEX);
#endif
					ATARI_SIO.write(*b);
					b++;
				}

				byte crc = 0x00;
  
				// !!! Обязательно !!!
				ATARI_SIO.flush();
	  
#ifdef DEBUG
				LOG.print("Send data size:");
				LOG.println(cSize);
#endif
		
			}
		
			nAddr += cSize;

		}

		_disp.drawFontCenter8P(3, 51+(3*9), 128-6, sp + " Complete! " + sp, COLOR(0,0,0));
	}
};
