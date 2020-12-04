/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *
 * File:        soc_schan_fifo.h
 * Purpose:     Maps out structures used for SCHAN FIFO ASYNC/SYNC operations and
 *              exports routines.
 */

#ifndef _SOC_SCHAN_FIFO_H
#define _SOC_SCHAN_FIFO_H

#include <soc/defs.h>
#include <soc/types.h>
#include <sal/types.h>
#include <soc/schanmsg.h>
#include <soc/soc_async.h>


/* This structure will be related to single SCHAN message
 * Following message type will be supported
 * READ_MEMORY_CMD_MSG
 * WRITE_MEMORY_CMD_MSG
 * READ_REGISTER_CMD_MSG
 * WRITE_REGISTER_CMD_MSG
 * TABLE_INSERT_CMD_MSG
 * TABLE_DELETE_CMD_MSG
 * TABLE_LOOKUP_CMD_MSG
 * FIFO_POP_CMD_MSG
 * FIFO_PUSH_CMD_MSG
 */

/* This structure supports  bunch of SCHAN meesages
 * in single transaction. In case of blocked call
 * max value size can be 2* CMIC_SCHAN_FIFO_CMD_SIZE_MAX
 * Both the channels can be used to process the message in
 * parallel. In case of non blocked call the max value of
 * size should be CMIC_SCHAN_FIFO_CMD_SIZE_MAX because
 * API can be called again to post the next message
 */

#define CMIC_SCHANFIFO_RESP_ALLOC     (32)

typedef enum {
    CTL_FIFO_START = 1,
    CTL_FIFO_STOP,
    CTL_FIFO_ABORT,
    CTL_FIFO_FLUSH,
    CTL_FIFO_MAX_MSG_GET,
    CTL_FIFO_RESP_ALLOC,
    CTL_FIFO_RESP_FREE
} schan_fifo_ctl_t;


typedef struct schan_fifo_cmd_s {
    schan_header_t header;  /*schan header */
    uint32        address;
    uint32         data[1]; /*c90 doesn't support fleaxible array e.g.  data[] */
} schan_fifo_cmd_t;

typedef struct schan_fifo_resp_s {
    schan_header_t header;
    uint32 data[CMIC_SCHANFIFO_RESP_ALLOC - 1];
} schan_fifo_resp_t;


typedef struct schan_fifo_alloc_s {
    int  num;  /* in , number of structures to be allocated */
    schan_fifo_resp_t *resp; /* out, response buffer pointer */
}schan_fifo_alloc_t;


typedef struct soc_schan_fifo_msg_s {
    int                    num;      /* number of commands */
    int                    interval; /* update interval, should be -1 for messge done only */
    size_t                 size;     /* size of the  command buffer in words */
    schan_fifo_cmd_t       *cmd;     /* packed command buffer */
    schan_fifo_resp_t      *resp;    /* Response buffer, NULL or (num*sizeof(schan_fifo_resp_t)) */
} soc_schan_fifo_msg_t;

/* This structure is used to configure the SCHAN FIFO Op */

typedef struct soc_schan_fifo_config_s {
    int intrEnb; /* 0-> polling, 1-> interrupt */
    int timeout; /* timeout(usecs) for wait complete */
    int dma;     /* 0-> No ccmDMA, 1->ccmDMA */
} soc_schan_fifo_config_t;

/*******************************************
* @function soc_schan_fifo_msg_send
* purpose cmic SCHAN FIFO operation
*
* @param unit [in] unit
* @param msg [in, out] soc_schan_fifo_msg_t
* @param cookie [in] pointer to void, user data
* @param cbf [in] soc_async_cb_f, call back function
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
 */
extern int
soc_schan_fifo_msg_send(int unit,
             soc_schan_fifo_msg_t *msg,
             void *cookie,
             soc_async_cb_f cbf);

/*******************************************
* @function soc_schan_fifo_control
* purpose API to control SCHAN FIFO
*
* @param unit [in] unit
* @param ctl [in] schan_fifo_ctl_t
* @param data [in, out] pointer to void
*
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_schan_fifo_control(int unit,
                 schan_fifo_ctl_t ctl, void *data);


/*******************************************
* @function soc_schan_fifo_deinit
* purpose API to deInitialize and config SCHAN FIFO
* It will abort the current operation, flush the queue
* and release all the resources.
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_schan_fifo_deinit(int unit);


/*******************************************
* @function soc_schan_fifo_init
* purpose API to Initialize and config SCHAN FIFO
* It will initialize the SCHAN FIFO hardware and
* allocate resources for async operation.
* In case of sync only set soc_async_prop_t pointer
* to be NULL.
*
* @param unit [in] unit
* @param prop [in] pointer to soc_async_prop_t
* @param config [in] pointer to soc_schan_fifo_config_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_schan_fifo_init(int unit,
                   soc_async_prop_t  *prop,
                   soc_schan_fifo_config_t *config);

#endif

