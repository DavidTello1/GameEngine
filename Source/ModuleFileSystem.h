#ifndef __MODULEFILESYSTEM_H__
#define __MODULEFILESYSTEM_H__

#include "Module.h"
#include "Globals.h"
#include <vector>

#include "PhysFS/include/physfs.h"

struct SDL_RWops;
int close_sdl_rwops(SDL_RWops *rw);

class ModuleFileSystem : public Module
{
public:

	ModuleFileSystem(const char* game_path = nullptr);
	~ModuleFileSystem();

	bool Init(Config* config = nullptr);
	bool CleanUp();

	// Utility functions
	bool AddPath(const char* path_or_zip);

	bool Exists(const char* file) const { return PHYSFS_exists(file) != 0; }
	bool IsDirectory(const char* file) const { return PHYSFS_isDirectory(file) != 0; }
	void CreateDirectory(const char* directory) { PHYSFS_mkdir(directory); }

	void DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const;
	bool CopyFromOutsideFS(const char* full_path, const char* destination);
	bool Copy(const char* source, const char* destination);

	std::string GetFileName(const char* full_path) const; //returns file name (baker_house.fbx)
	std::string GetExtension(const char* full_path) const; //returns extension (fbx)
	void NormalizePath(char* full_path, bool to_lower) const;
	void NormalizePath(std::string& full_path, bool to_lower) const;

	// Open for Read/Write
	unsigned int LoadFromPath(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;
	//SDL_RWops* LoadFile(const char* file) const;

	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;
	bool SaveUnique(std::string& output, const void* buffer, uint size, const char* path, const char* prefix, const char* extension);
	bool Remove(const char* file);

	const char* GetBasePath() const { return PHYSFS_getBaseDir(); }
	const char* GetWritePath() const { return PHYSFS_getWriteDir(); }
	const char* GetReadPaths() const;
};

#endif // __MODULEFILESYSTEM_H__
