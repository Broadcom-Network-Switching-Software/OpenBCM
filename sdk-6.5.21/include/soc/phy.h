/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Useful constants and routines for PHY chips which
 * run with Orion (10/100/1000 Mb/s enet) and use MII, GMII,
 * or 8b/10b Serdes interfaces.
 *
 * See Also: IEEE 802.3 (1998) Local and Metropolitan Area Networks
 * Sections 22.2.4-XX
 */

#ifndef _SOC_PHY_H
#define _SOC_PHY_H

#include <soc/phy/drv.h>
#undef   BIT
#define  BIT(_n_)      (1U << (_n_))

/* Standard MII Registers */

#define MII_CTRL_REG            0x00    /* MII Control Register : r/w */
#define MII_STAT_REG            0x01    /* MII Status Register: ro */ 
#define MII_PHY_ID0_REG         0x02    /* MII PHY ID register: r/w */
#define MII_PHY_ID1_REG         0x03    /* MII PHY ID register: r/w */
#define MII_ANA_REG             0x04    /* MII Auto-Neg Advertisement: r/w */
#define MII_ANP_REG             0x05    /* MII Auto-Neg Link Partner: ro */
#define MII_AN_EXP_REG          0x06    /* MII Auto-Neg Expansion: ro */
#define MII_GB_CTRL_REG         0x09    /* MII 1000Base-T control register */
#define MII_GB_STAT_REG         0x0a    /* MII 1000Base-T Status register */
#define MII_ESR_REG             0x0f    /* MII Extended Status register */

/* Non-standard MII Registers */

#define MII_ECR_REG             0x10    /* MII Extended Control Register */
#define MII_AUX_REG             0x18       /* MII Auxiliary Control/Status Register */
#define MII_ASSR_REG            0x19    /* MII Auxiliary Status Summary Reg */
#define MII_GSR_REG             0x1c    /* MII General status (BROADCOM) */
#define MII_MSSEED_REG          0x1d    /* MII Master/slave seed (BROADCOM) */
#define MII_AUX_MULTI_PHY_REG   0x1e    /* Auxiliary Multiple PHY (BROADCOM) */
#define MII_TEST2_REG           0x1f    /* MII Test reg (BROADCOM) */

#define MII_SHA_CD_CTRL_REG     0x13 /*13h-0;13h-1;13h-2;13h-3;
                                  *13h-4;13h-5;13h-6;13h-7 */
#define MII_SHA_CD_SEL_REG      0x14 
#define MII_SHA_AUX_STAT2_REG   0x1B 
#define MII_BRCM_TEST_REG       0x1F


/* MII Control Register: bit definitions */

#define MII_CTRL_FS_2500        (1 << 5) /* Force speed to 2500 Mbps */
#define MII_CTRL_SS_MSB         (1 << 6) /* Speed select, MSb */
#define MII_CTRL_CST            (1 << 7) /* Collision Signal test */
#define MII_CTRL_FD             (1 << 8) /* Full Duplex */
#define MII_CTRL_RAN            (1 << 9) /* Restart Autonegotiation */
#define MII_CTRL_IP             (1 << 10) /* Isolate Phy */
#define MII_CTRL_PD             (1 << 11) /* Power Down */
#define MII_CTRL_AE             (1 << 12) /* Autonegotiation enable */
#define MII_CTRL_SS_LSB         (1 << 13) /* Speed select, LSb */
#define MII_CTRL_LE             (1 << 14) /* Loopback enable */
#define MII_CTRL_RESET          (1 << 15) /* PHY reset */

#define MII_CTRL_SS(_x)         ((_x) & (MII_CTRL_SS_LSB|MII_CTRL_SS_MSB))
#define MII_CTRL_SS_10          0
#define MII_CTRL_SS_100         (MII_CTRL_SS_LSB)
#define MII_CTRL_SS_1000        (MII_CTRL_SS_MSB)
#define MII_CTRL_SS_INVALID     (MII_CTRL_SS_LSB | MII_CTRL_SS_MSB)
#define MII_CTRL_SS_MASK        (MII_CTRL_SS_LSB | MII_CTRL_SS_MSB)

/* 
 * MII Status Register: See 802.3, 1998 pg 544 
 */
#define MII_STAT_EXT            (1 << 0) /* Extended Registers */
#define MII_STAT_JBBR           (1 << 1) /* Jabber Detected */
#define MII_STAT_LA             (1 << 2) /* Link Active */
#define MII_STAT_AN_CAP         (1 << 3) /* Autoneg capable */
#define MII_STAT_RF             (1 << 4) /* Remote Fault */
#define MII_STAT_AN_DONE        (1 << 5) /* Autoneg complete */
#define MII_STAT_MF_PS          (1 << 6) /* Preamble suppression */
#define MII_STAT_ES             (1 << 8) /* Extended status (R15) */
#define MII_STAT_HD_100_T2      (1 << 9) /* Half duplex 100Mb/s supported */
#define MII_STAT_FD_100_T2      (1 << 10)/* Full duplex 100Mb/s supported */
#define MII_STAT_HD_10          (1 << 11)/* Half duplex 100Mb/s supported */
#define MII_STAT_FD_10          (1 << 12)/* Full duplex 100Mb/s supported */
#define MII_STAT_HD_100         (1 << 13)/* Half duplex 100Mb/s supported */
#define MII_STAT_FD_100         (1 << 14)/* Full duplex 100Mb/s supported */
#define MII_STAT_100_T4         (1 << 15)/* Full duplex 100Mb/s supported */

/*
 * MII Link Advertisment
 */
#define MII_ANA_ASF             (1 << 0)/* Advertise Selector Field */
#define MII_ANA_HD_10           (1 << 5)/* Half duplex 10Mb/s supported */
#define MII_ANA_FD_10           (1 << 6)/* Full duplex 10Mb/s supported */
#define MII_ANA_HD_100          (1 << 7)/* Half duplex 100Mb/s supported */
#define MII_ANA_FD_100          (1 << 8)/* Full duplex 100Mb/s supported */
#define MII_ANA_T4              (1 << 9)/* T4 */
#define MII_ANA_PAUSE           (1 << 10)/* Pause supported */
#define MII_ANA_ASYM_PAUSE      (1 << 11)/* Asymmetric pause supported */
#define MII_ANA_RF              (1 << 13)/* Remote fault */
#define MII_ANA_NP              (1 << 15)/* Next Page */

#define MII_ANA_ASF_802_3       (1)     /* 802.3 PHY */

/*
 * MII Link Advertisment (Clause 37) 
 */
#define MII_ANA_C37_NP          (1 << 15)  /* Next Page */
#define MII_ANA_C37_RF_OK       (0 << 12)  /* No error, link OK */
#define MII_ANA_C37_RF_LINK_FAIL (1 << 12)  /* Offline */
#define MII_ANA_C37_RF_OFFLINE  (2 << 12)  /* Link failure */
#define MII_ANA_C37_RF_AN_ERR   (3 << 12)  /* Auto-Negotiation Error */
#define MII_ANA_C37_PAUSE       (1 << 7)   /* Symmetric Pause */
#define MII_ANA_C37_ASYM_PAUSE  (1 << 8)   /* Asymmetric Pause */
#define MII_ANA_C37_HD          (1 << 6)   /* Half duplex */
#define MII_ANA_C37_FD          (1 << 5)   /* Full duplex */ 

/*
 * MII Link Partner Ability (Clause 37)
 */
#define MII_ANP_C37_NP          (1 << 15)  /* Next Page */
#define MII_ANP_C37_ACK         (1 << 14)  /* Acknowledge */
#define MII_ANP_C37_RF_OK       (0 << 12)  /* No error, link OK */
#define MII_ANP_C37_RF_LINK_FAIL (1 << 12)  /* Offline */
#define MII_ANP_C37_RF_OFFLINE  (2 << 12)  /* Link failure */
#define MII_ANP_C37_RF_AN_ERR   (3 << 12)  /* Auto-Negotiation Error */
#define MII_ANP_C37_PAUSE       (1 << 7)   /* Symmetric Pause */
#define MII_ANP_C37_ASYM_PAUSE  (1 << 8)   /* Asymmetric Pause */
#define MII_ANP_C37_HD          (1 << 6)   /* Half duplex */
#define MII_ANP_C37_FD          (1 << 5)   /* Full duplex */ 

/*
 * MII Link Partner Ability (SGMII)
 */
#define MII_ANP_SGMII_MODE      (1 << 0)   /* Link parneter in SGMII mode */
#define MII_ANP_SGMII_FD        (1 << 12)  /* SGMII duplex */
#define MII_ANP_SGMII_SS_10     (0 << 10)  /* 10Mbps SGMII */
#define MII_ANP_SGMII_SS_100    (1 << 10)  /* 100Mbps SGMII */
#define MII_ANP_SGMII_SS_1000   (2 << 10)  /* 1000Mbps SGMII */
#define MII_ANP_SGMII_SS_MASK   (3 << 10)  /* SGMII speed mask */
#define MII_ANP_SGMII_ACK       (1 << 14)  /* SGMII Ackonwledge */
#define MII_ANP_SGMII_LINK      (1 << 15)  /* SGMII Link */


/*
 * 1000Base-T Control Register
 */
#define MII_GB_CTRL_MS_MAN      (1 << 12) /* Manual Master/Slave mode */
#define MII_GB_CTRL_MS          (1 << 11) /* Master/Slave negotiation mode */
#define MII_GB_CTRL_PT          (1 << 10) /* Port type */
#define MII_GB_CTRL_ADV_1000FD  (1 << 9) /* Advertise 1000Base-T FD */
#define MII_GB_CTRL_ADV_1000HD  (1 << 8) /* Advertise 1000Base-T HD */

/*
 * 1000Base-T Status Register
 */
#define MII_GB_STAT_MS_FAULT    (1 << 15) /* Master/Slave Fault */
#define MII_GB_STAT_MS          (1 << 14) /* Master/Slave, 1 == Master */
#define MII_GB_STAT_LRS         (1 << 13) /* Local receiver status */
#define MII_GB_STAT_RRS         (1 << 12) /* Remote receiver status */
#define MII_GB_STAT_LP_1000FD   (1 << 11) /* Link partner 1000FD capable */
#define MII_GB_STAT_LP_1000HD   (1 << 10) /* Link partner 1000HD capable */
#define MII_GB_STAT_IDE         (0xff << 0) /* Idle error count */

/*
 * IEEE Extended Status Register
 */
#define MII_ESR_1000_X_FD       (1 << 15) /* 1000Base-T FD capable */
#define MII_ESR_1000_X_HD       (1 << 14) /* 1000Base-T HD capable */
#define MII_ESR_1000_T_FD       (1 << 13) /* 1000Base-T FD capable */
#define MII_ESR_1000_T_HD       (1 << 12) /* 1000Base-T FD capable */

/*
 * MII Extended Control Register (BROADCOM)
 */
#define MII_ECR_FE              (1 << 0) /* FIFO Elasticity */
#define MII_ECR_TLLM            (1 << 1) /* Three link LED mode */
#define MII_ECR_ET_IPG          (1 << 2) /* Extended XMIT IPG mode */
#define MII_ECR_FLED_OFF        (1 << 3) /* Force LED off */
#define MII_ECR_FLED_ON         (1 << 4) /* Force LED on */
#define MII_ECR_ELT             (1 << 5) /* Enable LED traffic */
#define MII_ECR_RS              (1 << 6) /* Reset Scrambler */
#define MII_ECR_BRSA            (1 << 7) /* Bypass Receive Sym. align */
#define MII_ECR_BMLT3           (1 << 8) /* Bypass MLT3 Encoder/Decoder */
#define MII_ECR_BSD             (1 << 9) /* Bypass Scramble/Descramble */
#define MII_ECR_B4B5B           (1 << 10) /* Bypass 4B/5B Encode/Decode */
#define MII_ECR_FI              (1 << 11) /* Force Interrupt */
#define MII_ECR_ID              (1 << 12) /* Interrupt Disable */
#define MII_ECR_TD              (1 << 13) /* XMIT Disable */
#define MII_ECR_DAMC            (1 << 14) /* DIsable Auto-MDI Crossover */
#define MII_ECR_10B             (1 << 15) /* 1 == 10B, 0 == GMII */

/*
 * GSR (BROADCOM)
 */
#define MII_GSR_FD              (1 << 0) /* Full duplex active */
#define MII_GSR_SI              (1 << 1) /* Speed 0-->10, 1 --> 100 */
#define MII_GSR_FORCE           (1 << 2) /* Force 0-->10, 1--> 100 */
#define MII_GSR_AN              (1 << 3) /* Autonegotiation enabled */

/*
 * Auxiliary Control/Status Register
 */
#define MII_AUX_LD          (1<<14) /* Link Integrity test disabled in PHY */
  
/*
 * Auxiliary Status Summary Register (ASSR - Broadcom BCM5400)
 */
#define MII_ASSR_PRTD           (1 << 0) /* Pause resolution/XMIT direction */
#define MII_ASSR_PRRD           (1 << 1) /* Pause resolution/RCV direction */
#define MII_ASSR_LS             (1 << 2) /* Link Status (1 == link up) */
#define MII_ASSR_LPNPA          (1 << 3) /* Link partner next page cap */
#define MII_ASSR_LPANA          (1 << 4) /* Link Partner AN capable */
#define MII_ASSR_ANPR           (1 << 5) /* Autoneg page received */
#define MII_ASSR_RF             (1 << 6) /* Remote Fault */
#define MII_ASSR_PDF            (1 << 7) /* Parallel detection fault */
#define MII_ASSR_HCD            (7 << 8) /* Current operating speed */
#define MII_ASSR_ANNPW          (1 << 11) /* Auto-neg next page wait */
#define MII_ASSR_ANABD          (1 << 12) /* Auto-neg Ability detected */
#define MII_ASSR_ANAD           (1 << 13) /* Auto-neg ACK detect */
#define MII_ASSR_ANCA           (1 << 14) /* Auto-neg complete ACK */
#define MII_ASSR_ANC            (1 << 15) /* AUto-neg complete */

#define MII_ASSR_HCD_FD_1000    (7 << 8)
#define MII_ASSR_HCD_HD_1000    (6 << 8)
#define MII_ASSR_HCD_FD_100     (5 << 8)
#define MII_ASSR_HCD_T4_100     (4 << 8)
#define MII_ASSR_HCD_HD_100     (3 << 8)
#define MII_ASSR_HCD_FD_10      (2 << 8)
#define MII_ASSR_HCD_HD_10      (1 << 8)
#define MII_ASSR_HCD_NC         (0 << 8) /* Not complete */

#define MII_TEST2_MS_SEL        (1 << 11)

/* 
 * MII Broadcom Test Register(MII_SHA_CD_CTRL_REG)
 */
#define MII_SHA_CD_CTRL_PB_STAT (0x3 << 12) /* 13-0 pair B status */
#define MII_SHA_CD_CTRL_PA_STAT (0x3 << 10) /* 13-0 pair A status */
#define MII_SHA_CD_CTRL_START   (0x1 << 2)  /* 13-0 start bit */
#define MII_SHA_CD_CTRL_PB_LEN  (0xFF << 8)  /* 13-1 pair B length */
#define MII_SHA_CD_CTRL_PA_LEN  (0xFF)       /* 13-1 pair A length */

/* 
 * MII Broadcom Test Register(MII_SHA_AUX_STAT2_REG)
 */
#define MII_SHA_AUX_STAT2_LEN   (0x7 << 12) /* Enable Shadow registers */

/* 
 * MII Broadcom Test Register(MII_BRCM_TEST_REG): bit definitions
 */
#define MII_BRCM_TEST_ENSHA         (1 << 7)    /* Enable Shadow registers */
#define MII_BRCM_TEST_REG_DEFAULT   0x000B      /* Register's default value */

/* 
 * MII Broadcom 
 * Auxiliary Multiple PHY Register(MII_AUX_MULTI_PHY_REG): bit definitions
 */
#define MII_BRCM_AUX_MULTI_PHY_SUPER_ISO (1 << 3) /* Super Isolate registers */
#define MII_BRCM_AUX_MULTI_PHY_RAN       (1 << 8) /* Restart-AN registers */

#define GOOD_CABLE_LEN_TUNING   35

#define CABLE_FUZZY_LEN1        10      /* for established good link */ 
#define CABLE_FUZZY_LEN2        1

#define MII_MSSEED_SEED         0x3ff

#define PHY_MIN_REG             0
#define PHY_MAX_REG             0x1f

/* Clause 45 MIIM */
#define PHY_C45_DEV_RESERVED    0x00
#define PHY_C45_DEV_PMA_PMD     0x01
#define PHY_C45_DEV_WIS         0x02
#define PHY_C45_DEV_PCS         0x03
#define PHY_C45_DEV_PHYXS       0x04
#define PHY_C45_DEV_DTEXS       0x05
#define PHY_C45_DEV_AN          0x07
#define PHY_C45_DEV_USER        0x1e

/* LRE/BroadR-Reach (Broadcom) Registers */

#define LRE_CTRL_REG            0x00    /* LRE Control Register (Addr 00h) */
#define LRE_STAT_REG            0x01    /* LRE Status Register (ADDR 01h) */ 
#define LRE_PHY_ID0_REG         0x02    /* LRE PHY Identifier Register (ADDR 02h) */
#define LRE_PHY_ID1_REG         0x03    /* LRE PHY Identifier Register (ADDR 03h) */
#define LDS_ADVA_REG            0x04    /* LDS Advertised Ability Register (ADDR 04h) */
#define LDS_ADVC_REG            0x05    /* LDS Advertised Control Register (ADDR 05h)  */
#define LDS_ABNP_REG            0x06    /* LDS Ability Next Page Register (ADDR 06h) */
#define LDS_LPAB_REG            0x07    /* LDS Link Partner Ability Register (ADDR 07h) */
#define LDS_LPNP_REG            0x08    /* LDS Link Partner Next Page Message Register (ADDR 08h) */
#define LDS_LPABNP_REG          0x09    /* LDS Ability Next Page Register (ADDR 09h) */
#define LDS_EXP_REG             0x0a    /* LDS Expansion Register (ADDR 0ah) */
#define LDS_ACC_REG             0x0e    /* LDS Access Register (ADDR 0eh) */
#define LRE_EXTEN_REG           0x0f    /* LRE Extended Status Register (ADDR 0fh) */

/* LRE Control Register: bit definitions */

#define LRE_CTRL_UE            (1 << 2)   /* Unidirectional Enable */
#define LRE_CTRL_MS            (1 << 3)   /* Master Selection */

#define LRE_CTRL_PS_MASK       (0x3 << 4) /* Pair Selection Mask */
#define LRE_CTRL_PS_1P         (0x0 << 4) /* 1 Pair */
#define LRE_CTRL_PS_2P         (0x1 << 4) /* 2 Pairs */
#define LRE_CTRL_PS_4P         (0x2 << 4) /* 4 Pairs */

#define LRE_CTRL_SS_MASK       (0xf << 6) /* Speed Selection Mask */
#define LRE_CTRL_SS_10         (0x0 << 6) /* 10Mbps */
#define LRE_CTRL_SS_50         (0x1 << 6) /* 50Mbps */
#define LRE_CTRL_SS_33         (0x2 << 6) /* 33Mbps */
#define LRE_CTRL_SS_25         (0x3 << 6) /* 25Mbps */
#define LRE_CTRL_SS_20         (0x4 << 6) /* 20Mbps */
#define LRE_CTRL_SS_100        (0x8 << 6) /* 100Mbps */

#define LRE_CTRL_IP            (1 << 10)  /* Isolate Phy */
#define LRE_CTRL_PD            (1 << 11)  /* Power Down */
#define LRE_CTRL_LDSE          (1 << 12)  /* LDS Enable */
#define LRE_CTRL_RLDS          (1 << 13)  /* Restart LDS */
#define LRE_CTRL_LE            (1 << 14)  /* Loopback Enable */
#define LRE_CTRL_RESET         (1 << 15)  /* Reset */


/* LRE Status Register: bit definitions */

#define LRE_STAT_EXT           (1 << 0)   /* Extended Capability */
#define LRE_STAT_JBBR          (1 << 1)   /* Jabber Detected */
#define LRE_STAT_LA            (1 << 2)   /* Link Active */
#define LRE_STAT_LDS_CAP       (1 << 3)   /* LDS capable */
#define LRE_STAT_IEEE_CAP      (1 << 4)   /* Supports 802.3 PHY */
#define LRE_STAT_LDS_DONE      (1 << 5)   /* LDS Done */
#define LRE_STAT_MF_PS         (1 << 6)   /* Preamble Suppression */
#define LRE_STAT_UA            (1 << 7)   /* Unidirectional Ability */
#define LRE_STAT_ES            (1 << 8)   /* Extended Status */

#define LRE_STAT_1_10          (1 << 9)   /* 1 pair 10Mbps capable */
#define LRE_STAT_2_10          (1 << 10)  /* 2 pair 10Mbps capable */
#define LRE_STAT_2_100         (1 << 11)  /* 2 pair 100Mbps capable */
#define LRE_STAT_4_100         (1 << 12)  /* 4 pair 100Mbps capable */
#define LRE_STAT_1_100         (1 << 13)  /* 1 pair 100Mbps capable */


/* LDS Advertised Ability Register: bit definitions */

#define LDS_ADVA_1_10          (1 << 1)   /* Supports 1 pair 10Mbps */
#define LDS_ADVA_2_10          (1 << 2)   /* Supports 2 pair 10Mbps */
#define LDS_ADVA_2_100         (1 << 3)   /* Supports 2 pair 100Mbps */
#define LDS_ADVA_4_100         (1 << 4)   /* Supports 4 pair 100Mbps */
#define LDS_ADVA_1_100         (1 << 5)   /* Supports 1 pair 100Mbps */
#define LDS_ADVA_1_20          (1 << 6)   /* Supports 1 pair 20Mbps */
#define LDS_ADVA_1_25          (1 << 7)   /* Supports 1 pair 25Mbps */
#define LDS_ADVA_1_33          (1 << 8)   /* Supports 1 pair 33Mbps */
#define LDS_ADVA_1_50          (1 << 9)   /* Supports 1 pair 33Mbps */
#define LDS_ADVA_2_20          (1 << 10)  /* Supports 2 pair 20Mbps */
#define LDS_ADVA_2_25          (1 << 11)  /* Supports 2 pair 25Mbps */
#define LDS_ADVA_2_33          (1 << 12)  /* Supports 2 pair 33Mbps */
#define LDS_ADVA_2_50          (1 << 13)  /* Supports 2 pair 33Mbps */

#define LDS_ADVA_PAUSE         (1 << 14)  /* Supports Pause */
#define LDS_ADVA_ASYM_PAUSE    (1 << 15)  /* Supports Asymetric Pause */

/* LDS Advertised Control Register: bit definitions */

#define LDS_ADVC_AFU            (1 << 8)   /* ADVA register updated */

/* LDS Link Partner Ability Register: bit definitions */

#define LDS_LPAB_1_10          (1 << 1)   /* Supports 1 pair 10Mbps */
#define LDS_LPAB_2_10          (1 << 2)   /* Supports 2 pair 10Mbps */
#define LDS_LPAB_2_100         (1 << 3)   /* Supports 2 pair 100Mbps */
#define LDS_LPAB_4_100         (1 << 4)   /* Supports 4 pair 100Mbps */
#define LDS_LPAB_1_100         (1 << 5)   /* Supports 1 pair 100Mbps */
#define LDS_LPAB_1_20          (1 << 6)   /* Supports 1 pair 20Mbps */
#define LDS_LPAB_1_25          (1 << 7)   /* Supports 1 pair 25Mbps */
#define LDS_LPAB_1_33          (1 << 8)   /* Supports 1 pair 33Mbps */
#define LDS_LPAB_1_50          (1 << 9)   /* Supports 1 pair 33Mbps */
#define LDS_LPAB_2_20          (1 << 10)  /* Supports 2 pair 20Mbps */
#define LDS_LPAB_2_25          (1 << 11)  /* Supports 2 pair 25Mbps */
#define LDS_LPAB_2_33          (1 << 12)  /* Supports 2 pair 33Mbps */
#define LDS_LPAB_2_50          (1 << 13)  /* Supports 2 pair 33Mbps */
#define LDS_LPAB_PAUSE         (1 << 14)  /* Supports Pause */
#define LDS_LPAB_ASYM_PAUSE    (1 << 15)  /* Supports Asymetric Pause */


/* LDS Access Register: bit definitions */

#define LDS_ACC_STAT           (1 << 0)   /* LRE Register Acress Status */
#define LDS_ACC_OVV            (1 << 1)   /* LRE Register Acress Override Value */
#define LDS_ACC_OVR            (1 << 2)   /* LRE Register Acress Override */


/* Initial BR mode settings */
#define LR_INITIAL_MODE_LR_ENABLE (1 << 0) /* Enable BroadReach at init() */

/* Initial BR ctrl settings */
#define LR_INITIAL_CTRL_LDS_ENABLE (1 << 0) /* Enable LDS at init(), valid only in BR in enabled */
#define LR_INITIAL_CTRL_LR_MS (1 << 1) /* Master Selection */
#define LR_INITIAL_CTRL_LR_UE (1 << 2) /* Unidirectional enable */
/* Initial BR pairs, valid only if LDS is disabled, but BR is enabled (3 bits) */
#define LR_INITIAL_CTRL_LR_PS_MASK (0x7 << 3)
#define LR_INITIAL_CTRL_LR_PS_1P (0x0 << 3) /* 1 Pair */
#define LR_INITIAL_CTRL_LR_PS_2P (0x1 << 3) /* 2 Pairs */
#define LR_INITIAL_CTRL_LR_PS_4P (0x2 << 3) /* 4 Pairs */
/* Initial BR speed, valid only if LDS is disabled, but BR is enabled (6 bits) */ 
#define LR_INITIAL_CTRL_LR_SS_MASK (0x3f << 6)
#define LR_INITIAL_CTRL_LR_SS_10 (0x0 << 6) /* 10Mbps */
#define LR_INITIAL_CTRL_LR_SS_20 (0x1 << 6) /* 20Mbps */
#define LR_INITIAL_CTRL_LR_SS_25 (0x2 << 6) /* 25Mbps */
#define LR_INITIAL_CTRL_LR_SS_33 (0x3 << 6) /* 33Mbps */
#define LR_INITIAL_CTRL_LR_SS_50 (0x4 << 6) /* 50Mbps */
#define LR_INITIAL_CTRL_LR_SS_100 (0x5 << 6) /* 100Mbps */

/* Initial advert ability, valid only if LDS is enabled */
#define LR_INITIAL_ADVA_1_10 (1 << 0) /* Advertise 1 pair 10Mbps */
#define LR_INITIAL_ADVA_2_10 (1 << 1) /* Advertise 2 pair 10Mbps */
#define LR_INITIAL_ADVA_1_20 (1 << 2) /* Advertise 1 pair 20Mbps */
#define LR_INITIAL_ADVA_2_20 (1 << 3) /* Advertise 2 pair 20Mbps */
#define LR_INITIAL_ADVA_1_25 (1 << 4) /* Advertise 1 pair 25Mbps */
#define LR_INITIAL_ADVA_2_25 (1 << 5) /* Advertise 2 pair 25Mbps */
#define LR_INITIAL_ADVA_1_33 (1 << 6) /* Advertise 1 pair 33Mbps */
#define LR_INITIAL_ADVA_2_33 (1 << 7) /* Advertise 2 pair 33Mbps */
#define LR_INITIAL_ADVA_1_50 (1 << 8) /* Advertise 1 pair 50Mbps */
#define LR_INITIAL_ADVA_2_50 (1 << 9) /* Advertise 2 pair 50Mbps */
#define LR_INITIAL_ADVA_1_100 (1 << 10) /* Advertise 1 pair 100Mbps */
#define LR_INITIAL_ADVA_2_100 (1 << 11) /* Advertise 2 pair 100Mbps */
#define LR_INITIAL_ADVA_4_100 (1 << 12) /* Advertise 4 pair 100Mbps */
#define LR_INITIAL_ADVA_PAUSE_RX (1 << 13) /* Advertise Pause RX */
#define LR_INITIAL_ADVA_PAUSE_TX (1 << 14) /* Advertise Pause TX */ 


/*
 * Enumeration of known PHYs
 */
typedef enum soc_known_phy_e {
    _phy_id_unknown = 0,
    _phy_id_BCM5218,
    _phy_id_BCM5220,
    _phy_id_BCM5226,
    _phy_id_BCM5228,
    _phy_id_BCM5238,
    _phy_id_BCM5248,
    _phy_id_BCM5400,
    _phy_id_BCM5401,
    _phy_id_BCM5402,
    _phy_id_BCM5404,
    _phy_id_BCM5411,
    _phy_id_BCM5421,
    _phy_id_BCM5424,
    _phy_id_BCM5461,
    _phy_id_BCM5464,
    _phy_id_BCM5466,
    _phy_id_BCM5478,
    _phy_id_BCM5488,
    _phy_id_BCM5482,
    _phy_id_BCM5481,
    _phy_id_BCM54680,
    _phy_id_BCM54880,
    _phy_id_BCM54680E,
    _phy_id_BCM54880E,
    _phy_id_BCM54881,
    _phy_id_BCM54810,
    _phy_id_BCM54811,
    _phy_id_BCM54682,
    _phy_id_BCM54684,
    _phy_id_BCM54684E,
    _phy_id_BCM54685,
    _phy_id_BCM52681E,
    _phy_id_BCM54640,
    _phy_id_BCM54616,
    _phy_id_BCM54618E,
    _phy_id_BCM54618SE,
    _phy_id_BCM84707, /* start BCM84728 family */
    _phy_id_BCM84073,
    _phy_id_BCM84074,
    _phy_id_BCM84728,
    _phy_id_BCM84729 = _phy_id_BCM84728,
    _phy_id_BCM84748,
    _phy_id_BCM84727,
    _phy_id_BCM84747,
    _phy_id_BCM84762,
    _phy_id_BCM84764,
    _phy_id_BCM84042,
    _phy_id_BCM84044, /* end  BCM84728 family */
    _phy_id_BCM54584,
    _phy_id_BCM54580,
    _phy_id_BCM54540,
    _phy_id_BCM54585,
    _phy_id_BCM8729,
    _phy_id_BCM84756,
    _phy_id_BCM84757,
    _phy_id_BCM84759,
    _phy_id_BCM84749,
    _phy_id_BCM54980,
    _phy_id_BCM54980C,
    _phy_id_BCM54980V,
    _phy_id_BCM54980VC,
    _phy_id_BCM8011,
    _phy_id_BCM8703,
    _phy_id_BCM8704,
    _phy_id_BCM8705,
    _phy_id_BCM8706,
    _phy_id_BCM8072 = _phy_id_BCM8706,
    _phy_id_BCMXGXS1,
    _phy_id_BCMXGXS2,
    _phy_id_BCMXGXS5,
    _phy_id_BCMXGXS6,
    _phy_id_BCM53314,
    _phy_id_BCM53324,
    _phy_id_SERDES,
    _phy_id_SERDES100FX,
    _phy_id_SERDES65LP,
    _phy_id_XGXS_HL65    = _phy_id_SERDES65LP,
    _phy_id_XGXS_HC65    = _phy_id_SERDES65LP,
    _phy_id_XGXS_16G     = _phy_id_SERDES65LP,
    _phy_id_XGXS_WC40    = _phy_id_SERDES65LP,
    _phy_id_XGXS_WL      = _phy_id_SERDES65LP,
    _phy_id_XGXS_QSGMIIE = _phy_id_SERDES65LP,
    _phy_id_XGXS_TSC,
    _phy_id_XGXS_VIPER,
    _phy_id_SERDESCOMBO,
    _phy_id_SERDESCOMBO65,
    _phy_id_BCM8727,
    _phy_id_BCM8073 = _phy_id_BCM8727,
    _phy_id_BCM8074 = _phy_id_BCM8727,
    _phy_id_BCM8040,
    _phy_id_BCM8481x,
    _phy_id_BCM84812ce,
    _phy_id_BCM84821,
    _phy_id_BCM84822,
    _phy_id_BCM84823,
    _phy_id_BCM84833,
    _phy_id_BCM84333 = _phy_id_BCM84833,
    _phy_id_BCM84834,
    _phy_id_BCM84334 = _phy_id_BCM84834,
    _phy_id_BCM84835,
    _phy_id_BCM84836,
    _phy_id_BCM84336 = _phy_id_BCM84836,
    _phy_id_BCM84844,
    _phy_id_BCM84846,
    _phy_id_BCM84848,
    _phy_id_BCM84858,
    _phy_id_BCM84856,
    _phy_id_BCM84860,
    _phy_id_BCM84861,
    _phy_id_BCM84864,
    _phy_id_BCM84868,
    _phy_id_BCM84888,
    _phy_id_BCM84884,
    _phy_id_BCM84888E,
    _phy_id_BCM84884E,
    _phy_id_BCM84881,
    _phy_id_BCM84880,
    _phy_id_BCM84888S,
    _phy_id_BCM84885,
    _phy_id_BCM84886,
    _phy_id_BCM84887,
    _phy_id_BCM8747,
    _phy_id_BCM8750,
    _phy_id_BCM8752,
    _phy_id_BCM8754,
    _phy_id_BCM84740,
    _phy_id_BCM84780,
    _phy_id_BCM84784,
    _phy_id_BCM84758,
    _phy_id_BCM84164,
    _phy_id_BCM84168,
    _phy_id_BCM54380,
    _phy_id_BCM54382,
    _phy_id_BCM54340,
    _phy_id_BCM54385,
    _phy_id_BCM54210,
    _phy_id_BCM54220,
    _phy_id_BCM54280,
    _phy_id_BCM54282,
    _phy_id_BCM54240,
    _phy_id_BCM54285,
    _phy_id_BCM84328,
    _phy_id_BCM84793,
    _phy_id_BCM54290,
    _phy_id_BCM54182,
    _phy_id_BCM54185,
    _phy_id_BCM54180,
    _phy_id_BCM54140,
    _phy_id_BCM54192,
    _phy_id_BCM54195,
    _phy_id_BCM54190,
    _phy_id_BCM54194,
    _phy_id_BCM54292,
    _phy_id_BCM54294,
    _phy_id_BCM54295,
    _phy_id_BCM54296,
    _phy_id_BCM5428x,
    _phy_id_BCM82328,
    _phy_id_BCM84318,
    _phy_id_BCM82381,
    _phy_id_BCM82764,
    _phy_id_BCM82780,
    _phy_id_BCM56160_GPHY,
    _phy_id_BCM53540,
    _phy_id_BCM56275_GPHY,
    _phy_id_BCM8806x,
    _phy_id_BCM82864,
    _phy_id_NULL,
    _phy_id_SIMUL,
    _phy_id_BCM82109,
    _phy_id_numberKnown                 /* Always last, please */
} soc_known_phy_t;

/*
   uc description
*/

typedef struct soc_phy_wcmod_uc_desc_s{
    int pll_range;
    int tx_pre_cursor;
    int tx_main;
    int tx_post_cursor;
    char *vga_bias_reduced;
    int postc_metric;
    char pmd_mode_s[100];
    int pf_ctrl;
    int vga_sum;
    int dfe1_bin;
    int dfe2_bin;
    int dfe3_bin;
    int dfe4_bin;
    int dfe5_bin;
    int integ_reg;
    int integ_reg_xfer;
    int clk90_offset;
    int clkp1_offset;
    int sd;
    int lopf;
    int p1_lvl;
    int m1_lvl;
    int tx_drvr;
    int slicer_target;
    int offset_pe;
    int offset_ze;
    int offset_me;
    int offset_po;
    int offset_zo;
    int offset_mo;
    int dsc_sm;
    int temp;
    int phy_ad;
    int lane;
} soc_phy_wcmod_uc_desc_t;

/* Forward def */
typedef struct soc_phy_table_s soc_phy_table_t;
typedef int (*soc_phy_ident_f)(int unit, soc_port_t port,
                               soc_phy_table_t *my_entry,
                               uint16 phy_id0, uint16 phy_id1,
                               soc_phy_info_t *pi);

/* For identifying phy */
struct soc_phy_table_s {
    soc_phy_ident_f     checkphy;       /* call back to check for this phy */
    soc_known_phy_t     myNum;          /* For known phys, its enum val */
    char                *phy_name;      /* Phy Name */
    phy_driver_t        *driver;        /* the driver for this phy */
    void                *cookie;        /* cookie for callback.  */
};

extern int
soc_phy_add_entry(soc_phy_table_t *entry);

#endif  /* !_SOC_PHY_H */
