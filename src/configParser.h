#ifndef CONFIG_PARSER_h
#define CONFIG_PARSER_h

#include <Arduino.h>
#include <SD.h>

#define CONFIG_MAX	128

////////////////////////////////////////////////////////////////////////////////////////
struct  Dic {
	String	key;
	String	value;
};

class ConfigParser {
	public:
		ConfigParser();
		bool 	init(String configPath);
		String	getValue(String key);
  private:
		bool			_configInited;
		Dic				_config[CONFIG_MAX];
		unsigned int	_configLen;
};

#endif
