/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:      tcamtype1.h
 */

/* microcode location */
#define TYPE1_UCODE_ADDR                 0x06f00000

/* Database register accessing: address[19:18] is device ID */
#define TYPE1_DBREG_BASE(dev)            (((dev) & 0x03) << 18)
/* Indirect mapped address accessing: address[29:28] is device ID */
#define TYPE1_IMA_BASE(dev)              (((dev) & 0x03) << 28)

/*
 * register addresses
 */

/* Database register: Database Configuration Register */
#define TYPE1_DBREG_DBCFG_ADDR(dev)      (0xa0 + TYPE1_DBREG_BASE(dev))

/* Database register: Device ID Register */
#define TYPE1_DBREG_DEVID_ADDR(dev)      (0x08 + TYPE1_DBREG_BASE(dev))

/* Database register: Result Register */
#define TYPE1_DBREG_RESULT_ADDR(dev,i)   (0x10 + (i) + TYPE1_DBREG_BASE(dev))

/* Database register: Logical Table Register */
#define TYPE1_DBREG_LTBL_ADDR(dev,i)     (0x40 + (i) + TYPE1_DBREG_BASE(dev))

/* Database register: Block [0-n] Mask Register [0-7] */
#define TYPE1_DBREG_BMASK_ADDR(dev,blk,mask) (0x820 + (blk) * 0x100 + \
                                              ((blk) >= 8 ? 0x800 : 0) + \
                                              (mask) * 0x10 + \
                                              TYPE1_DBREG_BASE(dev))

/* Database register: Block Configuration Register */
#define TYPE1_DBREG_BCFG_ADDR(dev,blk)   (0x80 + (blk) + TYPE1_DBREG_BASE(dev))

/* Indirectly mapped register: NPU IO Parity and FIFO Control Register */
#define TYPE1_IMREG_QDR_FIFO_CTL_ADDR(dev) (0x0f000008 + TYPE1_IMA_BASE(dev))

/* Indirectly mapped register: Status Register */
#define TYPE1_IMREG_QDR_STATUS_ADDR(dev)   (0x0f000048 + TYPE1_IMA_BASE(dev))

/* Indirectly mapped register: Device Configuration Register */
#define TYPE1_IMREG_DEV_CFG_ADDR(dev)      (0x0f000000 + TYPE1_IMA_BASE(dev))

/* Indirectly mapped register: Clock Control Register */
#define TYPE1_IMREG_CLK_CTL_ADDR(dev)      (0x0f000010 + TYPE1_IMA_BASE(dev))

/* Indirectly mapped register: QDR IO Termination control Register */
#define TYPE1_IMREG_IO_TERM_CTL_ADDR(dev)  (0x0f000038 + TYPE1_IMA_BASE(dev))

/*
 * register bit fields and values
 */
#define MAKE_FIELD(field, val)  (((val) & field## _mask) << field## _oft)

/* Database register: Database Configuration Register */
#define TYPE1_DBREG_DBCFG_MSB_XYMODE_oft        22
#define TYPE1_DBREG_DBCFG_MSB_XYMODE_mask       1
#define TYPE1_ST3_DBREG_DBCFG_LSB_FG_PS_oft     3
#define TYPE1_ST3_DBREG_DBCFG_LSB_FG_PS_mask    0x01
#define TYPE1_ST3_DBREG_DBCFG_LSB_MIN_REV_oft   8
#define TYPE1_ST3_DBREG_DBCFG_LSB_MIN_REV_mask  0xff
#define TYPE1_ST3_DBREG_DBCFG_LSB_MAJ_REV_oft   16
#define TYPE1_ST3_DBREG_DBCFG_LSB_MAJ_REV_mask  0xff
#define TYPE1_ST3_DBREG_DBCFG_LSB_SIZE_oft      24
#define TYPE1_ST3_DBREG_DBCFG_LSB_SIZE_mask     0xff
#define TYPE1_ST3_DBREG_DBCFG_MSB_SIZE_oft      0
#define TYPE1_ST3_DBREG_DBCFG_MSB_SIZE_mask     0xff
#define TYPE1_ST3_DBREG_DBCFG_MSB_FAMILY_oft    20
#define TYPE1_ST3_DBREG_DBCFG_MSB_FAMILY_mask   0x03
#define TYPE1_ST3_DBREG_DBCFG_TOP8_CMP_ANY_oft  1
#define TYPE1_ST3_DBREG_DBCFG_TOP8_CMP_ANY_mask 0x01
#define TYPE1_ST3_DBREG_DBCFG_TOP8_MAP_KEY_oft  3
#define TYPE1_ST3_DBREG_DBCFG_TOP8_MAP_KEY_mask 0x01

/* Database register: Device ID Register */
#define TYPE1_DBREG_DEVID_ID_oft              0
#define TYPE1_DBREG_DEVID_ID_mask             0x03

/* Database register: Block Configuration Register */
#define TYPE1_DBREG_BCFG_WIDTH_oft            0
#define TYPE1_DBREG_BCFG_WIDTH_mask           0x0f
#define TYPE1_ST1_DBREG_BCFG_LTID_oft         4    /* used by ST2 as well */
#define TYPE1_ST1_DBREG_BCFG_LTID_mask        0x0f
#define TYPE1_ST3_DBREG_BCFG_LTID_oft         4
#define TYPE1_ST3_DBREG_BCFG_LTID_mask        0x1f
#define TYPE1_ST3_DBREG_BCFG_LPWR_oft         9
#define TYPE1_ST3_DBREG_BCFG_LPWR_mask        0x01

#define TYPE1_DBREG_BCFG_WIDTH_72             0
#define TYPE1_DBREG_BCFG_WIDTH_144            1
#define TYPE1_DBREG_BCFG_WIDTH_288            2
#define TYPE1_DBREG_BCFG_WIDTH_576            3

/* Database register: Logical Table Register */
#define TYPE1_DBREG_LTBL_BMASK_oft            0
#define TYPE1_DBREG_LTBL_BMASK_mask           0x07
#define TYPE1_ST1_DBREG_LTBL_LTID_oft         4
#define TYPE1_ST1_DBREG_LTBL_LTID_mask        0x07
#define TYPE1_ST2_DBREG_LTBL_LTID_oft         9
#define TYPE1_ST2_DBREG_LTBL_LTID_mask        0x0f
#define TYPE1_ST3_DBREG_LTBL_BMASK0_oft       0
#define TYPE1_ST3_DBREG_LTBL_BMASK0_mask      0x0f
#define TYPE1_ST3_DBREG_LTBL_LTID0_oft        9
#define TYPE1_ST3_DBREG_LTBL_LTID0_mask       0x1f
#define TYPE1_ST3_DBREG_LTBL_BMASK1_oft       16
#define TYPE1_ST3_DBREG_LTBL_BMASK1_mask      0x0f
#define TYPE1_ST3_DBREG_LTBL_LTID1_oft        25
#define TYPE1_ST3_DBREG_LTBL_LTID1_mask       0x1f
#define TYPE1_ST3_DBREG_LTBL_PCOMP_oft        0     /* bit 32 */
#define TYPE1_ST3_DBREG_LTBL_PCOMP_mask       0x01
#define TYPE1_ST3_DBREG_LTBL_KEY_oft          1     /* bit 33 */
#define TYPE1_ST3_DBREG_LTBL_KEY_mask         0x07

/* QDR interface register: Parity and FIFO Control Register */
#define TYPE1_IMREG_QDR_FIFO_LSB_0_AF_OFF_oft   0
#define TYPE1_IMREG_QDR_FIFO_LSB_0_AF_OFF_mask  0x0f
#define TYPE1_IMREG_QDR_FIFO_LSB_0_AF_ON_oft    4
#define TYPE1_IMREG_QDR_FIFO_LSB_0_AF_ON_mask   0x0f
#define TYPE1_IMREG_QDR_FIFO_LSB_1_AF_OFF_oft   16
#define TYPE1_IMREG_QDR_FIFO_LSB_1_AF_OFF_mask  0x0f
#define TYPE1_IMREG_QDR_FIFO_LSB_1_AF_ON_oft    20
#define TYPE1_IMREG_QDR_FIFO_LSB_1_AF_ON_mask   0x0f
#define TYPE1_IMREG_QDR_FIFO_MSB_RSVD_oft       0
#define TYPE1_IMREG_QDR_FIFO_MSB_RSVD_mask      0x7fff
#define TYPE1_IMREG_QDR_FIFO_MSB_MODE_oft       17
#define TYPE1_IMREG_QDR_FIFO_MSB_MODE_mask      0x01
#define TYPE1_IMREG_QDR_FIFO_MSB_FCI_oft        18
#define TYPE1_IMREG_QDR_FIFO_MSB_FCI_mask       0x01

#define TYPE1_IMREG_QDR_FIFO_MSB_RSVD_VALUE     0x0520

/* QDR interface register: QDR Status Register */
#define TYPE1_IMREG_QDR_STATUS_LSB_5000         0x00005001
#define TYPE1_IMREG_QDR_STATUS_LSB_3280         0x00015001

/* I/O control register: QDR IO Termination control Register */
#define TYPE1_IMREG_IO_TERM_CTL_INIT_LSB        0x000f000f
#define TYPE1_IMREG_IO_TERM_CTL_INIT_MSB        0x04030000

/* Device control register: Device Configuration Register */
#define TYPE1_IMREG_DEV_CFG_LSB_RSVD1_oft       0
#define TYPE1_IMREG_DEV_CFG_LSB_RSVD1_mask      0xff
#define TYPE1_IMREG_DEV_CFG_LSB_UCODE_ENA_oft   12
#define TYPE1_IMREG_DEV_CFG_LSB_UCODE_ENA_mask  0x01
#define TYPE1_IMREG_DEV_CFG_LSB_RSVD2_oft       13
#define TYPE1_IMREG_DEV_CFG_LSB_RSVD2_mask      0x3ff
#define TYPE1_IMREG_DEV_CFG_LSB_NPU1_PSE_oft    30
#define TYPE1_IMREG_DEV_CFG_LSB_NPU1_PSE_mask   0x01
#define TYPE1_IMREG_DEV_CFG_MSB_SW_RST_oft      0
#define TYPE1_IMREG_DEV_CFG_MSB_SW_RST_mask     0xffff

#define TYPE1_ST1_IMREG_DEV_CFG_LSB_RSVD1_VALUE 0x64
#define TYPE1_ST2_IMREG_DEV_CFG_LSB_RSVD1_VALUE 0xa4
#define TYPE1_ST2_IMREG_DEV_CFG_LSB_RSVD2_VALUE 1
#define TYPE1_IMREG_DEV_CFG_MSB_SW_RST_ON       0x0100

/* Clock control register: Clock Control Register */
#define TYPE1_IMREG_CLK_CTL_LSB_SYSTEM_SRC_oft  16
#define TYPE1_IMREG_CLK_CTL_LSB_SYSTEM_SRC_mask 0x07
#define TYPE1_IMREG_CLK_CTL_MSB_SRAM_SRC_oft    16
#define TYPE1_IMREG_CLK_CTL_MSB_SRAM_SRC_mask   0x07

#define TYPE1_IMREG_CLK_CTL_SRC_HCLK            0
#define TYPE1_IMREG_CLK_CTL_SRC_XCLK            1
#define TYPE1_IMREG_CLK_CTL_SRC_N0_K            2
#define TYPE1_IMREG_CLK_CTL_SRC_N0_K2           3
#define TYPE1_IMREG_CLK_CTL_SRC_N1_K            4
#define TYPE1_IMREG_CLK_CTL_SRC_N1_K2           5

/*
 * QDR address bus format (for subtype 1 and 2)
 * +-------+-------+-------+-----+
 * | 19:18 | 17:12 | 11:5  | 4:0 |
 * | IPCMD |  IP   | CNTXT | DW  |
 * +-------+-------+-------+-----+
 */
#define TYPE1_QDR_ADDR_CNTXT_oft              5
#define TYPE1_QDR_ADDR_CNTXT_mask             0x7f
#define TYPE1_QDR_ADDR_IP_oft                 12
#define TYPE1_QDR_ADDR_IP_mask                0x3f
#define TYPE1_QDR_ADDR_IPCMD_oft              18
#define TYPE1_QDR_ADDR_IPCMD_mask             0x03

/* Instruction pointer command (IPCMD) (for subtype 1 and 2) */
#define TYPE1_IPCMD_READ_RESULT               0
#define TYPE1_IPCMD_WRITE_IB                  1
#define TYPE1_IPCMD_EXEC_SECONDARY            2
#define TYPE1_IPCMD_EXEC_PRIMARY              3

/* Instruction pointer (IP) (for subtype 1 and 2), customerized by sdk */
#define TYPE1_IP_WRITE_IMA                    0x00
#define TYPE1_IP_READ_IMA                     0x01
#define TYPE1_IP_WRITE_DB_D72                 0x02
#define TYPE1_IP_WRITE_DB_D144                0x03
#define TYPE1_IP_WRITE_DB_M72                 0x04
#define TYPE1_IP_WRITE_DB_M144                0x05
#define TYPE1_IP_READ_DB_D72                  0x06
#define TYPE1_IP_READ_DB_M72                  0x07
#define TYPE1_IP_WRITE_DB_BMR                 0x08
#define TYPE1_IP_WRITE_DB_REG                 0x09
#define TYPE1_IP_READ_DB_REG                  0x0a
#define TYPE1_IP_SET_VALID                    0x0b
#define TYPE1_IP_CLEAR_VALID                  0x0c
#define TYPE1_IP_SEARCH_LPM                   0x0d
#define TYPE1_IP_SEARCH_EPM                   0x0e /* exact prefix match */
#define TYPE1_IP_SEARCH_ACL                   0x0f
#define TYPE1_IP_SEARCH_LPM_H1                0x10
#define TYPE1_IP_SEARCH_EPM_H1                0x11 /* exact prefix match */
#define TYPE1_IP_SEARCH_ACL_H1                0x12
#define TYPE1_IP_NOP                          0x13

/*
 * Opcode (for subtype 3)
 */
#define TYPE1_OPCODE_NOP               0x00
#define TYPE1_OPCODE_ENABLE_NEXT_DEV   0x01
#define TYPE1_OPCODE_SW_RESET          0x02
#define TYPE1_OPCODE_RD_REG            0x40
#define TYPE1_OPCODE_RD_DATA           0x41
#define TYPE1_OPCODE_RD_MASK           0x42
#define TYPE1_OPCODE_CMP_WORD(idx)     (0x80 + (idx))
#define TYPE1_OPCODE_WR_REG            0x93
#define TYPE1_OPCODE_DEL_RECORD        0x94
#define TYPE1_OPCODE_WR_ASDATA         0x05
#define TYPE1_OPCODE_WR_RECORD         0x96
#define TYPE1_OPCODE_SEARCH(ltr)       (0xc0 | (((ltr) & 7) << 3) | \
                                        (((ltr) & 0x08) >> 1) | \
                                        (((ltr) & 0x10) >> 3))


/* the longest instrction format defined by vendor (for subtype 1 and 2) */
#define TYPE1_INST_DATA_SIZE_MAX              10  /* 40 bytes, 10 uint32 */

/* number of entries per block */
#define TYPE1_ST1_ENTRIES_PER_BLK             (32 * 1024)
#define TYPE1_ST2_ENTRIES_PER_BLK             (16 * 1024)
#define TYPE1_ST3_ENTRIES_PER_BLK             (16 * 1024)

/* number of blocks per device */
#define TYPE1_ST1_BLKS_PER_DEV                8
#define TYPE1_ST2_BLKS_PER_DEV                16
#define TYPE1_ST3_BLKS_PER_DEV                16

/*
 * Definitions for configuration, these are customerized by sdk
 */
/* logical table register index for easyrider */
#define TYPE1_ER_LTBL_LPM                     1
#define TYPE1_ER_LTBL_EPM                     2
#define TYPE1_ER_LTBL_ACL                     3
#define TYPE1_ER_LTBL_LPM_H1                  5
#define TYPE1_ER_LTBL_EPM_H1                  6
#define TYPE1_ER_LTBL_ACL_H1                  7

/* logical table ID for easyrider */
#define TYPE1_ER_LTID_LPM                     1   /* LPM in ER, ER_H0 */
#define TYPE1_ER_LTID_ACL                     2   /* ACL in ER, ER_H0 */
#define TYPE1_ER_LTID_LPM_H1                  5   /* LPM in ER_H1 */
#define TYPE1_ER_LTID_ACL_H1                  6   /* ACL in ER_H1 */

/* block mask ID for easyrider */
#define TYPE1_ER_BMASK_EM                     0 /* for exact match */
#define TYPE1_ER_BMASK_LPM                    1 /* for LPM */

/*
 * logical table register index for triumph
 * These number needs to match with ESM hardware (LTR_LTID_usage_060807.xls)
 * even the order is the same as document for easy reading (maybe)
 */
#define TYPE1_TR_LTBL_ACL_NONE_FWD_L2         0
#define TYPE1_TR_LTBL_ACL_NONE_FWD_IP4        1
#define TYPE1_TR_LTBL_ACL_NONE_FWD_IP6U       2
#define TYPE1_TR_LTBL_ACL_NONE_FWD_IP6        3
#define TYPE1_TR_LTBL_ACL_L2_FWD_L2           4
#define TYPE1_TR_LTBL_ACL_IP4_FWD_NONE        7
#define TYPE1_TR_LTBL_ACL_IP4_FWD_IP4         8
#define TYPE1_TR_LTBL_ACL_IP6S_FWD_NONE       5
#define TYPE1_TR_LTBL_ACL_IP6S_FWD_IP6U       9
#define TYPE1_TR_LTBL_ACL_IP6F_FWD_NONE       6
#define TYPE1_TR_LTBL_ACL_IP6F_FWD_IP6U       10
#define TYPE1_TR_LTBL_ACL_IP6F_FWD_IP6        11
#define TYPE1_TR_LTBL_ACL_L2C_FWD_NONE        12
#define TYPE1_TR_LTBL_ACL_IP4C_FWD_NONE       16
#define TYPE1_TR_LTBL_ACL_IP6C_FWD_NONE       18
#define TYPE1_TR_LTBL_ACL_L2IP4_FWD_NONE      21
#define TYPE1_TR_LTBL_ACL_L2IP6_FWD_NONE      22
#define TYPE1_TR_LTBL_ACL_NONE_FWD_IP6U_K4    23 /* for subtype 4 @ 500 Mhz */
#define TYPE1_TR_LTBL_ACL_NONE_FWD_IP6_K4     24 /* for subtype 4 @ 500 Mhz */
#define TYPE1_TR_LTBL_ACL_NONE_FWD_L2_K2      23 /* for subtype 4 @ 350 Mhz */
#define TYPE1_TR_LTBL_ACL_NONE_FWD_IP4_K2     24 /* for subtype 4 @ 350 Mhz */
#define TYPE1_TR_LTBL_FWD_IP4_FWD_L2          25 /* for subtype 4 @ 350 Mhz */
#define TYPE1_TR_LTBL_FWD_IP6U_FWD_L2         26 /* for subtype 4 @ 350 Mhz */
#define TYPE1_TR_LTBL_DEV0_TBL144_DEV0_TBL72  30 /* for software test */
#define TYPE1_TR_LTBL_DEV1_TBL144_DEV1_TBL72  31 /* for software test */

/*
 * logical table ID for triumph
 * These are numbers used by AV, s/w can change these to different order
 */
#define TYPE1_TR_LTID_FWD_L2                  14
#define TYPE1_TR_LTID_FWD_IP4                 2
#define TYPE1_TR_LTID_FWD_IP6U                3   /* ipv6 upper 64-bit */
#define TYPE1_TR_LTID_FWD_IP6                 4
#define TYPE1_TR_LTID_ACL_L2                  5
#define TYPE1_TR_LTID_ACL_IP4                 6
#define TYPE1_TR_LTID_ACL_IP6S                7   /* ipv6 64-bit short mode */
#define TYPE1_TR_LTID_ACL_IP6F                8   /* ipv6 64-bit full mode */
#define TYPE1_TR_LTID_ACL_L2C                 9   /* l2 compact (144-bit) */
#define TYPE1_TR_LTID_ACL_IP4C                10  /* ipv4 compact (144-bit) */
#define TYPE1_TR_LTID_ACL_IP6C                11  /* ipv6 compact (144-bit) */
#define TYPE1_TR_LTID_ACL_L2IP4               12
#define TYPE1_TR_LTID_ACL_L2IP6               13
#define TYPE1_TR_LTID_UNUSED                  15  /* for all unused blocks */
/* following test blocks should not co-exist with the normal blocks */
#define TYPE1_TR_LTID_DEV0_TBL72              2   /* for software test */
#define TYPE1_TR_LTID_DEV0_TBL144             3   /* for software test */
#define TYPE1_TR_LTID_DEV1_TBL72              4   /* for software test */
#define TYPE1_TR_LTID_DEV1_TBL144             5   /* for software test */

/* block mask ID for triumph */
#define TYPE1_TR_BMASK                        0

/* Triumph3 tcam definitions: */

/*
 * Opcodes (for tr3/nl11k)
 */
#define TR3_OPCODE_NOP               0x000
#define TR3_OPCODE_WR_REG_DB         0x001
#define TR3_OPCODE_RD_REG_DBX        0x002
#define TR3_OPCODE_RD_DBY            0x003
#define TR3_OPCODE_CB_WR_CMP1(ltr)   (0x40 | ((ltr) & 0x3f))
#define TR3_OPCODE_CB_WR_CMP2(ltr)   (0x80 | ((ltr) & 0x3f))
#define TR3_OPCODE_CB_WR	     0x100

/* given 4 input bytes make a 32 bit word out of it */
#define MAKE_WORD(b1, b2, b3, b4) (((b1) << 24) | ((b2) << 16) | ((b3) << 8) \
				  | (b4))

/* given a 32bit word, get the byte specified */
#define GET_BYTE(word, byte) (((word) >> (byte*8)) & 0xff)
#define SET_VBIT 0x40
#define SET_XY_WRITE 0x80

/* entries per tr3 nl11k block */
#define TR3_ENTRIES_PER_BLK             (4 * 1024)
#define TR3_BLKS_PER_DEV_64             64
#define TR3_BLKS_PER_DEV_128            128

/* Database register accessing: address[24:23] is device ID */
#define TR3_REG_BASE(dev)            (((dev) & 0x03) << 23)

/* Database register: Database Identification Register */
#define TR3_REG_DEVID_ADDR(dev)      (0x0 + TR3_REG_BASE(dev))

#define TR3_REG_DEVID_DB_SIZE_oft      6
#define TR3_REG_DEVID_DB_SIZE_mask     0x3

/* Device Configuration Register */
#define TR3_REG_DEVCFG_ADDR(dev)      (0x1 + TR3_REG_BASE(dev))

/* Database register: Block Configuration Register */
#define TR3_DBREG_BCFG_ADDR(dev,blk)   (0x1000 + (blk)*0x20 + TR3_REG_BASE(dev))

/* Database entry address */
#define TR3_DBASE_ADDR(dev,blk)  ((blk) + TR3_REG_BASE(dev))

/* Database entry address */
#define TR3_DBASE_ADDR(dev,blk)  ((blk) + TR3_REG_BASE(dev))

#define TR3_DBREG_BCFG_WIDTH_oft            1
#define TR3_DBREG_BCFG_WIDTH_mask           0x7
#define TR3_DBREG_BCFG_ENABLE_oft           0
#define TR3_DBREG_BCFG_ENABLE_mask          0x1

#define TR3_DBREG_BLK_ENABLE_oft            0
#define TR3_DBREG_BLK_ENABLE_mask           1
#define TR3_DBREG_BMR_SELECT0_oft           0
#define TR3_DBREG_BMR_SELECT0_mask          0x7
#define TR3_DBREG_BMR_SELECT1_oft           4
#define TR3_DBREG_BMR_SELECT1_mask          0x7
#define TR3_DBREG_BMR_SELECT2_oft           8
#define TR3_DBREG_BMR_SELECT2_mask          0x7
#define TR3_DBREG_BMR_SELECT3_oft           12
#define TR3_DBREG_BMR_SELECT3_mask          0x7
#define TR3_DBREG_NUM_SEARCH_RESULTS_oft    24
#define TR3_DBREG_NUM_SEARCH_RESULTS_mask   0x3
#define TR3_DBREG_KCR_START_BYTE0_oft       0
#define TR3_DBREG_KCR_START_BYTE0_mask      0x7f
#define TR3_DBREG_KCR_NUM_BYTE0_oft         7
#define TR3_DBREG_KCR_NUM_BYTE0_mask        0xf
#define TR3_DBREG_KCR_START_BYTE1_oft       11
#define TR3_DBREG_KCR_START_BYTE1_mask      0x7f
#define TR3_DBREG_KCR_NUM_BYTE1_oft         18
#define TR3_DBREG_KCR_NUM_BYTE1_mask        0xf
#define TR3_DBREG_KCR_START_BYTE2_oft       22
#define TR3_DBREG_KCR_START_BYTE2_mask      0x7f
#define TR3_DBREG_KCR_NUM_BYTE2_oft         29
#define TR3_DBREG_KCR_NUM_BYTE2_mask        0x7
#define TR3_DBREG_KCR_START_BYTE3_oft       1
#define TR3_DBREG_KCR_START_BYTE3_mask      0x7f
#define TR3_DBREG_KCR_NUM_BYTE3_oft         8
#define TR3_DBREG_KCR_NUM_BYTE3_mask        0xf
#define TR3_DBREG_KCR_START_BYTE4_oft       12
#define TR3_DBREG_KCR_START_BYTE4_mask      0x7f
#define TR3_DBREG_KCR_NUM_BYTE4_oft         19
#define TR3_DBREG_KCR_NUM_BYTE4_mask        0xf
#define TR3_DBREG_KCR_START_BYTE5_oft       0
#define TR3_DBREG_KCR_START_BYTE5_mask      0x7f
#define TR3_DBREG_KCR_NUM_BYTE5_oft         7
#define TR3_DBREG_KCR_NUM_BYTE5_mask        0xf
#define TR3_DBREG_KCR_START_BYTE6_oft       11
#define TR3_DBREG_KCR_START_BYTE6_mask      0x7f
#define TR3_DBREG_KCR_NUM_BYTE6_oft         18
#define TR3_DBREG_KCR_NUM_BYTE6_mask        0xf
#define TR3_DBREG_KCR_START_BYTE7_oft       22
#define TR3_DBREG_KCR_START_BYTE7_mask      0x7f
#define TR3_DBREG_KCR_NUM_BYTE7_oft         29
#define TR3_DBREG_KCR_NUM_BYTE7_mask        0x7
#define TR3_DBREG_KCR_START_BYTE8_oft       1
#define TR3_DBREG_KCR_START_BYTE8_mask      0x7f
#define TR3_DBREG_KCR_NUM_BYTE8_oft         8
#define TR3_DBREG_KCR_NUM_BYTE8_mask        0xf
#define TR3_DBREG_KCR_START_BYTE9_oft       12
#define TR3_DBREG_KCR_START_BYTE9_mask      0x7f
#define TR3_DBREG_KCR_NUM_BYTE9_oft         19
#define TR3_DBREG_KCR_NUM_BYTE9_mask        0xf
#define TR3_WRCMP_RSP_SMF_oft               30
#define TR3_WRCMP_RSP_SMF_mask              1
#define TR3_WRCMP_RSP_RESULT_oft           0
#define TR3_WRCMP_RSP_RESULT_mask          0x3fffff

#ifdef later
#define TR3_DBREG_BCFG_WIDTH_80             0
#define TR3_DBREG_BCFG_WIDTH_160            1
#define TR3_DBREG_BCFG_WIDTH_320            2
#define TR3_DBREG_BCFG_WIDTH_640            3
#endif

/* Database register: Block [0-n] Mask Register [0-4] */
#define TR3_DBREG_BMASK_ADDR(dev,blk,mask) (0x1001 + (mask) + (blk) * 0x20 + \
                                            TR3_REG_BASE(dev))

/* Database register: Logical Table Register */
#define TR3_DBREG_BLK_SEL0_ADDR(dev,i)      (0x4000 + (i)*0x20 + \
                                            TR3_REG_BASE(dev))
#define TR3_DBREG_SBLK_KPU_SEL_ADDR(dev,i)   (0x4002 + (i)*0x20 + \
                                               TR3_REG_BASE(dev))
#define TR3_DBREG_LTR_MISC_REG_ADDR(dev,i)   (0x4009 + (i)*0x20 + \
                                               TR3_REG_BASE(dev))
#define TR3_DBREG_LTR_KCR_ADDR(dev,kpu_num,i)   (0x400B + (kpu_num)*2 + \
                                                (i)*0x20 + TR3_REG_BASE(dev))
#define TR3_DBREG_LTR_RANGE_INS_ADDR(dev, range_num,i)   (0x4007 + (range_num) \
                                            + (i)*0x20 + TR3_REG_BASE(dev))
#define TR3_DBREG_LTR_PARALLEL_SRCH_REG_ADDR(dev,par_srch,i)   \
                          (0x4003 + (par_srch) + (i)*0x20 + TR3_REG_BASE(dev))

/* bit_num specifies the MSB of the 80bit register section being accessed */
#define TR3_DBREG_BMR(dev, blk_num, reg_num, bit_num) \
     (0x1001 + (blk_num)*0x20 + 4*(reg_num) + (bit_num)/80 + TR3_REG_BASE(dev))

#define MAKE_BSR_WORDS(bsr, part, entries_per_blk) \
    do {\
        int i, num_blks, blk_base;\
        num_blks = ((partitions[(part)].num_entries <<\
                 partitions[(part)].tcam_width_shift) +\
                    (entries_per_blk) - 1) / (entries_per_blk);\
            blk_base = partitions[(part)].tcam_base / (entries_per_blk);\
			for (i = blk_base; i < blk_base + num_blks; i++) {\
				(bsr)[i/32] |= (1 << (i - 32*(i/32)));\
			}\
    } while (0)

#ifdef raw_logic
            num_blks = ((partitions[TCAM_PARTITION_FWD_L2].num_entries <<
                         partitions[part].tcam_width_shift) +
                        entries_per_blk - 1) / entries_per_blk;
            blk_base = partitions[part].tcam_base /entries_per_blk;
			for (i = blk_base; i < blk_base + num_blks; i++) {
				bsr[i/32] |= (1 << (i - 32*(i/32)));
			}
#endif

#define MAKE_SUPER_BLOCK_KPU_SELECT(sblk_kpu, part, entries_per_blk, kpu)\
do {\
    int i, num_blks, blk_base;\
    num_blks = ((partitions[(part)].num_entries <<\
                 partitions[(part)].tcam_width_shift) +\
                (entries_per_blk) - 1) / (entries_per_blk);\
    blk_base = partitions[(part)].tcam_base / (entries_per_blk);\
    /* blk_base is always a multiple of 4 */\
    for (i = blk_base/4; i < (blk_base + num_blks + 3)/4; i++) {\
        (sblk_kpu)[(i*2)/32] |= (((kpu) & 0x3) << (i*2 - 32*(i*2/32)));\
    }\
} while (0)

#define MAKE_PARALLEL_SEARCH_REGISTER(parallel_srch, part, entries_per_blk,\
                                      port)\
do {\
    int i, num_blks, blk_base;\
    num_blks = ((partitions[(part)].num_entries <<\
                 partitions[(part)].tcam_width_shift) +\
                (entries_per_blk) - 1) / (entries_per_blk);\
    blk_base = partitions[(part)].tcam_base / (entries_per_blk);\
    for (i = blk_base; i < blk_base + num_blks; i++) {\
        (parallel_srch)[(i*2)/32] |= (((port) & 0x3) << (i*2 - 32*(i*2/32)));\
    }\
} while (0)

#define WRITE_PARALLEL_SEARCH_REGISTERS(unit, dev, ltr, reg0_1, reg0_0, \
                              reg1_1, reg1_0, reg2_1, reg2_0, reg3_1, reg3_0)\
do {\
    int addr;\
    /* write registers 0-3 */\
    addr = TR3_DBREG_LTR_PARALLEL_SRCH_REG_ADDR((dev), 0, (ltr));\
    SOC_IF_ERROR_RETURN(tr3_write_reg((unit), addr, 0, (reg0_1), (reg0_0)));\
    addr += 1;\
    SOC_IF_ERROR_RETURN(tr3_write_reg((unit), addr, 0, (reg1_1), (reg1_0)));\
    addr += 1;\
    SOC_IF_ERROR_RETURN(tr3_write_reg((unit), addr, 0, (reg2_1), (reg2_0)));\
    addr += 1;\
    SOC_IF_ERROR_RETURN(tr3_write_reg((unit), addr, 0, (reg3_1), (reg3_0)));\
} while (0)

#define SET_BLOCK_SELECT_REG(unit, dev, ltr, bsr3, bsr2, bsr1, bsr0) \
do {\
    int addr;\
    /* block select reg 0 */\
    addr = TR3_DBREG_BLK_SEL0_ADDR((dev), (ltr));\
    SOC_IF_ERROR_RETURN(tr3_write_reg((unit), addr, 0, (bsr1), (bsr0)));\
\
    /* block select reg 1 */\
    addr += 1;\
    SOC_IF_ERROR_RETURN(tr3_write_reg((unit), addr, 0, (bsr3), (bsr2)));\
} while (0)


#define NUM_PARTITIONS_PER_SEARCH       4
#define NUM_TR3_TCAMS                   2

/* TR3 LTRs */
#define TR3_LTR_FWD_L2_DA                 0
#define TR3_LTR_FWD_L2_SA                 1
#define TR3_LTR_V4_ONLY_DIP               2
#define TR3_LTR_V4_ONLY_SIP               3
#define TR3_LTR_V6_64_ONLY_DIP            4
#define TR3_LTR_V6_64_ONLY_SIP            5
#define TR3_LTR_V6_128_ONLY_DIP           6
#define TR3_LTR_V6_128_ONLY_SIP           7
#define TR3_LTR_V4_SIP_SA                 8
#define TR3_LTR_V6_64_SIP_SA              9
#define TR3_LTR_V6_128_SIP_SA             10
#define TR3_LTR_AFLSS0_V6_128_DIP         16
#define TR3_LTR_AFLSS0_ACL_V6_128_SIP_SA  17
#define TR3_LTR_AFLSS1_V4_DIP             18
#define TR3_LTR_AFLSS1_ACL_V4_SIP_SA      19
#define TR3_LTR_AFLSS2_V6_64_DIP          20
#define TR3_LTR_AFLSS2_ACL_V6_64_SIP_SA   21
#define TR3_LTR_AFLSS3_ACL_SA             22
#define TR3_LTR_AFLSS0A_V6_128_DIP        23
#define TR3_LTR_AFLSS0A_ACL_V6_128_SIP_SA 24
#define TR3_LTR_AFLSS1A_V4_DIP            25
#define TR3_LTR_AFLSS1A_ACL_V4_SIP_SA     26
#define TR3_LTR_AFLSS2A_V6_64_DIP         27
#define TR3_LTR_AFLSS2A_ACL_V6_64_SIP_SA  28
#define TR3_LTR_AFLSS1MP_V4_DIP_SIP_SA    29
#define TR3_LTR_FLSS0_V6_128_SIP          40
#define TR3_LTR_FLSS0_SA                  41
#define TR3_LTR_RAW_L2                    51
#define TR3_LTR_CP_V4_UCAST               52
#define TR3_LTR_CP_V4_UCAST_WIDE          53
#define TR3_LTR_CP_V4_DEFIP               54
#define TR3_LTR_CP_V4_ALL                 55
#define TR3_LTR_CP_V6_64_DEFIP            56
#define TR3_LTR_CP_V6_128_UCAST           57
#define TR3_LTR_CP_V6_128_UCAST_WIDE      58
#define TR3_LTR_CP_V6_128_DEFIP           59
#define TR3_LTR_CP_V6_128_ALL             60
#define TR3_LTR_CP_L2                     61
#define TR3_LTR_CP_L2_WIDE                62
#define TR3_LTR_CP_L2_ALL                 63

/* TR2 ACL LTRs , mutually exclusive with the ACL LTRs above */
#define TR3_LTR_TR2_ACL_L2C_DA                  30
#define TR3_LTR_TR2_ACL_L2C_SA_ACL              31
#define TR3_LTR_TR2_ACL_L2_DA                   32
#define TR3_LTR_TR2_ACL_L2_SA_ACL               33
#define TR3_LTR_TR2_ACL_IP4C_DIP                34
#define TR3_LTR_TR2_ACL_IP4C_ACL_SIP_SA         35
#define TR3_LTR_TR2_ACL_IP4_DIP                 36
#define TR3_LTR_TR2_ACL_IP4_ACL_SIP_SA          37
#define TR3_LTR_TR2_ACL_L2IP4_ACL_SIP_SA        38
#define TR3_LTR_TR2_ACL_IP6C_DIPV6_64           39
#define TR3_LTR_TR2_ACL_IP6C_ACL_SIPV6_64_SA    42
#define TR3_LTR_TR2_ACL_IP6S_DIPV6              43
#define TR3_LTR_TR2_ACL_IP6S_ACL_SIPV6_SA       44
#define TR3_LTR_TR2_ACL_IP6F_ACL_SIPV6_SA       45
#define TR3_LTR_TR2_ACL_L2IP6_ACL_SIPV6_SA      46

/* LTR library */
#define TR3_LTR_LIB_L2_FWD_DA               0
#define TR3_LTR_LIB_L2_FWD_SA               1
#define TR3_LTR_LIB_L2_FWD_DUP              2
#define TR3_LTR_LIB_V4_ONLY_DIP             3
#define TR3_LTR_LIB_V4_ONLY_SIP             4
#define TR3_LTR_LIB_V6_64_ONLY_DIP          5
#define TR3_LTR_LIB_V6_64_ONLY_SIP          6
#define TR3_LTR_LIB_V6_128_ONLY_DIP         7
#define TR3_LTR_LIB_V6_128_ONLY_SIP         8
#define TR3_LTR_LIB_AFLSS0_DIP              9
#define TR3_LTR_LIB_V4_SIP_SA             10
#define TR3_LTR_LIB_V4_DUP                11
#define TR3_LTR_LIB_V6_128_SIP_SA         12
#define TR3_LTR_LIB_V6_128_DUP            13
#define TR3_LTR_LIB_V6_64_SIP_SA          14
#define TR3_LTR_LIB_V6_64_DUP             15
#define TR3_LTR_LIB_AFLSS0_ACL_SIP_SA       16
#define TR3_LTR_LIB_AFLSS1_DIP              17
#define TR3_LTR_LIB_AFLSS1_ACL_SIP_SA       18
#define TR3_LTR_LIB_AFLSS2_DIP              19
#define TR3_LTR_LIB_AFLSS2_ACL_SIP_SA       20
#define TR3_LTR_LIB_AFLSS3_ACL_SA           21
#define TR3_LTR_LIB_AFLSS0A_DIP             22
#define TR3_LTR_LIB_AFLSS0A_ACL_SIP_SA      23
#define TR3_LTR_LIB_AFLSS1A_DIP             24
#define TR3_LTR_LIB_AFLSS1A_ACL_SIP_SA      25
#define TR3_LTR_LIB_AFLSS2A_DIP             26
#define TR3_LTR_LIB_AFLSS2A_ACL_SIP_SA      27
#define TR3_LTR_LIB_AFLSS1AMP               28
#define TR3_LTR_LIB_FLSS0_SIP               29
#define TR3_LTR_LIB_FLSS0_SA                30

/* TR2 ACLs */
#define TR3_LTR_LIB_ACL_L2C_DA                  31
#define TR3_LTR_LIB_ACL_L2C_SA_ACL              32
#define TR3_LTR_LIB_ACL_L2_DA                   33
#define TR3_LTR_LIB_ACL_L2_SA_ACL               34
#define TR3_LTR_LIB_ACL_IP4C_DIP                35
#define TR3_LTR_LIB_ACL_IP4C_ACL_SIP_SA         36
#define TR3_LTR_LIB_ACL_IP4_DIP                 37
#define TR3_LTR_LIB_ACL_IP4_ACL_SIP_SA          38
#define TR3_LTR_LIB_ACL_L2IP4_ACL_SIP_SA        39
#define TR3_LTR_LIB_ACL_IP6C_DIP                40
#define TR3_LTR_LIB_ACL_IP6C_ACL_SIP_SA         41
#define TR3_LTR_LIB_ACL_IP6S_DIP                42
#define TR3_LTR_LIB_ACL_IP6S_ACL_SIP_SA         43
#define TR3_LTR_LIB_ACL_IP6F_ACL_SIP_SA         44
#define TR3_LTR_LIB_ACL_L2IP6_ACL_SIP_SA        45
#define TR3_LTR_LIB_ACL_IP4C_ONLY               46
#define TR3_LTR_LIB_ACL_IP4C_L2_SA              47
#define TR3_LTR_LIB_ACL_IP4C_L2_ACL_DA          48
#define TR3_LTR_LIB_ACL_IP4_ONLY                49
#define TR3_LTR_LIB_ACL_IP4_L2_DA               50
#define TR3_LTR_LIB_ACL_IP4_L2_ACL_SA           51
#define TR3_LTR_LIB_ACL_L2IP4_ONLY              52
#define TR3_LTR_LIB_ACL_L2IP4_L2_DA             53
#define TR3_LTR_LIB_ACL_L2IP4_L2_ACL_SA         54
#define TR3_LTR_LIB_ACL_IP6C_ONLY               55
#define TR3_LTR_LIB_ACL_IP6C_L2_DA              56
#define TR3_LTR_LIB_ACL_IP6C_L2_ACL_SA          57
#define TR3_LTR_LIB_ACL_IP6S_ONLY               58
#define TR3_LTR_LIB_ACL_IP6S_L2_DA              59
#define TR3_LTR_LIB_ACL_IP6S_L2_ACL_SA          60
#define TR3_LTR_LIB_ACL_IP6F_ONLY               61
#define TR3_LTR_LIB_ACL_IP6F_L2_DA              62
#define TR3_LTR_LIB_ACL_IP6F_L2_ACL_SA          63
#define TR3_LTR_LIB_ACL_L2IP6_ONLY              64
#define TR3_LTR_LIB_ACL_L2IP6_L2_DA             65
#define TR3_LTR_LIB_ACL_L2IP6_L2_ACL_SA         66
#define TR3_LTR_LIB_ACL_L2C_DIP                 67
#define TR3_LTR_LIB_ACL_L2C_SA_ACL_SIP          68
#define TR3_LTR_LIB_ACL_L2C_DIPV6_64            69
#define TR3_LTR_LIB_ACL_L2C_SA_ACL_SIPV6_64     70
#define TR3_LTR_LIB_ACL_L2C_DIPV6_128           71
#define TR3_LTR_LIB_ACL_L2C_SA_ACL_SIPV6_128    72
#define TR3_LTR_LIB_ACL_L2_DIPV6_64             73
#define TR3_LTR_LIB_ACL_L2_SA_ACL_SIPV6_64      74
#define TR3_LTR_LIB_ACL_L2C_ONLY                75
#define TR3_LTR_LIB_ACL_L2_ONLY                 76
#define TR3_LTR_LIB_ACL_IP6S_DIPV6_64           77
#define TR3_LTR_LIB_ACL_IP6S_ACL_SIPV6_64_SA    78
#define TR3_LTR_LIB_ACL_IP6C_DIPV6_128          79
#define TR3_LTR_LIB_ACL_IP6C_ACL_SIPV6_128_SA   80
#define TR3_LTR_LIB_ACL_IP6F_ACL_SIPV6_64_SA    81
#define TR3_LTR_LIB_ACL_L2_SA_ACL_N7K           82
#define TR3_LTR_LIB_ACL_L2_ONLY_N7K             83

extern int soc_tr3_build_ltr(int unit, int ltr_lib_idx);
extern int soc_tr3_tcam_fast_init(int unit);
extern int soc_tr3_tcam_l2_fast_init(int unit, int start_dev_idx, 
        int num_tcams, void *l2_entry);
       
