/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM macro values for BCM56960
 */

#ifndef TDM_TH_PREPROCESSOR_MACROS_H
#define TDM_TH_PREPROCESSOR_MACROS_H

/* Frequency */
#define TH_CLK_950MHZ 950
#define TH_CLK_850MHZ 850
#define TH_CLK_765MHZ 765
#define TH_CLK_672MHZ 672
#define TH_CLK_645MHZ 645
#define TH_CLK_545MHZ 545

/* Calendar length */
#define TH_LEN_950MHZ_HG 243
#define TH_LEN_950MHZ_EN 250
#define TH_LEN_850MHZ_HG 215
#define TH_LEN_850MHZ_EN 224
#define TH_LEN_765MHZ_HG 194
#define TH_LEN_765MHZ_EN 202
#define TH_LEN_672MHZ_HG 170
#define TH_LEN_672MHZ_EN 177
#define TH_LEN_645MHZ_HG 163
#define TH_LEN_645MHZ_EN 170
#define TH_LEN_545MHZ_HG 138
#define TH_LEN_545MHZ_EN 143

/* Default number of PM in chip */
#define TH_NUM_PM_MOD 33
#define TH_NUM_PHY_PM 32
/* Default number of lanes per PM in chip */
#define TH_NUM_PM_LNS 4
/* Allocation length of port lookup tables */
#define TH_NUM_EXT_PORTS 136

/* Number of possible physical ports */
#define TH_NUM_PHY_PORTS 128
/* Number of scheduling blocks */
#define TH_NUM_QUAD 2

/* Default number of ancillary ports in calendar */
#define TH_ACC_PORT_NUM 10

/* Allocation length of VBS line rate calendar */
#define TH_LR_VBS_LEN 256
/* Allocation area of VBS port groups */
#define TH_OS_VBS_GRP_NUM 8
#define TH_OS_VBS_GRP_LEN 12

/* Architecturally specific allocation area of VBS vector map */
#define TH_VMAP_MAX_LEN TH_LR_VBS_LEN
#define TH_VMAP_MAX_WID 64

/* Min sister port spacing (VBS scheduler req) */
#define TH_MIN_SPACING_SISTER_PORT 4
/* Min same port spacing (VBS scheduler req) */
#define TH_MIN_SPACING_SAME_PORT 4

/* Tokenization values 
		TH_OVSB_TOKEN: token for oversub round robin
		TH_IDL1_TOKEN: idle slot for memreset, L2 management, other
		TH_IDL2_TOKEN: idle slot guaranteed for refresh
		TH_NULL_TOKEN: null slot, no pick, no opportunistic
		TH_ANCL_TOKEN: reservation for ancillary soc functions
*/
#ifdef _TDM_STANDALONE
	#define TH_OVSB_TOKEN 137 
	#define TH_IDL1_TOKEN 138 
	#define TH_IDL2_TOKEN 139 
	#define TH_NULL_TOKEN 140 
	#define TH_ANCL_TOKEN (TH_NUM_EXT_PORTS+9) 
#else
	#define TH_OVSB_TOKEN 250 
	#define TH_IDL1_TOKEN 251 
	#define TH_IDL2_TOKEN 252 
	#define TH_NULL_TOKEN 253 
	#define TH_ANCL_TOKEN (TH_NUM_EXT_PORTS+9) 
#endif

/* Static port identities */
#define TH_CMIC_TOKEN 0 /* Pipe 0 - CPU slot */
#define TH_LPB0_TOKEN (TH_NUM_EXT_PORTS-4) /* Pipe 0 - loopback slot */
#define TH_LPB1_TOKEN (TH_NUM_EXT_PORTS-3) /* Pipe 1 - loopback slot */
#define TH_MGM1_TOKEN (TH_NUM_EXT_PORTS-7) /* Pipe 1 - management slot */
#define TH_MGM2_TOKEN (TH_NUM_EXT_PORTS-5) /* Pipe 2 - management slot */
#define TH_LPB2_TOKEN (TH_NUM_EXT_PORTS-2) /* Pipe 2 - loopback slot */
#define TH_RSVD_TOKEN (TH_NUM_EXT_PORTS-6) /* Reserved slot */
#define TH_LPB3_TOKEN (TH_NUM_EXT_PORTS-1) /* Pipe 3 - loopback slot */

#define TH_IDB_PIPE_0_CAL_ID 0
#define TH_IDB_PIPE_1_CAL_ID 1
#define TH_IDB_PIPE_2_CAL_ID 2
#define TH_IDB_PIPE_3_CAL_ID 3
#define TH_MMU_PIPE_0_CAL_ID 4
#define TH_MMU_PIPE_1_CAL_ID 5
#define TH_MMU_PIPE_2_CAL_ID 6
#define TH_MMU_PIPE_3_CAL_ID 7

/* Linerate jitter threshold */
#define TH_LR_JITTER_LO 5
#define TH_LR_JITTER_MD 2
#define TH_LR_JITTER_HI 1
#define TH_LR_CHK_TRHD  TH_LR_JITTER_HI 

#endif /* TDM_TH_PREPROCESSOR_MACROS_H */
