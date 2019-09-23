#include "RaceRoadChecker.h"
#include "game_config.h"
RaceRoadCheckerManager::RaceRoadCheckerManager()
{
}

RaceRoadCheckerManager::~RaceRoadCheckerManager()
{
}

bool RaceRoadCheckerManager::Check(Vector3 pos, CheckerPoint & p)
{
	for (int i = 0; i < m_PointCount; i++)
	{
		CheckerPoint cp = m_AllPoint[i];
		Vector3 dir = pos - cp.mPosition;
		float len = Length(dir);
		if (len <= cp.mRadius)
		{
			p = cp;
			return true;
		}
	}
	return false;
}

void RaceRoadCheckerManager::Init()
{
	m_PointCount = MAX_CHECK_POINT_COUNT;
	gConfig.CopyCheckPointData(m_AllPoint, m_PointCount);
}

void RaceRoadCheckerManager::GetDir(Vector3 & ret, int next)
{
	Vector3 v1, v2;
	int set = 0;
	for (int i = 0; i < m_PointCount; i++)
	{
		if (m_AllPoint[i].mIndex == next)
		{
			v2 = m_AllPoint[i].mPosition;
			set += 1;

		}
		else if (m_AllPoint[i].mIndex == next - 1)
		{
			set += 1;
			v1 = m_AllPoint[i].mPosition;
		}
		if (set == 2)break;
	}
	if(set == 2)ret = Normalize(v2 - v1);
}

bool RaceRoadCheckerManager::CheckDir(Vector3 run_dir, Vector3 target_postion, int next, Vector3 &right_dir)
{
	bool b = false;
	Vector3 v1, v2;
	int set = 0;
	for (int i = 0; i < m_PointCount; i++)
	{
		if (m_AllPoint[i].mIndex == next)
		{
			v2 = m_AllPoint[i].mPosition;
			set += 1;
			
		}
		else if (m_AllPoint[i].mIndex == next - 1)
		{
			set += 1;
			v1 = m_AllPoint[i].mPosition;
		}
		if (set == 2)break;
	}
	right_dir = Normalize(v2 - v1);
	run_dir = Normalize(run_dir);

	b = Dot(run_dir, right_dir) > -0.5f;
	return b;
}

