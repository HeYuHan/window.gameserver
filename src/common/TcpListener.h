#pragma once
#ifndef __TCPLISTENER_H__
#define __TCPLISTENER_H__
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <string>
class TcpListener
{
public:
	
	struct sockaddr_in m_ListenAddr;
	evconnlistener *m_Listener;
	struct event *m_Listener2;
public:
	TcpListener();
	~TcpListener();
public:
	bool CreateTcpServerInAnyAddr(int &port, int max_client, bool inner = true);
	bool CreateTcpServer(const char *addr, int max_client);
	bool CreateTcpServer(const char *ip, int port, int max_client);
	void CloseServer();
	static void ListenEvent(evconnlistener *listener, evutil_socket_t fd, sockaddr *sock, int socklen, void *arg);
	virtual void OnTcpAccept(evutil_socket_t socket, sockaddr*)=0;
private:
	int m_Socket;
};

#endif // !__TCPLISTENER_H__
