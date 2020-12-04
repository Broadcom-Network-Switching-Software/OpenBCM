/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        qsgmii65.h
 * Purpose:     65nm Internal (Octal) Serdes PHY Driver
 */

#ifndef   _QSGMII65_H_
#define   _QSGMII65_H_

/* 65LP SerDes register access */
#define QSGMII65_REG_READ(_unit, _phy_ctrl, _flags, \
                          _reg_addr, _val) \
            phy_reg_qsgmii_aer_read((_unit), (_phy_ctrl), \
                            (_reg_addr), (_val))
#define QSGMII65_REG_WRITE(_unit, _phy_ctrl, _flags, \
                          _reg_addr, _val) \
            phy_reg_qsgmii_aer_write((_unit), (_phy_ctrl), \
                             (_reg_addr), (_val))
#define QSGMII65_REG_MODIFY(_unit, _phy_ctrl, _flags, \
                            _reg_addr, _val, _mask) \
            phy_reg_qsgmii_aer_modify((_unit), (_phy_ctrl), \
                              (_reg_addr), (_val), (_mask))


/***************************/
/* Standard IEEE Registers */
/***************************/

/* MII Control (Addr 00h) */
#define READ_QSGMII65_MII_CTRLr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00000000, (_val))
#define WRITE_QSGMII65_MII_CTRLr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00000000, (_val))
#define MODIFY_QSGMII65_MII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00000000, \
                               (_val), (_mask))

/* MII Status (Addr 01h) */
#define READ_QSGMII65_MII_STATr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00000001, (_val))
#define WRITE_QSGMII65_MII_STATr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00000001, (_val))
#define MODIFY_QSGMII65_MII_STATr(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00000001, \
                               (_val), (_mask))

/* PHY ID 0 (Addr 02h) */
#define READ_QSGMII65_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00000002, (_val))
#define WRITE_QSGMII65_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00000002, (_val))
#define MODIFY_QSGMII65_MII_PHY_ID0r(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00000002, \
                             (_val), (_mask))

/* PHY ID 1 (Addr 03h) */
#define READ_QSGMII65_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00000003, (_val))
#define WRITE_QSGMII65_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00000003, (_val))
#define MODIFY_QSGMII65_MII_PHY_ID1r(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00000003, \
                             (_val), (_mask))

/* Autoneg Adv (Addr 04h) */
#define READ_QSGMII65_MII_ANAr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00000004, (_val))
#define WRITE_QSGMII65_MII_ANAr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00000004, (_val))
#define MODIFY_QSGMII65_MII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00000004, \
                             (_val), (_mask))

/* Autoneg Link Partner Ability (Addr 05h) */
#define READ_QSGMII65_MII_ANPr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                   0x00, 0x00000005, (_val))
#define WRITE_QSGMII65_MII_ANPr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                   0x00, 0x00000005, (_val))
#define MODIFY_QSGMII65_MII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00000005, \
                             (_val), (_mask))

/* Autoneg Expansion (Addr 06h) */
#define READ_QSGMII65_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00000006, (_val))
#define WRITE_QSGMII65_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00000006, (_val))
#define MODIFY_QSGMII65_MII_AN_EXPr(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00000006, \
                             (_val), (_mask))

/* Autoneg Next Page (Addr 07h) */
/* Autoneg Link Partner Ability 2 (Addr 08h) */

/* Extended Status (Addr 0fh) */
#define READ_QSGMII65_MII_EXT_STATr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00000009, (_val))
#define WRITE_QSGMII65_MII_EXT_STATr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00000009, (_val))
#define MODIFY_QSGMII65_MII_EXT_STATr(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00000009, \
                             (_val), (_mask))
/****************************************************************************
 * QSGMII65_USER_XgxsBlk0
 */

/* XgxsBlk0 :: xgxsControl :: start_sequencer [13:13] */
#define XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK                  0x2000
#define XGXSBLK0_XGXSCONTROL_START_SEQUENCER_ALIGN                 0
#define XGXSBLK0_XGXSCONTROL_START_SEQUENCER_BITS                  1
#define XGXSBLK0_XGXSCONTROL_START_SEQUENCER_SHIFT                 13


/* XGXG control register */
#define READ_QSGMII65_XGXSBLK0_XGXSCONTROLr(_unit, _pc, _val) \
             QSGMII65_REG_READ((_unit), (_pc), 0x00, 0x00008000, (_val))
#define WRITE_QSGMII65_XGXSBLK0_XGXSCONTROLr(_unit, _pc, _val) \
             QSGMII65_REG_WRITE((_unit), (_pc), 0x00, 0x00008000, (_val))
#define MODIFY_QSGMII65_XGXSBLK0_XGXSCONTROLr(_unit, _pc, _val, _mask) \
             QSGMII65_REG_MODIFY((_unit), (_pc), 0x00, 0x00008000, (_val), (_mask))


/*************************/
/* XGXS BLOCK 1 */
/*************************/
/* LANE Control 0 (Addr 15h) */
#define READ_QSGMII65_LANE_CTRL0r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008015, (_val))
#define WRITE_QSGMII65_LANE_CTRL0r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008015, (_val))
#define MODIFY_QSGMII65_LANE_CTRL0r(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008015, \
                             (_val), (_mask))

/* LANE Control 3 (Addr 18h) */
#define READ_QSGMII65_LANE_CTRL3r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008018, (_val))
#define WRITE_QSGMII65_LANE_CTRL3r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008018, (_val))
#define MODIFY_QSGMII65_LANE_CTRL3r(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008018, \
                             (_val), (_mask))

/*************************/
/* Digital Block (Blk 0) */
/*************************/
/* 1000X Control 1 (Addr 10h) */
#define READ_QSGMII65_1000X_CTRL1r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008300, (_val))
#define WRITE_QSGMII65_1000X_CTRL1r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008300, (_val))
#define MODIFY_QSGMII65_1000X_CTRL1r(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008300, \
                             (_val), (_mask))

/* 1000X Control 2 (Addr 11h) */
#define READ_QSGMII65_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008301, (_val))
#define WRITE_QSGMII65_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008301, (_val))
#define MODIFY_QSGMII65_1000X_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008301, \
                             (_val), (_mask))

/* 1000X Control 3 (Addr 12h) */
#define READ_QSGMII65_1000X_CTRL3r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008302, (_val))
#define WRITE_QSGMII65_1000X_CTRL3r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008302, (_val))
#define MODIFY_QSGMII65_1000X_CTRL3r(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008302, \
                             (_val), (_mask))

/* 1000X Status 1 (Addr 14h) */
#define READ_QSGMII65_1000X_STAT1r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008304, (_val))
#define WRITE_QSGMII65_1000X_STAT1r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008304, (_val))
#define MODIFY_QSGMII65_1000X_STAT1r(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008304, \
                             (_val), (_mask))

/* 1000X Status 2 (Addr 15h) */
#define READ_QSGMII65_1000X_STAT2r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008305, (_val))
#define WRITE_QSGMII65_1000X_STAT2r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008305, (_val))
#define MODIFY_QSGMII65_1000X_STAT2r(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008305, \
                             (_val), (_mask))

/* Ber Crc Err Rx Pkt cntr (Addr 17h) */
#define READ_QSGMII65_1000X_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008307, (_val))
#define WRITE_QSGMII65_1000X_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008307, (_val))
#define MODIFY_QSGMII65_1000X_PKT_COUNTERr(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008307, \
                             (_val), (_mask))

/* Pat Gen Control (Addr 1Ah) */
#define READ_QSGMII65_1000X_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000830a, (_val))
#define WRITE_QSGMII65_1000X_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000830a, (_val))
#define MODIFY_QSGMII65_1000X_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000830a, \
                             (_val), (_mask))

/* Pat Gen Status (Addr 1Bh) */
#define READ_QSGMII65_1000X_PAT_GEN_STATr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000830b, (_val))
#define WRITE_QSGMII65_1000X_PAT_GEN_STATr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000830b, (_val))
#define MODIFY_QSGMII65_1000X_PAT_GEN_STATr(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000830b, \
                             (_val), (_mask))

/* Test Mode (Addr 1Ch) */
#define READ_QSGMII65_1000X_TEST_MODEr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000830c, (_val))
#define WRITE_QSGMII65_1000X_TEST_MODEr(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000830c, (_val))
#define MODIFY_QSGMII65_1000X_TEST_MODEr(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000830c, \
                             (_val), (_mask))

/* 1000X Control #1 Register: Controls 10B/SGMII mode */
#define LP_1000X_FIBER_MODE    (1 << 0)     /* Enable SGMII fiber mode */
#define LP_1000X_EN10B_MODE    (1 << 1)     /* Enable TBI 10B interface */
#define LP_1000X_INVERT_SD     (1 << 3)     /* Invert Signal Detect */
#define LP_1000X_AUTO_DETECT   (1 << 4)     /* Auto-detect SGMII and 1000X */
#define LP_1000X_SGMII_MASTER  (1 << 5)
#define LP_1000X_PLL_PWRDWN    (1 << 6)
#define LP_1000X_RX_PKTS_CNTR_SEL (1 << 11) /* Select receive counter for 17h*/
#define LP_1000X_TX_AMPLITUDE_OVRD    (1 << 12)

/* 1000X Control #2 Register Fields */
#define LP_1000X_PAR_DET_EN        (1 << 0)    /* Enable Parallel Detect */
#define LP_1000X_FALSE_LNK_DIS     (1 << 1)    /* Disable false link */
#define LP_1000X_FILTER_FORCE_EN   (1 << 2)    /* Enable filter force link */
/* Allow packet to be transmitted regardless of the condition of the link */
#define LP_1000X_FORCE_XMIT_DATA_ON_TXSIDE (1 << 5) 
#define LP_1000X_CLRAR_BER_CNTR    (1 << 14)   /* Clear bit-err-rate counter */

/* 1000X Control #3 Register Fields */
#define LP_1000X_TX_FIFO_RST           (1 << 0)    /* Reset TX FIFO */
#define LP_1000X_FIFO_ELASTICITY_MASK  (0x3 << 1)  /* Fifo Elasticity */
#define LP_1000X_FIFO_ELASTICITY_5K    (0x0 << 1)  /* 5 Kbytes */
#define LP_1000X_FIFO_ELASTICITY_10K   (0x1 << 1)  /* 10 Kbytes */
#define LP_1000X_FIFO_ELASTICITY_13_5K (0x2 << 1)  /* 13.5 Kbytes */
#define LP_1000X_FREQ_LOCK_ELASTICITY_RX (1 << 6)
#define LP_1000X_FREQ_LOCK_ELASTICITY_TX (1 << 5)
#define LP_1000X_RX_FIFO_RST           (1 << 14)   /* Reset RX FIFO */

/* 1000X Control #4 Register Fields */
#define LP_1000X_DIG_RESET             (1 << 6)    /* Reset Datapath */

/* 1000X Status #1 Register Fields */
#define LP_1000X_STATUS1_SGMII_MODE    (1 << 0)
#define LP_1000X_STATUS1_SPEED         (3 << 3)
#define LP_1000X_STATUS1_SPEED_10      (0)
#define LP_1000X_STATUS1_SPEED_100     (1 << 3)
#define LP_1000X_STATUS1_SPEED_1000    (1 << 4)

/************************/
/* Analog Block (Blk 3) */
/************************/
/* Analog Tx 1 (Addr 10h) */
#define READ_QSGMII65_ANALOG_TX1r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x00008067, (_val))
#define WRITE_QSGMII65_ANALOG_TX1r(_unit, _phy_ctrl, _val) \
            QSGMII65_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x00008067, (_val))
#define MODIFY_QSGMII65_ANALOG_TX1r(_unit, _phy_ctrl, _val, _mask) \
            QSGMII65_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x00008067, \
                             (_val), (_mask))
#endif /* _QSGMII65_H_ */
