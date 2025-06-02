#include "sio.h"
#include "sio.h"
#include "config.h"
#include "imgTypes.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SIO — основной блок прёма/передачи данных с ATARI
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SIO::SIO() {}

#ifdef ENABLE_WIFI
void SIO::init(DispST7735 *disp, Net *n, uint16_t mColorNum, uint16_t mColorBg, uint16_t mColorSecR, uint16_t mColorSecW, uint16_t mColorSecS, uint16_t mColorSecF, uint16_t mColorDP, uint16_t mColorCI, uint16_t mColorCD, uint16_t mColorNR, uint16_t mColorD) {
	_disp = disp;
	_n = n;

	_mColorNum = mColorNum;
	_mColorBg = mColorBg;

	_mColorBg = mColorBg;
	_mColorSecR = mColorSecR;
	_mColorSecW = mColorSecW;
	_mColorSecS = mColorSecS;
	_mColorSecF = mColorSecF;
	_mColorDP = mColorDP;
	_mColorCI = mColorCI;
	_mColorCD = mColorCD;
	_mColorNR = mColorNR;
	_mColorD = mColorD;

	// Инициализация виртуального дисковода D1
	vd1.init(DEV_D1);
	// Инициализация виртуального дисковода D2
	vd2.init(DEV_D2);
	// Инициализация виртуального дисковода D3
	vd3.init(DEV_D3);
	// Инициализация виртуального дисковода D4
	vd4.init(DEV_D4);

	// Установить пин Atari Cmd на вход (приём данных)
	pinMode(ATARI_CMD_PIN, INPUT);

	// Начальная инициализация SIO
	// Нельзя на данном этапе вызывать changeState, не готова консоль
	m_sioState = SIO_READY;

	// Инициализация контрольной сумма буфера
	m_cmdBufferCrc = 0;

	// Инициализация времени последнего попринятого байта
	m_timeReceivedByte = 0;

	// Инициализация сектора
	m_dataBuffer[0] = 0;
	m_dataBufferPtr = m_dataBuffer;

	_isDataSending = false;

	_lastStatusMsg = "";
}

#else

void SIO::init(DispST7735 *disp, uint16_t mColorNum, uint16_t mColorBg, uint16_t mColorSecR, uint16_t mColorSecW, uint16_t mColorSecS, uint16_t mColorSecF, uint16_t mColorDP, uint16_t mColorCI, uint16_t mColorCD, uint16_t mColorNR, uint16_t mColorD) {

	_disp = disp;

	_mColorNum = mColorNum;
	_mColorBg = mColorBg;

	_mColorBg = mColorBg;
	_mColorSecR = mColorSecR;
	_mColorSecW = mColorSecW;
	_mColorSecS = mColorSecS;
	_mColorSecF = mColorSecF;
	_mColorDP = mColorDP;
	_mColorCI = mColorCI;
	_mColorCD = mColorCD;
	_mColorNR = mColorNR;
	_mColorD = mColorD;

	// Инициализация виртуального дисковода D1
	vd1.init(DEV_D1);
	// Инициализация виртуального дисковода D2
	vd2.init(DEV_D2);
	// Инициализация виртуального дисковода D3
	vd3.init(DEV_D3);
	// Инициализация виртуального дисковода D4
	vd4.init(DEV_D4);

	// Установить пин Atari Cmd на вход (приём данных)
	pinMode(ATARI_CMD_PIN, INPUT);

	// Начальная инициализация SIO
	// Нельзя на данном этапе вызывать changeState, не готова консоль
	m_sioState = SIO_READY;

	// Инициализация контрольной сумма буфера
	m_cmdBufferCrc = 0;

	// Инициализация времени последнего попринятого байта
	m_timeReceivedByte = 0;

	// Инициализация сектора
	m_dataBuffer[0] = 0;
	m_dataBufferPtr = m_dataBuffer;

	_isDataSending = false;

	_lastStatusMsg = "";
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::setSize(uint8_t w, uint8_t h) {
	_width  = w;
	_height = h;

	_xPos = (w - 128) / 2;
	_yPos = (h - 128) / 2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SIO::isBusy() {
	return _isDataSending;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
vDrive* SIO::getVDriveById(byte devId) {
	switch (devId) {
		case DEV_D2: {
			return &vd2;
		}
		case DEV_D3: {
			return &vd3;
		}
		case DEV_D4: {
			return &vd4;
		}
	}
	return &vd1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte SIO::getFileType(String pFileName) {
	byte type = IMG_TYPE_NONE;

	String fileName = String(pFileName);
	unsigned short lfn = fileName.length();
	if (lfn > 0) {
		String fileExt = "";
		for (unsigned short i=lfn-1; i>0; i--) {
			char c = fileName.charAt(i);
			if ( c == '.') break;
			fileExt = c + fileExt;
		}

		fileExt.toUpperCase();

#ifdef DEBUG
		LOG.println("file name = \"" + fileName + "\"");
		LOG.println("file ext = \"" + fileExt + "\"");
#endif

		if (fileExt == "ATR") {
			type = IMG_TYPE_ATR;

		} else if (fileExt == "XFD") {
			type = IMG_TYPE_XFD;

		} else if (fileExt == "DCM") {
			type = IMG_TYPE_DCM;

		} else if (fileExt == "SCP") {
			type = IMG_TYPE_SCP;

		} else if (fileExt == "PRO") {
			type = IMG_TYPE_PRO;

		} else if (fileExt == "XEX") {
			type = IMG_TYPE_XEX;

    } else if (fileExt == "ATX") {
			type = IMG_TYPE_ATX;

		}
	}
	return type;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::update() {
	// Обработка статуса SIO
	switch (m_sioState) {
		// SIO готова к приёму команд
		case SIO_READY: {
			checkCmdPinHigh();
			break;
		}		
		// Ожидание начала SIO команды
		case SIO_WAIT_CMD_START: {
			checkCmdPinLow();
			break;
		}
		// Чтение SIO команды
		case SIO_READ_CMD: {
			checkReadCmdComplete();
			break;
		}
		// Чтение SIO данных
		case SIO_READ_DATA: {
			checkReadDataComplete();
			break;
		}

		// Ожидание завершения SIO команды
		case SIO_WAIT_CMD_END: {
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::changeState(byte state) {
	m_sioState = state;
#ifdef DEBUG
	LOG.print("State: ");
	LOG.println(sioStateMsg[m_sioState]);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::checkCmdPinHigh() {
	if (digitalRead(ATARI_CMD_PIN) == HIGH) {
		changeState(SIO_WAIT_CMD_START);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::checkCmdPinLow() {
	if (digitalRead(ATARI_CMD_PIN) == LOW) {    
		delay(ATARI_CMD_DELAY);
		clearCmdBuffer();
		changeState(SIO_READ_CMD);
	} 
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::clearCmdBuffer() {
	// Очистка буфера команд перед приёмом новой
	memset(&m_cmdBuffer, 0x00, sizeof(m_cmdBuffer));
	m_cmdBufferPtr = (byte*)&m_cmdBuffer;
	// Контрольная сумма буфера в 0
	m_cmdBufferCrc = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::checkReadDataComplete() { 
	if (m_dataBufferPtr - (byte*)&m_dataBuffer == m_currentSectorSize+1) {

#ifdef DEBUG
		LOG.print("<Send ACK: $");
		LOG.println(SEND_ACK, HEX);
#endif

		// Данные получены
		delay(DELAY_ACK);
		ATARI_SIO.write(SEND_ACK);

		// Расчёт CRC нового пакета
		calcDataCrc();

		byte receivedCrc = m_dataBuffer[m_currentSectorSize];

#ifdef DEBUG
		LOG.print("receivedCrc=");
		LOG.println(receivedCrc, HEX);
		LOG.print("m_dataBufferCrc=");
		LOG.println(m_dataBufferCrc, HEX);
#endif

		if (m_dataBufferCrc == receivedCrc) {
			// Приём завершён
			delay(DELAY_COMPLETE);
			ATARI_SIO.write(SEND_COMPLETE);

			if (m_cmdBuffer.devId >= DEV_D1 && m_cmdBuffer.devId <= DEV_DO) {
				//TODO: SAVE
				vDrive* workDrive = getVDriveById(m_cmdBuffer.devId);

				// Первые 3 сектора не зависимо от размеров всего 128б это бут область!
				m_currentSectorSize = 128;
				if (m_currentSector > 3) m_currentSectorSize = workDrive->getSectorSize();
				workDrive->setSectorData(m_currentSector, m_dataBuffer);	
			}
		
			// !!! Обязательно !!!
			ATARI_SIO.flush();
	
			changeState(SIO_WAIT_CMD_START);
	
		} else {
			// Если ошибка контрольной суммы, то повторить запрос
			ATARI_SIO.write(SEND_NACK);
			changeState(SIO_READY);
		}
	
		// Иначе по истечению таймаута сбросить данные и ждать команду
	} else if (millis() - m_timeReceivedByte > READ_CMD_TIMEOUT) {
		clearCmdBuffer();
		changeState(SIO_READY);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::checkReadCmdComplete() { 
	// Если прилетели все 5 байт, пакет данных команды завершен
	if (m_cmdBufferPtr - (byte*)&m_cmdBuffer == 5) {    
		// Расчёт CRC нового пакета
		calcCmdDataCrc();

		if (m_cmdBufferCrc == m_cmdBuffer.crc) {
			changeState(SIO_WAIT_CMD_START);

			if ((m_cmdBuffer.devId >= DEV_D1 && m_cmdBuffer.devId <= DEV_DO) || (m_cmdBuffer.devId == DEV_BUS))  {
				cmdProcessing();

#ifdef ENABLE_WIFI
			} else if (m_cmdBuffer.devId == DEV_RTC && _n->isTimeInited()) {
				rCmdProcessing();
#endif
			}

		} else {
			// Если ошибка контрольной суммы, то повторить запрос
			ATARI_SIO.write(SEND_NACK);
			changeState(SIO_READY);
		} 
	
	// Иначе по истечению таймаута сбросить данные и ждать команду
	} else if (millis() - m_timeReceivedByte > READ_CMD_TIMEOUT) {
		clearCmdBuffer();
		changeState(SIO_READY);
	} 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::incomingByte() {
	byte b = ATARI_SIO.read();
	m_timeReceivedByte = millis();

	switch (m_sioState) {
		case SIO_READY: {
			break;
		}
		case SIO_WAIT_CMD_START: {
			break;
		}
		case SIO_WAIT_CMD_END: {
			break;
		}
		case SIO_READ_CMD: {
			*m_cmdBufferPtr = b;
			m_cmdBufferPtr++;
			break;
		}
		case SIO_READ_DATA: {
			*m_dataBufferPtr = b;
			m_dataBufferPtr++;
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::calcDataCrc() {
	m_dataBufferCrc = calcCrc((byte*)&m_dataBuffer, m_currentSectorSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::calcCmdDataCrc() {
	m_cmdBufferCrc = calcCrc((byte*)&m_cmdBuffer, 4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte SIO::calcCrc(byte* chunk, int len) {
	unsigned short crc = 0;
	for(int i=0; i < len; i++) {
		crc = ((crc + chunk[i])>>8) + ((crc + chunk[i]) & 0xff);
	}
	return (byte)crc;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::cmdProcessing() {
	switch (m_cmdBuffer.cmdId) {
		case SIO_CMD_READ_SECTOR: {
			sendDevSector();
			break;
		}
		case SIO_CMD_WRITE_SECTOR_WITH_VERIFY: {
#ifdef DEBUG
			LOG.println("---------------------- SIO_CMD_WRITE_SECTOR_WITH_VERIFY ----------------------");
#endif
			drawStatus("WF", _mColorSecW);
			recieveDevSector();
			break;
		}
		case SIO_CMD_WRITE_SECTOR: {
#ifdef DEBUG
			LOG.println("---------------------- SIO_CMD_WRITE_SECTOR ----------------------");
#endif
			drawStatus("WS", _mColorSecW);
			recieveDevSector();
			break;
		}
		case SIO_CMD_GET_STATUS: {
#ifdef DEBUG
			LOG.println("---------------------- SIO_CMD_GET_STATUS ----------------------");
#endif
			drawStatus("GS", _mColorSecS);
			sendDevStatus();
			break;
		}
		case SIO_CMD_FORMAT_DRIVE: {
#ifdef DEBUG
			LOG.println("---------------------- SIO_CMD_FORMAT_DRIVE ----------------------");
#endif
			drawStatus("FD", _mColorSecF);
			formatDev();
			break;
		}
		case SIO_CMD_FORMAT_MEDIUM_DENSITY: {
#ifdef DEBUG
			LOG.println("---------------------- SIO_CMD_FORMAT_MEDIUM_DENSITY ----------------------");
#endif
			drawStatus("FM", _mColorSecF);
			//TODO:
#ifdef DEBUG
			LOG.println("!!! FORMAT DRIVE NOT IMPLEMENTED YET !!! ");
#endif
			break;
		}
		case SIO_CMD_DEVPOOL: {
#ifdef DEBUG
			LOG.println("---------------------- SIO_CMD_DEVPOOL ----------------------");
#endif
			drawStatus("DP", _mColorDP);
			sendDevPool();
			break;
		}
		case SIO_CMD_CHUNK_INFO: {
#ifdef DEBUG
			LOG.println("---------------------- SIO_CMD_CHUNK_INFO ----------------------");
#endif
			drawStatus("CI", _mColorCI);
			sendDevChunkInfo();
			break;
		}
		case SIO_CMD_CHUNK_DATA: {
#ifdef DEBUG
			LOG.println("---------------------- SIO_CMD_CHUNK_DATA ----------------------");
#endif
			drawStatus("CD", _mColorCD);
			sendDevChunkData();
			break;
		}
		case SIO_CMD_NOTIFY_RUN: {
#ifdef DEBUG
			LOG.println("---------------------- SIO_CMD_NOTIFY_RUN ----------------------");
#endif
			drawStatus("NR", _mColorNR);
			sendNotifyRun();
			break;
		}
		default: {
			drawStatus("oO", _mColorD);
			break;
		}
	} 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::rCmdProcessing() {
	switch (m_cmdBuffer.cmdId) {
#ifdef ENABLE_WIFI
		case APE_GET_TIMEDATE: {
			sendApeDateTime();
			break;
		}
		case APE_SUBMIT_URL: {
			//TODO:
#ifdef DEBUG
			LOG.println("!!! APE SUBMIT URL NOT IMPLEMENTED YET !!! ");
#endif
			break;
		}
#endif
		default: {
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ENABLE_WIFI

void SIO::sendApeDateTime() {

#ifdef DEBUG
	LOG.print("<Send ACK: $");
	LOG.println(SEND_ACK, HEX);
#endif

	// Данные получены
	delay(DELAY_ACK);
	ATARI_SIO.write(SEND_ACK);

	NetDateTime* dt = _n->getDateTime();
	sendBytes(6, (byte*)dt, false);

	// !!! Обязательно !!!
	ATARI_SIO.flush();
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::formatDev() {
	vDrive* workDrive = getVDriveById(m_cmdBuffer.devId);
	byte mt = workDrive->getMountedImgType();
	
	// Если тип примонтированного образа не поддерживается, то молчать и не выдавать своё присутствие
	if ( mt != IMG_TYPE_NONE ) {
#ifdef DEBUG
		LOG.println("!!! FORMAT DRIVE NOT IMPLEMENTED YET !!! ");
#endif
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::sendDevStatus() {
	vDrive* workDrive = getVDriveById(m_cmdBuffer.devId);
	byte mt = workDrive->getMountedImgType();

	// Если тип примонтированного образа не поддерживается, то молчать и не выдавать своё присутствие
	if ( mt != IMG_TYPE_NONE ) {
#ifdef DEBUG
		LOG.print("Dev ID: 0x");
		LOG.println(m_cmdBuffer.devId, HEX);
		LOG.print("Mount type: 0x");
		LOG.println(mt, HEX);

		LOG.print("<Send ACK: $");
		LOG.println(SEND_ACK, HEX);
#endif
	
		// Данные получены
		delay(DELAY_ACK);
		ATARI_SIO.write(SEND_ACK);

#ifdef DEBUG
		LOG.print("<Send COMPLETE: $");
		LOG.println(SEND_COMPLETE, HEX);
#endif

		// Приём завершён
		delay(DELAY_COMPLETE);
		ATARI_SIO.write(SEND_COMPLETE);

		StatusFrame* m_statusFrame = workDrive->getStatusFrame();
		byte frameLength = sizeof(m_statusFrame);

		byte* b = (byte*)m_statusFrame;
		for (int i=0; i < frameLength; i++) {
			ATARI_SIO.write(*b);
			b++;
		}

		byte crc = calcCrc((byte*)m_statusFrame, frameLength);
		ATARI_SIO.write(crc);
	
		// !!! Обязательно !!!
		ATARI_SIO.flush();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::sendDevSector() {
	vDrive* workDrive = getVDriveById(m_cmdBuffer.devId);
	byte mt = workDrive->getMountedImgType();

	// Если тип примонтированного образа не поддерживается, то молчать и не выдавать своё присутствие
	if ( mt != IMG_TYPE_NONE ) {
   		_isDataSending = true;

		m_currentSector = m_cmdBuffer.aByte2*256 + m_cmdBuffer.aByte1;

#ifdef DEBUG
		LOG.println("---------------------- SIO_CMD_READ_SECTOR " + String(m_currentSector) + "----------------------");
		LOG.print("<Send ACK: $");
		LOG.println(SEND_ACK, HEX);
#endif

		// Данные получены
		delay(DELAY_ACK);
		ATARI_SIO.write(SEND_ACK);

		// Первые 3 сектора не зависимо от размеров всего 128б это бут область!
		m_currentSectorSize = 128;
		if (m_currentSector > 3) {
			m_currentSectorSize = workDrive->getSectorSize();
		}

		// Обновление индикации
		drawStatus("RS", _mColorSecR);
		drawPos(m_currentSector);
    
		byte* sectorData = workDrive->getSectorData(m_currentSector);

		if (workDrive->getMountedImgType() == IMG_TYPE_PRO || workDrive->getMountedImgType() == IMG_TYPE_ATX) {

			StatusFrame* sf = workDrive->getStatusFrame();   
			if (sf->ctrlStatus.crcError == 0 || sf->ctrlStatus.recordNotFound == 0) {	// 0xF7 или 0xEF
				// «Симуляция» ошибки чтения диска
				sendBytes(m_currentSectorSize, sectorData, true);
			} else {
				sendBytes(m_currentSectorSize, sectorData, false);
			}
		} else {
			sendBytes(m_currentSectorSize, sectorData, false);
		}

		_isDataSending = false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::recieveDevSector() {
	vDrive* workDrive = getVDriveById(m_cmdBuffer.devId);
	byte mt = workDrive->getMountedImgType();

	// Если тип примонтированного образа не поддерживается, то молчать и не выдавать своё присутствие
	if ( mt != IMG_TYPE_NONE ) {

#ifdef DEBUG
		LOG.print("<Send ACK: $");
		LOG.println(SEND_ACK, HEX);
#endif

		// Данные получены
		delay(DELAY_ACK);
		ATARI_SIO.write(SEND_ACK);

		m_currentSector = m_cmdBuffer.aByte2*256 + m_cmdBuffer.aByte1;

		// Обновление индикации
		drawPos(m_currentSector);

#ifdef DEBUG
		LOG.print("Current Sector:");
		LOG.println(m_currentSector);
#endif

		memset(m_dataBuffer, 0x00, sizeof(m_dataBuffer));
		m_dataBufferPtr = m_dataBuffer;

		changeState(SIO_READ_DATA);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::sendBytes(unsigned short sectorSize, byte* pSectorData, bool isError) {
#ifdef DEBUG
	LOG.println("---------------------- Send bytes ----------------------");
#endif

	if (isError) {

#ifdef DEBUG
		LOG.print("<Send Error: $");
		LOG.println(SEND_ERR, HEX);
#endif
	
		// Симуляция ошибки данных
		delay(DELAY_ERR);
		ATARI_SIO.write(SEND_ERR);
	
  	} else {

#ifdef DEBUG
		LOG.print("<Send Complete: $");
		LOG.println(SEND_COMPLETE, HEX);
#endif
	
		// Приём завершён
		delay(DELAY_COMPLETE);
		ATARI_SIO.write(SEND_COMPLETE);
	}

	byte* b = pSectorData;
	for (int i=0; i < sectorSize; i++) {
		ATARI_SIO.write(*b);  
		b++;
	}

	byte crc = calcCrc(pSectorData, sectorSize);
	ATARI_SIO.write(crc);

	// !!! Обязательно !!!
	ATARI_SIO.flush();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::drawPos(unsigned short pos) {

	String sPos = String(pos);
	if (pos < 1000) {
		sPos = "0" + sPos;
	}
	if (pos < 100) {
		sPos = "0" + sPos;
	}
	if (pos < 10) {
		sPos = "0" + sPos;
	}
	_disp->setBgColor(_mColorBg);
	_disp->drawFont6String(_width -2-(4*6), 120, sPos, _mColorNum);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::drawStatus(String msg, uint16_t color) {
	vDrive* workDrive = getVDriveById(m_cmdBuffer.devId);
	byte mt = workDrive->getMountedImgType();

	// Если тип примонтированного образа не поддерживается, то молчать и не выдавать своё присутствие
	if ( mt != IMG_TYPE_NONE ) {
		if (_lastStatusMsg != msg) {
			_lastStatusMsg = msg;
			_disp->setBgColor(_mColorBg);
			_disp->drawFont6String(_width -2-(6*6)-3, 120, msg, color);

		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::sendDevPool() {

#ifdef DEBUG
	LOG.print("Command for device: $");
	LOG.print(m_cmdBuffer.aByte1, HEX);
	LOG.print(", $" );
	LOG.print(m_cmdBuffer.aByte2, HEX);
	LOG.print(", (\"" );
	
	if (m_cmdBuffer.aByte1 == 0x4f && m_cmdBuffer.aByte2 == 0x4f) {
		// Ничего не надо слать, это тупо безответная команада «Device Reset»
		LOG.print("Poll Reset");

	} else if (m_cmdBuffer.aByte1 == 0x00 && m_cmdBuffer.aByte2 == 0x00) {
		// Тут надо что-то отвечать и дальше работать с командами
		LOG.print("Main poll command");

	} else if (m_cmdBuffer.aByte1 == 0x4e && m_cmdBuffer.aByte2 == 0x4e) {
		// ???
		LOG.print("Null Poll");

	}
	LOG.println("\")" );
#endif

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::sendDevChunkInfo() {
	vDrive* workDrive = getVDriveById(m_cmdBuffer.devId);
	byte mt = workDrive->getMountedImgType();

	// Если тип примонтированного образа не поддерживается, то молчать и не выдавать своё присутствие
	if ( mt != IMG_TYPE_NONE ) {
		_isDataSending = true;

#ifdef DEBUG
		LOG.print("<Send ACK: $");
		LOG.println(SEND_ACK, HEX);
#endif

		// Данные получены
		delay(DELAY_ACK);
		ATARI_SIO.write(SEND_ACK);

		unsigned short m_currentChunk = m_cmdBuffer.aByte2*256 + m_cmdBuffer.aByte1;

#ifdef DEBUG
		LOG.print("Get chunk info for:" );
		LOG.println(m_currentChunk);
#endif

		vDrive* workDrive = getVDriveById(m_cmdBuffer.devId);
    
		Diskette d = workDrive->getRecordInfo(m_currentChunk);
		unsigned int recCount = workDrive->getDiskRecCount();	// Количество чанков всего

#ifdef DEBUG
		LOG.print("Chunk count=");
		LOG.println(recCount);
#endif

		byte chunkInfo[6];
		chunkInfo[0] = d.loadAddr % 256;			// +0x00: 2 байта: Адрес загрузки чанка
		chunkInfo[1] = d.loadAddr / 256;			// +0x02: 1 байт: Всегда 1 (нафига?)
		chunkInfo[2] = 1;
		chunkInfo[3] = (recCount != m_currentChunk + 1);	// +0x03: 1 байт: Если последний блок (чанк) из передаваемых, то 0
									//		  В противном случае всегда 1. Нужно для игнора RUN-блоков,
									//		  которые грузятся в адреса 0x02e0+
		chunkInfo[4] = d.loadSize % 256;			// +0x04: 2 байта: Размер загружаемого чанка
		chunkInfo[5] = d.loadSize / 256;
    
		byte frameLength = sizeof(chunkInfo);
		byte crc = calcCrc((byte*)&chunkInfo, frameLength);

		// Приём завершён
		delay(DELAY_COMPLETE);
		ATARI_SIO.write(SEND_COMPLETE);

		byte* b = (byte*)&chunkInfo;
		for (int i=0; i < frameLength; i++) {
			ATARI_SIO.write(*b);
			b++;
		}
		ATARI_SIO.write(crc);

		// !!! Обязательно !!!
		ATARI_SIO.flush();

		_isDataSending = false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::sendDevChunkData() {
	vDrive* workDrive = getVDriveById(m_cmdBuffer.devId);
	byte mt = workDrive->getMountedImgType();

	// Если тип примонтированного образа не поддерживается, то молчать и не выдавать своё присутствие
	if ( mt != IMG_TYPE_NONE ) {
		_isDataSending = true;

#ifdef DEBUG
		LOG.print("<Send ACK: $");
		LOG.println(SEND_ACK, HEX);
#endif

		// Данные получены
		delay(DELAY_ACK);
		ATARI_SIO.write(SEND_ACK);

		unsigned short m_currentChunk = m_cmdBuffer.aByte2*256 + m_cmdBuffer.aByte1;
		// Обновление индикации
		drawPos(m_currentChunk);


#ifdef DEBUG
		LOG.print("Get chunk data for:" );
		LOG.println(m_currentChunk);
#endif

		vDrive* workDrive = getVDriveById(m_cmdBuffer.devId);
		Diskette d = workDrive->getRecordInfo(m_currentChunk);

		byte* chunkData = workDrive->getRecordData(m_currentChunk);
		sendBytes(d.loadSize, chunkData, false);

		_isDataSending = false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::sendNotifyRun() {
	vDrive* workDrive = getVDriveById(m_cmdBuffer.devId);
	byte mt = workDrive->getMountedImgType();

	// Если тип примонтированного образа не поддерживается, то молчать и не выдавать своё присутствие
	if ( mt != IMG_TYPE_NONE ) {

#ifdef DEBUG
	LOG.print("<Send ACK: $");
	LOG.println(SEND_ACK, HEX);
#endif

	// Данные получены
	delay(DELAY_ACK);
	ATARI_SIO.write(SEND_ACK);

#ifdef DEBUG
	LOG.println("ATARI Run code notification");
#endif

	// Приём завершён
	delay(DELAY_COMPLETE);
	ATARI_SIO.write(SEND_COMPLETE);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SIO::unMountImageD1() {
#ifdef DEBUG
	LOG.println("UnMount Drive 1");
#endif
	vd1.unMount();
}

void SIO::unMountImageD2() {
#ifdef DEBUG
	LOG.println("UnMount Drive 2");
#endif
	vd2.unMount();
}

void SIO::unMountImageD3() {
#ifdef DEBUG
	LOG.println("UnMount Drive 3");
#endif
	vd3.unMount();
}

void SIO::unMountImageD4() {
#ifdef DEBUG
	LOG.println("UnMount Drive 4");
#endif
	vd4.unMount();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SIO::mountImageD1(String filePath) {
	byte fileType = getFileType(filePath);
	bool isMounted = false;
	if (fileType == IMG_TYPE_XEX) {
		isMounted = vd1.mountXex(filePath);
	
	} else if (
		fileType == IMG_TYPE_ATR ||
		fileType == IMG_TYPE_DCM ||
		fileType == IMG_TYPE_PRO ||
		fileType == IMG_TYPE_SCP ||
		fileType == IMG_TYPE_XFD ||
		fileType == IMG_TYPE_ATX
	) {
		isMounted = vd1.mountImage(filePath, fileType);
	} else {
		//TODO: ERR WIN!

#ifdef DEBUG
		LOG.println("!!! Error! Unsupported image type !!!");
#endif
	}

	return isMounted;
}

bool SIO::mountImageD2(String filePath) {
	byte fileType = getFileType(filePath);
	bool isMounted = false;
	if (fileType == IMG_TYPE_XEX) {
		isMounted = vd2.mountXex(filePath);

	} else if (
		fileType == IMG_TYPE_ATR ||
		fileType == IMG_TYPE_DCM ||
		fileType == IMG_TYPE_PRO ||
		fileType == IMG_TYPE_SCP ||
		fileType == IMG_TYPE_XFD ||
		fileType == IMG_TYPE_ATX
	) {
		isMounted = vd2.mountImage(filePath, getFileType(filePath));

	} else {
		//TODO: ERR WIN!

#ifdef DEBUG
		LOG.println("!!! Error! Unsupported image type !!!");
#endif
	}
	return isMounted;
}

bool SIO::mountImageD3(String filePath) {
	byte fileType = getFileType(filePath);
	bool isMounted = false;
	if (fileType == IMG_TYPE_XEX) {
		isMounted = vd3.mountXex(filePath);

	} else if (
		fileType == IMG_TYPE_ATR ||
		fileType == IMG_TYPE_DCM ||
		fileType == IMG_TYPE_PRO ||
		fileType == IMG_TYPE_SCP ||
		fileType == IMG_TYPE_XFD ||
		fileType == IMG_TYPE_ATX
	) {
		isMounted = vd3.mountImage(filePath, getFileType(filePath));

	} else {
		//TODO: ERR WIN!

#ifdef DEBUG
		LOG.println("!!! Error! Unsupported image type !!!");
#endif

	}
	return isMounted;
}

bool SIO::mountImageD4(String filePath) {
	byte fileType = getFileType(filePath);
	bool isMounted = false;
	if (fileType == IMG_TYPE_XEX) {
		isMounted = vd4.mountXex(filePath);

	} else if (
		fileType == IMG_TYPE_ATR ||
		fileType == IMG_TYPE_DCM ||
		fileType == IMG_TYPE_PRO ||
		fileType == IMG_TYPE_SCP ||
		fileType == IMG_TYPE_XFD ||
		fileType == IMG_TYPE_ATX
	) {
		isMounted = vd4.mountImage(filePath, getFileType(filePath));

	} else {
		//TODO: ERR WIN!

#ifdef DEBUG
		LOG.println("!!! Error! Unsupported image type !!!");
#endif
	}
	return isMounted;
}
