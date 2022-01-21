/*
** client.c -- a stream socket client demo
Tyler Hardy MP1
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <signal.h>
#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 
#define MAXDATASIZE 10000 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
    int codeLength;

// *** Listener Variables
	socklen_t addr_len;
	struct sockaddr_storage their_addr;

    codeLength = strlen(argv[2]);

    // Check for accurate input parameters
	if (argc != 3) {
	    fprintf(stderr,"usage: client hostname 6 digits\n");
	    exit(1);
	}

    // Validate code is correct length
	if (codeLength != 6) {
	    fprintf(stderr,"usage: client hostname _6_ digits\n");
	    exit(1);
	}

    // Validate code is, in fact, a code
    if (strspn(argv[2],"0123456789") != strlen(argv[2])){
        size_t digit = 0;
        sscanf(argv[2],"%zu%*c",&digit);
	    fprintf(stderr,"usage: client hostname 6 DIGITS\n");
	    exit(1);
    }

    printf("Destination: %s\n", argv[1]);
    printf("Code Request: %s\n", argv[2]);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
    printf("getaddrinfo complete\n");

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}
        printf("Servinfo complete\n");

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}
        printf("Breaking out of for loop...\n");        
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
    printf("P evaluated as non-null value\n");

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	if ((numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

    printf("Numbytes set equal to sendto.\n");


	addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(sockfd, buf, MAXDATASIZE-1 , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}

//	printf("listener: got packet from %s\n",
//		inet_ntop(their_addr.ss_family,
//			get_in_addr((struct sockaddr *)&their_addr),
//			s, sizeof s));

	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);
	freeaddrinfo(servinfo); // all done with this structure

	close(sockfd);
    printf("Closed socket connection...\n\n");
	return 0;
}

