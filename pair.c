#include "pair.h"
#include "tlvFct.h"
#include "list.h"

struct Index_Voisin * premier_Voisin() {

	int sock;

	char string_port[10];
	snprintf(string_port, 10, "%d", PROF_PORT);

	struct addrinfo hints, *r, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_V4MAPPED | AI_ALL;
	hints.ai_protocol = 0;

	if ((getaddrinfo(PROF_ADDR, string_port, &hints, &r)) != 0 || NULL == r) {
		perror("getaddrinfo");
		fprintf(stderr, "Error: Find host.\n");
		exit(EXIT_FAILURE);
	}

	/* Init sock */
	for (
		p = r;
		((NULL != p) && ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0));
		p = p->ai_next
	);

	if (NULL == p) {
		perror("create socket");
		fprintf(stderr, "Error: Creat sock.\n");
		exit(EXIT_FAILURE);
	}

	if(DEBUG) {
		printf("sock : %d\n", sock);
	}

	/* Init Voisin */

	struct Index_Voisin * iv = malloc(sizeof(struct Index_Voisin));
	memset(iv, 0, sizeof(struct Index_Voisin));

	iv->ip = ((struct sockaddr_in6*) p->ai_addr)->sin6_addr;
	iv->port = ((struct sockaddr_in6*) p->ai_addr)->sin6_port;

	close(sock);

	freeaddrinfo(r);

	return iv;
}

int set_addr_pair() {

	int sock, rc;

	sock = socket(AF_INET6, SOCK_DGRAM, 0);

	struct sockaddr_in6 server;

	memset(&server, 0, sizeof(server));
	server.sin6_family = AF_INET6;
	server.sin6_port = htons(MON_PORT);

	/* Set Option sock */

	int val_0 = 0;
	int val_1 = 1;
	
	rc = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val_1, sizeof(val_1));
	if(rc < 0) {
		perror("setsockopt - SOL_REUSEADDR");
		fprintf(stderr, "Error: Set Option.\n");
		exit(EXIT_FAILURE);
	}
	
	rc = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &val_0, sizeof(val_0));
	if(rc < 0) {
		perror("setsockopt - IPV6_V6ONLY");
		fprintf(stderr, "Error: Set Option.\n");
		exit(EXIT_FAILURE);
	}

	return sock;
}

//MEMOIRE ALOUEE POUR ListVoisin ET TOUS SES Voisin !!
/*struct ListVoisinPotentiel * init_ListVoisinPotentiel() {

	int s;
	struct sockaddr_in6 p;

	switch (get_socket_and_pair_addr(&s, &p, PROF_ADDR, PROF_PORT)) {
		case -1:
			fprintf(stderr, "Error: Find host.\n");
			exit(EXIT_FAILURE);
		case -2:
			fprintf(stderr, "Error: Creat sock.\n");
			exit(EXIT_FAILURE);
	}

	close(s);

	struct Voisin *	v = malloc(sizeof(struct Voisin));
	memset(v, 0, sizeof(struct Voisin));
	v->ip = p.sin6_addr;
	v->port = p.sin6_port;

	struct ListVoisinPotentiel * lv = malloc(sizeof(struct ListVoisinPotentiel));
	memset(lv, 0, sizeof(struct ListVoisinPotentiel));
	lv->voisin = v;
	lv->suite = NULL;
	
	return lv;
}

struct ListVoisin * init_ListVoisin(){
	
	struct Voisin *	v = malloc(sizeof(struct Voisin));
	memset(v, 0, sizeof(struct Voisin));
	memset(&v->ip, 0, sizeof(v->ip));
	memset(&v->port, 0, sizeof(v->port));

	struct ListVoisin * lv = malloc(sizeof(struct ListVoisin));
	memset(lv, 0, sizeof(struct ListVoisin));
	lv->voisin = v;
	lv->id = 0;
	memset(lv->date, 0, 30);
	memset(lv->date_long, 0, 30);
	//memset(&lv->suite, 0, sizeof(struct ListVoisin));
	lv->suite = NULL;

	return lv;
}*/

int get_voisin_addr(struct Index_Voisin * iv, struct sockaddr_in6 *addr) {

	memset(addr, 0, sizeof(*addr));

	addr->sin6_family = AF_INET6;
	addr->sin6_port = iv->port;
	addr->sin6_addr = iv->ip;

	return 0;
}

int traitement(char * msg) {

	time_t timep;
	time(&timep);
	/*//printf("%s", asctime(gmtime(&timep)));
	char * date_short = 0;//[26];
	char * date_long = 0;//[26];*/

	uint16_t body_len = getMsg_Body_Length(msg);

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

			case 0 : //Pad1 : Rien
				break;

			case 1 : //PadN : Rien
				break;

			case 2 : //Hello
				if(tlv_len == 8) {
					
				} else if(tlv_len == 16) {
					
				} else {
					
				}
				break;//TODO FAIRE des clear de Malloc -> list, voisin, data ,index -> ensuite : Neightbour TODO

			case 3 : //Neighbour
				//struct in6_addr ip;
				//getNeighbour_Ip(tlv, &ip);
				//in_port_t port = getNeighbour_Port(tlv);
				
				/*//put the Neighbour in the NeightbourList
				
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

			case 4 : //Data
				break;

			case 5 : //Ack : 
				break;

			case 6 : 
				if(VUE) {
					memcpy(txt, getGoAway_Message(tlv), getGoAway_Message_Taille(tlv));
					printf("GoAway : Code %"PRIu8", Message : %s\n", getGoAway_Code(tlv), txt);
				}
				break;

			case 7 : 
				if(VUE) {
					memcpy(txt, getWarning_Message(tlv), getWarning_Message_Taille(tlv));
					printf("Warning : %s\n", txt);
				}
				break;

			default : 
				if(VUE) {
					printf("Inconnu");
				}

		}

		i += (tlv_len+TLV_ENTETE);

	}
	printf("\nTime : %s \n", asctime(gmtime(&timep)));

	return 0;
}

int main() {

	int sock, rc;

	struct List * list_voisin_potentiel = add_List(premier_Voisin(), NULL);
	struct List * list_voisin = NULL;
	struct List * list_voisin_symetrique = NULL;//Utile ???

	struct List * list_data = create_Circ_List(8);

	sock = set_addr_pair();

	//struct ListVoisinPotentiel * potentiels = init_ListVoisinPotentiel();

	srand (time (NULL));

	uint64_t id =
	(((uint64_t) rand() <<  0) & 0x000000000000FFFFull) | 
	(((uint64_t) rand() << 16) & 0x00000000FFFF0000ull) | 
	(((uint64_t) rand() << 32) & 0x0000FFFF00000000ull) |
	(((uint64_t) rand() << 48) & 0xFFFF000000000000ull);

	if(DEBUG) {
		printf("Id = %" PRIu64 "\n", id);
	}

	struct timeval tv;
	fd_set readfds;
	
	char hello_s[BUF_SIZE];

	int hello_s_len = createHello_short(hello_s, id);

	//Fin de l'Initialisation

	char sendMsg[BUF_SIZE], recvMsg[BUF_SIZE];
	memset(sendMsg, 0, BUF_SIZE);
	memset(recvMsg, 0, BUF_SIZE);

	createMsg(sendMsg);

	int size = setMsg_Body(sendMsg, hello_s, hello_s_len);

	//On recupere l'adresse du Prof
	struct sockaddr_in6 dest_addr;
	get_voisin_addr((struct Index_Voisin *)list_voisin_potentiel->objet, &dest_addr);


	if(DEBUG) {
		//On verifie les données
		printMsg(sendMsg);
	}

	int premier = 0;

	again:

	rc = sendto(sock, &sendMsg, size, 0, (struct sockaddr *)&dest_addr, (socklen_t)sizeof(dest_addr));
	
	if(rc < 0) {
		perror("sendto");
		exit(EXIT_FAILURE);
	}

	if(DEBUG) {
		printf("Message Envoyé ! : %s\n",sendMsg);
	}

	again2:

	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);

	tv.tv_sec  = 180;
	tv.tv_usec  = 0;

	rc = select(sock + 1, &readfds, NULL, NULL, &tv);

	if(rc < 0) {
		perror("select");
		exit(EXIT_FAILURE);
	}

	if(rc > 0) {
		if(FD_ISSET(sock, &readfds)) {
			rc = recvfrom(sock, recvMsg, BUF_SIZE, 0, NULL, NULL);//NE PAS METTRE NULL, NULL !!!
			if(rc < 0) {
				if(errno == EAGAIN) {
					goto again;
				} else {
					perror("recvfrom");
					exit(EXIT_FAILURE);
				}
			}
			goto ok;
		} else {
			perror("Descripteur de fichier inattendu");
			exit(EXIT_FAILURE);
		}
	} else {/* timeout */
		if(DEBUG) {
			printf("Encore !\n");
		}
		goto again;
	}
  
	ok:

	if(DEBUG) {
		printf("Message Reçu ! : %s\n", recvMsg);
	}

	if(DEBUG) {
		printMsg(recvMsg);
	}

	if(premier == 0) {
		premier++;
		if(fork() == 0) {//LORSQUE L ON RECOIS LE PREMIER HELLO_LONG DU PROF, ON LUI ENVOIE UN HELLO_LONG TOUTES LES 30 SEC
			uint64_t idProf = getHello_Source_Id(getMsg_Tlv(recvMsg, 0));//Id du Prof

			printf("Id Prof : %"PRIu64"\n", idProf);

			char monhellolong[BUF_SIZE], hello_l[BUF_SIZE];

			int hello_l_len = createHello_long(hello_l, id, idProf);

			createMsg(monhellolong);

			int masize = setMsg_Body(monhellolong, hello_l, hello_l_len);

			struct timeval mon_tv;

			encore_long:

			mon_tv.tv_sec  = 30;
			mon_tv.tv_usec  = 0;

			rc = select(sock + 1, NULL, NULL, NULL, &mon_tv);

			if(rc < 0) {
				perror("select2");
				exit(EXIT_FAILURE);
			}

			if(rc > 0) {
				printf("WTF !!\n");
			} else {/* timeout */
				rc = sendto(sock, &monhellolong, masize, 0, (struct sockaddr *)&dest_addr, (socklen_t)sizeof(dest_addr));
				if(rc < 0) {
					perror("sendto2");
					exit(EXIT_FAILURE);
				}
				printf("Message LONG Envoyé ! : %s\n",sendMsg);
			}
			goto encore_long;

		}
	}

	memset(recvMsg, 0, BUF_SIZE);

	goto again2;

	clear_List(list_voisin_potentiel, (f_obj)&clear_Index_Voisin);
	clear_List(list_voisin, (f_obj)&clear_Voisin);
	clear_List(list_voisin_symetrique, (f_obj)&clear_Voisin);

	clear_List(list_data, (f_obj)&clear_Donnee);

	close(sock);

	return 0;
}


