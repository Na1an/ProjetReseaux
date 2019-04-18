#include "list.h"

/* Générer */

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

/* Egalité */

int eqIVoisin(struct Index_Voisin * v1, struct Index_Voisin * v2) {
	return (v1->port == v2->port) && (memcmp((v1->ip).s6_addr, (v2->ip).s6_addr, 16) == 0);
}

int eqIDonnee(struct Index_Donnee * d1, struct Index_Donnee * d2) {
	return (d1->id == d2->id) && (d1->nonce == d2->nonce);
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

int clear_Donnee(struct Donnee * d) {
	clear_Index_Donnee(d->index);
	clear_List(d->l, (f_obj)&(clear_Voisin));
	free(d);
	return 0;
}

/* Autre *///TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO 

struct Index_Voisin * get_and_remove(struct List * l, struct Index_Voisin * iv) {
	return iv;
}



