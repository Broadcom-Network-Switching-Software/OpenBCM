/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM soc header for BCM53570
 */

#ifndef TDM_BCM53570_PREPROCESSOR_SOC_DEFINES_H
#define TDM_BCM53570_PREPROCESSOR_SOC_DEFINES_H

#ifdef _TDM_STANDALONE
	#include <tdm_gh2_defines.h>
#else
	#include <soc/tdm/greyhound2/tdm_gh2_defines.h>
#endif

#define _SOC_GH2_TDM_LR_VBS_LEN       256
#define _SOC_GH2_TDM_OS_VBS_GRP_LEN   12
#define _SOC_GH2_TDM_NUM_EXT_PORTS    136
#define _SOC_GH2_TDM_NUM_PM_MOD       33

typedef struct _tdm_gh2_pipes_s {
	int idb_tdm_tbl_0[_SOC_GH2_TDM_LR_VBS_LEN];
	int idb_tdm_ovs_0_a[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_b[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_c[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_d[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_e[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_f[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_g[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_h[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_tbl_1[_SOC_GH2_TDM_LR_VBS_LEN];
	int idb_tdm_ovs_1_a[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_b[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_c[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_d[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_e[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_f[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_g[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_h[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_tbl_2[_SOC_GH2_TDM_LR_VBS_LEN];
	int idb_tdm_ovs_2_a[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_b[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_c[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_d[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_e[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_f[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_g[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_h[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_tbl_3[_SOC_GH2_TDM_LR_VBS_LEN];
	int idb_tdm_ovs_3_a[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_b[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_c[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_d[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_e[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_f[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_g[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_h[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_tbl_0[_SOC_GH2_TDM_LR_VBS_LEN];
	int mmu_tdm_ovs_0_a[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_b[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_c[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_d[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_e[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_f[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_g[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_h[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_tbl_1[_SOC_GH2_TDM_LR_VBS_LEN];
	int mmu_tdm_ovs_1_a[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_b[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_c[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_d[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_e[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_f[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_g[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_h[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_tbl_2[_SOC_GH2_TDM_LR_VBS_LEN];
	int mmu_tdm_ovs_2_a[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_b[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_c[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_d[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_e[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_f[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_g[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_h[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_tbl_3[_SOC_GH2_TDM_LR_VBS_LEN];
	int mmu_tdm_ovs_3_a[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_b[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_c[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_d[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_e[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_f[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_g[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_h[_SOC_GH2_TDM_OS_VBS_GRP_LEN];
} _tdm_gh2_pipes_t;

#define _TDM_LENGTH                    256
#define _OVS_GROUP_COUNT               6
#define _OVS_GROUP_TDM_LENGTH          12
#define NULL_TOKEN                     253

#define PORT_HIGIG2 999
#define PORT_ETHERNET 998

/* Value hardcoded in set_tdm.c, definition needs to be matched */
#define PORT_STATE_UNUSED              0
#define PORT_STATE_LINERATE            1
#define PORT_STATE_OVERSUB             2
#define PORT_STATE_CONTINUATION        3 /* part of other port */

#define GH2_PIPES_PER_DEV 4
#define GH2_PBLKS_PER_DEV 32

typedef struct _tdm_gh2_globals_s {
  enum port_speed_e speed[_SOC_GH2_TDM_NUM_EXT_PORTS];
  int clk_freq;
  int port_rates_array[_SOC_GH2_TDM_NUM_EXT_PORTS];
  int pm_encap_type[_SOC_GH2_TDM_NUM_PM_MOD];
} _tdm_gh2_globals_t;

typedef struct _soc_gh2_tdm_s {
    int speed[GH2_NUM_EXT_PORTS];
    int port_state[GH2_NUM_EXT_PORTS]; /* 0-unused, 1-line rate, 2-oversub,
									   * 3-part of other port 
									   */
    /*
    int idb_tdm[GH2_PIPES_PER_DEV][_TDM_LENGTH];
    int idb_ovs_tdm[GH2_PIPES_PER_DEV][_OVS_GROUP_COUNT][_OVS_GROUP_TDM_LENGTH];
    int mmu_tdm[GH2_PIPES_PER_DEV][_TDM_LENGTH];
    int mmu_ovs_tdm[GH2_PIPES_PER_DEV][_OVS_GROUP_COUNT][_OVS_GROUP_TDM_LENGTH];
    */
    int *idb_tdm[GH2_PIPES_PER_DEV];
    int *idb_ovs_tdm[GH2_PIPES_PER_DEV][_OVS_GROUP_COUNT];
    int *mmu_tdm[GH2_PIPES_PER_DEV];
    int *mmu_ovs_tdm[GH2_PIPES_PER_DEV][_OVS_GROUP_COUNT];
    
	int pm_encap_type[GH2_NUM_PM_MOD];
	
    _tdm_gh2_globals_t tdm_globals;
    _tdm_gh2_pipes_t tdm_pipe_tables;	

    /* Following fields are not arguments to the TDM code */
    int port_ratio[GH2_PBLKS_PER_DEV];
    int ovs_ratio_x1000[GH2_PIPES_PER_DEV];
} _soc_gh2_tdm_t;

#endif
