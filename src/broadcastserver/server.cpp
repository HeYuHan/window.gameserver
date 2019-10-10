#include "server.h"
#include "game.h"
#include "game_config.h"
#include <log.h>
#include <NetworkConnection.h>
#include <Timer.h>
#include <LuaEngine.h>
#include <unordered_map>
#include <properties.h>

#include <vector>
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <tchar.h>
#include <Psapi.h>
#include <conio.h>
#include <ThreadEventPool.h>
#include "serverstate.h"
#pragma comment (lib,"Psapi.lib")
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
	gGame.Update(t);
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
			AutoFilePath("./config");
			AutoFilePath("../exe");
			AutoFilePath("../exe/config");
			LuaEngine::GetInstance()->Start();
			LuaEngine::GetInstance()->LuaSearchPath("path", "./../src/script/?.lua");
			LuaEngine::GetInstance()->LuaSearchPath("path", "./../script/?.lua");
			LuaEngine::GetInstance()->LuaSearchPath("path", "./script/?.lua");
			if (!gConfig.Init())
			{
				log_error("cant init game config");
				break;
			}
			//check process
			{
				HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
				if (INVALID_HANDLE_VALUE == hSnapshot)
				{
					return NULL;
				}
				PROCESSENTRY32 pe = { 0 };
				pe.dwSize = sizeof(PROCESSENTRY32);
				DWORD processId = GetCurrentProcessId();
				for (bool fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
				{
					if (pe.th32ProcessID == processId)continue;
					char szProcessName[MAX_PATH] = { 0 };
					HANDLE        hProcess;
					hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pe.th32ProcessID);
					if (hProcess && GetProcessImageFileName(hProcess, szProcessName, MAX_PATH))
					{
						std::string str(szProcessName);
						if (str.find("broadcastserver") != std::string::npos)
						{
							log_info("kill %s", szProcessName);
							TerminateProcess(hProcess, 0);
							CloseHandle(hProcess);
							//break;
						}
						else if (gConfig.m_ClientPath && str.find(gConfig.m_ClientPath) != std::string::npos)
						{
							log_info("kill %s", szProcessName);
							TerminateProcess(hProcess, 0);
							bool b = CloseHandle(hProcess);
							//break;
						}
					}
				}
			}
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
			if (m_HttpAddr && !gServerState.CreateHttpServer(m_HttpAddr, 10, 2))
			{
				log_error("cant create http server %s", m_HttpAddr);
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
			//m_LocalDBHelper.OpenDatabase("user.db");
			m_UpdateTimer.Init(0, ServerUpdate, NULL, true);
			m_UpdateTimer.Begin();
			
			if (gConfig.m_ClientPath > 0)
			{
				char process_path[MAX_PATH] = { 0 };
				int port_tcp = -1;
				int prot_udp = -1;
				int prot_http = -1;
				if (m_TcpAddr)
				{
					char address[256];
					const char * port_start = strchr(m_TcpAddr, ':');
					if (port_start)
					{
						memcpy(address, m_TcpAddr, port_start - m_TcpAddr);
						address[port_start - m_TcpAddr] = 0;
						port_tcp = atoi(port_start + 1);
					}
				}
				if (m_UdpAddr)
				{
					char address[256];
					const char * port_start = strchr(m_UdpAddr, ':');
					if (port_start)
					{
						memcpy(address, m_UdpAddr, port_start - m_UdpAddr);
						address[port_start - m_UdpAddr] = 0;
						prot_udp = atoi(port_start + 1);
					}
				}
				if (m_HttpAddr)
				{
					char address[256];
					const char * port_start = strchr(m_HttpAddr, ':');
					if (port_start)
					{
						memcpy(address, m_HttpAddr, port_start - m_HttpAddr);
						address[port_start - m_HttpAddr] = 0;
						prot_http = atoi(port_start + 1);
					}
				}
				sprintf(process_path,"%s -RunWithServer -PortTcp=%d -PortUdp=%d -PortHttp=%d %s", gConfig.m_ClientPath, port_tcp, prot_udp,gConfig.m_ClientExeArg);
				log_info("start client %s", process_path);
				STARTUPINFO start_info;
				PROCESS_INFORMATION process_info;
				ZeroMemory(&start_info, sizeof(start_info));
				ZeroMemory(&process_info, sizeof(process_info));
				if (CreateProcess(NULL, process_path, NULL, NULL, false, 0, NULL, NULL, &start_info, &process_info))
				{
					CloseHandle(process_info.hThread);
					CloseHandle(process_info.hProcess);
				}
			}
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
