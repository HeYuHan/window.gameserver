#pragma once
#include "LuaEngine.h"
#ifndef __LUA_EXPORT_H__
#define __LUA_EXPORT_H__
#include <Vector3.h>
#include <google/protobuf/message.h>
template <typename T>
int register_networkstream(lua_State *L)
{
#define EXPORT_LUA_STREAM_READ(__TYPE__,__FUNC__) \
	__TYPE__ __FUNC__() {\
	__TYPE__ ret=0;\
		m_Stream->__FUNC__(ret);\
	return ret;}

#define EXPORT_LUA_STREAM_WRITE(__TYPE__,__FUNC__) \
	void __FUNC__(__TYPE__ value) { m_Stream->__FUNC__(value); }

#define EXPORT_LUA_STREAM(__TYPE__,__FUNC__) EXPORT_LUA_STREAM_READ(__TYPE__,Read##__FUNC__) EXPORT_LUA_STREAM_WRITE(__TYPE__,Write##__FUNC__)
	
	
#define REG_STREAM_FUNC(__NAME__) REG_FUNC(Read##__NAME__) REG_FUNC(Write##__NAME__)


	USING_NS_CORE
	struct Reg_Stream
	{
		T* m_Stream;
		EXPORT_LUA_STREAM(Core::byte, Byte)
		EXPORT_LUA_STREAM(Core::int32, Int)
		EXPORT_LUA_STREAM(Core::uint, UInt)
		EXPORT_LUA_STREAM(Core::uint64, ULong)
		EXPORT_LUA_STREAM(Core::int64, Long)
		EXPORT_LUA_STREAM(Core::int16, Short)
		EXPORT_LUA_STREAM(Core::uint16, UShort)
		EXPORT_LUA_STREAM(float, Float)
		EXPORT_LUA_STREAM(bool, Bool)
		EXPORT_LUA_STREAM(Vector3,Vector3)





		static Reg_Stream* BeginWrite(T* native)
		{
			Reg_Stream* handle = new Reg_Stream(native);
			native->BeginWrite();
			return handle;
			
		}
		static Reg_Stream* GetStream(T* native)
		{
			Reg_Stream* handle = new Reg_Stream(native);
			return handle;

		}
		static void ReleaseStream(Reg_Stream* handle)
		{
			handle->m_Stream = NULL;
			delete handle;
		}
		static void EndWrite(Reg_Stream* handle)
		{
			if (handle->m_Stream)handle->m_Stream->EndWrite();
			handle->m_Stream = NULL;
			delete handle;
		}
		void BeginWriteX()
		{
			m_Stream->BeginWrite();
		}
		void EndWriteX()
		{
			m_Stream->EndWrite();
		}
		std::string ReadString()
		{
			
			int ret = 0;
			m_Stream->ReadInt(ret);
			char* read_pos = m_Stream->read_position;
			char b = read_pos[ret];
			read_pos[ret] = 0;
			std::string str(read_pos);
			m_Stream->read_position += ret;
			read_pos[ret] = b;
			return str;
		}
		void WriteString(const char* str)
		{
			m_Stream->WriteString(str);
		}
		Reg_Stream(T* native):m_Stream(NULL)
		{
			m_Stream = native;
		}
		static Reg_Stream Create(T* native)
		{
			return Reg_Stream(native);
		}
		void operator = (const Reg_Stream & v)
		{
			m_Stream = v.m_Stream;
		}
		
	};



	BEGIN_REG_TYPE(Reg_Stream)
		REG_STREAM_FUNC(Int)
		REG_STREAM_FUNC(UInt)
		REG_STREAM_FUNC(Byte)
		REG_STREAM_FUNC(Short)
		REG_STREAM_FUNC(UShort)
		REG_STREAM_FUNC(Long)
		REG_STREAM_FUNC(ULong)
		REG_STREAM_FUNC(Bool)
		REG_STREAM_FUNC(Float)
		REG_STREAM_FUNC(Vector3)
		REG_STREAM_FUNC(String)
		.addFunction("BeginWrite", &Reg_Stream::BeginWriteX)
		.addFunction("EndWrite", &Reg_Stream::EndWriteX)
		END_REG_TYPE(Reg_Stream, L)



		luabridge::getGlobalNamespace(L)
		.addFunction("BeginWrite", &Reg_Stream::BeginWrite)
		.addFunction("EndWrite", &Reg_Stream::EndWrite)
		.addFunction("GetStream", &Reg_Stream::GetStream)
		.addFunction("ReleaseStream", &Reg_Stream::ReleaseStream)
		.addFunction("CreateStream", &Reg_Stream::Create);



	return 0;
}






#endif // !__LUA_EXPORT_H__
