#include "multicast.h"

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

	printf("sock : %d\n", *sock);

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
	socklen_t client_len = sizeof(local_addr);

	char sendMsg[BUF_SIZE], recvMsg[BUF_SIZE];
	memset(sendMsg, 0, BUF_SIZE);
	memset(recvMsg, 0, BUF_SIZE);

	int recv_len = snprintf(recvMsg, BUF_SIZE, "Je suis un Ninja!");//A ENLEVER
	
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

	//Pad1 p1;
	//PadN pn;

	Hello_short hello_s;
	memset(&hello_s,0,sizeof(hello_s));

	hello_s.Type = 2;
	hello_s.Length = 8;
	hello_s.Source_Id= id;

	Message msg;
	memset(&msg,0,sizeof(Message));

	msg.Magic = 93;
	msg.Version = 2;
	//msg.Body_Length = hello_s_len;
	memcpy(&msg.Tlv,&hello_s,sizeof(hello_s));

	Hello_short hello2;
	memcpy(&hello2, &msg.Tlv, sizeof(msg.Tlv));


	printf("Type: %" PRIu8 "\n", hello2.Type);
	printf("Lenght: %" PRIu8 "\n", hello2.Length);
	printf("SOurce_id : %" PRIu64 "\n", hello2.Source_Id);
	printf("id %" PRIu64"\n", id);
	Message buf;
	int buf_len = sizeof(buf);
	memset(&buf,0,buf_len);


	struct timeval tv;
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);

	if(DEBUG) {
		printf("%s\n",sendMsg);/**//*//**/
	}

	again:

	rc = sendto(sock, &msg, sizeof(msg), 0, (struct sockaddr *)&local_addr, (socklen_t)client_len);
	
	if(rc < 0){
		//printf("rc = :%d", rc);
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
		perror("Magic");
		exit(EXIT_FAILURE);
	}

	if(DEBUG) {
		printf("%" PRIu8 " -> %s\n",buf.Tlv.Type,buf.Tlv.Body);
	}

	memset(&buf,0,BUF_SIZE);

	goto again2;//On attend 10s avant de relancer


	return 0;
}


