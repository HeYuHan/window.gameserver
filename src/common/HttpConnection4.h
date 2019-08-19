#pragma once
#include<string>
#include <event2/http.h>
#ifndef __HTTP_CONNECTION4_H__
#define __HTTP_CONNECTION4_H__
#include<HTTPConnection2.h>
#include<TCPInterface.h>
#include <RakPeerInterface.h>
#include <GetTime.h>

class HttpRequest
{
public:
	enum
	{
		GET,POST
	};
public:
	HttpRequest();
	~HttpRequest();
	bool MakeRequest(int type, const char *url, const char *data, const char* dataType = 0);
	int GetResponse(std::string & ret);
	
private:
	RakNet::HTTPConnection2 *httpConnection2;
	RakNet::TCPInterface *tcp;
	RakNet::RakString rsRequest;
};

class UriParser
{
public:
	UriParser();
	~UriParser();
	bool Parse(const char* url);
	char* Decode(const char* url);
private:
	void Clean();
	
public:
	int port;
	char host[64];
	char* m_DecodeData;

};

int HttpGet(const char* url, std::string &ret);

int HttpPost(const char * url, const char* data, const char* contentType,std::string & content);
#endif
