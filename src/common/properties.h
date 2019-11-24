#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <map>
#include <cstdlib>
#include "common.h"
BEGIN_NS_CORE

#define P_U32(__KEY__) Properties::Instance()->GetAsUint32(__KEY__)
#define P_U64(__KEY__) Properties::Instance()->GetAsUint64(__KEY__)
#define P_STR(__KEY__) Properties::Instance()->GetAsCStr(__KEY__)
#define P_F32(__KEY__) Properties::Instance()->GetAsF64(__KEY__)
#define P_F64(__KEY__) Properties::Instance()->GetAsF64(__KEY__)
#define P_BOOL(__KEY__) Properties::Instance()->GetAsBool(__KEY__)

class Properties
{
	public:

		static Properties *Instance();

		bool Load(const char *file, bool clear = true);

		virtual const char * GetAsCStr(const char *key, const char *def = "")
		{
			if (const char *value = Get(key))
			{
				return value;
			}
			return def;
		}
		const char * GetAsCStr(const std::string &key, const char *def = "")
		{
			return GetAsCStr(key.c_str(), def);
		}
		bool GetAsStrArray(const char *key, std::vector<std::string> &ret);

		virtual uint32 GetAsUint32(const char *key, uint32 def = 0)
		{
			return GetAsType(properties_uint32_cache, key, def);
		}
		uint32 GetAsUint32(const std::string &key, uint32 def = 0)
		{
			return GetAsUint32(key.c_str(), def);
		}
		virtual int32 GetAsInt32(const char *key, int32 def = 0)
		{
			return GetAsType(properties_uint32_cache, key, def);
		}
		int32 GetAsInt32(const std::string &key, int32 def = 0)
		{
			return GetAsInt32(key.c_str(), def);
		}
		virtual uint32 GetAsUint64(const char *key, uint64 def = 0)
		{
			return GetAsType(properties_uint64_cache, key, def);
		}
		uint32 GetAsUint64(const std::string &key, uint64 def = 0)
		{
			return GetAsUint64(key.c_str(), def);
		}
		virtual int64 GetAsInt64(const char *key, int64 def = 0)
		{
			return GetAsType(properties_uint64_cache, key, def);
		}
		int64 GetAsInt64(const std::string &key, int64 def = 0)
		{
			return GetAsInt64(key.c_str(), def);
		}
		std::string UpdateKey(const char* key, const char* value)
		{
			PropsMap::iterator iter = properties.find(key);
			if (iter != properties.end())
			{
				std::string origin = iter->second;
				properties[key] = value;
				properties_uint32_cache.erase(key);
				properties_uint64_cache.erase(key);
				properties_double_cache.erase(key);
				return origin;
			}
			return std::string();
		}
		virtual double GetAsF64(const char *key, double def = 0.0f)
		{
			return GetAsType(properties_double_cache, key, def);
		}
		double GetAsF64(const std::string &key, double def = 0.0f)
		{
			return GetAsF64(key.c_str(), def);
		}
		bool GetAsBool(const std::string &key, bool def = false)
		{
			return GetAsUint32(key.c_str(), def) > 0;
		}


		static std::string trim(const char *str);

	protected:

		Properties()  {}
		~Properties() {}
		Properties(const Properties&);
		Properties& operator= (const Properties&);

	private:

		int32 atot(const char *value, int32)
		{
			return atoi(value);
		}
		uint32 atot(const char *value, uint32)
		{
			return atoi(value);
		}
		int64 atot(const char *value, int64)
		{
			return atoll(value);
		}
		uint64 atot(const char *value, uint64)
		{
			return atoll(value);
		}
		double atot(const char *value, double)
		{
			return atof(value);
		}

		template <class C, class T>
		T GetAsType(C &cache, const char *key, T def)
		{
			typename C::iterator iter = cache.find(key);
			if (iter != cache.end())
			{
				return iter->second;
			}
			else
			{
				if (const char *value = Get(key))
				{
					T data = atot(value, T());
					cache.insert(std::make_pair(key, data));
					return data;
				}
			}
			return def;
		}

		const char * Get(const char *key)
		{
			PropsMap::iterator iter = properties.find(key);
			if (iter != properties.end())
			{
				return iter->second.c_str();
			}
			return NULL;
		}

		typedef std::map<std::string, std::string> PropsMap;
		typedef std::map<std::string, uint32> PropsUint32Map;
		typedef std::map<std::string, uint64> PropsUint64Map;
		typedef std::map<std::string, double> PropsDoubleMap;

		PropsMap properties;
		PropsUint32Map properties_uint32_cache;
		PropsUint64Map properties_uint64_cache;
		PropsDoubleMap properties_double_cache;
};

#define sProperties Properties::Instance() 
END_NS_CORE
