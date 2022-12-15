#include "scene.h"
#include "icons.h"
#include "icons24.h"
#include "logo.h"
#include "font_16p.h"

Scene::Scene() {
	mList[0] = { iconDisk0,		MTYPE_D1,		C_MITEM_EMPTY,		S_NO_DISK,	"DiskDrive D1:",	S_DEV_D1 };
	mList[1] = { iconDisk0,		MTYPE_D2,		C_MITEM_EMPTY,		S_NO_DISK,	"DiskDrive D2:",	S_DEV_D1 };
	mList[2] = { iconDisk0,		MTYPE_D3,		C_MITEM_EMPTY,		S_NO_DISK,	"DiskDrive D3:",	S_DEV_D3 };
	mList[3] = { iconDisk0,		MTYPE_D4,		C_MITEM_EMPTY,		S_NO_DISK,	"DiskDrive D4:",	S_DEV_D4 };
	mList[4] = { iconCas0,		MTYPE_CAS,		C_MITEM_EMPTY,		S_NO_CAS,	"Recorder C:",		S_DEV_CAS };
	mList[5] = { iconTools,		MTYPE_TOOLS,	C_MITEM_DEFAULT,	"Tools",	"Tools",			""};
	mList[6] = { iconSettings,	MTYPE_SETTINGS,	C_MITEM_DEFAULT,	"Settings",	"Settings",			""};

	rootPath = "/";

	atariCMD = false;
	atariSio.init(&_disp, &_net);

	netIniting = false;
	netInited = false;

	_forceUpdate = false;
	
}

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
	
		if (netInited == false && netIniting == false) {
			_net.init(&_disp);
			netIniting = true;

		} else if (netInited == false && netIniting == true) {
			if(_net.checkWiFi() == true) {
				netInited = true;
				netIniting = false;
			}
		
		} else if (netInited == true && netIniting == false) {
			_net.update(_forceUpdate);
			_forceUpdate = false;
		}

		uint8_t st = kbd.refresh();
		switch (currentScene) {
			case SCENE_START: {
				if (st != KEY_NONE) {
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

					} if (ml.mType == MTYPE_CAS) {
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
					
					} if (ml.mType == MTYPE_SETTINGS) {
					
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
					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;
				}
				break;
			}

			case SCENE_EJECT_D1: {
				if (st == KEY_ESC) {
					currentScene = backScene;
					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;
				
				} else if (st == KEY_ENTER) {
					mList[0].mIcon = iconDisk0;
					mList[0].mColor = C_MITEM_EMPTY;
					mList[0].mName = S_NO_DISK;
					mList[0].mPath = S_DEV_D1;
					showMainMenu();
					st = KEY_NONE;
				}
				break;
			}

			case SCENE_EJECT_D2: {
				if (st == KEY_ESC) {
					currentScene = backScene;
					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;

				} else if (st == KEY_ENTER) {
					mList[1].mIcon = iconDisk0;
					mList[1].mColor = C_MITEM_EMPTY;
					mList[1].mName = S_NO_DISK;
					mList[1].mPath = S_DEV_D2;
					showMainMenu();
					st = KEY_NONE;
				}
				break;
			}

			case SCENE_EJECT_D3: {
				if (st == KEY_ESC) {
					currentScene = backScene;
					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;

				} else if (st == KEY_ENTER) {
					mList[2].mIcon = iconDisk0;
					mList[2].mColor = C_MITEM_EMPTY;
					mList[2].mName = S_NO_DISK;
					mList[2].mPath = S_DEV_D3;
					showMainMenu();
					st = KEY_NONE;
				}
				break;
			}

			case SCENE_EJECT_D4: {
				if (st == KEY_ESC) {
					currentScene = backScene;
					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;

				} else if (st == KEY_ENTER) {
					mList[3].mIcon = iconDisk0;
					mList[3].mColor = C_MITEM_EMPTY;
					mList[3].mName = S_NO_DISK;
					mList[3].mPath = S_DEV_D4;
					showMainMenu();
					st = KEY_NONE;
				}
				break;
			}

			case SCENE_EJECT_CAS: {
				if (st == KEY_ESC) {
					currentScene = backScene;
					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;

				} else if (st == KEY_ENTER) {
					mList[4].mIcon = iconDisk0;
					mList[4].mColor = C_MITEM_EMPTY;
					mList[4].mName = S_NO_CAS;
					mList[4].mPath = S_DEV_CAS;
					showMainMenu();
					st = KEY_NONE;
				}
				break;
			}

			case SCENE_START_CAS: {
				if (st == KEY_ESC) {
					currentScene = backScene;
					nav.redraw(_disp);
					_forceUpdate = true;
					st = KEY_NONE;

				} else if (st == KEY_ENTER) {
					showPlayCas(mList[4].mName);
					vr.startPlay(_disp, mList[4].mPath);
					st = KEY_NONE;
				}
				break;
			}

			case SCENE_PLAY_CAS: {
				if (st != KEY_NONE) {
					currentScene = backScene;
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

void Scene::checkSD() {
	if(SD.begin()){
		uint8_t cardType = SD.cardType();
		if(cardType == CARD_NONE){
			showNoSD();
		} else {
			//////////////////////////////////////////////////
			// atariSio.mountImageD1("/autorun.xex");
			//////////////////////////////////////////////////
			showMainMenu();
		}
	} else {
		showNoSD();
	}
}

void Scene::showNoSD() {
	_disp.clearScreen();
	winMsg.show(_disp, MSG_TYPE_STOP, "STOP!", "SD-Card is not available!\nIs damaged, or has not\nthe correct format.");
	currentScene = SCENE_REBOOT;
}

void Scene::showMainMenu() {
	nav.init(_disp, MENU_TOP_COLOR, MENU_BOTTOM_COLOR, "Astarta Menu", mList, 7);
	currentScene = SCENE_MAINMENU;
	_forceUpdate = true;
}

void Scene::buildName(char *buff) { 
	int month, day, year;
	static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	sscanf(__DATE__, "%s %d %d", buff, &day, &year);
	month = (strstr(month_names, buff)-month_names)/3+1;
	sprintf(buff, "%02d%02d%02d", year-2000, month, day);
}

void Scene::showStart() {
	char buff[6];
	buildName(buff);
	
	_disp.clearScreen();
	_disp.drawImage(16, 7, 96, 96, logo);
	_disp.drawFont6String(67, 106, "PROJECT", COLOR(255,255,0));
	String buildStr = buff;
	_disp.drawFont6String(10, 127-6, "v2.00 Build " + buildStr, COLOR(0,255,0));

	currentScene = SCENE_START;
}

void Scene::showSelectD1() {
	showSelect("D1:");
	currentScene = SCENE_SELECT_D1;
	_forceUpdate = true;
}

void Scene::showSelectD2() {
	showSelect("D2:");
	currentScene = SCENE_SELECT_D2;
	_forceUpdate = true;
}

void Scene::showSelectD3() {
	showSelect("D3:");
	currentScene = SCENE_SELECT_D3;
	_forceUpdate = true;
}

void Scene::showSelectD4() {
	showSelect("D4:");
	currentScene = SCENE_SELECT_D4;
	_forceUpdate = true;
}

void Scene::showSelectCAS() {
	showSelect("C:");
	currentScene = SCENE_SELECT_CAS;
	_forceUpdate = true;
}

void Scene::showSelect(String devName) {
	char buf[255];
	rootPath.toCharArray(buf, 255);

	File root = SD.open(buf);
	if(!root){
		winMsg.show(_disp, MSG_TYPE_STOP, "I/O Error", "Can't open current path.\n\"" + String(buf) + "\"");
		currentScene = SCENE_REBOOT;
	
	} else if (!root.isDirectory()) {
		winMsg.show(_disp, MSG_TYPE_STOP, "Dir Error", "Current path.\n\"" + rootPath + "\"\nis not a directory!");
		currentScene = SCENE_REBOOT;
	} else {
		
		File file = root.openNextFile();
		uint32_t dirsCount = 0;
		uint32_t filesCount = 0;
		while(file) {
			if(file.isDirectory()){
				dirsCount++;
			} else {
				filesCount++;
			}
			file = root.openNextFile();
		}

		uint32_t fileIdx = 0;
		uint32_t dirIdx = 0;
		uint8_t startCut = rootPath.length();
		if (rootPath != "/") {
			dirsCount++;
		}
		MenuList mList[dirsCount + filesCount];
		if (rootPath != "/") {
			startCut++;
			mList[dirIdx].mIcon = iconDirUp;
			mList[dirIdx].mType = MTYPE_DIR_UP;
			mList[dirIdx].mColor = C_MITEM_DIR_UP;
			mList[dirIdx].mName = "..";
			mList[dirIdx].mDis = "Back directory up";
			mList[dirIdx].mPath = "";
			dirIdx++;
		}

		root = SD.open(buf);
		file = root.openNextFile();
		while(file) {
			String fileName = String(file.name());
			fileName = fileName.substring(startCut, fileName.length());
			if(file.isDirectory()){
				mList[dirIdx].mIcon = iconDir;
				mList[dirIdx].mType = MTYPE_DIR;
				mList[dirIdx].mColor = C_MITEM_DIR;
				mList[dirIdx].mName = fileName;
				mList[dirIdx].mDis = "Directory";
				mList[dirIdx].mPath = "";
				dirIdx++;
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

				mList[fileIdx + dirsCount].mIcon = mIcon;
				mList[fileIdx + dirsCount].mType = mType;
				mList[fileIdx + dirsCount].mColor = C_MITEM_FILE;
				mList[fileIdx + dirsCount].mName = fileName;
				mList[fileIdx + dirsCount].mDis = fileSizeS + " " + fileSizeM + fileSizeA;
				mList[fileIdx + dirsCount].mPath = "";
				fileIdx++;
			}
			 file = root.openNextFile();
		}

		nav.init(_disp, COLOR(26,42,130), COLOR(47,70,172), "Select file for " + devName, mList, dirsCount + filesCount);
	}
}

void Scene::showWrongType(uint8_t _bs) {
	winMsg.show(_disp, MSG_TYPE_WARNING, "Attention!", "This file cannot be used.\nUnsupported file format.");
	currentScene = SCENE_INFO_WINDOW;
	backScene = _bs;
}

void Scene::showEjectD1() {
	showEject(S_DEV_D1);
	currentScene = SCENE_EJECT_D1;
	_forceUpdate = true;
}

void Scene::showEjectD2() {
	showEject(S_DEV_D2);
	currentScene = SCENE_EJECT_D2;
	_forceUpdate = true;
}

void Scene::showEjectD3() {
	showEject(S_DEV_D3);
	currentScene = SCENE_EJECT_D3;
	_forceUpdate = true;
}

void Scene::showEjectD4() {
	showEject(S_DEV_D4);
	currentScene = SCENE_EJECT_D4;
	_forceUpdate = true;
}

void Scene::showEjectCAS() {
	showEject(S_DEV_CAS);
	currentScene = SCENE_EJECT_CAS;
	_forceUpdate = true;
}

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
		uint8_t idx = getIndexById(devId);
		mList[idx].mIcon = iconDisk1;
		mList[idx].mColor = C_MITEM_ACTIVE;
		mList[idx].mName = ml.mName;
		mList[idx].mPath = rootPath + "/" + ml.mName;
		showMainMenu();


		if (devId == S_DEV_D1) {
			atariSio.mountImageD1(mList[idx].mPath);
		
		} else if (devId == S_DEV_D2) {
			atariSio.mountImageD2(mList[idx].mPath);
		
		} else if (devId == S_DEV_D3) {
			atariSio.mountImageD3(mList[idx].mPath);
		
		} else if (devId == S_DEV_D4) {
			atariSio.mountImageD4(mList[idx].mPath);
		}

	} else if (ml.mType == MTYPE_CASSETTE && devId == S_DEV_CAS) {
		uint8_t idx = getIndexById(devId);
		mList[idx].mIcon = iconCas1;
		mList[idx].mColor = C_MITEM_ACTIVE;
		mList[idx].mName = ml.mName;
		mList[idx].mPath = rootPath + "/" + ml.mName;
		showMainMenu();
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

void Scene::showStartCas() {
	winMsg.showCustom(
		_disp,
		"Cassette",
		"Turn ON your ATARI with\nSELECT+OPTION keys &\npress RETURN key. Then,\nafter SIGNAL, press OK.",
		iconCassette, true, true
	);
	backScene = SCENE_MAINMENU;
	currentScene = SCENE_START_CAS;
	_forceUpdate = true;
	kbd.reset();
}

void Scene::showPlayCas(String casName) {
	winMsg.showCustom(_disp,
		"Playing...",
		casName ,
		iconCassette, true, false
	);

	backScene = SCENE_MAINMENU;
	currentScene = SCENE_PLAY_CAS;
	_forceUpdate = true;
}
