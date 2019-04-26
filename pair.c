#include "pair.h"

/* Initialisation */

int set_addr_pair() {

	int sock, rc, val_0 = 0, val_1 = 1;

	sock = socket(AF_INET6, SOCK_DGRAM, 0);

	struct sockaddr_in6 server;

	memset(&server, 0, sizeof(server));
	server.sin6_family = AF_INET6;
	server.sin6_port = htons(MON_PORT);

	/* Set Option sock */

	rc = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val_1, sizeof(val_1));
	if(rc < 0) {perror("setsockopt - SOL_REUSEADDR");return -1;}
	
	rc = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &val_0, sizeof(val_0));
	if(rc < 0) {perror("setsockopt - IPV6_V6ONLY");return -1;}

	rc = fcntl(sock, F_GETFL);
	if(rc < 0) {perror("fcntls - get");return -1;}

	rc = fcntl(sock, F_SETFL, rc | O_NONBLOCK);
	if(rc < 0) {perror("fcntls - set");return -1;}

	if(VUE) {printf("System :\n\tSocket = %d\n", sock);}

	return sock;
}

struct Index_Voisin * premier_Voisin() {

	int sock;

	char string_port[10];
	snprintf(string_port, 10, "%d", PROF_PORT);

	struct addrinfo hints, *r, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_V4MAPPED | AI_ALL;
	hints.ai_protocol = 0;

	if ((getaddrinfo(PROF_ADDR, string_port, &hints, &r)) != 0 || NULL == r) {perror("getaddrinfo");return NULL;}

	/* Init sock */
	for (
		p = r;
		((NULL != p) && ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0));
		p = p->ai_next
	);

	if (NULL == p) {perror("create socket");return NULL;}

	/* Init Voisin */

	struct Index_Voisin * iv = malloc(sizeof(struct Index_Voisin));
	memset(iv, 0, sizeof(struct Index_Voisin));

	iv->ip = ((struct sockaddr_in6*) p->ai_addr)->sin6_addr;
	iv->port = ((struct sockaddr_in6*) p->ai_addr)->sin6_port;

	close(sock);

	freeaddrinfo(r);

	return iv;
}

struct Event * premier_Event(struct Index_Voisin * iv, uint64_t id) {
	struct Event * e = malloc(sizeof(struct Event));
	memset(e, 0, sizeof(struct Event));

	gettimeofday(&(e->tv), NULL);
	e->dest = iv;
	e->msg = malloc(BUF_SIZE);
	createMsg(e->msg);
	
	char hello_s[BUF_SIZE];
	int hello_s_len = createHello_short(hello_s, id);

	e->msg_len = setMsg_Body(e->msg, hello_s, hello_s_len);

	return e;
}

int setBase(struct Base * b, uint64_t id) {
	memset(b, 0, sizeof(struct Base));

	b->id = id;

	b->list_voisin_potentiel = add_List(premier_Voisin(), NULL);
	if(b->list_voisin_potentiel == NULL) {return -1;}

	b->list_voisin = NULL;

	b->list_data = create_Circ_List(32);//TODO 8 sûr?

	b->list_event = add_List(premier_Event(I_VOISIN(b->list_voisin_potentiel), id), NULL);

	return 0;
}

/* Fonctions Boucle */

int get_voisin_addr(struct Index_Voisin * iv, struct sockaddr_in6 * addr) {

	memset(addr, 0, sizeof(struct sockaddr_in6));

	addr->sin6_family = AF_INET6;
	addr->sin6_port = iv->port;
	addr->sin6_addr = iv->ip;

	return 0;
}

//Faire traitement envoie et traitement recois
int traitement_recv(int sock, struct Base * base) {

	int rc;

	struct sockaddr_in6 from;
	memset(&from, 0, sizeof(struct sockaddr_in6));
	socklen_t from_len = sizeof(from);

	char recvMsg[BUF_SIZE];
	memset(recvMsg, 0, BUF_SIZE);

	rc = recvfrom(sock, recvMsg, BUF_SIZE, 0, (struct sockaddr *)&from, &from_len);
	if(rc < 0) {
		if(errno == EAGAIN) {
			return 1;//Veut-on vraiment prendre en compte !!!TODO
		} else {perror("recvfrom");return -1;}
	}

	if(DEBUG) {printf("Message Reçu !\n");printMsg(recvMsg);}

	struct timeval now;

	gettimeofday(&now, NULL);

	switch(getMsg_Magic(recvMsg)) {
		case 93 : break;
		default : printf("System Recv :\n\tUnknow Magic\n");return 2;
	}
	switch(getMsg_Version(recvMsg)) {
		case 2 : break;
		default : printf("System Recv :\n\tUnknow Version\n");return 2;
	}

	uint16_t body_len = getMsg_Body_Length(recvMsg);

	if(body_len + MSG_ENTETE > BUF_SIZE) {printf("System Recv :\n\tToo Much Length\n");return 2;}

	char * tlv;
	uint8_t type;
	uint8_t tlv_len;

	struct Index_Voisin * iv_actuel = malloc(sizeof(struct Index_Voisin));
	memset(iv_actuel, 0, sizeof(struct Index_Voisin));
	iv_actuel->ip = from.sin6_addr;
	iv_actuel->port = from.sin6_port;

	char txt[BUF_SIZE];
	int k;
	struct Index_Voisin * iv;
	struct Voisin * v ;
	struct Index_Donnee * id;
	struct Event * e;
	struct List * l;
	struct List * aux;
	struct timeval tv;

	int i = 0;
	
	//uint64_t source_id = 0;
	while(i < body_len) {

		memset(txt, 0, BUF_SIZE);

		tlv = getMsg_Tlv(recvMsg, i);

		type = getTlv_Type(tlv);

		tlv_len = getTlv_Length(tlv);

		switch(type) {

			case 0 : //Pad1
				if(VUE) {printf("System Recv :\n\tPad1\n");}
				break;

			case 1 : //PadN
				if(VUE) {printf("System Recv :\n\tPadN\n");}
				break;

			case 2 : //Hello
				if(tlv_len == 8 || (tlv_len == 16 && base->id != getHello_long_Destination_Id(tlv))) {
					for(
						l = base->list_voisin;
						l != NULL && memcmp(VOISIN(l)->index, iv_actuel, sizeof(struct Index_Voisin)) != 0;
						l = l->suite
					);
					if(l != NULL) {//Déjà Voisin
						v  = VOISIN(l);
						v->date = now;
						if(tlv_len == 16) {v->date_long = now;}//Long
						
					} else {//Nouveau Voisin

						l = base->list_voisin_potentiel;

						if(l != NULL) {//On le supprime de la List de Voisin Potentiel
							if(memcmp((struct Index_Voisin *)l->objet, iv_actuel, sizeof(struct Index_Voisin)) == 0) {
								base->list_voisin_potentiel = l->suite;
								free(l->objet);
								free(l);
							} else {
								while(l->suite != NULL && memcmp(I_VOISIN(l->suite), iv_actuel, sizeof(struct Index_Voisin)) != 0) {
									l = l->suite;
								}
								if(l->suite != NULL) {
									aux = l->suite;
									l->suite = aux->suite;
									free(aux->objet);
									free(aux);
								}
							}

						}

						//On l'ajoute au Voisin

						v = malloc(sizeof(struct Voisin));
						memset(v, 0, sizeof(struct Voisin));
						memcpy(v->index, iv_actuel, sizeof(struct Index_Voisin));
						v->id = getHello_Source_Id(tlv);
						v->date = now;
						if(tlv_len == 16) {v->date_long = now;}//Long

						base->list_voisin = add_List(v, base->list_voisin);

						//On lui envoie des Hello_long

						e = malloc(sizeof(struct Event));
						memset(e, 0, sizeof(struct Event));

						setEventTime(e, 0, 0);
						e->dest = v->index;
						e->msg = malloc(BUF_SIZE);
						createMsg(e->msg);
	
						k = createHello_long(txt, base->id, v->id);

						e->msg_len = setMsg_Body(e->msg, txt, k);

						base->list_event = add_List_Event(e, base->list_event);
					}
				}
				break;

			case 3 : //Neighbour
				iv = malloc(sizeof(struct Index_Voisin));
				memset(iv, 0, sizeof(struct Index_Voisin));
				getNeighbour_Ip(tlv, &(iv->ip));
				iv->port = getNeighbour_Port(tlv);
				for(
					l = base->list_voisin_potentiel;
					l != NULL && memcmp(I_VOISIN(l), iv, sizeof(struct Index_Voisin)) != 0;
					l = l->suite
				);
				for(
					aux = base->list_voisin;
					aux != NULL && memcmp(VOISIN(aux)->index, iv, sizeof(struct Index_Voisin)) != 0;
					aux = aux->suite
				);
				if(l != NULL || aux != NULL) {//Déjà Connu(e)
					free(iv);break;
				} else {//On l ajoute au Voisin Potentiel
					base->list_voisin_potentiel = add_List(iv, base->list_voisin_potentiel);
				}

				k = 0;

				for(aux = base->list_voisin; aux != NULL; aux = aux->suite) {k++;}

				if(k < 8 && base->list_voisin_potentiel != NULL) {//TODO ?? Voir Event a Dest NULL
					//envoyer au premier si pas de reponse -> enlever au bout de (Comme ack)
				}

				break;

			case 4 : //Data
				id = malloc(sizeof(struct Index_Donnee));
				memset(id, 0, sizeof(struct Index_Donnee));
				id->id = getData_Sender_Id(tlv);
				id->nonce = getData_Nonce(tlv);
				for(
					l = base->list_data;
					l != NULL && memcmp(((struct Donnee *)l->objet)->index, id, sizeof(struct Index_Donnee)) != 0;
				);

				if(l == NULL) {//On l envoie a chaque Voisin
					gettimeofday(&tv, NULL);
					for(l = base->list_voisin; l != NULL; l = l->suite) {
						if(tvcmp(&VOISIN(l)->date_long, &tv) < 30) {
							e = malloc(sizeof(struct Event));
							memset(e, 0, sizeof(struct Event));

							e->opt = 1;

							setEventTime(e, 0, 1000*(rand()%2000));
							(e->tv).tv_usec += (rand())%2000;
							e->dest = VOISIN(l)->index;
							e->msg = malloc(BUF_SIZE);
							createMsg(e->msg);

							e->msg_len = setMsg_Body(e->msg, tlv, tlv_len);

							base->list_event = add_List_Event(e, base->list_event);
						}
					}
				}

				//On acquitte l envoyeur

				e = malloc(sizeof(struct Event));
				memset(e, 0, sizeof(struct Event));

				setEventTime(e, 0, 0);
				e->dest = iv_actuel;
				e->msg = malloc(BUF_SIZE);
				createMsg(e->msg);

				k = createAck(txt, id->id, id->nonce);

				e->msg_len = setMsg_Body(e->msg, txt, k);

				base->list_event = add_List_Event(e, base->list_event);

				type = 5;//On n envoie pas a l envoyeur

			case 5 : //Ack :
				for(l = base->list_event; l != NULL; l = l->suite) {
					if(EVENT(l->suite)->dest != NULL && memcmp(EVENT(l->suite)->dest, iv_actuel, sizeof(struct Index_Voisin)) == 0 && opt < 0) {//On suprime le prochain envoies
						l->suite = (l->suite)->suite;//TODO REMOVE EVENT
					}
				}
				break;

			case 6 : 
				if(VUE) {
					memcpy(txt, getGoAway_Message(tlv), getGoAway_Message_Taille(tlv));
					printf("System Recv :\n\tGoAway : Code %"PRIu8", Message : %s\n", getGoAway_Code(tlv), txt);
				}
				break;

			case 7 : 
				if(VUE) {
					memcpy(txt, getWarning_Message(tlv), getWarning_Message_Taille(tlv));
					printf("System Recv :\n\tWarning : %s\n", txt);
				}
				break;

			default : 
				if(VUE) {printf("System Recv :\n\tTlv Inconnu : type = %"PRIu8"\n", type);}

		}

		i += (tlv_len+TLV_ENTETE);

	}

	free(iv_actuel);

	return 0;
}

int traitement_send(struct Event * event, struct Base * base) {

	if(event == NULL) {return 1;}

	int rc;

	/*struct sockaddr_in6 from;
	memset(&from, 0, sizeof(struct sockaddr_in6));
	socklen_t from_len = sizeof(from);

	char recvMsg[BUF_SIZE];
	memset(recvMsg, 0, BUF_SIZE);

	rc = recvfrom(sock, recvMsg, BUF_SIZE, 0, (struct sockaddr *)&from, &from_len);//NOT TODO NE PAS SENDTO
	if(rc < 0) {
		if(errno == EAGAIN) {
			return 1;//Veut-on vraiment prendre en compte !!!TODO
		} else {perror("recvfrom");return -1;}
	}*/

	if(event->dest == NULL) {
		return 0;//Faire les neigthbours ou les 8 voisins
	}

	if(DEBUG) {printf("Message Envoyé !");printMsg(event->msg);}

	struct timeval now;

	gettimeofday(&now, NULL);

	switch(getMsg_Magic(event->msg)) {
		case 93 : break;
		default : printf("System Send :\n\tUnknow Magic\n");return 2;
	}
	switch(getMsg_Version(event->msg)) {
		case 2 : break;
		default : printf("System Send :\n\tUnknow Version\n");return 2;
	}

	uint16_t body_len = getMsg_Body_Length(event->msg);

	if(body_len + MSG_ENTETE > BUF_SIZE) {printf("System Send :\n\tToo Much Length\n");return 2;}

	char * tlv;
	uint8_t type;
	uint8_t tlv_len;

	char txt[BUF_SIZE];
	int k;
	struct Index_Voisin * iv;
	struct Voisin * v ;
	struct Event * e;
	struct List * l;
	struct List * aux;

	int i = 0;
	
	//uint64_t source_id = 0;
	while(i < body_len) {

		memset(txt, 0, BUF_SIZE);

		tlv = getMsg_Tlv(event->msg, i);

		type = getTlv_Type(tlv);

		tlv_len = getTlv_Length(tlv);

		switch(type) {

			case 0 : //Pad1
				if(VUE) {printf("System Send :\n\tPad1\n");}
				break;

			case 1 : //PadN
				if(VUE) {printf("System Send :\n\tPadN\n");}
				break;

			case 2 : //Hello
				if(tlv_len == 8 || tlv_len == 16) {//Verifie
					return 0;
					if(tlv_len == 16) {
						/*if(base->id != getHello_long_Destination_Id(tlv)) {
							return 2;
						}

						iv = malloc(sizeof(struct Index_Voisin));
						memset(iv, 0, sizeof(struct Index_Voisin));
						iv->ip = from.sin6_addr;
						iv->port = from.sin6_port;
						for(
							l = base->list_voisin;
							l != NULL && memcmp(((struct Voisin *)l->objet)->index, iv, sizeof(struct Index_Voisin)) != 0;
							l = l->suite
						);
						if(l != NULL) {//Déjà Voisin
							free(iv);
							v  = (struct Voisin *)l->objet;
							v->date = now;
							v->date_long = now;
						
						} else {//Nouveau Voisin

							l = base->list_voisin_potentiel;

							if(l != NULL) {//On le supprime de la List de Voisin Potentiel
								if(memcmp((struct Index_Voisin *)l->objet, iv, sizeof(struct Index_Voisin)) == 0) {
									base->list_voisin_potentiel = l->suite;
									free(l->objet);
									free(l);
								} else {
									while(l->suite != NULL && memcmp((struct Index_Voisin *)(l->suite)->objet, iv, sizeof(struct Index_Voisin)) != 0) {
										l = l->suite;
									}
									if(l->suite != NULL) {
										aux = l->suite;
										l->suite = aux->suite;
										free(aux->objet);
										free(aux);
									}
								}
							}

							v = malloc(sizeof(struct Voisin));
							memset(v, 0, sizeof(struct Voisin));
							v->index = iv;
							v->id = getHello_Source_Id(tlv);
							v->date = now;
						v->date_long = now;

						base->list_voisin = add_List(v, base->list_voisin);

						e = malloc(sizeof(struct Event));
						memset(e, 0, sizeof(struct Event));

						gettimeofday(&(e->tv), NULL);
						e->dest = iv;
						e->msg = malloc(BUF_SIZE);
						createMsg(e->msg);
	
						i = createHello_long(txt, base->id, v->id);

						e->msg_len = setMsg_Body(e->msg, txt, i);

						base->list_event = add_List_Event(e, base->list_event);*/
					}
				} else {
					return 2;//Ou Break ??
				}
				break;

			case 3 : //Neighbour
				/*iv = malloc(sizeof(struct Index_Voisin));
				memset(iv, 0, sizeof(struct Index_Voisin));
				getNeighbour_Ip(tlv, &(iv->ip));
				iv->port = getNeighbour_Port(tlv);
				for(
					l = base->list_voisin_potentiel;
					l != NULL && memcmp((struct Index_Voisin *)l->objet, iv, sizeof(struct Index_Voisin)) != 0;
					l = l->suite
				);
				if(l != NULL) {
					free(iv);
				} else {
					base->list_voisin_potentiel = add_List(iv, base->list_voisin_potentiel);
				}*/
				break;

			case 4 : //Data
				break;

			case 5 : //Ack : 
				break;

			case 6 : 
				if(VUE) {
					memcpy(txt, getGoAway_Message(tlv), getGoAway_Message_Taille(tlv));
					printf("System Send :\n\tGoAway : Code %"PRIu8", Message : %s\n", getGoAway_Code(tlv), txt);
				}
				break;

			case 7 : 
				if(VUE) {
					memcpy(txt, getWarning_Message(tlv), getWarning_Message_Taille(tlv));
					printf("System Send :\n\tWarning : %s\n", txt);
				}
				break;

			default : 
				if(VUE) {printf("System Send :\n\tTlv Inconnu\n");}

		}

		i += (tlv_len+TLV_ENTETE);

	}

	return 0;
}

int start(int sock) {

	/* Initialisation */

	int rc;
	struct timeval tv;
	struct timeval * mtv(struct timeval * tv) {if(tv->tv_sec < 0) {tv->tv_sec = 0;} return tv;}
	fd_set readfds;

	srand (time (NULL));

	uint64_t id =
	(((uint64_t) rand() <<  0) & 0x000000000000FFFFull) | 
	(((uint64_t) rand() << 16) & 0x00000000FFFF0000ull) | 
	(((uint64_t) rand() << 32) & 0x0000FFFF00000000ull) |
	(((uint64_t) rand() << 48) & 0xFFFF000000000000ull);

	if(VUE) {printf("System :\n\tId = %" PRIu64 "\n", id);}

	struct sockaddr_in6 dest_addr;

	struct Base base;

	struct Event * event_current = NULL;
	struct Event * event_next = NULL;

	restart:

	rc = setBase(&base, id);
	if(rc < 0) {return -1;}

//(struct Event *)(base.list_event)->objet;

	/* Boucle */

	suivant:

	printf("SUIVANT !\n");

	if(base.list_event == NULL) {
		goto restart;
	}

	if(event_next == NULL) {
		event_next = (struct Event *)(base.list_event)->objet;
	}

	event_current = event_next;

	base.list_event = base.list_event->suite;

	switch(traitement_send(event_current, &base)) {//2 = msg Ignoré
		case 1 : goto suivant;//Cas normalement Impossible
		case -1 : return -1;
		default : break;
	}

	renvoie://!!!Renvoie == Remodifie ? TODO

	printf("RENVOIE !\n");

	if(event_current->dest != NULL) {

		get_voisin_addr(event_current->dest, &dest_addr);

		rc = sendto(sock, event_current->msg, event_current->msg_len, 0, (struct sockaddr *)&dest_addr, (socklen_t)sizeof(struct sockaddr_in6));
		//Pas tout de suite -> Si !! -> faire plus -> peut-être dans le timeout !!TODO

	}

	encore:

	printf("ENCORE !\n");

	if(base.list_event == NULL) {
		event_next = NULL;
	} else {
		event_next = (struct Event *)(base.list_event)->objet;
	}//printf("GOT IT !!%d\n", base.list_event == NULL);

	if(event_next == NULL) {
		if(DEBUG) {printf("event_next == NULL\n");}
		tv.tv_sec = 10;
		tv.tv_usec = 0;
	} else {
		gettimeofday(&tv, NULL);
		tv.tv_sec = (event_next->tv).tv_sec - tv.tv_sec;
		tv.tv_sec = (event_next->tv).tv_usec - tv.tv_usec;
		if(DEBUG) {printf("TIME : sec : %ld, usec : %ld\n", tv.tv_sec, tv.tv_usec);}
	}

	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);

	rc = select(sock + 1, &readfds, NULL, NULL, mtv(&tv));

	if(rc < 0) {perror("select");return -1;}

	if(rc > 0) {
		if(FD_ISSET(sock, &readfds)) {
			switch(traitement_recv(sock, &base)) {//Voir pour envoyer warning ... 2 = msg Ignoré
				case 1 : goto renvoie;
				case -1 : return -1;
				default : goto encore;
			}
			//////////////////////////////////////////////////////////////////////////////TODO
		} else {perror("Descripteur de fichier inattendu");return -1;}
	} else {/* timeout */
		//if(DEBUG) {printf("Encore !\n");}
		goto suivant;
	}	

	//clear_Event(get_Voisin);//TODO ?

	clear_Base(&base);

	return 0;

}

/* Main */

int main() {

	int sock = set_addr_pair();

	if(sock < 0) {exit(EXIT_FAILURE);}

	int res = start(sock);

	close(sock);

	if(res != 0) {exit(EXIT_FAILURE);}

	return 0;
}

/* Fonctions Auxiliaires */

int tvcmp(struct timeval * tv1, struct timeval * tv2) {
	return tv2->tv_sec - tv1->tv_sec;
}

int setEventTime(struct Event * e, int sec, int usec) {
	gettimeofday(&(e->tv), NULL);
	(e->tv).tv_sec += sec;
	(e->tv).tv_usec += usec;
	return 0;
}

struct List * add_List_Event(struct Event * e, struct List * l) {
	if(l != NULL && tvcmp(&(e->tv), &(EVENT(l)->tv)) < 0) {
		l->suite = add_List_Event(e, l->suite);
		return l;
	} else {
		return add_List(e, l);
	}
}

int clear_Event(struct Event * e) {
	//clear_List(e->dest, (f_obj)&clear_Index_Voisin);
	//free(e->msg);// Oui ou Non ?!
	free(e);
	return 0;
}

int clear_Base(struct Base * base) {
	clear_List(base->list_voisin_potentiel, (f_obj)&clear_Index_Voisin);
	clear_List(base->list_voisin, (f_obj)&clear_Voisin);

	clear_List(base->list_event, (f_obj)&clear_Event);

	clear_List(base->list_data, (f_obj)&clear_Donnee);
	free(base);
	return 0;
}


