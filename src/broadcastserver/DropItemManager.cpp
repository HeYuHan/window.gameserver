#include "game_config.h"
#include "Game.h"

#include "DropItemManager.h"

DropItemManager::DropItemManager():listenner(NULL)
{
}

DropItemManager::~DropItemManager()
{
}
bool DropItemManager::Init(DropItemEventListenner* game)
{
	if (!game)return false;
	this->listenner = game;
	mAllDropTtemCount = gConfig.m_DropItemCount;
	for (int i = 0; i < mAllDropTtemCount; i++)
	{
		DropItem* item = &mAllDropItem[i];
		memset(item, 0,sizeof(DropItem));
		item->mData = gConfig.m_DropItemData[i];

	}
	gID = 0;
	return true;;
}

void DropItemManager::Update(float t)
{
	if (!listenner)return;
	for (int i = 0; i < mAllDropTtemCount; i++)
	{
		DropItem* item = &mAllDropItem[i];
		if (item->mData.mSet)
		{
			if (!item->mInitRefresh)
			{
				item->mInitRefresh = true;
				for (int j = 0; j < item->mData.mInitCount; j++)
				{
					CreateDropItem(i);
				}
			}
			if (item->mCount < item->mData.mMaxCount)
			{
				item->mRefreshTime += t;
				if (item->mRefreshTime > item->mData.mRefreshTime)
				{
					item->mRefreshTime = item->mRefreshTime - item->mData.mRefreshTime;
					CreateDropItem(i);
				}
			}
			else
			{
				item->mRefreshTime = 0;
			}
		}
	}
}

void DropItemManager::GetDropItem(Vector3 &pos, unsigned int uid)
{
	for (int i = m_AliveDropItem.size() - 1; i >= 0; i--)
	{
		AliveDroptItem* item = &m_AliveDropItem[i];
		DropItem* origin = &mAllDropItem[item->mIndex];
		Vector3 itemPos=item->mPosition.pos;
		
		float dis = Length(itemPos - pos);
		if (dis < origin->mData.mCheckRange)
		{
			listenner->OnDropItemEvent(DROP_EVENT_GET, *item,(void*)uid);
			/*if (remove_if_get)
			{
				listenner->OnDropItemEvent(DROP_EVENT_REMOVE, *item, (void*)uid);
			}*/
			m_AliveDropItem.erase(m_AliveDropItem.begin() + i);
			origin->mCount--;
		}
	}
}

void DropItemManager::CreateDropItem(int index)
{
	if (index >= 0 && index < mAllDropTtemCount)
	{
		DropItem* origin = &mAllDropItem[index];
		AliveDroptItem alive;
		alive.mID = gID++;
		gConfig.CopyDropItemPosition(origin->mData.mType, origin->mRefreshIndex++, alive.mPosition);
		alive.mIndex = index;
		origin->mCount++;
		m_AliveDropItem.push_back(alive);
		listenner->OnDropItemEvent(DROP_EVENT_CREATE, alive, NULL);
	}
}

DropItem* DropItemManager::GetDropItem(unsigned int id, bool remove)
{
	for (int i = m_AliveDropItem.size() - 1; i >= 0; i--)
	{
		AliveDroptItem* item = &m_AliveDropItem[i];
		DropItem* origin = &mAllDropItem[item->mIndex];
		if (item->mID == id)
		{
			if (remove)m_AliveDropItem.erase(m_AliveDropItem.begin() + i);
			return origin;
		}
	}
	return NULL;
}

