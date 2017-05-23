/*
 * \file: sq_mysql.h
 * \brief: Created by hushouguo at 17:15:09 Dec 03 2011
 */
 
#ifndef __SQ_MYSQL_H__
#define __SQ_MYSQL_H__

struct sq_mysql_config
{
	char*							host;
	char*							user;
	char*							passwd;
	char*							db;
	unsigned short					port;
	sq_mysql_config() : host(NULL), user(NULL), passwd(NULL), db(NULL), port(0) {}
	~sq_mysql_config()
	{
		if (host) { free(host); }
		if (user) { free(user); }
		if (passwd) { free(passwd); }
		if (db) { free(db); }
	}
	void init(const char* host, const char* user, const char* passwd, const char* db, unsigned short port)
	{
		this->host = strdup(host);
		this->user = strdup(user);
		this->passwd = strdup(passwd);
		this->db = strdup(db);
		this->port = port;
	}
};
struct sq_mysql
{
	sq_mysql_config					config;
	MYSQL							handler;
	MYSQL_STMT*						stmt;
	sq_mysql()
	{
		unsigned long prefetch_rows = 400;
		mysql_init(&this->handler);
		this->stmt = mysql_stmt_init(&this->handler);
		mysql_stmt_attr_set(stmt, STMT_ATTR_PREFETCH_ROWS, (void*)&prefetch_rows);
	}
	~sq_mysql()
	{
		mysql_stmt_close(this->stmt);
		mysql_close(&this->handler);
	}
	bool							open(sq_mysql_config* mysql_config);
	bool 							open(const char* host, const char* user, const char* passwd, const char* db, unsigned short port);
	bool							reopen();
	void							handle_mysql_error();
	void 							close();
	bool 							prepare(const char* stmt);
	bool 							bind_param(MYSQL_BIND* b);
	bool 							exec();
	bool 							bind_result(MYSQL_BIND* b);
	bool 							fetch();
	bool 							free_result();
	uint64_t						insert_id();
};

#endif
