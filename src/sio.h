#ifndef SIO_h
#define SIO_h

#include <Arduino.h>
#include "vDrive.h"
#include "dev.h"

#include "dispST7735.h"
#include "net.h"

#include "sio_cmd.h"

typedef struct { 
	byte	id;
	String	dis;
} HashStruct;

const HashStruct devNames[] {
	{DEV_D1,	"Disk Drive D1"},
	{DEV_D2,	"Disk Drive D2"},
	{DEV_D3,	"Disk Drive D3"},
	{DEV_D4,	"Disk Drive D4"},
	{DEV_D5,	"Disk Drive D5"},
	{DEV_D6,	"Disk Drive D6"},
	{DEV_D7,	"Disk Drive D7"},
	{DEV_D8,	"Disk Drive D8"},
	{DEV_D9,	"Disk Drive D9"}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const byte SEND_ACK			= 0x41;	// Отправить код данные получены
const byte SEND_NACK			= 0x4e;	// Отправить код ошибка, повторить отправку
const byte SEND_COMPLETE		= 0x43;	// Отправить код приём завершен
const byte SEND_ERR			= 0x45;	// Отправить код ошибка чтения диска

const uint16_t DELAY_ACK		= 5;	// Задержка(Mc) при отправке ACK (иначе ATARI не успевает принять команду)
const uint16_t DELAY_COMPLETE		= 5;	// Задержка(Mc) при отправке COMPLETE
const uint16_t DELAY_ERR		= 2000;	// Задержка(Mc) при отправке ERR

const unsigned long READ_CMD_TIMEOUT	= 500;	// Если в течение этого времени данные не переданы полностью,
						// прекратить приём и перейти в режим ожидания

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Команды устройства APE
const byte APE_GET_TIMEDATE		= 0x93;
const byte APE_SUBMIT_URL		= 0x55;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const HashStruct sioCommands[] {
	{SIO_CMD_FORMAT_AUTO,				"Format Auto"},
	{SIO_CMD_FORMAT_DRIVE,				"Format Drive"},
	{SIO_CMD_FORMAT_MEDIUM_DENSITY,			"Format Medium Density"},
	{SIO_CMD_SERVICE,				"Service"},
	{SIO_CMD_DIAGNOSTIC,				"Diagnostic"},
	{SIO_CMD_GET_HIGH_SPEED_INDEX,			"Get high-speed-index"},
	{SIO_CMD_DEVPOOL,				"Bus poll"},
	{SIO_CMD_ADD_REMOVE_COMMAND,			"Add/Remove Command"},
	{SIO_CMD_CONFIGURE_DRIVE,			"Configure Drive"},
	{SIO_CMD_HAPPY_COMMAND,				"Happy Command"},
	{SIO_CMD_SLOW_FAST_CONFIG,			"Slow/Fast Config"},
	{SIO_CMD_JUMP_WITHOUT_MESSAGE,			"Jump without Message"},
	{SIO_CMD_JUMP_MESSAGE,				"Jump with Message"},
	{SIO_CMD_READ_PERCOM_BLOCK,			"Read PERCOM Block"},
	{SIO_CMD_WRITE_PERCOM_BLOCK,			"Write PERCOM Block"},
	{SIO_CMD_WRITE_SECTOR,				"Write Sector"},
	{SIO_CMD_QUIT,					"Quit"},
	{SIO_CMD_READ_SECTOR,				"Read Sector"},
	{SIO_CMD_GET_STATUS,				"Get Status"},
	{SIO_CMD_READ_MEMORY,				"Read Memory or Get drive variables"},
	{SIO_CMD_MOTOR_ON,				"Motor ON"},
	{SIO_CMD_VERIFY_SECTOR,				"Verify Sector"},
	{SIO_CMD_WRITE_SECTOR_WITH_VERIFY,		"Write Sector with Verify"},
	{SIO_CMD_WRITE_TRACK,				"Write Track"},
	{SIO_CMD_READ_TRACK,				"Read Track"},
	{SIO_CMD_FORMAT_DISK_WITH_SPECIAL_SECTOR_SKEW,	"Format Disk with Special Sector-Skew"},
	{SIO_CMD_GET_SIO_LENGTH,			"Get SIO Length"},
	{SIO_CMD_GET_SIO_ROUTINE,			"Get SIO Routine"},
	{SIO_CMD_RETURN_PREPARED_BUFFER,		"Return prepared buffer"},
	{SIO_CMD_UPLOAD_AND_EXECUTE_CODE,		"Upload & execute code"},
	{SIO_CMD_NOTIFY_RUN,				"[XEX] Notify run"},
	{SIO_CMD_CHUNK_DATA,				"[XEX] Chunk data"},
	{SIO_CMD_CHUNK_INFO,				"[XEX] Chunk info"}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const byte SIO_READY		= 1;	// Готовность к приёму комманды
const byte SIO_WAIT_CMD_START	= 2;	// Ожидание начала команды
const byte SIO_READ_CMD		= 3;	// Чтение команды
const byte SIO_READ_DATA	= 4;	// Чтение данных
const byte SIO_WAIT_CMD_END	= 5;	// Ожидание завершения команды

const static char *sioStateMsg[] = {
	"Ready",
	"Wait for command start",
	"Read command",
	"Read data",
	"Wait for command end"
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Блок команды (со стороны ATARI)
struct CommandFrame {
	byte devId;	// Идентефикатор устройства
	byte cmdId;	// Код команды
	byte aByte1;	// Вспомогательный байт 1
	byte aByte2;	// Вспомогательный байт 2
	byte crc;	// Контрольная сумма фрейма
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SIO {
	public:
		SIO();

#ifdef ENABLE_WIFI
	void	init(DispST7735	*disp, Net *n, uint16_t mColorNum, uint16_t mColorBg, uint16_t mColorSecR, uint16_t mColorSecW, uint16_t mColorSecS, uint16_t mColorSecF, uint16_t mColorDP, uint16_t mColorCI, uint16_t mColorCD, uint16_t mColorNR, uint16_t mColorD);
#else
	void	init(DispST7735	*disp, uint16_t mColorNum, uint16_t mColorBg, uint16_t mColorSecR, uint16_t mColorSecW, uint16_t mColorSecS, uint16_t mColorSecF, uint16_t mColorDP, uint16_t mColorCI, uint16_t mColorCD, uint16_t mColorNR, uint16_t mColorD);
#endif
    void  setSize(uint8_t w, uint8_t h);
		void		update();				// Обработка статуса
		void		incomingByte();				// Байт входящих данных
		bool		mountImageD1(String filePath);
		bool		mountImageD2(String filePath);
		bool		mountImageD3(String filePath);
		bool		mountImageD4(String filePath);
		
		void		unMountImageD1();
		void		unMountImageD2();
		void		unMountImageD3();
		void		unMountImageD4();

		bool		isBusy();

	private:

		DispST7735	*_disp;

		uint8_t		_width;
		uint8_t		_height;

		uint16_t	_mColorNum;
		uint16_t	_mColorBg;

		uint16_t	_mColorSecR;
		uint16_t	_mColorSecW;
		uint16_t	_mColorSecS;
		uint16_t	_mColorSecF;
		uint16_t	_mColorDP;
		uint16_t	_mColorCI;
		uint16_t	_mColorCD;
		uint16_t	_mColorNR;
		uint16_t	_mColorD;

		uint8_t		_xPos;
		uint8_t		_yPos;

#ifdef ENABLE_WIFI
		Net		*_n;
#endif

		vDrive		vd1;
		vDrive		vd2;
		vDrive		vd3;
		vDrive		vd4;

		unsigned short	m_currentSector;		// Текущий сектор (чтения/запись);
		unsigned short	m_currentSectorSize;		// Размер текущего сектора

		byte		m_dataBuffer[1024];		// Буфер для получения данных cо стороны ATARI
		byte		*m_dataBufferPtr;		// Указатель на буфер для получения данных cо стороны ATARI
		byte		m_dataBufferCrc;		// Контрольная сумма буфера данных

		byte		m_sioState;			// Текущее состояние SIO

		int		m_timeReceivedByte;		// Время последнено принятого байта

		CommandFrame	m_cmdBuffer;			// Буфер для приёма данных команды
		byte*		m_cmdBufferPtr;			// Указатель на начала буфера
		byte		m_cmdBufferCrc;			// Контрольная сумма буфера

		bool		_isDataSending = false;
		String		_lastStatusMsg;


		void		checkCmdPinHigh();		// Проверка состояния пина Command (HIGH)
		void		checkCmdPinLow();		// Проверка состояния пина Command (LOW)
		void		clearCmdBuffer();		// Подготовка буфера комманды к приёму данных
		void		checkReadCmdComplete();		// Проверка окончания приёма данных команды
		void		changeState(byte state);	// Изменить текущее состояние SIO

		void		calcCmdDataCrc();		// Расчёт контрольной суммы блока команды
		byte		calcCrc(byte* chunk, int len);	// Расчёт контрольной суммы блока данных

		void		cmdProcessing();		// Обработка поступившей команды
		void		sendDevStatus();		// Вернуть статус устройства

		void		sendDevSector();		// Отправить ATARI сектор данных
		void		recieveDevSector();
		void		sendDevPool();
		void		sendDevChunkInfo();
		void		sendDevChunkData();
		void		formatDev();
		
		void		sendNotifyRun();

		void		sendBytes(unsigned short sectorSize, byte* pSectorData, bool isError);

		byte		getFileType(String pFileName);
		vDrive*		getVDriveById(byte devId);

		void		checkReadDataComplete();

		void		calcDataCrc();

		void		drawPos(unsigned short pos);
		void		drawStatus(String msg, uint16_t color);

		void		rCmdProcessing();

#ifdef ENABLE_WIFI
		void		sendApeDateTime();
#endif

};

#endif
