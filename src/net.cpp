#include "net.h"

Net::Net() {}

void Net::init(DispST7735	*disp) {
	_disp = disp;

	WiFi.mode(WIFI_STA);
	WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
	WiFi.setHostname(hostname.c_str());

	aPos = 0;
	tryCount = 0;
	timeInited = false;
	WiFi.begin(ssid, password);

	_lastMin = 0;
	_lastRSSI = RSSI_NONE;
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

void Net::update(bool force) {
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
}

NetDateTime* Net::getDateTime() {
	return &ntd;
}

bool Net::isTimeInited() {
	return timeInited;
}
