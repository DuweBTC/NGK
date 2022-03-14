/*
* For at bygge brug følgende kommando i terminalen
* gcc -o udp-client udp-client-source.c
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
 
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
    
    int sock;
    struct sockaddr_in server_addr;
    struct hostent *server;

    char send_data[255];
 
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        error("kunne ikke åbne socket");
        exit(1);
    }

    server = gethostbyname(argv[1]);
    
    int portno = atoi(argv[2]);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);
    server_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(server_addr.sin_zero),8);
 
    printf("\nFor at afslutte indtast Q eller q");
    printf("\nFor at få server uptime indtast U\nFor at få server loadavg indtast L\n");
    
    while (1)
    {    
        fgets(send_data,255,stdin);
 
        if ((strcmp(send_data , "q") == 0) || strcmp(send_data , "Q") == 0)
        {
            break;
        } else {
            sendto(sock, send_data, strlen(send_data), 0,(struct sockaddr *)&server_addr, sizeof(struct sockaddr));
        }

        int bytes_read = recvfrom(sock, send_data, 255, 0,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));
        
        printf("\nBesked fra server: %s\n\n", send_data);

        memset(send_data, 0, sizeof(send_data)); // Tømmer send_data array
    }

    return 0;
}