#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS 1
#pragma once
#include <dinput.h>
#include <tchar.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <comdef.h>
#include <vector>
#include <hash_map>
#include "imgui/imgui.h"

#include "VMProtectSDK.h"
#define StrA VMProtectDecryptStringA
#define StrW VMProtectDecryptStringW
bool check_version();
void set_random_title();

//��Ϸ״̬
#define TimeDateStamp1				0x5fc7d976// ʱ���
#define CheckSum1					0x22// У���
#define rotation					57.295779513082f//��ת
#define ClientState					0x1281d00// �ͻ���״̬
#define SignonState					0x1281d98// ��½״̬
#define LevelName					0x1281eb0// ��Ϸ�汾


#define cl_entitylist				0x18c62f8//0x18cdce8//0x18c6928  ����б�
#define LocalPlayer					0x1c75558//0x1198fc8//0x1c7cf28//0x18c6928  ������� ��LocalPlayer!!!!!!
#define OFFSET_ViewRender			0x4076fc0//0x407dff8//0x407def8//0x4072958  ��Ⱦ��ͼ
#define OFFSET_ViewMatrix			0x1b3bd0//0x1b3bd0   ������ͼ
//��Ϸ��������Ҫ�޸������ƫ��,����Ƚ��ٸ���
//////////////////////////////////////////////////////////////////////////
#define m_iSignifierName			0x0580//0x05e0//0x0560 ������ player prop_survival
#define m_customScriptInt			0x1618//0x1688//0x1608 Item�������
#define OFFSET_BONES				0x0f38//0x0f98//0xf18  m_bConstrainBetweenEndpoints ����
#define m_ModelName					0x0030//0x0a40  m_ModelName ģ������
//�����Ϣ
#define m_vecAbsOrigin				0x14C//0x144  absԭ��
#define armorType					0x4274//0x432c//װ������
#define helmetType					0x4270//0x4328//ͷ������
#define OFFSET_VIEWANGLE			0x2470//0x2528//0x24A0  m_ammoPoolCapacity - 0x14   ��ͼ�Ƕ�
//#define OFFSET_CAMERAPOS			0x1EF0//0x1E6C  �����	40 57 48 83 EC 20 F3 0F 10 0A 48 8B F9 0F 2E 89
#define OFFSET_BREATH_ANGLES		OFFSET_VIEWANGLE - 0x10//0x24A0 - 0x10  �����Ƕ�
#define m_iHealth					0x0440//0x04a0//0x420  Ѫ��
#define m_iMaxHealth				0x0578//0x05d8//0x0558  ���Ѫ��
#define m_shieldHealth				0x0170// ����ֵ
#define m_shieldHealthMax			0x0174//0x16c ��󻤶�ֵ
#define m_iTeamNum					0x0450//0x430 �Ŷӱ�ʶ		97��ʱ���ǵ�����
#define m_lifeState					0x0798//0x07f8//0x0778 ���״̬
#define m_bleedoutState				0x25f0//0x26b0//0x2620 ��Ѫ״̬
#define m_bleedoutStartTime			0x25f4//0x262c// ��Ѫʱ��
#define m_vecVelocity				0x0484//0x04e4//0x0464 �ٶ�
#define m_latestPrimaryWeapons		0x19ec//0x1a8c//0x1a0c ��ǰ����
#define m_vecPunchWeapon_Angle		0x2390//0x2450//0x23C8  ������
#define m_fIsSprinting				0x28ac//0x28cc // ���
#define lastVisibleTime				0x1A4C//0x1A6C// ����ʱ���
#define m_nSkin						0x0e88
//������Ϣ
#define m_flProjectileSpeed			0x1ea8//0x1e1c  �ӵ��ٶ�
#define m_flProjectileScale			0x1eb0//�ӵ�����  m_flProjectileSpeed + 0x8
#define m_ammoInClip				0x16b4//0x1684Int �����ӵ���
#define m_bInReload					0x16ca//0x164a// ������ bool
#define m_discarded					0x16c9//�������� bool
#define m_weaponIsCharging			0x17f5//���
#define m_skinOverride				0x1750
#define m_skinOverrideIsValid		0x1754
#define m_flWeaponSpread1			0x1780//0x1370
#define m_flWeaponSpread2			0x178c//0x1380
#define m_allWeapons				0x2C//????



//==���õ�
#define GlobalVars					0x1281a00// ȫ�ֱ���
//#define LocalEntityHandle			0x11b2278//0x1191ebc// LocalEntityHandle û�õ�
#define m_iName						0x0589// ���Ʊ�ʶ		125780153691248��ʱ�������
#define m_vecAbsVelocity			0x140
#define m_renderMouse				0x10
#define m_latestNonOffhandWeapons	0x1a1c//0x170c
#define m_selectedOffhands			0x1a1e//0x174E
#define m_duckState					0x2970//0x26A0
#define m_entityBasePunch			0x23C8//0x213c
#define m_armorType					0x42d0//0x4248
#define m_helmetType				0x42cc//0x4244
#define NameList					0x816bcb0//0x8167c30//0x8167BB0
#define m_nameArrayListSub			0x38//0xA00

typedef struct __ColorStruct {
	unsigned int ShiShi;
	unsigned int ShengQi;
	unsigned int XiYou;//ϡ��
	unsigned int GaoJi;//�߼�
	unsigned int PuTong;//��ͨ
	unsigned int NengLiang;//����
	unsigned int ZhongXing;//����
	unsigned int SanDan;//ɢ��
	unsigned int QingXing;//����
} ColorStruct;

typedef struct __AppConfigs {//�˵��ṹ��
	bool MenuStatus;//  1
	bool FangKuang;//  2
	float TouShiFanWei;//  3
	bool GuGeTouShi;//����͸��  4
	bool WuPingTouShi;//��Ʒ͸��  5
	float WuPingFanWei;//��Ʒ��Χ  6
	bool QiangXieTouShi;//ǹе͸��  7
	bool ZiDanTouShi;//�ӵ�͸��  8
	bool FangJuTouShi;//����͸��  9
	bool PeiJianTouShi;//���͸��  10
	bool ZhongYaoWuZi;//��Ҫ����  11
	bool ZiDongMiaoZhun;//�Զ���׼ 12
	float ZiMiaoFanWei;//���鷶Χ  13
	int MiaoZhunReJian;//��׼�ȼ�  14
	bool XianShiZhaZhu;//��ʾը��  15
	bool KaiFaZheXuanXiang;//ON����ѡ��  16

	bool GUGETOUSHI;
	bool ZIDONGGUAJI;
} AppConfigs;
/*��Ʒ��Ϣname,color*/
typedef struct ItemInfo {
	const char* name;
	unsigned int color;
} ApexItem;
/**
 * ʵ����Ϣ�ṹ��{point,type,flag,name,apexName,color,zc,distance,item,WeaponName,pName,armonum,visabletime}
 */
typedef struct EntityStruct {
	__int64 point;//1 ��ַ
	int type; // 0-item 1-player  ����
	int flag;//3
	char* ModelName;//4
	const char* apexName;//5  prop_survival player��Щ
	unsigned int color;//6
	int zc;//7  list�е����
	int distance;//8
	ApexItem item;//9  ��Ʒ��ɫ
	const char* WeaponName;//10 ��������
	__int64 pName;//11  r�����ǳƵĵ�ַ
	//int bleedout;
	int armonum;//12
	float visabletime;//13
} ApexEntity;

typedef struct __Vec3D {
	float x;
	float y;
	float z;
} Vec3D;

typedef struct __DVec3D {
	double x;
	double y;
	double z;
} DVec3D;

typedef struct __Point_INDEX {
	__int64 point;
	int index;
} Point_Index;

typedef struct __AimEntityDrawData {
	float BoxX;
	float BoxY;
	float BoxY1;
	ImColor color;
	char* text;
} AimEntityDrawData;

extern HWND myHWND;
extern int gamePID;
extern HWND hGameWind;
extern __int64 hGameModule;
extern int fontSize;
extern ColorStruct colors;
extern AppConfigs appConfigs;
extern int menu_zuobian;
extern int menu_dingbian;
extern int menu_kuan;
extern int menu_gao;
extern int menu_hangju;
extern int menu_select;
extern int menu_size;
extern int menu_fenge;
extern int nengnenglvse;
extern int nengnenglanse;
extern POINT gamePoint;
extern RECT gameRect;
extern int windowW;
extern int windowH;
extern POINT CentWindow;

//point
extern __int64 TempPoint;
extern __int64 ArrayPoint;
extern __int64 MySelfPoint;
extern __int64 EntityListPoint;
extern __int64 _VIEWANGLE;

//data
extern int MyTeam;
extern float worldArray[4][4];
extern std::vector<ApexEntity> apexEntityList;
extern std::hash_map<int, ApexItem> entityNames;
extern bool aim;
extern bool aimThreadStop;
extern HANDLE hAimThread;
extern __int64 aimEntity;

extern int ѰѪ[3][16];
extern int ֱ��[3][16];
extern int ����[3][16];
extern int ̽·[3][16];
extern int ����[3][16];

extern int ����[3][16];
extern int ���[3][16];
extern int ��ʴ[3][16];
extern int ����[3][16];
extern int ��ɭ[3][16];

DWORD GetProcessIDByName(const char* pName);
void rand_str(char* str, const int len);
void initColor();
void initConfig();
void initMenu();
void startThreads();
void initEntityNames();
void initBones();
void usleep(unsigned int usec);
int getRandomInt(int min, int max);
void Print_Memory(const unsigned char* start, unsigned int length);

enum Bones : int {
	head = 12,
	neck = 11,
	chest = 7,//��
	abdomen = 6,//����
	�� = 5,

	������� = 40, // +1
	�����Ҽ� = 15, // +1
	���������� = 38, // ��� - 2
	���������� = 13, // �Ҽ� - 2
	������� = 41, // ��� + 1
	�������1 = 43, // ��OFF�� + 2
	�������2 = 42, // 1OFF�� - 1
	�������3 = 44, // 1OFF�� + 1
	�������4 = 45, // 3OFF�� + 1
	�������5 = 47, // 4OFF�� + 2 �м��и�OFF��
	�����ұ� = 16, // �Ҽ� + 1
	�����ұ�1 = 18, // ��OFF�� + 2
	�����ұ�2 = 17, // 1OFF�� -1
	�����ұ�3 = 19, // 1OFF�� + 1
	�����ұ�4 = 20, // 3OFF�� + 1
	�����ұ�5 = 22, // 4OFF�� + 2 �м��и�OFF��
	������ϥ = 70, // N
	������� = 71, // ��ϥ + 1
	�������1 = 72, // ��� + 1
	������ϥ = 65, // N
	�����ҽ� = 66, // ��ϥ + 1
	�����ҽ�1 = 67, // �ҽ� + 1

	ֱ����� = 43,
	ֱ���Ҽ� = 16,
	ֱ����ϥ = 79,
	ֱ����ϥ = 74,

	ѰѪ��� = 49,
	ѰѪ�Ҽ� = 16,
	ѰѪ��ϥ = 94,
	ѰѪ��ϥ = 90,

	������� = 40,
	�����Ҽ� = 15,
	������ϥ = 97,
	������ϥ = 92,

	̽·��� = 28,
	̽·�Ҽ� = 14,
	̽·��ϥ = 77,
	̽·��ϥ = 70,

	������ = 44,
	����Ҽ� = 15,
	�����ϥ = 87,
	�����ϥ = 82,

	��ʴ��� = 38,
	��ʴ�Ҽ� = 14,
	��ʴ��ϥ = 81,
	��ʴ��ϥ = 76,

	������� = 41,
	�����Ҽ� = 14,
	������ϥ = 91,
	������ϥ = 86,

	������� = 44, // 44
	�����Ҽ� = 15,
	������ϥ = 92, // 92
	������ϥ = 82, // 82

	��ɭ��� = 69,
	��ɭ�Ҽ� = 44,
	��ɭ��ϥ = 116, // 92
	��ɭ��ϥ = 110, // 82
};