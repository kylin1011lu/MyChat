/*
* file: TCPClient.h
* brief: Created by kylin
*/

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>
#include <google/protobuf/message_lite.h>

#define MAX_DOMAIN_SIZE							64
#define MAX_NETWORK_PACKAGE_SIZE                65535

#define MY_MSG_HEAD_SIZE_INDEX					(sizeof(((MY_MSG_HEAD*)0)->msgid) + sizeof(((MY_MSG_HEAD*)0)->userid))		//消息头结构体消息长度开始位置
#define MY_MSG_HEAD_CMD_INDEX					0																			//消息头结构体消息id开始位置
#define MY_MSG_HEAD_SIZE						(MY_MSG_HEAD_SIZE_INDEX + sizeof(((MY_MSG_HEAD*)0)->size))

typedef ::google::protobuf::MessageLite  MessageLite;

//协议结构体
#pragma pack(1)
typedef struct message_head
{
	uint16_t msgid;
	uint32_t userid;
	uint32_t size;

}MY_MSG_HEAD;

struct network_address
{
	char					                ip[MAX_DOMAIN_SIZE];
	char									domain[MAX_DOMAIN_SIZE];
	uint16_t				                port;
};

#pragma pack(4)

#endif