#pragma once
#include "Resource.h"
#include "GameObject.h"

class ResourceScene : public Resource
{
public:
	ResourceScene(UID id);
	virtual ~ResourceScene();

	bool Import(const char* full_path);
	bool SaveOwnFormat() const;
	bool LoadtoScene();
	void UnLoad();

private:
	std::vector<GameObject*> gameobjs;
};

