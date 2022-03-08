/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * Purpose: SOC SBUS-DMA and FIFO DMA driver.
 *
 */

#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/sbusdma_internal.h>
#include <soc/l2x.h>
#ifdef BCM_SAND_SUPPORT
#include <soc/sand/sand_aux_access.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_er_threading.h>
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#ifdef BCM_SBUSDMA_SUPPORT

#if (!defined(_LIMITS_H)) && !defined(_LIBC_LIMITS_H_)

#if (!defined(INT_MIN)) && !defined(INT_MAX)
#define INT_MIN (((int)1)<<(sizeof(int)*8-1))
#define INT_MAX (~INT_MIN)
#endif

#ifndef UINT_MAX
#define UINT_MAX ((unsigned)-1)
#endif

#endif

#ifdef BCM_KATANA2_SUPPORT
#define RD_DMA_CFG_REG                   0
#define RD_DMA_HOTMEM_THRESHOLD_REG      1
#define RD_DMA_STAT                      2
#define RD_DMA_STAT_CLR                  3
#endif 

#if (THREAD_SANITIZER_IS_ACTIVE == 1)
/* { */
/*
 * Function:
 *      soc_get_ptr_sbd_dm_inf_for_get
 * Purpose:
 *      Read value of 'soc_control[unit]->sbd_dm_inf' to carry out a 'get'
 *      operation for specified unit.
 * Parameters:
 *      unit - Device unit #
 * Returns:
 *      soc_sbusdma_desc_info_t * - Value of 'soc_control[unit]->sbd_dm_inf'.
 *      If 'unit' is illegal then return 'NULL'
 */
static soc_sbusdma_desc_info_t
    *soc_get_ptr_sbd_dm_inf_for_get(int unit)
{
    soc_control_t *soc;
    soc_sbusdma_desc_info_t *ret;

    soc = SOC_CONTROL(unit);
    if ((uint32)unit >= (uint32)SOC_MAX_NUM_DEVICES)
    {
        ret = NULL;
    }
    else
    {
        ret = soc->sbd_dm_inf;
    }
    return (ret);
}
#define SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit) \
    soc_get_ptr_sbd_dm_inf_for_get(unit)
/* } */
#else
/* { */
#define SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit) \
    SOC_CONTROL(unit)->sbd_dm_inf
/* } */
#endif



typedef struct soc_sbusdma_reg_s {
    soc_sbusdma_reg_drv_t drv;
    soc_sbusdma_cmic_ch_t  ch;
} soc_sbusdma_reg_t;

STATIC soc_sbusdma_desc_drv_t _sbusdma_desc_drv[SOC_MAX_NUM_DEVICES];
STATIC soc_sbusdma_reg_t _sbusdma_reg_drv[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *     _soc_mem_sbusdma_read
 * Purpose:
 *     DMA acceleration for soc_mem_read_range()
 * Parameters:
 *     buffer -- must be pointer to sufficiently large
 *               DMA-able block of memory
 */
int
_soc_mem_sbusdma_read(int unit, soc_mem_t mem, int copyno,
                      int index_min, int index_max,
                      uint32 ser_flags, void *buffer)
{
    return _soc_mem_array_sbusdma_read(unit, mem, 0, copyno,
                                       index_min, index_max,
                                       ser_flags, buffer, -1);
}

int
_soc_mem_array_sbusdma_read(int unit, soc_mem_t mem, unsigned array_index,
                            int copyno, int index_min, int index_max,
                            uint32 ser_flags, void *buffer, int vchan)
{
    int rv = SOC_E_MEMORY;
    soc_sbusdma_reg_param_t  param;

    param.mem = mem;
    param.array_id_start = array_index;
    param.index_begin = index_min;
    param.index_end = index_max;
    param.copyno = copyno;
	param.ser_flags = ser_flags;
	param.flags = 0;
    param.buffer = buffer;
    param.vchan = vchan;

    if (_sbusdma_reg_drv[unit].drv.soc_mem_sbusdma_read != NULL) {
        rv = _sbusdma_reg_drv[unit].drv.soc_mem_sbusdma_read(unit, &param);
        if ((rv == SOC_E_INTERNAL) || (rv == SOC_E_BUSY)) {
            LOG_VERBOSE(BSL_LS_SOC_DMA,
                (BSL_META_U(unit, "Internal Error, Retry to recover\n")));
            /* Recoverable, Retry */
            rv = _sbusdma_reg_drv[unit].drv.soc_mem_sbusdma_read(unit, &param);
            /* Report failure in case of consecutive internal error */
            if ((rv == SOC_E_INTERNAL) || (rv == SOC_E_BUSY)) {
                rv = SOC_E_FAIL;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     _soc_mem_array_sbusdma_write
 * Purpose:
 *      DMA acceleration for soc_mem_write_range() on FB/ER
 * Parameters:
 *      buffer -- must be pointer to sufficiently large
 *                DMA-able block of memory
 *      index_begin <= index_end - Forward direction
 *      index_begin >  index_end - Reverse direction
 *      mem_clear -- if TRUE: simply clear memory using one
 *                   mem chunk from the supplied buffer
 *      clear_buf_ent is not used as SLAM DMA only supports
 *                   a one entry buffer
 */

int
_soc_mem_array_sbusdma_write(int unit, int flags, soc_mem_t mem,
                             unsigned array_index_start,
                             unsigned array_index_end, int copyno,
                             int index_begin, int index_end, 
                             void *buffer, uint8 mem_clear, 
                             int clear_buf_ent, int vchan)
{
    int rv = SOC_E_MEMORY;

    soc_sbusdma_reg_param_t  param;

    param.mem = mem;
    param.array_id_start = array_index_start;
    param.array_id_end = array_index_end;
    param.index_begin = index_begin;
    param.index_end = index_end;
    param.copyno = copyno;
	param.ser_flags = 0;
    param.flags= flags;
    param.mem_clear = mem_clear;
    param.buffer = buffer;
    param.vchan = vchan;

    if (_sbusdma_reg_drv[unit].drv.soc_mem_sbusdma_write != NULL) {
        rv = _sbusdma_reg_drv[unit].drv.soc_mem_sbusdma_write(unit,
                                                      &param);
        if ((rv == SOC_E_INTERNAL) || (rv == SOC_E_BUSY)) {
            LOG_VERBOSE(BSL_LS_SOC_DMA,
                (BSL_META_U(unit, "Internal Error, Retry to recover\n")));
            /* Recoverable, Retry */
            rv = _sbusdma_reg_drv[unit].drv.soc_mem_sbusdma_write(unit,
                                                      &param);
            /* Report failure in case of consecutive internal error */
        if ((rv == SOC_E_INTERNAL) || (rv == SOC_E_BUSY)) {
                rv = SOC_E_FAIL;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     _soc_mem_sbusdma_write
 * Purpose:
 *      DMA acceleration for soc_mem_write_range() on FB/ER
 * Parameters:
 *      buffer -- must be pointer to sufficiently large
 *                DMA-able block of memory
 *      index_begin <= index_end - Forward direction
 *      index_begin >  index_end - Reverse direction
 *      mem_clear -- if TRUE: simply clear memory using one
 *                   mem chunk from the supplied buffer 
 */

int
_soc_mem_sbusdma_write(int unit, soc_mem_t mem, int copyno,
                       int index_begin, int index_end, 
                       void *buffer, uint8 mem_clear, 
                       int clear_buf_ent)
{
    return _soc_mem_array_sbusdma_write(unit, 0, mem, 0, 0, copyno, index_begin,
                                        index_end, buffer, mem_clear,
                                        clear_buf_ent, -1);
}

/*
 * Function:
 *     _soc_mem_sbusdma_clear_specific
 * Purpose:
 *     clear a specific memory/table region using DMA
 *     write (slam) acceleration.
 */
int
_soc_mem_sbusdma_clear_specific(int unit, soc_mem_t mem,
                                unsigned array_index_min,
                                unsigned array_index_max,
                                int copyno,
                                int index_min, int index_max,
                                void *null_entry)
{
    int    rv = SOC_E_MEMORY;

    soc_sbusdma_reg_param_t  param;

    param.mem = mem;
    param.array_id_start = array_index_min;
    param.array_id_end = array_index_max;
    param.index_begin = index_min;
    param.index_end = index_max;
    param.copyno = copyno;
    param.buffer = null_entry;
    param.vchan = -1;

    if (_sbusdma_reg_drv[unit].drv.soc_mem_sbusdma_clear != NULL) {
        rv = _sbusdma_reg_drv[unit].drv.soc_mem_sbusdma_clear(unit, &param);
        if ((rv == SOC_E_INTERNAL) || (rv == SOC_E_BUSY)) {
            LOG_VERBOSE(BSL_LS_SOC_DMA,
                (BSL_META_U(unit, "Internal Error, Retry to recover\n")));
            /* Retry */
            rv = _sbusdma_reg_drv[unit].drv.soc_mem_sbusdma_clear(unit, &param);
            /* Report failure in case of consecutive internal error */
            if ((rv == SOC_E_INTERNAL) || (rv == SOC_E_BUSY)) {
                rv = SOC_E_FAIL;
            }
        }
    }


    return rv;
}


/*
 * Function:
 *     _soc_mem_sbusdma_clear
 * Purpose:
 *     soc_mem_clear acceleration using table DMA write (slam)
 */
int
_soc_mem_sbusdma_clear(int unit, soc_mem_t mem, int copyno, void *null_entry)
{
    return _soc_mem_sbusdma_clear_specific(unit, mem, 0, UINT_MAX, copyno,
                                           INT_MIN, INT_MAX, null_entry);
}

/*
 ****************************************************************
 * SBUSDMA descriptor mode
 ****************************************************************
 */

STATIC int
_soc_sbusdma_desc_abort(int unit)
{
    soc_timeout_t to;
    soc_sbusdma_desc_info_t *sbd_dm_inf_for_get;

    sbd_dm_inf_for_get = SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit);
    if (sbd_dm_inf_for_get == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s(): Failure in soc_get_ptr_sbd_dm_inf_for_get(). Probably wrong unit\n"), __func__));
        return SOC_E_INTERNAL;
    }
    /* signal abort */
    SOC_SBUSDMA_DM_TO(unit) = 0;
    if (sbd_dm_inf_for_get->pid != SAL_THREAD_ERROR) {
        /* Wake up thread so it will check the exit flag */
        sal_sem_give(sbd_dm_inf_for_get->intr);

        /* Give thread a few seconds to wake up and exit */
        if (SAL_BOOT_SIMULATION) {
            soc_timeout_init(&to, 50 * 1000000, 0);
        } else {
            soc_timeout_init(&to, 10 * 1000000, 0);
        }

        while (sbd_dm_inf_for_get->pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_sbusdma_desc_detach: SBUDMA Desc Mode thread will not exit\n")));
                return SOC_E_INTERNAL;
            }
        }
    }
    return SOC_E_NONE;
}


int
soc_sbusdma_desc_abort(int unit)
{
    int rv = SOC_E_NONE;

    if (!SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit)) {
        return rv;
    }

    rv = _soc_sbusdma_desc_abort(unit);

    return rv;
}

int
soc_sbusdma_desc_detach(int unit)
{

    int i, rv = SOC_E_NONE;
    _soc_sbusdma_state_t *swd; 
    soc_sbusdma_desc_info_t *sbd_dm_inf_for_get;

    sbd_dm_inf_for_get = SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit);
    if (!sbd_dm_inf_for_get) {
        return rv;
    }
    SOC_SBUSDMA_DM_INIT(unit) = 2;
    /* Check if something is running and wait for it to complete
     * (or issue an abort ?) */
    if (soc_sbusdma_desc_abort(unit)) {
        return SOC_E_INTERNAL;
    }
    if (_sbusdma_desc_drv[unit].soc_sbusdma_desc_detach) {
        rv = _sbusdma_desc_drv[unit].soc_sbusdma_desc_detach(unit);
    }

    for (i=1; i<SOC_SBUSDMA_MAX_DESC; i++) {
        if (sbd_dm_inf_for_get->handles[i]) {
            swd = sbd_dm_inf_for_get->handles[i];
            sal_free(swd->cfg);
            if (!(swd->ctrl.flags & SOC_SBUSDMA_CFG_USE_SUPPLIED_DESC)) {
                soc_cm_sfree(unit, swd->desc);
            }
            sal_free(swd);
            SOC_SBUSDMA_DM_HANDLES(unit)[i] = 0;
            SOC_SBUSDMA_DM_COUNT(unit)--;
        }
    }
    
    if (sbd_dm_inf_for_get->lock) {
        sal_mutex_destroy(sbd_dm_inf_for_get->lock);
        SOC_SBUSDMA_DM_MUTEX(unit) = NULL;
    }
    if (sbd_dm_inf_for_get->intr) {
        sal_sem_destroy(sbd_dm_inf_for_get->intr);
        SOC_SBUSDMA_DM_INTR(unit) = NULL;
    }
    SOC_SBUSDMA_DM_INIT(unit) = 0;
    sal_free(sbd_dm_inf_for_get);
    SOC_SBUSDMA_DM_INFO(unit) = NULL;

    return rv;
}

STATIC void
_soc_sbusdma_desc(void *unit_vp)
{
    int rv;
    int unit = PTR_TO_INT(unit_vp);
    soc_control_t *soc = SOC_CONTROL(unit);
    sal_usecs_t interval;
    sal_usecs_t stime, etime;
    int cmc = SOC_PCI_CMC(unit);
    int ch;
    _soc_sbusdma_state_t *swd;
    soc_sbusdma_desc_info_t *sbd_dm_inf_for_get;

    sbd_dm_inf_for_get = SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit);
    if (sbd_dm_inf_for_get == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s(): Failure in soc_get_ptr_sbd_dm_inf_for_get(). Probably wrong unit\n"), __func__));
        return;
    }

    SOC_CONTROL_LOCK(unit);
    ch = soc->desc_ch;
    assert((ch >= 0) && (ch < soc->max_sbusdma_channels));
    SOC_CONTROL_UNLOCK(unit);

    while((interval = sbd_dm_inf_for_get->timeout) != 0) {
        /* wait for run indication */
        (void)sal_sem_take(sbd_dm_inf_for_get->intr, sal_sem_FOREVER);
        if (!sbd_dm_inf_for_get->timeout) {
            goto cleanup_exit;
        }
        if (!sbd_dm_inf_for_get->active || !sbd_dm_inf_for_get->working) {
            continue;
        }

        LOG_VERBOSE(BSL_LS_SOC_DMA,
                    (BSL_META_U(unit,
                                "soc_sbusdma_desc: Process \n")));
        stime = sal_time_usecs();
        SOC_SBUSDMA_DM_LOCK(unit);
        swd = sbd_dm_inf_for_get->working;

        /* execute preconfig */
        if (swd->ctrl.pc) {
            swd->ctrl.pc(unit, swd->handle, swd->ctrl.pc_data);
        }

        rv = _sbusdma_desc_drv[unit].soc_sbusdma_program(unit, &cmc, swd, &ch);

        if (rv == SOC_E_NONE) {
            rv = _sbusdma_desc_drv[unit].soc_sbusdma_wait(unit, cmc, ch,
                                                          sbd_dm_inf_for_get->timeout,
                                                          sbd_dm_inf_for_get->intrEnb);
            if ((rv == SOC_E_INTERNAL) || (rv == SOC_E_BUSY)) {
                LOG_VERBOSE(BSL_LS_SOC_DMA,
                    (BSL_META_U(unit, "Internal Error, Retry to recover\n")));
                /* Recoverable, Retry */
                rv = _sbusdma_desc_drv[unit].soc_sbusdma_wait(unit, cmc, ch,
                                                          sbd_dm_inf_for_get->timeout,
                                                          sbd_dm_inf_for_get->intrEnb);
                /* Report failure in case of consecutive internal error */
            if ((rv == SOC_E_INTERNAL) || (rv == SOC_E_BUSY)) {
                    rv = SOC_E_FAIL;
                }
            }
        }

        if (rv == SOC_E_NONE) {
            etime = sal_time_usecs();
            LOG_VERBOSE(BSL_LS_SOC_DMA,
                        (BSL_META_U(unit,
                                    "soc_sbusdma_desc: unit=%d mode(%s) done in %d usec\n"),
                         unit, sbd_dm_inf_for_get->intrEnb ? "interrupt" : "polling",
                         SAL_USECS_SUB(etime, stime)));
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_sbusdma_desc:%s Error\n"),
                       swd->ctrl.name));
        }

        /* execute callback */
        swd->status = 0;
        swd->ctrl.cb(unit, rv, swd->handle, swd->ctrl.data);

        SOC_SBUSDMA_DM_ACTIVE(unit) = 0;
        SOC_SBUSDMA_DM_UNLOCK(unit);
    }
cleanup_exit:

    /* cleanup the any pending/aborted descriptor */
    if (sbd_dm_inf_for_get->active && sbd_dm_inf_for_get->working) {
        SOC_SBUSDMA_DM_LOCK(unit);
        swd = sbd_dm_inf_for_get->working;
        swd->status = 0;
        SOC_SBUSDMA_DM_ACTIVE(unit) = 0;
        SOC_SBUSDMA_DM_UNLOCK(unit);
    }

    LOG_INFO(BSL_LS_SOC_DMA,
             (BSL_META_U(unit,
                         "_soc_sbusdma_desc: exiting\n")));
    SOC_SBUSDMA_DM_PID(unit) = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}


STATIC int
_soc_sbusdma_desc_init(int unit, int interval, uint8 intrEnb)
{
    int rv;
    soc_sbusdma_desc_info_t *sbd_dm_inf_for_get;

    sbd_dm_inf_for_get = SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit);
    if (sbd_dm_inf_for_get && (sbd_dm_inf_for_get->init == 2)) {
        return SOC_E_BUSY;
    }
    if (sbd_dm_inf_for_get) {
        SOC_SBUSDMA_DM_INIT(unit) = 2;
        if ((rv = soc_sbusdma_desc_detach(unit)) != SOC_E_NONE) {
            return rv;
        }
    }
    SOC_SBUSDMA_DM_INFO(unit) = sal_alloc(sizeof(soc_sbusdma_desc_info_t),
                                          "sbusdma dm info");
    sbd_dm_inf_for_get = SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit);
    if (sbd_dm_inf_for_get == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(sbd_dm_inf_for_get, 0, sizeof(soc_sbusdma_desc_info_t));
    SOC_SBUSDMA_DM_INIT(unit) = 2;

    if ((SOC_SBUSDMA_DM_MUTEX(unit) = 
           sal_mutex_create("sbusdma dm lock")) == NULL) {
        (void)soc_sbusdma_desc_detach(unit);
        return SOC_E_MEMORY;
    }
    if ((SOC_SBUSDMA_DM_INTR(unit) = 
           sal_sem_create("Desc DMA interrupt", sal_sem_BINARY, 0)) == NULL) {
        (void)soc_sbusdma_desc_detach(unit);
        return SOC_E_MEMORY;
    }
    if (intrEnb) {
        SOC_SBUSDMA_DM_INTRENB(unit) = 1;
    } else {
        SOC_SBUSDMA_DM_INTRENB(unit) = 0;
    }
    SOC_SBUSDMA_DM_TO(unit) = interval ? interval : SAL_BOOT_QUICKTURN ? 
                                  30000000 : 10000000;
    sal_snprintf(sbd_dm_inf_for_get->name, sizeof(sbd_dm_inf_for_get->name),
                 "socdmadesc.%d", unit);
    SOC_SBUSDMA_DM_PID(unit) = sal_thread_create(sbd_dm_inf_for_get->name,
                                                 SAL_THREAD_STKSZ,
                                                 soc_property_get(unit, spn_SBUS_DMA_DESC_THREAD_PRI, 50),
                                                 _soc_sbusdma_desc,
                                                 INT_TO_PTR(unit));
    if (sbd_dm_inf_for_get->pid == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_sbusdma_desc_init: Could not start SBUDMA Desc Mode thread.\n")));
        (void)soc_sbusdma_desc_detach(unit);
        return SOC_E_MEMORY;
    }

    SOC_SBUSDMA_DM_COUNT(unit) = 0;
    SOC_SBUSDMA_DM_INIT(unit) = 1;
    return SOC_E_NONE;
}

/*******************************************
* @function soc_sbusdma_desc_create
* Create SBUS DMA Descriptors, based on information in ctrl and cfg,
* the alloc_memory flag is used to indicate if we are using internal
* or host memory, for internal memory, no allocation is made
*
* @param unit [in] unit
* @param ctrl [in] soc_sbusdma_desc_ctrl_t pointer
* @param cfg [in] soc_sbusdma_desc_cfg_t pointer
* @param alloc_memory [in] 1- allocate descriptor memory, 0 - use received descriptor pointer
* @param desc [out] descriptor pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/
int
soc_sbusdma_desc_create(int unit, soc_sbusdma_desc_ctrl_t *ctrl,
                        soc_sbusdma_desc_cfg_t *cfg,
                        uint8 alloc_memory,
                        sbusdma_desc_handle_t *desc_handle)
{
    uint32 i;
    _soc_sbusdma_state_t *swd = NULL;
    int rv;
    soc_sbusdma_desc_info_t *sbd_dm_inf_for_get;

    sbd_dm_inf_for_get = SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit);

    if ((sbd_dm_inf_for_get == NULL) || !sbd_dm_inf_for_get->init) {
        return SOC_E_INIT;
    }

    if (!ctrl || !cfg || (alloc_memory && !ctrl->cb) || !ctrl->cfg_count) {
        return SOC_E_PARAM;
    }

    if (!alloc_memory)
    {
        void *desc_ptr = (void*)desc_handle;
        /* Prepare h/w desc */
        rv =  _sbusdma_desc_drv[unit].soc_sbusdma_desc_create(
                                                     unit, ctrl,
                                                     cfg,
                                                     alloc_memory,
                                                     &desc_ptr);
        return rv;
    }
    SOC_SBUSDMA_DM_LOCK(unit);

    swd = sal_alloc(sizeof(_soc_sbusdma_state_t), "_soc_sbusdma_state_t");
    if (swd == NULL) {
        SOC_SBUSDMA_DM_UNLOCK(unit);
        return SOC_E_MEMORY;
    }
    sal_memset(swd, 0, sizeof(_soc_sbusdma_state_t));

    /* save s/w ctrl and config */
    sal_memcpy(&swd->ctrl, ctrl, sizeof(soc_sbusdma_desc_ctrl_t));
    if (ctrl->cfg_count == 1) { /* single mode */
        swd->cfg = sal_alloc(sizeof(soc_sbusdma_desc_cfg_t),
                                    "soc_sbusdma_desc_cfg_t");
        if (swd->cfg == NULL) {
            sal_free(swd);
            SOC_SBUSDMA_DM_UNLOCK(unit);
            return SOC_E_MEMORY;
        }
        sal_memcpy(swd->cfg, cfg, sizeof(soc_sbusdma_desc_cfg_t));
        if (swd->ctrl.buff) { /* ctrl buff over-rides cfg buff */
            swd->cfg->buff = swd->ctrl.buff;
        }
        assert(swd->cfg->buff);
        if (!(ctrl->flags & SOC_SBUSDMA_CFG_USE_SUPPLIED_DESC) ||
              !ctrl->hw_desc) {

            /* Prepare h/w desc */
            rv =  _sbusdma_desc_drv[unit].soc_sbusdma_desc_create(
                                                         unit, ctrl,
                                                         swd->cfg,
                                                         alloc_memory,
                                                         &swd->desc);
            if (SOC_FAILURE(rv)) {
                sal_free(swd->cfg);
                sal_free(swd);
                SOC_SBUSDMA_DM_UNLOCK(unit);
                return rv;
            }
        } else {
            swd->desc = ctrl->hw_desc;
        }
    } else { /* chain mode */

        swd->cfg = sal_alloc(sizeof(soc_sbusdma_desc_cfg_t) * ctrl->cfg_count,
                             "soc_sbusdma_desc_cfg_t");
        if (swd->cfg == NULL) {
            sal_free(swd);
            SOC_SBUSDMA_DM_UNLOCK(unit);
            return SOC_E_MEMORY;
        }
        sal_memcpy(swd->cfg, cfg, sizeof(soc_sbusdma_desc_cfg_t) *
                   ctrl->cfg_count);
        if (!(ctrl->flags & SOC_SBUSDMA_CFG_USE_SUPPLIED_DESC) ||
                           !ctrl->hw_desc) {

            /* Prepare h/w desc */
            rv =  _sbusdma_desc_drv[unit].soc_sbusdma_desc_create(
                                                         unit, ctrl,
                                                         swd->cfg,
                                                         alloc_memory,
                                                         &swd->desc);
            if (SOC_FAILURE(rv)) {
                sal_free(swd->cfg);
                sal_free(swd);
                SOC_SBUSDMA_DM_UNLOCK(unit);
                return rv;
            }
        } else {
            swd->desc = ctrl->hw_desc;
        }
    }
    /* find a free handle */
    for (i=1; i<SOC_SBUSDMA_MAX_DESC;) {
        if (sbd_dm_inf_for_get->handles[i] == 0) {
            break;
        }
        i++;
    }
    if(i >= SOC_SBUSDMA_MAX_DESC) {
        LOG_WARN(BSL_LS_SOC_DMA,
            (BSL_META_U(unit,
                        "No free DMA descriptors. All %d are used. \n"), i));
        if (!(swd->ctrl.flags & SOC_SBUSDMA_CFG_USE_SUPPLIED_DESC)) {
            soc_cm_sfree(unit, swd->desc);
        }
        sal_free(swd->cfg);
        sal_free(swd);
        SOC_SBUSDMA_DM_UNLOCK(unit);
        return SOC_E_RESOURCE;
    }

    *desc_handle = i;
    swd->handle = *desc_handle;
    SOC_SBUSDMA_DM_HANDLES(unit)[i] = swd;
    SOC_SBUSDMA_DM_COUNT(unit)++;
    SOC_SBUSDMA_DM_UNLOCK(unit);

    if (ctrl->cfg_count == 1) {
        LOG_INFO(BSL_LS_SOC_DMA,
                 (BSL_META_U(unit,
                             "Create Single:: Handle: %d,"
                             "opcount: %d, buff: %p\n"),
                  swd->handle, swd->ctrl.cfg_count, swd->cfg->buff));
    } else {
        LOG_INFO(BSL_LS_SOC_DMA,
                 (BSL_META_U(unit,
                             "Create Chain:: Handle: %d, desc count: %d\n"), 
                  swd->handle, swd->ctrl.cfg_count));
    }
    
    LOG_INFO(BSL_LS_SOC_DMA,
             (BSL_META_U(unit,
                         "SBD DM count: %d\n"), sbd_dm_inf_for_get->count));
    return SOC_E_NONE;
}

int
soc_sbusdma_desc_get(int unit, sbusdma_desc_handle_t desc_handle,
                     soc_sbusdma_desc_ctrl_t *ctrl,
                     soc_sbusdma_desc_cfg_t *cfg)
{
    _soc_sbusdma_state_t *swd;
    soc_sbusdma_desc_info_t *sbd_dm_inf_for_get;

    sbd_dm_inf_for_get = SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit);

    if ((sbd_dm_inf_for_get == NULL) || !sbd_dm_inf_for_get->init ||
        !sbd_dm_inf_for_get->count) {
        return SOC_E_INIT;
    }

    if (!ctrl || !cfg) {
        return SOC_E_PARAM;
    }

    SOC_SBUSDMA_DM_LOCK(unit);
    if ((desc_handle > 0) && (desc_handle <= SOC_SBUSDMA_MAX_DESC)
        && sbd_dm_inf_for_get->handles[desc_handle]) {
        swd = sbd_dm_inf_for_get->handles[desc_handle];
    } else {
        SOC_SBUSDMA_DM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Get request for invalid or non-existing descriptor handle: %d\n"),
                   desc_handle));
        return SOC_E_PARAM;
    }
    if (swd->handle != desc_handle) {
        SOC_SBUSDMA_DM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Handle mismatch found: %d<=>%d\n"), swd->handle,
                   desc_handle));
        return SOC_E_INTERNAL;
    }
    sal_memcpy(ctrl, &swd->ctrl, sizeof(soc_sbusdma_desc_ctrl_t));
    sal_memcpy(cfg, swd->cfg, sizeof(soc_sbusdma_desc_cfg_t) * ctrl->cfg_count);
    SOC_SBUSDMA_DM_UNLOCK(unit);
    return SOC_E_NONE;
}

int
soc_sbusdma_desc_get_state(int unit, sbusdma_desc_handle_t desc_handle,
                           uint8 *state)
{
    _soc_sbusdma_state_t *swd;
    soc_sbusdma_desc_info_t *sbd_dm_inf_for_get;

    sbd_dm_inf_for_get = SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit);

    if ((sbd_dm_inf_for_get == NULL) || !sbd_dm_inf_for_get->init ||
        !sbd_dm_inf_for_get->count) {
        return SOC_E_INIT;
    }
    SOC_SBUSDMA_DM_LOCK(unit);
    if ((desc_handle > 0) && (desc_handle <= SOC_SBUSDMA_MAX_DESC) 
        && sbd_dm_inf_for_get->handles[desc_handle]) {
        swd = sbd_dm_inf_for_get->handles[desc_handle];
    } else {
        SOC_SBUSDMA_DM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Get state request for invalid or non-existing descriptor handle: %d\n"), 
                   desc_handle));
        return SOC_E_PARAM;
    }
    if (swd->handle != desc_handle) {
        SOC_SBUSDMA_DM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Handle mismatch found: %d<=>%d\n"), swd->handle,
                   desc_handle));
        return SOC_E_INTERNAL;
    }
    *state = swd->status;
    SOC_SBUSDMA_DM_UNLOCK(unit);
    return SOC_E_NONE;
}

int
soc_sbusdma_desc_run(int unit, sbusdma_desc_handle_t desc_handle)
{
    _soc_sbusdma_state_t *swd;
    soc_sbusdma_desc_info_t *sbd_dm_inf_for_get;

    sbd_dm_inf_for_get = SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit);

    if ((sbd_dm_inf_for_get == NULL) || !sbd_dm_inf_for_get->init ||
        !sbd_dm_inf_for_get->count) {
        return SOC_E_INIT;
    }
    SOC_SBUSDMA_DM_LOCK(unit);
    if (sbd_dm_inf_for_get->active) {
        SOC_SBUSDMA_DM_UNLOCK(unit);
        return SOC_E_BUSY;
    }
    if ((desc_handle > 0) && (desc_handle <= SOC_SBUSDMA_MAX_DESC)
        && sbd_dm_inf_for_get->handles[desc_handle]) {
        swd = sbd_dm_inf_for_get->handles[desc_handle];
    } else {
        SOC_SBUSDMA_DM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Run request for invalid or non-existing descriptor handle: %d\n"),
                   desc_handle));
        return SOC_E_PARAM;
    }
    if (swd->handle != desc_handle) {
        SOC_SBUSDMA_DM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Handle mismatch found: %d<=>%d\n"),
                   swd->handle, desc_handle));
        return SOC_E_INTERNAL;
    }

    /* process swd->desc */
    swd->status = 1;
    SOC_SBUSDMA_DM_ACTIVE(unit) = 1;
    SOC_SBUSDMA_DM_WORKING(unit) = swd;
    
    if (swd->ctrl.cfg_count == 1) {
        LOG_INFO(BSL_LS_SOC_DMA,
                 (BSL_META_U(unit,
                             "Run Single:: Handle: %d, desc count: %d,"
                             "buff: %p\n"),
                  swd->handle, swd->ctrl.cfg_count, swd->cfg->buff));
    } else {
        LOG_INFO(BSL_LS_SOC_DMA,
                 (BSL_META_U(unit,
                             "Run Chain:: Handle: %d, desc count: %d\n"),
                  swd->handle, swd->ctrl.cfg_count));
    }
    sal_sem_give(sbd_dm_inf_for_get->intr);

    SOC_SBUSDMA_DM_UNLOCK(unit);

    return SOC_E_NONE;
}

int
soc_sbusdma_desc_start(int unit, sbusdma_desc_handle_t desc_handle)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    _soc_sbusdma_state_t *swd;
    sal_usecs_t stime, etime;
    int cmc = SOC_PCI_CMC(unit);
    int ch = soc->desc_ch;
    int rv;
    soc_sbusdma_desc_info_t *sbd_dm_inf_for_get;

    sbd_dm_inf_for_get = SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit);

    if (!SOC_CHECK_MULTI_CMC(unit)) {
        return soc_sbusdma_desc_run(unit, desc_handle);
    }

    if ((sbd_dm_inf_for_get == NULL) || !sbd_dm_inf_for_get->init ||
        !sbd_dm_inf_for_get->count) {
        return SOC_E_INIT;
    }
    SOC_SBUSDMA_DM_LOCK(unit);
    if ((desc_handle > 0) && (desc_handle <= SOC_SBUSDMA_MAX_DESC)
        && sbd_dm_inf_for_get->handles[desc_handle]) {
        swd = sbd_dm_inf_for_get->handles[desc_handle];
    } else {
        SOC_SBUSDMA_DM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Start request for invalid or non-existing descriptor handle: %d\n"),
                   desc_handle));
        return SOC_E_PARAM;
    }
    if (swd->handle != desc_handle) {
        SOC_SBUSDMA_DM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Handle mismatch found: %d<=>%d\n"),
                   swd->handle, desc_handle));
        return SOC_E_INTERNAL;
    }
    if (swd->status) {
        SOC_SBUSDMA_DM_UNLOCK(unit);
        return SOC_E_BUSY;
    }
    swd->status = 1;
    SOC_SBUSDMA_DM_UNLOCK(unit);

    if (swd->ctrl.cfg_count == 1) {
        LOG_INFO(BSL_LS_SOC_DMA,
                 (BSL_META_U(unit,
                             "Start Single:: Handle: %d, desc count: %d,"
                             "buff: %p\n"),
                  swd->handle, swd->ctrl.cfg_count, swd->cfg->buff));
    } else {
        LOG_INFO(BSL_LS_SOC_DMA,
                 (BSL_META_U(unit,
                             "Start Chain:: Handle: %d, desc count: %d\n"),
                  swd->handle, swd->ctrl.cfg_count));
    }

    stime = sal_time_usecs();

    /* execute preconfig */
    if (swd->ctrl.pc) {
        swd->ctrl.pc(unit, swd->handle, swd->ctrl.pc_data);
    }

    rv = _sbusdma_desc_drv[unit].soc_sbusdma_program(unit, &cmc, swd, &ch);

    if (rv == SOC_E_NONE) {
        rv = _sbusdma_desc_drv[unit].soc_sbusdma_wait(unit, cmc, ch,
                                                      sbd_dm_inf_for_get->timeout,
                                                      sbd_dm_inf_for_get->intrEnb);
    }

    if (rv == SOC_E_NONE) {
        etime = sal_time_usecs();
        LOG_VERBOSE(BSL_LS_SOC_DMA,
                    (BSL_META_U(unit,
                                "soc_sbusdma_desc: unit=%d mode(%s) done in %d usec\n"),
                     unit, sbd_dm_inf_for_get->intrEnb ? "interrupt" : "polling",
                     SAL_USECS_SUB(etime, stime)));
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_sbusdma_desc:%s Error\n"),
                   swd->ctrl.name));
    }

    /* execute callback */
    swd->ctrl.cb(unit, rv, swd->handle, swd->ctrl.data);

    SOC_SBUSDMA_DM_LOCK(unit);
    swd->status = 0;
    SOC_SBUSDMA_DM_UNLOCK(unit);

    return rv;
}

int
soc_sbusdma_desc_delete(int unit, sbusdma_desc_handle_t handle)
{
    _soc_sbusdma_state_t *swd;
    soc_sbusdma_desc_info_t *sbd_dm_inf_for_get;

    sbd_dm_inf_for_get = SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit);

    if ((sbd_dm_inf_for_get == NULL) || !sbd_dm_inf_for_get->init ||
        !sbd_dm_inf_for_get->count) {
        return SOC_E_INIT;
    }
    SOC_SBUSDMA_DM_LOCK(unit);
    if ((handle > 0) && (handle <= SOC_SBUSDMA_MAX_DESC)
        && sbd_dm_inf_for_get->handles[handle]) {
        swd = sbd_dm_inf_for_get->handles[handle];
        if (swd->handle != handle) {
            SOC_SBUSDMA_DM_UNLOCK(unit);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Handle mismatch found: %d<=>%d\n"),
                       swd->handle, handle));
            return SOC_E_INTERNAL;
        }
        if (swd->ctrl.cfg_count == 1) {
            LOG_INFO(BSL_LS_SOC_DMA,
                     (BSL_META_U(unit,
                                 "Delete Single:: Handle: %d, desc count: %d,"
                                 "buff: %p\n"),
                      swd->handle, swd->ctrl.cfg_count, swd->cfg->buff));
        } else {
            LOG_INFO(BSL_LS_SOC_DMA,
                     (BSL_META_U(unit,
                                 "Delete Chain:: Handle: %d, desc count: %d\n"),
                      swd->handle, swd->ctrl.cfg_count));
        }
        sal_free(swd->cfg);
        if (!(swd->ctrl.flags & SOC_SBUSDMA_CFG_USE_SUPPLIED_DESC)) {
            soc_cm_sfree(unit, swd->desc);
        }
        sal_free(swd);
        SOC_SBUSDMA_DM_HANDLES(unit)[handle] = 0;
        SOC_SBUSDMA_DM_COUNT(unit)--;
        LOG_INFO(BSL_LS_SOC_DMA,
                 (BSL_META_U(unit,
                             "SBD DM count: %d\n"),
                  sbd_dm_inf_for_get->count));
    } else {
        SOC_SBUSDMA_DM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Del request for invalid or non-existing descriptor handle: %d\n"),
                   handle));
        return SOC_E_PARAM;
    }
    SOC_SBUSDMA_DM_UNLOCK(unit);
    return SOC_E_NONE;
}

/*******************************************
* @function soc_sbusdma_desc_update_ctrl
* Update SBUS DMA Descriptors, based on information in ctrl
*
* @param unit [in] unit
* @param desc [in] descriptor handle
* @param ctrl [in] soc_sbusdma_desc_ctrl_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/
int
soc_sbusdma_desc_update_ctrl(int unit, sbusdma_desc_handle_t desc_handle,
                        soc_sbusdma_desc_ctrl_t *ctrl)
{
    _soc_sbusdma_state_t *swd = NULL;
    soc_sbusdma_desc_info_t *sbd_dm_inf_for_get;

    sbd_dm_inf_for_get = SOC_GET_PTR_SBD_DM_INF_FOR_GET(unit);

    if ((sbd_dm_inf_for_get == NULL) || !sbd_dm_inf_for_get->init) {
        return SOC_E_INIT;
    }

    if (!ctrl || !ctrl->cfg_count) {
        return SOC_E_PARAM;
    }

    SOC_SBUSDMA_DM_LOCK(unit);

    if ((desc_handle > 0) && (desc_handle <= SOC_SBUSDMA_MAX_DESC)
        && sbd_dm_inf_for_get->handles[desc_handle]) {
        swd = sbd_dm_inf_for_get->handles[desc_handle];
    } else {
        SOC_SBUSDMA_DM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Run request for invalid or non-existing descriptor handle: %d\n"),
                   desc_handle));
        return SOC_E_PARAM;
    }

    /* save s/w ctrl */
    sal_memcpy(&swd->ctrl, ctrl, sizeof(soc_sbusdma_desc_ctrl_t));

    /* Update hw_desc pointer if necessary */
    if ((ctrl->flags & SOC_SBUSDMA_CFG_USE_SUPPLIED_DESC) && (ctrl->hw_desc)) {
        swd->desc = ctrl->hw_desc;
    }

    SOC_SBUSDMA_DM_UNLOCK(unit);

    return SOC_E_NONE;
}

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
********************************************/
int soc_sbusdma_ch_try_get(int unit, int cmc, int ch)
{
    int rv = SOC_E_NONE;

    if (_sbusdma_reg_drv[unit].ch.soc_sbusdma_ch_try_get != NULL) {
        rv =
         _sbusdma_reg_drv[unit].ch.soc_sbusdma_ch_try_get(unit, cmc, ch);
    } else {
        rv = SOC_E_INIT;
    }

    return rv;
}

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
********************************************/
int soc_sbusdma_ch_put(int unit, int cmc, int ch)
{
   int rv;

   if (_sbusdma_reg_drv[unit].ch.soc_sbusdma_ch_put != NULL) {
        rv =
         _sbusdma_reg_drv[unit].ch.soc_sbusdma_ch_put(unit, cmc, ch);
   } else {
       rv = SOC_E_INIT;
   }

   return rv;
}

/*******************************************
* @function soc_sbusdma_ch_endian_set
* set the endianess value
*
* @param cmc [in] cmc number
* @param ch [in] channel number
* @param ch [in] big_endian
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
********************************************/
int soc_sbusdma_ch_endian_set(int unit, int cmc, int ch, int be)
{
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        return cmicx_sbusdma_ch_endian_set(unit, cmc, ch, be);
    }
#endif

    return SOC_E_NONE;
}

/*******************************************
* @function soc_sbusdma_cmc_max_num_ch_get
* @Returns number of SBUSDMA channels in cmc
*
* @end
********************************************/
uint32 soc_sbusdma_cmc_max_num_ch_get(int unit)
{
    if(_sbusdma_reg_drv[unit].drv.soc_sbusdma_cmc_max_num_ch_get == NULL) {
        /* driver is not attached yet, so return zero */
        return 0;
    }
    return _sbusdma_reg_drv[unit].drv.soc_sbusdma_cmc_max_num_ch_get(unit);
}

/*
 * Function:
 *      soc_sbusdma_pci_timeout_handle
 * Purpose:
 *      Process SBUSDMA PCI timeout error.
 * Parameters:
 *      unit number
 *        cmc
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code.
 * Notes:
 *      INTERRUPT LEVEL ROUTINE
 */
int
soc_sbusdma_pci_timeout_handle(int unit, int cmc)
{
    int rv = SOC_E_NONE;

#if defined(BCM_ESW_SUPPORT) && defined(BCM_IPROC_SUPPORT) && defined(BCM_CMICM_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicm)) {
        rv = cmicm_sbusdma_pci_timeout_handle(unit, cmc);
    }
#endif

    return rv;
}


/*******************************************
* @function soc_sbusdma_reg_init
* purpose API to Initialize cmicx/cmicm Register DMA
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/
int soc_sbusdma_reg_init(int unit)
{
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        SOC_IF_ERROR_RETURN(cmicx_sbusdma_reg_init(unit,
                            &_sbusdma_reg_drv[unit].drv));
    }
#endif

#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        SOC_IF_ERROR_RETURN(cmicm_sbusdma_reg_init(unit,
                            &_sbusdma_reg_drv[unit].drv));
    }
#endif

    return SOC_E_NONE;
}


/*******************************************
* @function soc_sbusdma_init
* purpose API to Initialize SBUS DMA
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/
int soc_sbusdma_init(int unit, int interval, uint8 intrEnb)
{
    int rv;


#ifdef BCM_ACCESS_SUPPORT
    if (SOC_IS_NEW_ACCESS_INITIALIZED(unit)) {

        rv = _soc_sbusdma_desc_init(unit, interval, intrEnb);
        return rv;
    }
#endif

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_control_t *soc = SOC_CONTROL(unit);
        SOC_IF_ERROR_RETURN(cmicx_sbusdma_reg_init(unit,
                            &_sbusdma_reg_drv[unit].drv));
        SOC_IF_ERROR_RETURN(cmicx_sbusdma_ch_init(unit,
                        soc->sbusDmaTimeout + CMIC_CMCx_SBUSDMA_CHSELECT_TIMEOUT,
                        &_sbusdma_reg_drv[unit].ch));

        if ((soc->sbusDmaIntrEnb) || (soc->tslamDmaIntrEnb) ||
               (soc->tableDmaIntrEnb)) {
            SOC_IF_ERROR_RETURN(cmicx_sbusdma_intr_init(unit));
        }

        SOC_IF_ERROR_RETURN(_soc_sbusdma_desc_init(unit, interval, intrEnb));

        rv = cmicx_sbusdma_desc_init(unit, &_sbusdma_desc_drv[unit]);
        if (rv != SOC_E_NONE) {
            (void)soc_sbusdma_desc_detach(unit);
            return rv;
        }

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,":SUCCESS\n")));
    }
#endif


#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {

        SOC_IF_ERROR_RETURN(cmicm_sbusdma_reg_init(unit,
                            &_sbusdma_reg_drv[unit].drv));
        SOC_IF_ERROR_RETURN(cmicm_sbusdma_ch_init(unit,
                        CMIC_CMCx_SBUSDMA_CHSELECT_TIMEOUT,
                        &_sbusdma_reg_drv[unit].ch));

        SOC_IF_ERROR_RETURN(_soc_sbusdma_desc_init(unit, interval, intrEnb));

        rv = cmicm_sbusdma_desc_init(unit, &_sbusdma_desc_drv[unit]);
        if (rv != SOC_E_NONE) {
          (void)soc_sbusdma_desc_detach(unit);
           return rv;
        }
    }
#endif

    return SOC_E_NONE;
}

/*******************************************
* @function soc_sbusdma_desc_drv_get
* purpose Get sbusdma desc driver
*
* @param unit [in] unit
*
* @returns _sbusdma_desc_drv
*
* @end
********************************************/
soc_sbusdma_desc_drv_t* soc_sbusdma_desc_drv_get(int unit)
{
    return &_sbusdma_desc_drv[unit];
}

#endif /* BCM_SBUSDMA_SUPPORT */
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) */
