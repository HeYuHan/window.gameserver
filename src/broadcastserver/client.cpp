
#include "client.h"
#include <log.h>
#include <tools.h>
#include "server.h"
#include <NetworkConnection.h>
USING_NS_CORE
Client::Client():NetworkStream(1024*1024*2, 1024 * 1024 * 2), m_TcpConnection(this), m_UdpConnection(this)
{
}

Client::~Client()
{
}

void Client::OnMessage()
{
	if (m_ProfileLength < 0)
	{
		m_ProfileLength = MIN(read_end - read_position, MAX_PROFILE_LEN);
		ReadData(m_Profile, m_ProfileLength);
		for (Core::uint i = 0; i < gServer.m_ClientPool.Size(); i++)
		{
			Client* c = gServer.m_ClientPool.Begin() + i;
			if (c->connection && c->m_ProfileLength>0)
			{
				if (c->uid != uid)
				{
					this->BeginWrite();
					this->WriteByte(CONNECTED);
					this->WriteUInt(c->uid);
					this->WriteData(c->m_Profile, c->m_ProfileLength);
					this->EndWrite();

					c->BeginWrite();
					c->WriteByte(CONNECTED);
					c->WriteUInt(this->uid);
					c->WriteData(this->m_Profile, this->m_ProfileLength);
					c->EndWrite();
				}
			}
		}
		return;
	}
	Core::uint to_uid = 0;
	ReadUInt(to_uid);
	if (to_uid > 0)
	{
		Client* c = gServer.m_ClientPool.Get(to_uid);
		if (c->connection && c->m_ProfileLength>0)
		{
			c->BeginWrite();
			c->WriteByte(MESSAGE);
			c->WriteUInt(this->uid);
			c->WriteData(read_position, read_end - read_position);
			c->EndWrite();
		}
	}
	else
	{
		for (Core::uint i = 0; i < gServer.m_ClientPool.Size(); i++)
		{
			Client* c = gServer.m_ClientPool.Begin() + i;
			if (c->connection && c->m_ProfileLength>0)
			{
				if (c->uid != uid)
				{
					c->BeginWrite();
					c->WriteByte(MESSAGE);
					c->WriteUInt(this->uid);
					c->WriteData(read_position, read_end - read_position);
					c->EndWrite();
				}
			}
		}
	}

}

void Client::OnConnected()
{
	m_ProfileLength = -1;
	Reset();
	BeginWrite();
	WriteUInt(uid);
	EndWrite();



}

void Client::OnDisconnected()
{
	this->connection = NULL;
	for (Core::uint i = 0; i < gServer.m_ClientPool.Size(); i++)
	{
		Client* c = gServer.m_ClientPool.Begin() + i;
		if (c->connection && c->m_ProfileLength>0)
		{
			if (c->GetGUID() != GetGUID())
			{
				c->BeginWrite();
				c->WriteByte(DISCONNECTED);
				c->WriteUInt(uid);
				c->EndWrite();
			}
		}
	}
	gServer.m_ClientPool.Free(uid);
}

uint64_t Client::GetGUID()
{
	return m_ConnectionType == TCP_SOCKET ? uid : m_UdpGUID;
}



TcpImp::TcpImp(Client * c):m_Client(c)
{
	m_Type = TCP_SOCKET;
}

void TcpImp::OnConnected()
{
	m_Client->OnConnected();
}

void TcpImp::OnDisconnected()
{
	m_Client->OnDisconnected();
}

UdpImp::UdpImp(Client * c) :m_Client(c)
{
	m_Type = UDP_SOCKET;
}

void UdpImp::OnConnected()
{
	m_Client->OnConnected();
}

void UdpImp::OnDisconnected()
{
	m_Client->OnDisconnected();
}
