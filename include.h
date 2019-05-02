#ifndef INCLUDE_H
#define INCLUDE

#define _GNU_SOURCE

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/**
	* Taille maximale des Buffers utilisés
	*/
#define BUF_SIZE 4096

/**
	* Si l'on veut afficher les informations de debugage
	*/
#define DEBUG 1

/**
	* Si l'on veut afficher des informations supplémentaires
	*/
#define VUE 1

/**
	* Si l'on veut afficher le chemin par lequel le programme passe
	*/
#define SHOWPATH 0

/**
	* Fonction qui agit sur un objet
	* @param void * : l'objet
	* @return 0
	*/
typedef int (*f_obj)(void *);

#endif
