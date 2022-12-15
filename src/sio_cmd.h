#ifndef SIO_CMD_h
#define SIO_CMD_h

#include <Arduino.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SIO COMMANDS
// SRC: https://www.atarimax.com/jindroush.atari.org/asio.html
// + own info
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const byte SIO_CMD_FORMAT_AUTO							= 0x20;		// Format Auto
																	// AUX1	Config Byte 1 (?)
																	// AUX2	Config Byte 2 (?)
																	// Speedy 1050: Format automatically, acts like $21,
																	// but computer doesn't have to wait 'till drive is ready

const byte SIO_CMD_FORMAT_DRIVE							= 0x21;		// Format Drive
																	// AUX1	n/a
																	// AUX2	n/a
																	// Single for 810 and stock 1050 or as defined by CMD $4F for upgraded
																	// drives (Speedy, Happy, HDI, Black Box, XF 551).
																	// Returns 128 bytes (SD) or 256 bytes (DD) after format. Format ok requires that
																	// the first two bytes are $FF. It is said to return bad sector list (never seen one).

const byte SIO_CMD_FORMAT_MEDIUM_DENSITY				= 0x22;		// Format Medium Density
																	// AUX1	n/a
																	// AUX2	n/a
																	// Formats medium density on an Atari 1050.
																	// Format medium density cannot be achieved via PERCOM block settings!

const byte SIO_CMD_SERVICE								= 0x23;		// Service
																	// AUX1	n/a
																	// AUX2	n/a
																	// Atari 1050 Diagnostic Software uses this. Don't use it yourself, you may lose data.

const byte SIO_CMD_DIAGNOSTIC							= 0x24;		// Diagnostic
																	// AUX1	n/a
																	// AUX2	n/a
																	// Atari 1050 Diagnostic Software uses this. Don't use it yourself, you may lose data.

const byte SIO_CMD_GET_HIGH_SPEED_INDEX					= 0x3f;		// Get high-speed-index
																	// AUX1	n/a
																	// AUX2	n/a
																	// Returns a single byte
																	// Happy: Returns $0A
																	// US-Doubler: Returns $0A
																	// HDI: Returns $0A
																	// Speedy: Returns $09
																	// Toms 720: Returns $06 (?)
																	// Drive							Speed	Cmd	Data
																	// All devices						19120	$28	$28
																	// XF551 High Speed					39000	$28	$10
																	// US-Doubler & compatibles (Happy?)52000	$0A	$0A
																	// Speedy							57000	$09	$09
																	// IndusGT Syncromesh				72000	$28	$06
																	// XF551 & Indus have bit 7 of command byte set for highspeed data frame.

const byte SIO_CMD_DEVPOOL								= 0x40;		// Команда отправки спец.команд устройству

const byte SIO_CMD_ADD_REMOVE_COMMAND					= 0x41;		// Add/Remove Command
																	// AUX1	n/a
																	// AUX2	n/a
																	// Speedy 1050, HDI: Sends 3 bytes to drive. First byte is new command.
																	// Bytes 2 and 3 are the address in Speeder RAM. Add/remove command to/from internal
																	// command table. Existing command will be overwritten. Use address of $0000
																	// to erasecommand.

const byte SIO_CMD_CONFIGURE_DRIVE						= 0x44;		// Configure Drive
																	// AUX1	Configuration Byte
																	// AUX2	n/a
																	// Bit		Description
																	// Bit 0	ERROR beep ON
																	// Bit 1	unused
																	// Bit 2	unused
																	// Bit 3	Don't stop drive motor upon $51 cmd
																	// Bit 4	Don't write VTOC and Bootsector upon $20 cmd
																	// Bit 5	Format without verify
																	// Bit 6	HEX display
																	// Bit 7	allow ERROR display

const byte SIO_CMD_HAPPY_COMMAND						= 0x48;		// Happy Command
																	// AUX1	$01
																	// AUX2	$00-$FF
																	// Set motor off delay from 0 to 6.5 seconds. Default is 3 seconds.
																	// AUX1	$02
																	// AUX2	$Bx
																	// Set new drive number
																	// You should check before if this potential drive already exists using STATUS
																	// AUX1	$03
																	// AUX2	$00
																	// Clears all Software settings. Reads disk density.
																	// AUX1	$18
																	// AUX2	$10
																	// Software write protect ON
																	// AUX1	$18
																	// AUX2	$08
																	// Software write protect OFF
																	// AUX1	$20
																	// AUX2	$00
																	// Drive uses command $50 instead of $57
																	// AUX1	$60
																	// AUX2	$60
																	// Set RAM protect. Drive does not use RAM for track buffers or something like this.
																	// AUX1	$00
																	// AUX2	$80
																	// Set RAM free. Allows RAM usage for track buffering.
																	// AUX1	$E0
																	// AUX2	$E0
																	// Set unhappy mode. Disable all speeder functions but DD.

const byte SIO_CMD_SLOW_FAST_CONFIG						= 0x4b;		// Slow/Fast Config
																	// AUX1	Configuration Byte
																	// AUX2	n/a
																	// Bit		Description
																	// Bit 0	Read sector slow
																	// Bit 1	Write sector slow
																	// Bit 2	Verify OFF
																	// Bit 3	Drive slow
																	// Bit 4	unused
																	// Bit 5	unused
																	// Bit 6	read 1 track slow (resets when track changes)
																	// Bit 7	read 1 disk slow (resets when disk is changed)

const byte SIO_CMD_JUMP_WITHOUT_MESSAGE					= 0x4c;		// 	Jump without Message
																	// AUX1	Address LOW
																	// AUX2	Address HIGH
																	// Speedy 1050: Drive jumps to program address in its memory.

const byte SIO_CMD_JUMP_MESSAGE							= 0x4d;		// Jump with Message
																	// AUX1	Address LOW
																	// AUX2	Address HIGH
																	// Speedy 1050: Like $4C, but drive acknowledges command prior execution (sends COMPLETE).

const byte SIO_CMD_READ_PERCOM_BLOCK					= 0x4e;		// Read PERCOM Block
																	// AUX1	n/a
																	// AUX2	n/a
																	// Happy, Speedy, F2000, HDI, XF551, BB: Reads 12 bytes from drive.
																	// Offset	Description
																	// $00		Number of tracks
																	// $01		Step rate (00=30ms 01=20ms 10=12ms 11=6ms)
																	// $02		Sectors per Track HIGH
																	// $03		Sectors per Track LOW
																	// $04		Number of sides decreased by one
																	// $05		Record Method (0=FM single, 4=MFM double, 6=1.2M)
																	// $06		Bytes per Sector HIGH
																	// $07		Bytes per Sector LOW
																	// $08		Drive online ($FF or $40 for XF551)
																	// $09		Unused (Serial rate control?)
																	// $0A		Unused
																	// $0B		Unused
																	// For harddrives or ramdisks, track equals one and byte 2 and 3 are no. of sectors
																	// per disk (minus 1 on MIO board and Black Box)
																	// I suggest ramdisks return $00 for tracks

const byte SIO_CMD_WRITE_PERCOM_BLOCK					= 0x4f;		// Write PERCOM Block
																	// AUX1	n/a
																	// AUX2	n/a
																	// Same definition as in $4E. Send this command prior format ($21) to choose how
																	// to format a disk.

const byte SIO_CMD_WRITE_SECTOR							= 0x50;		// 	Write Sector
																	// AUX1	Sector LOW
																	// AUX2	Sector HIGH
																	// Sends 128 or 256 (or more) bytes to drive and writes sector w/o verify.

const byte SIO_CMD_QUIT									= 0x51;		// Quit
																	// AUX1	n/a
																	// AUX2	n/a
																	// Speedy 1050, HDI: Writes buffered data to disk and stops motor

const byte SIO_CMD_READ_SECTOR							= 0x52;		// Read Sector
																	// AUX1	Sector LOW
																	// AUX2	Sector HIGH
																	// Reads 128 or 256 (or more) bytes from drive.

const byte SIO_CMD_GET_STATUS							= 0x53;		// Get Status
																	// AUX1	n/a
																	// AUX2	n/a
																	// Gets status information from intelligent peripheral. Reads 4 bytes
																	// Offset	Description
																	// $00		Drive status
																	// 			bit 0	Command frame error
																	// 			bit 1	Checksum error (Data frame error).
																	// 			bit 2	Write Error (Operation error/FDC error)
																	// 			bit 3	Write protected
																	// 			bit 4	Motor is ON
																	// 			bit 5	Sector size (0=$80 1=$100)
																	// 			bit 6	Unused
																	// 			bit 7	Medium Density (MFM & $80 byte mode)
																	// $01	Inverted, contains WD2793 Status Register. Depends on command used prior status.
																	// 			bit 0	Controller busy
																	// 			bit 1	Data Request or Index (DRQ)
																	// 			bit 2	Data lost or track 0
																	// 			bit 3	CRC Error
																	// 			bit 4	Record not found
																	// 			bit 5	Record Type or Head Loaded
																	// 			bit 6	Write Protected (Always false upon read)
																	// 			bit 7	Not Ready (Disk Removed)
																	// $02	Timeout for format ($E0) - Time the drive will need to format a disk
																	// $03	Copy of WD2793 Master status register

const byte SIO_CMD_READ_MEMORY							= 0x54;		// 	Read Memory
																	// AUX1	Address LOW
																	// AUX2	Address HIGH
																	// HDI: Reads 256 bytes of memory. In case you try to read the HDI hardware
																	// registers ($4000 to $4FFF) it may crash.

const byte SIO_CMD_GET_DRIVE_VARIABLES					= 0x54;		// 	Get drive variables
																	// AUX1	?
																	// AUX2	?
																	// Super Archiver: Sends 00-7F variables.

const byte SIO_CMD_MOTOR_ON								= 0x55;		// Motor ON
																	// AUX1	?
																	// AUX2	?

const byte SIO_CMD_VERIFY_SECTOR						= 0x56;		// Verify Sector
																	// AUX1	?
																	// AUX2	?

const byte SIO_CMD_WRITE_SECTOR_WITH_VERIFY				= 0x57;		// Write Sector with Verify
																	// AUX1	Sector LOW
																	// AUX2	Sector HIGH
																	// Writes Sector to drive, drive rereads the sector, but doesn't send the reread
																	// sector back to the computer

const byte SIO_CMD_WRITE_TRACK							= 0x60;		// Write Track
																	// AUX1	First Sector
																	// AUX2	Last Sector
																	// Speedy 1050: Writes multiple sectors. Amount of data equals # sectors by sector size.

const byte SIO_CMD_READ_TRACK							= 0x62;		// 	Read Track
																	// AUX1	First Sector
																	// AUX2	Last Sector
																	// Speedy 1050: Reads multiple sectors. Amount of data equals # sectors by sector size.

const byte SIO_CMD_FORMAT_DISK_WITH_SPECIAL_SECTOR_SKEW	= 0x66;		// Format Disk with Special Sector-Skew
																	// AUX1	n/a
																	// AUX2	n/a
																	// US-Doubler, HDI: Sends 128 byte of data to the floppy. The first 12 bytes are
																	// the Percom block. The next bytes (as many as there are sectors per track)
																	// are the sector numbers in the order you would like them to be formatted.
																	// Filler bytes must be zeroes.
																	// Example for SSSD:
																	// 28 02 00 12 00 00 00 80 FF 00 00 00 12 10 0E 0C
																	// 0A 08 06 04 02 11 0F 0D 0B 09 07 05 03 01 00 00
																	// 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
																	// 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
																	// 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
																	// 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
																	// 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
																	// 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

const byte SIO_CMD_GET_SIO_LENGTH						= 0x68;		// Get SIO Length
																	// AUX1	n/a
																	// AUX2	n/a
																	// Speedy 1050, HDI: Returns 2 byte value matching the length of the SIO
																	// routine which you can read from the drive using command $69.

const byte SIO_CMD_GET_SIO_ROUTINE						= 0x69;		// Get SIO Routine
																	// AUX1	Relocate Address LOW
																	// AUX2	Relocate Address HIGH
																	// Speedy 1050, HDI: Reads high speed SIO routine from Speedy 1050 relocated
																	// to any address you wish. You can use this routine on your XL/XE to perform
																	// highspeed I/O. Use this routine instead of $E459.
																	// LDA $300	; wanna talk to disk?
																	// AND #$F0
																	// CMP #$30
																	// BEQ NEWSIO	; Yes
																	// JMP $E459	; No
																	// NEWSIO ...

const byte SIO_CMD_RETURN_PREPARED_BUFFER				= 0x74;		// Return prepared buffer
																	// AUX1	?
																	// AUX2	?
																	// Super-Archiver: Returns buffer as set by previous $75 command(?)

const byte SIO_CMD_UPLOAD_AND_EXECUTE_CODE				= 0x75;		// 	Upload & execute code
																	// AUX1	?
																	// AUX2	?
																	// Super-Archiver: Writes code to $1000 and executes it.

const byte SIO_CMD_NOTIFY_RUN							= 0xfd;		// Команда информирования о запуске кода
const byte SIO_CMD_CHUNK_DATA							= 0xfe;		// Команда получить данные чанка
const byte SIO_CMD_CHUNK_INFO							= 0xff;		// Команда получить информацию о чанке

#endif
