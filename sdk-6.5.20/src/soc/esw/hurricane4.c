/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hurricane4.c
 * Purpose:
 * Requires:
 */


#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/dpc.h>
#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>
#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#endif

#include <soc/bradley.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/i2c.h>
#include <soc/l2x.h>
#include <soc/mcm/intr_iproc.h>

#ifdef BCM_HURRICANE4_SUPPORT

#include <soc/mmu_config.h>
#include <soc/hurricane4.h>
#include <soc/hurricane4_tdm.h>
#include <soc/tomahawk.h>
#include <soc/esw/cancun.h>
#include <soc/soc_ser_log.h>
#include <soc/pstats.h>
#include <soc/flexport/hurricane4/hurricane4_flexport.h>
#include <soc/esw/portctrl.h>
#include <soc/scache.h>
#include <soc/bondoptions.h>
#include <shared/pack.h>
#include <bcm_int/esw/port.h>
#include <soc/esw/utt.h>
#include <soc/esw/flowtracker.h>

#ifdef INCLUDE_XFLOW_MACSEC
#include <bcm_int/common/xflow_macsec_cmn.h>
#endif

#define HR4_NUM_PHY_PORT               (72)
#define HR4_NUM_PORT                   (72)
#define HR4_NUM_MMU_PORT               (72)
#define HR4_PMQPORT_BLOCK_INSTANCE_INDEX_GET_BY_PHYSICAL_PORT(port) \
           ((port - 25 - 1) / 16)

#define ETHER_MODE                     (0)
#define Q_MODE                         (1)

#define HR4_PORT_IS_GPHY(unit, phy_port) \
        (((phy_port  >=  1) && (phy_port <= 24)) ? 1 : 0)

#define HR4_PORT_IS_PMQ1(unit, phy_port) \
        (((phy_port  >= 41) && (phy_port <= 56)) ? 1 : 0)

#define HR4_PORT_IS_FALCON(unit, phy_port) \
        (((phy_port  >= 57) && (phy_port <= 60)) ? 1 : 0)

#define HR4_PORT_IS_MACSEC(unit, phy_port) \
        (((phy_port  >= 65) && (phy_port <= 68)) ? 1 : 0)

/* UAT bank size: Same for Ingress and Egress */
#define HR4_UAT_BANK_SIZE         (512)
/* IUAT number of shared banks */
#define HR4_SOC_IUAT_BANKS        (8)
/* EUAT number of shared banks */
#define HR4_SOC_EUAT_BANKS        (4)

/* UTT info */
#define HR4_DEFIP_MAX_TCAMS            (8)
#define HR4_UTT_TCAM_DEPTH             (1024)
#define HR4_UTT_MAX_TCAMS              (24)
#define HR4_UTT_IFP_MIN_TCAMS          (8)
#define HR4_UTT_LPM_MIN_TCAMS          (6)
#define HR4_UTT_IFP_MAX_TCAMS          (18)
#define HR4_UTT_LPM_MAX_TCAMS          (16)
#define HR4_UTT_LPM_MAX_TCAMS_PER_LKUP (4)
#define HR4_UTT_IFP_LKUP_BASE          (0)
#define HR4_UTT_LPM_LKUP_BASE          (18)
#define HR4_UTT_IFP_ID                 (0)
#define HR4_UTT_LPM_ID                 (1)
#define HR4_UTT_IFP_TCAM_START         (0)

#define IS_MGMT_PORT(unit,port) IS_MANAGEMENT_PORT(unit, port)

#ifdef BCM_WARM_BOOT_SUPPORT
#define SOC_TDM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_TDM_WB_DEFAULT_VERSION            SOC_TDM_WB_VERSION_1_0
#endif

typedef struct {
    int port;
    int phy_port;
    int pipe;
    int idb_port;
    int mmu_port;
} _soc_hr4_anc_ports_t;

static const _soc_hr4_anc_ports_t hr4_anc_ports[] = {
    { 0,   0,   0, 70, 70 },  /* cpu port */
    { 70,  69,  0, 71, 71 },  /* loopback port */
    { 71,  70,  0, 69, 69 },  /* FAE port */
    { 65,  65,  0, 65, 65 },  /* MACSEC port1 */
    { 66,  66,  0, 66, 66 },  /* MACSEC port2 */
    { 67,  67,  0, 67, 67 },  /* MACSEC port3 */
    { 68,  68,  0, 68, 68 },  /* MACSEC port4 */
};

int
_soc_hr4_port_speed_cap[SOC_MAX_NUM_DEVICES][HURRICANE4_PHY_PORTS_PER_DEV];


typedef struct _soc_hr4_mmu_traffic_ctrl_s {
    uint8   thdo_drop_bmp[HR4_NUM_PORT];
} _soc_hr4_mmu_traffic_ctrl_t;

#define   MMU_TRAFFIC_EN_CTRL           (1 << 1)
#define   PORT_TX_EN_CTRL               (1 << 2)
#define IS_MGMT_PORT(unit,port) IS_MANAGEMENT_PORT(unit, port)
static    _soc_hr4_mmu_traffic_ctrl_t
              *_soc_hr4_mmu_traffic_ctrl[SOC_MAX_NUM_DEVICES];
static    sal_mutex_t _fwd_ctrl_lock[SOC_MAX_NUM_DEVICES];
#define   FWD_CTRL_LOCK(unit) \
          sal_mutex_take(_fwd_ctrl_lock[unit], sal_mutex_FOREVER)
#define   FWD_CTRL_UNLOCK(unit) \
          sal_mutex_give(_fwd_ctrl_lock[unit])

/* Externs */
extern int soc_hr4_portctrl_pm_type_get(int unit, soc_port_t phy_port, int* pm_type);

static const soc_reg_t hr4_pvtmon_result_reg[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r,
    TOP_PVTMON_RESULT_2r, TOP_PVTMON_RESULT_3r,
    TOP_PVTMON_RESULT_4r, TOP_PVTMON_RESULT_5r,
    TOP_PVTMON_RESULT_6r
};

extern int soc_td3_hash_mutex_init (int unit);

egr_vlan_xlate_1_hash_control_entry_t       egr_vlan_1_hash_control;
egr_vlan_xlate_2_hash_control_entry_t       egr_vlan_2_hash_control;
egr_vp_vlan_membership_hash_control_entry_t egr_vp_vlan_mbm_hash_control;
ing_vp_vlan_membership_hash_control_entry_t ing_vp_vlan_mbm_hash_control;
l2_entry_hash_control_entry_t               l2_entry_hash_control;
l3_entry_hash_control_entry_t               l3_entry_hash_control;
mpls_entry_hash_control_entry_t             mpls_entry_hash_control;
vlan_xlate_1_hash_control_entry_t           vlan_1_hash_control;
vlan_xlate_2_hash_control_entry_t           vlan_2_hash_control;
ip_uat_shared_banks_control_entry_t         ip_uat_shared_banks_control;
ep_uat_shared_banks_control_entry_t         ep_uat_shared_banks_control;
ft_key_ss0_shared_banks_control_entry_t     ft_key_ss0_shared_banks_control;
ft_key_ss1_shared_banks_control_entry_t     ft_key_ss1_shared_banks_control;

#ifdef BCM_UTT_SUPPORT
int
soc_hr4_utt_init(int u)
{
    soc_utt_agent_t aid;

    SOC_UTT_TCAM_DEPTH(u) = HR4_UTT_TCAM_DEPTH;
    SOC_UTT_MAX_TCAMS(u)  = HR4_UTT_MAX_TCAMS;
    SOC_UTT_MAX_AGENTS(u) = 2;

    /* IFP */
    aid = SOC_UTT_AGENT_IFP;
    SOC_UTT_AGENT_MIN_TCAMS(u, aid)            = HR4_UTT_IFP_MIN_TCAMS;
    SOC_UTT_AGENT_MAX_TCAMS(u, aid)            = HR4_UTT_IFP_MAX_TCAMS;
    SOC_UTT_AGENT_MAX_LOOKUPS(u, aid)          = 18;
    SOC_UTT_AGENT_HW_ID(u, aid)                = HR4_UTT_IFP_ID;
    SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(u, aid) = 1;
    SOC_UTT_AGENT_DEF_NUM_LOOKUPS(u, aid)      = 18;
    SOC_UTT_AGENT_DEF_TABLE_SIZE(u, aid)       = 18 * HR4_UTT_TCAM_DEPTH;
    SOC_UTT_AGENT_LOOKUP_BASE(u, aid)          = HR4_UTT_IFP_LKUP_BASE;

    /* LPM */
    aid = SOC_UTT_AGENT_LPM;
    SOC_UTT_AGENT_FLAGS(u, aid)                = SOC_UTT_LPM_FIXED_DATA_ADDRESS;
    SOC_UTT_AGENT_MIN_TCAMS(u, aid)            = HR4_UTT_LPM_MIN_TCAMS;
    SOC_UTT_AGENT_MAX_TCAMS(u, aid)            = HR4_UTT_LPM_MAX_TCAMS;
    SOC_UTT_AGENT_MAX_LOOKUPS(u, aid)          = 4;
    SOC_UTT_AGENT_HW_ID(u, aid)                = HR4_UTT_LPM_ID;
    SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(u, aid) = HR4_UTT_LPM_MAX_TCAMS_PER_LKUP;
    SOC_UTT_AGENT_DEF_NUM_LOOKUPS(u, aid)      = 4;
    SOC_UTT_AGENT_DEF_TABLE_SIZE(u, aid)       = 6 * HR4_UTT_TCAM_DEPTH;
    SOC_UTT_AGENT_LOOKUP_BASE(u, aid)          = HR4_UTT_LPM_LKUP_BASE;

    return SOC_E_NONE;
}
#endif /* BCM_UTT_SUPPORT */

/*
 * Function:
 *      soc_hurricane4_mem_config
 * Purpose:
 *      Configure depth of UFT/UAT memories
 * Parameters:
 *      unit (IN)    - switch unit
 * Returns:
 *      SOC_E_*
 */
int
soc_hurricane4_mem_config(int unit)
{
    soc_persist_t *sop;
    int l2_entries, fixed_l2_entries;
    int num_ipv6_128b_entries = 0;
    int config_v6_entries = 0;
    int defip_config = 0;
    int l3_entries, fixed_l3_entries;
    int mpls_entries, vlan_xlate_1_entries, vlan_xlate_2_entries;
    int mpls_banks;
    int vlan_xlate_1_banks;
    int vlan_xlate_2_banks;
    int egr_vlan_xlate_1_entries, egr_vlan_xlate_2_entries;
    int egr_vlan_xlate_1_banks, egr_vlan_xlate_2_banks;

    sop = SOC_PERSIST(unit);

    /*  No UFT */
    fixed_l2_entries = 64 * 1024;  /* 64k dedicated L2 entries, (32k * 2 banks) */
    l2_entries = fixed_l2_entries;

    fixed_l3_entries = 32 * 1024;  /* 32k dedicated L3 entries, (16k * 2 banks) */
    l3_entries = fixed_l3_entries;

    _soc_alpm_mode[unit] = 0;

    /* IUAT BANK SIZING
     *
	 * Banks 0-7 are shared banks of size 512 per bank
     */

    mpls_entries = soc_property_get(unit, spn_MPLS_MEM_ENTRIES, 2048);
    mpls_banks = (mpls_entries + HR4_UAT_BANK_SIZE - 1) / HR4_UAT_BANK_SIZE;
    mpls_entries = mpls_banks * HR4_UAT_BANK_SIZE;

    vlan_xlate_1_entries = soc_property_get(unit, spn_VLAN_XLATE_1_MEM_ENTRIES,
                                            1024);
    vlan_xlate_1_banks = (vlan_xlate_1_entries +
                          HR4_UAT_BANK_SIZE - 1) / HR4_UAT_BANK_SIZE;
    vlan_xlate_1_entries = vlan_xlate_1_banks * HR4_UAT_BANK_SIZE;

    vlan_xlate_2_entries = soc_property_get(unit, spn_VLAN_XLATE_2_MEM_ENTRIES,
                                            1024);
    vlan_xlate_2_banks = (vlan_xlate_2_entries +
                          HR4_UAT_BANK_SIZE - 1) / HR4_UAT_BANK_SIZE;
    vlan_xlate_2_entries = vlan_xlate_2_banks * HR4_UAT_BANK_SIZE;

    if ((mpls_banks + vlan_xlate_1_banks + vlan_xlate_2_banks)
                                > HR4_SOC_IUAT_BANKS) {
        return SOC_E_PARAM;
    }

    /* EUAT BANK SIZING
     *
     * Banks 0-3 are shared banks of size 512 per bank
     */

    egr_vlan_xlate_1_entries = soc_property_get(unit,
                                    spn_EGR_VLAN_XLATE_1_MEM_ENTRIES, 1024);
    egr_vlan_xlate_1_banks = (egr_vlan_xlate_1_entries +
                              HR4_UAT_BANK_SIZE - 1) / HR4_UAT_BANK_SIZE;
    egr_vlan_xlate_1_entries = egr_vlan_xlate_1_banks * HR4_UAT_BANK_SIZE;

    egr_vlan_xlate_2_entries = soc_property_get(unit,
                                    spn_EGR_VLAN_XLATE_2_MEM_ENTRIES, 1024);
    egr_vlan_xlate_2_banks = (egr_vlan_xlate_2_entries +
                              HR4_UAT_BANK_SIZE - 1) / HR4_UAT_BANK_SIZE;
    egr_vlan_xlate_2_entries = egr_vlan_xlate_2_banks * HR4_UAT_BANK_SIZE;

    if ((egr_vlan_xlate_1_banks + egr_vlan_xlate_2_banks)
                                > HR4_SOC_EUAT_BANKS) {
        return SOC_E_PARAM;
    }

    /*
     * LP memory sizing
     * L2:
     * Memory Bank with 1W support       - 8k entries
     * L3:
     * Memory Bank with 1W,2W,4W support - 7k entries
     * IUAT:
     * Memory Bank with 1W,2W support    - 192 entries
     * EUAT:
     * Memory Bank with 1W,2W support    - 192 entries
     */

    /* Adjust L2 table size */
    sop->memState[L2Xm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_SINGLEm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_ONLY_SINGLEm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_ECCm].index_max = l2_entries - 1;
    sop->memState[L2_HITDA_ONLYm].index_max = l2_entries / 4 - 1;
    sop->memState[L2_HITSA_ONLYm].index_max = l2_entries / 4 - 1;
    /* 8k per dedicated L2 bank */
    sop->memState[L2_ENTRY_LPm].index_max = (2 * 8192) - 1;

    /* Adjust L3 table size */
    sop->memState[L3_ENTRY_SINGLEm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_ONLY_SINGLEm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_ECCm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_DOUBLEm].index_max = l3_entries / 2 - 1;
    sop->memState[L3_ENTRY_ONLY_DOUBLEm].index_max = l3_entries / 2 - 1;
    sop->memState[L3_ENTRY_QUADm].index_max = l3_entries / 4 - 1;
    sop->memState[L3_ENTRY_ONLY_QUADm].index_max = l3_entries / 4 - 1;
    sop->memState[L3_ENTRY_HIT_ONLYm].index_max = l3_entries / 4 - 1;
    /* 7k per dedicated L3 bank */
    sop->memState[L3_ENTRY_LPm].index_max = (2 * 7168) - 1;


    /* Adjust MPLS table size */
    sop->memState[MPLS_ENTRY_SINGLEm].index_max = mpls_entries - 1;
    sop->memState[MPLS_ENTRYm].index_max = mpls_entries / 2 - 1;
    /* 192 per bank */
    sop->memState[MPLS_ENTRY_LPm].index_max = (mpls_banks * 192) - 1;
    sop->memState[MPLS_ENTRY_ECCm].index_max = mpls_entries - 1;

    /* Adjust VLAN_XLATE_1 table size */
    sop->memState[VLAN_XLATE_1_SINGLEm].index_max = vlan_xlate_1_entries - 1;
    sop->memState[VLAN_XLATE_1_DOUBLEm].index_max = vlan_xlate_1_entries / 2 - 1;
    /* 192 per bank */
    sop->memState[VLAN_XLATE_1_LPm].index_max = (vlan_xlate_1_banks * 192) - 1;
    sop->memState[VLAN_XLATE_1_ECCm].index_max = vlan_xlate_1_entries - 1;

    /* Adjust VLAN_XLATE_2 table size */
    sop->memState[VLAN_XLATE_2_SINGLEm].index_max = vlan_xlate_2_entries - 1;
    sop->memState[VLAN_XLATE_2_DOUBLEm].index_max = vlan_xlate_2_entries / 2 - 1;
    /* 192 per bank */
    sop->memState[VLAN_XLATE_2_LPm].index_max = (vlan_xlate_2_banks * 192) - 1;
    sop->memState[VLAN_XLATE_2_ECCm].index_max = vlan_xlate_2_entries - 1;

    /* Adjust EGR_VLAN_XLATE_1 table size */
    sop->memState[EGR_VLAN_XLATE_1_SINGLEm].index_max = egr_vlan_xlate_1_entries - 1;
    sop->memState[EGR_VLAN_XLATE_1_DOUBLEm].index_max = egr_vlan_xlate_1_entries / 2 - 1;
    /* 192 per bank */
    sop->memState[EGR_VLAN_XLATE_1_LPm].index_max = (egr_vlan_xlate_1_banks * 192) - 1;
    sop->memState[EGR_VLAN_XLATE_1_ECCm].index_max = egr_vlan_xlate_1_entries - 1;

    /* Adjust EGR_VLAN_XLATE_2 table size */
    sop->memState[EGR_VLAN_XLATE_2_SINGLEm].index_max = egr_vlan_xlate_2_entries - 1;
    sop->memState[EGR_VLAN_XLATE_2_DOUBLEm].index_max = egr_vlan_xlate_2_entries / 2 - 1;
    /* 192 per bank */
    sop->memState[EGR_VLAN_XLATE_2_LPm].index_max = (egr_vlan_xlate_2_banks * 192) - 1;
    sop->memState[EGR_VLAN_XLATE_2_ECCm].index_max = egr_vlan_xlate_2_entries - 1;

    SOC_CONTROL(unit)->l3_defip_max_tcams = HR4_DEFIP_MAX_TCAMS;
    SOC_CONTROL(unit)->l3_defip_tcam_size = HR4_UTT_TCAM_DEPTH * 2;
#ifdef BCM_UTT_SUPPORT
    SOC_IF_ERROR_RETURN(soc_utt_init(unit));
#endif
    if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
        if (!soc_feature(unit, soc_feature_utt)) {

            defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);

            num_ipv6_128b_entries = soc_property_get(unit,
                                        spn_NUM_IPV6_LPM_128B_ENTRIES,
                                        (defip_config ?
                                        (SOC_CONTROL(unit)->l3_defip_tcam_size * 2)
                                        : 0));
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
            soc_l3_defip_indexes_init(unit, config_v6_entries);
        }
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

    /* Adjust the Flow-tracker session table size based on the
       supported size, these features would have been set based
       on the OTP values */
    if (soc_feature(unit, soc_feature_flowtracker_3banks_ft_table)) {
        /* 3 banks x 1k */
        sop->memState[FT_KEY_SINGLEm].index_max = 3071;
        sop->memState[FT_KEY_DOUBLEm].index_max = 1535;
        sop->memState[FT_KEY_DOUBLE_PIPE0m].index_max = 1535;
        sop->memState[FT_KEY_ECCm].index_max = 3071;
    } else if (soc_feature(unit, soc_feature_flowtracker_5banks_ft_table)) {
        /* 5 banks x 1k */
        sop->memState[FT_KEY_SINGLEm].index_max = 5119;
        sop->memState[FT_KEY_DOUBLEm].index_max = 2559;
        sop->memState[FT_KEY_DOUBLE_PIPE0m].index_max = 2559;
        sop->memState[FT_KEY_ECCm].index_max = 3071;
    }

    /* Adjust the Flow-tracker group table size based on the
       supported size, these features would have been set based
       on the OTP values */
    if (soc_feature(unit, soc_feature_flowtracker_half_entries_ft_group)) {
        sop->memState[BSC_KG_GROUP_TABLEm].index_max = 127;
        sop->memState[BSC_DG_GROUP_TABLEm].index_max = 127;
        sop->memState[BSC_KG_FLOW_ACTIVE_COUNTERm].index_max = 127;
        sop->memState[BSC_KG_FLOW_LEARNED_COUNTERm].index_max = 127;
        sop->memState[BSC_KG_FLOW_AGE_OUT_COUNTERm].index_max = 127;
        sop->memState[BSC_KG_FLOW_EXCEED_COUNTERm].index_max = 127;
        sop->memState[BSC_KG_FLOW_MISSED_COUNTERm].index_max = 127;
        sop->memState[BSC_DG_FLOW_OPAQUE_COUNTER_0m].index_max = 127;
        sop->memState[BSC_DG_FLOW_OPAQUE_COUNTER_1m].index_max = 127;
        sop->memState[BSC_DG_FLOW_METER_EXCEED_COUNTERm].index_max = 127;
    }

    return SOC_E_NONE;
}

/***********************************************************************************
 *  HURRICANE4 MMU PORT NUMBERING, QUEUE ALLOCATION AND QUEUE ASSIGNMENT           *
 *---------------------------------------------------------------------------------*
 *  MMU PORT NO         PORT NAME          QUEUE USED         QUEUE ALLOCATED      *
 *---------------------------------------------------------------------------------*
 *  0-3                 Stacking port      26(UC/MC)          26(UC/MC)            *
 *  4-7                 Uplink port        26(UC/MC)          26(UC/MC)            *
 *  8-15                Down link port     16(UC/MC)          26(UC/MC)            *
 *  16-69               Down link port     16(UC/MC)          16(UC/MC)            *
 *  70                  Cpu port           48(MC)             48(MC)               *
 *  71                  Loopback port      10(MC)             10(MC)               *
 */
#define  SOC_HR4_NUM_HIGH_QUEUE_PORT                 16
int
soc_hurricane4_num_cosq_init_port(int unit, int port)
{
    soc_info_t *si;
    int mmu_port;

    si = &SOC_INFO(unit);

    mmu_port = SOC_TD3_MMU_PORT(unit, port);

    if (IS_CPU_PORT(unit, port)) {
        si->port_num_cosq[port] = SOC_HR4_NUM_CPU_QUEUES;
        si->port_cosq_base[port] = SOC_HR4_CPU_MCQ_BASE;
    } else if (IS_LB_PORT(unit, port)) {
        si->port_num_cosq[port] = SOC_HR4_NUM_MCAST_QUEUE_PER_LB_PORT;
        si->port_cosq_base[port] = SOC_HR4_LB_MCQ_BASE;
    } else if (mmu_port < SOC_HR4_NUM_HIGH_QUEUE_PORT) {
        /* Note that we have many holes in queue database */
        /* Stacking and uplink port */
        si->port_num_cosq[port] = SOC_HR4_NUM_MCAST_QUEUE_PER_ST_PORT;
        si->port_num_uc_cosq[port] = SOC_HR4_NUM_UCAST_QUEUE_PER_ST_PORT;
        si->port_cosq_base[port] =   mmu_port * SOC_HR4_NUM_MCAST_QUEUE_PER_ST_PORT;
        si->port_uc_cosq_base[port] = mmu_port * SOC_HR4_NUM_UCAST_QUEUE_PER_ST_PORT;
        si->port_num_ext_cosq[port] = 0;
    } else {
        si->port_num_cosq[port] = SOC_HR4_NUM_MCAST_QUEUE_PER_PORT;
        si->port_cosq_base[port] =  SOC_HR4_DOWNLINK_PORTQ_BASE + (mmu_port -
                                    SOC_HR4_NUM_HIGH_QUEUE_PORT)  *  SOC_HR4_NUM_MCAST_QUEUE_PER_PORT;
        si->port_num_uc_cosq[port] = SOC_HR4_NUM_UCAST_QUEUE_PER_PORT;
        si->port_uc_cosq_base[port] = SOC_HR4_DOWNLINK_PORTQ_BASE + (mmu_port -
                                      SOC_HR4_NUM_HIGH_QUEUE_PORT)  * SOC_HR4_NUM_UCAST_QUEUE_PER_PORT;
        si->port_num_ext_cosq[port] = 0;
    }

    return SOC_E_NONE;
}

int
soc_hurricane4_num_cosq_init(int unit)
{
    int port;

    PBMP_ALL_ITER(unit, port) {
        if (!IS_FAE_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
                (soc_hurricane4_num_cosq_init_port(unit, port));
        }
    }

    return SOC_E_NONE;
}


STATIC int
soc_hurricane4_max_frequency_get(int unit,  uint16 dev_id, uint8 rev_id,
                               int skew_id, int *frequency)
{
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        /* Fill from default_frequecy, if soc_property exists from the same, overwrite
           and also check the default is not greater than the max_frequecy */
        *frequency = SOC_BOND_INFO(unit)->default_core_freq;
        *frequency = soc_property_get(unit, spn_CORE_CLOCK_FREQUENCY, *frequency);
        if (*frequency > (SOC_BOND_INFO(unit)->max_core_freq + 1)) {
            return SOC_E_PARAM;
        }
    } else {
        switch (dev_id) {
            case BCM56275_DEVICE_ID:
                *frequency = soc_property_get(unit, spn_CORE_CLOCK_FREQUENCY,
                                              HR4_CLK_DEFAULT);
                break;
            default:
                return SOC_E_INTERNAL;
        }
    }

    return SOC_E_NONE;
}

/*
 * #1   single: 100  -   x  -   x  -   x  SOC_HR4_PORT_RATIO_SINGLE
 * #2   single:  40  -   x  -   x  -   x  SOC_HR4_PORT_RATIO_TRI_023_2_1_1
 * #3   dual:    50  -   x  -  50  -   x  SOC_HR4_PORT_RATIO_DUAL_1_1
 * #4   dual:    40  -   x  -  40  -   x  SOC_HR4_PORT_RATIO_DUAL_1_1
 * #5   dual:    20  -   x  -  20  -   x  SOC_HR4_PORT_RATIO_DUAL_1_1
 * #6   dual:    40  -   x  -  20  -   x  SOC_HR4_PORT_RATIO_DUAL_2_1
 * #7   dual:    20  -   x  -  40  -   x  SOC_HR4_PORT_RATIO_DUAL_1_2
 * #8   tri:     50  -   x  - 25/x - 25/x SOC_HR4_PORT_RATIO_TRI_023_2_1_1
 * #9   tri:     20  -   x  - 10/x - 10/x SOC_HR4_PORT_RATIO_TRI_023_2_1_1
 * #10  tri:     40  -   x  - 10/x - 10/x SOC_HR4_PORT_RATIO_TRI_023_4_1_1
 * #11  tri:    25/x - 25/x -  50  -   x  SOC_HR4_PORT_RATIO_TRI_012_1_1_2
 * #12  tri:    10/x - 10/x -  20  -   x  SOC_HR4_PORT_RATIO_TRI_012_1_1_2
 * #13  tri:    10/x - 10/x -  40  -   x  SOC_HR4_PORT_RATIO_TRI_012_1_1_4
 * #14  quad:   25/x - 25/x - 25/x - 25/x SOC_HR4_PORT_RATIO_QUAD
 * #15  quad:   10/x - 10/x - 10/x - 10/x SOC_HR4_PORT_RATIO_QUAD
 * #16  qsgmii0: 1   - 1    - 1    - 1    SOC_HR4_PORT_RATIO_QSGMII0
 * #17  qsgmii1: 2.5 - 2.5  - 2.5  - 2.5  SOC_HR4_PORT_RATIO_QSGMII1
 */
void
soc_hurricane4_port_ratio_get(int unit, int port, int *mode)
{
    soc_info_t *si;
    int phy_port_base, lane, qmode;
    int num_lanes[_HR4_PORTS_PER_PBLK];
    int speed_max[_HR4_PORTS_PER_PBLK];

    si = &SOC_INFO(unit);
    phy_port_base = SOC_HR4_PORT_BLOCK_BASE_PORT(port);
    soc_hurricane4_port_qsgmii_mode_get(unit, port, &qmode);

    for (lane = 0; lane < _HR4_PORTS_PER_PBLK; lane++) {
        port = si->port_p2l_mapping[phy_port_base + lane];
        if (port == -1 || SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            num_lanes[lane] = -1;
            speed_max[lane] = -1;
        } else {
            num_lanes[lane] = si->port_num_lanes[port];
            speed_max[lane] = si->port_speed_max[port];
        }
    }

    if (qmode) {
        *mode = (2500 == speed_max[0]) ?
                SOC_HR4_PORT_RATIO_QSGMII1: SOC_HR4_PORT_RATIO_QSGMII0;
        return;
    }

    if (num_lanes[0] == 4) {
        *mode = SOC_HR4_PORT_RATIO_SINGLE;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 2) {
        if (speed_max[0] == speed_max[2]) {
            *mode = SOC_HR4_PORT_RATIO_DUAL_1_1;
        } else if (speed_max[0] > speed_max[2]) {
            *mode = SOC_HR4_PORT_RATIO_DUAL_2_1;
        } else {
            *mode = SOC_HR4_PORT_RATIO_DUAL_1_2;
        }
    } else if (num_lanes[0] == 2) {
        if (num_lanes[2] == -1) {
            *mode = SOC_HR4_PORT_RATIO_DUAL_1_1;
        } else {
            *mode = (speed_max[0] == 20000 || speed_max[0] == 21000 ||
                     speed_max[0] == 50000 || speed_max[0] == 53000) ?
                    SOC_HR4_PORT_RATIO_TRI_023_2_1_1 :
                    SOC_HR4_PORT_RATIO_TRI_023_4_1_1;
        }
    } else if (num_lanes[2] == 2) {
        if (num_lanes[0] == -1) {
            *mode = SOC_HR4_PORT_RATIO_DUAL_1_1;
        } else {
            *mode = (speed_max[2] == 20000 || speed_max[2] == 21000 ||
                     speed_max[2] == 50000 || speed_max[2] == 53000) ?
                    SOC_HR4_PORT_RATIO_TRI_012_1_1_2 :
                    SOC_HR4_PORT_RATIO_TRI_012_1_1_4;
        }
    } else {
        *mode = SOC_HR4_PORT_RATIO_QUAD;
    }
}

/* Function to get the number of ports present in a given Port Macro */
STATIC int
soc_hr4_ports_per_pm_get(int unit, int pm_id)
{
    soc_info_t *si;
    int phy_port, num_ports = 0, i;

    si = &SOC_INFO(unit);
    if (pm_id >= _HR4_PBLKS_PER_DEV(unit)) {
       return SOC_E_PARAM;
    }
    phy_port = 1 + (pm_id * _HR4_PORTS_PER_PBLK);

    for (i = 0; i < _HR4_PORTS_PER_PBLK; ) {
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
_soc_hr4_port_flex_init(int unit, int is_any_cap)
{
    soc_info_t *si;
    int pm, blk_idx, blk_type, i;
    int flex_en, phy_base, max_ports, port, phy_port;
    int qmode = 0;

    si = &SOC_INFO(unit);
    SHR_BITCLR_RANGE(si->flexible_pm_bitmap, 0, SOC_MAX_NUM_BLKS);

    /* portmap_x=y:speed:i */
    /* portmap_x=y:speed:cap */
    if (SOC_PBMP_NOT_NULL(SOC_PORT_DISABLED_BITMAP(unit, all)) || is_any_cap) {
        SHR_BITSET_RANGE(si->flexible_pm_bitmap, 1, SOC_MAX_NUM_BLKS);
    } else {
        /* port_flex_enable{x}=<0...1> */
        /* port_flex_enable_corex=<0...1> */
        /* port_flex_enable=<0...1> */
        for (phy_port = 1; phy_port <= HURRICANE4_GPHY_PORTS_PER_PIPE; phy_port++) {
            port = si->port_p2l_mapping[phy_port];
            if (port == -1) {
                continue;
            }
            soc_hurricane4_port_qsgmii_mode_get(unit, port, &qmode);
            /* Skip PMQ & GPHY */
            if (qmode || (phy_port < 25)) {
                continue;
            }

            /* Verify if PMQ TSC is disabled in HW */
            if (soc_feature(unit, soc_feature_untethered_otp)) {
                if ((soc_feature(unit, soc_feature_PM4x10QInst0Disable) &&
                     (phy_port >= 25) && (phy_port <= 40)) ||
                    (soc_feature(unit, soc_feature_PM4x10QInst1Disable) &&
                     (phy_port >= 41) && (phy_port <= 56))) {
                    continue;
                }
            }

            pm = si->port_serdes[port];
            phy_base = SOC_HR4_PORT_BLOCK_BASE_PORT(port);
            /* port_flex_enable_corex=<0...1> */
            flex_en = soc_property_suffix_num_get_only_suffix
                        (unit, pm, spn_PORT_FLEX_ENABLE, "core", -1);
            /* port_flex_enable{x}=<0...1> */
            /* port_flex_enable=<0...1> */
            if (flex_en == -1) {
                flex_en = soc_property_phys_port_get
                            (unit, phy_base, spn_PORT_FLEX_ENABLE, 0);
            }
            for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
                blk_idx = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
                if (flex_en && (blk_idx != -1)) {
                    SHR_BITSET(si->flexible_pm_bitmap, blk_idx);
                }
            }
        }
    }

    si->flex_eligible =
        (!SHR_BITNULL_RANGE(si->flexible_pm_bitmap, 1, SOC_MAX_NUM_BLKS));

    /* port_flex_max_ports */
    memset(&(si->pm_max_ports), 0xff, sizeof(si->pm_max_ports));
    for (phy_port = 1; phy_port <= HURRICANE4_GPHY_PORTS_PER_PIPE; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        }
        soc_hurricane4_port_qsgmii_mode_get(unit, port, &qmode);
        if (qmode || (phy_port < 25)) {
            continue;
        }
        pm = si->port_serdes[port];
        phy_base = SOC_HR4_PORT_BLOCK_BASE_PORT(port);
        for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
            blk_idx = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
            if (blk_idx == -1) {
                continue;
            }
            blk_type = SOC_BLOCK_TYPE(unit, blk_idx);
            if (SHR_BITGET(si->flexible_pm_bitmap, blk_idx)) {
                /* port_flex_max_ports{x}=y */
                max_ports = soc_property_phys_port_get(unit,
                    phy_base, spn_PORT_FLEX_MAX_PORTS, -1);
                /* port_flex_max_ports_corex=y */
                if (max_ports == -1) {
                    max_ports = soc_property_suffix_num_get(unit, pm,
                        spn_PORT_FLEX_MAX_PORTS, "core", 4);
                }
                /* validation check */
                if ((max_ports < 0) || (max_ports > 16) ) {
                    LOG_CLI((BSL_META_U(unit,
                        "Core %d: Bad port_flex_max_ports %d"),
                        pm, max_ports));
                    return SOC_E_CONFIG;
                }
                si->pm_max_ports[blk_idx] = max_ports;
            } else {
                si->pm_max_ports[blk_idx] =
                    (blk_type == SOC_BLK_GXPORT) ? ((phy_base > 24) ? 16 : 8) : 4;
            }
        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_hr4_port_lanes_set(int unit, int phy_port, int *lanes, int port_bandwidth)
{
    if (HR4_PORT_IS_FALCON(unit, phy_port)) {
        *lanes = (port_bandwidth > 27000) ? -1
                 : (20000 == SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(port_bandwidth)) ? 2
                 : 1;
    } else if (HR4_PORT_IS_MACSEC(unit, phy_port)) {
        *lanes = (port_bandwidth == 20000 ||
                  port_bandwidth == 25000 ||
                  port_bandwidth == 10000) ? 1 : -1;
    } else if (phy_port >= 1 && phy_port <= 24) {
        *lanes = (port_bandwidth <= 1000) ? 1 : -1;
    } else {
        *lanes = (port_bandwidth > 21000) ? -1
                 : (port_bandwidth > 11000) ? 2
                 : 1;
    }

    if (HR4_PORT_IS_PMQ1(unit, phy_port) && (port_bandwidth > 11000)) {
         *lanes = -1;
    }

    if (soc_property_phys_port_get(unit, 25, spn_PORT_GMII_MODE, 0) == 0) {
        if (phy_port >= 29 && phy_port <= 40) {
            return SOC_E_CONFIG;
        }
    }
    if (soc_property_phys_port_get(unit, 41, spn_PORT_GMII_MODE, 0) == 0) {
        if (phy_port >= 45 && phy_port <= 56) {
            return SOC_E_CONFIG;
        }
    }

    return ((*lanes == -1) ? SOC_E_CONFIG : SOC_E_NONE);
}

STATIC void
_soc_hr4_mmu_idb_ports_assign(int unit)
{
    int port, phy_port, mmu_port, idb_port;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    if  (SOC_WARM_BOOT(unit) && (si->flex_eligible) ) {
        /* Port mappings recovered from Scache */
        return;
    }

    /* PHY Ports: 1-56, 57-64
     * MMU Ports: 8-63, 0-7
     * IDB Ports: 0-55, 56-63
     */
    for (phy_port = 1; phy_port <= 64; phy_port++) {
        mmu_port = (phy_port > 56) ? (phy_port - 57) : (phy_port + 7);
        idb_port = phy_port - 1;
        si->port_p2m_mapping[phy_port] = mmu_port;
        si->port_m2p_mapping[mmu_port] = phy_port;

        port = si->port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        }
        si->port_l2i_mapping[port] = idb_port;
    }

    return;
}

/*
 * Hurricane4 port mapping
 *
 *                   physical   idb/       device   mmu
 *                   port       idb port   port     port
 *     CMIC          0          0/70       0        70
 *     GPHY          1-24       0/0-23     1-24     8-31
 *     XLPORTQ25-40  25-40      0/24-39    25-40    32-47
 *     XLPORTQ41-56  41-56      0/40-55    41-56    48-63
 *     CLPORT57-60   57-60      1/56-59    57-60    0-3
 *     XLPORT61-64   61-64      0/60-63    41-56    4-7
 *     LBPORT        69         0/71       70       71
 *     IPFIX         70         1/69       71       69
 *     MACSEC1       65         1/65       65       65
 *     MACSEC2       66         1/66       66       66
 *     MACSEC3       67         1/67       67       67
 *     MACSEC4       68         1/68       68       68
 * Although MMU port number is flexible within the above range, it is
 *     configured as a fixed mapped to physical port number
 */
int
soc_hurricane4_port_config_init(int unit, uint16 dev_id)
{
#ifdef BCM_56275_A0
    soc_info_t *si;
    char *config_str, *sub_str, *sub_str_end;
    static const char str_2p5[] = "2.5";
    char str_buf[8];
    int rv;
    int num_port, num_phy_port, num_mmu_port;
    int port, phy_port, mmu_port;
    int pipe, index, bandwidth_cap, is_any_cap = FALSE;
    int port_bandwidth;
    char option1, option2;

    si = &SOC_INFO(unit);

    num_phy_port = HR4_NUM_PHY_PORT;
    num_port = HR4_NUM_PORT;
    num_mmu_port = HR4_NUM_MMU_PORT;

    soc_hurricane4_max_frequency_get(unit, dev_id, -1, -1, &si->frequency);

    si->bandwidth = 2048000;


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
    for (index = 0; index < COUNTOF(hr4_anc_ports); index++) {
        port = hr4_anc_ports[index].port;
        phy_port = hr4_anc_ports[index].phy_port;
        pipe = hr4_anc_ports[index].pipe;;

        si->port_l2p_mapping[port] = phy_port;
        si->port_l2i_mapping[port] = hr4_anc_ports[index].idb_port;
        si->port_p2l_mapping[phy_port] = port;
        si->port_p2m_mapping[phy_port] = hr4_anc_ports[index].mmu_port;
        si->port_pipe[port] = pipe;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);
    }

    rv = SOC_E_NONE;
    for (port = 1; port < num_port; port++) {
        /* fixed mapped port */
        if ((si->port_l2p_mapping[port] != -1) && (port < 65)) {
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

        /* Reserved for MACSEC ports only */
        if ((HR4_PORT_IS_MACSEC(unit, phy_port) ||
             HR4_PORT_IS_MACSEC(unit, port)) && phy_port != port) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Reserved for MACSEC invalid physical port number %d\n"),
                     port, phy_port));
            rv = SOC_E_FAIL;
            continue;
        }

        if (port > 68) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Invalid logical port number %d\n"),
                     port, port));
            rv = SOC_E_FAIL;
            continue;
        }

        /* coverity[check_after_sink : FALSE] */
        if ((si->port_p2l_mapping[phy_port] != -1) && (phy_port < 65)) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Physical port %d is used by port %d\n"),
                     port, phy_port, si->port_p2l_mapping[phy_port]));
            rv = SOC_E_FAIL;
            continue;
        }

        /* Verify if PMQ TSC is disabled in HW */
        if (soc_feature(unit, soc_feature_untethered_otp)) {
            if (soc_feature(unit, soc_feature_PM4x10QInst0Disable) &&
                (phy_port >= 25) && (phy_port <= 40)) {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Physical port %d PMQ#0 disabled\n"),
                         port, phy_port));
                rv = SOC_E_FAIL;
                continue;
            }

            if (soc_feature(unit, soc_feature_PM4x10QInst1Disable) &&
                (phy_port >= 41) && (phy_port <= 56)) {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Physical port %d PMQ#1 disabled\n"),
                         port, phy_port));
                rv = SOC_E_FAIL;
                continue;
            }
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
            case 5000:
            case 10000:
            case 11000:
            case 20000:
            case 21000:
            case 25000:
            case 27000:
                break;
            default:
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Invalid bandwidth %d Gb\n"),
                         port, port_bandwidth / 1000));
                rv = SOC_E_FAIL;
                continue;
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
                    sub_str = sub_str_end;
                    _soc_hr4_port_speed_cap[unit][port] = bandwidth_cap;
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
        pipe = 0;

        si->port_pipe[port] = pipe;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);
        si->port_speed_max[port] = port_bandwidth;
        si->port_init_speed[port] = port_bandwidth;
        if (option1 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
            si->port_num_lanes[port] = -1;
        } else {
            uint32 mask;
            switch (option1) {
            case '1': si->port_num_lanes[port] = 1; break;
            case '2': si->port_num_lanes[port] = 2; break;
            case '4': si->port_num_lanes[port] = 4; break;
            default:
                if (SOC_FAILURE(_soc_hr4_port_lanes_set(unit, phy_port,
                                                        &si->port_num_lanes[port],
                                                        port_bandwidth))) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Port %d: Bandwidth %d phy_port %d\n"),
                             port, port_bandwidth, phy_port));
                    rv = SOC_E_FAIL;
                }
                break;
            }
            SOC_IF_ERROR_RETURN(soc_hr4_support_speeds(unit, port, &mask));
            if (!(SOC_PA_SPEED(port_bandwidth) & mask)) {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Bandwidth %d phy_port %d mask 0x%x\n"),
                         port, port_bandwidth, phy_port, mask));
                rv = SOC_E_FAIL;
            }
         }
        if (option2 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
            si->port_num_lanes[port] = -1;
        }
        si->port_serdes[port] = HR4_PHY_PORT_SERDERS(phy_port);
    }


    /* get flex port properties */
    if (SOC_FAILURE(_soc_hr4_port_flex_init(unit, is_any_cap))) {
        rv = SOC_E_FAIL;
    }

    _soc_hr4_mmu_idb_ports_assign(unit);
#if defined(BCM_WARM_BOOT_SUPPORT)
    if (si->flex_eligible) {
        if (SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN(soc_hr4_flexport_scache_recovery(unit));
        } else {
            SOC_IF_ERROR_RETURN(soc_hr4_flexport_scache_allocate(unit));
        }
    }
#endif
    

    si->xpe_ipipe_map[0] = 1;
    si->xpe_epipe_map[0] = 1;
    si->sc_ipipe_map[0] = 1;
    si->sc_epipe_map[0] = 1;
    si->ipipe_xpe_map[0] = 1;
    si->epipe_xpe_map[0] = 1;
    si->ipipe_sc_map[0] = 1;
    si->epipe_sc_map[0] = 1;

    return rv;
#else
    return SOC_E_UNAVAIL;
#endif

}

static uint32 _soc_hr4_temp_mon_mask[SOC_MAX_NUM_DEVICES];

int
soc_hurricane4_uft_uat_config(int unit)
{

    int index, shared_bank_offset = 2;
    int num_l2_banks, num_l3_banks, num_fpem_banks = 0;
    int dedicated_banks = 2;
    int offset_base, offset_multiplier ;
    int mpls_banks;
    int vlan_xlate_1_banks;
    int vlan_xlate_2_banks;
    int egr_vlan_xlate_1_banks, egr_vlan_xlate_2_banks;
    uint32 hash_control_entry[4], shared_hash_control_entry[4];
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
        B0_HASH_OFFSETf, B1_HASH_OFFSETf, B2_HASH_OFFSETf,
        B3_HASH_OFFSETf, B4_HASH_OFFSETf, B5_HASH_OFFSETf,
        B6_HASH_OFFSETf, B7_HASH_OFFSETf
    };
    /* UFT/UAT control memories programming */
    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, L2Xm, &num_l2_banks));
    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, L3_ENTRY_ONLY_SINGLEm,
                                          &num_l3_banks));

    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, EGR_VLAN_XLATE_1_SINGLEm,
                                          &egr_vlan_xlate_1_banks));
    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, EGR_VLAN_XLATE_2_SINGLEm,
                                          &egr_vlan_xlate_2_banks));
    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, MPLS_ENTRY_SINGLEm,
                                          &mpls_banks));
    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, VLAN_XLATE_1_SINGLEm,
                                          &vlan_xlate_1_banks));
    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, VLAN_XLATE_2_SINGLEm,
                                          &vlan_xlate_2_banks));


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

    offset_multiplier = 64 / num_l2_banks;
    offset_base = 64 % num_l2_banks;

    for (index = 0; index < dedicated_banks; index++) {
        soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm, hash_control_entry,
                            dedicated_fields[index], offset_base + (index *
                            offset_multiplier));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_L2_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0, hash_control_entry));

    sal_memset(hash_control_entry, 0, sizeof(l3_entry_hash_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                        hash_control_entry));
    shared_bank_offset += num_fpem_banks;
    offset_multiplier = 64 / num_l3_banks;
    offset_base = 64 % num_l3_banks;
    soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, ((((1 << (num_l3_banks -
                        dedicated_banks)) - 1) << shared_bank_offset) |
                        ((1 << dedicated_banks) - 1)));

    for (index = 0; index < dedicated_banks; index++) {
        soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm, hash_control_entry,
                            dedicated_fields[index], offset_base + (index *
                            offset_multiplier));
    }

    for (index = dedicated_banks ; index < num_l3_banks ; index++) {
        soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm, hash_control_entry,
                          logical_to_physical_fields[index],
                          index + shared_bank_offset - dedicated_banks);
    }
    SOC_IF_ERROR_RETURN
        (WRITE_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                        hash_control_entry));

    sal_memset(shared_hash_control_entry, 0,
                sizeof(ep_uat_shared_banks_control_entry_t));
    sal_memset(hash_control_entry, 0,
                sizeof(egr_vlan_xlate_1_hash_control_entry_t));

    if (egr_vlan_xlate_1_banks) {
        offset_base = 64 % egr_vlan_xlate_1_banks;
        offset_multiplier = 64 / egr_vlan_xlate_1_banks;
    }

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
        soc_mem_field32_set(unit, EGR_VLAN_XLATE_1_HASH_CONTROLm,
                            hash_control_entry,
                            logical_to_physical_fields[index],
                            index);
        soc_mem_field32_set(unit, EP_UAT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index],
                            offset_base + (index * offset_multiplier));
    }
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));

    sal_memset(hash_control_entry, 0,
                sizeof(egr_vlan_xlate_2_hash_control_entry_t));

    if (egr_vlan_xlate_2_banks) {
        offset_base = 64 % egr_vlan_xlate_2_banks;
        offset_multiplier = 64 / egr_vlan_xlate_2_banks;
    }

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                hash_control_entry));
    soc_mem_field32_set(unit, EGR_VLAN_XLATE_2_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf,
                        ((1 << egr_vlan_xlate_2_banks) - 1) <<
                        egr_vlan_xlate_1_banks) ;

    for(index = 0; index < egr_vlan_xlate_2_banks; index++) {
        soc_mem_field32_set(unit, EGR_VLAN_XLATE_2_HASH_CONTROLm,
                            hash_control_entry,
                            logical_to_physical_fields[index],
                            index + egr_vlan_xlate_1_banks);
        soc_mem_field32_set(unit, EP_UAT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index +
                            egr_vlan_xlate_1_banks], offset_base + (index *
                            offset_multiplier));
    }
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
    SOC_IF_ERROR_RETURN
        (WRITE_EP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                shared_hash_control_entry));

    sal_memset(shared_hash_control_entry, 0,
                sizeof(ip_uat_shared_banks_control_entry_t));
    sal_memset(shared_hash_control_entry, 0,
                sizeof(ip_uat_shared_banks_control_entry_t));
    sal_memset(hash_control_entry, 0,
                sizeof(mpls_entry_hash_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_IP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                shared_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (READ_IP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                shared_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (READ_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                hash_control_entry));

    if (mpls_banks) {
        offset_base = 64 % (mpls_banks);
        offset_multiplier = 64 / (mpls_banks);
    }
    soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, ((1 << mpls_banks) - 1));
    for(index = 0; index < mpls_banks; index++) {
        soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, hash_control_entry,
                            logical_to_physical_fields[index], index);
        soc_mem_field32_set(unit, IP_UAT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index],
                            offset_base + ( index * offset_multiplier));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));

    sal_memset(hash_control_entry, 0,
                sizeof(vlan_xlate_1_hash_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                hash_control_entry));
    if (vlan_xlate_1_banks) {
        offset_base = 64 % (vlan_xlate_1_banks);
        offset_multiplier = 64 / (vlan_xlate_1_banks);
    }
    soc_mem_field32_set(unit, VLAN_XLATE_1_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, ((((1 << vlan_xlate_1_banks)
                        - 1) << mpls_banks)));

    for(index = 0; index < vlan_xlate_1_banks; index++) {
        soc_mem_field32_set(unit, VLAN_XLATE_1_HASH_CONTROLm,
                            hash_control_entry,
                            logical_to_physical_fields[index], index +
                            mpls_banks);
        soc_mem_field32_set(unit, IP_UAT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index +
                            mpls_banks], offset_base + (index *
                            offset_multiplier));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));

    sal_memset(hash_control_entry, 0,
                sizeof(vlan_xlate_2_hash_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                hash_control_entry));
    if (vlan_xlate_2_banks) {
        offset_base = 64 % (vlan_xlate_2_banks);
        offset_multiplier = 64 / (vlan_xlate_2_banks);
    }
    soc_mem_field32_set(unit, VLAN_XLATE_2_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, ((((1 << vlan_xlate_2_banks)
                        - 1) << (mpls_banks + vlan_xlate_1_banks))));

    for(index = 0; index < vlan_xlate_2_banks; index++) {
        soc_mem_field32_set(unit, VLAN_XLATE_2_HASH_CONTROLm, hash_control_entry,
                            logical_to_physical_fields[index], index +
                            mpls_banks + vlan_xlate_1_banks);
        soc_mem_field32_set(unit, IP_UAT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index +
                            mpls_banks + vlan_xlate_1_banks], offset_base
                            + (index * offset_multiplier));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
    SOC_IF_ERROR_RETURN
        (WRITE_IP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                shared_hash_control_entry));

    /* Create a mutex for s/w re-ordering */
    SOC_IF_ERROR_RETURN(soc_td3_hash_mutex_init (unit));

    return SOC_E_NONE;
}


/*
 *  PM4X10_QTC core has two work modes: Q(QTC) mode and Ethernet(XLPORT) mode.
 *  In QTC mode, QTC is reset via register PMQ_XGXS0_CTRL_REGr which is belong to SOC_BLK_PMQPORT block.
 *  In Eth mode, TSC is reset via register XLPORT_XGXS0_CTRL_REGr which is belong to SOC_BLK_XLPORT block.
 *  To reset QTC or TSC core, the work mode must be identified by register CHIP_CONFIGr which is configured in soc_hurricane4_chip_reset.
 */
int
soc_hurricane4_port_reset(int unit)
{
    int blk, port;
    uint32 rval;
    uint64 rval64;
    uint32 val = 1;
    soc_info_t *si;
    uint32 index, qtc_mode = ETHER_MODE;
    int sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;

    si = &SOC_INFO(unit);

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
        if (IS_QSGMII_PORT(unit, port)) {
            continue;
        }

        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 1));
    }
    sal_usleep(sleep_usec + 10000);
    /* Power on XLPORT blocks */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        if (IS_QSGMII_PORT(unit, port)) {
            continue;
        }

        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 0));

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, XLPORT_XGXS0_CTRL_REGr, port, 0, &rval64));
        if (((si->port_l2p_mapping[port] >= 25) &&
             (si->port_l2p_mapping[port] <= 28)) ||
            ((si->port_l2p_mapping[port] >= 61) &&
             (si->port_l2p_mapping[port] <= 64))) {
            /* External Clock */
            val = 1;
        } else {
            /* Internal Clock */
            val = 5;
        }
        soc_reg64_field32_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval64, REFSELf, val);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, XLPORT_XGXS0_CTRL_REGr, port, 0, rval64));
    }

    /* Power off PMQ blocks */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_PMQPORT) {
        port = blk | SOC_REG_ADDR_BLOCK_ID_MASK;
        index = SOC_BLOCK_NUMBER(unit, blk);

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, index, 0, &rval64));
        qtc_mode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64, QMODEf);
        if (qtc_mode == Q_MODE) {
            SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 1));
        }
    }
    sal_usleep(sleep_usec + 10000);
    /* Power on PMQ blocks */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_PMQPORT) {
        port = blk | SOC_REG_ADDR_BLOCK_ID_MASK;
        index = SOC_BLOCK_NUMBER(unit, blk);

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, index, 0, &rval64));
        qtc_mode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64, QMODEf);
        if (qtc_mode == Q_MODE) {
            SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 0));

            SOC_IF_ERROR_RETURN(soc_reg_get(unit, PMQ_XGXS0_CTRL_REGr, port, 0, &rval64));
            if (index == 0) {
                /* External Clock */
                val = 1;
            } else {
                /* Internal Clock */
                val = 5;
            }
            soc_reg64_field32_set(unit, PMQ_XGXS0_CTRL_REGr, &rval64, REFSELf, val);
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, PMQ_XGXS0_CTRL_REGr, port, 0, rval64));
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
        SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
        sal_udelay(10);
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));

        if (IS_QSGMII_PORT(unit, port)) {
            continue;
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_hurricane4_port_mapping_init(int unit)
{
    soc_info_t *si;
    int port, phy_port, idb_port, i;
    soc_reg_t reg;
    soc_mem_t mem;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe = 0;
    int max_idx = 0;

    si = &SOC_INFO(unit);

    /* Ingress IDB to device port mapping */
    sal_memset(&entry, 0, sizeof(entry));
    mem = SOC_MEM_UNIQUE_ACC(unit, ING_PHY_TO_IDB_PORT_MAPm)[pipe];

    /* Set all entries to 0x7f as default */
    soc_mem_field32_set(unit, mem, &entry, IDB_PORTf, 0x7f);
    soc_mem_field32_set(unit, mem, &entry, VALIDf, 0);
    max_idx = soc_mem_index_max(unit, ING_PHY_TO_IDB_PORT_MAPm);
    for (phy_port = 0; phy_port <= max_idx; phy_port++) {
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port, &entry));
    }

    /* Ingress IDB to device port mapping */
    sal_memset(&entry, 0,
               sizeof(ing_idb_to_device_port_number_mapping_table_entry_t));
    mem = SOC_MEM_UNIQUE_ACC(unit, ING_IDB_TO_DEVICE_PORT_NUMBER_MAPPING_TABLEm)[pipe];
    /* Set all entries to 0x7f since Device Port No. 0 corresponds to CPU port*/
    soc_mem_field32_set(unit, mem, &entry, DEVICE_PORT_NUMBERf, 0x7f);
    for (idb_port = 0; idb_port < HR4_NUM_PORT; idb_port++) {
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, idb_port, &entry));
    }

    /* Ancillary ports */
    for (i = 0; i < COUNTOF(hr4_anc_ports); i++) {
        idb_port = hr4_anc_ports[i].idb_port;
        phy_port = si->port_m2p_mapping[idb_port];
        port = si->port_p2l_mapping[phy_port];

        soc_mem_field32_set(unit, mem, &entry, DEVICE_PORT_NUMBERf, port);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, idb_port, &entry));

        /* Egress device port to physical port mapping */
        rval = 0;
        reg = EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr;
        soc_reg_field_set(unit, reg, &rval, PHYSICAL_PORT_NUMBERf, phy_port);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));

        /* MMU port to physical port mapping */
        rval = 0;
        reg = MMU_PORT_TO_PHY_PORT_MAPPINGr;
        soc_reg_field_set(unit, reg, &rval, PHY_PORTf, phy_port);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));

        /* MMU port to device port mapping */
        rval = 0;
        reg = MMU_PORT_TO_DEVICE_PORT_MAPPINGr;
        soc_reg_field_set(unit, reg, &rval, DEVICE_PORTf, port);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }
    /* Ingress GPP port to device port mapping */
    mem = SYS_PORTMAPm;
    sal_memset(&entry, 0, sizeof(sys_portmap_entry_t));
    for (port = 0; port < HR4_NUM_PORT; port++) {
        soc_mem_field32_set(unit, mem, &entry, DEVICE_PORT_NUMBERf, port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &entry));
    }

    /* Handled in DV flexport code */
    if (!soc_feature(unit, soc_feature_port_leverage)) {
        sal_memset(&entry, 0,
                   sizeof(ing_idb_to_device_port_number_mapping_table_entry_t));
        mem = SOC_MEM_UNIQUE_ACC(unit, ING_IDB_TO_DEVICE_PORT_NUMBER_MAPPING_TABLEm)[pipe];
        for (port = 0; port < HR4_NUM_PORT; port++) {
            idb_port = si->port_l2i_mapping[port];

            if (idb_port == -1) {
                continue;
            }
            soc_mem_field32_set(unit, mem, &entry, DEVICE_PORT_NUMBERf, port);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, idb_port, &entry));
        }

        /* Ingress physical to IDB port mapping
         * This table is programmed only for front panel ports
         */
        sal_memset(&entry, 0, sizeof(ing_phy_to_idb_port_map_entry_t));
        mem = SOC_MEM_UNIQUE_ACC(unit, ING_PHY_TO_IDB_PORT_MAPm)[pipe];
        for (phy_port = 1; phy_port < (HR4_NUM_PORT - 1); phy_port++) {
            port = si->port_p2l_mapping[phy_port];

            if (port != -1) {
                idb_port = si->port_l2i_mapping[port];
                if (idb_port == -1) {
                    return SOC_E_INTERNAL;
                }

                soc_mem_field32_set(unit, mem, &entry, IDB_PORTf, idb_port);
                soc_mem_field32_set(unit, mem, &entry, VALIDf, 1);
            } else {
                soc_mem_field32_set(unit, mem, &entry, VALIDf, 0);
            }

            SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port - 1, &entry));
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

        /* MMU port to device port mapping */
        rval = 0;
        reg = MMU_PORT_TO_DEVICE_PORT_MAPPINGr;
        PBMP_ALL_ITER(unit, port) {
            soc_reg_field_set(unit, reg, &rval, DEVICE_PORTf, port);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_hurricane4_latency_config
 * Purpose:
 *      Configure latency in SLOT_PIPELINE_CONFIG register
 * Parameters:
 *      unit (IN)    - switch unit
 * Returns:
 *      SOC_E_*
 */
STATIC int
_soc_hurricane4_latency_config(int unit)
{
    soc_reg_t reg;
    uint32 rval;

    reg = SLOT_PIPELINE_CONFIGr;
    rval = 0;

    soc_reg_field_set(unit, reg, &rval, LATENCYf, 387);
    soc_reg_field_set(unit, reg, &rval, WR_ENf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_hr4_set_idb_dpp_ctrl
 * Purpose:
 *      Configure IDB DPP credits
 * Parameters:
 *      unit (IN)    - switch unit
 * Returns:
 *      SOC_E_*
 */
static int
_soc_hr4_set_idb_dpp_ctrl(int unit)
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
        soc_reg_field_set(unit, reg, &rval, CREDITSf, 40);
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
_soc_hurricane4_tdm_init(int unit, soc_port_schedule_state_t *port_schedule_state)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_map_type_t *in_portmap;
    int port, speed;
    int i = 0;
    int present = 0;

    if (soc->tdm_info == NULL) {
        soc->tdm_info = sal_alloc(sizeof(_soc_hurricane4_tdm_temp_t),
                                    "Hurricane4 TDM info");
        if (soc->tdm_info == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(soc->tdm_info, 0, sizeof(_soc_hurricane4_tdm_temp_t));
    }
    sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));
#if defined(BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_hr4_tdm_scache_recovery(unit));
        return SOC_E_NONE;
    } else {
        SOC_IF_ERROR_RETURN(
            soc_hr4_tdm_scache_allocate(unit));
    }
#endif
    /* Core clock frequency */
    port_schedule_state->frequency = si->frequency;
    port_schedule_state->bandwidth = si->bandwidth;
    port_schedule_state->io_bandwidth = si->io_bandwidth;
    port_schedule_state->lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);;
    port_schedule_state->is_flexport = 0;
    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META_U
               (unit, "Freq:%d BW:%d IO_BW:%d\n"),
               si->frequency, si->bandwidth, si->io_bandwidth));

    /* Construct in_port_map */
    in_portmap = &port_schedule_state->in_port_map;
    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            continue;
        }
        if (IS_HG_PORT(unit, port)) {
            speed = soc_port_speed_higig2eth(si->port_speed_max[port]);
            port_schedule_state->resource[i].encap = SOC_ENCAP_HIGIG2;
        } else {
            speed = si->port_speed_max[port];
        }
        in_portmap->log_port_speed[port] = speed;
        in_portmap->port_num_lanes[port] = si->port_num_lanes[port];

        port_schedule_state->resource[i].physical_port = si->port_l2p_mapping[port];
        port_schedule_state->resource[i].logical_port = port;
        port_schedule_state->resource[i].idb_port = si->port_l2i_mapping[port];
        port_schedule_state->resource[i].mmu_port = si->port_p2m_mapping[si->port_l2p_mapping[port]];
        port_schedule_state->resource[i].num_lanes = si->port_num_lanes[port];
        port_schedule_state->resource[i].speed = speed;
        port_schedule_state->resource[i].pipe = 0;
        port_schedule_state->resource[i].oversub = IS_OVERSUB_PORT(unit, port) ? 1 : 0;
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META_U
                   (unit, "%d: port %d speed %d phy %d idb %d mmu %d lane %d over %d\n"),
                   i,
                   port_schedule_state->resource[i].logical_port,
                   port_schedule_state->resource[i].speed,
                   port_schedule_state->resource[i].physical_port,
                   port_schedule_state->resource[i].idb_port,
                   port_schedule_state->resource[i].mmu_port,
                   port_schedule_state->resource[i].num_lanes,
                   port_schedule_state->resource[i].oversub));
        port_schedule_state->nport = ++i;
    }
    /* MACSEC ports */
    for (port = 65; port < 69 ; port++) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            continue;
        }
        if (si->port_l2p_mapping[port] == -1) {
            continue;
        }
        speed = si->port_speed_max[port];
        if (speed <= 0 ) {
            if (si->frequency == HR4_CLK_DEFAULT) {
                speed = 20000;
                si->port_speed_max[port] = speed;
                si->port_init_speed[port] = speed;
            } else {
                continue;
            }
        } else {
            if (soc_feature(unit, soc_feature_untethered_otp) &&
                !soc_feature(unit, soc_feature_xflow_macsec)) {
                continue;
            }
        }
        si->port_num_lanes[port] = 1;

        in_portmap->log_port_speed[port] = speed;
        in_portmap->port_num_lanes[port] = si->port_num_lanes[port];

        port_schedule_state->resource[i].physical_port = si->port_l2p_mapping[port];
        port_schedule_state->resource[i].logical_port = port;
        port_schedule_state->resource[i].idb_port = si->port_l2i_mapping[port];
        port_schedule_state->resource[i].mmu_port = si->port_p2m_mapping[si->port_l2p_mapping[port]];
        port_schedule_state->resource[i].num_lanes = si->port_num_lanes[port];
        port_schedule_state->resource[i].speed = speed;
        port_schedule_state->resource[i].pipe = 0;
        port_schedule_state->resource[i].oversub = IS_OVERSUB_PORT(unit, port) ? 1 : 0;
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META_U
                   (unit, "%d: port %d speed %d phy %d idb %d mmu %d lane %d over %d\n"),
                   i,
                   port_schedule_state->resource[i].logical_port,
                   port_schedule_state->resource[i].speed,
                   port_schedule_state->resource[i].physical_port,
                   port_schedule_state->resource[i].idb_port,
                   port_schedule_state->resource[i].mmu_port,
                   port_schedule_state->resource[i].num_lanes,
                   port_schedule_state->resource[i].oversub));
        port_schedule_state->nport = ++i;
    }
    sal_memcpy(in_portmap->port_p2l_mapping, si->port_p2l_mapping,
                sizeof(int)*HURRICANE4_TDM_PHY_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_l2p_mapping, si->port_l2p_mapping,
                sizeof(int)*_HR4_TDM_DEV_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_p2m_mapping, si->port_p2m_mapping,
                sizeof(int)*HURRICANE4_TDM_PHY_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_m2p_mapping, si->port_m2p_mapping,
                sizeof(int)*_HR4_TDM_MMU_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_l2i_mapping, si->port_l2i_mapping,
                sizeof(int)*_HR4_TDM_DEV_PORTS_PER_DEV);
    sal_memcpy(&in_portmap->physical_pbm, &si->physical_pbm, sizeof(pbmp_t));
    sal_memcpy(&in_portmap->hg2_pbm, &si->hg.bitmap, sizeof(pbmp_t));
    sal_memcpy(&in_portmap->management_pbm, &si->management_pbm,
                sizeof(pbmp_t));
    sal_memcpy(&in_portmap->oversub_pbm, &si->oversub_pbm, sizeof(pbmp_t));

    SOC_IF_ERROR_RETURN(soc_hr4_port_schedule_tdm_init(unit, port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_hr4_tdm_init(unit, port_schedule_state));
    /* Flexport dependency */
    if (soc_feature(unit, soc_feature_port_leverage)) {
        if (!soc_feature(unit, soc_feature_egphy_port_leverage)) {
            /* Restore the flexport function pointers for gphy */
            SOC_IF_ERROR_RETURN
                (soc_check_flex_phase(unit, hr4_flexport_phases,
                                      soc_hurricane4_flex_top_port_down,
                                      &present));
            if (!present) {
                SOC_IF_ERROR_RETURN
                    (soc_attach_flex_phase(unit, hr4_flexport_phases,
                                           soc_hurricane4_flex_top_port_down, 1));
            }
            present = 0;
            SOC_IF_ERROR_RETURN
                (soc_check_flex_phase(unit, hr4_flexport_phases,
                                      soc_hurricane4_flex_top_port_up,
                                      &present));
            if (!present) {
                SOC_IF_ERROR_RETURN
                    (soc_attach_flex_phase(unit, hr4_flexport_phases,
                                           soc_hurricane4_flex_top_port_up, 8));
            }
        }
        SOC_IF_ERROR_RETURN
            (soc_hurricane4_reconfigure_ports(unit, port_schedule_state));
    }
    SOC_IF_ERROR_RETURN(soc_hr4_soc_tdm_update(unit, port_schedule_state));

    return SOC_E_NONE;
}

/* IDB, EDB configuration for Front Panel ports handled in DV flexport code */
STATIC int
_soc_hurricane4_edb_init(int unit)
{
    int i;
    int port, phy_port;
    soc_info_t *si;
    uint32 entry[SOC_MAX_MEM_WORDS];
    si = &SOC_INFO(unit);

    for (i = 0; i < COUNTOF(hr4_anc_ports); i++) {
        phy_port = si->port_l2p_mapping[hr4_anc_ports[i].port];

        /* MACSEC ports handled in DV flexport code */
        if (soc_feature(unit, soc_feature_port_leverage) &&
            HR4_PORT_IS_MACSEC(unit, phy_port)) {
            continue;
        }

        /* Configure egress mmu cell credit */
        sal_memset(entry, 0, sizeof(egr_mmu_cell_credit_entry_t));
        if (HR4_PORT_IS_MACSEC(unit, phy_port)) {
            soc_EGR_MMU_CELL_CREDITm_field32_set(unit, &entry, CREDITf, 32);
        } else if (phy_port == 69) {
            soc_EGR_MMU_CELL_CREDITm_field32_set(unit, &entry, CREDITf, 96);
        } else {
            soc_EGR_MMU_CELL_CREDITm_field32_set(unit, &entry, CREDITf, 16);
        }
        SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CELL_CREDITm
            (unit, MEM_BLOCK_ALL, hr4_anc_ports[i].phy_port, &entry));

        /* Configure egress transmit start count */
        sal_memset(entry, 0, sizeof(egr_xmit_start_count_pipe0_entry_t));
        if (HR4_PORT_IS_MACSEC(unit, phy_port)) {
            soc_EGR_XMIT_START_COUNT_PIPE0m_field32_set(unit, &entry,
                                                        THRESHOLDf, 9);
        } else {
            soc_EGR_XMIT_START_COUNT_PIPE0m_field32_set(unit, &entry,
                                                        THRESHOLDf, 7);
        }
        SOC_IF_ERROR_RETURN(WRITE_EGR_XMIT_START_COUNT_PIPE0m
            (unit, MEM_BLOCK_ALL, hr4_anc_ports[i].port, &entry));

        /* Enable egress */
        sal_memset(entry, 0, sizeof(egr_enable_entry_t));
        soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEm
            (unit, MEM_BLOCK_ALL, hr4_anc_ports[i].phy_port, entry));
    }

    /* Handled in DV flexport code */
    if (!soc_feature(unit, soc_feature_port_leverage)) {
        PBMP_ALL_ITER(unit, port) {
            for (i = 0; i < COUNTOF(hr4_anc_ports); i++) {
                if (hr4_anc_ports[i].port == port) {
                    break;
                }
            }

            if (i < COUNTOF(hr4_anc_ports)) {
                continue;
            }

            phy_port = si->port_l2p_mapping[port];
            if (phy_port == -1) {
                continue;
            }

            /* Configure egress mmu cell credit */
            sal_memset(entry, 0, sizeof(egr_mmu_cell_credit_entry_t));
            if (phy_port >= 57 && phy_port <= 60) {
                soc_EGR_MMU_CELL_CREDITm_field32_set(unit, &entry, CREDITf, 32);
            } else if (phy_port >= 61 && phy_port <= 64) {
                soc_EGR_MMU_CELL_CREDITm_field32_set(unit, &entry, CREDITf, 21);
            } else if (phy_port >= 25 && phy_port <= 56 &&
                       (!IS_QSGMII_PORT(unit, port))) {
                soc_EGR_MMU_CELL_CREDITm_field32_set(unit, &entry, CREDITf, 21);
            } else {
                soc_EGR_MMU_CELL_CREDITm_field32_set(unit, &entry, CREDITf, 5);
            }
            SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CELL_CREDITm
                (unit, MEM_BLOCK_ALL, phy_port, &entry));

            /* Configure egress transmit start count */
            sal_memset(entry, 0, sizeof(egr_xmit_start_count_pipe0_entry_t));
            /* 4x25 its 9 */
            if (phy_port >= 57 && phy_port <= 60) {
                soc_EGR_XMIT_START_COUNT_PIPE0m_field32_set(unit, &entry,
                                                            THRESHOLDf, 9);
            } else {
                soc_EGR_XMIT_START_COUNT_PIPE0m_field32_set(unit, &entry,
                                                            THRESHOLDf, 7);
            }
            SOC_IF_ERROR_RETURN(WRITE_EGR_XMIT_START_COUNT_PIPE0m
                (unit, MEM_BLOCK_ALL, phy_port, &entry));

            /* Enable egress */
            sal_memset(entry, 0, sizeof(egr_enable_entry_t));
            soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEm
                (unit, MEM_BLOCK_ALL, phy_port, entry));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_hurricane4_idb_init(int unit)
{
    soc_reg_t reg;
    uint32 rval0;
    uint32 rval;
    soc_info_t *si;
    int block_info_idx;
    int idx, count = 0;
    int phy_port_base, port;
    int lane, mode;
    static const int soc_hr4_hw_mode_values[SOC_HR4_PORT_RATIO_COUNT] = {
        0, 1, 1, 1, 3, 5, 4, 6, 2, 7, 8
    };
    static soc_field_t port_fields[_HR4_PORTS_PER_PMQ_PBLK] = {
        PORT0_RESETf, PORT1_RESETf, PORT2_RESETf, PORT3_RESETf,
        PORT4_RESETf, PORT5_RESETf, PORT6_RESETf, PORT7_RESETf,
        PORT8_RESETf, PORT9_RESETf, PORT10_RESETf, PORT11_RESETf,
        PORT12_RESETf, PORT13_RESETf, PORT14_RESETf, PORT15_RESETf
    };
    soc_reg_t egphy_reg[6] = {
        IDB_CA_PM0_Q_CA_CONTROL_0r, IDB_CA_PM1_Q_CA_CONTROL_0r,
        IDB_CA_PM2_Q_CA_CONTROL_0r, IDB_CA_PM3_Q_CA_CONTROL_0r,
        IDB_CA_PM4_Q_CA_CONTROL_0r, IDB_CA_PM5_Q_CA_CONTROL_0r
    };


    /* Handled in DV flexport code */
    if (!soc_feature(unit, soc_feature_port_leverage)) {
        si = &SOC_INFO(unit);

        /* CLPORT */
        SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
            port = SOC_BLOCK_PORT(unit, block_info_idx);
            phy_port_base = SOC_HR4_PORT_BLOCK_BASE_PORT(port);

            rval0 = 0;
            reg = IDB_CA_PM0_48_CA_CONTROL_0r;
            soc_hurricane4_port_ratio_get(unit, port, &mode);
            soc_reg_field_set(unit, reg, &rval0, PORT_MODEf,
                              soc_hr4_hw_mode_values[mode]);

            rval = rval0;

            for (lane = 0; lane < _HR4_PORTS_PER_PBLK; lane++) {
                if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                    soc_reg_field_set(unit, reg, &rval0,
                                      port_fields[lane], 1);
                }
            }
            soc_reg_field_set(unit, reg, &rval0, CA_RESETf, 1);

            LOG_DEBUG(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Setting Port %d reg %d mode %d[%d] "
                                  "value 0x%x, 0x%x\n"),
                       phy_port_base, reg, mode, soc_hr4_hw_mode_values[mode],
                       rval0, rval));

            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }

        /* XLPORT & PMQ */
        SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_XLPORT) {
            port = SOC_BLOCK_PORT(unit, block_info_idx);
            phy_port_base = SOC_HR4_PORT_BLOCK_BASE_PORT(port);

            if (phy_port_base < 61) {
                rval0 = 0;
                if (count == 0) {
                    reg = IDB_CA_PM6_Q_CA_CONTROL_0r;
                } else if (count == 1) {
                    reg = IDB_CA_PM7_Q_CA_CONTROL_0r;
                } else {
                    continue;
                }

                soc_hurricane4_port_ratio_get(unit, port, &mode);
                soc_reg_field_set(unit, reg, &rval0, PORT_MODEf,
                                  soc_hr4_hw_mode_values[mode]);

                rval = rval0;

                for (lane = 0; lane < _HR4_PORTS_PER_PMQ_PBLK; lane++) {
                    if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                        soc_reg_field_set(unit, reg, &rval0,
                                          port_fields[lane], 1);
                    }
                }
                soc_reg_field_set(unit, reg, &rval0, CA_RESETf, 1);

                LOG_DEBUG(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Setting Port %d reg %d mode %d[%d] "
                                      "value 0x%x, 0x%x\n"),
                           phy_port_base, reg, mode, soc_hr4_hw_mode_values[mode],
                           rval0, rval));

                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
                count++;
            } else {

                rval0 = 0;
                reg = IDB_CA_PM1_48_CA_CONTROL_0r;
                soc_hurricane4_port_ratio_get(unit, port, &mode);
                soc_reg_field_set(unit, reg, &rval0, PORT_MODEf,
                                  soc_hr4_hw_mode_values[mode]);

                rval = rval0;

                for (lane = 0; lane < _HR4_PORTS_PER_PBLK; lane++) {
                    if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                        soc_reg_field_set(unit, reg, &rval0,
                                          port_fields[lane], 1);
                    }
                }
                soc_reg_field_set(unit, reg, &rval0, CA_RESETf, 1);

                LOG_DEBUG(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Setting Port %d reg %d mode %d[%d] "
                                      "value 0x%x, 0x%x\n"),
                           phy_port_base, reg, mode, soc_hr4_hw_mode_values[mode],
                           rval0, rval));

                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
            }
        }

        /* GPHY */
        mode = 2;
        for (idx = 0; idx < 6 ; idx++) {

            reg = egphy_reg[idx];

            rval0= 0;
            soc_reg_field_set(unit, reg, &rval0, PORT_MODEf, mode);

            rval = rval0;

            for (lane = 0; lane < _HR4_PORTS_PER_PMQ_PBLK; lane++) {
                soc_reg_field_set(unit, reg, &rval0,
                                  port_fields[lane], 1);
            }
            soc_reg_field_set(unit, reg, &rval0, CA_RESETf, 1);

            LOG_DEBUG(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Setting QGPHY reg %d mode %d "
                                  "value 0x%x, 0x%x\n"),
                       reg, mode, rval0, rval));

            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }

        /* MACSEC */
        mode = 8;
        SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_MACSEC) {
            port = SOC_BLOCK_PORT(unit, block_info_idx);
            phy_port_base = SOC_HR4_PORT_BLOCK_BASE_PORT(port);

            rval0 = 0;
            reg = IDB_CA_PM2_48_CA_CONTROL_0r;
            soc_reg_field_set(unit, reg, &rval0, PORT_MODEf,
                              soc_hr4_hw_mode_values[mode]);

            rval = rval0;

            for (lane = 0; lane < _HR4_PORTS_PER_PBLK; lane++) {
                if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                    soc_reg_field_set(unit, reg, &rval0,
                                      port_fields[lane], 1);
                }
            }
            soc_reg_field_set(unit, reg, &rval0, CA_RESETf, 1);

            LOG_DEBUG(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Setting Port %d reg %d mode %d[%d] "
                                  "value 0x%x, 0x%x\n"),
                       phy_port_base, reg, mode, soc_hr4_hw_mode_values[mode],
                       rval0, rval));

            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }
    }

    /* Toggle cpu port cell assembly reset to send initial credit to EP */
    reg = IDB_CA_CPU_CONTROLr;
    rval0 = 0;
    soc_reg_field_set(unit, reg, &rval0, PORT_RESETf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));

    /* Toggle loopback cell assembly reset to send initial credit to EP */
    reg = IDB_CA_LPBK_CONTROLr;
    rval0 = 0;
    soc_reg_field_set(unit, reg, &rval0, PORT_RESETf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));

    /* Toggle FAE port cell assembly reset to send initial credit to EP */
    reg = IDB_CA_BSK_CONTROLr;
    rval0 = 0;
    soc_reg_field_set(unit, reg, &rval0, PORT_RESETf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));

    _soc_hr4_set_idb_dpp_ctrl(unit);

    return SOC_E_NONE;
}

soc_error_t
soc_hurricane4_idb_buf_reset(int unit, soc_port_t port, int reset)
{
    soc_reg_t reg;
    int port_block_base, phy_port, lane;
    int idx, clport;
    uint32 rval;
    soc_info_t *si = &SOC_INFO(unit);
#if 0
    int ca_empty = 0;
    static const soc_reg_t idb_ca_status_regs[] = {
        IDB_OBM0_CA_HW_STATUSr, IDB_OBM1_CA_HW_STATUSr,
        IDB_OBM2_CA_HW_STATUSr, IDB_OBM3_CA_HW_STATUSr,
        IDB_OBM4_CA_HW_STATUSr, IDB_OBM5_CA_HW_STATUSr,
        IDB_OBM6_CA_HW_STATUSr, IDB_OBM7_CA_HW_STATUSr,
        IDB_OBM8_CA_HW_STATUSr, IDB_OBM9_CA_HW_STATUSr,
        IDB_OBM10_CA_HW_STATUSr, IDB_OBM11_CA_HW_STATUSr,
        IDB_OBM12_CA_HW_STATUSr, IDB_OBM13_CA_HW_STATUSr,
        IDB_OBM14_CA_HW_STATUSr, IDB_OBM15_CA_HW_STATUSr
    };
    static const soc_field_t fifo_empty_port_fields[] = {
        FIFO_EMPTY_PORT0f, FIFO_EMPTY_PORT1f, FIFO_EMPTY_PORT2f,
        FIFO_EMPTY_PORT3f
    };
#endif
    static const soc_reg_t idb_ca_ctrl_regs[] = {
        IDB_CA_PM0_Q_CA_CONTROL_0r, IDB_CA_PM1_Q_CA_CONTROL_0r,
        IDB_CA_PM2_Q_CA_CONTROL_0r, IDB_CA_PM3_Q_CA_CONTROL_0r,
        IDB_CA_PM4_Q_CA_CONTROL_0r, IDB_CA_PM5_Q_CA_CONTROL_0r,
        IDB_CA_PM6_Q_CA_CONTROL_0r, IDB_CA_PM7_Q_CA_CONTROL_0r,
        IDB_CA_PM0_48_CA_CONTROL_0r, IDB_CA_PM1_48_CA_CONTROL_0r,
        IDB_CA_PM2_48_CA_CONTROL_0r
    };
    static const soc_field_t port_reset_fields[] = {
        PORT0_RESETf, PORT1_RESETf, PORT2_RESETf, PORT3_RESETf,
        PORT4_RESETf, PORT5_RESETf, PORT6_RESETf, PORT7_RESETf,
        PORT8_RESETf, PORT9_RESETf, PORT10_RESETf, PORT11_RESETf,
        PORT12_RESETf, PORT13_RESETf, PORT14_RESETf, PORT15_RESETf,
    };

    phy_port = si->port_l2p_mapping[port];
    port_block_base = PORT_BLOCK_BASE_PORT(port);
    lane = phy_port - port_block_base;
    clport = si->port_serdes[port];
    idx = clport & 0xF;
    /* GPHY's */
    if (phy_port <= 24) {
        idx = phy_port / 4;
        lane = phy_port % 4;
    } else if (phy_port <= 56) {
        lane = (phy_port - 25) % 16;
        /* PMQ's */
        if (phy_port >= 41) {
            idx = 7;
        } else {
            idx = 6;
        }
    } else if (phy_port <= 60) {
        /* Falcon */
        idx = 8;
    } else if (phy_port <= 64) {
        /* Merlin */
        idx = 9;
    } else {
        /* MACSEC */
        idx = 10;
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Reg Idx %d , port %d phy_port %d lane %d\n"),
               idx, port, phy_port, lane));
#if 0
    if (reset && !SAL_BOOT_SIMULATION) {
        soc_timeout_t to;

        reg = idb_ca_status_regs[idx];

        soc_timeout_init(&to, 250000, 0);
        /* Poll until IDB buffer is empty */
        for (;;) {
            if (0 == ca_empty) {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, REG_PORT_ANY, 0,
                                                &rval));
                ca_empty = soc_reg64_field32_get(unit, reg, rval,
                                                 fifo_empty_port_fields[lane]);
            }
            if (1 == ca_empty) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                    "IDBBufferDrainTimeOut:port %d,%s, timeout"
                                    "(ca_fifo_empty: %d)\n"),
                          unit, SOC_PORT_NAME(unit, port), ca_empty));
                return SOC_E_INTERNAL;
            }
        }
    }
#endif

    reg = idb_ca_ctrl_regs[idx];
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, port_reset_fields[lane], reset);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

extern int
(*_phy_tsce_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);
extern int
(*_phy_tscf_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);

#define TSC_REG_ADDR_TSCID_SET(_phy_reg, _phyad)    \
                            ((_phy_reg) |= ((_phyad) & 0x1f) << 19)

STATIC int
_soc_hurricane4_mdio_addr_to_port(uint32 phy_addr)
{
    int bus, offset, int_addr;

    /* Must be internal MDIO address */
    if ((phy_addr & 0x80) == 0) {
        return 0;
    }

    /*
     * Internal phy address:
     * bus 0 phy 1 to 24 are mapped to Physical port  1 to 24 GPHY
     * bus 1 phy 1 to 4  are mapped to Physical port 25 to 40 PMQ-0
     * bus 1 phy 4 to 8  are mapped to Physical port 41 to 56 PMQ-1
     * bus 1 phy 9 to 12 are mapped to Physical port 61 to 64 Merlin
     * bus 2 phy 1 to 4  are mapped to Physical port 57 to 60 Falcon
     * bus 3 phy 1 to 4  are mapped to Physical port IPROC PM
     */
    bus = PHY_ID_BUS_NUM(phy_addr);
    if (bus > 3) {
        return 0;
    }

    int_addr = phy_addr & 0x1f;

    switch (bus) {
    case 0: offset = 0;
            break;
    case 1:
            if (int_addr < 5) {
                offset = 24;
            } else if (int_addr < 9) {
                offset = 24 + 12;
            } else {
                offset = 61 - 9;
            }
            break;
    case 2: offset = 56;
            break;
    case 3: offset = 72;
            break;
    default:
            return 0;
    }

    return int_addr + offset;
}

STATIC int
_soc_hurricane4_tscx_reg_read(int unit, uint32 phy_addr,
                            uint32 phy_reg, uint32 *phy_data)
{
    int rv, blk, blk_i, blktype, port;
    int qmod, inst;
    uint64 rval64;
    int phy_port = _soc_hurricane4_mdio_addr_to_port(phy_addr);

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_hurricane4_tscx_reg_read[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);

    if(phy_port > 24 && phy_port < 57){
        inst = (phy_port - 25) / 16;
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst, 0, &rval64));
        qmod = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64, QMODEf);

        for (blk_i = 0; SOC_BLOCK_INFO(unit, blk_i).type >= 0; blk_i++) {
            blktype = SOC_BLOCK_INFO(unit, blk_i).type;
            if ((blktype == (qmod ? SOC_BLK_PMQPORT : SOC_BLK_XLPORT)) &&
                (SOC_BLOCK_INFO(unit, blk_i).number == inst)) {
                blk = blk_i;
                break;
            }
        }
    }

    rv = soc_sbus_tsc_reg_read(unit, port, blk, phy_addr,
                               phy_reg, phy_data);
    return rv;
}

STATIC int
_soc_hurricane4_tscx_reg_write(int unit, uint32 phy_addr,
                             uint32 phy_reg, uint32 phy_data)
{
    int rv, blk, blk_i, blktype, port;
    int phy_port = _soc_hurricane4_mdio_addr_to_port(phy_addr);
    int qmod, inst;
    uint64 rval64;

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_hurricane4_tscx_reg_write[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);

    if(phy_port > 24 && phy_port < 57){
        inst = (phy_port - 25) / 16;
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst, 0, &rval64));
        qmod = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64, QMODEf);

        for (blk_i = 0; SOC_BLOCK_INFO(unit, blk_i).type >= 0; blk_i++) {
            blktype = SOC_BLOCK_INFO(unit, blk_i).type;
            if ((blktype == (qmod ? SOC_BLK_PMQPORT : SOC_BLK_XLPORT)) &&
                (SOC_BLOCK_INFO(unit, blk_i).number == inst)) {
                blk = blk_i;
                break;
            }
        }
    }

    rv = soc_sbus_tsc_reg_write(unit, port, blk, phy_addr,
                                phy_reg, phy_data);
    return rv;
}

STATIC int
soc_hr4_egphy_top_init(int unit)
{
    uint32  pbmp, rval;
    int sleep_usec = 1100;

    /* 24 ports in 6 QGPHY */
    pbmp = 0xFFFFFF;

    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY0_IDDQ_GLOBAL_PWRf, 0);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY1_IDDQ_GLOBAL_PWRf, 0);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY2_IDDQ_GLOBAL_PWRf, 0);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY3_IDDQ_GLOBAL_PWRf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_3r, &rval, GPHY4_IDDQ_GLOBAL_PWRf, 0);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_3r, &rval, GPHY5_IDDQ_GLOBAL_PWRf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_3r(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, IDDQ_BIASf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
    sal_usleep(sleep_usec);

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_QGPHY_RST_Lf, 0x3F);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(sleep_usec);

    /* power down */
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_0r, &rval, EXT_PWRDOWNf, pbmp);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_0r(unit, rval));
    sal_usleep(sleep_usec);

    /* Toggle iddq isolation and reset */
    /* iddq_pw1 = 1,  iddq_bias = 1, reset_n = 1 */
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY0_IDDQ_GLOBAL_PWRf, 0xf);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY1_IDDQ_GLOBAL_PWRf, 0xf);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY2_IDDQ_GLOBAL_PWRf, 0xf);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY3_IDDQ_GLOBAL_PWRf, 0xf);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_3r, &rval, GPHY4_IDDQ_GLOBAL_PWRf, 0xf);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_3r, &rval, GPHY5_IDDQ_GLOBAL_PWRf, 0xf);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_3r(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, IDDQ_BIASf, 0x3f);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_QGPHY_RST_Lf, 0x3f);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(sleep_usec);

    /* iddq_pw1 = 0,  iddq_bias = 0, reset_n = 0 */
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY0_IDDQ_GLOBAL_PWRf, 0);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY1_IDDQ_GLOBAL_PWRf, 0);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY2_IDDQ_GLOBAL_PWRf, 0);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY3_IDDQ_GLOBAL_PWRf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_3r, &rval, GPHY4_IDDQ_GLOBAL_PWRf, 0);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_3r, &rval, GPHY5_IDDQ_GLOBAL_PWRf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_3r(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, IDDQ_BIASf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_QGPHY_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(sleep_usec);

    /* reset_n = 1, iddq_pw1 = 1,  iddq_bias = 1 */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_QGPHY_RST_Lf, 0x3f);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY0_IDDQ_GLOBAL_PWRf, 0xf);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY1_IDDQ_GLOBAL_PWRf, 0xf);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY2_IDDQ_GLOBAL_PWRf, 0xf);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY3_IDDQ_GLOBAL_PWRf, 0xf);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_3r, &rval, GPHY4_IDDQ_GLOBAL_PWRf, 0xf);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_3r, &rval, GPHY5_IDDQ_GLOBAL_PWRf, 0xf);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_3r(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, IDDQ_BIASf, 0x3f);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
    sal_usleep(sleep_usec);

    /* power up the QGPHY */
    /* iddq_pw1 = 0,  iddq_bias = 0 */
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY0_IDDQ_GLOBAL_PWRf, 0);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY1_IDDQ_GLOBAL_PWRf, 0);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY2_IDDQ_GLOBAL_PWRf, 0);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, GPHY3_IDDQ_GLOBAL_PWRf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_3r, &rval, GPHY4_IDDQ_GLOBAL_PWRf, 0);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_3r, &rval, GPHY5_IDDQ_GLOBAL_PWRf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_3r(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_2r, &rval, IDDQ_BIASf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_2r(unit, rval));
    sal_usleep(sleep_usec);
    SOC_IF_ERROR_RETURN(READ_TOP_QGPHY_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_0r, &rval, EXT_PWRDOWNf, 0);
    soc_reg_field_set(unit, TOP_QGPHY_CTRL_0r, &rval, CK25_DISABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_QGPHY_CTRL_0r(unit, rval));
    sal_usleep(sleep_usec);

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_hurricane4_gphyblock_init
 * Purpose:
 *      To init Gphy port block
 * Notes:
 *      Called from misc init
 */
STATIC int
_soc_hurricane4_gphyblock_init(int unit)
{
    int port;
    int index;
    uint32 rval;
    uint32  reg_addr;
    int block_info_idx;
    uint32 prev_reg_addr;

    rval = 0;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 1);
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, GPORT_ENf, 1);

    prev_reg_addr = 0xffffffff;
    PBMP_E_ITER(unit, port) {
        if (IS_XL_PORT(unit, port) || IS_CL_PORT(unit, port)) {
            continue;
        }

        if (IS_QSGMII_PORT(unit, port)) {
            continue;
        }

        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }

    prev_reg_addr = 0xffffffff;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);

    PBMP_E_ITER(unit, port) {
        if (IS_XL_PORT(unit, port) || IS_CL_PORT(unit, port)) {
            continue;
        }

        if (IS_QSGMII_PORT(unit, port)) {
            continue;
        }

        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_QTGPORT) {
        index = SOC_BLOCK_NUMBER(unit, block_info_idx);
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "QTGPORT: index %d \n"), index));
        SOC_IF_ERROR_RETURN(READ_QTG_CHIP_CONFIGr(unit, index, &rval));
        soc_reg_field_set(unit, QTG_CHIP_CONFIGr, &rval, IP_TDMf, 0);
        soc_reg_field_set(unit, QTG_CHIP_CONFIGr, &rval, IP_TDMf, 1);
        soc_reg_field_set(unit, QTG_CHIP_CONFIGr, &rval, PAUSE_PFC_SELf, 0);
        SOC_IF_ERROR_RETURN(WRITE_QTG_CHIP_CONFIGr(unit, index, rval));
    }


    return SOC_E_NONE;
}

STATIC int
_soc_hurricane4_ft_bank_config(int unit)
{
    soc_mem_t mem;
    int ft_banks;
    ft_key_hash_control_entry_t ft_key_ctrl;

    if (!soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        return SOC_E_NONE;
    }

    /* Banks allocated as per OTP bit */
    if (soc_feature(unit, soc_feature_flowtracker_3banks_ft_table)) {
        ft_banks = 0x7;
    } else if (soc_feature(unit, soc_feature_flowtracker_5banks_ft_table)) {
        ft_banks = 0x1f;
    } else {
        ft_banks = 0x1ff;
    }

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, FT_KEY_HASH_CONTROLm,
                                     MEM_BLOCK_ANY, 0, &ft_key_ctrl));

    /* Enable banks for flowtracking. */
    soc_mem_field32_set(unit, FT_KEY_HASH_CONTROLm, &ft_key_ctrl,
        HASH_TABLE_BANK_CONFIGf, ft_banks);

    soc_mem_field32_set(unit, FT_KEY_HASH_CONTROLm, &ft_key_ctrl,
        HASH_TABLE_TEST_MODEf, 0);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, FT_KEY_HASH_CONTROLm,
        MEM_BLOCK_ALL, 0, &ft_key_ctrl));

    mem = FT_KEY_SS0_SHARED_BANKS_CONTROLm;
    soc_mem_field32_set(unit, mem,
            (void *) &ft_key_ss0_shared_banks_control, B0_HASH_OFFSETf, 6);
    soc_mem_field32_set(unit, mem,
            (void *) &ft_key_ss0_shared_banks_control, B1_HASH_OFFSETf, 12);
    soc_mem_field32_set(unit, mem,
            (void *) &ft_key_ss0_shared_banks_control, B2_HASH_OFFSETf, 18);
    soc_mem_field32_set(unit, mem,
            (void *) &ft_key_ss0_shared_banks_control, B3_HASH_OFFSETf, 24);
    soc_mem_field32_set(unit, mem,
            (void *) &ft_key_ss0_shared_banks_control, B4_HASH_OFFSETf, 30);
    BCM_IF_ERROR_RETURN(WRITE_FT_KEY_SS0_SHARED_BANKS_CONTROLm(unit,
                MEM_BLOCK_ANY, 0, (void *) &ft_key_ss0_shared_banks_control));

    mem = FT_KEY_SS1_SHARED_BANKS_CONTROLm;
    soc_mem_field32_set(unit, mem,
            (void *) &ft_key_ss1_shared_banks_control, B0_HASH_OFFSETf, 6);
    soc_mem_field32_set(unit, mem,
            (void *) &ft_key_ss1_shared_banks_control, B1_HASH_OFFSETf, 12);
    soc_mem_field32_set(unit, mem,
            (void *) &ft_key_ss1_shared_banks_control, B2_HASH_OFFSETf, 18);
    soc_mem_field32_set(unit, mem,
            (void *) &ft_key_ss1_shared_banks_control, B3_HASH_OFFSETf, 24);
    BCM_IF_ERROR_RETURN(WRITE_FT_KEY_SS1_SHARED_BANKS_CONTROLm(unit,
                MEM_BLOCK_ANY, 0, (void *) &ft_key_ss1_shared_banks_control));

    return SOC_E_NONE;
}

STATIC int
_soc_hurricane4_init_hash_control_reset(int unit)
{

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

    SOC_IF_ERROR_RETURN
        (WRITE_ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                     &ing_vp_vlan_mbm_hash_control));

    soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm,
            &l2_entry_hash_control, HASH_TABLE_BANK_CONFIGf, 0x3);
    SOC_IF_ERROR_RETURN
        (WRITE_L2_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &l2_entry_hash_control));

    soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm,
          &l3_entry_hash_control, HASH_TABLE_BANK_CONFIGf, 0x3);
    SOC_IF_ERROR_RETURN
        (WRITE_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &l3_entry_hash_control));

    soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm,
          &mpls_entry_hash_control, HASH_TABLE_BANK_CONFIGf, 0x0);
    SOC_IF_ERROR_RETURN
        (WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &mpls_entry_hash_control));

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
        (WRITE_IP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                              &ip_uat_shared_banks_control));

    SOC_IF_ERROR_RETURN
        (WRITE_EP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                              &ep_uat_shared_banks_control));

    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        SOC_IF_ERROR_RETURN
            (WRITE_FT_KEY_SS0_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                          &ft_key_ss0_shared_banks_control));

        SOC_IF_ERROR_RETURN
            (WRITE_FT_KEY_SS1_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                          &ft_key_ss1_shared_banks_control));
    }

     return SOC_E_NONE;
}

int
soc_hurricane4_chip_reset(int unit)
{
#define _SOC_HR4_TEMP_MAX       130
#define _SOC_HR4_DEF_TEMP_MAX   110
    int index;
    uint32 to_usec, temp_mask;
    uint32 rval, temp_thr;
    int pm = 0, field = 0, fval = 0, reg = 0, pipe = 0;
    int frequency = -1, freq_sel = 0;
    uint64 rval64;
    soc_field_t fields[4];
    uint32 values[4];
    int clk_nx_factor = -1;
    int ndivint = -1;
    int ch0mdiv = -1;
    int qmode = 0;

    static const soc_reg_t temp_thr_reg[] = {
        TOP_PVTMON_0_INTR_THRESHOLDr, TOP_PVTMON_1_INTR_THRESHOLDr,
        TOP_PVTMON_2_INTR_THRESHOLDr, TOP_PVTMON_3_INTR_THRESHOLDr,
        TOP_PVTMON_4_INTR_THRESHOLDr, TOP_PVTMON_5_INTR_THRESHOLDr,
        TOP_PVTMON_6_INTR_THRESHOLDr
    };
    static const char *temp_thr_prop[] = {
        "temp0_threshold", "temp1_threshold", "temp2_threshold",
        "temp3_threshold", "temp4_threshold", "temp5_threshold",
        "temp6_threshold"
    };
    static soc_field_t egr_reset_fields[] = {
        PM0_RESETf, PM1_RESETf, PM2_RESETf, PM3_RESETf,
        PM4_RESETf, PM5_RESETf, PM6_RESETf, PM7_RESETf,
        PM8_RESETf, PM9_RESETf
    };

    soc_info_t *si = &SOC_INFO(unit);

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    soc_hurricane4_sbus_ring_map_config(unit);

    sal_usleep(to_usec);
    /* Reset IP, EP, MMU and port macros */
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, 0x0));


    /* If frequency is specificed use it , else use the si->frequency */
    if (frequency == -1) {
        frequency = si->frequency;
    }

    if (frequency > -1) {
        switch (frequency) {
        case 496:
        case 495:
            freq_sel = 0;
            ndivint = 99;
            ch0mdiv = 5;
            break;
        case 445:
            freq_sel = 1;
            ndivint = 89;
            ch0mdiv = 5;
            break;
        case 415:
            freq_sel = 2;
            ndivint = 83;
            ch0mdiv = 5;
            break;
        case 325:
            freq_sel = 3;
            ndivint = 91;
            ch0mdiv = 7;
            break;
        case 266:
        case 265:
            freq_sel = 4;
            ndivint = 106;
            ch0mdiv = 10;
            break;
        case 200:
            freq_sel = 5;
            ndivint = 96;
            ch0mdiv = 12;
            break;
        case 135:
            freq_sel = 6;
            ndivint = 108;
            ch0mdiv = 20;
            break;
        case 100:
            freq_sel = 7;
            ndivint = 100;
            ch0mdiv = 25;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Config Error - "
                         "Invalid frequency set: %d\n"), si->frequency));
            return SOC_E_PARAM;
        }

        fields[0] = CORE_CLK_0_FREQ_SELf;
        values[0] = freq_sel;
        fields[1] = SW_CORE_CLK_0_SEL_ENf;
        values[1] = 1;
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, TOP_CORE_CLK_FREQ_SELr,
                                     REG_PORT_ANY, 2, fields, values));
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Config Error - "
                     "Invalid frequency set: %d\n"), si->frequency));
            return SOC_E_PARAM;
    }

    /* Update PDIV and NDIV as per core clock freq */
    SOC_IF_ERROR_RETURN(READ_TOP_CORE_PLL0_CTRL_0r(unit,&rval));
    soc_reg_field_set(unit, TOP_CORE_PLL0_CTRL_0r, &rval, PDIVf, 1);
    soc_reg_field_set(unit, TOP_CORE_PLL0_CTRL_0r, &rval, NDIV_INTf, ndivint);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CORE_PLL0_CTRL_0r(unit, rval));

    /* Update CH0_MDIV as per core clock freq */
    SOC_IF_ERROR_RETURN(READ_TOP_CORE_PLL0_CTRL_8r(unit,&rval));
    soc_reg_field_set(unit, TOP_CORE_PLL0_CTRL_8r, &rval, CH0_MDIVf, ch0mdiv);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CORE_PLL0_CTRL_8r(unit, rval));

    /* Core clock to PM clock factor */
    rval = 0;
    clk_nx_factor = soc_property_get(unit,
                            spn_CORE_CLOCK_TO_PM_CLOCK_FACTOR, -1);
    if ((clk_nx_factor > 0) && (clk_nx_factor < 3) &&
            !(SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM)) {
        soc_reg_field_set(unit, TOP_MISC_GENERIC_CONTROLr, &rval,
                                CLK_TO_CLK_NX_FACTOR_SW_OVERRIDEf, 0x1);
        soc_reg_field_set(unit, TOP_MISC_GENERIC_CONTROLr, &rval,
                                CLK_TO_CLK_NX_FACTORf, clk_nx_factor);
    }
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, TOP_MISC_GENERIC_CONTROLr, REG_PORT_ANY, 0, rval));

    /* Bring PLLs out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                        TOP_TS_PLL_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                        TOP_XG_PLL0_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                        TOP_XG_PLL1_RST_Lf, 0);

    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                        TOP_TS_PLL_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                        TOP_XG_PLL0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                        TOP_XG_PLL1_RST_Lf, 1);

    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    /* Give time to lock */
    sal_usleep(to_usec);

    /* De-assert TS PLL, BS PLL0/1 post reset and bring AVS out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                       TOP_TS_PLL_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                       TOP_XG_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                       TOP_XG_PLL1_POST_RST_Lf, 1);

    soc_reg_field_set(unit,TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    /* Reset Nanosync and Broadsync. */
    if (!(SAL_BOOT_XGSSIM || SAL_BOOT_BCMSIM)) {
        SOC_IF_ERROR_RETURN(READ_IDM_GENRES_S0_IDM_RESET_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, IDM_GENRES_S0_IDM_RESET_CONTROLr, &rval, RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IDM_GENRES_S0_IDM_RESET_CONTROLr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval,
                          TOP_CORE_TO_IPROC_TS_RST_Lf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_8r(unit, &rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_8r, &rval,
                          CH0_HOLDf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_8r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval,
                          TOP_CORE_TO_IPROC_TS_RST_Lf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_8r(unit, &rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_8r, &rval,
                          CH0_HOLDf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_8r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_IDM_GENRES_S0_IDM_RESET_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, IDM_GENRES_S0_IDM_RESET_CONTROLr, &rval, RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_IDM_GENRES_S0_IDM_RESET_CONTROLr(unit, rval));
    }

    sal_usleep(to_usec);

    /* Enable high temperature interrupt monitoring.
     * Default on: pvtmon6 (close to core_plls at center of die).
     */
    temp_mask = soc_property_get(unit, "temp_monitor_select",
                                 ((1 << COUNTOF(temp_thr_reg)) - 1));
    /* The above is a 7 bit mask to indicate which temp sensor to hook up to
     * the interrupt.
     */
    rval = 0;
    for (index = 0; index < COUNTOF(temp_thr_reg); index++) {
        uint32 trval;

        /*
         * Temperature threshold triggers both HW reset and software INTR
         * for each sensor per HR4 hardware design. Reset and INTR can be
         * enabled none, either or both per sensor based on configuration.
         *
         * HR4 PVTMON overheating protection divides thermal sensors
         * into two groups. One group is configured with a lower threshold
         * and used solely for interrupt. The other group is configured with
         * with a higher threshold and used for hardware reset.
         *
         * At runtime, when HR4 is going through overheating, the group of sensors
         * with lower threshold will raise PVT interrupt, ISR will log
         * overheating information and flood console if overheating condition
         * persists. As the temperature keeps heating up, the hardware reset will
         * eventually kicks in when temperature is over reset threshold.
         *
         * Thus HR4 PVT monitoring uses hardware reset to protect the chip and
         * software interrupt to notify user.
         *
         * Sensor group for hardware reset:
         * 1, 2 and 3.
         *
         * Sensor group for interrupt:
         * 0, 4 and 5.
         *
         * AVS sensor (6) is used for voltage monitoring and should not be used for
         * any thermal protection feature (reset or interrupt), but its thermal data
         * can be used for temperature readout only.
         *
         * It hurts nothting to enable interrupt on all sensors.
         * when over-heating happens at higher threshold, both hw reset and interrupt
         * will be activated. HW reset will reset HR4, thus no interrupt will be
         * created anyway.
         */

        temp_thr = soc_property_get(unit, temp_thr_prop[index],
                                    ((index >= 1) && (index <= 3)) ?\
                                    _SOC_HR4_TEMP_MAX : _SOC_HR4_DEF_TEMP_MAX);

        if (temp_thr > _SOC_HR4_TEMP_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                          "Unsupported temp value %d !! Max %d. Using %d.\n"),
                          temp_thr, _SOC_HR4_TEMP_MAX, _SOC_HR4_DEF_TEMP_MAX));
            temp_thr = _SOC_HR4_DEF_TEMP_MAX;
        }

        /* Convert temperature to config data */
        /* Temp = 457.8289 - o_ADC_data * 0.55702
         * data = (457.8289 - temp)/0.55702 = (457828 - (temp*1000))/557
         * Note: Since this is a high temp value we can safely assume it to
         * always be a +ive number. This is in degrees celcius.
         */
        temp_thr = (457828 - (temp_thr * 1000)) / 557;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, temp_thr_reg[index], REG_PORT_ANY, 0, &trval));
        soc_reg_field_set(unit, temp_thr_reg[index], &trval, MIN_THRESHOLDf,
                          temp_thr);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, temp_thr_reg[index], REG_PORT_ANY, 0, trval));
        if ((temp_mask & (1 << index)) && (index != 6)) {
            /* AVS sensor 6 should not be configured. */
            rval |= (1 << ((index * 2) + 1)); /* 2 bits per pvtmon, using min */
        }
    }
    /* Skip AVS sensor */
    _soc_hr4_temp_mon_mask[unit] = (temp_mask & (~(1 << 6)));

    /* Set PVT interrupt mask map */
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_INTR_MASKr(unit, rval));

    /* Clear interrupt latch bits */
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_INTR_STATUSr(unit, 0xffffffff));

    /*
     * Enable HW PVTMON high temperature protection by setting
     * hightemp_ctrl_en field for pvtmon 1-3
     */
    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, HIGHTEMP_RESET_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON1_HIGHTEMP_CTRL_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON2_HIGHTEMP_CTRL_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON3_HIGHTEMP_CTRL_ENf, 1);
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

    /* Bring IP, EP, MMU and port macros out of reset */
    /* Reset GPHY, MACSEC */
    rval = 0x3fff;
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_QGPHY_REG_RST_Lf, 0x1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MACSEC_RST_Lf, 0x1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_CORE_TO_IPROC_TS_RST_Lf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    SOC_IF_ERROR_RETURN(soc_hr4_egphy_top_init(unit));
    sal_usleep(to_usec);

    /* PM4x10Q QSGMII mode control
     */
    reg = TOP_MISC_GENERIC_CONTROLr;
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_GENERIC_CONTROLr(unit, &rval));
    qmode = soc_property_phys_port_get(unit, 25, spn_PORT_GMII_MODE, 0);
    if (qmode) {
        if (soc_feature(unit, soc_feature_untethered_otp)) {
            if (soc_feature(unit, soc_feature_PM4x10QInst0QsgmiiDisable)) {
                LOG_CLI((BSL_META_U(unit,
                                    "QSGMII mode not supported on PM4x10Q#0\n")));
                return SOC_E_FAIL;
            }
            if ((qmode == 2) &&
                soc_feature(unit, soc_feature_PM4x10QInst0UsxgmiiDisable)) {
                LOG_CLI((BSL_META_U(unit,
                                    "USXGMII mode not supported on PM4x10Q#0\n")));
                return SOC_E_FAIL;
            }
        }
        soc_reg_field_set(unit, reg, &rval, PM4X10Q_0_QSGMII_MODE_ENf, 0x1);
        soc_reg_field32_modify(unit, CHIP_CONFIGr, 0, QMODEf, 1);
    }
    qmode = soc_property_phys_port_get(unit, 41, spn_PORT_GMII_MODE, 0);
    if (qmode) {
        if (soc_feature(unit, soc_feature_untethered_otp)) {
            if (soc_feature(unit, soc_feature_PM4x10QInst1QsgmiiDisable)) {
                LOG_CLI((BSL_META_U(unit,
                                    "QSGMII mode not supported on PM4x10Q#1\n")));
                return SOC_E_FAIL;
            }
            if ((qmode == 2) &&
                soc_feature(unit, soc_feature_PM4x10QInst1UsxgmiiDisable)) {
                LOG_CLI((BSL_META_U(unit,
                                    "USXGMII mode not supported on PM4x10Q#1\n")));
                return SOC_E_FAIL;
            }
        }
        soc_reg_field_set(unit, reg, &rval, PM4X10Q_1_QSGMII_MODE_ENf, 0x1);
        soc_reg_field32_modify(unit, CHIP_CONFIGr, 1, QMODEf, 1);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    /* Reset egress hardware resource */
    reg = SOC_REG_UNIQUE_ACC(unit, EGR_PORT_BUFFER_SFT_RESET_0r)[pipe];

    SOC_IF_ERROR_RETURN
        (soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval64));
    for ( pm = 0; pm < sizeof(egr_reset_fields)/sizeof(soc_field_t); pm++) {
        field = egr_reset_fields[pm];
        fval = 4;
        /* MACSEC should always have 0 */
        if (field == PM7_RESETf) {
            fval = 0;
        }
        soc_reg64_field32_set(unit, reg, &rval64, field, fval);
    }
    /* Write the value to enable 4 lanes on the PM */
    SOC_IF_ERROR_RETURN(soc_reg64_set(unit, reg, REG_PORT_ANY, 0, rval64));

    /* Set it back to zero now */
    for ( pm = 0; pm < sizeof(egr_reset_fields)/sizeof(soc_field_t); pm++) {
        field = egr_reset_fields[pm];
        fval = 0;
        soc_reg64_field32_set(unit, reg, &rval64, field, fval);
    }
    SOC_IF_ERROR_RETURN(soc_reg64_set(unit, reg, REG_PORT_ANY, 0, rval64));

    if (soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, IDB_SER_CONTROL_64r,
                                                   REG_PORT_ANY,
                                                   IS_TDM_ECC_ENf,1));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, IDB_SER_CONTROL_64r,
                                                   REG_PORT_ANY,
                                                   CA_CPU_ECC_ENABLEf,1));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, IDB_SER_CONTROL_64r,
                                                   REG_PORT_ANY,
                                                   CA_LPBK_ECC_ENABLEf,1));
    }

    SOC_IF_ERROR_RETURN(soc_trident3_init_idb_memory(unit));
    SOC_IF_ERROR_RETURN(_soc_hurricane4_init_hash_control_reset(unit));
    SOC_IF_ERROR_RETURN(soc_hurricane4_uft_uat_config(unit));
    SOC_IF_ERROR_RETURN(_soc_hurricane4_ft_bank_config(unit));

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

#ifdef BCM_CMICX_SUPPORT
STATIC int
_soc_hr4_ledup_init(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int freq;
    uint32 rval, clk_half_period, refresh_period;

    /* WAR: Let LED accumulation work. */
    SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_MISC_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IPROC_WRAP_MISC_CONTROLr, &rval,
                      IPROC_PM_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_MISC_CONTROLr(unit, rval));

    /* freq in KHz */
    freq = (si->frequency) * 1000;

    /* For LED refresh period = 33 ms (about 30Hz)  */

    /* refresh period
     * = (required refresh period in sec)*(switch clock frequency in Hz)
     */
    refresh_period = (freq * 33);

    rval = 0;
    soc_reg_field_set(unit, U0_LED_REFRESH_CTRLr, &rval,
                      REFRESH_CYCLE_PERIODf, refresh_period);

    SOC_IF_ERROR_RETURN(WRITE_U0_LED_REFRESH_CTRLr(unit, rval));

    /* For LED clock period */
    /* LEDCLK_HALF_PERIOD
     *  = [(required LED clock period in sec)/2]*(M0SS clock frequency in Hz)
     *
     *  Where M0SS freqency is 858MHz and
     *  Typical LED clock period is 200ns(5MHz) = 2*10^-7
     */
    freq = 850 * 1000000;
    clk_half_period = (freq + 2500000) / 5000000;
    clk_half_period = clk_half_period / 2;

    rval = 0;
    soc_reg_field_set(unit, U0_LED_CLK_DIV_CTRLr, &rval,
                      LEDCLK_HALF_PERIODf, clk_half_period);

    SOC_IF_ERROR_RETURN(WRITE_U0_LED_CLK_DIV_CTRLr(unit, rval));

    /* Setup LED last port. */
    SOC_IF_ERROR_RETURN(READ_U0_LED_ACCU_CTRLr(unit, &rval));
    soc_reg_field_set(unit, U0_LED_ACCU_CTRLr, &rval, LAST_PORTf, 11);
    SOC_IF_ERROR_RETURN(WRITE_U0_LED_ACCU_CTRLr(unit, rval));

    /* To initialize M0s for LED or Firmware Link Scan*/
    if (soc_feature(unit, soc_feature_cmicx) && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
        SOC_IF_ERROR_RETURN(soc_iproc_m0_init(unit));
    }
    return SOC_E_NONE;
}
#endif /* BCM_CMICX_SUPPORT */

int
_soc_hurricane4_misc_init(int unit)
{
    soc_control_t *soc;
    soc_info_t *si;
    soc_mem_t mem;
    uint64 rval64;
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    soc_pbmp_t pbmp;
    int port;
    int parity_enable;
    modport_map_subport_entry_t map_entry;

    int block_info_idx;
    int clport, lane, mode, xlport;
    int phy_port_base;
    static soc_field_t port_fields[_HR4_PORTS_PER_PBLK] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };
    static const int mode_encodings[SOC_HR4_PORT_RATIO_COUNT] = {
        4, 3, 3, 3, 2, 2, 1, 1, 0
    };
    uint32 mc_ctrl = 0;
    soc_bond_info_t *bond_info = SOC_BOND_INFO(unit);

    soc = SOC_CONTROL(unit);
    si = &SOC_INFO(unit);

    /* Stop the mem scan task before all of the parity init takes place */
    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));
    SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_stop(unit));

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    if (parity_enable) {
        if (!SOC_WARM_BOOT(unit)) {
            /* Certain mems/regs need to be cleared before enabling SER */
            SOC_IF_ERROR_RETURN(soc_trident3_clear_mmu_memory(unit));
        }
        soc_ser_log_init(unit, NULL, 0);

        SOC_IF_ERROR_RETURN(soc_hr4_ser_enable_all(unit, TRUE));
        soc_hr4_ser_register(unit);

        /*Initialize chip-specific functions for SER testing*/
        soc_hr4_ser_test_register(unit);
    }

    SOC_IF_ERROR_RETURN(_soc_trident3_init_mmu_memory(unit));
        SOC_IF_ERROR_RETURN(READ_MMU_GCFG_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, PARITY_ENf, parity_enable ? 1 : 0);
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, REFRESH_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(
            _soc_cancun_load_status_clear(unit, CANCUN_SOC_FILE_TYPE_CIH));
        /* Dummy call to clear garbage h/w values */
        SOC_IF_ERROR_RETURN
            (soc_hurricane4_temperature_monitor_get(unit,
                                                  COUNTOF(hr4_pvtmon_result_reg),
                                                  NULL,
                                                  NULL));
    }

    SOC_IF_ERROR_RETURN(soc_hurricane4_uft_uat_config(unit));
    SOC_IF_ERROR_RETURN(_soc_hurricane4_ft_bank_config(unit));

    soc_cancun_init(unit);
    _soc_hurricane4_latency_config(unit);
    SOC_IF_ERROR_RETURN(_soc_hurricane4_port_mapping_init(unit));

#ifdef INCLUDE_XFLOW_MACSEC
    if (soc_feature(unit, soc_feature_xflow_macsec)) {
        SOC_IF_ERROR_RETURN(_soc_xflow_macsec_init(unit));
    }
#endif

    if (!SAL_BOOT_XGSSIM) {
        int rv;
        soc_port_schedule_state_t *port_schedule_state;
        if (soc->tdm_info == NULL) {
            soc->tdm_info = sal_alloc(sizeof(_soc_hurricane4_tdm_temp_t),
                                      "Hurricane4 TDM info");
            if (soc->tdm_info == NULL) {
                return SOC_E_MEMORY;
            }
        }
        port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                        "Hurricane4 soc_port_schedule_state_t");
        if (port_schedule_state == NULL) {
            return SOC_E_MEMORY;
        }

        rv = _soc_hurricane4_tdm_init(unit, port_schedule_state);
        sal_free(port_schedule_state);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
    }
    SOC_IF_ERROR_RETURN(_soc_hurricane4_idb_init(unit));
    SOC_IF_ERROR_RETURN(_soc_hurricane4_edb_init(unit));

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

    /* Handled in DV flexport code */
    if (!soc_feature(unit, soc_feature_port_leverage)) {
        SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
            port = SOC_BLOCK_PORT(unit, block_info_idx);
            clport = SOC_BLOCK_NUMBER(unit, block_info_idx);
            phy_port_base = SOC_HR4_PORT_BLOCK_BASE_PORT(port);

            /* Assert CLPORT soft reset */
            rval = 0;
            for (lane = 0; lane < _HR4_PORTS_PER_PBLK; lane++) {
                if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &rval,
                                      port_fields[lane], 1);
                }
            }
            SOC_IF_ERROR_RETURN(WRITE_CLPORT_SOFT_RESETr(unit, port, rval));

            /* Set port mode */
            soc_hurricane4_port_ratio_get(unit, clport, &mode);
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
            for (lane = 0; lane < _HR4_PORTS_PER_PBLK; lane++) {
                if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                    soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &rval,
                                      port_fields[lane], 1);
                }
            }
            SOC_IF_ERROR_RETURN(WRITE_CLPORT_ENABLE_REGr(unit, port, rval));
        }

        SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_XLPORT) {
            port = SOC_BLOCK_PORT(unit, block_info_idx);
            xlport = SOC_BLOCK_NUMBER(unit, block_info_idx);
            phy_port_base = SOC_HR4_PORT_BLOCK_BASE_PORT(port);

            if (phy_port_base == 25 || phy_port_base == 41) {
                /* PMQ in QSGMII mode */
                if (soc_property_phys_port_get(unit, phy_port_base,
                                                   spn_PORT_GMII_MODE, 0)) {
                    continue;
                }
            }

            /* Assert XLPORT soft reset */
            rval = 0;
            for (lane = 0; lane < _HR4_PORTS_PER_PBLK; lane++) {
                if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                    soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &rval,
                                      port_fields[lane], 1);
                }
            }
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, rval));

            /* Set port mode */
            soc_hurricane4_port_ratio_get(unit, xlport, &mode);
            rval = 0;
            soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, XPORT0_CORE_PORT_MODEf,
                              mode_encodings[mode]);
            soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, XPORT0_PHY_PORT_MODEf,
                              mode_encodings[mode]);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));

            /* De-assert XLPORT soft reset */
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, 0));

            /* Enable XLPORT */
            rval = 0;
            for (lane = 0; lane < _HR4_PORTS_PER_PBLK; lane++) {
                if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                    soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval,
                                      port_fields[lane], 1);
                }
            }
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_ENABLE_REGr(unit, port, rval));
        }
    }

    /* Enable dual hash tables */
    SOC_IF_ERROR_RETURN(soc_trident3_hash_init(unit));

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
    SOC_PBMP_REMOVE(pbmp, PBMP_FAE_ALL(unit));
    SOC_PBMP_REMOVE(pbmp, PBMP_MACSEC_ALL(unit));
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

    /*FIXME:
    if (soc_mspi_init(unit) != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "unit %d : MSPI Init Failed\n"), unit));
    }*/

    /* Setup MDIO divider */
    soc_trident3_mdio_rate_divisor_set(unit);

    _phy_tsce_firmware_set_helper[unit] = soc_trident3_tscx_firmware_set;
    _phy_tscf_firmware_set_helper[unit] = soc_trident3_tscx_firmware_set;

    /* Check if Hierarchical ECMP mode is set */
    if (soc_property_get(unit, spn_L3_ECMP_LEVELS, 1) == 2) {
        uint32 rval = 0;
        uint32 ecmp_mode = 1;
        soc_reg_field_set(unit, ECMP_CONFIGr, &rval, ECMP_MODEf, ecmp_mode);
        SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIGr(unit, rval));
    } else {
        uint32 rval = 0;
        uint32 ecmp_mode = 0;
        soc_reg_field_set(unit, ECMP_CONFIGr, &rval, ECMP_MODEf, ecmp_mode);
        SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIGr(unit, rval));

    }

    SOC_IF_ERROR_RETURN
        (soc_counter_tomahawk_status_enable(unit, TRUE));

    /* Gphy port block init */
    SOC_IF_ERROR_RETURN(_soc_hurricane4_gphyblock_init(unit));

#ifdef BCM_UTT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(soc_utt_ifp_bank_config(unit));
        if (soc_feature(unit, soc_feature_utt)) {
            SOC_IF_ERROR_RETURN
                (soc_utt_lpm_bank_config(unit,
                                     SOC_URPF_STATUS_GET(unit) ? 1 : 0,
                                     SOC_L3_DEFIP_UTT_PAIRED_TCAMS_GET(unit)));
        }
    }
#endif

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) &&
        !(SAL_BOOT_SIMULATION))
    {
        SOC_IF_ERROR_RETURN(_soc_hr4_ledup_init(unit));
    }
#endif /* BCM_CMICX_SUPPORT */

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MISC Init completed (u=%d)\n"), unit));

   /* Populate and enable RTAG7 Macro flow offset table */
   SOC_IF_ERROR_RETURN(_soc_trident3_macro_flow_offset_init(unit));

    /* Mem Scan thread start should be the last step in Misc init */
    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));
        SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_start(unit));
    }

    if (soc_feature(unit, soc_feature_untethered_otp) &&
        soc_feature(unit, soc_feature_chip_icid_info)) {
        SOC_IF_ERROR_RETURN(_soc_reg32_get(unit, 11, 0, 0x2025c00,
                                           &bond_info->ic_id[0]));
        SOC_IF_ERROR_RETURN(_soc_reg32_get(unit, 11, 0, 0x2021000,
                                           &bond_info->ic_id[1]));
    }

    return SOC_E_NONE;
}


/*
 * Function used for index calculation of certain memories,
 * where index is derived from more than one key.
 * Eg. (Port, SP) or (Port, PG)
 * Defined as Macro: SOC_TD3_MMU_PIPED_MEM_INDEX
 * Note: If no special indexig required, return the same index.
 */
uint32 _soc_hr4_piped_mem_index(int unit, soc_port_t port,
                               soc_mem_t mem, int arr_off)
{
    int     mmu_port, index;

    mmu_port = SOC_TD3_MMU_PORT(unit, port);

    switch (mem) {
        case MMU_THDM_DB_PORTSP_CONFIGm:
        case MMU_THDM_MCQE_PORTSP_CONFIGm:
            index = (_HR4_PORTS_PER_PIPE * arr_off) + (mmu_port &
                     SOC_HR4_MMU_PORT_STRIDE);
            break;
        case THDI_PORT_SP_CONFIGm:
        case MMU_THDU_CONFIG_PORTm:
        case MMU_THDU_RESUME_PORTm:
            index = ((mmu_port & SOC_HR4_MMU_PORT_STRIDE) * _HR4_MMU_NUM_POOL)
                     + arr_off;
            break;
        case THDI_PORT_PG_CONFIGm:
            index = ((mmu_port & SOC_HR4_MMU_PORT_STRIDE) * _HR4_MMU_NUM_PG)
                     + arr_off;
            break;
        default:
            return arr_off;
    }

    return index;
}

/*
 * Function used for global resource reservation (MCQE / RQE / Buf cell).
 * Input state: Total hardware resource.
 * Output state: Resource available for admission control.
 */
STATIC void
_soc_hr4_mmu_config_dev_reserve(int unit, _soc_mmu_device_info_t *devcfg,
                                int lossless)
{
    int port, pm, asf_rsvd = 0;
    uint32 total_mcq = 0, num_ports = 0;
    uint32 cpu_cnt = 1, lb_cnt = 1, mgmt_cnt = 0;
    soc_info_t *si = &SOC_INFO(unit);

    /* Device reservation for RQE Entry */
    devcfg->total_rqe_queue_entry -= _HR4_MMU_RQE_ENTRY_RSVD_PER_XPE;

    /* Device reservation for MCQ Entry - 6 entries for each MC queue */
    if (si->flex_eligible) {
        for (pm = 0; pm < _HR4_PBLKS_PER_DEV(unit); pm++) {
            num_ports += soc_hr4_ports_per_pm_get(unit, pm);
        }
        total_mcq += (num_ports * SOC_HR4_NUM_MCAST_QUEUE_PER_PORT);
    } else {
        PBMP_ALL_ITER(unit, port) {
            if (IS_CPU_PORT(unit, port) ||
                IS_LB_PORT(unit,port) ||
                IS_MGMT_PORT(unit,port) ||
                _HR4_IS_NON_FP_PORT(port)) {
                continue;
            }
            total_mcq += si->port_num_cosq[port];
        }
    }
    
    total_mcq += ((cpu_cnt * SOC_HR4_NUM_CPU_QUEUES) +
                  (lb_cnt + mgmt_cnt) * SOC_HR4_NUM_MCAST_QUEUE_PER_LB_PORT +
                  SOC_HR4_NUM_MACSEC_MC_QUEUE);

    devcfg->total_mcq_entry -= ((total_mcq * _HR4_MCQE_RSVD_PER_MCQ) +
                                _HR4_TDM_MCQE_RSVD_OVERSHOOT);

    devcfg->mmu_total_cell -= asf_rsvd;
}

STATIC void
_soc_hr4_mmu_init_dev_config(int unit, _soc_mmu_device_info_t *devcfg,
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

    devcfg->max_pkt_byte = _HR4_MMU_MAX_PACKET_BYTES;
    devcfg->mmu_hdr_byte = _HR4_MMU_PACKET_HEADER_BYTES;
    devcfg->jumbo_pkt_size = _HR4_MMU_JUMBO_FRAME_BYTES;
    devcfg->default_mtu_size = _HR4_MMU_DEFAULT_MTU_BYTES;
    devcfg->mmu_cell_size = _HR4_MMU_BYTES_PER_CELL;
    devcfg->mmu_total_cell = _HR4_MMU_TOTAL_CELLS_PER_XPE;
    devcfg->num_pg = _HR4_MMU_NUM_PG;
    devcfg->num_service_pool = _HR4_MMU_NUM_POOL;
    devcfg->flags = SOC_MMU_CFG_F_PORT_MIN | SOC_MMU_CFG_F_PORT_POOL_MIN |
                    SOC_MMU_CFG_F_RQE | SOC_MMU_CFG_F_EGR_MCQ_ENTRY;
    devcfg->total_mcq_entry = _HR4_MMU_TOTAL_MCQ_ENTRY(unit);
    devcfg->rqe_queue_num = 11;
    devcfg->total_rqe_queue_entry = _HR4_MMU_TOTAL_RQE_ENTRY(unit);

    _soc_hr4_mmu_config_dev_reserve(unit, devcfg, lossless);
}

STATIC void
_soc_hr4_mmu_sw_info_config (int unit, _soc_hr4_mmu_sw_config_info_t * swcfg,
                             int lossless)
{
    /* Default settings is lossless */
    swcfg->mmu_egr_queue_min = 0;
    swcfg->mmu_egr_qgrp_min = 0;
    swcfg->mmu_total_pri_groups = 8;
    swcfg->mmu_active_pri_groups = 1;
    swcfg->mmu_pg_min = 7;
    swcfg->mmu_port_min = 0;
    swcfg->mmu_mc_egr_qentry_min = 4;
    swcfg->mmu_rqe_qentry_min = 8;
    swcfg->mmu_rqe_queue_min = 7;
    if (lossless == 0)
    {
        swcfg->mmu_egr_queue_min = 7;
        swcfg->mmu_egr_qgrp_min = 14;
        swcfg->mmu_pg_min = 0;
    }
}

STATIC int
_soc_hr4_default_pg_headroom(int unit, soc_port_t port,
                            int lossless)
{
    int hdrm = 0;
    int speed = 1000;

    if (IS_CPU_PORT(unit, port)) {
        return 69;
    } else if (!lossless) {
        return 0;
    } else if (IS_MACSEC_PORT(unit, port)) {
        return 117;
    } else if (IS_LB_PORT(unit, port)) {
        return 160;
    }

    if (_HR4_IS_NON_FP_PORT(port)) {
        return 0;
    }

    speed = SOC_INFO(unit).port_speed_max[port];

    if (speed <= 1000) {
        hdrm = 120;
    } else if (speed <= 2500) {
        hdrm = 122;
    } else if (speed <= 5000) {
        hdrm = 153;
    }else if (speed <= 11000) {
        hdrm = 162;
    } else if (speed <= 21000) {
        hdrm = 199;
    } else if (speed <= 27000) {
        hdrm = 256;
    } else if (speed <= 42000) {
        hdrm = 273;
    } else if (speed <= 53000) {
        hdrm = 350;
    } else if (speed >= 100000) {
        hdrm = 568;

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
_soc_hr4_mmu_config_buf_phase1(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg,
                               _soc_hr4_mmu_sw_config_info_t *swcfg,
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
    int rqe_entry_shared_total, qmin;
    int mcq_entry_shared_total;

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

    total_pool_size = devcfg->mmu_total_cell + asf_rsvd; /* Add back ASF reserved. */

    buf->headroom = max_packet_cells;   /* 1 packet per pipe. */

    mcq_entry_shared_total = devcfg->total_mcq_entry;

    rqe_entry_shared_total = devcfg->total_rqe_queue_entry -
        (_HR4_MMU_NUM_RQE_QUEUES * swcfg->mmu_rqe_qentry_min);

    for (idx = 0; idx < _HR4_MMU_NUM_POOL; idx++) {
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

    for (idx = 0; idx < SOC_HR4_MMU_CFG_QGROUP_MAX; idx++) {
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
        for (idx = 0; idx < _HR4_MMU_NUM_INT_PRI; idx++) {
            buf_port->pri_to_prigroup[idx] = 7;
        }

        /* priority group to pool mapping */
        for (idx = 0; idx < _HR4_MMU_NUM_PG; idx++) {
            buf_port->prigroups[idx].pool_idx = 0;
        }

        for (idx = 0; idx < _HR4_MMU_NUM_POOL; idx++) {
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
        for (idx = 0; idx < _HR4_MMU_NUM_PG; idx++) {
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
                    _soc_hr4_default_pg_headroom(unit, port, lossless);
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
            buf_queue->qgroup_min_enable = 1;
            buf_queue->discard_enable = 1;
            buf_queue->qgroup_id = 0;
            if (lossless) {
                buf_queue->discard_enable = 0;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;
            } else {
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;
            }
        }

        /* queue to pool mapping */
        for (idx = 0;
             idx < si->port_num_cosq[port] + si->port_num_uc_cosq[port]; idx++) {
            buf_port->queues[idx].pool_idx = 0;
        }
    }

    /* RQE */
    for (idx = 0; idx < _HR4_MMU_NUM_RQE_QUEUES; idx++) {
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
_soc_hr4_mmu_config_buf_phase2(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg,
                               _soc_hr4_mmu_sw_config_info_t *swcfg,
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
    int pg_hdrm_ob[] = {690, 372, 240}; /* PG headroom of 1/2/4-port PortMacro */
    int total_pool_size = 0;
    int ing_rsvd_total = 0, egr_rsvd_total = 0;
    int ing_shared_total, egr_shared_total;
    int total_rsvd_qmin = 0;
    uint32 color_limit = 0;
    uint32 num_ports = 0, mgmt_cnt = 0, cpu_cnt = 0, lb_cnt = 0;
    uint32 cpu_hdrm = 0, lb_hdrm = 0, mgmt_hdrm = 0;
    uint32 macsec_cnt = 0, macsec_hdrm = 0;

    si = &SOC_INFO(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Initializing default MMU config phase 2(u=%d)\n"), unit));

    buf_pool = &buf->pools[0];
    total_pool_size = devcfg->mmu_total_cell; /* per XPE limit */
    total_rsvd_qmin = swcfg->mmu_egr_queue_min * (SOC_HR4_NUM_CPU_QUEUES +
                      SOC_HR4_NUM_MACSEC_UC_QUEUE + SOC_HR4_NUM_MACSEC_MC_QUEUE +
                     _HR4_MMU_NUM_LBK_QUEUES );

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            cpu_cnt++;
            cpu_hdrm = _soc_hr4_default_pg_headroom(unit, port, lossless);
        } else if (IS_LB_PORT(unit,port)) {
            lb_cnt++;
            lb_hdrm = _soc_hr4_default_pg_headroom(unit, port, lossless);
        } else if (IS_MACSEC_PORT(unit,port)) {
            macsec_cnt++;
            macsec_hdrm = _soc_hr4_default_pg_headroom(unit, port, lossless);
        } else {
            if (!_HR4_IS_NON_FP_PORT(port)) {
                num_ports++;
            }
        }
    }
    if (si->flex_eligible) {
        num_ports = 0;
        for (pm = 0; pm < _HR4_PBLKS_PER_DEV(unit); pm++) {
            port = soc_hr4_ports_per_pm_get(unit, pm);
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
                4*ports[2]*pg_hdrm_ob[2] + cpu_cnt*cpu_hdrm + lb_cnt*lb_hdrm +
                macsec_cnt*macsec_hdrm) +
                swcfg->mmu_pg_min*swcfg->mmu_active_pri_groups*(
                num_ports + cpu_cnt + lb_cnt + mgmt_cnt + macsec_cnt) +
                swcfg->mmu_port_min*swcfg->mmu_active_pri_groups*(
                num_ports + cpu_cnt + lb_cnt + mgmt_cnt + macsec_cnt));
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
            buf_pool->prigroup_headroom += _HR4_PG_HEADROOM_FOR_MACSEC_PORTS;
        }
        ing_rsvd_total += (buf->headroom + buf_pool->prigroup_headroom +
                           buf_pool->prigroup_guarantee);
    }

    egr_rsvd_total += total_rsvd_qmin +
                       ((num_ports + mgmt_cnt) * swcfg->mmu_egr_qgrp_min) +
                       (swcfg->mmu_rqe_queue_min * _HR4_MMU_NUM_RQE_QUEUES);

    if (lossless) {
        ing_rsvd_total += egr_rsvd_total;
    }
    
    buf_pool->total_mcq_entry = _HR4_MMU_TOTAL_MCQ_ENTRY(unit) -
                                 (_HR4_MAX_MMU_MCQE_PER_XPE * _HR4_MCQE_RSVD_PER_MCQ) -
                                 _HR4_TDM_MCQE_RSVD_OVERSHOOT;

    buf_pool->mcq_entry_reserved = _HR4_MAX_MMU_MCQE_PER_XPE * 4;


    ing_shared_total = total_pool_size - ing_rsvd_total;
    egr_shared_total = total_pool_size - egr_rsvd_total;

    swcfg->ing_shared_total = ing_shared_total;
    swcfg->egr_shared_total = egr_shared_total;

    LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "MMU config: Total Shared size: ingress %d egress %d\n"),
                            ing_shared_total, egr_shared_total));

    color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;

    for (idx = 0; idx < SOC_HR4_MMU_CFG_QGROUP_MAX; idx++) {
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
        for (idx = 0; idx < _HR4_MMU_NUM_PG; idx++) {
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
                buf_queue->yellow_limit = color_limit;
                buf_queue->red_limit = color_limit;
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
                buf_queue->red_limit = color_limit;
                buf_queue->yellow_limit = color_limit;
            }
        }
    }

    /* RQE */
    for (idx = 0; idx < _HR4_MMU_NUM_RQE_QUEUES; idx++) {
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
_soc_hr4_pool_scale_to_limit(int size, int scale)
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
_soc_hr4_mmu_config_buf_set_hw_port(int unit, int port, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless,
                               _soc_hr4_mmu_sw_config_info_t *swcfg)
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
    int base, numq, enable;
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
    for (idx = 0; idx < _HR4_MMU_NUM_PG; idx++) {
        soc_reg_field_set(unit, THDI_PORT_PG_SPIDr, &rval,
                          prigroup_spid_field[idx],
                          buf_port->prigroups[idx].pool_idx);
    }
    SOC_IF_ERROR_RETURN(WRITE_THDI_PORT_PG_SPIDr(unit, port, rval));


    
    rval = 0;
    for (idx = 0; idx < _HR4_MMU_NUM_PG; idx++) {
        soc_reg_field_set(unit, THDI_HDRM_PORT_PG_HPIDr, &rval,
                prigroup_hpid_field[idx],
                buf_port->prigroups[idx].pool_idx);
    }
    SOC_IF_ERROR_RETURN(WRITE_THDI_HDRM_PORT_PG_HPIDr(unit, port, rval));
    /* Per port per pool settings */
    mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdi_port_mem[0])[pipe];
    if (mem0 != INVALIDm) {
        for (idx = 0; idx < _HR4_MMU_NUM_POOL; idx++) {
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
    for (idx = 0; idx < _HR4_MMU_NUM_PG; idx++) {
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
        for (idx = 0; idx < _HR4_MMU_NUM_PG; idx++) {
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
            if (!IS_FAE_PORT(unit, port)) {
                soc_mem_field32_set(unit, mem1, &pg_config_mem,
                        PG_HDRM_LIMITf, buf_prigroup->headroom);

                soc_mem_field32_set(unit, mem1, &pg_config_mem, PG_RESET_OFFSETf,
                        0);

                soc_mem_field32_set(unit, mem1, &pg_config_mem,
                        PG_RESET_FLOORf, buf_prigroup->pool_floor);
            }
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

            soc_mem_field32_set(unit, mem0, entry0, Q_MIN_LIMITf, 0);
            soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_ENABLEf, 1);
            soc_mem_field32_set(unit, mem0, entry0, Q_COLOR_LIMIT_DYNAMICf, 1);
            soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_DYNAMICf, 1);
            soc_mem_field32_set(unit, mem0, entry0, Q_SHARED_ALPHAf,  8);
            soc_mem_field32_set(unit, mem0, entry0, Q_SHARED_LIMITf, 8);
            soc_mem_field32_set(unit, mem0, entry0, Q_SPIDf, 0);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, base + idx, entry0));

            sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_offset_entry_t));

            soc_mem_field32_set(unit, mem1, entry0, RESUME_OFFSETf, 2);
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
            if (buf_pool->total == 0) {
                continue;
            }
            sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_queue_config_entry_t));

            soc_mem_field32_set(unit, mem2, entry0, Q_MIN_LIMITf, 1);
            soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_ENABLEf, 1);
            soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_DYNAMICf, 1);
            soc_mem_field32_set(unit, mem2, entry0, Q_COLOR_LIMIT_DYNAMICf, 1);
            soc_mem_field32_set(unit, mem2, entry0, Q_SHARED_ALPHAf, 8);
            soc_mem_field32_set(unit, mem2, entry0, Q_SHARED_LIMITf, 8);
            soc_mem_field32_set(unit, mem2, entry0, Q_SPIDf, 0);

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
    for (idx = 0; idx < _HR4_MMU_NUM_POOL; idx++) {
        buf_port_pool = &buf->ports[port].pools[idx];
        if (buf_port_pool->pool_limit == 0) {
            continue;
        }

        limit = buf_port_pool->pool_limit;
        rlimit = limit - (default_mtu_cells * 2);

        if(lossless) {
           enable = (!IS_OVERSUB_PORT(unit, port) && si->port_speed_max[port] <= 2500) ? 1 : 0;
        }else {
           enable = 1;
        }

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

        soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMIT_ENABLEf, enable);

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

        limit = _HR4_MMU_TOTAL_MCQ_ENTRY(unit);

        soc_mem_field32_set(unit, mem1, entry0, SHARED_LIMITf,
                            (sal_ceil_func(limit, 4)) - 1);
        soc_mem_field32_set(unit, mem1, entry0, YELLOW_SHARED_LIMITf,
                            (sal_ceil_func(limit, 8)) - 1);
        soc_mem_field32_set(unit, mem1, entry0, RED_SHARED_LIMITf,
                            (sal_ceil_func(limit, 8)) - 1);

        soc_mem_field32_set(unit, mem1, entry0, SHARED_LIMIT_ENABLEf, enable);

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
                        buf_queue->yellow_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_RED_CELLf,
                        buf_queue->red_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
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
    for (idx = 0; idx < _HR4_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];

        if (IS_LB_PORT(unit, port) || IS_CPU_PORT(unit, port)
            || IS_FAE_PORT(unit, port)) {
             continue;
        }

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
                            sal_ceil_func(limit , 8) / 8);

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
_soc_hr4_mmu_config_buf_set_hw_global(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless,
                               _soc_hr4_mmu_sw_config_info_t *swcfg)
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
    fval = _HR4_MMU_PHYSICAL_CELLS_PER_XPE - _HR4_MMU_RSVD_CELLS_CFAP_PER_XPE;
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
                      _HR4_MMU_CFAP_BANK_FULL_LIMIT);
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
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, THDI_HDRM_POOL_CFGr, 0, 0, &rval));
        soc_reg_field_set(unit, THDI_HDRM_POOL_CFGr, &rval,
                          PEAK_COUNT_UPDATE_EN_HPf, 0);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, THDI_HDRM_POOL_CFGr, 0, 0, rval));
    /* Input thresholds */
    rval = 0;
    soc_reg_field_set(unit, THDI_GLOBAL_HDRM_LIMITr,
                      &rval, GLOBAL_HDRM_LIMITf, buf->headroom );
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, THDI_GLOBAL_HDRM_LIMITr,
                                      REG_PORT_ANY, 0, rval));

    for (idx = 0; idx < _HR4_MMU_NUM_POOL; idx++) {
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
    for (idx = 0; idx < _HR4_MMU_NUM_POOL; idx++) {
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
                          &rval, RESUME_LIMITf, sal_floor_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_RESUME_LIMITr(unit,
                                                                 idx,
                                                                 rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr,
                          &rval, YELLOW_RESUME_LIMITf, sal_floor_func(limit, 8));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr(unit,
                                                                        idx,
                                                                        rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RED_RESUME_LIMITr,
                          &rval, RED_RESUME_LIMITf, sal_floor_func(limit, 8));
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
        for (idx = 0; idx < SOC_HR4_MMU_CFG_QGROUP_MAX; idx++) {
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
                pval = _soc_hr4_pool_scale_to_limit(queue_grp->pool_limit,
                          queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_RED_CELLf, sal_ceil_func(pval, 8));

            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_RED_CELLf,
                                    sal_ceil_func(queue_grp->red_limit, 8));
            }

            if (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                pval = _soc_hr4_pool_scale_to_limit(queue_grp->pool_limit,
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
    for (idx = 0; idx < _HR4_MMU_NUM_RQE_QUEUES; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_MIN_PRIQr,
                          &rval, MIN_LIMITf, buf_rqe_queue->guarantee);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_LIMIT_MIN_PRIQr(unit, idx, rval));

        rval = 0;
        rval2 = 0;
        rval3 = 0;
        COMPILER_64_SET(rval64, 0, 0);
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
            soc_reg64_field32_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval64,
                              SHARED_ALPHAf, buf_rqe_queue->pool_scale);
        } else {
            soc_reg64_field32_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval64,
                              SHARED_LIMITf, buf_rqe_queue->pool_limit);
        }
        soc_reg64_field32_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval64,
                          RESET_OFFSETf, 2);

        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG1_PRIQr(unit, idx, rval));
        SOC_IF_ERROR_RETURN(soc_reg64_set(unit, MMU_THDR_DB_CONFIG_PRIQr, REG_PORT_ANY, idx, rval64));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_LIMIT_COLOR_PRIQr(unit,
                                                                idx, rval3));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_REDf, 2);
        soc_reg_field_set(unit, MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_YELLOWf,2);
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
    for (idx = 0; idx < _HR4_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if (((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) ||
            (buf_pool->total == 0)) {
            continue;
        }

        limit = swcfg->egr_shared_total;

        COMPILER_64_SET(rval64, 0, 0);
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
_soc_hr4_mmu_config_buf_set_hw(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless,
                               _soc_hr4_mmu_sw_config_info_t *swcfg)
{
    int port;

    SOC_IF_ERROR_RETURN
        (_soc_hr4_mmu_config_buf_set_hw_global(unit, buf, devcfg, lossless,
                                               swcfg));

    PBMP_ALL_ITER(unit, port) {
       SOC_IF_ERROR_RETURN
            (_soc_hr4_mmu_config_buf_set_hw_port(unit, port, buf, devcfg,
                                                 lossless, swcfg));
    }

    return SOC_E_NONE;
}

int
soc_hr4_mmu_config_init_port(int unit, int port)
{
    int rv;
    int lossless = 1;
    _soc_mmu_cfg_buf_t *buf = NULL;
    _soc_mmu_cfg_buf_t *buf_canned = NULL;
    _soc_mmu_cfg_buf_t *buf_user = NULL;
    _soc_mmu_device_info_t devcfg;
    _soc_hr4_mmu_sw_config_info_t swcfg;

    buf_canned = soc_mmu_cfg_alloc(unit);
    if (!buf_canned) {
        return SOC_E_MEMORY;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    SOC_INFO(unit).mmu_lossless = lossless;
    _soc_hr4_mmu_init_dev_config(unit, &devcfg, lossless);
    _soc_hr4_mmu_sw_info_config (unit, &swcfg, lossless);


    /* Calculate the buf - global as well as per port
     * but _soc_hr4_mmu_config_buf_set_hw_port is only called
     * for that port - since it is flex operation - we don't
     * touch any other port */
    _soc_hr4_mmu_config_buf_phase1(unit, buf_canned, &devcfg, &swcfg, lossless);
    _soc_mmu_cfg_buf_check(unit, buf_canned, &devcfg);
    _soc_hr4_mmu_config_buf_phase2(unit, buf_canned, &devcfg, &swcfg, lossless);
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

    rv = _soc_hr4_mmu_config_buf_set_hw(unit, buf, &devcfg, lossless, &swcfg);

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
soc_hr4_mmu_config_init(int unit, int test_only)
{
    int rv;
    int lossless = 1;
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_device_info_t devcfg;
    _soc_hr4_mmu_sw_config_info_t swcfg;

    buf = soc_mmu_cfg_alloc(unit);
    if (!buf) {
        return SOC_E_MEMORY;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    _soc_hr4_mmu_init_dev_config(unit, &devcfg, lossless);
    _soc_hr4_mmu_sw_info_config (unit, &swcfg, lossless);

    _soc_hr4_mmu_config_buf_phase1(unit, buf, &devcfg, &swcfg, lossless);
    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        /* Override default config */
        _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
    }
    rv = _soc_mmu_cfg_buf_check(unit, buf, &devcfg);
    if (!test_only) {
        if (SOC_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "MMU config: Use default setting\n")));
            _soc_hr4_mmu_config_buf_phase1(unit, buf, &devcfg, &swcfg, lossless);
            SOC_IF_ERROR_RETURN
                (_soc_mmu_cfg_buf_calculate(unit, buf, &devcfg));
        }

        _soc_hr4_mmu_config_buf_phase2(unit, buf, &devcfg, &swcfg, lossless);
        /* Override default phase 2 config */
        if (SOC_SUCCESS(rv)) {
            if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
                _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
            }
        }
        rv = _soc_hr4_mmu_config_buf_set_hw(unit, buf, &devcfg, lossless, &swcfg);
    }

    soc_mmu_cfg_free(unit, buf);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));
    return rv;
}

STATIC int
_soc_hurricane4_mmu_init(int unit)
{
    uint32 pipe_map;
    int pipe;
    uint64 enable[NUM_PIPE(unit)];
    uint64 enable64;
    int test_only;
    int alloc_size;


    if (_fwd_ctrl_lock[unit] == NULL) {
        _fwd_ctrl_lock[unit] = sal_mutex_create("_fwd_ctrl_lock");
    }
    if (_fwd_ctrl_lock[unit] == NULL) {
        return SOC_E_MEMORY;
    }

    if (_soc_hr4_mmu_traffic_ctrl[unit] == NULL) {
        alloc_size = sizeof(_soc_hr4_mmu_traffic_ctrl_t);
        _soc_hr4_mmu_traffic_ctrl[unit] =
            sal_alloc(alloc_size,"_soc_hr4_mmu_traffic_ctrl");
        if (_soc_hr4_mmu_traffic_ctrl[unit] == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(_soc_hr4_mmu_traffic_ctrl[unit], 0, alloc_size);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
    } else
#endif
    {
        soc_trident3_pipe_map_get(unit, &pipe_map);

        test_only = (SAL_BOOT_XGSSIM) ? TRUE : FALSE;
        SOC_IF_ERROR_RETURN(soc_hr4_mmu_config_init(unit, test_only));

        sal_memset(enable, 0, sizeof(enable));

        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            u64_H(enable[pipe]) = 0xFFFFFFFF;
            u64_L(enable[pipe]) = 0xFFFFFFFF;
        }

        COMPILER_64_SET(enable64, 0, 0x3f);
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            if (!(pipe_map & (1 << pipe))) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_trident3_xpe_reg_set(unit, THDU_OUTPUT_PORT_RX_ENABLE_SPLIT0r,
                                          -1, pipe, 0, enable[pipe]));
            SOC_IF_ERROR_RETURN
                (soc_trident3_xpe_reg_set(unit, THDU_OUTPUT_PORT_RX_ENABLE_SPLIT1r,
                                          -1, pipe, 0, enable64));
        }

        COMPILER_64_SET(enable64, 0, 0xff);
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            if (!(pipe_map & (1 << pipe))) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_trident3_xpe_reg_set(unit,
                                          MMU_THDM_DB_PORT_RX_ENABLE_64_SPLIT0r,
                                          -1, pipe, 0, enable[pipe]));
            SOC_IF_ERROR_RETURN
                (soc_trident3_xpe_reg_set(unit,
                                          MMU_THDM_DB_PORT_RX_ENABLE_64_SPLIT1r,
                                          -1, pipe, 0, enable64));
        }

        COMPILER_64_SET(enable64, 0, 0xff);
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            if (!(pipe_map & (1 << pipe))) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_trident3_xpe_reg_set(unit, MMU_THDM_MCQE_PORT_RX_ENABLE_64_SPLIT0r,
                                          -1, pipe, 0, enable[pipe]));
            SOC_IF_ERROR_RETURN
                (soc_trident3_xpe_reg_set(unit, MMU_THDM_MCQE_PORT_RX_ENABLE_64_SPLIT1r,
                                          -1, pipe, 0, enable64));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_hr4_mmu_config_shared_update_check(int val1, int val2, int flags)
{
    int rv = 0;
    if (flags == 1) {
        if (val1 > val2) {
            rv = 1;
        }
    } else {
        if (val1 < val2) {
            rv = 1;
        }
    }
    return rv;
}

#define HR4_MMU_SHARED_LIMIT_CHK(val1, val2, flags) \
    _soc_hr4_mmu_config_shared_update_check((val1), (val2), (flags))


int
soc_hr4_mmu_config_shared_buf_recalc(int unit, int res, int thdi_shd,
                                    int thdo_db_shd, int thdo_qe_shd,
                                    int post_update)
{
    soc_info_t *si;
    int pipe;
    uint32 entry0[SOC_MAX_MEM_WORDS], cur_limit, rval = 0, rval2 = 0;
    uint64 rval64;
    thdi_port_pg_config_entry_t pg_config_mem;
    mmu_thdo_config_qgroup_entry_t cfg_qgrp;
    int port, base, numq, idx, midx;
    soc_mem_t base_mem = INVALIDm, base_mem2 = INVALIDm;
    soc_mem_t mem = INVALIDm, mem2 = INVALIDm;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    int granularity, index1;

    si = &SOC_INFO(unit);
    COMPILER_64_SET(rval64, 0, 0);

    /***********************************
     * THDI
 */
    if (res & 0x1) { /* if (res & THDI) { */

        for (idx = 0; idx < _HR4_MMU_NUM_POOL; idx++) {
            /* THDI SP entries */
            rval = 0;
            reg = THDI_BUFFER_CELL_LIMIT_SPr;
            field = LIMITf;
            granularity = 1;
            SOC_IF_ERROR_RETURN
                (READ_THDI_BUFFER_CELL_LIMIT_SPr(unit, idx, &rval));
            cur_limit = soc_reg_field_get(unit, reg, rval, field);
            if ((cur_limit != 0) &&
                HR4_MMU_SHARED_LIMIT_CHK(cur_limit, thdi_shd/granularity,
                                        post_update)) {
                soc_reg_field_set(unit, reg, &rval, field, (thdi_shd/granularity));
                SOC_IF_ERROR_RETURN(
                    soc_trident3_xpe_reg32_set(unit, reg,
                                               -1, -1, idx, rval));
            }
        }

        /* Input per port settings */
        base_mem = THDI_PORT_PG_CONFIGm;
        PBMP_ALL_ITER(unit, port) {
            pipe = si->port_pipe[port];
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            field = PG_SHARED_LIMITf;
            granularity = 1;
            /* Input port per Port-PG settings */
            for (idx = 0; idx < _HR4_MMU_NUM_PG; idx++) {
                midx = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, port, mem, idx);
                sal_memset(&pg_config_mem, 0, sizeof(pg_config_mem));
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                  midx, &pg_config_mem));
                if (!soc_mem_field32_get(unit, mem, &pg_config_mem,
                                         PG_SHARED_DYNAMICf)) {
                    cur_limit = soc_mem_field32_get(unit, mem, &pg_config_mem,
                                                    field);
                    if ((cur_limit != 0) &&
                        HR4_MMU_SHARED_LIMIT_CHK(cur_limit, thdi_shd/granularity,
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, &pg_config_mem,
                                            field, thdi_shd/granularity);
                    }
                }

                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                   midx, &pg_config_mem));
            }
        }
    }

    /***********************************
     * THDO
 */
    if (res & 0x2) { /* if (res & THDO) { */
        /* Per SP settings */
        for (idx = 0; idx < _HR4_MMU_NUM_POOL; idx++) {
            /* DB entries */
            rval = 0;
            reg = MMU_THDM_DB_POOL_SHARED_LIMITr;
            field = SHARED_LIMITf;
            granularity = 1;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
            cur_limit = soc_reg_field_get(unit, reg, rval, field);
            if ((cur_limit != 0) &&
                HR4_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_db_shd/granularity,
                                        post_update)) {
                soc_reg_field_set(unit, reg, &rval, field, (thdo_db_shd/granularity));
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval));
            }

            rval = 0;
            reg = MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr;
            field = YELLOW_SHARED_LIMITf;
            granularity = 8;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
            cur_limit = soc_reg_field_get(unit, reg, rval, field);
            if ((cur_limit != 0) &&
                HR4_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd, granularity),
                                        post_update)) {
                soc_reg_field_set(unit, reg, &rval, field, sal_ceil_func(thdo_db_shd, granularity));
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval));
            }

            rval = 0;
            reg = MMU_THDM_DB_POOL_RED_SHARED_LIMITr;
            field = RED_SHARED_LIMITf;
            granularity = 8;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
            cur_limit = soc_reg_field_get(unit, reg, rval, field);
            if ((cur_limit != 0) &&
                HR4_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd, granularity),
                                        post_update)) {
                soc_reg_field_set(unit, reg, &rval, field, sal_ceil_func(thdo_db_shd, granularity));
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval));
            }

            /* MCQE */
            rval = 0;
            reg = MMU_THDM_MCQE_POOL_SHARED_LIMITr;
            field = SHARED_LIMITf;
            granularity = 1; /* Value retrieved from QE. Hence no further
                              * granularity for Non-color Shd Limit */
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
            cur_limit = soc_reg_field_get(unit, reg, rval, field);
            if ((cur_limit != 0) &&
                HR4_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_qe_shd/granularity,
                                        post_update)) {
                soc_reg_field_set(unit, reg, &rval, field,
                                  (thdo_qe_shd/granularity));
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval));
            }

            rval = 0;
            reg = MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr;
            field = YELLOW_SHARED_LIMITf;
            granularity = 8;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
            cur_limit = soc_reg_field_get(unit, reg, rval, field);
            if ((cur_limit != 0) &&
                HR4_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_qe_shd/granularity,
                                        post_update)) {
                soc_reg_field_set(unit, reg, &rval, field,
                                  (thdo_qe_shd/granularity));
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval));
            }

            rval = 0;
            reg = MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr;
            field = RED_SHARED_LIMITf;
            granularity = 8;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
            cur_limit = soc_reg_field_get(unit, reg, rval, field);
            if ((cur_limit != 0) &&
                HR4_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_qe_shd/granularity,
                                        post_update)) {
                soc_reg_field_set(unit, reg, &rval, field,
                                  (thdo_qe_shd/granularity));
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval));
            }
        }

        /* configure Q-groups */
        base_mem = MMU_THDU_CONFIG_QGROUPm;
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            for (idx = 0; idx < SOC_HR4_MMU_CFG_QGROUP_MAX; idx++) {
                sal_memset(&cfg_qgrp, 0, sizeof(cfg_qgrp));

                SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, idx, &cfg_qgrp));

                field = Q_SHARED_LIMIT_CELLf;
                granularity = 1;
                if (!soc_mem_field32_get(unit, mem, &cfg_qgrp, Q_LIMIT_DYNAMIC_CELLf)) {
                    cur_limit = soc_mem_field32_get(unit, mem, &cfg_qgrp, field);
                    if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_db_shd/granularity,
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, &cfg_qgrp,
                                            field, thdo_db_shd/granularity);
                    }
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, &cfg_qgrp,
                                         Q_COLOR_LIMIT_DYNAMIC_CELLf)) {
                    field = LIMIT_RED_CELLf;
                    cur_limit = soc_mem_field32_get(unit, mem, &cfg_qgrp, field);
                    if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_db_shd/granularity,
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, &cfg_qgrp,
                                            field, thdo_db_shd/granularity);
                    }

                    field = LIMIT_YELLOW_CELLf;
                    cur_limit = soc_mem_field32_get(unit, mem, &cfg_qgrp, field);
                    if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_db_shd/granularity,
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, &cfg_qgrp,
                                            field, thdo_db_shd/granularity);
                    }
                }

                SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, &cfg_qgrp));
            }
        }

        /* Output port per port per queue setting for UC queue */
        PBMP_PORT_ITER(unit, port) {
            base_mem = MMU_THDU_CONFIG_QUEUEm;
            pipe = si->port_pipe[port];

            /* per port UC queue */
            numq = si->port_num_uc_cosq[port];
            base = si->port_uc_cosq_base[port];

            if (numq == 0) {
                continue;
            }
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            for (idx = 0; idx < numq; idx++) {
                sal_memset(entry0, 0, sizeof(mmu_thdu_config_queue_entry_t));

                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL, base + idx, entry0));

                field = Q_SHARED_LIMIT_CELLf;
                granularity = 1;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_LIMIT_DYNAMIC_CELLf)) {
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_db_shd/granularity,
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_db_shd/granularity);
                    }
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMIC_CELLf)) {
                    field = LIMIT_YELLOW_CELLf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd, granularity),
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, sal_ceil_func(thdo_db_shd, granularity));
                    }
                    field = LIMIT_RED_CELLf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                                sal_ceil_func(thdo_db_shd, granularity), post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, sal_ceil_func(thdo_db_shd, granularity));
                    }
                }
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, MEM_BLOCK_ALL, base + idx, entry0));
            }

            /* Per port-SP unicast */
            base_mem = MMU_THDU_CONFIG_PORTm;
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            for (idx = 0; idx < _HR4_MMU_NUM_POOL; idx++) {
                index1 = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, port, mem, idx);
                sal_memset(entry0, 0, sizeof(mmu_thdu_xpipe_config_port_entry_t));

                SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                    index1, entry0));

                field = SHARED_LIMITf;
                granularity = 1;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, thdo_db_shd/granularity);
                }

                granularity = 8;
                field = YELLOW_LIMITf;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, thdo_db_shd/granularity);
                }
                field = RED_LIMITf;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, thdo_db_shd/granularity);
                }

                SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                                  index1, entry0));
            }
        }

        /* Output port per port per queue setting for MC queue */
        PBMP_PORT_ITER(unit, port) {
            base_mem = MMU_THDM_DB_PORTSP_CONFIGm;
            base_mem2 = MMU_THDM_MCQE_PORTSP_CONFIGm;
            pipe = si->port_pipe[port];

            /* Per Port-SP */
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            mem2 = SOC_MEM_UNIQUE_ACC(unit, base_mem2)[pipe];
            for (idx = 0; idx < _HR4_MMU_NUM_POOL; idx++) {
                /* DB entries - Port-SP */
                index1 = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, port, mem, idx);
                sal_memset(entry0, 0, sizeof(mmu_thdm_db_portsp_config_entry_t));

                SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                    index1, entry0));

                field = SHARED_LIMITf;
                granularity = 1;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, thdo_db_shd/granularity);
                }

                field = RED_SHARED_LIMITf;
                granularity = 8;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, thdo_db_shd/granularity);
                }

                field = YELLOW_SHARED_LIMITf;
                granularity = 8;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, thdo_db_shd/granularity);
                }

                SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                    index1, entry0));

                /* MCQE - Port-SP*/
                sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_portsp_config_entry_t));

                SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem2, MEM_BLOCK_ALL,
                                    index1, entry0));
                field = SHARED_LIMITf;
                granularity = 4;
                cur_limit = soc_mem_field32_get(unit, mem2, entry0, field);
                if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_qe_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem2, entry0,
                                        field, thdo_qe_shd/granularity);
                }

                field = RED_SHARED_LIMITf;
                granularity = 8;
                cur_limit = soc_mem_field32_get(unit, mem2, entry0, field);
                if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_qe_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem2, entry0,
                                        field, thdo_qe_shd/granularity);
                }

                field = YELLOW_SHARED_LIMITf;
                granularity = 8;
                cur_limit = soc_mem_field32_get(unit, mem2, entry0, field);
                if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_qe_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem2, entry0,
                                        field, thdo_qe_shd/granularity);
                }
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL,
                                    index1, entry0));

            }

            /* DB entries - Queue */
            numq = si->port_num_cosq[port];
            base = si->port_cosq_base[port];
            if (numq == 0) {
                continue;
            }

            base_mem = MMU_THDM_DB_QUEUE_CONFIGm;
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];

            for (idx = 0; idx < numq; idx++) {
                sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_config_entry_t));

                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL, base + idx, entry0));

                field = Q_SHARED_LIMITf;
                granularity = 1;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_LIMIT_DYNAMICf)) {
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_db_shd/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_db_shd/granularity);
                    }
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMICf)) {
                    field = YELLOW_SHARED_LIMITf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_db_shd/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_db_shd/granularity);
                    }
                    field = RED_SHARED_LIMITf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_db_shd/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_db_shd/granularity);
                    }
                }
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, MEM_BLOCK_ALL, base + idx,
                                   entry0));
            }

            /* MCQE - Queue */
            base_mem = MMU_THDM_MCQE_QUEUE_CONFIGm;
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            for (idx = 0; idx < numq; idx++) {
                sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_queue_config_entry_t));

                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL, base + idx, entry0));

                field = Q_SHARED_LIMITf;
                granularity = 4;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_LIMIT_DYNAMICf)) {
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_qe_shd/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_qe_shd/granularity);
                    }
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMICf)) {
                    field = YELLOW_SHARED_LIMITf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_qe_shd/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_qe_shd/granularity);
                    }
                    field = RED_SHARED_LIMITf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_qe_shd/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_qe_shd/granularity);
                    }
                }
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, MEM_BLOCK_ALL, base + idx,
                                   entry0));
            }
        }

        /* RQE */
        for (idx = 0; idx < 11; idx++) {
            rval = 0;
            COMPILER_64_SET(rval64, 0, 0);

            SOC_IF_ERROR_RETURN
                (READ_MMU_THDR_DB_CONFIG1_PRIQr(unit, idx, &rval));

            reg = MMU_THDR_DB_CONFIG_PRIQr;
            field = SHARED_LIMITf;
            granularity = 1;
            SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, reg, REG_PORT_ANY, idx, &rval64));
            if (!soc_reg_field_get(unit, MMU_THDR_DB_CONFIG1_PRIQr, rval,
                                   DYNAMIC_ENABLEf)) {
                cur_limit = soc_reg64_field32_get(unit, reg, rval64, field);
                if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_reg64_field32_set(unit, reg, &rval64, field,
                                      (thdo_db_shd/granularity));
                    SOC_IF_ERROR_RETURN
                        (soc_reg64_set(unit, reg, REG_PORT_ANY, idx, rval64));
                }
            }

            rval2 = 0;
            reg = MMU_THDR_DB_LIMIT_COLOR_PRIQr;
            granularity = 8;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval2));

            if (!soc_reg_field_get(unit, MMU_THDR_DB_CONFIG1_PRIQr, rval,
                                   COLOR_LIMIT_DYNAMICf)) {
                field = SHARED_RED_LIMITf;
                cur_limit = soc_reg_field_get(unit, reg, rval2, field);
                if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_reg_field_set(unit, reg, &rval2, field, (thdo_db_shd/granularity));
                }

                field = SHARED_YELLOW_LIMITf;
                cur_limit = soc_reg_field_get(unit, reg, rval2, field);
                if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_reg_field_set(unit, reg, &rval2, field, (thdo_db_shd/granularity));
                }
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval2));
            }

        }
        /* per pool RQE settings */
        for (idx = 0; idx < 4; idx++) {
            reg = MMU_THDR_DB_CONFIG_SPr;
            field = SHARED_LIMITf;
            granularity = 1;

            SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, reg, REG_PORT_ANY, idx, &rval64));
            cur_limit = soc_reg_field_get(unit, reg, rval, field);

            if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                        thdo_db_shd/granularity, post_update)) {
                soc_reg64_field32_set
                    (unit, reg, &rval64, field, (thdo_db_shd/granularity));
                SOC_IF_ERROR_RETURN
                    (soc_reg64_set(unit, reg, REG_PORT_ANY, idx, rval64));
            }

            reg = MMU_THDR_DB_SP_SHARED_LIMITr;
            granularity = 8;

            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval2));

            field = SHARED_YELLOW_LIMITf;
            cur_limit = soc_reg_field_get(unit, reg, rval, field);

            if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                        thdo_db_shd/granularity, post_update)) {
                soc_reg_field_set
                    (unit, reg, &rval2, field, (thdo_db_shd/granularity));
            }

            field = SHARED_RED_LIMITf;
            cur_limit = soc_reg_field_get(unit, reg, rval, field);

            if (HR4_MMU_SHARED_LIMIT_CHK(cur_limit,
                        thdo_db_shd/granularity, post_update)) {
                soc_reg_field_set
                    (unit, reg, &rval2, field, (thdo_db_shd/granularity));
            }
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval2));
        }
    }
    return SOC_E_NONE;
}

soc_pstats_tbl_desc_t
_soc_hr4_pstats_tbl_desc_all = {SOC_BLK_NONE, INVALIDm, FALSE, FALSE};
/* pstats support */
soc_pstats_tbl_desc_t _soc_hr4_pstats_tbl_desc[] = {
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
soc_hurricane4_mmu_pstats_tbl_config_all(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_mem_desc_t *desc, *desc_all;
    int ti, mi, di = 0;

    desc_all = _soc_hr4_pstats_tbl_desc_all.desc;
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

    soc->pstats_tbl_desc = &_soc_hr4_pstats_tbl_desc_all;
    soc->pstats_tbl_desc_count = 1;
}

void
soc_hurricane4_mmu_pstats_tbl_config(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_mem_desc_t *desc;
    soc_mem_t bmem;
    int pe, mor_dma;
    int ti, mi, xpe, pipe;

    soc->pstats_tbl_desc = _soc_hr4_pstats_tbl_desc;
    soc->pstats_tbl_desc_count = COUNTOF(_soc_hr4_pstats_tbl_desc);
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
        soc_hurricane4_mmu_pstats_tbl_config_all(unit);
    }
}

int
soc_hurricane4_mmu_pstats_mode_set(int unit, uint32 flags)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int pf = 0;
    uint32 rval = 0;
    int rv;

    SOC_PSTATS_LOCK(unit);

    if (flags & _HR4_MMU_PSTATS_HWM_MOD) {
        soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval,
                          HWM_MODEf, 1);
        pf |= _HR4_MMU_PSTATS_HWM_MOD;
        if (flags & _HR4_MMU_PSTATS_RESET_ON_READ) {
            soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval,
                              HWM_RESET_ON_READf, 1);
            pf |= _HR4_MMU_PSTATS_RESET_ON_READ;
        }
    }

    if (soc_feature(unit, soc_feature_mor_dma) &&
        (flags & _HR4_MMU_PSTATS_MOR_EN)) {
        soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval,
                          MOR_ENf, 1);
    }
    if (flags & _HR4_MMU_PSTATS_MOR_EN) {
        pf |= _HR4_MMU_PSTATS_MOR_EN;
    }

    if (flags & _HR4_MMU_PSTATS_ENABLE) {
        soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval,
                          ENABLEf, 1);
        pf |= _HR4_MMU_PSTATS_ENABLE;
        if ((flags & _HR4_MMU_PSTATS_SYNC) ||
            (flags & _HR4_MMU_PSTATS_HWM_MOD &&
             flags & _HR4_MMU_PSTATS_RESET_ON_READ)) {
            soc->pstats_mode = PSTATS_MODE_PKT_SYNC;
            pf |= _HR4_MMU_PSTATS_SYNC;
        } else {
            soc->pstats_mode = PSTATS_MODE_PKT_BUFF;
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
soc_hurricane4_mmu_pstats_mode_get(int unit, uint32 *flags)
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
        pf |= _HR4_MMU_PSTATS_ENABLE;
        pf |= _HR4_MMU_PSTATS_PKT_MOD;
    }
    if (soc_reg_field_get(unit, reg, rval, HWM_MODEf)) {
        pf |= _HR4_MMU_PSTATS_HWM_MOD;
        if (soc_reg_field_get(unit, reg, rval, HWM_RESET_ON_READf)) {
            pf |= _HR4_MMU_PSTATS_RESET_ON_READ;
        }
    }

    SOC_PSTATS_UNLOCK(unit);

    *flags = pf;

    return SOC_E_NONE;
}

int
soc_hurricane4_mmu_pstats_mor_enable(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 rval;


    if (soc->pstats_flags & _HR4_MMU_PSTATS_MOR_EN) {
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
soc_hurricane4_mmu_pstats_mor_disable(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 rval;

    if (!soc_feature(unit, soc_feature_mor_dma) &&
        (soc->pstats_flags & _HR4_MMU_PSTATS_MOR_EN)) {
        SOC_IF_ERROR_RETURN(READ_MMU_GCFG_PKTSTAT_OOBSTATr(unit, &rval));
        soc_reg_field_set(unit, MMU_GCFG_PKTSTAT_OOBSTATr, &rval, MOR_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_PKTSTAT_OOBSTATr(unit, rval));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_hr4_port_schedule_tdm_init
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
soc_hr4_port_schedule_tdm_init(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    _soc_hurricane4_tdm_temp_t *tdm = soc->tdm_info;
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
                                        _HR4_TDM_CALENDAR_UNIVERSAL :
                                        _HR4_TDM_CALENDAR_ETHERNET_OPTIMIZED;
	*/
    /* Construct tdm_ingress_schedule_pipe and tdm_egress_schedule_pipe */
    for (pipe = 0; pipe <  HURRICANE4_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        tdm_eschd = &port_schedule_state->tdm_egress_schedule_pipe[pipe];

        tdm_ischd->num_slices = HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE;
        tdm_eschd->num_slices = HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE;

        if (is_flexport) {
            /* TDM Calendar is always in slice 0 */
            sal_memcpy(tdm_ischd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].idb_tdm,
                        sizeof(int)*_HR4_TDM_LENGTH);
            sal_memcpy(tdm_eschd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].mmu_tdm,
                        sizeof(int)*_HR4_TDM_LENGTH);
        }

        /* OVS */
        for (hpipe = 0; hpipe < HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            /* IDB OVERSUB*/
            sched = &tdm_ischd->tdm_schedule_slice[hpipe];
            sched->cal_len = _HR4_TDM_LENGTH;
            sched->num_ovs_groups = HURRICANE4_TDM_OVS_GROUPS_PER_HPIPE;
            sched->ovs_group_len = HURRICANE4_TDM_OVS_GROUP_LENGTH;
            sched->num_pkt_sch_or_ovs_space = 1;
            sched->pkt_sch_or_ovs_space_len = _HR4_PKT_SCH_LENGTH;

            if (is_flexport) {
                for (group = 0; group < HURRICANE4_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                    for (slot = 0; slot < HURRICANE4_TDM_OVS_GROUP_LENGTH; slot++) {
                        sched->oversub_schedule[group][slot] =
                            tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot];
                    }
                }
                for (slot = 0; slot < _HR4_PKT_SCH_LENGTH; slot++) {
                    sched->pkt_sch_or_ovs_space[0][slot] =
                        tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot];
                }
            }

            /* MMU OVERSUB */
            sched = &tdm_eschd->tdm_schedule_slice[hpipe];
            sched->cal_len = _HR4_TDM_LENGTH;
            sched->num_ovs_groups = HURRICANE4_TDM_OVS_GROUPS_PER_HPIPE;
            sched->ovs_group_len = HURRICANE4_TDM_OVS_GROUP_LENGTH;
            sched->num_pkt_sch_or_ovs_space = 1;
            sched->pkt_sch_or_ovs_space_len = _HR4_PKT_SCH_LENGTH;

            if (is_flexport) {
                for (group = 0; group < HURRICANE4_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                    for (slot = 0; slot < HURRICANE4_TDM_OVS_GROUP_LENGTH; slot++) {
                        sched->oversub_schedule[group][slot] =
                            tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot];
                        }
                    }
                for (slot = 0; slot < _HR4_PKT_SCH_LENGTH; slot++) {
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
        for (phy_port = 1; phy_port < HR4_NUM_EXT_PORTS; phy_port++) {
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
 *      soc_hr4_soc_tdm_update
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
soc_hr4_soc_tdm_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    _soc_hurricane4_tdm_temp_t *tdm = soc->tdm_info;
    soc_tdm_schedule_pipe_t *tdm_ischd, *tdm_eschd;
    soc_tdm_schedule_t *sched;
    int pipe, hpipe, phy_port, slot;
    int index, group;

    if (tdm == NULL) {
        return SOC_E_INIT;
    }

    /* Copy info from soc_port_schedule_state_t to _soc_hurricane4_tdm_temp_t */
    for (pipe = 0; pipe <  HURRICANE4_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        tdm_eschd = &port_schedule_state->tdm_egress_schedule_pipe[pipe];

        /* TDM Calendar is always in slice 0 */
        sal_memcpy(tdm->tdm_pipe[pipe].idb_tdm,
                    tdm_ischd->tdm_schedule_slice[0].linerate_schedule,
                    sizeof(int)*_HR4_TDM_LENGTH);
        sal_memcpy(tdm->tdm_pipe[pipe].mmu_tdm,
                    tdm_eschd->tdm_schedule_slice[0].linerate_schedule,
                    sizeof(int)*_HR4_TDM_LENGTH);

        for (hpipe = 0; hpipe < HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            sched = &tdm_ischd->tdm_schedule_slice[hpipe];
            for (group = 0; group < HURRICANE4_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                for (slot = 0; slot < HURRICANE4_TDM_OVS_GROUP_LENGTH; slot++) {
                    tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot] =
                        sched->oversub_schedule[group][slot];
                }
            }
            for (slot = 0; slot < _HR4_PKT_SCH_LENGTH; slot++) {
                tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot] =
                    sched->pkt_sch_or_ovs_space[0][slot];

            }
        }
    }

    /* pblk info init  */
    for (phy_port = 1; phy_port < HR4_NUM_EXT_PORTS; phy_port++) {
        tdm->pblk_info[phy_port].pblk_cal_idx = -1;
        tdm->pblk_info[phy_port].pblk_hpipe_num = -1;
    }

    /* pblk info for phy_port */
    for (pipe = 0; pipe < HURRICANE4_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        for (hpipe = 0; hpipe < HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            for (group = 0; group < HURRICANE4_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                for (index = 0; index < HURRICANE4_TDM_OVS_GROUP_LENGTH; index++) {
                    phy_port =
                        tdm_ischd->tdm_schedule_slice[hpipe].oversub_schedule[group][index];
                    if (phy_port < HR4_NUM_EXT_PORTS) {
                        tdm->pblk_info[phy_port].pblk_hpipe_num = hpipe;
                        tdm->pblk_info[phy_port].pblk_cal_idx =
                            port_schedule_state->out_port_map.port_p2PBLK_inst_mapping[phy_port];
                    }
                }
            }
        }
    }

    return SOC_E_NONE;
}

void
soc_hurricane4_port_qsgmii_mode_get(int unit, soc_port_t port, int* qsgmii_mode)
{
    soc_info_t *si;
    int phy_port, base_port;

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    base_port = SOC_HR4_PORT_BLOCK_BASE_PORT(port);

    if ((phy_port >= 25) && (phy_port <= 56)) {
        *qsgmii_mode = soc_property_phys_port_get(unit, base_port,
                                                  spn_PORT_GMII_MODE, 0);
    } else {
        *qsgmii_mode = 0;
    }

    return;
}

int
soc_hr4_mpls_hash_control_set(int *hash_entry)
{
    int unit = 0;
    int mpls_banks = 0;
    int index = 0;
    int offset_base = 0, offset_multiplier = 0;
    int hash_control_entry[2];
    uint32 shared_hash_control_entry[8];

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
            sizeof(ip_uat_shared_banks_control_entry_t));

    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, MPLS_ENTRY_SINGLEm,
                                          &mpls_banks));
    SOC_IF_ERROR_RETURN
        (READ_IP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                        shared_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (READ_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                       hash_control_entry));
    sal_memcpy(hash_entry, hash_control_entry, sizeof(hash_control_entry));

    if (mpls_banks) {
        offset_base = 64 % (mpls_banks);
        offset_multiplier = 64 / (mpls_banks);
        }

    soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, ((1 << mpls_banks) - 1));
    for(index = 0; index < mpls_banks; index++) {
        soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, hash_control_entry,
                            logical_to_physical_fields[index], index);
        soc_mem_field32_set(unit, IP_UAT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index],
                            offset_base + ( index * offset_multiplier));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                        hash_control_entry));

    return SOC_E_NONE;
}


void
soc_hurricane4_temperature_intr(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4)
{
    int i, rv;
    uint32 rval;
    int trval;
    int unit = PTR_TO_INT(unit_vp);
    int num_entries_out =0;

    if ((rv = READ_TOP_PVTMON_INTR_STATUSr(unit, &rval)) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "HR4 Temp Intr, Reg access error.\n")));
    }

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "High temp interrupt: 0x%08x\n"), rval));

    num_entries_out = COUNTOF(hr4_pvtmon_result_reg);

    /* Raise event to app for it to take further graceful actions */
    for (i = 0; i < num_entries_out; i++) {
        if ((1 << i) & _soc_hr4_temp_mon_mask[unit]) {
            if ((rv = soc_reg32_get(unit, hr4_pvtmon_result_reg[i], REG_PORT_ANY, 0,
                                    &rval)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "HR4 Temp Intr, Reg access error.\n")));
            }
            trval = soc_reg_field_get(unit, hr4_pvtmon_result_reg[i], rval,
                                      PVT_DATAf);
            /* Convert data to temperature.
             * temp = 457.8289-(data*0.5570) = (4578289-(trval*5570))/1000;
             * Note: Since this is a high temp interrupt we can safely assume
             * that this will end up being a +ive value.
             */
            trval = (4578289 - (trval * 5570)) / 1000;

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TempMon %d: %3d.%d deg C.\n"), i,
                                  trval / 10,
                                  (trval >= 0 ? (trval % 10) : ((-trval) % 10))));
            (void)soc_event_generate(unit, SOC_SWITCH_EVENT_ALARM,
                                     SOC_SWITCH_EVENT_ALARM_HIGH_TEMP, i, trval);
        }
    }

    /* Acknowledge the interrupt - write 1 to clear latch bit */
    if ((rv = WRITE_TOP_PVTMON_INTR_STATUSr(unit, 0xFFFFFFFF)) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "HR4 Temp Intr, Reg access error.\n")));
    }

    /* Re-enable top lp interrupt, local PVT interrup is managed via intr mask */
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        (void)soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
    }
#endif

    /* HR4 has HW overtemp protection,  no shutdown logic needed. */
    return;
}

int
soc_hurricane4_temperature_monitor_get(int unit, int temperature_max,
    soc_switch_temperature_monitor_t *temperature_array, int *temperature_count)
{
    soc_reg_t reg;
    int index;
    uint32 rval;
    int fval, cur, peak;
    int num_entries_out;
    int iter_count = 2;

    if (temperature_count) {
        *temperature_count = 0;
    }

    if (COUNTOF(hr4_pvtmon_result_reg) > temperature_max) {
        num_entries_out = temperature_max;
    } else {
        num_entries_out = COUNTOF(hr4_pvtmon_result_reg);
    }

    while (iter_count--) {
        /* Configure SELECT for temperature */
        SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_3r(unit, &rval));
        if (soc_reg_field_get(unit, TOP_PVTMON_CTRL_3r, rval, PVTMON_RESET_Nf) == 0) {
            soc_reg_field_set(unit, TOP_PVTMON_CTRL_3r, &rval, PVTMON_RESET_Nf, 1);
        }
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_3r, &rval, PVTMON_SELECTf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_3r(unit, rval));
        sal_usleep(1000);

        rval = 0;
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT0f, 0);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT1f, 0);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT2f, 0);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT3f, 0);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT4f, 0);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT5f, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_2r(unit, rval));
        sal_usleep(1000);

        for (index = 0; index < num_entries_out; index++) {
            reg = hr4_pvtmon_result_reg[index];
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

            fval = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
            cur = (4578289 - (fval * 5570)) / 1000;

            fval = soc_reg_field_get(unit, reg, rval, MIN_PVT_DATAf);
            peak = (4578289 - (fval * 5570)) / 1000;

            if (temperature_array) {
                (temperature_array + index)->curr = cur;
                (temperature_array + index)->peak = peak;
            }
        }
        /* Reset Min & Max logic for PVTMON */
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                          TOP_PVT_MON_MIN_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                          TOP_PVT_MON_MAX_RST_Lf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                          TOP_PVT_MON_MIN_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                          TOP_PVT_MON_MAX_RST_Lf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    }

    if (temperature_count) {
        *temperature_count = num_entries_out;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *    soc_hurricane4_port_is_portctrl
 * Purpose:
 *    Helper function to check if PORTCTRL is enabled for a port.
 * Parameters:
 *    unit - (IN) Device Number
 * Return Value:
 *    TRUE/FALSE
 * Notes:
 */
int
_soc_hurricane4_port_is_portctrl(int unit, int port)
{
#ifdef PORTMOD_SUPPORT
    int lport;
    int rv = SOC_E_NONE;

    if (!SOC_IS_HURRICANE4(unit)) {
        return TRUE;
    }

    lport = port;
    if (port != BCM_PORT_INVALID) {
        rv = _bcm_esw_port_gport_validate(unit, port, &lport);

        if (_SHR_E_FAILURE(rv) &&
            !(SOC_E_INIT == rv && SOC_WARM_BOOT(unit))) {
            LOG_DEBUG(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit, "Port 0x%x (%d)\n"), port, rv));
        }
    }

    if ((_SHR_E_SUCCESS(rv) || (SOC_E_INIT == rv && SOC_WARM_BOOT(unit))) &&
        lport >= 0 && lport < SOC_MAX_NUM_PORTS) {
        if (IS_EGPHY_PORT(unit, lport)) {
            return FALSE;
        }
    }
#endif
    return TRUE;
}

void
soc_hurricane4_sbus_ring_map_config(int unit)
{
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
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_8_15_OFFSET,0x44455005);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_16_23_OFFSET,0x66666664);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_24_31_OFFSET,0x70003666);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_32_39_OFFSET,0x00000044);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_40_47_OFFSET,0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_48_55_OFFSET,0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_56_63_OFFSET,0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_TIMEOUT_OFFSET,0x5000);
    }
#endif

    return;
}

/*
 * Function:
 *      soc_hr4_support_speeds
 * Purpose:
 *      Get the supported speed of port for specified lanes
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      ports                - (IN)  Logical port number.
 *      speed_mask           - (IN)  Bitmap for supported speed.
 * Returns:
 *      BCM_E_xxx
 */
int
soc_hr4_support_speeds(int unit, int port, uint32 *speed_mask)
{
    uint32 speed_valid;
    int phy_port, lanes;
    soc_info_t *si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    lanes = si->port_num_lanes[port];


    switch(lanes) {
    case 1:
        /* Single Lane Port Speeds */
        if (HR4_PORT_IS_MACSEC(unit, phy_port)) {
            speed_valid = SOC_PA_SPEED_10GB | SOC_PA_SPEED_20GB | SOC_PA_SPEED_25GB;
        } else {
            speed_valid = SOC_PA_SPEED_1000MB;
            if (!HR4_PORT_IS_GPHY(unit, phy_port)) {
                speed_valid |=  SOC_PA_SPEED_10GB | SOC_PA_SPEED_11GB;
            }
            if (HR4_PORT_IS_FALCON(unit, phy_port)) {
                speed_valid |= SOC_PA_SPEED_25GB | SOC_PA_SPEED_27GB;
            } else {
                speed_valid |=  SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB;
                if (!HR4_PORT_IS_GPHY(unit, phy_port)) {
                    speed_valid |=  SOC_PA_SPEED_2500MB | SOC_PA_SPEED_5000MB;
                }
            }
        }
        break;
    case 2:
        /* Dual Lane Port Speeds */
        if (HR4_PORT_IS_GPHY(unit, phy_port)) {
            return SOC_E_PARAM;
        }
        speed_valid =  SOC_PA_SPEED_10GB | SOC_PA_SPEED_11GB;
        if (!HR4_PORT_IS_PMQ1(unit, phy_port)) {
            speed_valid |= SOC_PA_SPEED_20GB | SOC_PA_SPEED_21GB;
        }
        break;
    default:
        return SOC_E_PARAM;
    }

    *speed_mask = speed_valid;

    return SOC_E_NONE;
}


#ifdef BCM_WARM_BOOT_SUPPORT

#define SOC_FLEXPORT_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_FLEXPORT_WB_DEFAULT_VERSION            SOC_FLEXPORT_WB_VERSION_1_0

int
soc_hr4_flexport_scache_allocate(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int stable_size;
    int default_ver = SOC_FLEXPORT_WB_DEFAULT_VERSION;

    alloc_size =  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* phy to logical*/
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* phy to mmu */
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* logical to phy */
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* max port speed */
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* init port speed */
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* num of lanes */
                  (sizeof(pbmp_t))                       + /* HG bitmap */
                  (sizeof(pbmp_t));                        /* Disabled bitmap */

    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    /* check to see if an scache table has been configured */
    rv = soc_stable_size_get(unit, &stable_size);
    if (SOC_FAILURE(rv) || stable_size <= 0) {
        return rv;
    }

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                          TRUE, &alloc_get,
                                          &flexport_scache_ptr,
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
                 "ERROR: scache memory for flexport size mismatch"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }

    if (NULL == flexport_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory not allocated for flexport"
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
 *      soc_hr4_flexport_scache_sync
 * Purpose:
 *      Record Port info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
 *    port_speed_max
 *    port_init_speed
 *    port_num_lanes
 *    disabled_bitmap
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_hr4_flexport_scache_sync(int unit)
{
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    uint32 var_size = 0;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 scache_offset=0;
    int rv = 0;

    alloc_size =  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* phy to logical*/
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* phy to mmu */
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* logical to phy */
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* max port speed */
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* init port speed */
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* num of lanes */
                  (sizeof(pbmp_t))                       + /* HG bitmap */
                  (sizeof(pbmp_t));                        /* Disabled bitmap */

    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_get,
                        &flexport_scache_ptr,
                        SOC_FLEXPORT_WB_DEFAULT_VERSION,
                        NULL);
    if (rv == SOC_E_NOT_FOUND) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory for flexport size mismatch"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }
    if (NULL == flexport_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory not allocated for flexport"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /* Physical to logical port mapping */
    var_size = sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV;
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2l_mapping, var_size);
    scache_offset += var_size;


    /* Physical to MMU port mapping */
    var_size = sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV;
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2m_mapping, var_size);
    scache_offset += var_size;

    /* Logical to Physical port mapping */
    var_size = sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV;
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_l2p_mapping, var_size);
    scache_offset += var_size;

    /* Max port speed */
    var_size = sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV;

    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_speed_max, var_size);
    scache_offset += var_size;

    /* Init port speed */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_init_speed, var_size);
    scache_offset += var_size;

    /* Num of lanes */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_num_lanes, var_size);
    scache_offset += var_size;

    /* HG Bitmap */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               &si->hg.bitmap,
               sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    /* Disabled Port Bitmap */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               &si->all.disabled_bitmap,
               sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_hr4_flexport_scache_recovery
 * Purpose:
 *      Recover Port info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
 *    port_p2l_mapping
 *    port_l2p_mapping
 *    port_speed_max
 *    port_init_speed
 *    port_num_lanes
 *    disabled_bitmap
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_hr4_flexport_scache_recovery(int unit)
{
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int rv = SOC_E_NONE, port, phy_port;
    uint8 *flexport_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    uint32 scache_offset=0;
    uint32 var_size = 0;
    uint16 recovered_ver = 0;
    soc_info_t *si = &SOC_INFO(unit);

    alloc_size =  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* phy to logical*/
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* phy to mmu */
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* logical to phy */
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* max port speed */
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* init port speed */
                  (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV) + /* num of lanes */
                  (sizeof(pbmp_t))                         + /* HG bitmap */
                  (sizeof(pbmp_t));                          /* Disabled bitmap */

    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_get,
                        &flexport_scache_ptr,
                        SOC_FLEXPORT_WB_DEFAULT_VERSION,
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
                 "ERROR: scache recovery for flexport"
                 "%s()[LINE:%d] DONE \n"),
                  FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }

    if (NULL == flexport_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache recovery for flexport"
                 "%s()[LINE:%d] DONE \n"),
                  FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /*Physical to logical port mapping */
    var_size = (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV);
    sal_memcpy(si->port_p2l_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /*Physical to MMU port mapping */
    var_size = (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV);
    sal_memcpy(si->port_p2m_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /*Logical to Physical port mapping*/
    var_size = (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV);
    sal_memcpy(si->port_l2p_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Max port speed */
    var_size = (sizeof(int) * HURRICANE4_PHY_PORTS_PER_DEV);

    sal_memcpy(si->port_speed_max,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Init port speed */
    sal_memcpy(si->port_init_speed,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Num of lanes */
    sal_memcpy(si->port_num_lanes,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* HG Bitmap */
    sal_memcpy(&si->hg.bitmap,
           &flexport_scache_ptr[scache_offset],
           sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    /* Disabled Port Bitmap */
    sal_memcpy(&si->all.disabled_bitmap,
           &flexport_scache_ptr[scache_offset],
           sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    SOC_PBMP_CLEAR(si->pipe_pbm[0]);

    for (port = 1; port < HR4_NUM_PORT; port++) {
        phy_port = si->port_l2p_mapping[port];
        if (phy_port == -1) {
            si->port_l2i_mapping[port] = -1;
            si->port_serdes[port] = -1;
            si->port_pipe[port] = -1;
            continue;
        }
        if (HR4_PORT_IS_MACSEC(unit, phy_port)) {
            si->port_l2i_mapping[port] = phy_port;
        } else if (phy_port == 69) {
            si->port_l2i_mapping[port] = 71;
        } else {
            si->port_l2i_mapping[port] = phy_port - 1;
        }
        si->port_serdes[port] = HR4_PHY_PORT_SERDERS(phy_port);
        si->port_pipe[port] = 0;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[0], port);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

#define SOC_TDM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_TDM_WB_DEFAULT_VERSION            SOC_TDM_WB_VERSION_1_0

/*
 * Function:
 *      soc_hr4_tdm_scache_allocate
 * Purpose:
 *      Calculate TDM info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
 *    IDB TDM info on pipe0
 *    MMU TDM info on pipe0
 *    Oversub group info on pipe0
 *    pkt shaper info on pipe0
 *    pblk id of phy-port
 *    port ratio
 *    oversub ratio
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int soc_hr4_tdm_scache_allocate(int unit)
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
    ovs_size = HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE *
               HURRICANE4_OVS_GROUP_COUNT_PER_HPIPE *
               HURRICANE4_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE *
                   _HR4_PKT_SCH_LENGTH;
    hpipes = HURRICANE4_PIPES_PER_DEV * HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * HURRICANE4_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * HURRICANE4_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * HURRICANE4_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * HURRICANE4_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * HURRICANE4_PBLKS_PER_DEV)     + /* port ratio */
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
 *      soc_hr4_tdm_scache_sync
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
 *    pblk id of phy-port
 *    port ratio
 *    oversub ratio
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int soc_hr4_tdm_scache_sync(int unit)
{
    uint8 *tdm_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    uint32 var_size = 0;
    uint32 scache_offset=0;
    int rv = 0;
    int ilen, ovs_size, pkt_shp_size, hpipes, phy_port;
    _soc_hurricane4_tdm_temp_t  *tdm = SOC_CONTROL(unit)->tdm_info;

    ilen = sizeof(int);
    ovs_size = HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE *
               HURRICANE4_OVS_GROUP_COUNT_PER_HPIPE *
               HURRICANE4_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE *
                   _HR4_PKT_SCH_LENGTH;
    hpipes = HURRICANE4_PIPES_PER_DEV * HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * HURRICANE4_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * HURRICANE4_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * HURRICANE4_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * HURRICANE4_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * HURRICANE4_PBLKS_PER_DEV)     + /* port ratio */
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
    var_size = ilen * HURRICANE4_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[0].idb_tdm,
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe0 */
    var_size = ilen * HURRICANE4_TDM_LENGTH;
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

    /* hpipe id of phy-port */
    for (phy_port = 0; phy_port < HURRICANE4_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm_scache_ptr[scache_offset],
                   &tdm->pblk_info[phy_port].pblk_hpipe_num,
                   var_size);
        scache_offset += var_size;
    }

    /* pblk id of phy-port */
    for (phy_port = 0; phy_port < HURRICANE4_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm_scache_ptr[scache_offset],
                   &tdm->pblk_info[phy_port].pblk_cal_idx,
                   var_size);
        scache_offset += var_size;
    }

    /* port ratio */
    var_size = ilen * HURRICANE4_PBLKS_PER_DEV;
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
 *      soc_hr4_tdm_scache_recovery
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
int soc_hr4_tdm_scache_recovery(int unit)
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
    _soc_hurricane4_tdm_temp_t *tdm = SOC_CONTROL(unit)->tdm_info;

    ilen = sizeof(int);
    ovs_size = HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE *
               HURRICANE4_OVS_GROUP_COUNT_PER_HPIPE *
               HURRICANE4_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE *
                   _HR4_PKT_SCH_LENGTH;
    hpipes = HURRICANE4_PIPES_PER_DEV * HURRICANE4_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * HURRICANE4_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * HURRICANE4_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * HURRICANE4_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * HURRICANE4_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * HURRICANE4_PBLKS_PER_DEV)     + /* port ratio */
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
    var_size = ilen * HURRICANE4_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[0].idb_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe0 */
    var_size = ilen * HURRICANE4_TDM_LENGTH;
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

    /* hpipe id of phy-port */
    for (phy_port = 0; phy_port < HURRICANE4_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm->pblk_info[phy_port].pblk_hpipe_num,
                   &tdm_scache_ptr[scache_offset],
                   var_size);
        scache_offset += var_size;
    }

    /* pblk id of phy-port */
    for (phy_port = 0; phy_port < HURRICANE4_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm->pblk_info[phy_port].pblk_cal_idx,
                   &tdm_scache_ptr[scache_offset],
                   var_size);
        scache_offset += var_size;
    }

    /* port ratio */
    var_size = ilen * HURRICANE4_PBLKS_PER_DEV;
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


int
soc_hurricane4_show_voltage(int unit)
{
    int index;
    soc_reg_t reg;
    uint32 rval = 0, fval = 0, avg = 0;

    /* Configure SELECT  for voltage */
    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_3r(unit, &rval));
    if (soc_reg_field_get(unit, TOP_PVTMON_CTRL_3r, rval, PVTMON_RESET_Nf) == 0) {
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_3r, &rval, PVTMON_RESET_Nf, 1);
    }
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_3r, &rval, PVTMON_SELECTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_3r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT0f, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT1f, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT2f, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT3f, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT4f, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT5f, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_2r(unit, rval));

    sal_usleep(1000);

    avg = 0;

    /* Read Voltages. */
    for (index = 0; index < COUNTOF(hr4_pvtmon_result_reg); index++) {
        reg = hr4_pvtmon_result_reg[index];

        /* TOP_PVTMON_RESULT_6r is used for AVS, skip the same from
           voltage calculation */
        if (reg == TOP_PVTMON_RESULT_6r) {
            continue;
        }

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        fval = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
        /* (PVT_DATA * 950) / (1024 * 0.8) */
        fval = (fval * 950 * 10) / (1024 * 8);

        LOG_CLI((BSL_META_U(unit,
                            "Voltage monitor %d: voltage = %d.%03dV\n"),
                 index, (fval / 1000), (fval % 1000)));

        avg += fval;
    }

    /* Get count and reduce 1 since TOP_PVTMON_RESULT_6r is not not applicable */
    avg /= (COUNTOF(hr4_pvtmon_result_reg) - 1);
    LOG_CLI((BSL_META_U(unit,
                        "Average voltage is = %d.%03dV\n"),
             (avg / 1000), (avg % 1000)));

    /* Reset Min & Max logic for PVTMON */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MIN_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MAX_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MIN_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MAX_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    return SOC_E_NONE;
}

/*
 * Few SKU's doesn't support BroadScan feature in HW.
 * Skip those memories during SER init/TR's.
 */
int _soc_hr4_mem_skip(int unit, soc_mem_t mem)
{
    int skip = FALSE;

    if (!SOC_IS_HURRICANE4(unit)) {
        return FALSE;
    }

    skip = _soc_flowtracker_mem_skip(unit, mem);
    if (skip == TRUE) {
        return TRUE;
    }

    if (!soc_feature(unit, soc_feature_xflow_macsec)) {
        switch (mem) {
            case ESEC_MIB0m:
            case ESEC_MIB1m:
            case ESEC_SA_TABLEm:
            case ESEC_SC_TABLEm:
            case ISEC_MIB0m:
            case ISEC_MIB0_Bm:
            case ISEC_MIB1m:
            case ISEC_MIB2m:
            case ISEC_SA_TABLEm:
            case ISEC_SC_TABLEm:
            case ISEC_SC_TCAMm:
            case ISEC_SP_TCAMm:
            case SUB_PORT_DATAm:
            case SUB_PORT_POLICY_TABLEm:
            case ISEC_INMGMTPKTS_COUNTm:
            case ISEC_MISC_COUNTERSm:
            case ISEC_SCTCAM_HIT_COUNTm:
            case ISEC_SPTCAM_HIT_COUNTm:
            case ESEC_MISC_COUNTERSm:
            case ESEC_OUTMGMTPKTS_COUNTm:
                return TRUE;
            default:
                break;
        }
    }
    return FALSE;
}

int _soc_hr4_reg_skip(int unit, soc_reg_t reg)
{
    int skip = FALSE;

    if (!SOC_IS_HURRICANE4(unit)) {
        return FALSE;
    }

    skip = _soc_flowtracker_reg_skip(unit, reg);
    if (skip == TRUE) {
        return TRUE;
    }

    if (!soc_feature(unit, soc_feature_xflow_macsec)) {
        switch (reg) {
            case ESEC_AES_CNTRLr:
            case ESEC_AES_STATUSr:
            case ESEC_CONFIGr:
            case ESEC_EGRESS_MTU0_1r:
            case ESEC_EGRESS_MTU2_3r:
            case ESEC_EGRESS_MTU_FOR_MGMT_PKTr:
            case ESEC_FIPS_CONTROLr:
            case ESEC_FIPS_DATA_REG0r:
            case ESEC_FIPS_DATA_REG1r:
            case ESEC_FIPS_DATA_REG2r:
            case ESEC_FIPS_DATA_REG3r:
            case ESEC_FIPS_IV_REG0r:
            case ESEC_FIPS_IV_REG1r:
            case ESEC_FIPS_IV_REG2r:
            case ESEC_FIPS_KEY_REG0r:
            case ESEC_FIPS_KEY_REG1r:
            case ESEC_FIPS_KEY_REG2r:
            case ESEC_FIPS_KEY_REG3r:
            case ESEC_FIPS_KEY_REG4r:
            case ESEC_FIPS_KEY_REG5r:
            case ESEC_FIPS_KEY_REG6r:
            case ESEC_FIPS_KEY_REG7r:
            case ESEC_FIPS_STATUSr:
            case ESEC_INTR_ENr:
            case ESEC_OLP_RX_ETYPEr:
            case ESEC_OLP_RX_MAC_DA0r:
            case ESEC_OLP_RX_MAC_DA1r:
            case ESEC_OLP_RX_MAC_SA0r:
            case ESEC_OLP_RX_MAC_SA1r:
            case ESEC_OLP_RX_VLANr:
            case ESEC_OVR_IV_REG0r:
            case ESEC_OVR_IV_REG1r:
            case ESEC_OVR_IV_REG2r:
            case ESEC_OVR_KEY_REG0r:
            case ESEC_OVR_KEY_REG1r:
            case ESEC_OVR_KEY_REG2r:
            case ESEC_OVR_KEY_REG3r:
            case ESEC_OVR_KEY_REG4r:
            case ESEC_OVR_KEY_REG5r:
            case ESEC_OVR_KEY_REG6r:
            case ESEC_OVR_KEY_REG7r:
            case ESEC_PN_THDr:
            case ESEC_XPN_THD0r:
            case ESEC_XPN_THD1r:
            case ISEC_AES_CNTRLr:
            case ISEC_CNTRLr:
            case ISEC_CPU_MAC_ADDR0r:
            case ISEC_CPU_MAC_ADDR1r:
            case ISEC_ETYPE_CONFIGr:
            case ISEC_ETYPE_MAX_LENr:
            case ISEC_FIPS_CONTROLr:
            case ISEC_FIPS_DATA_REG0r:
            case ISEC_FIPS_DATA_REG1r:
            case ISEC_FIPS_DATA_REG2r:
            case ISEC_FIPS_DATA_REG3r:
            case ISEC_FIPS_IV_REG0r:
            case ISEC_FIPS_IV_REG1r:
            case ISEC_FIPS_IV_REG2r:
            case ISEC_FIPS_KEY_REG0r:
            case ISEC_FIPS_KEY_REG1r:
            case ISEC_FIPS_KEY_REG2r:
            case ISEC_FIPS_KEY_REG3r:
            case ISEC_FIPS_KEY_REG4r:
            case ISEC_FIPS_KEY_REG5r:
            case ISEC_FIPS_KEY_REG6r:
            case ISEC_FIPS_KEY_REG7r:
            case ISEC_FRMVLDT_RULE_CFGr:
            case ISEC_FRM_VLDT_RULEr:
            case ISEC_HR4_GLB_CTRLr:
            case ISEC_INTR_ENr:
            case ISEC_MGMTRULE_CFG_DA_0_0r:
            case ISEC_MGMTRULE_CFG_DA_0_1r:
            case ISEC_MGMTRULE_CFG_DA_1_0r:
            case ISEC_MGMTRULE_CFG_DA_1_1r:
            case ISEC_MGMTRULE_CFG_DA_2_0r:
            case ISEC_MGMTRULE_CFG_DA_2_1r:
            case ISEC_MGMTRULE_CFG_DA_3_0r:
            case ISEC_MGMTRULE_CFG_DA_3_1r:
            case ISEC_MGMTRULE_CFG_DA_4_0r:
            case ISEC_MGMTRULE_CFG_DA_4_1r:
            case ISEC_MGMTRULE_CFG_DA_5_0r:
            case ISEC_MGMTRULE_CFG_DA_5_1r:
            case ISEC_MGMTRULE_CFG_DA_6_0r:
            case ISEC_MGMTRULE_CFG_DA_6_1r:
            case ISEC_MGMTRULE_CFG_DA_7_0r:
            case ISEC_MGMTRULE_CFG_DA_7_1r:
            case ISEC_MGMTRULE_CFG_ETYPE_0r:
            case ISEC_MGMTRULE_CFG_ETYPE_2r:
            case ISEC_MGMTRULE_CFG_ETYPE_4r:
            case ISEC_MGMTRULE_CFG_ETYPE_6r:
            case ISEC_MGMTRULE_DA_ETYPE_1ST_0r:
            case ISEC_MGMTRULE_DA_ETYPE_1ST_1r:
            case ISEC_MGMTRULE_DA_ETYPE_2ND_0r:
            case ISEC_MGMTRULE_DA_ETYPE_2ND_1r:
            case ISEC_MGMTRULE_DA_RANGE_HIGH_0r:
            case ISEC_MGMTRULE_DA_RANGE_HIGH_1r:
            case ISEC_MGMTRULE_DA_RANGE_LOW_0r:
            case ISEC_MGMTRULE_DA_RANGE_LOW_1r:
            case ISEC_MPLS_ETYPEr:
            case ISEC_MTU0_1r:
            case ISEC_MTU2_3r:
            case ISEC_OLP_TX_ETYPEr:
            case ISEC_OLP_TX_MAC_DA0r:
            case ISEC_OLP_TX_MAC_DA1r:
            case ISEC_OLP_TX_MAC_SA0r:
            case ISEC_OLP_TX_MAC_SA1r:
            case ISEC_OLP_TX_MISCr:
            case ISEC_OLP_TX_TYPEr:
            case ISEC_OLP_TX_VLANr:
            case ISEC_OVR_IV_REG0r:
            case ISEC_OVR_IV_REG1r:
            case ISEC_OVR_IV_REG2r:
            case ISEC_OVR_KEY_REG0r:
            case ISEC_OVR_KEY_REG1r:
            case ISEC_OVR_KEY_REG2r:
            case ISEC_OVR_KEY_REG3r:
            case ISEC_OVR_KEY_REG4r:
            case ISEC_OVR_KEY_REG5r:
            case ISEC_OVR_KEY_REG6r:
            case ISEC_OVR_KEY_REG7r:
            case ISEC_PN_EXPIRE_THDr:
            case ISEC_PP_CNTRLr:
            case ISEC_RUD_MGMT_RULEr:
            case ISEC_RUD_MGMT_SPNUM_RULE0r:
            case ISEC_RUD_MGMT_SPNUM_RULE1r:
            case ISEC_RUD_MGMT_SPNUM_RULE2r:
            case ISEC_RUD_MGMT_SPNUM_RULE3r:
            case ISEC_RUD_MGMT_SPNUM_RULE4r:
            case ISEC_RUD_MGMT_SPNUM_RULE5r:
            case ISEC_RUD_MGMT_SPNUM_RULE6r:
            case ISEC_RUD_MGMT_SPNUM_RULE7r:
            case ISEC_RUD_MGMT_SPNUM_RULE8r:
            case ISEC_RUD_MGMT_SPNUM_RULE9r:
            case ISEC_RUD_MGMT_SPNUM_RULE10r:
            case ISEC_RUD_MGMT_SPNUM_RULE11r:
            case ISEC_RUD_MGMT_SPNUM_RULE12r:
            case ISEC_RUD_MGMT_SPNUM_RULE13r:
            case ISEC_RUD_MGMT_SPNUM_RULE14r:
            case ISEC_RUD_MGMT_SPNUM_RULE15r:
            case ISEC_RUD_MGMT_SPNUM_RULE16r:
            case ISEC_RUD_MGMT_SPNUM_RULE17r:
            case ISEC_RUD_MGMT_SPNUM_RULE18r:
            case ISEC_RUD_MGMT_SPNUM_RULE19r:
            case ISEC_RUD_MGMT_SPNUM_RULE20r:
            case ISEC_RUD_MGMT_SPNUM_RULE21r:
            case ISEC_RUD_MGMT_SPNUM_SPTCAM_MISr:
            case ISEC_TPID0r:
            case ISEC_TPID1r:
            case ISEC_TPID2r:
            case ISEC_TPID3r:
            case ISEC_XPN_EXPIRE_THD0r:
            case ISEC_XPN_EXPIRE_THD1r:
            case SC_CAM_BIST_CONFIGr:
            case SC_CAM_BIST_CONTROLr:
            case SC_CAM_BIST_DBGr:
            case SC_CAM_BIST_MISCr:
            case SP_CAM_BIST_CONFIGr:
            case SP_CAM_BIST_CONTROLr:
            case SP_CAM_BIST_DBGr:
            case SP_CAM_BIST_MISCr:
            case SP_GLB_CTRLr:
            case SP_GLB_INTR_ENr:
            case SP_GLB_MIB_CTRLr:
            case SP_GLB_MISC_CTRLr:
            case SP_GLB_OLP_RX_TYPEr:
            case SP_GLB_PRESCALEr:
            case SP_GLB_PROG_CRCr:
            case SP_GLB_SECTAG_ETYPE0_1r:
            case SP_GLB_SECTAG_ETYPE2_3r:
            case SP_GLB_TIMETICKr:
                return TRUE;
            default:
                break;
        }
    }
    return FALSE;
}

#define HR4_MAX_PORT                 64
#define HR4_PORT_FIELD_BITS          8

int
soc_hr4_sobmh_war_l2_vxlate_entry_update(int unit, int modid, int old_modid)
{
    int rv;
    int port;
    int sgpp;
    int old_sgpp;
    l2x_entry_t l2_ent;
    vlan_xlate_2_single_entry_t vxlt2_ent;

    rv = BCM_E_NONE;
    if (!soc_feature(unit, soc_feature_hr4_a0_sobmh_war)) {
        return rv;
    }

    if (SOC_WARM_BOOT(unit)) {
        return rv;
    }

    sal_memset(&l2_ent, 0, sizeof(l2x_entry_t));
    sal_memset(&vxlt2_ent, 0, sizeof(vlan_xlate_2_single_entry_t));

    for (port = 1; port <= HR4_MAX_PORT ; port++) {

        sgpp = ((modid << HR4_PORT_FIELD_BITS) | port)  & 0xFFFF;

        /* L2X entry to map local port to SGPP */
        soc_L2Xm_field32_set(unit, &l2_ent, BASE_VALIDf, 1);
        soc_L2Xm_field32_set(unit, &l2_ent,
                             KEY_TYPEf, HR4_WAR_L2X_SOBMH_KEY_TYPE);
        soc_L2Xm_field32_set(unit, &l2_ent,
                             SOBMH_FLEX__LOCAL_DEST_PORTf, port);
        soc_L2Xm_field32_set(unit, &l2_ent,
                             DATA_TYPEf, HR4_WAR_L2X_SOBMH_DATA_TYPE);
        soc_L2Xm_field32_set(unit, &l2_ent,
                             SOBMH_FLEX__EDIT_CTRL_IDf, HR4_WAR_L2X_SOBMH_EDIT_CTRL_ID);
        soc_mem_field32_dest_set(unit, L2Xm, &l2_ent,
                                 SOBMH_FLEX__DESTINATIONf,
                                 SOC_MEM_FIF_DEST_DGPP,
                                 sgpp);
        soc_L2Xm_field32_set(unit, &l2_ent, STATIC_BITf, 1);
        rv = soc_mem_insert_return_old(unit, L2Xm, MEM_BLOCK_ANY, &l2_ent, NULL);
        if (BCM_FAILURE(rv) && rv != BCM_E_EXISTS) {
            break;
        }
    }
    if ((port <= HR4_MAX_PORT) && BCM_FAILURE(rv)) {
        return rv;
    }

    /*
     * To support Loopback header enabled SOBMH pkts,
     * add LB port also along with other UP/DOWN link ports.
     */
    port = 70;
    sgpp = ((modid << HR4_PORT_FIELD_BITS) | port)  & 0xFFFF;

    /* L2X entry to map local port to SGPP */
    soc_L2Xm_field32_set(unit, &l2_ent, BASE_VALIDf, 1);
    soc_L2Xm_field32_set(unit, &l2_ent,
                         KEY_TYPEf, HR4_WAR_L2X_SOBMH_KEY_TYPE);
    soc_L2Xm_field32_set(unit, &l2_ent,
                         SOBMH_FLEX__LOCAL_DEST_PORTf, port);
    soc_L2Xm_field32_set(unit, &l2_ent,
                         DATA_TYPEf, HR4_WAR_L2X_SOBMH_DATA_TYPE);
    soc_L2Xm_field32_set(unit, &l2_ent,
                         SOBMH_FLEX__EDIT_CTRL_IDf, HR4_WAR_L2X_SOBMH_EDIT_CTRL_ID);
    soc_mem_field32_dest_set(unit, L2Xm, &l2_ent,
                             SOBMH_FLEX__DESTINATIONf,
                             SOC_MEM_FIF_DEST_DGPP,
                             sgpp);
    soc_L2Xm_field32_set(unit, &l2_ent, STATIC_BITf, 1);
    rv = soc_mem_insert_return_old(unit, L2Xm, MEM_BLOCK_ANY, &l2_ent, NULL);
    if (BCM_FAILURE(rv) && rv != BCM_E_EXISTS) {
        return rv;
    }

    /* Disable vlan checks for CPU SOBMH pkts. */
    port = 0;
    sgpp = ((modid << HR4_PORT_FIELD_BITS) | port)  & 0xFFFF;
    soc_VLAN_XLATE_2_SINGLEm_field32_set(unit, &vxlt2_ent, BASE_VALIDf, 1);
    soc_VLAN_XLATE_2_SINGLEm_field32_set(unit, &vxlt2_ent,
                                         KEY_TYPEf, HR4_WAR_VXLT2_SOBMH_KEY_TYPE);
    soc_VLAN_XLATE_2_SINGLEm_field32_set(unit, &vxlt2_ent,
                                         SOBMH_FLEX__SGPPf, sgpp);
    soc_VLAN_XLATE_2_SINGLEm_field32_set(unit, &vxlt2_ent,
                                         DATA_TYPEf, HR4_WAR_VXLT2_SOBMH_DATA_TYPE);
    soc_VLAN_XLATE_2_SINGLEm_field32_set(unit, &vxlt2_ent,
                                         SOBMH_FLEX__DISABLE_VLAN_CHECKSf, 1);
    rv = soc_mem_insert_return_old(unit, VLAN_XLATE_2_SINGLEm,
                                   MEM_BLOCK_ANY, &vxlt2_ent, NULL);
    if (BCM_FAILURE(rv) && rv != BCM_E_EXISTS) {
        return rv;
    }

    if (old_modid != modid) {
        /* Delete entry with old MODID + CPU SGPP entry */
        old_sgpp = ((old_modid << 8) | port)  & 0xFFFF;
        /* Disable vlan checks for SGPP SOBMH pkts. */
        soc_VLAN_XLATE_2_SINGLEm_field32_set(unit, &vxlt2_ent,
                                             BASE_VALIDf, 1);
        soc_VLAN_XLATE_2_SINGLEm_field32_set(unit, &vxlt2_ent,
                                             KEY_TYPEf,
                                             HR4_WAR_VXLT2_SOBMH_KEY_TYPE);
        soc_VLAN_XLATE_2_SINGLEm_field32_set(unit, &vxlt2_ent,
                                             SOBMH_FLEX__SGPPf, old_sgpp);
        soc_VLAN_XLATE_2_SINGLEm_field32_set(unit, &vxlt2_ent,
                                             DATA_TYPEf,
                                             HR4_WAR_VXLT2_SOBMH_DATA_TYPE);
        soc_VLAN_XLATE_2_SINGLEm_field32_set(unit, &vxlt2_ent,
                                             SOBMH_FLEX__DISABLE_VLAN_CHECKSf, 1);
        rv = soc_mem_generic_delete(unit, VLAN_XLATE_2_SINGLEm, MEM_BLOCK_ANY,
                                    0, &vxlt2_ent, NULL, NULL);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*
 * Hurricane4 chip driver functions.
 * Pls keep at the end of this file for easy access.
 */
soc_functions_t soc_hurricane4_drv_funs = {
    _soc_hurricane4_misc_init,
    _soc_hurricane4_mmu_init,
    soc_trident3_age_timer_get,
    soc_trident3_age_timer_max_get,
    soc_trident3_age_timer_set,
    soc_trident3_tscx_firmware_set,
    _soc_hurricane4_tscx_reg_read,
    _soc_hurricane4_tscx_reg_write,
    soc_hurricane4_bond_info_init,
    NULL,
    _soc_hurricane4_port_is_portctrl,
};
#endif /* BCM_HURRICANE4_SUPPORT */
