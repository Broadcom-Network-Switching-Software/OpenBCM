/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        serdes65LP.h
 * Purpose:     65nm Internal (Quad/Octal/Dodeca) Serdes PHY Driver
 */

#ifndef   _SERDES_65LP_H_
#define   _SERDES_65LP_H_

/* 65LP SerDes register access */
#define SERDES65LP_REG_READ(_unit, _phy_ctrl, _flags, _reg_bank, \
                          _reg_addr, _val) \
            phy_reg_serdes_read((_unit), (_phy_ctrl), (_reg_bank), \
                            (_reg_addr), (_val))
#define SERDES65LP_REG_WRITE(_unit, _phy_ctrl, _flags, _reg_bank, \
                          _reg_addr, _val) \
            phy_reg_serdes_write((_unit), (_phy_ctrl), (_reg_bank), \
                             (_reg_addr), (_val))
#define SERDES65LP_REG_MODIFY(_unit, _phy_ctrl, _flags, _reg_bank, \
                            _reg_addr, _val, _mask) \
            phy_reg_serdes_modify((_unit), (_phy_ctrl), (_reg_bank), \
                              (_reg_addr), (_val), (_mask))


/***************************/
/* Standard IEEE Registers */
/***************************/

/* MII Control (Addr 00h) */
#define READ_SERDES65LP_MII_CTRLr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x00, (_val))
#define WRITE_SERDES65LP_MII_CTRLr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x00, (_val))
#define MODIFY_SERDES65LP_MII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, \
                               (_val), (_mask))

/* MII Status (Addr 01h) */
#define READ_SERDES65LP_MII_STATr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x01, (_val))
#define WRITE_SERDES65LP_MII_STATr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x01, (_val))
#define MODIFY_SERDES65LP_MII_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, \
                               (_val), (_mask))

/* PHY ID 0 (Addr 02h) */
#define READ_SERDES65LP_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x02, (_val))
#define WRITE_SERDES65LP_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x02, (_val))
#define MODIFY_SERDES65LP_MII_PHY_ID0r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, \
                             (_val), (_mask))

/* PHY ID 1 (Addr 03h) */
#define READ_SERDES65LP_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x03, (_val))
#define WRITE_SERDES65LP_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x03, (_val))
#define MODIFY_SERDES65LP_MII_PHY_ID1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, \
                             (_val), (_mask))

/* Autoneg Adv (Addr 04h) */
#define READ_SERDES65LP_MII_ANAr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x04, (_val))
#define WRITE_SERDES65LP_MII_ANAr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x04, (_val))
#define MODIFY_SERDES65LP_MII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, \
                             (_val), (_mask))

/* Autoneg Link Partner Ability (Addr 05h) */
#define READ_SERDES65LP_MII_ANPr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                   0x00, 0x0000, 0x05, (_val))
#define WRITE_SERDES65LP_MII_ANPr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                   0x00, 0x0000, 0x05, (_val))
#define MODIFY_SERDES65LP_MII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, \
                             (_val), (_mask))

/* Autoneg Expansion (Addr 06h) */
#define READ_SERDES65LP_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x06, (_val))
#define WRITE_SERDES65LP_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x06, (_val))
#define MODIFY_SERDES65LP_MII_AN_EXPr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, \
                             (_val), (_mask))

/* Autoneg Next Page (Addr 07h) */
/* Autoneg Link Partner Ability 2 (Addr 08h) */

/* Extended Status (Addr 0fh) */
#define READ_SERDES65LP_MII_EXT_STATr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x0f, (_val))
#define WRITE_SERDES65LP_MII_EXT_STATr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x09, (_val))
#define MODIFY_SERDES65LP_MII_EXT_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, \
                             (_val), (_mask))

/*************************/
/* Digital Block (Blk 0) */
/*************************/
/* 1000X Control 1 (Addr 10h) */
#define READ_SERDES65LP_1000X_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x10, (_val))
#define WRITE_SERDES65LP_1000X_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x10, (_val))
#define MODIFY_SERDES65LP_1000X_CTRL1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, \
                             (_val), (_mask))

/* 1000X Control 2 (Addr 11h) */
#define READ_SERDES65LP_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x11, (_val))
#define WRITE_SERDES65LP_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x11, (_val))
#define MODIFY_SERDES65LP_1000X_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, \
                             (_val), (_mask))

/* 1000X Control 3 (Addr 12h) */
#define READ_SERDES65LP_1000X_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x12, (_val))
#define WRITE_SERDES65LP_1000X_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x12, (_val))
#define MODIFY_SERDES65LP_1000X_CTRL3r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x12, \
                             (_val), (_mask))

/* 1000X Control 4 (Addr 13h) */
#define READ_SERDES65LP_1000X_CTRL4r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x13, (_val))
#define WRITE_SERDES65LP_1000X_CTRL4r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x13, (_val))
#define MODIFY_SERDES65LP_1000X_CTRL4r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x13, \
                             (_val), (_mask))

/* 1000X Status 1 (Addr 14h) */
#define READ_SERDES65LP_1000X_STAT1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x14, (_val))
#define WRITE_SERDES65LP_1000X_STAT1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x14, (_val))
#define MODIFY_SERDES65LP_1000X_STAT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x14, \
                             (_val), (_mask))

/* 1000X Status 2 (Addr 15h) */
#define READ_SERDES65LP_1000X_STAT2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x15, (_val))
#define WRITE_SERDES65LP_1000X_STAT2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x15, (_val))
#define MODIFY_SERDES65LP_1000X_STAT2r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x15, \
                             (_val), (_mask))

/* 1000X Status 3 (Addr 16h) */
#define READ_SERDES65LP_1000X_STAT3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x16, (_val))
#define WRITE_SERDES65LP_1000X_STAT3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x16, (_val))
#define MODIFY_SERDES65LP_1000X_STAT3r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x16, \
                             (_val), (_mask))

/* Ber Crc Err Rx Pkt cntr (Addr 17h) */
#define READ_SERDES65LP_1000X_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x17, (_val))
#define WRITE_SERDES65LP_1000X_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x17, (_val))
#define MODIFY_SERDES65LP_1000X_PKT_COUNTERr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x17, \
                             (_val), (_mask))

/* Prbs Control (Addr 18h) */
#define READ_SERDES65LP_1000X_PRBS_CTRLr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x18, (_val))
#define WRITE_SERDES65LP_1000X_PRBS_CTRLr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x18, (_val))
#define MODIFY_SERDES65LP_1000X_PRBS_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, \
                             (_val), (_mask))

/* Prbs Status (Addr 19h) */
#define READ_SERDES65LP_1000X_PRBS_STATr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x19, (_val))
#define WRITE_SERDES65LP_1000X_PRBS_STATr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x19, (_val))
#define MODIFY_SERDES65LP_1000X_PRBS_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x19, \
                             (_val), (_mask))

/* Pat Gen Control (Addr 1Ah) */
#define READ_SERDES65LP_1000X_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1a, (_val))
#define WRITE_SERDES65LP_1000X_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1a, (_val))
#define MODIFY_SERDES65LP_1000X_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1a, \
                             (_val), (_mask))

/* Pat Gen Status (Addr 1Bh) */
#define READ_SERDES65LP_1000X_PAT_GEN_STATr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1b, (_val))
#define WRITE_SERDES65LP_1000X_PAT_GEN_STATr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1b, (_val))
#define MODIFY_SERDES65LP_1000X_PAT_GEN_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1b, \
                             (_val), (_mask))

/* Test Mode (Addr 1Ch) */
#define READ_SERDES65LP_1000X_TEST_MODEr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1c, (_val))
#define WRITE_SERDES65LP_1000X_TEST_MODEr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1c, (_val))
#define MODIFY_SERDES65LP_1000X_TEST_MODEr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1c, \
                             (_val), (_mask))

/* Force Tx Data1 (Addr 1Dh) */
#define READ_SERDES65LP_1000X_FORCE_TX_DATA1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1d, (_val))
#define WRITE_SERDES65LP_1000X_FORCE_TX_DATA1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1d, (_val))
#define MODIFY_SERDES65LP_1000X_FORCE_TX_DATA1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1d, \
                             (_val), (_mask))

/* Force Tx Data2 (Addr 1Eh) */
#define READ_SERDES65LP_1000X_FORCE_TX_DATA2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1e, (_val))
#define WRITE_SERDES65LP_1000X_FORCE_TX_DATA2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1e, (_val))
#define MODIFY_SERDES65LP_1000X_FORCE_TX_DATA2r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, \
                             (_val), (_mask))

/* 1000X Control #1 Register: Controls 10B/SGMII mode */
#define LP_1000X_FIBER_MODE    (1 << 0)     /* Enable SGMII fiber mode */
#define LP_1000X_EN10B_MODE    (1 << 1)     /* Enable TBI 10B interface */
#define LP_1000X_SIGNAL_DETECT_EN     (1 << 2)  /* Enable Signal Detect From Pin */
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
#define LP_1000X_LINK_FORCE            (1 << 7)    /* Link Force */
#define LP_1000X_DIG_RESET             (1 << 6)    /* Reset Datapath */
#define LP_1000X_ANALOG_SIG_DETECT     (1 << 0)    /* User analog signal det */

/* 1000X Status #1 Register Fields */
#define LP_1000X_STATUS1_SGMII_MODE    (1 << 0)
#define LP_1000X_STATUS1_FULL_DUPLEX   (1 << 2)
#define LP_1000X_STATUS1_SPEED         (3 << 3)
#define LP_1000X_STATUS1_SPEED_10      (0)
#define LP_1000X_STATUS1_SPEED_100     (1 << 3)
#define LP_1000X_STATUS1_SPEED_1000    (1 << 4)

/***************************/
/* SerDes ID Block (Blk 1) */
/***************************/
/* SerDes ID0 (Addr 10h) */
#define READ_SERDES65LP_SERDES_ID0r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x10, (_val))
#define WRITE_SERDES65LP_SERDES_ID0r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x10, (_val))
#define MODIFY_SERDES65LP_SERDES_ID0r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x10, \
                             (_val), (_mask))

/* SerDes ID1 (Addr 11h) */
#define READ_SERDES65LP_SERDES_ID1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x11, (_val))
#define WRITE_SERDES65LP_SERDES_ID1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x11, (_val))
#define MODIFY_SERDES65LP_SERDES_ID1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x11, \
                             (_val), (_mask))

/* SerDes ID2 (Addr 12h) */
#define READ_SERDES65LP_SERDES_ID2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x12, (_val))
#define WRITE_SERDES65LP_SERDES_ID2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x12, (_val))
#define MODIFY_SERDES65LP_SERDES_ID2r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x12, \
                             (_val), (_mask))

/* SerDes ID3 (Addr 13h) */
#define READ_SERDES65LP_SERDES_ID3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x13, (_val))
#define WRITE_SERDES65LP_SERDES_ID3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x13, (_val))
#define MODIFY_SERDES65LP_SERDES_ID3r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x13, \
                             (_val), (_mask))

/* TPOUT 1 (Addr 18h) */
#define READ_SERDES65LP_SERDES_TPOUT1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x18, (_val))
#define WRITE_SERDES65LP_SERDES_TPOUT1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x18, (_val))
#define MODIFY_SERDES65LP_SERDES_TPOUT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, \
                             (_val), (_mask))

/* TPOUT 2 (Addr 19h) */
#define READ_SERDES65LP_SERDES_TPOUT2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x19, (_val))
#define WRITE_SERDES65LP_SERDES_TPOUT2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x19, (_val))
#define MODIFY_SERDES65LP_SERDES_TPOUT2r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x19, \
                             (_val), (_mask))

/**************************/
/*  FX100 Block (Block 2) */
/**************************/

/* FXControl 1 Register (Addr 0x10) */
#define READ_SERDES65LP_FX100_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x10, \
                                    (_val))
#define WRITE_SERDES65LP_FX100_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x10, \
                                    (_val))
#define MODIFY_SERDES65LP_FX100_CTRL1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x10, (_val), (_mask))

/* FXControl 2 Register (Addr 0x11) */
#define READ_SERDES65LP_FX100_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x11, \
                                    (_val))
#define WRITE_SERDES65LP_FX100_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x11, \
                                    (_val))
#define MODIFY_SERDES65LP_FX100_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x11, (_val), (_mask))

/* FXControl 3 Register (Addr 0x12) */
#define READ_SERDES65LP_FX100_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x12, \
                                    (_val))
#define WRITE_SERDES65LP_FX100_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x12, \
                                    (_val))
#define MODIFY_SERDES65LP_FX100_CTRL3r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x12, (_val), (_mask))

/* FXStatus 1 Register (Addr 0x13) */
#define READ_SERDES65LP_FX100_STAT1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x13, \
                                    (_val))
#define WRITE_SERDES65LP_FX100_STAT1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x13, \
                                    (_val))
#define MODIFY_SERDES65LP_FX100_STAT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x13, (_val), (_mask))

/* FXStatus 2 Register (Addr 0x14) */
#define READ_SERDES65LP_FX100_STAT2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x14, \
                                    (_val))
#define WRITE_SERDES65LP_FX100_STAT2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x14, \
                                    (_val))
#define MODIFY_SERDES65LP_FX100_STAT2r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x14, (_val), (_mask))

/* FXStatus 2 Register (Addr 0x14) */
#define READ_SERDES65LP_FX100_STAT2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x14, \
                                    (_val))
#define WRITE_SERDES65LP_FX100_STAT2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x14, \
                                    (_val))
#define MODIFY_SERDES65LP_FX100_STAT2r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x14, (_val), (_mask))

/* FXStatus 3 Register (Addr 0x14) */
#define READ_SERDES65LP_FX100_STAT3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x14, \
                                    (_val))
#define WRITE_SERDES65LP_FX100_STAT3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x14, \
                                    (_val))
#define MODIFY_SERDES65LP_FX100_STAT3r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x14, (_val), (_mask))

/* FXStatus 4 Register (Addr 0x14) */
#define READ_SERDES65LP_FX100_STAT4r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x14, \
                                    (_val))
#define WRITE_SERDES65LP_FX100_STAT4r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x14, \
                                    (_val))
#define MODIFY_SERDES65LP_FX100_STAT4r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x14, (_val), (_mask))

/* 100FX Control 1 Register Fields */
#define LP_FX100_CONTROL1_EN           (1 << 0) /* Enable 65LP */
#define LP_FX100_CONTROL1_FD           (1 << 1) /* 65LP full duplex */
#define LP_FX100_CONTROL1_AUTO_DET     (1 << 2) /* Auto detect
                                                            65LP/1000X */
#define LP_FX100_CTRL1_PWRDWN_WAKEUP_250ms   (1 << 14)
#define LP_FX100_CTRL1_PWRDWN_PERIOD_3SEC    (1 << 13)
#define LP_FX100_CTRL1_AUTO_PWRDWN_EN        (1 << 12)


/* 100FX Control 2 Register Fields */
#define LP_FX100_CONTROL2_EXT_PKT_SZ   (1 << 0) /* Allow pkt sz > 9k*/

/* 100FX Control 3 Register Fields */
#define LP_FX100_CONTROL3_FAST_TIMER   (1 << 0)

/************************/
/* Analog Block (Blk 3) */
/************************/
/* Analog Tx 1 (Addr 10h) */
#define READ_SERDES65LP_ANALOG_TX1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x10, (_val))
#define WRITE_SERDES65LP_ANALOG_TX1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x10, (_val))
#define MODIFY_SERDES65LP_ANALOG_TX1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0003, 0x10, \
                             (_val), (_mask))

/* Analog Tx 2 (Addr 11h) */
#define READ_SERDES65LP_ANALOG_TX2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x11, (_val))
#define WRITE_SERDES65LP_ANALOG_TX2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x11, (_val))
#define MODIFY_SERDES65LP_ANALOG_TX2r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0003, 0x11, \
                             (_val), (_mask))

/* Analog Tx Amp (Addr 12h) */
#define READ_SERDES65LP_ANALOG_TX_AMPr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x12, (_val))
#define WRITE_SERDES65LP_ANALOG_TX_AMPr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x12, (_val))
#define MODIFY_SERDES65LP_ANALOG_TX_AMPr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0003, 0x12, \
                             (_val), (_mask))


/* Analog RX1 (Addr 13h) */
#define READ_SERDES65LP_ANALOG_RX1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x13, (_val))
#define WRITE_SERDES65LP_ANALOG_RX1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x13, (_val))
#define MODIFY_SERDES65LP_ANALOG_RX1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0003, 0x13, \
                             (_val), (_mask))

/* Analog RX2 (Addr 14h) */
#define READ_SERDES65LP_ANALOG_RX2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x14, (_val))
#define WRITE_SERDES65LP_ANALOG_RX2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x14, (_val))
#define MODIFY_SERDES65LP_ANALOG_RX2r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0003, 0x14, \
                             (_val), (_mask))

/* Analog RX3 (Addr 15h) */
#define READ_SERDES65LP_ANALOG_RX3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x15, (_val))
#define WRITE_SERDES65LP_ANALOG_RX3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x15, (_val))
#define MODIFY_SERDES65LP_ANALOG_RX3r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0003, 0x15, \
                             (_val), (_mask))

/* Analog RX4 (Addr 16h) */
#define READ_SERDES65LP_ANALOG_RX4r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x16, (_val))
#define WRITE_SERDES65LP_ANALOG_RX4r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x16, (_val))
#define MODIFY_SERDES65LP_ANALOG_RX4r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0003, 0x16, \
                             (_val), (_mask))

/* Analog PLL 1 (Addr 18h) */
#define READ_SERDES65LP_ANALOG_PLL1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x18, (_val))
#define WRITE_SERDES65LP_ANALOG_PLL1r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x18, (_val))
#define MODIFY_SERDES65LP_ANALOG_PLL1r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0003, 0x18, \
                             (_val), (_mask))

/* Analog PLL 2 (Addr 19h) */
#define READ_SERDES65LP_ANALOG_PLL2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x19, (_val))
#define WRITE_SERDES65LP_ANALOG_PLL2r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x19, (_val))
#define MODIFY_SERDES65LP_ANALOG_PLL2r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0003, 0x19, \
                             (_val), (_mask))

/* Analog PLL 3 (Addr 1Ah) */
#define READ_SERDES65LP_ANALOG_PLL3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x1A, (_val))
#define WRITE_SERDES65LP_ANALOG_PLL3r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x1A, (_val))
#define MODIFY_SERDES65LP_ANALOG_PLL3r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0003, 0x1A, \
                             (_val), (_mask))

/* Analog PLL 4 (Addr 1Bh) */
#define READ_SERDES65LP_ANALOG_PLL4r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x1B, (_val))
#define WRITE_SERDES65LP_ANALOG_PLL4r(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x1B, (_val))
#define MODIFY_SERDES65LP_ANALOG_PLL4r(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0003, 0x1B, \
                             (_val), (_mask))

/* Analog Misc (Addr 1Ch) */
#define READ_SERDES65LP_ANALOG_MISCr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x1C, (_val))
#define WRITE_SERDES65LP_ANALOG_MISCr(_unit, _phy_ctrl, _val) \
            SERDES65LP_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0003, 0x1C, (_val))
#define MODIFY_SERDES65LP_ANALOG_MISCr(_unit, _phy_ctrl, _val, _mask) \
            SERDES65LP_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0003, 0x1C, \
                             (_val), (_mask))

#endif /* _SERDES_65LP_H_ */
