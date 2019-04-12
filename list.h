#ifndef LIST_H
#define LIST

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <errno.h>
#include <sys/select.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <netdb.h>

#include <time.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#define BUF_SIZE 4096

//#include <inttypes.h>
//#include <fcntl.h>
#include <sys/time.h>

/* Structures */

/*List en Général*/

struct List {
	void * objet;
	struct List * suite;
};

/*Voisins*/

struct Index_Voisin {
	struct in6_addr ip;
	in_port_t port;
};

struct Voisin {
	struct Index_Voisin index;
	uint64_t id;
	char date[26];
	char date_long[26];
};

/*Données*/

struct Index_Donnee {
	uint64_t id;
	uint32_t nonce;
};

struct Donnee {
	struct Index_Donnee index;
	struct List * l;
};

/* Fonctions */

struct List * add_List(void * ob, struct List * list);

struct List * add_Circ_List(void * ob, struct List * list);

struct List * create_Circ_List(int size);


#endif
