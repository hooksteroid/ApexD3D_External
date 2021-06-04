#include <string>
#include "proc_help.h"

#ifndef CATAPEXCPP_APEXUTILS_H
#define CATAPEXCPP_APEXUTILS_H

using std::string;
using std::wstring;

#endif //CATAPEXCPP_APEXUTILS_H

void readWorldArray(void* array);
void readVec3D(__int64 addr, Vec3D* vec3D);
void writeVec3D(__int64 addr, Vec3D* vec3D);
void readPlayerName(int index, char* buff);
__int64 getNamePoint(int index);
Vec3D GetBonePos(__int64 entity, int ID, Vec3D entityLocal);
Vec3D CalcBonePos(float matrix[128][3][4], int ID, Vec3D entityLocal);
void GetBoneArray(__int64 entity, void* arrayBuf);
void DrawBone(ImDrawList* drawList, __int64 entity, Vec3D entityLocal, ImFont* font, float size, ImColor col);
int GetEntityType(__int64 entityPoint);
void GetEntityTypeStr(__int64 entityPoint, char* entityInfoNameStr);
const char* GetWeaponName(char* mName);
void drawBones(ImDrawList* drawList, __int64 entity, Vec3D local, ImColor col, int boneIndex[3][16]);
