/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    techsupport.c
 * Purpose: This file contains the generic infrastructure code
 *          for techsupport utility.
 */

#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <appl/diag/shell.h>
#include <sal/core/time.h>
#include <appl/diag/techsupport.h>
#include <soc/drv.h>
#include <sal/appl/sal.h>
#include <bcm/types.h>
#include <soc/defs.h>

char command_techsupport_usage[] =
     "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "techsupport <option> [args...]\n"
#else
    "techsupport basic - collects basic config/setup information from switch\n\t"
    "techsupport all - Executes all techsupport commands supported on switch\n\t"
    "techSupport <feature_name> [diag] [reg] "
#ifndef BCM_SW_STATE_DUMP_DISABLE
    "[mem] [list] [sw] [verbose]- collects feature specific debug information\n\t"
#else
    "[mem] [list] [verbose]- collects feature specific debug information\n\t"
#endif /* BCM_SW_STATE_DUMP_DISABLE */
    "When <feature_name> is "COMMAND_ETRAP" - collects etrap related debug information\n\t"
    "When <feature_name> is "COMMAND_INT" - collects int related debug information\n\t"
    "When <feature_name> is "COMMAND_LATENCY_HISTOGRAM" - collects histogram related debug information\n\t"
    "When <feature_name> is "COMMAND_ALPM" - collects alpm related debug information\n\t"
    "When <feature_name> is "COMMAND_COS" - collects cos related debug information\n\t"
    "When <feature_name> is "COMMAND_EFP" - collects efp related debug information\n\t"
    "When <feature_name> is "COMMAND_IFP" - collects ifp related debug information\n\t"
    "When <feature_name> is "COMMAND_LOAD_BALANCE" - collects RTAG7,DLB,ECMP and "
    "trunk related debug information\n\t"
    "When <feature_name> is "COMMAND_L3UC" - collects L3 unicast related debug information\n\t"
    "When <feature_name> is "COMMAND_L3MC" - collects L3 multicast related debug information\n\t"
    "When <feature_name> is "COMMAND_MPLS" - collects mpls related debug information\n\t"
    "When <feature_name> is "COMMAND_MMU" - collects mmu related debug information\n\t"
    "When <feature_name> is "COMMAND_NIV" - collects niv related debug information\n\t"
    "When <feature_name> is "COMMAND_VFP" - collects vfp related debug information\n\t"
    "When <feature_name> is "COMMAND_RIOT" - collects riot related debug information\n\t"
    "When <feature_name> is "COMMAND_OAM" - collects oam related debug information\n\t"
    "When <feature_name> is "COMMAND_VXLAN" - collects vxlan related debug information\n\t"
    "When <feature_name> is "COMMAND_VLAN" - collects vlan related debug information\n\t"
    "When <feature_name> is "COMMAND_TCB" - collects tcb related debug information\n\t"
    "When <feature_name> is "COMMAND_DGM" - collects dgm related debug information\n\t"
    "When <feature_name> is "COMMAND_PSTAT" - collects pstat related debug information\n\t"
    "When <feature_name> is "COMMAND_FLEX_FLOW" - collects FlexFlow related debug information\n\t"
    "When <feature_name> is "COMMAND_PHY" - collects Phy related debug information\n\t"
    "When <feature_name> is "COMMAND_LEDUP" - collects CMIC led-processor debug information\n\t"
    "When <feature_name> is "COMMAND_CMIC" - collects CMIC debug information\n\t"
    "When <feature_name> is "COMMAND_CANCUN" - collects Cancun debug information\n\t"
    "When <feature_name> is "COMMAND_VISIBILITY" - collects packet trace information for packet data\n\t"
#ifdef BCM_MONTEREY_SUPPORT
    "When <feature_name> is "COMMAND_CPRI" - collects CPRI related debug information\n\t"
    "When <feature_name> is "COMMAND_RSVD4" - collects RSVD4 related debug information\n\t"
#endif
    "techsupport visibility file/data=<val> sourceport=<val> options=<flags> [raw]\n\t"
    "file or data- packet data, sourceport- source port for packet, (mandatory)\n\t"
#ifdef BCM_WARM_BOOT_SUPPORT
    "When <feature_name> is "COMMAND_WARMBOOT" - collects warmboot info\n\t"
#endif
#ifdef INCLUDE_XFLOW_MACSEC
    "When <feature_name> is "COMMAND_XFLOW_MACSEC" - collects xflow-macsec debug info\n\t"
#endif
#ifdef BCM_FLOWTRACKER_SUPPORT
    "When <feature_name> is "COMMAND_FLOWTRACKER" - collects flowtracker debug information\n\t"
#endif
    "raw- dumps raw memory content (optional)\n\t"
    "options- packet trace flags (optional)\n\t"
    "       - 0x1 BCM_PKT_TRACE_LEARN\n\t"
    "       - 0x2 BCM_PKT_TRACE_NO_IFP\n\t"
    "       - 0x4 BCM_PKT_TRACE_FORWARD\n\t"
    "       - 0x8 BCM_PKT_TRACE_DROP_TO_MMU\n\t"
    "       - 0x10 BCM_PKT_TRACE_DROP_TO_EGR\n"
#endif /* COMPILER_STRING_CONST_LIMIT */
    ;
extern cmd_result_t command_techsupport(int unit, args_t *a);

mbcm_techsupport_t *mbcm_techsupport[BCM_MAX_NUM_UNITS];

#ifdef BCM_WARM_BOOT_SUPPORT
extern techsupport_data_t techsupport_warmboot_data;
#endif


extern techsupport_data_t techsupport_l3uc_trident2plus_data;
extern techsupport_data_t techsupport_l3mc_trident2plus_data;
extern techsupport_data_t techsupport_mmu_trident2plus_data;
extern techsupport_data_t techsupport_mpls_trident2plus_data;
extern techsupport_data_t techsupport_niv_trident2plus_data;
extern techsupport_data_t techsupport_riot_trident2plus_data;
extern techsupport_data_t techsupport_vxlan_trident2plus_data;
extern techsupport_data_t techsupport_vlan_trident2plus_data;
extern techsupport_data_t techsupport_ifp_trident2plus_data;
extern techsupport_data_t techsupport_vfp_trident2plus_data;
extern techsupport_data_t techsupport_efp_trident2plus_data;
extern techsupport_data_t techsupport_cos_trident2plus_data;
extern techsupport_data_t techsupport_loadbalance_trident2plus_data;
extern techsupport_data_t techsupport_oam_trident2plus_data;
extern techsupport_data_t techsupport_ledup_trident2plus_data;

/* Global structure that maintains different features for "Trident2plus" chipset */
mbcm_techsupport_t mbcm_trident2plus_techsupport[] = {
    {COMMAND_L3MC, &techsupport_l3mc_trident2plus_data},
    {COMMAND_L3UC, &techsupport_l3uc_trident2plus_data},
    {COMMAND_MMU, &techsupport_mmu_trident2plus_data},
    {COMMAND_MPLS, &techsupport_mpls_trident2plus_data},
    {COMMAND_NIV, &techsupport_niv_trident2plus_data},
    {COMMAND_RIOT, &techsupport_riot_trident2plus_data},
    {COMMAND_OAM, &techsupport_oam_trident2plus_data},
    {COMMAND_VXLAN, &techsupport_vxlan_trident2plus_data},
    {COMMAND_VLAN, &techsupport_vlan_trident2plus_data},
    {COMMAND_IFP, &techsupport_ifp_trident2plus_data},
    {COMMAND_VFP, &techsupport_vfp_trident2plus_data},
    {COMMAND_EFP, &techsupport_efp_trident2plus_data},
    {COMMAND_COS, &techsupport_cos_trident2plus_data},
    {COMMAND_LOAD_BALANCE, &techsupport_loadbalance_trident2plus_data},
    {COMMAND_LEDUP, &techsupport_ledup_trident2plus_data},
#ifdef BCM_WARM_BOOT_SUPPORT
    {COMMAND_WARMBOOT, &techsupport_warmboot_data},
#endif
    {NULL, NULL} /* Must be the last element in this structure */
  };

extern techsupport_data_t techsupport_l3uc_apache_data;
extern techsupport_data_t techsupport_l3mc_apache_data;
extern techsupport_data_t techsupport_cos_apache_data;
extern techsupport_data_t techsupport_mmu_apache_data;
extern techsupport_data_t techsupport_ifp_apache_data;
extern techsupport_data_t techsupport_efp_apache_data;
extern techsupport_data_t techsupport_mpls_apache_data;
extern techsupport_data_t techsupport_vxlan_apache_data;
extern techsupport_data_t techsupport_vlan_apache_data;
extern techsupport_data_t techsupport_loadbalance_apache_data;

/* Global structure that maintains different features for "apache" chipset */
mbcm_techsupport_t mbcm_apache_techsupport[] = {
    {COMMAND_L3MC, &techsupport_l3mc_apache_data},
    {COMMAND_L3UC, &techsupport_l3uc_apache_data},
    {COMMAND_MMU, &techsupport_mmu_apache_data},
    {COMMAND_MPLS, &techsupport_mpls_apache_data},
    {COMMAND_NIV, &techsupport_niv_trident2plus_data},
    {COMMAND_RIOT, &techsupport_riot_trident2plus_data},
    {COMMAND_VXLAN, &techsupport_vxlan_apache_data},
    {COMMAND_VLAN, &techsupport_vlan_apache_data},
    {COMMAND_IFP, &techsupport_ifp_apache_data},
    {COMMAND_VFP, &techsupport_vfp_trident2plus_data},
    {COMMAND_EFP, &techsupport_efp_apache_data},
    {COMMAND_COS, &techsupport_cos_apache_data},
    {COMMAND_LOAD_BALANCE, &techsupport_loadbalance_apache_data},
    {COMMAND_LEDUP, &techsupport_ledup_trident2plus_data},
#ifdef BCM_WARM_BOOT_SUPPORT
    {COMMAND_WARMBOOT, &techsupport_warmboot_data},
#endif
    {NULL, NULL} /* Must be the last element in this structure */
  };

#ifdef BCM_MONTEREY_SUPPORT
extern techsupport_data_t techsupport_mmu_monterey_data;
extern techsupport_data_t techsupport_phy_monterey_data;
extern techsupport_data_t techsupport_cpri_monterey_data;
extern techsupport_data_t techsupport_rsvd4_monterey_data;
#if INCLUDE_XFLOW_MACSEC
extern techsupport_data_t techsupport_xflow_macsec_monterey_data;
#endif

/* Global structure that maintains different features for "Monterey" chipset */
mbcm_techsupport_t mbcm_monterey_techsupport[] = {
    {COMMAND_L3MC, &techsupport_l3mc_apache_data},
    {COMMAND_L3UC, &techsupport_l3uc_apache_data},
    {COMMAND_MMU, &techsupport_mmu_monterey_data},
    {COMMAND_MPLS, &techsupport_mpls_apache_data},
    {COMMAND_NIV, &techsupport_niv_trident2plus_data},
    {COMMAND_RIOT, &techsupport_riot_trident2plus_data},
    {COMMAND_VXLAN, &techsupport_vxlan_apache_data},
    {COMMAND_VLAN, &techsupport_vlan_apache_data},
    {COMMAND_IFP, &techsupport_ifp_apache_data},
    {COMMAND_VFP, &techsupport_vfp_trident2plus_data},
    {COMMAND_EFP, &techsupport_efp_apache_data},
    {COMMAND_COS, &techsupport_cos_apache_data},
    {COMMAND_LOAD_BALANCE, &techsupport_loadbalance_apache_data},
    {COMMAND_LEDUP, &techsupport_ledup_trident2plus_data},
    {COMMAND_PHY, &techsupport_phy_monterey_data},
    {COMMAND_CPRI, &techsupport_cpri_monterey_data},
    {COMMAND_RSVD4, &techsupport_rsvd4_monterey_data},
#ifdef BCM_WARM_BOOT_SUPPORT
    {COMMAND_WARMBOOT, &techsupport_warmboot_data},
#endif
#if INCLUDE_XFLOW_MACSEC
    {COMMAND_XFLOW_MACSEC, &techsupport_xflow_macsec_monterey_data},
#endif
    {NULL, NULL} /* Must be the last element in this structure */
  };
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
extern techsupport_data_t techsupport_vxlan_tomahawk_data;
extern techsupport_data_t techsupport_ifp_tomahawk_data;
extern techsupport_data_t techsupport_vfp_tomahawk_data;
extern techsupport_data_t techsupport_efp_tomahawk_data;
extern techsupport_data_t techsupport_mmu_tomahawk_data;
extern techsupport_data_t techsupport_oam_tomahawk_data;
extern techsupport_data_t techsupport_vlan_tomahawk_data;
extern techsupport_data_t techsupport_l3uc_tomahawk_data;
extern techsupport_data_t techsupport_l3mc_tomahawk_data;
extern techsupport_data_t techsupport_mpls_tomahawk_data;
extern techsupport_data_t techsupport_niv_tomahawk_data;
extern techsupport_data_t techsupport_loadbalance_tomahawk_data;
extern techsupport_data_t techsupport_cos_tomahawk_data;
extern techsupport_data_t techsupport_phy_tomahawk_data;
extern techsupport_data_t techsupport_ledup_tomahawk_data;

mbcm_techsupport_t mbcm_tomahawk_techsupport[] = {
    {COMMAND_VXLAN, &techsupport_vxlan_tomahawk_data},
    {COMMAND_MMU, &techsupport_mmu_tomahawk_data},
    {COMMAND_OAM, &techsupport_oam_tomahawk_data},
    {COMMAND_IFP, &techsupport_ifp_tomahawk_data},
    {COMMAND_VFP, &techsupport_vfp_tomahawk_data},
    {COMMAND_EFP, &techsupport_efp_tomahawk_data},
    {COMMAND_VLAN, &techsupport_vlan_tomahawk_data},
    {COMMAND_L3UC, &techsupport_l3uc_tomahawk_data},
    {COMMAND_L3MC, &techsupport_l3mc_tomahawk_data},
    {COMMAND_MPLS, &techsupport_mpls_tomahawk_data},
    {COMMAND_NIV, &techsupport_niv_tomahawk_data},
    {COMMAND_LOAD_BALANCE, &techsupport_loadbalance_tomahawk_data},
    {COMMAND_COS, &techsupport_cos_tomahawk_data},
    {COMMAND_PHY, &techsupport_phy_tomahawk_data},
    {COMMAND_LEDUP, &techsupport_ledup_tomahawk_data},
    {COMMAND_VISIBILITY, NULL},
#ifdef BCM_WARM_BOOT_SUPPORT
    {COMMAND_WARMBOOT, &techsupport_warmboot_data},
#endif
    {NULL, NULL} /* Must be the last element in this structure */
  };
#endif

#ifdef BCM_TRIDENT3_SUPPORT
extern techsupport_data_t techsupport_mpls_trident3_data;
extern techsupport_data_t techsupport_cos_trident3_data;
extern techsupport_data_t techsupport_mmu_trident3_data;
extern techsupport_data_t techsupport_vxlan_trident3_data;
extern techsupport_data_t techsupport_niv_trident3_data;
extern techsupport_data_t techsupport_vlan_trident3_data;
extern techsupport_data_t techsupport_flex_flow_trident3_data;
extern techsupport_data_t techsupport_riot_trident3_data;
extern techsupport_data_t techsupport_loadbalance_trident3_data;
extern techsupport_data_t techsupport_l3uc_trident3_data;
extern techsupport_data_t techsupport_l3mc_trident3_data;
extern techsupport_data_t techsupport_cmic_trident3_data;
extern techsupport_data_t techsupport_cancun_trident3_data;
extern techsupport_data_t techsupport_cancun_helix5_data;

mbcm_techsupport_t mbcm_trident3_techsupport[] = {
    {COMMAND_IFP, &techsupport_ifp_tomahawk_data},
    {COMMAND_VFP, &techsupport_vfp_tomahawk_data},
    {COMMAND_EFP, &techsupport_efp_tomahawk_data},
    {COMMAND_MPLS,&techsupport_mpls_trident3_data},
    {COMMAND_COS, &techsupport_cos_trident3_data},
    {COMMAND_MMU, &techsupport_mmu_trident3_data},
    {COMMAND_VXLAN, &techsupport_vxlan_trident3_data},
    {COMMAND_NIV, &techsupport_niv_trident3_data},
    {COMMAND_VLAN, &techsupport_vlan_trident3_data},
    {COMMAND_RIOT, &techsupport_riot_trident3_data},
    {COMMAND_FLEX_FLOW, &techsupport_flex_flow_trident3_data},
    {COMMAND_LOAD_BALANCE, &techsupport_loadbalance_trident3_data},
    {COMMAND_L3UC, &techsupport_l3uc_trident3_data},
    {COMMAND_L3MC, &techsupport_l3mc_trident3_data},
    {COMMAND_CMIC, &techsupport_cmic_trident3_data},
    {COMMAND_VISIBILITY, NULL},
#ifdef BCM_WARM_BOOT_SUPPORT
    {COMMAND_WARMBOOT, &techsupport_warmboot_data},
#endif
    {COMMAND_CANCUN, &techsupport_cancun_trident3_data},
    {NULL, NULL} /* Must be the last element in this structure */
  };
#endif

#ifdef BCM_HURRICANE4_SUPPORT
extern techsupport_data_t techsupport_cancun_hurricane4_data;
#if INCLUDE_XFLOW_MACSEC
extern techsupport_data_t techsupport_xflow_macsec_hurricane4_data;
#endif
#ifdef BCM_FLOWTRACKER_SUPPORT
extern techsupport_data_t techsupport_flowtracker_hurricane4_data;
#endif
mbcm_techsupport_t mbcm_hurricane4_techsupport[] = {
    {COMMAND_IFP, &techsupport_ifp_tomahawk_data},
    {COMMAND_VFP, &techsupport_vfp_tomahawk_data},
    {COMMAND_EFP, &techsupport_efp_tomahawk_data},
    {COMMAND_MPLS,&techsupport_mpls_trident3_data},
    {COMMAND_COS, &techsupport_cos_trident3_data},
    {COMMAND_MMU, &techsupport_mmu_trident3_data},
    {COMMAND_VXLAN, &techsupport_vxlan_trident3_data},
    {COMMAND_NIV, &techsupport_niv_trident3_data},
    {COMMAND_VLAN, &techsupport_vlan_trident3_data},
    {COMMAND_RIOT, &techsupport_riot_trident3_data},
    {COMMAND_FLEX_FLOW, &techsupport_flex_flow_trident3_data},
    {COMMAND_LOAD_BALANCE, &techsupport_loadbalance_trident3_data},
    {COMMAND_L3UC, &techsupport_l3uc_trident3_data},
    {COMMAND_L3MC, &techsupport_l3mc_trident3_data},
    {COMMAND_CMIC, &techsupport_cmic_trident3_data},
    {COMMAND_VISIBILITY, NULL},
#ifdef BCM_WARM_BOOT_SUPPORT
    {COMMAND_WARMBOOT, &techsupport_warmboot_data},
#endif
#if INCLUDE_XFLOW_MACSEC
    {COMMAND_XFLOW_MACSEC, &techsupport_xflow_macsec_hurricane4_data},
#endif
    {COMMAND_CANCUN, &techsupport_cancun_hurricane4_data},
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    {COMMAND_FLOWTRACKER, &techsupport_flowtracker_hurricane4_data},
#endif
    {NULL, NULL} /* Must be the last element in this structure */
  };
#endif

#ifdef BCM_FLOWTRACKER_SUPPORT
extern techsupport_data_t techsupport_flowtracker_helix5_data;
#endif
#ifdef BCM_HELIX5_SUPPORT
mbcm_techsupport_t mbcm_helix5_techsupport[] = {
    {COMMAND_IFP, &techsupport_ifp_tomahawk_data},
    {COMMAND_VFP, &techsupport_vfp_tomahawk_data},
    {COMMAND_EFP, &techsupport_efp_tomahawk_data},
    {COMMAND_MPLS,&techsupport_mpls_trident3_data},
    {COMMAND_COS, &techsupport_cos_trident3_data},
    {COMMAND_MMU, &techsupport_mmu_trident3_data},
    {COMMAND_VXLAN, &techsupport_vxlan_trident3_data},
    {COMMAND_NIV, &techsupport_niv_trident3_data},
    {COMMAND_VLAN, &techsupport_vlan_trident3_data},
    {COMMAND_RIOT, &techsupport_riot_trident3_data},
    {COMMAND_FLEX_FLOW, &techsupport_flex_flow_trident3_data},
    {COMMAND_LOAD_BALANCE, &techsupport_loadbalance_trident3_data},
    {COMMAND_L3UC, &techsupport_l3uc_trident3_data},
    {COMMAND_L3MC, &techsupport_l3mc_trident3_data},
    {COMMAND_CMIC, &techsupport_cmic_trident3_data},
    {COMMAND_VISIBILITY, NULL},
#ifdef BCM_WARM_BOOT_SUPPORT
    {COMMAND_WARMBOOT, &techsupport_warmboot_data},
#endif
    {COMMAND_CANCUN, &techsupport_cancun_helix5_data},
#ifdef BCM_FLOWTRACKER_SUPPORT
    {COMMAND_FLOWTRACKER, &techsupport_flowtracker_helix5_data},
#endif
    {NULL, NULL} /* Must be the last element in this structure */
  };
#endif

#ifdef BCM_TOMAHAWK2_SUPPORT
extern techsupport_data_t techsupport_efp_tomahawk2_data;
extern techsupport_data_t techsupport_tcb_tomahawk2_data;
extern techsupport_data_t techsupport_dgm_tomahawk2_data;
extern techsupport_data_t techsupport_pstat_tomahawk2_data;
extern techsupport_data_t techsupport_vxlan_tomahawk2_data;
extern techsupport_data_t techsupport_mmu_tomahawk2_data;
extern techsupport_data_t techsupport_mpls_tomahawk2_data;
extern techsupport_data_t techsupport_loadbalance_tomahawk2_data;
extern techsupport_data_t techsupport_cos_tomahawk2_data;
extern techsupport_data_t techsupport_ledup_tomahawk2_data;

mbcm_techsupport_t mbcm_tomahawk2_techsupport[] = {
    {COMMAND_IFP, &techsupport_ifp_tomahawk_data},
    {COMMAND_VFP, &techsupport_vfp_tomahawk_data},
    {COMMAND_EFP, &techsupport_efp_tomahawk2_data},
    {COMMAND_TCB, &techsupport_tcb_tomahawk2_data},
    {COMMAND_DGM, &techsupport_dgm_tomahawk2_data},
    {COMMAND_PSTAT, &techsupport_pstat_tomahawk2_data},
    {COMMAND_VLAN, &techsupport_vlan_tomahawk_data},
    {COMMAND_L3UC, &techsupport_l3uc_tomahawk_data},
    {COMMAND_L3MC, &techsupport_l3mc_tomahawk_data},
    {COMMAND_MPLS, &techsupport_mpls_tomahawk2_data},
    {COMMAND_NIV, &techsupport_niv_tomahawk_data},
    {COMMAND_LOAD_BALANCE, &techsupport_loadbalance_tomahawk2_data},
    {COMMAND_COS, &techsupport_cos_tomahawk2_data},
    {COMMAND_VXLAN, &techsupport_vxlan_tomahawk2_data},
    {COMMAND_MMU, &techsupport_mmu_tomahawk2_data},
    {COMMAND_PHY, &techsupport_phy_tomahawk_data},
    {COMMAND_LEDUP, &techsupport_ledup_tomahawk2_data},
    {COMMAND_VISIBILITY, NULL},
#ifdef BCM_WARM_BOOT_SUPPORT
    {COMMAND_WARMBOOT, &techsupport_warmboot_data},
#endif
    {NULL, NULL} /* Must be the last element in this structure */
  };
#endif

#ifdef BCM_TRIDENT2_SUPPORT
extern techsupport_data_t techsupport_l3uc_trident2_data;
extern techsupport_data_t techsupport_l3mc_trident2_data;
extern techsupport_data_t techsupport_mmu_trident2_data;
extern techsupport_data_t techsupport_mpls_trident2_data;
extern techsupport_data_t techsupport_niv_trident2_data;
extern techsupport_data_t techsupport_vxlan_trident2_data;
extern techsupport_data_t techsupport_vlan_trident2_data;
extern techsupport_data_t techsupport_cos_trident2_data;
extern techsupport_data_t techsupport_loadbalance_trident2_data;
extern techsupport_data_t techsupport_efp_trident2_data;
extern techsupport_data_t techsupport_ifp_trident2_data;
extern techsupport_data_t techsupport_oam_trident2_data;

mbcm_techsupport_t mbcm_trident2_techsupport[] = {
    {COMMAND_L3MC, &techsupport_l3mc_trident2_data},
    {COMMAND_L3UC, &techsupport_l3uc_trident2_data},
    {COMMAND_MMU, &techsupport_mmu_trident2_data},
    {COMMAND_OAM, &techsupport_oam_trident2_data},
    {COMMAND_COS, &techsupport_cos_trident2_data},
    {COMMAND_LOAD_BALANCE, &techsupport_loadbalance_trident2_data},
    {COMMAND_MPLS, &techsupport_mpls_trident2_data},
    {COMMAND_NIV, &techsupport_niv_trident2_data},
    {COMMAND_VXLAN, &techsupport_vxlan_trident2_data},
    {COMMAND_VLAN, &techsupport_vlan_trident2_data},
    /*Intentionally using td2plus data set for td2
     * as there is no change in data for IFP/VFP*/
    {COMMAND_VFP, &techsupport_vfp_trident2plus_data},
    {COMMAND_IFP, &techsupport_ifp_trident2_data},
    {COMMAND_EFP, &techsupport_efp_trident2_data},
#ifdef BCM_WARM_BOOT_SUPPORT
    {COMMAND_WARMBOOT, &techsupport_warmboot_data},
#endif
    {NULL, NULL} /* Must be the last element in this structure */
   };
#endif

#ifdef BCM_SABER2_SUPPORT
extern techsupport_data_t techsupport_l3uc_saber2_data;
extern techsupport_data_t techsupport_l3mc_saber2_data;
extern techsupport_data_t techsupport_mmu_saber2_data;
extern techsupport_data_t techsupport_mpls_saber2_data;
extern techsupport_data_t techsupport_niv_saber2_data;
extern techsupport_data_t techsupport_vxlan_saber2_data;
extern techsupport_data_t techsupport_vlan_saber2_data;
extern techsupport_data_t techsupport_cos_saber2_data;
extern techsupport_data_t techsupport_loadbalance_saber2_data;
extern techsupport_data_t techsupport_vfp_saber2_data;
extern techsupport_data_t techsupport_efp_saber2_data;
extern techsupport_data_t techsupport_ifp_saber2_data;
extern techsupport_data_t techsupport_oam_saber2_data;

mbcm_techsupport_t mbcm_saber2_techsupport[] = {
    {COMMAND_L3MC, &techsupport_l3mc_saber2_data},
    {COMMAND_L3UC, &techsupport_l3uc_saber2_data},
    {COMMAND_MMU, &techsupport_mmu_saber2_data},
    {COMMAND_COS, &techsupport_cos_saber2_data},
    {COMMAND_LOAD_BALANCE, &techsupport_loadbalance_saber2_data},
    {COMMAND_MPLS, &techsupport_mpls_saber2_data},
    {COMMAND_NIV, &techsupport_niv_saber2_data},
    {COMMAND_VLAN, &techsupport_vlan_saber2_data},
    {COMMAND_VFP, &techsupport_vfp_saber2_data},
    {COMMAND_IFP, &techsupport_ifp_saber2_data},
    {COMMAND_EFP, &techsupport_efp_saber2_data},
    {COMMAND_OAM, &techsupport_oam_saber2_data},
#ifdef BCM_WARM_BOOT_SUPPORT
    {COMMAND_WARMBOOT, &techsupport_warmboot_data},
#endif
    {NULL, NULL} /* Must be the last element in this structure */
   };
#endif


#if defined BCM_TOMAHAWK3_SUPPORT
extern techsupport_data_t techsupport_cos_tomahawk3_data;
extern techsupport_data_t techsupport_vlan_tomahawk3_data;
extern techsupport_data_t techsupport_ifp_tomahawk3_data;
extern techsupport_data_t techsupport_cmic_trident3_data;
extern techsupport_data_t techsupport_mpls_tomahawk3_data;
extern techsupport_data_t techsupport_loadbalance_tomahawk3_data;
extern techsupport_data_t techsupport_mmu_tomahawk3_data;
extern techsupport_data_t techsupport_etrap_tomahawk3_data;
extern techsupport_data_t techsupport_dgm_tomahawk3_data;
extern techsupport_data_t techsupport_efp_tomahawk3_data;
extern techsupport_data_t techsupport_phy_tomahawk3_data;
extern techsupport_data_t techsupport_pstat_tomahawk3_data;
extern techsupport_data_t techsupport_ledup_tomahawk3_data;
extern techsupport_data_t techsupport_int_tomahawk3_data;
extern techsupport_data_t techsupport_l3uc_tomahawk3_data;
extern techsupport_data_t techsupport_histogram_tomahawk3_data;
extern techsupport_data_t techsupport_vfp_tomahawk3_data;
extern techsupport_data_t techsupport_alpm_tomahawk3_data;
extern techsupport_data_t techsupport_l3mc_tomahawk3_data;

mbcm_techsupport_t mbcm_tomahawk3_techsupport[] = {
    {COMMAND_COS, &techsupport_cos_tomahawk3_data},
    {COMMAND_VLAN, &techsupport_vlan_tomahawk3_data},
    {COMMAND_IFP, &techsupport_ifp_tomahawk3_data},
    {COMMAND_CMIC, &techsupport_cmic_trident3_data},
    {COMMAND_MPLS, &techsupport_mpls_tomahawk3_data},
    {COMMAND_LOAD_BALANCE, &techsupport_loadbalance_tomahawk3_data},
    {COMMAND_MMU, &techsupport_mmu_tomahawk3_data},
    {COMMAND_ETRAP, &techsupport_etrap_tomahawk3_data},
    {COMMAND_DGM, &techsupport_dgm_tomahawk3_data},
    {COMMAND_EFP, &techsupport_efp_tomahawk3_data},
    {COMMAND_PHY, &techsupport_phy_tomahawk3_data},
    {COMMAND_PSTAT, &techsupport_pstat_tomahawk3_data},
    {COMMAND_LEDUP, &techsupport_ledup_tomahawk3_data},
    {COMMAND_INT, &techsupport_int_tomahawk3_data},
    {COMMAND_L3UC, &techsupport_l3uc_tomahawk3_data},
    {COMMAND_LATENCY_HISTOGRAM, &techsupport_histogram_tomahawk3_data},
    {COMMAND_VFP, &techsupport_vfp_tomahawk3_data},
    {COMMAND_ALPM, &techsupport_alpm_tomahawk3_data},
    {COMMAND_L3MC, &techsupport_l3mc_tomahawk3_data},
    {COMMAND_VISIBILITY, NULL},
#ifdef BCM_WARM_BOOT_SUPPORT
    {COMMAND_WARMBOOT, &techsupport_warmboot_data},
#endif
    {NULL, NULL} /* Must be the last element in this structure */
};
#endif /* BCM_TOMAHAWK3_SUPPORT */

#ifdef BCM_FLOWTRACKER_V3_SUPPORT
extern techsupport_data_t techsupport_flowtracker_firebolt6_data;
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
mbcm_techsupport_t mbcm_firebolt6_techsupport[] = {
    {COMMAND_IFP, &techsupport_ifp_tomahawk_data},
    {COMMAND_VFP, &techsupport_vfp_tomahawk_data},
    {COMMAND_EFP, &techsupport_efp_tomahawk_data},
    {COMMAND_MPLS,&techsupport_mpls_trident3_data},
    {COMMAND_COS, &techsupport_cos_trident3_data},
    {COMMAND_MMU, &techsupport_mmu_trident3_data},
    {COMMAND_VXLAN, &techsupport_vxlan_trident3_data},
    {COMMAND_NIV, &techsupport_niv_trident3_data},
    {COMMAND_VLAN, &techsupport_vlan_trident3_data},
    {COMMAND_RIOT, &techsupport_riot_trident3_data},
    {COMMAND_FLEX_FLOW, &techsupport_flex_flow_trident3_data},
    {COMMAND_LOAD_BALANCE, &techsupport_loadbalance_trident3_data},
    {COMMAND_L3UC, &techsupport_l3uc_trident3_data},
    {COMMAND_L3MC, &techsupport_l3mc_trident3_data},
    {COMMAND_CMIC, &techsupport_cmic_trident3_data},
    {COMMAND_VISIBILITY, NULL},
#ifdef BCM_WARM_BOOT_SUPPORT
    {COMMAND_WARMBOOT, &techsupport_warmboot_data},
#endif
    {COMMAND_CANCUN, &techsupport_cancun_helix5_data},
#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    {COMMAND_FLOWTRACKER, &techsupport_flowtracker_firebolt6_data},
#endif
    {NULL, NULL} /* Must be the last element in this structure */
  };
#endif

#ifdef BCM_HELIX5_SUPPORT
soc_reg_t hx5_cmic_broadsync_reg_skip_list  [] = {
    CMIC_BROADSYNC_REF_CLK_GEN_CTRLr,
    CMIC_BS0_CLK_CTRLr,
    CMIC_BS0_CONFIGr,
    CMIC_BS0_HEARTBEAT_CTRLr,
    CMIC_BS0_HEARTBEAT_DOWN_DURATIONr,
    CMIC_BS0_HEARTBEAT_UP_DURATIONr,
    CMIC_BS0_INITIAL_CRCr,
    CMIC_BS0_INPUT_TIME_0r,
    CMIC_BS0_INPUT_TIME_1r,
    CMIC_BS0_INPUT_TIME_2r,
    CMIC_BS0_OUTPUT_TIME_0r,
    CMIC_BS0_OUTPUT_TIME_1r,
    CMIC_BS0_OUTPUT_TIME_2r,
    CMIC_BS1_CLK_CTRLr,
    CMIC_BS1_CONFIGr,
    CMIC_BS1_HEARTBEAT_CTRLr,
    CMIC_BS1_HEARTBEAT_DOWN_DURATIONr,
    CMIC_BS1_HEARTBEAT_UP_DURATIONr,
    CMIC_BS1_INITIAL_CRCr,
    CMIC_BS1_INPUT_TIME_0r,
    CMIC_BS1_INPUT_TIME_1r,
    CMIC_BS1_INPUT_TIME_2r,
    CMIC_BS1_OUTPUT_TIME_0r,
    CMIC_BS1_OUTPUT_TIME_1r,
    CMIC_BS1_OUTPUT_TIME_2r
};
#endif

static struct techsupport_cmd {
    const char *techsupport_cmd;
    int (*techsupport_func) (int unit, args_t *a,
                             techsupport_data_t *techsupport_feature_data);
} techsupport_cmds[] = {
    {COMMAND_BASIC, techsupport_basic},
    {COMMAND_L3MC, techsupport_l3mc},
    {COMMAND_L3UC, techsupport_l3uc},
    {COMMAND_MMU, techsupport_mmu},
    {COMMAND_MPLS, techsupport_mpls},
    {COMMAND_NIV, techsupport_niv},
    {COMMAND_RIOT, techsupport_riot},
    {COMMAND_VLAN, techsupport_vlan},
    {COMMAND_VXLAN, techsupport_vxlan},
    {COMMAND_IFP, techsupport_ifp},
    {COMMAND_VFP, techsupport_vfp},
    {COMMAND_EFP, techsupport_efp},
    {COMMAND_COS, techsupport_cos},
    {COMMAND_LOAD_BALANCE, techsupport_loadbalance},
    {COMMAND_OAM, techsupport_oam},
    {COMMAND_TCB, techsupport_tcb},
    {COMMAND_DGM, techsupport_dgm},
    {COMMAND_PSTAT, techsupport_pstat},
    {COMMAND_FLEX_FLOW, techsupport_flex_flow},
    {COMMAND_PHY, techsupport_phy},
    {COMMAND_LEDUP, techsupport_ledup},
    {COMMAND_CMIC, techsupport_cmic},
    {COMMAND_VISIBILITY, techsupport_visibility},
#ifdef BCM_MONTEREY_SUPPORT
    {COMMAND_CPRI, techsupport_cpri},
    {COMMAND_RSVD4, techsupport_rsvd4},
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
    {COMMAND_WARMBOOT, techsupport_warmboot},
#endif
    {COMMAND_ETRAP, techsupport_etrap},
    {COMMAND_INT, techsupport_int},
    {COMMAND_LATENCY_HISTOGRAM, techsupport_histogram},
    {COMMAND_ALPM, techsupport_alpm},
    {COMMAND_CANCUN, techsupport_cancun},
#ifdef INCLUDE_XFLOW_MACSEC
    {COMMAND_XFLOW_MACSEC, techsupport_xflow_macsec},
#endif
#ifdef BCM_FLOWTRACKER_SUPPORT
    {COMMAND_FLOWTRACKER, techsupport_flowtracker},
#endif
    {NULL, NULL } /* Must be the last element in this structure */
  };

/* Function:  techsupport_calc_and_print_time_taken
 * Purpose :  Calculate and print the take taken
 * Parameters: start_time - start time in seconds
 *             end_time -   end time in seconds
 *             str      - string.
 *             print_header_footer_string - 1 or 0
*/
void techsupport_calc_and_print_time_taken(sal_time_t start_time,
                        sal_time_t end_time, char *str,
                        int print_header_footer_string)
{
    unsigned long hrs, mins, secs;
    sal_time_t time_diff;

    if (end_time < start_time) {
      return ;
    }

    time_diff = end_time - start_time;
    secs = time_diff % 60;
    time_diff = time_diff / 60;
    mins = time_diff % 60;
    hrs = time_diff / 60;

    if ((0 != secs) || (0 != mins) || (0 != hrs)) {
        if (1 == print_header_footer_string) {
          cli_out("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
        }
        cli_out("\nTime taken to execute \"%s\""
                " is %02d:%02d:%02d (hh:mm:ss).\n", str, (int)hrs, (int)mins, (int)secs);
        if (1 == print_header_footer_string) {
          cli_out(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        }
    }
    return;
}
/* Function:  techsupport_command_execute
 * Purpose :  Executes the specified "command" and prints the time
 *            taken to execute the specified "command".
 * Parameters: unit - Unit number
 *             command - pointer to command.
*/
void
techsupport_command_execute(int unit, char *command)
{
    int  diff = 0, diff1 = 0, j = 0;
    char str[MAX_STR_LEN] = {0};
    char *pattern1 = "Output of \"";
    char *pattern2 = "\" start";
    char *pattern3 = "\" end";
    sal_time_t  start_time, end_time;
    /* Before actually dumping the command output, head string is
     * printed. Example of head string is something like below.
     * >>>>>>>>>>>>>>>>>>>>>>Output of "config show" start>>>>>>>>>>>>>>>>>>>
     * The below logic froms the head string.
    */
    memset(str, 0, MAX_STR_LEN);
    diff = MAX_STR_LEN - (strlen(command) + strlen(pattern1) + strlen(pattern2));
    diff = (diff % 2) == 0  ?  diff : ( diff + 1 );

    for (j=0; j < (diff / 2); j++) {
        str[j]='>';
    }

    cli_out("\n%s%s%s%s%s\n\n", str, pattern1, command, pattern2,str);
    start_time = sal_time();
    sh_process_command(unit, command);
    end_time = sal_time();

    /* After dumping the command output, tail string is
     * printed. Example of tail string is something like below.
     * <<<<<<<<<<<<<<<<<<<<<<Output of "config show" end<<<<<<<<<<<<<<<<<<<<<<<
     * The below logic froms the tail string.
    */

    memset(str, 0, MAX_STR_LEN);
    diff1 = MAX_STR_LEN - (strlen(command) + strlen(pattern1) + strlen(pattern3));
    diff1 = (diff1 % 2) == 0  ?  diff1 : ( diff1 + 1 );

    for (j=0; j < (diff1 / 2); j++) {
        str[j]='<';
    }
    cli_out("\n%s%s%s%s%s\n", str, pattern1, command, pattern3, str);

    techsupport_calc_and_print_time_taken(start_time, end_time, command, 0);
}
/* Function:   techsupport_feature_process
 *  Purpose :  Executes the following
 *             1) diag shell comands(techsupport_feature_data->techsupport_data_diag_cmdlist[])
 *             2) dumps the memories(techsupport_feature_data->techsupport_data_mem_list[])
 *             3) dumps the registers(techsupport_feature_data->techsupport_data_reg_list[])
 * Parameters: unit - Unit number
 *             a - pointer to argument.
 *             techsupport_data_t -structure that maintains per chip per feature debug info.
 * Returns:    CMD_OK :done
 *             CMD_FAIL : INVALID INPUT
 */
int
techsupport_feature_process(int unit, args_t *a,
                          techsupport_data_t *techsupport_feature_data)
{
    int i=0;
    char *arg1;
    char command_str[MAX_STR_LEN] ={0}, command_options_flag = 0, dump_all = 1;
    char dont_execute = 0, script_generate = 0;
#ifdef BCM_HELIX5_SUPPORT
    int j = 0, skip_hx5_cmic_reg = 0;
#endif

    /* By default, dump only the changes */
    command_options_flag |= DUMP_TABLE_CHANGED;

    /* Parse the options */
    arg1 = ARG_GET(a);
    for (;;) {
        if (arg1 != NULL && !sal_strcasecmp(arg1, VERBOSE)) {
            command_options_flag &= ~(DUMP_TABLE_CHANGED);
            arg1 = ARG_GET(a);
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, REG)) {
            command_options_flag |= DUMP_REGISTERS;
            arg1 = ARG_GET(a);
            dump_all = 0;
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, MEM)) {
            command_options_flag |= DUMP_MEMORIES;
            arg1 = ARG_GET(a);
            dump_all = 0;
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, DIAG)) {
            command_options_flag |= DUMP_DIAG_CMDS;
            arg1 = ARG_GET(a);
            dump_all = 0;
#ifndef BCM_SW_STATE_DUMP_DISABLE
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, SW)) {
            command_options_flag |= DUMP_SW;
            arg1 = ARG_GET(a);
            dump_all = 0;
#endif /* BCM_SW_STATE_DUMP_DISABLE */
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, LIST)) {
            command_options_flag |= DUMP_LIST;
            arg1 = ARG_GET(a);
            /* if "list" option is specified, just list out the following
             * with out executing.
             * 1)diag commands
             * 2)Memory names
             * 3)Register names
             * 4)software dump commands
             */
            dont_execute = 1;
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, SCRIPT_GEN)) {
            arg1 = ARG_GET(a);
            /* if "script-generate" option is specified, just list out the commands
             * with out executing.
             */
            dont_execute = 1;
            script_generate = 1;
        }

        else {
            break;
        }
    }

    /* Displays the output of feature specific diag commands */
    if (dump_all == 1 || (command_options_flag & DUMP_DIAG_CMDS)) {
        if (1 == dont_execute) {
            if (1 == script_generate) {
                cli_out("echo \"List of Diag commands\"\n");
            } else {
                cli_out("\nList of Diag commands:\n");
                cli_out(">>>>>>>>>>>>>>>>>>>>>>\n");
          }
        }
        for(i = 0; techsupport_feature_data->techsupport_data_diag_cmdlist[i] != NULL; i++) {
            if (1 == dont_execute) {
                if (1 == script_generate) {
                    cli_out("echo \"Executing command \"\\\"%s\\\"\n", techsupport_feature_data->techsupport_data_diag_cmdlist[i]);
                } 
                cli_out("%s\n", techsupport_feature_data->techsupport_data_diag_cmdlist[i]);
            } else {
                techsupport_command_execute(unit, techsupport_feature_data->techsupport_data_diag_cmdlist[i]);
            }
        }

        for(i = 0; techsupport_feature_data->techsupport_data_device_diag_cmdlist[i] != NULL; i++) {
            if (1 == dont_execute) {
                if (1 == script_generate) {
                    cli_out("echo \"Executing command \"\\\"%s\\\"\n", techsupport_feature_data->techsupport_data_device_diag_cmdlist[i]);
                }
                cli_out("%s\n", techsupport_feature_data->techsupport_data_device_diag_cmdlist[i]);
            } else {
                techsupport_command_execute(unit, techsupport_feature_data->techsupport_data_device_diag_cmdlist[i]);
            }
        }
    }

    /* Dumps feature specific memory tables */
    if (dump_all == 1 || (command_options_flag & DUMP_MEMORIES)) {
        if (1 == dont_execute) {
            if (1 == script_generate) {
                cli_out("echo \"List of memory dumps\"\n");
            } else {
                cli_out("\nList of Memory Table names:\n");
                cli_out(">>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
            }
        }
        for (i=0; techsupport_feature_data->techsupport_data_mem_list[i] != INVALIDm ; i++) {
            if ((1 == dont_execute) && (0 == script_generate)) {
                cli_out("%s\n", SOC_MEM_NAME(unit, techsupport_feature_data->techsupport_data_mem_list[i]));
            } else {
                if (SOC_MEM_IS_VALID(unit, techsupport_feature_data->techsupport_data_mem_list[i])) { 
                    sal_strlcpy(command_str, DUMP, sizeof(command_str));

                    if (command_options_flag & DUMP_TABLE_CHANGED) {
                        sal_strncat(command_str, CHANGE, (MAX_STR_LEN - strlen(command_str) - 1));
                    }
                    sal_strncat(command_str,
                            SOC_MEM_NAME(unit, techsupport_feature_data->techsupport_data_mem_list[i]),
                            (MAX_STR_LEN - strlen(command_str) - 1));
                    if ((1 == dont_execute) && (1  == script_generate)) {
                        cli_out("echo \"Executing command \"\\\"%s\\\"\n", command_str);
                        cli_out("%s\n", command_str);
                    } else if ((0 == dont_execute) && (0 == script_generate)) {
                        techsupport_command_execute(unit, command_str);
                    }
                }
            }
        }
    }

#ifndef BCM_SW_STATE_DUMP_DISABLE
    /* Displays the output of sofware dump  diag commands */
    if (dump_all == 1 || (command_options_flag & DUMP_SW)) {
        if (1 == dont_execute) {
            if (1 == script_generate) {
                cli_out("echo \"Software dump commands\"\n");
            } else {
                cli_out("\nSoftware dump commands:\n");
                cli_out(">>>>>>>>>>>>>>>>>>>>>>\n");
          }
        }
        for(i = 0; techsupport_feature_data->techsupport_data_sw_dump_cmdlist[i] != NULL; i++) {
            if (1 == dont_execute) {
                if (1 == script_generate) {
                    cli_out("echo \"Executing command \"\\\"%s\\\"\n", techsupport_feature_data->techsupport_data_sw_dump_cmdlist[i]);
                }
                cli_out("%s\n", techsupport_feature_data->techsupport_data_sw_dump_cmdlist[i]);
            } else {
                techsupport_command_execute(unit, techsupport_feature_data->techsupport_data_sw_dump_cmdlist[i]);
            }
        }

        for(i = 0; techsupport_feature_data->techsupport_data_device_sw_dump_cmdlist[i] != NULL; i++) {
            if (1 == dont_execute) {
                if (1 == script_generate) {
                    cli_out("echo \"Executing command \"\\\"%s\\\"\n", techsupport_feature_data->techsupport_data_device_sw_dump_cmdlist[i]);
                }
                cli_out("%s\n", techsupport_feature_data->techsupport_data_device_sw_dump_cmdlist[i]);
            } else {
                techsupport_command_execute(unit, techsupport_feature_data->techsupport_data_device_sw_dump_cmdlist[i]);
            }

        }

    }
#endif /* BCM_SW_STATE_DUMP_DISABLE */

    /* Dumps feature specific registers */
    if (dump_all == 1 || (command_options_flag & DUMP_REGISTERS)) {
        if (1 == dont_execute) {
            if (1 == script_generate) {
                cli_out("echo \"List of Register dumps\"\n");
            } else {
                cli_out("\nList of Registers:\n");
                cli_out(">>>>>>>>>>>>>>>>>>\n");
            }
        }

        for (i = 0; techsupport_feature_data->techsupport_data_reg_list[i].reg != INVALIDr; i++) {
            if ((1 == dont_execute) && (0 == script_generate)) {
                cli_out("%s\n", SOC_REG_NAME(unit, techsupport_feature_data->techsupport_data_reg_list[i].reg));
            } else {
                if(SOC_REG_IS_VALID(unit, techsupport_feature_data->techsupport_data_reg_list[i].reg)) {
#ifdef BCM_HELIX5_SUPPORT
                    
                    if (SOC_IS_HELIX5(unit)) {
                        skip_hx5_cmic_reg = 0;
                        /* Check if this is a skipped a reg on HX5. if so just continue */
                        for (j = 0; j < (COUNTOF(hx5_cmic_broadsync_reg_skip_list)); j++) {
                            if (techsupport_feature_data->techsupport_data_reg_list[i].reg ==
                                    hx5_cmic_broadsync_reg_skip_list[j]) {
                                skip_hx5_cmic_reg = 1;
                                break;
                            }
                        }

                        if (skip_hx5_cmic_reg) {
                            continue;
                        }
                    }
#endif /* BCM_HELIX5_SUPPORT */
                    sal_strlcpy(command_str, GET_REG, sizeof(command_str));
                    if (command_options_flag & DUMP_TABLE_CHANGED){
                        sal_strncat(command_str, CHANGE, (MAX_STR_LEN - strlen(command_str) - 1));
                    }
                    sal_strncat(command_str,  SOC_REG_NAME(unit, techsupport_feature_data->techsupport_data_reg_list[i].reg),
                            (MAX_STR_LEN - strlen(command_str) - 1));
                    if ((1 == dont_execute) && (1  == script_generate)) {
                        cli_out("echo \"Executing command \"\\\"%s\\\"\n", command_str);
                        cli_out("%s\n", command_str);
                    } else if ((0 == dont_execute) && (0 == script_generate)) {
                        techsupport_command_execute(unit, command_str);
                    }
                }
            }
        }
    }
    return CMD_OK;
}
/* Function:    command_techsupport
 * Purpose :    Displays all the debug info for a given subfeature.
 * Parameters:  unit - Unit number
 *              a - pointer to argument.
 * Returns:     CMD_OK :done
 *              CMD_FAIL : INVALID INPUT
 */
cmd_result_t
command_techsupport(int unit, args_t *a)
{
    int rc = CMD_OK;
    sal_time_t  start_time = 0, end_time = 0;
    sal_time_t  start_time_all = 0, end_time_all = 0;
    char *feature_name = ARG_GET(a);
    char feature_name_supported_flag = 0;
    struct techsupport_cmd *command;
    techsupport_data_t *techsupport_feature_data;
    mbcm_techsupport_t *mbcm_techsupport_ptr;
    char str[MAX_STR_LEN] ={0};
    if (feature_name == NULL) {
      return CMD_USAGE;
    }

    if (sal_strcasecmp(feature_name, "basic") == 0) {
        /*No sub-options allowed after "techsupport basic"*/
        if (NULL != ARG_GET(a)) {
            return CMD_USAGE;
        }
        start_time = sal_time();
        rc  = techsupport_basic(unit, a, NULL);
        end_time = sal_time();
    }
     else {
        if (NULL == mbcm_techsupport[unit]) {
#if defined BCM_TOMAHAWK3_SUPPORT
            if(SOC_IS_TOMAHAWK3(unit)) {
                mbcm_techsupport[unit] = mbcm_tomahawk3_techsupport;
            } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
            if (SOC_IS_TRIDENT2PLUS(unit)) {
                mbcm_techsupport[unit] = mbcm_trident2plus_techsupport;
            }
#ifdef  BCM_MONTEREY_SUPPORT
            else if (SOC_IS_MONTEREY(unit)) {
                mbcm_techsupport[unit] = mbcm_monterey_techsupport;
            }
#endif
#ifdef  BCM_APACHE_SUPPORT
            else if (SOC_IS_APACHE(unit)) {
                mbcm_techsupport[unit] = mbcm_apache_techsupport;
            }
#endif 
#ifdef  BCM_SABER2_SUPPORT
            else if (SOC_IS_SABER2(unit)) {
                mbcm_techsupport[unit] = mbcm_saber2_techsupport;
            }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
            else if (SOC_IS_TRIDENT2(unit)) {
                mbcm_techsupport[unit] = mbcm_trident2_techsupport;
            }
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
            else if (SOC_IS_TOMAHAWK2(unit)) {
                mbcm_techsupport[unit] = mbcm_tomahawk2_techsupport;
            }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
            else if (SOC_IS_TOMAHAWK(unit)) {
                mbcm_techsupport[unit] = mbcm_tomahawk_techsupport;
            }
#endif
#ifdef BCM_HURRICANE4_SUPPORT
            else if (SOC_IS_HURRICANE4(unit)) {
                mbcm_techsupport[unit] = mbcm_hurricane4_techsupport;
            }
#endif
#ifdef BCM_HELIX5_SUPPORT
            else if (SOC_IS_HELIX5(unit)) {
                mbcm_techsupport[unit] = mbcm_helix5_techsupport;
            }
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
            else if (SOC_IS_FIREBOLT6(unit)) {
                mbcm_techsupport[unit] = mbcm_firebolt6_techsupport;
            }
#endif
#ifdef BCM_TRIDENT3_SUPPORT
            else if (SOC_IS_TRIDENT3X(unit)) {
                mbcm_techsupport[unit] = mbcm_trident3_techsupport;
            }
#endif
            else {
                cli_out("command \"techSupport %s\" not supported on chip %s.\n", feature_name, SOC_UNIT_NAME(unit));
                return CMD_FAIL;
            }
        }

        mbcm_techsupport_ptr = mbcm_techsupport[unit];

        /*techsupport all*/
        if (sal_strcasecmp(feature_name, "all") == 0) {
            /*No sub-options allowed after "techsupport all"*/
            if (NULL != ARG_GET(a)) {
                return CMD_USAGE;
            }
            cli_out("START: Executing \"techSupport all\".\n");
            start_time_all = sal_time();
            cli_out("START: Executing \"techSupport basic\".\n");
            start_time = sal_time();
            rc  = techsupport_basic(unit, a, NULL);
            end_time = sal_time();
            cli_out("END: Executing \"techSupport basic\".\n");
            sal_strlcpy(str, TECHSUPPORT, sizeof(str));
            sal_strncat(str, "basic", (sizeof(str) - strlen(str) - 1));
            techsupport_calc_and_print_time_taken(start_time, end_time, str, 1);
            for (; mbcm_techsupport_ptr->techsupport_feature_name; mbcm_techsupport_ptr++) {
                techsupport_feature_data = mbcm_techsupport_ptr->techsupport_data;
                for (command = techsupport_cmds; command->techsupport_cmd; command++) {
                    if (sal_strcasecmp(mbcm_techsupport_ptr->techsupport_feature_name, command->techsupport_cmd) == 0) {
                        cli_out("START: Executing \"techSupport %s\".\n", mbcm_techsupport_ptr->techsupport_feature_name);
                        start_time = sal_time();
                        rc = (command->techsupport_func(unit, a, techsupport_feature_data));
                        end_time = sal_time();
                        cli_out("END: Executing \"techSupport %s\".\n", mbcm_techsupport_ptr->techsupport_feature_name);
                        sal_strlcpy(str, TECHSUPPORT, sizeof(str));
                        sal_strncat(str, mbcm_techsupport_ptr->techsupport_feature_name, (sizeof(str) - strlen(str) - 1));
                        techsupport_calc_and_print_time_taken(start_time, end_time, str, 1);
                        break;
                    }
                }
            }
            end_time_all = sal_time();
            cli_out("END: Executing \"techSupport all\".\n");
            sal_strlcpy(str, TECHSUPPORT, sizeof(str));
            sal_strncat(str, "all", (sizeof(str) - strlen(str) - 1));
            techsupport_calc_and_print_time_taken(start_time_all, end_time_all, str, 1);
            return rc;
        } else { /*if (sal_strcasecmp(feature_name, "all") == 0)*/
            for (; mbcm_techsupport_ptr->techsupport_feature_name; mbcm_techsupport_ptr++) {
                if (sal_strcasecmp(feature_name, mbcm_techsupport_ptr->techsupport_feature_name) == 0) {
                    techsupport_feature_data = mbcm_techsupport_ptr->techsupport_data;
                    feature_name_supported_flag = 1;
                    break;
                }
            }

            if (feature_name_supported_flag == 0) {
                cli_out("command \"techSupport %s\" not supported on chip %s.\n", feature_name, SOC_UNIT_NAME(unit));
                return CMD_FAIL;
            }

            for (command = techsupport_cmds; command->techsupport_cmd; command++) {
                if (sal_strcasecmp(feature_name, command->techsupport_cmd) == 0) {
                    start_time = sal_time();
                    rc = (command->techsupport_func(unit, a, techsupport_feature_data));
                    end_time = sal_time();
                    break;
                }
            }
        }
    }

    sal_strlcpy(str, TECHSUPPORT, sizeof(str));
    sal_strncat(str, feature_name, (sizeof(str) - strlen(str) - 1));
    sal_strncat(str, " [options..]", (sizeof(str) - strlen(str) - 1));
    techsupport_calc_and_print_time_taken(start_time, end_time, str, 1);
    return rc;
}
