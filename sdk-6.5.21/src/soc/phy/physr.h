/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        physr.h
 * Purpose:     Basic defines for PHY specific data that gets pointed to 
 *              by phy_ctrl. The data gets allocated and freed outside the
 *              driver.
 * Note:        This header file is for PHY driver module. Should not include
 *              this header file in SOC or BCM layers.
 */

#ifndef _PHY_SR_H
#define _PHY_SR_H

/*
 * phy56xxx_5601x private data holds shadow registers
 */
typedef struct serdes_5601x_sregs {
    uint16      mii_ctrl;               /* 0*0  */
    uint16      mii_ana;                /* 0*4  */
    uint16      r1000x_ctrl1;           /* 0*10 */
    uint16      r1000x_ctrl2;           /* 0*11 */
    uint16      analog_tx;              /* 2*1b */
    uint16      digi3_ctrl;             /* 2*10 */
 } serdes_5601x_sregs_t;


/*
 * serdescombo_5601x private data holds shadow registers
 */
typedef struct serdescombo_5601x_sregs {
    uint16      mii_ctrl;               /* 0*0  */
    uint16      mii_ana;                /* 0*4  */
    uint16      r1000x_ctrl1;           /* 0*10 */
    uint16      r1000x_ctrl2;           /* 0*11 */
    uint16      analog_tx;              /* 2*1b */
    uint16      digi3_ctrl;             /* 2*10 */
    uint16      misc_misc2;             /* 5*1e */
    uint16      misc_tx_actrl3;         /* 5*17 */
} serdescombo_5601x_sregs_t;

/*
 * serdescombo65_5602x private data
 */
typedef struct serdescombo65_5602x_sregs {
    uint16      ieee0_mii_ctrl;         /* 0000ffe0 */
    uint16      ieee0_mii_ana;          /* 0000ffe4 */
    uint16      xgxs_blk0_ctrl;         /* 00008000 */
    uint16      tx_all_driver;          /* 000080a7 */
    uint16      over_1g_up1;            /* 00008329 */
    uint16      serdes_digital_ctrl1;   /* 00008300 */
    uint16      serdes_digital_misc1;   /* 00008308 */
    uint16      cl73_ieeeb0_an_ctrl;    /* 38000000 */
    uint16      cl73_ieeeb0_an_adv1;    /* 38000010 */
    uint16      cl73_ieeeb0_an_adv2;    /* 38000011 */
    uint16      cl73_ieeeb0_bamctrl1;   /* 00008372 */
    uint16      cl73_ieeeb0_bamctrl3;   /* 00008374 */
} serdescombo65_5602x_sregs_t;


#endif /* _PHY_SR_H */
