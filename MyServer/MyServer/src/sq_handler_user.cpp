#include "framework.h"
#include "server.h"
#include "sq_handler_user.h"

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

void sq_handler_user::do_user_login(const MY_MSG_HEAD* msghead)
{
	message::UserLoginRequest *request = (message::UserLoginRequest *)msghead->msg;
	debug_log("do_user_login->name:%s,pwd:%s\n", request->login_name().c_str(), request->login_password().c_str());
	
	message::UserLoginResponse response;

	response.set_login_name(request->login_name());
	response.set_ret_code(0);
	response.set_userid(1);

	MY_MSG_HEAD resmsg;
	resmsg.msgid = message::UserLoginResponse::ID;
	resmsg.msg = &response;
	resmsg.userid = 1;

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
	resmsg.userid = 1;

	server->sendmsg(msghead->s, &resmsg);
}