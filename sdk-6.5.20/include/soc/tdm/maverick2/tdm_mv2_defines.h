/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM macro values for Maverick2
 */

#ifndef TDM_MV2_PREPROCESSOR_MACROS_H
#define TDM_MV2_PREPROCESSOR_MACROS_H

/* Device ID */
#ifdef _TDM_STANDALONE
    #define TDM_MV2_DEV_ID_MASK 0xffff
    #define TDM_MV2_DEV_ID_BCM56770 56770
    #define TDM_MV2_DEV_ID_BCM56771 56771
#else
    #define TDM_MV2_DEV_ID_MASK 0x0fff
    #define TDM_MV2_DEV_ID_BCM56770 0x56770
    #define TDM_MV2_DEV_ID_BCM56771 0x56771
#endif

/* Frequency */
#define MV2_CLK_1700MHZ 1700
#define MV2_CLK_1625MHZ 1625
#define MV2_CLK_1525MHZ 1525
#define MV2_CLK_1425MHZ 1425
#define MV2_CLK_1325MHZ 1325
#define MV2_CLK_1275MHZ 1275
#define MV2_CLK_1133MHZ 1133
#define MV2_CLK_1125MHZ 1125
#define MV2_CLK_1083MHZ 1083
#define MV2_CLK_1016MHZ 1016
#define MV2_CLK_1012MHZ 1012
#define MV2_CLK_950MHZ 950
#define MV2_CLK_883MHZ 883
#define MV2_CLK_850MHZ 850

/* Calendar Length */
#define MV2_LEN_1700MHZ_HG 430
#define MV2_LEN_1700MHZ_EN 456
#define MV2_LEN_1625MHZ_HG 411
#define MV2_LEN_1625MHZ_EN 436
#define MV2_LEN_1525MHZ_HG 386
#define MV2_LEN_1525MHZ_EN 408
#define MV2_LEN_1425MHZ_HG 361
#define MV2_LEN_1425MHZ_EN 383
#define MV2_LEN_1325MHZ_HG 335
#define MV2_LEN_1325MHZ_EN 356
#define MV2_LEN_1275MHZ_HG 323
#define MV2_LEN_1275MHZ_EN 342
#define MV2_LEN_1133MHZ_HG 287
#define MV2_LEN_1133MHZ_EN 304
#define MV2_LEN_1125MHZ_HG 285
#define MV2_LEN_1125MHZ_EN 302
#define MV2_LEN_1083MHZ_HG 274
#define MV2_LEN_1083MHZ_EN 291
#define MV2_LEN_1016MHZ_HG 257
#define MV2_LEN_1016MHZ_EN 273
#define MV2_LEN_1012MHZ_HG 256
#define MV2_LEN_1012MHZ_EN 272
#define MV2_LEN_950MHZ_HG 240
#define MV2_LEN_950MHZ_EN 255
#define MV2_LEN_883MHZ_HG 223
#define MV2_LEN_883MHZ_EN 237
#define MV2_LEN_850MHZ_HG 215
#define MV2_LEN_850MHZ_EN 228

/* Default number of PM in chip */
#define MV2_NUM_PHY_PM 20 /* 20 -> 18 */
#define MV2_NUM_PM_MOD 21 /* 21 -> 19 */
/* Default number of lanes per PM in chip */
#define MV2_NUM_PM_LNS 4
/* Number of logical ports: phy ports + cpu + mgmt + lpbk : 83 -> 75 */
#define MV2_NUM_EXT_PORTS 83

/* Number of physical ports : 80 -> 72 */
#define MV2_NUM_PHY_PORTS 80
/* Number of scheduling blocks */
#define MV2_NUM_QUAD 1
/* Number of scheduling blocks */
#define MV2_NUM_PIPE 1
/* Number of possible physical ports per pipe : 80 -> 72 */
#define MV2_NUM_PHY_PORTS_PER_PIPE (MV2_NUM_PHY_PORTS / MV2_NUM_PIPE)
/* Number of possible physical ports per halfpipe: 40 -> 36 */
#define MV2_NUM_PHY_PORTS_PER_HPIPE (MV2_NUM_PHY_PORTS_PER_PIPE / 2)
/* Number of PMs per pipe : 20 -> 18 */
#define MV2_NUM_PMS_PER_PIPE (MV2_NUM_PHY_PM / MV2_NUM_PIPE)
/* Number of PMs per halfpipe : 10 -> 9 */
#define MV2_NUM_PMS_PER_HPIPE (MV2_NUM_PHY_PM / MV2_NUM_PIPE / 2)

/* Number of slots per lane in OVSB Packet Scheduler */
#define MV2_NUM_PKT_SLOTS_PER_LANE 5
/* Number of slots per PM in OVSB Packet Scheduler */
#define MV2_NUM_PKT_SLOTS_PER_PM 20

/* Default number of ancillary ports in calendar */
#define MV2_NUM_ANCL 16
#define MV2_NUM_ANCL_CPU 4
#define MV2_NUM_ANCL_MGMT 4
#define MV2_NUM_ANCL_LPBK 4
#define MV2_NUM_ANCL_IDB_OPT1 1
#define MV2_NUM_ANCL_IDB_NULL 1
#define MV2_NUM_ANCL_IDB_IDLE 2
#define MV2_NUM_ANCL_MMU_OPT1 0
#define MV2_NUM_ANCL_MMU_NULL 3
#define MV2_NUM_ANCL_MMU_IDLE 1

/* Allocation length of VBS line rate calendar */
#define MV2_LR_VBS_LEN 512
/* Allocation area of VBS port groups */
#define MV2_OS_VBS_GRP_NUM 12
#define MV2_OS_VBS_GRP_LEN 12

/* Architecturally specific allocation area of VBS vector map */
#define MV2_VMAP_MAX_LEN MV2_LR_VBS_LEN
#define MV2_VMAP_MAX_WID 128

/* Min sister port spacing (VBS scheduler req) */
#define MV2_MIN_SPACING_SISTER_PORT 4
/* Min sister port spacing (VBS scheduler req) */
#define MV2_MIN_SPACING_SAME_PORT_HSP 8
#define MV2_MIN_SPACING_SAME_PORT_LSP 14

/* Static port identities (global)
        MV2_CMIC_TOKEN ( 0): Pipe 0 - CPU slot
        MV2_MGM1_TOKEN (81): Pipe 0 - management slot
        MV2_LPB0_TOKEN (82): Pipe 0 - loopback slot

        MV2_MGM2_TOKEN (900): unused
        MV2_LPB1_TOKEN (901): unused
*/
#define MV2_CMIC_TOKEN 0
#define MV2_MGM1_TOKEN (MV2_NUM_EXT_PORTS-2)
#define MV2_LPB0_TOKEN (MV2_NUM_EXT_PORTS-1)
#define MV2_MGM2_TOKEN 900
#define MV2_LPB1_TOKEN 901

/* Tokenization values (global)
        MV2_OVSB_TOKEN (84): token for oversub round robin
        MV2_IDL1_TOKEN (85): idle slot for memreset, L2 management, SBus
        MV2_IDL2_TOKEN (86): idle slot guaranteed for refresh
        MV2_NULL_TOKEN (87): null slot, no pick, no opportunistic
*/
#define MV2_OVSB_TOKEN (MV2_NUM_EXT_PORTS+1)
#define MV2_IDL1_TOKEN (MV2_NUM_EXT_PORTS+2)
#define MV2_IDL2_TOKEN (MV2_NUM_EXT_PORTS+3)
#define MV2_NULL_TOKEN (MV2_NUM_EXT_PORTS+4)

/* Tokenization values (for TDM internal usage)
        MV2_ANCL_TOKEN (93): reservation for ancillary soc functions
*/
#define MV2_ANCL_TOKEN (MV2_NUM_EXT_PORTS+10)

/* Calendar ID */
#define MV2_IDB_PIPE_0_CAL_ID 0
#define MV2_IDB_PIPE_1_CAL_ID 1
#define MV2_IDB_PIPE_2_CAL_ID 2
#define MV2_IDB_PIPE_3_CAL_ID 3
#define MV2_MMU_PIPE_0_CAL_ID 4
#define MV2_MMU_PIPE_1_CAL_ID 5
#define MV2_MMU_PIPE_2_CAL_ID 6
#define MV2_MMU_PIPE_3_CAL_ID 7

/* Physical port number range of each pipe */
#define MV2_CAL_0_PORT_LO 1
#define MV2_CAL_0_PORT_HI 80
#define MV2_CAL_1_PORT_LO 0
#define MV2_CAL_1_PORT_HI 0
#define MV2_CAL_2_PORT_LO 0
#define MV2_CAL_2_PORT_HI 0
#define MV2_CAL_3_PORT_LO 0
#define MV2_CAL_3_PORT_HI 0
#define MV2_CAL_4_PORT_LO 1
#define MV2_CAL_4_PORT_HI 80
#define MV2_CAL_5_PORT_LO 0
#define MV2_CAL_5_PORT_HI 0
#define MV2_CAL_6_PORT_LO 0
#define MV2_CAL_6_PORT_HI 0
#define MV2_CAL_7_PORT_LO 0
#define MV2_CAL_7_PORT_HI 0

/* Management port mode (10G per management port)
       0 -> Etherner management port (default)
       1 -> Disable management port
       2 -> HiGig management port
*/
enum mv2_mgmt_mode_e {
    MV2_MGMT_MODE_PORT_EN=0,
    MV2_MGMT_MODE_PORT_DISABLE=1,
    MV2_MGMT_MODE_PORT_HG=2
};

/* Index and length of shaping calendar within each inst */
#define MV2_SHAPING_GRP_NUM 2
#define MV2_SHAPING_GRP_LEN (MV2_NUM_PMS_PER_HPIPE * MV2_NUM_PKT_SLOTS_PER_PM)
#define MV2_SHAPING_GRP_IDX_0 MV2_OS_VBS_GRP_NUM
#define MV2_SHAPING_GRP_IDX_1 (MV2_OS_VBS_GRP_NUM+1)

/* Linerate jitter threshold */
#define MV2_LR_JITTER_LO 5
#define MV2_LR_JITTER_MD 2
#define MV2_LR_JITTER_HI 1
#define MV2_LR_CHK_TRHD  0 

/* Number of speed types in enum mv2_port_speed_indx_e */
#define MV2_PORT_SPEED_INDX_SIZE 6

#endif /* TDM_MV2_PREPROCESSOR_MACROS_H */
