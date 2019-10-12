#include <getopt.h>
#include <tools.h>
#include <log.h>
#include "server.h"
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
	log_path,
	flag_daemon_process,
	flag_show_window
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
	{ "log_path",optional_argument,0,log_path },
	{ "daemon_process",optional_argument,0,flag_daemon_process },
	{"windowed",optional_argument,0,flag_show_window }
};

int PROCESS_ARG_COUNT;
char** PROCESS_ARG_LIST;
bool IS_DAEMON_PROCESS = false;
bool RUN_AS_DAEMON = false;
bool SHOW_WINDOW = true;

int main(int argc, char **args) {
	PROCESS_ARG_COUNT = argc;
	PROCESS_ARG_LIST = args;
	bool as_daemon = false;
#ifdef _DEBUG
	bool show_window = true;
#else
	bool show_window = false;
#endif // _DEBUG

	
	gLogger.logName = "server";
	gServer.m_TcpAddr = "0.0.0.0:9500";
	gServer.m_HttpAddr = "0.0.0.0:9501";
	gServer.m_UdpAddr = "0.0.0.0:9502";
	gServer.m_UdpPwd = "broadserver";
	gServer.m_HeartTime = 5;
	gServer.m_MaxClient = 2;
//#ifndef _DEBUG
//	gLogger.m_LogToFile = true;
//	gLogger.m_LogToConsole = true;
//	gLogger.logName = "broadserver";
//	gLogger.filePath = "./";
//#endif

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
		case flag_daemon_process:
			IS_DAEMON_PROCESS = true;
			break;
		case flag_show_window:
			show_window = true;
			break;
		case '?':
			return 1;
			break;
		default:
			break;
		}
	}
	SHOW_WINDOW = show_window;
	RUN_AS_DAEMON = as_daemon;
	if (!SHOW_WINDOW)
	{
		gLogger.m_LogToFile = true;
		gLogger.m_LogToConsole = true;
		gLogger.logName = "broadserver";
		gLogger.filePath = "./";
	}
	if (as_daemon && !RunAsDaemon())
	{
		log_error("%s", "run as daemon error!");
		return -1;
	}
	return gServer.Run();
}
