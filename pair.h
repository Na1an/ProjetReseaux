#ifndef PAIR_H
#define PAIR

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

//#include <inttypes.h>
//#include <fcntl.h>
#include <sys/time.h>

#define DEBUG 1
#define VUE 1
#define MON_PORT 4242
#define PROF_PORT 1212
#define PROF_ADDR "jch.irif.fr"

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
#endif
