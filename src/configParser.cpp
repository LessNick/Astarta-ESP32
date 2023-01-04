#include "configParser.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Config Parser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ConfigParser::ConfigParser(){}

bool ConfigParser::init(String configPath) {
	_configInited = false;

	File configFile = SD.open(configPath);

	if (configFile) {
		uint8_t* pBuffer;
		unsigned int fileSize = configFile.size();
		pBuffer = (uint8_t*)malloc(fileSize + 1);
		configFile.read(pBuffer, fileSize);
		pBuffer[fileSize] = '\0';
		configFile.close();

		_configLen=0;

		bool keyReady = false;
		String key = "";
		String value = "";
		for (unsigned int i=0; i<fileSize; i++) {
			char c = (char)pBuffer[i];
			if (keyReady == false) {
				if (c == '=') {
					keyReady = true;
				} else {
					key += c;
				}
			} else {
				if (c == 0x0D || c == 0x0A || c == 0x00) {		
					_config[_configLen].key = key;
					_config[_configLen].value = value;
					_configLen++;
					key = "";
					value = "";
					keyReady = false;
					if (i+1 < fileSize) {
						if (pBuffer[i+1] == 0x0A) i++;
					}
				} else {
					value += c;
				}
			}

		}
		_configInited = true;
		free(pBuffer);
	}

	return _configInited;
}

String ConfigParser::getValue(String key) {
	String val = "";
	for (unsigned int i=0; i<_configLen; i++) {
		if (_config[i].key == key) {
			val = _config[i].value;
			break;
		}
	}
	return val;
}
