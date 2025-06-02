#ifndef VDRIVE_h
#define VDRIVE_h

#include <Arduino.h>
#include <SD.h>
#include "dev.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRO disk protection list
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const static String emuList[] = {
	"Simple Emulation",
	"Mindscape Special",
	"Global Flip/Flop",
	"Global Flip/Flop",
	"Heuristic",
	"Sticky",
	"Reverse Sticky",
	"Shimmering",
	"Reverse Shimmer",
	"Rolling Thunder"
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const unsigned short MAX_DISK_RECORDS	= 200;		// Сколько максимальных записей (секторов/чанком) может быть
const unsigned short MAX_CHUNK_SIZE	= 1024;		// Максимальный размер чанка
const unsigned short ATX_DUP_SIZE	= 255;		// Зарезервировать на количество записей о дублях секторов в ATX

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Atari executable booter by Fatih Aygun
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static byte xexLoader[] {
	// Sector 1
	0x60, 0x02, 0x00, 0x07, 0xa4, 0x07, 0xa2, 0x00, 0x8e, 0x44, 0x02, 0xe8, 0x86, 0x09, 0xa9, 0x77,
	0x85, 0x02, 0x85, 0x0a, 0xa9, 0xe4, 0x85, 0x03, 0x85, 0x0b, 0xa9, 0x00, 0x8d, 0xe0, 0x02, 0xa9,
	0x07, 0x8d, 0xe1, 0x02, 0xa9, 0x00, 0x8d, 0xe2, 0x02, 0xa9, 0x07, 0x8d, 0xe3, 0x02, 0xa9, 0x31,
	0x8d, 0x00, 0x03, 0xa9, 0x01, 0x8d, 0x01, 0x03, 0xa9, 0xff, 0x8d, 0x02, 0x03, 0xa9, 0x06, 0x8d,
	0x08, 0x03, 0xa9, 0x00, 0x8d, 0x09, 0x03, 0xa9, 0x04, 0x8d, 0x04, 0x03, 0xa9, 0x03, 0x8d, 0x05,
	0x03, 0xa9, 0x40, 0x8d, 0x03, 0x03, 0xad, 0xc3, 0x07, 0x8d, 0x0a, 0x03, 0xad, 0xc4, 0x07, 0x8d,
	0x0b, 0x03, 0x20, 0x59, 0xe4, 0x30, 0x46, 0xa9, 0xfe, 0x8d, 0x02, 0x03, 0xa9, 0x40, 0x8d, 0x03,
	0x03, 0x20, 0x59, 0xe4, 0x30, 0x37, 0xad, 0x07, 0x03, 0x48, 0x20, 0xa7, 0x07, 0xee, 0xc3, 0x07,

	// Sector 2  
	0xd0, 0x03, 0xee, 0xc4, 0x07, 0x68, 0xd0, 0x9c, 0xa9, 0xfd, 0x8d, 0x02, 0x03, 0xa9, 0x00, 0x8d,
	0x03, 0x03, 0xad, 0xe0, 0x02, 0x8d, 0x0a, 0x03, 0xad, 0xe1, 0x02, 0x8d, 0x0b, 0x03, 0x20, 0x59,
	0xe4, 0x20, 0xaa, 0x07, 0x4c, 0x77, 0xe4, 0x6c, 0xe2, 0x02, 0x6c, 0xe0, 0x02, 0x78, 0xa2, 0x00,
	0x8e, 0x00, 0xd4, 0x8e, 0x0e, 0xd4, 0xca, 0x8e, 0x44, 0x02, 0xad, 0x0b, 0xd4, 0x8d, 0x1a, 0xd0,
	0x4c, 0xba, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Состояние привода
struct  DriveStatus {
	byte	unused_0		: 1;		// bit 0 Не задействован
	byte	unused_1		: 1;		// bit 1 Не задействован
	byte	writeError		: 1;		// bit 2 Неудачное выполнение команды PUT
	byte	writeProtected		: 1;		// bit 3 Ошибка из-за защиты от записи
	byte	motorRunning		: 1;		// bit 4 Мотор вращается
	byte	unused_5		: 1;		// bit 5 Не задействован
	byte	unused_6		: 1;		// bit 6 Не задействован
	byte	unused_7		: 1;		// bit 7 Не задействован
};

// Состояние контроллера дисковода гибких дисков (инвертировано из FDC)
struct  ControllerStatus {
	byte	busy			: 1;		// bit 0 Устройство занято
	byte	dataPending		: 1;		// bit 1 Ожидание данных
	byte	lostData		: 1;		// bit 2 Данные утеряны
	byte	crcError		: 1;		// bit 3 Ошибка контрольной суммы
	byte	recordNotFound		: 1;		// bit 4 Запись не найдена (ошибка сектора)
	byte	deletedSector		: 1;		// bit 5 Удалённый сектор (сектор отмечен как удаленный в заголовке сектора)
	byte	writeProtectError	: 1;		// bit 6 Ошибка защита от записи
	byte	notReady		: 1;		// bit 7 Нет диска в приводе

};

// Блок ответа статуса
struct StatusFrame {
	DriveStatus			drvStatus;	// Статус устройства
	ControllerStatus		ctrlStatus;	// Статус контроллера
	byte				defaultTimeout;	// $E0 Время ожидания по умолчанию ($E0 = 224 vertical blanks)
	byte				unused;		// $00 Не задействован
};

// состояние Сектора
struct AtxSectorStatus  {
	byte	unused_0		: 1;		// bit 0 Не задействован
	byte	unused_1		: 1;		// bit 1 Не задействован
	byte	lostData		: 1;		// bit 2 FDC Lost data error
	byte	crcError		: 1;		// bit 3 FDC CRC Error
	byte	missingSec	  	: 1;		// bit 4 Missing Sector Data
	byte	deletedSec		: 1;		// bit 5 Deleted Sector (DAM $F8)
	byte	extentedSec		: 1;		// bit 6 Extented Data present for Sector
	byte	unused_2		: 1;		// bit 7 Не задействован
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Структура данных для чанков
struct Diskette {
	unsigned int		offset;			// 4 байта Смещение относительно файла с образом
	unsigned short		loadAddr;		// 2 байта Адрес загрузки (только для чанков)
	unsigned short		loadSize;		// 2 байта Размер загружаемых данных в байтах (сектор/чанк)
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Блок флагов образа диска
struct DiskFlags  {
	byte	unused_0		: 1;		// bit 0 Не задействован
	byte	unused_1		: 1;		// bit 1 Не задействован
	byte	unused_2		: 1;		// bit 2 Не задействован
	byte	unused_3		: 1;		// bit 3 Не задействован
	byte	copyProtected		: 1;		// bit 4 Защищён от копирования (имеются сектора с ошибками)
	byte	writeProtected		: 1;		// bit 5 Защищён от записи
	byte	unused_6		: 1;		// bit 6 Не задействован
	byte	unused_7		: 1;		// bit 7 Не задействован
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Заголовок дискеты в формате Atr
struct AtrHeader  {
	unsigned short		signature;		// +0x00: 2 байта: Идентификатор формата дискеты 0x0296
	unsigned short		diskSize;		// +0x02: 2 байта: Размер образа диска. Размер указывается в «параграфах»
	unsigned short		secSize;		// +0x04: 2 байта: Размер сектора. 0x80 - 128 байт, 0x100 — 256байт
	unsigned short		highPartSize;		// +0x06: 2 байта: Размер старшей части в параграфах. Добавлено в rev.3.00
	DiskFlags		diskFlags;		// +0x08: 1 байт: Флаги образа диска
	unsigned short		firstBadSector;		// +0x09: 2 байта: Номер первого сектора с ошибками
	byte			unused_0;		// +0x0b: 1 байт: Не задействован
	byte			unused_1;		// +0x0c: 1 байт: Не задействован
	byte			unused_2;		// +0x0d: 1 байт: Не задействован
	byte			unused_3;		// +0x0e: 1 байт: Не задействован
	byte			unused_4;		// +0x0f: 1 байт: Не задействован
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Заголовок дискеты в формате Pro (http://www.whizzosoftware.com/sio2arduino/prosys.html)
struct ProHeader  {
	unsigned short		imageSizeSec;		// +0x00: 2 байта (BigIndian): Количество секторов, содержащихся в данных файла, включая фантомы
	char			signature;		// +0x02: 1 байта: Идентификатор формата дискеты "P"
	char			version;		// +0x03: 1 байта: Версия формата дискеты "3" (возможны варианты "2")
	byte			emuPhantomSecor;	// +0x04: 1 байт: Режим эмуляции фантомных секторов:
							//				0 (Simple Emulation) - Неизвестно. Кажется, что всегда возвращается АКТУАЛЬНЫЙ сектор.
							//				1 (Mindscape Special) - Неизвестно. Кажется, что всегда возвращается АКТУАЛЬНЫЙ сектор.
							//				2 (Global Flip/Flop) - Чтение любого сектора вызывает циклическое переключение фантомных
							//										секторов между возвращением ACTUAL и PHANTOM1 при окончательном чтении.
							//				3 (Global Flip/Flop) - Чтение любого сектора вызывает циклическое переключение фантомных
							//										секторов между возвращением ACTUAL и PHANTOM1 при окончательном чтении.
							//				4 (Heuristic) - Неизвестно. Похоже, ведет себя как Global Flip/Flop.
							//				5 (Sticky) - Неизвестно. Кажется, что всегда возвращается АКТУАЛЬНЫЙ сектор.
							//				6 (Reverse Sticky) - Возвращает сектор PHANTOM1 при первом чтении, а затем каждое последующее чтение возвращает ФАКТИЧЕСКИЙ сектор.
							//				7 (Shimmering) - Неизвестно. Кажется, что всегда возвращается АКТУАЛЬНЫЙ сектор.
							//				8 (Reverse Shimmer) - Неизвестно. Кажется, что всегда возвращается АКТУАЛЬНЫЙ сектор.
							//				9 (Rolling Thunder) - Последовательные чтения секторов, по-видимому, многократно циклически переключаются между PHANTOM1, PHANTOM2,
							//										PHANTOM3, PHANTOM4, PHANTOM5, ACTUAL. Если один из PHANTOMx не определен (0), он пропускается.
							//										Последнее чтение в цикле для каждого сектора, по-видимому, сохраняется, так что следующее чтение этого сектора
							//										(даже после того, как был прочитан другой сектор) может возобновиться с того места, где оно остановилось в цикле.
	byte			delay;			// +0x05: 1 байт: Задержка между чтением каждого сектора, выраженная в 1/60 секунды (значение может быть от 1 до 99)
	unsigned short		diskSizeSec;		// +0x06: 2 байта: (BigIndian) Исходное количество секторов диска. (не больше 1040) (актуально только для версии 3)
	byte			unused_0;		// +0x08: 1 байт: Не задействован  
	byte			unused_1;		// +0x09: 1 байт: Не задействован
	byte			unused_2;		// +0x0a: 1 байт: Не задействован
	byte			unused_3;		// +0x0b: 1 байт: Не задействован
	byte			unused_4;		// +0x0c: 1 байт: Не задействован
	byte			unused_5;		// +0x0d: 1 байт: Не задействован
	byte			unused_6;		// +0x0e: 1 байт: Не задействован
	byte			unused_7;		// +0x0f: 1 байт: Не задействован
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Структура сектора в формате Pro
struct ProSectorData {
	DriveStatus		drvStatus;		// +0x00: 1 байт: Статус привода
	ControllerStatus	ctrlStatus;		// +0x01: 1 байт: Статус контроллера
	unsigned short		timeout;		// +0x02: 2 байт: Таймаут (1771 controller status)

	byte			unused_0;		// +0x04: 1 байт: Не задействован (всегда 0)

	byte			totalPhantomSecAss;	// +0x05: 1 байт: Общее количество фантомных секторов, связанных с этим сектором (значения могут быть от 0 до 5, поскольку ProSys выполняет 5 проходов чтения на сектор)

	byte			idxPhantomSector4;	// +0x06: 1 байт: Индекс фантомного сектора (PHANTOM4) начиная с сектора 721 (1-индексный) 
	byte			idxPhantomSector1;	// +0x07: 1 байт: Индекс фантомного сектора (PHANTOM1) начиная с сектора 721 (1-индексный)
	byte			idxPhantomSector2;	// +0x08: 1 байт: Индекс фантомного сектора (PHANTOM2) начиная с сектора 721 (1-индексный)  
	byte			idxPhantomSector3;	// +0x09: 1 байт: Индекс фантомного сектора (PHANTOM3) начиная с сектора 721 (1-индексный)

	byte			secStatus;		// +0x0a: 1 байт: Неизвестно, возможны варианты:                                                      
							//					$E4 (228) Bad sectors sometimes have this value
							//					$EC (236) Bad or CRC error sectors sometimes have this value
							//					$F0 (240) Normal sectors sometimes have this value
							//					$F8 (248) Normal sectors sometimes have this value

	byte			idxPhantomSector5;	// +0x0b: 1 байт: Индекс фантомного сектора (PHANTOM5) начиная с сектора 721 (1-индексный)  
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Заголовок дискеты в формате ATX (https://www.a8preservation.com/#/guides/atx) 48 байт
struct AtxHeader  {
	char			signature[4];		// +0x00  4 байта Идентификатор формата дискеты "AT8X"
	uint16_t		version;		// +0x04  2 байта Версия формата дискеты (0x01)
	uint16_t		minVersion;		// +0x06  2 байта Минимальная версия формата дискеты (для совместимости?)
	uint16_t		creator;		// +0x08  2 байта ID создателя:
							//			$01 - TX_CR_FX7
							//			$02 - ATX_CR_FX8
							//			$03 - ATX_CR_ATR
							//			$10 - ATX_CR_WH2PC
							//			$74 - a8diskutil
	uint16_t		creatorVersion;		// +0x0a  2 байта Версия создателя ???
	uint32_t		flags;			// +0x0c  4 байта Флаги ???
	uint16_t		imageType;		// +0x10  2 байта Тип образа ???
	uint8_t			density;		// +0x12  1 байт Плотность диска:
							//			$00 - Single
							//			$01 - Medium
							//			$02 - Double
	byte			reserved_0;		// +0x13  1 байта: Не задействовано
	uint32_t		imageId;		// +0x14  4 байта ID образа ???
	uint16_t		imageVersion;		// +0x18  2 байта Версия образа ???
	byte			reserved_1[2];		// +0x1a  2 байта: Не задействовано
	
 	uint32_t		start;			// +0x1C  4 байта (Little-Endian) смещение от начала файла до записи 1го трека (0x00000030)
	uint32_t		end;			// +0x20  4 байта (Little-Endian) Конец чего???
	byte			reserved_2[12];		// +0x24 12 байт: Не задействовано
};

// Заголовок трека 32 байта
struct AtxTrackHeader  {
	uint32_t		lenght;			// +0x00  4 байта (Little-Endian) размер всего трека включая заголовки
	uint16_t		type;			// +0x04  2 байта (Little-Endian) тип: (0x0000 - data track record, 0x0100 - host data record)
	byte			reserved_0[2];		// +0x06  2 байта: Не задействовано
	uint8_t			trackNumber;		// +0x08  1 байт номер трека (0 - 39)
	byte			reserved_1;		// +0x09  1 байт: Не задействовано
	uint16_t		sectorCount;		// +0x0A  2 байта (Little-Endian) количество секторов на треке
	uint16_t		rate;			// +0x0C  2 байта (Little-Endian) ???
	byte			reserved_2[2];		// +0x0E  2 байт: Не задействовано
	uint32_t		flags;			// +0x10  4 байта (Little-Endian) 0x0002 - MFM track, 0x0100 -Couldn't measure skew align
	uint32_t		headerSize;		// +0x14  4 байта (Little-Endian) offset from start of track header to start of track data in bytes.
	byte			reserved_3[8];		// +0x18  8 байт: Не задействовано
};

// Заголовок Список секторов
struct AtxTrackChunkHeader  {
	uint32_t		length;			// +0x00  4 байта (Little-Endian) длина всей записи данных трека в байтах (включая заголовок).
	uint8_t			type;			// +0x04  1 байта тип: 0x00-Sector data, 0x01-Sector list, 0x10-Weak sector data, 0x11 - Extended sector header
	uint8_t			sectorIndex;		// +0x05  1 байт contains the index of a sector within the track for chunk types that need it (e.g. weak sector data). For other chunk types it is 0.
	uint16_t		headerData;		// +0x06  2 байта: contains additional chunk-specific data (or two 0's if not used).
};

// Заголовок Сектора
struct AtxSectorHeader  {
	uint8_t			sectorNumber;		// +0x00  1 байт номер сектора
	AtxSectorStatus		sectorStatus;		// +0x01  1 байт состояние сектора
	uint16_t		sectorPos;		// +0x02  2 байта (Little-Endian) позиция сектора (0-26042)
	uint32_t		startData;		// +0x04  4 байта (Little-Endian) смещение до данных сектора от начала заголовка
};

// Данные расширенного Сектора
struct AtxExtSectorData  {
	uint32_t		size;			// +0x00  4 байта (Little-Endian) размер (чего?)
	uint8_t			type;			// +0x04  1 байт тип (0x10 - weak sector present)
	uint8_t			sectorIndex;		// +0x05  1 байт идекс физического сектора на треке
	uint16_t		weakOffset;		// +0x06  2 байта (Little-Endian) смещение от начала сектора, байт котого будет заменён случайным (не повторяемым) значением
};

// Подготовленные данные для быстрого чтения
struct AtxDisk {
	uint16_t		sioSector;		// Запрашиваемый сектор SIO
	uint8_t			trackNumber;		// Реальный track, расчитываемый из учёта количества секторов на треке
};

struct AtxDuplicates {
	uint8_t			track;			// Трек
	uint8_t			sector;			// Сектор
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class vDrive {
	public:
		vDrive();
		void		init(byte vDriveId);
		void		resetDevice();					// Сброс статусов привода в исходное состояние

		unsigned short 	getSectorSize();				// Получить размер сектора
		StatusFrame*	getStatusFrame();				// Получить указатель на блок данных статуса устройства
		byte*		getSectorData(int sectorPos);			// Получить указатель на блок данных сектора

		bool		mountImage(String filePath, byte imageType);	// Подключить образ дискеты, типа
		bool		mountXex(String filePath);			// Подключить исполняемый файл как образ

		void		unMount();					// Убрать образ

		unsigned short	getImgHeaderSize(byte imageType);
		unsigned short	getImgSectorInfoSize(byte imageType);

		Diskette	getRecordInfo(unsigned int recId);
		byte*		getRecordData(unsigned int recId);

		unsigned int	getDiskRecCount();

		byte		getMountedImgType();
		void		setSectorData(unsigned short sectorPos, byte* sectorData);

	private:
		int		m_vDriveId;					// ID Привода
		StatusFrame	m_statusFrame;					// Блок данных (4 байта) ответа статуса устройства

		int		m_fileSize;					// Размер файла
		int		m_imageSize;					// Размер образа
		byte		m_imageType;					// Тип образа
		unsigned short	m_imageHeaderSize;				// Размер заголовка образа

		unsigned short	m_sectorSize;					// Размер сектора
		unsigned short	m_sectorInfoSize;				// Размер блока доп.данных для сектора

		unsigned short	m_sectorCount;					// Количество секторов
		unsigned short	m_secPerTrack;					// Количество секторов на дорожку (условно)
		unsigned short	m_trackCount;					// Количество треков (условно)

		uint16_t	m_currentSectorPos;				// Номер текущего сектора
		
		uint8_t		m_currentTrackPos;				// Номер текущего трека
		uint8_t		m_currentTrackSecPos;				// Номер текущего сектора на треке

		byte		m_diskSideCount;				// Количество сторон у дискеты
		
		unsigned int	m_diskRecCount;					// Количество записей (секторов / чанков) в диске

		byte		sector[1024];					// Буфер для чтения данных (сектор / чанк)
		byte		*sectorPtr;					// Указатель на буфер для чтения данных (сектор / чанк)

		Diskette	m_disk[MAX_DISK_RECORDS];			// Список секторов на дискете
		Diskette	*m_diskPtr;					// Указатель на список секторов на дискете

		AtrHeader	atrHeader[16];					// Заголовок примонтированной дискеты формата ATR (16 байт)
		ProHeader	proHeader[16];					// Заголовок примонтированной дискеты в формате PRO (16 байт)
		ProSectorData	proSectorData[12];				// Системная информация о секторе дискеты в формате PRO (12 байт)

		File		imageFile;					// Объект для чтения/записи образа диска
		String		_filePath;					// Текущий путь

		uint32_t	atxTrackPosFile[40];				// Расположение начала данных ATX треков в файле
		AtxDisk		atxDisk[40][3];					// Таблица соотношения запрашиваемого сектора по SIO <> Реальный Track

		AtxDuplicates	atxDuplicates[ATX_DUP_SIZE];			// Таблица на ATX_DUP_SIZE записей о дублях секторов
		uint8_t		atxDupPos;					// Текущая позиция в atxDuplicates

		uint8_t		currentHoleIndex;				// Где сейчас читали сектор 1 - первая половина дискеты, 2 - вторая
};

#endif
