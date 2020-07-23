/*! \file bcm56996_a0_tm_scheduler_shaper_init.c
 *
 * File containing scheduler shaper related functions for
 * bcm56996_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_alloc.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/chip/bcm56996_a0_enum.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56996_a0_tm.h>
#include <bcmtm/sched_shaper/bcmtm_shaper_internal.h>
#include <bcmcfg/bcmcfg_lt.h>
#include "bcm56996_a0_tm_scheduler_shaper.h"


#define BSL_LOG_MODULE  BSL_LS_BCMTM_INIT
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Shaper configuration.
 *
 * \param [in]  unit   Unit number.
 *
 * \retval SHR_E_NONE  No error.
 */
static int
bcm56996_a0_tm_shaper_config_init(int unit)
{
    static bcmdrd_sid_t sid = MMU_MTRO_CONFIGr;
    bcmltd_sid_t ltid = TM_SHAPER_CONFIGt;
    uint64_t field_value = 0;
    uint32_t fval = 0, ltmbuf = 0;
    bcmtm_pt_info_t pt_dyn_info = {0};
    int rv;

    SHR_FUNC_ENTER(unit);

    /* ITU settings */
    rv = bcmcfg_field_get(unit, ltid,
                          TM_SHAPER_CONFIGt_ITU_MODEf,
                          &field_value);
    if (rv == SHR_E_UNAVAIL) {
        SHR_EXIT();
    }

    fval = (uint32_t) field_value;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, ITU_MODE_SELf, &ltmbuf, &fval));

    /* Metering enable/disable */
    rv = bcmcfg_field_get(unit, ltid,
                          TM_SHAPER_CONFIGt_SHAPERf,
                          &field_value);
    if (rv == SHR_E_UNAVAIL) {
        SHR_EXIT();
    }

    fval = (uint32_t) field_value;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, REFRESH_DISABLEf, &ltmbuf, &fval));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, (void *)&pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Scheduler init configuration.
 *
 * \param [in]  unit   Unit number.
 *
 * \retval SHR_E_NONE  No error.
 */
static int
bcm56996_a0_tm_scheduler_config_init(int unit)
{
    static bcmdrd_sid_t sid[] = { MMU_TOQ_CONFIGr,
                                  MMU_TOQ_CQEB_CONFIGr,
                                  MMU_THDO_CONFIGr,
                                  MMU_CRB_CONFIGr,
                                  MMU_EBPCC_MMUQ_CFGr};
    static bcmdrd_fid_t fid = MC_Q_MODEf;
    bcmltd_sid_t ltid = TM_SCHEDULER_CONFIGt;
    bcmltd_fid_t ltfid = TM_SCHEDULER_CONFIGt_NUM_MC_Qf;
    uint32_t fval = 0, idx = 0;
    uint64_t mc_q_mode;
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE];
    int rv, i, tbl_inst_num;
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);

    /* TM_SCHEDULER_CONFIG settings */
    rv = bcmcfg_field_get(unit, ltid, ltfid, &mc_q_mode);
    if (rv == SHR_E_UNAVAIL) {
        SHR_EXIT();
    }

    /* TM_SCHEDULER_CONFIGt_NUM_MC_Qf  */
    fval = (uint32_t)mc_q_mode;
    for (idx = 0; idx < sizeof(sid)/sizeof(bcmdrd_sid_t); idx++) {
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));
        tbl_inst_num = bcmdrd_pt_num_tbl_inst(unit, sid[idx]);
        for (i = 0; i < tbl_inst_num; i++) {
            if (SHR_SUCCESS(bcm56996_a0_tm_check_valid_itm(unit, i))) {
                BCMTM_PT_DYN_INFO(pt_dyn_info, 0, i);
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, sid[idx], ltid,
                                           &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, sid[idx], fid, ltmbuf, &fval));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, sid[idx], ltid,
                                            &pt_dyn_info, ltmbuf));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcm56996_a0_tm_scheduler_shaper_chip_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    if (!warm) {
        /* Scheduler config. */
        SHR_IF_ERR_EXIT
            (bcm56996_a0_tm_scheduler_config_init(unit));

        /* Shaper config */
        SHR_IF_ERR_EXIT
            (bcm56996_a0_tm_shaper_config_init(unit));
    }
    SHR_IF_ERR_EXIT
        (bcm56996_a0_tm_scheduler_profile_init(unit, -1, NULL));
exit:
    SHR_FUNC_EXIT();
}
