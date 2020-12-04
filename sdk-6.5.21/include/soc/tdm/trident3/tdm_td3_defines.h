/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM macro values for BCM56870
 */

#ifndef TDM_TD3_PREPROCESSOR_MACROS_H
#define TDM_TD3_PREPROCESSOR_MACROS_H

/* Frequency */
#define TD3_CLK_1700MHZ 1700
#define TD3_CLK_1625MHZ 1625
#define TD3_CLK_1525MHZ 1525
#define TD3_CLK_1425MHZ 1425
#define TD3_CLK_1325MHZ 1325
#define TD3_CLK_1275MHZ 1275
#define TD3_CLK_1133MHZ 1133
#define TD3_CLK_1125MHZ 1125
#define TD3_CLK_1083MHZ 1083
#define TD3_CLK_1016MHZ 1016
#define TD3_CLK_1012MHZ 1012
#define TD3_CLK_950MHZ 950
#define TD3_CLK_883MHZ 883
#define TD3_CLK_850MHZ 850

/* Calendar Length */
#define TD3_LEN_1700MHZ_HG 430
#define TD3_LEN_1700MHZ_EN 456
#define TD3_LEN_1625MHZ_HG 411
#define TD3_LEN_1625MHZ_EN 436
#define TD3_LEN_1525MHZ_HG 386
#define TD3_LEN_1525MHZ_EN 408
#define TD3_LEN_1425MHZ_HG 361
#define TD3_LEN_1425MHZ_EN 383
#define TD3_LEN_1325MHZ_HG 335
#define TD3_LEN_1325MHZ_EN 356
#define TD3_LEN_1275MHZ_HG 323
#define TD3_LEN_1275MHZ_EN 342
#define TD3_LEN_1133MHZ_HG 287
#define TD3_LEN_1133MHZ_EN 304
#define TD3_LEN_1125MHZ_HG 285
#define TD3_LEN_1125MHZ_EN 302
#define TD3_LEN_1083MHZ_HG 274
#define TD3_LEN_1083MHZ_EN 291
#define TD3_LEN_1016MHZ_HG 257
#define TD3_LEN_1016MHZ_EN 273
#define TD3_LEN_1012MHZ_HG 256
#define TD3_LEN_1012MHZ_EN 272
#define TD3_LEN_950MHZ_HG 240
#define TD3_LEN_950MHZ_EN 255
#define TD3_LEN_883MHZ_HG 223
#define TD3_LEN_883MHZ_EN 237
#define TD3_LEN_850MHZ_HG 215
#define TD3_LEN_850MHZ_EN 228

/* Default number of PM in chip */
#define TD3_NUM_PM_MOD 33
#define TD3_NUM_PHY_PM 32
/* Default number of lanes per PM in chip */
#define TD3_NUM_PM_LNS 4
/* Allocation length of port lookup tables */
#define TD3_NUM_EXT_PORTS 132

/* Number of possible physical ports */
#define TD3_NUM_PHY_PORTS 128
/* Number of scheduling blocks */
#define TD3_NUM_QUAD 2
/* Number of scheduling blocks */
#define TD3_NUM_PIPE 2
/* Number of Subpipes per pipe */
#define TD3_NUM_SPIPE 2
/* Number of possible physical ports per pipe */
#define TD3_NUM_PHY_PORTS_PER_PIPE 64
/* Number of possible physical ports per subpipe */
#define TD3_NUM_PHY_PORTS_PER_SPIPE 32

/* Number of slots per lane in OVSB Packet Scheduler */
#define TD3_NUM_PKT_SLOTS_PER_LANE 5
/* Number of slots per PM in OVSB Packet Scheduler */
#define TD3_NUM_PKT_SLOTS_PER_PM 20

/* Number of PMs per pipe : 16 */
#define TD3_NUM_PMS_PER_PIPE (TD3_NUM_PHY_PM / TD3_NUM_PIPE)
/* Number of PMs per subpipe : 8 */
#define TD3_NUM_PMS_PER_SPIPE (TD3_NUM_PMS_PER_PIPE / TD3_NUM_SPIPE)

/* Default number of ancillary ports in calendar */
#define TD3_NUM_ANCL 16
#define TD3_NUM_ANCL_CPU 8
#define TD3_NUM_ANCL_MGMT 8
#define TD3_NUM_ANCL_LPBK 4
#define TD3_NUM_ANCL_IDB_OPT1 1
#define TD3_NUM_ANCL_IDB_NULL 1
#define TD3_NUM_ANCL_IDB_IDLE 2
#define TD3_NUM_ANCL_MMU_OPT1 0
#define TD3_NUM_ANCL_MMU_NULL 3
#define TD3_NUM_ANCL_MMU_IDLE 1

/* Allocation length of VBS line rate calendar */
#define TD3_LR_VBS_LEN 512
/* Allocation area of VBS port groups */
#define TD3_OS_VBS_GRP_NUM 12
#define TD3_OS_VBS_GRP_LEN 12

/* Architecturally specific allocation area of VBS vector map */
#define TD3_VMAP_MAX_LEN TD3_LR_VBS_LEN
#define TD3_VMAP_MAX_WID 128

/* Min sister port spacing (VBS scheduler req) */
#define TD3_MIN_SPACING_SISTER_PORT 4
/* Min sister port spacing (VBS scheduler req) */
#define TD3_MIN_SPACING_SAME_PORT_HSP 8
#define TD3_MIN_SPACING_SAME_PORT_LSP 14

/* static port identities 
        TD3_CMIC_TOKEN (  0): Pipe 0 - CPU slot
        TD3_MGM1_TOKEN (129): Pipe 1 - management port 1 slot
        TD3_MGM2_TOKEN (128): Pipe 1 - management port 2 slot
        TD3_LPB0_TOKEN (130): Pipe 0 - loopback slot
        TD3_LPB1_TOKEN (131): Pipe 1 - loopback slot
*/
#define TD3_CMIC_TOKEN 0
#define TD3_MGM1_TOKEN (TD3_NUM_EXT_PORTS-3)
#define TD3_MGM2_TOKEN (TD3_NUM_EXT_PORTS-4)
#define TD3_LPB0_TOKEN (TD3_NUM_EXT_PORTS-2)
#define TD3_LPB1_TOKEN (TD3_NUM_EXT_PORTS-1)
/* Tokenization values 
        TD3_OVSB_TOKEN (133): token for oversub round robin
        TD3_IDL1_TOKEN (134): idle slot for memreset, L2 management, other
        TD3_IDL2_TOKEN (135): idle slot guaranteed for refresh
        TD3_NULL_TOKEN (136): null slot, no pick, no opportunistic
        TD3_ANCL_TOKEN (142): reservation for ancillary soc functions
*/
#define TD3_OVSB_TOKEN (TD3_NUM_EXT_PORTS+1)
#define TD3_IDL1_TOKEN (TD3_NUM_EXT_PORTS+2)
#define TD3_IDL2_TOKEN (TD3_NUM_EXT_PORTS+3)
#define TD3_NULL_TOKEN (TD3_NUM_EXT_PORTS+4)
#define TD3_ANCL_TOKEN (TD3_NUM_EXT_PORTS+10)

/* Calendar ID */
#define TD3_IDB_PIPE_0_CAL_ID 0
#define TD3_IDB_PIPE_1_CAL_ID 1
#define TD3_IDB_PIPE_2_CAL_ID 2
#define TD3_IDB_PIPE_3_CAL_ID 3
#define TD3_MMU_PIPE_0_CAL_ID 4
#define TD3_MMU_PIPE_1_CAL_ID 5
#define TD3_MMU_PIPE_2_CAL_ID 6
#define TD3_MMU_PIPE_3_CAL_ID 7

/* Physical port number range of each pipe */
#define TD3_CAL_0_PORT_LO 1
#define TD3_CAL_0_PORT_HI 64
#define TD3_CAL_1_PORT_LO 65
#define TD3_CAL_1_PORT_HI 128
#define TD3_CAL_2_PORT_LO 0
#define TD3_CAL_2_PORT_HI 0
#define TD3_CAL_3_PORT_LO 0
#define TD3_CAL_3_PORT_HI 0
#define TD3_CAL_4_PORT_LO 1
#define TD3_CAL_4_PORT_HI 64
#define TD3_CAL_5_PORT_LO 65
#define TD3_CAL_5_PORT_HI 128
#define TD3_CAL_6_PORT_LO 0
#define TD3_CAL_6_PORT_HI 0
#define TD3_CAL_7_PORT_LO 0
#define TD3_CAL_7_PORT_HI 0

/* Management port mode (10G per management port) 
       0 -> two Etherner management ports (default)
       1 -> one Ethernet management port
       2 -> two HiGig management ports (reserved)
       3 -> one HiGig management port (reserved)
*/
enum td3_mgmt_mode_e {
    TD3_MGMT_MODE_2_PORT_EN=0,
    TD3_MGMT_MODE_1_PORT_EN=1,
    TD3_MGMT_MODE_2_PORT_HG=2,
    TD3_MGMT_MODE_1_PORT_HG=3
};

/* Index and length of shaping calendar within each inst */
#define TD3_SHAPING_GRP_NUM 2
#define TD3_SHAPING_GRP_LEN 160
#define TD3_SHAPING_GRP_IDX_0 TD3_OS_VBS_GRP_NUM
#define TD3_SHAPING_GRP_IDX_1 (TD3_OS_VBS_GRP_NUM+1)
/* granularity of packet shaper calendar (in Mbps): 5000 */
#define TD3_SHAPER_SLOT_UNIT (SPEED_5G)
/* max speed per lane within each PM (in Mbps): 25000 */
#define TD3_SHAPER_MAX_SPD_PER_LN (SPEED_25G)
/* number of slots per sublane: 5*/
#define TD3_SHAPER_NUM_LN_SLOTS 5
/* number of slots per PM: 20*/
#define TD3_SHAPER_NUM_PM_SLOTS (TD3_SHAPER_NUM_LN_SLOTS*TD3_NUM_PM_LNS)
/* number of slots packet shaper calendar: 160 */
#define TD3_SHAPER_LEN (TD3_SHAPER_NUM_PM_SLOTS*TD3_NUM_PMS_PER_SPIPE)

/* Linerate jitter threshold */
#define TD3_LR_JITTER_LO 5
#define TD3_LR_JITTER_MD 2
#define TD3_LR_JITTER_HI 1
#define TD3_LR_CHK_TRHD TD3_LR_JITTER_HI

#endif /* TDM_TD3_PREPROCESSOR_MACROS_H */
