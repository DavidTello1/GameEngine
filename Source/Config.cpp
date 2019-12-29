#include "Config.h"

Config::Config(JSON_Object* obj) : node(obj)
{
}

//Contructor used for data append
Config::Config()
{
	root_value = json_value_init_object();
	node = json_value_get_object(root_value);
}

//Constructor used for data read
Config::Config(const char* buffer)
{
	root_value = json_parse_string(buffer);
	if (root_value)
		node = json_value_get_object(root_value);
}

Config::~Config()
{
	Release();
}

uint Config::Serialize(char** buffer)
{
	size_t size = json_serialization_size_pretty(root_value);
	*buffer = new char[size];
	json_serialize_to_buffer_pretty(root_value, *buffer, size);
	return size;
}

void Config::Release()
{
	if (root_value)
	{
		json_value_free(root_value);
	}
}

Config Config::SetNode(const char* name)
{
	json_object_set_value(node, name, json_value_init_object());
	return Config(json_object_get_object(node, name));
}

double Config::GetNumber(const char* name, double default) const
{
	if (json_object_has_value_of_type(node, name, JSONNumber))
		return json_object_get_number(node, name);
	return default;
}

std::string Config::GetString(const char* name, const char* default) const
{
	if (json_object_has_value_of_type(node, name, JSONString))
		return json_object_get_string(node, name);
	return default;
}

bool Config::GetBool(const char* name, bool default) const
{
	if (json_object_has_value_of_type(node, name, JSONBoolean))
		return json_object_get_boolean(node, name);
	return default;
}