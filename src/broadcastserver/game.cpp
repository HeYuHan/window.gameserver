#include "game.h"
#include "server.h"
#include "game_config.h"
#include <properties.h>
#include <vector>
using namespace Core;
typedef Core::LocalUserDatabaseHelper::SampleUser User;
Game gGame;
Game::Game():m_GameState(None)
{
}

Game::~Game()
{
}

void Game::OnClientMessage(Client * c)
{
	Core::byte cmd;
	c->ReadByte(cmd);
	switch (cmd)
	{
	case CM_OB_PLAYER_JOIN:
		c->m_IsObClient = true;
		log_info("ob client connect");
		break;
	case CM_PLAYER_MOVE:
		OnPlayerMove(c);
		break;
	case CM_REQUEST_LOAD_GAME:
		OnRequestLoadGame(c);
		break;
	case CM_GAME_MAP_LOADED:
		OnClientMapLoaded(c);
		break;
	case CM_COMMIT_SCORE:
		OnClientCommitSocre(c);
		break;
	case CM_STOP_GAME:
		OnPlayerStopGame(c);
		break;
	case CM_GET_DROP_ITEM:
		OnPlayerGetDropItem(c);
		break;
	default:
		break;
	}
}

void Game::OnClientDisconnect(Client * c)
{
	Client* pool_begin = gServer.m_ClientPool.Begin();
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (c->uid!=other->uid&& other->IsValid() && !other->m_IsObClient)
		{
			return;

		}
	}
	EndGame();
}

void Game::OnPlayerMove(Client* c)
{
	char *data_start = c->read_position;
	int data_size = c->read_end - c->read_position;
	Core::byte flag=0;
	float time;
	c->ReadByte(flag);
	c->ReadFloat(time);
	float skip_data;
	if (flag&MOVE_FLAG_INPUT_STEER)c->ReadFloat(skip_data);
	if (flag&MOVE_FLAG_INPUT_BRAKE)c->ReadFloat(skip_data);
	if (flag&MOVE_FLAG_INPUT_RUN)c->ReadFloat(skip_data);
	Vector3 pos;
	if (flag & MOVE_FLAG_POSE_POSTION)
	{
		c->ReadVector3(pos);
		c->m_Position = pos;

		//m_DropManager.GetDropItem(pos, c->uid);


		//逆行检查
		Vector3 run_dir = pos - c->m_CheckerPosition;
		float len = Length(run_dir);
		if (len > 10)
		{
			c->m_CheckerPosition = pos;
			Vector3 out_dir;
			bool check_dir = m_RoadCheckerManager.CheckDir(Normalize(run_dir), pos, c->m_RoadCheckerTag == 0 ? 0 : (c->m_LastCheckIndex + 1),out_dir);
			
			Client* pool_begin = gServer.m_ClientPool.Begin();
			for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
			{
				Client* other = pool_begin + i;
				if (other->IsValid())
				{
					other->BeginWrite();
					other->WriteByte(SM_PLSYER_RUN_DIR_ERROR);
					other->WriteUInt(c->uid);
					other->WriteBool(check_dir);
					other->WriteVector3(out_dir);
					int p = c->m_LastCheckIndex * 100 / gConfig.m_CheckerPointCount;
					p = MAX(0, p);
					p = MIN(100, p);
					other->WriteInt(p);
					other->EndWrite();

				}
			}
			
		}




		CheckerPoint p;
		if (m_RoadCheckerManager.Check(pos, p))
		{
			c->m_LastCheckIndex = p.mIndex;
			//log_info("index %d", p.mIndex);
			//碰到最后一个点，检查一圈结束
			if (p.mEndPoint)
			{
				c->m_LastCheckIndex = 0;
				bool finsh = true;
				int untag_count = 0;
				for (int i = 0; i < p.mIndex; i++)
				{
					if (((1 << i)&c->m_RoadCheckerTag) == 0)
					{
						log_info("checker lost %d", i);
						untag_count++;
					}
				}
				//触碰半数以上的点，此圈有效
				if (untag_count < p.mIndex*0.5f)
				{
					c->m_RoadCheckerTag = 0;
					c->m_Complete = true;
					BalanceGame();
				}
				else
				{
					c->m_RoadCheckerTag = 0;
					c->m_LastCheckIndex = 0;
				}
			}
			else
			{
				c->m_RoadCheckerTag |= 1 << p.mIndex;
			}
		}
	}

	Client* pool_begin = gServer.m_ClientPool.Begin();
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid())
		{
			other->BeginWrite();
			other->WriteByte(SM_PLAYER_MOVE);
			other->WriteUInt(c->uid);
			other->WriteData(data_start, data_size);
			other->EndWrite();

		}
	}
}

void Game::OnPlayerStopGame(Client * c)
{
	ClearData();
}

void Game::OnPlayerGetDropItem(Client * c)
{
	Core::uint id;
	c->ReadUInt(id);
	DropItem* item = m_DropManager.GetDropItem(id, true);
	if (item)
	{
		int get_count;
		if (item->mData.mType == DROPITEM_ICON)
		{
			c->m_CoinCount++;
			get_count = c->m_CoinCount;
			log_info("eat coin");
		}
		else
		{
			c->m_CoinCount--;
			c->m_CoinCount = MAX(c->m_CoinCount, 0);
			get_count = c->m_CoinCount;
			log_info("eat OBSTACLE");
		}
		Client* pool_begin = gServer.m_ClientPool.Begin();
		for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
		{
			Client* other = pool_begin + i;

			if (other->IsValid())
			{
				other->BeginWrite();
				other->WriteByte(SM_REMOVE_DROP_ITEM);
				other->WriteUInt(id);
				other->WriteUInt(c->uid);
				other->WriteShort(get_count);
				other->EndWrite();
			}
		}
	}

}


void Game::ClearData()
{
	m_GameState = Wait;
	m_BrithIndex = 0;
}

void Game::EndGame()
{
	if (m_GameState == Wait)return;
	log_info("end game");
	
	ClearData();
	if (gServer.m_ClientPool.Count() > 0)
	{
		Client* pool_begin = gServer.m_ClientPool.Begin();
		for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
		{
			Client* other = pool_begin + i;
			if (other->IsValid())
			{
				other->BeginWrite();
				other->WriteByte(SM_GAME_END);
				other->EndWrite();
				other->connection->Disconnect();

			}
		}
	}

	
}

void Game::Update(float t)
{
	
	if (gServer.m_CheckConnection && gServer.m_ClientPool.Count() == 0)
	{
		m_RestartTimer += t;
		if (m_RestartTimer > 5)
		{
			log_error("server error");
			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, GetCurrentProcessId());
			::TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
		}
	}

	if (m_GameState == SyncScore)
	{
		m_GameRunTime += t;
		if (m_GameRunTime > m_GameTotleTime + gConfig.m_GameBalanceTime)
		{
			EndGame();
		}
	}
	else if (m_GameState == Show)
	{
		m_GameShowTime -= t;
		if (m_GameShowTime < 0)m_GameState = Play;
	}
	if (m_GameState != Play)return;
	m_GameRunTime += t;
	if (m_GameRunTime - m_GameSyncTime > 0.99f)
	{
		m_GameSyncTime = m_GameRunTime;
		SyncGameTime();
	}
	if (m_GameRunTime > m_GameTotleTime)
	{
		BalanceGame();
	}
	m_DropManager.Update(t);
}

void Game::BalanceGame()
{
	if (m_GameState != Play)return;
	m_GameState = Balance;
	log_info("balance game");
	Client* set[10] = {0};
	int count = 0;
	Client* pool_begin = gServer.m_ClientPool.Begin();
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		
		if (other->IsValid()&&!other->m_IsObClient)
		{
			set[count++] = other;
		}
	}
	for (int i = 0; i < count; i++)
	{
		Client* other = set[i];
		other->m_CommitScore = false;
		other->BeginWrite();
		other->WriteByte(SM_GAME_BALANCE);
		other->WriteInt(other->m_CoinCount);
		other->WriteFloat(m_GameRunTime);
		other->EndWrite();
	}
}


void Game::OnRequestLoadGame(Client * c)
{
	if (m_GameState > Wait)
	{
		c->BeginWrite();
		c->WriteByte(SM_GAME_STATE_ERROR);
		c->WriteByte(m_GameState);
		c->EndWrite();
		return;
		
	}
	m_GameState = Load;

	int data_size = c->read_end - c->read_position;
	char* data_start = c->read_position;
	c->m_SetUserInfo = true;
	c->ReadString(c->m_UserOpenId);
	c->ReadString(c->m_UserName);
	c->ReadString(c->m_UserHeadImgUrl);
	c->ReadString(c->m_CarData);
	c->m_DBCoinCount = 0;
	c->m_DBCompleteTime = 0.0f;
	c->m_LastCheckIndex = 0;
	


	Client* pool_begin = gServer.m_ClientPool.Begin();
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid())
		{
			other->m_MapLoaded = false;
			other->BeginWrite();
			other->WriteByte(SM_LOAD_GAME);
			other->WriteUInt(c->uid);
			other->WriteData(data_start, data_size);
			other->EndWrite();


		}
	}
}

void Game::OnClientMapLoaded(Client * c)
{
	c->m_MapLoaded = true;
	Client* pool_begin = gServer.m_ClientPool.Begin();
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid() && !other->m_MapLoaded)return;
	}
	StartGame();
	
}

void Game::StartGame()
{
	if (m_GameState != Load)return;
	m_GameState = Show;
	m_GameTotleTime = gConfig.m_GameTime;
	m_GameShowTime = gConfig.m_GameShowTime;
	m_GameSyncTime = 0;
	m_GameRunTime = 0;
	m_BrithIndex = 0;
	m_DropManager.Init(this);
	m_RoadCheckerManager.Init();
	uint players[10] = { 0 };
	Core::byte count = 0;
	Client* pool_begin = gServer.m_ClientPool.Begin();
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid() && !other->m_IsObClient)
		{
			players[count] = other->uid;
			count++;
			other->m_CoinCount = 0;
			other->m_RoadCheckerTag = 0;
			other->m_Complete = false;
			BrithPose pose;
			gConfig.CopyBrithPose(0, pose);
			other->m_CheckerPosition = pose.position;
			other->m_Position = pose.position;
			other->m_Rotation = pose.rotation;
			other->m_IdlePostion = pose.position;
			other->m_LastCheckIndex = 0;
		}
	}
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* c = pool_begin + i;
		if (c->IsValid())
		{
			c->BeginWrite();
			c->WriteByte(SM_GAME_START);
			c->WriteByte(count);
			for (int j = 0; j < count; j++)
			{
				Client* other = gServer.m_ClientPool.Get(players[j]);
				c->WriteUInt(players[j]);
				c->WriteVector3(other->m_Position);
				c->WriteVector3(other->m_Rotation);

			}
			
			c->WriteFloat(m_GameTotleTime);
			c->WriteFloat(m_GameShowTime);
			c->EndWrite();
		}

	}
}

void Game::SyncGameTime()
{
	Client* pool_begin = gServer.m_ClientPool.Begin();
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid())
		{
			other->BeginWrite();
			other->WriteByte(SM_GAME_SYNC_TIME);
			other->WriteFloat(m_GameSyncTime);
			other->EndWrite();

			
			if(m_GameSyncTime>5){
				float dis = Length(other->m_IdlePostion - other->m_Position);
				other->m_IdlePostion = other->m_Position;
				if (dis < 1)
				{
					Vector3 out_dir;
					m_RoadCheckerManager.GetDir(out_dir, other->m_RoadCheckerTag == 0 ? 1 : (other->m_LastCheckIndex + 1));

					other->BeginWrite();
					other->WriteByte(SM_PLSYER_RUN_DIR_ERROR);
					other->WriteUInt(other->uid);
					other->WriteBool(false);
					other->WriteVector3(out_dir);
					other->WriteInt(other->m_LastCheckIndex * 100 / gConfig.m_CheckerPointCount);
					other->EndWrite();
				}
			}

		}
	}
}

void Game::OnClientCommitSocre(Client * c)
{
	if (m_GameState != Balance)return;
	c->m_CommitScore = true;
	char* start = c->read_position;
	int size = c->read_end - c->read_position;
	Client* pool_begin = gServer.m_ClientPool.Begin();
	/*for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid()&&!other->m_IsObClient&&!other->m_CommitScore)
		{
			return;
		}
	}*/
	m_GameState = SyncScore;
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid())
		{
			other->BeginWrite();
			other->WriteByte(SM_SYNC_RANK_SCORE);
			other->WriteFloat(gConfig.m_GameBalanceTime);
			other->WriteFloat(gConfig.m_GameBalanceTimeForPad);
			other->WriteData(start, size);
			other->EndWrite();
		}
	}
	m_GameRunTime = m_GameTotleTime;
}

void Game::OnDropItemEvent(DropItemEvent event, AliveDroptItem item, void * arg)
{
	if (event == DropItemEvent::DROP_EVENT_CREATE)
	{
		Client* pool_begin = gServer.m_ClientPool.Begin();
		for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
		{
			Client* other = pool_begin + i;
			if (other->IsValid())
			{
				other->BeginWrite();
				other->WriteByte(SM_CREATE_DROP_ITEM);
				other->WriteUInt(item.mID);
				other->WriteByte(gConfig.m_DropItemData[item.mIndex].mType);
				other->WriteInt(item.mPosition.index);
				other->WriteVector3(item.mPosition.pos);
				other->EndWrite();
				log_info("creat drop item %d %d", gConfig.m_DropItemData[item.mIndex].mType, item.mPosition.index);
			}
		}
	}
	else if (event == DropItemEvent::DROP_EVENT_GET)
	{
		
		Core::uint get_uid = 0;
		Core::uint get_count = 0;
		Client* get_client = gServer.m_ClientPool.Get((Core::uint)arg);
		if (get_client&&get_client->IsValid())
		{
			get_uid = get_client->uid;
			if (gConfig.m_DropItemData[item.mIndex].mType == DROPITEM_ICON)
			{
				get_client->m_CoinCount++;
				get_count = get_client->m_CoinCount;
				log_info("eat coin");
			}
			else
			{
				get_client->m_CoinCount--;
				get_client->m_CoinCount = MAX(get_client->m_CoinCount, 0);
				get_count = get_client->m_CoinCount;
				log_info("eat OBSTACLE");
			}
			
		}
		Client* pool_begin = gServer.m_ClientPool.Begin();
		for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
		{
			Client* other = pool_begin + i;

			if (other->IsValid())
			{
				other->BeginWrite();
				other->WriteByte(SM_REMOVE_DROP_ITEM);
				other->WriteUInt(item.mID);
				other->WriteUInt(get_uid);
				other->WriteShort(get_count);
				other->EndWrite();
			}
		}
	}
}
