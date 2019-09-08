#include "game.h"
#include "server.h"
#include "game_config.h"
#include <properties.h>
using namespace Core;
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
		break;
	case CM_PLAYER_AVATAR_DATA:
		OnBroadcastAvatarData(c);
		break;
	case CM_SELECT_GAME_MAP:
		OnRequestSelectMap(c);
		break;
	default:
		break;
	}
}

void Game::OnClientDisconnect(Client * c)
{
}

void Game::OnBroadcastAvatarData(Client * c)
{
	
	int data_size = c->read_end - c->read_position;
	if (data_size > MAX_PROFILE_LEN)
	{
		log_error("client avatar data size too more");
		c->connection->Disconnect();
		return;
	}
	memcpy(c->m_Profile, c->read_position, data_size);
	c->m_ProfileLength = data_size;
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
				other->WriteData(c->read_position, data_size);
				other->EndWrite();
			}


			c->BeginWrite();
			c->WriteByte(SM_PLAYER_AVATAR_DATA);
			c->WriteUInt(other->uid);
			c->WriteData(other->read_position, data_size);
			c->EndWrite();

		}
	}
}

void Game::ClearData()
{
	m_GameState = Wait;
	m_MapDataLen = 0;
}

void Game::EndGame()
{
	ClearData();
}

void Game::Update(float t)
{
	if (m_GameState != Play)return;
	m_GameRunTime += t;
	if (m_GameRunTime - m_GameSyncTime > 0.99f)
	{
		m_GameSyncTime = m_GameRunTime;
		SyncGameTime();
	}
	m_DropManager.Update(t);
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

	c->m_ProfileLength = data_size;
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
	m_GameState = Load;
	Client* pool_begin = gServer.m_ClientPool.Begin();
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid())
		{
			other->m_MapLoaded = false;

			other->BeginWrite();
			other->WriteByte(SM_LAOD_GAME_MAP);
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
	m_GameState = Play;
	m_GameTotleTime = gConfig.m_GameTime;
	m_GameSyncTime = 0;
	m_GameRunTime = 0;
	m_DropManager.Init(this);
	for (int i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* other = pool_begin + i;
		if (other->IsValid())
		{
			other->m_CoinCount = 0;







			other->BeginWrite();
			other->WriteByte(SM_GAME_START);
			other->WriteFloat(m_GameTotleTime);
			other->EndWrite();
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
	else if (event == DropItemEvent::DROP_EVENT_CREATE)
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
