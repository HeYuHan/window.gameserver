#pragma once

#ifndef __DROP_ITEM_MANAGER_H__
#define __DROP_ITEM_MANAGER_H__
#define MAX_DROP_POSITION_COUNT 50
#define MAX_DROP_COUNT 64
#include <vector>
#include <Vector3.h>
class Game;
enum DropItemType
{
	DROPITEM_SET_NONE = 0,
	DROPITEM_ICON = 1,//金币
	DROPITEM_ACCELERATE = 2,//加速
	DROPITEM_RPG_GUN = 3,//火箭炮
	DROPITEM_SHIELD = 4,//盾
	DROPITEM_STORM = 5,//磁力风暴
	DROPITEM_COUNT = 6

};
struct DropItemData
{
	int mInitCount;
	int mMaxCount;
	int mType;
	float mRefreshTime;
	float mCheckRange;
	bool mSet;
};
struct DropItem
{
	
	DropItemData mData;
	int mRefreshIndex;
	int mCount;
	float mRefreshTime;
	bool mInitRefresh;
};
struct AliveDroptItem
{
	unsigned int mID;
	Vector3 mPosition;
	int mIndex;
};
enum DropItemEvent
{
	DROP_EVENT_CREATE,DROP_EVENT_GET
};
struct DropItemEventListenner
{
	virtual void OnDropItemEvent(DropItemEvent event, AliveDroptItem item,void* arg) = 0;
};
class DropItemManager
{
public:
	DropItemManager();
	~DropItemManager();
public:
	bool Init(DropItemEventListenner* game);
	void Update(float t);
	void GetDropItem(Vector3 &pos,unsigned int uid);
	void CreateDropItem(int index);
private:
	DropItemEventListenner* listenner;
	unsigned int gID;
	DropItem mAllDropItem[MAX_DROP_COUNT];
	int mAllDropTtemCount;
public:
	std::vector<AliveDroptItem> m_AliveDropItem;
};

#endif // !__DROP_ITEM_MANAGER_H__



