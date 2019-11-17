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

	//virtual void Save(Config& config) const;
	//virtual void Load(const Config& config);
	
	const char* GetFile() const { return file.c_str(); }
	const char* GetExportedFile() const { return exported_file.c_str(); }

	bool LoadToMemory();
	void ReleaseFromMemory();
	bool IsLoadedToMemory() const { return times_loaded > 0; }
	uint CountReferences() const { return times_loaded; }

protected:
	virtual bool SaveOwnFormat(std::string& output) const = 0;
	virtual bool LoadtoScene() = 0;
	virtual void UnLoad() = 0;

protected:
	UID uid = 0;
	Type type = unknown;
	std::string name;

	std::string file;
	std::string exported_file;

	uint times_loaded = 0;

	//bool visible = true;
};