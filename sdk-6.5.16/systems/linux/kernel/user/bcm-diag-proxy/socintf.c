/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * Requires:
 *     Socket library
 *
 *     
 * Provides:
 *     setup_socket
 *     wait_for_cnxn
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

int
setup_socket(int port)
{
    struct sockaddr_in serv_addr;
    int i;
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	
	perror("server: can't open stream socket");
	exit(1);
    }

    /*
     * Set socket option to reuse local address. This is supposed
     * to have the effect of freeing up the local address.
     */

    i = 1;
    if (setsockopt(sockfd, SOL_SOCKET,
                   SO_REUSEADDR, (char *) &i, 4) < 0) {
	perror("setsockopt");
    }

    /*
     * Set up server address...
     */

    memset((void *) &serv_addr,0x0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port        = htons(port);

    /*
     * Bind our local address & port
     */

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
	perror("server: can't bind local address");
	exit(1);
    }

    /*
     * Only process one connection at a time.
     */

    listen(sockfd, 1);

    return sockfd;
}

int 
wait_for_cnxn(int sockfd)
{
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    int newsockfd;

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0) {
	perror("accept");
	exit(1);
    }

    return newsockfd;
}


int 
conn(char* server, int port)
{
    struct sockaddr_in srv_addr;
    int sockfd = -1; 
    struct hostent* hostentPtr = NULL; 

    /*
     * Connect to host running Vera (ncsim)
     */
    memset((void *)&srv_addr, 0, sizeof(srv_addr));
    hostentPtr = gethostbyname(server);
    if(hostentPtr == NULL) {
	printf("pli_client_attach: hostname lookup failed "
	       "for host [%s].\n", server); 
	perror("gethostbyname");
	goto error;
    }
    memcpy(&srv_addr.sin_addr,hostentPtr->h_addr,4);

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	perror("server: can't open stream socket");
	goto error;
    }

    if (connect(sockfd,
		(struct sockaddr*)&srv_addr, sizeof(srv_addr)) < 0) {
	perror("connect");
	goto error;
    }
    return sockfd; 

 error:
    close(sockfd); 
    return -1; 
}





