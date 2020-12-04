/*! \file bcm56780_a0_cth_mon_ft_intr.c
 *
 * Chip specific routines for configuring and
 * handling FT_LEARN_NOTIFY_FIFO memory related interrupts
 * (top level and feature specific).
 * Memory/registers: FT_LEARN_CTRLr, FT_LEARN_INTR_ENABLEr
 * Also bcmbd calls for enabling/disabling top level interrupts and setting
 * interrupt handler for top level interrupts.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>

#include <bcmltd/bcmltd_types.h>
#include <bcmltd/id/bcmltd_common_id.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmbd/bcmbd_ipep_intr.h>
#include <bcmbd/chip/bcm56780_a0_ipep_intr.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>

#include <bcmcth/bcmcth_imm_util.h>
#include <bcmcth/bcmcth_mon_flowtracker_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/******************************************************************************
* Private functions
 */
/*
 * This routines enables/disables the top level interrupt for every
 * new flow learnt in HW and flow learn fail in HW
 */
static int bcm56780_a0_cth_mon_ft_learn_top_lvl_intr_enable(int unit,
                                                            bool enable)
{
    SHR_FUNC_ENTER(unit);

    /* Top level interrupt enable/disable */
    if (enable == true) {
        SHR_IF_ERR_EXIT(bcmbd_ipep_intr_enable(unit, FT_LEARN_INTR));
    } else {
        SHR_IF_ERR_EXIT(bcmbd_ipep_intr_disable(unit, FT_LEARN_INTR));
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * This routines enables/disables the interrupt for every new flow learnt in HW
 * and pushed into FT_LEARN_NOTIFY_FIFO memory.
 * NOTE that the interrupt will get triggered only when the threshold crosses
 * a value set using bcm56780_a0_cth_mon_ft_learn_intr_threshold_set API.
 * Configured register/field : FT_LEARN_INTR_ENABLE.FT_LEARN_NOTIFY_INTR_ENABLE
 */
static int bcm56780_a0_cth_mon_ft_learn_intr_enable(int unit,
                                                    int pipe,
                                                    bool enable)
{
    bcmltd_sid_t lt_id;
    bcmdrd_sid_t pt_id;
    int index;
    FT_LEARN_INTR_ENABLEr_t entry = {{0}};
    SHR_FUNC_ENTER(unit);

    lt_id = MON_FLOWTRACKER_LEARN_EVENT_CONTROLt;
    pt_id = FT_LEARN_INTR_ENABLEr;
    index = 0;

    SHR_IF_ERR_EXIT(
            bcmcth_imm_pipe_ireq_read(unit, lt_id, pt_id, pipe, index, &entry));
    FT_LEARN_INTR_ENABLEr_FT_LEARN_NOTIFY_INTR_ENABLEf_SET(entry, enable);
    SHR_IF_ERR_EXIT(
            bcmcth_imm_pipe_ireq_write(unit, lt_id, pt_id, pipe, index, &entry));

exit:
    SHR_FUNC_EXIT();
}

/*
 * This routines enables/disables the interrupt for every flow that failed to
 * learn in HW due to hash table being full.
 * NOTE that the interrupt will get triggered only when the threshold crosses
 * a value set using bcm56780_a0_cth_mon_ft_learn_fail_intr_threshold_set API.
 * Configured register/field : FT_LEARN_INTR_ENABLE.FT_LEARN_FAIL_INTR_ENABLE
 */
static int bcm56780_a0_cth_mon_ft_learn_fail_intr_enable(int unit,
                                                         int pipe,
                                                         bool enable)
{
    bcmltd_sid_t lt_id;
    bcmdrd_sid_t pt_id;
    int index;
    FT_LEARN_INTR_ENABLEr_t entry = {{0}};
    SHR_FUNC_ENTER(unit);

    lt_id = MON_FLOWTRACKER_LEARN_EVENT_CONTROLt;
    pt_id = FT_LEARN_INTR_ENABLEr;
    index = 0;

    SHR_IF_ERR_EXIT(
            bcmcth_imm_pipe_ireq_read(unit, lt_id, pt_id, pipe, index, &entry));
    FT_LEARN_INTR_ENABLEr_FT_LEARN_FAIL_INTR_ENABLEf_SET(entry, enable);
    SHR_IF_ERR_EXIT(
            bcmcth_imm_pipe_ireq_write(unit, lt_id, pt_id, pipe, index, &entry));
exit:
    SHR_FUNC_EXIT();
}

/*
 * This routines configures the threshold for number of entries to be present
 * in FT_LEARN_NOTIFY_FIFO memory before an interrupt is raised to the HOST CPU.
 * Configured register/field : FT_LEARN_CTRL.FT_LEARN_NOTIFY_INTR_THRESHOLD
 */
static int bcm56780_a0_cth_mon_ft_learn_intr_threshold_set(int unit,
                                                           int pipe,
                                                           uint16_t threshold)
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
    FT_LEARN_CTRLr_FT_LEARN_NOTIFY_INTR_THRESHOLDf_SET(entry, threshold);
    SHR_IF_ERR_EXIT(
            bcmcth_imm_pipe_ireq_write(unit, lt_id, pt_id, pipe, index, &entry));
exit:
    SHR_FUNC_EXIT();
}

/*
 * This routines configures the threshold for the value in FT_LEARN_FAIL_COUNTr
 * to cross before an interrupt is raised to HOST CPU.
 * Configured register/field : FT_LEARN_CTRL.FT_LEARN_FAIL_INTR_THRESHOLD
 */
static int bcm56780_a0_cth_mon_ft_learn_fail_intr_threshold_set(int unit,
                                                                int pipe,
                                                                uint16_t threshold)
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
    FT_LEARN_CTRLr_FT_LEARN_FAIL_INTR_THRESHOLDf_SET(entry, threshold);
    SHR_IF_ERR_EXIT(
            bcmcth_imm_pipe_ireq_write(unit, lt_id, pt_id, pipe, index, &entry));
exit:
    SHR_FUNC_EXIT();
}

/*
 * This routine gets the interrupt enable status of learn success
 * or learn failure events.
 * Lookup register/field: FT_LEARN_INTR_ENABLE
 */
static int bcm56780_a0_cth_mon_ft_learn_intr_enable_get(int unit,
                                                        int pipe,
                                                        bool *lrn_intr_ena,
                                                        bool *lrn_fail_intr_ena)
{
    bcmltd_sid_t lt_id;
    bcmdrd_sid_t pt_id;
    int index;
    FT_LEARN_INTR_ENABLEr_t entry = {{0}};
    SHR_FUNC_ENTER(unit);
    lt_id = MON_FLOWTRACKER_LEARN_EVENT_CONTROLt;
    pt_id = FT_LEARN_INTR_ENABLEr;
    index = 0;

    SHR_IF_ERR_EXIT(
            bcmcth_imm_pipe_ireq_read(unit, lt_id, pt_id, pipe, index, &entry));
    *lrn_intr_ena =
            FT_LEARN_INTR_ENABLEr_FT_LEARN_NOTIFY_INTR_ENABLEf_GET(entry);
    *lrn_fail_intr_ena =
            FT_LEARN_INTR_ENABLEr_FT_LEARN_FAIL_INTR_ENABLEf_GET(entry);
exit:
    SHR_FUNC_EXIT();
}

/*
 * This routine gets the indication for interrupt due to
 * learn success or learn failure.
 * Lookup register/fields:
 * FT_LEARN_INTR_STATUS.FT_LEARN_FAIL_INTR_STATUS
 * FT_LEARN_INTR_STATUS.FT_LEARN_NOTIFY_INTR_STATUS
 */
static int bcm56780_a0_cth_mon_ft_learn_intr_status_get(int unit,
                                                        int pipe,
                                                        bool *lrn_intr_set,
                                                        bool *lrn_fail_intr_set)
{
    bcmltd_sid_t lt_id;
    bcmdrd_sid_t pt_id;
    int index;
    FT_LEARN_INTR_STATUSr_t entry = {{0}};
    SHR_FUNC_ENTER(unit);
    lt_id = MON_FLOWTRACKER_LEARN_EVENT_CONTROLt;
    pt_id = FT_LEARN_INTR_STATUSr;
    index = 0;

    SHR_IF_ERR_EXIT(
            bcmcth_imm_pipe_ireq_read(unit, lt_id, pt_id, pipe, index, &entry));
    *lrn_intr_set =
            FT_LEARN_INTR_STATUSr_FT_LEARN_NOTIFY_INTR_STATUSf_GET(entry);
    *lrn_fail_intr_set =
            FT_LEARN_INTR_STATUSr_FT_LEARN_FAIL_INTR_STATUSf_GET(entry);
exit:
    SHR_FUNC_EXIT();
}

/*
 * This routine clears the indications for interrupt due to
 * learn success or learn failure.
 * Cleared register: FT_LEARN_INTR_STATUS
 */
static int bcm56780_a0_cth_mon_ft_learn_intr_status_clear(int unit,
                                                        int pipe)
{
    bcmltd_sid_t lt_id;
    bcmdrd_sid_t pt_id;
    int index;
    FT_LEARN_INTR_STATUSr_t entry;
    SHR_FUNC_ENTER(unit);
    lt_id = MON_FLOWTRACKER_LEARN_EVENT_CONTROLt;
    pt_id = FT_LEARN_INTR_STATUSr;
    index = 0;

    FT_LEARN_INTR_STATUSr_CLR(entry);
    SHR_IF_ERR_EXIT(
            bcmcth_imm_pipe_ireq_write(unit, lt_id, pt_id, pipe, index, &entry));
exit:
    SHR_FUNC_EXIT();
}

/*
 * This routine sets the interrupt handler for top level learn and
 * learn fail interrupts.
 */
static int bcm56780_a0_cth_mon_ft_learn_intr_handler_set(
                                                    int unit,
                                                    bcmbd_intr_f intr_hdlr,
                                                    uint32_t intr_param)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmbd_ipep_intr_func_set(unit, FT_LEARN_INTR,
                                            intr_hdlr, intr_param));
exit:
    SHR_FUNC_EXIT();
}

/*
 * This routine clears the FT_LEARN_FAIL_COUNTERr register. Needs to be called
 * once fail interrupt is handled, so that further comparisons of this counter
 * against this threshold happens for new failures only.
 */
static int bcm56780_a0_cth_mon_ft_learn_fail_counter_clear(
                                                    int unit,
                                                    int pipe)
{
    bcmltd_sid_t lt_id;
    bcmdrd_sid_t pt_id;
    int index;
    FT_LEARN_FAIL_COUNTERr_t entry;
    SHR_FUNC_ENTER(unit);
    lt_id = MON_FLOWTRACKER_LEARN_EVENT_CONTROLt;
    pt_id = FT_LEARN_FAIL_COUNTERr;
    index = 0;

    FT_LEARN_FAIL_COUNTERr_CLR(entry);
    SHR_IF_ERR_EXIT(
            bcmcth_imm_pipe_ireq_write(unit, lt_id, pt_id, pipe, index, &entry));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public APIs or interfaces
 */

/* BCM56780_A0 FT INTR Driver structure */
bcmcth_mon_flowtracker_intr_drv_t bcm56780_a0_cth_mon_ft_intr_drv = {
    .ft_learn_top_lvl_intr_enable     = &bcm56780_a0_cth_mon_ft_learn_top_lvl_intr_enable,
    .ft_learn_intr_enable             = &bcm56780_a0_cth_mon_ft_learn_intr_enable,
    .ft_learn_fail_intr_enable        = &bcm56780_a0_cth_mon_ft_learn_fail_intr_enable,
    .ft_learn_intr_threshold_set      = &bcm56780_a0_cth_mon_ft_learn_intr_threshold_set,
    .ft_learn_fail_intr_threshold_set = &bcm56780_a0_cth_mon_ft_learn_fail_intr_threshold_set,
    .ft_learn_intr_status_get         = &bcm56780_a0_cth_mon_ft_learn_intr_status_get,
    .ft_learn_intr_status_clear       = &bcm56780_a0_cth_mon_ft_learn_intr_status_clear,
    .ft_learn_intr_handler_set        = &bcm56780_a0_cth_mon_ft_learn_intr_handler_set,
    .ft_learn_intr_enable_get         = &bcm56780_a0_cth_mon_ft_learn_intr_enable_get,
    .ft_learn_fail_counter_clear      = &bcm56780_a0_cth_mon_ft_learn_fail_counter_clear
};
