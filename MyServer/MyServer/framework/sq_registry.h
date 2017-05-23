/*
 * \file: sq_registry.h
 * \brief: Created by hushouguo at Dec 11 2013 14:15:50
 */
 
#ifndef __SQ_REGISTRY_H__
#define __SQ_REGISTRY_H__

struct sq_registry
{
	//typedef __gnu_cxx::hash_map<std::string, std::string> value_type_t;
	typedef std::map<std::string, std::string>		value_type_t;
	value_type_t 									values;

	bool 											push_value(const char* key, char value);
	bool 											push_value(const char* key, unsigned char value);
	bool 											push_value(const char* key, short value);
	bool 											push_value(const char* key, unsigned short value);
	bool 											push_value(const char* key, int value);
	bool 											push_value(const char* key, unsigned int value);
	bool 											push_value(const char* key, long value);
	bool 											push_value(const char* key, unsigned long value);
	bool 											push_value(const char* key, long long value);
	bool 											push_value(const char* key, unsigned long long value);
	bool 											push_value(const char* key, float value);
	bool 											push_value(const char* key, double value);
	bool 											push_value(const char* key, char* value);
	bool 											push_value(const char* key, const char* value);
	bool 											push_value(const char* key, std::string value);

	char 											get_value(const char* key, char default_value);
	unsigned char 									get_value(const char* key, unsigned char default_value);
	short 											get_value(const char* key, short default_value);
	unsigned short 									get_value(const char* key, unsigned short default_value);
	int 											get_value(const char* key, int default_value);
	unsigned int 									get_value(const char* key, unsigned int default_value);
	long 											get_value(const char* key, long default_value);
	unsigned long 									get_value(const char* key, unsigned long default_value);
	//long long 										get_value(const char* key, long long default_value);
	//unsigned long long 								get_value(const char* key, unsigned long long default_value);
	//float 											get_value(const char* key, float default_value);
	double 											get_value(const char* key, double default_value);
	char* 											get_value(const char* key, char* default_value);
	const char* 									get_value(const char* key, const char* default_value);

	const char*										find_key(const char* key);
	bool 											delete_key(const char* key);

	void 											clear();
	void 											print();
};

extern sq_registry conf;

#endif
