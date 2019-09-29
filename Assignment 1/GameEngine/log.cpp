#pragma once
#include "Globals.h"
#include "Console.h"

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;
	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);

	// Filtering
	static char extendedlog[4096];
	static char extendedformat[4096];
	strcpy(extendedformat, format);
	strcat(extendedformat, "%c");
	vsprintf(extendedlog, extendedformat, ap);
	char logtype = extendedlog[strlen(extendedlog)-1];

	va_end(ap);

	//Default visual studio logging
	sprintf_s(tmp_string2, 4096, "%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	//Console logging
	static char short_file[256];
	// 92 stands for '\' character
	const char* lastslashlocation = strrchr(file, 92);
	strcpy(short_file, lastslashlocation+1);

	// geometry logs
	if (logtype == 'g' && Console::console.ShowGeometryLog) {
		sprintf_s(tmp_string2, 4096, "[%s] %s(%d) : %s", "GEOMETRY", short_file, line, tmp_string);
		Console::console.AddLog(tmp_string2);
	}
	// debug logs
	else if (logtype == 'd' && Console::console.ShowDebugLog) {
		sprintf_s(tmp_string2, 4096, "[%s] %s(%d) : %s", "DEBUG", short_file, line,  tmp_string);
		Console::console.AddLog(tmp_string2);
	}
	// verbose logs
	else if (Console::console.ShowVerboseLog) {
		sprintf_s(tmp_string2, 4096, "[%s] %s(%d) : %s", "VERBOSE", short_file, line,  tmp_string);
		Console::console.AddLog(tmp_string2);
	}

}