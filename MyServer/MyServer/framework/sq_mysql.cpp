/*
 * \file: sq_mysql.c
 * \brief: Created by hushouguo at 17:15:12 Dec 03 2011
 */

#include "framework.h"

bool sq_mysql::open(sq_mysql_config* mysql_config)
{
	return this->open(mysql_config->host, mysql_config->user, mysql_config->passwd, mysql_config->db, mysql_config->port);
}
bool sq_mysql::open(const char* host, const char* user, const char* passwd, const char* db, unsigned short port)
{
	uint32_t timeout;
	char buf[64];
	my_bool truncation;

	timeout = 10;
	if (mysql_options(&this->handler, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&timeout))
	{
		error_log("mysql_options error:%s\n", mysql_error(&this->handler));
		return false;
	}

	timeout = 604800;
	snprintf(buf, sizeof(buf), "set wait_timeout=%d", timeout);
	if (mysql_options(&this->handler, MYSQL_INIT_COMMAND, buf))
	{
		error_log("mysql set wait_timeout:%d error:%s\n", timeout, mysql_error(&this->handler));
		return false;
	}

	snprintf(buf, sizeof(buf), "set interactive_timeout=%d", timeout);
	if (mysql_options(&this->handler, MYSQL_INIT_COMMAND, buf))
	{
		error_log("mysql set interactive_timeout:%d error:%s\n", timeout, mysql_error(&this->handler));
		return false;
	}

	snprintf(buf, sizeof(buf), "set names utf8");
	if (mysql_options(&this->handler, MYSQL_INIT_COMMAND, buf))
	{
		error_log("mysql set names utf8 error:%s\n", mysql_error(&this->handler));
		return false;
	}	

	truncation = true;
	if (mysql_options(&this->handler, MYSQL_REPORT_DATA_TRUNCATION, (const char*)&truncation))
	{
		error_log("mysql MYSQL_REPORT_DATA_TRUNCATION error:%s\n", mysql_error(&this->handler));
		return false;
	}

	if (!mysql_real_connect(&this->handler, host, user, passwd, db, port, NULL, 0))
	{
		error_log("Connecting to database:%s,%s,%s,%s,%d error: %s\n", host, user, passwd, db, port, mysql_error(&this->handler));
		return false;
	}

	config.init(host, user, passwd, db, port);

	return true;
}
bool sq_mysql::reopen()
{
	this->close();
	return open(&config);
}
void sq_mysql::close()
{
	mysql_stmt_close(this->stmt);
	mysql_close(&this->handler);
	mysql_init(&this->handler);
	this->stmt = mysql_stmt_init(&this->handler);
}
bool sq_mysql::prepare(const char* stmt)
{
	if (mysql_stmt_prepare(this->stmt, stmt, strlen(stmt)))
	{
		if (mysql_stmt_errno(this->stmt) == CR_SERVER_GONE_ERROR)
		{
			alarm_log("try to reopen mysql\n");
			if (!reopen())
			{
				error_log("reopen mysql failure:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
				return false;
			}
			//debug_log("reopen success\n");
			if (mysql_stmt_prepare(this->stmt, stmt, strlen(stmt)))
			{
				error_log("mysql_stmt_prepare error:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
				return false;
			}
			return true;
		}
		error_log("mysql_stmt_prepare error:%d,%s, stmt:%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt), stmt);
		return false;
	}
	//uint32_t count = mysql_stmt_param_count(this->stmt);
	//debug_log("STATEMENT:%s, total parameters:%u\n", statement, count);
	return true;
}
bool sq_mysql::bind_param(MYSQL_BIND* b)
{
	if (mysql_stmt_bind_param(this->stmt, b))
	{
		if (mysql_stmt_errno(this->stmt) == CR_SERVER_GONE_ERROR)
		{
			alarm_log("try to reopen mysql\n");
			if (!reopen())
			{
				error_log("reopen mysql failure:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
				return false;
			}
			//debug_log("reopen success\n");
			if (mysql_stmt_bind_param(this->stmt, b))
			{
				error_log("mysql_stmt_bind_param error:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
				return false;
			}
			return true;
		}
		error_log("mysql_stmt_bind_param error:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
		return false;
	}
	return true;
}
bool sq_mysql::exec()
{
	if (mysql_stmt_execute(this->stmt))
	{
		if (mysql_stmt_errno(this->stmt) == CR_SERVER_GONE_ERROR)
		{
			alarm_log("try to reopen mysql\n");
			if (!reopen())
			{
				error_log("reopen mysql failure:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
				return false;
			}
			//debug_log("reopen success\n");
			if (mysql_stmt_execute(this->stmt))
			{
				error_log("mysql_stmt_execute error:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
				return false;
			}
			return true;
		}
		error_log("mysql_stmt_execute error:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
		return false;
	}
	//uint32_t affected = mysql_stmt_affected_rows(_stmt);
	//debug_log("This execute affected rows:%u\n", affected);
	return true;
}
bool sq_mysql::bind_result(MYSQL_BIND* b)
{
	if (mysql_stmt_bind_result(this->stmt, b))
	{
		if (mysql_stmt_errno(this->stmt) == CR_SERVER_GONE_ERROR)
		{
			alarm_log("try to reopen mysql\n");
			if (!reopen())
			{
				error_log("reopen mysql failure:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
				return false;
			}
			//debug_log("reopen success\n");
			if (mysql_stmt_bind_result(this->stmt, b))
			{
				error_log("mysql_stmt_bind_result error:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
				return false;
			}
			return true;
		}
		error_log("mysql_stmt_bind_result error:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
		return false;
	}
	//if (mysql_stmt_store_result(this->stmt))
	//{
	//	if (mysql_stmt_errno(this->stmt) == CR_SERVER_GONE_ERROR)
	//	{
	//		alarm_log("try to reopen mysql\n");
	//		if (!reopen())
	//		{
	//			error_log("reopen mysql failure:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
	//			return false;
	//		}
	//		//debug_log("reopen success\n");
	//		if (mysql_stmt_store_result(this->stmt))
	//		{
	//			error_log("mysql_stmt_store_result error:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
	//			return false;
	//		}
	//		return true;
	//	}
	//	error_log("mysql_stmt_store_result error:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
	//	return false;
	//}
	return true;
}
bool sq_mysql::fetch()
{
	if (mysql_stmt_fetch(this->stmt))
	{
		if (mysql_stmt_errno(this->stmt) == CR_SERVER_GONE_ERROR)
		{
			alarm_log("try to reopen mysql\n");
			if (!reopen())
			{
				error_log("reopen mysql failure:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
				return false;
			}
			//debug_log("reopen success\n");
			if (mysql_stmt_fetch(this->stmt))
			{
				//error_log("mysql_stmt_prepare error:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
				return false;
			}
			return true;
		}
		//error_log("mysql_stmt_fetch error:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
		return false;
	}
	return true;
}
bool sq_mysql::free_result()
{
	if (mysql_stmt_free_result(this->stmt))
	{
		if (mysql_stmt_errno(this->stmt) == CR_SERVER_GONE_ERROR)
		{
			alarm_log("try to reopen mysql\n");
			if (!reopen())
			{
				error_log("reopen mysql failure:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
				return false;
			}
			//debug_log("reopen success\n");
			if (mysql_stmt_free_result(this->stmt))
			{
				error_log("mysql_stmt_free_result error:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
				return false;
			}
			return true;
		}
		error_log("mysql_stmt_free_result error:%d,%s\n", mysql_stmt_errno(this->stmt), mysql_stmt_error(this->stmt));
		return false;
	}
	return true;
}
uint64_t sq_mysql::insert_id()
{
	return mysql_stmt_insert_id(this->stmt);
}

