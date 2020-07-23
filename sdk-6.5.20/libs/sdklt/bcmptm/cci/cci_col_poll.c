/*! \file cci_col_poll.c
 *
 * Functions for poll based counter collection
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <sal/sal_time.h>
#include <sal/sal_msgq.h>
#include <sal/sal_sleep.h>
#include <sal/sal_time.h>
#include <sal/sal_sem.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmbd/bcmbd_fifodma.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_hal.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include "cci_col.h"
#include "cci_cache.h"


#define ACC_FOR_SER FALSE

#define COL_POLL_MSGQ_SIZE           4096

#define IS_PIPE_ENABLE(map, p) ((map) & (0x01 << p))

/* CMIC-specific S-channel definitions */
#define SCMH_BANK_SET(d,v)      F32_SET(d,1,2,v)
#define SCMH_DMA_SET(d,v)       F32_SET(d,3,1,v)
#define SCMH_DATALEN_SET(d,v)   F32_SET(d,7,7,v)
#define SCMH_ACCTYPE_SET(d,v)   F32_SET(d,14,5,v)
#define SCMH_DSTBLK_SET(d,v)    F32_SET(d,19,7,v)
#define SCMH_OPCODE_SET(d,v)    F32_SET(d,26,6,v)
#define READ_MEMORY_CMD_MSG     0x07
#define READ_REGISTER_CMD_MSG   0x0b
#define FIFO_POP_CMD_MSG        0x2a

/*!
* \brief Description of the poll based counter collection structure
*/

typedef struct cci_col_poll_s {
    /*! Context of parent */
    const bcmptm_cci_col_t *parent;

    /*! Number of threads */
    int nthreads;

    /*! Context of thread instance */
    cci_handle_t *hthreads;

} cci_col_poll_t;

/*!
* \brief Counter block meta data
*/

typedef struct col_ctr_block_s {
    size_t wsize;
    uint32_t min_mapid;
    uint32_t max_mapid;
    uint32_t min_offset;
    uint32_t max_offset;
    uint32_t *vaddr;
    uint64_t paddr;
    uint32_t index_min;
    uint32_t index_max;
    /*! Symbol ID */
    bcmdrd_sid_t sid;
    /* Internal use param */
    int param;
    /* current index */
    uint32_t index;
    /* current map id */
    uint32_t mapid;
    int tbl_inst_max;
    int addr_shift;
    uint32_t pt_entry_wsize;
    struct col_ctr_block_s *next;
} col_ctr_block_t;

/*!
* \brief List of counter DMA work list
*/
struct col_ctr_list_s;

typedef struct col_ctr_work_s {
    /*! DMA work  for Port Counter Collection */
    bcmbd_sbusdma_work_t sbusdma_work;

    /* map id in counter cache */
    uint32_t mapid;

    /* Soft port */
    int port;

    /* tbl instance */
    int tbl_inst;

    /* Entry Size */
    uint32_t pt_entry_wsize;

    /*! Virtual address of collection buffer */
    uint32_t *vaddr;

   /*! Counter symbol metadata to which work is associated */
    struct col_ctr_list_s *ctr_sym;

   /*! Next work Node */
    struct col_ctr_work_s *next;
} col_ctr_work_t;

/*!
* \brief List of counter collection entities of one type
*/
typedef struct col_ctr_list_s {
    /*! Symbol ID */
    bcmdrd_sid_t sid;

    /*! Number of counter collection blocks */
    int tbl_inst;

    /*! Head of collection meta data list */
    col_ctr_block_t *ctr_head;

    /*! Pipe map */
    uint32_t pipe_map;

    /*! Counter DMA work list */
    col_ctr_work_t *dma_work;

    /*! Next symbol Node */
    struct col_ctr_list_s *next;
} col_ctr_list_t;


typedef struct col_fifodma_work_s {
    /*! Eviction work item  */
    bcmbd_fifodma_work_t evict_work;

    /*! Eviction work item data */
    bcmbd_fifodma_data_t evict_data;

    /*! Eviction buffer */
    uint32_t *buffer;

    /* Entry Size */
    uint32_t entry_wsize;

    /*! FIFO DMA Channel */
    int chan;

    /* FIFO DMA start */
    bool start;

    /* FIFO DMA ticks */
    uint16_t ticks;

} col_fifodma_work_t;

/*
 * Counter collection instrumentation data.
 */
typedef struct col_thread_instr_data_s {

    /*! DMA work create. */
    uint32_t dma_work_create_count;

    /*! DMA work delete. */
    uint32_t dma_work_delete_count;

    /*! Port counter collection time. */
    uint32_t ctr_port_col_time_usec;

    /*! Eviction counter collection time. */
    uint32_t ctr_evict_col_time_usec;

    /*! Ingress counter collection time. */
    uint32_t ctr_ipipe_col_time_usec;

    /*! Egress counter collection time. */
    uint32_t ctr_epipe_col_time_usec;

    /*! TM counter collection time. */
    uint32_t ctr_tm_col_time_usec;

   /*! Number of Counter entires Evicted. */
   uint32_t ctr_evict_num;

    /*! Eviction counter FIFO DMA collection time. */
    uint32_t ctr_evict_fifo_col_time_usec;

} col_thread_instr_data_t;

/*!
* \brief Description of the poll based counter thread context.
*/

typedef struct col_thread_ctrl_s {
    /*! Logical device ID. */
    int unit;

    /*! Polling context. */
    cci_col_poll_t *col_pol;

    /*! Handle to CCI cache. */
    cci_handle_t hcache;

    /*! CCI context. */
    cci_context_t *cci;

    /*! Thread handle. */
    shr_thread_t th;

    /*! Handle of message queue. */
    sal_msgq_t msgq_hdl;

    /*! wait to handshaking */
    sal_sem_t wait_sem;

    /*! Collection tick */
    uint64_t col_time_tick;

    /*! number of port counter blocks */
    int num_port_reg_blk;

    /*! Port reg counter blocks */
    col_ctr_block_t port_reg_blk[CCI_MAX_PORT_REG_GROUP];

    /*! DMA work  for Port Counter Collection */
    bcmbd_sbusdma_work_t *port_reg_work[CCI_MAX_PPORT_COUNT];

    /*! max port tbl instances */
    int port_reg_inst_max;

    /*! virtual address of collection buffer */
    uint32_t *vaddr_buf;

    /*! phyiscal address of collection buffer */
    uint64_t paddr_buf;

    /*! size of collection buffer */
    size_t wsize_buf;

    /*! collection configuration */
    cci_config_t config;

    /*!Counter  collection meta data list */
    col_ctr_list_t *ctr_mem_col_list[CCI_CTR_TYPE_NUM];
    col_ctr_list_t *ctr_reg_col_list[CCI_CTR_TYPE_NUM];

    /*!FIFO DMA work item for eviction counter */
    col_fifodma_work_t fifodma_work;

    /* Counter collection instrumrntation data. */
    col_thread_instr_data_t instr_data;

    /*! Config semaphore */
    sal_sem_t cnfg_sem;

}  col_thread_ctrl_t;

/*!
* \brief Determine if it is time to collect counter block
*/
static inline bool
cci_col_time(col_thread_ctrl_t *ctc, unsigned int block)
{
    uint64_t multiplier = ctc->config.multiplier[block];
    return (multiplier && (ctc->col_time_tick % multiplier) == 0);
}

static inline int
cci_sbusdma_batch_work_create(int unit, col_thread_ctrl_t *ctc, bcmbd_sbusdma_work_t * work)
{
    col_thread_instr_data_t *instr_data = &ctc->instr_data;
    instr_data->dma_work_create_count++;
    return bcmbd_sbusdma_batch_work_create(unit, work);
}

static inline int
cci_sbusdma_batch_work_delete(int unit, col_thread_ctrl_t *ctc, bcmbd_sbusdma_work_t * work)
{
    col_thread_instr_data_t *instr_data = &ctc->instr_data;
    instr_data->dma_work_delete_count++;
    return bcmbd_sbusdma_batch_work_delete(unit, work);
}

/*!
 * copy data between message and configuration
 */
static int
col_poll_config_copy(int unit,
                    cci_config_t *config,
                    col_thread_msg_t *msg,
                    bool to_cfg)
{

    switch (msg->cmd) {
    case MSG_CMD_POL_CONFIG_COL_EN:
        if (to_cfg) {
            config->col_enable = msg->data[0];
        } else {
            msg->data[0] = config->col_enable;
        }
        break;
    case MSG_CMD_POL_CONFIG_INTERVAL:
        if (to_cfg) {
            config->interval = msg->data[0];
        } else {
            msg->data[0] = config->interval;
        }
        break;
    case MSG_CMD_POL_CONFIG_PORT:
       {
           uint32_t i, count, mask, vbase;
           bcmdrd_pbmp_t pbmp, vpbmp;
           int port;

           count = sizeof(config->pbmp.w)/sizeof(uint32_t);
           vbase = count/2;
           /* Copy LSW and then MSW */
           if (to_cfg) {
               mask = ~0x0;
               for (i = 0; i < count/2; i++) {
                   pbmp.w[2 * i] = msg->data[i] & mask;
                   pbmp.w[2 * i + 1] =  msg->data[i] >> 32;
                   vpbmp.w[2 * i] = msg->data[i + vbase] & mask;
                   vpbmp.w[2 * i + 1] =  msg->data[i + vbase] >> 32;
               }
               /* Iterate the valid pbmp to update the config->pbmp */
               BCMDRD_PBMP_ITER(vpbmp, port) {
                   if (BCMDRD_PBMP_MEMBER(pbmp, port)){
                       BCMDRD_PBMP_PORT_ADD(config->pbmp, port);
                   } else {
                       BCMDRD_PBMP_PORT_REMOVE(config->pbmp, port);
                   }
               }
           } else {
               /* Store both pbmp and valid_pbmp to msg data */
               for (i = 0; i < count/2; i++) {
                   msg->data[i] = config->pbmp.w[2 * i];
                   msg->data[i + vbase] = config->valid_pbmp.w[2 * i];
                   msg->data[i] |= (uint64_t)config->pbmp.w[2 * i + 1] << 32;
                   msg->data[i + vbase] |= (uint64_t)config->valid_pbmp.w[2 * i + 1] << 32;
               }

           }
       }
       break;
    case MSG_CMD_POL_CONFIG_MULTIPLIER_PORT:
        if (to_cfg) {
            config->multiplier[CCI_CTR_TYPE_PORT] = msg->data[0];
        } else {
            msg->data[0] = config->multiplier[CCI_CTR_TYPE_PORT];
        }
        break;
    case MSG_CMD_POL_CONFIG_MULTIPLIER_EPIPE:
        if (to_cfg) {
            config->multiplier[CCI_CTR_TYPE_EPIPE] = msg->data[0];
        } else {
            msg->data[0] = config->multiplier[CCI_CTR_TYPE_EPIPE];
        }
        break;
    case MSG_CMD_POL_CONFIG_MULTIPLIER_IPIPE:
        if (to_cfg) {
            config->multiplier[CCI_CTR_TYPE_IPIPE] = msg->data[0];
        } else {
            msg->data[0] = config->multiplier[CCI_CTR_TYPE_IPIPE];
        }
        break;
    case MSG_CMD_POL_CONFIG_MULTIPLIER_TM:
        if (to_cfg) {
            config->multiplier[CCI_CTR_TYPE_TM] = msg->data[0];
        } else {
            msg->data[0] = config->multiplier[CCI_CTR_TYPE_TM];
        }
        break;
    case MSG_CMD_POL_CONFIG_MULTIPLIER_EVICT:
        if (to_cfg) {
            config->multiplier[CCI_CTR_TYPE_EVICT] = msg->data[0];
        } else {
            msg->data[0] = config->multiplier[CCI_CTR_TYPE_EVICT];
        }
        break;
    case MSG_CMD_POL_CONFIG_MULTIPLIER_SEC:
        if (to_cfg) {
            config->multiplier[CCI_CTR_TYPE_SEC] = msg->data[0];
        } else {
            msg->data[0] = config->multiplier[CCI_CTR_TYPE_SEC];
        }
        break;
    case MSG_CMD_EVICTION_THRESHOLD:
        if (to_cfg) {
            config->eviction_threshold = msg->data[0];
        } else {
            msg->data[0] = config->eviction_threshold;
        }
        break;
    default:
        break;
    }

    return SHR_E_NONE;

}

/*!
 * Send collection configuration change message
 */
int
bcmptm_cci_col_poll_config(int unit,
                           cci_handle_t handle,
                           cci_pol_cmd_t cmd,
                           cci_config_t *config)
{
    cci_col_poll_t *pol = (cci_col_poll_t *)handle;
    col_thread_ctrl_t *ctc;
    col_thread_msg_t msg;
    bool emul;
    bool sim;
    int timeout =  CFG_CCI_TIME_OUT;

    SHR_FUNC_ENTER(unit);

    if (pol->hthreads) {

        ctc = (col_thread_ctrl_t *)pol->hthreads[0];
        SHR_NULL_CHECK(ctc, SHR_E_PARAM);

        emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);
        sim = bcmdrd_feature_is_sim(unit);
        if (emul || sim) {
            timeout *= 20;
        }
        sal_memset(&msg, 0, sizeof(msg));
        msg.cmd = cmd;

        SHR_IF_ERR_EXIT
            (col_poll_config_copy(unit, config, &msg, false));
        if (sal_msgq_post(ctc->msgq_hdl, (void *)&msg, SAL_MSGQ_NORMAL_PRIORITY,
             MSGQ_TIME_OUT) == SAL_MSGQ_E_TIMEOUT) {
            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        }
        /* Wait for configuration to complete */
        if (sal_sem_take(ctc->cnfg_sem, timeout) == -1) {
            SHR_IF_ERR_EXIT(SHR_E_TIMEOUT);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/* Send port add/del change message */
int
bcmptm_cci_col_poll_port_config(int unit,
                                cci_handle_t handle,
                                cci_pol_cmd_t cmd,
                                shr_port_t port)
{
    cci_col_poll_t *pol = (cci_col_poll_t *)handle;
    col_thread_ctrl_t *ctc;
    col_thread_msg_t msg;

    SHR_FUNC_ENTER(unit);

    if (pol->hthreads) {
        ctc = (col_thread_ctrl_t *)pol->hthreads[0];
        SHR_NULL_CHECK(ctc, SHR_E_PARAM);

        sal_memset(&msg, 0, sizeof(msg));
        msg.cmd = cmd;
        msg.data[0] = port;

        if (sal_msgq_post(ctc->msgq_hdl, (void *)&msg, SAL_MSGQ_NORMAL_PRIORITY,
                          MSGQ_TIME_OUT) == SAL_MSGQ_E_TIMEOUT) {
            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        }
    }

exit:
    SHR_FUNC_EXIT();

}
/*!
 * Configure SCHAN PIO opcode
 */
static uint32_t
schan_opcode_set(int unit, int op_code, int dst_blk, int acc_type,
                 int data_len, int dma, uint32_t ignore_mask)
{
    uint32_t oc = 0;

    SCMH_OPCODE_SET(oc, op_code);
    SCMH_DSTBLK_SET(oc, dst_blk);
    SCMH_ACCTYPE_SET(oc, acc_type);
    SCMH_DATALEN_SET(oc, data_len);
    SCMH_DMA_SET(oc, dma);
    SCMH_BANK_SET(oc, ignore_mask);

    return oc;
}

/*!
 * \brief Get the symbole domain port from logical port and symbol ID.
 *
 * \param [in] unit Unit number
 * \param [in] sid Symbol ID
 * \param [in] lport Logical port
 * \param [out] sport Symbol domain port
 *
 * \retval SHR_E_NONE No Errors.
 * \retval SHR_E_PARAM Fail due to null check.
 * \retval SHR_E_PORT Error port assigned.
 * \retval SHR_E_UNAVAIL Unexpected PND of the sid.
 */
static int
col_poll_lport_to_sport(int unit, bcmdrd_sid_t sid, int lport, int *sport)
{
    bcmdrd_port_num_domain_t pnd;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(sport, SHR_E_PARAM);

    /* Check the port domain from sid */
    pnd = bcmdrd_pt_port_num_domain(unit, sid);
    if (pnd == BCMDRD_PND_PHYS) {
        *sport = bcmpc_lport_to_pport(unit, lport);
        if (*sport == BCMPC_INVALID_PPORT) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Invalid port(%d) selected. No physical port mapped.\n"),
                        lport));
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else if (pnd == BCMDRD_PND_LOGIC) {
        *sport = lport;
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "PND(%d) of SID = %s is not applicable for cci\n"),
                  pnd, bcmdrd_pt_sid_to_name(unit, sid)));
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the DMA work items
 */
static int
col_poll_port_reg_dma_work_init(int unit,
                                cci_handle_t handle,
                                int lport,
                                bcmbd_sbusdma_data_t *data,
                                int *num_items)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    int i;
    int items = 0;
    uint32_t acctype;
    int blknum;
    int lane;
    int rv;
    bcmdrd_sid_t sid;
    bcmdrd_sym_info_t sinfo;
    const bcmdrd_chip_info_t *cinfo;
    col_ctr_block_t *port_blk = NULL;
    uint64_t buff_offset = 0;
    int sport;
    int addr_shift;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    SHR_NULL_CHECK(cinfo, SHR_E_FAIL);

    for (i = 0; i < ctc->num_port_reg_blk; i++) {
        port_blk = &ctc->port_reg_blk[i];

        /* Get information about first SID of counter block */
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_mapid_to_sid(unit, ctc->hcache,
                                           port_blk->min_mapid, &sid));
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_info_get(unit, sid, &sinfo));
        acctype = BCMDRD_SYM_INFO_ACCTYPE(sinfo.blktypes);

        SHR_IF_ERR_EXIT(col_poll_lport_to_sport(unit, sid, lport, &sport));
        addr_shift = port_blk->addr_shift;
        if (sport <= port_blk->tbl_inst_max) {
            rv = bcmdrd_pt_port_block_lane_get(unit, sid, sport,
                                               &blknum, &lane);
            if (SHR_FAILURE(rv)) {
                /* Skip invalid ports */
                /* Update the offset for port block */
                buff_offset += port_blk->wsize * sizeof(uint32_t);
                continue;
            }
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "SID = %s, sport=%d, blknum=%d lane =%d\n"),
                        bcmdrd_pt_sid_to_name(unit, sid), sport, blknum, lane));

            /* Populate the work data */
            data[items].op_code =
                schan_opcode_set(unit, READ_REGISTER_CMD_MSG, blknum, acctype,
                                 0, 0, 0);
            data[items].buf_paddr = ctc->paddr_buf + buff_offset;
            data[items].start_addr =
                bcmbd_block_port_addr(unit, blknum, lane, sinfo.offset, 0);
            data[items].data_width = port_blk->pt_entry_wsize;
            data[items].addr_gap = addr_shift;
            data[items].op_count =
                ((port_blk->max_offset - port_blk->min_offset) >> addr_shift) + 1;
            items++;
        }
        /* Update the offset for port block */
        buff_offset += port_blk->wsize * sizeof(uint32_t);
    }
    *num_items = items;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the Port Reg DMA by port
 */
static int
col_poll_port_reg_dma_init_by_port(int unit,
                                   cci_handle_t handle,
                                   shr_port_t port)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    int i;
    int items = 0;
    int sport;
    col_ctr_block_t *port_blk = NULL;
    bcmbd_sbusdma_data_t *data = NULL;
    bcmbd_sbusdma_work_t *work = NULL;
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    items = 0;
    /* Find the number work items needed for this port */
    for (i = 0; i < ctc->num_port_reg_blk; i++) {
        port_blk = &ctc->port_reg_blk[i];
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_mapid_to_sid(
                unit, ctc->hcache, port_blk->min_mapid, &sid));
        SHR_IF_ERR_VERBOSE_EXIT(col_poll_lport_to_sport(unit, sid, port, &sport));
        if (bcmdrd_pt_tbl_inst_from_port(unit, sid, sport) < 0) {
            continue;
        }
        if (sport <= port_blk->tbl_inst_max) {
            items++;
        }
    }
    /* Check applicable items */
    if (items > 0) {
        /* Allocate the memory for work items */
        SHR_ALLOC(work,
                  sizeof(bcmbd_sbusdma_work_t),
                  "bcmptmCciDmaWork");
        SHR_NULL_CHECK(work, SHR_E_MEMORY);
        SHR_ALLOC(data,
                  sizeof(bcmbd_sbusdma_data_t) * items,
                           "bcmptmCciWorkItem");
        SHR_NULL_CHECK(data, SHR_E_MEMORY);
        sal_memset(data, 0,
                   (sizeof(bcmbd_sbusdma_data_t) * items));
        sal_memset(work, 0, sizeof(bcmbd_sbusdma_work_t));
        /* Populate work meta data */
        work->data = data;
        work->flags = SBUSDMA_WF_READ_CMD;

        /* Populate the work item data */
        SHR_IF_ERR_EXIT
            (col_poll_port_reg_dma_work_init(
                unit, handle, port, data, &items));
        work->items = items;
        if (work->items > 0) {
            /* Create Batch work */
            SHR_IF_ERR_EXIT
                (cci_sbusdma_batch_work_create(unit, ctc, work));

            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "DMA work [%p] for port=%d items=%d\n"),
                      (void *)work, port, work->items));
        }
        ctc->port_reg_work[port] = work;
    }
exit:
    if (SHR_FUNC_ERR()) {
        /* Clean up the failed work meta data */
        ctc->port_reg_work[port] = NULL;
        if (data) {
            SHR_FREE(data);
        }
        if (work) {
            SHR_FREE(work);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the Port Reg DMA
 */
static int
col_poll_port_reg_dma_init(int unit,
                           cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    shr_port_t lport = 0;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    /* Iterate over all enabled ports */
    BCMDRD_PBMP_ITER(ctc->config.pbmp, lport) {
        /* Check lport is valid */
        rv = col_poll_port_reg_dma_init_by_port(unit, handle, lport);
        if (SHR_FAILURE(rv)) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Bypass lport %d reg dma init \n"),
                       lport));
            continue;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the DMA work items
 */
static int
col_poll_port_mem_dma_work_init(int unit,
                                cci_handle_t handle,
                                col_ctr_work_t *ctr_work)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    uint32_t acctype;
    int blknum;
    int lane;
    int rv;
    bcmdrd_sid_t sid;
    bcmdrd_sym_info_t sinfo;
    const bcmdrd_chip_info_t *cinfo;
    bcmbd_sbusdma_data_t *data = NULL;
    int index_min, index_max;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr_work, SHR_E_PARAM);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    SHR_NULL_CHECK(cinfo, SHR_E_FAIL);
    data = ctr_work->sbusdma_work.data;

    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_mapid_to_sid(unit, ctc->hcache,
                                       ctr_work->mapid, &sid));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, sid, &sinfo));

    acctype = BCMDRD_SYM_INFO_ACCTYPE(sinfo.blktypes);

    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_frmt_index_from_port(unit, sid, ctr_work->tbl_inst,
                                            &index_min, &index_max));

    rv = bcmdrd_pt_port_block_lane_get(unit, sid, ctr_work->tbl_inst,
                                       &blknum, &lane);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "ERROR: SID=%s, port=%d"),
             bcmdrd_pt_sid_to_name(unit, sid), ctr_work->port));
        /* Skip unsupported port */
        SHR_ERR_EXIT(SHR_E_PORT);
    }
    /* Populate the work data */
    data->op_code = schan_opcode_set(unit, READ_MEMORY_CMD_MSG,
                                     blknum, acctype,
                                     sinfo.entry_wsize * sizeof(uint32_t),
                                     0, 0);
    data->buf_paddr = ctc->paddr_buf;
    data->op_count = index_max - index_min + 1;
    data->addr_gap = 0;
    data->start_addr =
        bcmbd_block_port_addr(unit, blknum, -1, sinfo.offset,
                              lane * data->op_count);
    data->data_width = sinfo.entry_wsize;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "ctr_work dma init sid %s port %d\n"),
              bcmdrd_pt_sid_to_name(unit, sid), ctr_work->port));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the Port mem DMA by port
 */
static int
col_poll_port_mem_dma_init_by_port(int unit,
                                   cci_handle_t handle,
                                   shr_port_t port)
{
    col_thread_ctrl_t *ctc;
    col_ctr_list_t *port_mem;
    col_ctr_work_t *ctr_work = NULL;
    int rv;
    int sport;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    ctc = (col_thread_ctrl_t *)handle;
    port_mem = ctc->ctr_mem_col_list[CCI_CTR_TYPE_PORT];
    while (port_mem) {
        bcmbd_sbusdma_work_t *sbusdma_work = NULL;

        /* Get port based on sid */
        if (col_poll_lport_to_sport(unit, port_mem->sid, port, &sport) < 0) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "DMA work of %s skipped on port=%d\n"),
                      bcmdrd_pt_sid_to_name(unit, port_mem->sid), port));
            port_mem = port_mem->next;
            continue;
        }

        /* Check if sport is valid for this device */
        if (bcmdrd_pt_tbl_inst_from_port(unit, port_mem->sid, sport) < 0) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "DMA work of %s skipped on port=%d\n"),
                      bcmdrd_pt_sid_to_name(unit, port_mem->sid), port));
            port_mem = port_mem->next;
            continue;
        }

        SHR_ALLOC(ctr_work, sizeof(col_ctr_work_t), "bcmptmCciDmaMemWork");
        SHR_NULL_CHECK(ctr_work, SHR_E_MEMORY);
        sal_memset(ctr_work, 0, sizeof(col_ctr_work_t));
        ctr_work->port = port;
        ctr_work->mapid = port_mem->ctr_head->min_mapid;
        ctr_work->pt_entry_wsize = port_mem->ctr_head->pt_entry_wsize;
        ctr_work->tbl_inst = sport;

        sbusdma_work = &ctr_work->sbusdma_work;
        sal_memset(sbusdma_work, 0, sizeof(bcmbd_sbusdma_work_t));
        SHR_ALLOC(sbusdma_work->data,
                  sizeof(bcmbd_sbusdma_data_t), "bcmptmCciDmaMemData");
        SHR_NULL_CHECK(sbusdma_work->data, SHR_E_MEMORY);
        sal_memset(sbusdma_work->data, 0, sizeof(bcmbd_sbusdma_data_t));
        sbusdma_work->flags = SBUSDMA_WF_READ_CMD;
        /* Populate the work item data */
        sbusdma_work->items = 0;
        rv = col_poll_port_mem_dma_work_init(unit, handle, ctr_work);
        if (SHR_SUCCESS(rv)) {
            /* Create Batch work */
            sbusdma_work->items = 1;
            rv = cci_sbusdma_batch_work_create(unit, ctc, sbusdma_work);
            if (SHR_FAILURE(rv)) {
                sbusdma_work->items = 0;
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "DMA work Error [%p] for port=%d\n"),
                          (void *)sbusdma_work, port));
            }
        }
        ctr_work->next = port_mem->dma_work;
        port_mem->dma_work = ctr_work;
        ctr_work = NULL;

        port_mem = port_mem->next;
    }
exit:
    if (SHR_FUNC_ERR()) {
        /* Free the counter work if not saved in the list */
        if (ctr_work) {
            SHR_FREE(ctr_work);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the Port mem DMA
 */
static int
col_poll_port_mem_dma_init(int unit,
                           cci_handle_t handle)
{
    col_thread_ctrl_t *ctc;
    shr_port_t port;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    ctc = (col_thread_ctrl_t *)handle;
    BCMDRD_PBMP_ITER(ctc->config.pbmp, port) {
        rv = col_poll_port_mem_dma_init_by_port(unit, handle, port);
        if (SHR_FAILURE(rv)) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Bypass lport %d mem dma init \n"),
                       port));
            continue;
        }
    }

exit:
        SHR_FUNC_EXIT();
}

/*!
 * Cleanup the Port Mem DMA work by port
 */
static int
col_poll_port_mem_dma_cleanup_by_port(int unit,
                                      cci_handle_t handle,
                                      int port)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    col_ctr_list_t *port_mem;
    bcmbd_sbusdma_work_t *sbusdma_work;
    col_ctr_work_t *ctr_work, *work;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;
    port_mem = ctc->ctr_mem_col_list[CCI_CTR_TYPE_PORT];

    while (port_mem) {
        ctr_work = port_mem->dma_work;
        work = ctr_work;
        while (ctr_work) {
            if (ctr_work->port == port) {
                sbusdma_work = &ctr_work->sbusdma_work;
                if (sbusdma_work->items > 0) {
                    cci_sbusdma_batch_work_delete(unit, ctc, sbusdma_work);
                }
                if (sbusdma_work->data) {
                    SHR_FREE(sbusdma_work->data);
                }
                if (ctr_work == port_mem->dma_work) {
                    port_mem->dma_work = ctr_work->next;
                } else {
                    work->next = ctr_work->next;
                }
                SHR_FREE(ctr_work);
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "DMA ctr work %p for port=%d \n"),
                          (void *)ctr_work, port));
                break;
            } else {
                work = ctr_work;
                ctr_work = ctr_work->next;
            }
        }
        port_mem = port_mem->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Cleanup the Port Mem DMA work
 */
static int
col_poll_port_mem_dma_cleanup(int unit,
                              cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    col_ctr_list_t *port_mem;
    bcmbd_sbusdma_work_t *sbusdma_work;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;
    port_mem = ctc->ctr_mem_col_list[CCI_CTR_TYPE_PORT];

    while (port_mem) {
        col_ctr_work_t *ctr_work = port_mem->dma_work;
        while (ctr_work) {
            col_ctr_work_t *work;
            sbusdma_work = &ctr_work->sbusdma_work;
            if (sbusdma_work->items > 0) {
                cci_sbusdma_batch_work_delete(unit, ctc, sbusdma_work);
            }
            if (sbusdma_work->data) {
                SHR_FREE(sbusdma_work->data);
            }
            work = ctr_work;
            ctr_work = ctr_work->next;
            SHR_FREE(work);
        }
        port_mem = port_mem->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Cleanup the Port Reg DMA work by port
 */
static int
col_poll_port_reg_dma_cleanup_by_port(int unit,
                                      cci_handle_t handle,
                                      int port)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    bcmbd_sbusdma_work_t *work;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    if (ctc->port_reg_work[port] != NULL) {
        work = ctc->port_reg_work[port];
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "DMA work [%p] for port=%d items=%d\n"),
                  (void *)work, port, work->items));
        if (work->items > 0) {
            cci_sbusdma_batch_work_delete(unit, ctc, work);
        }
        ctc->port_reg_work[port] = NULL;
        /* Delete the batch work */
        if (work->data) {
            SHR_FREE(work->data);
        }
        SHR_FREE(work);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Cleanup the Port Reg DMA work
 */
static int
col_poll_port_reg_dma_cleanup(int unit,
                              cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    int lport;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    /* Iterate over all enabled ports */
    BCMDRD_PBMP_ITER(ctc->config.pbmp, lport) {
        SHR_IF_ERR_EXIT
            (col_poll_port_reg_dma_cleanup_by_port(unit, handle, lport));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the port reg counter collection meta data
 */
static int
col_poll_ctr_port_reg_init(int unit,
                           cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    uint32_t  maxid, i, offset;
    bcmptm_cci_cache_ctr_info_t cinfo;
    int addr_shift;
    int index = -1;
    col_ctr_block_t *port_blk = NULL;
    int blktype;

    SHR_FUNC_ENTER(unit);

    /* Get maximum map ID */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_ctr_map_size(unit, ctc->hcache, &maxid));

    offset = 0;
    ctc->wsize_buf = 0;
    blktype = 0;

    /* Divide the port counter registers into groups
        * so that offsets of member SID's are equispaced for same port.
        * This will help to DMA the counter values all registers in group
        * at the same time.
        * Only the ports set active by user will be collected.
        */
    for (i = 0; i < maxid; i++) {
        /* Get Counter Information */
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_ctr_info_get(unit, ctc->hcache, i, &cinfo));
        if ((cinfo.ctrtype == CCI_CTR_TYPE_PORT) && !cinfo.is_mem) {
            if (bcmptm_pt_cci_ctr_is_bypass(unit, cinfo.sid)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "IP-EP bypass=%s\n"),
                             bcmdrd_pt_sid_to_name(unit, cinfo.sid)));
                continue;
            }
            addr_shift = bcmptm_pt_cci_reg_addr_gap_get(unit, cinfo.sid);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Map id=%d offset=0x%x\n"),
                         i, cinfo.offset));
           if ((cinfo.offset == 0) ||
               (cinfo.offset > offset + (1 << addr_shift)) ||
               (cinfo.blktype != blktype)) {
               /* Discontinuity in offset or different blktype, next chunk */
               index++;
               port_blk = &ctc->port_reg_blk[index];
               SHR_NULL_CHECK(port_blk, SHR_E_INIT);
               port_blk->min_mapid = i;
               port_blk->min_offset = cinfo.offset;
               port_blk->wsize = 0;
               port_blk->addr_shift = addr_shift;
               port_blk->pt_entry_wsize = cinfo.pt_entry_wsize;
               port_blk->tbl_inst_max = cinfo.tbl_inst;
               if (ctc->port_reg_inst_max < port_blk->tbl_inst_max) {
                   ctc->port_reg_inst_max = port_blk->tbl_inst_max;
               }
               LOG_VERBOSE(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Min Map id=%d offset=0x%x tbl_inst=%d\n"),
                   i, cinfo.offset, cinfo.tbl_inst));
               port_blk->max_mapid = i;
               port_blk->wsize += cinfo.pt_entry_wsize;
               ctc->wsize_buf += cinfo.pt_entry_wsize;
               offset = cinfo.offset;
               blktype = cinfo.blktype;
           } else if (cinfo.offset == offset) {
               /* Duplicate entry ignore */
               continue;
           } else {
               SHR_NULL_CHECK(port_blk, SHR_E_INIT);
               port_blk->max_mapid = i;
               port_blk->wsize += cinfo.pt_entry_wsize;
               ctc->wsize_buf += cinfo.pt_entry_wsize;
               offset = cinfo.offset;
               blktype = cinfo.blktype;
               port_blk->max_offset = offset;
           }
       }
   }
   ctc->num_port_reg_blk = index + 1;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect the memory type counter block using DMA
 */
static int
col_poll_ctr_dma_work(int unit,
                      cci_handle_t handle,
                      void *param,
                      bool update_cache)
{
    uint32_t i;
    col_ctr_block_t *ctr_head = NULL;
    bcmptm_cci_ctr_info_t info = {0};
    col_ctr_list_t *ctr_mem = NULL;
    col_ctr_work_t *ctr_work = NULL;
    bcmbd_sbusdma_work_t *work = NULL;
    uint32_t *vaddr = NULL;


    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    ctr_work = (col_ctr_work_t *)param;
    ctr_mem = ctr_work->ctr_sym;
    work = &ctr_work->sbusdma_work;
    ctr_head = ctr_mem->ctr_head;
    vaddr = ctr_work->vaddr;

    if (work->items) {
        SHR_IF_ERR_EXIT
            (bcmbd_sbusdma_work_execute(unit, work));
        while (ctr_head) {
            for (i = ctr_head->index_min; i <= ctr_head->index_max; i++) {
                /* Update the cache with value as input parameter */
                if (update_cache) {
                    info.dyn_info.index = i;
                    info.dyn_info.tbl_inst = ctr_work->tbl_inst;
                    info.map_id = ctr_head->min_mapid;
                    info.sid = ctr_mem->sid;
                    SHR_IF_ERR_EXIT
                         (bcmptm_cci_cache_update(unit, handle, &info,
                                                  vaddr,
                                                  ctr_work->pt_entry_wsize));
                }
                vaddr += ctr_work->pt_entry_wsize;
            }
            ctr_head = ctr_head->next;
        }
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * Do counter memory  DMA
 */
static int
col_poll_ctr_mem_dma(int unit,
                     int ctr_type,
                     cci_handle_t handle)
{
    col_thread_ctrl_t *ctc;
    col_ctr_list_t *ctr_mem;
    bcmptm_cci_pool_state_t state;
    int i;
    bool cor, emul;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    ctc = (col_thread_ctrl_t *)handle;;
    ctr_mem = ctc->ctr_mem_col_list[ctr_type];
    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    while (ctr_mem) {
        if (ctr_type == CCI_CTR_TYPE_EVICT) {
            SHR_IF_ERR_EXIT
                (bcmptm_cci_flex_counter_pool_get_state(unit, ctr_mem->sid, &state));

            /* Skip SBUSDMA if COR is set and not emulator, counter will be evicted */
             SHR_IF_ERR_EXIT
                 (bcmptm_cci_cache_clearon_read_get(unit, ctc->hcache,
                                           ctr_mem->ctr_head->min_mapid, &cor));
            /* Check if eviction counter is enabled */
            if ((state != ENABLE) || (cor && !emul)) {
                if (shr_thread_stopping(ctc->th)) {
                    SHR_EXIT();
                }
                ctr_mem = ctr_mem->next;
                continue;
            }

            LOG_DEBUG(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Evict mem DMA = %s, pipemap = 0x%x\n"),
                 bcmdrd_pt_sid_to_name(unit, ctr_mem->sid), ctr_mem->pipe_map));
        }
        /* Iterate over all the instances of memory table */
        for (i = 0; i < ctr_mem->tbl_inst; i++) {
            col_ctr_work_t *ctr_work = &ctr_mem->dma_work[i];
            if ((ctr_type != CCI_CTR_TYPE_PORT) &&
                (!IS_PIPE_ENABLE(ctr_mem->pipe_map, i))) {
                continue;
            }
            /* Collect the counter and update the Counter cache */
            SHR_IF_ERR_EXIT
                (bcmptm_cci_cache_hw_sync(unit, ctc->hcache,
                                          col_poll_ctr_dma_work,
                                          ctr_work, TRUE));
            if (shr_thread_stopping(ctc->th)) {
                SHR_EXIT();
            }
        }
        ctr_mem = ctr_mem->next;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the DMA work items
 */
static int
col_poll_ctr_mem_dma_work_init(int unit,
                               cci_handle_t handle,
                               col_ctr_list_t *ctr_mem,
                               col_ctr_work_t *ctr_work)
{
    col_thread_ctrl_t *ctc = NULL;
    int item = 0;
    bcmbd_sbusdma_data_t *data = NULL;
    col_ctr_block_t *ctr_head = NULL;
    bcmbd_pt_dyn_info_t dyn_info;
    uint32_t cwords[4], nwords;
    uint64_t buff_offset = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;
    SHR_NULL_CHECK(ctr_work, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr_mem, SHR_E_PARAM);
    data = ctr_work->sbusdma_work.data;
    ctr_head = ctr_mem->ctr_head;
    /* Populate the DMA data */
    while (ctr_head) {
        dyn_info.index = ctr_head->index_min;
        dyn_info.tbl_inst = ctr_work->tbl_inst;

        SHR_IF_ERR_EXIT
            (bcmbd_pt_cmd_hdr_get(unit, ctr_mem->sid, &dyn_info, NULL,
                                  BCMBD_PT_CMD_READ, 4, cwords, &nwords));
        assert(nwords != 0);
        data[item].start_addr = cwords[1];                 /* Addr beat */
        data[item].data_width = ctr_work->pt_entry_wsize;  /* Num of data beats */
        data[item].addr_gap = 0;                           /* Gap between addr */
        data[item].op_code = cwords[0];                    /* Opcode beat */
        data[item].op_count = ctr_head->index_max - ctr_head->index_min + 1;
        data[item].buf_paddr = ctc->paddr_buf + buff_offset;
        buff_offset += ctr_head->wsize;
        LOG_DEBUG(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "DMA work  init Name = %s, size = %u\n"),
                 bcmdrd_pt_sid_to_name(unit, ctr_mem->sid),
                 (uint32_t)ctr_head->wsize));
        ctr_head = ctr_head->next;
        item++;
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * Initialize the counter memory. DMA meta data
 */
static int
col_poll_ctr_mem_dma_init(int unit,
                          int ctr_type,
                          cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = NULL;
    col_ctr_list_t *ctr_mem;
    int items = 0;
    int rv, i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;
    ctr_mem = ctc->ctr_mem_col_list[ctr_type];

    while (ctr_mem) {
        col_ctr_block_t *ctr_head = NULL;
        /* find the number of items needed in the DMA work */
        ctr_head = ctr_mem->ctr_head;
        items = 0;
        /* Check if any blocks are initialized */
        if (ctr_head == NULL) {
            LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "No DMA block:Name = %s\n"),
                   bcmdrd_pt_sid_to_name(unit, ctr_mem->sid)));
            ctr_mem = ctr_mem->next;
            continue;
        }
        while (ctr_head) {
            items++;
            ctr_head = ctr_head->next;
        }
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "DMA init Name = %s, items = %d\n"),
                   bcmdrd_pt_sid_to_name(unit, ctr_mem->sid), items));
        SHR_ALLOC(ctr_mem->dma_work, sizeof(col_ctr_work_t) * ctr_mem->tbl_inst,
                "bcmptmCciMemDmaWork");
        SHR_NULL_CHECK(ctr_mem->dma_work, SHR_E_MEMORY);
        /* Iterate over all the instance of table */
        for (i = 0; i < ctr_mem->tbl_inst; i++) {
            bcmbd_sbusdma_work_t *sbusdma_work = NULL;
            col_ctr_work_t *ctr_work = &ctr_mem->dma_work[i];
            if ((ctr_type != CCI_CTR_TYPE_PORT) &&
                (!IS_PIPE_ENABLE(ctr_mem->pipe_map, i))) {
                continue;
            }
            sal_memset(ctr_work, 0, sizeof(col_ctr_work_t));
            ctr_work->mapid = ctr_mem->ctr_head->min_mapid;
            ctr_work->pt_entry_wsize = ctr_mem->ctr_head->pt_entry_wsize;
            ctr_work->tbl_inst = i;
            ctr_work->vaddr = ctc->vaddr_buf;
            ctr_work->ctr_sym = ctr_mem;
            /* Initialize SBUSDMA work */
            sbusdma_work = &ctr_work->sbusdma_work;
            sal_memset(sbusdma_work, 0, sizeof(bcmbd_sbusdma_work_t));
            SHR_ALLOC(sbusdma_work->data,
                      sizeof(bcmbd_sbusdma_data_t) * items, "bcmptmCciDmaMemData");
            SHR_NULL_CHECK(sbusdma_work->data, SHR_E_MEMORY);
            sal_memset(sbusdma_work->data, 0,
                      sizeof(bcmbd_sbusdma_data_t) * items);
            sbusdma_work->flags = SBUSDMA_WF_READ_CMD;
            /* Populate the work item data */
            rv = col_poll_ctr_mem_dma_work_init(unit, handle,
                                                ctr_mem, ctr_work);
            if (SHR_SUCCESS(rv)) {
                sbusdma_work->items = items;
                /* Create Batch work */
                rv = cci_sbusdma_batch_work_create(unit, ctc, sbusdma_work);
                if (SHR_FAILURE(rv)) {
                    sbusdma_work->items = 0;
                    LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "DMA work Error [%p]\n"),
                             (void *)sbusdma_work));
                }
            }
        }
        ctr_mem = ctr_mem->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Cleanup mem counter DMA meta data
 */
static int
col_poll_ctr_mem_dma_cleanup(int unit,
                           int ctr_type,
                           cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = NULL;
    col_ctr_list_t *ctr_mem;
    bcmbd_sbusdma_work_t *sbusdma_work;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;
    ctr_mem = ctc->ctr_mem_col_list[ctr_type];

    while (ctr_mem) {
        if (ctr_mem->dma_work) {
            for (i = 0; i < ctr_mem->tbl_inst; i++) {
                col_ctr_work_t *ctr_work = &ctr_mem->dma_work[i];
                if ((ctr_type != CCI_CTR_TYPE_PORT) &&
                    (!IS_PIPE_ENABLE(ctr_mem->pipe_map, i))) {
                    continue;
                }
                sbusdma_work = &ctr_work->sbusdma_work;
                if (sbusdma_work->items > 0) {
                    cci_sbusdma_batch_work_delete(unit, ctc, sbusdma_work);
                }
                if (sbusdma_work->data) {
                    SHR_FREE(sbusdma_work->data);
                }
            }
            SHR_FREE(ctr_mem->dma_work);
        }
        ctr_mem = ctr_mem->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect the reg type counter block using DMA
 */
static int
col_poll_ctr_reg_dma_work(int unit,
                          cci_handle_t handle,
                          void *param,
                          bool update_cache)
{
    uint32_t i;
    col_ctr_block_t *ctr_head = NULL;
    bcmptm_cci_ctr_info_t info = {0};
    col_ctr_list_t *ctr_reg = NULL;
    col_ctr_work_t *ctr_work = NULL;
    bcmbd_sbusdma_work_t *work = NULL;
    uint32_t *vaddr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    ctr_work = (col_ctr_work_t *)param;
    ctr_reg = ctr_work->ctr_sym;
    work = &ctr_work->sbusdma_work;
    ctr_head = ctr_reg->ctr_head;
    vaddr = ctr_work->vaddr;

    if (work->items) {
        SHR_IF_ERR_EXIT
            (bcmbd_sbusdma_work_execute(unit, work));
        while (ctr_head) {
            if (update_cache) {
                for (i = ctr_head->min_mapid; i <= ctr_head->max_mapid; i++) {
                    /* Update the cache with value as input parameter */
                    info.dyn_info.index = 0;
                    info.dyn_info.tbl_inst = ctr_work->tbl_inst;
                    info.map_id = i;
                    SHR_IF_ERR_EXIT
                         (bcmptm_cci_cache_update(unit, handle, &info,
                                                  vaddr,
                                                  ctr_work->pt_entry_wsize));
                    vaddr += ctr_head->pt_entry_wsize;
                }
            }
            vaddr += ctr_head->wsize;
            ctr_head = ctr_head->next;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Do counter registers DMA
 */
static int
col_poll_ctr_reg_dma(int unit,
                     int ctr_type,
                     cci_handle_t handle)
{
    col_thread_ctrl_t *ctc;
    col_ctr_list_t *ctr_reg;
    int i;
    col_ctr_work_t *ctr_work;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    ctc = (col_thread_ctrl_t *)handle;;
    ctr_reg = ctc->ctr_reg_col_list[ctr_type];

    while (ctr_reg) {
        if (ctr_reg->dma_work) {
            /* Iterate over all the instances of registers list */
            for (i = 0; i < ctr_reg->tbl_inst; i++) {
                if ((ctr_type != CCI_CTR_TYPE_PORT) &&
                    (!IS_PIPE_ENABLE(ctr_reg->pipe_map, i))) {
                    continue;
                }
                ctr_work = &ctr_reg->dma_work[i];
                /* Collect the counter and update the Counter cache */
                SHR_IF_ERR_EXIT
                    (bcmptm_cci_cache_hw_sync(unit, ctc->hcache,
                                              col_poll_ctr_reg_dma_work,
                                              ctr_work, TRUE));
                if (shr_thread_stopping(ctc->th)) {
                    SHR_EXIT();
                }
            }
        }
        ctr_reg = ctr_reg->next;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the DMA work items
 */
static int
col_poll_ctr_reg_dma_work_init(int unit,
                               cci_handle_t handle,
                               int ctr_type,
                               col_ctr_list_t *ctr_reg,
                               col_ctr_work_t *ctr_work)
{
    col_thread_ctrl_t *ctc = NULL;
    int item = 0;
    bcmbd_sbusdma_data_t *data = NULL;
    col_ctr_block_t *ctr_head = NULL;
    bcmbd_pt_dyn_info_t dyn_info;
    uint32_t cwords[4], nwords;
    uint64_t buff_offset = 0;
    int addr_shift;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;
    SHR_NULL_CHECK(ctr_work, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr_reg, SHR_E_PARAM);
    data = ctr_work->sbusdma_work.data;
    ctr_head = ctr_reg->ctr_head;

    /* Populate the DMA data */
    while (ctr_head) {
        dyn_info.index = ctr_head->index_min;
        dyn_info.tbl_inst = ctr_work->tbl_inst;
        addr_shift = ctr_head->addr_shift;

        SHR_IF_ERR_EXIT
            (bcmbd_pt_cmd_hdr_get(unit, ctr_reg->sid, &dyn_info, NULL,
                                  BCMBD_PT_CMD_READ, 4, cwords, &nwords));
        assert(nwords != 0);
        if (ctr_head->max_offset != 0) {
            /* Gap between addresses */
            data[item].addr_gap = addr_shift;
            data[item].op_count = ctr_head->wsize / ctr_work->pt_entry_wsize;
        } else {
            data[item].addr_gap = 0;
            data[item].op_count = 1;
        }
        data[item].start_addr = cwords[1];
        data[item].data_width = ctr_work->pt_entry_wsize;
        data[item].op_code = cwords[0];
        data[item].buf_paddr = ctc->paddr_buf + buff_offset;
        buff_offset += ctr_head->wsize;
        LOG_DEBUG(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "DMA work init Name = %s, size = %u\n"),
                 bcmdrd_pt_sid_to_name(unit, ctr_reg->sid),
                 (uint32_t)ctr_head->wsize));
        ctr_head = ctr_head->next;
        item++;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the counter register DMA meta data
 */
static int
col_poll_ctr_reg_dma_init(int unit,
                          int ctr_type,
                          cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = NULL;
    col_ctr_list_t *ctr_reg;
    int items = 0;
    int rv, i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;
    ctr_reg = ctc->ctr_reg_col_list[ctr_type];

    while (ctr_reg) {
        col_ctr_block_t *ctr_head = NULL;
        /* find the number of items needed in the DMA work */
        ctr_head = ctr_reg->ctr_head;
        items = 0;
        /* Check if any blocks are initialized */
        if (ctr_head == NULL) {
            ctr_reg = ctr_reg->next;
            continue;
        }
        while (ctr_head) {
            items++;
            ctr_head = ctr_head->next;
        }
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "DMA init Name = %s, items = %d\n"),
                   bcmdrd_pt_sid_to_name(unit, ctr_reg->sid), items));
        SHR_ALLOC(ctr_reg->dma_work, sizeof(col_ctr_work_t) * ctr_reg->tbl_inst,
                  "bcmptmCciRegDmaWork");
        SHR_NULL_CHECK(ctr_reg->dma_work, SHR_E_MEMORY);
        /* Iterate over all the instance of table */
        for (i = 0; i < ctr_reg->tbl_inst; i++) {
            bcmbd_sbusdma_work_t *sbusdma_work = NULL;
            col_ctr_work_t *ctr_work;

            if ((ctr_type != CCI_CTR_TYPE_PORT) &&
                (!IS_PIPE_ENABLE(ctr_reg->pipe_map, i))) {
                continue;
            }
            ctr_work = &ctr_reg->dma_work[i];
            sal_memset(ctr_work, 0, sizeof(col_ctr_work_t));
            ctr_work->mapid = ctr_reg->ctr_head->min_mapid;
            ctr_work->pt_entry_wsize = ctr_reg->ctr_head->pt_entry_wsize;
            ctr_work->tbl_inst = i;
            ctr_work->vaddr = ctc->vaddr_buf;
            ctr_work->ctr_sym = ctr_reg;
            /* Initialize SBUSDMA work */
            sbusdma_work = &ctr_work->sbusdma_work;
            sal_memset(sbusdma_work, 0, sizeof(bcmbd_sbusdma_work_t));
            SHR_ALLOC(sbusdma_work->data,
                      sizeof(bcmbd_sbusdma_data_t) * items, "bcmptmCciDmaMemData");
            SHR_NULL_CHECK(sbusdma_work->data, SHR_E_MEMORY);
            sal_memset(sbusdma_work->data, 0,
                      sizeof(bcmbd_sbusdma_data_t) * items);
            sbusdma_work->flags = SBUSDMA_WF_READ_CMD;
            /* Populate the work item data */
            rv = col_poll_ctr_reg_dma_work_init(unit, handle, ctr_type,
                                                ctr_reg, ctr_work);
            if (SHR_SUCCESS(rv)) {
                sbusdma_work->items = items;
                /* Create Batch work */
                rv = cci_sbusdma_batch_work_create(unit, ctc, sbusdma_work);
                if (SHR_FAILURE(rv)) {
                    sbusdma_work->items = 0;
                    LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "DMA work Error [%p]\n"),
                             (void *)sbusdma_work));
                }
            }
        }
        ctr_reg = ctr_reg->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Cleanup reg counter DMA meta data
 */
static int
col_poll_ctr_reg_dma_cleanup(int unit,
                           int ctr_type,
                           cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = NULL;
    col_ctr_list_t *ctr_reg;
    bcmbd_sbusdma_work_t *sbusdma_work;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;
    ctr_reg = ctc->ctr_reg_col_list[ctr_type];

    while (ctr_reg) {
        if (ctr_reg->dma_work) {
            for (i = 0; i < ctr_reg->tbl_inst; i++) {
                col_ctr_work_t *ctr_work;

                if ((ctr_type != CCI_CTR_TYPE_PORT) &&
                    (!IS_PIPE_ENABLE(ctr_reg->pipe_map, i))) {
                    continue;
                }
                ctr_work = &ctr_reg->dma_work[i];
                sbusdma_work = &ctr_work->sbusdma_work;
                if (sbusdma_work->items > 0) {
                    cci_sbusdma_batch_work_delete(unit, ctc, sbusdma_work);
                }
                if (sbusdma_work->data) {
                    SHR_FREE(sbusdma_work->data);
                }
            }
            SHR_FREE(ctr_reg->dma_work);
        }
        ctr_reg = ctr_reg->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
col_poll_ctr_reg_init(int unit,
                      int ctr_type,
                      cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    uint32_t  maxid, i;
    bcmptm_cci_cache_ctr_info_t cinfo;
    col_ctr_block_t *block = NULL;
    col_ctr_list_t **ctc_col_list = NULL;
    uint32_t offset;
    int addr_shift;
    bool new_block;

    SHR_FUNC_ENTER(unit);

    ctc_col_list = &ctc->ctr_reg_col_list[ctr_type];
    offset = 0;
    /* Get maximum map ID */
    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_ctr_map_size(unit, ctc->hcache, &maxid));
    for (i = 0; i < maxid; i++) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_ctr_info_get(unit, ctc->hcache, i, &cinfo));
        if ((cinfo.ctrtype == ctr_type) && (!cinfo.is_mem)) {
            col_ctr_list_t *ctr_list = NULL;
            if (bcmptm_pt_cci_ctr_is_bypass(unit, cinfo.sid)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "IP-EP bypass=%s\n"),
                     bcmdrd_pt_sid_to_name(unit, cinfo.sid)));
                continue;
            }
            /* Include New counter to the collection list */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Name = %s\n"),
                         bcmdrd_pt_sid_to_name(unit, cinfo.sid)));
            SHR_ALLOC(ctr_list, sizeof(col_ctr_list_t), "bcmptmCciListNode");
            SHR_NULL_CHECK(ctr_list, SHR_E_MEMORY);
            sal_memset(ctr_list , 0, sizeof(col_ctr_list_t));
            ctr_list->next = *ctc_col_list;
            *ctc_col_list = ctr_list;
            ctr_list->tbl_inst = cinfo.tbl_inst;
            ctr_list->sid = cinfo.sid;
            ctr_list->pipe_map = cinfo.pipe_map;
            ctr_list->ctr_head = NULL;
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Counter table = %s tbl_inst %d\n"),
                       bcmdrd_pt_sid_to_name(unit, ctr_list->sid),
                       ctr_list->tbl_inst));
            addr_shift = bcmptm_pt_cci_reg_addr_gap_get(unit, cinfo.sid);
            new_block = bcmptm_pt_cci_reg_new_block(unit, cinfo.sid);

            if ((cinfo.offset == 0) ||
                (cinfo.offset > offset + (1 << addr_shift)) ||
                /* TM counter regs has different NUMELS so keep separate block */
                (new_block == TRUE) || (ctr_type == CCI_CTR_TYPE_TM)) {
                block = NULL;
                SHR_ALLOC(block, sizeof(col_ctr_block_t), "bcmptmCciCtrBlock");
                SHR_NULL_CHECK(block, SHR_E_MEMORY);
                sal_memset(block, 0, sizeof(col_ctr_block_t));
                block->next = ctr_list->ctr_head;
                ctr_list->ctr_head = block;
                block->min_mapid = i;
                block->max_mapid = i;
                block->min_offset = cinfo.offset;
                /* TM counter regs have non zero NUMELS. */
                if (ctr_type == CCI_CTR_TYPE_TM) {
                    block->index_min = cinfo.index_min;
                    block->index_max = cinfo.index_max;
                }
                block->wsize = cinfo.pt_entry_wsize *
                                (cinfo.index_max - cinfo.index_min + 1);
                block->addr_shift = addr_shift;
                block->pt_entry_wsize = cinfo.pt_entry_wsize;
                block->tbl_inst_max = cinfo.tbl_inst;
                offset = cinfo.offset;
            } else {
                if (block) {
                    block->max_mapid = i;
                    block->wsize += cinfo.pt_entry_wsize;
                    offset = cinfo.offset;
                    block->max_offset = offset;
                }
            }
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Name = %s block %p\n"),
                         bcmdrd_pt_sid_to_name(unit, cinfo.sid),
                         (void *)block));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
col_poll_ctr_reg_cleanup(int unit,
                         int ctr_type,
                         cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = NULL;
    col_ctr_list_t *ctr_list = NULL;
    col_ctr_list_t **ctr_col = NULL;


    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;
    ctr_col = &ctc->ctr_reg_col_list[ctr_type];

    ctr_list = *ctr_col;
    while (ctr_list) {
        /* Free collection meta data */
        col_ctr_block_t *ctr_head = ctr_list->ctr_head;
        while (ctr_head) {
            ctr_list->ctr_head = ctr_head->next;
            SHR_FREE(ctr_head);
            ctr_head = ctr_list->ctr_head;
        }
        ctr_list->tbl_inst = 0;
        *ctr_col = ctr_list->next;
        SHR_FREE(ctr_list);
        ctr_list = *ctr_col;
    }
    *ctr_col = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the counter memory. collection meta data
 */
static int
col_poll_ctr_mem_init(int unit,
                      int ctr_type,
                      cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    uint32_t  maxid, i, k;
    bcmptm_cci_cache_ctr_info_t cinfo;
    size_t wsize = 0;
    col_ctr_block_t *block = NULL;
    int inst  = 1;

    SHR_FUNC_ENTER(unit);
    /* Get maximum map ID */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_ctr_map_size(unit, ctc->hcache, &maxid));

    /* find the number of Port memory table symbols */
    for (i = 0; i < maxid; i++) {
        col_ctr_list_t *ctr_list = NULL;
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_ctr_info_get(unit, ctc->hcache, i, &cinfo));
        if ((cinfo.ctrtype == ctr_type) && cinfo.is_mem) {
            if (bcmptm_pt_cci_ctr_is_bypass(unit, cinfo.sid)) {
                LOG_DEBUG(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "IP-EP bypass=%s\n"),
                     bcmdrd_pt_sid_to_name(unit, cinfo.sid)));
                continue;
            }
            /* Include New counter to the collection list */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Name = %s\n"),
                         bcmdrd_pt_sid_to_name(unit, cinfo.sid)));

            SHR_ALLOC(ctr_list, sizeof(col_ctr_list_t), "bcmptmCciListNode");
            SHR_NULL_CHECK(ctr_list, SHR_E_MEMORY);
            sal_memset(ctr_list , 0, sizeof(col_ctr_list_t));
            ctr_list->next = ctc->ctr_mem_col_list[ctr_type];
            ctc->ctr_mem_col_list[ctr_type] = ctr_list;
            ctr_list->tbl_inst = cinfo.tbl_inst;
            ctr_list->sid = cinfo.sid;
            ctr_list->pipe_map = cinfo.pipe_map;
            ctr_list->ctr_head = NULL;
            /* Get valid tbl instance */
            if (ctr_type != CCI_CTR_TYPE_PORT) {
                int j;
                for (j = 0; j < ctr_list->tbl_inst; j++) {
                     if (IS_PIPE_ENABLE(ctr_list->pipe_map, j)) {
                         inst = j;
                         break;
                     }
                }
            }
            /* Prepare collection meta data */
            for (k = cinfo.index_min; k <= cinfo.index_max; k++) {
                if (bcmptm_pt_cci_index_valid(unit, cinfo.sid, inst, k)) {
                    LOG_DEBUG(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "Valid: Name = %s index =%d\n"),
                               bcmdrd_pt_sid_to_name(unit, cinfo.sid), k));
                    if (!block) {
                        SHR_ALLOC(block, sizeof(col_ctr_block_t), "bcmptmCciCtrBlock");
                        SHR_NULL_CHECK(block, SHR_E_MEMORY);
                        sal_memset(block, 0, sizeof(col_ctr_block_t));
                        block->next = ctr_list->ctr_head;
                        ctr_list->ctr_head = block;
                        /* Initialize collection meta data */
                        block->min_mapid = i;
                        block->max_mapid = i;
                        block->min_offset = cinfo.offset;
                        block->max_offset = cinfo.offset;
                        block->addr_shift = 0;
                        block->pt_entry_wsize = cinfo.pt_entry_wsize;
                        block->tbl_inst_max = cinfo.tbl_inst;
                        block->index_min = k;
                        block->index_max = k;
                        block->next = NULL;
                    } else {
                        block->index_max = k;
                    }
                } else {
                    /* Discontinuity, Table has holes, create new block*/
                    LOG_DEBUG(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "Invalid: Name = %s, index =%d\n"),
                               bcmdrd_pt_sid_to_name(unit, cinfo.sid), k));
                    if (block) {
                        block->wsize = cinfo.pt_entry_wsize *
                                (block->index_max - block->index_min + 1);
                        block = NULL;
                    }
                }
            }
            /* store the size of the table instance */
            if (block) {
                block->wsize = cinfo.pt_entry_wsize * \
                           (block->index_max - block->index_min + 1);
                if ((block->wsize * block->pt_entry_wsize) > wsize) {
                    wsize = block->wsize * block->pt_entry_wsize;
                }
                block = NULL;
            }
        }
    }

    if (wsize > ctc->wsize_buf) {
        ctc->wsize_buf = wsize;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Cleanup mem counter collection meta data
 */
static int
col_poll_ctr_mem_cleanup(int unit,
                         int ctr_type,
                         cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = NULL;
    col_ctr_list_t *ctr_list = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;
    ctr_list = ctc->ctr_mem_col_list[ctr_type];

    while (ctr_list) {
        /* Free collection meta data */
        col_ctr_block_t *ctr_head = ctr_list->ctr_head;
        while (ctr_head) {
            ctr_list->ctr_head = ctr_head->next;
            SHR_FREE(ctr_head);
            ctr_head = ctr_list->ctr_head;
        }
        ctr_list->tbl_inst = 0;
        ctc->ctr_mem_col_list[ctr_type] = ctr_list->next;
        SHR_FREE(ctr_list);
        ctr_list = ctc->ctr_mem_col_list[ctr_type];
    }
    ctc->ctr_mem_col_list[ctr_type] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Cleanup the port mem counter collection meta data
 */
static int
col_poll_port_mem_cleanup(int unit,
                          cci_handle_t handle)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (col_poll_ctr_mem_cleanup(unit, CCI_CTR_TYPE_PORT, handle));
exit:
    SHR_FUNC_EXIT();
}


/*!
 * Initialize the Port memory counter memory collection meta data
 */
static int
col_poll_ctr_port_mem_init(int unit,
                           cci_handle_t handle)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (col_poll_ctr_mem_init(unit, CCI_CTR_TYPE_PORT, handle));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the port reg counter collection meta data
 */
static int
col_poll_ctr_port_init(int unit,
                       cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    size_t wsize = 0;
    col_ctr_block_t *port_blk = NULL;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(col_poll_ctr_port_reg_init(unit, handle));

    SHR_IF_ERR_EXIT(col_poll_ctr_port_mem_init(unit, handle));

    /* Find the max buffer size needed */
    for (i = 0; i < ctc->num_port_reg_blk; i++) {
        port_blk = &ctc->port_reg_blk[i];
        wsize += port_blk->wsize;
    }

    if (wsize > ctc->wsize_buf) {
        ctc->wsize_buf = wsize;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Cleanup the port collection meta data
 */
static void
col_poll_ctr_port_cleanup(int unit,
                          cci_handle_t handle)
{
    col_poll_port_mem_cleanup(unit, handle);
}

/*
 * Collect the block of contiguous port counters
.*  for specified port Reg using non DMA
 */
static int
col_poll_ctr_port_reg_block_nodma(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache)
{
    col_ctr_block_t *port_blk;
    bcmptm_cci_ctr_info_t info = {0};
    bcmbd_pt_dyn_info_t dyn;
    uint32_t i;
    bcmptm_cci_cache_ctr_info_t cinfo;
    int port, sport, tbl_inst;
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    port_blk = (col_ctr_block_t *)param;
    port = port_blk->param;

    for (i = port_blk->min_mapid; i <= port_blk->max_mapid; i++) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_ctr_info_get(unit, handle, i, &cinfo));
        /* Check if counter type is Port and register */
        if ((cinfo.ctrtype != CCI_CTR_TYPE_PORT) || cinfo.is_mem) {
            continue;
        }
        /* Get information about first SID of counter block */
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_mapid_to_sid(unit, handle,
                                           i, &sid));
        /* Get sport based on sid */
        if (col_poll_lport_to_sport(unit, sid, port, &sport) < 0) {
            continue;
        }
        tbl_inst = bcmdrd_pt_tbl_inst_from_port(unit, sid, sport);
        if (tbl_inst == -1) {
            continue;
        }
        dyn.index = 0;
        dyn.tbl_inst = tbl_inst;
        /* Check if tbl-inst and index are valid */
        if (!bcmptm_cci_cache_mapid_index_valid(
                unit, handle, i, dyn.index, dyn.tbl_inst)) {
            continue;
        }
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SID=%s, port=%d sport=%d\n"),
                   bcmdrd_pt_sid_to_name(unit, sid), port, sport));
        /* Read Counter value using SCHAN PIO */
        SHR_IF_ERR_EXIT
            (bcmbd_pt_read(unit, cinfo.sid, &dyn, NULL,
                           port_blk->vaddr, port_blk->pt_entry_wsize));
        /* Update the cache with value as input parameter */
        if (update_cache) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "update cache port=%d\n"),
                       port));
            info.dyn_info = dyn;
            info.map_id = i;
            info.sid = cinfo.sid;
            SHR_IF_ERR_EXIT
                (bcmptm_cci_cache_update(unit, handle, &info,
                                         port_blk->vaddr,
                                         port_blk->pt_entry_wsize));
        }
   }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Collect the block of contiguous port counters for specified port using SBUS DMA
 */
static int
col_poll_ctr_port_reg_block_dma(int unit,
                                cci_handle_t handle,
                                void *param,
                                bool update_cache)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)param;
    bcmptm_cci_cache_ctr_info_t cinfo;
    bcmptm_cci_ctr_info_t info = {0};
    bcmbd_pt_dyn_info_t dyn;
    uint32_t i, offset;
    bcmbd_sbusdma_work_t *work;
    int tbl_inst, j;
    uint64_t buff_offset;
    uint32_t *vaddr;
    int lport, sport; /* logical port, symbol port */
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);

    /* Execute the DMA work */
    lport = ctc->port_reg_blk[0].param;
    if (ctc->port_reg_work[lport] == NULL) {
        SHR_EXIT();
    }
    work = ctc->port_reg_work[lport];
    if (work->items == 0) {
        SHR_EXIT();
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "lport=%d\n"),
               lport));
    SHR_IF_ERR_EXIT
        (bcmbd_sbusdma_work_execute(unit, work));
    /* Update the cache with values collected using DMA */
    if (update_cache) {
        for (j = 0; j < ctc->num_port_reg_blk; j++) {
            col_ctr_block_t *port_blk = &ctc->port_reg_blk[j];

            SHR_IF_ERR_EXIT
                (bcmptm_cci_cache_mapid_to_sid(
                    unit, ctc->hcache, port_blk->min_mapid, &sid));

            if (col_poll_lport_to_sport(unit, sid, lport, &sport) < 0) {
                continue;
            }
            tbl_inst = bcmdrd_pt_tbl_inst_from_port(unit, sid, sport);
            if (tbl_inst == -1) {
                continue;
            }
            /* Check if port is valid for counter group */
            if (sport >= port_blk->tbl_inst_max) {
                continue;
            }

            buff_offset = 0;
            offset = ~0x0;
            /* Update each counter cache */
            for (i = port_blk->min_mapid; i <= port_blk->max_mapid; i++) {
                SHR_IF_ERR_EXIT
                    (bcmptm_cci_cache_mapid_to_sid(unit, ctc->hcache,
                                                   i, &sid));
                /*
                 * Don't need get tbl_inst again.
                 * It will be same in the same block.
                 */
                dyn.index = 0;
                dyn.tbl_inst = tbl_inst;
                info.dyn_info = dyn;
                /* Check if tbl-inst and index are valid */
                if (!bcmptm_cci_cache_mapid_index_valid(
                    unit, handle, i, dyn.index, dyn.tbl_inst)) {
                    continue;
                }

                SHR_IF_ERR_EXIT
                    (bcmptm_cci_cache_ctr_info_get(unit, handle, i, &cinfo));
                if (cinfo.offset != offset) {
                    info.map_id = i;
                    vaddr = &port_blk->vaddr[buff_offset];
                    SHR_IF_ERR_EXIT
                        (bcmptm_cci_cache_update(unit, handle, &info,
                                                 vaddr,
                                                 port_blk->pt_entry_wsize));
                    buff_offset += port_blk->pt_entry_wsize;
                    offset = cinfo.offset;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect all port reg. counters for all active ports using DMA
 */
static int
col_poll_ctr_port_reg_dma(int unit,
                      cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    int i;
    uint64_t buff_offset = 0;
    int lport;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    if (ctc->num_port_reg_blk <= 0) {
        SHR_EXIT();
    }
    /* Iterate over all enabled ports */
    BCMDRD_PBMP_ITER(ctc->config.pbmp, lport) {
        /* No collection if not mapped to logical port */
        buff_offset = 0;
        if (ctc->port_reg_work[lport] != NULL) {
            for (i = 0; i < ctc->num_port_reg_blk; i++) {
                col_ctr_block_t *port_blk = &ctc->port_reg_blk[i];
                port_blk->vaddr = &ctc->vaddr_buf[buff_offset];
                port_blk->param = lport;
                /* update the offset for port block */
                buff_offset += port_blk->wsize;
            }

            /* Collect the couter and update the Counter cache */
            SHR_IF_ERR_EXIT
                (bcmptm_cci_cache_hw_sync(unit, ctc->hcache,
                                          col_poll_ctr_port_reg_block_dma,
                                          ctc, TRUE));
            if (shr_thread_stopping(ctc->th)) {
                SHR_EXIT();
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect the port counter table
.*  for specified port Mem using non DMA
 */
static int
col_poll_ctr_port_mem_block_dma(int unit,
                                cci_handle_t handle,
                                void *param,
                                bool update_cache)
{
    bcmptm_cci_ctr_info_t info = {0};
    bcmbd_pt_dyn_info_t dyn;
    col_ctr_work_t *ctr_work = NULL;
    bcmbd_sbusdma_work_t *work = NULL;
    int i;
    int index_min, index_max;
    bcmdrd_sid_t sid;
    uint32_t *vaddr = NULL;
    int tbl_inst;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    ctr_work = (col_ctr_work_t *)param;
    work = &ctr_work->sbusdma_work;
    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_mapid_to_sid(unit, handle,
                                       ctr_work->mapid, &sid));

    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_frmt_index_from_port(unit, sid, ctr_work->tbl_inst,
                                            &index_min, &index_max));
    tbl_inst = bcmdrd_pt_tbl_inst_from_port(unit, sid, ctr_work->tbl_inst);
    if (work->items) {
        SHR_IF_ERR_EXIT
            (bcmbd_sbusdma_work_execute(unit, work));
        /* Update the cache with values collected using DMA */
        if (update_cache) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "SID=%s, port=%d, tbl_inst=%d "
                                  "index_min=%u, index_max=%u entry_size=%u\n"),
                       bcmdrd_pt_sid_to_name(unit, sid), ctr_work->port,
                       ctr_work->tbl_inst, index_min, index_max,
                       ctr_work->pt_entry_wsize));
            vaddr = ctr_work->vaddr;
            for (i = index_min; i <= index_max; i++) {
                dyn.index = i;
                dyn.tbl_inst = tbl_inst;
                /* Check if tbl-inst and index are valid */
                if (bcmptm_cci_cache_mapid_index_valid(
                    unit, handle, ctr_work->mapid, dyn.index, dyn.tbl_inst)) {
                    info.dyn_info = dyn;
                    info.map_id = ctr_work->mapid;
                    info.sid = sid;
                    SHR_IF_ERR_EXIT
                        (bcmptm_cci_cache_update(unit, handle, &info,
                                                 vaddr,
                                                 ctr_work->pt_entry_wsize));
                    vaddr += ctr_work->pt_entry_wsize;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect all port mem counters for all active ports using DMA
 */
static int
col_poll_ctr_port_mem_dma(int unit,
                            cci_handle_t handle)
{
    col_thread_ctrl_t *ctc;
    col_ctr_list_t *port_mem;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    ctc = (col_thread_ctrl_t *)handle;
    port_mem = ctc->ctr_mem_col_list[CCI_CTR_TYPE_PORT];

    while (port_mem) {
        col_ctr_work_t *ctr_work = port_mem->dma_work;
        /* Iterate over all enabled ports */
        while (ctr_work) {
            ctr_work->vaddr = ctc->vaddr_buf;
            /* Collect the couter and update the Counter cache */
            SHR_IF_ERR_EXIT
                (bcmptm_cci_cache_hw_sync(unit, ctc->hcache,
                                          col_poll_ctr_port_mem_block_dma,
                                          ctr_work, TRUE));
            if (shr_thread_stopping(ctc->th)) {
                SHR_EXIT();
            }
            ctr_work = ctr_work->next;
        }
        port_mem = port_mem->next;
    }
exit:
    SHR_FUNC_EXIT();
}


/*
 * Collect all port reg counters for all active ports using SCHAN PIO
 */
static int
col_poll_ctr_port_reg_nodma(int unit,
                            cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    int i;
    int lport;
    int sport;
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    if (ctc->num_port_reg_blk <= 0) {
        SHR_EXIT();
    }
    /* Iterate over all enabled ports */
    BCMDRD_PBMP_ITER(ctc->config.pbmp, lport) {
        for (i = 0; i < ctc->num_port_reg_blk; i++) {
            col_ctr_block_t *port_blk = &ctc->port_reg_blk[i];
            SHR_IF_ERR_EXIT
                (bcmptm_cci_cache_mapid_to_sid(
                    unit, ctc->hcache, port_blk->min_mapid, &sid));

            if (col_poll_lport_to_sport(unit, sid, lport, &sport) < 0) {
                continue;
            }
            if (bcmdrd_pt_tbl_inst_from_port(unit, sid, sport) < 0) {
                continue;
            }
            /* Check if port is valid for counter group */
            if (sport >= port_blk->tbl_inst_max) {
                continue;
            }
            port_blk->vaddr = ctc->vaddr_buf;
            port_blk->param = lport;
            /* Collect the couter and update the Counter cache */
            SHR_IF_ERR_EXIT
                (bcmptm_cci_cache_hw_sync(unit, ctc->hcache,
                                          col_poll_ctr_port_reg_block_nodma,
                                          port_blk, TRUE));
            if (shr_thread_stopping(ctc->th)) {
                SHR_EXIT();
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect the port counter table
.*  for specified entry using non DMA
 */
static int
col_poll_ctr_port_mem_entry_nodma(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache)
{
    col_ctr_block_t *port_blk = NULL;
    bcmptm_cci_ctr_info_t info = {0};
    bcmbd_pt_dyn_info_t dyn;
    bcmdrd_sid_t sid;
    int sport;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    port_blk = (col_ctr_block_t *)param;

    dyn.index = port_blk->index;
    sid = port_blk->sid;
    SHR_IF_ERR_EXIT
        (col_poll_lport_to_sport(unit, sid, port_blk->param, &sport));

    dyn.tbl_inst = bcmdrd_pt_tbl_inst_from_port(unit, sid, sport);
    if (dyn.tbl_inst < 0) {
        SHR_EXIT();
    }
    /* Check if tbl-inst and index are valid */
    if (bcmptm_cci_cache_mapid_index_valid(
        unit, handle, port_blk->min_mapid, dyn.index, dyn.tbl_inst)) {
        /* Read Counter value using SCHAN PIO */
        SHR_IF_ERR_EXIT
            (bcmbd_pt_read(unit, sid, &dyn, NULL,
                           port_blk->vaddr, port_blk->pt_entry_wsize));
        /* Update the cache with value as input parameter */
        if (update_cache) {
            info.dyn_info = dyn;
            info.map_id = port_blk->min_mapid;
            info.sid = sid;
            SHR_IF_ERR_EXIT
                (bcmptm_cci_cache_update(unit, handle, &info,
                                         port_blk->vaddr,
                                         port_blk->pt_entry_wsize));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect the port counter table
.*  for specified port Mem block using non DMA
 */
static int
col_poll_ctr_port_mem_block_nodma(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache)
{
    col_ctr_block_t *port_blk = NULL;
    col_thread_ctrl_t *ctc;
    int i;
    int index_min, index_max;
    bcmdrd_sid_t sid;
    int sport;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;
    port_blk = (col_ctr_block_t *)param;

    sid = port_blk->sid;
    SHR_IF_ERR_EXIT
        (col_poll_lport_to_sport(unit, sid, port_blk->param, &sport));

    if (bcmdrd_pt_tbl_inst_from_port(unit, sid, sport) < 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_frmt_index_from_port(unit, sid, sport,
                                            &index_min, &index_max));
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "SID=%s, port=%d sport=%d "
                          "index_min=%u, index_max=%u entry_size=%u\n"),
               bcmdrd_pt_sid_to_name(unit, sid),
               port_blk->param, sport, index_min, index_max,
               port_blk->pt_entry_wsize));

    for (i = index_min; i <= index_max; i++) {
        port_blk->index = i;
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_hw_sync(unit, ctc->hcache,
                                      col_poll_ctr_port_mem_entry_nodma,
                                      port_blk, update_cache));
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect all port mem counters for all active ports using SCHAN PIO
 */
static int
col_poll_ctr_port_mem_nodma(int unit,
                            cci_handle_t handle)
{
    col_thread_ctrl_t *ctc;
    int lport;
    bcmdrd_sid_t sid;
    col_ctr_list_t *port_mem;
    int rv, sport;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    ctc = (col_thread_ctrl_t *)handle;

    /* Iterate over all enabled ports */
    BCMDRD_PBMP_ITER(ctc->config.pbmp, lport) {
        port_mem = ctc->ctr_mem_col_list[CCI_CTR_TYPE_PORT];
        while (port_mem) {
            col_ctr_block_t *port_blk = port_mem->ctr_head;
            SHR_IF_ERR_EXIT
                (bcmptm_cci_cache_mapid_to_sid(unit, ctc->hcache,
                                               port_blk->min_mapid, &sid));
            rv = col_poll_lport_to_sport(unit, sid, lport, &sport);
            if (SHR_SUCCESS(rv)) {
                port_blk->vaddr = ctc->vaddr_buf;
                port_blk->param = lport;
                port_blk->sid = sid;
                /* Collect the couter and update the Counter cache */
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_port_mem_block_nodma(unit, handle, port_blk,
                                                       TRUE));
            }
            if (shr_thread_stopping(ctc->th)) {
                SHR_EXIT();
            }
            port_mem = port_mem->next;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect the counter entry using non DMA
 */
static int
col_poll_ctr_entry_nodma(int unit,
                              cci_handle_t handle,
                              void *param,
                              bool update_cache)
{
    col_ctr_block_t *block = NULL;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t dyn;
    bcmptm_cci_ctr_info_t info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    block = (col_ctr_block_t *)param;

    dyn.index = block->index;
    dyn.tbl_inst = block->param;
    sid = block->sid;

    /* Read Counter value using SCHAN PIO */
    SHR_IF_ERR_EXIT
        (bcmbd_pt_read(unit, sid, &dyn, NULL,
                       block->vaddr, block->pt_entry_wsize));

    /* Update the cache with value as input parameter */
    if (update_cache) {
        info.dyn_info = dyn;
        info.map_id = block->mapid;
        info.sid = sid;
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_update(unit, handle, &info,
                                     block->vaddr,
                                     block->pt_entry_wsize));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect the eviction counter block using non DMA
 */
static int
col_poll_ctr_evict_block_nodma(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache)
{
    col_ctr_block_t *block = NULL;
    col_thread_ctrl_t *ctc;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    block = (col_ctr_block_t *)param;
    ctc = (col_thread_ctrl_t *)handle;

    for (i = block->index_min; i <= block->index_max; i++) {
        block->index = i;
        block->mapid = block->min_mapid;
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_hw_sync(unit, ctc->hcache,
                                      col_poll_ctr_entry_nodma,
                                      param, update_cache));
        if (shr_thread_stopping(ctc->th)) {
            SHR_EXIT();
        }
    }
exit:
    SHR_FUNC_EXIT();
}


/*
 * Collect eviction counters using SCHAN PIO
 */
static int
col_poll_ctr_evict_nodma(int unit,
                            cci_handle_t handle)
{
    col_thread_ctrl_t *ctc;
    col_ctr_list_t *evict;
    int i;
    bcmptm_cci_pool_state_t state;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    ctc = (col_thread_ctrl_t *)handle;
    evict = ctc->ctr_mem_col_list[CCI_CTR_TYPE_EVICT];

    while (evict) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_flex_counter_pool_get_state(unit, evict->sid, &state));
        /* Check if eviction conter is enabled  */
        if (state == ENABLE) {
            col_ctr_block_t *ctr_head = evict->ctr_head;
            while (ctr_head) {
                for (i = 0; i < evict->tbl_inst; i++) {
                    if (IS_PIPE_ENABLE(evict->pipe_map, i)) {
                        /* Collect the couter and update the Counter cache */
                        ctr_head->param = i;
                        ctr_head->sid = evict->sid;
                        ctr_head->vaddr = ctc->vaddr_buf;
                        SHR_IF_ERR_EXIT
                            (col_poll_ctr_evict_block_nodma(unit, handle, ctr_head,
                                                        TRUE));
                        if (shr_thread_stopping(ctc->th)) {
                            SHR_EXIT();
                        }
                    }
                }
                ctr_head = ctr_head->next;

            }
        }
        evict = evict->next;
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * Called when number of evicted counter thresh is reached
 */
static void
col_fifodma_intr_cb(int unit, void *cb_data)
{
    col_thread_ctrl_t  *thread_handle;
    col_thread_msg_t msg;

    thread_handle = (col_thread_ctrl_t *)cb_data;

    if (thread_handle) {
        msg.cmd = MSG_CMD_FIFODMA_REQ;
        msg.data[0] = sal_time_usecs();
        if (sal_msgq_post(thread_handle->msgq_hdl, &msg, 0,
                          MSGQ_TIME_OUT) == SAL_MSGQ_E_TIMEOUT) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "[MSG_CMD_FIFODMA_REQ] message timeout\n")));
        }
    }

}

/*!
 * Process the DMA work items
 */
static int
col_evict_fifodma_work_proc(int unit,
                                cci_handle_t handle)
{
    col_thread_ctrl_t  *ctc;
    col_fifodma_work_t *fifodma_work;
    bcmptm_cci_ctr_info_t info = {0};
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    uint32_t offset = 0;
    uint32_t num = 0;
    uint32_t num_entries = 0;
    uint32_t *evict;
    int rv;
    col_thread_instr_data_t *instr_data;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    ctc = (col_thread_ctrl_t *)handle;
    fifodma_work = &ctc->fifodma_work;
    instr_data = &ctc->instr_data;

    /* Return is FIFO DMA is not active */
    if (!fifodma_work->start) {
         SHR_EXIT();
    }
    num_entries = bcmptm_pt_cci_ctr_evict_fifo_buf_size_get(unit);;
    fifodma_work->ticks = 0;
    sal_memset(&info, 0, sizeof(bcmptm_cci_ctr_info_t));

    /* Get evicted entries */
    rv = bcmbd_fifodma_pop(unit, fifodma_work->chan, 0, &offset, &num);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_EMPTY);

    if (num > 0) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "FIFO DMA: Evicted entries = %u, offset = %u\n"),
                   num, offset));
    }
    instr_data->ctr_evict_num += num;

    if (SHR_SUCCESS(rv)) {
        /* Process evicted entries */
        while (num--) {
            evict = fifodma_work->buffer +
                     (offset * fifodma_work->entry_wsize);
            offset = (offset + 1) % num_entries;
            /* Ignore the entires with error */
            rv = bcmptm_pt_cci_ctr_evict_fifo_entry_get(unit,
                                                        evict,
                                                        ctc->vaddr_buf,
                                                        &info);
            if (SHR_FAILURE(rv)) {
                continue;
            }
            rv = bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, info.sid,
                                             &ptcache_ctr_info);
            if (SHR_FAILURE(rv)) {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Invalid pool symbol = %u\n"),
                           info.sid));
                continue;
            }
            info.map_id = ptcache_ctr_info.cci_map_id;
            rv = bcmptm_cci_cache_update(unit, ctc->hcache, &info,
                                         ctc->vaddr_buf, ctc->wsize_buf);
            if (SHR_FAILURE(rv)) {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Error in cache update = %s\n"),
                           bcmdrd_pt_sid_to_name(unit, info.sid)));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the DMA work items
 */
static int
col_evict_fifodma_init(int unit,
                          cci_handle_t handle)
{
    int rv;
    col_thread_ctrl_t  *ctc;
    col_fifodma_work_t *fifodma_work;
    bcmbd_fifodma_work_t *evict_work;
    bcmbd_fifodma_data_t *evict_data;
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_sym_info_t sinfo = {0};
    int blktype;
    bcmdrd_sid_t sid;
    uint32_t acctype;
    uint32_t offset;
    int index;
    int blknum;
    int lane = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    ctc = (col_thread_ctrl_t *)handle;
    fifodma_work = &ctc->fifodma_work;
    sal_memset(fifodma_work, 0, sizeof(col_fifodma_work_t));
    evict_work = &fifodma_work->evict_work;
    sal_memset(evict_work, 0, sizeof(bcmbd_fifodma_work_t));
    evict_data = &fifodma_work->evict_data;
    sal_memset(evict_data, 0, sizeof(bcmbd_fifodma_data_t));

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (!cinfo) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

    rv = bcmptm_pt_cci_ctr_evict_fifo_sym_get(unit, &sid);
    /* Exit with out error if FIFO DMA channel is unavailable */
    if (rv == SHR_E_UNAVAIL) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT(
        bcmdrd_pt_info_get(unit, sid, &sinfo));

    /* Initialize FIFO DMA work */
    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    acctype = BCMDRD_SYM_INFO_ACCTYPE(sinfo.blktypes);
    offset = sinfo.offset;
    index = sinfo.index_min;
    blknum = bcmdrd_chip_block_number(cinfo, blktype, 0);
    if (sinfo.flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        lane = -1;
    }

    evict_data->start_addr = bcmbd_block_port_addr(unit, blknum, lane,
                                                   offset, index);
    evict_data->data_width = sinfo.entry_wsize;
    evict_data->op_code = schan_opcode_set(unit, FIFO_POP_CMD_MSG,
                                           blknum, acctype,
                                           sinfo.entry_wsize * sizeof(uint32_t),
                                           0, 0);
    evict_data->num_entries = bcmptm_pt_cci_ctr_evict_fifo_buf_size_get(unit);
    evict_data->thresh_entries =
             (ctc->config.eviction_threshold * evict_data->num_entries) / 100;
    evict_work->data = evict_data;
    evict_work->cb = col_fifodma_intr_cb;
    evict_work->cb_data = handle;
    evict_work->flags = FIFODMA_WF_INT_MODE;

    fifodma_work->chan = bcmptm_pt_cci_fifodma_chan_get(unit);
    fifodma_work->entry_wsize = sinfo.entry_wsize;
    if (fifodma_work->chan < 0) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    /* Enable the eviction FIFO */
    SHR_IF_ERR_EXIT(bcmptm_cci_evict_fifo_enable(unit, 1));

    /* Start FIFO DMA */
    SHR_IF_ERR_EXIT(
        bcmbd_fifodma_start(unit, fifodma_work->chan, evict_work));
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Eviction: Chan = %d, Threshold = %u\n"),
               fifodma_work->chan, evict_data->thresh_entries));
    fifodma_work->buffer = evict_work->buf_cache;
    fifodma_work->start = 1;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the counter memory collection meta data
 */
static int
col_poll_ctr_evict_init(int unit,
                        cci_handle_t handle)
{
    size_t size = CTR_EVICT_ENABLE_DEFAULT_MAX;
    bcmdrd_sid_t sid[CTR_EVICT_ENABLE_DEFAULT_MAX];
    size_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (col_poll_ctr_mem_init(unit, CCI_CTR_TYPE_EVICT, handle));

    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_ctr_evict_default_enable_list_get(unit, &size, sid));

    for (i = 0; i < size; i++) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_ctr_evict_enable(unit, sid[i], TRUE, FALSE));
        SHR_IF_ERR_EXIT
            (bcmptm_cci_flex_counter_pool_set_state(unit, sid[i], ENABLE));

    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Cleanup the eviction counter collection meta data
 */
static int
col_poll_ctr_evict_cleanup(int unit,
                           cci_handle_t handle)
{
    col_thread_ctrl_t *ctc;
    col_fifodma_work_t *fifodma_work;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    ctc = (col_thread_ctrl_t *)handle;
    fifodma_work = &ctc->fifodma_work;

    if (fifodma_work->start) {
        SHR_IF_ERR_EXIT
            (bcmbd_fifodma_stop(unit, fifodma_work->chan));
        fifodma_work->start = 0;
    }

    /* Disable the eviction FIFO */
    SHR_IF_ERR_EXIT(bcmptm_cci_evict_fifo_enable(unit, 0));

    SHR_IF_ERR_EXIT(
        col_poll_ctr_mem_cleanup(unit, CCI_CTR_TYPE_EVICT, handle));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Collect the ipep counter block using non DMA
 */
static int
col_poll_ctr_ipep_block_nodma(int unit,
                              cci_handle_t handle,
                              void *param,
                              bool update_cache)
{
    col_ctr_block_t *block = NULL;
    col_thread_ctrl_t *ctc;
    uint32_t i, j;
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;
    block = (col_ctr_block_t *)param;

    for (i = block->min_mapid; i <= block->max_mapid; i++) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_mapid_to_sid(unit, ctc->hcache, i, &sid));
            block->sid = sid;
            block->mapid = i;
        for (j = block->index_min; j <= block->index_max; j++) {
            block->index = j;
            SHR_IF_ERR_EXIT
                (bcmptm_cci_cache_hw_sync(unit, ctc->hcache,
                                          col_poll_ctr_entry_nodma,
                                          block, update_cache));
            if (shr_thread_stopping(ctc->th)) {
                SHR_EXIT();
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
col_poll_ctr_tm_nodma(int unit,
                      cci_handle_t handle,
                      void *param,
                      bool update_cache)
{
    col_ctr_block_t *block = NULL;
    uint32_t map_id, idx;
    bcmdrd_sid_t sid;
    col_thread_ctrl_t *ctc;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(param, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;
    block = (col_ctr_block_t *)param;

    for (map_id = block->min_mapid; map_id <= block->max_mapid; map_id++) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_mapid_to_sid(unit,ctc->hcache, map_id, &sid));
        block->sid = sid;
        block->mapid = map_id;

        for (idx = block->index_min; idx <= block->index_max; idx++) {
            block->index = idx;
            /* Read Counter value using SCHAN PIO. */
            SHR_IF_ERR_EXIT
                (bcmptm_cci_cache_hw_sync(unit, ctc->hcache,
                                          col_poll_ctr_entry_nodma,
                                          block, update_cache));
            if (shr_thread_stopping(ctc->th)) {
                SHR_EXIT();
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Poll counter registes and memories
 *
 * Counter collection thread poll the counter registers and memories
 * for various counter types and updates the cache.
 *
 * \param [in] unit Logical unit number.
 * \param [in] is_mem Memory or registers
 * \param [in] ctr_type Counter type.
 * \param [in] hw_op Function pointer for hardware operations.
 * \param [in] handle CCI handle.
 *
 * \return SHR_E_NONE No error.
 * \return !SHR_E_NONE Error code.
 */
static int
col_poll_ctr_nodma (int unit,
                    bool is_mem,
                    int ctr_type,
                    bcmptm_cci_cache_hw_op_f hw_op,
                    cci_handle_t handle)
{
    col_thread_ctrl_t *ctc;
    col_ctr_list_t *ctr_list;
    int idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(hw_op, SHR_E_PARAM);

    ctc = (col_thread_ctrl_t *) handle;
    if (is_mem) {
        ctr_list = ctc->ctr_mem_col_list[ctr_type];
    } else {
        ctr_list = ctc->ctr_reg_col_list[ctr_type];
    }

    while (ctr_list) {
        col_ctr_block_t *ctr_head = ctr_list->ctr_head;

        while (ctr_head) {
            for (idx = 0; idx < ctr_list->tbl_inst; idx++) {
                /* collect the counter and update the counter cache */
                if (IS_PIPE_ENABLE(ctr_list->pipe_map, idx)) {
                    ctr_head->param = idx;
                    ctr_head->vaddr = ctc->vaddr_buf;
                    SHR_IF_ERR_EXIT
                        (hw_op(unit, handle, ctr_head, TRUE));
                    if (shr_thread_stopping(ctc->th)) {
                        SHR_EXIT();
                    }
                }
            }
            ctr_head = ctr_head->next;
        }
        ctr_list = ctr_list->next;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Initialize the ipep counter collection meta data
 */
static int
col_poll_ctr_ipep_init(int unit,
                       cci_handle_t handle)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (col_poll_ctr_reg_init(unit, CCI_CTR_TYPE_IPIPE, handle));
    SHR_IF_ERR_EXIT
        (col_poll_ctr_reg_init(unit, CCI_CTR_TYPE_EPIPE, handle));

    SHR_IF_ERR_EXIT
        (col_poll_ctr_mem_init(unit, CCI_CTR_TYPE_IPIPE, handle));
    SHR_IF_ERR_EXIT
        (col_poll_ctr_mem_init(unit, CCI_CTR_TYPE_EPIPE, handle));

exit:
    SHR_FUNC_EXIT();
}

static int
col_poll_ctr_ipep_cleanup(int unit,
                          cci_handle_t handle)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (col_poll_ctr_reg_cleanup(unit, CCI_CTR_TYPE_IPIPE, handle));
    SHR_IF_ERR_EXIT
        (col_poll_ctr_reg_cleanup(unit, CCI_CTR_TYPE_EPIPE, handle));
    SHR_IF_ERR_EXIT
        (col_poll_ctr_mem_cleanup(unit, CCI_CTR_TYPE_IPIPE, handle));
    SHR_IF_ERR_EXIT
        (col_poll_ctr_mem_cleanup(unit, CCI_CTR_TYPE_EPIPE, handle));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM counter collection clean up.
 *
 * \param [in] unit Logical unit number.
 * \param [in] handle Counter collection handle.
 *
 * \return SHR_E_NONE No error.
 * \return !SHR_E_NONE Error specific code.
 */
static int
col_poll_ctr_tm_cleanup(int unit,
                        cci_handle_t handle)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (col_poll_ctr_reg_cleanup(unit, CCI_CTR_TYPE_TM, handle));
    SHR_IF_ERR_EXIT
        (col_poll_ctr_mem_cleanup(unit, CCI_CTR_TYPE_TM, handle));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief SEC counter collection clean up.
 *
 * \param [in] unit Logical unit number.
 * \param [in] handle Counter collection handle.
 *
 * \return SHR_E_NONE No error.
 * \return !SHR_E_NONE Error specific code.
 */
static int
col_poll_ctr_sec_cleanup(int unit,
                         cci_handle_t handle)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (col_poll_ctr_reg_cleanup(unit, CCI_CTR_TYPE_SEC, handle));
    SHR_IF_ERR_EXIT
        (col_poll_ctr_mem_cleanup(unit, CCI_CTR_TYPE_SEC, handle));
exit:
    SHR_FUNC_EXIT();
}

static void
col_poll_dump_instr_data(int unit, col_thread_ctrl_t *ctc)
{
    col_thread_instr_data_t *instr_data = &ctc->instr_data;

   LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "CCI- bcmbd_sbusdma_batch_work_create = %u\n"),
             instr_data->dma_work_create_count));
   LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "CCI- bcmbd_sbusdma_batch_work_delete = %u\n"),
             instr_data->dma_work_delete_count));
   LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "Port counter collection time(usec) = %u\n"),
             instr_data->ctr_port_col_time_usec));
   LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "Eviction counter collection time(usec) = %u\n"),
             instr_data->ctr_evict_col_time_usec));
   LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "IPIPE counter collection time(usec) = %u\n"),
             instr_data->ctr_ipipe_col_time_usec));
   LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "EPIPE counter collection time(usec) = %u\n"),
             instr_data->ctr_epipe_col_time_usec));
   LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "TM counter collection time(usec) = %u\n"),
             instr_data->ctr_tm_col_time_usec));
   LOG_INFO(BSL_LOG_MODULE,(BSL_META_U(unit,
                        "Eviction counter FIFO DMA collection time(usec) = %u\n"),
             instr_data->ctr_evict_fifo_col_time_usec));
   LOG_INFO(BSL_LOG_MODULE,(BSL_META_U(unit,
                        "Number of Counter entires Evicted. = %u\n"),
             instr_data->ctr_evict_num));
}

/*
 * Counter collection dispatch function
 */
static int
col_poll_ctr_dispatch(int unit, cci_handle_t handle, bool sync, cci_ctr_type_id type)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    col_fifodma_work_t *fifodma_work = NULL;
    col_thread_instr_data_t *instr_data = &ctc->instr_data;
    sal_usecs_t stime, etime;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    /* Collection is not enabled */
    if ((!ctc->config.col_enable) && (!sync)) {
        SHR_EXIT();
    }

    /* Port Counters */
    if ((type == CCI_CTR_TYPE_PORT) || (type == CCI_CTR_TYPE_NUM)) {
        if (cci_col_time(ctc, CCI_CTR_TYPE_PORT) || sync) {
            stime = sal_time_usecs();
            if (ctc->config.dma) {
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_port_reg_dma(unit, handle));
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_port_mem_dma(unit, handle));
            } else {
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_port_reg_nodma(unit, handle));
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_port_mem_nodma(unit, handle));
            }
            etime = sal_time_usecs();
            instr_data->ctr_port_col_time_usec =  SAL_USECS_SUB(etime, stime);
        }
    }

    /* Eviction Counters */
    fifodma_work = &ctc->fifodma_work;
    /* Force FIFODMA collection */
    if ((type == CCI_CTR_TYPE_EVICT) || (type == CCI_CTR_TYPE_NUM)) {
        if ((ctc->config.dma) && (fifodma_work->start)) {
            /* FIFO DMA forced collection if no Interrupt */
            if ((fifodma_work->ticks >=
                   ctc->config.multiplier[CCI_CTR_TYPE_EVICT]) || sync) {
                col_fifodma_intr_cb(unit, handle);
            } else {
                fifodma_work->ticks++;
            }
        }
        if (cci_col_time(ctc, CCI_CTR_TYPE_EVICT) || sync) {
            stime = sal_time_usecs();
            if (ctc->config.dma) {
                /* Residue (Counters not evicted) collection using SBUSDMA */
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_mem_dma(unit, CCI_CTR_TYPE_EVICT, handle));
            } else {
                SHR_IF_ERR_EXIT(col_poll_ctr_evict_nodma(unit, handle));
            }
            etime = sal_time_usecs();
            instr_data->ctr_evict_col_time_usec =  SAL_USECS_SUB(etime, stime);
        }
    }

    /* IPIPE Counters */
    if ((type == CCI_CTR_TYPE_IPIPE) || (type == CCI_CTR_TYPE_NUM)) {
        if (cci_col_time(ctc, CCI_CTR_TYPE_IPIPE) || sync) {
            stime = sal_time_usecs();
            if (ctc->config.dma) {
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_mem_dma(unit, CCI_CTR_TYPE_IPIPE, handle));
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_reg_dma(unit, CCI_CTR_TYPE_IPIPE, handle));
            } else {
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_nodma(unit, FALSE, CCI_CTR_TYPE_IPIPE,
                                        col_poll_ctr_ipep_block_nodma, handle));
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_nodma(unit, TRUE, CCI_CTR_TYPE_IPIPE,
                                        col_poll_ctr_ipep_block_nodma, handle));
            }
            etime = sal_time_usecs();
            instr_data->ctr_ipipe_col_time_usec =  SAL_USECS_SUB(etime, stime);
        }
    }
    /* EPIPE Counters */
    if ((type == CCI_CTR_TYPE_EPIPE) || (type == CCI_CTR_TYPE_NUM)) {
        if (cci_col_time(ctc, CCI_CTR_TYPE_EPIPE) || sync) {
            stime = sal_time_usecs();
            if (ctc->config.dma) {
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_mem_dma(unit, CCI_CTR_TYPE_EPIPE, handle));
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_reg_dma(unit, CCI_CTR_TYPE_EPIPE, handle));
            } else {
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_nodma(unit, FALSE, CCI_CTR_TYPE_EPIPE,
                                        col_poll_ctr_ipep_block_nodma, handle));
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_nodma(unit, TRUE, CCI_CTR_TYPE_EPIPE,
                                    col_poll_ctr_ipep_block_nodma, handle));
            }
            etime = sal_time_usecs();
            instr_data->ctr_epipe_col_time_usec =  SAL_USECS_SUB(etime, stime);
        }
    }

    /* TM Counters */
    if ((type == CCI_CTR_TYPE_TM) || (type == CCI_CTR_TYPE_NUM)) {
        if (cci_col_time(ctc, CCI_CTR_TYPE_TM) || sync) {
            stime = sal_time_usecs();
            if (ctc->config.dma) {
                /* Counter collection using SBUSDMA. */
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_mem_dma(unit, CCI_CTR_TYPE_TM, handle));
            } else {
                /* Counter collection using SCHAN PIO. */
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_nodma(unit, TRUE, CCI_CTR_TYPE_TM,
                                    col_poll_ctr_tm_nodma, handle));
            }
            /* Counter collection for TM registers. */
            SHR_IF_ERR_EXIT
                (col_poll_ctr_nodma(unit, FALSE, CCI_CTR_TYPE_TM,
                                    col_poll_ctr_tm_nodma, handle));
            etime = sal_time_usecs();
            instr_data->ctr_tm_col_time_usec =  SAL_USECS_SUB(etime, stime);
        }
    }

    /* SEC Counters */
    if ((type == CCI_CTR_TYPE_SEC) || (type == CCI_CTR_TYPE_NUM)) {
        if (cci_col_time(ctc, CCI_CTR_TYPE_SEC) || sync) {
            stime = sal_time_usecs();
            if (ctc->config.dma) {
                /* Counter collection using SBUSDMA. */
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_mem_dma(unit, CCI_CTR_TYPE_SEC, handle));
            } else {
                /* Counter collection using SCHAN PIO. */
                SHR_IF_ERR_EXIT
                    (col_poll_ctr_nodma(unit, TRUE, CCI_CTR_TYPE_SEC,
                                    col_poll_ctr_tm_nodma, handle));
            }
            /* Counter collection for SEC registers. */
            SHR_IF_ERR_EXIT
                (col_poll_ctr_nodma(unit, FALSE, CCI_CTR_TYPE_SEC,
                                    col_poll_ctr_tm_nodma, handle));
            etime = sal_time_usecs();
            instr_data->ctr_tm_col_time_usec =  SAL_USECS_SUB(etime, stime);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Counter collection polling cleanup function
 */
static int
col_poll_cleanup(int unit,
                 col_thread_ctrl_t  *ctc)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctc, SHR_E_PARAM);

    if (ctc->config.dma) {
        bcmdrd_hal_dma_free(unit, ctc->wsize_buf * sizeof(uint32_t),
                            ctc->vaddr_buf, ctc->paddr_buf);
        /* Cleanup DMA for Port Reg batch work */
        col_poll_port_reg_dma_cleanup(unit, ctc);
        /* Cleanup DMA for Port mem batch work */
        col_poll_port_mem_dma_cleanup(unit, ctc);
        /* Cleanup DMA for SEC memory batch work */
        col_poll_ctr_mem_dma_cleanup(unit, CCI_CTR_TYPE_SEC, ctc);
        /* Cleanup Eviction counter mem batch work */
        col_poll_ctr_mem_dma_cleanup(unit, CCI_CTR_TYPE_EVICT, ctc);
        /* Cleanup DMA for TM memory batch work */
        col_poll_ctr_mem_dma_cleanup(unit, CCI_CTR_TYPE_TM, ctc);
        /* Cleanup DMA for IPIPE/EPIPE memory batch work */
        col_poll_ctr_mem_dma_cleanup(unit, CCI_CTR_TYPE_IPIPE, ctc);
        col_poll_ctr_mem_dma_cleanup(unit, CCI_CTR_TYPE_EPIPE, ctc);
        /* Cleanup DMA for IPIPE/EPIPE register batch work */
        col_poll_ctr_reg_dma_cleanup(unit, CCI_CTR_TYPE_IPIPE, ctc);
        col_poll_ctr_reg_dma_cleanup(unit, CCI_CTR_TYPE_EPIPE, ctc);
    } else {
        if (ctc->vaddr_buf) {
            SHR_FREE(ctc->vaddr_buf);
        }
    }
    ctc->vaddr_buf = NULL;
    ctc->paddr_buf = 0;

    /* Clean up port counter collection resources */
    col_poll_ctr_port_cleanup(unit, ctc);
    /* Clean up eviction counter collection resources */
    col_poll_ctr_evict_cleanup(unit, ctc);
    /* Clean up ipep counter collection resources */
    col_poll_ctr_ipep_cleanup(unit, ctc);
    /* Clean up TM counter collection resources */
    col_poll_ctr_tm_cleanup(unit, ctc);
    /* Clean up SEC counter collection resources */
    col_poll_ctr_sec_cleanup(unit, ctc);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize TM counter collection meta data.
 *
 * \param [in] unit Logical unit number.
 * \param [in] handle CCI handle.
 */
static int
col_poll_ctr_tm_init(int unit,
                     cci_handle_t handle)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (col_poll_ctr_reg_init(unit, CCI_CTR_TYPE_TM, handle));

    SHR_IF_ERR_EXIT
        (col_poll_ctr_mem_init(unit, CCI_CTR_TYPE_TM, handle));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize SEC counter collection meta data.
 *
 * \param [in] unit Logical unit number.
 * \param [in] handle CCI handle.
 */
static int
col_poll_ctr_sec_init(int unit,
                     cci_handle_t handle)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (col_poll_ctr_reg_init(unit, CCI_CTR_TYPE_SEC, handle));

    SHR_IF_ERR_EXIT
        (col_poll_ctr_mem_init(unit, CCI_CTR_TYPE_SEC, handle));
exit:
    SHR_FUNC_EXIT();
}


/*
 * Counter collection polling init function
 */
static int
col_poll_init(int unit,
              col_thread_ctrl_t *ctc)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctc, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Initialize Port collection DMA buffers and meta data */
    SHR_IF_ERR_EXIT(col_poll_ctr_port_init(unit, ctc));
    /* Initialize Eviction collection DMA buffers and meta data */
    SHR_IF_ERR_EXIT(col_poll_ctr_evict_init(unit, ctc));
    /* Initialize IPEP collection DMA buffers and meta data */
    SHR_IF_ERR_EXIT(col_poll_ctr_ipep_init(unit, ctc));
    /* Initialize TM collection DMA buffers and meta data. */
    SHR_IF_ERR_EXIT(col_poll_ctr_tm_init(unit, ctc));
    /* Initialize SEC collection DMA buffers and meta data. */
    SHR_IF_ERR_EXIT(col_poll_ctr_sec_init(unit, ctc));

   /* Allocate buffer */
    ctc->vaddr_buf = NULL;

    if (bcmptm_pt_cci_ctr_col_dma_supported(unit)) {
        uint64_t fvalue = 0;
        bcmltd_sid_t lt = DEVICE_CONFIGt;

        ctc->config.dma = 1;
        /* Check if DMA is disabled in DEVICE_CONFIG */
        if (SHR_SUCCESS
                (bcmcfg_field_get(unit, lt, DEVICE_CONFIGt_PIO_BULK_READf,
                                 &fvalue))) {
            if (fvalue) {
                ctc->config.dma = 0;
            }
        }
    } else {
        ctc->config.dma = 0;
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Using [%s] for Counter collection \n"),
               (ctc->config.dma) ? "DMA" : "PIO"));

    if (ctc->wsize_buf == 0) {
        SHR_EXIT();
    }

    if (ctc->config.dma) {
        ctc->vaddr_buf =
            bcmdrd_hal_dma_alloc(unit,
                    ctc->wsize_buf * sizeof(uint32_t),
                    "bcmptmCtrDma", &ctc->paddr_buf);

        SHR_NULL_CHECK(ctc->vaddr_buf, SHR_E_MEMORY);
        /* Initalize DMA for batch work */
        SHR_IF_ERR_EXIT(col_poll_port_reg_dma_init(unit, ctc));
        SHR_IF_ERR_EXIT(col_poll_port_mem_dma_init(unit, ctc));
        SHR_IF_ERR_EXIT(col_evict_fifodma_init(unit, ctc));
        SHR_IF_ERR_EXIT
            (col_poll_ctr_mem_dma_init(unit, CCI_CTR_TYPE_EVICT, ctc));
        SHR_IF_ERR_EXIT
            (col_poll_ctr_mem_dma_init(unit, CCI_CTR_TYPE_TM, ctc));
        SHR_IF_ERR_EXIT
            (col_poll_ctr_mem_dma_init(unit, CCI_CTR_TYPE_IPIPE, ctc));
        SHR_IF_ERR_EXIT
            (col_poll_ctr_mem_dma_init(unit, CCI_CTR_TYPE_EPIPE, ctc));
        SHR_IF_ERR_EXIT
            (col_poll_ctr_mem_dma_init(unit, CCI_CTR_TYPE_SEC, ctc));
        SHR_IF_ERR_EXIT
            (col_poll_ctr_reg_dma_init(unit, CCI_CTR_TYPE_IPIPE, ctc));
        SHR_IF_ERR_EXIT
            (col_poll_ctr_reg_dma_init(unit, CCI_CTR_TYPE_EPIPE, ctc));

    } else {
        SHR_ALLOC(ctc->vaddr_buf,
                  ctc->wsize_buf * sizeof(uint32_t),
                  "bcmptmCtrDma");
        SHR_NULL_CHECK(ctc->vaddr_buf, SHR_E_MEMORY);
    }
    sal_memset(ctc->vaddr_buf, 0,
               (ctc->wsize_buf * sizeof(uint32_t)));

exit:
    SHR_FUNC_EXIT();
}


static int
col_poll_port_dma_cleanup_by_port(int unit,
                                  cci_handle_t handle,
                                  shr_port_t port)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (col_poll_port_reg_dma_cleanup_by_port(unit, handle, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (col_poll_port_mem_dma_cleanup_by_port(unit, handle, port));
    sal_usleep(1000);
exit:
    SHR_FUNC_EXIT();

}

static int
col_poll_port_dma_init_by_port(int unit,
                               cci_handle_t handle,
                               shr_port_t port)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
         (col_poll_port_reg_dma_init_by_port(unit, handle, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (col_poll_port_mem_dma_init_by_port(unit, handle, port));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Send CCI  set hit bit message.
 */

int
bcmptm_cci_col_poll_msg(int unit,
                        col_thread_msg_t *msg,
                        cci_handle_t handle)
{

    cci_col_poll_t *pol = (cci_col_poll_t *)handle;
    col_thread_ctrl_t *ctc;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pol, SHR_E_PARAM);
    SHR_NULL_CHECK(msg, SHR_E_PARAM);

    if (pol->hthreads) {
        ctc = (col_thread_ctrl_t *)pol->hthreads[0];
        SHR_NULL_CHECK(ctc, SHR_E_INIT);
        if (sal_msgq_post(ctc->msgq_hdl, msg, 0,
                          MSGQ_TIME_OUT) == SAL_MSGQ_E_TIMEOUT) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "[%d] message Timeout \n"),
                       msg->cmd));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();

}


/*
 * Set Flex counter hit bit
 */
static int
col_poll_hit_bit_set(int unit,
                     cci_handle_t handle,
                     void *param,
                     bool update_cache)
{
    col_thread_msg_t *msg = (col_thread_msg_t *)param;
    bcmdrd_sid_t sid;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    bcmptm_cci_ctr_info_t info = {0};
    uint32_t entry[4] = {0};
    uint32_t bit;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(msg, SHR_E_PARAM);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    sid = msg->data[0];
    bit = msg->data[3];
    SHR_IF_ERR_EXIT
        (bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, sid,
                                             &ptcache_ctr_info));
    info.dyn_info.index = msg->data[2];
    info.dyn_info.tbl_inst = msg->data[1];
    info.sid = sid;
    info.map_id = ptcache_ctr_info.cci_map_id;
    entry[bit/32] = 1 << (bit % 32);
    if (update_cache) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_update(unit, handle, &info, entry, COUNTOF(entry)));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * Validate move counter table entry.
 */
static int
col_poll_ctr_entry_move_validate(int unit,
                                 cci_handle_t handle,
                                 bcmptm_cci_ctr_info_t *msg_info)
{
    bcmptm_cci_ctr_info_t *src_info = &msg_info[0];
    bcmptm_cci_ctr_info_t *dst_info = &msg_info[1];
    bcmptm_cci_ctr_mode_t src_ctr_mode;
    bcmptm_cci_ctr_mode_t dst_ctr_mode;
    bcmptm_cci_update_mode_t update_mode[3];
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    cci_ctr_type_id type;
    size_t size = 3;
    uint32_t slim_flag = (0x1 << SLIM_MODE);
    uint32_t normal_double_flag = (0x1 << NORMAL_DOUBLE_MODE);


    SHR_FUNC_ENTER(unit);
    /* Check source sid, pipe, index. */
    dyn =  &src_info->dyn_info;
    if (!bcmptm_pt_cci_index_valid(unit, src_info->sid,
                                   dyn->tbl_inst, dyn->index)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT
       (bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER,
                                    src_info->sid, &ptcache_ctr_info));
    src_info->map_id = ptcache_ctr_info.cci_map_id;

    /* Check for source is a flex counter. */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_counter_type_get(unit, handle,
                                           src_info->map_id, &type));

    if (type !=  CCI_CTR_TYPE_EVICT) {
         SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get counter mode. */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_flex_counter_mode_get(unit, src_info->sid,
                                          dyn->index, dyn->tbl_inst,
                                          &src_ctr_mode, update_mode, &size));
    if (src_ctr_mode == SLIM_MODE) {
        src_info->flags = slim_flag;
    }
    if (src_ctr_mode == NORMAL_DOUBLE_MODE) {
        src_info->flags = normal_double_flag;
    }

    /* Check destination sid, pipe, index. */
    dyn =  &dst_info->dyn_info;
    if (!bcmptm_pt_cci_index_valid(unit, dst_info->sid,
                                   dyn->tbl_inst, dyn->index)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, dst_info->sid,
                                    &ptcache_ctr_info));
    dst_info->map_id = ptcache_ctr_info.cci_map_id;

    /* Check for desination is a flex counter. */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_counter_type_get(unit, handle, dst_info->map_id, &type));

    if (type !=  CCI_CTR_TYPE_EVICT) {
         SHR_ERR_EXIT(SHR_E_PARAM);
    }
    size = 3;
    SHR_IF_ERR_EXIT
        (bcmptm_cci_flex_counter_mode_get(unit, dst_info->sid,
                                          dyn->index, dyn->tbl_inst,
                                          &dst_ctr_mode, update_mode, &size));
    if (dst_ctr_mode == SLIM_MODE) {
        dst_info->flags = slim_flag;
    }
    if (dst_ctr_mode == NORMAL_DOUBLE_MODE) {
        dst_info->flags = normal_double_flag;
    }
    /* Check if counter mode of source and destination are same. */
    if (src_ctr_mode != dst_ctr_mode) {
         SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Move counter table entry.
 */
static int
col_poll_ctr_entry_move(int unit,
                        cci_handle_t handle,
                        bcmptm_cci_ctr_info_t *msg_info)
{
    uint32_t entry[4] = {0};
    bcmptm_cci_cache_pt_param_t param;
    bcmbd_pt_dyn_info_t dyn;
    bool slim;
    bcmltd_sid_t rsp_ltid;
    uint32_t rsp_flags;
    uint32_t normal_double = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(msg_info, SHR_E_PARAM);

    /* Validate if move parameters are correct */
    SHR_IF_ERR_EXIT
        (col_poll_ctr_entry_move_validate(unit, handle, msg_info));

    /* Check if counter mode if slim */
    slim = msg_info[0].flags & (0x1 << SLIM_MODE);
    normal_double = msg_info[0].flags & (0x1 << NORMAL_DOUBLE_MODE);
    msg_info[1].flags = msg_info[0].flags = 0;

    /* Sync the hardware and read the source cache */
    msg_info[0].flags = BCMLT_ENT_ATTR_GET_FROM_HW;
    dyn.tbl_inst = msg_info[0].dyn_info.tbl_inst;
    dyn.index = msg_info[0].dyn_info.index;
    msg_info[0].in_pt_dyn_info = &dyn;
    msg_info[0].req_ltid = 0;
    msg_info[0].rsp_ltid = &rsp_ltid;
    msg_info[0].rsp_flags = &rsp_flags;

    /* Read source counter table entry. */
    if (slim) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_slim_ctr_read(unit, &msg_info[0], entry, sizeof(entry)));
     } else if (normal_double) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_normal_double_ctr_read(unit, &msg_info[0], entry, sizeof(entry)));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_ctr_read(unit, &msg_info[0], entry, sizeof(entry)));
    }

    /* Update the cache of destination table entry. */
    param.info = &msg_info[1];
    param.buf = entry;

    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_hw_sync(unit, handle,
                                 bcmptm_cci_cache_lock_write,
                                 &param, slim));

    /* Set source table counter cache and physical table to zero. */
    sal_memset(entry, 0,  sizeof(entry));
    if (slim) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_slim_ctr_write(unit, &msg_info[0], entry));
    } else if (normal_double) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_normal_double_ctr_write(unit, &msg_info[0], entry));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_ctr_write(unit, &msg_info[0], entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Counter collection polling thread function
 */
static void
col_poll_thread(shr_thread_t shr_ctc, cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    col_thread_msg_t msg;
    bool loop = TRUE;
    int unit = ctc->unit;
    shr_port_t port;
    bcmdrd_pbmp_t opbmp, pbmp;
    int rv;
    col_fifodma_work_t *fifodma_work;
    sal_usecs_t stime, etime;
    col_thread_instr_data_t *instr_data = &ctc->instr_data;

    stime = sal_time_usecs();

    while (loop) {
        uint32_t interval;
        if (ctc->config.col_enable) {
            interval = ctc->config.interval;
            etime = sal_time_usecs();
            /* Do Collection if time interval passed */
            if ((etime - stime) > interval) {
                ctc->col_time_tick++;
                col_poll_ctr_dispatch(unit, handle,
                                      FALSE, CCI_CTR_TYPE_NUM);
                stime = sal_time_usecs();
            }
        } else {
            interval = SAL_MSGQ_FOREVER;
        }
        /* Wait for message and process the collection */
        if (sal_msgq_recv(ctc->msgq_hdl, &msg, interval) == SAL_MSGQ_E_TIMEOUT) {
            ctc->col_time_tick++;
            /* No message, do collection */
            col_poll_ctr_dispatch(unit, handle, FALSE, CCI_CTR_TYPE_NUM);
            stime = sal_time_usecs();
        } else {
            switch (msg.cmd) {
            case MSG_CMD_POL_STOP:
                loop = FALSE;
                break;
            case MSG_CMD_FIFODMA_REQ:
                /* FIFO DMA interrupt to collect evicted counters */
                if (SHR_FAILURE(col_evict_fifodma_work_proc(unit, ctc))) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "Error in FIFO DMA work processing\n")));
                }
                instr_data->ctr_evict_fifo_col_time_usec +=
                    SAL_USECS_SUB(sal_time_usecs(), msg.data[0]);
                break;
            /* Configuration message processing */
            case MSG_CMD_POL_CONFIG_COL_EN:
                fifodma_work = &ctc->fifodma_work;
                col_poll_config_copy(unit, &ctc->config, &msg, true);
                if (ctc->config.dma) {
                    if (ctc->config.col_enable) {
                        if (!fifodma_work->start) {
                            rv = col_evict_fifodma_init(unit, ctc);
                            if (SHR_FAILURE(rv)) {
                                LOG_WARN(BSL_LOG_MODULE,
                                         (BSL_META_U(unit,
                                                     "Fail to start FIFO DMA.\n")));
                            }
                        }
                    } else {
                        if (fifodma_work->start) {
                            /* Get the evicted counters */
                            rv = col_evict_fifodma_work_proc(unit, ctc);
                            if (SHR_FAILURE(rv)) {
                                LOG_WARN(BSL_LOG_MODULE,
                                         (BSL_META_U(unit,
                                                     "Error in FIFO DMA work processing\n")));
                            }
                            rv = bcmbd_fifodma_stop(unit, fifodma_work->chan);
                            if (SHR_FAILURE(rv)) {
                                LOG_WARN(BSL_LOG_MODULE,
                                         (BSL_META_U(unit,
                                                     "Fail to stop FIFO DMA.\n")));
                            } else {
                                fifodma_work->start = 0;
                                bcmptm_cci_evict_fifo_enable(unit, 0);
                            }
                        }
                    }
                }
                sal_sem_give(ctc->cnfg_sem);
                break;
            case MSG_CMD_POL_CONFIG_INTERVAL:
            case MSG_CMD_POL_CONFIG_MULTIPLIER_PORT:
            case MSG_CMD_POL_CONFIG_MULTIPLIER_EPIPE:
            case MSG_CMD_POL_CONFIG_MULTIPLIER_IPIPE:
            case MSG_CMD_POL_CONFIG_MULTIPLIER_TM:
            case MSG_CMD_POL_CONFIG_MULTIPLIER_EVICT:
            case MSG_CMD_POL_CONFIG_MULTIPLIER_SEC:
                col_poll_config_copy(unit, &ctc->config, &msg, true);
                sal_sem_give(ctc->cnfg_sem);
                break;
            case MSG_CMD_POL_PORT_ADD:
                if (ctc->config.dma) {
                    port = (shr_port_t)msg.data[0];
                    if (BCMDRD_PBMP_MEMBER(ctc->config.pbmp, port)) {
                        rv = col_poll_port_dma_init_by_port(unit, handle, port);
                        if (SHR_FAILURE(rv)) {
                            LOG_WARN(BSL_LOG_MODULE,
                                     (BSL_META_U(unit,
                                                 "Fail to initialize lport %d dma.\n"),
                                      port));
                        }
                    }
                }
                sal_sem_give(ctc->cnfg_sem);
                break;
            case MSG_CMD_POL_PORT_DEL:
                if (ctc->config.dma) {
                    port = (shr_port_t)msg.data[0];
                    if (BCMDRD_PBMP_MEMBER(ctc->config.pbmp, port)) {
                        rv = col_poll_port_dma_cleanup_by_port(unit, handle, port);
                        if (SHR_FAILURE(rv)) {
                            LOG_WARN(BSL_LOG_MODULE,
                                     (BSL_META_U(unit,
                                                 "Fail to clean up lport %d dma.\n"),
                                      port));
                        }
                    }
                }
                sal_sem_give(ctc->cnfg_sem);
                break;
            case MSG_CMD_POL_CONFIG_PORT:
                BCMDRD_PBMP_ASSIGN(pbmp, ctc->config.pbmp);
                BCMDRD_PBMP_ASSIGN(opbmp, ctc->config.pbmp);
                /* Update configuration */
                col_poll_config_copy(unit, &ctc->config, &msg, true);
                if (ctc->config.dma) {
                    BCMDRD_PBMP_XOR(pbmp, ctc->config.pbmp);
                    /* deleted pbmp */
                    BCMDRD_PBMP_AND(opbmp, pbmp);
                    /* new added pbmp */
                    BCMDRD_PBMP_AND(pbmp, ctc->config.pbmp);
                    BCMDRD_PBMP_ITER(opbmp, port) {
                        rv = col_poll_port_dma_cleanup_by_port(unit, handle, port);
                        if (SHR_FAILURE(rv)) {
                            LOG_WARN(BSL_LOG_MODULE,
                                     (BSL_META_U(unit,
                                                 "Fail to clean up lport %d dma.\n"),
                                      port));
                            continue;
                        }
                    }
                    /* Check new add port */
                    BCMDRD_PBMP_ITER(pbmp, port) {
                        rv = col_poll_port_dma_init_by_port(unit, handle, port);
                        if (SHR_FAILURE(rv)) {
                            LOG_DEBUG(BSL_LOG_MODULE,
                                      (BSL_META_U(unit,
                                                  "Bypass lport %d dma init \n"),
                                       port));
                            continue;
                        }
                    }
                }
                sal_sem_give(ctc->cnfg_sem);
                break;
            case MSG_CMD_EVICTION_THRESHOLD:
                /* Update configuration */
                if (ctc->config.dma) {
                    if (msg.data[0] != ctc->config.eviction_threshold) {
                        fifodma_work = &ctc->fifodma_work;
                        /* Get the evicted counters */
                        rv = col_evict_fifodma_work_proc(unit, ctc);
                        if (SHR_FAILURE(rv)) {
                            LOG_WARN(BSL_LOG_MODULE,
                                     (BSL_META_U(unit,
                                                 "Error in FIFO DMA work processing\n")));
                        }
                        col_poll_config_copy(unit, &ctc->config, &msg, true);
                        LOG_DEBUG(BSL_LOG_MODULE,
                                  (BSL_META_U(unit,
                                              "MSG_CMD_EVICTION_THRESHOLD = %d\n"),
                                   ctc->config.eviction_threshold));
                        /* Restart FIFO DMA */
                        if (fifodma_work->start) {
                            rv = bcmbd_fifodma_stop(unit, fifodma_work->chan);
                            if (SHR_FAILURE(rv)) {
                                LOG_WARN(BSL_LOG_MODULE,
                                         (BSL_META_U(unit,
                                                     "Fail to stop FIFO DMA.\n")));
                            } else {
                                fifodma_work->start = 0;
                                bcmptm_cci_evict_fifo_enable(unit, 0);
                            }
                        }
                        if (fifodma_work->start == 0) {
                            rv = col_evict_fifodma_init(unit, ctc);
                            if (SHR_FAILURE(rv)) {
                                LOG_WARN(BSL_LOG_MODULE,
                                         (BSL_META_U(unit,
                                                     "Fail to start FIFO DMA.\n")));
                            }
                        }
                    }
                }
                sal_sem_give(ctc->cnfg_sem);
                break;
            case MSG_CMD_HIT_BIT_SET:
                rv = bcmptm_cci_cache_hw_sync(unit, ctc->hcache, col_poll_hit_bit_set,
                                              &msg, TRUE);
                if (SHR_FAILURE(rv)) {
                    LOG_WARN(BSL_LOG_MODULE,
                             (BSL_META_U(unit,
                                         "Fail to Set Hit bit.\n")));
                }
                break;
            case  MSG_CMD_POL_STAT_SYNC:
                {
                    cci_ctr_type_id type = msg.data[0];
                    /*Do collection to sync the cache immediatley */
                    col_poll_ctr_dispatch(unit, handle, TRUE, type);
                    rv = bcmptm_cci_stat_sync_complete(unit, type);
                    if (SHR_FAILURE(rv)) {
                        LOG_WARN(BSL_LOG_MODULE,
                                 (BSL_META_U(unit,
                                             "Fail to complete sync.\n")));
                    }
                }
                break;
            case  MSG_CMD_CTR_ENTRY_MOVE:
                {
                    bcmptm_cci_ctr_info_t *msg_info = NULL;
                    sal_memcpy(&msg_info, msg.data, sizeof(bcmptm_cci_ctr_info_t *));
                    rv = col_poll_ctr_entry_move(unit, ctc->hcache, msg_info);
                    if (SHR_FAILURE(rv)) {
                        LOG_WARN(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "Fail to move entry.\n")));
                    }
                    if (msg_info != NULL) {
                        SHR_FREE(msg_info);
                    }
                }
                break;
            default:
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Invalid Command = %d\n"),
                          msg.cmd));

                break;
            }
        }
        /* Set up timeout */
    }

    sal_sem_give(ctc->wait_sem);

}

/*
 * Disables all the eviction counter pools by default
 */
static int
cci_evict_pool_disable(int unit,
                       cci_handle_t handle)
{
    col_thread_ctrl_t *ctc = (col_thread_ctrl_t *)handle;
    uint32_t  maxid, i;
    bcmptm_cci_cache_ctr_info_t cinfo;

    SHR_FUNC_ENTER(unit);
    /* Get maximum map ID */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_ctr_map_size(unit, ctc->hcache, &maxid));

    /* Find the number of port memory table symbols */
    for (i = 0; i < maxid; i++) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_ctr_info_get(unit, ctc->hcache, i, &cinfo));
        /* Disable eviction counters.*/
        if (cinfo.ctrtype == CCI_CTR_TYPE_EVICT) {
            SHR_IF_ERR_EXIT
               (bcmptm_cci_ctr_evict_enable(unit, cinfo.sid, FALSE, FALSE));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * Counter collection polling thread init function
 */
static int
col_poll_thread_init(int unit,
                     cci_col_poll_t *col_pol,
                     int prio,
                     cci_handle_t *handle)
{
    col_thread_ctrl_t *ctc = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(col_pol, SHR_E_PARAM);

    SHR_ALLOC(ctc, sizeof(col_thread_ctrl_t), "bcmptmCciPollCol");
    SHR_NULL_CHECK(ctc, SHR_E_MEMORY);
    sal_memset(ctc, 0, sizeof(col_thread_ctrl_t));
    *handle = ctc;

    /* Create Message  Queue */
    ctc->msgq_hdl =  sal_msgq_create(sizeof(col_thread_msg_t),
                                      COL_POLL_MSGQ_SIZE, "bcmptmCciPollQueue");
    SHR_NULL_CHECK(ctc->msgq_hdl, SHR_E_MEMORY);

    /* Create Wait Semaphore */
    ctc->wait_sem = sal_sem_create("BcmPtmCciWaitSem", SAL_SEM_BINARY, 0);
    SHR_NULL_CHECK(ctc->wait_sem, SHR_E_MEMORY);
    /* Create config Semaphore */
    ctc->cnfg_sem = sal_sem_create("BcmPtmCciCnfgSem", SAL_SEM_BINARY, 0);
    SHR_NULL_CHECK(ctc->cnfg_sem, SHR_E_MEMORY);

    ctc->unit = unit;
    ctc->col_pol = col_pol;

    /* Get handle of CCI context */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_handle_get(unit, ctc->col_pol->parent, &ctc->cci));

    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_handle_get(unit, ctc->cci, &ctc->hcache));

    /* Configure the thread properties */
    sal_memset(&ctc->config, 0, sizeof(cci_config_t));

    SHR_IF_ERR_EXIT
        (bcmptm_cci_config_get(unit, &ctc->config));

    if (!ctc->cci->warm) {
        /* Disable the eviction pools */
        SHR_IF_ERR_EXIT
            (cci_evict_pool_disable(unit, ctc));
    }
    /* Initialize counter collection resources */
    SHR_IF_ERR_EXIT(col_poll_init(unit, ctc));

    /* Create thread */
    ctc->th = shr_thread_start("bcmptmCciPoll", prio,
                               col_poll_thread, ctc);

    if (ctc->th == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        /* Free the allocated resources */
        if (ctc) {
            col_poll_ctr_port_cleanup(unit, ctc);
            if (ctc->wait_sem) {
                sal_sem_destroy(ctc->wait_sem);
                ctc->wait_sem = NULL;
            }
            if (ctc->msgq_hdl) {
                sal_msgq_destroy(ctc->msgq_hdl);
                ctc->msgq_hdl = NULL;
            }
            SHR_FREE(ctc);
        }
    }
    SHR_FUNC_EXIT();

}

/*
 * Counter collection polling thread cleanup function
 */
static int
col_poll_thread_cleanup(int unit, cci_col_poll_t *col_pol,
                        cci_handle_t handle)
{
    col_thread_ctrl_t *ctc;
    col_thread_msg_t msg;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    ctc = (col_thread_ctrl_t *)handle;

    /* Signal the thread and wait for it to exit */
    if (ctc->msgq_hdl) {
        SHR_IF_ERR_EXIT
            (shr_thread_stop(ctc->th, 0));
        msg.cmd = MSG_CMD_POL_STOP;
        if (sal_msgq_post(ctc->msgq_hdl, &msg, 0,
                          MSGQ_TIME_OUT) == SAL_MSGQ_E_TIMEOUT) {
            SHR_IF_ERR_EXIT(SHR_E_TIMEOUT);
        }
        /* Wait for thread to wake up */
        if (sal_sem_take(ctc->wait_sem, CFG_CCI_TIME_OUT) == -1) {
            SHR_IF_ERR_EXIT(SHR_E_TIMEOUT);
        }
    }
    SHR_IF_ERR_EXIT
        (shr_thread_stop(ctc->th, CFG_CCI_TIME_OUT));

    /* Cleanup counter collection resources */
    SHR_IF_ERR_EXIT(col_poll_cleanup(unit, ctc));

    /* Destroy wait semaphore */
    if (ctc->wait_sem) {
        sal_sem_destroy(ctc->wait_sem);
    }

    /* Destroy config semaphore */
    if (ctc->cnfg_sem) {
        sal_sem_destroy(ctc->cnfg_sem);
    }

    /* Destroy Message Queue handle */
    if (ctc->msgq_hdl) {
        sal_msgq_destroy(ctc->msgq_hdl);
    }

    /* Countr collection instrumentation data */
    col_poll_dump_instr_data(unit, ctc);

    /* clean up the thread context */
    SHR_FREE(handle);
exit:
    SHR_FUNC_EXIT();


}

/*
 * Cleans polling threads and frees the thread instance
 */
static int
cci_col_poll_stop(int unit,
                  cci_col_poll_t *pol)
{
    int i;
    int nthreads = pol->nthreads;

    SHR_FUNC_ENTER(unit);
    for (i = 0; i < nthreads; i++) {
        SHR_IF_ERR_EXIT
            (col_poll_thread_cleanup(unit, pol, pol->hthreads[i]));
        pol->nthreads--;
        pol->hthreads[i] = NULL;
    }
    SHR_FREE(pol->hthreads);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize poll based counter collection resources.
 */
int
bcmptm_cci_col_poll_init(int unit,
                         const bcmptm_cci_col_t *con_col,
                         cci_handle_t *handle)
{
    cci_col_poll_t *pol = NULL;
    SHR_FUNC_ENTER(unit);

    /* Allocate the poll base counter collection handle */
    SHR_ALLOC(pol, sizeof(cci_col_poll_t), "bcmptmCciPoll");
    SHR_NULL_CHECK(pol, SHR_E_MEMORY);
    sal_memset(pol, 0, sizeof(cci_col_poll_t));
    pol->parent = con_col;
    *handle = pol;

exit:
    SHR_FUNC_EXIT();

}

/*
 * Cleanup poll based counter collection resources.
 */
int
bcmptm_cci_col_poll_cleanup(int unit,
                            cci_handle_t handle)
{
    cci_col_poll_t   *pol = (cci_col_poll_t *)handle;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pol, SHR_E_PARAM);

    SHR_FREE(pol);

exit:
    SHR_FUNC_EXIT();

}

/*
 * Start poll based counter collection.
 */
extern int
bcmptm_cci_col_poll_run(int unit,
                        cci_handle_t handle)
{
    cci_col_poll_t *pol = (cci_col_poll_t *)handle;
    int tcnt = 1;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pol, SHR_E_PARAM);

    pol->nthreads = 0;

    if (tcnt) {
        SHR_ALLOC(pol->hthreads, sizeof(cci_handle_t) * tcnt,
                                  "bcmptmCciPollThread");
        SHR_NULL_CHECK(pol->hthreads, SHR_E_MEMORY);
        sal_memset(pol->hthreads, 0, sizeof(cci_handle_t) * tcnt);
    }

    /* Initialize threads */
    for (i = 0; i < tcnt; i++) {
        SHR_IF_ERR_EXIT
            (col_poll_thread_init(unit, pol, -1,
                                  &pol->hthreads[i]));
        pol->nthreads++;
    }

exit:
    if (SHR_FUNC_ERR()) {
        cci_col_poll_stop(unit, pol);
    }
    SHR_FUNC_EXIT();
}

/*
 * Stop poll based counter collection.
 */
extern int
bcmptm_cci_col_poll_stop(int unit,
                         cci_handle_t handle)
{
    cci_col_poll_t   *pol = (cci_col_poll_t *)handle;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pol, SHR_E_PARAM);
    SHR_IF_ERR_EXIT
        (cci_col_poll_stop(unit, pol));

 exit:
    SHR_FUNC_EXIT();

}
