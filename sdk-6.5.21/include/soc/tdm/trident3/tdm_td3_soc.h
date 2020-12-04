/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM soc header for BCM56870
 */

#ifndef TDM_BCM56870_PREPROCESSOR_SOC_DEFINES_H
#define TDM_BCM56870_PREPROCESSOR_SOC_DEFINES_H

#ifdef _TDM_STANDALONE
	#include <tdm_td3_defines.h>
#else
	#include <soc/tdm/trident3/tdm_td3_defines.h>
#endif

/* Speed index used in oversub half pipes */
enum td3_port_speed_indx_e {
    TD3_SPEED_IDX_10G=0,
    TD3_SPEED_IDX_20G=1,
    TD3_SPEED_IDX_25G=2,
    TD3_SPEED_IDX_40G=3,
    TD3_SPEED_IDX_50G=4, 
    TD3_SPEED_IDX_100G=5
};
/* flexport status */
enum td3_flex_pm_state_e {
    TD3_FLEXPORT_PM_DOWN_DOWN=0,
    TD3_FLEXPORT_PM_SAME=1,
    TD3_FLEXPORT_PM_SOME_SAME=2,
    TD3_FLEXPORT_PM_UP_DOWN=3,
    TD3_FLEXPORT_PM_DOWN_UP=4,
    TD3_FLEXPORT_PM_UP_UP=5
};
/* tdm_td3_ovsb_pm_t */
typedef struct tdm_td3_pm_s {
    int pm_en;          /* pm enable               */
    int pm_num;         /* pm num                  */
    int pm_bw;          /* total speed of pm       */
    int pm_max_spd;     /* max port speed of pm    */
} tdm_td3_ovsb_pm_t;

#endif
