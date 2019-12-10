#pragma once

#include "Globals.h"
#include <string>
#include <vector>

struct PathNode
{
	PathNode() : path("") {};
	
	std::string path; //Path in reference to the first PathNode
	std::string file_name; //Folder / file itself
	std::vector<PathNode> children;

	bool leaf = true;
	bool file = true;

	bool IsLastFolder() const
	{
		for (uint i = 0; i < children.size(); i++)
			if (children[i].file == false)
				return false;
		return true;
	}

	bool operator == (const PathNode node) const
	{
		return path == node.path;
	}
};
