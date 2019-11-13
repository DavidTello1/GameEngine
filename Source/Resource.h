#pragma once
#include "Globals.h"
#include <string>

class Config;

class Resource
{
	friend class ModuleResources;

public:
	enum Type
	{
		unknown = 0,
		model,
		material,
		mesh,
	};

public:
	Resource(UID uid, Resource::Type type);
	virtual ~Resource();

	Resource::Type GetType() const { return type; }
	UID GetID() const { return uid; }

	const char* GetName() const { return name.c_str(); }
	void SetName(std::string n) { name = n; }

	virtual bool Import();
	virtual bool Load();
	virtual bool UnLoad();
	virtual void Draw();


	//const char*         GetFile() const;
	//const char*         GetExportedFile() const;
	//bool                IsLoadedToMemory() const;
	//bool                LoadToMemory();
	//void                Release();

	//uint                CountReferences() const;

	//virtual void        Save(Config& config) const;
	//virtual void        Load(const Config& config);
//
//protected:
//
//	virtual bool    LoadInMemory() = 0;
//	virtual void    ReleaseFromMemory() = 0;

protected:
	UID uid = 0;
	Type type = unknown;
	std::string name;

	std::string file;
	std::string exported_file;

	//uint loaded = 0;
};