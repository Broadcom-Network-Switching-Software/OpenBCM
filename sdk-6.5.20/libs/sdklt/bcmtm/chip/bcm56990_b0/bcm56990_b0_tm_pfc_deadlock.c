/*! \file bcm56990_b0_tm_pfc_deadlock.c
 *
 * PFC deadlock recovery handler chip specific implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>
#include <shr/shr_timeout.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmbd/chip/bcm56990_b0_acc.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/chip/bcm56990_b0_tm.h>
#include <bcmltd/bcmltd_bitop.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmtm/bcmtm_ha_internal.h>
#include <bcmtm/pfc/bcmtm_pfc_deadlock_internal.h>
#include "bcm56990_b0_tm_pfc.h"
#include "bcm56990_b0_tm_imm_internal.h"

/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief PFC deadlock recovery action start function.
 *
 * The function is used to begin recovery when a (Port, PFC priority) is
 * declared to be in Deadlock by hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] lport device port number.
 * \param [in] pri PFC priority.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm56990_b0_pfc_deadlock_recovery_begin(int unit, int lport, int pri)
{
    int mport = -1;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t sid = TM_PFC_DEADLOCK_RECOVERYt;
    uint32_t rval = 0, fval = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_lport_mport_get(unit, lport, &mport));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);

    /* 1: Mask the Intr by setting 1.  */
    rval = 0;
    fval = 0;

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, MMU_INTFI_DD_TIMER_INT_MASKr, sid,
                               &pt_dyn_info, &rval));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, MMU_INTFI_DD_TIMER_INT_MASKr,
                         PFC_PRIORITY_INT_MASKf, &rval, &fval));

    fval |= 1U << pri;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, MMU_INTFI_DD_TIMER_INT_MASKr,
                         PFC_PRIORITY_INT_MASKf, &rval, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, MMU_INTFI_DD_TIMER_INT_MASKr,
                                sid, &pt_dyn_info, &rval));
    /* 2: Disable the timer. */
    rval = 0;
    fval = 0;

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, MMU_INTFI_DD_TIMER_ENABLEr, sid,
                               &pt_dyn_info, &rval));

    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, MMU_INTFI_DD_TIMER_ENABLEr, ENABLEf,
                         &rval, &fval));

    fval &= ~(1U << pri);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, MMU_INTFI_DD_TIMER_ENABLEr, ENABLEf, &rval,
                             &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, MMU_INTFI_DD_TIMER_ENABLEr, sid,
                                &pt_dyn_info, &rval));

   /* 3: For that port, set ignore_pfc_xoff = 1 (per port reg) */

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, MMU_INTFI_IGNORE_PORT_PFC_XOFFr, sid,
                               &pt_dyn_info, &rval));

    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, MMU_INTFI_IGNORE_PORT_PFC_XOFFr, PRI_IGNORE_XOFFf,
                         &rval, &fval));
    fval |= 1U << pri;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, MMU_INTFI_IGNORE_PORT_PFC_XOFFr,
                         PRI_IGNORE_XOFFf, &rval, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, MMU_INTFI_IGNORE_PORT_PFC_XOFFr, sid,
                                &pt_dyn_info, &rval));
exit:
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
int
bcm56990_b0_pfc_deadlock_recovery_end(int unit, int lport, int pri)
{
    int mport = -1;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t sid = TM_PFC_DEADLOCK_RECOVERYt;
    uint32_t rval = 0, fval = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_lport_mport_get(unit, lport, &mport));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);

   /* 1: For that port, set ignore_pfc_xoff = 0. */

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, MMU_INTFI_IGNORE_PORT_PFC_XOFFr, sid,
                               &pt_dyn_info, &rval));

    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, MMU_INTFI_IGNORE_PORT_PFC_XOFFr,
                         PRI_IGNORE_XOFFf, &rval, &fval));

    fval &= ~(1U << pri);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, MMU_INTFI_IGNORE_PORT_PFC_XOFFr,
                         PRI_IGNORE_XOFFf, &rval,
                         &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, MMU_INTFI_IGNORE_PORT_PFC_XOFFr,
                                sid, &pt_dyn_info, &rval));

    /* 2: Unmask the interrupt by setting 0. */
    rval = 0;
    fval = 0;

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, MMU_INTFI_DD_TIMER_INT_MASKr,
                               sid, &pt_dyn_info, &rval));

    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, MMU_INTFI_DD_TIMER_INT_MASKr,
                         PFC_PRIORITY_INT_MASKf, &rval, &fval));

    fval &= ~(1U << pri);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, MMU_INTFI_DD_TIMER_INT_MASKr,
                         PFC_PRIORITY_INT_MASKf, &rval, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, MMU_INTFI_DD_TIMER_INT_MASKr, sid,
                                &pt_dyn_info, &rval));

    /* 3: clear interrupt status, set 1 to clear. */
    rval = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, MMU_INTFI_DD_TIMER_INT_STATUSr, sid,
                               &pt_dyn_info, &rval));
    rval &= (1U << pri);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, MMU_INTFI_DD_TIMER_INT_STATUSr, sid,
                                &pt_dyn_info, &rval));

    /* 4 Turn timer back on. */
    rval = 0;
    fval = 0;

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, MMU_INTFI_DD_TIMER_ENABLEr, sid,
                               &pt_dyn_info, &rval));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, MMU_INTFI_DD_TIMER_ENABLEr, ENABLEf,
                         &rval, &fval));
    fval |= 1U << pri;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, MMU_INTFI_DD_TIMER_ENABLEr, ENABLEf, &rval,
                             &fval));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, MMU_INTFI_DD_TIMER_ENABLEr, sid,
                                &pt_dyn_info, &rval));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_pfc_deadlock_hw_status_get(int unit, int32_t lport,
                                       uint32_t *mask, uint32_t *status)
{
    int mport = -1;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmltd_sid_t sid = TM_PFC_DEADLOCK_RECOVERYt;
    uint32_t rval_status, rval_mask;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_lport_mport_get(unit, lport, &mport));

    /* Read the register from HW. */
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);

    *status = 0;
    rval_status = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, MMU_INTFI_DD_TIMER_INT_STATUSr, sid,
                               &pt_dyn_info, &rval_status));

    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, MMU_INTFI_DD_TIMER_INT_STATUSr,
                         PFC_PRIORITY_ISRf, &rval_status, status));

    /* Read the interrupt mask register, if the interrupt is not masked,
     * means the recovery is not started and we are seeing the interrupt
     * for the first time.
     */

    rval_mask = 0;
    *mask = 0;

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, MMU_INTFI_DD_TIMER_INT_MASKr, sid,
                               &pt_dyn_info, &rval_mask));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, MMU_INTFI_DD_TIMER_INT_MASKr,
                         PFC_PRIORITY_INT_MASKf, &rval_mask, mask));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_pfc_deadlock_config_hw_set(int unit,
                                       uint32_t fid,
                                       uint32_t lport,
                                       uint32_t pri,
                                       uint32_t val)
{
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t rvalbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    uint32_t rval = 0;
    int mport = -1;
    const bcmdrd_fid_t timer_init_fields[] = {
                                    INIT_VALUE_PFC0f, INIT_VALUE_PFC1f,
                                    INIT_VALUE_PFC2f, INIT_VALUE_PFC3f,
                                    INIT_VALUE_PFC4f, INIT_VALUE_PFC5f,
                                    INIT_VALUE_PFC6f, INIT_VALUE_PFC7f};
    const bcmdrd_fid_t timer_tick_fields[] = {
                                    TICK_UNIT_PFC0f, TICK_UNIT_PFC1f,
                                    TICK_UNIT_PFC2f, TICK_UNIT_PFC3f,
                                    TICK_UNIT_PFC4f, TICK_UNIT_PFC5f,
                                    TICK_UNIT_PFC6f, TICK_UNIT_PFC7f,
                                   };
    bcmlrd_sid_t sid = TM_PFC_DEADLOCK_RECOVERYt;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_lport_mport_get(unit, lport, &mport));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport)

    if (fid == TM_PFC_DEADLOCK_RECOVERYt_DEADLOCK_RECOVERYf) {
        rval = 0;
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, MMU_INTFI_DD_TIMER_ENABLEr, sid,
                                   &pt_dyn_info, &rval));
        if (val == 0)  {
            rval &= ~(1U << pri);
        } else {
            rval |= (1U << pri);
        }
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, MMU_INTFI_DD_TIMER_ENABLEr, sid,
                                    &pt_dyn_info, &rval));
    } else if (fid == TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMER_GRANULARITYf) {
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, MMU_INTFI_DD_TIMER_CFGr, sid,
                                   &pt_dyn_info, rvalbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, MMU_INTFI_DD_TIMER_CFGr,
                             timer_tick_fields[pri], rvalbuf, &val));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, MMU_INTFI_DD_TIMER_CFGr, sid,
                                &pt_dyn_info, rvalbuf));
    } else if (fid == TM_PFC_DEADLOCK_RECOVERYt_DETECTION_TIMERf) {
        SHR_IF_ERR_EXIT
           (bcmtm_pt_indexed_read(unit, MMU_INTFI_DD_TIMER_CFGr, sid,
                               &pt_dyn_info, rvalbuf));
        SHR_IF_ERR_EXIT
           (bcmtm_field_set(unit, MMU_INTFI_DD_TIMER_CFGr,
                            timer_init_fields[pri], rvalbuf, &val));
        SHR_IF_ERR_EXIT
           (bcmtm_pt_indexed_write(unit, MMU_INTFI_DD_TIMER_CFGr, sid,
                                   &pt_dyn_info, rvalbuf));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To populate the TM_DEADLOCK_RECOVERY_STATUS LT.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXX  LT populate failed.
 */
int
bcm56990_b0_tm_pfc_deadlock_recovery_status_populate(int unit)
{

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmtm_pfc_deadlock_recovery_status_imm_populate(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_tm_pfc_deadlock_recovery_status_update(int unit,
                                                   int pport,
                                                   int up)
{
    int lport, rv, pri = 0;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERY_STATUSt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmtm_pport_lport_get(unit, pport, &lport));

    if (bcmtm_lport_is_fp(unit, lport) || bcmtm_lport_is_mgmt(unit, lport)) {
        for (pri = 0; pri < MAX_NUM_PFC_PRI; pri++) {
            SHR_IF_ERR_EXIT
                (bcmtm_pfc_deadlock_recovery_status_populate_imm_entry(unit,
                                                             (bcmtm_lport_t )lport,
                                                             pri,
                                                             FALSE, FALSE, up));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_tm_pfc_deadlock_recovery_update(int unit,
                                            int pport, int up)
{
    int lport, rv, pri = 0;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_PFC_DEADLOCK_RECOVERYt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmtm_pport_lport_get(unit, pport, &lport));

    for (pri = 0; pri < MAX_NUM_PFC_PRI; pri++) {
        SHR_IF_ERR_EXIT
            (bcmtm_pfc_deadlock_recovery_populate_imm_entry(unit,
                                                            (bcmtm_lport_t )lport,
                                                            pri,
                                                            up));
    }
exit:
    SHR_FUNC_EXIT();
}
