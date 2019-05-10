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
	e->dest = malloc(sizeof(struct Index_Voisin));
	memset(e->dest, 0, sizeof(struct Index_Voisin));
	memcpy(e->dest, iv, sizeof(struct Index_Voisin));
	e->tlv = malloc(BUF_SIZE);
	e->tlv_len = createHello_short(e->tlv, id);

	return e;
}

int setBase(struct Base * b, uint64_t id, struct Index_Voisin * piv) {

	struct Index_Voisin * iv = malloc(sizeof(struct Index_Voisin));
	memset(iv, 0, sizeof(struct Index_Voisin));
	memcpy(iv, piv, sizeof(struct Index_Voisin));

	memset(b, 0, sizeof(struct Base));

	b->id = id;

	b->list_voisin_potentiel = add_List(iv, NULL);
	if(b->list_voisin_potentiel == NULL) {return -1;}

	b->list_voisin = NULL;

	b->list_data = create_Circ_List(32);

	b->list_event = add_List(premier_Event(iv, id), NULL);

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
	//int k;
	struct Index_Voisin * iv;
	struct Voisin * v ;
	struct Index_Donnee * id;
	struct Event * e;
	struct List * l;
	struct List * aux;
	struct timeval tv;

	int i = 0;

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
if(DEBUG) {printf("HELLO\n");}
				if(tlv_len == 8 || (tlv_len == 16 && base->id == getHello_long_Destination_Id(tlv))) {
					for(
						l = base->list_voisin;
						l != NULL && memcmp(VOISIN(l)->index, iv_actuel, sizeof(struct Index_Voisin)) != 0;
						l = l->suite
					);
					if(l != NULL) {//Déjà Voisin
if(DEBUG) {printf("DEJA VOISIN\n");}
						v = VOISIN(l);
						v->date = now;
						if(tlv_len == 16) {v->date_long = now;}//Long
						
					} else {//Nouveau Voisin
if(DEBUG) {printf("NEW VOISIN\n");}
						rmVoisin(iv_actuel, base);

						//On l'ajoute au Voisin

						v = malloc(sizeof(struct Voisin));
						memset(v, 0, sizeof(struct Voisin));
						v->index = malloc(sizeof(struct Index_Voisin));
						memcpy(v->index, iv_actuel, sizeof(struct Index_Voisin));
						v->id = getHello_Source_Id(tlv);
						v->date = now;
						if(tlv_len == 16) {v->date_long = now;}//Long
						base->list_voisin = add_List(v, base->list_voisin);

						//On lui envoie des Hello_long

						e = malloc(sizeof(struct Event));
						memset(e, 0, sizeof(struct Event));
						setEventTime(e, 0);
						e->dest = malloc(sizeof(struct Index_Voisin));
						memset(e->dest, 0, sizeof(struct Index_Voisin));
						memcpy(e->dest, v->index, sizeof(struct Index_Voisin));
						e->tlv = malloc(BUF_SIZE);
						e->tlv_len = createHello_long(e->tlv, base->id, v->id);
						base->list_event = add_List_Event(e, base->list_event);
					}
				}
				break;

			case 3 : //Neighbour
if(DEBUG) {printf("NEIGHTBOUR\n");}
				iv = malloc(sizeof(struct Index_Voisin));
				memset(iv, 0, sizeof(struct Index_Voisin));
				getNeighbour_Ip(tlv, &(iv->ip));
				iv->port = getNeighbour_Port(tlv);
if(DEBUG) {printf("NEIGHTBOUR1\n");}
				for(
					l = base->list_voisin_potentiel;
					l != NULL && memcmp(I_VOISIN(l), iv, sizeof(struct Index_Voisin)) != 0;
					l = l->suite
				);
if(DEBUG) {printf("NEIGHTBOUR2\n");}
				for(
					aux = base->list_voisin;
					aux != NULL && memcmp(VOISIN(aux)->index, iv, sizeof(struct Index_Voisin)) != 0;
					aux = aux->suite
				);
if(DEBUG) {printf("NEIGHTBOUR3\n");}
				if(l != NULL || aux != NULL) {//Déjà Connu(e)
					free(iv);
				} else {//On l ajoute au Voisin Potentiel
					base->list_voisin_potentiel = add_List(iv, base->list_voisin_potentiel);
				}
if(DEBUG) {printf("NEIGHTBOUR4\n");}

				break;

			case 4 : //Data
if(DEBUG) {printf("DATA\n");}
				id = malloc(sizeof(struct Index_Donnee));
				memset(id, 0, sizeof(struct Index_Donnee));
				id->id = getData_Sender_Id(tlv);
				id->nonce = getData_Nonce(tlv);

if(DEBUG) {printf("DATA1\n");}
				for(
					l = base->list_data;
					l->suite != base->list_data && (I_DONNEE(l) == NULL || memcmp(I_DONNEE(l), id, sizeof(struct Index_Donnee))) != 0;
					l = l->suite
				) ;
if(DEBUG) {printf("DATA2\n");}

				if(l->suite == base->list_data &&
				(I_DONNEE(l) == NULL || memcmp(I_DONNEE(l), id, sizeof(struct Index_Donnee)) != 0)) {
if(getData_Type(tlv) == 0) {memcpy(txt, getData_Donnees(tlv), getData_Donnees_Taille(tlv));printf("%s\n", txt);}
					if(I_DONNEE(l) != NULL) {free(I_DONNEE(l));}
					l->objet = id;

					gettimeofday(&tv, NULL);
					for(l = base->list_voisin; l != NULL; l = l->suite) {
						if(tvcmp(&VOISIN(l)->date_long, &tv) < 30) {
							e = malloc(sizeof(struct Event));
							memset(e, 0, sizeof(struct Event));
							e->opt = 1;
							setEventTime(e, rand()%2);
							e->dest = malloc(sizeof(struct Index_Voisin));
							memset(e->dest, 0, sizeof(struct Index_Voisin));
							memcpy(e->dest, VOISIN(l)->index, sizeof(struct Index_Voisin));
							e->tlv = malloc(BUF_SIZE);
							memcpy(e->tlv, tlv, tlv_len);
							e->tlv_len = tlv_len;
							base->list_event = add_List_Event(e, base->list_event);
if(DEBUG) {printf("DATA3.5\n");}
						}
					}
if(DEBUG) {printf("DATA3\n");}
				}

				//On acquitte l envoyeur

				e = malloc(sizeof(struct Event));
				memset(e, 0, sizeof(struct Event));
				setEventTime(e, 0);
				e->dest = malloc(sizeof(struct Index_Voisin));
				memset(e->dest, 0, sizeof(struct Index_Voisin));
				memcpy(e->dest, iv_actuel, sizeof(struct Index_Voisin));
				e->tlv = malloc(BUF_SIZE);
				e->tlv_len = createAck(e->tlv, id->id, id->nonce);
				base->list_event = add_List_Event(e, base->list_event);

				type = 5;//On n envoie pas a l envoyeur
if(DEBUG) {printf("DATA4\n");}

			case 5 : //Ack :
if(DEBUG) {printf("ACK\n");}
				if(base->list_event != NULL) {
					for(l = base->list_event; l->suite != NULL; l = l->suite) {
if(DEBUG) {printf("ACK2\n");}
						e = EVENT(l->suite);
						if(e != NULL &&
						memcmp(e->dest, iv_actuel, sizeof(struct Index_Voisin)) == 0 &&
						getTlv_Type(e->tlv) == 4 &&
						getData_Sender_Id(e->tlv) == getData_Sender_Id(tlv) &&
						getData_Nonce(e->tlv) == getData_Nonce(tlv)) {
if(DEBUG) {printf("ACK3\n");}
							aux = l->suite;
							l->suite = aux->suite;
							clear_Event(EVENT(aux));
							free(aux);
						}
					}

					e = EVENT(base->list_event);
					if(e != NULL &&
					memcmp(e->dest, iv_actuel, sizeof(struct Index_Voisin)) == 0 &&
					getTlv_Type(e->tlv) == 4 &&
					getData_Sender_Id(e->tlv) == getData_Sender_Id(tlv) &&
					getData_Nonce(e->tlv) == getData_Nonce(tlv)) {
						l = base->list_event;
						base->list_event = l->suite;
						clear_Event(EVENT(l));
						free(l);
					}
				}
						
if(DEBUG) {printf("ACK1\n");}
				break;

			case 6 : 
				if(VUE) {
					memcpy(txt, getGoAway_Message(tlv), getGoAway_Message_Taille(tlv));
if(VUE) {printf("System Recv :\n\tGoAway : Code %"PRIu8", Message : %s\n", getGoAway_Code(tlv), txt);}
				}
				break;

			case 7 : 
				if(VUE) {
					memcpy(txt, getWarning_Message(tlv), getWarning_Message_Taille(tlv));
if(VUE) {printf("System Recv :\n\tWarning : %s\n", txt);}
				}
				break;

			default : 
if(VUE) {printf("System Recv :\n\tTlv Inconnu : type = %"PRIu8"\n", type);}

		}

		if(type != 0) {
			i += (tlv_len+TLV_ENTETE);
		} else {
			i++;
		}

	}

	clear_Index_Voisin(iv_actuel);

	return 0;
}

int traitement_send(struct Event * event, struct Base * base, char * msg) {

	if(event == NULL) {return 0;}

	//int rc;// Tout les 12 passages (Dans hellolong ? Non)Voir pour Neightbour => Compte Nb Voisin et voir si Peut avoir autre (Sachant helloshort=>enlever

	if(event->dest == NULL) {
		return -1;//Faire les neigthbours ou les 8 voisins
	}

	memset(msg, 0, BUF_SIZE);

	struct timeval now;

	gettimeofday(&now, NULL);

	uint8_t type;
	uint8_t tlv_len;

	char txt[BUF_SIZE];
	int k;
	//struct Index_Voisin * iv;
	struct Voisin * v ;
	struct Event * e;
	//struct List * l;
	//struct List * aux;
	struct timeval tv;
	
	memset(txt, 0, BUF_SIZE);

	type = getTlv_Type(event->tlv);

	tlv_len = getTlv_Length(event->tlv);

	createMsg(msg);
	int msg_len = setMsg_Body(msg, event->tlv, event->tlv_len);

	switch(type) {

		case 0 : //Pad1
if(VUE) {printf("System Send :\n\tPad1\n");}
			break;

		case 1 : //PadN
if(VUE) {printf("System Send :\n\tPadN\n");}
			break;

		case 2 : //Hello
if(DEBUG) {printf("HELLO\n");}
			if(tlv_len == 8) {//Short
				rmVoisin(event->dest, base);
if(DEBUG) {printf("HELLO1\n");}
			}
			if(tlv_len == 16) {//Long
if(DEBUG) {printf("HELLO2 \n");}
				v = find_Voisin(event->dest, base->list_voisin);
				if(v == NULL) {return -1;}
				gettimeofday(&tv, NULL);
				if(tvcmp(&v->date, &tv) < 30) {//Encore
if(DEBUG) {printf("HELLO3\n");}
					e = malloc(sizeof(struct Event));
					memset(e, 0, sizeof(struct Event));
					setEventTime(e, 30);
					e->dest = malloc(sizeof(struct Index_Voisin));
					memset(e->dest, 0, sizeof(struct Index_Voisin));
					memcpy(e->dest, event->dest, sizeof(struct Index_Voisin));
					e->tlv = malloc(BUF_SIZE);
					memcpy(e->tlv, event->tlv, event->tlv_len);
					e->tlv_len = event->tlv_len;
					base->list_event = add_List_Event(e, base->list_event);
				} else {//Mort
if(DEBUG) {printf("HELLO4\n");}
					createMsg(msg);
					k = createGoAway(txt, 2, NULL, 0);
if(VUE) {printf("System Send :\n\tGoAway By Death in 'Hello'\n");}
					msg_len = setMsg_Body(msg, txt, k);
					rmVoisin(event->dest, base);//TODO PROBLEME S EFFACE LUI MEME
				}
			}
if(DEBUG) {printf("HELLO5\n");}
			break;

		case 3 : //Neighbour//TODO
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
			if(event->opt < 5) {//Encore
				e = malloc(sizeof(struct Event));
				memset(e, 0, sizeof(struct Event));
				e->opt = event->opt + 1;
				setEventTime(e, (2^(event->opt-1)) + rand()%(2^(event->opt-1)));
				e->dest = malloc(sizeof(struct Index_Voisin));
				memset(e->dest, 0, sizeof(struct Index_Voisin));
				memcpy(e->dest, event->dest, sizeof(struct Index_Voisin));
				e->tlv = malloc(BUF_SIZE);
				memcpy(e->tlv, event->tlv, event->tlv_len);
				e->tlv_len = event->tlv_len;
				base->list_event = add_List_Event(e, base->list_event);
			} else {//Mort
				createMsg(msg);
				k = createGoAway(txt, 2, NULL, 0);
if(VUE) {printf("System Send :\n\tGoAway By Death in 'Data'\n");}
				msg_len = setMsg_Body(msg, txt, k);
				rmVoisin(event->dest, base);//TODO PROBLEME S EFFACE LUI MEME
			}
			break;

		case 5 : //Ack : 
			/* Rien : Tout est fait dans traitement_recv */
			break;

		case 6 : //GoAway :
			if(VUE) {
				memcpy(txt, getGoAway_Message(event->tlv), getGoAway_Message_Taille(event->tlv));
if(VUE) {printf("System Send :\n\tGoAway : Code %"PRIu8", Message : %s\n", getGoAway_Code(event->tlv), txt);}
			}
			break;

		case 7 : //Warning :
			if(VUE) {
				memcpy(txt, getWarning_Message(event->tlv), getWarning_Message_Taille(event->tlv));
if(VUE) {printf("System Send :\n\tWarning : %s\n", txt);}
			}
			break;

		default : 
if(VUE) {printf("System Send :\n\tTlv Inconnu\n");}
	}

	return msg_len;
}

int start(int sock) {

	/* Initialisation */

	int rc, k;
	struct timeval tv;
	struct timeval * mtv(struct timeval * tv) {if(tv->tv_sec < 0) {tv->tv_sec = 0;} if(tv->tv_usec < 0) {tv->tv_usec = 0;} return tv;}
	fd_set readfds;

	char chat[BUF_SIZE];
	char data[BUF_SIZE];
	struct List * l;
	struct Index_Donnee * ixd;

	char * msg = malloc(BUF_SIZE);
	int msg_len;

	srand (time (NULL));

	uint64_t id =
	(((uint64_t) rand() <<  0) & 0x000000000000FFFFull) | 
	(((uint64_t) rand() << 16) & 0x00000000FFFF0000ull) | 
	(((uint64_t) rand() << 32) & 0x0000FFFF00000000ull) |
	(((uint64_t) rand() << 48) & 0xFFFF000000000000ull);

if(VUE) {printf("System :\n\tId = %" PRIu64 "\n", id);}

	uint32_t nonce;

	struct sockaddr_in6 dest_addr;

	struct Index_Voisin * piv = premier_Voisin();

	struct Base base;

	struct Event * event_current = NULL;
	struct Event * event_next = NULL;
	struct Event * e;

	restart: 
if(SHOWPATH) {printf("RESTART !\n");}

	rc = setBase(&base, id, piv);
	if(rc < 0) {return -1;}

	/* Boucle */

	suivant: 
if(SHOWPATH) {printf("SUIVANT !\n");}

	if(base.list_event == NULL) {
		clear_Base(&base);
		goto restart;
	}

	if(event_next == NULL) {
		event_next = EVENT(base.list_event);
	}

	if(event_current != NULL) {clear_Event(event_current);}

	event_current = event_next;//Non NULL

	l = base.list_event;
	base.list_event = base.list_event->suite;
	free(l);

if(DEBUG) {printf("Debut traitement send\n");}

	msg_len = traitement_send(event_current, &base, msg);
if(DEBUG) {printf("Fin traitement send : %d\n", msg_len);}

	renvoie: 
if(SHOWPATH) {printf("RENVOIE !\n");}

	if(event_current->dest != NULL && msg_len != -1) {

if(DEBUG) {printf("Message Envoyé !\n");printMsg(msg);}

		get_voisin_addr(event_current->dest, &dest_addr);

		rc = sendto(sock, msg, msg_len, 0, (struct sockaddr *)&dest_addr, (socklen_t)sizeof(struct sockaddr_in6));

	}

	encore: if(SHOWPATH) {printf("ENCORE !\n");}

	if(base.list_event == NULL) {
		event_next = NULL;
	} else {
		event_next = EVENT(base.list_event);
	}

	if(event_next == NULL) {
if(DEBUG) {printf("event_next == NULL\n");}
		tv.tv_sec = 2;
		tv.tv_usec = 0;
	} else {
		gettimeofday(&tv, NULL);
		tv.tv_sec = (event_next->tv).tv_sec - tv.tv_sec;
		tv.tv_usec = (event_next->tv).tv_usec - tv.tv_usec;

if(DEBUG) {printf("TIME : sec : %ld, usec : %ld\n", tv.tv_sec, tv.tv_usec);}
	}

	FD_ZERO(&readfds);
	FD_SET(0, &readfds);
	FD_SET(sock, &readfds);

	rc = select(sock + 1, &readfds, NULL, NULL, mtv(&tv));

	if(rc < 0) {perror("select");return -1;}

	if(rc > 0) {
		if(FD_ISSET(sock, &readfds)) {
if(DEBUG) {printf("traitement recv\n");}
			switch(traitement_recv(sock, &base)) {
				case 1 : goto renvoie;
				case -1 : return -1;
				default : goto encore;
			}
		} else if(FD_ISSET(0, &readfds)) {
			memset(chat, 0, BUF_SIZE);
			memset(data, 0, BUF_SIZE);
			k = snprintf(chat, BUF_SIZE, "YucheB : ");
			rc = read(0, chat+k, BUF_SIZE-k);
			if(rc < 0) {
				printf("System Erreur :\n\tLe message n'a pas été envoyé\n");
				goto encore;
			} else if(memcmp("EXIT", chat+k, 4) != 0) {

				nonce = (((uint32_t) rand() <<  0) & 0x0000FFFFull) | (((uint32_t) rand() << 16) & 0xFFFF0000ull);
				rc = createData(data, base.id, nonce, 0, chat, rc+k);
				if(rc < 0) {
					printf("System Erreur :\n\tLe message est trop long\n");
					goto encore;
				}
				gettimeofday(&tv, NULL);
				for(l = base.list_voisin; l != NULL; l = l->suite) {
					if(tvcmp(&VOISIN(l)->date_long, &tv) < 30) {
						e = malloc(sizeof(struct Event));
						memset(e, 0, sizeof(struct Event));
						e->opt = 1;
						setEventTime(e, rand()%2);
						e->dest = malloc(sizeof(struct Index_Voisin));
						memset(e->dest, 0, sizeof(struct Index_Voisin));
						memcpy(e->dest, VOISIN(l)->index, sizeof(struct Index_Voisin));
						e->tlv = malloc(BUF_SIZE);
						memcpy(e->tlv, data, rc);
						e->tlv_len = rc;
						base.list_event = add_List_Event(e, base.list_event);
					}
				}

				ixd = malloc(sizeof(struct Index_Donnee));
				memset(ixd, 0, sizeof(struct Index_Donnee));
				ixd->id = base.id;
				ixd->nonce = nonce;
				for(
					l = base.list_data;
					l->suite != base.list_data;
					l = l->suite
				);
				if(I_DONNEE(l) != NULL) {free(I_DONNEE(l));}
				l->objet = ixd;

				goto encore;
			}
		} else {perror("Descripteur de fichier inattendu");return -1;}
	} else {/* timeout */
		goto suivant;
	}
	fin:
if(SHOWPATH) {printf("FIN !\n");}

	if(event_current != NULL) {clear_Event(event_current);}

	clear_Index_Voisin(piv);

	free(msg);

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

int clear_Base(struct Base * base) {
	clear_List(base->list_voisin_potentiel, (f_obj)&clear_Index_Voisin);

	clear_List(base->list_voisin, (f_obj)&clear_Voisin);

	clear_List(base->list_event, (f_obj)&clear_Event);

	clear_Circ_List(base->list_data, (f_obj)&clear_Index_Donnee);

	return 0;
}

int rmVoisin(struct Index_Voisin * iv, struct Base * b) {
	struct List * l;
	struct List * aux;
	if(b->list_voisin_potentiel != NULL) {
		for(l = b->list_voisin_potentiel; l->suite != NULL; l = l->suite) {
			if(I_VOISIN(l->suite) != NULL && memcmp(I_VOISIN(l->suite), iv, sizeof(struct Index_Voisin)) == 0) {
				aux = l->suite;
				l->suite = aux->suite;
				clear_Index_Donnee(I_DONNEE(aux));
				free(aux);
			}
		}
		if(I_VOISIN(b->list_voisin_potentiel) != NULL && memcmp(I_VOISIN(b->list_voisin_potentiel), iv, sizeof(struct Index_Voisin)) == 0) {
			l = b->list_voisin_potentiel;
			b->list_voisin_potentiel = l->suite;
			clear_Index_Donnee(I_DONNEE(l));
			free(l);
		}
	}
	if(b->list_voisin != NULL) {
		for(l = b->list_voisin; l->suite != NULL; l = l->suite) {
			if(VOISIN(l->suite) != NULL && VOISIN(l->suite)->index != NULL && memcmp(VOISIN(l->suite)->index, iv, sizeof(struct Index_Voisin)) == 0) {
				aux = l->suite;
				l->suite = aux->suite;
				clear_Voisin(VOISIN(aux));
				free(aux);
			}
		}
		if(VOISIN(b->list_voisin) != NULL && VOISIN(b->list_voisin)->index != NULL && memcmp(VOISIN(b->list_voisin)->index, iv, sizeof(struct Index_Voisin)) == 0) {
			l = b->list_voisin;
			b->list_voisin = l->suite;
			clear_Voisin(VOISIN(l));
			free(l);
		}
	}
	if(b->list_event != NULL) {
		for(l = b->list_event; l->suite != NULL; l = l->suite) {
			if(EVENT(l->suite) != NULL && EVENT(l->suite)->dest != NULL && memcmp(EVENT(l->suite)->dest, iv, sizeof(struct Index_Voisin)) == 0) {
				aux = l->suite;
				l->suite = aux->suite;
				clear_Event(EVENT(aux));
				free(aux);
			}
		}
		if(EVENT(b->list_event) != NULL && EVENT(b->list_event)->dest != NULL && memcmp(EVENT(b->list_event)->dest, iv, sizeof(struct Index_Voisin)) == 0) {
			l = b->list_event;
			b->list_event = l->suite;
			clear_Event(EVENT(l));
			free(l);
		}
	}
	return 0;
}



