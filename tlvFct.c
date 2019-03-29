#include "tlvFct.h"

/*Message*/

int createMsg(char * msg) {
	memset(msg, 0, BUF_SIZE);
	uint8_t magic = 0x5d;//93;
	uint8_t ver = 0x2;//2;
	memcpy(msg, &magic, 1);
	memcpy(msg+1, &ver,1);
	return 4;
}

//TODO Voir Pour ajouter une suite de Tlv en modifiant taille de Msg
int setMsg_Body(char * msg, char * body, uint16_t len) {
	memcpy(msg+2, &len, 2);
	memcpy(msg+4, body, len);
	return 4+len;
}

uint8_t getMsg_Magic(char * msg) {
	uint8_t magic;
	memcpy(&magic, msg, 1);
	return magic;
}

uint8_t getMsg_Version(char * msg) {
	uint8_t ver;
	memcpy(&ver, msg+1, 1);
	return ver;
}

uint16_t getMsg_Body_Length(char * msg) {
	uint16_t len;
	memcpy(&len, msg+2, 2);
	return len;
}

char * getMsg_Tlv(char * msg) {
	return msg+4;
}


/*TLV*/


uint8_t getTlv_Type(char * tlv) {
	uint8_t type;
	memcpy(&type, tlv, 1);
	return type;
}

uint8_t getTlv_Length(char * tlv) {
	uint8_t len;
	memcpy(&len, tlv+1, 1);
	return len;
}

char * getTlv_Body(char * tlv) {
	return tlv+2;
}


/*Hello*/

uint64_t getHello_Source_Id(char * hello) {
	uint64_t sId;
	memcpy(&sId, hello+2, 8);
	return sId;
}


/*Hello_short*/

int createHello_short(char * hello_s, uint64_t sId) {
	memset(hello_s, 0, BUF_SIZE-4);
	uint8_t type = 0x2;//2;
	uint8_t len = 0x8;//8;
	memcpy(hello_s, &type, 1);
	memcpy(hello_s+1, &len, 1);
	memcpy(hello_s+2, &sId,8);
	return 10;
}


/*Hello_long*/

int createHello_long(char * hello_l, uint64_t sId, uint64_t dId) {
	memset(hello_l, 0, BUF_SIZE-4);
	uint8_t type = 0x2;//2;
	uint8_t len = 0x10;//16;
	memcpy(hello_l, &type, 1);
	memcpy(hello_l+1, &len, 1);
	memcpy(hello_l+2, &sId,8);
	memcpy(hello_l+10, &dId,8);
	return 18;
}

uint64_t getHello_long_Destination_Id(char * hello_l) {
	uint64_t dId;
	memcpy(&dId, hello_l+10, 8);
	return dId;
}


