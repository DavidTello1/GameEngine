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
	char logtype = 'a';
	while (va_arg(ap, char) != NULL) {
		logtype = va_arg(ap, char);

	}
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	//Console logging
	static char short_file[256];
	// 92 stands for \ character
	static const char* lastslashlocation = strrchr(file, 92);
	strcpy(short_file, lastslashlocation+1);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", short_file, line, tmp_string);

	// geometry logs
	if (logtype == 'g' && Console::console.ShowGeometryLog) {
		Console::console.AddLog(tmp_string2);
	}
	// verbose logs
	else if (logtype == 'v' && Console::console.ShowVerboseLog) {
		Console::console.AddLog(tmp_string2);
	}
	// debug logs
	else if (logtype == 'd' && Console::console.ShowDebugLog) {
		Console::console.AddLog(tmp_string2);
	}

}