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
	sq_mysql* 									db[8];
	char										table_name[MAX_TABLE_NAME];
	sq_record_entry()
	{
		for (int n = 0; n < 8; ++n)
		{
			this->db[n] = new sq_mysql;
		}
	}
	~sq_record_entry()
	{
		for (int n = 0; n < 8; ++n)
		{
			SQ_SAFE_DELETE(this->db[n]);
		}
	}
};

struct sq_record
{
	sq_mysql_config*							conf[8];
	int											init_handle;
	__gnu_cxx::hash_map<int, sq_record_entry*>	entry_table;
	sq_record() : init_handle(1) { memset(conf, 0, sizeof(conf)); }
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

//struct _db_user_;
//
//typedef _db_user_* (*DB_CALLBACK_FUNC)(user_record_entry* data, 
//	user_var_entry* var, user_vector_data<int>* offline_event, user_mail_entry* mail, 
//	user_pve_record_entry* pve, user_friends_record_entry* friends, 
//	NodeArray<building_model_data>* village_model,NodeArray<user_temp_data,std::string>* temp_data, int64_t);

sq_record_entry* sq_record_data_init(sq_record* record, const char* table_name);
//bool sq_record_data_create(sq_record* record, sq_record_entry* entry, int64_t id, const void* d, size_t size);
//bool sq_record_data_delete(sq_record* record, sq_record_entry* entry, int64_t id);
//bool sq_record_data_select(sq_record* record, sq_record_entry* entry, int64_t id, void* d, size_t& size);
//bool sq_record_data_select(sq_record* record, sq_record_entry* entry, int64_t id, user_record_entry* &user_data, 
//	user_var_entry* &user_var, user_vector_data<int>* &user_event, user_mail_entry* &user_mail, 
//	user_pve_record_entry* &user_pve, user_friends_record_entry* &user_friends, 
//	NodeArray<building_model_data>* &user_village_model,NodeArray<user_temp_data,std::string>* &temp_data);
//bool sq_record_data_update(sq_record* record, sq_record_entry* entry, int64_t id, const void* d, size_t size);
//bool sq_record_data_update(sq_record* record, sq_record_entry* entry, int64_t id, user_record_entry* data, 
//	user_var_entry* var, user_vector_data<int>* offline_event, user_mail_entry* mail, 
//	user_pve_record_entry* pve, user_friends_record_entry* friends,
//	NodeArray<building_model_data>* village_model,NodeArray<user_temp_data,std::string>* temp_data);
//bool sq_record_data_select(sq_record* record, sq_record_entry* entry, DB_CALLBACK_FUNC);
//bool sq_record_data_select(sq_record* record, sq_record_entry* entry, int64_t mod, DB_CALLBACK_FUNC);
//int sq_record_data_select(sq_record* record, sq_record_entry* entry, int64_t mod, int offset, int rowcount, DB_CALLBACK_FUNC);
//int sq_record_data_first_select(sq_record* record, sq_record_entry* entry, int64_t mod, int offset, DB_CALLBACK_FUNC func);
//int sq_record_data_next_select(sq_record* record, sq_record_entry* entry, int64_t mod, DB_CALLBACK_FUNC func);

#endif
