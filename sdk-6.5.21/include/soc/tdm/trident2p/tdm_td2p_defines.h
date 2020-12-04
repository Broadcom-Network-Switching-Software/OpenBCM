/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM macro values for BCM56860
 */

#ifndef TDM_TD2P_PREPROCESSOR_MACROS_H
#define TDM_TD2P_PREPROCESSOR_MACROS_H

/* Default number of PM in chip */
#define TD2P_NUM_PM_MOD 33
#define TD2P_NUM_PHY_PM 32
/* Default number of lanes per PM in chip */
#define TD2P_NUM_PM_LNS 4
/* Allocation length of port lookup tables */
#define TD2P_NUM_EXT_PORTS 130
/* Number of the hardware designated management PM */
#define TD2P_MGMT_TSC_ID 3

/* Number of possible physical ports */
#define TD2P_NUM_PHY_PORTS 128
/* Number of scheduling blocks */
#define TD2P_NUM_QUAD 4

/* Default number of ancillary ports in calendar */
#define TD2P_ACC_PORT_NUM 10

/* Allocation length of LLS line rate calendar */
#define TD2P_LR_LLS_LEN 64
/* Allocation length of VBS line rate calendar */
#define TD2P_LR_VBS_LEN 256
/* Allocation length of IARB static calendar */
#define TD2P_LR_IARB_STATIC_LEN 512
/* Allocation area of LLS port groups */
#define TD2P_OS_LLS_GRP_NUM 2
#define TD2P_OS_LLS_GRP_LEN 32
/* Allocation area of VBS port groups */
#define TD2P_OS_VBS_GRP_NUM 8
#define TD2P_OS_VBS_GRP_LEN 16

/* Architecturally specific allocation area of VBS vector map */
#define TD2P_VMAP_MAX_LEN TD2P_LR_VBS_LEN
#define TD2P_VMAP_MAX_WID 128 

/* Tokenization values 
	TD2P_MGMT_TOKEN: for pmap management port
	TD2P_US1G_TOKEN: for granularity scaling
	TD2P_ANCL_TOKEN: for ancillary soc functions
	TD2P_OVSB_TOKEN: for oversub round robin
	TD2P_IDL1_TOKEN: for memory reset, L2 management, and other
	TD2P_IDL2_TOKEN: for EP refresh, and idle cycle
*/
#ifdef _TDM_STANDALONE
	#define TD2P_MGMT_TOKEN (TD2P_NUM_EXT_PORTS+3) 
	#define TD2P_US1G_TOKEN (TD2P_NUM_EXT_PORTS+4) 
	#define TD2P_ANCL_TOKEN (TD2P_NUM_EXT_PORTS+5)
    #define TD2P_OVSB_TOKEN (TD2P_NUM_EXT_PORTS+7)
    #define TD2P_IDL1_TOKEN (TD2P_NUM_EXT_PORTS+8)
    #define TD2P_IDL2_TOKEN (TD2P_NUM_EXT_PORTS)
#else
	#define TD2P_MGMT_TOKEN (TD2P_NUM_EXT_PORTS+3) 
	#define TD2P_US1G_TOKEN (TD2P_NUM_EXT_PORTS+4) 
	#define TD2P_ANCL_TOKEN (TD2P_NUM_EXT_PORTS+5)
    #define TD2P_OVSB_TOKEN 250
    #define TD2P_IDL1_TOKEN 251
    #define TD2P_IDL2_TOKEN 252
#endif

#define MMU_OVRSUB_PORT 57
#define MMU_NULL_PORT   58

/* Static port identities */
#define TD2P_MGMT_PORT_0 13
#define TD2P_MGMT_PORT_1 14
#define TD2P_MGMT_PORT_2 15
#define TD2P_MGMT_PORT_3 16
#define TD2P_LOOPBACK_PORT 129
#define TD2P_CPU_PORT 0

/* Macro definitions */
#define TD2P_PIPE_X_ID 0
#define TD2P_PIPE_Y_ID 1

#define TD2P_XPIPE_CAL_ID 4
#define TD2P_YPIPE_CAL_ID 5


#endif /* TDM_TD2P_PREPROCESSOR_MACROS_H */
