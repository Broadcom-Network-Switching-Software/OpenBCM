/*! \file bcm56780_a0_cth_mon_ft_fifo.c
 *
 * Chip specific routines for handling FT_LEARN_NOTIFY_FIFO memory.
 * Memory/registers: FT_LEARN_NOTIFYm, FT_LEARN_NOTIFY_FIFO_PTRSr,
 *                   FT_LEARN_CTRLr
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl_enum.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>

#include <bcmltd/bcmltd_types.h>
#include <bcmltd/id/bcmltd_common_id.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcmcth/bcmcth_imm_util.h>
#include <bcmcth/generated/flowtracker_ha.h>
#include <bcmcth/chip/bcm56780_a0/bcm56780_a0_cth_mon_ft_fifo.h>
#include <bcmcth/bcmcth_mon_flowtracker_flow_entry.h>
#include <bcmcth/bcmcth_mon_flowtracker_event_log.h>
#include <bcmcth/bcmcth_mon_flowtracker_fifo.h>

/*******************************************************************************
 * Defines
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/* SBUSDMA Work ptr and Work data ptr */
#define BCM56780_A0_FT_FIFO_SBUSDMA_WORK_DATA_PTR(unit, pipe)\
 (bcm56780_a0_ft_fifo_sbusdma_work[unit]->sbusdma_work[pipe].data)
#define BCM56780_A0_FT_FIFO_SBUSDMA_WORK_DATA_SIZE\
    sizeof(bcmbd_sbusdma_data_t)
#define BCM56780_A0_FT_FIFO_SBUSDMA_WORK_PTR(unit, pipe)\
 (&(bcm56780_a0_ft_fifo_sbusdma_work[unit]->sbusdma_work[pipe]))
#define BCM56780_A0_FT_FIFO_ALL_PIPES_SBUSDMA_WORK_PTR(unit)\
 (bcm56780_a0_ft_fifo_sbusdma_work[unit])
#define BCM56780_A0_FT_FIFO_ALL_PIPES_SBUSDMA_WORK_SIZE\
    sizeof(bcm56780_a0_ft_fifo_sbusdma_work_ptr_pipes_t)

/* FIFO SW cache (unit) level pointers */
#define BCM56780_A0_FT_FIFO_SW_CACHE_PTR(unit)\
 (bcm56780_a0_ft_fifo_sw_cache[unit])
#define BCM56780_A0_FT_FIFO_SW_CACHE_SIZE\
 sizeof(bcm56780_a0_ft_fifo_sw_cache_pipes_t)

/* FIFO SW cache (unit,pipe) level pointers */
#define BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_ENTRIES_PTR(unit, pipe)\
 (bcm56780_a0_ft_fifo_sw_cache[unit]->pipe_fifo_sw_cache[pipe]->entries)
#define BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_PTR(unit, pipe)\
 (bcm56780_a0_ft_fifo_sw_cache[unit]->pipe_fifo_sw_cache[pipe])
#define BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_SIZE\
    sizeof(bcm56780_a0_ft_fifo_sw_cache_entries_t)
#define BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_PADDR(unit, pipe)\
 (bcm56780_a0_ft_fifo_sw_cache[unit]->pipe_fifo_sw_cache_paddr[pipe])
#define BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_PADDR_PTR(unit, pipe)\
 (&(BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_PADDR(unit, pipe)))

/*******************************************************************************
 * Private variables
 */
/* SBUSDMA work handles */
static bcm56780_a0_ft_fifo_sbusdma_work_ptr_pipes_t *bcm56780_a0_ft_fifo_sbusdma_work[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

/* FIFO entries SW cache */
static bcm56780_a0_ft_fifo_sw_cache_pipes_t *bcm56780_a0_ft_fifo_sw_cache[BCMDRD_CONFIG_MAX_UNITS] = {NULL};

/******************************************************************************
* Private functions
 */

/*
 * This routine updates the
 * BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_ENTRIES_PTR(unit,pipe) from index 0 to
 * count - 1 with the FIFO entries popped from FT_LEARN_NOTIFY_FIFOm
 * using SBUSDMA pop operation.
 * NOTE: The setup of the SBUSDMA work pointer is already done during
 * bcm56780_a0_cth_mon_ft_fifo_drv_init
 */
static int bcm56780_a0_cth_mon_ft_fifo_sw_cache_refresh(
                                            int unit,
                                            int pipe,
                                            uint32_t count)
{
    bcmbd_sbusdma_work_t *work_ptr;

    SHR_FUNC_ENTER(unit);

    /* Sanity check of count variable */
    if (count > BCM56780_A0_CTH_MON_FT_LRN_NOTIF_FIFO_NUM_ENTRIES) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    work_ptr = BCM56780_A0_FT_FIFO_SBUSDMA_WORK_PTR(unit, pipe);

    if (work_ptr == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /*
     * Work pointer is already set with proper values during driver init.
     * Just need to update the count.
     */
    work_ptr->data->op_count = count;

    /* Initialize the light work(means not batched) */
    SHR_IF_ERR_EXIT(bcmbd_sbusdma_light_work_init(unit, work_ptr));

    /* Execute the SBUSDMA */
    SHR_IF_ERR_EXIT(bcmbd_sbusdma_work_execute(unit, work_ptr));

exit:
    SHR_FUNC_EXIT();
}


/*
 * This routine gets the count of unpopped fifo entries in the FIFO
 * FT_LEARN_NOTIFY_FIFO_PTRSr.NOTIFY_FIFO_ENTRY_COUNTf
 */
static int bcm56780_a0_cth_mon_ft_fifo_entry_count_get(
                                        int unit, int pipe,
                                        uint32_t *count)
{
    bcmltd_sid_t lt_id;
    bcmdrd_sid_t pt_id;
    int index;
    FT_LEARN_NOTIFY_FIFO_PTRSr_t entry = {{0}};
    SHR_FUNC_ENTER(unit);

    lt_id = MON_FLOWTRACKER_LEARN_EVENT_LOGt;
    pt_id = FT_LEARN_NOTIFY_FIFO_PTRSr;
    index = 0;

    SHR_IF_ERR_EXIT(
            bcmcth_imm_pipe_ireq_read(unit, lt_id, pt_id, pipe, index, &entry));
    *count =
            FT_LEARN_NOTIFY_FIFO_PTRSr_NOTIFY_FIFO_ENTRY_COUNTf_GET(entry);
exit:
    SHR_FUNC_EXIT();
}

/*
 * This routine sets the behavior of notification fifo
 * Only behavior supported for 56780 currently is to stop learning
 * when FIFO is full.
 * FT_LEARN_CTRL.FT_LEARN_NOTIFY_FULL_BEHAVIOR
 */
static int bcm56780_a0_cth_mon_ft_fifo_behavior_set(int unit,
                                                    int pipe,
                                                    uint32_t behavior)
{
    bcmltd_sid_t lt_id;
    bcmdrd_sid_t pt_id;
    int index;
    FT_LEARN_CTRLr_t entry = {{0}};
    SHR_FUNC_ENTER(unit);

    lt_id = MON_FLOWTRACKER_LEARN_EVENT_CONTROLt;
    pt_id = FT_LEARN_CTRLr;
    index = 0;

    SHR_IF_ERR_EXIT(
            bcmcth_imm_pipe_ireq_read(unit, lt_id, pt_id, pipe, index, &entry));
    if (behavior & BCMCTH_MON_FT_LEARN_FIFO_FULL_STOP_LEARNING) {
        FT_LEARN_CTRLr_FT_LEARN_NOTIFY_FULL_BEHAVIORf_SET(entry, 1);
    }

    SHR_IF_ERR_EXIT(
            bcmcth_imm_pipe_ireq_write(unit, lt_id, pt_id, pipe, index, &entry));
exit:
    SHR_FUNC_EXIT();
}

static void
bcm56780_a0_cth_mon_ft_fifo_sw_cache_entry_evt_log_lt_entry_convert(
                        int unit,
                        int pipe,
                        FT_LEARN_NOTIFY_FIFOm_t *entry,
                        bcmcth_mon_flowtracker_event_log_entry_t *lt_entry)
{
    uint32_t hit_index = 0;
    uint8_t hw_mode = 0;

    lt_entry->pipe = pipe;
    lt_entry->mon_flowtracker_group_id = FT_LEARN_NOTIFY_FIFOm_GROUP_IDf_GET(*entry);
    hit_index = FT_LEARN_NOTIFY_FIFOm_HIT_INDEXf_GET(*entry);
    hw_mode = BCM56780_A0_EXACT_MATCH_MODE_FROM_HIT_IDX_GET(hit_index);
    switch (hw_mode) {
        case 0:
            lt_entry->mode =
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_EXACT_MATCH_IDX_MODE_T_T_QUAD;
            lt_entry->exact_match_idx =
            BCM56780_A0_EXACT_MATCH_QUAD_WIDE_IDX_FROM_HIT_IDX_GET(hit_index);
            break;
        case 1:
            lt_entry->mode =
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_EXACT_MATCH_IDX_MODE_T_T_DOUBLE;
            lt_entry->exact_match_idx =
            BCM56780_A0_EXACT_MATCH_DOUBLE_WIDE_IDX_FROM_HIT_IDX_GET(hit_index);
            break;
        case 2:
            lt_entry->mode =
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_EXACT_MATCH_IDX_MODE_T_T_SINGLE;
            lt_entry->exact_match_idx =
            BCM56780_A0_EXACT_MATCH_SINGLE_WIDE_IDX_FROM_HIT_IDX_GET(hit_index);
            break;
        default:
            /* Cannot happen */
            break;
    }
}

/*
 * This routine traverses the sw cache and reports the newly learnt flows
 * to MON_FLOWTRACKER_LEARN_EVENT_LOG logical table.
 */
static int bcm56780_a0_cth_mon_ft_fifo_sw_cache_traverse_and_report(int unit,
                                                                    int pipe,
                                                                    uint32_t count)
{
    FT_LEARN_NOTIFY_FIFOm_t *entry = NULL;
    FT_LEARN_NOTIFY_FIFOm_t *fifo_sw_cache = NULL;
    bcmcth_mon_flowtracker_event_log_entry_t lt_entry_info = {0};
    SHR_FUNC_ENTER(unit);


    /* Sanity check of count variable */
    if (count > BCM56780_A0_CTH_MON_FT_LRN_NOTIF_FIFO_NUM_ENTRIES) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }


    fifo_sw_cache = BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_ENTRIES_PTR(unit, pipe);

    if (fifo_sw_cache == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    BCMCTH_MON_FT_LEARN_FIFO_SW_CACHE_ITER(fifo_sw_cache, entry, count) {
        sal_memset(&lt_entry_info, 0, sizeof(lt_entry_info));

        bcm56780_a0_cth_mon_ft_fifo_sw_cache_entry_evt_log_lt_entry_convert(
                unit, pipe, entry, &lt_entry_info);


        /*
         * Do a dummy insert into MON_FLOWTRACKER_LEARN_FLOW_ENTRY LT to
         * enable lookup of the flow entry from HW.
         */
        SHR_IF_ERR_EXIT(
            bcmcth_mon_ft_learn_flow_entry_dummy_insert(unit,
                                            lt_entry_info.pipe,
                                            lt_entry_info.mode,
                                            lt_entry_info.exact_match_idx));

        SHR_IF_ERR_EXIT(
            bcmcth_mon_ft_learn_event_log_entry_update(unit, &lt_entry_info));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize most of the SBUSDMA work pointer and SBUSDMA work data pointer
 * which does not change dynamically for later use.
 */
static int bcm56780_a0_ft_fifo_sbusdma_work_ptr_init(int unit, int pipe)
{
    bcmbd_sbusdma_work_t *work_ptr = NULL;
    bcmbd_sbusdma_data_t *work_data_ptr = NULL;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t cwords_array[4], num_cwords;
    SHR_FUNC_ENTER(unit);

    work_ptr = BCM56780_A0_FT_FIFO_SBUSDMA_WORK_PTR(unit, pipe);
    work_data_ptr = work_ptr->data;

    sal_memset(&pt_dyn_info, 0, sizeof(pt_dyn_info));
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = pipe;
    SHR_IF_ERR_EXIT(
            bcmbd_pt_cmd_hdr_get(unit, FT_LEARN_NOTIFY_FIFOm, &pt_dyn_info, NULL,
                BCMBD_PT_CMD_POP, 4, cwords_array, &num_cwords));

    work_data_ptr->start_addr = cwords_array[1];
    work_data_ptr->data_width = BCM56780_A0_CTH_MON_FT_LRN_NOTIF_FIFO_ENTRY_WSIZE;
    work_data_ptr->op_code = cwords_array[0];
    work_data_ptr->buf_paddr = BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_PADDR(unit, pipe);

    work_ptr->items = 1;
    /*
     * FIFO POP is done. So read command. And also for fifo operations,
     * SBUSDMA_WF_SGL_ADDR is set so that the SBUSDMA engine can
     * just operate using single start address and need not increment for each
     * pop operation internally.
     */
    work_ptr->flags = (SBUSDMA_WF_READ_CMD | SBUSDMA_WF_SGL_ADDR);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Destroy the data structures for FIFO SW cache
 * and SBUSDMA work pointers.
 */
static void bcm56780_a0_cth_mon_ft_fifo_drv_deinit(int unit)
{
    int pipe;
    bool sim = bcmdrd_feature_is_sim(unit);
    for (pipe = 0; pipe < BCM56780_A0_CTH_MON_MAX_PIPES; pipe++) {
        if (!sim && BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_PTR(unit, pipe) != NULL) {
            /* Free up the FIFO cache(s) for given pipe */
            bcmdrd_hal_dma_free(unit, BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_SIZE,
                    BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_PTR(unit, pipe),
                    BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_PADDR(unit, pipe));
            BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_PTR(unit, pipe) = NULL;
        }

        /* Free up the SBUSDMA work data pointers for given pipe */
        SHR_FREE(BCM56780_A0_FT_FIFO_SBUSDMA_WORK_DATA_PTR(unit, pipe));
    }
    /* Free up the FIFO cache(s) pointers for given unit */
    SHR_FREE(BCM56780_A0_FT_FIFO_SW_CACHE_PTR(unit));

    /* Free up the SBUSDMA work pointers */
    SHR_FREE(BCM56780_A0_FT_FIFO_ALL_PIPES_SBUSDMA_WORK_PTR(unit));
}

/*
 * Allocate and initialize the data structures for FIFO SW cache
 * and SBUSDMA work pointers.
 */
static int bcm56780_a0_cth_mon_ft_fifo_drv_init(int unit)
{
    int pipe;
    bool sim = bcmdrd_feature_is_sim(unit);
    SHR_FUNC_ENTER(unit);

    /* Allocate SBUSDMA work pointers for given unit */
    SHR_ALLOC(BCM56780_A0_FT_FIFO_ALL_PIPES_SBUSDMA_WORK_PTR(unit),
              BCM56780_A0_FT_FIFO_ALL_PIPES_SBUSDMA_WORK_SIZE,
              "bcm56780CthMonFtFifoSbusdmaWork");
    SHR_NULL_CHECK(BCM56780_A0_FT_FIFO_ALL_PIPES_SBUSDMA_WORK_PTR(unit),
            SHR_E_MEMORY);
    sal_memset(BCM56780_A0_FT_FIFO_ALL_PIPES_SBUSDMA_WORK_PTR(unit),0,
               BCM56780_A0_FT_FIFO_ALL_PIPES_SBUSDMA_WORK_SIZE);


    /* Allocate FIFO sw cache(s) pointers for given unit */
    SHR_ALLOC(BCM56780_A0_FT_FIFO_SW_CACHE_PTR(unit),
              BCM56780_A0_FT_FIFO_SW_CACHE_SIZE,
              "bcm56780CthMonFtFifoSwCachePtrs");
    SHR_NULL_CHECK(BCM56780_A0_FT_FIFO_SW_CACHE_PTR(unit), SHR_E_MEMORY);
    sal_memset(BCM56780_A0_FT_FIFO_SW_CACHE_PTR(unit),0,
               BCM56780_A0_FT_FIFO_SW_CACHE_SIZE);

    for (pipe = 0; pipe < BCM56780_A0_CTH_MON_MAX_PIPES; pipe++) {
        /* Allocate SBUSDMA work data pointers for given pipe */
        SHR_ALLOC(BCM56780_A0_FT_FIFO_SBUSDMA_WORK_DATA_PTR(unit, pipe),
                  BCM56780_A0_FT_FIFO_SBUSDMA_WORK_DATA_SIZE,
                  "bcm56780CthMonFtFifoSbusdmaWorkData");
        SHR_NULL_CHECK(BCM56780_A0_FT_FIFO_SBUSDMA_WORK_DATA_PTR(unit,pipe),
                SHR_E_MEMORY);
        sal_memset(BCM56780_A0_FT_FIFO_SBUSDMA_WORK_DATA_PTR(unit, pipe), 0,
                   BCM56780_A0_FT_FIFO_SBUSDMA_WORK_DATA_SIZE);

        if (!sim) {
            /* Allocate FIFO sw cache(s) for given pipe */
            BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_PTR(unit, pipe) =
                bcmdrd_hal_dma_alloc(unit,
                        BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_SIZE,
                        "bcm56780CthMonFtFifoSwCache",
                        BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_PADDR_PTR(unit,pipe));
            SHR_NULL_CHECK(BCM56780_A0_FT_FIFO_PIPE_SW_CACHE_PTR(unit,pipe),
                    SHR_E_MEMORY);
        }

        /* Setting up of SBUSDMA work pointer */
        SHR_IF_ERR_EXIT(bcm56780_a0_ft_fifo_sbusdma_work_ptr_init(unit, pipe));
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcm56780_a0_cth_mon_ft_fifo_drv_deinit(unit);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public APIs or interfaces
 */

/* BCM56780_A0 FT FIFO Driver structure */
bcmcth_mon_flowtracker_fifo_drv_t bcm56780_a0_cth_mon_ft_fifo_drv = {
    .drv_init                        = &bcm56780_a0_cth_mon_ft_fifo_drv_init,
    .drv_deinit                      = &bcm56780_a0_cth_mon_ft_fifo_drv_deinit,
    .sw_cache_refresh                = &bcm56780_a0_cth_mon_ft_fifo_sw_cache_refresh,
    .entry_count_get                 = &bcm56780_a0_cth_mon_ft_fifo_entry_count_get,
    .fifo_behavior_set               = &bcm56780_a0_cth_mon_ft_fifo_behavior_set,
    .sw_cache_traverse_and_report    = &bcm56780_a0_cth_mon_ft_fifo_sw_cache_traverse_and_report
};
