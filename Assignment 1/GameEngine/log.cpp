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
	char logtype = va_arg(ap, char);
	// geometry logs
	if (logtype == 'g') {
		OutputDebugString(" Geometry logging");
	}
	// verbose logs
	if (logtype == 'v') {
		OutputDebugString(" Verbose logging");
	}
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);
	Console::console.AddLog(tmp_string2);

}