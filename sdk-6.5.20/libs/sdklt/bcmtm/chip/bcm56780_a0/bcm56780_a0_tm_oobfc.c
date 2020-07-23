/*! \file bcm56780_a0_tm_oobfc.c
 *
 * TM OOBFC TD4-X9 specific functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmtm/chip/bcm56780_a0_tm.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include "bcm56780_a0_tm_oobfc.h"
#include <bcmtm/oobfc/bcmtm_oobfc_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief TD4-X9 oobfc port mapping
 *
 * Maps MMU chip port number to OOBFC port number for THDI.
 *
 * \param [in] unit Logical unit number.
 * \param [in] port  MMU chip port number.
 * \param [in] oobport OOBFC port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_oobfc_port_set_hw(int unit, int port, int oob_port)
{
    bcmdrd_sid_t sid = MMU_INTFO_MMU_PORT_TO_OOB_PORT_MAPPING_THDIm;
    bcmdrd_fid_t fid = OOB_PORT_NUMf;
    bcmtm_pt_info_t pt_info = {0};
    uint32_t ltmbuf = {0};

    SHR_FUNC_ENTER(unit);
    BCMTM_PT_DYN_INFO(pt_info, port, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, -1, &pt_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, (uint32_t *)&oob_port));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, -1, &pt_info, &ltmbuf));

    sid = MMU_INTFO_MMU_PORT_TO_OOB_PORT_MAPPING_THDO0m;
    BCMTM_PT_DYN_INFO(pt_info, port, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, -1, &pt_info, &ltmbuf));

    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, (uint32_t *)&oob_port));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, -1, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TD4-X9 queue oobfc profile map.
 *
 * Profile configuration to map the queue in the OOBFC message.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] profile_id OOBFC profile ID.
 * \param [in] q_id Queue ID.
 * \param [in] q_profile Queue profile.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_tm_oobfc_q_profile_set(int unit,
                                   bcmltd_sid_t ltid,
                                   uint8_t profile_id,
                                   uint8_t q_id,
                                   bcmtm_oobfc_q_profile_t *q_profile)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[2] = {0}, fval;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_fid_t oobfc_bit_offset[] = {
        MMUQ0_TO_OOB_BIT_OFFSETf, MMUQ1_TO_OOB_BIT_OFFSETf,
        MMUQ2_TO_OOB_BIT_OFFSETf, MMUQ3_TO_OOB_BIT_OFFSETf,
        MMUQ4_TO_OOB_BIT_OFFSETf, MMUQ5_TO_OOB_BIT_OFFSETf,
        MMUQ6_TO_OOB_BIT_OFFSETf, MMUQ7_TO_OOB_BIT_OFFSETf,
        MMUQ8_TO_OOB_BIT_OFFSETf, MMUQ9_TO_OOB_BIT_OFFSETf,
        MMUQ10_TO_OOB_BIT_OFFSETf, MMUQ11_TO_OOB_BIT_OFFSETf};
    bcmdrd_fid_t oobfc_cng_notify[] = {
        MMUQ0_TO_OOB_ENf, MMUQ1_TO_OOB_ENf, MMUQ2_TO_OOB_ENf,
        MMUQ3_TO_OOB_ENf, MMUQ4_TO_OOB_ENf, MMUQ5_TO_OOB_ENf,
        MMUQ6_TO_OOB_ENf, MMUQ7_TO_OOB_ENf, MMUQ8_TO_OOB_ENf,
        MMUQ9_TO_OOB_ENf, MMUQ10_TO_OOB_ENf, MMUQ11_TO_OOB_ENf};

    SHR_FUNC_ENTER(unit);

    /* Get ENG_PORT_WIDTH */
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    sid = MMU_INTFO_CONFIG0r;
    fid = ENG_PORT_WIDTHf;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, -1, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf, &fval));

    /* ENG_PORT_WIDTH = 0 represents 8 bit message. */
    if (fval == 0 &&  q_profile->oob_bit_offset >= 8) {
        q_profile->opcode = OOBFC_BIT_OFFSET_INVALID;
        SHR_EXIT();
    }

    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    BCMTM_PT_DYN_INFO(pt_info, profile_id, 0);
    sid = MMU_INTFO_OOBFC_ENG_Q_MAPr;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));

    /* congestion notification */
    fid = oobfc_cng_notify[q_id];
    fval = q_profile->cng_notify;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* oobfc bit offset */
    fid = oobfc_bit_offset[q_id];
    fval = q_profile->oob_bit_offset;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TD4-X9 multicast queue oobfc profile map.
 *
 * Profile configuration to map the queue in the OOBFC message.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] profile_id OOBFC profile ID.
 * \param [in] mcq_id Queue ID.
 * \param [in] mcq_profile Multicast queue profile.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_tm_oobfc_mcq_profile_set(int unit,
                                     bcmltd_sid_t ltid,
                                     uint8_t profile_id,
                                     uint8_t mcq_id,
                                     bcmtm_oobfc_q_profile_t *mcq_profile)
{
    int mcq_base;
    SHR_FUNC_ENTER(unit);

    mcq_base =  bcm56780_a0_tm_num_uc_q_non_cpu_port_get(unit);
    mcq_id += mcq_base;

    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_oobfc_q_profile_set(unit, ltid, profile_id,
                                           mcq_id, mcq_profile));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function set port properties for OOB.
 *
 * \param [in] unit Logical unit id.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] port_cfg OOBFC port configuration.
 *
 * \retval !SHR_E_NONE Error code.
 * \retval SHR_E_NONE No error.
 */
static int
bcm56780_a0_tm_oobfc_port_set(int unit,
                              bcmltd_sid_t ltid,
                              bcmtm_lport_t lport,
                              bcmtm_oobfc_port_cfg_t *port_cfg)
{
    bcmdrd_sid_t bcmtm_oob_port_ing_sid[] =
            { MMU_INTFO_OOBFC_ING_PORT_EN0_64r,
               MMU_INTFO_OOBFC_ING_PORT_EN1_64r,
               MMU_INTFO_OOBFC_ING_PORT_EN2r };
    bcmdrd_sid_t bcmtm_oob_port_egr_sid[] =
             { MMU_INTFO_OOBFC_ENG_PORT_EN0_64r,
               MMU_INTFO_OOBFC_ENG_PORT_EN1_64r,
               MMU_INTFO_OOBFC_ENG_PORT_EN2r };
    bcmdrd_sid_t bcmtm_oob_port_cng_sid[] =
             { MMU_INTFO_OOBFC_CONGST_ST_EN0_64r,
               MMU_INTFO_OOBFC_CONGST_ST_EN1_64r,
               MMU_INTFO_OOBFC_CONGST_ST_EN2r };
    int chip_port, idx, offset;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf_arr[2] = {0}, ltmbuf = 0, fval;
    bcmtm_pt_info_t pt_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_lport_mchip_port_get(unit, lport, &chip_port));

    idx = port_cfg->oob_port/64;
    offset = port_cfg->oob_port % 64;

    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    /* Ingress */
    sid = bcmtm_oob_port_ing_sid[idx];
    fid = ING_PORT_ENf;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf_arr));

    if (port_cfg->ingress) {
        SHR_BITSET(ltmbuf_arr, offset);
    } else {
        SHR_BITCLR(ltmbuf_arr, offset);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf_arr));

    /* Egress */
    sid = bcmtm_oob_port_egr_sid[idx];
    fid = ENG_PORT_ENf;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf_arr));

    if (port_cfg->egress) {
        SHR_BITSET(ltmbuf_arr, offset);
    } else {
        SHR_BITCLR(ltmbuf_arr, offset);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf_arr));

    /* Congestion */
    sid = bcmtm_oob_port_cng_sid[idx];
    fid = CONGST_ST_ENf;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf_arr));

    if (port_cfg->cng_report) {
        SHR_BITSET(ltmbuf_arr, offset);
    } else {
        SHR_BITCLR(ltmbuf_arr, offset);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf_arr));

    /* OOB port */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_oobfc_port_set_hw(unit, chip_port, port_cfg->oob_port));

    /* Profile ID */
    sid = MMU_INTFO_OOBFC_ENG_PORT_PSELm;
    fid = ENG_PORT_PROFILE_SELf;
    BCMTM_PT_DYN_INFO(pt_info, chip_port, 0);
    fval = port_cfg->q_map_profile_id;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Maps user configuration to egress service pool in the PT table.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table id.
 * \param [in] spid Service pool ID..
 * \param [in] oobfc_sp_cfg OOBFC egress service pool configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_tm_oobfc_egr_sp_set (int unit,
                                 bcmltd_sid_t ltid,
                                 uint8_t spid,
                                 bcmtm_oobfc_sp_cfg_t *oobfc_sp_cfg)
{
    bcmdrd_sid_t sid = MMU_THDO_INTFO_INTERFACE_CONFIGr;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_info = {0};
    uint32_t ltmbuf = 0;
    uint32_t fval = {0};

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_info, 0, 0);

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));

    /* POOL_COUPLING_UCf */
    fid = POOL_COUPLING_UCf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
    if (oobfc_sp_cfg->ucq_merge == 0) {
        SHR_BITCLR(&fval, spid);
    } else if (oobfc_sp_cfg->ucq_merge == 1) {
        SHR_BITSET(&fval, spid);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));

    /* POOL_COUPLING_MCf */
    fid = POOL_COUPLING_MCf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
    if (oobfc_sp_cfg->mcq_merge == 0) {
        SHR_BITCLR(&fval, spid);
    } else if (oobfc_sp_cfg->mcq_merge == 1) {
        SHR_BITSET(&fval, spid);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TD4-X9 OOBFC related function pointers.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNIT Unit information not found.
 */
/*******************************************************************************
 * Public functions
 */
int
bcm56780_a0_tm_oobfc_drv_get(int unit, void *oobfc_drv)
{

    bcmtm_oobfc_drv_t bcm56780_a0_oobfc_drv = {
        .ucq_profile_set = bcm56780_a0_tm_oobfc_q_profile_set,
        .mcq_profile_set = bcm56780_a0_tm_oobfc_mcq_profile_set,
        .port_set = bcm56780_a0_tm_oobfc_port_set,
        .egr_sp_set = bcm56780_a0_tm_oobfc_egr_sp_set
    };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(oobfc_drv, SHR_E_INTERNAL);
    *((bcmtm_oobfc_drv_t *)oobfc_drv) = bcm56780_a0_oobfc_drv;
exit:
    SHR_FUNC_EXIT();
}
