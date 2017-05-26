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
	for (int n = 0; n < MAX_DB_NUMBER; ++n)
	{
		record->conf[n] = new sq_mysql_config;
	}

	for (int n = 0; n < MAX_DB_NUMBER; ++n)
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

	const char* host = conf.get_value("root.db.dba.host", (const char*)NULL);
	const char* user = conf.get_value("root.db.dba.user", (const char*)NULL);
	const char* passwd = conf.get_value("root.db.dba.passwd", (const char*)NULL);
	const char* db = conf.get_value("root.db.dba.db", (const char*)NULL);
	int port = conf.get_value("root.db.dba.port", 0);

	record->acct_conf = new sq_mysql_config;
	record->acct_conf->init(host, user, passwd, db, port);
	
	return record;
}

void sq_record_destroy(sq_record* record)
{
	SQ_SAFE_DELETE(record);
}

sq_record_entry* sq_record_data_init(sq_record* record, const char* table_name)
{
	sq_record_entry* entry = new sq_record_entry;
	for (int n = 0; n < MAX_DB_NUMBER; ++n)
	{
		if (!entry->db[n]->open(record->conf[n]))
		{
			delete entry;
			return 0;
		}
	}
	if (!entry->db_acct->open(record->acct_conf))
	{
		delete entry;
		return 0;
	}
	strncpy(entry->acct_table, table_name, sizeof(entry->acct_table));

	return entry;
}

bool sq_record_data_select(sq_record* record, sq_record_entry* entry, const char* name, char* d, size_t& size)
{
	if (!entry)
	{
		return false;
	}

	char buf[960] = { 0 };
	MYSQL_BIND result[1];
	unsigned long lengths[1] = { 0 };
	sq_mysql*db = entry->db_acct;

	snprintf(buf, sizeof(buf), "SELECT name FROM `%s` WHERE name = '%s'", entry->acct_table, name);
	if (!db->prepare(buf)){ return false; }
	if (!db->exec()){ return false; }

	memset(result, 0, sizeof(result));

	result[0].buffer_type = MYSQL_TYPE_STRING;
	result[0].buffer = (char*)d;
	result[0].buffer_length = size; 
	result[0].length = &lengths[0];

	if (!db->bind_result(result)){ return false; }
	if (!db->fetch()) { return false; }
	if (!db->free_result()) { return false; }

	size = lengths[0];
	return true;
}

bool sq_record_data_select(sq_record* record, sq_record_entry* entry, uint32_t id, char* name, char*pwd, size_t& size)
{
	if (!entry)
	{
		return false;
	}

	char buf[960] = { 0 };
	MYSQL_BIND result[2];
	unsigned long lengths[2] = { 0 };
	sq_mysql*db = entry->db_acct;

	snprintf(buf, sizeof(buf), "SELECT name,pwd FROM `%s` WHERE id = %d", entry->acct_table, id);
	if (!db->prepare(buf)){ return false; }
	if (!db->exec()){ return false; }

	memset(result, 0, sizeof(result));

	result[0].buffer_type = MYSQL_TYPE_STRING;
	result[0].buffer = (char*)name;
	result[0].buffer_length = size;
	result[0].length = &lengths[0];


	result[1].buffer_type = MYSQL_TYPE_STRING;
	result[1].buffer = (char*)pwd;
	result[1].buffer_length = size;
	result[1].length = &lengths[1];

	if (!db->bind_result(result)){ return false; }
	if (!db->fetch()) { return false; }
	if (!db->free_result()) { return false; }

	size = lengths[0];
	return true;
}

bool sq_record_data_insert(sq_record* record, sq_record_entry* entry, uint32_t id, const char* name,const char*pwd)
{
	if (!entry)
	{
		return false;
	}

	char buf[960] = {0};
	sq_mysql*db = entry->db_acct;

	snprintf(buf, sizeof(buf), "INSERT INTO `%s` (id,name,pwd) VALUES(%d,'%s','%s')", entry->acct_table, id,name,pwd);
	if (!db->prepare(buf)){ return false; }
	if (!db->exec()){ return false; }

	return true;
}
