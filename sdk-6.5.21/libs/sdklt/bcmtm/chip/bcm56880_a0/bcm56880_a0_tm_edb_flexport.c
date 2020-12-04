/*! \file bcm56880_a0_tm_edb_flexport.c
 *
 * File containing flexport sequence for bcm56880_a0.
 *
 * The code in this file is shared with DV team. This file will only be updated
 * by porting DV code changes. If any extra change needs to be made to the
 * shared code, please also inform the DV team.
 *
 * DV file: $DV/trident4/dv/ngsdk_interface/flexport/edb_flexport.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmtm/chip/bcm56880_a0_tm.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include "bcm56880_a0_tm_edb_flexport.h"
#include "bcm56880_a0_tm_e2e_settings.h"

#define BSL_LOG_MODULE  BSL_LS_BCMTM_FLEXPORT
/*******************************************************************************
 * START SDKLT-DV SHARED CODE
 */


/***************************************************************************
 * LOCAL WRAPPER FUNCTIONS
 */
static int
td4_edb_pt_write(int unit, int pt_id, int pt_inst, bcmdrd_sid_t mem,
                 bcmdrd_fid_t fid, uint32_t fval)
{
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t     ltmbuf[2] = {0};

    SHR_FUNC_ENTER(unit);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "edb_flexport_api\n")));
    BCMTM_PT_DYN_INFO(pt_info, pt_id, pt_inst);
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, mem, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit, mem, ltid, &pt_info, ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

static int
td4_edb_pt_read(int unit, int pt_id, int pt_inst, bcmdrd_sid_t mem,
                bcmdrd_fid_t fid, uint32_t *fval)
{
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[2] = {0};
    uint32_t fval_tmp;

    SHR_FUNC_ENTER(unit);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "edb_flexport_api\n")));
    BCMTM_PT_DYN_INFO(pt_info, pt_id, pt_inst);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit, mem, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT(bcmtm_field_get(unit, mem, fid, ltmbuf, &fval_tmp));
    *fval = fval_tmp;

exit:
    SHR_FUNC_EXIT();
}



/***************************************************************************
 * LOCAL FUNCTIONS
 */
/*!
 * \brief Get index of table td4_edb_e2e_tbl by the given port's speed.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport: Logical port.
 *
 * \retval Pre-configured speed encoding value.
 */
static int
td4_edb_e2e_tbl_speed_encoding_get(int unit, int lport)
{
    int idx, speed;
    int spd_encoding = 0;
    bcmtm_port_map_info_t *port_map;

    bcmtm_port_map_get(unit, &port_map);
    speed = port_map->lport_map[lport].max_speed;
    for (idx = 0; idx < TD4_EDB_NUM_CT_CLASSES; idx++) {
        if (td4_edb_e2e_tbl[idx].speed == speed) {
            spd_encoding = idx;
            break;
        }
    }

    return spd_encoding;
}

/*!
 * \brief Get index of xmit_cnt[] in table td4_edb_e2e_tbl.
 *
 * \param [in] unit: Device unit.
 *
 * \retval frequency encoding value.
 */
static int
td4_edb_e2e_tbl_opt_encoding_get(int unit)
{
    bcmtm_drv_info_t *drv_info;
    int core_freq, dpp_freq;
    int opt_encoding = 0;

    bcmtm_drv_info_get(unit, &drv_info);
    core_freq = drv_info->frequency;
    dpp_freq  = drv_info->dpr_clock_frequency;

    if ((core_freq == 1350 && dpp_freq == 1350) ||
        (core_freq == 1500 && dpp_freq == 1500)) {
        opt_encoding = 1;
    }

    return (opt_encoding % TD4_EDB_E2E_NUM_PFC_OPT_CFG);
}

/*!
 * \brief Get CT class of the given port in td4_edb_e2e_tbl.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport: Logical port.
 *
 * \retval Pre-configured CT class.
 */
static int
td4_edb_e2e_tbl_ct_class_get(int unit, int lport)
{
    int spd_encoding, ct_class;

    spd_encoding = td4_edb_e2e_tbl_speed_encoding_get(unit, lport);
    ct_class = td4_edb_e2e_tbl[spd_encoding].ct_class;

    return ct_class;
}

/*!
 * \brief Get ep2mmu credit of the given port from td4_edb_e2e_tbl.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport: Logical port.
 *
 * \retval Pre-configured port's ep2mmu credit.
 */
static int
td4_edb_e2e_tbl_mmu_credit_get(int unit, int lport)
{
    int spd_encoding, opt_encoding;
    int credit = 0;

    spd_encoding = td4_edb_e2e_tbl_speed_encoding_get(unit, lport);
    opt_encoding = td4_edb_e2e_tbl_opt_encoding_get(unit);

    if (opt_encoding == 0) {
        /* General cases */
        credit = td4_edb_e2e_tbl[spd_encoding].mmu_credit;
    } else {
        /* PFC Optimized cases for 1350MHz or 1500MHz */
        credit = td4_edb_e2e_tbl[spd_encoding].mmu_credit_pfc_opt;
    }

    return credit;
}

/*!
 * \brief Get SAF or CT xmit_cnt from td4_edb_e2e_tbl.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport: Logical port.
 * \param [in] ct_valid: ct valid. 0->saf xmit_cnt, otherwise->ct xmit_cnt.
 *
 * \retval Pre-configured xmit_cnt value.
 */
static int
td4_edb_e2e_tbl_xmit_cnt_get(int unit, int lport, int ct_valid)
{
    int spd_encoding, opt_encoding;
    int xmit_cnt = 0;

    /* FP */
    if (bcmtm_lport_is_fp(unit, lport)) {
        opt_encoding = td4_edb_e2e_tbl_opt_encoding_get(unit);
        spd_encoding = td4_edb_e2e_tbl_speed_encoding_get(unit, lport);

        if (ct_valid == 0) {
            xmit_cnt = td4_edb_e2e_tbl[spd_encoding].xmit_cnt[opt_encoding].saf;
        } else {
            xmit_cnt = td4_edb_e2e_tbl[spd_encoding].xmit_cnt[opt_encoding].ct;
        }
    } else if (bcmtm_lport_is_mgmt(unit, lport)) {
        /* MGM: 10G SAF */
        /* opt_encoding  = 0 */
        opt_encoding = td4_edb_e2e_tbl_opt_encoding_get(unit);
        spd_encoding = 1; /* 10G speed encoding in table td4_edb_e2e_tbl */
        xmit_cnt = TD4_EDB_XMIT_START_CNT_MGMT;
    } else if (bcmtm_lport_is_lb(unit, lport) ||
             bcmtm_lport_is_cpu(unit, lport)) {
        /* LPBK and CPU: low threshold, always SAF */
        xmit_cnt = 1;
    }

    return xmit_cnt;
}

/*!
 * \brief Drain EDB port buffer by polling num_entries to 0.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
td4_edb_drain_port(int unit, int lport)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t reg, mem;
    bcmdrd_fid_t fid;
    uint32_t fval_saved;
    uint32_t fval = 0;
    int pport, pipe_num;
    int pt_id, pt_inst;
    int timeout_us = 0;
    uint32_t num_entries;
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    pport    = port_map->lport_map[lport].pport;
    pipe_num = port_map->lport_map[lport].pipe;

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "edb_flexport_api: lport=%3d, pport=%3d\n"),
               lport, pport));

    /*
     * Configure EGR_EDB_MISC_CTRL such that EGR_MAX_USED_ENTRIES will return
     * the current buffer utilization in terms of number of used_entries.
     */
    pt_id   = 0;
    pt_inst = pipe_num;
    reg     = EDB_MISC_CTRLr;
    fid     = SELECT_CURRENT_USED_ENTRIESf;
    fval    = 1;

    SHR_IF_ERR_EXIT(td4_edb_pt_read(unit, pt_id, pt_inst, reg, fid, &fval_saved));
    SHR_IF_ERR_EXIT(td4_edb_pt_write(unit, pt_id, pt_inst, reg, fid, fval));

    /* Poll port's buffer utilization until is 0. */
    pt_id   = pport;
    pt_inst = 0;
    mem     = EGR_MAX_USED_ENTRIESm;
    fid     = LEVELf;

    do {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "edb_flexport_api: lport=%3d, pport=%3d, timeout_us=%0d\n"),
                   lport, pport, timeout_us));
        /*  Check number of used entries of port buffer */
        SHR_IF_ERR_EXIT
            (td4_edb_pt_read(unit, pt_id, pt_inst, mem, fid, &num_entries));
        if (num_entries == 0) break;

        sal_usleep(1 + (emul ? 1 : 0) * EMULATION_FACTOR);

        /* Check timeout. */
        if (timeout_us++ > EDB_DRAIN_TIMEOUT_US) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "timeout (%0d us), pport=%0d, num_entries=%0d\n"),
                       timeout_us, pport, num_entries));
        }
    } while (num_entries > 0);

    /* Configure EDB_MISC_CTRL back to previous value. */
    reg     = EDB_MISC_CTRLr;
    fid     = SELECT_CURRENT_USED_ENTRIESf;
    pt_id   = 0;
    pt_inst = pipe_num;
    fval    = fval_saved;

    SHR_IF_ERR_EXIT(td4_edb_pt_write(unit, pt_id, pt_inst, reg, fid, fval));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable or disable port in EDB.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 * \param [in] rst_on Disable port when setting to 1, otherwise enable port.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
td4_edb_port_enable_set(int unit, int lport, int rst_on)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmdrd_fid_t fid;
    uint32_t fval;
    int pport;
    int pt_id, pt_inst;

    SHR_FUNC_ENTER(unit);

    /* Enable or disable port : write EDB_ENABLEm.PRT_ENABLEf. */
    bcmtm_port_map_get(unit, &port_map);
    pport   = port_map->lport_map[lport].pport;

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "edb_flexport_api: lport=%3d, pport=%3d, rst_on=%0d\n"),
               lport, pport, rst_on));

    pt_id   = pport;
    pt_inst = 0;
    mem     = EGR_ENABLEm;
    fid     = PRT_ENABLEf;
    fval    = (rst_on == 1) ? 0 : 1;

    SHR_IF_ERR_EXIT(td4_edb_pt_write(unit, pt_id, pt_inst, mem, fid, fval));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Soft reset for EDB per port buffer.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 * \param [in] rst_on Reset port buffer when setting to 1, otherwise release reset.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
td4_edb_port_buf_sft_rst_set(int unit, int lport, int rst_on)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmdrd_fid_t fid;
    uint32_t fval;
    int pport;
    int pt_id, pt_inst;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    pport   = port_map->lport_map[lport].pport;

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "edb_flexport_api: lport=%3d, pport=%3d, rst_on=%0d\n"),
               lport, pport, rst_on));

    /* Assert port soft reset: write EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf. */
    pt_id   = pport;
    pt_inst = 0;
    mem     = EGR_PER_PORT_BUFFER_SFT_RESETm;
    fid     = ENABLEf;
    fval    = (rst_on == 1) ? 1 : 0;

    SHR_IF_ERR_EXIT(td4_edb_pt_write(unit, pt_id, pt_inst, mem, fid, fval));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set/clear per port EDB_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGm.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 * \param [in] rst_on Reset dev_to_phy mapping value to unused token (default)
 *                    when setting to 1, otherwise set to proper physical port.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
td4_edb_dev2phy_mapping_set(int unit, int lport, int rst_on)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmdrd_fid_t fid;
    uint32_t fval;
    int pport;
    int pt_id, pt_inst;

    SHR_FUNC_ENTER(unit);

    if (rst_on == 1) {
        pport = 0x1FF;
    } else {
        bcmtm_port_map_get(unit, &port_map);
        pport = port_map->lport_map[lport].pport;
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "edb_flexport_api: lport=%3d, pport=%3d\n"),
               lport, pport));

    /* Assert port soft reset: write EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf. */
    pt_id   = lport;
    pt_inst = 0;
    mem     = EDB_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGm;
    fid     = DATAf;
    fval    = pport;

    SHR_IF_ERR_EXIT(td4_edb_pt_write(unit, pt_id, pt_inst, mem, fid, fval));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get EDB port's ep2mmu credit.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 *
 * \retval EP2MMU port credit.
 */
static int
td4_edb_ep2mmu_port_credit_get(int unit, int lport)
{
    int credit = 0;


    if (bcmtm_lport_is_fp(unit, lport)) {
        /* FP */
        credit = td4_edb_e2e_tbl_mmu_credit_get(unit, lport);
    } else if (bcmtm_lport_is_lb(unit, lport)) {
        /* LPBK: LBK credits for >100G */
        credit = TD4_EDB_EP2MMU_CRED_LBK;
    } else if (bcmtm_lport_is_cpu(unit, lport)) {
        /* CPU: CPU credits for 25G */
        credit = TD4_EDB_EP2MMU_CRED_CPU;
    } else if (bcmtm_lport_is_mgmt(unit, lport)) {
        /* MGM: MGM credits for 10G (same as 10G FP port) */
        credit = td4_edb_e2e_tbl_mmu_credit_get(unit, lport);
    }

    return credit;
}

/*!
 * \brief Get port xmit start cnt settings.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport: Logical port.
 * \param [out] xmit_cnt: ptr to port's xmit start cnt array.
 * \param [in] xmit_cnt_size: size of port's xmit start cnt array.
 *
 * \retval 0.
 */
static int
td4_edb_port_xmit_cnt_array_get(int unit, int lport,
                                int *xmit_cnt, int xmit_cnt_size)
{
    int ct_class, dst_ct_class, src_ct_class;
    int saf_xmit_cnt, ct_xmit_cnt;
    int rv;
    bcmdrd_dev_id_t dev_id;

    for (ct_class = 0; ct_class < xmit_cnt_size; ct_class++) {
        xmit_cnt[ct_class] = 0;
    }

    rv = bcmdrd_dev_id_get(unit, &dev_id);
    if (SHR_FAILURE(rv)) {
        return SHR_E_UNIT;
    }
    if (bcmtm_lport_is_fp(unit, lport)) {
        /* FP */
        dst_ct_class = td4_edb_e2e_tbl_ct_class_get(unit, lport);
        saf_xmit_cnt = td4_edb_e2e_tbl_xmit_cnt_get(unit, lport, 0);
        ct_xmit_cnt  = td4_edb_e2e_tbl_xmit_cnt_get(unit, lport, 1);

        if (dev_id.revision >= BCM56880_REV_B0) {
            /* In B0, program same value for all CT classes. */
            for (src_ct_class = 0; src_ct_class < xmit_cnt_size; src_ct_class++) {
                xmit_cnt[src_ct_class] = (src_ct_class == 0) ?
                                         saf_xmit_cnt : ct_xmit_cnt;
            }
        } else {
            /* In A0, program different settings for each CT class. */
            for (src_ct_class = 0; src_ct_class < xmit_cnt_size; src_ct_class++) {
                xmit_cnt[src_ct_class] = (src_ct_class < dst_ct_class) ?
                                         saf_xmit_cnt : ct_xmit_cnt;
            }
        }
    } else if (bcmtm_lport_is_lb(unit, lport)  ||
             bcmtm_lport_is_cpu(unit, lport) ||
             bcmtm_lport_is_mgmt(unit, lport)) {
        /* LPBK, CPU and MGM */
        xmit_cnt[0] = td4_edb_e2e_tbl_xmit_cnt_get(unit, lport, 0);
    }

    return 0;
}

/*!
 * \brief Set/clear EDB mmu_cell_credit per port.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 * \param [in] rst_on Reset port credit to zero when setting to 1,
 *                    otherwise set to proper value
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
td4_edb_mmu_cell_credit_set(int unit, int lport, int rst_on)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmdrd_fid_t fid;
    uint32_t fval;
    int pport;
    int pt_id, pt_inst;
    int credit;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    pport  = port_map->lport_map[lport].pport;
    credit = (rst_on == 1) ? 0 : (td4_edb_ep2mmu_port_credit_get(unit, lport));

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "edb_flexport_api: lport=%3d, pport=%3d, credit=%0d\n"),
               lport, pport, credit));

    /* Assert port soft reset: write EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf. */
    pt_id   = pport;
    pt_inst = 0;
    mem     = EGR_MMU_CELL_CREDITm;
    fid     = CREDITf;
    fval    = credit;

    SHR_IF_ERR_EXIT(td4_edb_pt_write(unit, pt_id, pt_inst, mem, fid, fval));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set EDB CT class per port.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 * \param [in] rst_on Reset port ct_class to zero when setting to 1,
 *                    otherwise set ct_class properly based on port speed.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
td4_edb_ct_class_set(int unit, int lport, int rst_on)
{
    bcmdrd_sid_t mem;
    bcmdrd_fid_t fid;
    uint32_t fval;
    int pt_id, pt_inst;
    int ct_class;

    SHR_FUNC_ENTER(unit);

    /* Only Front Panel ports have CT configured. */
    if (!bcmtm_lport_is_fp(unit, lport)) {
        return 0;
    }

    /*
     * Note: FlexPort doesn't need to reset the entry when port goes down;
     * needs to be touched when port goes up.
     */
    ct_class = (rst_on == 1) ? 0 : (td4_edb_e2e_tbl_ct_class_get(unit, lport));

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "edb_flexport_api: lport=%3d, ct_class=%0d\n"),
               lport, ct_class));

    /* Assert port soft reset: write EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf. */
    pt_id   = lport;
    pt_inst = 0;
    mem     = EDB_IP_CUT_THRU_CLASSm;
    fid     = CUT_THRU_CLASSf;
    fval    = ct_class;

    SHR_IF_ERR_EXIT(td4_edb_pt_write(unit, pt_id, pt_inst, mem, fid, fval));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set per port EDB xmit start count.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport Logical port.
 * \param [in] rst_on Reset per port xmit_cnt values to zero
 *                    when setting to 1, otherwise set to proper values.
 *
 * \note Per port xmit_cnt is an array with size equal to num_ct_classes.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
td4_edb_xmit_start_cnt_set(int unit, int lport, int rst_on)
{
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmdrd_fid_t fid;
    uint32_t fval;
    int pt_id, pt_inst;
    int ct_class;
    int pport, pipe_num;
    int local_lport;
    int xmit_cnt[TD4_EDB_MAX_CT_CLASSES];

    SHR_FUNC_ENTER(unit);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "edb_flexport_api: \n")));

    /*
     * Set all XMIT START CNT values for a port.
     * 0->SAF; [1-7]-> CT; 8-15 reserved;
     */
    bcmtm_port_map_get(unit, &port_map);
    pipe_num = port_map->lport_map[lport].pipe;
    pport    = port_map->lport_map[lport].pport;

    if (rst_on == 1) {
        sal_memset(xmit_cnt, 0, TD4_EDB_MAX_CT_CLASSES * sizeof(int));
    } else {
        td4_edb_port_xmit_cnt_array_get(unit, lport, xmit_cnt,
                                        TD4_EDB_MAX_CT_CLASSES);
    }

    /* Write data into PT memory. */
    pt_inst = pipe_num;
    mem     = EDB_XMIT_START_COUNTm;
    fid     = THRESHOLDf;

    for (ct_class = 0; ct_class < TD4_EDB_MAX_CT_CLASSES; ct_class++) {
        local_lport = lport % TD4_DEV_PORTS_PER_PIPE;
        pt_id = local_lport * TD4_EDB_MAX_CT_CLASSES + ct_class;
        fval = xmit_cnt[ct_class];

        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "edb_flexport_api: lport=%3d, pport=%3d, xmit_cnt[%02d]=%0d\n"),
                   lport, pport, ct_class, fval));

        SHR_IF_ERR_EXIT(td4_edb_pt_write(unit, pt_id, pt_inst, mem, fid, fval));
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * EXTERNAL FUNCTIONS
 */
/*!
 * \brief EDB port-down sequence.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport_bmp Ptr to logical port bitmap.
 *
 * \retval SHR_FUNC_EXIT.
 */
int
td4_port_edb_down(int unit, int *lport_bmp)
{
    int lport;
    int rst_on = 1;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "edb_flexport_api: \n")));

    /*
     * Drain EDB per port buffer until empty.
     * 1. Set EDB_EDB_MISC_CTRLr.SELECT_CURRENT_USED_ENTRIESf to 1.
     * 2. Poll until EGR_MAX_USED_ENTRIESm.LEVELf is equal to 0.
     */
    for (lport = 0; lport < TD4_DEV_PORTS_PER_DEV; lport++) {
        if (lport_bmp[lport]) {
            SHR_IF_ERR_EXIT(td4_edb_drain_port(unit, lport));
        }
    }

    /*
     * Disable EDB to MMU cell request generation.
     * 1. set EDB_ENABLEm.PRT_ENABLEf to 0.
     * 2. set EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf to 1.
     */
    for (lport = 0; lport < TD4_DEV_PORTS_PER_DEV; lport++) {
        if (lport_bmp[lport]) {
            SHR_IF_ERR_EXIT(td4_edb_port_enable_set(unit, lport, rst_on));
            SHR_IF_ERR_EXIT(td4_edb_port_buf_sft_rst_set(unit, lport, rst_on));
        }
    }

    /*
     * Configure port related property.
     * 1. Delete Dev_to_Phy mapping.
     * 2. Reset max Ep2MMU credits to 0.
     */
    for (lport = 0; lport < TD4_DEV_PORTS_PER_DEV; lport++) {
        if (lport_bmp[lport]) {
            SHR_IF_ERR_EXIT(td4_edb_dev2phy_mapping_set(unit, lport, rst_on));
            SHR_IF_ERR_EXIT(td4_edb_mmu_cell_credit_set(unit, lport, rst_on));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief EDB port-up sequence: configure up-ports.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport_bmp Ptr to logical port bitmap.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
td4_port_edb_up_config_ports(int unit, int *lport_bmp)
{
    int lport;
    int rst_on = 0;
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "edb_flexport_api: \n")));

    /*
     * Configure port properties for UP ports.
     * 1. Map Dev to Phy for added port.
     * 2. Configure max Ep2MMU port credits.
     * 3. Configure CT_class for added port.
     * 4. Configure XMIT_START_CNT entries for added port.
     */
    for (lport = 0; lport < TD4_DEV_PORTS_PER_DEV; lport++) {
        if (lport_bmp[lport]) {
            SHR_IF_ERR_EXIT(td4_edb_dev2phy_mapping_set(unit, lport, rst_on));
            SHR_IF_ERR_EXIT(td4_edb_mmu_cell_credit_set(unit, lport, rst_on));
            SHR_IF_ERR_EXIT(td4_edb_ct_class_set(unit, lport, rst_on));
            SHR_IF_ERR_EXIT(td4_edb_xmit_start_cnt_set(unit, lport, rst_on));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief EDB port-up sequence: enable up-ports
 *
 * \param [in] unit: Device unit.
 * \param [in] lport_bmp Ptr to logical port bitmap.
 *
 * \retval SHR_FUNC_EXIT.
 */
static int
td4_port_edb_up_enable_ports(int unit, int *lport_bmp)
{
    int lport;
    int rst_on = 0;
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "edb_flexport_api: \n")));

    /*
     * Release EDB port buffer reset, and enable cell request generation in EP.
     * 1.1 Set EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf to 1. (Assert reset)
     * 1.2 Set EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf to 0. (Des-Assert reset)
     * 2.  Set EDB_ENABLEm.PRT_ENABLEf to 1.
     */
    rst_on = 1;
    for (lport = 0; lport < TD4_DEV_PORTS_PER_DEV; lport++) {
        if (lport_bmp[lport]) {
            SHR_IF_ERR_EXIT(td4_edb_port_buf_sft_rst_set(unit, lport, rst_on));
        }
    }
    rst_on = 0;
    for (lport = 0; lport < TD4_DEV_PORTS_PER_DEV; lport++) {
        if (lport_bmp[lport]) {
            SHR_IF_ERR_EXIT(td4_edb_port_buf_sft_rst_set(unit, lport, rst_on));
        }
    }
    for (lport = 0; lport < TD4_DEV_PORTS_PER_DEV; lport++) {
        if (lport_bmp[lport]) {
            SHR_IF_ERR_EXIT(td4_edb_port_enable_set(unit, lport, rst_on));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EDB port-up sequence.
 *
 * \param [in] unit: Device unit.
 * \param [in] lport_bmp Ptr to logical port bitmap.
 *
 * \retval SHR_FUNC_EXIT.
 */
int
td4_port_edb_up(int unit, int *lport_bmp)
{
    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "edb_flexport_api: \n")));

    /*
     * Configure port properties for UP ports.
     * 1. Map Dev to Phy for added port.
     * 2. Configure max Ep2MMU port credits.
     * 3. Configure CT_class for added port.
     * 4. Configure XMIT_START_CNT entries for added port.
     */
    SHR_IF_ERR_EXIT(td4_port_edb_up_config_ports(unit, lport_bmp));

    /*
     * Release EDB port buffer reset, and enable cell request generation in EP.
     * 1.1 Set EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf to 1. (Assert reset)
     * 1.2 Set EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf to 0. (Des-Assert reset)
     * 2.  Set EDB_ENABLEm.PRT_ENABLEf to 1.
     */
    SHR_IF_ERR_EXIT(td4_port_edb_up_enable_ports(unit, lport_bmp));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * END SDKLT-DV SHARED CODE
 */

/*******************************************************************************
 * BCMTM INTERNAL PUBLIC FUNCTIONS
 */

int
bcm56880_a0_tm_port_edb_down(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;
    int rst_on = 1;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;
    if (lport != -1) {

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "edb_flexport_api: \n")));

        /*
         * Drain EDB per port buffer until empty.
         * 1. Set EDB_EDB_MISC_CTRLr.SELECT_CURRENT_USED_ENTRIESf to 1.
         * 2. Poll until EGR_MAX_USED_ENTRIESm.LEVELf is equal to 0.
         */
        SHR_IF_ERR_EXIT(td4_edb_drain_port(unit, lport));

        /*
         * Disable EDB to MMU cell request generation.
         * 1. set EDB_ENABLEm.PRT_ENABLEf to 0.
         * 2. set EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf to 1.
         */
        SHR_IF_ERR_EXIT(td4_edb_port_enable_set(unit, lport, rst_on));
        SHR_IF_ERR_EXIT(td4_edb_port_buf_sft_rst_set(unit, lport, rst_on));
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcm56880_a0_tm_port_edb_delete(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;
    int rst_on = 1;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;
    if (lport != -1) {

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "edb_flexport_api: \n")));
        /*
         * Configure port related property.
         * 1. Delete Dev_to_Phy mapping.
         * 2. Reset max Ep2MMU credits to 0.
         */
        SHR_IF_ERR_EXIT(td4_edb_dev2phy_mapping_set(unit, lport, rst_on));
        SHR_IF_ERR_EXIT(td4_edb_mmu_cell_credit_set(unit, lport, rst_on));

    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_port_edb_up(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;
    int lport_bmp[TD4_DEV_PORTS_PER_DEV] = {0};

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;
    if (lport != -1) {
        lport_bmp[lport] = 1;
        /*
         * Release EDB port buffer reset, and enable cell request generation in EP.
         * 1.1 Set EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf to 1. (Assert reset)
         * 1.2 Set EDB_PER_PORT_BUFFER_SFT_RESETm.ENABLEf to 0. (Des-Assert reset)
         * 2.  Set EDB_ENABLEm.PRT_ENABLEf to 1.
         */

        SHR_IF_ERR_EXIT
            (td4_port_edb_up_enable_ports(unit, lport_bmp));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_port_edb_add(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;
    int lport_bmp[TD4_DEV_PORTS_PER_DEV] = {0};

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;
    if (lport != -1) {
        lport_bmp[lport] = 1;
        SHR_IF_ERR_EXIT
            (td4_port_edb_up_config_ports(unit, lport_bmp));
    }

exit:
    SHR_FUNC_EXIT();
}
