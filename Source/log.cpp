#pragma once
#include "Globals.h"
#include "Console.h"

#define GEOMETRY_STRING "[Geometry]"

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
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	//Console logging
	static char short_file[256];
	// 92 stands for '\' character
	const char* lastslashlocation = strrchr(file, 92);
	strcpy(short_file, lastslashlocation+1);

	// geometry logs
	if (logtype == 'g')
	{
		sprintf_s(extendedformat,4096,GEOMET)
		if (Console::ShowGeometryLog) 
		{
			sprintf_s(extendedlog, 4096, "[Geometry] %s(%d) : %s", short_file, line, tmp_string);
			Console::AddLog(extendedlog);
		} // else { ignore log }
	}
	// debug logs
	else if (logtype == 'd')
	{ 
		if (Console::ShowDebugLog)
		{
			sprintf_s(extendedlog, 4096, "[Debug] %s(%d) : %s", short_file, line, tmp_string);
			Console::AddLog(extendedlog);
		} // else { ignore log }
	}
	// warning logs
	else if (logtype == 'w')
	{
			sprintf_s(extendedlog, 4096, "[Warn] %s(%d) : %s", short_file, line, tmp_string);
			Console::AddLog(extendedlog);
	}
	else if (logtype == 'e') // errors can not be ignored, there's always something to learn
	{
		sprintf_s(extendedlog, 4096, "[Error] %s(%d) : %s", short_file, line, tmp_string);
		Console::AddLog(extendedlog);
	}
	// verbose logs
	else if (Console::ShowVerboseLog) {
		sprintf_s(extendedlog, 4096, "[Verbose] %s(%d) : %s", short_file, line,  tmp_string);
		Console::AddLog(extendedlog);
	} // else { ignore log }

}


//OLD CONSOLE
//// geometry logs
//if (logtype == 'g')
//{
//	if (Console::ShowGeometryLog)
//	{
//		sprintf_s(extendedlog, 4096, "[Geometry] %s(%d) : %s", short_file, line, tmp_string);
//		Console::AddLog(extendedlog);
//	} // else { ignore log }
//}
//// debug logs
//else if (logtype == 'd')
//{
//	if (Console::ShowDebugLog)
//	{
//		sprintf_s(extendedlog, 4096, "[Debug] %s(%d) : %s", short_file, line, tmp_string);
//		Console::AddLog(extendedlog);
//	} // else { ignore log }
//}
//// warning logs
//else if (logtype == 'w')
//{
//	if (Console::ShowDebugLog)
//	{
//		sprintf_s(extendedlog, 4096, "[Warn] %s(%d) : %s", short_file, line, tmp_string);
//		Console::AddLog(extendedlog);
//	}
//}
//else if (logtype == 'e') // errors can not be ignored, there's always something to learn
//{
//	sprintf_s(extendedlog, 4096, "[Error] %s(%d) : %s", short_file, line, tmp_string);
//	Console::AddLog(extendedlog);
//}
//// verbose logs
//else if (Console::ShowVerboseLog) {
//	sprintf_s(extendedlog, 4096, "[Verbose] %s(%d) : %s", short_file, line, tmp_string);
//	Console::AddLog(extendedlog);
//} // else { ignore log }