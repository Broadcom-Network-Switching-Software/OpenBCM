/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cmic.h
 * Purpose:     Maps out structures used for CMIC operations and
 *              exports routines and constants.
 */

#ifndef _SOC_CMIC_H
#define _SOC_CMIC_H

#include <soc/defs.h>
#include <soc/schanmsg.h>
#include <soc/register.h>

#define CMIC_OFFSET_MIN                 0

#define CCMDMA_TIMEOUT          (5   * SECOND_USEC)
#define CCMDMA_TIMEOUT_QT       (120 * SECOND_USEC)

/*
 * S-Channel Message Buffer Registers (0x00 -> 0x4c, or 0x800 -> 0x854).
 * Block where S-Channel messages are written to CMIC.
 */
#define CMIC_SCHAN_MESSAGE(unit, word)  \
    (soc_feature(unit, soc_feature_schmsg_alias) ? \
    (0x00000800 + 4 * (word)) : (0x00000000 + 4 * (word)))

#define CMIC_SCHAN_MESSAGE_WORDS(unit)  \
    (soc_feature(unit, soc_feature_schmsg_alias) ? 22 : 20)

#define CMIC_SCHAN_MESSAGE_WORDS_MAX    22

/* S-Channel Control Register */
#define CMIC_SCHAN_CTRL                 0x00000050

/* CMIC offset in AXI address space*/
#define AXI_CMIC_OFFSET 0x3200000

/*
 * SCHAN_CONTROL: control bits
 *
 *  SC_xxx_SET and SC_xxx_CLR can be WRITTEN to CMIC_SCHAN_CTRL.
 *  SC_xxx_TST can be masked against values READ from CMIC_SCHAN_CTRL.
 */
#define SC_MSG_START_SET                (0x80|0)
#define SC_MSG_START_CLR                (0x00|0)
#define SC_MSG_START_TST                0x00000001

#define SC_MSG_DONE_SET                 (0x80|1)
#define SC_MSG_DONE_CLR                 (0x00|1)
#define SC_MSG_DONE_TST                 0x00000002

#define SC_ARL_RDY_SET(msg)             (0x80|((msg)+2))   /* 0 <= msg <= 3 */
#define SC_ARL_RDY_CLR(msg)             (0x00|((msg)+2))
#define SC_ARL_RDY_TST(msg)             (0x00000004 << (msg))
#define SC_ARL_RDY_TST_ANY              (0x0000003c)

#define SC_ARL_DMA_EN_SET               (0x80|6)
#define SC_ARL_DMA_EN_CLR               (0x00|6)
#define SC_ARL_DMA_EN_TST               0x00000040

#define SC_ARL_DMA_DONE_SET             (0x80|7)
#define SC_ARL_DMA_DONE_CLR             (0x00|7)
#define SC_ARL_DMA_DONE_TST             0x00000080

#define SC_LINK_STAT_MSG_SET            (0x80|8)
#define SC_LINK_STAT_MSG_CLR            (0x00|8)
#define SC_LINK_STAT_MSG_TST            0x00000100

#define SC_PCI_FATAL_ERR_SET            (0x80|9)
#define SC_PCI_FATAL_ERR_CLR            (0x00|9)
#define SC_PCI_FATAL_ERR_TST            0x00000200

#define SC_PCI_PARITY_ERR_SET           (0x80|10)
#define SC_PCI_PARITY_ERR_CLR           (0x00|10)
#define SC_PCI_PARITY_ERR_TST           0x00000400

#define SC_ARL_MSG_RCV_OFF_SET          (0x80|11)
#define SC_ARL_MSG_RCV_OFF_CLR          (0x00|11)
#define SC_ARL_MSG_RCV_OFF_TST          0x00000800

#define SC_ARL_MSG_DROPPED_SET          (0x80|12)
#define SC_ARL_MSG_DROPPED_CLR          (0x00|12)
#define SC_ARL_MSG_DROPPED_TST          0x00001000

#define SC_ARL_DMA_XFER_DONE_SET        (0x80|13)
#define SC_ARL_DMA_XFER_DONE_CLR        (0x00|13)
#define SC_ARL_DMA_XFER_DONE_TST        0x00002000

#define SC_MIIM_SCAN_BUSY_TST           0x00004000

#define SC_GBP_OK_SET                   (0x80|15)
#define SC_GBP_OK_CLR                   (0x00|15)
#define SC_GBP_OK_TST                   0x00008000



#define SC_MIIM_RD_START_SET            (0x80|16)
#define SC_MIIM_RD_START_CLR            (0x00|16)
#define SC_MIIM_RD_START_TST            0x00010000

#define SC_MIIM_WR_START_SET            (0x80|17)
#define SC_MIIM_WR_START_CLR            (0x00|17)
#define SC_MIIM_WR_START_TST            0x00020000

#define SC_MIIM_OP_DONE_SET             (0x80|18)
#define SC_MIIM_OP_DONE_CLR             (0x00|18)
#define SC_MIIM_OP_DONE_TST             0x00040000

#define SC_MIIM_LINK_SCAN_EN_SET        (0x80|19)
#define SC_MIIM_LINK_SCAN_EN_CLR        (0x00|19)
#define SC_MIIM_LINK_SCAN_EN_TST        0x00080000

#define SC_MSG_SER_CHECK_FAIL_SET       (0x80|20)
#define SC_MSG_SER_CHECK_FAIL_CLR       (0x00|20)
#define SC_MSG_SER_CHECK_FAIL_TST       0x00100000

#define SC_MSG_NAK_SET                  (0x80|21)
#define SC_MSG_NAK_CLR                  (0x00|21)
#define SC_MSG_NAK_TST                  0x00200000

#define SC_MSG_TIMEOUT_SET              (0x80|22)
#define SC_MSG_TIMEOUT_CLR              (0x00|22)
#define SC_MSG_TIMEOUT_TST              0x00400000


/* ARL DMA Address Register */
#define CMIC_ARL_DMA_ADDR               0x00000054      /* 32-bit address */

/* ARL DMA Packet count Register */
#define CMIC_ARL_DMA_CNT                0x00000058      /* 16-bit counter */
#define CMIC_ARL_DMA_CNT_MAX            0x0000FFFF      /* max value      */

/*
 * S-Channel Error Status Register.
 *
 * Use the SCERR_XXX macros to extract fields.  Fields are valid only if
 * VALID bit is on.  VALID bit is cleared on register read, except on
 * Lynx where the bit must be written to 0 to clear it (clear on read is
 * very bad when PCI burst reads are in use).
 *
 *     NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE:
 *
 * These values are not valid for Tucana, and probably not
 * for Draco either.  Use the regsfile defines.
 */
#define CMIC_SCHAN_ERR                  0x0000005C

#define SCERR_VALID(reg)                ((reg) >> 31 & 0x01)
#define SCERR_DST_PORT(reg)             ((reg) >> 23 & 0xff)
#define SCERR_SRC_PORT(reg)             ((reg) >> 15 & 0xff)
#define SCERR_OP_CODE(reg)              ((reg) >>  8 & 0x7f)
#define SCERR_ERR_CODE(reg)             ((reg) >>  0 & 0xff)

/*
 * COS Port Status
 *
 * There is one Active Port register per class of service.
 * Each register contains a port bitmap of active ports (see PBMP_*).
 * There are 4 classes of service (0 <= cos <= 3), with room for 8 more.
 * Not applicable on Lynx, reads as 0xffffffff.
 */

#define CMIC_COS_ENABLE(cos)            (0x00000060 + 4 * (cos))

/*
 * ARL Message Buffers. These messages are in the format schan_msg_t:
 *  If msg.header.opcode==ARL_INSERT_CMD_MSG, use msg.arlins union member.
 *  If msg.header.opcode==ARL_DELETE_CMD_MSG, use msg.arldel union member.
 * There are four buffers (0 <= num <= 3) and each is four words in size.
 */

#define NUM_ARL_MBUF                    4

#define CMIC_ARL_MBUF(num)              (0x00000080 + 0x20 * (num))

/* DMA Control Register */
#define CMIC_DMA_CTRL                   0x00000100

#define DC_SOC_TO_MEM(ch)               (0x00 << (8 * (ch))) /* 0 <= ch <= 3 */
#define DC_MEM_TO_SOC(ch)               (0x01 << (8 * (ch)))
#define DC_DIRECTION_MASK(ch)           (0x01 << (8 * (ch)))

#define DC_MOD_BITMAP(ch)               (0x00 << (8 * (ch))) /* 0 <= ch <= 3 */
#define DC_NO_MOD_BITMAP(ch)            (0x02 << (8 * (ch)))
#define DC_MOD_BITMAP_MASK(ch)          (0x02 << (8 * (ch)))

#define DC_NO_ABORT_DMA(ch)             (0x00 << (8 * (ch))) /* 0 <= ch <= 3 */
#define DC_ABORT_DMA(ch)                (0x04 << (8 * (ch)))
#define DC_ABORT_DMA_MASK(ch)           (0x04 << (8 * (ch)))

#define DC_INTR_ON_PKT(ch)              (0x00 << (8 * (ch))) /* 0 <= ch <= 3 */
#define DC_INTR_ON_DESC(ch)             (0x08 << (8 * (ch)))
#define DC_INTR_ON_MASK(ch)             (0x08 << (8 * (ch)))

#define DC_NO_DROP_TX(ch)               (0x00 << (8 * (ch))) /* 0 <= ch <= 3 */
#define DC_DROP_TX(ch)                  (0x10 << (8 * (ch)))
#define DC_DROP_TX_MASK(ch)             (0x10 << (8 * (ch)))
#define DC_DROP_TX_ALL                  0x10101010

#define DC_CHAN_MASK(ch)                (0xff << (8 * (ch)))

/* DMA Status Register */
#define CMIC_DMA_STAT                   0x00000104

/*
 * DMA_STAT: control bits
 *
 *  xxx_SET and xxx_CLR can be WRITTEN to CMIC_DMA_STAT
 *  xxx_TST can be masked against values read from CMIC_DMA_STAT.
 *  Argument required: 0 <= ch <= 3
 */
#define DS_DMA_EN_SET(ch)               (0x80|(ch))
#define DS_DMA_EN_CLR(ch)               (0x00|(ch))
#define DS_DMA_EN_TST(ch)               (0x00000001 << (ch))

#define DS_CHAIN_DONE_SET(ch)           (0x80|(4+(ch)))
#define DS_CHAIN_DONE_CLR(ch)           (0x00|(4+(ch)))
#define DS_CHAIN_DONE_TST(ch)           (0x00000010 << (ch))

#define DS_DESC_DONE_SET(ch)            (0x80|(8+(ch)))
#define DS_DESC_DONE_CLR(ch)            (0x00|(8+(ch)))
#define DS_DESC_DONE_TST(ch)            (0x00000100 << (ch))

#define DS_DMA_RESET                    0x1000
#define _DS_DMA_RESET                   DS_DMA_RESET

#define DS_STAT_DMA_DONE_SET            (0x80|13)
#define DS_STAT_DMA_DONE_CLR            (0x00|13)
#define DS_STAT_DMA_DONE_TST            0x2000

#define DS_STAT_DMA_ITER_DONE_SET       (0x80|14)
#define DS_STAT_DMA_ITER_DONE_CLR       (0x00|14)
#define DS_STAT_DMA_ITER_DONE_TST       0x4000

#define DS_STAT_DMA_ACTIVE              0x20000
#define DS_STAT_DMA_ERROR_SET           (0x80|16)
#define DS_STAT_DMA_ERROR_CLR           (0x00|16)
#define DS_STAT_DMA_ERROR               0x10000


#define DS_DMA_ACTIVE(ch)               (0x00040000 << (ch))

/* These actually return the DMA channel number of failure */
#define DS_PCI_PARITY_ERR(reg)          ((reg) >> 24 & 3)
#define DS_PCI_FATAL_ERR(reg)           ((reg) >> 29 & 3)
#define DS_EXT_PCI_PARITY_ERR(reg)      ((reg) >> 22 & 0x1f)
#define DS_EXT_PCI_FATAL_ERR(reg)       ((reg) >> 27 & 0x1f)

/*
 * CMIC Head Of Line (HOL) Status.
 * Contains a port bitmap (see PBMP_*) of ports that are
 * currently AVAILABLE (NOT blocking on HOL)
 * Not applicable on Lynx, reads as 0xffffffff.
 */
#define CMIC_HOL_STAT                   0x00000108

/* CMIC Configuration Register */
/* RD_BRST_EN defaults to 1 for Lynx */

#define CMIC_CONFIG                     0x0000010C

#define CC_RD_BRST_EN                   0x00000001 /* enb PCI read bursts */
#define CC_WR_BRST_EN                   0x00000002 /* enb PCI write bursts */
#define CC_BE_CHECK_EN                  0x00000004 /* Big endian chk PCI Wr */
#define CC_MSTR_Q_MAX_EN                0x00000008 /* Queue 4 PCI mastr reqs */
#define CC_LINK_STAT_EN                 0x00000010 /* enb linkstat autoupd */
#define CC_RESET_CPS                    0x00000020 /* Drive CPS bus reset */
#define CC_ACT_LOW_INT                  0x00000040 /* INTA# is active low */
#define CC_SCHAN_ABORT                  0x00000080 /* abort S-Channel opern */
#define CC_STACK_ARCH_EN                0x00000100 /* (see databook) */
#define CC_UNTAG_EN                     0x00000200 /* (see databook) */
#define CC_LE_DMA_EN                    0x00000400 /* Little endian DMA fmt */
#define CC_I2C_EN                       0x00000800 /* enb CPU access to I2C */
#define CC_LINK_SCAN_GIG                0x00001000 /* Also scan gig ports */
#define CC_DMA_GARBAGE_COLL_EN          0x00008000 /* Collect 'purge' pkts */

#define CC_ALN_OPN_EN                   0x00002000 /* Unaligned DMA enable */
#define CC_PCI_RST_ON_INIT_EN           0x00010000
#define CC_DIS_TIME_STAMP_CTR           0x00020000
#define CC_SG_OPN_EN                    0x00040000 /* DMA S/G enable */
#define CC_RLD_OPN_EN                   0x00080000 /* DMA RLD enable */
#define CC_DIS_RLD_STAT_UPDATE          0x00100000
#define CC_STOP_AUTO_SCAN_ON_LCHG       0x00200000 /* LS stop in intr */
#define CC_ABORT_STAT_DMA               0x00400000
#define CC_ATO_SCAN_FROM_ID_ZERO        0x00800000 /* Scan from PHYID=0 */
#define CC_COS_QUALIFIED_DMA_RX_EN      0x01000000
#define CC_ABORT_TBL_DMA                0x02000000 /* Abort Table DMA op */
#define CC_EXT_MDIO_MSTR_DIS            0x04000000 /* Disable MDIO on ATE */
#define CC_EXTENDED_DCB_ENABLE          0x08000000 /* type7 dcb (5695) */
#define CC_INT_PHY_CLAUSE_45            0x08000000 /* MIIM 45 vs. 22 (5673) */

#define N_DMA_CHAN                      4
#define MAX_N_DMA_CHAN                  8
#define CMICX_N_DMA_CHAN                8
#define CMICX_N_FIFODMA_CHAN            12
/* CMIC DMA Memory Address Registers (4 base addresses for 0 <= ch <= 3) */
#define CMIC_DMA_DESC(ch)               (0x00000110 + 4 * (ch))

/* CMIC I2C Registers */
#define CMIC_I2C_SLAVE_ADDR             0x00000120
#define CMIC_I2C_DATA                   0x00000124
#define CMIC_I2C_CTRL                   0x00000128
#define CMIC_I2C_STAT                   0x0000012C
#define CMIC_I2C_SLAVE_XADDR            0x00000130
#define CMIC_I2C_GP0                    0x00000134
#define CMIC_I2C_GP1                    0x00000138
#define CMIC_I2C_RESET                  0x0000013C

/*
 * Link Status (UP/Down) Register
 * Contains a port bitmap (see PBMP_*) of ports whose links are up.
 */
#define CMIC_LINK_STAT                  0x00000140
/* For 5665 */
#define CMIC_LINK_STAT_MOD1             0x00000540

/* IRQ Register (RO) 
 */

#define CMIC_IRQ_STAT                   0x00000144
#define CMIC_IRQ_STAT_1                 0x00000064
#define CMIC_IRQ_STAT_2                 0x00000068

/* IRQ Mask Register (R/W)
 */

#define CMIC_IRQ_MASK                   0x00000148
#define CMIC_IRQ_MASK_1                 0x0000006C
#define CMIC_IRQ_MASK_2                 0x00000070

/* IRQ Mask bits */
#define IRQ_SCH_MSG_DONE                0x00000001 /* Bit 0 */
#define IRQ_ARL_MBUF                    0x00000002 /* Bit 1 */
#define IRQ_CHIP_FUNC_0                 IRQ_ARL_MBUF
#define IRQ_ARL_MBUF_DROP               0x00000004 /* Bit 2 */
#define IRQ_CHIP_FUNC_1                 IRQ_ARL_MBUF_DROP
#define IRQ_GBP_FULL                    0x00000008 /* Bit 3 */
#define IRQ_CHIP_FUNC_2                 IRQ_GBP_FULL
#define IRQ_LINK_STAT_MOD               0x00000010 /* Bit 4 */
#define IRQ_ARL_DMA_XFER                0x00000020 /* Bit 5 */
#define IRQ_L2_MOD_FIFO_NOT_EMPTY       IRQ_ARL_DMA_XFER
#define IRQ_CHIP_FUNC_3                 IRQ_ARL_DMA_XFER
#define IRQ_ARL_DMA_CNT0                0x00000040 /* Bit 6 */
#define IRQ_CHIP_FUNC_4                 IRQ_ARL_DMA_CNT0
#define IRQ_DESC_DONE(ch)               (0x00000080 << (2 * (ch)))
#define IRQ_CHAIN_DONE(ch)              (0x00000100 << (2 * (ch)))
#define IRQ_PCI_PARITY_ERR              0x00008000 /* Bit 15 */
#define IRQ_PCI_FATAL_ERR               0x00010000 /* Bit 16 */
#define IRQ_SCHAN_ERR                   0x00020000 /* Bit 17 */
#define IRQ_I2C_INTR                    0x00040000 /* bit 18 */
#define IRQ_MIIM_OP_DONE                0x00080000 /* Bit 19 */
#define IRQ_STAT_ITER_DONE              0x00100000 /* Bit 20 */
#define IRQ_BIT21                       0x00200000 /* bit 21 */
#define IRQ_MMU_IRQ_STAT                IRQ_BIT21  /* Bit 21 - XGS Fabric */
#define IRQ_ARL_ERROR                   IRQ_BIT21  /* Bit 21 - XGS Switch */
#define IRQ_FIFO_CH0_DMA                IRQ_BIT21  /* Bit 21 */
#define IRQ_BIT22                       0x00400000 /* Bit 22 */
#define IRQ_IGBP_FULL                   IRQ_BIT22  /* Bit 22 */
#define IRQ_FIFO_CH1_DMA                IRQ_BIT22  /* Bit 22 */
#define IRQ_BIT23                       0x00800000 /* Unused */
#define IRQ_FIFO_CH2_DMA                IRQ_BIT23  /* Bit 23 */
#define IRQ_ARL_LPM_LO_PAR              0x01000000 /* Bit 24 */
#define IRQ_FIFO_CH3_DMA                IRQ_ARL_LPM_LO_PAR  /* Bit 24 */
#define IRQ_BIT25                       0x02000000 /* bit 25 */
#define IRQ_ARL_LPM_HI_PAR              IRQ_BIT25  /* bit 25 - 5665 etc. */
#define IRQ_BSE_CMDMEM_DONE             IRQ_BIT25  /* bit 25 - MCMD */
#define IRQ_MEM_CMD_DONE                IRQ_BIT25
#define IRQ_BIT26                       0x04000000 /* Bit 26 */
#define IRQ_ARL_L3_PAR                  IRQ_BIT26  /* Bit 26 - 5665 etc. */
#define IRQ_CSE_CMDMEM_DONE             IRQ_BIT26  /* bit 25 - MCMD */
#define IRQ_BIT27                       0x08000000 /* bit 27 */
#define IRQ_ARL_L2_PAR                  IRQ_BIT27  /* Bit 27 - 5665 etc.*/
#define IRQ_HSE_CMDMEM_DONE             IRQ_BIT27  /* bit 27 - MCMD */
#define IRQ_BIT28                       0x10000000 /* Bit 28 */
#define IRQ_ARL_VLAN_PAR                IRQ_BIT28  /* Bit 28 */
#define IRQ_MEM_FAIL                    IRQ_BIT28  /* Bit 28 */
#define IRQ_TDMA_DONE                   0x20000000 /* Bit 29 */
#define IRQ_TSLAM_DONE                  0x40000000 /* Bit 30 */
#define IRQ_BIT31                       0x80000000 /* Bit 31 */
#define IRQ_BSAFE_OP_DONE               IRQ_BIT31  /* Bit 31 */
#define IRQ_BROADSYNC_INTR              IRQ_BIT31  /* Bit 31 */

/* should be used only with soc_intr_block_lo/hi_enable/disable */
#define IRQ_BLOCK(bit)                 (1U<<(bit) ) /* Bit 0-31  */

/* Hercules CMIC MMU Status/Mask Registers */
#define CMIC_MMUIRQ_STAT                0x1a0 /* One bit for each MMU */
#define CMIC_MMUIRQ_MASK                0x1a4 /* Mask of enables */

#define CMIC_MMUIRQ_ENABLE_MASK         0x1ff /* Set all MMU's enabled */

/* Memory Fault Register */
#define CMIC_MEM_FAIL                   0x0000014C

/* Ingress Back Pressure Warning Register */
#define CMIC_IGBP_WARN                  0x00000150

/* Ingress Back Pressure Discard Register */
#define CMIC_IGBP_DISCARD               0x00000154

/* MIIM Parameter Register */
#define CMIC_MIIM_PARAM                 0x00000158

#define MIIM_PARAM_ID_OFFSET            16
#define MIIM_PARAM_REG_ADDR_OFFSET      24      /* Ignored on Lynx; see */
                                                /* CMIC_MIIM_ADDRESS */

/* MIIM Read Data Register */
#define CMIC_MIIM_READ_DATA             0x0000015C

/* Scan Ports Register */
#define CMIC_SCAN_PORTS                 0x00000160

/* Counter DMA address register */
#define CMIC_STAT_DMA_ADDR              0x00000164

/* Counter DMA setup register */
#define CMIC_STAT_DMA_SETUP             0x00000168

#define SDS_EN                          0x80000000      /* Stats DMA Enable */
#define SDS_ET                          0x40000000      /* Timer mode enable */
#define SDS_TIMERVAL_SHFT               16
#define SDS_TIMERVAL_MASK               (0x3fff << SDS_TIMERVAL_SHFT)
#define SDS_TIMER_USEC_PER_COUNT        64
#define SDS_TIMER_USEC_MAX              (0x4000 * 64 - 1)
#define SDS_TIMER_USEC_TO_COUNT(usec)   (usec / 64)
/*
 * The next two fields are not present in Lynx.  See instead
 * CMIC_64BIT_STATS_CFG (0x198, below).
 */
#define SDS_LASTREG_SHFT                8
#define SDS_LASTREG_MASK                (0xff << SDS_LASTREG_SHFT)
#define SDS_FIRSTREG_SHFT               0
#define SDS_FIRSTREG_MASK               (0xff << SDS_FIRSTREG_SHFT)

/* Counter DMA valid ports register */
#define CMIC_STAT_DMA_PORTS             0x0000016c

/*
 * Counter DMA current position register
 * Contains the internal address of the counter register currently
 * being scanned.
 */

#define CMIC_STAT_DMA_CURRENT           0x00000170

/* Endian selection register */
#define CMIC_ENDIAN_SELECT              0x00000174

#define ES_BIG_ENDIAN_PIO               0x01000001
#define ES_BIG_ENDIAN_DMA_PACKET        0x02000002
#define ES_BIG_ENDIAN_DMA_OTHER         0x04000004
#define ES_BIG_ENDIAN_CTR64_WORDS       0x08000008      /* Lynx only */
#define EN_BIG_ENDIAN_EB2_2B_SEL        0x20000020      /* Raptor/Raven EB slave only */

/*
 * Revision ID register (read-only)
 *
 * Allows chip type to be read from I2C.
 * On Draco, also contains externally jumpered module ID.
 */

#define CMIC_REVID_DEVID                0x00000178

#define CREV_DEVID(_a)                  ((_a) & 0xffff)
#define CREV_REVID(_a)                  (((_a) >> 16) & 0xff)
#define CREV_MODID(_a)                  (((_a) >> 24) & 0x1f)

/* Clock gate register (XGS) */

#define CMIC_CLOCK_GATE                 0x0000017c

#define CG_GATE_SHFT                    0
#define CG_GATE_MASK                    0x1fff
#define CG_RESET_SHFT                   16
#define CG_RESET_MASK                   (0x1fff << CG_RESET_SHFT)

/* Receive COS mapping per channel (Draco) */

#define CMIC_RX_COS_CONTROL             0x00000180

/* Receive cos mapping per channel (Triumph/Scorpion) */

#define CMIC_RX_COS_CONTROL_0           0x00000D90

#define RCC_COS_MAP_SHFT(ch)            ((ch) * 8)
#define RCC_COS_MAP_MASK(ch)            (0xff << RCC_CHAN_SHFT(ch))

/*
 * Tx Control Registers (Draco, Lynx only).  These registers specify a
 * portion of the Higig header content that doesn't vary a lot, for
 * packets sent from the CPU.  Most of the rest of the header is
 * supplied in the DCBs.
 */

#define CMIC_TX_CONTROL1                0x00000184
#define CMIC_TX_CONTROL2                0x00000188

/* CMIC Packet Mirror Control (Draco) */

#define CMIC_MIRROR_CONTROL             0x0000018c

#define MIRC_INGRESS_EN                 0x80000000
#define MIRC_EGRESS_PORTS_MASK          0x00001fff

#define CMIC_MIRROR_PORTS               0x00000190

/* Bitmap to specify which linkscan ports are internal PHY (Draco) */

#define CMIC_SCAN_INTERNAL              0x00000194

/* Control for 64-bit stat DMA (Draco IPIC) */

#define CMIC_STAT64_DMA_SETUP           0x00000198

#define SDS64_REG_COUNT_128             0x80000000

#define SDS64_LASTREG_SHFT              0
#define SDS64_LASTREG_MASK              0xff

/* For Lynx, this register is used to hold the first/last regs */
#define SDS64_LYNX_LASTREG_SHFT         16
#define SDS64_LYNX_LASTREG_MASK         (0x3ff << SDS64_LYNX_LASTREG_SHFT)

#define SDS64_LYNX_FIRSTREG_SHFT        0
#define SDS64_LYNX_FIRSTREG_MASK        0x3ff

/*
 * Table DMA Switch Table Start Address Register (Draco).
 * DMA transfer range is limited to 64k table entries (16 bits).
 */

#define CMIC_TABLE_DMA_START            0x0000019C

/* MIIM support for XGS PHYs (clause 45/22, Lynx only) */

/* MIIM address selection (Lynx only) */

#define MIIM_CL22_ADDR_SHFT             0
#define MIIM_CL22_ADDR_MASK             0x1f

#define MIIM_CL45_ADDR_SHFT             0
#define MIIM_CL45_ADDR_MASK             0xffff

#define MIIM_CL45_DTYPE_SHFT            16
#define MIIM_CL45_DTYPE_MASK            (0x3ff << MIIM_CL45_DTYPE_SHFT)


/* MIIM port selection (bit map of clause selection, 1=22, 0=45; Lynx only) */

#define MIIM_CYCLE_AUTO                 0x0
#define MIIM_CYCLE_C22_REG_WR           0x1
#define MIIM_CYCLE_C22_REG_RD           0x2
#define MIIM_CYCLE_C45_REG_AD           0x4
#define MIIM_CYCLE_C45_REG_WR           0x5
#define MIIM_CYCLE_C45_REG_RD_ADINC     0x6
#define MIIM_CYCLE_C45_REG_RD           0x7

/* CL45 warmboot write disable override */
#define MIIM_WB_C45                     (1 << 0)

#define MIIM_CYCLE_C45_SHFT             6
#define MIIM_CYCLE_C45_MASK             (0x3 << MIIM_CYCLE_C45_SHFT)
#define MIIM_CYCLE_C45_WR               (1 << (MIIM_CYCLE_C45_SHFT + 0))
#define MIIM_CYCLE_C45_WR_AD            (1 << (MIIM_CYCLE_C45_SHFT + 1))
#define MIIM_CYCLE_C45_RD               (1 << (MIIM_CYCLE_C45_SHFT + 0))
#define MIIM_CYCLE_C45_RD_ADINC         (1 << (MIIM_CYCLE_C45_SHFT + 1))


/* TAP protocol access for BIST (Lynx only) */

#define CMIC_TAP_CONTROL                0x000001ac

/*
** FB/ER TDMA/TSLAM
*/
#define CMIC_TABLE_DMA_PCIMEM_START_ADDR        0x00000420
#define CMIC_TABLE_DMA_SBUS_START_ADDR          0x00000424
#define CMIC_TABLE_DMA_ENTRY_COUNT              0x00000428
#define CMIC_TABLE_DMA_CFG                      0x0000042C
#define CMIC_TDMA_CFG_EN                        (1 << 0)
#define CMIC_TDMA_CFG_DONE                      (1 << 2)

#define CMIC_SLAM_DMA_PCIMEM_START_ADDR         0x00000440
#define CMIC_SLAM_DMA_SBUS_START_ADDR           0x00000444
#define CMIC_SLAM_DMA_ENTRY_COUNT               0x00000448
#define CMIC_SLAM_DMA_CFG                       0x0000044C
#define CMIC_SLAM_CFG_EN                        (1 << 0)
#define CMIC_SLAM_CFG_DONE                      (1 << 2)
#define CMIC_SLAM_CFG_DIR                       (1 << 21)
/*
 * -----------------------------------------------------------------------
 * LED Microcontroller Registers
 * -----------------------------------------------------------------------
 */

#define CMIC_LED_CTRL                   0x00001000
#define CMIC_LED_STATUS                 0x00001004
#define CMIC_LED_PROGRAM_RAM_BASE       0x00001800
#define CMIC_LED_DATA_RAM_BASE          0x00001c00
#define CMIC_LED_PROGRAM_RAM(_a)        (CMIC_LED_PROGRAM_RAM_BASE + 4 * (_a))
#define CMIC_LED_PROGRAM_RAM_SIZE       0x100
#define CMIC_LED_DATA_RAM(_a)           (CMIC_LED_DATA_RAM_BASE + 4 * (_a))
#define CMIC_LED_DATA_RAM_SIZE          0x100
#define CMIC_LED_REG_SIZE               4

/* Trident Switch introduces two LED processors. LED0 handles first 36 ports
 * LED1 handles the remaining 36 ports.  
 */
#define CMICE_LEDUP0_CTRL               0x00001000
#define CMICE_LEDUP0_STATUS             0x00001004
#define CMICE_LEDUP0_PROGRAM_RAM_BASE   0x00001800
#define CMICE_LEDUP0_DATA_RAM_BASE      0x00001400
#define CMICE_LEDUP1_CTRL               0x00002000
#define CMICE_LEDUP1_STATUS             0x00002004
#define CMICE_LEDUP1_PROGRAM_RAM_BASE   0x00002800
#define CMICE_LEDUP1_DATA_RAM_BASE      0x00002400

#define LC_LED_ENABLE                   0x1     /* Enable */

#define LS_LED_INIT                     0x200   /* Initializing */
#define LS_LED_RUN                      0x100   /* Running */
#define LS_LED_PC                       0xff    /* Current PC */

/* This is the offset from the data ram base that is scanned for */
/* link status.  One bit per port, 0-7 in 0x80, etc.  4 bytes.  */
#define LS_LED_DATA_OFFSET              0x80
#define LS_LED_DATA_OFFSET_A0           0xa0
#define LS_TUCANA_LED_DATA_OFFSET       0xa0
#define LS_RAPTOR_LED_DATA_OFFSET       0xa0
#define LS_RAVEN_LED_DATA_OFFSET        0xa0
#define LS_TR_VL_LED_DATA_OFFSET        0xa0
#define LS_SC_CQ_LED_DATA_OFFSET        0xa0


#define CMIC_OFFSET_MAX                 0x19c   /* For "dump pcim" */

#define CMIC_OFFSET_TRIGGER             0x1fc

#define MIIM_INTR_RETRY_COUNT   5
/*
 * Define the default number of microseconds before an S-Channel or MIIM
 * operation times out and fails.  Different values apply if the SAL
 * boot flag BOOT_F_QUICKTURN or BOOT_F_PLISIM are set.
 */

#define SCHAN_TIMEOUT           (300 * MILLISECOND_USEC)
#define SCHAN_TIMEOUT_QT        (3   * SECOND_USEC)
#define SCHAN_TIMEOUT_PLI       (40  * SECOND_USEC)

#define MIIM_INTR_RETRY_TIMEOUT (300 * MILLISECOND_USEC)
#define MIIM_TIMEOUT            (1   * SECOND_USEC)
#define MIIM_TIMEOUT_RCPU       (1   * SECOND_USEC)
#define MIIM_TIMEOUT_QT         (3   * SECOND_USEC)
#define MIIM_TIMEOUT_PLI        (40  * SECOND_USEC)

#define TDMA_TIMEOUT            (1   * SECOND_USEC)
#define TDMA_TIMEOUT_QT         (120 * SECOND_USEC)

#define TSLAM_TIMEOUT           (1   * SECOND_USEC)
#define TSLAM_TIMEOUT_QT        (120 * SECOND_USEC)

#define SBUS_TIMEOUT            (1   * SECOND_USEC)
#define SBUS_TIMEOUT_QT         (120 * SECOND_USEC)

#define FIFO_TIMEOUT            (1   * SECOND_USEC)
#define FIFO_TIMEOUT_QT         (120 * SECOND_USEC)

#define CDMA_TIMEOUT            (1   * SECOND_USEC)
#define CDMA_TIMEOUT_QT         (120 * SECOND_USEC)
#define CDMA_TIMEOUT_BCMSIM     (200 * SECOND_USEC)

#define STAT_SYNC_TIMEOUT        (40  * SECOND_USEC)
#define STAT_SYNC_TIMEOUT_QT     (120 * SECOND_USEC)
#define STAT_SYNC_TIMEOUT_BCMSIM (600 * SECOND_USEC)

#define PAXB_TX_REQ_SEQ_EN       0
#define PAXB_TX_ARB_PRIORITY     2

/*
 * CMIC Interface Routines
 */

/* Turn CMIC register offset into static pointer to descriptive name. */
extern char *soc_pci_off2name(int unit, uint32 offset);

/* Turn CMIC opcode into static pointer to descriptive name */
extern char *soc_schan_op_name(int op);

#define CMIC_SWAP32(_x)   ((((_x) & 0xff000000) >> 24) \
                         | (((_x) & 0x00ff0000) >>  8) \
                         | (((_x) & 0x0000ff00) <<  8) \
                         | (((_x) & 0x000000ff) << 24))

#define CMIC_SWAP32(_x)   ((((_x) & 0xff000000) >> 24) \
                         | (((_x) & 0x00ff0000) >>  8) \
                         | (((_x) & 0x0000ff00) <<  8) \
                         | (((_x) & 0x000000ff) << 24))

/* Get/Set CMIC PCI Register (function and inline versions) */
#ifdef  SOC_PCI_DEBUG
extern int soc_pci_getreg(int unit, uint32 addr, uint32 *data_ptr);
extern uint32 soc_pci_read(int unit, uint32 addr);
extern int soc_pci_write(int unit, uint32 addr, uint32 data);
extern uint32 soc_pci_conf_read(int unit, uint32 addr);
extern int soc_pci_conf_write(int unit, uint32 addr, uint32 data);
#else
extern int soc_pci_getreg(int unit, uint32 addr, uint32 *data_ptr);
#if defined(CMIC_SOFT_BYTE_SWAP)
#define soc_pci_read(unit, addr)        ({uint32 _retval = CMREAD(unit, addr);\
                                          _retval = CMIC_SWAP32(_retval);     \
                                          _retval;})
#define soc_pci_write(unit, addr, data) ({uint32 _var = CMIC_SWAP32(data);    \
                                          CMWRITE(unit, addr, _var);})


#if defined(IPROC_ACCESS_DEBUG) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
#undef soc_pci_read
#undef soc_pci_write
/* check that the address is in the currently supported address range of the CMIC BAR in Jericho 2 and Ramon */
#define soc_pci_read(unit, addr)        ({uint32 _retval; \
                                          if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) { \
                                              assert(addr < 0x50000 || ((addr<7*1024*1024+128*1024) && (addr >= 7*1024*1024))); \
                                          } \
                                          _retval = CMREAD(unit, addr); \
                                          _retval = CMIC_SWAP32(_retval);     \
                                          _retval;})
#define soc_pci_write(unit, addr, data) ({uint32 _var = CMIC_SWAP32(data); \
                                              if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) { \
                                                  assert(addr < 0x50000 || ((addr<7*1024*1024+128*1024) && (addr >= 7*1024*1024))); \
                                              } \
                                          CMWRITE(unit, addr, _var);})
#endif /* IPROC_ACCESS_DEBUG) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */

#else

#define soc_pci_read(unit, addr)        CMREAD(unit, addr)
#define soc_pci_write(unit, addr, data) CMWRITE(unit, addr, data)

#if defined(IPROC_ACCESS_DEBUG) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))
#undef soc_pci_read
#undef soc_pci_write
extern uint32 soc_pci_read_helper(int unit, uint32 addr);
extern uint32 soc_pci_write_helper(int unit, uint32 addr, uint32 data);

/* check that the address is in the currently supported address range of the CMIC BAR in Jericho 2 and Ramon */
#define soc_pci_read(unit, addr)        soc_pci_read_helper(unit, (addr))
#define soc_pci_write(unit, addr, data) soc_pci_write_helper(unit, (addr), data)

#endif /* IPROC_ACCESS_DEBUG) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */

#endif /* CMIC_SOFT_BYTE_SWAP */
#define	soc_pci_conf_read(unit, addr)		CMCONFREAD(unit, addr)
#define	soc_pci_conf_write(unit, addr, data)	CMCONFWRITE(unit, addr, data)
#endif  /* SOC_PCI_DEBUG */

#ifndef _SOC_CMIC_OR_IPROC_REG
#define _SOC_CMIC_OR_IPROC_REG
extern int soc_cmic_or_iproc_getreg(int unit, soc_reg_t reg, uint32 *data);
extern int soc_cmic_or_iproc_setreg(int unit, soc_reg_t reg, uint32 data);
#endif

extern uint32 soc_pci_mcs_read(int unit, uint32 addr);
extern int soc_pci_mcs_getreg(int unit, uint32 addr, uint32 *data_ptr);
extern int soc_pci_mcs_write(int unit, uint32 addr, uint32 data);


/* Test PCI memory range 0x00-0x4f */
extern int soc_is_cmicx(int unit);
extern int soc_cmicx_pci_test(int unit);
extern int soc_pci_test(int unit);
extern int soc_pcie_phy_read(int unit, uint32 addr, uint16 *val);
extern int soc_pcie_phy_write(int unit, uint32 addr, uint16 val);
extern int soc_pcie_phy_type_get(int unit, int *type, unsigned *iproc_ver);
extern int soc_pcie_fw_status_get(int unit, uint32 *valid);
extern int soc_cmicx_iproc_version_get(int unit,
            unsigned int *iproc_ver, unsigned int *cmic_ver, unsigned int *cmic_rev);

extern int soc_cmicx_paxb_tx_arbiter_set(int unit, uint8 enable, uint8 priority);
extern void soc_cmicx_reg_offset_get(uint32 *min, uint32 *max);

/*
 * Do a "harmless" memory read from a special location.
 */
extern void soc_pci_analyzer_trigger(int unit);

extern int soc_anyreg_read(int unit, soc_regaddrinfo_t *ainfo, uint64 *data);
extern int soc_anyreg_write(int unit, soc_regaddrinfo_t *ainfo, uint64 data);

/* Read/write internal registers */
extern int soc_reg_read(int unit, soc_reg_t reg, uint32 addr, uint64 *data);
extern int soc_reg_write(int unit, soc_reg_t reg, uint32 addr, uint64 data);

extern int soc_reg_get(int unit, soc_reg_t reg, int port, int index, uint64 *data);
extern int soc_reg_set(int unit, soc_reg_t reg, int port, int index, uint64 data);
extern int soc_reg_set_nocache(int unit, soc_reg_t reg, int port, int index, uint64 data);
extern int _soc_reg32_get(int unit, soc_block_t block, int acc, uint32 addr, uint32 *data);
extern int _soc_reg32_set(int unit, soc_block_t block, int acc, uint32 addr, uint32 data);
extern int soc_reg32_read(int unit, uint32 addr, uint32 *data);
extern int soc_reg32_write(int unit, uint32 addr, uint32 data);

extern int soc_reg32_get(int unit, soc_reg_t reg, int port, int index, uint32 *data);
extern int soc_reg32_set(int unit, soc_reg_t reg, int port, int index, uint32 data);

extern int soc_reg64_read(int unit, uint32 addr, uint64 *data);
extern int soc_reg64_write(int unit, uint32 addr, uint64 data);

extern int soc_reg64_get(int unit, soc_reg_t reg, int port, int index, uint64 *data);
extern int soc_reg64_set(int unit, soc_reg_t reg, int port, int index, uint64 data);
extern int _soc_reg64_get(int unit, soc_block_t block, int acc, uint32 addr, uint64 *data);
extern int _soc_reg64_set(int unit, soc_block_t block, int acc, uint32 addr, uint64 data);

extern int soc_reg_rawport_get(int unit, soc_reg_t reg, int port, int index, uint64 *data);
extern int soc_reg_rawport_set(int unit, soc_reg_t reg, int port, int index, uint64 data);
extern int soc_reg32_rawport_get(int unit, soc_reg_t reg, int port, int index, uint32 *data);
extern int soc_reg32_rawport_set(int unit, soc_reg_t reg, int port, int index, uint32 data);

extern int soc_reg_egress_cell_count_get(int unit, soc_port_t port, int cos, 
                                         uint32 *data);
/* Read/Write custom registers */
extern int soc_custom_reg32_get(int unit, soc_reg_t reg, int port, int index, uint32 *data);
extern int soc_custom_reg32_set(int unit, soc_reg_t reg, int port, int index, uint32 data);

extern int soc_custom_reg64_get(int unit, soc_reg_t reg, int port, int index, uint64 *data);
extern int soc_custom_reg64_set(int unit, soc_reg_t reg, int port, int index, uint64 data);

extern int soc_custom_reg_above_64_get(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data);
extern int soc_custom_reg_above_64_set(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data);

/* Write internal register for a group of ports */
extern int soc_reg_write_ports(int unit, soc_reg_t reg, pbmp_t pbmp,
                               uint32 value);

/* Write internal register for a block or group of blocks */
extern int soc_reg_write_blocks(int unit, soc_reg_t reg, soc_block_t block,
                                uint64 value);

/* Write internal register for all possible blocks */
extern int soc_reg_write_all_blocks(int unit, soc_reg_t reg, uint32 value);
extern int soc_reg64_write_all_blocks(int unit, soc_reg_t reg, uint64 value);
/* Read a single copy of the register from any appropriate block */
extern int soc_reg_read_any_block(int unit, soc_reg_t reg, uint32 *datap);
extern int soc_reg64_read_any_block(int unit, soc_reg_t reg, uint64 *datap);

extern int soc_direct_reg_get(int unit, int cmic_block,
                              uint32 addr, uint32 dwc_read, uint32 *data);
extern int soc_direct_reg_set(int unit, int cmic_block,
                              uint32 addr, uint32 dwc_write, uint32 *data);
extern int soc_direct_memreg_get(int unit, int cmic_block,
                              uint32 addr, uint32 dwc_read, int is_mem, uint32 *data);
extern int soc_direct_memreg_set(int unit, int cmic_block, 
                              uint32 addr, uint32 dwc_write, int is_mem, uint32 *data);

/* Set link scanning calls */

extern int  soc_linkscan_config(int unit, pbmp_t mii_pbm, pbmp_t direct_pbm);
extern void soc_linkscan_pause(int unit);
extern void soc_linkscan_continue(int unit);
extern int  soc_linkscan_register(int unit, void (*f)(int));
extern int  soc_linkscan_hw_init(int unit);
extern int  _soc_linkscan_hw_port_init(int unit, soc_port_t port);
extern int  soc_linkscan_hw_port_update(int unit, soc_port_t port);
extern int  soc_linkscan_hw_link_get(int unit, soc_pbmp_t *hw_link);
extern int _soc_linkscan_phy_flags_test(int unit, int port, int flags);
extern int soc_linkscan_hw_link_cache_get(int unit, soc_pbmp_t *hw_link);

extern int  soc_link_fwd_set(int unit, pbmp_t fwd);
extern void soc_link_fwd_get(int unit, pbmp_t *fwd);
extern int  soc_link_mask1_set(int unit, pbmp_t mask);
extern void soc_link_mask1_get(int unit, pbmp_t *mask);
extern int  soc_link_mask2_set(int unit, pbmp_t mask);
extern void soc_link_mask2_get(int unit, pbmp_t *mask);

/*
 * Read/write from PHY registers.
 *
 *   On BCM5690, the internal MDIO bus can be accessed by adding
 *   an offset of 0x80 to the PHY ID.
 */

#define SOC_INTERNAL_PHY_ID_OFFSET              0x80

/* 
 * Identifier of bus type of external PHY access
 *
 *
 *  31                16 15          8 7           0
 * +--------------------+------ ------+-------------+
 * | Reg bus type       |    addr1    |    addr2    |
 * +--------------------+-------------+-------------+
 *
 * Reg bus type: Bus type identifier
 * addr1: 
 *        For 'Reg bus type=SOC_EXTERNAL_PHY_BUS_CPUMDIO': PHY id.
 * addr2: 
 *        For 'Reg bus type=SOC_EXTERNAL_PHY_BUS_CPUMDIO': PHY reg addr.
 */
/* Bus type identifier: external PHY bus is CPU's MDIO interface */
#define SOC_EXTERNAL_PHY_BUS_CPUMDIO 0x80000000 

extern int soc_miim_write(int unit, uint16 phy_id,
                          uint8 phy_reg_addr, uint16 phy_wr_data);
extern int soc_esw_miim_write(int unit, uint32 phy_id,
                              uint32 phy_reg_addr, uint16 phy_wr_data);
extern int soc_dcmn_miim_write(int unit, int clause, uint32 phy_id,
                              uint32 phy_reg_addr, uint16 phy_wr_data);
extern int soc_dcmn_miim_cl22_write(int unit, uint32 phy_id,
                              uint32 phy_reg_addr, uint16 phy_wr_data);
extern int soc_dcmn_miim_cl45_write(int unit, uint32 phy_id,
                              uint32 phy_reg_addr, uint16 phy_wr_data);
extern int soc_miim_read(int unit, uint16 phy_id,
                         uint8 phy_reg_addr, uint16 *phy_rd_data);
extern int soc_esw_miim_read(int unit, uint32 phy_id,
                             uint32 phy_reg_addr, uint16 *phy_rd_data);
extern int soc_dcmn_miim_read(int unit, int clause, uint32 phy_id,
                             uint32 phy_reg_addr, uint16 *phy_rd_data);
extern int soc_dcmn_miim_cl22_read(int unit, uint32 phy_id,
                             uint32 phy_reg_addr, uint16 *phy_rd_data);
extern int soc_dcmn_miim_cl45_read(int unit, uint32 phy_id,
                             uint32 phy_reg_addr, uint16 *phy_rd_data);
extern int soc_sbus_mdio_write(int unit, uint16 phy_id,
                               uint32 phy_reg_addr, uint32 phy_wr_data);
extern int soc_sbus_mdio_read(int unit, uint16 phy_id,
                              uint32 phy_reg_addr, uint32 *phy_rd_data);
#ifdef PORTMOD_SUPPORT
extern int portmod_ext_phy_mdio_c45_reg_read( void* user_acc, 
                                                     uint32 core_addr, 
                                                     uint32 reg_addr, 
                                                     uint32 *val);

extern int portmod_ext_phy_mdio_c45_reg_write( void* user_acc, 
                                                      uint32 core_addr, 
                                                      uint32 reg_addr, 
                                                      uint32 val);
#endif /* PORTMOD_SUPPORT */


extern int soc_miim_modify(int unit, uint16 phy_id,
                 uint16 phy_reg_addr, uint16 phy_rd_data, uint16 phy_rd_mask);

extern int soc_miimc45_write(int unit, uint16 phy_id, uint8 phy_devad,
           uint16 phy_reg_addr, uint16 phy_wr_data);
extern int soc_esw_miimc45_write(int unit, uint32 phy_id,
                              uint32 phy_reg_addr, uint16 phy_wr_data);
extern int soc_esw_miimc45_wb_write(int unit, uint32 phy_id,
                              uint32 phy_reg_addr, uint16 phy_wr_data);
extern int soc_miimc45_read(int unit, uint16 phy_id, uint8 phy_devad,
          uint16 phy_reg_addr, uint16 *phy_rd_data);
extern int soc_esw_miimc45_read(int unit, uint32 phy_id,
                             uint32 phy_reg_addr, uint16 *phy_rd_data);
extern int soc_esw_miimc45_data_write(int unit, uint32 phy_id,
                                      uint8 phy_devad, uint16 phy_wr_data);
extern int soc_esw_miimc45_data_read(int unit, uint32 phy_id,
                                     uint8 phy_devad, uint16 *phy_rd_data);
extern int soc_esw_miimc45_addr_write(int unit, uint32 phy_id,
                                      uint8 phy_devad, uint16 phy_ad_data);
extern int soc_esw_miimc45_post_read(int unit, uint32 phy_id,
                                     uint8 phy_devad, uint16 *phy_rd_data);

extern int soc_miimc45_modify(int unit, uint16 phy_id, uint8 phy_devad,
                 uint16 phy_reg_addr, uint16 phy_rd_data, uint16 phy_rd_mask);

extern int soc_rcpu_miim_write(int unit, uint16 phy_id,
                uint8 phy_reg_addr, uint16 phy_wr_data);
extern int soc_rcpu_miim_read(int unit, uint16 phy_id,
                 uint8 phy_reg_addr, uint16 *phy_rd_data);
extern int soc_rcpu_miimc45_write(int unit, uint16 phy_id, uint8 phy_devad,
                  uint16 phy_reg_addr, uint16 phy_wr_data);
extern int soc_rcpu_miimc45_read(int unit, uint16 phy_id, uint8 phy_devad,
                 uint16 phy_reg_addr, uint16 *phy_rd_data);


/* Return the iproc version of a device, given its PCIe device ID.
 * Needed for devices with iproc of version >= 14, for PCIe handling through I2C.
 * A return value of 0 means the device ID is not recognized.
 */
extern unsigned get_iproc_version_from_device_id(uint16 dev_id);

/* Return the iproc version of a device, given its unit number
 * A return value of 0 means the device ID is not recognized.
 * known to not be composite.
 */
extern unsigned get_iproc_version_from_unit(int unit);

#endif  /* !_SOC_CMIC_H */
