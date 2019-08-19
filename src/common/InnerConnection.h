#pragma once
#ifndef __INNER_CONNECTION_H__
#define __INNER_CONNECTION_H__
#include "common.h"
#include "TcpListener.h"
#include "NetworkConnection.h"
#include "TcpConnection.h"
#include <functional>
#include <vector>
BEGIN_NS_CORE

#define MAX_INNER_SERVER_COUNT 128
class InnerConnectionListenner;

class InnerConnectionClient:public TcpConnection,public NetworkStream
{
	friend class InnerConnectionListenner;


public:
	InnerConnectionClient();
	~InnerConnectionClient();
	
	
protected:
	// 通过 TcpConnection 继承
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;


	// 通过 NetworkStream 继承
	virtual void OnMessage() override;
	//virtual bool ThreadSafe() override;
public:
	Core::uint uid;
private:
	int id;
	bool is_server;
	InnerConnectionListenner* m_Listenner;

};



class InnerConnectionListenner :public TcpListener
{
	friend class InnerConnectionClient;
public:
	enum EventType
	{
		ON_CONNECTED,ON_MESSAGE,ON_DISCONNECTED
	};
public:
	typedef std::function<void(EventType,int, InnerConnectionClient*)> InnerConnectionEvent;


	

	

public:
	InnerConnectionListenner();
	~InnerConnectionListenner();
public:
	InnerConnectionClient m_Client[MAX_INNER_SERVER_COUNT];
	// 通过 TcpListener 继承
	virtual void OnTcpAccept(evutil_socket_t socket, sockaddr *) override;
private:
	InnerConnectionClient* GetEmptyClient();
	void FireEvent(EventType type, InnerConnectionClient* stream);
	std::vector<InnerConnectionEvent> m_EventListenners;


public:
	void AddListenner(InnerConnectionEvent callback);
	void EachOnlineClient(std::function<void(NetworkStream*)> call_back);
	void EachOnlineClient(std::function<void(NetworkStream*)> call_back, int id);

};

extern InnerConnectionListenner gInnerConnectionListenner;






END_NS_CORE






#endif