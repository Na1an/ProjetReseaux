#include "pair.h"
#include "tlvFct.h"
#include "list.h"

int get_socket_and_pair_addr(int *sock, struct sockaddr_in6 *addr, char * pair, int port) {

	char string_port[10];
	snprintf(string_port,10,"%d",port);

	memset(addr, 0, sizeof(struct sockaddr_in6));

	struct addrinfo hints, *r, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_V4MAPPED | AI_ALL;
	hints.ai_protocol = 0;

	if ((getaddrinfo(pair, string_port, &hints, &r)) != 0 || NULL == r) {
		perror("getaddrinfo");
		return -1;
	}

	/* Init sock */
	for (
		p = r;
		((NULL != p) && ((*sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0));
		p = p->ai_next
	);

	if (NULL == p) {
		perror("create socket");
		return -2;
	}

	if(DEBUG) {
		printf("sock : %d\n", *sock);
	}

	/* Init addr */
	*addr = *((struct sockaddr_in6*) p->ai_addr);

	freeaddrinfo(r);

	return 0;
}

int set_addr_pair(int *sock) {

	int rc;

	*sock = socket(AF_INET6, SOCK_DGRAM, 0);

	struct sockaddr_in6 server;

	memset(&server, 0, sizeof(server));
	server.sin6_family = AF_INET6;
	server.sin6_port = htons(MON_PORT);

	/* Set Option sock */

	int val_0 = 0;
	int val_1 = 1;
	
	rc = setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &val_1, sizeof(val_1));
	if(rc < 0) {
		perror("setsockopt - SOL_REUSEADDR");
		fprintf(stderr, "Error: Set Option.\n");
		exit(EXIT_FAILURE);
	}
	
	rc = setsockopt(*sock, IPPROTO_IPV6, IPV6_V6ONLY, &val_0, sizeof(val_0));
	if(rc < 0) {
		perror("setsockopt - IPV6_V6ONLY");
		fprintf(stderr, "Error: Set Option.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
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

struct Voisin * premier_Voisin() {
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
	(v->index).ip = p.sin6_addr;
	(v->index).port = p.sin6_port;

	return v;
}

int get_voisin_addr(struct Voisin * v, struct sockaddr_in6 *addr) {

	memset(addr, 0, sizeof(*addr));

	addr->sin6_family = AF_INET6;
	addr->sin6_port = v->index.port;
	addr->sin6_addr = v->index.ip;

	return 0;
}

int main() {

	int sock, rc;
	struct List * list_voisin_potentiel = add_List(premier_Voisin(), NULL);

	set_addr_pair(&sock);

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

	createMsg(sendMsg);

	int size = setMsg_Body(sendMsg, hello_s, hello_s_len);

	//On recupere l'adresse du Prof
	struct sockaddr_in6 dest_addr;
	get_voisin_addr((struct Voisin *)list_voisin_potentiel->objet, &dest_addr);


	if(DEBUG && VUE) {
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
		if(fork() == 0) {
			uint64_t idProf = getHello_Source_Id(getMsg_Tlv(recvMsg, 0));

			char hello_l[BUF_SIZE];

			int hello_l_len = createHello_long(hello_l, id, idProf);

			char monhellolong[BUF_SIZE];

			createMsg(monhellolong);

			int masize = setMsg_Body(monhellolong, hello_l, hello_l_len);

			struct timeval mon_tv;

			ici:

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
			goto ici;

		}
	}

	goto again2;

	close(sock);

	return 0;
}


