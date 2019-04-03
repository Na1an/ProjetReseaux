#include "tlvFct.h"


/*Message*/

int MSG_ENTETE = 4;
int TLV_ENTETE = 2;

int createMsg(char * msg) {
	memset(msg, 0, BUF_SIZE);
	uint8_t magic = 0x5d;//93;
	uint8_t ver = 0x2;//2;
	memcpy(msg, &magic, 1);
	memcpy(msg+1, &ver, 1);
	return MSG_ENTETE;
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

char * getMsg_Tlv(char * msg, uint8_t len) {
	return msg+MSG_ENTETE+len;
}

int setMsg_Body(char * msg, char * body, uint16_t len) {
	uint16_t l = ntohs(getMsg_Body_Length(msg));
	memcpy(msg+4+l, body, len);
	l += len;
	uint16_t h = htons(l);
	memcpy(msg+2, &h, 2);
	return MSG_ENTETE+l;
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
	return tlv+TLV_ENTETE;
}


/*Pad1*/

int createPad1(char * pad) {
	memset(pad, 0, BUF_SIZE-MSG_ENTETE);
	uint8_t type = 0x0;//0;
	memcpy(pad, &type, 1);
	return 1;
}


/*PadN*/

int createPadN(char * pad, uint8_t len) {
	memset(pad, 0, BUF_SIZE-MSG_ENTETE);
	uint8_t type = 0x1;//1;
	memcpy(pad, &type, 1);
	memcpy(pad+1, &len, 1);
	return TLV_ENTETE;
}


/*Hello*/

uint64_t getHello_Source_Id(char * hello) {
	uint64_t sId;
	memcpy(&sId, hello+2, 8);
	return sId;
}


/*Hello_short*/

int createHello_short(char * hello_s, uint64_t sId) {
	memset(hello_s, 0, BUF_SIZE-MSG_ENTETE);
	uint8_t type = 0x2;//2;
	uint8_t len = 0x8;//8;
	memcpy(hello_s, &type, 1);
	memcpy(hello_s+1, &len, 1);
	memcpy(hello_s+2, &sId, 8);
	return len+TLV_ENTETE;
}


/*Hello_long*/

int createHello_long(char * hello_l, uint64_t sId, uint64_t dId) {
	memset(hello_l, 0, BUF_SIZE-MSG_ENTETE);
	uint8_t type = 0x2;//2;
	uint8_t len = 0x10;//16;
	memcpy(hello_l, &type, 1);
	memcpy(hello_l+1, &len, 1);
	memcpy(hello_l+2, &sId,8);
	memcpy(hello_l+10, &dId, 8);
	return len+TLV_ENTETE;
}

uint64_t getHello_long_Destination_Id(char * hello_l) {
	uint64_t dId;
	memcpy(&dId, hello_l+10, 8);
	return dId;
}


/*Neighbour*/

int createNeighbour(char * neigbour, struct in6_addr ip, in_port_t port) {
	memset(neigbour, 0, BUF_SIZE-MSG_ENTETE);
	uint8_t type = 0x3;//3;
	uint8_t len = 0x12;//18;
	memcpy(neigbour, &type, 1);
	memcpy(neigbour+1, &len, 1);
	memcpy(neigbour+2, &ip, 16);
	memcpy(neigbour+18, &port, 2);
	return len+TLV_ENTETE;
}

int getNeighbour_Ip(char * neighbour, struct in6_addr * ip) {
	memset(ip, 0, 16);
	memcpy(ip, neighbour+2, 16);
	return 16;
}

in_port_t getNeighbour_Port(char * neighbour) {
	in_port_t port;
	memcpy(&port, neighbour+18, 2);
	return port;
}


/*Data*/

int createData(char * data, uint64_t sId, uint32_t nonce) {
	memset(data, 0, BUF_SIZE-MSG_ENTETE);
	uint8_t type = 0x4;//4;
	uint8_t len = 0xD;//13;
	memcpy(data, &type, 1);
	memcpy(data+1, &len, 1);
	memcpy(data+2, &sId, 8);
	memcpy(data+10, &nonce, 4);
	return len+TLV_ENTETE;
}

uint64_t getData_Sender_Id(char * data) {
	uint64_t sId;
	memcpy(&sId, data+2, 8);
	return sId;
}

uint32_t getData_Nonce(char * data) {
	uint32_t nonce;
	memcpy(&nonce, data+10, 4);
	return nonce;
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
	return TLV_ENTETE+l;
}


/*Ack*/

int createAck(char * ack, uint64_t sId, uint32_t nonce) {
	memset(ack, 0, BUF_SIZE-MSG_ENTETE);
	uint8_t type = 0x5;//5;
	uint8_t len = 0xC;//12;
	memcpy(ack, &type, 1);
	memcpy(ack+1, &len, 1);
	memcpy(ack+2, &sId, 8);
	memcpy(ack+10, &nonce, 4);
	return len+TLV_ENTETE;
}

uint64_t getAck_Sender_Id(char * ack) {
	uint64_t sId;
	memcpy(&sId, ack+2, 8);
	return sId;
}

uint32_t getAck_Nonce(char * ack) {
	uint32_t nonce;
	memcpy(&nonce, ack+10, 4);
	return nonce;
}


/*GoAway*/

int createGoAway(char * goAway, uint8_t code, char * message, int taille) {
	memset(goAway, 0, BUF_SIZE-MSG_ENTETE);
	uint8_t type = 0x6;//6;
	uint8_t len = 0x1+taille;//1;
	memcpy(goAway, &type, 1);
	memcpy(goAway+1, &len, 1);
	memcpy(goAway+2, &code, 1);
	memcpy(goAway+3, &message, taille);
	return len+TLV_ENTETE;
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
	memset(warning, 0, BUF_SIZE-MSG_ENTETE);
	uint8_t type = 0x7;//7;
	uint8_t len = taille;
	memcpy(warning, &type, 1);
	memcpy(warning+1, &len, 1);
	memcpy(warning+2, &message, taille);
	return len+TLV_ENTETE;
}

char * getWarning_Message(char * warning) {
	return warning+2;
}

int getWarning_Message_Taille(char * warning) {
	uint8_t len = getTlv_Length(warning);
	return len;
}

/*Global*/

int printMsg(char * msg) {
	printf("Magic : %"PRIu8"\n", getMsg_Magic(msg));
	printf("Version : %"PRIu8"\n", getMsg_Version(msg));

	uint16_t body_len = getMsg_Body_Length(msg);

	printf("Body_Length : %"PRIu16"\n", body_len);

	char * tlv; uint8_t type; uint8_t tlv_len;

	int i = 0;

	while(i < body_len) {

		tlv = getMsg_Tlv(msg, i);

		type = getTlv_Type(tlv);

		tlv_len = getTlv_Length(tlv);

		printf("Tlv_Type : %"PRIu8"\n", type);

		tlv_len = getTlv_Length(tlv);

		printf("Tlv_Length : %"PRIu8"\n", tlv_len);

		switch(type) {

			case 0 : printf("Pad1\n");
				break;

			case 1 : printf("PadN\n");
				break;

			case 2 : if(tlv_len == 8) {
					printf("Hello_short :\n");
					printf("\tSource_Id : %"PRIu64"\n", getHello_Source_Id(tlv));
				} else if(tlv_len == 16) {
					printf("Hello_long :\n");
					printf("\tSource_Id : %"PRIu64"\n", getHello_Source_Id(tlv));
					printf("\tDestination_Id : %"PRIu64"\n", getHello_long_Destination_Id(tlv));
				} else {
					printf("Hello Inconnu\n");
				}
				break;

			case 3 : printf("Neighbour\n");
				break;

			case 4 : printf("Data\n");
				printf("\tSource : %"PRIu64"\n", getData_Sender_Id(tlv));
				printf("\tNonce : %"PRIu32"\n", getData_Nonce(tlv));
				printf("\tType : %"PRIu8"\n", getData_Type(tlv));
				printf("\tDonnées : %s\n", getData_Donnees(tlv));
				break;

			case 5 : printf("Ack\n");
				printf("\tSource : %"PRIu64"\n", getAck_Sender_Id(tlv));
				printf("\tNonce : %"PRIu32"\n", getAck_Nonce(tlv));
				break;

			case 6 : printf("GoAway\n");
				printf("\tCode : %"PRIu8"\n", getGoAway_Code(tlv));
				printf("\tMessage : %s\n", getGoAway_Message(tlv));
				break;

			case 7 : printf("Warning\n");
				printf("\tMessage : %s\n", getWarning_Message(tlv));
				break;

			default : printf("Inconnu\n");

		}

		i += (tlv_len+TLV_ENTETE);

	}

	printf("///////////////////////////////\n");

	return 0;

}











