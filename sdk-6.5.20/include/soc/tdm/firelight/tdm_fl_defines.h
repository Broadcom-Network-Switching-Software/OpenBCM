/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM macro values for Firelight
 */

#ifndef TDM_FL_PREPROCESSOR_MACROS_H
#define TDM_FL_PREPROCESSOR_MACROS_H

/* Device ID */
#ifdef _TDM_STANDALONE
    #define TDM_FL_DEV_ID_MASK 0xffff
    #define TDM_FL_DEV_ID_BCM56770 56770
    #define TDM_FL_DEV_ID_BCM56771 56771
#else
    #define TDM_FL_DEV_ID_MASK 0x0fff
    #define TDM_FL_DEV_ID_BCM56770 0x56770
    #define TDM_FL_DEV_ID_BCM56771 0x56771
#endif

/* Frequency */
#define FL_CLK_850MHZ 850
#define FL_CLK_680MHZ 680
#define FL_CLK_667MHZ 667

#define FL_CLK_219MHZ 219
#define FL_CLK_313MHZ 313
#define FL_CLK_375MHZ 375
#define FL_CLK_438MHZ 438
#define FL_CLK_500MHZ 500
#define FL_CLK_625MHZ 625
#define FL_CLK_688MHZ 688
#define FL_CLK_700MHZ 700

#define FL_CLK_250MHZ 250
#define FL_CLK_282MHZ 282
#define FL_CLK_344MHZ 344
#define FL_CLK_407MHZ 407
#define FL_CLK_469MHZ 469
#define FL_CLK_532MHZ 532
#define FL_CLK_563MHZ 563
#define FL_CLK_594MHZ 594
#define FL_CLK_719MHZ 719


/* Calendar Length */
/* Set DIFFERENT length for Ethernet and HiGig */
/* NOTE:
 * 1) The slot granularity of 667MHze is 1G.
 * 2) The slot granularity of all other frequencies are 2.5G.
 * 3) Treat 700MHz as 688MHz (687.5MHz).
 */
/*
#define FL_LEN_667MHZ_HG 169
#define FL_LEN_667MHZ_EN 176
*/
#define FL_LEN_667MHZ_HG 448
#define FL_LEN_667MHZ_EN 448

#define FL_LEN_850MHZ_HG 215
#define FL_LEN_850MHZ_EN 224
#define FL_LEN_680MHZ_HG 172
#define FL_LEN_680MHZ_EN 178

#define FL_LEN_219MHZ_HG 55
#define FL_LEN_219MHZ_EN 56
#define FL_LEN_313MHZ_HG 78
#define FL_LEN_313MHZ_EN 82
#define FL_LEN_375MHZ_HG 94
#define FL_LEN_375MHZ_EN 96
#define FL_LEN_438MHZ_HG 110
#define FL_LEN_438MHZ_EN 115
#define FL_LEN_500MHZ_HG 126
#define FL_LEN_500MHZ_EN 132
#define FL_LEN_625MHZ_HG 158
#define FL_LEN_625MHZ_EN 164
#define FL_LEN_688MHZ_HG 174
#define FL_LEN_688MHZ_EN 180
#define FL_LEN_700MHZ_HG 174
#define FL_LEN_700MHZ_EN 180

#define FL_LEN_250MHZ_HG 62
#define FL_LEN_250MHZ_EN 66
#define FL_LEN_282MHZ_HG 70
#define FL_LEN_282MHZ_EN 74
#define FL_LEN_344MHZ_HG 86
#define FL_LEN_344MHZ_EN 90
#define FL_LEN_407MHZ_HG 102
#define FL_LEN_407MHZ_EN 106
#define FL_LEN_469MHZ_HG 118
#define FL_LEN_469MHZ_EN 122
#define FL_LEN_532MHZ_HG 134
#define FL_LEN_532MHZ_EN 140
#define FL_LEN_563MHZ_HG 142
#define FL_LEN_563MHZ_EN 148
#define FL_LEN_594MHZ_HG 150
#define FL_LEN_594MHZ_EN 156
#define FL_LEN_719MHZ_HG 182
#define FL_LEN_719MHZ_EN 188

/* Set SAME length for Ethernet and HiGig */
/*
#define FL_LEN_667MHZ_HG 448
#define FL_LEN_667MHZ_EN 448
#define FL_LEN_680MHZ_HG 172
#define FL_LEN_680MHZ_EN 172
#define FL_LEN_850MHZ_HG 215
#define FL_LEN_850MHZ_EN 215

#define FL_LEN_219MHZ_HG 55
#define FL_LEN_219MHZ_EN 55
#define FL_LEN_313MHZ_HG 78
#define FL_LEN_313MHZ_EN 78
#define FL_LEN_375MHZ_HG 94
#define FL_LEN_375MHZ_EN 94
#define FL_LEN_438MHZ_HG 110
#define FL_LEN_438MHZ_EN 110
#define FL_LEN_500MHZ_HG 126
#define FL_LEN_500MHZ_EN 126
#define FL_LEN_625MHZ_HG 158
#define FL_LEN_625MHZ_EN 158
#define FL_LEN_688MHZ_HG 174
#define FL_LEN_688MHZ_EN 174
#define FL_LEN_700MHZ_HG 174
#define FL_LEN_700MHZ_EN 174

#define FL_LEN_250MHZ_HG 62
#define FL_LEN_250MHZ_EN 62
#define FL_LEN_282MHZ_HG 70
#define FL_LEN_282MHZ_EN 70
#define FL_LEN_344MHZ_HG 86
#define FL_LEN_344MHZ_EN 86
#define FL_LEN_407MHZ_HG 102
#define FL_LEN_407MHZ_EN 102
#define FL_LEN_469MHZ_HG 118
#define FL_LEN_469MHZ_EN 118
#define FL_LEN_532MHZ_HG 134
#define FL_LEN_532MHZ_EN 134
#define FL_LEN_563MHZ_HG 142
#define FL_LEN_563MHZ_EN 142
#define FL_LEN_594MHZ_HG 150
#define FL_LEN_594MHZ_EN 150
#define FL_LEN_719MHZ_HG 182
#define FL_LEN_719MHZ_EN 182
*/
/* Number of logical ports: phy ports + cmic + (TDM reserve) : 76+1+(3) -> 80 */
#define FL_NUM_EXT_PORTS 80
/* Number of physical ports : 77-1 */
#define FL_NUM_PHY_PORTS 76


/* Default number of PM in chip */
#define FL_NUM_PHY_PM 20
#define FL_NUM_PM_MOD 20
/* Default number of lanes per PM in chip */
#define FL_NUM_PM_LNS 4
/* Number of scheduling blocks */
#define FL_NUM_QUAD 1
/* Number of scheduling blocks */
#define FL_NUM_PIPE 1
/* Number of possible physical ports per pipe : 80 -> 72 */
#define FL_NUM_PHY_PORTS_PER_PIPE (FL_NUM_PHY_PORTS / FL_NUM_PIPE)
/* Number of possible physical ports per halfpipe: 40 -> 36 */
#define FL_NUM_PHY_PORTS_PER_HPIPE (FL_NUM_PHY_PORTS_PER_PIPE / 2)
/* Number of PMs per pipe : 20 -> 18 */
#define FL_NUM_PMS_PER_PIPE (FL_NUM_PHY_PM / FL_NUM_PIPE)
/* Number of PMs per halfpipe : 10 -> 9 */
#define FL_NUM_PMS_PER_HPIPE (FL_NUM_PHY_PM / FL_NUM_PIPE / 2)

/* Number of slots per lane in OVSB Packet Scheduler */
#define FL_NUM_PKT_SLOTS_PER_LANE 5
/* Number of slots per PM in OVSB Packet Scheduler */
#define FL_NUM_PKT_SLOTS_PER_PM 20

/* Default number of ancillary ports in calendar */
#define FL_NUM_ANCL 1
#define FL_NUM_ANCL_CPU 1
#define FL_NUM_ANCL_MGMT 0
#define FL_NUM_ANCL_LPBK 0
#define FL_NUM_ANCL_IDB_OPT1 0
#define FL_NUM_ANCL_IDB_NULL 0
#define FL_NUM_ANCL_IDB_IDLE 0
#define FL_NUM_ANCL_MMU_OPT1 0
#define FL_NUM_ANCL_MMU_NULL 0
#define FL_NUM_ANCL_MMU_IDLE 0

/* Allocation length of VBS line rate calendar */
#define FL_LR_VBS_LEN 512
/* Allocation area of VBS port groups */
#define FL_OS_VBS_GRP_NUM 8
#define FL_OS_VBS_GRP_LEN 16

/* Architecturally specific allocation area of VBS vector map */
#define FL_VMAP_MAX_LEN FL_LR_VBS_LEN
#define FL_VMAP_MAX_WID 128

/* Min sister port spacing (VBS scheduler req) */
#define FL_MIN_SPACING_SISTER_PORT 4
/* Min sister port spacing (VBS scheduler req) */
#define FL_MIN_SPACING_SAME_PORT_HSP 4
#define FL_MIN_SPACING_SAME_PORT_LSP 4

/* Static port identities (global)
        FL_CMIC_TOKEN ( 0): Pipe 0 - CPU slot
        FL_MGM1_TOKEN (9xx): unused
        FL_LPB0_TOKEN (9xx): unused

        FL_MGM2_TOKEN (9xx): unused
        FL_LPB1_TOKEN (9xx): unused
*/
#define FL_CMIC_TOKEN 0
#define FL_MGM1_TOKEN 900
#define FL_MGM2_TOKEN 901
#define FL_LPB0_TOKEN 902
#define FL_LPB1_TOKEN 903
#define FL_LPB2_TOKEN 904
#define FL_LPB3_TOKEN 905

/* Tokenization values (global)
        FL_OVSB_TOKEN (): token for oversub round robin
        FL_IDL1_TOKEN (): idle slot for keeping spacing purpose
        FL_IDL2_TOKEN (): idle slot guaranteed for refresh
        FL_NULL_TOKEN (): null slot, no pick, no opportunistic
*/
#define FL_OVSB_TOKEN (FL_NUM_EXT_PORTS+1)
#define FL_IDL1_TOKEN (127)
#define FL_IDL2_TOKEN (FL_NUM_EXT_PORTS+3)
#define FL_NULL_TOKEN (FL_NUM_EXT_PORTS+4)

/* Tokenization values (for TDM internal usage)
        FL_ANCL_TOKEN (93): reservation for ancillary soc functions
*/
#define FL_ANCL_TOKEN (FL_NUM_EXT_PORTS+10)

/* Calendar ID */
#define FL_IDB_PIPE_0_CAL_ID 0
#define FL_IDB_PIPE_1_CAL_ID 1
#define FL_IDB_PIPE_2_CAL_ID 2
#define FL_IDB_PIPE_3_CAL_ID 3
#define FL_MMU_PIPE_0_CAL_ID 4
#define FL_MMU_PIPE_1_CAL_ID 5
#define FL_MMU_PIPE_2_CAL_ID 6
#define FL_MMU_PIPE_3_CAL_ID 7

/* Physical port number range of each pipe */
#define FL_CAL_0_PORT_LO 1
#define FL_CAL_0_PORT_HI 80
#define FL_CAL_1_PORT_LO 0
#define FL_CAL_1_PORT_HI 0
#define FL_CAL_2_PORT_LO 0
#define FL_CAL_2_PORT_HI 0
#define FL_CAL_3_PORT_LO 0
#define FL_CAL_3_PORT_HI 0
#define FL_CAL_4_PORT_LO 1
#define FL_CAL_4_PORT_HI 80
#define FL_CAL_5_PORT_LO 0
#define FL_CAL_5_PORT_HI 0
#define FL_CAL_6_PORT_LO 0
#define FL_CAL_6_PORT_HI 0
#define FL_CAL_7_PORT_LO 0
#define FL_CAL_7_PORT_HI 0

/* Management port mode (10G per management port)
       0 -> Etherner management port (default)
       1 -> Disable management port
       2 -> HiGig management port
*/
enum fl_mgmt_mode_e {
    FL_MGMT_MODE_DISABLE=0,
    FL_MGMT_MODE_ETH=1,
    FL_MGMT_MODE_HG=2
};

/* Index and length of shaping calendar within each inst */
#define FL_SHAPING_GRP_NUM 2
#define FL_SHAPING_GRP_LEN (FL_NUM_PMS_PER_HPIPE * FL_NUM_PKT_SLOTS_PER_PM)
#define FL_SHAPING_GRP_IDX_0 FL_OS_VBS_GRP_NUM
#define FL_SHAPING_GRP_IDX_1 (FL_OS_VBS_GRP_NUM+1)

/* Linerate jitter threshold */
#define FL_LR_JITTER_LO 5
#define FL_LR_JITTER_MD 2
#define FL_LR_JITTER_HI 1
#define FL_LR_CHK_TRHD  0

/* Number of speed types in enum fl_port_speed_indx_e */
#define FL_PORT_SPEED_INDX_SIZE 6


/* Shift physical port mapping from [2,77] to [1,76] */
#define FL_PHY_PORT_NUMBER_OFFSET 1

/* Adjusted/Pseudo GPORT numbering */
#define FL_GPORT_PSEUDO_PHY_LO 1
#define FL_GPORT_PSEUDO_PHY_HI 48
#define FL_GPORT_PSEUDO_NUM_PMS 3
#define FL_GPORT_PSEUDO_SUBP_PER_PM 16
#define FL_GPORT_LANES_PER_PM 8

/* Macsec calendar */
#define FL_MS_MAX_CAL_LEN 96
#define FL_MS_SLOT_IDLE   49


#endif /* TDM_FL_PREPROCESSOR_MACROS_H */
