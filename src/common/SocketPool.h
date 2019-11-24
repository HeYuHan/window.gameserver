#pragma once
#ifndef __SOCKET_POOL_H__
#define __SOCKET_POOL_H__
#include "objectpool.h"
#include "common.h"
#include <event2/bufferevent.h>
#include <vector>
#include <NetworkConnection.h>
#include "ThreadPool2.h"
#include "MessageQueue.h"
#include "Timer.h"
BEGIN_NS_CORE
class SocketPoolClinet;
class SocketPool;
class ISocketClient
{
	friend class SocketPoolClinet;
	friend class SocketPool;
public:
	Core::uint uid;
	bool m_InBackgound;
protected:
	virtual void OnWrite() = 0;
	virtual void OnRevcMessage() = 0;
	virtual void OnDisconnect() = 0;
	virtual void OnConnected() = 0;
	virtual Core::Event* GetEvent() { return NULL; }
	virtual void OnReconnected(SocketPoolClinet*) = 0;
	virtual NetworkStream* GetStream() = 0;
	virtual void OnFreeWait() {}
private:
	bool m_CanWait;
	float m_WaitTime;
	
};
class SocketPool;
class SocketPoolClinet:public NetworkConnection, protected IThreadMessageHadle
{
	friend class ObjectPool<SocketPoolClinet>;
	friend class SocketPool;
public:
	SocketPoolClinet();
private:
	uint uid;
	bool m_IsWebSocket;
	bool m_HandWebSocket;
public:
	int m_Socket;
	bool m_HandShake;
	sockaddr_in m_SockAddr;
	bufferevent *m_BufferEvent;
	ISocketClient *m_Handle;
	SocketPool *m_Pool;
public:
	//virtual int SetEvent(struct event_base *base);
	virtual void Update(float time);
	
	virtual int Read(void* data, int size);
	virtual int Send(void* data, int size);
	virtual void OnWrite();
	virtual void OnConnected();
	virtual void OnDisconnected();
	virtual void Disconnect();
	void DisconnectCanWait();
	virtual void OnReconnected();
	virtual void OnRead();
	virtual void OnThreadMessage(unsigned char id);
	virtual int GetSocket() { return m_Socket; }
	bool IsWebSocket() const { return m_IsWebSocket; }
	
	
public:
	void Init(int fd, sockaddr * sock, SocketPool *pool,bool check_uid,bool use_thread);
private:
	event_base *m_ThreadEventBase;
private:
	static void ReadEvent(bufferevent * bev, void * arg);
	static void WriteEvent(bufferevent * bev, void * arg);
	static void SocketEvent(bufferevent * bev, short events, void * arg);
};
typedef void(*OnAcceptNewClient)(SocketPoolClinet*,void* user_data);
typedef ISocketClient*(*OnRequetReconnect)(Core::uint uid);
class SocketPool
{
	friend class SocketPoolClinet;
public:
	SocketPool();
	~SocketPool();
private:
	static void PoolUpdate(float t, void* arg);
	void Update(float t);
	Timer m_UpdateTimer;
public:
	bool Init(uint size, bool hand_shake_uid, OnAcceptNewClient accept_callback, bool useThread,void* user_data);
	bool Connect(sockaddr *addr,int sock_len);
	void AcceptClient(int fd, sockaddr * sock);
	void RegisterDisconectedClient(ISocketClient *c);
	void UnRegisterDisconectedClient(ISocketClient *c);
private:
	bool FreeOneWaitClient();
public:
	ObjectPool<SocketPoolClinet> m_CachedClients;
	std::vector<ISocketClient*> m_DisconnectedClients;
	OnAcceptNewClient m_OnAcceptNewClient;
	bool m_UseThread;
	bool m_HandShake;
	void* m_UserData;
	
private:
	MutexLock m_Lock;
};

extern SocketPool gSocketPool;
END_NS_CORE
#endif