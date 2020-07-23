/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM soc header for BCM56275
 */

#ifndef TDM_BCM56275_PREPROCESSOR_SOC_DEFINES_H
#define TDM_BCM56275_PREPROCESSOR_SOC_DEFINES_H

#ifdef _TDM_STANDALONE
	#include <tdm_hr4_defines.h>
#else
	#include <soc/tdm/hurricane4/tdm_hr4_defines.h>
#endif

/* Speed index used in oversub half pipes */
enum hr4_port_speed_indx_e {
    HR4_SPEED_IDX_10G=0,
    HR4_SPEED_IDX_20G=1,
    HR4_SPEED_IDX_25G=2,
    HR4_SPEED_IDX_40G=3,
    HR4_SPEED_IDX_50G=4, 
    HR4_SPEED_IDX_100G=5
};
/* flexport status */
enum hr4_flex_pm_state_e {
   HR4_FLEXPORT_PM_DOWN_DOWN=0,
   HR4_FLEXPORT_PM_SAME=1,
   HR4_FLEXPORT_PM_SOME_SAME=2,
   HR4_FLEXPORT_PM_UP_DOWN=3,
   HR4_FLEXPORT_PM_DOWN_UP=4,
   HR4_FLEXPORT_PM_UP_UP=5
};


typedef struct {
	int  ovs_tables[2][HR4_OS_VBS_GRP_NUM/2][HR4_OS_VBS_GRP_LEN]; /* indexed by HP, ovs group num; ovs group index */
	enum port_speed_e ovs_grp_speed[2][HR4_OS_VBS_GRP_NUM/2];
	int  ovs_grp_weight[2][HR4_OS_VBS_GRP_NUM/2];
	int  prev_num_grps[2][6];  /* first index is HP num; second index is the speed port_speed_indx_e*/
	int  new_num_grps[2][6];   /* first index is HP num; second index is the speed port_speed_indx_e*/
	int  ports_to_be_removed[HR4_NUM_EXT_PORTS];  /* indexed by phy_port; set to 1 if needs to be removed */
	int  ports_to_be_added[HR4_NUM_EXT_PORTS];    /* indexed by phy_port; set to 1 if needs to be added   */
	int  ovs_grp_pms[2][HR4_OS_VBS_GRP_NUM/2][HR4_OS_VBS_GRP_LEN]; /* indicates which PMs contains each group */

	int  skip_ovs_for_speed[2][6];   /* first index is HP num; second index is the speed port_speed_indx_e*/
} hr4_flxport_t;

#endif
