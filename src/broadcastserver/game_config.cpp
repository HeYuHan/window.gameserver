#include "game_config.h"
GameConfig gConfig;
void GameConfig::AddBrithPose(float px,float py, float pz, float rx, float ry, float rz)
{

	BrithPose &pose = m_BrithPoseArray[m_BrithPoseCount];
	pose.position = Vector3(px, py, pz);
	pose.rotation = Vector3(px, py, pz);
	m_BrithPoseCount++;
}

void GameConfig::AddDropItemPosition(int type, float px, float py, float pz)
{
	if (type > DROPITEM_SET_NONE && type < DROPITEM_COUNT)
	{
		int &dropCount = m_DropItemPositionCount[type];
		Vector3 &p = m_DropItemPositionArray[type][dropCount];
		p.x = px;
		p.y = py;
		p.z = pz;
		dropCount++;
	}
}

void GameConfig::AddCheckPoint(int index, float r, float px, float py, float pz, bool end)
{
	if (index > 64 || index < 0)
	{
		log_error("check point max index %d  set %d", 64, index);
		return;
	}
	if (m_CheckerPointCount < MAX_CHECK_POINT_COUNT)
	{
		m_CheckerPointArray[m_CheckerPointCount].mIndex = index;
		m_CheckerPointArray[m_CheckerPointCount].mRadius = r;
		m_CheckerPointArray[m_CheckerPointCount].mPosition = Vector3(px, py, pz);
		m_CheckerPointArray[m_CheckerPointCount].mEndPoint = end;
		m_CheckerPointCount++;
	}
	else
	{
		log_error("check point max size %d", MAX_CHECK_POINT_COUNT);
	}
}


DropItemData* GameConfig::AddDropItem()
{
	DropItemData* ret = &(m_DropItemData[m_DropItemCount]);
	ret->mSet = true;
	m_DropItemCount++;
	return ret;

}

GameConfig* getGlobalConfig()
{
	return &gConfig;
}
bool GameConfig::Init()
{
	GameConfig* configPointer = &gConfig;
	lua_State *L = Core::LuaEngine::NewState();
	int top = lua_gettop(L);
	luabridge::getGlobalNamespace(L)
		.beginClass<GameConfig>("GameConfig")
		.addFunction("AddBrithPose", &GameConfig::AddBrithPose)
		.addFunction("AddDropItemPosition", &GameConfig::AddDropItemPosition)
		.addFunction("AddCheckPoint", &GameConfig::AddCheckPoint)
		.addFunction("AddDropItem", &GameConfig::AddDropItem)
		.addData("ClientPath",&GameConfig::m_ClientPath)
		.addData("ClientArg", &GameConfig::m_ClientExeArg)
		.addData("GameTime", &GameConfig::m_GameTime)
		.addData("TotalCircle", &GameConfig::m_TotalCircle)
		.endClass()
		.addFunction("gConfig", getGlobalConfig)
		//.addVariable("gConfig", configPointer)
		.beginClass<DropItemData>("DropItemData")
		.addData("mMaxCount", &DropItemData::mMaxCount, true)
		.addData("mInitCount", &DropItemData::mInitCount, true)
		.addData("mCheckRange", &DropItemData::mCheckRange, true)
		.addData("mRefreshTime", &DropItemData::mRefreshTime, true)
		.addData("mType", &DropItemData::mType, true)
		.endClass();

	lua_settop(L, top);
	memset(m_DropItemPositionCount, 0, sizeof(int)*DROPITEM_COUNT);
	memset(m_DropItemData, 0, sizeof(DropItemData)*MAX_DROP_COUNT);
	m_BrithPoseCount = 0;
	m_DropItemCount = 0;
	m_CheckerPointCount = 0;
	m_ClientPath = NULL;
	m_ClientExeArg = NULL;
	const char* auto_path = AutoFilePath("config.lua");
	if (auto_path)
	{
		if (LUA_OK != luaL_dofile(L, auto_path))
		{
			log_error("cant run lua error: %s", lua_tostring(L, -1));
			return false;
		}
	}
	else
	{
		log_warn("cant find mode script %s", "config.lua");
	}
	lua_close(L);
	return true;
}

bool GameConfig::CopyBrithPose(int index, BrithPose & p)
{
	
	index = index % m_BrithPoseCount;
	p = m_BrithPoseArray[index];
	return true;
}

bool GameConfig::CopyAllDropItemData(DropItemData *dest, int &count)
{
	
	count = m_DropItemCount;
	memcpy(dest, m_DropItemData, sizeof(DropItemData)*count);
	return true;
}

bool GameConfig::CopyDropItemPosition(int type, int index, Vector3 & p)
{
	if (type >DROPITEM_SET_NONE && type < DROPITEM_COUNT)
	{
		index = index % m_DropItemPositionCount[type];
		p = m_DropItemPositionArray[type][index];
		return true;
	}
	return false;
}

bool GameConfig::CopyCheckPointData(CheckerPoint * dest, int & size)
{
	if (m_CheckerPointCount > size)return false;
	memcpy(dest, m_CheckerPointArray, sizeof(CheckerPoint)*m_CheckerPointCount);
	size = m_CheckerPointCount;
	return true;
}


