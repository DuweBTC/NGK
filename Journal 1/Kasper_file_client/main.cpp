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

void receiveFile(string fileName, int socketfd);

int main(int argc, char *argv[])
{
	printf("Starting client...\n");

	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[BUFSIZE];
    
	if (argc < 3)
	{
		error( "ERROR usage: ""hostname"",  ""port""");
	}

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	{
		error("ERROR opening socket");
	}

	server = gethostbyname(argv[1]);
	if (server == NULL) 
	{
		error("ERROR no such host");
	}
	    

	printf("Server at: %s, port: %s\n",argv[1], argv[2]);

	printf("Connect...\n");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	{
		error("ERROR connecting");
	}

	printf("Please enter the message: ");
	fgets(buffer,sizeof(buffer),stdin);

    char *ptr = strchr(buffer, '\n');
    if (ptr)
    {
        //if new line found replace with null character
        *ptr  = '\0';
    }

	n = write(sockfd,buffer,strlen(buffer)+1);  // socket write
	if (n < 0) 
	{
		error("ERROR writing to socket");
	}
	
    bzero(buffer,sizeof(buffer));
	n = read(sockfd,buffer,sizeof(buffer));  // socket read
	if (n < 0) 
	{
		error("ERROR reading from socket");
	}
	printf("\n %s\n",buffer);

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
	// TO DO Your own code

	uint8_t Buffer[BUFSIZE];
	char *text;
	const char* text2;
	
	text2 = readTextTCP(text, BUFSIZ, sockfd);


	ofstream file_fw(fileName, std::ios::binary);  // Define output stream
	file_fw.write(text2, BUFSIZE);
	file_fw.close();
}

