/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        sbusdma.h
 * Purpose:     Maps out structures used for SBUSDMA operations and
 *              exports routines.
 */

#ifndef _SOC_SBUSDMA_H
#define _SOC_SBUSDMA_H

#include <soc/defs.h>
#include <soc/types.h>
#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>

#define SOC_SBUSDMA_TYPE_TDMA      0
#define SOC_SBUSDMA_TYPE_SLAM      1
#define SOC_SBUSDMA_TYPE_DESC      2

/* The following define to be used to create software structures only,
   it doesn't represent actual number of channels in CMC hardware.
   This should be the greater of number of channels per CMC in all
   CMIC versions. As of now CMICX has 4 channels vs 3 channels in
   prior CMIC versions, so this is set to 4 */
#define SOC_SBUSDMA_CH_PER_CMC     (4)

/***** Register mode stuff *****/
typedef struct soc_sbusdma_reg_param_s {
    soc_mem_t mem;
    int array_id_start;
    int array_id_end;
    int index_begin;
    int index_end;
    int data_beats;
    int copyno;
    int ser_flags;
    int flags;
    uint8 mem_clear;
    void *buffer;
    int vchan;
}soc_sbusdma_reg_param_t;

extern int
_soc_mem_array_sbusdma_read(int unit, soc_mem_t mem, unsigned array_index,
                            int copyno, int index_min, int index_max,
                            uint32 ser_flags, void *buffer, int vchan);
extern int
_soc_mem_sbusdma_read(int unit, soc_mem_t mem, int copyno,
                      int index_min, int index_max,
                      uint32 ser_flags, void *buffer);
extern int
_soc_mem_array_sbusdma_write(int unit, int flags, soc_mem_t mem,
                             unsigned array_index_start,
                             unsigned array_index_end, int copyno,
                             int index_begin, int index_end, 
                             void *buffer, uint8 mem_clear, 
                             int clear_buf_ent, int vchan);
extern int
_soc_mem_sbusdma_write(int unit, soc_mem_t mem, int copyno,
                       int index_begin, int index_end, 
                       void *buffer, uint8 mem_clear, int clear_buf_ent);
extern int
_soc_mem_sbusdma_clear(int unit, soc_mem_t mem, int copyno, void *null_entry);

extern int
_soc_mem_sbusdma_clear_specific(int unit, soc_mem_t mem,
                                unsigned array_index_min, unsigned array_index_max,
                                int copyno,
                                int index_min, int index_max,
                                void *null_entry);

/***** Descriptor mode stuff *****/
#define SOC_SBUSDMA_CTRL_LAST      (1<<31)
#define SOC_SBUSDMA_CTRL_NOT_LAST ~(1<<31)
#define SOC_SBUSDMA_CTRL_SKIP      (1<<30)
#define SOC_SBUSDMA_CTRL_NOT_SKIP ~(1<<30)
#define SOC_SBUSDMA_CTRL_JUMP      (1<<29)
#define SOC_SBUSDMA_CTRL_NOT_JUMP ~(1<<29)
#define SOC_SBUSDMA_CTRL_APND      (1<<28)
#define SOC_SBUSDMA_CTRL_NOT_APND ~(1<<28)
#define SOC_SBUSDMA_CTRL_DSEC_REMAIN_SHIFT (24)
#define SOC_SBUSDMA_CTRL_DSEC_REMAIN_MASK  (0x0F << 24)


/* Public structures and routines */

typedef int sbusdma_desc_handle_t;
/* status: Non-zero value indicates an error */
typedef void (*soc_sbd_dm_cb_f)(int unit, int status, 
                                sbusdma_desc_handle_t handle, void *data);
typedef void (*soc_sbd_dm_pc_f)(int unit, sbusdma_desc_handle_t handle, void *data);

typedef struct {
/* Used to indicate that the supplied h/w descriptor info should be used 
   and not generated internally by the create routine */
#define SOC_SBUSDMA_CFG_USE_SUPPLIED_DESC 0x001 
/* Used to indicate that this is a h/w managed descriptor */
#define SOC_SBUSDMA_CFG_DESC_HW_MANAGED   0x002
/* Used to indicate that the flags provided in desc_cfg should be used
   instead of the flags in desc_ctrl, this allows having register and memory writes
   in the same descriptor chain. */
#define SOC_SBUSDMA_CFG_USE_FLAGS         0x004
/* Indicates that the opcode in the descriptor contains a write command */
#define SOC_SBUSDMA_WRITE_CMD_MSG         0x010
/* Indicates that the opcode in the descriptor contains a memory read/write */
#define SOC_SBUSDMA_MEMORY_CMD_MSG        0x100
/* Indicates that the counter is implemented as a memory in the h/w */
#define SOC_SBUSDMA_CFG_COUNTER_IS_MEM    0x100
/* Indicates dma to floor */
#define SOC_SBUSDMA_CFG_NO_BUFF           0x200
/* Indicates if prefetch enable is set */
#define SOC_SBUSDMA_CFG_PREFETCH_ENB      0x400
/* Indicates that MOR dma is used */
#define SOC_SBUSDMA_CFG_MOR               0x1000
    uint32 flags;
    char name[16];               /* Descriptor name */
    uint32 cfg_count;            /* Number of items in descriptor chain (1: single descriptor mode) */
    void *hw_desc;               /* Custom hw descriptor info (Optional).
                                    Valid only when SOC_SBUSDMA_CFG_USE_SUPPLIED_DESC is set */
    void *buff;                  /* Common/contiguous host memory buffer for the complete chain */
    soc_sbd_dm_cb_f cb;          /* Callback function for completion */
    void *data;                  /* Caller's data transparently returned in callback */
    soc_sbd_dm_pc_f pc;          /* Preconfig function for specific DMA mode */
    void *pc_data;               /* Caller's data transparently returned in preconfig function */
} soc_sbusdma_desc_ctrl_t;

typedef struct {
    /* NOTE: If hw_desc is used from desc_ctrl then the following
             are ignored: blk, acc_type, addr, width, count, addr_shift */
    int acc_type;      /* access type */
    uint32 blk;        /* Schan block number */
    uint32 addr;       /* Schan addr */
    uint32 width;      /* Counter width */
    uint32 count;      /* Number of counter entries */
    uint32 addr_shift; /* Gap between individual counters (refer h/w spec for details).
                          NOTE: this value should be the same for all counters in a descriptor */
    void *buff;        /* Specific host memory buffer for each descriptor.
                          NOTE: 'buff' is used only if the supplied common/continuous 
                                buffer in desc_ctrl is NULL */
    uint32 flags;      /* Used instead of desc_ctrl flags if the SOC_SBUSDMA_CFG_USE_FLAGS is provided */
    uint32 mem_clear;  /* indicate a mem clear (init full tables with a single entry) */
} soc_sbusdma_desc_cfg_t;

extern int
soc_sbusdma_desc_create(int unit, soc_sbusdma_desc_ctrl_t *ctrl,
                        soc_sbusdma_desc_cfg_t *cfg,
                        uint8 alloc_memory,
                        sbusdma_desc_handle_t *desc_handle);

/* Note: cfg should point to an array with an allocation of at least ctrl->cfg_count entries */
extern int
soc_sbusdma_desc_get(int unit, sbusdma_desc_handle_t desc_handle,
                     soc_sbusdma_desc_ctrl_t *ctrl,
                     soc_sbusdma_desc_cfg_t *cfg);
extern int
soc_sbusdma_desc_get_state(int unit, sbusdma_desc_handle_t desc_handle, 
                           uint8 *state);
/* NOTE: cfg_index only applies to multiple descriptors */
extern int
soc_sbusdma_desc_skip(int unit, sbusdma_desc_handle_t desc_handle, 
                      uint16 cfg_index);
extern int
soc_sbusdma_desc_delete(int unit, sbusdma_desc_handle_t desc_handle);
/* NOTE: cfg_index only applies to multiple descriptors */
extern int
soc_sbusdma_desc_update(int unit, sbusdma_desc_handle_t desc_handle, 
                        uint16 cfg_index, 
                        soc_sbusdma_desc_ctrl_t *ctrl,
                        soc_sbusdma_desc_cfg_t *cfg);
extern int
soc_sbusdma_desc_update_ctrl(int unit, sbusdma_desc_handle_t desc_handle,
                        soc_sbusdma_desc_ctrl_t *ctrl);
extern int
soc_sbusdma_desc_extend(int unit, sbusdma_desc_handle_t desc_handle, 
                        soc_sbusdma_desc_ctrl_t *ctrl,
                        soc_sbusdma_desc_cfg_t *cfg);
extern int
soc_sbusdma_desc_run(int unit, sbusdma_desc_handle_t desc_handle);
extern int
soc_sbusdma_desc_start(int unit, sbusdma_desc_handle_t desc_handle);
extern int
soc_sbusdma_ch_endian_set(int unit, int cmc, int ch, int be);
extern uint32
soc_sbusdma_cmc_max_num_ch_get(int unit);

/*******************************************
* @function soc_sbusdma_ch_try_get
* purpose get idle channel on cmc
*
* @param unit [in] unit #
* @param cmc [out] cmc number
* @param ch [out] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_sbusdma_ch_try_get(int unit, int cmc, int ch);

/*******************************************
* @function soc_sbusdma_ch_put
* purpose put back the freed channel on cmc
*
* @param cmc [in] cmc number
* @param ch [in] channel number
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_sbusdma_ch_put(int unit, int cmc, int ch);


/* Private structures and routines */
#define SOC_SBUSDMA_MAX_DESC 600
typedef struct _soc_sbusdma_state_s { /* s/w state maintenance structure */
    sbusdma_desc_handle_t handle;
    soc_sbusdma_desc_ctrl_t ctrl;
    soc_sbusdma_desc_cfg_t *cfg;
    void *desc;
    uint8 status;
} _soc_sbusdma_state_t;

typedef struct {
    sal_mutex_t lock;
    sal_usecs_t timeout;
    sal_sem_t intr;
    int intrEnb;
    char name[16];
    VOL sal_thread_t pid;
    uint32 count;
    uint8 init;
    uint8 active;
    _soc_sbusdma_state_t *working;
    uint32 _working;
    _soc_sbusdma_state_t *handles[SOC_SBUSDMA_MAX_DESC+1];
} soc_sbusdma_desc_info_t;

#define SOC_SBUSDMA_DM_INFO(unit) \
    SOC_SBUSDMA_INFO(unit)
#define SOC_SBUSDMA_DM_MUTEX(unit) \
    SOC_SBUSDMA_DM_INFO(unit)->lock
#define SOC_SBUSDMA_DM_TO(unit) \
    SOC_SBUSDMA_DM_INFO(unit)->timeout
#define SOC_SBUSDMA_DM_INTR(unit) \
    SOC_SBUSDMA_DM_INFO(unit)->intr
#define SOC_SBUSDMA_DM_INTRENB(unit) \
    SOC_SBUSDMA_DM_INFO(unit)->intrEnb
#define SOC_SBUSDMA_DM_NAME(unit) \
    SOC_SBUSDMA_DM_INFO(unit)->name
#define SOC_SBUSDMA_DM_PID(unit) \
    SOC_SBUSDMA_DM_INFO(unit)->pid
#define SOC_SBUSDMA_DM_START(unit) \
    SOC_SBUSDMA_DM_INFO(unit)->start
#define SOC_SBUSDMA_DM_LAST(unit) \
    SOC_SBUSDMA_DM_INFO(unit)->last
#define SOC_SBUSDMA_DM_COUNT(unit) \
    SOC_SBUSDMA_DM_INFO(unit)->count
#define SOC_SBUSDMA_DM_INIT(unit) \
    SOC_SBUSDMA_DM_INFO(unit)->init
#define SOC_SBUSDMA_DM_ACTIVE(unit) \
    SOC_SBUSDMA_DM_INFO(unit)->active
#define SOC_SBUSDMA_DM_WORKING(unit) \
    SOC_SBUSDMA_DM_INFO(unit)->working
#define SOC_SBUSDMA_DM_HANDLES(unit) \
    SOC_SBUSDMA_DM_INFO(unit)->handles

extern int soc_sbusdma_reg_init(int unit);
extern int soc_sbusdma_init(int unit, int interval, uint8 intrEnb);
extern int soc_sbusdma_desc_detach(int unit);

extern int _soc_l2mod_start(int unit, uint32 flags, sal_usecs_t interval);
extern int _soc_l2mod_stop(int unit);

extern int _soc_l2mod_sbus_fifo_field_set(int unit, soc_field_t field, int enable);
extern int _soc_l2mod_sbus_fifo_field_get(int unit, soc_field_t field, int *enable);

extern int _soc_cmicx_l2mod_start(int unit, uint32 flags, sal_usecs_t interval);
extern int _soc_cmicx_l2mod_stop(int unit);

extern int _soc_cmicx_l2mod_sbus_fifo_field_set(int unit, soc_field_t field, int enable);
extern int _soc_cmicx_l2mod_sbus_fifo_field_get(int unit, soc_field_t field, int *enable);

extern void _soc_sbusdma_curr_op_details(int unit, int cmc, int ch);

#endif /* !_SOC_SBUSDMA_H */
