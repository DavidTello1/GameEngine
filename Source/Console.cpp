#include "Console.h"

char						Console::InputBuf[256];
ImVector<char*>				Console::Items;
ImVector<const char*>		Console::Commands;
ImVector<char*>				Console::History;
int							Console::HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
ImGuiTextFilter				Console::Filter;
bool						Console::AutoScroll;
bool						Console::ShowVerboseLog;
bool						Console::ShowGeometryLog;
bool						Console::ShowDebugLog;
bool						Console::ShowWarningLog;
bool						Console::EnableFileName;
bool						Console::ScrollToBottom;

Console::Console() : Panel("Console")
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
	ShowVerboseLog	= false;
	ShowWarningLog	= true;
	EnableFileName	= false;
	ScrollToBottom	= false;
	has_menubar 	= true;

	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	UpdateFilters();

	AddLog("Enter 'help' or '-h' for help, press TAB to use text completion.");
}

void Console::UpdateFilters()
{
	char filters_buffer[256] = " ";

	if (!ShowDebugLog)	 strcat_s(filters_buffer, 256, "-[Debug],");
	if (!ShowGeometryLog) strcat_s(filters_buffer, 256, "-[Geometry],");
	if (!ShowVerboseLog)  strcat_s(filters_buffer, 256, "-[Verbose],");
	if (!ShowWarningLog)  strcat_s(filters_buffer, 256, "-[Warning],");


	Filter.Clear();
	sprintf_s(Filter.InputBuf, 256, "%s", filters_buffer);
	Filter.Build();
}

Console::~Console()
{
	ClearLog();
	for (int i = 0; i < History.Size; i++)
		free(History[i]);
}

void Console::Draw()
{
	static bool copy_to_clipboard = false;

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			in_menu = true;

			ImGui::MenuItem("Auto-scroll", NULL, &AutoScroll);
			ImGui::Separator();

			if(ImGui::MenuItem("Show verbose log", NULL, &ShowVerboseLog))UpdateFilters();
			if(ImGui::MenuItem("Show geometry log", NULL, &ShowGeometryLog))UpdateFilters();
			if(ImGui::MenuItem("Show debug log", NULL, &ShowDebugLog))UpdateFilters();
			if (ImGui::MenuItem("Show warning log", NULL, &ShowWarningLog))UpdateFilters();

			ImGui::Separator();
			ImGui::MenuItem("Enable file name", NULL, &EnableFileName);

			ImGui::EndMenu();
		}
		else
			in_menu = false;

		if (ImGui::MenuItem("Clear")) { ClearLog(); }
		copy_to_clipboard = ImGui::MenuItem("Copy");
		
		ImGui::EndMenuBar();
	}

	Filter.Draw("Filter ('inclusion,-exclusion')", 180);
	ImGui::Separator();

	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::Selectable("Clear")) ClearLog();
		ImGui::EndPopup();
	}

	// Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
	// NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
	// You can seek and display only the lines that are visible using the ImGuiListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
	// To use the clipper we could replace the 'for (int i = 0; i < Items.Size; i++)' loop with:
	//     ImGuiListClipper clipper(Items.Size);
	//     while (clipper.Step())
	//         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
	// However, note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
	// A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
	// and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
	// If your items are of variable size you may want to implement code similar to what ImGuiListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	if (copy_to_clipboard)
		ImGui::LogToClipboard();
	for (int i = 0; i < Items.Size; i++)
	{
		const char* item = Items[i];
		if (!Filter.PassFilter(item))
			continue;

		// Normally you would store more information in your item (e.g. make Items[] an array of structure, store color/type etc.)
		bool pop_color = false;
		if (strstr(item, "[Error]")) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true; }
		else if (strstr(item, "[Warning]")) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.54f, 0.0f, 1.0f)); pop_color = true; }
		else if (strncmp(item, "# ", 2) == 0) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f)); pop_color = true; }
		ImGui::TextUnformatted(item);
		if (pop_color)
			ImGui::PopStyleColor();
	}
	if (copy_to_clipboard)
		ImGui::LogFinish();

	if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
		ImGui::SetScrollHereY(1.0f);
	ScrollToBottom = false;

	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::Separator();

	// Command-line
	bool reclaim_focus = false;
	if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub,nullptr /*(void*)this*/))
	{
		char* s = InputBuf;
		Strtrim(s);
		if (s[0])
			ExecCommand(s);
		strcpy(s, "");
		reclaim_focus = true;
	}

	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

}

void Console::ExecCommand(const char* command_line)
{
	AddLog("# %s\n", command_line);

	// Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
	HistoryPos = -1;
	for (int i = History.Size - 1; i >= 0; i--)
		if (Stricmp(History[i], command_line) == 0)
		{
			free(History[i]);
			History.erase(History.begin() + i);
			break;
		}
	History.push_back(Strdup(command_line));

	// Process command
	if (Stricmp(command_line, "clear") == 0 || Stricmp(command_line, "Clear") == 0 || Stricmp(command_line, "CLEAR") == 0 || Stricmp(command_line, "-cl") == 0)
	{
		ClearLog();
	}
	else if (Stricmp(command_line, "help") == 0 || Stricmp(command_line, "Help") == 0 || Stricmp(command_line, "HELP") == 0 || Stricmp(command_line, "-h") == 0)
	{
		AddLog("Commands:");
		for (int i = 0; i < Commands.Size; i++)
			AddLog("- %s", Commands[i]);
	}
	else if (Stricmp(command_line, "copy") == 0 || Stricmp(command_line, "Copy") == 0 || Stricmp(command_line, "COPY") == 0 || Stricmp(command_line, "-cp") == 0)
	{
		int first = History.Size - 10;
		for (int i = first > 0 ? first : 0; i < History.Size; i++)
			AddLog("%3d: %s\n", i, History[i]);
	}
	else
	{
		AddLog("Unknown command: '%s'\n", command_line);
	}

	// On commad input, we scroll to bottom even if AutoScroll==false
	ScrollToBottom = true;
}

int Console::TextEditCallbackStub(ImGuiInputTextCallbackData* data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
{
	Console* console = (Console*)data->UserData;
	return console->TextEditCallback(data);
}

int Console::TextEditCallback(ImGuiInputTextCallbackData* data)
{
	//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackCompletion:
	{
		// Example of TEXT COMPLETION

		// Locate beginning of current word
		const char* word_end = data->Buf + data->CursorPos;
		const char* word_start = word_end;
		while (word_start > data->Buf)
		{
			const char c = word_start[-1];
			if (c == ' ' || c == '\t' || c == ',' || c == ';')
				break;
			word_start--;
		}

		// Build a list of candidates
		ImVector<const char*> candidates;
		for (int i = 0; i < Commands.Size; i++)
			if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
				candidates.push_back(Commands[i]);

		if (candidates.Size == 0)
		{
			// No match
			AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
		}
		else if (candidates.Size == 1)
		{
			// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
			data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
			data->InsertChars(data->CursorPos, candidates[0]);
			data->InsertChars(data->CursorPos, " ");
		}
		else
		{
			// Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
			int match_len = (int)(word_end - word_start);
			for (;;)
			{
				int c = 0;
				bool all_candidates_matches = true;
				for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
					if (i == 0)
						c = toupper(candidates[i][match_len]);
					else if (c == 0 || c != toupper(candidates[i][match_len]))
						all_candidates_matches = false;
				if (!all_candidates_matches)
					break;
				match_len++;
			}

			if (match_len > 0)
			{
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
			}

			// List matches
			AddLog("Possible matches:\n");
			for (int i = 0; i < candidates.Size; i++)
				AddLog("- %s\n", candidates[i]);
		}

		break;
	}
	case ImGuiInputTextFlags_CallbackHistory:
	{
		// Example of HISTORY
		const int prev_history_pos = HistoryPos;
		if (data->EventKey == ImGuiKey_UpArrow)
		{
			if (HistoryPos == -1)
				HistoryPos = History.Size - 1;
			else if (HistoryPos > 0)
				HistoryPos--;
		}
		else if (data->EventKey == ImGuiKey_DownArrow)
		{
			if (HistoryPos != -1)
				if (++HistoryPos >= History.Size)
					HistoryPos = -1;
		}

		// A better implementation would preserve the data on the current input line along with cursor position.
		if (prev_history_pos != HistoryPos)
		{
			const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
			data->DeleteChars(0, data->BufTextLen);
			data->InsertChars(0, history_str);
		}
	}
	}
	return 0;
}