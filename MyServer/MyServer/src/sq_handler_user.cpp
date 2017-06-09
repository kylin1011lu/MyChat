#include "framework.h"
#include "server.h"
#include "sq_handler_user.h"

std::map<int64_t, game_user_table*> m_user_online_table;

//-----------------------------------------------------------------------------------------------
game_user_table::game_user_table()
{
	connfd = -1;
	userid = 0;
	user_state = USER_STATE::OFFLINE;
}

game_user_table::~game_user_table()
{

}


//--------------------------------------------------------------------
sq_handler_user::sq_handler_user()
{
	m_record = sq_record_init();
	m_record_entry = sq_record_data_init(m_record, "acct");

	REGISTER_MSG_HANDLER(sq_handler_user, do_acct_register, message::AccountRegisterRequest::ID);
	REGISTER_MSG_HANDLER(sq_handler_user, do_user_login, message::UserLoginRequest::ID);
	REGISTER_MSG_HANDLER(sq_handler_user, do_chat_request, message::ChatRequest::ID);
	REGISTER_MSG_HANDLER(sq_handler_user, do_chat_history_request, message::ChatHistoryRequest::ID);

}

sq_handler_user::~sq_handler_user()
{
	SQ_SAFE_DELETE(this->m_record_entry);
	SQ_SAFE_DELETE(this->m_record);
}

bool sq_handler_user::init(int n)
{
	//if (n == 0)
	//	pthread_mutex_init(&clanMapLocker, NULL);

	//if (!sq_http::init(service_export_ip, ANDROID_IAP_VERIFY_BASE_PORT + n))
	{
	//	return false;
	}

	return sq_handler::init(n);
}

void sq_handler_user::poll()
{
	sq_handler::poll();

	//delete disconnected user
	std::map<int64_t, game_user_table*>::iterator it = m_user_online_table.begin();
	while (it != m_user_online_table.end())
	{
		if (NULL == server->conntable[it->second->connfd])
		{
			delete it->second;
			m_user_online_table.erase(it++);
		}
		else
		{
			it++;
		}
	}

}

//------------------------------------------------------------
void sq_handler_user::do_acct_register(const MY_MSG_HEAD* msghead)
{
	message::AccountRegisterRequest *request = (message::AccountRegisterRequest *)msghead->msg;
	debug_log("do_acct_register->name:%s,pwd:%s\n", request->register_name().c_str(), request->register_password().c_str());

	char buf[56] = { 0 };
	size_t dataLen = sizeof(buf);
	sq_record_data_select(m_record, m_record_entry, request->register_name().c_str(), buf, dataLen);

	uint32_t userid = 0;
	message::AccountRegisterResponse response;
	if (strcmp(buf,"") != 0)
	{
		response.set_ret_code(message::AccountRegisterResponse::CODE_SAME_NAME);
	}
	else
	{
		userid = generateUUID();
		if (sq_record_data_insert(m_record, m_record_entry, userid, request->register_name().c_str(), request->register_password().c_str()))
		{
			response.set_ret_code(message::AccountRegisterResponse::CODE_SUCCESS);
		}
		else
		{
			response.set_ret_code(message::AccountRegisterResponse::CODE_INSERT_ERROR);
		}
	}

	response.set_userid(userid);

	MY_MSG_HEAD resmsg;
	resmsg.msgid = message::AccountRegisterResponse::ID;
	resmsg.msg = &response;
	resmsg.userid = userid;

	server->sendmsg(msghead->s, &resmsg);
}

void sq_handler_user::do_user_login(const MY_MSG_HEAD* msghead)
{
	message::UserLoginRequest *request = (message::UserLoginRequest *)msghead->msg;
	debug_log("do_user_login->name:%s,pwd:%s\n", request->login_name().c_str(), request->login_password().c_str());
	
	uint32_t userid = msghead->userid;
	char name[56] = { 0 };
	char pwd[56] = { 0 };
	size_t dataLen = sizeof(name);

	message::UserLoginResponse::LoginRetCode ret = message::UserLoginResponse::CODE_SUCCESS;

	if (sq_record_data_select(m_record, m_record_entry, userid, name,pwd, dataLen))
	{
		if (strcmp(name,request->login_name().c_str()) != 0 || strcmp(pwd,request->login_password().c_str()) != 0)
		{
			ret = message::UserLoginResponse::CODE_ERROR;
		}
	}
	else
	{
		ret = message::UserLoginResponse::CODE_FAILURE;
	}

	if (ret == message::UserLoginResponse::CODE_SUCCESS)
	{
		game_user_table* user = NULL;
		std::map<int64_t, game_user_table*>::iterator it = m_user_online_table.find(userid);

		if (it != m_user_online_table.end())
		{
			user = (*it).second;
		}
		else
		{
			user = new game_user_table;
			m_user_online_table[userid] = user;
		}

		user->connfd = msghead->s;
		user->userid = userid;
		user->user_state = USER_STATE::ONLINE;
	}

	//error_log("user:0x%llu request login\n", msg->userid);

	message::UserLoginResponse response;
	response.set_ret_code(ret);
	response.set_userid(userid);

	MY_MSG_HEAD resmsg;
	resmsg.msgid = message::UserLoginResponse::ID;
	resmsg.msg = &response;
	resmsg.userid = userid;

	server->sendmsg(msghead->s, &resmsg);
}

void sq_handler_user::do_chat_request(const MY_MSG_HEAD* msghead)
{
	message::ChatRequest *request = (message::ChatRequest *)msghead->msg;
	debug_log("do_chat_request->chat_content:%s\n", request->chat_content().c_str());

	if (!sq_record_data_insert(m_record,m_record_entry,msghead->userid,request->chat_content().c_str()))
	{
		error_log("do_chat_request->sq_record_data_insert error");
	}
	message::ChatResponse response;

	message::MessageInfo *info = response.add_message();
	info->set_chat_content(request->chat_content());
	info->set_send_userid(msghead->userid);
	info->set_send_time(time(0));

	MY_MSG_HEAD resmsg;
	resmsg.msgid = message::ChatResponse::ID;
	resmsg.msg = &response;
	resmsg.userid = msghead->userid;

	//broadcast to all online users
	std::map<int64_t, game_user_table*>::iterator it = m_user_online_table.begin();
	while (it != m_user_online_table.end())
	{
		server->sendmsg(it->second->connfd, &resmsg);
		it++;
	}

	//server->sendmsg(msghead->s, &resmsg);
}

void sq_handler_user::do_chat_history_request(const MY_MSG_HEAD* msghead)
{
	message::ChatHistoryRequest *request = (message::ChatHistoryRequest *)msghead->msg;
	debug_log("do_chat_history_request->last_time:%d\n", request->last_time());

	message::ChatHistoryResponse response;

	if (!sq_record_data_select(m_record,m_record_entry,request->last_time(),response))
	{
		error_log("do_chat_history_request->sq_record_data_select error");
	}
	MY_MSG_HEAD resmsg;
	resmsg.msgid = message::ChatResponse::ID;
	resmsg.msg = &response;
	resmsg.userid = msghead->userid;

	server->sendmsg(msghead->s, &resmsg);
}