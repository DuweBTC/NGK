//============================================================================
// Description : file_client in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "iknlib.h"

using namespace std;
fstream file;

#define BUFSIZE 1000

void receiveFile(string fileName, int socketfd);



int main(int argc, char *argv[])
{
	
	
	printf("Starting client...\n");

	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[BUFSIZE];
    
	if (argc < 3){
	    printf( "ERROR usage: ""hostname"",  ""port""");
		exit(1);
	}

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
	    printf("ERROR opening socket");
		exit(1);
	}
	std::string fileName = "server.txt";
	file.open(".//file//" + fileName, std::ios::out | std::ios::trunc | std::ios::binary);
	if(file.is_open()){
		printf("File is created\n");
	} else {
		printf("File creation failed\n");
		exit(1);
	}

	server = gethostbyname(argv[1]);
	if (server == NULL){ 
	    printf("ERROR no such host");
		exit(1);
	}

	printf("Server at: %s, port: %s\n",argv[1], argv[2]);

	printf("Connect...\n");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
	    printf("ERROR connecting");
		exit(1);
	}

	printf("Please enter the message: ");
	fgets(buffer,sizeof(buffer),stdin);
	n = write(sockfd,buffer,strlen(buffer));  // socket write
	if (n < 0) {
	    printf("ERROR writing to socket");
		exit(1);
	}
    bzero(buffer,sizeof(buffer));
	n = read(sockfd,buffer,sizeof(buffer));  // socket read
	if (n < 0){ 
	    printf("ERROR reading from socket");
		exit(1);
	}
	printf("\n%s\n",buffer);

	receiveFile(buffer, sockfd);

    printf("Closing client...\n\n");
	close(sockfd);
	return 0;
}

/**
 * Modtager filstørrelsen og udskriver meddelelsen: "Filen findes ikke" hvis størrelsen = 0
 * ellers
 * Åbnes filen for at skrive de bytes som senere modtages fra serveren (HUSK kun selve filnavn)
 * Modtag og gem filen i blokke af 1000 bytes indtil alle bytes er modtaget.
 * Luk filen, samt input output streams
 *
 * @param fileName Det fulde filnavn incl. evt. stinavn
 * @param sockfd Stream for at skrive til/læse fra serveren
 */
void receiveFile(string fileName, int sockfd)
{
	
	char buffer[1000] = {};
	int valread = read(sockfd, buffer, sizeof(buffer));
	file << buffer;
}

