
#ifndef __GAME_CONFIG_H__
#define __GAME_CONFIG_H__
#include <vector>
#include <common.h>
#include <Vector3.h>
#include <LuaEngine.h>
#include "DropItemManager.h"
#include "RaceRoadChecker.h"
#define MAX_BRITH_POS_COUNT 40
#define MAX_DORPITME_POS_COUNT 500
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
	void AddDropItemPosition(int type,int subType, float px, float py, float pz);
	void AddCheckPoint(int index, float r, float px, float py, float pz, bool end);
	DropItemData* AddDropItem();
public:
	bool Init();
public:
	bool CopyBrithPose(int index,BrithPose &p);
	bool CopyAllDropItemData(DropItemData *dest,int &count);
	bool CopyDropItemPosition(int type, int index, DropItemPosition &p);
	bool CopyCheckPointData(CheckerPoint* dest, int &size);
public:
	float m_GameTime;
	float m_GameBalanceTime;
	float m_GameBalanceTimeForPad;
	float m_GameShowTime;
	float m_KeepAlive;
	int m_TotalCircle;
	int m_DropItemCount;
	int m_BrithPoseCount;
	int m_CheckerPointCount;
	std::string m_ClientPath;
	std::string m_ClientExeArg;
	std::string m_ClientConfig;
	DropItemData m_DropItemData[MAX_DROP_COUNT];
	BrithPose m_BrithPoseArray[MAX_BRITH_POS_COUNT];
	CheckerPoint m_CheckerPointArray[MAX_CHECK_POINT_COUNT];
	DropItemPosition m_DropItemPositionArray[DROPITEM_COUNT][MAX_DORPITME_POS_COUNT];
	int m_DropItemPositionCount[DROPITEM_COUNT];

};
extern GameConfig gConfig;







#endif // !__GAME_CONFIG_H__
