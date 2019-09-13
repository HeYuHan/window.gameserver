
#include "client.h"
#include <log.h>
#include <tools.h>
#include <NetworkConnection.h>
#include "game.h"
#include "server.h"

USING_NS_CORE
Client::Client():NetworkStream(1024*1024*2, 1024 * 1024 * 2), m_TcpConnection(this), m_UdpConnection(this)
{
}

Client::~Client()
{
}

void Client::OnMessage()
{
	if (!this->m_RevciveUID)
	{
		this->m_RevciveUID = true;
	}
	else
	{
		gGame.OnClientMessage(this);
	}
	

}

void Client::OnConnected()
{
	m_UserInfo = Core::LocalUserDatabaseHelper::SampleUser();
	Reset();
	m_RevciveUID = false;
	connection->Send(&uid,sizeof(Core::uint));
	m_IsObClient = false;


}

void Client::OnDisconnected()
{
	gGame.OnClientDisconnect(this);
	gServer.m_ClientPool.Free(uid);
	this->connection = NULL;
}

uint64_t Client::GetGUID()
{
	return m_ConnectionType == TCP_SOCKET ? uid : m_UdpGUID;
}

bool Client::IsValid()
{
	return connection != NULL && (!m_UserInfo.m_Account.empty() || m_IsObClient);
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
