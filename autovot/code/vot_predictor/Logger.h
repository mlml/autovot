/************************************************************************
 Copyright (c) 2014 Joseph Keshet, Morgan Sonderegger, Thea Knowles

This file is part of Autovot, a package for automatic extraction of
voice onset time (VOT) from audio files.

Autovot is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

Autovot is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with Autovot.  If not, see
<http://www.gnu.org/licenses/>.
************************************************************************/

#ifndef __LOG1_H__
#define __LOG1_H__

#include <sstream>
#include <string>
#include <algorithm>
#include <cstdio>
//#include <stdio.h>
#include <sys/time.h>


struct MatchPathSeparator
{
	bool operator()( char ch ) const
	{
		return ch == '\\' || ch == '/';
	}
};

inline std::string basename( std::string const& pathname )
{
	return std::string(std::find_if(pathname.rbegin(), pathname.rend(), MatchPathSeparator()).base(), pathname.end());
}

inline std::string NowTime()
{
	char buffer[11];
	time_t t;
	time(&t);
	tm r = {0};
	strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
	struct timeval tv;
	gettimeofday(&tv, 0);
	char result[100] = {0};
	std::sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000);
	return result;
}


enum LogLevel {ERROR, WARNING, INFO, DEBUG};

class Log
{
public:
	Log();
	virtual ~Log();
	std::ostringstream& Get(LogLevel level = INFO);
public:
	static LogLevel& ReportingLevel();
	static std::string& ExecutableName();
	static std::string ToString(LogLevel level);
	static LogLevel FromString(const std::string& level);
protected:
	std::ostringstream os;
private:
	Log(const Log&);
	Log& operator =(const Log&);
};

#define LOG(level) \
if (level > Log::ReportingLevel()) ; \
else Log().Get(level)


inline Log::Log()
{
}

inline std::ostringstream& Log::Get(LogLevel level)
{
	os << NowTime();
	os << " [" << Log::ExecutableName() << "] ";
	os << ToString(level) << ": ";
	os << std::string(level > INFO ? level - INFO : 0, '\t');
	return os;
}

inline Log::~Log()
{
	os << std::endl;
	fprintf(stderr, "%s", os.str().c_str());
	fflush(stderr);
}

inline LogLevel& Log::ReportingLevel()
{
	static LogLevel reportingLevel = INFO;
	return reportingLevel;
}

inline std::string& Log::ExecutableName()
{
	static std::string executableName = "";
	return executableName;
}

inline std::string Log::ToString(LogLevel level)
{
	static const char* const buffer[] = {"ERROR", "WARNING", "INFO", "DEBUG"};
	return buffer[level];
}

inline LogLevel Log::FromString(const std::string& level)
{
	if (level == "DEBUG")
		return DEBUG;
	if (level == "INFO")
		return INFO;
	if (level == "WARNING")
		return WARNING;
	if (level == "ERROR")
		return ERROR;
	Log().Get(WARNING) << "Unknown logging level '" << level << "'. Using INFO level as default.";
	return INFO;
}


#endif //__LOG_H__
