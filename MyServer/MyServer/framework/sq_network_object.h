 
#ifndef __SQ_NETWORK_OBJECT_H__
#define __SQ_NETWORK_OBJECT_H__

struct sq_network_object
{
	SOCKET							fd;
	bool							lost_flag;
	bool							close_flag;
	sq_network_object(SOCKET s) : fd(s), lost_flag(false), close_flag(false) {}
	virtual ~sq_network_object() {}
	bool							active() { return !lost_flag && !close_flag; }
	virtual void					receive() = 0;
	virtual void					send() = 0;
	virtual void					lost_connection() = 0;
	virtual void					close_connection() = 0;
};

#endif
