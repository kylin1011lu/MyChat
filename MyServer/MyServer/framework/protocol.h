/*
 * \file: protocol.h
 * \brief: Created by hushouguo at Apr 16 2013 14:33:33
 */
 
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>

#ifdef WIN32
	#pragma warning(disable:4200)
	#pragma warning(disable:4819)
#endif

#ifndef WIN32
typedef uint32_t			DWORD;
typedef int32_t             BOOL;
typedef uint8_t				BYTE;
typedef uint16_t			WORD;
typedef float               FLOAT;
typedef const char			*LPCSTR, *PCSTR;
#endif

#define APP_ID									2
#define APP_NAME								"iPart2"

#define PLATFORM_ANDROID_YOUQUDAO				1	//有趣�?
#define PLATFORM_ANDROID_360					2	//360
#define PLATFORM_ANDROID_WANDOUJIA				3	//豌豆�?
#define PLATFORM_ANDROID_UC						4	//UC
#define PLATFORM_ANDROID_DOWNJOY				5	//当乐
#define PLATFORM_IOS_PP							6	//IOS-PP
#define PLATFORM_ANDROID_MI						7	//小米
#define PLATFORM_IOS							8	//IOS
#define PLATFORM_ANDROID_91						9	//91
#define PLATFORM_IOS_91							10	//ios-91
#define PLATFORM_ANDROID_DUOKU					11	//多酷
#define PLATFORM_ANDROID_HUAWEI					12	//华为

#define MAX_NETWORK_PACKAGE_SIZE                (0xffff * 4)
#define MAX_DOMAIN_SIZE							64

#define ANDROID_ORDER_ID_LENGTH					64
#define ANDROID_RECEIPT_LENGTH					1024

#define MY_MSG_HEAD_SIZE_INDEX					(sizeof(((MY_MSG_HEAD*)0)->msgid) + sizeof(((MY_MSG_HEAD*)0)->userid))		//消息头结构体消息长度开始位�?
#define MY_MSG_HEAD_CMD_INDEX					0																//消息头结构体消息id开始位�?
#define MY_MSG_HEAD_SIZE						(MY_MSG_HEAD_SIZE_INDEX + sizeof(((MY_MSG_HEAD*)0)->size))

typedef ::google::protobuf::MessageLite			mymsg;
enum
{

	ORDER_STATUS_OK						=	0,
	ORDER_STATUS_NEW					=	1,
	ORDER_STATUS_VERIFY_FAILURE			=	2,
};

//namespace protocol
//{
#pragma pack(push, 1)

	struct nullmsg
	{
		typedef nullmsg* MessageCreator();
		enum { MaxMsgId = 65535 };

		uint32_t								size;
		uint16_t								cmd;
		int64_t									userid;
		SOCKET s;
		nullmsg()
		{
			size = sizeof(*this);
			cmd = 0;
			userid = 0;
			s = 0;
		}
		virtual WORD MessageId() { return 0; }

		static nullmsg *CreateMessage(WORD wMsgId);
		static void DestroyMessage(nullmsg *pkMsg);
	};

	typedef struct message_head
	{
		uint16_t msgid;
		uint32_t userid;
		uint32_t size;
		SOCKET s;
		mymsg* msg;

		message_head()
		{
			size = sizeof(*this);
			msgid = 0;
			userid = 0;
			s = 0;
			msg = NULL;
		}

		static void DestroyMessage(message_head *msghead);

	}MY_MSG_HEAD;


	struct network_address
	{
		uint32_t				                ip;
		uint16_t				                port;
		//char									domain[MAX_DOMAIN_SIZE];
	};

#pragma pack(pop)

	struct _Msg_Creator_Entry_
	{
		_Msg_Creator_Entry_() { lpszMsgClassName = NULL; pkCreator = NULL; }
		LPCSTR lpszMsgClassName;
		nullmsg::MessageCreator* pkCreator;
	};

	struct _MSG_CLASS_INIT_
	{
		_MSG_CLASS_INIT_(WORD wMsgId, LPCSTR lpszClassName, nullmsg::MessageCreator* pkMsgCreator);
	};

	#define M_DECLARE_MESSAGE(class_name, id) \
		static nullmsg* CreateMessage(); \
		enum { __MSG_ID__ = id }; \
		WORD MessageId() { return id; }

	#define M_IMPLEMENT_MESSAGE(class_name) \
		nullmsg* class_name::CreateMessage() { return new class_name; } \
		_MSG_CLASS_INIT_ _init_##class_name(class_name::__MSG_ID__, #class_name, class_name::CreateMessage);

	#define RF_LE(x) if (x < 0) { assert(false); return false; }
	#define RF_FAILED(x) if (!x) { assert(false); return false; }
//};

#endif
