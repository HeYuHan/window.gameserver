#pragma once
#ifndef __LocalUserDatabaseHelper_H__
#define __LocalUserDatabaseHelper_H__
#include<string>
#include <vector>
namespace SQLite
{
	class Database;
}
namespace Core
{

	class LocalUserDatabaseHelper
	{
	public:
		enum
		{
			OK=0,USER_NOT_FOUND,USER_NAME_EXITS,USER_ACCOUNT_EXITES,AVILD_ERROR,USER_ACCOUNT_IS_NULL,USER_PASSWARD_IS_NULL,USER_NAME_IS_NULL
		};
		enum
		{
			UPDATE_USER_NAME=1<<1,UPDATE_USER_PASSWARD=1<<2,UPDATE_USER_DATA=1<<3, UPDATE_USER_ALL= UPDATE_USER_NAME| UPDATE_USER_PASSWARD| UPDATE_USER_DATA
		};
		struct SampleUser
		{
			unsigned long long m_ID;
			std::string m_Account;
			std::string m_Password;
			std::string m_Name;
			std::string m_CreateTime;
			std::string m_UserData;
			
		};
	public:
		LocalUserDatabaseHelper();
		~LocalUserDatabaseHelper();
		bool OpenDatabase(const char* path, int flg= 0x00000002 | 0x00000004);
		int ValidUser(const SampleUser user,bool check_in_db);
		int AddUser(SampleUser user);
		int UpdateUser(SampleUser user,int tag = UPDATE_USER_ALL);
		int GetUserByAccount(SampleUser &user);
		int GetUserByName(SampleUser &user);
		int DelUserById(int id);
		int DelUserByAccount(const char* account);
		int DelUserByName(const char* name);
		void GetAllUser(std::vector<SampleUser> &ret);
	private:
		SQLite::Database *m_Database;
	};

}







#endif // !LocalUserDatabaseHelper
