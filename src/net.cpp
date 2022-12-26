#include "net.h"

Net::Net() {}

void Net::init(DispST7735 *disp, Message *winMsg) {
	_disp = disp;
	_winMsg = winMsg;

	WiFi.mode(WIFI_STA);
	WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
	WiFi.setHostname(hostname.c_str());

	aPos = 0;
	tryCount = 0;
	timeInited = false;
	WiFi.begin(ssid, password);

	_lastMin = 0;
	_lastRSSI = RSSI_NONE;

	rootPath = "/";
	renamePath = "";
	
}

bool Net::checkWiFi() {
	wl_status_t status = WiFi.status();
	if (status == WL_CONNECTED) {
		checkCount = 0;

#ifdef DEBUG
		LOG.println("WiFi connected");
		LOG.println("IP address: ");
		LOG.println(WiFi.localIP());

		LOG.println("Contacting Time Server");
#endif
		configTime(3600 * timeZone, daySaveTime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");

		timeCount = (60 * 1000) - 2000;		// через две секунды обновить время

		timeInited = true;

		ftpServer = new WiFiServer( FTP_CTRL_PORT );
		dataServer = new WiFiServer( FTP_DATA_PORT_PASV );

		ftpServer->begin();
		delay(10);
		dataServer->begin();
		delay(10);

	} else {
		if (checkCount > 10000) {
			checkCount = 0;

			if ((tryCount > 50) && (status == WL_NO_SSID_AVAIL || status == WL_CONNECT_FAILED || status == WL_DISCONNECTED)) {
				_disp->drawImage(posX, posY, 14, 8, none);

			} else {
				_disp->drawImage(posX, posY, 14, 8, aList[aPos]);
				aPos++;
				if (aPos == aSize) aPos = 0;

				tryCount++;
			}

		} else {
			checkCount++;
		}
	}
	return (status == WL_CONNECTED);
}

bool Net::update(bool force) {
	tmStruct.tm_year = 0;
	getLocalTime(&tmStruct, 5000);

	ntd.day = (byte)tmStruct.tm_mday;
	ntd.month = (byte)tmStruct.tm_mon;
	ntd.year = (byte)((tmStruct.tm_year + 1900) % 100);
	ntd.hour = (byte)tmStruct.tm_hour;
	ntd.minute = (byte)tmStruct.tm_min;
	ntd.second = (byte)tmStruct.tm_sec;

	// Раз в минуту
	if (_lastMin != ntd.minute || force) {

		_lastMin = ntd.minute;
		_disp->setBgColor(MENU_TOP_COLOR);

		String h = String(tmStruct.tm_hour);
		if (tmStruct.tm_hour < 10) h = "0" + h;
		String m = String(tmStruct.tm_min);
		if (tmStruct.tm_min < 10) m = "0" + m;

		_disp->drawFont6String(128-2-(5*6)+4, 2, h + ":", COLOR(255,255,255));
		_disp->drawFont6String(128-2-(2*6), 2, m, COLOR(255,255,255));

	}

	int _RSSI = WiFi.RSSI();
	byte RSSI = RSSI_NONE;

	if (_RSSI <= -30) {
		RSSI = RSSI_EXCELENT;
	
	} else if (_RSSI <= -67) {
		RSSI = RSSI_VERYGOOD;

	} else if (_RSSI <= -70) {
		RSSI = RSSI_OKAY;

	} else if (_RSSI <= -80) {
		RSSI = RSSI_NOTGOOD;

	} else if (_RSSI <= -90) {
		RSSI = RSSI_UNUSABLE;
	}

	if (_lastRSSI != RSSI || force) {
		_lastRSSI = RSSI;

		const uint8_t *img = none;

		if (RSSI == RSSI_EXCELENT) {
			img = excellent;
		} else if (RSSI == RSSI_VERYGOOD) {
			img = veryGood;
		} else if (RSSI == RSSI_OKAY) {
			img = okay;
		} else if (RSSI == RSSI_NOTGOOD) {
			img = notGood;
		} else if (RSSI == RSSI_UNUSABLE) {
			img = unusable;
		} else {
		}
		
		_disp->drawImage(posX, posY, 14, 8, img);
	}

	return checkFTP();
}

NetDateTime* Net::getDateTime() {
	return &ntd;
}

bool Net::isTimeInited() {
	return timeInited;
}

bool Net::checkFTP() {
	bool redraw = false;
	WiFiClient client = ftpServer->available();
	if (client) {

#ifdef DEBUG
		LOG.println("New Client.");
#endif
		_winMsg->showCustom(
			*_disp,
			" Freeze",
			"FTP mode is activated.\nInterface is frozen.\nDisconnect to continue\nor press RESET!",
			iconStop, false, false
		);

		rootPath = "/";
		String currentCmd = "";
		String currentArgs = "";
		bool isArgs = false;

		bool newConnect = true;
		while (client.connected()) {
			if (newConnect) {
				client.println("220 FTP server ready.");
				newConnect = false;
			}
			if (client.available()) {
				char c = client.read();
				if (c == '\n') {
					if (currentCmd.length() == 0) {
						break;

					} else {
						parseCmd(client, currentCmd, currentArgs);
						currentCmd = "";
						currentArgs = "";
						isArgs = false;
					}
				} else if (c == ' ') {
					if (!isArgs) {
						isArgs = true;
					} else {
						currentArgs += c;
					}
				} else if (c != '\r') {
					if (isArgs) {
						currentArgs += c;
					} else {
						currentCmd += c;
					}
				}
			}
		}

		client.stop();
#ifdef DEBUG
		LOG.println("Client disconnected.");
#endif
		redraw = true;
	}

	return redraw;
}

void Net::parseCmd(WiFiClient client, String currentCmd, String currentArgs) {

#ifdef DEBUG
	LOG.println("[FTP] CMD=" + currentCmd);
	LOG.println("[FTP] ARGS=" + currentArgs);
#endif

	WiFiClient data = dataServer->available();

	if (currentCmd == "USER") {
		if (currentArgs == ftpLogin) {
			client.println("331 " + currentArgs + " login ok");
		} else {
			client.println("100 wrong " + currentArgs + " login");
		}

	} else if (currentCmd == "PASS") {
		if (currentArgs == ftpPass) {
			client.println("230- Welcome to Astarta FTP");
			client.println("230 access granted, restrictions apply");
		
		} else {
			client.println("100 wrong password");
		}

	} else if (currentCmd == "CWD") {
		if (currentArgs == "..") {
			if (rootPath.length() > 0) {
				for (int i=rootPath.length(); i>=0; i--) {
					if (rootPath.charAt(i) == '/') {
						if (i == 0) {
							rootPath = "/";
						} else {
							rootPath = rootPath.substring(0,i);
						}
						break;
					}
				}
			} else {
				rootPath = "/";
			}

		} else {
			if (rootPath == "/") {
				rootPath += currentArgs;
			} else {
				rootPath += "/" + currentArgs;
			}
		}

		client.println("250 CWD command successful");

	} else if (currentCmd == "PWD") {
		client.println("257 \"" + rootPath + "\" is the current directory");

	} else if (currentCmd == "NLST") {
		char buf[255];
		rootPath.toCharArray(buf, 255);
		File root = SD.open(buf);
		if(!root){
			client.println("550 " + rootPath + ": No such file or directory.");

		} else if (!root.isDirectory()) {
			client.println("550 " + rootPath + ": No such file or directory.");

		} else {
			client.println("150 Opening BINARY mode data connection for file list");
			File file = root.openNextFile();
			while(file) {
				data.println(file.name());
				file = root.openNextFile();
			}
			file.close();
			client.println("226 Transfer complete");
		}

	} else if (currentCmd == "LIST") {

		char buf[255];
		rootPath.toCharArray(buf, 255);
		File root = SD.open(buf);
		if(!root){
			client.println("550 " + rootPath + ": No such file or directory.");

		} else if (!root.isDirectory()) {
			client.println("550 " + rootPath + ": No such file or directory.");

		} else {
			client.println("150 Opening BINARY mode data connection for file list");
			File file = root.openNextFile();
			while(file) {
				String line = "";
				time_t t= file.getLastWrite();
				struct tm * tmstruct = localtime(&t);
				String fDate = mNames[tmstruct->tm_mon] + " " + String(tmstruct->tm_mday) + " " + String((tmstruct->tm_year)+1900) + " " + String(tmstruct->tm_hour) + ":" + String(tmstruct->tm_hour);

				if(file.isDirectory()){
					line = "drwxrwsr-x 2 user group " + String(long(4096)) + " " + fDate + " " + String(file.name());
				} else {
					line = "-rw-rw-r-- 1 user group " + String(file.size()) + " " + fDate + " " + String(file.name());
				}

				data.println(line);

				file = root.openNextFile();
			}
			file.close();
			client.println("226 Transfer complete");
		}

	} else if (currentCmd == "TYPE") {
		if (currentArgs == "I") {
			client.println("200 Type set to binary mode");

		} else if (currentArgs == "A") {
			client.println("200 Type set to ASCII mode");
		}

	} else if (currentCmd == "PASV") {
		String s = WiFi.localIP().toString().c_str();
		s.replace(".", ",");
		uint8_t p1 = FTP_DATA_PORT_PASV / 256;
		uint8_t p2 = FTP_DATA_PORT_PASV - (p1 * 256);
		client.println("227 Entering Passive Mode (" + s + "," + String(p1) + "," + String(p2) + ").");

	} else if (currentCmd == "SYST") {
		client.println("215 WIN");

	} else if (currentCmd == "FEAT") {
		client.println("211-Features:");
		client.println(" CWD");
		client.println(" DELE");
		client.println(" FEAT");
		client.println(" LIST");
		client.println(" MKD");
		client.println(" NLST");
		client.println(" PASS");
		client.println(" PASV");
		client.println(" QUIT");
		client.println(" RETR");
		client.println(" RNFR");
		client.println(" RNTO");
		client.println(" SIZE");
		client.println(" STOR");
		client.println(" SYST");
		client.println(" TYPE");
		client.println(" USER");
		client.println("211 End");

	// } else if (currentCmd == "HELP") {

	} else if (currentCmd == "RETR") {
		String filePath = rootPath + '/' + currentArgs;
		File f = SD.open(filePath, "r");
		if (!f) {
			client.println("550 " + filePath + ": Can't open file for read.");
		} else {
			client.println("150 Opening BINARY mode data connection for " + currentArgs + " (" + String(f.size()) + " bytes)");
			while (f.available()) {
				data.write(f.read());
			}
			f.close();
			client.println("226 Transfer complete");
		}

	} else if (currentCmd == "QUIT") {
		client.println("221 Goodbye.");

	} else if (currentCmd == "STOR") {
		String filePath = rootPath + '/' + currentArgs;
		File f = SD.open(filePath, "w");
		if (!f) {
			client.println("550 " + filePath + ": Can't open file for write.");
		} else {
			client.println("150 Starting data transfer.");

			delay(1000);

			while (data.available()) {
				f.write(data.read());
			}
			f.close();
			client.println("226 Operation successful");
		}

	} else if (currentCmd == "SIZE") {
		String filePath = rootPath + '/' + currentArgs;
		File f = SD.open(filePath, "r");
		if (!f) {
			client.println("550 " + filePath + ": No such file or directory.");
		} else {
			client.println("213 " + String(f.size()));
			f.close();
		}

	} else if (currentCmd == "DELE") {
		String filePath = rootPath + '/' + currentArgs;
	File f = SD.open(filePath, "r");
		if (!f) {
	  client.println("550 " + filePath + ": No such file or directory.");
	} else {
	  if (f.isDirectory()) {
		f.close();
		if(SD.rmdir(filePath)){
		  client.println("226 Operation successful");
		} else {
		  client.println("550 " + filePath + ": Delete failed");
		}
	  } else {
		f.close();
		if(SD.remove(filePath)){
		  client.println("226 Operation successful");
		} else {
		  client.println("550 " + filePath + ": Delete failed");
		}
	  }
	}

	} else if (currentCmd == "RNFR") {
		String filePath = rootPath + '/' + currentArgs;
		File f = SD.open(filePath, "r");
		if (!f) {
			client.println("550 " + filePath + ": No such file or directory.");
		} else {
			renamePath = filePath;
			client.println("350 File exist, ready for destination name.");
			f.close();
		}

	} else if (currentCmd == "RNTO") {
		String filePath = rootPath + '/' + currentArgs;
		if (!SD.rename(renamePath, filePath)) {
			client.println("550 " + renamePath + ": No such file or directory.");
		} else {
			renamePath = "";
			client.println("250 File or directory renamed successfully.");
		}

  } else if (currentCmd == "MKD") {
	String filePath = rootPath + '/' + currentArgs;
	if (!SD.mkdir(filePath)) {
			client.println("550 " + renamePath + ": Can't create directory.");
		} else {
			renamePath = "";
			client.println("250 File or directory created successfully.");
		}

	} else {
		client.println("500 " + currentCmd + " not understood");
	}

}
