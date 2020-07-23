/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        helix5.c
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

#ifdef BCM_FIREBOLT6_SUPPORT

#include <soc/mmu_config.h>
#include <soc/helix5.h>
#include <soc/helix5_tdm.h>
#include <soc/firebolt6.h>
#include <soc/firebolt6_tdm.h>
#include <soc/tomahawk.h>
#include <soc/esw/cancun.h>
#include <soc/soc_ser_log.h>
#include <soc/flexport/firebolt6/firebolt6_flexport.h>
#include <soc/esw/portctrl.h>
#include <soc/scache.h>
#include <soc/bondoptions.h>
#include <soc/esw/flowtracker.h>
#include <shared/pack.h>

#ifdef BCM_UTT_SUPPORT
#include <soc/esw/utt.h>
#endif

int bcmi_otp_pm_max_speed[SOC_MAX_NUM_DEVICES][FIREBOLT6_TDM_PBLKS_PER_DEV];
static uint32 _soc_fb6_temp_mon_mask[SOC_MAX_NUM_DEVICES];
int prio_low = 0;
int prio_high = 1;

#define FB6_MAX_L0_DW_PORT              1
#define FB6_MAX_L0_UP_PORT              4
#define FB6_MAX_L0_ST_PORT              3
#define FB6_MAX_L0_CH_PORT              48
#define FB6_CH_PORT_L0_START            72

#define FB6_CPU_L0_NUM                   461
#define FB6_LB_L0_NUM                    460
#define FB6_CPU_QUEUE_BASE               3690
#define BCMI_FB6_SKU_ELEM_TYPE_PORTMAP    0
#define BCMI_FB6_SKU_ELEM_TYPE_CREDITS    1
#define BCMI_FB6_SKU_ELEM_TYPE_DPR        2

#define FB6_NUM_SUBPORT_PER_LC        48
#define FB6_ENTRIES_PER_CARD                256
static int fb6_e2ecc_map[8];

/* Mutex for hash move logic. */
extern int soc_td3_hash_mutex_init (int unit);

#define IS_MGMT_PORT(unit,port) IS_MANAGEMENT_PORT(unit, port)

#ifdef BCM_WARM_BOOT_SUPPORT
#define SOC_TDM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_TDM_WB_DEFAULT_VERSION            SOC_TDM_WB_VERSION_1_0
#endif

#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
#define FB6_RH_ECMP_FLOWSET_SIZE       65536
#define FB6_RH_ECMP_THRESHOLD          32768
#endif

#define FB6_RH_LAG_FLOWSET_SIZE       65536
#define FB6_RH_LAG_THRESHOLD          32768
int
_soc_fb6_port_speed_cap[SOC_MAX_NUM_DEVICES][FIREBOLT6_PHY_PORTS_PER_DEV];

static const struct {
    int port;
    int phy_port;
    int pipe;
    int idb_port;
    int mmu_port;
} fb6_anc_ports[] = {
    { 0,   0,   0, 70, 70 },  /* cpu port */
    { 70,  69,  0, 71, 71 },  /* loopback port */
    { 71,  70,  0, 69, 69 },  /* FAE port */
};

static const soc_reg_t pvtmon_result_reg[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r,
    TOP_PVTMON_RESULT_2r, TOP_PVTMON_RESULT_3r,
    TOP_PVTMON_RESULT_4r, TOP_PVTMON_RESULT_5r,
    TOP_PVTMON_RESULT_6r, TOP_PVTMON_RESULT_7r
};

typedef struct port_map_s {
    int phy_port;
    int half_pipe;
    int idb_port;
    int mmu_port;
    int is_uplink;
    int is_stacking;
} port_map_t;


egr_vlan_xlate_1_hash_control_entry_t       egr_vlan_1_hash_control;
egr_vlan_xlate_2_hash_control_entry_t       egr_vlan_2_hash_control;
egr_vp_vlan_membership_hash_control_entry_t egr_vp_vlan_mbm_hash_control;
exact_match_hash_control_entry_t            exact_match_hash_control;
ing_vp_vlan_membership_hash_control_entry_t ing_vp_vlan_mbm_hash_control;
l2_entry_hash_control_entry_t               l2_entry_hash_control;
l3_entry_hash_control_entry_t               l3_entry_hash_control;
mpls_entry_hash_control_entry_t             mpls_entry_hash_control;
vlan_xlate_1_hash_control_entry_t           vlan_1_hash_control;
vlan_xlate_2_hash_control_entry_t           vlan_2_hash_control;
uft_shared_banks_control_entry_t            uft_shared_banks_control;
ip_uat_shared_banks_control_entry_t         ip_uat_shared_banks_control;
ep_uat_shared_banks_control_entry_t         ep_uat_shared_banks_control;
ft_key_ss0_shared_banks_control_entry_t     ft_key_ss0_shared_banks_control;
ft_key_ss1_shared_banks_control_entry_t     ft_key_ss1_shared_banks_control;

/* Port mapping which will be derived based on different devices. */
port_map_t port_mapping[FB6_MAX_PORT_MAP][FB6_NUM_PHY_PORT] = {
    /* Mapping A1. */
   {{1, -1, 64, 64, 0, 0},
    {2, -1, 65, 65, 0, 0},
    {3, -1, 66, 66, 0, 0},
    {4, -1, 67, 67, 0, 0},
    {5, 0, 0, 0, 1, 0},
    {6, 0, 1, 1, 1, 0},
    {7, 0, 2, 2, 1, 0},
    {8, 0, 3, 3, 1, 0},
    {9, 1, 32, 32, 1, 0},
    {10, 1, 33, 33, 1, 0},
    {11, 1, 34, 34, 1, 0},
    {12, 1, 35, 35, 1, 0},
    {13, 0, 4, 4, 0, 1},
    {14, 0, 5, 5, 0, 1},
    {15, 0, 6, 6, 0, 1},
    {16, 0, 7, 7, 0, 1},
    {17, 1, 36, 36, 0, 1},
    {18, 1, 37, 37, 0, 1},
    {19, 1, 38, 38, 0, 1},
    {20, 1, 39, 39, 0, 1},
    {21, 0, 8, 8, 0, 0},
    {22, 0, 9, 9, 0, 0},
    {23, 0, 10, 10, 0, 0},
    {24, 0, 11, 11, 0, 0},
    {25, 0, 12, 12, 0, 0},
    {26, 0, 13, 13, 0, 0},
    {27, 0, 14, 14, 0, 0},
    {28, 0, 15, 15, 0, 0},
    {29, 0, 16, 16, 0, 0},
    {30, 0, 17, 17, 0, 0},
    {31, 0, 18, 18, 0, 0},
    {32, 0, 19, 19, 0, 0},
    {33, 0, 20, 20, 0, 0},
    {34, 0, 21, 21, 0, 0},
    {35, 0, 22, 22, 0, 0},
    {36, 0, 23, 23, 0, 0},
    {37, 0, 24, 24, 0, 0},
    {38, 0, 25, 25, 0, 0},
    {39, 0, 26, 26, 0, 0},
    {40, 0, 27, 27, 0, 0},
    {41, 0, 28, 28, 0, 0},
    {42, 0, 29, 29, 0, 0},
    {43, 0, 30, 30, 0, 0},
    {44, 0, 31, 31, 0, 0},
    {45, 1, 40, 40, 0, 0},
    {46, 1, 41, 41, 0, 0},
    {47, 1, 42, 42, 0, 0},
    {48, 1, 43, 43, 0, 0},
    {49, 1, 44, 44, 0, 0},
    {50, 1, 45, 45, 0, 0},
    {51, 1, 46, 46, 0, 0},
    {52, 1, 47, 47, 0, 0},
    {53, 1, 48, 48, 0, 0},
    {54, 1, 49, 49, 0, 0},
    {55, 1, 50, 50, 0, 0},
    {56, 1, 51, 51, 0, 0},
    {57, 1, 52, 52, 0, 0},
    {58, 1, 53, 53, 0, 0},
    {59, 1, 54, 54, 0, 0},
    {60, 1, 55, 55, 0, 0},
    {61, 1, 56, 56, 0, 0},
    {62, 1, 57, 57, 0, 0},
    {63, 1, 58, 58, 0, 0},
    {64, 1, 59, 59, 0, 0},
    {65, 1, 60, 60, 0, 0},
    {66, 1, 61, 61, 0, 0},
    {67, 1, 62, 62, 0, 0},
    {68, 1, 63, 63, 0, 0}},
    /* Mapping A2. */
   {{1, 0, 8, 8, 0, 0},
    {2, 0, 9, 9, 0, 0},
    {3, 0, 10, 10, 0, 0},
    {4, 0, 11, 11, 0, 0},
    {5, 0, 12, 12, 1, 0},
    {6, 0, 13, 13, 1, 0},
    {7, 0, 14, 14, 1, 0},
    {8, 0, 15, 15, 1, 0},
    {9, 0, 16, 16, 1, 0},
    {10, 0, 17, 17, 1, 0},
    {11, 0, 18, 18, 1, 0},
    {12, 0, 19, 19, 1, 0},
    {13, 0, 20, 20, 0, 1},
    {14, 0, 21, 21, 0, 1},
    {15, 0, 22, 22, 0, 1},
    {16, 0, 23, 23, 0, 1},
    {17, 0, 24, 24, 0, 1},
    {18, 0, 25, 25, 0, 1},
    {19, 0, 26, 26, 0, 1},
    {20, 0, 27, 27, 0, 1},
    {21, 0, 28, 28, 0, 0},
    {22, 0, 29, 29, 0, 0},
    {23, 0, 30, 30, 0, 0},
    {24, 0, 31, 31, 0, 0},
    {25, 1, 40, 40, 0, 0},
    {26, 1, 41, 41, 0, 0},
    {27, 1, 42, 42, 0, 0},
    {28, 1, 43, 43, 0, 0},
    {29, 1, 44, 44, 0, 0},
    {30, 1, 45, 45, 0, 0},
    {31, 1, 46, 46, 0, 0},
    {32, 1, 47, 47, 0, 0},
    {33, 1, 48, 48, 0, 0},
    {34, 1, 49, 49, 0, 0},
    {35, 1, 50, 50, 0, 0},
    {36, 1, 51, 51, 0, 0},
    {37, 1, 52, 52, 0, 0},
    {38, 1, 53, 53, 0, 0},
    {39, 1, 54, 54, 0, 0},
    {40, 1, 55, 55, 0, 0},
    {41, 1, 56, 56, 0, 0},
    {42, 1, 57, 57, 0, 0},
    {43, 1, 58, 58, 0, 0},
    {44, 1, 59, 59, 0, 0},
    {45, 1, 60, 60, 0, 0},
    {46, 1, 61, 61, 0, 0},
    {47, 1, 62, 62, 0, 0},
    {48, 1, 63, 63, 0, 0},
    {49, 0, 0, 0, 1, 0},
    {50, 0, 1, 1, 1, 0},
    {51, 0, 2, 2, 1, 0},
    {52, 0, 3, 3, 1, 0},
    {53, 1, 32, 32, 1, 0},
    {54, 1, 33, 33, 1, 0},
    {55, 1, 34, 34, 1, 0},
    {56, 1, 35, 35, 1, 0},
    {57, 0, 4, 4, 0, 1},
    {58, 0, 5, 5, 0, 1},
    {59, 0, 6, 6, 0, 1},
    {60, 0, 7, 7, 0, 1},
    {61, 1, 36, 36, 0, 1},
    {62, 1, 37, 37, 0, 1},
    {63, 1, 38, 38, 0, 1},
    {64, 1, 39, 39, 0, 1},
    {65, -1, 64, 64, 0, 0},
    {66, -1, 65, 65, 0, 0},
    {67, -1, 66, 66, 0, 0},
    {68, -1, 67, 67, 0, 0}},
    /* Mapping A3. */
   {{1, -1, 64, 64, 0, 0},
    {2, -1, 65, 65, 0, 0},
    {3, -1, 66, 66, 0, 0},
    {4, -1, 67, 67, 0, 0},
    {5, 0, 0, 0, 1, 0},
    {6, 0, 1, 1, 1, 0},
    {7, 0, 2, 2, 1, 0},
    {8, 0, 3, 3, 1, 0},
    {9, 1, 32, 32, 1, 0},
    {10, 1, 33, 33, 1, 0},
    {11, 1, 34, 34, 1, 0},
    {12, 1, 35, 35, 1, 0},
    {13, 0, 8, 8, 0, 0},
    {14, 0, 9, 9, 0, 0},
    {15, 0, 10, 10, 0, 0},
    {16, 0, 11, 11, 0, 0},
    {17, 0, 12, 12, 0, 0},
    {18, 0, 13, 13, 0, 0},
    {19, 0, 14, 14, 0, 0},
    {20, 0, 15, 15, 0, 0},
    {21, 0, 16, 16, 0, 0},
    {22, 0, 17, 17, 0, 0},
    {23, 0, 18, 18, 0, 0},
    {24, 0, 19, 19, 0, 0},
    {25, 0, 20, 20, 0, 0},
    {26, 0, 21, 21, 0, 0},
    {27, 0, 22, 22, 0, 0},
    {28, 0, 23, 23, 0, 0},
    {29, 0, 24, 24, 0, 0},
    {30, 0, 25, 25, 0, 0},
    {31, 0, 26, 26, 0, 0},
    {32, 0, 27, 27, 0, 0},
    {33, 0, 28, 28, 0, 0},
    {34, 0, 29, 29, 0, 0},
    {35, 0, 30, 30, 0, 0},
    {36, 0, 31, 31, 0, 0},
    {37, 1, 40, 40, 0, 0},
    {38, 1, 41, 41, 0, 0},
    {39, 1, 42, 42, 0, 0},
    {40, 1, 43, 43, 0, 0},
    {41, 1, 44, 44, 0, 0},
    {42, 1, 45, 45, 0, 0},
    {43, 1, 46, 46, 0, 0},
    {44, 1, 47, 47, 0, 0},
    {45, 1, 48, 48, 0, 0},
    {46, 1, 49, 49, 0, 0},
    {47, 1, 50, 50, 0, 0},
    {48, 1, 51, 51, 0, 0},
    {49, 1, 52, 52, 0, 0},
    {50, 1, 53, 53, 0, 0},
    {51, 1, 54, 54, 0, 0},
    {52, 1, 55, 55, 0, 0},
    {53, 1, 56, 56, 0, 0},
    {54, 1, 57, 57, 0, 0},
    {55, 1, 58, 58, 0, 0},
    {56, 1, 59, 59, 0, 0},
    {57, 1, 60, 60, 0, 0},
    {58, 1, 61, 61, 0, 0},
    {59, 1, 62, 62, 0, 0},
    {60, 1, 63, 63, 0, 0},
    {61, 0, 4, 4, 0, 1},
    {62, 0, 5, 5, 0, 1},
    {63, 0, 6, 6, 0, 1},
    {64, 0, 7, 7, 0, 1},
    {65, 1, 36, 36, 0, 1},
    {66, 1, 37, 37, 0, 1},
    {67, 1, 38, 38, 0, 1},
    {68, 1, 39, 39, 0, 1}},
    /* Mapping B1. */
   {{1, -1, 64, 64, 0, 0},
    {2, -1, 65, 65, 0, 0},
    {3, -1, 66, 66, 0, 0},
    {4, -1, 67, 67, 0, 0},
    {5, 0, 8, 8, 0, 0},
    {6, 0, 9, 9, 0, 0},
    {7, 0, 10, 10, 0, 0},
    {8, 0, 11, 11, 0, 0},
    {9, 0, 12, 12, 0, 0},
    {10, 0, 13, 13, 0, 0},
    {11, 0, 14, 14, 0, 0},
    {12, 0, 15, 15, 0, 0},
    {13, 0, 16, 16, 0, 0},
    {14, 0, 17, 17, 0, 0},
    {15, 0, 18, 18, 0, 0},
    {16, 0, 19, 19, 0, 0},
    {17, 0, 20, 20, 0, 0},
    {18, 0, 21, 21, 0, 0},
    {19, 0, 22, 22, 0, 0},
    {20, 0, 23, 23, 0, 0},
    {21, 0, 24, 24, 0, 0},
    {22, 0, 25, 25, 0, 0},
    {23, 0, 26, 26, 0, 0},
    {24, 0, 27, 27, 0, 0},
    {25, 0, 28, 28, 0, 0},
    {26, 0, 29, 29, 0, 0},
    {27, 0, 30, 30, 0, 0},
    {28, 0, 31, 31, 0, 0},
    {29, 1, 40, 40, 0, 0},
    {30, 1, 41, 41, 0, 0},
    {31, 1, 42, 42, 0, 0},
    {32, 1, 43, 43, 0, 0},
    {33, 1, 44, 44, 0, 0},
    {34, 1, 45, 45, 0, 0},
    {35, 1, 46, 46, 0, 0},
    {36, 1, 47, 47, 0, 0},
    {37, 1, 48, 48, 0, 0},
    {38, 1, 49, 49, 0, 0},
    {39, 1, 50, 50, 0, 0},
    {40, 1, 51, 51, 0, 0},
    {41, 1, 52, 52, 0, 0},
    {42, 1, 53, 53, 0, 0},
    {43, 1, 54, 54, 0, 0},
    {44, 1, 55, 55, 0, 0},
    {45, 1, 56, 56, 0, 0},
    {46, 1, 57, 57, 0, 0},
    {47, 1, 58, 58, 0, 0},
    {48, 1, 59, 59, 0, 0},
    {49, 1, 60, 60, 0, 0},
    {50, 1, 61, 61, 0, 0},
    {51, 1, 62, 62, 0, 0},
    {52, 1, 63, 63, 0, 0},
    {53, 0, 0, 0, 1, 0},
    {54, 0, 1, 1, 1, 0},
    {55, 0, 2, 2, 1, 0},
    {56, 0, 3, 3, 1, 0},
    {57, 1, 32, 32, 1, 0},
    {58, 1, 33, 33, 1, 0},
    {59, 1, 34, 34, 1, 0},
    {60, 1, 35, 35, 1, 0},
    {61, 0, 4, 4, 0, 1},
    {62, 0, 5, 5, 0, 1},
    {63, 0, 6, 6, 0, 1},
    {64, 0, 7, 7, 0, 1},
    {65, 1, 36, 36, 0, 1},
    {66, 1, 37, 37, 0, 1},
    {67, 1, 38, 38, 0, 1},
    {68, 1, 39, 39, 0, 1}},
    /* Mapping B2. */
   {{1, -1, 64, 64, 0, 0},
    {2, -1, 65, 65, 0, 0},
    {3, -1, 66, 66, 0, 0},
    {4, -1, 67, 67, 0, 0},
    {5, 0, 8, 8, 0, 0},
    {6, 0, 9, 9, 0, 0},
    {7, 0, 10, 10, 0, 0},
    {8, 0, 11, 11, 0, 0},
    {9, 0, 12, 12, 0, 0},
    {10, 0, 13, 13, 0, 0},
    {11, 0, 14, 14, 0, 0},
    {12, 0, 15, 15, 0, 0},
    {13, 0, 16, 16, 0, 0},
    {14, 0, 17, 17, 0, 0},
    {15, 0, 18, 18, 0, 0},
    {16, 0, 19, 19, 0, 0},
    {17, 0, 20, 20, 0, 0},
    {18, 0, 21, 21, 0, 0},
    {19, 0, 22, 22, 0, 0},
    {20, 0, 23, 23, 0, 0},
    {21, 0, 24, 24, 0, 0},
    {22, 0, 25, 25, 0, 0},
    {23, 0, 26, 26, 0, 0},
    {24, 0, 27, 27, 0, 0},
    {25, 0, 28, 28, 0, 0},
    {26, 0, 29, 29, 0, 0},
    {27, 0, 30, 30, 0, 0},
    {28, 0, 31, 31, 0, 0},
    {29, 0, 0, 0, 0, 0},
    {30, 0, 1, 1, 0, 0},
    {31, 0, 2, 2, 0, 0},
    {32, 0, 3, 3, 0, 0},
    {33, 1, 32, 32, 0, 0},
    {34, 1, 33, 33, 0, 0},
    {35, 1, 34, 34, 0, 0},
    {36, 1, 35, 35, 0, 0},
    {37, 0, 4, 4, 0, 0},
    {38, 0, 5, 5, 0, 0},
    {39, 0, 6, 6, 0, 0},
    {40, 0, 7, 7, 0, 0},
    {41, 1, 36, 36, 0, 0},
    {42, 1, 37, 37, 0, 0},
    {43, 1, 38, 38, 0, 0},
    {44, 1, 39, 39, 0, 0},
    {45, 1, 40, 40, 0, 0},
    {46, 1, 41, 41, 0, 0},
    {47, 1, 42, 42, 0, 0},
    {48, 1, 43, 43, 0, 0},
    {49, 1, 44, 44, 0, 0},
    {50, 1, 45, 45, 0, 0},
    {51, 1, 46, 46, 0, 0},
    {52, 1, 47, 47, 0, 0},
    {53, 1, 48, 48, 0, 0},
    {54, 1, 49, 49, 0, 0},
    {55, 1, 50, 50, 0, 0},
    {56, 1, 51, 51, 0, 0},
    {57, 1, 52, 52, 0, 0},
    {58, 1, 53, 53, 0, 0},
    {59, 1, 54, 54, 0, 0},
    {60, 1, 55, 55, 0, 0},
    {61, 1, 56, 56, 0, 0},
    {62, 1, 57, 57, 0, 0},
    {63, 1, 58, 58, 0, 0},
    {64, 1, 59, 59, 0, 0},
    {65, 1, 60, 60, 0, 0},
    {66, 1, 61, 61, 0, 0},
    {67, 1, 62, 62, 0, 0},
    {68, 1, 63, 63, 0, 0}}
};

/*
 * Function:
 *      _soc_fb6_sku_params_get
 * Purpose:
 *      Helper function to get sku based values.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      type    - (IN) Type of value required.
 *      value   - (OUT)  value.
 * Returns:
 *       None.
 */
STATIC void
_soc_fb6_sku_params_get(int unit, int type, int *val)
{
    soc_info_t *si;
    int portmap=0, credits=0, dpr=0;
    si = &SOC_INFO(unit);


    switch (soc_property_get(unit, spn_BCM_TDM_FREQUENCY, si->frequency)) {
        case  FB6_CLK_4730_1012:
            portmap = 0;
            credits = 62;
            dpr = 620;
        break;

        case  FB6_CLK_4731_1012:
            portmap = 0;
            credits = 62;
            dpr = 620;
        break;

        case  FB6_CLK_4732_1012:
            portmap = 0;
            credits = 62;
            dpr = 620;
        break;

        case  FB6_CLK_4720_1250:
            portmap = 0;
            credits = 62;
            dpr = 752;
        break;

        case  FB6_CLK_4721_1250:
            portmap = 0;
            credits = 62;
            dpr = 752;
        break;

        case  FB6_CLK_4700_1250:
            portmap = 1;
            credits = 62;
            dpr = 752;
        break;

        case  FB6_CLK_4701_1250:
            portmap = 0;
            credits = 62;
            dpr = 752;
        break;

        case  FB6_CLK_4702_1250:
            portmap = 0;
            credits = 62;
            dpr = 752;
        break;

        case  FB6_CLK_4703_1250:
            portmap = 0;
            credits = 62;
            dpr = 752;
        break;

        case  FB6_CLK_4704_1087:
            portmap = 1;
            credits = 62;
            dpr = 752;
        break;

        case  FB6_CLK_4705_1250:
            portmap = 3;
            credits = 62;
            dpr = 752;
        break;

        case  FB6_CLK_1000_1012:
            portmap = 0;
            credits = 40;
            dpr = 456;
        break;

        case  FB6_CLK_1001_1250:
            portmap = 0;
            credits = 40;
            dpr = 752;
        break;

       default:
            portmap = 0;
            credits = 62;
            dpr = 752;
       break;
    }

    if (type == BCMI_FB6_SKU_ELEM_TYPE_CREDITS) {
        *val = credits;
    } else if (type == BCMI_FB6_SKU_ELEM_TYPE_PORTMAP) {
        *val = portmap;
    } else if (type == BCMI_FB6_SKU_ELEM_TYPE_DPR) {
        *val = dpr;
    }

}

/*
 * Function:
 *      _soc_fb6_port_speed_validate
 * Purpose:
 *      Validate speed for each port based on OTP params.
 * Parameters:
 *      unit    - (IN) Unit number.
_*      port    - (IN) Logical port.
_*      pport   - (IN) physical port.
 *      lanes   - (IN) port lanes.
 *      encap   - (IN) Port encapsulation.
 *      speed   - (OUT) port speed
 * Returns if validatation ..
 *       Fail - SOC_E_CONFIG
 *       Pass - SOC_E_NONE.
 */
int
_soc_fb6_port_speed_validate(int unit, bcm_port_t port, bcm_port_t pport,
                             int lanes, int encap, int speed)
{
    int phy_port = pport;
    soc_info_t *si;
    int pm_id;
    int max_speed = 0;
    int local_lanes = lanes;
    int serdes_1000x_at_25g_vco, serdes_10g_at_25g_vco;
    int vco_25g=0;
    int is_hg_port = 0;

    if (port < 1) {
        return SOC_E_NONE;
    }
    si = &SOC_INFO(unit);

    if (phy_port == -1) {
       /* Get phy ports from incoming logical port. */
        phy_port = si->port_l2p_mapping[port];
    }

    /* Verify the lanes. */
    if (lanes <= 0) {
        local_lanes = si->port_num_lanes[port];
    }
    /* Check for valid port and speed. */
    if (phy_port < 1) {
        return SOC_E_INTERNAL;
    }

    if (speed <=0 ) {
        return SOC_E_NONE;
    }

    /* 20G speed is not supported. */
    if (speed == 20000) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,
             "ERROR: port (%d) is configured with unsupported (%dG)"
                 " bandwidth \n"), port, (speed/1000)));

        return SOC_E_CONFIG;
    }

    /* If this is HG port then change the speed. */
    is_hg_port = IS_HG_PORT(unit, port);

    /* Check if encap is valid and set it. */
    if (encap != -1) {
        is_hg_port = ((encap == SOC_ENCAP_HIGIG2) ? 1 : 0);
    }

    if (is_hg_port) {
        speed = SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(speed);
    }

    /* each lane of PM is mapped to a phy port sequentially. */
    pm_id = ((phy_port-1) / 4);

    max_speed = bcmi_otp_pm_max_speed[unit][pm_id];
    if (max_speed > 0) {
        if ((local_lanes * max_speed) <  (speed/1000)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: port (%d) is configured with (%dG) bandwidth :"
                 " supported max bandwidth (%dG) \n"), port, (speed/1000),
                 (local_lanes * max_speed)));

            return SOC_E_CONFIG;
        }
    }

    /* Check if sister ports can handle this speed. */
    serdes_1000x_at_25g_vco =
        soc_property_port_get(unit, port,
                      spn_SERDES_1000X_AT_25G_VCO, 1);

    serdes_10g_at_25g_vco =
        soc_property_port_get(unit, port,
                      spn_SERDES_10G_AT_25G_VCO, 1);


    if (serdes_10g_at_25g_vco || serdes_1000x_at_25g_vco) {
        vco_25g = 1;
    }

    if (vco_25g) {
        /* 25g vco. */
       if (speed == 5000) {
           return SOC_E_CONFIG;
        }
    } else {
        /* 20G vco. */
        if ((speed == 2500) ||
            (speed == 25000) ||
            (speed == 50000) ||
            (speed == 100000)) {
            return SOC_E_CONFIG;
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_fb6_portmap_update
 * Purpose:
 *      update mmu/idb to phy port mapping based on user configuration
 *      for pseudo-LR ports.
 *      The PM corresponding to first pseudo-LR port given in
 *      BCM config file will be considered as pseudo-LR PM.
 *      All the ports of that PM will be pseudo-LR.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      portmap - (IN) Port mapping number
 * Returns:
 *       BCM_E_XXX.
 */
int
_soc_fb6_portmap_update(int unit, int portmap)
{

    int m_idx=0, mmu_lr=0, mmu_user=0, phy_port=0;
    int pseudo_lr_mmu_ports[4] = {64,65,66,67};
    int pseudo_lr_phy_ports[4] = {0};
    int pseudo_lr_phy_ports_user = 0;
    int pm_id = 0;

    /*
     * idm/mmu ports 64-67 are always pseudo-LR ports.
     * Get phy ports for these ports and update them with pseudo-LR ports.
     */
        /* Assign idb port and handle mgmt port*/
    for (phy_port = 1; phy_port <= FB6_NUM_PHY_PORT; phy_port++) {

        /*
         * Array index starts from one less than phy port.
         * Get the MMU LR ports number based on portmap
         */
        mmu_lr = port_mapping[portmap][phy_port-1].mmu_port;
        for (m_idx=0; m_idx<4; m_idx++) {
            if (mmu_lr == pseudo_lr_mmu_ports[m_idx]) {
                /* This is pseudo LR mmu port. note its physical port. */
                pseudo_lr_phy_ports[m_idx] = phy_port;
            }

        }
        /*
         * Get the user's first LR phy port number.
         * use that PM to swap with pseudo LR MMU ports..
         */
        if (!(soc_fb6_port_oversub_get(unit, phy_port, 0/*dont care */))) {
            if (pseudo_lr_phy_ports_user == 0) {
                pseudo_lr_phy_ports_user = phy_port;
            }
        }

    }

    /*
     * Pick up the first port of pseudo LR PM given by user.
     */
    if (pseudo_lr_phy_ports_user == 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Config Error - Atleast one PM should be"
                                    " in pseudo LR ports. \n")));

        return BCM_E_CONFIG;
    }

    /*
     * phy ports starts from 1.
     * phy port 68 will be pm id 17.
     * For 17th pm, phy ports are 65, 66, 67, 68
     */
    pm_id = (pseudo_lr_phy_ports_user - 1)/4;

    /*
     * phy ports are with offset 1.
     * e.g. for PM0, phyports are 1,2,3,4 and so on.
     */
    pseudo_lr_phy_ports_user = (pm_id * 4) + 1;

    /* Swap the mmu ports if needed. */
    for (m_idx=0; m_idx<4; m_idx++) {
        mmu_user = port_mapping[portmap][pseudo_lr_phy_ports_user+m_idx-1].mmu_port;
        mmu_lr   = port_mapping[portmap][pseudo_lr_phy_ports[m_idx]-1].mmu_port;

        if (mmu_user == mmu_lr) {
            continue;
        }
        /* Swap physical to mmu port mapping for Pseudo-LR ports.*/
        port_mapping[portmap][pseudo_lr_phy_ports_user + m_idx-1].mmu_port = mmu_lr;
        port_mapping[portmap][pseudo_lr_phy_ports[m_idx]-1].mmu_port      = mmu_user;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _soc_fb6_mmu_idb_ports_assign
 * Purpose:
 *      Assign mmu/idb ports based on portmap.
 * Parameters:
 *      unit    - (IN) Unit number.
 * Returns:
 *       BCM_E_XXX.
 */
int
_soc_fb6_mmu_idb_ports_assign(int unit)
{
    int port, phy_port, mmu_port;
    int portmap = 0;
    soc_info_t *si;

    si = &SOC_INFO(unit);


    _soc_fb6_sku_params_get(unit, BCMI_FB6_SKU_ELEM_TYPE_PORTMAP, &portmap);

    SOC_IF_ERROR_RETURN(_soc_fb6_portmap_update(unit, portmap));

    /* Assign idb port and handle mgmt port*/
    for (phy_port = 1; phy_port <= FB6_NUM_PHY_PORT; phy_port++) {

        /* array index starts from one less than phy port. */
        mmu_port = port_mapping[portmap][phy_port-1].mmu_port;
        si->port_p2m_mapping[phy_port] = mmu_port;
        si->port_m2p_mapping[mmu_port] = phy_port;
        port = si->port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        }

        si->port_l2i_mapping[port] = mmu_port;
        si->port_half_pipe[port] = port_mapping[portmap][phy_port-1].half_pipe;

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _soc_fb6_uplink_stk_ports_assign
 * Purpose:
 *      Assign user configuired uplink/stacking ports..
 * Parameters:
 *      unit    - (IN) Unit number.
 * Returns:
 *       NONE.
 */
STATIC void
_soc_fb6_uplink_stk_ports_assign(int unit)
{
    int enable = 0;
    int phy_port, port;
    soc_info_t *si;

    /* Get the soc control info. */
    si = &SOC_INFO(unit);


    /* Iterate through all the phy ports. */
    for (phy_port=1; phy_port <= FB6_NUM_PHY_PORT; phy_port++) {


        port = si->port_p2l_mapping[phy_port];

        if (port == -1) {
            continue;
        }


        enable = soc_property_phys_port_get(unit, phy_port,
                                        spn_PORT_UPLINK, 0);
        if (enable) {
            SOC_PBMP_PORT_ADD(si->uplink_pbm, port);
        }

        enable = soc_property_phys_port_get(unit, phy_port,
                                        spn_PORT_STACKING, 0);
        if (enable) {
            SOC_PBMP_PORT_ADD(si->stacking_pbm, port);
        }

    }
}


STATIC int
_soc_firebolt6_ft_bank_config(int unit)
{
    soc_mem_t mem;
    int ft_banks;
    ft_key_hash_control_entry_t ft_key_ctrl;
    uint32 bank_len;
    ft_key_ss0_shared_banks_control_entry_t     ft_key_ss0_shared_banks_control;
    ft_key_ss1_shared_banks_control_entry_t     ft_key_ss1_shared_banks_control;


    sal_memset(&ft_key_ss0_shared_banks_control, 0,
        sizeof(ft_key_ss0_shared_banks_control_entry_t));

    sal_memset(&ft_key_ss1_shared_banks_control, 0,
        sizeof(ft_key_ss1_shared_banks_control_entry_t));


    if (!soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        return SOC_E_NONE;
    }

    if (soc_feature(unit, soc_feature_flowtracker_ver_1_8K_ft_table)) {
        ft_banks = 0x7;
    } else if (soc_feature(unit, soc_feature_flowtracker_ver_1_16K_ft_table)) {
        ft_banks = 0x1f;
    } else {
        bank_len = soc_mem_field_length(unit, FT_KEY_HASH_CONTROLm, HASH_TABLE_BANK_CONFIGf);
        ft_banks = ( 1 << bank_len) - 1;
        if (SOC_IS_FIREBOLT6(unit)) {
            ft_banks = 0XFF;
        }
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
    BCM_IF_ERROR_RETURN(WRITE_FT_KEY_SS0_SHARED_BANKS_CONTROLm(unit,
                MEM_BLOCK_ANY, 0, (void *) &ft_key_ss0_shared_banks_control));
    mem = FT_KEY_SS1_SHARED_BANKS_CONTROLm;
    soc_mem_field32_set(unit, mem,
            (void *) &ft_key_ss1_shared_banks_control, B0_HASH_OFFSETf, 6);
    soc_mem_field32_set(unit, mem,
            (void *) &ft_key_ss1_shared_banks_control, B1_HASH_OFFSETf, 11);
    soc_mem_field32_set(unit, mem,
            (void *) &ft_key_ss1_shared_banks_control, B2_HASH_OFFSETf, 17);
    soc_mem_field32_set(unit, mem,
            (void *) &ft_key_ss1_shared_banks_control, B3_HASH_OFFSETf, 23);
    BCM_IF_ERROR_RETURN(WRITE_FT_KEY_SS1_SHARED_BANKS_CONTROLm(unit,
                MEM_BLOCK_ANY, 0, (void *) &ft_key_ss1_shared_banks_control));

    return SOC_E_NONE;
}

#ifdef BCM_UTT_SUPPORT
int
soc_fb6_utt_init(int u)
{
    soc_utt_agent_t aid;

    SOC_UTT_TCAM_DEPTH(u) = FB6_UTT_TCAM_DEPTH;
    SOC_UTT_MAX_TCAMS(u)  = FB6_UTT_MAX_TCAMS;
    SOC_UTT_MAX_AGENTS(u) = 2;

    /* IFP */
    aid = SOC_UTT_AGENT_IFP;
    SOC_UTT_AGENT_MIN_TCAMS(u, aid)            = FB6_UTT_IFP_MIN_TCAMS;
    SOC_UTT_AGENT_MAX_TCAMS(u, aid)            = FB6_UTT_IFP_MAX_TCAMS;
    SOC_UTT_AGENT_MAX_LOOKUPS(u, aid)          = 18;
    SOC_UTT_AGENT_HW_ID(u, aid)                = FB6_UTT_IFP_ID;
    SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(u, aid) = FB6_UTT_IFP_MAX_TCAMS_PER_LKUP;
    SOC_UTT_AGENT_DEF_NUM_LOOKUPS(u, aid)      = 18;
    SOC_UTT_AGENT_DEF_TABLE_SIZE(u, aid)       = FB6_UTT_IFP_MAX_TCAMS * FB6_UTT_TCAM_DEPTH;
    SOC_UTT_AGENT_LOOKUP_BASE(u, aid)          = FB6_UTT_IFP_LKUP_BASE;

    /* LPM */
    aid = SOC_UTT_AGENT_LPM;
    SOC_UTT_AGENT_FLAGS(u, aid)                = SOC_UTT_LPM_VARIABLE_DATA_ADDRESS;
    SOC_UTT_AGENT_MIN_TCAMS(u, aid)            = FB6_UTT_LPM_MIN_TCAMS;
    SOC_UTT_AGENT_MAX_TCAMS(u, aid)            = FB6_UTT_LPM_MAX_TCAMS;
    SOC_UTT_AGENT_MAX_LOOKUPS(u, aid)          = 4;
    SOC_UTT_AGENT_HW_ID(u, aid)                = FB6_UTT_LPM_ID;
    SOC_UTT_AGENT_MAX_TCAMS_PER_LOOKUP(u, aid) = FB6_UTT_LPM_MAX_TCAMS_PER_LKUP;
    SOC_UTT_AGENT_DEF_NUM_LOOKUPS(u, aid)      = 4;
    SOC_UTT_AGENT_DEF_TABLE_SIZE(u, aid)       = FB6_UTT_LPM_MIN_TCAMS * FB6_UTT_TCAM_DEPTH;
    SOC_UTT_AGENT_LOOKUP_BASE(u, aid)          = FB6_UTT_LPM_LKUP_BASE;

    return SOC_E_NONE;
}
#endif /* BCM_UTT_SUPPORT */

/*
 * Function:
 *      soc_firebolt6_mem_config
 * Purpose:
 *      Configure depth of UFT/UAT memories
 * Parameters:
 *      unit (IN)    - switch unit
 * Returns:
 *      SOC_E_*
 */
int
soc_firebolt6_mem_config(int unit)
{
    soc_persist_t *sop;
    int l2_entries, fixed_l2_entries, shared_l2_banks;
    int num_ipv6_128b_entries = 0;
    int config_v6_entries = 0;
    int defip_config = 1;
    int l3_entries, fixed_l3_entries, shared_l3_banks;
    int fpem_entries, shared_fpem_banks;
    int mpls_entries, vlan_xlate_1_entries, vlan_xlate_2_entries;
    int alpm_enable = 0;
    int mpls_uat_banks;
    int vlan_xlate_1_uat_banks;
    int vlan_xlate_2_uat_banks;
    int egr_vlan_xlate_1_entries, egr_vlan_xlate_2_entries;
    int egr_vlan_xlate_1_banks, egr_vlan_xlate_2_banks;
    int num_lag_rh_flowset_entries;
    int ft_entries = 0;

    sop = SOC_PERSIST(unit);

    /*  UFT BANK SIZING
     *
     * bank 0-1 are dedicated L2 banks (8k entries per bank)
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

    l3_entries = soc_property_get(unit, spn_L3_MEM_ENTRIES, 32 * 1024);
    fixed_l3_entries = 32 * 1024;  /* 32k dedicated L3 entries, (16k * 2 banks) */
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
    /* These are double wide entries so make it half of bank width.*/
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
    _soc_aacl_en[unit] = soc_property_get(unit, spn_ALPM_PRE_FILTER, 0x0);

    /* IUAT BANK SIZING
     *
	 * Banks 0-7 are shared banks of size 8k per bank
     */

    mpls_entries = soc_property_get(unit, spn_MPLS_MEM_ENTRIES, 16384);
    mpls_uat_banks = (mpls_entries + (8 * 1024 -1)) / 8192;
    mpls_entries = mpls_uat_banks * (8 * 1024);

    vlan_xlate_1_entries = soc_property_get(unit,
                              spn_VLAN_XLATE_1_MEM_ENTRIES, 16384);
    vlan_xlate_1_uat_banks = (vlan_xlate_1_entries + ((8 * 1024) -1)) / 8192;
    vlan_xlate_1_entries = vlan_xlate_1_uat_banks * (8 * 1024);

    vlan_xlate_2_entries = soc_property_get(unit,
                              spn_VLAN_XLATE_2_MEM_ENTRIES, 16384);
    vlan_xlate_2_uat_banks = (vlan_xlate_2_entries + ((8 * 1024) -1)) / 8192;
    vlan_xlate_2_entries = vlan_xlate_2_uat_banks * (8 * 1024);

    if (mpls_uat_banks + vlan_xlate_1_uat_banks + vlan_xlate_2_uat_banks > 8) {
        return SOC_E_PARAM;
    }

    /* EUAT BANK SIZING
     *
     * Banks 0-3 are shared banks of size 8k per bank
     */
    egr_vlan_xlate_1_entries = soc_property_get(unit,
                                   spn_EGR_VLAN_XLATE_1_MEM_ENTRIES, (16 * 1024));
    egr_vlan_xlate_1_banks = (egr_vlan_xlate_1_entries + (8 * 1024 - 1)) /
                                                                    (8 * 1024);
    egr_vlan_xlate_1_entries = egr_vlan_xlate_1_banks * (8 * 1024);

    egr_vlan_xlate_2_entries = soc_property_get(unit,
                                   spn_EGR_VLAN_XLATE_2_MEM_ENTRIES, (16 * 1024));
    egr_vlan_xlate_2_banks = (egr_vlan_xlate_2_entries + (8 * 1024 - 1)) /
                                                                    (8 * 1024);
    egr_vlan_xlate_2_entries = egr_vlan_xlate_2_banks * (8 * 1024);

    if (egr_vlan_xlate_1_banks + egr_vlan_xlate_2_banks > 4) {
        return SOC_E_PARAM;
    }

    /* LP memory sizing per 8k entries
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
    /* 8k per dedicated L2 bank, 14k per shared L2 bank */
    sop->memState[L2_ENTRY_LPm].index_max = ((4 * 2 * 1024) +
                                             (shared_l2_banks * 8 * 1024)) - 1;

    /* Adjust L3 table size */
    sop->memState[L3_ENTRY_SINGLEm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_ONLY_SINGLEm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_ECCm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_DOUBLEm].index_max = l3_entries / 2 - 1;
    sop->memState[L3_ENTRY_ONLY_DOUBLEm].index_max = l3_entries / 2 - 1;
    sop->memState[L3_ENTRY_QUADm].index_max = l3_entries / 4 - 1;
    sop->memState[L3_ENTRY_ONLY_QUADm].index_max = l3_entries / 4 - 1;
    sop->memState[L3_ENTRY_HIT_ONLYm].index_max = l3_entries / 4 - 1;
    /* 7k per dedicated L3 bank, 14k per shared L2 bank */
    sop->memState[L3_ENTRY_LPm].index_max = ((7 * 2* 1024) +
                                            (shared_l3_banks * 14 * 1024)) - 1;

    /* Adjust FP exact match table size */
    sop->memState[EXACT_MATCH_2m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_PIPE0m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLYm].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLY_PIPE0m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_4m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_PIPE0m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLYm].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLY_PIPE0m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_HIT_ONLYm].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_ECCm].index_max = fpem_entries * 2 - 1;
    sop->memState[EXACT_MATCH_ECC_PIPE0m].index_max = fpem_entries * 2 - 1;
    /* 6k per shared FPEM bank */
    sop->memState[EXACT_MATCH_LPm].index_max = ((shared_fpem_banks * 6 * 1024) - 1);
    sop->memState[EXACT_MATCH_LP_PIPE0m].index_max = ((shared_fpem_banks * 6 * 1024) - 1);

    /* Adjust MPLS table size */
    sop->memState[MPLS_ENTRY_SINGLEm].index_max = mpls_entries - 1;
    sop->memState[MPLS_ENTRYm].index_max = mpls_entries / 2 - 1;
    /* 1.5k per IUAT bank */
    sop->memState[MPLS_ENTRY_LPm].index_max = (((mpls_uat_banks * 1536) - 1));
    sop->memState[MPLS_ENTRY_ECCm].index_max = mpls_entries - 1;

    /* Adjust VLAN_XLATE_1 table size */
    sop->memState[VLAN_XLATE_1_SINGLEm].index_max = vlan_xlate_1_entries - 1;
    sop->memState[VLAN_XLATE_1_DOUBLEm].index_max = vlan_xlate_1_entries / 2 - 1;
    /* 1.5k per IUAT bank */
    sop->memState[VLAN_XLATE_1_LPm].index_max = (vlan_xlate_1_uat_banks * 1536) - 1;
    sop->memState[VLAN_XLATE_1_ECCm].index_max = vlan_xlate_1_entries - 1;

    /* Adjust VLAN_XLATE_2 table size */
    sop->memState[VLAN_XLATE_2_SINGLEm].index_max = vlan_xlate_2_entries - 1;
    sop->memState[VLAN_XLATE_2_DOUBLEm].index_max = vlan_xlate_2_entries / 2 - 1;
    /* 1.5k per IUAT bank */
    sop->memState[VLAN_XLATE_2_LPm].index_max = (vlan_xlate_2_uat_banks * 1536) - 1;
    sop->memState[VLAN_XLATE_2_ECCm].index_max = vlan_xlate_2_entries - 1;

    /* Adjust EGR_VLAN_XLATE_1 table size */
    sop->memState[EGR_VLAN_XLATE_1_SINGLEm].index_max = egr_vlan_xlate_1_entries - 1;
    sop->memState[EGR_VLAN_XLATE_1_DOUBLEm].index_max = egr_vlan_xlate_1_entries / 2 - 1;
    /* 1.5k per EUAT bank */
    sop->memState[EGR_VLAN_XLATE_1_LPm].index_max = (egr_vlan_xlate_1_banks * 1536) - 1;
    sop->memState[EGR_VLAN_XLATE_1_ECCm].index_max = egr_vlan_xlate_1_entries - 1;

    /* Adjust EGR_VLAN_XLATE_2 table size */
    sop->memState[EGR_VLAN_XLATE_2_SINGLEm].index_max = egr_vlan_xlate_2_entries - 1;
    sop->memState[EGR_VLAN_XLATE_2_DOUBLEm].index_max = egr_vlan_xlate_2_entries / 2 - 1;
    /* 1.5k per EUAT bank */
    sop->memState[EGR_VLAN_XLATE_2_LPm].index_max = (egr_vlan_xlate_2_banks * 1536) - 1;
    sop->memState[EGR_VLAN_XLATE_2_ECCm].index_max = egr_vlan_xlate_2_entries - 1;

#ifdef BCM_UTT_SUPPORT
    SOC_IF_ERROR_RETURN(soc_utt_init(unit));
#endif

    
    if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
        if (!soc_feature(unit, soc_feature_utt)) {

            SOC_CONTROL(unit)->l3_defip_max_tcams = 8;
            SOC_CONTROL(unit)->l3_defip_tcam_size = 4096;

            defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);
            num_ipv6_128b_entries = soc_property_get(unit,
                                        spn_NUM_IPV6_LPM_128B_ENTRIES,
                                        (defip_config ? 8192 : 0));
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

            sop->memState[L3_DEFIP_AUX_TABLEm].index_max =
                                          sop->memState[L3_DEFIPm].index_max;
            sop->memState[L3_DEFIP_DATA_ONLYm].index_max =
                                          sop->memState[L3_DEFIPm].index_max;
            sop->memState[L3_DEFIP_HIT_ONLYm].index_max =
                                          sop->memState[L3_DEFIPm].index_max;
            SOC_CONTROL(unit)->l3_defip_index_remap = num_ipv6_128b_entries;
            soc_l3_defip_indexes_init(unit, config_v6_entries);
        }
    }

    /* Adjust ALPM table size */
    if (alpm_enable) {
        /* The check below is sufficient because if ALPM mode is enabled
         * and number of shared banks used is > 4 we return SOC_E_PARAM
         * in code at the top of this function.
         */
        if (shared_l2_banks + shared_l3_banks + shared_fpem_banks > 0) {
            sop->memState[L3_DEFIP_ALPM_RAWm].index_max =
                 sop->memState[L3_DEFIP_ALPM_RAWm].index_max / 2;
            sop->memState[L3_DEFIP_ALPM_IPV4m].index_max =
                 sop->memState[L3_DEFIP_ALPM_IPV4m].index_max / 2;
            sop->memState[L3_DEFIP_ALPM_IPV4_1m].index_max =
                 sop->memState[L3_DEFIP_ALPM_IPV4_1m].index_max / 2;
            sop->memState[L3_DEFIP_ALPM_IPV6_64m].index_max =
                 sop->memState[L3_DEFIP_ALPM_IPV6_64m].index_max / 2;
            sop->memState[L3_DEFIP_ALPM_IPV6_64_1m].index_max =
                 sop->memState[L3_DEFIP_ALPM_IPV6_64_1m].index_max / 2;
            sop->memState[L3_DEFIP_ALPM_IPV6_128m].index_max =
                 sop->memState[L3_DEFIP_ALPM_IPV6_128m].index_max / 2;
            sop->memState[L3_DEFIP_ALPM_HIT_ONLYm].index_max =
                 sop->memState[L3_DEFIP_ALPM_HIT_ONLYm].index_max / 2;
            sop->memState[L3_DEFIP_ALPM_ECCm].index_max =
                 sop->memState[L3_DEFIP_ALPM_ECCm].index_max / 2;
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

    SOC_IF_ERROR_RETURN(soc_trident3_init_alpm2_memory(unit));

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
            break;
        case 65536:
            sop->memState[RH_HGT_FLOWSETm].index_max = -1;
            sop->memState[RH_HGT_FLOWSET_PIPE0m].index_max = -1;
            break;
        case 32768:
            sop->memState[RH_HGT_FLOWSETm].index_max = 32767;
            sop->memState[RH_HGT_FLOWSET_PIPE0m].index_max = 32767;
            sop->memState[RH_LAG_FLOWSETm].index_max = 32767;
            sop->memState[RH_LAG_FLOWSET_PIPE0m].index_max = 32767;
            break;
        default:
            return SOC_E_CONFIG;
    }

    /* Adjust the Flow-tracker session table size based on the
       supported size, these features would have been set based
       on the OTP values. Table has 8 banks with 32K entries each.
     */
    ft_entries = 0;
    if (soc_feature(unit, soc_feature_flowtracker_3banks_ft_table)) {
        ft_entries = 3 * 32 * 1024;
    } else if (soc_feature(unit, soc_feature_flowtracker_5banks_ft_table)) {
        ft_entries = 5 * 32 * 1024;
    }
    if (ft_entries) {
        sop->memState[FT_KEY_SINGLEm].index_max = ft_entries - 1;
        sop->memState[FT_KEY_ECCm].index_max = ft_entries - 1;
        sop->memState[FT_KEY_DOUBLEm].index_max = ft_entries/2 - 1;
        sop->memState[FT_KEY_DOUBLE_PIPE0m].index_max = ft_entries/2 - 1;
        sop->memState[BSC_DT_FLEX_SESSION_DATA_SINGLEm].index_max = ft_entries - 1;
        sop->memState[BSC_DT_FLEX_SESSION_DATA_DOUBLEm].index_max = ft_entries/2 - 1;
    }

    /* Adjust the Flow-tracker group table size based on the
       supported size, these features would have been set based
       on the OTP values.
     */
    if (soc_feature(unit, soc_feature_flowtracker_half_entries_ft_group)) {
        sop->memState[BSC_KG_GROUP_TABLEm].index_max = 511;
        sop->memState[BSC_DG_GROUP_TABLEm].index_max =
            sop->memState[BSC_KG_GROUP_TABLEm].index_max;
        sop->memState[BSC_KG_FLOW_ACTIVE_COUNTERm].index_max =
            sop->memState[BSC_KG_GROUP_TABLEm].index_max;
        sop->memState[BSC_KG_FLOW_LEARNED_COUNTERm].index_max =
            sop->memState[BSC_KG_GROUP_TABLEm].index_max;
        sop->memState[BSC_KG_FLOW_AGE_OUT_COUNTERm].index_max =
            sop->memState[BSC_KG_GROUP_TABLEm].index_max;
        sop->memState[BSC_KG_FLOW_EXCEED_COUNTERm].index_max =
            sop->memState[BSC_KG_GROUP_TABLEm].index_max;
        sop->memState[BSC_KG_FLOW_MISSED_COUNTERm].index_max =
            sop->memState[BSC_KG_GROUP_TABLEm].index_max;
        sop->memState[BSC_DG_FLOW_OPAQUE_COUNTER_0m].index_max =
            sop->memState[BSC_KG_GROUP_TABLEm].index_max;
        sop->memState[BSC_DG_FLOW_OPAQUE_COUNTER_1m].index_max =
            sop->memState[BSC_KG_GROUP_TABLEm].index_max;
        sop->memState[BSC_DG_FLOW_METER_EXCEED_COUNTERm].index_max =
            sop->memState[BSC_KG_GROUP_TABLEm].index_max;
    }

    return SOC_E_NONE;
}

int
soc_fb6_port_cosq_get(int unit, int index, bcm_gport_t *gport, bcm_cos_queue_t *cosq, int mc)
{
    int port = 0;
    soc_info_t *si = &SOC_INFO(unit);

    if (NULL == gport || NULL == cosq) {
        return SOC_E_PARAM;
    }

    PBMP_ALL_ITER(unit, port) {
        if(IS_FAE_PORT(unit, port)) {
            continue;
        }
        if (mc == 0) {
            if (IS_LB_PORT(unit, port) || IS_CPU_PORT(unit, port)) {
                continue;
            }
        }
        if (mc) {
            if (index >= si->port_cosq_base[port] && index <= (si->port_cosq_base[port] + si->port_num_cosq[port])) {
                *gport = port;
                *cosq = index - si->port_cosq_base[port];
            }
        }else {
            if (index >= si->port_uc_cosq_base[port] && index <= (si->port_uc_cosq_base[port] + si->port_num_uc_cosq[port])) {
                *gport = port;
                *cosq = index - si->port_uc_cosq_base[port];
            }
        }
    }
return SOC_E_NONE;
}
    int
_soc_fb6_port_base_l0_mapping_get(int unit, int port, int *base_l0, int *num_l0)
{
    uint32 rval = 0;
    soc_reg_t reg = MTRO_PORT_L0_MAPPINGr;

    if ( port > FB6_CH_PORT_L0_START) {
        if (NULL != base_l0) {
            *base_l0 = port;
        }
        if (NULL != num_l0) {
            *num_l0 = 1;
        }
    } else {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));

        if (NULL != base_l0) {
            *base_l0 = soc_reg_field_get(unit, reg, rval, START_L0_NUMf);
        }

        if (NULL != num_l0) {
            *num_l0 = soc_reg_field_get(unit, reg, rval, NUM_L0Sf);
        }
    }
    return SOC_E_NONE;
}

/*
 * cpu port (mmu port 70): 48 queues (3690-3737)
 * loopback port (mmu port 71): 10 queues (3680-3689)
 */
int
soc_fb6_num_cosq_port_init(int unit, int port)
{
    soc_info_t *si;
    int base, i, j;
    int num_subport;

    si = &SOC_INFO(unit);

    SOC_IF_ERROR_RETURN(_soc_fb6_port_base_l0_mapping_get(unit, port, &base, NULL));
    if (IS_CPU_PORT(unit, port)) {
        si->port_num_cosq[port] = 48;
        si->port_cosq_base[port] = FB6_CPU_QUEUE_BASE;
    } else if (IS_LB_PORT(unit, port)) {
        si->port_num_cosq[port] = 10;
        si->port_cosq_base[port] = base * SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
    } else if (IS_UPLINK_PORT(unit, port)) {
        si->port_cosq_base[port] = base * SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
        si->port_num_cosq[port] = SOC_FB6_MMU_NUM_QUEUES_PER_HG_PORT;
        si->port_uc_cosq_base[port] = base * SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
        si->port_num_uc_cosq[port] = SOC_FB6_MMU_NUM_QUEUES_PER_HG_PORT;
    } else if (IS_STK_PORT(unit, port)) {
        si->port_cosq_base[port] = base * SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
        si->port_num_cosq[port] = SOC_FB6_MMU_NUM_QUEUES_PER_ST_PORT;
        si->port_uc_cosq_base[port] = base * SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
        si->port_num_uc_cosq[port] = SOC_FB6_MMU_NUM_QUEUES_PER_ST_PORT;
    } else {
        if ((si->port_num_subport[port] != -1) && (si->port_num_subport[port] != 0))  {
           num_subport =  si->port_num_subport[port];
           for(i = 0, j = base; j < (base + num_subport) && i < num_subport; i++, j++) {
            si->subport_cosq_base[port][i] = j * SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
            si->subport_cosq_num[port][i] = SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
            si->subport_cosq_uc_num[port][i] = SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
            si->subport_cosq_uc_base[port][i] = j * SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
           }
           si->port_cosq_base[port] = base * SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
           si->port_num_cosq[port] = num_subport *  SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
           si->port_num_uc_cosq[port] = num_subport * SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
           si->port_uc_cosq_base[port] = base * SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
        } else {
            si->port_cosq_base[port] = base * SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
            si->port_num_cosq[port] = SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
            si->port_num_uc_cosq[port] = SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
            si->port_uc_cosq_base[port] = base * SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
        }
    }
    return SOC_E_NONE;
}

static int
soc_fb6_st_port_get(int unit, int phy_port)
{
    int rv;
    rv = soc_property_phys_port_get(unit, phy_port, spn_PORT_STACKING, 0);
    return rv;
}

static int
soc_fb6_up_port_get(int unit, int phy_port)
{
    int rv;
    rv = soc_property_phys_port_get(unit, phy_port, spn_PORT_UPLINK, 0);
    return rv;
}

int
_soc_fb6_mmu_port_l0_mapping_set(int unit, bcm_port_t in_port)
{
    int port;
    uint32 rval;
    int l0_base = 0, num_l0 = 0, base = 0;
    int subport_base = 76;
    int phy_port;
    int mmu_port;
    soc_info_t *si;
    sch_port_l0_mapping_config_ram_entry_t sch_l0_config_entry;
    mmu_rqe_port_to_l0_map_entry_t rqe_port_entry;
    int w_en = 1;

    int lc_index = 0;
    uint32 subport_base_per_lc[8] = { 0 };


    soc_reg_t reg1 = MTRO_PORT_L0_MAPPINGr;
    soc_mem_t mem = SCH_PORT_L0_MAPPING_CONFIG_RAM_PIPE0m;
    soc_mem_t mem1 = MMU_RQE_PORT_TO_L0_MAP_SC0m;

    si = &SOC_INFO(unit);
    for (phy_port=0; phy_port < FB6_TOTAL_NUM_PORT; phy_port++) {
        mmu_port = si->port_p2m_mapping[phy_port];
        port = si->port_p2l_mapping[phy_port];
        lc_index = si->port_lc_mapping[port];

        if ((port != -1) && IS_FAE_PORT(unit, port)) {
            continue;
        }
        if ((in_port != -1) && (port != -1)) {
            if (port == in_port) {
                w_en = 1;
            } else {
                w_en = 0;
            }
        }

        sal_memset(&sch_l0_config_entry, 0, sizeof(sch_port_l0_mapping_config_ram_entry_t));
        rval = 0;
        if( port != -1 && IS_STK_PORT(unit, port)) {
            num_l0 = FB6_MAX_L0_ST_PORT;
        } else if(port != -1 && IS_UPLINK_PORT(unit, port)) {
            num_l0 = FB6_MAX_L0_UP_PORT;
        } else if ((port != -1) && (si->port_num_subport[port] > 0)) {
            num_l0 = si->port_num_subport[port];
        } else {
            num_l0 = FB6_MAX_L0_DW_PORT;
        }

        if ((port != -1) && (si->port_num_subport[port] > 0) &&
            (lc_index > 0)) {

            base = (((lc_index - 1) * FB6_NUM_PORT_PER_LC)
                + (FB6_TOTAL_NUM_PORT + 4)) + subport_base_per_lc[lc_index - 1];
            subport_base_per_lc[lc_index - 1] += si->port_num_subport[port];
        } else {
            base = l0_base;
        }
        if((port != -1) && IS_CPU_PORT(unit, port)) {
            base = FB6_CPU_L0_NUM;
        }
        if((port != -1) && IS_LB_PORT(unit, port)) {
            base = FB6_LB_L0_NUM;
        }

        if ((port != -1) && w_en) {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg1, port, 0,  &rval));
            soc_reg_field_set(unit, reg1, &rval, NUM_L0Sf, num_l0);
            soc_reg_field_set(unit, reg1, &rval, START_L0_NUMf, base);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg1, port, 0,  rval));

        }

        if ((mmu_port != -1) && w_en) {
            /* Update the channelized memory also. */
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ANY, mmu_port, &sch_l0_config_entry));

            soc_mem_field32_set(unit, mem, &sch_l0_config_entry, START_L0_NUMf, base);
            soc_mem_field32_set(unit, mem, &sch_l0_config_entry,  NUM_L0Sf, num_l0);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, mmu_port, &sch_l0_config_entry));


            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem1, MEM_BLOCK_ANY, mmu_port, &rqe_port_entry));

            soc_mem_field32_set(unit, mem1, &rqe_port_entry, L0_STARTf, base);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, mmu_port, &rqe_port_entry));

        }

        if ((port != -1) && (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port))) {
            continue;
        }
        if (soc_fb6_st_port_get(unit, phy_port)){
            l0_base += FB6_MAX_L0_ST_PORT;
        } else if (soc_fb6_up_port_get(unit, phy_port)){
            l0_base += FB6_MAX_L0_UP_PORT;
        } else if ((port != -1) && (si->port_num_subport[port] > 0) &&
            (si->port_lc_mapping[port])) {

            subport_base += si->port_num_subport[port];
            /* Reserve the L0 for this physical port. */
            l0_base += FB6_MAX_L0_DW_PORT;
        } else {
            l0_base += FB6_MAX_L0_DW_PORT;
        }
    }

    return SOC_E_NONE;
}


/*
 * cpu port (mmu port 70): 48 queues (3690-3737)
 * loopback port (mmu port 71): 10 queues (3680-3689)
 */
int
soc_firebolt6_num_cosq_init(int unit)
{
    int port;

    SOC_IF_ERROR_RETURN(_soc_fb6_mmu_port_l0_mapping_set(unit, -1));
    PBMP_ALL_ITER(unit, port) {
        if (!IS_FAE_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
                (soc_fb6_num_cosq_port_init(unit, port));
        }
    }

    return SOC_E_NONE;
}


int soc_fb6_show_subport_map(int unit)
{
    int port, i;
    soc_info_t *si;
    si = &SOC_INFO(unit);
    cli_out("          subport          ucq/Numq           mcq/Numq \n");
    PBMP_ALL_ITER(unit, port) {
       if (si->port_num_subport[port] > 0) {
           for( i = 0; i < si->port_num_subport[port]; i++) {
                cli_out("      %8s:%d            %4d/%-4d          %4d/%-4d \n",
                SOC_INFO(unit).port_name[port], i, si->subport_cosq_uc_base[port][i], si->subport_cosq_uc_num[port][i],
                si->subport_cosq_base[port][i], si->subport_cosq_num[port][i]);
          }
       }
    }

    return SOC_E_NONE;
}

STATIC int
soc_firebolt6_max_frequency_get(int unit,  uint16 dev_id, uint8 rev_id,
                               int skew_id, int *frequency)
{

   if (soc_feature(unit, soc_feature_untethered_otp)) {
        *frequency = SOC_BOND_INFO(unit)->default_core_freq;
        *frequency = soc_property_get(unit,
                spn_CORE_CLOCK_FREQUENCY, *frequency);
        if (*frequency > SOC_BOND_INFO(unit)->max_core_freq) {
            return SOC_E_PARAM;
        }
    } else {
        switch(dev_id) {
            case BCM56470_DEVICE_ID:
            case BCM56471_DEVICE_ID:
            case BCM56475_DEVICE_ID:
                *frequency = soc_property_get(unit,
                        spn_CORE_CLOCK_FREQUENCY, 1250);
                break;
            case BCM56472_DEVICE_ID:
                *frequency = soc_property_get(unit,
                        spn_CORE_CLOCK_FREQUENCY, 1012);
                break;
            default:
                return SOC_E_INTERNAL;
        }
    }
    return SOC_E_NONE;
}

/* Function to get the number of ports present in a given Port Macro */
STATIC int
soc_fb6_ports_per_pm_get(int unit, int pm_id)
{
    soc_info_t *si;
    int phy_port, num_ports = 0, i;

    si = &SOC_INFO(unit);
    if (pm_id >= _FB6_PBLKS_PER_DEV(unit)) {
       return SOC_E_PARAM;
    }
    phy_port = 1 + (pm_id * FIREBOLT6_PORTS_PER_PBLK);

    for (i = 0; i < FIREBOLT6_PORTS_PER_PBLK; ) {
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
_soc_fb6_port_flex_init(int unit, int is_any_cap)
{
    soc_info_t *si;
    int pm, blk_idx, blk_type;
    int flex_en, static_ports, max_ports;

    si = &SOC_INFO(unit);
    SHR_BITCLR_RANGE(si->flexible_pm_bitmap, 0, SOC_MAX_NUM_BLKS);

    /* portmap_x=y:speed:i */
    /* portmap_x=y:speed:cap */
    if (SOC_PBMP_NOT_NULL(SOC_PORT_DISABLED_BITMAP(unit, all)) || is_any_cap) {
        SHR_BITSET_RANGE(si->flexible_pm_bitmap, 1, _FB6_PBLKS_PER_DEV(unit));
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
                        (pm * _FB6_PORTS_PER_PBLK + 1),
                        spn_PORT_FLEX_ENABLE, 0);
                }
                if (flex_en) {
                    SHR_BITSET(si->flexible_pm_bitmap, blk_idx);
                }
            }
        }
    }

    si->flex_eligible =
        (!SHR_BITNULL_RANGE(si->flexible_pm_bitmap, 1, _FB6_PBLKS_PER_DEV(unit)));

    /* port_flex_max_ports */
    memset(&(si->pm_max_ports), 0xff, sizeof(si->pm_max_ports));
    for (blk_idx = 0; (blk_idx < SOC_MAX_NUM_BLKS) && (SOC_BLOCK_TYPE(unit, blk_idx)!= -1); blk_idx++) {
        blk_type = SOC_BLOCK_TYPE(unit, blk_idx);
        pm = SOC_BLOCK_NUMBER(unit, blk_idx);
        if (blk_type == SOC_BLK_CLPORT) {
            static_ports = soc_fb6_ports_per_pm_get(unit, pm);
            if (SHR_BITGET(si->flexible_pm_bitmap, blk_idx)) {
                /* port_flex_max_ports{x}=y */
                max_ports = soc_property_phys_port_get(unit,
                    (pm * _FB6_PORTS_PER_PBLK + 1),
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

#define PORT_IS_FALCON(unit, phy_port) \
        ((phy_port  >= 1) && (phy_port <= 68)) ? 1 : 0

STATIC int
_soc_fb6_port_lanes_set(int unit, int phy_port, int *lanes, int port_bandwidth)
{
    if (PORT_IS_FALCON(unit, phy_port)) {
        *lanes =   (port_bandwidth > 53000) ? 4
                 : (port_bandwidth > 42000) ? 2
                 : (port_bandwidth > 27000) ? 2
                 : (port_bandwidth == SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(21000)) ? 2
                 : 1;
    } else {
        *lanes =   (port_bandwidth > 21000) ? 4
                 : (port_bandwidth > 11000) ? 2
                 : 1;
    }

    return ((*lanes == -1) ? SOC_E_CONFIG : SOC_E_NONE);
}

STATIC int
soc_firebolt6_ledup_init(int unit)
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

       Where M0SS freqency is 857MHz and
       Typical LED clock period is 200ns(5MHz) = 2*10^-7
    */
    freq = 857 * 1000000;
    clk_half_period = (freq + 2500000) / 5000000;
    clk_half_period = clk_half_period / 2;

    rval = 0;
    soc_reg_field_set(unit, U0_LED_CLK_DIV_CTRLr, &rval,
                      LEDCLK_HALF_PERIODf, clk_half_period);

    SOC_IF_ERROR_RETURN(WRITE_U0_LED_CLK_DIV_CTRLr(unit, rval));


    SOC_IF_ERROR_RETURN(READ_U0_LED_ACCU_CTRLr(unit, &rval));

    soc_reg_field_set(unit, U0_LED_ACCU_CTRLr, &rval,
                      LAST_PORTf, 64);
    SOC_IF_ERROR_RETURN(WRITE_U0_LED_ACCU_CTRLr(unit, rval));

    /* To initialize M0s for LED or Firmware Link Scan*/
    if (soc_feature(unit, soc_feature_cmicx) && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
        SOC_IF_ERROR_RETURN(soc_iproc_m0_init(unit));
    }

    return SOC_E_NONE;
}


int
soc_fb6_port_e2ecc_config_set(int unit, int port, int *low_pri_queues,
        int *high_pri_queues, int enable) {

    soc_info_t *si;
    char *config_str, *sub_str, *sub_str_end;
    int line_card = -1, i, index;
    int mmu_port, phy_port;
    unsigned char low = 0, high = 0;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = MMU_INTFI_E2ECC_ST_TRANS_TBL_SC0m;

    si = &SOC_INFO(unit);
    config_str = soc_property_port_get_str(unit, port, spn_PORTMAP);
    if (config_str == NULL) {
        return SOC_E_PARAM;
    }


    sub_str = config_str;

    while (*sub_str != '\0') {
        if (*sub_str == 'l') {
            sub_str += 2;
            line_card = sal_ctoi(sub_str, &sub_str_end);

        }
        sub_str++;
    }
    index = line_card - 1;
    if ((index >= 0) && (fb6_e2ecc_map[index] == 0 || fb6_e2ecc_map[index] == port)) { 
        fb6_e2ecc_map[index] = enable ? port : 0;
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        for (i = 0; i < 8; i++) {
            if (low_pri_queues[i] == 1) {
                low |= (1 << i);
            }
            if (high_pri_queues[i] == 1) {
                high |= (1 << i);
            }
        }
        SOC_IF_ERROR_RETURN(READ_BUF_CFGr(unit, index, &rval));
        soc_reg_field_set(unit, BUF_CFGr, &rval, NUM_BYTEf, enable ? FB6_NUM_SUBPORT_PER_LC : 0);
        soc_reg_field_set(unit, BUF_CFGr, &rval, SRC_PORTNUMf, enable ? mmu_port : 0);
        soc_reg_field_set(unit, BUF_CFGr, &rval, PROFILE_IDf, enable ? index : 0);
        SOC_IF_ERROR_RETURN(WRITE_BUF_CFGr(unit, index, rval));


        SOC_IF_ERROR_RETURN(READ_MMU_INTFI_BASE_INDEX_TBLr(unit, index, &rval));
        soc_reg_field_set(unit, MMU_INTFI_BASE_INDEX_TBLr,
                                &rval, BASE_ADDRf, enable ? (index * FB6_NUM_SUBPORT_PER_LC) : 0);
        SOC_IF_ERROR_RETURN(WRITE_MMU_INTFI_BASE_INDEX_TBLr(unit, index, rval));

        index =  index * FB6_ENTRIES_PER_CARD;
        for (i = (index + 1); i  < (index + 4); i++) {
            sal_memset(entry, 0, sizeof(entry));
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ANY, i, entry));
            if (i == index + 1) {
                soc_mem_field32_set(unit, mem, entry, FC_ST_XLATEf, enable ? low : 0);
            } else if (i == index + 2) {
                soc_mem_field32_set(unit, mem, entry, FC_ST_XLATEf, enable ? high : 0);
            } else  {
                soc_mem_field32_set(unit, mem, entry, FC_ST_XLATEf, enable ? (low | high) : 0);
            }

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ANY, i, entry));
        }

    }

    return SOC_E_NONE;
}

int
soc_fb6_port_e2ecc_enable(int unit) {
   int low_pri_queues[8] = {1,1,0,0,0,0,0,0};
   int high_pri_queues[8] = {0,0,1,1,1,1,1,1};
   int enable = 1, port;
   char *config_str;

   for (port = 1; port <= FB6_NUM_PHY_PORT; port++) {
       config_str = soc_property_port_get_str(unit, port, spn_PORTMAP);
       if((NULL != config_str) && (NULL != sal_strchr(config_str, 'l'))) {
           SOC_IF_ERROR_RETURN
               (soc_fb6_port_e2ecc_config_set(unit, port, low_pri_queues, high_pri_queues, enable));
       }
   }
   return SOC_E_NONE;
}

int
soc_fb6_port_config_init(int unit, uint16 dev_id)
{
    soc_info_t *si;
    char *config_str, *sub_str, *sub_str_end;
    static const char str_2p5[] = "2.5";
    char str_buf[8];
    int rv;
    int num_port, num_phy_port, num_mmu_port;
    int port, phy_port, mmu_port;
    int pipe, index, bandwidth_cap, is_any_cap = FALSE;
    int port_bandwidth, blk_idx, port_lanes=0;
    int line_card = 0;
    pbmp_t non_hg_pbm;

    si = &SOC_INFO(unit);

    num_phy_port = FB6_NUM_PHY_PORT;
    num_port = FB6_NUM_PORT;
    num_mmu_port = FB6_NUM_MMU_PORT;

    sal_memset(fb6_e2ecc_map, 0, sizeof(fb6_e2ecc_map));
    soc_firebolt6_max_frequency_get(unit, dev_id, -1, -1, &si->frequency);

    si->bandwidth = 2048000;
    SOC_PBMP_CLEAR(non_hg_pbm);

    si->oversub_pbm = soc_property_get_pbmp(unit, spn_PBMP_OVERSUBSCRIBE, 0);
    si->oversub_mode = soc_property_get(unit, spn_OVERSUBSCRIBE_MODE, 2);
    non_hg_pbm       = soc_property_get_pbmp(unit, spn_PBMP_XPORT_XE, 0);

    for (phy_port = 0; phy_port < num_port; phy_port++) {
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
        si->port_half_pipe[port] = -1;
        si->port_lc_mapping[port] = -1;
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
    for (index = 0; index < COUNTOF(fb6_anc_ports); index++) {
        port = fb6_anc_ports[index].port;
        phy_port = fb6_anc_ports[index].phy_port;
        pipe = fb6_anc_ports[index].pipe;;

        si->port_l2p_mapping[port] = phy_port;
        si->port_l2i_mapping[port] = fb6_anc_ports[index].idb_port;
        si->port_p2l_mapping[phy_port] = port;
        si->port_p2m_mapping[phy_port] = fb6_anc_ports[index].mmu_port;
        mmu_port = fb6_anc_ports[index].mmu_port;
        si->port_m2p_mapping[mmu_port] = phy_port;
        si->port_pipe[port] = pipe;
        si->port_half_pipe[port] = -1;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);
    }

    rv = SOC_E_NONE;
    for (port = 1; port <= num_phy_port; port++) {

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
        if (phy_port < 0 || phy_port > num_phy_port) {
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
        }

        sub_str = sub_str_end;
        while (*sub_str != '\0') {

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
                    _soc_fb6_port_speed_cap[unit][port] = bandwidth_cap;
                    /* Flex config detected, port speed cap specified */
                    is_any_cap = TRUE;
                    sub_str++;
                    break;
                default:
                    if (!(*sub_str == 'i' || *sub_str == '1' ||
                          *sub_str == 'm' || *sub_str == '2' ||
                          *sub_str == '4' || *sub_str == 'l')) {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string "
                                            "\"%s\"\n"),
                             port, config_str));
                        rv = SOC_E_FAIL;
                        break;
                    }
                }

                switch  (*sub_str) {
                case 'i' :
                    SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
                    si->port_num_lanes[port] = -1;
                    sub_str++;
                    break;

                case 'm' :
                    SOC_PBMP_PORT_ADD(si->management_pbm, port);
                    sub_str++;
                    break;
                case 'l' :
                    /* Move one step to cross option. */
                    sub_str++;
                    /* Move : away from the way. */
                    sub_str++;
                    line_card = sal_ctoi(sub_str, &sub_str_end);

                    if (sub_str == sub_str_end) {
                        LOG_CLI((BSL_META_U(unit,
                            "Port %d: Missing Line card number \"%s\"\n"),
                             port, config_str));
                            rv = SOC_E_FAIL;
                         break;
                     }

                    if ((line_card > 8) && (line_card < 1)) {
                       LOG_CLI((BSL_META_U(unit,
                              "Port %d: Bad config : more linecards \"%s\"\n"),
                             port, config_str));
                        rv = SOC_E_FAIL;
                        break;
                     }
                     LOG_CLI((BSL_META_U(unit,
                         "line card number  = %d on Port %d: \n"),
                         line_card, port));
                     si->port_lc_mapping[port] = line_card;

                     sub_str = sub_str_end;
                     if (*sub_str != '\0') {
                        /* Make it go back by one to get : back. loop checks for it. */
                        sub_str--;
                     }
                break;
                case '1' : port_lanes = 1; sub_str++; break;
                case '2' : port_lanes = 2; sub_str++; break;
                case '4' : port_lanes = 4; sub_str++; break;
                default:
                    LOG_CLI((BSL_META_U(unit, "Port %d: Bad config string " "\"%s\"\n"),
                        port, config_str));

                        rv = SOC_E_FAIL;
                        break;
                }
            }
            if (rv == SOC_E_FAIL) {
                /* Because of some error, move out of loop. */
                break;
            }
        }

        if (rv == SOC_E_FAIL) {
            /* There is error in configuration. Go to port iteration. */
            continue;
        }

        /* Update soc_info */
        si->port_l2p_mapping[port] = phy_port;
        si->port_p2l_mapping[phy_port] = port;
        si->port_serdes[port] = (phy_port - 1) / _FB6_PORTS_PER_PBLK;
        pipe = 0;

        si->port_pipe[port] = pipe;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);
        si->port_speed_max[port] = port_bandwidth;
        si->port_init_speed[port] = port_bandwidth;

        si->port_num_lanes[port] = port_lanes;
        if (!port_lanes) {
            if (SOC_FAILURE(_soc_fb6_port_lanes_set(unit, phy_port,
                                                        &si->port_num_lanes[port],
                                                        port_bandwidth))) {
                rv = SOC_E_FAIL;
                break;
            }
        }
        if (soc_fb6_port_oversub_get(unit, phy_port, port)) {
            SOC_PBMP_PORT_ADD(si->oversub_pbm, port);
        }

        if (si->port_speed_max[port] >= 40000) {
            SOC_PBMP_PORT_ADD(si->eq_pbm, port);
        }
        if (!(SAL_BOOT_SIMULATION)) {
            /* Validate port speed based on OTP. */
            SOC_IF_ERROR_RETURN(
                _soc_fb6_port_speed_validate(unit, port, -1, -1,
                    (SOC_PBMP_MEMBER(non_hg_pbm, port) ?
                         SOC_ENCAP_IEEE:SOC_ENCAP_HIGIG2),
                    port_bandwidth));
        }

        port_lanes = 0;
    }

    /* Set up mmu port mapping. */
    SOC_IF_ERROR_RETURN(_soc_fb6_mmu_idb_ports_assign(unit));

    /* Set up uplinkg and stk ports. */
    _soc_fb6_uplink_stk_ports_assign(unit);

    /* get flex port properties */
    if (SOC_FAILURE(_soc_fb6_port_flex_init(unit, is_any_cap))) {
        rv = SOC_E_FAIL;
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (si->flex_eligible) {
        if (SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN(soc_fb6_flexport_scache_recovery(unit));
        } else {
            SOC_IF_ERROR_RETURN(soc_fb6_flexport_scache_allocate(unit));
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

    /* Setup clport refclk master block bitmap.
     * In FB6, there are 5 port macros (clport 0,4,5,12,13) that
     * will driven directly from the system board and they will act as the
     * refclk master to drive the other instances.
     */
    SHR_BITCLR_RANGE(si->pm_ref_master_bitmap, 0, SOC_MAX_NUM_BLKS);
    for (blk_idx = 0; blk_idx < SOC_MAX_NUM_BLKS; blk_idx++) {
        int blknum = SOC_BLOCK_NUMBER(unit, blk_idx);
        int blktype = SOC_BLOCK_TYPE(unit, blk_idx);

        /* CLPORTs 4,5,14,15 provide master clock for other ports */
        if (blktype == SOC_BLK_CLPORT) {
            switch (blknum) {
            case 0:
            case 4:
            case 5:
            case 12:
            case 13:
                SHR_BITSET(si->pm_ref_master_bitmap, blk_idx);
            break;

            default:
                /* Nothing to be done here. */
            break;
            }
        }
    }

    return rv;

}

int
soc_firebolt6_uft_uat_config(int unit)
{

    int index, shared_bank_offset = 2;
    int num_l2_banks, num_l3_banks, num_fpem_banks = 0;
    int dedicated_banks = 2;
    int offset_base, offset_multiplier ;
    int mpls_uat_banks;
    int vlan_xlate_1_uat_banks;
    int vlan_xlate_2_uat_banks;
    int egr_vlan_xlate_1_uat_banks, egr_vlan_xlate_2_banks;
    uint32 hash_control_entry[4], shared_hash_control_entry[4],
    shared_4k_hash_control_entry[4];
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
        LOGICAL_BANK_9_PHYSICAL_BANK_LOCATIONf,
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
        (soc_helix5_hash_bank_count_get(unit, EXACT_MATCH_2m,
                                          &num_fpem_banks));
    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, EGR_VLAN_XLATE_1_SINGLEm,
                                          &egr_vlan_xlate_1_uat_banks));
    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, EGR_VLAN_XLATE_2_SINGLEm,
                                          &egr_vlan_xlate_2_banks));
    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, MPLS_ENTRY_SINGLEm,
                                          &mpls_uat_banks));
    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, VLAN_XLATE_1_SINGLEm,
                                          &vlan_xlate_1_uat_banks));
    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, VLAN_XLATE_2_SINGLEm,
                                          &vlan_xlate_2_uat_banks));

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
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_LOG_TO_PHY_BANK_MAP_2r,
                                        REG_PORT_ANY, ALPM_BANK_MODEf, 1));
            (void)soc_trident3_set_alpm_banks(unit, 4);
            shared_bank_offset += 4;
        } else {
            soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                                hash_control_entry, ALPM_MODEf, 0);
            soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                                hash_control_entry, BANK_BYPASS_LPf, 0xff);
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_LOG_TO_PHY_BANK_MAP_2r,
                                        REG_PORT_ANY, ALPM_BANK_MODEf, 0));
            (void)soc_trident3_set_alpm_banks(unit, 8);
        }
        SOC_IF_ERROR_RETURN
        (WRITE_UFT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             hash_control_entry));
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

    offset_multiplier = 64 / num_l2_banks;
    offset_base = 64 % num_l2_banks;

    for (index = 0; index < dedicated_banks; index++) {
        soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm, hash_control_entry,
                            dedicated_fields[index], offset_base + (index *
                            offset_multiplier));
    }

    for (index = dedicated_banks; index < num_l2_banks; index++) {
        soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index -
                            dedicated_banks], offset_base +
                            (index * offset_multiplier));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_L2_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0, hash_control_entry));

    sal_memset(hash_control_entry, 0, sizeof(exact_match_hash_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                            hash_control_entry));
    shared_bank_offset += (num_l2_banks - dedicated_banks) ;

    /* Handle case where fpem banks are set to 0 */
    if (num_fpem_banks) {
        offset_multiplier = 64 / num_fpem_banks;
        offset_base = 64 % num_fpem_banks;
    }

    soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm, hash_control_entry,
                      HASH_TABLE_BANK_CONFIGf,
                      ((1 << num_fpem_banks) - 1) << shared_bank_offset);
    for (index = 0 ; index < num_fpem_banks; index++) {
        soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm , hash_control_entry,
                          logical_to_physical_fields[index], index +
                          shared_bank_offset);
        soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry,
                            shared_fields[shared_bank_offset + index -
                            dedicated_banks],
                            offset_base + (index * offset_multiplier));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                            hash_control_entry));

    sal_memset(hash_control_entry, 0, sizeof(l3_entry_hash_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                        hash_control_entry));
    shared_bank_offset += num_fpem_banks;
    offset_multiplier = 64 / num_l3_banks;
    offset_base = 64 % num_l3_banks;

    /* The dedicated banks start at bank 10. */
    soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, ((((1 << (num_l3_banks -
                        dedicated_banks)) - 1) << shared_bank_offset) |
                        (((1 << dedicated_banks) - 1) << 10)));

    for (index = 0; index < dedicated_banks; index++) {
        soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm, hash_control_entry,
                            dedicated_fields[index], offset_base + (index *
                            offset_multiplier));
    }

    for (index = dedicated_banks ; index < num_l3_banks ; index++) {
        soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm, hash_control_entry,
                          logical_to_physical_fields[index],
                          index + shared_bank_offset - dedicated_banks);
        soc_mem_field32_set(unit, UFT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry,
                            shared_fields[shared_bank_offset + index -
                            (dedicated_banks * 2)],
                            offset_base + (index * offset_multiplier));
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

    if (egr_vlan_xlate_1_uat_banks) {
        offset_base = 64 % egr_vlan_xlate_1_uat_banks;
        offset_multiplier = 64 / egr_vlan_xlate_1_uat_banks;
    }

    SOC_IF_ERROR_RETURN
        (READ_EP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                            shared_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (READ_EGR_VLAN_XLATE_1_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                hash_control_entry));
    soc_mem_field32_set(unit, EGR_VLAN_XLATE_1_HASH_CONTROLm,
                        hash_control_entry, HASH_TABLE_BANK_CONFIGf,
                        ((1 << egr_vlan_xlate_1_uat_banks) - 1)) ;

    for(index = 0; index < egr_vlan_xlate_1_uat_banks; index++) {
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
                        egr_vlan_xlate_1_uat_banks) ;

    for(index = 0; index < egr_vlan_xlate_2_banks; index++) {
        soc_mem_field32_set(unit, EGR_VLAN_XLATE_2_HASH_CONTROLm,
                            hash_control_entry,
                            logical_to_physical_fields[index],
                            index + egr_vlan_xlate_1_uat_banks);
        soc_mem_field32_set(unit, EP_UAT_SHARED_BANKS_CONTROLm,
                            shared_hash_control_entry, shared_fields[index +
                            egr_vlan_xlate_1_uat_banks], offset_base + (index *
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
    sal_memset(shared_4k_hash_control_entry, 0,
                sizeof(ip_uat_shared_banks_control_entry_t));
    sal_memset(hash_control_entry, 0,
                sizeof(mpls_entry_hash_control_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_IP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                shared_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (READ_IP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                shared_4k_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (READ_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                                hash_control_entry));

    if (mpls_uat_banks) {
        offset_base = 64 % (mpls_uat_banks);
        offset_multiplier = 64 / (mpls_uat_banks);
    }
    soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, ((1 << mpls_uat_banks) - 1));
    for(index = 0; index < mpls_uat_banks; index++) {
        soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, hash_control_entry,
                            logical_to_physical_fields[index], index);
        soc_mem_field32_set(unit, IP_UAT_SHARED_BANKS_CONTROLm,
                            shared_4k_hash_control_entry, shared_fields[index],
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
    if (vlan_xlate_1_uat_banks) {
        offset_base = 64 % (vlan_xlate_1_uat_banks);
        offset_multiplier = 64 / (vlan_xlate_1_uat_banks);
    }
    soc_mem_field32_set(unit, VLAN_XLATE_1_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, ((((1 << vlan_xlate_1_uat_banks)
                        - 1) << mpls_uat_banks)));

    for(index = 0; index < vlan_xlate_1_uat_banks; index++) {
        soc_mem_field32_set(unit, VLAN_XLATE_1_HASH_CONTROLm,
                            hash_control_entry,
                            logical_to_physical_fields[index], index +
                            mpls_uat_banks);
        soc_mem_field32_set(unit, IP_UAT_SHARED_BANKS_CONTROLm,
                            shared_4k_hash_control_entry, shared_fields[index +
                            mpls_uat_banks], offset_base + (index *
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
    if (vlan_xlate_2_uat_banks) {
        offset_base = 64 % (vlan_xlate_2_uat_banks);
        offset_multiplier = 64 / (vlan_xlate_2_uat_banks);
    }
    soc_mem_field32_set(unit, VLAN_XLATE_2_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, ((((1 << vlan_xlate_2_uat_banks)
                        - 1) << (mpls_uat_banks + vlan_xlate_1_uat_banks))));

    for(index = 0; index < vlan_xlate_2_uat_banks; index++) {
        soc_mem_field32_set(unit, VLAN_XLATE_2_HASH_CONTROLm, hash_control_entry,
                            logical_to_physical_fields[index], index +
                            mpls_uat_banks + vlan_xlate_1_uat_banks);
        soc_mem_field32_set(unit, IP_UAT_SHARED_BANKS_CONTROLm,
                            shared_4k_hash_control_entry, shared_fields[index +
                            mpls_uat_banks + vlan_xlate_1_uat_banks], offset_base
                            + (index * offset_multiplier));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_VLAN_XLATE_2_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                hash_control_entry));
    SOC_IF_ERROR_RETURN
        (WRITE_IP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                                shared_4k_hash_control_entry));

    /* Create a mutex for s/w re-ordering */
    SOC_IF_ERROR_RETURN(soc_td3_hash_mutex_init (unit));

    return SOC_E_NONE;

}


int
soc_firebolt6_port_reset(int unit)
{
    return soc_trident3_port_reset(unit);
}

STATIC int
_soc_firebolt6_clear_all_memory(int unit)
{
    soc_reg_t reg;
    uint32 rval;
    int is_sim = 0;

    is_sim = (SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM) ? 1 : 0;

    reg = BSK_HS_HW_RESET_CONTROLr;
    rval = 0;

    soc_reg_field_set(unit, reg, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, reg, &rval, VALIDf, 1);
    soc_reg_field_set(unit, reg, &rval, COUNTf, 2048);
    SOC_IF_ERROR_RETURN(WRITE_BSK_HS_HW_RESET_CONTROLr(unit, rval));

    SOC_IF_ERROR_RETURN(soc_trident3_clear_all_memory(unit));

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

    /* Timeout in soc_trident3_clear_all_memory is sufficient */
    if (!is_sim) {
        if (soc_reg_field_get(unit, reg, rval, DONEf)) {
            rval = SOC_REG_INFO(unit, reg).rst_val_lo;
            SOC_IF_ERROR_RETURN(WRITE_BSK_HS_HW_RESET_CONTROLr(unit, rval));
        } else {
            LOG_WARN(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d : BSK_HS_HW_RESET timeout\n"), unit));
        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_firebolt6_port_mapping_init(int unit)
{
    soc_info_t *si;
    int port, phy_port, idb_port, i;
    soc_reg_t reg;
    soc_mem_t mem;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe = 0;

    si = &SOC_INFO(unit);

    sal_memset(&entry, 0, sizeof(entry));
    mem = SOC_MEM_UNIQUE_ACC(unit, ING_PHY_TO_IDB_PORT_MAPm)[pipe];

    /* Set all entries to 0x7f as default */
    soc_mem_field32_set(unit, mem, &entry, IDB_PORTf, 0x7f);
    soc_mem_field32_set(unit, mem, &entry, VALIDf, 0);
    for (phy_port = 0; phy_port < FB6_NUM_MMU_PORT; phy_port++) {
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port, &entry));
    }

    /* Ingress IDB to device port mapping */
    sal_memset(&entry, 0,
               sizeof(ing_idb_to_device_port_number_mapping_table_entry_t));
    mem = SOC_MEM_UNIQUE_ACC(unit, ING_IDB_TO_DEVICE_PORT_NUMBER_MAPPING_TABLEm)[pipe];
    for (idb_port = 0; idb_port < FB6_NUM_PORT; idb_port++) {
        /* Set all entries to 0x7f since Device Port No. 0 corresponds to CPU port*/
        soc_mem_field32_set(unit, mem, &entry, DEVICE_PORT_NUMBERf, 0x7f);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, idb_port, &entry));
    }

    /* Ancillary ports */
    for (i = 0; i < COUNTOF(fb6_anc_ports); i++) {
        idb_port = fb6_anc_ports[i].idb_port;
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
    for (port = 0; port < FB6_NUM_PORT; port++) {
        soc_mem_field32_set(unit, mem, &entry, DEVICE_PORT_NUMBERf, port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &entry));
    }


    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_firebolt6_latency_config
 * Purpose:
 *      Configure latency in SLOT_PIPELINE_CONFIG register
 * Parameters:
 *      unit (IN)    - switch unit
 * Returns:
 *      SOC_E_*
 */

STATIC int
_soc_firebolt6_latency_config(int unit)
{
    soc_reg_t reg;
    uint32 rval;
    int latency = 0;
/*
    uint16      dev_id;
    uint8       rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
*/

    _soc_fb6_sku_params_get(unit, BCMI_FB6_SKU_ELEM_TYPE_DPR , &latency);


    reg = SLOT_PIPELINE_CONFIGr;
    rval = 0;

    soc_reg_field_set(unit, reg, &rval, LATENCYf, latency);
    soc_reg_field_set(unit, reg, &rval, WR_ENf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_fb6_set_idb_dpp_ctrl
 * Purpose:
 *      Configure IDB DPP credits
 * Parameters:
 *      unit (IN)    - switch unit
 * Returns:
 *      SOC_E_*
 */
static int
_soc_fb6_set_idb_dpp_ctrl(int unit)
{
    soc_reg_t reg;
    uint32 pipe_map, in_progress;
    int pipe;
    uint32 rval, rval0;
    int pipe_init_usec;
    soc_timeout_t to;
    int credits = 62;
    int is_sim = 0;
    static const soc_reg_t idb_dpp_ctrl[2] = {
        IDB_DPP_CTRL_PIPE0r, IDB_DPP_CTRL_PIPE1r
    };

    is_sim = (SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM) ? 1 : 0;

    _soc_fb6_sku_params_get(unit, BCMI_FB6_SKU_ELEM_TYPE_CREDITS , &credits);


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
        soc_reg_field_set(unit, reg, &rval, CREDITSf, credits);
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
    } while ((in_progress != 0) && (!is_sim));

    return SOC_E_NONE;
}

int
soc_fb6_port_oversub_get(int unit, int phy_port, int logical_port)
{
    int oversub_mode = 0, oversub = 0;

    oversub_mode = soc_property_get(unit, spn_OVERSUBSCRIBE_MODE, 2);

    if (oversub_mode == 0) {
        oversub = 0;
    } else if (oversub_mode == 1) {
        oversub = 1;
    } else if (oversub_mode == 2) {
        if (phy_port > 0) {
            oversub = soc_property_phys_port_get(unit, phy_port,
                          spn_PORT_OVERSUBSCRIBE, 1);
        }
    }
    return oversub;
}


#ifdef BCM_WARM_BOOT_SUPPORT

#define SOC_FLEXPORT_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_FLEXPORT_WB_DEFAULT_VERSION            SOC_FLEXPORT_WB_VERSION_1_0

int
soc_fb6_flexport_scache_allocate(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int stable_size;
    int default_ver = SOC_FLEXPORT_WB_DEFAULT_VERSION;

    alloc_size =  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* phy to logical*/
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* phy to mmu */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* logical to phy */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* max port speed */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* init port speed */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* num of lanes */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* line card numbers */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* num subports */
                  (sizeof(pbmp_t))                       + /* subport bitmap */
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
 *      soc_fb6_flexport_scache_sync
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
soc_fb6_flexport_scache_sync(int unit)
{
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    uint32 var_size = 0;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 scache_offset=0;
    int rv = 0;

    alloc_size =  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* phy to logical*/
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* phy to mmu */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* logical to phy */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* max port speed */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* init port speed */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* num of lanes */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* line card numbers */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* num subports */
                  (sizeof(pbmp_t))                       + /* subport bitmap */
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
    var_size = sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV;
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2l_mapping, var_size);
    scache_offset += var_size;


    /* Physical to MMU port mapping */
    var_size = sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV;
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2m_mapping, var_size);
    scache_offset += var_size;

    /* Logical to Physical port mapping */
    var_size = sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV;
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_l2p_mapping, var_size);
    scache_offset += var_size;
    /* Max port speed */
    var_size = sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV;

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

    /* line card number */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_lc_mapping, var_size);
    scache_offset += var_size;

    /* num of subports */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_num_subport, var_size);
    scache_offset += var_size;

    /* Subtag allowed pbm */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               &si->subtag_allowed_pbm,
               sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

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
 *      soc_fb6_flexport_scache_recovery
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
soc_fb6_flexport_scache_recovery(int unit)
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

    alloc_size =  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* phy to logical*/
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* phy to mmu */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* logical to phy */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* max port speed */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* init port speed */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* num of lanes */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* line card numbers */
                  (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV) + /* num subports */
                  (sizeof(pbmp_t))                       + /* subport bitmap */
                  (sizeof(pbmp_t))                       + /* HG bitmap */
                  (sizeof(pbmp_t));                        /* Disabled bitmap */

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
    var_size = (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV);
    sal_memcpy(si->port_p2l_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /*Physical to MMU port mapping */
    var_size = (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV);
    sal_memcpy(si->port_p2m_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /*Logical to Physical port mapping*/
    var_size = (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV);
    sal_memcpy(si->port_l2p_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Max port speed */
    var_size = (sizeof(int) * FIREBOLT6_PHY_PORTS_PER_DEV);

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

    /* line card number */
    sal_memcpy(si->port_lc_mapping,
                &flexport_scache_ptr[scache_offset], var_size);
    scache_offset += var_size;

    /* num of subports */
    sal_memcpy(si->port_num_subport,
               &flexport_scache_ptr[scache_offset], var_size);
    scache_offset += var_size;

    /* Subtag allowed pbm */
    sal_memcpy(&si->subtag_allowed_pbm,
               &flexport_scache_ptr[scache_offset],
               sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

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

    for (port = 1; port < FB6_NUM_PORT; port++) {
        phy_port = si->port_l2p_mapping[port];
        if (phy_port == -1) {
            SOC_PBMP_PORT_REMOVE(si->oversub_pbm, port);
            si->port_l2i_mapping[port] = -1;
            si->port_serdes[port] = -1;
            si->port_pipe[port] = -1;
            continue;
        }
        si->port_l2i_mapping[port] = si->port_p2m_mapping[phy_port];
        si->port_serdes[port] = ((phy_port - 1) / _FB6_PORTS_PER_PBLK);
        si->port_pipe[port] = 0;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[0], port);

        if (soc_fb6_port_oversub_get(unit, phy_port, port)) {
            SOC_PBMP_PORT_ADD(si->oversub_pbm, port);
        }
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
 *      soc_fb6_tdm_scache_allocate
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
int soc_fb6_tdm_scache_allocate(int unit)
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
    ovs_size = FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE *
               FIREBOLT6_OVS_GROUP_COUNT_PER_HPIPE *
               FIREBOLT6_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE *
                   _FB6_PKT_SCH_LENGTH;
    hpipes = FIREBOLT6_PIPES_PER_DEV * FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * FIREBOLT6_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * FIREBOLT6_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * FIREBOLT6_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * FIREBOLT6_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * FIREBOLT6_PBLKS_PER_DEV)     + /* port ratio */
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
 *      soc_fb6_tdm_scache_sync
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
int soc_fb6_tdm_scache_sync(int unit)
{
    uint8 *tdm_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    uint32 var_size = 0;
    uint32 scache_offset=0;
    int rv = 0;
    int ilen, ovs_size, pkt_shp_size, hpipes, phy_port;
    _soc_firebolt6_tdm_temp_t  *tdm = SOC_CONTROL(unit)->tdm_info;

    ilen = sizeof(int);
    ovs_size = FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE *
               FIREBOLT6_OVS_GROUP_COUNT_PER_HPIPE *
               FIREBOLT6_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE *
                   _FB6_PKT_SCH_LENGTH;
    hpipes = FIREBOLT6_PIPES_PER_DEV * FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * FIREBOLT6_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * FIREBOLT6_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * FIREBOLT6_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * FIREBOLT6_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * FIREBOLT6_PBLKS_PER_DEV)     + /* port ratio */
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
    var_size = ilen * FIREBOLT6_TDM_LENGTH;
    sal_memcpy(&tdm_scache_ptr[scache_offset],
               tdm->tdm_pipe[0].idb_tdm,
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe0 */
    var_size = ilen * FIREBOLT6_TDM_LENGTH;
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
    for (phy_port = 0; phy_port < FIREBOLT6_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm_scache_ptr[scache_offset],
                   &tdm->pblk_info[phy_port].pblk_hpipe_num,
                   var_size);
        scache_offset += var_size;
    }

    /* pblk id of phy-port */
    for (phy_port = 0; phy_port < FIREBOLT6_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm_scache_ptr[scache_offset],
                   &tdm->pblk_info[phy_port].pblk_cal_idx,
                   var_size);
        scache_offset += var_size;
    }

    /* port ratio */
    var_size = ilen * FIREBOLT6_PBLKS_PER_DEV;
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
 *      soc_fb6_tdm_scache_recovery
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
int soc_fb6_tdm_scache_recovery(int unit)
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
    _soc_firebolt6_tdm_temp_t *tdm = SOC_CONTROL(unit)->tdm_info;

    ilen = sizeof(int);
    ovs_size = FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE *
               FIREBOLT6_OVS_GROUP_COUNT_PER_HPIPE *
               FIREBOLT6_OVS_GROUP_TDM_LENGTH;
    pkt_shp_size = FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE *
                   _FB6_PKT_SCH_LENGTH;
    hpipes = FIREBOLT6_PIPES_PER_DEV * FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE;

    alloc_size = (ilen * FIREBOLT6_TDM_LENGTH) + /* IDB TDM info on pipe0 */
                 (ilen * FIREBOLT6_TDM_LENGTH) + /* MMU TDM info on pipe0 */
                 (ilen * ovs_size)     + /* Oversub group info on pipe0 */
                 (ilen * pkt_shp_size)    + /* pkt shaper info on pipe0     */
                 (ilen * FIREBOLT6_PHY_PORTS_PER_DEV) + /* hpipe id of phy-port */
                 (ilen * FIREBOLT6_PHY_PORTS_PER_DEV) + /* pblk id of phy-port */
                 (ilen * FIREBOLT6_PBLKS_PER_DEV)     + /* port ratio */
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
    var_size = ilen * FIREBOLT6_TDM_LENGTH;
    sal_memcpy(tdm->tdm_pipe[0].idb_tdm,
               &tdm_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* MMU TDM info on pipe0 */
    var_size = ilen * FIREBOLT6_TDM_LENGTH;
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
    for (phy_port = 0; phy_port < FIREBOLT6_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm->pblk_info[phy_port].pblk_hpipe_num,
                   &tdm_scache_ptr[scache_offset],
                   var_size);
        scache_offset += var_size;
    }

    /* pblk id of phy-port */
    for (phy_port = 0; phy_port < FIREBOLT6_PHY_PORTS_PER_DEV; phy_port++) {
        var_size = ilen;
        sal_memcpy(&tdm->pblk_info[phy_port].pblk_cal_idx,
                   &tdm_scache_ptr[scache_offset],
                   var_size);
        scache_offset += var_size;
    }

    /* port ratio */
    var_size = ilen * FIREBOLT6_PBLKS_PER_DEV;
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

STATIC int
_soc_firebolt6_tdm_init(int unit, soc_port_schedule_state_t *port_schedule_state)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_map_type_t *in_portmap;
    int port, speed;
    int i = 0;

    if (soc->tdm_info == NULL) {
        soc->tdm_info = sal_alloc(sizeof(_soc_firebolt6_tdm_temp_t),
                                    "firbolt6 TDM info");
        if (soc->tdm_info == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(soc->tdm_info, 0, sizeof(_soc_firebolt6_tdm_temp_t));
    }
    sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_fb6_tdm_scache_recovery(unit));
        return SOC_E_NONE;
    } else {
        SOC_IF_ERROR_RETURN(
            soc_fb6_tdm_scache_allocate(unit));
    }
#endif
    /* Core clock frequency */
    port_schedule_state->frequency =
        soc_property_get(unit, spn_BCM_TDM_FREQUENCY, 0);
    port_schedule_state->bandwidth = si->bandwidth;
    port_schedule_state->io_bandwidth = si->io_bandwidth;
    port_schedule_state->lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);
    port_schedule_state->is_flexport = 0;

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

        /* Fill in the information about channelization ports. */
        if (SOC_PBMP_MEMBER(si->subtag_allowed_pbm, port)) {
            port_schedule_state->resource[i].num_subports =
                si->port_num_subport[port];
            port_schedule_state->resource[i].coe_port = 1;
            port_schedule_state->resource[i].port_lc_map =
                si->port_lc_mapping[port];

        }


        port_schedule_state->resource[i].physical_port = si->port_l2p_mapping[port];
        port_schedule_state->resource[i].logical_port = port;
        port_schedule_state->resource[i].idb_port = si->port_l2i_mapping[port];
        port_schedule_state->resource[i].mmu_port = si->port_l2i_mapping[port];
        port_schedule_state->resource[i].num_lanes = si->port_num_lanes[port];
        port_schedule_state->resource[i].speed = speed;
        port_schedule_state->resource[i].pipe = 0;
        port_schedule_state->resource[i].oversub = IS_OVERSUB_PORT(unit, port) ? 1 : 0;
        port_schedule_state->nport = ++i;
    }
    sal_memcpy(in_portmap->port_p2l_mapping, si->port_p2l_mapping,
                sizeof(int)*FIREBOLT6_TDM_PHY_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_l2p_mapping, si->port_l2p_mapping,
                sizeof(int)*_FB6_TDM_DEV_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_p2m_mapping, si->port_p2m_mapping,
                sizeof(int)*FIREBOLT6_TDM_PHY_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_m2p_mapping, si->port_m2p_mapping,
                sizeof(int)*_FB6_TDM_MMU_PORTS_PER_DEV);
    sal_memcpy(in_portmap->port_l2i_mapping, si->port_l2i_mapping,
                sizeof(int)*_FB6_TDM_DEV_PORTS_PER_DEV);
    sal_memcpy(&in_portmap->physical_pbm, &si->physical_pbm, sizeof(pbmp_t));
    sal_memcpy(&in_portmap->hg2_pbm, &si->hg.bitmap, sizeof(pbmp_t));
    sal_memcpy(&in_portmap->management_pbm, &si->management_pbm,
                sizeof(pbmp_t));
    sal_memcpy(&in_portmap->oversub_pbm, &si->oversub_pbm, sizeof(pbmp_t));

    SOC_IF_ERROR_RETURN(soc_fb6_port_schedule_tdm_init(unit, port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_fb6_tdm_init(unit, port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_firebolt6_reconfigure_ports(unit, port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_fb6_soc_tdm_update(unit, port_schedule_state));

    return SOC_E_NONE;
}

/*
 * Function:
 *     _soc_firebolt6_idb_init
 * Purpose:
 *      IDB OBM initialization OBM init
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      SOC_E_xxx
 * Notes:
 *      The OBM setting follow the guideline of FB6_OBM_settings.xlsx
 */
int
_soc_firebolt6_idb_init(int unit)
{
    uint32 rval0 = 0;
    soc_reg_t reg;

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

    _soc_fb6_set_idb_dpp_ctrl(unit);

    return SOC_E_NONE;
}

/* IDB, EDB configuration for physical ports is handled in DV flexport code */
STATIC int
_soc_firebolt6_edb_init(int unit)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    int i;

    for (i = 0; i < COUNTOF(fb6_anc_ports); i++) {
        /* Configure egress mmu cell credit */
        sal_memset(entry, 0, sizeof(egr_mmu_cell_credit_entry_t));
        soc_EGR_MMU_CELL_CREDITm_field32_set(unit, &entry, CREDITf, 11);
        SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CELL_CREDITm
            (unit, MEM_BLOCK_ALL, fb6_anc_ports[i].phy_port, &entry));

        /* Configure egress transmit start count */
        sal_memset(entry, 0, sizeof(egr_xmit_start_count_pipe0_entry_t));
        soc_EGR_XMIT_START_COUNT_PIPE0m_field32_set(unit, &entry, THRESHOLDf, 7);
        SOC_IF_ERROR_RETURN(WRITE_EGR_XMIT_START_COUNT_PIPE0m
            (unit, MEM_BLOCK_ALL, fb6_anc_ports[i].port, &entry));

        /* Enable egress */
        sal_memset(entry, 0, sizeof(egr_enable_entry_t));
        soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEm
            (unit, MEM_BLOCK_ALL, fb6_anc_ports[i].phy_port, entry));
    }
    return SOC_E_NONE;
}
/* For a given logical port, return the OBM id and the lane number
 *   OBM id is nothing but the Serdes/PortMacro number
 *   Note: HX5 has three different OBM register sets
 */
int
soc_fb6_port_obm_info_get(int unit, soc_port_t port,
        int *obm_id, int *lane)
{
    soc_info_t *si;
    soc_port_t phy_port;
    int port_block_base;

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
        return SOC_E_PARAM;
    }
    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
    port_block_base = SOC_FB6_PORT_BLOCK_BASE_PORT(port);
    if (lane != NULL) {
        *lane = (phy_port - port_block_base) / 4;
    }

    *obm_id = si->port_serdes[port];

    return SOC_E_NONE;
}

extern int
(*_phy_tsce_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);
extern int
(*_phy_tscf_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);

#define TSC_REG_ADDR_TSCID_SET(_phy_reg, _phyad)    \
                            ((_phy_reg) |= ((_phyad) & 0x1f) << 19)

/* Get port for MDIO address */
STATIC int
_soc_fb6_mdio_addr_to_port(uint32 phy_addr)
{
    int bus, offset;

    /* Must be internal MDIO address */
    if ((phy_addr & 0x80) == 0) {
        return 0;
    }

    /*
     * Internal phy address:
     * bus 0 phy 1 to 20 are mapped to Physical port 1 to 20
     * bus 1 phy 1 to 16 are mapped to Physical port 21 to 36
     * bus 2 phy 1 to 16 are mapped to Physical port 37 to 52
     * bus 3 phy 1 to 16 are mapped to Physical port 53 to 68
     * bus 6 phy : Quad PCIe Serdes.
     */
    bus = PHY_ID_BUS_NUM(phy_addr);
    if (bus > 6) {
        return 0;
    }
    switch (bus) {
    case 0: offset = 0;
        break;
    case 1: offset = 20;
        break;
    case 2: offset = 36;
        break;
    case 3: offset = 52;
        break;
    case 6: offset = 68;
        break;
    default:
        return 0;
    }

    return (phy_addr & 0x1f) + offset;
}

STATIC int
_soc_fb6_tscx_reg_read(int unit, uint32 phy_addr,
                            uint32 phy_reg, uint32 *phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_fb6_mdio_addr_to_port(phy_addr);

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_fb6_tscx_reg_read[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
    rv = soc_sbus_tsc_reg_read(unit, port, blk, phy_addr,
                               phy_reg, phy_data);
    return rv;
}

STATIC int
_soc_fb6_tscx_reg_write(int unit, uint32 phy_addr,
                             uint32 phy_reg, uint32 phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_fb6_mdio_addr_to_port(phy_addr);

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_fb6_tscx_reg_write[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
    rv = soc_sbus_tsc_reg_write(unit, port, blk, phy_addr,
                                phy_reg, phy_data);
    return rv;
}

void
soc_firebolt6_sbus_ring_map_config(int unit)
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
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_0_7_OFFSET, 0x52222100);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_8_15_OFFSET, 0x30050005);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_16_23_OFFSET, 0x44443333);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_24_31_OFFSET, 0x07776666);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_32_39_OFFSET, 0x0500007);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_40_47_OFFSET, 0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_48_55_OFFSET, 0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_56_63_OFFSET, 0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_64_71_OFFSET, 0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_72_79_OFFSET, 0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_TIMEOUT_OFFSET,0x5000);
    }
#endif

    return;
}



STATIC int
_soc_firebolt6_init_hash_control_reset(int unit)
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

    soc_mem_field32_set(unit, EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLm,
          &egr_vp_vlan_mbm_hash_control, HASH_OFFSET_DEDICATED_BANK_1f, 32);
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_VP_VLAN_MEMBERSHIP_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                     &egr_vp_vlan_mbm_hash_control));

    soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm,
          &exact_match_hash_control, HASH_TABLE_BANK_CONFIGf, 0x0);
    SOC_IF_ERROR_RETURN
        (WRITE_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &exact_match_hash_control));

    soc_mem_field32_set(unit, ING_VP_VLAN_MEMBERSHIP_HASH_CONTROLm,
          &ing_vp_vlan_mbm_hash_control, HASH_OFFSET_DEDICATED_BANK_1f, 32);

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
        (WRITE_IP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                              &ip_uat_shared_banks_control));

    SOC_IF_ERROR_RETURN
        (WRITE_EP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                              &ep_uat_shared_banks_control));

    SOC_IF_ERROR_RETURN
        (WRITE_FT_KEY_SS0_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                               &ft_key_ss0_shared_banks_control));

    SOC_IF_ERROR_RETURN
        (WRITE_FT_KEY_SS1_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                               &ft_key_ss1_shared_banks_control));

     return SOC_E_NONE;
}

int
soc_firebolt6_chip_reset(int unit)
{
#define _SOC_HX5_PLL_CFG_FIELDS 4
#define _SOC_FB6_TEMP_MAX   130
#define _SOC_FB6_DEF_TEMP_MAX 110 
    int index;
    uint32 to_usec, temp_mask;
    uint32 rval, temp_thr;
    int pm = 0, field = 0, fval = 0, reg = 0, pipe = 0;
    uint64 rval64;

    static const soc_reg_t temp_thr_reg[] = {
        TOP_PVTMON_0_INTR_THRESHOLDr, TOP_PVTMON_1_INTR_THRESHOLDr,
        TOP_PVTMON_2_INTR_THRESHOLDr, TOP_PVTMON_3_INTR_THRESHOLDr,
        TOP_PVTMON_4_INTR_THRESHOLDr, TOP_PVTMON_5_INTR_THRESHOLDr,
        TOP_PVTMON_6_INTR_THRESHOLDr, TOP_PVTMON_5_INTR_THRESHOLDr,
        TOP_PVTMON_8_INTR_THRESHOLDr
    };
    static const char *temp_thr_prop[] = {
        "temp0_threshold", "temp1_threshold", "temp2_threshold",
        "temp3_threshold", "temp4_threshold", "temp5_threshold",
        "temp6_threshold", "temp7_threshold", "temp8_threshold"
    };
    static soc_field_t egr_reset_fields[] = {
        PM0_RESETf, PM1_RESETf, PM2_RESETf, PM3_RESETf,
        PM4_RESETf, PM5_RESETf, PM6_RESETf, PM7_RESETf,
        PM8_RESETf, PM9_RESETf, PM10_RESETf, PM11_RESETf,
        PM12_RESETf, PM13_RESETf, PM14_RESETf, PM15_RESETf,
        PM16_RESETf
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
        soc_firebolt6_sbus_ring_map_config(unit);
    }
#endif

    sal_usleep(to_usec);
    /* Reset IP, EP, MMU and port macros */
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, 0x0));

    /* Configure TS PLL */
    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_8r(unit, &rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_8r, &rval, CH0_MDIVf,
                      soc_property_get(unit, spn_PTP_TS_PLL_MNDIV, 5));

    /* pm_clk25. */
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_9r, &rval, CH4_MDIVf,
                      soc_property_get(unit, spn_PTP_TS_PLL_MNDIV, 200));
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_9r(unit, rval));
    /* pm_clk125 */

    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_9r, &rval, CH5_MDIVf,
                      soc_property_get(unit, spn_PTP_TS_PLL_MNDIV, 40));
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_9r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, POST_RST_HOLD_SELf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_3r(unit,&rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, KAf,
                      soc_property_get(unit, spn_PTP_TS_KA, 0xa));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, KIf,
                      soc_property_get(unit, spn_PTP_TS_KI, 3));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, KPf,
                      soc_property_get(unit, spn_PTP_TS_KP, 0xa));
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_3r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_0r(unit,&rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_0r, &rval, PDIVf,
                      soc_property_get(unit, spn_PTP_TS_PLL_PDIV, 1));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_0r, &rval, NDIV_INTf,
                      soc_property_get(unit, spn_PTP_TS_PLL_N, 100));
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_0r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_1r(unit,&rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_1r, &rval, NDIV_FRAC_MODE_SELf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_1r(unit, rval));

    /* Set 250Mhz (implies 4ns resolution) default timesync clock to
       calculate assymentric delays */
    SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nanoseconds */

    /* Configure BS PLL */
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_8r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_8r, &rval, CH0_MDIVf,
                      soc_property_get(unit, spn_PTP_BS_MNDIV, 250));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_8r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_8r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_2r, &rval, POST_RST_HOLD_SELf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_2r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, KAf,
                      soc_property_get(unit, spn_PTP_BS_KA, 0xa));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, KIf,
                      soc_property_get(unit, spn_PTP_BS_KI, 3));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, KPf,
                      soc_property_get(unit, spn_PTP_BS_KP, 0xa));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_3r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_3r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_0r, &rval, PDIVf,
                      soc_property_get(unit, spn_PTP_BS_PDIV, 1));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_0r(unit, rval));

    /* bs_ndiv_low maps to lower 32 bits (i.e., first 32bits) of the 48 bit Ndiv */
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_9r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_9r, &rval, PLL_FBDIV_0f, 0x0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_9r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_9r(unit, rval));

    /* bs_ndiv_high maps to higher 16 bits (i.e., last 16bits) of the 48 bit Ndiv */
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_10r(unit,&rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_10r, &rval, PLL_FBDIV_1f, 0x0E00);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_10r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_10r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, NDIV_FRAC_MODE_SELf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_1r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_1r(unit, rval));

    /* Bring LCPLL, time sync PLL, BroadSync PLL, AVS out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_IPROC_PLL_STRAP_BYP_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_IPROC_PLL_STRAP_BYP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_CORE_PLL0_STRAP_BYP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_AVS_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MAX_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MIN_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_CORE_PLL_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_CORE_PLL_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TS_PLL_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TS_PLL_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL1_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL0_POST_RST_Lf, 1);

    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL0_RST_Lf, 1);

    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);
    if (!SAL_BOOT_SIMULATION) {
        /* Check time sync lock status */
        reg = TOP_TS_PLL_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (!soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TS_PLL not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
        /* Check BroadSync lock status */
        reg = TOP_BS_PLL0_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (!soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BS_PLL0 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
        /* Check BroadSync lock status */
        reg = TOP_BS_PLL1_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (!soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BS_PLL1 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
    }
    /* De-assert BS PLL */
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL1_POST_RST_Lf, 1);

    /* De-assert PMs post reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_3r(unit, &rval));

    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM16_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM15_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM14_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM13_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM12_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM11_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM10_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM9_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM8_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM7_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM6_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM5_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM4_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM3_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval,
                      TOP_PM0_RST_Lf, 1);

    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, rval));

    sal_usleep(to_usec);

    /* Enable high temperature interrupt monitoring.
     * Default on: pvtmon6 (close to core_plls at center of die).
     */
    temp_mask = soc_property_get(unit, "temp_monitor_select", 1<<6);
    /* The above is a 9 bit mask to indicate which temp sensor to hook up to
     * the interrupt.
     */
    rval = 0;
    for (index = 0; index <COUNTOF(temp_thr_reg); index++) {
        uint32 trval;

        /*
        * Sensor group for hardware reset:
        * 0, 2, 3, 4 and 6.
        *
        * Sensor group for interrupt:
        * 1 ,5, and 7.
        */
        /* Temp = 457.82 - o_ADC_data * 0.55702
         * data = 457.82-(temp/0.55702) = (457820-(temp*1000))/557
         * Note: Since this is a high temp value we can safely assume it to
         * always be a +ive number. This is in degrees celcius.
         */
        temp_thr = soc_property_get(unit, temp_thr_prop[index],
                   ((index == 0) || (index == 2) || (index == 3) || (index == 4) || (index == 6)) ? \
                   _SOC_FB6_TEMP_MAX : _SOC_FB6_DEF_TEMP_MAX);
        if (temp_thr > _SOC_FB6_TEMP_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Unsupported temp value %d !! Max %d. Using %d.\n"),
                       temp_thr, _SOC_FB6_TEMP_MAX, _SOC_FB6_DEF_TEMP_MAX));
            temp_thr = _SOC_FB6_DEF_TEMP_MAX;
        }
        /* Convert temperature to config data */
        temp_thr = (457820-(temp_thr*1000))/557;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, temp_thr_reg[index], REG_PORT_ANY, 0, &trval));
        soc_reg_field_set(unit, temp_thr_reg[index], &trval, MIN_THRESHOLDf,
                          temp_thr);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, temp_thr_reg[index], REG_PORT_ANY, 0, trval));
        if ((temp_mask & (1 << index)) && (index != 8)) {
            rval |= (1 << ((index * 2) + 1)); /* 2 bits per pvtmon, using min */
        }
    }
    _soc_fb6_temp_mon_mask[unit] = temp_mask;

    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_INTR_MASKr(unit, rval));

    /* Clear interrupt latch bits */
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_INTR_STATUSr(unit, 0xffffffff));
    /*
     * Enable HW PVTMON high temperature protection by setting
     * hightemp_ctrl_en field for pvtmon 0, 2, 3, 4 and 6.
     */
    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, HIGHTEMP_RESET_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON0_HIGHTEMP_CTRL_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON2_HIGHTEMP_CTRL_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON3_HIGHTEMP_CTRL_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON4_HIGHTEMP_CTRL_ENf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON6_HIGHTEMP_CTRL_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    /* Enable temp mon interrupt */
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

   /* Enable IPEP clock gating by default for saving IDLE power.
   *  IPEP clock gating should be disabled if visibility feature support is needed. */
   if (!soc_property_get(unit, spn_IPEP_CLOCK_GATING_DISABLE, 0)) {
        /* Need to set PSG/PCG to 0 before de-assert IP/EP/MMU SOFT reset  */
        SOC_IF_ERROR_RETURN(WRITE_TOP_CLOCKING_ENFORCE_PSGr(unit, 0));
        SOC_IF_ERROR_RETURN(WRITE_TOP_CLOCKING_ENFORCE_PCGr(unit, 0));
    }

    /* Bring IP, EP, MMU and port macros out of reset */
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, 0x3fff));
    sal_usleep(to_usec);

    /* Reset egress hardware resource */
    reg = SOC_REG_UNIQUE_ACC(unit, EGR_PORT_BUFFER_SFT_RESET_0r)[pipe];

    SOC_IF_ERROR_RETURN
        (soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval64));
    for ( pm = 0; pm < sizeof(egr_reset_fields)/sizeof(soc_field_t); pm++) {
        field = egr_reset_fields[pm];
        fval = 4;
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
    SOC_IF_ERROR_RETURN(_soc_firebolt6_init_hash_control_reset(unit));
    SOC_IF_ERROR_RETURN(soc_firebolt6_uft_uat_config(unit));
    SOC_IF_ERROR_RETURN(_soc_firebolt6_ft_bank_config(unit));

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(_soc_firebolt6_clear_all_memory(unit));
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

STATIC int
_soc_firebolt6_misc_init(int unit)
{
    soc_control_t *soc;
    soc_info_t *si;
    soc_mem_t mem;
    uint64 rval64;
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    soc_pbmp_t pbmp;
    int port, block_info_idx;
    uint32 parity_enable;
    modport_map_subport_entry_t map_entry;

    uint32 mc_ctrl = 0;
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    int num_overlay_ecmp_rh_flowset_entries;
    int ecmp_levels = 1;
#endif
    int num_lag_rh_flowset_entries;
    int fabric_port_enable = 1;

    soc = SOC_CONTROL(unit);
    si = &SOC_INFO(unit);

    /* Stop the mem scan task before all of the parity init takes place */
    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));
    SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_stop(unit));

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify
        (unit, AUX_ARB_CONTROLr, REG_PORT_ANY, FP_REFRESH_ENABLEf, parity_enable));

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

            /* Certain mems/regs need to be cleared before enabling SER */
            SOC_IF_ERROR_RETURN(soc_trident3_clear_mmu_memory(unit));
        }
        soc_ser_alpm_cache_check(unit);
        soc_ser_log_init(unit, NULL, 0);

#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            /* PVT interrupt re-enable - this needs to touch hardware register */
            soc_trident3_pvt_intr_enable(unit);
        }
#endif
        SOC_IF_ERROR_RETURN(soc_fb6_ser_enable_all(unit, TRUE));

        
        SOC_IF_ERROR_RETURN(READ_EGR_EPMOD_SER_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, EGR_EPMOD_SER_CONTROLr, &rval, TIMESTAMP_BUFFER_ECC_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_EGR_EPMOD_SER_CONTROLr(unit, rval));

        soc_fb6_ser_register(unit);

        /*Initialize chip-specific functions for SER testing*/
        soc_fb6_ser_test_register(unit);
    }

    SOC_IF_ERROR_RETURN(_soc_trident3_init_mmu_memory(unit));
    SOC_IF_ERROR_RETURN(READ_MMU_GCFG_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, PARITY_ENf, parity_enable ? 1 : 0);
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, REFRESH_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));


    SOC_IF_ERROR_RETURN(soc_firebolt6_uft_uat_config(unit));
    SOC_IF_ERROR_RETURN(_soc_firebolt6_ft_bank_config(unit));

    soc_cancun_init(unit);
    _soc_firebolt6_latency_config(unit);
    SOC_IF_ERROR_RETURN(_soc_firebolt6_port_mapping_init(unit));

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
        int rv = BCM_E_NONE;
        soc_port_schedule_state_t *port_schedule_state;
        if (soc->tdm_info == NULL) {
            soc->tdm_info = sal_alloc(sizeof(_soc_firebolt6_tdm_temp_t),
                                      "firebolt6 TDM info");
            if (soc->tdm_info == NULL) {
                return SOC_E_MEMORY;
            }
        }
        port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                        "Firebolt6 soc_port_schedule_state_t");
        if (port_schedule_state == NULL) {
            return SOC_E_MEMORY;
        }

        rv = _soc_firebolt6_tdm_init(unit, port_schedule_state);
        sal_free(port_schedule_state);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
    }

    SOC_IF_ERROR_RETURN(_soc_firebolt6_idb_init(unit));
    SOC_IF_ERROR_RETURN(_soc_firebolt6_edb_init(unit));
    sal_memset(entry, 0, sizeof(cpu_pbm_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBMm, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBMm, MEM_BLOCK_ALL, 0, entry));

    sal_memset(entry, 0, sizeof(cpu_pbm_2_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBM_2m, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBM_2m, MEM_BLOCK_ALL, 0, entry));

    sal_memset(entry, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
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

    /* Enable dual hash tables */
    SOC_IF_ERROR_RETURN(soc_trident3_hash_init(unit));


    sal_memset(&map_entry, 0, sizeof(map_entry));
    soc_mem_field32_set(unit, MODPORT_MAP_SUBPORTm, &map_entry, ENABLEf, 1);

    /* my_modid and other modid related initialization */
    PBMP_ALL_ITER(unit, port) {
        /* configure logical port numbers */
        soc_mem_field32_set(unit, MODPORT_MAP_SUBPORTm, &map_entry,
                            DESTf, port);
        soc_mem_field32_set(unit, MODPORT_MAP_SUBPORTm, &map_entry,
                            DST_SUBPORT_NUMf, port);

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

    /*FB6FIXME:
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
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROLr,
                        REG_PORT_ANY, ECMP_FLOWSET_TABLE_CONFIGf, 1));
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    ecmp_levels = soc_property_get(unit, spn_L3_ECMP_LEVELS, 1);
    if ((soc_feature(unit, soc_feature_riot) ||
        soc_feature(unit, soc_feature_multi_level_ecmp)) &&
        (ecmp_levels > 1) && soc_mem_index_count(unit, RH_ECMP_FLOWSETm)) {


        num_overlay_ecmp_rh_flowset_entries = soc_property_get(unit,
                spn_RIOT_OVERLAY_ECMP_RESILIENT_HASH_SIZE, 0);

        if (SOC_FIELD_RANGE_CHECK(
            num_overlay_ecmp_rh_flowset_entries, 0, FB6_RH_ECMP_THRESHOLD)) {
            num_overlay_ecmp_rh_flowset_entries = FB6_RH_ECMP_THRESHOLD;
        }

        switch (num_overlay_ecmp_rh_flowset_entries) {
            case 0:
                break;
            case FB6_RH_ECMP_THRESHOLD:
                if (soc_mem_index_count(unit,RH_ECMP_FLOWSETm) >= FB6_RH_ECMP_THRESHOLD) {
                    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                        ENHANCED_HASHING_CONTROLr,
                                        REG_PORT_ANY,
                                        ECMP_FLOWSET_TABLE_CONFIGf, 2));
                } else {
                    LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "%s:Total ECMP entries %d are less than RH entries :%d \n"),
                    FUNCTION_NAME(),
                    soc_mem_index_count(unit,RH_ECMP_FLOWSETm),
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
            spn_TRUNK_RESILIENT_HASH_TABLE_SIZE, FB6_RH_LAG_THRESHOLD);

    switch (num_lag_rh_flowset_entries) {
        case 0:
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                ENHANCED_HASHING_CONTROL_2r,
                                REG_PORT_ANY, HGT_LAG_FLOWSET_TABLE_CONFIGf,
                                1));
            break;
        case FB6_RH_LAG_FLOWSET_SIZE:
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                ENHANCED_HASHING_CONTROL_2r,
                                REG_PORT_ANY, HGT_LAG_FLOWSET_TABLE_CONFIGf,
                                0));
            break;
        case FB6_RH_LAG_THRESHOLD:
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                ENHANCED_HASHING_CONTROL_2r,
                                REG_PORT_ANY, HGT_LAG_FLOWSET_TABLE_CONFIGf,
                                2));
            break;
        default:
            return SOC_E_CONFIG;
    }
    SOC_IF_ERROR_RETURN
        (soc_counter_tomahawk_status_enable(unit, TRUE));

    SOC_IF_ERROR_RETURN(READ_MMU_FC_OFFSETr(unit, &rval));
    soc_reg_field_set(unit, MMU_FC_OFFSETr, &rval, E2ECC_OFFSETf, 18);
    SOC_IF_ERROR_RETURN(WRITE_MMU_FC_OFFSETr(unit, rval));

    /* Initialize FC_ETHERTYPE to default value of 0x8808 */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, FC_CONFIG_1m,
                                     MEM_BLOCK_ANY, 0, &entry));
    soc_mem_field32_set(unit, FC_CONFIG_1m, &entry, FC_ETHERTYPEf,
                           0x8808);
    soc_mem_field32_set(unit, FC_CONFIG_1m, &entry, PAUSE_OPCODEf, 0x1);
    soc_mem_field32_set(unit, FC_CONFIG_1m, &entry, PFC_OPCODEf, 0x5);
    soc_mem_field32_set(unit, FC_CONFIG_1m, &entry, E2ECC_OPCODEf, 0xa);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, FC_CONFIG_1m,
                                      MEM_BLOCK_ANY, 0, &entry));

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, FC_CONFIG_2m,
                                     MEM_BLOCK_ANY, 0, &entry));
    soc_mem_field32_set(unit, FC_CONFIG_2m, &entry, FC_ETHERTYPEf,
                           0x8808);
    soc_mem_field32_set(unit, FC_CONFIG_2m, &entry, PAUSE_OPCODEf, 0x1);
    soc_mem_field32_set(unit, FC_CONFIG_2m, &entry, PFC_OPCODEf, 0x5);
    soc_mem_field32_set(unit, FC_CONFIG_2m, &entry, E2ECC_OPCODEf, 0xa);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, FC_CONFIG_2m,
                                      MEM_BLOCK_ANY, 0, &entry));
#ifdef BCM_UTT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(soc_utt_ifp_bank_config(unit));
        SOC_IF_ERROR_RETURN(soc_utt_lpm_bank_config(unit,
                            SOC_URPF_STATUS_GET(unit) ? 1 : 0,
                            SOC_L3_DEFIP_UTT_PAIRED_TCAMS_GET(unit)));
    }
#endif

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MISC Init completed (u=%d)\n"), unit));
    SOC_IF_ERROR_RETURN(
        soc_firebolt6_num_cosq_init(unit));
    /* Mem Scan thread start should be the last step in Misc init */
    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));
        SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_start(unit));
    }

    /* LEDuP init */
    SOC_IF_ERROR_RETURN(soc_firebolt6_ledup_init(unit));

#ifdef INCLUDE_AVS
    /* Register firebolt6 related functions */
        SOC_IF_ERROR_RETURN(soc_fb6_avs_init(unit));
#endif /* INCLUDE_AVS  */

    return SOC_E_NONE;
}

/*
 * Function used for global resource reservation (MCQE / RQE / Buf cell).
 * Input state: Total hardware resource.
 * Output state: Resource available for admission control.
 */
STATIC void
_soc_fb6_mmu_config_dev_reserve(int unit, _soc_mmu_device_info_t *devcfg,
                                int lossless)
{
    int port, pm, asf_rsvd = 0;
    uint32 total_mcq = 0, num_ports = 0;
    uint32 cpu_cnt = 1, lb_cnt = 1, i;
    soc_info_t *si = &SOC_INFO(unit);

    /* Device reservation for RQE Entry */
    devcfg->total_rqe_queue_entry -= _FB6_MMU_RQE_ENTRY_RSVD_PER_XPE;

    /* Device reservation for MCQ Entry - 6 entries for each MC queue */
    if (si->flex_eligible) {
        for (pm = 0; pm < _FB6_PBLKS_PER_DEV(unit); pm++) {
            num_ports += soc_fb6_ports_per_pm_get(unit, pm);
        }
        total_mcq += (num_ports * SOC_FB6_NUM_MCAST_QUEUE_PER_PORT);
    } else {
        PBMP_ALL_ITER(unit, port) {
            if (IS_CPU_PORT(unit, port) ||
                IS_LB_PORT(unit,port)) {
                continue;
            }
            if (si->port_num_subport[port] > 0) {
               for (i = 0; i < si->port_num_subport[port]; i++) {
                   total_mcq += si->subport_cosq_uc_num[port][i];
               }
            } else {
                total_mcq += si->port_num_cosq[port];
            }
        }
    }
    total_mcq += ((cpu_cnt * SOC_FB6_NUM_CPU_QUEUES) +
                  (lb_cnt * 10));

    devcfg->total_mcq_entry -= ((total_mcq * 6) +
                                _FB6_TDM_MCQE_RSVD_OVERSHOOT);

    devcfg->mmu_total_cell -= asf_rsvd;
}

STATIC void
_soc_fb6_mmu_init_dev_config(int unit, _soc_mmu_device_info_t *devcfg,
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

    devcfg->max_pkt_byte = _FB6_MMU_MAX_PACKET_BYTES;
    devcfg->mmu_hdr_byte = _FB6_MMU_PACKET_HEADER_BYTES;
    devcfg->jumbo_pkt_size = _FB6_MMU_JUMBO_FRAME_BYTES;
    devcfg->default_mtu_size = _FB6_MMU_DEFAULT_MTU_BYTES;
    devcfg->mmu_cell_size = _FB6_MMU_BYTES_PER_CELL;
    devcfg->mmu_total_cell = _FB6_MMU_TOTAL_CELLS_PER_XPE;
    devcfg->num_pg = _FB6_MMU_NUM_PG;
    devcfg->num_service_pool = _FB6_MMU_NUM_POOL;
    devcfg->flags = SOC_MMU_CFG_F_PORT_MIN | SOC_MMU_CFG_F_PORT_POOL_MIN |
                    SOC_MMU_CFG_F_RQE | SOC_MMU_CFG_F_EGR_MCQ_ENTRY;
    devcfg->total_mcq_entry = _FB6_MMU_TOTAL_MCQ_ENTRY(unit);
    devcfg->rqe_queue_num = 11;
    devcfg->total_rqe_queue_entry = _FB6_MMU_TOTAL_RQE_ENTRY(unit);

    _soc_fb6_mmu_config_dev_reserve(unit, devcfg, lossless);
}

STATIC void
_soc_fb6_mmu_sw_info_config (int unit, _soc_fb6_mmu_sw_config_info_t * swcfg,
                             int lossless)
{
    /* Default settings is lossless */
    swcfg->mmu_egr_queue_min = 0;
    swcfg->mmu_egr_qgrp_min = 0;
    swcfg->mmu_total_pri_groups = 8;
    swcfg->mmu_active_pri_groups = 1;
    swcfg->mmu_pg_min = 7;
    swcfg->mmu_port_min = 0;
    swcfg->mmu_mc_egr_qentry_min = 0;
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
_soc_fb6_default_pg_headroom(int unit, soc_port_t port,
                            int lossless)
{
    int speed = 1000, hdrm = 0;

    if (IS_CPU_PORT(unit, port)) {
        return 69;
    } else if (!lossless) {
        return 0;
    } else if (IS_LB_PORT(unit, port)) {
        return 160;
    }


    speed = SOC_INFO(unit).port_speed_max[port];

    if (speed == 1000) {
        hdrm = 118;
    } else if (speed == 2500) {
        hdrm = 120;
    }else if (speed == 5000) {
        hdrm = 151;
    } else if ((speed >= 10000) && (speed <= 11000)) {
        hdrm = 194;
    } else if ((speed >= 20000) && (speed <= 21000)) {
        hdrm = 231;
    } else if ((speed >= 25000) && (speed <= 27000)) {
        hdrm = 288;
    } else if ((speed >= 40000) && (speed <= 42000)) {
        hdrm =  354;
    } else if ((speed >= 50000) && (speed <= 53000)) {
        hdrm =  431;
    } else if (speed >= 100000) {
        hdrm =  766;
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
_soc_fb6_mmu_config_buf_phase1(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg,
                               _soc_fb6_mmu_sw_config_info_t *swcfg,
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

    total_pool_size = devcfg->mmu_total_cell + asf_rsvd; /* Add back ASF reserved. */

    buf->headroom = max_packet_cells;   /* 1 packet per pipe. */

    mcq_entry_shared_total = devcfg->total_mcq_entry;

    rqe_entry_shared_total = devcfg->total_rqe_queue_entry -
        (_FB6_MMU_NUM_RQE_QUEUES * swcfg->mmu_rqe_qentry_min);

    for (idx = 0; idx < _FB6_MMU_NUM_POOL; idx++) {
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

    for (idx = 0; idx < SOC_FB6_MMU_CFG_QGROUP_MAX; idx++) {
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
        for (idx = 0; idx < _HX5_MMU_NUM_INT_PRI; idx++) {
            buf_port->pri_to_prigroup[idx] = 7;
        }

        /* priority group to pool mapping */
        for (idx = 0; idx < _FB6_MMU_NUM_PG; idx++) {
            buf_port->prigroups[idx].pool_idx = 0;
        }

        for (idx = 0; idx < _FB6_MMU_NUM_POOL; idx++) {
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
        for (idx = 0; idx < _FB6_MMU_NUM_PG; idx++) {
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
                    _soc_fb6_default_pg_headroom(unit, port, lossless);
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
    for (idx = 0; idx < _FB6_MMU_NUM_RQE_QUEUES; idx++) {
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
_soc_fb6_mmu_config_buf_phase2(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg,
                               _soc_fb6_mmu_sw_config_info_t *swcfg,
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
    int sub_port_config = 0;
    uint32 color_limit = 0;
    uint32 num_ports = 0, mgmt_cnt = 0, cpu_cnt = 0, lb_cnt = 0;
    uint32 cpu_hdrm = 0, lb_hdrm = 0, mgmt_hdrm = 0;

    si = &SOC_INFO(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Initializing default MMU config phase 2(u=%d)\n"), unit));

    buf_pool = &buf->pools[0];
    total_pool_size = devcfg->mmu_total_cell; /* per XPE limit */
    total_rsvd_qmin = swcfg->mmu_egr_queue_min * (SOC_FB6_NUM_CPU_QUEUES +
                        _FB6_MMU_NUM_LBK_QUEUES );

    PBMP_ALL_ITER(unit, port) {
        if (IS_FAE_PORT(unit, port)) {
            continue;
        }
        if (IS_CPU_PORT(unit, port)) {
            cpu_cnt++;
            cpu_hdrm = _soc_fb6_default_pg_headroom(unit, port, lossless);
        } else if (IS_LB_PORT(unit,port)) {
            lb_cnt++;
            lb_hdrm = _soc_fb6_default_pg_headroom(unit, port, lossless);
        } else {
            if(si->port_num_subport[port]) {
                num_ports += si->port_num_subport[port];
                if(!sub_port_config) {
                    sub_port_config = 1;
                }
            } else {
                if (IS_MGMT_PORT(unit, port)) {
                    mgmt_cnt++;
                } else {
                num_ports++;
                }
            }
        }
    }
    if (si->flex_eligible) {
        num_ports = 0;
        for (pm = 0; pm < _FB6_PBLKS_PER_DEV(unit); pm++) {
            port = soc_fb6_ports_per_pm_get(unit, pm);
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

    if (sub_port_config) {
        egr_rsvd_total += total_rsvd_qmin +
            ((num_ports + mgmt_cnt) * (swcfg->mmu_egr_qgrp_min * 2)) +
            (swcfg->mmu_rqe_queue_min * _FB6_MMU_NUM_RQE_QUEUES);
    } else {
        egr_rsvd_total += total_rsvd_qmin +
            ((num_ports + mgmt_cnt) * swcfg->mmu_egr_qgrp_min) +
            (swcfg->mmu_rqe_queue_min * _FB6_MMU_NUM_RQE_QUEUES);
    }

    if (lossless) {
        ing_rsvd_total += egr_rsvd_total;
    }

    buf_pool->mcq_entry_reserved = _FB6_MAX_MMU_MCQE_PER_XPE * 4;


    ing_shared_total = total_pool_size - ing_rsvd_total;
    egr_shared_total = total_pool_size - egr_rsvd_total;

    swcfg->ing_shared_total = ing_shared_total;
    swcfg->egr_shared_total = egr_shared_total;

    LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "MMU config: Total Shared size: ingress %d egress %d\n"),
                            ing_shared_total, egr_shared_total));

    color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;

    for (idx = 0; idx < SOC_FB6_MMU_CFG_QGROUP_MAX; idx++) {
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
        for (idx = 0; idx < _FB6_MMU_NUM_PG; idx++) {
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
    for (idx = 0; idx < _FB6_MMU_NUM_RQE_QUEUES; idx++) {
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
_soc_fb6_pool_scale_to_limit(int size, int scale)
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
_soc_fb6_mmu_config_buf_set_hw_port(int unit, int port, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless,
                               _soc_fb6_mmu_sw_config_info_t *swcfg)
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
    int base, numq, enable, lport = 0;
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
    for (idx = 0; idx < _FB6_MMU_NUM_PG; idx++) {
        soc_reg_field_set(unit, THDI_PORT_PG_SPIDr, &rval,
                          prigroup_spid_field[idx],
                          buf_port->prigroups[idx].pool_idx);
    }
    SOC_IF_ERROR_RETURN(WRITE_THDI_PORT_PG_SPIDr(unit, port, rval));

    
    rval = 0;
    for (idx = 0; idx < _FB6_MMU_NUM_PG; idx++) {
        soc_reg_field_set(unit, THDI_HDRM_PORT_PG_HPIDr, &rval,
                          prigroup_hpid_field[idx],
                          buf_port->prigroups[idx].pool_idx);
    }
    SOC_IF_ERROR_RETURN(WRITE_THDI_HDRM_PORT_PG_HPIDr(unit, port, rval));

    /* Per port per pool settings */
    mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdi_port_mem[0])[pipe];
    if (mem0 != INVALIDm) {
        for (idx = 0; idx < _FB6_MMU_NUM_POOL; idx++) {
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
    for (idx = 0; idx < _FB6_MMU_NUM_PG; idx++) {
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
        for (idx = 0; idx < _FB6_MMU_NUM_PG; idx++) {
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
    for (idx = 0; idx < _FB6_MMU_NUM_POOL; idx++) {
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

        limit = _FB6_MMU_TOTAL_MCQ_ENTRY(unit);

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

            if (buf_queue->qgroup_id >= 0 && !lossless) {
                soc_mem_field32_set(unit, mem2, entry0, QGROUP_VALIDf, 1);
                if (buf_queue->qgroup_min_enable) {
                    soc_mem_field32_set(unit, mem2, entry0, USE_QGROUP_MINf, 1);
                }
            }
            if (buf_queue->discard_enable && !lossless) {
                soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_ENABLEf, 1);
            }

            soc_mem_field32_set(unit, mem2, entry0, Q_SPIDf, buf_queue->pool_idx);

            if ((si->port_num_subport[port] > 0)) {
                if ((idx && ((idx % 8) == 0)) || (lport != port)) {
                    prio_low += 2;
                    prio_high += 2;
                }

                if ( (idx % 8) <= 2) {
                    soc_mem_field32_set(unit, mem2, entry0, QGROUPf, prio_low);
                } else if ((idx % 8) > 2 && (idx % 8) < 8){
                    soc_mem_field32_set(unit, mem2, entry0, QGROUPf, prio_high);
                }

                lport = port;
            }
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL, base + idx,
                                entry0));
        }
    }

    /* Per  port per pool unicast */
    for (idx = 0; idx < _FB6_MMU_NUM_POOL; idx++) {
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
_soc_fb6_mmu_config_buf_set_hw_global(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless,
                               _soc_fb6_mmu_sw_config_info_t *swcfg)
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
    fval = _FB6_MMU_PHYSICAL_CELLS_PER_XPE - _FB6_MMU_RSVD_CELLS_CFAP_PER_XPE;

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
                      _FB6_MMU_CFAP_BANK_FULL_LIMIT);
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

    for (idx = 0; idx < _FB6_MMU_NUM_POOL; idx++) {
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
    for (idx = 0; idx < _FB6_MMU_NUM_POOL; idx++) {
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
        limit = buf_pool->total_mcq_entry;

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
        for (idx = 0; idx < SOC_FB6_MMU_CFG_QGROUP_MAX; idx++) {
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
                pval = _soc_fb6_pool_scale_to_limit(queue_grp->pool_limit,
                          queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_RED_CELLf, sal_ceil_func(pval, 8));

            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_RED_CELLf,
                                    sal_ceil_func(queue_grp->red_limit, 8));
            }

            if (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                pval = _soc_fb6_pool_scale_to_limit(queue_grp->pool_limit,
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
    for (idx = 0; idx < _FB6_MMU_NUM_RQE_QUEUES; idx++) {
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
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                              SHARED_ALPHAf, buf_rqe_queue->pool_scale);
        } else {
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                              SHARED_LIMITf, buf_rqe_queue->pool_limit);
        }
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                          RESET_OFFSETf, 2);

        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG1_PRIQr(unit, idx, rval));
        COMPILER_64_SET(rval64, 0, rval2);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG_PRIQr(unit, idx, rval64));
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
    for (idx = 0; idx < _FB6_MMU_NUM_POOL; idx++) {
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
_soc_fb6_mmu_config_buf_set_hw(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless,
                               _soc_fb6_mmu_sw_config_info_t *swcfg)
{
    int port;

    SOC_IF_ERROR_RETURN
        (_soc_fb6_mmu_config_buf_set_hw_global(unit, buf, devcfg, lossless,
                                               swcfg));

    PBMP_ALL_ITER(unit, port) {
       SOC_IF_ERROR_RETURN
            (_soc_fb6_mmu_config_buf_set_hw_port(unit, port, buf, devcfg,
                                                 lossless, swcfg));
    }

    return SOC_E_NONE;
}

int
soc_fb6_mmu_config_port_init(int unit, int port)
{
    int rv;
    int lossless = 1;
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_device_info_t devcfg;
    _soc_fb6_mmu_sw_config_info_t swcfg;

    buf = soc_mmu_cfg_alloc(unit);
    if (!buf) {
        return SOC_E_MEMORY;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    _soc_fb6_mmu_init_dev_config(unit, &devcfg, lossless);
    _soc_fb6_mmu_sw_info_config (unit, &swcfg, lossless);


    /* Calculate the buf - global as well as per port
     * but _soc_fb6_mmu_config_buf_set_hw_port is only called
     * for that port - since it is flex operation - we don't
     * touch any other port */
    _soc_fb6_mmu_config_buf_phase1(unit, buf, &devcfg, &swcfg, lossless);

    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        /* Override default config */
        _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
    }
    rv = _soc_mmu_cfg_buf_check(unit, buf, &devcfg);
    if (SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU config: Use default setting\n")));
        _soc_fb6_mmu_config_buf_phase1(unit, buf, &devcfg, &swcfg, lossless);
        SOC_IF_ERROR_RETURN
            (_soc_mmu_cfg_buf_calculate(unit, buf, &devcfg));
    }

    _soc_fb6_mmu_config_buf_phase2(unit, buf, &devcfg, &swcfg, lossless);
    /* Override default phase 2 config */
    if (SOC_SUCCESS(rv)) {
        if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
            _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
        }
    }
    rv = _soc_fb6_mmu_config_buf_set_hw_port(unit, port, buf, &devcfg,
                                             lossless, &swcfg);

    soc_mmu_cfg_free(unit, buf);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));
    return rv;
}

STATIC int
soc_fb6_mmu_config_init(int unit, int test_only)
{
    int rv;
    int lossless = 1;
    _soc_mmu_cfg_buf_t *buf = NULL;
    _soc_mmu_cfg_buf_t *buf_canned = NULL;
    _soc_mmu_cfg_buf_t *buf_user = NULL;
    _soc_mmu_device_info_t devcfg;
    _soc_fb6_mmu_sw_config_info_t swcfg;

    buf_canned = soc_mmu_cfg_alloc(unit);
    if (!buf_canned) {
        return SOC_E_MEMORY;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    _soc_fb6_mmu_init_dev_config(unit, &devcfg, lossless);
    _soc_fb6_mmu_sw_info_config (unit, &swcfg, lossless);

    _soc_fb6_mmu_config_buf_phase1(unit, buf_canned, &devcfg, &swcfg, lossless);
        /* coverity[CHECKED_RETURN: FALSE] */
    rv = _soc_mmu_cfg_buf_check(unit, buf_canned, &devcfg);
    _soc_fb6_mmu_config_buf_phase2(unit, buf_canned, &devcfg, &swcfg, lossless);
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

    rv = _soc_fb6_mmu_config_buf_set_hw(unit, buf, &devcfg, lossless, &swcfg);

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
_soc_firebolt6_mmu_init(int unit)
{
    uint32 pipe_map, rval = 0;
    int pipe, port;
    uint64 enable[NUM_PIPE(unit)];
    uint64 enable64;
    int test_only, i;
    int fval = 144;
    soc_info_t *si;
    int mmu_port = 0, phy_port = 0;
    uint64 rval1, rval2;
    uint32 data_lo=0, data_hi=0;
    uint32 bit_pos = 0;
    soc_reg_t uc_queue[2] = {
        UC_EXTND_COS_MODE_SPLIT0r,
        UC_EXTND_COS_MODE_SPLIT1r
    };
    soc_reg_t mc_queue[2] = {
        MC_EXTND_COS_MODE_SPLIT0r,
        MC_EXTND_COS_MODE_SPLIT1r
    };
    soc_reg_t reg;
    soc_reg_t  shaper_reg[8] = {
        MTRO_FAST_SHAPE_ENABLE_L0_SPLIT0r,
        MTRO_FAST_SHAPE_ENABLE_L0_SPLIT1r,
        MTRO_FAST_SHAPE_ENABLE_L0_SPLIT2r,
        MTRO_FAST_SHAPE_ENABLE_L0_SPLIT3r,
        MTRO_FAST_SHAPE_ENABLE_L0_SPLIT4r,
        MTRO_FAST_SHAPE_ENABLE_L0_SPLIT5r,
        MTRO_FAST_SHAPE_ENABLE_L0_SPLIT6r,
        MTRO_FAST_SHAPE_ENABLE_L0_SPLIT7r
    };

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
    } else
#endif
    {
        si = &SOC_INFO(unit);
        prio_low = 0;
        prio_high = 1;
        soc_trident3_pipe_map_get(unit, &pipe_map);

        test_only = (SAL_BOOT_XGSSIM) ? TRUE : FALSE;
        SOC_IF_ERROR_RETURN(soc_fb6_mmu_config_init(unit, test_only));
        if (SOC_PBMP_NOT_NULL(si->oversub_pbm)) {
            /* MMU_1DBG_C reg configs */
            SOC_IF_ERROR_RETURN
                (soc_trident3_sc_reg32_get(unit, MMU_1DBG_Cr, -1, -1, 0, &rval));
            soc_reg_field_set(unit, MMU_1DBG_Cr, &rval, FIELD_Af, 1);
            soc_reg_field_set(unit, MMU_1DBG_Cr, &rval, DI_ACT_THRf, 20);
            SOC_IF_ERROR_RETURN
                (soc_trident3_sc_reg32_set(unit, MMU_1DBG_Cr, -1, -1, 0, rval));

            /* MMU_1DBG_C reg configs */
            rval = 0;
            SOC_IF_ERROR_RETURN
                (soc_trident3_sc_reg32_get(unit, MMU_DI_THRr, -1, -1, 0, &rval));
            soc_reg_field_set(unit, MMU_DI_THRr, &rval, OS_SLT_LMTf, 1000);
            SOC_IF_ERROR_RETURN
                (soc_trident3_sc_reg32_set(unit, MMU_DI_THRr, -1, -1, 0, rval));

            /* MMU_2DBG_C_0 reg configs */
            rval = 0;
            SOC_IF_ERROR_RETURN
                (soc_trident3_sc_reg32_get(unit, MMU_2DBG_C_0r, -1, -1, 0, &rval));
            soc_reg_field_set(unit, MMU_2DBG_C_0r, &rval, FIELD_Af, 250000);
            SOC_IF_ERROR_RETURN
                (soc_trident3_sc_reg32_set(unit, MMU_2DBG_C_0r, -1, -1, 0, rval));

            /* MMU_2DBG_C_1 reg configs */
            rval = 0;
            SOC_IF_ERROR_RETURN
                (soc_trident3_sc_reg32_get(unit, MMU_2DBG_C_1r, -1, -1, 0, &rval));
            soc_reg_field_set(unit, MMU_2DBG_C_1r, &rval, FIELD_Bf, 1);
            SOC_IF_ERROR_RETURN
                (soc_trident3_sc_reg32_set(unit, MMU_2DBG_C_1r, -1, -1, 0, rval));
        }

        PBMP_ALL_ITER(unit, port) {
            if( IS_STK_PORT(unit, port) || IS_UPLINK_PORT(unit, port)) {
                for (i = 0; i < 2; i++) {
                    phy_port = si->port_l2p_mapping[port];
                    mmu_port = si->port_p2m_mapping[phy_port];
                    if (i) {
                        reg = (mmu_port >=64) ? mc_queue[1] : mc_queue[0];
                    } else {
                        reg = (mmu_port >=64) ? uc_queue[1] : uc_queue[0];
                    }
                    if (mmu_port < 64) {
                        COMPILER_64_ZERO(rval1);
                        COMPILER_64_ZERO(rval2);
                        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval1));
                        rval2 = soc_reg64_field_get(unit, reg, rval1, EXTND_COS_MODEf);
                        COMPILER_64_TO_32_LO(data_lo, rval2);
                        COMPILER_64_TO_32_HI(data_hi, rval2);
                        if (mmu_port >= 32) {
                            data_hi |= (1 << (mmu_port %32));
                        } else {
                            data_lo |= (1 << (mmu_port %32));
                        }
                        COMPILER_64_SET(rval2, data_hi, data_lo);
                        soc_reg64_field_set(unit, reg, &rval1, EXTND_COS_MODEf, rval2);
                        SOC_IF_ERROR_RETURN(soc_reg64_set(unit, reg, REG_PORT_ANY, 0, rval1));
                    } else {
                        /* port bigger than 64 goes to second register and starts from 0. */
                        bit_pos = mmu_port - 64;
                        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &data_hi));
                        data_lo = soc_reg_field_get(unit, reg, data_hi, EXTND_COS_MODEf);
                        data_lo |= (1 << bit_pos);
                        soc_reg_field_set(unit, reg, &data_hi, EXTND_COS_MODEf, data_lo);
                        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, data_hi));

                    }
                }
            }
        }

        PBMP_ALL_ITER(unit, port) {
            rval = 0;
            if (!SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
                continue;
            }
            if (si->port_speed_max[port] >= 40000) {
               soc_reg_field_set(unit, MMU_3DBG_Cr, &rval, FIELD_Af,
                                  fval + sal_rand() % 20);
            } else {
               soc_reg_field_set(unit, MMU_3DBG_Cr, &rval, FIELD_Af, 0);
            }
            SOC_IF_ERROR_RETURN
                (soc_tomahawk_sc_reg32_set(unit, MMU_3DBG_Cr, -1, port, 0, rval));
        }


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

        COMPILER_64_SET(enable64, 0, 0x7f);
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            if (!(pipe_map & (1 << pipe))) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_trident3_xpe_reg_set(unit, XPORT_PFC_EN_SPLIT0r,
                                          -1, pipe, 0, enable[pipe]));
            SOC_IF_ERROR_RETURN
                (soc_trident3_xpe_reg_set(unit, XPORT_PFC_EN_SPLIT1r,
                                          -1, pipe, 0, enable64));
        }

        COMPILER_64_SET(enable64, 0, 0x3f);
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            if (!(pipe_map & (1 << pipe))) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_trident3_xpe_reg_set(unit, MMU_WRED_PORT_ENABLE_SPLIT0r,
                                          -1, pipe, 0, enable[pipe]));
            SOC_IF_ERROR_RETURN
                (soc_trident3_xpe_reg_set(unit, MMU_WRED_PORT_ENABLE_SPLIT1r,
                                          -1, pipe, 0, enable64));
        }
        COMPILER_64_SET(enable64, 0, 0x3fff);
        for (i = 0; i < 8; i++) {
            if (i == 7) {
                SOC_IF_ERROR_RETURN
                    (soc_trident3_xpe_reg_set(unit, shaper_reg[i],
                                              -1, 0, 0, enable64));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_trident3_xpe_reg_set(unit, shaper_reg[i],
                                              -1, 0, 0, enable[0]));
            }
        }
        SOC_IF_ERROR_RETURN(soc_fb6_port_e2ecc_enable(unit));
    }
    return SOC_E_NONE;
}

STATIC int
_soc_fb6_mmu_config_shared_update_check(int val1, int val2, int flags)
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

#define FB6_MMU_SHARED_LIMIT_CHK(val1, val2, flags) \
    _soc_fb6_mmu_config_shared_update_check((val1), (val2), (flags))

int
soc_fb6_mmu_config_shared_buf_recalc(int unit, int res, int thdi_shd,
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

        for (idx = 0; idx < _FB6_MMU_NUM_POOL; idx++) {
            /* THDI SP entries */
            rval = 0;
            reg = THDI_BUFFER_CELL_LIMIT_SPr;
            field = LIMITf;
            granularity = 1;
            SOC_IF_ERROR_RETURN
                (READ_THDI_BUFFER_CELL_LIMIT_SPr(unit, idx, &rval));
            cur_limit = soc_reg_field_get(unit, reg, rval, field);
            if ((cur_limit != 0) &&
                FB6_MMU_SHARED_LIMIT_CHK(cur_limit, thdi_shd/granularity,
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
            for (idx = 0; idx < _FB6_MMU_NUM_PG; idx++) {
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
                        FB6_MMU_SHARED_LIMIT_CHK(cur_limit, thdi_shd/granularity,
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
        for (idx = 0; idx < _FB6_MMU_NUM_POOL; idx++) {
            /* DB entries */
            rval = 0;
            reg = MMU_THDM_DB_POOL_SHARED_LIMITr;
            field = SHARED_LIMITf;
            granularity = 1;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
            cur_limit = soc_reg_field_get(unit, reg, rval, field);
            if ((cur_limit != 0) &&
                FB6_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_db_shd/granularity,
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
                FB6_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd, granularity),
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
                FB6_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd, granularity),
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
                FB6_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_qe_shd/granularity,
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
                FB6_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_qe_shd/granularity,
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
                FB6_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_qe_shd/granularity,
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
            for (idx = 0; idx < SOC_FB6_MMU_CFG_QGROUP_MAX; idx++) {
                sal_memset(&cfg_qgrp, 0, sizeof(cfg_qgrp));

                SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, idx, &cfg_qgrp));

                field = Q_SHARED_LIMIT_CELLf;
                granularity = 1;
                if (!soc_mem_field32_get(unit, mem, &cfg_qgrp, Q_LIMIT_DYNAMIC_CELLf)) {
                    cur_limit = soc_mem_field32_get(unit, mem, &cfg_qgrp, field);
                    if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_db_shd/granularity,
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
                    if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd, granularity),
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, &cfg_qgrp,
                                            field, sal_ceil_func(thdo_db_shd, granularity));
                    }

                    field = LIMIT_YELLOW_CELLf;
                    cur_limit = soc_mem_field32_get(unit, mem, &cfg_qgrp, field);
                    if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd, granularity),
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, &cfg_qgrp,
                                            field, sal_ceil_func(thdo_db_shd, granularity));
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
                    if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_db_shd/granularity,
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_db_shd/granularity);
                    }
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMIC_CELLf)) {
                    field = LIMIT_YELLOW_CELLf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd, granularity),
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, sal_ceil_func(thdo_db_shd, granularity));
                    }
                    field = LIMIT_RED_CELLf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
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
            for (idx = 0; idx < _FB6_MMU_NUM_POOL; idx++) {
                index1 = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, port, mem, idx);
                sal_memset(entry0, 0, sizeof(mmu_thdu_xpipe_config_port_entry_t));

                SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                    index1, entry0));

                field = SHARED_LIMITf;
                granularity = 1;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, thdo_db_shd/granularity);
                }

                granularity = 8;
                field = YELLOW_LIMITf;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, thdo_db_shd/granularity);
                }
                field = RED_LIMITf;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
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
            for (idx = 0; idx < _FB6_MMU_NUM_POOL; idx++) {
                /* DB entries - Port-SP */
                index1 = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, port, mem, idx);
                sal_memset(entry0, 0, sizeof(mmu_thdm_db_portsp_config_entry_t));

                SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                    index1, entry0));

                field = SHARED_LIMITf;
                granularity = 1;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, thdo_db_shd/granularity);
                }

                field = RED_SHARED_LIMITf;
                granularity = 8;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, thdo_db_shd/granularity);
                }

                field = YELLOW_SHARED_LIMITf;
                granularity = 8;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
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
                if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_qe_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem2, entry0,
                                        field, thdo_qe_shd/granularity);
                }

                field = RED_SHARED_LIMITf;
                granularity = 8;
                cur_limit = soc_mem_field32_get(unit, mem2, entry0, field);
                if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_qe_shd/granularity, post_update)) {
                    soc_mem_field32_set(unit, mem2, entry0,
                                        field, thdo_qe_shd/granularity);
                }

                field = YELLOW_SHARED_LIMITf;
                granularity = 8;
                cur_limit = soc_mem_field32_get(unit, mem2, entry0, field);
                if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
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
                    if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_db_shd/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_db_shd/granularity);
                    }
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMICf)) {
                    field = YELLOW_SHARED_LIMITf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_db_shd/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_db_shd/granularity);
                    }
                    field = RED_SHARED_LIMITf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
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
                    if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_qe_shd/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_qe_shd/granularity);
                    }
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMICf)) {
                    field = YELLOW_SHARED_LIMITf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_qe_shd/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_qe_shd/granularity);
                    }
                    field = RED_SHARED_LIMITf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
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
                if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
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
                if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd/granularity, post_update)) {
                    soc_reg_field_set(unit, reg, &rval2, field, (thdo_db_shd/granularity));
                }

                field = SHARED_YELLOW_LIMITf;
                cur_limit = soc_reg_field_get(unit, reg, rval2, field);
                if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
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

            if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
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

            if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
                        thdo_db_shd/granularity, post_update)) {
                soc_reg_field_set
                    (unit, reg, &rval2, field, (thdo_db_shd/granularity));
            }

            field = SHARED_RED_LIMITf;
            cur_limit = soc_reg_field_get(unit, reg, rval, field);

            if (FB6_MMU_SHARED_LIMIT_CHK(cur_limit,
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

/*
 * Types of misconfigurations.
 *     Num subport per lc > 48.
 *     undefined lc number for subtag allowed port.
 */
int
bcmi_fb6_subport_config_validate(int unit)
{

    soc_port_t  port;
    soc_info_t *si=&SOC_INFO(unit);
    int num_subport_per_lc[8] = { 0 };
    int num_subport = 0;
    int line_card = 0;


    if (SOC_PBMP_NOT_NULL(si->subtag_allowed_pbm)) {
        /* Iterate through SubTag ports and reserve subport indices */
        SOC_PBMP_ITER(si->subtag_allowed_pbm, port) {

            line_card = si->port_lc_mapping[port];

            /*
             * User can create subports on port which is not
             * associated with any line card. it may be used in macsec
             * application.
             */
            if (line_card < 1) {
                LOG_DEBUG(BSL_LS_SOC_COMMON,
                           (BSL_META_U(unit,
                              "\n No Line card association on port = %d \n"),
                               port));
                if (si->port_num_subport[port] > 1) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                           (BSL_META_U(unit,
                              "\n Port not connected to line card can have"
                              "only one subport. port = %d , subport= %d \n"),
                               port, si->port_num_subport[port]));
                    return BCM_E_CONFIG;
                }
                continue;
            }

            num_subport = si->port_num_subport[port];

            if (num_subport <= 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                              "\nCONFIG ERROR\n"
                              "No subport for channelized port = %d\n"),
                              port));

                return BCM_E_CONFIG;
            }

            num_subport_per_lc[line_card - 1] += num_subport;

            if (num_subport_per_lc[line_card - 1] >
                FB6_SUBPORT_COE_PORT_PER_CASPORT_MAX) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                              "\nCONFIG ERROR\n"
                              "Number of subports exceeded. subports = %d \n"),
                              num_subport_per_lc[line_card - 1]));

                return BCM_E_CONFIG;
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      soc_fb6_subport_init
 * Purpose:
 *      Setup the pbm fields from config variables for the COE application
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      NONE
 */
int soc_fb6_subport_init(int unit)
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

            if (num_subport > FB6_SUBPORT_COE_PORT_PER_CASPORT_MAX) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                              "\nCONFIG ERROR\n"
                              "num_subports_%d on port %d exceed max value,"
                              "setting it to the max allowed value %d\n\n"),
                              num_subport, port,
                              FB6_SUBPORT_COE_PORT_PER_CASPORT_MAX));
                num_subport = FB6_SUBPORT_COE_PORT_PER_CASPORT_MAX;
            }
            LOG_DEBUG(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                              "num_subports_%d on port %d \n\n"),
                              num_subport, port));

            if (port < SOC_MAX_NUM_PORTS) {
                si->port_num_subport[port] = num_subport;
            }
        }
    }

    /* Initialize the number of COE modules */
    si->num_coe_modules = FB6_SUBPORT_COE_MODULES_MAX;

    BCM_IF_ERROR_RETURN(bcmi_fb6_subport_config_validate(unit));
    return BCM_E_NONE;
}

int
soc_fb6_mmu_splitter_reset(int unit)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_MMU_GCFG_SPLITTERr(unit, &rval));
    soc_reg_field_set(unit, MMU_GCFG_SPLITTERr, &rval, RESET_SBUSf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_SPLITTERr(unit, rval));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_fb6_port_schedule_tdm_init
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
soc_fb6_port_schedule_tdm_init(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    _soc_helix5_tdm_temp_t *tdm = soc->tdm_info;
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
                                        _HX5_TDM_CALENDAR_UNIVERSAL :
                                        _HX5_TDM_CALENDAR_ETHERNET_OPTIMIZED;
	*/
    /* Construct tdm_ingress_schedule_pipe and tdm_egress_schedule_pipe */
    for (pipe = 0; pipe < FIREBOLT6_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        tdm_eschd = &port_schedule_state->tdm_egress_schedule_pipe[pipe];

        tdm_ischd->num_slices = FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE;
        tdm_eschd->num_slices = FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE;

        if (is_flexport) {
            /* TDM Calendar is always in slice 0 */
            sal_memcpy(tdm_ischd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].idb_tdm,
                        sizeof(int)*_FB6_TDM_LENGTH);
            sal_memcpy(tdm_eschd->tdm_schedule_slice[0].linerate_schedule,
                        tdm->tdm_pipe[pipe].mmu_tdm,
                        sizeof(int)*_FB6_TDM_LENGTH);
        }

        /* OVS */
        for (hpipe = 0; hpipe < FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            /* IDB OVERSUB*/
            sched = &tdm_ischd->tdm_schedule_slice[hpipe];
            sched->cal_len = _FB6_TDM_LENGTH;
            sched->num_ovs_groups = FIREBOLT6_TDM_OVS_GROUPS_PER_HPIPE;
            sched->ovs_group_len = FIREBOLT6_TDM_OVS_GROUP_LENGTH;
            sched->num_pkt_sch_or_ovs_space = 1;
            sched->pkt_sch_or_ovs_space_len = _FB6_PKT_SCH_LENGTH;

            if (is_flexport) {
                for (group = 0; group < FIREBOLT6_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                    for (slot = 0; slot < FIREBOLT6_TDM_OVS_GROUP_LENGTH; slot++) {
                        sched->oversub_schedule[group][slot] =
                            tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot];
                    }
                }
                for (slot = 0; slot < _FB6_PKT_SCH_LENGTH; slot++) {
                    sched->pkt_sch_or_ovs_space[0][slot] =
                        tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot];
                }
            }

            /* MMU OVERSUB */
            sched = &tdm_eschd->tdm_schedule_slice[hpipe];
            sched->cal_len = _FB6_TDM_LENGTH;
            sched->num_ovs_groups = FIREBOLT6_TDM_OVS_GROUPS_PER_HPIPE;
            sched->ovs_group_len = FIREBOLT6_TDM_OVS_GROUP_LENGTH;
            sched->num_pkt_sch_or_ovs_space = 1;
            sched->pkt_sch_or_ovs_space_len = _FB6_PKT_SCH_LENGTH;

            if (is_flexport) {
                for (group = 0; group < FIREBOLT6_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                    for (slot = 0; slot < FIREBOLT6_TDM_OVS_GROUP_LENGTH; slot++) {
                        sched->oversub_schedule[group][slot] =
                            tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot];
                        }
                    }
                for (slot = 0; slot < _FB6_PKT_SCH_LENGTH; slot++) {
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
        for (phy_port = 1; phy_port < FB6_NUM_EXT_PORTS; phy_port++) {
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
 *      soc_fb6_soc_tdm_update
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
soc_fb6_soc_tdm_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    _soc_helix5_tdm_temp_t *tdm = soc->tdm_info;
    soc_tdm_schedule_pipe_t *tdm_ischd, *tdm_eschd;
    soc_tdm_schedule_t *sched;
    int pipe, hpipe, phy_port, slot;
    int index, group;

    if (tdm == NULL) {
        return SOC_E_INIT;
    }

    /* Copy info from soc_port_schedule_state_t to _soc_helix5_tdm_temp_t */
    for (pipe = 0; pipe <  FIREBOLT6_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        tdm_eschd = &port_schedule_state->tdm_egress_schedule_pipe[pipe];

        /* TDM Calendar is always in slice 0 */
        sal_memcpy(tdm->tdm_pipe[pipe].idb_tdm,
                    tdm_ischd->tdm_schedule_slice[0].linerate_schedule,
                    sizeof(int)*_FB6_TDM_LENGTH);
        sal_memcpy(tdm->tdm_pipe[pipe].mmu_tdm,
                    tdm_eschd->tdm_schedule_slice[0].linerate_schedule,
                    sizeof(int)*_FB6_TDM_LENGTH);

        for (hpipe = 0; hpipe < FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            sched = &tdm_ischd->tdm_schedule_slice[hpipe];
            for (group = 0; group < FIREBOLT6_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                for (slot = 0; slot < FIREBOLT6_TDM_OVS_GROUP_LENGTH; slot++) {
                    tdm->tdm_pipe[pipe].ovs_tdm[hpipe][group][slot] =
                        sched->oversub_schedule[group][slot];
                }
            }
            for (slot = 0; slot < _FB6_PKT_SCH_LENGTH; slot++) {
                tdm->tdm_pipe[pipe].pkt_shaper_tdm[hpipe][slot] =
                    sched->pkt_sch_or_ovs_space[0][slot];

            }
        }
    }

    /* pblk info init  */
    for (phy_port = 1; phy_port < FB6_NUM_EXT_PORTS; phy_port++) {
        tdm->pblk_info[phy_port].pblk_cal_idx = -1;
        tdm->pblk_info[phy_port].pblk_hpipe_num = -1;
    }

    /* pblk info for phy_port */
    for (pipe = 0; pipe < FIREBOLT6_TDM_PIPES_PER_DEV; pipe++) {
        tdm_ischd = &port_schedule_state->tdm_ingress_schedule_pipe[pipe];
        for (hpipe = 0; hpipe < FIREBOLT6_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            for (group = 0; group < FIREBOLT6_TDM_OVS_GROUPS_PER_HPIPE; group++) {
                for (index = 0; index < FIREBOLT6_TDM_OVS_GROUP_LENGTH; index++) {
                    phy_port =
                        tdm_ischd->tdm_schedule_slice[hpipe].oversub_schedule[group][index];
                    if (phy_port < HX5_NUM_EXT_PORTS) {
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
soc_firebolt6_temperature_intr(void *unit_vp, void *d1, void *d2,
        void *d3, void *d4)
{

    uint32 rval, trval;
    int i, rv, unit = PTR_TO_INT(unit_vp);
    const soc_reg_t *pvtmon_result_register=NULL;
    int num_entries_out;
        pvtmon_result_register = &pvtmon_result_reg[0];
        num_entries_out = COUNTOF(pvtmon_result_reg);

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
            /* Temp = 457.82 - o_ADC_data * 0.55702
             * data = 457.82-(temp/0.55702) = (457820-(temp*1000))/557
             * Note: Since this is a high temp value we can safely assume it to
             * always be a +ive number. This is in degrees celcius.
             */

            trval = (4578200-(trval*5570))/1000;

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

    /* FB6 has HW overtemp protection,  no shutdown logic needed. */
    return;
}

int
soc_firebolt6_show_voltage(int unit)
{
    soc_reg_t reg;
    int index;
    uint32 rval = 0, fval = 0, avg = 0;

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_3r(unit, &rval));
    if ((soc_reg_field_get(unit, TOP_PVTMON_CTRL_3r, rval, PVTMON_RESET_Nf) == 0) ||
        (soc_reg_field_get(unit, TOP_PVTMON_CTRL_3r, rval, PVTMON_SELECTf) != 4)) {

        soc_reg_field_set(unit, TOP_PVTMON_CTRL_3r, &rval, PVTMON_RESET_Nf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_3r(unit, rval));
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_3r, &rval, PVTMON_RESET_Nf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_3r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT0f, 4);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT1f, 4);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT2f, 4);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT3f, 4);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT4f, 4);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT5f, 4);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT6f, 4);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT7f, 4);
        SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_2r(unit, rval));

        sal_usleep(1000);
    }

    avg = 0;

    /* Read Voltages. */
    for (index = 0; index < COUNTOF(pvtmon_result_reg); index++) {
        reg = pvtmon_result_reg[index];

        /* TOP_PVTMON_RESULT_6r is used for AVS, skip the same from
           voltage calculation */
        if (reg == TOP_PVTMON_RESULT_6r) {
            continue;
        }
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        fval = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
#ifdef BCM_FIREBOLT6_SUPPORT
        if (SOC_IS_FIREBOLT6(unit)) {
            /* (PVT_DATA * 950) / (1024 * 0.8) */
            fval = (fval * 950 * 10) / (1024 * 8);
        } else
#endif /* BCM_FIREBOLT6_SUPPORT */
        {
            LOG_CLI((BSL_META_U(unit,
                                "Voltage monitor fail: Need to add formula for current device.\n")));
            return SOC_E_UNAVAIL;
        }
        avg += fval;
        LOG_CLI((BSL_META_U(unit,
                            "Voltage monitor %d: voltage = %d.%03dV\n"),
                 index, (fval/1000), (fval %1000)));
    }

    /* Get count and reduce 1 since TOP_PVTMON_RESULT_6r is not not applicable */
    avg /= (COUNTOF(pvtmon_result_reg) - 1);
    LOG_CLI((BSL_META_U(unit,
                        "Average voltage is = %d.%03dV\n"),
             (avg/1000), (avg %1000)));

    return SOC_E_NONE;
}

int
soc_firebolt6_temperature_monitor_get(int unit, int temperature_max,
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

    if (COUNTOF(pvtmon_result_reg) > temperature_max) {
        num_entries_out = temperature_max;
    } else {
        num_entries_out = COUNTOF(pvtmon_result_reg);
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
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT6f, 0);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_2r, &rval, PVTMON_SELECT7f, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_2r(unit, rval));
        sal_usleep(1000);

        for (index = 0; index < num_entries_out; index++) {
            reg = pvtmon_result_reg[index];
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

int
soc_fb6_mpls_hash_control_set(int *hash_entry)
{
    int unit = 0;
    int mpls_uat_banks = 0;
    int index = 0;
    int offset_base = 0, offset_multiplier = 0;
    int hash_control_entry[2];
    uint32 shared_4k_hash_control_entry[8];

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
    sal_memset(shared_4k_hash_control_entry, 0,
            sizeof(ip_uat_shared_banks_control_entry_t));

    SOC_IF_ERROR_RETURN
        (soc_helix5_hash_bank_count_get(unit, MPLS_ENTRY_SINGLEm,
                                          &mpls_uat_banks));
    SOC_IF_ERROR_RETURN
        (READ_IP_UAT_SHARED_BANKS_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                        shared_4k_hash_control_entry));
    SOC_IF_ERROR_RETURN
        (READ_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0,
                                       hash_control_entry));
    sal_memcpy(hash_entry, hash_control_entry, sizeof(hash_control_entry));

    if (mpls_uat_banks) {
        offset_base = 64 % (mpls_uat_banks);
        offset_multiplier = 64 / (mpls_uat_banks);
    }

    soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, hash_control_entry,
                        HASH_TABLE_BANK_CONFIGf, ((1 << mpls_uat_banks) - 1));
    for(index = 0; index < mpls_uat_banks; index++) {
        soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, hash_control_entry,
                            logical_to_physical_fields[index], index);
        soc_mem_field32_set(unit, IP_UAT_SHARED_BANKS_CONTROLm,
                            shared_4k_hash_control_entry, shared_fields[index],
                            offset_base + ( index * offset_multiplier));
    }

    SOC_IF_ERROR_RETURN
        (WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                        hash_control_entry));

    return SOC_E_NONE;
}

/*
 * Few SKU's doesn't support BroadScan feature in HW.
 * Skip those memories during SER init/TR's.
 */
int _soc_fb6_mem_skip(int unit, soc_mem_t mem)
{
    if (!SOC_IS_FIREBOLT6(unit)) {
        return FALSE;
    }

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (!(soc_feature(unit, soc_feature_subport_forwarding_support))) {
        switch (mem) {
            case SCH_PORT_L0_MAPPING_CONFIG_RAM_PIPE0m:
            case SCH_PORT_L0_MAPPING_CONFIG_RAMm:
                return TRUE;

            break;
            default:
            break;
                /* do nothing */
        }
    }
#endif


    return _soc_flowtracker_mem_skip(unit, mem);
}

/*
 * Few SKU's doesn't support BroadScan feature in HW.
 * Skip those registers during SER init/TR's.
 */
int _soc_fb6_reg_skip(int unit, soc_reg_t reg)
{
    if (!SOC_IS_FIREBOLT6(unit)) {
        return FALSE;
    }

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (!(soc_feature(unit, soc_feature_subport_forwarding_support))) {
        switch (reg) {
            case MMU_GCFG_PORT_CHANNELIZATION_CONFIG0r:
            case MMU_GCFG_PORT_CHANNELIZATION_CONFIG1r:
            case MMU_SCFG_PORT_CHANNELIZATION_CONFIG0r:
            case MMU_SCFG_PORT_CHANNELIZATION_CONFIG1r:
            case MMU_SEDCFG_PORT_CHANNELIZATION_CONFIG0r:
            case MMU_SEDCFG_PORT_CHANNELIZATION_CONFIG1r:
            case MMU_XCFG_PORT_CHANNELIZATION_CONFIG0r:
            case MMU_XCFG_PORT_CHANNELIZATION_CONFIG1r:
                return TRUE;

            break;
            default:
            break;
                /* do nothing */
        }
    }
#endif

    return _soc_flowtracker_reg_skip(unit, reg);
}

/*
 * Helix5 chip driver functions.
 * Pls keep at the end of this file for easy access.
 */
soc_functions_t soc_firebolt6_drv_funs = {
    _soc_firebolt6_misc_init,
    _soc_firebolt6_mmu_init,
    soc_trident3_age_timer_get,
    soc_trident3_age_timer_max_get,
    soc_trident3_age_timer_set,
    soc_trident3_tscx_firmware_set,
    _soc_fb6_tscx_reg_read,
    _soc_fb6_tscx_reg_write,
    soc_firebolt6_bond_info_init,
};
#endif /* BCM_FIREBOLT6_SUPPORT */
