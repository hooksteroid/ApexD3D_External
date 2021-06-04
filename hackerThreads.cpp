#include <Windows.h>
#include "proc_help.h"
#include "hackerThreads.h"
#include "DriverHelp.h"
#include "apexUtils.h"
#include <vector>
#include <cmath>
#include "DrawEng.h"

using namespace std;

POINT gamePoint = {};
RECT gameRect = {};
__int64 TempPoint = 0;
__int64 ArrayPoint = 0;
__int64 MySelfPoint = 0;
__int64 EntityListPoint = 0;
float worldArray[4][4] = {
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
};
int MyTeam;
__int64 _VIEWANGLE = 0;
std::vector<ApexEntity> apexEntityList;
bool aim;
bool aimThreadStop;
__int64 aimEntity;
int windowW;
int windowH;
POINT CentWindow;

/*��Ϣ�߳�*/
DWORD WINAPI InfoThread(LPVOID lpParam) {
	int a = false;
	CentWindow = { 0, 0 };
	while (!a) {
		DWORD gameID = GetProcessIDByName("r5apex.exe");
		if (gameID == 0) exit(0);
		POINT tempGamePoint = {};
		RECT tempGameRect = {};
		ClientToScreen(hGameWind, &tempGamePoint);//��ȡ��Ϸ����ֵд��tempgamepoint�ĵ�ַ  �ǰѴ�������ת��Ϊ��Ļ����
		GetClientRect(hGameWind, &tempGameRect);//��ȡ���ڴ�С
		if (tempGamePoint.x != gamePoint.x || tempGamePoint.y != gamePoint.y || tempGameRect.right != gameRect.right || tempGameRect.bottom != gameRect.bottom)
		{
			MoveWindow(myHWND, tempGamePoint.x, tempGamePoint.y, tempGameRect.right, tempGameRect.bottom, true);
			printf(StrA("Checked the game window moved. ReDraw\n"));
		}
		gamePoint = tempGamePoint;
		gameRect = tempGameRect;
		windowW = gameRect.right;//��ȡ��Ϸ���
		windowH = gameRect.bottom;//��ȡ��Ϸ�߶�
		CentWindow.x = windowW / 2;//��ȡ��Ϸ���ĵĵ�
		CentWindow.y = windowH / 2;

		readMem(gamePID, hGameModule + OFFSET_ViewRender, 8, &TempPoint);
		readMem(gamePID, TempPoint + OFFSET_ViewMatrix, 8, &ArrayPoint);//��ȡ������Ϣ
		readMem(gamePID, hGameModule + LocalPlayer, 8, &MySelfPoint);//��ȡ�ҵ���Ϣ�ĵ�ַ
		readMem(gamePID, MySelfPoint + m_iTeamNum, 4, &MyTeam);//��ȡ�ҵĶ���ĺ���
		_VIEWANGLE = MySelfPoint + OFFSET_VIEWANGLE;
		Sleep(1000);
	}
	return 0;
}
/*��Ʒ����*/
void itemGlow(__int64 cuPoint) {
	//1379961481);//1363184265);//1396738697);//true);//1);
	Write<uintptr_t>((uint32_t)gamePID, cuPoint + 0x02c0, 1379961481);//m_highlightFunctionBits
}
/*ʵ�ﴦ���߳�*/
DWORD WINAPI EntityManager(LPVOID lpParam) {
	initEntityNames();//��ʼ��ʵ���id��Ӧ���ƣ������ڴ�
	initBones();
	EntityListPoint = hGameModule + cl_entitylist;//��Ϸ��ַ������б�ĵ�ַƫ�ƾ�������б����ڵĵ�ַ
	bool a = false;
	char* apexName = (char*)calloc(32, 32);
	__int64 cuPoint;
	int len = 65536 << 5;
	char* EntityListMem = (char*)malloc(len + 1);//ʵ���б�Ļ���
	char* EntityMemCached = (char*)malloc(0x10340);//����ʵ����Ϣ�Ļ���
	char* prop_scriptBuff = (char*)malloc(512);
	char* weaponData = (char*)malloc(m_flProjectileSpeed + 16);
	char* weaponModelStr = (char*)malloc(50);
	char* entityInfoNameStr = (char*)malloc(80);

	char* mySelfData = (char*)malloc(0x44b8 + 160);
	while (!a) {
		Sleep(300);//300msˢ��ʱ��
		readMem(gamePID, MySelfPoint, 0x44b8 + 160, mySelfData);
		vector<ApexEntity> tempEntityList;
		readMem(gamePID, EntityListPoint, len, EntityListMem);//��ȡʵ���б�
		Vec3D myLocal = {};
		readVec3D(MySelfPoint + m_vecAbsOrigin, &myLocal);
		//printf("myLocal.x:%f\n", myLocal.x);
		for (int i = 0; i < 65535; ++i) {
			cuPoint = *(__int64*)&EntityListMem[i << 5];//��ʵ���б��ж�ȡÿ��ʵ��ĵ�ַ,Ҳ����i������λ����ʵ���ַ
			if (cuPoint < 1000000) continue;
			readMem(gamePID, cuPoint, 65537, EntityMemCached);//��ȡ����ʵ�����Ϣ
			__int64 apexNamePoint = 0;
			apexNamePoint = *(__int64*)&EntityMemCached[m_iSignifierName];//������ prop_survival�ĵ�ַ
			if (apexNamePoint < 1000000) continue;
			readMem(gamePID, apexNamePoint, 32, apexName);//����������ַ�ж�ȡ������
			if (appConfigs.KaiFaZheXuanXiang) {//ON����
				int flag = *(int*)&EntityMemCached[m_customScriptInt];
				Vec3D local = *(Vec3D*)&EntityMemCached[m_vecAbsOrigin];
				//printf("local.x:%f\n",local.x);
				float xx = local.x - myLocal.x;
				float yy = local.y - myLocal.y;
				float zz = local.z - myLocal.z;
				float distance = sqrt(xx * xx + yy * yy + zz * zz);
				distance *= 0.01905f;
				char* ModelName = (char*)malloc(80);
				GetEntityTypeStr(cuPoint, ModelName);
				char* apexName1 = (char*)calloc(32, 32);
				readMem(gamePID, apexNamePoint, 32, apexName1);
				ApexEntity entity = { cuPoint, 0, flag, ModelName, apexName1, ImColor(0, 255, 255), i, distance, {},"" };
				tempEntityList.emplace_back(entity);
				//printf("apexName: %s  ModelName: %s distance:%d\n",apexName, ModelName, distance);
				continue;
			}
			else if (!memcmp(apexName, "prop_survival", 14)) { //��Ʒ͸�� type = 0
				Vec3D local = *(Vec3D*)&EntityMemCached[m_vecAbsOrigin];
				float xx = local.x - myLocal.x;
				float yy = local.y - myLocal.y;
				float zz = local.z - myLocal.z;
				float distance = sqrt(xx * xx + yy * yy + zz * zz);
				distance *= 0.01905f;
				if (distance > 350)continue;
				itemGlow(cuPoint);
				int flag = *(int*)&EntityMemCached[m_customScriptInt];
				//printf("flag: %d\n", flag);
				ItemInfo item = entityNames[flag];
				if (item.color == 0) continue;
				if (!appConfigs.WuPingTouShi) continue;
				if (distance > appConfigs.WuPingFanWei && (item.color != colors.ShiShi || distance > 350)) continue;
				if (item.color == colors.ShiShi) {
					item.color = ImColor(rand() % (255 - 1 + 1) + 1, rand() % (255 - 1 + 1) + 1, rand() % (255 - 1 + 1) + 1);
				}
				ApexEntity entity = { cuPoint, 0, flag, (char*)item.name, apexName, item.color, i, distance, item };//(char*)item.name+
				tempEntityList.emplace_back(entity);
				continue;
			}
			else if (!memcmp(apexName, "player", 7) || !memcmp(apexName, "npc_dummie", 10)) {//����͸�� type = 1
				Vec3D local = *(Vec3D*)&EntityMemCached[m_vecAbsOrigin];
				int team = *(int*)&EntityMemCached[m_iTeamNum];
				if (team == MyTeam) continue;
				if (cuPoint == MySelfPoint) continue;
				float xx = local.x - myLocal.x;
				float yy = local.y - myLocal.y;
				float zz = local.z - myLocal.z;
				float distance = sqrt(xx * xx + yy * yy + zz * zz);
				distance *= 0.01905f;
				if (distance > appConfigs.TouShiFanWei) continue;//͸�ӷ�Χ
				int weaponEntityid = 0;
				__int64 weaponEntityPoint = 0;
				weaponEntityid = *(int*)&EntityMemCached[m_latestPrimaryWeapons];// +m_allWeapons];
				weaponEntityid &= 0xFFFF;
				//printf("weaponEntityid: %d\n", weaponEntityid);
				if (weaponEntityid > 0 && weaponEntityid < 65535) {
					readMem(gamePID, EntityListPoint + (weaponEntityid << 5), 8, &weaponEntityPoint);
				}
				readMem(gamePID, weaponEntityPoint, m_flProjectileSpeed + 32, weaponData);//��ȡ������Ϣ������
				GetEntityTypeStr(weaponEntityPoint, entityInfoNameStr);
				if (!memcmp("mdl/weapons/", entityInfoNameStr, 12)) {//��ģ����������ȡ��������
					char* newModelName = &entityInfoNameStr[12];
					for (size_t i = 0; i < 50; i++) {
						if (newModelName[i] == '/') {
							weaponModelStr[i] = '\0';
							break;
						}
						weaponModelStr[i] = newModelName[i];
					}
				}
				int myarmo = *(int*)&weaponData[m_ammoInClip];
				//printf("myarmo:%d\n", myarmo);
				float vis_t = *(float*)&EntityMemCached[lastVisibleTime]; //Read<float>((uintptr_t)gamePID, cuPoint + lastVisibleTime); //*(float*)&EntityMemCached[lastVisibleTime];
				//printf("visablet: %f\n", vis_t);
				ApexEntity entity = { cuPoint, 1, 0, NULL, apexName, 255, i, distance, {}, GetWeaponName(weaponModelStr), getNamePoint(i) ,myarmo, vis_t };//
				tempEntityList.emplace_back(entity);
				continue;
			}
			else if (!memcmp(apexName, "npc_frag", 8) || !memcmp(apexName, "npc_marvin", 10)) {//type = 2  npc����Non-Player Character,Ҳ���Ƿ���ҿ��ƽ�ɫ
				if (!appConfigs.XianShiZhaZhu) continue;
				int flag = *(int*)&EntityMemCached[m_customScriptInt];
				Vec3D local = *(Vec3D*)&EntityMemCached[m_vecAbsOrigin];
				float xx = local.x - myLocal.x;
				float yy = local.y - myLocal.y;
				float zz = local.z - myLocal.z;
				float distance = sqrt(xx * xx + yy * yy + zz * zz);
				distance *= 0.01905f;
				if (distance > 350)continue;
				ApexEntity entity = { cuPoint, 2, flag, NULL, apexName, ImColor(0, 255, 200), 0, distance, {} };
				tempEntityList.emplace_back(entity);
				continue;
			}
			else if (!memcmp(apexName, "prop_script", 12)) {//type = 3 grenade_frag
				int flag = *(int*)&EntityMemCached[m_customScriptInt];
				Vec3D local = *(Vec3D*)&EntityMemCached[m_vecAbsOrigin];
				float xx = local.x - myLocal.x;
				float yy = local.y - myLocal.y;
				float zz = local.z - myLocal.z;
				float distance = sqrt(xx * xx + yy * yy + zz * zz);
				distance *= 0.01905f;
				if (distance > 200)continue;
				GetEntityTypeStr(cuPoint, prop_scriptBuff);
				//printf("prop_scriptBuff: %s\n", prop_scriptBuff);
				if (memcmp(prop_scriptBuff, "mdl/props/caustic_gas_tank", 26)) continue; //������
				ApexEntity entity = { cuPoint, 3, flag, NULL, apexName, ImColor(255, 0, 0), 0, distance, {} };
				tempEntityList.emplace_back(entity);
				continue;
			}
			else if (!memcmp(apexName, "player_vehicle", 14)) {//type = 3  �ؾ�
				int flag = *(int*)&EntityMemCached[m_customScriptInt];
				Vec3D local = *(Vec3D*)&EntityMemCached[m_vecAbsOrigin];
				float xx = local.x - myLocal.x;
				float yy = local.y - myLocal.y;
				float zz = local.z - myLocal.z;
				float distance = sqrt(xx * xx + yy * yy + zz * zz);
				distance *= 0.01905f;
				if (distance > 350)continue;
				ApexEntity entity = { cuPoint, 4, flag, NULL, apexName, ImColor(0, 255, 255), 0, distance, {} };
				tempEntityList.emplace_back(entity);
				continue;
			}
			else if (!memcmp(apexName, "prop_dynamic", 12)) {
				int flag = *(int*)&EntityMemCached[m_customScriptInt];
				Vec3D local = *(Vec3D*)&EntityMemCached[m_vecAbsOrigin];
				float xx = local.x - myLocal.x;
				float yy = local.y - myLocal.y;
				float zz = local.z - myLocal.z;
				float distance = sqrt(xx * xx + yy * yy + zz * zz);
				distance *= 0.01905f;
				if (distance > 350)continue;
				GetEntityTypeStr(cuPoint, prop_scriptBuff);
				if (memcmp(prop_scriptBuff, "mdl/vehicle/drop_loot", 21))continue;
				ApexEntity entity = { cuPoint, 5, flag, NULL, apexName, ImColor(0, 255, 255), 0, distance, {} };
				tempEntityList.emplace_back(entity);
				continue;
			}
		}
		apexEntityList.clear();
		apexEntityList = tempEntityList;//ͬ��ʵ����Ϣ
	}
	return 0;
}

DWORD WINAPI SuperAim(LPVOID lpParam) {
	aim = false;
	aimEntity = 0;
	char* weaponData = (char*)malloc(m_flProjectileSpeed + 16);
	char* aimPlayerData = (char*)malloc(m_vecVelocity + 160);//����һ��ָ����С�Ļ�����
	char* mySelfData = (char*)malloc(0x24b8 + 160);
	Vec3D lastLocal = { 0.f, 0.f, 0.f };
	DVec3D VectorVec3D = { 0, 0, 0 };
	INT64 lastTime = 0;
	int getNum = 4;
	Vec3D lastbreath = { 0.f, 0.f, 0.f };

	while (true) {
		int weaponEntityid = 0;
		__int64 weaponEntityPoint = 0;
		float bulletSpeed = 0;
		float bullet_gv = 0;
		INT64 nowTime = 0;
		if (!aim) {
			aimEntity = 0;
			aimThreadStop = true;
			VectorVec3D = { 0.f, 0.f, 0.f };
			lastLocal = { 0, 0, 0 };
			lastTime = 0;
			SuspendThread(hAimThread);
		}
		readMem(gamePID, MySelfPoint, 0x24b8 + 160, mySelfData);//��ȡ���ҵ���Ϣ������ ֮ǰ��ȡ�Ĵ�С̫С�ˣ���������
		readMem(gamePID, aimEntity, m_vecVelocity + 160, aimPlayerData);//��ȡ������׼��ҵ���Ϣ������
		weaponEntityid = *(int*)&mySelfData[m_latestPrimaryWeapons];// +m_allWeapons];m_fIsSprinting
		weaponEntityid &= 0xFFFF;
		//printf("weaponEntityid: %d\n", weaponEntityid);
		if (weaponEntityid > 0 && weaponEntityid < 65535) {
			readMem(gamePID, EntityListPoint + (weaponEntityid << 5), 8, &weaponEntityPoint);
		}
		readMem(gamePID, weaponEntityPoint, m_flProjectileSpeed + 32, weaponData);//��ȡ������Ϣ������
		bulletSpeed = *(float*)&weaponData[m_flProjectileSpeed];
		//printf("bulletSpeed:%0.9f\n", bulletSpeed);
		if (bulletSpeed < 10) {
			bulletSpeed = 15000;
		}
		//bullet_gv = *(float*)&weaponData[m_flProjectileScale];
		//printf("bullet_gv:%f\n", bullet_gv);
		Vec3D entityLocal = *(Vec3D*)&aimPlayerData[m_vecAbsOrigin];//����ԭ��λ��
		float matrix[128][3][4];
		GetBoneArray(aimEntity, &matrix);//��ȡ����׼�������
		Vec3D headLocal = CalcBonePos(matrix, Bones::head, entityLocal);//����ͷ��λ��
		Vec3D neckLocal = CalcBonePos(matrix, Bones::neck, entityLocal);//���ﾱ��λ��
		Vec3D chestLocal = CalcBonePos(matrix, Bones::chest, entityLocal);//���ﾱ��λ��
		//Vec3D calcLocal = { (headLocal.x - neckLocal.x) * 0.39f, (headLocal.y - neckLocal.y) * 0.39f, (headLocal.z - neckLocal.z) * 0.39f };
		//Vec3D aimLocal = { headLocal.x - calcLocal.x, headLocal.y - calcLocal.y, headLocal.z - calcLocal.z };
		Vec3D aimLocal = headLocal;//���յõ��ĵ���λ��
		if (lastLocal.x == 0.f) {
			lastLocal = aimLocal;
		}
		Vec3D myLocal = {};
		if (aimLocal.x == 0 || aimLocal.y == 0 || aimLocal.z == 0) continue;
		readVec3D(_VIEWANGLE - 28, &myLocal);//��ȡ�ҵ�λ��  viewangle-28�����ҵ��������λ��
		//readVec3D(_Camerpos, &myLocal);//һ���ģ�ֻҪ֪��viewangle��ƫ�ƾ��У���Ȼ�õ���ƫ��ֵ��һ�������õ���λ��ֵһ��
		float xx = aimLocal.x - myLocal.x;
		float yy = aimLocal.y - myLocal.y;
		float zz = aimLocal.z - myLocal.z;
		float distance = sqrt(xx * xx + yy * yy + zz * zz);//�������Ҿ���
		if (distance * 0.01905f > 10) aimLocal = neckLocal;
		if (distance * 0.01905f > 100) aimLocal = chestLocal;

		float flyTime = distance / bulletSpeed;//����ʱ��΢��
		//printf("flyTime:%0.9f\n", flyTime);
		QueryPerformanceCounter((LARGE_INTEGER*)&nowTime);//��ȡ����ʱ��  ΢��
		float flT1 = flyTime * 1000;//����ʱ����
		INT64 timeSub = nowTime - lastTime;//ʱ���
		//printf("timeSub/10000:  %llf\n", timeSub / 10000);
		//printf("flT1/getNum:  %llf\n", flT1 / getNum);
		if (timeSub / 10000 >= (flT1 / getNum)) {//�������Ա�
			lastTime = nowTime;
			//����Ԥ�е�λ��
			VectorVec3D = { (entityLocal.x - lastLocal.x) * getNum, (entityLocal.y - lastLocal.y) * getNum, (entityLocal.z - lastLocal.z) * getNum };
			lastLocal = entityLocal;
		}
		float viewDistance = distance * 0.01905f;//��������ʵ�ʾ��룬��Ϊ��λ
		if (viewDistance > 5) {//ONʼԤ�о���
			aimLocal.x += VectorVec3D.x;
			aimLocal.y += VectorVec3D.y;
			if (VectorVec3D.z < 100.f) {
				aimLocal.z += (VectorVec3D.z * 0.67f);
			}
			else {
				aimLocal.z += (VectorVec3D.z * 0.88f);
			}
		}
		xx = aimLocal.x - myLocal.x;//xyz��һ������
		yy = aimLocal.y - myLocal.y;
		zz = aimLocal.z - myLocal.z;
		float lf = atan2f(yy, xx) * rotation;
		float tb = 0 - ((atan2f(zz, sqrt(xx * xx + yy * yy))) * rotation);
		float BaseFaill = 630.f * bullet_gv * (flyTime * flyTime);
		zz += BaseFaill * ((89 - abs(tb)) / 89);
		tb = 0 - ((atan2f(zz, sqrt(xx * xx + yy * yy))) * rotation);
		if (!(lf >= 0 || lf <= 0) || !(tb >= 0 || tb <= 0)) continue;
		Vec3D angle = { tb, lf, 0.f };//����Ҫд��ķ���Ƕ�
		Vec3D punch = *(Vec3D*)&mySelfData[m_vecPunchWeapon_Angle];
		//Vec3D dynamic = *(Vec3D*)&mySelfData[OFFSET_BREATH_ANGLES];//m_vecPunchBase_Angleǹ��ʵ����׼�Ƕ�
		//printf("dynamic.x:%f\n", dynamic.x);
		angle.x -= punch.x;
		angle.y -= punch.y;
		Vec3D cuAng = {};
		readVec3D(_VIEWANGLE, &cuAng);//viewangle���ҵ�ǰ�ĽǶ�
		//angle.z = cuAng.z;
		writeVec3D(_VIEWANGLE, &angle);//���Ƕ�д���ڴ�
		usleep(10);//΢�룿
	}
	return 0;
}
/*�Զ��һ�*/
DWORD WINAPI ZiDongGuaJi(LPVOID lpParam) {
	while (1) {
		if (!appConfigs.ZIDONGGUAJI)continue;
		Sleep(500);
	}
	return 0;
}
/*�ӵ���ɢ����*/
DWORD WINAPI HentaiThread(LPVOID lpParam) {
	while (true)
	{
		int weaponEntityid = 0;
		__int64 weaponEntityPoint = 0;
		readMem(gamePID, MySelfPoint + m_latestPrimaryWeapons, 4, &weaponEntityid);
		weaponEntityid &= 0xFFFF;
		if (weaponEntityid > 0 && weaponEntityid < 65535) {
			readMem(gamePID, EntityListPoint + (weaponEntityid << 5), 8, &weaponEntityPoint);
		}
		float w = 0.f;
		writeMem(gamePID, weaponEntityPoint + m_flWeaponSpread1, 4, &w);
		writeMem(gamePID, weaponEntityPoint + m_flWeaponSpread2, 4, &w);
		//float bulletSpeed = 0;
		//readMem(gameHandle, weaponEntityPoint + m_flBulletSpeed, sizeof(float), &bulletSpeed);
		//printf("SPEED: %f\n", bulletSpeed);
		usleep(10);
	}
	return 0;
}
DWORD WINAPI MouseThread(LPVOID lpParam) {
	while (true) {
		Sleep(100);
	}
	return 0;
}
DWORD WINAPI StartDraw(LPVOID lpParam) {
	try {
		gamePoint.x = 0;
		gamePoint.y = 0;
		gameRect.left = 0;
		gameRect.top = 0;
		gameRect.right = 0;
		gameRect.bottom = 0;
		ClientToScreen(hGameWind, &gamePoint);
		GetClientRect(hGameWind, &gameRect);
		startDraw();
		return 0;
	}
	catch (wchar_t* str) {
		MessageBoxW(NULL, str, L"Error", NULL);
	}
}
/*��ȡ��Ϸ�ļ�*/
bool check_version() {
	// Sanity check the image base address...
	if (hGameModule == 0 || (hGameModule & 0xfff) != 0) {
		printf(StrA("apex(%u) Invalid image base: perhaps your bypass is incomplete.\n"), gamePID);
		return false;
	}
	IMAGE_DOS_HEADER dos_header;
	IMAGE_NT_HEADERS64 nt_headers;
	if (!(read(gamePID, hGameModule, dos_header) && read(gamePID, hGameModule + dos_header.e_lfanew, nt_headers))) {
		printf(StrA("apex(%u) Error reading headers: incorrect image base dos_header , broken bypass or other issue!\n"), gamePID);
		return false;
	}
	printf(StrA("\ndos_header:%p\nnt_headers:%p\n"), dos_header, nt_headers);
	//Sanity check the image magic values...
	if (dos_header.e_magic != IMAGE_DOS_SIGNATURE || nt_headers.Signature != IMAGE_NT_SIGNATURE ||
		nt_headers.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
		printf(StrA("apex(%u) Incorrect magic values: the image base address is incorrect!\n"), gamePID);
		return false;
	}
	// If TimeDateStamp and CheckSum match then our offsets are probably up-to-date
	// This can also happen if the base address points to the wrong image in memory
	if (nt_headers.FileHeader.TimeDateStamp == TimeDateStamp1 && nt_headers.OptionalHeader.CheckSum == CheckSum1) {
		printf(StrA("nt_headers.FileHeader.TimeDateStamp:%x\n"), nt_headers.FileHeader.TimeDateStamp);
		printf("nt_headers.OptionalHeader.CheckSum:%x\n", nt_headers.OptionalHeader.CheckSum);
		printf(StrA("apex(%u) Gamedata TimeDateStamp and CheckSum matched!.\n"), gamePID);
		return true;
	}
	printf(StrA("apex(%u) Gamedata mismatch! Please update the offsets.\n"), gamePID);
	// Wait a minute to give the game a chance to decrypt itself
	printf(StrA("apex(%u) Proceeding to dump the game executable in ~1 second.\n"), gamePID);
	Sleep(1000);
	// Dump the game binary from memory
	const size_t target_len = nt_headers.OptionalHeader.SizeOfImage;
	auto target = std::unique_ptr<uint8_t[]>(new uint8_t[target_len]);
	if (read_array(gamePID, hGameModule, target.get(), target_len)) {
		// Fixup section headers...
		auto pnt_headers = reinterpret_cast<PIMAGE_NT_HEADERS64>(target.get() + dos_header.e_lfanew);
		auto section_headers = reinterpret_cast<PIMAGE_SECTION_HEADER>(
			target.get() +
			static_cast<size_t>(dos_header.e_lfanew) +
			static_cast<size_t>(FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader)) +
			static_cast<size_t>(nt_headers.FileHeader.SizeOfOptionalHeader));
		for (size_t i = 0; i < nt_headers.FileHeader.NumberOfSections; i += 1) {
			auto& section = section_headers[i];
			// Rewrite the file offsets to the virtual addresses
			section.PointerToRawData = section.VirtualAddress;
			section.SizeOfRawData = section.Misc.VirtualSize;
			// Rewrite the base relocations to the ".reloc" section
			if (!memcmp(section.Name, ".reloc\0\0", 8)) {
				pnt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC] = {
					section.VirtualAddress,
					section.Misc.VirtualSize,
				};
			}
		}
		const auto dump_file = CreateFileW(StrW(L"r5apex.bin"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_COMPRESSED, NULL);
		if (dump_file != INVALID_HANDLE_VALUE) {
			if (!WriteFile(dump_file, target.get(), static_cast<DWORD>(target_len), NULL, NULL)) {
				printf(StrA("apex(%u) Error writing r5apex.bin: %u\n"), gamePID, GetLastError());
			}
			CloseHandle(dump_file);
		}
		else {
			printf(StrA("apex(%u) Error writing r5apex.bin: %u\n"), gamePID, GetLastError());
		}
		printf(StrA("apex(%u) Wrote r5apex.bin!\n\n"), gamePID);
	}
	else {
		printf(StrA("apex(%u) Error reading the image from memory!\n"), gamePID);
	}

	return false;
}