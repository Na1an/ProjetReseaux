#ifndef TLVFCT_H
#define TLVFCT

#include <time.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#define BUF_SIZE 4096

/*struct*/


/*
struct Voisin {
	struct in6_addr ip;
	in_port_t port;
};

struct ListVoisinPotentiel {
	struct Voisin * voisin;
	struct ListVoisinPotentiel * suite;
};

struct ListVoisin {
	struct Voisin * voisin;
	uint64_t id;
	char date[26];
	char date_long[26];
	struct ListVoisin * suite;
};
*/

//struct ListVoisin * init_ListVoisin();


/*struct Voisin {//Index
	struct in6_addr ip;
	in_port_t port;
};

struct ListVoisinPotentiel {//VoisinPot
	struct Voisin * voisin;
	struct ListVoisinPotentiel * suite;
};

struct ListVoisin {//Voisin
	struct Voisin * voisin;
	uint64_t id;
	char date[30];
	char date_long[30];
	struct ListVoisin * suite;
};*/
/*type define*/

//typedef __int128 int128_t;//??
//typedef __uint128 uint128_t;

/*Message*/

int createMsg(char * msg);

uint8_t getMsg_Magic(char * msg);

uint8_t getMsg_Version(char * msg);

uint16_t getMsg_Body_Length(char * msg);

char * getMsg_Tlv(char * msg, uint8_t len);

int setMsg_Body(char * msg, char * body, uint16_t len);


/*TLV*/

uint8_t getTlv_Type(char * tlv);

uint8_t getTlv_Length(char * tlv);

char * getTlv_Body(char * tlv);


/*Pad1*/

int createPad1(char * pad);


/*PadN*/

int createPadN(char * pad, uint8_t len);


/*Hello*/

uint64_t getHello_Source_Id(char * hello);


/*Hello_short*/

int createHello_short(char * hello_s, uint64_t sId);


/*Hello_long*/

int createHello_long(char * hello_l, uint64_t sId, uint64_t dId);

uint64_t getHello_long_Destination_Id(char * hello_l);


/*Neighbour*/

int createNeightbour(char * neigbour, struct in6_addr ip, in_port_t port);

int getNeightbour_Ip(char * neightbour, struct in6_addr * ip);

in_port_t getNeightbour_Port(char * neightbour);


/*Data*/

int createData(char * data, uint64_t sId, uint32_t nonce);

uint64_t getData_Sender_Id(char * data);

uint32_t getData_Nonce(char * data);

uint8_t getData_Type(char * data);

char * getData_Donnees(char * data);

int getData_Donnees_Taille(char * data);

int setData_Donnees(char * data, uint8_t t, char * donnees, int taille);


/*Ack*/

int createAck(char * ack, uint64_t sId, uint32_t nonce);

uint64_t getAck_Sender_Id(char * ack);

uint32_t getAck_Nonce(char * ack);


/*GoAway*/

int createGoAway(char * goAway, uint8_t code, char * message, int taille);

uint8_t getGoAway_Code(char * goAway);

char * getGoAway_Message(char * goAway);

int getGoAway_Message_Taille(char * goAway);


/*Warning*/

int createWarning(char * warning, char * message, int taille);

char * getWarning_Message(char * warning);

int getWarning_Message_Taille(char * warning);

/*Global*/

int printMsg(char * msg);


#endif
