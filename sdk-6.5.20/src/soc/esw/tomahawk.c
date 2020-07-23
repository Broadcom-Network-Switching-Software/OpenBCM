/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk.c
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
#include <soc/soc_ser_log.h>

#ifdef BCM_TOMAHAWK_SUPPORT

#ifdef ALPM_ENABLE
#include <soc/lpm.h>
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#endif /* ALPM_ENABLE */

#include <soc/mmu_config.h>
#include <soc/tomahawk.h>
#include <soc/trident2.h>

#ifdef BCM_TOMAHAWK2_SUPPORT
#include <soc/tomahawk2.h>
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif

#include <soc/drv.h>
#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>
#endif


#define PORT_BLOCK_BASE_PORT(port) \
        (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0x3));

#define _SOC_TH_RECONFIGURE_PORT_TYPE_INFO(ptype) \
    si->ptype.num = 0; \
    si->ptype.min = si->ptype.max = -1; \
    PBMP_ITER(si->ptype.bitmap, it_port) { \
        si->ptype.port[si->ptype.num++] = it_port; \
        if (si->ptype.min < 0) { \
            si->ptype.min = it_port; \
        } \
        if (it_port > si->ptype.max) { \
            si->ptype.max = it_port; \
        } \
    }

#define _TH_SUBS_ACC_TYPE_DUPLICATE            9

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

static soc_speed_group_info_t  soc_speed_group_info[] = {
    {SOC_TH_SPEED_CLASS_10G, SOC_TH_PORT_MODE_QUAD,    4, 3,  "10G_SPEED_CLASS"},
    {SOC_TH_SPEED_CLASS_20G, SOC_TH_PORT_MODE_DUAL,    2, 6,  "20G_SPEED_CLASS"},
    {SOC_TH_SPEED_CLASS_25G, SOC_TH_PORT_MODE_QUAD,    4, 3,  "25G_SPEED_CLASS"},
    {SOC_TH_SPEED_CLASS_40G, SOC_TH_PORT_MODE_DUAL,    2, 6,  "40Gd_SPEED_CLASS"},
    {SOC_TH_SPEED_CLASS_40G, SOC_TH_PORT_MODE_SINGLE,  1, 12, "40Gs_SPEED_CLASS"},
    {SOC_TH_SPEED_CLASS_50G, SOC_TH_PORT_MODE_DUAL,    2, 6,  "50G_SPEED_CLASS"},
    {SOC_TH_SPEED_CLASS_100G, SOC_TH_PORT_MODE_SINGLE, 1, 12, "100G_SPEED_CLASS"}
};

static int soc_speed_group_info_size = sizeof(soc_speed_group_info) / sizeof(soc_speed_group_info_t);

static int num_shared_alpm_banks[SOC_MAX_NUM_DEVICES] = {0};

static soc_mem_t *_soc_th_alpm_bkt_view_map[SOC_MAX_NUM_DEVICES];
static soc_mem_t *_soc_th_lpm_view_map[SOC_MAX_NUM_DEVICES];


static uint32 _soc_th_iss_log_to_phy_bank_map_shadow[SOC_MAX_NUM_DEVICES];

soc_pbmp_t loopback_disable[SOC_MAX_NUM_DEVICES];

typedef void (*soc_mmu_config_buf_default_f)(int unit, _soc_mmu_cfg_buf_t *buf,
                                             _soc_mmu_device_info_t *devcfg,
                                             int lossless);
STATIC soc_error_t
_soc_tomahawk_flexed_port_speed_get(int unit, soc_th_port_lanes_t *lanes_ctrl,
                                     uint32 *port_speed);
STATIC int
soc_th_port_lanes_ctrl_info_diag(int unit, soc_th_port_lanes_t *lanes_ctrl);
STATIC int
soc_tomahawk_oversub_group_find(int unit, soc_th_port_lanes_t *lanes_ctrl);
STATIC int
_soc_tomahawk_port_lanes_update_tdm(int unit, soc_th_port_lanes_t *lanes_ctrl);
STATIC int
_soc_th_default_pg_headroom(int unit, soc_port_t port, int lossless);
STATIC soc_error_t
soc_th_os_mixed_sister_port_mode_get(int unit, int initTime, soc_th_port_lanes_t *lanes_ctrl);
STATIC int
_soc_tomahawk_obm_thresholds_reconfigure(int unit, soc_port_t port_conf);
STATIC int
soc_th_speed_group_consolidate_check(int unit, soc_th_port_lanes_t *lanes_ctrl, int flag);
STATIC int
soc_th_speed_group_migrate(int unit, soc_th_port_lanes_t *lanes_ctrl, int flag);

STATIC int
_soc_tomahawk_mmu_port_empty_check(int unit, soc_port_t port, int *empty)
{
    soc_reg_t status_reg = Q_SCHED_PORT_EMPTY_STATUSr;
    soc_info_t *si = &SOC_INFO(unit);
    int pipe, phy_port, mmu_port;
    uint64 rval64;

    pipe = si->port_pipe[port];
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];
    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, status_reg, pipe, 0, &rval64));

    /* Reg config is per pipe */
    mmu_port %= SOC_TH_MMU_PORT_STRIDE;

    if (mmu_port < 32) {
        *empty = COMPILER_64_LO(rval64) & (1 << mmu_port) ? TRUE : FALSE;
    } else {
        *empty = COMPILER_64_HI(rval64) & (1 << (mmu_port - 32)) ?
                 TRUE : FALSE;
    }

    return SOC_E_NONE;
}

int
soc_tomahawk_mmu_port_flush_hw(int unit, soc_port_t port, uint32 drain_timeout)
{
    soc_info_t *si;
    uint64 rval64;
    uint64 temp64;
    uint32 rval;
    int pipe;
    int empty;
    soc_timeout_t to;
    int phy_port, mmu_port;
    soc_field_t initiate_val;

    COMPILER_64_ZERO(rval64);
    COMPILER_64_ZERO(temp64);
    si = &SOC_INFO(unit);
    pipe = si->port_pipe[port];

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    /* Reg config is per pipe */
    mmu_port %= SOC_TH_MMU_PORT_STRIDE;

    if (mmu_port < 32) {
        COMPILER_64_SET(temp64, 0, (1 << mmu_port));
    } else {
        COMPILER_64_SET(temp64, (1 << (mmu_port - 32)), 0);
    }

    SOC_IF_ERROR_RETURN
        (_soc_tomahawk_mmu_port_empty_check(unit, port, &empty));

    if (empty) {
        return SOC_E_NONE;
    }

    /* Read the Port flush Register */
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, Q_SCHED_PORT_FLUSHr, pipe, 0, &rval64));

    COMPILER_64_OR(rval64, temp64);

    /* Port Not Empty - Initiate Flush */
    /* ENABLEf is the only field in FLUSH reg. Hence writing it directly */
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, Q_SCHED_PORT_FLUSHr, pipe, 0, rval64));

    /* Disable Shaper Updates */
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, MTRO_PORT_ENTITY_DISABLEr, pipe, 0, &rval64));

    COMPILER_64_OR(rval64, temp64);

    /* METERING_DISABLEf is the only field in reg. Hence writing it directly */
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, MTRO_PORT_ENTITY_DISABLEr, pipe, 0, rval64));

    /* Initiate RQE FIFO Flush */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, Q_SCHED_RQE_SNAPSHOTr, pipe, 0, &rval));
    initiate_val = 0x1;
    soc_reg_field_set(unit, Q_SCHED_RQE_SNAPSHOTr, &rval, INITIATEf, initiate_val);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, Q_SCHED_RQE_SNAPSHOTr, pipe, 0, rval));

    /* Restart timeout */
    soc_timeout_init(&to, drain_timeout, 0);
    for (;;) {
        if (soc_timeout_check(&to)) {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, Q_SCHED_RQE_SNAPSHOTr, pipe, 0, &rval));
            initiate_val = soc_reg_field_get(unit, Q_SCHED_RQE_SNAPSHOTr,
                                               rval, INITIATEf);
            if (initiate_val == 0) {
                /* Completion of Snapshot Process */
                break;
            } else {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                          "MacDrainTimeOut:port %d,%s, "
                          "Unable to drain packets from MMU\n"),
                          unit, SOC_PORT_NAME(unit, port)));
                return SOC_E_INTERNAL;
            }
        }
    }

    COMPILER_64_NOT(temp64);
    /* Reenable Shaper Updates */
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, MTRO_PORT_ENTITY_DISABLEr, pipe, 0, &rval64));
    COMPILER_64_AND(rval64, temp64);
    /* METERING_DISABLEf is the only field in reg. Hence writing it directly */
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, MTRO_PORT_ENTITY_DISABLEr, pipe, 0, rval64));

    /* Read the Port flush Register */
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, Q_SCHED_PORT_FLUSHr, pipe, 0, &rval64));
    COMPILER_64_AND(rval64, temp64);
    /* Port Empty - Disable Flush */
    /* ENABLEf is the only field in FLUSH reg. Hence writing it directly */
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, Q_SCHED_PORT_FLUSHr, pipe, 0, rval64));

    SOC_IF_ERROR_RETURN
        (_soc_tomahawk_mmu_port_empty_check(unit, port, &empty));
    if (empty) {
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

soc_th_latency_table_cfg_t
soc_th_tbl_cfg[SOC_MAX_NUM_DEVICES] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

STATIC int
soc_bcm56168_mem_config(int unit)
{
    soc_persist_t *sop;

    sop = SOC_PERSIST(unit);
    sop->memState[VFIm].index_max = 4095;
    sop->memState[VFI_1m].index_max = 4095;
    sop->memState[EGR_VFIm].index_max = 4095;
    sop->memState[L2MCm].index_max = 4095;
    sop->memState[L3_IPMCm].index_max = 2047;
    sop->memState[L3_IPMC_REMAPm].index_max = 2047;
    sop->memState[EGR_IPMCm].index_max = 2047;
    sop->memState[EGR_L3_NEXT_HOPm].index_max = 4095;
    sop->memState[ING_L3_NEXT_HOPm].index_max = 4095;
    sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 4095;

    return SOC_E_NONE;
}

int
soc_tomahawk_mem_config(int unit)
{
#define _SOC_TH_DEFIP_MAX_TCAMS   8
#define _SOC_TH_DEFIP_REDUCED_TCAMS   3
#define _SOC_TH_DEFIP_TCAM_DEPTH  SOC_IS_TOMAHAWK2(unit) ? 2048 : 1024
#define _SOC_TH_FIXED_TBL_SZ      (8 * 1024)
    soc_persist_t *sop;
    int l2_entries, fixed_l2_entries = 0, shared_l2_banks = 0;
    int num_ipv6_128b_entries = 0;
    int config_v6_entries = 0;
    int defip_config = 0;
    int l3_entries, fixed_l3_entries = 0, shared_l3_banks = 0;
    int fpem_entries, shared_fpem_banks = 0;
    int num_share_banks_enabled = 4;
    int share_banks_bitmap_enabled = 0xf;
    int alpm_enable;
    int latency;
    int max_tcams  = _SOC_TH_DEFIP_MAX_TCAMS;
    int tcam_depth = _SOC_TH_DEFIP_TCAM_DEPTH;
    uint16 dev_id = 0;
    uint8 rev_id = 0;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    sop = SOC_PERSIST(unit);

    l2_entries   = soc_property_get(unit, spn_L2_MEM_ENTRIES, _SOC_TH_FIXED_TBL_SZ);
    l3_entries   = soc_property_get(unit, spn_L3_MEM_ENTRIES, _SOC_TH_FIXED_TBL_SZ);
    fpem_entries = soc_property_get(unit, spn_FPEM_MEM_ENTRIES, 0);
    alpm_enable  = soc_property_get(unit, spn_L3_ALPM_ENABLE, 0);

    if ((dev_id == BCM56168_DEVICE_ID) || (dev_id == BCM56169_DEVICE_ID)) {
        max_tcams = _SOC_TH_DEFIP_REDUCED_TCAMS;
    }

    /* switch latency bypass */
    SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency));

    switch(latency) {
        case SOC_SWITCH_BYPASS_MODE_NONE:
            /*
             * bank 0-1 are dedicated L2 banks (4k entries per bank)
             * bank 6-9 are dedicated L3 banks (2k entries per bank)
             * bank 2-5 are shared banks (32k L2/L3 or ALPM(uses all 4 or banks 2,3)
             *          or 16k FPEM entres per bank)
             */
            fixed_l2_entries = _SOC_TH_FIXED_TBL_SZ;  /* 8k dedicated L2 entries */
            if (l2_entries < fixed_l2_entries) {
                l2_entries = fixed_l2_entries;
                shared_l2_banks = 0;
            } else {
                l2_entries -= fixed_l2_entries;
#if defined(BCM_TOMAHAWK2_SUPPORT)
                if (SOC_IS_TOMAHAWK2(unit)) {
                    shared_l2_banks = (l2_entries + (64 * 1024 - 1)) / (64 * 1024);
                    l2_entries = fixed_l2_entries + shared_l2_banks * 64 * 1024;
                } else
#endif
                {
                    shared_l2_banks = (l2_entries + (32 * 1024 - 1)) / (32 * 1024);
                    l2_entries = fixed_l2_entries + shared_l2_banks * 32 * 1024;
                }
            }

            fixed_l3_entries = _SOC_TH_FIXED_TBL_SZ;  /* 8k dedicated L3 entries */
            if (l3_entries < fixed_l3_entries) {
                l3_entries = fixed_l3_entries;
                shared_l3_banks = 0;
            } else {
                l3_entries -= fixed_l3_entries;
#if defined(BCM_TOMAHAWK2_SUPPORT)
                if (SOC_IS_TOMAHAWK2(unit)) {
                    shared_l3_banks = (l3_entries + (64 * 1024 - 1)) / (64 * 1024);
                    l3_entries = fixed_l3_entries + shared_l3_banks * 64 * 1024;
                } else
#endif
                if ((dev_id == BCM56168_DEVICE_ID) || (dev_id == BCM56169_DEVICE_ID)) {
                    shared_l3_banks = 0;
                    l3_entries = fixed_l3_entries;
                } else {
                    shared_l3_banks = (l3_entries + (32 * 1024 - 1)) / (32 * 1024);
                    l3_entries = fixed_l3_entries + shared_l3_banks * 32 * 1024;
                }
            }

#if defined(BCM_TOMAHAWK2_SUPPORT)
            if (SOC_IS_TOMAHAWK2(unit)) {
                shared_fpem_banks = (fpem_entries + (32 * 1024 - 1)) / (32 * 1024);
                fpem_entries = shared_fpem_banks * 32 * 1024;
            } else
#endif
            {
                shared_fpem_banks = (fpem_entries + (16 * 1024 - 1)) / (16 * 1024);
                fpem_entries = shared_fpem_banks * 16 * 1024;
            }

            if ((dev_id == BCM56168_DEVICE_ID) || (dev_id == BCM56169_DEVICE_ID)) {
                if (shared_l2_banks  + shared_fpem_banks > 1) {
                    return SOC_E_PARAM;
                }
            }

#ifdef BCM_TOMAHAWK2_SUPPORT            
            if (soc_feature(unit, soc_feature_untethered_otp)) {
                soc_th2_uft_otp_info_get(unit, &num_share_banks_enabled,
                    &share_banks_bitmap_enabled);
            }
#endif

            if (shared_l2_banks + shared_l3_banks + shared_fpem_banks 
                > num_share_banks_enabled) {
                return SOC_E_PARAM;
            }

            if (alpm_enable && soc_feature(unit, soc_feature_alpm)) {
                if (((shared_l2_banks + shared_l3_banks + shared_fpem_banks + 2) 
                    > num_share_banks_enabled)
                    /* If any of B2, B3 banks is disabled */
                    || (~share_banks_bitmap_enabled & 0x3)) {
                    return SOC_E_PARAM;
                }
            }

            break;

        case SOC_SWITCH_BYPASS_MODE_LOW:
            if (l2_entries > _SOC_TH_FIXED_TBL_SZ) {
                LOG_CLI(("ERROR: Unsupported L2 table size specified in "
                         "config.bcm\n"));
                LOG_CLI(("INFO : L2 table size reduced to 8K in "
                         "low (L2) latency mode\n"));
            }
            if (l3_entries) {
                LOG_CLI(("ERROR: L3 tables unsupported in "
                         "low (L2) latency mode\n"));
                LOG_CLI(("INFO : Ignored L3 table config specified "
                         "in config.bcm\n"));
            }
            if (alpm_enable) {
                LOG_CLI(("ERROR: ALPM unsupported in "
                         "low (L2) latency mode\n"));
                LOG_CLI(("INFO : Ignored ALPM config specified "
                         "in config.bcm\n"));
            }

            l2_entries = _SOC_TH_FIXED_TBL_SZ;
            fixed_l2_entries = _SOC_TH_FIXED_TBL_SZ;
            l3_entries = 0;
            fixed_l3_entries = 0;
            max_tcams = tcam_depth = 0;

            fpem_entries = 0;
            shared_l2_banks = 0;
            shared_l3_banks = 0;
            shared_fpem_banks = 0;

            alpm_enable = 0;

            sop->memState[VLAN_MPLSm].index_max = -1;
            SOC_MEM_PTR(unit, VLAN_MPLSm) = 0;
            break;

        case SOC_SWITCH_BYPASS_MODE_BALANCED:
            if (l2_entries > _SOC_TH_FIXED_TBL_SZ) {
                LOG_CLI(("ERROR: Unsupported L2 table size specified in "
                         "config.bcm\n"));
                LOG_CLI(("INFO : L2 table size reduced to 8K in "
                         "balanced (L2/L3) latency mode\n"));
            }
            if (l3_entries > _SOC_TH_FIXED_TBL_SZ) {

                LOG_CLI(("ERROR: Unsupported L3 table size specified in"
                         "config.bcm\n"));
                LOG_CLI(("INFO : L3 table size reduced to 8K in "
                         "balanced (L2/L3) latency mode\n"));
            }
            if (alpm_enable) {
                LOG_CLI(("ERROR: ALPM unsupported in "
                         "balanced (L2/L3) latency mode\n"));
                LOG_CLI(("INFO : Ignored ALPM config specified "
                         "in config.bcm\n"));
            }

            l2_entries = _SOC_TH_FIXED_TBL_SZ;
            fixed_l2_entries = _SOC_TH_FIXED_TBL_SZ;
            l3_entries = _SOC_TH_FIXED_TBL_SZ;
            fixed_l3_entries = _SOC_TH_FIXED_TBL_SZ;

            fpem_entries = 0;
            shared_l2_banks = 0;
            shared_l3_banks = 0;
            shared_fpem_banks = 0;

            alpm_enable = 0;

            /* In balanced latency mode, only first half of ecmp tables are active */
            sop->memState[L3_ECMPm].index_max =
                                   sop->memState[L3_ECMPm].index_max/2;
            sop->memState[L3_ECMP_COUNTm].index_max =
                             sop->memState[L3_ECMP_COUNTm].index_max/2;
            sop->memState[INITIAL_L3_ECMP_COUNTm].index_max =
                             sop->memState[INITIAL_L3_ECMP_COUNTm].index_max/2;
            sop->memState[INITIAL_L3_ECMPm].index_max =
                             sop->memState[INITIAL_L3_ECMPm].index_max/2;
            break;

            default:
                return SOC_E_INTERNAL;
    }

    if (soc_feature(unit, soc_feature_alpm)) {
        _soc_alpm_mode[unit] = alpm_enable;
    }

    if (SAL_BOOT_QUICKTURN) {
        sop->memState[MY_STATION_TCAMm].index_max = 7;
    }  

    /* Adjust L2 table size */
    sop->memState[L2Xm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_TILEm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_ONLY_TILEm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_ONLY_ECCm].index_max = l2_entries - 1;
    sop->memState[L2_HITDA_ONLYm].index_max = l2_entries / 4 - 1;
    sop->memState[L2_HITSA_ONLYm].index_max = l2_entries / 4 - 1;
    sop->memState[L2_ENTRY_ISS_LPm].index_max =
        (l2_entries - fixed_l2_entries) / 4 - 1;

    /* Adjust L3 table size */
    sop->memState[L3_ENTRY_ONLYm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = l3_entries / 2 - 1;
    sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = l3_entries/ 2 - 1;
    sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = l3_entries / 4 - 1;
    sop->memState[L3_ENTRY_ONLY_ECCm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_HIT_ONLYm].index_max = l3_entries / 4 - 1;
    sop->memState[L3_ENTRY_ISS_LPm].index_max =
        (l3_entries - fixed_l3_entries) / 4 - 1;

    /* Adjust FP exact match table size */
    sop->memState[EXACT_MATCH_2m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_PIPE0m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_PIPE1m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_PIPE2m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_PIPE3m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLYm].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLY_PIPE0m].index_max =
        fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLY_PIPE1m].index_max =
        fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLY_PIPE2m].index_max =
        fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLY_PIPE3m].index_max =
        fpem_entries - 1;
    sop->memState[EXACT_MATCH_4m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_PIPE0m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_PIPE1m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_PIPE2m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_PIPE3m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLYm].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLY_PIPE0m].index_max =
        fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLY_PIPE1m].index_max =
        fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLY_PIPE2m].index_max =
        fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLY_PIPE3m].index_max =
        fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_HIT_ONLYm].index_max = fpem_entries / 2 - 1;
    sop->memState[FPEM_LPm].index_max = fpem_entries / 2 - 1;
    sop->memState[FPEM_ECCm].index_max = fpem_entries * 2 - 1;
    sop->memState[FPEM_ECC_PIPE0m].index_max = fpem_entries * 2 - 1;
    sop->memState[FPEM_ECC_PIPE1m].index_max = fpem_entries * 2 - 1;
    sop->memState[FPEM_ECC_PIPE2m].index_max = fpem_entries * 2 - 1;
    sop->memState[FPEM_ECC_PIPE3m].index_max = fpem_entries * 2 - 1;

    SOC_CONTROL(unit)->l3_defip_max_tcams = max_tcams;
    SOC_CONTROL(unit)->l3_defip_tcam_size = tcam_depth;

    if (SOC_SWITCH_BYPASS_MODE_LOW == latency) {
        sop->memState[L3_DEFIP_PAIR_128m].index_max = -1;
        sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max = -1;
        sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max = -1;
        sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max = -1;
        sop->memState[L3_DEFIPm].index_max = -1;
        sop->memState[L3_DEFIP_ONLYm].index_max = -1;
        sop->memState[L3_DEFIP_DATA_ONLYm].index_max = -1;
        sop->memState[L3_DEFIP_HIT_ONLYm].index_max = -1;
    } else {
        if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
            if (!(soc_feature(unit, soc_feature_alpm) &&
                  soc_property_get(unit, spn_L3_ALPM_ENABLE, 0))) {
                defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);

                if (SOC_IS_TOMAHAWK2(unit)) {
                    num_ipv6_128b_entries = soc_property_get(unit,
                                                spn_NUM_IPV6_LPM_128B_ENTRIES,
                                                (defip_config ? 4096 : 0));
                } else if ((dev_id == BCM56168_DEVICE_ID) || (dev_id == BCM56169_DEVICE_ID)) {
                    num_ipv6_128b_entries =  soc_property_get(unit,
                                                spn_NUM_IPV6_LPM_128B_ENTRIES,
                                                (defip_config ? 512 : 0));
                    if (num_ipv6_128b_entries > 512) {
                        num_ipv6_128b_entries = 512;
                    }
                } else {
                    num_ipv6_128b_entries = soc_property_get(unit,
                                                spn_NUM_IPV6_LPM_128B_ENTRIES,
                                                (defip_config ? 2048 : 0));
                }
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

                if (soc_feature(unit, soc_feature_l3_1k_defip_table)) {
                    sop->memState[L3_DEFIPm].index_max = 1023;
                }

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
                    if (SOC_IS_TOMAHAWK2(unit)) {
                        num_ipv6_128b_entries = soc_property_get(unit,
                                                    spn_NUM_IPV6_LPM_128B_ENTRIES,
                                                    4096);
                    } else {
                        num_ipv6_128b_entries = soc_property_get(unit,
                                                    spn_NUM_IPV6_LPM_128B_ENTRIES,
                                                    2048);
                    }
                    num_ipv6_128b_entries = num_ipv6_128b_entries +
                                            (num_ipv6_128b_entries % 2);
                    if (soc_tomahawk_alpm_mode_get(unit) == 1 ||
                        soc_tomahawk_alpm_mode_get(unit) == 3) {
                        num_ipv6_128b_entries = (num_ipv6_128b_entries + 3) / 4 * 4;
                    }
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
    }/* switch latency bypass */

    /* Adjust ALPM table size */
    if ((SOC_SWITCH_BYPASS_MODE_NONE == latency) &&
         soc_feature(unit, soc_feature_alpm) &&
         soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {
        /* The check below is sufficient because if ALPM mode is enabled
         * and number of shared banks used is > 2 we return SOC_E_PARAM
         * in code at the top of this function.
         */
        if ((shared_l2_banks + shared_l3_banks + shared_fpem_banks > 0)
              /* If any of B4, B5 banks is disabled, ALPM can only use Bank B2, B3 */
            || (~share_banks_bitmap_enabled & 0xc)){
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

    if (_soc_th_alpm_bkt_view_map[unit] != NULL) {
        sal_free(_soc_th_alpm_bkt_view_map[unit]);
        _soc_th_alpm_bkt_view_map[unit] = NULL;
    }

    if (NULL == _soc_th_alpm_bkt_view_map[unit]) {
        if ((_soc_th_alpm_bkt_view_map[unit] =
            sal_alloc(sizeof(soc_mem_t) * SOC_TH_ALPM_MAX_BKTS(unit),
                      "alpm_bkt_map")) == NULL) {
            return SOC_E_MEMORY;
        }
    }

    sal_memset(_soc_th_alpm_bkt_view_map[unit], INVALIDm,
               sizeof(soc_mem_t) * SOC_TH_ALPM_MAX_BKTS(unit));

    if (_soc_th_lpm_view_map[unit] != NULL) {
        sal_free(_soc_th_lpm_view_map[unit]);
        _soc_th_lpm_view_map[unit] = NULL;
    }

    if (NULL == _soc_th_lpm_view_map[unit]) {
        if ((_soc_th_lpm_view_map[unit] =
            sal_alloc(sizeof(soc_mem_t) *
                      SOC_L3_DEFIP_MAX_TCAMS_GET(unit) *
                      SOC_L3_DEFIP_TCAM_DEPTH_GET(unit),
                      "lpm_view_map")) == NULL) {
            return SOC_E_MEMORY;
        }
    }

    sal_memset(_soc_th_lpm_view_map[unit], INVALIDm,
               sizeof(soc_mem_t) *
               SOC_L3_DEFIP_MAX_TCAMS_GET(unit) *
               SOC_L3_DEFIP_TCAM_DEPTH_GET(unit));
    if ((dev_id == BCM56168_DEVICE_ID) || (dev_id == BCM56169_DEVICE_ID)) {
        soc_bcm56168_mem_config(unit);
    }

    /* update tbl cfg to latency subsystem for diag purposes */
    soc_th_tbl_cfg[unit].l2_entries = l2_entries;
    soc_th_tbl_cfg[unit].fixed_l2_entries = fixed_l2_entries;
    soc_th_tbl_cfg[unit].shared_l2_banks = shared_l2_banks;
    soc_th_tbl_cfg[unit].l3_entries = l3_entries;
    soc_th_tbl_cfg[unit].fixed_l3_entries = fixed_l3_entries;
    soc_th_tbl_cfg[unit].shared_l3_banks = shared_l3_banks;
    soc_th_tbl_cfg[unit].fpem_entries = fpem_entries;
    soc_th_tbl_cfg[unit].shared_fpem_banks = shared_fpem_banks;
    soc_th_tbl_cfg[unit].alpm_enable = alpm_enable;
    soc_th_tbl_cfg[unit].max_tcams = max_tcams;
    soc_th_tbl_cfg[unit].tcam_depth = tcam_depth;

    return SOC_E_NONE;
}

int
soc_tomahawk_mem_cpu_write_control(int unit, soc_mem_t mem, int copyno,
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
#if defined(BCM_TOMAHAWK2_SUPPORT)
            (SOC_BLOCK_TYPE(unit, block_info_index) == SOC_BLK_MMU_SED) ||
#endif
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
}

int
soc_tomahawk_reg_cpu_write_control(int unit, int enable)
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

uint32 _soc_th_mmu_port(int unit, int port)
{
    soc_info_t *si = &SOC_INFO(unit);
    return si->port_p2m_mapping[si->port_l2p_mapping[port]];
}

/*
 * CPU port (mmu port 32): 48 MC queues
 * Loopback port (mmu port 33,97,161,225): 10 MC queues
 * Mgmt port (mmu port 96,160): 10 UC, 10 MC queues
 */
int
soc_tomahawk_num_cosq_init_port(int unit, int port)
{
#define CPU_MCQ_BASE  330 /* CPU port MC queue base index */
#define LB_MCQ_BASE   320 /* LB ports MC queue base index */
#define MGMT_UCQ_BASE 320 /* Mgmt ports UC queue base index */
#define MGMT_MCQ_BASE 330 /* Mgmt ports MC queue base index */
    soc_info_t *si;
    int mmu_port;

    si = &SOC_INFO(unit);
    mmu_port = SOC_TH_MMU_PORT(unit, port);

    if (IS_CPU_PORT(unit, port)) {
        si->port_num_cosq[port] = 48;
        si->port_cosq_base[port] = CPU_MCQ_BASE;
    } else if (IS_LB_PORT(unit, port)) {
        si->port_num_cosq[port] = 10;
        si->port_cosq_base[port] = LB_MCQ_BASE;
    } else if (SOC_PBMP_MEMBER(si->management_pbm, port)) {
        si->port_num_cosq[port] = 10;
        si->port_cosq_base[port] = MGMT_MCQ_BASE;
        si->port_num_uc_cosq[port] = 10;
        si->port_uc_cosq_base[port] = MGMT_UCQ_BASE;
    } else {
        if (mmu_port == -1) {
            return SOC_E_INTERNAL;
        }

        si->port_num_cosq[port] = 10;
        si->port_cosq_base[port] =
                    (mmu_port % SOC_TH_MMU_PORT_STRIDE) * 10;
        si->port_num_uc_cosq[port] = 10;
        si->port_uc_cosq_base[port] =
                    (mmu_port % SOC_TH_MMU_PORT_STRIDE) * 10;
        si->port_num_ext_cosq[port] = 0;
    }

    return SOC_E_NONE;
}


/*
 * CPU port (mmu port 32): 48 MC queues
 * Loopback port (mmu port 33,97,161,225): 10 MC queues
 * Mgmt port (mmu port 96,160): 10 UC, 10 MC queues
 */
int
soc_tomahawk_num_cosq_init(int unit)
{
    int port;

    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_num_cosq_init_port(unit, port));
    }

    return SOC_E_NONE;
}

STATIC int
soc_tomahawk_max_frequency_get(int unit,  uint16 dev_id, uint8 rev_id,
                               int skew_id, int *frequency)
{
    switch (dev_id) {
    default:
        *frequency = 850;
    }

    return SOC_E_NONE;
}

void
soc_tomahawk_tsc_map_get(int unit, uint32 *tsc_map)
{
    *tsc_map = 0xffffffff;
}

void
soc_tomahawk_pipe_map_get(int unit, uint32 *pipe_map)
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
 * #1   single: 100  -   x  -   x  -   x  SOC_TH_PORT_RATIO_SINGLE
 * #2   single:  40  -   x  -   x  -   x  SOC_TH_PORT_RATIO_TRI_023_2_1_1
 * #3   dual:    50  -   x  -  50  -   x  SOC_TH_PORT_RATIO_DUAL_1_1
 * #4   dual:    40  -   x  -  40  -   x  SOC_TH_PORT_RATIO_DUAL_1_1
 * #5   dual:    20  -   x  -  20  -   x  SOC_TH_PORT_RATIO_DUAL_1_1
 * #6   dual:    40  -   x  -  20  -   x  SOC_TH_PORT_RATIO_DUAL_2_1
 * #7   dual:    20  -   x  -  40  -   x  SOC_TH_PORT_RATIO_DUAL_1_2
 * #8   tri:     50  -   x  - 25/x - 25/x SOC_TH_PORT_RATIO_TRI_023_2_1_1
 * #9   tri:     20  -   x  - 10/x - 10/x SOC_TH_PORT_RATIO_TRI_023_2_1_1
 * #10  tri:     40  -   x  - 10/x - 10/x SOC_TH_PORT_RATIO_TRI_023_4_1_1
 * #11  tri:    25/x - 25/x -  50  -   x  SOC_TH_PORT_RATIO_TRI_012_1_1_2
 * #12  tri:    10/x - 10/x -  20  -   x  SOC_TH_PORT_RATIO_TRI_012_1_1_2
 * #13  tri:    10/x - 10/x -  40  -   x  SOC_TH_PORT_RATIO_TRI_012_1_1_4
 * #14  quad:   25/x - 25/x - 25/x - 25/x SOC_TH_PORT_RATIO_QUAD
 * #15  quad:   10/x - 10/x - 10/x - 10/x SOC_TH_PORT_RATIO_QUAD
 */
void
soc_tomahawk_port_ratio_get(int unit, int initTime, int clport, int *mode)
{
    soc_info_t *si;
    int port, phy_port_base, lane;
    int num_lanes[_TH_PORTS_PER_PBLK];
    int speed_max[_TH_PORTS_PER_PBLK];
    int cur_mode;
    int port_sub, port_sis;

    si = &SOC_INFO(unit);
    phy_port_base = 1 + clport * _TH_PORTS_PER_PBLK;
    for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane += 2) {
        port = si->port_p2l_mapping[phy_port_base + lane];
        if (port == -1 || SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            num_lanes[lane] = -1;
            speed_max[lane] = -1;
        } else {
            num_lanes[lane] = si->port_num_lanes[port];
            /* At init time port_speed_max_max contains port init speed_max and
             * at run time during flex port opeartion, init_speed_max
             * contains current port speed_max
             */
            speed_max[lane] = initTime ? si->port_speed_max[port] :
                              si->port_init_speed[port];
        }
    }

    if (num_lanes[0] == 4) {
        *mode = SOC_TH_PORT_RATIO_SINGLE;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 2) {
        if (speed_max[0] == speed_max[2]) {
            *mode = SOC_TH_PORT_RATIO_DUAL_1_1;
        } else if (speed_max[0] > speed_max[2]) {
            *mode = SOC_TH_PORT_RATIO_DUAL_2_1;
        } else {
            *mode = SOC_TH_PORT_RATIO_DUAL_1_2;
        }
    } else if (num_lanes[0] == 2) {
        if (num_lanes[2] == -1) {
            *mode = SOC_TH_PORT_RATIO_DUAL_1_1;
        } else {
            *mode = cur_mode = (speed_max[0] == 20000 || speed_max[0] == 21000 ||
                     speed_max[0] == 50000 || speed_max[0] == 53000) ?
                SOC_TH_PORT_RATIO_TRI_023_2_1_1 : SOC_TH_PORT_RATIO_TRI_023_4_1_1;
            if (initTime && si->os_mixed_sister_25_50_enable) {
                *mode = ((speed_max[0] == 50000 && speed_max[2] == 25000) ||
                         (speed_max[0] == 53000 && speed_max[2] == 27000)) ?
                    SOC_TH_PORT_RATIO_DUAL_1_1 : cur_mode;
            }
        }
    } else if (num_lanes[2] == 2) {
        if (num_lanes[0] == -1) {
            *mode = SOC_TH_PORT_RATIO_DUAL_1_1;
        } else {
            *mode = cur_mode = (speed_max[2] == 20000 || speed_max[2] == 21000 ||
                     speed_max[2] == 50000 || speed_max[2] == 53000) ?
                SOC_TH_PORT_RATIO_TRI_012_1_1_2 : SOC_TH_PORT_RATIO_TRI_012_1_1_4;
            if (initTime && si->os_mixed_sister_25_50_enable) {
                *mode = ((speed_max[0] == 25000 && speed_max[2] == 50000) ||
                         (speed_max[0] == 27000 && speed_max[2] == 53000)) ?
                    SOC_TH_PORT_RATIO_DUAL_1_1 : cur_mode;
            }
        }
    } else {
        *mode = cur_mode = SOC_TH_PORT_RATIO_QUAD;

        if (initTime && si->os_mixed_sister_25_50_enable) {
            port_sub = si->port_p2l_mapping[phy_port_base + 1];
            port_sis = si->port_p2l_mapping[phy_port_base + 3];
            *mode = (((speed_max[0] == 25000 && speed_max[2] == 25000) ||
                      (speed_max[0] == 27000 && speed_max[2] == 27000)) &&
                     SOC_PBMP_MEMBER(si->all.disabled_bitmap, port_sub) &&
                     SOC_PBMP_MEMBER(si->all.disabled_bitmap, port_sis)) ?
                     SOC_TH_PORT_RATIO_DUAL_1_1 : cur_mode;
        }
    }
}

#if 0
STATIC void
_soc_tomahawk_pipe_bandwidth_get(int unit, int pipe,
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
void
_soc_tomahawk_speed_to_ovs_class_mapping(int unit, int speed, int *ovs_class)
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

void
_soc_tomahawk_speed_to_slot_mapping(int unit, int speed, int *slot)
{
    if (speed >= 40000) {
        if (speed >= 100000) {
            *slot = 40;
        } else if (speed >= 50000) {
            *slot = 20;
        } else {
            *slot = 16;
        }
    } else {
        if (speed >= 25000) {
            *slot = 10;
        } else if (speed >= 20000) {
            *slot = 8;
        } else if (speed >= 10000) {
            *slot = 4;
        } else {
            *slot = 1;
        }
    }
}

static int _soc_th_port_speed_cap[SOC_MAX_NUM_DEVICES][132] = {{ 0 }};

STATIC int
_soc_th_post_mmu_init_update(int unit)
{
    int port;
    soc_info_t *si = &SOC_INFO(unit);

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        return soc_th2_post_mmu_init_update(unit);
    }
#endif

    PBMP_ALL_ITER(unit, port) {
        /* Set init speed to max speed by default */
        si->port_init_speed[port] = si->port_speed_max[port];
        if (_soc_th_port_speed_cap[unit][port]) {
            /* If cap speed is available then adjust max speed for further use */
            si->port_speed_max[port] = _soc_th_port_speed_cap[unit][port];
        }
    }
    return SOC_E_NONE;
}

/*
 * FUNCTION : soc_th_bypass_unused_pm
 * ARGS:
 *         unit    -    IN, unit number
 * Description :
 * Bypass unused port blocks, by programming TOP_TSC_ENABLEr
 * appropriately.
 */
soc_error_t
soc_th_bypass_unused_pm(int unit)
{
    int port, phy_port, pm, block, bn;
    uint32 rval = 0, orval, xrval;
    soc_info_t *si = &SOC_INFO(unit);
    uint8 used_pm_map[_TH_PBLKS_PER_DEV + 1]; /* #CLPORT + #XLPORT */
    soc_reg_t reg = TOP_TSC_ENABLEr;
    soc_field_t reg_field[_TH_PBLKS_PER_DEV] = {
        TSC_0_ENABLEf, TSC_1_ENABLEf, TSC_2_ENABLEf, TSC_3_ENABLEf,
        TSC_4_ENABLEf, TSC_5_ENABLEf, TSC_6_ENABLEf, TSC_7_ENABLEf,
        TSC_8_ENABLEf, TSC_9_ENABLEf, TSC_10_ENABLEf, TSC_11_ENABLEf,
        TSC_12_ENABLEf, TSC_13_ENABLEf, TSC_14_ENABLEf, TSC_15_ENABLEf,
        TSC_16_ENABLEf, TSC_17_ENABLEf, TSC_18_ENABLEf, TSC_19_ENABLEf,
        TSC_20_ENABLEf, TSC_21_ENABLEf, TSC_22_ENABLEf, TSC_23_ENABLEf,
        TSC_24_ENABLEf, TSC_25_ENABLEf, TSC_26_ENABLEf, TSC_27_ENABLEf,
        TSC_28_ENABLEf, TSC_29_ENABLEf, TSC_30_ENABLEf, TSC_31_ENABLEf
    };

    sal_memset(used_pm_map, 0, sizeof(used_pm_map));
    for (phy_port = 1; phy_port <= _TH_PORTS_PER_DEV; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        pm = si->port_serdes[port];
        if ((-1 != pm) && (0 == used_pm_map[pm])) {
            used_pm_map[pm] = 1;
        }
    }
    SOC_IF_ERROR_RETURN(READ_TOP_TSC_ENABLEr(unit, &rval));
    orval = rval;
    /* Upper loop bound omits XLPORT block intentionally */
    for (pm = 0; pm < _TH_PBLKS_PER_DEV; pm++) {
        /* If PM is not used, bypass it */
        if (0 == used_pm_map[pm]) {
            soc_reg_field_set(unit, reg, &rval, reg_field[pm], 0);
            phy_port = 1 + (pm * _TH_PORTS_PER_PBLK);
            block = SOC_PORT_BLOCK(unit, phy_port);
            /* Invalidate port block in soc info */
            if (SOC_BLK_CLPORT == SOC_BLOCK_INFO(unit, block).type) {
                si->block_valid[block] = 0;
            }
        }
    }
    if (rval != orval) {
        /*Calculate different bits number between old and new*/
        xrval = rval ^ orval;
        for (bn = 0; xrval != 0; bn++) {
            xrval &= (xrval - 1);
        }

        /*Do two steps configuration change for the case of different bits number beyond 10*/
        if (bn > 10) {
            /*Change half of those different bits to new at first*/
            bn /= 2;
            xrval = rval ^ orval;
            do {
                xrval &= (xrval-1);
            } while (bn-- > 0);
            xrval ^= rval;
            SOC_IF_ERROR_RETURN(WRITE_TOP_TSC_ENABLEr(unit, xrval));
            sal_usleep(100000);
        }
        SOC_IF_ERROR_RETURN(WRITE_TOP_TSC_ENABLEr(unit, rval));
        sal_usleep(100000);
    }

    return SOC_E_NONE;
}

#ifdef BCM_TOMAHAWKX_SUPPORT
static int
 _soc_tomahawk_phy_dev_port_check(int unit, int phy_port, int dev_port)
{
    int num_phy_port = 136;

    if( phy_port < 0 || phy_port >= num_phy_port) {
        return FALSE;
    } else if ( phy_port >= 1 && phy_port <= 32) {
        return ( dev_port >= 1 && dev_port <=32 );
    } else if ( phy_port >= 33 && phy_port <= 64) {
        return ( dev_port >= 34 && dev_port <= 66 );
    } else if ( phy_port >= 65 && phy_port <= 96) {
        return ( dev_port >= 68 && dev_port <= 100 );
    } else if ( phy_port >= 97 && phy_port <= 128) {
        return ( dev_port >= 102 && dev_port <= 133 );
    } else if ( phy_port == 129) {
        return ( dev_port >= 34 && dev_port <= 66 );
    } else if ( phy_port == 131) {
        return ( dev_port >= 68 && dev_port <= 100 );
    } else {
        /*skip LBPORT(132,133,134,135) , CPU port(0) and hole (130)*/
        return TRUE;
    }

}
#endif

/*
 * Tomahawk port mapping
 *
 *                   physical   idb/       device   mmu
 *                   port       idb port   port     port
 *     CMIC          0          0/32       0        32
 *     CLPORT0-7     1-32       0/0-31     1-32     0-31
 *     CLPORT8-15    33-64      1/0-31     34-66    64-95
 *     CLPORT16-23   65-96      2/0-31     68-100   128-159
 *     CLPORT24-31   97-128     3/0-31     102-133  192-223
 *     XLPORT0/0     129        1/32       34-66    96
 *     hole          130        3/32       134      224
 *     XLPORT0/2     131        2/32       68-100   160
 *     LBPORT0       132        0/33       33       33
 *     LBPORT1       133        1/33       67       97
 *     LBPORT2       134        2/33       101      161
 *     LBPORT3       135        3/33       135      225
 * Software uses device port 135 for LBPORT3 although it can be 134
 * Device port number is flexible within the above range
 * Although MMU port number is flexible within the above range, it is
 *     configured as a fixed mapped to physical port number
 */
int
soc_tomahawk_port_config_init(int unit, uint16 dev_id)
{
#ifdef BCM_TOMAHAWKX_SUPPORT
    soc_info_t *si;
    char *config_str, *sub_str, *sub_str_end;
    static const char str_2p5[] = "2.5";
    char str_buf[8];
    int rv, oversub_mode, frequency, os_mixed_sister_25_50_enable;
    int num_port, num_phy_port, num_mmu_port;
    int port, phy_port, mmu_port, idb_port;
    int pipe, xpe, sc, index, bandwidth_cap;
    int port_bandwidth, freq_list_len;
    char option1, option2;
    uint32 pipe_map, xpe_map, sc_map, ipipe_map, epipe_map;
    soc_pbmp_t oversub_pbm;
    static const int freq_list[] = { 850, 765, 672, 645, 545 };
    static const struct {
        int port;
        int phy_port;
        int pipe;
        int idb_port;
        int mmu_port;
    } fixed_ports[] = {
        { 0,   0,   0, 32, 32 },    /* cpu port */
        { 33,  132, 0, 33, 33 },    /* loopback port 0 */
        { 67,  133, 1, 33, 97 },    /* loopback port 1 */
        { 101, 134, 2, 33, 161 },   /* loopback port 2 */
        { 135, 135, 3, 33, 225 }    /* loopback port 3 */
    };

    si = &SOC_INFO(unit);

    si->flex_eligible = FALSE;

    num_phy_port = 136;
    num_port = 136;
    num_mmu_port = 226;

    /* Can't read TOP register at this point */
    soc_tomahawk_max_frequency_get(unit, dev_id, -1, -1, &si->frequency);
    frequency = soc_property_get(unit, spn_CORE_CLOCK_FREQUENCY, -1);
    /*frequency = rev_id == BCM56965_A0_REV_ID ? 545 : frequency;*/

    si->bandwidth = /*rev_id == BCM56965_A0_REV_ID ? 1024000 :*/ 2048000;

    if (frequency != -1 && frequency < si->frequency) {
        freq_list_len = COUNTOF(freq_list);
        for (index = 1; index < freq_list_len; index++) { /* skip index 0 */
            if (freq_list[index] < si->frequency &&
                frequency == freq_list[index]) {
                break;
            }
        }

        if (index < freq_list_len) {
            si->frequency = frequency;
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "*** Input core clock frequency %dMHz is not "
                                "supported!\n"), frequency));
        }
    } else if (frequency > si->frequency) {
        LOG_CLI((BSL_META_U(unit,
                            "*** Input core clock frequency %dMHz is not supported!\n"),
                 frequency));
    }


    SOC_PBMP_CLEAR(oversub_pbm);
    oversub_mode = soc_property_get(unit, spn_OVERSUBSCRIBE_MODE, 0);
    oversub_pbm  = soc_property_get_pbmp(unit, spn_PBMP_OVERSUBSCRIBE, 0);
    oversub_mode = oversub_mode || SOC_PBMP_NOT_NULL(oversub_pbm);
    si->oversub_mode = oversub_mode;
    os_mixed_sister_25_50_enable = soc_property_get(unit, spn_OVERSUBSCRIBE_MIXED_SISTER_25_50_ENABLE, 0);
    if (oversub_mode) {
        si->os_mixed_sister_25_50_enable = os_mixed_sister_25_50_enable;
    } else {
        si->os_mixed_sister_25_50_enable = 0;
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
    for (pipe = 0; pipe < SOC_MAX_NUM_PIPES; pipe++) {
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
        if (port == 134) { /* SDK uses device port 135 for loopback port 3 */
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
        if (!_soc_tomahawk_phy_dev_port_check(unit, phy_port, port)) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Invalid mapping physical port number %d\n"),
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

        /* Check if option presents */
        option1 = 0; option2 = 0;
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            /* Skip ':' between bandwidth and cap or options */
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
                /* For distinguish 1G cap or lane 1, left-to-right */
                if ((bandwidth_cap == 1000) &&
                    (bandwidth_cap < port_bandwidth)) {
                    bandwidth_cap = 0;
                }
                switch (bandwidth_cap) {
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
                    sub_str = sub_str_end;
                    _soc_th_port_speed_cap[unit][port] = bandwidth_cap;
                    /* Flex config detected, port speed cap specified */
                    si->flex_eligible = TRUE;
                    break;
                default:
                    if (!(*sub_str == 'i' || *sub_str == '1' || *sub_str == '2' ||
                          *sub_str == '4')) {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string \"%s\"\n"),
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

                    if (*sub_str != 'i') {
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
            si->port_l2i_mapping[port] = 32;
            si->port_p2m_mapping[phy_port] = 96;
            si->port_serdes[port] = 32;
            SOC_PBMP_PORT_ADD(si->management_pbm, port);
        } else if (phy_port == 131) { /* management port 1 */
            pipe = 2;
            si->port_l2i_mapping[port] = 32;
            si->port_p2m_mapping[phy_port] = 160;
            si->port_serdes[port] = 32;
            SOC_PBMP_PORT_ADD(si->management_pbm, port);
        } else {
            idb_port = (phy_port - 1) % _TH_PORTS_PER_PIPE;
            pipe = (phy_port - 1) / _TH_PORTS_PER_PIPE;
            si->port_l2i_mapping[port] = idb_port;
            si->port_p2m_mapping[phy_port] =
                (pipe << 6) | ((idb_port & 0x1) << 4) | (idb_port >> 1);
            si->port_serdes[port] = (phy_port - 1) / _TH_PORTS_PER_PBLK;
        }
        si->port_pipe[port] = pipe;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);
        si->port_speed_max[port] = port_bandwidth;
        if (option1 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
        } else {
            switch (option1) {
            case '1': si->port_num_lanes[port] = 1; break;
            case '2': si->port_num_lanes[port] = 2; break;
            case '4': si->port_num_lanes[port] = 4; break;
            default: break;
            }
        }
        if (option2 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
        }
        if (oversub_mode) {
            SOC_PBMP_PORT_ADD(si->oversub_pbm, port);
        }
    }
    /* Flex config detected, disabled ports in config */
    if (SOC_PBMP_NOT_NULL(SOC_PORT_DISABLED_BITMAP(unit, all))) {
        si->flex_eligible = TRUE;
    }

    for (index = 1; index < COUNTOF(fixed_ports); index++) {
        pipe = fixed_ports[index].pipe;
        port = fixed_ports[index].port;
        phy_port = fixed_ports[index].phy_port;
        if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            /* Remove loopback port if the entire pipe is not in use */
            si->port_l2p_mapping[port] = -1;
            si->port_l2i_mapping[port] = -1;
            si->port_p2l_mapping[phy_port] = -1;
            si->port_p2m_mapping[phy_port] = -1;
            si->port_pipe[port] = -1;
        } else {
            SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);
        }
    }

    /* Since in tomahawk, CPU port's Queues and Scheduling nodes are bound to
       pipeline 0, add CPU Port to PIPE_PBM of PIPE 0 */
    SOC_PBMP_PORT_ADD(si->pipe_pbm[0], CMIC_PORT(unit));

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
            if (si->port_num_lanes[port] == -1) {
                /* But RXAUI and XAUI on mgmt ports use 2 and 4 lanes, which
                 * can be provided via the portmap interface.
                 */
                si->port_num_lanes[port] = 1;
            }
        }
    }

    /* Setup high speed port (HSP) pbm */
    for (port = 0; port < num_port; port++) {
        phy_port = si->port_l2p_mapping[port];
        if (phy_port == -1) {
            continue;
        }
        if (si->port_speed_max[port] >= 40000) {
            SOC_PBMP_PORT_ADD(si->eq_pbm, port);
        }
    }

    /* Setup pipe/xpe mapping
     * XPE0 (SC_R/XPE_A): IP 0/3 EP 0/1
     * XPE1 (SC_S/XPE_A): IP 0/3 EP 2/3
     * XPE2 (SC_R/XPE_B): IP 1/2 EP 0/1
     * XPE3 (SC_S/XPE_B): IP 1/2 EP 2/3
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
        ipipe_map = (xpe < 2 ? 0x9 : 0x6) & pipe_map;
        epipe_map = (xpe & 1 ? 0xc : 0x3) & pipe_map;
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
        epipe_map = (sc & 1 ? 0xc : 0x3) & pipe_map;
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
        si->ipipe_xpe_map[pipe] = (pipe == 0 || pipe == 3 ? 0x3 : 0xc) &
            xpe_map;
        /* coverity[overrun-local : FALSE] */
        si->epipe_xpe_map[pipe] = (pipe < 2 ? 0x5 : 0xa) & xpe_map;
        /* coverity[overrun-local : FALSE] */
        si->ipipe_sc_map[pipe] = sc_map;
        /* coverity[overrun-local : FALSE] */
        si->epipe_sc_map[pipe] = (pipe < 2 ? 0x1 : 0x2) & sc_map;
    }

    return rv;
#else
    return SOC_E_UNAVAIL;
#endif
}

#if defined(BCM_TOMAHAWK2_SUPPORT)
/*
 * Function:
 *      soc_th2_port_max_oversub_ratio_get
 * Purpose:
 *      Get the max OVS ratio among all half pipes in the device.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ratio - (OUT) Max OVS ratio among all half pipes in the device
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */
STATIC int
soc_th2_port_max_oversub_ratio_get(
    int              unit,
    OUT int* const   ratio)
{
    soc_info_t           *si;
    _soc_tomahawk2_tdm_t *tdm;
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
    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        for (hpipe = 0; hpipe < _TH2_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            if (max_ovs_ratio < tdm->ovs_ratio_x1000[pipe][hpipe]) {
                max_ovs_ratio = tdm->ovs_ratio_x1000[pipe][hpipe];
            }
        }
    }
    *ratio = max_ovs_ratio;

    return SOC_E_NONE;
}
#endif
soc_error_t
soc_th_subsidiary_ports_get(int unit, soc_port_t port, soc_pbmp_t *pbmp)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_t phy_port_base, phy_port;
    soc_pbmp_t pbm;
    int max_subport;

    if (!SOC_PORT_VALID(unit, port) || IS_CPU_PORT(unit, port) ||
        IS_LB_PORT(unit, port) ||
        SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port)) {
         /* CPU, LB and Management ports do not support flexport */
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

#ifdef BCM_TOMAHAWK3_SUPPORT
    /* Tomahawk 3 has 8 lanes per PM instead of 4 */
    if (SOC_IS_TOMAHAWK3(unit)) {
        max_subport = phy_port + (8 - ((phy_port - 1) % 8));
    } else
#endif
    {
        max_subport = phy_port + (4 - ((phy_port - 1) % 4));
    }
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
soc_th_port_oversub_ratio_get(
    int              unit,
    soc_port_t       port,
    OUT int* const   ratio)
{
    soc_info_t          *si;
    _soc_tomahawk_tdm_t *tdm;
    int                  pipe;

    if (!ratio) {
        return SOC_E_PARAM;
    }

#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        return soc_th2_port_max_oversub_ratio_get(unit, ratio);
    }
#endif

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    if (!si || !tdm) {
        return SOC_E_INTERNAL;
    }

    pipe = si->port_pipe[port];
    *ratio = tdm->ovs_ratio_x1000[pipe];

    return SOC_E_NONE;
}

uint32 _soc_th_temp_mon_mask[SOC_MAX_NUM_DEVICES];

static int
_soc_th_freq_val_sel(int frequency)
{
    switch (frequency) {
    case 850:
        return 1;
    case 765:
        return 2;
    case 672:
        return 3;
    case 645:
        return 4;
    case 545:
        return 5;
    default:
        return 1;
    }
}

STATIC int _soc_tomahawk_clear_all_memory(int unit, int mmu_init);
STATIC int _soc_tomahawk_tdm_calculation_free_memory(int unit);

int
soc_tomahawk_chip_reset(int unit)
{
#define _SOC_TH_LCPLL_COUNT    4
#define _SOC_TH_PLL_CFG_FIELDS 4
#define _SOC_TH_DEF_TEMP_MAX   125
#define _SOC_TH_NUM_PORTS      136
#define _SOC_TH_LINK_DELAY_REG_BASE   0x20000700   /* EGR_1588_LINK_DELAY_64 */
#define _SOC_TH_LINK_DELAY_REG_BLK    2            /* EPIPE BLOCKNUM */

    soc_info_t *si;
    uint16 dev_id;
    uint8 rev_id;
    uint32 to_usec, temp_mask;
    uint32 rval, temp_thr;
    soc_reg_t reg;
    int freq_sel, freq_list_len, max_frequency;
    int num_banks, index, shared_bank, shared_bank_offset = 0;
    int num_shared_l2_banks, num_shared_l3_banks, num_shared_fpem_banks;
    uint32 config = 0, map = 0, fpmap = 0;
    soc_field_t fields[_SOC_TH_PLL_CFG_FIELDS];
    uint32 values[_SOC_TH_PLL_CFG_FIELDS];
    static soc_field_t l2_fields[] = {
        L2_ENTRY_BANK_2f, L2_ENTRY_BANK_3f, L2_ENTRY_BANK_4f,
        L2_ENTRY_BANK_5f
    };
    static soc_field_t l3_fields[] = {
        L3_ENTRY_BANK_4f, L3_ENTRY_BANK_5f, L3_ENTRY_BANK_6f,
        L3_ENTRY_BANK_7f
    };
    static soc_field_t fpem_fields[] = {
        FPEM_ENTRY_BANK_0f, FPEM_ENTRY_BANK_1f, FPEM_ENTRY_BANK_2f,
        FPEM_ENTRY_BANK_3f
    };
    static const soc_reg_t lcpll_ctrl0_reg[] = {
        TOP_XG_PLL0_CTRL_0r, TOP_XG_PLL1_CTRL_0r,
        TOP_XG_PLL2_CTRL_0r, TOP_XG_PLL3_CTRL_0r
    };
    static const soc_reg_t lcpll_ctrl4_reg[] = {
        TOP_XG_PLL0_CTRL_4r, TOP_XG_PLL1_CTRL_4r,
        TOP_XG_PLL2_CTRL_4r, TOP_XG_PLL3_CTRL_4r
    };
    static const soc_reg_t lcpll_status_reg[] = {
        TOP_XG_PLL0_STATUSr, TOP_XG_PLL1_STATUSr,
        TOP_XG_PLL2_STATUSr, TOP_XG_PLL3_STATUSr
    };
    static const soc_reg_t temp_thr_reg[] = {
        TOP_PVTMON_0_INTR_THRESHOLDr, TOP_PVTMON_1_INTR_THRESHOLDr,
        TOP_PVTMON_2_INTR_THRESHOLDr, TOP_PVTMON_3_INTR_THRESHOLDr,
        TOP_PVTMON_4_INTR_THRESHOLDr, TOP_PVTMON_5_INTR_THRESHOLDr,
        TOP_PVTMON_6_INTR_THRESHOLDr, TOP_PVTMON_7_INTR_THRESHOLDr
    };
    static const char *temp_thr_prop[] = {
        "temp0_threshold", "temp1_threshold", "temp2_threshold",
        "temp3_threshold", "temp4_threshold", "temp5_threshold",
        "temp6_threshold", "temp7_threshold"
    };
    static const int freq_list[] = { 850, 765, 672, 645, 545 };

    int port, acc;
    uint64 rval64;

    /* Settings for 500 MHz TSPLL output clock. */
    unsigned ts_ref_freq = 0;
    unsigned ts_idx = 0;

    static const soc_pll_param_t ts_pll[] = {
    /*     Fref, Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
      {12800000,      195,   0x50000,     1,    5,  0,  2,  3,        1},
      {20000000,      125,         0,     1,    5,  0,  2,  3,        1},
      {25000000,      100,         0,     1,    5,  0,  2,  3,        1},
      {32000000,       78,   0x20000,     1,    5,  0,  2,  3,        1},
      {50000000,      100,         0,     2,    5,  0,  2,  3,        1},
      {       0,      100,         0,     2,    5,  0,  2,  3,        1} /* 50 MHz internal reference. */
    };

    /* Settings for 20 MHz BSPLL output clock. */
    unsigned bs_ref_freq = 0;
    unsigned bs_idx = 0;

    static const soc_pll_param_t bs_pll[] = {
    /*     Fref, Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
      {12800000,      195,   0x50000,     1,  125,  0,  2,  3,        1},
      {20000000,      125,         0,     1,  125,  0,  2,  3,        1},
      {25000000,      100,         0,     1,  125,  0,  2,  3,        1},
      {32000000,       78,   0x20000,     1,  125,  0,  2,  3,        1},
      {50000000,      100,         0,     2,  125,  0,  2,  3,        1},
      {       0,      100,         0,     2,  125,  0,  2,  3,        1} /* 50 MHz internal reference. */
    };

    si = &SOC_INFO(unit);
    soc_cm_get_id(unit, &dev_id, &rev_id);
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
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x55222100);
        WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x30004005);
        WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x43333333);
        WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x44444444);
        WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x34444444);
        WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x03333333);
        WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x30000000);
        WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x00005000);
        WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x7d0);
    }
#endif

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_0_7_OFFSET,0x55222100);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_8_15_OFFSET,0x30004005);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_16_23_OFFSET,0x43333333);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_24_31_OFFSET,0x44444444);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_32_39_OFFSET,0x34444444);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_40_47_OFFSET,0x03333333);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_48_55_OFFSET,0x30000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_56_63_OFFSET,0x00005000);
        soc_pci_write(unit, CMIC_TOP_SBUS_TIMEOUT_OFFSET,0x7d0);
    }
#endif
    sal_usleep(to_usec);


    SOC_IF_ERROR_RETURN
        (soc_tomahawk_max_frequency_get(unit, dev_id, rev_id, -1,
                                        &max_frequency));
    if ((si->frequency != max_frequency)
        || (si->frequency != freq_list[0])) {
        if (si->frequency > max_frequency) {
            si->frequency = max_frequency;
        }
        freq_list_len = COUNTOF(freq_list);
        for (freq_sel = 1; freq_sel < freq_list_len; freq_sel++) { /* skip index 0 */
            if (si->frequency == freq_list[freq_sel]) {
                break;
            }
        }
        if (freq_sel < freq_list_len) {
            LOG_CLI((BSL_META_U(unit,
                                "*** change core clock frequency to %dMHz\n"),
                                si->frequency));

            freq_sel = _soc_th_freq_val_sel(si->frequency);
            fields[0] = CORE_CLK_0_FREQ_SELf;
            values[0] = freq_sel;
            fields[1] = SW_CORE_CLK_0_SEL_ENf;
            values[1] = 1;
            SOC_IF_ERROR_RETURN
                (soc_reg_fields32_modify(unit, TOP_CORE_CLK_FREQ_SELr,
                                         REG_PORT_ANY, 2, fields, values));
            sal_usleep(to_usec);
        }
    }
    else {
        /* Set correct frequency in non OTP'd SKUs */
        SOC_IF_ERROR_RETURN(READ_TOP_DEV_REV_IDr(unit, &rval));
        if (!(soc_reg_field_get(unit, TOP_DEV_REV_IDr, rval, DEVICE_SKEWf) & 0x8)) {
            SOC_IF_ERROR_RETURN
                (soc_tomahawk_max_frequency_get(unit, dev_id, rev_id, -1,
                                                &max_frequency));
            LOG_CLI((BSL_META_U(unit,
                                "*** change core clock frequency to %dMHz\n"),
                                max_frequency));
            freq_sel = _soc_th_freq_val_sel(max_frequency);
            fields[0] = CORE_CLK_0_FREQ_SELf;
            values[0] = freq_sel;
            fields[1] = SW_CORE_CLK_0_SEL_ENf;
            values[1] = 1;
            SOC_IF_ERROR_RETURN
                (soc_reg_fields32_modify(unit, TOP_CORE_CLK_FREQ_SELr,
                                         REG_PORT_ANY, 2, fields, values));
            sal_usleep(to_usec);
        }
    }

    /* Power on core pll1 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_CORE_PLL1_CTRL_1r,
                                REG_PORT_ANY, PWRONf, 0));
    /* Program LCPLL frequency */
    for (index = 0; index < COUNTOF(lcpll_ctrl0_reg); index++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, lcpll_ctrl4_reg[index],
                                    REG_PORT_ANY, POST_RST_SELf, 3));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, lcpll_ctrl0_reg[index],
                                    REG_PORT_ANY, PDIVf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, lcpll_ctrl0_reg[index],
                                    REG_PORT_ANY, NDIV_INTf, 20));
    }

    /* Configure TS PLL */
    ts_ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0);  /* 0->internal reference */

    for (ts_idx = 0; ts_idx < sizeof(ts_pll)/sizeof(ts_pll[0]); ++ts_idx) {
        if (ts_pll[ts_idx].ref_freq == ts_ref_freq) {
            break;
        }
    }

    if (ts_idx == sizeof(ts_pll)/sizeof(ts_pll[0])) {
        /* Valid configuration lookup failed. Use internal reference. */
        ts_idx = 5;
        ts_ref_freq = 0;
    }

    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_0r, &rval, POST_RESETB_SELECTf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_0r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_1r, & rval, FREF_SELf, (ts_ref_freq != 0));
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_2r(unit,&rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, PDIVf,
                      soc_property_get(unit, spn_PTP_TS_PLL_PDIV, ts_pll[ts_idx].pdiv));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, CH0_MDIVf,
                      soc_property_get(unit, spn_PTP_TS_PLL_MNDIV, ts_pll[ts_idx].mdiv));
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_3r(unit,&rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, NDIV_INTf,
                      soc_property_get(unit, spn_PTP_TS_PLL_N, ts_pll[ts_idx].ndiv_int));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, NDIV_FRACf, ts_pll[ts_idx].ndiv_frac);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_3r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_4r(unit,&rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KAf,
                      soc_property_get(unit, spn_PTP_TS_KA, ts_pll[ts_idx].ka));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KIf,
                      soc_property_get(unit, spn_PTP_TS_KI, ts_pll[ts_idx].ki));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KPf,
                      soc_property_get(unit, spn_PTP_TS_KP, ts_pll[ts_idx].kp));
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_4r(unit, rval));

    /* Set 250Mhz (implies 4ns resolution) default timesync clock to
       calculate assymentric delays */
    SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nanoseconds */


    /* Configure BroadSync PLLs. */
    bs_ref_freq = soc_property_get(unit, spn_PTP_BS_FREF, 0);  /* 0->internal reference */

    for (bs_idx = 0; bs_idx < sizeof(bs_pll)/sizeof(bs_pll[0]); ++bs_idx) {
        if (bs_pll[bs_idx].ref_freq == bs_ref_freq) {
            break;
        }
    }

    if (bs_idx == sizeof(bs_pll)/sizeof(bs_pll[0])) {
        /* Valid configuration lookup failed. Use internal reference. */
        bs_idx = 5;
        bs_ref_freq = 0;
    }

    /* Configure BS PLL0 */
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_0r, &rval, POST_RESETB_SELECTf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_0r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, & rval, FREF_SELf, (bs_ref_freq != 0));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_2r, &rval, PDIVf,
                      soc_property_get(unit, spn_PTP_BS_PDIV, bs_pll[bs_idx].pdiv));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_2r, &rval, CH0_MDIVf,
                      soc_property_get(unit, spn_PTP_BS_MNDIV, bs_pll[bs_idx].mdiv));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, NDIV_INTf,
                      soc_property_get(unit, spn_PTP_BS_NDIV_INT, bs_pll[bs_idx].ndiv_int));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, NDIV_FRACf,
                      soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs_pll[bs_idx].ndiv_frac));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_3r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_4r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KAf,
                      soc_property_get(unit, spn_PTP_BS_KA, bs_pll[bs_idx].ka));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KIf,
                      soc_property_get(unit, spn_PTP_BS_KI, bs_pll[bs_idx].ki));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KPf,
                      soc_property_get(unit, spn_PTP_BS_KP, bs_pll[bs_idx].kp));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_4r(unit, rval));

    /* Configure BS PLL1 */
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_0r, &rval, POST_RESETB_SELECTf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_0r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, & rval, FREF_SELf, (bs_ref_freq != 0));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_2r, &rval, PDIVf,
                      soc_property_get(unit, spn_PTP_BS_PDIV, bs_pll[bs_idx].pdiv));
    soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_2r, &rval, CH0_MDIVf,
                      soc_property_get(unit, spn_PTP_BS_MNDIV, bs_pll[bs_idx].mdiv));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_3r, &rval, NDIV_INTf,
                      soc_property_get(unit, spn_PTP_BS_NDIV_INT, bs_pll[bs_idx].ndiv_int));
    soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_3r, &rval, NDIV_FRACf,
                      soc_property_get(unit, spn_PTP_BS_NDIV_FRAC, bs_pll[bs_idx].ndiv_frac));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_3r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_4r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_4r, &rval, KAf,
                      soc_property_get(unit, spn_PTP_BS_KA, bs_pll[bs_idx].ka));
    soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_4r, &rval, KIf,
                      soc_property_get(unit, spn_PTP_BS_KI, bs_pll[bs_idx].ki));
    soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_4r, &rval, KPf,
                      soc_property_get(unit, spn_PTP_BS_KP, bs_pll[bs_idx].kp));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_4r(unit, rval));

    /* Bring LCPLL, time sync PLL, BroadSync PLL, AVS out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL0_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL1_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL2_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL3_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);

    if (!SAL_BOOT_SIMULATION) {
        /* Check LCPLL lock status */
        for (index = 0; index < _SOC_TH_LCPLL_COUNT; index++) {
            reg = lcpll_status_reg[index];
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            if (!soc_reg_field_get(unit, reg, rval, TOP_XGPLL_LOCKf)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "LCPLL %d not locked on unit %d "
                                      "status = 0x%08x\n"), index, unit, rval));
            }
        }
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
    }

    /* De-assert LCPLL's post reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_XG_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_XG_PLL1_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_XG_PLL2_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_XG_PLL3_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TS_PLL_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL1_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);

    /* Set correct value for BG_ADJ */
    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_0r, &rval, BG_ADJf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_0r(unit, rval));

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

        /* data = 410.04-(temp/0.48705) = (410040-(temp*1000))/487
         * Note: Since this is a high temp value we can safely assume it to
         * always be a +ive number. This is in degrees celcius.
         */
        temp_thr = soc_property_get(unit, temp_thr_prop[index], _SOC_TH_DEF_TEMP_MAX);
        if (temp_thr > 410) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Unsupported temp value %d !! Max 410. Using %d.\n"),
                       temp_thr, _SOC_TH_DEF_TEMP_MAX));
            temp_thr = _SOC_TH_DEF_TEMP_MAX;
        }
        /* Convert temperature to config data */
        temp_thr = (410040-(temp_thr*1000))/487;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, temp_thr_reg[index], REG_PORT_ANY, 0, &trval));
        soc_reg_field_set(unit, temp_thr_reg[index], &trval, MIN_THRESHOLDf,
                          temp_thr);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, temp_thr_reg[index], REG_PORT_ANY, 0, trval));
        if (temp_mask & (1 << index)) {
            rval |= (1 << ((index * 2) + 1)); /* 2 bits per pvtmon, using min */
        }
    }
    _soc_th_temp_mon_mask[unit] = temp_mask;

    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_INTR_MASKr(unit, rval));
    /* Enable temp mon interrupt */
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        (void)soc_cmicm_intr3_enable(unit, 0x4); /* PVTMON_INTR bit 2 */
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
    sal_usleep(to_usec);

    if (!SAL_BOOT_PLISIM) {
        COMPILER_64_ZERO(rval64);

        for (port = 0; port < _SOC_TH_NUM_PORTS; port++) {
            for (acc = 0; acc < _TH_PIPES_PER_DEV; acc++) {
                SOC_IF_ERROR_RETURN(
                    _soc_reg64_set(unit, _SOC_TH_LINK_DELAY_REG_BLK, acc,
                                   (_SOC_TH_LINK_DELAY_REG_BASE + port),
                                   rval64));
            }
        }
    }

    /* Must do this before setting low-latency mode */
    SOC_IF_ERROR_RETURN(_soc_tomahawk_clear_all_memory(unit, 0));

    /* Configure Switch Latency Bypass Mode */
    SOC_IF_ERROR_RETURN(soc_th_latency_init(unit));

    SOC_IF_ERROR_RETURN
        (soc_tomahawk_hash_bank_count_get(unit, L2Xm, &num_banks));
    num_shared_l2_banks = num_banks - 2; /* minus 2 dedicated L2 banks */
    SOC_IF_ERROR_RETURN
        (soc_tomahawk_hash_bank_count_get(unit, L3_ENTRY_ONLYm, &num_banks));
    num_shared_l3_banks = num_banks - 4; /* minus 4 dedicated L3 banks */
    SOC_IF_ERROR_RETURN
        (soc_tomahawk_hash_bank_count_get(unit, EXACT_MATCH_2m,
                                          &num_shared_fpem_banks));

    if (soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m)) {
        int non_alpm = num_shared_fpem_banks + num_shared_l3_banks +
            num_shared_l2_banks;
        if ((non_alpm) && (non_alpm <= 2)) {
            /* If Shared banks are used between ALPM and non-ALPM memories,
             * then ALPM uses Shared Bank B2, B3 and non-ALPM uses B4, B5 banks
             */
            soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                              ALPM_ENTRY_BANK_CONFIGf, 0x3);
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                              ALPM_BANK_MODEf, 1);
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_LOG_TO_PHY_BANK_MAP_2r,
                                        REG_PORT_ANY, ALPM_BANK_MODEf, 1));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_MEMORY_CONTROL_84r,
                                        REG_PORT_ANY, BYPASS_ISS_MEMORY_LPf, 0x3));
            soc_th_set_alpm_banks(unit, 2);
            shared_bank_offset = 2;
        } else {
            soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                              ALPM_ENTRY_BANK_CONFIGf, 0xf);
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                              ALPM_BANK_MODEf, 0);
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_LOG_TO_PHY_BANK_MAP_2r,
                                        REG_PORT_ANY, ALPM_BANK_MODEf, 0));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_MEMORY_CONTROL_84r,
                                        REG_PORT_ANY, BYPASS_ISS_MEMORY_LPf, 0xf));
            soc_th_set_alpm_banks(unit, 4);
        }
    }

    soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config, L2_ENTRY_BANK_CONFIGf,
                      ((1 << num_shared_l2_banks) - 1) << shared_bank_offset);
    for (index = 0; index < num_shared_l2_banks; index++) {
        shared_bank = index + shared_bank_offset;
        soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                          l2_fields[index], shared_bank);
    }
    shared_bank_offset += num_shared_l2_banks;

    soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config, L3_ENTRY_BANK_CONFIGf,
                      ((1 << num_shared_l3_banks) - 1) << shared_bank_offset);
    for (index = 0; index < num_shared_l3_banks; index++) {
        shared_bank = index + shared_bank_offset;;
        soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                          l3_fields[index], shared_bank);
    }
    shared_bank_offset += num_shared_l3_banks;

    soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                      FPEM_ENTRY_BANK_CONFIGf,
                      ((1 << num_shared_fpem_banks) - 1) << shared_bank_offset);
    for (index = 0; index < num_shared_fpem_banks; index++) {
        shared_bank = index + shared_bank_offset;
        soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                          fpem_fields[index], shared_bank);
        soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAP_3r, &fpmap,
                          fpem_fields[index], shared_bank);
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, ISS_BANK_CONFIGr, REG_PORT_ANY, 0, config));
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, REG_PORT_ANY, 0, map));
    SOC_IF_ERROR_RETURN
        (soc_th_iss_log_to_phy_bank_map_shadow_set(unit, map));
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, ISS_LOG_TO_PHY_BANK_MAP_3r, REG_PORT_ANY, 0, fpmap));

    /* Bypass unused Port Blocks */
    if (soc_property_get(unit, "disable_unused_port_blocks", TRUE)) {
        SOC_IF_ERROR_RETURN(soc_th_bypass_unused_pm(unit));
    }

    if (!(SOC_CONTROL(unit)->soc_flags & SOC_F_ALL_MODULES_INITED) &&
        soc_feature(unit, soc_feature_turbo_boot)) {
        SOC_HW_RESET_START(unit);
    }

#ifdef BCM_IPROC_SUPPORT
    /* Reset all mHost processors */
    SOC_IF_ERROR_RETURN(WRITE_MHOST_0_CR5_RST_CTRLr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_MHOST_1_CR5_RST_CTRLr(unit, 0));
    /*Reset the common UARTs */
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONG_UART0_UART_SRRr(unit, 1));
    SOC_IF_ERROR_RETURN(WRITE_CHIPCOMMONG_UART1_UART_SRRr(unit, 1));
#endif

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      soc_tomahawk_chip_warmboot_reset
 * Purpose:
 *      Special re-init sequencing for BCM56960 during warmboot
 */
int
soc_tomahawk_chip_warmboot_reset(int unit)
{
    uint32 map = 0;

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, ISS_LOG_TO_PHY_BANK_MAPr, REG_PORT_ANY, 0, &map));
    SOC_IF_ERROR_RETURN
        (soc_th_iss_log_to_phy_bank_map_shadow_set(unit, map));

    return SOC_E_NONE;
}
#endif

int
soc_tomahawk_port_reset(int unit)
{
    uint32 rval;
    int blk, port;
    int sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (soc_feature(unit, soc_feature_pm_refclk_master)) {
        soc_info_t *si = &SOC_INFO(unit);
        if (!SHR_BITNULL_RANGE(si->pm_ref_master_bitmap, 0,
                                 SOC_MAX_NUM_BLKS)) {
            soc_xgxs_reset_master_tsc(unit);
        }
    }
#endif

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);

        SOC_IF_ERROR_RETURN(READ_CLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MAC_CONTROLr(unit, port, rval));
        sal_udelay(10);
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
        /* PM interface is 16B in TH2 */
        if (soc_feature(unit, soc_feature_clmac_16byte_interface_mode)) {
            soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval,
                                SYS_16B_INTF_MODEf, 1);
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
soc_tomahawkplus_port_reset(int unit)
{
    uint32 rval;
    int blk, port;
    int sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;

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

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);

        SOC_IF_ERROR_RETURN(READ_CLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MAC_CONTROLr(unit, port, rval));
        sal_udelay(10);
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
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
    }
    return SOC_E_NONE;
}


/* Configure HSP port multicast T2OQ setting.
 * Note: Caller must check if the Port is part of HSP port bitmap
 *       HSP Port check:  SOC_PBMP_MEMBER(si->eq_pbm, port)
 */

STATIC int
_soc_tomahawk_mc_toq_cfg(int unit, int port, int enable)
{
    static soc_field_t t2oq_fields[] = {
        IS_MC_T2OQ_PORT0f, IS_MC_T2OQ_PORT1f
    };
    soc_field_t field;
    uint32 rval, fval;
    int inst, pipe, mmu_port;
    soc_reg_t reg = MMU_SCFG_TOQ_MC_CFG1r;
    soc_info_t *si = &SOC_INFO(unit);;

    pipe = si->port_pipe[port];
    field = t2oq_fields[pipe & 0x01];
    inst = (pipe >> 1) | SOC_REG_ADDR_INSTANCE_MASK;
    mmu_port = si->port_p2m_mapping[si->port_l2p_mapping[port]];

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
    fval = soc_reg_field_get(unit, reg, rval, field);
    if (enable) {
        fval |= 1 << (mmu_port & 0x0f);
    } else {
        fval &= ~(1 << (mmu_port & 0x0f));
    }
    soc_reg_field_set(unit, reg, &rval, field, fval);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;
}

/*
 * Utility function to update multicast initial copy count
 * Invoked during speed set and flexport operations.
 */
STATIC soc_error_t
soc_tomahawk_update_icc(int unit, soc_port_t port, int port_speed)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port, count_width;
    uint32 regval = 0;

    phy_port = si->port_l2p_mapping[port];
    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        phy_port == 129 || phy_port == 131) {
        /* For CPU, loopback, and management ports,
         * the copy count width is fixed at 3 bits (encoded value of 1).
         * Pysical ports 129 and 131 are management ports.
         */
        count_width = 1;
    } else {
        /* For ports with speed 40G and above the copy count width
         * is 3 bits (encoded value of 1) for lower speeds its
         * 2 bits (encoded value of 0).
         */
        count_width = (port_speed >= 40000) ? 1 : 0;
    }
    soc_reg_field_set(unit, PORT_INITIAL_COPY_COUNT_WIDTHr, &regval,
                      BIT_WIDTHf, count_width);
    SOC_IF_ERROR_RETURN(WRITE_PORT_INITIAL_COPY_COUNT_WIDTHr
                           (unit, port, regval));

    return SOC_E_NONE;
}

/*
 * Function to update Port PG's Headroom allocation.
 * Port PG needs to be updated during
 *  1. Flex (bcmPortControlLanes) operation
 *  2. Port Speed update (bcm_port_speed_set api)
 * Note: This function will update the Port's PG Headroom to the
 * updated port speed (soc_info.port_init_speed).
 */
STATIC
int soc_tomahawk_port_pg_headroom_update(int unit, soc_port_t port)
{
#define DEFAULT_PG_NUM 7  /* Default Priority Group Num */
    int headroom, lossless, pipe, midx;
    soc_mem_t mem;
    thdi_port_pg_config_entry_t pg_config_mem_entry;

    lossless = SOC_INFO(unit).mmu_lossless;
    headroom = _soc_th_default_pg_headroom(unit, port, lossless);
    pipe = SOC_INFO(unit).port_pipe[port];
    mem = SOC_MEM_UNIQUE_ACC(unit, THDI_PORT_PG_CONFIGm)[pipe];
    midx = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port, THDI_PORT_PG_CONFIGm,
                                      DEFAULT_PG_NUM);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, midx, &pg_config_mem_entry));
    soc_mem_field32_set(unit, mem, &pg_config_mem_entry,
                        PG_HDRM_LIMITf, headroom);
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, &pg_config_mem_entry));
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_port_mixed_speed_check(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_t phy_port_base, phy_port;
    soc_port_t port_sub, port_sis;
    int port_sub_speed, port_sis_speed;
    soc_port_t port_base = lanes_ctrl->port_base;

    phy_port = si->port_l2p_mapping[port_base];
    if (-1 == phy_port) {
        return SOC_E_PORT;
    }
    phy_port_base = PORT_BLOCK_BASE_PORT(port_base);
    if ((phy_port_base != phy_port) && ((phy_port_base + 2) != phy_port)) {
        /* Not a controlling port, skip the checking */
        return SOC_E_NONE;
    }

    /* check mixed sister port mode */
    port_sub = si->port_p2l_mapping[phy_port_base];
    port_sis = si->port_p2l_mapping[phy_port_base + 2];
    if (phy_port_base == phy_port) {
        port_sub_speed = lanes_ctrl->speed;
    } else {
        port_sub_speed = si->port_init_speed[port_sub];
    }
    if ((phy_port_base + 2) == phy_port) {
        port_sis_speed = lanes_ctrl->speed;
    } else {
        port_sis_speed = si->port_init_speed[port_sis];
    }

    if (!SOC_PBMP_MEMBER(si->all.disabled_bitmap, si->port_p2l_mapping[phy_port_base + 1]) ||
        !SOC_PBMP_MEMBER(si->all.disabled_bitmap, si->port_p2l_mapping[phy_port_base + 3])) {
        return SOC_E_PORT;
    }

    if (((port_sub_speed == 50000) && (port_sis_speed == 50000)) ||
        ((port_sub_speed == 53000) && (port_sis_speed == 53000))) {
        lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_50_0_50_0;
    } else if (((port_sub_speed == 50000) && (port_sis_speed == 25000)) ||
               ((port_sub_speed == 53000) && (port_sis_speed == 27000))) {
               lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_50_0_25_0;
    } else if (((port_sub_speed == 25000) && (port_sis_speed == 50000)) ||
               ((port_sub_speed == 27000) && (port_sis_speed == 53000))) {
               lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_25_0_50_0;
    } else if (((port_sub_speed == 25000) && (port_sis_speed == 25000)) ||
               ((port_sub_speed == 27000) && (port_sis_speed == 27000))) {
               lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_25_0_25_0;
    } else if (((port_sub_speed == 40000) && (port_sis_speed == 40000)) ||
               ((port_sub_speed == 43000) && (port_sis_speed == 43000))) {
               lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_40_0_40_0;
    } else if (((port_sub_speed == 20000) && (port_sis_speed == 20000)) ||
               ((port_sub_speed == 21000) && (port_sis_speed == 21000))) {
               lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_20_0_20_0;
    } else {
        lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_0;
    }
    if (lanes_ctrl->mixed_port_mode == MIXED_DUAL_MODE_0) {
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}

int
soc_tomahawk_port_speed_update(int unit, soc_port_t port, int speed)
{
    uint32 rval, fval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_t it_port;
    int port_rename = 0;
    int rv;
    soc_th_port_lanes_t lanes_ctrl;

    sal_memset(&lanes_ctrl, 0, sizeof(lanes_ctrl));
    lanes_ctrl.port_base = port;
    lanes_ctrl.lanes = TH_FLEXPORT_SPEED_SET_NUM_LANES;
    lanes_ctrl.cur_lanes = TH_FLEXPORT_SPEED_SET_NUM_LANES;
    lanes_ctrl.speed = speed;
    lanes_ctrl.os_mixed_sister = si->os_mixed_sister_25_50_enable;

    SOC_IF_ERROR_RETURN(READ_CLPORT_MODE_REGr(unit, port, &rval));
    lanes_ctrl.cur_mode = soc_reg_field_get(unit, CLPORT_MODE_REGr, rval,
            XPORT0_CORE_PORT_MODEf);

    /* Validate the port config under Mixed Speed */
    if ((lanes_ctrl.os_mixed_sister) &&
        (lanes_ctrl.cur_mode == SOC_TH_PORT_MODE_DUAL)) {
        if (!SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port)) {
            SOC_IF_ERROR_RETURN(_soc_tomahawk_port_mixed_speed_check(unit, &lanes_ctrl));
        }
    }

    /*
     * Update bitmaps and port names of Management Port
     * when shift b/w XE <=> GE <=> FE
    */
    if (SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port)) {
        if (IS_XE_PORT(unit, port) && (speed < 10000) && (speed > 100)) {
            SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
            SOC_PBMP_PORT_ADD(si->ge.bitmap, port);
            port_rename = 1;
        } else if (IS_XE_PORT(unit, port) && (speed <= 100)) {
            SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
            SOC_PBMP_PORT_ADD(si->fe.bitmap, port);
            port_rename = 1;
        } else if (IS_GE_PORT(unit, port) && (speed == 10000)) {
            SOC_PBMP_PORT_REMOVE(si->ge.bitmap, port);
            SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
            port_rename = 1;
        } else if (IS_GE_PORT(unit, port) && (speed <= 100)) {
            SOC_PBMP_PORT_REMOVE(si->ge.bitmap, port);
            SOC_PBMP_PORT_ADD(si->fe.bitmap, port);
            port_rename = 1;
        } else if (IS_FE_PORT(unit, port) && (speed == 10000)) {
            SOC_PBMP_PORT_REMOVE(si->fe.bitmap, port);
            SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
            port_rename = 1;
        } else if (IS_FE_PORT(unit, port) && (speed < 10000) && (speed > 100)) {
            SOC_PBMP_PORT_REMOVE(si->fe.bitmap, port);
            SOC_PBMP_PORT_ADD(si->ge.bitmap, port);
            port_rename = 1;
        }
    }
    if (port_rename) {
        _SOC_TH_RECONFIGURE_PORT_TYPE_INFO(xe);
        _SOC_TH_RECONFIGURE_PORT_TYPE_INFO(ge);
        _SOC_TH_RECONFIGURE_PORT_TYPE_INFO(fe);
        soc_dport_map_update(unit);
    }

    /*
     * No reconfiguration is needed if new speed
     * matches current speed
     */
    if (speed == si->port_init_speed[port]) {
        return SOC_E_NONE;
    }

    /* Update HSP port multicast T2OQ setting */
    if (SOC_PBMP_MEMBER(si->eq_pbm, port) && (!SOC_IS_TOMAHAWK2(unit))) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk_mc_toq_cfg(unit, port,
                                      (speed >= 40000) ? TRUE : FALSE));
    }

    rv = soc_th_port_asf_mode_set(unit, port, speed,
                                  _SOC_TH_ASF_MODE_CFG_UPDATE);
    if ((SOC_E_NONE != rv) && (SOC_E_UNAVAIL != rv) && (SOC_E_PARAM != rv)) {
        return rv;
    }

    /* OS related updates */
    if (SOC_PBMP_NOT_NULL(SOC_INFO(unit).oversub_pbm)) {
        int factor = SOC_IS_TOMAHAWK2(unit) ? 178125 : 118750;
        sal_memset(entry, 0, sizeof(egr_xmit_start_count_entry_t));
        fval = ((speed / 10) * factor) / 100000;
        soc_mem_field32_set(unit, EDB_1DBG_Bm, &entry, FIELD_Bf, fval);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, EDB_1DBG_Bm, MEM_BLOCK_ALL,
                           SOC_INFO(unit).port_l2p_mapping[port], entry));

        rval = 0;
        if (speed >= 100000) {
            fval = 140;
        } else if (speed >= 40000) {
            fval = 60;
        } else if (speed >= 25000) {
            fval = 40;
        } else if (speed >= 20000) {
            fval = 30;
        } else {
            fval = 15;
        }
        soc_reg_field_set(unit, MMU_3DBG_Cr, &rval, FIELD_Af,
                          fval + sal_rand() % 20);
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_sc_reg32_set(unit, MMU_3DBG_Cr, -1, port, 0, rval));
    }

    if (!SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port)) {
        SOC_IF_ERROR_RETURN(soc_th_speed_set_init_ctrl(unit, &lanes_ctrl));
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_port_lanes_set(unit, &lanes_ctrl));
    }

    return SOC_E_NONE;
}

/*
 * Utility routine called when converting an HiGig2 encapped port
 * to IEEE encap. This routine returns the IEEE speed corresponding
 * to the current HG speed of the port
 */
int
soc_th_port_ie_speed_get(int unit, soc_port_t port, int *speed)
{
    soc_info_t *si;
    si = &SOC_INFO(unit);

    switch (si->port_init_speed[port]) {
        case 11000:
            *speed = 10000;
            break;
        case 21000:
            *speed = 20000;
            break;
        case 27000:
            *speed = 25000;
            break;
        case 42000:
            *speed = 40000;
            break;
        case 53000:
            *speed = 50000;
            break;
        case 106000:
            *speed = 100000;
            break;
        default:
            /* port_init_speed is already IEEE speed */
            *speed = si->port_init_speed[port];
            break;
    }

    return SOC_E_NONE;
}

/*
 * Utility routine called when converting an IEEE encapped port
 * to HiGig2 encap. This routine returns the HG speed corresponding
 * to the current ethernet speed of the port
 */
int
soc_th_port_hg_speed_get(int unit, soc_port_t port, int *speed)
{
    soc_info_t *si;
    si = &SOC_INFO(unit);

    switch (si->port_init_speed[port]) {
        case 10000:
            *speed = 11000;
            break;
        case 20000:
            *speed = 21000;
            break;
        case 25000:
            *speed = 27000;
            break;
        case 40000:
            *speed = 42000;
            break;
        case 50000:
            *speed = 53000;
            break;
        case 100000:
            *speed = 106000;
            break;
        default:
            /* port_init_speed is already HG speed */
            *speed = si->port_init_speed[port];
            break;
    }

    return SOC_E_NONE;
}

int
soc_tomahawk_xpe_base_index_check(int unit, int base_type, int xpe,
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
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: SLICE%d is not in config\n"),
                         msg, base_index));
            }
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

void soc_th_set_alpm_banks(int unit, int alpm_banks)
{
    num_shared_alpm_banks[unit] = alpm_banks;
    return;
}

int soc_th_get_alpm_banks(int unit)
{
    int rv = SOC_E_NONE;
    uint32 bank_config = 0xf;
    uint32 config = 0;

    if (num_shared_alpm_banks[unit] == 0) {
        rv = soc_reg32_get(unit, ISS_BANK_CONFIGr, REG_PORT_ANY, 0, &config);
        if (SOC_FAILURE(rv)) {
            num_shared_alpm_banks[unit] = 4;
        } else {
            bank_config = soc_reg_field_get(unit, ISS_BANK_CONFIGr, config,
                                            ALPM_ENTRY_BANK_CONFIGf);
            if (bank_config == 0x3) {
                num_shared_alpm_banks[unit] = 2;
            } else {
                num_shared_alpm_banks[unit] = 4;
            }
        }
    }

    return num_shared_alpm_banks[unit];
}

int
soc_tomahawk_alpm_mode_get(int unit)
{
    return _soc_alpm_mode[unit];
}

void
_soc_tomahawk_alpm_bkt_view_set(int unit, int index, soc_mem_t view)
{
    int bkt = (index >> (soc_th_get_alpm_banks(unit) + 1) / 2) & SOC_TH_ALPM_BKT_MASK(unit);

    if (view != INVALIDm) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Unit:%d ALPM bkt set index:%d bkt:%d view:%s\n"),
                                unit, index, bkt, SOC_MEM_NAME(unit, view)));
    }
    _soc_th_alpm_bkt_view_map[unit][bkt] = view;
}

soc_mem_t
_soc_tomahawk_alpm_bkt_view_get(int unit, int index)
{
    soc_mem_t view;
    int bkt = (index >> (soc_th_get_alpm_banks(unit) + 1) / 2) & SOC_TH_ALPM_BKT_MASK(unit);
    
    view = _soc_th_alpm_bkt_view_map[unit][bkt];
    if (view != INVALIDm) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Unit:%d ALPM bkt get index:%d bkt:%d view:%s\n"),
                                unit, index, bkt, SOC_MEM_NAME(unit, view)));
    }
    return view;
}

int soc_tomahawk_mem_basetype_get(int unit, soc_mem_t mem)
{
    int base_type;

    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;
    return base_type;
}

int soc_tomahawk_mem_is_xpe(int unit, soc_mem_t mem)
{
    int block_info_index;

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        return 0;
    }

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
soc_tomahawk_mem_xpe_pipe_check(int unit, soc_mem_t mem, int xpe, int pipe)
{
    int rv = SOC_E_NONE;
    int block_info_index, base_type;

    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
    if (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_XPE) {
        return SOC_E_PARAM;
    }

    base_type = soc_tomahawk_mem_basetype_get(unit, mem);
    rv = soc_tomahawk_xpe_base_index_check(unit, base_type, xpe, pipe, NULL);

    if (rv == SOC_E_PARAM) {
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}

STATIC int
_soc_tomahawk_xpe_reg_check(int unit, soc_reg_t reg, int xpe, int base_index)
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

    return soc_tomahawk_xpe_base_index_check(unit, base_type, xpe, base_index,
                                             SOC_REG_NAME(unit, reg));
}

STATIC int
_soc_tomahawk_xpe_mem_check(int unit, soc_mem_t mem, int xpe, int base_index)
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

    return soc_tomahawk_xpe_base_index_check(unit, base_type, xpe, base_index,
                                             SOC_MEM_NAME(unit, mem));
}

/*
 * Parameters for soc_tomahawk_xpe_reg_access and soc_tomahawk_xpe_mem_access:
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
_soc_tomahawk_xpe_reg_access(int unit, soc_reg_t reg, int xpe, int base_index,
                             int index, uint64 *data, int write)
{
    soc_info_t *si;
    soc_reg_t orig_reg;
    int port;
    int base_type, break_after_first, unique, accessed;
    int base_index_count;
    uint32 base_index_map, xpe_map=0, orig_xpe_map;
    soc_pbmp_t base_index_pbmp;
    uint32 inst;

    si = &SOC_INFO(unit);
    orig_reg = reg;
    base_type = SOC_REG_BASE_TYPE(unit, reg);
    break_after_first = TRUE;

    if (xpe >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk_xpe_reg_check(unit, reg, xpe, base_index));
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

        soc_tomahawk_pipe_map_get(unit, &base_index_map);
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
        if (xpe == -1) {
            xpe_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        } else {
            xpe_map = (1 << xpe) & (si->ipipe_xpe_map[0]|si->ipipe_xpe_map[1]);
        }
        unique = (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL);
        
        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
            if (!(xpe_map & (1 << xpe))) {
                continue;
            }

            if (unique) {
                reg = SOC_REG_UNIQUE_ACC(unit, orig_reg)[xpe];
            }
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, 0, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, 0, index, data));
            }
            /* only unique writing need access all xpe */
            if (!(write && unique)) {
                break;
            }
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
        if (base_index == -1) {
            base_index_map = base_type == 6 ? si->ipipe_sc_map[0] : 0x3;
        } else {
            base_index_map = 1 << base_index;
        }
        if (xpe == -1) {
            orig_xpe_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        } else {
            orig_xpe_map = (1 << xpe);
        }
        unique = (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL);
        accessed = 0;

        base_index_count = base_type == 6 ? NUM_SLICE(unit) : NUM_LAYER(unit);
        for (base_index = 0; base_index < base_index_count; base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            
            /* Loop through XPE(s) only on UNIQUE type base register */
            if (base_type == 6) {
                /* epipe 0 on sc 0, epipe 2 on sc 1 */
                xpe_map = si->epipe_xpe_map[base_index * 2];
            } else {
                /* ipipe 0 on layer 0, ipipe 1 on layer 1 */
                xpe_map = si->ipipe_xpe_map[base_index];
            }
            xpe_map &= orig_xpe_map;

            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                if (!(xpe_map & (1 << xpe))) {
                    continue;
                }
                
                if (unique) {
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
                accessed = 1;
                /* only unique writing need access all xpe */
                if (!(write && unique)) {
                    break;
                }
            }
            /* reading operation breaks if accessed */
            if ((!write) && (accessed)) {
                break;
            }
        }
        break;
    /* No default case as base_type can have value only between 0-7 */
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_xpe_mem_access(int unit, soc_mem_t mem, int xpe, int base_index,
                             int copyno, int offset_in_section,
                             void *entry_data, int write)
{
    soc_info_t *si;
    soc_mem_t mem_list[8] = {INVALIDm};
    int mem_index, mem_cnt;
    int index;
    int base_type;
    uint32 base_index_map, xpe_map=0, orig_xpe_map;

    si = &SOC_INFO(unit);
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;
    mem_index = 0;
    
    if (xpe >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk_xpe_mem_check(unit, mem, xpe, base_index));
    }

    switch (base_type) {
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        /* no _PIPEx */
        if (SOC_MEM_UNIQUE_ACC(unit, mem) == NULL) {
            mem_list[mem_index++] = mem;
            break;
        }
        
        soc_tomahawk_pipe_map_get(unit, &base_index_map);
        if (base_index != -1) {
            base_index_map &= (1 << base_index);
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        orig_xpe_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        if (xpe != -1) {
            orig_xpe_map &= (1 << xpe);
            if (orig_xpe_map == 0) {
                return SOC_E_PARAM;
            }
        }
        
        for (base_index = 0; base_index < NUM_PIPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }

            /* no need to check each pipe, 0 is enough */
            if (SOC_MEM_ACC_TYPE(unit, SOC_MEM_UNIQUE_ACC_PIPE(unit, mem, 0)) 
                == _TH_SUBS_ACC_TYPE_DUPLICATE) {
                /* _PIPEx */
                mem_list[mem_index++] = 
                    SOC_MEM_UNIQUE_ACC_PIPE(unit, mem, base_index);
            } else {
                /* _XPEy_PIPEx */
                xpe_map = base_type == 2 ?
                    si->ipipe_xpe_map[base_index] : 
                    si->epipe_xpe_map[base_index];
                xpe_map &= orig_xpe_map;
                for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                    if (!(xpe_map & (1 << xpe))) {
                        continue;
                    }
                    mem_list[mem_index++] = 
                        SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, mem, xpe, base_index);
                }
            }
        }
        break;
    case 4: /* CHIP */
        /* no _XPEx */
        if (SOC_MEM_UNIQUE_ACC(unit, mem) == NULL) {
            mem_list[mem_index++] = mem;
            break;
        }
        
        xpe_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        if (xpe != -1) {
            xpe_map &= (1 << xpe);
            if (xpe_map == 0) {
                return SOC_E_PARAM;
            }
        }

        /* _XPEx */
        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
            if (!(xpe_map & (1 << xpe))) {
                continue;
            }
            mem_list[mem_index++] = 
                SOC_MEM_UNIQUE_ACC_XPE(unit, mem, xpe);
        }
        break;
    case 5: /* XPE */
        /* no _XPEx */
        if (SOC_MEM_UNIQUE_ACC(unit, mem) == NULL) {
            mem_list[mem_index++] = mem;
            break;
        }
        
        xpe_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        if (xpe != -1) {
            xpe_map &= (1 << xpe);
            if (xpe_map == 0) {
                return SOC_E_PARAM;
            }
        }

        /* _XPEx */
        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
            if (!(xpe_map & (1 << xpe))) {
                continue;
            }
            mem_list[mem_index++] = 
                SOC_MEM_UNIQUE_ACC_XPE(unit, mem, xpe);
        }
        break;
    case 6: /* SLICE */
        /* no _SCx */
        if (SOC_MEM_UNIQUE_ACC(unit, mem) == NULL) {
            mem_list[mem_index++] = mem;
            break;
        }
        
        base_index_map = si->ipipe_sc_map[0];
        if (base_index != -1) {
            base_index_map &= (1 << base_index);
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        for (base_index = 0; base_index < NUM_SLICE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }

            /* no need to check each sc, 0 is enough */
            if (SOC_MEM_ACC_TYPE(unit, SOC_MEM_UNIQUE_ACC_SC(unit, mem, 0))
                == _TH_SUBS_ACC_TYPE_DUPLICATE) {
                /* _PIPEx */
                mem_list[mem_index++] = 
                    SOC_MEM_UNIQUE_ACC_SC(unit, mem, base_index);
            } else {
                /* No unique access type for such base_type till now */
            }
        }
        break;
    case 0: /* IPORT */
    case 1: /* EPORT */
    case 7: /* LAYER */
    default:
        return SOC_E_INTERNAL;
    }

    mem_cnt = write ? mem_index : 1;
    index = offset_in_section;
    for (mem_index = 0; mem_index < mem_cnt; mem_index ++) {
        if (write) {
            SOC_IF_ERROR_RETURN(soc_mem_write
                (unit, mem_list[mem_index], copyno, index, entry_data));
        } else {
            SOC_IF_ERROR_RETURN(soc_mem_read
                (unit, mem_list[mem_index], copyno, index, entry_data));
        }
    }

    return SOC_E_NONE;
}

int
soc_tomahawk_sc_base_index_check(int unit, int base_type, int sc,
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
_soc_tomahawk_sc_reg_check(int unit, soc_reg_t reg, int sc, int base_index)
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

    return soc_tomahawk_sc_base_index_check(unit, base_type, sc, base_index,
                                            SOC_REG_NAME(unit, reg));
}

#if 0
STATIC int
_soc_tomahawk_sc_mem_check(int unit, soc_mem_t mem, int sc, int base_index)
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

    return soc_tomahawk_sc_base_index_check(unit, base_type, sc, base_index,
                                            SOC_MEM_NAME(unit, mem));
}
#endif

STATIC int
_soc_tomahawk_sc_reg_access(int unit, soc_reg_t reg, int sc, int base_index,
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
            (_soc_tomahawk_sc_reg_check(unit, reg, sc, base_index));
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
        soc_tomahawk_pipe_map_get(unit, &base_index_map);
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

        for (base_index = 0; base_index < NUM_XPE(unit); base_index++) {
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
        /* No unique access type for such base_type */
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
_soc_tomahawk_sc_mem_access(int unit, soc_mem_t mem, int sc, int base_index,
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
            (_soc_tomahawk_sc_mem_check(unit, mem, sc, base_index));
    }

    switch (base_type) {
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (sc >= 0 && SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) {
            mem = SOC_MEM_UNIQUE_ACC(unit, mem)[sc];
        }

        /* All tables with such base_type should have multiple sections */
        soc_tomahawk_pipe_map_get(unit, &base_index_map);
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

/*About layer concept, you can refer to 1.3.3 chapter in MMUuA_overview.pdf*/
int
soc_tomahawk_mmu_layer_get(int unit, int pipe, int *layer)
{
    if( layer == NULL ) {
        return SOC_E_PARAM;
    }
    if ( pipe >= 0 && pipe < NUM_PIPE(unit)) {
        if ( pipe == 0 || pipe == 3 ) { /* pipe 0 and 3 are in layer 0 */
            *layer = 0;
        } else { /* pipe 1 and 2 are in layer 1 */
            *layer = 1;
        }
        return SOC_E_NONE;
    }
    return SOC_E_PARAM;
}

int
soc_tomahawk_xpe_reg32_set(int unit, soc_reg_t reg, int xpe, int base_index,
                           int index, uint32 data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk_xpe_reg_access(unit, reg, xpe, base_index,
                                                     index, &data64, TRUE));

    return SOC_E_NONE;
}

int
soc_tomahawk_xpe_reg32_get(int unit, soc_reg_t reg, int xpe, int base_index,
                           int index, uint32 *data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, *data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk_xpe_reg_access(unit, reg, xpe, base_index,
                                                     index, &data64, FALSE));
    *data = COMPILER_64_LO(data64);

    return SOC_E_NONE;
}

int
soc_tomahawk_xpe_reg_set(int unit, soc_reg_t reg, int xpe, int base_index,
                         int index, uint64 data)
{
    return _soc_tomahawk_xpe_reg_access(unit, reg, xpe, base_index,
                                        index, &data, TRUE);
}

int
soc_tomahawk_xpe_reg_get(int unit, soc_reg_t reg, int xpe, int base_index,
                         int index, uint64 *data)
{
    return _soc_tomahawk_xpe_reg_access(unit, reg, xpe, base_index,
                                        index, data, FALSE);
}

int
soc_tomahawk_xpe_mem_write(int unit, soc_mem_t mem, int xpe, int base_index,
                           int copyno, int offset_in_section, void *entry_data)
{
    return _soc_tomahawk_xpe_mem_access(unit, mem, xpe, base_index, copyno,
                                        offset_in_section, entry_data, TRUE);
}

int
soc_tomahawk_xpe_mem_read(int unit, soc_mem_t mem, int xpe, int base_index,
                          int copyno, int offset_in_section, void *entry_data)
{
    return _soc_tomahawk_xpe_mem_access(unit, mem, xpe, base_index, copyno,
                                        offset_in_section, entry_data, FALSE);
}

int
soc_tomahawk_sc_reg32_set(int unit, soc_reg_t reg, int sc, int base_index,
                          int index, uint32 data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk_sc_reg_access(unit, reg, sc, base_index,
                                                    index, &data64, TRUE));

    return SOC_E_NONE;
}

int
soc_tomahawk_sc_reg32_get(int unit, soc_reg_t reg, int sc, int base_index,
                          int index, uint32 *data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, *data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk_sc_reg_access(unit, reg, sc, base_index,
                                                    index, &data64, FALSE));
    *data = COMPILER_64_LO(data64);

    return SOC_E_NONE;
}

int
soc_tomahawk_sc_reg_set(int unit, soc_reg_t reg, int sc, int base_index,
                        int index, uint64 data)
{
    return _soc_tomahawk_sc_reg_access(unit, reg, sc, base_index,
                                       index, &data, TRUE);
}

int
soc_tomahawk_sc_reg_get(int unit, soc_reg_t reg, int sc, int base_index,
                        int index, uint64 *data)
{
    return _soc_tomahawk_sc_reg_access(unit, reg, sc, base_index,
                                       index, data, FALSE);
}

int
soc_tomahawk_port_traffic_egr_enable_set(int unit, soc_port_t port,
                        int enable)
{
    uint64 rval64;
    int mmu_port, phy_port, i;
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t reg[3] = {
        THDU_OUTPUT_PORT_RX_ENABLE_64r,
        MMU_THDM_DB_PORT_RX_ENABLE_64r,
        MMU_THDM_MCQE_PORT_RX_ENABLE_64r
    };
    int pipe;
    uint32 mmu_inst_map;
    int xpe;

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];
    mmu_port %= SOC_TH_MMU_PORT_STRIDE;
    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, port, &pipe));
    mmu_inst_map = si->epipe_xpe_map[pipe];
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        if (!(mmu_inst_map & (1 << xpe))) {
            continue;
        }

        for (i = 0; i < 3; i++) {
            COMPILER_64_ZERO(rval64);
            SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg_get(unit, reg[i],
                xpe, pipe, 0, &rval64));
            if (enable) {
                COMPILER_64_BITSET(rval64, mmu_port);
            } else {
                COMPILER_64_BITCLR(rval64, mmu_port);
            }
            SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg_set(unit, reg[i],
                xpe, pipe, 0, rval64));
        }
    }

    return SOC_E_NONE;
}

#if 0
int
soc_tomahawk_sc_mem_write(int unit, soc_mem_t mem, int sc, int base_index,
                          int copyno, int offset_in_section, void *entry_data)
{
    return _soc_tomahawk_sc_mem_access(unit, mem, sc, base_index, copyno,
                                       offset_in_section, entry_data, TRUE);
}

int
soc_tomahawk_sc_mem_read(int unit, soc_mem_t mem, int sc, int base_index,
                         int copyno, int offset_in_section, void *entry_data)
{
    return _soc_tomahawk_sc_mem_access(unit, mem, sc, base_index, copyno,
                                       offset_in_section, entry_data, FALSE);
}
#endif

int
soc_tomahawk_clear_mmu_memory(int unit)
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
    static struct {
        soc_mem_t mem; /* base view */
        int use_base_type_views; /* 1 means use xpe/sc/etc views */
        int index; /* -1 means clear entire mem */
    } mem_list[] = {
        { MMU_CBPDATA0m, 1, 0 },
        { MMU_CBPDATA15m, 1, 0 },
        { MMU_CBPDATA30m, 1, 0 },
        { MMU_CBPDATA45m, 1, 0 },
        { INVALIDm, 0, -1 }
    };

#ifdef  BCM_TOMAHAWKPLUS_SUPPORT
    /* adjust CBPDATA for resetting on TH+ */
    if (SOC_IS_TOMAHAWKPLUS(unit)) {
        mem_list[0].mem = MMU_CBPDATA0m;
        mem_list[1].mem = MMU_CBPDATA19m;
        mem_list[2].mem = MMU_CBPDATA38m;
        mem_list[3].mem = MMU_CBPDATA57m;
    }
#endif

    for (i = 0; mem_list[i].mem != INVALIDm; i++) {
        mem = mem_list[i].mem;
        index = mem_list[i].index;
        use_base_type_views = mem_list[i].use_base_type_views;
        if (use_base_type_views) {
            base_type = soc_tomahawk_mem_basetype_get(unit, mem);
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

/* Function to Clear all Memories.
 * Param
 *   mmu_init - bool val to indicate whether to initialize mmu or not.
 */
STATIC int
_soc_tomahawk_clear_all_memory(int unit, int mmu_init)
{
    uint32 pipe_map;
    soc_reg_t reg;
    int pipe, count;
    uint32 rval, in_progress;
    int pipe_init_usec;
    soc_timeout_t to;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    /* Initial IPIPE memory */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    /* Set count to # entries of largest IPIPE table */
    count = soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
    if (count < soc_mem_index_count(unit, L2Xm)) {
        count = soc_mem_index_count(unit, L2Xm);
    }
    if (count < soc_mem_index_count(unit, L3_ENTRY_ONLYm)) {
        count = soc_mem_index_count(unit, L3_ENTRY_ONLYm);
    }
    if (count < soc_mem_index_count(unit, FPEM_ECCm)) {
        count = soc_mem_index_count(unit, FPEM_ECCm);
    }
    if (count < soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m)) {
        count = soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m);
    }
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, count);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    /* Initial L2_MGT memories */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_L2_MGMT_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, L2_MGMT_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, L2_MGMT_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    count = soc_mem_index_count(unit, L2_MOD_FIFOm);
    if (count < soc_mem_index_count(unit, L2_MGMT_SER_FIFOm)) {
        count = soc_mem_index_count(unit, L2_MGMT_SER_FIFOm);
    }
    soc_reg_field_set(unit, L2_MGMT_HW_RESET_CONTROL_1r, &rval, COUNTf, count);
    SOC_IF_ERROR_RETURN(WRITE_L2_MGMT_HW_RESET_CONTROL_1r(unit, rval));

    /* Initial EPIPE memory */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    /* Set count to # entries in largest EPIPE table (EGR_L3_NEXT_HOP) */
    count = SOC_MEM_INFO(unit, EGR_L3_NEXT_HOPm).index_max + 1;
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
        sal_usleep(1000);
        for (pipe = 0; pipe < _TH_PIPES_PER_DEV && in_progress; pipe++) {
            if (in_progress & (1 << pipe)) { /* not done yet */
                reg = SOC_REG_UNIQUE_ACC(unit, ING_HW_RESET_CONTROL_2r)[pipe];
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
    } while (in_progress != 0);

    /* Wait for EPIPE memory initialization done */
    in_progress = pipe_map;
    do {
        for (pipe = 0; pipe < _TH_PIPES_PER_DEV && in_progress; pipe++) {
            if (in_progress & (1 << pipe)) { /* not done yet */
                reg = SOC_REG_UNIQUE_ACC(unit, EGR_HW_RESET_CONTROL_1r)[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                    if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                        in_progress ^= 1 << pipe;
                    }
            }
        }
        if (in_progress != 0) {
            sal_usleep(1000);
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : EGR_HW_RESET timeout\n"), unit));
            break;
        }
    } while (in_progress != 0);

    /* Make sure L2_MGMT memories are all clean */
    reg = L2_MGMT_HW_RESET_CONTROL_1r;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    if (!soc_reg_field_get(unit, reg, rval, DONEf)) {
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
            "unit %d : L2_MGMT_HW_RESET not done (reg val: 0x%x) !! \n"),
            unit, rval));
    } else {
        SOC_IF_ERROR_RETURN(WRITE_L2_MGMT_HW_RESET_CONTROL_1r(unit, 0));
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    rval = SOC_REG_INFO(unit, EGR_HW_RESET_CONTROL_1r).rst_val_lo;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* Initialize IDB memory */
    SOC_IF_ERROR_RETURN(WRITE_IDB_HW_CONTROLr(unit, 0));
    rval = 0;
    soc_reg_field_set(unit, IDB_HW_CONTROLr, &rval, IS_MEM_INITf, 1);
    SOC_IF_ERROR_RETURN(WRITE_IDB_HW_CONTROLr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_IDB_HW_CONTROLr(unit, 0));

    if (mmu_init) {
        /* Initialize MMU memory */
        SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, 0));
        rval = 0;
        soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, INIT_MEMf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, 0));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_port_mapping_init(int unit)
{
    soc_info_t *si;
    int port, phy_port, idb_port;
    int num_port, num_phy_port;
    soc_reg_t reg;
    soc_mem_t mem;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe;
    int i;
    uint16 dev_id = 0;
    uint8 rev_id = 0;

    /*
     * 96 entries MMU_CHFC_SYSPORT_MAPPINGm.SYS_PORT
     * 256 entries SYS_PORTMAPm.DEVICE_PORT_NUMBER
     */

    si = &SOC_INFO(unit);
    soc_cm_get_id(unit, &dev_id, &rev_id);

    num_port = soc_mem_index_count(unit, PORT_TABm) - 1;
    num_phy_port = 136;

    /* Ingress physical to device port mapping */
    sal_memset(&entry, 0,
               sizeof(ing_idb_to_device_port_number_mapping_table_entry_t));
    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port != -1) {
            pipe = si->port_pipe[port];
            idb_port = si->port_l2i_mapping[port];
        } else if (phy_port >= 132) { /* loopback port */
            pipe = phy_port - 132;
            idb_port = 33;
        } else if (phy_port == 129) { /* management port 0 (if not in use) */
            pipe = 1;
            idb_port = 32;
        } else if (phy_port == 130) { /* undefined physical port */
            pipe = 3;
            idb_port = 32;
        } else if (phy_port == 131) { /* management port 1 (if not in use) */
            pipe = 2;
            idb_port = 32;
        } else {
            pipe = (phy_port - 1) / _TH_PORTS_PER_PIPE;
            idb_port = (phy_port -1 ) % _TH_PORTS_PER_PIPE;
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

    /* MMU port to device port mapping */
    rval = 0;
    reg = MMU_PORT_TO_DEVICE_PORT_MAPPINGr;
    PBMP_ALL_ITER(unit, port) {
        soc_reg_field_set(unit, reg, &rval, DEVICE_PORTf, port);
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
    
    if ((BCM56963_DEVICE_ID == dev_id) || (BCM56967_DEVICE_ID == dev_id)) {
        const int phy_pbmp_lb_disable[] = {
            13,14,15,16,17,18,19,20,25,26,27,28,29,30,31,32,45,46,47,48,
            57,58,59,60,61,62,63,64,73,74,75,76,77,78,79,80,89,90,91,92,
            93,94,95,96,113,114,115,116,121,122,123,124,125,126,127,128};
        for (i = 0; i < COUNTOF(phy_pbmp_lb_disable); i++) {
            phy_port = phy_pbmp_lb_disable[i];
            port = si->port_p2l_mapping[phy_port];
            if (port != -1) {
                SOC_PBMP_PORT_ADD(loopback_disable[unit],port);
            }
        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_idb_calendar_set(int unit,  int calendar_id, int in_pipe)
{
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    uint32 pipe_map;
    soc_mem_t mem;
    soc_reg_t reg;
    soc_field_t field;
    int pipe, slot, id, length;
    int port, phy_port, idb_port;
    uint32 rval = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    static const soc_mem_t calendar_mems[] = {
        IS_TDM_CALENDAR0m, IS_TDM_CALENDAR1m
    };
    static const soc_field_t calendar_end_fields[] = {
        CAL0_ENDf, CAL1_ENDf
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe)) ||
            ((UPDATE_ALL_PIPES != in_pipe) && (pipe != in_pipe))) {
            continue;
        }

        for (length = _TDM_LENGTH; length > 0; length--) {
            if (tdm->idb_tdm[pipe][length - 1] != TH_NUM_EXT_PORTS) {
                break;
            }
        }

        reg = SOC_REG_UNIQUE_ACC(unit, IS_TDM_CONFIGr)[pipe];
        if (calendar_id == -1) { /* choose "the other one" */
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            calendar_id = soc_reg_field_get(unit, reg, rval, CURR_CALf) ^ 1;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, calendar_mems[calendar_id])[pipe];
        field = calendar_end_fields[calendar_id];
        sal_memset(entry, 0, soc_mem_entry_words(unit, mem) * sizeof(uint32));
        for (slot = 0; slot < length; slot += 2) {
            phy_port = tdm->idb_tdm[pipe][slot];
            id = -1;
            if (phy_port == TH_OVSB_TOKEN) {
                idb_port = _SOC_TH_TDM_OVERSUB_TOKEN;
            } else if (phy_port == TH_IDL1_TOKEN) {
                idb_port = _SOC_TH_TDM_IDL1_TOKEN;
            } else if (phy_port == TH_IDL2_TOKEN) {
                idb_port = _SOC_TH_TDM_IDL2_TOKEN;
            } else if (phy_port == NULL_TOKEN) {
                idb_port = _SOC_TH_TDM_NULL_TOKEN;
            } else if (phy_port >= TH_NUM_EXT_PORTS) {
                idb_port = _SOC_TH_TDM_UNUSED_TOKEN;
            } else {
                /* TDM code  always allocates slots for management ports.
                 * If mgmgt ports are omitted from config, this mapping
                 * is not set up.
                 */
                if ((phy_port == 129) || (phy_port == 131)){
                    idb_port = 32;
                    id = 0xf;
                }
                else {
                    port = si->port_p2l_mapping[phy_port];
                    idb_port = si->port_l2i_mapping[port];
                    id = si->port_serdes[port];
                }
            }
            soc_mem_field32_set(unit, mem, entry, PORT_NUM_EVENf, idb_port);
            soc_mem_field32_set(unit, mem, entry, PHY_PORT_ID_EVENf, id & 0xf);
            phy_port = tdm->idb_tdm[pipe][slot + 1];
            id = -1;
            if (phy_port == TH_OVSB_TOKEN) {
                idb_port = _SOC_TH_TDM_OVERSUB_TOKEN;
            } else if (phy_port == TH_IDL1_TOKEN) {
                idb_port = _SOC_TH_TDM_IDL1_TOKEN;
            } else if (phy_port == TH_IDL2_TOKEN) {
                idb_port = _SOC_TH_TDM_IDL2_TOKEN;
            } else if (phy_port == NULL_TOKEN) {
                idb_port = _SOC_TH_TDM_NULL_TOKEN;
            } else if (phy_port >= TH_NUM_EXT_PORTS) {
                idb_port = _SOC_TH_TDM_UNUSED_TOKEN;
            } else {
                /* TDM code  always allocates slots for management ports.
                 * If mgmgt ports are omitted from config, this mapping
                 * is not set up.
                 */
                if ((phy_port == 129) || (phy_port == 131)){
                    idb_port = 32;
                    id = 0xf;
                }
                else {
                    port = si->port_p2l_mapping[phy_port];
                    idb_port = si->port_l2i_mapping[port];
                    id = si->port_serdes[port];
                }
            }
            soc_mem_field32_set(unit, mem, entry, PORT_NUM_ODDf, idb_port);
            soc_mem_field32_set(unit, mem, entry, PHY_PORT_ID_ODDf, id & 0xf);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, slot / 2, entry));
            if (tdm->idb_tdm[pipe][slot + 2] == TH_NUM_EXT_PORTS) {
                soc_reg_field_set(unit, reg, &rval, field,
                                  phy_port == TH_NUM_EXT_PORTS ? slot : slot + 1);
                break;
            }
        }
        soc_reg_field_set(unit, reg, &rval, CURR_CALf, calendar_id);
        soc_reg_field_set(unit, reg, &rval, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_idb_oversub_group_set(int unit)
{
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    uint32 pipe_map;
    int block_info_idx;
    soc_reg_t reg, reg1;
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
    static int pblk_slots[SOC_TH_PORT_RATIO_COUNT][7] = {
        { 0, -1,  0,  0, -1,  0, -1 }, /* SOC_TH_PORT_RATIO_SINGLE */
        { 0, -1,  2,  0, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_1 */
        { 0,  0,  2,  0,  0,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_2_1 */
        { 0,  2,  2,  0,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_2 */
        { 0, -1,  2,  0, -1,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_2_1_1 */
        { 0,  0,  2,  0,  0,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_4_1_1 */
        { 0, -1,  2,  1, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_2 */
        { 0,  2,  2,  1,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_4 */
        { 0, -1,  2,  1, -1,  3, -1 }  /* SOC_TH_PORT_RATIO_QUAD */
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        reg1 = SOC_REG_UNIQUE_ACC(unit, IS_OVR_SUB_GRP_CFGr)[pipe];
        for (group = 0; group < _OVS_GROUP_COUNT; group++) {
            reg = SOC_REG_UNIQUE_ACC(unit, grp_tbl_regs[group])[pipe];
            rval = 0;
            for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                phy_port = tdm->idb_ovs_tdm[pipe][group][slot];
                if (phy_port >= TH_NUM_EXT_PORTS) {
                    idb_port = 0x3f;
                    id = 0;
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
            /* Handle the case where the first element in the group
             * is invalid but other valid ports exist in the group.
             */
            for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                phy_port = tdm->idb_ovs_tdm[pipe][group][slot];
                if (phy_port < TH_NUM_EXT_PORTS) {
                    break;
                }
            }
            /* No valid ports found, skip group */
            if (_OVS_GROUP_TDM_LENGTH == slot) {
                continue;
            }
            if (si->os_mixed_sister_25_50_enable == 0) {
                port = si->port_p2l_mapping[phy_port];
                speed_max = 25000 * si->port_num_lanes[port];
                if (speed_max > si->port_speed_max[port]) {
                    speed_max = si->port_speed_max[port];
                }
            } else {
                speed_max = tdm->speed[phy_port];
            }
            _soc_tomahawk_speed_to_ovs_class_mapping(unit, speed_max,
                                                     &ovs_class);
            rval = 0;
            soc_reg_field_set(unit, reg1, &rval, SAME_SPACINGf, 4);
            soc_reg_field_set(unit, reg1, &rval, SISTER_SPACINGf, 4);
            soc_reg_field_set(unit, reg1, &rval, SPEEDf, ovs_class);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg1, REG_PORT_ANY, group, rval));
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

int
_soc_tomahawk_tdm_idb_opportunistic_set(int unit, int enable)
{
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe;
    uint32 rval;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
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
        soc_reg_field_set(unit, reg, &rval, DISABLE_PORT_NUMf, 35);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

int
_soc_tomahawk_tdm_idb_hsp_set(int unit, int all_pipes)
{
    soc_info_t *si;
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe;
    int port, idb_port;
    uint32 port_map[_TH_PIPES_PER_DEV];
    uint32 rval = 0x0;

    si = &SOC_INFO(unit);

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        port_map[pipe] = 0;
    }
    SOC_PBMP_ITER(si->eq_pbm, port) {
        pipe = si->port_pipe[port];
        idb_port = si->port_l2i_mapping[port];
        port_map[pipe] |= 1 << idb_port;
    }

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        if ((UPDATE_ALL_PIPES == all_pipes) || (pipe == all_pipes)) {
            reg = SOC_REG_UNIQUE_ACC(unit, IS_TDM_HSPr)[pipe];
            soc_reg_field_set(unit, reg, &rval, PORT_BMPf, port_map[pipe]);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_mmu_calendar_set(int unit, int calendar_id, int in_pipe)
{
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    uint32 pipe_map;
    soc_mem_t mem;
    soc_reg_t reg;
    soc_field_t field;
    int pipe, slot, id, length;
    int port, phy_port, mmu_port, inst;
    uint32 rval = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    static const soc_mem_t calendar_mems[] = {
        TDM_CALENDAR0m, TDM_CALENDAR1m
    };
    static const soc_field_t calendar_end_fields[] = {
        CAL0_ENDf, CAL1_ENDf
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe)) ||
            ((UPDATE_ALL_PIPES != in_pipe) && (pipe != in_pipe))) {
            continue;
        }

        for (length = _TDM_LENGTH; length > 0; length--) {
            if (tdm->mmu_tdm[pipe][length - 1] != TH_NUM_EXT_PORTS) {
                break;
            }
        }

        reg = TDM_CONFIGr;
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
        if (calendar_id == -1) { /* choose "the other one" */
            calendar_id = soc_reg_field_get(unit, reg, rval, CURR_CALf) ^ 1;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, calendar_mems[calendar_id])[pipe];
        field = calendar_end_fields[calendar_id];
        sal_memset(entry, 0, soc_mem_entry_words(unit, mem) * sizeof(uint32));
        for (slot = 0; slot < length; slot += 2) {
            phy_port = tdm->mmu_tdm[pipe][slot];
            id = -1;
            if (phy_port == TH_OVSB_TOKEN) {
                mmu_port = _SOC_TH_TDM_OVERSUB_TOKEN;
            } else if (phy_port == TH_IDL1_TOKEN) {
                mmu_port = _SOC_TH_TDM_IDL1_TOKEN;
            } else if (phy_port == TH_IDL2_TOKEN) {
                mmu_port = _SOC_TH_TDM_IDL2_TOKEN;
            } else if (phy_port == NULL_TOKEN) {
                mmu_port = _SOC_TH_TDM_NULL_TOKEN;
            } else if (phy_port >= TH_NUM_EXT_PORTS) {
                mmu_port = _SOC_TH_TDM_UNUSED_TOKEN;
            } else {
                /* TDM code  always allocates slots for management ports.
                 * If mgmgt ports are omitted from config, this mapping
                 * is not set up.
                 */
                id = 0xf;
                if (phy_port == 129) {
                    mmu_port = 96;
                } else if (phy_port == 131){
                    mmu_port = 160;
                } else {
                    mmu_port = si->port_p2m_mapping[phy_port];
                    port = si->port_p2l_mapping[phy_port];
                    id = si->port_serdes[port];
                }
            }
            soc_mem_field32_set(unit, mem, entry, PORT_NUM_EVENf,
                                mmu_port & 0x3f);
            soc_mem_field32_set(unit, mem, entry, PHY_PORT_ID_EVENf, id & 0xf);
            phy_port = tdm->mmu_tdm[pipe][slot + 1];
            id = -1;
            if (phy_port == TH_OVSB_TOKEN) {
                mmu_port = _SOC_TH_TDM_OVERSUB_TOKEN;
            } else if (phy_port == TH_IDL1_TOKEN) {
                mmu_port = _SOC_TH_TDM_IDL1_TOKEN;
            } else if (phy_port == TH_IDL2_TOKEN) {
                mmu_port = _SOC_TH_TDM_IDL2_TOKEN;
            } else if (phy_port == NULL_TOKEN) {
                mmu_port = _SOC_TH_TDM_NULL_TOKEN;
            } else if (phy_port >= TH_NUM_EXT_PORTS) {
                mmu_port = _SOC_TH_TDM_UNUSED_TOKEN;
            } else {
                /* TDM code  always allocates slots for management ports.
                 * If mgmgt ports are omitted from config, this mapping
                 * is not set up.
                 */
                id = 0xf;
                if (phy_port == 129) {
                    mmu_port = 96;
                } else if (phy_port == 131){
                    mmu_port = 160;
                } else {
                    mmu_port = si->port_p2m_mapping[phy_port];
                    port = si->port_p2l_mapping[phy_port];
                    id = si->port_serdes[port];
                }
            }
            soc_mem_field32_set(unit, mem, entry, PORT_NUM_ODDf,
                                mmu_port & 0x3f);
            soc_mem_field32_set(unit, mem, entry, PHY_PORT_ID_ODDf, id & 0xf);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, slot / 2, entry));
            if (tdm->mmu_tdm[pipe][slot + 2] == TH_NUM_EXT_PORTS) {
                soc_reg_field_set(unit, reg, &rval, field,
                                  phy_port == TH_NUM_EXT_PORTS ? slot : slot + 1);
                break;
            }
        }
        soc_reg_field_set(unit, reg, &rval, CURR_CALf, calendar_id);
        soc_reg_field_set(unit, reg, &rval, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_mmu_oversub_group_set(int unit)
{
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    uint32 pipe_map;
    int block_info_idx;
    soc_reg_t reg, reg1;
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
    static const int pblk_slots[SOC_TH_PORT_RATIO_COUNT][7] = {
        { 0, -1,  0,  0, -1,  0, -1 }, /* SOC_TH_PORT_RATIO_SINGLE */
        { 0, -1,  2,  0, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_1 */
        { 0,  0,  2,  0,  0,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_2_1 */
        { 0,  2,  2,  0,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_2 */
        { 0, -1,  2,  0, -1,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_2_1_1 */
        { 0,  0,  2,  0,  0,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_4_1_1 */
        { 0, -1,  2,  1, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_2 */
        { 0,  2,  2,  1,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_4 */
        { 0, -1,  2,  1, -1,  3, -1 }  /* SOC_TH_PORT_RATIO_QUAD */
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    reg1 = OVR_SUB_GRP_CFGr;
    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        for (group = 0; group < _OVS_GROUP_COUNT; group++) {
            reg = grp_tbl_regs[group];
            rval = 0;
            for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                phy_port = tdm->mmu_ovs_tdm[pipe][group][slot];
                if (phy_port >= TH_NUM_EXT_PORTS) {
                    mmu_port = 0x3f;
                    id = 0;
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
            /* Handle the case where the first element in the group
             * is invalid but other valid ports exist in the group.
             */
            for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                phy_port = tdm->mmu_ovs_tdm[pipe][group][slot];
                if (phy_port < TH_NUM_EXT_PORTS) {
                    break;
                }
            }
            /* No valid ports found, skip group */
            if (_OVS_GROUP_TDM_LENGTH == slot) {
                continue;
            }
            if (si->os_mixed_sister_25_50_enable == 0) {
                port = si->port_p2l_mapping[phy_port];
                speed_max = 25000 * si->port_num_lanes[port];
                if (speed_max > si->port_speed_max[port]) {
                    speed_max = si->port_speed_max[port];
                }
            } else {
                speed_max = tdm->speed[phy_port];
            }
            _soc_tomahawk_speed_to_ovs_class_mapping(unit, speed_max,
                                                     &ovs_class);
            rval = 0;
            soc_reg_field_set(unit, reg1, &rval, SAME_SPACINGf,
                              speed_max >= 40000 ? 4 : 8);
            soc_reg_field_set(unit, reg1, &rval, SISTER_SPACINGf, 4);
            soc_reg_field_set(unit, reg1, &rval, SPEEDf, ovs_class);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg1, inst, group, rval));
        }
    }

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        phy_port_base = PORT_BLOCK_BASE_PORT(port);
        if (si->os_mixed_sister_25_50_enable == 0) {
            speed_max = 25000 * si->port_num_lanes[port];
            if (speed_max > si->port_speed_max[port]) {
                speed_max = si->port_speed_max[port];
            }
        } else {
                phy_port = si->port_l2p_mapping[port];
                speed_max = tdm->speed[phy_port];
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

int
_soc_tomahawk_tdm_mmu_opportunistic_set(int unit, int enable)
{
    uint32 pipe_map;
    soc_reg_t reg;
    int pipe;
    int inst;
    uint32 rval;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
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
        soc_reg_field_set(unit, reg, &rval, DISABLE_PORT_NUMf, 35);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;
}

int
_soc_tomahawk_tdm_mmu_hsp_set(int unit, int all_pipes)
{
    soc_info_t *si;
    uint32 pipe_map;
    soc_reg_t reg;
    int pipe;
    int port, phy_port, mmu_port, inst;
    uint32 port_map[_TH_PIPES_PER_DEV];
    uint32 rval = 0x0;

    si = &SOC_INFO(unit);

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        port_map[pipe] = 0;
    }
    SOC_PBMP_ITER(si->eq_pbm, port) {
        pipe = si->port_pipe[port];
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        port_map[pipe] |= 1 << (mmu_port & 0x1f);
    }

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        if ((UPDATE_ALL_PIPES == all_pipes) || (pipe == all_pipes)) {
            inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
            reg = TDM_HSPr;
            soc_reg_field_set(unit, reg, &rval, PORT_BMPf, port_map[pipe]);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_calculation(int unit, int initTime, _soc_tomahawk_tdm_t *tdm)
{
    soc_info_t *si;
    int port, phy_port;
    int pipe, clport, lane, group, block_info_idx, index, cal_id, cal_grp;
    int length, ovs_core_slot_count, ovs_io_slot_count, port_slot_count;
    int mode[_TH_PIPES_PER_DEV];
    int *tdm_idb_pipe_main, *tdm_mmu_pipe_main;
    int **tdm_idb_pipe_group, **tdm_mmu_pipe_group;
    tdm_soc_t _chip_pkg;
    tdm_mod_t *_tdm_pkg;
    soc_pbmp_t pbmp;
    soc_th_port_lanes_t lanes_ctrl;

    si = &SOC_INFO(unit);

    sal_memset(tdm, 0, sizeof(_soc_tomahawk_tdm_t));
    sal_memset(&_chip_pkg, 0, sizeof(tdm_soc_t));

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        SOC_PBMP_ASSIGN(pbmp, si->pipe_pbm[pipe]);
        SOC_PBMP_AND(pbmp, PBMP_HG_ALL(unit));
        mode[pipe] = SOC_PBMP_NOT_NULL(pbmp) ? PORT_HIGIG2 : PORT_ETHERNET;
    }

    for (index = 0; index < COUNTOF(tdm->pm_encap_type); index++) {
        tdm->pm_encap_type[index] = PORT_ETHERNET;
    }
    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            continue;
        }
        phy_port = si->port_l2p_mapping[port];
        tdm->port_state[phy_port] =
            SOC_PBMP_MEMBER(si->oversub_pbm, port) ?
            PORT_STATE_OVERSUB : PORT_STATE_LINERATE;
        for (lane = 1; lane < si->port_num_lanes[port]; lane++) {
            tdm->port_state[phy_port + lane] = PORT_STATE_CONTINUATION;
        }
        if (initTime) {
            /* When invoked by init misc, use speed obtained from
             * config file.
             */
            tdm->speed[phy_port] = si->port_speed_max[port];
            /* bootup in normmally config */
            if (si->os_mixed_sister_25_50_enable == 0) {
                tdm->speed[phy_port] = si->port_speed_max[port];
            } else { /* bootup in mixed config */
                sal_memset(&lanes_ctrl, 0, sizeof(lanes_ctrl));
                lanes_ctrl.port_base = port;
                lanes_ctrl.os_mixed_sister = si->os_mixed_sister_25_50_enable;
                soc_th_os_mixed_sister_port_mode_get(unit, initTime, &lanes_ctrl);
                if (lanes_ctrl.mixed_port_mode == MIXED_DUAL_MODE_50_0_25_0 ||
                    lanes_ctrl.mixed_port_mode == MIXED_DUAL_MODE_25_0_50_0 ||
                    lanes_ctrl.mixed_port_mode == MIXED_DUAL_MODE_25_0_25_0) {
                    tdm->speed[phy_port] = IS_HG_PORT(unit, port) ? 53000 : 50000;
                } else {
                    tdm->speed[phy_port] = si->port_speed_max[port];
                }
            }
        } else {
            /* When invoked as part of flexport transform use fixedup
             * speed.
             */
            tdm->speed[phy_port] = si->port_init_speed[port];
        }
    }

    /* CLPORT */
    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);
        soc_tomahawk_port_ratio_get(unit, initTime, clport, &tdm->port_ratio[clport]);
        tdm->pm_encap_type[clport] = mode[si->port_pipe[port]];
    }

    /* Management ports */
    SOC_PBMP_ITER(si->management_pbm, port) {
        phy_port = si->port_l2p_mapping[port];
        tdm->pm_encap_type[_TH_PBLKS_PER_DEV] = mode[si->port_pipe[port]];
        tdm->speed[phy_port] = si->port_speed_max[port];
    }

    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        LOG_CLI((BSL_META_U(unit,
                            "frequency: %dMHz\n"), si->frequency));
        LOG_CLI((BSL_META_U(unit,
                            "port speed:")));
        for (index = 0; index < TH_NUM_EXT_PORTS; index++) {
            if (index % 8 == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                " %6d"), tdm->speed[index]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
        LOG_CLI((BSL_META_U(unit,
                            "port state map:")));
        for (index = 0; index < TH_NUM_EXT_PORTS; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            if (index == 0 || index == (TH_NUM_EXT_PORTS - 1)) {
                LOG_CLI((BSL_META_U(unit,
                                    " ---")));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    " %3d"), tdm->port_state[index]));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
        LOG_CLI((BSL_META_U(unit,
                            "pm encap type:")));
        for (index = 0; index < TH_NUM_PM_MOD; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                " %3d"), tdm->pm_encap_type[index]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

    for (index = 0; index < TH_NUM_EXT_PORTS; index++) {
        tdm->tdm_globals.speed[index] = tdm->speed[index];
    }
    tdm->tdm_globals.clk_freq = si->frequency;
    for (index = 1; index < TH_NUM_EXT_PORTS; index++) {
        tdm->tdm_globals.port_rates_array[index-1] = tdm->port_state[index];
    }
    for (index = 1; index < TH_NUM_PM_MOD; index++) {
        tdm->tdm_globals.pm_encap_type[index] = tdm->pm_encap_type[index];
    }
    _chip_pkg.unit = unit;
    _chip_pkg.num_ext_ports = TH_NUM_EXT_PORTS;
    _chip_pkg.state=sal_alloc((_chip_pkg.num_ext_ports)*sizeof(int *),
                                                           "port state list");
    if (_chip_pkg.state == NULL) {
        return SOC_E_MEMORY;
    }
    _chip_pkg.speed=sal_alloc((_chip_pkg.num_ext_ports)*sizeof(int *),
                                                           "port speed list");
    if (_chip_pkg.speed == NULL) {
        sal_free(_chip_pkg.state);
        return SOC_E_MEMORY;
    }
    for (index=0; index<TH_NUM_PHY_PORTS; index++) {
        _chip_pkg.state[index] = tdm->pm_encap_type[((index)/TH_NUM_PM_LNS)]==
        PORT_HIGIG2?((tdm->tdm_globals.port_rates_array[index])|
        PSBMP_ENCAP_HIGIG):(tdm->tdm_globals.port_rates_array[index]);
    }
    for (index=TH_NUM_PHY_PORTS; index<TH_NUM_EXT_PORTS; index++) {
        _chip_pkg.state[index] = tdm->tdm_globals.port_rates_array[index];
    }
    for (index=0; index<(_chip_pkg.num_ext_ports); index++) {
        _chip_pkg.speed[index] = tdm->tdm_globals.speed[index];
    }
    _chip_pkg.soc_vars.th.cal_universal_en = si->fabric_port_enable;
    _chip_pkg.clk_freq = tdm->tdm_globals.clk_freq;
    _chip_pkg.soc_vars.th.mgmt_pm_hg=
            ((tdm->tdm_globals.pm_encap_type[TH_NUM_PHY_PM]==PORT_HIGIG2)&&
             ((tdm->tdm_globals.speed[129]>0)||
             (tdm->tdm_globals.speed[131]>0)))?(1):(0);
    _chip_pkg.tvec_size = 0;
    _tdm_pkg = _soc_set_tdm_tbl(SOC_SEL_TDM(&_chip_pkg));
    sal_free(_chip_pkg.state);
    sal_free(_chip_pkg.speed);
    if (NULL == _tdm_pkg) {
        LOG_CLI((BSL_META_U(unit,
            "Unsupported config for TDM calendar generation\n")));
        return SOC_E_FAIL;
    }
    else {
        for (cal_id = 0; cal_id < 4; cal_id++) {
            switch (cal_id) {
                case 0:
                    tdm_idb_pipe_group=_tdm_pkg->_chip_data.cal_0.cal_grp;
                    tdm_idb_pipe_main=_tdm_pkg->_chip_data.cal_0.cal_main;
                    tdm_mmu_pipe_group=_tdm_pkg->_chip_data.cal_4.cal_grp;
                    tdm_mmu_pipe_main=_tdm_pkg->_chip_data.cal_4.cal_main;
                    break;
                case 1:
                    tdm_idb_pipe_group=_tdm_pkg->_chip_data.cal_1.cal_grp;
                    tdm_idb_pipe_main=_tdm_pkg->_chip_data.cal_1.cal_main;
                    tdm_mmu_pipe_group=_tdm_pkg->_chip_data.cal_5.cal_grp;
                    tdm_mmu_pipe_main=_tdm_pkg->_chip_data.cal_5.cal_main;
                    break;
                case 2:
                    tdm_idb_pipe_group=_tdm_pkg->_chip_data.cal_2.cal_grp;
                    tdm_idb_pipe_main=_tdm_pkg->_chip_data.cal_2.cal_main;
                    tdm_mmu_pipe_group=_tdm_pkg->_chip_data.cal_6.cal_grp;
                    tdm_mmu_pipe_main=_tdm_pkg->_chip_data.cal_6.cal_main;
                    break;
                case 3:
                    tdm_idb_pipe_group=_tdm_pkg->_chip_data.cal_3.cal_grp;
                    tdm_idb_pipe_main=_tdm_pkg->_chip_data.cal_3.cal_main;
                    tdm_mmu_pipe_group=_tdm_pkg->_chip_data.cal_7.cal_grp;
                    tdm_mmu_pipe_main=_tdm_pkg->_chip_data.cal_7.cal_main;
                    break;
                default:
                    tdm_idb_pipe_group=NULL;
                    tdm_idb_pipe_main=NULL;
                    tdm_mmu_pipe_group=NULL;
                    tdm_mmu_pipe_main=NULL;
            }
            tdm->idb_tdm[cal_id] = sal_alloc(sizeof(int)*_TDM_LENGTH,
                                                     "IDB TDM main calendar");
            if (tdm->idb_tdm[cal_id] == NULL) {
                return SOC_E_MEMORY;
            }
            sal_memcpy(tdm->idb_tdm[cal_id], tdm_idb_pipe_main,
                                                     sizeof(int)*_TDM_LENGTH);
            tdm->mmu_tdm[cal_id] = sal_alloc(sizeof(int)*_TDM_LENGTH,
                                                     "MMU TDM main calendar");
            if (tdm->mmu_tdm[cal_id] == NULL) {
                return SOC_E_MEMORY;
            }
            sal_memcpy(tdm->mmu_tdm[cal_id], tdm_mmu_pipe_main,
                                                     sizeof(int)*_TDM_LENGTH);
            for (cal_grp = 0; cal_grp < _OVS_GROUP_COUNT; cal_grp++) {
                tdm->idb_ovs_tdm[cal_id][cal_grp] = sal_alloc(sizeof(int)*
                              _OVS_GROUP_TDM_LENGTH,"IDB TDM group calendar");
                if (tdm->idb_ovs_tdm[cal_id][cal_grp] == NULL) {
                    return SOC_E_MEMORY;
                }
                sal_memcpy(tdm->idb_ovs_tdm[cal_id][cal_grp],
              tdm_idb_pipe_group[cal_grp], sizeof(int)*_OVS_GROUP_TDM_LENGTH);
                tdm->mmu_ovs_tdm[cal_id][cal_grp] = sal_alloc(sizeof(int)*
                              _OVS_GROUP_TDM_LENGTH,"MMU TDM group calendar");
                if (tdm->mmu_ovs_tdm[cal_id][cal_grp] == NULL) {
                    return SOC_E_MEMORY;
                }
                sal_memcpy(tdm->mmu_ovs_tdm[cal_id][cal_grp],
              tdm_mmu_pipe_group[cal_grp], sizeof(int)*_OVS_GROUP_TDM_LENGTH);
            }
        }
        tdm_th_free(_tdm_pkg);
        sal_free(_tdm_pkg);
    }

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        /* Count number of TH_OVSB_TOKEN assigned by the TDM code */
        for (length = _TDM_LENGTH; length > 0; length--) {
            if (tdm->mmu_tdm[pipe][length - 1] != TH_NUM_EXT_PORTS) {
                break;
            }
        }
        ovs_core_slot_count = 0;
        for (index = 0; index < length; index++) {
            if (tdm->mmu_tdm[pipe][index] == TH_OVSB_TOKEN) {
                ovs_core_slot_count++;
            }
        }

        /* Count number of slot needed for the oversub I/O bandwidth */
        ovs_io_slot_count = 0;
        for (group = 0; group < _OVS_GROUP_COUNT; group++) {
            port_slot_count = 0;
            for (index = 0; index < _OVS_GROUP_TDM_LENGTH; index++) {
                phy_port = tdm->mmu_ovs_tdm[pipe][group][index];
                if (phy_port == TH_NUM_EXT_PORTS) {
                    continue;
                }
                if (port_slot_count == 0) {
                    _soc_tomahawk_speed_to_slot_mapping
                        (unit, tdm->speed[phy_port], &port_slot_count);
                }
                ovs_io_slot_count += port_slot_count;
            }
        }

        if (ovs_core_slot_count != 0) {
            tdm->ovs_ratio_x1000[pipe] =
                ovs_io_slot_count * 1000 / ovs_core_slot_count;
        }
    }

    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
            LOG_CLI((BSL_META_U(unit,
                                "Pipe %d idb_tdm:"), pipe));
            for (index = 0; index < _TDM_LENGTH; index++) {
                if (index % 16 == 0) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n    ")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    " %3d"), tdm->idb_tdm[pipe][index]));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
            for (group = 0; group < _OVS_GROUP_COUNT; group++) {
                LOG_CLI((BSL_META_U(unit,
                                    "Pipe %d group %d idb_ovs_tdm"), pipe, group));
                for (index = 0; index < _OVS_GROUP_TDM_LENGTH; index++) {
                    if (index % 16 == 0) {
                        LOG_CLI((BSL_META_U(unit,
                                            "\n    ")));
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        " %3d"), tdm->idb_ovs_tdm[pipe][group][index]));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "\n")));
            }

            LOG_CLI((BSL_META_U(unit,
                                "Pipe %d mmu_tdm:"), pipe));
            for (index = 0; index < _TDM_LENGTH; index++) {
                if (index % 16 == 0) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n    ")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    " %3d"), tdm->mmu_tdm[pipe][index]));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
            for (group = 0; group < _OVS_GROUP_COUNT; group++) {
                LOG_CLI((BSL_META_U(unit,
                                    "Pipe %d group %d mmu_ovs_tdm"),
                         pipe, group));
                for (index = 0; index < _OVS_GROUP_TDM_LENGTH; index++) {
                    if (index % 16 == 0) {
                        LOG_CLI((BSL_META_U(unit,
                                            "\n    ")));
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        " %3d"), tdm->mmu_ovs_tdm[pipe][group][index]));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "\n")));
            }
            LOG_CLI((BSL_META_U(unit,
                                "Pipe %d oversubscribe ratio %d.%-2d\n"),
                     pipe, tdm->ovs_ratio_x1000[pipe] / 1000,
                     tdm->ovs_ratio_x1000[pipe] % 1000));
        }
    }

    return SOC_E_NONE;
}

STATIC int 
_soc_tomahawk_tdm_calculation_free_memory(int unit)
{
    int pipe, cal_grp;
    soc_control_t *soc;
    _soc_tomahawk_tdm_t *tdm;

    soc = SOC_CONTROL(unit);
    tdm = NULL;
    if (soc!=NULL){
        if(soc->tdm_info!=NULL){
            tdm = soc->tdm_info;
        }
    }
    if (tdm!=NULL){
        for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
            if (tdm->idb_tdm[pipe]!=NULL){
                sal_free(tdm->idb_tdm[pipe]);
            }
            if (tdm->mmu_tdm[pipe]!=NULL){
                sal_free(tdm->mmu_tdm[pipe]);
            }
            for (cal_grp = 0; cal_grp < _OVS_GROUP_COUNT; cal_grp++) {
                if (tdm->idb_ovs_tdm[pipe][cal_grp]!=NULL){
                    sal_free(tdm->idb_ovs_tdm[pipe][cal_grp]);
                }
                if (tdm->mmu_ovs_tdm[pipe][cal_grp]!=NULL){
                    sal_free(tdm->mmu_ovs_tdm[pipe][cal_grp]);
                }
            }
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_hash_init(int unit)
{
    soc_field_t fields[4];
    uint32 values[4];


    /* L2 dedicated banks */
    fields[0] = BANK0_HASH_OFFSETf;
    values[0] = 0;  /* CRC32_LOWER */
    fields[1] = BANK1_HASH_OFFSETf;
    values[1] = 16; /* CRC32_UPPER */
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L2_TABLE_HASH_CONTROLr, REG_PORT_ANY, 2,
                                 fields, values));

    if (soc_feature(unit, soc_feature_l3)) {
        /* L3 dedicated banks */
        fields[0] = BANK6_HASH_OFFSETf;
        values[0] = 0;  /* CRC32_LOWER */
        fields[1] = BANK7_HASH_OFFSETf;
        values[1] = 8;  /* CRC32 >> 8 */
        fields[2] = BANK8_HASH_OFFSETf;
        values[2] = 16; /* CRC32_UPPER */
        fields[3] = BANK9_HASH_OFFSETf;
        values[3] = 32; /* CRC16 */
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, L3_TABLE_HASH_CONTROLr, REG_PORT_ANY, 4,
                                     fields, values));

        /* SHARED_TABLE_HASH_CONTROLr is not accessible in
         * low, balanced latency modes */
        if (SOC_REG_IS_VALID(unit, SHARED_TABLE_HASH_CONTROLr)) {
            /* L2/L3/ALPM shared banks */
            fields[0] = BANK2_HASH_OFFSETf;
            values[0] = 4;  /* CRC32 >> 4 */
            fields[1] = BANK3_HASH_OFFSETf;
            values[1] = 12; /* CRC32 >> 12 */
            fields[2] = BANK4_HASH_OFFSETf;
            values[2] = 20; /* CRC32 >> 20 */
            fields[3] = BANK5_HASH_OFFSETf;
            values[3] = 24; /* CRC32 >> 24 */
            SOC_IF_ERROR_RETURN
                (soc_reg_fields32_modify(unit, SHARED_TABLE_HASH_CONTROLr,
                                         REG_PORT_ANY, 4, fields, values));
        }
    }
#ifdef SOC_ROBUST_HASH
    /* Robust Hash initialization */
    if (soc_feature(unit, soc_feature_robust_hash)) {
        uint32 value, seed = 0;

        if (NULL == ROBUSTHASH(unit)) {
            ROBUSTHASH(unit) = sal_alloc(sizeof(soc_robust_hash_db_t),
                                         "soc_robust_hash");
            if (ROBUSTHASH(unit) == NULL) {
                return SOC_E_MEMORY;
            }
            sal_memset(ROBUSTHASH(unit), 0, sizeof(soc_robust_hash_db_t));
        }

        /* Enable Robust hashing for VLAN_XLATE table and configure corresponding
           remap and action tables */
        if ((soc_property_get(unit, "robust_hash_disable_vlan", 0)) == 1) {
            /* Disable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_VLAN_XLATE_HASH_CONTROLr(unit, &value));
            soc_reg_field_set(unit, VLAN_XLATE_HASH_CONTROLr, &value,
                                        ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_VLAN_XLATE_HASH_CONTROLr(unit, value));

            ROBUSTHASH(unit)->ing_xlate.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_VLAN_XLATE_HASH_CONTROLr(unit, &value));
            soc_reg_field_set(unit, VLAN_XLATE_HASH_CONTROLr, &value,
                                        ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_VLAN_XLATE_HASH_CONTROLr(unit, value));
            ROBUSTHASH(unit)->ing_xlate.enable = 1;

            /* Fill action and remap tables with random data */
            ROBUSTHASH(unit)->ing_xlate.remap_tab[0] = ING_VXLT_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->ing_xlate.remap_tab[1] = ING_VXLT_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->ing_xlate.action_tab[0] = ING_VXLT_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->ing_xlate.action_tab[1] = ING_VXLT_ACTION_TABLE_Bm;

            seed = soc_property_get(unit, spn_ROBUST_HASH_SEED_VLAN, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->ing_xlate),
                                           seed));
        }

        /* Enable Robust hashing for EGR_VLAN_XLATE table and configure corresponding
           remap and action tables */
        if ((soc_property_get(unit, "robust_hash_disable_egress_vlan", 0)) == 1) {
            /* Disable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_EGR_VLAN_XLATE_HASH_CONTROLr(unit, &value));
            soc_reg_field_set(unit, EGR_VLAN_XLATE_HASH_CONTROLr, &value,
                                        ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_VLAN_XLATE_HASH_CONTROLr(unit, value));

            ROBUSTHASH(unit)->egr_xlate.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_EGR_VLAN_XLATE_HASH_CONTROLr(unit, &value));
            soc_reg_field_set(unit, EGR_VLAN_XLATE_HASH_CONTROLr, &value,
                                        ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_VLAN_XLATE_HASH_CONTROLr(unit, value));

            ROBUSTHASH(unit)->egr_xlate.enable = 1;

            /* Fill action and remap tables with random data */
            ROBUSTHASH(unit)->egr_xlate.remap_tab[0] = EGR_VXLT_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->egr_xlate.remap_tab[1] = EGR_VXLT_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->egr_xlate.action_tab[0] = EGR_VXLT_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->egr_xlate.action_tab[1] = EGR_VXLT_ACTION_TABLE_Bm;

            seed = soc_property_get(unit, spn_ROBUST_HASH_SEED_EGRESS_VLAN,
                                    16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->egr_xlate),
                                           seed));
        }

        /* Enable Robust hashing for MPLS_ENTRY table and configure corresponding
           remap and action tables */
        if ((soc_property_get(unit, "robust_hash_disable_mpls", 0)) == 1) {
            /* Disable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_MPLS_ENTRY_HASH_CONTROLr(unit, &value));
            soc_reg_field_set(unit, MPLS_ENTRY_HASH_CONTROLr, &value,
                    ROBUST_HASH_ENf, 0x0);
            SOC_IF_ERROR_RETURN
                (WRITE_MPLS_ENTRY_HASH_CONTROLr(unit, value));
            ROBUSTHASH(unit)->mpls_entry.enable = 0;
        } else {
            /* Enable robust hashing */
            SOC_IF_ERROR_RETURN
                (READ_MPLS_ENTRY_HASH_CONTROLr(unit, &value));
            soc_reg_field_set(unit, MPLS_ENTRY_HASH_CONTROLr, &value,
                    ROBUST_HASH_ENf, 0x1);
            SOC_IF_ERROR_RETURN
                (WRITE_MPLS_ENTRY_HASH_CONTROLr(unit, value));
            ROBUSTHASH(unit)->mpls_entry.enable = 1;

            /* Fill action and remap tables with random data */
            ROBUSTHASH(unit)->mpls_entry.remap_tab[0] = ING_MPLS_ENTRY_REMAP_TABLE_Am;
            ROBUSTHASH(unit)->mpls_entry.remap_tab[1] = ING_MPLS_ENTRY_REMAP_TABLE_Bm;
            ROBUSTHASH(unit)->mpls_entry.action_tab[0] = ING_MPLS_ENTRY_ACTION_TABLE_Am;
            ROBUSTHASH(unit)->mpls_entry.action_tab[1] = ING_MPLS_ENTRY_ACTION_TABLE_Bm;

            seed = soc_property_get(unit, spn_ROBUST_HASH_SEED_MPLS, 16777213);

            SOC_IF_ERROR_RETURN
                (soc_robust_hash_table_set(unit,
                                           &(ROBUSTHASH(unit)->mpls_entry),
                                           seed));
       }
    }
#endif /* SOC_ROBUST_HASH */

    return SOC_E_NONE;
}

/* For a given logical port, return the OBM id and the lane number */
int
soc_tomahawk_port_obm_info_get(int unit, soc_port_t port,
                               int *obm_id, int *lane)
{
    soc_info_t *si;
    soc_port_t phy_port;
    int pipe, clport, port_block_base, ports_per_pblk;

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        (SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port))) {
        return SOC_E_PARAM;
    }
    si = &SOC_INFO(unit);
    pipe = si->port_pipe[port];
    phy_port = si->port_l2p_mapping[port];
    port_block_base = PORT_BLOCK_BASE_PORT(port);
    clport = si->port_serdes[port];

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        port_block_base = SOC_TH3_PORT_BLOCK_BASE_PORT(port);
        ports_per_pblk = _TH3_PORTS_PER_PBLK;
        /* TH3 PIPE 2/3/6/7 has reversed (mirrored) OBM number*/
        *obm_id = pipe & 0x10 ? 3 - (clport & 0x3) : clport & 0x3;
    } else
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        ports_per_pblk = _TH2_PBLKS_PER_PIPE;
        *obm_id = clport & 0xF;
    } else
#endif
    {
        ports_per_pblk = _TH_PBLKS_PER_PIPE;
        /* obm number is reversed (mirrored) in odd pipe */
        *obm_id = pipe & 1 ? 7 - (clport & 0x7) : clport & 0x7;
    }

    *lane = (phy_port - port_block_base) % ports_per_pblk;

    if (*obm_id >= ports_per_pblk) {
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_idb_init(int unit)
{
#define _TH_BYTES_PER_OBM_CELL    16
#define _TH_CELLS_PER_OBM         1012
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    soc_reg_t reg;
    soc_mem_t mem;
    soc_field_t field;
    int block_info_idx, pipe, clport, obm, lane;
    int port, phy_port_base;
    int lossless, i, count;
    uint32 rval0, rval1, fval;
    uint32 rval_port_cfg = 0, rval_max_usage_cfg = 0;
    uint64 rval64;
    int num_lanes, oversub_ratio_idx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    static const struct _obm_setting_s {
        int discard_limit;
        int lossless_xoff;
        int lossy_only_lossy_low_pri;
        int port_xoff[3];          /* for oversub ratio 1.5, 1.8, and above */
        int lossy_low_pri[3];      /* for oversub ratio 1.5, 1.8, and above */
        int lossy_discard[3];      /* for oversub ratio 1.5, 1.8, and above */
    } obm_settings[2][5] = {
        /* LOSSY Settings */
        { /* indexed by number of lanes used in the port */
            { /* 0 - invalid  */ 0 },
            { /* 1 lane */
                _TH_CELLS_PER_OBM / 4, 1023, 157,
                { 1023, 1023, 1023 }, /* port_xoff */
                { 157, 157, 157 }, /* lossy_low_pri */
                { 253, 253, 253 }  /* lossy_discard */
            },
            { /* 2 lanes */
                _TH_CELLS_PER_OBM / 2, 1023, 410,
                { 1023, 1023, 1023 }, /* port_xoff */
                { 410, 410, 410 }, /* lossy_low_pri */
                { 506, 506, 506 }  /* lossy_discard */
            },
            { /* 3 - invalid  */ 0 },
            { /* 4 lanes */
                _TH_CELLS_PER_OBM, 1023, 916,
                { 1023, 1023, 1023 }, /* port_xoff */
                { 916, 916, 916 }, /* lossy_low_pri */
                { 1012, 1012, 1012 }  /* lossy_discard */
            }
        },
        /* LOSSLESS Settings*/
        { /* indexed by number of lanes used in the port */
            { /* 0 - invalid  */ 0 },
            { /* 1 lane */
                _TH_CELLS_PER_OBM / 4, 45, 157,
                { 129, 87, 67 }, /* port_xoff */
                { 100, 76, 56 }, /* lossy_low_pri */
                { 196, 152, 112 }  /* lossy_discard */
            },
            { /* 2 lanes */
                _TH_CELLS_PER_OBM / 2, 108, 410,
                { 312, 248, 215 }, /* port_xoff */
                { 100, 100, 100 }, /* lossy_low_pri */
                { 196, 196, 196 }  /* lossy_discard */
            },
            { /* 3 - invalid  */ 0 },
            { /* 4 lanes */
                _TH_CELLS_PER_OBM, 258, 916,
                { 682, 572, 517 }, /* port_xoff */
                { 100, 100, 100 }, /* lossy_low_pri */
                { 196, 196, 196 }  /* lossy_discard */
            }
        }
    };
    static const soc_reg_t obm_ctrl_regs[] = {
        IDB_OBM0_CONTROLr, IDB_OBM1_CONTROLr,
        IDB_OBM2_CONTROLr, IDB_OBM3_CONTROLr,
        IDB_OBM4_CONTROLr, IDB_OBM5_CONTROLr,
        IDB_OBM6_CONTROLr, IDB_OBM7_CONTROLr
    };
    static const soc_reg_t obm_ca_ctrl_regs[] = {
        IDB_OBM0_CA_CONTROLr, IDB_OBM1_CA_CONTROLr,
        IDB_OBM2_CA_CONTROLr, IDB_OBM3_CA_CONTROLr,
        IDB_OBM4_CA_CONTROLr, IDB_OBM5_CA_CONTROLr,
        IDB_OBM6_CA_CONTROLr, IDB_OBM7_CA_CONTROLr
    };
    static const soc_reg_t obm_thresh_regs[] = {
        IDB_OBM0_THRESHOLDr, IDB_OBM1_THRESHOLDr,
        IDB_OBM2_THRESHOLDr, IDB_OBM3_THRESHOLDr,
        IDB_OBM4_THRESHOLDr, IDB_OBM5_THRESHOLDr,
        IDB_OBM6_THRESHOLDr, IDB_OBM7_THRESHOLDr
    };
    static const soc_reg_t obm_fc_thresh_regs[] = {
        IDB_OBM0_FC_THRESHOLDr, IDB_OBM1_FC_THRESHOLDr,
        IDB_OBM2_FC_THRESHOLDr, IDB_OBM3_FC_THRESHOLDr,
        IDB_OBM4_FC_THRESHOLDr, IDB_OBM5_FC_THRESHOLDr,
        IDB_OBM6_FC_THRESHOLDr, IDB_OBM7_FC_THRESHOLDr
    };
    static const soc_reg_t obm_shared_regs[] = {
        IDB_OBM0_SHARED_CONFIGr, IDB_OBM1_SHARED_CONFIGr,
        IDB_OBM2_SHARED_CONFIGr, IDB_OBM3_SHARED_CONFIGr,
        IDB_OBM4_SHARED_CONFIGr, IDB_OBM5_SHARED_CONFIGr,
        IDB_OBM6_SHARED_CONFIGr, IDB_OBM7_SHARED_CONFIGr
    };
    static const soc_reg_t obm_max_usage_regs[] = {
        IDB_OBM0_MAX_USAGE_SELECTr, IDB_OBM1_MAX_USAGE_SELECTr,
        IDB_OBM2_MAX_USAGE_SELECTr, IDB_OBM3_MAX_USAGE_SELECTr,
        IDB_OBM4_MAX_USAGE_SELECTr, IDB_OBM5_MAX_USAGE_SELECTr,
        IDB_OBM6_MAX_USAGE_SELECTr, IDB_OBM7_MAX_USAGE_SELECTr
    };
    static const soc_reg_t obm_port_config_regs[] = {
        IDB_OBM0_PORT_CONFIGr, IDB_OBM1_PORT_CONFIGr,
        IDB_OBM2_PORT_CONFIGr, IDB_OBM3_PORT_CONFIGr,
        IDB_OBM4_PORT_CONFIGr, IDB_OBM5_PORT_CONFIGr,
        IDB_OBM6_PORT_CONFIGr, IDB_OBM7_PORT_CONFIGr
    };
    static const soc_reg_t obm_fc_config_regs[] = {
        IDB_OBM0_FLOW_CONTROL_CONFIGr, IDB_OBM1_FLOW_CONTROL_CONFIGr,
        IDB_OBM2_FLOW_CONTROL_CONFIGr, IDB_OBM3_FLOW_CONTROL_CONFIGr,
        IDB_OBM4_FLOW_CONTROL_CONFIGr, IDB_OBM5_FLOW_CONTROL_CONFIGr,
        IDB_OBM6_FLOW_CONTROL_CONFIGr, IDB_OBM7_FLOW_CONTROL_CONFIGr
    };
    static const soc_field_t obm_bypass_fields[] = {
        PORT0_BYPASS_ENABLEf, PORT1_BYPASS_ENABLEf,
        PORT2_BYPASS_ENABLEf, PORT3_BYPASS_ENABLEf
    };
    static const soc_field_t obm_oversub_fields[] = {
        PORT0_OVERSUB_ENABLEf, PORT1_OVERSUB_ENABLEf,
        PORT2_OVERSUB_ENABLEf, PORT3_OVERSUB_ENABLEf
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
    };
    static const soc_field_t obm_ob_pri_fields[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf, OFFSET8_OB_PRIORITYf,
        OFFSET9_OB_PRIORITYf, OFFSET10_OB_PRIORITYf, OFFSET11_OB_PRIORITYf,
        OFFSET12_OB_PRIORITYf, OFFSET13_OB_PRIORITYf, OFFSET14_OB_PRIORITYf,
        OFFSET15_OB_PRIORITYf
    };
    static const int hw_mode_values[SOC_TH_PORT_RATIO_COUNT] = {
        0, 1, 1, 1, 3, 5, 4, 6, 2
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    if (SOC_IS_TOMAHAWKPLUS(unit)) {
        lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 
                                    SOC_TH_MMU_LOSSLESS_DEFAULT_DISABLE);
    } else {
        lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 
                                    SOC_TH_MMU_LOSSLESS_DEFAULT_ENABLE);
    }

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
        /* obm number is reversed (mirrored) in odd pipe */
        obm = pipe & 1 ? 7 - (clport & 0x7) : clport & 0x7;

        /* Set cell assembly mode then toggle reset to send initial credit
         * to EP */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_ca_ctrl_regs[obm])[pipe];
        rval0 = 0;
        soc_reg_field_set(unit, reg, &rval0, PORT_MODEf,
                          hw_mode_values[tdm->port_ratio[clport]]);
        rval1 = rval0;
        soc_reg_field_set(unit, reg, &rval0, PORT0_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT1_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT2_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT3_RESETf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval1));

        /* Enable oversub */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_ctrl_regs[obm])[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval0));
        for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane++) {
            if (si->port_p2l_mapping[phy_port_base + lane] == -1) {
                continue;
            }
            field = obm_bypass_fields[lane];
            soc_reg_field_set(unit, reg, &rval0, field, 1);
            field = obm_oversub_fields[lane];
            soc_reg_field_set(unit, reg, &rval0, field, 1);
        }

        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        if (!SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
            continue;
        }

        /* Configure shared config */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_shared_regs[obm])[pipe];
        SOC_IF_ERROR_RETURN
            (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, DISCARD_THRESHOLDf, 1023);
        soc_reg64_field32_set(unit, reg, &rval64, SOP_THRESHOLDf, 1023);
        soc_reg64_field32_set(unit, reg, &rval64, SOP_DISCARD_MODEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

        if (lossless) {
            if (tdm->ovs_ratio_x1000[pipe] >= 1800) { /* ratio >= 1.8 */
                oversub_ratio_idx = 2;
            } else if (tdm->ovs_ratio_x1000[pipe] >= 1500) { /* ratio >= 1.5 */
                oversub_ratio_idx = 1;
            } else {
                oversub_ratio_idx = 0;
            }

            for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane++) {
                port = si->port_p2l_mapping[phy_port_base + lane];
                if (port == -1) {
                    continue;
                }
                num_lanes = si->port_num_lanes[port];

                /* Configure threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[1][num_lanes].discard_limit;
                soc_reg64_field32_set(unit, reg, &rval64, DISCARD_LIMITf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                                      1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_DISCARDf,
                                      1023);
                fval = obm_settings[1][num_lanes].lossy_low_pri[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_LOW_PRIf, fval);
                fval = obm_settings[1][num_lanes].lossy_discard[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_DISCARDf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_MINf, 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[1][num_lanes].port_xoff[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XOFFf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XONf, fval - 10);
                fval = obm_settings[1][num_lanes].lossless_xoff;
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                                      fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XOFFf,
                                      fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XONf,
                                      fval - 10);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XONf,
                                      fval - 10);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control config */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_config_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, PORT_FC_ENf, 1);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_FC_ENf, 1);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_FC_ENf, 1);
                soc_reg64_field32_set(unit, reg, &rval64,
                                      LOSSLESS0_PRIORITY_PROFILEf, 0xff);
                soc_reg64_field32_set(unit, reg, &rval64,
                                      LOSSLESS1_PRIORITY_PROFILEf, 0xff);
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
                (soc_reg32_set(unit, reg, REG_PORT_ANY, 0,
                               rval_max_usage_cfg));

        } else { /* lossy */
            for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane++) {
                port = si->port_p2l_mapping[phy_port_base + lane];
                if (port == -1) {
                    continue;
                }
                num_lanes = si->port_num_lanes[port];

                /* Configure threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[0][num_lanes].discard_limit;
                soc_reg64_field32_set(unit, reg, &rval64, DISCARD_LIMITf, fval);
                /* same as DISCARD_LIMIT setting */
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_DISCARDf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                                      1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_DISCARDf,
                                      1023);
                fval = obm_settings[0][num_lanes].lossy_only_lossy_low_pri;
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_LOW_PRIf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_MINf, 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XOFFf, 1023);
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XONf, 1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                                      1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XOFFf,
                                      1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XONf, 1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XONf, 1023);
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
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));

    /* Toggle loopback port cell assembly reset to send initial credit to EP */
    reg = IDB_CA_LPBK_CONTROLr;
    rval0 = 0;
    soc_reg_field_set(unit, reg, &rval0, PORT_RESETf, 1);
    PBMP_LB_ITER(unit, port) {
        reg = SOC_REG_UNIQUE_ACC(unit, IDB_CA_LPBK_CONTROLr)
            [si->port_pipe[port]];
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));
    }

    return SOC_E_NONE;
}

extern int
(*_phy_tsce_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);
extern int
(*_phy_tscf_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);

STATIC int
_soc_tomahawk_tscx_firmware_set(int unit, int port, uint8 *array, int datalen)
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
_soc_tomahawk_mdio_addr_to_port(uint32 phy_addr)
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
     * bus 2 phy 1 to 24 are mapped to Physical port 41 to 64
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
    case 3: offset = 64;
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
_soc_tomahawk_tscx_reg_read(int unit, uint32 phy_addr,
                            uint32 phy_reg, uint32 *phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_tomahawk_mdio_addr_to_port(phy_addr);

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        phy_port = soc_tomahawk2_mdio_addr_to_port(phy_addr);
    }
#endif

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_tomahawk_tscx_reg_read[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
    rv = soc_sbus_tsc_reg_read(unit, port, blk, phy_addr,
                               phy_reg, phy_data);
    return rv;
}

STATIC int
_soc_tomahawk_tscx_reg_write(int unit, uint32 phy_addr,
                             uint32 phy_reg, uint32 phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_tomahawk_mdio_addr_to_port(phy_addr);

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        phy_port = soc_tomahawk2_mdio_addr_to_port(phy_addr);
    }
#endif

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_tomahawk_tscx_reg_write[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
    rv = soc_sbus_tsc_reg_write(unit, port, blk, phy_addr,
                                phy_reg, phy_data);
    return rv;
}

STATIC int
_soc_th_ledup_init(int unit)
{
    uint32 rval = 0;
    int ix, pval1, pval2, pval_size;

    typedef struct led_remap_s {
        uint32 reg_addr;
        uint32 port0;
        uint32 port1;
        uint32 port2;
        uint32 port3;
    } led_remap_t;

    led_remap_t led0_remap[] = {
        { CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r,
         REMAP_PORT_0f, REMAP_PORT_1f, REMAP_PORT_2f, REMAP_PORT_3f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r,
         REMAP_PORT_4f, REMAP_PORT_5f, REMAP_PORT_6f, REMAP_PORT_7f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r,
         REMAP_PORT_8f, REMAP_PORT_9f, REMAP_PORT_10f, REMAP_PORT_11f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r,
         REMAP_PORT_12f, REMAP_PORT_13f, REMAP_PORT_14f, REMAP_PORT_15f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r,
         REMAP_PORT_16f, REMAP_PORT_17f, REMAP_PORT_18f, REMAP_PORT_19f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r,
         REMAP_PORT_20f, REMAP_PORT_21f, REMAP_PORT_22f, REMAP_PORT_23f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r,
         REMAP_PORT_24f, REMAP_PORT_25f, REMAP_PORT_26f, REMAP_PORT_27f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r,
         REMAP_PORT_28f, REMAP_PORT_29f, REMAP_PORT_30f, REMAP_PORT_31f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r,
         REMAP_PORT_32f, REMAP_PORT_33f, REMAP_PORT_34f, REMAP_PORT_35f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r,
         REMAP_PORT_36f, REMAP_PORT_37f, REMAP_PORT_38f, REMAP_PORT_39f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r,
         REMAP_PORT_40f, REMAP_PORT_41f, REMAP_PORT_42f, REMAP_PORT_43f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r,
         REMAP_PORT_44f, REMAP_PORT_45f, REMAP_PORT_46f, REMAP_PORT_47f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r,
         REMAP_PORT_48f, REMAP_PORT_49f, REMAP_PORT_50f, REMAP_PORT_51f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r,
         REMAP_PORT_52f, REMAP_PORT_53f, REMAP_PORT_54f, REMAP_PORT_55f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r,
         REMAP_PORT_56f, REMAP_PORT_57f, REMAP_PORT_58f, REMAP_PORT_59f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r,
         REMAP_PORT_60f, REMAP_PORT_61f, REMAP_PORT_62f, REMAP_PORT_63f }
    };

    led_remap_t led1_remap[] = {
        { CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r,
         REMAP_PORT_0f, REMAP_PORT_1f, REMAP_PORT_2f, REMAP_PORT_3f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r,
         REMAP_PORT_4f, REMAP_PORT_5f, REMAP_PORT_6f, REMAP_PORT_7f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r,
         REMAP_PORT_8f, REMAP_PORT_9f, REMAP_PORT_10f, REMAP_PORT_11f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r,
         REMAP_PORT_12f, REMAP_PORT_13f, REMAP_PORT_14f, REMAP_PORT_15f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r,
         REMAP_PORT_16f, REMAP_PORT_17f, REMAP_PORT_18f, REMAP_PORT_19f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r,
         REMAP_PORT_20f, REMAP_PORT_21f, REMAP_PORT_22f, REMAP_PORT_23f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_24_27r,
         REMAP_PORT_24f, REMAP_PORT_25f, REMAP_PORT_26f, REMAP_PORT_27f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_28_31r,
         REMAP_PORT_28f, REMAP_PORT_29f, REMAP_PORT_30f, REMAP_PORT_31f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_32_35r,
         REMAP_PORT_32f, REMAP_PORT_33f, REMAP_PORT_34f, REMAP_PORT_35f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_36_39r,
         REMAP_PORT_36f, REMAP_PORT_37f, REMAP_PORT_38f, REMAP_PORT_39f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_40_43r,
         REMAP_PORT_40f, REMAP_PORT_41f, REMAP_PORT_42f, REMAP_PORT_43f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_44_47r,
         REMAP_PORT_44f, REMAP_PORT_45f, REMAP_PORT_46f, REMAP_PORT_47f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_48_51r,
         REMAP_PORT_48f, REMAP_PORT_49f, REMAP_PORT_50f, REMAP_PORT_51f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_52_55r,
         REMAP_PORT_52f, REMAP_PORT_53f, REMAP_PORT_54f, REMAP_PORT_55f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_56_59r,
         REMAP_PORT_56f, REMAP_PORT_57f, REMAP_PORT_58f, REMAP_PORT_59f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_60_63r,
         REMAP_PORT_60f, REMAP_PORT_61f, REMAP_PORT_62f, REMAP_PORT_63f }
    };

    led_remap_t led2_remap[] = {
        { CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r,
         REMAP_PORT_0f, REMAP_PORT_1f, REMAP_PORT_2f, REMAP_PORT_3f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r,
         REMAP_PORT_4f, REMAP_PORT_5f, REMAP_PORT_6f, REMAP_PORT_7f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r,
         REMAP_PORT_8f, REMAP_PORT_9f, REMAP_PORT_10f, REMAP_PORT_11f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r,
         REMAP_PORT_12f, REMAP_PORT_13f, REMAP_PORT_14f, REMAP_PORT_15f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r,
         REMAP_PORT_16f, REMAP_PORT_17f, REMAP_PORT_18f, REMAP_PORT_19f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r,
         REMAP_PORT_20f, REMAP_PORT_21f, REMAP_PORT_22f, REMAP_PORT_23f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_24_27r,
         REMAP_PORT_24f, REMAP_PORT_25f, REMAP_PORT_26f, REMAP_PORT_27f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_28_31r,
         REMAP_PORT_28f, REMAP_PORT_29f, REMAP_PORT_30f, REMAP_PORT_31f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_32_35r,
         REMAP_PORT_32f, REMAP_PORT_33f, REMAP_PORT_34f, REMAP_PORT_35f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_36_39r,
         REMAP_PORT_36f, REMAP_PORT_37f, REMAP_PORT_38f, REMAP_PORT_39f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_40_43r,
         REMAP_PORT_40f, REMAP_PORT_41f, REMAP_PORT_42f, REMAP_PORT_43f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_44_47r,
         REMAP_PORT_44f, REMAP_PORT_45f, REMAP_PORT_46f, REMAP_PORT_47f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_48_51r,
         REMAP_PORT_48f, REMAP_PORT_49f, REMAP_PORT_50f, REMAP_PORT_51f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_52_55r,
         REMAP_PORT_52f, REMAP_PORT_53f, REMAP_PORT_54f, REMAP_PORT_55f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_56_59r,
         REMAP_PORT_56f, REMAP_PORT_57f, REMAP_PORT_58f, REMAP_PORT_59f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_60_63r,
         REMAP_PORT_60f, REMAP_PORT_61f, REMAP_PORT_62f, REMAP_PORT_63f }
    };


    pval_size = COUNTOF(led0_remap);
    pval2 = (pval_size >> 1) - 1;

    /* Configure LED-0 remap register settings.
     * LED-0 chain provides port status for tile0 (ports 1-32)
     * and tile 3 (ports 97-128).
     * Scan chain order: 32, 31, ...., 1, 128, 127, ...., 97
     * LED port status for port 32 arrives first at the CMIC block and
     * port 97's LED status arrives last.
     */
    for (ix = 0; ix < pval_size ; ix++) {
        if (ix == pval_size >> 1) {
            pval2 = pval_size -1;
        }

        pval1 = ix << 2;

        rval = 0;
        soc_reg_field_set(unit, led0_remap[pval2].reg_addr, &rval,
                led0_remap[pval2].port3, pval1);
        soc_reg_field_set(unit, led0_remap[pval2].reg_addr, &rval,
                led0_remap[pval2].port2, pval1 + 1);
        soc_reg_field_set(unit, led0_remap[pval2].reg_addr, &rval,
                led0_remap[pval2].port1, pval1 + 2);
        soc_reg_field_set(unit, led0_remap[pval2].reg_addr, &rval,
                led0_remap[pval2].port0, pval1 + 3);
        soc_pci_write(unit, soc_reg_addr(unit,
                      led0_remap[pval2].reg_addr, REG_PORT_ANY, 0), rval);
        pval2--;
    }

    /* Configure LED-1 remap register settings.
     * LED-1 chain provides port status for tile1 (ports 33-64)
     * and tile 2 (ports 65-96).
     * Scan chain order: 33, 34, 35, ...., 96
     * LED port status for port 33 arrives first at the CMIC block and
     * port 96's LED status arrives last.
     */
    for (ix = 0; ix < pval_size ; ix++) {
        pval1 = ix << 2;

        rval = 0;
        soc_reg_field_set(unit, led1_remap[ix].reg_addr, &rval,
                led1_remap[ix].port3, pval1);
        soc_reg_field_set(unit, led1_remap[ix].reg_addr, &rval,
                led1_remap[ix].port2, pval1 + 1);
        soc_reg_field_set(unit, led1_remap[ix].reg_addr, &rval,
                led1_remap[ix].port1, pval1 + 2);
        soc_reg_field_set(unit, led1_remap[ix].reg_addr, &rval,
                led1_remap[ix].port0, pval1 + 3);
        soc_pci_write(unit, soc_reg_addr(unit,
                      led1_remap[ix].reg_addr, REG_PORT_ANY, 0), rval);
    }

    /* Configure LED-2 remap register settings.
     * LED-2 chain provides port status for management ports (129-132).
     * Scan chain port order is - 132 (unused), 131, 130 (unused), 129.
     * Unused remapping port registers are programmed to a larger value.
     */
    for (ix = 0; ix < pval_size ; ix++) {
        rval = 0;
        soc_reg_field_set(unit, led2_remap[ix].reg_addr, &rval,
                led2_remap[ix].port0, 0x3F);
        soc_reg_field_set(unit, led2_remap[ix].reg_addr, &rval,
                led2_remap[ix].port1, 0x3F);
        soc_reg_field_set(unit, led2_remap[ix].reg_addr, &rval,
                led2_remap[ix].port2, 0x3F);
        soc_reg_field_set(unit, led2_remap[ix].reg_addr, &rval,
                led2_remap[ix].port3, 0x3F);

        if (ix == 0) {
            soc_reg_field_set(unit, led2_remap[ix].reg_addr, &rval,
                    led2_remap[ix].port1, 1);
            soc_reg_field_set(unit, led2_remap[ix].reg_addr, &rval,
                    led2_remap[ix].port3, 0);
        }

        soc_pci_write(unit, soc_reg_addr(unit, led2_remap[ix].reg_addr,
                                         REG_PORT_ANY, 0), rval);
    }

    /* initialize the LED processors data ram */
    rval = 0;
    for (ix = 0; ix < 256; ix++) {
        WRITE_CMIC_LEDUP0_DATA_RAMr(unit,ix, rval);
        WRITE_CMIC_LEDUP1_DATA_RAMr(unit,ix, rval);
        soc_pci_write(unit,
                      soc_reg_addr(unit, CMIC_LEDUP2_DATA_RAMr, REG_PORT_ANY, ix),
                      rval);
    }

    return SOC_E_NONE;
}

static const soc_reg_t pvtmon_result_reg[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r,
    TOP_PVTMON_RESULT_2r, TOP_PVTMON_RESULT_3r,
    TOP_PVTMON_RESULT_4r, TOP_PVTMON_RESULT_5r,
    TOP_PVTMON_RESULT_6r, TOP_PVTMON_RESULT_7r
};

STATIC int
soc_th_port_config_validate(int unit)
{
    int encap, ovs, lport, blk;
    int port_sister_speed[4] = {-1, -1, -1, -1};
    int port_status[4] = {0};
    int port_base, port_base_speed = 0;
    soc_info_t *si = &SOC_INFO(unit);
    int mixed_25_50_en = 0;
    int i = 0, idx = 0;

    ovs = si->oversub_mode;
    mixed_25_50_en = si->os_mixed_sister_25_50_enable;

    if (!ovs) {
        return SOC_E_NONE;
    }

    for (blk = 0; blk < SOC_MAX_NUM_BLKS; blk++) {
        if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLPORT) {
            encap = SOC_ENCAP_COUNT;
            i = 0;
            for (idx = 0; idx < 4; idx++) {
                port_sister_speed[idx] = -1;
                port_status[idx] = 0;
            }

            SOC_PBMP_ITER(si->block_bitmap[blk], lport) {
                if (encap == SOC_ENCAP_COUNT) {
                    encap = IS_HG_PORT(unit, lport) ?
                            SOC_ENCAP_HIGIG2 : SOC_ENCAP_IEEE;
                    port_base = lport;
                    port_base_speed = si->port_speed_max[port_base];
                    ovs = SOC_PBMP_MEMBER(si->oversub_pbm, port_base) ? 1 : 0;
                }

                port_sister_speed[i] = si->port_speed_max[lport];
                port_status[i] = (!SOC_PBMP_MEMBER(si->all.disabled_bitmap, lport)) ? 1 : 0;
                i++;
            }

            /*
             * The port config was restricted.
             * portmap_1.0=1:100
             * portmap_2.0=2:25:i
             * portmap_3.0=3:50:i
             * portmap_4.0=4:25:i
             */
            if (((port_base_speed == 100000) && (port_sister_speed[1] == 25000) &&
                 (port_sister_speed[2] == 50000) && (port_sister_speed[3] == 25000) &&
                 (port_status[0]) && (!port_status[1]) && (!port_status[2]) && (!port_status[3])) ||
                ((port_sister_speed[0] == 25000) && (port_sister_speed[1] == 25000) &&
                 (port_sister_speed[2] == 50000) && (port_status[0]) && (port_status[1]) &&
                 (port_status[2]))) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "Unsupported port config in oversub mode on port macro %d\n"),
                          SOC_BLOCK_NUMBER(unit, blk)));
                return SOC_E_CONFIG;
            }
            if (!mixed_25_50_en &&
                (((port_sister_speed[0] == 25000) &&
                  (port_sister_speed[2] == 50000) &&
                  (port_status[0]) && (port_status[2])) ||
                 ((port_sister_speed[0] == 50000) &&
                  (port_sister_speed[2] == 25000) &&
                  (port_status[0]) && port_status[2]))) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "Unsupported port config in mixed mode on port macro %d\n"),
                           SOC_BLOCK_NUMBER(unit, blk)));
                return SOC_E_CONFIG;
            }
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_misc_init(int unit)
{
#ifdef BCM_TOMAHAWKX_SUPPORT
    static int rtag7_field_width[] = { 16, 16, 4, 16, 8, 8, 16, 16 };
    soc_mem_t mem;
    soc_reg_t reg;
    uint32 rval, rval1, fval;
    uint64 rval64;
    int rv, pipe = 0;
    soc_pbmp_t pbmp;
    uint8 rev_id;
    uint16 dev_id;
    int port_chk, num_port = 136;
    int port, phy_port_base, lport;
    _soc_tomahawk_tdm_t *tdm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int block_info_idx, parity_enable;
    int clport, lane, mode, ovs_class, index;
    int count, sub_sel, offset;
    int freq, target_freq, divisor, dividend, delay;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_info_t *si = &SOC_INFO(unit);
    int ecmp_mode = 0;
    int fabric_port_enable = 1;
    static soc_field_t port_fields[_TH_PORTS_PER_PBLK] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };
    static const int mode_encodings[SOC_TH_PORT_RATIO_COUNT] = {
        4, 3, 3, 3, 2, 2, 1, 1, 0
    };
    int phy_port_mgmt0, phy_port_mgmt1;
#if defined(BCM_TOMAHAWK2_SUPPORT)
    egr_mmu_cell_credit_entry_t credit_entry;
    char *board_name;
#endif

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Stop the mem scan task before all of the parity init takes place */
    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));
    SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_stop(unit));

    /* Prevent ASF from accessing phy */
    SOC_IF_ERROR_RETURN(soc_th_asf_deinit(unit));

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    if (parity_enable) {
        if ((!SAL_BOOT_SIMULATION || SAL_BOOT_QUICKTURN) &&
            !SOC_WARM_BOOT(unit)) {

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
            if (SOC_IS_TOMAHAWK(unit)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk_clear_mmu_memory(unit));
            }
#ifdef BCM_TOMAHAWK2_SUPPORT
            else if (SOC_IS_TOMAHAWK2(unit)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk2_clear_mmu_memory(unit)); 
            }
#endif
        }

        /* Check the whether the alpm tables use cache to recover*/
        soc_ser_alpm_cache_check(unit);

        /* Enable logging of SER events */
        soc_ser_log_init(unit, NULL, 0);

        /* Enable l2_mgmt interrupt */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, L2_MGMT_INTR_MASKr, REG_PORT_ANY,
                                    SER_FIFO_NOT_EMPTYf, 1));
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            (void)soc_cmicm_intr3_enable(unit, 0x2); /* L2_MGMT_TO_CMIC_INTR bit 1 */
        }
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            SOC_IF_ERROR_RETURN(soc_tomahawk2_clear_enabled_port_data(unit));
        }
#endif
        SOC_IF_ERROR_RETURN(soc_tomahawk_tcam_ser_init(unit));
        
        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_enable_all(unit, TRUE));
        
        soc_tomahawk_ser_register(unit);
#if defined(SER_TR_TEST_SUPPORT)
        /*Initialize chip-specific functions for SER testing*/
        soc_th_ser_test_register(unit);
#endif /*defined(SER_TR_TEST_SUPPORT*/
    }

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            SOC_IF_ERROR_RETURN(soc_tomahawk2_init_mmu_memory(unit));
        } else
#endif
        {
            SOC_IF_ERROR_RETURN(_soc_tomahawk_clear_all_memory(unit, 1));
        }

        /* Dummy call to clear garbage h/w values */
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_temperature_monitor_get(unit, COUNTOF(pvtmon_result_reg),
                                                  NULL, NULL));
    }

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        int latency;
        int lmode = SOC_SWITCH_BYPASS_MODE_NONE;

        SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &lmode));

        /* Set the latency in Slot Pipeline config register, need to be
         * programmed before any traffic going through the IP */
        latency = soc_th2_slot_pipeline_latency_get(si->dpp_clk_ratio_x10, lmode);
        reg = SLOT_PIPELINE_CONFIGr;
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, LATENCYf, latency);
        soc_reg_field_set(unit, reg, &rval, WR_ENf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }
#endif

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        SOC_IF_ERROR_RETURN(_soc_tomahawk2_port_mapping_init(unit));
    } else
#endif
    {
        SOC_IF_ERROR_RETURN(_soc_tomahawk_port_mapping_init(unit));
    }

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

    if (dev_id == BCM56968_DEVICE_ID) {
        if ((soc_property_get(unit, spn_SWITCH_BYPASS_MODE, 
            SOC_SWITCH_BYPASS_MODE_NONE)) != SOC_SWITCH_BYPASS_MODE_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Low latency and Balanced latency modes not"
                                    "supported on BCM56968 \n")));
            return SOC_E_CONFIG;
        }
        if (SOC_PBMP_NEQ(si->ether.bitmap, si->management_pbm) &&
            !SOC_PBMP_MEMBER(si->hg.bitmap, si->port_p2l_mapping[129]) &&
            !SOC_PBMP_MEMBER(si->hg.bitmap, si->port_p2l_mapping[131])) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                            "Port config error!! The MGMT/CPU ports support "
                            "Eth/HiGig encap and the General ports only "
                            "support HiGig encap on BCM56968.\n")));
            return SOC_E_CONFIG;
        }
    }

    for(port_chk = 0; port_chk < num_port; port_chk++) {
        if (si->port_speed_max[port_chk] > 0 &&
            SOC_PBMP_MEMBER(PBMP_ALL(unit), port_chk)) {
            rv = soc_th_port_speed_supported(unit, port_chk,
                                             si->port_speed_max[port_chk]);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "Unsupported port speed : port - %d, "
                                        "speed - %d\n"), port_chk,
                                        si->port_speed_max[port_chk]));
                return rv;
            }
        }
    }

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWK(unit)) {
        SOC_IF_ERROR_RETURN(soc_th_port_config_validate(unit));
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        /* Check sister ports */
        SOC_IF_ERROR_RETURN(soc_port_sister_validate(unit, _TH2_PORTS_PER_PBLK));

        /* Check if speed and encap mode matched */
        SOC_IF_ERROR_RETURN(soc_port_speed_encap_validate(unit));

        /* Check line rate bandwidth for each pipe */
        SOC_IF_ERROR_RETURN(soc_th2_max_lr_bandwidth_validate(unit));

        /* Check the speed classes */
        SOC_IF_ERROR_RETURN(soc_th2_speed_class_validate(unit));
    }
#endif /* BCM_TOMAHAWK2_SUPPORT */

    if (!SAL_BOOT_XGSSIM) {
        if (SOC_IS_TOMAHAWK(unit)) {
            if (soc->tdm_info == NULL) {
                soc->tdm_info = sal_alloc(sizeof(_soc_tomahawk_tdm_t),
                                          "Tomahakw TDM info");
                if (soc->tdm_info == NULL) {
                    return SOC_E_MEMORY;
                }
            }
            tdm = soc->tdm_info;
            /* Calculate TDM calendars */
            rv = _soc_tomahawk_tdm_calculation(unit, TRUE, tdm);
            if (SOC_FAILURE(rv)){
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "FAILED in"
                                      "_soc_tomahawk_tdm_calculation\n")));
                SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation_free_memory(unit));
                return rv;
            }
            /* Set IDB calendars */
            rv = _soc_tomahawk_tdm_idb_calendar_set(unit, 0, UPDATE_ALL_PIPES);
            if (SOC_FAILURE(rv)){
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "FAILED in"
                                      "_soc_tomahawk_tdm_idb_calendar_set\n")));
                SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation_free_memory(unit));
                return rv;
            }
            rv = _soc_tomahawk_tdm_idb_oversub_group_set(unit);
            if (SOC_FAILURE(rv)){
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "FAILED in"
                                      "_soc_tomahawk_tdm_idb_oversub_group_set\n")));
                SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation_free_memory(unit));
                return rv;
            }
            rv = _soc_tomahawk_tdm_idb_opportunistic_set(unit, TRUE);
            if (SOC_FAILURE(rv)){
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "FAILED in"
                                      "_soc_tomahawk_tdm_idb_opportunistic_set\n")));
                SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation_free_memory(unit));
                return rv;
            }
            rv = _soc_tomahawk_tdm_idb_hsp_set(unit, UPDATE_ALL_PIPES);
            if (SOC_FAILURE(rv)){
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "FAILED in"
                                      "_soc_tomahawk_tdm_idb_hsp_set\n")));
                SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation_free_memory(unit));
                return rv;
            }
            /* Set MMU calendars */
            rv = _soc_tomahawk_tdm_mmu_calendar_set(unit, 0, UPDATE_ALL_PIPES);
            if (SOC_FAILURE(rv)){
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "FAILED in"
                                      "_soc_tomahawk_tdm_mmu_calendar_set\n")));
                SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation_free_memory(unit));
                return rv;
            }
            rv = _soc_tomahawk_tdm_mmu_oversub_group_set(unit);
            if (SOC_FAILURE(rv)){
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "FAILED in"
                                      "_soc_tomahawk_tdm_mmu_oversub_group_set\n")));
                SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation_free_memory(unit));
                return rv;
            }
            SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation_free_memory(unit));
            rv = _soc_tomahawk_tdm_mmu_opportunistic_set(unit, TRUE);
            if (SOC_FAILURE(rv)){
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "FAILED in"
                                      "_soc_tomahawk_tdm_mmu_opportunistic_set\n")));
                return rv;
            }
            rv = _soc_tomahawk_tdm_mmu_hsp_set(unit, UPDATE_ALL_PIPES);
            if (SOC_FAILURE(rv)){
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "FAILED in"
                                      "_soc_tomahawk_tdm_mmu_hsp_set\n")));
                return rv;
            }
            /* Initialize IDB */
            rv = _soc_tomahawk_idb_init(unit);
            if (SOC_FAILURE(rv)){
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "FAILED in"
                                      "_soc_tomahawk_idb_init\n")));
                return rv;
            }
#ifdef BCM_TOMAHAWK2_SUPPORT
        } else if (SOC_IS_TOMAHAWK2(unit)) {
            SOC_IF_ERROR_RETURN(_soc_tomahawk2_tdm_init(unit));
            SOC_IF_ERROR_RETURN(_soc_tomahawk2_idb_init(unit));
#endif
        }
    }

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
        for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane++) {
            if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &rval,
                                  port_fields[lane], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_SOFT_RESETr(unit, port, rval));

        /* Set port mode */
        soc_tomahawk_port_ratio_get(unit, 1 /*initTime*/, clport, &mode);
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
        rval1 = 0;
        soc_reg_field_set(unit, CLPORT_FLOW_CONTROL_CONFIGr, &rval1,
                          MERGE_MODE_ENf, 1);
        for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane++) {
            lport = si->port_p2l_mapping[phy_port_base + lane];
            if (lport != -1) {
                soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &rval,
                                  port_fields[lane], 1);
                SOC_IF_ERROR_RETURN
                    (WRITE_CLPORT_FLOW_CONTROL_CONFIGr(unit, lport, rval1));
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_ENABLE_REGr(unit, port, rval));
    }
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        phy_port_mgmt0 = _TH2_PHY_PORT_MNG0;
        phy_port_mgmt1 = _TH2_PHY_PORT_MNG1;
    } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
    {
        phy_port_mgmt0 = 129;
        phy_port_mgmt1 = 131;
    }

    SOC_PBMP_ITER(si->management_pbm, port) {
        /* Assert XLPORT soft reset */
        rval = 0;
        if (si->port_p2l_mapping[phy_port_mgmt0] != -1) { /* management port 0 */
            soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &rval, PORT0f, 1);
        }
        if (si->port_p2l_mapping[phy_port_mgmt1] != -1) { /* management port 1 */
            soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &rval, PORT2f, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, rval));

        /* Set port mode based upon lanes config */
        rval = 0;
        /* Note - this is a special h/w requirement to use dual lane mode */
        fval = 3;
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, XPORT0_CORE_PORT_MODEf,
                          fval);
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, XPORT0_PHY_PORT_MODEf,
                          fval);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));

        /* De-assert XLPORT soft reset */
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, 0));

        /* Enable XLPORT */
        rval = 0;
        if (si->port_p2l_mapping[phy_port_mgmt0] != -1) { /* management port 0 */
            soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval, PORT0f, 1);
        }
        if (si->port_p2l_mapping[phy_port_mgmt1] != -1) { /* management port 1 */
            soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval, PORT2f, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_ENABLE_REGr(unit, port, rval));

        rval = 0;
        soc_reg_field_set(unit, XLPORT_FLOW_CONTROL_CONFIGr, &rval,
                          MERGE_MODE_ENf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_XLPORT_FLOW_CONTROL_CONFIGr(unit, port, rval));
    }

    SOC_IF_ERROR_RETURN(READ_MMU_GCFG_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, REFRESH_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));

    /* Enable dual hash tables */
    SOC_IF_ERROR_RETURN(_soc_tomahawk_hash_init(unit));

    /* Configure EP credit */
    freq = si->frequency;
#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (soc_feature(unit, soc_feature_egr_mmu_cell_credit_is_memory)) {
        PBMP_ALL_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(
                soc_th_port_asf_speed_to_mmu_cell_credit(unit, port, 
                    si->port_speed_max[port], &fval));
            SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ANY,
                                si->port_l2p_mapping[port], &credit_entry));
            soc_EGR_MMU_CELL_CREDITm_field32_set(unit, &credit_entry,
                                                 CREDITf, fval);
            SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ALL,
                                si->port_l2p_mapping[port], &credit_entry));
        }
    } else
#endif
    {
        reg = EGR_MMU_CELL_CREDITr;
        rval = 0;
        PBMP_ALL_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(
                soc_th_port_asf_speed_to_mmu_cell_credit(unit, port, 
                    si->port_speed_max[port], &fval));
            soc_reg_field_set(unit, reg, &rval, CREDITf, fval);
            SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CELL_CREDITr(unit, port, rval));
        }
    }

    /* Configure egress transmit start count */
    mem = EGR_XMIT_START_COUNTm;
    sal_memset(entry, 0, sizeof(egr_xmit_start_count_entry_t));
    soc_mem_field32_set(unit, EGR_XMIT_START_COUNTm, entry, THRESHOLDf, 18);
    PBMP_ALL_ITER(unit, port) {
        for (ovs_class = 0; ovs_class < 13; ovs_class++) {
            index = (port % 34) * 16 + ovs_class;
            pipe = si->port_pipe[port];
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, SOC_MEM_UNIQUE_ACC(unit, mem)[pipe],
                               MEM_BLOCK_ALL, index, entry));
        }
    
        /* Update HSP port multicast T2OQ setting, and this is not supported on TH2 */
        if (SOC_PBMP_MEMBER(si->eq_pbm, port) && !SOC_IS_TOMAHAWK2(unit)) {
            SOC_IF_ERROR_RETURN(_soc_tomahawk_mc_toq_cfg(unit, port, TRUE));
        }
    }
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_MMU_SCFG_TOQ_MC_CFG0r(unit, &rval));
    soc_reg_field_set(unit, MMU_SCFG_TOQ_MC_CFG0r, &rval,
                      MCQE_FULL_THRESHOLDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MMU_SCFG_TOQ_MC_CFG0r(unit, rval));

    /* Ingress Force SAF */
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        SOC_IF_ERROR_RETURN(_soc_tomahawk2_ing_fsaf_init(unit));
    }
#endif

    /* Egress Force SAF */
    if (SOC_PBMP_NOT_NULL(si->oversub_pbm)) {
        uint32 cd = 0, dd = si->frequency;
        int factor = SOC_IS_TOMAHAWK2(unit) ? 178125 : 118750;
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
            if (SOC_IS_TOMAHAWK2(unit)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));
                cd = 135 * si->frequency / 10;
                dd = 15 * si->frequency / 10;
                soc_reg64_field32_set(unit, EDB_1DBG_Ar, &rval64, FIELD_Af, dd);
                soc_reg64_field32_set(unit, EDB_1DBG_Ar, &rval64, FIELD_Bf, cd);
                soc_reg64_field32_set(unit, EDB_1DBG_Ar, &rval64, FIELD_Cf, 25);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, 0, rval64));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                switch (si->frequency) {
                case 850: cd = 7650;
                    break;
                case 765: cd = 6885;
                    break;
                case 672: cd = 6048;
                    break;
                case 645: cd = 5805;
                    break;
                case 545: cd = 4905;
                    break;
                default:
                    break;
                }
                soc_reg_field_set(unit, EDB_1DBG_Ar, &rval, FIELD_Af, dd);
                soc_reg_field_set(unit, EDB_1DBG_Ar, &rval, FIELD_Bf, cd);
                soc_reg_field_set(unit, EDB_1DBG_Ar, &rval, FIELD_Cf, 8);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
            }
        }
    }

    /* Enable egress */
    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        if (SOC_IS_TOMAHAWK2(unit)) {
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, 
                                   si->port_l2p_mapping[port], entry));
        } else {
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port, entry));
        }
    }

    sal_memset(entry, 0, sizeof(epc_link_bmap_entry_t));
    soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                           &PBMP_CMIC(unit));
    if (soc_mem_field_valid(unit, EPC_LINK_BMAPm, ENABLE_SOBMH_BLOCKINGf)) {
        soc_mem_field32_set(unit, EPC_LINK_BMAPm, entry, ENABLE_SOBMH_BLOCKINGf,
                            soc_property_get(unit, "epc_sobmh_block", 0));
    }
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0, entry));

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        /* enable all ports */
        ing_dest_port_enable_entry_t ingr_entry;

        sal_memset(&ingr_entry, 0, sizeof(ingr_entry));
        soc_mem_pbmp_field_set(unit, ING_DEST_PORT_ENABLEm, &ingr_entry, 
                               PORT_BITMAPf, &PBMP_ALL(unit));
        SOC_IF_ERROR_RETURN(WRITE_ING_DEST_PORT_ENABLEm(unit, MEM_BLOCK_ALL, 0,
                                                        &ingr_entry));
    }
#endif
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
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, rval64));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_CONFIG_1r, REG_PORT_ANY, RING_MODEf,
                                1));
    /* This bit controls MAC_DA format on MPB bus used for L3UC extended view packet flow */
    if (soc_property_get(unit, spn_EMBEDDED_NH_VP_SUPPORT, 0)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_CONFIG_1r, REG_PORT_ANY,
                                    MACDA_OUI_PROFILE_ENABLEf, 1));
    }

    /* The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    rval = 0;
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, VT_MISS_UNTAGf, 0);

    /* Enable pri/cfi remarking on egress ports. */
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, REMARK_OUTER_DOT1Pf,
                      1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, rval));
    }

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
                              soc_mem_index_count(unit, L2MCm) / 2),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                              soc_mem_index_count(unit, L2MCm) / 2)));
    SOC_IF_ERROR_RETURN
        (soc_hbx_mcast_size_set(unit, soc_property_get(unit,
             spn_MULTICAST_L2_RANGE, soc_mem_index_count(unit, L2MCm) / 2)));
    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_set(unit, soc_property_get(unit,
             spn_MULTICAST_L3_RANGE, soc_mem_index_count(unit, L2MCm) / 2)));

#if 0
    
    if (SAL_BOOT_QUICKTURN && SOC_IS_TOMAHAWK(unit)) {
        PBMP_PORT_ITER(unit, port) {
            if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_tomahawk_port_speed_update(unit, port,
                                                si->port_speed_max[port]));
        }
    }
#endif

    freq = si->frequency;
    /*
     * Set external MDIO freq to around 2MHz
     * target_freq = core_clock_freq * DIVIDEND / DIVISOR / 2
     */
    target_freq = 2;
    divisor = (freq + (target_freq * 2 - 1)) / (target_freq * 2);
    divisor = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVISOR, divisor);
    dividend = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVIDEND, 1);
    rval = 0;

    if (soc_feature(unit, soc_feature_cmicm)) {
        soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVISORf, divisor);
        soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVIDENDf, dividend);
        WRITE_CMIC_RATE_ADJUSTr(unit, rval);

        /*
         * Set internal MDIO freq to around 12MHz
         * Valid range is from 2.5MHz to 20MHz
         * target_freq = core_clock_freq * DIVIDEND / DIVISOR / 2
         * or
         * DIVISOR = core_clock_freq * DIVIDENT / (target_freq * 2)
         */
        target_freq = 12;
        divisor = (freq + (target_freq * 2 - 1)) / (target_freq * 2);
        divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR, divisor);
        dividend = soc_property_get(unit, spn_RATE_INT_MDIO_DIVIDEND, 1);
        rval = 0;
        soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVISORf,
                           divisor);
        soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVIDENDf,
                           dividend);
        WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval);

        delay = soc_property_get(unit, spn_MDIO_OUTPUT_DELAY, 10);
        if (delay >= 1  && delay <= 15) {
            READ_CMIC_MIIM_CONFIGr(unit, &rval);
            soc_reg_field_set(unit, CMIC_MIIM_CONFIGr, &rval, MDIO_OUT_DELAYf,
                              delay);
            WRITE_CMIC_MIIM_CONFIGr(unit, rval);
        }

        if (!SOC_WARM_BOOT(unit)) {
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (SOC_IS_TOMAHAWK2(unit)) {
                SOC_IF_ERROR_RETURN(soc_th2_ledup_init(unit));
            } else
#endif
            {
                SOC_IF_ERROR_RETURN(_soc_th_ledup_init(unit));
            }
        }
    }
    /*FIXME:
    if (soc_mspi_init(unit) != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "unit %d : MSPI Init Failed\n"), unit));
    }*/
    _phy_tsce_firmware_set_helper[unit] = _soc_tomahawk_tscx_firmware_set;
    _phy_tscf_firmware_set_helper[unit] = _soc_tomahawk_tscx_firmware_set;

    rval = 0;
    soc_reg32_get(unit, MMU_SCFG_TOQ_MC_CFG2r, REG_PORT_ANY, 0, &rval);
    soc_reg_field_set(unit, MMU_SCFG_TOQ_MC_CFG2r, &rval,
                      EPRG_KILL_TIMEOUTf, 512);
    SOC_IF_ERROR_RETURN
        (soc_tomahawk_sc_reg32_set(unit, MMU_SCFG_TOQ_MC_CFG2r,
                                   -1, -1, 0, rval));

    SOC_IF_ERROR_RETURN
        (soc_counter_tomahawk_status_enable(unit, TRUE));

    /* Check if Hierarchical ECMP mode is set */
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_ECMP_CONFIGr(unit, &rval));
    /*
     * If ecmp mode is Fast ECMP (low latency), ecmp mode cannot be changed
     * to Hierarchical or Single
     */
    if (rval != 0) {
        if (soc_property_get(unit, spn_L3_ECMP_LEVELS, 1) == 2) {
            ecmp_mode = 3;
            soc_reg_field_set(unit, ECMP_CONFIGr, &rval, ECMP_MODEf, ecmp_mode);
            SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIGr(unit, rval));
        }
    }

    /* Populate and enable RTAG7 Macro flow offset table */
    if (soc_mem_is_valid(unit, RTAG7_FLOW_BASED_HASHm)) {
        count = soc_mem_index_max(unit, RTAG7_FLOW_BASED_HASHm);
        sal_memset(entry, 0, sizeof(rtag7_flow_based_hash_entry_t));
        for (index = 0; index < count; ) {
            for (sub_sel = 0; sub_sel < 8 && index < count; sub_sel++) {
                for (offset = 0;
                     offset < rtag7_field_width[sub_sel] && index < count;
                     offset++) {
                    if (ecmp_mode == 3) {
                        soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                            SUB_SEL_ECMP_LEVEL2f, sub_sel);
                        soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                            OFFSET_ECMP_LEVEL2f, offset);
                    }
                    soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                        SUB_SEL_ECMPf, sub_sel);
                    soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                        OFFSET_ECMPf, offset);
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, RTAG7_FLOW_BASED_HASHm,
                                       MEM_BLOCK_ANY, index, &entry));
                    index++;
                }
            }
        }
        rval = 0;
        soc_reg_field_set(unit, RTAG7_HASH_SELr, &rval, USE_FLOW_SEL_ECMPf, 1);
        soc_reg_field_set(unit, RTAG7_HASH_SELr, &rval,
                                                USE_FLOW_SEL_TRILL_ECMPf, 1);
        SOC_IF_ERROR_RETURN(WRITE_RTAG7_HASH_SELr(unit, rval));
    }
#ifdef INCLUDE_AVS
    /* Register tomahawk related functions */
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        soc_th2_avs_init(unit);
    } else
#endif /* BCM_TOMAHAWK2_SUPPORT  */
    {
        soc_th_avs_init(unit);
    }
#endif /* INCLUDE_AVS  */

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        /* Add board properity which can be used in rc.soc */
        board_name = soc_property_get_str(unit, spn_BOARD_NAME);
        if (board_name) {
            if (soc_mem_config_set == NULL) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit, "unit %d : soc_mem_config_set() is NULL\n"),
                      unit));
            } else {
                if (sal_strcmp(board_name, "BCM956970K64") == 0) {
                    soc_mem_config_set("bcm956970k64", "1");
                } else if (sal_strcmp(board_name, "BCM956970K32") == 0) {
                    soc_mem_config_set("bcm956970k32", "1");
                } else if (sal_strcmp(board_name, "BCM956970S") == 0) {
                    soc_mem_config_set("bcm956970s", "1");
                }
            }
        }
    }
#endif

    /* Mem Scan thread start should be the last step in Misc init */
    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));
        SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_start(unit));
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MISC Init completed (u=%d)\n"), unit));
    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}

/*
 * Function used for index calculation of certain memories,
 * where index is derived from more than one key.
 * Eg. (Port, SP) or (Port, PG)
 * Defined as Macro: SOC_TH_MMU_PIPED_MEM_INDEX
 * Note: If no special indexig required, return the same index.
 */
uint32 _soc_th_piped_mem_index(int unit, soc_port_t port,
                               soc_mem_t mem, int arr_off)
{
    int     mmu_port, index;

    mmu_port = SOC_TH_MMU_PORT(unit, port);

    switch (mem) {
        case MMU_THDM_DB_PORTSP_CONFIGm:
        case MMU_THDM_MCQE_PORTSP_CONFIGm:
            index = (_TH_MMU_PORTS_PER_PIPE * arr_off) + (mmu_port & 0x3f);
            break;
        case THDI_PORT_SP_CONFIGm:
        case MMU_THDU_CONFIG_PORTm:
        case MMU_THDU_RESUME_PORTm:
        case MMU_THDU_COUNTER_PORTm:
            index = ((mmu_port & 0x3f) * _TH_MMU_NUM_POOL) + arr_off;
            break;
        case THDI_PORT_PG_CONFIGm:
            index = ((mmu_port & 0x3f) * _TH_MMU_NUM_PG) + arr_off;
            break;
        default:
            return arr_off;
    }

    return index;
}

STATIC void
_soc_th_mmu_init_dev_config(int unit, _soc_mmu_device_info_t *devcfg,
                            int lossless)
{
#define _TH_TDM_MCQE_RSVD_OVERSHOOT 12
#define _TH_MCQE_RSVD_PER_MCQ 6
    uint32 total_rsvd_mcq = 0;
    int port;
    soc_info_t *si = &SOC_INFO(unit);;
    sal_memset(devcfg, 0, sizeof(_soc_mmu_device_info_t));

    devcfg->max_pkt_byte = _TH_MMU_MAX_PACKET_BYTES;
    devcfg->mmu_hdr_byte = _TH_MMU_PACKET_HEADER_BYTES;
    devcfg->jumbo_pkt_size = _TH_MMU_JUMBO_FRAME_BYTES;
    devcfg->default_mtu_size = _TH_MMU_DEFAULT_MTU_BYTES;
    devcfg->mmu_cell_size = _TH_MMU_BYTES_PER_CELL;
    devcfg->mmu_total_cell = _TH_MMU_TOTAL_CELLS_PER_XPE;
#ifdef  BCM_TOMAHAWKPLUS_SUPPORT
    if (SOC_IS_TOMAHAWKPLUS(unit)) {
        devcfg->mmu_total_cell = _THP_MMU_TOTAL_CELLS_PER_XPE;
    }
#endif
    devcfg->num_pg = _TH_MMU_NUM_PG;
    devcfg->num_service_pool = _TH_MMU_NUM_POOL;
    devcfg->flags = SOC_MMU_CFG_F_PORT_MIN | SOC_MMU_CFG_F_PORT_POOL_MIN |
                    SOC_MMU_CFG_F_RQE | SOC_MMU_CFG_F_EGR_MCQ_ENTRY;
    devcfg->total_mcq_entry = _TH_MMU_TOTAL_MCQ_ENTRY(unit);
    /* Device reservation for MCQ Entry - 6 cells for each MC queue */
    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        total_rsvd_mcq += (si->port_num_cosq[port] * _TH_MCQE_RSVD_PER_MCQ);
    }
    total_rsvd_mcq >>= 1; /* Converting it to per XPE */
    total_rsvd_mcq += ((SOC_TH_NUM_CPU_QUEUES * _TH_MCQE_RSVD_PER_MCQ) +
                       _TH_TDM_MCQE_RSVD_OVERSHOOT);
    devcfg->total_mcq_entry -= total_rsvd_mcq;
    devcfg->total_rqe_queue_entry = _TH_MMU_TOTAL_RQE_ENTRY(unit) -
                                    SOC_TH_MMU_RQE_ENTRY_RSVD_PER_XPE;
    devcfg->rqe_queue_num = _TH_MMU_NUM_RQE_QUEUES;
}

STATIC int
_soc_th_default_pg_headroom(int unit, soc_port_t port,
                            int lossless)
{
    int speed = 1000, hdrm = 0;
    uint8 port_oversub = 0;

    if (IS_CPU_PORT(unit, port)) {
        return 77;
    } else if (!lossless) {
        return 0;
    } else if (SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port)) {
        return 166;
    } else if (IS_LB_PORT(unit, port)) {
        return 150;
    }

    speed = SOC_INFO(unit).port_init_speed[port] ?
            SOC_INFO(unit).port_init_speed[port] :
            SOC_INFO(unit).port_speed_max[port];

    if (SOC_PBMP_NOT_NULL(PBMP_OVERSUB(unit))) {
        port_oversub = 1;
    }

    if ((speed >= 1000) && (speed < 20000)) {
        hdrm = port_oversub ? 189 : 166;
    } else if ((speed >= 20000) && (speed <= 30000)) {
        hdrm = port_oversub ? 217 : 194;
    } else if ((speed >= 30000) && (speed <= 42000)) {
        hdrm = port_oversub ? 340 : 286;
    } else if ((speed >= 50000) && (speed < 100000)) {
        hdrm = port_oversub ? 358 : 304;
    } else if (speed >= 100000) {
        hdrm = port_oversub ? 648 : 519;
    } else {
        hdrm = port_oversub ? 189 : 166;
    }
    return hdrm;
}

/* Function to get the number of ports present in a given Port Macro */
int
soc_th_ports_per_pm_get(int unit, int pm_id)
{
    soc_info_t *si;
    int phy_port_base, num_ports = 0;

    si = &SOC_INFO(unit);
    if (pm_id >= _TH_PBLKS_PER_DEV) {
       return SOC_E_PARAM;
    }
    phy_port_base = 1 + (pm_id * _TH_PORTS_PER_PBLK);

    if (si->port_p2l_mapping[phy_port_base] != -1) {
        num_ports = 1;
        if ((si->port_num_lanes[phy_port_base] == 2) &&
            (si->port_p2l_mapping[phy_port_base + 2] != -1)) {
            /* for cases when num_lanes is given in the config (40g).
             */
            num_ports = 2;
        }

        /* When phy_port_base + 1 is valid,
         *      All 4 ports of the PM(port macro) are valid.
         * When phy_port_base + 2 is valid,
         *      The PM is operating in Dual lane mode
         */
        if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
            num_ports = 4;
            if (si->port_p2l_mapping[phy_port_base + 2] == -1) {
                num_ports = 2;
            }
        } else if (si->port_p2l_mapping[phy_port_base + 2] != -1) {
            num_ports = 2;
        }
    }
    return num_ports;
}

/* Function to reserve cells for CutThru(CT) and Flex ports
 * Parameters:
 *      unit          - (IN) Unit number.
 *      pipe          - (IN) Pipe number
 *      flex          - (IN) Config has Flex ports or Not
 *      lossless      - (IN) MMU Lossless or Lossy
 *      asf_profile   - (IN) ASF profile configuration
 *      rsvd_buffer   - (OUT) Buffers reserved per resource (Ing/Egr/ASF)
 * Returns:
 *      SOC_E_xxx
 */
int
soc_th_mmu_additional_buffer_reserve(int unit, int pipe, int flex,
                                     int lossless, int asf_profile,
                                     _soc_mmu_rsvd_buffer_t *rsvd_buffer)
{
    /* array indexed by num_ports/pm 1, 2, 4 */
    int similar_speed_asf_cells_lr[] = {20, 40, 80}; /* Line Rate */
    int extreme_speed_asf_cells_lr[] = {46, 92, 184}; /* Line Rate */
    int similar_speed_asf_cells_ob[] = {48, 96, 168}; /* Oversub mode */
    int extreme_speed_asf_cells_ob[] = {56, 112, 224}; /* Oversub mode */
    int hdrm_lossless_cells[] = {648, 358, 217};
    int zero_cells[] = {0, 0, 0};
    int *rsvd_asf_cells_p, *headroom_cells_p;
    int id, min_pm_id = 0, max_pm_id = 0, oversub_mode = FALSE;
    int pg_min_cells = 0, qgrp_min_cells = 0, hdrm_cells = 0, asf_cells = 0;
    int total_reserve = 0, num_ports = 0;
    uint8 two_port_pm = FALSE, four_port_pm = FALSE;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    if ((pipe >= NUM_PIPE(unit)) || (!rsvd_buffer)) {
        return SOC_E_PARAM;
    }

    sal_memset(rsvd_buffer, 0, sizeof(_soc_mmu_rsvd_buffer_t));

    if (SOC_PBMP_NOT_NULL(si->oversub_pbm)) {
        oversub_mode = TRUE;
    }

    rsvd_asf_cells_p = zero_cells;

    if (oversub_mode == TRUE) {
        if (asf_profile == 1) {
            rsvd_asf_cells_p = similar_speed_asf_cells_ob;
        } else if (asf_profile == 2) {
            rsvd_asf_cells_p = extreme_speed_asf_cells_ob;
        }
    } else {
        if (asf_profile == 1) {
            rsvd_asf_cells_p = similar_speed_asf_cells_lr;
        } else if (asf_profile == 2) {
            rsvd_asf_cells_p = extreme_speed_asf_cells_lr;
        }
    }

    headroom_cells_p = zero_cells;
    if (flex && (lossless != 0)) {
        headroom_cells_p = hdrm_lossless_cells;
    }

    min_pm_id = pipe * _TH_PBLKS_PER_PIPE;
    max_pm_id = min_pm_id + _TH_PBLKS_PER_PIPE;

    for (id = min_pm_id; id < max_pm_id; id++) {
        num_ports = soc_th_ports_per_pm_get(unit, id);
        switch (num_ports) {
            case 1:
                hdrm_cells = headroom_cells_p[0];
                asf_cells = rsvd_asf_cells_p[0];
                break;
            case 2:
                hdrm_cells = headroom_cells_p[1];
                asf_cells = rsvd_asf_cells_p[1];
                two_port_pm = TRUE;
                break;
            case 4:
                hdrm_cells = headroom_cells_p[2];
                asf_cells = rsvd_asf_cells_p[2];
                four_port_pm = TRUE;
                break;
            default:
                continue;
        }
        if (flex) {
            
            pg_min_cells = 0; /*(lossless[unit]) ? 8 : 0; */
        }
        qgrp_min_cells = (lossless) ? 0 : 16;
        if (lossless) {
            rsvd_buffer->ing_rsvd_cells += (hdrm_cells + pg_min_cells +
                                            qgrp_min_cells) * num_ports;
        } else {
            rsvd_buffer->ing_rsvd_cells += (hdrm_cells + pg_min_cells) * num_ports;
        }
        rsvd_buffer->egr_rsvd_cells += qgrp_min_cells * num_ports;
        rsvd_buffer->asf_rsvd_cells += asf_cells;
    }

    if (asf_profile == 1) {
        /* In Similar speed CT profile, cap limits in bytes are as follows
         * 150K for 1 port/pm case
         * 200K for 2 port/pm case
         * 250K for 3/4 port/pm case
         */
        int cap_limit = 0;
        if (four_port_pm == TRUE) {
            cap_limit = 250;
        } else if (two_port_pm == TRUE) {
            cap_limit = 200;
        } else {
            cap_limit = 150;
        }
        /* calculating the cap_limit in cells */
        cap_limit = ((cap_limit * 1024) + (_TH_MMU_BYTES_PER_CELL - 1)) /
                    _TH_MMU_BYTES_PER_CELL;
        cap_limit = (cap_limit + 1) / 2; /* Each pipe belongs to 2 XPEs.
                                          * Hence half the value */
        if (rsvd_buffer->asf_rsvd_cells > cap_limit) {
            rsvd_buffer->asf_rsvd_cells = cap_limit;
        }
    }

    total_reserve = rsvd_buffer->ing_rsvd_cells;
    total_reserve += rsvd_buffer->egr_rsvd_cells;
    total_reserve += rsvd_buffer->asf_rsvd_cells;

    LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "MMU config: Cells rsvd for Pipe %d,"
                            " Flex/ASF per XPE: %d, Ing: %d,"
                            " Egr: %d, ASF: %d\n"),
                            pipe, total_reserve, rsvd_buffer->ing_rsvd_cells,
                            rsvd_buffer->egr_rsvd_cells,
                            rsvd_buffer->asf_rsvd_cells));
    return SOC_E_NONE;
}

#define DEFAULT_VALUE_MAGIC (0xFFFFFF)

STATIC void
_soc_th_mmu_config_buf_default_v5_0(int unit, _soc_mmu_cfg_buf_t *buf,
                                    _soc_mmu_device_info_t *devcfg,
                                    int lossless)
{
#define DEFAULT_PG_NUM 7  /* Default Priority Group Num */
#define DEFAULT_POOL_ID 0 /* Default Service/Shared Pool Id */
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    int max_packet_cells, default_mtu_cells;
    int port, idx;
    int total_pool_size = 0, egr_shared_total = 0;
    int in_reserved = 0;
    uint32 color_limit = 0;
    int rqe_entry_shared_total, asf_rsvd = 0, ing_rsvd = 0, egr_rsvd = 0;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    int asf_profile;

    si = &SOC_INFO(unit);

    asf_profile = soc_property_get(unit, spn_ASF_MEM_PROFILE,
                                   _SOC_TH_ASF_MEM_PROFILE_SIMILAR);

    LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "Initializing default MMU config(v5.3). (u=%d), "
                            "Lossless %s, Flex %s, ASF profile %d\n"),
                            unit, lossless ? "YES": "NO",
                            si->flex_eligible ? "YES": "NO", asf_profile));
    max_packet_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->max_pkt_byte +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

    total_pool_size = devcfg->mmu_total_cell; /* per XPE limit */


    for (idx = 0; idx < NUM_PIPE(unit); idx++) {
        soc_th_mmu_additional_buffer_reserve(unit, idx, si->flex_eligible,
                                             lossless, asf_profile,
                                             &buf->rsvd_buffers[idx]);
        if (asf_rsvd < buf->rsvd_buffers[idx].asf_rsvd_cells) {
            asf_rsvd = buf->rsvd_buffers[idx].asf_rsvd_cells;
        }
        if (egr_rsvd < buf->rsvd_buffers[idx].egr_rsvd_cells) {
            egr_rsvd = buf->rsvd_buffers[idx].egr_rsvd_cells;
        }
        if (ing_rsvd < buf->rsvd_buffers[idx].ing_rsvd_cells) {
            ing_rsvd = buf->rsvd_buffers[idx].ing_rsvd_cells;
        }
    }

    buf->headroom = 2 * max_packet_cells;

    in_reserved += buf->headroom;

    egr_shared_total = total_pool_size - ((lossless) ? 88 : 0) -
                       ((asf_rsvd + egr_rsvd) << 1);
    LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "MMU config: Total Shared size: %d\n"),
                            egr_shared_total));

    rqe_entry_shared_total = devcfg->total_rqe_queue_entry - 88;

    for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];

        if (idx == 0) {  /* 100% scale up by 100 */
            buf_pool->size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->yellow_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->red_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->total_mcq_entry = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->total_rqe_entry = rqe_entry_shared_total;
        } else {
            buf_pool->size = 0;
            buf_pool->yellow_size = 0;
            buf_pool->red_size = 0;
            buf_pool->total_mcq_entry = 0;
            buf_pool->total_rqe_entry = 0;
        }
    }

    color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;
    for (idx = 0; idx < SOC_TH_MMU_CFG_QGROUP_MAX; idx++) {
        /* Default Settings: QGRP min will be used by Lossy Queues only */
        queue_grp = &buf->qgroups[idx];
        queue_grp->guarantee = 0;
        queue_grp->pool_resume = 16;
        queue_grp->yellow_resume = 16;
        queue_grp->red_resume = 16;
        if (lossless) {
            queue_grp->pool_scale = -1;
            queue_grp->guarantee = 0;
            queue_grp->pool_limit = 0;
            queue_grp->discard_enable = 1;
            queue_grp->red_limit = 0;
            queue_grp->yellow_limit = 0;
        } else {
        /* resume limits - accounted for 8 cell granularity */
            queue_grp->guarantee = 16;
            queue_grp->pool_limit = 0;
            queue_grp->discard_enable = 1;
            queue_grp->pool_scale = 8;
            queue_grp->red_limit = color_limit;
            queue_grp->yellow_limit = color_limit;
        }
    }

    PBMP_ALL_ITER(unit, port) {
        if (port >= SOC_MMU_CFG_PORT_MAX) {
            break;
        }
        buf_port = &buf->ports[port];

        /* internal priority to priority group mapping */
        for (idx = 0; idx < _TH_MMU_NUM_INT_PRI; idx++) {
            buf_port->pri_to_prigroup[idx] = DEFAULT_PG_NUM;
        }

        /* priority group to pool mapping */
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
            buf_port->prigroups[idx].pool_idx = DEFAULT_POOL_ID;
        }

        for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            buf_port_pool->guarantee = 0;
            buf_port_pool->pool_limit = 0;
            buf_port_pool->pool_resume = 0;
            if (idx == DEFAULT_POOL_ID) {
                buf_port_pool->pool_limit = total_pool_size;
                buf_port_pool->pool_resume = total_pool_size - 16;
            }
            in_reserved += buf_port_pool->guarantee;
        }

        buf_port->pkt_size = max_packet_cells;

        /* priority group */
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            buf_prigroup->guarantee = 0;
            buf_prigroup->user_delay = -1;
            buf_prigroup->switch_delay = -1;
            buf_prigroup->pkt_size = max_packet_cells;
            buf_prigroup->device_headroom_enable = 0;
            buf_prigroup->pool_limit = 0;
            buf_prigroup->pool_floor = 0;
            buf_prigroup->pool_scale = -1;
            buf_prigroup->headroom = 0;
            buf_prigroup->pool_resume = 0;
            buf_prigroup->flow_control_enable = 0;
            if (idx == DEFAULT_PG_NUM) {
                buf_prigroup->device_headroom_enable = 1;
                buf_prigroup->flow_control_enable = lossless;
                /* Init pool_limit to DEFAULT_VALUE_MAGIC to check if user
                   have pool_limit setting in config file. */
                buf_prigroup->pool_limit = DEFAULT_VALUE_MAGIC;
                if (lossless) {
                    buf_prigroup->guarantee = default_mtu_cells;
                    buf_prigroup->headroom =
                        _soc_th_default_pg_headroom(unit, port, lossless);
                    buf_prigroup->pool_scale = 8;
                } else if (IS_CPU_PORT(unit, port)) {
                    buf_prigroup->headroom =
                        _soc_th_default_pg_headroom(unit, port, lossless);
                }
            }
        }

        /* multicast queue */
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];

            buf_queue->qgroup_id = -1;
            buf_queue->mcq_entry_guarantee = 4;
            color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG
                            | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;;
            if (lossless) {
                buf_queue->guarantee = 0;
                buf_queue->discard_enable = 0;
                buf_queue->pool_scale = -1;
                buf_queue->pool_limit = egr_shared_total;
                buf_queue->yellow_limit = egr_shared_total;
                buf_queue->red_limit = egr_shared_total;
                buf_queue->color_discard_enable = 0;
                buf_queue->pool_resume = 16;
            } else {
                if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
                    buf_queue->guarantee = 8;
                } else {
                    buf_queue->guarantee = 0;
                }
                buf_queue->discard_enable = 1;
                buf_queue->pool_scale = 8;
                buf_queue->pool_limit = 0;
                buf_queue->yellow_limit = color_limit;
                buf_queue->red_limit = color_limit;
                buf_queue->color_discard_enable = 0;
                /* pool_resume - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
            }
        }

        /* unicast queue */
        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];

            buf_queue->qgroup_id = -1;
            color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG
                            | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
            if (lossless) {
                buf_queue->guarantee = 0;
                buf_queue->discard_enable = 0;
                buf_queue->pool_scale = -1;
                buf_queue->pool_limit = egr_shared_total;
                buf_queue->red_limit = egr_shared_total;
                buf_queue->yellow_limit = egr_shared_total;
                buf_queue->color_discard_enable = 0;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;
            } else {
                buf_queue->guarantee = 0;
                buf_queue->discard_enable = 1;
                buf_queue->pool_scale = 8;
                buf_queue->pool_limit = 0;
                buf_queue->red_limit = color_limit;
                buf_queue->yellow_limit = color_limit;
                buf_queue->color_discard_enable = 0;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;

                /* Enable QGROUP in LOSSY MODE */
                buf_queue->qgroup_id = 0;
                buf_queue->qgroup_min_enable = 1;
            }
        }

        /* queue to pool mapping */
        for (idx = 0;
             idx < si->port_num_cosq[port] + si->port_num_uc_cosq[port]; idx++) {
            buf_port->queues[idx].pool_idx = DEFAULT_POOL_ID;
        }
    }

    /* RQE */
    for (idx = 0; idx < _TH_MMU_NUM_RQE_QUEUES; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];
        buf_rqe_queue->pool_idx = DEFAULT_POOL_ID;
        buf_rqe_queue->guarantee = 8;
        color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;
        if (lossless) {
            buf_rqe_queue->discard_enable = 0;
            buf_rqe_queue->pool_scale = -1;
            buf_rqe_queue->pool_limit = egr_shared_total;
            buf_rqe_queue->red_limit = egr_shared_total;
            buf_rqe_queue->yellow_limit = egr_shared_total;
        } else {
            buf_rqe_queue->discard_enable = 1;
            buf_rqe_queue->pool_scale = 8;
            buf_rqe_queue->pool_limit = 0;
            buf_rqe_queue->red_limit = color_limit;
            buf_rqe_queue->yellow_limit = color_limit;
        }
    }
}
STATIC int
_soc_th_pool_scale_to_limit(int size, int scale)
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
_soc_th_mmu_config_buf_set_hw(int unit, _soc_mmu_cfg_buf_t *buf,
                              _soc_mmu_device_info_t *devcfg, int lossless)
{
#define DEFAULT_PG_NUM 7  /* Default Priority Group Num */
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    mmu_thdo_config_qgroup_entry_t cfg_qgrp;
    mmu_thdo_offset_qgroup_entry_t offset_qgrp;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    thdi_port_sp_config_entry_t thdi_sp_config;
    thdi_port_pg_config_entry_t pg_config_mem;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    uint32 entry0[SOC_MAX_MEM_WORDS], entry1[SOC_MAX_MEM_WORDS];
    uint32 ent_sz = sizeof(uint32) * SOC_MAX_MEM_WORDS;
    soc_reg_t reg = INVALIDr;
    soc_mem_t mem0, mem1, mem2, mem3;
    uint32 fval, rval, rval2, rval3;
    int base, numq, qbase, rqlen;
    int port, idx, midx, pval, pri, index1;
    int jumbo_frame_cells, default_mtu_cells;
    int limit, ing_limit = 0, egr_limit, rlimit;
    int pipe;
    uint32 mgmt_hdrm, mgmt_pg_min, lb_hdrm, cpu_hdrm;
    uint32 mgmt_q_min, lb_q_min, cpu_q_min, cpu_pg_min;
    uint32 total_mcqe_rsvd, cpu_mcqe_rsvd;
    uint32 rqe_rsvd = _TH_MMU_NUM_RQE_QUEUES * 8;
    static const soc_mem_t mmu_thdi_port_mem[] = {
        THDI_PORT_SP_CONFIGm,
        THDI_PORT_PG_CONFIGm
    };
    static const soc_mem_t mmu_thdo_q_uc_mem[] = {
        MMU_THDU_CONFIG_QUEUEm,
        MMU_THDU_OFFSET_QUEUEm,
        MMU_THDU_Q_TO_QGRP_MAPm
    };
    static const soc_mem_t mmu_thdo_qgrp_uc_mem[] = {
        MMU_THDU_CONFIG_QGROUPm,
        MMU_THDU_OFFSET_QGROUPm
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
    int pool_resume = 0;
    uint16      dev_id;
    uint8       rev_id;
    int         ing_rsvd = 0, egr_rsvd = 0, asf_rsvd = 0;
    int         pool_pg_headroom = 0, queue_guarantee = 0;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    si = &SOC_INFO(unit);
    jumbo_frame_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->jumbo_pkt_size +
                                                    devcfg->mmu_hdr_byte,
                                                    devcfg->mmu_cell_size);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    pool_resume = 2 * jumbo_frame_cells;

    rval = 0;
    fval = _TH_MMU_PHYSICAL_CELLS_PER_XPE - _TH_MMU_RSVD_CELLS_CFAP_PER_XPE;
#ifdef  BCM_TOMAHAWKPLUS_SUPPORT
    if (SOC_IS_TOMAHAWKPLUS(unit)) {
        fval = _THP_MMU_PHYSICAL_CELLS_PER_XPE - _THP_MMU_RSVD_CELLS_CFAP_PER_XPE;
    }
#endif
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDSETr, &rval, CFAPFULLSETPOINTf,
                      fval);
    SOC_IF_ERROR_RETURN(
        soc_tomahawk_xpe_reg32_set(unit, CFAPFULLTHRESHOLDSETr, -1, -1,
                                   -1, rval));

    rval = 0;
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDRESETr, &rval,
                      CFAPFULLRESETPOINTf, fval - (2 * jumbo_frame_cells));
    SOC_IF_ERROR_RETURN(
        soc_tomahawk_xpe_reg32_set(unit, CFAPFULLTHRESHOLDRESETr, -1, -1,
                                   -1, rval));

    rval = 0;
    soc_reg_field_set(unit, CFAPBANKFULLr, &rval, LIMITf, 767);
    for (idx = 0; idx < SOC_REG_NUMELS(unit, CFAPBANKFULLr); idx++) {
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_xpe_reg32_set(unit, CFAPBANKFULLr, -1, -1, idx, rval));
    }

    for (idx = 0; idx < NUM_PIPE(unit); idx++) {
        if (asf_rsvd < buf->rsvd_buffers[idx].asf_rsvd_cells) {
            asf_rsvd = buf->rsvd_buffers[idx].asf_rsvd_cells;
        }
        if (egr_rsvd < buf->rsvd_buffers[idx].egr_rsvd_cells) {
            egr_rsvd = buf->rsvd_buffers[idx].egr_rsvd_cells;
        }
        if (ing_rsvd < buf->rsvd_buffers[idx].ing_rsvd_cells) {
            ing_rsvd = buf->rsvd_buffers[idx].ing_rsvd_cells;
        }
    }
    /* Converting 1 pipe value to 2 pipe (1 XPE handles 2 pipes) */
    ing_rsvd <<= 1;
    egr_rsvd <<= 1;
    asf_rsvd <<= 1;
    asf_rsvd -= 1; /* To offset approximation */

    LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "MMU config set HW: Cells rsvd per XPE: %d\n"),
                            (ing_rsvd + egr_rsvd + asf_rsvd)));

    mgmt_hdrm = 0;
    mgmt_q_min = (lossless) ? 0 : 16;
    mgmt_pg_min = 0;
    SOC_PBMP_ITER(si->management_pbm, port) {
        mgmt_hdrm += _soc_th_default_pg_headroom(unit, port, lossless);
    }
    lb_hdrm = 0;
    lb_q_min = 0;
    SOC_PBMP_ITER(si->lb_pbm, port) {
        lb_hdrm += _soc_th_default_pg_headroom(unit, port, lossless);
        if (!lossless) {
            lb_q_min += (10 * 8) >> 1; /* Taking half, 2 pipe = 1 xpe */
        }
    }
    cpu_hdrm = _soc_th_default_pg_headroom(unit, CMIC_PORT(unit), lossless);
    cpu_q_min = (lossless) ? 0 : (48 * 8);
    cpu_pg_min =
        buf->ports[CMIC_PORT(unit)].prigroups[DEFAULT_PG_NUM].guarantee;
    /* internal priority to priority group mapping */
    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];

        if (SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port)) {
            buf_prigroup = &buf_port->prigroups[DEFAULT_PG_NUM];
            mgmt_pg_min += buf_prigroup->guarantee;
        }
        for (idx = 0; idx < 16; idx++) {
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
    }

    /* Input thresholds */
    rval = 0;
    soc_reg_field_set(unit, THDI_GLOBAL_HDRM_LIMITr,
                      &rval, GLOBAL_HDRM_LIMITf, buf->headroom / 2);
    /* Per XPE register */
    SOC_IF_ERROR_RETURN(
        soc_tomahawk_xpe_reg32_set(unit, THDI_GLOBAL_HDRM_LIMITr,
                                   -1, -1, 0, rval));

    for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if (((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0)
            || (buf_pool->prigroup_headroom == 0)) {
            continue;
        }

        if (FALSE == si->flex_eligible) {
            int temp_headroom = 0;
            if (lossless) {
                /* Calculate PG headroom for Layer A. CPU, No Mgmt*/
                pool_pg_headroom = ((buf_pool->prigroup_headroom -
                                    (cpu_hdrm + mgmt_hdrm)) >> 1) + cpu_hdrm;
                /* Calculate PG headroom for Layer B. No CPU, Mgmt */
                temp_headroom = ((buf_pool->prigroup_headroom -
                                 (cpu_hdrm + mgmt_hdrm)) >> 1) + mgmt_hdrm;
                if (temp_headroom > pool_pg_headroom) {
                    pool_pg_headroom = temp_headroom;
                }
            } else {
                pool_pg_headroom = cpu_hdrm;
            }
        } else {
            int temp_headroom = 0;
            /* Calculate PG headroom for Layer A. CPU, No Mgmt */
            pool_pg_headroom = ing_rsvd + (lb_hdrm >> 1) + cpu_hdrm;

            /* Calculate PG headroom for Layer B. No CPU, Mgmt */
            temp_headroom = ing_rsvd + (lb_hdrm >> 1) + mgmt_hdrm;
            if (temp_headroom > pool_pg_headroom) {
                pool_pg_headroom = temp_headroom;
            }
        }

        limit = buf_pool->total -
            (((buf_pool->prigroup_guarantee - mgmt_pg_min - cpu_pg_min) >> 1) +
             mgmt_pg_min + asf_rsvd + pool_pg_headroom + buf->headroom) -
            (lossless ? (rqe_rsvd + cpu_q_min + mgmt_q_min) : 0);
        ing_limit = limit;

        if (limit > 0) {
            rval = 0;
            soc_reg_field_set(unit, THDI_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
                              limit);
            SOC_IF_ERROR_RETURN(
                soc_tomahawk_xpe_reg32_set(unit, THDI_BUFFER_CELL_LIMIT_SPr,
                                           -1, -1, idx, rval));
        }

        rval = 0;
        soc_reg_field_set(unit, THDI_CELL_RESET_LIMIT_OFFSET_SPr, &rval,
                          OFFSETf, pool_resume);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_xpe_reg32_set(unit, THDI_CELL_RESET_LIMIT_OFFSET_SPr,
                                       -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, THDI_HDRM_BUFFER_CELL_LIMIT_HPr, &rval,
                          LIMITf, pool_pg_headroom);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_xpe_reg32_set(unit, THDI_HDRM_BUFFER_CELL_LIMIT_HPr,
                                       -1, -1, idx, rval));
    }
    rval = 0;
    SOC_IF_ERROR_RETURN(
        soc_tomahawk_xpe_reg32_set(unit, THDI_BUFFER_CELL_LIMIT_PUBLIC_POOLr,
                                   -1, -1, 0, rval));

    /* Input port per port settings */
    PBMP_ALL_ITER(unit, port) {
        pipe = si->port_pipe[port];
        buf_port = &buf->ports[port];

        rval = 0;
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
            soc_reg_field_set(unit, THDI_PORT_PG_SPIDr, &rval,
                              prigroup_spid_field[idx],
                              buf_port->prigroups[idx].pool_idx);
        }
        SOC_IF_ERROR_RETURN(WRITE_THDI_PORT_PG_SPIDr(unit, port, rval));

        /* Per port per pool settings */
        mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdi_port_mem[0])[pipe];
        if (mem0 == INVALIDm) {
            continue;
        }
        for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            midx = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port, mmu_thdi_port_mem[0],
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

        fval = 0;
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
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
        if (mem1 == INVALIDm) {
            continue;
        }
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++)
        {
            buf_prigroup = &buf->ports[port].prigroups[idx];

            midx = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port, mmu_thdi_port_mem[1],
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
                if (buf_prigroup->pool_limit == DEFAULT_VALUE_MAGIC) {
                    soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                        PG_SHARED_LIMITf,
                                        ing_limit);
                } else {
                    soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                        PG_SHARED_LIMITf,
                                        buf_prigroup->pool_limit);
                }
            }

            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_GBL_HDRM_ENf,
                                buf_prigroup->device_headroom_enable);
            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_HDRM_LIMITf, buf_prigroup->headroom);

            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_RESET_OFFSETf, buf_prigroup->pool_resume / 8);

            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_RESET_FLOORf, buf_prigroup->pool_floor);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1,
                                              MEM_BLOCK_ALL, midx,
                                              &pg_config_mem));
        }
    }


    /***********************************
     * THDO
 */
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

    /* Output port per port per queue setting for regular multicast queue */
    cpu_mcqe_rsvd = 0;
    total_mcqe_rsvd = 0;
    egr_limit = 0;

    port = CMIC_PORT(unit);
    numq = si->port_num_cosq[port];
    for (idx = 0; idx < numq; idx++) {
        buf_queue = &buf->ports[port].queues[idx];
        cpu_mcqe_rsvd += buf_queue->mcq_entry_guarantee;
    }

    /* per service pool settings */
    for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        queue_guarantee = egr_rsvd;
        if (lossless == 0) {
            queue_guarantee += (lb_q_min + cpu_q_min + mgmt_q_min);
        }
        limit = buf_pool->total - (queue_guarantee + asf_rsvd + rqe_rsvd);

        if (limit <= 0) {
            limit = 0;
        }
        egr_limit = limit;

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_SHARED_LIMITr, &rval,
                          SHARED_LIMITf, limit);
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_DB_POOL_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr,
                          &rval, YELLOW_SHARED_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RED_SHARED_LIMITr,
                          &rval, RED_SHARED_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_DB_POOL_RED_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RESUME_LIMITr,
                          &rval, RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_DB_POOL_RESUME_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr,
                          &rval, YELLOW_RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RED_RESUME_LIMITr,
                          &rval, RED_RESUME_LIMITf, sal_ceil_func(limit, 8));
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_DB_POOL_RED_RESUME_LIMITr, -1, -1, idx, rval));

        /* mcq entries */
        total_mcqe_rsvd = ((buf_pool->mcq_entry_reserved -
                           cpu_mcqe_rsvd) >> 1) + cpu_mcqe_rsvd;
        limit = buf_pool->total_mcq_entry - total_mcqe_rsvd;
        if (limit <= 0) {
            continue;
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_SHARED_LIMITr,
                          &rval, SHARED_LIMITf, limit/4);
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_MCQE_POOL_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr,
                          &rval, YELLOW_SHARED_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr,
                          &rval, RED_SHARED_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RESUME_LIMITr,
                          &rval, RESUME_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_MCQE_POOL_RESUME_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr,
                          &rval, YELLOW_RESUME_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr,
                                       &rval, RED_RESUME_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr, -1, -1, idx, rval));
    }


    PBMP_ALL_ITER(unit, port) {
        pipe = si->port_pipe[port];
        numq = si->port_num_cosq[port];
        qbase = si->port_cosq_base[port];
        if (numq == 0) {
            continue;
        }

        mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[0])[pipe];
        mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[1])[pipe];
        if ((mem0 == INVALIDm) || (mem1 == INVALIDm)) {
            continue;
        }
        base = qbase;
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[idx];
            sal_memset(entry0, 0, ent_sz);

            soc_mem_field32_set(unit, mem0, entry0,
                                Q_MIN_LIMITf, buf_queue->guarantee);
            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem0, entry0, Q_COLOR_LIMIT_ENABLEf,
                                    1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem0, entry0,
                           Q_SHARED_ALPHAf, buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_SHARED_LIMITf, egr_limit);
            }
            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_COLOR_LIMIT_DYNAMICf, 1);
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
                     sal_ceil_func(egr_limit, 8));
                soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                     sal_ceil_func(egr_limit, 8));
            }

            soc_mem_field32_set(unit, mem0, entry0,
                                Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, base + idx, entry0));

            sal_memset(entry0, 0, ent_sz);

            soc_mem_field32_set(unit, mem1, entry0, RESUME_OFFSETf, 2);
            soc_mem_field32_set(unit, mem1, entry0,
                                YELLOW_RESUME_OFFSET_PROFILE_SELf, 0);
            soc_mem_field32_set(unit, mem1, entry0,
                                RED_RESUME_OFFSET_PROFILE_SELf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, base + idx,
                               entry0));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                WRITE_MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_YELLOWr(unit, 0,
                                                                      2));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                WRITE_MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_REDr(unit, 0, 2));
        }

        mem2 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[2])[pipe];
        mem3 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[3])[pipe];
        if ((mem2 == INVALIDm) || (mem3 == INVALIDm)) {
            continue;
        }

        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[idx];
            buf_pool = &buf->pools[buf_queue->pool_idx];
            if (buf_pool->total == 0) {
                continue;
            }
            total_mcqe_rsvd = ((buf_pool->mcq_entry_reserved -
                               cpu_mcqe_rsvd) >> 1) + cpu_mcqe_rsvd;
            limit = buf_pool->total_mcq_entry - total_mcqe_rsvd;
            if (limit <= 0) {
                continue;
            }

            sal_memset(entry0, 0, ent_sz);

            soc_mem_field32_set(unit, mem2, entry0,
                                Q_MIN_LIMITf,
                                buf_queue->mcq_entry_guarantee/4);

            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0, Q_COLOR_LIMIT_ENABLEf,
                                    1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_SHARED_ALPHAf,
                                    buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_SHARED_LIMITf, limit/4);
            }
            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_COLOR_LIMIT_DYNAMICf, 1);
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
                                    limit / 8);
                soc_mem_field32_set(unit, mem2, entry0, RED_SHARED_LIMITf,
                                    limit / 8);

            }

            soc_mem_field32_set(unit, mem2, entry0,
                                Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem2, MEM_BLOCK_ALL, base + idx, entry0));

            sal_memset(entry0, 0, ent_sz);

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

        /* Per  port per pool */
        for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
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

            index1 = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port,
                                                mmu_thdo_port_mc_mem[0],
                                                idx);
            sal_memset(entry0, 0, ent_sz);

            soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMITf, limit);
            soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                                limit/8);
            soc_mem_field32_set(unit, mem0, entry0, YELLOW_SHARED_LIMITf,
                                limit/8);

            soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMIT_ENABLEf,
                                !lossless);

            soc_mem_field32_set(unit, mem0, entry0, SHARED_RESUME_LIMITf,
                                rlimit/8);
            soc_mem_field32_set(unit, mem0, entry0, YELLOW_RESUME_LIMITf,
                                rlimit/8);
            soc_mem_field32_set(unit, mem0, entry0, RED_RESUME_LIMITf,
                                rlimit/8);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem0, MEM_BLOCK_ALL,
                                index1, entry0));

            mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_mc_mem[1])[pipe];
            if (mem1 == INVALIDm) {
                continue;
            }
            buf_pool = &buf->pools[idx];
            sal_memset(entry0, 0, ent_sz);

            /* Use the Sky high value of MCQE, not the limit after reservation */
            limit = _TH_MMU_TOTAL_MCQ_ENTRY(unit);
            if (limit <= 0) {
                continue;
            }

            soc_mem_field32_set(unit, mem1, entry0, SHARED_LIMITf, (limit/4) - 1);
            soc_mem_field32_set(unit, mem1, entry0, YELLOW_SHARED_LIMITf,
                                (limit/8) - 1);
            soc_mem_field32_set(unit, mem1, entry0, RED_SHARED_LIMITf,
                                (limit/8) - 1);

            soc_mem_field32_set(unit, mem1, entry0, SHARED_LIMIT_ENABLEf,
                                !lossless);

            soc_mem_field32_set(unit, mem1, entry0, SHARED_RESUME_LIMITf,
                                (limit/8) - 2);
            soc_mem_field32_set(unit, mem1, entry0, YELLOW_RESUME_LIMITf,
                                (limit/8) - 2);
            soc_mem_field32_set(unit, mem1, entry0, RED_RESUME_LIMITf,
                                (limit/8) - 2);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL,
                                index1, entry0));
        }
    }

    /* Output port per port per queue setting for regular unicast queue */
    PBMP_ALL_ITER(unit, port) {
        pipe = si->port_pipe[port];
        /* Per  port per pool unicast */
        for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
            buf_pool = &buf->pools[idx];

            if (buf_pool->total == 0) {
                continue;
            }

            limit = buf_pool->total;

            mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_uc_mem[0])[pipe];
            mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_uc_mem[1])[pipe];
            if ((mem0 == INVALIDm) || (mem1 == INVALIDm)) {
                continue;
            }
            index1 = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port,
                                                mmu_thdo_port_uc_mem[0],
                                                idx);
            sal_memset(entry0, 0, ent_sz);
            sal_memset(entry1, 0, ent_sz);

            soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMITf, limit);
            soc_mem_field32_set(unit, mem1, entry1,
                                SHARED_RESUMEf,
                                (limit - (default_mtu_cells * 2))/8);

            soc_mem_field32_set(unit, mem0, entry0, YELLOW_LIMITf, limit/8);
            soc_mem_field32_set(unit, mem1, entry1,
                                YELLOW_RESUMEf,
                                (limit - (default_mtu_cells*2))/8);

            soc_mem_field32_set(unit, mem0, entry0, RED_LIMITf, limit/8);
            soc_mem_field32_set(unit, mem1, entry1,
                                RED_RESUMEf,
                                (limit - (default_mtu_cells * 2))/8);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem0, MEM_BLOCK_ALL,
                                index1, entry0));
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL,
                                index1, entry1));
        }

        /* per port regular unicast queue */
        numq = si->port_num_uc_cosq[port];
        qbase = si->port_uc_cosq_base[port];

        if (numq == 0) {
            continue;
        }
        base = qbase;

        mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[0])[pipe];
        mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[1])[pipe];
        mem2 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[2])[pipe];
        if ((mem0 == INVALIDm) || (mem1 == INVALIDm) || (mem2 == INVALIDm)) {
            continue;
        }
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[si->port_num_cosq[port] + idx];

            sal_memset(entry0, 0, ent_sz);
            sal_memset(entry1, 0, ent_sz);

            soc_mem_field32_set(unit, mem0, entry0,
                                Q_MIN_LIMIT_CELLf, buf_queue->guarantee);
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_DYNAMIC_CELLf,
                                    1);
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_SHARED_ALPHA_CELLf,
                                    buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_SHARED_LIMIT_CELLf, egr_limit);
            }
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_CELLf,
                                buf_queue->pool_resume / 8);

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
                                    sal_ceil_func(egr_limit, 8));
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_RED_CELLf,
                                    sal_ceil_func(egr_limit, 8));
            }
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_YELLOW_CELLf,
                                buf_queue->yellow_resume / 8);
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_RED_CELLf,
                                buf_queue->red_resume / 8);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, base + idx, entry0));

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, base + idx, entry1));

            sal_memset(entry0, 0, ent_sz);
            soc_mem_field32_set(unit, mem2, entry0,
                                Q_SPIDf, buf_queue->pool_idx);

            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_COLOR_ENABLE_CELLf, 1);
            }


            if (buf_queue->qgroup_id >= 0) {
                soc_mem_field32_set(unit, mem2, entry0, QGROUP_VALIDf, 1);
                if (buf_queue->qgroup_min_enable) {
                    soc_mem_field32_set(unit, mem2, entry0, USE_QGROUP_MINf, 1);
                }
            }

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL,
                                              base + idx, entry0));
        }

    }

    /* configure Q-groups */
    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        for (idx = 0; idx < SOC_TH_MMU_CFG_QGROUP_MAX; idx++) {
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
                                    Q_SHARED_LIMIT_CELLf, egr_limit);
            }

            if ((queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
            }

            if (queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                pval = _soc_th_pool_scale_to_limit(queue_grp->pool_limit,
                          queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_RED_CELLf, pval/8);

            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                        LIMIT_RED_CELLf, egr_limit / 8);
            }

            if (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                pval = _soc_th_pool_scale_to_limit(queue_grp->pool_limit,
                          queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_YELLOW_CELLf, pval/8);

            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_YELLOW_CELLf, egr_limit / 8);
            }

            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_CELLf,
                                queue_grp->pool_resume / 8);
            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_YELLOW_CELLf,
                                queue_grp->pool_resume / 8);
            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_RED_CELLf,
                                queue_grp->pool_resume / 8);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, idx, &cfg_qgrp));
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, idx, &offset_qgrp));
        }
    }


    /* RQE */
    for (idx = 0; idx < _TH_MMU_NUM_RQE_QUEUES; idx++) {
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
                SHARED_RED_LIMITf, buf_rqe_queue->red_limit / 8);

            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                SHARED_YELLOW_LIMITf, buf_rqe_queue->yellow_limit / 8);
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
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG_PRIQr_REG32(unit, idx, rval2));
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_THDR_DB_LIMIT_COLOR_PRIQr(unit, idx, rval3));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_REDf, 2);
        soc_reg_field_set(unit, MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_YELLOWf, 2);
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr(unit, idx, rval));

        /* queue entry */
        buf_pool = &buf->pools[buf_rqe_queue->pool_idx];
        fval = (buf_pool->total_rqe_entry + 8 * 11 - 1) / (8 * 11);
        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_MIN_PRIQr,
                          &rval, MIN_LIMITf, buf_rqe_queue->guarantee/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_LIMIT_MIN_PRIQr(unit, idx, rval));

        rval = 0;
        rval2 = 0;
        rval3 = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                          &rval, SPIDf, buf_rqe_queue->pool_idx);


        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr, &rval, LIMIT_ENABLEf,
                          (buf_rqe_queue->discard_enable ? 1 : 0));

        if (buf_rqe_queue->pool_scale != -1) {
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                              &rval, DYNAMIC_ENABLEf, 1);
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                              SHARED_ALPHAf, buf_rqe_queue->pool_scale);
            soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                              &rval3, SHARED_RED_LIMITf, 0);
            soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                              &rval3, SHARED_YELLOW_LIMITf, 0);
            soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                              &rval, COLOR_LIMIT_DYNAMICf, 1);
        } else {
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                              SHARED_LIMITf, fval);
            soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                              &rval3, SHARED_RED_LIMITf, fval);
            soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                              &rval3, SHARED_YELLOW_LIMITf, fval);
            soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                              &rval, COLOR_LIMIT_DYNAMICf, 0);
        }
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_PRIQr, &rval2,
                          RESET_OFFSETf, 1);

        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG1_PRIQr(unit, idx, rval));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG_PRIQr(unit, idx, rval2));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_LIMIT_COLOR_PRIQr(unit, idx, rval3));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_REDf, default_mtu_cells/8);
        soc_reg_field_set(unit, MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_YELLOWf, default_mtu_cells/8);
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr(unit, idx, rval));
    }

    /* per pool RQE settings */
    for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if (((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) ||
            (buf_pool->total == 0)) {
            continue;
        }

        limit = egr_limit;
        if (!lossless) {
            limit -= (buf->headroom + cpu_hdrm);
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_SPr,
                          &rval, SHARED_LIMITf, limit);
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_SPr, &rval, RESUME_LIMITf,
                          (limit - (default_mtu_cells * 2) + 7)/8);
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDR_DB_CONFIG_SPr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_SP_SHARED_LIMITr, &rval,
                          SHARED_RED_LIMITf, (limit + 7)/8);
        soc_reg_field_set(unit, MMU_THDR_DB_SP_SHARED_LIMITr, &rval,
                          SHARED_YELLOW_LIMITf, (limit+7)/8);
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDR_DB_SP_SHARED_LIMITr, -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_RED_LIMITf,
                          (limit - (default_mtu_cells * 2) + 7)/8);
        soc_reg_field_set(unit, MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_YELLOW_LIMITf,
                          (limit - (default_mtu_cells * 2) + 7)/8);
        SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
            MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, -1, -1, idx, rval));

        rqlen = buf_pool->total_rqe_entry/8;
        if (rqlen <= 0) {
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
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr(unit, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_RED_LIMITf, rqlen - 1);
        soc_reg_field_set(unit, MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_YELLOW_LIMITf, rqlen - 1);
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr(unit, idx, rval));
    }

    /* Device level config setting */
    if (soc_property_get(unit, spn_PORT_UC_MC_ACCOUNTING_COMBINE, 0)) {
        SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, MMU_THDM_DB_DEVICE_THR_CONFIGr, &rval,
                          UC_MC_PORTSP_COMB_ACCT_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, rval));
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_MMU_THDR_DB_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, MMU_THDR_DB_CONFIGr, &rval, MOP_POLICY_1Bf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIGr(unit, rval));

    return SOC_E_NONE;
}

STATIC int
soc_th_mmu_config_init(int unit, int test_only)
{
    int rv;
    int lossless = 1;
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_device_info_t devcfg;
    soc_mmu_config_buf_default_f soc_mmu_config_buf_default = NULL;

    buf = soc_mmu_cfg_alloc(unit);
    if (!buf) {
        return SOC_E_MEMORY;
    }

    soc_mmu_config_buf_default = _soc_th_mmu_config_buf_default_v5_0;

    if (SOC_IS_TOMAHAWKPLUS(unit)) {
        lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 
                                    SOC_TH_MMU_LOSSLESS_DEFAULT_DISABLE);
    } else {
        lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 
                                    SOC_TH_MMU_LOSSLESS_DEFAULT_ENABLE);
    }

    /* Update the soc_info mmu_lossless to the updated value */
    SOC_INFO(unit).mmu_lossless = lossless;

    _soc_th_mmu_init_dev_config(unit, &devcfg, lossless);
    soc_mmu_config_buf_default(unit, buf, &devcfg, lossless);

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
            soc_mmu_config_buf_default(unit, buf, &devcfg, lossless);
            SOC_IF_ERROR_RETURN
                (_soc_mmu_cfg_buf_calculate(unit, buf, &devcfg));
        }
        rv = _soc_th_mmu_config_buf_set_hw(unit, buf, &devcfg, lossless);
    }

    soc_mmu_cfg_free(unit, buf);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));
    return rv;
}

STATIC int
_soc_tomahawk_mmu_init(int unit)
{
    soc_info_t *si;
    uint32 pipe_map;
    int pipe, oversub_mode = FALSE;
    uint32 rval = 0;
    uint32 fval = 0;
    int port, phy_port, mmu_port;
    uint64 enable[_TH_PIPES_PER_DEV];
    int test_only;
#ifdef BCM_TOMAHAWK2_SUPPORT
    pbmp_t pbmp_mmu;

    SOC_PBMP_CLEAR(pbmp_mmu);
#endif

    si = &SOC_INFO(unit);

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    test_only = soc_property_get(unit, "simple_simulation_only", 
                         (SAL_BOOT_BCMSIM || SAL_BOOT_XGSSIM)) ? TRUE : FALSE;
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        SOC_IF_ERROR_RETURN(soc_th2_mmu_config_init(unit, test_only));
    } else
#endif
    {
        SOC_IF_ERROR_RETURN(soc_th_mmu_config_init(unit, test_only));
    }

    if (SOC_PBMP_NOT_NULL(si->oversub_pbm)) {
        oversub_mode = TRUE;
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_sc_reg32_get(unit, MMU_1DBG_Cr, -1, -1, 0, &rval));
        soc_reg_field_set(unit, MMU_1DBG_Cr, &rval, FIELD_Af, 1);
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_sc_reg32_set(unit, MMU_1DBG_Cr, -1, -1, 0, rval));
        rval = 0xffffffff;
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_sc_reg32_set(unit, MMU_1DBG_Ar, -1, -1, 0, rval));
    }

    sal_memset(enable, 0, sizeof(enable));
    PBMP_ALL_ITER(unit, port) {
        pipe = si->port_pipe[port];
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
#ifdef BCM_TOMAHAWK2_SUPPORT
        SOC_PBMP_PORT_ADD(pbmp_mmu, mmu_port);
#endif
        if (mmu_port & 0x20) {
            u64_H(enable[pipe]) |= 1 << (mmu_port & 0x1f);
        } else {
            u64_L(enable[pipe]) |= 1 << (mmu_port & 0x1f);
        }
        if (oversub_mode) {
            rval = 0;
            if (si->port_speed_max[port] >= 100000) {
                fval = 140;
            } else if (si->port_speed_max[port] >= 40000) {
                fval = 60;
            } else if (si->port_speed_max[port] >= 25000) {
                fval = 40;
            } else if (si->port_speed_max[port] >= 20000) {
                fval = 30;
            } else {
                fval = 15;
            }
            soc_reg_field_set(unit, MMU_3DBG_Cr, &rval, FIELD_Af,
                              fval + sal_rand() % 20);
            SOC_IF_ERROR_RETURN
                (soc_tomahawk_sc_reg32_set(unit, MMU_3DBG_Cr, -1, port, 0, rval));
        }
    }

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        /*
         * THDU_OUTPUT_PORT_RX_ENABLE_64,
         * MMU_THDM_DB_PORT_RX_ENABLE_64,
         * MMU_THDM_MCQE_PORT_RX_ENABLE_64,
         * THDI_INPUT_PORT_XON_ENABLES.INPUT_PORT_RX_ENABLE,
         * MMU RX_ENABLE registers listed above shouldn't be changed during
         * run-time. Enable RX for all MMU ports that flex eligible.
         */
        if (si->flex_eligible) {
            for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
                if (!(pipe_map & (1 << pipe))) {
                    continue;
                }
                for (mmu_port = 0; mmu_port < _TH2_MMU_GPORTS_PER_PIPE;
                     mmu_port++) {
                    u64_L(enable[pipe]) |= 1 << mmu_port;
                    SOC_PBMP_PORT_ADD(pbmp_mmu,
                        (mmu_port + pipe * _TH2_MMU_PORTS_OFFSET_PER_PIPE));
                }
            }
        }

        SOC_PBMP_ITER(pbmp_mmu, mmu_port) {
            SOC_IF_ERROR_RETURN
                (soc_reg32_rawport_get(unit, THDI_INPUT_PORT_XON_ENABLESr,
                                       mmu_port,0, &rval));
            soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                              INPUT_PORT_RX_ENABLEf, 1);
            SOC_IF_ERROR_RETURN
                (soc_reg32_rawport_set(unit, THDI_INPUT_PORT_XON_ENABLESr,
                                       mmu_port, 0, rval));
        }

        /* enable WRED refresh */
        SOC_IF_ERROR_RETURN(READ_WRED_REFRESH_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, WRED_REFRESH_CONTROLr, &rval,
                                REFRESH_DISABLEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_WRED_REFRESH_CONTROLr(unit, rval));

    }
#endif /* BCM_TOMAHAWK2_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit))
#endif
    {
        for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
            if (!(pipe_map & (1 << pipe))) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_tomahawk_xpe_reg_set(unit, THDU_OUTPUT_PORT_RX_ENABLE_64r,
                                          -1, pipe, 0, enable[pipe]));
        }

        for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
            if (!(pipe_map & (1 << pipe))) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_tomahawk_xpe_reg_set(unit, MMU_THDM_DB_PORT_RX_ENABLE_64r,
                                          -1, pipe, 0, enable[pipe]));
        }

        for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
            if (!(pipe_map & (1 << pipe))) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_tomahawk_xpe_reg_set(unit, MMU_THDM_MCQE_PORT_RX_ENABLE_64r,
                                          -1, pipe, 0, enable[pipe]));
        }
    }
    SOC_IF_ERROR_RETURN(_soc_th_post_mmu_init_update(unit));
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_age_timer_get(int unit, int *age_seconds, int *enabled)
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

STATIC int
_soc_tomahawk_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds = 0x7fffffff;
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_age_timer_set(int unit, int age_seconds, int enable)
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
_soc_tomahawk_bond_info_init(int unit)
{
    soc_chip_types type = SOC_DRIVER(unit)->type;

    switch (type) {
#ifdef BCM_TOMAHAWK2_SUPPORT
    case SOC_CHIP_BCM56970_A0:
        return soc_tomahawk2_bond_info_init(unit);
#endif
    default:
        return SOC_E_NONE;
    }
}

soc_error_t
soc_th_port_speed_supported(int unit, soc_port_t port, int speed)
{
    uint8 rev_id;
    uint16 dev_id;
    int num_lanes;
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port;

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        return soc_th2_port_speed_supported(unit, port, speed);
    }
#endif

    /*
     * If Universal calendar was not selected at init time then
     * disallow speed change to HG2 speeds.
     */
    if (FALSE == si->fabric_port_enable) {
        if (IS_HG2_SPEED(speed)) {
            /*
             * Intentionally returning SOC_E_CONFIG as this is a case
             * of incorrect configuration
             */
            return SOC_E_CONFIG;
        }
    }

    /*
     * Management ports only support 1G, 2.5G and 10G. Flexport
     * on management ports is not supported. Management ports
     * are guaranteed 10G bandwidth.
     */
    if (SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port)) {
        if ((1000 == speed) || (2500 == speed) || (10000 == speed)) {
            return SOC_E_NONE;
        } else {
            return SOC_E_PARAM;
        }
    }

    soc_cm_get_id(unit, &dev_id, &rev_id);
    num_lanes = si->port_num_lanes[port];
    phy_port = si->port_l2p_mapping[port];
    
    /*
     * Check if Core Clock Frequency supports Port Speed
     */

    switch (speed) {
    /* 1G, 10G, 20G, 40G, HG11, HG21, HG 42 */
    case 1000:
    case 10000:
    case 20000:
    case 40000:
    case 11000:
    case 21000:
    case 42000:
        break;

    /* 25G, 50G, 100G */
    case 25000:
    case 50000:
    case 100000:
    /* HG27, HG 53, HG 106 */
    case 27000:
    case 53000:
    case 106000:
        /* Universal Calendar */
        if (si->fabric_port_enable) {
            if (si->frequency < 672) {
                return SOC_E_PARAM;
            }
        /* Ethernet Optimized Calendar */
        } else {
            if (si->frequency < 645) {
                return SOC_E_PARAM;
            }
        }
        break;

    default:
       return SOC_E_PARAM; 
    }

    /*
     * Laying down the framework to handle SKU differences. This is
     * needed because some SKUs have speed caps on certain PMs.
     */
    switch (dev_id) {
    case BCM56168_DEVICE_ID:
        /*
         * The specification of FC Max lanes speed on BCM56168 as below
         *     FC[0:2] FC[3:7] FC[8:9] FC[10:15] FC[16:21] FC[22:23] FC[24:29]  FC[30:31]
         *      4x25    4x10    4x25     4x10      4x10      4x25      4x10       4x25
         */
        if (((phy_port >= 13) && (phy_port <= 32)) || \
            ((phy_port >= 41) && (phy_port <= 88)) || \
            ((phy_port >= 97) && (phy_port <= 120))) {
            if ((10000 != speed) && (11000 != speed) && \
                (20000 != speed) && (21000 != speed) && \
                (40000 != speed) && (42000 != speed))
                return SOC_E_PARAM;
        }
        break;

    case BCM56961_DEVICE_ID:
    case BCM56969_DEVICE_ID:
        /* SERDES VCO capped at 21.875Ghz */
        if ((25000 == speed) || (27000 == speed) || (speed >= 50000)) {
            return SOC_E_PARAM;
        }
        /* Fall through to default case */

    default:
        if (speed >= 100000) { /* 100G, HG106 */
            if (4 != num_lanes) {
                return SOC_E_PARAM;
            }
        } else if (speed >= 50000) { /* 50G, HG53 */
            if (2 != num_lanes) {
                return SOC_E_PARAM;
            }
        } else if (speed >= 40000) { /* 40G, HG42 */
            if ((2 != num_lanes) && (4 != num_lanes)) {
                return SOC_E_PARAM;
            }
        } else if ((speed == 20000) || ( speed == 21000)) { /* 20G, HG21 */
            if (2 != num_lanes) {
                return SOC_E_PARAM;
            }
        } else { /* 10G, HG11, 25G, HG27 */
            if (1 != num_lanes) {
                return SOC_E_PARAM;
            }
        }
    }
    return SOC_E_NONE;
}

STATIC int
soc_th_port_lanes_ctrl_info_diag(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
    soc_info_t *si = &SOC_INFO(unit);
    int port_base, phy_port_base;
    int pipe, i;

    static char *mode_name[] = {
        "QUAD", "TRI_012", "TRI_023", "DUAL", "SINGLE"
    };

    port_base = lanes_ctrl->port_base;
    phy_port_base = si->port_l2p_mapping[port_base];
    if (phy_port_base == -1) {
        return SOC_E_PORT;
    }

    pipe = si->port_pipe[port_base];

    LOG_CLI((BSL_META_U(unit,
                        ">>>> The current port lanes control info\n")));
    LOG_CLI((BSL_META_U(unit,
                        "oversub mode:%s mixed sister mode:%s mixed sister port mode:%d\n"),
             lanes_ctrl->oversub ? "enable" : "disable",
             lanes_ctrl->os_mixed_sister ? "enable" : "disable",
             lanes_ctrl->mixed_port_mode));
    LOG_CLI((BSL_META_U(unit,
                        "pipe:%d logical port:%d physical port:%d bindex:%d\n"),
             pipe, port_base, phy_port_base, lanes_ctrl->bindex));
    LOG_CLI((BSL_META_U(unit,
                        "  mode(cur:%s ==>> new:%s) lanes(cur:%d ==>> new:%d)\n"),
             mode_name[lanes_ctrl->cur_mode],
             mode_name[lanes_ctrl->mode],
             lanes_ctrl->cur_lanes, lanes_ctrl->lanes));
    LOG_CLI((BSL_META_U(unit,
                        "  speed class(cur:%d ==>> new:%d)\n"),
             lanes_ctrl->cur_ovs_class, lanes_ctrl->ovs_class));
    for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
        LOG_CLI((BSL_META_U(unit,
                            "  %s physical port:%d (logical port:%d)\n"),
                 lanes_ctrl->lanes > lanes_ctrl->cur_lanes ?
                 "del" : "add",
                 lanes_ctrl->phy_ports[i],
                 si->port_p2l_mapping[lanes_ctrl->phy_ports[i]]));
    }
    if (lanes_ctrl->idb_group != -1) {
        LOG_CLI((BSL_META_U(unit,
                            "new idb group:%d slot:%d"),
                 lanes_ctrl->idb_group, lanes_ctrl->idb_slots[0]));
        if (lanes_ctrl->lanes < lanes_ctrl->cur_lanes &&
            lanes_ctrl->idb_slots[0] != -1) {
            LOG_CLI((BSL_META_U(unit,
                                " add:")));
            for (i = 1; i < lanes_ctrl->phy_ports_len; i++) {
                LOG_CLI((BSL_META_U(unit,
                                    " slot:%d"), lanes_ctrl->idb_slots[i]));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }
    LOG_CLI((BSL_META_U(unit,
                        "cur idb group:%d slot:%d"),
             lanes_ctrl->cur_idb_group, lanes_ctrl->cur_idb_slots[0]));
    if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
        LOG_CLI((BSL_META_U(unit,
                            " del:")));
        /* port(s) to be removed */
        for (i = 1; i < lanes_ctrl->phy_ports_len; i++) {
            LOG_CLI((BSL_META_U(unit,
                                " slot:%d"), lanes_ctrl->cur_idb_slots[i]));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    if (lanes_ctrl->mmu_group != -1) {
        LOG_CLI((BSL_META_U(unit,
                            "new mmu group:%d slot:%d"),
                 lanes_ctrl->mmu_group, lanes_ctrl->mmu_slots[0]));
        if (lanes_ctrl->lanes < lanes_ctrl->cur_lanes &&
            lanes_ctrl->mmu_slots[0] != -1) {
            LOG_CLI((BSL_META_U(unit,
                                " add:")));
            /* port(s) to be added */
            for (i = 1; i < lanes_ctrl->phy_ports_len; i++) {
                LOG_CLI((BSL_META_U(unit,
                                    " slot:%d"), lanes_ctrl->mmu_slots[i]));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }
    LOG_CLI((BSL_META_U(unit,
                        "cur mmu group:%d slot:%d"),
             lanes_ctrl->cur_mmu_group, lanes_ctrl->cur_mmu_slots[0]));
    if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
            LOG_CLI((BSL_META_U(unit,
                                " del:")));
        /* port(s) to be removed */
        for (i = 1; i < lanes_ctrl->phy_ports_len; i++) {
            LOG_CLI((BSL_META_U(unit,
                                " slot:%d"), lanes_ctrl->mmu_slots[i]));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    return SOC_E_NONE;
}

STATIC int
soc_th_port_lanes_migrate(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
    soc_info_t *si;
    int port_base, phy_port, idb_port, mmu_port;
    int group, pipe, slot, id;
    soc_reg_t reg;
    uint32 rval;
    uint32 inst;

    si = &SOC_INFO(unit);
    port_base = lanes_ctrl->port_base;
    phy_port = si->port_l2p_mapping[port_base];
    pipe = si->port_pipe[port_base];
    id = si->port_serdes[port_base] & 7;

    /* Migrate only one group/slot to the destination */
    if ((-1 != lanes_ctrl->cur_idb_group) &&
        (-1 != lanes_ctrl->cur_mmu_group) &&
        (-1 != lanes_ctrl->idb_group) &&
        (-1 != lanes_ctrl->mmu_group)) {
        /* Remove idb port(s) from current group */
        group = lanes_ctrl->cur_idb_group;
        reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, 0x3f);
        slot = lanes_ctrl->cur_idb_slots[0];
        if (slot < 0) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));

        /* Add idb port(s) to new group */
        group = lanes_ctrl->idb_group;
        reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id);
        idb_port = si->port_l2i_mapping[port_base];
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, idb_port);
        slot = lanes_ctrl->idb_slots[0];
        if (slot < 0) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));

        /* Remove mmu port(s) from current group */
        group = lanes_ctrl->cur_mmu_group;
        reg = mmu_grp_tbl_regs[group];
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, 0x3f);
        slot = lanes_ctrl->cur_mmu_slots[0];
        if (slot < 0) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));

        /* Add mmu port(s) to new group */
        group = lanes_ctrl->mmu_group;
        reg = mmu_grp_tbl_regs[group];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id);
        mmu_port = si->port_p2m_mapping[phy_port];
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, mmu_port & 0x3f);
        slot = lanes_ctrl->mmu_slots[0];
        if (slot < 0) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));

    } else {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

STATIC int
soc_tomahawk_oversub_group_find(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_th_port_lanes_t re_lanes_ctrl;
    soc_reg_t reg;
    int port_base, phy_port_base, idb_port_base, mmu_port_base;
    int port, mmu_port, id, inst;
    uint32 rval, fval;
    int pipe, group, slot, i;
    int cur_ids[4], cur_ids_len;
    int ids_len;
    int cur_mmu_ids[4];
    int slots[4], slots_len;
    uint32 speed_max = 0, cur_speed_max;
    int sp_group1 = -1, sp_group2 = -1;
    int cnt_idb[_OVS_GROUP_COUNT] = {0,0,0,0,0,0};
    int cnt_mmu[_OVS_GROUP_COUNT] = {0,0,0,0,0,0};
    typedef struct {
        int group;
        int slot;
        int port;
        int free_slot;
    } cur_group_info_t;
    cur_group_info_t cur_idb[_OVS_GROUP_COUNT];
    cur_group_info_t cur_mmu[_OVS_GROUP_COUNT];

    port_base = lanes_ctrl->port_base;
    phy_port_base = si->port_l2p_mapping[port_base];
    idb_port_base = si->port_l2i_mapping[port_base];
    mmu_port_base = si->port_p2m_mapping[phy_port_base];
    pipe = si->port_pipe[port_base];
    sal_memset(cur_idb, -1, sizeof(cur_idb));
    sal_memset(cur_mmu, -1, sizeof(cur_mmu));

    /* Figure out the new speed group */
    _soc_tomahawk_flexed_port_speed_get(unit, lanes_ctrl, &speed_max);
    _soc_tomahawk_speed_to_ovs_class_mapping(unit, speed_max,
                                             &lanes_ctrl->ovs_class);

    /* Figure out the current speed group */
    cur_speed_max = si->port_init_speed[port_base];
    _soc_tomahawk_speed_to_ovs_class_mapping(unit, cur_speed_max,
                                             &lanes_ctrl->cur_ovs_class);

    /* Construct new and old idb port list for oversub group replacement */
    cur_idb[0].port = cur_ids[0] = idb_port_base;
    ids_len = cur_ids_len = 1;

    if (TH_FLEXPORT_SPEED_SET_NUM_LANES != lanes_ctrl->lanes) {
        if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
            /* idb port(s) to be removed */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
                cur_idb[1 + i].port = cur_ids[1 + i] = si->port_l2i_mapping[port];
            }
            cur_ids_len += lanes_ctrl->phy_ports_len;
        } else { /* idb port(s) to be added */
            ids_len += lanes_ctrl->phy_ports_len;
        }
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
        } else {
            if (fval == lanes_ctrl->ovs_class) {
                if (lanes_ctrl->idb_slots[0] == -1) { /* hasn't found the group */
                    /* Find slot(s) for new idb port oversub group */
                    reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
                    slots_len = 0;
                    for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
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
            }
            if (fval == lanes_ctrl->cur_ovs_class) {
                if (lanes_ctrl->cur_idb_slots[0] == -1) {
                    /* Find slot(s) used by the current idb port oversub group */
                    reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
                    slots_len = 0;
                    lanes_ctrl->cur_idb_empty = TRUE;
                    for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_get(unit, reg, REG_PORT_ANY, slot, &rval));
                        id = soc_reg_field_get(unit, reg, rval, PORT_NUMf);
                        if (id == 0x3f) {
                            continue;
                        }
                        for (i = 0; i < cur_ids_len; i++) {
                            if (id == cur_idb[i].port) {
                                cur_idb[i].group  = group;
                                cur_idb[i].slot = slot;
                            }
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
        }
        if (lanes_ctrl->idb_slots[0] != -1 &&
            lanes_ctrl->cur_idb_slots[0] != -1) {
            break;
        }
    }

    if (lanes_ctrl->idb_group == -1) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "IDB Speed Group is Full\n")));
        return SOC_E_FULL;
    }

    /* Construct new and old mmu port list for oversub group replacement */
    cur_mmu[0].port = cur_mmu_ids[0] = mmu_port_base & 0x3f;
    ids_len = cur_ids_len = 1;
    if (TH_FLEXPORT_SPEED_SET_NUM_LANES != lanes_ctrl->lanes) {
        if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
            /* mmu port(s) to be removed */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                mmu_port = si->port_p2m_mapping[lanes_ctrl->phy_ports[i]];
                cur_mmu[1 + i].port = cur_mmu_ids[1 + i] = mmu_port & 0x3f;
            }
            cur_ids_len += lanes_ctrl->phy_ports_len;
        } else { /* mmu port(s) to be added */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                mmu_port = si->port_p2m_mapping[lanes_ctrl->phy_ports[i]];
            }
            ids_len += lanes_ctrl->phy_ports_len;
        }
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
        } else {
            if (fval == lanes_ctrl->ovs_class) {
                if (lanes_ctrl->mmu_slots[0] == -1) { /* hasn't found the group */
                    /* Find slot(s) for new mmu port oversub group */
                    inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
                    reg = mmu_grp_tbl_regs[group];
                    slots_len = 0;
                    for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
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
            }
            if (fval == lanes_ctrl->cur_ovs_class) {
                if (lanes_ctrl->cur_mmu_slots[0] == -1) {
                    /* Find slot(s) used by the currnet mmu port oversub group */
                    reg = mmu_grp_tbl_regs[group];
                    inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
                    slots_len = 0;
                    lanes_ctrl->cur_mmu_empty = TRUE;
                    for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_get(unit, reg, inst, slot, &rval));
                        id = soc_reg_field_get(unit, reg, rval, PORT_NUMf);
                        if (id == 0x3f) {
                            continue;
                        }
                        for (i = 0; i < cur_ids_len; i++) {
                            if (id == cur_mmu[i].port) {
                                cur_mmu[i].group = group;
                                cur_mmu[i].slot = slot;
                            }
                            if (id == cur_mmu_ids[i]) {
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
        }
        if (lanes_ctrl->mmu_slots[0] != -1 &&
            lanes_ctrl->cur_mmu_slots[0] != -1) {
            break;
        }
    }

    if (lanes_ctrl->mmu_group == -1) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "MMU Speed Group is Full\n")));
        return SOC_E_FULL;
    }

    /* Peform validation of OS group/slots */
    if (TH_FLEXPORT_SPEED_SET_NUM_LANES != lanes_ctrl->lanes) {
        if ((-1 == lanes_ctrl->cur_idb_group) ||
            (-1 == lanes_ctrl->cur_mmu_group) ||
            (-1 == lanes_ctrl->idb_group) ||
            (-1 == lanes_ctrl->mmu_group)) {
            if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
                SOC_IF_ERROR_RETURN(soc_th_port_lanes_ctrl_info_diag(unit, lanes_ctrl));

                for (i = 0; i < _OVS_GROUP_COUNT; i++) {
                    LOG_CLI((BSL_META_U(unit,
                                       "cur_idb[%d]: %d %d %d %d\n"),
                             i, cur_idb[i].group, cur_idb[i].slot, cur_idb[i].port, cur_idb[i].free_slot));
                    LOG_CLI((BSL_META_U(unit,
                                       "cur_mmu[%d]: %d %d %d %d\n"),
                             i, cur_mmu[i].group, cur_mmu[i].slot, cur_mmu[i].port, cur_mmu[i].free_slot));
                }
            }

            /* Peform adjusting the group/slots which is splitted into different groups */
            if (lanes_ctrl->lanes == 4 && lanes_ctrl->cur_lanes == 1 &&
                lanes_ctrl->ovs_class == 4 && lanes_ctrl->cur_ovs_class == 1) {
                /* figure out the number of same group id */
                for (i = 0; i < _OVS_GROUP_COUNT; i++) {
                    cnt_idb[cur_idb[i].group]++;
                    cnt_mmu[cur_mmu[i].group]++;
                }
                /* figure out one group is different with the others */
                for (i = 0; i < _OVS_GROUP_COUNT; i++) {
                    if (cnt_idb[i] == 1) {
                        sp_group1 = i;
                    }
                    if (cnt_idb[i] == 3) {
                        sp_group2 = i;
                    }
                    if ((sp_group1 > 0) && (sp_group2 > 0))
                        break;
                }
                if (sp_group1 < 0 || sp_group2 < 0) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "FAILED in search the number of speed group\n")));
                    return SOC_E_INTERNAL;
                }

                if (lanes_ctrl->idb_slots[0] == -1) {
                    reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[sp_group2])[pipe];
                    for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_get(unit, reg, REG_PORT_ANY, slot, &rval));
                        if (soc_reg_field_get(unit, reg, rval, PORT_NUMf) != 0x3f) {
                            continue;
                        }
                        lanes_ctrl->idb_group = sp_group2;
                        lanes_ctrl->idb_slots[0] = slot;
                        break;
                    }
                    if (slot == _OVS_GROUP_TDM_LENGTH) {
                        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                    (BSL_META_U(unit,
                                                "FAILED in search the empty speed group slot\n")));
                        return SOC_E_INTERNAL;
                    }
                }
                if (lanes_ctrl->cur_idb_slots[0] == -1) {
                    for (i = 0; i < _OVS_GROUP_COUNT; i++) {
                        if(cur_idb[i].group == sp_group1) {
                            lanes_ctrl->cur_idb_group = cur_idb[i].group;
                            lanes_ctrl->cur_idb_slots[0] = cur_idb[i].slot;
                        }
                    }
                }

                if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
                    SOC_IF_ERROR_RETURN(soc_th_port_lanes_ctrl_info_diag(unit, lanes_ctrl));
                }

                for (i = 0; i < _OVS_GROUP_COUNT; i++) {
                    if (cnt_mmu[i] == 1)
                        sp_group1 = i;
                    if (cnt_mmu[i] == 3)
                        sp_group2 = i;
                    if ((sp_group1 > 0) && (sp_group2 > 0))
                        break;
                }
                if (sp_group1 < 0 || sp_group2 < 0) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "FAILED in search the number of mmu\n")));
                    return SOC_E_INTERNAL;
                }
                if (lanes_ctrl->mmu_slots[0] == -1) {
                    inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
                    reg = mmu_grp_tbl_regs[sp_group2];
                    for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_get(unit, reg, inst, slot, &rval));
                        if (soc_reg_field_get(unit, reg, rval, PORT_NUMf) != 0x3f) {
                            continue;
                        }
                        lanes_ctrl->mmu_group = sp_group2;
                        lanes_ctrl->mmu_slots[0] = slot;
                        break;
                    }
                    if (slot == _OVS_GROUP_TDM_LENGTH) {
                        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                    (BSL_META_U(unit,
                                                "FAILED in search the empty mmu slot\n")));
                        return SOC_E_INTERNAL;
                    }
                }
                if (lanes_ctrl->cur_mmu_slots[0] == -1) {
                    for (i = 0; i < _OVS_GROUP_COUNT; i++) {
                        if(cur_mmu[i].group == sp_group1) {
                            lanes_ctrl->cur_mmu_group = cur_mmu[i].group;
                            lanes_ctrl->cur_mmu_slots[0] = cur_mmu[i].slot;
                        }
                    }
                }

                if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
                    SOC_IF_ERROR_RETURN(soc_th_port_lanes_ctrl_info_diag(unit, lanes_ctrl));
                }

                /* Perform migrating to new group/slot */
                SOC_IF_ERROR_RETURN(soc_th_port_lanes_migrate(unit, lanes_ctrl));

                /* Re-structure the lanes control */
                sal_memset(&re_lanes_ctrl, 0, sizeof(re_lanes_ctrl));
                re_lanes_ctrl.port_base = lanes_ctrl->port_base;
                re_lanes_ctrl.lanes = lanes_ctrl->lanes;
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_port_lanes_validate(unit, &re_lanes_ctrl));
                sal_memcpy(lanes_ctrl, &re_lanes_ctrl, sizeof(re_lanes_ctrl));
                return SOC_E_NONE;
            } else {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "FAILED in search lanes group\n")));
                return SOC_E_INTERNAL;
            }
        }

        if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
            /* ports being removed */
            for (i = 0; i <= lanes_ctrl->phy_ports_len; i++) {
                if ((-1 == lanes_ctrl->cur_idb_slots[i]) ||
                    (-1 == lanes_ctrl->cur_mmu_slots[i])) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "FAILED in search ports slots remove\n")));
                    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
                        SOC_IF_ERROR_RETURN(soc_th_port_lanes_ctrl_info_diag(unit, lanes_ctrl));
                    }
                    return SOC_E_INTERNAL;
                }
            }
        } else {
            /* ports being added */
            if ((-1 == lanes_ctrl->cur_idb_slots[0]) ||
                (-1 == lanes_ctrl->cur_mmu_slots[0])) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "FAILED in search ports slots added\n")));
                if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
                    SOC_IF_ERROR_RETURN(soc_th_port_lanes_ctrl_info_diag(unit, lanes_ctrl));
                }
                return SOC_E_INTERNAL;
            }
        }
    }
    return SOC_E_NONE;
}

soc_error_t
soc_th_speed_set_init_ctrl(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_t port_base = lanes_ctrl->port_base;

    lanes_ctrl->oversub = SOC_PBMP_MEMBER(si->oversub_pbm, port_base);
    if (lanes_ctrl->oversub) {
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_oversub_group_find(unit, lanes_ctrl));
    }

    return SOC_E_NONE;
}

STATIC soc_error_t
soc_th_os_mixed_sister_port_mode_get(int unit, int initTime, soc_th_port_lanes_t *lanes_ctrl)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_t phy_port_base, phy_port;
    soc_port_t port_sub, port_sis;
    int port_sub_speed, port_sis_speed;
    soc_port_t port_base = lanes_ctrl->port_base;

    if (lanes_ctrl->os_mixed_sister == 0) {
        return SOC_E_FAIL;
    }

    phy_port = si->port_l2p_mapping[port_base];
    if (-1 == phy_port) {
        return SOC_E_PORT;
    }
    phy_port_base = PORT_BLOCK_BASE_PORT(port_base);
    if ((phy_port_base != phy_port) && ((phy_port_base + 2) != phy_port)) {
        /* Not a controlling port */
        return SOC_E_PORT;
    }

    /* check mixed sister port mode */
    port_sub = si->port_p2l_mapping[phy_port_base];
    port_sis = si->port_p2l_mapping[phy_port_base + 2];
    port_sub_speed = initTime ? si->port_speed_max[port_sub] : si->port_init_speed[port_sub];
    port_sis_speed = initTime ? si->port_speed_max[port_sis] : si->port_init_speed[port_sis];

    if (!SOC_PBMP_MEMBER(si->all.disabled_bitmap, si->port_p2l_mapping[phy_port_base + 1]) ||
        !SOC_PBMP_MEMBER(si->all.disabled_bitmap, si->port_p2l_mapping[phy_port_base + 3])) {
        return SOC_E_PORT;
    }

    if (((port_sub_speed == 50000) && (port_sis_speed == 50000)) ||
        ((port_sub_speed == 53000) && (port_sis_speed == 53000))) {
        lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_50_0_50_0;
    } else if (((port_sub_speed == 50000) && (port_sis_speed == 25000)) ||
               ((port_sub_speed == 53000) && (port_sis_speed == 27000))) {
               lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_50_0_25_0;
    } else if (((port_sub_speed == 25000) && (port_sis_speed == 50000)) ||
               ((port_sub_speed == 27000) && (port_sis_speed == 53000))) {
               lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_25_0_50_0;
    } else if (((port_sub_speed == 25000) && (port_sis_speed == 25000)) ||
               ((port_sub_speed == 27000) && (port_sis_speed == 27000))) {
               lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_25_0_25_0;
    } else if (((port_sub_speed == 40000) && (port_sis_speed == 40000)) ||
               ((port_sub_speed == 43000) && (port_sis_speed == 43000))) {
               lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_40_0_40_0;
    } else if (((port_sub_speed == 20000) && (port_sis_speed == 20000)) ||
               ((port_sub_speed == 21000) && (port_sis_speed == 21000))) {
               lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_20_0_20_0;
    } else {
        lanes_ctrl->mixed_port_mode = MIXED_DUAL_MODE_0;
    }
    if (initTime == FALSE) {
        if ((si->port_init_speed[port_base] == 25000) ||
            (si->port_init_speed[port_base] == 27000)) {
            lanes_ctrl->cur_lanes = 1;
        } else if ((si->port_init_speed[port_base] == 50000) ||
                   (si->port_init_speed[port_base] == 40000) ||
                   (si->port_init_speed[port_base] == 53000) ||
                   (si->port_init_speed[port_base] == 42000) ||
                   (si->port_init_speed[port_base] == 20000) ||
                   (si->port_init_speed[port_base] == 21000)) {
                   lanes_ctrl->cur_lanes = 2;
        } else {
            return SOC_E_PORT;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_tomahawk_port_lanes_validate
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
soc_tomahawk_port_lanes_validate(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
    soc_info_t *si = &SOC_INFO(unit);
    int port_base, phy_port_base;
    uint32 rval;
    int i;
    int chk_mixed_sister = 0;

    lanes_ctrl->os_mixed_sister = si->os_mixed_sister_25_50_enable;

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
    case SOC_TH_PORT_MODE_QUAD:
        lanes_ctrl->cur_lanes = 1;
        break;
    case SOC_TH_PORT_MODE_TRI_012:
        lanes_ctrl->cur_lanes = lanes_ctrl->bindex == 0 ? 1 : 2;
        break;
    case SOC_TH_PORT_MODE_TRI_023:
        lanes_ctrl->cur_lanes = lanes_ctrl->bindex == 0 ? 2 : 1;
        break;
    case SOC_TH_PORT_MODE_DUAL:
        if (lanes_ctrl->os_mixed_sister == 0) {
            lanes_ctrl->cur_lanes = 2;
        } else {
            chk_mixed_sister = 1;
        }
        break;
    case SOC_TH_PORT_MODE_SINGLE:
        lanes_ctrl->cur_lanes = 4;
        break;
    default:
        return SOC_E_FAIL;
    }

    /* check the mixed sister port mode */
    if (chk_mixed_sister) {
        SOC_IF_ERROR_RETURN(soc_th_os_mixed_sister_port_mode_get(unit, FALSE, lanes_ctrl));
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

    /* Figure out new mode */
    if (lanes_ctrl->lanes == 4) {
        lanes_ctrl->mode = SOC_TH_PORT_MODE_SINGLE;
    } else if (lanes_ctrl->lanes == 2) {
        if (lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_QUAD) {
            lanes_ctrl->mode = lanes_ctrl->bindex == 0 ?
                SOC_TH_PORT_MODE_TRI_023 : SOC_TH_PORT_MODE_TRI_012;
        } else {
            lanes_ctrl->mode = SOC_TH_PORT_MODE_DUAL;
        }
    } else{
        if (lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_DUAL) {
            if (lanes_ctrl->os_mixed_sister &&
                (lanes_ctrl->mixed_port_mode != MIXED_DUAL_MODE_40_0_40_0)) {
                lanes_ctrl->mode = SOC_TH_PORT_MODE_DUAL;
            } else {
                lanes_ctrl->mode = lanes_ctrl->bindex == 0 ?
                    SOC_TH_PORT_MODE_TRI_012 : SOC_TH_PORT_MODE_TRI_023;
            }
        } else {
            lanes_ctrl->mode = SOC_TH_PORT_MODE_QUAD;
        }
    }
    if (lanes_ctrl->oversub) {
        if (SOC_TH_PORT_MODE_TRI_012 == lanes_ctrl->mode ||
            SOC_TH_PORT_MODE_TRI_023 == lanes_ctrl->mode) {
            return SOC_E_PARAM;
        }
    }
    if (lanes_ctrl->os_mixed_sister) {
        if ((lanes_ctrl->mixed_port_mode == MIXED_DUAL_MODE_50_0_25_0) &&
            (SOC_TH_PORT_MODE_SINGLE == lanes_ctrl->mode) &&
            (SOC_TH_PORT_MODE_DUAL == lanes_ctrl->cur_mode)) {
                return SOC_E_PARAM;
        }
    }

    lanes_ctrl->phy_ports_len = 0;
    if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
        /* Figure out which port(s) to be removed */
        if (lanes_ctrl->lanes == 4) {
            if (lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_TRI_012 ||
                lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_QUAD) {
                if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
                    lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                        phy_port_base + 1;
                }
            }
            if (si->port_p2l_mapping[phy_port_base + 2] != -1) {
                lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                                      phy_port_base + 2;
            }
            if (lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_TRI_023 ||
                lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_QUAD) {
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
            if (lanes_ctrl->os_mixed_sister == 0) {
                if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
                    lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                        phy_port_base + 1;
                }
            } else {
                if (((lanes_ctrl->lanes != 2) && (lanes_ctrl->cur_lanes != 1)) &&
                    ((lanes_ctrl->lanes != 1) && (lanes_ctrl->cur_lanes != 2))) {
                    lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                        phy_port_base + 1;
                } /* 1x25G+1x50G,1x50G+1x25G,2x25G don't need to change number of port */
                if ((lanes_ctrl->lanes == 1) && (lanes_ctrl->cur_lanes == 4)) {
                    lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                        phy_port_base + 1;
                } /* 1x100G --> 4x25G */
            }

            if (lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_SINGLE) {
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

    /* Initialize all OS group/slot info to -1 */
    lanes_ctrl->cur_idb_group = lanes_ctrl->idb_group = -1;
    lanes_ctrl->cur_mmu_group = lanes_ctrl->mmu_group = -1;
    for (i = 0; i <= lanes_ctrl->phy_ports_len; i++) {
        lanes_ctrl->idb_slots[i] = -1;
        lanes_ctrl->mmu_slots[i] = -1;
        lanes_ctrl->cur_idb_slots[i] = -1;
        lanes_ctrl->cur_mmu_slots[i] = -1;
    }

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        SOC_IF_ERROR_RETURN(soc_th_port_lanes_ctrl_info_diag(unit, lanes_ctrl));
    }
    if (lanes_ctrl->os_mixed_sister == 0) {
        if (lanes_ctrl->oversub) {
            SOC_IF_ERROR_RETURN(soc_tomahawk_oversub_group_find(unit, lanes_ctrl));
        }
    } else {
        if (((lanes_ctrl->lanes == 1) && (lanes_ctrl->cur_lanes == 4)) ||
            ((lanes_ctrl->lanes == 4) && (lanes_ctrl->cur_lanes == 1)) ||
            ((lanes_ctrl->lanes == 4) && (lanes_ctrl->cur_lanes == 2)) ||
            ((lanes_ctrl->lanes == 2) && (lanes_ctrl->cur_lanes == 4)) ||
            (((lanes_ctrl->lanes != 2) && (lanes_ctrl->cur_lanes != 1)) &&
            ((lanes_ctrl->lanes != 1) && (lanes_ctrl->cur_lanes != 2)))) {
            SOC_IF_ERROR_RETURN(soc_tomahawk_oversub_group_find(unit, lanes_ctrl));
        }
    }

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        SOC_IF_ERROR_RETURN(soc_th_port_lanes_ctrl_info_diag(unit, lanes_ctrl));
    }

    return SOC_E_NONE;
}

STATIC soc_error_t
_soc_tomahawk_flexed_port_speed_get(int unit, soc_th_port_lanes_t *lanes_ctrl,
                                     uint32 *port_speed)
{
    uint32 vco;
    soc_error_t rc;
    int port = lanes_ctrl->port_base;
    int higigPort = IS_HG_PORT(unit, port);
    soc_info_t *si = &SOC_INFO(unit);

    if (TH_FLEXPORT_SPEED_SET_NUM_LANES == lanes_ctrl->lanes) {
        if (((lanes_ctrl->speed >= 100000) && (si->port_speed_max[port] < 100000)) ||
            ((lanes_ctrl->speed >= 50000) && (si->port_speed_max[port] < 50000)) ||
            ((lanes_ctrl->speed >= 40000) && (si->port_speed_max[port] < 40000)) ||
            ((lanes_ctrl->speed >= 25000) && (si->port_speed_max[port] < 25000)) ||
            ((lanes_ctrl->speed >= 20000) && (si->port_speed_max[port] < 20000)) ||
            ((lanes_ctrl->speed >= 10000) && (si->port_speed_max[port] < 10000))) {
            return SOC_E_CONFIG;
        } else {
            *port_speed = lanes_ctrl->speed;
            return SOC_E_NONE;
        }
    }
    /* Determine new port speed based on vco frequency of the PM and number of
     * lanes in the flexed port.
     */
    rc = soc_phyctrl_redirect_control_get(unit, port, 0, 0, 0, SOC_PHY_CONTROL_VCO_FREQ, &vco);
    rc = soc_phyctrl_control_get(unit, port,
                                 SOC_PHY_CONTROL_VCO_FREQ,
                                 &vco);
    if (SOC_E_NONE != rc) {
        /* Set default vco, phyctrl may not exist, for e.x. ext phy */
        if (si->port_init_speed[port] >= 50000) {
            vco = 25781;
        } else if (si->port_init_speed[port] >= 40000) {
            vco = 20625;
        } else if (si->port_init_speed[port] >= 25000) {
            vco = 25781;
        } else {
            vco = 20625;
        }
    }
    if (lanes_ctrl->lanes == 1) {
        /* Possibilities are 25G/10G */
        if (vco >= 25781) {
            *port_speed = higigPort ? 27000 : 25000;
        } else {
            *port_speed = higigPort ? 11000 : 10000;
        }
    } else if (lanes_ctrl->lanes == 2) {
        if (vco >= 25781) {
            *port_speed = higigPort ? 53000 : 50000;
        } else {
            *port_speed = higigPort ? 42000 : 40000;
        }
    } else {
        /* Possibilities are 100G/40G */
        if (vco >= 25781) {
            *port_speed = higigPort ? 106000 : 100000;
        } else {
            *port_speed = higigPort ? 42000 : 40000;
        }
    }
    if (*port_speed > SOC_INFO(unit).port_speed_max[port]) {
        return SOC_E_CONFIG;
    }
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_xmt_start_cnt_update(int unit, int extra_delay)
{
    int                 port;
    int                 port_speed;
    soc_th_asf_mode_e   mode = _SOC_TH_ASF_MODE_SAF;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
            continue;
        }
        port_speed = si->port_init_speed[port];
        if (!SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port)) {
            SOC_IF_ERROR_RETURN(
                soc_th_port_asf_mode_get(unit, port, port_speed, &mode));
        }
        SOC_IF_ERROR_RETURN(
            soc_th_port_asf_xmit_start_count_set(unit, port, port_speed,
                                                 mode, extra_delay));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_port_lanes_update_tdm(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
    soc_control_t *soc;
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    int port_base, port, phy_port, idb_port, mmu_port;
    int group, pipe, slot, id, i, clport, mode, lane, port_block_base;
    int rv;
    soc_reg_t reg;
    uint32 rval;
    uint32 inst;

    static const soc_reg_t pblk_calendar_regs[] ={
        IS_PBLK0_CALENDARr, IS_PBLK1_CALENDARr,
        IS_PBLK2_CALENDARr, IS_PBLK3_CALENDARr,
        IS_PBLK4_CALENDARr, IS_PBLK5_CALENDARr,
        IS_PBLK6_CALENDARr, IS_PBLK7_CALENDARr
    };
    static const soc_reg_t mmu_pblk_calendar_regs[] = {
        PBLK0_CALENDARr, PBLK1_CALENDARr, PBLK2_CALENDARr, PBLK3_CALENDARr,
        PBLK4_CALENDARr, PBLK5_CALENDARr, PBLK6_CALENDARr, PBLK7_CALENDARr
    };
    static int pblk_slots[SOC_TH_PORT_RATIO_COUNT][7] = {
        { 0, -1,  0,  0, -1,  0, -1 }, /* SOC_TH_PORT_RATIO_SINGLE */
        { 0, -1,  2,  0, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_1 */
        { 0,  0,  2,  0,  0,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_2_1 */
        { 0,  2,  2,  0,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_2 */
        { 0, -1,  2,  0, -1,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_2_1_1 */
        { 0,  0,  2,  0,  0,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_4_1_1 */
        { 0, -1,  2,  1, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_2 */
        { 0,  2,  2,  1,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_4 */
        { 0, -1,  2,  1, -1,  3, -1 }  /* SOC_TH_PORT_RATIO_QUAD */
    };

    soc = SOC_CONTROL(unit);
    si = &SOC_INFO(unit);
    tdm = soc->tdm_info;
    port_base = lanes_ctrl->port_base;
    phy_port = si->port_l2p_mapping[port_base];
    pipe = si->port_pipe[port_base];
    id = si->port_serdes[port_base] & 7;
    clport = si->port_serdes[port_base];

    if (!SOC_PBMP_MEMBER(si->oversub_pbm, port_base)) {
        rv = _soc_tomahawk_xmt_start_cnt_update(unit, 9);
        if (SOC_FAILURE(rv)){
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "FAILED in"
                                  "_soc_tomahawk_xmt_start_cnt_update\n")));
            return rv;
        }

        rv = _soc_tomahawk_tdm_calculation(unit, FALSE, tdm);
        if (SOC_FAILURE(rv)){
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "FAILED in"
                                  "_soc_tomahawk_tdm_calculation\n")));
            SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation_free_memory(unit));
            return rv;
        }

        rv = _soc_tomahawk_tdm_idb_calendar_set(unit, -1, pipe);
        if (SOC_FAILURE(rv)){
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "FAILED in"
                                  "_soc_tomahawk_tdm_idb_calendar_set\n")));
            SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation_free_memory(unit));
            return rv;
        }

        rv = _soc_tomahawk_tdm_mmu_calendar_set(unit, -1, pipe);
        if (SOC_FAILURE(rv)){
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "FAILED in"
                                  "_soc_tomahawk_tdm_mmu_calendar_set\n")));
            SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation_free_memory(unit));
            return rv;
        }

        SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation_free_memory(unit));

        rv = _soc_tomahawk_xmt_start_cnt_update(unit, 0);
        if (SOC_FAILURE(rv)){
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "FAILED in"
                                  "_soc_tomahawk_xmt_start_cnt_update\n")));
            return rv;
        }
    } else if ((lanes_ctrl->ovs_class != lanes_ctrl->cur_ovs_class) ||
               ((SOC_TH_SPEED_CLASS_40G == lanes_ctrl->ovs_class) &&
                (-1 != lanes_ctrl->cur_idb_group) &&
                (-1 != lanes_ctrl->cur_mmu_group) &&
                (-1 != lanes_ctrl->idb_group) &&
                (-1 != lanes_ctrl->mmu_group))) {
        /* Update CLPORT details in tdm structure*/
        soc_tomahawk_port_ratio_get(unit, 0 /*initTime*/, clport, &tdm->port_ratio[clport]);

        /* Remove idb port(s) from current group */
        group = lanes_ctrl->cur_idb_group;
        reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, 0x3f);
        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, 0);
        slot = lanes_ctrl->cur_idb_slots[0];
        if (slot < 0) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
        if ((lanes_ctrl->lanes > lanes_ctrl->cur_lanes) &&
            (TH_FLEXPORT_SPEED_SET_NUM_LANES != lanes_ctrl->lanes)) {
            /* port(s) to be removed */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                slot = lanes_ctrl->cur_idb_slots[i + 1];
                if (slot < 0) {
                    return SOC_E_INTERNAL;
                }
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
            }
        }
        if (lanes_ctrl->cur_idb_empty &&
            (lanes_ctrl->idb_group != lanes_ctrl->cur_idb_group)) { /* old group become empty */
            reg = SOC_REG_UNIQUE_ACC(unit, IS_OVR_SUB_GRP_CFGr)[pipe];
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, group, 0));
        }

        /* Add idb port(s) to new group */
        group = lanes_ctrl->idb_group;
        reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id & 0x7);
        idb_port = si->port_l2i_mapping[port_base];
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, idb_port);
        slot = lanes_ctrl->idb_slots[0] != -1 ? lanes_ctrl->idb_slots[0] : 0;
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
        if ((lanes_ctrl->lanes < lanes_ctrl->cur_lanes) &&
            (TH_FLEXPORT_SPEED_SET_NUM_LANES != lanes_ctrl->lanes)) {
            /* port(s) to be added */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
                idb_port = si->port_l2i_mapping[port];
                soc_reg_field_set(unit, reg, &rval, PORT_NUMf, idb_port);
                slot = lanes_ctrl->idb_slots[0] != -1 ?
                    lanes_ctrl->idb_slots[i + 1] : i + 1;
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
            }
        }
        if (lanes_ctrl->idb_slots[0] == -1) { /* new group */
            reg = SOC_REG_UNIQUE_ACC(unit, IS_OVR_SUB_GRP_CFGr)[pipe];
            rval = 0;
            soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf, 4);
            soc_reg_field_set(unit, reg, &rval, SISTER_SPACINGf, 4);
            soc_reg_field_set(unit, reg, &rval, SPEEDf, lanes_ctrl->ovs_class);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, group, rval));
        }

        /* Remove mmu port(s) from current group */
        group = lanes_ctrl->cur_mmu_group;
        reg = mmu_grp_tbl_regs[group];
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, 0x3f);
        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, 0);
        slot = lanes_ctrl->cur_mmu_slots[0];
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));
        if ((lanes_ctrl->lanes > lanes_ctrl->cur_lanes) &&
            (TH_FLEXPORT_SPEED_SET_NUM_LANES != lanes_ctrl->lanes)) {
            /* port(s) to be removed */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                slot = lanes_ctrl->cur_mmu_slots[i + 1];
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));
            }
        }
        if (lanes_ctrl->cur_idb_empty &&
            (lanes_ctrl->idb_group != lanes_ctrl->cur_idb_group)) { /* old group became empty */
            reg = OVR_SUB_GRP_CFGr;
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, group, 0));
        }

        /* Add mmu port(s) to new group */
        group = lanes_ctrl->mmu_group;
        reg = mmu_grp_tbl_regs[group];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id & 0x7);
        mmu_port = si->port_p2m_mapping[phy_port];
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, mmu_port & 0x3f);
        slot = lanes_ctrl->mmu_slots[0] != -1 ? lanes_ctrl->mmu_slots[0] : 0;
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));
        if ((lanes_ctrl->lanes < lanes_ctrl->cur_lanes) &&
            (TH_FLEXPORT_SPEED_SET_NUM_LANES != lanes_ctrl->lanes)) {
            /* port(s) to be added */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                mmu_port = si->port_p2m_mapping[lanes_ctrl->phy_ports[i]];
                soc_reg_field_set(unit, reg, &rval, PORT_NUMf, mmu_port & 0x3f);
                slot = lanes_ctrl->mmu_slots[0] != -1 ?
                    lanes_ctrl->mmu_slots[i + 1] : i + 1;
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));
            }
        }
        if (lanes_ctrl->mmu_slots[0] == -1) { /* new group */
            reg = OVR_SUB_GRP_CFGr;
            rval = 0;
            soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf,
                              lanes_ctrl->ovs_class >= 4 ? 4 : 8);
            soc_reg_field_set(unit, reg, &rval, SISTER_SPACINGf, 4);
            soc_reg_field_set(unit, reg, &rval, SPEEDf, lanes_ctrl->ovs_class);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, group, rval));
        }
    }

    if ((TH_FLEXPORT_SPEED_SET_NUM_LANES != lanes_ctrl->lanes) ||
        ((TH_FLEXPORT_SPEED_SET_NUM_LANES == lanes_ctrl->lanes) &&
         ((lanes_ctrl->ovs_class < 4 && lanes_ctrl->cur_ovs_class >= 4) ||
         (lanes_ctrl->ovs_class >= 4 && lanes_ctrl->cur_ovs_class < 4)))) {
        mode = tdm->port_ratio[clport];
        port_block_base = PORT_BLOCK_BASE_PORT(port_base);
        /* Reprogram IS port block calendar */
        reg = SOC_REG_UNIQUE_ACC(unit, pblk_calendar_regs[id])[pipe];
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
            port = si->port_p2l_mapping[port_block_base + lane];
            idb_port = si->port_l2i_mapping[port];
            soc_reg_field_set(unit, reg, &rval, PORT_NUMf, idb_port);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
        }

        /* Reprogram MMU port block calendar */
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        reg = mmu_pblk_calendar_regs[id];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, VALIDf, 1);
        soc_reg_field_set(unit, reg, &rval, SPACINGf,
                          lanes_ctrl->ovs_class >= 4 ? 4 : 8);
        for (slot = 0; slot < 7; slot++) {
            lane = pblk_slots[mode][slot];
            if (lane == -1) {
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, 0));
                continue;
            }
            mmu_port = si->port_p2m_mapping[port_block_base + lane];
            soc_reg_field_set(unit, reg, &rval, PORT_NUMf, mmu_port & 0x3f);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));
        }
    }

    return SOC_E_NONE;
}

STATIC soc_error_t
_soc_tomahawk_obm_counters_reset(int unit, soc_port_t port)
{
    int idx = 0;
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t obm_counter_regs[] = {
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO,
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO,
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI,
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI,
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0,
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0,
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1,
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1
    };
    uint64 my_zero;

    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
    ctrl_info.instance = port;
    COMPILER_64_ZERO(my_zero);
    for (idx = 0; idx < COUNTOF(obm_counter_regs); idx++) {
        SOC_IF_ERROR_RETURN
            (soc_counter_generic_set(unit, obm_counter_regs[idx], ctrl_info, 0,
                                     0, my_zero));
    }

    return SOC_E_NONE;
}

soc_error_t
soc_tomahawk_edb_buf_reset(int unit, soc_port_t port, int reset)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port, index;

    phy_port = si->port_l2p_mapping[port];

    /* In Tomahawk2, EGR_PER_PORT_BUFFER_SFT_RESETm, EGR_ENABLEm are indexed
     * by physical port num */
    index = SOC_IS_TOMAHAWK2(unit) ? phy_port : port;

    if (reset) {
        soc_reg_t reg;
        soc_timeout_t to;
        int pipe;
        uint32 rval, level;

        pipe = si->port_pipe[port];
        reg = SOC_REG_UNIQUE_ACC(unit, EGR_EDB_MISC_CTRLr)[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, SELECT_CURRENT_USED_ENTRIESf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

        soc_timeout_init(&to, 250000, 0);
        /* Poll until EDB buffer is empty */
        for (;;) {
            SOC_IF_ERROR_RETURN
                (READ_EGR_MAX_USED_ENTRIESm(unit, MEM_BLOCK_ALL,
                                            phy_port, entry));
            level = soc_mem_field32_get(unit, EGR_MAX_USED_ENTRIESm,
                                        entry, LEVELf);
            if (!level) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "EDBBufferDrainTimeOut:port %d,%s, timeout"
                                      "(pending: %d)\n"),
                           unit, SOC_PORT_NAME(unit, port), level));
                return SOC_E_INTERNAL;
            }
        }
        SOC_IF_ERROR_RETURN(READ_EGR_ENABLEm(unit, MEM_BLOCK_ALL, index, entry));
        soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, !reset);
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, index, entry));
        SOC_IF_ERROR_RETURN
            (READ_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                 index, entry));
        soc_mem_field32_set(unit, EGR_PER_PORT_BUFFER_SFT_RESETm,
                            entry, ENABLEf, reset);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                  index, entry));
    } else {
        uint32 rval = 0;
        /*
         * Initialize EP credits in MMU so EP is in charge of distributing
         * the correct number of credits
         */
        soc_reg_field_set(unit, MMU_PORT_CREDITr, &rval, INITIALIZEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, MMU_PORT_CREDITr, port, 0, rval));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, MMU_PORT_CREDITr, port, 0, 0));

        /* Release EDB port buffer reset and enable cell request generation in EP */
        SOC_IF_ERROR_RETURN
            (READ_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                 index, entry));
        soc_mem_field32_set(unit, EGR_PER_PORT_BUFFER_SFT_RESETm,
                            entry, ENABLEf, reset);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                  index, entry));
        SOC_IF_ERROR_RETURN(READ_EGR_ENABLEm(unit, MEM_BLOCK_ALL, index, entry));
        soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, !reset);
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, index, entry));
    }
    return SOC_E_NONE;
}

soc_error_t
soc_tomahawk_idb_buf_reset(int unit, soc_port_t port, int reset)
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
        IDB_OBM6_USAGEr, IDB_OBM7_USAGEr
    };
    static const soc_reg_t idb_obm_ca_status_regs[] = {
        IDB_OBM0_CA_HW_STATUSr, IDB_OBM1_CA_HW_STATUSr,
        IDB_OBM2_CA_HW_STATUSr, IDB_OBM3_CA_HW_STATUSr,
        IDB_OBM4_CA_HW_STATUSr, IDB_OBM5_CA_HW_STATUSr,
        IDB_OBM6_CA_HW_STATUSr, IDB_OBM7_CA_HW_STATUSr
    };
    static const soc_reg_t obm_ctrl_regs[] = {
        IDB_OBM0_CONTROLr, IDB_OBM1_CONTROLr,
        IDB_OBM2_CONTROLr, IDB_OBM3_CONTROLr,
        IDB_OBM4_CONTROLr, IDB_OBM5_CONTROLr,
        IDB_OBM6_CONTROLr, IDB_OBM7_CONTROLr
    };
    static const soc_reg_t obm_ca_ctrl_regs[] = {
        IDB_OBM0_CA_CONTROLr, IDB_OBM1_CA_CONTROLr,
        IDB_OBM2_CA_CONTROLr, IDB_OBM3_CA_CONTROLr,
        IDB_OBM4_CA_CONTROLr, IDB_OBM5_CA_CONTROLr,
        IDB_OBM6_CA_CONTROLr, IDB_OBM7_CA_CONTROLr
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
    /* obm number is reversed (mirrored) in odd pipe */
    obm = pipe & 1 ? 7 - (clport & 0x7) : clport & 0x7;

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

    if (reset) {
        /* OBM counters will be cleared by IDB reset.
         * Clear in counter thread accordingly.
         */
        SOC_IF_ERROR_RETURN(_soc_tomahawk_obm_counters_reset(unit, port));
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

STATIC int
_soc_tomahawk_obm_thresholds_reconfigure(int unit, soc_port_t port_conf)
{
#define _TH_BYTES_PER_OBM_CELL    16
#define _TH_CELLS_PER_OBM         1012
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    soc_reg_t reg;
    int block_info_idx, pipe, clport, obm, lane;
    int port, phy_port_base;
    int lossless;
    uint32 fval;
    uint64 rval64;
    int num_lanes, oversub_ratio_idx;
    static const struct _obm_setting_s {
        int discard_limit;
        int lossless_xoff;
        int lossy_only_lossy_low_pri;
        int port_xoff[3];          /* for oversub ratio 1.5, 1.8, and above */
        int lossy_low_pri[3];      /* for oversub ratio 1.5, 1.8, and above */
        int lossy_discard[3];      /* for oversub ratio 1.5, 1.8, and above */
    } obm_settings[2][5] = {
        /* LOSSY Settings */
        { /* indexed by number of lanes used in the port */
            { /* 0 - invalid  */ 0 },
            { /* 1 lane */
                _TH_CELLS_PER_OBM / 4, 1023, 157,
                { 1023, 1023, 1023 }, /* port_xoff */
                { 157, 157, 157 }, /* lossy_low_pri */
                { 253, 253, 253 }  /* lossy_discard */
            },
            { /* 2 lanes */
                _TH_CELLS_PER_OBM / 2, 1023, 410,
                { 1023, 1023, 1023 }, /* port_xoff */
                { 410, 410, 410 }, /* lossy_low_pri */
                { 506, 506, 506 }  /* lossy_discard */
            },
            { /* 3 - invalid  */ 0 },
            { /* 4 lanes */
                _TH_CELLS_PER_OBM, 1023, 916,
                { 1023, 1023, 1023 }, /* port_xoff */
                { 916, 916, 916 }, /* lossy_low_pri */
                { 1012, 1012, 1012 }  /* lossy_discard */
            }
        },
        /* LOSSLESS Settings*/
        { /* indexed by number of lanes used in the port */
            { /* 0 - invalid  */ 0 },
            { /* 1 lane */
                _TH_CELLS_PER_OBM / 4, 45, 157,
                { 129, 87, 67 }, /* port_xoff */
                { 100, 76, 56 }, /* lossy_low_pri */
                { 196, 152, 112 }  /* lossy_discard */
            },
            { /* 2 lanes */
                _TH_CELLS_PER_OBM / 2, 108, 410,
                { 312, 248, 215 }, /* port_xoff */
                { 100, 100, 100 }, /* lossy_low_pri */
                { 196, 196, 196 }  /* lossy_discard */
            },
            { /* 3 - invalid  */ 0 },
            { /* 4 lanes */
                _TH_CELLS_PER_OBM, 258, 916,
                { 682, 572, 517 }, /* port_xoff */
                { 100, 100, 100 }, /* lossy_low_pri */
                { 196, 196, 196 }  /* lossy_discard */
            }
        }
    };
    static const soc_reg_t obm_thresh_regs[] = {
        IDB_OBM0_THRESHOLDr, IDB_OBM1_THRESHOLDr,
        IDB_OBM2_THRESHOLDr, IDB_OBM3_THRESHOLDr,
        IDB_OBM4_THRESHOLDr, IDB_OBM5_THRESHOLDr,
        IDB_OBM6_THRESHOLDr, IDB_OBM7_THRESHOLDr
    };
    static const soc_reg_t obm_fc_thresh_regs[] = {
        IDB_OBM0_FC_THRESHOLDr, IDB_OBM1_FC_THRESHOLDr,
        IDB_OBM2_FC_THRESHOLDr, IDB_OBM3_FC_THRESHOLDr,
        IDB_OBM4_FC_THRESHOLDr, IDB_OBM5_FC_THRESHOLDr,
        IDB_OBM6_FC_THRESHOLDr, IDB_OBM7_FC_THRESHOLDr
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    if (SOC_IS_TOMAHAWKPLUS(unit)) {
        lossless = soc_property_get(unit, spn_MMU_LOSSLESS,
                                    SOC_TH_MMU_LOSSLESS_DEFAULT_DISABLE);
    } else {
        lossless = soc_property_get(unit, spn_MMU_LOSSLESS,
                                    SOC_TH_MMU_LOSSLESS_DEFAULT_ENABLE);
    }

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        if (port != port_conf) {
            continue;
        }

        phy_port_base = PORT_BLOCK_BASE_PORT(port);
        pipe = si->port_pipe[port];
        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);
        /* obm number is reversed (mirrored) in odd pipe */
        obm = pipe & 1 ? 7 - (clport & 0x7) : clport & 0x7;

        if (!SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
            continue;
        }

        if (lossless) {
            if (tdm->ovs_ratio_x1000[pipe] >= 1800) { /* ratio >= 1.8 */
                oversub_ratio_idx = 2;
            } else if (tdm->ovs_ratio_x1000[pipe] >= 1500) { /* ratio >= 1.5 */
                oversub_ratio_idx = 1;
            } else {
                oversub_ratio_idx = 0;
            }

            for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane++) {
                port = si->port_p2l_mapping[phy_port_base + lane];
                if (port == -1) {
                    continue;
                }
                num_lanes = si->port_num_lanes[port];

                /* Configure threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[1][num_lanes].discard_limit;
                soc_reg64_field32_set(unit, reg, &rval64, DISCARD_LIMITf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                                      1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_DISCARDf,
                                      1023);
                fval = obm_settings[1][num_lanes].lossy_low_pri[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_LOW_PRIf, fval);
                fval = obm_settings[1][num_lanes].lossy_discard[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_DISCARDf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_MINf, 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[1][num_lanes].port_xoff[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XOFFf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XONf, fval - 10);
                fval = obm_settings[1][num_lanes].lossless_xoff;
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                                      fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XOFFf,
                                      fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XONf,
                                      fval - 10);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XONf,
                                      fval - 10);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));
            }
        } else { /* lossy */
            for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane++) {
                port = si->port_p2l_mapping[phy_port_base + lane];
                if (port == -1) {
                    continue;
                }
                num_lanes = si->port_num_lanes[port];

                /* Configure threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[0][num_lanes].discard_limit;
                soc_reg64_field32_set(unit, reg, &rval64, DISCARD_LIMITf, fval);
                /* same as DISCARD_LIMIT setting */
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_DISCARDf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                                      1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_DISCARDf,
                                      1023);
                fval = obm_settings[0][num_lanes].lossy_only_lossy_low_pri;
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_LOW_PRIf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_MINf, 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XOFFf, 1023);
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XONf, 1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                                      1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XOFFf,
                                      1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XONf, 1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XONf, 1023);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_tomahawk_port_lanes_set
 * Description:
 *      Configure number of lanes used by a port
 * Parameters:
 *      unit          - Device number
 *      lanes_ctrl    - Control structure
 * Notes: Caller needs to call soc_tomahawk_port_lanes_validate to fill
 *        in the control structure.
 */
int
soc_tomahawk_port_lanes_set(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
    uint32 rval;
    soc_reg_t reg;
    uint32 values[2];
    soc_port_t it_port;
    soc_field_t fields[2];
    soc_info_t *si = &SOC_INFO(unit);
    int i, pipe, port_base, phy_port_base, port, port_rename = 0;
    int clport, obm;
    uint32 port_speed = 0, cur_port_speed = 0;
    int save_port_speed = 0;
    _soc_tomahawk_tdm_t *tdm;
    static const soc_reg_t obm_ca_ctrl_regs[] = {
        IDB_OBM0_CA_CONTROLr, IDB_OBM1_CA_CONTROLr,
        IDB_OBM2_CA_CONTROLr, IDB_OBM3_CA_CONTROLr,
        IDB_OBM4_CA_CONTROLr, IDB_OBM5_CA_CONTROLr,
        IDB_OBM6_CA_CONTROLr, IDB_OBM7_CA_CONTROLr
    };
    static const int hw_mode_values[SOC_TH_PORT_RATIO_COUNT] = {
        0, 1, 1, 1, 3, 5, 4, 6, 2
    };

    /* Find physical port number and lane index of the specified port */
    port_base = lanes_ctrl->port_base;
    phy_port_base = si->port_l2p_mapping[port_base];
    if (phy_port_base == -1) {
        return SOC_E_PORT;
    }
    pipe = si->port_pipe[port_base];

    /* Update port_init_max info which is used by SerDes driver */
    SOC_IF_ERROR_RETURN
        (_soc_tomahawk_flexed_port_speed_get(unit, lanes_ctrl, &port_speed));

    /* Update soc_control information */
    SOC_CONTROL_LOCK(unit);
    /* No need to update lanes information during speed change */
    if (TH_FLEXPORT_SPEED_SET_NUM_LANES != lanes_ctrl->lanes) {
        if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
            /* port(s) to be removed */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
                SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
            }
        } else { /* port(s) to be added */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
                SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, port);
            }
        }

        /* Update num of lanes info which is used by SerDes driver */
        si->port_num_lanes[port_base] = lanes_ctrl->lanes;
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
            si->port_num_lanes[port] =
                lanes_ctrl->lanes > lanes_ctrl->cur_lanes ? 
                (si->port_speed_max[port] == 50000 ? 2 :
                (si->port_speed_max[port] == 25000 ? 1 : 0)) :
                lanes_ctrl->lanes;
        }
    }

    si->port_init_speed[port_base] = port_speed;
    if (si->port_init_speed[port_base] >= 40000) {
        SOC_PBMP_PORT_ADD(si->eq_pbm, port_base);
        /* Update HSP port multicast T2OQ setting */
        SOC_IF_ERROR_RETURN(_soc_tomahawk_mc_toq_cfg(unit, port_base, TRUE));
    } else {
        SOC_PBMP_PORT_REMOVE(si->eq_pbm, port_base);
        /* Update HSP port multicast T2OQ setting */
        SOC_IF_ERROR_RETURN(_soc_tomahawk_mc_toq_cfg(unit, port_base, FALSE));
    }
    if (lanes_ctrl->os_mixed_sister) {
            cur_port_speed = port_speed;
        /* figure out 25G/50G mixed sister port mode */
        if ((lanes_ctrl->mixed_port_mode > MIXED_DUAL_MODE_40_0_40_0) &&
            ((lanes_ctrl->lanes == 1) ||
             (lanes_ctrl->lanes == 2))) {
            port_speed = 50000;
            save_port_speed = 1;
        }
    }
    SOC_IF_ERROR_RETURN
        (soc_tomahawk_update_icc(unit, port_base, port_speed));
    if (lanes_ctrl->os_mixed_sister) {
        port_speed = cur_port_speed;
    }
    for (i = 0; i < lanes_ctrl->phy_ports_len; i += 1) {
        port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
        if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
            /* port(s) to be removed */
            /* Update pbm of HSP */
            if (si->port_init_speed[port] >= 40000) {
                SOC_PBMP_PORT_REMOVE(si->eq_pbm, port);
                /* Update HSP port multicast T2OQ setting */
                SOC_IF_ERROR_RETURN(_soc_tomahawk_mc_toq_cfg(unit, port,
                                                             FALSE));
            }
        } else { /* port(s) to be added */
            si->port_init_speed[port] = port_speed;
            /* Update pbm of HSP */
            if (si->port_init_speed[port] >= 40000) {
                SOC_PBMP_PORT_ADD(si->eq_pbm, port);
                /* Update HSP port multicast T2OQ setting */
                SOC_IF_ERROR_RETURN(_soc_tomahawk_mc_toq_cfg(unit, port, TRUE));
            }
            if (lanes_ctrl->os_mixed_sister && save_port_speed) {
                port_speed = 50000;
            }
            SOC_IF_ERROR_RETURN
                (soc_tomahawk_update_icc(unit, port, port_speed));
            if (lanes_ctrl->os_mixed_sister && save_port_speed) {
                port_speed = cur_port_speed;
            }
         }
     }

    /* Update TDM config for HSP */
    SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_idb_hsp_set(unit, pipe));
    SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_mmu_hsp_set(unit, pipe));

    /* OBM setting reconfigure */
    if (TH_FLEXPORT_SPEED_SET_NUM_LANES == lanes_ctrl->lanes) {
        SOC_IF_ERROR_RETURN(_soc_tomahawk_obm_thresholds_reconfigure(unit, port_base));
    }

    /*
     * Update bitmaps and port names when switching b/w CE <=> XE ports.
     * Or b/w XE <=> GE ports.
     * Conditions tested include :
     * - Port is a CE port and we are flexing to a port with < 4 lanes
     * - Port is a CE port and we are setting speed to < 100G
     * - Port is a XE port and we are setting speed to 1G
     */
    if (((lanes_ctrl->cur_lanes == 4) && IS_CE_PORT(unit, port_base)) ||
        ((TH_FLEXPORT_SPEED_SET_NUM_LANES == lanes_ctrl->lanes) &&
         IS_CE_PORT(unit, port_base) && (port_speed < 100000))) {

        /* Convert to XE ports */
        SOC_PBMP_PORT_REMOVE(si->ce.bitmap, port_base);
        SOC_PBMP_PORT_ADD(si->xe.bitmap,
                          si->port_p2l_mapping[phy_port_base]);
        port_rename = 1;
    } else if (((lanes_ctrl->lanes == 4) && IS_XE_PORT(unit, port_base) &&
               (port_speed == 100000)) ||
               ((TH_FLEXPORT_SPEED_SET_NUM_LANES == lanes_ctrl->lanes) &&
                (IS_XE_PORT(unit, port_base) && (port_speed == 100000)))) {

        /* Convert to CE ports */
        SOC_PBMP_PORT_ADD(si->ce.bitmap, port_base);
        SOC_PBMP_PORT_REMOVE(si->xe.bitmap,
                             si->port_p2l_mapping[phy_port_base]);
        port_rename = 1;
    } else if ((TH_FLEXPORT_SPEED_SET_NUM_LANES == lanes_ctrl->lanes) &&
               (IS_XE_PORT(unit, port_base) && (port_speed == 1000))) {
        SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port_base);
        SOC_PBMP_PORT_ADD(si->ge.bitmap, port_base);
        port_rename = 1;
    } else if ((TH_FLEXPORT_SPEED_SET_NUM_LANES == lanes_ctrl->lanes) &&
               (IS_GE_PORT(unit, port_base) && ((port_speed == 10000) || 
                                                (port_speed == 25000)))) {
        SOC_PBMP_PORT_REMOVE(si->ge.bitmap, port_base);
        SOC_PBMP_PORT_ADD(si->xe.bitmap, port_base);
        port_rename = 1;
    }

    if (port_rename) {
        _SOC_TH_RECONFIGURE_PORT_TYPE_INFO(xe);
        _SOC_TH_RECONFIGURE_PORT_TYPE_INFO(ce);
        _SOC_TH_RECONFIGURE_PORT_TYPE_INFO(ge);
        soc_dport_map_update(unit);
    }
    SOC_CONTROL_UNLOCK(unit);

    /* No need to change CLPORT mode during speed set */
    if (TH_FLEXPORT_SPEED_SET_NUM_LANES != lanes_ctrl->lanes) {
        /* Change CLPORT mode */
        fields[0] = XPORT0_CORE_PORT_MODEf;
        values[0] = lanes_ctrl->mode;
        fields[1] = XPORT0_PHY_PORT_MODEf;
        values[1] = lanes_ctrl->mode;
        SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, CLPORT_MODE_REGr,
                                                    port_base, 2, fields,
                                                    values));
    }

    /* Update TDM */
    SOC_IF_ERROR_RETURN
        (_soc_tomahawk_port_lanes_update_tdm(unit, lanes_ctrl));

    SOC_IF_ERROR_RETURN
        (soc_tomahawk_port_pg_headroom_update(unit, port_base));
    /* No need to change CA mode during speed set */
    if (TH_FLEXPORT_SPEED_SET_NUM_LANES != lanes_ctrl->lanes) {
        /* Reconfigure cell assembly port mode */
        tdm = SOC_CONTROL(unit)->tdm_info;
        pipe = si->port_pipe[port_base];
        clport = si->port_serdes[port_base];
        /* obm number is reversed (mirrored) in odd pipe */
        obm = pipe & 1 ? 7 - (clport & 0x7) : clport & 0x7;
        reg = SOC_REG_UNIQUE_ACC(unit, obm_ca_ctrl_regs[obm])[pipe];
        rval = 0;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, PORT_MODEf,
                          hw_mode_values[tdm->port_ratio[clport]]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_th_speed_group_sort(soc_th_speed_group_count_t *min_group_array, int len)
{
    int i, j;
    soc_th_speed_group_count_t tmp;

    sal_memset(&tmp, 0, sizeof(soc_th_speed_group_count_t));

    for (i = 1; i < len; i ++) {
        sal_memcpy(&tmp, &min_group_array[i], sizeof(soc_th_speed_group_count_t));
        for (j = i - 1 ; j >= 0 && min_group_array[j].count_ports > tmp.count_ports; j --) {
            sal_memcpy(&min_group_array[j + 1], &min_group_array[j], sizeof(soc_th_speed_group_count_t));
        }
        sal_memcpy(&min_group_array[j + 1], &tmp, sizeof(soc_th_speed_group_count_t));
    }
    return SOC_E_NONE;
}

STATIC int
soc_th_speed_group_consolidate_check(int unit, soc_th_port_lanes_t *lanes_ctrl, int flag)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t reg;
    int port_base;
    int inst;
    uint32 rval, fval;
    int pipe, group, slot, i;
    int speed_id, port_mode;
    int count_ports = 0;
    int idb_sum_count_ports = 0, mmu_sum_count_ports = 0;
    int idb_count_group = 0, mmu_count_group = 0;
    int idb_speed_group_ratio, mmu_speed_group_ratio;
    int tmp, pass1 = 0, pass2 = 0;

    soc_th_speed_group_count_t idb_min_group[_OVS_GROUP_COUNT];
    soc_th_speed_group_count_t mmu_min_group[_OVS_GROUP_COUNT];

    speed_id = flag ? lanes_ctrl->cur_ovs_class : lanes_ctrl->ovs_class;
    port_mode = flag ? lanes_ctrl->cur_mode : lanes_ctrl->mode;
    port_base = lanes_ctrl->port_base;
    pipe = si->port_pipe[port_base];

    for (group = 0; group < _OVS_GROUP_COUNT; group++) {
        reg = SOC_REG_UNIQUE_ACC(unit, IS_OVR_SUB_GRP_CFGr)[pipe];
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, group, &rval));
        fval = soc_reg_field_get(unit, reg, rval, SPEEDf);
        if (speed_id == fval) {
            reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
            for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, slot, &rval));
                if (soc_reg_field_get(unit, reg, rval, PORT_NUMf) != 0x3f) {
                    count_ports ++;
                }
            }

            idb_min_group[idb_count_group].group_id = group;
            idb_min_group[idb_count_group].count_ports = count_ports;
            idb_sum_count_ports += count_ports;
            idb_count_group ++;
        }
        count_ports = 0;
    }

    /* Sort the element of speed group and calculate the ratio of speed group */
    _soc_th_speed_group_sort(idb_min_group, idb_count_group);
    idb_speed_group_ratio = (idb_sum_count_ports - 1) / _OVS_GROUP_TDM_LENGTH + 1;
    tmp = idb_min_group[0].count_ports + idb_min_group[1].count_ports;

    if ((idb_speed_group_ratio < idb_count_group) &&
        (tmp <= _OVS_GROUP_TDM_LENGTH)) {
        /* Determine the source and destination of group for migration */
        lanes_ctrl->pipe = pipe;
        lanes_ctrl->speed_id = speed_id;
        lanes_ctrl->port_mode = port_mode;
        lanes_ctrl->src_idb_group = idb_min_group[0].group_id;
        lanes_ctrl->dst_idb_group = idb_min_group[1].group_id;
        pass1 = 1;
    }

    count_ports = 0;
    for (group = 0; group < _OVS_GROUP_COUNT; group++) {
        reg = OVR_SUB_GRP_CFGr;
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, group, &rval));
        fval = soc_reg_field_get(unit, reg, rval, SPEEDf);
        if (speed_id == fval) {
            reg = mmu_grp_tbl_regs[group];
            for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, inst, slot, &rval));
                if (soc_reg_field_get(unit, reg, rval, PORT_NUMf) != 0x3f) {
                    count_ports ++;
                }
            }

            mmu_min_group[mmu_count_group].group_id = group;
            mmu_min_group[mmu_count_group].count_ports = count_ports;
            mmu_sum_count_ports += count_ports;
            mmu_count_group ++;
        }
        count_ports = 0;
    }

    _soc_th_speed_group_sort(mmu_min_group, mmu_count_group);
    mmu_speed_group_ratio = (mmu_sum_count_ports - 1) / _OVS_GROUP_TDM_LENGTH + 1;
    tmp = mmu_min_group[0].count_ports + mmu_min_group[1].count_ports;

    if ((mmu_speed_group_ratio < mmu_count_group) &&
        (tmp <= _OVS_GROUP_TDM_LENGTH)) {
       lanes_ctrl->src_mmu_group = mmu_min_group[0].group_id;
       lanes_ctrl->dst_mmu_group = mmu_min_group[1].group_id;
       pass2 = 1;
    }

    if (pass1 && pass2) {
        lanes_ctrl->action_id = ACT_MIGRATE;
    }

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        LOG_CLI((BSL_META_U(unit,
                        ">>>> The idb min group info in pipe %d\n"), pipe));
        for (i=0; i < idb_count_group; i++) {
            LOG_CLI((BSL_META_U(unit,
                            "idb_min_group[%d]: g_id=%d c_ports=%d\n"),
                        i, idb_min_group[i].group_id, idb_min_group[i].count_ports));
        }
        LOG_CLI((BSL_META_U(unit, "min ratio: %d, cur ratio: %d\n"),
                    idb_speed_group_ratio, idb_count_group));
        if (idb_speed_group_ratio < idb_count_group) {
            LOG_CLI((BSL_META_U(unit, "speed group %d should consolidate\n"), speed_id));
            if (pass1) {
                LOG_CLI((BSL_META_U(unit, "speed group %d has sufficient empty slots\n"), speed_id));
            }
        } else {
            LOG_CLI((BSL_META_U(unit, "speed group %d need not consolidate\n"), speed_id));
        }

        LOG_CLI((BSL_META_U(unit,
                        ">>>> The mmu min group info in pipe %d\n"), pipe));
        for (i=0; i < mmu_count_group; i++) {
            LOG_CLI((BSL_META_U(unit,
                            "mmu_min_group[%d]: g_id=%d c_ports=%d\n"),
                        i, mmu_min_group[i].group_id, mmu_min_group[i].count_ports));
        }
        LOG_CLI((BSL_META_U(unit, "min ratio: %d, cur ratio: %d\n"),
                    mmu_speed_group_ratio, mmu_count_group));
        if (mmu_speed_group_ratio < mmu_count_group) {
            LOG_CLI((BSL_META_U(unit, "speed group %d should consolidate\n"), speed_id));
            if (pass2) {
                LOG_CLI((BSL_META_U(unit, "speed group %d has sufficient empty slots\n"), speed_id));
            }
        } else {
            LOG_CLI((BSL_META_U(unit, "speed group %d need not consolidate\n"), speed_id));
        }
    }

    return SOC_E_NONE;
}

STATIC int
soc_th_speed_group_migrate(int unit, soc_th_port_lanes_t *lanes_ctrl, int flag)
{
    int i;
    int p_pm = 0, speed_id, port_mode, pm_id = -1;
    int src_g, dst_g;
    int slot, src_slot, dst_slot;
    int pipe, inst;
    soc_reg_t src_reg, dst_reg;
    uint32 rval;
    int no_empty;
    soc_th_speed_group_slot_t entry[4];
    static char *flag_name[] = {"MMU", "IDB"};

    pipe = lanes_ctrl->pipe;
    inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
    speed_id = lanes_ctrl->speed_id;
    port_mode = lanes_ctrl->port_mode;

    src_g = flag ? lanes_ctrl->src_idb_group : lanes_ctrl->src_mmu_group;
    dst_g = flag ? lanes_ctrl->dst_idb_group : lanes_ctrl->dst_mmu_group;

    for (i = 0; i < soc_speed_group_info_size; i++) {
        if ((soc_speed_group_info[i].speed_id == speed_id) &&
            (soc_speed_group_info[i].port_mode == port_mode)) {
            p_pm = soc_speed_group_info[i].p_pm;
        }
    }

    /* Figure out each port in the PM */
    src_reg = flag ? SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[src_g])[pipe] : mmu_grp_tbl_regs[src_g];
    for (slot = 0, src_slot = 0; src_slot < _OVS_GROUP_TDM_LENGTH; src_slot ++) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, src_reg, flag ? REG_PORT_ANY : inst, src_slot, &rval));
        if (soc_reg_field_get(unit, src_reg, rval, PORT_NUMf) != 0x3f) {
            entry[slot].slot = src_slot;
            entry[slot].pm_id = soc_reg_field_get(unit, src_reg, rval, PHY_PORT_IDf);
            entry[slot].port_id = soc_reg_field_get(unit, src_reg, rval, PORT_NUMf);

            /* Checking all ports belonging to a PM */
            if (slot == 0) {
                pm_id = entry[0].pm_id;
            }
            if ((slot > 0) && (pm_id != entry[slot].pm_id)) {
                slot --;
                continue;
            }
            slot ++;
        }
        if (slot >= p_pm) {
            break;
        }
    }

    if (slot < p_pm) {
        lanes_ctrl->action_id = ACT_NONE;
        return SOC_E_NONE;
    }

    /* Configure the destination of group to contain all ports */
    rval = 0;
    slot = 0;
    dst_reg = flag ? SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[dst_g])[pipe] : mmu_grp_tbl_regs[dst_g];
    for (dst_slot = 0; dst_slot < _OVS_GROUP_TDM_LENGTH; dst_slot ++) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, dst_reg, flag ? REG_PORT_ANY : inst, dst_slot, &rval));
        if (soc_reg_field_get(unit, dst_reg, rval, PORT_NUMf) == 0x3f) {
            soc_reg_field_set(unit, dst_reg, &rval, PORT_NUMf, entry[slot].port_id);
            soc_reg_field_set(unit, dst_reg, &rval, PHY_PORT_IDf, entry[slot].pm_id);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, dst_reg, flag ? REG_PORT_ANY : inst, dst_slot, rval));
            slot ++;
        }
        if (slot >= p_pm) {
            break;
        }
    }

    /* Remove all ports from source group and reset the resource of slot */
    rval = 0;
    for (i = 0; i < p_pm; i++) {
        soc_reg_field_set(unit, src_reg, &rval, PORT_NUMf, 0x3f);
        soc_reg_field_set(unit, src_reg, &rval, PHY_PORT_IDf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, src_reg, flag ? REG_PORT_ANY : inst, entry[i].slot, rval));
    }

    /* Reset the resource of speed group */
    rval = 0;
    no_empty = 0;
    for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot ++) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, src_reg, flag ? REG_PORT_ANY : inst, slot, &rval));
        if (soc_reg_field_get(unit, src_reg, rval, PORT_NUMf) != 0x3f) {
            no_empty = 1;
        }
    }
    if (!no_empty) {
        src_reg = flag ? SOC_REG_UNIQUE_ACC(unit, IS_OVR_SUB_GRP_CFGr)[pipe] : OVR_SUB_GRP_CFGr;
        rval = 0;
        soc_reg_field_set(unit, src_reg, &rval, SAME_SPACINGf, 8);
        soc_reg_field_set(unit, src_reg, &rval, SISTER_SPACINGf, 4);
        soc_reg_field_set(unit, src_reg, &rval, SPEEDf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, src_reg, flag ? REG_PORT_ANY : inst, src_g, rval));
    }

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        LOG_CLI((BSL_META_U(unit,
                        "Migration on %s sp_id=%d port_mode=%d src_g=%d dst_g=%d DONE.\n"),
                    flag_name[flag], speed_id, port_mode, src_g, dst_g));
        for (i=0; i < p_pm; i++) {
            LOG_CLI((BSL_META_U(unit,
                            "slot[%d] {%d, %d} \n"),
                        entry[i].slot, entry[i].pm_id, entry[i].port_id));
        }
    }

    return SOC_E_NONE;
}

int
soc_tomahawk_port_lanes_consolidation(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
#define _TH_IDB_GROUP    1
#define _TH_MMU_GROUP    0
#define _TH_CUR_GROUP    1
#define _TH_NEW_GROUP    0
    int speed_id, port_mode, lanes;
    int co_enable;
    int oversub_mode;

    if (SOC_IS_TOMAHAWK(unit)) {
        oversub_mode = lanes_ctrl->oversub;
        speed_id = lanes_ctrl->cur_ovs_class;
        port_mode = lanes_ctrl->cur_mode;
        lanes = lanes_ctrl->lanes;

        if (speed_id == -1 || port_mode == -1 || lanes == -1) {
            return SOC_E_PARAM;
        }

        co_enable = soc_property_get(unit,
                                     spn_OVERSUB_SPEED_GROUP_CONSOLIDATION_ENABLE,
                                     0);
        if (!oversub_mode) {
            co_enable = 0;
        }

        if(co_enable) {
            /* Check whether the speed group requires consolidation */
            lanes_ctrl->action_id = ACT_CONSOLIDATE;
            SOC_IF_ERROR_RETURN
                (soc_th_speed_group_consolidate_check(unit,
                                                      lanes_ctrl,
                                                      _TH_CUR_GROUP));

            /* Migrate all ports to different OS group */
            if (lanes_ctrl->action_id == ACT_MIGRATE) {
                SOC_IF_ERROR_RETURN
                    (soc_th_speed_group_migrate(unit,
                                                lanes_ctrl,
                                                _TH_IDB_GROUP));
                SOC_IF_ERROR_RETURN
                    (soc_th_speed_group_migrate(unit,
                                                lanes_ctrl,
                                                _TH_MMU_GROUP));
            }
        } else {
            lanes_ctrl->action_id = ACT_NONE;
        }
    }

    return SOC_E_NONE;
}

soc_error_t
soc_th_port_speed_recover(int unit, soc_port_t port)
{
    soc_info_t *si = &SOC_INFO(unit);
    mac_driver_t *macdp;
    int port_speed = 0;
    int port_rename = 0;
    soc_port_t it_port;

    if (!IS_HG_PORT(unit, port) && !IS_CE_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &macdp));
    SOC_IF_ERROR_RETURN(MAC_SPEED_GET(macdp, unit, port, &port_speed));
    SOC_CONTROL_LOCK(unit);

    si->port_init_speed[port] = port_speed;
    /* Update bitmap of high speed ports */
    if (port_speed >= 40000) {
        SOC_PBMP_PORT_ADD(si->eq_pbm, port);
    } else {
        SOC_PBMP_PORT_REMOVE(si->eq_pbm, port);
    }
    /* Adjust XE/CE port name */
    if (IS_E_PORT(unit, port)) {
        if(port_speed < 100000) {
            SOC_PBMP_PORT_REMOVE(si->ce.bitmap, port);
            SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
        } else {
            SOC_PBMP_PORT_ADD(si->ce.bitmap, port);
            SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
        }
        port_rename = 1;
    }
    if (port_rename) {
        _SOC_TH_RECONFIGURE_PORT_TYPE_INFO(xe);
        _SOC_TH_RECONFIGURE_PORT_TYPE_INFO(ce);
        soc_dport_map_update(unit);
    }

    SOC_CONTROL_UNLOCK(unit);
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th_flexport_recover
 * Description:
 *      Add port to xe/ce and high speed port bitmap by reading
 *      port speed.
 * Parameters:
 *      unit      - Device number
 *      port      - logical port number
 */
soc_error_t
soc_th_flexport_recover(int unit, soc_port_t port)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_t it_port;
    mac_driver_t *macdp;
    int port_speed = 0, port_rename = 0;

    SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &macdp));
    SOC_IF_ERROR_RETURN(MAC_SPEED_GET(macdp, unit, port, &port_speed));
    SOC_CONTROL_LOCK(unit);
    si->port_init_speed[port] = port_speed;
    /* Update bitmap of high speed ports */
    if (port_speed >= 40000) {
        SOC_PBMP_PORT_ADD(si->eq_pbm, port);
    } else {
        SOC_PBMP_PORT_REMOVE(si->eq_pbm, port);
    }
    /* Adjust XE/CE port bitmaps */
    if (IS_E_PORT(unit, port)) {
        if (port_speed < 100000) {
            SOC_PBMP_PORT_REMOVE(si->ce.bitmap, port);
            SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
            port_rename = 1;
        } else {
            SOC_PBMP_PORT_ADD(si->ce.bitmap, port);
            SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
            port_rename = 1;
        }
    }
    if (port_rename) {
        _SOC_TH_RECONFIGURE_PORT_TYPE_INFO(xe);
        _SOC_TH_RECONFIGURE_PORT_TYPE_INFO(ce);
        soc_dport_map_update(unit);
    }
    SOC_CONTROL_UNLOCK(unit);

    return SOC_E_NONE;
}

soc_error_t
soc_th_granular_speed_get(int unit, soc_port_t port, int *speed)
{
    uint32 fval, rval;

#if defined(BCM_TOMAHAWK2_SUPPORT)
    egr_mmu_cell_credit_entry_t credit_entry;
    if (soc_feature(unit, soc_feature_egr_mmu_cell_credit_is_memory)) {
        SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITm(unit, MEM_BLOCK_ANY,
                            SOC_INFO(unit).port_l2p_mapping[port], &credit_entry));
        fval = soc_EGR_MMU_CELL_CREDITm_field32_get(unit, &credit_entry, CREDITf);
    } else
#endif
    {
        SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITr(unit, port, &rval));
        fval = soc_reg_field_get(unit, EGR_MMU_CELL_CREDITr, rval, CREDITf);
    }
    SOC_IF_ERROR_RETURN(soc_th_port_asf_mmu_cell_credit_to_speed
                           (unit, port, (uint8) fval, speed));
    return SOC_E_NONE;
}

void
soc_tomahawk_temperature_intr(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4)
{
    uint32 rval, trval;
    uint32 cur_temp, max_temp;
    int i, rv, unit = PTR_TO_INT(unit_vp);
    soc_port_t port;
    soc_block_t blk;
    int spurious_intr = 0;

    cli_out("Interrupt stat3: 0x%08x\n", soc_pci_read(unit, 0x3140c));

    if ((rv = READ_TOP_PVTMON_INTR_STATUSr(unit, &rval)) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "TH Temp Intr, Reg access error.\n")));
    }
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "High temp interrupt: 0x%08x\n"), rval));
    if (rval == 0) {
        spurious_intr = 1;
    }

    /* Raise event to app for it to take further graceful actions */
    for (i = 0; i < COUNTOF(pvtmon_result_reg); i++) {
        if ((rv = soc_reg32_get(unit, pvtmon_result_reg[i], REG_PORT_ANY, 0,
                                &trval)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, Reg access error.\n")));
        }
        cur_temp = soc_reg_field_get(unit, pvtmon_result_reg[i], trval,
                                     PVT_DATAf);
        max_temp = soc_reg_field_get(unit, pvtmon_result_reg[i], trval,
                                     MIN_PVT_DATAf);
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            /* Temp = 434.10 - o_ADC_data * 0.53504
             * data = (434.10 - temp)/0.53504 = (434100-(temp*1000))/535
             * Note: Since this is a high temp value we can safely assume it to
             * always be a +ive number. This is in degrees celcius.
             */
            cur_temp = (434100-(cur_temp*535))/1000;
            max_temp = (434100-(max_temp*535))/1000;
        } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
        {
            /* Convert data to temperature.
             * temp = 410.04-(data*0.48705) = (410040-(data*487))/1000
             * Note: Since this is a high temp interrupt we can safely assume
             * that this will end up being a +ive value.
             */
            cur_temp = (410040-(cur_temp*487))/1000;
            max_temp = (410040-(max_temp*487))/1000;
        }
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "TempMon %d: raw value 0x%08x, current %d deg C, maximum %d deg C.\n"),
                   i, trval, cur_temp, max_temp));

        if ((0x1 << i) & _soc_th_temp_mon_mask[unit]) {
            (void)soc_event_generate(unit, SOC_SWITCH_EVENT_ALARM,
                                     SOC_SWITCH_EVENT_ALARM_HIGH_TEMP, i, cur_temp);
        }
    }

    if (spurious_intr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Spurious interrupt raised, will not shut down.\n")));
        /* Enable temp mon interrupt */
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            (void)soc_cmicm_intr3_enable(unit, 0x4); /* PVTMON_INTR bit 2 */
        }
#endif
    }

    /* Optionally hold system in reset. TH2 will skip as it has HW overtemp protection.
     * Note: The main intention is to stop the chip from getting fried and halt
     *       all schan accesses as the h/w will not respond anymore.
     *       We are not implementing a gracefull recovery, the unit needs to be
     *       rebooted after this.
     */
    if ((!SOC_IS_TOMAHAWK2(unit)) && !spurious_intr &&
        (soc_property_get(unit, "temp_monitor_shutdown", 1))) {
        if ((rv = WRITE_TOP_PVTMON_INTR_MASKr(unit, 0)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, Reg access error.\n")));
        }

        /* Stop all schan transactions on this unit */
#ifdef  INCLUDE_I2C
        if ((rv = soc_i2c_detach(unit)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, i2c detach error.\n")));
        }
#endif
        if (!SOC_IS_RCPU_ONLY(unit)) {
            /* Free up DMA memory */
            if ((rv = soc_dma_detach(unit)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH Temp Intr, dma detach error.\n")));
            }
        }
#ifdef INCLUDE_MEM_SCAN        
        if ((rv = soc_mem_scan_stop(unit)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, mem scan stop error.\n")));
        }
#endif /* INCLUDE_MEM_SCAN */      
        /* Terminate counter module */
        if ((rv = soc_counter_stop(unit)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, counter stop error.\n")));
        }
        if (SOC_SBUSDMA_DM_INFO(unit)) {
            if ((rv = soc_sbusdma_desc_detach(unit)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH Temp Intr, sbusdma stop error.\n")));
            }
        }
        if (soc_feature(unit, soc_feature_arl_hashed)) {
            /* Stop L2X thread */
            if ((rv = soc_l2x_stop(unit)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH Temp Intr, L2x stop error.\n")));
            }

        }
        if ((rv = soc_th_l2_bulk_age_stop(unit)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, L2 age stop error.\n")));
        }

        rv = READ_TOP_SOFT_RESET_REGr(unit, &rval);
        if (SOC_E_NONE != rv) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, Reg access error.\n")));
        }
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TS_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 0);
        rv = WRITE_TOP_SOFT_RESET_REGr(unit, rval);
        if (SOC_E_NONE != rv) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, Reg access error.\n")));
        }

        if ((rv = READ_TOP_SOFT_RESET_REG_2r(unit, &rval)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, Reg access error.\n")));
        }

#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL0_RST_Lf,
                              0);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf,
                              1);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf,
                              0);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf,
                              0);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_RST_Lf, 1);
        } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
        {
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL0_RST_Lf,
                              0);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL1_RST_Lf,
                              0);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL2_RST_Lf,
                              0);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL3_RST_Lf,
                              0);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf,
                              1);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf,
                              0);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf,
                              0);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_RST_Lf, 1);
        }

        if ((rv = WRITE_TOP_SOFT_RESET_REG_2r(unit, rval)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, Reg access error.\n")));
        }
        /* Powerdown Falcon analog cores */
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
            port = SOC_BLOCK_PORT(unit, blk);
            rv = READ_CLPORT_XGXS0_CTRL_REGr(unit, port, &rval);
            if (SOC_E_NONE != rv) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH Temp Intr, Reg access error.\n")));
            }
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &rval, IDDQf, 1);
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &rval, PWRDWNf, 1);
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &rval, RSTB_HWf, 0);
            rv = WRITE_CLPORT_XGXS0_CTRL_REGr(unit, port, rval);
            if (SOC_E_NONE != rv) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH Temp Intr, Reg access error.\n")));
            }
        }
        /* Powerdown Eagle analog cores */
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
            port = SOC_BLOCK_PORT(unit, blk);
            rv = READ_XLPORT_XGXS0_CTRL_REGr(unit, port, &rval);
            if (SOC_E_NONE != rv) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH Temp Intr, Reg access error.\n")));
            }
            soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval, IDDQf, 1);
            soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval, PWRDWNf, 1);
            soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval, RSTB_HWf, 0);
            rv = WRITE_XLPORT_XGXS0_CTRL_REGr(unit, port, rval);
            if (SOC_E_NONE != rv) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH Temp Intr, Reg access error.\n")));
            }
        }

        /* Disable all interrupts */
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            soc_cmicm_intr0_disable(unit, ~0);
            soc_cmicm_intr1_disable(unit, ~0);
            soc_cmicm_intr2_disable(unit, ~0);
            soc_cmicm_intr3_disable(unit, ~0);
            soc_cmicm_intr4_disable(unit, ~0);
            soc_cmicm_intr5_disable(unit, ~0);
        }
#endif

#ifdef BCM_CMICDV4_SUPPORT
        if (soc_feature(unit, soc_feature_cmicd_v4)) {
            soc_cmicm_intr6_disable(unit, ~0);
        }
#endif /* BCM_CMICDV4_SUPPORT */

        /* Power down core PLL, once this is done all SCHAN accesses
         * will fail.
         */
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            rv = READ_TOP_CORE_PLL0_CTRL_6r(unit, &rval);
            if (SOC_E_NONE != rv) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH2 Temp Intr, Reg access error.\n")));
            }
            soc_reg_field_set(unit, TOP_CORE_PLL0_CTRL_6r, &rval, ISO_INANDOUT_AONf, 1);
            soc_reg_field_set(unit, TOP_CORE_PLL0_CTRL_6r, &rval, PWRON_PLL_AONf, 0);
            soc_reg_field_set(unit, TOP_CORE_PLL0_CTRL_6r, &rval, PWRON_LDO_ANALOG_AONf, 0);
            WRITE_TOP_CORE_PLL0_CTRL_6r(unit, rval);
        } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
        {
            rv = READ_TOP_CORE_PLL0_CTRL_1r(unit, &rval);
            if (SOC_E_NONE != rv) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH Temp Intr, Reg access error.\n")));
            }
            soc_reg_field_set(unit, TOP_CORE_PLL0_CTRL_1r, &rval, ISO_INf, 1);
            soc_reg_field_set(unit, TOP_CORE_PLL0_CTRL_1r, &rval, PWRONf, 0);
            soc_reg_field_set(unit, TOP_CORE_PLL0_CTRL_1r, &rval, PWRON_LDOf, 0);
            WRITE_TOP_CORE_PLL0_CTRL_1r(unit, rval);
        }
        SCHAN_LOCK(unit);

        /* Clear all outstanding DPCs owned by this unit */
        sal_dpc_cancel(INT_TO_PTR(unit));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "\nReboot the system ...\n")));
    }
}

int
soc_tomahawk_temperature_monitor_get(int unit, int temperature_max,
    soc_switch_temperature_monitor_t *temperature_array, int *temperature_count)
{
    soc_reg_t reg;
    int index;
    uint32 rval;
    int fval, cur, peak;
    int num_entries_out;

    if (temperature_count) {
        *temperature_count = 0;
    }
    if (COUNTOF(pvtmon_result_reg) > temperature_max) {
        num_entries_out = temperature_max;
    } else {
        num_entries_out = COUNTOF(pvtmon_result_reg);
    }
    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    if ((soc_reg_field_get(unit, TOP_PVTMON_CTRL_1r, rval, PVTMON_RESET_Nf) == 0) ||
        (soc_reg_field_get(unit, TOP_PVTMON_CTRL_1r, rval, PVTMON_SELECTf) != 0)) {
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RESET_Nf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RESET_Nf, 1);
        soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_SELECTf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
        sal_usleep(1000);
    }

    for (index = 0; index < num_entries_out; index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        fval = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            cur = (4341000-(fval*5350))/1000;
        } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
        {
            cur = (4100400-(fval*4870))/1000;
        }
        fval = soc_reg_field_get(unit, reg, rval, MIN_PVT_DATAf);
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            peak = (4341000-(fval*5350))/1000;
        } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
        {
            peak = (4100400-(fval*4870))/1000;
        }
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
        *temperature_count = num_entries_out;
    }
    return SOC_E_NONE;
}

int
soc_tomahawk_show_voltage(int unit)
{
    soc_reg_t reg;
    int index;
    uint32 rval, fval, avg;

    if (!SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
        if ((soc_reg_field_get(unit, TOP_PVTMON_CTRL_1r, rval, PVTMON_RESET_Nf) == 0) ||
            (soc_reg_field_get(unit, TOP_PVTMON_CTRL_1r, rval, PVTMON_SELECTf) != 4)) {
            soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RESET_Nf, 0);
            SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
            soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_RESET_Nf, 1);
            soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_SELECTf, 4);
            SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
            sal_usleep(1000);
        }
    }

    avg = 0;

    /* Read Voltages. */
    for (index = 0; index < COUNTOF(pvtmon_result_reg); index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        fval = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit) || SOC_IS_TOMAHAWK3(unit)) {
            /* (PVT_DATA * 950) / (1024 * 0.8) */
            fval = (fval * 950 * 10) / (1024 * 8);
        } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
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

    avg /= (COUNTOF(pvtmon_result_reg));
    LOG_CLI((BSL_META_U(unit,
                        "Average voltage is = %d.%03dV\n"),
             (avg/1000), (avg %1000)));

    return SOC_E_NONE;
}

int
soc_tomahawk_show_ring_osc(int unit)
{
    static const struct {
        int osc_sel;
        soc_field_t field0;
        int value0;
        soc_field_t field1;
        int value1;
        char *name;
    } osc_tbl[] = {
        { 2, IROSC_SELf, 0, INVALIDf, -1, "Core ring 0 five stages" },
        { 2, IROSC_SELf, 1, INVALIDf, -1, "Core ring 0 nine stages" },
        { 3, IROSC_SELf, 0, INVALIDf, -1, "Core ring 1 five stages" },
        { 3, IROSC_SELf, 1, INVALIDf, -1, "Core ring 1 nine stages" },
        { 4, SRAM_OSC_0_PENf, 0, SRAM_OSC_0_NENf, 1, "SRAM ring 0 NMOS" },
        { 5, SRAM_OSC_0_PENf, 1, SRAM_OSC_0_NENf, 0, "SRAM ring 0 PMOS" },
        { 6, SRAM_OSC_1_PENf, 0, SRAM_OSC_1_NENf, 1, "SRAM ring 1 NMOS" },
        { 7, SRAM_OSC_1_PENf, 1, SRAM_OSC_1_NENf, 0, "SRAM ring 1 PMOS" },
        { 8, SRAM_OSC_2_PENf, 0, SRAM_OSC_2_NENf, 1, "SRAM ring 2 NMOS" },
        { 9, SRAM_OSC_2_PENf, 1, SRAM_OSC_2_NENf, 0, "SRAM ring 2 PMOS" },
        { 10, SRAM_OSC_3_PENf, 0, SRAM_OSC_3_NENf, 1, "SRAM ring 3 NMOS" },
        { 11, SRAM_OSC_3_PENf, 1, SRAM_OSC_3_NENf, 0, "SRAM ring 3 PMOS" }
    };
    soc_reg_t ctrl_reg, stat_reg;
    uint32 rval, fval;
    int index, core_clk, quo, rem, frac, retry;

    core_clk = SOC_INFO(unit).frequency * 1024;
    ctrl_reg = TOP_RING_OSC_CTRLr;
    stat_reg = TOP_OSC_COUNT_STATr;

    for (index = 0; index < COUNTOF(osc_tbl); index++) {
        rval = 0;

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit) || (SOC_IS_TOMAHAWK3(unit))) {
            if (SRAM_OSC_1_PENf == osc_tbl[index].field0 ||
                SRAM_OSC_2_PENf == osc_tbl[index].field0 ||
                SRAM_OSC_3_PENf == osc_tbl[index].field0) {
                continue;
            }
        }
#endif

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

STATIC _soc_th_bst_hw_cb th_bst_cb;

int
_soc_th_process_mmu_bst(int unit, int xpe, soc_field_t field)
{
    int rv = SOC_E_NONE;
    if (th_bst_cb != NULL) {
        rv = th_bst_cb(unit);
    }
    return rv;
}

int soc_th_set_bst_callback(int unit, _soc_th_bst_hw_cb cb)
{
    th_bst_cb = cb;
    return SOC_E_NONE;
}

void
soc_tomahawk_process_func_intr(void *unit_vp, void *d1, void *d2, void *d3, void *d4)
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
        (void) soc_tomahawk_process_ser_fifo(unit, SOC_BLK_IPIPE,
                                             0, /* pipe */
                                             prefix_str,
                                             1, 0, 0); /* l2_mgmt_ser_fifo */
    }

    if (soc_feature(unit, soc_feature_l2_overflow)) {
        if (soc_reg_field_get(unit, L2_MGMT_INTRr, rval, L2_LEARN_INSERT_FAILUREf)) {
            soc_td2_l2_overflow_interrupt_handler(unit);
        }
    }

    sal_usleep(SAL_BOOT_QUICKTURN ? 100000 : 1000); /* Don't reenable too soon */
    if (d1 != NULL) {
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            (void) soc_cmicm_intr3_enable(unit, PTR_TO_INT(d1));
        }
#endif
    }
}

/*
 * All non-parity related interrupts must be handled in this function.
 * All unserviced intr_status bits must either be serviced or disabled.
 *
 * Note:
 * 1. MEM_PAR_ERR_STATf is handled by tomahawk/ser.c, so ignore this
 * 2. blocktype (in) can only be SOC_BLK_MMU_XPE/SC
 */
int
soc_th_mmu_non_ser_intr_handler(int unit, soc_block_t blocktype,
                                int mmu_base_index,
                                uint32 rval_intr_status_reg)
{
    int i;
    uint32 rval = 0;
    soc_reg_t intr_stat_reg, intr_clr_reg;
    static soc_field_t xpe_intr_field_list[] = {
        MEM_PAR_ERR_STATf, /* handled by tomahawk/ser.c, so ignore this */
        BST_THDI_INT_STATf,
        BST_THDO_INT_STATf,
        DEQ0_NOT_IP_ERR_STATf,
        DEQ1_NOT_IP_ERR_STATf,
        INVALIDf
    };
    static soc_field_t sc_intr_field_list[] = {
        CFAP_A_MEM_FAIL_STATf,
        CFAP_B_MEM_FAIL_STATf,
        MEM_PAR_ERR_STATf, /* handled by tomahawk/ser.c, so ignore this */
        BST_CFAP_A_INT_STATf,
        BST_CFAP_B_INT_STATf,
        ES_X_DEADLOCK_DET_INT_STATf,
        ES_Y_DEADLOCK_DET_INT_STATf,
        START_BY_START_ERR_STATf,
        ES_X_1IN4_ERR_INT_STATf,
        ES_Y_1IN4_ERR_INT_STATf,
        INVALIDf
    };

    if (blocktype == SOC_BLK_MMU_XPE) {
        intr_stat_reg = MMU_XCFG_XPE_CPU_INT_STATr;
        intr_clr_reg = MMU_XCFG_XPE_CPU_INT_CLEARr;
        for (i = 0; xpe_intr_field_list[i] != INVALIDf; i++) {
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
                    (soc_tomahawk_xpe_reg32_get(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, &rval));
                rval |= (1 << i);
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_xpe_reg32_set(unit, intr_clr_reg,
                                                mmu_base_index, mmu_base_index,
                                                0, rval));
                SOC_IF_ERROR_RETURN
                    (_soc_th_process_mmu_bst(unit, mmu_base_index,
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

    if (blocktype == SOC_BLK_MMU_SC) {
        intr_stat_reg = MMU_SCFG_SC_CPU_INT_STATr;
        intr_clr_reg = MMU_SCFG_SC_CPU_INT_CLEARr;
        for (i = 0; sc_intr_field_list[i] != INVALIDf; i++) {
            if (!soc_reg_field_get(unit, intr_stat_reg,
                                   rval_intr_status_reg,
                                   sc_intr_field_list[i])) {
                continue;
            }
            switch (sc_intr_field_list[i]) {
            case BST_CFAP_A_INT_STATf:
            case BST_CFAP_B_INT_STATf:
                /* interrupt processing */
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Serviced %s intr "
                                              "from sc = %0d \n"),
                             unit, SOC_FIELD_NAME(unit, sc_intr_field_list[i]),
                             mmu_base_index));


                /* clear the interrupt */
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_sc_reg32_get(unit, intr_clr_reg,
                                               mmu_base_index, mmu_base_index,
                                               0, &rval));
                rval |= (1 << i);
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_sc_reg32_set(unit, intr_clr_reg,
                                               mmu_base_index, mmu_base_index,
                                               0, rval));
                SOC_IF_ERROR_RETURN
                    (_soc_th_process_mmu_bst(unit, mmu_base_index,
                                             sc_intr_field_list[i]));

                break;
            case CFAP_A_MEM_FAIL_STATf:
            case CFAP_B_MEM_FAIL_STATf:
                /* interrupt processing */
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "Unit: %0d -- Serviced %s intr "
                                              "from sc = %0d \n"),
                             unit, SOC_FIELD_NAME(unit, sc_intr_field_list[i]),
                             mmu_base_index));

                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_CFAP_MEM_FAIL, -1, -1);

                /* clear the interrupt */
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_sc_reg32_get(unit, intr_clr_reg,
                                               mmu_base_index, mmu_base_index,
                                               0, &rval));
                rval |= (1 << i);
                SOC_IF_ERROR_RETURN
                    (soc_tomahawk_sc_reg32_set(unit, intr_clr_reg,
                                               mmu_base_index, mmu_base_index,
                                               0, rval));
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "Unit: %0d -- Could not service %s "
                                            "intr from sc = %0d \n"),
                           unit, SOC_FIELD_NAME(unit, sc_intr_field_list[i]),
                           mmu_base_index));
                break;
            }
        }
        return SOC_E_NONE;
    }

    /* cannot be here for any other blocktype */
    return SOC_E_FAIL;
}

STATIC int
_soc_th_mmu_config_shared_update_check(int val1, int val2, int flags)
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

#define TH_MMU_SHARED_LIMIT_CHK(val1, val2, flags) \
    _soc_th_mmu_config_shared_update_check((val1), (val2), (flags))

int
soc_th_mmu_config_shared_buf_recalc(int unit, int res, 
                        int *thdi_shd, int *thdo_db_shd, int *thdo_qe_shd,
                        int post_update)
{
    soc_info_t *si;
    int pipe, xpe, xpe_map;
    uint32 entry0[SOC_MAX_MEM_WORDS], cur_limit, rval = 0, rval2 = 0;
    thdi_port_pg_config_entry_t pg_config_mem;
    mmu_thdo_config_qgroup_entry_t cfg_qgrp;
    int port, base, numq, idx, midx;
    soc_mem_t base_mem = INVALIDm;
    soc_mem_t mem = INVALIDm;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    int granularity;
    int thdi_shd_min, thdo_db_shd_min, thdo_qe_shd_min;

    si = &SOC_INFO(unit);

    xpe_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
    
    _TH_ARRAY_MIN(thdi_shd, NUM_XPE(unit), xpe_map, thdi_shd_min);
    _TH_ARRAY_MIN(thdo_db_shd, NUM_XPE(unit), xpe_map, thdo_db_shd_min);
    _TH_ARRAY_MIN(thdo_qe_shd, NUM_XPE(unit), xpe_map, thdo_qe_shd_min);
    
    /***********************************
     * THDI
 */
    if (res & 0x1) { /* if (res & THDI)  */

        /* THDI SP entries */
        rval = 0;
        reg = THDI_BUFFER_CELL_LIMIT_SPr;
        field = LIMITf;
        granularity = 1;
        for (xpe = 0; xpe < NUM_XPE(unit); xpe ++) {
            if (!(xpe_map & (1 << xpe))) {
                continue;
            }
        
            for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
                SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_get(unit, 
                    reg, xpe, -1, idx, &rval));
                cur_limit = soc_reg_field_get(unit, reg, rval, field);
                if ((cur_limit != 0) &&
                    TH_MMU_SHARED_LIMIT_CHK(cur_limit, thdi_shd[xpe]/granularity,
                                            post_update)) {
                    soc_reg_field_set(unit, reg, &rval, field, (thdi_shd[xpe]/granularity));
                    SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
                        reg, xpe, -1, idx, rval));
                }
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
            for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
                midx = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port, base_mem, idx);
                sal_memset(&pg_config_mem, 0, sizeof(pg_config_mem));
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                  midx, &pg_config_mem));
                if (!soc_mem_field32_get(unit, mem, &pg_config_mem,
                                         PG_SHARED_DYNAMICf)) {
                    cur_limit = soc_mem_field32_get(unit, mem, &pg_config_mem,
                                                    field);
                    if ((cur_limit != 0) &&
                        TH_MMU_SHARED_LIMIT_CHK(cur_limit, thdi_shd_min/granularity,
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, &pg_config_mem,
                                            field, thdi_shd_min/granularity);
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
    if (res & 0x2) { /* if (res & THDO)  */
        /* Per SP settings */
        for (xpe = 0; xpe < NUM_XPE(unit); xpe ++) {
            if (!(xpe_map & (1 << xpe))) {
                continue;
            }

            for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
                /* DB entries */
                rval = 0;
                reg = MMU_THDM_DB_POOL_SHARED_LIMITr;
                field = SHARED_LIMITf;
                granularity = 1;
                SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_get(unit, 
                    reg, xpe, -1, idx, &rval));
                cur_limit = soc_reg_field_get(unit, reg, rval, field);
                if ((cur_limit != 0) &&
                    TH_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_db_shd[xpe]/granularity,
                                            post_update)) {
                    soc_reg_field_set(unit, reg, &rval, field, (thdo_db_shd[xpe]/granularity));
                    SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
                        reg, xpe, -1, idx, rval));
                }

                rval = 0;
                reg = MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr;
                field = YELLOW_SHARED_LIMITf;
                granularity = 8;
                SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_get(unit, 
                    reg, xpe, -1, idx, &rval));
                cur_limit = soc_reg_field_get(unit, reg, rval, field);
                if ((cur_limit != 0) &&
                    TH_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd[xpe], granularity),
                                            post_update)) {
                    soc_reg_field_set(unit, reg, &rval, field, sal_ceil_func(thdo_db_shd[xpe], granularity));
                    SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
                        reg, xpe, -1, idx, rval));
                }

                rval = 0;
                reg = MMU_THDM_DB_POOL_RED_SHARED_LIMITr;
                field = RED_SHARED_LIMITf;
                granularity = 8;
                SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_get(unit, 
                    reg, xpe, -1, idx, &rval));
                cur_limit = soc_reg_field_get(unit, reg, rval, field);
                if ((cur_limit != 0) &&
                    TH_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd[xpe], granularity),
                                            post_update)) {
                    soc_reg_field_set(unit, reg, &rval, field, sal_ceil_func(thdo_db_shd[xpe], granularity));
                    SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
                        reg, xpe, -1, idx, rval));
                }

                rval = 0;
                reg = MMU_THDM_DB_POOL_RESUME_LIMITr;
                field = RESUME_LIMITf;
                granularity = 8;
                SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_get(unit,
                    reg, xpe, -1, idx, &rval));
                cur_limit = soc_reg_field_get(unit, reg, rval, field);
                if ((cur_limit != 0) &&
                    TH_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd[xpe], granularity),
                                            post_update)) {
                    soc_reg_field_set(unit, reg, &rval, field, sal_ceil_func(thdo_db_shd[xpe], granularity));
                    SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit,
                        reg, xpe, -1, idx, rval));
                }

                rval = 0;
                reg = MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr;
                field = YELLOW_RESUME_LIMITf;
                granularity = 8;
                SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_get(unit,
                    reg, xpe, -1, idx, &rval));
                cur_limit = soc_reg_field_get(unit, reg, rval, field);
                if ((cur_limit != 0) &&
                    TH_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd[xpe], granularity),
                                            post_update)) {
                    soc_reg_field_set(unit, reg, &rval, field, sal_ceil_func(thdo_db_shd[xpe], granularity));
                    SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit,
                        reg, xpe, -1, idx, rval));
                }

                rval = 0;
                reg = MMU_THDM_DB_POOL_RED_RESUME_LIMITr;
                field = RED_RESUME_LIMITf;
                granularity = 8;
                SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_get(unit,
                    reg, xpe, -1, idx, &rval));
                cur_limit = soc_reg_field_get(unit, reg, rval, field);
                if ((cur_limit != 0) &&
                    TH_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd[xpe], granularity),
                                            post_update)) {
                    soc_reg_field_set(unit, reg, &rval, field, sal_ceil_func(thdo_db_shd[xpe], granularity));
                    SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit,
                        reg, xpe, -1, idx, rval));
                }

                /* MCQE */
                rval = 0;
                reg = MMU_THDM_MCQE_POOL_SHARED_LIMITr;
                field = SHARED_LIMITf;
                granularity = 4;
                SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_get(unit, 
                    reg, xpe, -1, idx, &rval));
                cur_limit = soc_reg_field_get(unit, reg, rval, field);
                if ((cur_limit != 0) &&
                    TH_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_qe_shd[xpe]/granularity,
                                            post_update)) {
                    soc_reg_field_set(unit, reg, &rval, field,
                                      (thdo_qe_shd[xpe]/granularity));
                    SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
                        reg, xpe, -1, idx, rval));
                }

                rval = 0;
                reg = MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr;
                field = YELLOW_SHARED_LIMITf;
                granularity = 8;
                SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_get(unit, 
                    reg, xpe, -1, idx, &rval));
                cur_limit = soc_reg_field_get(unit, reg, rval, field);
                if ((cur_limit != 0) &&
                    TH_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_qe_shd[xpe]/granularity,
                                            post_update)) {
                    soc_reg_field_set(unit, reg, &rval, field,
                                      (thdo_qe_shd[xpe]/granularity));
                    SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
                        reg, xpe, -1, idx, rval));
                }

                rval = 0;
                reg = MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr;
                field = RED_SHARED_LIMITf;
                granularity = 8;
                SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_get(unit, 
                    reg, xpe, -1, idx, &rval));
                cur_limit = soc_reg_field_get(unit, reg, rval, field);
                if ((cur_limit != 0) &&
                    TH_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_qe_shd[xpe]/granularity,
                                            post_update)) {
                    soc_reg_field_set(unit, reg, &rval, field,
                                      (thdo_qe_shd[xpe]/granularity));
                    SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
                        reg, xpe, -1, idx, rval));
                }
            }
        }

        /* configure Q-groups */
        base_mem = MMU_THDU_CONFIG_QGROUPm;
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            for (idx = 0; idx < SOC_TH_MMU_CFG_QGROUP_MAX; idx++) {
                sal_memset(&cfg_qgrp, 0, sizeof(cfg_qgrp));

                SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, idx, &cfg_qgrp));

                field = Q_SHARED_LIMIT_CELLf;
                granularity = 1;
                if (!soc_mem_field32_get(unit, mem, &cfg_qgrp, Q_LIMIT_DYNAMIC_CELLf)) {
                    cur_limit = soc_mem_field32_get(unit, mem, &cfg_qgrp, field);
                    if (TH_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_db_shd_min/granularity,
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, &cfg_qgrp,
                                            field, thdo_db_shd_min/granularity);
                    }
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, &cfg_qgrp,
                                         Q_COLOR_LIMIT_DYNAMIC_CELLf)) {
                    field = LIMIT_RED_CELLf;
                    cur_limit = soc_mem_field32_get(unit, mem, &cfg_qgrp, field);
                    if (TH_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_db_shd_min/granularity,
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, &cfg_qgrp,
                                            field, thdo_db_shd_min/granularity);
                    }

                    field = LIMIT_YELLOW_CELLf;
                    cur_limit = soc_mem_field32_get(unit, mem, &cfg_qgrp, field);
                    if (TH_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_db_shd_min/granularity,
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, &cfg_qgrp,
                                            field, thdo_db_shd_min/granularity);
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
                    if (TH_MMU_SHARED_LIMIT_CHK(cur_limit, thdo_db_shd_min/granularity,
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_db_shd_min/granularity);
                    }
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMIC_CELLf)) {
                    field = LIMIT_YELLOW_CELLf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (TH_MMU_SHARED_LIMIT_CHK(cur_limit, sal_ceil_func(thdo_db_shd_min, granularity),
                                                post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, sal_ceil_func(thdo_db_shd_min, granularity));
                    }
                    field = LIMIT_RED_CELLf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (TH_MMU_SHARED_LIMIT_CHK(cur_limit,
                                sal_ceil_func(thdo_db_shd_min, granularity), post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, sal_ceil_func(thdo_db_shd_min, granularity));
                    }
                }
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, MEM_BLOCK_ALL, base + idx, entry0));
            }
        }

        /* Output port per port per queue setting for MC queue */
        PBMP_PORT_ITER(unit, port) {
            /* DB entries - Queue */
            numq = si->port_num_cosq[port];
            base = si->port_cosq_base[port];
            if (numq == 0) {
                continue;
            }

            base_mem = MMU_THDM_DB_QUEUE_CONFIGm;
            pipe = si->port_pipe[port];
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];

            for (idx = 0; idx < numq; idx++) {
                sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_config_entry_t));

                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL, base + idx, entry0));

                field = Q_SHARED_LIMITf;
                granularity = 1;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_LIMIT_DYNAMICf)) {
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (TH_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_db_shd_min/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_db_shd_min/granularity);
                    }
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMICf)) {
                    field = YELLOW_SHARED_LIMITf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (TH_MMU_SHARED_LIMIT_CHK(cur_limit,
                                sal_ceil_func(thdo_db_shd_min, granularity), post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, sal_ceil_func(thdo_db_shd_min, granularity));
                    }
                    field = RED_SHARED_LIMITf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (TH_MMU_SHARED_LIMIT_CHK(cur_limit,
                                sal_ceil_func(thdo_db_shd_min, granularity), post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, sal_ceil_func(thdo_db_shd_min, granularity));
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
                    if (TH_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_qe_shd_min/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_qe_shd_min/granularity);
                    }
                }

                granularity = 8;
                if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMICf)) {
                    field = YELLOW_SHARED_LIMITf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (TH_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_qe_shd_min/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_qe_shd_min/granularity);
                    }
                    field = RED_SHARED_LIMITf;
                    cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                    if (TH_MMU_SHARED_LIMIT_CHK(cur_limit,
                                thdo_qe_shd_min/granularity, post_update)) {
                        soc_mem_field32_set(unit, mem, entry0,
                                            field, thdo_qe_shd_min/granularity);
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
            rval2 = 0;

            SOC_IF_ERROR_RETURN
                (READ_MMU_THDR_DB_CONFIG1_PRIQr(unit, idx, &rval));

            reg = MMU_THDR_DB_CONFIG_PRIQr;
            field = SHARED_LIMITf;
            granularity = 1;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval2));
            if (!soc_reg_field_get(unit, MMU_THDR_DB_CONFIG1_PRIQr, rval,
                                   DYNAMIC_ENABLEf)) {
                cur_limit = soc_reg_field_get(unit, reg, rval2, field);
                if (TH_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd_min/granularity, post_update)) {
                    soc_reg_field_set(unit, reg, &rval2, field,
                                      (thdo_db_shd_min/granularity));
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval2));
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
                if (TH_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd_min/granularity, post_update)) {
                    soc_reg_field_set(unit, reg, &rval2, field, (thdo_db_shd_min/granularity));
                }

                field = SHARED_YELLOW_LIMITf;
                cur_limit = soc_reg_field_get(unit, reg, rval2, field);
                if (TH_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd_min/granularity, post_update)) {
                    soc_reg_field_set(unit, reg, &rval2, field, (thdo_db_shd_min/granularity));
                }
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval2));
            }

        }
        /* per pool RQE settings */
        for (xpe = 0; xpe < NUM_XPE(unit); xpe ++) {
            if (!(xpe_map & (1 << xpe))) {
                continue;
            }

            for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
                reg = MMU_THDR_DB_CONFIG_SPr;
                field = SHARED_LIMITf;
                granularity = 1;
            
                SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_get(unit, 
                    reg, xpe, -1, idx, &rval));
                cur_limit = soc_reg_field_get(unit, reg, rval, field);
            
                if (TH_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd[xpe]/granularity, post_update)) {
                    soc_reg_field_set
                        (unit, reg, &rval, field, (thdo_db_shd[xpe]/granularity));
                    SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
                        reg, xpe, -1, idx, rval));
                }
            
                reg = MMU_THDR_DB_SP_SHARED_LIMITr;
                granularity = 8;
            
                SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_get(unit, 
                    reg, xpe, -1, idx, &rval));
            
                field = SHARED_YELLOW_LIMITf;
                cur_limit = soc_reg_field_get(unit, reg, rval, field);
            
                if (TH_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd[xpe]/granularity, post_update)) {
                    soc_reg_field_set
                        (unit, reg, &rval, field, (thdo_db_shd[xpe]/granularity));
                }
            
                field = SHARED_RED_LIMITf;
                cur_limit = soc_reg_field_get(unit, reg, rval, field);
            
                if (TH_MMU_SHARED_LIMIT_CHK(cur_limit,
                            thdo_db_shd[xpe]/granularity, post_update)) {
                    soc_reg_field_set
                        (unit, reg, &rval, field, (thdo_db_shd[xpe]/granularity));
                }
                SOC_IF_ERROR_RETURN(soc_tomahawk_xpe_reg32_set(unit, 
                    reg, xpe, -1, idx, rval));
            }
        }
    }
    return SOC_E_NONE;
}

int soc_th_iss_log_to_phy_bank_map_shadow_get(int unit, uint32 *shadow_val)
{
    if (shadow_val == NULL) {
        return SOC_E_PARAM;
    }

    *shadow_val = _soc_th_iss_log_to_phy_bank_map_shadow[unit];
    return SOC_E_NONE;
}

int soc_th_iss_log_to_phy_bank_map_shadow_set(int unit, uint32 shadow_val)
{
    _soc_th_iss_log_to_phy_bank_map_shadow[unit] = shadow_val;
    return SOC_E_NONE;
}

int soc_th_reset_serdes(int unit, soc_port_t port, int reset_delay_us)
{
    uint32 rval = 0;
    int rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(READ_CLPORT_XGXS0_CTRL_REGr(unit, port, &rval));
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &rval, RSTB_HWf, 0 );
    SOC_IF_ERROR_RETURN(WRITE_CLPORT_XGXS0_CTRL_REGr(unit, port, rval));
    sal_usleep((reset_delay_us == 0) ? 10 : reset_delay_us );
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &rval, RSTB_HWf, 1 );
    SOC_IF_ERROR_RETURN(WRITE_CLPORT_XGXS0_CTRL_REGr(unit, port, rval));
    sal_usleep((reset_delay_us == 0) ? 10 : reset_delay_us );
    
    return rv;
}

void
_soc_th_lpm_view_set(int unit, int index, soc_mem_t view, int pair_lpm)
{
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);

    if (!_soc_th_lpm_view_map[unit]) {
        return;
    }

    if (pair_lpm) {
        index = (index % tcam_depth) +
                (index / tcam_depth) * 2 * tcam_depth;
        _soc_th_lpm_view_map[unit][index] = view;
        _soc_th_lpm_view_map[unit][index + tcam_depth] = view;
    } else {
        _soc_th_lpm_view_map[unit][index] = view;
    }
}

soc_mem_t
_soc_th_lpm_view_get(int unit, int index, int pair_lpm)
{
    soc_mem_t view;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);

    if (!_soc_th_lpm_view_map[unit]) {
        return INVALIDm;
    }

    if (pair_lpm) {
        index = (index % tcam_depth) +
                (index / tcam_depth) * 2 * tcam_depth;
        view = _soc_th_lpm_view_map[unit][index];
        if (view == INVALIDm) {
            view = _soc_th_lpm_view_map[unit][index + tcam_depth];
        }
    } else {
        view = _soc_th_lpm_view_map[unit][index];
    }
    return view;
}

#if defined(INCLUDE_FLOWTRACKER)
STATIC soc_th_ft_em_ser_handler_t th_ft_em_ser_handler[SOC_MAX_NUM_DEVICES] = {NULL};

void
soc_th_ft_em_ser_handler_register(int unit, soc_th_ft_em_ser_handler_t handler)
{
    th_ft_em_ser_handler[unit] = handler;
}

int
soc_th_ft_em_ser_process(int unit, int pipe, soc_mem_t mem, int index)
{
    if (th_ft_em_ser_handler[unit]) {
        return th_ft_em_ser_handler[unit](unit, pipe, mem, index);
    } else {
        return SOC_E_UNAVAIL;
    }
}
#endif /* INCLUDE_FLOWTRACKER */

#ifndef BCM_SW_STATE_DUMP_DISABLE
void
soc_th_flexport_sw_dump(int unit)
{
    int port, phy_port, mmu_port, pipe, pm, cosq, numq, uc_cosq, uc_numq;
    int max_speed, init_speed, num_lanes;
    char  pfmt[SOC_PBMP_FMT_LEN];

    LOG_CLI((BSL_META_U(unit,
                    "  port(log/phy/mmu)  pipe  pm  lanes    "
                    "speed(Max)    uc_Qbase/Numq mc_Qbase/Numq\n")));
    PBMP_ALL_ITER(unit, port) {
        pipe = SOC_INFO(unit).port_pipe[port];
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
        num_lanes = SOC_INFO(unit).port_num_lanes[port];
        pm = SOC_INFO(unit).port_serdes[port];
        max_speed = SOC_INFO(unit).port_speed_max[port];
        init_speed = SOC_INFO(unit).port_init_speed[port];
        cosq = SOC_INFO(unit).port_cosq_base[port];
        numq = SOC_INFO(unit).port_num_cosq[port];
        uc_cosq = SOC_INFO(unit).port_uc_cosq_base[port];
        uc_numq = SOC_INFO(unit).port_num_uc_cosq[port];

        LOG_CLI((BSL_META_U(unit,
                        "  %4s(%3d/%3d/%3d)  %4d  %2d  %5d %7d(%7d) "
                        "%6d/%-6d  %6d/%-6d\n"),
                    SOC_INFO(unit).port_name[port], port, phy_port, mmu_port,
                    pipe, pm, num_lanes, init_speed, max_speed,
                    uc_cosq, uc_numq, cosq, numq));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n    Oversub Bitmap: %s"),
                SOC_PBMP_FMT(SOC_INFO(unit).oversub_pbm, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "\n    Disabled Bitmap: %s \n"),
                SOC_PBMP_FMT(SOC_INFO(unit).all.disabled_bitmap, pfmt)));
}

void
soc_th_flexport_sw_pipe_speed_group_dump(int unit, int pipe, int speed_id)
{
    int rv;
    soc_reg_t reg;
    int port_id, pm_id, inst;
    uint32 rval, fval;
    int speed, group, slot;

    static char *speed_id_name[] = {
        "NONE", "10G", "20G", "25G", "40G", "50G", "100G"
    };

    speed = speed_id;

    if (pipe < 0 || pipe > 3) {
        LOG_CLI((BSL_META_U(unit,
                        "\n    Error: pipe %d not valid for chip %s.\n"),
                    pipe, SOC_UNIT_NAME(unit)));
        return;
    }
    if (speed < 0 || speed > SOC_TH_SPEED_CLASS_100G) {
        LOG_CLI((BSL_META_U(unit,
                        "\n    Error: speed class %d not valid for chip %s.\n"),
                    speed, SOC_UNIT_NAME(unit)));
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                    "\n    pipe %d OverSub Scheduler IDB & MMU Table\n"),
                pipe));
    for (group = 0; group < _OVS_GROUP_COUNT; group++) {
        reg = SOC_REG_UNIQUE_ACC(unit, IS_OVR_SUB_GRP_CFGr)[pipe];
        rv = soc_reg32_get(unit, reg, REG_PORT_ANY, group, &rval);
        if (rv < 0) {
            LOG_CLI((BSL_META_U(unit, " FAILED: Register read.\n")));
        }
        fval = soc_reg_field_get(unit, reg, rval, SPEEDf);
        if (fval == speed) {
            LOG_CLI((BSL_META_U(unit,
                            "\n    SPEED %s in IDB SPEED GROUP\n"),
                        speed_id_name[fval]));
            reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
            for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                rv = soc_reg32_get(unit, reg, REG_PORT_ANY, slot, &rval);
                if (rv <0) {
                    continue;
                }
                port_id = soc_reg_field_get(unit, reg, rval, PORT_NUMf);
                if (port_id != 0x3f) {
                    pm_id = soc_reg_field_get(unit, reg, rval, PHY_PORT_IDf);
                    LOG_CLI((BSL_META_U(unit,
                                    "{%d, %d} "),
                                pm_id, port_id));
                }
            }
            LOG_CLI((BSL_META_U(unit, "\n")));
        }
    }

    for (group = 0; group < _OVS_GROUP_COUNT; group++) {
        reg = OVR_SUB_GRP_CFGr;
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        rv = soc_reg32_get(unit, reg, inst, group, &rval);
        if (rv <0) {
            continue;
        }
        fval = soc_reg_field_get(unit, reg, rval, SPEEDf);
        if (fval == speed) {
            LOG_CLI((BSL_META_U(unit,
                            "\n    SPEED %s in MMU SPEED GROUP\n"),
                        speed_id_name[fval]));
            reg = mmu_grp_tbl_regs[group];
            for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                rv = soc_reg32_get(unit, reg, inst, slot, &rval);
                if (rv < 0) {
                    continue;
                }
                port_id = soc_reg_field_get(unit, reg, rval, PORT_NUMf);
                if (port_id != 0x3f) {
                    pm_id = soc_reg_field_get(unit, reg, rval, PHY_PORT_IDf);
                    LOG_CLI((BSL_META_U(unit,
                                    "{%d, %d} "),
                                pm_id, port_id));
                }
            }
            LOG_CLI((BSL_META_U(unit, "\n")));
        }
    }

    LOG_CLI((BSL_META_U(unit, "\n")));
}

#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * Tomahawk chip driver functions.
 * Pls keep at the end of this file for easy access.
 */
soc_functions_t soc_tomahawk_drv_funs = {
    _soc_tomahawk_misc_init,
    _soc_tomahawk_mmu_init,
    _soc_tomahawk_age_timer_get,
    _soc_tomahawk_age_timer_max_get,
    _soc_tomahawk_age_timer_set,
    _soc_tomahawk_tscx_firmware_set,
    _soc_tomahawk_tscx_reg_read,
    _soc_tomahawk_tscx_reg_write,
    _soc_tomahawk_bond_info_init
};
#endif /* BCM_TOMAHAWK_SUPPORT */
