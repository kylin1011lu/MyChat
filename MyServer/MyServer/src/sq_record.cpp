/*
 * \file: sq_record.cpp
 * \brief: Created by hushouguo at Jul 08 2014 15:55:46
 */

#include "framework.h"
#include "sq_record.h"

struct db_filelds
{
	int64_t id;
	char db_data[65*KB];
	char db_var[65*KB];
	char db_offline_event[65*KB];
	char db_mail[65*KB];
	char db_pve[65*KB];
	char db_friends[65*KB];
	char db_village_model[65*KB];
	char db_temp_data[65*KB];
};

db_filelds db_buf;

sq_record* sq_record_init()
{
	sq_record* record = new sq_record;
	for (int n = 0; n < 8; ++n)
	{
		record->conf[n] = new sq_mysql_config;
	}
	record->init_handle = 1;

	for (int n = 0; n < 8; ++n)
	{
		char p[96];
		snprintf(p, sizeof(p), "root.db.db%d.host", n);
		const char* host = conf.get_value(p, (const char*)NULL);
		snprintf(p, sizeof(p), "root.db.db%d.user", n);
		const char* user = conf.get_value(p, (const char*)NULL);
		snprintf(p, sizeof(p), "root.db.db%d.passwd", n);
		const char* passwd = conf.get_value(p, (const char*)NULL);
		snprintf(p, sizeof(p), "root.db.db%d.db", n);
		const char* db = conf.get_value(p, (const char*)NULL);
		snprintf(p, sizeof(p), "root.db.db%d.port", n);
		int port = conf.get_value(p, 0);

		record->conf[n]->init(host, user, passwd, db, port);
	}
	
	return record;
}
sq_record* sq_record_init(const char* conf_file)
{
	sq_record* record = new sq_record;
	for (int n = 0; n < 8; ++n)
	{
		record->conf[n] = new sq_mysql_config;
	}
	record->init_handle = 1;

	sq_xml_parser* xml_parser = new sq_xml_parser(conf_file);

	xml_node_ptr root = xml_parser->root_node();
	if (!root) { return NULL; }

	for (int n = 0; n < 8; ++n)
	{
		char p[32];
		snprintf(p, sizeof(p), "db%d", n);
		xml_node_ptr db_node = xml_parser->child_node(root, p);
		if (!db_node)
		{
			error_log("not found `%s` node\n", p);
			return NULL;
		}

		const char* host = xml_parser->node_value_string(db_node, "host");
		if (!host)
		{
			error_log("not found `%s.host` node\n", p);
			return NULL;
		}

		const char* user = xml_parser->node_value_string(db_node, "user");
		if (!user)
		{
			error_log("not found `%s.user` node\n", p);
			return NULL;
		}

		const char* passwd = xml_parser->node_value_string(db_node, "passwd");
		if (!passwd)
		{
			error_log("not found `%s.passwd` node\n", p);
			return NULL;
		}

		const char* db = xml_parser->node_value_string(db_node, "db");
		if (!db)
		{
			error_log("not found `%s.db` node\n", p);
			return NULL;
		}

		int port = xml_parser->node_value_int(db_node, "port");
		if (!port)
		{
			error_log("not found `%s.port` node\n", p);
			return NULL;
		}

		record->conf[n]->init(host, user, passwd, db, port);
	}

	SQ_SAFE_DELETE(xml_parser);
	
	return record;
}
void sq_record_destroy(sq_record* record)
{
	SQ_SAFE_DELETE(record);
}

sq_record_entry* sq_record_data_init(sq_record* record, const char* table_name)
{
	sq_record_entry* entry = new sq_record_entry;
	for (int n = 0; n < 8; ++n)
	{
		if (!entry->db[n]->open(record->conf[n]))
		{
			delete entry;
			return 0;
		}
	}
	strncpy(entry->table_name, table_name, sizeof(entry->table_name));

	if (!record->entry_table.insert(std::make_pair(record->init_handle, entry)).second)
	{
		delete entry;
		return 0;
	}

	return entry;
}
