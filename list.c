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

/*Juste Un Test*/
/*int printList(struct List * l) {

	if(l != NULL) {
		if(l->objet != NULL) {printf("%d\n", ((struct Voisin *)l->objet)->a);}
		printList(l->suite);
	}
	
	return 0;
}*/

/*
int main() {

	/ Create Voisin /

	struct Voisin * v1 = malloc(sizeof(struct Voisin));
	memset(v1, 0, sizeof(struct Voisin));
	v1->a = 3;

	struct Voisin * v2 = malloc(sizeof(struct Voisin));
	memset(v2, 0, sizeof(struct Voisin));
	v2->a = 7;

	/ Create List /

	struct List * lv2 = add_List(v2, NULL);
	struct List * lv1 = add_List(v1, lv2);

	/ Create Donnee /

	struct Donnee * d1 = malloc(sizeof(struct Donnee));
	memset(d1, 0, sizeof(struct Donnee));
	d1->donnees = "Donnée 1";
	d1->l = lv1;

	struct Donnee * d2 = malloc(sizeof(struct Donnee));
	memset(d2, 0, sizeof(struct Donnee));
	d2->donnees = "Donnée 2";
	d2->l = lv2;

	/ Create List Circulaire /

	struct List * lc = create_Circ_List(8);
	add_Circ_List(d1, add_Circ_List(NULL, add_Circ_List(NULL, add_Circ_List(d2, add_Circ_List(NULL, add_Circ_List(NULL, add_Circ_List(NULL, add_Circ_List(NULL, lc))))))));

	/ Affichage /

	struct Donnee * d;

	int i = 0;

	printf("Start\n");

	for(struct List * l = lc; l->suite != lc; l = l->suite) {
		i++;
		printf("Data %d :\n", i);
		d = (struct Donnee *)l->objet;
		if(d != NULL) {
			printf("\tDonnées : %s\n", d->donnees);
			printList(d->l);
		} else {
			printf("\tPas de Données\n");
		}
	}

	printf("Over\n");

	return 0;
}
*/

