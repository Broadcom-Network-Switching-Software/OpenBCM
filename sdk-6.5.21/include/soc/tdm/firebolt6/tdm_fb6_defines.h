/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM macro values for BCM56870
 */

#ifndef TDM_FB6_PREPROCESSOR_MACROS_H
#define TDM_FB6_PREPROCESSOR_MACROS_H


/* Frequency */

#define FB6_CLK_4730_1012 1013
#define FB6_CLK_4731_1012 1011
#define FB6_CLK_4732_1012 1016
#define FB6_CLK_4720_1250 1257
#define FB6_CLK_4721_1250 1258
#define FB6_CLK_4700_1250 1250
#define FB6_CLK_4701_1250 1251
#define FB6_CLK_4702_1250 1252
#define FB6_CLK_4703_1250 1253
#define FB6_CLK_4704_1087 1256
#define FB6_CLK_4705_1250 1254
#define FB6_CLK_1000_1012 1014
#define FB6_CLK_1001_1250 1255

/* Calendar Length */

#define FB6_LEN_4730_1012 272
#define FB6_LEN_4731_1012 272
#define FB6_LEN_4732_1012 272
#define FB6_LEN_4720_1250 336
#define FB6_LEN_4721_1250 336
#define FB6_LEN_4700_1250 336
#define FB6_LEN_4701_1250 336
#define FB6_LEN_4702_1250 336
#define FB6_LEN_4703_1250 336
#define FB6_LEN_4704_1087 336
#define FB6_LEN_4705_1250 236
#define FB6_LEN_1000_1012 272
#define FB6_LEN_1001_1250 336



/* Default number of PM in chip */
#define FB6_NUM_PMS_PER_HPIPE 8
#define FB6_NUM_HPIPE 2
#define FB6_NUM_PM_MOD 17
#define FB6_NUM_PHY_PM 17
#define FB6_NUM_PHY_GPORTS 0
/* Default number of lanes per PM in chip */
#define FB6_NUM_PM_LNS 4
/* Allocation length of port lookup tables */
#define FB6_NUM_EXT_PORTS 73

/* Number of possible physical ports */
#define FB6_NUM_PHY_PORTS 64
/* Number of scheduling blocks */
#define FB6_NUM_QUAD 1
/* Number of scheduling blocks */
#define FB6_NUM_PIPE 1
/* Number of possible physical ports per pipe */
#define FB6_NUM_PHY_PORTS_PER_PIPE 64
/* Number of possible physical ports per halfpipe */
#define FB6_NUM_PHY_PORTS_PER_HPIPE 32

/* Number of slots per lane in OVSB Packet Scheduler */
#define FB6_NUM_PKT_SLOTS_PER_PM 16

/* Default number of ancillary ports in calendar */
#define FB6_NUM_ANCL 47
#define FB6_NUM_ANCL_4730_1012 34
#define FB6_NUM_ANCL_4731_1012 17
#define FB6_NUM_ANCL_4732_1012 17
#define FB6_NUM_ANCL_4720_1250 25
#define FB6_NUM_ANCL_4721_1250 33
#define FB6_NUM_ANCL_4700_1250 33
#define FB6_NUM_ANCL_4701_1250 29
#define FB6_NUM_ANCL_4702_1250 26
#define FB6_NUM_ANCL_4703_1250 21
#define FB6_NUM_ANCL_4704_1087 33
#define FB6_NUM_ANCL_4705_1250 33
#define FB6_NUM_ANCL_1000_1012 40
#define FB6_NUM_ANCL_1001_1250 34
#define FB6_NUM_ANC_MGMT 28
#define FB6_NUM_ANCL_CPU 2
#define FB6_NUM_ANCL_FAE 4
#define FB6_NUM_ANCL_LPBK 4
#define FB6_NUM_ANCL_IDB_OPT1 1
#define FB6_NUM_ANCL_IDB_NULL 8
#define FB6_NUM_ANCL_IDB_IDLE 0

#define FB6_NUM_ANCL_MMU_OPT1 0
#define FB6_NUM_ANCL_MMU_NULL 9
#define FB6_NUM_ANCL_MMU_IDLE 4

/* Allocation length of VBS line rate calendar */
#define FB6_LR_VBS_LEN 512
/* Allocation area of VBS port groups */
#define FB6_OS_VBS_GRP_NUM 12
#define FB6_OS_VBS_GRP_LEN 12

/* Architecturally specific allocation area of VBS vector map */
#define FB6_VMAP_MAX_LEN FB6_LR_VBS_LEN
/* num of physical ports for vmap scheduler */
#define FB6_VMAP_MAX_WID 76

/* Min sister port spacing (VBS scheduler req) 4-1 */
#define FB6_MIN_SPACING_SISTER_PORT 4
#define FB6_MIN_SPACING_SISTER_FALCON_PORT 4
#define FB6_MIN_SPACING_SISTER_EAGLE_PORT 4
/* Min sister port spacing (VBS scheduler req) 8 - 6 , 14 - 6*/
#define FB6_MIN_SPACING_SAME_PORT_HSP 6
#define FB6_MIN_SPACING_SAME_PORT_LSP 6

/* static port identities
        FB6_CMIC_TOKEN (  0): Pipe 0 - CPU slot
        FB6_MGM1_TOKEN (129): Pipe 1 - management port 1 slot
        FB6_MGM2_TOKEN (128): Pipe 1 - management port 2 slot
        FB6_LPB0_TOKEN (130): Pipe 0 - loopback slot
        FB6_LPB1_TOKEN (131): Pipe 1 - loopback slot
*/
#define FB6_CMIC_TOKEN 0
#define FB6_MGM1_TOKEN 1
#define FB6_MGM2_TOKEN 2
#define FB6_MGM3_TOKEN 3
#define FB6_MGM4_TOKEN 4
#define FB6_FAE_TOKEN 70
/* #define FB6_MGM2_TOKEN (FB6_NUM_EXT_PORTS+3) */
/*#define FB6_LPB0_TOKEN (FB6_NUM_EXT_PORTS+1)*/
#define FB6_LPB0_TOKEN 69
/*#define FB6_LPB1_TOKEN (FB6_NUM_EXT_PORTS+1)*/
/* Tokenization values
        FB6_OVSB_TOKEN (133): token for oversub round robin
        FB6_IDL1_TOKEN (134): idle slot for memreset, L2 management, other
        FB6_IDL2_TOKEN (135): idle slot guaranteed for refresh
        FB6_NULL_TOKEN (136): null slot, no pick, no opportunistic
        FB6_ANCL_TOKEN (142): reservation for ancillary soc functions
*/
#define FB6_OVSB_TOKEN 72   /*This needs to be taken from Port Map in Freq Analysis sheet*/
#define FB6_IDL1_TOKEN (FB6_NUM_EXT_PORTS+5)
#define FB6_IDL2_TOKEN (FB6_NUM_EXT_PORTS+6)
#define FB6_NULL_TOKEN (FB6_NUM_EXT_PORTS+7)
#define FB6_ANCL_TOKEN (FB6_NUM_EXT_PORTS+10)
#define FB6_DUMMY_TOKEN 127

/* Calendar ID */
#define FB6_IDB_PIPE_0_CAL_ID 0
#define FB6_IDB_PIPE_1_CAL_ID 1
#define FB6_IDB_PIPE_2_CAL_ID 2
#define FB6_IDB_PIPE_3_CAL_ID 3
#define FB6_MMU_PIPE_0_CAL_ID 4
#define FB6_MMU_PIPE_1_CAL_ID 5
#define FB6_MMU_PIPE_2_CAL_ID 6
#define FB6_MMU_PIPE_3_CAL_ID 7

/* Physical port number range of each pipe */
#define FB6_CAL_0_PORT_LO 1
#define FB6_CAL_0_PORT_HI 68
#define FB6_CAL_1_PORT_LO 0
#define FB6_CAL_1_PORT_HI 0
#define FB6_CAL_2_PORT_LO 0
#define FB6_CAL_2_PORT_HI 0
#define FB6_CAL_3_PORT_LO 0
#define FB6_CAL_3_PORT_HI 0
#define FB6_CAL_4_PORT_LO 1
#define FB6_CAL_4_PORT_HI 68
#define FB6_CAL_5_PORT_LO 0
#define FB6_CAL_5_PORT_HI 0
#define FB6_CAL_6_PORT_LO 0
#define FB6_CAL_6_PORT_HI 0
#define FB6_CAL_7_PORT_LO 0
#define FB6_CAL_7_PORT_HI 0

/* Management port mode (10G per management port)
       0 -> two Etherner management ports (default)
       1 -> one Ethernet management port
       2 -> two HiGig management ports (reserved)
       3 -> one HiGig management port (reserved)
*/
enum fb6_mgmt_mode_e {
    FB6_MGMT_MODE_2_PORT_EN=0,
    FB6_MGMT_MODE_1_PORT_EN=1,
    FB6_MGMT_MODE_2_PORT_HG=2,
    FB6_MGMT_MODE_1_PORT_HG=3
};

/* 1G port state
       0 -> None 1G port
       1 -> Regular 1G port
       2 -> PM4x10Q 1G port (16x1G super PM subport)
*/
enum fb6_1g_port_state_e {
    FB6_1G_PORT_STATE_NONE=0,
    FB6_1G_PORT_STATE_REGULAR=1,
    FB6_1G_PORT_STATE_16x1G=2,
    FB6_1G_PORT_STATE_16x2G=3
};
/* Index and length of shaping calendar within each inst */
#define FB6_SHAPING_GRP_NUM 2
#define FB6_SHAPING_GRP_LEN 160
#define FB6_SHAPING_GRP_IDX_0 FB6_OS_VBS_GRP_NUM
#define FB6_SHAPING_GRP_IDX_1 (FB6_OS_VBS_GRP_NUM+1)
/* number of slots per sublane: 5*/
#define FB6_SHAPER_NUM_LN_SLOTS 5
/* number of slots per PM: 20*/
#define FB6_SHAPER_NUM_PM_SLOTS (FB6_SHAPER_NUM_LN_SLOTS*FB6_NUM_PM_LNS)
/* number of slots packet shaper calendar: 160 */
#define FB6_SHAPER_LEN (FB6_SHAPER_NUM_PM_SLOTS*FB6_NUM_PM_MOD)

#define FB6_SHAPER_SLOT_UNIT (SPEED_5G)

/* Linerate jitter threshold */
#define FB6_LR_JITTER_LO 5
#define FB6_LR_JITTER_MD 2
#define FB6_LR_JITTER_HI 1
#define FB6_LR_CHK_TRHD  0

#endif /* TDM_FB6_PREPROCESSOR_MACROS_H */
