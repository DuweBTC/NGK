//============================================================================
// Description : file_server in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "iknlib.h"

using namespace std;
#define BUFFSIZE 1000

void sendFile(string fileName, long fileSize, int outToClient);

/**
 * main starter serveren og venter på en forbindelse fra en klient
 * Læser filnavn som kommer fra klienten.
 * Undersøger om filens findes på serveren.
 * Sender filstørrelsen tilbage til klienten (0 = Filens findes ikke)
 * Hvis filen findes sendes den nu til klienten
 *
 * HUSK at lukke forbindelsen til klienten og filen når denne er sendt til klienten
 *
 * @throws IOException
 *
 */
int main(int argc, char *argv[])
{
    printf("Starting server...\n");

	int sockfd, newsockfd, portno;

	socklen_t clilen;
	char buffer[BUFSIZE];
	char bufferTx[BUFSIZE];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

    uint8_t Buffer[BUFFSIZE];
    long fileSize;


	if (argc < 2) 
	{
        error("ERROR no port provided");
    }


	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 

	{
        error("ERROR opening socket");
    }

	int reuse = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

	printf("Binding...\n");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
	{
        error("ERROR on binding");
    }

	printf("Listen...\n");
	listen(sockfd,5);

	clilen = sizeof(cli_addr);

	for (;;)
	{
		printf("Accept...\n");
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		if (newsockfd < 0) 
        {
            error("ERROR on accept");
        } else {
            printf("Accepted\n");
        }

		bzero(buffer,sizeof(buffer));
		n = read(newsockfd,buffer,sizeof(buffer));

		if (n < 0) 
        {
            error("ERROR reading from socket");
        }
        printf("Message: %s\n",buffer);

		const char* filename = extractFileName(buffer);

        fileSize = check_File_Exists(filename);
		if (fileSize == 0)
		{
			snprintf(bufferTx, sizeof(bufferTx), "File does not exist\n");
		} else {
			printf("fandt filen\n");
			sendFile(filename,fileSize, newsockfd);
		}

        cout << "filename is: " << filename << " filesize is: "<< fileSize << endl; // debug

		snprintf(bufferTx, sizeof(bufferTx), "Got message: %s",buffer);

		n = write(newsockfd,bufferTx,strlen(bufferTx));

		if (n < 0) 
        {
            error("ERROR writing to socket");
        }

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
void sendFile(string fileName, long fileSize, int outToClient)
{
	// char buffer[BUFSIZE];

	// ifstream file_fr(fileName, std::ios::binary);  // Define input stream
	// file_fr.read((char*)buffer, BUFSIZE);  // Automatic seek!
	// writeTextTCP(outToClient, buffer);
	// file_fr.close();

}