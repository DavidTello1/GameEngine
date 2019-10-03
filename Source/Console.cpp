#include "Console.h"

Console::Console()
{
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;

	//Commands
	Commands.push_back("help");
	Commands.push_back("copy");
	Commands.push_back("clear");

	//Booleans
	AutoScroll		= true;
	ShowDebugLog	= true;
	ShowGeometryLog = true;
	ShowVerboseLog	= true;
	ScrollToBottom	= false;

	AddLog("Enter 'help' or '-h' for help, press TAB to use text completion.");
}

Console::~Console()
{
	ClearLog();
	for (int i = 0; i < History.Size; i++)
		free(History[i]);
}

Console Console::console = Console();

void Console::ShowConsole(bool* p_open)
{
	console.Draw("Console", p_open);
}
