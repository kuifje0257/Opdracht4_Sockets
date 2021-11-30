/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2020.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 57-4 */

/* us_xfr_cl.c

   An example UNIX domain stream socket client. This client transmits contents
   of stdin to a server socket.

   See also us_xfr_sv.c.
*/
#include "ud_ucase.h"
#include "PJ_RPI.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    if(map_peripheral(&gpio) == -1) 
	{
		printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
		return -1;
	}

    struct sockaddr_un addr;
    int sfd, j;
    ssize_t numRead;
    char buf[BUF_SIZE];

    struct sockaddr_un svaddr, claddr;
    size_t msgLen;
    ssize_t numBytes;
    char resp[BUF_SIZE];

    //data to send
    int GpioNum;
    int ToggleTime;
    char Data2Send[15];
    

    printf("Geef een GPIO number (17,22,27):");
    gets(GpioNum);
    printf("\nGeef een Toggle speed (in seconden):");
    gets(ToggleTime);

    printf("Gpio: %d zal om de %d seconden Aan/Uit gaan\n",GpioNum,ToggleTime);
    snprintf(Data2Send,"%d,%d",GpioNum,ToggleTime);
    //char buf[]={GpioNum,ToggleTime};
    //

    /* Create client socket; bind to unique pathname (based on PID) */

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        errExit("socket");

    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    snprintf(claddr.sun_path, sizeof(claddr.sun_path),
            "/tmp/ud_ucase_cl.%ld", (long) getpid());

    if (bind(sfd, (struct sockaddr *) &claddr, sizeof(struct sockaddr_un)) == -1)
        errExit("bind");

    /* Construct address of server */

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    /* Send messages to server; echo responses on stdout */

    for (j = 1; j < 2; j++) {
        msgLen = strlen(Data2Send);       /* May be longer than BUF_SIZE */
        if (sendto(sfd, Data2Send, msgLen, 0, (struct sockaddr *) &svaddr,
                sizeof(struct sockaddr_un)) != msgLen)
            fatal("sendto");

        numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
        /* Or equivalently: numBytes = recv(sfd, resp, BUF_SIZE, 0);
                        or: numBytes = read(sfd, resp, BUF_SIZE); */
        if (numBytes == -1)
            errExit("recvfrom");
        printf("Response %d: %.*s\n", j, (int) numBytes, resp);
    }
    exit(EXIT_SUCCESS);         /* Closes our socket; server sees EOF */
}
