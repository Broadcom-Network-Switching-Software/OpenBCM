/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        trident3.c
 * Purpose:
 * Requires:
 */


#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/dpc.h>
#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>
#include <soc/tdm/tomahawk/tdm_th_soc.h>
#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#include <soc/cmicx_miim.h>
#include <shared/cmicfw/iproc_m0ssq.h>
#endif

#include <soc/bradley.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/i2c.h>
#include <soc/l2x.h>
#include <soc/mcm/intr_iproc.h>

#ifdef BCM_TRIDENT3_SUPPORT

#include <soc/mmu_config.h>
#include <soc/trident3.h>
#include <soc/trident3_tdm.h>
#include <soc/tomahawk.h>
#include <soc/esw/cancun.h>
#include <soc/soc_ser_log.h>
#include <soc/pstats.h>
#include <soc/scache.h>
#include <soc/flexport/trident3/trident3_flexport_defines.h>

/* MDIO Clock Frquency for TD3 is 250 MHz
 * Set external MDIO freq to around 5 MHz
 * Hence divisor is set to 50
 * HR4/FB6 its set to 20 as per HW design
 */
#define TD3X_RATE_EXT_MDIO_DIVISOR_DEF(unit)       \
          ((SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) ? 20 : 50)

/*
 * Set internal MDIO freq to around 10 MHz
 * Valid range is from 2.5MHz to 20MHz
 * Hence divisor is set to 25.
 * HR4/FB6 its set to 20 as per HW design
 */
#define TD3X_RATE_INT_MDIO_DIVISOR_DEF(unit)       \
          ((SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) ? 20 : 25)

#define _TDM_LENGTH                    256
#define _OVS_GROUP_COUNT               6
#define _OVS_GROUP_TDM_LENGTH          12
#define TD3_NUM_PHY_PORT                (132)
#define TD3_NUM_PORT                    (132)
#define TD3_NUM_MMU_PORT                (226)

/* Value hardcoded in set_tdm.c, definition needs to be matched */
#define PORT_STATE_UNUSED              0
#define PORT_STATE_LINERATE            1
#define PORT_STATE_OVERSUB             2
#define PORT_STATE_CONTINUATION        3 /* part of other port */

#define PORT_BLOCK_BASE_PORT(port) \
        (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0x3));

/* IUAT number of shared banks */

#define SOC_IUAT_8K_BANKS              4
#define SOC_IUAT_16K_BANKS             4

#define IS_MGMT_PORT(unit,port) IS_MANAGEMENT_PORT(unit, port)

static int num_shared_alpm_banks[SOC_MAX_NUM_DEVICES] = {8};
static soc_mem_t *_soc_td3_alpm_bkt_view_map[SOC_MAX_NUM_DEVICES];
static int8 *_soc_td3_alpm2_bnk_fmt_map[SOC_MAX_NUM_DEVICES];
int
_soc_td3_port_speed_cap[SOC_MAX_NUM_DEVICES][TRIDENT3_PHY_PORTS_PER_DEV];

typedef struct _soc_td3_mmu_traffic_ctrl_s {
    uint8   thdo_drop_bmp[TD3_NUM_PORT];
} _soc_td3_mmu_traffic_ctrl_t;

#define   MMU_TRAFFIC_EN_CTRL           (1 << 1)
#define   PORT_TX_EN_CTRL               (1 << 2)
static    _soc_td3_mmu_traffic_ctrl_t
              *_soc_td3_mmu_traffic_ctrl[SOC_MAX_NUM_DEVICES];
static    sal_mutex_t _fwd_ctrl_lock[SOC_MAX_NUM_DEVICES];
#define   FWD_CTRL_LOCK(unit) \
          sal_mutex_take(_fwd_ctrl_lock[unit], sal_mutex_FOREVER)
#define   FWD_CTRL_UNLOCK(unit) \
          sal_mutex_give(_fwd_ctrl_lock[unit])

int soc_td3_rx_etype_niv[SOC_MAX_NUM_DEVICES];
int soc_td3_rx_etype_pe[SOC_MAX_NUM_DEVICES];

int
_soc_trident3_init_mmu_memory(int unit);
int
soc_trident3_init_alpm2_memory(int unit);
/* Externs */
extern int soc_trident3_get_alpm_banks(int unit);
extern int soc_td3_hash_mutex_init (int unit);

static const soc_reg_t pvtmon_result_reg[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r,
    TOP_PVTMON_RESULT_2r, TOP_PVTMON_RESULT_3r,
    TOP_PVTMON_RESULT_4r, TOP_PVTMON_RESULT_5r,
    TOP_PVTMON_RESULT_6r, TOP_PVTMON_RESULT_7r,
    TOP_PVTMON_RESULT_8r, TOP_PVTMON_RESULT_9r,
    TOP_PVTMON_RESULT_10r, TOP_PVTMON_RESULT_11r
};

#ifdef BCM_MAVERICK2_SUPPORT
static const soc_reg_t pvtmon_result_reg_mv2[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r,
    TOP_PVTMON_RESULT_2r, TOP_PVTMON_RESULT_3r,
    TOP_PVTMON_RESULT_4r, TOP_PVTMON_RESULT_5r,
    TOP_PVTMON_RESULT_6r, TOP_PVTMON_RESULT_7r,
    TOP_PVTMON_RESULT_8r, TOP_PVTMON_RESULT_11r
};
#endif

egr_vlan_xlate_1_hash_control_entry_t       egr_vlan_1_hash_control;
egr_vlan_xlate_2_hash_control_entry_t       egr_vlan_2_hash_control;
egr_vp_vlan_membership_hash_control_entry_t egr_vp_vlan_mbm_hash_control;
exact_match_hash_control_entry_t            exact_match_hash_control;
ing_dnat_address_type_hash_control_entry_t  ing_dnat_hash_control;
ing_vp_vlan_membership_hash_control_entry_t ing_vp_vlan_mbm_hash_control;
l2_entry_hash_control_entry_t               l2_entry_hash_control;
l3_entry_hash_control_entry_t               l3_entry_hash_control;
mpls_entry_hash_control_entry_t             mpls_entry_hash_control;
subport_id_to_sgpp_map_hash_control_entry_t subport_hash_control;
vlan_xlate_1_hash_control_entry_t           vlan_1_hash_control;
vlan_xlate_2_hash_control_entry_t           vlan_2_hash_control;
uft_shared_banks_control_entry_t            uft_shared_banks_control;
ip_uat_16k_shared_banks_control_entry_t     ip_uat_16k_shared_banks_control;
ip_uat_8k_shared_banks_control_entry_t      ip_uat_8k_shared_banks_control;
ep_uat_shared_banks_control_entry_t         ep_uat_shared_banks_control;

int
soc_trident3_pvt_intr_enable(int unit)
{
#ifdef BCM_CMICX_SUPPORT
    uint32 rval;

    /* Clear PVT INTR latch bit if any */
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_INTR_STATUSr(unit, 0xffffffff));

    if (soc_feature(unit, soc_feature_cmicx)) {
        /* Enable PVTMON interrupt in CMIC */
        SOC_IF_ERROR_RETURN(soc_iproc_getreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                            REG_PORT_ANY, 0), &rval));
        rval |= 0x4;
        SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                            REG_PORT_ANY, 0), rval));
    }
#endif

    return SOC_E_NONE;

}

int
soc_trident3_pvt_intr_disable(int unit)
{
#ifdef BCM_CMICX_SUPPORT
    uint32 rval;

    if (soc_feature(unit, soc_feature_cmicx)) {
        /* Disable PVTMON interrupt in CMIC */
        SOC_IF_ERROR_RETURN(soc_iproc_getreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                            REG_PORT_ANY, 0), &rval));
        rval &= ~0x4;
        SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                            REG_PORT_ANY, 0), rval));
    }

    /* Clear PVT INTR latch bit if any */
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_INTR_STATUSr(unit, 0xffffffff));
#endif

    return SOC_E_NONE;
}

int
soc_trident3_init_alpm2_memory(int unit)
{
    if (_soc_td3_alpm2_bnk_fmt_map[unit] != NULL) {
        sal_free(_soc_td3_alpm2_bnk_fmt_map[unit]);
        _soc_td3_alpm2_bnk_fmt_map[unit] = NULL;
    }

    if (NULL == _soc_td3_alpm2_bnk_fmt_map[unit]) {
        if ((_soc_td3_alpm2_bnk_fmt_map[unit] = sal_alloc(sizeof(int8) * \
            soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm), "alpm_bnt_map")) \
            == NULL) {
            return SOC_E_MEMORY;
        }
    }

    sal_memset(_soc_td3_alpm2_bnk_fmt_map[unit], 0,
               sizeof(int8) * soc_mem_index_count(unit, L3_DEFIP_ALPM_RAWm));
    return SOC_E_NONE;
}

int
soc_td3_post_mmu_init_update(int unit)
{
    int port;
    soc_info_t *si = &SOC_INFO(unit);

    PBMP_ALL_ITER(unit, port) {
        /* Set init speed to max speed by default */
        si->port_init_speed[port] = si->port_speed_max[port];
        if (_soc_td3_port_speed_cap[unit][port]) {
            /* If cap speed is available then adjust max speed for further use */
            si->port_speed_max[port] = _soc_td3_port_speed_cap[unit][port];
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_trident3_iuat_sizing
 * Purpose:
 *      Given number of entries for IUAT memory, determine number of 8k and 16k
 *      IUAT banks (already allocated + shared banks for requesting module)
 * Parameters:
 *      unit (IN)         - switch unit
 *      iuat entries (IN) - number of memory entries
 *      iuat_16k (IN/OUT) - total number of allocated 16k banks
 *      iuat_8k (IN/OUT)  - total number of allocated 8k banks
 * Returns:
 *      SOC_E_*
 */
int
soc_trident3_iuat_sizing(int unit, int iuat_entries, int *iuat_16k, int *iuat_8k)
{
    if (iuat_entries == 0) {
        return SOC_E_NONE;
    } else if (iuat_entries <= (8 *1024)) {
        /* Minimum hash table size should be > 8k */
        return SOC_E_PARAM;
    }

    /* Recurse for iuat_entries > 32k */
    while (iuat_entries > (32 * 1024)) {
        int residual_entries = 32 * 1024;
        SOC_IF_ERROR_RETURN
            (soc_trident3_iuat_sizing(unit, residual_entries, iuat_16k, iuat_8k));
        iuat_entries -= residual_entries;
    }

    /* Allocation Policy - Minimum Number of banks to be allocated is 2
     * Preferable if banks allocated are of the same size
     */

    /* Depending on number of iuat entries, there are several allocation schemes
     * possible. They are listed in order of priority. If none of them are
     * possible, error is returned.
     */

    /* This case works only when iuat_entries > 32k and recursion has happened */
    if(iuat_entries <= 8 * 1024) {
        if ((*iuat_8k + 1) <= SOC_IUAT_8K_BANKS) {
            *iuat_8k += 1;
        } else {
            return SOC_E_PARAM;
        }
    } else if (iuat_entries <= 16 * 1024) {
       /* For iuat entries in range 1 to (16 * 1024)
        * 1. Allocate two 8k banks
        */
        if ((*iuat_8k + 2) <= SOC_IUAT_8K_BANKS) {
            *iuat_8k += 2;
        } else {
            return SOC_E_PARAM;
        }
    } else if (iuat_entries <= 24 * 1024) {
        /* For iuat entries in range (16 * 1024) + 1 to 24 * 1024
         * 1. Allocate two 16k banks
         * 2. Allocate one 16k bank and one 8k bank
         * 3. Allocate three 8k banks
         */
        if ((*iuat_16k + 2) <= SOC_IUAT_16K_BANKS){
            *iuat_16k += 2;
        } else if (((*iuat_16k + 1) <= SOC_IUAT_16K_BANKS) &&
                    ((*iuat_8k + 1) <= SOC_IUAT_8K_BANKS)) {
            *iuat_8k += 1;
            *iuat_16k += 1;
        } else if ((*iuat_8k + 3) <= SOC_IUAT_8K_BANKS) {
            *iuat_8k += 3;
        } else {
            return SOC_E_PARAM;
        }
    } else { /* remaining case: (iuat_entries <= 32 * 1024) */
        /* For iuat entries in range (24 * 1024) + 1 to 32 * 1024,
         * 1. Allocate two 16k banks
         * 2. Allocate four 8k banks
         * 3. Allocate two 8k banks and one 16k bank.
         */
        if ((*iuat_16k + 2) <= SOC_IUAT_16K_BANKS) {
            *iuat_16k += 2;
        } else if ((*iuat_8k + 4) <= SOC_IUAT_8K_BANKS) {
            *iuat_8k += 4;
        } else if(((*iuat_8k + 2) <= SOC_IUAT_8K_BANKS) &&
                    (*iuat_16k + 1) <= SOC_IUAT_16K_BANKS) {
            *iuat_8k += 2;
            *iuat_16k += 1;
        } else {
            return SOC_E_PARAM;
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_trident3_mem_config
 * Purpose:
 *      Configure depth of UFT/UAT memories
 * Parameters:
 *      unit (IN)    - switch unit
 * Returns:
 *      SOC_E_*
 */
int
soc_trident3_mem_config(int unit)
{
    soc_persist_t *sop;
    int l2_entries, fixed_l2_entries, shared_l2_banks;
    int num_ipv6_128b_entries = 0;
    int config_v6_entries = 0;
    int defip_config = 0;
#define _SOC_TD3_DEFIP_MAX_TCAMS    8
#define _SOC_TD3_DEFIP_TCAM_DEPTH   2048
    int l3_entries, fixed_l3_entries, shared_l3_banks;
    int fpem_entries, shared_fpem_banks;
    int mpls_entries, vlan_xlate_1_entries, vlan_xlate_2_entries;
    int alpm_enable = 0;
    int mpls_16k_banks, mpls_8k_banks;
    int vlan_xlate_1_16k_banks, vlan_xlate_1_8k_banks;
    int vlan_xlate_2_16k_banks, vlan_xlate_2_8k_banks;
    int uat_16k_banks, uat_8k_banks;
    int egr_vlan_xlate_1_entries, egr_vlan_xlate_2_entries;
    int egr_vlan_xlate_1_banks, egr_vlan_xlate_2_banks;
    int num_lag_rh_flowset_entries;

    sop = SOC_PERSIST(unit);

    /* TD3 doesn't need TCAM atomicity WAR */
    SOC_CONTROL(unit)->tcam_protect_write = 0;
    SOC_CONTROL(unit)->tcam_protect_write_init = 0;

    /*  UFT BANK SIZING
     *
     * bank 0-1 are dedicated L2 banks (16k entries per bank)
     * bank 10-11 are dedicated L3 banks (8k entries per bank)
     * bank 2-9 are shared banks (32k L2/L3 or 16k FPEM entries
     *                              or 48k ALPM entries per bank)
     */

    l2_entries = soc_property_get(unit, spn_L2_MEM_ENTRIES, 32 * 1024);
    fixed_l2_entries = 32 * 1024;  /* 32k dedicated L2 entries, (16k * 2 banks) */
    if (l2_entries < fixed_l2_entries) {
        l2_entries = fixed_l2_entries;
        shared_l2_banks = 0;
    } else {
        l2_entries -= fixed_l2_entries;
        shared_l2_banks = (l2_entries + (32 * 1024 - 1)) / (32 * 1024);
        l2_entries = fixed_l2_entries + shared_l2_banks * 32 * 1024;
    }

    l3_entries = soc_property_get(unit, spn_L3_MEM_ENTRIES, 16 * 1024);
    fixed_l3_entries = 16 * 1024;  /* 16k dedicated L3 entries, (8k * 2 banks) */
    if (l3_entries < fixed_l3_entries) {
        l3_entries = fixed_l3_entries;
        shared_l3_banks = 0;
    } else {
        l3_entries -= fixed_l3_entries;
        shared_l3_banks = (l3_entries + (32 * 1024 - 1)) / (32 * 1024);
        l3_entries = fixed_l3_entries + shared_l3_banks * 32 * 1024;
    }

    fpem_entries = soc_property_get(unit, spn_FPEM_MEM_ENTRIES, 0);
    shared_fpem_banks = (fpem_entries + (16 * 1024 - 1)) / (16 * 1024);
    /* Allocate atleast 2 banks to FP for dual hash benefits */
    if(shared_fpem_banks == 1) {
        if ((shared_l2_banks + shared_l3_banks) > 6) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Config Error - Atleast two shared UFT"
                                    "banks should be allocated to FPEM. \n")));
            return SOC_E_PARAM;
        }
        shared_fpem_banks = 2;
    }
    fpem_entries = shared_fpem_banks * 16 * 1024;

    if (shared_l2_banks + shared_l3_banks + shared_fpem_banks > 8) {
        return SOC_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_alpm)) {
        alpm_enable = soc_property_get(unit, spn_L3_ALPM_ENABLE, 0);
    }

    /* ALPM needs either 4 banks or 8 banks depending on ALPM bank mode.
     * If more than 4 banks are already allocated to non ALPM tables (L2, L3,
     * FPEM), then ALPM needs cannot be satisfied */
    if ((alpm_enable) &&
        (shared_l2_banks + shared_l3_banks + shared_fpem_banks > 4)) {
        return SOC_E_PARAM;
    }

    _soc_alpm_mode[unit] = alpm_enable;

    /* IUAT BANK SIZING
     *
     * Banks 0-3 are shared banks of size 8k per bank
     * Banks 4-7 are shared banks of size 16k per bank
     */

    uat_16k_banks = 0; uat_8k_banks = 0;
    mpls_entries = soc_property_get(unit, spn_MPLS_MEM_ENTRIES, 32768);
    SOC_IF_ERROR_RETURN(soc_trident3_iuat_sizing(unit, mpls_entries,
                                                 &uat_16k_banks, &uat_8k_banks));
    mpls_16k_banks = uat_16k_banks;
    mpls_8k_banks = uat_8k_banks;
    mpls_entries = mpls_16k_banks * 16 * 1024 + mpls_8k_banks * 8 * 1024;

    vlan_xlate_1_entries = soc_property_get(unit, spn_VLAN_XLATE_1_MEM_ENTRIES,
                                            32768);
    SOC_IF_ERROR_RETURN(soc_trident3_iuat_sizing(unit, vlan_xlate_1_entries,
                                                 &uat_16k_banks, &uat_8k_banks));
    vlan_xlate_1_16k_banks = uat_16k_banks - mpls_16k_banks;
    vlan_xlate_1_8k_banks = uat_8k_banks - mpls_8k_banks;
    vlan_xlate_1_entries = vlan_xlate_1_16k_banks * 16 * 1024 +
                            vlan_xlate_1_8k_banks * 8 * 1024;

    vlan_xlate_2_entries = soc_property_get(unit, spn_VLAN_XLATE_2_MEM_ENTRIES,
                                            32768);
    SOC_IF_ERROR_RETURN(soc_trident3_iuat_sizing(unit, vlan_xlate_2_entries,
                                                 &uat_16k_banks, &uat_8k_banks));
    vlan_xlate_2_16k_banks = uat_16k_banks - mpls_16k_banks -
                                vlan_xlate_1_16k_banks;
    vlan_xlate_2_8k_banks = uat_8k_banks - mpls_8k_banks - vlan_xlate_1_8k_banks;
    vlan_xlate_2_entries = vlan_xlate_2_16k_banks * 16 * 1024 +
                            vlan_xlate_2_8k_banks * 8 * 1024;

    /* EUAT BANK SIZING
     *
     * Banks 0-3 are shared banks of size 8k per bank
     */

    egr_vlan_xlate_1_entries = soc_property_get(unit,
                                    spn_EGR_VLAN_XLATE_1_MEM_ENTRIES, 16384);
    egr_vlan_xlate_1_banks = (egr_vlan_xlate_1_entries + (8 * 1024 - 1)) /
                                                                    ( 8 * 1024);
    egr_vlan_xlate_1_entries = egr_vlan_xlate_1_banks * 8 * 1024;

    egr_vlan_xlate_2_entries = soc_property_get(unit,
                                    spn_EGR_VLAN_XLATE_2_MEM_ENTRIES, 16384);
    egr_vlan_xlate_2_banks = (egr_vlan_xlate_2_entries + (8 * 1024 - 1)) /
                                                                    ( 8 * 1024);
    egr_vlan_xlate_2_entries = egr_vlan_xlate_2_banks * 8 * 1024;

    if (egr_vlan_xlate_1_banks + egr_vlan_xlate_2_banks > 4) {
        return SOC_E_PARAM;
    }

    /* LP memory sizing
     * Memory Bank with 1W,2W,4W support - 3.5k entries
     * Memory Bank with 1W,2W support    - 3k entries
     * Memory Bank with 1W support       - 2k entries
     */

    /* Adjust L2 table size */
    sop->memState[L2Xm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_SINGLEm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_ONLY_SINGLEm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_ECCm].index_max = l2_entries - 1;
    sop->memState[L2_HITDA_ONLYm].index_max = l2_entries / 4 - 1;
    sop->memState[L2_HITSA_ONLYm].index_max = l2_entries / 4 - 1;
    /* 4k per dedicated L2 bank, 14k per shared L2 bank */
    sop->memState[L2_ENTRY_LPm].index_max = (2 * 4 * 1024) +
                                (shared_l2_banks * 14 * 1024) - 1;

    /* Adjust L3 table size */
    sop->memState[L3_ENTRY_SINGLEm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_ONLY_SINGLEm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_ECCm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_DOUBLEm].index_max = l3_entries / 2 - 1;
    sop->memState[L3_ENTRY_ONLY_DOUBLEm].index_max = l3_entries / 2 - 1;
    sop->memState[L3_ENTRY_QUADm].index_max = l3_entries / 4 - 1;
    sop->memState[L3_ENTRY_ONLY_QUADm].index_max = l3_entries / 4 - 1;
    sop->memState[L3_ENTRY_HIT_ONLYm].index_max = l3_entries / 4 - 1;
    /* 3.5k per dedicated L3 bank, 14k per shared L3 bank */
    /* 2 * 3.5 * 1024 is replaced by 7 * 1024 to avoid automatic type
     * conversion, which could force shared_l3_banks to double type */
    sop->memState[L3_ENTRY_LPm].index_max = (7 * 1024) +
                                   (shared_l3_banks * 14 * 1024) - 1;

    /* Adjust FP exact match table size */
    sop->memState[EXACT_MATCH_2m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_PIPE0m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_PIPE1m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLYm].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLY_PIPE0m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLY_PIPE1m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_4m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_PIPE0m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_PIPE1m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLYm].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLY_PIPE0m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLY_PIPE1m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_HIT_ONLYm].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_ECCm].index_max = fpem_entries * 2 - 1;
    sop->memState[EXACT_MATCH_ECC_PIPE0m].index_max = fpem_entries * 2 - 1;
    sop->memState[EXACT_MATCH_ECC_PIPE1m].index_max = fpem_entries * 2 - 1;
    /* 14k per shared FPEM bank */
    sop->memState[EXACT_MATCH_LPm].index_max =
                              shared_fpem_banks * 14 * 1024 - 1;
    sop->memState[EXACT_MATCH_LP_PIPE0m].index_max =
                              shared_fpem_banks * 14 * 1024 -1;
    sop->memState[EXACT_MATCH_LP_PIPE1m].index_max =
                              shared_fpem_banks * 14 * 1024 -1;

    /* Adjust MPLS table size */

    sop->memState[MPLS_ENTRY_SINGLEm].index_max = mpls_entries - 1;
    sop->memState[MPLS_ENTRYm].index_max = mpls_entries / 2 - 1;
    /* 3k per 8k bank and 6k per 8k bank */
    sop->memState[MPLS_ENTRY_LPm].index_max = (mpls_8k_banks * 3 * 1024 +
                                              mpls_16k_banks * 2 * 3 * 1024) - 1;
    sop->memState[MPLS_ENTRY_ECCm].index_max = mpls_entries - 1;

    /* Adjust VLAN_XLATE_1 table size */

    sop->memState[VLAN_XLATE_1_SINGLEm].index_max = vlan_xlate_1_entries - 1;
    sop->memState[VLAN_XLATE_1_DOUBLEm].index_max = vlan_xlate_1_entries / 2 - 1;
    /* 3k per 8k bank and 6k per 8k bank */
    sop->memState[VLAN_XLATE_1_LPm].index_max = ((vlan_xlate_1_16k_banks * 2 * 3 * 1024)
                                                + (vlan_xlate_1_8k_banks * 3 * 1024)) - 1;
    sop->memState[VLAN_XLATE_1_ECCm].index_max = vlan_xlate_1_entries - 1;

    /* Adjust VLAN_XLATE_2 table size */

    sop->memState[VLAN_XLATE_2_SINGLEm].index_max = vlan_xlate_2_entries - 1;
    sop->memState[VLAN_XLATE_2_DOUBLEm].index_max = vlan_xlate_2_entries / 2 - 1;
    /* 3k per 8k bank and 6k per 8k bank */
    sop->memState[VLAN_XLATE_2_LPm].index_max = (vlan_xlate_2_16k_banks * 2 * 3 * 1024)
                                                + (vlan_xlate_2_8k_banks * 3 * 1024) - 1;
    sop->memState[VLAN_XLATE_2_ECCm].index_max = vlan_xlate_2_entries - 1;

    /* Adjust EGR_VLAN_XLATE_1 table size */

    sop->memState[EGR_VLAN_XLATE_1_SINGLEm].index_max = egr_vlan_xlate_1_entries - 1;
    sop->memState[EGR_VLAN_XLATE_1_DOUBLEm].index_max = egr_vlan_xlate_1_entries / 2 - 1;
    /* 3k per bank */
    sop->memState[EGR_VLAN_XLATE_1_LPm].index_max = egr_vlan_xlate_1_banks * 3 * 1024 - 1;
    sop->memState[EGR_VLAN_XLATE_1_ECCm].index_max = egr_vlan_xlate_1_entries - 1;

    /* Adjust EGR_VLAN_XLATE_2 table size */

    sop->memState[EGR_VLAN_XLATE_2_SINGLEm].index_max = egr_vlan_xlate_2_entries - 1;
    sop->memState[EGR_VLAN_XLATE_2_DOUBLEm].index_max = egr_vlan_xlate_2_entries / 2 - 1;
    /* 3k per bank */
    sop->memState[EGR_VLAN_XLATE_2_LPm].index_max = egr_vlan_xlate_2_banks * 3 * 1024 - 1;
    sop->memState[EGR_VLAN_XLATE_2_ECCm].index_max = egr_vlan_xlate_2_entries - 1;


    SOC_CONTROL(unit)->l3_defip_max_tcams = _SOC_TD3_DEFIP_MAX_TCAMS;
    SOC_CONTROL(unit)->l3_defip_tcam_size = _SOC_TD3_DEFIP_TCAM_DEPTH;

    if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
        if (!alpm_enable) {
            defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);

            num_ipv6_128b_entries = soc_property_get(unit,
                                        spn_NUM_IPV6_LPM_128B_ENTRIES,
                                        (defip_config ? 4096 : 0));
            num_ipv6_128b_entries = num_ipv6_128b_entries +
                                    (num_ipv6_128b_entries % 2);

            config_v6_entries = num_ipv6_128b_entries;
            if (soc_property_get(unit, spn_LPM_SCALING_ENABLE, 0)) {
                num_ipv6_128b_entries = 0;
                if (!soc_property_get(unit, spn_LPM_IPV6_128B_RESERVED, 1)) {
                    config_v6_entries = ((config_v6_entries /
                                        SOC_CONTROL(unit)->l3_defip_tcam_size) +
                                        ((config_v6_entries %
                                         SOC_CONTROL(unit)->l3_defip_tcam_size)
                                        ? 1 : 0)) * SOC_CONTROL(unit)->l3_defip_tcam_size;
                }
            }
            sop->memState[L3_DEFIP_PAIR_128m].index_max =
                                              num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max =
                                              num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max =
                                              num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max =
                                              num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIPm].index_max =
                                     (SOC_CONTROL(unit)->l3_defip_max_tcams *
                                     SOC_CONTROL(unit)->l3_defip_tcam_size) -
                                     (num_ipv6_128b_entries * 2) - 1;

            sop->memState[L3_DEFIP_ONLYm].index_max =
                                          sop->memState[L3_DEFIPm].index_max;
            sop->memState[L3_DEFIP_DATA_ONLYm].index_max =
                                          sop->memState[L3_DEFIPm].index_max;
            sop->memState[L3_DEFIP_HIT_ONLYm].index_max =
                                          sop->memState[L3_DEFIPm].index_max;
            SOC_CONTROL(unit)->l3_defip_index_remap = num_ipv6_128b_entries;
        } else {
            if (soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1)) {
                /* slightly different processing for v6-128 */
                num_ipv6_128b_entries = soc_property_get(unit,
                                            spn_NUM_IPV6_LPM_128B_ENTRIES,
                                            4096);
                num_ipv6_128b_entries = num_ipv6_128b_entries +
                                        (num_ipv6_128b_entries % 2);
                config_v6_entries = num_ipv6_128b_entries;
                sop->memState[L3_DEFIP_PAIR_128m].index_max =
                                        num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max =
                                        num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max =
                                        num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max =
                                        num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIPm].index_max =
                                  (SOC_CONTROL(unit)->l3_defip_max_tcams *
                                  SOC_CONTROL(unit)->l3_defip_tcam_size) -
                                  (num_ipv6_128b_entries * 2) - 1;

                sop->memState[L3_DEFIP_ONLYm].index_max =
                                        sop->memState[L3_DEFIPm].index_max;
                sop->memState[L3_DEFIP_DATA_ONLYm].index_max =
                                        sop->memState[L3_DEFIPm].index_max;
                sop->memState[L3_DEFIP_HIT_ONLYm].index_max =
                                        sop->memState[L3_DEFIPm].index_max;

                SOC_CONTROL(unit)->l3_defip_index_remap =
                                        (num_ipv6_128b_entries / 2) * 2;
            } else {
                sop->memState[L3_DEFIP_PAIR_128m].index_max = -1;
                sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max = -1;
                sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max = -1;
                sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max = -1;
            }
        }
        soc_l3_defip_indexes_init(unit, config_v6_entries);
    }

    /* Adjust ALPM table size */
    if (alpm_enable) {
        /* The check below is sufficient because if ALPM mode is enabled
         * and number of shared banks used is > 4 we return SOC_E_PARAM
         * in code at the top of this function.
         */
        if (shared_l2_banks + shared_l3_banks + shared_fpem_banks > 0) {
            sop->memState[L3_DEFIP_ALPM_RAWm].index_max =
                sop->memState[L3_DEFIP_ALPM_RAWm].index_max/2;
            sop->memState[L3_DEFIP_ALPM_IPV4m].index_max =
                sop->memState[L3_DEFIP_ALPM_IPV4m].index_max/2;
            sop->memState[L3_DEFIP_ALPM_IPV4_1m].index_max =
                sop->memState[L3_DEFIP_ALPM_IPV4_1m].index_max/2;
            sop->memState[L3_DEFIP_ALPM_IPV6_64m].index_max =
                sop->memState[L3_DEFIP_ALPM_IPV6_64m].index_max/2;
            sop->memState[L3_DEFIP_ALPM_IPV6_64_1m].index_max =
                sop->memState[L3_DEFIP_ALPM_IPV6_64_1m].index_max/2;
            sop->memState[L3_DEFIP_ALPM_IPV6_128m].index_max =
                sop->memState[L3_DEFIP_ALPM_IPV6_128m].index_max/2;
            sop->memState[L3_DEFIP_ALPM_HIT_ONLYm].index_max =
                sop->memState[L3_DEFIP_ALPM_HIT_ONLYm].index_max/2;
            sop->memState[L3_DEFIP_ALPM_ECCm].index_max =
                sop->memState[L3_DEFIP_ALPM_ECCm].index_max/2;
        }
    } else {
        sop->memState[L3_DEFIP_ALPM_RAWm].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_IPV4m].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_IPV4_1m].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_IPV6_64m].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_IPV6_64_1m].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_IPV6_128m].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_HIT_ONLYm].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_ECCm].index_max = -1;
    }

    /* Adjust MY_STATION_TCAM table in Emulator */
    if (SAL_BOOT_QUICKTURN) {
        sop->memState[MY_STATION_TCAMm].index_max = 31;
        sop->memState[MY_STATION_TCAM_2m].index_max =
            sop->memState[MY_STATION_TCAMm].index_max;
        sop->memState[MY_STATION_TCAM_2_DATA_ONLYm].index_max =
            sop->memState[MY_STATION_TCAMm].index_max;
        sop->memState[MY_STATION_TCAM_2_ENTRY_ONLYm].index_max =
            sop->memState[MY_STATION_TCAMm].index_max;
        sop->memState[MY_STATION_TCAM_DATA_ONLYm].index_max =
            sop->memState[MY_STATION_TCAMm].index_max;
        sop->memState[MY_STATION_TCAM_ENTRY_ONLYm].index_max =
            sop->memState[MY_STATION_TCAMm].index_max;
    }

    if (_soc_td3_alpm_bkt_view_map[unit] != NULL) {
        sal_free(_soc_td3_alpm_bkt_view_map[unit]);
        _soc_td3_alpm_bkt_view_map[unit] = NULL;
    }

    if (NULL == _soc_td3_alpm_bkt_view_map[unit]) {
        if ((_soc_td3_alpm_bkt_view_map[unit] =
            sal_alloc(sizeof(soc_mem_t) * SOC_TD3_ALPM_MAX_BKTS(unit),
                      "alpm_bkt_map")) == NULL) {
            return SOC_E_MEMORY;
        }
    }

    sal_memset(_soc_td3_alpm_bkt_view_map[unit], INVALIDm,
               sizeof(soc_mem_t) * SOC_TD3_ALPM_MAX_BKTS(unit));

    if (soc_trident3_alpm_mode_get(unit)) {
        SOC_IF_ERROR_RETURN(soc_trident3_init_alpm2_memory(unit));
    }

   /* Enhanced hashing  LAG/TRUNK config
    * LAG/TRUNK share the same flow set table used in RH
    * 64k is shared between LAG and HGT for
    * - 32k each - default
    * - 64K dedicated to HGT RH
    * - 64K dedicated to LAG RH
    */
    num_lag_rh_flowset_entries = soc_property_get(unit,
                spn_TRUNK_RESILIENT_HASH_TABLE_SIZE, 32768);

    switch (num_lag_rh_flowset_entries) {
        case 0:
            sop->memState[RH_LAG_FLOWSETm].index_max = -1;
            sop->memState[RH_LAG_FLOWSET_PIPE0m].index_max = -1;
            sop->memState[RH_LAG_FLOWSET_PIPE1m].index_max = -1;
            break;
        case 65536:
            sop->memState[RH_HGT_FLOWSETm].index_max = -1;
            sop->memState[RH_HGT_FLOWSET_PIPE0m].index_max = -1;
            sop->memState[RH_HGT_FLOWSET_PIPE1m].index_max = -1;
            break;
        case 32768:
            sop->memState[RH_HGT_FLOWSETm].index_max = 32767;
            sop->memState[RH_HGT_FLOWSET_PIPE0m].index_max = 32767;
            sop->memState[RH_HGT_FLOWSET_PIPE1m].index_max = 32767;
            sop->memState[RH_LAG_FLOWSETm].index_max = 32767;
            sop->memState[RH_LAG_FLOWSET_PIPE0m].index_max = 32767;
            sop->memState[RH_LAG_FLOWSET_PIPE1m].index_max = 32767;
            break;
        default:
            return SOC_E_CONFIG;
    }
    return SOC_E_NONE;
}

void
_soc_trident3_alpm2_bnk_fmt_set(int unit, int index, int8 fmt)
{
    _soc_td3_alpm2_bnk_fmt_map[unit][index] = fmt;
}

int8
_soc_trident3_alpm2_bnk_fmt_get(int unit, int index)
{
    return _soc_td3_alpm2_bnk_fmt_map[unit][index];
}

void
_soc_trident3_alpm_bkt_view_set(int unit, int index, soc_mem_t view)
{
    int bkt = soc_trident3_get_alpm_banks(unit);
    if (bkt == 8) {
        bkt = 3;
    } else if (bkt == 4) {
        bkt = 2;
    } else if (bkt == 2) {
        bkt = 1;
    }
    bkt = (index >> bkt) & SOC_TD3_ALPM_BKT_MASK(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Unit:%d ALPM bkt set index:%d bkt:%d view:%s\n"),
                 unit, index, bkt, SOC_MEM_NAME(unit, view)));
    _soc_td3_alpm_bkt_view_map[unit][bkt] = view;
}

soc_mem_t
_soc_trident3_alpm_bkt_view_get(int unit, int index)
{
    soc_mem_t view;
    int bkt = soc_trident3_get_alpm_banks(unit);
    if (bkt == 8) {
        bkt = 3;
    } else if (bkt == 4) {
        bkt = 2;
    } else if (bkt == 2) {
        bkt = 1;
    }
    bkt = (index >> bkt) & SOC_TD3_ALPM_BKT_MASK(unit);

    view = _soc_td3_alpm_bkt_view_map[unit][bkt];
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Unit:%d ALPM bkt get index:%d bkt:%d view:%s\n"),
                 unit, index, bkt, SOC_MEM_NAME(unit, view)));
    return view;
}

int
soc_trident3_mem_cpu_write_control(int unit, soc_mem_t mem, int copyno,
                                   int enable, int *orig_enable)
{
    soc_reg_t reg;
    soc_field_t field;
    uint32 rval, fval;
    int blk, port, block_info_index;
    uint32 orig_fval, enable_fval, disable_fval;

    enable_fval = 1;
    disable_fval = 0;

    switch (mem) {
    case CLPORT_WC_UCMEM_DATAm:
        reg = CLPORT_WC_UCMEM_CTRLr;
        field = ACCESS_MODEf;
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
            port = SOC_BLOCK_PORT(unit, blk);
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
            /* It will use the setting from the last block */
            *orig_enable = soc_reg_field_get(unit, reg, rval, field);
            soc_reg_field_set(unit, reg, &rval, field, enable ? 1 : 0);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
        }
        return SOC_E_NONE;
    default:
        block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
        if ((SOC_BLOCK_TYPE(unit, block_info_index) == SOC_BLK_MMU_XPE) ||
            (SOC_BLOCK_TYPE(unit, block_info_index) == SOC_BLK_MMU_SED) ||
            (SOC_BLOCK_TYPE(unit, block_info_index) == SOC_BLK_MMU_SC)){
            reg = MMU_GCFG_MISCCONFIGr;
            field = REFRESH_ENf;
            enable_fval = 0;
            disable_fval = 1;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            orig_fval = soc_reg_field_get(unit, reg, rval, field);
            fval = enable ? enable_fval : disable_fval;
            *orig_enable = orig_fval == enable_fval;
            if (fval != orig_fval) {
                soc_reg_field_set(unit, reg, &rval, field, fval);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
            }
        }
        return _soc_trident2_mem_cpu_write_control(unit, mem, copyno,
                                                   enable, orig_enable);
    }
    return SOC_E_NONE;

#if 0
    soc_reg_t reg;
    soc_field_t field;
    int blk, port;
    uint32 rval;

    switch (mem) {
    case CLPORT_WC_UCMEM_DATAm:
        reg = CLPORT_WC_UCMEM_CTRLr;
        field = ACCESS_MODEf;
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
            port = SOC_BLOCK_PORT(unit, blk);
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
            /* It will use the setting from the last block */
            *orig_enable = soc_reg_field_get(unit, reg, rval, field);
            soc_reg_field_set(unit, reg, &rval, field, enable ? 1 : 0);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
        }
        return SOC_E_NONE;
    default:
        return _soc_trident2_mem_cpu_write_control(unit, mem, copyno,
                                                   enable, orig_enable);
    }
#endif
}


int
soc_trident3_reg_cpu_write_control(int unit, int enable)
{
    uint32 intfo_dis = 0;

    if (enable) {
        /* Disable HW updates */
        soc_reg_field_set(unit, INTFO_HW_UPDATE_DISr, &intfo_dis,
                          EG_SPf, 1);
        soc_reg_field_set(unit, INTFO_HW_UPDATE_DISr, &intfo_dis,
                          ING_SPf, 1);
        soc_reg_field_set(unit, INTFO_HW_UPDATE_DISr, &intfo_dis,
                          CONGST_STf, 1);
    }
    SOC_IF_ERROR_RETURN(WRITE_INTFO_HW_UPDATE_DISr(unit, intfo_dis));

    return SOC_E_NONE;
}

soc_mem_t
_soc_trident3_pmem(int unit, int port, soc_mem_t mema, soc_mem_t memb)
{
    return (SOC_INFO(unit).port_pipe[port]) ? memb : mema;
}

uint32 _soc_td3_mmu_port(int unit, int port)
{
    soc_info_t *si = &SOC_INFO(unit);
    return si->port_p2m_mapping[si->port_l2p_mapping[port]];
}

/*
 * cpu port (mmu port 64): 48 queues (650-697)
 * loopback port (mmu port 65, 193): 10 queues (640-649)
 * mgmt port (mmu port 192): 10 UC, 10 MC queues
 */
int
soc_trident3_num_cosq_init_port(int unit, int port)
{
    soc_info_t *si;
    int mmu_port;

    si = &SOC_INFO(unit);

    mmu_port = SOC_TD3_MMU_PORT(unit, port);

    if (IS_CPU_PORT(unit, port)) {
        si->port_num_cosq[port] = 48;
        si->port_cosq_base[port] = SOC_TD3_CPU_MCQ_BASE;
    } else if (IS_LB_PORT(unit, port)) {
        si->port_num_cosq[port] = 10;
        si->port_cosq_base[port] = SOC_TD3_LB_MCQ_BASE;
    } else if (IS_MGMT_PORT(unit, port)) {
        si->port_num_cosq[port] = 10;
        si->port_num_uc_cosq[port] = 10;
        if (si->port_l2p_mapping[port] == 128) {
            /* 2nd Mgmt port */
            si->port_cosq_base[port] = SOC_TD3_SECOND_MGMT_BASE;
            si->port_uc_cosq_base[port] = SOC_TD3_SECOND_MGMT_BASE;
        } else {
            si->port_cosq_base[port] = SOC_TD3_MGMT_MCQ_BASE;
            si->port_uc_cosq_base[port] = SOC_TD3_MGMT_UCQ_BASE;
        }
    } else {
        si->port_num_cosq[port] = 10;
        si->port_cosq_base[port] =
            (mmu_port % 64) * 10;
        si->port_num_uc_cosq[port] = 10;
        si->port_uc_cosq_base[port] =
            (mmu_port % 64) * 10;
        si->port_num_ext_cosq[port] = 0;
    }

    return SOC_E_NONE;
}

/*
 * cpu port (mmu port 64): 48 queues (650-697)
 * loopback port (mmu port 65, 193): 10 queues (640-649)
 * mgmt port (mmu port 192): 10 UC, 10 MC queues
 */
int
soc_trident3_num_cosq_init(int unit)
{
    int port;

    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_trident3_num_cosq_init_port(unit, port));
    }

    return SOC_E_NONE;
}

STATIC int
soc_trident3_max_frequency_get(int unit,  uint16 dev_id, uint8 rev_id,
                               int skew_id, int *frequency)
{
    switch (dev_id) {
    case BCM56870_DEVICE_ID:
    case BCM56873_DEVICE_ID:
        *frequency = CCLK_FREQ_1525MHZ;
        break;
    default:
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

void
soc_trident3_tsc_map_get(int unit, uint32 *tsc_map)
{
    *tsc_map = 0xffffffff;
}

void
soc_trident3_pipe_map_get(int unit, uint32 *pipe_map)
{
    soc_info_t *si;
    int port;

    si = &SOC_INFO(unit);

    *pipe_map = 0;
    PBMP_LB_ITER(unit, port) {
        *pipe_map |= 1 << si->port_pipe[port];
    }
}

/*
 * #1   single: 100  -   x  -   x  -   x  SOC_TD3_PORT_RATIO_SINGLE
 * #2   single:  40  -   x  -   x  -   x  SOC_TD3_PORT_RATIO_TRI_023_2_1_1
 * #3   dual:    50  -   x  -  50  -   x  SOC_TD3_PORT_RATIO_DUAL_1_1
 * #4   dual:    40  -   x  -  40  -   x  SOC_TD3_PORT_RATIO_DUAL_1_1
 * #5   dual:    20  -   x  -  20  -   x  SOC_TD3_PORT_RATIO_DUAL_1_1
 * #6   dual:    40  -   x  -  20  -   x  SOC_TD3_PORT_RATIO_DUAL_2_1
 * #7   dual:    20  -   x  -  40  -   x  SOC_TD3_PORT_RATIO_DUAL_1_2
 * #8   tri:     50  -   x  - 25/x - 25/x SOC_TD3_PORT_RATIO_TRI_023_2_1_1
 * #9   tri:     20  -   x  - 10/x - 10/x SOC_TD3_PORT_RATIO_TRI_023_2_1_1
 * #10  tri:     40  -   x  - 10/x - 10/x SOC_TD3_PORT_RATIO_TRI_023_4_1_1
 * #11  tri:    25/x - 25/x -  50  -   x  SOC_TD3_PORT_RATIO_TRI_012_1_1_2
 * #12  tri:    10/x - 10/x -  20  -   x  SOC_TD3_PORT_RATIO_TRI_012_1_1_2
 * #13  tri:    10/x - 10/x -  40  -   x  SOC_TD3_PORT_RATIO_TRI_012_1_1_4
 * #14  quad:   25/x - 25/x - 25/x - 25/x SOC_TD3_PORT_RATIO_QUAD
 * #15  quad:   10/x - 10/x - 10/x - 10/x SOC_TD3_PORT_RATIO_QUAD
 */
void
soc_trident3_port_ratio_get(int unit, int clport, int *mode)
{
    soc_info_t *si;
    int port, phy_port_base, lane;
    int num_lanes[_TD3_PORTS_PER_PBLK];
    int speed_max[_TD3_PORTS_PER_PBLK];

    si = &SOC_INFO(unit);
    phy_port_base = 1 + clport * _TD3_PORTS_PER_PBLK;
    for (lane = 0; lane < _TD3_PORTS_PER_PBLK; lane += 2) {
        port = si->port_p2l_mapping[phy_port_base + lane];
        if (port == -1 || SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            num_lanes[lane] = -1;
            speed_max[lane] = -1;
        } else {
            num_lanes[lane] = si->port_num_lanes[port];
            speed_max[lane] = si->port_speed_max[port];
        }
    }

    if (num_lanes[0] == 4) {
        *mode = SOC_TD3_PORT_RATIO_SINGLE;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 2) {
        if (speed_max[0] == speed_max[2]) {
            *mode = SOC_TD3_PORT_RATIO_DUAL_1_1;
        } else if (speed_max[0] > speed_max[2]) {
            *mode = SOC_TD3_PORT_RATIO_DUAL_2_1;
        } else {
            *mode = SOC_TD3_PORT_RATIO_DUAL_1_2;
        }
    } else if (num_lanes[0] == 2) {
        if (num_lanes[2] == -1) {
            *mode = SOC_TD3_PORT_RATIO_DUAL_1_1;
        } else {
            *mode = (speed_max[0] == 20000 || speed_max[0] == 21000 ) ?
                SOC_TD3_PORT_RATIO_TRI_023_2_1_1 : SOC_TD3_PORT_RATIO_TRI_023_4_1_1;
        }
    } else if (num_lanes[2] == 2) {
        if (num_lanes[0] == -1) {
            *mode = SOC_TD3_PORT_RATIO_DUAL_1_1;
        } else {
            *mode = (speed_max[2] == 20000 || speed_max[2] == 21000 ) ?
                SOC_TD3_PORT_RATIO_TRI_012_1_1_2 : SOC_TD3_PORT_RATIO_TRI_012_1_1_4;
        }
    } else {
        *mode = SOC_TD3_PORT_RATIO_QUAD;
    }
}

#if 0
STATIC void
_soc_trident3_pipe_bandwidth_get(int unit, int pipe,
                                 int *max_pipe_core_bandwidth,
                                 int *pipe_linerate_bandwidth,
                                 int *pipe_oversub_bandwidth)
{
    soc_info_t *si;
    int port;
    int bandwidth;

    si = &SOC_INFO(unit);

    *max_pipe_core_bandwidth = si->bandwidth / 4;

    *pipe_linerate_bandwidth = 0;
    *pipe_oversub_bandwidth = 0;
    PBMP_PORT_ITER(unit, port) {
        if (si->port_pipe[port] != pipe) {
            continue;
        }
        bandwidth = si->port_speed_max[port] >= 2500 ?
            si->port_speed_max[port] : 2500;
        if (SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
            *pipe_oversub_bandwidth += bandwidth;
        } else {
            *pipe_linerate_bandwidth += bandwidth;
        }
    }
    /* 10G cpu/management, 5G loopback, 10G purge/refresh */
    *pipe_linerate_bandwidth += 25;
}
#endif

#if defined(BCM_TD3_ASF_EXCLUDE)
/*
 * Cut-through class encoding
 * 0 - SAF
 * 1 - 10GE
 * 2 - HG[11]
 * 3 - 20GE
 * 4 - 21[HG}
 * 5 - 25GE
 * 6 - HG[27]
 * 7 - 40GE
 * 8 - HG[42]
 * 9 - 50GE
 * 10 - HG[53]
 * 11 - 100GE
 * 12 - HG[106]
 */
STATIC void
_soc_trident3_speed_to_ct_class_mapping(int unit, int speed, int *ct_class)
{
    if (speed >= 40000) {
        if (speed >= 100000) {
            *ct_class = speed > 100000 ? 12 : 11;
        } else if (speed >= 50000) {
            *ct_class = speed > 50000 ? 10 : 9;
        } else {
            *ct_class = speed > 40000 ? 8 : 7;
        }
    } else {
        if (speed >= 25000) {
            *ct_class = speed > 25000 ? 6 : 5;
        } else if (speed >= 20000) {
            *ct_class = speed > 20000 ? 4 : 3;
        } else {
            *ct_class = speed > 10000 ? 2 : 1;
        }
    }
}
#endif

/*
 * Oversubscription group speed class encoding
 * 0 - 0
 * 1 - 2 (10G)
 * 2 - 4 (20G)
 * 3 - 5 (25G)
 * 4 - 8 (40G)
 * 5 - 10 (50G)
 * 6 - 20 (100G)
 */
STATIC void
_soc_trident3_speed_to_ovs_class_mapping(int unit, int speed, int *ovs_class)
{
    if (speed >= 40000) {
        if (speed >= 100000) {
            *ovs_class = 6;
        } else if (speed >= 50000) {
            *ovs_class = 5;
        } else {
            *ovs_class = 4;
        }
    } else {
        if (speed >= 25000) {
            *ovs_class = 3;
        } else if (speed >= 20000) {
            *ovs_class = 2;
        } else {
            *ovs_class = 1;
        }
    }
}

/* Function to get the number of ports present in a given Port Macro */
int
soc_td3_ports_per_pm_get(int unit, int pm_id)
{
    soc_info_t *si;
    int phy_port, num_ports = 0, i;

    si = &SOC_INFO(unit);
    if (pm_id >= _TD3_PBLKS_PER_DEV(unit)) {
       return SOC_E_PARAM;
    }
    phy_port = 1 + (pm_id * _TD3_PORTS_PER_PBLK);

    for (i = 0; i < _TD3_PORTS_PER_PBLK; ) {
        if (si->port_p2l_mapping[phy_port + i] != -1) {
            num_ports ++;
            i += si->port_num_lanes[si->port_p2l_mapping[phy_port + i]];
        } else {
            i ++;
        }
    }
    return num_ports;
}

STATIC int
_soc_td3_port_flex_init(int unit, int is_any_cap)
{
    soc_info_t *si;
    int pm, blk_idx, blk_type;
    int flex_en, static_ports, max_ports;

    si = &SOC_INFO(unit);
    SHR_BITCLR_RANGE(si->flexible_pm_bitmap, 0, SOC_MAX_NUM_BLKS);

    /* portmap_x=y:speed:i */
    /* portmap_x=y:speed:cap */
    if (SOC_PBMP_NOT_NULL(SOC_PORT_DISABLED_BITMAP(unit, all)) || is_any_cap) {
        SHR_BITSET_RANGE(si->flexible_pm_bitmap, 2, _TD3_PBLKS_PER_DEV(unit));
    } else {
        /* port_flex_enable{x}=<0...1> */
        /* port_flex_enable_corex=<0...1> */
        /* port_flex_enable=<0...1> */
        for (blk_idx = 0; (blk_idx < SOC_MAX_NUM_BLKS) && (SOC_BLOCK_TYPE(unit, blk_idx)!= -1); blk_idx++) {
            blk_type = SOC_BLOCK_TYPE(unit, blk_idx);
            pm = SOC_BLOCK_NUMBER(unit, blk_idx);
            if (blk_type == SOC_BLK_CLPORT) {
       /* port_flex_enable_corex=<0...1> */
                flex_en = soc_property_suffix_num_get_only_suffix(unit,
                    pm, spn_PORT_FLEX_ENABLE, "core", -1);
                /* port_flex_enable{x}=<0...1> */
                /* port_flex_enable=<0...1> */
                if (flex_en == -1) {
                    flex_en = soc_property_phys_port_get(unit,
                        (pm * _TD3_PORTS_PER_PBLK + 1),
                        spn_PORT_FLEX_ENABLE, 0);
                }
                if (flex_en) {
                    SHR_BITSET(si->flexible_pm_bitmap, blk_idx);
                }
            }
        }
    }

    si->flex_eligible =
        (!SHR_BITNULL_RANGE(si->flexible_pm_bitmap, 1, _TD3_PBLKS_PER_DEV(unit)));

    /* port_flex_max_ports */
    memset(&(si->pm_max_ports), 0xff, sizeof(si->pm_max_ports));
    for (blk_idx = 0; (blk_idx < SOC_MAX_NUM_BLKS) && (SOC_BLOCK_TYPE(unit, blk_idx)!= -1); blk_idx++) {
        blk_type = SOC_BLOCK_TYPE(unit, blk_idx);
        pm = SOC_BLOCK_NUMBER(unit, blk_idx);
        if (blk_type == SOC_BLK_CLPORT) {
            static_ports = soc_td3_ports_per_pm_get(unit, pm);
            if (SHR_BITGET(si->flexible_pm_bitmap, blk_idx)) {
                /* port_flex_max_ports{x}=y */
                max_ports = soc_property_phys_port_get(unit,
                    (pm * _TD3_PORTS_PER_PBLK + 1),
                    spn_PORT_FLEX_MAX_PORTS, -1);
                /* port_flex_max_ports_corex=y */
                if (max_ports == -1) {
                    max_ports = soc_property_suffix_num_get(unit, pm,
                        spn_PORT_FLEX_MAX_PORTS, "core", 4);
                }
                /* validation check */
                if ((max_ports < 0) || (max_ports > 4) || (max_ports < static_ports)) {
                    LOG_CLI((BSL_META_U(unit,
                        "Core %d: Bad port_flex_max_ports %d; static ports %d"),
                        pm, max_ports, static_ports));
                    return SOC_E_CONFIG;
                }
                si->pm_max_ports[blk_idx] = max_ports;
            } else {
                si->pm_max_ports[blk_idx] = static_ports;
            }

        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_trident3_port_map_validity_check
 * Purpose:
 *      Logical Port <-> Physical port map is fixed in Trident3.
 *      This function checks if port map used in config.bcm is correct.
 * Parameters:
 *      unit (IN) - switch unit
 *      port (IN) - device port
 *      phy_port (IN) - physical port
 * Returns:
 *      SOC_E_FAIL OR SOC_E_NONE
 */
int
soc_trident3_port_map_validity_check(int unit, int port, int phy_port) {

    int pmap_err = 0;

    if ((port >= 0) && (port <= 64)) {
        if (port != phy_port) {
            pmap_err = 1;
        }
    } else if((port == 65) && (phy_port != 130)) {
        pmap_err = 1;
    } else if ((port == 66) && (phy_port != 129)) {
        pmap_err = 1;
    } else if((port >= 67) && (port <= 130)) {
        if (port != phy_port + 2) {
            pmap_err = 1;
        }
    } else if((port == 131) && (phy_port != 131)) {
        pmap_err = 1;
    }

    if (pmap_err) {
        LOG_CLI((BSL_META_U(unit,
                            "Port %d: Invalid port map configuration\n"),
                            port));
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

/*
 * Trident3 port mapping
 *
 *                   physical   idb/       device   mmu
 *                   port       idb port   port     port
 *     CMIC          0          0/64       0        64
 *     CLPORT0-7     1-32       0/0-31     1-32     0-31
 *     CLPORT8-15    33-64      0/32-63    33-64    32-63
 *     CLPORT16-23   65-96      1/0-32     67-98    128-159
 *     CLPORT24-31   97-128     1/32-63    99-130   160-191
 *     MGMT          129        1/64       66       192
 *     LBPORT0       130        0/65       65       65
 *     LBPORT1       131        1/65       131      193
 * Although MMU port number is flexible within the above range, it is
 *     configured as a fixed mapped to physical port number
 */
int
soc_trident3_port_config_init(int unit, uint16 dev_id)
{
#if defined(BCM_56870_A0)
    soc_info_t *si;
    char *config_str, *sub_str, *sub_str_end;
    static const char str_2p5[] = "2.5";
    char str_buf[8];
    int rv, oversub_mode;
    int num_port, num_phy_port, num_mmu_port;
    int port, phy_port, mmu_port, idb_port;
    int pipe, xpe, sc, index, bandwidth_cap, is_any_cap = FALSE;
    int port_bandwidth, blk_idx;
    char option1=0, option2=0;
    uint32 pipe_map, xpe_map, sc_map, ipipe_map, epipe_map;
    soc_pbmp_t oversub_pbm;
    int list_len, frequency;
    static const int freq_list[] = { 1525, 1425, 1325, 1012, 850 };
    const char *str_ratio[] = { "2:3", "1:1" };
    static const int dpp_ratio_x10_list[] = {
        15, /* core clk : pp clk ratio is 3:2 (default) */
        10  /* core clk : pp clk ratio is 1:1 */
    };

    static const struct {
        int port;
        int phy_port;
        int pipe;
        int idb_port;
        int mmu_port;
    } fixed_ports[] = {
        { 0,   0,   0, 64, 64 },    /* cpu port */
        { 65,  130, 0, 65, 65 },    /* loopback port 0 */
        { 131, 131, 1, 65, 193 },   /* loopback port 1 */
    };

    si = &SOC_INFO(unit);

    num_phy_port = TD3_NUM_PHY_PORT;
    num_port = TD3_NUM_PORT;
    num_mmu_port = TD3_NUM_MMU_PORT;

    soc_trident3_max_frequency_get(unit, dev_id, -1, -1, &si->frequency);
    frequency = soc_property_get(unit, spn_CORE_CLOCK_FREQUENCY, -1);

    /* Core CLK frequency */
    if (frequency != -1 && frequency <= si->frequency) {
        list_len = COUNTOF(freq_list);
        for (index = 0; index < list_len; index++) {
            if (freq_list[index] <= si->frequency &&
                frequency == freq_list[index]) {
                break;
            }
        }

        if (index < list_len) {
            si->frequency = frequency;
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "*** Input core clock frequency %dMHz is not "
                                "supported!\n"), frequency));
        }
    }

    config_str = soc_property_get_str(unit, spn_DPP_CLOCK_RATIO);
    if (config_str) {
        list_len = COUNTOF(dpp_ratio_x10_list);
        for (index = 0; index < list_len; index++) {
            if (!sal_strcmp(config_str, str_ratio[index])) {
                break;
            }
        }
        if (index < list_len &&
            !(si->frequency > 1012 && dpp_ratio_x10_list[index] == 10)) {
            /* ratio 1:1 with core freq larger than 1125MHz is not supported */
            si->dpp_clk_ratio_x10 = dpp_ratio_x10_list[index];
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "*** Input dpp clock frequency ratio %s is not "
                                "supported with core frequency %d!\n"),
                     config_str, si->frequency));
        }
    }

    si->bandwidth = 2048000;


    oversub_pbm = soc_property_get_pbmp(unit, spn_PBMP_OVERSUBSCRIBE, 0);
    oversub_mode = soc_property_get(unit, spn_OVERSUBSCRIBE_MODE, 1) ||
                   SOC_PBMP_NOT_NULL(oversub_pbm);

    si->oversub_mode = oversub_mode;
    si->os_mixed_sister_25_50_enable = soc_property_get(unit,
                         spn_OVERSUBSCRIBE_MIXED_SISTER_25_50_ENABLE, 0);

    /* Line rate mode requires DPR 1:1 and Core frequency no more than 1125MHz */
    if ((!si->oversub_mode)) {
        if (si->frequency > 1012) {
            LOG_CLI((BSL_META_U(unit,
                                "*** Input core clock frequency %dMHz is not "
                                "supported with line rate mode! "
                                "Must be no more than 1012MHz.\n"), si->frequency));
            return SOC_E_FAIL;
        }
        if (si->dpp_clk_ratio_x10 != 10) {
            LOG_CLI((BSL_META_U(unit,
                                "*** Input dpp clock frequency ratio %s is not "
                                "supported with line rate mode! "
                                "Must be 1:1.\n"), (config_str ? config_str : "")));
            return SOC_E_FAIL;
        }
    }

    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        si->port_p2l_mapping[phy_port] = -1;
        si->port_p2m_mapping[phy_port] = -1;
    }
    for (port = 0; port < num_port; port++) {
        si->port_l2p_mapping[port] = -1;
        si->port_l2i_mapping[port] = -1;
        si->port_speed_max[port] = -1;
        si->port_group[port] = -1;
        si->port_serdes[port] = -1;
        si->port_pipe[port] = -1;
        si->port_num_lanes[port] = -1;
    }
    for (mmu_port = 0; mmu_port < num_mmu_port; mmu_port++) {
        si->port_m2p_mapping[mmu_port] = -1;
    }
    SOC_PBMP_CLEAR(si->eq_pbm);
    SOC_PBMP_CLEAR(si->management_pbm);
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        SOC_PBMP_CLEAR(si->pipe_pbm[pipe]);
    };
    SOC_PBMP_CLEAR(si->oversub_pbm);
    SOC_PBMP_CLEAR(si->all.disabled_bitmap);

    /* Populate the fixed mapped ports */
    for (index = 0; index < COUNTOF(fixed_ports); index++) {
        port = fixed_ports[index].port;
        phy_port = fixed_ports[index].phy_port;
        pipe = fixed_ports[index].pipe;;

        si->port_l2p_mapping[port] = phy_port;
        si->port_l2i_mapping[port] = fixed_ports[index].idb_port;
        si->port_p2l_mapping[phy_port] = port;
        si->port_p2m_mapping[phy_port] = fixed_ports[index].mmu_port;
        si->port_pipe[port] = pipe;
    }

    rv = SOC_E_NONE;
    for (port = 1; port < num_port; port++) {
        if (si->port_l2p_mapping[port] != -1) { /* fixed mapped port */
            continue;
        }

        config_str = soc_property_port_get_str(unit, port, spn_PORTMAP);
        if (config_str == NULL) {
            continue;
        }

        /*
         * portmap_<port>=<physical port number>:<bandwidth in Gb>[:<bandwidth cap in Gb/i(inactive)/1/2/4(num lanes)>][:<i>]
         * eg:    portmap_1=1:100
         *     or portmap_1=1:40
         *     or portmap_1=1:40:i
         *     or portmap_1=1:40:2
         *     or portmap_1=1:40:2:i
         *     or portmap_1=1:10:100
         */
        sub_str = config_str;

        /* Parse physical port number */
        phy_port = sal_ctoi(sub_str, &sub_str_end);
        if (sub_str == sub_str_end) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Missing physical port information \"%s\"\n"),
                     port, config_str));
            rv = SOC_E_FAIL;
            continue;
        }
        if (phy_port < 0 || phy_port >= num_phy_port) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Invalid physical port number %d\n"),
                     port, phy_port));
            rv = SOC_E_FAIL;
            continue;
        }
        /* coverity[check_after_sink : FALSE] */
        if (si->port_p2l_mapping[phy_port] != -1) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Physical port %d is used by port %d\n"),
                     port, phy_port, si->port_p2l_mapping[phy_port]));
            rv = SOC_E_FAIL;
            continue;
        }

        /* Skip ':' between physical port number and bandwidth */
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Bad config string \"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            sub_str++;
        }

        if (soc_trident3_port_map_validity_check(unit, port, phy_port)) {
            rv = SOC_E_FAIL;
            continue;
        }

        /* Parse bandwidth */
        for (index = 0; index < sizeof(str_2p5) - 1; index++) {
            if (sub_str[index] == '\0') {
                break;
            }
            str_buf[index] = sub_str[index];
        }
        str_buf[index] = '\0';
        if (!sal_strcmp(str_buf, str_2p5)) {
            port_bandwidth = 2500;
            sub_str_end = &sub_str[sizeof(str_2p5) - 1];
        } else {
            port_bandwidth = sal_ctoi(sub_str, &sub_str_end) * 1000;
            if (sub_str == sub_str_end) {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Missing bandwidth information \"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            switch (port_bandwidth) {
            case 1000:
            case 10000:
            case 11000:
            case 20000:
            case 21000:
            case 25000:
            case 27000:
            case 40000:
            case 42000:
            case 50000:
            case 53000:
            case 100000:
            case 106000:
                break;
            default:
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Invalid bandwidth %d Gb\n"),
                         port, port_bandwidth / 1000));
                rv = SOC_E_FAIL;
                continue;
            }
            if ((!si->oversub_mode) && ((port_bandwidth == 100000) ||
                                        (port_bandwidth == 106000))) {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: %d Gb is Invalid bandwidth for line rate configuration\n"),
                         port, port_bandwidth / 1000));
                return SOC_E_FAIL;
            }
        }

        /* Check if option presents */
        option1 = 0; option2 = 0;
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            /* Skip ':' between bandwidth and options */
            /* Skip ':' between bandwidth and bandwidth cap or options */
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Bad config string \"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            sub_str++;

            if (*sub_str != '\0') {
                /* Parse bandwidth cap or options */
                bandwidth_cap = sal_ctoi(sub_str, &sub_str_end) * 1000;
                switch (bandwidth_cap) {
                case 20000:
                case 21000:
                case 25000:
                case 27000:
                case 40000:
                case 42000:
                case 50000:
                case 53000:
                case 100000:
                case 106000:
                    sub_str = sub_str_end;
                    _soc_td3_port_speed_cap[unit][port] = bandwidth_cap;
                    /* Flex config detected, port speed cap specified */
                    is_any_cap = TRUE;
                    break;
                default:
                    if (!(*sub_str == 'i' || *sub_str == '1' ||
                          *sub_str == 'm' || *sub_str == '2' ||
                          *sub_str == '4')) {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string "
                                            "\"%s\"\n"),
                             port, config_str));
                        rv = SOC_E_FAIL;
                        continue;
                    } else {
                        option1 = *sub_str;
                        sub_str++;
                    }
                }

                /* Check if more options present */
                if (*sub_str != '\0') {
                    /* Skip ':' between options */
                    if (*sub_str != ':') {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string \"%s\"\n"),
                                 port, config_str));
                        rv = SOC_E_FAIL;
                        continue;
                    }
                    sub_str++;

                    if (*sub_str != 'i' && *sub_str != 'm') {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string \"%s\"\n"),
                                 port, config_str));
                        rv = SOC_E_FAIL;
                        continue;
                    }
                    option2 = *sub_str;
                    sub_str++;
                }
            }
        }

        /* Check trailing string */
        if (*sub_str != '\0') {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Bad config string \"%s\"\n"),
                     port, config_str));
            rv = SOC_E_FAIL;
            continue;
        }

        /* Update soc_info */
        si->port_l2p_mapping[port] = phy_port;
        si->port_p2l_mapping[phy_port] = port;
        if (phy_port == 129) { /* management port 0 */
            pipe = 1;
            si->port_l2i_mapping[port] = 64;
            si->port_p2m_mapping[phy_port] = 192;
            si->port_serdes[port] = 32;
            SOC_PBMP_PORT_ADD(si->management_pbm, port);
            if (option2 == 'm'){
                si->port_num_lanes[port] = (option1 == '4')? 4 : (option1 == '2')? 2 : 1;
            } else {
                si->port_num_lanes[port] = 1;
            }
        } else if ((phy_port == 128) && ((option1 == 'm')||(option2 == 'm'))) { /* Management port 1 */
            pipe = 1;
            si->port_l2i_mapping[port] = 63;        /* Local  MMU Port */
            si->port_p2m_mapping[phy_port] = 191;   /* Global MMU Port */
            si->port_serdes[port] = 32;
            SOC_PORT_BLOCK(unit, phy_port) = SOC_PORT_BLOCK(unit, 129); /* lane 2 */
            SOC_PORT_BINDEX(unit, phy_port) = 2;
            SOC_PBMP_PORT_ADD(si->management_pbm, port);
            if (option2 == 'm'){
                si->port_num_lanes[port] = (option1 == '2')? 2 : 1;
            } else {
                si->port_num_lanes[port] = 1;
            }
        } else {
            idb_port = (phy_port - 1) % _TD3_PORTS_PER_PIPE;
            pipe = (phy_port - 1) / _TD3_PORTS_PER_PIPE;
            si->port_l2i_mapping[port] = idb_port;
            si->port_p2m_mapping[phy_port] = (pipe << 7) | idb_port;
            si->port_serdes[port] = (phy_port - 1) / _TD3_PORTS_PER_PBLK;
            if (phy_port == 128) {
                SOC_PORT_BLOCK(unit, phy_port) = SOC_PORT_BLOCK(unit, 127);
                SOC_PORT_BINDEX(unit, phy_port) = SOC_PORT_BINDEX(unit, 127) + 1;
            }
        }
        si->port_pipe[port] = pipe;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);
        si->port_speed_max[port] = port_bandwidth;
        if (option1 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
        } else {
            if ((option2 != 'm') && (option1 != 'm')) {
                switch (option1) {
                    case '1': si->port_num_lanes[port] = 1; break;
                    case '2': si->port_num_lanes[port] = 2; break;
                    case '4': si->port_num_lanes[port] = 4; break;
                    default: si->port_num_lanes[port] = 4; break;
                }
            }
        }
        if (option2 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
        }
        if (oversub_mode) {
            SOC_PBMP_PORT_ADD(si->oversub_pbm, port);
        }
    }

    for (index = 1; index < COUNTOF(fixed_ports); index++) {
        pipe = fixed_ports[index].pipe;
        if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            /* Remove loopback port if the entire pipe is not in use */
            port = fixed_ports[index].port;
            phy_port = fixed_ports[index].phy_port;
            si->port_l2p_mapping[port] = -1;
            si->port_l2i_mapping[port] = -1;
            si->port_p2l_mapping[phy_port] = -1;
            si->port_p2m_mapping[phy_port] = -1;
            si->port_pipe[port] = -1;
        } else {
            port = fixed_ports[index].port;
            SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);
        }
    }

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "physical to logical mapping:")));
        for (index = 0; index < num_phy_port; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                " %3d"), si->port_p2l_mapping[index]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
        LOG_CLI((BSL_META_U(unit,
                            "physical port bandwidth:")));
        for (index = 0; index < num_phy_port; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            port = si->port_p2l_mapping[index];
            if (port == -1) {
                LOG_CLI((BSL_META_U(unit,
                                    "  -1")));
            } else if (si->port_speed_max[port] == 2500) {
                LOG_CLI((BSL_META_U(unit,
                                    " 2.5")));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    " %3d"), si->port_speed_max[port] / 1000));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));

        LOG_CLI((BSL_META_U(unit,
                            "physical to mmu mapping:")));
        for (index = 0; index < num_phy_port; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                " %3d"), si->port_p2m_mapping[index]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

    

    /* Setup port_num_lanes */
    for (port = 0; port < num_port; port++) {
        if (si->port_speed_max[port] > 53000) {
            si->port_num_lanes[port] = 4;
        } else if (si->port_speed_max[port] == 40000 ||
                   si->port_speed_max[port] == 42000) {
            /* Note: 40G, HG[42] can operate either in dual or quad lane mode
                     Check if adjacent ports exist to set dual lane mode */
            phy_port = si->port_l2p_mapping[port];
            if (phy_port >= 1 && phy_port <= 127 && phy_port != -1) {
                int sis_port;

                if (phy_port % 4 == 1) {
                    sis_port = si->port_p2l_mapping[phy_port+2];
                    if ((si->port_speed_max[sis_port] > 0) &&
                        !SOC_PBMP_MEMBER(si->all.disabled_bitmap, sis_port)) {
                        si->port_num_lanes[port] = 2;
                    }
                } else if (phy_port % 4 == 3) {
                    sis_port = si->port_p2l_mapping[phy_port-2];
                    if ((si->port_speed_max[sis_port] > 0) &&
                        !SOC_PBMP_MEMBER(si->all.disabled_bitmap, sis_port)) {
                        si->port_num_lanes[port] = 2;
                    }
                }
            }
            /* Else set to quad lane mode by default if the user did not
               specify anything and no sister ports existed */
            if (si->port_num_lanes[port] == -1) {
                si->port_num_lanes[port] = 4;
            }
        } else if ((si->port_speed_max[port] >= 20000) &&
                    !(si->port_speed_max[port] == 25000 ||
                      si->port_speed_max[port] == 27000)) {
            /* 50G, HG[53], 20G MLD, HG[21] use 2 lanes */
            si->port_num_lanes[port] = 2;
        } else if (si->port_speed_max[port] > 0) {
            /* 10G XFI, HG[11], 25G XFI and HG[27] use 1 lane */
            if (!SOC_PBMP_MEMBER(SOC_INFO(unit).management_pbm, port)) {
                si->port_num_lanes[port] = 1;
            }
        }
    }

    /* get flex port properties */
    if (SOC_FAILURE(_soc_td3_port_flex_init(unit, is_any_cap))) {
        rv = SOC_E_FAIL;
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (si->flex_eligible) {
        if (SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN(
                soc_td3_flexport_scache_recovery(unit));
        } else {
            SOC_IF_ERROR_RETURN(
                soc_td3_flexport_scache_allocate(unit));
        }
    }
#endif

    /*
     * After WARM BOOT flex port scache recovery, P/L and L/P bit map changed
     * Need to redo bit map creation per port type.
     */

    /* Setup high speed port (HSP) pbm */
    for (port = 0; port < num_port; port++) {
        phy_port = si->port_l2p_mapping[port];
        if (phy_port == -1) {
            continue;
        }

        if (phy_port == 129) { /* management port 0 */
            pipe = 1;
            si->port_l2i_mapping[port] = 64;
            si->port_p2m_mapping[phy_port] = 192;
            si->port_serdes[port] = 32;
            SOC_PBMP_PORT_ADD(si->management_pbm, port);
        } else if ((phy_port == 128) && (option1 == 'm')) { /* Management port 1 */
            pipe = 1;
            si->port_l2i_mapping[port] = 63;        /* Local  MMU Port */
            si->port_p2m_mapping[phy_port] = 191;   /* Global MMU Port */
            si->port_serdes[port] = 32;
            SOC_PORT_BLOCK(unit, phy_port) = SOC_PORT_BLOCK(unit, 129); /* lane 2 */
            SOC_PORT_BINDEX(unit, phy_port) = 2;
            SOC_PBMP_PORT_ADD(si->management_pbm, port);
        } else if (phy_port >= 130) { /* loopback port (130 and 131 in TD3) */
            pipe = phy_port - 130;
            idb_port = 65;
        } else if (phy_port == 0) {
            pipe = 0;
        } else {
            idb_port = (phy_port - 1) % _TD3_PORTS_PER_PIPE;
            pipe = (phy_port - 1) / _TD3_PORTS_PER_PIPE;
            si->port_l2i_mapping[port] = idb_port;
            si->port_p2m_mapping[phy_port] = (pipe << 7) | idb_port;
            si->port_serdes[port] = (phy_port - 1) / _TD3_PORTS_PER_PBLK;
        }
        si->port_pipe[port] = pipe;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);

        if (oversub_mode) {
            SOC_PBMP_PORT_ADD(si->oversub_pbm, port);
        }

        if (si->port_speed_max[port] >= 40000) {
            SOC_PBMP_PORT_ADD(si->eq_pbm, port);
        }
    }

    /* Setup pipe/xpe mapping
     * XPE_E / XPE_D (SC/SED): IP 0/1 EP 0/1
     */
    pipe_map = 0;
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            continue;
        }
        pipe_map |= 1 << pipe;
    }
    xpe_map = 0;
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        ipipe_map = 0x3 & pipe_map;
        epipe_map = 0x3 & pipe_map;
        if (ipipe_map == 0 || epipe_map == 0) {
            continue;
        }
        xpe_map |= 1 << xpe;
        si->xpe_ipipe_map[xpe] = ipipe_map;
        si->xpe_epipe_map[xpe] = epipe_map;
    }
    sc_map = 0;
    for (sc = 0; sc < NUM_SLICE(unit); sc++) {
        ipipe_map = pipe_map;
        epipe_map = 0x3 & pipe_map;
        if (ipipe_map == 0 || epipe_map == 0) {
            continue;
        }
        sc_map |= 1 << sc;
        si->sc_ipipe_map[sc] = ipipe_map;
        si->sc_epipe_map[sc] = epipe_map;
    }
    for (pipe = 0; pipe < SOC_MAX_NUM_PIPES; pipe++) {
        /* Number of pipes is 4 in Tomahawk */
        /* coverity[overrun-local : FALSE] */
        if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            continue;
        }
        /* coverity[overrun-local : FALSE] */
        si->ipipe_xpe_map[pipe] = 0x1 & xpe_map;
        /* coverity[overrun-local : FALSE] */
        si->epipe_xpe_map[pipe] = 0x1 & xpe_map;
        /* coverity[overrun-local : FALSE] */
        si->ipipe_sc_map[pipe] = 0x1 & sc_map;
        /* coverity[overrun-local : FALSE] */
        si->epipe_sc_map[pipe] = 0x1 & sc_map;
    }

    /* Setup clport refclk master block bitmap.
     * In TD3, there are 8 port macros (clport 6,7,24,25) that
     * will driven directly from the system board and they will act as the
     * refclk master to drive the other instances.
     */
    SHR_BITCLR_RANGE(si->pm_ref_master_bitmap, 0, SOC_MAX_NUM_BLKS);
    for (blk_idx = 0; (blk_idx < SOC_MAX_NUM_BLKS) && (SOC_BLOCK_TYPE(unit, blk_idx)!= -1); blk_idx++) {
        int blknum = SOC_BLOCK_NUMBER(unit, blk_idx);
        int blktype = SOC_BLOCK_TYPE(unit, blk_idx);

        /* CLPORTs 7,8,23,24 provide master clock for other ports */
        if (blktype == SOC_BLK_CLPORT) {
            if ((blknum == 7 || blknum == 8 || blknum == 23 || blknum == 24)) {
                SHR_BITSET(si->pm_ref_master_bitmap, blk_idx);
            }
        }
    }

    return rv;
#else
    return SOC_E_UNAVAIL;
#endif
}

#if !defined(BCM_TD3_ASF_EXCLUDE)
int
soc_td3_port_oversub_ratio_get(
    int              unit,
    soc_port_t       port,
    OUT int* const   ratio)
{
    soc_info_t          *si;
    _soc_trident3_tdm_temp_t *tdm;
    int                  pipe, hpipe, max_ovs_ratio;

    if (!ratio) {
        return SOC_E_PARAM;
    }

    if (SAL_BOOT_XGSSIM) {
        *ratio = 0;
        return SOC_E_NONE;
    }

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    if (!si || !tdm) {
        return SOC_E_INTERNAL;
    }

    max_ovs_ratio = 0;
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        for (hpipe = 0; hpipe < TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            if (max_ovs_ratio < tdm->ovs_ratio_x1000[pipe][hpipe]) {
                max_ovs_ratio = tdm->ovs_ratio_x1000[pipe][hpipe];
            }
        }
    }
    *ratio = max_ovs_ratio;

    return SOC_E_NONE;
}
#endif

uint32 soc_td3_temp_mon_mask[SOC_MAX_NUM_DEVICES]={0};

/*
 * Function:
 *      soc_td3_set_idb_dpp_ctrl
 * Purpose:
 *      Configure IDB DPP credits
 * Parameters:
 *      unit (IN)    - switch unit
 * Returns:
 *      SOC_E_*
 */
int
soc_td3_set_idb_dpp_ctrl(int unit)
{
    soc_reg_t reg;
    uint32 pipe_map, in_progress;
    int pipe;
    uint32 rval, rval0;
    int pipe_init_usec;
    soc_timeout_t to;
    static const soc_reg_t idb_dpp_ctrl[2] = {
        IDB_DPP_CTRL_PIPE0r, IDB_DPP_CTRL_PIPE1r
    };

    soc_trident3_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        reg = idb_dpp_ctrl[pipe];
        rval = 0;
        /*SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));*/
        soc_reg_field_set(unit, reg, &rval, MAX_REF_CNTf, 4);
        soc_reg_field_set(unit, reg, &rval, MAX_SBUS_CNTf, 4);
        soc_reg_field_set(unit, reg, &rval, CREDITSf, 62);
        soc_reg_field_set(unit, reg, &rval, DONEf, 0);
        soc_reg_field_set(unit, reg, &rval, STARTf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for DPP credit initialization done */
    in_progress = pipe_map;
    do {
        for (pipe = 0; pipe < NUM_PIPE(unit) && in_progress; pipe++) {
            reg = SOC_REG_UNIQUE_ACC(unit, IDB_DPP_CTRLr)[pipe];
            if (in_progress & (1 << pipe)) { /* not done yet */
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval0));
                if (soc_reg_field_get(unit, reg, rval0, DONEf)) {
                    in_progress ^= (1 << pipe);
                }
            }
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : IDB_DPP_CTRL timeout\n"), unit));
            break;
        }
    } while (in_progress != 0);

    return SOC_E_NONE;
}

STATIC int
_soc_trident3_idb_init(int unit)
{
    soc_info_t *si;
    _soc_trident3_tdm_temp_t *tdm;
    soc_reg_t reg;
    soc_field_t field;
    int block_info_idx, pipe, clport, obm, lane;
    int port, phy_port_base, lport;
    int lossless, i, count;
    uint32 rval0, rval1, fval;
    uint32 rval_port_cfg = 0, rval_max_usage_cfg = 0;
    uint64 rval64;
    int num_lanes, oversub_ratio_idx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;

    static const struct _obm_setting_s {
        int discard_limit;
        int lossless_xoff;
        int lossy_only_lossy_low_pri;
        int port_xoff[3];          /* for oversub ratio 1.5, 1.8, and above */
        int lossy_low_pri[3];      /* for oversub ratio 1.5, 1.8, and above */
        int lossy_discard[3];      /* for oversub ratio 1.5, 1.8, and above */
        int buffer_start[4];
        int buffer_end[4];
    } obm_settings[] = { /* indexed by number of lanes */
        { /* 0 - invalid  */ 0 },
        { /* 1 lane */
            1148, 136, 806,
            { 428, 428, 428 }, /* port_xoff */
            { 235, 235, 235 }, /* lossy_low_pri */
            { 573, 573, 573 },  /* lossy_discard */
            { 0,    1152, 2304, 3456}, /* buffer_start */
            { 1151, 2303, 3455, 4607}  /* buffer_end */
        },
        { /* 2 lanes */
            2300, 324, 1958,
            { 1310, 1310, 1310 }, /* port_xoff */
            { 235, 235, 235 }, /* lossy_low_pri */
            { 573, 573, 573 },  /* lossy_discard */
            { 0,    0, 2304,    0   },  /* buffer_start */
            { 2303, 4607, 4607, 4607}   /* buffer_end */
        },
        { /* 3 - invalid  */ 0 },
        { /* 4 lanes */
            4604, 774, 4262,
            { 2989, 2989, 2989 }, /* port_xoff */
            { 235, 235, 235 }, /* lossy_low_pri */
            { 573, 573, 573 },  /* lossy_discard */
            { 0,    0,    0,    0   }, /* buffer_start */
            { 4607, 4607, 4607, 4607}  /* buffer_end */
        }
    };

    static const soc_reg_t obm_ctrl_regs[] = {
        IDB_OBM0_CONTROLr, IDB_OBM1_CONTROLr,
        IDB_OBM2_CONTROLr, IDB_OBM3_CONTROLr,
        IDB_OBM4_CONTROLr, IDB_OBM5_CONTROLr,
        IDB_OBM6_CONTROLr, IDB_OBM7_CONTROLr,
        IDB_OBM8_CONTROLr, IDB_OBM9_CONTROLr,
        IDB_OBM10_CONTROLr, IDB_OBM11_CONTROLr,
        IDB_OBM12_CONTROLr, IDB_OBM13_CONTROLr,
        IDB_OBM14_CONTROLr, IDB_OBM15_CONTROLr
    };
    static const soc_reg_t obm_ca_ctrl_regs[] = {
        IDB_OBM0_CA_CONTROLr, IDB_OBM1_CA_CONTROLr,
        IDB_OBM2_CA_CONTROLr, IDB_OBM3_CA_CONTROLr,
        IDB_OBM4_CA_CONTROLr, IDB_OBM5_CA_CONTROLr,
        IDB_OBM6_CA_CONTROLr, IDB_OBM7_CA_CONTROLr,
        IDB_OBM8_CA_CONTROLr, IDB_OBM9_CA_CONTROLr,
        IDB_OBM10_CA_CONTROLr, IDB_OBM11_CA_CONTROLr,
        IDB_OBM12_CA_CONTROLr, IDB_OBM13_CA_CONTROLr,
        IDB_OBM14_CA_CONTROLr, IDB_OBM15_CA_CONTROLr
    };
    static const soc_reg_t obm_thresh_regs[] = {
        IDB_OBM0_THRESHOLDr, IDB_OBM1_THRESHOLDr,
        IDB_OBM2_THRESHOLDr, IDB_OBM3_THRESHOLDr,
        IDB_OBM4_THRESHOLDr, IDB_OBM5_THRESHOLDr,
        IDB_OBM6_THRESHOLDr, IDB_OBM7_THRESHOLDr,
        IDB_OBM8_THRESHOLDr, IDB_OBM9_THRESHOLDr,
        IDB_OBM10_THRESHOLDr, IDB_OBM11_THRESHOLDr,
        IDB_OBM12_THRESHOLDr, IDB_OBM13_THRESHOLDr,
        IDB_OBM14_THRESHOLDr, IDB_OBM15_THRESHOLDr
    };
    static const soc_reg_t obm_thresh_1_regs[] = {
        IDB_OBM0_THRESHOLD_1r, IDB_OBM1_THRESHOLD_1r,
        IDB_OBM2_THRESHOLD_1r, IDB_OBM3_THRESHOLD_1r,
        IDB_OBM4_THRESHOLD_1r, IDB_OBM5_THRESHOLD_1r,
        IDB_OBM6_THRESHOLD_1r, IDB_OBM7_THRESHOLD_1r,
        IDB_OBM8_THRESHOLD_1r, IDB_OBM9_THRESHOLD_1r,
        IDB_OBM10_THRESHOLD_1r, IDB_OBM11_THRESHOLD_1r,
        IDB_OBM12_THRESHOLD_1r, IDB_OBM13_THRESHOLD_1r,
        IDB_OBM14_THRESHOLD_1r, IDB_OBM15_THRESHOLD_1r
    };
    static const soc_reg_t obm_fc_thresh_regs[] = {
        IDB_OBM0_FC_THRESHOLDr, IDB_OBM1_FC_THRESHOLDr,
        IDB_OBM2_FC_THRESHOLDr, IDB_OBM3_FC_THRESHOLDr,
        IDB_OBM4_FC_THRESHOLDr, IDB_OBM5_FC_THRESHOLDr,
        IDB_OBM6_FC_THRESHOLDr, IDB_OBM7_FC_THRESHOLDr,
        IDB_OBM8_FC_THRESHOLDr, IDB_OBM9_FC_THRESHOLDr,
        IDB_OBM10_FC_THRESHOLDr, IDB_OBM11_FC_THRESHOLDr,
        IDB_OBM12_FC_THRESHOLDr, IDB_OBM13_FC_THRESHOLDr,
        IDB_OBM14_FC_THRESHOLDr, IDB_OBM15_FC_THRESHOLDr
    };
    static const soc_reg_t obm_fc_thresh_1_regs[] = {
        IDB_OBM0_FC_THRESHOLD_1r, IDB_OBM1_FC_THRESHOLD_1r,
        IDB_OBM2_FC_THRESHOLD_1r, IDB_OBM3_FC_THRESHOLD_1r,
        IDB_OBM4_FC_THRESHOLD_1r, IDB_OBM5_FC_THRESHOLD_1r,
        IDB_OBM6_FC_THRESHOLD_1r, IDB_OBM7_FC_THRESHOLD_1r,
        IDB_OBM8_FC_THRESHOLD_1r, IDB_OBM9_FC_THRESHOLD_1r,
        IDB_OBM10_FC_THRESHOLD_1r, IDB_OBM11_FC_THRESHOLD_1r,
        IDB_OBM12_FC_THRESHOLD_1r, IDB_OBM13_FC_THRESHOLD_1r,
        IDB_OBM14_FC_THRESHOLD_1r, IDB_OBM15_FC_THRESHOLD_1r
    };
    static const soc_reg_t obm_shared_regs[] = {
        IDB_OBM0_SHARED_CONFIGr, IDB_OBM1_SHARED_CONFIGr,
        IDB_OBM2_SHARED_CONFIGr, IDB_OBM3_SHARED_CONFIGr,
        IDB_OBM4_SHARED_CONFIGr, IDB_OBM5_SHARED_CONFIGr,
        IDB_OBM6_SHARED_CONFIGr, IDB_OBM7_SHARED_CONFIGr,
        IDB_OBM8_SHARED_CONFIGr, IDB_OBM9_SHARED_CONFIGr,
        IDB_OBM10_SHARED_CONFIGr, IDB_OBM11_SHARED_CONFIGr,
        IDB_OBM12_SHARED_CONFIGr, IDB_OBM13_SHARED_CONFIGr,
        IDB_OBM14_SHARED_CONFIGr, IDB_OBM15_SHARED_CONFIGr
    };
    static const soc_reg_t obm_max_usage_regs[] = {
        IDB_OBM0_MAX_USAGE_SELECTr, IDB_OBM1_MAX_USAGE_SELECTr,
        IDB_OBM2_MAX_USAGE_SELECTr, IDB_OBM3_MAX_USAGE_SELECTr,
        IDB_OBM4_MAX_USAGE_SELECTr, IDB_OBM5_MAX_USAGE_SELECTr,
        IDB_OBM6_MAX_USAGE_SELECTr, IDB_OBM7_MAX_USAGE_SELECTr,
        IDB_OBM8_MAX_USAGE_SELECTr, IDB_OBM9_MAX_USAGE_SELECTr,
        IDB_OBM10_MAX_USAGE_SELECTr, IDB_OBM11_MAX_USAGE_SELECTr,
        IDB_OBM12_MAX_USAGE_SELECTr, IDB_OBM13_MAX_USAGE_SELECTr,
        IDB_OBM14_MAX_USAGE_SELECTr, IDB_OBM15_MAX_USAGE_SELECTr
    };
    static const soc_field_t obm_bypass_fields[] = {
        PORT0_BYPASS_ENABLEf, PORT1_BYPASS_ENABLEf,
        PORT2_BYPASS_ENABLEf, PORT3_BYPASS_ENABLEf
    };
    static const soc_field_t obm_oversub_fields[] = {
        PORT0_OVERSUB_ENABLEf, PORT1_OVERSUB_ENABLEf,
        PORT2_OVERSUB_ENABLEf, PORT3_OVERSUB_ENABLEf
    };
    static const soc_field_t obm_ca_sop_fields[] = {
        PORT0_CA_SOPf, PORT1_CA_SOPf, PORT2_CA_SOPf, PORT3_CA_SOPf
    };
    static const soc_reg_t obm_port_config_regs[] = {
        IDB_OBM0_PORT_CONFIGr, IDB_OBM1_PORT_CONFIGr,
        IDB_OBM2_PORT_CONFIGr, IDB_OBM3_PORT_CONFIGr,
        IDB_OBM4_PORT_CONFIGr, IDB_OBM5_PORT_CONFIGr,
        IDB_OBM6_PORT_CONFIGr, IDB_OBM7_PORT_CONFIGr,
        IDB_OBM8_PORT_CONFIGr, IDB_OBM9_PORT_CONFIGr,
        IDB_OBM10_PORT_CONFIGr, IDB_OBM11_PORT_CONFIGr,
        IDB_OBM12_PORT_CONFIGr, IDB_OBM13_PORT_CONFIGr,
        IDB_OBM14_PORT_CONFIGr, IDB_OBM15_PORT_CONFIGr
    };
    static const soc_reg_t obm_fc_config_regs[] = {
        IDB_OBM0_FLOW_CONTROL_CONFIGr, IDB_OBM1_FLOW_CONTROL_CONFIGr,
        IDB_OBM2_FLOW_CONTROL_CONFIGr, IDB_OBM3_FLOW_CONTROL_CONFIGr,
        IDB_OBM4_FLOW_CONTROL_CONFIGr, IDB_OBM5_FLOW_CONTROL_CONFIGr,
        IDB_OBM6_FLOW_CONTROL_CONFIGr, IDB_OBM7_FLOW_CONTROL_CONFIGr,
        IDB_OBM8_FLOW_CONTROL_CONFIGr, IDB_OBM9_FLOW_CONTROL_CONFIGr,
        IDB_OBM10_FLOW_CONTROL_CONFIGr, IDB_OBM11_FLOW_CONTROL_CONFIGr,
        IDB_OBM12_FLOW_CONTROL_CONFIGr, IDB_OBM13_FLOW_CONTROL_CONFIGr,
        IDB_OBM14_FLOW_CONTROL_CONFIGr, IDB_OBM15_FLOW_CONTROL_CONFIGr
    };
    static const soc_reg_t obm_buffer_config_regs[] = {
        IDB_OBM0_BUFFER_CONFIGr, IDB_OBM1_BUFFER_CONFIGr,
        IDB_OBM2_BUFFER_CONFIGr, IDB_OBM3_BUFFER_CONFIGr,
        IDB_OBM4_BUFFER_CONFIGr, IDB_OBM5_BUFFER_CONFIGr,
        IDB_OBM6_BUFFER_CONFIGr, IDB_OBM7_BUFFER_CONFIGr,
        IDB_OBM8_BUFFER_CONFIGr, IDB_OBM9_BUFFER_CONFIGr,
        IDB_OBM10_BUFFER_CONFIGr, IDB_OBM11_BUFFER_CONFIGr,
        IDB_OBM12_BUFFER_CONFIGr, IDB_OBM13_BUFFER_CONFIGr,
        IDB_OBM14_BUFFER_CONFIGr, IDB_OBM15_BUFFER_CONFIGr
    };
    static const soc_mem_t obm_pri_map_mem[][4] = {
        {IDB_OBM0_PRI_MAP_PORT0m, IDB_OBM0_PRI_MAP_PORT1m,
         IDB_OBM0_PRI_MAP_PORT2m, IDB_OBM0_PRI_MAP_PORT3m
        },
        {IDB_OBM1_PRI_MAP_PORT0m, IDB_OBM1_PRI_MAP_PORT1m,
         IDB_OBM1_PRI_MAP_PORT2m, IDB_OBM1_PRI_MAP_PORT3m
        },
        {IDB_OBM2_PRI_MAP_PORT0m, IDB_OBM2_PRI_MAP_PORT1m,
         IDB_OBM2_PRI_MAP_PORT2m, IDB_OBM2_PRI_MAP_PORT3m
        },
        {IDB_OBM3_PRI_MAP_PORT0m, IDB_OBM3_PRI_MAP_PORT1m,
         IDB_OBM3_PRI_MAP_PORT2m, IDB_OBM3_PRI_MAP_PORT3m
        },
        {IDB_OBM4_PRI_MAP_PORT0m, IDB_OBM4_PRI_MAP_PORT1m,
         IDB_OBM4_PRI_MAP_PORT2m, IDB_OBM4_PRI_MAP_PORT3m
        },
        {IDB_OBM5_PRI_MAP_PORT0m, IDB_OBM5_PRI_MAP_PORT1m,
         IDB_OBM5_PRI_MAP_PORT2m, IDB_OBM5_PRI_MAP_PORT3m
        },
        {IDB_OBM6_PRI_MAP_PORT0m, IDB_OBM6_PRI_MAP_PORT1m,
         IDB_OBM6_PRI_MAP_PORT2m, IDB_OBM6_PRI_MAP_PORT3m
        },
        {IDB_OBM7_PRI_MAP_PORT0m, IDB_OBM7_PRI_MAP_PORT1m,
         IDB_OBM7_PRI_MAP_PORT2m, IDB_OBM7_PRI_MAP_PORT3m
        },
        {IDB_OBM8_PRI_MAP_PORT0m, IDB_OBM8_PRI_MAP_PORT1m,
         IDB_OBM8_PRI_MAP_PORT2m, IDB_OBM8_PRI_MAP_PORT3m
        },
        {IDB_OBM9_PRI_MAP_PORT0m, IDB_OBM9_PRI_MAP_PORT1m,
         IDB_OBM9_PRI_MAP_PORT2m, IDB_OBM9_PRI_MAP_PORT3m
        },
        {IDB_OBM10_PRI_MAP_PORT0m, IDB_OBM10_PRI_MAP_PORT1m,
         IDB_OBM10_PRI_MAP_PORT2m, IDB_OBM10_PRI_MAP_PORT3m
        },
        {IDB_OBM11_PRI_MAP_PORT0m, IDB_OBM11_PRI_MAP_PORT1m,
         IDB_OBM11_PRI_MAP_PORT2m, IDB_OBM11_PRI_MAP_PORT3m
        },
        {IDB_OBM12_PRI_MAP_PORT0m, IDB_OBM12_PRI_MAP_PORT1m,
         IDB_OBM12_PRI_MAP_PORT2m, IDB_OBM12_PRI_MAP_PORT3m
        },
        {IDB_OBM13_PRI_MAP_PORT0m, IDB_OBM13_PRI_MAP_PORT1m,
         IDB_OBM13_PRI_MAP_PORT2m, IDB_OBM13_PRI_MAP_PORT3m
        },
        {IDB_OBM14_PRI_MAP_PORT0m, IDB_OBM14_PRI_MAP_PORT1m,
         IDB_OBM14_PRI_MAP_PORT2m, IDB_OBM14_PRI_MAP_PORT3m
        },
        {IDB_OBM15_PRI_MAP_PORT0m, IDB_OBM15_PRI_MAP_PORT1m,
         IDB_OBM15_PRI_MAP_PORT2m, IDB_OBM15_PRI_MAP_PORT3m
        },
    };

    static const soc_field_t obm_ob_pri_fields[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf, OFFSET8_OB_PRIORITYf,
        OFFSET9_OB_PRIORITYf, OFFSET10_OB_PRIORITYf, OFFSET11_OB_PRIORITYf,
        OFFSET12_OB_PRIORITYf, OFFSET13_OB_PRIORITYf, OFFSET14_OB_PRIORITYf,
        OFFSET15_OB_PRIORITYf
    };
    static const int hw_mode_values[SOC_TD3_PORT_RATIO_COUNT] = {
        0, 1, 1, 1, 3, 5, 4, 6, 2
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;
    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    rval_port_cfg = 0;
    reg = SOC_REG_UNIQUE_ACC(unit, obm_port_config_regs[0])[0];
    soc_reg_field_set(unit, reg, &rval_port_cfg, PORT_PRIf, 2);

    rval_max_usage_cfg = 0;
    reg = SOC_REG_UNIQUE_ACC(unit, obm_max_usage_regs[0])[0];
    soc_reg_field_set(unit, reg, &rval_max_usage_cfg, PRIORITY_SELECTf, 2);

    sal_memset(entry, 0, sizeof(idb_obm0_pri_map_port0_entry_t));
    mem = SOC_MEM_UNIQUE_ACC(unit, obm_pri_map_mem[0][0])[0];

    count = COUNTOF(obm_ob_pri_fields);
    for (i = 0; i < count; i++) {
        soc_mem_field32_set(unit, mem, entry, obm_ob_pri_fields[i], 2);
    }

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        phy_port_base = PORT_BLOCK_BASE_PORT(port);
        pipe = si->port_pipe[port];
        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);
        obm = clport & 0xF;

        /* Set cell assembly mode then toggle reset to send initial credit
         * to EP */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_ca_ctrl_regs[obm])[pipe];
        rval0 = 0;
        soc_reg_field_set(unit, reg, &rval0, PORT_MODEf,
                          hw_mode_values[tdm->port_ratio[clport]]);

        rval1 = rval0;
        /* PORT_RESETf is a 4 bit field, set to all one's in the next step */
        soc_reg_field_set(unit, reg, &rval0, PORT0_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT1_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT2_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT3_RESETf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval1));

        if (!SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
            continue;
        }

        /* Enable oversub */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_ctrl_regs[obm])[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval0));
        for (lane = 0; lane < _TD3_PORTS_PER_PBLK; lane++) {
            if (si->port_p2l_mapping[phy_port_base + lane] == -1) {
                continue;
            }
            field = obm_bypass_fields[lane];
            soc_reg_field_set(unit, reg, &rval0, field, 1);
            field = obm_oversub_fields[lane];
            soc_reg_field_set(unit, reg, &rval0, field, 1);
            lport = si->port_p2l_mapping[phy_port_base + lane];
            /* Update CA_SOP settings */
            if (si->port_speed_max[lport] < 100000) {
                field = obm_ca_sop_fields[lane];
                soc_reg_field_set(unit, reg, &rval0, field, 0);
            }
        }

        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        if (!SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
            continue;
        }

        /* Configure buffer */
        for (lane = 0; lane < _TD3_PORTS_PER_PBLK; lane++) {
            port = si->port_p2l_mapping[phy_port_base + lane];
            if (port == -1) {
                continue;
            }
            num_lanes = si->port_num_lanes[port];
            reg = SOC_REG_UNIQUE_ACC(unit, obm_buffer_config_regs[obm])[pipe];
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, lane, &rval0));
            fval = obm_settings[num_lanes].buffer_start[lane];
            soc_reg_field_set(unit, reg, &rval0, BUFFER_STARTf, fval);
            fval = obm_settings[num_lanes].buffer_end[lane];
            soc_reg_field_set(unit, reg, &rval0, BUFFER_ENDf, fval);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, lane, rval0));
        }

        /* Configure shared config */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_shared_regs[obm])[pipe];
        SOC_IF_ERROR_RETURN
            (soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, SOP_DISCARD_MODEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, REG_PORT_ANY, 0, rval64));

        if (lossless) {
            if ((tdm->ovs_ratio_x1000[pipe][0] + tdm->ovs_ratio_x1000[pipe][1]) >= 1800) { /* ratio >= 1.8 */
                oversub_ratio_idx = 2;
            } else if ((tdm->ovs_ratio_x1000[pipe][0] + tdm->ovs_ratio_x1000[pipe][1]) >= 1500) { /* ratio >= 1.5 */
                oversub_ratio_idx = 1;
            } else {
                oversub_ratio_idx = 0;
            }

            for (lane = 0; lane < _TD3_PORTS_PER_PBLK; lane++) {
                port = si->port_p2l_mapping[phy_port_base + lane];
                if (port == -1) {
                    continue;
                }
                num_lanes = si->port_num_lanes[port];

                /* Configure threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                                      4604);
                fval = obm_settings[num_lanes].lossy_low_pri[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_LOW_PRIf, fval);
                fval = obm_settings[num_lanes].lossy_discard[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_DISCARDf, fval);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_1_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[num_lanes].discard_limit;
                soc_reg64_field32_set(unit, reg, &rval64, DISCARD_LIMITf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_DISCARDf,
                                      4604);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[num_lanes].lossless_xoff;
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                                      fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XONf,
                                      fval - 30);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XOFFf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XONf,
                                      fval - 30);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control threshold_1 */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_1_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[num_lanes].port_xoff[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XOFFf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XONf, fval - 30);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control config */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_config_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, PORT_FC_ENf, 1);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_FC_ENf, 1);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_FC_ENf, 0);
                soc_reg64_field32_set(unit, reg, &rval64,
                                      LOSSLESS0_PRIORITY_PROFILEf, 0xffff);
                soc_reg64_field32_set(unit, reg, &rval64,
                                      LOSSLESS1_PRIORITY_PROFILEf, 0);
                soc_reg64_field32_set(unit, reg, &rval64,
                                      XOFF_REFRESH_TIMERf, 0x100);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* OBM Port config */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_port_config_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, lane,
                                   rval_port_cfg));
                mem = SOC_MEM_UNIQUE_ACC
                        (unit, obm_pri_map_mem[obm][lane])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));
            }
            reg = SOC_REG_UNIQUE_ACC(unit, obm_max_usage_regs[obm])[pipe];
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval_max_usage_cfg));
        } else { /* lossy */
            for (lane = 0; lane < _TD3_PORTS_PER_PBLK; lane++) {
                port = si->port_p2l_mapping[phy_port_base + lane];
                if (port == -1) {
                    continue;
                }
                num_lanes = si->port_num_lanes[port];

                /* Configure threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[num_lanes].discard_limit;
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_DISCARDf,
                                      fval - 4);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                                      4604);
                fval = obm_settings[num_lanes].lossy_only_lossy_low_pri;
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_LOW_PRIf, fval);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure threshold_1 */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_1_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[num_lanes].discard_limit;
                soc_reg64_field32_set(unit, reg, &rval64, DISCARD_LIMITf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_DISCARDf,
                                      4604);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_1_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XOFFf, 4604);
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XONf, 4604);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                                      4604);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XONf, 4604);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XONf, 4604);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XOFFf,
                                      4604);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control config */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_config_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64,
                                      XOFF_REFRESH_TIMERf, 0x100);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));
            }
        }
    }

    /* Toggle cpu port cell assembly reset to send initial credit to EP */
    reg = IDB_CA_CPU_CONTROLr;
    rval0 = 0;
    soc_reg_field_set(unit, reg, &rval0, PORT_RESETf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
    sal_msleep(5);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));

    /* Toggle loopback cell assembly reset to send initial credit to EP */
    reg = IDB_CA_LPBK_CONTROLr;
    rval0 = 0;
    soc_reg_field_set(unit, reg, &rval0, PORT_RESETf, 1);
    PBMP_LB_ITER(unit, port) {
        reg = SOC_REG_UNIQUE_ACC(unit, IDB_CA_LPBK_CONTROLr)
                                            [si->port_pipe[port]] ;
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));
    }

    soc_td3_set_idb_dpp_ctrl(unit);

    return SOC_E_NONE;
}
#ifdef BCM_WARM_BOOT_SUPPORT

#define SOC_TDM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_TDM_WB_DEFAULT_VERSION            SOC_TDM_WB_VERSION_1_0

int soc_td3_tdm_scache_allocate(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *tdm_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int stable_size;
    int default_ver = SOC_TDM_WB_DEFAULT_VERSION;
    int ilen, ovs_size, pkt_shp_size, hpipes;

    ilen = sizeof(int);
    ovs_size = TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE *
               TRIDENT3_OVS_GROUP_COUNT_PER_HPIPE *
               TRIDENT3_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE *
                   TRIDENT3_PKT_SCH_LENGTH;
    hpipes = TRIDENT3_PIPES_PER_DEV * TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * TRIDENT3_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * TRIDENT3_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * TRIDENT3_TDM_LENGTH) + /* IDB TDM info on pipe1 */
                 (ilen * TRIDENT3_TDM_LENGTH) + /* MMU TDM info on pipe1 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe1 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe1     */
                 (ilen * TRIDENT3_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * TRIDENT3_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * TRIDENT3_PBLKS_PER_DEV)     + /* port ratio */
                 (ilen * hpipes);                  /* oversub ratio */
    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_TDM_HANDLE, 0);
    /* check to see if an scache table has been configured */
    rv = soc_stable_size_get(unit, &stable_size);
    if (SOC_FAILURE(rv) || stable_size <= 0) {
        return rv;
    }

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      TRUE, &alloc_get,
                                      &tdm_scache_ptr,
                                      default_ver,
                                      NULL);

    if (rv  == SOC_E_CONFIG) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    /* NotRequired but just to confirm Get the pointer for the Level 2 cache */
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: scache memory for tdm size mismatch"
                  "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }

    if (NULL == tdm_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: scache memory not allocated for tdm"
                  "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td3_tdm_scache_sync
 * Purpose:
 *      Record TDM info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
 *    IDB TDM info on pipe0
 *    MMU TDM info on pipe0
 *    Oversub group info on pipe0
 *    pkt shaper info on pipe0
 *    IDB TDM info on pipe1
 *    MMU TDM info on pipe1
 *    Oversub group info on pipe1
 *    pkt shaper info on pipe1
 *    pblk id of phy-port
 *    port ratio
 *    oversub ratio
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int soc_td3_tdm_scache_sync(int unit)
{
    uint8 *tdm_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    uint32 var_size = 0;
    uint32 scache_offset=0;
    int rv = 0;
    int ilen, ovs_size, pkt_shp_size, hpipes, phy_port;
    _soc_trident3_tdm_temp_t  *tdm = SOC_CONTROL(unit)->tdm_info;

    ilen = sizeof(int);
    ovs_size = TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE *
               TRIDENT3_OVS_GROUP_COUNT_PER_HPIPE *
               TRIDENT3_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE *
                   TRIDENT3_PKT_SCH_LENGTH;
    hpipes = TRIDENT3_PIPES_PER_DEV * TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * TRIDENT3_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * TRIDENT3_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * TRIDENT3_TDM_LENGTH) + /* IDB TDM info on pipe1 */
                 (ilen * TRIDENT3_TDM_LENGTH) + /* MMU TDM info on pipe1 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe1 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe1     */
                 (ilen * TRIDENT3_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * TRIDENT3_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * TRIDENT3_PBLKS_PER_DEV)     + /* port ratio */
                 (ilen * hpipes);                  /* oversub ratio */
    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_TDM_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      FALSE, &alloc_get,
                                      &tdm_scache_ptr,
                                      SOC_TDM_WB_DEFAULT_VERSION,
                                      NULL);
    if (rv == SOC_E_NOT_FOUND) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: scache memory for tdm size mismatch"
                  "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }
    if (NULL == tdm_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: scache memory not allocated for tdm"
                  "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /* IDB TDM info on pipe0 */
    var_size = ilen * TRIDENT3_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[0].idb_tdm,
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe0 */
    var_size = ilen * TRIDENT3_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[0].mmu_tdm,
               var_size);
    scache_offset += var_size;

    /* Oversub group info on pipe0 */
    var_size = ilen * ovs_size;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[0].ovs_tdm,
               var_size);
    scache_offset += var_size;

    /* pkt shaper info on pipe0 */
    var_size = ilen * pkt_shp_size;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[0].pkt_shaper_tdm,
               var_size);
    scache_offset += var_size;

    /* IDB TDM info on pipe1 */
    var_size = ilen * TRIDENT3_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[1].idb_tdm,
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe1 */
    var_size = ilen * TRIDENT3_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[1].mmu_tdm,
               var_size);
    scache_offset += var_size;

    /* Oversub group info on pipe1 */
    var_size = ilen * ovs_size;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[1].ovs_tdm,
               var_size);
    scache_offset += var_size;

    /* pkt shaper info on pipe1 */
    var_size = ilen * pkt_shp_size;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[1].pkt_shaper_tdm,
               var_size);
    scache_offset += var_size;

    /* hpipe id of phy-port */
    for (phy_port = 0; phy_port < TRIDENT3_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm_scache_ptr[scache_offset],
                   &tdm->pblk_info[phy_port].pblk_hpipe_num,
                   var_size);
        scache_offset += var_size;
    }

    /* pblk id of phy-port */
    for (phy_port = 0; phy_port < TRIDENT3_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm_scache_ptr[scache_offset],
                   &tdm->pblk_info[phy_port].pblk_cal_idx,
                   var_size);
        scache_offset += var_size;
    }

    /* port ratio */
    var_size = ilen * TRIDENT3_PBLKS_PER_DEV;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->port_ratio,
               var_size);
    scache_offset += var_size;

    /* oversub ratio */
    var_size = ilen * hpipes;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->ovs_ratio_x1000,
               var_size);
    scache_offset += var_size;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td3_tdm_scache_recovery
 * Purpose:
 *      Recover TDM info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
 *    IDB TDM info on pipe0
 *    MMU TDM info on pipe0
 *    Oversub group info on pipe0
 *    pkt shaper info on pipe0
 *    IDB TDM info on pipe1
 *    MMU TDM info on pipe1
 *    Oversub group info on pipe1
 *    pkt shaper info on pipe1
 *    hpipe id of phy-port
 *    pblk id of phy-port
 *    port ratio
 *    oversub ratio
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int soc_td3_tdm_scache_recovery(int unit)
{
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int rv = SOC_E_NONE;
    uint8 *tdm_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    uint32 scache_offset=0;
    uint32 var_size = 0;
    uint16 recovered_ver = 0;
    int ilen, ovs_size, pkt_shp_size, hpipes, phy_port;
    _soc_trident3_tdm_temp_t *tdm = SOC_CONTROL(unit)->tdm_info;

    ilen = sizeof(int);
    ovs_size = TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE *
               TRIDENT3_OVS_GROUP_COUNT_PER_HPIPE *
               TRIDENT3_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE *
                   TRIDENT3_PKT_SCH_LENGTH;
    hpipes = TRIDENT3_PIPES_PER_DEV * TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * TRIDENT3_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * TRIDENT3_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * TRIDENT3_TDM_LENGTH) + /* IDB TDM info on pipe1 */
                 (ilen * TRIDENT3_TDM_LENGTH) + /* MMU TDM info on pipe1 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe1 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe1     */
                 (ilen * TRIDENT3_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * TRIDENT3_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * TRIDENT3_PBLKS_PER_DEV)     + /* port ratio */
                 (ilen * hpipes);                  /* oversub ratio */
    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_TDM_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      FALSE, &alloc_get,
                                      &tdm_scache_ptr,
                                      SOC_TDM_WB_DEFAULT_VERSION,
                                      &recovered_ver);
    if (SOC_FAILURE(rv)) {
        if ((rv == SOC_E_CONFIG) ||
            (rv == SOC_E_NOT_FOUND)) {
            /* warmboot file does not contain this
             * module, or the warmboot state does not exist.
             * in this case return SOC_E_NOT_FOUND
             */
            return SOC_E_NOT_FOUND;
        } else {
            /* Only Level2 - flexport treat this as a error */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                      "Failed to recover scache data - %s\n"),soc_errmsg(rv)));
            return rv;
        }
    }
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: scache recovery for tdm"
                  "%s()[LINE:%d] DONE \n"),
                  FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }

    if (NULL == tdm_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "ERROR: scache recovery for tdm"
                  "%s()[LINE:%d] DONE \n"),
                  FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /* IDB TDM info on pipe0 */
    var_size = ilen * TRIDENT3_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[0].idb_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe0 */
    var_size = ilen * TRIDENT3_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[0].mmu_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Oversub group info on pipe0 */
    var_size = ilen * ovs_size;
    sal_memcpy(tdm->tdm_pipe[0].ovs_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* pkt shaper info on pipe0 */
    var_size = ilen * pkt_shp_size;
    sal_memcpy(tdm->tdm_pipe[0].pkt_shaper_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* IDB TDM info on pipe1 */
    var_size = ilen * TRIDENT3_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[1].idb_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe1 */
    var_size = ilen * TRIDENT3_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[1].mmu_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Oversub group info on pipe1 */
    var_size = ilen * ovs_size;
    sal_memcpy(tdm->tdm_pipe[1].ovs_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* pkt shaper info on pipe1 */
    var_size = ilen * pkt_shp_size;
    sal_memcpy(tdm->tdm_pipe[1].pkt_shaper_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* hpipe id of phy-port */
    for (phy_port = 0; phy_port < TRIDENT3_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm->pblk_info[phy_port].pblk_hpipe_num,
                   &tdm_scache_ptr[scache_offset],
                   var_size);
        scache_offset += var_size;
    }

    /* pblk id of phy-port */
    for (phy_port = 0; phy_port < TRIDENT3_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm->pblk_info[phy_port].pblk_cal_idx,
                   &tdm_scache_ptr[scache_offset],
                   var_size);
        scache_offset += var_size;
    }

    /* port ratio */
    var_size = ilen * TRIDENT3_PBLKS_PER_DEV;
    sal_memcpy(tdm->port_ratio,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* oversub ratio */
    var_size = ilen * hpipes;
    sal_memcpy(tdm->ovs_ratio_x1000,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#define TD3_UFT_HASH_OFFSET_MAX     51
#define TD3_EUAT_HASH_OFFSET_MAX    53
#define TD3_UAT_8K_HASH_OFFSET_MAX  53
#define TD3_UAT_16K_HASH_OFFSET_MAX 52

int
soc_trident3_uft_uat_config(int unit) {
    int index, shared_bank_offset = 2;
    int num_l2_banks, num_l3_banks, num_fpem_banks = 0;
    int dedicated_banks = 2;
    int num_entries, shared_hash_offset;
    int uat_16k_banks = 0, uat_8k_banks = 0;
    int mpls_16k_banks, mpls_8k_banks;
    int vlan_xlate_1_16k_banks, vlan_xlate_1_8k_banks;
    int vlan_xlate_2_16k_banks, vlan_xlate_2_8k_banks;
    int egr_vlan_xlate_1_banks, egr_vlan_xlate_2_banks;
    int m, mem_sz;
    uint32 hash_control_entry[4], shared_hash_control_entry[4],
    shared_8k_hash_control_entry[4];
    soc_mem_t dedicated_bank_mems[] = {
        ING_DNAT_ADDRESS_TYPE_HASH_CONTROLm,
        EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLm,
        ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLm,
        SUBPORT_ID_TO_SGPP_MAP_HASH_CONTROLm
    };

    static const soc_field_t logical_to_physical_fields[] = {
        LOGICAL_BANK_0_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_4_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_5_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_6_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_7_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_8_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_9_PHYSICAL_BANK_LOCATIONf
    };
    static const soc_field_t dedicated_fields[] = {
        HASH_OFFSET_DEDICATED_BANK_0f, HASH_OFFSET_DEDICATED_BANK_1f
    };
    static const soc_field_t shared_fields[] = {
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf, B3_HASH_OFFSETf,
        B4_HASH_OFFSETf, B5_HASH_OFFSETf, B6_HASH_OFFSETf, B7_HASH_OFFSETf
    };
    /* UFT/UAT control memories programming */
    SOC_IF_ERROR_RETURN
        (soc_trident3_hash_bank_count_get(unit, L2Xm, &num_l2_banks));
    SOC_IF_ERROR_RETURN
        (soc_trident3_hash_bank_count_get(unit, L3_ENTRY_ONLY_SINGLEm,
                                          &num_l3_banks));
    SOC_IF_ERROR_RETURN
        (soc_trident3_hash_bank_count_get(unit, EXACT_MATCH_2m,
                                          &num_fpem_banks));
    SOC_IF_ERROR_RETURN
        (soc_trident3_hash_bank_count_get(unit, EGR_VLAN_XLATE_1_SINGLEm,
                                          &egr_vlan_xlate_1_banks));
    SOC_IF_ERROR_RETURN
        (soc_trident3_hash_bank_count_get(unit, EGR_VLAN_XLATE_2_SINGLEm,
                                          &egr_vlan_xlate_2_banks));

    if (soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m)) {
        int non_alpm = num_fpem_banks + num_l3_banks +
                       num_l2_banks - 2 * (dedicated_banks);
        sal_memset(hash_control_entry, 0,
                    sizeof(uft_shared_banks_control_entry_t));
        SOC_IF_ERROR_RETURN
        (READ_UFT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                             hash_control_entry));
        if ((non_alpm) && (non_alpm <= 4)) {
            /* If Shared banks are used between ALPM and non-ALPM memories,
             * then ALPM uses Shared Bank B2, B3, B4, B5 and non-ALPM uses
             * B6, B7, B8, B9 banks
             */
            soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                                 hash_control_entry, ALPM_MODEf, 1);
            soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                                 hash_control_entry, BANK_BYPASS_LPf, 0xf);
            num_shared_alpm_banks[unit] = 4;
            shared_bank_offset += 4;
        } else {
            soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                                 hash_control_entry, ALPM_MODEf, 0);
            soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                                 hash_control_entry, BANK_BYPASS_LPf, 0xff);
            num_shared_alpm_banks[unit] = 8;
        }
        SOC_IF_ERROR_RETURN
        (WRITE_UFT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             hash_control_entry));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ISS_LOG_TO_PHY_BANK_MAP_2r,
                                    REG_PORT_ANY, ALPM_BANK_MODEf,
                                    num_shared_alpm_banks[unit] <= 4));
    }

    sal_memset(shared_hash_control_entry, 0,
                sizeof(uft_shared_banks_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_UFT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                        shared_hash_control_entry));
    sal_memset(hash_control_entry, 0, sizeof(l2_entry_hash_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_L2_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0, hash_control_entry));
    soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm , hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, (((1 << dedicated_banks) - 1)
                        | (((1 << (num_l2_banks - dedicated_banks)) - 1) <<
                        shared_bank_offset)));
    for (index = dedicated_banks ; index < num_l2_banks ; index++) {
        soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm , hash_control_entry,
                            logical_to_physical_fields[index],
                            index + shared_bank_offset - dedicated_banks);
    }

    for (index = 0; index < dedicated_banks; index++) {
        shared_hash_offset = (index == (num_l2_banks-1))? TD3_UFT_HASH_OFFSET_MAX :
                              ((index * TD3_UFT_HASH_OFFSET_MAX) / (num_l2_banks-1));
        soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm, hash_control_entry,
                            dedicated_fields[index], shared_hash_offset);
    }

    for (index = dedicated_banks; index < num_l2_banks; index++) {
        shared_hash_offset = (index == (num_l2_banks-1))? TD3_UFT_HASH_OFFSET_MAX :
                              ((index * TD3_UFT_HASH_OFFSET_MAX) / (num_l2_banks-1));
        soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index -
                            dedicated_banks], shared_hash_offset);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_L2_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0, hash_control_entry));

    /* Memories with only dedicated banks */
    mem_sz = sizeof(dedicated_bank_mems) / sizeof(dedicated_bank_mems[0]);
    for (m = 0; m < mem_sz; m++) {
        sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));
        SOC_IF_ERROR_RETURN(
            soc_mem_read(unit, dedicated_bank_mems[m], MEM_BLOCK_ANY, 0,
            hash_control_entry));
        for (index = 0; index < dedicated_banks; index++) {
            soc_mem_field32_set(unit, dedicated_bank_mems[m],
                hash_control_entry, dedicated_fields[index], 0 + (index * 32));
        }
        SOC_IF_ERROR_RETURN(
            soc_mem_write(unit, dedicated_bank_mems[m], MEM_BLOCK_ANY, 0,
            hash_control_entry));
    }

    /* Exact match */
    sal_memset(hash_control_entry, 0, sizeof(exact_match_hash_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                            hash_control_entry));
    shared_bank_offset += (num_l2_banks - dedicated_banks) ;

    soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm, hash_control_entry,
                      HASH_TABLE_BANK_CONFIGf,
                      ((1 << num_fpem_banks) - 1) << shared_bank_offset);
    for (index = 0 ; index < num_fpem_banks; index++) {
        shared_hash_offset = (index == (num_fpem_banks-1)) ? TD3_UFT_HASH_OFFSET_MAX  :
                              ((index * TD3_UFT_HASH_OFFSET_MAX) / (num_fpem_banks-1));
        soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm , hash_control_entry,
                          logical_to_physical_fields[index], index +
                          shared_bank_offset);
        soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry,
                            shared_fields[shared_bank_offset + index -
                            dedicated_banks], shared_hash_offset);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                            hash_control_entry));

    sal_memset(hash_control_entry, 0, sizeof(l3_entry_hash_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                        hash_control_entry));
    shared_bank_offset += num_fpem_banks;

    soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, ((((1 << (num_l3_banks -
                        dedicated_banks)) - 1) << shared_bank_offset) |
                        (((1 << dedicated_banks) - 1) << 10)));

    for (index = 0; index < dedicated_banks; index++) {
        shared_hash_offset = (index == (num_l3_banks-1))? TD3_UFT_HASH_OFFSET_MAX :
                              ((index * TD3_UFT_HASH_OFFSET_MAX)/(num_l3_banks-1));
        soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm, hash_control_entry,
                            dedicated_fields[index], shared_hash_offset);
    }

    for (index = dedicated_banks ; index < num_l3_banks ; index++) {
        shared_hash_offset = (index == (num_l3_banks-1))? TD3_UFT_HASH_OFFSET_MAX :
                              ((index * TD3_UFT_HASH_OFFSET_MAX)/(num_l3_banks-1));
        soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm, hash_control_entry,
                          logical_to_physical_fields[index],
                          index + shared_bank_offset - dedicated_banks);
        soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry,
                            shared_fields[shared_bank_offset + index -
                            (dedicated_banks * 2)], shared_hash_offset);
    }
    SOC_IF_ERROR_RETURN
        (WRITE_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                        hash_control_entry));
    SOC_IF_ERROR_RETURN
        (WRITE_UFT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                        shared_hash_control_entry));

    sal_memset(shared_hash_control_entry, 0,
                sizeof(ep_uat_shared_banks_control_entry_t));
    sal_memset(hash_control_entry, 0,
                sizeof(egr_vlan_xlate_1_hash_control_entry_t));

    SOC_IF_ERROR_RETURN
        (READ_EP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                            shared_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (READ_EGR_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                hash_control_entry));
    soc_mem_field32_set(unit, EGR_VLAN_XLATE_1_HASH_CONTROLm,
                        hash_control_entry, HASH_TABLE_BANK_CONFIGf,
                        ((1 << egr_vlan_xlate_1_banks) - 1)) ;

    for(index = 0; index < egr_vlan_xlate_1_banks; index++) {
        shared_hash_offset = (index == (egr_vlan_xlate_1_banks-1))?
                               TD3_EUAT_HASH_OFFSET_MAX :
                               ((index * TD3_EUAT_HASH_OFFSET_MAX) / (egr_vlan_xlate_1_banks-1));
        soc_mem_field32_set(unit, EGR_VLAN_XLATE_1_HASH_CONTROLm,
                            hash_control_entry,
                            logical_to_physical_fields[index],
                            index);
        soc_mem_field32_set(unit, EP_UAT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index],
                            shared_hash_offset);
    }
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));

    sal_memset(hash_control_entry, 0,
                sizeof(egr_vlan_xlate_2_hash_control_entry_t));

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                hash_control_entry));
    soc_mem_field32_set(unit, EGR_VLAN_XLATE_2_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf,
                        ((1 << egr_vlan_xlate_2_banks) - 1) <<
                        egr_vlan_xlate_1_banks) ;

    for(index = 0; index < egr_vlan_xlate_2_banks; index++) {
        shared_hash_offset = (index == (egr_vlan_xlate_2_banks-1))?
                               TD3_EUAT_HASH_OFFSET_MAX :
                               ((index * TD3_EUAT_HASH_OFFSET_MAX) / (egr_vlan_xlate_2_banks-1));
        soc_mem_field32_set(unit, EGR_VLAN_XLATE_2_HASH_CONTROLm,
                            hash_control_entry,
                            logical_to_physical_fields[index],
                            index + egr_vlan_xlate_1_banks);
        soc_mem_field32_set(unit, EP_UAT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index +
                            egr_vlan_xlate_1_banks], shared_hash_offset);
    }
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
    SOC_IF_ERROR_RETURN
        (WRITE_EP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                shared_hash_control_entry));

    sal_memset(shared_hash_control_entry, 0,
                sizeof(ip_uat_16k_shared_banks_control_entry_t));
    sal_memset(shared_8k_hash_control_entry, 0,
                sizeof(ip_uat_8k_shared_banks_control_entry_t));
    sal_memset(hash_control_entry, 0,
                sizeof(mpls_entry_hash_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_IP_UAT_16K_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                shared_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (READ_IP_UAT_8K_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                shared_8k_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (READ_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                hash_control_entry));

    num_entries = soc_mem_index_count(unit, MPLS_ENTRY_SINGLEm);
    SOC_IF_ERROR_RETURN(soc_trident3_iuat_sizing(unit, num_entries,
                                                    &uat_16k_banks, &uat_8k_banks));
    mpls_16k_banks = uat_16k_banks;
    mpls_8k_banks = uat_8k_banks;
    soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, (((1 << mpls_8k_banks) - 1) |
                        (((1 << mpls_16k_banks) - 1) << 4)));

    for(index = 0; index < mpls_8k_banks; index++) {
        shared_hash_offset = (index == (mpls_8k_banks-1))?
                               TD3_UAT_8K_HASH_OFFSET_MAX :
                               ((index * TD3_UAT_8K_HASH_OFFSET_MAX) / (mpls_8k_banks-1));
        soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, hash_control_entry,
                            logical_to_physical_fields[index], index);
        soc_mem_field32_set(unit, IP_UAT_8K_SHARED_BANKS_CONTROLm,
                            shared_8k_hash_control_entry, shared_fields[index],
                            shared_hash_offset);
    }

    for(index = 0; index < mpls_16k_banks; index++) {
        shared_hash_offset = (index == (mpls_16k_banks-1))?
                               TD3_UAT_16K_HASH_OFFSET_MAX :
                               ((index * TD3_UAT_16K_HASH_OFFSET_MAX) / (mpls_16k_banks-1));
        soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm,
                            hash_control_entry,
                            logical_to_physical_fields[index + mpls_8k_banks], index + 4);
        soc_mem_field32_set(unit, IP_UAT_16K_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index],
                            shared_hash_offset);
    }
    SOC_IF_ERROR_RETURN
        (WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));

    sal_memset(hash_control_entry, 0,
                sizeof(vlan_xlate_1_hash_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                hash_control_entry));
    num_entries = soc_mem_index_count(unit, VLAN_XLATE_1_SINGLEm);
    SOC_IF_ERROR_RETURN(
    soc_trident3_iuat_sizing(unit, num_entries, &uat_16k_banks,
                                                    &uat_8k_banks));
    vlan_xlate_1_16k_banks = uat_16k_banks - mpls_16k_banks;
    vlan_xlate_1_8k_banks = uat_8k_banks - mpls_8k_banks;
    soc_mem_field32_set(unit, VLAN_XLATE_1_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, ((((1 << vlan_xlate_1_8k_banks)
                        - 1) << mpls_8k_banks) |
                        (((1 << vlan_xlate_1_16k_banks) - 1) << (4 +
                        mpls_16k_banks))));

    for(index = 0; index < vlan_xlate_1_8k_banks; index++) {
        shared_hash_offset = (index == (vlan_xlate_1_8k_banks-1))?
                               TD3_UAT_8K_HASH_OFFSET_MAX :
                               ((index * TD3_UAT_8K_HASH_OFFSET_MAX) / (vlan_xlate_1_8k_banks-1));
        soc_mem_field32_set(unit, VLAN_XLATE_1_HASH_CONTROLm,
                            hash_control_entry,
                            logical_to_physical_fields[index], index +
                            mpls_8k_banks);
        soc_mem_field32_set(unit, IP_UAT_8K_SHARED_BANKS_CONTROLm,
                            shared_8k_hash_control_entry, shared_fields[index +
                            mpls_8k_banks], shared_hash_offset);
    }

    for(index = 0; index < vlan_xlate_1_16k_banks; index++) {
        shared_hash_offset = (index == (vlan_xlate_1_16k_banks-1))?
                               TD3_UAT_16K_HASH_OFFSET_MAX :
                               ((index * TD3_UAT_16K_HASH_OFFSET_MAX) / (vlan_xlate_1_16k_banks-1));
        soc_mem_field32_set(unit, VLAN_XLATE_1_HASH_CONTROLm,
                                hash_control_entry,
                                logical_to_physical_fields[index + vlan_xlate_1_8k_banks], index +
                                mpls_16k_banks + 4);
        soc_mem_field32_set(unit, IP_UAT_16K_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index +
                            mpls_16k_banks], shared_hash_offset);
    }
    SOC_IF_ERROR_RETURN
        (WRITE_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));

    sal_memset(hash_control_entry, 0,
                sizeof(vlan_xlate_2_hash_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                hash_control_entry));
    num_entries = soc_mem_index_count(unit, VLAN_XLATE_2_SINGLEm);
    SOC_IF_ERROR_RETURN(soc_trident3_iuat_sizing(unit, num_entries,
                            &uat_16k_banks, &uat_8k_banks));
    vlan_xlate_2_16k_banks = uat_16k_banks - mpls_16k_banks -
                                vlan_xlate_1_16k_banks;
    vlan_xlate_2_8k_banks = uat_8k_banks - mpls_8k_banks -
                                vlan_xlate_1_8k_banks;

    soc_mem_field32_set(unit, VLAN_XLATE_2_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, ((((1 << vlan_xlate_2_8k_banks)
                        - 1) << (mpls_8k_banks + vlan_xlate_1_8k_banks)) |
                        (((1 << vlan_xlate_2_16k_banks) - 1) << (4 +
                        mpls_16k_banks + vlan_xlate_1_16k_banks ))));

    for(index = 0; index < vlan_xlate_2_8k_banks; index++) {
        shared_hash_offset = (index == (vlan_xlate_2_8k_banks-1))?
                               TD3_UAT_8K_HASH_OFFSET_MAX :
                               ((index * TD3_UAT_8K_HASH_OFFSET_MAX) / (vlan_xlate_2_8k_banks-1));
        soc_mem_field32_set(unit, VLAN_XLATE_2_HASH_CONTROLm, hash_control_entry,
                            logical_to_physical_fields[index], index +
                            mpls_8k_banks + vlan_xlate_1_8k_banks);
        soc_mem_field32_set(unit, IP_UAT_8K_SHARED_BANKS_CONTROLm,
                            shared_8k_hash_control_entry, shared_fields[index +
                            mpls_8k_banks + vlan_xlate_1_8k_banks],
                            shared_hash_offset);
    }

    for(index = 0; index < vlan_xlate_2_16k_banks; index++) {
        shared_hash_offset = (index == (vlan_xlate_2_16k_banks-1))?
                               TD3_UAT_16K_HASH_OFFSET_MAX :
                               ((index * TD3_UAT_16K_HASH_OFFSET_MAX) / (vlan_xlate_2_16k_banks-1));
        soc_mem_field32_set(unit, VLAN_XLATE_2_HASH_CONTROLm,
                                hash_control_entry,
                                logical_to_physical_fields[index + vlan_xlate_2_8k_banks], index +
                                mpls_16k_banks + vlan_xlate_1_16k_banks + 4);
        soc_mem_field32_set(unit, IP_UAT_16K_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index +
                            mpls_16k_banks + vlan_xlate_1_16k_banks],
                            shared_hash_offset);
    }
    SOC_IF_ERROR_RETURN
        (WRITE_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
    SOC_IF_ERROR_RETURN
        (WRITE_IP_UAT_8K_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                shared_8k_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (WRITE_IP_UAT_16K_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                shared_hash_control_entry));

    /* Create a mutex for s/w re-ordering */
    SOC_IF_ERROR_RETURN(soc_td3_hash_mutex_init (unit));

    return SOC_E_NONE;

}

int
soc_trident3_init_idb_memory(int unit)
{
    uint32 pipe_map;
    soc_reg_t reg;
    int pipe, chip_num_pipe;
    uint32 rval, in_progress;
    int pipe_init_usec;
    soc_timeout_t to;
    uint32 count;
    int is_sim;
	static const soc_reg_t idb_tdm_config_reg[TRIDENT3_TDM_PIPES_PER_DEV] = {
        	IS_TDM_CONFIG_PIPE0r, IS_TDM_CONFIG_PIPE1r
        };
	static const soc_reg_t idb_opp_sched_config[TRIDENT3_TDM_PIPES_PER_DEV] = {
        	IS_OPP_SCHED_CFG_PIPE0r, IS_OPP_SCHED_CFG_PIPE1r
        };

    is_sim = (SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM) ? 1 : 0;

    soc_trident3_pipe_map_get(unit, &pipe_map);

    /* While full chip emulaiton database is used, if only PIPE0 ports are configured
     * NUM_PIPE is overwritten to 1, however we still need to reset entire chip using
     * chip_num_pipe which reflects total number of pipes in the chip.
     */
    chip_num_pipe = soc_property_get(unit, "num_pipe", NUM_PIPE(unit));

    /* Initialize IDB memory */
    rval = 0;
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, REGIONf, 1);
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        count = 0x40000;
    } else
#endif
    {
        count = 0x100;
    }
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, count);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IDB memory initialization done */
    in_progress = pipe_map;
    do {
        for (pipe = 0; pipe < chip_num_pipe && in_progress; pipe++) {
            reg = SOC_REG_UNIQUE_ACC(unit, ING_HW_RESET_CONTROL_2r)[pipe];
            if (in_progress & (1 << pipe)) { /* not done yet */
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                    in_progress ^= (1 << pipe);
                }
            }
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET timeout\n"), unit));
            break;
        }
    } while ((in_progress != 0) && (!is_sim));
    rval = 0;
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, REGIONf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    for (pipe = 0; pipe < chip_num_pipe; pipe++) {

		rval = 0;
		reg = idb_tdm_config_reg[pipe];
		soc_reg_field_set(unit, reg, &rval, ENABLEf, 1);
        	SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

		reg = idb_opp_sched_config[pipe];
        	SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        	soc_reg_field_set(unit, reg, &rval, OPP1_PORT_ENf, 1);
        	soc_reg_field_set(unit, reg, &rval, OPP1_SPACINGf, 6);
        	SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
	}

    return SOC_E_NONE;
}

int
soc_trident3_clear_all_memory(int unit)
{
    uint32 pipe_map;
    int block_info_idx;
    int is_sim;
    soc_reg_t reg;
    int pipe, port, index, count;
    uint32 rval, in_progress;
    int pipe_init_usec;
    soc_timeout_t to;
    uint32 hash_control_entry[4];
    uint32 bitmap = 0, uft_bmap=0;
    soc_mem_t hmem = L3_ENTRY_HASH_CONTROLm;
    static const struct {
        soc_mem_t mem;
        uint32 skip_flags; /* always skip on QUICKTURN or XGSSIM */
    } cam_list[] = {
        { CPU_COS_MAPm,                         BOOT_F_PLISIM },
        { DST_COMPRESSIONm,                     BOOT_F_PLISIM },
        { EFP_TCAMm,                            BOOT_F_PLISIM },
        { EGR_FIELD_EXTRACTION_PROFILE_1_TCAMm, BOOT_F_PLISIM },
        { EGR_FIELD_EXTRACTION_PROFILE_2_TCAMm, BOOT_F_PLISIM },
        { EGR_PKT_FLOW_SELECT_TCAMm,            BOOT_F_PLISIM },
        { EGR_QOS_CTRL_TCAMm,                   BOOT_F_PLISIM },
        { EGR_ZONE_0_EDITOR_CONTROL_TCAMm,      BOOT_F_PLISIM },
        { EGR_ZONE_1_EDITOR_CONTROL_TCAMm,      BOOT_F_PLISIM },
        { EGR_ZONE_2_EDITOR_CONTROL_TCAMm,      BOOT_F_PLISIM },
        { EGR_ZONE_3_EDITOR_CONTROL_TCAMm,      BOOT_F_PLISIM },
        { EGR_ZONE_4_EDITOR_CONTROL_TCAMm,      BOOT_F_PLISIM },
        { EXACT_MATCH_LOGICAL_TABLE_SELECTm,    BOOT_F_PLISIM },
        { FLEX_RTAG7_HASH_TCAMm,                BOOT_F_PLISIM },
        { IFP_LOGICAL_TABLE_SELECTm,            BOOT_F_PLISIM },
        { IFP_TCAMm,                            BOOT_F_PLISIM },
        { ING_SNATm,                            BOOT_F_PLISIM },
        { IP_MULTICAST_TCAMm,                   BOOT_F_PLISIM },
        { L2_USER_ENTRYm,                       BOOT_F_PLISIM },
        { L3_DEFIPm,                            BOOT_F_PLISIM },
        { L3_DEFIP_PAIR_128m,                   BOOT_F_PLISIM },
        { L3_TUNNELm,                           BOOT_F_PLISIM },
        { MY_STATION_TCAMm,                     BOOT_F_PLISIM },
        { MY_STATION_TCAM_2m,                   BOOT_F_PLISIM },
        { PHB_SELECT_TCAMm,                     BOOT_F_PLISIM },
        { PKT_FLOW_SELECT_TCAM_0m,              BOOT_F_PLISIM },
        { PKT_FLOW_SELECT_TCAM_1m,              BOOT_F_PLISIM },
        { PKT_FLOW_SELECT_TCAM_2m,              BOOT_F_PLISIM },
        { SRC_COMPRESSIONm,                     BOOT_F_PLISIM },
        { VFP_TCAMm,                            BOOT_F_PLISIM },
        { VLAN_SUBNETm,                         0 },/* VLAN API needs all 0 mask */
    };

    is_sim = (SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM) ? 1 : 0;

    soc_trident3_pipe_map_get(unit, &pipe_map);

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, L2_ENTRY_HASH_CONTROLm,
                                MEM_BLOCK_ALL, 0, hash_control_entry));
    uft_bmap |= soc_mem_field32_get(unit, L2_ENTRY_HASH_CONTROLm,
                                hash_control_entry, HASH_TABLE_BANK_CONFIGf);

    if (soc_mem_is_valid(unit, EXACT_MATCH_HASH_CONTROLm)) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, EXACT_MATCH_HASH_CONTROLm,
                                    MEM_BLOCK_ALL, 0, hash_control_entry));
        uft_bmap |= soc_mem_field32_get(unit, EXACT_MATCH_HASH_CONTROLm,
                                    hash_control_entry, HASH_TABLE_BANK_CONFIGf);
    }

    /* L3_ENTRY_HASH_CONTROL->HASH_TABLE_BANK_CONFIG must be set to cover shared
     * UFT banks, that are not included in other hash control during HW MEM RESET ALL
     * and also include dedicated L3 banks.
     */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, hmem, MEM_BLOCK_ALL, 0,
                                hash_control_entry));
    bitmap = soc_mem_field32_get(unit, hmem, hash_control_entry,
                                HASH_TABLE_BANK_CONFIGf);

    uft_bmap = 0xFFC & ~uft_bmap;
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        /* No UFT only dedicated banks */
        soc_mem_field32_set(unit, hmem, hash_control_entry, HASH_TABLE_BANK_CONFIGf, 0x3);
    } else
#endif
    {
        soc_mem_field32_set(unit, hmem, hash_control_entry, HASH_TABLE_BANK_CONFIGf, uft_bmap);
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, hmem, MEM_BLOCK_ALL, 0, hash_control_entry));

    /* Initial IPIPE memory */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, REGIONf, 0);
    /* Set count to # entries of largest IPIPE table */
    count = soc_mem_index_count(unit, ING_L3_NEXT_HOPm);

    if (SOC_MEM_IS_VALID(unit, BSC_AG_AGE_TABLEm)) {
        if (count < soc_mem_index_count(unit, BSC_AG_AGE_TABLEm)) {
            count = soc_mem_index_count(unit, BSC_AG_AGE_TABLEm);
        }
    }

    if (count < soc_mem_index_count(unit, L2Xm)) {
        count = soc_mem_index_count(unit, L2Xm);
    }
    if (count < soc_mem_index_count(unit, L3_ENTRY_ONLY_SINGLEm)) {
        count = soc_mem_index_count(unit, L3_ENTRY_ONLY_SINGLEm);
    }
    /*if (count < soc_mem_index_count(unit, FPEM_ECCm)) {
        count = soc_mem_index_count(unit, FPEM_ECCm);
    } */
    if (soc_mem_is_valid(unit, L3_DEFIP_ALPM_IPV4m)) {
        if (count < soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m)) {
            count = soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m);
        }
    }
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        /* 64K */
        count = 0x10000;
    }
#endif
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, count);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    /* Initial EPIPE memory */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    /* Set count to # entries in largest EPIPE table (EGR_L3_NEXT_HOP) */
    count = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, count);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IPIPE memory initialization done */
    in_progress = pipe_map;
    do {
        for (pipe = 0; pipe < NUM_PIPE(unit) && in_progress; pipe++) {
            reg = SOC_REG_UNIQUE_ACC(unit, ING_HW_RESET_CONTROL_2r)[pipe];
            if (in_progress & (1 << pipe)) { /* not done yet */
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                    if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                        in_progress ^= 1 << pipe;
                    }
            }
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET timeout\n"), unit));
            break;
        }
    } while ((in_progress != 0) && (!is_sim));

    /* Restore L3_ENTRY_HASH_CONTROL->HASH_TABLE_BANK_CONFIG value */
    soc_mem_field32_set(unit, hmem, hash_control_entry, HASH_TABLE_BANK_CONFIGf, bitmap);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, hmem, MEM_BLOCK_ALL, 0, hash_control_entry));

    /* Wait for EPIPE memory initialization done */
    in_progress = pipe_map;
    do {
        for (pipe = 0; pipe < NUM_PIPE(unit) && in_progress; pipe++) {
            reg = SOC_REG_UNIQUE_ACC(unit, EGR_HW_RESET_CONTROL_1r)[pipe];
            if (in_progress & (1 << pipe)) { /* not done yet */
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                    if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                        in_progress ^= 1 << pipe;
                    }
            }
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : EGR_HW_RESET timeout\n"), unit));
            break;
        }
    } while ((in_progress != 0) && (!is_sim));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    rval = SOC_REG_INFO(unit, EGR_HW_RESET_CONTROL_1r).rst_val_lo;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* Initial IDB memory */
    SOC_IF_ERROR_RETURN(WRITE_IDB_HW_CONTROLr(unit, 0));
    rval = 0;
    soc_reg_field_set(unit, IDB_HW_CONTROLr, &rval, IS_MEM_INITf, 1);
    SOC_IF_ERROR_RETURN(WRITE_IDB_HW_CONTROLr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_IDB_HW_CONTROLr(unit, 0));

    /* Initial PORT MIB counter */
    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        soc_reg_field_set(unit, CLPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MIB_RESETr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MIB_RESETr(unit, port, 0));
    }
    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, 0));
    }

    /* TCAM tables are not handled by hardware reset control */
    if (!SAL_BOOT_QUICKTURN && !SAL_BOOT_XGSSIM) {
        for (index = 0; index < COUNTOF(cam_list); index++) {
            if (!soc_mem_is_valid(unit, cam_list[index].mem)) {
                continue;
            }
            if (sal_boot_flags_get() & cam_list[index].skip_flags) {
                continue;
            }

            if (!SOC_MEM_IS_VALID(unit, cam_list[index].mem)) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, cam_list[index].mem, COPYNO_ALL, TRUE));
        }
    }

/* TD3-1847 */
    if (!SOC_WARM_BOOT(unit)) {
        /* Clear the higher 32k of RH_*_flowset table */
        if (soc_mem_is_valid(unit, RH_LAG_FLOWSETm)) {
            if (soc_mem_index_count(unit, RH_LAG_FLOWSETm)) {
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROL_2r,
                            REG_PORT_ANY, RH_DLB_SELECTIONf, 1));
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, RH_LAG_FLOWSETm, COPYNO_ALL, TRUE));
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROL_2r,
                            REG_PORT_ANY, RH_DLB_SELECTIONf, 0));
            }
        }
        if (soc_mem_is_valid(unit, RH_HGT_FLOWSETm)) {
            if (soc_mem_index_count(unit, RH_HGT_FLOWSETm)) {
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROL_2r,
                            REG_PORT_ANY, RH_DLB_SELECTIONf, 1));

                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROL_2r,
                            REG_PORT_ANY, HGT_LAG_FLOWSET_TABLE_CONFIGf, 2));
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, RH_HGT_FLOWSETm, COPYNO_ALL, TRUE));
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROL_2r,
                            REG_PORT_ANY, RH_DLB_SELECTIONf, 0));
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROL_2r,
                            REG_PORT_ANY, HGT_LAG_FLOWSET_TABLE_CONFIGf, 0));
            }
        }
    }

    soc_td3_rx_etype_niv[unit] = 0;
    soc_td3_rx_etype_pe[unit] = 0;

    return SOC_E_NONE;

}

/* Determine a PLL's current DCO value.  The PLL should be checked to ensure
   that it is locked prior to calling this function */
int
_soc_trident3_pll_dco_code_get(int unit, soc_td3_plls_e pll, uint32 *dco_code)
{
    soc_reg_t stat_ctrl_reg;
    soc_reg_t stat_reg;
    soc_field_t stat_sel_fld, stat_rst_fld, stat_mode_fld, stat_update_fld;
    soc_field_t stat_fld;
    uint32 rval;

    stat_sel_fld = STAT_SELECTf;
    stat_rst_fld = STAT_RESETf;
    stat_mode_fld = STAT_MODEf;
    stat_update_fld = STAT_UPDATEf;
    stat_fld = PLL_STATUSf;
    switch (pll) {
        case SOC_TD3_TS_PLL:
            stat_ctrl_reg = TOP_TS_PLL_CTRL_4r;
            stat_reg = TOP_TS_PLL_STATUSr;
            break;
        case SOC_TD3_BS_PLL0:
            stat_ctrl_reg = TOP_BS_PLL0_CTRL_4r;
            stat_reg = TOP_BS_PLL0_STATUSr;
            break;
        case SOC_TD3_BS_PLL1:
            stat_ctrl_reg = TOP_BS_PLL1_CTRL_4r;
            stat_reg = TOP_BS_PLL1_STATUSr;
            break;
        default:
            return SOC_E_PARAM;
    }

    /* Configure the stat logic to read the DCO value */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, stat_ctrl_reg, REG_PORT_ANY, 0,
                                      &rval));
    soc_reg_field_set(unit, stat_ctrl_reg, &rval, stat_sel_fld, 4); /* DCO */
    soc_reg_field_set(unit, stat_ctrl_reg, &rval, stat_mode_fld, 3); /* avg */
    soc_reg_field_set(unit, stat_ctrl_reg, &rval, stat_rst_fld, 1);
    soc_reg_field_set(unit, stat_ctrl_reg, &rval, stat_update_fld, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, stat_ctrl_reg, REG_PORT_ANY, 0,
                                      rval));

    /* Take out of reset */
    soc_reg_field_set(unit, stat_ctrl_reg, &rval, stat_rst_fld, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, stat_ctrl_reg, REG_PORT_ANY, 0,
                                      rval));

    /* Initiate a capture */
    soc_reg_field_set(unit, stat_ctrl_reg, &rval, stat_update_fld, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, stat_ctrl_reg, REG_PORT_ANY, 0,
                                      rval));

    sal_usleep(10 * MILLISECOND_USEC);

    /* Get the DCO code */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, stat_reg, REG_PORT_ANY, 0,
                                      &rval));
    *dco_code = soc_reg_field_get(unit, stat_reg, rval, stat_fld);

    /* Clear the capture state */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, stat_ctrl_reg, REG_PORT_ANY, 0,
                                      &rval));
    soc_reg_field_set(unit, stat_ctrl_reg, &rval, stat_update_fld, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, stat_ctrl_reg, REG_PORT_ANY, 0,
                                      rval));

    return SOC_E_NONE;
}

/* Linearly scale the DCO code as a function of the VCO's frequency */
int
_soc_td3_pll_dco_normalize(uint32 *dco_code, uint32 ref_freq, uint32 ndiv)
{
    static const int int_ref_freq = 50000000;

    *dco_code <<= 20;
    if (ref_freq <= 0) {
        *dco_code /=  (int_ref_freq / 100000 * ndiv);
    } else {
        *dco_code /= (ref_freq / 100000 * ndiv);
    }

    return SOC_E_NONE;
}

int soc_td3_frequency_init(int unit)
{
    static const int freq_list[]     = { 1525, 1425, 1325, 1012, 850 };
    static const int freq_sel_list[] = {    2,    3,    4,    6,   7 };

    const char *str_ratio[] = { "2:3", "1:1" };
    char *config_str;
    static const int dpp_ratio_x10_list[] = {
        15, /* core clk : pp clk ratio is 3:2 (default) */
        10  /* core clk : pp clk ratio is 1:1 */
    };

    soc_field_t fields[3];
    uint32 values[3];
    soc_info_t *si = &SOC_INFO(unit);

    int index, list_len, frequency, freq_sel=2;

    soc_trident3_max_frequency_get(unit, BCM56870_DEVICE_ID, -1, -1, &si->frequency);
    frequency = soc_property_get(unit, spn_CORE_CLOCK_FREQUENCY, -1);

    /* Frequency is not selected.., so we go default setting. Nothing to do. */
    if (frequency == -1) {
        return (SOC_E_NONE);
    }

    /* Frequency is more than max allowed, use max value. */
    if (frequency > si->frequency) {
        LOG_CLI((BSL_META_U(unit,
                            "*** Input core clock frequency %dMHz is not "
                            "supported, (max allowed : %dMHz!\n"),
                             frequency, si->frequency));

        frequency = si->frequency;
    }

    /* Check if Core CLK frequency is correct */
    list_len = COUNTOF(freq_list);
    for (index = 0; index < list_len; index++) {
        if (freq_list[index] <= si->frequency &&
            frequency == freq_list[index]) {
            si->frequency          = frequency;
            si->dpp_clk_ratio_x10  = (frequency > CCLK_FREQ_1012MHZ)?
                                            dpp_ratio_x10_list[0] :
                                            dpp_ratio_x10_list[1];
            freq_sel  = freq_sel_list[index];
            break;
        }
    }

    if (index >= list_len) {
        LOG_CLI((BSL_META_U(unit,
                            "*** Input core clock frequency %dMHz is not "
                            "supported!\n"), frequency));
        return (SOC_E_PARAM);
    }

    /* Check if user specified dpp ratio and validate it */
    config_str = soc_property_get_str(unit, spn_DPP_CLOCK_RATIO);
    if (config_str) {
        list_len = COUNTOF(dpp_ratio_x10_list);
        for (index = 0; index < list_len; index++) {
            if (!sal_strcmp(config_str, str_ratio[index])) {
                break;
            }
        }
        if (index < list_len &&
            !(si->frequency > 1012 && dpp_ratio_x10_list[index] == 10)) {
            /* ratio 1:1 with core freq larger than 1125MHz is not supported */
            si->dpp_clk_ratio_x10 = dpp_ratio_x10_list[index];
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "*** Input dpp clock frequency ratio %s is not "
                                "supported with core frequency %d!\n"),
                     config_str, si->frequency));
            return (SOC_E_PARAM);
        }
    }

    fields[0] = CORE_CLK_0_FREQ_SELf;
    values[0] = freq_sel;
    fields[1] = SW_CORE_CLK_0_SEL_ENf;
    values[1] = 1;
    fields[2] = DPP_CLK_RATIO_SELf;
    values[2] = (si->dpp_clk_ratio_x10 == 10)? 3 : 1;

    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, TOP_CORE_CLK_FREQ_SELr,
                                 REG_PORT_ANY, 3, fields, values));
    return (SOC_E_NONE);
}

int
_soc_trident3_init_hash_control_reset(int unit) {

    soc_mem_field32_set(unit, EGR_VLAN_XLATE_1_HASH_CONTROLm,
          &egr_vlan_1_hash_control, HASH_TABLE_BANK_CONFIGf, 0xf);
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &egr_vlan_1_hash_control));

    soc_mem_field32_set(unit, EGR_VLAN_XLATE_2_HASH_CONTROLm,
          &egr_vlan_2_hash_control, HASH_TABLE_BANK_CONFIGf, 0);
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &egr_vlan_2_hash_control));

    SOC_IF_ERROR_RETURN
        (WRITE_EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                     &egr_vp_vlan_mbm_hash_control));

    soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm,
          &exact_match_hash_control, HASH_TABLE_BANK_CONFIGf, 0x0);
    SOC_IF_ERROR_RETURN
        (WRITE_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &exact_match_hash_control));

    SOC_IF_ERROR_RETURN
        (WRITE_ING_DNAT_ADDRESS_TYPE_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                     &ing_dnat_hash_control));

    SOC_IF_ERROR_RETURN
        (WRITE_ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                     &ing_vp_vlan_mbm_hash_control));

    soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm,
            &l2_entry_hash_control, HASH_TABLE_BANK_CONFIGf, 0x3);
    SOC_IF_ERROR_RETURN
        (WRITE_L2_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &l2_entry_hash_control));

    soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm,
          &l3_entry_hash_control, HASH_TABLE_BANK_CONFIGf, 0xffc);
    SOC_IF_ERROR_RETURN
        (WRITE_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &l3_entry_hash_control));

    soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm,
          &mpls_entry_hash_control, HASH_TABLE_BANK_CONFIGf, 0x0);
    SOC_IF_ERROR_RETURN
        (WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &mpls_entry_hash_control));

    SOC_IF_ERROR_RETURN
        (WRITE_SUBPORT_ID_TO_SGPP_MAP_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                              &subport_hash_control));

    soc_mem_field32_set(unit, VLAN_XLATE_1_HASH_CONTROLm,
          &vlan_1_hash_control, HASH_TABLE_BANK_CONFIGf, 0xff);
    SOC_IF_ERROR_RETURN
        (WRITE_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &vlan_1_hash_control));

    soc_mem_field32_set(unit, VLAN_XLATE_2_HASH_CONTROLm,
          &vlan_2_hash_control, HASH_TABLE_BANK_CONFIGf, 0x0);
    SOC_IF_ERROR_RETURN
        (WRITE_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &vlan_2_hash_control));

    SOC_IF_ERROR_RETURN
        (WRITE_UFT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                              &uft_shared_banks_control));

    SOC_IF_ERROR_RETURN
        (WRITE_IP_UAT_16K_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                              &ip_uat_16k_shared_banks_control));

    SOC_IF_ERROR_RETURN
        (WRITE_IP_UAT_8K_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                              &ip_uat_8k_shared_banks_control));

    SOC_IF_ERROR_RETURN
        (WRITE_EP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                              &ep_uat_shared_banks_control));
     return SOC_E_NONE;
}

int
soc_trident3_chip_reset(int unit)
{
#define _SOC_TD3_BSPLL_COUNT    2
#define _SOC_TD3_PLL_CFG_FIELDS 4
#define _SOC_TD3_TEMP_MAX       125
#define _SOC_TD3_DEF_TEMP_MAX   110
#if 0
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    int num_overlay_ecmp_rh_flowset_entries;
    int ecmp_levels = 1;
#endif
#endif

    soc_reg_t reg;
    int index, parity_enable;
    uint32 to_usec, temp_mask;
    uint32 rval, temp_thr;
    uint32 dco_code, dco_code_target;
    int ref_freq;
    char blank = 0;
    static const soc_pll_16_param_t ts_pll_params[] = {
        /* 25MHz external reference clock */
        {25000000, 25, 2, 10, 3, 10, 1, 200, {10, 20, 32, 10, 200, 40}},
        /* 50MHz external reference clock */
        {50000000, 50, 2, 10, 3, 10, 1, 100, {10,  0,  0,  0, 200, 40}},
        /* 50MHz internal reference clock from core PLL */
        {       0, 50, 2, 10, 3, 10, 1, 100, {10,  0,  0,  0, 200, 40}}
    };
    static const soc_pll_16_param_t bs_pll_params[] = {
        /* 12.8MHz external reference clock */
        {12800000, 1, 2, 10, 3, 10, 1, 200, {128, 0, 0, 0, 0, 0}},
        /* 20MHz external reference clock */
        {20000000, 1, 2, 10, 3, 10, 1, 125, {125, 0, 0, 0, 0, 0}},
        /* 25MHz external reference clock */
        {25000000, 1, 2, 10, 3, 10, 1, 100, {125, 0, 0, 0, 0, 0}},
        /* 32MHz external reference clock */
        {32000000, 1, 2, 10, 3, 10, 1,  80, {128, 0, 0, 0, 0, 0}},
        /* 50MHz external reference clock */
        {50000000, 1, 2, 10, 3, 10, 1,  50, {125, 0, 0, 0, 0, 0}},
        /* 50MHz internal reference clock from core PLL */
        {       0, 0, 2, 10, 3, 10, 1,  50, {125, 0, 0, 0, 0, 0}}
    };
    static const int auto_ref = -1;
    static const uint32 dco_poison = 0xffff;

    static const soc_reg_t temp_thr_reg[] = {
        TOP_PVTMON_0_INTR_THRESHOLDr,  TOP_PVTMON_1_INTR_THRESHOLDr,
        TOP_PVTMON_2_INTR_THRESHOLDr,  TOP_PVTMON_3_INTR_THRESHOLDr,
        TOP_PVTMON_4_INTR_THRESHOLDr,  TOP_PVTMON_5_INTR_THRESHOLDr,
        TOP_PVTMON_6_INTR_THRESHOLDr,  TOP_PVTMON_7_INTR_THRESHOLDr,
        TOP_PVTMON_8_INTR_THRESHOLDr,  TOP_PVTMON_9_INTR_THRESHOLDr,
        TOP_PVTMON_10_INTR_THRESHOLDr, TOP_PVTMON_11_INTR_THRESHOLDr
    };
    static const char *temp_thr_prop[] = {
        "temp0_threshold", "temp1_threshold", "temp2_threshold",
        "temp3_threshold", "temp4_threshold", "temp5_threshold",
        "temp6_threshold", "temp7_threshold", "temp8_threshold",
        "temp9_threshold", "temp10_threshold","temp11_threshold"
    };

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /*
     * SBUS ring and block number:
     * ring 0: IP(1), LBPORT0(54), LBPORT1(51), LBPORT2(52), LBPORT3(53)
     * ring 1: EP(2)
     * ring 2: MMU_XPE(3), MMU_SC(4), MMU_GLB(5)
     * ring 3: PM7(22), PM6(21), PM5(20), PM4(19), PM3(18), PM2(17), PM1(16),
     *         PM0(15), PM31(46), PM30(45), PM29(44), PM28(43), PM27(42),
     *         PM26(41), PM25(40), PM24(39), CLPORT32(55)
     * ring 4: PM32(11), PM8(23), PM9(24), PM10(25), PM11(26), PM12(27),
     *         PM13(28), PM14(29), PM15(30), PM16(31), PM17(32), PM18(33),
     *         PM19(34), PM20(35), PM21(36), PM22(37), PM23(38)
     * ring 5: OTPC(6), AVS(59), TOP(7), SER(8)
     */


#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_0_7_OFFSET,0x52222100);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_8_15_OFFSET,0x30053005);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_16_23_OFFSET,0x43333333);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_24_31_OFFSET,0x64444444);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_32_39_OFFSET,0x76666666);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_40_47_OFFSET,0x07777777);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_48_55_OFFSET,0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_56_63_OFFSET,0x00005000);
        soc_pci_write(unit, CMIC_TOP_SBUS_TIMEOUT_OFFSET,0x5000);
    }
#endif

    /* Reset all mHost processors */
    SOC_IF_ERROR_RETURN(WRITE_MHOST_0_CR5_RST_CTRLr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_MHOST_1_CR5_RST_CTRLr(unit, 0));
    /*Reset the common UARTs */
    SOC_IF_ERROR_RETURN(WRITE_IPROCPERIPH_UART0_UART_SRRr(unit, 1));
    SOC_IF_ERROR_RETURN(WRITE_IPROCPERIPH_UART1_UART_SRRr(unit, 1));

    sal_usleep(to_usec);

#if 1
    SOC_IF_ERROR_RETURN(soc_td3_frequency_init(unit));
#endif

    /* Bring up TS PLL */
    ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, auto_ref);
    index = (ref_freq == auto_ref) ? (COUNTOF(ts_pll_params) - 1) : 0;
    dco_code_target = dco_poison;

    for (; index < COUNTOF(ts_pll_params); index++) {
        if (ref_freq != auto_ref &&
                ts_pll_params[index].ref_freq != ref_freq) {
            continue;
        }

        /* Place in reset */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REG_2r, REG_PORT_ANY,
                                    TOP_TS_PLL_RST_Lf, 0));

        /* Set N-divider value */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_TS_PLL_CTRL_0r, REG_PORT_ANY,
                                    NDIV_INTf, ts_pll_params[index].ndiv));

        /* Select internal or external clock source */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_TS_PLL_CTRL_2r, REG_PORT_ANY,
                            REFCLK_SOURCE_SELf,
                            ts_pll_params[index].ref_freq == 0 ? 0 : 1));

        /* Set new reference frequency info */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_TS_PLL_CTRL_3r, REG_PORT_ANY,
                            FREFEFF_INFOf,
                            ts_pll_params[index].ref_freq_info));

        /* Take out of reset */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REG_2r, REG_PORT_ANY,
                                    TOP_TS_PLL_RST_Lf, 1));

        /* Give time to lock */
        sal_usleep(to_usec);

        /* Check PLL lock status */
        reg = TOP_TS_PLL_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (SAL_BOOT_SIMULATION ||
                soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
            if (ref_freq == auto_ref && dco_code_target == dco_poison) {
                /* If auto freq, get DCO value for later comparison */
                SOC_IF_ERROR_RETURN(
                    _soc_trident3_pll_dco_code_get(unit, SOC_TD3_TS_PLL,
                                                    &dco_code_target));
                _soc_td3_pll_dco_normalize(&dco_code_target,
                                            ts_pll_params[index].ref_freq,
                                            ts_pll_params[index].ndiv);

                /* Start iterating over all of the available frequencies */
                index = -1;
            } else if (ref_freq == auto_ref) {
                /* Make sure the current DCO code is close to the target */
                SOC_IF_ERROR_RETURN(
                    _soc_trident3_pll_dco_code_get(unit, SOC_TD3_TS_PLL,
                                                    &dco_code));

                _soc_td3_pll_dco_normalize(&dco_code,
                                            ts_pll_params[index].ref_freq,
                                            ts_pll_params[index].ndiv);

                if (dco_code >= (dco_code_target - 400) &&
                        dco_code <= (dco_code_target + 400)) {
                    if (ts_pll_params[index].ref_freq) {
                        LOG_WARN(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                "TS_PLL locked on unit %d using "
                                "%d.%dMHz external reference clock\n"
                                "Recommended to add \"%s=%u\" "
                                "config variable\n"),
                                unit,
                                ts_pll_params[index].ref_freq / 1000000,
                                ts_pll_params[index].ref_freq / 100000 % 10,
                                spn_PTP_TS_PLL_FREF,
                                ts_pll_params[index].ref_freq));
                    } else {
                        LOG_WARN(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "TS_PLL locked on unit %d using "
                                "internal reference clock\n"), unit));
                    }
                    break;
                } else {
                    continue;
                }
            } else {
                /* PLL locked at desired frequency */
                break;
            }
        } else if (ref_freq != auto_ref) {
            /* Not able to lock at desired frequency */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                    "TS_PLL failed to lock on unit %d "
                    "status = 0x%08x\n"), unit, rval));
            return SOC_E_INIT;
        }
    }

    /* Bring up BS0 PLL */
    ref_freq = soc_property_suffix_num_get(unit, 0, spn_PTP_BS_FREF,
                                           &blank, auto_ref);
    index = (ref_freq == auto_ref) ? (COUNTOF(bs_pll_params) - 1) : 0;
    dco_code_target = dco_poison;

    for (; index < COUNTOF(bs_pll_params); index++) {
        if (ref_freq != auto_ref &&
                bs_pll_params[index].ref_freq != ref_freq) {
            continue;
        }

        /* Place in reset */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REG_2r, REG_PORT_ANY,
                                    TOP_BS_PLL0_RST_Lf, 0));

        /* Set N-divider value */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_BS_PLL0_CTRL_0r, REG_PORT_ANY,
                                    NDIV_INTf, bs_pll_params[index].ndiv));

        /* Select internal or external clock source */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_BS_PLL0_CTRL_2r, REG_PORT_ANY,
                            REFCLK_SOURCE_SELf,
                            bs_pll_params[index].ref_freq == 0 ? 0 : 1));

        /* Set new reference frequency info */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_BS_PLL0_CTRL_3r, REG_PORT_ANY,
                            FREFEFF_INFOf,
                            bs_pll_params[index].ref_freq_info));

        /* Take out of reset */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REG_2r, REG_PORT_ANY,
                                    TOP_BS_PLL0_RST_Lf, 1));

        /* Give time to lock */
        sal_usleep(to_usec);

        /* Check PLL lock status */
        reg = TOP_TS_PLL_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (SAL_BOOT_SIMULATION ||
                soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
            if (ref_freq == auto_ref && dco_code_target == dco_poison) {
                /* If auto freq, get DCO value for later comparison */
                SOC_IF_ERROR_RETURN(
                    _soc_trident3_pll_dco_code_get(unit, SOC_TD3_BS_PLL0,
                                                    &dco_code_target));
                _soc_td3_pll_dco_normalize(&dco_code_target,
                                            bs_pll_params[index].ref_freq,
                                            bs_pll_params[index].ndiv);

                /* Start iterating over all of the available frequencies */
                index = -1;
            } else if (ref_freq == auto_ref) {
                /* Make sure the current DCO code is close to the target */
                SOC_IF_ERROR_RETURN(
                    _soc_trident3_pll_dco_code_get(unit, SOC_TD3_BS_PLL0,
                                                    &dco_code));
                _soc_td3_pll_dco_normalize(&dco_code,
                                            bs_pll_params[index].ref_freq,
                                            bs_pll_params[index].ndiv);

                /* LOG_WARN(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                        "BS_PLL0: %d %d %d\n"),
                        bs_pll_params[index].ref_freq,
                        dco_code,
                        dco_code_target)); */

                if (dco_code >= (dco_code_target - 400) &&
                        dco_code <= (dco_code_target + 400)) {
                    if (bs_pll_params[index].ref_freq) {
                        LOG_WARN(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                "BS_PLL0 locked on unit %d using "
                                "%d.%dMHz external reference clock\n"
                                "Recommended to add \"%s_0=%u\" "
                                "config variable\n"),
                                unit,
                                bs_pll_params[index].ref_freq / 1000000,
                                bs_pll_params[index].ref_freq / 100000 % 10,
                                spn_PTP_BS_FREF,
                                bs_pll_params[index].ref_freq));
                    } else {
                        LOG_WARN(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "BS_PLL0 locked on unit %d using "
                                "internal reference clock\n"), unit));
                    }
                    break;
                } else {
                    continue;
                }
            } else {
                /* PLL locked at desired frequency */
                break;
            }
        } else if (ref_freq != auto_ref) {
            /* Not able to lock at desired frequency */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                    "BS_PLL0 failed to lock on unit %d "
                    "status = 0x%08x\n"), unit, rval));
            return SOC_E_INIT;
        }
    }

    /* Bring up BS1 PLL */
    ref_freq = soc_property_suffix_num_get(unit, 1, spn_PTP_BS_FREF,
                                           &blank, auto_ref);
    index = (ref_freq == auto_ref) ? (COUNTOF(bs_pll_params) - 1) : 0;
    dco_code_target = dco_poison;

    for (; index < COUNTOF(bs_pll_params); index++) {
        if (ref_freq != auto_ref &&
                bs_pll_params[index].ref_freq != ref_freq) {
            continue;
        }

        /* Place in reset */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REG_2r, REG_PORT_ANY,
                                    TOP_BS_PLL1_RST_Lf, 0));

        /* Set N-divider value */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_BS_PLL1_CTRL_0r, REG_PORT_ANY,
                                    NDIV_INTf, bs_pll_params[index].ndiv));

        /* Select internal or external clock source */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_BS_PLL1_CTRL_2r, REG_PORT_ANY,
                            REFCLK_SOURCE_SELf,
                            bs_pll_params[index].ref_freq == 0 ? 0 : 1));

        /* Set new reference frequency info */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_BS_PLL1_CTRL_3r, REG_PORT_ANY,
                            FREFEFF_INFOf,
                            bs_pll_params[index].ref_freq_info));

        /* Take out of reset */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REG_2r, REG_PORT_ANY,
                                    TOP_BS_PLL1_RST_Lf, 1));

        /* Give time to lock */
        sal_usleep(to_usec);

        /* Check PLL lock status */
        reg = TOP_TS_PLL_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (SAL_BOOT_SIMULATION ||
                soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
            if (ref_freq == auto_ref && dco_code_target == dco_poison) {
                /* If auto freq, get DCO value for later comparison */
                SOC_IF_ERROR_RETURN(
                    _soc_trident3_pll_dco_code_get(unit, SOC_TD3_BS_PLL1,
                                                    &dco_code_target));
                _soc_td3_pll_dco_normalize(&dco_code_target,
                                            bs_pll_params[index].ref_freq,
                                            bs_pll_params[index].ndiv);

                /* Start iterating over all of the available frequencies */
                index = -1;
            } else if (ref_freq == auto_ref) {
                /* Make sure the current DCO code is close to the target */
                SOC_IF_ERROR_RETURN(
                    _soc_trident3_pll_dco_code_get(unit, SOC_TD3_BS_PLL1,
                                                    &dco_code));
                _soc_td3_pll_dco_normalize(&dco_code,
                                            bs_pll_params[index].ref_freq,
                                            bs_pll_params[index].ndiv);

                if (dco_code >= (dco_code_target - 400) &&
                        dco_code <= (dco_code_target + 400)) {
                    if (bs_pll_params[index].ref_freq) {
                        LOG_WARN(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                "BS_PLL1 locked on unit %d using "
                                "%d.%dMHz external reference clock\n"
                                "Recommended to add \"%s_1=%u\" "
                                "config variable\n"),
                                unit,
                                bs_pll_params[index].ref_freq / 1000000,
                                bs_pll_params[index].ref_freq / 100000 % 10,
                                spn_PTP_BS_FREF,
                                bs_pll_params[index].ref_freq));
                    } else {
                        LOG_WARN(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "BS_PLL1 locked on unit %d using "
                                "internal reference clock\n"), unit));
                    }
                    break;
                } else {
                    continue;
                }
            } else {
                /* PLL locked at desired frequency */
                break;
            }
        } else if (ref_freq != auto_ref) {
            /* Not able to lock at desired frequency */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                    "BS_PLL1 failed to lock on unit %d "
                    "status = 0x%08x\n"), unit, rval));
            return SOC_E_INIT;
        }
    }

    /* De-assert TS PLL, BS PLL0/1 post reset and bring AVS out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                       TOP_TS_PLL_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                       TOP_BS_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                       TOP_BS_PLL1_POST_RST_Lf, 1);
    soc_reg_field_set(unit,TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);

    /* SW WAR - SDK-215192 abnormal small pulses from max_temp_clk output pin */
    rval = 0;
    soc_reg_field_set(unit, AVS_REG_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr, &rval,
                      SEQ_MASK_PVT_MNTRf, 0x7e);
    SOC_IF_ERROR_RETURN(WRITE_AVS_REG_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr(unit, rval));

    /* Enable high temperature interrupt monitoring.
     * Default on: pvtmon[0-7] (enable all pvtmons). */
    temp_mask = soc_property_get(unit, "temp_monitor_select",
                                 ((1 << COUNTOF(temp_thr_reg)) - 1));
    /* The above is a 8 bit mask to indicate which temp sensor to hook up to
     * the interrupt. */
    rval = 0;
    for (index = 0; index <COUNTOF(temp_thr_reg); index++) {
        uint32 trval;
        /*
         * Temperature threshold triggers both HW reset and software INTR
         * for each sensor per TD3 hardware design. Reset and INTR can be
         * enabled none, either or both per sensor based on configuration.
         *
         * TD3 PVTMON overheating protection divides thermal sensors
         * into two groups. One group is configured with a lower threshold
         * and used solely for interrupt. The other group is configured with
         * with a higher threshold and used for hardware reset.
         *
         * At runtime, when TD3 is going through overheating, the group of sensors
         * with lower threshold will raise PVT interrupt, ISR will log
         * overheating information and flood console if overheating condition
         * persists. As the temperature keeps heating up, the hardware reset will
         * eventually kicks in when temperature is over reset threshold.
         *
         * Thus TD3 PVT monitoring uses hardware reset to protect the chip and
         * software interrupt to notify user.
         *
         * Sensor group for hardware reset:
         * 0, 1, 2, 3, 4, 6 and 10.
         *
         * Sensor group for interrupt:
         * 5, 7, 8 and 9.
         *
         * AVS sensor (11) is used for voltage monitoring and should not be used for
         * any thermal protection feature (reset or interrupt), but its thermal data
         * can be used for temperature readout only.
         *
         * It hurts nothting to enable interrupt on all sensors.
         * when over-heating happens at higher threshold, both hw reset and interrupt
         * will be activated. HW reset will reset TD3, thus no interrupt will be
         * created anyway.
         */
        temp_thr = soc_property_get(unit, temp_thr_prop[index],
                ((index < 5) || (index == 6) || (index == 10)) ? \
                _SOC_TD3_TEMP_MAX : _SOC_TD3_DEF_TEMP_MAX);

        if (temp_thr > _SOC_TD3_TEMP_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                          "Unsupported temp value %d !! Max %d. Using %d.\n"),
                          temp_thr, _SOC_TD3_TEMP_MAX, _SOC_TD3_DEF_TEMP_MAX));
            temp_thr = _SOC_TD3_DEF_TEMP_MAX;
        }
        /* Convert temperature to config data */
        /* Temp = 434.10 - o_ADC_data * 0.53504
         * data = (434.10 - temp)/0.53504 = (434100-(temp*1000))/535
         * Note: Since this is a high temp value we can safely assume it to
         * always be a +ive number. This is in degrees celcius.
         */
        temp_thr = (434100-(temp_thr*1000))/535;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, temp_thr_reg[index], REG_PORT_ANY, 0, &trval));
        soc_reg_field_set(unit, temp_thr_reg[index], &trval, MIN_THRESHOLDf,
                          temp_thr);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, temp_thr_reg[index], REG_PORT_ANY, 0, trval));
        if ((temp_mask & (1 << index)) && (index != 11)) {
            /* AVS sensor 11 should not be configured. */
            rval |= (1 << ((index * 2) + 1)); /*2 bits per pvtmon, using min*/
        }
    }
    soc_td3_temp_mon_mask[unit] = temp_mask;

    /* Set PVT interrupt mask map */
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_INTR_MASKr(unit, rval));

    /* Clear interrupt latch bits */
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_INTR_STATUSr(unit, 0xffffffff));

    /*
     * Enable HW PVTMON high temperature protection by setting
     * hightemp_ctrl_en field for pvtmon 0-4, 6 and 10
     */
    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, HIGHTEMP_RESET_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON0_HIGHTEMP_CTRL_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON1_HIGHTEMP_CTRL_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON2_HIGHTEMP_CTRL_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON3_HIGHTEMP_CTRL_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON4_HIGHTEMP_CTRL_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON6_HIGHTEMP_CTRL_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON10_HIGHTEMP_CTRL_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));

    /* Enable PVT interrupt - bit 2. */
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        /* Enable PVTMON interrupt in CMIC */
        SOC_IF_ERROR_RETURN(soc_iproc_getreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                            REG_PORT_ANY, 0), &rval));
        rval |= 0x4;
        SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit,
                            soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                            REG_PORT_ANY, 0), rval));
    }
#endif

    /* Bring port blocks out of reset */
    rval = 0;
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_PM32_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, 0xffffffff));
    sal_usleep(to_usec);

    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, IDB_SER_CONTROLr,
                                                   REG_PORT_ANY,
                                                   IS_TDM_ECC_ENf,1));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, IDB_SER_CONTROLr,
                                                   REG_PORT_ANY,
                                                   CA_CPU_ECC_ENABLEf,1));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, IDB_SER_CONTROLr,
                                                   REG_PORT_ANY,
                                                   CA_LPBK_ECC_ENABLEf,1));
    }

    sal_usleep(to_usec);
    SOC_IF_ERROR_RETURN(soc_trident3_init_idb_memory(unit));
    SOC_IF_ERROR_RETURN(_soc_trident3_init_hash_control_reset(unit));
    SOC_IF_ERROR_RETURN(soc_trident3_uft_uat_config(unit));

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(soc_trident3_clear_all_memory(unit));
    }

    if (soc_feature(unit, soc_feature_turbo_boot)) {
        uint32 def_val = 0;

        if (!(SOC_CONTROL(unit)->soc_flags & SOC_F_ALL_MODULES_INITED)) {
            def_val = 1;
        }
        if (soc_property_get(unit, "clear_on_hw_resetting", def_val)) {
            SOC_HW_RESET_START(unit);
        }
    }

    return SOC_E_NONE;
}



int
soc_trident3_port_reset(int unit)
{
    int blk, port;
    uint32 rval;
    uint64 rval64;
    int sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;

    if (soc_feature(unit, soc_feature_pm_refclk_master)) {
        soc_info_t *si = &SOC_INFO(unit);
        if (!SHR_BITNULL_RANGE(si->pm_ref_master_bitmap, 0,
                                 SOC_MAX_NUM_BLKS)) {
            soc_xgxs_reset_master_tsc(unit);
        }
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);

        SOC_IF_ERROR_RETURN(READ_CLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MAC_CONTROLr(unit, port, rval));
        sal_udelay(10);
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
        if (soc_reg_field_valid(unit, CLPORT_MAC_CONTROLr, SYS_16B_INTF_MODEf) &&
            soc_feature(unit, soc_feature_clmac_16byte_interface_mode)) {
            soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval, SYS_16B_INTF_MODEf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MAC_CONTROLr(unit, port, rval));
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, XLPORT_XGXS0_CTRL_REGr,
                                        port, 0, &rval64));
        soc_reg64_field32_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval64,
                                        REFSELf, 3);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, XLPORT_XGXS0_CTRL_REGr,
                                        port, 0, rval64));

        SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
        sal_udelay(10);
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
    }

    /* Power off CLPORT blocks */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 1));
    }
    sal_usleep(sleep_usec + 10000);
    /* Power on CLPORT blocks */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 0));
    }

    /* Power off XLPORT blocks */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 1));
    }
    sal_usleep(sleep_usec + 10000);
    /* Power off XLPORT blocks */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 0));
    }
    return SOC_E_NONE;
}

int
soc_trident3_port_speed_update(int unit, soc_port_t port, int speed)
{
    soc_info_t *si;
    soc_reg_t reg;
    soc_field_t field;
    uint32 rval, fval, mode;
    uint64 rval64;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe, phy_port;
    int p, count = 1, ports[2] = {-1, -1};
#if defined(BCM_TD3_ASF_EXCLUDE)
    int ovs_class;
#else
    int rv;
#endif

    static soc_field_t egr_reset_fields[] = {
        PM0_RESETf, PM1_RESETf, PM2_RESETf, PM3_RESETf,
        PM4_RESETf, PM5_RESETf, PM6_RESETf, PM7_RESETf
    };

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
#if 0
    mmu_port = si->port_p2m_mapping[phy_port];
#endif
    pipe = si->port_pipe[port];

    /* Special handling for single lane and tri mode ports */
    if (si->port_num_lanes[port] < 4) {
        /* Get the current mode */
        SOC_IF_ERROR_RETURN(READ_CLPORT_MODE_REGr(unit, port, &rval));
        mode = soc_reg_field_get(unit, CLPORT_MODE_REGr, rval,
                                 XPORT0_CORE_PORT_MODEf);

        /* Figure out the number of ports that will be affected */
        switch (mode) {
        case SOC_TD3_PORT_MODE_QUAD:
            count = 2;
            break;
        case SOC_TD3_PORT_MODE_TRI_012:
            count = SOC_PORT_BINDEX(unit, phy_port) > 1 ? 1 : 2;
            break;
        case SOC_TD3_PORT_MODE_TRI_023:
            count = SOC_PORT_BINDEX(unit, phy_port) > 1 ? 2 : 1;
            break;
        default:
            break;
        }

        /* Setup the list of ports to be updated */
        if (SOC_PORT_BINDEX(unit, phy_port) % 2) {
            ports[1] = port;
            ports[0] = si->port_l2p_mapping[phy_port-1];
        } else {
            ports[0] = port; /* This will handle 2xdual lane mode as well */
            ports[1] = si->port_l2p_mapping[phy_port+1];
        }
    } else {
        ports[0] = port;
    }

    /* Update MMU and EP credits */
    /* De-assert EGR_ENABLE */
    sal_memset(&entry, 0, sizeof(entry));
    for (p = 0; p < count; p++) {
        if (ports[p] < 0) {
            continue;
        }
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, si->port_l2p_mapping[ports[p]], &entry));
    }

    /* Clear MMU port credit before Resetting egress */
    reg = MMU_PORT_CREDITr;
    for (p = 0; p < count; p++) {
        if (ports[p] < 0) {
            continue;
        }
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, INITIALIZEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, ports[p], 0, rval));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, ports[p], 0, 0));
    }

#if defined(BCM_TD3_ASF_EXCLUDE)
    if (IS_CL_PORT(unit, port)) {
        /* Update cut-through speed class */
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        if (soc_reg_field_get(unit, ASF_EPORT_CFGr, rval, ENABLEf)) {
            _soc_trident3_speed_to_ct_class_mapping(unit, speed, &ovs_class);

            rval = 0;
            soc_reg_field_set(unit, ASF_IPORT_CFGr, &rval, ASF_PORT_SPEEDf,
                              ovs_class);
            SOC_IF_ERROR_RETURN(WRITE_ASF_IPORT_CFGr(unit, port, rval));

            sal_memset(entry, 0, sizeof(egr_ip_cut_thru_class_entry_t));
            soc_mem_field32_set(unit, EGR_IP_CUT_THRU_CLASSm, entry,
                                CUT_THRU_CLASSf, ovs_class);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, EGR_IP_CUT_THRU_CLASSm, MEM_BLOCK_ALL,
                               port, entry));
        }
    }
#endif

    /* Updtae HSP port multicast T2OQ setting */
#if 0
    if (SOC_PBMP_MEMBER(si->eq_pbm, port)) {
        int inst;
        static soc_field_t t2oq_fields[] = {
            IS_MC_T2OQ_PORT0f, IS_MC_T2OQ_PORT1f
        };
        reg = MMU_SCFG_TOQ_MC_CFG1r;
        inst = (pipe >> 1) | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
        field = t2oq_fields[pipe & 0x01];
        fval = soc_reg_field_get(unit, reg, rval, field);
        if (speed >= 40000) {
            fval |= 1 << (mmu_port & 0x0f);
        } else {
            fval &= ~(1 << (mmu_port & 0x0f));
        }
        soc_reg_field_set(unit, reg, &rval, field, fval);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }
#endif

#if !defined(BCM_TD3_ASF_EXCLUDE)
    if (soc_feature(unit, soc_feature_asf_multimode)) {
        rv = soc_td3_port_asf_mode_set(unit, port,
                 speed, _SOC_TD3_ASF_MODE_CFG_UPDATE);
        if ((SOC_E_NONE != rv) && (SOC_E_UNAVAIL != rv)) {
            return rv;
        }
    }
#endif

    /* Reset egress hardware resource */
    reg = SOC_REG_UNIQUE_ACC(unit, EGR_PORT_BUFFER_SFT_RESET_0r)[pipe];
    field = egr_reset_fields[si->port_serdes[port] % 8];
    if (4 == si->port_num_lanes[port]) {
        /* reset all 4 lanes */
        fval = 4;
    } else if (SOC_PORT_BINDEX(unit, phy_port) < 2) { /* reset lanes 0 and 1 */
        fval = 1;
    } else { /* reset lanes 2 and 3 */
        fval = 2;
    }
    SOC_IF_ERROR_RETURN
        (soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, field, fval);
    SOC_IF_ERROR_RETURN(soc_reg64_set(unit, reg, REG_PORT_ANY, 0, rval64));
    soc_reg64_field32_set(unit, reg, &rval64, field, 0);
    SOC_IF_ERROR_RETURN(soc_reg64_set(unit, reg, REG_PORT_ANY, 0, rval64));

    /* Enable egress */
    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
    for (p = 0; p < count; p++) {
        if (ports[p] < 0) {
            continue;
        }
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, si->port_l2p_mapping[ports[p]], entry));
    }
    return SOC_E_NONE;
}

int
soc_trident3_xpe_base_index_check(int unit, int base_type, int xpe,
                                  int base_index, char *msg)
{
    soc_info_t *si;
    int pipe;
    uint32 map;
    char *base_name;

    si = &SOC_INFO(unit);

    if (xpe == -1 || base_index == -1) {
        return SOC_E_NONE;
    }

    if (xpe < NUM_XPE(unit) && si->xpe_ipipe_map[xpe] == 0) {
        if (msg) {
            LOG_CLI((BSL_META_U(unit,
                                "%s: XPE%d is not in config\n"),
                     msg, xpe));
        }
        return SOC_E_PARAM;
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        base_name = base_type == 0 ? "IPORT" : "EPORT";
        pipe = si->port_pipe[base_index];
        if (pipe == -1) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (xpe < NUM_XPE(unit)) { /* Unique access type */
            map = base_type == 0 ?
                si->ipipe_xpe_map[pipe] : si->epipe_xpe_map[pipe];
            if (map & (1 << xpe)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in XPE%d\n"),
                         msg, base_name, base_index, xpe));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (base_type == 2) {
            base_name = "IPIPE";
            map = si->ipipe_xpe_map[base_index];
        } else {
            base_name = "EPIPE";
            map = si->epipe_xpe_map[base_index];
        }
        if (map == 0) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (xpe < NUM_XPE(unit)) { /* Unique access type */
            if (map & (1 << xpe)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in XPE%d\n"),
                         msg, base_name, base_index, xpe));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 4: /* CHIP */
        break;
    case 5: /* XPE */
        if (si->xpe_ipipe_map[base_index] != 0) {
            break;
        }
        if (msg != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "%s: XPE%d is not in config\n"),
                     msg, base_index));
        }
        return SOC_E_PARAM;
    case 6: /* SLICE */
        if (si->sc_ipipe_map[base_index] == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: SLICE%d is not in config\n"),
                         msg, base_index));
        } else if (xpe < NUM_XPE(unit)) { /* Unique access type */
            if (xpe & 1) { /* XPE 1 and 3 are in slice 1 */
                if (base_index == 1) {
                    break;
                }
            } else { /* XPE 0 and 2 are in slice 0 */
                if (base_index == 0) {
                    break;
                }
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: XPE%d is not in SLICE%d\n"),
                         msg, xpe, base_index));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 7: /* LAYER */
        if (xpe & 2) { /* XPE 2 and 3 are in layer 1 */
            if (base_index == 1) {
                break;
            }
        } else { /* XPE 0 and 1 are in layer 0 */
            if (base_index == 0) {
                break;
            }
        }
        if (msg) {
            LOG_CLI((BSL_META_U(unit,
                                "%s: XPE%d is not in LAYER%d\n"),
                     msg, xpe, base_index));
        }
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

int soc_trident3_get_alpm_banks(int unit)
{
    return num_shared_alpm_banks[unit];
}

int soc_trident3_set_alpm_banks(int unit, int banks)
{
    num_shared_alpm_banks[unit] = banks;
    return 0;
}

int soc_trident3_alpm_mode_get(int unit)
{
    return _soc_alpm_mode[unit];
}

int soc_trident3_mem_basetype_get(int unit, soc_mem_t mem)
{
    int base_type;

    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;
    return base_type;
}

int soc_trident3_mem_is_xpe(int unit, soc_mem_t mem)
{
    int block_info_index;

    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);

    if (SOC_BLOCK_TYPE(unit, block_info_index) == SOC_BLK_MMU_XPE) {
        return 1;
    } else {
        return 0;
    }
}

/* Function to check if given XPE/PIPE combo matches for the given mem's base
 * type definition.
 */
int
soc_trident3_mem_xpe_pipe_check(int unit, soc_mem_t mem, int xpe, int pipe)
{
    int rv = SOC_E_NONE;
    int block_info_index, base_type;

    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
    if (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_XPE) {
        return SOC_E_PARAM;
    }

    base_type = soc_trident3_mem_basetype_get(unit, mem);
    rv = soc_trident3_xpe_base_index_check(unit, base_type, xpe, pipe, NULL);

    if (rv == SOC_E_PARAM) {
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}

STATIC int
_soc_trident3_xpe_reg_check(int unit, soc_reg_t reg, int xpe, int base_index)
{
    int acc_type, base_type;

    if (!SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg).block, SOC_BLK_MMU_XPE)) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not XPE register\n"), SOC_REG_NAME(unit, reg)));
        return SOC_E_PARAM;
    }

    if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL) { /* UNIQUE base register */
        if (xpe == -1 || xpe >= NUM_XPE(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad XPE value %d\n"),
                     SOC_REG_NAME(unit, reg), xpe));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_REG_ACC_TYPE(unit, reg);
        if (acc_type < NUM_XPE(unit)) { /* UNIQUE per XPE register */
            if (xpe != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Ovveride XPE value %d with ACC_TYPE of %s\n"),
                         xpe, SOC_REG_NAME(unit, reg)));
            }
            xpe = acc_type;
        } else { /* non-UNIQUE register */
            return SOC_E_NONE;
        }
    }
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;

    return soc_trident3_xpe_base_index_check(unit, base_type, xpe, base_index,
                                             SOC_REG_NAME(unit, reg));
}

#if 0
STATIC int
_soc_trident3_xpe_mem_check(int unit, soc_mem_t mem, int xpe, int base_index)
{
    int block_info_index, acc_type, base_type;

    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
    if (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_XPE) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not XPE register\n"), SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }

    if (SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) { /* UNIQUE base memory */
        if (xpe == -1 || xpe >= NUM_XPE(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad XPE value %d\n"),
                     SOC_MEM_NAME(unit, mem), xpe));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_MEM_ACC_TYPE(unit, mem);
        if (acc_type < NUM_XPE(unit)) { /* UNIQUE per XPE memory */
            if (xpe != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Ovveride XPE value %d with ACC_TYPE of %s\n"),
                         xpe, SOC_MEM_NAME(unit, mem)));
                xpe = acc_type;
            }
        } else { /* non-UNIQUE memory */
            return SOC_E_NONE;
        }
    }
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;

    return soc_trident3_xpe_base_index_check(unit, base_type, xpe, base_index,
                                             SOC_MEM_NAME(unit, mem));
}
#endif

/*
 * Parameters for soc_trident3_xpe_reg_access and soc_trident3_xpe_mem_access:
 *
 *    base unique  reg/  xpe   write action          read action
 *   index  type   mem
 *    ==== ====== ===== ===   ================      =================
 * #1   -1  no     -     -    all XPEs/pipes        first pipe in the first XPE
 * #2   -1  yes   base   -1   (same as #1)
 * #3   -1  yes   base  0-3   all pipes in the XPE  first pipe in the XPE
 * #4   -1  yes  xpe0-3  -    (same as #3)
 * #5  0-3  no     -     -    applicable XPEs       first XPE has the pipe
 * #6  0-3  yes   base   -1   (same as #5)
 * #7  0-3  yes   base  0-3   the XPE/pipe          the XPE/pipe
 * #8  0-3  yes  xpe0-3  -    (same as #7)
 *
 * - S/W will loop through base types (i.e. pipes) if base_index==-1
 * - S/W will loop through applicable XPEs if xpe==-1 on unique access type
 *   base view (H/W will do the loop for other access types)
 * - base_index will be ignored if the table does not have multiple sections
 *   for example ACC_TYPE==UNIQUE BASE_TYPE==XPE
 */
STATIC int
_soc_trident3_xpe_reg_access(int unit, soc_reg_t reg, int xpe, int base_index,
                             int index, uint64 *data, int write)
{
    soc_info_t *si;
    soc_reg_t orig_reg;
    int port;
    int base_type, break_after_first;
    int base_index_count;
    uint32 base_index_map, xpe_map=0;
    soc_pbmp_t base_index_pbmp;
    uint32 inst;

    si = &SOC_INFO(unit);
    orig_reg = reg;
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;
    break_after_first = TRUE;

    if (xpe >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_trident3_xpe_reg_check(unit, reg, xpe, base_index));
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        if (xpe >= 0 && SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
            reg = SOC_REG_UNIQUE_ACC(unit, reg)[xpe];
        }

        if (base_index == -1) {
            SOC_PBMP_ASSIGN(base_index_pbmp, PBMP_ALL(unit));
        } else {
            /* This argument is logical port, same as soc_reg_get/set */
            SOC_PBMP_PORT_SET(base_index_pbmp, base_index);
        }

        SOC_PBMP_ITER(base_index_pbmp, port) {
            /* Loop through XPE(s) only on UNIQUE type base register */
            if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                if (base_index < -1) {
                    return SOC_E_PARAM;
                } else if (base_index == -1) {
                    xpe_map = 0xf;
                } else {
                    xpe_map = base_type == 0 ?
                        si->ipipe_xpe_map[base_index] :
                        si->epipe_xpe_map[base_index];
                }
                if (write) {
                    break_after_first = FALSE;
                }
            }
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                    if (!(xpe_map & (1 << xpe))) {
                        continue;
                    }
                    reg = SOC_REG_UNIQUE_ACC(unit, orig_reg)[xpe];
                }
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_set(unit, reg, port, index, *data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, reg, port, index, data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (xpe >= 0 && SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
            reg = SOC_REG_UNIQUE_ACC(unit, reg)[xpe];
        }

        soc_trident3_pipe_map_get(unit, &base_index_map);
        if (base_index != -1) {
            base_index_map &= 1 << base_index;
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        for (base_index = 0; base_index < NUM_PIPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            /* Loop through XPE(s) only on UNIQUE type base register */
            if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                xpe_map = base_type == 2 ?
                    si->ipipe_xpe_map[base_index] :
                    si->epipe_xpe_map[base_index];
                if (write) {
                    break_after_first = FALSE;
                }
            }
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                    if (!(xpe_map & (1 << xpe))) {
                        continue;
                    }
                    reg = SOC_REG_UNIQUE_ACC(unit, orig_reg)[xpe];
                }
                inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_set(unit, reg, inst, index, *data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, reg, inst, index, data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 4: /* CHIP */
        if (write) {
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, 0, index, *data));
        } else {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, 0, index, data));
        }
        break;
    case 5: /* XPE */
        if (xpe >= 0 && SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
            reg = SOC_REG_UNIQUE_ACC(unit, reg)[xpe];
        }

        if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL ||
            SOC_REG_ACC_TYPE(unit, reg) < NUM_XPE(unit)) {
            base_index_map = 1;
        } else if (base_index == -1) {
            base_index_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        } else {
            base_index_map = 1 << base_index;
        }

        /* Loop through XPE(s) only on UNIQUE type base register */
        if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
            xpe_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
            break_after_first = FALSE;
        }

        for (base_index = 0; base_index < NUM_XPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                    if (!(xpe_map & (1 << xpe))) {
                        continue;
                    }
                    reg = SOC_REG_UNIQUE_ACC(unit, orig_reg)[xpe];
                }
                inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_set(unit, reg, inst, index, *data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, reg, inst, index, data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 6: /* SLICE */
    case 7: /* LAYER */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = base_type == 6 ? si->ipipe_sc_map[0] : 0x3;
        } else {
            base_index_map = 1 << base_index;
        }

        base_index_count = base_type == 6 ? NUM_SLICE(unit) : NUM_LAYER(unit);
        for (base_index = 0; base_index < base_index_count; base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    /* No default case as base_type can have value only between 0-7 */
    }

    return SOC_E_NONE;
}
#if 0
STATIC int
_soc_trident3_xpe_mem_access(int unit, soc_mem_t mem, int xpe, int base_index,
                             int copyno, int offset_in_section,
                             void *entry_data, int write)
{
    soc_info_t *si;
    soc_mem_t orig_mem;
    int index, section_size;
    int base_type, break_after_first;
    uint32 base_index_map, xpe_map=0;

    si = &SOC_INFO(unit);
    orig_mem = mem;
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;
    section_size = SOC_MEM_INFO(unit, mem).section_size;
    break_after_first = TRUE;

    if (xpe >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_trident3_xpe_mem_check(unit, mem, xpe, base_index));
    }

    switch (base_type) {
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (xpe >= 0 && SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) {
            mem = SOC_MEM_UNIQUE_ACC(unit, mem)[xpe];
        }

        /* All tables with such base_type should have multiple sections */
        soc_trident3_pipe_map_get(unit, &base_index_map);
        if (base_index != -1) {
            base_index_map &= 1 << base_index;
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        for (base_index = 0; base_index < NUM_PIPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            /* Loop through XPE(s) only on UNIQUE type base memory */
            if (SOC_MEM_UNIQUE_ACC(unit, orig_mem) != NULL) {
                xpe_map = base_type == 2 ?
                    si->ipipe_xpe_map[base_index] :
                    si->epipe_xpe_map[base_index];
                if (write) {
                    break_after_first = FALSE;
                }
            }
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                if (SOC_MEM_UNIQUE_ACC(unit, orig_mem) != NULL) {
                    if (!(xpe_map & (1 << xpe))) {
                        continue;
                    }
                    mem = SOC_MEM_UNIQUE_ACC(unit, orig_mem)[xpe];
                }
                index = base_index * section_size + offset_in_section;
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, mem, copyno, index, entry_data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, mem, copyno, index, entry_data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 4: /* CHIP */
        index = offset_in_section;
        if (write) {
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, copyno, index, entry_data));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, copyno, index, entry_data));
        }
        break;
    case 5: /* XPE */
        if (xpe >= 0 && SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) {
            mem = SOC_MEM_UNIQUE_ACC(unit, mem)[xpe];
        }

        if (SOC_MEM_UNIQUE_ACC(unit, mem) != NULL ||
            SOC_MEM_ACC_TYPE(unit, mem) < NUM_XPE(unit)) {
            /* Single section table handling for unique acc_type */
            base_index_map = 1;
        } else if (base_index == -1) {
            base_index_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        } else {
            base_index_map = 1 << base_index;
        }

        /* Loop through XPE(s) only on UNIQUE type base memory */
        if (SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) {
            xpe_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
            break_after_first = FALSE;
        }

        for (base_index = 0; base_index < NUM_XPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                if (SOC_MEM_UNIQUE_ACC(unit, orig_mem) != NULL) {
                    if (!(xpe_map & (1 << xpe))) {
                        continue;
                    }
                    mem = SOC_MEM_UNIQUE_ACC(unit, orig_mem)[xpe];
                }
                index = base_index * section_size + offset_in_section;
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, mem, copyno, index, entry_data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, mem, copyno, index, entry_data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 6: /* SLICE */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = si->ipipe_sc_map[0];
        } else {
            base_index_map = 1 << base_index;
        }

        for (base_index = 0; base_index < NUM_SLICE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            index = base_index * section_size + offset_in_section;
            if (write) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, copyno, index, entry_data));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, copyno, index, entry_data));
            }
        }
        break;
    case 0: /* IPORT */
    case 1: /* EPORT */
    case 7: /* LAYER */
    default:
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}
#endif
int
soc_trident3_sc_base_index_check(int unit, int base_type, int sc,
                                 int base_index, char *msg)
{
    soc_info_t *si;
    int pipe;
    uint32 map;
    char *base_name;

    si = &SOC_INFO(unit);

    if (sc == -1 || base_index == -1) {
        return SOC_E_NONE;
    }

    if (sc < NUM_SLICE(unit) && si->sc_ipipe_map[sc] == 0) {
        if (msg) {
            LOG_CLI((BSL_META_U(unit,
                                "%s: SC%d is not in config\n"),
                     msg, sc));
        }
        return SOC_E_PARAM;
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        base_name = base_type == 0 ? "IPORT" : "EPORT";
        pipe = si->port_pipe[base_index];
        if (pipe == -1) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (sc < NUM_SLICE(unit)) { /* Unique access type */
            map = base_type == 0 ?
                si->ipipe_sc_map[pipe] : si->epipe_sc_map[pipe];
            if (map & (1 << sc)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in SC%d\n"),
                         msg, base_name, base_index, sc));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (base_type == 2) {
            base_name = "IPIPE";
            map = si->ipipe_sc_map[base_index];
        } else {
            base_name = "EPIPE";
            map = si->epipe_sc_map[base_index];
        }
        if (map == 0) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (sc < NUM_SLICE(unit)) { /* Unique access type */
            if (map & (1 << sc)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in SC%d\n"),
                         msg, base_name, base_index, sc));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 4: /* CHIP */
        break;
    case 5: /* XPE */
        if (si->xpe_ipipe_map[base_index] == 0) {
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: XPE%d is not in config\n"),
                         msg, base_index));
            }
        } else if (sc < NUM_SLICE(unit)) { /* Unique access type */
            if (sc == 0) { /* XPE 0 and 2 are in slice 0 */
                if (base_index == 0 || base_index == 2) {
                    break;
                }
            } else { /* XPE 1 and 3 are in slice 1 */
                if (base_index == 1 || base_index == 3) {
                    break;
                }
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: XPE%d is not in SLICE%d\n"),
                         msg, base_index, sc));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 6: /* SLICE */
    case 7: /* LAYER, not used */
        break;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_trident3_sc_reg_check(int unit, soc_reg_t reg, int sc, int base_index)
{
    int acc_type, base_type;

    if (!SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg).block, SOC_BLK_MMU_SC)) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not SC register\n"), SOC_REG_NAME(unit, reg)));
        return SOC_E_PARAM;
    }

    if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL) { /* UNIQUE base register */
        if (sc == -1 || sc >= NUM_SLICE(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad SC value %d\n"),
                     SOC_REG_NAME(unit, reg), sc));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_REG_ACC_TYPE(unit, reg);
        if (acc_type < NUM_SLICE(unit)) { /* UNIQUE per SC register */
            if (sc != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Ovveride SC value %d with ACC_TYPE of %s\n"),
                         sc, SOC_REG_NAME(unit, reg)));
            }
            sc = acc_type;
        } else { /* non-UNIQUE register */
            return SOC_E_NONE;
        }
    }
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;

    return soc_trident3_sc_base_index_check(unit, base_type, sc, base_index,
                                            SOC_REG_NAME(unit, reg));
}

#if 0
STATIC int
_soc_trident3_sc_mem_check(int unit, soc_mem_t mem, int sc, int base_index)
{
    int block_info_index, acc_type, base_type;

    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
    if (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_SC) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not SC register\n"), SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }

    if (SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) { /* UNIQUE base memory */
        if (sc == -1 || sc >= NUM_SLICE(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad SC value %d\n"),
                     SOC_MEM_NAME(unit, mem), sc));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_MEM_ACC_TYPE(unit, mem);
        if (acc_type < NUM_SLICE(unit)) { /* UNIQUE per SC memory */
            if (sc != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Ovveride SC value %d with ACC_TYPE of %s\n"),
                         sc, SOC_MEM_NAME(unit, mem)));
                sc = acc_type;
            }
        } else { /* non-UNIQUE memory */
            return SOC_E_NONE;
        }
    }
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;

    return soc_trident3_sc_base_index_check(unit, base_type, sc, base_index,
                                            SOC_MEM_NAME(unit, mem));
}
#endif

STATIC int
_soc_trident3_sc_reg_access(int unit, soc_reg_t reg, int sc, int base_index,
                            int index, uint64 *data, int write)
{
    soc_info_t *si;
    int port;
    int base_type;
    uint32 base_index_map;
    soc_pbmp_t base_index_pbmp;
    uint32 inst;

    si = &SOC_INFO(unit);
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;

    if (sc >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_trident3_sc_reg_check(unit, reg, sc, base_index));
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            SOC_PBMP_ASSIGN(base_index_pbmp, PBMP_ALL(unit));
        } else {
            /* This argument is logical port, same as soc_reg_get/set */
            SOC_PBMP_PORT_SET(base_index_pbmp, base_index);
        }

        SOC_PBMP_ITER(base_index_pbmp, port) {
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, index, data));
            }
        }
        break;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        /* No unique access type for such base_type */
        soc_trident3_pipe_map_get(unit, &base_index_map);
        if (base_index != -1) {
            base_index_map &= 1 << base_index;
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        for (base_index = 0; base_index < NUM_PIPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    case 4: /* CHIP */
        if (write) {
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, 0, index, *data));
        } else {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, 0, index, data));
        }
        break;
    case 5: /* XPE */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        } else {
            base_index_map = 1 << base_index;
        }

        for (base_index = 0; base_index < NUM_SLICE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    case 6: /* SLICE */
        if (sc >= 0 && SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
            reg = SOC_REG_UNIQUE_ACC(unit, reg)[sc];
        }
        if (base_index == -1) {
            base_index_map = si->ipipe_sc_map[0];;
        } else {
            base_index_map = 1 << base_index;
        }
        for (base_index = 0; base_index < NUM_SLICE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    case 7: /* LAYER */
    default:
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}
#if 0
STATIC int
_soc_trident3_sc_mem_access(int unit, soc_mem_t mem, int sc, int base_index,
                            int copyno, int offset_in_section, void *entry_data,
                            int write)
{
    soc_info_t *si;
    soc_mem_t orig_mem;
    int index, section_size;
    int base_type, break_after_first;
    uint32 base_index_map, sc_map=0;

    si = &SOC_INFO(unit);
    orig_mem = mem;
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;
    section_size = SOC_MEM_INFO(unit, mem).section_size;
    break_after_first = TRUE;

    if (sc >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_trident3_sc_mem_check(unit, mem, sc, base_index));
    }

    switch (base_type) {
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (sc >= 0 && SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) {
            mem = SOC_MEM_UNIQUE_ACC(unit, mem)[sc];
        }

        /* All tables with such base_type should have multiple sections */
        soc_trident3_pipe_map_get(unit, &base_index_map);
        if (base_index != -1) {
            base_index_map &= 1 << base_index;
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        for (base_index = 0; base_index < NUM_PIPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            /* Loop through SC(s) only on UNIQUE type base memory */
            if (SOC_MEM_UNIQUE_ACC(unit, orig_mem) != NULL) {
                sc_map = base_type == 2 ?
                    si->ipipe_sc_map[base_index] :
                    si->epipe_sc_map[base_index];
                if (write) {
                    break_after_first = FALSE;
                }
            }
            for (sc = 0; sc < NUM_SLICE(unit); sc++) {
                if (SOC_MEM_UNIQUE_ACC(unit, orig_mem) != NULL) {
                    if (!(sc_map & (1 << sc))) {
                        continue;
                    }
                    mem = SOC_MEM_UNIQUE_ACC(unit, orig_mem)[sc];
                }
                index = base_index * section_size + offset_in_section;
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, mem, copyno, index, entry_data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, mem, copyno, index, entry_data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 5: /* XPE */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        } else {
            base_index_map = 1 << base_index;
        }

        for (base_index = 0; base_index < NUM_SLICE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            index = base_index * section_size + offset_in_section;
            if (write) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, copyno, index, entry_data));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, copyno, index, entry_data));
            }
        }
        break;
    case 6: /* SLICE */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = si->ipipe_sc_map[0];
        } else {
            base_index_map = 1 << base_index;
        }
        for (base_index = 0; base_index < NUM_SLICE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            index = base_index * section_size + offset_in_section;
            if (write) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, copyno, index, entry_data));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, copyno, index, entry_data));
            }
        }
        break;
    case 0: /* IPORT */
    case 1: /* EPORT */
    case 4: /* CHIP */
    case 7: /* LAYER */
    default:
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}
#endif

int
soc_trident3_sed_base_index_check(int unit, int base_type, int sed,
                                 int base_index, char *msg)
{
    soc_info_t *si;
    int pipe;
    uint32 map;
    char *base_name;

    si = &SOC_INFO(unit);

    if (sed == -1 || base_index == -1) {
        return SOC_E_NONE;
    }

    if (sed < NUM_SED(unit) && si->sed_ipipe_map[sed] == 0) {
        if (msg) {
            LOG_CLI((BSL_META_U(unit,
                                "%s: SED%d is not in config\n"),
                     msg, sed));
        }
        return SOC_E_PARAM;
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        base_name = base_type == 0 ? "IPORT" : "EPORT";
        pipe = si->port_pipe[base_index];
        if (pipe == -1) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (sed < NUM_SED(unit)) { /* Unique access type */
            map = base_type == 0 ?
                si->ipipe_sed_map[pipe] : si->epipe_sed_map[pipe];
            if (map & (1 << sed)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in SED%d\n"),
                         msg, base_name, base_index, sed));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (base_type == 2) {
            base_name = "IPIPE";
            map = si->ipipe_sed_map[base_index];
        } else {
            base_name = "EPIPE";
            map = si->epipe_sed_map[base_index];
        }
        if (map == 0) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (sed < NUM_SED(unit)) { /* Unique access type */
            if (map & (1 << sed)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in SED%d\n"),
                         msg, base_name, base_index, sed));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 4: /* CHIP */
        break;
    case 5: /* XPE */
        if (si->xpe_ipipe_map[base_index] == 0) {
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: XPE%d is not in config\n"),
                         msg, base_index));
            }
        } else if (sed < NUM_SED(unit)) { /* Unique access type */
            if (sed == 0) { /* XPE 0 and 2 are in slice 0 */
                if (base_index == 0 || base_index == 2) {
                    break;
                }
            } else { /* XPE 1 and 3 are in slice 1 */
                if (base_index == 1 || base_index == 3) {
                    break;
                }
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: XPE%d is not in SLICE%d\n"),
                         msg, base_index, sed));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 6: /* SLICE */
    case 7: /* LAYER, not used */
        break;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_trident3_sed_reg_check(int unit, soc_reg_t reg, int sed, int base_index)
{
    int acc_type, base_type;

    if (!SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg).block, SOC_BLK_MMU_SED)) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not SED register\n"), SOC_REG_NAME(unit, reg)));
        return SOC_E_PARAM;
    }

    if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL) { /* UNIQUE base register */
        if (sed == -1 || sed >= NUM_SED(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad SED value %d\n"),
                     SOC_REG_NAME(unit, reg), sed));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_REG_ACC_TYPE(unit, reg);
        if (acc_type < NUM_SED(unit)) { /* UNIQUE per SED register */
            if (sed != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Override SED value %d with ACC_TYPE of %s\n"),
                         sed, SOC_REG_NAME(unit, reg)));
            }
            sed = acc_type;
        } else { /* non-UNIQUE register */
            return SOC_E_NONE;
        }
    }
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;

    return soc_trident3_sed_base_index_check(unit, base_type, sed, base_index,
                                            SOC_REG_NAME(unit, reg));
}
STATIC int
_soc_trident3_sed_reg_access(int unit, soc_reg_t reg, int sed, int base_index,
                             int index, uint64 *data, int write)
{
    soc_info_t *si;
    soc_reg_t orig_reg;
    int port;
    int base_type, break_after_first;
    int base_index_count;
    uint32 base_index_map;
    soc_pbmp_t base_index_pbmp;
    uint32 inst;

    si = &SOC_INFO(unit);
    orig_reg = reg;
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;
    break_after_first = TRUE;

    if (sed >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_trident3_sed_reg_check(unit, reg, sed, base_index));
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        if (base_index == -1) {
            SOC_PBMP_ASSIGN(base_index_pbmp, PBMP_ALL(unit));
        } else {
            /* This argument is logical port, same as soc_reg_get/set */
            SOC_PBMP_PORT_SET(base_index_pbmp, base_index);
        }

        SOC_PBMP_ITER(base_index_pbmp, port) {
            if (write) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, port, index, *data));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, port, index, data));
            }
        }
        break;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (sed >= 0 && SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
            reg = SOC_REG_UNIQUE_ACC(unit, reg)[sed];
        }

        soc_trident3_pipe_map_get(unit, &base_index_map);
        if (base_index != -1) {
            base_index_map &= 1 << base_index;
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        for (base_index = 0; base_index < NUM_PIPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                reg = SOC_REG_UNIQUE_ACC(unit, orig_reg)[sed];
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, inst, index, data));
            }
            if (break_after_first) {
                break;
            }
        }
        break;
    case 4: /* CHIP */
        if (write) {
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, 0, index, *data));
        } else {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, 0, index, data));
        }
        break;
    case 5: /* XPE */
        if (base_index == -1) {
            base_index_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        } else {
            base_index_map = 1 << base_index;
        }
        for (base_index = 0; base_index < NUM_XPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    case 6: /* SLICE */
    case 7: /* LAYER */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = base_type == 6 ? si->ipipe_sc_map[0] : 0x3;
        } else {
            base_index_map = 1 << base_index;
        }

        base_index_count = base_type == 6 ? NUM_SLICE(unit) : NUM_LAYER(unit);
        for (base_index = 0; base_index < base_index_count; base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    /* No default case as base_type can have value only between 0-7 */
    }

    return SOC_E_NONE;
}


int
soc_trident3_xpe_reg32_set(int unit, soc_reg_t reg, int xpe, int base_index,
                           int index, uint32 data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, data);
    SOC_IF_ERROR_RETURN(_soc_trident3_xpe_reg_access(unit, reg, xpe, base_index,
                                                     index, &data64, TRUE));

    return SOC_E_NONE;
}

int
soc_trident3_xpe_reg32_get(int unit, soc_reg_t reg, int xpe, int base_index,
                           int index, uint32 *data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, *data);
    SOC_IF_ERROR_RETURN(_soc_trident3_xpe_reg_access(unit, reg, xpe, base_index,
                                                     index, &data64, FALSE));
    *data = COMPILER_64_LO(data64);

    return SOC_E_NONE;
}

int
soc_trident3_xpe_reg_set(int unit, soc_reg_t reg, int xpe, int base_index,
                         int index, uint64 data)
{
    return _soc_trident3_xpe_reg_access(unit, reg, xpe, base_index,
                                        index, &data, TRUE);
}

int
soc_trident3_xpe_reg_get(int unit, soc_reg_t reg, int xpe, int base_index,
                         int index, uint64 *data)
{
    return _soc_trident3_xpe_reg_access(unit, reg, xpe, base_index,
                                        index, data, FALSE);
}
#if 0
int
soc_trident3_xpe_mem_write(int unit, soc_mem_t mem, int xpe, int base_index,
                           int copyno, int offset_in_section, void *entry_data)
{
    return _soc_trident3_xpe_mem_access(unit, mem, xpe, base_index, copyno,
                                        offset_in_section, entry_data, TRUE);
}

int
soc_trident3_xpe_mem_read(int unit, soc_mem_t mem, int xpe, int base_index,
                          int copyno, int offset_in_section, void *entry_data)
{
    return _soc_trident3_xpe_mem_access(unit, mem, xpe, base_index, copyno,
                                        offset_in_section, entry_data, FALSE);
}
#endif
int
soc_trident3_sc_reg32_set(int unit, soc_reg_t reg, int sc, int base_index,
                          int index, uint32 data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, data);
    SOC_IF_ERROR_RETURN(_soc_trident3_sc_reg_access(unit, reg, sc, base_index,
                                                    index, &data64, TRUE));

    return SOC_E_NONE;
}

int
soc_trident3_sc_reg32_get(int unit, soc_reg_t reg, int sc, int base_index,
                          int index, uint32 *data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, *data);
    SOC_IF_ERROR_RETURN(_soc_trident3_sc_reg_access(unit, reg, sc, base_index,
                                                    index, &data64, FALSE));
    *data = COMPILER_64_LO(data64);

    return SOC_E_NONE;
}

int
soc_trident3_sc_reg_set(int unit, soc_reg_t reg, int sc, int base_index,
                        int index, uint64 data)
{
    return _soc_trident3_sc_reg_access(unit, reg, sc, base_index,
                                       index, &data, TRUE);
}

int
soc_trident3_sc_reg_get(int unit, soc_reg_t reg, int sc, int base_index,
                        int index, uint64 *data)
{
    return _soc_trident3_sc_reg_access(unit, reg, sc, base_index,
                                       index, data, FALSE);
}
#if 0
int
soc_trident3_sc_mem_write(int unit, soc_mem_t mem, int sc, int base_index,
                          int copyno, int offset_in_section, void *entry_data)
{
    return _soc_trident3_sc_mem_access(unit, mem, sc, base_index, copyno,
                                       offset_in_section, entry_data, TRUE);
}

int
soc_trident3_sc_mem_read(int unit, soc_mem_t mem, int sc, int base_index,
                         int copyno, int offset_in_section, void *entry_data)
{
    return _soc_trident3_sc_mem_access(unit, mem, sc, base_index, copyno,
                                       offset_in_section, entry_data, FALSE);
}
#endif

int
soc_trident3_sed_reg32_set(int unit, soc_reg_t reg, int sed, int base_index,
                          int index, uint32 data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, data);
    SOC_IF_ERROR_RETURN(_soc_trident3_sed_reg_access(unit, reg, sed, base_index,
                                                    index, &data64, TRUE));

    return SOC_E_NONE;
}

int
soc_trident3_sed_reg32_get(int unit, soc_reg_t reg, int sed, int base_index,
                          int index, uint32 *data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, *data);
    SOC_IF_ERROR_RETURN(_soc_trident3_sed_reg_access(unit, reg, sed, base_index,
                                                    index, &data64, FALSE));
    *data = COMPILER_64_LO(data64);

    return SOC_E_NONE;
}

int
soc_trident3_sed_reg_set(int unit, soc_reg_t reg, int sed, int base_index,
                        int index, uint64 data)
{
    return _soc_trident3_sed_reg_access(unit, reg, sed, base_index,
                                       index, &data, TRUE);
}

int
soc_trident3_sed_reg_get(int unit, soc_reg_t reg, int sed, int base_index,
                        int index, uint64 *data)
{
    return _soc_trident3_sed_reg_access(unit, reg, sed, base_index,
                                       index, data, FALSE);
}


#if defined(BCM_TD3_ASF_EXCLUDE)
int
soc_trident3_cut_through_update(int unit, soc_port_t port, int enable)
{
    soc_info_t *si;
    _soc_trident3_tdm_temp_t *tdm;
    int pipe, ovs_class, index;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int *start_count;
    static int start_count_saf[13] =
        { 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18 };
    static int start_count_linerate[13] =
        { 18, 27, 27, 21, 21, 18, 18, 12, 12, 12, 12, 15, 15 };
    static int start_count_oversub_2_1[13] =
        { 18, 57, 57, 51, 51, 45, 45, 39, 39, 33, 33, 33, 33 };
    static int start_count_oversub_3_2[13] =
        { 18, 45, 45, 39, 39, 36, 36, 27, 27, 24, 24, 24, 24 };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    if (enable) {
        if (SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
            pipe = si->port_pipe[port];
            if (tdm->ovs_ratio_x1000[pipe][0] + tdm->ovs_ratio_x1000[pipe][1] > 1500) {
                /* More than 3:2 oversubscription ratio */
                start_count = start_count_oversub_2_1;
            } else {
                start_count = start_count_oversub_3_2;
            }
        } else {
            start_count = start_count_linerate;
        }
    } else {
        start_count = start_count_saf;
    }

    sal_memset(entry, 0, sizeof(egr_xmit_start_count_entry_t));
    for (ovs_class = 0; ovs_class < 13; ovs_class++) {
        soc_mem_field32_set(unit, EGR_XMIT_START_COUNTm, entry, THRESHOLDf,
                            start_count[ovs_class]);
        index = (port % 66) * 16 + ovs_class;
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_XMIT_START_COUNTm(unit, MEM_BLOCK_ALL, index, entry));
    }

    return SOC_E_NONE;
}
#endif

int
_soc_trident3_init_mmu_memory(int unit)
{
    uint32 rval = 0;
    int alloc_size;

    if (_fwd_ctrl_lock[unit] == NULL) {
        _fwd_ctrl_lock[unit] = sal_mutex_create("_fwd_ctrl_lock");
    }
    if (_fwd_ctrl_lock[unit] == NULL) {
        return SOC_E_MEMORY;
    }

    if (_soc_td3_mmu_traffic_ctrl[unit] == NULL) {
        alloc_size = sizeof(_soc_td3_mmu_traffic_ctrl_t);
        _soc_td3_mmu_traffic_ctrl[unit] =
            sal_alloc(alloc_size,"_soc_td3_mmu_traffic_ctrl");
        if (_soc_td3_mmu_traffic_ctrl[unit] == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(_soc_td3_mmu_traffic_ctrl[unit], 0, alloc_size);
    }

    /* Initialize MMU memory */
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, 0));
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, PARITY_ENf, 1);
    /* Need to assert PARITY_EN before setting INIT_MEM to start memory initialization */
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, INIT_MEMf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, INIT_MEMf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
    sal_usleep(20);
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, PARITY_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
    return SOC_E_NONE;
}

int
soc_trident3_clear_mmu_memory(int unit)
{
#define SOC_MMU_BASE_TYPE_IPORT 0
#define SOC_MMU_BASE_TYPE_EPORT 1
#define SOC_MMU_BASE_TYPE_IPIPE 2
#define SOC_MMU_BASE_TYPE_EPIPE 3
#define SOC_MMU_BASE_TYPE_CHIP 4
#define SOC_MMU_BASE_TYPE_XPE 5
#define SOC_MMU_BASE_TYPE_SC 6
#define SOC_MMU_BASE_TYPE_LAYER 7
    int i, j, use_base_type_views, base_type, index, num_views;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    static const struct {
        soc_mem_t mem; /* base view */
        int use_base_type_views; /* 1 means use xpe/sc/etc views */
        int index; /* -1 means clear entire mem */
    } mem_list[] = {
        { MMU_CBPDATA0m, 1, 0 },
        { MMU_CBPDATA32m, 1, 0 },
        { MMU_CBPDATA64m, 1, 0 },
        { MMU_CBPDATA96m, 1, 0 },
        { MMU_CBPDATA128m, 1, 0 },
        { MMU_CBPDATA160m, 1, 0 },
        { MMU_CBPDATA192m, 1, 0 },
        { MMU_CBPDATA224m, 1, 0 },
        { MMU_CBPDATA256m, 1, 0 },
        { MMU_CBPDATA288m, 1, 0 },
        { INVALIDm, 0, -1 }
    };
    for (i = 0; mem_list[i].mem != INVALIDm; i++) {
        mem = mem_list[i].mem;
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }

        index = mem_list[i].index;
        use_base_type_views = mem_list[i].use_base_type_views;
        if (use_base_type_views) {
            base_type = soc_trident3_mem_basetype_get(unit, mem);
            switch (base_type) {
            case SOC_MMU_BASE_TYPE_XPE:
                num_views = NUM_XPE(unit);
                break;
            case SOC_MMU_BASE_TYPE_SC:
                num_views = NUM_SLICE(unit);
                break;
            case SOC_MMU_BASE_TYPE_LAYER:
                num_views = NUM_LAYER(unit);
                break;
            case SOC_MMU_BASE_TYPE_IPIPE:
            case SOC_MMU_BASE_TYPE_EPIPE:
                num_views = NUM_PIPE(unit);
                break;
            default:
                num_views = -1; /* not supported */
                break;
            }
            if (num_views < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "mmu_mem %s, base_type %d will not be"
                                      "cleared \n"),
                           SOC_MEM_NAME(unit,mem), base_type));
                continue;
            }
        } else {
            num_views = 0; /* dont_care */
        }

        sal_memset(entry, 0x0,
                   soc_mem_entry_words(unit, mem) * sizeof(uint32));
        if (use_base_type_views) {
            for (j = 0; j < num_views; j++) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "mmu_mem %s, index %d will be "
                                        "cleared \n"),
                            SOC_MEM_NAME(unit, SOC_MEM_UNIQUE_ACC(unit, mem)[j]),
                            index));
                if (index >= 0) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit,
                                       SOC_MEM_UNIQUE_ACC(unit, mem)[j],
                                       MEM_BLOCK_ALL, index, entry));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_clear(unit,
                                       SOC_MEM_UNIQUE_ACC(unit, mem)[j],
                                       COPYNO_ALL, TRUE));
                }
            }
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "mmu_mem %s, index %d will be "
                                    "cleared \n"),
                        SOC_MEM_NAME(unit, mem), index));
            if (index >= 0) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                                   entry));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, mem, COPYNO_ALL, TRUE));
            }
        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_trident3_port_mapping_init(int unit)
{
    soc_info_t *si;
    int port, phy_port, idb_port;
    int num_port, num_phy_port;
    soc_reg_t reg;
    soc_mem_t mem;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe;

    /*
     * 96 entries MMU_CHFC_SYSPORT_MAPPINGm.SYS_PORT
     * 256 entries SYS_PORTMAPm.DEVICE_PORT_NUMBER
     */

    si = &SOC_INFO(unit);

    num_port = soc_mem_index_count(unit, ING_DEVICE_PORTm) - 1;
    num_phy_port = TD3_NUM_PHY_PORT;

    /* Ingress physical to device port mapping */
    sal_memset(&entry, 0,
               sizeof(ing_idb_to_device_port_number_mapping_table_entry_t));
    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port != -1) {
            pipe = si->port_pipe[port];
            idb_port = si->port_l2i_mapping[port];
        } else if (phy_port >= 130) { /* loopback port (130 and 131 in TD3) */
            pipe = phy_port - 130;
            idb_port = 65;
        } else if (phy_port == 129) { /* management port 0 (if not in use) */
            pipe = 1;
            idb_port = 64;
        } else {
            pipe = (phy_port - 1) / _TD3_PORTS_PER_PIPE;
            idb_port = (phy_port -1 ) % _TD3_PORTS_PER_PIPE;
        }
        mem = SOC_MEM_UNIQUE_ACC
            (unit, ING_IDB_TO_DEVICE_PORT_NUMBER_MAPPING_TABLEm)[pipe];
        soc_mem_field32_set(unit, mem, &entry, DEVICE_PORT_NUMBERf,
                            port == -1 ? 0xff : port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, idb_port, &entry));
    }

    /* Ingress GPP port to device port mapping */
    mem = SYS_PORTMAPm;
    sal_memset(&entry, 0, sizeof(sys_portmap_entry_t));
    for (port = 0; port < num_port; port++) {
        soc_mem_field32_set(unit, mem, &entry, DEVICE_PORT_NUMBERf, port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &entry));
    }

    /* Ingress device port to GPP port mapping
     * PORT_TAB.SRC_SYS_PORT_ID is programmed in the general port config
     * init routine _bcm_fb_port_cfg_init()
     */

    /* Egress device port to physical port mapping */
    rval = 0;
    reg = EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr;
    PBMP_ALL_ITER(unit, port) {
        soc_reg_field_set(unit, reg, &rval, PHYSICAL_PORT_NUMBERf,
                          si->port_l2p_mapping[port]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }

    /* MMU port to physical port mapping */
    rval = 0;
    reg = MMU_PORT_TO_PHY_PORT_MAPPINGr;
    PBMP_ALL_ITER(unit, port) {
        soc_reg_field_set(unit, reg, &rval, PHY_PORTf,
                          si->port_l2p_mapping[port]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }

    /* MMU port to system port mapping */
    rval = 0;
    reg = MMU_PORT_TO_SYSTEM_PORT_MAPPINGr;
    PBMP_ALL_ITER(unit, port) {
        soc_reg_field_set(unit, reg, &rval, SYSTEM_PORTf, port);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_trident3_latency_config
 * Purpose:
 *      Configure latency in SLOT_PIPELINE_CONFIG register
 * Parameters:
 *      unit (IN)    - switch unit
 * Returns:
 *      SOC_E_*
 */

int
soc_trident3_latency_config(int unit)
{
    soc_reg_t reg;
    uint32 rval;
    uint32 latency, pipe_latency_full = 467;
    int frequency, dpp_clk_ratio_x10;
    soc_info_t *si = &SOC_INFO(unit);

    frequency = soc_property_get(unit, spn_CORE_CLOCK_FREQUENCY, si->frequency);
    dpp_clk_ratio_x10 = (frequency > CCLK_FREQ_1012MHZ)? 15:10;

    reg = SLOT_PIPELINE_CONFIGr;
    rval = 0;
    latency = (pipe_latency_full * dpp_clk_ratio_x10 / 10) - 6 + 5;
    if ((dpp_clk_ratio_x10 == 15) && (pipe_latency_full % 2)) {
        /*
         * COVERITY
         * pipe_latency_full is subject to change. This checker will not be True.
         * It is kept intentionally as a defensive check for future development.
         */
        /* coverity[dead_error_line] */
        latency += 1;   /* To round up. */
    }
    soc_reg_field_set(unit, reg, &rval, LATENCYf, latency);
    soc_reg_field_set(unit, reg, &rval, WR_ENf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}



#if 0
STATIC int
_soc_trident3_tdm_idb_oversub_group_set(int unit)
{
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    uint32 pipe_map;
    int block_info_idx;
    soc_reg_t reg;
    int pipe, clport, group, lane, slot, id, mode, ovs_class;
    int port, phy_port, phy_port_base, idb_port;
    int speed_max;
    uint32 rval;
    static const soc_reg_t grp_tbl_regs[] = {
        IS_OVR_SUB_GRP0_TBLr, IS_OVR_SUB_GRP1_TBLr,
        IS_OVR_SUB_GRP2_TBLr, IS_OVR_SUB_GRP3_TBLr,
        IS_OVR_SUB_GRP4_TBLr, IS_OVR_SUB_GRP5_TBLr
    };
    static const soc_reg_t pblk_calendar_regs[] ={
        IS_PBLK0_CALENDARr, IS_PBLK1_CALENDARr,
        IS_PBLK2_CALENDARr, IS_PBLK3_CALENDARr,
        IS_PBLK4_CALENDARr, IS_PBLK5_CALENDARr,
        IS_PBLK6_CALENDARr, IS_PBLK7_CALENDARr
    };
    static int pblk_slots[SOC_TD3_PORT_RATIO_COUNT][7] = {
        { 0, -1,  0,  0, -1,  0, -1 }, /* SOC_TD3_PORT_RATIO_SINGLE */
        { 0, -1,  2,  0, -1,  2, -1 }, /* SOC_TD3_PORT_RATIO_DUAL_1_1 */
        { 0,  0,  2,  0,  0,  2, -1 }, /* SOC_TD3_PORT_RATIO_DUAL_2_1 */
        { 0,  2,  2,  0,  2,  2, -1 }, /* SOC_TD3_PORT_RATIO_DUAL_1_2 */
        { 0, -1,  2,  0, -1,  3, -1 }, /* SOC_TD3_PORT_RATIO_TRI_023_2_1_1 */
        { 0,  0,  2,  0,  0,  3, -1 }, /* SOC_TD3_PORT_RATIO_TRI_023_4_1_1 */
        { 0, -1,  2,  1, -1,  2, -1 }, /* SOC_TD3_PORT_RATIO_TRI_012_1_1_2 */
        { 0,  2,  2,  1,  2,  2, -1 }, /* SOC_TD3_PORT_RATIO_TRI_012_1_1_4 */
        { 0, -1,  2,  1, -1,  3, -1 }  /* SOC_TD3_PORT_RATIO_QUAD */
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    soc_trident3_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        for (group = 0; group < _OVS_GROUP_COUNT; group++) {
            reg = SOC_REG_UNIQUE_ACC(unit, grp_tbl_regs[group])[pipe];
            rval = 0;
            for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                phy_port = tdm->idb_ovs_tdm[pipe][group][slot];
                if (phy_port >= _SOC_TH_TDM_NUM_EXT_PORTS) {
                    idb_port = 0x3f;
                    id = -1;
                } else {
                    port = si->port_p2l_mapping[phy_port];
                    idb_port = si->port_l2i_mapping[port];
                    id = si->port_serdes[port];
                }
                soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id & 0x7);
                soc_reg_field_set(unit, reg, &rval, PORT_NUMf, idb_port);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
            }
        }
    }

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        reg = SOC_REG_UNIQUE_ACC(unit, IS_OVR_SUB_GRP_CFGr)[pipe];
        for (group = 0; group < _OVS_GROUP_COUNT; group++) {
            phy_port = tdm->idb_ovs_tdm[pipe][group][0];
            if (phy_port >= _SOC_TH_TDM_NUM_EXT_PORTS) {
                continue;
            }
            port = si->port_p2l_mapping[phy_port];
            speed_max = 25000 * si->port_num_lanes[port];
            if (speed_max > si->port_speed_max[port]) {
                speed_max = si->port_speed_max[port];
            }
            _soc_trident3_speed_to_ovs_class_mapping(unit, speed_max,
                                                     &ovs_class);
            rval = 0;
            soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf, 4);
            soc_reg_field_set(unit, reg, &rval, SPEEDf, ovs_class);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, group, rval));
        }
    }

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        phy_port_base = PORT_BLOCK_BASE_PORT(port);
        pipe = si->port_pipe[port];
        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);

        mode = tdm->port_ratio[clport];
        reg = SOC_REG_UNIQUE_ACC(unit, pblk_calendar_regs[clport & 0x7])[pipe];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, VALIDf, 1);
        soc_reg_field_set(unit, reg, &rval, SPACINGf, 4);
        for (slot = 0; slot < 7; slot++) {
            lane = pblk_slots[mode][slot];
            if (lane == -1) {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, 0));
                continue;
            }
            port = si->port_p2l_mapping[phy_port_base + lane];
            idb_port = si->port_l2i_mapping[port];
            soc_reg_field_set(unit, reg, &rval, PORT_NUMf, idb_port);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_trident3_tdm_idb_opportunistic_set(int unit, int enable)
{
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe;
    uint32 rval;

    soc_trident3_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        reg = SOC_REG_UNIQUE_ACC(unit, IS_CPU_LB_OPP_CFGr)[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, CPU_OPP_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, LB_OPP_ENf, enable ? 1 : 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

        reg = SOC_REG_UNIQUE_ACC(unit, IS_OPP_SCHED_CFGr)[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, OPP1_PORT_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP2_PORT_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP_OVR_SUB_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, DISABLE_PORT_NUMf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_trident3_tdm_idb_hsp_set(int unit)
{
    soc_info_t *si;
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe;
    int port, idb_port;
    uint32 port_map[NUM_PIPE(unit)];
    uint32 rval;

    si = &SOC_INFO(unit);

    soc_trident3_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        port_map[pipe] = 0;
    }
    SOC_PBMP_ITER(si->eq_pbm, port) {
        pipe = si->port_pipe[port];
        idb_port = si->port_l2i_mapping[port];
        port_map[pipe] |= 1 << idb_port;
    }

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        reg = SOC_REG_UNIQUE_ACC(unit, IS_TDM_HSPr)[pipe];
        soc_reg_field_set(unit, reg, &rval, PORT_BMPf, port_map[pipe]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}
#endif



#if 0
STATIC int
_soc_trident3_tdm_mmu_oversub_group_set(int unit)
{
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    uint32 pipe_map;
    int block_info_idx;
    soc_reg_t reg;
    int pipe, clport, group, lane, slot, id, mode, ovs_class;
    int port, phy_port, phy_port_base, mmu_port, inst;
    int speed_max;
    uint32 rval;
    static const soc_reg_t grp_tbl_regs[] = {
        OVR_SUB_GRP0_TBLr, OVR_SUB_GRP1_TBLr,
        OVR_SUB_GRP2_TBLr, OVR_SUB_GRP3_TBLr,
        OVR_SUB_GRP4_TBLr, OVR_SUB_GRP5_TBLr
    };
    static const soc_reg_t pblk_calendar_regs[] = {
        PBLK0_CALENDARr, PBLK1_CALENDARr, PBLK2_CALENDARr, PBLK3_CALENDARr,
        PBLK4_CALENDARr, PBLK5_CALENDARr, PBLK6_CALENDARr, PBLK7_CALENDARr
    };
    static const int pblk_slots[SOC_TD3_PORT_RATIO_COUNT][7] = {
        { 0, -1,  0,  0, -1,  0, -1 }, /* SOC_TD3_PORT_RATIO_SINGLE */
        { 0, -1,  2,  0, -1,  2, -1 }, /* SOC_TD3_PORT_RATIO_DUAL_1_1 */
        { 0,  0,  2,  0,  0,  2, -1 }, /* SOC_TD3_PORT_RATIO_DUAL_2_1 */
        { 0,  2,  2,  0,  2,  2, -1 }, /* SOC_TD3_PORT_RATIO_DUAL_1_2 */
        { 0, -1,  2,  0, -1,  3, -1 }, /* SOC_TD3_PORT_RATIO_TRI_023_2_1_1 */
        { 0,  0,  2,  0,  0,  3, -1 }, /* SOC_TD3_PORT_RATIO_TRI_023_4_1_1 */
        { 0, -1,  2,  1, -1,  2, -1 }, /* SOC_TD3_PORT_RATIO_TRI_012_1_1_2 */
        { 0,  2,  2,  1,  2,  2, -1 }, /* SOC_TD3_PORT_RATIO_TRI_012_1_1_4 */
        { 0, -1,  2,  1, -1,  3, -1 }  /* SOC_TD3_PORT_RATIO_QUAD */
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    soc_trident3_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        for (group = 0; group < _OVS_GROUP_COUNT; group++) {
            reg = grp_tbl_regs[group];
            rval = 0;
            for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                phy_port = tdm->mmu_ovs_tdm[pipe][group][slot];
                if (phy_port >= _SOC_TH_TDM_NUM_EXT_PORTS) {
                    mmu_port = 0x3f;
                    id = -1;
                } else {
                    mmu_port = si->port_p2m_mapping[phy_port];
                    port = si->port_p2l_mapping[phy_port];
                    id = si->port_serdes[port];
                }
                soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id & 0x7);
                soc_reg_field_set(unit, reg, &rval, PORT_NUMf,
                                  mmu_port & 0x3f);
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));
            }
        }
    }

    reg = OVR_SUB_GRP_CFGr;
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        for (group = 0; group < _OVS_GROUP_COUNT; group++) {
            phy_port = tdm->mmu_ovs_tdm[pipe][group][0];
            if (phy_port >= _SOC_TH_TDM_NUM_EXT_PORTS) {
                continue;
            }
            port = si->port_p2l_mapping[phy_port];
            speed_max = 25000 * si->port_num_lanes[port];
            if (speed_max > si->port_speed_max[port]) {
                speed_max = si->port_speed_max[port];
            }
            _soc_trident3_speed_to_ovs_class_mapping(unit, speed_max,
                                                     &ovs_class);
            rval = 0;
            soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf,
                              speed_max >= 40000 ? 4 : 8);
            soc_reg_field_set(unit, reg, &rval, SPEEDf, ovs_class);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, inst, group, rval));
        }
    }

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        phy_port_base = PORT_BLOCK_BASE_PORT(port);
        speed_max = 25000 * si->port_num_lanes[port];
        if (speed_max > si->port_speed_max[port]) {
            speed_max = si->port_speed_max[port];
        }
        pipe = si->port_pipe[port];
        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;

        mode = tdm->port_ratio[clport];
        reg = pblk_calendar_regs[clport & 0x7];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, VALIDf, 1);
        soc_reg_field_set(unit, reg, &rval, SPACINGf,
                          speed_max >= 40000 ? 4 : 8);
        for (slot = 0; slot < 7; slot++) {
            lane = pblk_slots[mode][slot];
            if (lane == -1) {
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, 0));
                continue;
            }
            mmu_port = si->port_p2m_mapping[phy_port_base + lane];
            soc_reg_field_set(unit, reg, &rval, PORT_NUMf, mmu_port & 0x3f);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_trident3_tdm_mmu_opportunistic_set(int unit, int enable)
{
    uint32 pipe_map;
    soc_reg_t reg;
    int pipe;
    int inst;
    uint32 rval;

    soc_trident3_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;

        reg = CPU_LB_OPP_CFGr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, CPU_OPP_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, LB_OPP_ENf, enable ? 1 : 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));

        reg = OPP_SCHED_CFGr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, OPP1_PORT_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP2_PORT_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP_OVR_SUB_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, DISABLE_PORT_NUMf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_trident3_tdm_mmu_hsp_set(int unit)
{
    soc_info_t *si;
    uint32 pipe_map;
    soc_reg_t reg;
    int pipe;
    int port, phy_port, mmu_port, inst;
    uint32 port_map[NUM_PIPE(unit)];
    uint32 rval;

    si = &SOC_INFO(unit);

    soc_trident3_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        port_map[pipe] = 0;
    }
    SOC_PBMP_ITER(si->eq_pbm, port) {
        pipe = si->port_pipe[port];
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        port_map[pipe] |= 1 << (mmu_port & 0x1f);
    }

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;

        reg = TDM_HSPr;
        soc_reg_field_set(unit, reg, &rval, PORT_BMPf, port_map[pipe]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;
}
#endif

int
soc_trident3_hash_init(int unit)
{
/* Robust Hash initialization */
#ifdef SOC_ROBUST_HASH
subport_id_to_sgpp_map_key_attributes_entry_t sgpp_map_attr;

    /* Initialize Robust Hash Global Datastructure */
    if (soc_feature(unit, soc_feature_robust_hash)) {
        uint32 seed = 0;
        uint32 hash_control_entry[4];

        if (NULL == ROBUSTHASH(unit)) {
            ROBUSTHASH(unit) = sal_alloc(sizeof(soc_robust_hash_db_t),
                                         "soc_robust_hash");
            if (ROBUSTHASH(unit) == NULL) {
                return SOC_E_MEMORY;
            }
            sal_memset(ROBUSTHASH(unit), 0, sizeof(soc_robust_hash_db_t));
        }

        /* L2 Robust Hash Init */

        sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));

        /* Disable robust hash for L2 - this config variable is used only
         * for debug purposes*/
        if ((soc_property_get(unit, "robust_hash_disable_l2", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_L2_ENTRY_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_L2_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
            ROBUSTHASH(unit)->l2.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_L2_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                             hash_control_entry));
            soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_L2_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
            ROBUSTHASH(unit)->l2.enable = 1;

            /* Configure remap and action tables for L2 table */
            ROBUSTHASH(unit)->l2.remap_tab[0] = L2_ENTRY_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->l2.remap_tab[1] = L2_ENTRY_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->l2.action_tab[0] = L2_ENTRY_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->l2.action_tab[1] = L2_ENTRY_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get(unit, spn_ROBUST_HASH_SEED_L2, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->l2),
                                           seed));
        }

        /* L3 Robust Hash Init */

        sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));

        /* Disable robust hash for L3 - this config variable is used only
         * for debug purposes*/
        if ((soc_property_get(unit, "robust_hash_disable_l3", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_L3_ENTRY_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
            ROBUSTHASH(unit)->l3.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                             hash_control_entry));
            soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
            ROBUSTHASH(unit)->l3.enable = 1;

            /* Configure remap and action tables for L3 table */
            ROBUSTHASH(unit)->l3.remap_tab[0] = L3_ENTRY_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->l3.remap_tab[1] = L3_ENTRY_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->l3.action_tab[0] = L3_ENTRY_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->l3.action_tab[1] = L3_ENTRY_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get(unit, spn_ROBUST_HASH_SEED_L3, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->l3),
                                           seed));
        }

    if (SOC_MEM_IS_VALID(unit, EXACT_MATCH_HASH_CONTROLm)) {
        /* Exact Match Robust Hash Init */

        sal_memset
            (hash_control_entry, 0, sizeof(hash_control_entry));

        /* Disable robust hash for EM - this config variable is used only
         * for debug purposes*/
        if ((soc_property_get
                (unit, "robust_hash_disable_exact_match", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_EXACT_MATCH_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
            ROBUSTHASH(unit)->exact_match.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                hash_control_entry));
            soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                 hash_control_entry));
            ROBUSTHASH(unit)->exact_match.enable = 1;

            /* Configure remap and action tables for EM table */
            ROBUSTHASH(unit)->exact_match.remap_tab[0] = EXACT_MATCH_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->exact_match.remap_tab[1] = EXACT_MATCH_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->exact_match.action_tab[0] = EXACT_MATCH_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->exact_match.action_tab[1] = EXACT_MATCH_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get
                    (unit, spn_ROBUST_HASH_SEED_EXACT_MATCH, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->exact_match),
                                           seed));
        }
    }

        /* Vlan Xlate 1 Robust Hash Init */

        sal_memset
            (hash_control_entry, 0, sizeof(hash_control_entry));

        /* Disable robust hash for Vlan Xlate 1 - this config variable is used
         * only for debug purposes*/
        if ((soc_property_get
                (unit, "robust_hash_disable_vlan1", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_VLAN_XLATE_1_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, VLAN_XLATE_1_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                  hash_control_entry));
            ROBUSTHASH(unit)->ing_xlate_1.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                 hash_control_entry));
            soc_mem_field32_set(unit, VLAN_XLATE_1_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                  hash_control_entry));
            ROBUSTHASH(unit)->ing_xlate_1.enable = 1;

            /* Configure remap and action tables for VLAN XLATE 1 table */
            ROBUSTHASH(unit)->ing_xlate_1.remap_tab[0] = VLAN_XLATE_1_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->ing_xlate_1.remap_tab[1] = VLAN_XLATE_1_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->ing_xlate_1.action_tab[0] = VLAN_XLATE_1_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->ing_xlate_1.action_tab[1] = VLAN_XLATE_1_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get
                    (unit, spn_ROBUST_HASH_SEED_VLAN_1, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->ing_xlate_1),
                                           seed));
        }

        /* Vlan Xlate 2 Robust Hash Init */

        sal_memset
            (hash_control_entry, 0, sizeof(hash_control_entry));

        /* Disable robust hash for Vlan Xlate 2 - this config variable is used
         * only for debug purposes*/
        if ((soc_property_get
                (unit, "robust_hash_disable_vlan2", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_VLAN_XLATE_2_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, VLAN_XLATE_2_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                  hash_control_entry));
            ROBUSTHASH(unit)->ing_xlate_2.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                 hash_control_entry));
            soc_mem_field32_set(unit, VLAN_XLATE_2_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                  hash_control_entry));
            ROBUSTHASH(unit)->ing_xlate_2.enable = 1;

            /* Configure remap and action tables for VLAN XLATE 2 table */
            ROBUSTHASH(unit)->ing_xlate_2.remap_tab[0] = VLAN_XLATE_2_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->ing_xlate_2.remap_tab[1] = VLAN_XLATE_2_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->ing_xlate_2.action_tab[0] = VLAN_XLATE_2_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->ing_xlate_2.action_tab[1] = VLAN_XLATE_2_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get
                    (unit, spn_ROBUST_HASH_SEED_VLAN_2, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->ing_xlate_2),
                                           seed));
        }

        /* Egr Vlan Xlate 1 Robust Hash Init */

        sal_memset
            (hash_control_entry, 0, sizeof(hash_control_entry));

        /* Disable robust hash for Egr Vlan Xlate 1 - this config variable is
         * used only for debug purposes*/
        if ((soc_property_get
                (unit, "robust_hash_disable_egress_vlan1", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_EGR_VLAN_XLATE_1_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, EGR_VLAN_XLATE_1_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                        hash_control_entry));
            ROBUSTHASH(unit)->egr_xlate_1.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_EGR_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                     hash_control_entry));
            soc_mem_field32_set(unit, EGR_VLAN_XLATE_1_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                      hash_control_entry));
            ROBUSTHASH(unit)->egr_xlate_1.enable = 1;

            /* Configure remap and action tables for EGR VLAN XLATE 1 table */
            ROBUSTHASH(unit)->egr_xlate_1.remap_tab[0] = EGR_VLAN_XLATE_1_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->egr_xlate_1.remap_tab[1] = EGR_VLAN_XLATE_1_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->egr_xlate_1.action_tab[0] = EGR_VLAN_XLATE_1_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->egr_xlate_1.action_tab[1] = EGR_VLAN_XLATE_1_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get
                    (unit, spn_ROBUST_HASH_SEED_EGRESS_VLAN_1, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->egr_xlate_1),
                                           seed));
        }

        /* Egr Vlan Xlate 2 Robust Hash Init */

        sal_memset
            (hash_control_entry, 0, sizeof(hash_control_entry));

        /* Disable robust hash for Egr Vlan Xlate 2 - this config variable is
         * used only for debug purposes*/
        if ((soc_property_get
                (unit, "robust_hash_disable_egress_vlan2", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_EGR_VLAN_XLATE_2_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, EGR_VLAN_XLATE_2_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                        hash_control_entry));
            ROBUSTHASH(unit)->egr_xlate_2.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_EGR_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                     hash_control_entry));
            soc_mem_field32_set(unit, EGR_VLAN_XLATE_2_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                      hash_control_entry));
            ROBUSTHASH(unit)->egr_xlate_2.enable = 1;

            /* Configure remap and action tables for EGR VLAN XLATE 2 table */
            ROBUSTHASH(unit)->egr_xlate_2.remap_tab[0] = EGR_VLAN_XLATE_2_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->egr_xlate_2.remap_tab[1] = EGR_VLAN_XLATE_2_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->egr_xlate_2.action_tab[0] = EGR_VLAN_XLATE_2_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->egr_xlate_2.action_tab[1] = EGR_VLAN_XLATE_2_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get
                    (unit, spn_ROBUST_HASH_SEED_EGRESS_VLAN_2, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->egr_xlate_2),
                                           seed));
        }
        /* Ing vlan vp Robust Hash Init */

        sal_memset
            (hash_control_entry, 0, sizeof(hash_control_entry));

        /* Disable robust hash for ing vp vlan - this config variable is
         * used only for debug purposes*/
        if ((soc_property_get
                (unit, "robust_hash_disable_ing_vp_vlan", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLm
                    (unit, MEM_BLOCK_ALL, 0, hash_control_entry));
            ROBUSTHASH(unit)->ing_vp_vlan_member.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLm(unit, MEM_BLOCK_ANY,
                                                            0, hash_control_entry));
            soc_mem_field32_set(unit, ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                            hash_control_entry));
            ROBUSTHASH(unit)->ing_vp_vlan_member.enable = 1;

            /* Configure remap and action tables for ing vp vlan table */
            ROBUSTHASH(unit)->ing_vp_vlan_member.remap_tab[0] =
                                        ING_VP_VLAN_MEMBERSHIP_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->ing_vp_vlan_member.remap_tab[1] =
                                        ING_VP_VLAN_MEMBERSHIP_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->ing_vp_vlan_member.action_tab[0] =
                                        ING_VP_VLAN_MEMBERSHIP_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->ing_vp_vlan_member.action_tab[1] =
                                        ING_VP_VLAN_MEMBERSHIP_ACTION_TABLE_Bm;

            seed = soc_property_get(unit,
                                    spn_ROBUST_HASH_SEED_INGRESS_VP_VLAN,
                                    16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                        &(ROBUSTHASH(unit)->ing_vp_vlan_member),
                                        seed));
        }

        /* Egr vlan vp Robust Hash Init */

        sal_memset
            (hash_control_entry, 0, sizeof(hash_control_entry));
        /* Disable robust hash for egr vp vlan - this config variable is
         * used only for debug purposes*/
        if ((soc_property_get
                (unit, "robust_hash_disable_egr_vp_vlan", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLm
                    (unit, MEM_BLOCK_ALL, 0, hash_control_entry));
            ROBUSTHASH(unit)->egr_vp_vlan_member.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLm(unit, MEM_BLOCK_ANY,
                                                            0, hash_control_entry));
            soc_mem_field32_set(unit, EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                            hash_control_entry));
            ROBUSTHASH(unit)->egr_vp_vlan_member.enable = 1;

            /* Configure remap and action tables for ing vp vlan table */
            ROBUSTHASH(unit)->egr_vp_vlan_member.remap_tab[0] =
                                        EGR_VP_VLAN_MEMBERSHIP_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->egr_vp_vlan_member.remap_tab[1] =
                                        EGR_VP_VLAN_MEMBERSHIP_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->egr_vp_vlan_member.action_tab[0] =
                                        EGR_VP_VLAN_MEMBERSHIP_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->egr_vp_vlan_member.action_tab[1] =
                                        EGR_VP_VLAN_MEMBERSHIP_ACTION_TABLE_Bm;

            seed = soc_property_get(unit,
                                    spn_ROBUST_HASH_SEED_EGRESS_VP_VLAN,
                                    16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                        &(ROBUSTHASH(unit)->egr_vp_vlan_member),
                                        seed));
        }

        /* Mpls Robust Hash Init */

        sal_memset
            (hash_control_entry, 0, sizeof(hash_control_entry));

        /* Disable robust hash for mpls - this config variable is
         * used only for debug purposes*/
        if ((soc_property_get
                (unit, "robust_hash_disable_mpls", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_MPLS_ENTRY_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                        hash_control_entry));
            ROBUSTHASH(unit)->mpls_entry.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                     hash_control_entry));
            soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                      hash_control_entry));
            ROBUSTHASH(unit)->mpls_entry.enable = 1;

            /* Configure remap and action tables for MPLS ENTRY table */
            ROBUSTHASH(unit)->mpls_entry.remap_tab[0] =
                                                MPLS_ENTRY_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->mpls_entry.remap_tab[1] =
                                                MPLS_ENTRY_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->mpls_entry.action_tab[0] =
                                                MPLS_ENTRY_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->mpls_entry.action_tab[1] =
                                                MPLS_ENTRY_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get
                    (unit, spn_ROBUST_HASH_SEED_MPLS, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->mpls_entry),
                                           seed));
        }

    if (SOC_MEM_IS_VALID(unit, ING_DNAT_ADDRESS_TYPE_HASH_CONTROLm)) {
        /*  Ing Dnat Address Robust Hash Init */

        sal_memset
            (hash_control_entry, 0, sizeof(hash_control_entry));

        /* Disable robust hash for ing dnat address type - this config variable
         * is used only for debug purposes*/
        if ((soc_property_get
                (unit, "robust_hash_disable_ing_dnat_address", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_ING_DNAT_ADDRESS_TYPE_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, ING_DNAT_ADDRESS_TYPE_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_ING_DNAT_ADDRESS_TYPE_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                           hash_control_entry));
            ROBUSTHASH(unit)->ing_dnat_address.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_ING_DNAT_ADDRESS_TYPE_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                          hash_control_entry));
            soc_mem_field32_set(unit, ING_DNAT_ADDRESS_TYPE_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_ING_DNAT_ADDRESS_TYPE_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                           hash_control_entry));
            ROBUSTHASH(unit)->ing_dnat_address.enable = 1;

            /* Configure remap and action tables for ING DNAT ADDRESS table */
            ROBUSTHASH(unit)->ing_dnat_address.remap_tab[0] =
                                            ING_DNAT_ADDRESS_TYPE_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->ing_dnat_address.remap_tab[1] =
                                            ING_DNAT_ADDRESS_TYPE_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->ing_dnat_address.action_tab[0] =
                                            ING_DNAT_ADDRESS_TYPE_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->ing_dnat_address.action_tab[1] =
                                            ING_DNAT_ADDRESS_TYPE_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get
                    (unit, spn_ROBUST_HASH_SEED_ING_DNAT_ADDRESS, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->ing_dnat_address),
                                           seed));
        }
    }

    if (SOC_MEM_IS_VALID(unit, SUBPORT_ID_TO_SGPP_MAP_HASH_CONTROLm)) {
        /*  Subport id to sgpp map Robust Hash Init */

        sal_memset(hash_control_entry, 0, sizeof(hash_control_entry));

        /* Disable robust hash for subport id to sgpp map - this config
         * variable is used only for debug purposes */
        if ((soc_property_get
                (unit, "robust_hash_disable_subport_id_sgpp_map", 0)) == 1) {
            SOC_IF_ERROR_RETURN
                (READ_SUBPORT_ID_TO_SGPP_MAP_HASH_CONTROLm
                    (unit, MEM_BLOCK_ANY, 0, hash_control_entry));
                soc_mem_field32_set(unit, SUBPORT_ID_TO_SGPP_MAP_HASH_CONTROLm ,
                                    hash_control_entry, ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_SUBPORT_ID_TO_SGPP_MAP_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                            hash_control_entry));
            ROBUSTHASH(unit)->subport_id_sgpp_map.enable = 0;
        } else {
            /* Enable robust hashing */
             SOC_IF_ERROR_RETURN
                (READ_SUBPORT_ID_TO_SGPP_MAP_KEY_ATTRIBUTESm(unit, MEM_BLOCK_ANY, 0,
                                                           &sgpp_map_attr));
            soc_mem_field32_set(unit, SUBPORT_ID_TO_SGPP_MAP_KEY_ATTRIBUTESm,
                               &sgpp_map_attr, KEY_WIDTHf, 0x5);
            SOC_IF_ERROR_RETURN
                (WRITE_SUBPORT_ID_TO_SGPP_MAP_KEY_ATTRIBUTESm(unit, MEM_BLOCK_ALL, 0,
                                                            &sgpp_map_attr));
            SOC_IF_ERROR_RETURN
                (READ_SUBPORT_ID_TO_SGPP_MAP_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                           hash_control_entry));
            soc_mem_field32_set(unit, SUBPORT_ID_TO_SGPP_MAP_HASH_CONTROLm,
                                hash_control_entry, ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_SUBPORT_ID_TO_SGPP_MAP_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                            hash_control_entry));
            ROBUSTHASH(unit)->subport_id_sgpp_map.enable = 1;

            /* Configure remap and action tables for Subport id to sgpp map table */
            ROBUSTHASH(unit)->subport_id_sgpp_map.remap_tab[0] =
                                            SUBPORT_ID_TO_SGPP_MAP_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->subport_id_sgpp_map.remap_tab[1] =
                                            SUBPORT_ID_TO_SGPP_MAP_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->subport_id_sgpp_map.action_tab[0] =
                                            SUBPORT_ID_TO_SGPP_MAP_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->subport_id_sgpp_map.action_tab[1] =
                                            SUBPORT_ID_TO_SGPP_MAP_ACTION_TABLE_Bm;

            /* Initialize seed for robust hash */
            seed = soc_property_get
                    (unit, spn_ROBUST_HASH_SEED_SUBPORT_ID_TO_SGPP_MAP, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->subport_id_sgpp_map),
                                           seed));
        }
    }
    }
#endif /* SOC_ROBUST_HASH */
    return SOC_E_NONE;
}

STATIC int _soc_trident3_tdm_init(int unit)
{
	soc_control_t *soc = SOC_CONTROL(unit);
	soc_info_t *si = &SOC_INFO(unit);
	soc_port_schedule_state_t *port_schedule_state;
	soc_port_map_type_t *in_portmap;
	int port, speed;
	int rv;
	if (soc->tdm_info == NULL) {
		soc->tdm_info = sal_alloc(sizeof(_soc_trident3_tdm_temp_t),
									"Trident3 TDM info");
		if (soc->tdm_info == NULL) {
			return SOC_E_MEMORY;
		}
		sal_memset(soc->tdm_info, 0, sizeof(_soc_trident3_tdm_temp_t));
	}

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_td3_tdm_scache_recovery(unit));
        return SOC_E_NONE;
    } else {
        SOC_IF_ERROR_RETURN(
            soc_td3_tdm_scache_allocate(unit));
    }
#endif
	port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                    "Trident3 soc_port_schedule_state_t");
	if (port_schedule_state == NULL) {
        return SOC_E_MEMORY;
    }
	sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));
	/* Core clock frequency */
	port_schedule_state->frequency = si->frequency;

	/* Construct in_port_map */
	in_portmap = &port_schedule_state->in_port_map;
	PBMP_PORT_ITER(unit, port) {
		if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
			continue;
		}
		if (IS_HG_PORT(unit, port)) {
            speed = soc_port_speed_higig2eth(si->port_speed_max[port]);
        } else {
            speed = si->port_speed_max[port];
        }
        in_portmap->log_port_speed[port] = speed;
        in_portmap->port_num_lanes[port] = si->port_num_lanes[port];
	}
	sal_memcpy(in_portmap->port_p2l_mapping, si->port_p2l_mapping,
                sizeof(int)*TRIDENT3_TDM_PHY_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_l2p_mapping, si->port_l2p_mapping,
                sizeof(int)*_TD3_TDM_DEV_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_p2m_mapping, si->port_p2m_mapping,
                sizeof(int)*TRIDENT3_TDM_PHY_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_m2p_mapping, si->port_m2p_mapping,
                sizeof(int)*_TD3_TDM_MMU_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_l2i_mapping, si->port_l2i_mapping,
                sizeof(int)*_TD3_TDM_DEV_PORTS_PER_DEV);
    sal_memcpy(&in_portmap->physical_pbm, &si->physical_pbm, sizeof(pbmp_t));
    sal_memcpy(&in_portmap->hg2_pbm, &si->hg.bitmap, sizeof(pbmp_t));
    sal_memcpy(&in_portmap->management_pbm, &si->management_pbm,
                sizeof(pbmp_t));
    sal_memcpy(&in_portmap->oversub_pbm, &si->oversub_pbm, sizeof(pbmp_t));

	port_schedule_state->is_flexport = 0;
	rv = soc_td3_port_schedule_tdm_init(unit, port_schedule_state);

    rv = soc_td3_tdm_init(unit, port_schedule_state);
	if (rv != SOC_E_NONE) {
        LOG_CLI((BSL_META_U(unit,
            "Unsupported config for TDM calendar generation\n")));
        sal_free(port_schedule_state);
        return rv;
    }
	rv = soc_td3_soc_tdm_update(unit, port_schedule_state);

    sal_free(port_schedule_state);

    return rv;
}

/* For a given logical port, return the OBM id and the lane number */
int
soc_trident3_port_obm_info_get(int unit, soc_port_t port,
                               int *obm_id, int *lane)
{
    soc_info_t *si;
    soc_port_t phy_port;
    int clport, port_block_base;

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        (SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port))) {
        return SOC_E_PARAM;
    }
    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
    port_block_base = PORT_BLOCK_BASE_PORT(port);
    if (lane != NULL) {
        *lane = (phy_port - port_block_base) % _TD3_PORTS_PER_PBLK;
    }

    clport = si->port_serdes[port];
    if ((obm_id != NULL) && (NUM_PIPE(unit) != 1)) {
        *obm_id = clport & 0xF;
    } else {
        *obm_id = clport;
    }
    return SOC_E_NONE;
}

extern int
(*_phy_tsce_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);
extern int
(*_phy_tscf_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);

int
soc_trident3_tscx_firmware_set(int unit, int port, uint8 *array, int datalen)
{
    soc_mem_t mem = IS_CL_PORT(unit, port) ?
                    CLPORT_WC_UCMEM_DATAm : XLPORT_WC_UCMEM_DATAm;
    soc_reg_t reg = IS_CL_PORT(unit, port) ?
                    CLPORT_WC_UCMEM_CTRLr : XLPORT_WC_UCMEM_CTRLr;

    return soc_warpcore_firmware_set(unit, port, array, datalen, 0, mem, reg);
}

#define TSC_REG_ADDR_TSCID_SET(_phy_reg, _phyad)    \
                            ((_phy_reg) |= ((_phyad) & 0x1f) << 19)

STATIC int
_soc_trident3_mdio_addr_to_port(uint32 phy_addr)
{
    int bus, offset;

    /* Must be internal MDIO address */
    if ((phy_addr & 0x80) == 0) {
        return 0;
    }

    /*
     * Internal phy address:
     * bus 0 phy 1 to 24 are mapped to Physical port 1 to 24
     * bus 1 phy 1 to 16 are mapped to Physical port 25 to 40
     * bus 2 phy 1 to 24 are mapped to Physical port 41 to 84
     * bus 3 phy 1 to 24 are mapped to Physical port 85 to 88
     * bus 4 phy 1 to 16 are mapped to Physical port 89 to 104
     * bus 5 phy 1 to 24 are mapped to Physical port 105 to 128
     * bus 6 phy 1 is mapped to Physical port 129 and
     *       phy 3 is mapped to Physical port 131
     */
    bus = PHY_ID_BUS_NUM(phy_addr);
    if (bus > 6) {
        return 0;
    }
    switch (bus) {
    case 0: offset = 0;
        break;
    case 1: offset = 24;
        break;
    case 2: offset = 40;
        break;
    case 3: offset = 84;
        break;
    case 4: offset = 88;
        break;
    case 5: offset = 104;
        break;
    case 6: offset = 128;
        if ((phy_addr & 0x1f) == 2 || (phy_addr & 0x1f) > 3) {
            return 0;
        }
        break;
    default:
        return 0;
    }

    return (phy_addr & 0x1f) + offset;
}

STATIC int
_soc_trident3_tscx_reg_read(int unit, uint32 phy_addr,
                            uint32 phy_reg, uint32 *phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_trident3_mdio_addr_to_port(phy_addr);

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_trident3_tscx_reg_read[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
    rv = soc_sbus_tsc_reg_read(unit, port, blk, phy_addr,
                               phy_reg, phy_data);
    return rv;
}

STATIC int
_soc_trident3_tscx_reg_write(int unit, uint32 phy_addr,
                             uint32 phy_reg, uint32 phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_trident3_mdio_addr_to_port(phy_addr);

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_trident3_tscx_reg_write[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
    rv = soc_sbus_tsc_reg_write(unit, port, blk, phy_addr,
                                phy_reg, phy_data);
    return rv;
}

#ifdef BCM_CMICX_SUPPORT
STATIC int
_soc_td3_ledup_init(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int freq;
    uint32 rval, clk_half_period, refresh_period;

    /* freq in KHz */
    freq = (si->frequency) * 1000;

    /* For LED refresh period = 33 ms (about 30Hz)  */

    /* refresh period
       = (required refresh period in sec)*(switch clock frequency in Hz)
     */
    refresh_period = (freq * 33);

    rval = 0;
    soc_reg_field_set(unit, U0_LED_REFRESH_CTRLr, &rval,
                      REFRESH_CYCLE_PERIODf, refresh_period);

    SOC_IF_ERROR_RETURN(WRITE_U0_LED_REFRESH_CTRLr(unit, rval));

    /* For LED clock period */
    /* LEDCLK_HALF_PERIOD
       = [(required LED clock period in sec)/2]*(M0SS clock frequency in Hz)

       Where M0SS freqency is 858MHz and
       Typical LED clock period is 200ns(5MHz) = 2*10^-7
     */
    freq = 858 * 1000000;
    clk_half_period = (freq + 2500000) / 5000000;
    clk_half_period = clk_half_period / 2;

        rval = 0;
    soc_reg_field_set(unit, U0_LED_CLK_DIV_CTRLr, &rval,
                      LEDCLK_HALF_PERIODf, clk_half_period);

    SOC_IF_ERROR_RETURN(WRITE_U0_LED_CLK_DIV_CTRLr(unit, rval));

    /*  Fixed for TD3
     *  When PM shift out the led data, it shift out lane 3 first,followed by lane 2, 1, and 0.
     *  The LAST_PORT value to be the port number of sub-port 0. (124 for TD3)
     */

    SOC_IF_ERROR_RETURN(READ_U0_LED_ACCU_CTRLr(unit, &rval));
    soc_reg_field_set(unit, U0_LED_ACCU_CTRLr, &rval,
                      LAST_PORTf, 124);
    soc_reg_field_set(unit, U0_LED_ACCU_CTRLr, &rval,
                      PORT_ENDIANNESSf, 1);
    SOC_IF_ERROR_RETURN(WRITE_U0_LED_ACCU_CTRLr(unit, rval));

    /* Initialize M0s for LED and Firmware Link Scan*/
    SOC_IF_ERROR_RETURN(soc_iproc_m0_init(unit));

    return SOC_E_NONE;
}

#endif /* BCM_CMICX_SUPPORT */

/*
 * Function:
 *      _soc_trident3_mmu_ep_credit_reset
 * Purpose:
 *      Initalize EP credits in MMU so EP is in charge
 *      of distributing the correct number of credits.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_trident3_mmu_ep_credit_reset(int unit, soc_port_t port)
{
    soc_reg_t reg;
    uint32 rval = 0;

    reg = MMU_PORT_CREDITr;

    soc_reg_field_set(unit, reg, &rval, INITIALIZEf, 1);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, port, 0, rval));
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, port, 0, 0));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_trident3_ing_fsaf_init
 * Purpose:
 *      Initalize ForceSAF global settings.
 *      Currently, duration register is set. Should always be
 *      targeted for a minimum duration of 2us.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Per port settings leverage ASF init process.
 */
STATIC int
_soc_trident3_ing_fsaf_init(int unit)
{
    int pipe;
    int duration;
    uint64 rval64;
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t reg;
    static const soc_reg_t idb_fsaf_cfg = IDB_DBG_Br;

    duration = si->frequency * 2;
    for (pipe = 0; pipe < si->num_pipe; pipe ++) {
        reg = SOC_REG_UNIQUE_ACC(unit, idb_fsaf_cfg)[pipe];
        SOC_IF_ERROR_RETURN
            (soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, FIELD_Af, duration);
        SOC_IF_ERROR_RETURN
            (soc_reg_set(unit, reg, REG_PORT_ANY, 0, rval64));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_trident3_edb_buf_reset
 * Purpose:
 *      Initialize EP credits in MMU, release EDB port buffer and
 *      enable cell request in EP.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 *      reset   - (IN) Reset.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_trident3_edb_buf_reset(int unit, soc_port_t port, int reset)
{
    soc_info_t *si = &SOC_INFO(unit);
    uint32 entry[SOC_MAX_MEM_WORDS];
    int phy_port;
    uint32 rval;
    int level;
    soc_timeout_t to;
    int wait_time = 250000;

    if (SAL_BOOT_QUICKTURN) {
        wait_time *= 20;
    }

    /* Get physical port */
    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        return SOC_E_INTERNAL;
    }

    if (reset) {
        /* Set register to get number of used entries in EDB buffer */
        SOC_IF_ERROR_RETURN
            (READ_EGR_EDB_MISC_CTRLr(unit, &rval));
        soc_reg_field_set(unit, EGR_EDB_MISC_CTRLr, &rval,
                          SELECT_CURRENT_USED_ENTRIESf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_EDB_MISC_CTRLr(unit, rval));

        soc_timeout_init(&to, wait_time, 0);

        /* Poll until EDB buffer is empty */
        for (;;) {
            SOC_IF_ERROR_RETURN
                (READ_EGR_MAX_USED_ENTRIESm(unit, MEM_BLOCK_ALL,
                                            phy_port, entry));
            level = soc_mem_field32_get(unit, EGR_MAX_USED_ENTRIESm,
                                        entry, LEVELf);
            if (level == 0) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "EDB buffer drain timeout: "
                                      "port %d, %s, "
                                      "timeout (pending: %d)\n"),
                           unit, SOC_PORT_NAME(unit, port), level));
                return SOC_E_INTERNAL;
            }
        }

        /*
         * Hold EDB port buffer in reset state and disable cell
         * request generation in EP.
         */
        SOC_IF_ERROR_RETURN
            (READ_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                 phy_port, entry));
        /* Don't reset for PMQ ports in QSGMII mode */
        if (!(SOC_IS_HURRICANE4(unit) && IS_QSGMII_PORT(unit, port))) {
            soc_mem_field32_set(unit, EGR_PER_PORT_BUFFER_SFT_RESETm,
                                entry, ENABLEf, 1);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                  phy_port, entry));
        SOC_IF_ERROR_RETURN
            (READ_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
        soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));

    } else {

        /* Initialize EP credits in MMU */
        /* Don't reset for PMQ ports in QSGMII mode */
        if (!(SOC_IS_HURRICANE4(unit) && IS_QSGMII_PORT(unit, port))) {
            SOC_IF_ERROR_RETURN(_soc_trident3_mmu_ep_credit_reset(unit, port));
        }

        /*
         * Release EDB port buffer reset and
         * enable cell request generation in EP.
         */
        SOC_IF_ERROR_RETURN
            (READ_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                 phy_port, entry));
        soc_mem_field32_set(unit, EGR_PER_PORT_BUFFER_SFT_RESETm,
                            entry, ENABLEf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                  phy_port, entry));

        SOC_IF_ERROR_RETURN
            (READ_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
        soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
    }

    return SOC_E_NONE;
}

soc_error_t
soc_trident3_idb_buf_reset(int unit, soc_port_t port, int reset)
{
    soc_reg_t reg, reg1;
    int port_block_base, phy_port, lane;
    int pipe, obm, clport, obm_usage_count = ~0, ca_empty = 0;
    uint32 rval;
    uint64 rval1, rval2;
    soc_info_t *si = &SOC_INFO(unit);
    static const soc_reg_t idb_obm_usage_regs[] = {
        IDB_OBM0_USAGEr, IDB_OBM1_USAGEr,
        IDB_OBM2_USAGEr, IDB_OBM3_USAGEr,
        IDB_OBM4_USAGEr, IDB_OBM5_USAGEr,
        IDB_OBM6_USAGEr, IDB_OBM7_USAGEr,
        IDB_OBM8_USAGEr, IDB_OBM9_USAGEr,
        IDB_OBM10_USAGEr, IDB_OBM11_USAGEr,
        IDB_OBM12_USAGEr, IDB_OBM13_USAGEr,
        IDB_OBM14_USAGEr, IDB_OBM15_USAGEr
    };
    static const soc_reg_t idb_obm_ca_status_regs[] = {
        IDB_OBM0_CA_HW_STATUSr, IDB_OBM1_CA_HW_STATUSr,
        IDB_OBM2_CA_HW_STATUSr, IDB_OBM3_CA_HW_STATUSr,
        IDB_OBM4_CA_HW_STATUSr, IDB_OBM5_CA_HW_STATUSr,
        IDB_OBM6_CA_HW_STATUSr, IDB_OBM7_CA_HW_STATUSr,
        IDB_OBM8_CA_HW_STATUSr, IDB_OBM9_CA_HW_STATUSr,
        IDB_OBM10_CA_HW_STATUSr, IDB_OBM11_CA_HW_STATUSr,
        IDB_OBM12_CA_HW_STATUSr, IDB_OBM13_CA_HW_STATUSr,
        IDB_OBM14_CA_HW_STATUSr, IDB_OBM15_CA_HW_STATUSr
    };
    static const soc_reg_t obm_ctrl_regs[] = {
        IDB_OBM0_CONTROLr, IDB_OBM1_CONTROLr,
        IDB_OBM2_CONTROLr, IDB_OBM3_CONTROLr,
        IDB_OBM4_CONTROLr, IDB_OBM5_CONTROLr,
        IDB_OBM6_CONTROLr, IDB_OBM7_CONTROLr,
        IDB_OBM8_CONTROLr, IDB_OBM9_CONTROLr,
        IDB_OBM10_CONTROLr, IDB_OBM11_CONTROLr,
        IDB_OBM12_CONTROLr, IDB_OBM13_CONTROLr,
        IDB_OBM14_CONTROLr, IDB_OBM15_CONTROLr
    };
    static const soc_reg_t obm_ca_ctrl_regs[] = {
        IDB_OBM0_CA_CONTROLr, IDB_OBM1_CA_CONTROLr,
        IDB_OBM2_CA_CONTROLr, IDB_OBM3_CA_CONTROLr,
        IDB_OBM4_CA_CONTROLr, IDB_OBM5_CA_CONTROLr,
        IDB_OBM6_CA_CONTROLr, IDB_OBM7_CA_CONTROLr,
        IDB_OBM8_CA_CONTROLr, IDB_OBM9_CA_CONTROLr,
        IDB_OBM10_CA_CONTROLr, IDB_OBM11_CA_CONTROLr,
        IDB_OBM12_CA_CONTROLr, IDB_OBM13_CA_CONTROLr,
        IDB_OBM14_CA_CONTROLr, IDB_OBM15_CA_CONTROLr,
    };
    static const soc_field_t fifo_empty_port_fields[] = {
        FIFO_EMPTY_PORT0f, FIFO_EMPTY_PORT1f, FIFO_EMPTY_PORT2f,
        FIFO_EMPTY_PORT3f
    };
    static const soc_field_t port_reset_fields[] = {
        PORT0_RESETf, PORT1_RESETf, PORT2_RESETf, PORT3_RESETf
    };

    phy_port = si->port_l2p_mapping[port];
    port_block_base = PORT_BLOCK_BASE_PORT(port);
    lane = phy_port - port_block_base;
    pipe = si->port_pipe[port];
    clport = si->port_serdes[port];
    obm = clport & 0xF;

    if (reset && !SAL_BOOT_SIMULATION) {
        soc_timeout_t to;

        reg = SOC_REG_UNIQUE_ACC(unit, idb_obm_usage_regs[obm])[pipe];
        reg1 = SOC_REG_UNIQUE_ACC(unit, idb_obm_ca_status_regs[obm])[pipe];

        soc_timeout_init(&to, 250000, 0);
        /* Poll until IDB buffer is empty */
        for (;;) {
            if (0 != obm_usage_count) {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, REG_PORT_ANY, lane,
                                                &rval1));
                obm_usage_count = soc_reg64_field32_get(unit, reg, rval1,
                                                        TOTAL_COUNTf);
            }
            if (0 == ca_empty) {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg1, REG_PORT_ANY, 0,
                                                &rval2));
                ca_empty = soc_reg64_field32_get(unit, reg1, rval2,
                                                 fifo_empty_port_fields[lane]);
            }
            if ((0 == obm_usage_count) && (1 == ca_empty)) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                    "IDBBufferDrainTimeOut:port %d,%s, timeout"
                                    "(idb_obm_usage: %d) "
                                    "(ca_fifo_empty: %d)\n"),
                          unit, SOC_PORT_NAME(unit, port), obm_usage_count,
                          ca_empty));
                return SOC_E_INTERNAL;
            }
        }
    }

    reg = SOC_REG_UNIQUE_ACC(unit, obm_ctrl_regs[obm])[pipe];
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, port_reset_fields[lane], reset);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    reg = SOC_REG_UNIQUE_ACC(unit, obm_ca_ctrl_regs[obm])[pipe];
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, port_reset_fields[lane], reset);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td3_speed_class_validate
 * Purpose:
 *      Check speed classes
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      This function is called at early stage of initialization for
 *      port configuration check.
 */
int
soc_td3_speed_class_validate(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int port, speed;
    uint32 speed_mask, count;

    speed_mask = 0;
    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port) ||
            IS_MANAGEMENT_PORT(unit, port) || IS_CPU_PORT(unit, port) ||
            IS_LB_PORT(unit, port)) {
            continue;
        }
        speed = soc_port_speed_higig2eth(si->port_speed_max[port]);
        speed_mask |= SOC_PA_SPEED(speed);
    }

    count = _shr_popcount(speed_mask);
    if (count > ((si->oversub_mode) ? 5 : 4)) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "No port configurations with more than 4 port "
                              "speed classes are supported.\n")));
        return SOC_E_CONFIG;
    }

    if(SOC_FAILURE(soc_td3_speed_mix_validate(unit, speed_mask))) {
        return SOC_E_CONFIG;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_trident3_mdio_rate_divisor_set
 * Purpose:
 *      Setup MDIO divisor speeds
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_trident3_mdio_rate_divisor_set(int unit)
{
    int int_divisor, ext_divisor, delay;
    /* spn_RATE_EXT_MDIO_DIVIDEND and spn_RATE_INT_MDIO_DIVIDEND are
     * NOT applicable to TRIDENT3.
     */

    ext_divisor = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVISOR,
                                   TD3X_RATE_EXT_MDIO_DIVISOR_DEF(unit));

    int_divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR,
                                   TD3X_RATE_INT_MDIO_DIVISOR_DEF(unit));

    delay = soc_property_get(unit, spn_MDIO_OUTPUT_DELAY, -1);
    if (delay < 0  || delay > 255) {
        /* valid range for delay is 0-255
         * if outside this range, skip configuring delay
         */
        delay = -1;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
              "Config MIIM rings with int divisor %d, ext divisor %d, delay %d\n"),
              int_divisor, ext_divisor, delay));

    soc_cmicx_miim_divider_set_all(unit, int_divisor, ext_divisor, delay);

    return SOC_E_NONE;
}

int
_soc_trident3_macro_flow_offset_init(int unit)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    int count, sub_sel, offset, index;
    static int rtag7_field_width[] = { 16, 16, 4, 16, 8, 8, 16, 16 };
    uint32 rval;

    /* Populate and enable RTAG7 Macro flow offset table */
    if (soc_mem_is_valid(unit, RTAG7_FLOW_BASED_HASHm)) {
        count = soc_mem_index_max(unit, RTAG7_FLOW_BASED_HASHm);
        sal_memset(entry, 0, sizeof(rtag7_flow_based_hash_entry_t));
        for (index = 0; index < count; ) {
            for (sub_sel = 0; sub_sel < 8 && index < count; sub_sel++) {
                for (offset = 0;
                     offset < rtag7_field_width[sub_sel] && index < count;
                     offset++) {
                    soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                        SUB_SEL_ECMPf, sub_sel);
                    soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                        OFFSET_ECMPf, offset);
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
                    if(soc_feature(unit, soc_feature_riot) ||
                       soc_feature(unit, soc_feature_multi_level_ecmp)) {
                        soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                            SUB_SEL_ECMP_LEVEL1f, sub_sel);
                        soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                            OFFSET_ECMP_LEVEL1f, offset);
                    }
#endif
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, RTAG7_FLOW_BASED_HASHm,
                                       MEM_BLOCK_ANY, index, &entry));
                    index++;
                }
            }
        }
        rval = 0;
        soc_reg_field_set(unit, RTAG7_HASH_SELr, &rval, USE_FLOW_SEL_ECMPf, 1);
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
        if(soc_feature(unit, soc_feature_riot) ||
           soc_feature(unit, soc_feature_multi_level_ecmp)) {
            soc_reg_field_set(unit, RTAG7_HASH_SELr,
                &rval, USE_FLOW_SEL_ECMP1f, 1);
        }
#endif
        SOC_IF_ERROR_RETURN(WRITE_RTAG7_HASH_SELr(unit, rval));
    }

    return SOC_E_NONE;
}


STATIC int
_soc_trident3_misc_init(int unit)
{
#if defined(BCM_56870_A0)
    soc_control_t *soc;
    soc_info_t *si;
    int block_info_idx;
    soc_mem_t mem;
    soc_reg_t reg;
    uint32 rval, fval;
    uint64 rval64;
    soc_field_t fields[2];
    uint32 values[2];
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_pbmp_t pbmp;
    int clport, lane, mode, index, factor;
    uint32 correction_duration, detection_duration;
    int port, phy_port, phy_port_base;
    int parity_enable, pbm_count=0;
    modport_map_subport_entry_t map_entry;
    ing_dest_port_enable_entry_t ingr_entry;

    static soc_field_t port_fields[_TD3_PORTS_PER_PBLK] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };
    static const int mode_encodings[SOC_TD3_PORT_RATIO_COUNT] = {
        4, 3, 3, 3, 2, 2, 1, 1, 0
    };
    uint32 mc_ctrl = 0;
    int fabric_port_enable = 1;
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    int num_overlay_ecmp_rh_flowset_entries;
    int ecmp_levels = 1;
#endif
    int num_lag_rh_flowset_entries;

    soc = SOC_CONTROL(unit);
    si = &SOC_INFO(unit);

    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));
    SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_stop(unit));

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    if (parity_enable) {
        if (!SOC_WARM_BOOT(unit)) {
            /* Initialize PORT MIB counter */
            rval = 0;
            SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
                port = SOC_BLOCK_PORT(unit, block_info_idx);
                soc_reg_field_set(unit, CLPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf);
                SOC_IF_ERROR_RETURN(WRITE_CLPORT_MIB_RESETr(unit, port, rval));
                SOC_IF_ERROR_RETURN(WRITE_CLPORT_MIB_RESETr(unit, port, 0));
            }
            rval = 0;
            SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_XLPORT) {
                port = SOC_BLOCK_PORT(unit, block_info_idx);
                soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf);
                SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, rval));
                SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, 0));
            }
            /* Certain mems/regs need to be cleared before enabling SER */
            SOC_IF_ERROR_RETURN(soc_trident3_clear_mmu_memory(unit));
            /*SOC_IF_ERROR_RETURN(_soc_trident3_clear_all_port_data(unit));*/
        }
        soc_ser_alpm_cache_check(unit);
        soc_ser_log_init(unit, NULL, 0);

        /* Enable l2_mgmt interrupt */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, L2_MGMT_INTR_MASKr, REG_PORT_ANY,
                                    SER_FIFO_NOT_EMPTYf, 1));

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        
        /* (void)soc_cmicm_intr3_enable(unit, 0x2); L2_MGMT_TO_CMIC_INTR bit 1 */
        /* PVT interrupt re-enable - this needs to touch hardware register */
        soc_trident3_pvt_intr_enable(unit);
    }
#endif
        soc_trident3_tcam_ser_init(unit);
        SOC_IF_ERROR_RETURN(soc_trident3_ser_enable_all(unit, TRUE));
        soc_trident3_ser_register(unit);

#if defined(SER_TR_TEST_SUPPORT)
        /*Initialize chip-specific functions for SER testing*/
        soc_td3_ser_test_register(unit);
#endif /*defined(SER_TR_TEST_SUPPORT*/
    }

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
        SOC_IF_ERROR_RETURN(_soc_td3_ledup_init(unit));
    }
#endif /* BCM_CMICX_SUPPORT */

    SOC_IF_ERROR_RETURN(_soc_trident3_init_mmu_memory(unit));

    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_trident3_ser_enable_parity_table_all(unit, TRUE));
        SOC_IF_ERROR_RETURN(READ_MMU_GCFG_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, PARITY_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
    }
    /* SDK-174533 Certain memories do have 1 bit error so the below function enables
       parity and disable 1 bit error correction reporting by default for these
       memories in order for the customers to determine if the part is bad or not.
    */
    SOC_IF_ERROR_RETURN(soc_trident3_ser_enable_special_table(unit));

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(
            _soc_cancun_load_status_clear(unit, CANCUN_SOC_FILE_TYPE_CIH));
        /* Dummy call to clear garbage h/w values */
        SOC_IF_ERROR_RETURN
            (soc_trident3_temperature_monitor_get(unit,
                                                  COUNTOF(pvtmon_result_reg),
                                                  NULL,
                                                  NULL));
    }

    /* UFT/UAT config programming */
    SOC_IF_ERROR_RETURN(soc_trident3_uft_uat_config(unit));
    soc_cancun_init(unit);
    soc_trident3_latency_config(unit);
    _soc_trident3_port_mapping_init(unit);

    /*
     * Parse config at init time and cache property value for use at
     * run time
     */
    if (SOC_PBMP_NOT_NULL(PBMP_HG_ALL(unit))) {
        fabric_port_enable = 1;
    } else {
        fabric_port_enable = soc_property_get(unit, spn_FABRIC_PORT_ENABLE, 1);
    }
    SOC_CONTROL_LOCK(unit);
    si->fabric_port_enable = fabric_port_enable;
    SOC_CONTROL_UNLOCK(unit);


    if (!SAL_BOOT_XGSSIM) {
        if (soc->tdm_info == NULL) {
            soc->tdm_info = sal_alloc(sizeof(_soc_trident3_tdm_temp_t),
                                      "Trident3 TDM info");
            if (soc->tdm_info == NULL) {
                return SOC_E_MEMORY;
            }
            sal_memset(soc->tdm_info, 0, sizeof(_soc_trident3_tdm_temp_t));
        }
        _soc_trident3_tdm_init(unit);
        _soc_trident3_idb_init(unit);
    }

    /* Check the speed classes */
    SOC_IF_ERROR_RETURN(soc_td3_speed_class_validate(unit));

    sal_memset(entry, 0, sizeof(cpu_pbm_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBMm, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBMm, MEM_BLOCK_ALL, 0, entry));

    sal_memset(entry, 0, sizeof(cpu_pbm_2_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBM_2m, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBM_2m, MEM_BLOCK_ALL, 0, entry));

#if __GNUC__ > 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmemset-elt-size"
#endif
    /*
     * We are NOT doing a sal_memset(entry,0,sizeof(entry)) for
     * performance reasons, because the actual soc_mem_field_set()
     * will only write sizeof(device_loopback_ports_bitmap_entry_t)=17
     * bytes while sizeof(entry)=SOC_MAX_MEM_BYTES can be up to 651
     * depending on which devices are compiled in.
     */
    sal_memset(entry, 0, sizeof(device_loopback_ports_bitmap_entry_t));
#if __GNUC__ > 6
#pragma GCC diagnostic pop
#endif
    soc_mem_pbmp_field_set(unit, DEVICE_LOOPBACK_PORTS_BITMAPm, entry, BITMAPf,
                           &PBMP_LB(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, DEVICE_LOOPBACK_PORTS_BITMAPm, MEM_BLOCK_ALL, 0,
                       entry));

    PBMP_LB_ITER(unit, port) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MULTIPASS_LOOPBACK_BITMAPm)
            [si->port_pipe[port]];
        sal_memset(entry, 0, sizeof(multipass_loopback_bitmap_entry_t));
        SOC_PBMP_PORT_SET(pbmp, port);
        soc_mem_pbmp_field_set(unit, mem, &entry, BITMAPf, &pbmp);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));
    }

    sal_memset(entry, 0, sizeof(egr_ing_port_entry_t));
    soc_mem_field32_set(unit, EGR_ING_PORTm, entry, PORT_TYPEf, 1);
    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, port, entry));
    }
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, si->cpu_hg_index,
                       entry));
    soc_mem_field32_set(unit, EGR_ING_PORTm, entry, PORT_TYPEf, 2);
    PBMP_LB_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, port, entry));
    }

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);
        phy_port_base = PORT_BLOCK_BASE_PORT(port);

        /* Assert CLPORT soft reset */
        rval = 0;
        for (lane = 0; lane < _TD3_PORTS_PER_PBLK; lane++) {
            if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &rval,
                                  port_fields[lane], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_SOFT_RESETr(unit, port, rval));

        /* Set port mode */
        soc_trident3_port_ratio_get(unit, clport, &mode);
        rval = 0;
        soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval, XPORT0_CORE_PORT_MODEf,
                          mode_encodings[mode]);
        soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval, XPORT0_PHY_PORT_MODEf,
                          mode_encodings[mode]);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MODE_REGr(unit, port, rval));

        /* De-assert XLPORT soft reset */
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_SOFT_RESETr(unit, port, 0));

        /* Enable CLPORT */
        rval = 0;
        for (lane = 0; lane < _TD3_PORTS_PER_PBLK; lane++) {
            if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &rval,
                                  port_fields[lane], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_ENABLE_REGr(unit, port, rval));
    }

    SOC_PBMP_COUNT(si->management_pbm, pbm_count);
    /* In case of dual management port - program top misc control */
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROLr(unit,&rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROLr, &rval, ENABLE_2ND_MGMT_PORTf,
                  (pbm_count > 1)? 1 : 0);
    WRITE_TOP_MISC_CONTROLr(unit, rval);

    SOC_IF_ERROR_RETURN(READ_SECOND_MGMT_PORTr(unit,&rval));
    soc_reg_field_set(unit, SECOND_MGMT_PORTr, &rval, ENABLEf,
                  (pbm_count > 1)? 1 : 0);
    WRITE_SECOND_MGMT_PORTr(unit, rval);

    SOC_PBMP_ITER(si->management_pbm, port) {
        /* Assert XLPORT soft reset */
        rval = 0;
        if (si->port_p2l_mapping[129] != -1) { /* management port 0 */
            soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &rval, PORT0f, 1);
        }
        if (si->port_p2l_mapping[128] != -1) { /* management port 1 */
            soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &rval, PORT2f, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, rval));

        /* Set port mode to DUAL */
        rval = 0;
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, XPORT0_CORE_PORT_MODEf,
                          3);
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, XPORT0_PHY_PORT_MODEf,
                          3);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));

        /* De-assert XLPORT soft reset */
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, 0));

        /* Enable XLPORT */
        rval = 0;
        if (si->port_p2l_mapping[129] != -1) { /* management port 0 */
            soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval, PORT0f, 1);
        }
        if (si->port_p2l_mapping[128] != -1) { /* management port 0 */
            soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval, PORT2f, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_ENABLE_REGr(unit, port, rval));
    }

    SOC_IF_ERROR_RETURN(READ_MMU_GCFG_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, REFRESH_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));

    /* Enable dual hash tables */
    SOC_IF_ERROR_RETURN(soc_trident3_hash_init(unit));

    /* Configure EP credit */
    sal_memset(entry, 0, sizeof(egr_mmu_cell_credit_entry_t));
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(
            soc_td3_port_asf_speed_to_mmu_cell_credit(unit, port,
                si->port_speed_max[port], &fval));
        SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ANY,
                            si->port_l2p_mapping[port], &entry));
        soc_EGR_MMU_CELL_CREDITm_field32_set(unit, &entry, CREDITf, fval);
        SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ALL,
                            si->port_l2p_mapping[port], &entry));
    }

    /* Configure egress transmit start count */
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(
            soc_td3_port_asf_xmit_start_count_set(unit, port,
                si->port_speed_max[port], _SOC_TD3_ASF_MODE_SAF, 0));
    }

    /* Ingress Force SAF */
    SOC_IF_ERROR_RETURN(_soc_trident3_ing_fsaf_init(unit));

    /* Egress Force SAF */
    if (SOC_PBMP_NOT_NULL(si->oversub_pbm)) {
        factor = 178125;
        mem = EDB_1DBG_Bm;
        sal_memset(entry, 0, sizeof(edb_1dbg_b_entry_t));

        PBMP_ALL_ITER(unit, port) {
            fval = ((si->port_speed_max[port] / 10) * factor) / 100000;
            soc_mem_field32_set(unit, mem, &entry, FIELD_Bf, fval);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                               si->port_l2p_mapping[port], entry));
        }

        for (index = 0; index < NUM_PIPE(unit); index++) {
            if (SOC_PBMP_IS_NULL(si->pipe_pbm[index])) {
                continue;
            }
            reg = SOC_REG_UNIQUE_ACC(unit, EDB_1DBG_Ar)[index];
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));
            correction_duration = 135 * si->frequency / 10;
            detection_duration = 15 * si->frequency / 10;
            soc_reg64_field32_set(unit, EDB_1DBG_Ar, &rval64, FIELD_Af, detection_duration);
            soc_reg64_field32_set(unit, EDB_1DBG_Ar, &rval64, FIELD_Bf, correction_duration);
            soc_reg64_field32_set(unit, EDB_1DBG_Ar, &rval64, FIELD_Cf, 25);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, reg, REG_PORT_ANY, 0, rval64));
        }
    }

    /* Enable egress */
    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        phy_port = si->port_l2p_mapping[port];
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
    }

    sal_memset(entry, 0, sizeof(epc_link_bmap_entry_t));
    soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                           &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0, entry));

    /* enable all ports */
    sal_memset(&ingr_entry, 0, sizeof(ingr_entry));
    soc_mem_pbmp_field_set(unit, ING_DEST_PORT_ENABLEm, &ingr_entry,
                           PORT_BITMAPf, &PBMP_ALL(unit));
    SOC_IF_ERROR_RETURN(
        WRITE_ING_DEST_PORT_ENABLEm(unit, MEM_BLOCK_ALL, 0, &ingr_entry));

    sal_memset(&map_entry, 0, sizeof(map_entry));
    soc_mem_field32_set(unit, MODPORT_MAP_SUBPORTm, &map_entry, ENABLEf, 1);

    /* my_modid and other modid related initialization */
    PBMP_ALL_ITER(unit, port) {
        /* configure logical port numbers */
        soc_mem_field32_set(unit, MODPORT_MAP_SUBPORTm, &map_entry,
                            DESTf, port);
        SOC_IF_ERROR_RETURN(WRITE_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ALL,
                                                        port, &map_entry));
    }

    /* setting up my_modid */
    SOC_IF_ERROR_RETURN(READ_MY_MODID_SET_2_64r(unit, &rval64));

    soc_reg64_field32_set(unit, MY_MODID_SET_2_64r, &rval64,
                          MODID_0_VALIDf, 1);
    soc_reg64_field32_set(unit, MY_MODID_SET_2_64r, &rval64,
                          MODID_0f, SOC_BASE_MODID(unit));

    SOC_IF_ERROR_RETURN(WRITE_MY_MODID_SET_2_64r(unit, rval64));

    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          L3SRC_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          L2DST_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          APPLY_EGR_MASK_ON_L2f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          APPLY_EGR_MASK_ON_L3f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          ARP_RARP_TO_FPf, 0x3); /* enable both ARP & RARP */
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          ARP_VALIDATION_ENf, 1);
    if (soc_feature(unit, soc_feature_port_lag_failover)) {
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                              IGNORE_HG_HDR_LAG_FAILOVERf, 0);
    } else {
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                              IGNORE_HG_HDR_LAG_FAILOVERf, 1);
    }
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          APPLY_HG_MGID_ADJUSTf, 0xFF);

    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, rval64));



    /* The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    /* Programming EGR_VLAN_CONTROL_1m table */
#if 0
    /* Can't call bcm function in soc layer. */
    if (soc_feature(unit, soc_feature_egr_all_profile)) {
        PBMP_ALL_ITER(unit, port) {
            soc_field_t flds = {VT_MISS_UNTAGf, REMARK_OUTER_DOT1Pf};
            uint32 vals = {0, 1};
            BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_fields_set(unit,
                port, EGR_VLAN_CONTROL_1m, flds, vals);
        }
    }
#endif

    SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    SOC_PBMP_REMOVE(pbmp, PBMP_LB(unit));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_EN_EFILTER_BITMAPm,
                                     MEM_BLOCK_ANY, 0, &entry));
    soc_mem_pbmp_field_set(unit, ING_EN_EFILTER_BITMAPm, &entry, BITMAPf,
                           &pbmp);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_EN_EFILTER_BITMAPm,
                                      MEM_BLOCK_ANY, 0, &entry));

    /* Multicast range initialization */
    SOC_IF_ERROR_RETURN
        (soc_hbx_higig2_mcast_sizes_set(unit,
             soc_property_get(unit, spn_HIGIG2_MULTICAST_VLAN_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L2_RANGE,
                              soc_mem_index_count(unit, L2MCm)),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                              soc_mem_index_count(unit, L3_IPMCm))));

    soc->mcast_size = soc_property_get(unit, spn_MULTICAST_L2_RANGE,
                                        soc_mem_index_count(unit, L2MCm));
    soc->ipmc_size =  soc_property_get(unit, spn_MULTICAST_L3_RANGE,
                                        soc_mem_index_count(unit, L3_IPMCm));

    SOC_IF_ERROR_RETURN(READ_MC_CONTROL_4r(unit, &mc_ctrl));

    soc_reg_field_set(unit, MC_CONTROL_4r, &mc_ctrl,
                      ALLOW_IPMC_INDEX_WRAP_AROUNDf, 1);
    SOC_IF_ERROR_RETURN
        (WRITE_MC_CONTROL_4r(unit, mc_ctrl));

    /* Setup MDIO divider */
    soc_trident3_mdio_rate_divisor_set(unit);

    /*FIXME:
    if (soc_mspi_init(unit) != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "unit %d : MSPI Init Failed\n"), unit));
    }*/
    _phy_tsce_firmware_set_helper[unit] = soc_trident3_tscx_firmware_set;
    _phy_tscf_firmware_set_helper[unit] = soc_trident3_tscx_firmware_set;

    /* Check if Hierarchical ECMP mode is set */
    if (soc_property_get(unit, spn_L3_ECMP_LEVELS, 1) == 2) {
        uint32 ecmp_mode = 1;
        rval = 0;
        soc_reg_field_set(unit, ECMP_CONFIGr, &rval, ECMP_MODEf, ecmp_mode);
        SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIGr(unit, rval));

    } else {
        uint32 rval = 0;
        uint32 ecmp_mode = 0;
        soc_reg_field_set(unit, ECMP_CONFIGr, &rval, ECMP_MODEf, ecmp_mode);
        SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIGr(unit, rval));

    }

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROLr,
                        REG_PORT_ANY, ECMP_FLOWSET_TABLE_CONFIGf, 1));
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
        ecmp_levels = soc_property_get(unit, spn_L3_ECMP_LEVELS, 1);
        if ((soc_feature(unit, soc_feature_riot) ||
            soc_feature(unit, soc_feature_multi_level_ecmp)) &&
            ecmp_levels > 1 && soc_mem_index_count(unit,RH_ECMP_FLOWSETm)) {

            num_overlay_ecmp_rh_flowset_entries = soc_property_get(unit,
                   spn_RIOT_OVERLAY_ECMP_RESILIENT_HASH_SIZE, 0);

            if (SOC_FIELD_RANGE_CHECK(
                 num_overlay_ecmp_rh_flowset_entries, 0, 32768)) {
                 num_overlay_ecmp_rh_flowset_entries = 32768;
            }

            switch (num_overlay_ecmp_rh_flowset_entries) {
            case 0:
                break;
            case 32768:
                if (soc_mem_index_count(unit,RH_ECMP_FLOWSETm) >= 32768) {
                    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                 ENHANCED_HASHING_CONTROLr,
                                 REG_PORT_ANY, ECMP_FLOWSET_TABLE_CONFIGf, 2));
                } else {
                    LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "%s:Total ECMP entries %d are less than RH entries :%d \n"),
                    FUNCTION_NAME(),  soc_mem_index_count(unit,RH_ECMP_FLOWSETm),
                    num_overlay_ecmp_rh_flowset_entries));
                    return SOC_E_CONFIG;
               }
                break;
            default:
                LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "%s: Value for overlay RH entries is not correct : %d \n"),
                    FUNCTION_NAME(),  num_overlay_ecmp_rh_flowset_entries));
               return SOC_E_CONFIG;
            }
        }
#endif

   /* Setup SW2_FP_DST_ACTION_CONTROL */
   fields[0] = HGTRUNK_RES_ENf;
   values[0] = 1;
   fields[1] = LAG_RES_ENf;
   values[1] = 1;
   SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit,
                                        SW2_IFP_DST_ACTION_CONTROLr,
                                        REG_PORT_ANY, 2, fields, values));

   /* Populate and enable RTAG7 Macro flow offset table */
   SOC_IF_ERROR_RETURN(_soc_trident3_macro_flow_offset_init(unit));

   /* Enhanced hashing  LAG/TRUNK config
    * LAG/TRUNK share the same flow set table used in RH
    * 64k is shared between LAG and HGT for
    * - 32k each - default
    * - 64K dedicated to HGT RH
    * - 64K dedicated to LAG RH
    */
    num_lag_rh_flowset_entries = soc_property_get(unit,
                spn_TRUNK_RESILIENT_HASH_TABLE_SIZE, 32768);

    switch (num_lag_rh_flowset_entries) {
        case 0:
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROL_2r,
                        REG_PORT_ANY, HGT_LAG_FLOWSET_TABLE_CONFIGf, 1));
            break;
        case 65536:
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROL_2r,
                        REG_PORT_ANY, HGT_LAG_FLOWSET_TABLE_CONFIGf, 0));
            break;
        case 32768:
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROL_2r,
                        REG_PORT_ANY, HGT_LAG_FLOWSET_TABLE_CONFIGf, 2));
            break;
        default:
            return SOC_E_CONFIG;
    }
    SOC_IF_ERROR_RETURN
        (soc_counter_tomahawk_status_enable(unit, TRUE));

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MISC Init completed (u=%d)\n"), unit));

    /* Mem Scan thread start should be the last step in Misc init */
    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));
        SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_start(unit));
    }

    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}

/*
 * Function used for index calculation of certain memories,
 * where index is derived from more than one key.
 * Eg. (Port, SP) or (Port, PG)
 * Defined as Macro: SOC_TD3_MMU_PIPED_MEM_INDEX
 * Note: If no special indexig required, return the same index.
 */
uint32 _soc_td3_piped_mem_index(int unit, soc_port_t port,
                               soc_mem_t mem, int arr_off)
{
    int     mmu_port, index;

    mmu_port = SOC_TD3_MMU_PORT(unit, port);

    switch (mem) {
        case MMU_THDM_DB_PORTSP_CONFIGm:
        case MMU_THDM_MCQE_PORTSP_CONFIGm:
            if (NUM_PIPE(unit) == 1) {
                index = ((mmu_port & SOC_TD3_MMU_PORT_STRIDE) * _TD3_MMU_NUM_POOL)
                        + arr_off;
            } else {
                index = (_TD3_PORTS_PER_PIPE * arr_off) + (mmu_port &
                        SOC_TD3_MMU_PORT_STRIDE);
            }
            break;
        case THDI_PORT_SP_CONFIGm:
        case MMU_THDU_CONFIG_PORTm:
        case MMU_THDU_RESUME_PORTm:
        case MMU_THDU_COUNTER_PORTm:
            index = ((mmu_port & SOC_TD3_MMU_PORT_STRIDE) * _TD3_MMU_NUM_POOL)
                     + arr_off;
            break;
        case THDI_PORT_PG_CONFIGm:
            index = ((mmu_port & SOC_TD3_MMU_PORT_STRIDE) * _TD3_MMU_NUM_PG)
                     + arr_off;
            break;
        default:
            return arr_off;
    }

    return index;
}

/*
 * Function:
 *      soc_td3_mmu_bmp_reg_pos_get
 * Purpose:
 *      Helper function for register resolve.
 *      Can be used for registers that include only bitmap field.
 *      Each bit in the bitmap is MMU port.
 *      2 splited registers are used to accomodate 66 ports for each pipe.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 *      pipe    - (OUT) Pipe number.
 *      split   - (OUT) Reg split number.
 *      pos     - (OUT) Position in bitmap.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_td3_mmu_bmp_reg_pos_get(int unit, soc_port_t port,
                            int *pipe, int *split, int *pos)
{
    int pipe_tmp, mmu_port, phy_port;
    int split_tmp, pos_tmp;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    pipe_tmp = si->port_pipe[port];
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];
    /* Reg config is per pipe, get local MMU port. */
    mmu_port = mmu_port & SOC_TD3_MMU_PORT_STRIDE;

    if (mmu_port < 64) {
        split_tmp = 0;
        pos_tmp = mmu_port;
    } else {
        split_tmp = 1;
        pos_tmp = mmu_port - 64;
    }

    if (pipe != NULL) {
        *pipe = pipe_tmp;
    }
    if (split != NULL) {
        *split = split_tmp;
    }
    if (pos != NULL) {
        *pos = pos_tmp;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_td3_thdo_hw_get(int unit, soc_port_t port, int *enable)
{
    uint64 rval64;
    int pipe, i;
    int split, pos;
    soc_reg_t reg[3][2] = {
        {
            THDU_OUTPUT_PORT_RX_ENABLE_SPLIT0r,
            THDU_OUTPUT_PORT_RX_ENABLE_SPLIT1r
        },
        {
            MMU_THDM_DB_PORT_RX_ENABLE_64_SPLIT0r,
            MMU_THDM_DB_PORT_RX_ENABLE_64_SPLIT1r
        },
        {
            MMU_THDM_MCQE_PORT_RX_ENABLE_64_SPLIT0r,
            MMU_THDM_MCQE_PORT_RX_ENABLE_64_SPLIT1r
        }
    };

    SOC_IF_ERROR_RETURN(
        soc_td3_mmu_bmp_reg_pos_get(unit, port, &pipe, &split, &pos));

    COMPILER_64_ZERO(rval64);
    *enable = TRUE;
    for (i = 0; i < 3; i++) {
        SOC_IF_ERROR_RETURN
            (soc_trident3_xpe_reg_get(unit, reg[i][split],
                                      -1, pipe, 0, &rval64));

        if (!COMPILER_64_BITTEST(rval64, pos)) {
            *enable = FALSE;
            break;
        }
    }
#ifdef INCLUDE_AVS
    /* Register trident3 related functions */
    SOC_IF_ERROR_RETURN(soc_td3_avs_init(unit));
#endif /* INCLUDE_AVS  */
    return SOC_E_NONE;
}

STATIC int
_soc_td3_thdo_hw_set(int unit, soc_port_t port, int enable)
{
    uint64 rval64, rval64_tmp;
    int pipe, i;
    int split, pos;
    soc_reg_t reg[3][2] = {
        {
            THDU_OUTPUT_PORT_RX_ENABLE_SPLIT0r,
            THDU_OUTPUT_PORT_RX_ENABLE_SPLIT1r
        },
        {
            MMU_THDM_DB_PORT_RX_ENABLE_64_SPLIT0r,
            MMU_THDM_DB_PORT_RX_ENABLE_64_SPLIT1r
        },
        {
            MMU_THDM_MCQE_PORT_RX_ENABLE_64_SPLIT0r,
            MMU_THDM_MCQE_PORT_RX_ENABLE_64_SPLIT1r
        }
    };

    SOC_IF_ERROR_RETURN(
        soc_td3_mmu_bmp_reg_pos_get(unit, port, &pipe, &split, &pos));

    for (i = 0; i < 3; i++) {
        COMPILER_64_ZERO(rval64);
        COMPILER_64_ZERO(rval64_tmp);

        if (pos < 32) {
            COMPILER_64_SET(rval64_tmp, 0, (1 << pos));
        } else {
            COMPILER_64_SET(rval64_tmp, (1 << (pos - 32)), 0);
        }

        SOC_IF_ERROR_RETURN
            (soc_trident3_xpe_reg_get(unit, reg[i][split],
                                      -1, pipe, 0, &rval64));

        if (enable) {
            COMPILER_64_OR(rval64, rval64_tmp);
        } else {
            COMPILER_64_NOT(rval64_tmp);
            COMPILER_64_AND(rval64, rval64_tmp);
        }

        SOC_IF_ERROR_RETURN
            (soc_trident3_xpe_reg_set(unit, reg[i][split],
                                      -1, pipe, 0, rval64));
    }

    return SOC_E_NONE;
}

/*
 * Funtion to caltulate global buffer reserve for ASF.
 */
STATIC int
_soc_td3_mmu_config_buf_asf(int unit, int lossless, int *cnt)
{
    soc_info_t *si;
    int pm, oversub, num_ports, ports[3], asf = 0;
    static int asf_profile[SOC_MAX_NUM_DEVICES] = {-2};
    int asf_reserved_cells[3][3][2] =     /* 1/2/4 port, asf profile, line/oversub*/
        {
            {{ 0,  0}, {16,  31}, { 39, 50}},     /* 1-port PM */
            {{ 0,  0}, {32,  62}, { 78, 100}},     /* 2-ports PM */
            {{ 0,  0}, {64, 124}, {156, 200}},    /* 3/4-ports PM */
        };

    /* To init static array to -1 */
    if (asf_profile[0] == -2) {
        sal_memset(asf_profile, -1, sizeof(asf_profile));
    }

    if (asf_profile[unit] == -1) {
        asf_profile[unit] = soc_property_get(unit, spn_ASF_MEM_PROFILE,
            _SOC_TD3_ASF_MEM_PROFILE_SIMILAR);
        if ((asf_profile[unit] > _SOC_TD3_ASF_MEM_PROFILE_EXTREME)
            || (asf_profile[unit] < _SOC_TD3_ASF_MEM_PROFILE_NONE)) {
            asf_profile[unit] = _SOC_TD3_ASF_MEM_PROFILE_NONE;
        }
    }

    si = &SOC_INFO(unit);
    oversub = SOC_PBMP_IS_NULL(si->oversub_pbm)? 0 : 1;

    ports[0] = ports[1] = ports[2] = 0;
    for (pm = 0; pm < _TD3_PBLKS_PER_DEV(unit); pm++) {
        num_ports = soc_td3_ports_per_pm_get(unit, pm);
        if (num_ports == 1) {
            ports[0]++;
        } else if (num_ports == 2) {
            ports[1]++;
        } else if ((num_ports == 4) || (num_ports == 3)) {
            ports[2]++;
        }
    }

    asf = asf_reserved_cells[0][asf_profile[unit]][oversub] * ports[0]
        + asf_reserved_cells[1][asf_profile[unit]][oversub] * ports[1]
        + asf_reserved_cells[2][asf_profile[unit]][oversub] * ports[2];

    if (cnt != NULL) {
        *cnt = asf;
    }

    return SOC_E_NONE;
}

/*
 * Function used for global resource reservation (MCQE / RQE / Buf cell).
 * Input state: Total hardware resource.
 * Output state: Resource available for admission control.
 */
STATIC void
_soc_td3_mmu_config_dev_reserve(int unit, _soc_mmu_device_info_t *devcfg,
                                int lossless)
{
    int port, pm, asf_rsvd = 0;
    uint32 total_mcq = 0, num_ports = 0;
    uint32 cpu_cnt = 1, lb_cnt = 2, mgmt_cnt = 1;
    soc_info_t *si = &SOC_INFO(unit);

    /* Device reservation for RQE Entry */
    devcfg->total_rqe_queue_entry -= _TD3_MMU_RQE_ENTRY_RSVD_PER_XPE;

    /* Device reservation for MCQ Entry - 6 entries for each MC queue */
    if (si->flex_eligible) {
        for (pm = 0; pm < _TD3_PBLKS_PER_DEV(unit); pm++) {
            num_ports += soc_td3_ports_per_pm_get(unit, pm);
        }
        total_mcq += (num_ports * SOC_TD3_NUM_MCAST_QUEUE_PER_PORT);
    } else {
        PBMP_ALL_ITER(unit, port) {
            if (IS_CPU_PORT(unit, port) ||
                IS_LB_PORT(unit,port) ||
                IS_MGMT_PORT(unit,port)) {
                continue;
            }
            total_mcq += si->port_num_cosq[port];
        }
    }
    total_mcq += ((cpu_cnt * SOC_TD3_NUM_CPU_QUEUES) +
                  (lb_cnt + mgmt_cnt) * SOC_TD3_NUM_MCAST_QUEUE_PER_PORT);

    devcfg->total_mcq_entry -= ((total_mcq * _TD3_MCQE_RSVD_PER_MCQ) +
                                _TD3_TDM_MCQE_RSVD_OVERSHOOT);

    /* Device reservation for buffer cell */
    (void)_soc_td3_mmu_config_buf_asf(unit, lossless, &asf_rsvd);
    devcfg->mmu_total_cell -= asf_rsvd;
}

STATIC void
_soc_td3_mmu_init_dev_config(int unit, _soc_mmu_device_info_t *devcfg,
                             int lossless)
{
    if (devcfg == NULL) {
        LOG_FATAL(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                            "MMU config data error\
                            (u=%d)\n"), unit));
        return;
    }
    sal_memset(devcfg, 0, sizeof(_soc_mmu_device_info_t));

    devcfg->max_pkt_byte = _TD3_MMU_MAX_PACKET_BYTES;
    devcfg->mmu_hdr_byte = _TD3_MMU_PACKET_HEADER_BYTES;
    devcfg->jumbo_pkt_size = _TD3_MMU_JUMBO_FRAME_BYTES;
    devcfg->default_mtu_size = _TD3_MMU_DEFAULT_MTU_BYTES;
    devcfg->mmu_cell_size = _TD3_MMU_BYTES_PER_CELL;
    devcfg->mmu_total_cell = _TD3_MMU_TOTAL_CELLS_PER_XPE;
    devcfg->num_pg = _TD3_MMU_NUM_PG;
    devcfg->num_service_pool = _TD3_MMU_NUM_POOL;
    devcfg->flags = SOC_MMU_CFG_F_PORT_MIN | SOC_MMU_CFG_F_PORT_POOL_MIN |
                    SOC_MMU_CFG_F_RQE | SOC_MMU_CFG_F_EGR_MCQ_ENTRY;
    devcfg->total_mcq_entry = _TD3_MMU_TOTAL_MCQ_ENTRY(unit);
    devcfg->rqe_queue_num = 11;
    devcfg->total_rqe_queue_entry = _TD3_MMU_TOTAL_RQE_ENTRY(unit);

    _soc_td3_mmu_config_dev_reserve(unit, devcfg, lossless);
}

STATIC void
_soc_td3_mmu_sw_info_config (int unit, _soc_td3_mmu_sw_config_info_t * swcfg,
                             int lossless)
{
    /* Default settings is lossless */
    swcfg->mmu_egr_queue_min = 0;
    swcfg->mmu_egr_qgrp_min = 0;
    swcfg->mmu_total_pri_groups = 8;
    swcfg->mmu_active_pri_groups = 1;
    swcfg->mmu_pg_min = 8;
    swcfg->mmu_port_min = 0;
    swcfg->mmu_mc_egr_qentry_min = 0;
    swcfg->mmu_rqe_qentry_min = 8;
    swcfg->mmu_rqe_queue_min = 8;
    if (lossless == 0)
    {
        swcfg->mmu_egr_queue_min = 8;
        swcfg->mmu_egr_qgrp_min = 16;
        swcfg->mmu_pg_min = 0;
    }
}

STATIC int
_soc_td3_default_pg_headroom(int unit, soc_port_t port,
                            int lossless)
{
    int speed = 1000, hdrm = 0;
    uint8 port_oversub = 0;

    if (IS_CPU_PORT(unit, port)) {
        return 77;
    } else if (!lossless) {
        return 0;
    } else if (IS_LB_PORT(unit, port)) {
        return 150;
    }

    speed = SOC_INFO(unit).port_speed_max[port];

    if (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port)) {
        port_oversub = 1;
    }
    if ((speed >= 1000) && (speed <= 10000)) {
        hdrm = port_oversub ? 194 : 160;
    } else if ((speed > 10000) && (speed <= 20000)) {
        hdrm = port_oversub ? 231 : 197;
    } else if ((speed > 20000) && (speed <= 25000)) {
        hdrm = port_oversub ? 288 : 254;
    } else if ((speed > 25000) && (speed <= 42000)) {
        hdrm = port_oversub ? 355 : 274;
    } else if ((speed > 42000) && (speed <= 50000)) {
        hdrm = port_oversub ? 432 : 351;
    } else if (speed >= 100000) {
        hdrm = port_oversub ? 768 : 574;
    } else {
        hdrm = port_oversub ? 194 : 160;
    }
    return hdrm;
}

/*
 * Default phase_1 MMU settings about headroom, guarantee, dynamic, color, ...
 * which depend on single port/queue/qgroup/pg...
 * Input state: Available for admission in devcfg, configured swcfg.
 * Output state: Independent data filled in buf.
 * Version 1.1
 */
STATIC void
_soc_td3_mmu_config_buf_phase1(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg,
                               _soc_td3_mmu_sw_config_info_t *swcfg,
                               int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    int max_packet_cells, default_mtu_cells;
    int port, idx;
    int total_pool_size = 0, asf_rsvd = 0;
    int mcq_entry_shared_total;
    int rqe_entry_shared_total, qmin;

    si = &SOC_INFO(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Initializing default MMU config phase 1(u=%d)\n"), unit));
    max_packet_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->max_pkt_byte +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

    (void)_soc_td3_mmu_config_buf_asf(unit, lossless, &asf_rsvd);
    total_pool_size = devcfg->mmu_total_cell + asf_rsvd; /* Add back ASF reserved. */

    buf->headroom = 2 * max_packet_cells;   /* 1 packet per pipe. */

    mcq_entry_shared_total = devcfg->total_mcq_entry;

    rqe_entry_shared_total = devcfg->total_rqe_queue_entry -
        (_TD3_MMU_NUM_RQE_QUEUES * swcfg->mmu_rqe_qentry_min);

    for (idx = 0; idx < _TD3_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if (idx == 0) {  /* 100% scale up by 100 */
            buf_pool->size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->yellow_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->red_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->total_mcq_entry = mcq_entry_shared_total;
            buf_pool->total_rqe_entry = rqe_entry_shared_total;
        } else {
            buf_pool->size = 0;
            buf_pool->yellow_size = 0;
            buf_pool->red_size = 0;
            buf_pool->total_mcq_entry = 0;
            buf_pool->total_rqe_entry = 0;
        }
    }

    for (idx = 0; idx < SOC_TD3_MMU_CFG_QGROUP_MAX; idx++) {
        queue_grp = &buf->qgroups[idx];
        /* resume limits - accounted for 8 cell granularity */
        queue_grp->pool_resume = 16;
        queue_grp->yellow_resume = 16;
        queue_grp->red_resume = 16;
        queue_grp->guarantee = swcfg->mmu_egr_qgrp_min;
        if (lossless) {
            queue_grp->discard_enable = 0;
        } else {
            queue_grp->discard_enable = 1;
        }
    }

    PBMP_ALL_ITER(unit, port) {
        if (port >= SOC_MMU_CFG_PORT_MAX) {
            break;
        }
        buf_port = &buf->ports[port];

        /* internal priority to priority group mapping */
        for (idx = 0; idx < _TD3_MMU_NUM_INT_PRI; idx++) {
            buf_port->pri_to_prigroup[idx] = 7;
        }

        /* priority group to pool mapping */
        for (idx = 0; idx < _TD3_MMU_NUM_PG; idx++) {
            buf_port->prigroups[idx].pool_idx = 0;
        }

        for (idx = 0; idx < _TD3_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            buf_port_pool->guarantee = 0;
            /* Port based accouting is always enabled
             * Set it to max for all service pools so that
             * it does not trigger drops by default */
            buf_port_pool->pool_limit = total_pool_size;
            buf_port_pool->pool_resume = total_pool_size -
                                         (default_mtu_cells * 2);
        }

        buf_port->pkt_size = max_packet_cells;

        /* priority group */
        for (idx = 0; idx < _TD3_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            buf_prigroup->guarantee = 0;
            buf_prigroup->user_delay = -1;
            buf_prigroup->switch_delay = -1;
            buf_prigroup->pkt_size = max_packet_cells;
            buf_prigroup->device_headroom_enable = 0;
            buf_prigroup->pool_floor = 0;
            buf_prigroup->headroom = 0;
            buf_prigroup->pool_resume = 0;
            buf_prigroup->flow_control_enable = 0;
            if (idx == 7) {
                buf_prigroup->device_headroom_enable = 1;
                buf_prigroup->flow_control_enable = lossless;
                buf_prigroup->headroom =
                    _soc_td3_default_pg_headroom(unit, port, lossless);
                if (lossless) {
                    buf_prigroup->guarantee = swcfg->mmu_pg_min;
                }
            }
        }

        /* multicast queue */
        if (IS_CPU_PORT(unit, port) ||
            IS_LB_PORT(unit,port)) {
            qmin = swcfg->mmu_egr_queue_min;
        } else {
            qmin = 0;
        }
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];
            buf_queue->qgroup_id = -1;
            buf_queue->guarantee = qmin;
            buf_queue->mcq_entry_guarantee = swcfg->mmu_mc_egr_qentry_min;
            buf_queue->color_discard_enable = 0;
            if (lossless) {
                buf_queue->discard_enable = 0;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
            } else {
                buf_queue->discard_enable = 1;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
            }
        }

        /* unicast queue */
        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];
            buf_queue->qgroup_id = -1;
            buf_queue->guarantee = 0;
            buf_queue->color_discard_enable = 0;
            if (lossless) {
                buf_queue->discard_enable = 0;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;
            } else {
                buf_queue->discard_enable = 1;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;
                buf_queue->qgroup_id = 0;
                buf_queue->qgroup_min_enable = 1;
            }
        }

        /* queue to pool mapping */
        for (idx = 0;
             idx < si->port_num_cosq[port] + si->port_num_uc_cosq[port]; idx++) {
            buf_port->queues[idx].pool_idx = 0;
        }
    }

    /* RQE */
    for (idx = 0; idx < _TD3_MMU_NUM_RQE_QUEUES; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];
        buf_rqe_queue->pool_idx = 0;
        buf_rqe_queue->guarantee = swcfg->mmu_rqe_queue_min;
        if (lossless) {
            buf_rqe_queue->discard_enable = 0;
        } else {
            buf_rqe_queue->discard_enable = 1;
        }
    }
}

/*
 * Default phase_2 MMU settings about shared buffer limit, which depends on
 * guaranteed/headroom buffers on all port/queue/qgroup/pg/...
 * Input state: _soc_mmu_cfg_buf_calculate processed buf.
 * Output state: All necessary data filled in buf.
 * Version 1.1
 */
STATIC void
_soc_td3_mmu_config_buf_phase2(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg,
                               _soc_td3_mmu_sw_config_info_t *swcfg,
                               int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    int port, idx, pm, ports[3] = {0};
    int pg_hdrm_ob[] = {699, 379, 224}; /* PG headroom of 1/2/4-port PortMacro */
    int total_pool_size = 0;
    int ing_rsvd_total = 0, egr_rsvd_total = 0;
    int ing_shared_total, egr_shared_total;
    int total_rsvd_qmin = 0;
    uint32 color_limit = 0;
    uint32 num_ports = 0, mgmt_cnt = 0, cpu_cnt = 0, lb_cnt = 0;
    uint32 cpu_hdrm = 0, lb_hdrm = 0, mgmt_hdrm = 0;

    si = &SOC_INFO(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Initializing default MMU config phase 2(u=%d)\n"), unit));

    buf_pool = &buf->pools[0];
    total_pool_size = devcfg->mmu_total_cell; /* per XPE limit */
    total_rsvd_qmin = swcfg->mmu_egr_queue_min * (SOC_TD3_NUM_CPU_QUEUES +
                        (SOC_TD3_NUM_MCAST_QUEUE_PER_PORT * 2));

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            cpu_cnt++;
            cpu_hdrm = _soc_td3_default_pg_headroom(unit, port, lossless);
        } else if (IS_LB_PORT(unit,port)) {
            lb_cnt++;
            lb_hdrm = _soc_td3_default_pg_headroom(unit, port, lossless);
        } else if (IS_MGMT_PORT(unit,port)) {
            mgmt_cnt++;
            mgmt_hdrm = _soc_td3_default_pg_headroom(unit, port, lossless);
        } else {
            num_ports++;
        }
    }
    if (si->flex_eligible) {
        num_ports = 0;
        for (pm = 0; pm < _TD3_PBLKS_PER_DEV(unit); pm++) {
            port = soc_td3_ports_per_pm_get(unit, pm);
            if (port == 1) {
                ports[0]++;
            } else if (port == 2) {
                ports[1]++;
            } else if ((port == 4) || (port == 3)) {
                ports[2]++;
                port = 4;
            }
            num_ports += port;
        }

        if (lossless) {
            ing_rsvd_total += (buf->headroom +
                swcfg->mmu_active_pri_groups*(
                ports[0]*pg_hdrm_ob[0] + 2*ports[1]*pg_hdrm_ob[1] +
                4*ports[2]*pg_hdrm_ob[2] + cpu_cnt*cpu_hdrm + lb_cnt*lb_hdrm) +
                swcfg->mmu_pg_min*swcfg->mmu_active_pri_groups*(
                num_ports + cpu_cnt + lb_cnt + mgmt_cnt) +
                swcfg->mmu_port_min*swcfg->mmu_active_pri_groups*(
                num_ports + cpu_cnt + lb_cnt + mgmt_cnt));
            /* Management port is not included during Total PG
             * headroom calculation */
            if (buf_pool->prigroup_headroom >= mgmt_hdrm) {
                buf_pool->prigroup_headroom -= mgmt_hdrm;
            }
        } else {
            ing_rsvd_total += buf->headroom + cpu_hdrm;
        }
    } else {
        if (lossless && buf_pool->prigroup_headroom >= mgmt_hdrm) {
            /* Management port is not included during Total PG
             * headroom calculation */
            buf_pool->prigroup_headroom -= mgmt_hdrm;
        }
        ing_rsvd_total += (buf->headroom + buf_pool->prigroup_headroom +
                           buf_pool->prigroup_guarantee);
    }

    egr_rsvd_total += total_rsvd_qmin +
                       ((num_ports + mgmt_cnt) * swcfg->mmu_egr_qgrp_min) +
                       (swcfg->mmu_rqe_queue_min * _TD3_MMU_NUM_RQE_QUEUES);

    if (lossless) {
        ing_rsvd_total += egr_rsvd_total;
    }

    ing_shared_total = total_pool_size - ing_rsvd_total;
    egr_shared_total = total_pool_size - egr_rsvd_total;

    swcfg->ing_shared_total = ing_shared_total;
    swcfg->egr_shared_total = egr_shared_total;

    LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "MMU config: Total Shared size: ingress %d egress %d\n"),
                            ing_shared_total, egr_shared_total));

    color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;

    for (idx = 0; idx < SOC_TD3_MMU_CFG_QGROUP_MAX; idx++) {
        queue_grp = &buf->qgroups[idx];
        if (lossless) {
            queue_grp->pool_scale = -1;
            queue_grp->pool_limit = egr_shared_total;
            queue_grp->red_limit = egr_shared_total;
            queue_grp->yellow_limit = egr_shared_total;
        } else {
            queue_grp->pool_scale = 8;
            queue_grp->pool_limit = 0;
            queue_grp->red_limit = color_limit;
            queue_grp->yellow_limit = color_limit;
        }
    }

    PBMP_ALL_ITER(unit, port) {
        if (port >= SOC_MMU_CFG_PORT_MAX) {
            break;
        }
        buf_port = &buf->ports[port];

        /* priority group */
        for (idx = 0; idx < _TD3_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            buf_prigroup->pool_limit = 0;
            buf_prigroup->pool_scale = -1;
            if (idx == 7) {
                if (lossless) {
                    buf_prigroup->pool_scale = 8;
                } else {
                    buf_prigroup->pool_limit = ing_shared_total;
                }
            }
        }

        /* multicast queue */
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];
            if (lossless) {
                buf_queue->pool_scale = -1;
                buf_queue->pool_limit = egr_shared_total;
                buf_queue->yellow_limit = egr_shared_total;
                buf_queue->red_limit = egr_shared_total;
            } else {
                buf_queue->pool_scale = 8;
                buf_queue->pool_limit = 0;
                buf_queue->yellow_limit = color_limit
                                          | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
                buf_queue->red_limit = color_limit
                                       | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
            }
        }

        /* unicast queue */
        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];
            if (lossless) {
                buf_queue->pool_scale = -1;
                buf_queue->pool_limit = egr_shared_total;
                buf_queue->red_limit = egr_shared_total;
                buf_queue->yellow_limit = egr_shared_total;
            } else {
                buf_queue->pool_scale = 8;
                buf_queue->pool_limit = 0;
                buf_queue->red_limit = color_limit
                                       | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
                buf_queue->yellow_limit = color_limit
                                          | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
            }
        }
    }

    /* RQE */
    for (idx = 0; idx < _TD3_MMU_NUM_RQE_QUEUES; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];
        if (lossless) {
            buf_rqe_queue->pool_scale = -1;
            buf_rqe_queue->pool_limit = egr_shared_total;
            buf_rqe_queue->red_limit = egr_shared_total;
            buf_rqe_queue->yellow_limit = egr_shared_total;
        } else {
            buf_rqe_queue->pool_scale = 8;
            buf_rqe_queue->pool_limit = 0;
            buf_rqe_queue->red_limit = color_limit;
            buf_rqe_queue->yellow_limit = color_limit;
        }
    }
}

STATIC int
_soc_td3_pool_scale_to_limit(int size, int scale)
{
    int factor = 1000;

    switch (scale) {
        case 7: factor = 875; break;
        case 6: factor = 750; break;
        case 5: factor = 625; break;
        case 4: factor = 500; break;
        case 3: factor = 375; break;
        case 2: factor = 250; break;
        case 1: factor = 125; break;
        case 0:
        default:
            factor = 1000; break;
    }
    return (size * factor)/1000;
}

STATIC int
_soc_td3_mmu_config_buf_set_hw_port(int unit, int port, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless,
                               _soc_td3_mmu_sw_config_info_t *swcfg)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    thdi_port_sp_config_entry_t thdi_sp_config;
    thdi_port_pg_config_entry_t pg_config_mem;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    uint32 entry0[SOC_MAX_MEM_WORDS], entry1[SOC_MAX_MEM_WORDS];
    soc_reg_t reg = INVALIDr;
    soc_mem_t mem0, mem1, mem2, mem3;
    uint32 fval, rval;
    int base, numq;
    int idx, midx, pri, index1;
    int default_mtu_cells, limit, rlimit;
    int pipe;

    static const soc_mem_t mmu_thdi_port_mem[] = {
        THDI_PORT_SP_CONFIGm,
        THDI_PORT_PG_CONFIGm
    };
    static const soc_mem_t mmu_thdo_q_uc_mem[] = {
        MMU_THDU_CONFIG_QUEUEm,
        MMU_THDU_OFFSET_QUEUEm,
        MMU_THDU_Q_TO_QGRP_MAPm
    };
    static const soc_mem_t mmu_thdo_port_uc_mem[] = {
        MMU_THDU_CONFIG_PORTm,
        MMU_THDU_RESUME_PORTm
    };
    static const soc_mem_t mmu_thdo_q_mc_mem[] = {
        MMU_THDM_DB_QUEUE_CONFIGm,
        MMU_THDM_DB_QUEUE_OFFSETm,
        MMU_THDM_MCQE_QUEUE_CONFIGm,
        MMU_THDM_MCQE_QUEUE_OFFSETm
    };

    static const soc_mem_t mmu_thdo_port_mc_mem[] = {
        MMU_THDM_DB_PORTSP_CONFIGm,
        MMU_THDM_MCQE_PORTSP_CONFIGm
    };

    static const soc_field_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };
    static const soc_field_t prigroup_hpid_field[] = {
        PG0_HPIDf, PG1_HPIDf, PG2_HPIDf, PG3_HPIDf,
        PG4_HPIDf, PG5_HPIDf, PG6_HPIDf, PG7_HPIDf
    };

    si = &SOC_INFO(unit);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

    /* internal priority to priority group mapping */
    buf_port = &buf->ports[port];

    for (idx = 0; idx < COUNTOF(prigroup_field); idx++) {
        if (idx % 8 == 0) { /* 8 fields per register */
            reg = prigroup_reg[idx / 8];
            rval = 0;
        }
        soc_reg_field_set(unit, reg, &rval, prigroup_field[idx],
                buf_port->pri_to_prigroup[idx]);
        if (idx % 8 == 7) { /* 8 fields per register */
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
        }
    }

    /* Input port per port settings */
    pipe = si->port_pipe[port];
    buf_port = &buf->ports[port];

    rval = 0;
    for (idx = 0; idx < _TD3_MMU_NUM_PG; idx++) {
        soc_reg_field_set(unit, THDI_PORT_PG_SPIDr, &rval,
                          prigroup_spid_field[idx],
                          buf_port->prigroups[idx].pool_idx);
    }
    SOC_IF_ERROR_RETURN(WRITE_THDI_PORT_PG_SPIDr(unit, port, rval));

    
    rval = 0;
    for (idx = 0; idx < _TD3_MMU_NUM_PG; idx++) {
        soc_reg_field_set(unit, THDI_HDRM_PORT_PG_HPIDr, &rval,
                          prigroup_hpid_field[idx],
                          buf_port->prigroups[idx].pool_idx);
    }
    SOC_IF_ERROR_RETURN(WRITE_THDI_HDRM_PORT_PG_HPIDr(unit, port, rval));

    /* Per port per pool settings */
    mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdi_port_mem[0])[pipe];
    if (mem0 != INVALIDm) {
        for (idx = 0; idx < _TD3_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            midx = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, port, mmu_thdi_port_mem[0],
                                               idx);
            sal_memset(&thdi_sp_config, 0, sizeof(thdi_sp_config));
            soc_mem_field32_set(unit, mem0, &thdi_sp_config,
                                PORT_SP_MIN_LIMITf, buf_port_pool->guarantee);
            soc_mem_field32_set(unit, mem0, &thdi_sp_config,
                            PORT_SP_RESUME_LIMITf, buf_port_pool->pool_resume);
            soc_mem_field32_set(unit, mem0, &thdi_sp_config,
                                PORT_SP_MAX_LIMITf, buf_port_pool->pool_limit);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL,
                               midx, &thdi_sp_config));
        }
    }

    fval = 0;
    for (idx = 0; idx < _TD3_MMU_NUM_PG; idx++) {
        if (buf_port->prigroups[idx].flow_control_enable != 0) {
            for (pri=0; pri < 16; pri++) {
                if (buf_port->pri_to_prigroup[pri] == idx) {
                    fval |= 1 << pri;
                }
            }
        }
    }

    rval = 0;
    soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                      INPUT_PORT_RX_ENABLEf, 1);
    if (port == CMIC_PORT (unit)) {
        fval = 0;
    }
    soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                      PORT_PRI_XON_ENABLEf, fval);
    soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                      PORT_PAUSE_ENABLEf, fval ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_THDI_INPUT_PORT_XON_ENABLESr(unit,
                        port, rval));

    rval = 0;
    soc_reg_field_set(unit, THDI_PORT_MAX_PKT_SIZEr, &rval,
                      PORT_MAX_PKT_SIZEf, buf_port->pkt_size);
    SOC_IF_ERROR_RETURN(WRITE_THDI_PORT_MAX_PKT_SIZEr(unit, rval));

    /* Input port per port per priority group settings */
    mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdi_port_mem[1])[pipe];
    if (mem1 != INVALIDm) {
        for (idx = 0; idx < _TD3_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf->ports[port].prigroups[idx];

            midx = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, port, mmu_thdi_port_mem[1],
                                               idx);
            sal_memset(&pg_config_mem, 0, sizeof(pg_config_mem));
            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_MIN_LIMITf, buf_prigroup->guarantee);

            if (buf_prigroup->pool_scale != -1) {
                soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                    PG_SHARED_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                    PG_SHARED_LIMITf,
                                    buf_prigroup->pool_scale);
            } else {
                soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                    PG_SHARED_LIMITf,
                                    buf_prigroup->pool_limit);
            }

            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_GBL_HDRM_ENf,
                                buf_prigroup->device_headroom_enable);
            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_HDRM_LIMITf, buf_prigroup->headroom);

            soc_mem_field32_set(unit, mem1, &pg_config_mem, PG_RESET_OFFSETf,
                                0);

            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_RESET_FLOORf, buf_prigroup->pool_floor);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL, midx,
                                &pg_config_mem));
        }
    }

    /***********************************
     * THDO
 */
    /* Output port per port per queue setting for regular multicast queue */
    pipe = si->port_pipe[port];
    numq = si->port_num_cosq[port];
    base = si->port_cosq_base[port];

    mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[0])[pipe];
    mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[1])[pipe];
    if ((numq != 0) && (mem0 != INVALIDm) && (mem1 != INVALIDm)) {
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[idx];

            sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_config_0_entry_t));

            soc_mem_field32_set(unit, mem0, entry0, Q_MIN_LIMITf,
                                buf_queue->guarantee);
            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem0, entry0, Q_COLOR_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem0, entry0, Q_SHARED_ALPHAf,
                                    buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0, Q_SHARED_LIMITf,
                                    buf_queue->pool_limit);
            }
            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                    (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem0, entry0, Q_COLOR_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem0, entry0, YELLOW_SHARED_LIMITf,
                        buf_queue->yellow_limit
                                 & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                                 & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
                soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                        buf_queue->red_limit
                                 & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                                 & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0, YELLOW_SHARED_LIMITf,
                                    sal_ceil_func(buf_queue->yellow_limit, 8));
                soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                                    sal_ceil_func(buf_queue->red_limit, 8));
            }

            soc_mem_field32_set(unit, mem0, entry0, Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, base + idx, entry0));

            sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_offset_entry_t));

            soc_mem_field32_set(unit, mem1, entry0, RESUME_OFFSETf,
                                (default_mtu_cells * 2)/8);
            soc_mem_field32_set(unit, mem1, entry0,
                                YELLOW_RESUME_OFFSET_PROFILE_SELf, 0);
            soc_mem_field32_set(unit, mem1, entry0,
                                RED_RESUME_OFFSET_PROFILE_SELf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, base + idx, entry0));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                    WRITE_MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_YELLOWr(unit, 0,
                        2));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                    WRITE_MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_REDr(unit, 0,
                        2));
        }
    }

    mem2 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[2])[pipe];
    mem3 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[3])[pipe];
    if ((numq != 0) && (mem2 != INVALIDm) && (mem3 != INVALIDm)) {
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[idx];
            buf_pool = &buf->pools[buf_queue->pool_idx];

            sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_queue_config_entry_t));

            soc_mem_field32_set(unit, mem2, entry0, Q_MIN_LIMITf,
                                sal_ceil_func(buf_queue->mcq_entry_guarantee, 4));
            limit = buf_pool->total_mcq_entry - buf_pool->mcq_entry_reserved;

            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0, Q_COLOR_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem2, entry0, Q_SHARED_ALPHAf,
                                    buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
                    soc_mem_field32_set(unit, mem0, entry0, Q_SHARED_LIMITf,
                                        buf_queue->pool_limit);
                } else {
                    soc_mem_field32_set(unit, mem2, entry0, Q_SHARED_LIMITf,
                                        sal_ceil_func(limit, 4));
                }
            }
            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                    (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem2, entry0, Q_COLOR_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem2, entry0, YELLOW_SHARED_LIMITf,
                        buf_queue->yellow_limit
                                 & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                                 & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
                soc_mem_field32_set(unit, mem2, entry0, RED_SHARED_LIMITf,
                        buf_queue->red_limit
                                 & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                                 & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem2, entry0, YELLOW_SHARED_LIMITf,
                                    sal_ceil_func(limit, 8));
                soc_mem_field32_set(unit, mem2, entry0, RED_SHARED_LIMITf,
                                    sal_ceil_func(limit, 8));
            }

            soc_mem_field32_set(unit, mem2, entry0, Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem2, MEM_BLOCK_ALL, base + idx, entry0));

            sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_queue_offset_entry_t));

            soc_mem_field32_set(unit, mem3, entry0, RESUME_OFFSETf, 1);
            soc_mem_field32_set(unit, mem3, entry0,
                                YELLOW_RESUME_OFFSET_PROFILE_SELf, 0);
            soc_mem_field32_set(unit, mem3, entry0,
                                RED_RESUME_OFFSET_PROFILE_SELf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem3, MEM_BLOCK_ALL, base + idx, entry0));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                    WRITE_MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_YELLOWr(unit, 0,
                        1));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                    WRITE_MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_REDr(unit, 0,
                        1));
        }
    }

    /* Per  port per pool */
    for (idx = 0; idx < _TD3_MMU_NUM_POOL; idx++) {
        buf_port_pool = &buf->ports[port].pools[idx];
        if (buf_port_pool->pool_limit == 0) {
            continue;
        }

        limit = buf_port_pool->pool_limit;
        rlimit = limit - (default_mtu_cells * 2);

        mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_mc_mem[0])[pipe];
        if (mem0 == INVALIDm) {
            continue;
        }

        index1 = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, port,
                                             mmu_thdo_port_mc_mem[0], idx);
        sal_memset(entry0, 0, sizeof(mmu_thdm_db_portsp_config_entry_t));

        soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMITf, limit);
        soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                            sal_ceil_func(limit, 8));
        soc_mem_field32_set(unit, mem0, entry0, YELLOW_SHARED_LIMITf,
                            sal_ceil_func(limit, 8));

        soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMIT_ENABLEf,
                            !lossless);

        soc_mem_field32_set(unit, mem0, entry0, SHARED_RESUME_LIMITf,
                            sal_ceil_func(rlimit, 8));
        soc_mem_field32_set(unit, mem0, entry0, YELLOW_RESUME_LIMITf,
                            sal_ceil_func(rlimit, 8));
        soc_mem_field32_set(unit, mem0, entry0, RED_RESUME_LIMITf,
                            sal_ceil_func(rlimit, 8));

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem0, MEM_BLOCK_ALL,
                            index1, entry0));

        mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_mc_mem[1])[pipe];
        if (mem1 == INVALIDm) {
            continue;
        }
        buf_pool = &buf->pools[idx];
        sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_portsp_config_entry_t));

        limit = _TD3_MMU_TOTAL_MCQ_ENTRY(unit);

        soc_mem_field32_set(unit, mem1, entry0, SHARED_LIMITf,
                            (sal_ceil_func(limit, 4)) - 1);
        soc_mem_field32_set(unit, mem1, entry0, YELLOW_SHARED_LIMITf,
                            (sal_ceil_func(limit, 8)) - 1);
        soc_mem_field32_set(unit, mem1, entry0, RED_SHARED_LIMITf,
                            (sal_ceil_func(limit, 8)) - 1);

        soc_mem_field32_set(unit, mem1, entry0, SHARED_LIMIT_ENABLEf,
                            !lossless);

        soc_mem_field32_set(unit, mem1, entry0, SHARED_RESUME_LIMITf,
                            (sal_ceil_func(limit, 8)) - 2);
        soc_mem_field32_set(unit, mem1, entry0, YELLOW_RESUME_LIMITf,
                            (sal_ceil_func(limit, 8)) - 2);
        soc_mem_field32_set(unit, mem1, entry0, RED_RESUME_LIMITf,
                            (sal_ceil_func(limit, 8)) - 2);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL,
                            index1, entry0));
    }

    /* Output port per port per queue setting for regular unicast queue */
    pipe = si->port_pipe[port];
    /* per port regular unicast queue */
    numq = si->port_num_uc_cosq[port];
    base = si->port_uc_cosq_base[port];

    mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[0])[pipe];
    mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[1])[pipe];
    mem2 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[2])[pipe];
    if ((numq != 0) &&
        (mem0 != INVALIDm) && (mem1 != INVALIDm) && (mem2 != INVALIDm)) {
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[si->port_num_cosq[port] + idx];

            sal_memset(entry0, 0, sizeof(mmu_thdu_config_queue_entry_t));
            sal_memset(entry1, 0, sizeof(mmu_thdu_offset_queue_entry_t));

            soc_mem_field32_set(unit, mem0, entry0,
                                Q_MIN_LIMIT_CELLf, buf_queue->guarantee);
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_DYNAMIC_CELLf, 1);
                soc_mem_field32_set(unit, mem0, entry0, Q_SHARED_ALPHA_CELLf,
                                    buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0, Q_SHARED_LIMIT_CELLf,
                                    buf_queue->pool_limit);
            }
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_CELLf,
                                sal_ceil_func(buf_queue->pool_resume, 8));

            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                    (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_YELLOW_CELLf,
                        buf_queue->yellow_limit
                                 & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                                 & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_RED_CELLf,
                        buf_queue->red_limit
                                 & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                                 & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_YELLOW_CELLf,
                                    sal_ceil_func(buf_queue->yellow_limit, 8));
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_RED_CELLf,
                                    sal_ceil_func(buf_queue->red_limit, 8));
            }
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_YELLOW_CELLf,
                                sal_ceil_func(buf_queue->yellow_resume, 8));
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_RED_CELLf,
                                sal_ceil_func(buf_queue->red_resume, 8));
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, base + idx, entry0));

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, base + idx, entry1));

            sal_memset(entry0, 0, sizeof(mmu_thdo_q_to_qgrp_map_entry_t));

            if (buf_queue->qgroup_id >= 0) {
                soc_mem_field32_set(unit, mem2, entry0, QGROUP_VALIDf, 1);
                if (buf_queue->qgroup_min_enable) {
                    soc_mem_field32_set(unit, mem2, entry0, USE_QGROUP_MINf, 1);
                }
            }
            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_ENABLEf, 1);
            }

            soc_mem_field32_set(unit, mem2, entry0, Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL, base + idx,
                                entry0));
        }
    }

    /* Per  port per pool unicast */
    for (idx = 0; idx < _TD3_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];

        if (buf_pool->total == 0) {
            continue;
        }

        limit = buf->ports[port].pools[idx].pool_limit;

        mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_uc_mem[0])[pipe];
        mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_uc_mem[1])[pipe];
        if ((mem0 == INVALIDm) || (mem1 == INVALIDm)) {
            continue;
        }
        index1 = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, port,
                                             mmu_thdo_port_uc_mem[0], idx);
        sal_memset(entry0, 0, sizeof(mmu_thdu_config_port_entry_t));
        sal_memset(entry1, 0, sizeof(mmu_thdu_resume_port_entry_t));

        soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMITf, limit);
        soc_mem_field32_set(unit, mem0, entry0, YELLOW_LIMITf,
                            sal_ceil_func(limit, 8) / 8);
        soc_mem_field32_set(unit, mem0, entry0, RED_LIMITf,
                            sal_ceil_func(limit, 8) / 8);
        if (limit >= default_mtu_cells * 2) {
            soc_mem_field32_set(unit, mem1, entry1, SHARED_RESUMEf,
                            sal_ceil_func((limit - (default_mtu_cells * 2)),
                            8) / 8);

            soc_mem_field32_set(unit, mem1, entry1, YELLOW_RESUMEf,
                            sal_ceil_func((limit - (default_mtu_cells * 2)),
                            8) / 8);

            soc_mem_field32_set(unit, mem1, entry1, RED_RESUMEf,
                            sal_ceil_func((limit - (default_mtu_cells * 2)),
                            8) / 8);
        }

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem0, MEM_BLOCK_ALL,
                            index1, entry0));
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL,
                            index1, entry1));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_td3_mmu_config_buf_set_hw_global(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless,
                               _soc_td3_mmu_sw_config_info_t *swcfg)
{
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    mmu_thdo_config_qgroup_entry_t cfg_qgrp;
    mmu_thdo_offset_qgroup_entry_t offset_qgrp;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    soc_mem_t mem0, mem1;
    uint32 fval, rval, rval2, rval3;
    int rqlen, idx, pval;
    int jumbo_frame_cells, default_mtu_cells, limit;
    int pipe;
    uint64 rval64;
    uint64 rval2_64;
    int pool_resume = 0;

     static const soc_mem_t mmu_thdo_qgrp_uc_mem[] = {
         MMU_THDU_CONFIG_QGROUPm,
         MMU_THDU_OFFSET_QGROUPm
    };

    jumbo_frame_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->jumbo_pkt_size +
                                                    devcfg->mmu_hdr_byte,
                                                    devcfg->mmu_cell_size);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    pool_resume = 2 * jumbo_frame_cells;

    rval = 0;
    fval = _TD3_MMU_PHYSICAL_CELLS_PER_XPE - _TD3_MMU_RSVD_CELLS_CFAP_PER_XPE;
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDSETr, &rval, CFAPFULLSETPOINTf,
                      fval);
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg32_set(unit, CFAPFULLTHRESHOLDSETr, -1, -1,
                                   -1, rval));

    rval = 0;
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDRESETr, &rval,
                      CFAPFULLRESETPOINTf, fval - (2 * jumbo_frame_cells));
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg32_set(unit, CFAPFULLTHRESHOLDRESETr, -1, -1,
                                   -1, rval));

    rval = 0;
    soc_reg_field_set(unit, CFAPBANKFULLr, &rval, LIMITf,
                      _TD3_MMU_CFAP_BANK_FULL_LIMIT);
    for (idx = 0; idx < SOC_REG_NUMELS(unit, CFAPBANKFULLr); idx++) {
        SOC_IF_ERROR_RETURN(
            soc_trident3_xpe_reg32_set(unit, CFAPBANKFULLr, -1, -1,
                                       idx, rval));
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_MMU_SCFG_TOQ_MC_CFG0r(unit, &rval));
    soc_reg_field_set(unit, MMU_SCFG_TOQ_MC_CFG0r, &rval,
                      MCQE_FULL_THRESHOLDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MMU_SCFG_TOQ_MC_CFG0r(unit, rval));

    rval = 0;
    for (idx = 0; idx < 2; idx++) {
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, THDI_HDRM_POOL_CFGr, idx, 0, &rval));
        soc_reg_field_set(unit, THDI_HDRM_POOL_CFGr, &rval,
                          PEAK_COUNT_UPDATE_EN_HPf, 0);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, THDI_HDRM_POOL_CFGr, idx, 0, rval));
    }

    /* Input thresholds */
    rval = 0;
    soc_reg_field_set(unit, THDI_GLOBAL_HDRM_LIMITr,
                      &rval, GLOBAL_HDRM_LIMITf, buf->headroom / 2);
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg32_set(unit, THDI_GLOBAL_HDRM_LIMITr, -1, -1, 0,
                                   rval));

    for (idx = 0; idx < _TD3_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        limit = buf_pool->total -
                buf_pool->port_guarantee -
                buf_pool->prigroup_guarantee -
                buf_pool->prigroup_headroom;

        rval = 0;
        soc_reg_field_set(unit, THDI_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
                          limit);
        SOC_IF_ERROR_RETURN(
            soc_trident3_xpe_reg32_set(unit, THDI_BUFFER_CELL_LIMIT_SPr,
                                       -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, THDI_CELL_RESET_LIMIT_OFFSET_SPr, &rval,
                          OFFSETf, pool_resume);
        SOC_IF_ERROR_RETURN(
            soc_trident3_xpe_reg32_set(unit, THDI_CELL_RESET_LIMIT_OFFSET_SPr,
                                       -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, THDI_HDRM_BUFFER_CELL_LIMIT_HPr, &rval,
                          LIMITf, buf_pool->prigroup_headroom);
        SOC_IF_ERROR_RETURN(
            soc_trident3_xpe_reg32_set(unit, THDI_HDRM_BUFFER_CELL_LIMIT_HPr,
                                       -1, -1, idx, rval));
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg32_set(unit, THDI_BUFFER_CELL_LIMIT_PUBLIC_POOLr,
                                   -1, -1, 0, rval));

    /* output thresholds */
    SOC_IF_ERROR_RETURN(READ_OP_THDU_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval,
                      ENABLE_QUEUE_AND_GROUP_TICKETf, 1);
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval,
                      ENABLE_UPDATE_COLOR_RESUMEf, 0);
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval, MOP_POLICY_1Bf, 1);
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval, MOP_POLICY_1Af, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_THDU_CONFIGr(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, MMU_THDM_DB_DEVICE_THR_CONFIGr, &rval,
                      MOP_POLICYf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_MMU_THDR_DB_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, MMU_THDR_DB_CONFIGr, &rval,
                      MOP_POLICY_1Bf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIGr(unit, rval));

    /* per service pool settings */
    for (idx = 0; idx < _TD3_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        limit = buf_pool->total -
                buf_pool->queue_group_guarantee -
                buf_pool->queue_guarantee -
                buf_pool->rqe_guarantee;

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_SHARED_LIMITr, &rval,
                          SHARED_LIMITf, limit);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_SHARED_LIMITr(unit,
                                                                 idx,
                                                                 rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr,
                          &rval, YELLOW_SHARED_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr(unit,
                                                                        idx,
                                                                        rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RED_SHARED_LIMITr,
                          &rval, RED_SHARED_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_RED_SHARED_LIMITr(unit,
                                                                     idx,
                                                                     rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RESUME_LIMITr,
                          &rval, RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_RESUME_LIMITr(unit,
                                                                 idx,
                                                                 rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr,
                          &rval, YELLOW_RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr(unit,
                                                                        idx,
                                                                        rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RED_RESUME_LIMITr,
                          &rval, RED_RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_RED_RESUME_LIMITr(unit,
                                                                     idx,
                                                                     rval));

        /* mcq entries */
        limit = buf_pool->total_mcq_entry - buf_pool->mcq_entry_reserved;

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_SHARED_LIMITr,
                          &rval, SHARED_LIMITf, sal_ceil_func(limit, 4));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_SHARED_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr,
                          &rval, YELLOW_SHARED_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr,
                          &rval, RED_SHARED_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RESUME_LIMITr,
                          &rval, RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_RESUME_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr,
                          &rval, YELLOW_RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr,
                          &rval, RED_RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr(unit,
                                                                   idx, rval));
    }

    /* configure Q-groups */
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        for (idx = 0; idx < SOC_TD3_MMU_CFG_QGROUP_MAX; idx++) {
            mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_qgrp_uc_mem[0])[pipe];
            mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_qgrp_uc_mem[1])[pipe];
            if ((mem0 == INVALIDm) || (mem1 == INVALIDm)) {
                continue;
            }
            queue_grp = &buf->qgroups[idx];


            sal_memset(&cfg_qgrp, 0, sizeof(cfg_qgrp));
            sal_memset(&offset_qgrp, 0, sizeof(offset_qgrp));

            soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                Q_MIN_LIMIT_CELLf, queue_grp->guarantee);

            if (queue_grp->pool_scale != -1) {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    Q_SHARED_ALPHA_CELLf,
                                    queue_grp->pool_scale);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    Q_LIMIT_DYNAMIC_CELLf, 1);
            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    Q_SHARED_LIMIT_CELLf,
                                    queue_grp->pool_limit);
            }

            if ((queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
            }

            if (queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                pval = _soc_td3_pool_scale_to_limit(queue_grp->pool_limit,
                          queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_RED_CELLf, sal_ceil_func(pval, 8));

            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_RED_CELLf,
                                    sal_ceil_func(queue_grp->red_limit, 8));
            }

            if (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                pval = _soc_td3_pool_scale_to_limit(queue_grp->pool_limit,
                          queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_YELLOW_CELLf, sal_ceil_func(pval, 8));

            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_YELLOW_CELLf,
                                    sal_ceil_func(queue_grp->yellow_limit, 8));
            }

            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_CELLf,
                                sal_ceil_func(queue_grp->pool_resume, 8));
            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_YELLOW_CELLf,
                                sal_ceil_func(queue_grp->pool_resume, 8));
            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_RED_CELLf,
                                sal_ceil_func(queue_grp->pool_resume, 8));
            SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, idx, &cfg_qgrp));
            SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, idx, &offset_qgrp));
        }
    }

    /* RQE */
    for (idx = 0; idx < _TD3_MMU_NUM_RQE_QUEUES; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_MIN_PRIQr,
                          &rval, MIN_LIMITf, buf_rqe_queue->guarantee);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_LIMIT_MIN_PRIQr(unit, idx, rval));

        rval = 0;
        rval2 = 0;
        rval3 = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                            &rval, SPIDf, buf_rqe_queue->pool_idx);

        if ((buf_rqe_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
            (buf_rqe_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                              &rval, COLOR_LIMIT_DYNAMICf, 1);
            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                SHARED_RED_LIMITf,
                buf_rqe_queue->red_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                SHARED_YELLOW_LIMITf,
                buf_rqe_queue->yellow_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
        } else {
            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                              SHARED_RED_LIMITf,
                              sal_ceil_func(buf_rqe_queue->red_limit, 8));

            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                              SHARED_YELLOW_LIMITf,
                              sal_ceil_func(buf_rqe_queue->yellow_limit, 8));
        }

        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr, &rval, LIMIT_ENABLEf,
                          (buf_rqe_queue->discard_enable ? 1 : 0));

        if (buf_rqe_queue->pool_scale != -1) {
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                              &rval, DYNAMIC_ENABLEf, 1);
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                              SHARED_ALPHAf, buf_rqe_queue->pool_scale);
        } else {
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                              SHARED_LIMITf, buf_rqe_queue->pool_limit);
        }
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                          RESET_OFFSETf, 2);

        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG1_PRIQr(unit, idx, rval));

        COMPILER_64_SET(rval2_64, 0, rval2);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG_PRIQr(unit, idx, rval2_64));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_LIMIT_COLOR_PRIQr(unit,
                                                                idx, rval3));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_REDf, 2);
        soc_reg_field_set(unit, MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_YELLOWf, 2);
        SOC_IF_ERROR_RETURN(
                  WRITE_MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr(unit, idx, rval));

        /* queue entry */
        buf_pool = &buf->pools[buf_rqe_queue->pool_idx];
        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_MIN_PRIQr,
                          &rval, MIN_LIMITf,
                          sal_ceil_func(buf_rqe_queue->guarantee, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_LIMIT_MIN_PRIQr(unit, idx, rval));

        rval = 0;
        rval2 = 0;
        rval3 = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                          &rval, SPIDf, buf_rqe_queue->pool_idx);

        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                          &rval, COLOR_LIMIT_DYNAMICf, (!lossless));
        soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                          &rval3, SHARED_RED_LIMITf,
                          lossless? (sal_ceil_func(buf_pool->total_rqe_entry,
                          8)): 0);
        soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                          &rval3, SHARED_YELLOW_LIMITf,
                          lossless? (sal_ceil_func(buf_pool->total_rqe_entry,
                          8)): 0);

        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr, &rval,
                          LIMIT_ENABLEf, !lossless);
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                          &rval, DYNAMIC_ENABLEf, (!lossless));

        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_PRIQr, &rval2,
                          SHARED_LIMITf,
                          lossless? (sal_ceil_func(buf_pool->total_rqe_entry,
                          8)): 8);
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_PRIQr, &rval2,
                          RESET_OFFSETf, 1);

        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG1_PRIQr(unit, idx, rval));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG_PRIQr(unit, idx, rval2));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_LIMIT_COLOR_PRIQr(unit, idx, rval3));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_REDf,
                          sal_ceil_func(default_mtu_cells, 8));
        soc_reg_field_set(unit, MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_YELLOWf,
                          sal_ceil_func(default_mtu_cells, 8));
        SOC_IF_ERROR_RETURN(
                  WRITE_MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr(unit, idx, rval));

    }

    /* per pool RQE settings */
    for (idx = 0; idx < _TD3_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if (((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) ||
            (buf_pool->total == 0)) {
            continue;
        }

        limit = swcfg->egr_shared_total;

        COMPILER_64_ZERO(rval64);
        soc_reg64_field32_set(unit, MMU_THDR_DB_CONFIG_SPr,
                              &rval64, SHARED_LIMITf, limit);
        soc_reg64_field32_set(unit, MMU_THDR_DB_CONFIG_SPr, &rval64,
            RESUME_LIMITf, sal_ceil_func((limit - (default_mtu_cells * 2)), 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG_SPr(unit, idx, rval64));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_SP_SHARED_LIMITr, &rval,
                          SHARED_RED_LIMITf,
                          sal_ceil_func(limit, 8));
        soc_reg_field_set(unit, MMU_THDR_DB_SP_SHARED_LIMITr, &rval,
                          SHARED_YELLOW_LIMITf,
                          sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_SP_SHARED_LIMITr(unit, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_RED_LIMITf,
                          sal_ceil_func((limit - (default_mtu_cells * 2)), 8));
        soc_reg_field_set(unit, MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_YELLOW_LIMITf,
                          sal_ceil_func((limit - (default_mtu_cells * 2)), 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr(unit,
                                                                     idx, rval));

        rqlen = sal_ceil_func(buf_pool->total_rqe_entry, 8);
        if (rqlen == 0) {
            continue;
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_SPr, &rval,
                          SHARED_LIMITf, rqlen);
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_SPr, &rval,
                          RESUME_LIMITf, rqlen - 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG_SPr(unit, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr, &rval,
                          SHARED_RED_LIMITf, rqlen);
        soc_reg_field_set(unit, MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr, &rval,
                          SHARED_YELLOW_LIMITf, rqlen);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr(unit,
                                                                     idx,
                                                                     rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_RED_LIMITf, rqlen - 1);
        soc_reg_field_set(unit, MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_YELLOW_LIMITf, rqlen - 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr(unit,
                                                                     idx, rval));
    }


    /* Device level config setting */
#if 0
    if (soc_property_get(unit, spn_PORT_UC_MC_ACCOUNTING_COMBINE, 0)) {
        SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, MMU_THDM_DB_DEVICE_THR_CONFIGr, &rval,
                          UC_MC_PORTSP_COMB_ACCT_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, rval));
    }
#endif
    return SOC_E_NONE;
}

STATIC int
_soc_td3_mmu_config_buf_set_hw(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless,
                               _soc_td3_mmu_sw_config_info_t *swcfg)
{
    int port;

    SOC_IF_ERROR_RETURN
        (_soc_td3_mmu_config_buf_set_hw_global(unit, buf, devcfg, lossless,
                                               swcfg));

    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (_soc_td3_mmu_config_buf_set_hw_port(unit, port, buf, devcfg,
                                                 lossless, swcfg));
    }

    return SOC_E_NONE;
}

int
soc_td3_mmu_config_init_port(int unit, int port)
{
    int rv;
    int lossless = 1;
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_device_info_t devcfg;
    _soc_td3_mmu_sw_config_info_t swcfg;

    buf = soc_mmu_cfg_alloc(unit);
    if (!buf) {
        return SOC_E_MEMORY;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    _soc_td3_mmu_init_dev_config(unit, &devcfg, lossless);
    _soc_td3_mmu_sw_info_config (unit, &swcfg, lossless);


    /* Calculate the buf - global as well as per port
     * but _soc_td3_mmu_config_buf_set_hw_port is only called
     * for that port - since it is flex operation - we don't
     * touch any other port */
    _soc_td3_mmu_config_buf_phase1(unit, buf, &devcfg, &swcfg, lossless);

    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        /* Override default config */
        _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
    }
    rv = _soc_mmu_cfg_buf_check(unit, buf, &devcfg);
    if (SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU config: Use default setting\n")));
        _soc_td3_mmu_config_buf_phase1(unit, buf, &devcfg, &swcfg, lossless);
        SOC_IF_ERROR_RETURN
            (_soc_mmu_cfg_buf_calculate(unit, buf, &devcfg));
    }

    _soc_td3_mmu_config_buf_phase2(unit, buf, &devcfg, &swcfg, lossless);
    /* Override default phase 2 config */
    if (SOC_SUCCESS(rv)) {
        if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
            _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
        }
    }
    rv = _soc_td3_mmu_config_buf_set_hw_port(unit, port, buf, &devcfg,
                                             lossless, &swcfg);

    soc_mmu_cfg_free(unit, buf);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));
    return rv;
}

/* Default MMU settings - Version 3.0 */
STATIC int
soc_td3_mmu_config_init(int unit, int test_only)
{
    int rv;
    int lossless = 1;
    _soc_mmu_cfg_buf_t *buf = NULL;
    _soc_mmu_cfg_buf_t *buf_canned = NULL;
    _soc_mmu_cfg_buf_t *buf_user = NULL;
    _soc_mmu_device_info_t devcfg;
    _soc_td3_mmu_sw_config_info_t swcfg;

    buf_canned = soc_mmu_cfg_alloc(unit);
    if (!buf_canned) {
        return SOC_E_MEMORY;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);
    SOC_INFO(unit).mmu_lossless = lossless;

    _soc_td3_mmu_init_dev_config(unit, &devcfg, lossless);
    _soc_td3_mmu_sw_info_config (unit, &swcfg, lossless);

    _soc_td3_mmu_config_buf_phase1(unit, buf_canned, &devcfg, &swcfg, lossless);
    rv = _soc_mmu_cfg_buf_check(unit, buf_canned, &devcfg);
    _soc_td3_mmu_config_buf_phase2(unit, buf_canned, &devcfg, &swcfg, lossless);
    buf = buf_canned;

    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        buf_user = soc_mmu_cfg_alloc(unit);
        if (!buf_user) {
            soc_mmu_cfg_free(unit, buf_canned);
            return SOC_E_MEMORY;
        }

        sal_memcpy(buf_user, buf_canned, sizeof(_soc_mmu_cfg_buf_t));

        /* Pickup user-defined parameters */
        _soc_mmu_cfg_buf_read(unit, buf_user, &devcfg);

        /* Validate the new parameters */
        rv = _soc_mmu_cfg_buf_check(unit, buf_user, &devcfg);
        if (SOC_SUCCESS(rv)) {
            rv = _soc_mmu_cfg_buf_calculate(unit, buf_user, &devcfg);
        }
        if (SOC_SUCCESS(rv)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU config: Using custom settings\n")));
            buf = buf_user;
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU config: Bad MMU settings.  Using defaults\n")));
        }
    }

    rv = _soc_td3_mmu_config_buf_set_hw(unit, buf, &devcfg, lossless, &swcfg);

    soc_mmu_cfg_free(unit, buf_canned);
    if (buf_user != NULL) {
        soc_mmu_cfg_free(unit, buf_user);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));
    return rv;
}

STATIC int
_soc_trident3_mmu_init(int unit)
{
    int port;
    int enable = 1;
    int test_only;
    uint32 rval = 0;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
    } else
#endif
    {
        test_only = (SAL_BOOT_XGSSIM) ? TRUE : FALSE;
        SOC_IF_ERROR_RETURN(soc_td3_mmu_config_init(unit, test_only));

        PBMP_ALL_ITER(unit, port) {
            _soc_td3_thdo_hw_set(unit, port, enable);
        }
    }

    /* enable WRED refresh */
    SOC_IF_ERROR_RETURN(READ_WRED_REFRESH_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, WRED_REFRESH_CONTROLr, &rval,
                      REFRESH_DISABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_WRED_REFRESH_CONTROLr(unit, rval));

    SOC_IF_ERROR_RETURN(soc_td3_post_mmu_init_update(unit));
    return SOC_E_NONE;
}

int
soc_trident3_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    *enabled = soc->l2x_age_pid != SAL_THREAD_ERROR &&
               soc->l2x_age_enable ?
               1 : 0;
    *age_seconds = soc->l2x_age_pid != SAL_THREAD_ERROR &&
                   soc->l2x_age_enable ?
                   soc->l2x_age_interval : 0;
    return SOC_E_NONE;
}

int
soc_trident3_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds = 0x7fffffff;
    return SOC_E_NONE;
}

int
soc_trident3_age_timer_set(int unit, int age_seconds, int enable)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    sal_usecs_t interval = soc->l2x_age_interval;

    if (soc->l2x_age_interval) {
        if (!enable) {
            soc->l2x_age_enable = 0;
        } else {
            if (age_seconds) {
                SOC_CONTROL_LOCK(unit);
                soc->l2x_age_interval = age_seconds;
                SOC_CONTROL_UNLOCK(unit);
            }
            soc->l2x_age_enable = 1;
            if (interval != age_seconds) {
                sal_sem_give(soc->l2x_age_notify);
            }
        }
    } else {
        if (enable) {
            soc->l2x_age_enable = 1;
            SOC_IF_ERROR_RETURN
                (soc_th_l2_bulk_age_start(unit, age_seconds));
        }
    }

    return SOC_E_NONE;
}

STATIC int
soc_trident3_oversub_group_find(int unit, soc_td3_port_lanes_t *lanes_ctrl)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t reg;
    int port_base, phy_port_base, idb_port_base, mmu_port_base;
    int port, mmu_port, id, inst;
    uint32 rval, fval;
    int pipe, group, slot, count, i;
    int cur_ids[4], cur_ids_len;
    int ids_len;
    int slots[4], slots_len;
    int speed_max, cur_speed_max;
    static const soc_reg_t idb_grp_tbl_regs[] = {
        IS_OVR_SUB_GRP0_TBLr, IS_OVR_SUB_GRP1_TBLr,
        IS_OVR_SUB_GRP2_TBLr, IS_OVR_SUB_GRP3_TBLr,
        IS_OVR_SUB_GRP4_TBLr, IS_OVR_SUB_GRP5_TBLr
    };
    static const soc_reg_t mmu_grp_tbl_regs[] = {
        OVR_SUB_GRP0_TBLr, OVR_SUB_GRP1_TBLr,
        OVR_SUB_GRP2_TBLr, OVR_SUB_GRP3_TBLr,
        OVR_SUB_GRP4_TBLr, OVR_SUB_GRP5_TBLr
    };

    port_base = lanes_ctrl->port_base;
    phy_port_base = si->port_l2p_mapping[port_base];
    idb_port_base = si->port_l2i_mapping[port_base];
    mmu_port_base = si->port_p2m_mapping[phy_port_base];
    pipe = si->port_pipe[port_base];

    /* Figure out the new speed group */
    speed_max = 25000 * lanes_ctrl->lanes;
    if (speed_max > si->port_speed_max[port_base]) {
        speed_max = si->port_speed_max[port_base];
    }
    _soc_trident3_speed_to_ovs_class_mapping(unit, speed_max,
                                             &lanes_ctrl->ovs_class);

    /* Figure out the current speed group */
    cur_speed_max = 25000 * lanes_ctrl->cur_lanes;
    if (cur_speed_max > si->port_speed_max[port_base]) {
        cur_speed_max = si->port_speed_max[port_base];
    }
    _soc_trident3_speed_to_ovs_class_mapping(unit, cur_speed_max,
                                             &lanes_ctrl->cur_ovs_class);

    /* Construct new and old idb port list for oversub group replacement */
    cur_ids[0] = idb_port_base;
    ids_len = cur_ids_len = 1;
    if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
        /* idb port(s) to be removed */
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
            cur_ids[1 + i] = si->port_l2i_mapping[port];
        }
        cur_ids_len += lanes_ctrl->phy_ports_len;
    } else { /* idb port(s) to be added */
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
        }
        ids_len += lanes_ctrl->phy_ports_len;
    }

    lanes_ctrl->idb_group = -1;
    lanes_ctrl->idb_slots[0] = -1;
    lanes_ctrl->cur_idb_slots[0] = -1;
    for (group = 0; group < _OVS_GROUP_COUNT; group++) {
        reg = SOC_REG_UNIQUE_ACC(unit, IS_OVR_SUB_GRP_CFGr)[pipe];
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, group, &rval));
        fval = soc_reg_field_get(unit, reg, rval, SPEEDf);
        if (fval == 0) { /* unused idb port oversub group */
            if (lanes_ctrl->idb_group == -1) {
                lanes_ctrl->idb_group = group;
            }
            continue;
        } else if (fval == lanes_ctrl->ovs_class) {
            if (lanes_ctrl->idb_slots[0] == -1) { /* hasn't found the group */
                /* Find slot(s) for new idb port oversub group */
                reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
                count = _OVS_GROUP_TDM_LENGTH /
                    (_TD3_PORTS_PER_PBLK / lanes_ctrl->lanes);
                slots_len = 0;
                for (slot = 0; slot < count; slot++) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, REG_PORT_ANY, slot, &rval));
                    if (soc_reg_field_get(unit, reg, rval, PORT_NUMf) != 0x3f) {
                        continue;
                    }
                    slots[slots_len] = slot;
                    slots_len++;
                    if (slots_len == ids_len) {
                        /* Enough free slots in an existing group */
                        lanes_ctrl->idb_group = group;
                        for (i = 0; i < slots_len; i++) {
                            lanes_ctrl->idb_slots[i] = slots[i];
                        }
                        break;
                    }
                }
            }
        } else if (fval == lanes_ctrl->cur_ovs_class) {
            if (lanes_ctrl->cur_idb_slots[0] == -1) {
                /* Find slot(s) used by the currnet idb port oversub group */
                reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
                count = _OVS_GROUP_TDM_LENGTH /
                    (_TD3_PORTS_PER_PBLK / lanes_ctrl->cur_lanes);
                slots_len = 0;
                lanes_ctrl->cur_idb_empty = TRUE;
                for (slot = 0; slot < count; slot++) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, REG_PORT_ANY, slot, &rval));
                    id = soc_reg_field_get(unit, reg, rval, PORT_NUMf);
                    if (id == 0x3f) {
                        continue;
                    }
                    for (i = 0; i < cur_ids_len; i++) {
                        if (id == cur_ids[i]) {
                            slots[slots_len] = slot;
                            slots_len++;
                            break;
                        }
                    }
                    if (i == cur_ids_len) {
                        lanes_ctrl->cur_idb_empty = FALSE;
                    } else if (slots_len == cur_ids_len) {
                        lanes_ctrl->cur_idb_group = group;
                        for (i = 0; i < slots_len; i++) {
                            lanes_ctrl->cur_idb_slots[i] = slots[i];
                        }
                    }
                    if (lanes_ctrl->cur_idb_slots[0] != -1 &&
                        !lanes_ctrl->cur_idb_empty) {
                        break;
                    }
                }
            }
        }
        if (lanes_ctrl->idb_slots[0] != -1 &&
            lanes_ctrl->cur_idb_slots[0] != -1) {
            break;
        }
    }

    if (lanes_ctrl->idb_group == -1) {
        return SOC_E_FULL;
    }

    /* Construct new and old mmu port list for oversub group replacement */
    cur_ids[0] = mmu_port_base & 0x3f;
    ids_len = cur_ids_len = 1;
    if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
        /* mmu port(s) to be removed */
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            mmu_port = si->port_p2m_mapping[lanes_ctrl->phy_ports[i]];
            cur_ids[1 + i] = mmu_port & 0x3f;
        }
        cur_ids_len += lanes_ctrl->phy_ports_len;
    } else { /* mmu port(s) to be added */
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            mmu_port = si->port_p2m_mapping[lanes_ctrl->phy_ports[i]];
        }
        ids_len += lanes_ctrl->phy_ports_len;
    }

    lanes_ctrl->mmu_group = -1;
    lanes_ctrl->mmu_slots[0] = -1;
    lanes_ctrl->cur_mmu_slots[0] = -1;
    for (group = 0; group < _OVS_GROUP_COUNT; group++) {
        reg = OVR_SUB_GRP_CFGr;
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, group, &rval));
        fval = soc_reg_field_get(unit, reg, rval, SPEEDf);
        if (fval == 0) { /* unused mmu port oversub group */
            if (lanes_ctrl->mmu_group == -1) {
                lanes_ctrl->mmu_group = group;
            }
            continue;
        } else if (fval == lanes_ctrl->ovs_class) {
            if (lanes_ctrl->mmu_slots[0] == -1) { /* hasn't found the group */
                /* Find slot(s) for new mmu port oversub group */
                inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
                reg = mmu_grp_tbl_regs[group];
                count = _OVS_GROUP_TDM_LENGTH /
                    (_TD3_PORTS_PER_PBLK / lanes_ctrl->lanes);
                slots_len = 0;
                for (slot = 0; slot < count; slot++) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, inst, slot, &rval));
                    if (soc_reg_field_get(unit, reg, rval, PORT_NUMf) != 0x3f) {
                        continue;
                    }
                    slots[slots_len] = slot;
                    slots_len++;
                    if (slots_len == ids_len) {
                        /* Enough free slots in an existing group */
                        lanes_ctrl->mmu_group = group;
                        for (i = 0; i < slots_len; i++) {
                            lanes_ctrl->mmu_slots[i] = slots[i];
                        }
                        break;
                    }
                }
            }
        } else if (fval == lanes_ctrl->cur_ovs_class) {
            if (lanes_ctrl->cur_mmu_slots[0] == -1) {
                /* Find slot(s) used by the currnet mmu port oversub group */
                reg = mmu_grp_tbl_regs[group];
                inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
                count = _OVS_GROUP_TDM_LENGTH /
                    (_TD3_PORTS_PER_PBLK / lanes_ctrl->cur_lanes);
                slots_len = 0;
                lanes_ctrl->cur_mmu_empty = TRUE;
                for (slot = 0; slot < count; slot++) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, inst, slot, &rval));
                    id = soc_reg_field_get(unit, reg, rval, PORT_NUMf);
                    if (id == 0x3f) {
                        continue;
                    }
                    for (i = 0; i < cur_ids_len; i++) {
                        if (id == cur_ids[i]) {
                            slots[slots_len] = slot;
                            slots_len++;
                            break;
                        }
                    }
                    if (i == cur_ids_len) {
                        lanes_ctrl->cur_mmu_empty = FALSE;
                    } else if (slots_len == cur_ids_len) {
                        lanes_ctrl->cur_mmu_group = group;
                        for (i = 0; i < slots_len; i++) {
                            lanes_ctrl->cur_mmu_slots[i] = slots[i];
                        }
                    }
                    if (lanes_ctrl->cur_mmu_slots[0] != -1 &&
                        !lanes_ctrl->cur_mmu_empty) {
                        break;
                    }
                }
            }
        }
        if (lanes_ctrl->mmu_slots[0] != -1 &&
            lanes_ctrl->cur_mmu_slots[0] != -1) {
            break;
        }
    }

    if (lanes_ctrl->mmu_group == -1) {
        return SOC_E_FULL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_trident3_port_lanes_validate
 * Description:
 *      Validate if the specified number of lanes can be configured on a port,
 *      If the request can be done, fill in the control structure which will
 *      be used by the routines programming the hardware.
 * Parameters:
 *      unit          - Device number
 *      lanes_ctrl    - Control structure
 *
 * Each TSC can be configured into following 5 mode:
 *   Lane number    0              1         2              3
 *   ------------   -----------    --------  ------------   --------
 *    single port   100G/40G       x         x              x
 *      dual port   50G/d40G/20G   x         50G/d40G/20G   x
 *   tri_023 port   50G/d40g/20G   x         25G/10G        25G/10G
 *   tri_012 port   25G/10G        25G/10G   50G/d40G/20G   x
 *      quad port   25G/10G        25G/10G   25G/10G        25G/10G
 *
 * (Note: All of the above apply to equivalent HG line rate speeds as well)
 *
 *          lanes                mode         valid lane index
 *       ------------      ----------------   ----------------
 *       new  current        new    current
 *       ---  -------      -------  -------
 * #1     4      1         single    quad            0
 * #2     4      1         single   tri_012          0
 * #3     4      2         single   tri_023          0
 * #4     4      2         single    dual            0
 * #5     2      1         tri_023   quad            0
 * #6     2      1         tri_012   quad            2
 * #7     2      1          dual    tri_023          2
 * #8     2      1          dual    tri_012          0
 * #9     2      4          dual    single           0
 * #10    1      2         tri_023   dual            2
 * #11    1      2         tri_012   dual            0
 * #12    1      2          quad    tri_023          0
 * #13    1      2          quad    tri_012          2
 * #14    1      4          quad    single           0
 * Following mode change requires 2 transition
 *   - from single to tri_023: #9 + #10
 *   - from single to tri_012: #9 + #11
 * Following mode change are the result of lane change on multiple ports
 *   - from quad to dual: #12 + #7 or #13 + #8
 *   - from dual to quad: #10 + #12 or #11 + # 13
 *   - from tri_023 to tri_012: #7 + #11 or #12 + #6
 *   - from tri_012 to tri_023: #8 + #10 or #13 + #5
 *
 * Logical port number will stay the same after conversion, for example
 *     converting single port to dual port, the logical port number of lane 0
 *     will be changed.
 */
int
soc_trident3_port_lanes_validate(int unit, soc_td3_port_lanes_t *lanes_ctrl)
{
    soc_info_t *si = &SOC_INFO(unit);
    int port_base, phy_port_base;
    uint32 rval;
    int i;

    /* Find physical and mmu port number for the specified port */
    port_base = lanes_ctrl->port_base;
    phy_port_base = si->port_l2p_mapping[port_base];
    if (phy_port_base == -1) {
        return SOC_E_PORT;
    }
    lanes_ctrl->oversub = SOC_PBMP_MEMBER(si->oversub_pbm, port_base);

    /* Find lane index for the specified port (base port) */
    lanes_ctrl->bindex = SOC_PORT_BINDEX(unit, phy_port_base);

    /* Get the current mode */
    SOC_IF_ERROR_RETURN(READ_CLPORT_MODE_REGr(unit, port_base, &rval));
    lanes_ctrl->cur_mode = soc_reg_field_get(unit, CLPORT_MODE_REGr, rval,
                                             XPORT0_CORE_PORT_MODEf);

    /* Figure out the current number of lanes */
    switch (lanes_ctrl->cur_mode) {
    case SOC_TD3_PORT_MODE_QUAD:
        lanes_ctrl->cur_lanes = 1;
        break;
    case SOC_TD3_PORT_MODE_TRI_012:
        lanes_ctrl->cur_lanes = lanes_ctrl->bindex == 0 ? 1 : 2;
        break;
    case SOC_TD3_PORT_MODE_TRI_023:
        lanes_ctrl->cur_lanes = lanes_ctrl->bindex == 0 ? 2 : 1;
        break;
    case SOC_TD3_PORT_MODE_DUAL:
        lanes_ctrl->cur_lanes = 2;
        break;
    case SOC_TD3_PORT_MODE_SINGLE:
        lanes_ctrl->cur_lanes = 4;
        break;
    default:
        return SOC_E_FAIL;
    }
    /* Validate new number of lanes */
    if (lanes_ctrl->lanes == 4 || lanes_ctrl->cur_lanes == 4) {
        if (lanes_ctrl->bindex & 0x3) {
            return SOC_E_PARAM;
        }
    } else if (lanes_ctrl->lanes == 2 || lanes_ctrl->cur_lanes == 2) {
        if (lanes_ctrl->bindex & 0x1) {
            return SOC_E_PARAM;
        }
    } else if (lanes_ctrl->lanes != 1) {
        return SOC_E_PARAM;
    }

    /* No change to number of lanes */
    if (lanes_ctrl->lanes == lanes_ctrl->cur_lanes) {
        return SOC_E_NONE;
    }

    if (lanes_ctrl->oversub) {
        SOC_IF_ERROR_RETURN(soc_trident3_oversub_group_find(unit, lanes_ctrl));
    }

    /* Figure out new mode */
    if (lanes_ctrl->lanes == 4) {
        lanes_ctrl->mode = 4;
    } else if (lanes_ctrl->lanes == 2) {
        if (lanes_ctrl->cur_mode == SOC_TD3_PORT_MODE_QUAD) {
            lanes_ctrl->mode = lanes_ctrl->bindex == 0 ?
                SOC_TD3_PORT_MODE_TRI_023 : SOC_TD3_PORT_MODE_TRI_012;
        } else {
            lanes_ctrl->mode = SOC_TD3_PORT_MODE_DUAL;
        }
    } else{
        if (lanes_ctrl->cur_mode == SOC_TD3_PORT_MODE_DUAL) {
            lanes_ctrl->mode = lanes_ctrl->bindex == 0 ?
                SOC_TD3_PORT_MODE_TRI_012 : SOC_TD3_PORT_MODE_TRI_023;
        } else {
            lanes_ctrl->mode = SOC_TD3_PORT_MODE_QUAD;
        }
    }

    lanes_ctrl->phy_ports_len = 0;
    if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
        /* Figure out which port(s) to be removed */
        if (lanes_ctrl->lanes == 4) {
            if (lanes_ctrl->cur_mode == SOC_TD3_PORT_MODE_TRI_012 ||
                lanes_ctrl->cur_mode == SOC_TD3_PORT_MODE_QUAD) {
                if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
                    lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                        phy_port_base + 1;
                }
            }
            if (si->port_p2l_mapping[phy_port_base + 2] != -1) {
                lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                                      phy_port_base + 2;
            }
            if (lanes_ctrl->cur_mode == SOC_TD3_PORT_MODE_TRI_023 ||
                lanes_ctrl->cur_mode == SOC_TD3_PORT_MODE_QUAD) {
                if (si->port_p2l_mapping[phy_port_base + 3] != -1) {
                    lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                        phy_port_base + 3;
                }
            }
        } else {
            if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
                lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                                      phy_port_base + 1;
            }
        }
    } else { /* Figure out which port(s) to be added */
        if (lanes_ctrl->lanes == 2) {
            if (si->port_p2l_mapping[phy_port_base + 2] != -1) {
                lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                                      phy_port_base + 2;
            }
        } else {
            if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
                lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                    phy_port_base + 1;
            }
            if (lanes_ctrl->cur_mode == SOC_TD3_PORT_MODE_SINGLE) {
                if (si->port_p2l_mapping[phy_port_base + 2] != -1) {
                    lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                        phy_port_base + 2;
                }
                if (si->port_p2l_mapping[phy_port_base + 3] != -1) {
                    lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                        phy_port_base + 3;
                }
            }
        }
    }

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        static char *mode_name[] = {
            "QUAD", "TRI_012", "TRI_023", "DUAL", "SINGLE"
        };
        LOG_CLI((BSL_META_U(unit,
                            "port %d physical port %d bindex %d\n"),
                 port_base, phy_port_base, lanes_ctrl->bindex));
        LOG_CLI((BSL_META_U(unit,
                            "  mode (new:%s cur:%s) lanes (new:%d cur:%d)\n"),
                 mode_name[lanes_ctrl->mode],
                 mode_name[lanes_ctrl->cur_mode],
                 lanes_ctrl->lanes, lanes_ctrl->cur_lanes));
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            LOG_CLI((BSL_META_U(unit,
                                "  %s physical port %d (port %d)\n"),
                     lanes_ctrl->lanes > lanes_ctrl->cur_lanes ?
                     "del" : "add",
                     lanes_ctrl->phy_ports[i],
                     si->port_p2l_mapping[lanes_ctrl->phy_ports[i]]));
        }
        if (lanes_ctrl->idb_group != -1) {
            LOG_CLI((BSL_META_U(unit,
                                "new idb group %d slot %d"),
                     lanes_ctrl->idb_group, lanes_ctrl->idb_slots[0]));
            if (lanes_ctrl->lanes < lanes_ctrl->cur_lanes &&
                lanes_ctrl->idb_slots[0] != -1) {
                for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                    LOG_CLI((BSL_META_U(unit,
                                        " slot %d"), lanes_ctrl->idb_slots[i]));
                }
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
        LOG_CLI((BSL_META_U(unit,
                            "cur idb group %d slot %d"),
                 lanes_ctrl->cur_idb_group, lanes_ctrl->cur_idb_slots[0]));
        if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
            /* port(s) to be removed */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                LOG_CLI((BSL_META_U(unit,
                                    " slot %d"), lanes_ctrl->idb_slots[i]));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));

        if (lanes_ctrl->mmu_group != -1) {
            LOG_CLI((BSL_META_U(unit,
                                "new mmu group %d slot %d"),
                     lanes_ctrl->mmu_group, lanes_ctrl->mmu_slots[0]));
            if (lanes_ctrl->lanes < lanes_ctrl->cur_lanes &&
                lanes_ctrl->mmu_slots[0] != -1) {
                /* port(s) to be added */
                for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                    LOG_CLI((BSL_META_U(unit,
                                        " slot %d"), lanes_ctrl->mmu_slots[i]));
                }
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
        LOG_CLI((BSL_META_U(unit,
                            "cur mmu group %d slot %d"),
                 lanes_ctrl->cur_mmu_group, lanes_ctrl->cur_mmu_slots[0]));
        if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
            /* port(s) to be removed */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                LOG_CLI((BSL_META_U(unit,
                                    " slot %d"), lanes_ctrl->mmu_slots[i]));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

    return SOC_E_NONE;
}

soc_error_t
soc_td3_granular_speed_get(int unit, soc_port_t port, int *speed)
{
    uint32 fval;
    egr_mmu_cell_credit_entry_t credit_entry;

    SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ANY,
                        SOC_INFO(unit).port_l2p_mapping[port], &credit_entry));
    fval = soc_EGR_MMU_CELL_CREDITm_field32_get(unit, &credit_entry, CREDITf);

    SOC_IF_ERROR_RETURN(soc_td3_port_asf_mmu_cell_credit_to_speed(unit,
                            port, (uint8) fval, speed));
    return SOC_E_NONE;
}

int soc_td3_mmu_traffic_ctrl_size(int unit, int *req_scache_size)
{
    *req_scache_size += sizeof(_soc_td3_mmu_traffic_ctrl_t);

    return SOC_E_NONE;
}

int soc_td3_mmu_traffic_ctrl_wb_sync(int unit, uint8 **scache_ptr)
{
    _soc_td3_mmu_traffic_ctrl_t *mmu_traffic_ctrl_scache_p;
    _soc_td3_mmu_traffic_ctrl_t *mmu_traffic_ctrl;

    mmu_traffic_ctrl_scache_p =
        (_soc_td3_mmu_traffic_ctrl_t *)(*scache_ptr);

    mmu_traffic_ctrl = _soc_td3_mmu_traffic_ctrl[unit];
    if (mmu_traffic_ctrl == NULL || mmu_traffic_ctrl_scache_p == NULL) {
        return SOC_E_NONE;
    }
    sal_memcpy(mmu_traffic_ctrl_scache_p, mmu_traffic_ctrl,
               sizeof(_soc_td3_mmu_traffic_ctrl_t));
    mmu_traffic_ctrl_scache_p++;

    *scache_ptr = (uint8 *)mmu_traffic_ctrl_scache_p;

    return SOC_E_NONE;
}

int soc_td3_mmu_traffic_ctrl_wb_restore(int unit, uint8 **scache_ptr)
{
    _soc_td3_mmu_traffic_ctrl_t *mmu_traffic_ctrl_scache_p;
    _soc_td3_mmu_traffic_ctrl_t *mmu_traffic_ctrl;

    mmu_traffic_ctrl_scache_p =
        (_soc_td3_mmu_traffic_ctrl_t *)(*scache_ptr);

    mmu_traffic_ctrl = _soc_td3_mmu_traffic_ctrl[unit];
    if (mmu_traffic_ctrl == NULL || mmu_traffic_ctrl_scache_p == NULL) {
        return SOC_E_NONE;
    }
    sal_memcpy(mmu_traffic_ctrl, mmu_traffic_ctrl_scache_p,
               sizeof(_soc_td3_mmu_traffic_ctrl_t));
    mmu_traffic_ctrl_scache_p++;

    *scache_ptr = (uint8 *)mmu_traffic_ctrl_scache_p;

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_td3_port_thdo_rx_enable_set
 * Purpose:
 *     enable/disable the traffic to the port according to
 *     different callers.
 *  the traffic to the port can be enabled only if all of the callers
 *  are to enable it.
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) local port
 *     enable              - (IN) enable/disable the traffic to the port.
 *     flag                - (IN) different callers.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_soc_td3_port_thdo_rx_enable_set(int unit, soc_port_t port,
                                 int enable, uint8 flag)
{
    int   rv = SOC_E_NONE;
    uint8 *thdo_drop_bmp;
    _soc_td3_mmu_traffic_ctrl_t *mmu_traffic_ctrl;

    FWD_CTRL_LOCK(unit);

    mmu_traffic_ctrl = _soc_td3_mmu_traffic_ctrl[unit];
    if (mmu_traffic_ctrl ==  NULL) {
        FWD_CTRL_UNLOCK(unit);
        return SOC_E_INIT;
    }
    LOG_INFO(BSL_LS_SOC_MMU,
             (BSL_META_UP(unit, port,
                          "[SET]MMU Tx enable set: %d, flag: 0x%02x.\n"),
                          enable, flag));

    thdo_drop_bmp = &(mmu_traffic_ctrl->thdo_drop_bmp[port]);

    if (!enable) {
        if (*thdo_drop_bmp & flag) {
            /*self if already in thdo drop. No HW Op.*/
        } else {
            if (*thdo_drop_bmp == 0) {
                rv = _soc_td3_thdo_hw_set(unit, port, enable);
                if (SOC_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "Clear the registers fail.\n")));
                    FWD_CTRL_UNLOCK(unit);
                    return rv;
                }
            }
        }

        *thdo_drop_bmp |= flag;
    } else {
        if ((*thdo_drop_bmp != 0) && (*thdo_drop_bmp != flag)) {
            /*other callers are in thdo drop. No HW Op.*/
        } else {
            /*Ok, only self is in thdo drop, enbable it*/
            rv = _soc_td3_thdo_hw_set(unit, port, enable);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Set the registers fail.\n")));
                FWD_CTRL_UNLOCK(unit);
                return rv;
            }
        }

        *thdo_drop_bmp &= ~flag;
    }

    FWD_CTRL_UNLOCK(unit);

    LOG_INFO(BSL_LS_SOC_MMU,
             (BSL_META_UP(unit, port,
                          "[SET]MMU Tx drop bmp: 0x%02x.\n"),
                          *thdo_drop_bmp));
    return rv;
}

/*
 * Function:
 *     _soc_td3_port_thdo_rx_enable_get
 * Purpose:
 *     Get port traffic enqueue status.
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) local port
 *     enable              - (OUT) traffic to the port status.
 *                                 False if UC/MC is disabled.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_soc_td3_port_thdo_rx_enable_get(int unit, soc_port_t port,
                                 int *enable)
{
    int rv = SOC_E_NONE;

    rv = _soc_td3_thdo_hw_get(unit, port, enable);
    LOG_INFO(BSL_LS_SOC_MMU,
             (BSL_META_UP(unit, port,
                          "[GET]MMU Tx drop bmp: 0x%02x, enable status: %d.\n"),
                          _soc_td3_mmu_traffic_ctrl[unit]->thdo_drop_bmp[port],
                          *enable));

    return rv;
}

/*
 * Function:
 *     soc_td3_port_mmu_rx_enable_set
 * Purpose:
 *     enable/disable mmu rx traffic from the gport.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) PORT identifier
 *     enable     - (IN) enable/disable
 * Returns:
 *     BCM_E_XXX
 */
int
soc_td3_port_mmu_rx_enable_set(int unit, soc_port_t port,
                               int enable)
{
    uint32 rval;

    /* Disable/Enable INPUT_PORT_RX to the port */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, THDI_INPUT_PORT_XON_ENABLESr,
                       port, 0, &rval));

    if (!enable) {
        soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                          INPUT_PORT_RX_ENABLEf, 0);
    } else {
        soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                          INPUT_PORT_RX_ENABLEf, 1);
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, THDI_INPUT_PORT_XON_ENABLESr,
                       port, 0, rval));
    LOG_INFO(BSL_LS_SOC_MMU,
             (BSL_META_UP(unit, port,
                          "[SET]MMU Rx enable: %d.\n"),
                          enable));

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_td3_port_mmu_rx_enable_get
 * Purpose:
 *     Get status of mmu rx traffic from the gport.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) PORT identifier
 *     enable     - (OUT) enable/disable
 * Returns:
 *     BCM_E_XXX
 */
int
soc_td3_port_mmu_rx_enable_get(int unit, soc_port_t port,
                               int *enable)
{
    uint32 rval;

    /* Disable/Enable INPUT_PORT_RX to the port */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, THDI_INPUT_PORT_XON_ENABLESr,
                       port, 0, &rval));

    if (enable != NULL) {
        *enable = soc_reg_field_get(unit, THDI_INPUT_PORT_XON_ENABLESr, rval,
                                    INPUT_PORT_RX_ENABLEf);
        LOG_INFO(BSL_LS_SOC_MMU,
             (BSL_META_UP(unit, port,
                          "[GET]MMU Rx enable status: %d.\n"),
                          *enable));
    }

    return SOC_E_NONE;
}

int
soc_td3_port_traffic_egr_enable_set(int unit, soc_port_t port,
                               int enable)
{
    return _soc_td3_port_thdo_rx_enable_set(unit, port, enable,
                                            PORT_TX_EN_CTRL);
}

int
soc_td3_port_mmu_tx_enable_set(int unit, soc_port_t port,
                               int enable)
{
    return _soc_td3_port_thdo_rx_enable_set(unit, port, enable,
                                            MMU_TRAFFIC_EN_CTRL);
}

int
soc_td3_port_mmu_tx_enable_get(int unit, soc_port_t port,
                               int *enable)
{
    return _soc_td3_port_thdo_rx_enable_get(unit, port, enable);
}

void
soc_trident3_temperature_intr(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4)
{
    uint32 rval, trval;
    int i, rv, unit = PTR_TO_INT(unit_vp);
    const soc_reg_t *pvtmon_result_register=NULL;
    int num_entries_out;
#ifdef BCM_MAVERICK2_SUPPORT
    if (SOC_IS_MAVERICK2(unit)) {
        pvtmon_result_register = &pvtmon_result_reg_mv2[0];
        num_entries_out = COUNTOF(pvtmon_result_reg_mv2);
    } else
#endif
    {
        pvtmon_result_register = &pvtmon_result_reg[0];
        num_entries_out = COUNTOF(pvtmon_result_reg);
    }

    if ((rv = READ_TOP_PVTMON_INTR_STATUSr(unit, &rval)) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "TD3 Temp Intr, Reg access error.\n")));
    }
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "High temp interrupt: 0x%08x\n"), rval));
    /* Raise event to app for it to take further graceful actions */
    for (i = 0; i < num_entries_out; i++) {
        if (i & soc_td3_temp_mon_mask[unit]) {
            if ((rv = soc_reg32_get(unit, pvtmon_result_register[i], REG_PORT_ANY, 0,
                                    &trval)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TD3 Temp Intr, Reg access error.\n")));
            }
            trval = soc_reg_field_get(unit, pvtmon_result_register[i], trval,
                                      PVT_DATAf);
            /* Convert data to temperature.
             * temp = 434.1-(data*0.5350) = (4341000-(trval*5350))/1000;
             * Note: Since this is a high temp interrupt we can safely assume
             * that this will end up being a +ive value.
             */
            trval = (4341000-(trval*5350))/1000;

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TempMon %d: %d deg C.\n"), i, trval));
            (void)soc_event_generate(unit, SOC_SWITCH_EVENT_ALARM,
                                     SOC_SWITCH_EVENT_ALARM_HIGH_TEMP, i, trval);
        }
    }

    /* Acknowledge the interrupt - write 1 to clear latch bit */
    if ((rv = WRITE_TOP_PVTMON_INTR_STATUSr(unit, 0xFFFFFFFF)) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "TD3 Temp Intr, Reg access error.\n")));
    }

    /* Re-enable top lp interrupt, local PVT interrup is managed via intr mask */
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        (void)soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
    }
#endif

    /* TD3 has HW overtemp protection,  no shutdown logic needed. */
    return;
}

int
soc_trident3_temperature_monitor_get(int unit, int temperature_max,
    soc_switch_temperature_monitor_t *temperature_array, int *temperature_count)
{
    soc_reg_t reg;
    int index;
    uint32 rval;
    int fval, cur, peak;
    int num_entries_out;
    const soc_reg_t *pvtmon_result_register=NULL;
#ifdef BCM_MAVERICK2_SUPPORT
    if (SOC_IS_MAVERICK2(unit)) {
        pvtmon_result_register = &pvtmon_result_reg_mv2[0];
        if (COUNTOF(pvtmon_result_reg_mv2) > temperature_max) {
            num_entries_out = temperature_max;
        } else {
            num_entries_out = COUNTOF(pvtmon_result_reg_mv2);
        }
    } else
#endif
    {
        pvtmon_result_register = &pvtmon_result_reg[0];
        if (COUNTOF(pvtmon_result_reg) > temperature_max) {
            num_entries_out = temperature_max;
        } else {
            num_entries_out = COUNTOF(pvtmon_result_reg);
        }
    }
    if (temperature_count) {
        *temperature_count = 0;
    }
    /* SDK-125480 temperature reading for VTMON 11
     *  Assert TOP_AVS_RST_L.
     */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_AVS_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_3r(unit, &rval));
    if ((soc_reg_field_get(unit, TOP_PVTMON_CTRL_3r, rval, PVTMON_RESET_Nf) == 0) ||
        (soc_reg_field_get(unit, TOP_PVTMON_CTRL_3r, rval, PVTMON_SELECTf) != 0)) {
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_3r, &rval, PVTMON_RESET_Nf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_3r(unit, rval));
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_3r, &rval, PVTMON_RESET_Nf, 1);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_3r, &rval, PVTMON_SELECTf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_3r(unit, rval));
        sal_usleep(1000);
    }

    for (index = 0; index < num_entries_out; index++) {
        reg = pvtmon_result_register[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

        fval = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
        cur = (4341000-(fval*5350))/1000;

        fval = soc_reg_field_get(unit, reg, rval, MIN_PVT_DATAf);
        peak = (4341000-(fval*5350))/1000;

        if (temperature_array) {
            (temperature_array + index)->curr = cur;
            (temperature_array + index)->peak = peak;
        }
    }

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MIN_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MIN_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    if (temperature_count) {
        *temperature_count=num_entries_out;
    }
    return SOC_E_NONE;
}

int
soc_trident3_show_ring_osc(int unit)
{
    static const struct {
        int osc_sel;
        soc_field_t field0;
        int value0;
        soc_field_t field1;
        int value1;
        char *name;
    } osc_tbl[] = {
        { 0, OSC_0_SELf, 0, INVALIDf, -1, "Core ring 0 HVT min" },
        { 0, OSC_0_SELf, 1, INVALIDf, -1, "Core ring 0 HVT mid" },
        { 0, OSC_0_SELf, 2, INVALIDf, -1, "Core ring 0 HVT max" },
        { 0, OSC_0_SELf, 3, INVALIDf, -1, "Core ring 0 SVT min" },
        { 1, OSC_1_SELf, 0, INVALIDf, -1, "Core ring 1 HVT min" },
        { 1, OSC_1_SELf, 1, INVALIDf, -1, "Core ring 1 HVT mid" },
        { 1, OSC_1_SELf, 2, INVALIDf, -1, "Core ring 1 HVT max" },
        { 1, OSC_1_SELf, 3, INVALIDf, -1, "Core ring 1 SVT min" },
        { 0, IROSC_SELf, 0, INVALIDf, -1, "Core ring 0 five stages" },
        { 0, IROSC_SELf, 1, INVALIDf, -1, "Core ring 0 nine stages" },
        { 1, IROSC_SELf, 0, INVALIDf, -1, "Core ring 1 five stages" },
        { 1, IROSC_SELf, 1, INVALIDf, -1, "Core ring 1 nine stages" },
        { 2, SRAM_OSC_0_PENf, 0, SRAM_OSC_0_NENf, 1, "SRAM ring 0 NMOS" },
        { 3, SRAM_OSC_0_PENf, 1, SRAM_OSC_0_NENf, 0, "SRAM ring 0 PMOS" },
        { 4, SRAM_OSC_1_PENf, 0, SRAM_OSC_1_NENf, 1, "SRAM ring 1 NMOS" },
        { 5, SRAM_OSC_1_PENf, 1, SRAM_OSC_1_NENf, 0, "SRAM ring 1 PMOS" },
        { 6, SRAM_OSC_2_PENf, 0, SRAM_OSC_2_NENf, 1, "SRAM ring 2 NMOS" },
        { 7, SRAM_OSC_2_PENf, 1, SRAM_OSC_2_NENf, 0, "SRAM ring 2 PMOS" },
        { 8, SRAM_OSC_3_PENf, 0, SRAM_OSC_3_NENf, 1, "SRAM ring 3 NMOS" },
        { 9, SRAM_OSC_3_PENf, 1, SRAM_OSC_3_NENf, 0, "SRAM ring 3 PMOS" },
    };
    soc_reg_t ctrl_reg, stat_reg;
    uint32 rval, fval;
    int index, core_clk, quo, rem, frac, retry;

    core_clk = SOC_INFO(unit).frequency * 1024;
    ctrl_reg = TOP_RING_OSC_CTRLr;
    stat_reg = TOP_OSC_COUNT_STATr;

    for (index = 0; index < COUNTOF(osc_tbl); index++) {
        rval = 0;
        /*
         * set OSC_CNT_RSTBf to 0 to do softreset
         * set OSC_CNT_START to 0 to hold the counter until it selects
         * the input signal
         */
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));
        soc_reg_field_set(unit, ctrl_reg, &rval, OSC_ENABLEf, 1);
        soc_reg_field_set(unit, ctrl_reg, &rval, IROSC_ENf, 1);
        soc_reg_field_set(unit, ctrl_reg, &rval, osc_tbl[index].field0,
                          osc_tbl[index].value0);
        if (osc_tbl[index].field1 != INVALIDf) {
            soc_reg_field_set(unit, ctrl_reg, &rval, osc_tbl[index].field1,
                              osc_tbl[index].value1);
        }
        soc_reg_field_set(unit, ctrl_reg, &rval, OSC_SELf,
                          osc_tbl[index].osc_sel);
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));
        soc_reg_field_set(unit, ctrl_reg, &rval, OSC_CNT_RSTBf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));
        soc_reg_field_set(unit, ctrl_reg, &rval, OSC_CNT_STARTf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));

        for (retry = 0; retry < 10; retry++) {
            sal_usleep(1000);
            SOC_IF_ERROR_RETURN(READ_TOP_OSC_COUNT_STATr(unit, &rval));
            if (!soc_reg_field_get(unit, stat_reg, rval, OSC_CNT_DONEf)) {
                continue;
            }

            fval = soc_reg_field_get(unit, stat_reg, rval, OSC_CNT_VALUEf);
            quo = core_clk / fval;
            rem = core_clk - quo * fval;
            frac = (rem * 10000) / fval;
            LOG_CLI((BSL_META_U(unit,
                                "%s: %d.%04d Mhz\n"),
                     osc_tbl[index].name, quo, frac));
            break;
        }
    }

    return SOC_E_NONE;
}

STATIC _soc_td3_bst_hw_cb td3_bst_cb;
extern int bst_td3_cfap_trigger_status[SOC_MAX_NUM_DEVICES];
int
soc_td3_process_mmu_bst(int unit, int xpe, soc_field_t field)
{
    int rv = SOC_E_NONE;
    if (field == BST_CFAP_INT_STATf) {
        bst_td3_cfap_trigger_status[unit] = 1;
    }
    if (td3_bst_cb != NULL) {
        rv = td3_bst_cb(unit);
    }
    return rv;
}

int soc_td3_set_bst_callback(int unit, _soc_td3_bst_hw_cb cb)
{
    td3_bst_cb = cb;
    return SOC_E_NONE;
}

STATIC _soc_td3_tcb_hw_cb td3_tcb_cb;

int
soc_td3_process_mmu_tcb(int unit, int xpe, soc_field_t field)
{
    int rv = SOC_E_NONE;
    if (td3_tcb_cb != NULL) {
        rv = td3_tcb_cb(unit);
    }
    return rv;
}

int soc_td3_set_tcb_callback(int unit, _soc_td3_tcb_hw_cb cb)
{
    td3_tcb_cb = cb;
    return SOC_E_NONE;
}

void
soc_trident3_process_func_intr(void *unit_vp, void *d1, void *d2, void *d3, void *d4)
{
    int rv = SOC_E_NONE;
    int unit = PTR_TO_INT(unit_vp);
    uint32 rval;
    char       prefix_str[10];
    sal_sprintf(prefix_str, "Unit: %d \n", unit);

    rv = READ_L2_MGMT_INTRr(unit, &rval);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Error reading %s reg !!\n"),
                              SOC_REG_NAME(unit, L2_MGMT_INTRr)));
        return;
    }

    if (soc_reg_field_get(unit, L2_MGMT_INTRr, rval, SER_FIFO_NOT_EMPTYf)) {
        (void) soc_trident3_process_ser_fifo(unit, SOC_BLK_IPIPE,
                                             0, /* pipe */
                                             prefix_str,
                                             1); /* l2_mgmt_ser_fifo */
    }
    if (soc_reg_field_get(unit, L2_MGMT_INTRr, rval, L2_LEARN_INSERT_FAILUREf)) {
        soc_td2_l2_overflow_interrupt_handler(unit);
    }

    sal_usleep(SAL_BOOT_QUICKTURN ? 100000 : 1000); /* Don't reenable too soon */
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        (void)soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
    }
#endif
}


/*
 * All non-parity related interrupts must be handled in this function.
 * All unserviced intr_status bits must either be serviced or disabled.
 *
 * Note:
 * 1. MEM_PAR_ERR_STATf is handled by trident3/ser.c, so ignore this
 * 2. blocktype (in) can only be SOC_BLK_MMU_XPE/SC/SED
 */
int
soc_td3_mmu_non_ser_intr_handler(int unit, soc_block_t blocktype,
                                int mmu_base_index,
                                uint32 rval_intr_status_reg)
{
    /*This need each other block to implement the non-ser interrupt handler */
    int i;
    uint32 rval = 0;
    soc_reg_t intr_stat_reg, intr_clr_reg;

    static soc_field_t xpe_intr_field_list[] = {
        MEM_PAR_ERR_STATf, /* handled by trident3/ser.c, so ignore this */
        BST_THDI_INT_STATf,
        BST_THDO_INT_STATf,
        DEQ0_NOT_IP_ERR_STATf,
        DEQ1_NOT_IP_ERR_STATf,
        ENQX_TO_CFG_MAX_CELL_INTR_0_STATf,
        ENQX_TO_CFG_MAX_CELL_INTR_1_STATf,
        TCB_FREEZE_INT_STATf,
        INVALIDf
    };

    static soc_field_t sed_intr_field_list[] = {
        CFAP_MEM_FAIL_STATf,
        MEM_PAR_ERR_STATf,
        BST_CFAP_INT_STATf,
        START_BY_START_ERR_STATf,
        INVALIDf
    };

    if (blocktype == SOC_BLK_MMU_XPE) {
        intr_stat_reg = MMU_XCFG_XPE_CPU_INT_STATr;
        intr_clr_reg = MMU_XCFG_XPE_CPU_INT_CLEARr;
        for (i = 0; xpe_intr_field_list[i] != INVALIDf; i++) {
            if (!soc_reg_field_valid(unit, intr_stat_reg, xpe_intr_field_list[i])) {
                continue;
            }
            if (!soc_reg_field_get(unit, intr_stat_reg,
                                   rval_intr_status_reg,
                                   xpe_intr_field_list[i])) {
                continue;
            }
            switch (xpe_intr_field_list[i]) {
            case BST_THDI_INT_STATf:
            case BST_THDO_INT_STATf:
                /* interrupt processing */
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Serviced %s intr "
                                              "from xpe = %0d \n"),
                             unit, SOC_FIELD_NAME(unit, xpe_intr_field_list[i]),
                             mmu_base_index));

                /* clear the interrupt */
                SOC_IF_ERROR_RETURN
                    (soc_trident3_xpe_reg32_get(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, &rval));
                rval |= (1 << i);
                SOC_IF_ERROR_RETURN
                    (soc_trident3_xpe_reg32_set(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, rval));
                SOC_IF_ERROR_RETURN
                    (soc_td3_process_mmu_bst(unit, mmu_base_index,
                                             xpe_intr_field_list[i]));
                break;
            case TCB_FREEZE_INT_STATf:
                /* interrupt processing */
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Serviced %s intr "
                                              "from xpe = %0d \n"),
                             unit, SOC_FIELD_NAME(unit, xpe_intr_field_list[i]),
                             mmu_base_index));

                /* clear the interrupt */
                SOC_IF_ERROR_RETURN
                    (soc_trident3_xpe_reg32_get(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, &rval));
                rval |= (1 << i);
                SOC_IF_ERROR_RETURN
                    (soc_trident3_xpe_reg32_set(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, rval));
                SOC_IF_ERROR_RETURN
                    (soc_td3_process_mmu_tcb(unit, mmu_base_index,
                                             xpe_intr_field_list[i]));
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "Unit: %0d -- Could not service %s "
                                            "intr from xpe = %0d \n"),
                           unit, SOC_FIELD_NAME(unit, xpe_intr_field_list[i]),
                           mmu_base_index));
                break;
            }
        }
        return SOC_E_NONE;
    }

    if (blocktype == SOC_BLK_MMU_SED) {
        intr_stat_reg = MMU_SEDCFG_SED_CPU_INT_STATr;
        intr_clr_reg = MMU_SEDCFG_SED_CPU_INT_CLEARr;
        for (i = 0; sed_intr_field_list[i] != INVALIDf; i++) {
            if (!soc_reg_field_valid(unit, intr_stat_reg, sed_intr_field_list[i])) {
                continue;
            }
            if (!soc_reg_field_get(unit, intr_stat_reg,
                                   rval_intr_status_reg,
                                   sed_intr_field_list[i])) {
                continue;
            }
            switch (sed_intr_field_list[i]) {
            case BST_CFAP_INT_STATf:
                /* interrupt processing */
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Serviced %s intr "
                                              "from sed = %0d \n"),
                             unit, SOC_FIELD_NAME(unit, sed_intr_field_list[i]),
                             mmu_base_index));

                /* clear the interrupt */
                SOC_IF_ERROR_RETURN
                    (soc_trident3_sed_reg32_get(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, &rval));
                rval |= (1 << i);
                SOC_IF_ERROR_RETURN
                    (soc_trident3_sed_reg32_set(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, rval));
                SOC_IF_ERROR_RETURN
                    (soc_td3_process_mmu_bst(unit, mmu_base_index,
                                             sed_intr_field_list[i]));
                break;
            case CFAP_MEM_FAIL_STATf:
                /* interrupt processing */
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Serviced %s intr "
                                              "from sed = %0d \n"),
                             unit, SOC_FIELD_NAME(unit, sed_intr_field_list[i]),
                             mmu_base_index));

                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_CFAP_MEM_FAIL, -1, -1);

                /* clear the interrupt */
                SOC_IF_ERROR_RETURN
                    (soc_trident3_sed_reg32_get(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, &rval));
                rval |= (1 << i);
                SOC_IF_ERROR_RETURN
                    (soc_trident3_sed_reg32_set(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, rval));
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "Unit: %0d -- Could not service %s "
                                            "intr from sed = %0d \n"),
                           unit, SOC_FIELD_NAME(unit, sed_intr_field_list[i]),
                           mmu_base_index));
                break;
            }
        }
        return SOC_E_NONE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td3_subport_init
 * Purpose:
 *      Setup the pbm fields from config variables for the COE application
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      NONE
 */
void soc_td3_subport_init(int unit)
{
    soc_port_t  port;
    soc_info_t *si=&SOC_INFO(unit);
    soc_pbmp_t  pbmp_subport;
    int         num_subport = 0;

    SOC_PBMP_CLEAR(si->subtag_pbm);
    SOC_PBMP_CLEAR(si->subtag_allowed_pbm);
    SOC_PBMP_CLEAR(si->subtag.bitmap);

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        si->port_num_subport[port] = 0;
    }

    pbmp_subport = soc_property_get_pbmp(unit, spn_PBMP_SUBPORT, 0);

    SOC_PBMP_ASSIGN(si->subtag_allowed_pbm, pbmp_subport);

    if (SOC_PBMP_NOT_NULL(si->subtag_allowed_pbm)) {
        /* Iterate through SubTag ports and reserve subport indices */
        SOC_PBMP_ITER(si->subtag_allowed_pbm, port) {

            num_subport =
                soc_property_port_get(unit, port, spn_NUM_SUBPORTS, 0);

            if (num_subport > TD3_MAX_SUBPORT_COE_PORT_PER_CASPORT) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                              "\nCONFIG ERROR\n"
                              "num_subports_%d on port %d exceed max value,"
                              "setting it to the max allowed value %d\n\n"),
                              num_subport, port,
                              TD3_MAX_SUBPORT_COE_PORT_PER_CASPORT));
                num_subport = TD3_MAX_SUBPORT_COE_PORT_PER_CASPORT;
            }

            if (port < SOC_MAX_NUM_PORTS) {
                si->port_num_subport[port] = num_subport;

            }
        }
    }

    /* Initialize the number of COE modules */
    si->num_coe_modules = TD3_MAX_COE_MODULES;
}

soc_pstats_tbl_desc_t
_soc_td3_pstats_tbl_desc_all = {SOC_BLK_NONE, INVALIDm, FALSE, FALSE};
/* pstats support */
soc_pstats_tbl_desc_t _soc_td3_pstats_tbl_desc[] = {
    /* List all the potential memories */
#ifdef BCM_PSTATS_MEASURE
    {
        SOC_BLK_MMU_GLB,
        INVALIDm,
        FALSE, FALSE,
        {
            {MMU_INTFO_TIMESTAMPm},
            {INVALIDm}
        }
    },
#endif
    {
        SOC_BLK_MMU_GLB,
        INVALIDm,
        FALSE, FALSE,
        {
            {MMU_INTFO_UTC_TIMESTAMPm},
            {INVALIDm}
        }
    },
    {
        SOC_BLK_MMU_XPE,
        MMU_THDU_UCQ_STATS_TABLEm,
        TRUE, TRUE
    },
    {
        SOC_BLK_MMU_XPE,
        THDI_PKT_STAT_SP_SHARED_COUNTm,
        FALSE, FALSE
    },
    {
        SOC_BLK_MMU_XPE,
        MMU_THDM_DB_POOL_MCUC_PKSTATm,
        FALSE, FALSE
#ifdef BCM_PSTATS_MEASURE
    },
    {
        SOC_BLK_MMU_GLB,
        INVALIDm,
        FALSE, FALSE,
        {
            {MMU_INTFO_TIMESTAMPm},
            {INVALIDm}
        }
#endif
    }
};

void
soc_trident3_mmu_pstats_tbl_config_all(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_mem_desc_t *desc, *desc_all;
    int ti, mi, di = 0;

    desc_all = _soc_td3_pstats_tbl_desc_all.desc;
    /* Check and strip invalid memories */
    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        mi = 0;
        desc = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].desc;
        while (desc[mi].mem != INVALIDm) {
            desc_all[di] = desc[mi];
            di++;
            mi++;
        }
    }
    desc_all[di].mem = INVALIDm;

    soc->pstats_tbl_desc = &_soc_td3_pstats_tbl_desc_all;
    soc->pstats_tbl_desc_count = 1;
}

void
soc_trident3_mmu_pstats_tbl_config(int unit)
{

    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_mem_desc_t *desc;
    soc_mem_t bmem;
    int pe, mor_dma;
    int ti, mi, xpe, pipe;

    soc->pstats_tbl_desc = _soc_td3_pstats_tbl_desc;
    soc->pstats_tbl_desc_count = COUNTOF(_soc_td3_pstats_tbl_desc);
    soc->pstats_mode = PSTATS_MODE_NULL;

    /* Check and strip invalid memories */
    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        mi = 0;
        desc = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].desc;
        bmem = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].bmem;
        if (bmem == INVALIDm) {
            while (desc[mi].mem != INVALIDm) {
                desc[mi].width = soc_mem_entry_words(unit, desc[mi].mem);
                desc[mi].entries = soc_mem_index_count(unit, desc[mi].mem);
                mi++;
            }
            continue;
        }
        pe = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].pipe_enum;
        mor_dma = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].mor_dma;
        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                desc[mi].mem = pe ? SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, bmem, xpe, pipe) :
                                    SOC_MEM_UNIQUE_ACC(unit, bmem)[xpe];
                if (desc[mi].mem == INVALIDm) {
                    continue;
                }
                desc[mi].width = soc_mem_entry_words(unit, desc[mi].mem);
                desc[mi].entries = soc_mem_index_count(unit, desc[mi].mem);
                if (mor_dma) {
                    desc[mi].mor_dma = TRUE;
                }
                mi++;
                if (!pe) {
                    break;
                }
            }
        }
        desc[mi].mem = INVALIDm;
    }

    if (soc_property_get(unit, "pstats_desc_all", 1)) {
        /* Use single desc chain */
        soc_trident3_mmu_pstats_tbl_config_all(unit);
    }
}

int
soc_trident3_mmu_pstats_mode_set(int unit, uint32 flags)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int pf = 0;
    uint32 rval = 0;
    int rv;

    SOC_PSTATS_LOCK(unit);

    if (flags & _TD3_MMU_PSTATS_HWM_MOD) {
        soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval,
                          HWM_MODEf, 1);
        pf |= _TD3_MMU_PSTATS_HWM_MOD;
        if (flags & _TD3_MMU_PSTATS_RESET_ON_READ) {
            soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval,
                              HWM_RESET_ON_READf, 1);
            pf |= _TD3_MMU_PSTATS_RESET_ON_READ;
        }
    }

    if (flags & _TD3_MMU_PSTATS_PKT_MOD) {
        if (SOC_REG_FIELD_VALID(unit, MMU_GCFG_PKTSTAT_OOBSTATr, SELECTf)) {
            soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval, SELECTf, 1);
        }
        pf |= _TD3_MMU_PSTATS_PKT_MOD;
        if (soc_feature(unit, soc_feature_mor_dma) &&
            (flags & _TD3_MMU_PSTATS_MOR_EN)) {
            soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval,
                              MOR_ENf, 1);
        }
        if (flags & _TD3_MMU_PSTATS_MOR_EN) {
            pf |= _TD3_MMU_PSTATS_MOR_EN;
        }
    }

    if (flags & _TD3_MMU_PSTATS_ENABLE) {
        soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval,
                          ENABLEf, 1);
        pf |= _TD3_MMU_PSTATS_ENABLE;
        if (flags & _TD3_MMU_PSTATS_PKT_MOD) {
            if ((flags & _TD3_MMU_PSTATS_SYNC) ||
                (flags & _TD3_MMU_PSTATS_HWM_MOD &&
                 flags & _TD3_MMU_PSTATS_RESET_ON_READ)) {
                soc->pstats_mode = PSTATS_MODE_PKT_SYNC;
                pf |= _TD3_MMU_PSTATS_SYNC;
            } else {
                soc->pstats_mode = PSTATS_MODE_PKT_BUFF;
            }
        } else {
            soc->pstats_mode = PSTATS_MODE_OOB;
        }
    } else {
        soc->pstats_mode = PSTATS_MODE_NULL;
    }

    soc->pstats_flags = pf;

    rv = WRITE_MMU_GCFG_PKTSTAT_OOBSTATr(unit, rval);

    SOC_PSTATS_UNLOCK(unit);

    return rv;
}

int
soc_trident3_mmu_pstats_mode_get(int unit, uint32 *flags)
{
    int pf = 0;
    uint32 rval = 0;
    int rv;
    soc_reg_t reg = MMU_GCFG_PKTSTAT_OOBSTATr;

    SOC_PSTATS_LOCK(unit);

    rv = READ_MMU_GCFG_PKTSTAT_OOBSTATr(unit, &rval);
    if (SOC_FAILURE(rv)) {
        SOC_PSTATS_UNLOCK(unit);
        return rv;
    }

    if (soc_reg_field_get(unit, reg, rval, ENABLEf)) {
        pf |= _TD3_MMU_PSTATS_ENABLE;
        if (SOC_REG_FIELD_VALID(unit, reg, SELECTf)) {
            if (soc_reg_field_get(unit, reg, rval, SELECTf)) {
                pf |= _TD3_MMU_PSTATS_PKT_MOD;
            }
        } else {
            pf |= _TD3_MMU_PSTATS_PKT_MOD;
        }
    }
    if (soc_reg_field_get(unit, reg, rval, HWM_MODEf)) {
        pf |= _TD3_MMU_PSTATS_HWM_MOD;
        if (soc_reg_field_get(unit, reg, rval, HWM_RESET_ON_READf)) {
            pf |= _TD3_MMU_PSTATS_RESET_ON_READ;
        }
    }

    SOC_PSTATS_UNLOCK(unit);

    *flags = pf;

    return SOC_E_NONE;
}

int
soc_trident3_mmu_pstats_mor_enable(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 rval;


    if (soc->pstats_flags & _TD3_MMU_PSTATS_MOR_EN) {
        SOC_IF_ERROR_RETURN(READ_MMU_GCFG_PKTSTAT_OOBSTATr(unit, &rval));
        if ( ! soc_reg_field_get(unit,
                                 MMU_GCFG_PKTSTAT_OOBSTATr, rval, MOR_ENf)) {
            soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval, MOR_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_PKTSTAT_OOBSTATr(unit, rval));
        }
    }

    return SOC_E_NONE;
}

int
soc_trident3_mmu_pstats_mor_disable(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 rval;

    if (!soc_feature(unit, soc_feature_mor_dma) &&
        (soc->pstats_flags & _TD3_MMU_PSTATS_MOR_EN)) {
        SOC_IF_ERROR_RETURN(READ_MMU_GCFG_PKTSTAT_OOBSTATr(unit, &rval));
        soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval, MOR_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_PKTSTAT_OOBSTATr(unit, rval));
    }

    return SOC_E_NONE;
}

int
soc_trident3_mmu_splitter_reset(int unit)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_MMU_GCFG_SPLITTERr(unit, &rval));
    soc_reg_field_set(unit, MMU_GCFG_SPLITTERr, &rval, RESET_SBUSf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_SPLITTERr(unit, rval));

    return SOC_E_NONE;
}
/*
 * Function:
 *      soc_td3_port_schedule_tdm_init
 * Purpose:
 *      Initialize TDM information in port_schedule_state that will be passed
 *      to DV FlexPort API
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (OUT) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      If called during Scheduler Initialization, only below values need to be
 *      properly set:
 *        soc_tdm_schedule_pipe_t::num_slices
 *        soc_tdm_schedule_t:: cal_len
 *        soc_tdm_schedule_t:: num_ovs_groups
 *        soc_tdm_schedule_t:: ovs_group_len
 *        soc_tdm_schedule_t:: num_pkt_sch_or_ovs_space
 *        soc_tdm_schedule_t:: pkt_sch_or_ovs_space_len
 */
int
soc_td3_port_schedule_tdm_init(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    _soc_trident3_tdm_temp_t *tdm = soc->tdm_info;
    /*soc_info_t *si = &SOC_INFO(unit);*/
    soc_tdm_schedule_pipe_t *tdm_ischd, *tdm_eschd;
    soc_tdm_schedule_t *sched;
    int pipe, hpipe, phy_port, is_flexport, group, slot;
    int *port_p2PBLK_inst_mapping;

    if (tdm == NULL) {
        return SOC_E_INIT;
    }

    is_flexport = port_schedule_state->is_flexport;
	/*
    port_schedule_state->calendar_type = si->fabric_port_enable ?
                                        _TD3_TDM_CALENDAR_UNIVERSAL :
                                        _TD3_TDM_CALENDAR_ETHERNET_OPTIMIZED;
	*/
    /* Construct tdm_ingress_schedule_pipe and tdm_egress_schedule_pipe */
    for (pipe = 0; pipe <  TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        tdm_eschd = &port_schedule_state->tdm_egress_schedule_pipe[pipe];

        tdm_ischd->num_slices = TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE;
        tdm_eschd->num_slices = TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE;

        if (is_flexport) {
            /* TDM Calendar is always in slice 0 */
            sal_memcpy(tdm_ischd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].idb_tdm,
                        sizeof(int)*_TD3_TDM_LENGTH);
            sal_memcpy(tdm_eschd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].mmu_tdm,
                        sizeof(int)*_TD3_TDM_LENGTH);
        }

        /* OVS */
        for (hpipe = 0; hpipe < TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            /* IDB OVERSUB*/
            sched = &tdm_ischd->tdm_schedule_slice[hpipe];
            sched->cal_len = _TD3_TDM_LENGTH;
            sched->num_ovs_groups = TRIDENT3_TDM_OVS_GROUPS_PER_HPIPE;
            sched->ovs_group_len = TRIDENT3_TDM_OVS_GROUP_LENGTH;
            sched->num_pkt_sch_or_ovs_space = 1;
            sched->pkt_sch_or_ovs_space_len = _TD3_PKT_SCH_LENGTH;

            if (is_flexport) {
                for (group = 0; group < TRIDENT3_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                    for (slot = 0; slot < TRIDENT3_TDM_OVS_GROUP_LENGTH; slot++) {
                        sched->oversub_schedule[group][slot] =
                            tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot];
                    }
                }
                for (slot = 0; slot < _TD3_PKT_SCH_LENGTH; slot++) {
                    sched->pkt_sch_or_ovs_space[0][slot] =
                        tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot];
                }
            }

            /* MMU OVERSUB */
            sched = &tdm_eschd->tdm_schedule_slice[hpipe];
            sched->cal_len = _TD3_TDM_LENGTH;
            sched->num_ovs_groups = TRIDENT3_TDM_OVS_GROUPS_PER_HPIPE;
            sched->ovs_group_len = TRIDENT3_TDM_OVS_GROUP_LENGTH;
            sched->num_pkt_sch_or_ovs_space = 1;
            sched->pkt_sch_or_ovs_space_len = _TD3_PKT_SCH_LENGTH;

            if (is_flexport) {
                for (group = 0; group < TRIDENT3_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                    for (slot = 0; slot < TRIDENT3_TDM_OVS_GROUP_LENGTH; slot++) {
                        sched->oversub_schedule[group][slot] =
                            tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot];
                        }
                    }
                for (slot = 0; slot < _TD3_PKT_SCH_LENGTH; slot++) {
                    sched->pkt_sch_or_ovs_space[0][slot] =
                        tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot];
                }
            }
        }
    }

    if (is_flexport) {
        port_p2PBLK_inst_mapping =
            port_schedule_state->in_port_map.port_p2PBLK_inst_mapping;
        /* pblk info for phy_port */
        for (phy_port = 1; phy_port < TD3_NUM_EXT_PORTS; phy_port++) {
            if (tdm->pblk_info[phy_port].pblk_cal_idx == -1) {
                continue;
            }
            port_p2PBLK_inst_mapping[phy_port] =
                                        tdm->pblk_info[phy_port].pblk_cal_idx;
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td3_soc_tdm_update
 * Purpose:
 *      Update TDM information in SOC with TDM state return from DV FlexPort API
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      port_schedule_state  - (OUT) Port resource state.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
soc_td3_soc_tdm_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_info_t *si = &SOC_INFO(unit);
    _soc_trident3_tdm_temp_t *tdm = soc->tdm_info;
    soc_tdm_schedule_pipe_t *tdm_ischd, *tdm_eschd;
    soc_tdm_schedule_t *sched;
    int pipe, hpipe, port, phy_port, clport, slot;
    int length, ovs_core_slot_count, ovs_io_slot_count, port_slot_count;
    int index, group;

    #define LBPORT0 65
    #define LBPORT1 131

    if (tdm == NULL) {
        return SOC_E_INIT;
    }

    /* Copy info from soc_port_schedule_state_t to _soc_trident3_tdm_temp_t */
    for (pipe = 0; pipe <  TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        tdm_eschd = &port_schedule_state->tdm_egress_schedule_pipe[pipe];

        /* TDM Calendar is always in slice 0 */
        sal_memcpy(tdm->tdm_pipe[pipe].idb_tdm,
                    tdm_ischd->tdm_schedule_slice[0].linerate_schedule,
                    sizeof(int)*_TD3_TDM_LENGTH);
        sal_memcpy(tdm->tdm_pipe[pipe].mmu_tdm,
                    tdm_eschd->tdm_schedule_slice[0].linerate_schedule,
                    sizeof(int)*_TD3_TDM_LENGTH);

        for (hpipe = 0; hpipe < TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            sched = &tdm_ischd->tdm_schedule_slice[hpipe];
            for (group = 0; group < TRIDENT3_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                for (slot = 0; slot < TRIDENT3_TDM_OVS_GROUP_LENGTH; slot++) {
                    tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot] =
                        sched->oversub_schedule[group][slot];
                }
            }
            for (slot = 0; slot < _TD3_PKT_SCH_LENGTH; slot++) {
                tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot] =
                    sched->pkt_sch_or_ovs_space[0][slot];

            }
        }
    }

    /* pblk info init  */
    for (phy_port = 1; phy_port < TD3_NUM_EXT_PORTS; phy_port++) {
        tdm->pblk_info[phy_port].pblk_cal_idx = -1;
        tdm->pblk_info[phy_port].pblk_hpipe_num = -1;
    }

    /* pblk info for phy_port */
    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        for (hpipe = 0; hpipe < TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            for (group = 0; group < TRIDENT3_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                for (index = 0; index < TRIDENT3_TDM_OVS_GROUP_LENGTH; index++) {
                    phy_port =
                        tdm_ischd->tdm_schedule_slice[hpipe].oversub_schedule[group][index];
                    if (phy_port < TD3_NUM_EXT_PORTS) {
                        tdm->pblk_info[phy_port].pblk_hpipe_num = hpipe;
                        tdm->pblk_info[phy_port].pblk_cal_idx =
                            port_schedule_state->out_port_map.port_p2PBLK_inst_mapping[phy_port];
                    }
                }
            }
        }
    }

    /* CLPORT port ratio */
    for (clport = 0; clport < TRIDENT3_TDM_PBLKS_PER_DEV; clport++) {
        soc_td3_tdm_get_port_ratio(unit, port_schedule_state, clport,
            &tdm->port_ratio[clport], 1);
    }

    /* Calculate the oversub ratio */
    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        /* Count number of OVSB_TOKEN assigned by the TDM code */
        for (length = _TD3_TDM_LENGTH; length > 0; length--) {
            if (tdm_ischd->tdm_schedule_slice[0].linerate_schedule[length - 1]
                    != TD3_NUM_EXT_PORTS) {
                break;
            }
        }
        ovs_core_slot_count = 0;
        for (index = 0; index < length; index++) {
            if (tdm_ischd->tdm_schedule_slice[0].linerate_schedule[index]
                    == TD3_OVSB_TOKEN) {
                ovs_core_slot_count++;
            }
        }

        /* Count number of slot needed for half-pipe's oversub I/O bandwidth */
        for (hpipe = 0; hpipe < TRIDENT3_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            ovs_io_slot_count = 0;
            for (group = 0; group < TRIDENT3_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                for (index = 0; index < TRIDENT3_TDM_OVS_GROUP_LENGTH; index++) {
                    phy_port = tdm_ischd->tdm_schedule_slice[hpipe].oversub_schedule[group][index];
                    if (phy_port == TD3_NUM_EXT_PORTS) {
                        continue;
                    }
                    port = si->port_p2l_mapping[phy_port];
                    /* Dedicated LB ports should use AUX bw, so flag the error. */
                    if ((port == -1) || IS_CPU_PORT(unit, port) ||
                        ((IS_LB_PORT(unit, port) &&
                              ((port == LBPORT0) || (port == LBPORT1)))) ||
                        (phy_port ==TRIDENT3_TDM_PHY_MGMT0)) {
                        LOG_ERROR(BSL_LS_SOC_PORT,
                                  (BSL_META_U(unit,
                                              "Flexport: Invalid physical "
                                              "port %d in OverSub table.\n"),
                                   phy_port));
                        continue;
                    }
                    port_slot_count = si->port_speed_max[port] / 2500;
                    ovs_io_slot_count += port_slot_count;
                }
            }
            if (ovs_core_slot_count != 0) {
                tdm->ovs_ratio_x1000[pipe][hpipe] =
                    ovs_io_slot_count * 1000 / (ovs_core_slot_count / 2);
            }
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_trident3_mmu_port_empty_check(int unit, soc_port_t port, int *empty)
{
    soc_reg_t status_reg[2] = {Q_SCHED_PORT_EMPTY_STATUS_SPLIT0r,
                               Q_SCHED_PORT_EMPTY_STATUS_SPLIT1r};
    soc_reg_t reg;
    int pipe, split, pos;
    uint64 rval64;

    SOC_IF_ERROR_RETURN(
        soc_td3_mmu_bmp_reg_pos_get(unit, port, &pipe, &split, &pos));
    reg = status_reg[split];

    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, pipe, 0, &rval64));

    if (pos < 32) {
        *empty = COMPILER_64_LO(rval64) & (1 << pos) ? TRUE : FALSE;
    } else {
        *empty = COMPILER_64_HI(rval64) & (1 << (pos - 32)) ?
                 TRUE : FALSE;
    }

    return SOC_E_NONE;
}

int
soc_trident3_mmu_port_flush_hw(int unit, soc_port_t port, uint32 drain_timeout)
{
    uint64 rval64;
    uint64 pbits64, mtro64;
    uint32 rval;
    int pipe, split;
    int empty, pos;
    soc_timeout_t to;
    soc_reg_t flush_reg, mtro_reg;
    soc_field_t initiate_val;
    soc_reg_t reg_flush[2] = {Q_SCHED_PORT_FLUSH_SPLIT0r,
                              Q_SCHED_PORT_FLUSH_SPLIT1r};
    soc_reg_t reg_mtro[2] = {MTRO_PORT_ENTITY_DISABLE_SPLIT0r,
                             MTRO_PORT_ENTITY_DISABLE_SPLIT1r};

    COMPILER_64_ZERO(rval64);
    COMPILER_64_ZERO(pbits64);

    SOC_IF_ERROR_RETURN(
        soc_td3_mmu_bmp_reg_pos_get(unit, port, &pipe, &split, &pos));
    flush_reg = reg_flush[split];
    mtro_reg = reg_mtro[split];

    if (pos < 32) {
        COMPILER_64_SET(pbits64, 0, (1 << pos));
    } else {
        COMPILER_64_SET(pbits64, (1 << (pos - 32)), 0);
    }

    SOC_IF_ERROR_RETURN
        (_soc_trident3_mmu_port_empty_check(unit, port, &empty));
    if (empty) {
        return SOC_E_NONE;
    }

    /* Read the Port flush Register */
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, flush_reg, pipe, 0, &rval64));

    COMPILER_64_OR(rval64, pbits64);

    /* Port Not Empty - Initiate Flush */
    /* ENABLEf is the only field in FLUSH reg. Hence writing it directly */
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, flush_reg, pipe, 0, rval64));

    /* Disable Shaper Updates */
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, mtro_reg, pipe, 0, &rval64));
    COMPILER_64_COPY(mtro64, rval64);
    COMPILER_64_AND(mtro64, pbits64);

    COMPILER_64_OR(rval64, pbits64);

    /* METERING_DISABLEf is the only field in reg. Hence writing it directly */
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, mtro_reg, pipe, 0, rval64));

    /* Initiate RQE FIFO Flush */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, Q_SCHED_RQE_SNAPSHOTr, pipe, 0, &rval));
    initiate_val = 0x1;
    soc_reg_field_set(unit, Q_SCHED_RQE_SNAPSHOTr, &rval,
                      INITIATEf, initiate_val);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, Q_SCHED_RQE_SNAPSHOTr, pipe, 0, rval));

    /* Restart timeout */
    initiate_val = 0x1;
    empty = 0;
    soc_timeout_init(&to, drain_timeout, 0);
    for (;;) {
        if (initiate_val != 0) {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, Q_SCHED_RQE_SNAPSHOTr, pipe, 0, &rval));
            initiate_val = soc_reg_field_get(unit, Q_SCHED_RQE_SNAPSHOTr,
                                             rval, INITIATEf);
        } else {    /* Completion of Snapshot Process */
            SOC_IF_ERROR_RETURN
                (_soc_trident3_mmu_port_empty_check(unit, port, &empty));
            if (empty) {
                break;
            }
        }

        if (soc_timeout_check(&to)) {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, Q_SCHED_RQE_SNAPSHOTr, pipe, 0, &rval));
            initiate_val = soc_reg_field_get(unit, Q_SCHED_RQE_SNAPSHOTr,
                                             rval, INITIATEf);
            if (initiate_val == 0) {
                /* Completion of Snapshot Process */
                SOC_IF_ERROR_RETURN
                    (_soc_trident3_mmu_port_empty_check(unit, port, &empty));
            }
            break;
        }
    }

    COMPILER_64_NOT(pbits64);
    /* Restore Shaper Updates */
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, mtro_reg, pipe, 0, &rval64));
    COMPILER_64_AND(rval64, pbits64);
    COMPILER_64_OR(rval64, mtro64);
    /* METERING_DISABLEf is the only field in reg. Hence writing it directly */
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, mtro_reg, pipe, 0, rval64));

    /* Read the Port flush Register */
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, flush_reg, pipe, 0, &rval64));
    COMPILER_64_AND(rval64, pbits64);
    /* Port Empty - Disable Flush */
    /* ENABLEf is the only field in FLUSH reg. Hence writing it directly */
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, flush_reg, pipe, 0, rval64));

    if ((initiate_val == 0) && (empty)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                    "MacDrain:port %d,%s, "
                    "Drained successfully\n"),
                    unit, SOC_PORT_NAME(unit, port)));
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "MacDrainError:port %d,%s, "
                  "Unable to drain packets from MMU\n"),
                  unit, SOC_PORT_NAME(unit, port)));
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

soc_error_t
soc_td3_subsidiary_ports_get(int unit, soc_port_t port, soc_pbmp_t *pbmp)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_t phy_port_base, phy_port;
    soc_pbmp_t pbm;
    int max_subport;

    if (!SOC_PORT_VALID(unit, port) || IS_CPU_PORT(unit, port) ||
        IS_LB_PORT(unit, port)) {
         /* CPU, LB and Management ports do not support flexport */
        return SOC_E_PORT;
    }

    if (SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port)) {
            return SOC_E_PORT;
    }

    phy_port = si->port_l2p_mapping[port];
    if (-1 == phy_port) {
        return SOC_E_PORT;
    }
    phy_port_base = PORT_BLOCK_BASE_PORT(port);

    /* Only lane 0 and lane 2 of a PM can be controlling ports */
    if ((phy_port_base != phy_port) && ((phy_port_base + 2) != phy_port)) {
        /* Not a controlling port */
        return SOC_E_PORT;
    }
    max_subport = phy_port + (4 - ((phy_port - 1) % 4));
    SOC_PBMP_CLEAR(pbm);
    for (; phy_port < max_subport; phy_port++) {
        if (si->port_p2l_mapping[phy_port] == -1) {
            continue;
        }
        SOC_PBMP_PORT_ADD(pbm, si->port_p2l_mapping[phy_port]);
    }
    SOC_PBMP_ASSIGN(*pbmp, pbm);

    return SOC_E_NONE;
}

int
soc_td3_mpls_hash_control_set(int *hash_entry)
{
    int unit = 0;
    int mpls_16k_banks = 0,mpls_8k_banks = 0;
    int uat_16k_banks = 0,uat_8k_banks = 0;
    int index = 0;
    int num_entries = 0, shared_hash_offset = 0;
    int hash_control_entry[2];
    uint32 shared_hash_control_entry[4];
    uint32 shared_8k_hash_control_entry[4];

    static const soc_field_t logical_to_physical_fields[] = {
        LOGICAL_BANK_0_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_4_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_5_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_6_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_7_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_8_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_9_PHYSICAL_BANK_LOCATIONf
    };

    static const soc_field_t shared_fields[] = {
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf, B3_HASH_OFFSETf,
        B4_HASH_OFFSETf, B5_HASH_OFFSETf, B6_HASH_OFFSETf, B7_HASH_OFFSETf
    };

    sal_memset(hash_control_entry, 0,
            sizeof(mpls_entry_hash_control_entry_t));
    sal_memset(shared_hash_control_entry, 0,
            sizeof(uft_shared_banks_control_entry_t));
    sal_memset(shared_8k_hash_control_entry, 0,
            sizeof(ip_uat_8k_shared_banks_control_entry_t));

    SOC_IF_ERROR_RETURN
        (READ_UFT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                        shared_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (READ_IP_UAT_8K_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                              shared_8k_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (READ_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                       hash_control_entry));
    sal_memcpy(hash_entry, hash_control_entry, sizeof(hash_control_entry));

    num_entries = soc_mem_index_count(unit, MPLS_ENTRY_SINGLEm);
    SOC_IF_ERROR_RETURN(soc_trident3_iuat_sizing(unit, num_entries,
                &uat_16k_banks, &uat_8k_banks));
    mpls_16k_banks = uat_16k_banks;
    mpls_8k_banks = uat_8k_banks;

    soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, hash_control_entry,
            HASH_TABLE_BANK_CONFIGf, (((1 << mpls_8k_banks) - 1) |
                (((1 << mpls_16k_banks) - 1) << 4)));

    for(index = 0; index < mpls_8k_banks; index++) {
        shared_hash_offset = (index == (mpls_8k_banks-1))?
                               TD3_UAT_8K_HASH_OFFSET_MAX :
                               ((index * TD3_UAT_8K_HASH_OFFSET_MAX) / (mpls_8k_banks-1));
        soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, hash_control_entry,
                logical_to_physical_fields[index], index);
        soc_mem_field32_set(unit, IP_UAT_8K_SHARED_BANKS_CONTROLm,
                shared_8k_hash_control_entry, shared_fields[index],
                shared_hash_offset);
    }

    for(index = 0; index < mpls_16k_banks; index++) {
        shared_hash_offset = (index == (mpls_16k_banks-1))?
                               TD3_UAT_16K_HASH_OFFSET_MAX :
                               ((index * TD3_UAT_16K_HASH_OFFSET_MAX) / (mpls_16k_banks-1));
        soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm,
                hash_control_entry,
                logical_to_physical_fields[index], index + 4);
        soc_mem_field32_set(unit, IP_UAT_16K_SHARED_BANKS_CONTROLm,
                shared_hash_control_entry, shared_fields[index],
                shared_hash_offset);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                        hash_control_entry));

    return SOC_E_NONE;
}
void soc_td3_hgoe_feature_update(int unit) {
    if (!soc_feature(unit, soc_feature_higig_over_ethernet)) {
        return;
    }
    /* if HGOE is enabled, disable the following features */
    /* L3 */
    SOC_FEATURE_CLEAR(unit, soc_feature_l3);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_ip6);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_entry_key_type);
    SOC_FEATURE_CLEAR(unit, soc_feature_lpm_tcam);
    SOC_FEATURE_CLEAR(unit, soc_feature_ip_mcast);
    SOC_FEATURE_CLEAR(unit, soc_feature_ip_mcast_repl);
    SOC_FEATURE_CLEAR(unit, soc_feature_ipmc_unicast);
    SOC_FEATURE_CLEAR(unit, soc_feature_ipmc_use_configured_dest_mac);
    SOC_FEATURE_CLEAR(unit, soc_feature_urpf);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3mc_use_egress_next_hop);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_sgv);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_dynamic_ecmp_group);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_ingress_interface);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_iif_zero_invalid);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_iif_under_4k);
    SOC_FEATURE_CLEAR(unit, soc_feature_ecmp_dlb);
    SOC_FEATURE_CLEAR(unit, soc_feature_ecmp_dlb_optimized);
    SOC_FEATURE_CLEAR(unit, soc_feature_ecmp_dlb_property_force_set);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_iif_profile);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_ingress_interface);
    SOC_FEATURE_CLEAR(unit, soc_feature_nat);
    SOC_FEATURE_CLEAR(unit, soc_feature_l2gre);
    SOC_FEATURE_CLEAR(unit, soc_feature_l2gre_default_tunnel);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_lookup_cmd);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_shared_defip_table);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_ip4_options_profile);
    SOC_FEATURE_CLEAR(unit, soc_feature_repl_l3_intf_use_next_hop);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_extended_host_entry);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_ecmp_1k_groups);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_defip_map);
    SOC_FEATURE_CLEAR(unit, soc_feature_l3_host_ecmp_group);
    SOC_FEATURE_CLEAR(unit, soc_feature_niv);
    SOC_FEATURE_CLEAR(unit, soc_feature_vlan_vp);
    SOC_FEATURE_CLEAR(unit, soc_feature_vp_lag);
    SOC_FEATURE_CLEAR(unit, soc_feature_virtual_switching);
    SOC_FEATURE_SET(unit, soc_feature_int_common_init);
    SOC_FEATURE_CLEAR(unit, soc_feature_mim);

    /* ALPM */
    SOC_FEATURE_CLEAR(unit, soc_feature_alpm);
    SOC_FEATURE_CLEAR(unit, soc_feature_alpm2);

    /* MPLS */

    SOC_FEATURE_CLEAR(unit, soc_feature_mpls);
    SOC_FEATURE_CLEAR(unit, soc_feature_mpls_enhanced);
    SOC_FEATURE_CLEAR(unit, soc_feature_mpls_entropy);
    SOC_FEATURE_CLEAR(unit, soc_feature_td2p_mpls_entropy_label);
    SOC_FEATURE_CLEAR(unit, soc_feature_mpls_lsr_ecmp);
    SOC_FEATURE_CLEAR(unit, soc_feature_mpls_segment_routing);
    SOC_FEATURE_CLEAR(unit, soc_feature_mpls_failover);
    SOC_FEATURE_CLEAR(unit, soc_feature_flex_ctr_mpls_3_label_count);
    SOC_FEATURE_CLEAR(unit, soc_feature_mpls_exp_to_phb_cng_map);
    SOC_FEATURE_CLEAR(unit, soc_feature_mpls_software_failover);
    SOC_FEATURE_CLEAR(unit, soc_feature_mpls_frr);

    /* VXLAN */
    SOC_FEATURE_CLEAR(unit, soc_feature_vxlan);
    SOC_FEATURE_CLEAR(unit, soc_feature_vrf_aware_vxlan_termination);

    /* TRILL */
    SOC_FEATURE_CLEAR(unit, soc_feature_trill);
    SOC_FEATURE_CLEAR(unit, soc_feature_fcoe);

}

/*
 * Enable flow based UDF extraction.
 * Few containers are disabled by default for certain flows.
 * This routine enables UDF extraction for various protocols
 */

int
soc_td3_flow_based_udf_enable(int unit)
{

    uint32 rval = 0;

    if (!soc_feature(unit, soc_feature_enable_flow_based_udf_extraction)) {
        return SOC_E_NONE;
    }

    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        SOC_IF_ERROR_RETURN(READ_EGR_FLEX_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, EGR_FLEX_CONFIGr, &rval,
                          ENABLE_FLEX_MIM_ISID_EXTRACTIONf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_FLEX_CONFIGr(unit, rval));
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_ING_FLEX_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, ING_FLEX_CONFIGr, &rval,
                      ENABLE_BFD_VER_EXTRACTIONf, 1);
    if (0 == soc_property_get(unit, spn_IPSEC_SPI_DISABLE, 0)) {
        soc_reg_field_set(unit, ING_FLEX_CONFIGr, &rval,
                          ENABLE_IPSEC_SPI_EXTRACTIONf, 1);
    }
    soc_reg_field_set(unit, ING_FLEX_CONFIGr, &rval,
                      ENABLE_IPV4_FLAGS_FRAG_OFFSET_EXTRACTIONf, 1);
    if (0 == soc_property_get(unit, spn_LLC_DISABLE, 0)) {
        soc_reg_field_set(unit, ING_FLEX_CONFIGr, &rval,
                          ENABLE_LLC_HDR_EXTRACTIONf, 1);
    }
    if (0 == soc_property_get(unit, spn_GPE_IOAM_DISABLE, 0)) {
        soc_reg_field_set(unit, ING_FLEX_CONFIGr, &rval,
                          GPE_IOAM_ENABLEf, 1);
    }
    if (soc_feature(unit, soc_feature_vxlan_gpe_support)) {
        soc_reg_field_set(unit, ING_FLEX_CONFIGr, &rval,
                GPE_IOAM_E2E_ENABLEf, 1);
        soc_reg_field_set(unit, ING_FLEX_CONFIGr, &rval,
                GPE_GBP_ENABLEf, 1);
    }

    /* Additional setting for devices supporting Broadscan */
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        soc_reg_field_set(unit, ING_FLEX_CONFIGr, &rval,
                          ENABLE_TCP_ACK_NO_EXTRACTIONf, 1);
        soc_reg_field_set(unit, ING_FLEX_CONFIGr, &rval,
                          ENABLE_TCP_URGENT_PTR_EXTRACTIONf, 1);
    }
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    /* Additional setting for devices supporting IOAM flex flow */
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        soc_reg_field_set(unit, ING_FLEX_CONFIGr, &rval,
                                    GPE_IOAM_E2E_ENABLEf, 1);
    }
#endif

    SOC_IF_ERROR_RETURN(WRITE_ING_FLEX_CONFIGr(unit, rval));

    return SOC_E_NONE;
}

int soc_td3_cosq_threshold_dynamic_change_check(
    int unit,
    int new_threshold,
    soc_reg_t ctr_reg,
    soc_ctr_control_info_t ctrl_info,
    int ar_idx,
    uint8 sync_mode,
    uint8 *allowed)
{
    uint32 flags = sync_mode ? SOC_COUNTER_SYNC_ENABLE : 0;
    uint64 val64s[_TD3_XPES_PER_DEV];
    soc_timeout_t timeout;
    int index;
    int check_pass = TRUE;

    /* Not check for threshold <= 0 case */
    if (new_threshold <= 0) {
        *allowed = TRUE;
        return SOC_E_NONE;
    }

    *allowed = FALSE;
    soc_timeout_init(&timeout, SOC_MMU_THRESHOLD_DYNAMIC_CHECK_TIMEOUT, 1);
    do {
        check_pass = TRUE;

        /* Each XPE maps to a pair of IPIPE or EPIPE based on memory type. */
        /* Retrieve the use count on all the valid XPEs. */
        sal_memset(&val64s, 0, sizeof(val64s));
        SOC_IF_ERROR_RETURN
            (soc_counter_get_per_buffer(unit, ctr_reg, ctrl_info,
                                                  flags, ar_idx, val64s));

        for (index = 0; index < _TD3_XPES_PER_DEV; index++) {
            /* Use count of any XPE greater or equal than new threshold. */
            if (COMPILER_64_HI(val64s[index])
                || COMPILER_64_LO(val64s[index]) >= new_threshold) {
                check_pass = FALSE;
                break;
            }
        }

        if (check_pass == TRUE) {
            *allowed = TRUE;
            return SOC_E_NONE;
        }

        if (soc_timeout_check(&timeout)) {
            LOG_ERROR(BSL_LS_SOC_COSQ,
                      (BSL_META_U(unit,
                                  "ERROR: Timeout during threshold dynamic check!\n")));
            return SOC_E_BUSY;
        }
    } while (TRUE);
}

/*
 * Trident3 chip driver functions.
 * Pls keep at the end of this file for easy access.
 */
soc_functions_t soc_trident3_drv_funs = {
    _soc_trident3_misc_init,
    _soc_trident3_mmu_init,
    soc_trident3_age_timer_get,
    soc_trident3_age_timer_max_get,
    soc_trident3_age_timer_set,
    soc_trident3_tscx_firmware_set,
    _soc_trident3_tscx_reg_read,
    _soc_trident3_tscx_reg_write,
    soc_trident3_bond_info_init,
};
#endif /* BCM_TOMAHAWK_SUPPORT */
