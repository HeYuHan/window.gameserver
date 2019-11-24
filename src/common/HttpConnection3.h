#pragma once
#include <functional>
#include <event2/http.h>
#include <functional>
#ifndef __HTTP_CONNECTION3_H__
#define __HTTP_CONNECTION3_H__
class HttpResponse
{
public:
	HttpResponse();
	
	//virtual void OnResponse() = 0;
	int GetBufferLength();
	int ReadBuffer(void *data, int size);
	int GetState();
	void* GetUserData();
	bool IsSet();
	virtual struct event_base* GetEventBase();
public:
	struct evhttp_connection* connection;
	struct evhttp_request* request;
	int state;
	void* user_data;
};
struct RequestPack;
class HttpManager
{
public:
	bool Proxy(const char* url, evhttp_request* request);
	bool Request(const char* url, const char* data, const char* content_type, int port, void* user_data,int flag, std::function<void(HttpResponse*)> func);
	bool Get(const char* url, int port, void* user_data,std::function<void(HttpResponse*)> func);
	bool Get(const char* url, std::function<void(HttpResponse*)> func = NULL);
	bool Post(const char* url, const char* data,const char* content_type,int port, void* user_data,std::function<void(HttpResponse*)> func);
	bool Post(const char* url, const char* data, const char* content_type, std::function<void(HttpResponse*)> func = NULL);
	void SetInterface(RequestPack *http);

};
extern HttpManager gHttpManager;

#endif // !__HTTP_CONNECTION_H__
