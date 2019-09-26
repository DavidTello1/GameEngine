#pragma once

#define MAX_NAME_LENGTH 512

class Hierarchy
{

public:

	Hierarchy();
	~Hierarchy();

	static void ShowHierarchy(bool* open);

	static Hierarchy hierarchy;

	static void Draw(const char* title, bool* p_open);

	static char scene_name[MAX_NAME_LENGTH];

	bool SetSceneName(const char* name);
};

