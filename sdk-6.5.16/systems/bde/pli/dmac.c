/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * A simple DMA controller "server". This code responds to a DMA
 * request to read or write a word of data into shared memory.
 *
 * The DMA controller is a simple task (Solaris Thread) which handles
 * two types of requests:
 *
 * RPC_DMAC_READ: read data from memory
 * RPC_DMAC_WRITE: write data to memory address
 */

#include <shared/bsl.h>

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>

#ifdef VXWORKS
#include <vxWorks.h>
#include <sockLib.h>
#endif

#include <sal/appl/io.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <sal/appl/pci.h>
#include <soc/debug.h>	

#include "verinet.h"

/*
 * DMA memory routines.
 * The routines below operate on memory which the CMIC onboard the
 * Orion will read and write. Under the PLI simulator, these routines
 * read and write shared memory.
 */

#define	MAX_DMA_MEMORY	(64*1024)
static	uint32	kluge_dma_memory[MAX_DMA_MEMORY/sizeof(uint32)];

static int
_dma_putw(uint32 addr, uint32 data)
{
    sal_vaddr_t dma_addr = addr;

    if (addr < MAX_DMA_MEMORY) {
        dma_addr += PTR_TO_UINTPTR(kluge_dma_memory);
    }

    *((uint32 *)dma_addr) = data;

    return(0);
}

static int
_dma_putb(uint32 addr, uint8 data)
{
    sal_vaddr_t dma_addr = addr;

    if (addr < MAX_DMA_MEMORY) {
        dma_addr += PTR_TO_UINTPTR(kluge_dma_memory);
    }

    *((uint8 *)dma_addr) = data;

    return(0);
}

static int
_dma_getw(uint32 addr)
{
    sal_vaddr_t dma_addr = addr;

    if (addr < MAX_DMA_MEMORY) {
        dma_addr += PTR_TO_UINTPTR(kluge_dma_memory);
    }

    return *((uint32 *)dma_addr);

}

static int
_dma_getb(uint32 addr)
{
    sal_vaddr_t dma_addr = addr;

    if (addr < MAX_DMA_MEMORY) {
        dma_addr += PTR_TO_UINTPTR(kluge_dma_memory);
    }

    return *((uint8 *)dma_addr);
}

/*
 * Main thread of control. This worker thread will service requests
 * on a socket, read or writing to memory based on the command
 * dispatched from the main execution thread.
 */ 
void
dmac_handler(void *v_void)
{
    verinet_t *v = (verinet_t *)v_void;
    volatile int finished = 0;
    volatile uint32 dmaAddr, dmaData;
    rpc_cmd_t cmd;
    int sockfd = v->dmacFd;
    uint8 *byte_ptr;
    uint32 i;

    while (!finished){    
        LOG_INFO(BSL_LS_SYS_VERINET,
                 (BSL_META("dmac_handler: wait...\n")));

	if (wait_command(sockfd, &cmd) < 0) {
	    break;	/* Error message already printed */
	}

        LOG_INFO(BSL_LS_SYS_VERINET,
                 (BSL_META("request opcode 0x%x\n"),
                  cmd.opcode));

	switch(cmd.opcode){
	case RPC_DMAC_READ_REQ:
	    /* Read DMA memory ... */
	    dmaAddr = cmd.args[0];
	    /* Read data from shared memory at Addr */
	    dmaData = _dma_getw(dmaAddr);
            LOG_INFO(BSL_LS_SYS_VERINET,
                     (BSL_META("**DMA_RD: 0x%x = 0x%x\n"),
                      dmaAddr, dmaData));
	    /* Send back a response */
	    make_rpc_getmem_resp(&cmd, dmaData);
	    write_command(sockfd, &cmd);
	    break;
      
	case RPC_DMAC_WRITE_REQ:
	    /* Write DMA memory ... */
	    dmaAddr = cmd.args[0]; /* Address */
	    dmaData = cmd.args[1]; /* Data */
	    /* Write the data to shared memory */
	    _dma_putw(dmaAddr, dmaData);
            LOG_INFO(BSL_LS_SYS_VERINET,
                     (BSL_META("**DMA_WR: 0x%x = 0x%x\n"),
                      dmaAddr, dmaData));
	    /* Send back a response */
	    make_rpc_setmem_resp(&cmd,dmaData);
	    write_command(sockfd, &cmd);
	    break;

	case RPC_DMAC_READ_BYTES_REQ:
	    /* Read DMA memory as a string.  Would be nice to use memcpy */
	    dmaAddr = cmd.args[0];
            byte_ptr = (uint8 *)&cmd.args[1];
            LOG_INFO(BSL_LS_SYS_VERINET,
                     (BSL_META("**DMA_RD_B: 0x%x, %d bytes\n"),
                      dmaAddr, cmd.argcount));
            /* coverity[tainted_data] */
            for (i = 0; i < cmd.argcount; i++) {
                byte_ptr[i] = _dma_getb(dmaAddr++);
            }
	    /* Send back a response */
            cmd.status = RPC_OK;
            cmd.opcode = RPC_DMAC_READ_BYTES_RESP;
	    write_command(sockfd, &cmd);
	    break;

	case RPC_DMAC_WRITE_BYTES_REQ:
	    /* Write DMA memory as a string.  Would be nice to use memcpy */
	    dmaAddr = cmd.args[0]; /* Address */
	    byte_ptr = (uint8 *)&cmd.args[1]; /* Data */
	    /* Write the data to shared memory */
            LOG_INFO(BSL_LS_SYS_VERINET,
                     (BSL_META("**DMA_WR_B: 0x%x, %d bytes\n"),
                      dmaAddr, cmd.argcount));
            for (i = 0; i < cmd.argcount; i++) {
                _dma_putb(dmaAddr++, byte_ptr[i]);
            }
	    /* Send back a response */
            cmd.status = RPC_OK;
            cmd.opcode = RPC_DMAC_WRITE_BYTES_RESP;
	    write_command(sockfd, &cmd);
	    break;

	case RPC_DISCONNECT:
	    finished = 1;
	    v->dmacWorkerExit++;
            LOG_INFO(BSL_LS_SYS_VERINET,
                     (BSL_META("received disconnect\n")));
	    break;
      
	default:
	    /* Unknown opcode */
	    break;
	}
    }
    LOG_INFO(BSL_LS_SYS_VERINET,
             (BSL_META("shutdown and tread existing\n")));
    close(sockfd);
    sal_thread_exit(0);
}


/*
 * main: Sets up the socket and handles client requests with a child 
 * process per socket.                                       
 */
void
dmac_listener(void *v_void)
{
    verinet_t *v = (verinet_t *)v_void;
    int sockfd, newsockfd, one = 1;
    socklen_t clilen;
    struct sockaddr_in cli_addr, serv_addr;

    /* Initialize socket ... */

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	perror("server: can't open stream socket");
	exit(1);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
		   (char *) &one, sizeof (one)) < 0) {
	perror("setsockopt");
    }

    /*
     * Setup server address...
     */
    memset((void *) &serv_addr,0x0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(0);	/* Pick any port */

    /*
     * Bind our local address
     */
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
	perror("dmac: unable to bind address");
	sal_thread_exit(0);
    }

    v->dmaPort = getsockport(sockfd);	/* Get port that was picked */

    /* listen for inbound connections ... */
    listen(sockfd, 5);
  
    /* Notify dmac_init that socket is listening */
    sal_sem_give(v->dmacListening);

    cli_out("DMA Controller listening on port[%d]\n", v->dmaPort);

    while (!v->dmacWorkerExit) { 
	clilen = sizeof(cli_addr);

	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0 && errno == EINTR) {
	    continue;
	}

	if (newsockfd < 0) {
	    perror("server: accept error");
	} else {
	    v->dmacFd = newsockfd;
	    v->dmacHandler = sal_thread_create("DMA-controller",
						SAL_THREAD_STKSZ, 100,
						dmac_handler, v);

	    if (SAL_THREAD_ERROR == v->dmacHandler) {
		cli_out("Thread creation error!\n");
	    } else {
                LOG_INFO(BSL_LS_SYS_VERINET,
                         (BSL_META("DMAC request thread dispatched.\n")));
	    }
	}
    }
    LOG_INFO(BSL_LS_SYS_VERINET,
             (BSL_META("DMA listener shutdown.\n")));
    close(sockfd);
    sal_thread_exit(0);
}

int
dmac_init(verinet_t *v)
{
    if (v->dmacInited) {
	return 0;
    }

    v->dmacListening =
	sal_sem_create("dmac listener", sal_sem_BINARY, 0);

    cli_out("Starting DMA service...\n");

    v->dmacListener = sal_thread_create("DMA-listener",
					SAL_THREAD_STKSZ, 100,
					dmac_listener, v);
    if (SAL_THREAD_ERROR == v->dmacListener) {
	cli_out("ERROR: could not create DMAC task: %s!\n", strerror(errno));
	return -2;
    }

    sal_sem_take(v->dmacListening, sal_sem_FOREVER);  /* Wait for listen() */
    sal_sem_destroy(v->dmacListening);

    v->dmacInited = 1;

    return 0;
}
