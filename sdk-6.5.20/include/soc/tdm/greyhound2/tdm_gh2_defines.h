/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM macro values for BCM53570
 */

#ifndef TDM_GH2_PREPROCESSOR_MACROS_H
#define TDM_GH2_PREPROCESSOR_MACROS_H

/* Frequency */
#define GH2_CLK_950MHZ 950
#define GH2_CLK_850MHZ 850
#define GH2_CLK_765MHZ 765
#define GH2_CLK_672MHZ 672
#define GH2_CLK_585MHZ 585
#define GH2_CLK_545MHZ 545

/* Calendar length */
#define GH2_LEN_950MHZ_HG 243
#define GH2_LEN_950MHZ_EN 250
#define GH2_LEN_850MHZ_HG 215
#define GH2_LEN_850MHZ_EN 224
#define GH2_LEN_765MHZ_HG 194
#define GH2_LEN_765MHZ_EN 202
#define GH2_LEN_672MHZ_HG 170
#define GH2_LEN_672MHZ_EN 177
#define GH2_LEN_585MHZ_HG 163
#define GH2_LEN_585MHZ_EN 381
#define GH2_LEN_545MHZ_HG 138
#define GH2_LEN_545MHZ_EN 143

/* Default number of PM in chip */
#define GH2_NUM_PM_MOD 33
#define GH2_NUM_PHY_PM 32
/* Default number of lanes per PM in chip */
#define GH2_NUM_PM_LNS 4
/* Allocation length of port lookup tables */
#define GH2_NUM_EXT_PORTS 136

/* Number of possible physical ports */
#define GH2_NUM_PHY_PORTS 128
/* Number of scheduling blocks */
#define GH2_NUM_QUAD 2

/* Default number of ancillary ports in calendar */
#define GH2_ACC_PORT_NUM 1

/* Allocation length of VBS line rate calendar */
#define GH2_LR_VBS_LEN 512
/* Allocation area of VBS port groups */
#define GH2_OS_VBS_GRP_NUM 8
#define GH2_OS_VBS_GRP_LEN 12

/* Architecturally specific allocation area of VBS vector map */
#define GH2_VMAP_MAX_LEN GH2_LR_VBS_LEN
#define GH2_VMAP_MAX_WID 64

/* Min sister port spacing (VBS scheduler req) */
#define GH2_MIN_SPACING_SISTER_PORT 4
/* Min same port spacing (VBS scheduler req) */
#define GH2_MIN_SPACING_SAME_PORT 5

/* Tokenization values 
		GH2_OVSB_TOKEN: token for oversub round robin
		GH2_IDL1_TOKEN: idle slot for memreset, L2 management, other
		GH2_IDL2_TOKEN: idle slot guaranteed for refresh
		GH2_NULL_TOKEN: null slot, no pick, no opportunistic
		GH2_ANCL_TOKEN: reservation for ancillary soc functions
*/
#ifdef _TDM_STANDALONE
	#define GH2_OVSB_TOKEN 137 
	#define GH2_IDL1_TOKEN 138 
	#define GH2_IDL2_TOKEN 139 
	#define GH2_NULL_TOKEN 140 
	#define GH2_ANCL_TOKEN (GH2_NUM_EXT_PORTS+9) 
#else
	#define GH2_OVSB_TOKEN 250 
	#define GH2_IDL1_TOKEN 251 
	#define GH2_IDL2_TOKEN 252 
	#define GH2_NULL_TOKEN 253 
	#define GH2_ANCL_TOKEN (GH2_NUM_EXT_PORTS+9) 
#endif

/* Static port identities */
#define GH2_CMIC_TOKEN 0 /* Pipe 0 - CPU slot */
#define GH2_LPB0_TOKEN (GH2_NUM_EXT_PORTS-4) /* Pipe 0 - loopback slot */
#define GH2_LPB1_TOKEN (GH2_NUM_EXT_PORTS-3) /* Pipe 1 - loopback slot */
#define GH2_MGM1_TOKEN (GH2_NUM_EXT_PORTS-7) /* Pipe 1 - management slot */
#define GH2_MGM2_TOKEN (GH2_NUM_EXT_PORTS-5) /* Pipe 2 - management slot */
#define GH2_LPB2_TOKEN (GH2_NUM_EXT_PORTS-2) /* Pipe 2 - loopback slot */
#define GH2_RSVD_TOKEN (GH2_NUM_EXT_PORTS-6) /* Reserved slot */
#define GH2_LPB3_TOKEN (GH2_NUM_EXT_PORTS-1) /* Pipe 3 - loopback slot */

#define GH2_IDB_PIPE_0_CAL_ID 0
#define GH2_IDB_PIPE_1_CAL_ID 1
#define GH2_IDB_PIPE_2_CAL_ID 2
#define GH2_IDB_PIPE_3_CAL_ID 3
#define GH2_MMU_PIPE_0_CAL_ID 4
#define GH2_MMU_PIPE_1_CAL_ID 5
#define GH2_MMU_PIPE_2_CAL_ID 6
#define GH2_MMU_PIPE_3_CAL_ID 7

/* Linerate jitter threshold */
#define GH2_LR_JITTER_LO 5
#define GH2_LR_JITTER_MD 2
#define GH2_LR_JITTER_HI 1
#define GH2_LR_CHK_TRHD  GH2_LR_JITTER_HI 

#endif /* TDM_GH2_PREPROCESSOR_MACROS_H */
