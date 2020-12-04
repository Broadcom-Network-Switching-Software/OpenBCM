/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        serdescombo.h
 * Purpose:     BCM5621X (Raptor) Internal Combo Port PHY Driver
 *              10/100/1000/2500
 */

#ifndef   _SERDES_COMBO_H_
#define   _SERDES_COMBO_H_

#include <soc/phy.h>
#include "phyreg.h"

/*
 * Definitions for Internal SerDes 10/100/1000 PHY
 */

/* MII Control Register */

/* COMBO SERDES: MII Control bit definitions, same as Standard 10/100 PHY */
#define COMBO_MII_CTRL_RESET      MII_CTRL_RESET   /* Self clearing SW reset */
#define COMBO_MII_CTRL_SS_LSB     MII_CTRL_SS_LSB  /* Speed Select, LSB */
#define COMBO_MII_CTRL_SS_MSB     MII_CTRL_SS_MSB  /* Speed Select, MSB */
#define COMBO_MII_CTRL_SS(_x)     MII_CTRL_SS(_x)
#define COMBO_MII_CTRL_SS_10      MII_CTRL_SS_10
#define COMBO_MII_CTRL_SS_100     MII_CTRL_SS_100
#define COMBO_MII_CTRL_SS_1000    MII_CTRL_SS_1000
#define COMBO_MII_CTRL_SS_INVALID MII_CTRL_SS_INVALID
#define COMBO_MII_CTRL_SS_MASK    MII_CTRL_SS_MASK
#define COMBO_MII_CTRL_FD         MII_CTRL_FD      /* Full Duplex */
#define COMBO_MII_CTRL_AN_RESTART (1 << 9)         /* Auto Negotiation Enable */
#define COMBO_MII_CTRL_AN_ENABLE  (1 << 12)        /* Auto Negotiation Enable */
#define COMBO_MII_CTRL_PD         MII_CTRL_PD      /* Power Down Enable */
#define COMBO_MII_CTRL_LOOPBACK   (1 << 14)        /* Loopback Enable */

/* MII Status Register */
/* COMBO: MII Status Register: not the same as standard 10/100 PHY */
#define COMBO_MII_STAT_RF          MII_STAT_RF     /* Remote fault */
#define COMBO_MII_STAT_LA          MII_STAT_LA     /* Link Up */
#define COMBO_MII_STAT_AN_COMPLETE (1 << 5)        /* AN complete */

/* Bit defines for ANA register */
#define COMBO_MII_ANA_HD         (1 << 6)
#define COMBO_MII_ANA_FD         (1 << 5)
#define COMBO_MII_ANA_PAUSE_NONE (0 << 7)
#define COMBO_MII_ANA_PAUSE_SYM  (1 << 7)
#define COMBO_MII_ANA_PAUSE_ASYM (1 << 8)
#define COMBO_MII_ANA_PAUSE_MASK (3 << 7)

/* Auto-Negotiation Link-Partner Ability Register */
#define COMBO_MII_ANP_FIBER_NEXT_PG    (1 << 15)
#define COMBO_MII_ANP_FIBER_ACK        (1 << 14)
#define COMBO_MII_ANP_FIBER_RF_SHFT    (12)        /* Remote fault */
#define COMBO_MII_ANP_FIBER_RF_MASK    (0x3000)
#define COMBO_MII_ANP_FIBER_PAUSE_ASYM (1 << 8)
#define COMBO_MII_ANP_FIBER_PAUSE_SYM  (1 << 7)
#define COMBO_MII_ANP_FIBER_HD         (1 << 6)
#define COMBO_MII_ANP_FIBER_FD         (1 << 5)

#define COMBO_MII_ANP_SGMII            (1 << 0)
#define COMBO_MII_ANP_SGMII_DUPLEX     (1 << 12)
#define COMBO_MII_ANP_SGMII_SPEED      (3 << 10) 
#define COMBO_MII_ANP_SGMII_SPEED_10   (0) 
#define COMBO_MII_ANP_SGMII_SPEED_100  (1 << 10) 
#define COMBO_MII_ANP_SGMII_SPEED_1000 (1 << 11) 

/* Auto-Negotiation Expansion Register */
#define COMBO_ANA_EXPANSION_PR      (1 << 1)        /* Page received */


/* If application want to use this header file for accessing PHY registers,
 * simply redefine SERDESCOMBO_REG macros to use BCM API as follows.
 *
 * #define SERDESCOMBO_REG_READ(_unit, _port, _flags, _reg_bank, \
 *                               _reg_addr, _val) \
 *            bcm_port_phy_get((_unit), (_port), 0,
 *                 BCM_PORT_PHY_REG_INDIRECT_ADDR(_flags, _reg_bank, _reg_addr),
 *                             (_val))
 *
 * #define SERDESCOMBO_REG_WRITE(_unit, _port, _flags, _reg_bank, \
 *                                _reg_addr, _val) \
 *            bcm_port_phy_set((_unit), (_port), 0,
 *                 BCM_PORT_PHY_REG_INDIRECT_ADDR(_flags, _reg_bank, _reg_addr),
 *                             (_val))
 *
 * #define SERDESCOMBO_REG_MODIFY(_unit, _port, _flags, _reg_bank, \
 *                                 _reg_addr, _val)\
 *            do { \
 *                return BCM_E_UNAVAIL; \
 *            } while(0)
 */

/* SERDES register access */
#define SERDESCOMBO_REG_READ(_unit, _phy_ctrl, _flags, _reg_bank, \
                          _reg_addr, _val) \
            phy_reg_serdes_read((_unit), (_phy_ctrl), (_reg_bank), \
                            (_reg_addr), (_val))
#define SERDESCOMBO_REG_WRITE(_unit, _phy_ctrl, _flags, _reg_bank, \
                          _reg_addr, _val) \
            phy_reg_serdes_write((_unit), (_phy_ctrl), (_reg_bank), \
                             (_reg_addr), (_val))
#define SERDESCOMBO_REG_MODIFY(_unit, _phy_ctrl, _flags, _reg_bank, \
                            _reg_addr, _val, _mask) \
            phy_reg_serdes_modify((_unit), (_phy_ctrl), (_reg_bank), \
                              (_reg_addr), (_val), (_mask))

/***************************/
/* Standard IEEE Registers */
/***************************/

/* MII Control (Addr 00h) */
#define READ_SERDESCOMBO_MII_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x00, (_val))
#define WRITE_SERDESCOMBO_MII_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x00, (_val))
#define MODIFY_SERDESCOMBO_MII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, \
                               (_val), (_mask))

/* MII Status (Addr 01h) */
#define READ_SERDESCOMBO_MII_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x01, (_val))
#define WRITE_SERDESCOMBO_MII_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x01, (_val))
#define MODIFY_SERDESCOMBO_MII_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, \
                               (_val), (_mask))

/* PHY ID 0 (Addr 02h) */
#define READ_SERDESCOMBO_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x02, (_val))
#define WRITE_SERDESCOMBO_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x02, (_val))
#define MODIFY_SERDESCOMBO_MII_PHY_ID0r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, \
                             (_val), (_mask))

/* PHY ID 1 (Addr 03h) */
#define READ_SERDESCOMBO_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x03, (_val))
#define WRITE_SERDESCOMBO_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x03, (_val))
#define MODIFY_SERDESCOMBO_MII_PHY_ID1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, \
                             (_val), (_mask))

/* Autoneg Adv (Addr 04h) */
#define READ_SERDESCOMBO_MII_ANAr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x04, (_val))
#define WRITE_SERDESCOMBO_MII_ANAr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x04, (_val))
#define MODIFY_SERDESCOMBO_MII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, \
                             (_val), (_mask))

/* Autoneg Link Partner Ability (Addr 05h) */
#define READ_SERDESCOMBO_MII_ANPr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                   0x00, 0x0000, 0x05, (_val))
#define WRITE_SERDESCOMBO_MII_ANPr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                   0x00, 0x0000, 0x05, (_val))
#define MODIFY_SERDESCOMBO_MII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, \
                             (_val), (_mask))

/* Autoneg Expansion (Addr 06h) */
#define READ_SERDESCOMBO_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x06, (_val))
#define WRITE_SERDESCOMBO_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x06, (_val))
#define MODIFY_SERDESCOMBO_MII_AN_EXPr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, \
                             (_val), (_mask))

/* Extended Status (Addr 0fh) */
 
/*************************/
/* Digital Block (Blk 0) */
/*************************/
/* 1000X Control 1 (Addr 10h) */
#define READ_SERDESCOMBO_1000X_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x10, (_val))
#define WRITE_SERDESCOMBO_1000X_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x10, (_val))
#define MODIFY_SERDESCOMBO_1000X_CTRL1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, \
                             (_val), (_mask))

/* 1000X Control 2 (Addr 11h) */
#define READ_SERDESCOMBO_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x11, (_val))
#define WRITE_SERDESCOMBO_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x11, (_val))
#define MODIFY_SERDESCOMBO_1000X_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, \
                             (_val), (_mask))

/* 1000X Control 3 (Addr 12h) */
#define READ_SERDESCOMBO_1000X_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x12, (_val))
#define WRITE_SERDESCOMBO_1000X_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x12, (_val))
#define MODIFY_SERDESCOMBO_1000X_CTRL3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x12, \
                             (_val), (_mask))

/* 1000X Control 4 (Addr 13h) */
#define READ_SERDESCOMBO_1000X_CTRL4r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x13, (_val))
#define WRITE_SERDESCOMBO_1000X_CTRL4r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x13, (_val))
#define MODIFY_SERDESCOMBO_1000X_CTRL4r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x13, \
                             (_val), (_mask))

/* 1000X Status 1 (Addr 14h) */
#define READ_SERDESCOMBO_1000X_STAT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x14, (_val))
#define WRITE_SERDESCOMBO_1000X_STAT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x14, (_val))
#define MODIFY_SERDESCOMBO_1000X_STAT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x14, \
                             (_val), (_mask))

/* 1000X Status 2 (Addr 15h) */
#define READ_SERDESCOMBO_1000X_STAT2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x15, (_val))
#define WRITE_SERDESCOMBO_1000X_STAT2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x15, (_val))
#define MODIFY_SERDESCOMBO_1000X_STAT2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x15, \
                             (_val), (_mask))

/* 1000X Status 3 (Addr 16h) */
#define READ_SERDESCOMBO_1000X_STAT3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x16, (_val))
#define WRITE_SERDESCOMBO_1000X_STAT3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x16, (_val))
#define MODIFY_SERDESCOMBO_1000X_STAT3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x16, \
                             (_val), (_mask))

/* Ber Crc Err Rx Pkt cntr (Addr 17h) */
#define READ_SERDESCOMBO_1000X_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x17, (_val))
#define WRITE_SERDESCOMBO_1000X_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x17, (_val))
#define MODIFY_SERDESCOMBO_1000X_PKT_COUNTERr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x17, \
                             (_val), (_mask))

/* Prbs Control (Addr 18h) */
#define READ_SERDESCOMBO_1000X_PRBS_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x18, (_val))
#define WRITE_SERDESCOMBO_1000X_PRBS_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x18, (_val))
#define MODIFY_SERDESCOMBO_1000X_PRBS_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, \
                             (_val), (_mask))

/* Prbs Status (Addr 19h) */
#define READ_SERDESCOMBO_1000X_PRBS_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x19, (_val))
#define WRITE_SERDESCOMBO_1000X_PRBS_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x19, (_val))
#define MODIFY_SERDESCOMBO_1000X_PRBS_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x19, \
                             (_val), (_mask))

/* Pat Gen Control (Addr 1Ah) */
#define READ_SERDESCOMBO_1000X_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1a, (_val))
#define WRITE_SERDESCOMBO_1000X_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1a, (_val))
#define MODIFY_SERDESCOMBO_1000X_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1a, \
                             (_val), (_mask))

/* Pat Gen Status (Addr 1Bh) */
#define READ_SERDESCOMBO_1000X_PAT_GEN_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1b, (_val))
#define WRITE_SERDESCOMBO_1000X_PAT_GEN_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1b, (_val))
#define MODIFY_SERDESCOMBO_1000X_PAT_GEN_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1b, \
                             (_val), (_mask))

/* Test Mode (Addr 1Ch) */
#define READ_SERDESCOMBO_1000X_TEST_MODEr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1c, (_val))
#define WRITE_SERDESCOMBO_1000X_TEST_MODEr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1c, (_val))
#define MODIFY_SERDESCOMBO_1000X_TEST_MODEr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1c, \
                             (_val), (_mask))

/* Force Tx Data1 (Addr 1Dh) */
#define READ_SERDESCOMBO_1000X_FORCE_TX_DATA1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1d, (_val))
#define WRITE_SERDESCOMBO_1000X_FORCE_TX_DATA1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1d, (_val))
#define MODIFY_SERDESCOMBO_1000X_FORCE_TX_DATA1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1d, \
                             (_val), (_mask))

/* Force Tx Data2 (Addr 1Eh) */
#define READ_SERDESCOMBO_1000X_FORCE_TX_DATA2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1e, (_val))
#define WRITE_SERDESCOMBO_1000X_FORCE_TX_DATA2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1e, (_val))
#define MODIFY_SERDESCOMBO_1000X_FORCE_TX_DATA2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, \
                             (_val), (_mask))

/* 1000X Control #1 Register: Controls 10B/SGMII mode */
#define COMBO_1000X_FIBER_MODE    (1 << 0)     /* Enable SGMII fiber mode */
#define COMBO_1000X_EN10B_MODE    (1 << 1)     /* Enable TBI 10B interface */
#define COMBO_1000X_INVERT_SD     (1 << 3)     /* Invert Signal Detect */
#define COMBO_1000X_AUTO_DETECT   (1 << 4)     /* Auto-detect SGMII and 1000X */ 
#define COMBO_1000X_RX_PKTS_CNTR_SEL (1 << 11) /* Select receive counter for 17h*/
#define COMBO_1000X_TX_AMPLITUDE_OVRD    (1 << 12)

/* 1000X Control #2 Register Fields */
#define COMBO_1000X_PAR_DET_EN     (1 << 0)     /* Enable Parallel Detect */
#define COMBO_1000X_FALSE_LNK_DIS  (1 << 1)     /* Disable false link */
#define COMBO_1000X_FLT_FORCE_EN   (1 << 2)     /* Enable filter force link */
#define COMBO_1000X_CLRAR_BER_CNTR (1 << 14)    /* Clear bit-err-rate counter */

/* 1000X Control #3 Register Fields */
#define COMBO_1000X_TX_FIFO_RST           (1 << 0)    /* Reset TX FIFO */
#define COMBO_1000X_FIFO_ELASTICITY_MASK  (0x3 << 1)  /* Fifo Elasticity */
#define COMBO_1000X_FIFO_ELASTICITY_5K    (0x0 << 1)  /* 5 Kbytes */
#define COMBO_1000X_FIFO_ELASTICITY_10K   (0x1 << 1)  /* 10 Kbytes */
#define COMBO_1000X_FIFO_ELASTICITY_13_5K (0x2 << 1)  /* 13.5 Kbytes */
#define COMBO_1000X_RX_FIFO_RST           (1 << 14)   /* Reset RX FIFO */

/* 1000X Control #4 Register Fields */
#define COMBO_1000X_DIG_RESET             (1 << 6)    /* Reset Datapath */

/* 1000X Status #1 Register Fields */
#define COMBO_1000X_STATUS1_SGMII_MODE    (1 << 0)
#define COMBO_1000X_STATUS1_SPEED         (3 << 3)
#define COMBO_1000X_STATUS1_SPEED_10      (0)
#define COMBO_1000X_STATUS1_SPEED_100     (1 << 3)
#define COMBO_1000X_STATUS1_SPEED_1000    (1 << 4)


/************************/
/* Analog Block (Blk 1) */
/************************/
/* Analog Tx (Addr 10h) */
#define READ_SERDESCOMBO_ANALOG_TXr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x10, (_val))
#define WRITE_SERDESCOMBO_ANALOG_TXr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x10, (_val))
#define MODIFY_SERDESCOMBO_ANALOG_TXr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x10, \
                             (_val), (_mask))

/* Analog RX1 (Addr 11h) */
#define READ_SERDESCOMBO_ANALOG_RX1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x11, (_val))
#define WRITE_SERDESCOMBO_ANALOG_RX1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x11, (_val))
#define MODIFY_SERDESCOMBO_ANALOG_RX1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x11, \
                             (_val), (_mask))

/* Analog RX2 (Addr 12h) */
#define READ_SERDESCOMBO_ANALOG_RX2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x12, (_val))
#define WRITE_SERDESCOMBO_ANALOG_RX2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x12, (_val))
#define MODIFY_SERDESCOMBO_ANALOG_RX2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x12, \
                             (_val), (_mask))

/* Analog PLL (Addr 13h) */
#define READ_SERDESCOMBO_ANALOG_PLLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x13, (_val))
#define WRITE_SERDESCOMBO_ANALOG_PLLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x13, (_val))
#define MODIFY_SERDESCOMBO_ANALOG_PLLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x13, \
                             (_val), (_mask))

/* Analog RX3 (Addr 14h) */
#define READ_SERDESCOMBO_ANALOG_RX3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x14, (_val))
#define WRITE_SERDESCOMBO_ANALOG_RX3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x14, (_val))
#define MODIFY_SERDESCOMBO_ANALOG_RX3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x14, \
                             (_val), (_mask))

/* TPOUT1 (Addr 18h) */
#define READ_SERDESCOMBO_ANALOG_TPOUT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x18, (_val))
#define WRITE_SERDESCOMBO_ANALOG_TPOUT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x18, (_val))
#define MODIFY_SERDESCOMBO_ANALOG_TPOUT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, \
                             (_val), (_mask))

/* TPOUT2 (Addr 19h) */
#define READ_SERDESCOMBO_ANALOG_TPOUT2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x19, (_val))
#define WRITE_SERDESCOMBO_ANALOG_TPOUT2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x19, (_val))
#define MODIFY_SERDESCOMBO_ANALOG_TPOUT2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x19, \
                             (_val), (_mask))

/* Analog Rx 1 Register Fields */
#define COMBO_ANALOG_RX1_IDDQ             (1 << 0)  
#define COMBO_ANALOG_RX1_RESET            (1 << 1)
#define COMBO_ANALOG_RX1_EDGE_SEL         (1 << 8) 
#define COMBO_ANALOG_RX1_SIG_DET          (1 << 10)

/*****************************/
/* Digital 3 Block (Block 2) */
/*****************************/
/* Digital3 ContCTRL0rol0 Reg (Addr 0x10) */
#define READ_SERDESCOMBO_DIGI3_CTRL0r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0002, 0x10, (_val))
#define WRITE_SERDESCOMBO_DIGI3_CTRL0r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0002, 0x10, (_val))
#define MODIFY_SERDESCOMBO_DIGI3_CTRL0r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x10, (_val), (_mask))

/* Digital3 Control1 Reg (Addr 0x11) */
#define READ_SERDESCOMBO_DIGI3_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0002, 0x11, (_val))
#define WRITE_SERDESCOMBO_DIGI3_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0002, 0x11, (_val))
#define MODIFY_SERDESCOMBO_DIGI3_CTRL1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x11, (_val), (_mask))

/* Digital3 Control2 Reg (Addr 0x12) */
#define READ_SERDESCOMBO_DIGI3_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0002, 0x12, (_val))
#define WRITE_SERDESCOMBO_DIGI3_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0002, 0x12, (_val))
#define MODIFY_SERDESCOMBO_DIGI3_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x12, (_val), (_mask))

/* Digital3 Control3 Reg (Addr 0x13) */
#define READ_SERDESCOMBO_DIGI3_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0002, 0x13, (_val))
#define WRITE_SERDESCOMBO_DIGI3_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0002, 0x13, (_val))
#define MODIFY_SERDESCOMBO_DIGI3_CTRL3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x13, (_val), (_mask))

/* Digital3 Control4 Reg (Addr 0x14) */
#define READ_SERDESCOMBO_DIGI3_CTRL4r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0002, 0x14, (_val))
#define WRITE_SERDESCOMBO_DIGI3_CTRL4r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0002, 0x14, (_val))
#define MODIFY_SERDESCOMBO_DIGI3_CTRL4r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x14, (_val), (_mask))

/* Digital3 ControlB Reg (Addr 0x1B) */
#define READ_SERDESCOMBO_DIGI3_CTRLBr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0002, 0x1b, (_val))
#define WRITE_SERDESCOMBO_DIGI3_CTRLBr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0002, 0x1b, (_val))
#define MODIFY_SERDESCOMBO_DIGI3_CTRLBr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x1b, (_val), (_mask))

/* Digital3 ControlC Reg (Addr 0x1C) */
#define READ_SERDESCOMBO_DIGI3_CTRLCr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0002, 0x1c, (_val))
#define WRITE_SERDESCOMBO_DIGI3_CTRLCr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0002, 0x1c, (_val))
#define MODIFY_SERDESCOMBO_DIGI3_CTRLCr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x1c, (_val), (_mask))

/* Digital3 ControlD Reg (Addr 0x1D) */
#define READ_SERDESCOMBO_DIGI3_CTRLDr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0002, 0x1d, (_val))
#define WRITE_SERDESCOMBO_DIGI3_CTRLDr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0002, 0x1d, (_val))
#define MODIFY_SERDESCOMBO_DIGI3_CTRLDr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x1d, (_val), (_mask))

/* Digital3 ControlE REG (Addr 0x1E) */
#define READ_SERDESCOMBO_DIGI3_CTRLEr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0002, 0x1e, (_val))
#define WRITE_SERDESCOMBO_DIGI3_CTRLEr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0002, 0x1e, (_val))
#define MODIFY_SERDESCOMBO_DIGI3_CTRLEr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0002, 0x1e, (_val), (_mask))

#define COMBO_DIGI3_CTRLB_OVER_1G  (1 << 0)
#define COMBO_DIGI3_CTRLD_OVER_1G  (1 << 0)

/***********************/
/* PLL Block (Block 3) */
/***********************/
/* PLL STATUS Reg (Addr 0x10) */
#define READ_SERDESCOMBO_PLL_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x10, (_val))
#define WRITE_SERDESCOMBO_PLL_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x10, (_val))
#define MODIFY_SERDESCOMBO_PLL_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x10, (_val), (_mask))

/* PLL CONTROL Reg (Addr 0x11) */
#define READ_SERDESCOMBO_PLL_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x11, (_val))
#define WRITE_SERDESCOMBO_PLL_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x11, (_val))
#define MODIFY_SERDESCOMBO_PLL_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x11, (_val), (_mask))

/* PLL TIMER1 Reg (Addr 0x12) */
#define READ_SERDESCOMBO_PLL_TIMER1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x12, (_val))
#define WRITE_SERDESCOMBO_PLL_TIMER1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x12, (_val))
#define MODIFY_SERDESCOMBO_PLL_TIMER1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x12, (_val), (_mask))

/* PLL TIMER2 Reg (Addr 0x13) */
#define READ_SERDESCOMBO_PLL_TIMER2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x13, (_val))
#define WRITE_SERDESCOMBO_PLL_TIMER2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x13, (_val))
#define MODIFY_SERDESCOMBO_PLL_TIMER2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x13, (_val), (_mask))

/* PLL TIMER3 Reg (Addr 0x14) */
#define READ_SERDESCOMBO_PLL_TIMER3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x14, (_val))
#define WRITE_SERDESCOMBO_PLL_TIMER3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x14, (_val))
#define MODIFY_SERDESCOMBO_PLL_TIMER3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x14, (_val), (_mask))

/* PLL CAP CONTROL Reg (Addr 0x15) */
#define READ_SERDESCOMBO_PLL_CAP_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x15, (_val))
#define WRITE_SERDESCOMBO_PLL_CAP_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x15, (_val))
#define MODIFY_SERDESCOMBO_PLL_CAP_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x15, (_val), (_mask))

/* PLL AMP CONTROL Reg (Addr 0x16) */
#define READ_SERDESCOMBO_PLL_AMP_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x16, (_val))
#define WRITE_SERDESCOMBO_PLL_AMP_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x16, (_val))
#define MODIFY_SERDESCOMBO_PLL_AMP_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x16, (_val), (_mask))

/* PLL FREQ DET CNTR Reg (Addr 0x17) */
#define READ_SERDESCOMBO_PLL_FREQ_DET_COUNTERr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x17, (_val))
#define WRITE_SERDESCOMBO_PLL_FREQ_DET_COUNTERr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x17, (_val))
#define MODIFY_SERDESCOMBO_PLL_FREQ_DET_COUNTERr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x17, (_val), (_mask))

/* PLL ASTATUS1 Reg (Addr 0x18) */
#define READ_SERDESCOMBO_PLL_ASTAT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x18, (_val))
#define WRITE_SERDESCOMBO_PLL_ASTAT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x18, (_val))
#define MODIFY_SERDESCOMBO_PLL_ASTAT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x18, (_val), (_mask))

/* PLL ASTATUS2 Reg (Addr 0x19) */
#define READ_SERDESCOMBO_PLL_ASTAT2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x19, (_val))
#define WRITE_SERDESCOMBO_PLL_ASTAT2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x19, (_val))
#define MODIFY_SERDESCOMBO_PLL_ASTAT2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x19, (_val), (_mask))

/* PLL ACONTROL1 Reg (Addr 0x1A) */
#define READ_SERDESCOMBO_PLL_ACTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x1a, (_val))
#define WRITE_SERDESCOMBO_PLL_ACTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x1a, (_val))
#define MODIFY_SERDESCOMBO_PLL_ACTRL1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x1a, (_val), (_mask))

/* PLL ACONTROL2 Reg (Addr 0x1B) */
#define READ_SERDESCOMBO_PLL_ACTRL2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x1b, (_val))
#define WRITE_SERDESCOMBO_PLL_ACTRL2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x1b, (_val))
#define MODIFY_SERDESCOMBO_PLL_ACTRL2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x1b, (_val), (_mask))

/* PLL ACONTROL3 Reg (Addr 0x1C) */
#define READ_SERDESCOMBO_PLL_ACTRL3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x1c, (_val))
#define WRITE_SERDESCOMBO_PLL_ACTRL3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x1c, (_val))
#define MODIFY_SERDESCOMBO_PLL_ACTRL3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x1c, (_val), (_mask))

/* PLL ACONTROL4 Reg (Addr 0x1D) */
#define READ_SERDESCOMBO_PLL_ACTRL4r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0003, 0x1d, (_val))
#define WRITE_SERDESCOMBO_PLL_ACTRL4r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0003, 0x1d, (_val))
#define MODIFY_SERDESCOMBO_PLL_ACTRL4r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0003, 0x1d, (_val), (_mask))

/**********************/
/* RX Block (Block 4) */
/**********************/

/* RX Status Reg (Addr 0x10) */
/* RX Control Reg (Addr 0x11) */
/* RX Timer1 Reg (Addr 0x12) */
/* RX Timer2 Reg (Addr 0x13) */
/* RX Sig Det Reg (Addr 0x14) */
/* RX CDR Phase Reg (Addr 0x15) */
/* RX CDR Freq Reg (Addr 0x16) */
/* RX EQ Config Reg (Addr 0x17) */
/* RX EQ Force Reg (Addr 0x18) */
/* RX Control 1G Reg (Addr 0x19) */
/* RX AStatus Reg (Addr 0x1A) */
/* RX AControl1 Reg (Addr 0x1B) */
/* RX AControl2 Reg (Addr 0x1C) */
/* RX AControl3 Reg (Addr 0x1D) */
/* RX Acontrol4 Reg (Addr 0x1E) */

/************************/
/* Misc Block (Block 5) */
/************************/
/* Misc 2500X Status Reg (Addr 0x10) */
#define READ_SERDESCOMBO_MISC_2500X_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0005, 0x10, (_val))
#define WRITE_SERDESCOMBO_MISC_2500X_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0005, 0x10, (_val))
#define MODIFY_SERDESCOMBO_MISC_2500X_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0005, 0x10, (_val), (_mask))

/* Reserved (Addr 0x11 - 0x13) */
/* Misc AStatus1 Reg (Addr 0x14) */
#define READ_SERDESCOMBO_MISC_ASTAT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0005, 0x14, (_val))
#define WRITE_SERDESCOMBO_MISC_ASTAT1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0005, 0x14, (_val))
#define MODIFY_SERDESCOMBO_MISC_ASTAT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0005, 0x14, (_val), (_mask))

/* Misc TX AContTX_ACTRL1rol1 Reg (Addr 0x15) */
#define READ_SERDESCOMBO_MISC_TX_ACTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0005, 0x15, (_val))
#define WRITE_SERDESCOMBO_MISC_TX_ACTRL1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0005, 0x15, (_val))
#define MODIFY_SERDESCOMBO_MISC_TX_ACTRL1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0005, 0x15, (_val), (_mask))

/* Misc TX AContTX_ACTRL2rol2 Reg (Addr 0x16) */
#define READ_SERDESCOMBO_MISC_TX_ACTRL2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctTX_ACTRL2rl), \
                                    0x00, 0x0005, 0x16, (_val))
#define WRITE_SERDESCOMBO_MISC_TX_ACTRL2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctTX_ACTRL2rl), \
                                     0x00, 0x0005, 0x16, (_val))
#define MODIFY_SERDESCOMBO_MISC_TX_ACTRL2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctTX_ACTRL2rl), \
                                      0x00, 0x0005, 0x16, (_val), (_mask))

/* Misc TX AContTX_ACTRL3rol3 Reg (Addr 0x17) */
#define READ_SERDESCOMBO_MISC_TX_ACTRL3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0005, 0x17, (_val))
#define WRITE_SERDESCOMBO_MISC_TX_ACTRL3r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0005, 0x17, (_val))
#define MODIFY_SERDESCOMBO_MISC_TX_ACTRL3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0005, 0x17, (_val), (_mask))

/* Reserved (Addr 0x18) */
/* Misc CAP ContCAP_CTRLrols Reg (Addr 0x19) */
#define READ_SERDESCOMBO_MISC_CAP_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0005, 0x19, (_val))
#define WRITE_SERDESCOMBO_MISC_CAP_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0005, 0x19, (_val))
#define MODIFY_SERDESCOMBO_MISC_CAP_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0005, 0x19, (_val), (_mask))

/* Misc AMP Controls Reg (Addr 0x1A) */
#define READ_SERDESCOMBO_MISC_AMP_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0005, 0x1a, (_val))
#define WRITE_SERDESCOMBO_MISC_AMP_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0005, 0x1a, (_val))
#define MODIFY_SERDESCOMBO_MISC_AMP_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0005, 0x1a, (_val), (_mask))

/* Misc RX Status Reg (Addr 0x1B) */
#define READ_SERDESCOMBO_MISC_RX_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0005, 0x1b, (_val))
#define WRITE_SERDESCOMBO_MISC_RX_STATr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0005, 0x1b, (_val))
#define MODIFY_SERDESCOMBO_MISC_RX_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0005, 0x1b, (_val), (_mask))

/* Misc IF Reg (Addr 0x1C) */
#define READ_SERDESCOMBO_MISC_IFr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0005, 0x1c, (_val))
#define WRITE_SERDESCOMBO_MISC_IFr(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0005, 0x1c, (_val))
#define MODIFY_SERDESCOMBO_MISC_IFr(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0005, 0x1c, (_val), (_mask))

/* Misc Misc1 Reg (Addr 0x1D) */
#define READ_SERDESCOMBO_MISC_MISC1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0005, 0x1d, (_val))
#define WRITE_SERDESCOMBO_MISC_MISC1r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0005, 0x1d, (_val))
#define MODIFY_SERDESCOMBO_MISC_MISC1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0005, 0x1d, (_val), (_mask))

/* Misc Misc2 Reg (Addr 0x1E) */
#define READ_SERDESCOMBO_MISC_MISC2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_READ((_unit), (_phy_ctrl), \
                                    0x00, 0x0005, 0x1e, (_val))
#define WRITE_SERDESCOMBO_MISC_MISC2r(_unit, _phy_ctrl, _val) \
            SERDESCOMBO_REG_WRITE((_unit), (_phy_ctrl), \
                                     0x00, 0x0005, 0x1e, (_val))
#define MODIFY_SERDESCOMBO_MISC_MISC2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESCOMBO_REG_MODIFY((_unit), (_phy_ctrl), \
                                      0x00, 0x0005, 0x1e, (_val), (_mask))

#define COMBO_MISC_MISC2_PLL_MODE_DIGITAL (1 << 1) /* (0)1.25Ghz, (1)3.125Ghz */
#define COMBO_MISC_MISC2_PLL_MODE_ANALOG  (1 << 0) /* (0)1.25Ghz, (1)3.125Ghz */

#define PHY_SERDESCOMBO_STOP_MAC_DIS        (0x01)
#define PHY_SERDESCOMBO_STOP_PHY_DIS        (0x02)
#define PHY_SERDESCOMBO_STOP_DRAIN          (0x04)
#define PHY_SERDESCOMBO_STOP_DUPLEX_CHG     (0x08)
#define PHY_SERDESCOMBO_STOP_SPEED_CHG      (0x10)
#define PHY_SERDESCOMBO_STOP_COPPER         (0x20)

#endif /* _SERDES_COMBO_H_ */
