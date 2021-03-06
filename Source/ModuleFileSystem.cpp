#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "PathNode.h"

#include "PhysFS/include/physfs.h"
#include <fstream>
#include <filesystem>

#include "Assimp/include/cfileio.h"
#include "Assimp/include/types.h"
#include "SDL/include/SDL.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

#include "mmgr/mmgr.h"

using namespace std;

ModuleFileSystem::ModuleFileSystem(const char* game_path) : Module("FileSystem", true)
{
	// needs to be created before Init so other modules can use it
	LOG("Loading PhysFS", 'd');
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	// workaround VS string directory mess
	AddPath(".");
	if (0 && game_path != nullptr)
		AddPath(game_path);

	LOG("FileSystem Operations base is [%s] plus:", GetBasePath(), 'd');
	LOG(GetReadPaths());

	if (PHYSFS_setWriteDir(".") == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError(), 'e');

	// Create Standard Paths
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
		if (!Exists(dirs[i]))
			CreateFolder(dirs[i]);
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
	if (PHYSFS_mount(path_or_zip, nullptr, 1) == 0)
	{
		LOG("File System error while adding a path or zip: %s\n", PHYSFS_getLastError(), 'e');
		return false;
	}
	
	return true;
}

void ModuleFileSystem::GetFolderContent(const char* directory, vector<string>& file_list, vector<string>& dir_list) const
{
	char **rc = PHYSFS_enumerateFiles(directory); //get base directory
	char **i;

	string dir(directory);
	for (i = rc; *i != nullptr; i++)
	{
		string str = string(directory) + string("/") + string(*i);
		if (IsFolder(str.c_str()))
			dir_list.push_back(*i);
		else
			file_list.push_back(*i);
	}

	PHYSFS_freeList(rc);
}

bool ModuleFileSystem::CopyFromOutside(const char* full_path, const char* destination) // Only place we acces non virtual filesystem
{
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

		LOG("File System copied file [%s] to [%s]", full_path, destination, 'd');
		return true;
	}
	else
		LOG("File System error while copy from [%s] to [%s]", full_path, destination, 'e');

	return false;
}

bool ModuleFileSystem::Copy(const char* full_path, const char* destination)
{
	char buf[8192];

	PHYSFS_file* source = PHYSFS_openRead(full_path); //open source file (only read)
	PHYSFS_file* dest = PHYSFS_openWrite(destination); //open destination file (only write)

	PHYSFS_sint32 size;
	if (source && dest) //check for error
	{
		while (size = (PHYSFS_sint32)PHYSFS_read(source, buf, 1, 8192))
			PHYSFS_write(dest, buf, 1, size); //copy from source to destination file

		PHYSFS_close(source); //close source file
		PHYSFS_close(dest); //close destination file

		LOG("File System copied file [%s] to [%s]", full_path, destination, 'd');
		return true;
	}
	else
		LOG("File System error while copy from [%s] to [%s]", full_path, destination, 'e');

	return false;
}

string ModuleFileSystem::GetFileName(const char* path) const
{
	const char* file_name = strrchr(path, 128);
	if (file_name == nullptr) 
		file_name = (strrchr(path, '/') != nullptr) ? strrchr(path, '/') : "";
	file_name++;
	return file_name;
}

string ModuleFileSystem::GetExtension(const char* path) const
{
	char buffer[32] = "";
	const char* last_dot = strrchr(path, '.');
	if (last_dot != nullptr)
		strcpy_s(buffer, last_dot + 1);

	string extension(buffer);
	return extension;
}

void ModuleFileSystem::NormalizePath(string& full_path) const
{
	for (string::iterator it = full_path.begin(); it != full_path.end(); ++it)
	{
		if (*it == '\\')
			*it = '/';
	}
}

void ModuleFileSystem::ToLower(string& full_path) const
{
	for (string::iterator it = full_path.begin(); it != full_path.end(); ++it)
	{
		*it = tolower(*it);
	}
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

// Save a whole buffer to disk
uint ModuleFileSystem::Save(const char* file, const void* buffer, unsigned int size, bool append) const
{
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

			return written;
		}

		if (PHYSFS_close(fs_file) == 0) //close file
			LOG("File System error while closing file %s: %s", file, PHYSFS_getLastError(), 'e');
	}
	else
		LOG("File System error while opening file %s: %s", file, PHYSFS_getLastError(), 'e');

	return 0;
}

bool ModuleFileSystem::Remove(const char * file)
{

	if (file != nullptr) //check if file is valid
	{
		if (PHYSFS_delete(file) == 0) //delete file
		{
			LOG("File deleted: [%s]", file, 'd');
			return true;
		}
		else
			LOG("File System error while trying to delete [%s]: ", file, PHYSFS_getLastError(), 'e');
	}
	return false;
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

PathNode ModuleFileSystem::GetAllFiles(const char* directory, vector<string>* filter_ext, vector<string>* ignore_ext) const
{
	PathNode root;
	if (Exists(directory)) //check if directory exists
	{
		root.path = directory;
		root.file_name = GetFileName(directory);
		if (root.file_name == "")
			root.file_name = directory;

		if (IsFolder(directory))
		{
			vector<string> file_list, dir_list;
			GetFolderContent(directory, file_list, dir_list);

			//Adding all child directories
			for (uint i = 0; i < dir_list.size(); i++)
			{
				string str = directory + string("/") + dir_list[i];
				root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
			}

			//Adding all child files
			for (uint i = 0; i < file_list.size(); i++)
			{
				bool filter = true, discard = false;
				if (filter_ext != nullptr)
				{
					filter = CheckExtension(file_list[i].c_str(), *filter_ext); //check if file_ext == filter_ext
				}
				else if (ignore_ext != nullptr)
				{
					discard = CheckExtension(file_list[i].c_str(), *ignore_ext); //check if file_ext == ignore_ext
				}

				if (filter == true && discard == false)
				{
					string str = directory + string("/") + file_list[i];
					root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
				}
			}
			root.file = false;
			root.leaf = root.children.empty() == true;
		}
		else
		{
			root.file = true;
			root.leaf = true;
		}
	}
	return root;
}

bool ModuleFileSystem::CheckExtension(const char* path, vector<string> extensions) const
{
	string ext = "";
	ext = GetExtension(path);

	for (uint i = 0; i < extensions.size(); i++)
	{
		if (extensions[i] == ext)
			return true;
	}
	return false;
}


// -----------------------------------------------------
// ASSIMP IO
// -----------------------------------------------------

size_t AssimpWrite(aiFile* file, const char* data, size_t size, size_t chunks)
{
	PHYSFS_sint64 ret = PHYSFS_write((PHYSFS_File*)file->UserData, (void*)data, size, chunks);
	if (ret == -1)
		LOG("File System error while WRITE via assimp: %s", PHYSFS_getLastError());

	return (size_t)ret;
}

size_t AssimpRead(aiFile* file, char* data, size_t size, size_t chunks)
{
	PHYSFS_sint64 ret = PHYSFS_read((PHYSFS_File*)file->UserData, (void*)data, size, chunks);
	if (ret == -1)
		LOG("File System error while READ via assimp: %s", PHYSFS_getLastError());

	return (size_t)ret;
}

size_t AssimpTell(aiFile* file)
{
	PHYSFS_sint64 ret = PHYSFS_tell((PHYSFS_File*)file->UserData);
	if (ret == -1)
		LOG("File System error while TELL via assimp: %s", PHYSFS_getLastError());

	return (size_t)ret;
}

size_t AssimpSize(aiFile* file)
{
	PHYSFS_sint64 ret = PHYSFS_fileLength((PHYSFS_File*)file->UserData);
	if (ret == -1)
		LOG("File System error while SIZE via assimp: %s", PHYSFS_getLastError());

	return (size_t)ret;
}

void AssimpFlush(aiFile* file)
{
	if (PHYSFS_flush((PHYSFS_File*)file->UserData) == 0)
		LOG("File System error while FLUSH via assimp: %s", PHYSFS_getLastError());
}

aiReturn AssimpSeek(aiFile* file, size_t pos, aiOrigin from)
{
	int res = 0;

	switch (from)
	{
	case aiOrigin_SET:
		res = PHYSFS_seek((PHYSFS_File*)file->UserData, pos);
		break;
	case aiOrigin_CUR:
		res = PHYSFS_seek((PHYSFS_File*)file->UserData, PHYSFS_tell((PHYSFS_File*)file->UserData) + pos);
		break;
	case aiOrigin_END:
		res = PHYSFS_seek((PHYSFS_File*)file->UserData, PHYSFS_fileLength((PHYSFS_File*)file->UserData) + pos);
		break;
	}

	if (res == 0)
		LOG("File System error while SEEK via assimp: %s", PHYSFS_getLastError());

	return (res != 0) ? aiReturn_SUCCESS : aiReturn_FAILURE;
}

aiFile* AssimpOpen(aiFileIO* io, const char* name, const char* format)
{
	static aiFile file;

	file.UserData = (char*)PHYSFS_openRead(name);
	file.ReadProc = AssimpRead;
	file.WriteProc = AssimpWrite;
	file.TellProc = AssimpTell;
	file.FileSizeProc = AssimpSize;
	file.FlushProc = AssimpFlush;
	file.SeekProc = AssimpSeek;

	return &file;
}

void AssimpClose(aiFileIO* io, aiFile* file)
{
	if (PHYSFS_close((PHYSFS_File*)file->UserData) == 0)
		LOG("File System error while CLOSE via assimp: %s", PHYSFS_getLastError());
}

void ModuleFileSystem::CreateAssimpIO()
{
	RELEASE(AssimpIO);

	AssimpIO = new aiFileIO;
	AssimpIO->OpenProc = AssimpOpen;
	AssimpIO->CloseProc = AssimpClose;
}

aiFileIO * ModuleFileSystem::GetAssimpIO()
{
	return AssimpIO;
}