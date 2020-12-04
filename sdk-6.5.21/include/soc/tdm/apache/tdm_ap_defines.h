/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM macro values for BCM56860
 */

#ifndef TDM_AP_PREPROCESSOR_MACROS_H
#define TDM_AP_PREPROCESSOR_MACROS_H

/* Default number of PM in chip */
#define AP_NUM_PM_MOD 19
#define AP_NUM_PHY_PM 18
/* Default number of lanes per PM in chip */
#define AP_NUM_PM_LNS 4
/* Allocation length of port lookup tables */
#define AP_NUM_EXT_PORTS 130
/* Number of the hardware designated management PM */
#define AP_MGMT_TSC_ID 3

#define AP_CL1_TSC_ID 7
#define AP_CL2_TSC_ID 8
#define AP_CL4_TSC_ID 16
#define AP_CL5_TSC_ID 17
#define AP_CL1_PORT 29
#define AP_CL2_PORT 33
#define AP_CL4_PORT 65
#define AP_CL5_PORT 69

/* Number of possible physical ports */
#define AP_NUM_PHY_PORTS 72
/* Number of scheduling blocks */
#define AP_NUM_QUAD 2

/* Default number of ancillary ports in calendar */
#define AP_ACC_PORT_NUM 9

/* Allocation length of LLS line rate calendar */
#define AP_TDM_AUX_SIZE 72
#define AP_LR_LLS_LEN 64
/* Allocation length of VBS line rate calendar */
#define AP_LR_VBS_LEN 512
/* Allocation length of IARB static calendar */
#define AP_LR_IARB_STATIC_LEN 512
/* Allocation area of LLS port groups */
#define AP_OS_LLS_GRP_NUM 2
#define AP_OS_LLS_GRP_LEN 40
/* Allocation area of VBS port groups */
#define AP_OS_VBS_GRP_NUM 8
#define AP_OS_VBS_GRP_LEN 16

/* Architecturally specific allocation area of VBS vector map */
#define AP_VMAP_MAX_LEN 256
#define AP_VMAP_MAX_WID 55

/* Tokenization values */
#ifdef _TDM_STANDALONE
	/* Token for pmap management port */
	#define AP_MGMT_TOKEN (AP_NUM_EXT_PORTS+3) 
	/* Token for granularity scaling */
	#define AP_US1G_TOKEN (AP_NUM_EXT_PORTS+4) 
	/* Token for ancillary soc functions */
	#define AP_ANCL_TOKEN 103
    /* Token for oversub round robin */
    #define AP_OVSB_TOKEN 75
    /* Memreset, L2 management, other */
    #define AP_IDL1_TOKEN 76
    /* EP refresh, idle cycle */
    #define AP_IDL2_TOKEN 77
  /* EP aux cycle */
    #define AP_AUX_TOKEN 78
  /* Dedicated SBUS Op */
    #define AP_SBUS_TOKEN 81
#else
	/* Token for pmap management port */
	#define AP_MGMT_TOKEN (AP_NUM_EXT_PORTS+3) 
	/* Token for granularity scaling */
	#define AP_US1G_TOKEN (AP_NUM_EXT_PORTS+4) 
	/* Token for ancillary soc functions */
	#define AP_ANCL_TOKEN 103
    /* Token for oversub round robin */
    #define AP_OVSB_TOKEN 90
    /* Memreset, L2 management, other */
    #define AP_IDL1_TOKEN 91
    /* EP refresh, idle cycle */
    #define AP_IDL2_TOKEN AP_IDL1_TOKEN
  /* EP aux cycle */
    #define AP_AUX_TOKEN 93
  /* Dedicated SBUS Op */
    #define AP_SBUS_TOKEN 94
#endif

#define MMU_OVRSUB_PORT 57
#define MMU_NULL_PORT   58

/* Static port identities */
/*#define AP_MGMT_PORT_0 103 */ /*nak,dummy MGMT PORT defines*/
/*#define AP_MGMT_PORT_1 104
#define AP_MGMT_PORT_2 105
#define AP_MGMT_PORT_3 106*/
#define AP_LOOPBACK_PORT 73
#define AP_CPU_PORT 0

/* Macro definitions */
#define AP_PIPE_X_ID 0
#define AP_PIPE_Y_ID 1

#define AP_XPIPE_CAL_ID 2
#define AP_YPIPE_CAL_ID 5


#endif /* TDM_AP_PREPROCESSOR_MACROS_H */
