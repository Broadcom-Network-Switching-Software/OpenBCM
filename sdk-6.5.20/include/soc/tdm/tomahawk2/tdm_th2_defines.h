/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM macro values for BCM56970
 */

#ifndef TDM_TH2_PREPROCESSOR_MACROS_H
#define TDM_TH2_PREPROCESSOR_MACROS_H

/* Default number of PM in chip */
#define TH2_NUM_PM_MOD 65
#define TH2_NUM_PHY_PM 64
/* Default number of lanes per PM in chip */
#define TH2_NUM_PM_LNS 4
/* Allocation length of port lookup tables */
#define TH2_NUM_EXT_PORTS 264

/* Number of possible physical ports */
#define TH2_NUM_PHY_PORTS 256
/* Number of scheduling blocks */
#define TH2_NUM_QUAD 4

/* Default number of ancillary ports in calendar */
#define TH2_ACC_PORT_NUM 10

/* Allocation length of VBS line rate calendar */
#define TH2_LR_VBS_LEN 512
/* Allocation area of VBS port groups */
#define TH2_OS_VBS_GRP_NUM 12
#define TH2_OS_VBS_GRP_LEN 12

/* Architecturally specific allocation area of VBS vector map */
#define TH2_VMAP_MAX_LEN TH2_LR_VBS_LEN
#define TH2_VMAP_MAX_WID 49

/* Min sister port spacing (VBS scheduler req) */
#define TH2_VBS_MIN_SPACING 6

/* Tokenization values */
#ifdef _TDM_STANDALONE
	#define TH2_OVSB_TOKEN (TH2_NUM_EXT_PORTS+1) /* token for oversub round robin */
	#define TH2_IDL1_TOKEN (TH2_NUM_EXT_PORTS+2) /* idle slot for memreset, L2 management, other */
	#define TH2_IDL2_TOKEN (TH2_NUM_EXT_PORTS+3) /* idle slot guaranteed for refresh */
	#define TH2_NULL_TOKEN (TH2_NUM_EXT_PORTS+4) /* null slot, no pick, no opportunistic */
	#define TH2_ANCL_TOKEN (TH2_NUM_EXT_PORTS+9) /* reservation for ancillary soc functions */
#else
	#define TH2_OVSB_TOKEN (TH2_NUM_EXT_PORTS+1) /* token for oversub round robin */
	#define TH2_IDL1_TOKEN (TH2_NUM_EXT_PORTS+2) /* idle slot for memreset, L2 management, other */
	#define TH2_IDL2_TOKEN (TH2_NUM_EXT_PORTS+3) /* idle slot guaranteed for refresh */
	#define TH2_NULL_TOKEN (TH2_NUM_EXT_PORTS+4) /* null slot, no pick, no opportunistic */
	#define TH2_ANCL_TOKEN (TH2_NUM_EXT_PORTS+9) /* reservation for ancillary soc functions */
#endif

/* Static port identities */
#define TH2_CMIC_TOKEN 0 /* Pipe 0 - CPU slot */
#define TH2_LPB0_TOKEN (TH2_NUM_EXT_PORTS-4) /* Pipe 0 - loopback slot */
#define TH2_LPB1_TOKEN (TH2_NUM_EXT_PORTS-3) /* Pipe 1 - loopback slot */
#ifdef _TDM_STANDALONE
	#define TH2_MGM1_TOKEN (TH2_NUM_EXT_PORTS-7) /* Pipe 1 - management slot */
	#define TH2_MGM2_TOKEN (TH2_NUM_EXT_PORTS-5) /* Pipe 2 - management slot */
#else
	#define TH2_MGM1_TOKEN (TH2_NUM_EXT_PORTS-7) /* Pipe 1 - management slot */
	#define TH2_MGM2_TOKEN (TH2_NUM_EXT_PORTS-5) /* Pipe 2 - management slot */
#endif
#define TH2_LPB2_TOKEN (TH2_NUM_EXT_PORTS-2) /* Pipe 2 - loopback slot */
#define TH2_RSVD_TOKEN (TH2_NUM_EXT_PORTS-6) /* Reserved slot */
#define TH2_LPB3_TOKEN (TH2_NUM_EXT_PORTS-1) /* Pipe 3 - loopback slot */


#define TH2_CPU_SLOTS 4
#define TH2_MGM_SLOTS 4
#define TH2_LBK_SLOTS 2
#define TH2_ANC_SLOTS 4

/* Index and length of shaping calendar within each inst */
#define SHAPING_GRP_IDX_0 TH2_OS_VBS_GRP_NUM
#define SHAPING_GRP_IDX_1 (TH2_OS_VBS_GRP_NUM+1)
#define SHAPING_GRP_LEN 160


#define TH2_LR_JITTER_PCT 20


#endif /* TDM_TH2_PREPROCESSOR_MACROS_H */
