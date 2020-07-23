/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy5690.h
 * Purpose:     BCM5690 (Draco) Internal Quad Serdes PHY Driver
 */

#ifndef   _PHY_5690_H_
#define   _PHY_5690_H_

#include <soc/phy.h>

/*
 * Definitions for Draco Quad Serdes Internal 10/100/1000 PHY
 * From: Draco Serdes MII Register Specification: v2.0
 */

/* MII Control register */
#define QS_MII_CTRL_REG        MII_CTRL_REG /* 0x00 */
/* QS: MII Control bit definitions, same as Standard 10/100 PHY */
#define QS_MII_CTRL_RESET      MII_CTRL_RESET   /* Pulse 1us, for SW reset */
#define QS_MII_CTRL_SS_LSB     MII_CTRL_SS_LSB  /* Speed Select, LSB */
#define QS_MII_CTRL_SS_MSB     MII_CTRL_SS_MSB  /* Speed Select, MSB */
#define	QS_MII_CTRL_SS(_x)     MII_CTRL_SS(_x)
#define	QS_MII_CTRL_SS_10      MII_CTRL_SS_10
#define	QS_MII_CTRL_SS_100     MII_CTRL_SS_100
#define	QS_MII_CTRL_SS_1000    MII_CTRL_SS_1000
#define	QS_MII_CTRL_SS_INVALID MII_CTRL_SS_INVALID
#define	QS_MII_CTRL_SS_MASK    MII_CTRL_SS_MASK
#define QS_MII_CTRL_FD         MII_CTRL_FD     /* Full Duplex */

#define QS_MII_STAT_REG        MII_STAT_REG /* 0x01 */
/* QS: MII Status Register: not the same as standard 10/100 PHY */
#define QS_MII_STAT_RF         MII_STAT_RF     /* Remote fault */
#define QS_MII_STAT_LA         MII_STAT_LA     /* Link active */
#define QS_MII_FD_CAPABLE      (1 << 6)
#define QS_MII_HD_CAPABLE      (1 << 5)

/* Auto-negotiation advertisement register */
#define QS_ANA_REG		0x04

/* Bit defines for ANA register */
#define	QS_MII_ANA_FD		(1 << 6)
#define	QS_MII_ANA_HD		(1 << 5)

/* Auto-Negotiation Link-Partner Ability Register */
#define QS_ANP_REG		0x05

#define QS_MII_ANP_SGMII_LINK		(1 << 15)
#define QS_MII_ANP_SGMII_FD		(1 << 12)
#define QS_MII_ANP_SGMII_SPEED_SHFT   	10
#define QS_MII_ANP_SGMII_SPEED_MASK   	0x0c00

#define QS_MII_ANP_FIBER_NEXT_PG	(1 << 15)
#define QS_MII_ANP_FIBER_ACK   		(1 << 14)
#define QS_MII_ANP_FIBER_RF_SHFT  	12	/* Remote fault */
#define QS_MII_ANP_FIBER_RF_MASK   	0x3000
#define QS_MII_ANP_FIBER_PAUSE_ASYM 	(1 << 8)
#define QS_MII_ANP_FIBER_PAUSE_SYM  	(1 << 7)
#define QS_MII_ANP_FIBER_HD  		(1 << 6)
#define QS_MII_ANP_FIBER_FD  		(1 << 5)

/* Auto-Negotiation Expansion Register */
#define QS_ANA_EXPANSION_REG   MII_AN_EXP_REG
#define QS_ANA_EXPANSION_PR    (1 << 1) /* Page received */

/* SGMII Control register. Quad Serdes comes up with ANA enabled. */
#define QS_SGMII_CTRL_REG      0x0B
#define QS_SGMII_FORCE_DATA   (1 << 15) /* Force Data mode (see spec) */
#define QS_SGMII_RF_SHFT      13	/* Remote Fault */
#define QS_SGMII_RF_MASK      0x6000
#define QS_SGMII_PAUSE_ASYM   (1 << 12)
#define QS_SGMII_PAUSE_SYM    (1 << 11)
#define QS_SGMII_AN_SEL       (1 << 10) /* Select SGMII ANA, or IEEE c37 ANA */
#define QS_SGMII_RX_PRE_EXT   (1 << 9)	/* If 1, disable RX early preamble */
					/* extension for 10/100 mode. */
					/* This bit is not in 5690A0/A1). */
#define QS_SGMII_ERR_TIMER_EN (1 << 8)
#define QS_SGMII_REV_PHASE    (1 << 7)
#define QS_SGMII_EXT_CTRL     (1 << 6)
#define QS_SGMII_TX_PRE_EXT   (1 << 5)  /* If 1, enable TX early preamble */
					/* extension for all speeds (short */
					/* latency enable). */
#define QS_SGMII_CDET_DISABLE (1 << 4)  /* If 1, disable comma detector */
#define QS_SGMII_AN_TEST_MODE (1 << 3)
#define QS_SGMII_AN_DISABLE   (1 << 2)  /* If 1, disable ANA to data mode*/
#define QS_SGMII_REMOTE_LOOP  (1 << 1)  /* Enable remote loopback */
#define QS_SGMII_TBI_LOOP     (1 << 0)  /* Enable TBI loopback */

/* SGMII Status Register */
#define QS_SGMII_STAT_REG      0x0C
#define QS_SGMII_ANA_COMPLETE (1 << 1) 	/* Autonegotiation completed */
#define QS_SGMII_ANA_ERROR    (1 << 0) 	/* SGMII Autoneg error flag */

/* SGMII CRC Register: bits 0:15 contain SGMII CRC value */
#define QS_SGMII_CRC_REG       0x0D

/* SGMII Misc Control Register */
#define QS_SGMII_MISC_REG      0x0E
#define QS_SGMII_ANA_RESTART   (1 << 0)	/* Restart Autonegotiation */

/* SGMII Control #2 Register: Controls 10B/SGMII mode */
#define QS_SGMII_CTRL2_REG     0x0F
#define QS_SGMII_EN10B_MODE   (1 << 1) 	/* Enable TBI 10B interface */
#define QS_SGMII_FIBER_MODE   (1 << 0) 	/* Enable SGMII fiber mode */

/* Serdes TX Control register */
#define QS_SERDES_TX_CTRL_REG  0x10
#define QS_SERDES_TX_PD        (1 << 15) /* Power-down Serdes */
#define QS_SERDES_BIAS_REFH    (1 << 14) /* Bias generator RefH */
#define QS_SERDES_BIAS_REFL    (1 << 13) /* Bias generator RefL */
#define QS_SERDES_CPM_ENABLE   (1 << 12) /* Clock Phase Mode enable */
#define QS_SERDES_RXW_SEL      (1 << 11) /* Clock (rxwclk) edge select  */

/* Serdes RX Control register */
#define QS_SERDES_RX_CTRL_REG  0x11
#define QS_SERDES_RX_PD        (1 << 15) /* Power-down Serdes */
#define QS_SERDES_PE_ENA       (1 << 14) /* Pre-Emphasis enable */
#define QS_SERDES_TX_CRS_LB    (1 << 13) /* If 0, MAC_CRS = RX_CRS or TX_CRS */
					 /* If 1, MAC_CRS = RX_CRS only */
#define QS_SERDES_PL_CLK_EDGE  (1 << 12) /* Parallel load clock edge */

/* Phase Control and Status Registers */
#define QS_PHASE_CTRL_REG      0x12
#define QS_PHASE_STAT_REG      0x13

/* SGMII Misc control register */
#define QS_SGMII_MISC_CTRL_REG 0x14

#define QS_SGMII_IDDQ_MODE     (1 << 5)	  /* Put Serdes in IDDQ mode */
/* Configuration/Link detection in SGMII mode */
#define QS_SGMII_SIGNAL_DETECT (1 << 3) /* When in SGMII, pin tied to 1*/
#define QS_SGMII_SGMII_ENABLE  (1 << 2) /* If 1, enable SGMII mode */

/* When autoneg is enabled, MAC_MODE=0, When disabled, MAC_MODE=1 */
#define QS_SGMII_MAC_MODE      (1 << 1) /* Always inverse of MII_CTRL.an_ena */

/* External PHY link status */
#define QS_SGMII_LINK_STATUS   (1 << 0) /* Copper XCVR final status*/

/* Misc SGMII control definitions */
#define QS_SGMII_MISC_STAT_REG 0x15
#define QS_SGMII_MISC_PRLE     (1 << 2) /* Priority resolution error enable*/
#define QS_SGMII_PLL_LOCK      (1 << 1) /* PLL lock detect */

/* Common registers: Shares for all 4-ports on Serdes Module */
#define QS_PLL_CTRL_REG        0x16     
#define QS_HW_RESET            (1 << 7)  /* S/W Hard reset of Serdes core*/
#define QS_PLL_TEST_ENA        (1 << 2)  /* Enable PLL test */
#define QS_PLL_RESET           (1 << 1)  /* Reset PLL */
#define QS_PLL_PD              (1 << 0)  /* Powerdown PLL */

/* Other test and Control registers */
#define QS_TEST_MUX_CTRL_REG   0x17 /* Shared for all 4-ports */
#define QS_PRBS_TEST_CTRL_REG  0x18
#define QS_PRBS_TEST_STAT_REG  0x19
#define QS_BERT_IPG_REG        0x1A
#define QS_BERT_CTRL_REG       0x1B
#define QS_BERT_OVERFLOW_REG   0x1C
#define QS_BERT_MDIO_CTRL_REG  0x1D
#define QS_BERT_MDIO_DATA_REG  0x1E


/* To Use onboard MII Controller, Bit 7 selects internal MDIO */
#define QS_PHY_ADDR(phyid)    (0x80 | (phyid)) 

#endif /* _PHY_5690_H_ */
