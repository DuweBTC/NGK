/* udpserver.c */ 

/* 
* for at bygge programmet brug følgende kommando i terminal
* gcc -o udp-server udp-server-source.c
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

char data[BUFSIZ];
 int i = 0;

void uCommand();
void lCommand();

int main(int argc, char *argv[])
{
	int sock;
	int addr_len, bytes_read;
	char recv_data[255];
	struct sockaddr_in server_addr , client_addr;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{
		perror("Socket");
		exit(1);
	}

	int portno = atoi(argv[1]);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portno);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_addr.sin_zero),8);


	if (bind(sock,(struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Bind");
		exit(1);
	}

	addr_len = sizeof(struct sockaddr);
	
	printf("\nUDP server køre, venter på client på port: %d", portno);
	fflush(stdout);

	while (1)
	{
		bytes_read = recvfrom(sock,recv_data,1024,0,(struct sockaddr *)&client_addr, &addr_len);

		//recv_data[bytes_read] = '\0';
		recv_data[strcspn(recv_data, "\n")] = 0;

		printf("\nBesked modtaget fra (%s , %d) ",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		printf("\nBeskeden modtaget: %s", recv_data);

		if (strcmp(recv_data, "u") == 0 || strcmp(recv_data, "U") == 0)
		{
			uCommand();
			sendto(sock, data, i-2, 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
		} else if ( strcmp(recv_data, "l") == 0 || strcmp(recv_data, "L") == 0)
		{
			lCommand();
			sendto(sock, data, i-2, 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
		} else {
			sendto(sock, "Ukendt kommando", 15, 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
		}
		
		fflush(stdout);
	}
	
	return 0;
}

/*
* Functions for reading uptime file, and sending it to UDP-client
*/
void uCommand()
{
    FILE* ptr;
	i = 0;
    ptr = fopen("/proc/uptime", "r");

    while (!feof(ptr))
    {
        data[i] = fgetc(ptr);
        i++;
    }
    fclose(ptr);
}

/*
* Functions for reading loadAvg file, and sending it to UDP-client
*/
void lCommand()
{
    FILE* ptr;
	i = 0;
    ptr = fopen("/proc/loadavg", "r");

    while (!feof(ptr))
    {
        data[i] = fgetc(ptr);
        i++;
    }

    fclose(ptr);
}