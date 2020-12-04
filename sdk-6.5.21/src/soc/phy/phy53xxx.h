/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy53xxx.h
 * Purpose:    Fiber driver for 53xxx using internal QuadSerdes PHY.
 */

#ifndef _PHY_53XXX_H
#define _PHY_53XXX_H
/* QS: MII Control bit definitions, same as Standard 10/100 PHY */
#define QS_MII_CTRL_RESET      MII_CTRL_RESET  /* Self clearing SW reset */
#define QS_MII_CTRL_SS_LSB     MII_CTRL_SS_LSB /* Speed Select, LSB */
#define QS_MII_CTRL_SS_MSB     MII_CTRL_SS_MSB /* Speed Select, MSB */
#define QS_MII_CTRL_SS(_x)     MII_CTRL_SS(_x)
#define QS_MII_CTRL_SS_10      MII_CTRL_SS_10
#define QS_MII_CTRL_SS_100     MII_CTRL_SS_100
#define QS_MII_CTRL_SS_1000    MII_CTRL_SS_1000
#define QS_MII_CTRL_SS_INVALID MII_CTRL_SS_INVALID
#define QS_MII_CTRL_SS_MASK    MII_CTRL_SS_MASK
#define QS_MII_CTRL_FD         MII_CTRL_FD /* Full Duplex */
#define QS_MII_CTRL_AN_RESTART (1 << 9)    /* Auto Negotiation Enable */
#define QS_MII_CTRL_AN_ENABLE  (1 << 12)   /* Auto Negotiation Enable */
#define QS_MII_CTRL_PD         MII_CTRL_PD      /* Power Down Enable */
#define QS_MII_CTRL_LOOPBACK   (1 << 14)   /* Loopback Enable */


/* Auto-Negotiation Link-Partner Ability Register */
#define QS_MII_ANP_SGMII            (1 << 0)
#define QS_MII_ANP_SGMII_DUPLEX     (1 << 12)
#define QS_MII_ANP_SGMII_SPEED      (3 << 10) 
#define QS_MII_ANP_SGMII_SPEED_10   (0) 
#define QS_MII_ANP_SGMII_SPEED_100  (1 << 10) 
#define QS_MII_ANP_SGMII_SPEED_1000 (1 << 11) 





/* 1000X Control #1 Register: Controls 10B/SGMII mode */
#define QS_1000X_FIBER_MODE    (1 << 0)     /* Enable SGMII fiber mode */
#define QS_1000X_EN10B_MODE    (1 << 1)     /* Enable TBI 10B interface */
#define QS_1000X_INVERT_SD     (1 << 3)     /* Invert Signal Detect */
#define QS_1000X_AUTO_DETECT   (1 << 4)     /* Auto-detect SGMII and 1000X */ 
#define QS_1000X_RX_PKTS_CNTR_SEL (1 << 11) /* Select receive counter for 17h*/
#define QS_1000X_TX_AMPLITUDE_OVRD    (1 << 12)

/* 1000X Control #2 Register Fields */
#define QS_1000X_PAR_DET_EN     (1 << 0)     /* Enable Parallel Detect */
#define QS_1000X_FALSE_LNK_DIS  (1 << 1)     /* Disable false link */
#define QS_1000X_FLT_FORCE_EN   (1 << 2)     /* Enable filter force link */
#define QS_1000X_CLRAR_BER_CNTR (1 << 14)    /* Clear bit-err-rate counter */

/* 1000X Control #3 Register Fields */
#define QS_1000X_TX_FIFO_RST           (1 << 0)    /* Reset TX FIFO */
#define QS_1000X_FIFO_ELASTICITY_MASK  (0x3 << 1)  /* Fifo Elasticity */
#define QS_1000X_FIFO_ELASTICITY_5K    (0x0 << 1)  /* 5 Kbytes */
#define QS_1000X_FIFO_ELASTICITY_10K   (0x1 << 1)  /* 10 Kbytes */
#define QS_1000X_FIFO_ELASTICITY_13_5K (0x2 << 1)  /* 13.5 Kbytes */
#define QS_1000X_RX_FIFO_RST           (1 << 14)   /* Reset RX FIFO */

/* 1000X Control #4 Register Fields */
#define QS_1000X_DIG_RESET             (1 << 6)    /* Reset Datapath */

/* 1000X Status #1 Register Fields */
#define QS_1000X_STATUS1_SGMII_MODE    (1 << 0)
#define QS_1000X_STATUS1_SPEED         (3 << 3)
#define QS_1000X_STATUS1_SPEED_10      (0)
#define QS_1000X_STATUS1_SPEED_100     (1 << 3)
#define QS_1000X_STATUS1_SPEED_1000    (1 << 4)


#endif /* _PHY_53XXX_H*/

