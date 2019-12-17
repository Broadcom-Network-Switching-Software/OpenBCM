/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * The part of PCID that interfaces to sockets
 *
 * Requires:
 *     Socket library
 *     Verinet functions (write_command, etc)
 *     pli_getreg_service
 *     pli_setreg_service
 *
 *     
 * Provides:
 *     pcid_setup_socket
 *     pcid_wait_for_cnxn
 *     pcid_process_request
 */

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "pcid.h"
#include "mem.h"
#include "cmicsim.h"
#include "dma.h"
#include "pli.h"

int
pcid_setup_socket(int port)
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

int pcid_wait_for_cnxn(int sockfd)
{
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    int newsockfd;

    for (;;) {
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

	if (newsockfd >= 0) {
	    break;
	}

	if (errno != EINTR) {
	    perror("accept");
	    exit(1);
	}
    }

    return newsockfd;
}

void pcid_close_cnxn(int sockfd)
{
    close(sockfd);
}

