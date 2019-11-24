#pragma once
#include <event2/util.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <functional>

#ifndef __TIMER_H__
#define __TIMER_H__
typedef  std::function<void(float, void*)> TimerCallBack;
class Timer
{
public:
	Timer();
	~Timer();
	void Init(float time, std::function<void(float,void*)> call_back, void* arg, bool loop = false);
	void ActiveTimer();
	void Reset();
	void Begin();
	void Stop();
	static int Loop();
	static void ExitLoop();
	static struct event_base* GetEventBase();
private:
	static void timeout_cb(evutil_socket_t fd, short event, void *arg);
	
public:
	struct timeval m_LastTime;
	bool m_Loop;
	bool m_Stop;
	bool m_Run;
	float m_Time;
	void* m_Arg;
	std::function<void(float, void*)> m_CallBack;
	struct event *m_TimerEvent;
	struct timeval m_Tv;
	event_base *m_EventBase;
};
float DiffTime(timeval &v1, timeval &v2);
#endif // !__TIMER_H__

