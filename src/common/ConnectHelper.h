#pragma once
#ifndef __CONNECT_HELPER_H__
#define __CONNECT_HELPER_H__
#include "ThreadPool2.h"
#include "common.h"
#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#endif // WIN32
#include <vector>

BEGIN_NS_CORE
class ConnectHelper;
class ConnectResult
{
	friend class ConnectTask;
	friend class ConnectHelper;

public:
	int user_set;
	int result_fd;
	sockaddr_in addr_sock;
	char addr_ip_port[64];
public:
	ConnectResult();
	bool Connected();
	bool Connecting();
public:
	bool connected;
	bool connecting;
};

class ConnectHelper
{
	friend class ConnectTask;

public:
	ConnectHelper();
	~ConnectHelper();
public:
	void Add(ConnectResult *result);
	void Init();
private:
	ConnectResult* GetWaitAddr();
private:
	MutexLock m_Lock;
	ThreadCondition m_Empty;
	bool m_Inited;
	std::queue<ConnectResult*> m_WaitConnectList;
};
extern ConnectHelper gConnectHelper;
END_NS_CORE
#endif