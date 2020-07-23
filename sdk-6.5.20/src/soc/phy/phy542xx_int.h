/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PHY542XX_INT_H
#define _PHY542XX_INT_H

/* Standard MII Registers */

#define PHY_BCM542XX_MII_CTRL_REG            0x00  /* MII Control Register : r/w */
#define PHY_BCM542XX_MII_STAT_REG            0x01  /* MII Status Register: ro */
#define PHY_BCM542XX_MII_PHY_ID0_REG         0x02  /* MII PHY ID register: r/w */
#define PHY_BCM542XX_MII_ANA_REG             0x04  /* MII Auto-Neg Advertisement: r/w */
#define PHY_BCM542XX_MII_ANP_REG             0x05  /* MII Auto-Neg Link Partner: ro */
#define PHY_BCM542XX_MII_AN_EXP_REG          0x06  /* MII Auti-Neg Expansion: ro */
#define PHY_BCM542XX_MII_GB_CTRL_REG         0x09  /* MII 1000Base-T control register */
#define PHY_BCM542XX_MII_GB_STAT_REG         0x0a  /* MII 1000Base-T Status register */
#define PHY_BCM542XX_MII_ESR_REG             0x0f  /* MII Extended Status register */

/* Non-standard MII Registers */

#define PHY_BCM542XX_MII_ECR_REG             0x10  /* MII Extended Control Register */
#define PHY_BCM542XX_MII_AUX_REG             0x18  /* MII Auxiliary Control/Status Reg */
#define PHY_BCM542XX_MII_ASSR_REG            0x19  /* MII Auxiliary Status Summary Reg */
#define PHY_BCM542XX_MII_GSR_REG             0x1c  /* MII General status (BROADCOM) */

/* MII Control Register: bit definitions */

#define PHY_BCM542XX_MII_CTRL_UNIDIR_EN      (BIT5)  /* Force speed to 2500 Mbps */
#define PHY_BCM542XX_MII_CTRL_SS_MSB         (BIT6)  /* Speed select, MSb */
#define PHY_BCM542XX_MII_CTRL_FD             (BIT8)  /* Full Duplex */
#define PHY_BCM542XX_MII_CTRL_RST_AN         (BIT9)  /* Restart Autonegotiation */
#define PHY_BCM542XX_MII_CTRL_PWR_DOWN       (BIT11) /* Power Down */
#define PHY_BCM542XX_MII_CTRL_AN_EN          (BIT12) /* Autonegotiation enable */
#define PHY_BCM542XX_MII_CTRL_SS_LSB         (BIT13) /* Speed select, LSb */
#define PHY_BCM542XX_MII_CTRL_LPBK_EN        (BIT14) /* Loopback enable */
#define PHY_BCM542XX_MII_CTRL_RESET          (BIT15) /* PHY reset */

#define PHY_BCM542XX_MII_CTRL_SS(_x)         ((_x) &  \
                       (PHY_BCM542XX_MII_CTRL_SS_LSB|PHY_BCM542XX_MII_CTRL_SS_MSB))
#define PHY_BCM542XX_MII_CTRL_SS_10          0
#define PHY_BCM542XX_MII_CTRL_SS_100         (PHY_BCM542XX_MII_CTRL_SS_LSB)
#define PHY_BCM542XX_MII_CTRL_SS_1000        (PHY_BCM542XX_MII_CTRL_SS_MSB)
#define PHY_BCM542XX_MII_CTRL_SS_INVALID     (PHY_BCM542XX_MII_CTRL_SS_LSB | \
                                              PHY_BCM542XX_MII_CTRL_SS_MSB)
#define PHY_BCM542XX_MII_CTRL_SS_MASK        (PHY_BCM542XX_MII_CTRL_SS_LSB | \
                                              PHY_BCM542XX_MII_CTRL_SS_MSB)
/* 
 * MII Status Register: See 802.3, 1998 pg 544 
 */
#define PHY_BCM542XX_MII_STAT_EXT            (BIT0) /* Extended Registers */
#define PHY_BCM542XX_MII_STAT_JBBR           (BIT1) /* Jabber Detected */
#define PHY_BCM542XX_MII_STAT_LA             (BIT2) /* Link Active */
#define PHY_BCM542XX_MII_STAT_AN_CAP         (BIT3) /* Autoneg capable */
#define PHY_BCM542XX_MII_STAT_RF             (BIT4) /* Remote Fault */
#define PHY_BCM542XX_MII_STAT_AN_DONE        (BIT5) /* Autoneg complete */
#define PHY_BCM542XX_MII_STAT_MF_PS          (BIT6) /* Preamble suppression */
#define PHY_BCM542XX_MII_STAT_ES             (BIT8) /* Extended status (R15) */
#define PHY_BCM542XX_MII_STAT_HD_100_T2      (BIT9) /* Half duplex 100Mb/s supported */
#define PHY_BCM542XX_MII_STAT_FD_100_T2      (BIT10)/* Full duplex 100Mb/s supported */
#define PHY_BCM542XX_MII_STAT_HD_10          (BIT11)/* Half duplex 100Mb/s supported */
#define PHY_BCM542XX_MII_STAT_FD_10          (BIT12)/* Full duplex 100Mb/s supported */
#define PHY_BCM542XX_MII_STAT_HD_100         (BIT13)/* Half duplex 100Mb/s supported */
#define PHY_BCM542XX_MII_STAT_FD_100         (BIT14)/* Full duplex 100Mb/s supported */
#define PHY_BCM542XX_MII_STAT_100_T4         (BIT15)/* Full duplex 100Mb/s supported */

/*
 * MII Link Advertisment
 */
#define PHY_BCM542XX_MII_ANA_ASF             (BIT0)  /* Advertise Selector Field */
#define PHY_BCM542XX_MII_ANA_HD_10           (BIT5)  /* Half duplex 10Mb/s supported */
#define PHY_BCM542XX_MII_ANA_FD_10           (BIT6)  /* Full duplex 10Mb/s supported */
#define PHY_BCM542XX_MII_ANA_HD_100          (BIT7)  /* Half duplex 100Mb/s supported */
#define PHY_BCM542XX_MII_ANA_FD_100          (BIT8)  /* Full duplex 100Mb/s supported */
#define PHY_BCM542XX_MII_ANA_T4              (BIT9)  /* T4 */
#define PHY_BCM542XX_MII_ANA_PAUSE           (BIT10) /* Pause supported */
#define PHY_BCM542XX_MII_ANA_ASYM_PAUSE      (BIT11) /* Asymmetric pause supported */
#define PHY_BCM542XX_MII_ANA_RF              (BIT13) /* Remote fault */
#define PHY_BCM542XX_MII_ANA_NP              (BIT15) /* Next Page */

#define PHY_BCM542XX_MII_ANA_ASF_802_3       (1)     /* 802.3 PHY */

/*
 * 1000X MII Link Advertisment
 */
#define PHY_BCM542XX_1000X_MII_ANA_FD           (BIT5) /* Full duplex supported */
#define PHY_BCM542XX_1000X_MII_ANA_HD           (BIT6) /* Half duplex supported */
#define PHY_BCM542XX_1000X_MII_ANA_PAUSE        (BIT7) /* Symmetric Pause supported */
#define PHY_BCM542XX_1000X_MII_ANA_ASYM_PAUSE   (BIT8) /* Asymmetric pause supported */
#define PHY_BCM542XX_1000X_MII_ANA_NP           (BIT15)/* Next Page */

/*
 * MII Link Advertisment (Clause 37) 
 */
#define PHY_BCM542XX_MII_ANA_C37_NP          (1 << 15)  /* Next Page */
#define PHY_BCM542XX_MII_ANA_C37_RF_OK       (0 << 12)  /* No error, link OK */
#define PHY_BCM542XX_MII_ANA_C37_RF_LINK_FAIL (1 << 12) /* Offline */
#define PHY_BCM542XX_MII_ANA_C37_RF_OFFLINE  (2 << 12)  /* Link failure */
#define PHY_BCM542XX_MII_ANA_C37_RF_AN_ERR   (3 << 12)  /* Auto-Negotiation Error */
#define PHY_BCM542XX_MII_ANA_C37_PAUSE       (1 << 7)   /* Symmetric Pause */
#define PHY_BCM542XX_MII_ANA_C37_ASYM_PAUSE  (1 << 8)   /* Asymmetric Pause */
#define PHY_BCM542XX_MII_ANA_C37_HD          (1 << 6)   /* Half duplex */
#define PHY_BCM542XX_MII_ANA_C37_FD          (1 << 5)   /* Full duplex */ 

/*
 * MII Link Partner Ability (Clause 37)
 */
#define PHY_BCM542XX_MII_ANP_C37_NP          (1 << 15)  /* Next Page */
#define PHY_BCM542XX_MII_ANP_C37_ACK         (1 << 14)  /* Acknowledge */
#define PHY_BCM542XX_MII_ANP_C37_RF_OK       (0 << 12)  /* No error, link OK */
#define PHY_BCM542XX_MII_ANP_C37_RF_LINK_FAIL (1 << 12) /* Offline */
#define PHY_BCM542XX_MII_ANP_C37_RF_OFFLINE  (2 << 12)  /* Link failure */
#define PHY_BCM542XX_MII_ANP_C37_RF_AN_ERR   (3 << 12)  /* Auto-Negotiation Error */
#define PHY_BCM542XX_MII_ANP_C37_PAUSE       (1 << 7)   /* Symmetric Pause */
#define PHY_BCM542XX_MII_ANP_C37_ASYM_PAUSE  (1 << 8)   /* Asymmetric Pause */
#define PHY_BCM542XX_MII_ANP_C37_HD          (1 << 6)   /* Half duplex */
#define PHY_BCM542XX_MII_ANP_C37_FD          (1 << 5)   /* Full duplex */ 

/*
 * MII Link Partner Ability (SGMII)
 */
#define PHY_BCM542XX_MII_ANP_SGMII_MODE      (1 << 0)   /* Link parneter in SGMII mode */
#define PHY_BCM542XX_MII_ANP_SGMII_FD        (1 << 12)  /* SGMII duplex */
#define PHY_BCM542XX_MII_ANP_SGMII_SS_10     (0 << 10)  /* 10Mbps SGMII */
#define PHY_BCM542XX_MII_ANP_SGMII_SS_100    (1 << 10)  /* 100Mbps SGMII */
#define PHY_BCM542XX_MII_ANP_SGMII_SS_1000   (2 << 10)  /* 1000Mbps SGMII */
#define PHY_BCM542XX_MII_ANP_SGMII_SS_MASK   (3 << 10)  /* SGMII speed mask */
#define PHY_BCM542XX_MII_ANP_SGMII_ACK       (1 << 14)  /* SGMII Ackonwledge */
#define PHY_BCM542XX_MII_ANP_SGMII_LINK      (1 << 15)  /* SGMII Link */

/*
 * 1000Base-T Control Register
 */
#define PHY_BCM542XX_MII_GB_CTRL_MS_MAN      (BIT12) /* Manual Master/Slave mode */
#define PHY_BCM542XX_MII_GB_CTRL_MS          (BIT11) /* Master/Slave negotiation mode */
#define PHY_BCM542XX_MII_GB_CTRL_PT          (BIT10) /* Port type */
#define PHY_BCM542XX_MII_GB_CTRL_ADV_1000FD  (BIT9)  /* Advertise 1000Base-T FD */
#define PHY_BCM542XX_MII_GB_CTRL_ADV_1000HD  (BIT8)  /* Advertise 1000Base-T HD */

/*
 * 1000Base-T Status Register
 */
#define PHY_BCM542XX_MII_GB_STAT_MS_FAULT    (BIT15) /* Master/Slave Fault */
#define PHY_BCM542XX_MII_GB_STAT_MS          (BIT14) /* Master/Slave, 1 == Master */
#define PHY_BCM542XX_MII_GB_STAT_LRS         (BIT13) /* Local receiver status */
#define PHY_BCM542XX_MII_GB_STAT_RRS         (BIT12) /* Remote receiver status */
#define PHY_BCM542XX_MII_GB_STAT_LP_1000FD   (BIT11) /* Link partner 1000FD capable */
#define PHY_BCM542XX_MII_GB_STAT_LP_1000HD   (BIT10) /* Link partner 1000HD capable */
#define PHY_BCM542XX_MII_GB_STAT_IDE         (0xff << 0) /* Idle error count */

/*
 * IEEE Extended Status Register
 */
#define PHY_BCM542XX_MII_ESR_1000_X_FD       (BIT15) /* 1000Base-T FD capable */
#define PHY_BCM542XX_MII_ESR_1000_X_HD       (BIT14) /* 1000Base-T HD capable */
#define PHY_BCM542XX_MII_ESR_1000_T_FD       (BIT13) /* 1000Base-T FD capable */
#define PHY_BCM542XX_MII_ESR_1000_T_HD       (BIT12) /* 1000Base-T FD capable */

/*
 * MII Extended Control Register (BROADCOM)
 */
#define PHY_BCM542XX_MII_ECR_FIFO_ELAST_0    (BIT0) /* FIFO Elasticity[0], MSB
                                                             at exp reg 46[14]*/
#define PHY_BCM542XX_MII_ECR_LED_OFF_F       (BIT3) /* Force LED off */
#define PHY_BCM542XX_MII_ECR_LED_ON_F        (BIT4) /* Force LED on */
#define PHY_BCM542XX_MII_ECR_EN_LEDT         (BIT5) /* Enable LED traffic */
#define PHY_BCM542XX_MII_ECR_RST_SCR         (BIT6) /* Reset Scrambler */
#define PHY_BCM542XX_MII_ECR_BYPASS_ALGN     (BIT7) /* Bypass Receive Sym. align */
#define PHY_BCM542XX_MII_ECR_BPASS_MLT3      (BIT8) /* Bypass MLT3 Encoder/Decoder */
#define PHY_BCM542XX_MII_ECR_BPASS_SCR       (BIT9) /* Bypass Scramble/Descramble */
#define PHY_BCM542XX_MII_ECR_BPASS_ENC       (BIT10) /* Bypass 4B/5B Encode/Decode */
#define PHY_BCM542XX_MII_ECR_FORCE_INT       (BIT11) /* Force Interrupt */
#define PHY_BCM542XX_MII_ECR_INT_DIS         (BIT12) /* Interrupt Disable */
#define PHY_BCM542XX_MII_ECR_TX_DIS          (BIT13) /* XMIT Disable */
#define PHY_BCM542XX_MII_ECR_DAMC            (BIT14) /* Disable Auto-MDI Crossover */

/*
 * Auxiliary Status Summary Register (ASSR - Broadcom BCM542xx)
 */
#define PHY_BCM542XX_MII_ASSR_PRTD           (1 << 0) /* Pause resolution/XMIT direction */
#define PHY_BCM542XX_MII_ASSR_PRRD           (1 << 1) /* Pause resolution/RCV direction */
#define PHY_BCM542XX_MII_ASSR_LS             (1 << 2) /* Link Status (1 == link up) */
#define PHY_BCM542XX_MII_ASSR_LPNPA          (1 << 3) /* Link partner next page cap */
#define PHY_BCM542XX_MII_ASSR_LPANA          (1 << 4) /* Link Partner AN capable */
#define PHY_BCM542XX_MII_ASSR_ANPR           (1 << 5) /* Autoneg page received */
#define PHY_BCM542XX_MII_ASSR_RF             (1 << 6) /* Remote Fault */
#define PHY_BCM542XX_MII_ASSR_PDF            (1 << 7) /* Parallel detection fault */
#define PHY_BCM542XX_MII_ASSR_HCD            (7 << 8) /* Current operating speed */
#define PHY_BCM542XX_MII_ASSR_ANNPW          (1 << 11) /* Auto-neg next page wait */
#define PHY_BCM542XX_MII_ASSR_ANABD          (1 << 12) /* Auto-neg Ability detected */
#define PHY_BCM542XX_MII_ASSR_ANAD           (1 << 13) /* Auto-neg ACK detect */
#define PHY_BCM542XX_MII_ASSR_ANCA           (1 << 14) /* Auto-neg complete ACK */
#define PHY_BCM542XX_MII_ASSR_ANC            (1 << 15) /* AUto-neg complete */
#define PHY_BCM542XX_MII_ASSR_HCD_FD_1000    (7 << 8)
#define PHY_BCM542XX_MII_ASSR_HCD_HD_1000    (6 << 8)
#define PHY_BCM542XX_MII_ASSR_HCD_FD_100     (5 << 8)
#define PHY_BCM542XX_MII_ASSR_HCD_T4_100     (4 << 8)
#define PHY_BCM542XX_MII_ASSR_HCD_HD_100     (3 << 8)
#define PHY_BCM542XX_MII_ASSR_HCD_FD_10      (2 << 8)
#define PHY_BCM542XX_MII_ASSR_HCD_HD_10      (1 << 8)
#define PHY_BCM542XX_MII_ASSR_HCD_NC         (0 << 8) /* Not complete */
#define PHY_BCM542XX_MII_ASSR_AUTONEG_HCD_MASK  (0x0700)

/*
 * Auto Detect Medium Register 
 */
#define PHY_BCM542XX_MII_AUTO_DET_MED_2ND_SERDES  (BIT9)
#define PHY_BCM542XX_MII_INV_FIBER_SD             (BIT8)
#define PHY_BCM542XX_MII_FIBER_IN_USE_LED         (BIT7)
#define PHY_BCM542XX_MII_FIBER_LED                (BIT6)
#define PHY_BCM542XX_MII_FIBER_SD_SYNC            (BIT5)
#define PHY_BCM542XX_MII_FIBER_AUTO_PWRDN         (BIT4)
#define PHY_BCM542XX_MII_SD_en_ov                 (BIT3)
#define PHY_BCM542XX_MII_AUTO_DET_MED_DEFAULT     (BIT2)
#define PHY_BCM542XX_MII_AUTO_DET_MED_PRI         (BIT1)
#define PHY_BCM542XX_MII_AUTO_DET_MED_EN          (BIT0)
#define PHY_BCM542XX_MII_AUTO_DET_MASK            (0x033F)

/*
 * Mode Control Register 
 */
#define PHY_BCM542XX_MODE_CNTL_CU_LINK          (BIT7) /* Copper link */
#define PHY_BCM542XX_MODE_CNTL_SERDES_LINK      (BIT6) /* Fiber, SGMII link */
#define PHY_BCM542XX_MODE_CNTL_CU_ENG_DET       (BIT5) /* Copper energy detected */
#define PHY_BCM542XX_MODE_CNTL_FBER_SIG_DET     (BIT4) /* Fiber signal detected */
#define PHY_BCM542XX_MODE_CNTL_SERDES_CAPABLE   (BIT3) /* SerDes capable device */
#define PHY_BCM542XX_MODE_CNTL_MODE_SEL_2       (BIT2) /* Mode select [2] */
#define PHY_BCM542XX_MODE_CNTL_MODE_SEL_1       (BIT1) /* Mode select [1] */
#define PHY_BCM542XX_MODE_CNTL_1000X_EN         (BIT0) /* Select 1000-X (fiber) regs
                                                          vs. Copper regs   */
#define PHY_BCM542XX_MODE_CNTL_MODE_SEL_MASK    (PHY_BCM542XX_MODE_CNTL_MODE_SEL_1 | \
                                                 PHY_BCM542XX_MODE_CNTL_MODE_SEL_2)

#define PHY_BCM542XX_MODE_SEL_COPPER_2_SGMII    (0x0)
#define PHY_BCM542XX_MODE_SEL_FIBER_2_SGMII     (PHY_BCM542XX_MODE_CNTL_MODE_SEL_1)
#define PHY_BCM542XX_MODE_SEL_SGMII_2_COPPER    (PHY_BCM542XX_MODE_CNTL_MODE_SEL_2)
#define PHY_BCM542XX_MODE_SEL_GBIC              (PHY_BCM542XX_MODE_CNTL_MODE_SEL_1 | \
                                                 PHY_BCM542XX_MODE_CNTL_MODE_SEL_2)

/* 1000 BASE X CONTROL REG */
#define PHY_BCM542XX_1000BASE_X_CTRL_REG_LE     (BIT14)
#define PHY_BCM542XX_1000BASE_X_CTRL_REG_AE     (BIT12)

/* 1000 BASE X STATUS REG */
#define PHY_BCM542XX_1000BASE_X_STAT_REG_LA     (BIT2)

/* 
 * MII Broadcom MISC Control Register(PHY_BCM542XX_MII_MISC_CTRL_REG)
 */
#define PHY_BCM542XX_MII_FORCE_AUTO_MDIX_MODE   (BIT9)
#define PHY_BCM542XX_MII_BP_WIRESPEED_TIMER     (BIT10)
#define PHY_BCM542XX_MII_WIRESPEED_EN           (BIT4)
#define PHY_BCM542XX_MII_MISC_CTRL_ALL          (0x0FF8)
#define PHY_BCM542XX_MII_MISC_CTRL_CLEARALL     (0x2007)

/* 
 * MII Broadcom Test Register(PHY_BCM542XX_MII_SHA_AUX_STAT2_REG)
 */                                       /* Enable Shadow registers */
#define PHY_BCM542XX_MII_SHA_AUX_STAT2_LEN      (0x7 << 12)

/* Miscellaneous registers */
#define PHY_BCM542XX_SPARE_CTRL_REG_LINK_LED         (BIT0)
#define PHY_BCM542XX_SPARE_CTRL_REG_LINK_SPEED_LED   (BIT2)


/* Flag indicating that its a SerDes register */

#define PHY_BCM542XX_REG_1000X       (BIT0)
#define PHY_BCM542XX_REG_PRI_SERDES  (BIT1)
#define PHY_BCM542XX_REG_QSGMII      (BIT4)
#define PHY_BCM542XX_REG_FIBER       (PHY_BCM542XX_REG_1000X | \
                                      PHY_BCM542XX_REG_PRI_SERDES)


/***********************************************
 *
 * Generic register accessing macros/functions
 */

int phy_bcm542xx_reg_read(int unit, phy_ctrl_t *pc, uint32 flags,
        uint16 reg_bank, uint8 reg_addr, uint16 *data);
int phy_bcm542xx_reg_write(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
        uint8 reg_addr, uint16 data);
int phy_bcm542xx_reg_modify(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
        uint8 reg_addr, uint16 data, uint16 mask);
int phy_bcm542xx_reg_read_modify_write(int unit, phy_ctrl_t *pc, uint32 reg_addr,
        uint16 reg_data, uint16 reg_mask);

int phy_bcm542xx_qsgmii_reg_read(int unit, phy_ctrl_t *pc, int dev_port,
        uint16 block, uint8 reg, uint16 *data);
int phy_bcm542xx_qsgmii_reg_write(int unit, phy_ctrl_t *pc, int dev_port,
        uint16 block, uint8 reg, uint16 data);
int phy_bcm542xx_qsgmii_reg_modify(int unit, phy_ctrl_t *pc, int dev_port,
        uint16 block, uint8 reg_addr, uint16 reg_data, uint16 reg_mask);

int phy_bcm542xx_cl45_reg_read(int unit, phy_ctrl_t *pc, uint8 dev_addr,
        uint16 reg_addr, uint16 *val);
int phy_bcm542xx_cl45_reg_write(int unit, phy_ctrl_t *pc, uint8 dev_addr,
        uint16 reg_addr, uint16 val);
int phy_bcm542xx_cl45_reg_modify(int unit, phy_ctrl_t *pc, uint8 dev_addr,
        uint16 reg_addr, uint16 val, uint16 mask);

/* PHY register access */
#define PHY_BCM542XX_READ_PHY_REG(_unit, _phy_ctrl,  _reg_addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl),  (_reg_addr), (_val))
#define PHY_BCM542XX_WRITE_PHY_REG(_unit, _phy_ctrl, _reg_addr, _val) \
            WRITE_PHY_REG((_unit), (_phy_ctrl), (_reg_addr), (_val))
#define PHY_BCM542XX_MODIFY_PHY_REG(_unit, _phy_ctrl, _reg_addr, _val, _mask) \
            MODIFY_PHY_REG((_unit), (_phy_ctrl), (_reg_addr), (_val), (_mask))

/* General - PHY register access */
#define PHY_BCM542XX_REG_RD(_unit, _pc, _flags, _reg_bank, _reg_addr, _val) \
            phy_bcm542xx_reg_read((_unit), (_pc), (_flags), (_reg_bank), \
                                    (_reg_addr), (_val))
#define PHY_BCM542XX_REG_WR(_unit, _pc, _flags, _reg_bank, _reg_addr, _val) \
            phy_bcm542xx_reg_write((_unit), (_pc), (_flags), (_reg_bank), \
                                    (_reg_addr), (_val))
#define PHY_BCM542XX_REG_MOD(_unit, _pc, _flags, _reg_bank, _reg_addr, _val, _mask) \
            phy_bcm542xx_reg_modify((_unit), (_pc), (_flags), (_reg_bank), \
                                    (_reg_addr), (_val), (_mask))

/* Clause 45 Registers access using Clause 22 register access */
#define PHY_BCM542XX_CL45_REG_READ(_unit, _pc, _dev_addr, _reg_addr, _val) \
            phy_bcm542xx_cl45_reg_read((_unit), (_pc), (_dev_addr), \
                                         (_reg_addr), (_val))
#define PHY_BCM542XX_CL45_REG_WRITE(_unit, _pc, _dev_addr, _reg_addr, _val) \
            phy_bcm542xx_cl45_reg_write((_unit), (_pc), (_dev_addr), \
                                         (_reg_addr), (_val))
#define PHY_BCM542XX_CL45_REG_MODIFY(_unit, _pc,  _dev_addr, _reg_addr, _val, _mask) \
            phy_bcm542xx_cl45_reg_modify((_unit), (_pc), (_dev_addr), \
                                         (_reg_addr), (_val), (_mask))

/**************************************************
 *
 * RDB/Legacy register accessing macros/functions
 */
#undef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* default to RDB mode */

/* Registers used to Rd/Wr using RDB mode */
#define PHY_BCM542XX_RDB_ADDR_REG_OFFSET        (0x1E)
#define PHY_BCM542XX_RDB_ADDR_REG_ADDR          (0xffff)
#define PHY_BCM542XX_RDB_DATA_REG_OFFSET        (0x1F)
#define PHY_BCM542XX_RDB_DATA_REG_DATA          (0xffff)
/* Registers used to enable RDB register access mode */
#define PHY_BCM542XX_REG_17_OFFSET              (0x17)
#define PHY_BCM542XX_REG_17_SELECT_EXP_7E       (0x0F7E)
#define PHY_BCM542XX_REG_15_OFFSET              (0x15)
#define PHY_BCM542XX_REG_15_RDB_EN              (0x0000)
#define PHY_BCM542XX_REG_15_RDB_DIS             (0x8000)
#define PHY_BCM542XX_REG_1E_SELECT_RDB          (0x0087)

/* 
 *  Macro for activating the RDB Register Addressing mode  *
 *  Enable direct RDB addressing mode, write to Expansion register 0x7E = 0x0000
 *  - MDIO write to reg 0x17 = 0x0F7E
 *  - MDIO write to reg 0x15 = 0x0000
 */
#define PHY_BCM542XX_RDB_ACCESS_MODE(_u, _pc)   do {  \
    SOC_IF_ERROR_RETURN(  \
        PHY_BCM542XX_WRITE_PHY_REG((_u), (_pc), PHY_BCM542XX_REG_17_OFFSET, \
                                                PHY_BCM542XX_REG_17_SELECT_EXP_7E) ); \
    SOC_IF_ERROR_RETURN(  \
        PHY_BCM542XX_WRITE_PHY_REG((_u), (_pc), PHY_BCM542XX_REG_15_OFFSET,    \
                                                PHY_BCM542XX_REG_15_RDB_EN) ); \
} while ( 0 )

/*  
 *  Macro for activating the Legacy Register Addressing mode  *
 *  Disable direct RDB addressing mode, write to RDB register 0x87 = 0x8000
 *  - MDIO write to reg 0x1E = 0x0087
 *  - MDIO write to reg 0x1F = 0x8000
 */
#define PHY_BCM542XX_LEGACY_ACCESS_MODE(_u, _pc)   do {  \
    SOC_IF_ERROR_RETURN(  \
        PHY_BCM542XX_WRITE_PHY_REG((_u), (_pc), PHY_BCM542XX_RDB_ADDR_REG_OFFSET, \
                                                PHY_BCM542XX_REG_1E_SELECT_RDB) );\
    SOC_IF_ERROR_RETURN(  \
        PHY_BCM542XX_WRITE_PHY_REG((_u), (_pc), PHY_BCM542XX_RDB_DATA_REG_OFFSET, \
                                                PHY_BCM542XX_REG_15_RDB_DIS) );   \
} while ( 0 )


#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING

/* RDB register accessing funtions from Legacy mode */
int phy_bcm542xx_direct_reg_read(  int unit, phy_ctrl_t *pc, uint16 reg_addr,
                                             uint16 *data);
int phy_bcm542xx_direct_reg_write( int unit, phy_ctrl_t *pc, uint16 reg_addr,
                                             uint16 data);
int phy_bcm542xx_direct_reg_modify(int unit, phy_ctrl_t *pc, uint16 reg_addr,
                                             uint16 data, uint16 mask);
#define PHY_BCM542XX_DEFAULT_REG_ACCESS_MODE    PHY_BCM542XX_LEGACY_ACCESS_MODE

#else  /* default to RDB register addressing mode */

#define PHY_BCM542XX_DEFAULT_REG_ACCESS_MODE    PHY_BCM542XX_RDB_ACCESS_MODE

/* RDB register accessing funtions */
int phy_bcm542xx_rdb_reg_read(  int unit, phy_ctrl_t *pc, uint16 reg_addr,
                                          uint16 *data);
int phy_bcm542xx_rdb_reg_write( int unit, phy_ctrl_t *pc, uint16 reg_addr,
                                          uint16 data);
int phy_bcm542xx_rdb_reg_modify(int unit, phy_ctrl_t *pc, uint16 reg_addr,
                                          uint16 data, uint16 mask);
#define phy_bcm542xx_direct_reg_read      phy_bcm542xx_rdb_reg_read
#define phy_bcm542xx_direct_reg_write     phy_bcm542xx_rdb_reg_write
#define phy_bcm542xx_direct_reg_modify    phy_bcm542xx_rdb_reg_modify

#endif /* PHY_BCM542XX_DEFAULT_TO_LEGACY_ACCESS */

#define PHY_BCM542XX_RDB_RD               phy_bcm542xx_direct_reg_read
#define PHY_BCM542XX_RDB_WR               phy_bcm542xx_direct_reg_write
#define PHY_BCM542XX_RDB_MO               phy_bcm542xx_direct_reg_modify


/***********************************************
 *
 * Specific register accessing macros
 */

/* 1000BASE-T/100BASE-TX/10BASE-T MII Control Register (Addr 00h) */
#define PHY_READ_BCM542XX_MII_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, \
                                 PHY_BCM542XX_MII_CTRL_REG, (_val))
#define PHY_WRITE_BCM542XX_MII_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, \
                                 PHY_BCM542XX_MII_CTRL_REG, (_val))
#define PHY_MODIFY_BCM542XX_MII_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, \
                                 PHY_BCM542XX_MII_CTRL_REG, (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T MII Status Register (ADDR 01h) */
#define PHY_READ_BCM542XX_MII_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, 0x01, (_val)) 
#define PHY_WRITE_BCM542XX_MII_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, 0x01, (_val)) 
#define PHY_MODIFY_BCM542XX_MII_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, 0x01, \
                                 (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 02h) */
#define PHY_READ_BCM542XX_MII_PHY_ID0r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, 0x02, (_val))
#define PHY_WRITE_BCM542XX_MII_PHY_ID0r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, 0x02, (_val))
#define PHY_MODIFY_BCM542XX_MII_PHY_ID0r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, 0x02, \
                                 (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 03h) */
#define PHY_READ_BCM542XX_MII_PHY_ID1r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, 0x03, (_val))
#define PHY_WRITE_BCM542XX_MII_PHY_ID1r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, 0x03, (_val))
#define PHY_MODIFY_BCM542XX_MII_PHY_ID1r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, 0x03, \
                                 (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Advertisment Register (ADDR 04h) */
#define PHY_READ_BCM542XX_MII_ANAr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, 0x04, (_val))
#define PHY_WRITE_BCM542XX_MII_ANAr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, 0x04, (_val))
#define PHY_MODIFY_BCM542XX_MII_ANAr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, 0x04, \
                                 (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Link Partner Ability (ADDR 05h) */
#define PHY_READ_BCM542XX_MII_ANPr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, 0x05, (_val))
#define PHY_WRITE_BCM542XX_MII_ANPr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, 0x05, (_val)) 
#define PHY_MODIFY_BCM542XX_MII_ANPr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, 0x05, \
                                 (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Expansion Register (ADDR 06h) */
#define PHY_READ_BCM542XX_MII_AN_EXPr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, 0x06, (_val))
#define PHY_WRITE_BCM542XX_MII_AN_EXPr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, 0x06, (_val))
#define PHY_MODIFY_BCM542XX_MII_AN_EXPr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, 0x06, \
                                 (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Next Page Transmit Register (ADDR 07h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Link Partner Received Next Page (ADDR 08h) */

/* 1000BASE-T Control Register  (ADDR 09h) */
#define PHY_READ_BCM542XX_MII_GB_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, 0x09, (_val))
#define PHY_WRITE_BCM542XX_MII_GB_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, 0x09, (_val))
#define PHY_MODIFY_BCM542XX_MII_GB_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, 0x09, \
                                 (_val), (_mask))

/* 1000BASE-T Status Register (ADDR 0ah) */
#define PHY_READ_BCM542XX_MII_GB_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, 0x0a, (_val)) 
#define PHY_WRITE_BCM542XX_MII_GB_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, 0x0a, (_val))
#define PHY_MODIFY_BCM542XX_MII_GB_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, 0x0a, \
                                 (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T IEEE Extended Status Register (ADDR 0fh) */
#define PHY_READ_BCM542XX_MII_ESRr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, 0x0f, (_val)) 
#define PHY_WRITE_BCM542XX_MII_ESRr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, 0x0f, (_val))
#define PHY_MODIFY_BCM542XX_MII_ESRr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, 0x0f, \
                                 (_val), (_mask))

/*****************************************************************************/

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Extended Control Register */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING    /* Legacy Addr 10h */
#define PHY_READ_BCM542XX_MII_ECRr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, \
                                 PHY_BCM542XX_MII_ECR_REG, (_val))
#define PHY_WRITE_BCM542XX_MII_ECRr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, \
                                 PHY_BCM542XX_MII_ECR_REG, (_val))
#define PHY_MODIFY_BCM542XX_MII_ECRr(_unit, _pc, _val, _mask)  \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, \
                                 PHY_BCM542XX_MII_ECR_REG, (_val), (_mask))
#else       /* RDB_Reg. 0x000 */
#define PHY_READ_BCM542XX_MII_ECRr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x000, _val)
#define PHY_WRITE_BCM542XX_MII_ECRr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x000, _val)
#define PHY_MODIFY_BCM542XX_MII_ECRr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x000, _val, _mask)
#endif

/* 1000BASE-T/100BASE-TX/10BASE-T Receive Error Counter Register (ADDR 12h) */
/* 1000BASE-T/100BASE-TX/10BASE-T False Carrier Sense Counter (ADDR 13h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Receive NOT_OK Counter Register (ADDR 14h) */

/* 1000BASE-T/100BASE-TX/10BASE-T Auxiliary Control Register */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 18h Shadow 000) */
#define PHY_READ_BCM542XX_MII_AUX_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, 0x18, (_val))
#define PHY_WRITE_BCM542XX_MII_AUX_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, 0x18, (_val))
#define PHY_MODIFY_BCM542XX_MII_AUX_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, 0x18, \
                             (_val), (_mask))
#else       /* RDB_Reg. 0x028 */
#define PHY_READ_BCM542XX_MII_AUX_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x028, _val)
#define PHY_WRITE_BCM542XX_MII_AUX_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x028, _val)
#define PHY_MODIFY_BCM542XX_MII_AUX_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x028, _val, _mask)
#endif

#define PHY_BCM542XX_MII_AUX_CTRL_REG_EXT_PKT_LEN                BIT(14)
#define PHY_BCM542XX_MII_AUX_CTRL_REG_EN_DSP_CLK                 BIT(11)
/* 10BASE-T Register */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 18h Shadow 001) */
#define PHY_READ_BCM542XX_MII_10BASE_Tr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0001, 0x18, (_val))
#define PHY_WRITE_BCM542XX_MII_10BASE_Tr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0001, 0x18, (_val))
#define PHY_MODIFY_BCM542XX_MII_10BASE_Tr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0001, 0x18, \
                             (_val), (_mask))
#else       /* RDB_Reg. 0x029 */
#define PHY_READ_BCM542XX_MII_10BASE_Tr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x029, _val)
#define PHY_WRITE_BCM542XX_MII_10BASE_Tr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x029, _val)
#define PHY_MODIFY_BCM542XX_MII_10BASE_Tr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x029, _val, _mask)
#endif

/* 1000BASE-T/100BASE-TX/10BASE-T Power/MII Control Reg */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 18h Shadow 010) */
#define PHY_READ_BCM542XX_MII_POWER_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0002, 0x18, (_val))
#define PHY_WRITE_BCM542XX_MII_POWER_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0002, 0x18, (_val))
#define PHY_MODIFY_BCM542XX_MII_POWER_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0002, 0x18, \
                             (_val), (_mask))
#else       /* RDB_Reg. 0x02A */
#define PHY_READ_BCM542XX_MII_POWER_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x02a, _val)
#define PHY_WRITE_BCM542XX_MII_POWER_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x02a, _val)
#define PHY_MODIFY_BCM542XX_MII_POWER_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x02a, _val, _mask)
#endif

#define PHY_BCM542XX_MII_MISC_TEST_RMT_LB  BIT(15) /* Remote Loopback */

/* 1000BASE-T/100BASE-TX/10BASE-T Misc Test Register */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 18h Shadow 100) */
#define PHY_READ_BCM542XX_MII_MISC_TESTr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0004, 0x18, (_val))
#define PHY_WRITE_BCM542XX_MII_MISC_TESTr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0004, 0x18, (_val))
#define PHY_MODIFY_BCM542XX_MII_MISC_TESTr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0004, 0x18, \
                             (_val), (_mask))
#else       /* RDB_Reg. 0x02C */
#define PHY_READ_BCM542XX_MII_MISC_TESTr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x02c, _val)
#define PHY_WRITE_BCM542XX_MII_MISC_TESTr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x02c, _val)
#define PHY_MODIFY_BCM542XX_MII_MISC_TESTr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x02c, _val, _mask)
#endif

/* 1000BASE-T/100BASE-TX/10BASE-T Misc Control Register */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 18h Shadow 111) */
#define PHY_READ_BCM542XX_MII_MISC_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0007, 0x18, (_val))
#define PHY_WRITE_BCM542XX_MII_MISC_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0007, 0x18, (_val))
#define PHY_MODIFY_BCM542XX_MII_MISC_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0007, 0x18, \
                             (_val), (_mask))
#else       /* RDB_Reg. 0x02F */
#define PHY_READ_BCM542XX_MII_MISC_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x02f, _val)
#define PHY_WRITE_BCM542XX_MII_MISC_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x02f, _val)
#define PHY_MODIFY_BCM542XX_MII_MISC_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x02f, _val, _mask)
#endif

/* 1000BASE-T/100BASE-TX/10BASE-T Auxiliary Status Register (ADDR 19h) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 19h */
#define PHY_READ_BCM542XX_MII_AUX_STATUSr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, 0x19, (_val))
#define PHY_WRITE_BCM542XX_MII_AUX_STATUSr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, 0x19, (_val))
#define PHY_MODIFY_BCM542XX_MII_AUX_STATUSr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, 0x19, \
                             (_val), (_mask))
#else       /* RDB_Reg. 0x009 */
#define PHY_READ_BCM542XX_MII_AUX_STATUSr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x009, _val)
#define PHY_WRITE_BCM542XX_MII_AUX_STATUSr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x009, _val)
#define PHY_MODIFY_BCM542XX_MII_AUX_STATUSr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x009, _val, _mask)
#endif

/* 1000BASE-T/100BASE-TX/10BASE-T Interrupt Status Register (ADDR 1ah) */
/* 1000BASE-T/100BASE-TX/10BASE-T Interrupt Control Register (ADDR 1bh) */

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl Reg (ADDR 1ch shadow 00010) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 02h) */
#define PHY_READ_BCM542XX_SPARE_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0002, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_SPARE_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0002, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_SPARE_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0002, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x012 */
#define PHY_READ_BCM542XX_SPARE_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x012, _val)
#define PHY_WRITE_BCM542XX_SPARE_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x012, _val)
#define PHY_MODIFY_BCM542XX_SPARE_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x012, _val, _mask)
#endif

/* 1000BASE-T/100BASE-TX/10BASE-T Clk Alignment Ctrl (ADDR 1ch shadow 00011) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 03h) */
#define PHY_READ_BCM542XX_CLK_ALIGN_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0003, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_CLK_ALIGN_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0003, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_CLK_ALIGN_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0003, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x013 */
#define PHY_READ_BCM542XX_CLK_ALIGN_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x013, _val)
#define PHY_WRITE_BCM542XX_CLK_ALIGN_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x013, _val)
#define PHY_MODIFY_BCM542XX_CLK_ALIGN_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x013, _val, _mask)
#endif

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl 2 Reg (ADDR 1ch shadow 00100) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 04h) */
#define PHY_READ_BCM542XX_SPARE_CTRL_2r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0004, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_SPARE_CTRL_2r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0004, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_SPARE_CTRL_2r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0004, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x014 */
#define PHY_READ_BCM542XX_SPARE_CTRL_2r(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x014, _val)
#define PHY_WRITE_BCM542XX_SPARE_CTRL_2r(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x014, _val)
#define PHY_MODIFY_BCM542XX_SPARE_CTRL_2r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x014, _val, _mask)
#endif

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl 3 Reg (ADDR 1ch shadow 00101) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 05h) */
#define PHY_READ_BCM542XX_SPARE_CTRL_3r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0005, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_SPARE_CTRL_3r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0005, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_SPARE_CTRL_3r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0005, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x015 */
#define PHY_READ_BCM542XX_SPARE_CTRL_3r(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x015, _val)
#define PHY_WRITE_BCM542XX_SPARE_CTRL_3r(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x015, _val)
#define PHY_MODIFY_BCM542XX_SPARE_CTRL_3r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x015, _val, _mask)
#endif
#define BCM542XX_AUTO_PWR_DOWN_DLL_DIS                  BIT(1)

/* 1000BASE-T/100BASE-TX/10BASE-T LED Status Reg (ADDR 1ch shadow 01000) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 08h) */
#define PHY_READ_BCM542XX_LED_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0008, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_LED_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0008, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_LED_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0008, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x018 */
#define PHY_READ_BCM542XX_LED_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x018, _val)
#define PHY_WRITE_BCM542XX_LED_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x018, _val)
#define PHY_MODIFY_BCM542XX_LED_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x018, _val, _mask)
#endif

/* 1000BASE-T/100BASE-TX/10BASE-T LED Ctrl Reg (ADDR 1ch shadow 01001) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 09h) */
#define PHY_READ_BCM542XX_LED_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0009, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_LED_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0009, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_LED_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0009, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x019 */
#define PHY_READ_BCM542XX_LED_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x019, _val)
#define PHY_WRITE_BCM542XX_LED_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x019, _val)
#define PHY_MODIFY_BCM542XX_LED_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x019, _val, _mask)
#endif

/* Auto Power-Down Reg (ADDR 1ch shadow 01010) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 0Ah) */
#define PHY_READ_BCM542XX_AUTO_POWER_DOWNr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x000a, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_AUTO_POWER_DOWNr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x000a, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_AUTO_POWER_DOWNr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x000a, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x01A */
#define PHY_READ_BCM542XX_AUTO_POWER_DOWNr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x01a, _val)
#define PHY_WRITE_BCM542XX_AUTO_POWER_DOWNr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x01a, _val)
#define PHY_MODIFY_BCM542XX_AUTO_POWER_DOWNr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x01a, _val, _mask)
#endif

/* 1000BASE-T/100BASE-TX/10BASE-T LED Selector 1 Reg (ADDR 1ch shadow 01101) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 0Dh) */
#define PHY_READ_BCM542XX_LED_SELECTOR_1r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x000d, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_LED_SELECTOR_1r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x000d, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_LED_SELECTOR_1r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x000d, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x01D */
#define PHY_READ_BCM542XX_LED_SELECTOR_1r(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x01d, _val)
#define PHY_WRITE_BCM542XX_LED_SELECTOR_1r(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x01d, _val)
#define PHY_MODIFY_BCM542XX_LED_SELECTOR_1r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x01d, _val, _mask)
#endif

/* 1000BASE-T/100BASE-TX/10BASE-T LED Selector 2 Reg (ADDR 1ch shadow 01110) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 0Eh) */
#define PHY_READ_BCM542XX_LED_SELECTOR_2r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x000e, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_LED_SELECTOR_2r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x000e, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_LED_SELECTOR_2r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x000e, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x01E */
#define PHY_READ_BCM542XX_LED_SELECTOR_2r(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x01e, _val)
#define PHY_WRITE_BCM542XX_LED_SELECTOR_2r(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x01e, _val)
#define PHY_MODIFY_BCM542XX_LED_SELECTOR_2r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x01e, _val, _mask)
#endif

/* 1000BASE-T/100BASE-TX/10BASE-T LED GPIO Ctrl/Stat (ADDR 1ch shadow 01111) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 0Fh) */
#define PHY_READ_BCM542XX_LED_GPIO_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x000f, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_LED_GPIO_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x000f, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_LED_GPIO_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x000f, 0x1c, \
                          (_val), (_mask))
#else       /* RDB_Reg. 0x01F */
#define PHY_READ_BCM542XX_LED_GPIO_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x01f, _val)
#define PHY_WRITE_BCM542XX_LED_GPIO_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x01f, _val)
#define PHY_MODIFY_BCM542XX_LED_GPIO_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x01f, _val, _mask)
#endif


/* SerDes 100BASE-FX Status Reg (ADDR 1ch shadow 10001) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 11h) */
#define PHY_READ_BCM542XX_100FX_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0011, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_100FX_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0011, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_100FX_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0011, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x231 */
#define PHY_READ_BCM542XX_100FX_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x231, _val)
#define PHY_WRITE_BCM542XX_100FX_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x231, _val)
#define PHY_MODIFY_BCM542XX_100FX_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x231, _val, _mask)
#endif


/* SerDes 100BASE-FX Control Reg (ADDR 1ch shadow 10011) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 13h) */
#define PHY_READ_BCM542XX_100FX_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0013, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_100FX_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0013, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_100FX_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0013, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x233 */
#define PHY_READ_BCM542XX_100FX_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x233, _val)
#define PHY_WRITE_BCM542XX_100FX_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x233, _val)
#define PHY_MODIFY_BCM542XX_100FX_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x233, _val, _mask)
#endif


/* External SerDes Control Reg (ADDR 1ch shadow 10100) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 14h) */
#define PHY_READ_BCM542XX_EXT_SERDES_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0014, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_EXT_SERDES_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0014, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_EXT_SERDES_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0014, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x234 */
#define PHY_READ_BCM542XX_EXT_SERDES_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x234, _val)
#define PHY_WRITE_BCM542XX_EXT_SERDES_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x234, _val)
#define PHY_MODIFY_BCM542XX_EXT_SERDES_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x234, _val, _mask)
#endif

/* SGMII Slave Reg (ADDR 1ch shadow 10101) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 15h) */
#define PHY_READ_BCM542XX_SGMII_SLAVEr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0015, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_SGMII_SLAVEr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0015, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_SGMII_SLAVEr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0015, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x235 */
#define PHY_READ_BCM542XX_SGMII_SLAVEr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x235, _val)
#define PHY_WRITE_BCM542XX_SGMII_SLAVEr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x235, _val)
#define PHY_MODIFY_BCM542XX_SGMII_SLAVEr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x235, _val, _mask)
#endif

/* Primary SerDes Control Reg (ADDR 1ch shadow 10110) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 16h) */
#define PHY_READ_BCM542XX_1ST_SERDES_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0016, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_1ST_SERDES_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0016, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_1ST_SERDES_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0016, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x236 */
#define PHY_READ_BCM542XX_1ST_SERDES_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x236, _val)
#define PHY_WRITE_BCM542XX_1ST_SERDES_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x236, _val)
#define PHY_MODIFY_BCM542XX_1ST_SERDES_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x236, _val, _mask)
#endif
#define PHY_BCM542XX_SERDES_CTRL_JUMBO_MSB      BIT(0)
#define PHY_BCM542XX_SERDES_CTRL_AN_PD_EN       BIT(1)

/* Misc 1000BASE-X Control (ADDR 1ch shadow 10111) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 17h) */
#define PHY_READ_BCM542XX_MISC_1000X_CONTROLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0017, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_MISC_1000X_CONTROLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0017, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_MISC_1000X_CONTROLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0017, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x237 */
#define PHY_READ_BCM542XX_MISC_1000X_CONTROLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x237, _val)
#define PHY_WRITE_BCM542XX_MISC_1000X_CONTROLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x237, _val)
#define PHY_MODIFY_BCM542XX_MISC_1000X_CONTROLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x237, _val, _mask)
#endif

/* 1000BASE-X Auto-Detect SGMII/Media Converter Reg (ADDR 1ch shadow 11000) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 18h) */
#define PHY_READ_BCM542XX_AUTO_DETECT_SGMII_MEDIAr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0018, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_AUTO_DETECT_SGMII_MEDIAr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0018, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_AUTO_DETECT_SGMII_MEDIAr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0018, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x238 */
#define PHY_READ_BCM542XX_AUTO_DETECT_SGMII_MEDIAr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x238, _val)
#define PHY_WRITE_BCM542XX_AUTO_DETECT_SGMII_MEDIAr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x238, _val)
#define PHY_MODIFY_BCM542XX_AUTO_DETECT_SGMII_MEDIAr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x238, _val, _mask)
#endif

/* 1000BASE-X Auto-neg Debug Reg (ADDR 1ch shadow 11010) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 1Ah) */
#define PHY_READ_BCM542XX_1000X_AN_DEBUGr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x001a, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_1000X_AN_DEBUGr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x001a, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_1000X_AN_DEBUGr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x001a, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x23A */
#define PHY_READ_BCM542XX_1000X_AN_DEBUGr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x23a, _val)
#define PHY_WRITE_BCM542XX_1000X_AN_DEBUGr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x23a, _val)
#define PHY_MODIFY_BCM542XX_1000X_AN_DEBUGr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x23a, _val, _mask)
#endif

#define PHY_MODIFY_BCM542XX_AUTO_DETECT_GMII_FIFO_JUMBO_LSB BIT(2)

/* Auxiliary 1000BASE-X Control Reg (ADDR 1ch shadow 11011) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 1Bh) */
#define PHY_READ_BCM542XX_AUX_1000X_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x001b, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_AUX_1000X_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x001b, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_AUX_1000X_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x001b, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x23B */
#define PHY_READ_BCM542XX_AUX_1000X_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x23b, _val)
#define PHY_WRITE_BCM542XX_AUX_1000X_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x23b, _val)
#define PHY_MODIFY_BCM542XX_AUX_1000X_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x23b, _val, _mask)
#endif
#define PHY_BCM542XX_SERDES_CTRL_JUMBO_LSB      BIT(1)

/* Auxiliary 1000BASE-X Status Reg (ADDR 1ch shadow 11100) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 1Ch) */
#define PHY_READ_BCM542XX_AUX_1000X_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x001c, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_AUX_1000X_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x001c, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_AUX_1000X_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x001c, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x23C */
#define PHY_READ_BCM542XX_AUX_1000X_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x23c, _val)
#define PHY_WRITE_BCM542XX_AUX_1000X_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x23c, _val)
#define PHY_MODIFY_BCM542XX_AUX_1000X_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x23c, _val, _mask)
#endif

/* Misc 1000BASE-X Status Reg (ADDR 1ch shadow 11101) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 1Dh) */
#define PHY_READ_BCM542XX_MISC_1000X_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x001d, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_MISC_1000X_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x001d, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_MISC_1000X_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x001d, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x23D */
#define PHY_READ_BCM542XX_MISC_1000X_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x23d, _val)
#define PHY_WRITE_BCM542XX_MISC_1000X_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x23d, _val)
#define PHY_MODIFY_BCM542XX_MISC_1000X_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x23d, _val, _mask)
#endif

/* Copper/Fiber Auto-Detect Medium Reg (ADDR 1ch shadow 11110) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 1Eh) */
#define PHY_READ_BCM542XX_AUTO_DETECT_MEDIUMr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x001e, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_AUTO_DETECT_MEDIUMr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x001e, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_AUTO_DETECT_MEDIUMr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x001e, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x23E */
#define PHY_READ_BCM542XX_AUTO_DETECT_MEDIUMr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x23e, _val)
#define PHY_WRITE_BCM542XX_AUTO_DETECT_MEDIUMr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x23e, _val)
#define PHY_MODIFY_BCM542XX_AUTO_DETECT_MEDIUMr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x23e, _val, _mask)
#endif


/* Mode Control Reg (ADDR 1ch shadow 11111) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Ch Shadow 1Fh) */
#define PHY_READ_BCM542XX_MODE_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x001f, 0x1c, (_val))
#define PHY_WRITE_BCM542XX_MODE_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x001f, 0x1c, (_val))
#define PHY_MODIFY_BCM542XX_MODE_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x001f, 0x1c, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x021 */
#define PHY_READ_BCM542XX_MODE_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x021, _val)
#define PHY_WRITE_BCM542XX_MODE_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x021, _val)
#define PHY_MODIFY_BCM542XX_MODE_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x021, _val, _mask)
#endif


/* 1000BASE-T/100BASE-TX/10BASE-T Test Register 1 */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Legacy Addr 1Eh */
#define PHY_READ_BCM542XX_TEST1r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0000, 0x1e, (_val))
#define PHY_WRITE_BCM542XX_TEST1r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0000, 0x1e, (_val))
#define PHY_MODIFY_BCM542XX_TEST1r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0000, 0x1e, \
                           (_val), (_mask))
#else       /* RDB_Reg. 0x00E */
#define PHY_READ_BCM542XX_TEST1r(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x00e, _val)
#define PHY_WRITE_BCM542XX_TEST1r(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x00e, _val)
#define PHY_MODIFY_BCM542XX_TEST1r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x00e, _val, _mask)
#endif

/* Top Level Configuration Register RDB_Reg. 0x810  */
#define PHY_READ_BCM542XX_TOP_LVL_CONFGr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x810, _val)
#define PHY_WRITE_BCM542XX_TOP_LVL_CONFGr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x810, _val)
#define PHY_MODIFY_BCM542XX_TOP_LVL_CONFGr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x810, _val, _mask)


/*          +------------------------------+
 *          |                              |
 *          |   Primary SerDes Registers   |
 *          |                              |
 *          +------------------------------+
 */
/* 1000BASE-X MII Control Register (Addr 00h) */
#define PHY_READ_BCM542XX_1000X_MII_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x00, (_val))
#define PHY_WRITE_BCM542XX_1000X_MII_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x00, (_val))
#define PHY_MODIFY_BCM542XX_1000X_MII_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x00, (_val), (_mask))
#define PHY_MODIFY_BCM542XX_PRI_SERDES_MII_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), PHY_BCM542XX_REG_PRI_SERDES, \
                             0x0000, 0x00, (_val), (_mask))

/* 1000BASE-X MII Status Register (Addr 01h) */
#define PHY_READ_BCM542XX_1000X_MII_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x01, (_val))
#define PHY_WRITE_BCM542XX_1000X_MII_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x01, (_val))
#define PHY_MODIFY_BCM542XX_1000X_MII_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x01, (_val), (_mask))

/* 1000BASE-X MII Auto-neg Advertise Register (Addr 04h) */
#define PHY_READ_BCM542XX_1000X_MII_ANAr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x04, (_val))
#define PHY_WRITE_BCM542XX_1000X_MII_ANAr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x04, (_val))
#define PHY_MODIFY_BCM542XX_1000X_MII_ANAr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x04, (_val), (_mask))

/* 1000BASE-X MII Auto-neg Link Partner Ability Register (Addr 05h) */
#define PHY_READ_BCM542XX_1000X_MII_ANPr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x05, (_val))
#define PHY_WRITE_BCM542XX_1000X_MII_ANPr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x05, (_val))
#define PHY_MODIFY_BCM542XX_1000X_MII_ANPr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x05, (_val), (_mask))

/* 1000BASE-X MII Auto-neg Extended Status Register (Addr 06h) */
#define PHY_READ_BCM542XX_1000X_MII_AN_EXPr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x06, (_val))
#define PHY_WRITE_BCM542XX_1000X_MII_AN_EXPr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x06, (_val))
#define PHY_MODIFY_BCM542XX_1000X_MII_AN_EXPr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x06, (_val), (_mask))

/* 1000BASE-X MII IEEE Extended Status Register (Addr 0fh) */
#define PHY_READ_BCM542XX_1000X_MII_EXT_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x0f, (_val))
#define PHY_WRITE_BCM542XX_1000X_MII_EXT_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x0f, (_val))
#define PHY_MODIFY_BCM542XX_1000X_MII_EXT_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), PHY_BCM542XX_REG_1000X, \
                             0x0000, 0x0f, (_val), (_mask))

/*          +-------------------------+
 *          |                         |
 *          |   Expansion Registers   |
 *          |                         |
 *          +-------------------------+
 */
/* Receive/Transmit Packet Counter Register (Addr 00h) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr 00h */
#define PHY_READ_BCM542XX_EXP_PKT_COUNTERr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0f00, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EXP_PKT_COUNTERr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0f00, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EXP_PKT_COUNTERr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0f00, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x030 */
#define PHY_READ_BCM542XX_EXP_PKT_COUNTERr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x030, _val)
#define PHY_WRITE_BCM542XX_EXP_PKT_COUNTERr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x030, _val)
#define PHY_MODIFY_BCM542XX_EXP_PKT_COUNTERr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x030, _val, _mask)
#endif

/* Expansion Interrupt Status Register (Addr 01h) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr 01h */
#define PHY_READ_BCM542XX_EXP_INTERRUPT_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0f01, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EXP_r(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0f01, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EXP_r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0f01, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x031 */
#define PHY_READ_BCM542XX_EXP_INTERRUPT_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x031, _val)
#define PHY_WRITE_BCM542XX_EXP_r(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x031, _val)
#define PHY_MODIFY_BCM542XX_EXP_r(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x031, _val, _mask)
#endif

/* Expansion Interrupt Mask Register (Addr 02h) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr 02h */
#define PHY_READ_BCM542XX_EXP_INTERRUPT_MASKr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0f02, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EXP_INTERRUPT_MASKr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0f02, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EXP_INTERRUPT_MASKr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0f02, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x032 */
#define PHY_READ_BCM542XX_EXP_INTERRUPT_MASKr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x032, _val)
#define PHY_WRITE_BCM542XX_EXP_INTERRUPT_MASKr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x032, _val)
#define PHY_MODIFY_BCM542XX_EXP_INTERRUPT_MASKr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x032, _val, _mask)
#endif

/* Multicolor LED Selector Register (Addr 04h) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr 04h */
#define PHY_READ_BCM542XX_EXP_LED_SELECTORr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0f04, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EXP_LED_SELECTORr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0f04, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EXP_LED_SELECTORr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0f04, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x034 */
#define PHY_READ_BCM542XX_EXP_LED_SELECTORr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x034, _val)
#define PHY_WRITE_BCM542XX_EXP_LED_SELECTORr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x034, _val)
#define PHY_MODIFY_BCM542XX_EXP_LED_SELECTORr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x034, _val, _mask)
#endif

/* Multicolor LED Flash Rate Controls Register (Addr 05h) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr 05h */
#define PHY_READ_BCM542XX_EXP_LED_FLASH_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0f05, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EXP_LED_FLASH_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0f05, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EXP_LED_FLASH_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0f05, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x035 */
#define PHY_READ_BCM542XX_EXP_LED_FLASH_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x035, _val)
#define PHY_WRITE_BCM542XX_EXP_LED_FLASH_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x035, _val)
#define PHY_MODIFY_BCM542XX_EXP_LED_FLASH_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x035, _val, _mask)
#endif

/* Multicolor LED Programmable Blink Controls Register (Addr 06h) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr 06h */
#define PHY_READ_BCM542XX_EXP_LED_BLINK_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0f06, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EXP_LED_BLINK_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0f06, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EXP_LED_BLINK_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0f06, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x036 */
#define PHY_READ_BCM542XX_EXP_LED_BLINK_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x036, _val)
#define PHY_WRITE_BCM542XX_EXP_LED_BLINK_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x036, _val)
#define PHY_MODIFY_BCM542XX_EXP_LED_BLINK_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x036, _val, _mask)
#endif

/* Expansion register 0x8 (For auto_early_dac_wake)*/
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr 08h */
#define PHY_READ_BCM542XX_EXP_EIGHTr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0f08, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EXP_EIGHTr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0f08, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EXP_EIGHTr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0f08, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x038 */
#define PHY_READ_BCM542XX_EXP_EIGHTr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x038, _val)
#define PHY_WRITE_BCM542XX_EXP_EIGHTr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x038, _val)
#define PHY_MODIFY_BCM542XX_EXP_EIGHTr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x038, _val, _mask)
#endif

/* Operating Mode Status Register (Addr 42h) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr 42h */
#define PHY_READ_BCM542XX_EXP_OPT_MODE_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0f42, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EXP_OPT_MODE_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0f42, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EXP_OPT_MODE_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0f42, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x051 */
#define PHY_READ_BCM542XX_EXP_OPT_MODE_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x051, _val)
#define PHY_WRITE_BCM542XX_EXP_OPT_MODE_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x051, _val)
#define PHY_MODIFY_BCM542XX_EXP_OPT_MODE_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x051, _val, _mask)
#endif

/* Pattern Generator Status Register (Addr 46h) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr  */
#define PHY_READ_BCM542XX_EXP_PATT_GEN_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0f46, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EXP_PATT_GEN_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0f46, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EXP_PATT_GEN_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0f46, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x006 */
#define PHY_READ_BCM542XX_EXP_PATT_GEN_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x006, _val)
#define PHY_WRITE_BCM542XX_EXP_PATT_GEN_STATr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x006, _val)
#define PHY_MODIFY_BCM542XX_EXP_PATT_GEN_STATr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x006, _val, _mask)
#endif
#define PHY_BCM542XX_PATT_GEN_STAT_FIFO_ELAST_1     BIT(14)
#define PHY_BCM542XX_PATT_GEN_STAT_GMII_FIFO_JUMBO_MSB BIT(15)

/* SerDes/SGMII RX Control Register (Addr 50h) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr  */
#define PHY_READ_BCM542XX_EXP_SERDES_SGMII_RXCTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0f50, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EXP_SERDES_SGMII_RXCTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0f50, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EXP_SERDES_SGMII_RXCTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0f50, 0x15, (_val), (_mask))
#else       /* RDB_Reg. [unknown] */
#define PHY_READ_BCM542XX_EXP_SERDES_SGMII_RXCTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0xfff, _val)
#define PHY_WRITE_BCM542XX_EXP_SERDES_SGMII_RXCTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0xfff, _val)
#define PHY_MODIFY_BCM542XX_EXP_SERDES_SGMII_RXCTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0xfff, _val, _mask)
#endif

/* SerDes/SGMII RX/TX Control Register (Addr 52h) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr  */
#define PHY_READ_BCM542XX_EXP_SERDES_SGMII_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0f52, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EXP_SERDES_SGMII_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0f52, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EXP_SERDES_SGMII_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0f52, 0x15, (_val), (_mask))
#else       /* RDB_Reg. [unknown] */
#define PHY_READ_BCM542XX_EXP_SERDES_SGMII_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0xfff, _val)
#define PHY_WRITE_BCM542XX_EXP_SERDES_SGMII_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0xfff, _val)
#define PHY_MODIFY_BCM542XX_EXP_SERDES_SGMII_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0xfff, _val, _mask)
#endif

/* External MACSec Interface Control Register (Addr 7fh) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr 7Fh */
#define PHY_READ_BCM542XX_EXP_EXT_MACSEC_IF_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0f7f, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EXP_EXT_MACSEC_IF_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0f7f, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EXP_EXT_MACSEC_IF_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0f7f, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x07F */
#define PHY_READ_BCM542XX_EXP_EXT_MACSEC_IF_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x07f, _val)
#define PHY_WRITE_BCM542XX_EXP_EXT_MACSEC_IF_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x07f, _val)
#define PHY_MODIFY_BCM542XX_EXP_EXT_MACSEC_IF_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x07f, _val, _mask)
#endif

/* Time Sync */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr F5h */
#define PHY_READ_BCM542XX_TIME_SYNC_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0ff5, 0x15, (_val))
#define PHY_WRITE_BCM542XX_TIME_SYNC_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0ff5, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_TIME_SYNC_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0ff5, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x0F5 */
#define PHY_READ_BCM542XX_TIME_SYNC_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x0f5, _val)
#define PHY_WRITE_BCM542XX_TIME_SYNC_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x0f5, _val)
#define PHY_MODIFY_BCM542XX_TIME_SYNC_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x0f5, _val, _mask)
#endif

/* EEE Statistics Registers*/

/* EEE Statistic Timer_12_hours_lpi remote Reg (Addr aah) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr AAh */
#define PHY_READ_BCM542XX_EEE_STAT_TX_DURATIONr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0faa, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EEE_STAT_TX_DURATIONr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0faa, 0x15, (_val))
#define PHY_MODIFY_BCM542XX__EEE_STAT_TX_DURATIONr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0faa, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x0AA */
#define PHY_READ_BCM542XX_EEE_STAT_TX_DURATIONr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x0aa, _val)
#define PHY_WRITE_BCM542XX_EEE_STAT_TX_DURATIONr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x0aa, _val)
#define PHY_MODIFY_BCM542XX__EEE_STAT_TX_DURATIONr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x0aa, _val, _mask)
#endif

/* EEE Statistic Timer_12_hours_lpi local Reg (Addr abh) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr ABh */
#define PHY_READ_BCM542XX_EEE_STAT_RX_DURATIONr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0fab, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EEE_STAT_RX_DURATIONr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0fab, 0x15, (_val))
#define PHY_MODIFY_BCM542XX__EEE_STAT_RX_DURATIONr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0fab, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x0AB */
#define PHY_READ_BCM542XX_EEE_STAT_RX_DURATIONr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x0ab, _val)
#define PHY_WRITE_BCM542XX_EEE_STAT_RX_DURATIONr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x0ab, _val)
#define PHY_MODIFY_BCM542XX__EEE_STAT_RX_DURATIONr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x0ab, _val, _mask)
#endif

/* EEE Local LPI request Counter Reg (Addr ACh) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr ACh */
#define PHY_READ_BCM542XX_EEE_STAT_TX_EVENTSr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0fac, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EEE_STAT_TX_EVENTSr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0fac, 0x15, (_val))
#define PHY_MODIFY_BCM542XX__EEE_STAT_TX_EVENTSr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0fac, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x0AC */
#define PHY_READ_BCM542XX_EEE_STAT_TX_EVENTSr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x0ac, _val)
#define PHY_WRITE_BCM542XX_EEE_STAT_TX_EVENTSr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x0ac, _val)
#define PHY_MODIFY_BCM542XX__EEE_STAT_TX_EVENTSr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x0ac, _val, _mask)
#endif

/* EEE Remote LPI request Counter Reg (Addr ADh) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr ADh */
#define PHY_READ_BCM542XX_EEE_STAT_RX_EVENTSr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0fad, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EEE_STAT_RX_EVENTSr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0fad, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EEE_STAT_RX_EVENTSr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0fad, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x0AD */
#define PHY_READ_BCM542XX_EEE_STAT_RX_EVENTSr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x0ad, _val)
#define PHY_WRITE_BCM542XX_EEE_STAT_RX_EVENTSr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x0ad, _val)
#define PHY_MODIFY_BCM542XX_EEE_STAT_RX_EVENTSr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x0ad, _val, _mask)
#endif

/* EEE Statistic counters ctrl/status (Addr AFh) */
#ifdef PHY_BCM542XX_DEFAULT_TO_LEGACY_ADDRESSING   /* Exp.Addr AFh */
#define PHY_READ_BCM542XX_EEE_STAT_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_RD((_unit), (_pc), 0x00, 0x0faf, 0x15, (_val))
#define PHY_WRITE_BCM542XX_EEE_STAT_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_REG_WR((_unit), (_pc), 0x00, 0x0faf, 0x15, (_val))
#define PHY_MODIFY_BCM542XX_EEE_STAT_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_REG_MOD((_unit), (_pc), 0x00, 0x0faf, 0x15, (_val), (_mask))
#else       /* RDB_Reg. 0x0AF */
#define PHY_READ_BCM542XX_EEE_STAT_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_RD(_unit, _pc, 0x0af, _val)
#define PHY_WRITE_BCM542XX_EEE_STAT_CTRLr(_unit, _pc, _val) \
            PHY_BCM542XX_RDB_WR(_unit, _pc, 0x0af, _val)
#define PHY_MODIFY_BCM542XX_EEE_STAT_CTRLr(_unit, _pc, _val, _mask) \
            PHY_BCM542XX_RDB_MO(_unit, _pc, 0x0af, _val, _mask)
#endif

/*
 *
 * CL45 Registers: EEE
 */
/* EEE Capability Register */
#define PHY_READ_BCM542XX_EEE_CAPr( _unit, _pc, _val) \
        PHY_BCM542XX_CL45_REG_READ( (_unit), (_pc), 0x3, 0x14, (_val))

/* EEE Advertisement Register */
#define PHY_READ_BCM542XX_EEE_ADVr(_unit, _pc, _val) \
        PHY_BCM542XX_CL45_REG_READ( (_unit), (_pc), 0x7, 0x3c, _val)
#define PHY_WRITE_BCM542XX_EEE_ADVr(_unit, _pc, _val) \
        PHY_BCM542XX_CL45_REG_WRITE((_unit), (_pc),  0x7, 0x3c, (_val))
#define PHY_MODIFY_BCM542XX_EEE_ADVr(_unit, _pc, _val, _mask) \
        PHY_BCM542XX_CL45_REG_MODIFY((_unit), (_pc), 0x7, 0x3c, (_val), (_mask))

#define PHY_READ_BCM542XX_EEE_803Dr(_unit, _pc, _val) \
        PHY_BCM542XX_CL45_REG_READ(_unit, _pc,  0x7, 0x803d, _val)
#define PHY_WRITE_BCM542XX_EEE_803Dr(_unit, _pc, _val) \
        PHY_BCM542XX_CL45_REG_WRITE(_unit, _pc,  0x7, 0x803d, _val)
#define PHY_MODIFY_BCM542XX_EEE_803Dr(_unit, _pc, _val, _mask) \
        PHY_BCM542XX_CL45_REG_MODIFY(_unit, _pc, 0x7, 0x803d, _val, _mask)

/* EEE Resolution Status Register */
#define PHY_READ_BCM542XX_EEE_RESOLUTION_STATr( _unit, _pc, _val) \
        PHY_BCM542XX_CL45_REG_READ(_unit, _pc,  0x7, 0x803e, _val)
#define PHY_WRITE_BCM542XX_EEE_RESOLUTION_STATr(_unit, _pc, _val) \
        PHY_BCM542XX_CL45_REG_WRITE(_unit, _pc, 0x7, 0x803e, _val)
#define PHY_MODIFY_BCM542XX_EEE_RESOLUTION_STATr(_unit, _pc, _val, _mask) \
        PHY_BCM542XX_CL45_REG_MODIFY(_unit, _pc,, 0x7, 0x803e, _val, _mask)

#define PHY_READ_BCM542XX_EEE_TEST_CTRL_Ar(_unit, _pc, _val) \
        PHY_BCM542XX_CL45_REG_READ( _unit, _pc, 0x7, 0x8031, _val)
#define PHY_WRITE_BCM542XX_EEE_TEST_CTRL_Ar(_unit, _pc, _val) \
        PHY_BCM542XX_CL45_REG_WRITE( _unit, _pc, 0x7, 0x8031, _val)
#define PHY_MODIFY_BCM542XX_EEE_TEST_CTRL_Ar(_unit, _pc, _val, _mask) \
        PHY_BCM542XX_CL45_REG_MODIFY(_unit, _pc, 0x7, 0x8031, _val, _mask)

/* EEE capability advertisement */
#define PHY_BCM542XX_EEE_ADV_10G     (BIT3)   /* EEE advertise for  10GBASE-T */
#define PHY_BCM542XX_EEE_ADV_1000    (BIT2)   /* EEE advertise for 1000BASE-T */
#define PHY_BCM542XX_EEE_ADV_100     (BIT1)   /* EEE advertise for  100BASE-T */
#define PHY_BCM542XX_EEE_ADV_MASK    (BIT3 | BIT2 | BIT1)

/*                         *\
|*  PHY Device Descriptor  *|
\*                         */
typedef struct {
    soc_port_config_phy_oam_t oam_config;
    uint32 flags;
    uint16 phy_id_orig;
    uint16 phy_id_base; /* port 0 addr */
    uint16 phy_slice;
    int32 port_pre_speed;
} PHY_BCM542XX_DEV_DESC_t;

#define PHY_BCM542XX_DEV_OAM_CONFIG_PTR(_pc) \
              (&(((PHY_BCM542XX_DEV_DESC_t *)((_pc) + 1))->oam_config))
#define PHY_BCM542XX_DEV_PHY_ID_ORIG(_pc) \
                (((PHY_BCM542XX_DEV_DESC_t *)((_pc) + 1))->phy_id_orig)
#define PHY_BCM542XX_DEV_PHY_ID_BASE(_pc) \
                (((PHY_BCM542XX_DEV_DESC_t *)((_pc) + 1))->phy_id_base)
#define PHY_BCM542XX_DEV_PHY_SLICE(_pc) \
                (((PHY_BCM542XX_DEV_DESC_t *)((_pc) + 1))->phy_slice)
#define PHY_BCM542XX_FLAGS(_pc) \
                (((PHY_BCM542XX_DEV_DESC_t *)((_pc) + 1))->flags)
#define PHY_BCM542XX_PHYADDR_REVERSE                    (BIT0)
#define PHY_BCM542XX_REAR_HALF                          (BIT1)
#define PHY_BCM542XX_SYS_SIDE_AUTONEG                   (BIT2)

#define PHY_BCM542XX_SYS_SIDE_AUTONEG_SET(_pc)     \
                do { PHY_BCM542XX_FLAGS(_pc) |=  PHY_BCM542XX_SYS_SIDE_AUTONEG; } while (0)
#define PHY_BCM542XX_SYS_SIDE_AUTONEG_CLR(_pc)     \
                do { PHY_BCM542XX_FLAGS(_pc) &= ~PHY_BCM542XX_SYS_SIDE_AUTONEG; } while (0)
#define PHY_BCM542XX_IS_SYS_SIDE_AUTONEG(_pc)      \
                    (PHY_BCM542XX_FLAGS(_pc) &   PHY_BCM542XX_SYS_SIDE_AUTONEG)

#define PHY_BCM542XX_REAR_HALF_SET(_pc) \
                do { PHY_BCM542XX_FLAGS(_pc) |=  PHY_BCM542XX_REAR_HALF; } while (0)
#define PHY_BCM542XX_REAR_HALF_CLR(_pc) \
                do { PHY_BCM542XX_FLAGS(_pc) &= ~PHY_BCM542XX_REAR_HALF; } while (0)
#define PHY_BCM542XX_IS_REAR_HALF(_pc)  \
                    (PHY_BCM542XX_FLAGS(_pc) &   PHY_BCM542XX_REAR_HALF)
#define PHY_BCM542XX_DEV_SET_BASE_ADDR(_pc) \
            PHY_BCM542XX_DEV_PHY_ID_BASE(_pc) = \
                (PHY_BCM542XX_FLAGS((_pc)) & PHY_BCM542XX_PHYADDR_REVERSE) \
                 ? PHY_BCM542XX_DEV_PHY_ID_ORIG(_pc) + PHY_BCM542XX_DEV_PHY_SLICE(_pc) \
                 : PHY_BCM542XX_DEV_PHY_ID_ORIG(_pc) - PHY_BCM542XX_DEV_PHY_SLICE(_pc)

#define PHY_BCM542XX_SLICE_ADDR(_pc, _slice) \
                (PHY_BCM542XX_FLAGS((_pc)) & PHY_BCM542XX_PHYADDR_REVERSE) \
                 ? (PHY_BCM542XX_DEV_PHY_ID_BASE(_pc) - (_slice)) \
                 : (PHY_BCM542XX_DEV_PHY_ID_BASE(_pc) + (_slice))

#endif /* _PHY542XX_INT_H */
