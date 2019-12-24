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
		scene
	};

public:
	Resource(UID uid, Resource::Type type);
	virtual ~Resource();

	Resource::Type GetType() const { return type; }
	UID GetID() const { return uid; }
	const char* GetFile() const { return original_file.c_str(); }
	const char* GetExportedFile() const;

	//virtual void Save(Config& config) const;
	//virtual void Load(const Config& config);
	
	bool LoadToMemory();
	void ReleaseFromMemory();
	bool IsLoadedToMemory() const { return times_loaded > 0; }
	uint CountReferences() const { return times_loaded; }

protected:
	virtual bool SaveOwnFormat(std::string& asset_file) const = 0;
	virtual bool LoadtoScene() = 0;
	virtual void UnLoad() = 0;

protected:
	UID uid = 0;
	Type type = unknown;
	std::string original_file; //bakerhouse.fbx

	uint times_loaded = 0;

	//bool visible = true;
};