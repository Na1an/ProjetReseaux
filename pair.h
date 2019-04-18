#ifndef PAIR_H
#define PAIR

#include "include.h"
#include "tlvFct.h"
#include "list.h"

/**
	* Le port UDP que j'utilise
	*/
#define MON_PORT 4242

/**
	* Le port UDP utilisé par Mr Chroboczek
	*/
#define PROF_PORT 1212

/**
	* L'hôte que utilisé par Mr Chroboczek
	*/
#define PROF_ADDR "jch.irif.fr"

/**
	* Ensemble d'information utile tout au long de la connection
	* @param uint64_t id : mon Id
	* @param List * list_voisin_potentiel : List de Voisin potentiel
	* @param List * list_voisin : List de Voisin
	* @param List * list_data : List circulaire de Data
	* @param List * list_event : List de Event
	*/
struct Base {
	uint64_t id;
	struct List * list_voisin_potentiel;
	struct List * list_voisin;
	struct List * list_data;
	struct List * list_event;
};

/**
	* Evènement d'envoie d'un message à un destinataire
	* @param struct timeval tv : date d'envoie
	* @param int opt : option
	* @param struct Index_Voisin * dest : le destinataire
	* @param char * msg : le message
	* @param int msg_len : taille du message
	*/
struct Event {
	struct timeval tv;
	int opt;
	struct Index_Voisin * dest;
	char * msg;
	int msg_len;
};

/**
	* Efface un Event de la mémoire
	* @param struct Event * e : le Event
	* @return 0
	*/
int clear_Event(struct Event * e);

/**
	* Efface une Base de la mémoire
	* @param struct Base * base : la Base
	* @return 0
	*/
int clear_Base(struct Base * base);

/**
	* Ajoute un Event dans une List de Event
	* @param struct Event * e : le Event
	* @param struct List * l : la List
	* @return 0
	*/
struct List * add_List_Event(struct Event * e, struct List * l);

#endif
