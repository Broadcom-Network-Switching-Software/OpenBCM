/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM soc header for BCM56870
 */

#ifndef TDM_BCM56370_PREPROCESSOR_SOC_DEFINES_H
#define TDM_BCM56370_PREPROCESSOR_SOC_DEFINES_H

#ifdef _TDM_STANDALONE
	#include <tdm_hx5_defines.h>
#else
	#include <soc/tdm/helix5/tdm_hx5_defines.h>
#endif

/* Speed index used in oversub half pipes */
enum hx5_port_speed_indx_e {
    HX5_SPEED_IDX_10G=0,
    HX5_SPEED_IDX_20G=1,
    HX5_SPEED_IDX_25G=2,
    HX5_SPEED_IDX_40G=3,
    HX5_SPEED_IDX_50G=4, 
    HX5_SPEED_IDX_100G=5
};
/* flexport status */
enum hx5_flex_pm_state_e {
   HX5_FLEXPORT_PM_DOWN_DOWN=0,
   HX5_FLEXPORT_PM_SAME=1,
   HX5_FLEXPORT_PM_SOME_SAME=2,
   HX5_FLEXPORT_PM_UP_DOWN=3,
   HX5_FLEXPORT_PM_DOWN_UP=4,
   HX5_FLEXPORT_PM_UP_UP=5
};


typedef struct {
	int  ovs_tables[2][HX5_OS_VBS_GRP_NUM/2][HX5_OS_VBS_GRP_LEN]; /* indexed by HP, ovs group num; ovs group index */
	enum port_speed_e ovs_grp_speed[2][HX5_OS_VBS_GRP_NUM/2];
	int  ovs_grp_weight[2][HX5_OS_VBS_GRP_NUM/2];
	int  prev_num_grps[2][6];  /* first index is HP num; second index is the speed port_speed_indx_e*/
	int  new_num_grps[2][6];   /* first index is HP num; second index is the speed port_speed_indx_e*/
	int  ports_to_be_removed[HX5_NUM_EXT_PORTS];  /* indexed by phy_port; set to 1 if needs to be removed */
	int  ports_to_be_added[HX5_NUM_EXT_PORTS];    /* indexed by phy_port; set to 1 if needs to be added   */
	int  ovs_grp_pms[2][HX5_OS_VBS_GRP_NUM/2][HX5_OS_VBS_GRP_LEN]; /* indicates which PMs contains each group */

	int  skip_ovs_for_speed[2][6];   /* first index is HP num; second index is the speed port_speed_indx_e*/
} hx5_flxport_t;

#endif
