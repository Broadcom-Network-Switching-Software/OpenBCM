/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        iproc_mbox.h
 * Purpose:     iproc M0SSQ communication protocol message format and APIs
 */
#ifndef __IPROC_MBOX_H__
#define __IPROC_MBOX_H__
#if defined(BCM_CMICX_SUPPORT) || defined(CMICX_FW_BUILD)
#include <shared/cmicfw/iproc_m0ssq.h>

/* Iproc mailbox message flags */
#define IPROC_SYNC_MSG                0x1
#define IPROC_RESP_REQUIRED           0x2
#define IPROC_RESP_READY              0x4
#define IPROC_RESP_SUCCESS            0x8
#define IPROC_ASYNC_STATUS            0x10

#define IPROC_MBOX_READ_RETRY_COUNT 5000
#define IPROC_MBOX_SLEEP_DELAY      250 /* in usecs */

#define IPROC_MAX_MBOX              16
#define IPROC_MAX_TYPE              2

/* Mbox Types */
#ifndef CMICX_FW_BUILD
#define IPROC_MBOX_TYPE_RX     0
#define IPROC_MBOX_TYPE_TX     1
#else
#define IPROC_MBOX_TYPE_RX     1
#define IPROC_MBOX_TYPE_TX     0
#endif /* CMICX_FW_BUILD */

typedef enum soc_iproc_msg_rtncode_e {
   IPROC_MSG_SUCCESS = 0,
   IPROC_MBOX_ERR_FULL = -1,
   IPROC_MBOX_ERR_EMPTY = -2,
   IPROC_MBOX_ERR_INTERNAL = -3,
   IPROC_MBOX_ERR_INVCMD = -4,
   IPROC_MBOX_ERR_INVPARAM = -5,
   IPROC_MBOX_ERR_ALLOC = -6
} soc_iproc_msg_rtncode_t;

#ifndef CMICX_FW_BUILD
typedef sal_mutex_t soc_iproc_mbox_lock_t;
#else
typedef uint32 soc_iproc_mbox_lock_t;
#endif /* CMICX_FW_BUILD */

typedef int (*soc_iproc_msg_hndlr_t)(void *);

typedef struct soc_iproc_mbox_ptr_s {
   uint32 head;
   uint32 tail;
} soc_iproc_mbox_ptr_t;

/* Iproc mailbox */
typedef struct soc_iproc_mbox_info_s {
   uint32  base;
   uint32  limit;
   uint32  unit;
   uint32  mbox;
   uint32  maxent;
   uint32  inuse;
   soc_iproc_mbox_lock_t lock;
   soc_iproc_mbox_ptr_t *ptr;
   soc_iproc_msg_hndlr_t msg_handler;
   void   *msg_handler_param;
} soc_iproc_mbox_info_t;

typedef struct soc_iproc_mbox_config_s {
   int max_ucores;
   int nucores_enabled;
   int max_mboxs[MAX_UCORES];
} soc_iproc_mbox_config_t;

/* Iproc mailbox message format */
typedef struct soc_iproc_mbox_msg_s {
   uint32  flags;       /* Flags to identify the message type */
   uint32  id;          /* Message identifier */
   uint32  size;        /* Number of 32 bit words in the message payload */
   uint32  tag0;        /* Lower 32bit of 64 bit metadata preserved to use it while handling response */
   uint32  tag1;        /* Upper 32bit of 64 bit metadata preserved to use it while handling response */
   uint32  data[1];     /* Variable size message payload */
} __attribute__ ((packed)) soc_iproc_mbox_msg_t;

#define IPROC_MSG_HDRSZ               ((sizeof(soc_iproc_mbox_msg_t) - sizeof(uint32)) / sizeof(uint32))
#define IPROC_MBOX_PTR(chan)          chan->ptr
#define IPROC_MBOX_MAX_ENT(chan)      chan->maxent
#define IPROC_MSG_SIZE(msg)           (msg->size + IPROC_MSG_HDRSZ)
#define IPROC_MBOX_BASE(chan)         chan->base
#define IPROC_MBOX_START_IDX          (sizeof(soc_iproc_mbox_ptr_t) / sizeof(uint32))

/*
 * Each IPROC MBOX is implemented as a circular ring buffer of 2048 bytes total.
 * Each entry is 4 bytes and hence max entry is 512 (indexed from 0 to 511)
 * The first 2 entries are store head and tail entry indices, hence total usable indices are 2 - 511.
 * MBOX is full if the given size (no. of entries) cannot fit in the available free space entries.
 */
#ifndef CMICX_FW_BUILD

#define IPROC_MBOX_HEAD(chan, mbox)         (soc_m0ssq_sram_read32(chan->unit, ((unsigned long)&mbox->head - soc_iproc_sram_membase_get(chan->unit))))
#define IPROC_MBOX_TAIL(chan, mbox)         (soc_m0ssq_sram_read32(chan->unit, ((unsigned long)&mbox->tail - soc_iproc_sram_membase_get(chan->unit))))

#define MOVE_IPROC_MBOX_HEAD(chan, mbox, size)  \
    { \
        soc_m0ssq_sram_write32(chan->unit, ((unsigned long)&mbox->head - soc_iproc_sram_membase_get(chan->unit)), \
       ((IPROC_MBOX_HEAD(chan, mbox) + size) >= IPROC_MBOX_MAX_ENT(chan)) ? \
       ((IPROC_MBOX_HEAD(chan, mbox) + size) % (IPROC_MBOX_MAX_ENT(chan)) + IPROC_MBOX_START_IDX) : \
       (IPROC_MBOX_HEAD(chan, mbox) + size)); \
    }
#define MOVE_IPROC_MBOX_TAIL(chan, mbox, size)  \
    { \
        soc_m0ssq_sram_write32(chan->unit, ((unsigned long)&mbox->tail - soc_iproc_sram_membase_get(chan->unit)), \
       ((IPROC_MBOX_TAIL(chan, mbox) + size) >= IPROC_MBOX_MAX_ENT(chan)) ? \
       ((IPROC_MBOX_TAIL(chan, mbox) + size) % (IPROC_MBOX_MAX_ENT(chan)) + IPROC_MBOX_START_IDX) : \
       (IPROC_MBOX_TAIL(chan, mbox) + size)); \
    }

static inline int
_iproc_mbox_full(soc_iproc_mbox_info_t *chan,
                 soc_iproc_mbox_ptr_t  *mbox,
                 int size)
{
    uint32 tail, head;
    tail = IPROC_MBOX_TAIL(chan, mbox);
    head = IPROC_MBOX_HEAD(chan, mbox);

    if (tail < head) {
        return (tail + size) >= head;
    } else {
        return (tail - head + size) >= (IPROC_MBOX_MAX_ENT(chan) - IPROC_MBOX_START_IDX);
    }
}

#define IPROC_MBOX_FULL(chan, mbox, size) _iproc_mbox_full(chan, mbox, size)

#define IPROC_MBOX_EMPTY(chan, mbox) (IPROC_MBOX_HEAD(chan, mbox) == IPROC_MBOX_TAIL(chan, mbox))

#else
#define IPROC_MBOX_INTERVAL (30) /* msecs */
#define IPROC_MBOX_HEAD(mbox)         mbox->head
#define IPROC_MBOX_TAIL(mbox)         mbox->tail
#define IPROC_MBOX_MAX_ENT(chan)      chan->maxent
#define MOVE_IPROC_MBOX_TAIL(chan, mbox, size)  \
   { \
        mbox->tail =  ((mbox->tail + size) >= chan->maxent) ? (((mbox->tail + size) % chan->maxent) + IPROC_MBOX_START_IDX) : (mbox->tail + size);  \
   }

#define MOVE_IPROC_MBOX_HEAD(chan, mbox, size)  \
   { \
        mbox->head = ((mbox->head + size) >= chan->maxent) ? (((mbox->head + size) % chan->maxent) + IPROC_MBOX_START_IDX) : (mbox->head + size);  \
   }

#define IPROC_MBOX_EMPTY(mbox)        (mbox->head == mbox->tail)

static inline int
_iproc_mbox_full(soc_iproc_mbox_info_t *chan,
                 soc_iproc_mbox_ptr_t  *mbox,
                 int size)
{
    uint32 tail, head;
    tail = IPROC_MBOX_TAIL(mbox);
    head = IPROC_MBOX_HEAD(mbox);

    if (tail < head) {
        return (tail + size) >= head;
    } else {
        return (tail - head + size) >= (IPROC_MBOX_MAX_ENT(chan) - IPROC_MBOX_START_IDX);
    }
}

#define IPROC_MBOX_FULL(chan, mbox, size) _iproc_mbox_full(chan, mbox, size)

#endif /* !CMICX_FW_BUILD */

#define IS_SYNC_IPROC_MSG(msg)      (msg->flags & IPROC_SYNC_MSG)
#define IS_IPROC_RESP_REQD(msg)     (msg->flags & IPROC_RESP_REQUIRED)
#define IS_IPROC_RESP_READY(msg)    (msg->flags & IPROC_RESP_READY)
#define IS_IPROC_RESP_SUCCESS(msg)  (msg->flags & IPROC_RESP_SUCCESS)
#define IS_ASYNC_IPROC_STATUS(msg)  (msg->flags & IPROC_ASYNC_STATUS)

#define IPROC_MSG_FLAG_SET(msg, flag)  (msg->flags |= flag)
#define IPROC_MSG_FLAG_CLEAR(msg, flag)  (msg->flags &= ~flag)

/* Iproc mailbox APIs */
int soc_iproc_mbox_init(int unit);
int soc_iproc_mbox_exit(int unit);
soc_iproc_mbox_info_t *soc_iproc_tx_mbox_get(int unit, int mboxid);
soc_iproc_mbox_info_t *soc_iproc_rx_mbox_get(int unit, int mboxid);
int soc_iproc_mbox_alloc(int unit, int app_id);
int soc_iproc_mbox_free(int unit, int mbox_id);
int soc_iproc_data_send_nowait(soc_iproc_mbox_info_t *chan, soc_iproc_mbox_msg_t *msg);
int soc_iproc_data_recv(soc_iproc_mbox_info_t *chan, soc_iproc_mbox_msg_t **msg);
int soc_iproc_data_send_wait(soc_iproc_mbox_info_t *chan, soc_iproc_mbox_msg_t *cmd, soc_iproc_mbox_msg_t *resp);
int soc_iproc_mbox_handler_register(int unit, int id, soc_iproc_msg_hndlr_t hndlr, void *param);
int soc_iproc_resp_send(soc_iproc_mbox_info_t *chan, soc_iproc_mbox_msg_t *resp, int didx);
int soc_iproc_msgintr_handler(int unit, void *param);
void _soc_iproc_fw_config(int unit);
int _soc_iproc_num_mbox_get(int unit, int ucnum);
int _soc_iproc_num_ucore_get(int unit);
#endif /* defined(BCM_CMICX_SUPPORT) || defined(CMICX_FW_BUILD) */
#endif /* __IPROC_MBOX_H__ */
