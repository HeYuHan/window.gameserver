#pragma once
#ifndef __RACE_ROAD_CHECKER_H__
#define __RACE_ROAD_CHECKER_H__
#define MAX_CHECK_POINT_COUNT 128
#include <Vector3.h>
#include <common.h>
struct CheckerPoint
{
	Vector3 mPosition;
	float mRadius;
	int mIndex;
	bool mEndPoint;
};



class RaceRoadCheckerManager
{
public:
	RaceRoadCheckerManager();
	~RaceRoadCheckerManager();
	bool Check(Vector3 pos, CheckerPoint &p);
	void Init();
	bool CheckDir(Vector3 run_dir, Vector3 target_postion,int next,Vector3 &right_dir);
	//bool GetPonit(int index, CheckerPoint &p);
private:
	CheckerPoint m_AllPoint[MAX_CHECK_POINT_COUNT];
	int m_PointCount;
};





#endif // !__RACE_ROAD_CHECKER_H__
