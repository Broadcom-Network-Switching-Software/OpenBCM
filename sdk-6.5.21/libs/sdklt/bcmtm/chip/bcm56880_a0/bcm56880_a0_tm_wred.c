/*! \file bcm56880_a0_tm_wred.c
 *
 * File containing wred related functions for
 * bcm56880_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include "bcm56880_a0_tm_wred.h"
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_drv.h>

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief TD4 wred unicast queue related physical table configurations.
 *
 * \param [in] unit Unit Number.
 * \param [in] lport Logical port ID.
 * \param [in] ucq_id Unicast queue ID.
 * \param [in] ltid Logical table ID.
 * \param [in] wred_ucq_cfg WRED unicast queue configurations.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Unicast queue ID is greater than number of unicast queue.
 * hardware table not found.
 */
static int
bcm56880_a0_tm_wred_ucq_set(int unit,
                            bcmtm_lport_t lport,
                            uint32_t ucq_id,
                            bcmltd_sid_t ltid,
                            bcmtm_wred_cfg_t *wred_ucq_cfg)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf = 0, fval;
    int ucq_base;
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_lport_ucq_base_get(unit, lport, &ucq_base));
    ucq_id += ucq_base;

    /* MMU_WRED_QUEUE_CONFIG */
    sid = MMU_WRED_QUEUE_CONFIGm;

    fid = WEIGHTf;
    fval = wred_ucq_cfg->weight;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    fid = CAP_AVERAGEf;
    fval = wred_ucq_cfg->curr_q_size;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    fid = WRED_ENf;
    fval = wred_ucq_cfg->wred;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    fid = TIME_DOMAIN_SELf;
    fval = wred_ucq_cfg->time_profile_id;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    fid = PROFILE_INDEXf;
    fval = wred_ucq_cfg->wcs_profile_id;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    fid = ECN_MARKING_ENf;
    fval = wred_ucq_cfg->ecn;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    fid = WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_MARKING_INDEXf;
    fval = wred_ucq_cfg->mark_cng_profile_id;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    fid = WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_DROPPING_INDEXf;
    fval = wred_ucq_cfg->drop_cng_profile_id;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    /* write to hardware */
    BCMTM_PT_DYN_INFO(pt_dyn_info, ucq_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TD4 wred port service pool related physical table configurations.
 *
 * \param [in] unit Unit Number.
 * \param [in] lport Logical port ID.
 * \param [in] portsp_id Port service pool ID.
 * \param [in] ltid Logical table ID.
 * \param [in] wred_portsp_cfg WRED port service pool configurations.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Hardware table not found.
 */
static int
bcm56880_a0_tm_wred_portsp_set(int unit,
                               bcmtm_lport_t lport,
                               uint32_t portsp_id,
                               bcmltd_sid_t ltid,
                               bcmtm_wred_cfg_t *wred_portsp_cfg)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf = 0, fval;
    int chip_portnum;
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_lport_mchip_port_get(unit, lport, &chip_portnum));
    portsp_id += (chip_portnum * 4);

    /* MMU_WRED_PORTSP_CONFIG */
    sid = MMU_WRED_PORTSP_CONFIGm;

    fid = WEIGHTf;
    fval = wred_portsp_cfg->weight;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    fid = CAP_AVERAGEf;
    fval = wred_portsp_cfg->curr_q_size;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    fid = WRED_ENf;
    fval = wred_portsp_cfg->wred;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    fid = TIME_DOMAIN_SELf;
    fval = wred_portsp_cfg->time_profile_id;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    fid = PROFILE_INDEXf;
    fval = wred_portsp_cfg->wcs_profile_id;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    /* Write to hardware */
    BCMTM_PT_DYN_INFO(pt_dyn_info, portsp_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TD4 wred congestion resolution related physical table configurations.
 *
 * \param [in] unit Unit Number.
 * \param [in] lport Logical port ID.
 * \param [in/out] cng_profile Congestion profile.
 * \param [in] set Set/Get profile to/from hardware.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Hardware table not found.
 */
static int
bcm56880_a0_tm_wred_cng_profile_set(int unit,
                                    bcmltd_sid_t ltid,
                                    bcmtm_wred_cng_profile_t *cng_profile)
{
    uint32_t idx, ltmbuf = 0, fval[1] = {0};
    bcmdrd_sid_t sid = MMU_WRED_CONG_NOTIFICATION_RESOLUTION_TABLEr;
    bcmdrd_fid_t fid = MMU_CONGESTION_EXPERIENCEf;
    uint32_t profile_id, q_min, q_avg, sp, action;
    bcmtm_pt_info_t pt_dyn_info = {0};

    SHR_FUNC_ENTER(unit);
    profile_id = cng_profile->profile_id;
    sp = cng_profile->sp;
    q_min = cng_profile->q_min;
    q_avg = cng_profile->q_avg;
    action = cng_profile->action;

    idx = (sp << TD4_WRED_CNG_SERVICE_POOL_OFFSET)|(q_min << TD4_WRED_CNG_Q_MIN_OFFSET)|q_avg;

    BCMTM_PT_DYN_INFO(pt_dyn_info, profile_id, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, &ltmbuf, fval));

    if (action != BCMTM_WRED_CNG_ACTION_DEFAULT ) {
        action ? (SHR_BITSET(fval, idx)): (SHR_BITCLR(fval, idx));
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, &ltmbuf, fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_dyn_info, &ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TD4 WRED congestion profile init.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNIT Unit information not found.
 */
static int
bcm56880_a0_tm_wred_cng_profile_init(int unit)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;
    size_t num_fid;
    int idx, profile_id;
    bcmltd_sid_t ltid = TM_WRED_CNG_NOTIFICATION_PROFILEt;
    bcmtm_wred_cng_profile_t cng_profile;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fid));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &in));

    for (profile_id = 0; profile_id < TD4_WRED_CNG_MAX_PROFILE; profile_id++) {
        cng_profile.profile_id = profile_id;
        for (idx = 0; idx < TD4_WRED_CNG_ENTRY_PER_PROFILE; idx++) {
            cng_profile.q_avg = idx & 0x1;
            cng_profile.q_min =
                 (idx & (1 << TD4_WRED_CNG_Q_MIN_OFFSET)) >> TD4_WRED_CNG_Q_MIN_OFFSET;
            cng_profile.sp = idx >> TD4_WRED_CNG_SERVICE_POOL_OFFSET;

            if (cng_profile.q_avg) {
                /* Default value for action is 1 when Q_AVG is enabled. */
                cng_profile.action = 1;
            } else {
                cng_profile.action = 0;
            }
            in.count = 0;

            fid = TM_WRED_CNG_NOTIFICATION_PROFILEt_TM_WRED_CNG_NOTIFICATION_PROFILE_IDf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in, fid, 0, cng_profile.profile_id));

            fid = TM_WRED_CNG_NOTIFICATION_PROFILEt_Q_AVGf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in, fid, 0, cng_profile.q_avg));

            fid = TM_WRED_CNG_NOTIFICATION_PROFILEt_Q_MINf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in, fid, 0, cng_profile.q_min));

            fid = TM_WRED_CNG_NOTIFICATION_PROFILEt_SERVICE_POOLf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in, fid, 0, cng_profile.sp));

            fid = TM_WRED_CNG_NOTIFICATION_PROFILEt_ACTIONf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in, fid, 0, cng_profile.action));

            SHR_IF_ERR_EXIT
                (bcmimm_entry_insert(unit, ltid, &in));

            SHR_IF_ERR_EXIT
                (bcm56880_a0_tm_wred_cng_profile_set(unit, ltid, &cng_profile));
        }
    }
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcm56880_a0_tm_wred_enable(int unit)
{
    bcmdrd_sid_t sid = MMU_WRED_CONFIG_READYr;
    bcmdrd_fid_t fid = WRED_READYf;
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t ltmbuf[2] = {0}, fval;

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, -1, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf, &fval));
    if (fval) {
        /* already enabled */
        SHR_EXIT();
    }
    fval = 1; /* enable */
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, -1, &pt_dyn_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_wred_drv_get(int unit, void *wred_drv)
{

    bcmtm_wred_drv_t bcm56880_a0_wred_drv = {
        .ucq_set = bcm56880_a0_tm_wred_ucq_set,
        .portsp_set = bcm56880_a0_tm_wred_portsp_set,
        .cng_profile_set = bcm56880_a0_tm_wred_cng_profile_set
    };

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(wred_drv, SHR_E_INTERNAL);

    *((bcmtm_wred_drv_t *)wred_drv) = bcm56880_a0_wred_drv;
exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_wred_chip_init(int unit, bool warm)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_WRED_CNG_NOTIFICATION_PROFILEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* prepopulate default values for wred tables. */
    if (!warm) {
        SHR_IF_ERR_EXIT(bcm56880_a0_tm_wred_cng_profile_init(unit));
    }
exit:
    SHR_FUNC_EXIT();
}
