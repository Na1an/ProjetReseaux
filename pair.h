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
	* Efface une Base de la mémoire
	* @param struct Base * base : la Base
	* @return 0
	*/
int clear_Base(struct Base * base);

/**
	* Compare 2 timevals
	* @param struct timeval * tv1 : le timeval 1
	* @param struct timeval * tv2 : le timeval 2
	* @return < 0 <=> tv1 < tv2
	*/
int tvcmp(struct timeval * tv1, struct timeval * tv2);

/**
	* Efface un Voisin d'une Base
	* @param struct Index_Voisin * iv : l'Index_Voisin du Voisin
	* @param struct Base * b : la Base
	* @return 0
	*/
int rmVoisin(struct Index_Voisin * iv, struct Base * b);

#endif
