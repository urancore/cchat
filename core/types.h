#ifndef CCHAT_TYPES_H
#define CCHAT_TYPES_H

#include <stdint.h>

typedef uint32_t ID;

typedef enum {
	T_MSG_TEXT,
	T_AUTH,
	T_PING,
	T_DISCONNECT
} cchatPackageT;


typedef struct Message_s
{
	ID user_id;
	char username[32];
	uint32_t size;
	char text[512];
} Message;

typedef struct Ping_s
{
	uint32_t timestamp;
	uint32_t user_id;
} Ping;

typedef struct Auth_t
{
	char username[32];
	char password[32];
} Auth;

typedef struct Disconnect_s
{
	ID user_id;
} Disconnect;


typedef struct Packet_s
{
	cchatPackageT type;
	union {
		Message text_message;
		Ping ping;
		Auth auth;
		Disconnect disconnect;
	} data;
} Packet;

#endif
