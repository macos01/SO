#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>

#define MAXDATA 256

/*
Socket strict UDP IPv4 0.0.0.0 (AF_INET IPv4 SOCK_DGRAM IPPROTO_UDP)
*/

/**
 * 	Domain= AF_INET IPv4
 * 	Type = SOCK_DGRAM IPPROTO_UDP
 */
	
	/**
 * 	SERVER:
 * 	-Socket opening -> socket()
 * 	-Associate address -> bind()
 * 	-Comunication -> recvfrom() - sendto()
 * 	-Close -> close()
 *
 * 	CLIENT:
 * 	-Socket opening -> socket()
 * 	-Comunication -> sendto() - recvfrom()
 * 	-Close -> close()
 */	


int main (int argc, const char *argv[]){
	
	int bytes = 0, fd_socket, resolv;
	char buffer[MAXDATA];
	char host[NI_MAXHOST];
	char serv[NI_MAXSERV];
	
	//structure that will be filled with the client's data. sockaddr_in -> (IPv4)
	struct sockaddr_in client;
	socklen_t client_len = sizeof(client);

	fprintf(stdout,"*** SOCKET UDP ***\n");
	
	if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
	}
	
	//set addrinfo struct to getaddrinfo
	struct addrinfo hints = {
		.ai_flags = AI_PASSIVE, 
		.ai_family = AF_INET,
		.ai_socktype = SOCK_DGRAM,
		.ai_protocol = IPPROTO_UDP,
		.ai_addrlen = 0,
		.ai_canonname = NULL,
		.ai_addr = NULL,
		.ai_next = NULL	
	}; 
	
	//the result of getaddrinfo after hints
	struct addrinfo *result; 

	/*
	 AI_PASSIVE with NULL -> 0.0.0.0
	*/
	if (getaddrinfo(NULL,argv[1],&hints,&result) != 0){
		fprintf(stderr,"ERROR(%d): %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	//create socket
	fd_socket = socket(result->ai_family,result->ai_family,result->ai_protocol);
	if (fd_socket == -1){
		fprintf(stderr,"ERROR(%d): %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	//assigning a name to a socket
	if (bind(fd_socket, (struct sockaddr *) result->ai_addr,result->ai_addrlen )== -1){
		fprintf(stderr,"ERROR(%d): %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	//loop
	while (1) {
		bytes = recvfrom(fd_socket, buffer, MAXDATA, 0, (struct sockaddr *) &client, &client_len);
		if(bytes == -1){
			fprintf(stderr,"Ignored ERROR(%d): %s\n", errno, strerror(errno));
		}
		buffer[bytes]='\0';
		
		//the inverse of getaddrinfo
		resolv = getnameinfo((struct sockaddr *) &client, client_len,host, NI_MAXHOST, serv, NI_MAXSERV,NI_NUMERICHOST|NI_NUMERICSERV);
		if (resolv == 0) {
			fprintf(stdout,"Received (%i) bytes from %s:%s\n",bytes, host, serv);
		}
	
		//print message
		fprintf(stdout,"Message: %s\n", buffer);
		
		//echo server
		sendto(fd_socket, buffer, bytes, 0, (struct sockaddr *) &client, client_len);

	}
	
	return (EXIT_SUCCESS);	
	
}//main