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

#define EVENT(_l) ((struct Event *)((_l)->objet))

/**
	* Ensemble d'information utile tout au long de la connection
	* @param uint64_t id : mon Id
	* @param List * list_voisin_potentiel : List de Voisin potentiel
	* @param List * list_voisin : List de Voisin
	* @param List * list_data : List circulaire de Index_Donnee
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
	* Evènement d'envoie d'un tlv à un destinataire
	* @param struct timeval tv : date d'envoie
	* @param int opt : option
	* @param struct Index_Voisin * dest : le destinataire
	* @param char * tlv : le tlv
	* @param int tlv_len : taille du tlv
	*/
struct Event {
	struct timeval tv;
	int opt;
	struct Index_Voisin * dest;
	char * tlv;
	int tlv_len;
};

/**
	Atteint l objet Event d'une liste
	*/
#define EVENT(_l) ((struct Event *)((_l)->objet))

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

/**
	* Compare 2 timevals
	* @param struct timeval * tv1 : le timeval 1
	* @param struct timeval * tv2 : le timeval 2
	* @return < 0 <=> tv1 < tv2
	*/
int tvcmp(struct timeval * tv1, struct timeval * tv2);

/**
	* Insert un temps dans un Event
	* @param struct Event * e : le Event
	* @param int sec : le nombre de second restant 
	* @param int usec : le nombre de micro-second restant 
	* @return 0
	*/
int setEventTime(struct Event * e, int sec, int usec);

#endif
