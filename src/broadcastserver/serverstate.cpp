#include <common.h>
#include "server.h"
#include "serverstate.h"
#include <string>
#include <json/json.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/buffer.h>
#include <InnerConnection.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <io.h>
#include "game.h"
#include "game_config.h"
ServerState gServerState;
USING_NS_CORE;
void ResponseJson(HttpTask * task, Json::Value &ret);
using namespace Core;
using namespace std;
ServerState::ServerState()
{
}

ServerState::~ServerState()
{
}

void ServerState::OnGet(HttpTask * task, const char * path, const char * query)
{
	std::string requt_path(path);
	if (requt_path.find("/getconfig") == 0)
	{
		Response(task, gConfig.m_ClientConfig.c_str());
	}
	if (requt_path.find("/getfile") == 0)
	{
		std::string path = requt_path.substr(9);
		if (path.empty())
		{
			HttpListenner::EndWrite(task->request, HTTP_OK, "ARG ERROR");
			return;
		}
		const char* file_path = AutoFilePath(path.c_str());
		if (file_path == NULL)
		{
			HttpListenner::EndWrite(task->request, HTTP_NOTFOUND, "FILE NOT FOUND");
			log_error("file not found: %s", path.c_str());
			return;
		}
		FILE *fd = fopen(file_path, "rb");
		const int buff_size = 1024 * 10;
		char data[buff_size];
		while (!feof(fd))
		{
			int count = fread(data, sizeof(char), buff_size, fd);
			if (count <= 0)break;
			struct evbuffer *buffer = evhttp_request_get_output_buffer(task->request);
			evbuffer_add(buffer, data, count);
		}
		std::fclose(fd);
		"application/x-www-form-urlencoded";
		struct evkeyvalq *header = evhttp_request_get_output_headers(task->request);
		evhttp_add_header(header, "Access-Control-Allow-Origin", "*");
		evhttp_add_header(header, "Content-Type", "application/x-www-form-urlencoded");
		HttpListenner::EndWrite(task->request, HTTP_OK, "ok");
		//bool is_jpeg = path.find(".jpg") != std::string::npos;
		
	}
	else if (requt_path.find("/getstate") == 0)
	{
		char msg[128] = { 0 };
		sprintf(msg, "%d", gGame.m_GameState);
		Response(task, msg);
	}
	else
	{
		HttpListenner::EndWrite(task->request, HTTP_NOTFOUND, "NOT FOUND");
	}
}

void ServerState::OnPost(HttpTask * task, const char * path, const char * query, struct evbuffer * buffer)
{
	//std::string requt_path(path);
	//if (requt_path.find("/save") == 0)
	//{
	//	std::string path = requt_path.substr(6);
	//	if (path.empty())
	//	{
	//		HttpListenner::EndWrite(task->request, HTTP_OK, "ARG ERROR");
	//		return;
	//	}
	//	int last = path.find_last_of("/");
	//	if (last != std::string::npos && last != 0)
	//	{
	//		std::string dir = path.substr(last);
	//		string commond = "md " + dir;         //md后面有空格
	//		if (_access(dir.c_str(), 0) == -1)   //判断目录是否存在 -1即表示不存在
	//		{
	//			system(commond.c_str());
	//		}
	//		evbuffer_iovec io;
	//		evbuffer_peek(buffer, -1, NULL, &io, 1);
	//		if (io.iov_len == 0)
	//		{
	//			HttpListenner::EndWrite(task->request, HTTP_OK, "FILE SIZE IS ZERO");
	//			return;
	//		}
	//		FILE *fd = fopen(path.c_str(), "wb");
	//		fwrite(io.iov_base, sizeof(char), io.iov_len, fd);
	//		fclose(fd);
	//		char ret[128] = { 0 };
	//		sprintf(ret, "write path: %s size %d", path.c_str(), io.iov_len);
	//		log_info(ret);
	//		HttpListenner::EndWrite(task->request, HTTP_OK, ret);
	//	}
	//}
}
std::string replace_str(std::string& str, const std::string& to_replaced, const std::string& newchars)
{
	for (string::size_type pos(0); pos != string::npos; pos += newchars.length())
	{
		pos = str.find(to_replaced, pos);
		if (pos != string::npos)
			str.replace(pos, to_replaced.length(), newchars);
		else
			break;
	}
	return   str;
}
void ServerState::OnPut(HttpTask * task, const char * path, const char * query, struct evbuffer * buffer)
{
	std::string requt_path(path);
	if (requt_path.find("/putfile") == 0)
	{
		std::string path = requt_path.substr(8);
		if (path.empty())
		{
			HttpListenner::EndWrite(task->request, HTTP_OK, "ARG ERROR");
			return;
		}
		path = replace_str(path, "/", "\\");
		int last = path.find_last_of("\\");
		
		if (last != std::string::npos && last != 0)
		{
			std::string dir = path.substr(1,last-1);
			string commond = "md " + dir;         //md后面有空格
			
			if (_access(dir.c_str(), 0) == -1)   //判断目录是否存在 -1即表示不存在
			{
				log_info("create dir %s", commond.c_str());
				system(commond.c_str());
			}
			
		}

		FILE *fd = fopen(path.substr(1).c_str(), "wb+");
		if (fd == NULL)
		{
			log_info("cant open file %s", path.c_str());
			HttpListenner::EndWrite(task->request, HTTP_OK, "WRITE ERROR");
			return;
		}

		const int read_buf_size = 1024 * 10;
		char read_buf[read_buf_size];
		int read_size = 0;
		int write_size = 0;
		do
		{
			read_size = evbuffer_remove(buffer, read_buf, read_buf_size);
			if (read_size > 0)
			{
				fwrite(read_buf, sizeof(char), read_size, fd);
				write_size += read_size;
			}

		} while (read_size > 0);


		std::fclose(fd);
		char ret[128] = { 0 };
		sprintf(ret, "write path: %s size %d", path.c_str(), write_size);
		log_info(ret);
		HttpListenner::EndWrite(task->request, HTTP_OK, ret);
	}
}
void ServerState::Response(HttpTask * task, const char * data)
{
	struct evkeyvalq *header = evhttp_request_get_output_headers(task->request);
	evhttp_add_header(header, "Access-Control-Allow-Origin", "*");
	evhttp_add_header(header, "Content-Type", "application/json");

	HttpListenner::WriteData(task->request, data);
	HttpListenner::EndWrite(task->request, HTTP_OK, "ok");
}

void ResponseJson(HttpTask * task, Json::Value &ret)
{
	Json::FastWriter writer;
	gServerState.Response(task, writer.write(ret).c_str());
}

