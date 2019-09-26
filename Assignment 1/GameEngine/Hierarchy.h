#pragma once
#include <vector>
#include <random>

#define MAX_NAME_LENGTH 512

struct HierarchyNode 
{

	long id;
	std::vector<HierarchyNode> childs;

	HierarchyNode() 
	{
		id = lrand();
	}
	~HierarchyNode() 
	{
		childs.clear();
	}

private:

	long lrand()
	{
		if (sizeof(int) < sizeof(long))
			return (static_cast<long>(rand()) << (sizeof(int) * 8)) |
			rand();

		return rand();
	}
};


class Hierarchy
{

public:

	Hierarchy();
	~Hierarchy();

	//static Hierarchy hierarchy;
	static void Init();

	//Gui

	static void ShowHierarchy(bool* open);

	static void Draw(const char* title, bool* p_open);

	//Naming

	static bool SetSceneName(const char* name);

	static char scene_name[MAX_NAME_LENGTH];

	//Data

	static std::vector<HierarchyNode> nodes;

	//std::vector<HierarchyNode>* nodes;
};

