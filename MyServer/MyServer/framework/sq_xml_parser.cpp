/*
 * \file: sq_xml_parser.cpp
 * \brief: Created by hushouguo at 18:10:24 Dec 26 2011
 */

#include "framework.h"

xml_node_ptr sq_xml_parser::root_node()
{
	return doc.first_node();
}
xml_node_ptr sq_xml_parser::next_node(xml_node_ptr node)
{
	return node->next_sibling();
}
xml_node_ptr sq_xml_parser::next_node(xml_node_ptr node, const char* nodename)
{
	return node->next_sibling(nodename);
}
xml_node_ptr sq_xml_parser::child_node(xml_node_ptr node)
{
	return node->first_node();
}
xml_node_ptr sq_xml_parser::child_node(xml_node_ptr node, const char* nodename)
{
	//xml_node_ptr firstnode = node->first_node();
	//debug_log("name:%s\n", firstnode->name());
	return node->first_node(nodename);
}
int	sq_xml_parser::node_value_int(xml_node_ptr node, const char* valuename)
{
	rapidxml::xml_attribute<>* attr = node->first_attribute(valuename);
	return attr != NULL ? strtol((char*)attr->value(), (char**)NULL, 10) : 0;
}
long long sq_xml_parser::node_value_longint(xml_node_ptr node, const char* valuename)
{
	rapidxml::xml_attribute<>* attr = node->first_attribute(valuename);
#ifdef WIN32
	return attr != NULL ? _strtoi64((char*)attr->value(), (char**)NULL, 10) : 0ULL;
#else
	return attr != NULL ? strtoll((char*)attr->value(), (char**)NULL, 10) : 0ULL;
#endif
}
float sq_xml_parser::node_value_float(xml_node_ptr node, const char* valuename)
{
	rapidxml::xml_attribute<>* attr = node->first_attribute(valuename);
	return attr != NULL ? strtod((char*)attr->value(), (char**)NULL) : 0.f;
}
bool sq_xml_parser::node_value_bool(xml_node_ptr node, const char* valuename)
{
	const char* s = node_value_string(node, valuename);
	if (!strcmp(s, "true")) { return true; }
	return false;
}
const char* sq_xml_parser::node_value_string(xml_node_ptr node, const char* valuename)
{
	rapidxml::xml_attribute<>* attr = node->first_attribute(valuename);
	return attr != NULL ? attr->value() : NULL;
}
sq_xml_parser::sq_xml_parser(const char* filename)
	: buf(NULL)
{
	open(filename);
}
sq_xml_parser::~sq_xml_parser()
{
	doc.clear();
	if (buf) { delete buf; }
}
bool sq_xml_parser::open(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if (!fp)
	{
		error_log("open file error:%s\n", filename);
		return false;
	}

	fseek (fp, 0, SEEK_END);
	size_t size = ftell(fp);
	rewind (fp);

	this->buf = new char[size+1];
	size_t n = fread (buf, 1, size, fp);
	if (n != size)
	{
		error_log("read file error:%s\n", filename);
		return false;
	}

	buf[size] = '\0';
	doc.parse<0>(buf);

	fclose(fp);

	return true;
}
void sq_xml_parser::make_registry(sq_registry* registry, xml_node_ptr node, std::string name)
{
	if (!node) { return; }

	for (rapidxml::xml_attribute<char>* cur = node->first_attribute(); cur; cur = cur->next_attribute())
	{    
		if (node == root_node())
		{
			char buf[1024];
			snprintf(buf, sizeof(buf), "%s%s.%s", 
					name.c_str(), (const char*)node->name(), (const char*)cur->name()
					);
			if (!registry->push_value(buf, (const char*)cur->value()))
			{
				panic("duplicate key:%s\n", cur->name());
			}
		}
		else
		{
			char buf[1024];
			snprintf(buf, sizeof(buf), "%s.%s.%s", 
					name.c_str(), (const char*)node->name(), (const char*)cur->name()
					);
			if (!registry->push_value(buf, (const char*)cur->value()))
			{
				panic("duplicate key:%s\n", cur->name());
			}
		}
	}
	
	if (node == root_node())
	{
		make_registry(registry, child_node(node), name);
	}
	else
	{
		if (name.length() > 0)
		{
			make_registry(registry, child_node(node), name + "." + (const char*)node->name());
		}
		else
		{
			make_registry(registry, child_node(node), name + (const char*)node->name());
		}
	}

	make_registry(registry, next_node(node), name);
}
void sq_xml_parser::make_registry(sq_registry* registry, const char* prefix)
{
	make_registry(registry, root_node(), prefix);
}

