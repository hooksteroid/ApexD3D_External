#include "proc_help.h"
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <random>
#include "imgui/imgui.h"
#include "hackerThreads.h"

ColorStruct colors;//��Ʒ��ɫ�ṹ��
AppConfigs appConfigs;//�˵�
int menu_zuobian;//���
int menu_dingbian;//����
int menu_kuan;//��
int menu_gao;//��
int menu_hangju;//�о�
int menu_select;
int menu_size;//��С
int menu_fenge;//�ָ�
int nengnenglvse;
int nengnenglanse;
std::hash_map<int, ApexItem> entityNames;
HANDLE hAimThread;//��׼�߳̾��

int ѰѪ[3][16] = {};
int ֱ��[3][16] = {};
int ����[3][16] = {};
int ̽·[3][16] = {};
int ����[3][16] = {};

int ����[3][16] = {};
int ���[3][16] = {};
int ��ʴ[3][16] = {};
int ����[3][16] = {};
int ��ɭ[3][16] = {};

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
/*�û������ȡ���pid*/
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
void initConfig() {//��ʼ����Ϣ
	appConfigs = { true, false, 300.f, true,  false, 50.f, true, true, true, false, true, true, 150.f, 2, true };
	DWORD tid = 0;
	HANDLE tHandle = CreateThread(NULL, 0, KeyBoardHookThread, 0, 0, &tid);
	CloseHandle(tHandle);
}

DWORD WINAPI KeyBoardHookThread(LPVOID lpParam) {
	keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHook, 0, 0);
	if (keyHook == 0) {
		MessageBoxA(NULL, "��װ���̹���ʧ��", NULL, 0);
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
	//entityNames[3] = { u8"���ֿ�", colors.ShiShi };
	entityNames[4] = { u8"L-STAR", colors.NengLiang };
	//entityNames[5] = { u8"���ֿ�[��]", colors.ShiShi };
	//entityNames[6] = { u8"HAVOC", colors.NengLiang };
	//entityNames[7] = { u8"HAVOC", colors.NengLiang };
	entityNames[8] = { u8"DEVOTION", colors.NengLiang };
	//entityNames[9] = { u8"HAVOC", colors.NengLiang };
	//entityNames[10] = { u8"TRIPLE TAKE", colors.ZhongXing };
	//entityNames[12] = { u8"VK-47", colors.ZhongXing };
	//entityNames[13] = { u8"G7�ѻ�ǹ", colors.QingXing };
	//entityNames[15] = { u8"ת���߳��ǹ", colors.QingXing };
	//entityNames[16] = { u8"HEMLOK", colors.ZhongXing };
	entityNames[17] = { u8"R99���ǹ", colors.QingXing };
	//entityNames[18] = { u8"���޳��ǹ", colors.ZhongXing };
	//entityNames[19] = { u8"ǱϮ���ǹ[��]", colors.ShiShi };
	//entityNames[20] = { u8"G7", colors.ZhongXing };
	//entityNames[22] = { u8"ALTERANTOR", colors.ShiShi };
	entityNames[23] = { u8"R301", colors.QingXing };
	//entityNames[28] = { u8"Īɣ�ȿ�", colors.SanDan };
	entityNames[26] = { u8"R-99", colors.ShiShi };
	//entityNames[25] = { u8"EVA-8", colors.SanDan };
	//entityNames[30] = { u8"PROWLER", colors.ZhongXing };
	//entityNames[31] = { u8"�޵�����[��]", colors.ShiShi };
	entityNames[32] = { u8"VOLT SMG", colors.NengLiang };
	entityNames[33] = { u8"VOLT SMG", colors.QingXing };
	//entityNames[34] = { u8"LONG BOW", colors.QingXing };
	//entityNames[35] = { u8"LONG BOW", colors.QingXing };
	//entityNames[36] = { u8"CHARGE", colors.QingXing };

	//entityNames[37] = { u8"���͵�ҩ", colors.QingXing };
	entityNames[38] = { u8"M600", colors.ZhongXing };
	//entityNames[39] = { u8"����ǹ��ҩ", colors.SanDan };
	entityNames[40] = { u8"R301", colors.QingXing };

	//entityNames[42] = { u8"���", colors.ShengQi };
	//entityNames[43] = { u8"�󼱾���", colors.ShengQi };
	//entityNames[44] = { u8"EVA-8", colors.ShengQi };
	//entityNames[45] = { u8"�󻤶ܵ��", colors.ShengQi };
	//entityNames[46] = { u8"С���ܵ��", colors.ShengQi };

	//entityNames[47] = { u8"ͷ lv.1", colors.PuTong };
	//entityNames[48] = { u8"ͷ lv.2", colors.GaoJi };
	//entityNames[49] = { u8"PEACE KEEPER", colors.ShiShi };
	//entityNames[50] = { u8"MOZAMBIQUE", colors.ShiShi };//ͷ lv.4

	//entityNames[51] = { u8"�� lv.1", colors.PuTong };
	//entityNames[52] = { u8"WINGMAN", colors.GaoJi };
	//entityNames[53] = { u8"�� lv.3", colors.ShiShi };
	//entityNames[54] = { u8"�� lv.4", colors.ShiShi };

	//entityNames[55] = { u8"�� lv.1", colors.PuTong };
	//entityNames[56] = { u8"P2020", colors.QingXing };
	//entityNames[57] = { u8"�� lv.3", colors.ShengQi };
	//entityNames[58] = { u8"RE-45", colors.ShiShi };

	//entityNames[59] = { u8"�� lv.1", colors.PuTong };
	//entityNames[60] = { u8"SENTINEL", colors.GaoJi };
	//entityNames[61] = { u8"�� lv.3", colors.ShiShi };
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

	//entityNames[86] = { u8"KNOWDOWN S lv.1", colors.PuTong }; //��������Lv.1
	//entityNames[87] = { u8"KNOWDOWN S lv.2", colors.GaoJi };
	//entityNames[88] = { u8"KNOWDOWN S lv.3", colors.ShengQi };
	//entityNames[89] = { u8"KNOWDOWN S lv.4", colors.PuTong };//����ǹͷLv.1

	//entityNames[90] = { u8"BACKPACK lv.1", colors.GaoJi };//����ǹͷLv.2
	//entityNames[91] = { u8"BACKPACK lv.2", colors.ShengQi };//����ǹͷLv.3
	//entityNames[92] = { u8"BACKPACK lv.3", colors.ShengQi };//ǹƨ��Lv.1
	entityNames[93] = { u8"BACKPACK lv.4", colors.ShiShi };//ǹƨ��Lv.2

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
	//ѰѪ
	{
		ѰѪ[0][0] = Bones::head;
		ѰѪ[0][1] = Bones::neck;
		ѰѪ[0][2] = Bones::chest;
		ѰѪ[0][3] = Bones::abdomen;
		ѰѪ[0][4] = Bones::��;

		ѰѪ[1][0] = Bones::ѰѪ��� + 7;
		ѰѪ[1][1] = Bones::ѰѪ��� + 5;
		ѰѪ[1][2] = Bones::ѰѪ��� + 4;
		ѰѪ[1][3] = Bones::ѰѪ��� + 2;
		ѰѪ[1][4] = Bones::ѰѪ��� + 3;
		ѰѪ[1][5] = Bones::ѰѪ��� + 1;
		ѰѪ[1][6] = Bones::ѰѪ���;
		ѰѪ[1][7] = Bones::ѰѪ��� - 2;

		ѰѪ[1][8] = Bones::ѰѪ�Ҽ� - 2;
		ѰѪ[1][9] = Bones::ѰѪ�Ҽ�;
		ѰѪ[1][10] = Bones::ѰѪ�Ҽ� + 1;
		ѰѪ[1][11] = Bones::ѰѪ�Ҽ� + 3;
		ѰѪ[1][12] = Bones::ѰѪ�Ҽ� + 2;
		ѰѪ[1][13] = Bones::ѰѪ�Ҽ� + 4;
		ѰѪ[1][14] = Bones::ѰѪ�Ҽ� + 5;
		ѰѪ[1][15] = Bones::ѰѪ�Ҽ� + 7;

		ѰѪ[2][0] = Bones::ѰѪ��ϥ + 2;
		ѰѪ[2][1] = Bones::ѰѪ��ϥ + 1;
		ѰѪ[2][2] = Bones::ѰѪ��ϥ;
		ѰѪ[2][3] = Bones::��;
		ѰѪ[2][4] = Bones::ѰѪ��ϥ;
		ѰѪ[2][5] = Bones::ѰѪ��ϥ + 1;
		ѰѪ[2][6] = Bones::ѰѪ��ϥ + 2;
	}
	//ֱ��
	{
		ֱ��[0][0] = Bones::head;
		ֱ��[0][1] = Bones::neck;
		ֱ��[0][2] = Bones::chest;
		ֱ��[0][3] = Bones::abdomen;
		ֱ��[0][4] = Bones::��;

		ֱ��[1][0] = Bones::ֱ����� + 7;
		ֱ��[1][1] = Bones::ֱ����� + 5;
		ֱ��[1][2] = Bones::ֱ����� + 4;
		ֱ��[1][3] = Bones::ֱ����� + 2;
		ֱ��[1][4] = Bones::ֱ����� + 3;
		ֱ��[1][5] = Bones::ֱ����� + 1;
		ֱ��[1][6] = Bones::ֱ�����;
		ֱ��[1][7] = Bones::ֱ����� - 2;

		ֱ��[1][8] = Bones::ֱ���Ҽ� - 2;
		ֱ��[1][9] = Bones::ֱ���Ҽ�;
		ֱ��[1][10] = Bones::ֱ���Ҽ� + 1;
		ֱ��[1][11] = Bones::ֱ���Ҽ� + 3;
		ֱ��[1][12] = Bones::ֱ���Ҽ� + 2;
		ֱ��[1][13] = Bones::ֱ���Ҽ� + 4;
		ֱ��[1][14] = Bones::ֱ���Ҽ� + 5;
		ֱ��[1][15] = Bones::ֱ���Ҽ� + 7;

		ֱ��[2][0] = Bones::ֱ����ϥ + 2;
		ֱ��[2][1] = Bones::ֱ����ϥ + 1;
		ֱ��[2][2] = Bones::ֱ����ϥ;
		ֱ��[2][3] = Bones::��;
		ֱ��[2][4] = Bones::ֱ����ϥ;
		ֱ��[2][5] = Bones::ֱ����ϥ + 1;
		ֱ��[2][6] = Bones::ֱ����ϥ + 2;
	}
	//����
	{
		����[0][0] = Bones::head;
		����[0][1] = Bones::neck;
		����[0][2] = Bones::chest;
		����[0][3] = Bones::abdomen;
		����[0][4] = Bones::��;

		����[1][0] = Bones::������� + 7;
		����[1][1] = Bones::������� + 5;
		����[1][2] = Bones::������� + 4;
		����[1][3] = Bones::������� + 2;
		����[1][4] = Bones::������� + 3;
		����[1][5] = Bones::������� + 1;
		����[1][6] = Bones::�������;
		����[1][7] = Bones::������� - 2;

		����[1][8] = Bones::�����Ҽ� - 2;
		����[1][9] = Bones::�����Ҽ�;
		����[1][10] = Bones::�����Ҽ� + 1;
		����[1][11] = Bones::�����Ҽ� + 3;
		����[1][12] = Bones::�����Ҽ� + 2;
		����[1][13] = Bones::�����Ҽ� + 4;
		����[1][14] = Bones::�����Ҽ� + 5;
		����[1][15] = Bones::�����Ҽ� + 7;

		����[2][0] = Bones::������ϥ + 2;
		����[2][1] = Bones::������ϥ + 1;
		����[2][2] = Bones::������ϥ;
		����[2][3] = Bones::��;
		����[2][4] = Bones::������ϥ;
		����[2][5] = Bones::������ϥ + 1;
		����[2][6] = Bones::������ϥ + 2;
	}
	//̽·
	{
		̽·[0][0] = Bones::head;
		̽·[0][1] = Bones::neck;
		̽·[0][2] = Bones::chest;
		̽·[0][3] = Bones::abdomen;
		̽·[0][4] = Bones::��;

		̽·[1][0] = Bones::̽·��� + 6;
		̽·[1][1] = Bones::̽·��� + 5;
		̽·[1][2] = Bones::̽·��� + 4;
		̽·[1][3] = Bones::̽·��� + 3;
		̽·[1][4] = Bones::̽·��� + 2;
		̽·[1][5] = Bones::̽·��� + 1;
		̽·[1][6] = Bones::̽·���;
		̽·[1][7] = Bones::̽·��� - 1;

		̽·[1][8] = Bones::̽·�Ҽ� - 1;
		̽·[1][9] = Bones::̽·�Ҽ�;
		̽·[1][10] = Bones::̽·�Ҽ� + 1;
		̽·[1][11] = Bones::̽·�Ҽ� + 2;
		̽·[1][12] = Bones::̽·�Ҽ� + 3;
		̽·[1][13] = Bones::̽·�Ҽ� + 4;
		̽·[1][14] = Bones::̽·�Ҽ� + 5;
		̽·[1][15] = Bones::̽·�Ҽ� + 6;

		̽·[2][0] = Bones::̽·��ϥ + 2;
		̽·[2][1] = Bones::̽·��ϥ + 1;
		̽·[2][2] = Bones::̽·��ϥ;
		̽·[2][3] = Bones::��;
		̽·[2][4] = Bones::̽·��ϥ;
		̽·[2][5] = Bones::̽·��ϥ + 1;
		̽·[2][6] = Bones::̽·��ϥ + 2;
	}
	//����
	{
		����[0][0] = Bones::head;
		����[0][1] = Bones::neck;
		����[0][2] = Bones::chest;
		����[0][3] = Bones::abdomen;
		����[0][4] = Bones::��;

		����[1][0] = Bones::������� + 7;
		����[1][1] = Bones::������� + 5;
		����[1][2] = Bones::������� + 4;
		����[1][3] = Bones::������� + 2;
		����[1][4] = Bones::������� + 3;
		����[1][5] = Bones::������� + 1;
		����[1][6] = Bones::�������;
		����[1][7] = Bones::������� - 2;

		����[1][8] = Bones::�����Ҽ� - 2;
		����[1][9] = Bones::�����Ҽ�;
		����[1][10] = Bones::�����Ҽ� + 1;
		����[1][11] = Bones::�����Ҽ� + 3;
		����[1][12] = Bones::�����Ҽ� + 2;
		����[1][13] = Bones::�����Ҽ� + 4;
		����[1][14] = Bones::�����Ҽ� + 5;
		����[1][15] = Bones::�����Ҽ� + 7;

		����[2][0] = Bones::������ϥ - 3;
		����[2][1] = Bones::������ϥ - 2;
		����[2][2] = Bones::������ϥ;
		����[2][3] = Bones::��;
		����[2][4] = Bones::������ϥ;
		����[2][5] = Bones::������ϥ + 1;
		����[2][6] = Bones::������ϥ + 2;
	}
	//����
	{
		����[0][0] = Bones::head;
		����[0][1] = Bones::neck;
		����[0][2] = Bones::chest;
		����[0][3] = Bones::abdomen;
		����[0][4] = Bones::��;

		����[1][0] = Bones::������� + 7;
		����[1][1] = Bones::������� + 5;
		����[1][2] = Bones::������� + 4;
		����[1][3] = Bones::������� + 2;
		����[1][4] = Bones::������� + 3;
		����[1][5] = Bones::������� + 1;
		����[1][6] = Bones::�������;
		����[1][7] = Bones::������� - 2;

		����[1][8] = Bones::�����Ҽ� - 2;
		����[1][9] = Bones::�����Ҽ�;
		����[1][10] = Bones::�����Ҽ� + 1;
		����[1][11] = Bones::�����Ҽ� + 3;
		����[1][12] = Bones::�����Ҽ� + 2;
		����[1][13] = Bones::�����Ҽ� + 4;
		����[1][14] = Bones::�����Ҽ� + 5;
		����[1][15] = Bones::�����Ҽ� + 7;

		����[2][0] = Bones::������ϥ + 2;
		����[2][1] = Bones::������ϥ + 1;
		����[2][2] = Bones::������ϥ;
		����[2][3] = Bones::��;
		����[2][4] = Bones::������ϥ;
		����[2][5] = Bones::������ϥ + 1;
		����[2][6] = Bones::������ϥ + 2;
	}
	//���
	{
		���[0][0] = Bones::head;
		���[0][1] = Bones::neck;
		���[0][2] = Bones::chest;
		���[0][3] = Bones::abdomen;
		���[0][4] = Bones::��;

		���[1][0] = Bones::������ + 7;
		���[1][1] = Bones::������ + 5;
		���[1][2] = Bones::������ + 4;
		���[1][3] = Bones::������ + 2;
		���[1][4] = Bones::������ + 3;
		���[1][5] = Bones::������ + 1;
		���[1][6] = Bones::������;
		���[1][7] = Bones::������ - 2;

		���[1][8] = Bones::����Ҽ� - 2;
		���[1][9] = Bones::����Ҽ�;
		���[1][10] = Bones::����Ҽ� + 1;
		���[1][11] = Bones::����Ҽ� + 3;
		���[1][12] = Bones::����Ҽ� + 2;
		���[1][13] = Bones::����Ҽ� + 4;
		���[1][14] = Bones::����Ҽ� + 5;
		���[1][15] = Bones::����Ҽ� + 7;

		���[2][0] = Bones::�����ϥ + 2;
		���[2][1] = Bones::�����ϥ + 1;
		���[2][2] = Bones::�����ϥ;
		���[2][3] = Bones::��;
		���[2][4] = Bones::�����ϥ;
		���[2][5] = Bones::�����ϥ + 1;
		���[2][6] = Bones::�����ϥ + 2;
	}
	//��ʴ
	{
		��ʴ[0][0] = Bones::head;
		��ʴ[0][1] = Bones::neck;
		��ʴ[0][2] = Bones::chest;
		��ʴ[0][3] = Bones::abdomen;
		��ʴ[0][4] = Bones::��;

		��ʴ[1][0] = Bones::��ʴ��� + 7;
		��ʴ[1][1] = Bones::��ʴ��� + 5;
		��ʴ[1][2] = Bones::��ʴ��� + 4;
		��ʴ[1][3] = Bones::��ʴ��� + 2;
		��ʴ[1][4] = Bones::��ʴ��� + 3;
		��ʴ[1][5] = Bones::��ʴ��� + 1;
		��ʴ[1][6] = Bones::��ʴ���;
		��ʴ[1][7] = Bones::��ʴ��� - 2;

		��ʴ[1][8] = Bones::��ʴ�Ҽ� - 2;
		��ʴ[1][9] = Bones::��ʴ�Ҽ�;
		��ʴ[1][10] = Bones::��ʴ�Ҽ� + 1;
		��ʴ[1][11] = Bones::��ʴ�Ҽ� + 3;
		��ʴ[1][12] = Bones::��ʴ�Ҽ� + 2;
		��ʴ[1][13] = Bones::��ʴ�Ҽ� + 4;
		��ʴ[1][14] = Bones::��ʴ�Ҽ� + 5;
		��ʴ[1][15] = Bones::��ʴ�Ҽ� + 7;

		��ʴ[2][0] = Bones::��ʴ��ϥ + 2;
		��ʴ[2][1] = Bones::��ʴ��ϥ + 1;
		��ʴ[2][2] = Bones::��ʴ��ϥ;
		��ʴ[2][3] = Bones::��;
		��ʴ[2][4] = Bones::��ʴ��ϥ;
		��ʴ[2][5] = Bones::��ʴ��ϥ + 1;
		��ʴ[2][6] = Bones::��ʴ��ϥ + 2;
	}
	//����
	{
		����[0][0] = Bones::head;
		����[0][1] = Bones::neck;
		����[0][2] = Bones::chest;
		����[0][3] = Bones::abdomen;
		����[0][4] = Bones::��;

		����[1][0] = Bones::������� + 7;
		����[1][1] = Bones::������� + 5;
		����[1][2] = Bones::������� + 4;
		����[1][3] = Bones::������� + 2;
		����[1][4] = Bones::������� + 3;
		����[1][5] = Bones::������� + 1;
		����[1][6] = Bones::�������;
		����[1][7] = Bones::������� - 1;

		����[1][8] = Bones::�����Ҽ� - 1;
		����[1][9] = Bones::�����Ҽ�;
		����[1][10] = Bones::�����Ҽ� + 1;
		����[1][11] = Bones::�����Ҽ� + 3;
		����[1][12] = Bones::�����Ҽ� + 2;
		����[1][13] = Bones::�����Ҽ� + 4;
		����[1][14] = Bones::�����Ҽ� + 5;
		����[1][15] = Bones::�����Ҽ� + 7;

		����[2][0] = Bones::������ϥ + 2;
		����[2][1] = Bones::������ϥ + 1;
		����[2][2] = Bones::������ϥ;
		����[2][3] = Bones::��;
		����[2][4] = Bones::������ϥ;
		����[2][5] = Bones::������ϥ + 1;
		����[2][6] = Bones::������ϥ + 2;
	}
	//��ɭ
	{
		��ɭ[0][0] = Bones::head;
		��ɭ[0][1] = Bones::neck;
		��ɭ[0][2] = Bones::chest;
		��ɭ[0][3] = Bones::abdomen;
		��ɭ[0][4] = Bones::��;

		��ɭ[1][0] = Bones::��ɭ��� + 7;
		��ɭ[1][1] = Bones::��ɭ��� + 5;
		��ɭ[1][2] = Bones::��ɭ��� + 4;
		��ɭ[1][3] = Bones::��ɭ��� + 2;
		��ɭ[1][4] = Bones::��ɭ��� + 3;
		��ɭ[1][5] = Bones::��ɭ��� + 1;
		��ɭ[1][6] = Bones::��ɭ���;
		��ɭ[1][7] = Bones::��ɭ��� - 2;

		��ɭ[1][8] = Bones::��ɭ�Ҽ� - 2;
		��ɭ[1][9] = Bones::��ɭ�Ҽ�;
		��ɭ[1][10] = Bones::��ɭ�Ҽ� + 1;
		��ɭ[1][11] = Bones::��ɭ�Ҽ� + 3;
		��ɭ[1][12] = Bones::��ɭ�Ҽ� + 2;
		��ɭ[1][13] = Bones::��ɭ�Ҽ� + 4;
		��ɭ[1][14] = Bones::��ɭ�Ҽ� + 5;
		��ɭ[1][15] = Bones::��ɭ�Ҽ� + 7;

		��ɭ[2][0] = Bones::��ɭ��ϥ + 2;
		��ɭ[2][1] = Bones::��ɭ��ϥ + 1;
		��ɭ[2][2] = Bones::��ɭ��ϥ;
		��ɭ[2][3] = Bones::��;
		��ɭ[2][4] = Bones::��ɭ��ϥ;
		��ɭ[2][5] = Bones::��ɭ��ϥ + 1;
		��ɭ[2][6] = Bones::��ɭ��ϥ + 2;
	}
}