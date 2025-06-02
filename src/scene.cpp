#include "scene.h"
#include "font_16p.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Scene::Scene() {
	_disp.init(SCENE_COLOR_BG);
	_disp.setDispMode(ST7735_MADCTL_RGB);
	winMsg.init(SCENE_COLOR_BG, MESSAGE_COLOR, MESSAGE_COLOR_BG, MESSAGE_BUTTON_COLOR, MESSAGE_BUTTON_COLOR_BG, MESSAGE_BUTTON_COLOR_B1, MESSAGE_BUTTON_COLOR_B2, MESSAGE_BUTTON_COLOR_B3, MESSAGE_BUTTON_COLOR_B4);
	vr.init(&kbd, SCENE_COLOR_BG, MESSAGE_COLOR);
	nav.init(MAX_NAV_ITEMS);
	rootPath = "/";
	atariCMD = false;

#ifdef ENABLE_WIFI
	atariSio.init(&_disp, &_net, MENU_BOTTOM_COLOR_SEC_NUM, MENU_BOTTOM_COLOR_BG, MENU_BOTTOM_COLOR_SEC_R, MENU_BOTTOM_COLOR_SEC_W, MENU_BOTTOM_COLOR_SEC_S, MENU_BOTTOM_COLOR_SEC_F, MENU_BOTTOM_COLOR_DP, MENU_BOTTOM_COLOR_CI, MENU_BOTTOM_COLOR_CD, MENU_BOTTOM_COLOR_NR, MENU_BOTTOM_COLOR_D);
	netIniting = false;
	netInited = false;
#else
	atariSio.init(&_disp, MENU_BOTTOM_COLOR_SEC_NUM, MENU_BOTTOM_COLOR_BG, MENU_BOTTOM_COLOR_SEC_R, MENU_BOTTOM_COLOR_SEC_W, MENU_BOTTOM_COLOR_SEC_S, MENU_BOTTOM_COLOR_SEC_F, MENU_BOTTOM_COLOR_DP, MENU_BOTTOM_COLOR_CI, MENU_BOTTOM_COLOR_CD, MENU_BOTTOM_COLOR_NR, MENU_BOTTOM_COLOR_D);
#endif

	_forceUpdate = false;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::setSize(uint8_t w, uint8_t h) {
	_width  = w;
	_height = h;
	_disp.setSize(_width, _height);
	nav.setSize(_width, _height);
	winMsg.setSize(_width, _height);
	atariSio.setSize(_width, _height);
	vr.setSize(_width, _height);

#ifdef ENABLE_WIFI
	_net.setSize(_width, _height);
#endif

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::refresh() {
	int nAtariCMD = digitalRead(ATARI_CMD_PIN);
	if (atariCMD != nAtariCMD) {
		atariCMD = nAtariCMD;

#ifdef DEBUG
		LOG.println("Atari CMD Pin Status:" + String(atariCMD));
#endif

	}
	
  atariSio.update();

	while (ATARI_SIO.available()) {
		atariSio.incomingByte();
	}

	if (!atariSio.isBusy()) {
	
#ifdef ENABLE_WIFI
		if (currentScene != SCENE_START && currentScene != SCENE_REBOOT) {
      if (netInited == true && netIniting == false) {
				bool isExitFromFtp = _net.update(_forceUpdate);
				if(isExitFromFtp) {
					showMainMenu();
					_net.update(true);
				}
				_forceUpdate = false;
			}
    }

    if (currentScene == SCENE_CONNECT) {
			if (netInited == false && netIniting == false) {
				_net.init(&_disp, &winMsg, &_cp, MENU_TOP_COLOR_TIME, MENU_TOP_COLOR_BG);
				netInited = false;
				netIniting = true;

			} else if (netInited == false && netIniting == true) {
				if(_net.checkWiFi()) {
					netInited = true;
					netIniting = false;
				}
			
			} else if (netInited == true && netIniting == false) {
				bool isExitFromFtp = _net.update(_forceUpdate);
					showMainMenu();
					_net.update(true);
				_forceUpdate = false;
			}
		}
#endif

		uint8_t st = kbd.refresh();
		switch (currentScene) {
			case SCENE_START: {
				if (st != KEY_NONE) {
#ifdef DEBUG
					LOG.println("[ OK ]");
					LOG.print("Check SD Card... ");
#endif
					checkSD();
				}
				break;
			}
			case SCENE_REBOOT: {
				if (st != KEY_NONE) {
					ESP.restart();
				}
				break;
			}
			case SCENE_MAINMENU: {
				if (st == KEY_UP) {
					nav.cursorUp(_disp);

				} else if (st == KEY_DOWN) {
					nav.cursorDown(_disp);

				} else if (st == KEY_ACTION_1) {
					MenuList ml = nav.getCurrentItem();
					if (ml.mType == MTYPE_D1) {

					} else if (ml.mType == MTYPE_D2) {

					} else if (ml.mType == MTYPE_D3) {

					} else if (ml.mType == MTYPE_D4) {

					} else if (ml.mType == MTYPE_CAS) {
						if (ml.mPath != S_DEV_CAS && ml.mName != S_NO_DISK && ml.mColor != C_MITEM_EMPTY) {
							showStartCas();
							st = KEY_NONE;
						} else {
							showSelectCAS();
							st = KEY_NONE;
						};
					}

				} else if (st == KEY_ENTER) {
					
					MenuList ml = nav.getCurrentItem();
					if (ml.mType == MTYPE_D1) {
						showSelectD1();
					
					} else if (ml.mType == MTYPE_D2) {
						showSelectD2();
					
					} if (ml.mType == MTYPE_D3) {
						showSelectD3();
					
					} if (ml.mType == MTYPE_D4) {
						showSelectD4();
					
					} if (ml.mType == MTYPE_CAS) {
						showSelectCAS();
					
					} if (ml.mType == MTYPE_TOOLS) {
            					showTools();
					
					} if (ml.mType == MTYPE_SETTINGS) {
           					showSettings();
					}

				} else if (st == KEY_ESC) {
					MenuList ml = nav.getCurrentItem();
					if (ml.mType == MTYPE_D1 && ml.mName != S_NO_DISK && ml.mColor != C_MITEM_EMPTY) {
						showEjectD1();
					
					} else if (ml.mType == MTYPE_D2 && ml.mName != S_NO_DISK && ml.mColor != C_MITEM_EMPTY) {
						showEjectD2();
					
					} else if (ml.mType == MTYPE_D3 && ml.mName != S_NO_DISK && ml.mColor != C_MITEM_EMPTY) {
						showEjectD3();

					} else if (ml.mType == MTYPE_D4 && ml.mName != S_NO_DISK && ml.mColor != C_MITEM_EMPTY) {
						showEjectD4();

					} else if (ml.mType == MTYPE_CAS && ml.mName != S_NO_CAS && ml.mColor != C_MITEM_EMPTY) {
						showEjectCAS();
					}

				}
				break;
			}

			case SCENE_SELECT_D1: {
				if (st == KEY_UP) {
					nav.cursorUp(_disp);
				
				} else if (st == KEY_DOWN) {
					nav.cursorDown(_disp);
				
				} else if (st == KEY_ENTER) {
					MenuList ml = nav.getCurrentItem();
					showSelectProcessing(ml, S_DEV_D1);
				
				} else if (st == KEY_ESC) {
					showMainMenu();
				}
				break;
			}

			case SCENE_SELECT_D2: {
				if (st == KEY_UP) {
					nav.cursorUp(_disp);
				
				} else if (st == KEY_DOWN) {
					nav.cursorDown(_disp);
				
				} else if (st == KEY_ENTER) {
					MenuList ml = nav.getCurrentItem();
					showSelectProcessing(ml, S_DEV_D2);
				
				} else if (st == KEY_ESC) {
					showMainMenu();
				}
				break;
			}

			case SCENE_SELECT_D3: {
				if (st == KEY_UP) {
					nav.cursorUp(_disp);
				
				} else if (st == KEY_DOWN) {
					nav.cursorDown(_disp);
				
				} else if (st == KEY_ENTER) {
					MenuList ml = nav.getCurrentItem();
					showSelectProcessing(ml, S_DEV_D3);
				
				} else if (st == KEY_ESC) {
					showMainMenu();
				}
				break;
			}

			case SCENE_SELECT_D4: {
				if (st == KEY_UP) {
					nav.cursorUp(_disp);
				
				} else if (st == KEY_DOWN) {
					nav.cursorDown(_disp);
				
				} else if (st == KEY_ENTER) {
					MenuList ml = nav.getCurrentItem();
					showSelectProcessing(ml, S_DEV_D4);
				
				} else if (st == KEY_ESC) {
					showMainMenu();
				}
				break;
			}

			case SCENE_SELECT_CAS: {
				if (st == KEY_UP) {
					nav.cursorUp(_disp);
				
				} else if (st == KEY_DOWN) {
					nav.cursorDown(_disp);
				
				} else if (st == KEY_ENTER) {
					MenuList ml = nav.getCurrentItem();
					showSelectProcessing(ml, S_DEV_CAS);
				
				} else if (st == KEY_ESC) {
					showMainMenu();
				}
				break;
			}

			case SCENE_INFO_WINDOW: {
				if (st != KEY_NONE) {
					currentScene = backScene;

#if defined(DEBUG) && defined(DEBUG_SCENES)
					LOG.println("Set current scene to Previous Scene");
#endif

					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;
				}
				break;
			}

			case SCENE_EJECT_D1: {
				if (st == KEY_ESC) {
					currentScene = backScene;

#if defined(DEBUG) && defined(DEBUG_SCENES)
					LOG.println("Set current scene to Previous Scene");
#endif

					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;
				
				} else if (st == KEY_ENTER) {
					showMainMenu();
					nav.changeItem(0, {
						iconDisk0,
						MTYPE_D1,
						C_MITEM_EMPTY,
						S_NO_DISK,
						"DiskDrive D1:",
						S_DEV_D1
					});
					nav.redraw(_disp);
					st = KEY_NONE;

					atariSio.unMountImageD1();
				}
				break;
			}

			case SCENE_EJECT_D2: {
				if (st == KEY_ESC) {
					currentScene = backScene;

#if defined(DEBUG) && defined(DEBUG_SCENES)
					LOG.println("Set current scene to Previous Scene");
#endif

					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;

				} else if (st == KEY_ENTER) {
					showMainMenu();
					nav.changeItem(1, {
						iconDisk0,
						MTYPE_D2,
						C_MITEM_EMPTY,
						S_NO_DISK,
						"DiskDrive D2:",
						S_DEV_D2
					});
					nav.redraw(_disp);
					st = KEY_NONE;

					atariSio.unMountImageD2();
				}
				break;
			}

			case SCENE_EJECT_D3: {
				if (st == KEY_ESC) {
					currentScene = backScene;

#if defined(DEBUG) && defined(DEBUG_SCENES)
					LOG.println("Set current scene to Previous Scene");
#endif

					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;

				} else if (st == KEY_ENTER) {
					showMainMenu();
					nav.changeItem(2, {
						iconDisk0,
						MTYPE_D3,
						C_MITEM_EMPTY,
						S_NO_DISK,
						"DiskDrive D3:",
						S_DEV_D3
					});
					nav.redraw(_disp);
					st = KEY_NONE;

					atariSio.unMountImageD3();
				}
				break;
			}

			case SCENE_EJECT_D4: {
				if (st == KEY_ESC) {
					currentScene = backScene;

#if defined(DEBUG) && defined(DEBUG_SCENES)
					LOG.println("Set current scene to Previous Scene");
#endif

					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;

				} else if (st == KEY_ENTER) {
					showMainMenu();
					nav.changeItem(3, {
						iconDisk0,
						MTYPE_D4,
						C_MITEM_EMPTY,
						S_NO_DISK,
						"DiskDrive D4:",
						S_DEV_D4
					});
					nav.redraw(_disp);
					st = KEY_NONE;

					atariSio.unMountImageD4();
				}
				break;
			}

			case SCENE_EJECT_CAS: {
				if (st == KEY_ESC) {
					currentScene = backScene;

#if defined(DEBUG) && defined(DEBUG_SCENES)
					LOG.println("Set current scene to Previous Scene");
#endif

					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;

				} else if (st == KEY_ENTER) {
					showMainMenu();
					nav.changeItem(4, {
						iconCas0,
						MTYPE_CAS,
						C_MITEM_EMPTY,
						S_NO_CAS,
						"Recorder C:",
						S_DEV_CAS
					});
					nav.redraw(_disp);
					st = KEY_NONE;
				}
				break;
			}

			case SCENE_START_CAS: {
				if (st == KEY_ESC) {
					currentScene = backScene;

#if defined(DEBUG) && defined(DEBUG_SCENES)
					LOG.println("Set current scene to Previous Scene");
#endif

					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;

				} else if (st == KEY_ENTER) {
					showPlayCas(nav.getItemName(4));
					st = KEY_NONE;
					currentScene = SCENE_PLAY_CAS;
					vr.startPlay(_disp, nav.getItemPath(4));
				}
				break;
			}

			case SCENE_PLAY_CAS: {
				if (vr.playComplete()) {
					winMsg.showCustom(
						_disp,
						"Complete!",
						"Loading from cassette\ncompleted successfully.",
						iconOk, false, true, 0
					);
				} else {
					winMsg.showCustom(
						_disp,
						"Stop!",
						"Loading from cassette\nwas aborted.",
						iconStop, false, true, 0
					);
				}
				st = KEY_NONE;
				currentScene = SCENE_STOP_CAS;
				break;
			}

			case SCENE_STOP_CAS: {
				if (st != KEY_NONE) {
					currentScene = backScene;

#if defined(DEBUG) && defined(DEBUG_SCENES)
					LOG.println("Set current scene to Previous Scene");
#endif
					showMainMenu();
					_forceUpdate = true;
					st = KEY_NONE;
				}
				break;
			}

			case SCENE_TOOLS: {
				if (st != KEY_NONE) {
					currentScene = backScene;

#if defined(DEBUG) && defined(DEBUG_SCENES)
					LOG.println("Set current scene to Previous Scene");
#endif
					showMainMenu();
					_forceUpdate = true;
					st = KEY_NONE;
				}
				break;
			}

			case SCENE_SETTINGS: {
				if (st != KEY_NONE) {
					currentScene = backScene;

#if defined(DEBUG) && defined(DEBUG_SCENES)
					LOG.println("Set current scene to Previous Scene");
#endif
					showMainMenu();
					_forceUpdate = true;
					st = KEY_NONE;
				}
				break;
			}

			default: {
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::checkSD() {
	if(SD.begin()){
		uint8_t cardType = SD.cardType();
		if(cardType == CARD_NONE){
#ifdef DEBUG
			LOG.println("[ ERROR ]");
#endif
			showNoSD();
		} else {
			//////////////////////////////////////////////////
			// atariSio.mountImageD1("/autorun.xex");
			//////////////////////////////////////////////////
#ifdef DEBUG
			LOG.println("[ OK ]");
			LOG.print("Load config file... ");
#endif
			loadConfig();
		}
	} else {
#ifdef DEBUG
			LOG.println("[ ERROR ]");
#endif
			showNoSD();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showNoSD() {
	_disp.clearScreen();
	winMsg.show(_disp, MSG_TYPE_STOP, "STOP!", "SD-Card is not available!\nIs damaged, or has not\nthe correct format.");
	currentScene = SCENE_REBOOT;

#if defined(DEBUG) && defined(DEBUG_SCENES)
      LOG.println("Set current scene to SCENE_REBOOT");
#endif

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showMainMenu() {
	uint16_t itemsCount = 7;
	//TODO: Что с этим делать !!!
	if (itemsCount > MAX_NAV_ITEMS) {
		itemsCount = MAX_NAV_ITEMS;
	}
	nav.setup(SCENE_COLOR_BG, MENU_TOP_COLOR, MENU_TOP_COLOR_BG, MENU_BOTTOM_COLOR, MENU_BOTTOM_COLOR_BG, "Astarta Menu",itemsCount);
	nav.addItem({ iconDisk0,	MTYPE_D1,	C_MITEM_EMPTY,		S_NO_DISK,	"DiskDrive D1:",	S_DEV_D1});
	nav.addItem({ iconDisk0,	MTYPE_D2,	C_MITEM_EMPTY,		S_NO_DISK,	"DiskDrive D2:",	S_DEV_D1});
	nav.addItem({ iconDisk0,	MTYPE_D3,	C_MITEM_EMPTY,		S_NO_DISK,	"DiskDrive D3:",	S_DEV_D3});
	nav.addItem({ iconDisk0,	MTYPE_D4,	C_MITEM_EMPTY,		S_NO_DISK,	"DiskDrive D4:",	S_DEV_D4});
	nav.addItem({ iconCas0,		MTYPE_CAS,	C_MITEM_EMPTY,		S_NO_CAS,	"Recorder C:",		S_DEV_CAS});
	nav.addItem({ iconTools,	MTYPE_TOOLS,	C_MITEM_DEFAULT,	"Tools",	"Tools",		""});
	nav.addItem({ iconSettings,	MTYPE_SETTINGS,	C_MITEM_DEFAULT,	"Settings",	"Settings",		""});
	nav.redraw(_disp);
	currentScene = SCENE_MAINMENU;
	_forceUpdate = true;

#if defined(DEBUG) && defined(DEBUG_SCENES)
      LOG.println("Set current scene to SCENE_MAINMENU");
#endif

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::buildName(char *buff) { 
	int month, day, year;
	static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	sscanf(__DATE__, "%s %d %d", buff, &day, &year);
	month = (strstr(month_names, buff)-month_names)/3+1;
	sprintf(buff, "%02d%02d%02d", year-2000, month, day);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showStart() {
	char buff[6];
	buildName(buff);
	
	_disp.clearScreen();

	uint8_t verWidth = 8;
	uint8_t pjxWidth = 8;

	uint8_t lWidth = _width - verWidth; // 122
	uint8_t aWidth = 96 + pjxWidth; // 112

	uint8_t logoY = (lWidth - aWidth)/3 * 2;

	_disp.drawImage((_width-96)/2, logoY, 96, 96, logo);
	_disp.drawFont6String(67, logoY + 96 + 3, "PROJECT", START_PROJECT_COLOR);

	String buildStr = buff;

#if defined(DEBUG)
	_disp.drawFont6String(10, _width-verWidth-2 , String(__VERSION__) + "D Build " + buildStr, START_VERSION_COLOR);
#else
	_disp.drawFont6String(10, _width-verWidth-1 , String(__VERSION__) + " Build " + buildStr, START_VERSION_COLOR);
#endif

	currentScene = SCENE_START;

#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_START");
#endif

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showSelectD1() {
	showSelect("D1:");
	currentScene = SCENE_SELECT_D1;
	_forceUpdate = true;

#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_SELECT_D1");
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showSelectD2() {
	showSelect("D2:");
	currentScene = SCENE_SELECT_D2;
	_forceUpdate = true;

#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_SELECT_D2");
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showSelectD3() {
	showSelect("D3:");
	currentScene = SCENE_SELECT_D3;
	_forceUpdate = true;

#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_SELECT_D3");
#endif

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showSelectD4() {
	showSelect("D4:");
	currentScene = SCENE_SELECT_D4;
	_forceUpdate = true;

#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_SELECT_D4");
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showSelectCAS() {
	showSelect("C:");
	currentScene = SCENE_SELECT_CAS;
	_forceUpdate = true;

#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_SELECT_CAS");
#endif

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showSelect(String devName) {
	char buf[255];
	rootPath.toCharArray(buf, 255);

	File root = SD.open(buf);
	if(!root) {
		winMsg.show(_disp, MSG_TYPE_STOP, "I/O Error", "Can't open current path.\n\"" + String(buf) + "\"");
		currentScene = SCENE_REBOOT;

#if defined(DEBUG) && defined(DEBUG_SCENES)
		LOG.println("Set current scene to SCENE_REBOOT");
#endif
	
	} else if (!root.isDirectory()) {
		winMsg.show(_disp, MSG_TYPE_STOP, "Dir Error", "Current path.\n\"" + rootPath + "\"\nis not a directory!");
		currentScene = SCENE_REBOOT;

#if defined(DEBUG) && defined(DEBUG_SCENES)
		LOG.println("Set current scene to SCENE_REBOOT");
#endif

	} else {

		nav.eraseCursor(_disp);

		winMsg.show(_disp, MSG_TYPE_PROCESS, "Wait...", "Readind directory...");
		

#if defined(DEBUG) && defined(DEBUG_SCENES)
		LOG.println("Calculate subdirs & files...");
#endif
		uint32_t dirsCount = 0;
		uint32_t filesCount = 0;
		File file;
		try {
			file = root.openNextFile();
			while(file) {
				if(file.isDirectory()){
					dirsCount++;
				} else {
					filesCount++;
				}
				file = root.openNextFile();
			}
		} catch( int ErrCode ) {

#if defined(DEBUG) && defined(DEBUG_SCENES)
			LOG.print("Error[0]=");
			LOG.println(ErrCode);
#endif
		}

		uint16_t itemsCount = dirsCount + filesCount;
		//TODO: Что с этим делать !!!
		if (itemsCount > MAX_NAV_ITEMS) {
			itemsCount = MAX_NAV_ITEMS;
		}
		nav.setup(SCENE_COLOR_BG, MENU_TOP_COLOR, MENU_TOP_COLOR_BG, MENU_BOTTOM_COLOR, MENU_BOTTOM_COLOR_BG, "Select file for " + devName, itemsCount);

#if defined(DEBUG) && defined(DEBUG_SCENES)
		LOG.print("Dirs:");
		LOG.println(dirsCount);
		LOG.print("Files:");
		LOG.println(filesCount);

		LOG.println("Prepare list of subdirs & files...");
#endif
		try {
			uint8_t startCut = rootPath.length();
			if (rootPath != "/") {
				dirsCount++;
			}
			if (rootPath != "/") {
				startCut++;
				nav.addItem({
					iconDirUp,
					MTYPE_DIR_UP,
					C_MITEM_DIR_UP,
					"..",
					"Back directory up",
					""
				});
			}

			root = SD.open(buf);
			file = root.openNextFile();
			String itemsCountStr = String(dirsCount + filesCount);
			uint16_t currentCount = 1;
			while(file) {

#if defined(DEBUG) && defined(DEBUG_SCENES)
				LOG.println("Processing (" + String(currentCount) + " / " + itemsCountStr + ")");
#endif

				winMsg.updateMsg(_disp, "Processing (" + String(currentCount) + " / " + itemsCountStr + ")");

				String fileName = String(file.name());
				if(file.isDirectory()){
					nav.addItem({
						iconDir,
						MTYPE_DIR,
						C_MITEM_DIR,
						fileName,
						"Directory",
						""
					});
				} else {
					long _fileSize = file.size();
					long fileSize = _fileSize;
					String fileSizeM = "bytes";
					if (fileSize >= 1073741824) {
						fileSizeM = "GiB";
						fileSize = fileSize/(1024^3);
					} else if (fileSize >= 1048576) {
						fileSizeM = "MiB";
						fileSize = fileSize/(1024^2);
					} else if (fileSize >= 1024) {
						fileSizeM = "KiB";
						fileSize = fileSize/1024;
					}
					String fileSizeS = String(fileSize);
					String fileSizeA = "";
					if (_fileSize != fileSize) {
						fileSizeA = " (" + String(_fileSize) + " bytes)";
					}

					const uint8_t *mIcon = iconFileUnknown;
					uint8_t mType = MTYPE_UNKNOWN;
					uint32_t fnLen = fileName.length();
					if (fnLen >=5) {
						String fileExt = fileName.substring(fnLen-4, fnLen);
						fileExt.toLowerCase();
						if (fileExt == ".atr") {
							mIcon = iconFileATR;
							mType = MTYPE_DISKETTE;
							
						} else if (fileExt == ".atx") {
							mIcon = iconFileATX;
							mType = MTYPE_DISKETTE;

						} else if (fileExt == ".cas") {
							mIcon = iconFileCAS;
							mType = MTYPE_CASSETTE;
						
						} else if (fileExt == ".dcm") {
							mIcon = iconFileDCM;
							mType = MTYPE_DISKETTE;
						
						} else if (fileExt == ".pro") {
							mIcon = iconFilePRO;
							mType = MTYPE_DISKETTE;
						
						} else if (fileExt == ".scp") {
							mIcon = iconFileSCP;
							mType = MTYPE_DISKETTE;

						} else if (fileExt == ".xex") {
							mIcon = iconFileXEX;
							mType = MTYPE_DISKETTE;

						} else if (fileExt == ".xfd") {
							mIcon = iconFileXFD;
							mType = MTYPE_DISKETTE;
						}
					}
					nav.addItem({
						mIcon,
						mType,
						C_MITEM_FILE,
						fileName,
						fileSizeS + " " + fileSizeM + fileSizeA,
						""
					});
				}
				file = root.openNextFile();
				currentCount++;
			}
		} catch( int ErrCode ) {

#if defined(DEBUG) && defined(DEBUG_SCENES)
			LOG.print("Error[1]=");
			LOG.println(ErrCode);
#endif
		}
		nav.redraw(_disp);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showWrongType(uint8_t _bs) {
	winMsg.show(_disp, MSG_TYPE_WARNING, "Attention!", "This file cannot be used.\nUnsupported file format.");
	currentScene = SCENE_INFO_WINDOW;
	backScene = _bs;

#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_INFO_WINDOW");
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showEjectD1() {
	showEject(S_DEV_D1);
	currentScene = SCENE_EJECT_D1;
	_forceUpdate = true;

#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_EJECT_D1");
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showEjectD2() {
	showEject(S_DEV_D2);
	currentScene = SCENE_EJECT_D2;
	_forceUpdate = true;

#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_EJECT_D2");
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showEjectD3() {
	showEject(S_DEV_D3);
	currentScene = SCENE_EJECT_D3;
	_forceUpdate = true;

#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_EJECT_D3");
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showEjectD4() {
	showEject(S_DEV_D4);
	currentScene = SCENE_EJECT_D4;
	_forceUpdate = true;

#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_EJECT_D4");
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showEjectCAS() {
	showEject(S_DEV_CAS);
	currentScene = SCENE_EJECT_CAS;
	_forceUpdate = true;

#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_EJECT_CAS");
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showEject(String devName) {
	String msg = "Are you sure you want\nto eject the ";
	if (devName == S_DEV_CAS) {
		msg += " cassette\n image from device " + devName + "?";
	} else {
		msg += " disk\n image from device " + devName + "?";
	}
	winMsg.show(_disp, MSG_TYPE_QUESTION, "Eject?", msg);
	backScene = SCENE_MAINMENU;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showSelectByDevId(String devId) {
	if (devId == S_DEV_D1) {
		showSelectD1();
	} else if (devId == S_DEV_D2) {
		showSelectD2();
	} else if (devId == S_DEV_D3) {
		showSelectD3();
	} else if (devId == S_DEV_D4) {
		showSelectD4();
	} else if (devId == S_DEV_CAS) {
		showSelectCAS();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showSelectProcessing(MenuList ml, String devId) {
	if (ml.mType == MTYPE_DIR) {
		if (rootPath == "/") {
			rootPath += ml.mName;
		} else {
			rootPath += "/" + ml.mName;
		}

		showSelectByDevId(devId);

	} else if (ml.mType == MTYPE_DIR_UP) {
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

		showSelectByDevId(devId);

	} else if (ml.mType == MTYPE_DISKETTE && (devId == S_DEV_D1 || devId == S_DEV_D2 || devId == S_DEV_D3 || devId == S_DEV_D4)) {

		bool isMounted = false;

		if (devId == S_DEV_D1) {
			isMounted = atariSio.mountImageD1(rootPath + "/" + ml.mName);
			if (isMounted != true) showWrongType(SCENE_SELECT_D1);

		} else if (devId == S_DEV_D2) {
			isMounted = atariSio.mountImageD2(rootPath + "/" + ml.mName);
			if (isMounted != true) showWrongType(SCENE_SELECT_D2);

		} else if (devId == S_DEV_D3) {
			isMounted = atariSio.mountImageD3(rootPath + "/" + ml.mName);
			if (isMounted != true) showWrongType(SCENE_SELECT_D3);

		} else if (devId == S_DEV_D4) {
			isMounted = atariSio.mountImageD4(rootPath + "/" + ml.mName);
			if (isMounted != true) showWrongType(SCENE_SELECT_D4);
		}

		if (isMounted == true) {
			uint8_t idx = getIndexById(devId);
			showMainMenu();
			nav.changeItem(idx, {
				iconDisk1,
				MTYPE_D1,
				C_MITEM_ACTIVE,
				ml.mName,
				"DiskDrive D" + String(idx+1) + ":",
				rootPath + "/" + ml.mName
			});
			nav.redraw(_disp);
		}

	} else if (ml.mType == MTYPE_CASSETTE && devId == S_DEV_CAS) {
		uint8_t idx = getIndexById(devId);
		showMainMenu();
		nav.changeItem(idx, {
			iconCas1,
			MTYPE_CAS,
			C_MITEM_ACTIVE,
			ml.mName,
			"Recorder C:",
			rootPath + "/" + ml.mName
		});
		nav.redraw(_disp);
	
	} else {
		if (devId == S_DEV_D1) {
			showWrongType(SCENE_SELECT_D1);
		
		} else if (devId == S_DEV_D2) {
			showWrongType(SCENE_SELECT_D2);
		
		} else if (devId == S_DEV_D3) {
			showWrongType(SCENE_SELECT_D3);
		
		} else if (devId == S_DEV_D4) {
			showWrongType(SCENE_SELECT_D4);
		
		} else if (devId == S_DEV_CAS) {
			showWrongType(SCENE_SELECT_CAS);
		}

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t Scene::getIndexById(String devId) {
	uint8_t index = 0;
	if (devId == S_DEV_D2) {
		index = 1;
	
	} else if (devId == S_DEV_D3) {
		index = 2;
	
	} else if (devId == S_DEV_D4) {
		index = 3;
	
	} else if (devId == S_DEV_CAS) {
		index = 4;
	}
	
	return index;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showStartCas() {
#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_START_CAS");
#endif
	winMsg.showCustom(
		_disp,
		"Cassette",
		"Turn ON your ATARI with\nSTART+OPTION keys &\npress RETURN key. Then,\nafter SIGNAL, press OK.",
		iconCassette, true, true, 0
	);
	backScene = SCENE_MAINMENU;
	currentScene = SCENE_START_CAS;

	_forceUpdate = true;
	kbd.reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showPlayCas(String casName) {
	winMsg.showCustom(_disp,
		"Playing...",
		casName ,
		iconCassette, true, false, 0
	);

	backScene = SCENE_MAINMENU;
	currentScene = SCENE_PLAY_CAS;
	_forceUpdate = true;

#if defined(DEBUG) && defined(DEBUG_SCENES)
	LOG.println("Set current scene to SCENE_PLAY_CAS");
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::loadConfig() {
	if (!_cp.init("/astarta.cfg")) {
#ifdef DEBUG
		LOG.println("[ ERROR ]");
#endif
		_disp.clearScreen();
		winMsg.show(_disp, MSG_TYPE_STOP, "STOP!", "Config file is't available,\ndamaged, or has not\nthe correct format.");
		currentScene = SCENE_REBOOT;

#if defined(DEBUG) && defined(DEBUG_SCENES)
		LOG.println("Set current scene to SCENE_REBOOT");
#endif

	} else {
#ifdef DEBUG
		LOG.println("[ OK ]");
#endif
#if defined(DEBUG) && defined(DEBUG_SCENES)
		LOG.println("Show Mainmenu... ");
#endif

#ifdef ENABLE_WIFI
		_disp.clearScreen();
		winMsg.show(_disp, MSG_TYPE_PROCESS, "Wait...", "till wifi connection\nhas established!");
		currentScene = SCENE_CONNECT;
#else
		String mPath = "/autorun.xex";
		String mName = mPath.substring(mPath.lastIndexOf("/") + 1);

#if defined(DEBUG) && defined(DEBUG_SCENES)
		LOG.print("Automount D1:\"");
		LOG.print(mName);
		LOG.print("\", Path:\"");
		LOG.print(mPath);
		LOG.println("\"");
#endif

		bool isMounted = atariSio.mountImageD1(mPath);

#if defined(DEBUG) && defined(DEBUG_SCENES)
		LOG.print("isMounted=");
		LOG.println(isMounted);
#endif

		showMainMenu();

		if (isMounted == true) {
			uint8_t idx = getIndexById(S_DEV_D1);

#if defined(DEBUG) && defined(DEBUG_SCENES)
			LOG.print("idx=");
			LOG.println(idx);
#endif
			nav.changeItem(idx, {
				iconDisk1,
				MTYPE_D1,
				C_MITEM_ACTIVE,
				mName,
				"DiskDrive D" + String(idx+1) + ":",
				mPath
			});
			nav.redraw(_disp);
		}
#endif
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showTools() {
	winMsg.show(_disp, MSG_TYPE_WARNING, "Oops!", "This is reserved for future\nuse, and at the moment\nit has't been implemented.");
	currentScene = SCENE_TOOLS;
	backScene = SCENE_MAINMENU;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::showSettings() {
	winMsg.show(_disp, MSG_TYPE_WARNING, "Oops!", "This is reserved for future\nuse, and at the moment\nit has't been implemented.");
	currentScene = SCENE_SETTINGS;
	backScene = SCENE_MAINMENU;
}
