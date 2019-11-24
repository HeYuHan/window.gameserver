#pragma once
#ifndef __SERVER_STATE_H__
#define __SERVER_STATE_H__
#include <HttpListenner.h>
#include <MessageQueue.h>
#include <string>
struct evbuffer;
class ServerState:public Core::HttpListenner
{
public:
	ServerState();
	~ServerState();
	
	void Response(Core::HttpTask * task, const char * data);
	void ResponseAIList(const char* data);
private:


	// Í¨¹ý HttpListenner ¼Ì³Ð
	virtual void OnGet(Core::HttpTask * task, const char * path, const char * query);

	virtual void OnPost(Core::HttpTask * task, const char * path, const char * query, struct evbuffer * buffer);
	virtual void OnPut(Core::HttpTask * task, const char * path, const char * query, struct evbuffer * buffer);

};
extern ServerState gServerState;







#endif // !__SERVER_STATE_H__
