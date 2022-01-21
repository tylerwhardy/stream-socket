/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <string.h>

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 100000 // max number of bytes we can get at once 


void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	int messageCode;
	char returnMessage[12];

	int numbytes;
	char buf[MAXDATASIZE];

	socklen_t addr_len;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
			printf("Fork created\n");
			addr_len = sizeof their_addr;
			printf("Addr_len executed.\n");

			if ((numbytes = recvfrom(new_fd, buf, MAXDATASIZE-1, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1){
				perror("recvfrom");
				exit(1);
			}

			printf("Buffer: %s\n", buf);

			if (strspn(buf,"01234567890") == strlen(buf)){
				size_t digit = 0;
				sscanf(buf,"%zu%*c",&digit);
				messageCode = atoi(buf);
			}

			memset(returnMessage,'\0', sizeof(returnMessage));

			// By using a switch, 6 digit numbers are explicitly whitelisted
			switch(messageCode){
				case 111111:
					printf("Request for case 1. \n");
					strcpy(returnMessage,"Hotdog"); 
				break;
				case 222222:
					printf("Request for case 2. \n");
					strcpy(returnMessage,"Hamburger"); 
				break;
				case 333333:
					printf("Request for case 3. \n");
					strcpy(returnMessage,"Chicken"); 
				break;
				case 444444:
					printf("Request for case 4. \n");
					strcpy(returnMessage,"Turkey"); 
				break;
				case 555555:
					printf("Request for case 5. \n");
					strcpy(returnMessage,"Steak"); 				
				break;			
				default:
					printf("Error: Improper code request. \n");
					strcpy(returnMessage,"Illegal"); 
			}
			printf("Return Message: %s\n", returnMessage);

			buf[numbytes] = '\0';

			printf("client: received '%s'\n",returnMessage);

			close(sockfd); // child doesn't need the listener
			if (send(new_fd, returnMessage, strlen(returnMessage), 0) == -1)
				perror("send");
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}

