/*! \file bcmpc_pm_drv_pm8x100.c
 *
 * PM driver for PM8x100 DPLL.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_pm_drv_internal.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmpc/bcmpc_pm_drv_utils.h>
#include "bcmpc_pm_drv_pm8x100_dpll.h"
#include "bcmpc_pm_drv_pm8x100_common.h"


/******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PMGR

/* Number of lanes per core in PM8X100. */
#define PM8X100_DPLL_NUM_LANES_PER_CORE 8

/* Number of lanes per PML in PM8x100. */
#define PM8X100_DPLL_NUM_LANES_PER_PML 4

#define PM8X100_DPLL_NRZ_LANE_SPEED_MAX 25000
#define PM8X100_DPLL_PHY_SIGNALLING_MODE_GET(speed, num_lanes) \
        (speed/num_lanes) > PM8X100_DPLL_NRZ_LANE_SPEED_MAX ? \
                        phymodSignallingMethodPAM4 : phymodSignallingMethodNRZ

#define PM8X100_DPLL_OS5_SUPPORT_SPEED_MODE(speed, num_lanes) \
                                    (speed/num_lanes) == 10000 ? 1 : 0
#define PM8X100_DPLL_PML_INDEX_GET(lane_mask) \
                              (lane_mask) & 0xf ? 0 : 1

#define PM8X100_DPLL_PMD_FW_INFO_SIZE  128
#define PM8X100_DPLL_MAX_AN_ABILITY 31
#define PM8X100_DPLL_MAX_SPEED_ABILITY 33

/******************************************************************************
 * PM driver private functions.
 */
static int
pm8x100_dpll_lanebitmap_set(int starting_lane, int num_lane, uint8_t *bitmap)
{
    int i;

    for (i = 0; i < num_lane; i++) {
        *bitmap |= 1 << (starting_lane + i);
    }

    return SHR_E_NONE;
}

static void
pm8x100_dpll_port_index_get(const pm_access_t *pm_acc,
                       pm_info_t *pm_info, int *index)
{
    const phymod_phy_access_t *phy_access;
    int idx = 0;

    phy_access = &(pm_acc->phy_acc);

    for (idx = 0; idx < PM8X100_DPLL_NUM_LANES_PER_CORE; idx++) {
        if ((phy_access->access.lane_mask >> idx) & 0x1) {
            break;
        }
    }
    *index = idx;
}

/*
 * This function will return the sister port on the same MPP for a given
 * port index within the PM.
 * PM8x100 support 8 logical ports within two PMLs.
 * each PML support 4 logical ports
 * Each MPP has 2 logical ports, the index is shown as below:
 *  MPP0:   Port 0, Port 1.  PML0
 *  MPP1:   Port 2, Port 3.  PML0
 *  MPP0:   Port 4, Port 5.  PML1
 *  MPP1:   Port 6, Port 7.  PML2
 */
static int
pm8x100_dpll_mpp_sister_port_index_get(int port_index, int *sister_port_index)
{
    switch (port_index) {
        case 0:
            *sister_port_index = 1;
            break;
        case 1:
            *sister_port_index = 0;
            break;
        case 2:
            *sister_port_index = 3;
            break;
        case 3:
            *sister_port_index = 2;
            break;
        case 4:
            *sister_port_index = 5;
            break;
        case 5:
            *sister_port_index = 4;
            break;
        case 6:
            *sister_port_index = 7;
            break;
        case 7:
            *sister_port_index = 6;
            break;
        default:
            return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

static int
pm8x100_dpll_required_vco_bitmap_get(int unit, pm_vco_t vco,
                               pm8x100_dpll_vcos_bmp_t *required_vco_bmp)
{
    if (vco == PM_VCO_51P562G) {
        PM8X100_DPLL_VCO_BMP_51P563G_SET(*required_vco_bmp);
    } else if (vco == PM_VCO_53P125G) {
        PM8X100_DPLL_VCO_BMP_53P125G_SET(*required_vco_bmp);
    }

    return SHR_E_NONE;
}


/*
 * This function will check if the given port_index
 * is a valid logical port that is already attached to the PM or not.
 */
static void
pm8x100_dpll_port_valid_check(pm_info_t *pm_info, int port_index, int *is_valid)
{
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;

    if ((pm_ha_info->tvco_active_lane_bmap >> port_index) & 0x1) {
        *is_valid = 1;
    } else if ((pm_ha_info->ovco_active_lane_bmap >> port_index) & 0x1) {
        *is_valid = 1;
    } else {
        *is_valid = 0;
    }
}


static int
pm8x100_dpll_pll_to_vco_get(int unit, const pm_access_t *pm_acc,
                      pm_oper_status_t *op_st, uint32_t pll, pm_vco_t *vco)
{
    SHR_FUNC_ENTER(unit);

    /* PM8x100 is re-using TSCBH PLL enum. */
    switch (pll) {
        case phymod_TSCBH_PLL_DIV132:
            *vco = PM_VCO_41P25G;
            break;
        case phymod_TSCBH_PLL_DIV165:
            *vco = PM_VCO_51P562G;
            break;
        case phymod_TSCBH_PLL_DIV170:
            *vco = PM_VCO_53P125G;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_VCO_UNAVAIL);
    }
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_vco_to_pll_get(int unit, pm_oper_status_t *op_st, pm_info_t *pm_info,
                       pm_vco_t vco, uint32_t *pll)
{
    SHR_FUNC_ENTER(unit);

    /* PM8x100 is re-using TSCBH PLL enum. */
    switch (vco) {
        case PM_VCO_41P25G:
            *pll = phymod_TSCBH_PLL_DIV132;
            break;
        case PM_VCO_51P562G:
            *pll = phymod_TSCBH_PLL_DIV165;
            break;
        case PM_VCO_53P125G:
            *pll = phymod_TSCBH_PLL_DIV170;
            break;
        case PM_VCO_INVALID:
            *pll = phymod_TSCBH_PLL_DIVNONE;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
            break;
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_VCO_UNAVAIL);
    }
    SHR_FUNC_EXIT();
}

/* Get the current VCO rate from the hardware. */
static int
pm8x100_dpll_pm_vcos_get(int unit, const pm_access_t *pm_acc,
                   pm_oper_status_t *op_st, pm_info_t *pm_info,
                   pm_vco_t *vcos)
{
    int tvco_pll_idx = 0, ovco_pll_idx = 0;
    uint32_t tvco_pll_div = 0, ovco_pll_div = 0, is_pll_pwrdn = 1;
    pm_vco_t cur_tvco = PM_VCO_INVALID, cur_ovco = PM_VCO_INVALID;
    pm_access_t pm_acc_copy;
    phymod_phy_access_t *phy_acc = &(pm_acc_copy.phy_acc);

    SHR_FUNC_ENTER(unit);
    sal_memcpy(&pm_acc_copy, pm_acc, sizeof(pm_acc_copy));

    if ( phy_acc->type == phymodDispatchTypeNull ) {
        return SHR_E_NONE;
    }

    /* Get TVCO */
    tvco_pll_idx = pm_acc->phy_acc.access.tvco_pll_index;
    phy_acc->access.pll_idx = tvco_pll_idx;
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_pll_powerdown_get(phy_acc, tvco_pll_idx, &is_pll_pwrdn));
    if (is_pll_pwrdn) {
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "The TVCO is powered off on port %"PRIu32".\n"),
                    pm_acc->pport));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_pll_multiplier_get(phy_acc, &tvco_pll_div));
    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x100_dpll_pll_to_vco_get(unit, pm_acc, op_st,
                               tvco_pll_div, &cur_tvco));

    /* Check if TVCO is valid */
    if (cur_tvco == PM_VCO_INVALID) {
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "The TVCO is invalid on physical port %"PRIu32".\n"),
                    pm_acc->pport));
        SHR_ERR_EXIT(SHR_E_FAIL);

    }
    vcos[0] = cur_tvco;

    /* Get OVCO
     * Check if OVCO is powered down.
     * If OVCO is not powered down, then get currrent OVCO.
     */
    ovco_pll_idx = (tvco_pll_idx) ? 0 : 1;
    phy_acc->access.pll_idx = ovco_pll_idx;
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_pll_powerdown_get(phy_acc, ovco_pll_idx, &is_pll_pwrdn));
    if (is_pll_pwrdn == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (phymod_phy_pll_multiplier_get(phy_acc, &ovco_pll_div));
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_pll_to_vco_get(unit, pm_acc, op_st, ovco_pll_div,
                                   &cur_ovco));
    }
    vcos[1] = cur_ovco;

exit:
    SHR_FUNC_EXIT();
}


static void
pm8x100_dpll_fec_lanebitmap_get(pm_info_t *pm_info,
                               uint8_t *rs544_bitmap,
                               uint8_t *rs272_bitmap)
{
    int idx;
    pm_port_fec_t tmp_fec;
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;

    *rs544_bitmap = 0;
    *rs272_bitmap = 0;

    /* Get RS544, RS272 usage bitmap from WB */
    for (idx = 0; idx < PM8X100_DPLL_NUM_LANES_PER_CORE; idx++) {
        if ((pm_ha_info->an_enable_lane_bmap >> idx) & 0x1) {
            /* Auto-negotiation FEC usage. */
            tmp_fec = pm_ha_info->an_lane_fec_map[idx];
        } else {
            /* Force speed FEC usage. */
            tmp_fec = pm_ha_info->lane_fec_map[idx];
        }

        if ((tmp_fec == PM_PORT_FEC_RS_544) ||
            (tmp_fec == PM_PORT_FEC_RS_544_2XN)) {
            *rs544_bitmap |= 1 << idx;
        } else if ((tmp_fec == PM_PORT_FEC_RS_272) ||
                   (tmp_fec == PM_PORT_FEC_RS_272_2XN)) {
            *rs272_bitmap |= 1 << idx;
        }
    }
}

/*
 * This function is to validate the FEC settings on each PM.
 * For each MPP, we can not support both RS544 and RS272.
 */
static int
pm8x100_dpll_fec_validate(int unit,
                        uint32_t lane_mask,
                        pm_info_t *pm_info,
                        uint8_t rs544_bitmap,
                        uint8_t rs272_bitmap,
                        uint8_t *affected_lane_bitmap)
{
    uint8_t rs544_mpp0, rs544_mpp1, rs272_mpp0, rs272_mpp1;
    uint8_t rs544_mpp0_u1, rs544_mpp1_u1, rs272_mpp0_u1, rs272_mpp1_u1;
    int pml_index = 0;


    SHR_FUNC_ENTER(unit);

    pml_index = PM8X100_DPLL_PML_INDEX_GET(lane_mask);

    /* first check PML index, based on the PML index get the proper fec
     * bitmap properly */
    if (pml_index == 0) {
        rs544_mpp0 = rs544_bitmap & 0x3;
        rs544_mpp1 = rs544_bitmap & 0xc;

        rs272_mpp0 = rs272_bitmap & 0x3;
        rs272_mpp1 = rs272_bitmap & 0xc;

        if (rs544_mpp0 && rs272_mpp0) {
            *affected_lane_bitmap = rs544_mpp0 | rs272_mpp0;
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else if (rs544_mpp1 && rs272_mpp1) {
            *affected_lane_bitmap = rs544_mpp1 | rs272_mpp1;
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    } else {
        /* next get U1 fec lane bitmap */
        rs544_mpp0_u1 = rs544_bitmap & 0x30;
        rs544_mpp1_u1 = rs544_bitmap & 0xc0;

        rs272_mpp0_u1 = rs272_bitmap & 0x30;
        rs272_mpp1_u1 = rs272_bitmap & 0xc0;

        /* next verify U1 fec config */
        if (rs544_mpp0_u1 && rs272_mpp0_u1) {
            *affected_lane_bitmap = rs544_mpp0_u1 | rs272_mpp0_u1;
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else if (rs544_mpp1_u1 && rs272_mpp1_u1) {
            *affected_lane_bitmap = rs544_mpp1_u1 | rs272_mpp1_u1;
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_pm_port_lane_map_validate(int unit, pm_oper_status_t *op_st,
                                      pm_info_t *pm_info, uint32_t lane_map)
{
    int valid = 0;
    int idx;
    uint32_t lmap_lane[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
                            0x3, 0xc, 0x30, 0xc0,
                            0xf, 0xf0, 0xff};

    SHR_FUNC_ENTER(unit);

    /* Check for lane map. */
    for (idx = 0; idx < COUNTOF(lmap_lane); idx++) {
        if (lane_map == lmap_lane[idx]) {
            valid = 1;
            break;
        }
    }

    if (!valid) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_LANE_MASK_INVALID);
    }
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_speed_to_vco_get(int unit, pm_oper_status_t *op_st,
                             pm_info_t *pm_info, pm_speed_config_t *speed_cfg,
                             pm_vco_t *vco)
{
    int idx;
    const pm8x100_dpll_force_speed_ability_entry_t *abil = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < COUNTOF(pm8x100_dpll_force_speed_ability_table); idx++) {
        abil = &(pm8x100_dpll_force_speed_ability_table[idx]);
        if (abil->speed == speed_cfg->speed &&
            abil->num_lanes == speed_cfg->num_lanes &&
            abil->fec == speed_cfg->fec) {
            if (vco) {
                *vco = abil->vco;
            }
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_SPEED_INVALID);
    }
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_vcos_request_allocate(int unit, const  pm_access_t *pm_acc,
                             pm_oper_status_t *op_st,
                             const pm8x100_dpll_vcos_bmp_t required_vcos_bmp,
                             pm_vco_t *current_tvco,
                             pm_vco_t *current_ovco)
{
    pm8x100_dpll_vcos_bmp_t required_vcos_bmp_temp;
    pm_vco_t required_vco[3];
    int required_vco_count = 0, idx;
    pm_oper_status_type_t status = PM_OPER_SUCCESS;

    SHR_FUNC_ENTER(unit);

    required_vcos_bmp_temp = required_vcos_bmp;

    SHR_BITCOUNT_RANGE(&required_vcos_bmp, required_vco_count, 0, PM_VCO_COUNT);

    if (required_vco_count == 0) {
        SHR_EXIT();
    } else if (required_vco_count > 2) {
        status = PM_VCO_UNAVAIL;
        SHR_ERR_EXIT(SHR_E_FAIL);
    } else {
        required_vco[0] = PM_VCO_INVALID;
        required_vco[1] = PM_VCO_INVALID;
        idx = 0;
        if (PM8X100_DPLL_VCO_BMP_53P125G_GET(required_vcos_bmp_temp)) {
            required_vco[idx] = PM_VCO_53P125G;
            idx++;
        }
        if (PM8X100_DPLL_VCO_BMP_51P563G_GET(required_vcos_bmp_temp)) {
            required_vco[idx] = PM_VCO_51P562G;
            idx++;
        }
    }

    /* One or two new VCO rates are required. */
    /* 1. TVCO is free to change. */
    if (*current_tvco == PM_VCO_INVALID) {
        *current_tvco = required_vco[0];
        *current_ovco = required_vco[1];

    /* 2. TVCO is not free, while OVCO is free to change. */
    } else if (*current_ovco == PM_VCO_INVALID) {

        /* 2.1. One new VCO rate is the same as current TVCO. */
        if (*current_tvco == required_vco[0]) {
            *current_ovco = required_vco[1];
        } else if (*current_tvco == required_vco[1]) {
            *current_ovco = required_vco[0];

        /* 2.2. Only one new VCO is required, and it is not the
         * same as current TVCO.
         */
        } else if (required_vco[1] == PM_VCO_INVALID) {
            *current_ovco = required_vco[0];

        /*
         * 2.3. Two new VCO are required, and neither of them is
         * the same as current TVCO.
         */
        } else {
            status = PM_VCO_UNAVAIL;
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, status);
    }
    SHR_FUNC_EXIT();
}


static int
pm8x100_dpll_vcos_setting_validate(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             uint32_t flag, pm8x100_dpll_vcos_bmp_t required_vcos_bmp,
                             pm_vco_setting_t *vco_setting)
{
    uint8_t ovco_active_lane_bitmap, tvco_active_lane_bitmap;
    uint8_t ovco_adv_lane_bitmap, tvco_adv_lane_bitmap;
    int is_tvco_in_use, is_ovco_in_use;
    uint32_t port_lane_mask = 0;
    pm_vco_t current_tvco, current_ovco, vcos[2] = {0};
    const phymod_phy_access_t *phy_access;
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;

    SHR_FUNC_ENTER(unit);

    ovco_active_lane_bitmap = pm_ha_info->ovco_active_lane_bmap &
                              (~pm_ha_info->an_enable_lane_bmap);
    tvco_active_lane_bitmap = pm_ha_info->tvco_active_lane_bmap &
                              (~pm_ha_info->an_enable_lane_bmap);
    ovco_adv_lane_bitmap = pm_ha_info->ovco_advert_lane_bmap &
                           pm_ha_info->an_enable_lane_bmap;
    tvco_adv_lane_bitmap = pm_ha_info->tvco_advert_lane_bmap &
                           pm_ha_info->an_enable_lane_bmap;

    phy_access = &(pm_acc->phy_acc);
    port_lane_mask = phy_access->access.lane_mask;
    ovco_active_lane_bitmap &= ~port_lane_mask;
    ovco_adv_lane_bitmap &= ~port_lane_mask;
    tvco_active_lane_bitmap &= ~port_lane_mask;
    tvco_adv_lane_bitmap &= ~port_lane_mask;

    SHR_IF_ERR_VERBOSE_EXIT(pm8x100_dpll_pm_vcos_get(unit, pm_acc, op_st, pm_info, vcos));
    is_ovco_in_use =
        ((ovco_active_lane_bitmap | ovco_adv_lane_bitmap) == 0)? 0: 1;

    if (is_ovco_in_use) {
        current_ovco = vcos[1];
    } else {
        current_ovco = PM_VCO_INVALID;
    }

    is_tvco_in_use = (((tvco_active_lane_bitmap | tvco_adv_lane_bitmap)
                      || is_ovco_in_use) == 0)? 0: 1;
    if (is_tvco_in_use) {
        current_tvco = vcos[0];
    } else {
        current_tvco = PM_VCO_INVALID;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x100_dpll_vcos_request_allocate(unit, pm_acc, op_st, required_vcos_bmp,
                                      &current_tvco, &current_ovco));

    vco_setting->tvco = current_tvco;
    vco_setting->is_tvco_new = (vcos[0] == current_tvco)? 0: 1;
    vco_setting->ovco = current_ovco;
    vco_setting->is_ovco_new = (vcos[1] == current_ovco)? 0: 1;

exit:
    SHR_FUNC_EXIT();
}


static int
pm8x100_dpll_autoneg_ability_to_vco_get(int unit,
                                       pm_info_t *pm_info,
                                       pm_port_ability_t ability,
                                       pm_vco_t *vco)
{
    int idx;
    const pm8x100_dpll_autoneg_ability_table_entry_t *abil = NULL;
    SHR_FUNC_ENTER(unit);

    *vco = PM_VCO_INVALID;

    for (idx = 0; idx < COUNTOF(pm8x100_dpll_autoneg_ability_table); idx++) {
        abil = &pm8x100_dpll_autoneg_ability_table[idx];
        if (abil->speed == ability.speed &&
            abil->num_lanes == ability.num_lanes &&
            abil->fec == ability.fec_type &&
            abil->an_mode == ability.an_mode) {
            *vco = abil->vco;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();

}

static int
pm8x100_dpll_port_mode_update(int unit, const pm_access_t *pm_acc,
                         pm_oper_status_t *op_st, pm_info_t *pm_info,
                         uint32_t data_rate)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    uint32_t lane_mask = 0, flags = 0;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));
    lane_mask = pm_acc->phy_acc.access.lane_mask;

    if ((lane_mask == 0xff) ||
        (((lane_mask == 0xf) || (lane_mask == 0xf0)) && (data_rate > 200000))) {
        BCMPMAC_PM_PORT_MODE_MUTI_MPP_SET(flags);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->port_mode_set(&pmac_acc, pm_acc->pmac_blkport,
                              flags, lane_mask));

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_MAC_OPER_ERROR);
    }
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_pm_vco_reconfig(int unit, const pm_access_t *pm_acc,
                            pm_oper_status_t *op_st, pm_info_t *pm_info,
                            pm_vco_t new_vco, int pll_idx)
{
    uint32_t pll_div;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x100_dpll_vco_to_pll_get(unit, op_st, pm_info, new_vco, &pll_div));

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_pll_reconfig(phy_acc, pll_idx, pll_div, pm_info->pm_data.ref_clk));

    pm_ha_info->speed_id_table_status[0] = PM_SPEED_ID_TABLE_STATUS_NOT_LOADED;
    pm_ha_info->speed_id_table_status[1] = PM_SPEED_ID_TABLE_STATUS_NOT_LOADED;


exit:
    SHR_FUNC_EXIT();
}

/*
 * This function will return the AN mode for a given
 * port_index on the PM.
 * If the port is in Force speed mode, AN mode will returned as
 * PM_PORT_AUTONEG_MODE_NONE.
 */
static void
pm8x100_dpll_port_an_mode_get(pm_info_t *pm_info, int port_index,
                         pm_port_autoneg_mode_t *an_mode)
{
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;

    if ((pm_ha_info->an_enable_lane_bmap >> port_index) & 0x1) {
        /* The first lane of the port is in AN mode. */
        *an_mode = pm_ha_info->an_mode[port_index];
    } else {
        *an_mode = PM_PORT_AUTONEG_MODE_NONE;
    }

}

/******************************************************************************
 * PM driver function vectors.
 */

/*
 * This function will validate a port's speed configuration:
 * 1. If the lane mask for the port is valid.
 * 2. If the speed configuration is supported by the PM.
 * 3. If there's any FEC confliction within each MPP. (non-first port.)
 * 4. If the current VCO can support the speed configuration. (non-first port.)
 * 5. Reconfigure VCO when the port is the only active port on the PM and the
 *    new speed requries VCO change.
 */
static int
pm8x100_dpll_pm_port_config_validate(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st, pm_info_t *pm_info,
                               pm_speed_config_t *spd_cfg, int is_first)
{
    int tvco_pll_idx = 0, ovco_pll_idx = 0;
    int rv = SHR_E_NONE;
    pm_vco_t cur_tvco = PM_VCO_INVALID, cur_ovco = PM_VCO_INVALID;
    pm_vco_t req_vco, new_vco = PM_VCO_INVALID;
    uint32_t tvco_pll_div = 0, ovco_pll_div = 0, is_pll_pwrdn = 1;
    bool rs544_req = false, rs272_req = false;
    uint8_t rs544_bm = 0, rs272_bm = 0, affected_bm = 0;
    pm_access_t pm_acc_copy;
    phymod_phy_access_t *phy_acc;
    pm_oper_status_type_t status = PM_OPER_SUCCESS;
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;

    SHR_FUNC_ENTER(unit);

    /* 1. Check lane_map. */
    rv = pm8x100_dpll_pm_port_lane_map_validate(unit, op_st, pm_info,
                                           pm_acc->phy_acc.access.lane_mask);
    if (SHR_FAILURE(rv)) {
        status = op_st->status;
        SHR_ERR_EXIT(rv);
    }

    /*
     * 2. Validate force speed ability.
     * Check if port speed configuration is one of the entries of
     * force_speed_ability table.
     */
    if (SHR_FAILURE(pm8x100_dpll_speed_to_vco_get(unit, op_st, pm_info,
                                             spd_cfg, &req_vco))) {
        status = op_st->status;
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* If the port is the first port on the PM, just return. */
    if (is_first) {
        return SHR_E_NONE;
    }

    sal_memcpy(&pm_acc_copy, pm_acc, sizeof(pm_acc_copy));
    phy_acc = &(pm_acc_copy.phy_acc);

    /* Get TVCO */
    tvco_pll_idx = pm_acc->phy_acc.access.tvco_pll_index;
    phy_acc->access.pll_idx = tvco_pll_idx;

    /* Use the first lane. */
    phy_acc->access.lane_mask = 0x1;

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_pll_powerdown_get(phy_acc, 0, &is_pll_pwrdn));
    if (is_pll_pwrdn) {
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "The VCO is powered off on physical port %"PRIu32".\n"),
                    pm_acc->pport));
        SHR_ERR_EXIT(SHR_E_FAIL);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (phymod_phy_pll_multiplier_get(phy_acc, &tvco_pll_div));
        rv = pm8x100_dpll_pll_to_vco_get(unit, pm_acc, op_st, tvco_pll_div, &cur_tvco);
        if (SHR_FAILURE(rv)) {
            status = op_st->status;
            SHR_ERR_EXIT(rv);
        }
    }
    /* Check if VCO is valid */
    if (cur_tvco == PM_VCO_INVALID) {
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "The VCO is invalid.\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);

    }

    /* Get OVCO */
    /*
     * Check if OVCO is powered down.
     * If OVCO is not powered down, then get currrent OVCO.
     */
    ovco_pll_idx = (tvco_pll_idx) ? 0 : 1;
    phy_acc->access.pll_idx = ovco_pll_idx;
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_pll_powerdown_get(phy_acc, ovco_pll_idx, &is_pll_pwrdn));
    if (is_pll_pwrdn == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (phymod_phy_pll_multiplier_get(phy_acc, &ovco_pll_div));
        rv = pm8x100_dpll_pll_to_vco_get(unit, pm_acc, op_st, ovco_pll_div,
                                   &cur_ovco);
        if (SHR_FAILURE(rv)) {
            status = op_st->status;
            SHR_ERR_EXIT(rv);
        }
    }

    if (req_vco != PM_VCO_INVALID) {
        if (req_vco == cur_tvco) {
            new_vco = PM_VCO_INVALID;
        } else if (req_vco == cur_ovco) {
            new_vco = PM_VCO_INVALID;
        } else if (cur_ovco == PM_VCO_INVALID) {
            new_vco = req_vco;
        } else if ((cur_ovco != PM_VCO_INVALID) &&
                   ((pm_ha_info->ovco_active_lane_bmap == 0) ||
                    (pm_ha_info->ovco_active_lane_bmap ==
                          pm_acc->phy_acc.access.lane_mask)) &&
                  ((pm_ha_info->ovco_advert_lane_bmap & pm_ha_info->an_enable_lane_bmap) == 0)) {
            new_vco = req_vco;
        } else {
            /* req_vco is other than TVCO and OVCO */
            status = PM_VCO_UNAVAIL;
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Requested VCO %"PRIu32" for speed "
                                  "%"PRIu32" cannot be configured on current "
                                  "settings TVCO %"PRIu32", OVCO %"PRIu32"\n"),
                       req_vco, spd_cfg->speed, cur_tvco, cur_ovco));
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
   } else {
       /* req_vco is invalid */
       status = PM_VCO_UNAVAIL;
       LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "The reqested VCO is invalid on "
                             "logical port.\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
   }

   /* Check if need to reconfig OVCO */
   if (new_vco != PM_VCO_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_pm_vco_reconfig(unit, pm_acc, op_st, pm_info, new_vco,
                                    ovco_pll_idx));
       LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "The reqested VCO is reconfigured on "
                             "logical port.\n")));
   }

   /* 3. Validate FEC settings on each MPP. */
    if ((spd_cfg->fec == PM_PORT_FEC_RS_544) ||
        (spd_cfg->fec == PM_PORT_FEC_RS_544_2XN)) {
        rs544_req = true;
    } else if ((spd_cfg->fec == PM_PORT_FEC_RS_272) ||
        (spd_cfg->fec == PM_PORT_FEC_RS_272_2XN)) {
        rs272_req = true;
    }

    sal_memcpy(&pm_acc_copy, pm_acc, sizeof(pm_acc_copy));
    phy_acc = &(pm_acc_copy.phy_acc);

    if (rs544_req || rs272_req) {
        pm8x100_dpll_fec_lanebitmap_get(pm_info, &rs544_bm, &rs272_bm);
        rs544_bm &= ~phy_acc->access.lane_mask;
        rs272_bm &= ~phy_acc->access.lane_mask;
        if (rs544_req) {
            rs544_bm |= phy_acc->access.lane_mask;
        }
        if (rs272_req) {
            rs272_bm |= phy_acc->access.lane_mask;
        }
        rv = pm8x100_dpll_fec_validate(unit, phy_acc->access.lane_mask, pm_info,
                                  rs544_bm, rs272_bm, &affected_bm);
        if (SHR_FAILURE(rv)) {
            status = PM_ABILITY_FEC_CONFLICT;
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Speed_config_validate failed on FEC "
                                 "validation.\n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, status);
    }
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_pm_core_probe(int unit, const pm_access_t *pm_acc,
                      pm_oper_status_t *op_st, pm_info_t *pm_info)
{
    uint32_t is_core_probed = 0;
    const phymod_core_access_t *core_acc = &(pm_acc->core_acc);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_core_identify(core_acc, 0, &is_core_probed));

    if (!is_core_probed) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_PHY_OPER_ERROR);
    }
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_pm_serdes_core_init(int unit, const pm_access_t *pm_acc,
                            pm_oper_status_t *op_st, pm_info_t *pm_info,
                            pm_port_add_info_t *add_info)
{
    int rv = 0;
    phymod_core_init_config_t core_config;
    phymod_core_status_t core_status;
    const phymod_core_access_t *core_acc = &(pm_acc->core_acc);
    pm_vco_t tvco, ovco;
    uint32_t tx_lane_map = 0, rx_lane_map = 0, i;
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    uint32_t pll0 = 0, pll1 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (phymod_core_init_config_t_init(&core_config));
    SHR_IF_ERR_VERBOSE_EXIT
       (phymod_core_status_t_init(&core_status));
    core_status.pmd_active = 0;

    /* Probe serdes core */
    if (PM_PORT_ADD_F_INIT_PROBE_GET(add_info)) {
        rv = pm8x100_dpll_pm_core_probe(unit, pm_acc, op_st, pm_info);
    }
    if (SHR_FAILURE(rv)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                     "The PM serdes core is not probed"
                     "for port %"PRIu32".\n"), pm_acc->pport));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    if (add_info->tvco != PM_VCO_INVALID) {
        tvco = add_info->tvco;
        ovco = add_info->ovco;
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                     "The PM serdes core doesn't have VCO information"
                     "for port %"PRIu32".\n"), pm_acc->pport));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    if  (pm_acc->phy_acc.access.tvco_pll_index) {
        /* Get associated PLL, tvco uses pll1 */
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_vco_to_pll_get(unit, op_st, pm_info,
                                   tvco, &pll1));
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_vco_to_pll_get(unit, op_st, pm_info,
                                   ovco, &pll0));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_vco_to_pll_get(unit, op_st, pm_info,
                                   tvco, &pll0));
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_vco_to_pll_get(unit, op_st, pm_info,
                                   ovco, &pll1));
    }

    core_config.pll0_div_init_value = pll0;
    core_config.pll1_div_init_value = pll1;

    /* Add lane map config */
    core_config.lane_map = add_info->init_config.lane_map;

    /* Translate the lane map into encoded format for RLM usage. */
    for (i = 0; i < core_config.lane_map.num_of_lanes; i++) {
        tx_lane_map |= core_config.lane_map.lane_map_tx[i] << (4 * i);
        rx_lane_map |= core_config.lane_map.lane_map_rx[i] << (4 * i);
    }

    core_config.polarity_map = add_info->init_config.polarity;
    core_config.interface.ref_clock = pm_info->pm_data.ref_clk;
    core_config.interface.com_clock = pm_info->pm_data.ref_clk;
    core_config.firmware_load_method = add_info->init_config.fw_load_method;
    core_config.firmware_loader = add_info->init_config.fw_loader;

    if (!PM_PORT_ADD_F_INIT_FIRMWARE_CRC_VERIFY_GET(add_info)) {
        PHYMOD_CORE_INIT_F_BYPASS_CRC_CHECK_SET(&core_config);
    }
    if (PM_PORT_ADD_F_INIT_FIRMWARE_LOAD_VERIFY_GET(add_info)) {
        PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_config);
    }

    if (PM_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
        (!pm_ha_info->firmware_downloaded)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&core_config);
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_CLR(&core_config);
    }

    if (PM_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_CLR(&core_config);
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&core_config);
    }

    /* need to check for pass2 need to bypass  PMD config */
    if (PM_PORT_ADD_F_INIT_PASS2_BYPASS_PMD_CONFIG_GET(add_info)) {
        PHYMOD_CORE_INIT_F_BYPASS_PMD_CONFIGURATION_SET(&core_config);
    } else {
        PHYMOD_CORE_INIT_F_BYPASS_PMD_CONFIGURATION_CLR(&core_config);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_core_init(core_acc, &core_config,
                          &core_status));

    /* Update firmware download flag if apply */
    if (PM_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        pm_ha_info->firmware_downloaded = 1;
    }
    /* only if core init pass2 and not bypass PMD flag */
    if (PM_PORT_ADD_F_INIT_PASS2_GET(add_info) &&
        !(PM_PORT_ADD_F_INIT_PASS2_BYPASS_PMD_CONFIG_GET(add_info))) {
        pm_ha_info->is_pm_initialized = 1;
    }

    pm_ha_info->rlm_original_rx_lane_swap = rx_lane_map;
    pm_ha_info->rlm_original_tx_lane_swap = tx_lane_map;

exit:
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_pm_init(int unit, const pm_access_t *pm_acc, pm_info_t *pm_info)
{
    pm_access_t pm_acc_copy;

    SHR_FUNC_ENTER(unit);
    sal_memcpy(&pm_acc_copy, pm_acc, sizeof(pm_acc_copy));

    /* Check the pmd_fw info_ptr. */
    if (pm_info->pm_data.pmd_fw_info == NULL) {
        /* Allocate a new entry. */
        SHR_ALLOC(pm_info->pm_data.pmd_fw_info,
                 (sizeof(char) * PM8X100_DPLL_PMD_FW_INFO_SIZE), "bcmpcPmdInfo");
        SHR_NULL_CHECK(pm_info->pm_data.pmd_fw_info, SHR_E_MEMORY);

        sal_memset(pm_info->pm_data.pmd_fw_info, 0,
                   (sizeof(char) * PM8X100_DPLL_PMD_FW_INFO_SIZE));
    }

exit:
    SHR_FUNC_EXIT();

}

static int
pm8x100_dpll_core_attach(int unit, const pm_access_t *pm_acc,
                        pm_oper_status_t *op_st, pm_info_t *pm_info,
                        pm_port_add_info_t *add_info)
{
    pm_access_t pm_acc_copy;

    SHR_FUNC_ENTER(unit);
    sal_memcpy(&pm_acc_copy, pm_acc, sizeof(pm_acc_copy));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x100_dpll_pm_init(unit, pm_acc, pm_info));

    if (pm_acc_copy.core_acc.access.pmd_info_ptr == NULL) {
        pm_acc_copy.core_acc.access.pmd_info_ptr = pm_info->pm_data.pmd_fw_info;
        pm_acc_copy.phy_acc.access.pmd_info_ptr = pm_info->pm_data.pmd_fw_info;
    }

    SHR_IF_ERR_VERBOSE_EXIT
       (pm8x100_dpll_pm_serdes_core_init(unit, &pm_acc_copy, op_st,
                                        pm_info, add_info));

exit:
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_port_add(int unit, const pm_access_t *pm_acc,
                     pm_oper_status_t *op_st, pm_info_t *pm_info,
                     pm_port_add_info_t *add_info)
{
    int idx, num_lanes = 0;
    phymod_phy_init_config_t init_config;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (phymod_phy_init_config_t_init(&init_config));

    num_lanes = add_info->speed_config.num_lanes;
    for (idx = 0; idx < num_lanes; idx++) {
        sal_memcpy(&init_config.tx[idx],
            &add_info->init_config.tx_params[idx], sizeof(phymod_tx_t));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_init(phy_acc, &init_config));

exit:
    SHR_FUNC_EXIT();
}

/*
 * We do not validate the speed_config within this function.
 * The assumption here is caller already verified the speed_config
 * by calling PM function pm_port_config_validate.
 */
static int
pm8x100_dpll_port_speed_config_set(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st, pm_info_t *pm_info,
                                  pm_speed_config_t *speed_config)
{
    phymod_phy_signalling_method_t sig_mode = phymodSignallingMethodNRZ;
    phymod_phy_speed_config_t phy_speed_config;
    phymod_phy_pll_state_t old_pll_state, new_pll_state;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    int port_index, port_num_lanes, i;
    pm_vco_t req_vco, cur_vcos[2] = {0};
    pm_encap_t encap;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_speed_config_t_init(&phy_speed_config));
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_pll_state_t_init(&old_pll_state));
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_pll_state_t_init(&new_pll_state));

    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);
    SHR_BITCOUNT_RANGE(&(phy_acc->access.lane_mask), port_num_lanes,
                       0, PM8X100_DPLL_NUM_LANES_PER_CORE);
    phy_speed_config.data_rate = speed_config->speed;
    phy_speed_config.fec_type = speed_config->fec;
    phy_speed_config.linkTraining = speed_config->link_training;

    sig_mode = PM8X100_DPLL_PHY_SIGNALLING_MODE_GET(speed_config->speed,
                                          speed_config->num_lanes);
    if (PM_PORT_USER_F_DEFAULT_LANE_FW_CONFIG_GET(speed_config->flags)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (phymod_phy_lane_config_default_get(phy_acc, sig_mode,
                                            &phy_speed_config.pmd_lane_config));
    } else {
        sal_memcpy(&phy_speed_config.pmd_lane_config,
                   &(speed_config->lane_config),
                   sizeof(phymod_firmware_lane_config_t));
        /* Set signalling mode in pmd_lane_config. */
        if (sig_mode == phymodSignallingMethodPAM4) {
            phy_speed_config.pmd_lane_config.ForceNRZMode = 0;
            phy_speed_config.pmd_lane_config.ForcePAM4Mode = 1;
        } else {
            phy_speed_config.pmd_lane_config.ForceNRZMode = 1;
            phy_speed_config.pmd_lane_config.ForcePAM4Mode = 0;
            phy_speed_config.pmd_lane_config.ForceNormalReach = 0;
            phy_speed_config.pmd_lane_config.ForceExtenedReach = 0;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x100_dpll_port_mode_update(unit, pm_acc, op_st, pm_info,
                                  speed_config->speed));

    SHR_IF_ERR_EXIT
        (phymod_phy_speed_config_set(phy_acc, &phy_speed_config, &old_pll_state,
                                     &new_pll_state));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x100_common_port_encap_get(unit, pm_acc, op_st, pm_info, &encap));
    /*
     * For Higig3 port, need to program codec mode to
     * ReducedIPG mode after speed change.
     */
    if (encap == BCMPMAC_ENCAP_HG3) {
        SHR_IF_ERR_VERBOSE_EXIT
            (phymod_phy_codec_mode_set(phy_acc, phymodCodecReducedIPG));
    }

    /*
     * Get current VCOs.
     * cur_vcos[0] is TVCO, cur_vcos[1] is OVCO.
     */
    SHR_IF_ERR_VERBOSE_EXIT
         (pm8x100_dpll_pm_vcos_get(unit, pm_acc, op_st, pm_info, cur_vcos));

    /* Get requested VCO */
    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x100_dpll_speed_to_vco_get(unit, op_st, pm_info, speed_config, &req_vco));

    /* Set TVCO/OVCO bitmap here. */
    if ((cur_vcos[0] != PM_VCO_INVALID) && (cur_vcos[0] == req_vco)) {
        pm_ha_info->tvco_active_lane_bmap |=
                                 pm_acc->phy_acc.access.lane_mask;
        pm_ha_info->ovco_active_lane_bmap &=
                                 ~(pm_acc->phy_acc.access.lane_mask);
    }
    if ((cur_vcos[1] != PM_VCO_INVALID) && (cur_vcos[1] == req_vco)) {
        pm_ha_info->ovco_active_lane_bmap |=
                                  pm_acc->phy_acc.access.lane_mask;
        pm_ha_info->tvco_active_lane_bmap &=
                                 ~(pm_acc->phy_acc.access.lane_mask);
    }

    pm_ha_info->original_fec[port_index] = speed_config->fec;
    for (i = 0; i < port_num_lanes; i++) {
        pm_ha_info->lane_fec_map[port_index + i] = speed_config->fec;
    }

    /* next check if OVCO can be powered down */
    if ( (pm_ha_info->ovco_active_lane_bmap == 0) &&
         ((pm_ha_info->ovco_advert_lane_bmap &
           pm_ha_info->an_enable_lane_bmap) == 0)) {
        int ovco_pll_index = 0;
        ovco_pll_index = (phy_acc->access.tvco_pll_index == 1)? 0: 1;
        if ( phy_acc->type == phymodDispatchTypeNull ) {
            return SHR_E_NONE;
        }
        SHR_IF_ERR_VERBOSE_EXIT
                  (phymod_phy_pll_pwrdn(phy_acc,
                                        ovco_pll_index,
                                        1));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *   pm8x100_dpll_port_advert_abilities_validate
 * Purpose:
 *   Validate AN advertisement based on:
 *      1. PM support list.
 *      2. Protocol limitation.
 *      3. VCO limitation.
 *      4. FEC arch restriction.
 *      5. Encap support.
 *   If validation passes, return requested
 *   an_mode, FEC arch and VCO.
 */
static int
pm8x100_dpll_port_advert_abilities_validate(int unit, const pm_access_t *pm_acc,
                                           pm_oper_status_t *op_st,
                                           pm_info_t *pm_info,
                                           int num_abilities,
                                           pm_port_ability_t *abilities,
                                           pm_port_autoneg_mode_t *an_mode,
                                           pm_port_fec_t *rs_fec_req,
                                           pm_vco_setting_t *vco_setting)
{
    int rv, idx, is_dual_50g, port_num_lanes;
    int rs544_req = 0, rs272_req = 0;
    int is_bam = 0, is_msa = 0, port_is_valid = 0;
    uint8_t rs544_bm = 0, rs272_bm = 0, affected_bm = 0;
    pm_port_fec_t fec_cl73_25g, fec_cl73bam_msa;
    pm_port_medium_t current_media_type;
    const phymod_phy_access_t *phy_access;
    phymod_firmware_lane_config_t fw_lane_config;
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    pm_oper_status_type_t status = PM_OPER_SUCCESS;
    int port_index, sister_port_index, port_id_valid = 0;
    pm_port_autoneg_mode_t sister_port_an_mode;
    pm_vco_t vco;
    uint32_t flag = 0;
    pm8x100_dpll_vcos_bmp_t vcos = 0;


    SHR_FUNC_ENTER(unit);

    if (num_abilities == 0) {
        return SHR_E_NONE;
    }

    fec_cl73_25g = PM_PORT_FEC_COUNT;
    fec_cl73bam_msa = PM_PORT_FEC_COUNT;

    phy_access = &(pm_acc->phy_acc);

    SHR_BITCOUNT_RANGE(&(phy_access->access.lane_mask), port_num_lanes, 0,
                       PM8X100_DPLL_NUM_LANES_PER_CORE);

    current_media_type = phymodFirmwareMediaTypeCount;
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_firmware_lane_config_get(phy_access, &fw_lane_config));

    if (fw_lane_config.MediaType == phymodFirmwareMediaTypePcbTraceBackPlane) {
        current_media_type = PM_PORT_MEDIUM_BACKPLANE;
    } else if (fw_lane_config.MediaType == phymodFirmwareMediaTypeCopperCable) {
        current_media_type = PM_PORT_MEDIUM_COPPER;
    }

    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);

    for (idx = 0; idx < num_abilities; idx++) {
        is_dual_50g = 0;

        /* 1. AN ability is supported by HW. */
        /* 1.1 AN ability is in the port's support list. */
        rv = pm8x100_dpll_autoneg_ability_to_vco_get(unit, pm_info,
                                                abilities[idx], &vco);
        if (SHR_FAILURE(rv)) {
            status = PM_ABILITY_ADVERT_CFG_INVALID;
            SHR_ERR_EXIT(rv);
        }

        /*
         * 1.2 Number of lane request in the abilies does not exceed the
         * number of physical lanes of the logical port.
         */
        if (abilities[idx].num_lanes > (uint32_t)port_num_lanes) {
            status = PM_ABILITY_ADVERT_CFG_INVALID;
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        if ((abilities[idx].fec_type == PM_PORT_FEC_RS_544) ||
            (abilities[idx].fec_type == PM_PORT_FEC_RS_544_2XN)) {
            rs544_req = 1;
        } else if ((abilities[idx].fec_type == PM_PORT_FEC_RS_272) ||
                   (abilities[idx].fec_type == PM_PORT_FEC_RS_272_2XN)) {
            rs272_req = 1;
        }

        /* 2. AN abilities do not conflict with each other. */
        /* 2.1 Pause, medium do not conflict among abilities. */
        if (abilities[idx].pause != abilities[0].pause) {
            status = PM_ABILITY_PAUSE_CONFLICT;
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
        if (abilities[idx].medium != current_media_type) {
            status = PM_ABILITY_MEDIA_TYPE_CONFLICT;
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
        /* 10G only support Backplane in AN. */
        if ((abilities[idx].speed == 10000) &&
            (abilities[idx].medium != PM_PORT_MEDIUM_BACKPLANE)) {
            status = PM_ABILITY_ADVERT_CFG_INVALID;
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        if ((abilities[idx].speed == 50000) &&
            (abilities[idx].num_lanes == 2) &&
            (abilities[idx].fec_type != PM_PORT_FEC_RS_544)) {
            /* Set flag indicating current ability is dual lane 50G. */
            is_dual_50g = 1;
        }

        /* 2.2 Check FEC conflicts for each AN mode. */
        switch (abilities[idx].an_mode) {
            case PM_PORT_AUTONEG_MODE_CL73_BAM:
                is_bam = 1;

                /*
                 * Speeds 25G and 50G-2lane share the same RS528 FEC
                 * bit in BAM. Need to verify if there's any conflict among
                 * these abilities.
                 */
                if ((abilities[idx].speed == 25000) || is_dual_50g) {
                    if ((fec_cl73bam_msa == PM_PORT_FEC_NONE) &&
                        (abilities[idx].fec_type != PM_PORT_FEC_NONE)) {
                        /*
                         * Previous abilities request FEC_NONE, while current
                         * ability request other FEC type.
                         */
                        status = PM_ABILITY_FEC_CONFLICT;
                        SHR_ERR_EXIT(SHR_E_FAIL);
                    } else if ((abilities[idx].fec_type == PM_PORT_FEC_NONE) &&
                               (fec_cl73bam_msa == PM_PORT_FEC_RS_528)) {
                        /*
                         * Current ability request FEC_NONE, while previous
                         * abilities request other FEC type.
                         */
                        status = PM_ABILITY_FEC_CONFLICT;
                        SHR_ERR_EXIT(SHR_E_FAIL);
                    } else {
                        /* No FEC conflicts. Record current FEC request. */
                        fec_cl73bam_msa = abilities[idx].fec_type;
                    }
                }
                break;
            case PM_PORT_AUTONEG_MODE_CL73_MSA:
                is_msa = 1;
                if ((abilities[idx].speed == 25000) || is_dual_50g) {
                    if ((fec_cl73bam_msa == PM_PORT_FEC_NONE) &&
                        (abilities[idx].fec_type != PM_PORT_FEC_NONE)) {
                        /*
                         * Previous abilities request FEC_NONE, while current
                         * ability request other FEC type.
                         */
                        status = PM_ABILITY_FEC_CONFLICT;
                        SHR_ERR_EXIT(SHR_E_FAIL);
                    } else if ((abilities[idx].fec_type == PM_PORT_FEC_NONE) &&
                               (fec_cl73bam_msa == PM_PORT_FEC_RS_528)) {
                        /*
                         * Current ability request FEC_NONE, while previous
                         * abilities request other FEC type.
                         */
                        status = PM_ABILITY_FEC_CONFLICT;
                        SHR_ERR_EXIT(SHR_E_FAIL);
                    } else {
                        /* No FEC conflicts. Record current FEC request. */
                        fec_cl73bam_msa = abilities[idx].fec_type;
                    }
                }
                break;
            case PM_PORT_AUTONEG_MODE_CL73:
                if (abilities[idx].speed == 25000) {
                    if ((abilities[idx].channel == PM_PORT_CHANNEL_SHORT) &&
                        (abilities[idx].fec_type == PM_PORT_FEC_RS_528)) {
                        /* Short channel does not support RS FEC. */
                        status = PM_ABILITY_ADVERT_CFG_INVALID;
                        SHR_ERR_EXIT(SHR_E_FAIL);
                    }

                    if ((fec_cl73_25g == PM_PORT_FEC_NONE) &&
                        (abilities[idx].fec_type != PM_PORT_FEC_NONE)) {
                        /*
                         * Previous 25G abilities request FEC_NONE while
                         * current ability request other FEC type.
                         */
                        status = PM_ABILITY_FEC_CONFLICT;
                        SHR_ERR_EXIT(SHR_E_FAIL);
                    } else if ((abilities[idx].fec_type == PM_PORT_FEC_NONE)
                               && (fec_cl73_25g == PM_PORT_FEC_RS_528)) {
                        /*
                         * Current ability request FEC_NONE while previous
                         * 25G abilities request other FEC type.
                         */
                        status = PM_ABILITY_FEC_CONFLICT;
                        SHR_ERR_EXIT(SHR_E_FAIL);
                    } else {
                        /* No FEC conflicts. Record current FEC request. */
                        fec_cl73_25g = abilities[idx].fec_type;
                    }
                }
                break;
            default:
                /* We should never fall into this case. */
                status = PM_ABILITY_ADVERT_CFG_INVALID;
                SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    /*
     * 2.3 Verify an_mode do not conflicts with each other.
     * CL73BAM and MSA can not be advertised at the same time.
     */
    if (is_bam && is_msa) {
        status = PM_ABILITY_AN_CONFLICT_CFG;
        SHR_ERR_EXIT(SHR_E_FAIL);
    } else if (is_bam) {
        *an_mode = PM_PORT_AUTONEG_MODE_CL73_BAM;
    } else if (is_msa) {
        *an_mode = PM_PORT_AUTONEG_MODE_CL73_MSA;
    } else {
        *an_mode = PM_PORT_AUTONEG_MODE_CL73;
    }

    /*
     * 2.4 Verify an_mode that CL73BAM and MSA does not co-exist
     * on the same MPP.
     * Each MPP can at most support two single lane ports. So this check is only
     * needed for single lane port.
     */
    if ((port_num_lanes == 1) && (is_bam || is_msa)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_mpp_sister_port_index_get(port_index,
                                           &sister_port_index));
        pm8x100_dpll_port_valid_check(pm_info,
                                      sister_port_index,
                                      &port_is_valid);
        if (port_id_valid) {
            pm8x100_dpll_port_an_mode_get(pm_info, sister_port_index,
                                     &sister_port_an_mode);
            if ((is_bam &&
                (sister_port_an_mode == PM_PORT_AUTONEG_MODE_CL73_MSA))
                ||
                (is_msa &&
                (sister_port_an_mode == PM_PORT_AUTONEG_MODE_CL73_BAM))) {
                status = PM_ABILITY_AN_CONFLICT_CFG;
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
        }
    }

    /* 3. AN abilities can be supported by current VCOs. */
    if (pm_ha_info->ovco_active_lane_bmap &
        ~phy_access->access.lane_mask) {
        flag = PM_VCO_VALIDATE_F_PLL_SWITCH_NOT_ALLOWED;
    } else {
        flag = PM_VCO_VALIDATE_F_ONE_PLL_SWITCH_ALLOWED;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x100_dpll_vcos_setting_validate(unit, pm_acc, op_st, pm_info, flag,
                                      vcos, vco_setting));

    /* 4. FEC arch can be supported within each MPP. */
    if (rs544_req || rs272_req) {
        pm8x100_dpll_fec_lanebitmap_get(pm_info, &rs544_bm, &rs272_bm);
        rs544_bm &= ~phy_access->access.lane_mask;
        rs272_bm &= ~phy_access->access.lane_mask;
        if (rs544_req) {
            rs544_bm |= phy_access->access.lane_mask;
        }
        if (rs272_req) {
            rs272_bm |= phy_access->access.lane_mask;
        }
        rv = (pm8x100_dpll_fec_validate(unit, phy_access->access.lane_mask, pm_info,
                                   rs544_bm, rs272_bm, &affected_bm));
        if (SHR_FAILURE(rv)) {
            status = PM_ABILITY_FEC_CONFLICT;
            SHR_ERR_EXIT(rv);
        }
        if (rs544_req) {
            *rs_fec_req = PM_PORT_FEC_RS_544;
        } else if (rs272_req) {
            *rs_fec_req = PM_PORT_FEC_RS_272;
        }
    } else {
        *rs_fec_req = PM_PORT_FEC_NONE;
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, status);
    }
    SHR_FUNC_EXIT();
}

/*
 * This function will:
 *   1. Validate input AN abilities.
 *   2. Set AN abilities in HW.
 *   3. Update DB.
 */
static int
pm8x100_dpll_port_ability_advert_set(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st,
                                    pm_info_t *pm_info, int num_abilities,
                                    pm_port_ability_t *ability)
{
    pm_port_autoneg_mode_t an_mode = PM_PORT_AUTONEG_MODE_NONE;
    phymod_autoneg_advert_ability_t
                                  autoneg_abilities[PM8X100_DPLL_MAX_AN_ABILITY];
    phymod_autoneg_advert_abilities_t an_advert_abilities;
    const phymod_phy_access_t *phy_access;
    phymod_phy_pll_state_t old_pll_state, new_pll_state;
    int port_index, idx;
    pm_port_fec_t rs_fec_req = PM_PORT_FEC_NONE;
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    int pml_index;
    pm_encap_t port_encap;
    pm_vco_setting_t vco_setting;

    SHR_FUNC_ENTER(unit);

    sal_memset(autoneg_abilities, 0,
          PM8X100_DPLL_MAX_AN_ABILITY * sizeof(phymod_autoneg_advert_ability_t));
    sal_memset(&vco_setting, 0, sizeof(pm_vco_setting_t));

    /* 1. Validate abilities. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x100_dpll_port_advert_abilities_validate(unit, pm_acc, op_st,
                                                pm_info, num_abilities,
                                                ability, &an_mode,
                                                &rs_fec_req,
                                                &vco_setting));

    an_advert_abilities.autoneg_abilities = autoneg_abilities;
    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_ability_to_phy_ability_get(unit, num_abilities,
                                            ability, &an_advert_abilities));

    /* 2. Set AN abilities in HW. */
    phy_access = &(pm_acc->phy_acc);
    pml_index = PM8X100_DPLL_PML_INDEX_GET(phy_access->access.lane_mask);

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_autoneg_advert_ability_set(phy_access,
                                               &an_advert_abilities,
                                               &old_pll_state,
                                               &new_pll_state));

    /* 2.2 check if speed id table needs to be loaded or reloaded
     * need to check if load either IEEE or HG3
     * speed ID table based on the current port's encap mode.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x100_common_port_encap_get(unit, pm_acc, op_st,
                                       pm_info, &port_encap));
    if (port_encap == BCMPMAC_ENCAP_HG3) {
        /* The port's encap mode is HG3. */
        if (pm_ha_info->speed_id_table_status[pml_index] !=
            PM_SPEED_ID_TABLE_STATUS_HG3_MODE_LOADED) {
            /*
             * Reload SPEED ID table for HG3 mode.
             * Update table loading status in HA info.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (phymod_phy_autoneg_speed_id_table_reload(phy_access, 1));
            pm_ha_info->speed_id_table_status[pml_index] =
                                 PM_SPEED_ID_TABLE_STATUS_HG3_MODE_LOADED;
        }
    } else {
        /* The port's encap mode is IEEE. */
        if (pm_ha_info->speed_id_table_status[pml_index] !=
            PM_SPEED_ID_TABLE_STATUS_IEEE_MODE_LOADED) {
            /*
             * Reload SPEED ID table for IEEE mode.
             * Update table loading status in HA info.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (phymod_phy_autoneg_speed_id_table_reload(phy_access, 0));
            pm_ha_info->speed_id_table_status[pml_index] =
                                PM_SPEED_ID_TABLE_STATUS_IEEE_MODE_LOADED;
        }
    }

    /* 3. Update DB. */
    /* 3.1 Update an_mode. */
    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);
    pm_ha_info->an_mode[port_index] = an_mode;

    /* 3.2 Update FEC type. */
    for (idx = 0; idx < PM8X100_DPLL_NUM_LANES_PER_CORE; idx++) {
        if ((phy_access->access.lane_mask >> idx) & 0x1) {
            pm_ha_info->an_lane_fec_map[idx] = rs_fec_req;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_port_ability_advert_get(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st,
                                    pm_info_t *pm_info,
                                    uint32_t max_num_abilities,
                                    uint32_t *actual_num_abilities,
                                    pm_port_ability_t *ability)
{
    phymod_autoneg_advert_ability_t
                                 autoneg_abilities[PM8X100_DPLL_MAX_AN_ABILITY];
    phymod_autoneg_advert_abilities_t an_advert_abilities;
    const phymod_phy_access_t *phy_access;
    int idx;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < PM8X100_DPLL_MAX_AN_ABILITY; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (phymod_autoneg_advert_ability_t_init(&autoneg_abilities[idx]));
    }

    an_advert_abilities.autoneg_abilities = autoneg_abilities;
    phy_access = &(pm_acc->phy_acc);

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_autoneg_advert_ability_get(phy_access,
                                               &an_advert_abilities));

    if (an_advert_abilities.num_abilities > max_num_abilities) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_phy_ability_to_port_ability_get(unit, &an_advert_abilities,
                                          actual_num_abilities, ability));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_port_ability_remote_get(int unit, const pm_access_t *pm_acc,
                                   pm_oper_status_t *op_st,
                                   pm_info_t *pm_info,
                                   uint32_t max_num_abilities,
                                   uint32_t *actual_num_abilities,
                                   pm_port_ability_t *ability)
{
    phymod_autoneg_advert_ability_t
                                 autoneg_abilities[PM8X100_DPLL_MAX_AN_ABILITY];
    phymod_autoneg_advert_abilities_t an_advert_abilities;
    const phymod_phy_access_t *phy_access;
    int idx;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < PM8X100_DPLL_MAX_AN_ABILITY; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (phymod_autoneg_advert_ability_t_init(&autoneg_abilities[idx]));
    }

    an_advert_abilities.autoneg_abilities = autoneg_abilities;
    phy_access = &(pm_acc->phy_acc);

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_autoneg_remote_advert_ability_get(phy_access,
                                                      &an_advert_abilities));

    if (an_advert_abilities.num_abilities > max_num_abilities) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_phy_ability_to_port_ability_get(unit, &an_advert_abilities,
                                               actual_num_abilities, ability));
    }

exit:
    SHR_FUNC_EXIT();

}

static int
pm8x100_dpll_port_autoneg_set(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             phymod_autoneg_control_t *an)
{
    phymod_autoneg_control_t an_control;
    pm_access_t pm_acc_copy;
    phymod_phy_access_t *phy_access;
    pm_port_autoneg_mode_t an_mode;
    pm_port_ability_t abilities[PM8X100_DPLL_MAX_AN_ABILITY];
    int port_num_lanes, port_index;
    uint32_t idx, actual_num_abilities, an_base_speed = 1000;
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    uint32_t lane_mask_backup;
    uint8_t pll_lanes_bitmap, ovco_adv_lanes_bitmap;
    pm_oper_status_type_t status = PM_OPER_SUCCESS;
    bool rs544_req = false, rs272_req = false;
    int ovco_pll_index = 0;

    SHR_FUNC_ENTER(unit);

    an_control = *an;

    sal_memcpy(&pm_acc_copy, pm_acc, sizeof(pm_acc_copy));
    phy_access = &(pm_acc_copy.phy_acc);
    lane_mask_backup = phy_access->access.lane_mask;
    SHR_BITCOUNT_RANGE(&(phy_access->access.lane_mask),
                       port_num_lanes,
                       0,
                       PM8X100_DPLL_NUM_LANES_PER_CORE);
    an_control.num_lane_adv = port_num_lanes;

    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);
    an_mode = pm_ha_info->an_mode[port_index];
    if (an_mode == PM_PORT_AUTONEG_MODE_CL73) {
        an_control.an_mode = phymod_AN_MODE_CL73;
    } else if (an_mode == PM_PORT_AUTONEG_MODE_CL73_BAM) {
        an_control.an_mode = phymod_AN_MODE_CL73BAM;
    } else if (an_mode == PM_PORT_AUTONEG_MODE_CL73_MSA) {
        an_control.an_mode = phymod_AN_MODE_CL73_MSA;
    } else {
        if (!an_control.enable) {
            an_control.an_mode = phymod_AN_MODE_CL73;
        } else {
            status = PM_ABILITY_ADVERT_CFG_INVALID;
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    if (an_control.enable) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_port_ability_advert_get(unit, pm_acc, op_st, pm_info,
                                                 PM8X100_DPLL_MAX_AN_ABILITY,
                                                 &actual_num_abilities,
                                                 abilities));

        for (idx = 0; idx < actual_num_abilities; idx++) {
            /* Check FEC requests. */
            if ((abilities[idx].fec_type == PM_PORT_FEC_RS_544) ||
                (abilities[idx].fec_type == PM_PORT_FEC_RS_544_2XN)) {
                rs544_req = true;
            } else if ((abilities[idx].fec_type == PM_PORT_FEC_RS_272) ||
                       (abilities[idx].fec_type == PM_PORT_FEC_RS_272_2XN)) {
                rs272_req = true;
            }
        }

        if (rs544_req) {
            PHYMOD_AN_F_FEC_RS272_CLR_SET(&an_control);
        } else if (rs272_req) {
            PHYMOD_AN_F_FEC_RS272_REQ_SET(&an_control);
        }

        /* Update port mode in MAC. */
        phy_access->access.lane_mask = 1 << port_index;
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_port_mode_update(unit, &pm_acc_copy, op_st,
                                      pm_info, an_base_speed));
    }

    phy_access->access.lane_mask = lane_mask_backup;
    /* Enable AN in Serdes. */
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_autoneg_set(phy_access, &an_control));

    if (an_control.enable) {
        /* When AN is enabled on a port, always connect that port to TVCO. */
        pll_lanes_bitmap = pm_ha_info->ovco_active_lane_bmap;
        pll_lanes_bitmap &= ~phy_access->access.lane_mask;
        pm_ha_info->ovco_active_lane_bmap = pll_lanes_bitmap;
        ovco_pll_index = (phy_access->access.tvco_pll_index == 1)? 0: 1;

        /* Power down OVCO if OVCO becomes free. */
        if (!pll_lanes_bitmap) {
            pll_lanes_bitmap = pm_ha_info->an_enable_lane_bmap | phy_access->access.lane_mask;
            pll_lanes_bitmap &= pm_ha_info->ovco_advert_lane_bmap;
            if (!pll_lanes_bitmap) {
                SHR_IF_ERR_VERBOSE_EXIT
                          (phymod_phy_pll_pwrdn(phy_access,
                                                ovco_pll_index,
                                                1));
            }
        }

        pll_lanes_bitmap = pm_ha_info->tvco_active_lane_bmap;
        pll_lanes_bitmap |= phy_access->access.lane_mask;
        pm_ha_info->tvco_active_lane_bmap = pll_lanes_bitmap;
        pm_ha_info->an_enable_lane_bmap |= phy_access->access.lane_mask;
    } else {
        pm_ha_info->an_enable_lane_bmap &= ~phy_access->access.lane_mask;
        pm_ha_info->an_mode[port_index] = PM_PORT_AUTONEG_MODE_NONE;
        /* next echeck if OVCO can be power down */
        pll_lanes_bitmap = pm_ha_info->ovco_active_lane_bmap & (~pm_ha_info->an_enable_lane_bmap);
        ovco_adv_lanes_bitmap = pm_ha_info->ovco_advert_lane_bmap & pm_ha_info->an_enable_lane_bmap;
        if (!(pll_lanes_bitmap | ovco_adv_lanes_bitmap)) {
            ovco_pll_index = (phy_access->access.tvco_pll_index == 1)? 0: 1;
            SHR_IF_ERR_VERBOSE_EXIT
                      (phymod_phy_pll_pwrdn(phy_access,
                                            ovco_pll_index,
                                            1));
        }
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, status);
    }
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_port_autoneg_get(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             phymod_autoneg_control_t *an)
{
    const phymod_phy_access_t *phy_access;
    uint32_t an_done;

    SHR_FUNC_ENTER(unit);

    phy_access = &(pm_acc->phy_acc);

    SHR_IF_ERR_VERBOSE_EXIT(phymod_phy_autoneg_get(phy_access, an, &an_done));

exit:
    SHR_FUNC_EXIT();

}

static int
pm8x100_dpll_port_autoneg_status_get(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                                    phymod_autoneg_status_t *an_status)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_autoneg_status_get(phy_acc, an_status));

    if (!(an_status->enabled && an_status->locked)) {
        an_status->data_rate = 0;
        an_status->interface = 0;
    }

exit:
    SHR_FUNC_EXIT();

}

static int
pm8x100_dpll_port_link_status_get(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st, pm_info_t *pm_info,
                                 uint32_t *link_status)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT
        (phymod_phy_link_status_get(phy_acc, link_status));

exit:
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_port_pmd_lane_cfg_set(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st, pm_info_t *pm_info,
                                  phymod_firmware_lane_config_t *fw_lane_cfg)
{
    phymod_phy_access_t phy_acc;
    phymod_firmware_lane_config_t firmware_lane_cfg, current_fw_lane_cfg;

    SHR_FUNC_ENTER(unit);

    phymod_firmware_lane_config_t_init(&firmware_lane_cfg);
    phymod_firmware_lane_config_t_init(&current_fw_lane_cfg);
    PHYMOD_MEMCPY(&phy_acc, &(pm_acc->phy_acc), sizeof(phy_acc));
    PHYMOD_MEMCPY(&firmware_lane_cfg, fw_lane_cfg, sizeof(firmware_lane_cfg));

    PHY_IF_ERR_EXIT
        (phymod_phy_firmware_lane_config_get(&phy_acc, &current_fw_lane_cfg));

    /*
     * ForceNRZMode and ForcePAM4Mode are not exposed to user.
     * Keep these two field untouched.
     */
    if (current_fw_lane_cfg.ForceNRZMode) {
        firmware_lane_cfg.ForceNRZMode = 1;
        firmware_lane_cfg.ForcePAM4Mode = 0;
        firmware_lane_cfg.ForceNormalReach = 0;
        firmware_lane_cfg.ForceExtenedReach = 0;
    } else {
        firmware_lane_cfg.ForceNRZMode = 0;
        firmware_lane_cfg.ForcePAM4Mode = 1;
    }

    PHY_IF_ERR_EXIT
        (phymod_phy_firmware_lane_config_set(&phy_acc, firmware_lane_cfg));

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_PHY_OPER_ERROR);
    }

    SHR_FUNC_EXIT();
}

/*
 * Current configuation port abilities
 * It comprises force speed and autonegotiation abilities.
 * The force speed returns a list of abilities with the same number of lanes.
 * The autonegotiation returns a list of abilities with less than or the same
 * number of lanes.
 */
static int
pm8x100_dpll_port_ability_local_get(int unit, const pm_access_t *pm_acc,
                                   pm_oper_status_t *op_st, pm_info_t *pm_info,
                                   uint32_t max_num_abilities,
                                   uint32_t *actual_num_abilities,
                                   pm_port_ability_t *abilities)
{
    uint8_t idx = 0;
    int port_num_lanes = 0;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    const pm8x100_dpll_force_speed_ability_entry_t *fs_abil = NULL;
    const pm8x100_dpll_autoneg_ability_table_entry_t *an_abil = NULL;

    SHR_BITCOUNT_RANGE(&(phy_acc->access.lane_mask), port_num_lanes,
                       0, PM8X100_DPLL_NUM_LANES_PER_CORE);

    for (idx = 0; idx < COUNTOF(pm8x100_dpll_force_speed_ability_table); idx++) {
        fs_abil = &pm8x100_dpll_force_speed_ability_table[idx];
        if (fs_abil->num_lanes == (uint32_t)port_num_lanes) {
            if (*actual_num_abilities < max_num_abilities) {
                abilities[*actual_num_abilities].speed = fs_abil->speed;
                abilities[*actual_num_abilities].num_lanes = fs_abil->num_lanes;
                abilities[*actual_num_abilities].fec_type = fs_abil->fec;
                abilities[*actual_num_abilities].medium = PM_PORT_MEDIUM_ALL;
                abilities[*actual_num_abilities].pause = PM_PORT_PAUSE_ALL;
                abilities[*actual_num_abilities].channel = PM_PORT_CHANNEL_ALL;
                abilities[*actual_num_abilities].an_mode =
                                                     PM_PORT_AUTONEG_MODE_NONE;
                (*actual_num_abilities) += 1;
            } else {
                break;
            }
        }
    }

    for (idx = 0; idx < COUNTOF(pm8x100_dpll_autoneg_ability_table); idx++) {
        an_abil = &pm8x100_dpll_autoneg_ability_table[idx];
        if (an_abil->num_lanes <= (uint32_t)port_num_lanes) {
            if (*actual_num_abilities < max_num_abilities) {
                abilities[*actual_num_abilities].speed = an_abil->speed;
                abilities[*actual_num_abilities].num_lanes = an_abil->num_lanes;
                abilities[*actual_num_abilities].fec_type = an_abil->fec;
                abilities[*actual_num_abilities].medium = PM_PORT_MEDIUM_ALL;
                if (an_abil->speed == 10000) {
                    abilities[*actual_num_abilities].medium =
                                                       PM_PORT_MEDIUM_BACKPLANE;
                }
                abilities[*actual_num_abilities].pause = PM_PORT_PAUSE_ALL;
                abilities[*actual_num_abilities].channel = PM_PORT_CHANNEL_ALL;
                if ((an_abil->speed == 25000) &&
                    (an_abil->fec == PM_PORT_FEC_RS_528) &&
                    (an_abil->an_mode == PM_PORT_AUTONEG_MODE_CL73)) {
                    abilities[*actual_num_abilities].channel =
                                                           PM_PORT_CHANNEL_LONG;
                }
                abilities[*actual_num_abilities].an_mode = an_abil->an_mode;
                (*actual_num_abilities) += 1;
            } else {
                break;
            }
        }
    }

    return SHR_E_NONE;
}

static int
pm8x100_dpll_port_abilities_get(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st, pm_info_t *pm_info,
                               pm_port_ability_type_t ability_type,
                               uint32_t *actual_num_abilities,
                               pm_port_ability_t *pabilities)
{

    SHR_FUNC_ENTER(unit);

    if (ability_type == PM_PORT_ABILITY_LOCAL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_port_ability_local_get(unit, pm_acc, op_st, pm_info,
                                            PM8X100_DPLL_MAX_SPEED_ABILITY,
                                            actual_num_abilities, pabilities));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_port_ability_advert_get(unit, pm_acc, op_st, pm_info,
                                           PM8X100_DPLL_MAX_AN_ABILITY,
                                           actual_num_abilities, pabilities));
   }

exit:
    SHR_FUNC_EXIT();
}


static int
pm8x100_dpll_port_timesync_config_set(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                                    pm_phy_timesync_config_t *config)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    int port_index, port_ts_status, ts_is_enable, ts_enable_port_count;
    uint32_t is_one_step;
    uint32_t flags = 0;

    SHR_FUNC_ENTER(unit);

    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);
    port_ts_status = pm_ha_info->timesync_enable[port_index];
    ts_is_enable = PM_PORT_TIMESYNC_F_1588_ENABLE_GET(port_ts_status);
    ts_enable_port_count = pm_ha_info->ts_enable_port_count;

    /* Clear port timestamp status. */
    port_ts_status = 0;

    if (config->timesync_en) {
        /* Enable 1588. */
        if (ts_is_enable) {
            /* Clear current 1588 configs if the port has 1588 enabled. */
            PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);
            PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_SET(flags);
            SHR_IF_ERR_VERBOSE_EXIT
                (phymod_timesync_enable_set(phy_acc, flags, 0));
            flags = 0;
        } else {
            /* 1588 is not enabled on the port. */
            if (ts_enable_port_count == 0) {
                /* This is the first 1588 port on the PM. */
                PHYMOD_TIMESYNC_ENABLE_F_CORE_SET(flags);
            }
            ts_enable_port_count++;
        }
        if (config->is_one_step) {
            PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_SET(flags);
        }
        /* Rx enable will be done on link up as per programming requirement. */
        SHR_IF_ERR_VERBOSE_EXIT(phymod_timesync_enable_set(phy_acc, flags, 1));

        /* Update port timestamp status. */
        PM_PORT_TIMESYNC_F_1588_ENABLE_SET(port_ts_status);
        if (config->is_sop) {
            PM_PORT_TIMESYNC_F_SOP_TS_SET(port_ts_status);
        }
        if (config->comp_mode == PM_TIMESYNC_COMP_MODE_EARLIEST_LANE) {
            PM_PORT_TIMESYNC_F_COMP_MODE_EARLIEST_SET(port_ts_status);
        } else if (config->comp_mode == PM_TIMESYNC_COMP_LATEST_LANE) {
            PM_PORT_TIMESYNC_F_COMP_MODE_LATEST_SET(port_ts_status);
        }
    } else {
        /* Disable 1588. */
        if (ts_is_enable) {
            /* If 1588 is currently enabled on the port. */
            /* 1. Disable One Stpe pipeline if it is enabled. */
            PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_SET(flags);
            SHR_IF_ERR_VERBOSE_EXIT
                (phymod_timesync_enable_get(phy_acc, flags, &is_one_step));
            if (!is_one_step) {
                flags = 0;
            }
            /* 2. Disable RX timestamp. */
            PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);

            ts_enable_port_count--;
            /* 3. Disable FCLK if no port will using 1588 on the PM. */
            if (ts_enable_port_count == 0) {
                PHYMOD_TIMESYNC_ENABLE_F_CORE_SET(flags);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (phymod_timesync_enable_set(phy_acc, flags, 0));
        }
    }

    /* Update Timesync Enable Status in WB. */
    pm_ha_info->timesync_enable[port_index] = port_ts_status;
    pm_ha_info->ts_enable_port_count = ts_enable_port_count;

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_PHY_OPER_ERROR);
    }

    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_port_detach(int unit, const pm_access_t *pm_acc,
                        pm_oper_status_t *op_st, pm_info_t *pm_info)
{
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    int port_index, port_num_lanes, idx;
    pm_phy_timesync_config_t ts_config;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));
    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);

    SHR_BITCOUNT_RANGE(&(pm_acc->phy_acc.access.lane_mask), port_num_lanes,
                       0, PM8X100_DPLL_NUM_LANES_PER_CORE);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x100_common_port_enable_set(unit, pm_acc, op_st, pm_info,
                                   PM_PORT_ENABLE_PHY | PM_PORT_ENABLE_MAC, 0));

    /* Disable 1588 on the port. */
    ts_config.timesync_en = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x100_dpll_port_timesync_config_set(unit, pm_acc, op_st,
                                              pm_info, &ts_config));

    pm_ha_info->tvco_active_lane_bmap &= ~(pm_acc->phy_acc.access.lane_mask);
    pm_ha_info->ovco_active_lane_bmap &= ~(pm_acc->phy_acc.access.lane_mask);
    pm_ha_info->tvco_advert_lane_bmap &= ~(pm_acc->phy_acc.access.lane_mask);
    pm_ha_info->ovco_advert_lane_bmap &= ~(pm_acc->phy_acc.access.lane_mask);

    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);

    /* clear AN HA info */
    pm_ha_info->an_enable_lane_bmap &= ~(pm_acc->phy_acc.access.lane_mask);
    pm_ha_info->an_mode[port_index] = PM_PORT_AUTONEG_MODE_NONE;

    for (idx = 0; idx < port_num_lanes; idx++) {
        pm_ha_info->lane_fec_map[port_index + idx] = PM_PORT_FEC_NONE;
        pm_ha_info->an_lane_fec_map[port_index + idx] = PM_PORT_FEC_NONE;
    }
    /* Clearing RLM ha info to default */
    pm_ha_info->rlm_enable[port_index] = 0;

    /* need to check if OVCO can be power down  */
    if ( (pm_ha_info->ovco_active_lane_bmap == 0) &&
         ((pm_ha_info->ovco_advert_lane_bmap &
           pm_ha_info->an_enable_lane_bmap) == 0)) {
        int ovco_pll_index = 0;
        ovco_pll_index = (pm_acc->phy_acc.access.tvco_pll_index == 1)? 0: 1;
        SHR_IF_ERR_VERBOSE_EXIT
                  (phymod_phy_pll_pwrdn(&pm_acc->phy_acc,
                                        ovco_pll_index,
                                        1));
    }

    /* Power off PM if no port is configured on it. */
    if (!pm_ha_info->tvco_active_lane_bmap &&
        !pm_ha_info->ovco_active_lane_bmap &&
        !pm_ha_info->tvco_advert_lane_bmap &&
        !pm_ha_info->ovco_advert_lane_bmap) {
        /* Power off PM.  */
        SHR_IF_ERR_VERBOSE_EXIT
            (pmacd->tsc_ctrl_set(&pmac_acc, pm_acc->pmac_blkport, 0));
        /* Update PM init State. */
        pm_ha_info->is_pm_initialized = 0;
        /* Update firmware download state. */
        pm_ha_info->firmware_downloaded = 0;
        if (pm_info->pm_data.pmd_fw_info != NULL) {
            SHR_FREE(pm_info->pm_data.pmd_fw_info);
            pm_info->pm_data.pmd_fw_info = NULL;
        }
        /* Update SPEED ID table loading status. */
        pm_ha_info->speed_id_table_status[0] =
                                           PM_SPEED_ID_TABLE_STATUS_NOT_LOADED;
        pm_ha_info->speed_id_table_status[1] =
                                           PM_SPEED_ID_TABLE_STATUS_NOT_LOADED;
   }

exit:
    SHR_FUNC_EXIT();
}

/*
 * During link up event, we need to do:
 * 1. 1588 ports: timesync adjust.
 * 2. AN ports: Port mode update.
 *              Pause configuration.
 */
static int
pm8x100_dpll_port_phy_link_up_event(int unit, const pm_access_t *pm_acc,
                                   pm_oper_status_t *op_st, pm_info_t *pm_info)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    pm_access_t pm_acc_copy;
    int port_index;
    int port_ts_status, ts_is_enable, rx_pause, tx_pause;
    uint32_t idx, flags = 0;
    phymod_timesync_compensation_mode_t comp_mode;
    phymod_autoneg_status_t an_status;
    uint32_t num_advert, num_remote;
    pm_port_ability_t advert_ability[PM8X100_DPLL_MAX_AN_ABILITY];
    pm_port_ability_t remote_ability[PM8X100_DPLL_MAX_AN_ABILITY];
    pm_port_pause_t pause_local, pause_remote;
    pm_pause_ctrl_t pause_ctrl;
    pm_encap_t port_encap;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);
    /* 1. 1588 ports: Timesync adjust. */
    port_ts_status = pm_ha_info->timesync_enable[port_index];
    ts_is_enable = PM_PORT_TIMESYNC_F_1588_ENABLE_GET(port_ts_status);
    if (ts_is_enable) {
        /*
         * If 1588 is enabled on the port, need to adjust timesync during
         * link up event.
         * rx_ts_update will be enabled at the end of adjust function.
         */
        if (PM_PORT_TIMESYNC_F_COMP_MODE_EARLIEST_GET(port_ts_status)) {
            comp_mode = phymodTimesyncCompensationModeEarliestLane;
        } else if (PM_PORT_TIMESYNC_F_COMP_MODE_LATEST_GET(port_ts_status)) {
            comp_mode = phymodTimesyncCompensationModeLatestlane;
        } else {
            comp_mode = phymodTimesyncCompensationModeNone;
        }

        /* Set flags for timestamp adjust. */
        if (PM_PORT_TIMESYNC_F_SOP_TS_GET(port_ts_status)) {
            PHYMOD_TIMESYNC_F_SOP_SET(flags);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_common_port_encap_get(unit, pm_acc, op_st,
                                          pm_info, &port_encap));

        if (port_encap == BCMPMAC_ENCAP_HG3) {
            PHYMOD_TIMESYNC_F_REDUCED_PREAMBLE_MODE_SET(flags);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (phymod_timesync_adjust_set(phy_acc, flags, comp_mode));
        /* Set flags to enable RX timestamp. */
        flags = 0;
        PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);
        SHR_IF_ERR_VERBOSE_EXIT(phymod_timesync_enable_set(phy_acc, flags, 1));
        /* Wait for one AM spacing time. */
        sal_usleep(500);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x100_dpll_port_autoneg_status_get(unit, pm_acc, op_st,
                                             pm_info, &an_status));
    if ((an_status.enabled && an_status.locked)) {
        /* 2.1 AN ports: Port mode update. */
        sal_memcpy(&pm_acc_copy, pm_acc, sizeof(pm_acc_copy));
        pm_acc_copy.phy_acc.access.lane_mask = 0;
        /* Update lane mask based on resovled number of lanes. */
        for (idx = 0; idx < an_status.resolved_num_lane; idx++) {
            pm_acc_copy.phy_acc.access.lane_mask |= 0x1 << (port_index + idx);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_port_mode_update(unit, &pm_acc_copy, op_st,
                                      pm_info, an_status.data_rate));

        /* 2.2 AN ports: Pause update. */
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_port_ability_advert_get(unit, pm_acc, op_st, pm_info,
                                                 PM8X100_DPLL_MAX_AN_ABILITY,
                                                 &num_advert, advert_ability));
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_port_ability_remote_get(unit, pm_acc, op_st, pm_info,
                                                 PM8X100_DPLL_MAX_AN_ABILITY,
                                                 &num_remote, remote_ability));
        if (num_advert && num_remote) {
            pause_local = advert_ability[0].pause;
            pause_remote = remote_ability[0].pause;
            if (
                ((pause_local == PM_PORT_PAUSE_RX) &&
                 (pause_remote == PM_PORT_PAUSE_TX)) ||
                ((pause_local == PM_PORT_PAUSE_RX ||
                  pause_local == PM_PORT_PAUSE_SYMM) &&
                 (pause_remote == PM_PORT_PAUSE_RX ||
                  pause_remote == PM_PORT_PAUSE_SYMM))
               ) {
                rx_pause = 1;
            } else {
                rx_pause = 0;
            }
            if (
                ((pause_local == PM_PORT_PAUSE_RX ||
                  pause_local == PM_PORT_PAUSE_SYMM) &&
                 (pause_remote == PM_PORT_PAUSE_RX ||
                  pause_remote == PM_PORT_PAUSE_SYMM)) ||
                ((pause_local == PM_PORT_PAUSE_TX) &&
                 (pause_remote == PM_PORT_PAUSE_RX))
               ) {
                tx_pause = 1;
            } else {
                tx_pause = 0;
            }
        } else {
            tx_pause = 0;
            rx_pause = 0;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_common_port_pause_ctrl_get(unit, pm_acc, op_st, pm_info,
                                               &pause_ctrl));
        if ((pause_ctrl.rx_enable != rx_pause) ||
            (pause_ctrl.tx_enable != tx_pause)) {
            pause_ctrl.rx_enable = rx_pause;
            pause_ctrl.tx_enable = tx_pause;
            SHR_IF_ERR_VERBOSE_EXIT
                (pm8x100_common_port_pause_ctrl_set(unit, pm_acc, op_st, pm_info,
                                                   &pause_ctrl));
        }
    }

    /* Need to do the MAC link down recovery sequence. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->rx_enable_set(&pmac_acc, pm_acc->pmac_blkport, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->discard_set(&pmac_acc, pm_acc->pmac_blkport, 1));

    sal_usleep(10000);

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->rx_enable_set(&pmac_acc, pm_acc->pmac_blkport, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->discard_set(&pmac_acc, pm_acc->pmac_blkport, 0));

exit:
    SHR_FUNC_EXIT();
}

/*
 *During link down event, we need to do:
 * 1. 1588 ports: Disable RX timestamp.
 */
static int
pm8x100_dpll_port_phy_link_down_event(int unit, const pm_access_t *pm_acc,
                                     pm_oper_status_t *op_st,
                                     pm_info_t *pm_info)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    int port_index, port_ts_status, ts_is_enable;
    uint32_t flags = 0;

    SHR_FUNC_ENTER(unit);

    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);
    port_ts_status = pm_ha_info->timesync_enable[port_index];
    ts_is_enable = PM_PORT_TIMESYNC_F_1588_ENABLE_GET(port_ts_status);

    if (ts_is_enable) {
        /*
         * If link down and 1588 is enabled on the port,
         * Need to disable rx_ts_update.
         */
        PHYMOD_TIMESYNC_ENABLE_F_RX_SET(flags);
        SHR_IF_ERR_VERBOSE_EXIT(phymod_timesync_enable_set(phy_acc, flags, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_port_rlm_state_update(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st, pm_info_t *pm_info)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    phymod_phy_access_t phy_copy;
    uint32_t is_initiator, pcs_is_reconfigured;
    uint32_t active_lane_num, active_lane_map;
    phymod_fec_type_t FecType;
    uint32_t txLaneSwap, rxLaneSwap;
    uint32_t prbs_active_lane_num = 0, prbs_active_lane_map = 0x0;
    pm8x100_common_port_rlm_fsm_t local_port_rlm_state;
    int port_index = -1, mac_link = 1, num_lane = 0, i;
    uint32_t prbs_flag = 0x0;
    phymod_prbs_t prbs_config;
    phymod_prbs_status_t prbs_status;
    char *state_string = " \n";
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    sal_memcpy(&phy_copy, phy_acc, sizeof(phy_copy));

    /* Need to know if 4 lane or 8 lane port and also start lane of this port */
    SHR_BITCOUNT_RANGE(&(phy_acc->access.lane_mask), num_lane, 0,
                       PM8X100_DPLL_NUM_LANES_PER_CORE);
    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);

    /* First get all the RLM warmboot variable. */
    local_port_rlm_state = pm_ha_info->rlm_state[port_index];
    is_initiator = pm_ha_info->rlm_initiator[port_index];
    pcs_is_reconfigured = pm_ha_info->pcs_reconfigured[port_index];
    active_lane_map = pm_ha_info->rlm_active_lane_bmap[port_index];
    FecType = pm_ha_info->original_fec[port_index];
    txLaneSwap = pm_ha_info->rlm_original_tx_lane_swap;
    rxLaneSwap = pm_ha_info->rlm_original_rx_lane_swap;

    /* get the active lane number */
    active_lane_num = 0;
    for (i = 0; i < num_lane; i++) {
        if (active_lane_map &  1 << (port_index + i)) {
            active_lane_num++;
        }
    }

    /* first thing is to check MAC link status */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->port_link_status_get(&pmac_acc, pm_acc->pmac_blkport, 1,
                                     port_index % PM8X100_DPLL_NUM_LANES_PER_PML,
                                      &mac_link));

    /* if mac link status ever go low, directly go to error state */
     if (!mac_link)  {
        local_port_rlm_state = PM8X100_COMMON_PORT_RLM_ERROR;
    } else    {
        /* RLM state machine update */
        switch (local_port_rlm_state) {
        case PM8X100_COMMON_PORT_RLM_SEND_PACKET_DATA:
            state_string = "SEND_PACKET_DATA";
            /* first disable tx stall */
            SHR_IF_ERR_VERBOSE_EXIT
                (pmacd->stall_tx_enable_set(&pmac_acc,
                                            pm_acc->pmac_blkport, 0));

            /* first check initiator  set or not */
            if (is_initiator) {
                /* update the state */
                local_port_rlm_state = PM8X100_COMMON_PORT_RLM_STOP_MAC;
            } else { /* follower */
                /* first if link interruption status */
                uint32_t  li_status = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (pmacd->link_interrupt_live_status_get(&pmac_acc,
                                             pm_acc->pmac_blkport, &li_status));
                if (li_status) {
                    /* update the state */
                    local_port_rlm_state = PM8X100_COMMON_PORT_RLM_STOP_MAC;
                }
            }
            break;
        case PM8X100_COMMON_PORT_RLM_STOP_MAC:
            state_string = "STOP_MAC";
            /* clear the pcs reconfigured */
            pcs_is_reconfigured = FALSE;
            /* enable tx stall */
            SHR_IF_ERR_VERBOSE_EXIT
                (pmacd->stall_tx_enable_set(&pmac_acc,
                                            pm_acc->pmac_blkport, 1));
            /* update the state to SEND_LI*/
            local_port_rlm_state = PM8X100_COMMON_PORT_RLM_SEND_LI;
            break;
        case PM8X100_COMMON_PORT_RLM_SEND_LI:
        {
            uint32_t li_enable = 0, prbs_enabled = 0, li_status, pcs_link = 0;

            state_string = "SEND_LI";

            /* first check if LI is enabled or not */
            SHR_IF_ERR_VERBOSE_EXIT
                (pmacd->link_interrupt_ordered_get(&pmac_acc,
                                             pm_acc->pmac_blkport, &li_enable));
            if (!li_enable) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (pmacd->link_interrupt_ordered_set(&pmac_acc,
                                                 pm_acc->pmac_blkport, 1));
            }
            /*next check if initiator */
            if (is_initiator) {
                /* if initiator, then check pcs reconfigured status */
                if (pcs_is_reconfigured) {
                    /*next need to check prbs status */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (phymod_phy_prbs_enable_get(phy_acc, 0, &prbs_enabled));
                    if (prbs_enabled) {
                        /* disable prbs for both tx/rx */
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_prbs_enable_set(phy_acc, 0, 0));
                        /* disable the tx/disable and lane dp reset override */
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_pmd_override_enable_set(phy_acc,
                                                 phymodPMDLaneReset, 0, 0));
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_pmd_override_enable_set(phy_acc,
                                                 phymodPMDTxDisable, 0, 0));
                    }
                    /* next poll the pcs link status and LI status */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (pmacd->link_interrupt_live_status_get(&pmac_acc,
                                             pm_acc->pmac_blkport, &li_status));
                   /* read twice the pcs link status */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (pm8x100_dpll_port_link_status_get(unit, pm_acc, op_st,
                                                          pm_info, &pcs_link));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (pm8x100_dpll_port_link_status_get(unit, pm_acc, op_st,
                                                          pm_info, &pcs_link));
                    if (li_status && pcs_link) {
                        local_port_rlm_state = PM8X100_COMMON_PORT_RLM_SEND_IDLE;
                    }
                } else { /* pcs is not reconfigured */
                    /* need to check LI status */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (pmacd->link_interrupt_live_status_get(&pmac_acc,
                                             pm_acc->pmac_blkport, &li_status));
                    if (li_status) {
                        local_port_rlm_state = PM8X100_COMMON_PORT_RLM_SEND_PRBS;
                    }
                }
            } else { /*non initiator */
                if (pcs_is_reconfigured) {
                    /*next need to check prbs status */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (phymod_phy_prbs_enable_get(phy_acc, 0, &prbs_enabled));
                    if (prbs_enabled) {
                        /* disable prbs for both tx/rx */
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_prbs_enable_set(phy_acc, 0, 0));
                        /* disable the tx/disable and lane dp reset override */
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_pmd_override_enable_set(phy_acc,
                                                 phymodPMDLaneReset, 0, 0));
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_pmd_override_enable_set(phy_acc,
                                                 phymodPMDTxDisable, 0, 0));
                    }
                    /* next poll the pcs link status and LI status */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (pmacd->link_interrupt_live_status_get(&pmac_acc,
                                             pm_acc->pmac_blkport, &li_status));
                   /* read twice the pcs link status */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (pm8x100_dpll_port_link_status_get(unit, pm_acc, op_st,
                                                          pm_info, &pcs_link));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (pm8x100_dpll_port_link_status_get(unit, pm_acc, op_st,
                                                          pm_info, &pcs_link));
                    if (li_status && pcs_link) {
                        local_port_rlm_state = PM8X100_COMMON_PORT_RLM_SEND_IDLE;
                    }
                } else { /* pcs is not reconfigured yet */
                    uint32_t pmd_lock = 0;
                    prbs_flag = 0x0;
                    prbs_active_lane_num = 0;
                    prbs_active_lane_map = 0x0;

                    /*
                     * first set RX prbs only  and check if
                     * rx prbs is enabled or not,
                     */
                    PHYMOD_PRBS_DIRECTION_RX_SET(prbs_flag);
                    SHR_IF_ERR_VERBOSE_EXIT
                        (phymod_phy_prbs_enable_get(phy_acc, prbs_flag,
                                                    &prbs_enabled));
                    if (!prbs_enabled) {
                        prbs_config.poly = phymodPrbsPoly13;
                        prbs_config.invert = 0;
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_prbs_config_set(phy_acc, prbs_flag,
                                                        &prbs_config));
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_prbs_enable_set(phy_acc, prbs_flag, 1));
                    }

                    /*
                     * before check the prbs status, make sure signal
                     * detection are valid on all lanes.
                     */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (phymod_phy_rx_pmd_locked_get(phy_acc, &pmd_lock));
                    sal_usleep(100);
                    SHR_IF_ERR_VERBOSE_EXIT
                        (phymod_phy_rx_pmd_locked_get(phy_acc, &pmd_lock));

                    /* next check per lane prbs status only if PMD is locked */
                    if ((pmd_lock) && (!is_initiator)) {
                        /* next check per lane prbs status */
                        for (i = 0; i < num_lane; i ++) {
                            phy_copy.access.lane_mask = 1 << (i + port_index);
                            /*first read clear the status */
                            SHR_IF_ERR_VERBOSE_EXIT
                                (phymod_phy_prbs_status_get(&phy_copy, 0,
                                                            &prbs_status));

                            sal_usleep(100);

                            SHR_IF_ERR_VERBOSE_EXIT
                                (phymod_phy_prbs_status_get(&phy_copy, 0,
                                                            &prbs_status));
                            /* next check the lock status */
                            if (prbs_status.prbs_lock) {
                                prbs_active_lane_num++;
                                prbs_active_lane_map |= 1 << (i + port_index);
                            }
                        }
                        /* next check active lane PRBS status */
                        if (((num_lane == 4) && (prbs_active_lane_num == 3)) ||
                            ((num_lane == 4) && (prbs_active_lane_num == 4)) ||
                            ((num_lane == 8) && (prbs_active_lane_num == 8)) ||
                            ((num_lane == 8) && (prbs_active_lane_num == 6)) ||
                            ((num_lane == 8) && (prbs_active_lane_num == 7))) {

                            if (active_lane_map == prbs_active_lane_map) {
                                local_port_rlm_state =
                                              PM8X100_COMMON_PORT_RLM_SEND_PRBS;
                            } else {
                                active_lane_map = prbs_active_lane_map;
                            }
                        }
                    }
                } /* end of pcs not configured for follower */
            }
            break;
        } /* end of this  LI case */
        case PM8X100_COMMON_PORT_RLM_SEND_PRBS:
        {
            uint32_t first_active_lane_num = 0, prbs_enabled = 0;
            uint32_t need_reconfig_pcs = 0;
            int follower_prbs_resend = 0;
            prbs_flag = 0;

            state_string = "SEND_PRBS";

            /*first for follower check the prbs lane active map */
            if ((!is_initiator) && (!pcs_is_reconfigured)) {
                /* next check per lane prbs status */
                for (i = 0; i < num_lane; i ++) {
                    phy_copy.access.lane_mask = 1 << (i + port_index);
                    /*first read clear the status */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (phymod_phy_prbs_status_get(&phy_copy, 0,
                                                    &prbs_status));

                    sal_usleep(100);
                    /* next read the status */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (phymod_phy_prbs_status_get(&phy_copy, 0,
                                                    &prbs_status));
                    /*first read clear the status */
                    /* next check the lock status */
                    if (prbs_status.prbs_lock) {
                        prbs_active_lane_num++;
                        prbs_active_lane_map |= 1 << (i + port_index);
                    }
                }

                if ((prbs_active_lane_map != active_lane_map) &&
                    (prbs_active_lane_map != 0x0)) {
                    active_lane_map = prbs_active_lane_map;
                    follower_prbs_resend = 1;
                }
            }

            /* first set TX prbs only  and check if tx prbs is enabled or not*/
            /* only if the pcs is not reconfigured */
            if (!pcs_is_reconfigured) {
                for (i = 0; i < num_lane; i++) {
                    if (active_lane_map &  1 << (port_index + i)) {
                        first_active_lane_num = port_index + i;
                        break;
                    }
                }
                PHYMOD_PRBS_DIRECTION_TX_SET(prbs_flag);
                phy_copy.access.lane_mask = 1 << first_active_lane_num;
                SHR_IF_ERR_VERBOSE_EXIT
                    (phymod_phy_prbs_enable_get(&phy_copy, prbs_flag,
                                                &prbs_enabled));
                /* enable tx prbs if not enabled */
                prbs_config.invert = 0;

                if ((!prbs_enabled) || follower_prbs_resend)  {
                    for (i = 0; i < num_lane; i++) {
                        phy_copy.access.lane_mask = 1 << (port_index + i);
                        if (active_lane_map &  1 << (port_index + i)) {
                            /* if active lane, use prbspoly13 */
                            prbs_config.poly = phymodPrbsPoly13;
                        } else {
                            /* if inactive lane, use prbspoly31 */
                            prbs_config.poly = phymodPrbsPoly31;
                        }
                        /* enable prbs on a per lane basis */
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_prbs_config_set(&phy_copy, prbs_flag,
                                                        &prbs_config));
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_prbs_enable_set(&phy_copy,
                                                        prbs_flag, 1));
                        /* set tx_disable and pmd lane reset override enable */
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_pmd_override_enable_set(&phy_copy,
                                                     phymodPMDTxDisable, 1, 0));
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_pmd_override_enable_set(&phy_copy,
                                                     phymodPMDLaneReset, 1, 1));
                    }
                    /* next check if initiator  */
                    if (is_initiator) {
                        prbs_flag = 0;
                        prbs_config.invert = 0;
                        prbs_config.poly = phymodPrbsPoly13;
                        PHYMOD_PRBS_DIRECTION_RX_SET(prbs_flag);
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_prbs_config_set(phy_acc, prbs_flag,
                                                        &prbs_config));
                        SHR_IF_ERR_VERBOSE_EXIT
                            (phymod_phy_prbs_enable_set(phy_acc, prbs_flag, 1));
                    }
                }
            }

            /* next check prbs rx status if initiator */
            if ((is_initiator) && (!pcs_is_reconfigured)) {
                prbs_active_lane_num = 0;
                prbs_active_lane_map = 0x0;
                for (i = 0; i < num_lane; i++) {
                    phy_copy.access.lane_mask = 1 << (port_index + i);
                    /*first read clear the status */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (phymod_phy_prbs_status_get(&phy_copy, 0,
                                                    &prbs_status));
                    sal_usleep(500);
                    /* first read clear the status */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (phymod_phy_prbs_status_get(&phy_copy, 0,
                                                    &prbs_status));
                    /* next check the lock status */
                    if (prbs_status.prbs_lock) {
                        prbs_active_lane_num++;
                        prbs_active_lane_map |= 1 << (i + port_index);
                    }
                }
            }

            /* next check the prbs active lane map if initiator*/
            if ((is_initiator) && (prbs_active_lane_map == active_lane_map) &&
                (!pcs_is_reconfigured)) {
                need_reconfig_pcs = 1;
            } else if ((!is_initiator) && (!pcs_is_reconfigured) &&
                       (!follower_prbs_resend)) {
                need_reconfig_pcs = 1;
            }

            /*
             * check pcs needs to be re-configured if
             * it's no re-congiured already
             */
            if (need_reconfig_pcs) {
                phymod_fec_type_t fec_type;
                /* need to reconfigure the pcs */
                /* first disable pcs of the port */
                phy_copy.access.lane_mask = 1 << port_index;
                SHR_IF_ERR_VERBOSE_EXIT
                    (phymod_phy_pcs_enable_set(phy_acc, 0));
                /*set the rx lock and sig detection override */
                SHR_IF_ERR_VERBOSE_EXIT
                    (phymod_phy_pmd_override_enable_set(phy_acc,
                                                 phymodPMDRxLock, 1, 1));
                SHR_IF_ERR_VERBOSE_EXIT
                    (phymod_phy_pmd_override_enable_set(phy_acc,
                                                 phymodPMDSignalDetect, 1, 1));

                /* need to adjust the pcs lane swap */
                SHR_IF_ERR_VERBOSE_EXIT
                    (phymod_phy_pcs_lane_swap_adjust(phy_acc, active_lane_map,
                                                     txLaneSwap, rxLaneSwap));

                /*then disablethe rx lock and sig detection override */
                SHR_IF_ERR_VERBOSE_EXIT
                    (phymod_phy_pmd_override_enable_set(phy_acc,
                                                 phymodPMDRxLock, 0, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (phymod_phy_pmd_override_enable_set(phy_acc,
                                                 phymodPMDSignalDetect, 0, 0));

                /* then load the speed id based on the active lane num */
                if ((active_lane_num == 4) || (active_lane_num == 8)) {
                    fec_type = FecType;
                } else {
                    /* for all the reduced lane speed, only 544_2xn is valid */
                    fec_type = phymod_fec_RS544_2XN;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (phymod_phy_load_speed_id_entry(phy_acc,
                                                    50000 * active_lane_num,
                                                    active_lane_num, fec_type));

                /* next re-enable pcs */
                phy_copy.access.lane_mask = 1 << port_index;
                SHR_IF_ERR_VERBOSE_EXIT
                    (phymod_phy_pcs_enable_set(&phy_copy, 1));
                /* set the warm boot variable */
                pcs_is_reconfigured = TRUE;
            }

            if (is_initiator && need_reconfig_pcs) {
                local_port_rlm_state = PM8X100_COMMON_PORT_RLM_SEND_LI;
            } else if (!is_initiator) {
                /* for follower, need to check LI status */
                uint32_t li_status = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (pmacd->link_interrupt_live_status_get(&pmac_acc,
                                             pm_acc->pmac_blkport, &li_status));
                if (li_status) {
                    local_port_rlm_state = PM8X100_COMMON_PORT_RLM_SEND_LI;
                }
            }
            break;
        }
        case PM8X100_COMMON_PORT_RLM_SEND_IDLE:
        {
            uint32_t li_enable, li_status;
            bcmpmac_fault_status_t fault_status;

            state_string = "SEND_ILDE";

            /* first check if LI is enabled or not */
            SHR_IF_ERR_VERBOSE_EXIT
                (pmacd->link_interrupt_ordered_get(&pmac_acc,
                                                   pm_acc->pmac_blkport,
                                                   &li_enable));
            /* For initiator, disable LI if LI is enabled */
            if (li_enable && is_initiator) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (pmacd->link_interrupt_ordered_set(&pmac_acc,
                                                   pm_acc->pmac_blkport, 0));
            }
            /* next check idles are valid */
            SHR_IF_ERR_VERBOSE_EXIT
                (pmacd->link_interrupt_live_status_get(&pmac_acc,
                                                   pm_acc->pmac_blkport,
                                                   &li_status));

            SHR_IF_ERR_VERBOSE_EXIT
                (pmacd->fault_status_get(&pmac_acc, pm_acc->pmac_blkport,
                                         &fault_status));

            /* idle is valid only if none of the above three status are 0 */
            if (!li_status && !(fault_status.local_fault) &&
                !(fault_status.remote_fault)) {
                /* For follower, disable LI only if idle is valid. */
                if (!is_initiator && li_enable) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (pmacd->link_interrupt_ordered_set(&pmac_acc,
                                                      pm_acc->pmac_blkport, 0));
                }
                local_port_rlm_state = PM8X100_COMMON_PORT_RLM_DONE;
                /* clear tx stall */
                SHR_IF_ERR_VERBOSE_EXIT
                    (pmacd->stall_tx_enable_set(&pmac_acc,
                                                pm_acc->pmac_blkport, 0));
            }
            break;
        }
        case PM8X100_COMMON_PORT_RLM_ERROR:
            state_string = "ERROR";
            break;
        case PM8X100_COMMON_PORT_RLM_DONE:
            state_string = "DONE";
            /* do nothing */
            break;
        default:
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "RLM state not supported \n")));
            break;
        }
    }

    /* finally restore all the Warm boot variables */
    pm_ha_info->rlm_initiator[port_index] = is_initiator;
    pm_ha_info->pcs_reconfigured[port_index] = pcs_is_reconfigured;
    pm_ha_info->rlm_active_lane_bmap[port_index] = active_lane_map;
    pm_ha_info->rlm_state[port_index] = local_port_rlm_state;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "port %d RLM state is %-23s \n"),
                 pm_acc->pport, state_string));

exit:
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_port_sw_state_update(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st, pm_info_t *pm_info)
{
    int rlm_enabled, port_index = -1;
    pm8x100_common_port_rlm_fsm_t local_port_rlm_state;
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;

    SHR_FUNC_ENTER(unit);

    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);

    /* first get all the RLM warmboot variable */
    rlm_enabled = pm_ha_info->rlm_enable[port_index];
    local_port_rlm_state = pm_ha_info->rlm_state[port_index];

   /* check if RLM state machine needs to be updated */
   if (rlm_enabled == 1) {
        /*next check only state is not done or error */
        if ((local_port_rlm_state != PM8X100_COMMON_PORT_RLM_DONE) &&
            (local_port_rlm_state != PM8X100_COMMON_PORT_RLM_ERROR)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (pm8x100_dpll_port_rlm_state_update(unit, pm_acc,
                                                   op_st, pm_info));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_port_rlm_config_set(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
                                pm_port_rlm_config_t *rlm_config)
{
    const phymod_phy_access_t *phy_access = &(pm_acc->phy_acc);
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    uint32_t  is_initiator, pcs_is_reconfigured, active_lane_map_local;
    pm8x100_common_port_rlm_fsm_t local_port_rlm_state;
    uint32_t txLaneSwap, rxLaneSwap;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    int port_index, num_lane = 0;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);

    SHR_BITCOUNT_RANGE(&(phy_access->access.lane_mask),
                       num_lane,
                       0,
                       PM8X100_DPLL_NUM_LANES_PER_CORE);

    if (rlm_config->rlm_enable) {
        is_initiator = rlm_config->is_initiator;
        /* set the RLM variable and state accordingly */
        pcs_is_reconfigured = FALSE;
        /* next check if initiator is set */
        /*if so need to get the active_lane_map */
        if (is_initiator) {
            active_lane_map_local = rlm_config->active_lane_bit_map;
        } else {
            active_lane_map_local = 0x0;
        }
        local_port_rlm_state = PM8X100_COMMON_PORT_RLM_SEND_PACKET_DATA;
    } else {
        uint32_t is_400g_port = 0, tmp_active_lane_map = 0x0f;
        pm_speed_config_t  local_speed_config;

        /* get original rx/tx lane swap */
        txLaneSwap = pm_ha_info->rlm_original_tx_lane_swap;
        rxLaneSwap = pm_ha_info->rlm_original_rx_lane_swap;

        /* RLM disable, then clear both initiator and active lane map */
        is_initiator = FALSE;
        active_lane_map_local = 0x0;
        /*next based on the port index, need to get mpp index */
        if (port_index == 4) {
            tmp_active_lane_map = 0xf0;
        } else if (num_lane  == 8) {
            is_400g_port = 1;
            tmp_active_lane_map = 0xff;
        }
        /* next need to restore the tx/rx original lane swap */
        SHR_IF_ERR_VERBOSE_EXIT(phymod_phy_pcs_lane_swap_adjust(phy_access,
                                                         tmp_active_lane_map,
                                                         txLaneSwap,
                                                         rxLaneSwap));

        /*then disable PRBS tx/rx  */
        SHR_IF_ERR_VERBOSE_EXIT(phymod_phy_prbs_enable_set(phy_access, 0, 0));

        /* next disable all the PMD override */
        SHR_IF_ERR_VERBOSE_EXIT
            (phymod_phy_pmd_override_enable_set(phy_access,
                                                phymodPMDRxLock, 0, 0));
        SHR_IF_ERR_VERBOSE_EXIT
            (phymod_phy_pmd_override_enable_set(phy_access,
                                                phymodPMDSignalDetect, 0, 0));
        SHR_IF_ERR_VERBOSE_EXIT
            (phymod_phy_pmd_override_enable_set(phy_access,
                                                phymodPMDTxDisable, 0, 0));
        SHR_IF_ERR_VERBOSE_EXIT
            (phymod_phy_pmd_override_enable_set(phy_access,
                                                phymodPMDLaneReset, 0, 0));

        /* next disable LI  */
        SHR_IF_ERR_VERBOSE_EXIT
            (pmacd->link_interrupt_ordered_set(&pmac_acc,
                                               pm_acc->pmac_blkport, 0));

        /* next read the current PMD lane config info */
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_common_port_speed_config_get(unit, pm_acc, op_st,
                                                 pm_info, &local_speed_config));

        if (is_400g_port) {
            local_speed_config.num_lanes = 8;
            local_speed_config.speed = 400000;
        } else {
            local_speed_config.num_lanes = 4;
            local_speed_config.speed = 200000;
        }

        /* then do a forced speed config to restore original speed/fec mode */
        local_speed_config.fec = pm_ha_info->original_fec[port_index];
        SHR_IF_ERR_VERBOSE_EXIT
            (pm8x100_dpll_port_speed_config_set(unit, pm_acc, op_st,
                                               pm_info, &local_speed_config));

        pcs_is_reconfigured = FALSE;
        active_lane_map_local = 0x0;
        local_port_rlm_state = PM8X100_COMMON_PORT_RLM_FSM_COUNT;
    }

    /* next set the HA info */
    pm_ha_info->rlm_enable[port_index] = rlm_config->rlm_enable;
    pm_ha_info->rlm_initiator[port_index] = is_initiator;
    pm_ha_info->pcs_reconfigured[port_index] = pcs_is_reconfigured;
    pm_ha_info->rlm_active_lane_bmap[port_index] = active_lane_map_local;
    pm_ha_info->rlm_state[port_index] = local_port_rlm_state;

exit:
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_port_rlm_status_get(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
                                pm_port_rlm_status_t *rlm_status)
{
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    int port_index;

    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);

    rlm_status->rlm_disabled = !pm_ha_info->rlm_enable[port_index];
    rlm_status->rlm_busy = FALSE;
    rlm_status->rlm_failed = FALSE;
    rlm_status->rlm_done = FALSE;

    if (pm_ha_info->rlm_state[port_index] == PM8X100_COMMON_PORT_RLM_ERROR) {
        rlm_status->rlm_failed = TRUE;
    } else if (pm_ha_info->rlm_state[port_index] ==
                                                 PM8X100_COMMON_PORT_RLM_DONE) {
        rlm_status->rlm_done = TRUE;
    } else if ((pm_ha_info->rlm_state[port_index] <
                                              PM8X100_COMMON_PORT_RLM_FSM_COUNT)
               && (pm_ha_info->rlm_enable[port_index])) {
        rlm_status->rlm_busy = TRUE;
    }

    return SHR_E_NONE;
}


static int
pm8x100_speed_config_validate(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st,
                                 pm_info_t *pm_info, pm_speed_config_t *spd_cfg)
{

    return SHR_E_NONE;
}

static int
pm8x100_dpll_pm_vco_calculate(int unit, pm_oper_status_t *op_st,
                             pm_info_t *pm_info, pm_vco_setting_t *vco_select)
{
    pm8x100_dpll_vcos_bmp_t vcos, vco_bmp;
    pm_vco_t current_tvco, current_ovco;
    uint8_t rs544_bm = 0, rs272_bm = 0, affected_bm = 0;
    pm_vco_t vco;
    int i, rv = 0;
    pm_oper_status_type_t status = PM_OPER_SUCCESS;

    SHR_FUNC_ENTER(unit);

    if (vco_select == NULL) {
         SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&vcos, 0, sizeof(vcos));
    for (i = 0; i < vco_select->num_speeds; i++) {
        vco = PM_VCO_INVALID;
        rv = pm8x100_dpll_speed_to_vco_get(unit, op_st, pm_info,
                                     &(vco_select->speed_config_list[i]), &vco);

        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }

        sal_memset(&vco_bmp, 0, sizeof(vco_bmp));
        SHR_IF_ERR_EXIT
            (pm8x100_dpll_required_vco_bitmap_get(unit, vco,
                                            &vco_bmp));
        vcos |= vco_bmp;

        /*
         * When only one port is configured on the PM, does not need to
         * validate FEC settings, port_starting_lane is set as NULL.
         */
        if (vco_select->port_starting_lane_list != NULL) {
            if ((vco_select->speed_config_list[i].fec ==
                                                      PM_PORT_FEC_RS_272) ||
                (vco_select->speed_config_list[i].fec ==
                                                      PM_PORT_FEC_RS_272_2XN)) {
                SHR_IF_ERR_EXIT
                    (pm8x100_dpll_lanebitmap_set(
                                  vco_select->port_starting_lane_list[i],
                                  vco_select->speed_config_list[i].num_lanes,
                                  &rs272_bm));
            } else if ((vco_select->speed_config_list[i].fec ==
                                                        PM_PORT_FEC_RS_544) ||
                   (vco_select->speed_config_list[i].fec ==
                                                     PM_PORT_FEC_RS_544_2XN)) {
                SHR_IF_ERR_EXIT
                    (pm8x100_dpll_lanebitmap_set(
                                  vco_select->port_starting_lane_list[i],
                                  vco_select->speed_config_list[i].num_lanes,
                                  &rs544_bm));
            }
        }
    }

    /* Validate FEC settings for RS544 and RS272 for both PML0 and PML1 */
    if (vco_select->port_starting_lane_list != NULL) {
        uint32_t lane_mask;
        /* first check PML0 */
        lane_mask = 0x1;
        SHR_IF_ERR_EXIT
            (pm8x100_dpll_fec_validate(unit, lane_mask, pm_info, rs544_bm,
                                 rs272_bm, &affected_bm));
        lane_mask = 0x10;
        SHR_IF_ERR_EXIT
            (pm8x100_dpll_fec_validate(unit, lane_mask, pm_info, rs544_bm,
                                 rs272_bm, &affected_bm));
    }

    current_tvco = PM_VCO_INVALID;
    current_ovco = PM_VCO_INVALID;

    /* Try to allocate required VCOs */
    SHR_IF_ERR_EXIT
        (pm8x100_dpll_vcos_request_allocate(unit, NULL, op_st, vcos,
                                      &current_tvco, &current_ovco));

    /* Set Output */
    vco_select->tvco = current_tvco;
    vco_select->ovco = current_ovco;
    vco_select->is_tvco_new = (current_tvco == PM_VCO_INVALID) ? 0 : 1;
    vco_select->is_ovco_new = (current_ovco == PM_VCO_INVALID) ? 0 : 1;

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, status);
    }
    SHR_FUNC_EXIT();
}

static int
pm8x100_dpll_port_default_lane_config_get(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                                    pm_speed_config_t *speed_config,
                                    phymod_firmware_lane_config_t *lane_config)
{
    phymod_phy_signalling_method_t sig_mode = phymodSignallingMethodNRZ;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    int port_index;

    SHR_FUNC_ENTER(unit);

    pm8x100_dpll_port_index_get(pm_acc, pm_info, &port_index);

    sig_mode = PM8X100_DPLL_PHY_SIGNALLING_MODE_GET(speed_config->speed,
                                               speed_config->num_lanes);
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_lane_config_default_get(phy_acc, sig_mode,
                                            lane_config));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions.
 */

bcmpc_pm_drv_t bcmpc_pm_drv_pm8x100_dpll = {
    
    .speed_config_validate = pm8x100_speed_config_validate,
    .core_attach = pm8x100_dpll_core_attach,
    .port_detach = pm8x100_dpll_port_detach,
    .pm_init = pm8x100_dpll_pm_init,
    .pm_destroy = pm8x100_common_pm_destroy,
    .port_add = pm8x100_dpll_port_add,
    .pm_port_speed_config_set = pm8x100_dpll_port_speed_config_set,
    .pm_port_speed_config_get = pm8x100_common_port_speed_config_get,
    .pm_port_config_validate = pm8x100_dpll_pm_port_config_validate,
    .pm_port_loopback_set = pm8x100_common_port_loopback_set,
    .pm_port_loopback_get = pm8x100_common_port_loopback_get,
    .pm_port_ability_advert_set = pm8x100_dpll_port_ability_advert_set,
    .pm_port_ability_advert_get = pm8x100_dpll_port_ability_advert_get,
    .pm_port_ability_remote_get = pm8x100_dpll_port_ability_remote_get,
    .pm_port_autoneg_set = pm8x100_dpll_port_autoneg_set,
    .pm_port_autoneg_get = pm8x100_dpll_port_autoneg_get,
    .pm_port_autoneg_status_get = pm8x100_dpll_port_autoneg_status_get,
    .pm_port_encap_set = pm8x100_common_port_encap_set,
    .pm_port_encap_get = pm8x100_common_port_encap_get,
    .pm_port_pause_ctrl_set = pm8x100_common_port_pause_ctrl_set,
    .pm_port_pause_ctrl_get = pm8x100_common_port_pause_ctrl_get,
    .pm_port_pause_addr_set = pm8x100_common_port_pause_addr_set,
    .pm_port_pause_addr_get = pm8x100_common_port_pause_addr_get,
    .pm_port_frame_max_set = pm8x100_common_port_frame_max_set,
    .pm_port_frame_max_get = pm8x100_common_port_frame_max_get,
    .pm_port_enable_set = pm8x100_common_port_enable_set,
    .pm_port_enable_get = pm8x100_common_port_enable_get,
    .pm_port_link_status_get = pm8x100_dpll_port_link_status_get,
    .pm_port_lag_failover_lpbk_get =
                                   pm8x100_common_port_lag_failover_loopback_get,
    .pm_port_tx_set = pm8x100_common_port_tx_set,
    .pm_port_tx_get = pm8x100_common_port_tx_get,
    .pm_port_pmd_lane_cfg_set = pm8x100_dpll_port_pmd_lane_cfg_set,
    .pm_port_pmd_lane_cfg_get = pm8x100_common_port_pmd_lane_cfg_get,
    .pm_port_failover_enable_set = pm8x100_common_port_failover_enable_set,
    .pm_port_failover_enable_get = pm8x100_common_port_failover_enable_get,
    .pm_port_link_training_set = pm8x100_common_port_link_training_set,
    .pm_port_link_training_get = pm8x100_common_port_link_training_get,
    .pm_mac_avg_ipg_get = pm8x100_common_avg_ipg_get,
    .pm_mac_avg_ipg_set = pm8x100_common_avg_ipg_set,
    .pm_phy_status_get = pm8x100_common_phy_status_get,
    .pm_phy_control_set = pm8x100_common_phy_control_set,
    .pm_port_abilities_get = pm8x100_dpll_port_abilities_get,
    .pm_vco_get = pm8x100_dpll_pm_vcos_get,
    .pm_port_timesync_config_set = pm8x100_dpll_port_timesync_config_set,
    .pm_port_synce_clk_ctrl_set = pm8x100_common_port_synce_clk_ctrl_set,
    .pm_port_synce_clk_ctrl_get = pm8x100_common_port_synce_clk_ctrl_get,
    .pm_port_phy_link_up_event = pm8x100_dpll_port_phy_link_up_event,
    .pm_port_phy_link_down_event = pm8x100_dpll_port_phy_link_down_event,
    .pm_port_sw_state_update = pm8x100_dpll_port_sw_state_update,
    .pm_port_rlm_config_set = pm8x100_dpll_port_rlm_config_set,
    .pm_port_link_training_status_get =
                                    pm8x100_common_port_link_training_status_get,
    .pm_port_rlm_status_get = pm8x100_dpll_port_rlm_status_get,
    .pm_vco_rate_calculate = pm8x100_dpll_pm_vco_calculate,
    .pm_default_pmd_lane_config_get = pm8x100_dpll_port_default_lane_config_get,
    .pm_default_tx_taps_config_get =
                         pm8x100_common_port_default_tx_taps_config_get,
    .pm_phy_polarity_get = pm8x100_common_phy_polarity_get,
    .pm_core_lane_map_get = pm8x100_common_core_lane_map_get,
    .port_tx_timestamp_info_get = pm8x100_common_port_tx_timestamp_info_get,
    .pm_phy_rsfec_uncorrectable_counter_get = pm8x100_common_phy_rsfec_uncorrectable_counter_get,
    .pm_phy_rsfec_correctable_counter_get = pm8x100_common_phy_rsfec_correctable_counter_get,
    .pm_phy_rsfec_symbol_error_counter_get = pm8x100_common_phy_rsfec_symbol_error_counter_get
};
