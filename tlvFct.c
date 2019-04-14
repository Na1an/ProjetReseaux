#include "tlvFct.h"

/*Message*/

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
	if(l+len+MSG_ENTETE > BUF_SIZE) return -1;
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
	return TLV_ENTETE+len;
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

//Remarque : ip = ip->s6_addr
int getNeighbour_Ip(char * neighbour, struct in6_addr * ip) {
	memset(ip, 0, 16);
	memcpy(ip, neighbour+2, 16);
	return 16;
}

/*int getNeighbour_Ip2(char * neighbour, struct in6_addr * ip, char * res) {
	memset(ip, 0, 16);
	
	char str1[16]; //copy la chaine
	memset(str1, 0, 16);
	memcpy(str1, neighbour+2, 16);
	
	//char str2[16];
	memset(res, 0, 16);

	inet_pton(AF_INET6, str1, ip);
	printf("str1 : %s \n", str1);
	printf("ip->s6_addr : %s \n", ip->s6_addr);
	inet_ntop(AF_INET6, ip, res, 16);
	return 1;	
}*/

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

int printMsg(char * msg) {/* TODO -> Améliorer : Attention au taille, magic, version, ...*/

	time_t timep;
	time(&timep);
	/*//printf("%s", asctime(gmtime(&timep)));
	char * date_short = 0;//[26];
	char * date_long = 0;//[26];*/

	printf("Magic : %"PRIu8"\n", getMsg_Magic(msg));
	printf("Version : %"PRIu8"\n", getMsg_Version(msg));

	uint16_t body_len = getMsg_Body_Length(msg);

	printf("Body_Length : %"PRIu16"\n", body_len);

	char * tlv; uint8_t type; uint8_t tlv_len; char txt[BUF_SIZE];

	int i = 0;
	
	//uint64_t source_id = 0;
	while(i < body_len) {

		memset(txt, 0, BUF_SIZE);

		tlv = getMsg_Tlv(msg, i);

		type = getTlv_Type(tlv);

		tlv_len = getTlv_Length(tlv);

		printf("Tlv_Type : %"PRIu8"\n", type);

		tlv_len = getTlv_Length(tlv);

		printf("Tlv_Length : %"PRIu8"\n", tlv_len);

		switch(type) {

			case 0 : 
				printf("=>\nPad1\n");
				break;

			case 1 : 
				printf("=>\nPadN\n");
				break;

			case 2 : 
				if(tlv_len == 8) {
					printf("{\n\tHello_short :\n");
					printf("\t\tSource_Id : %"PRIu64"\n", getHello_Source_Id(tlv));
					printf("\t\tDate : %s", asctime(gmtime(&timep)));
				 } else if(tlv_len == 16) {
					printf("{\n\tHello_long :\n");
					printf("\t\tSource_Id : %"PRIu64"\n", getHello_Source_Id(tlv));
					printf("\t\tDestination_Id : %"PRIu64"\n", getHello_long_Destination_Id(tlv));
					printf("\t\tDate : %s", asctime(gmtime(&timep)));
				 } else {
					printf("{\n\tHello Inconnu\n");	
				 }
				 break;

			case 3 : 
				printf("{\n\tNeighbour\n");
				struct in6_addr ip;
				getNeighbour_Ip(tlv, &ip);
				in_port_t port = getNeighbour_Port(tlv);
				printf("\t\tIP : %s\n", ip.s6_addr);
				printf("\t\tPort : %"PRIu16"\n", port);
				
				/*//put the Neighbour in the NeighbourList
				
				struct ListVoisin * tmp = malloc(sizeof(struct ListVoisin));
				memset(tmp, 0, sizeof(struct ListVoisin));
				
				tmp = list_voisin;
				while(tmp->suite != NULL){
					tmp->suite = init_ListVoisin();
					tmp = tmp->suite;
				}
				
				tmp->voisin->ip = ip;
				tmp->voisin->port = port;
				tmp->id = source_id;//NON !?

				printf("\t\tSource_Id : %"PRIu64"\n", tmp->id);
				date_short = asctime(gmtime(&timep));
				printf("\t\tDate_Short : %s", date_short);
				memcpy(tmp->date, date_short, strlen(date_short));
			      	
				date_long = asctime(gmtime(&timep));
				printf("\t\tDate_Long : %s", date_long);
				memcpy(tmp->date_long, date_long, strlen(date_long));
		
				//printf("\t\tid de list_voisin : %"PRIu64"\n", list_voisin->id);
				//free(tmp);*/	
				break;

			case 4 : 
				printf("{\n\tData\n");
				printf("\t\tSource : %"PRIu64"\n", getData_Sender_Id(tlv));
				printf("\t\tNonce : %"PRIu32"\n", getData_Nonce(tlv));
				printf("\t\tType : %"PRIu8"\n", getData_Type(tlv));
				memcpy(txt, getData_Donnees(tlv), getData_Donnees_Taille(tlv));
				printf("\t\tDonnées : %s\n", txt);
				break;

			case 5 : 
				printf("{\n\tAck\n");
				printf("\t\tSource : %"PRIu64"\n", getAck_Sender_Id(tlv));
				printf("\t\tNonce : %"PRIu32"\n", getAck_Nonce(tlv));
				break;

			case 6 : 
				printf("{\n\tGoAway\n");
				printf("\t\tCode : %"PRIu8"\n", getGoAway_Code(tlv));
				memcpy(txt, getGoAway_Message(tlv), getGoAway_Message_Taille(tlv));
				printf("\t\tMessage : %s\n", txt);
				break;

			case 7 : 
				printf("{\n\tWarning\n");
				memcpy(txt, getWarning_Message(tlv), getWarning_Message_Taille(tlv));
				printf("\t\tMessage : %s\n", txt);
				break;

			default : 
				printf("{\n\tInconnu\n");

		}

		printf("}\n");

		i += (tlv_len+TLV_ENTETE);

	}
	printf("\nTime : %s \n", asctime(gmtime(&timep)));

	printf("///////////////////////////////\n");

	return 0;

}











