#pragma once
#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <objectpool.h>
#include <NetworkConnection.h>
#include <UdpConnection.h>
#include <TcpConnection.h>
#define MAX_PROFILE_LEN 1024*512

enum SYNC_MESSAGE_TYPE
{
	CONNECTED=1,DISCONNECTED,MESSAGE
};







class Server;
class Client;

class TcpImp :public Core::TcpConnection
{
public:
	TcpImp(Client* c);
	// 通过 TcpConnection 继承
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
private:
	Client* m_Client;
};
class UdpImp :public Core::UdpConnection
{
public:
	UdpImp(Client* c);
	// 通过 UdpConnection 继承
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
private:
	Client* m_Client;
};


class Client:public Core::NetworkStream
{
	friend class Core::ObjectPool<Client>;
	friend class Server;
public:
	Client();
	~Client();

public:
	Core::uint uid;
	uint64_t m_UdpGUID;
	Core::byte m_ConnectionType;
	TcpImp m_TcpConnection;
	UdpImp m_UdpConnection;
	char m_Profile[MAX_PROFILE_LEN];
	int m_ProfileLength;

	float m_Heart;

public:
	void OnMessage();
	void OnConnected();
	void OnDisconnected();
	uint64_t GetGUID();

};


#endif // !__CLIENT_H__
