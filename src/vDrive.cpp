#include "vDrive.h"
#include "imgTypes.h"
#include "config.h"

vDrive::vDrive() {}

void vDrive::init(byte vDriveId) {
	m_vDriveId = vDriveId;

	// Размер сектора по умолчанию 128 байт
	m_sectorSize = 128;

	// Тип по умолчанию
	m_imageType = IMG_TYPE_NONE;

	// Размер заголовка по умолчанию (для ATR 16 байт)
	m_imageHeaderSize = 0;	// 16

	// Количество секторов на треке
	m_secPerTrack = 0;

	// Текущий трек
	m_currentTrackPos = 0;
	// Текущий сектор
	m_currentSectorPos = 0;

	// Количесво сторон диска
	m_diskSideCount = 1;

	// Количество записей
	m_diskRecCount = 0;

	// Инициализация сектора
	sector[0] = 0;
	sectorPtr = sector;

	// Инициализация массива 
	m_disk[0].side = 0;
	m_disk[0].track = 0;
	m_disk[0].sector = 0;
	m_disk[0].offset = 0;
	m_disk[0].loadAddr = 0;
	m_disk[0].loadSize = 0;  

	m_diskPtr = m_disk;

	m_imageType = IMG_TYPE_NONE;

	m_fileSize = 0;
	m_imageSize = 0;

	resetDevice();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void vDrive::resetDevice() { 
	m_statusFrame.drvStatus.writeError = 0;
	m_statusFrame.drvStatus.writeProtected = 0;
	m_statusFrame.drvStatus.motorRunning = 1;   // Мотор вкл

	// Инвертированные значения
	m_statusFrame.ctrlStatus.busy = 1;
	m_statusFrame.ctrlStatus.dataPending = 1;
	m_statusFrame.ctrlStatus.lostData = 1;
	m_statusFrame.ctrlStatus.crcError = 1;
	m_statusFrame.ctrlStatus.recordNotFound = 1;
	m_statusFrame.ctrlStatus.deletedSector = 1;
	m_statusFrame.ctrlStatus.writeProtectError = 1;

	// Диска нет в приводе(инвертированное значение)
	m_statusFrame.ctrlStatus.notReady = 0;

	// Установить таймаут ожидания
	m_statusFrame.defaultTimeout = 0xE0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short vDrive::getSectorSize() {
	return m_sectorSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusFrame* vDrive::getStatusFrame() {
	if (m_imageType == IMG_TYPE_PRO) {
	// Получаем статусы привода и контроллера из доп.данных сектора
	m_statusFrame.drvStatus = proSectorData->drvStatus;
	m_statusFrame.ctrlStatus = proSectorData->ctrlStatus;
	m_statusFrame.defaultTimeout = proSectorData->timeout;      // 0xE0;

	} else {
		// Сбрасываем по умолчанию (ошибок нет, привод готов)
		resetDevice();
	}
	return &m_statusFrame;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte* vDrive::getSectorData(int sectorPos) {

	if (!imageFile) {

#ifdef DEBUG
		LOG.println("Wrogn imageFile!");
#endif

	} else {

		// Текущий сектор
		m_currentSectorPos = sectorPos;
		// Текущий трек (псевдо расчёт)
		m_currentTrackPos = m_currentSectorPos/m_secPerTrack;

#ifdef DEBUG
		LOG.print("m_currentSectorPos=");
		LOG.println(m_currentSectorPos);
		LOG.print("m_secPerTrack=");
		LOG.println(m_secPerTrack);
#endif

		// Первые 3 сектора не зависимо от размеров всего 128б это бут область!
		unsigned short sSec = 128;
		if (sectorPos > 3) {
			sSec = m_sectorSize;
		}

		int sPos = (sSec + m_sectorInfoSize) * (sectorPos - 1);

		// Если размер сектора > 128
		if (m_sectorSize > 128 && sectorPos > 3) {
			sPos = sSec * (sectorPos - 1 - 3) + 128 *3;
		}

		if (m_imageType == IMG_TYPE_XEX) {
			byte* xldr = &xexLoader[0];
			memcpy(sector, xldr+sPos, 128);

		} else {
			if (m_imageType == IMG_TYPE_PRO) {
				imageFile.seek(sPos + m_imageHeaderSize);     // skip header
				byte psd[12];
				imageFile.read(psd, 12);
				memcpy(proSectorData, psd, 12);

#ifdef DEBUG
				LOG.println("\nPRO Sector Info:");

				LOG.println("\nDrive Status:");
				LOG.println("   Write Error: " + String(proSectorData->drvStatus.writeError ? "true" : "false"));
				LOG.println("   Write Protected: " + String(proSectorData->drvStatus.writeProtected ? "true" : "false"));
				LOG.println("   Motor Running: " + String(proSectorData->drvStatus.motorRunning ? "true" : "false"));

				LOG.println("\nController Status:");
				// Инвертированные значения битов
				LOG.println("   Busy: " + String(proSectorData->ctrlStatus.busy ? "false" : "true"));
				LOG.println("   Data Pending: " + String(proSectorData->ctrlStatus.dataPending ? "false" : "true"));
				LOG.println("   Lost Data: " + String(proSectorData->ctrlStatus.lostData ? "false" : "true"));
				LOG.println("   Crc Error: " + String(proSectorData->ctrlStatus.crcError ? "false" : "true"));
				LOG.println("   Record Not Found: " + String(proSectorData->ctrlStatus.recordNotFound ? "false" : "true"));
				LOG.println("   Deleted Sector: " + String(proSectorData->ctrlStatus.deletedSector ? "false" : "true"));
				LOG.println("   Write Protect Error: " + String(proSectorData->ctrlStatus.writeProtectError ? "false" : "true"));
				LOG.println("   Not Ready: " + String(proSectorData->ctrlStatus.notReady ? "false" : "true"));

				LOG.println(" ");

				LOG.print(String(proSectorData->timeout) + ", $");
				LOG.print(proSectorData->timeout, HEX);
				LOG.println(" - Timeout (1771 controller status)");

				if (proSectorData->totalPhantomSecAss > 0) {

					LOG.print(String(proSectorData->totalPhantomSecAss) + ", $");
					LOG.print(proSectorData->totalPhantomSecAss, HEX);
					LOG.println(" - Total number of phantom sectors associated with this sector");

					if (proSectorData->idxPhantomSector1 > 0 ) {
						LOG.print(String(proSectorData->idxPhantomSector1) + ", $");
						LOG.print(proSectorData->idxPhantomSector1, HEX);
						LOG.println(" - Index of phantom sector (PHANTOM1) starting with sector 721 (1-indexed)");
					}

					if (proSectorData->idxPhantomSector2 > 0 ) {
						LOG.print(String(proSectorData->idxPhantomSector2) + ", $");
						LOG.print(proSectorData->idxPhantomSector2, HEX);
						LOG.println(" - Index of phantom sector (PHANTOM2) starting with sector 721 (1-indexed)");
					}
					
					if (proSectorData->idxPhantomSector3 > 0 ) {
						LOG.print(String(proSectorData->idxPhantomSector3) + ", $");
						LOG.print(proSectorData->idxPhantomSector3, HEX);
						LOG.println(" - Index of phantom sector (PHANTOM3) starting with sector 721 (1-indexed)");
					}
					
					if (proSectorData->idxPhantomSector4 > 0 ) {
						LOG.print(String(proSectorData->idxPhantomSector4) + ", $");
						LOG.print(proSectorData->idxPhantomSector4, HEX);
						LOG.println(" - Index of phantom sector (PHANTOM4) starting with sector 721 (1-indexed)");
					}

					if (proSectorData->idxPhantomSector5 > 0 ) {
						LOG.print(String(proSectorData->idxPhantomSector5) + ", $");
						LOG.print(proSectorData->idxPhantomSector5, HEX);
						LOG.println(" - Index of phantom sector (PHANTOM5) starting with sector 721 (1-indexed)");
					}

				}

				if (proSectorData->secStatus != 0) {
					LOG.print(String(proSectorData->secStatus) + ", $");
					LOG.print(proSectorData->secStatus, HEX);

					switch(proSectorData->secStatus) {
						case 0xe4: {
							LOG.println(" - Bad sectors sometimes have this value");
							break;
						}
						case 0xec: {
							LOG.println(" - Bad or CRC error sectors sometimes have this value");
							break;
						}
						case 0xf0: {
							LOG.println(" - Normal sectors sometimes have this value");
							break;
						}
						case 0xf8: {
							LOG.println(" - Normal sectors sometimes have this value");
							break;
						}
						default: {
							LOG.println(" - Unknown ???");
						break;
						}
					}
				}
#endif

			} else {
				imageFile.seek(sPos + m_imageHeaderSize + m_sectorInfoSize);		// skip header + sector info
			}
			
			imageFile.read(sector, sSec);
		}
	}
	return sectorPtr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool vDrive::mountXex(String filePath) {
	
	_filePath = filePath;

	// Переинициализация
	init(m_vDriveId);
	
	if (imageFile) {
		imageFile.close();
	}
	
	imageFile = SD.open(filePath, "r");

	if (!imageFile) {

#ifdef DEBUG
		LOG.println("Error open file \"" + filePath + "\"");
#endif
		return false;

	} else {

#ifdef DEBUG
		LOG.println("XEX file \"" + filePath + "\" successfully mounted!");
#endif

		// Мотор вкл
		m_statusFrame.drvStatus.motorRunning = 1;

		// Диск в приводе (инвертированное значение)
		m_statusFrame.ctrlStatus.notReady = 1;

		m_imageType = IMG_TYPE_XEX;

		m_fileSize = imageFile.size();
		m_imageSize = imageFile.size();

		//TODO:
		m_secPerTrack = 18;

		byte tData[4];

		unsigned short runAddr = 0;
		unsigned short initAddr = 0;
		unsigned int nAddr = 2;					//skip header #FFFF
		unsigned int chunkId = 0;

		while(nAddr < m_imageSize) {
			imageFile.seek(nAddr);
			imageFile.read(tData, 4);

			unsigned short startAddr = tData[0] + (tData[1] * 256);

			if (startAddr == 0xFFFF) {

#ifdef DEBUG
				LOG.println("----------------------------------");
				LOG.println("Addr Shit detected #FFFF!! Skiping");
				LOG.println("----------------------------------");
#endif
				nAddr += 2;
				imageFile.seek(nAddr);
				imageFile.read(tData,4);
		
				startAddr = tData[0] + (tData[1] * 256);
			}

			unsigned short endAddr = tData[2] + (tData[3] * 256);
			unsigned short blockSize = endAddr - startAddr + 1;

			Diskette* d = m_diskPtr + chunkId;

			if (blockSize <= MAX_CHUNK_SIZE) {
				d->side = 1;
				d->track = chunkId;
				d->sector = 1;
				d->offset = nAddr+4;
				d->loadAddr = startAddr;
				d->loadSize = blockSize;
				chunkId++;
	  		} else {

				unsigned short chunkSize = blockSize;
				unsigned short nPos = nAddr+4;
				unsigned short aOffset = 0;

				while (chunkSize > MAX_CHUNK_SIZE) {
					d = m_diskPtr + chunkId;

					d->side = 1;
					d->track = chunkId;
					d->sector = 1;
					d->offset = nPos;
					d->loadAddr = startAddr + aOffset;
					d->loadSize = MAX_CHUNK_SIZE;
							
					chunkSize -= MAX_CHUNK_SIZE;
					chunkId++;
					nPos += MAX_CHUNK_SIZE;
					aOffset += MAX_CHUNK_SIZE;
				}

				d = m_diskPtr + chunkId;
		
				d->side = 1;
				d->track = chunkId;
				d->sector = 1;
				d->offset = nPos;
				d->loadAddr = startAddr + aOffset;
				d->loadSize = chunkSize;
		
				chunkId++;
			}

			if (startAddr == 0x02e0) {
				imageFile.read(tData,4);
				runAddr = tData[0] + tData[1] * 256;
				initAddr = tData[2] + tData[3] * 256;
			}

			// 4 - skip start & end addrs
			nAddr += blockSize + 4;
		}

		m_diskRecCount = chunkId;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool vDrive::mountImage(String filePath, byte imageType) {

	_filePath = filePath;

	// Переинициализация
	init(m_vDriveId);
	
	if (imageFile) {
		imageFile.close();
	}
	
	imageFile = SD.open(filePath, "r");

	if (!imageFile) {

#ifdef DEBUG
		LOG.println("Error open file \"" + filePath + "\"");
#endif
		return false;

	} else {

#ifdef DEBUG
		LOG.println("Disk image file \"" + filePath + "\" successfully mounted!");
		LOG.println("Read file header");
#endif

		// Мотор вкл
		m_statusFrame.drvStatus.motorRunning = 1;

		// Диск в приводе (инвертированное значение)
		m_statusFrame.ctrlStatus.notReady = 1;

		m_imageType = imageType;

		m_imageHeaderSize = getImgHeaderSize(imageType);
		m_fileSize = imageFile.size();
		m_imageSize = m_fileSize - m_imageHeaderSize;

		m_sectorInfoSize = getImgSectorInfoSize(imageType);

	// SD
	if (m_imageSize > 0 && m_imageSize <= 92160) {

#ifdef DEBUG
		LOG.println("Image type SD");
#endif

		m_secPerTrack = 18;
		m_diskSideCount = 1;

	// QD
	} else if (m_imageSize > 92160 && m_imageSize <= 133120) {

#ifdef DEBUG
		LOG.println("Image type QD");
#endif

		m_secPerTrack = 26;
		m_diskSideCount = 1;

	// DD
	} else if (m_imageSize > 133120 && m_imageSize <= 183936) {

#ifdef DEBUG
		LOG.println("Image type DD");
#endif

		m_secPerTrack = 18;
		m_diskSideCount = 2;

	// DD
	} else if (m_imageSize > 183936 && m_imageSize <= 368256) {

#ifdef DEBUG
		LOG.println("Image type DD");
#endif

		m_secPerTrack = 36;
		m_diskSideCount = 2;
	} else {

		//TODO:
#ifdef DEBUG
		LOG.println("Image broken ????");
#endif

	}

	m_sectorSize = 128;			// По умолчанию
	
	if (m_imageHeaderSize > 0) {
		byte header[m_imageHeaderSize];
		imageFile.read(header, m_imageHeaderSize);

		if (imageType == IMG_TYPE_ATR) {
			memcpy(atrHeader, header, m_imageHeaderSize);
			m_sectorSize = atrHeader->secSize;

		} else if (imageType == IMG_TYPE_PRO) {
			memcpy(proHeader, header, m_imageHeaderSize);
			// Из-за того что автор формата PRO решил хранить размер в BigIndian
			// приходится делать такой изврат ¯\_(ツ)_/¯
			proHeader->imageSizeSec = header[0] * 256 + header[1];
			proHeader->diskSizeSec =  header[6] * 256 + header[7];
		
			// В формате это нигде не указано, так что будем считать что это типа верно :)
			m_sectorSize = 128;
			 
		}

#ifdef DEBUG
		LOG.println("Header dump:");

		for (int i=0; i<m_imageHeaderSize; i++) {
			if (i>0) LOG.print(",");
			LOG.print("$");
			if (header[i]<10) LOG.print("0");
			LOG.print(header[i], HEX);
		}
		LOG.println(" ");
#endif 
	}

	int imSizeNoBoot = m_imageSize - (128*3);

#ifdef DEBUG
	LOG.print("imSizeNoBoot=");
	LOG.println(imSizeNoBoot);
	LOG.print("m_sectorSize=");
	LOG.println(m_sectorSize);
	LOG.print("m_secPerTrack=");
	LOG.println(m_secPerTrack);
#endif

	m_sectorCount = imSizeNoBoot/m_sectorSize + 3;
	m_trackCount = m_sectorCount / m_secPerTrack;

#ifdef DEBUG
	if (m_imageHeaderSize > 0) {
		if (imageType == IMG_TYPE_ATR) {
			LOG.println("\nATR Header:");
			LOG.print("$");
			LOG.print(atrHeader->signature, HEX);
			LOG.println(" - Identification");
			LOG.print(atrHeader->diskSize);
			LOG.println(" - Size of disk image. The size is expressed in \"paragraphs\".");
			LOG.print(atrHeader->secSize);
			LOG.println(" - Sector size. 128 or 256 bytes per sector.");
			LOG.print(atrHeader->highPartSize);
			LOG.println(" - High part of size in paragraphs (added by REV 3.00)");

			LOG.println("Disk flags:");
			LOG.print(atrHeader->diskFlags.copyProtected);
			LOG.println(" - Bit 4: copy protected (has bad sectors)");
			LOG.print(atrHeader->diskFlags.writeProtected);
			LOG.println(" - Bit 5: write protected");
			LOG.print(atrHeader->firstBadSector);
			LOG.println(" - word which contains the number of the first bad sector");
	
		} else if (imageType == IMG_TYPE_PRO) {
			LOG.println("\nPRO Header:");

			LOG.print("\"" + String(proHeader->signature) + "\", $");
			LOG.print(proHeader->signature, HEX);
			LOG.println(" - Identification");

			LOG.print("\"" + String(proHeader->version) + "\", $");
			LOG.print(proHeader->version, HEX);
			LOG.println(" - Image Version");

			LOG.print(String (proHeader->imageSizeSec) + ", $");
			LOG.print(proHeader->imageSizeSec, HEX);
			LOG.println(" - Disk Size (include phantoms) in Sectors");

			LOG.print(String (proHeader->diskSizeSec) + ", $");
			LOG.print(proHeader->diskSizeSec, HEX);
			LOG.println(" - Disk Size in Sectors");

			LOG.print(String (proHeader->emuPhantomSecor) + ", $");
			LOG.print(proHeader->emuPhantomSecor, HEX);
			
			String emuType = "Unknown";
			if (proHeader->emuPhantomSecor < sizeof(emuList)) {
				emuType = emuList[proHeader->emuPhantomSecor];
			}
			LOG.println(" \"" + emuType + "\" - Phantom sector emulation mode");

			LOG.print(String (proHeader->delay) + ", $");
			LOG.print(proHeader->delay, HEX);
			LOG.println(" - Delay between each sector read expressed in 1/60th of a second");	  
		}
	}
	
	LOG.print("\nDisk size (without image header): ");
	LOG.print(m_imageSize);
	LOG.println(" Bytes");

	LOG.print("Disk size (without boot sectors): ");
	LOG.print(imSizeNoBoot);
	LOG.println(" Bytes");  

	LOG.print("Sector size: ");
	LOG.print(m_sectorSize);
	LOG.println(" Bytes");

	LOG.print("Sector count: ");
	LOG.println(m_sectorCount);

	LOG.print("Sector per Track: ");
	LOG.println(m_secPerTrack);

	LOG.print("Track count: ");
	LOG.println(m_trackCount);

	LOG.print("Disk has side: ");
	LOG.println(m_diskSideCount);

	LOG.println("");

#endif

	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short vDrive::getImgHeaderSize(byte imageType) {
	unsigned short headerSize = 0;
	switch(imageType) {
		case IMG_TYPE_ATR: {
			headerSize = 16;
			break;
		}
		case IMG_TYPE_XFD: {
			headerSize = 0;
			break;
		}
		case IMG_TYPE_DCM: {
			//TODO:
			break;
		}
		case IMG_TYPE_ATX: {
			//TODO:
			break;
		}
		case IMG_TYPE_SCP: {
			//TODO:
			break;
		}
		case IMG_TYPE_PRO: {
			headerSize = 16;
			break;
		}
	}
	return headerSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned short vDrive::getImgSectorInfoSize(byte imageType) {
	unsigned short infoSize = 0;
	switch(imageType) {
		case IMG_TYPE_ATR: {
			infoSize = 0;
			break;
		}
		case IMG_TYPE_XFD: {
			infoSize = 0;
			break;
		}
		case IMG_TYPE_DCM: {
			//TODO:
			break;
		}
		case IMG_TYPE_ATX: {
			//TODO:
			break;
		}
		case IMG_TYPE_SCP: {
			//TODO:
			break;
		}
		case IMG_TYPE_PRO: {
			infoSize = 12;
			break;
		}
	}
	return infoSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte vDrive::getMountedImgType() {
	return m_imageType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Diskette vDrive::getRecordInfo(unsigned int recId) {
	return m_diskPtr[recId];
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int vDrive::getDiskRecCount() {
	return m_diskRecCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte* vDrive::getRecordData(unsigned int recId) {
	Diskette d = m_diskPtr[recId];
	m_sectorSize = d.loadSize;

	// TODO: Временное решение, нужно будет указывать реальный трек
	m_currentSectorPos = recId;
	m_currentTrackPos = 0;		// Ну как бэ все «сектора» на одном треке (впрочем хз) надо подумать ;)

#ifdef DEBUG
	LOG.print("Offset: +");
	LOG.println(d.offset, HEX);
#endif

	imageFile.seek(d.offset);
	imageFile.read(sector, d.loadSize);

	return (byte*)sector;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void vDrive::setSectorData(unsigned short sectorPos, byte* sectorData) {
	// Текущий сектор
	m_currentSectorPos = sectorPos;
	// Текущий трек (псевдо расчёт)
	m_currentTrackPos = m_currentSectorPos/m_secPerTrack;

#ifdef DEBUG
	LOG.print("[VD]: ID=0x");
	LOG.println(m_vDriveId, HEX);
	LOG.print("[VD]: currentSectorPos=");
	LOG.println(m_currentSectorPos);
#endif

	// Первые 3 сектора не зависимо от размеров всего 128б это бут область!
	unsigned short sSec = 128;
	if (sectorPos > 3) {
		sSec = m_sectorSize;
	}

	int sPos = (sSec + m_sectorInfoSize) * (sectorPos - 1);

#ifdef DEBUG
	LOG.print("[VD]: sectorSize=");
	LOG.println(m_sectorSize);
#endif

	// Если размер сектора > 128
	if (m_sectorSize > 128 && sectorPos > 3) {
		sPos = sSec * (sectorPos - 1 - 3) + 128 *3;
	}

#ifdef DEBUG
	LOG.print("[VD]: sPos=");
	LOG.println(sPos);
#endif

	if (m_imageType == IMG_TYPE_XEX) {

#ifdef DEBUG
		LOG.println("ERROR: XEX FILE READ ONLY!");
#endif 

	} else {

#ifdef DEBUG
		LOG.print("Write sector size ");
		LOG.print(sSec);
		LOG.print(" to file at $");
		LOG.print(sPos + m_imageHeaderSize, HEX);
		LOG.print("(");
		LOG.print(sPos + m_imageHeaderSize);
		LOG.println(")");

		byte* b = sectorData;
		byte* bb = sectorData;
		uint8_t count = 0;
		for (int i=0; i < sSec; i++) {
			String value = "";
			if (*b < 16) {
				value = "0";
			}
			LOG.print(value);
			LOG.print(*b, HEX);
			b++;
			
			count++;
			if (count == 32) {
				count = 0;
				LOG.print(" ");
				for (int j=0; j < 32; j++) {
					if (*bb >= 0x20 && *bb <= 0x7f) {
						LOG.write(*bb);
					} else {
						LOG.print(".");
					}
					bb++;
				}
				LOG.println(" ");
			} else {
				LOG.print(" ");
			}
		}
		LOG.println(" ");
#endif 

		 try {

			//TODO: Костыль из-за того что SD-либа не может одновременно открыть "rw"
			imageFile.close();
			imageFile = SD.open(_filePath, "w");

			// TODO: костыль из-за того что SD-либа заливает всё нулями между концом файла и куда писали
			imageFile.seek(m_fileSize);

			imageFile.seek(sPos + m_imageHeaderSize);		// skip header
			imageFile.write(sectorData, sSec);

			// TODO: костыль из-за того что SD-либа пишет неправильный размер файла
			imageFile.seek(m_fileSize);
			imageFile.close();

			imageFile = SD.open(_filePath, "r");

		 } catch( int ErrCode ) {

#ifdef DEBUG
			LOG.print("Write error=");
			LOG.println(ErrCode);
#endif

		 }
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
