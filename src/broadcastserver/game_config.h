
#ifndef __GAME_CONFIG_H__
#define __GAME_CONFIG_H__
#include <vector>
#include <common.h>
#include <Vector3.h>
#include <LuaEngine.h>
#include "DropItemManager.h"
#define MAX_BRITH_POS_COUNT 40
#define MAX_DORPITME_POS_COUNT 50
#define MAX_LEVEL_COUNT  50


struct BrithPose
{
	Vector3 position;
	Vector3 rotation;
};


class GameConfig
{

private:
	void AddBrithPose(float px, float py, float pz, float rx, float ry, float rz);
	void AddDropItemPosition(int type, float px, float py, float pz);
	DropItemData* AddDropItem();
public:
	bool Init();
public:
	bool CopyBrithPose(int index,BrithPose &p);
	bool CopyAllDropItemData(DropItemData *dest,int &count);
	bool CopyDropItemPosition(int type, int index, Vector3 &p);
public:
	float m_GameTime;
	int m_TotalCircle;
	int m_DropItemCount;
	int m_BrithPoseCount;
	const char* m_ClientPath;
	const char* m_ClientExeArg;
	DropItemData m_DropItemData[MAX_DROP_COUNT];
	BrithPose m_BrithPoseArray[MAX_BRITH_POS_COUNT];
	
	Vector3 m_DropItemPositionArray[DROPITEM_COUNT][MAX_DORPITME_POS_COUNT];
	int m_DropItemPositionCount[DROPITEM_COUNT];

};
extern GameConfig gConfig;







#endif // !__GAME_CONFIG_H__
