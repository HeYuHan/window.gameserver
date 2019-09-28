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
	case CM_PLAYER_AVATAR_DATA:
		OnBroadcastAvatarData(c);
		break;
	case CM_SELECT_GAME_MAP:
		OnRequestSelectMap(c);
		break;
	case CM_PLAYER_MOVE:
		OnPlayerMove(c);
		break;
	case CM_REQUEST_LOAD_MAP:
		OnRequestLoadMap(c);
		break;
	case CM_GAME_MAP_LOADED:
		OnClientMapLoaded(c);
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

void Game::OnBroadcastAvatarData(Client * c)
{
	
	int data_size = c->read_end - c->read_position;
	char* data_start = c->read_position;
	c->ReadString(c->m_UserInfo.m_Account);
	c->ReadString(c->m_UserInfo.m_Name);
	c->ReadString(c->m_UserInfo.m_UserData);
	c->m_UserInfo.m_Password = "empty";
	c->m_DBCoinCount = 0;
	c->m_DBCompleteTime = 0.0f;
	log_info("%s join game", c->m_UserInfo.m_Account.c_str());
	User temp = c->m_UserInfo;
	if (0 != gServer.m_LocalDBHelper.GetUserByAccount(temp))
	{
		Json::Reader reader;
		Json::Value root;
		reader.parse(c->m_UserInfo.m_UserData, root);
		root["score"] = 0;
		root["time"] = 0.0f;
		Json::FastWriter writer;
		c->m_UserInfo.m_UserData = writer.write(root);
		gServer.m_LocalDBHelper.AddUser(c->m_UserInfo);
	}
	else
	{
		c->m_UserInfo.m_ID = temp.m_ID;
		Json::Reader reader;
		Json::Value root;
		reader.parse(temp.m_UserData, root);
		if (!root["score"].isNull())
		{
			c->m_DBCoinCount = root["score"].asInt();
		}
		if (!root["time"].isNull())
		{
			c->m_DBCompleteTime = root["time"].asFloat();
		}
		reader = Json::Reader();
		root = Json::Value();
		reader.parse(c->m_UserInfo.m_UserData, root);
		root["score"] = c->m_DBCoinCount;
		root["time"] = c->m_DBCompleteTime;
		Json::FastWriter writer;
		c->m_UserInfo.m_UserData = writer.write(root);
		gServer.m_LocalDBHelper.UpdateUser(c->m_UserInfo);
	}
	Client* pool_begin = gServer.m_ClientPool.Begin();
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid()&&!other->uid !=c->uid)
		{
			if (!c->m_IsObClient)
			{
				other->BeginWrite();
				other->WriteByte(SM_PLAYER_AVATAR_DATA);
				other->WriteUInt(c->uid);
				other->WriteData(data_start, data_size);
				other->EndWrite();
			}

			if (!other->m_IsObClient)
			{
				c->BeginWrite();
				c->WriteByte(SM_PLAYER_AVATAR_DATA);
				c->WriteUInt(other->uid);
				c->WriteString(other->m_UserInfo.m_Account.c_str());
				c->WriteString(other->m_UserInfo.m_Name.c_str());
				c->WriteString(other->m_UserInfo.m_UserData.c_str());
				c->EndWrite();
			}


		}
	}
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
		m_DropManager.GetDropItem(pos, c->uid);


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
					other->WriteInt(c->m_LastCheckIndex * 100 / gConfig.m_CheckerPointCount);
					other->EndWrite();

				}
			}
			
		}




		CheckerPoint p;
		if (m_RoadCheckerManager.Check(pos, p))
		{
			c->m_LastCheckIndex = p.mIndex;
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


void Game::ClearData()
{
	m_GameState = Wait;
	m_MapDataLen = 0;
	m_BrithIndex = 0;
}

void Game::EndGame()
{
	if (m_GameState == Wait)return;
	log_info("end game");
	
	ClearData();
	Client* pool_begin = gServer.m_ClientPool.Begin();
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid())
		{
			other->connection->Disconnect();

		}
	}
	
}

void Game::Update(float t)
{
	if (m_GameState == Balance)
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
		if (m_GameShowTime < 0)StartGame();
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
	Client* pool_begin = gServer.m_ClientPool.Begin();
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid())
		{
			if (!other->m_IsObClient)
			{
				Json::Reader reader;
				Json::Value root;
				reader.parse(other->m_UserInfo.m_UserData, root);
				//if (other->m_CoinCount > other->m_DBCoinCount)
				{
					
					root["score"] = other->m_CoinCount;
					
				}
				//if (other->m_Complete && m_GameRunTime < other->m_DBCompleteTime)
				{
					root["time"] = other->m_Complete?m_GameRunTime:-1.0f;
				}
				Json::FastWriter writer;
				other->m_UserInfo.m_UserData = writer.write(root);
				gServer.m_LocalDBHelper.UpdateUser(other->m_UserInfo);
			}

		}
	}
	std::vector<User> db_users;
	gServer.m_LocalDBHelper.GetAllUser(db_users);
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid())
		{
			other->BeginWrite();
			other->WriteByte(SM_GAME_BALANCE);

			if (!other->m_IsObClient)
			{
				other->WriteInt(other->m_CoinCount);
				//other->WriteInt(other->m_DBCoinCount);
				other->WriteFloat(other->m_Complete ? m_GameRunTime : -1.0f);
			}
			Core::byte count = MIN(db_users.size(), 10);
			other->WriteByte(count);
			for (int j = 0; j < count; j++)
			{
				other->WriteString(db_users[j].m_Account.c_str());
				other->WriteString(db_users[j].m_Name.c_str());
				other->WriteString(db_users[j].m_UserData.c_str());

			}
			other->EndWrite();

		}
	}
}

void Game::OnRequestSelectMap(Client* c)
{
	if (m_GameState != Wait)return;
	int data_size = c->read_end - c->read_position;
	if (data_size > MAX_MAP_DATA_LEN)
	{
		log_error("client map data size too more");
		c->connection->Disconnect();
		return;
	}
	m_MapDataLen = data_size;
	memcpy(m_MapData, c->read_position, m_MapDataLen);
	Client* pool_begin = gServer.m_ClientPool.Begin();
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid() )
		{
			other->BeginWrite();
			other->WriteByte(SM_SELECT_GAME_MAP);
			other->WriteData(m_MapData, m_MapDataLen);
			other->EndWrite();

		}
	}
}

void Game::OnRequestLoadMap(Client * c)
{
	if (m_GameState > Load)
	{
		c->BeginWrite();
		c->WriteByte(SM_GAME_STATE_ERROR);
		c->WriteByte(m_GameState);
		c->EndWrite();
		return;
		
	}
	m_GameState = Load;
	char map_name[256] = { 0 };
	c->ReadString(map_name, 256);
	Client* pool_begin = gServer.m_ClientPool.Begin();
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid())
		{
			other->m_MapLoaded = false;

			other->BeginWrite();
			other->WriteByte(SM_LAOD_GAME_MAP);
			other->WriteString(map_name);
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
	m_GameState = Show;
	m_GameShowTime = gConfig.m_GameShowTime;

	
}

void Game::StartGame()
{
	if (m_GameState != Show)return;
	m_GameState = Play;
	m_GameTotleTime = gConfig.m_GameTime;
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
			gConfig.CopyBrithPose(m_BrithIndex++, pose);
			other->m_CheckerPosition = pose.position;
			other->m_Position = pose.position;
			other->m_Rotation = pose.rotation;
			other->m_IdlePostion = pose.position;
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

			
			/*{
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
			}*/

		}
	}
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
				other->WriteVector3(item.mPosition);
				other->EndWrite();
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
			get_client->m_CoinCount++;
			get_count = get_client->m_CoinCount;
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
