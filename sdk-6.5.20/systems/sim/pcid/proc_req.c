/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        proc_req.c
 * Purpose:
 * Requires:    
 */


#include <shared/bsl.h>

#include <unistd.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <soc/mem.h>
#include <soc/hash.h>

#include <soc/cmic.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/schanmsg.h>
#include <sal/appl/io.h>
#include <bde/pli/verinet.h>

#include "pcid.h"
#include "mem.h"
#include "cmicsim.h"
#include "dma.h"
#include "pli.h"

STATIC rpc_cmd_t cmd;
/*
 * Handles an incoming client request
 * We block until the client has something to send us and then 
 * we do something over the socket descriptor.
 * Returns 1 when a command is processed, 0 on disconnect.
 */

int pcid_process_request(pcid_info_t *pcid_info, int sockfd,
                    struct timeval *tmout)
{
    char buf[64];
    uint32 regval;
    int r, unit, finished = 0;

    unit = pcid_info->unit;
    r = get_command(sockfd, tmout, &cmd);

    if (r == 0) {
    return PR_NO_REQUEST;    /* No request; return to poll later */
    }

    if (r < 0) {        /* Error; cause polling loop to exit */
    return PR_ERROR;
    }

    switch (cmd.opcode) {
    /*
     * We only handle requests: anything else is a programming
     * error. Once we are here, all parameters have been marshalled
     * and we simply need to call the correct routine (see pli_cmd.c).
     */

    case RPC_SCHAN_REQ:
        {       
            schan_msg_t msg; 
            int i; 
            int dw_write, dw_read; 
            extern int schan_op(pcid_info_t *pcid_info, int unit, schan_msg_t* data); 

            dw_write = cmd.args[0]; 
            dw_read = cmd.args[1]; 
        
            for(i = 0; i < dw_write; i++) {
                msg.dwords[i] = cmd.args[i+2]; 
            }
            PCID_MEM_LOCK(pcid_info);
            if (!(pcid_info->schan_cb) || 
                (i = (pcid_info->schan_cb)(pcid_info, unit, &msg))) {
                i = schan_op(pcid_info, unit, &msg); 
            }
            PCID_MEM_UNLOCK(pcid_info);
            make_rpc(&cmd, RPC_SCHAN_RESP, RPC_OK, 0); 
            cmd.argcount = dw_read+1; 
            cmd.args[0] = i; 
            for(i = 0; i < dw_read; i++) {
                cmd.args[i+1] = msg.dwords[i]; 
            }
            if (write_command(sockfd, &cmd) != RPC_OK) {
                pcid_info->opt_rpc_error = 1;
            }
            break; 
        }       

    case RPC_GETREG_REQ: /* PIO read of register */
    /* Get a register from our model ... */
    /* cmd.args[1] is register type (verinet.h) */

    regval = pli_getreg_service(pcid_info, unit, cmd.args[1], cmd.args[0]);

    /* coverity[secure_coding] */
    strcpy(buf, "********");
    make_rpc_getreg_resp(&cmd,
                 0,
                 regval,
                 buf);
    if (write_command(sockfd, &cmd) != RPC_OK) {
        pcid_info->opt_rpc_error = 1;
    }

    break;

    case RPC_SETREG_REQ: /* PIO write to register */
    /* Set a register on our model... */
        /* coverity[stack_use_overflow] */
    pli_setreg_service(pcid_info, unit, cmd.args[2], cmd.args[0],
                           cmd.args[1]);
    make_rpc_setreg_resp(&cmd, cmd.args[1]);
    if (write_command(sockfd, &cmd) != RPC_OK) {
        pcid_info->opt_rpc_error = 1;
    }
    break;
      
    case RPC_IOCTL_REQ: /* Change something inside the model */
        if (pcid_info->ioctl) {
            pcid_info->ioctl(pcid_info, cmd.args);
        }
    break;
       
    case RPC_SHUTDOWN: /* Change something inside the model */
        if (pcid_info->ioctl) {
            cmd.args[0] = BCM_SIM_DEACTIVATE;
            pcid_info->ioctl(pcid_info, cmd.args);
        }
    break;
       
    case RPC_REGISTER_CLIENT:
    /* Setup port for DMA r/w requests to client and interrupts ...*/
    /* Connect to client interrupt and DMA ports and test*/
        LOG_CLI((BSL_META_U(unit,
                            "Received register client request\n")));
    regval = RPC_OK;

    pcid_info->client = register_client(sockfd,
                                        cmd.args[0],
                                        cmd.args[1],
                                        cmd.argcount - 2,
                                        &cmd.args[2]);

    if (pcid_info->client == 0) {
        finished = 1;
        break;
    }

    /*
     * TEST: Write to DMA address 0 on client.
     */
    r = dma_writemem(pcid_info->client->dmasock,
                  0,
                  0xbabeface);
      
    /*
     * TEST: Read from DMA address 0 on client.
     */
    r = dma_readmem(pcid_info->client->dmasock,0, &regval);
      
    /* TEST: Result should be the same, if not FAIL! */
    if(regval != 0xbabeface){
        LOG_CLI((BSL_META_U(unit,
                                "ERROR: regval = 0x%x\n"), regval));
        pcid_info->opt_rpc_error = 1;
    }
    
    /*
     * TEST: Send an interrupt to the client.
     */
    r = send_interrupt(pcid_info->client->intsock, 0);
        if (r < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "RPC error: soc_internal_send_int failed. \n")));
            pcid_info->opt_rpc_error = 1;
        } else {
        r = RPC_OK;
        }

    /* Send Registration status back ... */
    make_rpc_register_resp(&cmd, r);
    if (write_command(pcid_info->client->piosock, &cmd) != RPC_OK) {
        pcid_info->opt_rpc_error = 1;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Client registration: %s DMA=0x%x, INT=0x%x, PIO=0x%x -OK\n"),
                     inet_str(&pcid_info->client->inetaddr, buf, sizeof(buf)),
                     pcid_info->client->dmasock,
                     pcid_info->client->intsock,
                     pcid_info->client->piosock));

    break;

    case RPC_DISCONNECT:
    /* First disconnect the interrupt and dma sockets on client */

    unregister_client(pcid_info->client);

    finished = 1;
    break;

    default:
    /* Unknown opcode */
    break;
    }

    return (finished ? PR_ALL_DONE : PR_REQUEST_HANDLED);
}
