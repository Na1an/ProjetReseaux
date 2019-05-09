#include "list.h"

struct List * add_List(void * ob, struct List * list) {
	struct List * l = malloc(sizeof(struct List));
	memset(l, 0, sizeof(struct List));
	l->objet = ob;
	l->suite = list;
	return l;
}

struct List * add_Circ_List(void * ob, struct List * list) {
	struct List * l;
	for(l = list; l->suite != list; l=l->suite);
	l->objet = ob;
	return l;
}

struct List * create_Circ_List(int size) {
	struct List * l = malloc(sizeof(struct List));
	memset(l, 0, sizeof(struct List));
	l->objet = NULL;
	struct List * ls = l; 
	for(int i = 0; i < size; i++) {
		ls = add_List(NULL, ls);
	}
	l->suite = ls;
	
	return l;
}

/* Clear */

int clear_List(struct List * l, f_obj clearObjet) {
	if(l != NULL) {
		clearObjet(l->objet);
		clear_List(l->suite, clearObjet);
		free(l);
	}
	return 0;
}

int clear_Circ_List(struct List * l, f_obj clearObjet) {
	if(l != NULL) {
		struct List * s = l->suite;
		l->suite = NULL;
		clear_List(s, clearObjet);
	}
	return 0;
}

int clear_Index_Voisin(struct Index_Voisin * iv) {
	free(iv);
	return 0;
}

int clear_Voisin(struct Voisin * v) {
	clear_Index_Voisin(v->index);
	free(v);
	return 0;
}

int clear_Index_Donnee(struct Index_Donnee * id) {
	free(id);
	return 0;
}

int clear_Event(struct Event * e) {
	free(e->dest);
	free(e->tlv);
	free(e);
	return 0;
}

/* Autre */

struct List * add_List_Event(struct Event * e, struct List * l) {
	if(l != NULL && tvcmp(&(e->tv), &(EVENT(l)->tv)) < 0) {
		l->suite = add_List_Event(e, l->suite);
		return l;
	} else {
		return add_List(e, l);
	}
}

int setEventTime(struct Event * e, int sec) {
	gettimeofday(&(e->tv), NULL);
	(e->tv).tv_sec += sec;
	return 0;
}

int tvcmp(struct timeval * tv1, struct timeval * tv2) {
	return tv2->tv_sec - tv1->tv_sec;
}

struct Voisin * find_Voisin(struct Index_Voisin * iv, struct List * l) {
	if(l == NULL) {return NULL;}
	if(VOISIN(l) != NULL && memcmp(VOISIN(l)->index, iv, sizeof(struct Index_Voisin)) == 0) {
		return VOISIN(l);
	}
	return find_Voisin(iv, l->suite);
}

