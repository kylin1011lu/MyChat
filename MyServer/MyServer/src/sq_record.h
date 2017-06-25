/*
 * \file: sq_record.h
 * \brief: Created by hushouguo at Jul 08 2014 15:28:57
 */
 
#ifndef __SQ_RECORD_H__
#define __SQ_RECORD_H__

#define MAX_RECORD_SIZE							4*MB
#define MAX_TABLE_NAME							960

//extern char db_data[MAX_RECORD_SIZE];
//extern char db_var[4*MB];
//extern char db_offline_event[65*KB];

struct sq_record_entry
{
	sq_mysql*									db_main;
	sq_mysql* 									db[MAX_DB_NUMBER];

	char										acct_table[MAX_TABLE_NAME];
	sq_record_entry()
	{
		for (int n = 0; n < 8; ++n)
		{
			this->db[n] = new sq_mysql;
		}
		this->db_main = new sq_mysql;
	}
	~sq_record_entry()
	{
		for (int n = 0; n < 8; ++n)
		{
			SQ_SAFE_DELETE(this->db[n]);
		}

		SQ_SAFE_DELETE(this->db_main);
	}
};

struct sq_record
{
	sq_mysql_config*							main_conf;
	sq_mysql_config*							conf[MAX_DB_NUMBER];

	sq_record()
	{ 
		memset(conf, 0, sizeof(conf)); 
	}
	~sq_record()
	{
		for (int n = 0; n < 8; ++n)
		{
			SQ_SAFE_DELETE(this->conf[n]);
		}
	}
};

sq_record* sq_record_init();
void sq_record_destroy(sq_record* record);

sq_record_entry* sq_record_data_init(sq_record* record, const char* table_name);


bool sq_record_data_select(sq_record* record, sq_record_entry* entry, const char* name, char* d, size_t& size);
bool sq_record_data_select(sq_record* record, sq_record_entry* entry, uint32_t id, char* name,char*pwd, size_t& size);
bool sq_record_data_select(sq_record* record, sq_record_entry* entry, uint32_t last_time, message::ChatResponse & response);
bool sq_record_data_select(sq_record* record, sq_record_entry* entry, uint32_t id, uint32_t *last_time);
bool sq_record_data_insert(sq_record* record, sq_record_entry* entry, uint32_t id, const char* name, const char*pwd);
bool sq_record_data_insert(sq_record* record, sq_record_entry* entry, uint32_t id, const char* chat_content,uint32_t insert_time);

#endif
