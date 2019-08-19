#pragma once
#ifndef __FILE_READER_H__
#define __FILE_READER_H__

#include <string>
#include <queue>
#include <fstream>
#include<json/json.h>
#include<ThreadPool2.h>
#include <common.h>
BEGIN_NS_CORE
bool ReadText(std::string &ret,const std::string path);
bool WriteText(const std::string text,const std::string path);
bool ReadJson(Json::Value &root, const char* path);
#define ASYNC_FILE_BUFF_LEN 1024*512
class AsyncFileWriter
{
public:
	AsyncFileWriter();
	~AsyncFileWriter();
	bool Create(const std::string &path);
	int PushContent(const char* content);
	
	void Write();
	void Close();
private:
	static void* WriteThread(void* arg);
public:
	uint uid;
private:
	MutexLock mutex;
	ThreadCondition m_ContentFull;
	ThreadCondition m_HaveContent;
	char m_ContentQueue[ASYNC_FILE_BUFF_LEN];
	std::ofstream m_File;
	bool m_CreateStream;
	bool m_CreateThread;
	int m_WritePosition;
	pthread_t m_ThreadID;
};
END_NS_CORE
#endif // !__FILE_READER_H__
