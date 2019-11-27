#pragma once
#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <objectpool.h>
#include <NetworkConnection.h>
#include <UdpConnection.h>
#include <TcpConnection.h>
#include <LocalUserDatabaseHelper.h>

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
	std::string m_UserOpenId;
	std::string m_UserName;
	std::string m_UserHeadImgUrl;
	std::string m_CarData;
	bool m_SetUserInfo;
	//Core::LocalUserDatabaseHelper::SampleUser m_UserInfo;
	float m_Heart;
	bool m_IsObClient;
	bool m_CommitScore;
	bool m_MapLoaded;
	bool m_RevciveUID;
	bool m_Complete;
	float m_AliveTime;
	float m_SendAliveTime;
	int m_CoinCount;
	int m_DBCoinCount;
	float m_DBCompleteTime;
	Core::uint64 m_RoadCheckerTag;
	Core::uint64 m_LastCheckIndex;
	Vector3 m_CheckerPosition;
	Vector3 m_Position;
	Vector3 m_Rotation;
	Vector3 m_IdlePostion;


public:
	void OnMessage();
	void OnConnected();
	void OnDisconnected();
	uint64_t GetGUID();
	virtual bool IsValid();
};


#endif // !__CLIENT_H__
