/*! \file port.c
 *
 * BCM56880_A0 port Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/rate.h>
#include <bcm_int/ltsw/l2.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/stg.h>
#include <bcm_int/ltsw/stat.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw/link.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/mbcm/port.h>
#include <bcm_int/ltsw/xfs/port.h>
#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/chip/bcm56880_a0/variant_dispatch.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmdrd/bcmdrd_devlist.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_PORT

#define L2_IF_SIZE          2048
#define L2_IF_BLACK_HOLE    L2_IF_SIZE - 1

#define MAX_PORT_GROUP      L2_IF_SIZE - 1
#define MAX_DVP_GROUP       16383

#define PORT_RATE_MIN_KBPS  32                  /* from arch */
#define PORT_RATE_MAX_KBPS  400*1000*1000       /* from max speed */
#define PORT_RATE_MIN_PPS   2                   /* from arch */
#define PORT_RATE_MAX_PPS   595238095           /* max speed / min pkt */
#define PORT_BURST_MIN_KBIT 2                   /* from arch */
#define PORT_BURST_MAX_KBIT 8*128*1024          /* from arch */
#define PORT_BURST_MIN_PKT  1                   /* from arch */
#define PORT_BURST_MAX_PKT  2146959             /* from arch 524.288*4095 */

#define MAX_NUM_PORT        160
#define MAX_NUM_FP_PM       32
#define MAX_NUM_PHY_PER_PM  8
#define MAX_NUM_FP_PHY      256
#define MAX_NUM_PP_PIPE     4

#define BCM56880_A0_MAX_AN_ABILITY  20


static const ltsw_port_map_t bcm56880_fixed_port_map[] = {
    /* CPU */
    {0,     0, 0,   BCMI_LTSW_PORT_TYPE_CPU},
    /* LB */
    {39,    1, 261, BCMI_LTSW_PORT_TYPE_LB},
    {79,    3, 262, BCMI_LTSW_PORT_TYPE_LB},
    {119,   5, 263, BCMI_LTSW_PORT_TYPE_LB},
    {159,   7, 264, BCMI_LTSW_PORT_TYPE_LB},
    /* MGMT */
    {38,    1, 257, BCMI_LTSW_PORT_TYPE_MA | BCMI_LTSW_PORT_TYPE_XL, FLEXIBLE | NO_REMAP},
    {78,    3, 258, BCMI_LTSW_PORT_TYPE_MA | BCMI_LTSW_PORT_TYPE_XL, FLEXIBLE | NO_REMAP},
    {118,   5, 259, BCMI_LTSW_PORT_TYPE_MA | BCMI_LTSW_PORT_TYPE_XL, FLEXIBLE | NO_REMAP},
    {158,   7, 260, BCMI_LTSW_PORT_TYPE_MA | BCMI_LTSW_PORT_TYPE_XL, FLEXIBLE | NO_REMAP},
    /* spare */
    {19,    0, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {58,    2, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {59,    2, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {98,    4, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {99,    4, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {138,   6, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {139,   6, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {0},
};

static const ltsw_port_map_t bcm56883_fixed_port_map[] = {
    /* CPU */
    {0,     0, 0,   BCMI_LTSW_PORT_TYPE_CPU},
    /* LB */
    {39,    1, 261, BCMI_LTSW_PORT_TYPE_LB},
    {79,    3, 262, BCMI_LTSW_PORT_TYPE_LB},
    {119,   5, 263, BCMI_LTSW_PORT_TYPE_LB},
    {159,   7, 264, BCMI_LTSW_PORT_TYPE_LB},
    /* MGMT */
    {38,    1, 257, BCMI_LTSW_PORT_TYPE_MA | BCMI_LTSW_PORT_TYPE_XL, FLEXIBLE | NO_REMAP},
    {158,   7, 260, BCMI_LTSW_PORT_TYPE_MA | BCMI_LTSW_PORT_TYPE_XL, FLEXIBLE | NO_REMAP},
    /* spare */
    {19,    0, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {58,    2, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {59,    2, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {78,    3, 258, BCMI_LTSW_PORT_TYPE_SPARE, INACTIVE},
    {98,    4, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {99,    4, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {118,   5, 259, BCMI_LTSW_PORT_TYPE_SPARE, INACTIVE},
    {138,   6, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {139,   6, -1,  BCMI_LTSW_PORT_TYPE_SPARE},
    {0},
};

static const bcmi_ltsw_port_resource_spec_t bcm56880_port_resource_spec[BCMI_PORT_RESOURCE_CNT] = {
    [BCMI_PORT_GROUP_ING]   = {0, L2_IF_SIZE - 1},
    [BCMI_PORT_GROUP_EGR]   = {0, 16383},
    [BCMI_PORT_GROUP_L2_IF] = {0, L2_IF_SIZE - 1},
    [BCMI_PORT_GROUP_DVP]   = {L2_IF_SIZE, 16383},
};

typedef struct chip_spec_s {
    const bcmi_ltsw_port_resource_spec_t *resource;
    const ltsw_port_map_t *fixed_port_map;
    uint32_t inactive_pm;
    uint32_t max_num_port_pp;   /* 0 means no limit */
} chip_spec_t;

static const chip_spec_t bcm56880_chip_spec = {
    bcm56880_port_resource_spec,
    bcm56880_fixed_port_map,
    0x0,
    0,
};

static const chip_spec_t bcm56883_chip_spec = {
    bcm56880_port_resource_spec,
    bcm56883_fixed_port_map,
    0x2A542A54,     /* PM 2,4,6,9,11,13,18,20,22,25,27,29 */
    18,
};

static const chip_spec_t *chip_spec[BCM_MAX_NUM_UNITS] = {NULL};

/*! TD4 force speed abilities and their VCO rates. */
const ltsw_port_fs_ability_table_entry_t_t bcm56880_a0_fs_ability_table[] =
{
    /* port_speed, num_lanes, fec_type, vco */
    {10000,  1, bcmPortPhyFecNone,         ltswPortVCO20P625G},
    {10000,  1, bcmPortPhyFecBaseR,        ltswPortVCO20P625G},
    {40000,  4, bcmPortPhyFecNone,         ltswPortVCO20P625G},
    {40000,  4, bcmPortPhyFecBaseR,        ltswPortVCO20P625G},
    {40000,  2, bcmPortPhyFecNone,         ltswPortVCO20P625G},
    {25000,  1, bcmPortPhyFecNone,         ltswPortVCO25P781G},
    {25000,  1, bcmPortPhyFecBaseR,        ltswPortVCO25P781G},
    {25000,  1, bcmPortPhyFecRsFec,        ltswPortVCO25P781G},
    {50000,  1, bcmPortPhyFecNone,         ltswPortVCO25P781G},
    {50000,  1, bcmPortPhyFecRsFec,        ltswPortVCO25P781G},
    {50000,  2, bcmPortPhyFecNone,         ltswPortVCO25P781G},
    {50000,  2, bcmPortPhyFecRsFec,        ltswPortVCO25P781G},
    {100000, 2, bcmPortPhyFecNone,         ltswPortVCO25P781G},
    {100000, 2, bcmPortPhyFecRsFec,        ltswPortVCO25P781G},
    {100000, 4, bcmPortPhyFecNone,         ltswPortVCO25P781G},
    {100000, 4, bcmPortPhyFecRsFec,        ltswPortVCO25P781G},
    {200000, 4, bcmPortPhyFecNone,         ltswPortVCO25P781G},
    {50000,  1, bcmPortPhyFecRs544,        ltswPortVCO26P562G},
    {50000,  1, bcmPortPhyFecRs272,        ltswPortVCO26P562G},
    {50000,  2, bcmPortPhyFecRs544,        ltswPortVCO26P562G},
    {100000, 2, bcmPortPhyFecRs272,        ltswPortVCO26P562G},
    {100000, 2, bcmPortPhyFecRs544,        ltswPortVCO26P562G},
    {100000, 4, bcmPortPhyFecRs544,        ltswPortVCO26P562G},
    {200000, 4, bcmPortPhyFecRs544,        ltswPortVCO26P562G},
    {200000, 4, bcmPortPhyFecRs544_2xN,    ltswPortVCO26P562G},
    {200000, 4, bcmPortPhyFecRs272,        ltswPortVCO26P562G},
    {200000, 4, bcmPortPhyFecRs272_2xN,    ltswPortVCO26P562G},
    {400000, 8, bcmPortPhyFecRs544_2xN,    ltswPortVCO26P562G},
    {400000, 8, bcmPortPhyFecRs272_2xN,    ltswPortVCO26P562G}
};

/*! A comprehensive list of TD4 AN abilities
 * and their VCO rates. */
const ltsw_port_an_ability_table_entry_t bcm56880_a0_an_ability_table[] =
{
    /*! Speed, number of lanes, FEC type, AN mode, VCO rate. */
    {10000,  1, bcmPortPhyFecNone,      bcmPortAnModeCL73,     ltswPortVCO20P625G},
    {10000,  1, bcmPortPhyFecBaseR,     bcmPortAnModeCL73,     ltswPortVCO20P625G},
    {40000,  2, bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,  ltswPortVCO20P625G},
    {40000,  4, bcmPortPhyFecNone,      bcmPortAnModeCL73,     ltswPortVCO20P625G},
    {40000,  4, bcmPortPhyFecBaseR,     bcmPortAnModeCL73,     ltswPortVCO20P625G},
    {25000,  1, bcmPortPhyFecNone,      bcmPortAnModeCL73,     ltswPortVCO25P781G},
    {25000,  1, bcmPortPhyFecBaseR,     bcmPortAnModeCL73,     ltswPortVCO25P781G},
    {25000,  1, bcmPortPhyFecRsFec,     bcmPortAnModeCL73,     ltswPortVCO25P781G},
    {25000,  1, bcmPortPhyFecNone,      bcmPortAnModeCL73MSA,  ltswPortVCO25P781G},
    {25000,  1, bcmPortPhyFecBaseR,     bcmPortAnModeCL73MSA,  ltswPortVCO25P781G},
    {25000,  1, bcmPortPhyFecRsFec,     bcmPortAnModeCL73MSA,  ltswPortVCO25P781G},
    {25000,  1, bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,  ltswPortVCO25P781G},
    {25000,  1, bcmPortPhyFecBaseR,     bcmPortAnModeCL73BAM,  ltswPortVCO25P781G},
    {25000,  1, bcmPortPhyFecRsFec,     bcmPortAnModeCL73BAM,  ltswPortVCO25P781G},
    {50000,  1, bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,  ltswPortVCO25P781G},
    {50000,  1, bcmPortPhyFecRsFec,     bcmPortAnModeCL73BAM,  ltswPortVCO25P781G},
    {50000,  2, bcmPortPhyFecNone,      bcmPortAnModeCL73MSA,  ltswPortVCO25P781G},
    {50000,  2, bcmPortPhyFecRsFec,     bcmPortAnModeCL73MSA,  ltswPortVCO25P781G},
    {50000,  2, bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,  ltswPortVCO25P781G},
    {50000,  2, bcmPortPhyFecRsFec,     bcmPortAnModeCL73BAM,  ltswPortVCO25P781G},
    {100000, 2, bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,  ltswPortVCO25P781G},
    {100000, 2, bcmPortPhyFecRsFec,     bcmPortAnModeCL73BAM,  ltswPortVCO25P781G},
    {100000, 4, bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,  ltswPortVCO25P781G},
    {100000, 4, bcmPortPhyFecRsFec,     bcmPortAnModeCL73,     ltswPortVCO25P781G},
    {200000, 4, bcmPortPhyFecNone,      bcmPortAnModeCL73BAM,  ltswPortVCO25P781G},
    {50000,  1, bcmPortPhyFecRs544,     bcmPortAnModeCL73,     ltswPortVCO26P562G},
    {50000,  1, bcmPortPhyFecRs272,     bcmPortAnModeCL73MSA,  ltswPortVCO26P562G},
    {50000,  2, bcmPortPhyFecRs544,     bcmPortAnModeCL73BAM,  ltswPortVCO26P562G},
    {100000, 2, bcmPortPhyFecRs544,     bcmPortAnModeCL73,     ltswPortVCO26P562G},
    {100000, 2, bcmPortPhyFecRs272,     bcmPortAnModeCL73MSA,  ltswPortVCO26P562G},
    {100000, 4, bcmPortPhyFecRs544,     bcmPortAnModeCL73BAM,  ltswPortVCO26P562G},
    {200000, 4, bcmPortPhyFecRs544_2xN, bcmPortAnModeCL73,     ltswPortVCO26P562G},
    {200000, 4, bcmPortPhyFecRs544,     bcmPortAnModeCL73BAM,  ltswPortVCO26P562G},
    {200000, 4, bcmPortPhyFecRs272_2xN, bcmPortAnModeCL73MSA,  ltswPortVCO26P562G},
    {400000, 8, bcmPortPhyFecRs544_2xN, bcmPortAnModeCL73MSA,  ltswPortVCO26P562G}
};


static
int bcm56880_a0_fs_ability_table_vco_get(uint32 speed, uint32 num_lanes, bcm_port_phy_fec_t fec_type,
                                         ltsw_port_vco_type_t* vco)
{
    int i, entry_num;

    entry_num = sizeof(bcm56880_a0_fs_ability_table)/sizeof(ltsw_port_fs_ability_table_entry_t_t);

    *vco = ltswPortVCOInvalid;
    for (i = 0; i < entry_num; i++) {
        if (bcm56880_a0_fs_ability_table[i].speed == speed &&
            bcm56880_a0_fs_ability_table[i].num_lanes == num_lanes &&
            bcm56880_a0_fs_ability_table[i].fec_type == fec_type)
        {
            *vco = bcm56880_a0_fs_ability_table[i].vco;
            break;
        }
    }

    return SHR_E_NONE;
}

static
int bcm56880_a0_an_ability_table_vco_get(uint32 speed, uint32 num_lanes, bcm_port_phy_fec_t fec_type,
                                         bcm_port_autoneg_mode_t an_mode,
                                         ltsw_port_vco_type_t* vco)
{
    int i, entry_num;

    entry_num = sizeof(bcm56880_a0_an_ability_table) /sizeof(ltsw_port_an_ability_table_entry_t);

    *vco = ltswPortVCOInvalid;
    for (i = 0; i < entry_num; i++) {
        if (bcm56880_a0_an_ability_table[i].speed == speed &&
            bcm56880_a0_an_ability_table[i].num_lanes == num_lanes &&
            bcm56880_a0_an_ability_table[i].fec_type == fec_type &&
            bcm56880_a0_an_ability_table[i].an_mode == an_mode)
        {
            *vco = bcm56880_a0_an_ability_table[i].vco;
            break;
        }
    }

    return SHR_E_NONE;
}


int bcm56880_a0_manage_port_vco_get(int unit, int port,
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
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief adjust the operation of flexport resource according to validation status.
 *
 * \param [in] unit           Unit number.
 * \param [in] nport          Number of elements in array resource.
 * \param [in] resource       Port resource configuration array.
 * \param [in] validate       Port resource validation status.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int bcm56880_a0_ltsw_flexport_pm_operation_adjust(int unit, int nport, ltsw_flexport_port_cfg_t **port_cfg_array, ltsw_flexport_port_validation_resolve_t *validate)
{
    bcm_port_t port;
    uint32_t lane_mask, new_rs544_bmp, new_rs272_bmp;
    uint32_t current_rs544_bmp = 0;
    uint32_t current_rs272_bmp = 0;
    int i, an, an_ability_num;
    bcm_port_speed_ability_t an_ability[BCM56880_A0_MAX_AN_ABILITY];
    bcm_port_resource_t speed_resource;
    ltsw_port_vco_type_t vco;
    int is_flexport_operation = 0;
    int current_tvco, current_ovco, pll_num, required_vco_num;
    int tvco_need_change = 0, ovco_need_change = 0;
    ltsw_port_vco_type_t required_vco[2];
    bcm_pbmp_t reset_ports;

    SHR_FUNC_ENTER(unit);

    is_flexport_operation = !(port_cfg_array[0]->flags & FLEXPORT_FLG_NO_REMAP);

    required_vco_num = validate->required_vco_num;
    required_vco[0] = validate->required_vco[0];
    required_vco[1] = validate->required_vco[1];
    new_rs544_bmp = validate->new_rs544_bmp;
    new_rs272_bmp = validate->new_rs272_bmp;

    BCM_PBMP_CLEAR(reset_ports);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_vco_get(unit, port_cfg_array[0]->pmid, &pll_num, &current_tvco, &current_ovco));
    if (pll_num != 2) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "PLL get fail.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    }

    /* Set PC_RESET flag for flexport operation.
     * If port is set as FLEXPORT_OP_DETACH in flexport operation, always tell Port module to delete it.
     * FLEXPORT_OP_DETACH indicats the port need to be deleted or to be modified.
     * FLEXPORT_FLG_NO_REMAP indicats only speed/FEC change, not flexport operation.
     * FLEXPORT_OP_PC_RESET indicates the port need to delete/add on pc level.
     */
    if (is_flexport_operation) {
        for (i = 0; i < nport; i++) {
            if (port_cfg_array[i]->op & FLEXPORT_OP_DETACH) {
                port_cfg_array[i]->op |= FLEXPORT_OP_PC_RESET;
                BCM_PBMP_PORT_ADD(reset_ports, port_cfg_array[i]->lport);
            } else {
                if (BCM_PBMP_MEMBER(reset_ports, port_cfg_array[i]->lport)) {
                    port_cfg_array[i]->op |= FLEXPORT_OP_PC_RESET;
                }
            }
        }
    } else {
        /* Check if TVCO or OVCO need to be changed during speed/FEC change */
        if (current_ovco == ltswPortVCONone) {
            /* TVCO is active and OVCO is inactive.
             * OVCO always is avaible.
             */
            ovco_need_change = 0;
            if (required_vco_num == 1) {
                tvco_need_change = 0;
            } else if (required_vco_num == 2) {
                if ((required_vco[0] == current_tvco) || (required_vco[1] == current_tvco)) {
                    /* one of required VCOs equals TVCO */
                    tvco_need_change = 0;
                } else {
                    /* both required VCOs don't equal TVCO */
                    tvco_need_change = 1;
                }
            }
        } else if (current_ovco != ltswPortVCONone) {
            /* Both TVCO and OVCO are active.
            *  Check which one need to be changed.
             */
            if (required_vco_num == 1) {
                /* Only requirs one VCO, check OVCO if need to change */
                tvco_need_change = 0;
                if ((required_vco[0] == current_tvco) || (required_vco[0] == current_ovco)) {
                    /* required VCO equals TVCO or OVCO */
                    ovco_need_change = 0;
                } else {
                    /* required VCO equals neither TVCO nor OVCO */
                    ovco_need_change = 1;
                }
            } else if (required_vco_num == 2) {
                /* There are only three potential VCO values (20/25/26) for PLL0 and PLL1.
                 * So there must be one required VCO which equals one of current VCOs in this case.
                 * When one of required VCOs doesn't equal OVCO, the other required VCO must equal TVCO and vice versa.
                 */
                if((required_vco[0] != current_ovco) && (required_vco[1] != current_ovco)){
                    /* one of required VCOs doesn't equal OVCO */
                    tvco_need_change = 0;
                    ovco_need_change = 1;
                } else if ((required_vco[0] != current_tvco) && (required_vco[1] != current_tvco)) {
                    /* one of required VCOs doesn't equal TVCO */
                    tvco_need_change = 1;
                    ovco_need_change = 0;
                } else {
                    /* one of required VCOs equals OVCO, and the other equals TVCO */
                    tvco_need_change = 0;
                    ovco_need_change = 0;
                }
            }
        }

        /* Set PC_RESET flag for speed/FEC change if VCO need to change.
         * Delete all old ports if TVCO need to change.
         * Delete ports on OVCO if OVCO need to change.
         */
        BCM_PBMP_CLEAR(reset_ports);
        for (i = 0; i < nport; i++) {
            if (tvco_need_change) {
                /* If tvco need to change, delete all ports during speed/FEC change */
                port_cfg_array[i]->op |= FLEXPORT_OP_PC_RESET;
            } else if (ovco_need_change) {
                /* If ovco need to change, delete ports on OVCO during speed/FEC change */
                if (port_cfg_array[i]->op & FLEXPORT_OP_DETACH) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcm56880_a0_fs_ability_table_vco_get(port_cfg_array[i]->speed, port_cfg_array[i]->lanes,
                                                              port_cfg_array[i]->fec, &vco));
                    if (vco == current_ovco) {
                        port_cfg_array[i]->op |= FLEXPORT_OP_PC_RESET;
                        BCM_PBMP_PORT_ADD(reset_ports, port_cfg_array[i]->lport);
                    }
                } else {
                    if (BCM_PBMP_MEMBER(reset_ports, port_cfg_array[i]->lport)) {
                        port_cfg_array[i]->op |= FLEXPORT_OP_PC_RESET;
                    }
                }
            }
        }

        /* Set PC_RESET flag for FEC type change between RS272 and RS544.
         * RS272 and RS544 can't be accommodated in one MPP.
         * Delete the old ports which FEC type is RS272 and FEC type of other ports in one MPP are RS544, or
         * Old port FEC type is RS544 and FEC type of other port in one MPP are RS272.
         */
        BCM_PBMP_CLEAR(reset_ports);
        for (i = 0 ; i < nport; i++) {
            if (port_cfg_array[i]->op & FLEXPORT_OP_DETACH) {
                lane_mask = 0;
                port = port_cfg_array[i]->lport;
                SHR_IF_ERR_VERBOSE_EXIT
                   (bcmint_ltsw_port_autoneg_get(unit, port, &an));
                SHR_IF_ERR_VERBOSE_EXIT
                   (bcmint_ltsw_port_lane_mask_get(unit, port, &lane_mask));

                /*Get the current rs272 and rs544 bmp*/
                if (an) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_ltsw_port_autoneg_ability_advert_get(unit, port, BCM56880_A0_MAX_AN_ABILITY,
                                                                  an_ability, &an_ability_num));

                    for (i = 0; i < an_ability_num; i++) {
                        /*Set up fec bmp*/
                        if (an_ability[i].fec_type == bcmPortPhyFecRs272 ||
                            an_ability[i].fec_type == bcmPortPhyFecRs272_2xN) {
                            current_rs272_bmp |= lane_mask;
                        }

                        if (an_ability[i].fec_type == bcmPortPhyFecRs544 ||
                            an_ability[i].fec_type == bcmPortPhyFecRs544_2xN) {
                            current_rs544_bmp |= lane_mask;
                        }
                    }
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_ltsw_port_resource_get(unit, port, &speed_resource));
                    /*Set up fec bmp*/
                    if (speed_resource.fec_type == bcmPortPhyFecRs272 ||
                        speed_resource.fec_type == bcmPortPhyFecRs272_2xN) {
                        current_rs272_bmp |= lane_mask;
                    }

                    if (speed_resource.fec_type == bcmPortPhyFecRs544 ||
                        speed_resource.fec_type == bcmPortPhyFecRs544_2xN) {
                        current_rs544_bmp |= lane_mask;
                    }
                }

                if (((current_rs272_bmp & 0xf) && ((new_rs544_bmp & (~lane_mask)) & 0xf))||
                    ((current_rs272_bmp & 0xf0) && ((new_rs544_bmp & (~lane_mask)) & 0xf0))||
                    ((current_rs544_bmp & 0xf) && ((new_rs272_bmp & (~lane_mask)) & 0xf)) ||
                    ((current_rs544_bmp & 0xf0) && ((new_rs272_bmp & (~lane_mask)) & 0xf0))) {
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
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate if the new ports or new configure can be allocated in the PM.
 * \Validate lane map, FEC type and VCO.
 *
 * \param [in] unit           Unit number.
 * \param [in] current_ports  Current ports bmp in the PM.
 * \param [in] nport          Number of elements in array resource.
 * \param [in] resource       Port resource configuration array.
 * \param [out] validate      Port resource validation status.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static
int bcm56880_a0_ltsw_flexport_pm_resource_validate(int unit, bcm_pbmp_t *current_ports, int nport, ltsw_flexport_port_cfg_t **port_cfg_array, ltsw_flexport_port_validation_resolve_t *validate)
{
    uint32_t remain_ports_lane_bmp, new_ports_lane_bmp;
    bcm_pbmp_t remain_ports;
    bcm_port_t port;
    uint32_t lane_mask;
    uint32_t rs544_bmp, rs272_bmp;
    int i, an, an_ability_num;
    bcm_port_speed_ability_t an_ability[BCM56880_A0_MAX_AN_ABILITY];
    bcm_port_resource_t speed_resource;
    int current_tvco, current_ovco, pll_num;
    uint32_t tvco_lane_bmp, ovco_lane_bmp;
    ltsw_port_vco_type_t vco;
    int tvco_is_free = 0, ovco_is_free = 0;
    ltsw_port_vco_type_t required_vco[2] = {ltswPortVCOInvalid, ltswPortVCOInvalid};
    int required_vco_num, pm_type;
    int pm_id;
    int sp_10g_at_25g = 0;
    int vco_20g_index = -1, vco_26g_index = -1;

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(remain_ports);
    /*get the current tvco and ovco*/
    pm_id = port_cfg_array[0]->pmid;
    for (i = 0; i < nport; i++) {
        if (pm_id != port_cfg_array[i]->pmid) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pm_type_get(unit, pm_id, &pm_type));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_vco_get(unit, pm_id, &pll_num, &current_tvco, &current_ovco));
    if (pll_num != 2) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "PLL get fail.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    }

    if (pm_type == ltswPmTypePm4x10 && !(port_cfg_array[0]->flags & FLEXPORT_FLG_NO_REMAP)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "Management port doesn`t support flexport.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    BCM_PBMP_ASSIGN(remain_ports, *current_ports);
    remain_ports_lane_bmp = 0;
    new_ports_lane_bmp = 0;

    required_vco_num = 0;
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

            /*Get the required vco*/
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fs_ability_table_vco_get(port_cfg_array[i]->speed, port_cfg_array[i]->lanes,
                                                      port_cfg_array[i]->fec, &vco));
            if (vco == ltswPortVCOInvalid) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit, "port %d: speed[%d] lane[%d] fec[%d]is not supported.\n"),
                     port_cfg_array[i]->lport, port_cfg_array[i]->speed, port_cfg_array[i]->lanes, port_cfg_array[i]->fec));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
            }

            /*Record the port index for 1x20/2x20*/
            if (vco == ltswPortVCO20P625G) {
                if (!((port_cfg_array[i]->speed == 10000 && port_cfg_array[i]->lanes == 1) ||
                    (port_cfg_array[i]->speed == 40000 && port_cfg_array[i]->lanes == 4))) {
                    if (vco_20g_index == -1) {
                        vco_20g_index = i;
                    }
                }
            }

            if (vco == ltswPortVCO26P562G) {
                if (vco_26g_index == -1) {
                    vco_26g_index = i;
                }
            }
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

    /*Set up the rs544_bmp, rs272_bmp, tvco_lane_bmp, ovco_lane_bmp*/
    rs544_bmp = 0;
    rs272_bmp = 0;
    tvco_lane_bmp = 0;
    ovco_lane_bmp = 0;
    BCM_PBMP_ITER(remain_ports, port) {
        /*Check the AN enable*/
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_autoneg_get(unit, port, &an));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_lane_mask_get(unit, port, &lane_mask));
        if (an) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_autoneg_ability_advert_get(unit, port, BCM56880_A0_MAX_AN_ABILITY,
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

                /*set up vco bmp*/
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcm56880_a0_an_ability_table_vco_get(an_ability[i].speed, an_ability[i].resolved_num_lanes,
                                                          an_ability[i].fec_type, an_ability[i].an_mode, &vco));
                if ((an_ability[i].speed == 10000 && an_ability[i].resolved_num_lanes == 1) ||
                    (an_ability[i].speed == 40000 && an_ability[i].resolved_num_lanes == 4)) {
                    if (current_ovco != ltswPortVCO20P625G) {
                        vco = ltswPortVCO25P781G;
                    }
                }
                if (vco == current_tvco) {
                    tvco_lane_bmp |= lane_mask;
                } else if (vco == current_ovco) {
                    ovco_lane_bmp |= lane_mask;
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
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

            /*set up vco bmp*/
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fs_ability_table_vco_get(speed_resource.speed, speed_resource.lanes,
                                                      speed_resource.fec_type, &vco));
            if ((speed_resource.speed == 10000 && speed_resource.lanes == 1) ||
                (speed_resource.speed == 40000 && speed_resource.lanes == 4)) {
                if (current_ovco != ltswPortVCO20P625G) {
                    vco = ltswPortVCO25P781G;
                }
            }
            if (vco == current_tvco) {
                tvco_lane_bmp |= lane_mask;
            } else if (vco == current_ovco) {
                ovco_lane_bmp |= lane_mask;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }
    }

    /*Current vco free status check*/
    if ((tvco_lane_bmp | ovco_lane_bmp) == 0) {
        tvco_is_free = 1;
        ovco_is_free = 1;
    } else {
        if (ovco_lane_bmp == 0) {
            ovco_is_free = 1;
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
    if ((rs544_bmp & 0xf) && (rs272_bmp & 0xf)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "Can not accommodate FEC settings on MPP0.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if ((rs544_bmp & 0xf0) && (rs272_bmp & 0xf0)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "Can not accommodate FEC settings on MPP1.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /*VCO validation*/

    /*Setup required VCO array*/
    for (i = 0; i < nport; i++) {
        /*If op is FLEXPORT_OP_DETACH, the logic port will be removed*/
        if(!(port_cfg_array[i]->op & FLEXPORT_OP_DETACH)) {
            /*Get the required vco*/
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fs_ability_table_vco_get(port_cfg_array[i]->speed, port_cfg_array[i]->lanes,
                                                      port_cfg_array[i]->fec, &vco));
            if (vco == ltswPortVCOInvalid) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit, "port %d: speed[%d] lane[%d] fec[%d]is not supported.\n"),
                     port_cfg_array[i]->lport, port_cfg_array[i]->speed, port_cfg_array[i]->lanes, port_cfg_array[i]->fec));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
            }

            /*TD4 support 10G and 40G with 4 lanes working at both 20G and 25G*/
            if ((port_cfg_array[i]->speed == 10000 && port_cfg_array[i]->lanes == 1) ||
                (port_cfg_array[i]->speed == 40000 && port_cfg_array[i]->lanes == 4)) {
                /*If there is 20g VCO, speed 10g and 40g 4 lanes should
                  work at 20g vco*/
                if ((current_ovco == ltswPortVCO20P625G) && !ovco_is_free) {
                    vco = ltswPortVCO20P625G;
                } else {
                    if ((required_vco[0] == ltswPortVCO20P625G) ||
                        (required_vco[1] == ltswPortVCO20P625G)) {
                        vco = ltswPortVCO20P625G;
                    } else {
                        if (vco_20g_index != -1) {
                            /*For the special case 4x10/1x10+1x20/2x20*/
                            vco = ltswPortVCO20P625G;
                        } else {
                            vco = ltswPortVCO25P781G;
                            sp_10g_at_25g = 1;
                        }
                    }
                }
            }

            if (vco == required_vco[0] || vco == required_vco[1]) {
                continue;
            }

            required_vco_num ++;
            if (required_vco_num > 2) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit, "required vco number is more than 2.\n")));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            required_vco[required_vco_num -1] = vco;
        }
    }

    validate->required_vco_num = required_vco_num;
    validate->required_vco[0] = required_vco[0];
    validate->required_vco[1]= required_vco[1];
    validate->new_rs544_bmp = rs544_bmp;
    validate->new_rs272_bmp = rs272_bmp;

    /*special case for the VCO 20G*/
    if ((required_vco_num == 1) && tvco_is_free && (required_vco[0] == ltswPortVCO20P625G)) {
        SHR_EXIT();
    }

    /*special case for the VCO 20G & 26G
      If the VCO of the first port is 20G, insert the port with 26g first.
      If there are 4x10/1x10 and 1x20/2x20 ports, the 1x20/2x20 port is inserted before 4x10/1x10.
    */
    if ((required_vco[0] == ltswPortVCO20P625G && required_vco[1] == ltswPortVCO26P562G)) {
        if (tvco_is_free) {
           port_cfg_array[vco_26g_index]->op |= FLEXPORT_OP_ATTACH_FIRST;
           if (vco_20g_index != -1) {
               port_cfg_array[vco_20g_index]->op |= FLEXPORT_OP_ATTACH_SECOND;
           }
           SHR_EXIT();
        }
    }

    /*special case for the first port is 10G or 40G 4 lanes*/
    if ((required_vco[0] == ltswPortVCO25P781G) &&
        (required_vco[1] == ltswPortVCO26P562G) &&
        sp_10g_at_25g) {
        port_cfg_array[vco_26g_index]->op |= FLEXPORT_OP_ATTACH_FIRST;

        SHR_EXIT();
    }

    for (i = 0; i < required_vco_num; i++) {
        /*If the TVCO is in use, check if the required VCO is equal to the current TVCO */
        if (!tvco_is_free) {
            if (required_vco[i] == current_tvco) {
                continue;
            }
        }

        /*If the OVCO is in use, check if the required VCO is equal to the current OVCO */
        if (!ovco_is_free) {
            if (required_vco[i] == current_ovco) {
                continue;
            }
        }

       /*Check if the required VCOs can be accommodated when the TVCO or OVCO is free*/
        if (required_vco[i] != ltswPortVCO20P625G) {
            if (tvco_is_free) {
                current_tvco = required_vco[i];
                tvco_is_free = 0;
                continue;
            }

            if (ovco_is_free) {
                current_ovco = required_vco[i];
                ovco_is_free = 0;
                continue;
            }
        } else {
            if (ovco_is_free) {
                current_ovco = required_vco[i];
                ovco_is_free = 0;
                continue;
            }
        }

        /*The required VCO can`t be accommodated, validation failed*/
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "Can not accommodate the request VCO.\n request [VCO %d].\n"),
                        required_vco[i]));
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
int bcm56880_a0_ltsw_flexport_pm_validate(int unit, bcm_pbmp_t *current_ports, int nport, ltsw_flexport_port_cfg_t **port_cfg_array)
{
    ltsw_flexport_port_validation_resolve_t validate;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcm56880_a0_ltsw_flexport_pm_resource_validate(unit, current_ports, nport, port_cfg_array, &validate));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcm56880_a0_ltsw_flexport_pm_operation_adjust(unit, nport, port_cfg_array, &validate));

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
int bcm56880_a0_ltsw_port_speed_config_validate(int unit, bcm_port_resource_t *resource, bcm_pbmp_t *affect_ports)
{
    bcm_pbmp_t port_pbmp, ovco_port_pbmp;
    bcm_port_speed_ability_t an_ability[BCM56880_A0_MAX_AN_ABILITY];
    bcm_port_resource_t speed_resource;
    bcm_port_t port;
    ltsw_port_vco_type_t vco;
    int current_tvco, current_ovco, pll_num;
    uint32_t rs544_bmp = 0, rs272_bmp = 0, lane_mask = 0;
    int i, pm_id, an, an_ability_num, pm_type;
    uint32_t num_lane;

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(port_pbmp);
    BCM_PBMP_CLEAR(ovco_port_pbmp);
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
        (bcmint_ltsw_port_vco_get(unit, pm_id, &pll_num, &current_tvco, &current_ovco));
    if (pll_num > 2) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "PLL get fail.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FAIL);
    }

    /*Validate the speed for the manegement port*/
    if (pm_type == ltswPmTypePm4x10) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56880_a0_manage_port_vco_get(unit, resource->port, resource, &vco));
        if (vco != current_tvco) {
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
                (bcmint_ltsw_port_autoneg_ability_advert_get(unit, port, BCM56880_A0_MAX_AN_ABILITY,
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

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcm56880_a0_an_ability_table_vco_get(an_ability[i].speed, an_ability[i].resolved_num_lanes,
                                                          an_ability[i].fec_type, an_ability[i].an_mode, &vco));
                if (vco == current_ovco) {
                    BCM_PBMP_PORT_ADD(ovco_port_pbmp, port);
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

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fs_ability_table_vco_get(speed_resource.speed, speed_resource.lanes,
                                                      speed_resource.fec_type, &vco));
            if (vco == current_ovco) {
                BCM_PBMP_PORT_ADD(ovco_port_pbmp, port);
            }
        }

        if ((lane_mask & 0xf) && (rs544_bmp & 0xf) && (rs272_bmp & 0xf)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "Can not accommodate FEC settings on MPP0. Conflict with port %d\n"), port));
            BCM_PBMP_PORT_ADD(*affect_ports, port);
        }

        if ((lane_mask & 0xf0) && (rs544_bmp & 0xf0) && (rs272_bmp & 0xf0)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "Can not accommodate FEC settings on MPP1. Conflict with port %d\n"), port));
            BCM_PBMP_PORT_ADD(*affect_ports, port);
        }
    }

    /*FEC check*/
    if(BCM_PBMP_NOT_NULL(*affect_ports)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /*VCO check*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_fs_ability_table_vco_get(resource->speed, resource->lanes,
                                              resource->fec_type, &vco));
    if (vco == ltswPortVCOInvalid) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "port %d: speed[%d] lane[%d] fec[%d]is not supported.\n"),
             resource->port, resource->speed,resource->lanes, resource->fec_type));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }
    if ((resource->speed == 10000 && resource->lanes ==1) ||
        (resource->speed == 40000 && resource->lanes ==4)) {
        if (current_tvco == ltswPortVCO25P781G
            || current_ovco == ltswPortVCO25P781G) {
            vco = ltswPortVCO25P781G;
        }
    }

    if (vco == current_tvco || vco == current_ovco) {
        /*No VCO change is required*/
        SHR_EXIT();
    } else {
        /*Only ovco change is allowed*/
        if (BCM_PBMP_IS_NULL(ovco_port_pbmp)) {
            /*VCO change is required, when OVCO is not used by others ports*/
            SHR_EXIT();
        } else {
            /*Speed change is not allowed, return the affected ports*/
            BCM_PBMP_ASSIGN(*affect_ports, ovco_port_pbmp);
            LOG_ERROR(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "Can not accommodate the request VCO.\n request [VCO %d].\n"),vco));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static inline int pport_to_pp(int pport)
{
    return (pport - 1) / 64;
}

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Lookup port and lanes in fixed_port_map.
 *
 * \param [in] port Port.
 *
 * \return index of matched entry, or -1 if not found.
 */
static int
fixed_port_lkp(int unit, int port)
{
    const ltsw_port_map_t *pm = NULL;

    for (pm = chip_spec[unit]->fixed_port_map; pm->ptype; pm ++) {
        if (pm->lport == port) {
            return (pm - chip_spec[unit]->fixed_port_map);
        }
    }

    return -1;
}


/*!
 * \brief Init BCM56880 specific information for a port.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_port_info_init_per_port(int unit, int port)
{
    int id;

    id  = fixed_port_lkp(unit, port);
    if (id < 0) {
        /* common port */
        PORT(unit, port).port_type = BCMI_LTSW_PORT_TYPE_CD;
        PORT(unit, port).flags |= (FLEXIBLE | CT_VALID | HG_VALID);
    } else {
        /* fixed port */
        const ltsw_port_map_t *map = chip_spec[unit]->fixed_port_map + id;
        PORT(unit, port).pport = map->pport;
        PORT(unit, port).port_type = map->ptype;
        PORT(unit, port).flags = map->flags;
    }

    return SHR_E_NONE;
}


/*!
 * \brief Init BCM56880 specific information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_port_info_init(int unit)
{
    int port, pport, pm, flags;
    const ltsw_port_map_t *map = NULL;
    ltsw_pc_profile_info_t *pc_info;
    uint16_t dev_id = bcmi_ltsw_dev_device_id(unit);

    SHR_FUNC_ENTER(unit);

    if (dev_id == BCM56883_DEVICE_ID) {
        chip_spec[unit] = &bcm56883_chip_spec;
    } else {
        chip_spec[unit] = &bcm56880_chip_spec;
    }

    flags = FLEXIBLE | CT_VALID | HG_VALID;
    for (port = 0; port < MAX_NUM_PORT; port ++) {
        PORT(unit, port).port_type = BCMI_LTSW_PORT_TYPE_CD;
        PORT(unit, port).flags = flags;
    }

    for (pm = 0; pm < MAX_NUM_FP_PM; pm ++) {
        if (chip_spec[unit]->inactive_pm & (1 << pm)) {
            flags = INACTIVE;
        } else {
            flags = FLEXIBLE;
        }
        for (pport = pm * MAX_NUM_PHY_PER_PM + 1;
             pport <= (pm + 1) * MAX_NUM_PHY_PER_PM; pport ++) {
            ltsw_port_info[unit]->phy_port[pport].flags = flags;
        }
    }

    for (map = chip_spec[unit]->fixed_port_map; map->ptype; map ++) {
        port = map->lport;
        PORT(unit, port).pport = map->pport;
        PORT(unit, port).port_type = map->ptype;
        PORT(unit, port).flags = map->flags;
        pport = map->pport;
        if (pport >= 0) {
            ltsw_port_info[unit]->phy_port[pport].flags = map->flags;
        }
    }
    ltsw_port_info[unit]->min_lanes_per_pport = 1;
    ltsw_port_info[unit]->max_frame_size = 9416;

    pc_info = PROFILE_PC_AUTONEG(unit);
    pc_info->entry_idx_min = 0;
    pc_info->entry_idx_max = 255;
    pc_info->entries_per_set = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_l2_if_init(unit, L2_IF_SIZE));

    /* reserve for local ports and entry 0 */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_l2_if_reserve(unit, 0, MAX_NUM_PORT + 1));

    /* reserve for black hole */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_l2_if_reserve(unit, L2_IF_BLACK_HOLE, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_info_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_db_init(unit));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Deinit BCM56880 specific information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_port_info_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_info_deinit(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_l2_if_deinit(unit));

    chip_spec[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}


static int
bcm56880_a0_ltsw_port_profile_init_per_port(int unit, int port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_profile_init_per_port(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_pc_profile_id_init_per_port(unit, port));

exit:
    SHR_FUNC_EXIT();
}


static int
bcm56880_a0_ltsw_port_profile_deinit_per_port(int unit, int port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_profile_deinit_per_port(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_pc_profile_clear_per_port(unit, port));

exit:
    SHR_FUNC_EXIT();
}


int
bcm56880_a0_ltsw_port_profile_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_profile_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pc_profile_init(unit));

exit:
    SHR_FUNC_EXIT();
}


int
bcm56880_a0_ltsw_port_profile_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_profile_deinit(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_pc_profile_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Initialize basic port pipeline configuration for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_port_service_init_per_port(int unit, int port)
{
    ltsw_port_tab_field_t *fields = NULL;
    int fields_num_max, fields_num;
    int modid, l2_if, sys_port, pp_pipe, pp_port, strength;
    uint64_t u64[2];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &modid));
    if (IS_CPU_PORT(unit, port)) {
        /* CPU system port = mod:160 */
        sys_port = (modid << 8) | 160;
        /* CPU l2_if = 160 */
        l2_if = 160;
    } else {
        /* system port = mod:port */
        sys_port = (modid << 8) | (port & 0xFF);
        /* l2_if = port */
        l2_if = port;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_lt_add(unit, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_sys_port_add(unit, port, sys_port));
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_l2_if_add(unit, port, l2_if));
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_sys_port_map_l2_if(unit, sys_port, l2_if));

    fields_num_max = 32;
    fields_num = 0;
    SHR_ALLOC(fields,
        sizeof(ltsw_port_tab_field_t) * fields_num_max, "port_service_init");
    SHR_NULL_CHECK(fields, SHR_E_MEMORY);
    sal_memset(fields, 0, sizeof(ltsw_port_tab_field_t) * fields_num_max);

    /* port type */
    fields[fields_num].type = BCMI_PT_ING_PORT_TYPE;
    fields[fields_num].data.n = PORT_TYPE_ETHERNET;
    fields_num ++;
    fields[fields_num].type = BCMI_PT_EGR_PORT_TYPE;
    fields[fields_num].data.n =
        IS_CPU_PORT(unit, port) ? PORT_TYPE_CPU :
        (IS_LB_PORT(unit, port) ? PORT_TYPE_LOOPBACK : PORT_TYPE_ETHERNET);
    fields_num ++;
    fields[fields_num].type = BCMI_PT_EGR_ING_PORT_TYPE;
    fields[fields_num].data.n =
        IS_CPU_PORT(unit, port) ? PORT_TYPE_CPU : PORT_TYPE_ETHERNET;
    fields_num ++;

    /* port parser type */
    fields[fields_num].type = BCMI_PT_PORT_PARSER_TYPE;
    fields[fields_num].data.n =
        IS_LB_PORT(unit, port) ? PARSER_TYPE_LOOPBACK : PARSER_TYPE_ETHERNET;
    fields_num ++;

    /* internal loopback */
    fields[fields_num].type = BCMI_PT_LOOPBACK_EN;
    u64[0] = IS_LB_PORT(unit, port) ? 1 : 0;
    fields[fields_num].data.a.buf = &u64[0];
    fields[fields_num].data.a.size = 1;
    fields[fields_num].data.a.idx = port;
    fields_num ++;

    /* cpu */
    fields[fields_num].type = BCMI_PT_CPU_EN;
    u64[1] = IS_CPU_PORT(unit, port) ? 1 : 0;
    fields[fields_num].data.a.buf = &u64[1];
    fields[fields_num].data.a.size = 1;
    fields[fields_num].data.a.idx = port;
    fields_num ++;

    /* default VLAN */
    fields[fields_num].type = BCMI_PT_DEFAULT_VFI;
    fields[fields_num].data.n = BCM_VLAN_DEFAULT;
    fields_num ++;

    /* VLAN map to VFI */
    fields[fields_num].type = BCMI_PT_VLAN_MAP_TO_VFI;
    fields[fields_num].data.n = 1;
    fields_num ++;

    /* only preserve itag */
    fields[fields_num].type = BCMI_PT_VLAN_PRESERVE;
    fields[fields_num].data.n = 0x2;
    fields_num ++;

    /* learn and forward */
    fields[fields_num].type = BCMI_PT_CML_NEW;
    fields[fields_num].data.n = 0x4;
    fields_num ++;
    fields[fields_num].type = BCMI_PT_CML_MOVE;
    fields[fields_num].data.n = 0x4;
    fields_num ++;

    /* drop static move */
    fields[fields_num].type = BCMI_PT_STATIC_MOVE_DROP;
    fields[fields_num].data.n = 1;
    fields_num ++;

    /* egress pruning */
    fields[fields_num].type = BCMI_PT_EGR_L2_PROCESS_CTRL;
    fields[fields_num].data.n = 0x2;
    fields_num ++;

    pp_pipe = bcmi_ltsw_dev_logic_port_pp_pipe(unit, port);
    pp_port = bcmi_ltsw_dev_logic_port_pp_pipe_port(unit, port);

    /* ifp pbmp index */
    fields[fields_num].type = BCMI_PT_IFP_PBM_INDEX;
    fields[fields_num].data.n = pp_port;
    fields_num ++;

    /* flex stats for LB-ECN */
    fields[fields_num].type = BCMI_PT_EGR_OPAQUE_OBJ0;
    fields[fields_num].data.n = (pp_pipe << 6) | (pp_port & 0x3F);
    fields_num ++;

    /* VLAN PHB strength */
    fields[fields_num].type = BCMI_PT_PHB_VLAN_STR;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_fld_value_get
            (unit, BCMI_LTSW_SBR_TH_EGR_L2_OIF,
             BCMI_LTSW_SBR_FT_DOT1P_PTR, &strength));
    fields[fields_num].data.n = strength;
    fields_num ++;

    if (fields_num > fields_num_max) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, fields_num));

exit:
    SHR_FREE(fields);
    SHR_FUNC_EXIT();
}


/*!
 * \brief De-initialize basic port pipeline configuration for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_port_service_deinit_per_port(int unit, int port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_l2_if_del(unit, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_sys_port_del(unit, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_lt_del(unit, port));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Initialize basic port pipeline configuration.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_port_service_init(int unit)
{
    int port;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_strength_init(unit));

    PORT_ALL_ITER(unit, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56880_a0_ltsw_port_service_init_per_port(unit, port));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_gih_cpu_init(unit));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief De-initialize basic port pipeline configuration.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_port_service_deinit(int unit)
{
    int port;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_gih_cpu_deinit(unit));

    PORT_ALL_ITER(unit, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56880_a0_ltsw_port_service_deinit_per_port(unit, port));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_strength_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Initialize port misc configuration.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_port_misc_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_misc_init(unit));

    /* encap init */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_encap_init(unit));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Validate egress shaper settings for TD4.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] bandwidth Port bandwidth in kbps or pps. 0 means shaper is disabled.
 * \param [in] burst Maximum burst size in kbits or packets.
 * \param [in] mode Byte mode or packet mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56880_a0_ltsw_port_egr_shaper_validate(int unit, bcm_port_t port,
                                          uint32_t bandwidth, uint32_t burst,
                                          bcmi_ltsw_port_shaper_mode_t mode)
{
    if ((bandwidth == 0) || (burst == 0)) {
        return SHR_E_NONE;
    } else if (mode == SHAPER_MODE_BYTE) {
        if ((bandwidth < PORT_RATE_MIN_KBPS) || (bandwidth > PORT_RATE_MAX_KBPS)
            || (burst < PORT_BURST_MIN_KBIT) || (burst > PORT_BURST_MAX_KBIT)) {
            return SHR_E_PARAM;
        } else {
            return SHR_E_NONE;
        }
    } else if (mode == SHAPER_MODE_PKT) {
        if ((bandwidth < PORT_RATE_MIN_PPS) || (bandwidth > PORT_RATE_MAX_PPS)
            || (burst < PORT_BURST_MIN_PKT) || (burst > PORT_BURST_MAX_PKT)) {
            return SHR_E_PARAM;
        } else {
            return SHR_E_NONE;
        }
    } else {
        return SHR_E_PARAM;
    }
}


/*!
 * \brief Get the spec of port module resources.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Resource type.
 * \param [out] spec Spec of identified resource.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56880_a0_ltsw_port_resource_spec_get(
    int unit,
    bcmi_ltsw_port_resource_type_t type,
    const bcmi_ltsw_port_resource_spec_t **spec)
{
    SHR_FUNC_ENTER(unit);

    if ((type < 0) || (type >= BCMI_PORT_RESOURCE_CNT)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *spec = &(chip_spec[unit]->resource[type]);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Validate port class settings for TD4.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] class Classification type.
 * \param [in] class_id Class ID of the port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56880_a0_ltsw_port_class_validate(int unit, bcm_port_t *port,
                                     bcm_port_class_t class, uint32_t class_id)
{
    int min, max;

    SHR_FUNC_ENTER(unit);

    switch (class) {
        case bcmPortClassIngress:
        case bcmPortClassEgress:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, *port, port));
            if (class_id != CLASS_ID_INVALID) {
                min = chip_spec[unit]->resource[BCMI_PORT_GROUP_L2_IF].min;
                max = chip_spec[unit]->resource[BCMI_PORT_GROUP_L2_IF].max;
                if ((class_id > max) || (class_id < min)) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
            break;

        default:
            if (!BCM_GPORT_IS_MODPORT(*port)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_gport_validate(unit, *port, port));
            }
            break;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Validate class id range for TD4.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] class Classification type.
 * \param [in] class_id Class ID of the port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56880_a0_ltsw_port_group_range_get(int unit, bcm_port_group_type_t type,
                                      bcm_port_group_range_t *range)
{
    SHR_FUNC_ENTER(unit);

    switch (type) {
        case bcmPortGroupEgressL2Interface:
            range->min = chip_spec[unit]->resource[BCMI_PORT_GROUP_L2_IF].min;
            range->max = chip_spec[unit]->resource[BCMI_PORT_GROUP_L2_IF].max;
            break;

        case bcmPortGroupDvp:
            range->min = chip_spec[unit]->resource[BCMI_PORT_GROUP_DVP].min;
            range->max = chip_spec[unit]->resource[BCMI_PORT_GROUP_DVP].max;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get L2 interface from port or gport.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Local logical port or gport.
 * \param [out] l2_if L2 interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm56880_a0_ltsw_gport_to_l2_if(int unit, int port, int *l2_if)
{
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_BLACK_HOLE(port)) {
        *l2_if = L2_IF_BLACK_HOLE;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_gport_to_l2_if(unit, port, l2_if));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get gport from L2 interface.
 *
 * \param [in] unit Unit Number.
 * \param [in] l2_if L2 interface.
 * \param [out] port Gport.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm56880_a0_ltsw_l2_if_to_gport(int unit, int l2_if, int *port)
{
    SHR_FUNC_ENTER(unit);

    if ((l2_if < 0) || (l2_if >= L2_IF_SIZE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else if (l2_if == L2_IF_BLACK_HOLE) {
        *port = BCM_GPORT_BLACK_HOLE;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_l2_if_to_gport(unit, l2_if, port));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Check that given lane setting is valid for physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_flexport_lane_validate(int unit,
                                        ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op, pport, lanes;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (po->op & FLEXPORT_OP_DETACH) {
            continue;
        }

        pport = po->pport;
        lanes = po->lanes;

        /* legal number of lanes */
        if ((lanes != 1) && (lanes != 2) && (lanes != 4) && (lanes != 8)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit, "Invalid number of lanes %d\n"),
                       lanes));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* lane alignment and not across pm */
        if ((pport - 1) % lanes != 0) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Invalid number of lanes %d for "
                                  "physical port %d\n"),
                       lanes, pport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Check that given speed is valid for the number of serdes lanes.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_flexport_speed_validate(int unit,
                                         ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op, core_clk, speed, speed_max;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    core_clk = bcmi_ltsw_dev_core_clk_freq(unit);
    speed_max = (core_clk < 1325) ? 200000 : 400000;

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (po->op & FLEXPORT_OP_DETACH) {
            continue;
        }

        speed = po->speed;

        if (speed > speed_max) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Core clk %dMHz doesn't support "
                                  "%dG port\n"),
                       core_clk, speed / 1000));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Check that given encap is valid for port.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_flexport_encap_validate(int unit,
                                         ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op, encap;
    ltsw_flexport_port_cfg_t *po;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (po->op & FLEXPORT_OP_DETACH) {
            continue;
        }

        encap = po->encap;

        if ((encap != BCM_PORT_ENCAP_IEEE) &&
            (encap != BCM_PORT_ENCAP_HIGIG3)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Invalid encap mode %d on port %d\n"),
                       encap, po->lport));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Check that number of ports does not exceed maximum allowed.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56883_a0_ltsw_flexport_ports_max_validate(int unit,
                                             ltsw_flexport_resource_t *resource)
{
    int i, pp, num_port = resource->num_port;
    int num_port_pp[MAX_NUM_PP_PIPE] = {0};
    int num_port_pp_max = chip_spec[unit]->max_num_port_pp;
    ltsw_flexport_port_info_t *pn;

    SHR_FUNC_ENTER(unit);

    if (!num_port_pp_max) {
        SHR_EXIT();
    }

    for (i = 0, pn = resource->new; i < num_port; i ++, pn ++) {
        if (pn->valid && (PORT(unit, i).port_type & BCMI_LTSW_PORT_TYPE_CD)) {
            pp = pport_to_pp(pn->pport);
            num_port_pp[pp] ++;
            if (num_port_pp[pp] > num_port_pp_max) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "PP %d has too many logical ports: %d\n"),
                           pp, num_port_pp[pp]));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static int
bcm56880_a0_ltsw_pm_port_rearrange(int unit, int nport, ltsw_flexport_port_cfg_t **port_cfg_array)
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

    for (i = nport - 1, psrc = NULL; i >= 0; i--) {
        po = port_cfg_array[i];
        if (po->op & FLEXPORT_OP_DETACH) {
            break;
        }

        if (po->op & FLEXPORT_OP_ATTACH_SECOND) {
            psrc = po;
        }
    }

    if (psrc) {
        pdst = port_cfg_array[i+2];
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
bcm56880_a0_ltsw_flexport_rearrange(int unit,
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
            (bcm56880_a0_ltsw_pm_port_rearrange(unit, pm->num_op, pm->operation));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Check flexport input parameters and port status are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_flexport_input_validate(int unit,
                                         ltsw_flexport_resource_t *resource)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_flexport_operation_order_validate(unit, resource));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_flexport_port_eligible_validate(unit, resource));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_flexport_port_status_validate(unit, resource));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_ltsw_flexport_lane_validate(unit, resource));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_ltsw_flexport_speed_validate(unit, resource));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_ltsw_flexport_encap_validate(unit, resource));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_flexport_pipe_validate(unit, resource));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56883_a0_ltsw_flexport_ports_max_validate(unit, resource));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_flexport_lane_config_validate(unit, resource));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_flexport_link_training_validate(unit, resource));

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
bcm56880_a0_ltsw_flexport_resource_validate(int unit,
                                            ltsw_flexport_resource_t *resource)
{
    int i;
    ltsw_flexport_pm_cfg_t *pm;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_flexport_linkscan_validate(unit, resource));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_flexport_port_mapping_validate(unit, resource));

    for (i = 0, pm = resource->pm; i <= MAX_NUM_FP_PM; i ++, pm ++) {
        if (pm->valid) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_ltsw_flexport_pm_validate
                    (unit, &pm->pbm, pm->num_op, pm->operation));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_ltsw_flexport_rearrange(unit, resource));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Attach ports to the BCM layer and initialize them to the default state.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_flexport_resource_attach(int unit,
                                          ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op, port;
    ltsw_flexport_port_cfg_t *po = NULL;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (po->op & FLEXPORT_OP_DETACH) {
            continue;
        }

        port = po->lport;

        if (po->op & (FLEXPORT_OP_ADD | FLEXPORT_OP_REMAP | FLEXPORT_OP_SPEED)) {
            /* port info init */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_software_init_per_port(unit, port));
        }
        if (po->op & (FLEXPORT_OP_ADD | FLEXPORT_OP_REMAP)) {
            /* port init */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_ltsw_port_service_init_per_port(unit, port));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_ltsw_port_profile_init_per_port(unit, port));
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_ltsw_port_misc_init_per_port(unit, port));

            /* port phb */
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_ltsw_port_attach_phb(unit, port));

            /* hg3 */

            /* vlan */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_stg_port_attach(unit, port));

            /* vlan */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_port_attach(unit, port));

            /* cosq */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_port_attach(unit, port));

            /* qos */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_qos_port_attach(unit, port));

            /* stack */

            /* rate */

            /* field */

            /* mirror */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_mirror_port_attach(unit, port));

            /* mpls */

        }

        if (po->op & (FLEXPORT_OP_ADD | FLEXPORT_OP_REMAP | FLEXPORT_OP_LANES |
                      FLEXPORT_OP_PC_RESET)) {
            /* Link */
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_link_port_control_attach(unit, port));

            /* pm init */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_pm_init_per_port(unit, port));

            /* stat */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_stat_port_attach(unit, port));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Detach ports from the BCM layer.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_flexport_resource_detach(int unit,
                                          ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op, port;
    ltsw_flexport_port_cfg_t *po = NULL;

    SHR_FUNC_ENTER(unit);

    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (!(po->op & FLEXPORT_OP_DETACH)) {
            continue;
        }

        port = po->lport;

        if (po->op & (FLEXPORT_OP_DEL | FLEXPORT_OP_REMAP | FLEXPORT_OP_LANES |
                      FLEXPORT_OP_PC_RESET)) {
            /* Link */
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_link_port_control_detach(unit, port));

            /* stat */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_stat_port_detach(unit, port));
        }
        if (po->op & (FLEXPORT_OP_DEL | FLEXPORT_OP_REMAP)) {

            /* mpls */

            /* mirror */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_mirror_port_detach(unit, port));

            /* field */

            /* rate */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_rate_clear_per_port(unit, port));

            /* stack */

            /* qos */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_qos_port_detach(unit, port));

            /* cosq */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_port_detach(unit, port));

            /* l2 */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l2_port_detach(unit, port));

            /* vlan */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_port_detach(unit, port));

            /* vlan */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_stg_port_detach(unit, port));

            /* hg3 */

            /* port phb */
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_ltsw_port_detach_phb(unit, port));

            /* port deinit */
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_ltsw_port_misc_deinit_per_port(unit, port));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_ltsw_port_profile_deinit_per_port(unit, port));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_ltsw_port_service_deinit_per_port(unit, port));
        }

        if (po->op & (FLEXPORT_OP_DEL | FLEXPORT_OP_REMAP | FLEXPORT_OP_SPEED )) {
            /* port info deinit */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_software_deinit_per_port(unit, port));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Update the port resource status to depedent modules after flex operation.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcm56880_a0_ltsw_flexport_resource_status_update(int unit,
                                                 ltsw_flexport_resource_t *resource)
{
    int i, num_op = resource->num_op;
    bcm_pbmp_t add_pbm, del_pbm;
    ltsw_flexport_port_cfg_t *po = NULL;

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(add_pbm);
    BCM_PBMP_CLEAR(del_pbm);
    for (i = 0, po = resource->operation; i < num_op; i ++, po ++) {
        if (po->op & (FLEXPORT_OP_ADD | FLEXPORT_OP_DEL | FLEXPORT_OP_REMAP)) {
            if (po->op & FLEXPORT_OP_DETACH) {
                BCM_PBMP_PORT_ADD(del_pbm, po->lport);
            } else {
                BCM_PBMP_PORT_ADD(add_pbm, po->lport);
            }
        }
    }

    /* to be replaced by module functions */
    SHR_ERR_EXIT(
        bcmi_ltsw_field_flex_port_update(unit, del_pbm, add_pbm));

exit:
    SHR_FUNC_EXIT();
}


int
bcm56880_a0_ltsw_port_subsidiary_ports_get(int unit, bcm_port_t port,
                                           bcm_pbmp_t *pbmp)
{
    int p, pport, pport_min, pport_max;

    SHR_FUNC_ENTER(unit);

    /* only for flexible and remappable ports */
    if ((PORT(unit, port).flags & (FLEXIBLE | NO_REMAP)) != FLEXIBLE) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    pport_min = PORT(unit, port).pport;
    /* only 0,2,4,6 in a PM has subsidiary ports, note pport start from 1 */
    if (!(pport_min % 2)) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }
    pport_max = (pport_min + 7) & (~7);

    BCM_PBMP_CLEAR(*pbmp);
    PORT_FP_ITER(unit, p) {
        pport = PORT(unit, p).pport;
        if ((pport >= pport_min) && (pport <= pport_max)) {
            BCM_PBMP_PORT_ADD(*pbmp, p);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_ltsw_original_tvco_rearrange(int unit, int nport, ltsw_flexport_port_cfg_t **port_cfg_array, int original_tvco)
{
    int i;
    ltsw_flexport_port_cfg_t *po, *psrc, *pdst, tmp;
    ltsw_port_vco_type_t vco = ltswPortVCOInvalid;
    int vco_20g_index = -1, original_tvco_index = -1;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < nport; i++) {
        po = port_cfg_array[i];
        if (po->op & (FLEXPORT_OP_ATTACH_FIRST | FLEXPORT_OP_ATTACH_SECOND)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56880_a0_fs_ability_table_vco_get(po->speed, po->lanes, po->fec, &vco));
            if (vco == ltswPortVCOInvalid) {
                continue;
            }

            if ((original_tvco == ltswPortVCO25P781G)
                && (vco == ltswPortVCO20P625G)) {
                if (vco_20g_index == -1) {
                    vco_20g_index = i;
                }
            }

            if (original_tvco == vco) {
                if (original_tvco_index == -1) {
                    original_tvco_index = i;
                }
            }
        }
    }

    psrc = NULL;
    /*If 25G is original tvco and 20G vco is existed, the 20G vco port should be inserted in PM first,
    which is a default rule for PM8x50 core*/
    if (vco_20g_index != -1) {
        psrc = port_cfg_array[vco_20g_index];
    } else if (original_tvco_index != -1) {
        psrc = port_cfg_array[original_tvco_index];
    }

    if (psrc) {
        pdst = port_cfg_array[0];
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

int
bcm56880_a0_ltsw_pm_port_init_adjust(int unit, int nport, ltsw_flexport_port_cfg_t* ltsw_port_resource, int original_tvco)
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
        (bcm56880_a0_ltsw_flexport_pm_validate(unit, &current_pbm, nport, operation));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_ltsw_pm_port_rearrange(unit, nport, operation));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_ltsw_original_tvco_rearrange(unit, nport, operation, original_tvco));

exit:
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_port_drv_t bcm56880_a0_ltsw_port_drv = {
    .port_info_init                 = bcm56880_a0_ltsw_port_info_init,
    .port_info_init_per_port        = bcm56880_a0_ltsw_port_info_init_per_port,
    .port_info_deinit               = bcm56880_a0_ltsw_port_info_deinit,
    .port_info_recover              = xfs_ltsw_port_info_recover,
    .port_profile_init              = bcm56880_a0_ltsw_port_profile_init,
    .port_profile_deinit            = bcm56880_a0_ltsw_port_profile_deinit,
    .port_service_init              = bcm56880_a0_ltsw_port_service_init,
    .port_service_deinit            = bcm56880_a0_ltsw_port_service_deinit,
    .port_misc_init                 = bcm56880_a0_ltsw_port_misc_init,
    .port_misc_deinit               = xfs_ltsw_port_misc_deinit,
    .port_control_get               = xfs_ltsw_port_control_get,
    .port_control_set               = xfs_ltsw_port_control_set,
    .port_tabs_info_get             = xfs_ltsw_port_tabs_info_get,
    .port_tabs_op_get               = xfs_ltsw_port_tabs_op_get,
    .port_tabs_para_get             = xfs_ltsw_port_tabs_para_get,
    .l2_if_alloc                    = xfs_ltsw_l2_if_alloc,
    .l2_if_free                     = xfs_ltsw_l2_if_free,
    .l2_if_dump                     = xfs_ltsw_l2_if_dump,
    .l2_if_add                      = xfs_ltsw_l2_if_add,
    .l2_if_del                      = xfs_ltsw_l2_if_del,
    .l2_if_to_port                  = bcm56880_a0_ltsw_l2_if_to_gport,
    .port_to_l2_if                  = bcm56880_a0_ltsw_gport_to_l2_if,
    .l2_if_mask_to_port_mask        = xfs_ltsw_l2_if_mask_to_port_mask,
    .port_mask_to_l2_if_mask        = xfs_ltsw_port_mask_to_l2_if_mask,
    .l2_if_to_pipes                 = xfs_ltsw_l2_if_to_pipes,
    .sys_port_add                   = xfs_ltsw_sys_port_add,
    .sys_port_del                   = xfs_ltsw_sys_port_del,
    .port_sys_port_move             = xfs_ltsw_port_sys_port_move,
    .sys_port_to_port               = xfs_ltsw_sys_port_to_gport,
    .port_to_sys_port               = xfs_ltsw_gport_to_sys_port,
    .port_to_sys_ports              = xfs_ltsw_gport_to_sys_ports,
    .sys_port_map_l2_if             = xfs_ltsw_sys_port_map_l2_if,
    .sys_port_demap_l2_if           = xfs_ltsw_sys_port_demap_l2_if,
    .sys_port_traverse              = xfs_ltsw_sys_port_traverse,
    .port_learn_get                 = xfs_ltsw_port_learn_get,
    .port_learn_set                 = xfs_ltsw_port_learn_set,
    .port_l3_mtu_set                = xfs_ltsw_port_l3_mtu_set,
    .port_l3_mtu_get                = xfs_ltsw_port_l3_mtu_get,
    .port_tpid_set                  = xfs_ltsw_port_tpid_set,
    .port_tpid_add                  = xfs_ltsw_port_tpid_add,
    .port_tpid_get                  = xfs_ltsw_port_tpid_get,
    .port_tpid_get_all              = xfs_ltsw_port_tpid_get_all,
    .port_tpid_delete               = xfs_ltsw_port_tpid_delete,
    .port_tpid_delete_all           = xfs_ltsw_port_tpid_delete_all,
    .port_inner_tpid_get            = xfs_ltsw_port_inner_tpid_get,
    .port_inner_tpid_set            = xfs_ltsw_port_inner_tpid_set,
    .port_untagged_vlan_get         = xfs_ltsw_port_untagged_vlan_get,
    .port_untagged_vlan_set         = xfs_ltsw_port_untagged_vlan_set,
    .port_untagged_priority_get     = xfs_ltsw_port_untagged_priority_get,
    .port_untagged_priority_set     = xfs_ltsw_port_untagged_priority_set,
    .port_force_vlan_set            = xfs_ltsw_port_force_vlan_set,
    .port_force_vlan_get            = xfs_ltsw_port_force_vlan_get,
    .port_ing_vlan_action_set       = xfs_ltsw_port_ing_vlan_action_set,
    .port_ing_vlan_action_get       = xfs_ltsw_port_ing_vlan_action_get,
    .port_ing_vlan_action_reset     = xfs_ltsw_port_ing_vlan_action_reset,
    .port_egr_vlan_xlate_enable_set = xfs_ltsw_port_egr_vlan_xlate_enable_set,
    .port_egr_vlan_xlate_enable_get = xfs_ltsw_port_egr_vlan_xlate_enable_get,
    .port_class_get                 = xfs_ltsw_port_class_get,
    .port_class_set                 = xfs_ltsw_port_class_set,
    .port_resource_spec_get         = bcm56880_a0_ltsw_port_resource_spec_get,
    .port_class_validate            = bcm56880_a0_ltsw_port_class_validate,
    .port_group_range_get           = bcm56880_a0_ltsw_port_group_range_get,
    .port_group_to_pipes            = xfs_ltsw_port_group_to_pipes,
    .port_dscp_phb_map_mode_set     = xfs_ltsw_port_dscp_phb_map_mode_set,
    .port_dscp_phb_map_mode_get     = xfs_ltsw_port_dscp_phb_map_mode_get,
    .port_egr_shaper_set            = xfs_ltsw_port_egr_shaper_set,
    .port_egr_shaper_get            = xfs_ltsw_port_egr_shaper_get,
    .port_egr_shaper_validate       = bcm56880_a0_ltsw_port_egr_shaper_validate,
    .port_queue_count_get           = xfs_ltsw_port_queue_count_get,
    .flexport_input_validate        = bcm56880_a0_ltsw_flexport_input_validate,
    .flexport_resource_validate     = bcm56880_a0_ltsw_flexport_resource_validate,
    .port_speed_config_validate     = bcm56880_a0_ltsw_port_speed_config_validate,
    .flexport_resource_attach       = bcm56880_a0_ltsw_flexport_resource_attach,
    .flexport_resource_detach       = bcm56880_a0_ltsw_flexport_resource_detach,
    .flexport_resource_status_update= bcm56880_a0_ltsw_flexport_resource_status_update,
    .pm_port_init_adjust            = bcm56880_a0_ltsw_pm_port_init_adjust,
    .port_stat_attach               = xfs_ltsw_port_stat_attach,
    .port_stat_detach               = xfs_ltsw_port_stat_detach,
    .port_flexstate_attach          = xfs_ltsw_port_flexstate_attach,
    .port_flexstate_detach          = xfs_ltsw_port_flexstate_detach,
    .port_flood_block_set           = xfs_ltsw_port_flood_block_set,
    .port_flood_block_get           = xfs_ltsw_port_flood_block_get,
    .port_egress_set                = xfs_ltsw_port_egress_set,
    .port_egress_get                = xfs_ltsw_port_egress_get,
    .port_match_add                 = xfs_ltsw_port_match_add,
    .port_match_delete              = xfs_ltsw_port_match_delete,
    .port_match_multi_get           = xfs_ltsw_port_match_multi_get,
    .port_hg3_enable_set            = xfs_ltsw_port_hg3_enable_set,
    .port_hg3_enable_get            = xfs_ltsw_port_hg3_enable_get,
    .port_hg3_eth_type_set          = xfs_ltsw_port_hg3_eth_type_set,
    .port_hg3_eth_type_get          = xfs_ltsw_port_hg3_eth_type_get,
    .port_subsidiary_ports_get      = bcm56880_a0_ltsw_port_subsidiary_ports_get,
    .port_ifp_egr_vlan_check_enable_set = xfs_ltsw_port_ifp_egr_vlan_check_enable_set,
    .port_ifp_egr_vlan_check_enable_get = xfs_ltsw_port_ifp_egr_vlan_check_enable_get,
    .port_vlan_check_disable_get    = xfs_ltsw_port_vlan_check_disable_get,
    .port_stg_check_disable_get     = xfs_ltsw_port_stg_check_disable_get,
    .port_dump_sw                   = xfs_ltsw_port_dump_sw,
};

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_ltsw_port_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_drv_set(unit, &bcm56880_a0_ltsw_port_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_ltsw_variant_drv_attach(unit, BCM56880_A0_LTSW_VARIANT_PORT));

exit:
    SHR_FUNC_EXIT();
}
