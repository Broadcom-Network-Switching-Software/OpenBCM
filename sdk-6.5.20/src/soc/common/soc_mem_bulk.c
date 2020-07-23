/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Implements SOC memory Bulk API.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/mem.h>
#include <soc/cmic.h>
#include <soc/error.h>
#include <soc/register.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <shared/alloc.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#ifdef BCM_CMICX_SUPPORT
#include <soc/soc_schan_fifo.h>
#endif
#include <soc/schanmsg.h>
#include <soc/soc_mem_bulk.h>

/*!
 * Signature to verify bulk handle
*/
#define SOC_BULK_SIG    0xABCD

/*!
 * Queue ID to bulk handle
*/
#define QID_TO_HANDLE(x) (((x + 1) << 16) | (SOC_BULK_SIG))

/*!
 * bulk handle to Queue ID
*/
#define HANDLE_TO_QID(x) (((x >> 16) & (0xffff)) - 1)

/*!
 * Check if handle is valid
*/
#define IS_HANDLE_VALID(x) (((x) & (SOC_BULK_SIG)) == (SOC_BULK_SIG))

/*!
 * Default bulk handle pool
*/
#define DEF_BULK_Q_POOL_NUM 50

/*!
 * Command buffer Size
*/
#define BULK_CMD_BUF_SIZE 352

/*!
 * SOC bulk memory operation structure
 */

typedef struct soc_mem_bulk_opcache_s {
    soc_mem_t mem;
    int index;
    int copyno;
    int copyno_override;
    void *entry_data_ptr;
    uint32 entry_data[SOC_MAX_MEM_WORDS];
    uint32 con_entry_data[SOC_MAX_MEM_WORDS];
    uint32 cache_entry_data[SOC_MAX_MEM_WORDS];
} soc_mem_bulk_opcache_t;

/*!
 * SOC bulk memory schan command structure
 */

typedef struct soc_mem_bulk_cmd_s {
    /* Status for command sequence  */
    int status;

    /* Size words occupied by commands in buffer */
    size_t size;

    /* Number of Schan messages in data buffer */
    int num_data;

    /* Command data */
    uint32 *data;

    /* Number of cache opeartion */
    int num_cache;

    /* Bulk cache op data */
    soc_mem_bulk_opcache_t op[BULK_CMD_BUF_SIZE/2];

    /* Next pointer */
    struct soc_mem_bulk_cmd_s *next;
} soc_mem_bulk_cmd_t;

/*!
 * SOC bulk memory command queue structure
 */

typedef struct soc_mem_bulk_q_s {
    /* Count of the command buffers */
    uint32 count;

    /* Command queue head pointer */
    soc_mem_bulk_cmd_t *head;

    /* Command queue tail pointer */
    soc_mem_bulk_cmd_t *tail;

} soc_mem_bulk_q_t;


/*!
 * SOC bulk memory operation meta data
 */
typedef struct soc_mem_bulk_s {
    /* Unit number of the command queue */
    int unit;
    /* Number of command queues */
    int count;

    /* Schan command queue */
    soc_mem_bulk_q_t *queue;

    /* Bulk mem operation lock */
    sal_mutex_t  op_lock;

    /* Bulk mem operation complete Sync */
    sal_sem_t  op_sync;

   /* Bulk operator initialized */
    uint8 init;
} soc_mem_bulk_t;

soc_mem_bulk_t _soc_mem_bulk[SOC_MAX_NUM_DEVICES];

/*!
 * Allocate the bulk command.
 */

STATIC soc_mem_bulk_cmd_t *
_bulk_cmd_alloc(int unit)
{
    soc_mem_bulk_cmd_t *cmd = NULL;


    cmd = sal_alloc(sizeof(soc_mem_bulk_cmd_t), "BULK_CMD");
    if (cmd == NULL) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                              "Error: Allocating command\n")));
       goto error;
    }

    sal_memset(cmd, 0, sizeof(soc_mem_bulk_cmd_t));

    /* Allocate DMA able memory for bulk command */
    cmd->data = soc_cm_salloc(unit, sizeof(uint32) * BULK_CMD_BUF_SIZE,
                               "BULK_CMD_BUF");

    if (cmd->data == NULL) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                              "Error: Allocating command buff\n")));
       goto error;
    }

    return cmd;
error:

    if (cmd) {
        sal_free(cmd);
    }
    return NULL;
}

/*!
 * Free the bulk command.
 */

STATIC void
_bulk_cmd_free(int unit,
               soc_mem_bulk_cmd_t *cmd)
{
    if (cmd) {
        if (cmd->data) {
            soc_cm_sfree(unit, cmd->data);
        }
        sal_free(cmd);
    }
}

/*!
 * Initialize the resources of  command queue.
 */
STATIC int
_bulk_handle_create(int unit,
                    soc_mem_bulk_t *bulk,
                    soc_mem_blk_handle_t *handle)
{
    int rv = SOC_E_NONE;
    uint32 i;
    soc_mem_bulk_q_t *queue = NULL;

    /* look for empty queue */
    for (i = 0; i < bulk->count; i++) {
        if (bulk->queue[i].head == NULL) {
            break;
        }
    }
    /* Unlikely: All queues  are occupied , Allocate additional */
   if (i == bulk->count) {
       soc_mem_bulk_q_t *q = NULL;
       q = sal_alloc(sizeof(soc_mem_bulk_q_t) * bulk->count * 2,
                            "SOC_BULK_Q");
       if (q == NULL) {
           LOG_ERROR(BSL_LS_SOC_SOCMEM,
                     (BSL_META_U(unit,
                                 "Error: Allocating command queue\n")));
           return SOC_E_MEMORY;
       }
       sal_memcpy(q, bulk->queue, sizeof(soc_mem_bulk_q_t) * bulk->count);
       bulk->count *= 2;
       sal_free(bulk->queue);
       bulk->queue = q;
   }

    queue = &bulk->queue[i];

    queue->head =_bulk_cmd_alloc(unit);
    if (queue->head == NULL) {
        *handle = 0;
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                              "Error: Memory Allocation\n")));
        rv = SOC_E_MEMORY;
    } else {
        queue->tail = queue->head;
        /* Get handle */
        *handle = QID_TO_HANDLE(i);
        queue->count++;
    }

    return rv;

}

/*!
 * Free the resources of  command queue.
 */
STATIC void
_bulk_handle_destroy(int unit,
                     soc_mem_bulk_t *bulk,
                     soc_mem_blk_handle_t handle)
{
    uint32 i = HANDLE_TO_QID(handle);
    soc_mem_bulk_q_t *queue = &bulk->queue[i];
    soc_mem_bulk_cmd_t *head = queue->head;

    /* Destroy the schan command buffers */
    while (head) {
        soc_mem_bulk_cmd_t *next = head->next;
        _bulk_cmd_free(unit, head);
        head = next;
    }
    sal_memset(queue, 0, sizeof(soc_mem_bulk_q_t));

}

STATIC uint32 *
_bulk_cmd_buff_get(int unit,
                   soc_mem_blk_handle_t handle,
                   soc_mem_bulk_t *bulk,
                   size_t size)
{
    uint32 i = HANDLE_TO_QID(handle);
    uint32 *data = NULL;

    soc_mem_bulk_q_t *queue = &bulk->queue[i];
    soc_mem_bulk_cmd_t *tail = queue->tail;
    if ((tail->size + size) > BULK_CMD_BUF_SIZE) {
        /* Not enough space need to allocate buffer */
        soc_mem_bulk_cmd_t *cmd = _bulk_cmd_alloc(unit);
        if (cmd) {
            data = cmd->data;
            tail->next = cmd;
            tail = queue->tail = cmd;
            queue->count++;
        }
    } else {
        /* buffer pointer at the end of data */
        data = tail->data + tail->size;
    }
    tail->num_data++;
    /* data in words + header + address */
    tail->size += size + 2;

    return data;
}

/*!
 * Create schan message.
 */
STATIC void
_bulk_schan_msg_create(int unit,
                       soc_mem_t mem,
                       int index,
                       int blk,
                       size_t size,
                       uint32 *entry_data,
                       schan_msg_t *msg)
{
    int dst_blk, acc_type, src_blk;
    int data_byte_len = 0;
    uint32 maddr;
    uint8 at;

    schan_msg_clear(msg);

    acc_type = SOC_MEM_ACC_TYPE(unit, mem);
    maddr = soc_mem_addr_get(unit, mem, 0, blk, index, &at);
    soc_mem_dst_blk_update(unit, blk, maddr, &dst_blk);
    data_byte_len = size * sizeof(uint32);
    src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    soc_schan_header_cmd_set(unit, &msg->header, WRITE_MEMORY_CMD_MSG,
                             dst_blk, src_blk, acc_type, data_byte_len, 0, 0);
    msg->writecmd.address = maddr;
    /* Copy the message  and update queue meta data */
    sal_memcpy(msg->writecmd.data , entry_data, size * sizeof(uint32));

}

/*!
 * Check if operation paramters are valid.
 */
STATIC int
_bulk_op_valid(int unit,
                soc_mem_t mem,
                int index,
                int *copyno)
{
    int rv = SOC_E_NONE;

    if (!soc_mem_is_valid(unit, mem)) {
        rv = SOC_E_MEMORY;
        goto error;
    }

    if (*copyno == COPYNO_ALL) {
        *copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }
    if (*copyno == COPYNO_ALL) {
        rv = SOC_E_INTERNAL;
        goto error;
    }

    if (!SOC_MEM_BLOCK_VALID(unit, mem, *copyno)) {
        rv = SOC_E_PARAM;
        goto error;
    }
    if (!soc_mem_index_valid(unit, mem, index)) {
        rv = SOC_E_PARAM;
        goto error;
    }
error:
    return rv;

}

/*!
 * Stage the command in queue.
 */
STATIC int
_bulk_cmd_stage(int unit,
                soc_mem_blk_handle_t handle,
                soc_mem_bulk_t *bulk,
                soc_mem_t mem,
                int index,
                int copyno,
                uint32 *entry_data)
{
    int rv = SOC_E_NONE;

    /* Take opearation lock */
    sal_mutex_take(bulk->op_lock, sal_mutex_FOREVER);

    if (handle == 0) {
        /* Send command immediately using scan PIO */
        rv = soc_mem_write(unit, mem, copyno, index, entry_data);
    } else {
        schan_msg_t *msg = NULL;
        size_t size;
        uint32 *con_entry_data;
        uint32 *cache_entry_data;
        int copyno_override = 0;
        int blk;
        uint32 i = 0;
        soc_mem_bulk_q_t *queue = NULL;
        soc_mem_bulk_cmd_t *tail = NULL;
        soc_mem_bulk_opcache_t *bulk_op = NULL;

        /* Check if handle is valid */
        if (!IS_HANDLE_VALID(handle)) {
            rv = SOC_E_PARAM;
            goto error;
        }
        /* Go to the appropriate location */
        i = HANDLE_TO_QID(handle);
        queue = &bulk->queue[i];
        tail = queue->tail;
        bulk_op = &tail->op[tail->num_cache];
        rv = _bulk_op_valid(unit, mem, index, &copyno);
        if (SOC_FAILURE(rv)) {
            goto error;
        }
        size = soc_mem_entry_words(unit, mem);
       /* Store the bulk op data for cache update */
        bulk_op->mem = mem;
        bulk_op->index = index;
        bulk_op->copyno = copyno;
        con_entry_data =  bulk_op->con_entry_data;
        cache_entry_data = bulk_op->cache_entry_data;
        bulk_op->entry_data_ptr =
                soc_mem_write_tcam_to_hw_format(unit, mem, entry_data,
                                                 cache_entry_data,
                                                 con_entry_data);

        /* Store the entry  data for cache update */
        soc_mem_write_copyno_update(unit, mem, &copyno, &copyno_override);
        bulk_op->copyno_override = copyno_override;
        sal_memcpy(bulk_op->entry_data, entry_data, sizeof(uint32) * size);
        if (bulk_op->entry_data_ptr == con_entry_data) {
            sal_memcpy(entry_data, bulk_op->entry_data_ptr,
                       sizeof(uint32) * size);
        }
        tail->num_cache++;

        /* Write to one or all copies of the memory */
        SOC_MEM_BLOCK_ITER(unit, mem, blk) {
            if (copyno_override) {
                blk = copyno = copyno_override;
            } else if (copyno != COPYNO_ALL && copyno != blk) {
                continue;
            }
            msg = (schan_msg_t *)_bulk_cmd_buff_get(unit, handle, bulk, size);
            if (msg == NULL) {
                LOG_ERROR(BSL_LS_SOC_SOCMEM,
                          (BSL_META_U(unit,
                                      "Error: Getting data buffer\n")));
                rv = SOC_E_MEMORY;
            } else {
                _bulk_schan_msg_create(unit, mem, index, blk,
                                       size, entry_data, msg);
                /* Cache one copy of broadcast_block*/
                if (copyno_override) {
                    break;
                }
            }
        }
    }

error:
    /* Release opearation lock */
    sal_mutex_give(bulk->op_lock);

    return rv;
}
#if 0
STATIC void
_bulk_mem_cb_f(int unit,
                   void *data,
                   void *cookie,
                   int status)
{
    /* Get the command sequence and populate status */

    /* Give the Semaphore */
}
#endif

STATIC int _bulk_pio_ecc_error(int unit,
                               schan_msg_t *msg)
{
    int rv  = SOC_E_FAIL;

    return rv;
}

#ifdef BCM_CMICX_SUPPORT
STATIC int _bulk_fifo_ecc_error(int unit,
                               soc_mem_bulk_cmd_t *cmd)
{
    int rv  = SOC_E_FAIL;

    return rv;
}
#endif

/*!
 * Write commands using FIFO.
 */
STATIC void
_bulk_cache_update(int unit,
                   soc_mem_bulk_t *bulk,
                   soc_mem_bulk_cmd_t *node)
{
    int i;
    int blk;
    soc_mem_bulk_opcache_t *bulk_op = node->op;

    for (i = 0; i < node->num_cache; i++) {
        SOC_MEM_BLOCK_ITER(unit, bulk_op[i].mem, blk) {
            _soc_mem_write_cache_update(unit, bulk_op[i].mem, blk, 0,
                                        bulk_op[i].index, 0,
                                        bulk_op[i].entry_data,
                                        bulk_op[i].entry_data_ptr,
                                        bulk_op[i].cache_entry_data,
                                        bulk_op[i].con_entry_data);
        }
    }
}

/*!
 * Write commands using PIO.
 */
STATIC int
_bulk_pio_write(int unit,
                soc_mem_bulk_t *bulk,
                soc_mem_bulk_q_t *cmdq)
{
    int rv = SOC_E_NONE;
    soc_mem_bulk_cmd_t *head = cmdq->head;
    uint32 allow_intr = SOC_IS_SAND(unit) ? SCHAN_OP_FLAG_ALLOW_INTERRUPTS : 0;

    /* Traverse all the message buffers in the list */
    while (head) {
        int n = head->num_data;
        uint32 *data = head->data;
        /* Extract and send each message from buffer */
        while(n--) {
            schan_msg_t * msg = (schan_msg_t *)data;
            int dw;
            soc_schan_header_cmd_get(unit, &msg->header, 0, 0, 0, 0,
                                     &dw, 0, 0);
            /* data in words + header + address */
            dw = dw / sizeof(uint32) + 2;
            rv = soc_schan_op(unit, msg, dw, 0, allow_intr);
            if (SOC_FAILURE(rv)) {
                LOG_WARN(BSL_LS_SOC_SCHAN,
                    (BSL_META_U(unit,
                        "soc_schan_op: operation failed: %s\n"), soc_errmsg(rv)));

                rv = _bulk_pio_ecc_error(unit, msg);
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_SOC_SOCMEM,
                              (BSL_META_U(unit,
                                      "Error:Pio write = 0x%x\n"), rv));
                    goto error;
                }
            }
            data += dw;
        }
        /* Update the cache */
        _bulk_cache_update(unit, bulk, head);
        head = head->next;
    }
error:
    return rv;
}

#ifdef BCM_CMICX_SUPPORT
/*!
 * Write commands using FIFO.
 */
STATIC int
_bulk_fifo_write(int unit,
                 soc_mem_bulk_t *bulk,
                 soc_mem_bulk_q_t *cmdq)
{
    int rv = SOC_E_NONE;
    soc_mem_bulk_cmd_t *head = cmdq->head;

    /* Traverse all the message buffers in the list */
    while (head) {
        soc_schan_fifo_msg_t msg;
        msg.num = head->num_data;
        msg.size = head->size;
        msg.cmd = (schan_fifo_cmd_t *)head->data;
        msg.resp = NULL;
        msg.interval = 1;
        rv = soc_schan_fifo_msg_send(unit, &msg, head, NULL);
        if (SOC_FAILURE(rv)) {
            rv = _bulk_fifo_ecc_error(unit, head);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                          "Error: Schan FIFO message send = 0x%x\n"), rv));
                break;
            }
        } else {
            /* Update the cache */
            _bulk_cache_update(unit, bulk, head);
            head = head->next;
        }
    }

    return rv;
}
#endif

/*!
 * Commit all the commands in queue.
 */
STATIC int
_bulk_cmd_commit(int unit,
                 soc_mem_bulk_t *bulk,
                 soc_mem_blk_handle_t handle)
{
    int rv = SOC_E_NONE;
    uint32 id = HANDLE_TO_QID(handle);
    soc_control_t       *soc;

    soc = SOC_CONTROL(unit);
    /* Take opearation lock */
    sal_mutex_take(bulk->op_lock, sal_mutex_FOREVER);

    if (soc->memBulkSchanPioMode) {
        rv = _bulk_pio_write(unit, bulk, &bulk->queue[id]);
    }
#ifdef BCM_CMICX_SUPPORT
    else {
    if (soc_feature(unit, soc_feature_cmicx) &&
                    !((SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM))) {
            rv = _bulk_fifo_write(unit, bulk, &bulk->queue[id]);
        } else {
            rv = SOC_E_FAIL;
        }
    }
#endif

    /*Clean up the command queue */
    _bulk_handle_destroy(unit, bulk, handle);

    /* Release opearation lock */
    sal_mutex_give(bulk->op_lock);

    return rv;
}

/*!
 * Init command queue.
 */
STATIC int
_bulk_cmd_init(int unit,
               soc_mem_bulk_t *bulk,
               soc_mem_blk_handle_t *handle)
{

    int rv = SOC_E_NONE;

    /* Take opearation lock */
    sal_mutex_take(bulk->op_lock, sal_mutex_FOREVER);
    rv = _bulk_handle_create(unit, bulk, handle);
    /* Release opearation lock */
    sal_mutex_give(bulk->op_lock);

    return rv;
}

/*!
 * SOC bulk memory opeartion
 */
int
soc_mem_bulk_op(int unit,
                int flags,
                soc_mem_blk_handle_t *handle,
                soc_mem_t mem,
                int index,
                int copyno,
                uint32 *entry_data)
{
    int rv = SOC_E_NONE;
    soc_mem_bulk_t *bulk = &_soc_mem_bulk[unit];

    if (!bulk->init) {
        return SOC_E_INIT;
    }

    switch(flags) {

        case SOC_MEM_BULK_WRITE_INIT:
        /* Initialize bulk op queue and return the handle */
        rv = _bulk_cmd_init(unit, bulk, handle);
        break;

        case SOC_MEM_BULK_WRITE_STAGE:
        if (SOC_SUCCESS(rv)) {
            rv = _bulk_cmd_stage(unit, *handle,
                                 bulk, mem, index, copyno,
                                 entry_data);
        }
        break;

        case SOC_MEM_BULK_WRITE_COMMIT:
        /* Check if handle is valid */
        if (!IS_HANDLE_VALID(*handle)) {
            rv = SOC_E_PARAM;
        }
        if (SOC_SUCCESS(rv)) {
            rv = _bulk_cmd_commit(unit, bulk, *handle);
        }
        break;

        default:
        LOG_VERBOSE(BSL_LS_SOC_SOCMEM,
                    (BSL_META_U(unit, "Unknown flag = %d\n"),
                              flags));
        break;
    }

    return rv;
}

/*!
 * Bulk write for memory entry array.
 */
int
soc_mem_bulk_write(int unit,
                   soc_mem_t *mem,
                   int *index,
                   int *copyno,
                   uint32 **entry_data,
                   int count)
{
    int rv = SOC_E_NONE;
    soc_mem_blk_handle_t handle = 0;
    int i;
    soc_mem_bulk_t *bulk = &_soc_mem_bulk[unit];

    if (!bulk->init) {
        return SOC_E_INIT;
    }
#if defined (BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit) &&
        mem[0] == ISEC_SP_TCAM_KEYm &&
        mem[1] == ISEC_SP_TCAM_MASKm) {
        uint32 *xy_entry_0;
        uint32 *xy_entry_1;

        xy_entry_0 = sal_alloc(SOC_MAX_MEM_WORDS * sizeof(uint32),
                               "soc_mem_bulk_write tcam key entry");
        if (xy_entry_0 == NULL) {
            return SOC_E_MEMORY;
        }
        xy_entry_1 = sal_alloc(SOC_MAX_MEM_WORDS * sizeof(uint32),
                               "soc_mem_bulk_write tcam mask entry");
        if (xy_entry_1 == NULL) {
            sal_free(xy_entry_0);
            return SOC_E_MEMORY;
        }
        _soc_mem_tcam_dm_to_xy_coupled(unit, mem[0], mem[1],
                                       entry_data[0], entry_data[1],
                                       xy_entry_0, xy_entry_1,
                                       1);
        rv = soc_mem_write(unit, mem[0], MEM_BLOCK_ALL, index[0], xy_entry_0);
        if (SOC_FAILURE(rv)) {
            sal_free(xy_entry_0);
            sal_free(xy_entry_1);
            return rv;
        }
        rv = soc_mem_write(unit, mem[1], MEM_BLOCK_ALL, index[1], xy_entry_1);
        if (SOC_FAILURE(rv)) {
            sal_free(xy_entry_0);
            sal_free(xy_entry_1);
            return rv;
        }

        sal_free(xy_entry_0);
        sal_free(xy_entry_1);

        return rv;
    }
#endif /* BCM_FIRELIGHT_SUPPORT */

    rv = _bulk_cmd_init(unit, bulk, &handle);
    if (SOC_FAILURE(rv)) {
        goto error;
    }
    /* Stage the commands in queue */
    for (i = 0 ; i < count; i++) {
        rv = _bulk_cmd_stage(unit, handle,
                   bulk, mem[i], index[i], copyno[i],
                   entry_data[i]);
        if (SOC_FAILURE(rv)) {
            goto error;
        }
    }
    /* Commit the commands in queue */
    rv = _bulk_cmd_commit(unit, bulk, handle);
    if (SOC_FAILURE(rv)) {
        goto error;
    }

    return rv;
error:
    if (handle) {
        /*Clean up the command queue */
       _bulk_handle_destroy(unit, bulk, handle);
    }
    return rv;
}

/*!
 * Bulk mem module cleanup
 */
int
soc_mem_bulk_cleanup(int unit)
{
    int rv = SOC_E_NONE;
    soc_mem_bulk_t *bulk = &_soc_mem_bulk[unit];

    if (bulk->op_lock) {
        sal_mutex_destroy(bulk->op_lock);
        bulk->op_lock = NULL;
    }

    if (bulk->op_sync) {
        sal_sem_destroy(bulk->op_sync);
        bulk->op_sync = NULL;
    }

    if (bulk->queue) {
        sal_free(bulk->queue);
        bulk->queue = NULL;
   }

    return rv;
}

/*!
 * Bulk mem module init
 */
int
soc_mem_bulk_init(int unit)
{
    int rv = SOC_E_NONE;
    soc_mem_bulk_t *bulk = &_soc_mem_bulk[unit];

    sal_memset(bulk, 0, sizeof(soc_mem_bulk_t));
    bulk->op_lock = sal_mutex_create("SOC_BULK");
    if (bulk->op_lock == NULL) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                              "Error: Create mutex\n")));
        rv = SOC_E_INIT;
        goto error;
    }

    bulk->op_sync = sal_sem_create("SOC_BULK", sal_sem_BINARY, 0);

    if (bulk->op_sync == NULL) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                              "Error: Create Semaphore\n")));
       rv = SOC_E_INIT;
       goto error;
    }

    /* Create pool of command queues */
    bulk->queue = sal_alloc(sizeof(soc_mem_bulk_q_t) * DEF_BULK_Q_POOL_NUM,
                            "SOC_BULK_Q");
    if (bulk->queue == NULL) {
        LOG_ERROR(BSL_LS_SOC_SOCMEM,
                  (BSL_META_U(unit,
                              "Error: Allocating command queue\n")));
       rv = SOC_E_MEMORY;
       goto error;
    }

    sal_memset(bulk->queue, 0, sizeof(soc_mem_bulk_q_t) * DEF_BULK_Q_POOL_NUM);
    bulk->count = DEF_BULK_Q_POOL_NUM;
    bulk->init = 1;

    return rv;
error:
    soc_mem_bulk_cleanup(unit);
    return rv;
}

