#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "PathNode.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

#include "mmgr/mmgr.h"

using namespace std;

int close_sdl_rwops(SDL_RWops *rw)
{
	RELEASE_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

ModuleFileSystem::ModuleFileSystem(const char* game_path) : Module("FileSystem", true)
{
	// needs to be created before Init so other modules can use it
	char* base_path = SDL_GetBasePath();
	LOG("Loading PhysFS", 'd');
	PHYSFS_init(base_path);
	SDL_free(base_path);

	// workaround VS string directory mess
	AddPath(".");

	if (0 && game_path != nullptr)
		AddPath(game_path);

	// Dump list of paths
	LOG("FileSystem Operations base is [%s] plus:", GetBasePath(), 'd');
	LOG(GetReadPaths());

	// enable us to write in the game's dir area
	if (PHYSFS_setWriteDir(".") == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError(), 'e');

	// Make sure standard paths exist
	const char* dirs[] = {
		SETTINGS_FOLDER, 
		ASSETS_FOLDER, 
		LIBRARY_FOLDER,
		LIBRARY_MESH_FOLDER, 
		LIBRARY_MATERIAL_FOLDER, 
		LIBRARY_SCENE_FOLDER, 
		LIBRARY_MODEL_FOLDER
	};

	for (uint i = 0; i < sizeof(dirs) / sizeof(const char*); ++i)
	{
		if (PHYSFS_exists(dirs[i]) == 0)
			PHYSFS_mkdir(dirs[i]);
	}
}

// Destructor
ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit();
}

// Called before render is available
bool ModuleFileSystem::Init(Config* config)
{
	LOG("Loading File System");
	bool ret = true;

	// Ask SDL for a write dir
	char* write_path = SDL_GetPrefPath(App->GetOrganizationName(), App->GetAppName());

	// Trun this on while in game mode
	//if(PHYSFS_setWriteDir(write_path) == 0)
		//LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());


	SDL_free(write_path);

	return ret;
}

// Called before quitting
bool ModuleFileSystem::CleanUp()
{
	return true;
}

// Add a new zip file or folder
bool ModuleFileSystem::AddPath(const char* path_or_zip)
{
	bool ret = false;

	if (PHYSFS_mount(path_or_zip, nullptr, 1) == 0)
	{
		LOG("File System error while adding a path or zip: %s\n", PHYSFS_getLastError(), 'e');
	}
	else
		ret = true;

	return ret;
}

void ModuleFileSystem::DiscoverFiles(const char* directory, vector<string> & file_list, vector<string> & dir_list) const
{
	char **rc = PHYSFS_enumerateFiles(directory); //get base directory
	char **i;

	string dir(directory);
	for (i = rc; *i != nullptr; i++)
	{
		string str = string(directory) + string("/") + string(*i);
		if (IsDirectory(str.c_str()))
			dir_list.push_back(*i);
		else
			file_list.push_back(*i);
	}

	PHYSFS_freeList(rc);
}

bool ModuleFileSystem::CopyFromOutsideFS(const char * full_path, const char * destination)
{
	// Only place we acces non virtual filesystem
	bool ret = false;

	char buf[8192];
	size_t size;

	FILE* source = nullptr;
	fopen_s(&source, full_path, "rb"); //open source file
	PHYSFS_file* dest = PHYSFS_openWrite(destination); //open destination file (only write)

	if (source && dest) //check for error
	{
		while (size = fread_s(buf, 8192, 1, 8192, source))
			PHYSFS_write(dest, buf, 1, size); //copy source file to destination file

		fclose(source); //close source file
		PHYSFS_close(dest); //close destination file
		ret = true;

		LOG("File System copied file [%s] to [%s]", full_path, destination, 'd');
	}
	else
		LOG("File System error while copy from [%s] to [%s]", full_path, destination, 'e');

	return ret;
}

bool ModuleFileSystem::Copy(const char * source, const char * destination)
{
	bool ret = false;

	char buf[8192];

	PHYSFS_file* src = PHYSFS_openRead(source); //open source file (only read)
	PHYSFS_file* dst = PHYSFS_openWrite(destination); //open destination file (only write)

	PHYSFS_sint32 size;
	if (src && dst) //check for error
	{
		while (size = (PHYSFS_sint32)PHYSFS_read(src, buf, 1, 8192))
			PHYSFS_write(dst, buf, 1, size); //copy from source to destination file

		PHYSFS_close(src); //close source file
		PHYSFS_close(dst); //close destination file
		ret = true;

		LOG("File System copied file [%s] to [%s]", source, destination, 'd');
	}
	else
		LOG("File System error while copy from [%s] to [%s]", source, destination, 'e');

	return ret;
}

std::string ModuleFileSystem::GetFileName(const char* path) const
{
	const char* file_name = strrchr(path, 92);
	if (file_name == nullptr) 
		file_name = (strrchr(path, '/') != nullptr) ? strrchr(path, '/') : "";
	file_name++;
	return file_name;
}

std::string ModuleFileSystem::GetExtension(const char* path) const
{
	char buffer[32] = "";
	const char* last_dot = strrchr(path, '.');
	if (last_dot != nullptr)
		strcpy_s(buffer, last_dot + 1);

	std::string extension(buffer);
	return extension;
}

void ModuleFileSystem::NormalizePath(char* full_path) const
{
	int len = strlen(full_path);
	for (int i = 0; i < len; ++i)
	{
		if (full_path[i] == '\\')
			full_path[i] = '/';
	}
}

void ModuleFileSystem::NormalizePath(std::string & full_path) const
{
	for (string::iterator it = full_path.begin(); it != full_path.end(); ++it)
	{
		if (*it == '\\')
			*it = '/';
	}
}

void ModuleFileSystem::ToLower(char* full_path) const
{
	int len = strlen(full_path);
	for (int i = 0; i < len; ++i)
	{
		full_path[i] = tolower(full_path[i]);
	}
}

void ModuleFileSystem::ToLower(std::string & full_path) const
{
	for (string::iterator it = full_path.begin(); it != full_path.end(); ++it)
	{
		*it = tolower(*it);
	}
}


unsigned int ModuleFileSystem::LoadFromPath(const char * path, const char * file, char ** buffer) const
{
	string full_path(path); //get path
	full_path += file; //get full path (with file name)
	return Load(full_path.c_str(), buffer); //load
}

// Read a whole file and put it in a new buffer
uint ModuleFileSystem::Load(const char* file, char** buffer) const
{
	uint ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file); //open file (only read)

	if (fs_file != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fs_file); //get file size

		if (size > 0)
		{
			*buffer = new char[size];
			uint readed = (uint)PHYSFS_read(fs_file, *buffer, 1, size); //read file to buffer and get size
			if (readed != size) //check for error
			{
				LOG("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError(), 'e');
				RELEASE(buffer);
			}
			else
				ret = readed;
		}

		if (PHYSFS_close(fs_file) == 0) //close file
			LOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError(), 'e');
	}
	else
		LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError(), 'e');

	return ret; //return size of file
}

//// Read a whole file and put it in a new buffer
//SDL_RWops* ModuleFileSystem::LoadFile(const char* file) const
//{
//	char* buffer;
//	int size = Load(file, &buffer);
//
//	if (size > 0)
//	{
//		SDL_RWops* r = SDL_RWFromConstMem(buffer, size);
//		if (r != nullptr)
//			r->close = close_sdl_rwops;
//
//		return r;
//	}
//	else
//		return nullptr;
//}

// Save a whole buffer to disk
uint ModuleFileSystem::Save(const char* file, const void* buffer, unsigned int size, bool append) const
{
	unsigned int ret = 0;

	bool overwrite = PHYSFS_exists(file) != 0; //check if file already exists in file system
	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file); //open file (append or write only)

	if (fs_file != nullptr)
	{
		uint written = (uint)PHYSFS_write(fs_file, (const void*)buffer, 1, size); //write file to buffer and get size
		if (written != size) //check for error
		{
			LOG("File System error while writing to file %s: %s", file, PHYSFS_getLastError(), 'e');
		}
		else
		{
			if (append == true) //if append
			{
				LOG("Added %u data to [%s%s]", size, PHYSFS_getWriteDir(), file, 'd');
			}
			else if (overwrite == false) //if is new file
				LOG("New file created [%s%s] of %u bytes", PHYSFS_getWriteDir(), file, size, 'd');

			ret = written;
		}

		if (PHYSFS_close(fs_file) == 0) //close file
			LOG("File System error while closing file %s: %s", file, PHYSFS_getLastError(), 'e');
	}
	else
		LOG("File System error while opening file %s: %s", file, PHYSFS_getLastError(), 'e');

	return ret;
}

bool ModuleFileSystem::SaveUnique(string& name, const void * buffer, uint size, const char * path, const char * prefix, const char * extension)
{
	char result[250];

	sprintf_s(result, 250, "%s%s_%llu.%s", path, prefix, App->resources->GenerateUID(), extension); //put file data into new path
	NormalizePath(result); //normalize new path
	if (Save(result, buffer, size) > 0) //save to new path
	{
		name = result; //change source name to new name
		return true;
	}
	return false;
}

bool ModuleFileSystem::Remove(const char * file)
{
	bool ret = false;

	if (file != nullptr) //check if file is valid
	{
		if (PHYSFS_delete(file) == 0) //delete file
		{
			LOG("File deleted: [%s]", file, 'd');
			ret = true;
		}
		else
			LOG("File System error while trying to delete [%s]: ", file, PHYSFS_getLastError(), 'e');
	}

	return ret;
}

const char * ModuleFileSystem::GetReadPaths() const
{
	static char paths[512];
	paths[0] = '\0';

	char **path;
	for (path = PHYSFS_getSearchPath(); *path != nullptr; path++) //get curren search path
	{
		strcat_s(paths, 512, *path);
		strcat_s(paths, 512, "\n");
	}

	return paths;
}

PathNode ModuleFileSystem::GetAllFiles(const char* directory, std::vector<std::string>* filter_ext, std::vector<std::string>* ignore_ext) const
{
	PathNode root;
	if (Exists(directory))
	{
		root.path = directory;
		root.localPath = GetFileName(directory);
		if (root.localPath == "")
			root.localPath = directory;

		std::vector<string> file_list, dir_list;
		DiscoverFiles(directory, file_list, dir_list);

		//Adding all child directories
		for (uint i = 0; i < dir_list.size(); i++)
		{
			std::string str = directory;
			str.append("/").append(dir_list[i]);
			root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
		}
		//Adding all child files
		for (uint i = 0; i < file_list.size(); i++)
		{
			//Filtering extensions
			bool filter = true, discard = false;
			if (filter_ext != nullptr)
			{
				filter = HasExtension(file_list[i].c_str(), *filter_ext);
			}
			if (ignore_ext != nullptr)
			{
				discard = HasExtension(file_list[i].c_str(), *ignore_ext);
			}
			if (filter == true && discard == false)
			{
				std::string str = directory;
				str.append("/").append(file_list[i]);
				root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
			}
		}
		root.file = HasExtension(root.path.c_str());
		root.leaf = root.children.empty() == true;
	}
	return root;
}

bool ModuleFileSystem::HasExtension(const char* path) const
{
	std::string ext = "";
	ext = GetExtension(path);
	return ext != "";
}

bool ModuleFileSystem::HasExtension(const char* path, std::string extension) const
{
	std::string ext = "";
	ext = GetExtension(path);
	return ext == extension;
}

bool ModuleFileSystem::HasExtension(const char* path, std::vector<std::string> extensions) const
{
	std::string ext = "";
	ext = GetExtension(path);

	for (uint i = 0; i < extensions.size(); i++)
	{
		if (extensions[i] == ext)
			return true;
	}
	return false;
}
