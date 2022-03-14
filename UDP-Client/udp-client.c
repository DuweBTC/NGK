#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

int sockfd, portno;
char buffer[256];

void openFunc(char *argv[]);
void connectFunc(char *argv[]);
void writerFunc();
void readerFunc();

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc < 3) 
    {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    openFunc(argv);
    connectFunc(argv);
    writerFunc();
    readerFunc();

    close(sockfd);
    return 0;
}

void openFunc(char *argv[])
{
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        error("ERROR, Kunne ikke åbne socket");
    }
}

void connectFunc(char *argv[])
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

    server = gethostbyname(argv[1]);
    if (server == NULL) 
    {
        fprintf(stderr,"ERROR, Ingen host blev fundet\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    {
        error("ERROR, Kunne ikke forbinde");
    }
}

void writerFunc()
{
    int n;
    printf("For at få UDP uptime indtast 'U'\nFor at få UDP loadAVG indtast 'L'\n");
    printf("Indtast kommando: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
    { 
        error("ERROR, Fejl ved skrivning til socket");
    }
}

void readerFunc()
{
    int n;
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0)
    {
        error("ERROR, Kunne ikke læse fra socket");
    }
    printf("%s\n",buffer);
}