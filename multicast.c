#include "multicast.h"

int get_socket_and_server_addr(int *sock, struct sockaddr_in6 *addr) {

	int err = -1;
	/*int ifindex = if_nametoindex(IFCONFIG);
	if(ifindex == 0) {
		perror("if_nametoindex");
		return -3;
	}*/

	struct addrinfo hints, *r, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if ((getaddrinfo(ADDR, PORT, &hints, &r)) != 0 || NULL == r) {
		perror("getaddrinfo");
		return -1;
	}

	/* Init sock */
	for (
		p = r;
		(NULL != p && (*sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0);
		p = p->ai_next
	);
	if (NULL == p) {
		perror("create socket");
		return -2;
	}

	/* Init addr */
	*addr = *((struct sockaddr_in6*) p->ai_addr);
	/*addr->sin6_scope_id = ifindex;*/

	freeaddrinfo(r);

	/* Set Option sock */
	/*struct ipv6_mreq mreq;

	memset(&mreq, 0, sizeof(mreq));
	memcpy(&mreq.ipv6mr_multiaddr, &(addr->sin6_addr), sizeof(addr->sin6_addr));
	mreq.ipv6mr_interface = ifindex;

	err = setsockopt(*sock, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq));
	if(err < 0) {
		perror("setsockopt - IPV6_JOIN_GROUP");
		//return -3;
	}*/

	int val_1 = 1;
	int val_0 = 0;

	err = setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &val_1, sizeof(val_1));
	if(err < 0) {
		perror("setsockopt - SOL_REUSEADDR");
		return -3;
	}
	
	err = setsockopt(*sock, IPPROTO_IPV6, IPV6_V6ONLY, &val_1, sizeof(val_1));
	if(err < 0) {
		perror("setsockopt - IPV6_V6ONLY");
		return -3;
	}

	/*err = setsockopt(*sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &val_0, sizeof(val_0));
	if(err < 0) {
		perror("setsockopt - IPV6_MULTICAST_LOOP");
		return -3;
	}

	err = setsockopt(*sock, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &val_1, sizeof(val_1));
	if(err < 0) {
		perror("setsockopt - IPV6_UNICAST_HOPS");
		return -3;
	}

	err = setsockopt(*sock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &val_1, sizeof(val_1));
	if(err < 0) {
		perror("setsockopt - IPV6_MULTICAST_HOPS");
		return -3;
	}

	err = bind(*sock,(struct sockaddr *)addr, sizeof(*addr));
	if(err < 0){
		perror("bind");
		return -3;
	}*/

	return 0;
}

int main() {
	
	int sock, rc;
	struct sockaddr_in6 local_addr;

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

	char reply[BUF_SIZE];
	int reply_len = snprintf(reply, BUF_SIZE, "@Bai : Bienvenue");
	socklen_t client_len = sizeof(local_addr);

	char buf[BUF_SIZE];
	memset(buf,0,BUF_SIZE);

	struct timeval tv;
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);

	again:

	printf("%d, %s\n", reply_len, reply);
	rc = sendto(sock, reply, reply_len, 0, (struct sockaddr *)&local_addr, (socklen_t)client_len);
	
	if(rc < 0){
		perror("sendto");
		exit(EXIT_FAILURE);
	}
	
	printf("Message Envoyé !\n");

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
			rc = recvfrom(sock, &buf, BUF_SIZE, 0, NULL, NULL);
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
		printf("Encore !\n");
		goto again;
	}
  
	ok:

	printf("%s\n", buf);
	memset(buf,0,BUF_SIZE);

	goto again2;//On attend 10s avant de relancer

	return 0;
}


