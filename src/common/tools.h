#pragma once
#include <json/json.h>
#include <event2/event.h>
#include <vector>
#include <string>
#ifndef __TOOLS_H__
#define __TOOLS_H__

#define FOR_EACH_LIST(_Type_,_List_,_Iter_) \
	std::vector<_Type_*>::iterator iter##_Iter_; \
	for(iter##_Iter_ = _List_.begin(); iter##_Iter_ !=_List_.end(); iter##_Iter_++)

#define FOR_EACH_LIST_STRUCT(__TYPE__,__LIST__,__ITER__) \
	std::vector<__TYPE__>::iterator iter##__ITER__; \
	for( iter##__ITER__ =  __LIST__.begin(); iter##__ITER__ != __LIST__.end(); iter##__ITER__++)

/*
** return a random integer in the interval
** [a, b]
*/
int RandomRange(int a, int b);
bool ParseJsonValue(Json::Value json, const char* key, int &value);
bool ParseJsonValue(Json::Value json, const char* key, float &value);
bool ParseJsonValue(Json::Value json, const char* key, bool &value);
bool ParseJsonValue(Json::Value json, const char* key, char* str, int len);
bool ParseJsonValue(Json::Value json, const char* key, std::string &ret);
bool ParseJsonArray(Json::Value json, const char* key, std::vector<int> &ret,bool clear=true);
bool ParseJsonArray(Json::Value json, const char* key, std::vector<float> &ret, bool clear = true);
bool ParseJsonArray(Json::Value json, const char* key, std::vector<std::string> &ret, bool clear = true);


bool ParseSockAddr(sockaddr_in & addr, const char * str, bool by_name=false);
bool RunAsDaemon();

bool CheckRedis(bool if_not_create = false);

void CaculateSha1(const char* text, char* out);
float DiffTime(timeval &v1, timeval &v2);
unsigned long long PthreadSelf();
bool IsMainThread();
void ThreadSleep(unsigned int s);
#ifdef _WIN32
void print_stack_windows();
void catch_stack_windows(bool print=false);
#define print_stack print_stack_windows
#define catch_stack catch_stack_windows
#else
void print_stack_linux();
void catch_stack_linux(bool print = false);
#define print_stack print_stack_linux
#define catch_stack catch_stack_linux
#endif // _WIN32

void print_last_stack();

std::string addr_to_ip(sockaddr_in &addr);

std::string UrlEncode(const char* str);
std::string UrlDecode(const char* str);
bool CreateProcessX(const char* exe_path, char** arg);
void SetSocketAliveAndLinger(int fd);
int64_t string_to_timestep(const char* str, const char* format);
#endif // !__TOOLS_H__
