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
const unsigned short MAX_CHUNK_SIZE		= 1024;		// Максимальный размер чанка

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
	byte	unused_0			: 1;		// bit 0 Не задействован
	byte	unused_1			: 1;		// bit 1 Не задействован
	byte	writeError			: 1;		// bit 2 Неудачное выполнение команды PUT
	byte	writeProtected		: 1;		// bit 3 Ошибка из-за защиты от записи
	byte	motorRunning		: 1;		// bit 4 Мотор вращается
	byte	unused_5			: 1;		// bit 5 Не задействован
	byte	unused_6			: 1;		// bit 6 Не задействован
	byte	unused_7			: 1;		// bit 7 Не задействован
};

// Состояние контроллера дисковода гибких дисков (инвертировано из FDC)
struct  ControllerStatus {
	byte	busy				: 1;		// bit 0 Устройство занято
	byte	dataPending			: 1;		// bit 1 Ожидание данных
	byte	lostData			: 1;		// bit 2 Данные утеряны
	byte	crcError			: 1;		// bit 3 Ошибка контрольной суммы
	byte	recordNotFound		: 1;		// bit 4 Запись не найдена (ошибка сектора)
	byte	deletedSector		: 1;		// bit 5 Удалённый сектор (сектор отмечен как удаленный в заголовке сектора)
	byte	writeProtectError	: 1;		// bit 6 Ошибка защита от записи
	byte	notReady			: 1;		// bit 7 Нет диска в приводе

};

// Блок ответа статуса
struct StatusFrame {
	DriveStatus			drvStatus;			// Статус устройства
	ControllerStatus	ctrlStatus;			// Статус контроллера
	byte				defaultTimeout;		// $E0 Время ожидания по умолчанию ($E0 = 224 vertical blanks)
	byte				unused;				// $00 Не задействован
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Структура данных для чанков
struct Diskette {
	byte				side;				// Номер стороны диска
	byte				track;				// Номер трека стороны
	unsigned short		sector;				// Номер сектора трека
	unsigned int		offset;				// Смещение относительно файла с образом
	unsigned short		loadAddr;			// Адрес загрузки (только для чанков)
	unsigned short		loadSize;			// Размер загружаемых данных в байтах (сектор/чанк)
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Блок флагов образа диска
struct DiskFlags  {
	byte	unused_0			: 1;		// bit 0 Не задействован
	byte	unused_1			: 1;		// bit 1 Не задействован
	byte	unused_2			: 1;		// bit 2 Не задействован
	byte	unused_3			: 1;		// bit 3 Не задействован
	byte	copyProtected		: 1;		// bit 4 Защищён от копирования (имеются сектора с ошибками)
	byte	writeProtected		: 1;		// bit 5 Защищён от записи
	byte	unused_6			: 1;		// bit 6 Не задействован
	byte	unused_7			: 1;		// bit 7 Не задействован
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Заголовок дискеты в формате Atr
struct AtrHeader  {
	unsigned short	signature;				// +0x00: 2 байта: Идентификатор формата дискеты 0x0296
	unsigned short	diskSize;				// +0x02: 2 байта: Размер образа диска. Размер указывается в «параграфах»
	unsigned short	secSize;				// +0x04: 2 байта: Размер сектора. 0x80 - 128 байт, 0x100 — 256байт
	unsigned short	highPartSize;			// +0x06: 2 байта: Размер старшей части в параграфах. Добавлено в rev.3.00
	DiskFlags		diskFlags;				// +0x08: 1 байт: Флаги образа диска
	unsigned short	firstBadSector;			// +0x09: 2 байта: Номер первого сектора с ошибками
	byte			unused_0;				// +0x0b: 1 байт: Не задействован
	byte			unused_1;				// +0x0c: 1 байт: Не задействован
	byte			unused_2;				// +0x0d: 1 байт: Не задействован
	byte			unused_3;				// +0x0e: 1 байт: Не задействован
	byte			unused_4;				// +0x0f: 1 байт: Не задействован
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Заголовок дискеты в формате Pro (http://www.whizzosoftware.com/sio2arduino/prosys.html)
struct ProHeader  {
	unsigned short	imageSizeSec;			// +0x00: 2 байта (BigIndian): Количество секторов, содержащихся в данных файла, включая фантомы
	char			signature;				// +0x02: 1 байта: Идентификатор формата дискеты "P"
	char			version;				// +0x03: 1 байта: Версия формата дискеты "3" (возможны варианты "2")
	byte			emuPhantomSecor;		// +0x04: 1 байт: Режим эмуляции фантомных секторов:
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
	byte			delay;					// +0x05: 1 байт: Задержка между чтением каждого сектора, выраженная в 1/60 секунды (значение может быть от 1 до 99)
	unsigned short	diskSizeSec;			// +0x06: 2 байта: (BigIndian) Исходное количество секторов диска. (не больше 1040) (актуально только для версии 3)
	byte			unused_0;				// +0x08: 1 байт: Не задействован  
	byte			unused_1;				// +0x09: 1 байт: Не задействован
	byte			unused_2;				// +0x0a: 1 байт: Не задействован
	byte			unused_3;				// +0x0b: 1 байт: Не задействован
	byte			unused_4;				// +0x0c: 1 байт: Не задействован
	byte			unused_5;				// +0x0d: 1 байт: Не задействован
	byte			unused_6;				// +0x0e: 1 байт: Не задействован
	byte			unused_7;				// +0x0f: 1 байт: Не задействован
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Структура сектора в формате Pro
struct ProSectorData {
	DriveStatus			drvStatus;				// +0x00: 1 байт: Статус привода
	ControllerStatus	ctrlStatus;				// +0x01: 1 байт: Статус контроллера
	unsigned short		timeout;				// +0x02: 2 байт: Таймаут (1771 controller status)

	byte				unused_0;				// +0x04: 1 байт: Не задействован (всегда 0)

	byte				totalPhantomSecAss;		// +0x05: 1 байт: Общее количество фантомных секторов, связанных с этим сектором (значения могут быть от 0 до 5, поскольку ProSys выполняет 5 проходов чтения на сектор)

	byte				idxPhantomSector4;		// +0x06: 1 байт: Индекс фантомного сектора (PHANTOM4) начиная с сектора 721 (1-индексный) 
	byte				idxPhantomSector1;		// +0x07: 1 байт: Индекс фантомного сектора (PHANTOM1) начиная с сектора 721 (1-индексный)
	byte				idxPhantomSector2;		// +0x08: 1 байт: Индекс фантомного сектора (PHANTOM2) начиная с сектора 721 (1-индексный)  
	byte				idxPhantomSector3;		// +0x09: 1 байт: Индекс фантомного сектора (PHANTOM3) начиная с сектора 721 (1-индексный)

	byte				secStatus;				// +0x0a: 1 байт: Неизвестно, возможны варианты:                                                      
												//					$E4 (228) Bad sectors sometimes have this value
												//					$EC (236) Bad or CRC error sectors sometimes have this value
												//					$F0 (240) Normal sectors sometimes have this value
												//					$F8 (248) Normal sectors sometimes have this value

	byte				idxPhantomSector5;		// +0x0b: 1 байт: Индекс фантомного сектора (PHANTOM5) начиная с сектора 721 (1-индексный)  
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class vDrive {
	public:
		vDrive();
		void			init(byte vDriveId);
		void			resetDevice();					// Сброс статусов привода в исходное состояние

		unsigned short 	getSectorSize();				// Получить размер сектора
		StatusFrame*	getStatusFrame();				// Получить указатель на блок данных статуса устройства
		byte*			getSectorData(int sectorPos);	// Получить указатель на блок данных сектора
  
		// void			mountTest();
		bool			mountImage(String filePath, byte imageType);	// Подключить образ дискеты, типа
		bool			mountXex(String filePath);		// Подключить исполняемый файл как образ

		unsigned short 	getImgHeaderSize(byte imageType);
		unsigned short	getImgSectorInfoSize(byte imageType);

		Diskette		getRecordInfo(unsigned int recId);
		byte*			getRecordData(unsigned int recId);

		unsigned int 	getDiskRecCount();

		byte			getMountedImgType();
		void			setSectorData(unsigned short sectorPos, byte* sectorData);

	private:
		int				m_vDriveId;						// ID Привода
		StatusFrame		m_statusFrame;					// Блок данных (4 байта) ответа статуса устройства

		int				m_fileSize;						// Размер файла
		int				m_imageSize;					// Размер образа
		byte			m_imageType;					// Тип образа
		unsigned short	m_imageHeaderSize;				// Размер заголовка образа

		unsigned short  m_sectorSize;					// Размер сектора
		unsigned short  m_sectorInfoSize;				// Размер блока доп.данных для сектора

		unsigned short	m_sectorCount;					// Количество секторов
		unsigned short	m_secPerTrack;					// Количество секторов на дорожку (условно)
		unsigned short	m_trackCount;					// Количество треков (условно)

		int				m_currentSectorPos;				// Номер текущего сектора
		int				m_currentTrackPos;				// Номер текущего трека

		byte			m_diskSideCount;				// Количество сторон у дискеты
		
		unsigned int	m_diskRecCount;					// Количество записей (секторов / чанков) в диске

		byte			sector[1024];					// Буфер для чтения данных (сектор / чанк)
		byte			*sectorPtr;						// Указатель на буфер для чтения данных (сектор / чанк)

		Diskette		m_disk[MAX_DISK_RECORDS];		// Список секторов на дискете
		Diskette		*m_diskPtr;						// Указатель на список секторов на дискете

		AtrHeader		atrHeader[16];					// Заголовок примонтированной дискеты формата ATR (16 байт)
		ProHeader		proHeader[16];					// Заголовок примонтированной дискеты в формате PRO (16 байт)
		ProSectorData	proSectorData[12];				// Системная информация о секторе дискеты в формате PRO (12 байт)

		File			imageFile;						// Объект для чтения/записи образа диска
		String			_filePath;						// Текущий путь

};

#endif
