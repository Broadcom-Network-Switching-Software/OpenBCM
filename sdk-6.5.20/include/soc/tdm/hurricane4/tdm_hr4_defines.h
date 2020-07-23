/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM macro values for BCM56275
 */

#ifndef TDM_HR4_PREPROCESSOR_MACROS_H
#define TDM_HR4_PREPROCESSOR_MACROS_H



/* Frequency */

#define HR4_CLK_495MHZ 495
#define HR4_CLK_496MHZ 496
#define HR4_CLK_445MHZ 445
#define HR4_CLK_446MHZ 446
#define HR4_CLK_415MHZ 415
#define HR4_CLK_325MHZ 325
#define HR4_CLK_245MHZ 245
#define HR4_CLK_265MHZ 265
#define HR4_CLK_266MHZ 266
#define HR4_CLK_246MHZ 246
#define HR4_CLK_135MHZ 135
#define HR4_CLK_200MHZ 200
#define HR4_CLK_100MHZ 100
#define HR4_CLK_66MHZ   66

#define HR4_CLK_445MHZ 445
/* Calendar Length */
#define HR4_CAL_1_LEN   265 /*FAKE DV */
#define HR4_CAL_2_LEN   209 /* 2756, 2755, 2752, 2780, 2741, 2731 */
#define HR4_CAL_3_LEN   250 /* 2757, 2750, 2740 */
#define HR4_CAL_4_LEN    72 /* 2736, 2763 */
#define HR4_CAL_5_LEN   225 /* 2758 */
#define HR4_CAL_6_LEN   141 /* 2737 */
#define HR4_CAL_7_LEN   100 /* 2737 */
#define HR4_CAL_8_LEN    53 /* 2762 */
#define HR4_CAL_9_LEN   164 /* 3762 */
#define HR4_CAL_10_LEN  133 /* 2770 */

#define HR4_LEN_65MHZ   48
#define HR4_LEN_195MHZ  98
#define HR4_LEN_450MHZ 227
#define HR4_LEN_465MHZ 265
#define HR4_LEN_495MHZ 265
#define HR4_LEN_400MHZ 202
#define HR4_LEN_275MHZ 138
#define HR4_LEN_276MHZ 147
#define HR4_LEN_125MHZ  66
#define HR4_LEN_337MHZ 170


/* Default number of PM in chip */
#define HR4_NUM_PM_MOD 17
#define HR4_NUM_PHY_PM 17
#define HR4_NUM_PHY_GPORTS 64 
/* Default number of lanes per PM in chip */
#define HR4_NUM_PM_LNS 4
/* Allocation length of port lookup tables */
#define HR4_NUM_EXT_PORTS 71 /* 64 phy ports + cpu, lpbk, broadscan, 4 Macsec ports */


#define HR4_NUM_PMQ_SUBPORTS 16
/* GPORT related defines */
#define HR4_NUM_GPORT_SUBPORTS 8
#define HR4_GPORT0_START       1
#define HR4_GPORT0_END        12
#define HR4_GPORT1_START      13
#define HR4_GPORT1_END        24

/* Number of possible physical ports */
#define HR4_NUM_PHY_PORTS 64
#define HR4_NUM_ANCL_PORTS 4
/* Number of scheduling blocks */
#define HR4_NUM_QUAD 2
/* Number of scheduling blocks */
#define HR4_NUM_PIPE 1
/* Number of possible physical ports per pipe */
#define HR4_NUM_PHY_PORTS_PER_PIPE 68 /* 64 phy port + 4 MACSEC ports as MACSEC is included in .bcm file  */
/* Number of possible physical ports per halfpipe */
#define HR4_NUM_PHY_PORTS_PER_HPIPE 32

/* Number of slots per lane in OVSB Packet Scheduler */
#define HR4_NUM_PKT_SLOTS_PER_PM 16

/* Default number of ancillary ports in calendar */
#define HR4_NUM_ANCL 9
#define HR4_NUM_ANCL_CPU 1
#define HR4_NUM_ANCL_FAE 5
#define HR4_NUM_ANCL_MACSEC 80
#define HR4_NUM_ANCL_LPBK 1
#define HR4_NUM_ANCL_IDB_OPT1 1
#define HR4_NUM_ANCL_IDB_NULL 1
#define HR4_NUM_ANCL_IDB_IDLE 1

#define HR4_NUM_ANCL_MMU_OPT1 0
#define HR4_NUM_ANCL_MMU_NULL 1
#define HR4_NUM_ANCL_MMU_IDLE 3

/* Allocation length of VBS line rate calendar */
#define HR4_LR_VBS_LEN 512
/* Allocation area of VBS port groups */
#define HR4_OS_VBS_GRP_NUM 12
#define HR4_OS_VBS_GRP_LEN 12

/* Architecturally specific allocation area of VBS vector map */
#define HR4_VMAP_MAX_LEN HR4_LR_VBS_LEN
/* num of physical ports for vmap scheduler */ 
#define HR4_VMAP_MAX_WID 76

/* Min sister port spacing (VBS scheduler req) 4-2 */
#define HR4_MIN_SPACING_SISTER_PORT 2
#define HR4_MIN_SPACING_SISTER_FALCON_PORT 2
#define HR4_MIN_SPACING_SISTER_MACSEC_PORT 2
#define HR4_MIN_SPACING_SISTER_EAGLE_PORT 2
/* Min sister port spacing (VBS scheduler req) 8 - 6 , 14 - 6*/
#define HR4_MIN_SPACING_SAME_PORT_HSP 6
#define HR4_MIN_SPACING_SAME_PORT_LSP 6

/* static port identities 
        HR4_CMIC_TOKEN (  0): Pipe 0 - CPU slot
        HR4_MGM1_TOKEN (129): Pipe 1 - management port 1 slot
        HR4_MGM2_TOKEN (128): Pipe 1 - management port 2 slot
        HR4_LPB0_TOKEN (130): Pipe 0 - loopback slot
        HR4_LPB1_TOKEN (131): Pipe 1 - loopback slot
*/
#define HR4_CMIC_TOKEN      0
#define HR4_FAE_TOKEN      70
#define HR4_MACSEC_TOKEN_0 65
#define HR4_MACSEC_TOKEN_1 66
#define HR4_MACSEC_TOKEN_2 67
#define HR4_MACSEC_TOKEN_3 68
#define HR4_LPB0_TOKEN     69
#define HR4_IPFIX_TOKEN    70
/* #define HR4_MGM2_TOKEN (HR4_NUM_EXT_PORTS+3) */
/*#define HR4_LPB0_TOKEN (HR4_NUM_EXT_PORTS+1)*/
/*#define HR4_LPB1_TOKEN (HR4_NUM_EXT_PORTS+1)*/
/* Tokenization values 
        HR4_OVSB_TOKEN (133): token for oversub round robin
        HR4_IDL1_TOKEN (134): idle slot for memreset, L2 management, other
        HR4_IDL2_TOKEN (135): idle slot guaranteed for refresh
        HR4_NULL_TOKEN (136): null slot, no pick, no opportunistic
        HR4_ANCL_TOKEN (142): reservation for ancillary soc functions
*/
#define HR4_OVSB_TOKEN (HR4_NUM_EXT_PORTS+4)
#define HR4_IDL1_TOKEN (HR4_NUM_EXT_PORTS+5)
#define HR4_IDL2_TOKEN (HR4_NUM_EXT_PORTS+6)
#define HR4_NULL_TOKEN (HR4_NUM_EXT_PORTS+7)
#define HR4_ANCL_TOKEN (HR4_NUM_EXT_PORTS+10)

/* Calendar ID */
#define HR4_IDB_PIPE_0_CAL_ID 0
#define HR4_IDB_PIPE_1_CAL_ID 1
#define HR4_IDB_PIPE_2_CAL_ID 2
#define HR4_IDB_PIPE_3_CAL_ID 3
#define HR4_MMU_PIPE_0_CAL_ID 4
#define HR4_MMU_PIPE_1_CAL_ID 5
#define HR4_MMU_PIPE_2_CAL_ID 6
#define HR4_MMU_PIPE_3_CAL_ID 7

/* Physical port number range of each pipe */
#define HR4_CAL_0_PORT_LO 1
#define HR4_CAL_0_PORT_HI 64
#define HR4_CAL_1_PORT_LO 0
#define HR4_CAL_1_PORT_HI 0
#define HR4_CAL_2_PORT_LO 0
#define HR4_CAL_2_PORT_HI 0
#define HR4_CAL_3_PORT_LO 0
#define HR4_CAL_3_PORT_HI 0
#define HR4_CAL_4_PORT_LO 1
#define HR4_CAL_4_PORT_HI 64
#define HR4_CAL_5_PORT_LO 0
#define HR4_CAL_5_PORT_HI 0
#define HR4_CAL_6_PORT_LO 0
#define HR4_CAL_6_PORT_HI 0
#define HR4_CAL_7_PORT_LO 0
#define HR4_CAL_7_PORT_HI 0

/* Management port mode (10G per management port) 
       0 -> two Etherner management ports (default)
       1 -> one Ethernet management port
       2 -> two HiGig management ports (reserved)
       3 -> one HiGig management port (reserved)
*/
enum hr4_mgmt_mode_e {
    HR4_MGMT_MODE_2_PORT_EN=0,
    HR4_MGMT_MODE_1_PORT_EN=1,
    HR4_MGMT_MODE_2_PORT_HG=2,
    HR4_MGMT_MODE_1_PORT_HG=3
};

/* 1G port state
       0 -> None 1G port
       1 -> Regular 1G port
       2 -> PM4x10Q 1G port (16x1G super PM subport)
*/
enum hr4_1g_port_state_e {
    HR4_1G_PORT_STATE_NONE=0,
    HR4_1G_PORT_STATE_REGULAR=1,
    HR4_1G_PORT_STATE_16x1G=2,
    HR4_1G_PORT_STATE_16x2G=3
};
/* Index and length of shaping calendar within each inst */
#define HR4_SHAPING_GRP_NUM 2
#define HR4_SHAPING_GRP_LEN 160
#define HR4_SHAPING_GRP_IDX_0 HR4_OS_VBS_GRP_NUM
#define HR4_SHAPING_GRP_IDX_1 (HR4_OS_VBS_GRP_NUM+1)

/* Linerate jitter threshold */
#define HR4_LR_JITTER_LO 5
#define HR4_LR_JITTER_MD 2
#define HR4_LR_JITTER_HI 1
#define HR4_LR_CHK_TRHD  0 

#endif /* TDM_HR4_PREPROCESSOR_MACROS_H */
