/*! \file bcmtm_imm_pfc_deadlock.c
 *
 * In-memory call back function implementation for PFC deadlock Logical tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <bcmtm/pfc/bcmtm_pfc_deadlock_internal.h>
#include <bcmltd/bcmltd_bitop.h>
#include <bcmtm/bcmtm_ha_internal.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>

/*******************************************************************************
 * Local definitions
 */
bcmtm_pfc_ddr_ctrl_t *bcmtm_pfc_ctrl[BCMDRD_CONFIG_MAX_UNITS];

#define VALID_OP                BCMLTD_COMMON_TM_PORT_ENTRY_STATE_T_T##_VALID
#define PORT_INFO_UNAVAIL_OP    BCMLTD_COMMON_TM_PORT_ENTRY_STATE_T_T##_PORT_INFO_UNAVAILABLE

/* Protect the critical zone. */
#define PFC_DDR_INFO_LOCK(unit) \
    sal_mutex_take(bcmtm_pfc_ctrl[unit]->info_lock, SAL_MUTEX_FOREVER)

#define PFC_DDR_INFO_UNLOCK(unit) \
    sal_mutex_give(bcmtm_pfc_ctrl[unit]->info_lock)

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

static int
bcmtm_pfc_deadlock_config_set(int unit,
                              bcmltd_sid_t ltid,
                              uint32_t fid,
                              bcmtm_lport_t lport,
                              uint32_t pri,
                              uint32_t val);

int
bcmtm_pfc_deadlock_recovery_status_populate_imm_entry(int unit ,
                                                      int lport,
                                                      uint32_t pri,
                                                      bool recovery,
                                                      bool deadlock_hdlr,
                                                      bool port_up)
{
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    const bcmlrd_map_t *map = NULL;
    size_t num_fid;
    bcmltd_sid_t ltid = TM_PFC_DEADLOCK_RECOVERY_STATUSt;
    int pport;
    int rv = 0;
    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, ltid, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fid));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &in));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    in.count = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERY_STATUSt_PORT_IDf,
                              0, lport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERY_STATUSt_PFC_PRIf,
                              0, pri));
    /*
     * TRUE indicates the function called from PFC deadlock event handling,
     * update recovery field.
     */
    if (deadlock_hdlr == TRUE) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in,
                                  TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERYf,
                                  0, recovery));
    } else {
        /* Check port ID. */
        if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport)) ||
                               (port_up == FALSE)) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in,
                    TM_PFC_DEADLOCK_RECOVERY_STATUSt_OPERATIONAL_STATEf,
                    0, PORT_INFO_UNAVAIL_OP));

            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in,
                     TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf, 0, 0));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmtm_pfc_deadlock_config_set(unit,
                     TM_PFC_DEADLOCK_RECOVERY_STATUSt,
                     TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf,
                     lport, pri, 0));
        } else {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in,
                    TM_PFC_DEADLOCK_RECOVERY_STATUSt_OPERATIONAL_STATEf,
                    0, VALID_OP));
        }
    }

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, FALSE, ltid, &in));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, ltid, &in));
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief PFC deadlock recovery action end function.
 *
 * The function is used to bring the hardware out of recovery
 * state.
 *
 * \param [in] unit Unit number.
 * \param [in] lport device port number.
 * \param [in] pri PFC priority.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_pfc_deadlock_recovery_end(int unit, bcmtm_lport_t lport, uint32_t pri)
{
    bcmtm_pfc_ddr_info_t *pfcinfo;
    bcmtm_drv_t *drv;
    bcmtm_pfc_ddr_drv_t *pfc_ddr_drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmtm_pfc_ddr_info_get(unit, &pfcinfo);
    SHR_NULL_CHECK(pfcinfo, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));

    pfc_ddr_drv =  drv->pfc_ddr_drv_get(unit);
    SHR_NULL_CHECK(pfc_ddr_drv, SHR_E_PARAM);

    if (pfc_ddr_drv->hw_recovery_end) {
        SHR_IF_ERR_EXIT
            (pfc_ddr_drv->hw_recovery_end(unit, lport, pri));

        /* LT notification. */
        SHR_IF_ERR_EXIT
            (bcmtm_pfc_deadlock_recovery_status_populate_imm_entry(unit,
                                                               lport,
                                                               pri,
                                                               FALSE, TRUE, FALSE));
        pfcinfo[lport].in_recovery &= ~(1U << pri);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Brings out of dead lock recovery action before
 *  the recovery timer expiry.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_pfc_deadlock_recovery_force_end(int unit, bcmtm_lport_t lport, uint32_t pri)
{
    bcmtm_pfc_ddr_ctrl_t *pfcctrl = bcmtm_pfc_ctrl[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pfcctrl, SHR_E_INIT);
    pfcctrl->recovery_count[lport][pri] = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_pfc_deadlock_recovery_end(unit, lport, pri));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Function implementing software recovery timer.
 *
 * This function triggers the pfc dead recovery end process when the recovery time
 * period expires.
 *
 * \param [in] unit Unit number.
 * \param [in] lport device port number.
 * \param [in] pri PFC priority.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_pfc_deadlock_recovery_timer_refresh(int unit, bcmtm_lport_t lport, uint32_t pri)
{
    bcmtm_pfc_ddr_ctrl_t *pfcctrl = bcmtm_pfc_ctrl[unit];
    bcmtm_pfc_ddr_info_t *pfcinfo;

    SHR_FUNC_ENTER(unit);

    bcmtm_pfc_ddr_info_get(unit, &pfcinfo);
    SHR_NULL_CHECK(pfcctrl, SHR_E_INIT);
    SHR_NULL_CHECK(pfcinfo, SHR_E_INIT);

    if ((--pfcctrl->recovery_count[lport][pri]) == 0) {
         SHR_IF_ERR_EXIT
             (bcmtm_pfc_deadlock_recovery_end(unit, lport, pri));
    } else {
        /* bcmtm_pfc_info[lport].recovery_count is set to value + 1, for the reason
         * first time i.e when recovery is initiated this is called immediately,
         * and before PFC_SCAN_INTERVAL ticks.
         */
        pfcinfo[lport].recovery_count_acc[pri]++;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handles deadlock status monitor from hardware.
 *
 * This function also triggers the pfc dead recovery start when a port,
 * pfc priority is declared to be in dead lock by the hardware.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_pfc_deadlock_process(int unit)
{
    int mport = -1;
    bcmtm_pport_t pport = 0;
    int lport = 0;
    bcmdrd_pbmp_t pbmp;
    bcmtm_pfc_ddr_info_t *pfcinfo;
    uint32_t fval_status = 0, fval_mask = 0, pri = 0;
    bcmtm_pfc_ddr_ctrl_t *pfcctrl = bcmtm_pfc_ctrl[unit];
    bcmtm_drv_t *drv;
    bcmtm_pfc_ddr_drv_t *pfc_ddr_drv = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pfcctrl, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    pfc_ddr_drv =  drv->pfc_ddr_drv_get(unit);
    SHR_NULL_CHECK(pfc_ddr_drv, SHR_E_PARAM);

    if (pfc_ddr_drv->hw_status_get == NULL) {
        SHR_EXIT();
    }

    bcmtm_pfc_ddr_info_get(unit, &pfcinfo);
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_ports_get(unit, &pbmp));
    BCMDRD_PBMP_ITER(pbmp, pport) {
        if (SHR_FAILURE(bcmtm_pport_lport_get(unit, pport, &lport))) {
            continue;
        }
        if (pfcinfo[lport].ddr_enable == 0) {
           /* port is not enabled for PFC deadlock detection. */
            continue;
        }

        SHR_IF_ERR_EXIT
            (bcmtm_lport_mport_get(unit, lport, &mport));

       /* Read the PT interrupt mask and status registers. */
       SHR_IF_ERR_EXIT
           (pfc_ddr_drv->hw_status_get(unit, lport,
                                       &fval_mask, &fval_status));
       /* If the interrupt is not masked,
        * means the recovery is not started and we are seeing the interrupt
        * for the first time.
        */
        fval_status = fval_status & (~fval_mask);
        if (fval_status != 0) {
            for (pri = 0; pri < MAX_NUM_PFC_PRI; pri++) {
                if ((fval_status & (1U << pri)) &&
                        ((pfcinfo[lport].ddr_enable) & (1U << pri))) {
                    if (pfc_ddr_drv->hw_recovery_begin) {
                        SHR_IF_ERR_EXIT
                            (pfc_ddr_drv->hw_recovery_begin(unit, lport, pri));

                        /* LT notification. */
                        SHR_IF_ERR_EXIT
                            (bcmtm_pfc_deadlock_recovery_status_populate_imm_entry(unit,
                                                               lport,
                                                               pri,
                                                               TRUE, TRUE, FALSE));

                       /* Initialize the recovery count. */
                        pfcctrl->recovery_count[lport][pri] =
                                pfcinfo[lport].recovery_count_cfg[pri] + 1;
                        pfcinfo[lport].in_recovery |= (1U << pri);
                    }
                }
            }
        }

        /* Update the count for priority already in Deadlock state and reset
         * those priority where recovery timer has expired.
         */
        for (pri = 0; pri < MAX_NUM_PFC_PRI; pri++) {
            if ((pfcinfo[lport].in_recovery & (1U << pri)) != 0) {
                 SHR_IF_ERR_EXIT
                     (bcmtm_pfc_deadlock_recovery_timer_refresh(unit, lport, pri));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Deadlock detection monitor polling thread.
 *
 * \param [in] arg Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static void
bcmtm_pfc_ddr_scan_thread(shr_thread_t tc, void *arg)
{
    int unit = (unsigned long)(arg);
    bcmtm_pfc_ddr_ctrl_t *ddrc = bcmtm_pfc_ctrl[unit];
    int rv = SHR_E_NONE;

    while (ddrc->state == ACTIVE) {
        shr_thread_sleep(tc, PFC_SCAN_INTERVAL);
        PFC_DDR_INFO_LOCK(unit);

        rv = bcmtm_pfc_deadlock_process(unit);
        PFC_DDR_INFO_UNLOCK(unit);
        if (rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LS_BCMTM_IMM,
                          (BSL_META("Error in handling PFC deadlock .\n")));
        }
    }
}

/*!
 * \brief Function to initiate the dead lock monitor polling thread.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_pfc_deadlock_monitor_enable(int unit, bool enable)
{
    bcmtm_pfc_ddr_ctrl_t *ddrc = bcmtm_pfc_ctrl[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ddrc, SHR_E_INIT);

    if (enable) {
        /* PFC DDR thread is already running. */
        if (ddrc->pid) {
            SHR_EXIT();
        }

        ddrc->state = ACTIVE;
        ddrc->pid = shr_thread_start("bcmtmPfcDdrScan",
                              SAL_THREAD_PRIO_DEFAULT,
                              bcmtm_pfc_ddr_scan_thread,
                              ((void *)(unsigned long)(unit)));
        if (ddrc->pid == NULL) {
            ddrc->state = INVALID;
            SHR_IF_ERR_EXIT(SHR_E_FAIL);
        }
    } else {
        if (ddrc->pid == NULL) {
            /* PFC DDR thread is already killed. */
            SHR_EXIT();
        }
        ddrc->state = SHUTDOWN;
        /* Wait for thread to exit. */
        SHR_IF_ERR_EXIT(
            shr_thread_stop(ddrc->pid, PFC_SCAN_INTERVAL*5));
        ddrc->pid = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_pfc_deadlock_config_set(int unit,
                          bcmltd_sid_t ltid,
                          uint32_t fid,
                          bcmtm_lport_t lport,
                          uint32_t pri,
                          uint32_t val)
{
    bcmtm_pfc_ddr_info_t *pfcinfo;
    uint32_t idx = 0;
    uint32_t ddr_monitor = 0;
    int rv = 0;
    bcmtm_drv_t *drv;
    bcmtm_pfc_ddr_drv_t *pfc_ddr_drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmtm_pfc_ddr_info_get(unit, &pfcinfo);
    SHR_NULL_CHECK(pfcinfo, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    pfc_ddr_drv =  drv->pfc_ddr_drv_get(unit);

    SHR_NULL_CHECK(pfc_ddr_drv, SHR_E_PARAM);

    SHR_NULL_CHECK(pfc_ddr_drv->pt_set, SHR_E_INIT);
    if (ltid == TM_PFC_DEADLOCK_RECOVERY_STATUSt) {
        if (fid == TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf) {
            PFC_DDR_INFO_LOCK(unit);
            pfcinfo[lport].recovery_count_acc[pri] = val;
            PFC_DDR_INFO_UNLOCK(unit);
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        if (fid == TM_PFC_DEADLOCK_RECOVERYt_DEADLOCK_RECOVERYf) {
            PFC_DDR_INFO_LOCK(unit);
            if (val == 1) {
                SHR_BITSET(&pfcinfo[lport].ddr_enable, pri);
                ddr_monitor = 1;
            } else {
                SHR_BITCLR(&pfcinfo[lport].ddr_enable, pri);
                ddr_monitor = 0;
                for (idx = 0; idx < BCMDRD_CONFIG_MAX_PORTS; idx++) {
                    if (pfcinfo[lport].ddr_enable != 0) {
                        ddr_monitor = 1;
                        break;
                     }
                }

                /* handle recovery in progress. */
                if ((pfcinfo[lport].in_recovery & (1U << pri)) != 0) {
                    rv = bcmtm_pfc_deadlock_recovery_force_end(unit, lport, pri);
                    if (rv != SHR_E_NONE) {
                        PFC_DDR_INFO_UNLOCK(unit);
                        SHR_ERR_EXIT(rv);
                    }
                }
             }
             PFC_DDR_INFO_UNLOCK(unit);
             SHR_IF_ERR_EXIT
                 (bcmtm_pfc_deadlock_monitor_enable(unit,
                                            (ddr_monitor == 1) ? true : false));
             SHR_IF_ERR_EXIT
                 (pfc_ddr_drv->pt_set(unit, fid, lport, pri, val));
         } else if (fid == TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_TIMERf) {
             PFC_DDR_INFO_LOCK(unit);
             pfcinfo[lport].recovery_count_cfg[pri] = val;
             PFC_DDR_INFO_UNLOCK(unit);
         } else if (fid == TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMER_GRANULARITYf) {
             SHR_IF_ERR_EXIT
             (pfc_ddr_drv->pt_set(unit, fid, lport, pri, val));
         } else if (fid == TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMERf) {
             SHR_IF_ERR_EXIT
             (pfc_ddr_drv->pt_set(unit, fid, lport, pri, val));
         } else {
             SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Resets the PFC Egress port physical table to default values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_pfc_deadlock_pt_reset(int unit,
                            bcmltd_sid_t ltid,
                            bcmtm_lport_t lport,
                            uint32_t pfc_pri)
{
    int mport;
    uint32_t fid = 0;

    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
        SHR_EXIT();
    }
    /* Physical table update. */
    for (fid = TM_PFC_DEADLOCK_RECOVERYt_DEADLOCK_RECOVERYf;
                    fid <= TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_TIMERf; fid++) {
        SHR_IF_ERR_EXIT
            (bcmtm_pfc_deadlock_config_set(unit, ltid, fid, lport, pfc_pri, 0));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief  PFC deadlock recovery status function.
 *
 * \param [in]  unit  Unit number.
 * \param [in]  fid Logical table field ID.
 * \param [in]  lport Logical port number.
 * \param [in]  pfc_pri PFC priority
 * \param [out] val Logical table field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_pfc_deadlock_recovery_status_get(int unit,
                                       uint32_t fid,
                                       bcmtm_lport_t lport,
                                       uint32_t pfc_pri,
                                       uint32_t *val)
{
    bcmtm_pfc_ddr_info_t *pfcinfo;

    SHR_FUNC_ENTER(unit);

    bcmtm_pfc_ddr_info_get(unit, &pfcinfo);
    SHR_NULL_CHECK(pfcinfo, SHR_E_INIT);

    if (fid == TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf) {
        *val = pfcinfo[lport].recovery_count_acc[pfc_pri];
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_PFC_DEADLOCK_RECOVERY logical table callback function for staging.
 *
 * Handle TM_PFC_DEADLOCK_RECOVERY IMM table events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] data. This is a linked list of extra added data fields (mostly
 * read only fields).
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmtm_pfc_ddr_stage(int unit,
                    bcmltd_sid_t ltid,
                    uint32_t trans_id,
                    bcmimm_entry_event_t event_reason,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data,
                    void *context,
                    bcmltd_fields_t *out_fields)
{
    bcmtm_lport_t lport;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    uint32_t pfc_pri = 0;
    int32_t mport = 0;

    SHR_FUNC_ENTER(unit);

    /* TM_PFC_DEADLOCK_RECOVERYt_PORT_IDf */
    fid = TM_PFC_DEADLOCK_RECOVERYt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t)fval;

    /* PFC DDR is supported on front panel and management ports. */
    if (bcmtm_lport_is_lb(unit, lport) || bcmtm_lport_is_rdb(unit, lport)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* TM_PFC_DEADLOCK_RECOVERYt_PFC_PRIf */
    fid = TM_PFC_DEADLOCK_RECOVERYt_PFC_PRIf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    pfc_pri = (uint32_t)fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(out_fields, SHR_E_PARAM);

            /* Initialize output_fields. */
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_size_check(unit, out_fields, 1));
            out_fields->count = 0;

            if (SHR_FAILURE(bcmtm_lport_mport_get(unit, lport,
                                                    &mport))) {
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out_fields,
                                 TM_PFC_DEADLOCK_RECOVERYt_OPERATIONAL_STATEf,
                                 0, PORT_INFO_UNAVAIL_OP));
                SHR_EXIT();
            } else if (!data) {
                /*
                 * Data field not present in the insert command.
                 * Initialize the entry and return.
                 */
                out_fields->count = 0;
                return SHR_E_NONE;
            } else {
                for (fid = TM_PFC_DEADLOCK_RECOVERYt_DEADLOCK_RECOVERYf;
                      fid <= TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_TIMERf; fid++) {
                    if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                                    data, &fval))) {
                        SHR_IF_ERR_EXIT
                            (bcmtm_pfc_deadlock_config_set(unit,
                                                      ltid,
                                                      fid,
                                                      lport,
                                                      pfc_pri,
                                                     (uint32_t)fval));

                    }
                }

                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out_fields,
                                 TM_PFC_DEADLOCK_RECOVERYt_OPERATIONAL_STATEf,
                                 0, VALID_OP));
            }
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                    (bcmtm_pfc_deadlock_pt_reset(unit, ltid, lport, pfc_pri));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * In-memory PFC_DEADLOCK_RECOVERY_STATUS lookup callback helper function.
 */
static int
bcmtm_pfc_ddr_status_lookup (int unit,
                             bcmltd_sid_t sid,
                             uint32_t trans_id,
                             void *context,
                             bcmimm_lookup_type_t lkup_type,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out)
{
    const bcmltd_fields_t *key = NULL;
    bcmtm_lport_t lport = 0;
    uint32_t val = 0, i = 0, pri = 0;

    SHR_FUNC_ENTER(unit);

    /* key fields are in the "out" parameter for traverse operations */
    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    if (key) {
        for (i = 0; i < key->count; i++) {
            /* Get the keys value. */
            if (key->field[i]->id == TM_PFC_DEADLOCK_RECOVERY_STATUSt_PORT_IDf) {
                lport = key->field[i]->data;
            } else if ( key->field[i]->id == TM_PFC_DEADLOCK_RECOVERY_STATUSt_PFC_PRIf) {
                pri =  key->field[i]->data;
            }
        }
    }
    if (out) {
        for (i = 0; i < out->count; i++) {
           /* Set RECOVERY_CNT value. */
           if (out->field[i]->id == TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf) {

               SHR_IF_ERR_VERBOSE_EXIT
                   (bcmtm_pfc_deadlock_recovery_status_get(unit,
                                           TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf,
                                           lport, pri, &val));
               out->field[i]->data = val;
               break;
           }
       }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief In-memory logical table staging callback function.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key_fields This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data_field This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [out] out_fields This output parameter can be used by the component
 * to add fields into the entry. Typically this should be used for read-only
 * type fields that otherwise can not be set by the application.
 * The component must set the count field of the \c out_fields parameter to
 * indicate the actual number of fields that were set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmtm_pfc_ddr_status_stage(int unit,
                         bcmltd_sid_t sid,
                         uint32_t trans_id,
                         bcmimm_entry_event_t event_reason,
                         const bcmltd_field_t *key_fields,
                         const bcmltd_field_t *data_fields,
                         void *context,
                         bcmltd_fields_t *out_fields)
{
    uint32_t pri, cnt;
    int32_t  mport;
    bcmltd_fid_t ltfid;
    bcmtm_lport_t lport;
    uint64_t fval;
    uint8_t opcode;
    SHR_FUNC_ENTER(unit);

    ltfid = TM_PFC_DEADLOCK_RECOVERY_STATUSt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, ltfid, 0, key_fields,
                                         &fval));
    lport = (bcmtm_lport_t) fval;

    ltfid = TM_PFC_DEADLOCK_RECOVERY_STATUSt_PFC_PRIf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, ltfid, 0, key_fields,
                                         &fval));
    pri = (int)fval;


    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:

            SHR_NULL_CHECK(out_fields, SHR_E_PARAM);

            /* Parse data field for input entry. */
            if (!data_fields) {
                /*
                 * Data field not present in the insert command.
                 * Initialize the entry and return.
                 */
                out_fields->count = 0;
                return SHR_E_NONE;
            }

            out_fields->count = 0;

            /* TM local port information.
             * Returns error if local port to physical port map is not available.
             * Updates opeartional state.
             */
            if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
                opcode = PORT_INFO_UNAVAIL_OP;
                ltfid = TM_PFC_DEADLOCK_RECOVERY_STATUSt_OPERATIONAL_STATEf;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, out_fields, ltfid, 0, opcode));
                SHR_EXIT();
            }

            ltfid = TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf;
            SHR_IF_ERR_EXIT
               (bcmtm_fval_get_from_field_array(unit, ltfid, 0, data_fields,
                                                &fval));
            cnt = (uint32_t)fval;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmtm_pfc_deadlock_config_set(unit, sid,
                           TM_PFC_DEADLOCK_RECOVERY_STATUSt_RECOVERY_CNTf,
                           lport,
                           pri,
                           cnt));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_pfc_deadlock_imm_register(int unit)
{
    bcmimm_lt_cb_t tm_cb_handler;
    const bcmlrd_map_t *map = NULL;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    /*
     * Assign callback functions to IMM event callback handler.
     * commit and abort functions can be added if necessary.
     */

    sal_memset(&tm_cb_handler, 0, sizeof(tm_cb_handler));
    tm_cb_handler.stage    = bcmtm_pfc_ddr_stage;

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERYt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_PFC_DEADLOCK_RECOVERYt, &tm_cb_handler, NULL));

    /*
     * Assign callback functions to IMM event callback handler.
     * commit and abort functions can be added if necessary.
     */
    sal_memset(&tm_cb_handler, 0, sizeof(tm_cb_handler));
    tm_cb_handler.stage = bcmtm_pfc_ddr_status_stage;
    tm_cb_handler.lookup = bcmtm_pfc_ddr_status_lookup;

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERY_STATUSt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_PFC_DEADLOCK_RECOVERY_STATUSt,
                             &tm_cb_handler, NULL));
exit:
    SHR_FUNC_EXIT();
}


int
bcmtm_pfc_deadlock_recovery_status_imm_populate(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int lport, pri = 0;
    bcmltd_sid_t ltid = TM_PFC_DEADLOCK_RECOVERY_STATUSt;
    bcmlrd_field_def_t  field_def;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERY_STATUSt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    sal_memset(&field_def, 0, sizeof(field_def));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_def_get(unit,
                                    ltid,
                                    TM_PFC_DEADLOCK_RECOVERY_STATUSt_PORT_IDf,
                                    &field_def));

    for (lport = field_def.min.u16; lport <= field_def.max.u16; lport++) {
        if ((bcmtm_lport_is_fp(unit, (bcmtm_lport_t )lport)) ||
            (bcmtm_lport_is_mgmt(unit, (bcmtm_lport_t) lport))) {
            for (pri = 0; pri < MAX_NUM_PFC_PRI; pri++) {
                SHR_IF_ERR_EXIT
                    (bcmtm_pfc_deadlock_recovery_status_populate_imm_entry(unit,
                                                    (bcmtm_lport_t )lport, pri,
                                                    FALSE, FALSE, TRUE));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_pfc_deadlock_monitor_cleanup(int unit)
{
    bcmtm_pfc_ddr_ctrl_t *pfcctrl = bcmtm_pfc_ctrl[unit];
    const bcmlrd_map_t *map = NULL;
    int rv;
    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERYt, &map);

    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(pfcctrl, SHR_E_INIT);

    if (pfcctrl->info_lock) {
        sal_mutex_destroy(pfcctrl->info_lock);
    }

    sal_free(pfcctrl);
    bcmtm_pfc_ctrl[unit] = NULL;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_pfc_deadlock_monitor_stop(int unit)
{
    bcmtm_pfc_ddr_ctrl_t *ddrc = bcmtm_pfc_ctrl[unit];
    const bcmlrd_map_t *map = NULL;
    int rv;
    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERYt, &map);

    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(ddrc, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (bcmtm_pfc_deadlock_monitor_enable(unit, false));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_pfc_deadlock_monitor_init(int unit, bool warm)
{
    bcmtm_pfc_ddr_ctrl_t *pfcctrl = NULL;
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERYt, &map);

    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    pfcctrl = sal_alloc(sizeof(bcmtm_pfc_ddr_ctrl_t), "bcmtmPfcCtrl");

    if (!pfcctrl) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    sal_memset(pfcctrl, 0, sizeof(bcmtm_pfc_ddr_ctrl_t));

    pfcctrl->state = INIT;

    pfcctrl->info_lock = sal_mutex_create("bcmcthPfcDdrInfoLock");
    if (pfcctrl->info_lock == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    bcmtm_pfc_ctrl[unit] = pfcctrl;
exit:
    if (SHR_FUNC_ERR()) {
        if (pfcctrl && pfcctrl->info_lock) {
            sal_mutex_destroy(pfcctrl->info_lock);
            pfcctrl->info_lock = NULL;
            sal_free(pfcctrl);
        }
        bcmtm_pfc_ctrl[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmtm_pfc_deadlock_recovery_populate_imm_entry(int unit ,
                                               int lport,
                                               uint32_t pri,
                                               bool up)
{
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    const bcmlrd_map_t *map = NULL;
    size_t num_fid;
    bcmltd_sid_t ltid = TM_PFC_DEADLOCK_RECOVERYt;
    int pport;
    int rv = 0;
    bcmltd_fid_t fid;
    uint64_t fval;
    uint32_t val;
    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, ltid, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* PFC DDR is supported on front panel and management ports. */
    if (bcmtm_lport_is_lb(unit, lport) || bcmtm_lport_is_rdb(unit, lport)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fid));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &in));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    in.count = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERYt_PORT_IDf,
                              0, lport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in,
                              TM_PFC_DEADLOCK_RECOVERYt_PFC_PRIf,
                              0, pri));

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport)) ||
                               (up == FALSE)) {
            /* Port delete. */
            for (fid = TM_PFC_DEADLOCK_RECOVERYt_DEADLOCK_RECOVERYf;
                      fid <= TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_TIMERf; fid++) {

                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))) {
                    SHR_IF_ERR_EXIT
                        (bcmtm_pfc_deadlock_config_set(unit,
                                                       ltid,
                                                       fid,
                                                       lport,
                                                       pri,
                                                       0));
                }
            }
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in,
                       TM_PFC_DEADLOCK_RECOVERYt_OPERATIONAL_STATEf,
                       0, PORT_INFO_UNAVAIL_OP));
       } else {
           /* Port add .*/
           for (fid = TM_PFC_DEADLOCK_RECOVERYt_DEADLOCK_RECOVERYf;
                      fid <= TM_PFC_DEADLOCK_RECOVERYt_RECOVERY_TIMERf; fid++) {

                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))) {
                    val = (uint32_t)fval;
                    SHR_IF_ERR_EXIT
                        (bcmtm_pfc_deadlock_config_set(unit,
                                                       ltid,
                                                       fid,
                                                       lport,
                                                       pri,
                                                       val));
                }
            }
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in,
                          TM_PFC_DEADLOCK_RECOVERYt_OPERATIONAL_STATEf,
                          0, 0));
       }
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, FALSE, ltid, &in));
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Function to start dead lock monitor polling thread.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot mode.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmtm_pfc_deadlock_monitor_run(int unit, bool warm)
{
    bcmtm_pfc_ddr_ctrl_t *ddrc = bcmtm_pfc_ctrl[unit];
    bcmtm_pfc_ddr_info_t *pfcinfo;
    const bcmlrd_map_t *map = NULL;
    int lport = 0;
    int rv = 0;
    bcmtm_pport_t pport;
    bcmdrd_pbmp_t pbmp;


    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERYt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    SHR_NULL_CHECK(ddrc, SHR_E_INIT);
    bcmtm_pfc_ddr_info_get(unit, &pfcinfo);
    SHR_NULL_CHECK(pfcinfo, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_ports_get(unit, &pbmp));
    if (warm) {
        BCMDRD_PBMP_ITER(pbmp, pport) {
            if (SHR_SUCCESS(bcmtm_pport_lport_get(unit, pport, &lport))) {
                if (pfcinfo[lport].ddr_enable != 0) {
                    bcmtm_pfc_deadlock_monitor_enable(unit, true);
                    break;
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}
