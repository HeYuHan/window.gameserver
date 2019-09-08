#pragma once
#ifndef __ILUA_SCRIPT_H__
#define __ILUA_SCRIPT_H__

extern "C"
{
#include "lua/lua.h"
#include "lua/lualib.h" 
#include "lua/lauxlib.h" 
}
#include "LuaBridge/LuaBridge.h"
#include "log.h"
#include "common.h"
#include <functional>
#define BEGIN_REG_TYPE(__TYPE__)\
struct REG_CLASS##__TYPE__ { \
typedef __TYPE__ REG_TYPE; \
REG_CLASS##__TYPE__(lua_State* L) { \
	luabridge::getGlobalNamespace(L) \
	.beginClass<__TYPE__>(#__TYPE__)

#define BEGIN_REG_PROTO_TYPE(__TYPE__)\
struct REG_CLASS##__TYPE__ { \
typedef __TYPE__ REG_TYPE; \
static REG_TYPE Create(){ \
	REG_TYPE ret; return ret;\
}\
REG_CLASS##__TYPE__(lua_State* L) { \
	auto reg = luabridge::getGlobalNamespace(L) \
	.beginClass<__TYPE__>(#__TYPE__) \
	.addStaticFunction("Create",&Create);

#define BEGIN_REG_DERIVE_TYPE(__TYPE__,__DERIVE__)\
struct REG_CLASS##__TYPE__ { \
typedef __TYPE__ REG_TYPE; \
REG_CLASS##__TYPE__(lua_State* L) { \
	luabridge::getGlobalNamespace(L) \
	.deriveClass<__TYPE__,__DERIVE__>(#__TYPE__)
	

#define REG_FUNC(__NAME__) .addFunction(#__NAME__,&REG_TYPE::__NAME__)
#define REG_CFUNC(__NAME__) .addCFunction(#__NAME__,&REG_TYPE::__NAME__)
#define REG_DATA(__NAME__) .addData(#__NAME__,&REG_TYPE::__NAME__)
#define REG_CTOR(__FUNC__) .addConstructor<__FUNC__>()
#define REG_PROTO_GET_SET(__NAME__) .addProperty (#__NAME__, &REG_TYPE::__NAME__, &REG_TYPE::set_##__NAME__)

#define REG_PRO_AUTO(__NAME__) .addProperty(#__NAME__,&REG_TYPE::Get##__NAME__,&REG_TYPE::Set##__NAME__)
#define REG_PRO(__NAME__,__GET__,__SET__) .addProperty(#__NAME__,&REG_TYPE::__GET__,&REG_TYPE::__SET__)
#define END_REG_TYPE(__TYPE__,__L__) \
	.endClass();\
	}}REG_##__TYPE__##_temp(__L__);

#define END_REG_PROTO_TYPE(__TYPE__,__L__) \
	reg.endClass();\
	}}REG_##__TYPE__##_temp(__L__);



BEGIN_NS_CORE
typedef int(*register_lua_model)(lua_State* L);
class LuaInterface
{
	friend class LuaEngine;
public:
	LuaInterface();
	~LuaInterface();
protected:
	void FreeRef();
public:
	virtual const char* GetRequireScript() { return NULL; };
	virtual const char* GetNativeTypeName() = 0;
	virtual const char* GetScriptTypeName() { return NULL; };
public:
	int m_LuaRef;
	int m_TableRef;
	lua_State *L;
};
template<typename T>
inline bool CallLuaOnEnter(T native)
{
	if (native->m_LuaRef && native->m_TableRef && native->L)
	{
		lua_rawgeti(native->L, LUA_REGISTRYINDEX, native->m_TableRef);
		lua_rawgeti(native->L, LUA_REGISTRYINDEX, native->m_LuaRef);
		lua_pushvalue(native->L, -2);
		luabridge::push(native->L, native);
		int ret = _lua_call(native->L, 2, 0, 0);
		return ret ==0;
	}
	return false;
}

class LuaEngine
{
	friend class LuaInterface;
	friend struct LuaObject;
public:
	~LuaEngine();
public:
	void RegisterModel(register_lua_model model);
	void RegisterOnError(std::function<void(const char*)> call);
	void Start(const char* mainLua=0);
	bool DoFile(const char* file_path);
	bool DoString(const char* str);

	bool CreateScriptHandle(LuaInterface *lua);
	void DestoryScriptHandle(LuaInterface *lua);
	void LuaSearchPath(const char *name, const char *value);
	lua_State* GetState();
	luabridge::LuaRef GetRef(const char* key);
	luabridge::LuaRef GetRef(int index);

	template<typename T>
	T Get(int index);

	template<typename T>
	T Get(const char* key);

	template<typename T>
	T GetGlobal(const char* key);

	template<typename T>
	bool DoString(const char* script, T *native, const char* name = NULL);

	template<typename T>
	bool DoFile(const char* path, T *native, const char* name = NULL);


	template<typename T>
	void Set(const char* key,T navtive);

	template<typename T>
	bool ApplyTable(const char* table, T *native);


	static LuaEngine* GetInstance();
	static lua_State* NewState();
private:
	void Pop(int old_top,const char* msg=0);
	void OnError(const char* error);
private:
	lua_State* L;
	static LuaEngine* m_Instance;
	
};

template<class T>
T LuaEngine::Get(int index)
{
	return luabridge::Stack<T>::get(this->L, index);
}

template<class T>
T LuaEngine::GetGlobal(const char* key)
{
	 int top = lua_gettop(this->L);
	 lua_getglobal(this->L, key);
	 T ret = luabridge::Stack<T>::get(this->L, 1);
	 lua_settop(this->L,top);
	 return ret;
}

template<typename T>
void LuaEngine::Set(const char* key, T native)
{
	luabridge::push<T>(this->L, native);
	lua_setfield(this->L, -2, key);
}
extern int bind_native_func_script_reg;
extern int bind_natvie_func_file_reg;
extern int bind_natvie_func_script_table_reg;

template<typename T>
bool LuaEngine::DoString(const char* script, T *native,const char* name)
{
	
	if (script == NULL || native == NULL)return false;
	int top = lua_gettop(this->L);
	lua_rawgeti(L, LUA_REGISTRYINDEX, bind_native_func_script_reg);
	luabridge::push<T*>(L, native);
	lua_pushstring(L, script);
	lua_pushstring(L, name ? name : "self");
	int error = lua_pcall(L, 3, 0, 0);
	if (error)
	{
		log_error("error:%s\nscript:%s", lua_tostring(L, -1),script);
		lua_settop(this->L, top);
		return false;
	}
	lua_settop(this->L, top);
	return true;
}
template<typename T>
bool LuaEngine::DoFile(const char* path, T *native, const char* name)
{

	if (path == NULL || native == NULL)return false;
	int top = lua_gettop(this->L);
	lua_rawgeti(L, LUA_REGISTRYINDEX, bind_natvie_func_file_reg);
	luabridge::push<T*>(L, native);
	lua_pushstring(L, path);
	lua_pushstring(L, name ? name : "self");
	int error = lua_pcall(L, 3, 0, 0);
	if (error)
	{
		log_error("%s", lua_tostring(L, -1));
		lua_settop(this->L, top);
		return false;
	}
	lua_settop(this->L, top);
	return true;
}

template<typename T>
bool LuaEngine::ApplyTable(const char* table, T *native)
{
	if (table == NULL || native == NULL)return false;
	int top = lua_gettop(this->L);
	lua_rawgeti(L, LUA_REGISTRYINDEX, bind_natvie_func_script_table_reg);
	luabridge::push<T*>(L, native);
	lua_pushstring(L, table);
	int error = lua_pcall(L, 2, 0, 0);
	if (error)
	{
		log_error("%s", lua_tostring(L, -1));
		lua_settop(this->L, top);
		return false;
	}
	lua_settop(this->L, top);
	return true;
}
END_NS_CORE
#endif // !__ILUA_SCRIPT_H__



