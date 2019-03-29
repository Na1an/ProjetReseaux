#include "multicast.h"/**//*//**/

int get_socket_and_server_addr(int *sock, struct sockaddr_in6 *addr) { // n'égale pas 0, ça pose du problème

	int err = -1;

	struct addrinfo hints, *r, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_V4MAPPED | AI_ALL;
	hints.ai_protocol = 0;

	if ((getaddrinfo(ADDR, PORT, &hints, &r)) != 0 || NULL == r) {
		perror("getaddrinfo");
		return -1;
	}

	/* Init sock */
	for (
		p = r;
		((NULL != p) && ((*sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0));
		p = p->ai_next
	);
	if (NULL == p) {
		perror("create socket");
		return -2;
	}

	if(DEBUG) {
		printf("sock : %d\n", *sock);
	}

	/* Init addr */
	*addr = *((struct sockaddr_in6*) p->ai_addr);

	freeaddrinfo(r);

	/* Set Option sock */

	int val_1 = 1;
	
	err = setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &val_1, sizeof(val_1));
	if(err < 0) {
		perror("setsockopt - SOL_REUSEADDR");
		return -3;
	}
	
	/*err = setsockopt(*sock, IPPROTO_IPV6, IPV6_V6ONLY, &val_1, sizeof(val_1));
	if(err < 0) {
		perror("setsockopt - IPV6_V6ONLY");
		return -3;
	}*/

	return 0;
}

// uint64_t lg = ;

int main() {

	int sock, rc;

	struct sockaddr_in6 local_addr;
	socklen_t addr_len = sizeof(local_addr);

	char sendMsg[BUF_SIZE], recvMsg[BUF_SIZE];
	memset(sendMsg, 0, BUF_SIZE);
	memset(recvMsg, 0, BUF_SIZE);

	switch (get_socket_and_server_addr(&sock, &local_addr)) {
		case -1:
			fprintf(stderr, "Error: Find host.\n");
			exit(EXIT_FAILURE);
		case -2:
			fprintf(stderr, "Error: Creat sock.\n");
			exit(EXIT_FAILURE);
		case -3:
			fprintf(stderr, "Error: Set Option.\n");
			exit(EXIT_FAILURE);
	}
	
	uint64_t id =
	(((uint64_t) rand() <<  0) & 0x000000000000FFFFull) | 
	(((uint64_t) rand() << 16) & 0x00000000FFFF0000ull) | 
	(((uint64_t) rand() << 32) & 0x0000FFFF00000000ull) |
	(((uint64_t) rand() << 48) & 0xFFFF000000000000ull);

	if(DEBUG) {
		printf("Id = %" PRIu64 "\n", id);
	}

	/*Pad1:
	Pad1 p1;
	int p1_len = sizeof(p1);
	memset(&p1,0,p1_len);
	p1.Type = 0;
	/**/
	
	/*PadN:
	PadN pn;
	int pn_len = sizeof(pn);
	memset(&pn,0,pn_len);
	pn.Type = 1;
	/**/
	
	//*Hello_short:
	Hello_short hello_s;
	int hello_s_len = sizeof(hello_s);
	memset(&hello_s,0,hello_s_len);//Taille???
	hello_s.Type = 2;
	hello_s.Length = 8;
	hello_s.Source_Id= id;
	/**/
	
	/*Hello_long:
	Hello_long hello_l;
	int hello_l_len = sizeof(hello_l);
	memset(&hello_l,0,hello_l_len);
	hello_l.Type = 2;
	hello_s.Length = 16;
	hello_s.Source_Id= id;
	/**/

	Message buf;
	int buf_len = sizeof(buf);//Taille???
	memset(&buf,0,buf_len);//Taille???

	Message msg;
	int msg_len = sizeof(msg);
	memset(&msg,0,msg_len);//Taille???
	msg.Magic = 93;
	msg.Version = 2;

	msg.Body_Length = hello_s_len;//Taille???
	memcpy(&msg.Tlv,&hello_s,sizeof(hello_s));//Taille???

	memcpy(&sendMsg, &msg, BUF_SIZE);//Taille???

	/*YUCHEN TEST/

	Hello_short hello2;
	memcpy(&hello2, &msg.Tlv, sizeof(msg.Tlv));

	printf("Type: %" PRIu8 "\n", hello2.Type);
	printf("Lenght: %" PRIu8 "\n", hello2.Length);
	printf("Source_id : %" PRIu64 "\n", hello2.Source_Id);

	/*FIN YUCHEN TEST*/

	/*SEB TEST/
	Message mseb;
	memset(&mseb, 0, BUF_SIZE);
	
	//printf("%d\n", memcpy(&mseb, &sendMsg, BUF_SIZE));

	printf("%"PRIu8"\n",mseb.Magic);
	printf("%"PRIu8"\n",mseb.Version);
	printf("%"PRIu8"\n",mseb.Body_Length);
	if(mseb.Tlv.Type == 2) {
		if(mseb.Tlv.Length == 8) {
			Hello_short hsseb;
			memset(&hsseb, 0, sizeof(hsseb));
			memcpy(&hsseb, &mseb.Tlv, sizeof(hsseb));
			printf("%"PRIu64"\n", hsseb.Source_Id);
		} else {
			printf("Hello Long ? : %"PRIu8"\n", mseb.Tlv.Length);
		}
	} else {
		printf("Pas Hello\n");
	}

	/*FIN SEB TEST*/


	struct timeval tv;
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);

	if(DEBUG) {
		printf("SendMsg : %s\n",sendMsg);
	}

	again:

	rc = sendto(sock, &sendMsg, BUF_SIZE, 0, (struct sockaddr *)&local_addr, (socklen_t)addr_len);//Taille???
	
	if(rc < 0) {
		perror("sendto");
		exit(EXIT_FAILURE);
	}

	if(DEBUG) {
		printf("Message Envoyé !\n");
	}

	again2:

	tv.tv_sec  = 10;
	tv.tv_usec  = 0;

	rc = select(sock + 1, &readfds, NULL, NULL, &tv);

	if(rc < 0) {
		perror("select");
		exit(EXIT_FAILURE);
	}

	if(rc > 0) {
		if(FD_ISSET(sock, &readfds)) {
			rc = recvfrom(sock, recvMsg, BUF_SIZE, 0, NULL, NULL);
			if(rc < 0) {
				if(errno == EAGAIN) {
					goto again;
				} else {
					perror("recvfrom");
					exit(EXIT_FAILURE);
				}
			}
			goto ok;
		} else {
			perror("Descripteur de fichier inattendu");
			exit(EXIT_FAILURE);
		}
	} else {/* timeout */
		if(DEBUG) {
			printf("Encore !\n");
		}
		goto again;
	}
  
	ok:

	memcpy(&buf,recvMsg,BUF_SIZE);

	if(buf.Magic != 93 || buf.Version != 2) {
		if(DEBUG) {
			printf("Message Rejeté");
		}
	}

	if(DEBUG) {
		printf("MESSAGE RECU, IL FAUT LE TRAITER MAINTENANT!");
	}

	memset(&buf,0,BUF_SIZE);

	goto ok;


	return 0;
}


