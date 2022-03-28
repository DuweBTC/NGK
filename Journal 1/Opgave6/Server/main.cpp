//============================================================================
// Description : file_server in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "iknlib.h"
#include <arpa/inet.h>
#define BUFSIZE 1000



void sendFile(char* fileName, long fileSize, int outToClient)
{
	//Opening a fie type fstream 
    std::fstream file;
	
	//A way to make a char * to string data
	std::string fileName2 = "";
		for (int i = 0; i <= sizeof(fileName)+1; i++){
			fileName2 += fileName[i];
		}	

	//File location
	//Is the local folder, but inside a file folder with the specifyed name
	std::string filepath = ".//file//" + fileName2;
	std::cout << filepath << std::endl;

	//Operataion to open the file and read it as binary
	file.open(filepath, std::ios::in | std::ios::binary);
	
	//Check if file is open
	if(file.is_open()){
		printf("File is ready to transmit\n");
	} else {
		printf("File loading failed\n");
		exit(1);
	}

	// Read from file - How do is now the path here?
	//("Reading the file\n");
	//int n = read(outToClient,fileName,sizeof(fileName));
	//if (n < 0){
	//		printf("ERROR reading from file");
	//		exit(1);
	//	}
	//		printf("Successful reading for file");

	//Transmit file
	std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	int bytes_sent = send(outToClient , contents.c_str() , contents.length() , 0 );
}

int main(int argc, char *argv[])
{
    	printf("Starting server...\n");


	//Variables
	int sockfd, newsockfd, portno;

	socklen_t clilen;
	char buffer[BUFSIZE];
	char bufferTx[BUFSIZE];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	
	
	//No enough argmunts
	if (argc < 2){ 
		printf("ERROR no port provided");
		exit(1);
	}

	//Opning socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){ 
		printf("ERROR opening socket");
		exit(1);
	}
	//Binding 
	printf("Binding...\n");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){ 
		printf("ERROR on binding");
		exit(1);
	}

	printf("Listen...\n");
	listen(sockfd,5);
	
	clilen = sizeof(cli_addr);

	for (;;)
	{
		printf("Accept...\n");
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		if (newsockfd < 0) {
			printf("ERROR on accept");
			exit(1);
		}
		else printf("Accepted\n");


		bzero(buffer,sizeof(buffer));
		n = read(newsockfd,buffer,sizeof(buffer));



		if (n < 0){
			printf("ERROR reading from socket");
			exit(1);
		}
			printf("Message: %s\n",buffer);
		
		snprintf(bufferTx, sizeof(bufferTx), "Got message: %s",buffer);


		n = write(newsockfd,bufferTx,strlen(bufferTx));
		if (n < 0) {
			printf("ERROR writing to socket");
			exit(1);
		}

	
		sendFile(buffer, sizeof(n), newsockfd);
		
		close(newsockfd);
	}
	close(sockfd);
	return 0; 
}

/**
 * Sender filen som har navnet fileName til klienten
 *
 * @param fileName Filnavn som skal sendes til klienten
 * @param fileSize Størrelsen på filen, 0 hvis den ikke findes
 * @param outToClient Stream som der skrives til socket
     */

