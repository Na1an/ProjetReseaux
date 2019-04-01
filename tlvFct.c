#include "tlvFct.h"


/*Message*/

int createMsg(char * msg) {
	memset(msg, 0, BUF_SIZE);
	uint8_t magic = 0x5d;//93;
	uint8_t ver = 0x2;//2;
	memcpy(msg, &magic, 1);
	memcpy(msg+1, &ver, 1);
	return 4;
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
	return ntohs(len);
}

char * getMsg_Tlv(char * msg) {
	return msg+4;
}

int setMsg_Body(char * msg, char * body, uint16_t len) {
	uint16_t l = ntohs(getMsg_Body_Length(msg));
	memcpy(msg+4+l, body, len);
	l += len;
	uint16_t h = htons(l);
	memcpy(msg+2, &h, 2);
	return 4+l;
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


/*Pad1*/

int createPad1(char * pad) {
	memset(pad, 0, BUF_SIZE-4);
	uint8_t type = 0x0;//0;
	memcpy(pad, &type, 1);
	return 1;
}


/*PadN*/

int createPadN(char * pad, uint8_t len) {
	memset(pad, 0, BUF_SIZE-4);
	uint8_t type = 0x1;//1;
	memcpy(pad, &type, 1);
	memcpy(pad+1, &len, 1);
	return 2;
}


/*Hello*/

uint64_t getHello_Source_Id(char * hello) {
	uint64_t sId;
	memcpy(&sId, hello+2, 8);
	return ntohs(sId);
}


/*Hello_short*/

int createHello_short(char * hello_s, uint64_t sId) {
	memset(hello_s, 0, BUF_SIZE-4);
	uint8_t type = 0x2;//2;
	uint8_t len = 0x8;//8;
	uint64_t i = htons(sId);
	memcpy(hello_s, &type, 1);
	memcpy(hello_s+1, &len, 1);
	memcpy(hello_s+2, &i, 8);
	return 10;
}


/*Hello_long*/

int createHello_long(char * hello_l, uint64_t sId, uint64_t dId) {
	memset(hello_l, 0, BUF_SIZE-4);
	uint8_t type = 0x2;//2;
	uint8_t len = 0x10;//16;
	uint64_t si = htons(sId);
	uint64_t di = htons(dId);
	memcpy(hello_l, &type, 1);
	memcpy(hello_l+1, &len, 1);
	memcpy(hello_l+2, &si,8);
	memcpy(hello_l+10, &di, 8);
	return 18;
}

uint64_t getHello_long_Destination_Id(char * hello_l) {
	uint64_t dId;
	memcpy(&dId, hello_l+10, 8);
	return ntohs(dId);
}


/*Neighbour*/

int createNeightbour(char * neigbour, struct in6_addr ip, in_port_t port) {
	memset(neigbour, 0, BUF_SIZE-4);
	uint8_t type = 0x3;//3;
	uint8_t len = 0x12;//18;
	memcpy(neigbour, &type, 1);
	memcpy(neigbour+1, &len, 1);
	memcpy(neigbour+2, &ip, 16);
	memcpy(neigbour+18, &port, 2);
	return 20;
}

int getNeightbour_Ip(char * neightbour, struct in6_addr * ip) {
	memset(ip, 0, 16);
	memcpy(ip, neightbour+2, 16);
	return 16;
}

in_port_t getNeightbour_Port(char * neightbour) {
	in_port_t port;
	memcpy(&port, neightbour+18, 2);
	return port;
}


/*Data*/

int createData(char * data, uint64_t sId, uint32_t nonce) {
	memset(data, 0, BUF_SIZE-4);
	uint8_t type = 0x4;//4;
	uint8_t len = 0xD;//13;
	uint64_t i = htons(sId);
	uint32_t n = htons(nonce);
	memcpy(data, &type, 1);
	memcpy(data+1, &len, 1);
	memcpy(data+2, &i, 8);
	memcpy(data+10, &n, 4);
	return 15;
}

uint64_t getData_Sender_Id(char * data) {
	uint64_t sId;
	memcpy(&sId, data+2, 8);
	return ntohs(sId);
}

uint32_t getData_Nonce(char * data) {
	uint32_t nonce;
	memcpy(&nonce, data+10, 4);
	return ntohs(nonce);
}

uint8_t getData_Type(char * data) {
	uint8_t t;
	memcpy(&t, data+14, 1);
	return t;
}

char * getData_Donnees(char * data) {
	return data+15;
}

int getData_Donnees_Taille(char * data) {
	uint8_t len = getTlv_Length(data);
	return len-13;
}

int setData_Donnees(char * data, uint8_t t, char * donnees, int taille) {
	memcpy(data+14, &t, 1);
	uint8_t l = getTlv_Length(data);
	memcpy(data+2+l, donnees, taille);
	l += taille;
	memcpy(data+1, &l, 1);
	return 2+l;
}


/*Ack*/

int createAck(char * ack, uint64_t sId, uint32_t nonce) {
	memset(ack, 0, BUF_SIZE-4);
	uint8_t type = 0x5;//5;
	uint8_t len = 0xC;//12;
	uint64_t i = htons(sId);
	uint32_t n = htons(nonce);
	memcpy(ack, &type, 1);
	memcpy(ack+1, &len, 1);
	memcpy(ack+2, &i, 8);
	memcpy(ack+10, &n, 4);
	return 14;
}

uint64_t getAck_Sender_Id(char * ack) {
	uint64_t sId;
	memcpy(&sId, ack+2, 8);
	return ntohs(sId);
}

uint32_t getAck_Nonce(char * ack) {
	uint32_t nonce;
	memcpy(&nonce, ack+10, 4);
	return ntohs(nonce);
}


/*GoAway*/

int createGoAway(char * goAway, uint8_t code, char * message, int taille) {
	memset(goAway, 0, BUF_SIZE-4);
	uint8_t type = 0x6;//6;
	uint8_t len = 0x1+taille;//1;
	memcpy(goAway, &type, 1);
	memcpy(goAway+1, &len, 1);
	memcpy(goAway+2, &code, 1);
	memcpy(goAway+3, &message, taille);
	return taille+3;
}

uint8_t getGoAway_Code(char * goAway) {
	uint8_t code;
	memcpy(&code, goAway+2, 1);
	return code;
}

char * getGoAway_Message(char * goAway) {
	return goAway+3;
}

int getGoAway_Message_Taille(char * goAway) {
	uint8_t len = getTlv_Length(goAway);
	return len-1;
}


/*Warning*/

int createWarning(char * warning, char * message, int taille) {
	memset(warning, 0, BUF_SIZE-4);
	uint8_t type = 0x7;//7;
	uint8_t len = taille;
	memcpy(warning, &type, 1);
	memcpy(warning+1, &len, 1);
	memcpy(warning+2, &message, taille);
	return len+2;
}

char * getWarning_Message(char * warning) {
	return warning+2;
}

int getWarning_Message_Taille(char * warning) {
	uint8_t len = getTlv_Length(warning);
	return len;
}














