/*! \file intr.c
 *
 * Interrupt controller task.
 *
 * This task emulates an interrupt handler and callout mechanism.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "verinet.h"

int
intr_int_context(void)
{
    int i;

    for (i = 0; i < N_VERINET_DEVICES; i++) {
        if (sal_thread_self() == verinet[i].intr_dispatch) {
            return 1;
        }
    }

    return 0;
}

static void
intr_dispatch(void *v_void)
{
    verinet_t *v = (verinet_t *)v_void;

    while (!v->intr_finished) {
        if (sal_sem_take(v->intr_dispatch_sem, SAL_SEM_FOREVER)) {
            cli_out("intr_dispatch: sal_sem_take failed\n");
            continue;
        }

        if (v->isr) {
            (*v->isr)(v->isr_data);
        }
    }

    sal_sem_destroy(v->intr_dispatch_sem);
}

/*
 * Process an interrupt request dispatched on the socket.
 */
void
intr_handler(void *v_void)
{
    verinet_t *v = (verinet_t *)v_void;
    uint32_t cause;
    rpc_cmd_t cmd;
    int sockfd = v->intr_fd;

    v->intr_finished = 0;

    /* Start off dispatch thread */

    v->intr_dispatch_sem =
        sal_sem_create("Interrupt-dispatch", SAL_SEM_BINARY, 0);

    if (!v->intr_dispatch_sem) {
        cli_out("intr_handler: failed to create dispatch sem\n");
        v->intr_finished = 1;
    } else {
        v->intr_dispatch = sal_thread_create("Interrupt-dispatch",
                                             SAL_THREAD_STKSZ, 100,
                                             intr_dispatch, v);
        if (v->intr_dispatch == SAL_THREAD_ERROR) {
            cli_out("intr_handler: failed to create dispatch thread\n");
            sal_sem_destroy(v->intr_dispatch_sem);
            v->intr_dispatch_sem = 0;
            v->intr_finished = 1;
        }
    }

    while (!v->intr_finished) {

        if (wait_command(sockfd, &cmd) < 0) {
            break;      /* Error message already printed */
        }

        switch (cmd.opcode) {

        case RPC_SEND_INTR:
            /* Read cause */
            cause = cmd.args[0];
            make_rpc_intr_resp(&cmd,cause);
            write_command(sockfd, &cmd);

            if (v->intr_skip_test) {
                v->intr_skip_test = 0;
                break;
            }

            /* Wake up dispatch thread */

            sal_sem_give(v->intr_dispatch_sem);
            break;

        case RPC_DISCONNECT:
            v->intr_finished = 1;
            v->intr_worker_exit++;
            cli_out("Interrupt handler received disconnect\n");
            sal_sem_give(v->intr_dispatch_sem);
            break;

        default:
            /* Unknown opcode */
            break;
        }
    }
    cli_out("Interrupt service shutdown.\n");
    close(sockfd);

    /* If dispatch thread around - wait for him to exit */
    if (v->intr_dispatch) {
        sal_sem_give(v->intr_dispatch_sem);
    }
}

void
intr_set_vector(verinet_t *v, pli_isr_t isr, void *isr_data)
{
    /*
     * Set interrupt vector
     */

    v->isr = isr;
    v->isr_data = isr_data;
}

/*
 * Set up the socket and handle client requests with a child thread
 * per socket.
 */
static void
intr_listener(void *v_void)
{
    verinet_t *v = (verinet_t *)v_void;
    int sockfd, newsockfd, sockopt;
    socklen_t clilen;
    struct sockaddr_in6 cli_addr, serv_addr;

    if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        cli_out("intr: can't open stream socket (%s)\n",
                strerror(errno));
        return;
    }

    sockopt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                   (char *)&sockopt, sizeof(sockopt)) < 0) {
        cli_out("intr: setsockopt failed (%s)\n",
                strerror(errno));
    }

    sockopt = 0;
    if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY,
                   (char *)&sockopt, sizeof(sockopt)) < 0) {
        cli_out("dmac: setsockopt failed (%s)\n",
                strerror(errno));
    }

    /* Setup server address */
    memset((void *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_addr = in6addr_any;
    serv_addr.sin6_port = 0; /* Pick any port */

    /* Bind our local address */
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cli_out("intr: unable to bind address (%s)\n",
                strerror(errno));
        close(sockfd);
        return;
    }

    /* Get port that was picked */
    v->int_port = getsockport(sockfd);

    /* Listen for inbound homies ...*/
    listen(sockfd, 5);

    /* Notify intr_init that socket is listening */
    sal_sem_give(v->intr_listening);

    cli_out("ISR dispatcher listening on port[%d]\n", v->int_port);

    while (!v->intr_worker_exit) {
        clilen = sizeof(cli_addr);

        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0 && errno == EINTR) {
            continue;
        }

        if (newsockfd < 0) {
            cli_out("intr: accept error (%s)\n",
                    strerror(errno));
        } else {
            v->intr_skip_test = 1;

            v->intr_fd = newsockfd;

            v->intr_handler_thread = sal_thread_create("Interrupt-handler",
                                                       SAL_THREAD_STKSZ, 100,
                                                       intr_handler, v);

            if (v->intr_handler_thread == SAL_THREAD_ERROR) {
                cli_out("Error: could not create interrupt handler thread!\n");
            }
        }
    }
    close(sockfd);
}

int
intr_init(verinet_t *v)
{
    if (v->intr_inited) {
        return 0;
    }

    v->intr_listening = sal_sem_create("intr listener", SAL_SEM_BINARY, 0);

    /*
     * Create Interrupt task
     */
    cli_out("Starting Interrupt service...\n");

    v->intr_listener = sal_thread_create("Interrupt-listener",
                                         SAL_THREAD_STKSZ, 100,
                                         intr_listener, v);

    if (v->intr_listener == SAL_THREAD_ERROR) {
        cli_out("Error: could not create interrupt listener thread!\n");
        return -2;
    }

    sal_sem_take(v->intr_listening, SAL_SEM_FOREVER);   /* Wait for listen() */
    sal_sem_destroy(v->intr_listening);

    v->intr_inited = 1;

    return 0;
}
