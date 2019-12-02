#pragma once
#include "Resource.h"
#include "GameObject.h"

class ResourceScene : public Resource
{
public:
	ResourceScene(UID id);
	virtual ~ResourceScene();

	static bool Import(const char* full_path, std::string& output);
	bool SaveOwnFormat(std::string& output) const;
	bool LoadtoScene();
	void UnLoad();

private:
	std::vector<GameObject*> gameobjs;
};

