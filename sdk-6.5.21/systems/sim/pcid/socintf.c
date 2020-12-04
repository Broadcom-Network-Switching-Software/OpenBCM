/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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

#include <shared/bsl.h>

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
    struct sockaddr_in6 serv_addr;
    int sockopt;
    int sockfd;

    if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        cli_out("server: can't open stream socket (%s)\n",
                    strerror(errno));
        return -1;
    }

    sockopt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                   (char *)&sockopt, sizeof(sockopt)) < 0) {
        cli_out("server: setsockopt failed (%s)\n",
                strerror(errno));
    }

    sockopt = 0;
    if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY,
                   (char *)&sockopt, sizeof(sockopt)) < 0) {
        cli_out("server: setsockopt failed (%s)\n",
                strerror(errno));
    }

    /* Set up server address */
    memset((void *) &serv_addr, 0x0, sizeof(serv_addr));
    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_addr = in6addr_any;
    serv_addr.sin6_port = htons(port);

    /* Bind our local address & port */
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cli_out("server: unable to bind address (%s)\n",
                strerror(errno));
        close(sockfd);
        return -1;
    }

    /* Only process one connection at a time */
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

