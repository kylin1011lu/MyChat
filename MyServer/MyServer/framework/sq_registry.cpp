/*
 * \file: sq_registry.cpp
 * \brief: Created by hushouguo at Dec 11 2013 14:23:07
 */

#include "framework.h"

#define PUT_VALUE(KEY, VALUE_FORMAT_STRING, VALUE)\
	char buf[960];\
	snprintf(buf, sizeof(buf), VALUE_FORMAT_STRING, VALUE);\
	value_type_t::iterator i = values.find(key);\
	if (i != values.end()) { i->second = buf; }\
	else\
	{\
		return values.insert(std::make_pair(key, buf)).second;\
	}\
	return true;

bool sq_registry::push_value(const char* key, char value)
{
	PUT_VALUE(key, "%c", value);
}
bool sq_registry::push_value(const char* key, unsigned char value)
{
	PUT_VALUE(key, "%u", value);
}
bool sq_registry::push_value(const char* key, short value)
{
	PUT_VALUE(key, "%d", value);
}
bool sq_registry::push_value(const char* key, unsigned short value)
{
	PUT_VALUE(key, "%u", value);
}
bool sq_registry::push_value(const char* key, int value)
{
	PUT_VALUE(key, "%d", value);
}
bool sq_registry::push_value(const char* key, unsigned int value)
{
	PUT_VALUE(key, "%u", value);
}
bool sq_registry::push_value(const char* key, long value)
{
	PUT_VALUE(key, "%ld", value);
}
bool sq_registry::push_value(const char* key, unsigned long value)
{
	PUT_VALUE(key, "%lu", value);
}
bool sq_registry::push_value(const char* key, long long value)
{
	PUT_VALUE(key, "%lld", value);
}
bool sq_registry::push_value(const char* key, unsigned long long value)
{
	PUT_VALUE(key, "%llu", value);
}
bool sq_registry::push_value(const char* key, float value)
{
	PUT_VALUE(key, "%f", value);
}
bool sq_registry::push_value(const char* key, double value)
{
	PUT_VALUE(key, "%f", value);
}
bool sq_registry::push_value(const char* key, char* value)
{
	PUT_VALUE(key, "%s", value);
}
bool sq_registry::push_value(const char* key, const char* value)
{
	PUT_VALUE(key, "%s", value);
}
bool sq_registry::push_value(const char* key, std::string value)
{
	PUT_VALUE(key, "%s", value.c_str());
}
char sq_registry::get_value(const char* key, char default_value)
{
	const char* value = find_key(key);
	if (!value) { return default_value; }
	return atoi(value);
}
unsigned char sq_registry::get_value(const char* key, unsigned char default_value)
{
	const char* value = find_key(key);
	if (!value) { return default_value; }
	return atoi(value);
}
short sq_registry::get_value(const char* key, short default_value)
{
	const char* value = find_key(key);
	if (!value) { return default_value; }
	return atoi(value);
}
unsigned short sq_registry::get_value(const char* key, unsigned short default_value)
{
	const char* value = find_key(key);
	if (!value) { return default_value; }
	return atoi(value);
}
int sq_registry::get_value(const char* key, int default_value)
{
	const char* value = find_key(key);
	if (!value) { return default_value; }
	return atoi(value);
}
unsigned int sq_registry::get_value(const char* key, unsigned int default_value)
{
	const char* value = find_key(key);
	if (!value) { return default_value; }
	return strtoul(value, (char**)NULL, 10);
}
long sq_registry::get_value(const char* key, long default_value)
{
	const char* value = find_key(key);
	if (!value) { return default_value; }
	return strtol(value, (char**)NULL, 10);
}
unsigned long sq_registry::get_value(const char* key, unsigned long default_value)
{
	const char* value = find_key(key);
	if (!value) { return default_value; }
	return strtoul(value, (char**)NULL, 10);
}
//long long sq_registry::get_value(const char* key, long long default_value)
//{
//	const char* value = find_key(key);
//	if (!value) { return default_value; }
//	return strtoll(value, (char**)NULL, 10);
//}
//unsigned long long sq_registry::get_value(const char* key, unsigned long long default_value)
//{
//	const char* value = find_key(key);
//	if (!value) { return default_value; }
//	return strtoull(value, (char**)NULL, 10);
//}
//float sq_registry::get_value(const char* key, float default_value)
//{
//	const char* value = find_key(key);
//	if (!value) { return default_value; }
//	return strtof(value, (char**)NULL);
//}
double sq_registry::get_value(const char* key, double default_value)
{
	const char* value = find_key(key);
	if (!value) { return default_value; }
	return strtod(value, (char**)NULL);
}
char* sq_registry::get_value(const char* key, char* default_value)
{
	const char* value = find_key(key);
	if (!value) { return default_value; }
	return (char*)value;
}
const char* sq_registry::get_value(const char* key, const char* default_value)
{
	const char* value = find_key(key);
	if (!value) { return default_value; }
	return value;
}
const char* sq_registry::find_key(const char* key)
{
	if (!key) { return NULL; }
	value_type_t::iterator i = values.find(key);
	return i == values.end() ? NULL : i->second.c_str();
}
bool sq_registry::delete_key(const char* key)
{
	value_type_t::iterator i = values.find(key);
	if (i != values.end())
	{
		values.erase(i);
		return true;
	}
	return false;
}
void sq_registry::clear()
{
	values.clear();
}
void sq_registry::print()
{
	debug_log("configure:\n");
	for (value_type_t::iterator i = values.begin(); i != values.end(); ++i)
	{
		debug_log("\t%-s  ", i->first.c_str());
		uint32_t len = strlen(i->first.c_str());
		const uint32_t totalTitle = 40;
		for (uint32_t n = 0; n < (totalTitle > len ? (totalTitle - len) : 0); ++n)
		{
			note(".");
		}
		note(" %s\n", i->second.c_str());
	}
}

sq_registry conf;
