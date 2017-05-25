#include "framework.h"
#include "server.h"
#include "sq_handler_user.h"
#include "utility.h"

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
	m_entry_acct = sq_record_data_init(m_record, "acct");
	m_entry_order = sq_record_data_init(m_record, "order");

	REGISTER_MSG_HANDLER(sq_handler_user, do_acct_register, message::AccountRegisterRequest::ID);
	REGISTER_MSG_HANDLER(sq_handler_user, do_user_login, message::UserLoginRequest::ID);
	REGISTER_MSG_HANDLER(sq_handler_user, do_chat_request, message::ChatRequest::ID);

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
	std::map<int64_t, game_user_table*>::iterator it = m_user_table.begin();
	while (it != m_user_table.end())
	{
		if (NULL == server->conntable[it->second->connfd])
		{
			delete it->second;
			m_user_table.erase(it++);
		}
		else
		{
			it++;
		}
	}

}

//------------------------------------------------------------
void sq_handler_user::do_acct_register(const MY_MSG_HEAD* msg)
{

}

void sq_handler_user::do_user_login(const MY_MSG_HEAD* msghead)
{
	message::UserLoginRequest *request = (message::UserLoginRequest *)msghead->msg;
	debug_log("do_user_login->name:%s,pwd:%s\n", request->login_name().c_str(), request->login_password().c_str());
	
	//create temporary userid
	uint32_t userid = hashString(request->login_name().c_str());

	game_user_table* user = NULL;
	std::map<int64_t, game_user_table*>::iterator it = m_user_table.find(userid);

	if (it != m_user_table.end())
	{
		user = (*it).second;
	}

	if (user == NULL)
	{
		user = new game_user_table;
		m_user_table[userid] = user;
	}

	user->connfd = msghead->s;
	user->userid = userid;
	user->user_state = USER_STATE::ONLINE;

	//error_log("user:0x%llu request login\n", msg->userid);

	message::UserLoginResponse response;
	response.set_login_name(request->login_name());
	response.set_ret_code(0);
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


	message::ChatResponse response;

	response.set_chat_content(request->chat_content());

	MY_MSG_HEAD resmsg;
	resmsg.msgid = message::ChatResponse::ID;
	resmsg.msg = &response;
	resmsg.userid = msghead->userid;

	//broadcast to all online users
	std::map<int64_t, game_user_table*>::iterator it = m_user_table.begin();
	while (it != m_user_table.end())
	{
		server->sendmsg(it->second->connfd, &resmsg);
		it++;
	}

	//server->sendmsg(msghead->s, &resmsg);
}