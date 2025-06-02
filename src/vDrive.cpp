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
	m_secPerTrack = 0;	// Default

	// Текущий сектор
	m_currentSectorPos = 0;

	// Текущий трек
	m_currentTrackPos = 0;
	// Текущий сектор на треке
	m_currentTrackSecPos = 1;

	// Количесво сторон диска
	m_diskSideCount = 1;

	// Количество записей
	m_diskRecCount = 0;

	// Инициализация сектора
	sector[0] = 0;
	sectorPtr = sector;

	// Инициализация массива 
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
	
	} else if (m_imageType == IMG_TYPE_ATX) {
		// Ничего не делаем

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
		if (m_secPerTrack > 0) {
			m_currentTrackPos = (m_currentSectorPos - 1) / m_secPerTrack;
			m_currentTrackSecPos = m_currentSectorPos - (m_currentTrackPos * m_secPerTrack);

		} else {
#ifdef DEBUG
			LOG.print("Sector per track is 0!!! Broken disk image?");
#endif
			m_currentTrackPos = 0;
			m_currentTrackSecPos = 0;
		}
	
		// Текущий сектор на треке

#ifdef DEBUG
		LOG.print("[READ] Track:");
		LOG.print(m_currentTrackPos);
		LOG.print(", Sector:");
		LOG.print(m_currentTrackSecPos);

		LOG.print(" (");
		LOG.print(m_secPerTrack);
		LOG.println(" on track)");
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

			} else if (m_imageType == IMG_TYPE_ATX) {
				uint32_t currentTrackStartPos = atxTrackPosFile[m_currentTrackPos];
#if defined(DEBUG) && defined(DEBUG_ATX)
				LOG.print("Current track: ");
				LOG.print(m_currentTrackPos);
				LOG.print(" is file offset: 0x");
				LOG.println(currentTrackStartPos, HEX);
#endif				
				imageFile.seek(currentTrackStartPos);		// На позицию начала трека

				byte buf[255];

				AtxTrackHeader atxTrackHeader[32];
				imageFile.read(buf, 32);				
				memcpy(atxTrackHeader, buf, 32);
				uint16_t trackNumber = atxTrackHeader->trackNumber;
				uint16_t sectorCount = atxTrackHeader->sectorCount;

				imageFile.read(buf, 8); 			// пропускаем 8 байт МУСОРА Sector List

				uint32_t sectorDataStartPos = 0;
#if defined(DEBUG) && defined(DEBUG_ATX)
				LOG.print("Sectors count: ");
				LOG.print(sectorCount);
				LOG.print(", Found Sectors: ");
#endif

				// Сбрасываем настройки по умолчанию (всё ок)
				resetDevice();

				for (uint16_t i=0; i<sectorCount; i++) {
					// Заголовок сектора формата ATX (8 байт)
					AtxSectorHeader atxSectorHeader[8];
					imageFile.read(buf, 8);				
					memcpy(atxSectorHeader, buf, 8);

					uint8_t	sectorNumber = atxSectorHeader->sectorNumber;
#if defined(DEBUG) && defined(DEBUG_ATX)
					LOG.print(sectorNumber);
#endif

					if (m_currentTrackSecPos == sectorNumber) {

						uint8_t	prevHoleIndex = currentHoleIndex;
						if (i < (sectorCount + 2 - 1) / 2) {
							currentHoleIndex = 1;
						} else {
							currentHoleIndex = 2;
						}

#if defined(DEBUG) && defined(DEBUG_ATX)
						LOG.print("\nCurrent Hole Index : ");
						LOG.println(currentHoleIndex);
#endif

						AtxSectorStatus	sectorStatus = atxSectorHeader->sectorStatus;
						// Инвертированные значения
						if (sectorStatus.lostData == 1) m_statusFrame.ctrlStatus.lostData = 0;
						if (sectorStatus.crcError == 1) m_statusFrame.ctrlStatus.crcError = 0;
						if (sectorStatus.deletedSec == 1) m_statusFrame.ctrlStatus.deletedSector = 0;

						sectorDataStartPos = currentTrackStartPos + atxSectorHeader->startData;
						
						if (sectorStatus.lostData  || sectorStatus.crcError || sectorStatus.missingSec || sectorStatus.deletedSec || sectorStatus.extentedSec) {
#if defined(DEBUG) && defined(DEBUG_ATX)
							LOG.println("\nSector Status:");
							LOG.println("   FDC lost data error: " + String(sectorStatus.lostData ? "true" : "false"));
							LOG.println("   FDC CRC error: " + String(sectorStatus.crcError ? "true" : "false"));
							LOG.println("   Missting sector data: " + String(sectorStatus.missingSec ? "true" : "false"));
							LOG.println("   Deleted sector: " + String(sectorStatus.deletedSec ? "true" : "false"));
							LOG.println("   Extented data present for sector: " + String(sectorStatus.extentedSec ? "true" : "false"));
#endif
						}

						bool skipThisSec = false;

						for (uint8_t idx=0; idx < atxDupPos; idx++) {
							if (atxDuplicates[idx].track == trackNumber && atxDuplicates[idx].sector == sectorNumber) {
								if (currentHoleIndex == prevHoleIndex) {
									skipThisSec = true;
								}
							}
						}

						if (skipThisSec == false) {
							break;
						} else {
#if defined(DEBUG) && defined(DEBUG_ATX)
							LOG.print("(Skip DUPLICATE)");
#endif							
						}


					}
#if defined(DEBUG) && defined(DEBUG_ATX)
					LOG.print(", ");
#endif
				}
#if defined(DEBUG) && defined(DEBUG_ATX)
				LOG.println("");
#endif
				if (sectorDataStartPos == 0) {

					m_statusFrame.ctrlStatus.recordNotFound = 0;

#ifdef DEBUG
					LOG.print("\n\t\t\t\t\t\t\t\t!!! ERROR SECTOR NOT FOUND: ");
					LOG.print(m_currentTrackSecPos);
					LOG.println(" !!!");
#endif
				} else {
					imageFile.seek(sectorDataStartPos);
				}

			} else {
				imageFile.seek(sPos + m_imageHeaderSize + m_sectorInfoSize);		// skip header + sector info
			}
			
			imageFile.read(sector, sSec);

#if defined(DEBUG) && defined(DEBUG_ATX)
			if (m_imageType == IMG_TYPE_ATX) {
				LOG.println("Sector Dump:");
				for (uint8_t i=0; i<sSec; i++) {
					if (i > 0) LOG.print(",");
					if (sector[i] < 16) LOG.print("0");
					LOG.print(sector[i], HEX);
				}
				LOG.println("\n------------");
			}
#endif

		}
	}
	return sectorPtr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool vDrive::mountXex(String filePath) {
	
	_filePath = filePath;

	bool isMounted = false;

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
	} else {

		byte tData[4];
		imageFile.read(tData, 2);

		unsigned short xexID = tData[0] + (tData[1] * 256);
		if (xexID != 0xFFFF) {
			LOG.println("XEX file \"" + filePath + "\" fail to mount! Wrong Magic ID!");
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

#ifdef DEBUG
			LOG.print("XEX image size:");
			LOG.println(m_imageSize);
#endif

			//TODO:
			m_secPerTrack = 18;

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

#ifdef DEBUG
				LOG.print("XEX block: StartAddr=");
				LOG.print(startAddr);
				LOG.print("(0x");
				LOG.print(startAddr, HEX);
				LOG.print("), EndAddr=");
				LOG.print(endAddr);
				LOG.print("(0x");
				LOG.print(endAddr, HEX);
				LOG.print("), Size=");
				LOG.print(blockSize);
				LOG.print(", MAX_CHUNK_SIZE=");
				LOG.println(MAX_CHUNK_SIZE);
#endif

				if (blockSize <= MAX_CHUNK_SIZE) {
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

						d->offset = nPos;
						d->loadAddr = startAddr + aOffset;
						d->loadSize = MAX_CHUNK_SIZE;
								
						chunkSize -= MAX_CHUNK_SIZE;
						chunkId++;
						nPos += MAX_CHUNK_SIZE;
						aOffset += MAX_CHUNK_SIZE;
					}

					d = m_diskPtr + chunkId;

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
#ifdef DEBUG
			LOG.print("XEX chunks count:");
			LOG.println(m_diskRecCount);
#endif

			isMounted = true;
		}
	}

	return isMounted;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void vDrive::unMount() {
	// Переинициализация
	init(m_vDriveId);
	
	if (imageFile) {
		imageFile.close();
	}

	m_imageType = IMG_TYPE_NONE;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool vDrive::mountImage(String filePath, byte imageType) {

	_filePath = filePath;

	// Заголовок примонтированной дискеты формата ATX (48 байт)
	AtxHeader atxHeader[48];

	// Инициализация записей о дублях ATX
	atxDupPos = 0;
	currentHoleIndex = 1;
	atxDuplicates[ATX_DUP_SIZE] = {};

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

		if (imageType != IMG_TYPE_ATX) {
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

			} else if (imageType == IMG_TYPE_ATX) {
				memcpy(atxHeader, header, m_imageHeaderSize);

				m_secPerTrack = 18;
				m_diskSideCount = 1;

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
			
			} else if (imageType == IMG_TYPE_ATX) {
				LOG.println("\nATX Header:");

				LOG.print("\"" + String(atxHeader->signature)  + "\", $");
				LOG.print(atxHeader->signature[0], HEX);
				LOG.print(", $");
				LOG.print(atxHeader->signature[1], HEX);
				LOG.print(", $");
				LOG.print(atxHeader->signature[2], HEX);
				LOG.print(", $");
				LOG.print(atxHeader->signature[3], HEX);
				LOG.println(" - Identification");

				LOG.print("\"" + String(atxHeader->version) + "\", $");
				LOG.print(atxHeader->version, HEX);
				LOG.println(" - Version");
				LOG.print("\"" + String(atxHeader->minVersion) + "\", $");
				LOG.print(atxHeader->minVersion, HEX);
				LOG.println(" - Minimal Version");

				LOG.print("\"" + String(atxHeader->creator) + "\", $");
				LOG.print(atxHeader->creator, HEX);
				LOG.println(" - Creator ($01-TX_CR_FX7, $02-ATX_CR_FX8, $03-ATX_CR_ATR, $10-ATX_CR_WH2PC, $74-a8diskutil)");
				LOG.print("\"" + String(atxHeader->creatorVersion) + "\", $");
				LOG.print(atxHeader->creatorVersion, HEX);
				LOG.println(" - Creator Version");

				LOG.print("\"" + String(atxHeader->flags) + "\", $");
				LOG.print(atxHeader->flags, HEX);
				LOG.println(" - Flags");

				LOG.print("\"" + String(atxHeader->imageType) + "\", $");
				LOG.print(atxHeader->imageType, HEX);
				LOG.println(" - Image Type");

				LOG.print("\"" + String(atxHeader->density) + "\", $");
				LOG.print(atxHeader->density, HEX);
				LOG.println(" - Density ($00-Single, $01-Medium, $02-Double)");
				
				LOG.print("\"" + String(atxHeader->imageId) + "\", $");
				LOG.print(atxHeader->imageId, HEX);
				LOG.println(" - Image ID");
				LOG.print("\"" + String(atxHeader->imageVersion) + "\", $");
				LOG.print(atxHeader->imageVersion, HEX);
				LOG.println(" - Image Version");

				LOG.print("\"" + String(atxHeader->start) + "\", $");
				LOG.print(atxHeader->start, HEX);
				LOG.println(" - Start. Offset from start of file to first data record");
				LOG.print("\"" + String(atxHeader->end) + "\", $");
				LOG.print(atxHeader->end, HEX);
				LOG.println(" - End");

			}
		}

		if (imageType != IMG_TYPE_ATX) {
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
		}

#endif
		if (imageType == IMG_TYPE_ATX) {
			byte buf[255];

			uint32_t trackHeaderStartPos = imageFile.position();
			uint16_t sioSector = 1;
			uint8_t idx = 0;
			uint16_t tmpSectorCount[40] = {0};
			while (idx < 40) {
				// Заголовок дорожки примонтированной дискеты формата ATX (32 байта)
				AtxTrackHeader atxTrackHeader[32];

				imageFile.read(buf, 32);				
				memcpy(atxTrackHeader, buf, 32);

				uint32_t trackLenght = atxTrackHeader->lenght;
				uint16_t trackType = atxTrackHeader->type;
				uint8_t trackNumber = atxTrackHeader->trackNumber;
				uint16_t sectorCount = atxTrackHeader->sectorCount;
				uint16_t trackRate = atxTrackHeader->rate;
				uint32_t trackFlags = atxTrackHeader->flags;
				uint32_t offsetDataStart = atxTrackHeader->headerSize;

				atxTrackPosFile[trackNumber] = trackHeaderStartPos;
				atxDisk[idx]->trackNumber = trackNumber;
				atxDisk[idx]->sioSector = sioSector;

				tmpSectorCount[idx] = sectorCount;

				idx++;
				sioSector += sectorCount;

#if defined(DEBUG) && defined(DEBUG_ATX)
				LOG.print("\nATX Track ");
				LOG.print(String(trackNumber) + " (0x");
				LOG.print(trackNumber, HEX);
				LOG.print(") Header Start Pos at " + String(trackHeaderStartPos) + " (0x");
				LOG.print(trackHeaderStartPos, HEX);
				LOG.println(")");
				
				LOG.println("_____________________________");
				LOG.print("\"" + String(trackLenght) + "\", $");
				LOG.print(trackLenght, HEX);
				LOG.println(" - size of the whole track record in bytes");

				LOG.print("\"" + String(trackType) + "\", $");
				LOG.print(trackType, HEX);
				LOG.println(" - type (0x0000 - data track record, 0x0100 - host data record)");

				LOG.print("\"" + String(sectorCount) + "\", $");
				LOG.print(sectorCount, HEX);
				LOG.println(" - sectors in track");
				
				LOG.print("\"" + String(trackRate) + "\", $");
				LOG.print(trackRate, HEX);
				LOG.println(" - track Rate");
				
				LOG.print("\"" + String(trackFlags) + "\", $");
				LOG.print(trackFlags, HEX);
				LOG.println(" - track flags (0x0002 - MFM track, 0x0100 -Couldn't measure skew align)");

				LOG.print("\"" + String(offsetDataStart) + "\", $");
				LOG.print(offsetDataStart, HEX);
				LOG.println(" - offset from start of track header to start of track data in bytes.");
#endif

				uint32_t chunkHeaderStartPos = imageFile.position();

				// Заголовок дорожки примонтированной дискеты формата ATX (8 байта)
				AtxTrackChunkHeader atxTrackChunkHeader[8];
				imageFile.read(buf, 8);
				memcpy(atxTrackChunkHeader, buf, 8);

				uint32_t length = atxTrackChunkHeader->length;
				uint8_t type = atxTrackChunkHeader->type;
				uint8_t sectorIndex = atxTrackChunkHeader->sectorIndex;
				uint16_t headerData = atxTrackChunkHeader->headerData;

#if defined(DEBUG) && defined(DEBUG_ATX)
				LOG.print("\nATX Chunk Header Start Pos at " + String(chunkHeaderStartPos) + " (0x");
				LOG.print(chunkHeaderStartPos, HEX);
				LOG.println(")");

				LOG.print("\"" + String(length) + "\", $");
				LOG.print(length, HEX);
				LOG.println(" - size of sector list in bytes");
				
				LOG.print("\"" + String(type) + "\", $");
				LOG.print(type, HEX);
				LOG.println(" - type: 0x00-Sector data, 0x01-Sector list, 0x10-Weak sector data, 0x11 - Extended sector header");
				
				LOG.print("\"" + String(sectorIndex) + "\", $");
				LOG.print(sectorIndex, HEX);
				LOG.println(" - sectorIndex: contains the index of a sector within the track for chunk types that need it (e.g. weak sector data). For other chunk types it is 0.");
				
				LOG.print("\"" + String(headerData) + "\", $");
				LOG.print(headerData, HEX);
				LOG.println(" - headerData: contains additional chunk-specific data (or two 0's if not used).");

#endif
				// uint32_t firstSectorDataStartPos = 0;
				bool tmpSectorNums[255] = { false };
				for (uint16_t i=0; i<sectorCount; i++) {
					uint32_t currentPos = imageFile.position();

					// Заголовок сектора формата ATX (8 байт)
					AtxSectorHeader atxSectorHeader[8];
					imageFile.read(buf, 8);				
					memcpy(atxSectorHeader, buf, 8);

					uint8_t	sectorNumber = atxSectorHeader->sectorNumber;

					AtxSectorStatus	sectorStatus = atxSectorHeader->sectorStatus;
					bool lostData = false;
					if (sectorStatus.lostData == 1) lostData = true;
					bool crcError = false;
					if (sectorStatus.crcError == 1) crcError = true;
					bool missingSec = false;
					if (sectorStatus.missingSec == 1) missingSec = true;
					bool deletedSec = false;
					if (sectorStatus.deletedSec == 1) deletedSec = true;
					bool extentedSec = false;
					if (sectorStatus.extentedSec == 1) extentedSec = true;
					uint32_t sectorDataStartPos = trackHeaderStartPos + atxSectorHeader->startData;

					if (tmpSectorNums[sectorNumber] == true) {
						if (atxDupPos > ATX_DUP_SIZE) {
#ifdef DEBUG
							LOG.println("!!! ERROR: NOT ENOUGH MEMORY FOR DUPLICATES !!! ");
#endif
						} else {
							atxDuplicates[atxDupPos].track = trackNumber;
							atxDuplicates[atxDupPos].sector = sectorNumber;
							atxDupPos += 1;
						}

					} else {
						tmpSectorNums[sectorNumber] = true;
					}
				}

				trackHeaderStartPos = trackHeaderStartPos + trackLenght;
				imageFile.seek(trackHeaderStartPos);
			}

#if defined(DEBUG) && defined(DEBUG_ATX)
			LOG.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
			LOG.println("Track\t|\tCount\t|\tSIO\t|\tFile offset");
			LOG.println("--------------------------------------------------------------------");
			for (uint8_t idx=0; idx<40; idx++) {
				LOG.print(atxDisk[idx]->trackNumber);
				LOG.print("\t|\t");
				LOG.print(tmpSectorCount[idx]);
				LOG.print("\t|\t");
				LOG.print(atxDisk[idx]->sioSector);
				LOG.print("\t|\t0x");
				LOG.println(atxTrackPosFile[idx], HEX);
			}
			if (atxDupPos > 0) {
				LOG.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
				LOG.println("DUPLICATES:");
				for (uint8_t idx=0; idx<atxDupPos; idx++) {
					LOG.print("Track: ");
					LOG.print(atxDuplicates[idx].track);
					LOG.print(", Sector: ");
					LOG.println(atxDuplicates[idx].sector);
				}
			}
			LOG.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
#endif
		}


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
			headerSize = 48;
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
	m_currentTrackSecPos = 1;

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
	if (m_imageType != IMG_TYPE_NONE) {
		// Текущий сектор
		m_currentSectorPos = sectorPos;
		// Текущий трек (псевдо расчёт)
		m_currentTrackPos = (m_currentSectorPos - 1) / m_secPerTrack;
		// Текущий сектор на треке
		m_currentTrackSecPos = m_currentSectorPos - (m_currentTrackPos * m_secPerTrack);

#ifdef DEBUG
		LOG.print("[SAVE] sector:");
		LOG.println(m_currentSectorPos);

		LOG.print("Disk Track:");
		LOG.println(m_currentTrackPos);
		LOG.print("Disk Sector:");
		LOG.println(m_currentTrackSecPos);
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

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
