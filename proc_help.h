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

//游戏状态
#define TimeDateStamp1				0x5fc7d976// 时间戳
#define CheckSum1					0x22// 校验和
#define rotation					57.295779513082f//回转
#define ClientState					0x1281d00// 客户端状态
#define SignonState					0x1281d98// 登陆状态
#define LevelName					0x1281eb0// 游戏版本


#define cl_entitylist				0x18c62f8//0x18cdce8//0x18c6928  玩家列表
#define LocalPlayer					0x1c75558//0x1198fc8//0x1c7cf28//0x18c6928  本地玩家 是LocalPlayer!!!!!!
#define OFFSET_ViewRender			0x4076fc0//0x407dff8//0x407def8//0x4072958  渲染视图
#define OFFSET_ViewMatrix			0x1b3bd0//0x1b3bd0   矩阵视图
//游戏更新了需要修改上面的偏移,下面比较少更新
//////////////////////////////////////////////////////////////////////////
#define m_iSignifierName			0x0580//0x05e0//0x0560 类型名 player prop_survival
#define m_customScriptInt			0x1618//0x1688//0x1608 Item类别索引
#define OFFSET_BONES				0x0f38//0x0f98//0xf18  m_bConstrainBetweenEndpoints 骨骼
#define m_ModelName					0x0030//0x0a40  m_ModelName 模型名称
//玩家信息
#define m_vecAbsOrigin				0x14C//0x144  abs原点
#define armorType					0x4274//0x432c//装甲类型
#define helmetType					0x4270//0x4328//头盔类型
#define OFFSET_VIEWANGLE			0x2470//0x2528//0x24A0  m_ammoPoolCapacity - 0x14   视图角度
//#define OFFSET_CAMERAPOS			0x1EF0//0x1E6C  摄像机	40 57 48 83 EC 20 F3 0F 10 0A 48 8B F9 0F 2E 89
#define OFFSET_BREATH_ANGLES		OFFSET_VIEWANGLE - 0x10//0x24A0 - 0x10  呼吸角度
#define m_iHealth					0x0440//0x04a0//0x420  血量
#define m_iMaxHealth				0x0578//0x05d8//0x0558  最大血量
#define m_shieldHealth				0x0170// 护盾值
#define m_shieldHealthMax			0x0174//0x16c 最大护盾值
#define m_iTeamNum					0x0450//0x430 团队标识		97的时候是电脑人
#define m_lifeState					0x0798//0x07f8//0x0778 存活状态
#define m_bleedoutState				0x25f0//0x26b0//0x2620 流血状态
#define m_bleedoutStartTime			0x25f4//0x262c// 流血时间
#define m_vecVelocity				0x0484//0x04e4//0x0464 速度
#define m_latestPrimaryWeapons		0x19ec//0x1a8c//0x1a0c 当前武器
#define m_vecPunchWeapon_Angle		0x2390//0x2450//0x23C8  后坐力
#define m_fIsSprinting				0x28ac//0x28cc // 冲刺
#define lastVisibleTime				0x1A4C//0x1A6C// 可视时间和
#define m_nSkin						0x0e88
//武器信息
#define m_flProjectileSpeed			0x1ea8//0x1e1c  子弹速度
#define m_flProjectileScale			0x1eb0//子弹重力  m_flProjectileSpeed + 0x8
#define m_ammoInClip				0x16b4//0x1684Int 弹夹子弹数
#define m_bInReload					0x16ca//0x164a// 换弹夹 bool
#define m_discarded					0x16c9//被丢弃的 bool
#define m_weaponIsCharging			0x17f5//充电
#define m_skinOverride				0x1750
#define m_skinOverrideIsValid		0x1754
#define m_flWeaponSpread1			0x1780//0x1370
#define m_flWeaponSpread2			0x178c//0x1380
#define m_allWeapons				0x2C//????



//==无用的
#define GlobalVars					0x1281a00// 全局变量
//#define LocalEntityHandle			0x11b2278//0x1191ebc// LocalEntityHandle 没用的
#define m_iName						0x0589// 名称标识		125780153691248的时候是玩家
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
	unsigned int XiYou;//稀有
	unsigned int GaoJi;//高级
	unsigned int PuTong;//普通
	unsigned int NengLiang;//能量
	unsigned int ZhongXing;//重型
	unsigned int SanDan;//散弹
	unsigned int QingXing;//轻型
} ColorStruct;

typedef struct __AppConfigs {//菜单结构体
	bool MenuStatus;//  1
	bool FangKuang;//  2
	float TouShiFanWei;//  3
	bool GuGeTouShi;//骨骼透视  4
	bool WuPingTouShi;//物品透视  5
	float WuPingFanWei;//物品范围  6
	bool QiangXieTouShi;//枪械透视  7
	bool ZiDanTouShi;//子弹透视  8
	bool FangJuTouShi;//防具透视  9
	bool PeiJianTouShi;//配件透视  10
	bool ZhongYaoWuZi;//重要物资  11
	bool ZiDongMiaoZhun;//自动瞄准 12
	float ZiMiaoFanWei;//自瞄范围  13
	int MiaoZhunReJian;//瞄准热键  14
	bool XianShiZhaZhu;//显示炸珠  15
	bool KaiFaZheXuanXiang;//ON发者选项  16

	bool GUGETOUSHI;
	bool ZIDONGGUAJI;
} AppConfigs;
/*物品信息name,color*/
typedef struct ItemInfo {
	const char* name;
	unsigned int color;
} ApexItem;
/**
 * 实物信息结构体{point,type,flag,name,apexName,color,zc,distance,item,WeaponName,pName,armonum,visabletime}
 */
typedef struct EntityStruct {
	__int64 point;//1 地址
	int type; // 0-item 1-player  类型
	int flag;//3
	char* ModelName;//4
	const char* apexName;//5  prop_survival player这些
	unsigned int color;//6
	int zc;//7  list中的序号
	int distance;//8
	ApexItem item;//9  物品颜色
	const char* WeaponName;//10 武器名称
	__int64 pName;//11  r人物昵称的地址
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

extern int 寻血[3][16];
extern int 直布[3][16];
extern int 命脉[3][16];
extern int 探路[3][16];
extern int 动力[3][16];

extern int 恶灵[3][16];
extern int 班加[3][16];
extern int 侵蚀[3][16];
extern int 幻象[3][16];
extern int 华森[3][16];

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
	chest = 7,//胸
	abdomen = 6,//腹部
	屌 = 5,

	命脉左肩 = 40, // +1
	命脉右肩 = 15, // +1
	命脉左锁骨 = 38, // 左肩 - 2
	命脉右锁骨 = 13, // 右肩 - 2
	命脉左臂 = 41, // 左肩 + 1
	命脉左臂1 = 43, // 主OFF节 + 2
	命脉左臂2 = 42, // 1OFF节 - 1
	命脉左臂3 = 44, // 1OFF节 + 1
	命脉左臂4 = 45, // 3OFF节 + 1
	命脉左臂5 = 47, // 4OFF节 + 2 中间有个OFF节
	命脉右臂 = 16, // 右肩 + 1
	命脉右臂1 = 18, // 主OFF节 + 2
	命脉右臂2 = 17, // 1OFF节 -1
	命脉右臂3 = 19, // 1OFF节 + 1
	命脉右臂4 = 20, // 3OFF节 + 1
	命脉右臂5 = 22, // 4OFF节 + 2 中间有个OFF节
	命脉左膝 = 70, // N
	命脉左脚 = 71, // 左膝 + 1
	命脉左脚1 = 72, // 左脚 + 1
	命脉右膝 = 65, // N
	命脉右脚 = 66, // 右膝 + 1
	命脉右脚1 = 67, // 右脚 + 1

	直布左肩 = 43,
	直布右肩 = 16,
	直布左膝 = 79,
	直布右膝 = 74,

	寻血左肩 = 49,
	寻血右肩 = 16,
	寻血左膝 = 94,
	寻血右膝 = 90,

	恶灵左肩 = 40,
	恶灵右肩 = 15,
	恶灵左膝 = 97,
	恶灵右膝 = 92,

	探路左肩 = 28,
	探路右肩 = 14,
	探路左膝 = 77,
	探路右膝 = 70,

	班加左肩 = 44,
	班加右肩 = 15,
	班加左膝 = 87,
	班加右膝 = 82,

	侵蚀左肩 = 38,
	侵蚀右肩 = 14,
	侵蚀左膝 = 81,
	侵蚀右膝 = 76,

	幻象左肩 = 41,
	幻象右肩 = 14,
	幻象左膝 = 91,
	幻象右膝 = 86,

	动力左肩 = 44, // 44
	动力右肩 = 15,
	动力左膝 = 92, // 92
	动力右膝 = 82, // 82

	华森左肩 = 69,
	华森右肩 = 44,
	华森左膝 = 116, // 92
	华森右膝 = 110, // 82
};