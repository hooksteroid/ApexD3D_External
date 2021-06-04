#include "apexUtils.h"
#include "DriverHelp.h"
#include "proc_help.h"
#include "DrawEng.h"

char* entityInfoName = 0;
bool infoInit = false;

void readWorldArray(void* array) {//获取游戏世界矩阵，由viewmetrix获取
	readMem(gamePID, ArrayPoint, 64, array);
}
void readVec3D(__int64 addr, Vec3D* vec3D) {
	readMem(gamePID, addr, 12, vec3D);
}
void writeVec3D(__int64 addr, Vec3D* vec3D) {
	writeMem(gamePID, addr, 12, vec3D);
}
void readPlayerName(int index, char* buff) {
	readMem(gamePID, getNamePoint(index), 256, buff);
}
__int64 getNamePoint(int index) {
	__int64 nameArray = 0;
	readMem(gamePID, hGameModule + NameList, sizeof(nameArray), &nameArray);
	nameArray = nameArray + index * 8;
	__int64 namePoint = 0;
	readMem(gamePID, nameArray + m_nameArrayListSub, sizeof(namePoint), &namePoint);
	return namePoint;
}

std::string get_player_name(uint64_t entity)
{
	uint64_t name_list = hGameModule + 0x8167BB0;
	uint64_t name_array = Read<uint64_t>((uint32_t)gamePID, name_list + ((Read<uint64_t>((uint32_t)gamePID, entity + 0x38) - 1) << 4)); //0x50 and 0x58 should work too

	std::string player_name = Read<std::string>((uint32_t)gamePID, name_array);

	return player_name;
}

Vec3D GetBonePos(__int64 entity, int ID, Vec3D entityLocal) {//获取人物人体部位的位置
	float matrix[128][3][4];
	GetBoneArray(entity, &matrix);
	return CalcBonePos(matrix, ID, entityLocal);
}
Vec3D CalcBonePos(float matrix[128][3][4], int ID, Vec3D entityLocal) {
	Vec3D bone_pos = { matrix[ID][0][3], matrix[ID][1][3], matrix[ID][2][3] };
	bone_pos.x += entityLocal.x;
	bone_pos.y += entityLocal.y;
	bone_pos.z += entityLocal.z;
	return bone_pos;
}

void GetBoneArray(__int64 entity, void* arrayBuf) {//获取人物身体矩阵，由bone偏移获取
	__int64 pBoneMatrix = 0;
	float matrix[128][3][4];
	readMem(gamePID, entity + OFFSET_BONES, sizeof(pBoneMatrix), &pBoneMatrix);
	readMem(gamePID, pBoneMatrix, sizeof(matrix), arrayBuf);
}

void Vec3DBoneToScreen(Vec3D local, ImVec2* point) {
	float ViewW = worldArray[3][0] * local.x + worldArray[3][1] * local.y + worldArray[3][2] * local.z + worldArray[3][3];
	if (ViewW < 0.01) return;
	ViewW = 1 / ViewW;
	point->x = gamePoint.x + CentWindow.x + (worldArray[0][0] * local.x + worldArray[0][1] * local.y + worldArray[0][2] * local.z + worldArray[0][3]) * ViewW * CentWindow.x;
	point->y = gamePoint.y + CentWindow.y - (worldArray[1][0] * local.x + worldArray[1][1] * local.y + worldArray[1][2] * local.z + worldArray[1][3]) * ViewW * CentWindow.y;
}

void DrawBone(ImDrawList* drawList, __int64 entity, Vec3D entityLocal, ImFont* font, float size, ImColor col) {
	int entityType = GetEntityType(entity);
	if (entityType == 0) {
		drawBones(drawList, entity, entityLocal, col, 寻血);
		return;
	}
	else if (entityType == 1) {
		drawBones(drawList, entity, entityLocal, col, 直布);
	}
	else if (entityType == 2) {
		drawBones(drawList, entity, entityLocal, col, 命脉);
	}
	else if (entityType == 3) {
		drawBones(drawList, entity, entityLocal, col, 探路);
	}
	else if (entityType == 4) {
		drawBones(drawList, entity, entityLocal, col, 动力);
	}
	else if (entityType == 5) {
		drawBones(drawList, entity, entityLocal, col, 恶灵);
	}
	else if (entityType == 6) {
		drawBones(drawList, entity, entityLocal, col, 班加);
	}
	else if (entityType == 7) {
		drawBones(drawList, entity, entityLocal, col, 侵蚀);
	}
	else if (entityType == 8) {
		//for (size_t i = 0; i < 128; i++)
		//{
		//	ImVec2 cuIndex = {};
		//	Vec3DBoneToScreen(GetBonePos(entity, i, entityLocal), &cuIndex);
		//	char buffer[20];
		//	itoa(i, buffer, 10);
		//	drawStrockText(drawList, font, 11, cuIndex, col, buffer);
		//}
		drawBones(drawList, entity, entityLocal, col, 幻象);
	}
	else if (entityType == 9) {
		drawBones(drawList, entity, entityLocal, col, 华森);
	}
	else if (entityType == 10) {
		drawBones(drawList, entity, entityLocal, col, 华森);
	}
}
/*获取*/
int GetEntityType(__int64 entityPoint) {
	__int64 entityInfos = 0;
	readMem(gamePID, entityPoint + m_ModelName, sizeof(entityInfos), &entityInfos);
	if (!infoInit) {
		infoInit = true;
		entityInfoName = (char*)malloc(80);
	}
	readMem(gamePID, entityInfos, 80, entityInfoName);
	//printf("entityInfoName: %s\n", entityInfoName);
	if (strstr(entityInfoName, "bloodhound")) {
		return 0; // 寻血
	}
	else if (strstr(entityInfoName, "gibraltar")) {
		return 1; // 直布
	}
	else if (strstr(entityInfoName, "support")) {
		return 2; // 命脉
	}
	else if (strstr(entityInfoName, "pathfinder")) {
		return 3; // 探路
	}
	else if (strstr(entityInfoName, "stim")) {
		return 4; // 动力
	}
	else if (strstr(entityInfoName, "wraith")) {
		return 5; // 恶灵
	}
	else if (strstr(entityInfoName, "bangalore")) {
		return 6; // 班加
	}
	else if (strstr(entityInfoName, "caustic")) {
		return 7; //侵蚀
	}
	else if (strstr(entityInfoName, "holo")) {
		return 8; // 幻象
	}
	else if (strstr(entityInfoName, "wattson")) {
		return 9; // 华森
	}

	else if (strstr(entityInfoName, "generic")) {
		return 10; // dummie
	}
	else {
		if (entityInfoName[0] != 0) {
			//printf("entityInfoName: %s\n", entityInfoName);
		}
		return -1;
	}
}
/*这个就是获取ON发者中的那些文字的功能*/
void GetEntityTypeStr(__int64 entityPoint, char* entityInfoNameStr) {
	__int64 entityInfos = 0;
	readMem(gamePID, entityPoint + m_ModelName, sizeof(entityInfos), &entityInfos);
	readMem(gamePID, entityInfos, 80, entityInfoNameStr);
}

const char* GetWeaponName(char* mName) {
	if (!strcmp("empty_handed", mName)) {
		return u8"unarm";
	}
	else if (!strcmp("lstar", mName)) {
		return "LSTAR";
	}
	else if (!strcmp("mastiff_stgn", mName)) {
		return u8"Mastiff";
	}
	else if (!strcmp("at_rifle", mName)) {
		return "AWM";
	}
	else if (!strcmp("pstl_sa3", mName)) {
		return u8"pstl";
	}
	else if (!strcmp("peacekeeper", mName)) {
		return u8"peacekeeper";
	}
	else if (!strcmp("w1128", mName)) {
		return "EVA-8";
	}
	else if (!strcmp("p2011_auto", mName)) {
		return u8"RE-45";
	}
	else if (!strcmp("p2011", mName)) {
		return "P2020";
	}
	else if (!strcmp("rspn101", mName)) {
		return "R301";
	}
	else if (!strcmp("r97", mName)) {
		return "R99";
	}
	else if (!strcmp("alternator_smg", mName)) {
		return u8"alternator_smg";
	}
	else if (!strcmp("g2", mName)) {
		return "G7";
	}
	else if (!strcmp("b3wing", mName)) {
		return u8"WingMan";
	}
	else if (!strcmp("lmg_hemlok", mName)) {
		return "M600";
	}
	else if (!strcmp("rspn101_dmr", mName)) {
		return u8"LongB";
	}
	else if (!strcmp("prowler_smg", mName)) {
		return u8"prowler_smg";
	}
	else if (!strcmp("m1a1_hemlok", mName)) {
		return u8"hemlok";
	}
	else if (!strcmp("vinson", mName)) {
		return "VK47";
	}
	else if (!strcmp("doubletake", mName)) {
		return u8"tribletake";
	}
	else if (!strcmp("hemlock_br", mName)) {
		return u8"hemlock_br";
	}
	else if (!strcmp("beam_ar", mName)) {
		return u8"Harvok";
	}
	else if (!strcmp("grenades", mName)) {
		return u8"grenades";
	}
	else {
		return mName;
	}
}
/*绘制骨骼*/
void drawBones(ImDrawList* drawList, __int64 entity, Vec3D local, ImColor col, int boneIndex[3][16]) {
	ImVec2 lasteIndex = {};
	ImVec2 cuIndex = {};
	float matrix[128][3][4];
	GetBoneArray(entity, &matrix);
	for (size_t i = 1; i < 5; i++) {
		if (i == 1) Vec3DBoneToScreen(CalcBonePos(matrix, boneIndex[0][0], local), &lasteIndex);
		Vec3DBoneToScreen(CalcBonePos(matrix, boneIndex[0][i], local), &cuIndex);
		drawList->AddLine(lasteIndex, cuIndex, col, 1.f);
		lasteIndex = cuIndex;
	}

	for (size_t i = 1; i < 16; i++) {
		if (i == 1) Vec3DBoneToScreen(CalcBonePos(matrix, boneIndex[1][0], local), &lasteIndex);
		Vec3DBoneToScreen(CalcBonePos(matrix, boneIndex[1][i], local), &cuIndex);
		drawList->AddLine(lasteIndex, cuIndex, col, 1.f);
		lasteIndex = cuIndex;
	}

	for (size_t i = 1; i < 7; i++) {
		if (i == 1) Vec3DBoneToScreen(CalcBonePos(matrix, boneIndex[2][0], local), &lasteIndex);
		Vec3DBoneToScreen(CalcBonePos(matrix, boneIndex[2][i], local), &cuIndex);
		drawList->AddLine(lasteIndex, cuIndex, col, 1.f);
		lasteIndex = cuIndex;
	}
}