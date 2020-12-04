/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM macro values for BCM56860
 */

#ifndef TDM_MN_PREPROCESSOR_MACROS_H
#define TDM_MN_PREPROCESSOR_MACROS_H

/* Default number of PM in chip */
#define MN_NUM_PM_MOD 17
#define MN_NUM_PHY_PM 16
/* Default number of lanes per PM in chip */
#define MN_NUM_PM_LNS 4
/* Allocation length of port lookup tables */
#define MN_NUM_EXT_PORTS 130
/* Number of the hardware designated management PM */
#define MN_MGMT_TSC_ID 3

#define MN_CL1_TSC_ID 7
#define MN_CL2_TSC_ID 8
#define MN_CL4_TSC_ID 16
#define MN_CL5_TSC_ID 17
#define MN_CL1_PORT   1
#define MN_CL2_PORT   5
#define MN_CL3_PORT   5
#define MN_CL4_PORT   13
#define MN_CL5_PORT   17
#define MN_CL6_PORT   45
#define MN_CL7_PORT   49
#define MN_CL8_PORT   53
#define MN_CL9_PORT   57
#define MN_CL10_PORT  61
#define MN_PGW_PER_SLOT_IO_BW  2.5
#define MN_PGW_NUM_SLOTS_100G  20
#define MN_PGW_NUM_SLOTS_50G   10
#define MN_PGW_NUM_SLOTS_40G   8
#define MN_PGW_NUM_SLOTS_25G   5
#define MN_PGW_NUM_SLOTS_20G    4
#define MN_PGW_NUM_SLOTS_12p5G  2
#define MN_PGW_NUM_SLOTS_10G    2
#define MN_PGW_NUM_SLOTS_5G     1
#define MN_PGW_NUM_SLOTS_2p5G   1
#define MN_PGW_JITTER_PERCENT_LOWER  0.3
#define MN_PGW_JITTER_PERCENT_UPPER  0.3
#define MN_MMU_JITTER_PERCENT_LOWER  0.3
#define MN_MMU_JITTER_PERCENT_UPPER  0.3
#define MMU_AUX_MIN_SPACING 8
#define MN_PGW_OS_TDM_DEPTH   48
#define MN_PGW_JITTER_PERCENT_LOWER  0.3
#define MN_PGW_JITTER_PERCENT_UPPER  0.3
#define MN_MMU_JITTER_PERCENT_LOWER  0.3
#define MN_MMU_JITTER_PERCENT_UPPER  0.3
#define MN_LEN_415MHZ_HG 109
#define MN_LEN_415MHZ_EN 109
#define MN_LEN_705MHZ_HG 186
#define MN_LEN_705MHZ_EN 186
#define MN_LEN_815MHZ_HG 321
#define MN_LEN_815MHZ_EN 321
#define MN_LEN_525MHZ_HG 138
#define MN_LEN_525MHZ_EN 138
#define MN_LEN_816MHZ_HG 297
#define MN_LEN_816MHZ_EN 297
#define MN_LEN_817MHZ_HG 302
#define MN_LEN_817MHZ_EN 302
#define MN_LEN_818MHZ_EN 334
#define MN_LEN_819MHZ_EN 283
#define MN_LEN_820MHZ_EN 335
#define MN_LEN_861MHZ_EN 227
#define MN_LEN_862MHZ_EN 227
#define MN_LEN_550MHZ_EN 217
#define MN_LEN_550MHZ_HG 217
#define MN_LEN_666MHZ_EN 160
#define MN_LEN_667MHZ_EN 160

/* Number of possible physical ports */
#define MN_NUM_PHY_PORTS 64
#define MN_EXT_PLUS_CPU_PORT  (MN_NUM_PHY_PORTS+1)
#define MN_EXT_PLUS_CPU_MACSEC_PORT  (MN_EXT_PLUS_CPU_PORT+1)
/* Number of scheduling blocks */
#define MN_NUM_QUAD 2

/* Default number of ancillary ports in calendar */
#define MN_ACC_PORT_NUM 14


#define MN_MIN_SPACING_SAME_PORT_HSP 4
#define MN_MIN_SPACING_SISTER_PORT   0

/* Allocation length of LLS line rate calendar */
#define MN_TDM_AUX_SIZE 320
#define MN_LR_LLS_LEN 160 /* Monterey PGW has LR TDM depth of 128 which was 64 in Apache */
#define MN_PM_PER_PGW   8  /* Monterey has 8 PM per PGW */
/* Allocation length of VBS line rate calendar */
#define MN_LR_VBS_LEN 512
/* Allocation length of IARB static calendar */
#define MN_LR_IARB_STATIC_LEN 512
/* Allocation area of LLS port groups */
#define MN_OS_LLS_GRP_NUM 2
#define MN_OS_LLS_GRP_LEN 48
/* Allocation area of VBS port groups */
#define MN_OS_VBS_GRP_NUM 8
#define MN_OS_VBS_GRP_LEN 16

/* Architecturally specific allocation area of VBS vector map */
#define MN_VMAP_MAX_LEN 512
#define MN_VMAP_MAX_WID  67

/* Tokenization values */
#ifdef _TDM_STANDALONE
	/* Token for pmap management port */
	#define MN_MGMT_TOKEN (MN_NUM_EXT_PORTS+3) 
	/* Token for granularity scaling */
	#define MN_US1G_TOKEN (MN_NUM_EXT_PORTS+4) 
	/* Token for ancillary soc functions */
	#define MN_ANCL_TOKEN (MN_NUM_PHY_PORTS+5)
    /* Token for oversub round robin */
    #define MN_OVSB_TOKEN (MN_EXT_PLUS_CPU_PORT+3)
    /* Memreset, L2 management, other */
    #define MN_IDL1_TOKEN 69
    /* EP refresh, idle cycle */
    #define MN_IDL2_TOKEN MN_IDL1_TOKEN
  /* EP aux cycle */
    #define MN_AUX_TOKEN 70
  /* Dedicated SBUS Op */
    #define MN_SBUS_TOKEN 74
#else
	/* Token for pmap management port */
	#define MN_MGMT_TOKEN (MN_NUM_EXT_PORTS+3) 
	/* Token for granularity scaling */
	#define MN_US1G_TOKEN (MN_NUM_EXT_PORTS+4) 
	/* Token for ancillary soc functions */
	#define MN_ANCL_TOKEN (MN_NUM_EXT_PORTS+5)
    /* Token for oversub round robin */
    #define MN_OVSB_TOKEN 250
    /* Memreset, L2 management, other */
    #define MN_IDL1_TOKEN 251
    /* EP refresh, idle cycle */
    #define MN_IDL2_TOKEN MN_IDL1_TOKEN
  /* EP aux cycle */
    #define MN_AUX_TOKEN 253
    #define MN_SBUS_TOKEN 254
#endif

#define MMU_OVRSUB_PORT 57
#define MMU_NULL_PORT   58

/* Static port identities */
/*#define MN_MGMT_PORT_0 103 */ /*nak,dummy MGMT PORT defines*/
/*#define MN_MGMT_PORT_1 104
#define MN_MGMT_PORT_2 105
#define MN_MGMT_PORT_3 106*/
#define MN_LOOPBACK_PORT 65
#define MN_MACSEC_PORT 66
#define MN_CPU_PORT 0

/* Macro definitions */
#define MN_PIPE_X_ID 0
#define MN_PIPE_Y_ID 1

#define MN_XPIPE_CAL_ID 2
#define MN_YPIPE_CAL_ID 5


#endif /* TDM_MN_PREPROCESSOR_MACROS_H */
