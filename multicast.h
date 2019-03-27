#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <errno.h>
#include <sys/select.h>
#include <inttypes.h>
#include <netdb.h>

//#include <inttypes.h>
//#include <fcntl.h>
#include <sys/time.h>

#define DEBUG 1
#define BUF_SIZE 4096
#define PORT "1212"
//#define IFCONFIG "wlp2s0"//Non
#define ADDR /*/"localhost"/*/"jch.irif.fr"/**/

typedef __int128 uint128_t;

typedef struct{

	uint8_t Type;
	uint8_t Length;

	char * Body;

} TLV;

typedef struct{

	uint8_t Magic;
	uint8_t Version;

	uint16_t Body_Length;

	TLV Tlv;

} Message;

typedef struct{

	uint8_t Type; // = 0

} Pad1;

typedef struct{

	uint8_t Type; // = 1;
	uint8_t Length;

	char * MBZ;

} PadN;

typedef struct{

	uint8_t Type; // = 2;
	uint8_t Length;

	uint64_t Source_Id;

} Hello_short;


typedef struct{

	uint8_t Type; // = 2;
	uint8_t Length;

	uint64_t Source_Id;
	uint64_t Destination_Id;	

} Hello_long;


typedef struct{

	uint8_t Type; // = 3;
	uint8_t Length;

	uint128_t IP;
	uint16_t Port;
} Neighbour;

typedef struct{

	uint8_t Type; // = 4;
	uint8_t Length;

	uint64_t Sender_Id;
	uint32_t Nonce;

	uint8_t Type2;
	char * Data;

} Data;


typedef struct{

	uint8_t Type; // = 5;
	uint8_t Length;

	uint64_t Sender_Id;
	uint32_t Nonce;

} Ack;

typedef struct{

	uint8_t Type; // = 6;
	uint8_t Length;

	uint8_t Code;
	char * Message;

} GoAway;


typedef struct{

	uint8_t Type; // = 7;
	uint8_t Length;

	char * Message;

} Warning;




