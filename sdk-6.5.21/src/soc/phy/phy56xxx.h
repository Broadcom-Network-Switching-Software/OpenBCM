/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        serdesdozen.h
 * Purpose:     BCM56xxx (FB/ER) Internal Dodeca Serdes PHY Driver
 */

#ifndef   _SERDES_DOZEN_H_
#define   _SERDES_DOZEN_H_

#include <soc/phy.h>

/*
 * Definitions for Internal SerDes 10/100/1000 PHY
 */

/* MII Control Register */

/* DDS: MII Control bit definitions, same as Standard 10/100 PHY */
#define DDS_MII_CTRL_RESET      MII_CTRL_RESET   /* Self clearing SW reset */
#define DDS_MII_CTRL_SS_LSB     MII_CTRL_SS_LSB  /* Speed Select, LSB */
#define DDS_MII_CTRL_SS_MSB     MII_CTRL_SS_MSB  /* Speed Select, MSB */
#define DDS_MII_CTRL_SS(_x)     MII_CTRL_SS(_x)
#define DDS_MII_CTRL_SS_10      MII_CTRL_SS_10
#define DDS_MII_CTRL_SS_100     MII_CTRL_SS_100
#define DDS_MII_CTRL_SS_1000    MII_CTRL_SS_1000
#define DDS_MII_CTRL_SS_INVALID MII_CTRL_SS_INVALID
#define DDS_MII_CTRL_SS_MASK    MII_CTRL_SS_MASK
#define DDS_MII_CTRL_FD         MII_CTRL_FD      /* Full Duplex */
#define DDS_MII_CTRL_AN_RESTART (1 << 9)         /* Auto Negotiation Enable */
#define DDS_MII_CTRL_AN_ENABLE  (1 << 12)        /* Auto Negotiation Enable */
#define DDS_MII_CTRL_PD         MII_CTRL_PD      /* Power Down Enable */
#define DDS_MII_CTRL_LOOPBACK   (1 << 14)        /* Loopback Enable */

/* MII Status Register */
/* DDS: MII Status Register: not the same as standard 10/100 PHY */
#define DDS_MII_STAT_RF          MII_STAT_RF     /* Remote fault */
#define DDS_MII_STAT_LA          MII_STAT_LA     /* Link Up */
#define DDS_MII_STAT_AN_COMPLETE (1 << 5)        /* AN complete */

/* Bit defines for ANA register */
#define DDS_MII_ANA_HD         (1 << 6)
#define DDS_MII_ANA_FD         (1 << 5)
#define DDS_MII_ANA_PAUSE_NONE (0 << 7)
#define DDS_MII_ANA_PAUSE_SYM  (1 << 7)
#define DDS_MII_ANA_PAUSE_ASYM (1 << 8)
#define DDS_MII_ANA_PAUSE_MASK (3 << 7)

/* Auto-Negotiation Link-Partner Ability Register */

#define DDS_MII_ANP_FIBER_NEXT_PG    (1 << 15)
#define DDS_MII_ANP_FIBER_ACK        (1 << 14)
#define DDS_MII_ANP_FIBER_RF_SHFT    (12)        /* Remote fault */
#define DDS_MII_ANP_FIBER_RF_MASK    (0x3000)
#define DDS_MII_ANP_FIBER_PAUSE_ASYM (1 << 8)
#define DDS_MII_ANP_FIBER_PAUSE_SYM  (1 << 7)
#define DDS_MII_ANP_FIBER_HD         (1 << 6)
#define DDS_MII_ANP_FIBER_FD         (1 << 5)

#define DDS_MII_ANP_SGMII            (1 << 0)
#define DDS_MII_ANP_SGMII_DUPLEX     (1 << 12)
#define DDS_MII_ANP_SGMII_SPEED      (3 << 10) 
#define DDS_MII_ANP_SGMII_SPEED_10   (0) 
#define DDS_MII_ANP_SGMII_SPEED_100  (1 << 10) 
#define DDS_MII_ANP_SGMII_SPEED_1000 (1 << 11) 

/* Auto-Negotiation Expansion Register */
#define DDS_ANA_EXPANSION_PR      (1 << 1)        /* Page received */


/* If application want to use this header file for accessing PHY registers,
 * simply redefine SERDESDDS_REG macros to use BCM API as follows.
 *
 * #define SERDESDDS_REG_READ(_unit, _port, _flags, _reg_bank, \
 *                               _reg_addr, _val) \
 *            bcm_port_phy_get((_unit), (_port), 0,
 *                 BCM_PORT_PHY_REG_INDIRECT_ADDR(_flags, _reg_bank, _reg_addr),
 *                             (_val))
 *
 * #define SERDESDDS_REG_WRITE(_unit, _port, _flags, _reg_bank, \
 *                                _reg_addr, _val) \
 *            bcm_port_phy_set((_unit), (_port), 0,
 *                 BCM_PORT_PHY_REG_INDIRECT_ADDR(_flags, _reg_bank, _reg_addr),
 *                             (_val))
 *
 * #define SERDESDDS_REG_MODIFY(_unit, _port, _flags, _reg_bank, \
 *                                 _reg_addr, _val)\
 *            do { \
 *                return BCM_E_UNAVAIL; \
 *            } while(0)
 */

/* SERDES register access */
#define SERDESDDS_REG_READ(_unit, _phy_ctrl, _flags, _reg_bank, \
                          _reg_addr, _val) \
            phy_reg_serdes_read((_unit), (_phy_ctrl), (_reg_bank), \
                            (_reg_addr), (_val))
#define SERDESDDS_REG_WRITE(_unit, _phy_ctrl, _flags, _reg_bank, \
                          _reg_addr, _val) \
            phy_reg_serdes_write((_unit), (_phy_ctrl), (_reg_bank), \
                             (_reg_addr), (_val))
#define SERDESDDS_REG_MODIFY(_unit, _phy_ctrl, _flags, _reg_bank, \
                            _reg_addr, _val, _mask) \
            phy_reg_serdes_modify((_unit), (_phy_ctrl), (_reg_bank), \
                              (_reg_addr), (_val), (_mask))

/***************************/
/* Standard IEEE Registers */
/***************************/

/* MII Control (Addr 00h) */
#define READ_SERDESDDS_MII_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x00, (_val))
#define WRITE_SERDESDDS_MII_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x00, (_val))
#define MODIFY_SERDESDDS_MII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, \
                               (_val), (_mask))

/* MII Status (Addr 01h) */
#define READ_SERDESDDS_MII_STATr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x01, (_val))
#define WRITE_SERDESDDS_MII_STATr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x01, (_val))
#define MODIFY_SERDESDDS_MII_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, \
                               (_val), (_mask))

/* PHY ID 0 (Addr 02h) */
#define READ_SERDESDDS_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x02, (_val))
#define WRITE_SERDESDDS_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x02, (_val))
#define MODIFY_SERDESDDS_MII_PHY_ID0r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, \
                             (_val), (_mask))

/* PHY ID 1 (Addr 03h) */
#define READ_SERDESDDS_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x03, (_val))
#define WRITE_SERDESDDS_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x03, (_val))
#define MODIFY_SERDESDDS_MII_PHY_ID1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, \
                             (_val), (_mask))

/* Autoneg Adv (Addr 04h) */
#define READ_SERDESDDS_MII_ANAr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x04, (_val))
#define WRITE_SERDESDDS_MII_ANAr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x04, (_val))
#define MODIFY_SERDESDDS_MII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, \
                             (_val), (_mask))

/* Autoneg Link Partner Ability (Addr 05h) */
#define READ_SERDESDDS_MII_ANPr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                   0x00, 0x0000, 0x05, (_val))
#define WRITE_SERDESDDS_MII_ANPr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                   0x00, 0x0000, 0x05, (_val))
#define MODIFY_SERDESDDS_MII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, \
                             (_val), (_mask))

/* Autoneg Expansion (Addr 06h) */
#define READ_SERDESDDS_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x06, (_val))
#define WRITE_SERDESDDS_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x06, (_val))
#define MODIFY_SERDESDDS_MII_AN_EXPr(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, \
                             (_val), (_mask))

/* Extended Status (Addr 0fh) */
 
/*************************/
/* Digital Block (Blk 0) */
/*************************/
/* 1000X Control 1 (Addr 10h) */
#define READ_SERDESDDS_1000X_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x10, (_val))
#define WRITE_SERDESDDS_1000X_CTRL1r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x10, (_val))
#define MODIFY_SERDESDDS_1000X_CTRL1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, \
                             (_val), (_mask))

/* 1000X Control 2 (Addr 11h) */
#define READ_SERDESDDS_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x11, (_val))
#define WRITE_SERDESDDS_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x11, (_val))
#define MODIFY_SERDESDDS_1000X_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, \
                             (_val), (_mask))

/* 1000X Control 3 (Addr 12h) */
#define READ_SERDESDDS_1000X_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x12, (_val))
#define WRITE_SERDESDDS_1000X_CTRL3r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x12, (_val))
#define MODIFY_SERDESDDS_1000X_CTRL3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x12, \
                             (_val), (_mask))

/* 1000X Control 4 (Addr 13h) */
#define READ_SERDESDDS_1000X_CTRL4r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x13, (_val))
#define WRITE_SERDESDDS_1000X_CTRL4r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x13, (_val))
#define MODIFY_SERDESDDS_1000X_CTRL4r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x13, \
                             (_val), (_mask))

/* 1000X Status 1 (Addr 14h) */
#define READ_SERDESDDS_1000X_STAT1r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x14, (_val))
#define WRITE_SERDESDDS_1000X_STAT1r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x14, (_val))
#define MODIFY_SERDESDDS_1000X_STAT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x14, \
                             (_val), (_mask))

/* 1000X Status 2 (Addr 15h) */
#define READ_SERDESDDS_1000X_STAT2r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x15, (_val))
#define WRITE_SERDESDDS_1000X_STAT2r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x15, (_val))
#define MODIFY_SERDESDDS_1000X_STAT2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x15, \
                             (_val), (_mask))

/* 1000X Status 3 (Addr 16h) */
#define READ_SERDESDDS_1000X_STAT3r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x16, (_val))
#define WRITE_SERDESDDS_1000X_STAT3r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x16, (_val))
#define MODIFY_SERDESDDS_1000X_STAT3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x16, \
                             (_val), (_mask))

/* Ber Crc Err Rx Pkt cntr (Addr 17h) */
#define READ_SERDESDDS_1000X_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x17, (_val))
#define WRITE_SERDESDDS_1000X_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x17, (_val))
#define MODIFY_SERDESDDS_1000X_PKT_COUNTERr(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x17, \
                             (_val), (_mask))

/* Prbs Control (Addr 18h) */
#define READ_SERDESDDS_1000X_PRBS_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x18, (_val))
#define WRITE_SERDESDDS_1000X_PRBS_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x18, (_val))
#define MODIFY_SERDESDDS_1000X_PRBS_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, \
                             (_val), (_mask))

/* Prbs Status (Addr 19h) */
#define READ_SERDESDDS_1000X_PRBS_STATr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x19, (_val))
#define WRITE_SERDESDDS_1000X_PRBS_STATr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x19, (_val))
#define MODIFY_SERDESDDS_1000X_PRBS_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x19, \
                             (_val), (_mask))

/* Pat Gen Control (Addr 1Ah) */
#define READ_SERDESDDS_1000X_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1a, (_val))
#define WRITE_SERDESDDS_1000X_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1a, (_val))
#define MODIFY_SERDESDDS_1000X_PAT_GEN_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1a, \
                             (_val), (_mask))

/* Pat Gen Status (Addr 1Bh) */
#define READ_SERDESDDS_1000X_PAT_GEN_STATr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1b, (_val))
#define WRITE_SERDESDDS_1000X_PAT_GEN_STATr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1b, (_val))
#define MODIFY_SERDESDDS_1000X_PAT_GEN_STATr(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1b, \
                             (_val), (_mask))

/* Test Mode (Addr 1Ch) */
#define READ_SERDESDDS_1000X_TEST_MODEr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1c, (_val))
#define WRITE_SERDESDDS_1000X_TEST_MODEr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1c, (_val))
#define MODIFY_SERDESDDS_1000X_TEST_MODEr(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1c, \
                             (_val), (_mask))

/* Force Tx Data1 (Addr 1Dh) */
#define READ_SERDESDDS_1000X_FORCE_TX_DATA1r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1d, (_val))
#define WRITE_SERDESDDS_1000X_FORCE_TX_DATA1r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1d, (_val))
#define MODIFY_SERDESDDS_1000X_FORCE_TX_DATA1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1d, \
                             (_val), (_mask))

/* Force Tx Data2 (Addr 1Eh) */
#define READ_SERDESDDS_1000X_FORCE_TX_DATA2r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1e, (_val))
#define WRITE_SERDESDDS_1000X_FORCE_TX_DATA2r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0000, 0x1e, (_val))
#define MODIFY_SERDESDDS_1000X_FORCE_TX_DATA2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, \
                             (_val), (_mask))

/* 1000X Control #1 Register: Controls 10B/SGMII mode */
#define DDS_1000X_FIBER_MODE    (1 << 0)     /* Enable SGMII fiber mode */
#define DDS_1000X_EN10B_MODE    (1 << 1)     /* Enable TBI 10B interface */
#define DDS_1000X_INVERT_SD     (1 << 3)     /* Invert Signal Detect */
#define DDS_1000X_AUTO_DETECT   (1 << 4)     /* Auto-detect SGMII and 1000X */ 
#define DDS_1000X_SGMII_MASTER  (1 << 5)     /* SGMII Master mode enable */ 
#define DDS_1000X_RX_PKTS_CNTR_SEL (1 << 11) /* Select receive counter for 17h*/
#define DDS_1000X_TX_AMPLITUDE_OVRD    (1 << 12)

/* 1000X Control #2 Register Fields */
#define DDS_1000X_PAR_DET_EN     (1 << 0)     /* Enable Parallel Detect */
#define DDS_1000X_FALSE_LNK_DIS  (1 << 1)     /* Disable false link */
#define DDS_1000X_FLT_FORCE_EN   (1 << 2)     /* Enable filter force link */
#define DDS_1000X_FORCE_XMIT_TX  (1 << 5)     /* Force xmit on tx side */
#define DDS_1000X_CLRAR_BER_CNTR (1 << 14)    /* Clear bit-err-rate counter */

/* 1000X Control #3 Register Fields */
#define DDS_1000X_TX_FIFO_RST           (1 << 0)    /* Reset TX FIFO */
#define DDS_1000X_FIFO_ELASTICITY_MASK  (0x3 << 1)  /* Fifo Elasticity */
#define DDS_1000X_FIFO_ELASTICITY_5K    (0x0 << 1)  /* 5 Kbytes */
#define DDS_1000X_FIFO_ELASTICITY_10K   (0x1 << 1)  /* 10 Kbytes */
#define DDS_1000X_FIFO_ELASTICITY_13_5K (0x2 << 1)  /* 13.5 Kbytes */
#define DDS_1000X_RX_FIFO_RST           (1 << 14)   /* Reset RX FIFO */

/* 1000X Control #4 Register Fields */
#define DDS_1000X_LINK_FORCE            (1 << 7)    /* Link Force */
#define DDS_1000X_DIG_RESET             (1 << 6)    /* Reset Datapath */
#define DDS_1000X_ANALOG_SIG_DETECT     (1 << 0)    /* User analog signal det */

/* 1000X Status #1 Register Fields */
#define DDS_1000X_STATUS1_SGMII_MODE    (1 << 0)
#define DDS_1000X_STATUS1_SPEED         (3 << 3)
#define DDS_1000X_STATUS1_SPEED_10      (0)
#define DDS_1000X_STATUS1_SPEED_100     (1 << 3)
#define DDS_1000X_STATUS1_SPEED_1000    (1 << 4)


/************************/
/* Analog Block (Blk 1) */
/************************/
/* Analog Tx (Addr 10h) */
#define READ_SERDESDDS_ANALOG_TXr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x10, (_val))
#define WRITE_SERDESDDS_ANALOG_TXr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x10, (_val))
#define MODIFY_SERDESDDS_ANALOG_TXr(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x10, \
                             (_val), (_mask))

/* Analog RX1 (Addr 11h) */
#define READ_SERDESDDS_ANALOG_RX1r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x11, (_val))
#define WRITE_SERDESDDS_ANALOG_RX1r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x11, (_val))
#define MODIFY_SERDESDDS_ANALOG_RX1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x11, \
                             (_val), (_mask))

/* Analog RX2 (Addr 12h) */
#define READ_SERDESDDS_ANALOG_RX2r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x12, (_val))
#define WRITE_SERDESDDS_ANALOG_RX2r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x12, (_val))
#define MODIFY_SERDESDDS_ANALOG_RX2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x12, \
                             (_val), (_mask))

/* Analog PLL (Addr 13h) */
#define READ_SERDESDDS_ANALOG_PLLr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x13, (_val))
#define WRITE_SERDESDDS_ANALOG_PLLr(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x13, (_val))
#define MODIFY_SERDESDDS_ANALOG_PLLr(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x13, \
                             (_val), (_mask))

/* Analog RX3 (Addr 14h) */
#define READ_SERDESDDS_ANALOG_RX3r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x14, (_val))
#define WRITE_SERDESDDS_ANALOG_RX3r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x14, (_val))
#define MODIFY_SERDESDDS_ANALOG_RX3r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x14, \
                             (_val), (_mask))

/* TPOUT1 (Addr 18h) */
#define READ_SERDESDDS_ANALOG_TPOUT1r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x18, (_val))
#define WRITE_SERDESDDS_ANALOG_TPOUT1r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x18, (_val))
#define MODIFY_SERDESDDS_ANALOG_TPOUT1r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, \
                             (_val), (_mask))

/* TPOUT2 (Addr 19h) */
#define READ_SERDESDDS_ANALOG_TPOUT2r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_READ((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x19, (_val))
#define WRITE_SERDESDDS_ANALOG_TPOUT2r(_unit, _phy_ctrl, _val) \
            SERDESDDS_REG_WRITE((_unit), (_phy_ctrl), \
                                  0x00, 0x0001, 0x19, (_val))
#define MODIFY_SERDESDDS_ANALOG_TPOUT2r(_unit, _phy_ctrl, _val, _mask) \
            SERDESDDS_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x19, \
                             (_val), (_mask))

/* Analog Rx 1 Register Fields */
#define DDS_ANALOG_RX1_IDDQ             (1 << 0)  
#define DDS_ANALOG_RX1_RESET            (1 << 1)
#define DDS_ANALOG_RX1_EDGE_SEL         (1 << 8) 
#define DDS_ANALOG_RX1_SIG_DET          (1 << 10)

#endif /* _SERDES_DOZEN_H_ */
