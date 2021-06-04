#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include "proc_help.h"
#include "DriverHelp.h"
#include "DrawEng.h"
#include "hackerThreads.h"
#include <atlstr.h>
#include <chrono>
#include <string>

//#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")//不显示窗口
//#pragma comment(linker,"/MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR")//减小编译体积
using namespace std;
int gamePID;
__int64 hGameModule;
int fontSize;
HWND hGameWind;

int main(int argc, char** argv) {
	//HWND hWnd = GetForegroundWindow();
	//SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	std::chrono::milliseconds uptime = std::chrono::milliseconds(GetTickCount64());
	printf(StrA("UP TIME: %lld s\n"), uptime.count() / 1000);
	auto boot_time = std::chrono::system_clock::now() - uptime;
	printf(StrA("BOOT TIME: %lld\n"), ((boot_time.time_since_epoch().count() / 10000000) / 60) * 60);
	initColor();
	initConfig();
	initMenu();
	if (connectDrv()) {
		printf("Connected");
	}
	hGameWind = 0;
	while (!hGameWind) {
		hGameWind = FindWindowA(StrA("Respawn001"), StrA("Apex Legends"));
		Sleep(100);
	}
	gamePID = GetPID(L"r5apex.exe"); //(HANDLE)GetProcessIDByName(StrA("r5apex.exe"));
	//SuspendThread(gamePID);
	if (!gamePID) {
		MessageBoxA(nullptr, StrA(" Cant conn r5apex.exe"), nullptr, 0);
		return -1;
	}
	//printf(StrA("客户端Pid: %d\n"), _getpid());
	hGameModule = GetBase(L"r5apex.exe");//GetBaseAddr(pn);// getBaseModule(gamePID);
	printf(StrA("\nGamePid: %d \n 游戏基址: %I64x\n"), gamePID, hGameModule);
	//check_version();
	ShowWindow(hGameWind, 9);
	fontSize = 64;
	startThreads();
	StartDraw(0);
	return 0;
}