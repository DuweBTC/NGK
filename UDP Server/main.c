#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

/*Function declarations*/
void setupUDP(char *argv[]); // setup UDP server
void listner(); // function for listening for command
void reader(); // function for reading command
void uCommand(); // function for handling u command
void lCommand(); // function for handling l command

/* Global variables used in the program */
int sockfd, newsockfd, portno;

/*
* error handling function
* @param msg message to handle error messages
*/
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/*
* Main function to run the UDP server
* @param argc argument count
* @param argv array containing the arguments send to the main program
*/
int main(int argc, char *argv[])
{
    // Controll of user has send port number to to program
    if (argc < 2) 
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    // run setupUDP
    setupUDP(argv);
     
    while (1)
    {
        listner();
        reader();
    }

    close(newsockfd);
    close(sockfd);
    
    return 0; 
}

/*
* Function to setup UDP server
* @param argv reciving argv from function call, in this case from main, argv contains the portnumber
*/
void setupUDP(char *argv[])
{
    struct sockaddr_in serv_addr;

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
}

/*
* Function used to listen for messages from client
*/
void listner()
{
    struct sockaddr_in cli_addr;
    socklen_t clilen;

    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
    {
        error("ERROR on accept");
    }
}

/*
* Function to read the command send from the client
*/
void reader()
{
    char buffer[256];
    int n;
    
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0) 
    {
        error("ERROR reading from socket");
    }
    buffer[strcspn(buffer, "\n")] = 0;
    printf("Kommando modtaget: %s\n",buffer);

    if (strcmp(buffer, "U") == 0 || strcmp(buffer, "u") == 0)
    {
        uCommand(newsockfd);
    } else if (strcmp(buffer, "L") == 0 || strcmp(buffer, "l") == 0) 
    {
        lCommand(newsockfd);
    } else {
        write(newsockfd, "No such command", 15);
    }

    if (n < 0)
    {
        error("ERROR writing to socket");
    } 
}

/*
* Functions for reading uptime file, and sending it to UDP-client
*/
void uCommand()
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

/*
* Functions for reading loadAvg file, and sending it to UDP-client
*/
void lCommand()
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

    write(newsockfd,data,i-2);
}
