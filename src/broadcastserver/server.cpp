#include "server.h"
#include <log.h>
#include <NetworkConnection.h>
#include <Timer.h>
#include <unordered_map>
#include <vector>
using namespace std;
USING_NS_CORE
typedef unordered_map<uint64_t, Client*> UdpClientMap;
typedef unordered_map<uint64_t, Client*>::iterator UdpClientIterator;
typedef pair<uint64_t, Client*> UdpClientMapPair;
UdpClientMap m_UdpClientMap;
Timer m_UpdateTimer;
Server gServer;
Server::Server()
{
}

Server::~Server()
{
}

void Server::OnUdpAccept(Packet * p)
{
	Client* client = m_ClientPool.Allocate();
	if (!client)
	{
		log_error("cant allocate new client pool size:%d", m_ClientPool.Size());
		return;
	}
	client->connection = &client->m_UdpConnection;
	client->m_UdpConnection.stream = client;
	client->m_UdpGUID = p->guid.g;
	client->m_Heart = 0;
	client->m_ConnectionType = client->connection->m_Type;
	m_UdpClientMap.insert(UdpClientMapPair(p->guid.g, client));
	log_info("clinet connect %s", p->systemAddress.ToString(true));
	client->m_UdpConnection.InitServerSocket(UdpListener::m_Socket, p->systemAddress);
	
	
}

void Server::OnUdpClientMessage(Packet * p)
{
	UdpClientIterator it = m_UdpClientMap.find(p->guid.g);
	if (it != m_UdpClientMap.end())
	{
		Client *client = it->second;
		if (client)
		{
			client->m_Heart = 0;
			client->m_UdpConnection.m_MessagePacket = p;
			client->OnRevcMessage();
		}

	}
}

void Server::OnUdpClientDisconnected(Packet * p)
{
	UdpClientIterator it = m_UdpClientMap.find(p->guid.g);
	if (it != m_UdpClientMap.end())
	{
		if (it->second)
		{
			//log_error("client disconnect uid %d", it->second->uid);
			log_info("clinet disconnect %s", p->systemAddress.ToString(true));
			it->second->m_UdpConnection.Disconnect();
		}
		else
		{
			m_UdpClientMap.erase(it);
			UdpListener::m_Socket->CloseConnection(p->systemAddress, true, HIGH_PRIORITY);
		}
	}
}

void Server::OnKeepAlive(Packet * p)
{
	UdpClientIterator it = m_UdpClientMap.find(p->guid.g);
	if (it != m_UdpClientMap.end())
	{
		Client *client = it->second;
		if (client)
		{
			client->m_Heart = 0;
		}

	}
}

void Server::OnTcpAccept(evutil_socket_t socket, sockaddr *addr)
{
	Client* client = m_ClientPool.Allocate();
	if (!client)
	{
		log_error("cant allocate new client pool size:%d", m_ClientPool.Size());
		return;
	}
	client->connection = &client->m_TcpConnection;
	client->m_ConnectionType = client->connection->m_Type;
	client->m_TcpConnection.stream = client;

	client->m_TcpConnection.InitSocket(socket, addr, Timer::GetEventBase());

}
void ServerUpdate(float t, void*)
{
	gServer.Update();
	for (UdpClientIterator it = m_UdpClientMap.begin(); it != m_UdpClientMap.end();)
	{
		if (it->second)
		{
			it->second->m_Heart += t;
			if (it->second->m_Heart > gServer.m_HeartTime)
			{
				log_warn("client kepp alive time out %d", it->second->m_UdpGUID);
				it->second->m_UdpConnection.Disconnect();
				it = m_UdpClientMap.erase(it);
			}
			else
			{
				it++;
			}
			
		}
		else
		{
			it = m_UdpClientMap.erase(it);
		}
	}

}
bool Server::Init()
{
	if (BaseServer::Init())
	{
		do
		{
			if (m_MaxClient == 0)
			{
				log_error("Max Client Is Zero");
				break;
			}
			if (!m_TcpAddr && !m_UdpAddr)
			{
				log_error("have no tcp addr or udp addr");
				break;
			}
			if (!m_ClientPool.Initialize(m_MaxClient))
			{
				log_error("cant init client pool memery too low %d", m_MaxClient);
				break;
			}
			if (m_TcpAddr && !CreateTcpServer(m_TcpAddr, m_MaxClient))
			{
				log_error("cant create tcp server %s", m_TcpAddr);
				break;
			}
			if (m_UdpAddr && !CreateUdpServer(m_UdpAddr, m_UdpPwd, m_MaxClient))
			{
				log_error("cant create udp server %s", m_UdpPwd);
				break;
			}
			else
			{
				log_info("udp server run in %s pwd %s", m_UdpAddr, m_UdpPwd);
			}
			m_UpdateTimer.Init(0, ServerUpdate, NULL, true);
			m_UpdateTimer.Begin();
			return true;
		} while (false);
		
	}
	return false;
}
int Server::Run()
{
	if (Init())
	{
		return Timer::Loop();
	}
	return -1;
}
