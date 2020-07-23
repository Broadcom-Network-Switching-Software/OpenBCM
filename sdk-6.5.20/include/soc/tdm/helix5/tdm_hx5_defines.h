/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM macro values for BCM56870
 */

#ifndef TDM_HX5_PREPROCESSOR_MACROS_H
#define TDM_HX5_PREPROCESSOR_MACROS_H



/* Frequency */

#define HX5_CLK_450MHZ 450 
#define HX5_CLK_400MHZ 400 
#define HX5_CLK_543MHZ 543
#define HX5_CLK_587MHZ 587
#define HX5_CLK_668MHZ 668
#define HX5_CLK_812MHZ 812
#define HX5_CLK_893MHZ 893
#define HX5_CLK_931MHZ 931

/* Calendar Length */

#define HX5_LEN_400MHZ 214
#define HX5_LEN_400HMHZ 202
#define HX5_LEN_450MHZ 240
#define HX5_LEN_450HMHZ 228
#define HX5_LEN_543MHZ 292
#define HX5_LEN_587MHZ 314
#define HX5_LEN_587HMHZ 297
#define HX5_LEN_668MHZ 358
#define HX5_LEN_812MHZ 436
#define HX5_LEN_893MHZ 480
#define HX5_LEN_931MHZ 472



/* Default number of PM in chip */
#define HX5_NUM_PM_MOD 19
#define HX5_NUM_PHY_PM 19
#define HX5_NUM_PHY_GPORTS 48
/* Default number of lanes per PM in chip */
#define HX5_NUM_PM_LNS 4
/* Allocation length of port lookup tables */
#define HX5_NUM_EXT_PORTS 79

/* Number of possible physical ports */
#define HX5_NUM_PHY_PORTS 76
/* Number of scheduling blocks */
#define HX5_NUM_QUAD 2
/* Number of scheduling blocks */
#define HX5_NUM_PIPE 1
/* Number of possible physical ports per pipe */
#define HX5_NUM_PHY_PORTS_PER_PIPE 76
/* Number of possible physical ports per halfpipe */
#define HX5_NUM_PHY_PORTS_PER_HPIPE 38

/* Number of slots per lane in OVSB Packet Scheduler */
#define HX5_NUM_PKT_SLOTS_PER_PM 16

/* Default number of ancillary ports in calendar */
#define HX5_NUM_ANCL 24
#define HX5_NUM_ANCL_CPU 4
#define HX5_NUM_ANCL_FAE 4
#define HX5_NUM_ANCL_LPBK 8
#define HX5_NUM_ANCL_IDB_OPT1 2
#define HX5_NUM_ANCL_IDB_NULL 2
#define HX5_NUM_ANCL_IDB_IDLE 4

#define HX5_NUM_ANCL_MMU_OPT1 0
#define HX5_NUM_ANCL_MMU_NULL 6
#define HX5_NUM_ANCL_MMU_IDLE 4

/* Allocation length of VBS line rate calendar */
#define HX5_LR_VBS_LEN 512
/* Allocation area of VBS port groups */
#define HX5_OS_VBS_GRP_NUM 12
#define HX5_OS_VBS_GRP_LEN 12

/* Architecturally specific allocation area of VBS vector map */
#define HX5_VMAP_MAX_LEN HX5_LR_VBS_LEN
/* num of physical ports for vmap scheduler */ 
#define HX5_VMAP_MAX_WID 76

/* Min sister port spacing (VBS scheduler req) 4-2 */
#define HX5_MIN_SPACING_SISTER_PORT 2
#define HX5_MIN_SPACING_SISTER_FALCON_PORT 2
#define HX5_MIN_SPACING_SISTER_EAGLE_PORT 4
/* Min sister port spacing (VBS scheduler req) 8 - 6 , 14 - 6*/
#define HX5_MIN_SPACING_SAME_PORT_HSP 6
#define HX5_MIN_SPACING_SAME_PORT_LSP 6

/* static port identities 
        HX5_CMIC_TOKEN (  0): Pipe 0 - CPU slot
        HX5_MGM1_TOKEN (129): Pipe 1 - management port 1 slot
        HX5_MGM2_TOKEN (128): Pipe 1 - management port 2 slot
        HX5_LPB0_TOKEN (130): Pipe 0 - loopback slot
        HX5_LPB1_TOKEN (131): Pipe 1 - loopback slot
*/
#define HX5_CMIC_TOKEN 0
#define HX5_FAE_TOKEN 78 
/* #define HX5_MGM2_TOKEN (HX5_NUM_EXT_PORTS+3) */
/*#define HX5_LPB0_TOKEN (HX5_NUM_EXT_PORTS+1)*/
#define HX5_LPB0_TOKEN 77
/*#define HX5_LPB1_TOKEN (HX5_NUM_EXT_PORTS+1)*/
/* Tokenization values 
        HX5_OVSB_TOKEN (133): token for oversub round robin
        HX5_IDL1_TOKEN (134): idle slot for memreset, L2 management, other
        HX5_IDL2_TOKEN (135): idle slot guaranteed for refresh
        HX5_NULL_TOKEN (136): null slot, no pick, no opportunistic
        HX5_ANCL_TOKEN (142): reservation for ancillary soc functions
*/
#define HX5_OVSB_TOKEN (HX5_NUM_EXT_PORTS+4)
#define HX5_IDL1_TOKEN (HX5_NUM_EXT_PORTS+5)
#define HX5_IDL2_TOKEN (HX5_NUM_EXT_PORTS+6)
#define HX5_NULL_TOKEN (HX5_NUM_EXT_PORTS+7)
#define HX5_ANCL_TOKEN (HX5_NUM_EXT_PORTS+10)

/* Calendar ID */
#define HX5_IDB_PIPE_0_CAL_ID 0
#define HX5_IDB_PIPE_1_CAL_ID 1
#define HX5_IDB_PIPE_2_CAL_ID 2
#define HX5_IDB_PIPE_3_CAL_ID 3
#define HX5_MMU_PIPE_0_CAL_ID 4
#define HX5_MMU_PIPE_1_CAL_ID 5
#define HX5_MMU_PIPE_2_CAL_ID 6
#define HX5_MMU_PIPE_3_CAL_ID 7

/* Physical port number range of each pipe */
#define HX5_CAL_0_PORT_LO 1
#define HX5_CAL_0_PORT_HI 76
#define HX5_CAL_1_PORT_LO 0
#define HX5_CAL_1_PORT_HI 0
#define HX5_CAL_2_PORT_LO 0
#define HX5_CAL_2_PORT_HI 0
#define HX5_CAL_3_PORT_LO 0
#define HX5_CAL_3_PORT_HI 0
#define HX5_CAL_4_PORT_LO 1
#define HX5_CAL_4_PORT_HI 76
#define HX5_CAL_5_PORT_LO 0
#define HX5_CAL_5_PORT_HI 0
#define HX5_CAL_6_PORT_LO 0
#define HX5_CAL_6_PORT_HI 0
#define HX5_CAL_7_PORT_LO 0
#define HX5_CAL_7_PORT_HI 0

/* Management port mode (10G per management port) 
       0 -> two Etherner management ports (default)
       1 -> one Ethernet management port
       2 -> two HiGig management ports (reserved)
       3 -> one HiGig management port (reserved)
*/
enum hx5_mgmt_mode_e {
    HX5_MGMT_MODE_2_PORT_EN=0,
    HX5_MGMT_MODE_1_PORT_EN=1,
    HX5_MGMT_MODE_2_PORT_HG=2,
    HX5_MGMT_MODE_1_PORT_HG=3
};

/* 1G port state
       0 -> None 1G port
       1 -> Regular 1G port
       2 -> PM4x10Q 1G port (16x1G super PM subport)
*/
enum hx5_1g_port_state_e {
    HX5_1G_PORT_STATE_NONE=0,
    HX5_1G_PORT_STATE_REGULAR=1,
    HX5_1G_PORT_STATE_16x1G=2,
    HX5_1G_PORT_STATE_16x2G=3
};
/* Index and length of shaping calendar within each inst */
#define HX5_SHAPING_GRP_NUM 2
#define HX5_SHAPING_GRP_LEN 160
#define HX5_SHAPING_GRP_IDX_0 HX5_OS_VBS_GRP_NUM
#define HX5_SHAPING_GRP_IDX_1 (HX5_OS_VBS_GRP_NUM+1)

/* Linerate jitter threshold */
#define HX5_LR_JITTER_LO 5
#define HX5_LR_JITTER_MD 2
#define HX5_LR_JITTER_HI 1
#define HX5_LR_CHK_TRHD  0 

#endif /* TDM_HX5_PREPROCESSOR_MACROS_H */
