#pragma once
#include "Resource.h"
#include "GameObject.h"

class ResourceScene : public Resource
{
public:
	ResourceScene(UID id);
	virtual ~ResourceScene();

	static bool Import(const char* full_path, std::string& asset_file);
	bool SaveOwnFormat(std::string& asset_file) const;
	bool LoadtoScene();
	void UnLoad();

private:
	std::vector<GameObject*> gameobjs;
};

