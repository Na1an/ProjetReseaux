#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>

#define BUF_SIZE 4096
#define PORT 4242
#define ADDR "ff02::4242:4242" 

int main(){

	int rc=0;
	int sock=socket(AF_INET6,SOCK_DGRAM,0);

	if(sock<0){
		perror("create sock()");
		close(sock);
		exit(EXIT_FAILURE);
	}

	int val_1=1;
	int val_0=0;
	int err=-1;
	err=setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val_1, sizeof(val_1));
	if(err<0){
		perror("setsockopt - SOL_REUSEADDR");
	}
	
	err=setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &val_1, sizeof(val_1));
	if(err<0){
		perror("setsockopt - IPV6_V6ONLY");
	}

	err=setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &val_0, sizeof(val_0));
	if(err<0){
		perror("setsockopt - IPV6_MULTICAST_LOOP");
	}

	err=setsockopt(sock, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &val_1, sizeof(val_1));
	if(err<0){
		perror("setsockopt - IPV6_UNICAST_HOPS");
	}

	err=setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &val_1, sizeof(val_1));
	if(err<0){
		perror("setsockopt - SOL_SOCKET");
	}
	
	//initialisation local addr
	struct sockaddr_in6 local_addr;
	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin6_family=AF_INET6;
	local_addr.sin6_port=htons(PORT);

	local_addr.sin6_scope_id=if_nametoindex("wlp3s0");
	
	rc=inet_pton(AF_INET6, ADDR, &local_addr.sin6_addr);
	if(rc<0){
		fprintf(stderr, "Erreur de la consersion");
		return -1;
	}


	rc=bind(sock,(struct sockaddr *)&local_addr, sizeof(local_addr));

	if(rc<0){
		perror("bind");
		close(sock);
		exit(EXIT_FAILURE);
	}

	unsigned char reply[4096] = "@Bai : Bienvenue";
	int reply_len = sizeof(reply);
	socklen_t client_len = sizeof(local_addr);

	rc=sendto(sock, reply, reply_len, 0,(struct sockaddr *)&local_addr, (socklen_t)client_len);
	
	if(rc<0){
		perror("sendto");
		exit(EXIT_FAILURE);
	}

	//join multicast
	struct ipv6_mreq mreq;
	rc=inet_pton(AF_INET6, ADDR, &mreq.ipv6mr_multiaddr);
	if(rc<0) perror("inet_pton");
	mreq.ipv6mr_interface = if_nametoindex("wlp3s0");
	rc=setsockopt(sock, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq));
	if(rc<0) perror("setsockopt IPV6_JOIN_GROUPE");
	
	char buf[4096];
	while(1){

		memset(buf, 0, sizeof(buf));
		rc = recvfrom(sock, buf, 4096, 0, (struct sockaddr*)&local_addr, &client_len);
		printf("%s\n", buf);
		
	}	

	return 0;
}


