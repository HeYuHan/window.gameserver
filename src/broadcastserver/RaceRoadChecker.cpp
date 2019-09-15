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

bool RaceRoadCheckerManager::CheckDir(Vector3 run_dir, Vector3 target_postion, int next, Vector3 &right_dir)
{
	bool b = false;
	for (int i = 0; i < m_PointCount; i++)
	{
		if (m_AllPoint[i].mIndex == next)
		{
			Vector3 dir = Normalize(m_AllPoint[i].mPosition - target_postion);
			b = Dot(run_dir, dir) > -0.5f;
			right_dir = dir;
			if (b)break;
		}
	}
	return b;
}

