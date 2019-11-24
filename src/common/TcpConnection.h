#pragma once
#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__
#include "NetworkConnection.h"
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include "MessageQueue.h"
BEGIN_NS_CORE
class TcpConnection:public NetworkConnection, public IThreadMessageHadle
{
public:
	TcpConnection();
	~TcpConnection();
	virtual void Update(float time);
	virtual int Read(void* data, int size);
	virtual int Send(void* data, int size);
	void InitSocket(evutil_socket_t socket, sockaddr* addr, struct event_base *base);
	bool Connect(const char* ip, int port, event_base * base);
	bool Connect(const char* addr, event_base * base);
	bool Reconnect(event_base * base);
	Event* GetEventBase();
	virtual void Disconnect();
	void HandShake();
private:
	void HandShakeUID();
protected:
	virtual void OnThreadMessage(unsigned char id) override;
private:
	static void ReadEvent(bufferevent *bev, void *arg);
	static void WriteEvent(bufferevent *bev, void *arg);
	static void SocketEvent(bufferevent *bev, short events, void *arg);
public:
	int m_Socket;
	sockaddr_in m_Addr;
	bufferevent *m_BufferEvent;

	bool m_HandShake;
	Core::uint m_HandUid;
	
	bool m_NeedColse;
	
};
END_NS_CORE

#endif // !__TCPCONNECTION_H__

