
#ifndef __SQ_LISTENING_H__
#define __SQ_LISTENING_H__

struct sq_listening : public sq_network_object
{
	typedef void (*listening_dispatch)(sq_listening*, SOCKET connfd);

	listening_dispatch					dispatch;
	sq_listening(listening_dispatch f);
	void								receive();
	void								send();
	void								lost_connection();
	void								close_connection();
	bool								init(uint32_t ip, int port);
	bool								init(const char* ip, int port);
};

#endif
