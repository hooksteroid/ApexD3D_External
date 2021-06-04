#include "DrawCallBack.h"
#include "DrawEng.h"
#include "proc_help.h"
#include "apexUtils.h"
#include "imgui/imgui.h"
#include "DriverHelp.h"
#include <cstdlib>
#include <sstream>
#include <string>
#include <iostream>

void drawMenu();
void drawEntity();
using namespace std;
float losDistance = 0;
std::vector<ApexEntity> insidePlayer;//准心内玩家
std::vector<ImVec2> itemLocals;
char* entityData = 0;//实物信息
char* pNameBuff = 0;
bool playerDataInit = false;//判断是否已经初始化数据
char* renderBuff = 0;//sprintf的buffer
__int64 tempAim = 0;//缓存的玩家地址
char* aimBuff = 0;//sprintf的buffer
INT64 lastTime = 0;//时间

/*绘制功能*/
void draw() {
	if (!playerDataInit)
	{
		entityData = (char*)malloc(m_bleedoutState + 800);
		renderBuff = (char*)malloc(512);
		pNameBuff = (char*)malloc(512);
		aimBuff = (char*)malloc(512);
		playerDataInit = true;
		QueryPerformanceCounter((LARGE_INTEGER*)&lastTime);
	}
	drawMenu();
	drawEntity();
}
/*辉光*/
void glow2(uint64_t addr, float r, float g, float b) {
	uint32_t pid = (uint32_t)gamePID;
	Write(pid, addr + 0x03C8, true);
	Write(pid, addr + 0x440, 1);//Highlight_SetCurrentContext
	Write(pid, addr + 0x1D0, r);
	Write(pid, addr + 0x1D0 + 0x4, g);
	Write(pid, addr + 0x1D0 + 0x8, b);
	//for (int offset = 0x2F8; offset <= 0x328; offset += 0x4)
	for (int offset = 0x388; offset <= 0x3AC; offset += 0x4)
		Write(pid, addr + offset, FLT_MAX);
	Write(pid, addr + 0x394, FLT_MAX);
}


/*绘制实物*/
void drawEntity() {
	insidePlayer.clear();
	itemLocals.clear();
	readWorldArray(&worldArray);//将矩阵地址的信息读取到当前的地址
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	//drawList->AddCircle({ (float)CentWindow.x, (float)CentWindow.y }, appConfigs.ZiMiaoFanWei, ImColor({ 0x00, 0xff, 0xff }),50, 1.2f);//画圆圈
	int aimEntityStatus = 0;
	int _50Players = 0;//五十米内倒地敌人
	if (aimEntity > 0) {
		readMem(gamePID, aimEntity + m_bleedoutState, 4, &aimEntityStatus);
	}
	AimEntityDrawData aimDraw = { 0, 0 };
	for (ApexEntity entity : apexEntityList) {
		if (appConfigs.PeiJianTouShi && (entity.flag >= 62 || entity.flag == 38 || entity.flag == 25 || entity.flag == 28)) continue;
		readMem(gamePID, entity.point, m_bleedoutState + 8, entityData);
		Vec3D entityLocal = *(Vec3D*)&entityData[m_vecAbsOrigin];
		float ViewW = worldArray[3][0] * entityLocal.x + worldArray[3][1] * entityLocal.y + worldArray[3][2] * entityLocal.z + worldArray[3][3];
		if (entity.type == 1) {// 扫描50米内敌人 0-item 1-player
			int status = *(int*)&entityData[m_bleedoutState];
			if (status == 0 && (int)entity.distance <= 50) {
				int blood = *(int*)&entityData[m_iHealth];
				int armor = *(int*)&entityData[m_shieldHealthMax - 4];
				if (blood <= 0 || blood > 100 || (blood == 50 && armor == 0)) continue;
				_50Players++;
			}
		}
		if (ViewW < 0.01) {
			continue;
		}
		ViewW = 1 / ViewW;

		if (entity.type == 0) {// 物品透视 0-item 1-player   还有ON发者选项
			if (entity.distance > 50.f)continue;//距离
			float BoxX = gamePoint.x + CentWindow.x + (worldArray[0][0] * entityLocal.x + worldArray[0][1] * entityLocal.y +
				worldArray[0][2] * entityLocal.z + worldArray[0][3]) * ViewW * CentWindow.x;
			float BoxY = gamePoint.y + CentWindow.y - (worldArray[1][0] * entityLocal.x + worldArray[1][1] * entityLocal.y +
				worldArray[1][2] * entityLocal.z + worldArray[1][3]) * ViewW * CentWindow.y;
			if (BoxX > windowW || BoxX < -70 || BoxY > windowH || BoxY < 0) {
				continue;
			}
			const char* fNormal = StrA("%s   %s   %dM   %d\0");//%s| %d|
			sprintf(renderBuff, fNormal, entity.apexName, entity.ModelName, entity.distance, entity.zc);// entity.name, entity.distance,entity.flagentity.apexName ,
		startFor:
			for (ImVec2 local : itemLocals) {
				if (local.x == BoxX && local.y == BoxY) {
					BoxY -= 17;
					goto startFor;
				}
			}
			itemLocals.emplace_back(BoxX, BoxY);
			drawStrockText(drawList, font, myFontSize, { BoxX, BoxY }, entity.color, renderBuff);
		}

		else if (entity.type == 1) {//  人物透视0-item 1-player
			float BoxX = gamePoint.x + CentWindow.x + (worldArray[0][0] * entityLocal.x + worldArray[0][1] * entityLocal.y +
				worldArray[0][2] * entityLocal.z + worldArray[0][3]) * ViewW * CentWindow.x;
			float BoxY1 = gamePoint.y + CentWindow.y - (worldArray[1][0] * entityLocal.x + worldArray[1][1] * entityLocal.y +
				worldArray[1][2] * (entityLocal.z + 70) + worldArray[1][3]) * ViewW * CentWindow.y;
			float BoxY = gamePoint.y + CentWindow.y - (worldArray[1][0] * entityLocal.x + worldArray[1][1] * entityLocal.y +
				worldArray[1][2] * (entityLocal.z - 10) + worldArray[1][3]) * ViewW * CentWindow.y;
			if (BoxX - (BoxY1 - BoxY) / 4 > windowW || BoxX - (BoxY1 - BoxY) / 4 < -70 || BoxY1 > windowH || BoxY1 < 0) continue;
			ImColor playerColor;
			if (aimEntity == entity.point) {//如果是被瞄准的玩家
				playerColor = ImColor({ 0, 255, 255 });//blue
				glow2(entity.point, 20.f, 20.f, 0.f);
			}
			else {
				playerColor = ImColor({ 255, 80, 80 });//red
				glow2(entity.point, 0.f, 15.f, 0.f);
			}
			int blood = *(int*)&entityData[m_iHealth];
			int armor = *(int*)&entityData[m_shieldHealthMax - 4];
			int status = *(int*)&entityData[m_bleedoutState];
			if (blood <= 0 || blood > 100 || (blood == 50 && armor == 0)) continue;
			if (status != 0) {//倒地状态
				playerColor = ImColor({ 0, 120, 255 });
				glow2(entity.point, 0.f, 5.f, 5.f);
			}
			if (appConfigs.FangKuang) {//方框
				const char* fNormal = StrA(u8"%s\n%d\0");//hp:%d sh:%d WN %s
				const char* fName = StrA(u8"%s\n%d\0");
				if (entity.point == aimEntity) {
					readMem(gamePID, entity.pName, 256, pNameBuff);
					sprintf(aimBuff, fName, entity.WeaponName, entity.distance);
					aimDraw = { BoxX, BoxY, BoxY1, playerColor, aimBuff };
				}
				else {
					sprintf(renderBuff, fNormal, entity.WeaponName, entity.zc);//, armor, blood, entity.WeaponName entity.apexName,
					drawStrockText(drawList, font, 12, { (BoxX - (BoxY1 - BoxY) / 4) + (BoxY1 - BoxY) / 2, BoxY }, playerColor, renderBuff);
					//drawFrame(drawList, { BoxX - (BoxY1 - BoxY) / 4, BoxY, (BoxY1 - BoxY) / 2, BoxY1 - BoxY }, 1.f,playerColor);
				}
				ImVec2 tmpPiont;
				tmpPiont.x = CentWindow.x - BoxX;
				tmpPiont.y = CentWindow.y - BoxY;
				float showDistance = sqrt(tmpPiont.x * tmpPiont.x + tmpPiont.y * tmpPiont.y);
				if (entity.distance < appConfigs.TouShiFanWei && appConfigs.GUGETOUSHI) {//绘制骨骼
					DrawBone(drawList, entity.point, entityLocal, font, myFontSize, ImColor({ 0, 255, 255 }));
				}
			}
			//if (status != 0) continue;//倒地瞄准
			if ((aimThreadStop || aimEntityStatus != 0) && appConfigs.ZiDongMiaoZhun) {//
				ImVec2 tmpPiont;
				tmpPiont.x = CentWindow.x - BoxX;
				tmpPiont.y = CentWindow.y - BoxY;
				float showDistance = sqrt(tmpPiont.x * tmpPiont.x + tmpPiont.y * tmpPiont.y);
				if (entity.distance < 25 && !GetAsyncKeyState(VK_RBUTTON)) {
					insidePlayer.emplace_back(entity);
				}
				if (showDistance < appConfigs.ZiMiaoFanWei && entity.distance < appConfigs.TouShiFanWei) {
					if (showDistance < losDistance || losDistance == 0) {
						losDistance = showDistance;
						tempAim = entity.point;
					}
				}
			}
		}
		else if (entity.type == 2) {			
			char* name = (char*)u8"npc";//npc就是Non-Player Character,也就是非玩家控制角色
			float BoxX = gamePoint.x + CentWindow.x + (worldArray[0][0] * entityLocal.x + worldArray[0][1] * entityLocal.y +
				worldArray[0][2] * entityLocal.z + worldArray[0][3]) * ViewW * CentWindow.x;
			float BoxY = gamePoint.y + CentWindow.y - (worldArray[1][0] * entityLocal.x + worldArray[1][1] * entityLocal.y +
				worldArray[1][2] * entityLocal.z + worldArray[1][3]) * ViewW * CentWindow.y;
			if (BoxX > windowW || BoxX < -70 || BoxY > windowH || BoxY < 0) {
				continue;
			}
			const char* fNormal = "%s %d\0";
			sprintf(renderBuff, fNormal, name, entity.distance);
			itemLocals.emplace_back(BoxX, BoxY);
			drawStrockText(drawList, font, myFontSize, { BoxX, BoxY }, entity.color, renderBuff);
		}
		else if (entity.type == 3) {
			char* name = (char*)u8"GAS";//毒气瓶
			float BoxX = gamePoint.x + CentWindow.x + (worldArray[0][0] * entityLocal.x + worldArray[0][1] * entityLocal.y +
				worldArray[0][2] * entityLocal.z + worldArray[0][3]) * ViewW * CentWindow.x;
			float BoxY = gamePoint.y + CentWindow.y - (worldArray[1][0] * entityLocal.x + worldArray[1][1] * entityLocal.y +
				worldArray[1][2] * entityLocal.z + worldArray[1][3]) * ViewW * CentWindow.y;
			if (BoxX > windowW || BoxX < -70 || BoxY > windowH || BoxY < 0) {
				continue;
			}
			const char* fNormal = "%s\0";//| %d
			sprintf(renderBuff, fNormal, name);//, entity.distance
			itemLocals.emplace_back(BoxX, BoxY);
			drawStrockText(drawList, font, myFontSize, { BoxX, BoxY }, entity.color, renderBuff);
		}
		else if (entity.type == 4) {
			char* name = (char*)u8"ve";//载具
			float BoxX = gamePoint.x + CentWindow.x + (worldArray[0][0] * entityLocal.x + worldArray[0][1] * entityLocal.y +
				worldArray[0][2] * entityLocal.z + worldArray[0][3]) * ViewW * CentWindow.x;
			float BoxY = gamePoint.y + CentWindow.y - (worldArray[1][0] * entityLocal.x + worldArray[1][1] * entityLocal.y +
				worldArray[1][2] * entityLocal.z + worldArray[1][3]) * ViewW * CentWindow.y;
			if (BoxX > windowW || BoxX < -70 || BoxY > windowH || BoxY < 0) {
				continue;
			}
			const char* fNormal = "%s %d\0";//| %d
			sprintf(renderBuff, fNormal, name, entity.distance);//, entity.distance
			itemLocals.emplace_back(BoxX, BoxY);
			drawStrockText(drawList, font, myFontSize, { BoxX, BoxY }, entity.color, renderBuff);
		}
		else if (entity.type == 5) {
			char* name = (char*)u8"dloot";//载具
			float BoxX = gamePoint.x + CentWindow.x + (worldArray[0][0] * entityLocal.x + worldArray[0][1] * entityLocal.y +
				worldArray[0][2] * entityLocal.z + worldArray[0][3]) * ViewW * CentWindow.x;
			float BoxY = gamePoint.y + CentWindow.y - (worldArray[1][0] * entityLocal.x + worldArray[1][1] * entityLocal.y +
				worldArray[1][2] * entityLocal.z + worldArray[1][3]) * ViewW * CentWindow.y;
			if (BoxX > windowW || BoxX < -70 || BoxY > windowH || BoxY < 0) {
				continue;
			}
			const char* fNormal = "%s %d\0";//| %d
			sprintf(renderBuff, fNormal, name, entity.distance);//, entity.distance
			itemLocals.emplace_back(BoxX, BoxY);
			drawStrockText(drawList, font, myFontSize, { BoxX, BoxY }, entity.color, renderBuff);
		}
	}
	if (aimDraw.BoxY1) {
		drawStrockText(drawList, font, myFontSize, { (aimDraw.BoxX - (aimDraw.BoxY1 - aimDraw.BoxY) / 4) +
		(aimDraw.BoxY1 - aimDraw.BoxY) / 2, aimDraw.BoxY }, aimDraw.color, aimDraw.text);
		drawFrame(drawList, { aimDraw.BoxX - (aimDraw.BoxY1 - aimDraw.BoxY) / 4, aimDraw.BoxY,
		(aimDraw.BoxY1 - aimDraw.BoxY) / 2, aimDraw.BoxY1 - aimDraw.BoxY }, 1.5f, aimDraw.color);
	}
	sprintf(renderBuff, StrA(u8"50M Lives: %d\0"), _50Players);
	drawStrockText(drawList, font, 22, { (float)CentWindow.x - 100, 30.f }, _50Players == 0 ? ImColor{ 0, 255, 255 } : ImColor{ 255, 0, 0 }, renderBuff);

	float losInside = 0;
	Vec3D myLocal = {};
	readVec3D(_VIEWANGLE - 28, &myLocal);//viewangle-28等于我的位置的地址
	bool insideSubmit = false;//确认在范围内
	for (ApexEntity et1 : insidePlayer) {//for ApexEntity当中的insiderplayer
		Vec3D entityLocal = {};
		readVec3D(et1.point + m_vecAbsOrigin, &entityLocal);//玩家位置
		float ViewW = worldArray[3][0] * entityLocal.x + worldArray[3][1] * entityLocal.y + worldArray[3][2] * entityLocal.z + worldArray[3][3];
		if (ViewW < 0.01f) continue;
		ViewW = 1 / ViewW;//获取玩家的屏幕坐标
		float BoxX = gamePoint.x + CentWindow.x + (worldArray[0][0] * entityLocal.x + worldArray[0][1] * entityLocal.y +
			worldArray[0][2] * entityLocal.z + worldArray[0][3]) * ViewW * CentWindow.x;
		float BoxY1 = gamePoint.y + CentWindow.y - (worldArray[1][0] * entityLocal.x + worldArray[1][1] * entityLocal.y +
			worldArray[1][2] * (entityLocal.z + 70) + worldArray[1][3]) * ViewW * CentWindow.y;
		float BoxY = gamePoint.y + CentWindow.y - (worldArray[1][0] * entityLocal.x + worldArray[1][1] * entityLocal.y +
			worldArray[1][2] * (entityLocal.z - 10) + worldArray[1][3]
			) * ViewW * CentWindow.y;
		ImVec4 pRect = { BoxX - (BoxY1 - BoxY) / 4, BoxY, (BoxY1 - BoxY) / 2, BoxY1 - BoxY };
		pRect.z += pRect.x;
		pRect.w += pRect.y;
		//大数在前 小数在后
		// && (pRect.y > CentWindow.y && pRect.w < CentWindow.y)
		if ((pRect.x > CentWindow.x && pRect.z < CentWindow.x)) {//玩家在屏幕中心店
			float xx = entityLocal.x - myLocal.x;
			float yy = entityLocal.y - myLocal.y;
			float zz = entityLocal.z - myLocal.z;
			float pDistance = sqrt(xx * xx + yy * yy + zz * zz);
			if (pDistance < losInside || losInside == 0) {
				insideSubmit = true;//在中心点
				losInside = pDistance;
				tempAim = et1.point;
			}
		}
	}
	if (!insideSubmit && insidePlayer.size() > 0) {//如果不在中心点或
		for (ApexEntity et1 : insidePlayer) {
			Vec3D entityLocal = {};
			readVec3D(et1.point + m_vecAbsOrigin, &entityLocal);
			float xx = entityLocal.x - myLocal.x;
			float yy = entityLocal.y - myLocal.y;
			float zz = entityLocal.z - myLocal.z;
			float pDistance = sqrt(xx * xx + yy * yy + zz * zz);
			if (pDistance < losInside || losInside == 0) {
				insideSubmit = true;
				losInside = pDistance;
				tempAim = et1.point;
			}
		}
	}
	if (losDistance == 0 && losInside == 0 && aimThreadStop) {
		tempAim = 0;
	}
	aimEntity = tempAim;//被瞄准玩家
	if (appConfigs.ZiDongMiaoZhun) {
		if (GetAsyncKeyState(appConfigs.MiaoZhunReJian)) {//瞄准热键被按下 || GetAsyncKeyState(VK_CONTROL)
			if (aimEntity > 0) {
				aim = true;
				if (aimThreadStop) {
					aimThreadStop = false;
					ResumeThread(hAimThread);//恢复瞄准线程
				}
			}
			else {
				aim = false;
				losDistance = 0;
			}
		}
		else {
			aim = false;
			losDistance = 0;
		}
	}
}
/*绘制菜单*/
void drawMenu() {
	if (!appConfigs.MenuStatus) {
		return;
	}
	INT64 nowTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&nowTime);
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	const char* fps = StrA(u8"Framet: %lld ms / Fps: %.1f\0");
	INT64 usedTime = (nowTime - lastTime) / 10000;
	sprintf(renderBuff, fps, usedTime, usedTime == 0 ? 9999.f : (float)(1000.f / usedTime));
	lastTime = nowTime;
	drawStrockText(drawList, font, 16, { 10, 10 }, ImColor(0, 255, 255), renderBuff);

	int menuTop = 50;// (gameRect.bottom - 1000) / 3;
	int menuIndex = 0;
	drawStrockText(drawList, font, myFontSize, { 10, (float)menuTop }, { 0, 255, 255 }, StrA(u8"CatApex C++ [Home]Hide/Show"));

	++menuIndex;
	drawStrockText(drawList, font, myFontSize, { 10, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.FangKuang ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, StrA(u8"ESP[F11]"));
	drawStrockText(drawList, font, myFontSize, { 123, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.FangKuang ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, appConfigs.FangKuang ? StrA(u8"ON") : StrA(u8"OFF"));

	++menuIndex;
	drawStrockText(drawList, font, myFontSize, { 10, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.GUGETOUSHI ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, StrA(u8"DrawBone[F10]"));
	drawStrockText(drawList, font, myFontSize, { 123, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.GUGETOUSHI ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, appConfigs.GUGETOUSHI ? StrA(u8"ON") : StrA(u8"OFF"));

	++menuIndex;
	drawStrockText(drawList, font, myFontSize, { 10, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.FangKuang ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, StrA(u8"WallHack dis[F2]"));
	drawStrockText(drawList, font, myFontSize, { 123, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.FangKuang ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, to_string((int)appConfigs.TouShiFanWei).c_str());

	++menuIndex;
	drawStrockText(drawList, font, myFontSize, { 10, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.WuPingTouShi ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, StrA(u8"Item Draw[F3]"));
	drawStrockText(drawList, font, myFontSize, { 123, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.WuPingTouShi ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, appConfigs.WuPingTouShi ? StrA(u8"ON") : StrA(u8"OFF"));

	++menuIndex;
	drawStrockText(drawList, font, myFontSize, { 10, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.WuPingTouShi ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, StrA(u8"Item range[F4]"));
	drawStrockText(drawList, font, myFontSize, { 123, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.WuPingTouShi ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, to_string((int)appConfigs.WuPingFanWei).c_str());

	++menuIndex;
	drawStrockText(drawList, font, myFontSize, { 10, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.PeiJianTouShi ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, StrA(u8"Hide Mice[F5]"));
	drawStrockText(drawList, font, myFontSize, { 123, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.PeiJianTouShi ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, appConfigs.PeiJianTouShi ? StrA(u8"ON") : StrA(u8"OFF"));

	++menuIndex;
	drawStrockText(drawList, font, myFontSize, { 10, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.ZiDongMiaoZhun ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, StrA(u8"Aimbot[F6]"));
	drawStrockText(drawList, font, myFontSize, { 123, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.ZiDongMiaoZhun ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, appConfigs.ZiDongMiaoZhun ? StrA(u8"ON") : StrA(u8"OFF"));

	++menuIndex;
	drawStrockText(drawList, font, myFontSize, { 10, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.XianShiZhaZhu ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, StrA(u8"Show gender[F7]"));
	drawStrockText(drawList, font, myFontSize, { 123, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.XianShiZhaZhu ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, appConfigs.XianShiZhaZhu ? StrA(u8"ON") : StrA(u8"OFF"));

	++menuIndex;
	drawStrockText(drawList, font, myFontSize, { 10, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.ZIDONGGUAJI ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, StrA(u8"Auto hang up[F9]"));
	drawStrockText(drawList, font, myFontSize, { 123, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.ZIDONGGUAJI ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, appConfigs.ZIDONGGUAJI ? StrA(u8"ON") : StrA(u8"OFF"));

	++menuIndex;
	drawStrockText(drawList, font, myFontSize, { 10, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.KaiFaZheXuanXiang ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, StrA(u8"Developer[F12]"));
	drawStrockText(drawList, font, myFontSize, { 123, (float)menuTop + (menuIndex + 1) * 20 - 2 },
		appConfigs.KaiFaZheXuanXiang ? ImColor{ 0, 255, 255 } : ImColor{ 255, 255, 255 }, appConfigs.KaiFaZheXuanXiang ? StrA(u8"ON") : StrA(u8"OFF"));
}