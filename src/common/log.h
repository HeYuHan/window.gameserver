#pragma once
#ifndef __LOG_H__
#define __LOG_H__

#define USE_LOG4CPP
#ifdef USE_LOG4CPP

#endif // USE_LOG4CPP
#include <errno.h>

class Logger
{
public:
	Logger();
	~Logger();
	void Init();
public:
	const char* logName;
	const char* filePath;
	bool m_LogToConsole;
	bool m_LogToFile;
	void warn(const char* stringFormat, ...);
	void info(const char* stringFormat, ...);
	void debug(const char* stringFormat, ...);
	void error(const char* stringFormat, ...);
private:
	bool m_Inited;

};
extern Logger gLogger;
#define log_warn(__format__,...) \
	gLogger.warn(__format__,##__VA_ARGS__)

#define log_info(__format__,...) \
	gLogger.info(__format__,##__VA_ARGS__)

#define log_debug(__format__,...) \
	gLogger.debug(__format__,##__VA_ARGS__)

#define log_error(__format__,...) \
	gLogger.error(__format__,##__VA_ARGS__)

#define log_lasterror() \
	log_error("%s",strerror(errno))


#endif

