/*
 * iknlib.c
 *
 *  Created on: Sep 8, 2012
 *      Author: Lars Mortensen
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include "iknlib.h"

// *** Følgende funktioner bruges KUN i øvelse 8 ***

/**
 * Læser en tekststreng fra en socket
 *
 * @param inFromServer stream that holds data from server
 * @return En streng modtaget fra en socket
 */
const char *readTextTCP(char *text, int length, int inFromServer)
{
    char ch;
    int pos=0, len;

    len = read(inFromServer, &ch, 1);
    if(len != 1){}
    while(ch != 0)
    {
        if(pos < length)
            text[pos++] = ch;
        len = read(inFromServer, &ch, 1);
    }
    return text;
}

/**
 * Skriver en tekststreng til en socket med 0 terminering
 *
 * @param outToServer Stream hvortil der skrives data til socket
 * @param line Teksten der skal skrives til socket
 */
void writeTextTCP(int outToServer, char *line)
{
    int len = write(outToServer, line, strlen(line)+1);
    if(len == 0){}
}

/**
 * Konverter en streng som indeholder filstørrelsen in ascii format til en talværdi
 *
 * @param inFromServer Stream som indeholder data fra socket
 * @return Filstørrelsen som talværdi
 */
long getFileSizeTCP(int inFromServer)
{
    char buffer[256] = {};
    readTextTCP(buffer, sizeof(buffer), inFromServer);
    return atol(buffer);
}

// *** Følgende funktioner bruges både i øvelse 8 og 13 ***

/**
 * Udskriver fejlmeddelelse og stoppe programmet
 *
 * @param msg Fejlmeddelelse
 */
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

/**
 * Udtrækker et filnavn fra en tekststreng
 *
 * @param fileName Filnavn incl. evt. stinavn
 * @return Kun filnavn
 */
const char *extractFileName(const char *fileName)
{
    char *ecn;
    return ((ecn = (char *)strrchr(fileName,'/'))==0 ? fileName : ++ecn);
}

/**
 * Se om filen fileName eksisterer
 *
 * @param fileName Filnavn
 * @return Størrelsen på filen, 0 hvis den ikke findes
 */
long check_File_Exists(const char *fileName)
{
    struct stat sts;

    stat(fileName, &sts);
    if (stat (fileName, &sts) == -1)
        return 0;

    return sts.st_size;
}
