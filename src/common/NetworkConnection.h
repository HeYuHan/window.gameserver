#pragma once
#ifndef __NETWORKCONNECTION_H__
#define __NETWORKCONNECTION_H__
#include "common.h"
#include "Vector3.h"
#include "MessageQueue.h"
#include "ThreadPool2.h"
#include <string>
namespace google
{
	namespace protobuf
	{
		class MessageLite;
	}
}
BEGIN_NS_CORE
class NetworkConnection;
typedef enum
{
	TCP_SOCKET=1,UDP_SOCKET,WEB_SOCKET
}SocketType;
typedef enum
{
	READERROR = 0, WRITEERROR = 1
}NETERR;
class NetworkStream:public IThreadMessageHadle
{
public:
	NetworkStream(int send_buff_size = 1024 * 512, int read_buff_size = 1024 * 512);
	~NetworkStream();
	virtual void OnMessage()=0;
	//virtual bool ThreadSafe() = 0;
	void Reset();
	void OnRevcMessage();
	void ParseMessage();
	virtual bool IsValid();
public:
	void WriteBool(bool b);
	void WriteByte(byte data);
	void WriteChar(char data);
	void WriteShort(short data);
	void WriteUShort(ushort data);
	void WriteInt(int data);
	void WriteUInt(uint data);
	void WriteFloat(float data);
	void WriteLong(int64 data);
	void WriteULong(uint64 data);
	void WriteString(const char* str);
	void WriteData(const void* data, int count);
	void WriteVector3(Vector3 &v3);
	void WriteShortQuaternion(Quaternion &rot);
	bool WriteProtoBufferAutoSize(google::protobuf::MessageLite *message);
	bool WriteProtoBuffer(google::protobuf::MessageLite *message);
	
	virtual void BeginWrite();
	virtual void EndWrite();
	void WriteInputToOut();
	//////////////////////////////////////////////////////////////
	//read data
public:
	void ReadByte(byte &data);
	void ReadByte(char &data);
	void ReadShort(short &data);
	void ReadUShort(ushort &data);
	void ReadInt(int &data);
	void ReadUInt(uint &data);
	void ReadFloat(float &data);
	void ReadLong(int64 &data);
	void ReadULong(uint64 &data);
	void ReadBool(bool & data);
	int ReadString(char* str, int size);
	int ReadString(std::string &s);
	void ReadData(void* data, int count);
	void ReadVector3(Vector3 &v3);
	void ReadShortQuaternion(Quaternion &rot);
	bool ReadProtoBuffer(google::protobuf::MessageLite *message,int size);
	bool ReadProtoBufferAutoSize(google::protobuf::MessageLite *message);
private:
	bool Resize(bool revc_buff);
public:
	NetworkConnection* connection;
public:
	char* write_buff;
	char* write_position;
	char* write_end;
	char* write_buff_end;

	char* read_offset;
	char* read_buff;
	char* read_end;
	char* read_position;
	char* read_buff_end;
private:
	char* web_frame;
	// Í¨¹ý IThreadMessageHadle ¼Ì³Ð
	virtual void OnThreadMessage(unsigned char id) override;
	//ThreadObject m_ReadLock;
	//MutexLock m_WriteLock;
};


class NetworkConnection
{
public:
	NetworkConnection();
	~NetworkConnection();
	//virtual int SetEvent(struct event_base *) { return -1; }
	virtual void Update(float time) = 0;
	virtual int Read(void* data, int size) = 0;
	virtual int Send(void* data, int size) = 0;
	virtual void OnConnected() = 0;
	virtual void OnDisconnected() = 0;
	virtual void Disconnect()=0;
	virtual int GetSocket() { return -1; }
	
public:
	NetworkStream* stream;
	SocketType m_Type;

};
END_NS_CORE
#endif
