#pragma once
#ifndef WINDOWS_SAMPLE_RELEASE
#ifndef __REDIS_HELPER_H__
#define __REDIS_HELPER_H__
#define REDIS_SORT_STRING_TAG "|"
#include <functional>
namespace google
{
	namespace protobuf
	{
		class Message;
	}
}
struct redisContext;
struct redisReply;
struct redisAsyncContext;
class RedisAsyncCallback;
class RedisHelper;
typedef  unsigned long long redis_int;
class RedisResponse
{
	friend class RedisAsyncCallback;
	friend class RedisHelper;
public:
	RedisResponse();
	~RedisResponse();
	bool Valid();
	int Type();
	int Length();
	size_t Integer();
	const char* String();
	int ElementsCount();
	bool Proto(google::protobuf::Message &message);
	bool Element(RedisResponse *res, int index);
private:
	void Free();
private:
	redisReply* m_Reply;
	bool isChild;
};
class RedisAsyncCallback
{
public:
	static void OnRedisMessage(struct redisAsyncContext*, void* reply, void* arg);
	static void OnRedisConnect(const struct redisAsyncContext*, int status);
	static void OnRedisDisconnect(const struct redisAsyncContext*, int status);
	virtual void OnMessage(RedisResponse *res) = 0;
	virtual void OnConnect(int status) = 0;
	virtual void OnDisconnect(int status) = 0;
	RedisHelper *m_Helper;
};
enum
{
	REDIS_CMD_HASH = 1, REDIS_CMD_STRING = 2, REDIS_CMD_SORT_STRING = 3
};
enum
{
	REDIS_EVENT_CONNECT=0,REDIS_EVENT_DISCONNECT=1
};

class RedisHelper
{
public:
	RedisHelper();
	~RedisHelper();

public:
	bool Connnect(const char* addr);
	bool AsyncConnect(const char* addr, RedisAsyncCallback * callback);
	void SetAsyncCallback(RedisAsyncCallback* callback);
	void Close();

	void AsyncScript(RedisAsyncCallback * callback, const char* script);
	void AsyncHaveKey(RedisAsyncCallback * callback, int cmd, const char* mapName, const char* key);
	void AsyncDelKey(RedisAsyncCallback * callback, int cmd, const char* mapName, const char* key);
	void AsyncDelKey(RedisAsyncCallback * callback, int cmd, const char* mapName, size_t key);
	void AsyncSaveStringValue(RedisAsyncCallback * callback, int cmd, const char* mapName, const char* key, const char * value);
	void AsyncSaveValue(RedisAsyncCallback * callback, int cmd, const char* mapName, const char* key, const char * value, redis_int len);
	void AsyncSaveValue(RedisAsyncCallback * callback, int cmd, const char* mapName, size_t key, google::protobuf::Message& message);
	void AsyncGetValue(RedisAsyncCallback * callback, int cmd, const char* mapName, const char* key);
	void AsyncGetValue(RedisAsyncCallback * callback, int cmd, const char* mapName, size_t key);
	void AsyncSaveSortValue(RedisAsyncCallback * callback, const char* mapName, const char* key, redis_int score);
	void AsyncGetSortValue(RedisAsyncCallback * callback, const char* mapName, redis_int start, redis_int end);
	void AsyncCountSortValue(RedisAsyncCallback * callback, const char* mapName, redis_int start, redis_int end);
	void AsyncExpireKey(RedisAsyncCallback * callback, int cmd, const char* mapName, const char* key, redis_int s);

	bool ScriptCmd(RedisResponse *res, const char* cmd, const char* script);
	bool ScriptEval(RedisResponse *res,const char* cmd, const char* script,int argc,const char* arg);
	bool HaveKey(int cmd, const char* mapName, const char* key);
	bool DelKey(int cmd, const char* mapName, const char* key);
	bool DelKey(int cmd, const char* mapName, size_t key);

	bool SaveValue(int cmd, RedisResponse *res, const char* mapName, const char* key, size_t value);
	bool SaveValue(int cmd, RedisResponse *res, const char* mapName, size_t key, size_t value);

	bool SaveValue(int cmd, RedisResponse *res, const char* mapName, const char* key, const char * value, redis_int len);
	bool SaveValue(int cmd, RedisResponse *res, const char* mapName, size_t key, const char * value, redis_int len);
	bool SaveValue(int cmd, RedisResponse *res, const char* mapName, const char* key, google::protobuf::Message& message);
	bool SaveValue(int cmd, RedisResponse *res, const char* mapName, size_t key, google::protobuf::Message& message);
	bool SaveStringValue(int cmd, RedisResponse *res, const char* mapName, const char* key, const char * value);
	bool GetValue(int cmd, RedisResponse *res, const char* mapName, const char* key);
	bool GetValue(int cmd, RedisResponse *res, const char* mapName, size_t key);
	bool HgetAll(RedisResponse *res, const char* mapName);
	bool SaveSortValue(RedisResponse *res, const char* mapName, const char* key, redis_int score);
	bool GetSortValue(RedisResponse *res, const char* mapName, redis_int start, redis_int end);
	int CountSortValue(const char* mapName, redis_int start, redis_int end);
	bool ExpireKey(int cmd, const char* mapName, const char* key, int s);
	bool Keys(RedisResponse *res, const char* mapName, const char* key);

private:
	void Command(RedisResponse *res, const char *format, ...);
	void AsyncCommand(RedisAsyncCallback * callback, const char *format, ...);
private:
	redisContext* m_Context;
	redisAsyncContext* m_AsyncContext;
public:
	std::function<void(RedisHelper*,int)> m_OnEvent;
};

#endif // !__REDIS_HELPER_H__
#endif
