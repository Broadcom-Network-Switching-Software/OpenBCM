/*! \file pcid.c
 *
 * PLI server
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sim/pli/pcid.h>

#include "verinet.h"

static uint32_t test_value;

static uint32_t
getreg_service(int unit, uint32_t type, uint32_t regnum)
{
    return test_value;
}

static void
setreg_service(int unit, uint32_t type, uint32_t regnum, uint32_t value)
{
    test_value = value;
}

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

int
pcid_wait_for_cnxn(int sockfd)
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
            cli_out("intr: accept failed (%s)\n",
                    strerror(errno));
            return -1;
        }
    }

    return newsockfd;
}

void
pcid_close_cnxn(int sockfd)
{
    close(sockfd);
}

/*
 * Handles an incoming client request
 * We block until the client has something to send us and then
 * we do something over the socket descriptor.
 * Returns 1 when a command is processed, 0 on disconnect.
 */
int
pcid_process_request(int sockfd, struct timeval *tmout)
{
    static netconnection_t *nc;
    static rpc_cmd_t cmd;
    char buf[64];
    uint32_t regval, idx;
    int rv, resp, unit = 0;

    rv = get_command(sockfd, tmout, &cmd);

    if (rv == 0) {
        return PR_NO_REQUEST;    /* No request; return to poll later */
    }

    if (rv < 0) {        /* Error; cause polling loop to exit */
        return PR_ERROR;
    }

    /*
     * We only handle requests: anything else is a programming
     * error. Once we are here, all parameters have been marshalled
     * and we simply need to call the correct routine (see pli_cmd.c).
     */
    switch (cmd.opcode) {
    case RPC_SCHAN_REQ:
        break;

    case RPC_GETREG_REQ: /* PIO read of register */
        /* Get a register from our model ... */
        regval = getreg_service(unit, cmd.args[1], cmd.args[0]);

        /* coverity[secure_coding] */
        strcpy(buf, "********");
        make_rpc_getreg_resp(&cmd, 0, regval, buf);
        if (write_command(sockfd, &cmd) != RPC_OK) {
            return PR_ERROR;
        }

        break;

    case RPC_SETREG_REQ: /* PIO write to register */
        /* Set a register on our model... */
        /* coverity[stack_use_overflow] */
        setreg_service(unit, cmd.args[2], cmd.args[0], cmd.args[1]);
        make_rpc_setreg_resp(&cmd, cmd.args[1]);
        if (write_command(sockfd, &cmd) != RPC_OK) {
            return PR_ERROR;
        }
        break;

    case RPC_IOCTL_REQ: /* Change something inside the model */
        break;

    case RPC_SHUTDOWN: /* Change something inside the model */
        break;

    case RPC_REGISTER_CLIENT:
        cli_out("Received register client request (%d)\n",
                cmd.argcount);

        /* Restore IP addr */
        for (idx = 2; idx < cmd.argcount; idx++) {
            cmd.args[idx] = htonl(cmd.args[idx]);
        }
        /* Connect to client interrupt and DMA ports and test */
        nc = register_client(sockfd,
                             cmd.args[0], cmd.args[1],
                             cmd.argcount - 2, &cmd.args[2]);
        if (nc == NULL) {
            return PR_ALL_DONE;
        }

        resp = RPC_OK;

        /* TEST: Write to DMA address 0 on client */
        dma_writemem(nc->dmasock, 0, 0xbabeface);

        /* TEST: Read from DMA address 0 on client */
        regval = dma_readmem(nc->dmasock, 0);

        /* TEST: Result should be the same, if not FAIL! */
        if (regval != 0xbabeface) {
            cli_out("Error: regval = 0x%x\n", regval);
            resp = RPC_FAIL;
        }

        /* TEST: Send an interrupt to the client */
        rv = send_interrupt(nc->intsock, 0);
        if (rv < 0) {
            cli_out("Error: interrupt test failed.\n");
            resp = RPC_FAIL;
        }

        /* Send Registration status back ... */
        make_rpc_register_resp(&cmd, resp);
        if (write_command(nc->piosock, &cmd) != RPC_OK) {
            return PR_ERROR;
        }

        cli_out("Client registration: %s DMA=0x%x, INT=0x%x, PIO=0x%x\n",
                inet_str(&nc->inetaddr, buf, sizeof(buf)),
                nc->dmasock, nc->intsock, nc->piosock);
        break;

    case RPC_DISCONNECT:
        /* Disconnect the interrupt and DMA sockets on client */
        unregister_client(nc);
        return PR_ALL_DONE;

    default:
        /* Unknown opcode */
        break;
    }

    return PR_REQUEST_HANDLED;
}
