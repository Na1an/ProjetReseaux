#ifndef LIST_H
#define LIST

#include "include.h"

/*List Général*/

/**
	* Liste chainée d'élément
	* @param void * ob : l'élément de la List
	* @param struct List * suite : suite de la List
	*/
struct List {
	void * objet;
	struct List * suite;
};

/**
	* Ajoute un élément à une List
	* @param void * ob : l'élément
	* @param struct List * list : la List
	* @return la nouvelle List
	*/
struct List * add_List(void * ob, struct List * list);

/**
	* Créer une List circulaire
	* @param int size : taille de la List
	* @return la nouvelle List
	*/
struct List * create_Circ_List(int size);

/**
	* Remplit une List circulaire avec un élément
	* @param void * ob : l'élément
	* @param struct List * list : la List
	* @return la nouvelle List
	*/
struct List * add_Circ_List(void * ob, struct List * list);

/**
	* Efface une List
	* @param struct List * l : la List
	* @param f_obj clearObjet : fonction qui efface l'objet
	* @return 0
	*/
int clear_List(struct List * l, f_obj clearObjet);

/**
	* Efface une List Circulaire
	* @param struct List * l : la List
	* @param f_obj clearObjet : fonction qui efface l'objet
	* @return 0
	*/
int clear_Circ_List(struct List * l, f_obj clearObjet);

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
	* Efface un Index_Voisin de la mémoire
	* @param struct Index_Voisin * iv : le Index_Voisin
	* @return 0
	*/
int clear_Index_Voisin(struct Index_Voisin * iv);

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

/**
	* Efface un Voisin de la mémoire
	* @param struct Voisin * v : le Voisin
	* @return 0
	*/
int clear_Voisin(struct Voisin * v);

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
	* Efface un Index_Donnee de la mémoire
	* @param struct Index_Donnee * id : le Index_Donnee
	* @return 0
	*/
int clear_Index_Donnee(struct Index_Donnee * id);

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
	* Ajoute un Event dans une List de Event
	* @param struct Event * e : le Event
	* @param struct List * l : la List
	* @return la Nouvelle List
	*/
struct List * add_List_Event(struct Event * e, struct List * l);

/**
	* Insert un temps dans un Event
	* @param struct Event * e : le Event
	* @param int sec : le nombre de second restant 
	* @param int usec : le nombre de micro-second restant 
	* @return 0
	*/
int setEventTime(struct Event * e, int sec);

/**
	* Compare 2 timevals
	* @param struct timeval * tv1 : le timeval 1
	* @param struct timeval * tv2 : le timeval 2
	* @return < 0 <=> tv1 < tv2
	*/
int tvcmp(struct timeval * tv1, struct timeval * tv2);

/**
	* Trouve un Voisin dans une List 
	* @param struct Index_Voisin * iv : l'Index_Voisin du Voisin a chercher
	* @param struct List * l : la List dans laquelle chercher
	* @return le Voisin
	*/
struct Voisin * find_Voisin(struct Index_Voisin * iv, struct List * l);

#endif
