/*! \file bcm56990_b0_tm_pfc.c
 *
 * TM PFC functions used by LT TM_PFC_PRI_TO_COS_MAP IMM handlers to program the HW.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/chip/bcm56990_b0_acc.h>

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcmtm/chip/bcm56990_b0_tm.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/chip/bcm56990_b0_tm.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_internal.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include "bcm56990_b0_tm_scheduler_shaper.h"
#include <bcmltd/bcmltd_bitop.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmtm/pfc/bcmtm_pfc_deadlock_internal.h>
#include "bcm56990_b0_tm_imm_internal.h"
#include "bcm56990_b0_tm_pfc.h"

/*******************************************************************************
 * Local definitions
 */

#define TH4_B0_PFC_PRIORITY_MAX_INDEX         MAX_NUM_PFC_PRI - 1
#define TH4_B0_PFC_INDEX_INVALID              -1
#define TH4_B0_PFC_RX_PROFILE_NUM_ENTRIES     8
#define TH4_B0_PFC_COS_NUM                    MAX_NUM_COS
#define TH4_B0_PFC_MAX_NUM_PORT               272

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP

/*!
 * \brief Writes the PFC priority to MMU Queue mapping to H/W.
 * \param [in] unit Unit number.
 * \param [in] profile_id PFC priority to cos map profile id.
 * \param [in] config_array PFC priority to cos map object
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error when writing config to H/W.
 */
static int
th4_b0_tm_pfc_rx_mapping_hw_write(int unit,
                               int profile_id,
                               tm_pfc_pri_to_cos_map_config_t config_array[])
{
    bcmltd_sid_t sid = TM_PFC_PRI_PROFILEt;
    int  pri, cos;
    int mmu_qs[2];
    uint32_t mmuq_bmp, reg_index;
    uint32_t rval[BCMTM_MAX_ENTRY_WSIZE] = {0}, cos_bmp[1] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmtm_scheduler_profile_t profile[MAX_TM_SCHEDULER_NODE];
    uint8_t status = 0;
    bcmdrd_sid_t  pri_ptid = MMU_INTFI_PFCPRI_PROFILEr;
    bcmdrd_fid_t mmuq_field = PFCPRI_MMUQ_BMPf;


    SHR_FUNC_ENTER(unit);

    sal_memset(profile, 0,
                (sizeof(bcmtm_scheduler_profile_t) * MAX_TM_SCHEDULER_NODE));
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_profile_get(unit, profile_id, profile));

    /* Profile validation. */
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_profile_validate(unit, -1,
                                     (uint8_t)profile_id, profile, &status));
    if (!status) {
        return SHR_E_NONE;
    }

    /* Write PFC-rx priority to MMUq(s) mapping profile. */
    for (pri = 0; pri < MAX_NUM_PFC_PRI; pri++) {
        mmuq_bmp = 0;
        cos_bmp[0] = config_array[pri].cos_list_bmp;

        reg_index = profile_id + pri * TH4_B0_PFC_RX_PROFILE_NUM_ENTRIES;
        SHR_BIT_ITER(cos_bmp, TH4_B0_PFC_COS_NUM, cos) {
            bcm56990_b0_tm_sched_node_mmu_q_get(unit,
                                                profile_id,
                                                profile, mmu_qs);
            if (mmu_qs[0] != -1) {
                mmuq_bmp |= 1U << mmu_qs[0];
            }
            if (mmu_qs[1] != -1) {
                mmuq_bmp |= 1U << mmu_qs[1];
            }
        }

        /* Read the current limit from HW. */
        BCMTM_PT_DYN_INFO(pt_dyn_info, reg_index, 0);
        sal_memset(rval, 0, sizeof(rval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, pri_ptid, sid, &pt_dyn_info, rval));

        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, pri_ptid, mmuq_field, rval, &mmuq_bmp));

        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, pri_ptid, sid, &pt_dyn_info, rval));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

/*!
 * \brief Writes the PFC priority profile to H/W.
 *
 * Writes the PFC priority to MMUQ mappings to H/W registers, memories.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id PFC priority profile index.
 * \param [in] map PFC priority profile.
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error in writing the profile.
 */
int
bcm56990_b0_tm_pfc_rx_pri_cos_map_set(int unit,
                                      int profile_id,
                                      tm_pfc_pri_to_cos_map_config_t  map[])
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (th4_b0_tm_pfc_rx_mapping_hw_write(unit, profile_id, map));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_tm_pfc_egr_port_set(int unit,
                                bcmlrd_sid_t sid,
                                bcmpc_lport_t lport,
                                uint32_t pfc_enable,
                                uint32_t profile_id)
{
    int mport = -1;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid = MMU_INTFI_EGR_PORT_CFGr;
    uint32_t ltmbuf;
    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(bcmtm_lport_mport_get(unit, lport, &mport))) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Read the HW register. */
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mport);
    ltmbuf = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, &ltmbuf));

    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, DISABLE_Q_FCf, &ltmbuf, &pfc_enable));

    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, PROFILE_SELf, &ltmbuf, &profile_id));

    /* Write to the HW register. */
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}
