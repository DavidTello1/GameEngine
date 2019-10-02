#include "Console.h"

Console::Console()
{
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;

	//Commands
	Commands.push_back("help, -h");
	Commands.push_back("copy, -cp");
	Commands.push_back("clear, -cl");
	Commands.push_back("CLASSIFY");  // "classify" is only here to provide an example of "C"+[tab] completing to "CL" and displaying matches.

	//Booleans
	AutoScroll		= true;
	ShowDebugLog	= true;
	ShowGeometryLog = true;
	ShowVerboseLog	= true;
	ScrollToBottom	= false;


	AddLog("Enter 'HELP' for help, press TAB to use text completion.");
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
