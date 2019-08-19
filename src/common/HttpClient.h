#pragma once
#ifndef WINDOWS_SAMPLE_RELEASE
#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__
#include "common.h"
#include <curl/curl.h>
#include <string>
#include <functional>
#include <vector>
#include <Timer.h>
BEGIN_NS_CORE
class CurlRequestManager;
class HttpClient
{
	friend class CurlRequestManager;
	enum 
	{
		HTTP_CLIENT_GET,
		HTTP_CLIENT_POST
	};
public:
	HttpClient();
	~HttpClient();
public:

	bool Post(const char* url, const char* data, const char* contentType = 0);
	bool Get(const char* url);
	bool Request(const char* host, int method, const char **header, int header_length, const char* query, const char * data,unsigned int timeOut=0,bool aync=false);
	bool AsyncGet(const char* url, unsigned int timeOut = 0);
	bool AsyncPost(const char* url, const char* data, const char* contentType = 0, unsigned int timeOut = 0);
	bool AsyncPost(const char * url, const char * data, const char ** header, int head_length, unsigned int timeOut);

private:
	bool Update();

private:
	static int WriteCallback(void* buffer, size_t size, size_t nmemb, void* content);
	
public:
	const char* m_Error;
	int m_ResultCode;
	std::string m_Content;
	unsigned int uid;
private:
	CURL *curl;
	std::function<void(HttpClient* http)> callBack;
	void* userData;
	CURLM* multi;
	int still_running;
	
};

class CurlRequestManager
{
public:
	bool Post(const char* url, const char* data, const char* contentType,unsigned int timeOut, unsigned int uid,std::function<void(HttpClient* http)> callBack);
	bool PostHeader(const char * url, const char * data, const char ** header, int head_length, unsigned int timeOut, unsigned int uid, std::function<void(HttpClient*http)> callBack);
	bool Get(const char* url,unsigned int uid, unsigned int timeOut,std::function<void(HttpClient* http)> callBack);
private:
	std::vector<HttpClient*> m_Request;
	Timer m_UpdateTimer;
	static void Update(float t, void *arg);
	static void Init_Curl_Manager();
};
extern CurlRequestManager gCUrlManager;


END_NS_CORE
#endif
#endif