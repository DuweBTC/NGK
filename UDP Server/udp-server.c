/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void uCommand(int newsocketfd);
void lCommand(int newsocketfd);

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    if (argc < 2) 
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
     
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    { 
        error("ERROR opening socket");
    }

    int reuse = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        error("ERROR on binding");
    }

    while (1)
    {
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
        {
            error("ERROR on accept");
        }
        
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) 
        {
            error("ERROR reading from socket");
        }

        if (buffer == "U" || "u")
        {
            uCommand(newsockfd);
        }
        if (buffer == "L" || "l")
        {
            lCommand(newsockfd);
        }

        printf("Here is the message: %s\n",buffer);
        
        n = write(newsockfd,"I got your message",18);
        
        if (n < 0)
        {
            error("ERROR writing to socket");
        }

    }


    close(newsockfd);
    close(sockfd);
    return 0; 
}

void uCommand(int newsockfd)
{
    FILE* ptr;
    char data[BUFSIZ];
    int i = 0;

    ptr = fopen("/proc/uptime", "r");

    while (!feof(ptr))
    {
        data[i] = fgetc(ptr);
        i++;
    }

    write(newsockfd,data,i-2);
    fclose(ptr);
}

void lCommand(int newsocketfd)
{
    FILE* ptr;
    char data[BUFSIZ];
    int i = 0;
    
    ptr = fopen("/proc/loadavg", "r");

    while (!feof(ptr))
    {
        data[i] = fgetc(ptr);
        i++;
    }

    fclose(ptr);

    write(newsocketfd,data,i-2);
}