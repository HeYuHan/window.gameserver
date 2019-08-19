#pragma once
#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__
#include "common.h"
struct bufferevent;
BEGIN_NS_CORE
class IThreadMessageHadle
{
	friend class MainThreadMessageQueue;
protected:
	virtual void OnThreadMessage(unsigned char id)=0;
public:
	void SendMessageToMainThread(unsigned char id);
public:
	~IThreadMessageHadle();
	IThreadMessageHadle();
};
class MainThreadMessageQueue
{
	friend class IThreadMessageHadle;
private:
	void RegisterHandle(IThreadMessageHadle *handle, unsigned char id);
public:
	static void Init();
private:
	static void QueueRead(bufferevent * bev, void * arg);
	static void QueueWrite(bufferevent * bev, void * arg);
	static void QueueError(bufferevent * bev, short events, void * arg);
	static void InnerInit();
};

END_NS_CORE



#endif // !__MESSAGE_QUEUE_H__
