/*
 * \file: sq_xml_parser.h
 * \brief: Created by hushouguo at 18:10:21 Dec 26 2011
 */
 
#ifndef __SQ_XML_PARSER_H__
#define __SQ_XML_PARSER_H__

typedef rapidxml::xml_node<>*				xml_node_ptr;
struct sq_xml_parser
{
	char*									buf;
	rapidxml::xml_document<>				doc;
	sq_xml_parser(const char* filename);
	~sq_xml_parser();
	bool                            		open(const char* filename);
	xml_node_ptr							root_node();
	xml_node_ptr							next_node(xml_node_ptr node);
	xml_node_ptr							next_node(xml_node_ptr node, const char* nodename);
	xml_node_ptr							child_node(xml_node_ptr node);
	xml_node_ptr							child_node(xml_node_ptr node, const char* nodename);
	int										node_value_int(xml_node_ptr node, const char* valuename);
	long long								node_value_longint(xml_node_ptr node, const char* valuename);
	float									node_value_float(xml_node_ptr node, const char* valuename);
	bool									node_value_bool(xml_node_ptr node, const char* valuename);
	const char*								node_value_string(xml_node_ptr node, const char* valuename);

	void									make_registry(sq_registry* registry, xml_node_ptr node, std::string name);
	void 									make_registry(sq_registry* registry, const char* prefix = "");
};

#endif
