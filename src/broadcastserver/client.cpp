
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
	gGame.OnClientMessage(this);

}

void Client::OnConnected()
{
	m_ProfileLength = -1;
	Reset();
	BeginWrite();
	WriteUInt(uid);
	EndWrite();
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
	return connection != NULL && (m_ProfileLength>0 || m_IsObClient);
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
