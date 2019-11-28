#pragma once
#ifndef __GAME_H__
#define __GAME_H__
#include <common.h>
#include <Timer.h>

#include "DropItemManager.h"
#include "RaceRoadChecker.h"

#define MAX_MAP_DATA_LEN 1024 * 128
enum GAME_SEVER_MESSAGE
{
	SM_LOAD_GAME = 1,
	SM_GAME_START,
	SM_PLAYER_MOVE,
	SM_GAME_BALANCE,
	SM_GAME_END,
	SM_GAME_SYNC_TIME,
	SM_CREATE_DROP_ITEM,
	SM_REMOVE_DROP_ITEM,
	SM_PLSYER_RUN_DIR_ERROR,
	SM_GAME_STATE_ERROR,
	SM_SYNC_RANK_SCORE,
	SM_RESET_POSITION,
	SM_KEEP_ALIVE
	
	
};
enum GAME_CLIENT_MESSAGE
{
	CM_OB_PLAYER_JOIN=1,
	CM_REQUEST_LOAD_GAME,
	CM_GAME_MAP_LOADED,
	CM_PLAYER_MOVE,
	CM_COMMIT_SCORE,
	CM_GET_DROP_ITEM,
	CM_STOP_GAME,
	CM_KEEP_ALIVE,
	CM_REQUEST_RESET_POSITION,
	CM_SAVE_FILE,
	

};

enum PLYAER_MOVE_FLAG
{
	MOVE_FLAG_INPUT_STEER=1<<0,
	MOVE_FLAG_INPUT_BRAKE=1<<1,
	MOVE_FLAG_INPUT_RUN=1<<2,
	MOVE_FLAG_POSE_POSTION=1<<3,
	MOVE_FLAG_POSE_ROTATION=1<<4,
	MOVE_FLAG_RIGBODY_ANGULARVELOCITY = 1<<5,
	MOVE_FLAG_RIGBODY_VELOCITY=1<<6
};


enum GameState
{
	None, Wait,Load,Play, Balance,SyncScore,Show
};

class Client;
class Game: public DropItemEventListenner
{
public:
	Game();
	~Game();
public:
	void OnClientMessage(Client* c);
	void OnClientDisconnect(Client* c);
	void OnPlayerMove(Client* c);
	void OnPlayerStopGame(Client* c);
	void OnPlayerGetDropItem(Client* c);
	void OnClientSaveFile(Client* c);
	void ClearData();
	void EndGame();
	void Update(float t);
	void BalanceGame();

private:
	//void OnRequestSelectMap(Client* c);
	void OnRequestLoadGame(Client* c);
	void OnClientMapLoaded(Client* c);
	void StartGame();
	void SyncGameTime();
	void OnClientCommitSocre(Client* c);
public:
	GameState m_GameState;
private:
	int m_BrithIndex;
	float m_GameTotleTime;
	float m_GameSyncTime;
	float m_GameShowTime;
	float m_GameRunTime;
	float m_RestartTimer;
	DropItemManager m_DropManager;
	RaceRoadCheckerManager m_RoadCheckerManager;

	// ͨ�� DropItemEventListenner �̳�
	virtual void OnDropItemEvent(DropItemEvent event, AliveDroptItem item, void * arg) override;
};
extern Game gGame;
#endif // !__GAME_H__




