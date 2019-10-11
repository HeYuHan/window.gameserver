#include <getopt.h>
#include <tools.h>
#include <log.h>
#include "server.h"
#ifndef _DEBUG
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif // DEBUG
#include <windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <conio.h>
#include <tchar.h>
#pragma comment (lib,"Psapi.lib")
//
enum
{
	addr_udp = 0x100,
	addr_udp_pwd,
	addr_tcp,
	heart,
	max_client,
	flag_daemon,
	log_name,
	log_path
};

struct option long_options[] =
{
	{ "addr.udp",required_argument,0,addr_udp },
	{ "addr.udp.pwd",required_argument,0,addr_udp_pwd },
	{ "addr.tcp",required_argument,0,addr_tcp },
	{ "max",required_argument,0,max_client},
	{ "heart",required_argument,0,heart },
	{ "daemon",no_argument,0,flag_daemon },

	{ "log_name",optional_argument,0,log_name },
	{ "log_path",optional_argument,0,log_path }
};



int main(int argc, char **args) {
	bool as_daemon = false;
	gLogger.logName = "server";
	gServer.m_TcpAddr = "0.0.0.0:9500";
	gServer.m_HttpAddr = "0.0.0.0:9501";
	gServer.m_UdpAddr = "0.0.0.0:9502";
	gServer.m_UdpPwd = "broadserver";
	gServer.m_HeartTime = 5;
	gServer.m_MaxClient = 2;
#ifndef _DEBUG
	gLogger.m_LogToFile = true;
	gLogger.m_LogToConsole = false;
	gLogger.logName = "broadserver";
	gLogger.filePath = "./";
#endif
	while (1)
	{
		int option_index = 0;
		int option = getopt_long(argc, args, "", long_options, &option_index);
		if (option <= 0)break;
		switch (option)
		{
		case addr_udp_pwd:
			gServer.m_UdpPwd = optarg;
			break;
		case addr_tcp:
			gServer.m_TcpAddr = optarg;
			break;
		case addr_udp:
			gServer.m_UdpAddr = optarg;
			break;
		case max_client:
			gServer.m_MaxClient = atoi(optarg);
			break;
		case heart:
			gServer.m_HeartTime = atoi(optarg);
			break;
		case flag_daemon:
			as_daemon = true;
			break;
		case log_path:
			gLogger.m_LogToFile = true;
			gLogger.filePath = optarg;
			break;
		case log_name:
			gLogger.logName = optarg;
			break;
		case '?':
			return 1;
			break;
		default:
			break;
		}
	}
	std::string filePath = args[0];
	if (true)
	{
		
		std::string daemon_process = filePath.substr(0, filePath.length() - 4) + ".daemon.exe";
		int pos = daemon_process.find_last_of("\\");
		std::string daemon_process_tag = daemon_process.substr(pos + 1);
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hSnapshot)
		{
			return NULL;
		}
		PROCESSENTRY32 pe = { 0 };
		pe.dwSize = sizeof(PROCESSENTRY32);
		DWORD processId = GetCurrentProcessId();
		for (bool fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
		{
			if (pe.th32ProcessID == processId)continue;
			char szProcessName[MAX_PATH] = { 0 };
			HANDLE        hProcess;
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pe.th32ProcessID);
			if (hProcess && GetProcessImageFileName(hProcess, szProcessName, MAX_PATH))
			{
				std::string str(szProcessName);
				if (str.find(daemon_process_tag) != std::string::npos)
				{
					printf("kill daemon process %s\n", szProcessName);
					TerminateProcess(hProcess, 0);
					CloseHandle(hProcess);
					Sleep(1000);
					break;
				}
			}
		}
		if (as_daemon&&filePath.find(".daemon.exe") == std::string::npos)
		{
			FILE *fd1 = fopen(filePath.c_str(), "rb");
			FILE *fd2 = fopen(daemon_process.c_str(), "wb+");

			while (1)
			{
				char buff[1024 * 512];
				int ret = fread(buff, 1, 1024 * 512, fd1);
				if (ret > 0)
				{
					fwrite(buff, 1, ret, fd2);
					continue;
				}
				break;
			}
			fclose(fd1);
			fclose(fd2);
			char process_path[1024] = { 0 };
			std::string process_arg;
			for (int i = 1; i < argc; i++)
			{
				process_arg = process_arg + std::string(args[i]);
				if (i != argc - 1)process_arg += "\ ";
			}
			sprintf(process_path, "%s %s", daemon_process.c_str(), process_arg.c_str());
			printf("start deamon process %s\n", process_path);
			STARTUPINFO start_info;
			PROCESS_INFORMATION process_info;
			ZeroMemory(&start_info, sizeof(start_info));
			ZeroMemory(&process_info, sizeof(process_info));
			if (CreateProcess(NULL, process_path, NULL, NULL, false, 0, NULL, NULL, &start_info, &process_info))
			{
				CloseHandle(process_info.hThread);
				CloseHandle(process_info.hProcess);
			}
		}







		
	}
	if (filePath.find(".daemon.exe")!=std::string::npos)
	{
		
		std::string target_process = filePath.substr(0, filePath.length() - 11) + ".exe";
		
		int pos = target_process.find_last_of("\\");
		std::string target_process_tag = target_process.substr(pos+1);
		printf("run daemon process %s\n", target_process.c_str());
		while (true)
		{
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (INVALID_HANDLE_VALUE == hSnapshot)
			{
				return NULL;
			}
			PROCESSENTRY32 pe = { 0 };
			pe.dwSize = sizeof(PROCESSENTRY32);
			DWORD processId = GetCurrentProcessId();
			bool find_process = false;
			for (bool fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
			{
				if (pe.th32ProcessID == processId)continue;
				char szProcessName[MAX_PATH] = { 0 };
				HANDLE        hProcess;
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pe.th32ProcessID);
				if (hProcess && GetProcessImageFileName(hProcess, szProcessName, MAX_PATH))
				{
					std::string str(szProcessName);
					if (str.find(target_process_tag)!=std::string::npos)
					{
						printf("find target process %s\n", szProcessName);
						find_process = true;
						break;
					}
				}
			}
			if (!find_process)
			{
				char process_path[1024] = { 0 };
				std::string process_arg;
				for (int i = 1; i < argc; i++)
				{
					process_arg = process_arg + std::string(args[i]);
					if (i != argc - 1)process_arg += "\ ";
				}
				sprintf(process_path, "%s %s", target_process.c_str(), process_arg.c_str());
				printf("start deamon process %s\n", process_path);
				STARTUPINFO start_info;
				PROCESS_INFORMATION process_info;
				ZeroMemory(&start_info, sizeof(start_info));
				ZeroMemory(&process_info, sizeof(process_info));
				if (CreateProcess(NULL, process_path, NULL, NULL, false, 0, NULL, NULL, &start_info, &process_info))
				{
					CloseHandle(process_info.hThread);
					CloseHandle(process_info.hProcess);
				}
			}
			Sleep(1000);
		}
		return 0;
	}
	if (as_daemon && !RunAsDaemon())
	{
		log_error("%s", "run as daemon error!");
		return -1;
	}
	return gServer.Run();
}
