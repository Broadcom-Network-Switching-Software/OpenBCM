/*! \file verinet.c
 *
 * Routines for accessing the network and sending RPC commands to/from
 * the PLI test-server.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <unistd.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>

#include <sal/sal_mutex.h>

#include "verinet.h"

/*
 * From pcid.h: bcm_sim_msg_t
 */
#define BCM_SIM_ACTIVATE        1

/*
 * One verinet structure per dev_no (unit)
 */
verinet_t verinet[N_VERINET_DEVICES];

/*
 *  Write "n" bytes to a descriptor.
 */
int
writen(int fd, void *ptr, int nbytes)
{
    int nleft, nwritten;
    nleft = nbytes;
    while (nleft > 0) {
        nwritten = write(fd, ptr, nleft);
        if (nwritten <= 0)
            return(nwritten);
        nleft -= nwritten;
        ptr = (char *)ptr + nwritten;
    }
    return(nbytes - nleft);
}


/*
 *  Read "n" bytes from a descriptor.
 */
int
readn(int fd, void *ptr, int nbytes)
{
    int nleft, nread;
    nleft = nbytes;
    while (nleft > 0) {
        nread = read(fd, ptr, nleft);
        if (nread < 0)
            return(nread);              /* error, return < 0 */
        else if (nread == 0)
            break;                      /* EOF */
        nleft -= nread;
        ptr = (char *)ptr + nread;
    }
    return(nbytes - nleft);             /* return >= 0 */
}

/*
 * Get the port number to which a socket is bound.
 */

int
getsockport(int sock_fd)
{
    ipaddr_t ipaddr;
    socklen_t addrlen;

    addrlen = sizeof(ipaddr);

    if (getsockname(sock_fd, &ipaddr.sa, &addrlen) < 0) {
        cli_out("getsockname failed (%s)\n",
                strerror(errno));
        return 0;
    }

    return ntohs(ipaddr.sin.sin_port);
}

/*
 * IP address to text.
 */

const char *
inet_str(const ipaddr_t *ipaddr, char *dst, socklen_t size)
{
    const void *addr = &ipaddr->sin.sin_addr;

    if (ipaddr->sin.sin_family == AF_INET6) {
        addr = &ipaddr->sin6.sin6_addr;
    }
    return inet_ntop(ipaddr->sin.sin_family, addr, dst, size);
}

/*
 * reads an RPC command.
 * Parameters:
 *  sockfd -socket file descriptor.
 * Returns:
 *      -1 on error (nothing returned in rpc_cmd_t *)
 *       0 if no command is found (nothing returned in rpc_cmd_t *)
 *       1 if command is found (returned in rpc_cmd_t *)
 */

int
get_command(int sockfd, struct timeval *tv, rpc_cmd_t *cmd)
{
    int rv;
    fd_set read_vect;
    uint32_t i, swap;

    LOG_DEBUG(BSL_LS_SYS_VERINET,
              (BSL_META("get_command: sockfd=%d\n"),
               sockfd));

    if (sockfd <= 2) {
        /* Verinet not initialized? */
        return -1;
    }

    /* Setup bitmap for read notification ... */
    FD_ZERO(&read_vect);
    FD_SET(sockfd, &read_vect);
    do {
        rv = select(sockfd+1, &read_vect, (fd_set*)0x0, (fd_set*)0x0, tv);
    } while (rv < 0 && errno == EINTR);
    if (rv < 0) {
        cli_out("get_command: select error (%s)\n",
                strerror(errno));
        goto fail;
    }
    /*
     * Data ready to be read.
     */
    if (FD_ISSET(sockfd, &read_vect)) {

        /*
         * Read RPC opcode, argcount, status
         */
#define SOCKET_GET_U32(sockfd, swap, where, what) \
    do { \
        if (readn(sockfd, &(swap), sizeof(uint32_t)) < (int)sizeof(uint32_t)) { \
            cli_out("get_command: could not read RPC %s\n", what); \
            goto fail; \
        } \
        (where) = ntohl(swap);  /* Convert to host order */ \
    } while (0)

        SOCKET_GET_U32(sockfd, swap, cmd->opcode, "opcode");
        SOCKET_GET_U32(sockfd, swap, cmd->argcount, "arg count");

        if (RPC_OP_USE_STRING(cmd->opcode)) {
            if (cmd->argcount > (sizeof(uint32_t) * MAX_RPC_ARGS)) {
                cli_out("RPC string too long (get): %d\n", cmd->argcount);
                goto fail;
            }
            SOCKET_GET_U32(sockfd, swap, cmd->args[0], "pkt address");

            if (readn(sockfd, &cmd->args[1], cmd->argcount) <
                (int)cmd->argcount) {
                cli_out("get_command: could not read all of string. %d.\n",
                        cmd->argcount);
                goto fail;
            }
        } else {
            if (cmd->argcount > MAX_RPC_ARGS) {
                cli_out("get_command: bogus argcount (%d) from rpc socket\n",
                        cmd->argcount);
                goto fail;
            }
            /* Read each argument into command struct ... */
            for (i = 0; i < cmd->argcount; i++) {
                SOCKET_GET_U32(sockfd, swap, cmd->args[i], "argument");
            }
        }

        SOCKET_GET_U32(sockfd, swap, cmd->status, "status");

#undef SOCKET_GET_U32

        LOG_VERBOSE(BSL_LS_SYS_VERINET,
                    (BSL_META("get_command: opcode=0x%x nargs=%d status=%d\n"),
                     cmd->opcode, cmd->argcount, cmd->status));

        return 1;
    }

    return 0;   /* Time expire with no command */

 fail:
    return -1;
}


/*
 * Non-blocking version of actual call made by PLI interface.
 * Note that the timeout is 0 which guarantees that the Verilog
 * model does not block.
 *
 * Returns:
 *      -1 on error (nothing returned in rpc_cmd_t *)
 *       0 if no command is found (nothing returned in rpc_cmd_t *)
 *       1 if command is found (returned in rpc_cmd_t *)
 */
int
poll_command(int sockfd, rpc_cmd_t *cmd)
{
    struct timeval tv = {0,0};
    return get_command(sockfd, (struct timeval*)&tv, cmd);
}

/*
 * Blocking version of actual call made by PLI interface.
 *
 * Returns:
 *      -1 on error (nothing returned in rpc_cmd_t *)
 *       0 on success
 */
int
wait_command(int sockfd, rpc_cmd_t *cmd)
{
    return get_command(sockfd, (struct timeval*)0, cmd);
}


/*
 * Writes an RPC command to the server.
 * Parameters:
 * 1) sockfd-socket file descriptor.
 * 2) command -rpc_cmd_t filled in for the necessary command.
 *
 * Return values:
 *  RPC_OK on success
 *  RPC_FAIL on error
 */
int
write_command(int sockfd, rpc_cmd_t *cmd)
{
    uint8_t write_buf[sizeof(uint32_t) * (MAX_RPC_ARGS + 4)];
    int write_cnt;
    uint32_t i, swap;

    LOG_VERBOSE(BSL_LS_SYS_VERINET,
                (BSL_META("write_command: sockfd=%d opcode=0x%x nargs=%d status=%d\n"),
                 sockfd, cmd->opcode, cmd->argcount, cmd->status));

    if (sockfd <= 2) {
        /* Verinet not initialized? */
        return -1;
    }

    /*
     * Each value is converted to network byte order before writing.
     * On some hosts (e.g. SPARC,MIPS, COLDFire which run in big-endian
     * mode, these calls are NOPS, but we include them here for
     * correctness and porting to other platforms).
     *
     * Data is saved in a buffer and written using one write to make
     * performance over the network about a ZILLION times faster.
     */

    write_cnt = 0;

    /* RPC opcode */
#define STORE_U32(val, buf, swap, idx) \
    do { \
        swap = htonl(val); \
        memcpy(&(buf[idx]), &(swap), sizeof(uint32_t)); \
        idx += sizeof(uint32_t); \
    } while(0)

    /* Store opcode and argcount */
    STORE_U32(cmd->opcode, write_buf, swap, write_cnt);
    STORE_U32(cmd->argcount, write_buf, swap, write_cnt);

    /* Each argument from command struct, unless as string */

    if (RPC_OP_USE_STRING(cmd->opcode)) {
        if (cmd->argcount > (sizeof(uint32_t) * (MAX_RPC_ARGS - 1))) {
            cli_out("RPC string too long (send0: %d\n", cmd->argcount);
            goto fail;
        }
        /* Arg 0 is address */
        STORE_U32(cmd->args[0], write_buf, swap, write_cnt);
        /* Here, arg count is a number of bytes */
        memcpy(&write_buf[write_cnt], (uint8_t *) &cmd->args[1],
               cmd->argcount);
        write_cnt += cmd->argcount;
    } else {
        if (cmd->argcount > MAX_RPC_ARGS) {
            cli_out("RPC: too many arguments. %d\n", cmd->argcount);
            goto fail;
        }
        for (i = 0; i < cmd->argcount; i++) {
            STORE_U32(cmd->args[i], write_buf, swap, write_cnt);
        }
    }

    /* Store status */
    STORE_U32(cmd->status, write_buf, swap, write_cnt);

#undef STORE_U32

    /* Perform write */
    if (writen(sockfd, write_buf, write_cnt) < 0) {
        cli_out("write_command: could not write RPC request.\n");
        goto fail;
    }

    goto done;

    /* Failure */
 fail:
    return RPC_FAIL;

    /* Success */
 done:
    return RPC_OK;
}

/*
 * Creates an RPC command.
 * Generic routine used by all RPC command stubs below.
 * NOTE: this should be used the same way by all stubs since the
 * ordering of arguments is significant.
 */

void
make_rpc(rpc_cmd_t *cmd, uint32_t opcode,
         uint32_t status, uint32_t argcount, ...)
{
    uint32_t i;
    va_list ap;

    cmd->opcode = opcode;
    cmd->argcount = argcount;
    cmd->status = status;
    va_start(ap, argcount);
    for (i = 0; i < argcount; i++) {
        cmd->args[i] = va_arg(ap, uint32_t);
    }
    va_end(ap);
}


/*
 * Creates an RPC command with data as a string.
 */

void
make_rpc_string(rpc_cmd_t *cmd, uint32_t opcode, uint32_t status,
                uint32_t addr, uint8_t *data, uint32_t len)
{
    cmd->opcode = opcode;
    cmd->argcount = len;
    cmd->status = status;
    cmd->args[0] = addr;
    if (data) {
        memcpy((uint8_t *)&cmd->args[1], data, len);
    }
}

/*
 * Makes an RPC structure to be sent to the server for the client (request);
 * This is for the setmem(addr,data) call.
 */
void
make_rpc_setmem_req(rpc_cmd_t *cmd, uint32_t addr, uint32_t data)
{
    make_rpc(cmd,
             RPC_DMAC_WRITE_REQ,
             RPC_OUTSTANDING,
             2,
             addr,
             data);
}
/*
 * Makes an RPC structure to be sent back to the client in response.
 * This is for the response to the client for setmem(addr,data).
 * Here we send the updated value of the data @ addr.
 */
void
make_rpc_setmem_resp(rpc_cmd_t *cmd, uint32_t data)
{
    make_rpc(cmd,
             RPC_DMAC_WRITE_RESP,
             RPC_OK,
             1,
             data);
}


/*
 * Makes an RPC structure to be sent to the server for the client (request);
 * this is for the getmem(addr) call.
 */
void
make_rpc_getmem_req(rpc_cmd_t *cmd, uint32_t addr)
{
    make_rpc(cmd,
             RPC_DMAC_READ_REQ,
             RPC_OUTSTANDING,
             1,
             addr);
}

/*
 * Makes an RPC response to send back to the client. This is for
 * a response to getmem(addr).
 */
void
make_rpc_getmem_resp(rpc_cmd_t *cmd, uint32_t data)
{
    make_rpc(cmd,
             RPC_DMAC_READ_RESP,
             RPC_OK,
             1,
             data);
}



/*
 * Makes an RPC structure to be sent to the server for the client (request);
 * This is for the pli_setreg() call.
 */
void
make_rpc_setreg_req(rpc_cmd_t *cmd,
                    uint32_t regtype, uint32_t regnum, uint32_t regval)
{
    make_rpc(cmd,
             RPC_SETREG_REQ,
             RPC_OUTSTANDING,
             3,
             regnum,
             regval,
             regtype);
}

/*
 * Makes an RPC structure to be sent to the client from the server
 * (response); this is for the response to the pli_setreg() call.
 */

void
make_rpc_setreg_resp(rpc_cmd_t *cmd, uint32_t regval)
{
    make_rpc(cmd,
             RPC_SETREG_RESP,
             RPC_OK,
             1,
             regval);
}


/*
 * Makes an RPC structure to be sent to the server for the client (request);
 * this is for the pli_getreg() call.
 */
void
make_rpc_getreg_req(rpc_cmd_t *cmd, uint32_t regtype, uint32_t regnum)
{
    make_rpc(cmd,
             RPC_GETREG_REQ,
             RPC_OUTSTANDING,
             2,
             regnum,
             regtype);
}


/*
 * Makes an RPC structure to be sent to the client from the server
 * (response); this is for the response to the pli_getreg() call.
 * NOTE: buf must be 32 byte in size.
 */
void
make_rpc_getreg_resp(rpc_cmd_t *cmd, uint32_t status,
                     uint32_t regval, char *buf32byte)
{
    uint32_t    w[8];

    /* Kludge: shove a 32 byte buffer into some integers.*/

    memcpy((char *) w, buf32byte, 32);  /* Get aligned on word boundary */

    make_rpc(cmd,
             RPC_GETREG_RESP,
             RPC_OK,
             10,
             status,
             regval,
             w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7]);
}



/*
 * Makes an RPC structure to be sent to the server for the client (request);
 * this is for the pli_interrupt() call.
 */
void
make_rpc_intr_req(rpc_cmd_t *cmd, uint32_t cause)
{
    make_rpc(cmd,
             RPC_SEND_INTR,
             RPC_OUTSTANDING,
             1,
             cause);
}

/*
 * Makes an RPC structure to be sent to the server for the client (request);
 * this is for the response to the pli_interrupt() call.
 */
void
make_rpc_intr_resp(rpc_cmd_t *cmd, uint32_t cause)
{
    make_rpc(cmd,
             RPC_SEND_INTR,
             RPC_OK,
             1,
             cause);
}


/*
 * Makes an RPC disconnect request structure.
 */
void
make_rpc_disconnect_req(rpc_cmd_t *cmd)
{
    make_rpc(cmd,
             RPC_DISCONNECT,
             RPC_OUTSTANDING,
             0,0,0);
}

/*
 * Makes an RPC registration request. This call will register the
 * client to receive interrupts on one port, and dma requests (r/w) on
 * another. The net_addr parameter is an IPv4 address.
 */
void
make_rpc_register_req(rpc_cmd_t *cmd,
                      int int_port, int dmac_port, uint32_t net_addr)
{
    make_rpc(cmd,
             RPC_REGISTER_CLIENT,
             RPC_OUTSTANDING,
             3,
             int_port,
             dmac_port,
             htonl(net_addr));
}

/*
 * Same as make_rpc_register_req, but the net_addr parameter is an IPv6
 * address.
 */
void
make_rpc_register_ip6_req(rpc_cmd_t *cmd,
                          int int_port, int dmac_port, uint32_t *net_addr)
{
    make_rpc(cmd,
             RPC_REGISTER_CLIENT,
             RPC_OUTSTANDING,
             6,
             int_port,
             dmac_port,
             htonl(net_addr[0]),
             htonl(net_addr[1]),
             htonl(net_addr[2]),
             htonl(net_addr[3]));
}

/*
 * RPC registration response: send RPC_OK on success.
 */
void
make_rpc_register_resp(rpc_cmd_t *cmd,
                       uint32_t status)
{
    make_rpc(cmd,
             RPC_REGISTER_CLIENT,
             RPC_OK,
             1,
             status);
}


int
pli_client_count(void)
{
    char *s;

    /* Returns number of PLI clients expected to be running. */
    /* This should be set in the environment variable SOC_TARGET_COUNT. */

    s = getenv("SOC_TARGET_COUNT");

    return (s == NULL) ? 1 : atoi(s);
}

/*
 * Generic network routines for client and server.
 */

/*
 * Connect a client to the server.
 * This performs the following PLI client-side initialization.
 *
 * 1) Initialize a lock for PLI setreg/getreg accesses
 * 2) Input connection host/port information from user.
 * 3) Setup DMA controller task to listen (locally) on dma_port.
 * 4) Setup Interrupt Controller task to listen (locally) on int_port.
 * 5) Register an interrupt callback routine (passed in).
 * 6) Setup a local socket file descriptor to communicate with
 *    pli_host on the given pli_port.
 * 7) Register the client with the server on pli_host.
 *    The server may immediately start sending DMA and INTR requests.
 * Return:
 *    0 on success, a negative value on error.
 */
int
pli_client_attach(int dev_no)
{
    struct addrinfo *res = NULL, *rp;
    struct addrinfo hints;
    verinet_t *v = &verinet[dev_no];
    ipaddr_t cli_addr;
    socklen_t cli_addrlen;
    rpc_cmd_t cmd;
    char tmp[80], *hstr, *pstr;
    int rv = -1;

    if (v->intr_dispatch) {
        /*
         * Wait for threads to exit in case attach is called
         * immediately after detach.
         */
        sleep(1);
    }

    memset(v, 0, sizeof (*v));

    v->dev_no = dev_no;

    v->sockfd = -1;
    v->skip_test_int = 1;

    v->pli_mutex = sal_mutex_create("PLI socket mutex");

    /* Setup target host */
    hstr = getenv("SOC_TARGET_SERVER");
    if (!hstr) {
        snprintf(tmp, sizeof(tmp), "SOC_TARGET_SERVER%d", dev_no);
        hstr = getenv(tmp);
    }
    if (!hstr) {
        strcpy(v->pli_host, "localhost");
    } else {
        if (strlen(hstr) >= sizeof(v->pli_host)) {
            cli_out("pli_client_attach: Too long environment variable "
                    "SOC_TARGET_SERVER\n");
            goto error;
        }
        strcpy(v->pli_host, hstr);
    }

    /* Setup target port */
    pstr = getenv("SOC_TARGET_PORT");
    if (!pstr) {
        snprintf(tmp, sizeof(tmp), "SOC_TARGET_PORT%d", dev_no);
        pstr = getenv(tmp);
    }
    if (!pstr) {
        cli_out("pli_client_attach: Missing environment variable "
                "SOC_TARGET_PORT\n");
        goto error;
    }
    else {
        v->pli_port = atoi(pstr);
    }

    /* Connect to host running simulator */
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    if (getaddrinfo(v->pli_host, pstr, &hints, &res) != 0) {
        cli_out("pli_client_attach: hostname lookup failed "
                "for host [%s] (%s)\n",
                v->pli_host, strerror(errno));
        goto error;
    }

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        v->sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (v->sockfd < 0) {
            continue;
        }
        if (connect(v->sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            /* Successfully connected */
            break;
        }
        close(v->sockfd);
        v->sockfd = -1;
    }

    if (rp == NULL) {
        cli_out("client_attach: could not connect unit %d to [%s.%d] (%s)\n",
                dev_no, v->pli_host, v->pli_port, strerror(errno));
        goto error;
    }

    /* Get local host name to send back to server */
    cli_addrlen = sizeof(cli_addr);
    if (getsockname(v->sockfd, &cli_addr.sa, &cli_addrlen) < 0) {
        cli_out("client_attach: getsockname failed (%s)\n",
                strerror(errno));
        goto error;
    }

    /* Start INTC and DMAC tasks and register callback for isr */
    if (!v->jobs_started) {
        if (dmac_init(v) < 0) {
            goto error;
        }

        if (intr_init(v) < 0) {
            goto error;
        }

        v->jobs_started = 1;
    }

    /* Send REGISTER request to PLI server */
    if (cli_addr.sin6.sin6_family == AF_INET6) {
        make_rpc_register_ip6_req(&cmd, v->int_port, v->dma_port,
                                  (uint32_t *)&cli_addr.sin6.sin6_addr);
    } else {
        make_rpc_register_req(&cmd, v->int_port, v->dma_port,
                              (uint32_t)cli_addr.sin.sin_addr.s_addr);
    }

    if (write_command(v->sockfd, &cmd) != RPC_OK) {
        cli_out("pli_client_attach: could not send REGISTER request\n");
        goto error;
    }

    if (wait_command(v->sockfd, &cmd) < 0) {
        goto error;
    }

    if (cmd.opcode != RPC_REGISTER_CLIENT ||
        cmd.status != RPC_OK) {
        cli_out("pli_client_attach: PLI registration failed\n");
        goto error;
    }

    /* No longer needed */
    freeaddrinfo(res);

    return 0;

 error:
    if (res) {
        freeaddrinfo(res);
    }

    if (v->sockfd >= 0) {
        close(v->sockfd);
        v->sockfd = -1;
    }

    return rv;
}

int
pli_register_isr(int dev_no, pli_isr_t isr, void *isr_data)
{
    intr_set_vector(&verinet[dev_no], isr, isr_data);
    return 0;
}

int
pli_client_detach(int dev_no)
{
    verinet_t *v = &verinet[dev_no];

    if (v->sockfd >= 0) {
        disconnect(v->sockfd);
        close(v->sockfd);
        v->sockfd = -1;
    }

    return 0;
}

/*
 * Disconnect a client from the server.
 */
void
disconnect(int sockfd)
{
    rpc_cmd_t command;
    make_rpc_disconnect_req(&command);
    cli_out("Disconnect: 0x%x\n", sockfd);
    write_command(sockfd, &command);
}


/*
 * Send an interrupt to a client. "cause" indicates the reason.
 */
uint32_t
send_interrupt(int sockfd, uint32_t cause)
{
    rpc_cmd_t command;
    make_rpc_intr_req(&command,cause);
    write_command(sockfd, &command);
    if (wait_command(sockfd, &command) < 0) {
        return -1;
    }
    return command.args[0];
}




/*
 * Register a client with the server. This will add the client to
 * the servers queue of service requests. We pass the network address
 * and two ports which are used for DMA r/w requests and interrupt
 * requests.
 */
netconnection_t *
register_client(int sockfd, uint32_t int_port, uint32_t dma_port,
                int net_addrlen, uint32_t *net_addr)
{
    char buf[64];
    int dma_sockfd = -1; /* DMA socket file descriptor */
    int int_sockfd = -1; /* Interrupt file descriptor */
    socklen_t addrlen = 0;
    ipaddr_t cli_addr; /* Client net address */
    void *addr;
    netconnection_t* nc = (netconnection_t*)malloc(sizeof(netconnection_t));

    nc->intsock = -1;
    nc->dmasock = -1;
    nc->piosock = -1;

    /* Check client IP netaddr from RPC message */
    addr = &cli_addr.sin.sin_addr;
    cli_addr.sin.sin_family = AF_INET;
    if (net_addrlen == 4) {
        cli_addr.sin6.sin6_family = AF_INET6;
        addr = &cli_addr.sin6.sin6_addr;
    }
    memcpy(addr, net_addr, 4 * net_addrlen);
    LOG_VERBOSE(BSL_LS_SYS_VERINET,
                (BSL_META("server: RPC client address is %s\n"),
                 inet_str(&cli_addr, buf, sizeof(buf))));

    /* Get peer IP address */
    addrlen = sizeof(cli_addr);
    if (getpeername(sockfd, &cli_addr.sa, &addrlen) != 0) {
        cli_out("server: can't get peer address (%s)\n",
                strerror(errno));
        free(nc);
        return (netconnection_t*)0x0;
    }

    /* Keep local copy of net_addr */
    memcpy(&nc->inetaddr, &cli_addr, addrlen);

    /* Setup interrupt socket */
    if (cli_addr.sa.sa_family == AF_INET6) {
        cli_addr.sin6.sin6_port = htons(int_port);
    } else {
        cli_addr.sin.sin_port = htons(int_port);
    }
    if ((int_sockfd = socket(cli_addr.sa.sa_family, SOCK_STREAM, 0)) < 0) {
        cli_out("server: can't open stream socket (%s)\n",
                strerror(errno));
        free(nc);
        return (netconnection_t*)0x0;
    }
    /* Connect back to client */
    if (connect(int_sockfd, &cli_addr.sa, addrlen) < 0) {
        cli_out("server: INTC connect failed to [%s.%d] (%s)\n",
                inet_str(&nc->inetaddr, buf, sizeof(buf)),
                int_port, strerror(errno));
        close(int_sockfd);
        free(nc);
        return (netconnection_t*)0x0;
    }

    /* Setup DMA socket */
    if (cli_addr.sa.sa_family == AF_INET6) {
        cli_addr.sin6.sin6_port = htons(dma_port);
    } else {
        cli_addr.sin.sin_port = htons(dma_port);
    }
    if ((dma_sockfd = socket(cli_addr.sa.sa_family, SOCK_STREAM, 0)) < 0) {
        cli_out("server: can't open stream socket (%s)\n",
                strerror(errno));
        close(int_sockfd);
        free(nc);
        return (netconnection_t*)0x0;
    }

    /* Connect back to client */
    if (connect(dma_sockfd, &cli_addr.sa, addrlen) < 0) {
        cli_out("server: DMAC connect failed to [%s.%d] (%s)\n",
                inet_str(&nc->inetaddr, buf, sizeof(buf)),
                dma_port, strerror(errno));
        close(int_sockfd);
        close(dma_sockfd);
        free(nc);
        return (netconnection_t*)0x0;
    }

    /* Fill in fields */
    nc->intsock = int_sockfd ;
    nc->dmasock = dma_sockfd ;
    nc->piosock = sockfd;

    return nc;
}

int
unregister_client(netconnection_t *nc)
{
    disconnect(nc->intsock);
    close(nc->intsock);
    disconnect(nc->dmasock);
    close(nc->dmasock);
    close(nc->piosock);
    free(nc);
    return 0;
}

/*
 * Server PLI-level call to write to DMA memory.
 * Load the given memory address with some data on the client.
 */
uint32_t
dma_writemem(int sockfd, uint32_t addr, uint32_t data)
{
    rpc_cmd_t cmd;
    make_rpc_setmem_req(&cmd, addr, data);
    write_command(sockfd, &cmd);
    if (wait_command(sockfd, &cmd) < 0) {
        return -1;
    }
    return cmd.args[0];
}


/*
 * Server PLI-level client call to read DMA memory.
 * Returns the data at a given memory address on client.
 */
uint32_t
dma_readmem(int sockfd, uint32_t addr)
{
    rpc_cmd_t cmd;
    make_rpc_getmem_req(&cmd, addr);
    write_command(sockfd, &cmd);
    if (wait_command(sockfd, &cmd) < 0) {
        return -1;
    }
    return cmd.args[0];
}

/*
 * Server PLI-level call to write to DMA memory as a string.
 */
uint32_t
dma_write_bytes(int sockfd, uint32_t addr, uint8_t *data, int len)
{
    rpc_cmd_t cmd;

    make_rpc_string(&cmd, RPC_DMAC_WRITE_BYTES_REQ, RPC_OUTSTANDING,
                    addr, data, len);
    write_command(sockfd, &cmd);
    if (wait_command(sockfd, &cmd) < 0) {
        return -1;
    }

    return cmd.argcount;
}


/*
 * Server PLI-level client call to read DMA memory as a string.
 * Data is copied up to the max number of bytes to *data.
 */
uint32_t
dma_read_bytes(int sockfd, uint32_t addr, uint8_t *data, int len)
{
    rpc_cmd_t cmd;

    make_rpc_string(&cmd, RPC_DMAC_READ_BYTES_REQ, RPC_OUTSTANDING,
                    addr, NULL, len);
    write_command(sockfd, &cmd);
    if (wait_command(sockfd, &cmd) < 0) {
        return -1;
    }

    memcpy(data, &cmd.args[1], cmd.argcount);

    return cmd.argcount;
}


/*
 * Client Call: get a register on the simulator.
 */
uint32_t
pli_getreg(int dev_no, uint32_t regtype, uint32_t regnum)
{
    int i;
    char buf[32], *ptr;
    verinet_t *v = &verinet[dev_no];
    uint32_t status = 0x0;
    uint32_t regval = 0x0;
    rpc_cmd_t response;
    rpc_cmd_t command;

    LOG_VERBOSE(BSL_LS_SYS_PCI,
                (BSL_META("pli_getreg: regtype=0x%x regnum=0x%x\n"),
                 regtype, regnum));

    make_rpc_getreg_req(&command, regtype,regnum);

    sal_mutex_take(v->pli_mutex, SAL_MUTEX_FOREVER);

    if (write_command(v->sockfd, &command) != RPC_OK) {
        cli_out("Error: pli_getreg write_command failed\n");
        sal_mutex_give(v->pli_mutex);
        return -1;
    }

    if (wait_command(v->sockfd, &response) < 0) {
        sal_mutex_give(v->pli_mutex);
        return -1;
    }

    sal_mutex_give(v->pli_mutex);

    status = response.args[0];
    regval = response.args[1];
    ptr = (char*)&command.args[2];
    for (i = 0; i < 32; i++) {
        buf[i] = *ptr++;
    }

    LOG_VERBOSE(BSL_LS_SYS_PCI,
                (BSL_META("pli_getreg: regval=0x%x diagbuf=%s\n"),
                 regval, buf));

    if (status != 0) {
        /* RPC failed */
        return -1;
    }
    /* Read succeeded */
    return regval;
}

/*
 * Client call: set a register on the simulator.
 */
uint32_t
pli_setreg(int dev_no, uint32_t regtype, uint32_t regnum, uint32_t regval)
{
    rpc_cmd_t command;
    verinet_t *v = &verinet[dev_no];

    make_rpc_setreg_req(&command,regtype,regnum,regval);

    LOG_VERBOSE(BSL_LS_SYS_PCI,
                (BSL_META("pli_setreg: regtype=0x%x regnum=0x%x regval=0x%x\n"),
                 regtype, regnum, regval));

    sal_mutex_take(v->pli_mutex, SAL_MUTEX_FOREVER);

    if (write_command(v->sockfd, &command) != RPC_OK) {
        cli_out("Error: pli_setreg write_command failed\n");
        sal_mutex_give(v->pli_mutex);
        return -1;
    }

    if (wait_command(v->sockfd,&command) < 0) {
        sal_mutex_give(v->pli_mutex);
        return -1;
    }

    sal_mutex_give(v->pli_mutex);

    return command.args[0];
}

/*
 * Client call: send packet to be transmitted into the switch
 */
uint32_t
pli_send_packet(int dev_no, uint32_t src_port, uint32_t count,
                int len, unsigned char *buf)
{
    verinet_t *v = &verinet[dev_no];
    rpc_cmd_t command;

    make_rpc(&command,RPC_SEND_PACKET,RPC_OK,3,src_port,count,len);

    if (write_command(v->sockfd, &command) != RPC_OK) {
        cli_out("Error: pli_send_packet cmd failed\n");
        return -1;
    }
    if (writen(v->sockfd,buf,len) != len) {
        cli_out("Error: pli_send_packet data failed\n");
        return -1;
    }
    return 0;
}

/*
 * Client call: enable port transmission
 */
uint32_t
pli_tx_enable(int dev_no, uint32_t portmap)
{
    verinet_t *v = &verinet[dev_no];
    rpc_cmd_t command;

    make_rpc(&command,RPC_TX_ENABLE,RPC_OK,1,portmap);

    if (write_command(v->sockfd, &command) != RPC_OK) {
        cli_out("Error: pli_tx_enable cmd failed\n");
        return -1;
    }
    return 0;
}

/*
 * Client call: tell testbench to shutdown
 */
uint32_t
pli_shutdown(int dev_no)
{
    verinet_t *v = &verinet[dev_no];
    rpc_cmd_t command;

    make_rpc(&command,RPC_SHUTDOWN,RPC_OK,0);

    if (write_command(v->sockfd, &command) != RPC_OK) {
        cli_out("Error: pli_shutdown cmd failed\n");
        return -1;
    }
    return 0;
}


/*
 * Send signal to simulation to activate
 */
uint32_t
pli_sim_start(int dev_no)
{
    verinet_t *v = &verinet[dev_no];
    rpc_cmd_t command;

    make_rpc(&command, RPC_IOCTL_REQ, RPC_OK, 3, BCM_SIM_ACTIVATE, 0, 0);

    if (write_command(v->sockfd, &command) != RPC_OK) {
        cli_out("Error: pli_sim_start cmd failed\n");
        return -1;
    }
    return 0;
}

int
get_pkt_data(int sockfd, struct timeval *tv, uint32_t *data)
{
    int rv;
    int i, len;
    fd_set read_vect;
    uint32_t swap;

    LOG_DEBUG(BSL_LS_SYS_VERINET,
              (BSL_META("get_pkt_data: sockfd=%d\n"),
               sockfd));

    if (sockfd <= 2) {
        /* Verinet not initialized? */
        return -1;
    }

    /* Setup bitmap for read notification ... */
    FD_ZERO(&read_vect);
    FD_SET(sockfd, &read_vect);
    do {
        rv = select(sockfd+1, &read_vect, (fd_set*)0x0, (fd_set*)0x0, tv);
    } while (rv < 0 && errno == EINTR);
    if (rv < 0) {
        cli_out("get_command: select error (%s)\n",
                strerror(errno));
        goto fail;
    }
    /*
     * Data ready to be read.
     */
    if (FD_ISSET(sockfd, &read_vect)) {
        /*
         * Read RPC opcode.
         */
        if (readn(sockfd, &swap, sizeof(uint32_t)) < (int)sizeof(uint32_t)) {
            goto fail;
        }
        /* Convert precious info to host byte order */
        i=0;
        len = ntohl(swap);
        data[i]=len;
        while (len > 0) {
            if (readn(sockfd, &swap, sizeof(uint32_t)) < (int)sizeof(uint32_t)) {
                cli_out("get_pkt_data: could not read DATA.\n");
                goto fail;
            }
            i++;
            data[i]=ntohl(swap);
            len-=4;
        }

        return 1;
    }

 fail:
    return -1;
}

/*
 * Perform an SCHANNEL operation.
 */
int
pli_schan(int dev_no, uint32_t *words, int dw_write, int dw_read)
{
    int i ;
    verinet_t *v = &verinet[dev_no];
    uint32_t status = 0x0;
    rpc_cmd_t command;

    make_rpc(&command, RPC_SCHAN_REQ, RPC_OUTSTANDING, 0);

    command.argcount = dw_write+2;
    command.args[0] = dw_write;
    command.args[1] = dw_read;

    for (i = 0; i < dw_write; i++) {
        command.args[i+2] = words[i];
    }

    sal_mutex_take(v->pli_mutex, SAL_MUTEX_FOREVER);

    if (write_command(v->sockfd, &command) != RPC_OK) {
        cli_out("Error: pli_schan write_command failed\n");
        sal_mutex_give(v->pli_mutex);
        return -1;
    }

    if (wait_command(v->sockfd, &command) < 0) {
        sal_mutex_give(v->pli_mutex);
        return -1;
    }

    sal_mutex_give(v->pli_mutex);

    /* S-channel operation response is the first argument */
    status = command.args[0];

    for (i = 0; i < dw_read; i++) {
        words[i] = command.args[i+1];
    }

    if (status != 0) {
        /* RPC failed */
        return -1;
    }
    /* Read succeeded */
    return 0;
}
