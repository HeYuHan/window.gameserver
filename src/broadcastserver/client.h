#pragma once
#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <objectpool.h>
#include <NetworkConnection.h>
#include <UdpConnection.h>
#include <TcpConnection.h>
#define MAX_PROFILE_LEN 1024*512

class Server;
class Client;

class TcpImp :public Core::TcpConnection
{
public:
	TcpImp(Client* c);
	// ͨ�� TcpConnection �̳�
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
private:
	Client* m_Client;
};
class UdpImp :public Core::UdpConnection
{
public:
	UdpImp(Client* c);
	// ͨ�� UdpConnection �̳�
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
	bool m_IsObClient;
	bool m_MapLoaded;
	int m_CoinCount;

public:
	void OnMessage();
	void OnConnected();
	void OnDisconnected();
	uint64_t GetGUID();
	virtual bool IsValid();
};


#endif // !__CLIENT_H__