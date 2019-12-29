#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "parson\parson.h"
#include <string>
#include "Globals.h"
#include <vector>

#include "MathGeoLib/include/MathBuildConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"


//http://kgabis.github.io/parson/

struct json_object_t;
typedef struct json_object_t JSON_Object;

struct json_value_t;
typedef struct json_value_t  JSON_Value;

struct json_array_t;
typedef struct json_array_t  JSON_Array;

class Config_Array;
class Config
{
	///Brief parson explanation
	/*
	-	Save a JSON file into a buffer then create a new file with fileSystem:
		JSON_Status json_serialize_to_buffer(const JSON_Value *value, char *buf, size_t buf_size_in_bytes);
	-	JSON nodes are called objects
	-	Append attributes to object: json_object_set_[type of attribute]. Attribute types are bool, number, string and value (any attribute or an object)
	-	Get values from a node: same function as append, but with "get"
	*/

public:
	Config();						//Contructor used for new files
	Config(const char* buffer);		//Constructor used for data read
	Config(JSON_Object* obj);		//Constructor used for node append
	~Config();						//Free data if initialized

	uint Serialize(char** buffer);	//Returns a filled buffer
	bool Exists() { return root_value != nullptr; }
	void Release();

	Config SetNode(const char* name);
	void SetNumber(const char* name, double data) { json_object_set_number(node, name, data); }
	void SetString(const char* name, const char* data) { json_object_set_string(node, name, data); }
	void SetBool(const char* name, bool data) { json_object_set_boolean(node, name, data); }

	Config GetNode(const char* name) const { return Config(json_object_get_object(node, name)); }
	double GetNumber(const char* name, double default = 0) const;
	std::string GetString(const char* name, const char* default = "") const;
	bool GetBool(const char* name, bool default = true) const;

private:
	JSON_Value* root_value = nullptr; //Only used for file root
	JSON_Object* node = nullptr;
};

#endif //__CONFIG_H__
