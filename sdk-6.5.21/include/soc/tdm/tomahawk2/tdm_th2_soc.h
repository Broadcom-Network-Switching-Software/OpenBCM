/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM soc header for BCM56970
 */

#ifndef TDM_BCM56970_PREPROCESSOR_SOC_DEFINES_H
#define TDM_BCM56970_PREPROCESSOR_SOC_DEFINES_H

#ifdef _TDM_STANDALONE
	#include <tdm_th2_defines.h>
#else
	#include <soc/tdm/tomahawk2/tdm_th2_defines.h>
#endif

#define TDM_CHIP_EXEC__ACCESSORIZE 13


enum port_speed_indx_e { INDX_10G=0, INDX_20G=1, INDX_25G=2, INDX_40G=3, INDX_50G=4, INDX_100G=5};
enum flexport_pm_state_e { FLEXPORT_PM_DOWN_DOWN=0, FLEXPORT_PM_SAME=1, FLEXPORT_PM_SOME_SAME=2, FLEXPORT_PM_UP_DOWN=3, FLEXPORT_PM_DOWN_UP=4, FLEXPORT_PM_UP_UP=5};

typedef struct {
	int  ovs_tables[2][TH2_OS_VBS_GRP_NUM/2][TH2_OS_VBS_GRP_LEN]; /* indexed by HP, ovs group num; ovs group index */
	enum port_speed_e ovs_grp_speed[2][TH2_OS_VBS_GRP_NUM/2];
	int               ovs_grp_weight[2][TH2_OS_VBS_GRP_NUM/2];
	int  prev_num_grps[2][6];  /* first index is HP num; second index is the speed port_speed_indx_e*/
	int  new_num_grps[2][6];   /* first index is HP num; second index is the speed port_speed_indx_e*/
	int  ports_to_be_removed[TH2_NUM_EXT_PORTS];  /* indexed by phy_port; set to 1 if needs to be removed */
	int  ports_to_be_added[TH2_NUM_EXT_PORTS];    /* indexed by phy_port; set to 1 if needs to be added   */
	int  ovs_grp_pms[2][TH2_OS_VBS_GRP_NUM/2][TH2_OS_VBS_GRP_LEN]; /* indicates which PMs contains each group */

	int  skip_ovs_for_speed[2][6];   /* first index is HP num; second index is the speed port_speed_indx_e*/
} th2_flxport_t;

#endif
