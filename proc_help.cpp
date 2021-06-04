#include "proc_help.h"
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <random>
#include "imgui/imgui.h"
#include "hackerThreads.h"

ColorStruct colors;//ÎïÆ·ÑÕÉ«½á¹¹Ìå
AppConfigs appConfigs;//²Ëµ¥
int menu_zuobian;//×ó±ß
int menu_dingbian;//¶¥±ß
int menu_kuan;//¿í
int menu_gao;//¸ß
int menu_hangju;//ÐÐ¾à
int menu_select;
int menu_size;//´óÐ¡
int menu_fenge;//·Ö¸ô
int nengnenglvse;
int nengnenglanse;
std::hash_map<int, ApexItem> entityNames;
HANDLE hAimThread;//Ãé×¼Ïß³Ì¾ä±ú

int Ñ°Ñª[3][16] = {};
int Ö±²¼[3][16] = {};
int ÃüÂö[3][16] = {};
int Ì½Â·[3][16] = {};
int ¶¯Á¦[3][16] = {};

int ¶ñÁé[3][16] = {};
int °à¼Ó[3][16] = {};
int ÇÖÊ´[3][16] = {};
int »ÃÏó[3][16] = {};
int »ªÉ­[3][16] = {};

typedef struct __KeyBoardHookStruct {
	int vk_Code;
	int scan_Code;
	int flags;
	int time;
	int dwExtraInfo;
} KeyBoardHookStruct;

using namespace std;

LRESULT CALLBACK keyboardHook(_In_  int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);
DWORD WINAPI KeyBoardHookThread(LPVOID lpParam);

HHOOK keyHook = 0;
/*ÓÃ»§²ãÃæ»ñÈ¡Èí¼þpid*/
DWORD GetProcessIDByName(const char* pName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return 0;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
		_bstr_t info_p_Name(pe.szExeFile);
		if (strcmp(info_p_Name, pName) == 0) {
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
		//printf("%-6d %s\n", pe.th32ProcessID, pe.szExeFile);
	}
	CloseHandle(hSnapshot);
	return 0;
}

void initColor() {
	colors = { ImColor({255, 255, 95}), ImColor({255, 0, 255}), ImColor({188, 102, 255}), ImColor({0, 191, 255}),
			  ImColor({255, 255, 255}), ImColor({197, 219, 70}), ImColor({111, 205, 168}), ImColor({106, 221, 34}),
			  ImColor({255, 188, 0}) };
}
void initConfig() {//³õÊ¼»¯ÐÅÏ¢
	appConfigs = { true, false, 300.f, true,  false, 50.f, true, true, true, false, true, true, 150.f, 2, true };
	DWORD tid = 0;
	HANDLE tHandle = CreateThread(NULL, 0, KeyBoardHookThread, 0, 0, &tid);
	CloseHandle(tHandle);
}

DWORD WINAPI KeyBoardHookThread(LPVOID lpParam) {
	keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHook, 0, 0);
	if (keyHook == 0) {
		MessageBoxA(NULL, "°²×°¼üÅÌ¹³×ÓÊ§°Ü", NULL, 0);
		return 0;
	}
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT) {
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}
		Sleep(1);
	}
	return 0;
}

LRESULT CALLBACK keyboardHook(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	if (nCode == 0 && wParam == 256) {
		KeyBoardHookStruct* kbh = (KeyBoardHookStruct*)lParam;
		switch (kbh->vk_Code) {
		case VK_F11:
			appConfigs.FangKuang = !appConfigs.FangKuang;
			break;
		case VK_F10:
			appConfigs.GUGETOUSHI = !appConfigs.GUGETOUSHI;
			break;
		case VK_F2:
			if (appConfigs.TouShiFanWei >= 1000) {
				appConfigs.TouShiFanWei = 100;
				break;
			}
			appConfigs.TouShiFanWei += 100;
			break;
		case VK_F3:
			appConfigs.WuPingTouShi = !appConfigs.WuPingTouShi;
			break;
		case VK_F4:
			if (appConfigs.WuPingFanWei >= 600) {
				appConfigs.WuPingFanWei = 0;
				break;
			}
			appConfigs.WuPingFanWei += 50;
			break;
		case VK_F5:
			appConfigs.PeiJianTouShi = !appConfigs.PeiJianTouShi;
			break;
		case VK_F6:
			appConfigs.ZiDongMiaoZhun = !appConfigs.ZiDongMiaoZhun;
			break;
		case VK_F7:
			appConfigs.XianShiZhaZhu = !appConfigs.XianShiZhaZhu;
			break;
		case VK_F12:
			appConfigs.KaiFaZheXuanXiang = !appConfigs.KaiFaZheXuanXiang;
			break;
		case VK_F9:
			appConfigs.ZIDONGGUAJI = !appConfigs.ZIDONGGUAJI;
			break;
		case VK_HOME:
			appConfigs.MenuStatus = !appConfigs.MenuStatus;
			break;
		}
	}
	return CallNextHookEx(keyHook, nCode, wParam, lParam);
}

void initMenu() {
	int ju = fontSize * 6;
	menu_zuobian = 10;
	menu_kuan = (int)(ju + fontSize * 2.65 + 10);
	menu_gao = fontSize * 13 + 7;
	menu_fenge = 2;
	menu_dingbian = 150;
	menu_hangju = 16;
	menu_select = 1;
	menu_size = 27;
	nengnenglvse = 10091863;
	nengnenglanse = 16088846;
}

void startThreads() {
	DWORD tid = 0;
	HANDLE tHandle = CreateThread(NULL, 0, InfoThread, 0, 0, &tid);
	CloseHandle(tHandle);
	tHandle = CreateThread(NULL, 0, EntityManager, 0, 0, &tid);
	CloseHandle(tHandle);
	//tHandle = CreateThread(NULL, 0, HentaiThread, 0, 0, &tid);
	//CloseHandle(tHandle);
	//tHandle = CreateThread(NULL, 0, ZiDongGuaJi, 0, 0, &tid);
	//CloseHandle(tHandle);
	hAimThread = CreateThread(NULL, 0, SuperAim, 0, 0, &tid);
}

void rand_str(char* str, const int len) {
	int i;
	for (i = 0; i < len; ++i) {
		switch ((getRandomInt(0, RAND_MAX) % 3)) {
		case 1:
			str[i] = 'A' + getRandomInt(0, RAND_MAX) % 26;
			break;
		case 2:
			str[i] = 'a' + getRandomInt(0, RAND_MAX) % 26;
			break;
		default:
			str[i] = '0' + getRandomInt(0, RAND_MAX) % 10;
			break;
		}
	}
	str[++i] = '\0';
}

void usleep(unsigned int usec) {
	HANDLE timer;
	LARGE_INTEGER ft;
	ft.QuadPart = -(10 * (__int64)usec);
	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}

int getRandomInt(int min, int max) {
	std::random_device rd;
	std::mt19937 mt(rd());
	return mt() % (max - min + 1) + min;
}

void Print_Memory(const unsigned char* start, unsigned int length) {
	//create row, col, and i.  Set i to 0
	int row, col, i = 0;
	//iterate through the rows, which will be 16 bytes of memory wide
	for (row = 0; (i + 1) < length; row++) {
		//print hex representation
		for (col = 0; col < 16; col++) {
			//calculate the current index
			i = row * 16 + col;
			//divides a row of 16 into two columns of 8
			if (col == 8) {
				printf(" ");
			}
			//print the hex value if the current index is in range.
			if (i < length) {
				printf("%02X", start[i]);
			}
			//print a blank if the current index is past the end
			else {
				printf("  ");
			}
			//print a space to keep the values separate
			printf(" ");
		}
		//create a vertial seperator between hex and ascii representations
		printf(" ");
		//print ascii representation
		for (col = 0; col < 16; col++) {
			//calculate the current index
			i = row * 16 + col;
			//divides a row of 16 into two coumns of 8
			if (col == 8) {
				printf("  ");
			}
			//print the value if it is in range
			if (i < length) {
				//print the ascii value if applicable
				if (start[i] > 0x20 && start[i] < 0x7F)  //A-Z
				{
					printf("%c", start[i]);
				}
				//print a period if the value is not printable
				else {
					printf(".");
				}
			}
			//nothing else to print, so break out of this for loop
			else {
				break;
			}
		}
		//create a new row
		printf("\n");
	}
}

void initEntityNames() {
	entityNames[1] = { u8"KRABER", colors.ShiShi };
	//entityNames[2] = { u8"MASTIFF", colors.ShiShi };
	//entityNames[3] = { u8"¹þÎÖ¿Ë", colors.ShiShi };
	entityNames[4] = { u8"L-STAR", colors.NengLiang };
	//entityNames[5] = { u8"¹þÎÖ¿Ë[½ð]", colors.ShiShi };
	//entityNames[6] = { u8"HAVOC", colors.NengLiang };
	//entityNames[7] = { u8"HAVOC", colors.NengLiang };
	entityNames[8] = { u8"DEVOTION", colors.NengLiang };
	//entityNames[9] = { u8"HAVOC", colors.NengLiang };
	//entityNames[10] = { u8"TRIPLE TAKE", colors.ZhongXing };
	//entityNames[12] = { u8"VK-47", colors.ZhongXing };
	//entityNames[13] = { u8"G7¾Ñ»÷Ç¹", colors.QingXing };
	//entityNames[15] = { u8"×ª»»Õß³å·æÇ¹", colors.QingXing };
	//entityNames[16] = { u8"HEMLOK", colors.ZhongXing };
	entityNames[17] = { u8"R99³å·æÇ¹", colors.QingXing };
	//entityNames[18] = { u8"ÁÔÊÞ³å·æÇ¹", colors.ZhongXing };
	//entityNames[19] = { u8"Ç±Ï®³å·æÇ¹[½ð]", colors.ShiShi };
	//entityNames[20] = { u8"G7", colors.ZhongXing };
	//entityNames[22] = { u8"ALTERANTOR", colors.ShiShi };
	entityNames[23] = { u8"R301", colors.QingXing };
	//entityNames[28] = { u8"ÄªÉ£±È¿Ë", colors.SanDan };
	entityNames[26] = { u8"R-99", colors.ShiShi };
	//entityNames[25] = { u8"EVA-8", colors.SanDan };
	//entityNames[30] = { u8"PROWLER", colors.ZhongXing };
	//entityNames[31] = { u8"ÎÞµÐ×óÂÖ[½ð]", colors.ShiShi };
	entityNames[32] = { u8"VOLT SMG", colors.NengLiang };
	entityNames[33] = { u8"VOLT SMG", colors.QingXing };
	//entityNames[34] = { u8"LONG BOW", colors.QingXing };
	//entityNames[35] = { u8"LONG BOW", colors.QingXing };
	//entityNames[36] = { u8"CHARGE", colors.QingXing };

	//entityNames[37] = { u8"ÇáÐÍµ¯Ò©", colors.QingXing };
	entityNames[38] = { u8"M600", colors.ZhongXing };
	//entityNames[39] = { u8"ö±µ¯Ç¹µ¯Ò©", colors.SanDan };
	entityNames[40] = { u8"R301", colors.QingXing };

	//entityNames[42] = { u8"·ï»Ë", colors.ShengQi };
	//entityNames[43] = { u8"´ó¼±¾ÈÏä", colors.ShengQi };
	//entityNames[44] = { u8"EVA-8", colors.ShengQi };
	//entityNames[45] = { u8"´ó»¤¶Üµç³Ø", colors.ShengQi };
	//entityNames[46] = { u8"Ð¡»¤¶Üµç³Ø", colors.ShengQi };

	//entityNames[47] = { u8"Í· lv.1", colors.PuTong };
	//entityNames[48] = { u8"Í· lv.2", colors.GaoJi };
	//entityNames[49] = { u8"PEACE KEEPER", colors.ShiShi };
	//entityNames[50] = { u8"MOZAMBIQUE", colors.ShiShi };//Í· lv.4

	//entityNames[51] = { u8"¼× lv.1", colors.PuTong };
	//entityNames[52] = { u8"WINGMAN", colors.GaoJi };
	//entityNames[53] = { u8"¼× lv.3", colors.ShiShi };
	//entityNames[54] = { u8"¼× lv.4", colors.ShiShi };

	//entityNames[55] = { u8"¶Ü lv.1", colors.PuTong };
	//entityNames[56] = { u8"P2020", colors.QingXing };
	//entityNames[57] = { u8"¶Ü lv.3", colors.ShengQi };
	//entityNames[58] = { u8"RE-45", colors.ShiShi };

	//entityNames[59] = { u8"°ü lv.1", colors.PuTong };
	//entityNames[60] = { u8"SENTINEL", colors.GaoJi };
	//entityNames[61] = { u8"°ü lv.3", colors.ShiShi };
	//entityNames[62] = { u8"LIGHT", colors.ShiShi };

	//entityNames[63] = { u8"ENERGY", colors.NengLiang };
	/*entityNames[64] = { u8"SHOTGUN", colors.GaoJi };
	entityNames[65] = { u8"HEAVY", colors.GaoJi };
	entityNames[66] = { u8"SNIPER", colors.GaoJi };*/

	//entityNames[67] = { u8"ULTIMATE ACCELERANT", colors.GaoJi };
	//entityNames[68] = { u8"PHOENIX KIT", colors.GaoJi };
	//entityNames[69] = { u8"MED KIT", colors.ShengQi };
	//entityNames[70] = { u8"SYRINGR", colors.ShengQi };
	//entityNames[71] = { u8"SHILED BATTERY", colors.GaoJi };
	//entityNames[72] = { u8"SHILED CELL", colors.GaoJi };
	//entityNames[73] = { u8"HELMET LV.1", colors.ShengQi };
	//entityNames[74] = { u8"HELMET LV.2", colors.ShengQi };
	//entityNames[75] = { u8"HELMET LV.3", colors.ShengQi };
	entityNames[76] = { u8"HELMET LV.4", colors.ShiShi };
	/*entityNames[77] = { u8"SHILED LV.1", colors.GaoJi };
	entityNames[78] = { u8"SHILED LV.2", colors.ShengQi };
	entityNames[79] = { u8"SHILED LV.3", colors.ShengQi };*/
	//entityNames[80] = { u8"SHILED LV.4", colors.PuTong };
	//entityNames[81] = { u8"SHILED LV.4", colors.GaoJi };
	/*entityNames[82] = { u8"EVO SHILED LV.1", colors.ShiShi };
	entityNames[83] = { u8"EVO SHILED LV.2", colors.PuTong };
	entityNames[84] = { u8"EVO SHILED LV.3", colors.GaoJi };*/
	//entityNames[85] = { u8"EVO SHILED LV.4", colors.ShiShi };

	//entityNames[86] = { u8"KNOWDOWN S lv.1", colors.PuTong }; //ÄÜÁ¿À©ÈÝLv.1
	//entityNames[87] = { u8"KNOWDOWN S lv.2", colors.GaoJi };
	//entityNames[88] = { u8"KNOWDOWN S lv.3", colors.ShengQi };
	//entityNames[89] = { u8"KNOWDOWN S lv.4", colors.PuTong };//ö±µ¯Ç¹Í·Lv.1

	//entityNames[90] = { u8"BACKPACK lv.1", colors.GaoJi };//ö±µ¯Ç¹Í·Lv.2
	//entityNames[91] = { u8"BACKPACK lv.2", colors.ShengQi };//ö±µ¯Ç¹Í·Lv.3
	//entityNames[92] = { u8"BACKPACK lv.3", colors.ShengQi };//Ç¹Æ¨¹ÉLv.1
	entityNames[93] = { u8"BACKPACK lv.4", colors.ShiShi };//Ç¹Æ¨¹ÉLv.2

	/*entityNames[94] = { u8"THERMITE GRENADE", colors.ShengQi };
	entityNames[95] = { u8"FRAG GRENADE", colors.PuTong };
	entityNames[96] = { u8"ARC STAR", colors.GaoJi };*/

	/*entityNames[97] = { u8"1X", colors.ShengQi };
	entityNames[98] = { u8"2X", colors.ShengQi };
	entityNames[99] = { u8"1X HOLO", colors.ShengQi };
	entityNames[100] = { u8"1-2X", colors.ShengQi };
	entityNames[101] = { u8"1X-DIGITAL", colors.ShengQi };
	entityNames[102] = { u8"3X", colors.ShengQi };
	entityNames[103] = { u8"2-4X", colors.ShengQi };

	entityNames[104] = { u8"1X", colors.ShengQi };
	entityNames[105] = { u8"2X", colors.ShengQi };
	entityNames[106] = { u8"1X HOLO", colors.ShengQi };
	entityNames[107] = { u8"1-2X", colors.ShengQi };
	entityNames[108] = { u8"1X-DIGITAL", colors.ShengQi };
	entityNames[109] = { u8"3X", colors.ShengQi };*/
	entityNames[110] = { u8"BARREL STABLE LV.4", colors.ShengQi };

	//entityNames[111] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	//entityNames[112] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	//entityNames[113] = { u8"LIGHT MAG LV.3", colors.ShengQi };

	/*entityNames[114] = { u8"HEAVY MAG LV.1", colors.ShengQi };
	entityNames[115] = { u8"HEAVY MAG LV.2", colors.ShengQi };
	entityNames[116] = { u8"HEAVY MAG LV.3", colors.ShengQi };

	entityNames[117] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[118] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[119] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[120] = { u8"BARREL STABLE LV.4", colors.ShengQi };

	entityNames[121] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[122] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[123] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[124] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[125] = { u8"BARREL STABLE LV.4", colors.ShengQi };*/

	//entityNames[126] = { u8"STANDER STOCK LV.1", colors.ShengQi };
	//entityNames[127] = { u8"STANDER STOCK LV.2", colors.ShengQi };
	entityNames[128] = { u8"STANDER STOCK LV.3", colors.ShengQi };

	/*entityNames[129] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[131] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[132] = { u8"BARREL STABLE LV.4", colors.ShengQi };

	entityNames[133] = { u8"SELECTFIRE", colors.ShengQi };*/
	entityNames[134] = { u8"SKULLPIERCER", colors.ShengQi };

	entityNames[135] = { u8"HAMMERPOINT", colors.ShengQi };
	/*entityNames[136] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[137] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[138] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[139] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[140] = { u8"BARREL STABLE LV.4", colors.ShengQi };*/
}

void initBones() {
	//Ñ°Ñª
	{
		Ñ°Ñª[0][0] = Bones::head;
		Ñ°Ñª[0][1] = Bones::neck;
		Ñ°Ñª[0][2] = Bones::chest;
		Ñ°Ñª[0][3] = Bones::abdomen;
		Ñ°Ñª[0][4] = Bones::ŒÅ;

		Ñ°Ñª[1][0] = Bones::Ñ°Ñª×ó¼ç + 7;
		Ñ°Ñª[1][1] = Bones::Ñ°Ñª×ó¼ç + 5;
		Ñ°Ñª[1][2] = Bones::Ñ°Ñª×ó¼ç + 4;
		Ñ°Ñª[1][3] = Bones::Ñ°Ñª×ó¼ç + 2;
		Ñ°Ñª[1][4] = Bones::Ñ°Ñª×ó¼ç + 3;
		Ñ°Ñª[1][5] = Bones::Ñ°Ñª×ó¼ç + 1;
		Ñ°Ñª[1][6] = Bones::Ñ°Ñª×ó¼ç;
		Ñ°Ñª[1][7] = Bones::Ñ°Ñª×ó¼ç - 2;

		Ñ°Ñª[1][8] = Bones::Ñ°ÑªÓÒ¼ç - 2;
		Ñ°Ñª[1][9] = Bones::Ñ°ÑªÓÒ¼ç;
		Ñ°Ñª[1][10] = Bones::Ñ°ÑªÓÒ¼ç + 1;
		Ñ°Ñª[1][11] = Bones::Ñ°ÑªÓÒ¼ç + 3;
		Ñ°Ñª[1][12] = Bones::Ñ°ÑªÓÒ¼ç + 2;
		Ñ°Ñª[1][13] = Bones::Ñ°ÑªÓÒ¼ç + 4;
		Ñ°Ñª[1][14] = Bones::Ñ°ÑªÓÒ¼ç + 5;
		Ñ°Ñª[1][15] = Bones::Ñ°ÑªÓÒ¼ç + 7;

		Ñ°Ñª[2][0] = Bones::Ñ°Ñª×óÏ¥ + 2;
		Ñ°Ñª[2][1] = Bones::Ñ°Ñª×óÏ¥ + 1;
		Ñ°Ñª[2][2] = Bones::Ñ°Ñª×óÏ¥;
		Ñ°Ñª[2][3] = Bones::ŒÅ;
		Ñ°Ñª[2][4] = Bones::Ñ°ÑªÓÒÏ¥;
		Ñ°Ñª[2][5] = Bones::Ñ°ÑªÓÒÏ¥ + 1;
		Ñ°Ñª[2][6] = Bones::Ñ°ÑªÓÒÏ¥ + 2;
	}
	//Ö±²¼
	{
		Ö±²¼[0][0] = Bones::head;
		Ö±²¼[0][1] = Bones::neck;
		Ö±²¼[0][2] = Bones::chest;
		Ö±²¼[0][3] = Bones::abdomen;
		Ö±²¼[0][4] = Bones::ŒÅ;

		Ö±²¼[1][0] = Bones::Ö±²¼×ó¼ç + 7;
		Ö±²¼[1][1] = Bones::Ö±²¼×ó¼ç + 5;
		Ö±²¼[1][2] = Bones::Ö±²¼×ó¼ç + 4;
		Ö±²¼[1][3] = Bones::Ö±²¼×ó¼ç + 2;
		Ö±²¼[1][4] = Bones::Ö±²¼×ó¼ç + 3;
		Ö±²¼[1][5] = Bones::Ö±²¼×ó¼ç + 1;
		Ö±²¼[1][6] = Bones::Ö±²¼×ó¼ç;
		Ö±²¼[1][7] = Bones::Ö±²¼×ó¼ç - 2;

		Ö±²¼[1][8] = Bones::Ö±²¼ÓÒ¼ç - 2;
		Ö±²¼[1][9] = Bones::Ö±²¼ÓÒ¼ç;
		Ö±²¼[1][10] = Bones::Ö±²¼ÓÒ¼ç + 1;
		Ö±²¼[1][11] = Bones::Ö±²¼ÓÒ¼ç + 3;
		Ö±²¼[1][12] = Bones::Ö±²¼ÓÒ¼ç + 2;
		Ö±²¼[1][13] = Bones::Ö±²¼ÓÒ¼ç + 4;
		Ö±²¼[1][14] = Bones::Ö±²¼ÓÒ¼ç + 5;
		Ö±²¼[1][15] = Bones::Ö±²¼ÓÒ¼ç + 7;

		Ö±²¼[2][0] = Bones::Ö±²¼×óÏ¥ + 2;
		Ö±²¼[2][1] = Bones::Ö±²¼×óÏ¥ + 1;
		Ö±²¼[2][2] = Bones::Ö±²¼×óÏ¥;
		Ö±²¼[2][3] = Bones::ŒÅ;
		Ö±²¼[2][4] = Bones::Ö±²¼ÓÒÏ¥;
		Ö±²¼[2][5] = Bones::Ö±²¼ÓÒÏ¥ + 1;
		Ö±²¼[2][6] = Bones::Ö±²¼ÓÒÏ¥ + 2;
	}
	//ÃüÂö
	{
		ÃüÂö[0][0] = Bones::head;
		ÃüÂö[0][1] = Bones::neck;
		ÃüÂö[0][2] = Bones::chest;
		ÃüÂö[0][3] = Bones::abdomen;
		ÃüÂö[0][4] = Bones::ŒÅ;

		ÃüÂö[1][0] = Bones::ÃüÂö×ó¼ç + 7;
		ÃüÂö[1][1] = Bones::ÃüÂö×ó¼ç + 5;
		ÃüÂö[1][2] = Bones::ÃüÂö×ó¼ç + 4;
		ÃüÂö[1][3] = Bones::ÃüÂö×ó¼ç + 2;
		ÃüÂö[1][4] = Bones::ÃüÂö×ó¼ç + 3;
		ÃüÂö[1][5] = Bones::ÃüÂö×ó¼ç + 1;
		ÃüÂö[1][6] = Bones::ÃüÂö×ó¼ç;
		ÃüÂö[1][7] = Bones::ÃüÂö×ó¼ç - 2;

		ÃüÂö[1][8] = Bones::ÃüÂöÓÒ¼ç - 2;
		ÃüÂö[1][9] = Bones::ÃüÂöÓÒ¼ç;
		ÃüÂö[1][10] = Bones::ÃüÂöÓÒ¼ç + 1;
		ÃüÂö[1][11] = Bones::ÃüÂöÓÒ¼ç + 3;
		ÃüÂö[1][12] = Bones::ÃüÂöÓÒ¼ç + 2;
		ÃüÂö[1][13] = Bones::ÃüÂöÓÒ¼ç + 4;
		ÃüÂö[1][14] = Bones::ÃüÂöÓÒ¼ç + 5;
		ÃüÂö[1][15] = Bones::ÃüÂöÓÒ¼ç + 7;

		ÃüÂö[2][0] = Bones::ÃüÂö×óÏ¥ + 2;
		ÃüÂö[2][1] = Bones::ÃüÂö×óÏ¥ + 1;
		ÃüÂö[2][2] = Bones::ÃüÂö×óÏ¥;
		ÃüÂö[2][3] = Bones::ŒÅ;
		ÃüÂö[2][4] = Bones::ÃüÂöÓÒÏ¥;
		ÃüÂö[2][5] = Bones::ÃüÂöÓÒÏ¥ + 1;
		ÃüÂö[2][6] = Bones::ÃüÂöÓÒÏ¥ + 2;
	}
	//Ì½Â·
	{
		Ì½Â·[0][0] = Bones::head;
		Ì½Â·[0][1] = Bones::neck;
		Ì½Â·[0][2] = Bones::chest;
		Ì½Â·[0][3] = Bones::abdomen;
		Ì½Â·[0][4] = Bones::ŒÅ;

		Ì½Â·[1][0] = Bones::Ì½Â·×ó¼ç + 6;
		Ì½Â·[1][1] = Bones::Ì½Â·×ó¼ç + 5;
		Ì½Â·[1][2] = Bones::Ì½Â·×ó¼ç + 4;
		Ì½Â·[1][3] = Bones::Ì½Â·×ó¼ç + 3;
		Ì½Â·[1][4] = Bones::Ì½Â·×ó¼ç + 2;
		Ì½Â·[1][5] = Bones::Ì½Â·×ó¼ç + 1;
		Ì½Â·[1][6] = Bones::Ì½Â·×ó¼ç;
		Ì½Â·[1][7] = Bones::Ì½Â·×ó¼ç - 1;

		Ì½Â·[1][8] = Bones::Ì½Â·ÓÒ¼ç - 1;
		Ì½Â·[1][9] = Bones::Ì½Â·ÓÒ¼ç;
		Ì½Â·[1][10] = Bones::Ì½Â·ÓÒ¼ç + 1;
		Ì½Â·[1][11] = Bones::Ì½Â·ÓÒ¼ç + 2;
		Ì½Â·[1][12] = Bones::Ì½Â·ÓÒ¼ç + 3;
		Ì½Â·[1][13] = Bones::Ì½Â·ÓÒ¼ç + 4;
		Ì½Â·[1][14] = Bones::Ì½Â·ÓÒ¼ç + 5;
		Ì½Â·[1][15] = Bones::Ì½Â·ÓÒ¼ç + 6;

		Ì½Â·[2][0] = Bones::Ì½Â·×óÏ¥ + 2;
		Ì½Â·[2][1] = Bones::Ì½Â·×óÏ¥ + 1;
		Ì½Â·[2][2] = Bones::Ì½Â·×óÏ¥;
		Ì½Â·[2][3] = Bones::ŒÅ;
		Ì½Â·[2][4] = Bones::Ì½Â·ÓÒÏ¥;
		Ì½Â·[2][5] = Bones::Ì½Â·ÓÒÏ¥ + 1;
		Ì½Â·[2][6] = Bones::Ì½Â·ÓÒÏ¥ + 2;
	}
	//¶¯Á¦
	{
		¶¯Á¦[0][0] = Bones::head;
		¶¯Á¦[0][1] = Bones::neck;
		¶¯Á¦[0][2] = Bones::chest;
		¶¯Á¦[0][3] = Bones::abdomen;
		¶¯Á¦[0][4] = Bones::ŒÅ;

		¶¯Á¦[1][0] = Bones::¶¯Á¦×ó¼ç + 7;
		¶¯Á¦[1][1] = Bones::¶¯Á¦×ó¼ç + 5;
		¶¯Á¦[1][2] = Bones::¶¯Á¦×ó¼ç + 4;
		¶¯Á¦[1][3] = Bones::¶¯Á¦×ó¼ç + 2;
		¶¯Á¦[1][4] = Bones::¶¯Á¦×ó¼ç + 3;
		¶¯Á¦[1][5] = Bones::¶¯Á¦×ó¼ç + 1;
		¶¯Á¦[1][6] = Bones::¶¯Á¦×ó¼ç;
		¶¯Á¦[1][7] = Bones::¶¯Á¦×ó¼ç - 2;

		¶¯Á¦[1][8] = Bones::¶¯Á¦ÓÒ¼ç - 2;
		¶¯Á¦[1][9] = Bones::¶¯Á¦ÓÒ¼ç;
		¶¯Á¦[1][10] = Bones::¶¯Á¦ÓÒ¼ç + 1;
		¶¯Á¦[1][11] = Bones::¶¯Á¦ÓÒ¼ç + 3;
		¶¯Á¦[1][12] = Bones::¶¯Á¦ÓÒ¼ç + 2;
		¶¯Á¦[1][13] = Bones::¶¯Á¦ÓÒ¼ç + 4;
		¶¯Á¦[1][14] = Bones::¶¯Á¦ÓÒ¼ç + 5;
		¶¯Á¦[1][15] = Bones::¶¯Á¦ÓÒ¼ç + 7;

		¶¯Á¦[2][0] = Bones::¶¯Á¦×óÏ¥ - 3;
		¶¯Á¦[2][1] = Bones::¶¯Á¦×óÏ¥ - 2;
		¶¯Á¦[2][2] = Bones::¶¯Á¦×óÏ¥;
		¶¯Á¦[2][3] = Bones::ŒÅ;
		¶¯Á¦[2][4] = Bones::¶¯Á¦ÓÒÏ¥;
		¶¯Á¦[2][5] = Bones::¶¯Á¦ÓÒÏ¥ + 1;
		¶¯Á¦[2][6] = Bones::¶¯Á¦ÓÒÏ¥ + 2;
	}
	//¶ñÁé
	{
		¶ñÁé[0][0] = Bones::head;
		¶ñÁé[0][1] = Bones::neck;
		¶ñÁé[0][2] = Bones::chest;
		¶ñÁé[0][3] = Bones::abdomen;
		¶ñÁé[0][4] = Bones::ŒÅ;

		¶ñÁé[1][0] = Bones::¶ñÁé×ó¼ç + 7;
		¶ñÁé[1][1] = Bones::¶ñÁé×ó¼ç + 5;
		¶ñÁé[1][2] = Bones::¶ñÁé×ó¼ç + 4;
		¶ñÁé[1][3] = Bones::¶ñÁé×ó¼ç + 2;
		¶ñÁé[1][4] = Bones::¶ñÁé×ó¼ç + 3;
		¶ñÁé[1][5] = Bones::¶ñÁé×ó¼ç + 1;
		¶ñÁé[1][6] = Bones::¶ñÁé×ó¼ç;
		¶ñÁé[1][7] = Bones::¶ñÁé×ó¼ç - 2;

		¶ñÁé[1][8] = Bones::¶ñÁéÓÒ¼ç - 2;
		¶ñÁé[1][9] = Bones::¶ñÁéÓÒ¼ç;
		¶ñÁé[1][10] = Bones::¶ñÁéÓÒ¼ç + 1;
		¶ñÁé[1][11] = Bones::¶ñÁéÓÒ¼ç + 3;
		¶ñÁé[1][12] = Bones::¶ñÁéÓÒ¼ç + 2;
		¶ñÁé[1][13] = Bones::¶ñÁéÓÒ¼ç + 4;
		¶ñÁé[1][14] = Bones::¶ñÁéÓÒ¼ç + 5;
		¶ñÁé[1][15] = Bones::¶ñÁéÓÒ¼ç + 7;

		¶ñÁé[2][0] = Bones::¶ñÁé×óÏ¥ + 2;
		¶ñÁé[2][1] = Bones::¶ñÁé×óÏ¥ + 1;
		¶ñÁé[2][2] = Bones::¶ñÁé×óÏ¥;
		¶ñÁé[2][3] = Bones::ŒÅ;
		¶ñÁé[2][4] = Bones::¶ñÁéÓÒÏ¥;
		¶ñÁé[2][5] = Bones::¶ñÁéÓÒÏ¥ + 1;
		¶ñÁé[2][6] = Bones::¶ñÁéÓÒÏ¥ + 2;
	}
	//°à¼Ó
	{
		°à¼Ó[0][0] = Bones::head;
		°à¼Ó[0][1] = Bones::neck;
		°à¼Ó[0][2] = Bones::chest;
		°à¼Ó[0][3] = Bones::abdomen;
		°à¼Ó[0][4] = Bones::ŒÅ;

		°à¼Ó[1][0] = Bones::°à¼Ó×ó¼ç + 7;
		°à¼Ó[1][1] = Bones::°à¼Ó×ó¼ç + 5;
		°à¼Ó[1][2] = Bones::°à¼Ó×ó¼ç + 4;
		°à¼Ó[1][3] = Bones::°à¼Ó×ó¼ç + 2;
		°à¼Ó[1][4] = Bones::°à¼Ó×ó¼ç + 3;
		°à¼Ó[1][5] = Bones::°à¼Ó×ó¼ç + 1;
		°à¼Ó[1][6] = Bones::°à¼Ó×ó¼ç;
		°à¼Ó[1][7] = Bones::°à¼Ó×ó¼ç - 2;

		°à¼Ó[1][8] = Bones::°à¼ÓÓÒ¼ç - 2;
		°à¼Ó[1][9] = Bones::°à¼ÓÓÒ¼ç;
		°à¼Ó[1][10] = Bones::°à¼ÓÓÒ¼ç + 1;
		°à¼Ó[1][11] = Bones::°à¼ÓÓÒ¼ç + 3;
		°à¼Ó[1][12] = Bones::°à¼ÓÓÒ¼ç + 2;
		°à¼Ó[1][13] = Bones::°à¼ÓÓÒ¼ç + 4;
		°à¼Ó[1][14] = Bones::°à¼ÓÓÒ¼ç + 5;
		°à¼Ó[1][15] = Bones::°à¼ÓÓÒ¼ç + 7;

		°à¼Ó[2][0] = Bones::°à¼Ó×óÏ¥ + 2;
		°à¼Ó[2][1] = Bones::°à¼Ó×óÏ¥ + 1;
		°à¼Ó[2][2] = Bones::°à¼Ó×óÏ¥;
		°à¼Ó[2][3] = Bones::ŒÅ;
		°à¼Ó[2][4] = Bones::°à¼ÓÓÒÏ¥;
		°à¼Ó[2][5] = Bones::°à¼ÓÓÒÏ¥ + 1;
		°à¼Ó[2][6] = Bones::°à¼ÓÓÒÏ¥ + 2;
	}
	//ÇÖÊ´
	{
		ÇÖÊ´[0][0] = Bones::head;
		ÇÖÊ´[0][1] = Bones::neck;
		ÇÖÊ´[0][2] = Bones::chest;
		ÇÖÊ´[0][3] = Bones::abdomen;
		ÇÖÊ´[0][4] = Bones::ŒÅ;

		ÇÖÊ´[1][0] = Bones::ÇÖÊ´×ó¼ç + 7;
		ÇÖÊ´[1][1] = Bones::ÇÖÊ´×ó¼ç + 5;
		ÇÖÊ´[1][2] = Bones::ÇÖÊ´×ó¼ç + 4;
		ÇÖÊ´[1][3] = Bones::ÇÖÊ´×ó¼ç + 2;
		ÇÖÊ´[1][4] = Bones::ÇÖÊ´×ó¼ç + 3;
		ÇÖÊ´[1][5] = Bones::ÇÖÊ´×ó¼ç + 1;
		ÇÖÊ´[1][6] = Bones::ÇÖÊ´×ó¼ç;
		ÇÖÊ´[1][7] = Bones::ÇÖÊ´×ó¼ç - 2;

		ÇÖÊ´[1][8] = Bones::ÇÖÊ´ÓÒ¼ç - 2;
		ÇÖÊ´[1][9] = Bones::ÇÖÊ´ÓÒ¼ç;
		ÇÖÊ´[1][10] = Bones::ÇÖÊ´ÓÒ¼ç + 1;
		ÇÖÊ´[1][11] = Bones::ÇÖÊ´ÓÒ¼ç + 3;
		ÇÖÊ´[1][12] = Bones::ÇÖÊ´ÓÒ¼ç + 2;
		ÇÖÊ´[1][13] = Bones::ÇÖÊ´ÓÒ¼ç + 4;
		ÇÖÊ´[1][14] = Bones::ÇÖÊ´ÓÒ¼ç + 5;
		ÇÖÊ´[1][15] = Bones::ÇÖÊ´ÓÒ¼ç + 7;

		ÇÖÊ´[2][0] = Bones::ÇÖÊ´×óÏ¥ + 2;
		ÇÖÊ´[2][1] = Bones::ÇÖÊ´×óÏ¥ + 1;
		ÇÖÊ´[2][2] = Bones::ÇÖÊ´×óÏ¥;
		ÇÖÊ´[2][3] = Bones::ŒÅ;
		ÇÖÊ´[2][4] = Bones::ÇÖÊ´ÓÒÏ¥;
		ÇÖÊ´[2][5] = Bones::ÇÖÊ´ÓÒÏ¥ + 1;
		ÇÖÊ´[2][6] = Bones::ÇÖÊ´ÓÒÏ¥ + 2;
	}
	//»ÃÏó
	{
		»ÃÏó[0][0] = Bones::head;
		»ÃÏó[0][1] = Bones::neck;
		»ÃÏó[0][2] = Bones::chest;
		»ÃÏó[0][3] = Bones::abdomen;
		»ÃÏó[0][4] = Bones::ŒÅ;

		»ÃÏó[1][0] = Bones::»ÃÏó×ó¼ç + 7;
		»ÃÏó[1][1] = Bones::»ÃÏó×ó¼ç + 5;
		»ÃÏó[1][2] = Bones::»ÃÏó×ó¼ç + 4;
		»ÃÏó[1][3] = Bones::»ÃÏó×ó¼ç + 2;
		»ÃÏó[1][4] = Bones::»ÃÏó×ó¼ç + 3;
		»ÃÏó[1][5] = Bones::»ÃÏó×ó¼ç + 1;
		»ÃÏó[1][6] = Bones::»ÃÏó×ó¼ç;
		»ÃÏó[1][7] = Bones::»ÃÏó×ó¼ç - 1;

		»ÃÏó[1][8] = Bones::»ÃÏóÓÒ¼ç - 1;
		»ÃÏó[1][9] = Bones::»ÃÏóÓÒ¼ç;
		»ÃÏó[1][10] = Bones::»ÃÏóÓÒ¼ç + 1;
		»ÃÏó[1][11] = Bones::»ÃÏóÓÒ¼ç + 3;
		»ÃÏó[1][12] = Bones::»ÃÏóÓÒ¼ç + 2;
		»ÃÏó[1][13] = Bones::»ÃÏóÓÒ¼ç + 4;
		»ÃÏó[1][14] = Bones::»ÃÏóÓÒ¼ç + 5;
		»ÃÏó[1][15] = Bones::»ÃÏóÓÒ¼ç + 7;

		»ÃÏó[2][0] = Bones::»ÃÏó×óÏ¥ + 2;
		»ÃÏó[2][1] = Bones::»ÃÏó×óÏ¥ + 1;
		»ÃÏó[2][2] = Bones::»ÃÏó×óÏ¥;
		»ÃÏó[2][3] = Bones::ŒÅ;
		»ÃÏó[2][4] = Bones::»ÃÏóÓÒÏ¥;
		»ÃÏó[2][5] = Bones::»ÃÏóÓÒÏ¥ + 1;
		»ÃÏó[2][6] = Bones::»ÃÏóÓÒÏ¥ + 2;
	}
	//»ªÉ­
	{
		»ªÉ­[0][0] = Bones::head;
		»ªÉ­[0][1] = Bones::neck;
		»ªÉ­[0][2] = Bones::chest;
		»ªÉ­[0][3] = Bones::abdomen;
		»ªÉ­[0][4] = Bones::ŒÅ;

		»ªÉ­[1][0] = Bones::»ªÉ­×ó¼ç + 7;
		»ªÉ­[1][1] = Bones::»ªÉ­×ó¼ç + 5;
		»ªÉ­[1][2] = Bones::»ªÉ­×ó¼ç + 4;
		»ªÉ­[1][3] = Bones::»ªÉ­×ó¼ç + 2;
		»ªÉ­[1][4] = Bones::»ªÉ­×ó¼ç + 3;
		»ªÉ­[1][5] = Bones::»ªÉ­×ó¼ç + 1;
		»ªÉ­[1][6] = Bones::»ªÉ­×ó¼ç;
		»ªÉ­[1][7] = Bones::»ªÉ­×ó¼ç - 2;

		»ªÉ­[1][8] = Bones::»ªÉ­ÓÒ¼ç - 2;
		»ªÉ­[1][9] = Bones::»ªÉ­ÓÒ¼ç;
		»ªÉ­[1][10] = Bones::»ªÉ­ÓÒ¼ç + 1;
		»ªÉ­[1][11] = Bones::»ªÉ­ÓÒ¼ç + 3;
		»ªÉ­[1][12] = Bones::»ªÉ­ÓÒ¼ç + 2;
		»ªÉ­[1][13] = Bones::»ªÉ­ÓÒ¼ç + 4;
		»ªÉ­[1][14] = Bones::»ªÉ­ÓÒ¼ç + 5;
		»ªÉ­[1][15] = Bones::»ªÉ­ÓÒ¼ç + 7;

		»ªÉ­[2][0] = Bones::»ªÉ­×óÏ¥ + 2;
		»ªÉ­[2][1] = Bones::»ªÉ­×óÏ¥ + 1;
		»ªÉ­[2][2] = Bones::»ªÉ­×óÏ¥;
		»ªÉ­[2][3] = Bones::ŒÅ;
		»ªÉ­[2][4] = Bones::»ªÉ­ÓÒÏ¥;
		»ªÉ­[2][5] = Bones::»ªÉ­ÓÒÏ¥ + 1;
		»ªÉ­[2][6] = Bones::»ªÉ­ÓÒÏ¥ + 2;
	}
}