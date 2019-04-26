#ifndef LIST_H
#define LIST

#include "include.h"

/* Structures */

/*List en Général*/

/**
	* Liste chainée d'élément
	* @param void * ob : l'élément de la List
	* @param struct List * suite : suite de la List
	*/
struct List {
	void * objet;
	struct List * suite;
};

/*Voisins*/

/**
	* Index unique des Voisin
	* @param struct in6_addr ip : addresse ip du Voisin
	* @param in_port_t port : port du Voisin
	*/
struct Index_Voisin {
	struct in6_addr ip;
	in_port_t port;
};

/**
	Atteint l objet Index_Voisin d'une liste
	*/
#define I_VOISIN(_l) ((struct Index_Voisin *)((_l)->objet))

/**
	* Voisin dans le réseau
	* @param struct Index_Voisin * index : index du Voisin
	* @param uint64_t id : id du Voisin globalement unique
	* @param struct timeval date : date du dernier Hello reçu du Voisin
	* @param struct timeval date_long : date du dernier Hello_Long reçu du Voisin
	*/
struct Voisin {
	struct Index_Voisin * index;
	uint64_t id;
	struct timeval date;
	struct timeval date_long;
};

/**
	Atteint l objet Voisin d'une liste
	*/
#define VOISIN(_l) ((struct Voisin *)((_l)->objet))

/*Données*/

/**
	* Index unique des Donnee
	* @param uint64_t id : id du Voisin qui nous à envoyer la Donnee
	* @param uint32_t nonce : nonce de la Donnee
	*/
struct Index_Donnee {
	uint64_t id;
	uint32_t nonce;
};

/**
	Atteint l objet Index_Donnee d'une liste
	*/
#define I_DONNEE(_l) ((struct Index_Donnee *)((_l)->objet))

/**
	* Donnee à envoyer
	* @param struct Index_Donnee * index : index de la Donnee
	* @param struct List * l : List de Voisin à qui il faut envoyer la Donnee
	*/
struct Donnee {
	struct Index_Donnee * index;
	struct List * l;
};

/**
	Atteint l objet Donnee d'une liste
	*/
#define DONNEE(_l) ((struct Donnee *)((_l)->objet))

/* Fonctions */

/* Generer */

/**
	* Ajoute un élément à une List
	* @param void * ob : l'élément
	* @param struct List * list : la List
	* @return la nouvelle List
	*/
struct List * add_List(void * ob, struct List * list);

/**
	* Remplit une List circulaire avec un élément
	* @param void * ob : l'élément
	* @param struct List * list : la List
	* @return la nouvelle List
	*/
struct List * add_Circ_List(void * ob, struct List * list);

/**
	* Créer une List circulaire
	* @param int size : taille de la List
	* @return la nouvelle List
	*/
struct List * create_Circ_List(int size);

/* Egalite */

/**
	* Compare deux Index_Voisin
	* @param struct Index_Voisin * v1 : un Index_Voisin
	* @param struct Index_Voisin * v2 : un autre Index_Voisin
	* @return si les deux Index_Voisin sont égaux
	*/
int eqIVoisin(struct Index_Voisin * v1, struct Index_Voisin * v2);

/**
	* Compare deux Index_Donnee
	* @param struct Index_Donnee * d1 : un Index_Donnee
	* @param struct Index_Donnee * d2 : un autre Index_Donnee
	* @return si les deux Index_Donnee sont égaux
	*/
int eqIDonnee(struct Index_Donnee * d1, struct Index_Donnee * d2);

/* Clear */

/**
	* Efface une List de la mémoire
	* @param struct List * l : la List
	* @param f_obj clearObjet : fonction utilisé pour effacer les élément de la List
	* @return 0
	*/
int clear_List(struct List * l, int (*clearObjet)(void *));

/**
	* Efface une List circulaire de la mémoire
	* @param struct List * l : la List
	* @param f_obj clearObjet : fonction utilisé pour effacer les élément de la List
	* @return 0
	*/
int clear_Circ_List(struct List * l, int (*clearObjet)(void *));

/**
	* Efface un Index_Voisin de la mémoire
	* @param struct Index_Voisin * iv : le Index_Voisin
	* @return 0
	*/
int clear_Index_Voisin(struct Index_Voisin * iv);

/**
	* Efface un Voisin de la mémoire
	* @param struct Voisin * v : le Voisin
	* @return 0
	*/
int clear_Voisin(struct Voisin * v);

/**
	* Efface un Index_Donnee de la mémoire
	* @param struct Index_Donnee * id : le Index_Donnee
	* @return 0
	*/
int clear_Index_Donnee(struct Index_Donnee * id);

/**
	* Efface une Donnee de la mémoire
	* @param struct Donnee * d : la Donnee
	* @return 0
	*/
int clear_Donnee(struct Donnee * d);

#endif
