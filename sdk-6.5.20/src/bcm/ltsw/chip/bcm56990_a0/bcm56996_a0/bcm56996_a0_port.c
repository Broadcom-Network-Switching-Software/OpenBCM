/*! \file bcm56996_a0_port.c
 *
 * BCM56996_A0 Port subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/port_int.h>
#include <bcm_int/ltsw/xgs/port.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/mbcm/port.h>

#include <shr/shr_debug.h>

#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_PORT

static const char* decap_key_mode[] = {
    OVID_DST_IPs,
    OVID_IPs,
};

static const ltsw_port_tab_sym_list_t decap_key_mode_map = {
    .len    = 2,
    .syms   = decap_key_mode,
};

static const char* vfi_key_mode[] = {
    OVID_VNIDs,
    OVID_VNID_SRC_IPs,
};

static const ltsw_port_tab_sym_list_t vfi_key_mode_map = {
    .len    = 2,
    .syms   = vfi_key_mode,
};

static const char* port_grp_mode[] = {
    ALL_ONESs,
    PORT_GRP_IDs,
};

static const ltsw_port_tab_sym_list_t port_grp_mode_map = {
    .len    = 2,
    .syms   = port_grp_mode,
};

static const ltsw_port_tab_lt_info_t port_system = {PORT_SYSTEMs, PORT_SYSTEM_IDs};
static const ltsw_port_tab_lt_info_t port = {PORTs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_lb = {PORT_LBs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_ing_ts_ptp = {PORT_ING_TS_PTPs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_policy = {PORT_POLICYs, PORT_IDs};

static const ltsw_port_tab_info_t port_tab_map_delta[BCMI_PT_CNT] = {
    [BCMI_PT_MPLS] = {&port_system, MPLSs, 0},
    [BCMI_PT_VXLAN_EN] = {&port_system, VXLAN_DECAPs, 0},
    [BCMI_PT_VXLAN_KEY_MODE] = {&port_system, VXLAN_DECAP_KEY_MODEs,
                                FLD_IS_SYMBOL, &decap_key_mode_map},
    [BCMI_PT_VXLAN_USE_PKT_OVID] = {&port_system, VXLAN_DECAP_USE_PKT_OVIDs, 0},
    [BCMI_PT_VXLAN_VFI_KEY_MODE] = {&port_system, VXLAN_VFI_ASSIGN_KEY_MODEs,
                                    FLD_IS_SYMBOL, &vfi_key_mode_map},
    [BCMI_PT_VXLAN_VFI_USE_PKT_OVID] = {&port_system, VXLAN_VFI_ASSIGN_USE_PKT_OVIDs, 0},
    [BCMI_PT_VXLAN_SVP_KEY_MODE] = {&port_system, VXLAN_SVP_ASSIGN_USE_PKT_OVIDs, 0},
    [BCMI_PT_VXLAN_SVP_DEFAULT_NETWORK] = {&port_system, ASSIGN_DEFAULT_NETWORK_SVPs, 0},

    [BCMI_PT_VFI_EGR_ADAPT_PORT_GRP_LOOKUP] = {&port, VFI_EGR_ADAPT_PORT_GRP_LOOKUPs, 0},
    [BCMI_PT_VFI_EGR_ADAPT_PORT_GRP_LOOKUP_MISS_DROP] = {&port, VFI_EGR_ADAPT_PORT_GRP_LOOKUP_MISS_DROPs, 0},
    [BCMI_PT_VXLAN_VFI_EGR_ADAPT_PORT_GRP_LOOKUP] = {&port, VXLAN_VFI_EGR_ADAPT_PORT_GRP_LOOKUPs, 0},
    [BCMI_PT_VXLAN_VFI_EGR_ADAPT_PORT_GRP_LOOKUP_MISS_DROP] = {&port, VXLAN_VFI_EGR_ADAPT_PORT_GRP_LOOKUP_MISS_DROPs, 0},

    [BCMI_PT_VFI_EGR_ADAPT_PORT_GRP] = {&port, VFI_EGR_ADAPT_PORT_GRPs, 0},
    [BCMI_PT_VFI_ING_ADAPT_PORT_GRP] = {&port_system, VFI_ING_ADAPT_PORT_GRPs, 0},

    [BCMI_PT_VFI_ING_ADAPT_FIRST_LOOKUP] = {&port_system, VFI_ING_ADAPT_FIRST_LOOKUPs, 0},
    [BCMI_PT_VFI_ING_ADAPT_FIRST_LOOKUP_PORT_GRP_MODE] = {&port_system, VFI_ING_ADAPT_FIRST_LOOKUP_PORT_GRP_MODEs,
                                                          FLD_IS_SYMBOL, &port_grp_mode_map},
    [BCMI_PT_VFI_ING_ADAPT_SECOND_LOOKUP] = {&port_system, VFI_ING_ADAPT_SECOND_LOOKUPs, 0},
    [BCMI_PT_VFI_ING_ADAPT_SECOND_LOOKUP_PORT_GRP_MODE] = {&port_system, VFI_ING_ADAPT_SECOND_LOOKUP_PORT_GRP_MODEs,
                                                           FLD_IS_SYMBOL, &port_grp_mode_map},
    [BCMI_PT_OFFSET_ECMP_RANDOM] = {&port_lb, OFFSET_ECMP_LEVEL1_RANDOMs },
    [BCMI_PT_OFFSET_ECMP_LVL2_RANDOM] = {&port_lb, OFFSET_ECMP_LEVEL2_RANDOMs },
    [BCMI_PT_1588_LINK_DELAY] = {&port_ing_ts_ptp, LINK_DELAYs},
    [BCMI_PT_PASS_ON_PAYLOAD_OUTER_TPID_MATCH] = {&port_policy, PASS_ON_PAYLOAD_OUTER_TPID_MATCHs, FLD_IS_ARRAY },
    [BCMI_PT_DEFAULT_VFI] = {&port_system, VFI_IDs},
    [BCMI_PT_EGR_OPAQUE_TAG] = {&port, EGR_OPAQUE_TAGs }
};

static const ltsw_port_tab_info_t null_info = {NULL, NULL, 0};

#define MAX_NUM_PORT        272
#define MAX_NUM_FP_PM       32
#define MAX_NUM_FP_PHY      256

#define BCM56996_A0_FDR_CW_S_ERR_MASK    (PC_FDR_CW_S0_ERR|PC_FDR_CW_S1_ERR|PC_FDR_CW_S2_ERR|    \
                                           PC_FDR_CW_S3_ERR|PC_FDR_CW_S4_ERR|PC_FDR_CW_S5_ERR|    \
                                           PC_FDR_CW_S6_ERR|PC_FDR_CW_S7_ERR)


#define BCM56996_A0_MAX_AN_ABILITY 20

/*! TH4G force speed abilities and their VCO rates. */
const ltsw_port_fs_ability_table_entry_t_t bcm56996_a0_fs_ability_table[] =
{
    /* port_speed, num_lanes, FEC type, VCO rate */
    {20000,     1,     bcmPortPhyFecNone,           ltswPortVCO41P25G},
    {40000,     2,     bcmPortPhyFecNone,           ltswPortVCO41P25G},
    {10000,     1,     bcmPortPhyFecNone,           ltswPortVCO51P562G},
    {25000,     1,     bcmPortPhyFecNone,           ltswPortVCO51P562G},
    {25000,     1,     bcmPortPhyFecRsFec,          ltswPortVCO51P562G},
    {40000,     4,     bcmPortPhyFecNone,           ltswPortVCO51P562G},
    {50000,     1,     bcmPortPhyFecRsFec,          ltswPortVCO51P562G},
    {50000,     2,     bcmPortPhyFecNone,           ltswPortVCO51P562G},
    {50000,     2,     bcmPortPhyFecRsFec,          ltswPortVCO51P562G},
    {100000,    2,     bcmPortPhyFecNone,           ltswPortVCO51P562G},
    {100000,    2,     bcmPortPhyFecRsFec,          ltswPortVCO51P562G},
    {100000,    4,     bcmPortPhyFecNone,           ltswPortVCO51P562G},
    {100000,    4,     bcmPortPhyFecRsFec,          ltswPortVCO51P562G},
    {200000,    4,     bcmPortPhyFecNone,           ltswPortVCO51P562G},
    {50000,     1,     bcmPortPhyFecRs544,          ltswPortVCO53P125G},
    {50000,     1,     bcmPortPhyFecRs272,          ltswPortVCO53P125G},
    {50000,     2,     bcmPortPhyFecRs544,          ltswPortVCO53P125G},
    {100000,    1,     bcmPortPhyFecRs544,          ltswPortVCO53P125G},
    {100000,    1,     bcmPortPhyFecRs544_2xN,      ltswPortVCO53P125G},
    {100000,    1,     bcmPortPhyFecRs272,          ltswPortVCO53P125G},
    {100000,    2,     bcmPortPhyFecRs544,          ltswPortVCO53P125G},
    {100000,    2,     bcmPortPhyFecRs272,          ltswPortVCO53P125G},
    {200000,    2,     bcmPortPhyFecRs544,          ltswPortVCO53P125G},
    {200000,    2,     bcmPortPhyFecRs544_2xN,      ltswPortVCO53P125G},
    {200000,    2,     bcmPortPhyFecRs272,          ltswPortVCO53P125G},
    {200000,    2,     bcmPortPhyFecRs272_2xN,      ltswPortVCO53P125G},
    {200000,    4,     bcmPortPhyFecRs544,          ltswPortVCO53P125G},
    {200000,    4,     bcmPortPhyFecRs544_2xN,      ltswPortVCO53P125G},
    {200000,    4,     bcmPortPhyFecRs272,          ltswPortVCO53P125G},
    {200000,    4,     bcmPortPhyFecRs272_2xN,      ltswPortVCO53P125G},
    {200000,    8,     bcmPortPhyFecRs544_2xN,      ltswPortVCO53P125G},
    {400000,    4,     bcmPortPhyFecRs544_2xN,      ltswPortVCO53P125G},
    {400000,    4,     bcmPortPhyFecRs272_2xN,      ltswPortVCO53P125G},
    {400000,    8,     bcmPortPhyFecRs544_2xN,      ltswPortVCO53P125G},
    {400000,    8,     bcmPortPhyFecRs272_2xN,      ltswPortVCO53P125G}
};


/*! A comprehensive list of TH4G AN abilities
 * and their VCO rates. */
const ltsw_port_an_ability_table_entry_t bcm56996_a0_an_ability_table[] =
{
    /*! Speed, number of lanes, FEC type, AN mode, VCO rate. */
    {20000,   1,    bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,   ltswPortVCO41P25G},
    {40000,   2,    bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,   ltswPortVCO41P25G},
    {10000,   1,    bcmPortPhyFecNone,      bcmPortAnModeCL73,      ltswPortVCO51P562G},
    {25000,   1,    bcmPortPhyFecNone,      bcmPortAnModeCL73,      ltswPortVCO51P562G},
    {25000,   1,    bcmPortPhyFecRsFec,     bcmPortAnModeCL73,      ltswPortVCO51P562G},
    {25000,   1,    bcmPortPhyFecNone,      bcmPortAnModeCL73MSA,   ltswPortVCO51P562G},
    {25000,   1,    bcmPortPhyFecRsFec,     bcmPortAnModeCL73MSA,   ltswPortVCO51P562G},
    {25000,   1,    bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,   ltswPortVCO51P562G},
    {25000,   1,    bcmPortPhyFecRsFec,     bcmPortAnModeCL73BAM,   ltswPortVCO51P562G},
    {40000,   4,    bcmPortPhyFecNone,      bcmPortAnModeCL73,      ltswPortVCO51P562G},
    {50000,   1,    bcmPortPhyFecRsFec,     bcmPortAnModeCL73BAM,   ltswPortVCO51P562G},
    {50000,   2,    bcmPortPhyFecNone,      bcmPortAnModeCL73MSA,   ltswPortVCO51P562G},
    {50000,   2,    bcmPortPhyFecRsFec,     bcmPortAnModeCL73MSA,   ltswPortVCO51P562G},
    {50000,   2,    bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,   ltswPortVCO51P562G},
    {50000,   2,    bcmPortPhyFecRsFec,     bcmPortAnModeCL73BAM,   ltswPortVCO51P562G},
    {100000,  2,    bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,   ltswPortVCO51P562G},
    {100000,  2,    bcmPortPhyFecRsFec,     bcmPortAnModeCL73BAM,   ltswPortVCO51P562G},
    {100000,  4,    bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,   ltswPortVCO51P562G},
    {100000,  4,    bcmPortPhyFecRsFec,     bcmPortAnModeCL73,      ltswPortVCO51P562G},
    {200000,  4,    bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,   ltswPortVCO51P562G},
    {50000,   1,    bcmPortPhyFecRs544,     bcmPortAnModeCL73,      ltswPortVCO53P125G},
    {50000,   1,    bcmPortPhyFecRs272,     bcmPortAnModeCL73MSA,   ltswPortVCO53P125G},
    {50000,   2,    bcmPortPhyFecRs544,     bcmPortAnModeCL73BAM,   ltswPortVCO53P125G},
    {100000,  1,    bcmPortPhyFecRs544_2xN, bcmPortAnModeCL73,      ltswPortVCO53P125G},
    {100000,  1,    bcmPortPhyFecRs544,     bcmPortAnModeCL73BAM,   ltswPortVCO53P125G},
    {100000,  2,    bcmPortPhyFecRs544,     bcmPortAnModeCL73,      ltswPortVCO53P125G},
    {100000,  2,    bcmPortPhyFecRs272,     bcmPortAnModeCL73MSA,   ltswPortVCO53P125G},
    {200000,  2,    bcmPortPhyFecRs544_2xN, bcmPortAnModeCL73,      ltswPortVCO53P125G},
    {200000,  4,    bcmPortPhyFecRs544_2xN, bcmPortAnModeCL73,      ltswPortVCO53P125G},
    {200000,  4,    bcmPortPhyFecRs544,     bcmPortAnModeCL73BAM,   ltswPortVCO53P125G},
    {200000,  4,    bcmPortPhyFecRs272_2xN, bcmPortAnModeCL73MSA,   ltswPortVCO53P125G},
    {400000,  4,    bcmPortPhyFecRs544_2xN, bcmPortAnModeCL73,      ltswPortVCO53P125G},
    {400000,  8,    bcmPortPhyFecRs544_2xN, bcmPortAnModeCL73MSA,   ltswPortVCO53P125G}
};


static const ltsw_port_map_t bcm56996_a0_fixed_port_map[] = {
    /* CPU */
    {0,     0, 0,   BCMI_LTSW_PORT_TYPE_CPU},
    /* LB */
    {33,    1, 259, BCMI_LTSW_PORT_TYPE_LB},
    {67,    3, 260, BCMI_LTSW_PORT_TYPE_LB},
    {101,   5, 261, BCMI_LTSW_PORT_TYPE_LB},
    {135,   7, 262, BCMI_LTSW_PORT_TYPE_LB},
    {169,   9, 263, BCMI_LTSW_PORT_TYPE_LB},
    {203,  11, 264, BCMI_LTSW_PORT_TYPE_LB},
    {237,  13, 265, BCMI_LTSW_PORT_TYPE_LB},
    {271,  15, 266, BCMI_LTSW_PORT_TYPE_LB},
    /* MGMT */
    {50,    2, 258, BCMI_LTSW_PORT_TYPE_MA | BCMI_LTSW_PORT_TYPE_XL, FLEXIBLE | NO_REMAP},
    {152,   8, 257, BCMI_LTSW_PORT_TYPE_MA | BCMI_LTSW_PORT_TYPE_XL, FLEXIBLE | NO_REMAP},
    /* spare */
    {84,    4, 267, BCMI_LTSW_PORT_TYPE_SPARE},
    {118,   6, 268, BCMI_LTSW_PORT_TYPE_SPARE},
    {186,   10,269, BCMI_LTSW_PORT_TYPE_SPARE},
    {220,   12,270, BCMI_LTSW_PORT_TYPE_SPARE},
    {254,   14,271, BCMI_LTSW_PORT_TYPE_SPARE},
    {0},
};

/******************************************************************************
 * Private functions
 */
static int
bcm56996_a0_ltsw_port_tab_info_update(int unit)
{
    int type;

    SHR_FUNC_ENTER(unit);

    for (type = 0; type < BCMI_PT_CNT; type ++) {
        if (port_tab_map_delta[type].table) {
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_ltsw_port_tab_info_update(unit, type, &port_tab_map_delta[type]));
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_lport_tab_info_update(unit, BCMI_PT_MPLS, &null_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check that given lane mask of a logical port belongs to which half of PM.
 * Only 2 logical ports per half of PM(4 lanes).
 *
 * \param [in] unit Unit number.
 * \param [in] lane_mask Lane mask.
 * \param [in] lport Logical port.
 * \param [in] pm_half_r_inc 1: logical port uses a part of lanes 0-3, 2: uses all of lanes 0-3.
 * \param [in] pm_half_l_inc 1: logical port uses a part of lanes 4-7, 2: uses all of lanes 4-7.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56996_a0_ltsw_flexport_half_pm_lane_num_get(int unit,
                                               uint32_t lane_mask,
                                               bcm_port_t lport,
                                               int *pm_half_r_inc,
                                               int *pm_half_l_inc)
{
    SHR_FUNC_ENTER(unit);

    *pm_half_r_inc = 0;
    *pm_half_l_inc = 0;
    if ((lane_mask & 0xf) && (lane_mask & 0xf0)) {
        if ((lane_mask & 0xff) != 0xff) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Invalid lane mask[0x%x] of lport[%d].\n"),
                       lane_mask, lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        *pm_half_r_inc = 2;
        *pm_half_l_inc = 2;
    } else if (lane_mask & 0xf) {
        if ((lane_mask & 0xf) == 0xf) {
            *pm_half_r_inc = 2;
        } else {
            *pm_half_r_inc = 1;
        }
    } else if (lane_mask & 0xf0) {
        if ((lane_mask & 0xf0) == 0xf0) {
            *pm_half_l_inc = 2;
        } else {
            *pm_half_l_inc = 1;
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Invalid lane mask[0x%x] of lport[%d].\n"),
                   lane_mask, lport));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Users can only flex to port configuration that have 4 logical ports per PM
 *  or 2 logical ports per half of PM (4 Serdes lanes).
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56996_a0_ltsw_flexport_lane_num_per_pm_validate(int unit,
                                                   ltsw_flexport_resource_t *resource)
{
    int i, j, lport_num;
    ltsw_flexport_port_cfg_t *po;
    int num_pm = ltsw_port_info[unit]->pm_num_max;
    ltsw_flexport_pm_cfg_t *pm;
    bcm_pbmp_t old_pbm, current_pbm;
    bcm_port_t lport;
    uint32_t lane_mask;
    int pm_half_l, pm_half_r, pm_r, pm_l;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < num_pm; i++) {
        pm = resource->pm + i;
        if (pm->valid == 0) {
            continue;
        }
        pm_half_l = 0;
        pm_half_r = 0;
        BCM_PBMP_ASSIGN(old_pbm, pm->pbm);
        BCM_PBMP_ITER(old_pbm, lport) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_lane_mask_get(unit, lport, &lane_mask));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_ltsw_flexport_half_pm_lane_num_get(unit,
                                                                lane_mask,
                                                                lport,
                                                                &pm_r,
                                                                &pm_l));
            pm_half_l += pm_l;
            pm_half_r += pm_r;
        }
        BCM_PBMP_ASSIGN(current_pbm, pm->pbm);
        for (j = 0; j < pm->num_op; j++) {
            po = pm->operation[j];
            lport = po->lport;
            if (po->op & FLEXPORT_OP_DETACH) {
                BCM_PBMP_PORT_REMOVE(current_pbm, lport);

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_port_lane_mask_get(unit, lport, &lane_mask));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcm56996_a0_ltsw_flexport_half_pm_lane_num_get(unit,
                                                                    lane_mask,
                                                                    lport,
                                                                    &pm_r,
                                                                    &pm_l));
                pm_half_l -= pm_l;
                pm_half_r -= pm_r;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_pport_lane_mask_get(unit, po->pport,
                                                     po->lanes, &lane_mask));
                if (BCM_PBMP_MEMBER(current_pbm, lport) == 0) {
                    BCM_PBMP_PORT_ADD(current_pbm, lport);
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcm56996_a0_ltsw_flexport_half_pm_lane_num_get(unit,
                                                                        lane_mask,
                                                                        lport,
                                                                        &pm_r,
                                                                        &pm_l));
                    pm_half_l += pm_l;
                    pm_half_r += pm_r;
                }
            }
        }
        BCM_PBMP_COUNT(current_pbm, lport_num);
        if (lport_num > 8) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "lport number in PM[%d]"
                                  " is larger than 8.\n"), i));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        if (pm_half_r > 4 || pm_half_l > 4) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "User can only flex to port configuration "
                                  "that 2 logical ports per half of PM[%d].\n"),
                       i));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        if (pm_half_r < 0 || pm_half_l < 0) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "User can not flex to port configuration "
                                  "that number of logical ports per half of PM[%d] < 0.\n"),
                       i));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_an_ability_table_vco_get(uint32 speed, uint32 num_lanes, bcm_port_phy_fec_t fec_type,
                                     bcm_port_autoneg_mode_t an_mode,
                                     ltsw_port_vco_type_t* vco)
{
    int i, entry_num;

    entry_num = sizeof(bcm56996_a0_an_ability_table) /sizeof(ltsw_port_an_ability_table_entry_t);

    *vco = ltswPortVCOInvalid;
    for (i = 0; i < entry_num; i++) {
        if (bcm56996_a0_an_ability_table[i].speed == speed &&
            bcm56996_a0_an_ability_table[i].num_lanes == num_lanes &&
            bcm56996_a0_an_ability_table[i].fec_type == fec_type &&
            bcm56996_a0_an_ability_table[i].an_mode == an_mode)
        {
            *vco = bcm56996_a0_an_ability_table[i].vco;
            break;
        }
    }

    return SHR_E_NONE;
}

int
bcm56996_a0_fs_ability_table_vco_get(uint32 speed, uint32 num_lanes, bcm_port_phy_fec_t fec_type,
                                     ltsw_port_vco_type_t* vco)
{
    int i, entry_num;

    entry_num = sizeof(bcm56996_a0_fs_ability_table)/sizeof(ltsw_port_fs_ability_table_entry_t_t);

    *vco = ltswPortVCOInvalid;
    for (i = 0; i < entry_num; i++) {
        if (bcm56996_a0_fs_ability_table[i].speed == speed &&
            bcm56996_a0_fs_ability_table[i].num_lanes == num_lanes &&
            bcm56996_a0_fs_ability_table[i].fec_type == fec_type)
        {
            *vco = bcm56996_a0_fs_ability_table[i].vco;
            break;
        }
    }

    return SHR_E_NONE;
}

static int
bcm56996_a0_manage_port_vco_get(int unit, int port,
                                const bcm_port_resource_t* speed_resource,
                                ltsw_port_vco_type_t* vco_rate)
{
    int custom_speed_vco;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_custom_speed_vco_get(unit, port, &custom_speed_vco));
    switch (speed_resource->speed) {
        case 10:
        case 100:
        case 1000:
            if (custom_speed_vco == ltswSpeed1gAt12p5g) {
                *vco_rate = ltswPortVCO12P5;
            } else {
                *vco_rate = ltswPortVCO10P3125;
            }
            break;
        case 2500:
            if (custom_speed_vco == ltswSpeed2p5gAt12p5g) {
                *vco_rate = ltswPortVCO12P5;
            } else {
                *vco_rate = ltswPortVCO10P3125;
            }
            break;
        case 10000:
            *vco_rate = ltswPortVCO10P3125;
            break;
        case 40000:
            *vco_rate = ltswPortVCO10P3125;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Validate if the new ports can be allocated in the PM.
 *
 * \param [in] unit           Unit number.
 * \param [in] current_ports  Current ports bmp in the PM.
 * \param [in] nport          Number of elements in array resource.
 * \param [in] resource       Port resource configuration array.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm56996_a0_ltsw_flexport_pm_validate(int unit, bcm_pbmp_t *current_ports, int nport, ltsw_flexport_port_cfg_t **port_cfg_array)
{
    uint32_t remain_ports_lane_bmp, new_ports_lane_bmp;
    bcm_pbmp_t remain_ports;
    bcm_port_t port;
    uint32_t lane_mask;
    uint32_t rs544_bmp, rs272_bmp;
    int i, j, an, an_ability_num;
    bcm_port_speed_ability_t an_ability[BCM56996_A0_MAX_AN_ABILITY];
    bcm_port_resource_t speed_resource;
    int current_tvco, current_ovco, pll_num, current_vco;
    uint32_t vco_lane_bmp;
    ltsw_port_vco_type_t vco;
    ltsw_port_vco_type_t required_vco = ltswPortVCOInvalid;
    int pm_id, pm_type;
    int vco_is_free = 0;
    int vco_41g_index = -1;
    uint32_t current_rs544_bmp = 0;
    uint32_t current_rs272_bmp = 0;
    bcm_pbmp_t reset_ports;

    SHR_FUNC_ENTER(unit);

    pm_id = port_cfg_array[0]->pmid;
    for (i = 0; i < nport; i++) {
        if (pm_id != port_cfg_array[i]->pmid) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_type_get(unit, pm_id, &pm_type));

    if (pm_type == ltswPmTypePm4x10) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "Management port doesn`t support flexport.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Get current tvco and ovco */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_vco_get(unit, pm_id, &pll_num, &current_tvco, &current_ovco));

    if (pll_num != 1) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "PLL get fail.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    }

    current_vco = current_tvco;

    BCM_PBMP_ASSIGN(remain_ports, *current_ports);
    remain_ports_lane_bmp = 0;
    new_ports_lane_bmp = 0;

    for (i = 0; i < nport; i++) {
        /*If op is FLEXPORT_OP_DETACH, the logic port will be removed*/
        if(port_cfg_array[i]->op & FLEXPORT_OP_DETACH) {
            /*Set up bmp for the remaining ports*/
            BCM_PBMP_PORT_REMOVE(remain_ports, port_cfg_array[i]->lport);
        } else {
            /*Set up the lane bmp for the new ports*/
            lane_mask = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_pport_lane_mask_get(unit, port_cfg_array[i]->pport,
                                                 port_cfg_array[i]->lanes, &lane_mask));
            new_ports_lane_bmp |= lane_mask;
        }
    }

    /*Set up the lane bmp for the remaining ports*/
    BCM_PBMP_ITER(remain_ports, port) {
       lane_mask = 0;
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmint_ltsw_port_lane_mask_get(unit, port, &lane_mask));
       remain_ports_lane_bmp |= lane_mask;
    }

    /*Validate lane overlap*/
    if ((remain_ports_lane_bmp & new_ports_lane_bmp) != 0) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "The new ports take over the lane of remaining ports overlap lane: 0x%x\n"),
                        remain_ports_lane_bmp & new_ports_lane_bmp));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /*Set up the rs544_bmp, rs272_bmp, tvco_lane_bmp*/
    rs544_bmp = 0;
    rs272_bmp = 0;
    vco_lane_bmp = 0;
    BCM_PBMP_ITER(remain_ports, port) {
        /*Check the AN enable*/
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_autoneg_get(unit, port, &an));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_lane_mask_get(unit, port, &lane_mask));
        /*Set up vco pbmp*/
        vco_lane_bmp |= lane_mask;
        if (an) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_autoneg_ability_advert_get(unit, port, BCM56996_A0_MAX_AN_ABILITY,
                                                          an_ability, &an_ability_num));
            for (i = 0; i < an_ability_num; i++) {
                /*Set up fec bmp*/
                if (an_ability[i].fec_type == bcmPortPhyFecRs272 ||
                    an_ability[i].fec_type == bcmPortPhyFecRs272_2xN) {
                    rs272_bmp |= lane_mask;
                }

                if (an_ability[i].fec_type == bcmPortPhyFecRs544 ||
                    an_ability[i].fec_type == bcmPortPhyFecRs544_2xN) {
                    rs544_bmp |= lane_mask;
                }
            }
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_resource_get(unit, port, &speed_resource));
            /*Set up fec bmp*/
            if (speed_resource.fec_type == bcmPortPhyFecRs272 ||
                speed_resource.fec_type == bcmPortPhyFecRs272_2xN) {
                rs272_bmp |= lane_mask;
            }

            if (speed_resource.fec_type == bcmPortPhyFecRs544 ||
                speed_resource.fec_type == bcmPortPhyFecRs544_2xN) {
                rs544_bmp |= lane_mask;
            }
        }
    }

    /*Add the lane mask of new ports to rs272_bmp and rs544_bmp*/
    for (i = 0 ; i < nport; i++) {
        if (!(port_cfg_array[i]->op & FLEXPORT_OP_DETACH)) {
            lane_mask = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_pport_lane_mask_get(unit, port_cfg_array[i]->pport,
                                                 port_cfg_array[i]->lanes, &lane_mask));

            if (port_cfg_array[i]->fec == bcmPortPhyFecRs272 ||
                port_cfg_array[i]->fec == bcmPortPhyFecRs272_2xN) {
                rs272_bmp |= lane_mask;
            }

            if (port_cfg_array[i]->fec == bcmPortPhyFecRs544 ||
                port_cfg_array[i]->fec == bcmPortPhyFecRs544_2xN) {
                rs544_bmp |= lane_mask;
            }
        }
    }

    /*FEC validation*/
    if ((rs544_bmp & 0x3) && (rs272_bmp & 0x3)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "Can not accommodate FEC settings on U0_MPP0.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if ((rs544_bmp & 0xc) && (rs272_bmp & 0xc)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "Can not accommodate FEC settings on U0_MPP1.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if ((rs544_bmp & 0x30) && (rs272_bmp & 0x30)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "Can not accommodate FEC settings on U1_MPP0.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if ((rs544_bmp & 0xc0) && (rs272_bmp & 0xc0)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "Can not accommodate FEC settings on U1_MPP1.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Set PC_RESET flag for FEC type change between RS272 and RS544.
     * RS272 and RS544 can't be accommodated in one MPP.
     * Delete the old ports which FEC type is RS272 and FEC type of other ports in one MPP are RS544, or
     * Old port FEC type is RS544 and FEC type of other port in one MPP are RS272.
     */
    BCM_PBMP_CLEAR(reset_ports);
    for (i = 0; i < nport; i++) {
        if (port_cfg_array[i]->op & FLEXPORT_OP_DETACH) {
            lane_mask = 0;
            port = port_cfg_array[i]->lport;
            /*Check the AN enable*/
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_autoneg_get(unit, port, &an));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_lane_mask_get(unit, port, &lane_mask));

            if (an) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_port_autoneg_ability_advert_get(unit, port, BCM56996_A0_MAX_AN_ABILITY,
                                                            an_ability, &an_ability_num));
                for (j = 0; j < an_ability_num; j++) {
                    /*Set up fec bmp*/
                    if (an_ability[j].fec_type == bcmPortPhyFecRs272 ||
                        an_ability[j].fec_type == bcmPortPhyFecRs272_2xN) {
                        current_rs272_bmp = lane_mask;
                    }

                    if (an_ability[j].fec_type == bcmPortPhyFecRs544 ||
                        an_ability[j].fec_type == bcmPortPhyFecRs544_2xN) {
                        current_rs544_bmp = lane_mask;
                    }
                }
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_port_resource_get(unit, port, &speed_resource));
                /*Set up fec bmp*/
                if (speed_resource.fec_type == bcmPortPhyFecRs272 ||
                    speed_resource.fec_type == bcmPortPhyFecRs272_2xN) {
                    current_rs272_bmp = lane_mask;
                }

                if (speed_resource.fec_type == bcmPortPhyFecRs544 ||
                    speed_resource.fec_type == bcmPortPhyFecRs544_2xN) {
                    current_rs544_bmp = lane_mask;
                }
            }

            if (((current_rs272_bmp & 0x3) && ((rs544_bmp & (~lane_mask)) & 0x3))||
                ((current_rs272_bmp & 0xc) && ((rs544_bmp & (~lane_mask)) & 0xc)) ||
                ((current_rs272_bmp & 0x30) && ((rs544_bmp & (~lane_mask)) & 0x30))||
                ((current_rs272_bmp & 0xc0) && ((rs544_bmp & (~lane_mask)) & 0xc0))||
                ((current_rs544_bmp & 0x3) && ((rs272_bmp & (~lane_mask)) & 0x3)) ||
                ((current_rs544_bmp & 0xc) && ((rs272_bmp & (~lane_mask)) & 0xc)) ||
                ((current_rs544_bmp & 0x30) && ((rs272_bmp & (~lane_mask)) & 0x30)) ||
                ((current_rs544_bmp & 0xc0) && ((rs272_bmp & (~lane_mask)) & 0xc0))) {
                if (port_cfg_array[i]->op & FLEXPORT_OP_FEC) {
                    port_cfg_array[i]->op |= FLEXPORT_OP_PC_RESET;
                    BCM_PBMP_PORT_ADD(reset_ports, port);
                }
            }
        } else {
            if (BCM_PBMP_MEMBER(reset_ports, port_cfg_array[i]->lport)) {
                if (port_cfg_array[i]->op & FLEXPORT_OP_FEC) {
                    port_cfg_array[i]->op |= FLEXPORT_OP_PC_RESET;
                }
            }
        }
    }

    /*VCO validation*/

    /*Current vco free status check*/
    if (vco_lane_bmp == 0) {
        vco_is_free = 1;
    }

    /* Point out the index of 40G (2-Lanes) or 20G (1-Lane) */
    for (i = 0; i < nport; i++) {
        if (!(port_cfg_array[i]->op & FLEXPORT_OP_DETACH)) {
            if ((port_cfg_array[i]->speed == 20000 && port_cfg_array[i]->lanes == 1)
                || (port_cfg_array[i]->speed == 40000 && port_cfg_array[i]->lanes == 2)) {
                if (vco_41g_index == -1) {
                    vco_41g_index = i;
                    break;
                }
            }
        }
    }

    for (i = 0; i < nport; i++) {
        /*If op is FLEXPORT_OP_DETACH, the logic port will be removed*/
        if(!(port_cfg_array[i]->op & FLEXPORT_OP_DETACH)) {
            /*Get the required vco*/
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fs_ability_table_vco_get(port_cfg_array[i]->speed, port_cfg_array[i]->lanes,
                                                      port_cfg_array[i]->fec, &vco));
            if (vco == ltswPortVCOInvalid) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit, "port %d: speed[%d] lane[%d] fec[%d]is not supported.\n"),
                        port_cfg_array[i]->lport, port_cfg_array[i]->speed, port_cfg_array[i]->lanes, port_cfg_array[i]->fec));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
            }

            if ((port_cfg_array[i]->speed == 10000 && port_cfg_array[i]->lanes == 1)
                || (port_cfg_array[i]->speed == 40000 && port_cfg_array[i]->lanes == 4)) {
                if (!vco_is_free && current_vco == ltswPortVCO41P25G) {
                    vco = ltswPortVCO41P25G;
                } else if (vco_41g_index != -1) {
                    vco = ltswPortVCO41P25G;
                } else {
                    vco = ltswPortVCO51P562G;
                }
            }

            if (required_vco == ltswPortVCOInvalid) {
                required_vco = vco;
            }

            if (required_vco != vco) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit, "required vco number is more than 1.\n")));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }
    }

    /* Required VCO is invalid means flexport opearation is delete */
    if (required_vco == ltswPortVCOInvalid) {
        SHR_EXIT();
    }

    if (vco_is_free) {
        /* Speical case: If insert 40G(2-Lanes) and 40G(4-Lanes)/10G(1-Lane) meanwhile,
        40G(2-Lanes) should be added first to make VCO work at 41.25G */
        if (vco_41g_index != -1) {
            port_cfg_array[vco_41g_index]->op |= FLEXPORT_OP_ATTACH_FIRST;
        }
        /* if current vco is free, it might change to another vco, so reset all port */
        for (i = 0; i < nport; i++) {
            if (!(port_cfg_array[i]->flags & FLEXPORT_FLG_NO_REMAP)) {
                port_cfg_array[i]->op |= FLEXPORT_OP_PC_RESET;
            }
        }
        SHR_EXIT();
    } else {
        if (required_vco != current_vco) {
            /*The required VCO can`t be accommodated, validation failed*/
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "Can not accommodate the request VCO.\n request [VCO %d].\n"),
                    required_vco));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        } else {
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate if the new speed can be allocated in the PM.
 *
 * \param [in] unit           Unit number.
 * \param [in] resource       Port resource configuration array.
 * \param [out]affect_ports   The affected ports bmp.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm56996_a0_ltsw_port_speed_config_validate(int unit, bcm_port_resource_t *resource, bcm_pbmp_t *affect_ports)
{
    bcm_pbmp_t port_pbmp;
    bcm_port_speed_ability_t an_ability[BCM56996_A0_MAX_AN_ABILITY];
    bcm_port_resource_t speed_resource;
    bcm_port_t port;
    ltsw_port_vco_type_t vco;
    int current_vco, ovco, pll_num, tvco;
    uint32_t rs544_bmp = 0, rs272_bmp = 0, lane_mask = 0;
    int i, pm_id, an, an_ability_num, pm_type;
    uint32_t num_lane;

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(port_pbmp);
    BCM_PBMP_CLEAR(*affect_ports);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_id_get(unit, resource->port, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_pm_ports_get(unit, pm_id, &port_pbmp));
    BCM_PBMP_PORT_REMOVE(port_pbmp, resource->port);

    /*lane check*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_num_lane_get(unit, resource->port, &num_lane));
    if (num_lane != resource->lanes) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "Invalid lane number request on exising logical port.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /*get the pm_type*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_type_get(unit, pm_id, &pm_type));
    /*get the current tvco and ovco*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_vco_get(unit, pm_id, &pll_num, &tvco, &ovco));
    if (pll_num > 1) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "PLL get fail.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    }

    current_vco = tvco;

    /*Validate the speed for the manegement port*/
    if (pm_type == ltswPmTypePm4x10) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56996_a0_manage_port_vco_get(unit, resource->port, resource, &vco));
        if (vco != current_vco) {
            /*Only resource->port in PM core, return SHR_E_NONE*/
            if (BCM_PBMP_IS_NULL(port_pbmp)) {
                SHR_EXIT();
            } else {
                LOG_ERROR(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit, "VCO change is required for management port.\n")));
                BCM_PBMP_ASSIGN(*affect_ports, port_pbmp);
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        } else {
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_lane_mask_get(unit, resource->port, &lane_mask));
    if (resource->fec_type ==bcmPortPhyFecRs272 ||
        resource->fec_type == bcmPortPhyFecRs272_2xN) {
        rs272_bmp |= lane_mask;
    }

    if (resource->fec_type ==bcmPortPhyFecRs544 ||
        resource->fec_type == bcmPortPhyFecRs544_2xN) {
        rs544_bmp |= lane_mask;
    }

    BCM_PBMP_ITER(port_pbmp, port) {
        /*Check the AN enable*/
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_autoneg_get(unit, port, &an));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_lane_mask_get(unit, port, &lane_mask));
        if (an) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_autoneg_ability_advert_get(unit, port, BCM56996_A0_MAX_AN_ABILITY,
                                                          an_ability, &an_ability_num));

            for (i = 0; i < an_ability_num; i++) {
                /*Set up fec bmp*/
                if (an_ability[i].fec_type == bcmPortPhyFecRs272 ||
                    an_ability[i].fec_type == bcmPortPhyFecRs272_2xN) {
                    rs272_bmp |= lane_mask;
                }

                if (an_ability[i].fec_type == bcmPortPhyFecRs544 ||
                    an_ability[i].fec_type == bcmPortPhyFecRs544_2xN) {
                    rs544_bmp |= lane_mask;
                }
            }
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_resource_get(unit, port, &speed_resource));
            /*Set up fec bmp*/
            if (speed_resource.fec_type == bcmPortPhyFecRs272 ||
                speed_resource.fec_type == bcmPortPhyFecRs272_2xN) {
                rs272_bmp |= lane_mask;
            }

            if (speed_resource.fec_type == bcmPortPhyFecRs544 ||
                speed_resource.fec_type == bcmPortPhyFecRs544_2xN) {
                rs544_bmp |= lane_mask;
            }
        }

        if ((lane_mask & 0x3) && (rs544_bmp & 0x3) && (rs272_bmp & 0x3)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "Can not accommodate FEC settings on U0_MPP0. Conflict with port %d\n"), port));
            BCM_PBMP_PORT_ADD(*affect_ports, port);
        }

        if ((lane_mask & 0xc) && (rs544_bmp & 0xc) && (rs272_bmp & 0xc)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "Can not accommodate FEC settings on U0_MPP1. Conflict with port %d\n"), port));
            BCM_PBMP_PORT_ADD(*affect_ports, port);
        }

        if ((lane_mask & 0x30) && (rs544_bmp & 0x30) && (rs272_bmp & 0x30)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "Can not accommodate FEC settings on U1_MPP0. Conflict with port %d\n"), port));
            BCM_PBMP_PORT_ADD(*affect_ports, port);
        }

        if ((lane_mask & 0xc0) && (rs544_bmp & 0xc0) && (rs272_bmp & 0xc0)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "Can not accommodate FEC settings on U1_MPP1. Conflict with port %d\n"), port));
            BCM_PBMP_PORT_ADD(*affect_ports, port);
        }
    }

    /*FEC check*/
    if(BCM_PBMP_NOT_NULL(*affect_ports)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /*VCO check*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_fs_ability_table_vco_get(resource->speed, resource->lanes,
                                              resource->fec_type, &vco));

    if (vco == ltswPortVCOInvalid) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "port %d: speed[%d] lane[%d] fec[%d]is not supported.\n"),
             resource->port, resource->speed,resource->lanes, resource->fec_type));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }

    if ((resource->speed == 10000 && resource->lanes == 1)
        || (resource->speed == 40000 && resource->lanes == 4)) {
        if (current_vco == ltswPortVCO41P25G) {
            vco = ltswPortVCO41P25G;
        }
    }

    if (vco == current_vco) {
        /*No VCO change is required*/
        SHR_EXIT();
    } else {
        /*Only resource->port in PM core, return SHR_E_NONE*/
        if (BCM_PBMP_IS_NULL(port_pbmp)) {
            SHR_EXIT();
        } else {
            /*Speed change is not allowed, return the affected ports*/
            BCM_PBMP_ASSIGN(*affect_ports, port_pbmp);
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "Can not accommodate the request VCO.\n request [VCO %d].\n"),vco));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ltsw_pm_port_rearrange(int unit, int nport,
                                   ltsw_flexport_port_cfg_t **port_cfg_array)
{
    int i;
    ltsw_flexport_port_cfg_t *po, *psrc, *pdst, tmp;

    SHR_FUNC_ENTER(unit);

    for (i = nport - 1, psrc = NULL; i >= 0; i--) {
        po = port_cfg_array[i];
        if (po->op & FLEXPORT_OP_DETACH) {
            break;
        }

        if (po->op & FLEXPORT_OP_ATTACH_FIRST) {
            psrc = po;
        }
    }

    if (psrc) {
        pdst = port_cfg_array[i+1];
        if (pdst != psrc) {
            sal_memcpy(&tmp, psrc, sizeof(tmp));
            sal_memcpy(psrc, pdst, sizeof(tmp));
            sal_memcpy(pdst, &tmp, sizeof(tmp));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Rearrange the sequence of operations.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56996_a0_ltsw_flexport_rearrange(int unit,
                                    ltsw_flexport_resource_t *resource)
{
    int i, num_pm = resource->num_pm;
    ltsw_flexport_pm_cfg_t *pm;

    SHR_FUNC_ENTER(unit);

    for (i = 0, pm = resource->pm; i < num_pm; i ++, pm ++) {
        if (!pm->valid) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56996_a0_ltsw_pm_port_rearrange(unit, pm->num_op, pm->operation));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check device has enough resource to complete all operations.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56996_a0_ltsw_flexport_resource_validate(int unit,
                                            ltsw_flexport_resource_t *resource)
{
    int i;
    ltsw_flexport_pm_cfg_t *pm;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ltsw_flexport_lane_num_per_pm_validate(unit, resource));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_flexport_linkscan_validate(unit, resource));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_flexport_port_mapping_validate(unit, resource));

    for (i = 0, pm = resource->pm; i <= MAX_NUM_FP_PM; i ++, pm ++) {
        if (pm->valid) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_ltsw_flexport_pm_validate
                    (unit, &pm->pbm, pm->num_op, pm->operation));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ltsw_flexport_rearrange(unit, resource));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init BCM56990 specific information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcm56996_a0_ltsw_port_info_init(int unit)
{
    int port, pport, pipe, pipe_num;
    ltsw_port_profile_info_t *pinfo;
    ltsw_pc_profile_info_t *pc_info;
    const ltsw_port_map_t *pm = NULL;
    bool mode;

    SHR_FUNC_ENTER(unit);

    for (port = 0; port < MAX_NUM_PORT; port ++) {
        PORT(unit, port).port_type = BCMI_LTSW_PORT_TYPE_CD;
        PORT(unit, port).flags = (FLEXIBLE | CT_VALID);
    }

    for (pport = 1; pport <= MAX_NUM_FP_PHY; pport ++) {
        ltsw_port_info[unit]->phy_port[pport].flags = FLEXIBLE;
    }

    for (pm = bcm56996_a0_fixed_port_map; pm->ptype; pm ++) {
        port = pm->lport;
        PORT(unit, port).pport = pm->pport;
        PORT(unit, port).port_type = pm->ptype;
        PORT(unit, port).flags = pm->flags;
        pport = pm->pport;
        if (pport >= 0) {
            ltsw_port_info[unit]->phy_port[pport].flags = pm->flags;
        }
    }
    /* 1 lane */
    ltsw_port_info[unit]->min_lanes_per_pport = 1;
    ltsw_port_info[unit]->max_frame_size = 9416;

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_info_init(unit));

    pinfo = xgs_ltsw_port_profile_port_pkt_ctrl(unit);
    pinfo->entry_idx_min = 0;
    pinfo->entry_idx_max = 63;
    pinfo->entries_per_set = 1;

    pipe_num = bcmi_ltsw_dev_max_pp_pipe_num(unit);
    if (pipe_num > MAX_INST_NUM_LPORT_TAB) {
        return SHR_E_CONFIG;
    }
    (void)xgs_ltsw_port_system_profile_opermode_get(unit, &mode);
    for (pipe = 0; pipe < pipe_num; pipe++) {
        pinfo = xgs_ltsw_port_system_profile(unit, pipe);
        pinfo->entry_idx_min = 0;
        pinfo->entry_idx_max = 67;
        pinfo->entries_per_set = 1;
        if (mode) {
            pinfo->inst = pipe;
        } else {
            pinfo->inst = 0;
        }
    }

    pc_info = PROFILE_PC_AUTONEG(unit);
    pc_info->entry_idx_min = 0;
    pc_info->entry_idx_max = 255;
    pc_info->entries_per_set = 1;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ltsw_pm_port_init_adjust(int unit, int nport,
                                     ltsw_flexport_port_cfg_t *ltsw_port_resource,
                                     int original_tvco)
{
    ltsw_flexport_port_cfg_t* operation[MAX_PORT_PER_PM] = {NULL};
    bcm_pbmp_t current_pbm;
    int i;

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(current_pbm);

    for (i = 0; i < nport; i++) {
        operation[i] = &ltsw_port_resource[i];
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ltsw_flexport_pm_validate(unit, &current_pbm, nport, operation));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ltsw_pm_port_rearrange(unit, nport, operation));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ltsw_port_fdr_config_set(int unit, bcm_port_t port, bcm_port_fdr_config_t *fdr_config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_fdr_config_set(unit, port, fdr_config));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ltsw_port_fdr_config_get(int unit, bcm_port_t port, bcm_port_fdr_config_t *fdr_config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_fdr_config_get(unit, port, fdr_config));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ltsw_port_fdr_stats_get(int unit, bcm_port_t port, bcm_port_fdr_stats_t *fdr_stats)
{
    uint32_t field_bmp;

    SHR_FUNC_ENTER(unit);

    field_bmp = BCM56996_A0_FDR_CW_S_ERR_MASK;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_fdr_stats_get(unit, port, field_bmp, fdr_stats));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ltsw_port_fdr_callback_register(int unit, bcm_port_fdr_handler_t f, void* user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_fdr_callback_register(unit, f, user_data));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ltsw_port_fdr_callback_unregister(int unit, bcm_port_fdr_handler_t f, void* user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_fdr_callback_unregister(unit, f, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Port sub driver functions for bcm56996_a0.
 */
static mbcm_ltsw_port_drv_t bcm56996_a0_port_sub_drv = {
    .port_info_init = bcm56996_a0_ltsw_port_info_init,
    .flexport_resource_validate = bcm56996_a0_ltsw_flexport_resource_validate,
    .port_speed_config_validate = bcm56996_a0_ltsw_port_speed_config_validate,
    .pm_port_init_adjust = bcm56996_a0_ltsw_pm_port_init_adjust,
    .port_fdr_config_set                    = bcm56996_a0_ltsw_port_fdr_config_set,
    .port_fdr_config_get                    = bcm56996_a0_ltsw_port_fdr_config_get,
    .port_fdr_stats_get                     = bcm56996_a0_ltsw_port_fdr_stats_get,
    .port_fdr_callback_register             = bcm56996_a0_ltsw_port_fdr_callback_register,
    .port_fdr_callback_unregister           = bcm56996_a0_ltsw_port_fdr_callback_unregister,
};

/******************************************************************************
 * Public functions
 */

int
bcm56996_a0_port_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ltsw_port_tab_info_update(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_drv_set(unit, &bcm56996_a0_port_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
