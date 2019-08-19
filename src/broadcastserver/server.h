#pragma once
#ifndef __SERVER_H__
#define __SERVER_H__
#include <common.h>
#include <UdpListener.h>
#include <TcpListener.h>
#include <BaseServer.h>
#include "client.h"

class Server:public UdpListener,public TcpListener,public BaseServer
{
public:
	Server();
	~Server();

private:


	// 通过 UdpListener 继承
	virtual void OnUdpAccept(Packet * p) override;

	virtual void OnUdpClientMessage(Packet * p) override;

	virtual void OnUdpClientDisconnected(Packet * p) override;

	virtual void OnKeepAlive(Packet * p) override;


	// 通过 TcpListener 继承
	virtual void OnTcpAccept(evutil_socket_t socket, sockaddr *) override;

public:
	Core::ObjectPool<Client> m_ClientPool;
	const char* m_TcpAddr;
	const char* m_UdpAddr;
	const char* m_UdpPwd;
	int m_MaxClient;
	int m_HeartTime;
	virtual bool Init();
	virtual int Run();
};

extern Server gServer;






#endif // !__SERVER_H__
