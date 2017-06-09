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

	record->main_conf = new sq_mysql_config;
	record->main_conf->init(host, user, passwd, db, port);
	
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
	if (!entry->db_main->open(record->main_conf))
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
	sq_mysql*db = entry->db_main;

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
	sq_mysql*db = entry->db_main;

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

bool sq_record_data_select(sq_record* record, sq_record_entry* entry, uint32_t last_time, message::ChatHistoryResponse & response)
{
	if (!entry)
	{
		return false;
	}
	char buf[960] = { 0 };// "SELECT send_userid,send_time,chat_content FROM message WHERE send_time <= \"2017-06-01\"";
	MYSQL_BIND result[3];
	unsigned long lengths[3] = { 0 };

	int useid;
	MYSQL_TIME ts;
	char content[CHAT_CONTENT_SIZE];

	struct tm _tm;
	time_t t = last_time;
	localtime_r(&t, &_tm);

	mktime(&_tm);

	sq_mysql*db = entry->db_main;

	snprintf(buf, sizeof(buf), "SELECT send_userid,send_time,chat_content FROM message WHERE send_time <= \"%04d-%02d-%02d %02d:%02d:%02d\" ORDER BY send_time DESC", _tm.tm_year+1900, _tm.tm_mon+1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
	if (!db->prepare(buf)){ return false; }
	if (!db->exec()){ return false; }

	memset(result, 0, sizeof(result));

	result[0].buffer_type = MYSQL_TYPE_LONG;
	result[0].buffer = (char*)&useid;
	result[0].length = &lengths[0];

	result[1].buffer_type = MYSQL_TYPE_TIMESTAMP;
	result[1].buffer = (char*)&ts;
	result[1].length = &lengths[1];

	result[2].buffer_type = MYSQL_TYPE_STRING;
	result[2].buffer = (char*)content;
	result[2].buffer_length = CHAT_CONTENT_SIZE;
	result[2].length = &lengths[2];

	if (!db->bind_result(result)){ return false; }
	
	struct tm chat_time;

	while (!db->continue_fetch())
	{
		message::MessageInfo *info = response.add_message();
		info->set_chat_content(content);
		info->set_send_userid(useid);
		info->set_send_time(sql_mktime(ts));
		//debug_log("%d(%ld) ", useid, lengths[0]);
		//debug_log(" %04d-%02d-%02d %02d:%02d:%02d (%ld)",
		//	ts.year, ts.month, ts.day,
		//	ts.hour, ts.minute, ts.second, lengths[1]);
		//debug_log(" %s(%ld)\n", content, lengths[2]);
	}

	if (!db->free_result()) { return false; }

	return true;
}

bool sq_record_data_insert(sq_record* record, sq_record_entry* entry, uint32_t id, const char* name,const char*pwd)
{
	if (!entry)
	{
		return false;
	}

	char buf[960] = {0};
	sq_mysql*db = entry->db_main;

	snprintf(buf, sizeof(buf), "INSERT INTO `%s` (id,name,pwd) VALUES(%d,'%s','%s')", entry->acct_table, id,name,pwd);
	if (!db->prepare(buf)){ return false; }
	if (!db->exec()){ return false; }

	return true;
}

bool sq_record_data_insert(sq_record* record, sq_record_entry* entry, uint32_t id, const char* chat_content)
{
	if (!entry)
	{
		return false;
	}

	char buf[960] = { 0 };
	sq_mysql*db = entry->db_main;

	snprintf(buf, sizeof(buf), "INSERT INTO `message` (send_userid,send_time,chat_content) VALUES(%d,now(),'%s')", id, chat_content);
	if (!db->prepare(buf)){ return false; }
	if (!db->exec()){ return false; }

	return true;
}
