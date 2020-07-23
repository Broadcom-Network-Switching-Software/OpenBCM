/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy5464.h
 *
 * This driver applies to both BCM5464 and BCM5464S.
 * Both parts support driving Copper or SERDES interfaces.
 *
 * The BCM5464S also has a SERDES pass-through mode, making it possible
 * to auto-select copper or fiber based on signal detects.
 */

#ifndef   _PHY5464_H_
#define   _PHY5464_H_

#include <soc/phy.h>

/* PHY register access */
#define PHY5464_REG_READ(_unit, _phy_ctrl, _flags, _reg_bank, _reg_addr, _val) \
            phy_reg_ge_read((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                            (_reg_addr), (_val))
#define PHY5464_REG_WRITE(_unit, _phy_ctrl, _flags, \
                          _reg_bank, _reg_addr, _val) \
            phy_reg_ge_write((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                             (_reg_addr), (_val))
#define PHY5464_REG_MODIFY(_unit, _phy_ctrl, _flags, _reg_bank, \
                           _reg_addr, _val, _mask) \
            phy_reg_ge_modify((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                              (_reg_addr), (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T MII Control Register (Addr 00h) */
#define READ_PHY5464_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val))
#define WRITE_PHY5464_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val)) 
#define MODIFY_PHY5464_MII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, \
                               (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T MII Status Register (ADDR 01h) */
#define READ_PHY5464_MII_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val)) 
#define WRITE_PHY5464_MII_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val)) 
#define MODIFY_PHY5464_MII_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, \
                               (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 02h) */
#define READ_PHY5464_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define WRITE_PHY5464_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define MODIFY_PHY5464_MII_PHY_ID0r(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 03h) */
#define READ_PHY5464_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define WRITE_PHY5464_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define MODIFY_PHY5464_MII_PHY_ID1r(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Advertisment Register (ADDR 04h) */
#define READ_PHY5464_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define WRITE_PHY5464_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define MODIFY_PHY5464_MII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Link Partner Ability (ADDR 05h) */
#define READ_PHY5464_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val))
#define WRITE_PHY5464_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val)) 
#define MODIFY_PHY5464_MII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Expansion Register (ADDR 06h) */
#define READ_PHY5464_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define WRITE_PHY5464_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define MODIFY_PHY5464_MII_AN_EXPr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Next Page Transmit Register (ADDR 07h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Link Partner Received Next Page (ADDR 08h) */

/* 1000BASE-T Control Register  (ADDR 09h) */
#define READ_PHY5464_MII_GB_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define WRITE_PHY5464_MII_GB_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define MODIFY_PHY5464_MII_GB_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, \
                             (_val), (_mask))

/* 1000BASE-T Status Register (ADDR 0ah) */
#define READ_PHY5464_MII_GB_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val)) 
#define WRITE_PHY5464_MII_GB_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val))
#define MODIFY_PHY5464_MII_GB_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, \
                             (_val), (_mask))

/* (Addr 0ch-0eh) Reserved (Do not read/write to reserved registers. */

/* 1000BASE-T/100BASE-TX/10BASE-T IEEE Extended Status Register (ADDR 0fh) */

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Extended Control Register (ADDR 10h) */
#define READ_PHY5464_MII_ECRr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, (_val)) 
#define WRITE_PHY5464_MII_ECRr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, (_val))
#define MODIFY_PHY5464_MII_ECRr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Extended Status Register (ADDR 11h) */
#define READ_PHY5464_MII_ESRr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, (_val)) 
#define WRITE_PHY5464_MII_ESRr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, (_val))
#define MODIFY_PHY5464_MII_ESRr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Receive Error Counter Register (ADDR 12h) */
/* 1000BASE-T/100BASE-TX/10BASE-T False Carrier Sense Counter (ADDR 13h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Receive NOT_OK Counter Register (ADDR 14h) */

/* 1000BASE-T/100BASE-TX/10BASE-T Auxiliary Control Reg (ADDR 18h Shadow 000)*/
#define READ_PHY5464_MII_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, (_val))
#define WRITE_PHY5464_MII_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, (_val))
#define MODIFY_PHY5464_MII_AUX_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, \
                             (_val), (_mask))

/* 10BASE-T Register (ADDR 18h Shadow 001) */
#define READ_PHY5464_MII_10BASE_Tr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, (_val))
#define WRITE_PHY5464_MII_10BASE_Tr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, (_val))
#define MODIFY_PHY5464_MII_10BASE_Tr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Power/MII Control Reg (ADDR 18h Shadow 010)*/
#define READ_PHY5464_MII_POWER_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, (_val))
#define WRITE_PHY5464_MII_POWER_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, (_val))
#define MODIFY_PHY5464_MII_POWER_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Misc Test Register (ADDR 18h Shadow 100) */
#define READ_PHY5464_MII_MISC_TESTr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, (_val))
#define WRITE_PHY5464_MII_MISC_TESTr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, (_val))
#define MODIFY_PHY5464_MII_MISC_TESTr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Misc Control Register (ADDR 18h Shadow 111)*/
#define READ_PHY5464_MII_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, (_val))
#define WRITE_PHY5464_MII_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, (_val))
#define MODIFY_PHY5464_MII_MISC_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auxiliary Status Register (ADDR 19h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Interrupt Status Register (ADDR 1ah) */
/* 1000BASE-T/100BASE-TX/10BASE-T Interrupt Control Register (ADDR 1bh) */

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl Reg (ADDR 1ch shadow 00010) */
#define READ_PHY5464_SPARE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, (_val))
#define WRITE_PHY5464_SPARE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, (_val))
#define MODIFY_PHY5464_SPARE_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Clk Alignment Ctrl (ADDR 1ch shadow 00011) */
#define READ_PHY5464_CLK_ALIGN_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0003, 0x1c, (_val))
#define WRITE_PHY5464_CLK_ALIGN_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0003, 0x1c, (_val))
#define MODIFY_PHY5464_CLK_ALIGN_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0003, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl 2 Reg (ADDR 1ch shadow 00100) */
#define READ_PHY5464_SPARE_CTRL_2r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, (_val))
#define WRITE_PHY5464_SPARE_CTRL_2r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, (_val))
#define MODIFY_PHY5464_SPARE_CTRL_2r(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl 3 Reg (ADDR 1ch shadow 00101) */
#define READ_PHY5464_SPARE_CTRL_3r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, (_val))
#define WRITE_PHY5464_SPARE_CTRL_3r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, (_val))
#define MODIFY_PHY5464_SPARE_CTRL_3r(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED Status Reg (ADDR 1ch shadow 01000) */
#define READ_PHY5464_LED_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, (_val))
#define WRITE_PHY5464_LED_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, (_val))
#define MODIFY_PHY5464_LED_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED Ctrl Reg (ADDR 1ch shadow 01001) */
#define READ_PHY5464_LED_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, (_val))
#define WRITE_PHY5464_LED_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, (_val))
#define MODIFY_PHY5464_LED_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, \
                           (_val), (_mask))

/* Auto Power-Down Reg (ADDR 1ch shadow 01010) */
#define READ_PHY5464_AUTO_POWER_DOWNr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000a, 0x1c, (_val))
#define WRITE_PHY5464_AUTO_POWER_DOWNr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000a, 0x1c, (_val))
#define MODIFY_PHY5464_AUTO_POWER_DOWNr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000a, 0x1c, \
                           (_val), (_mask))

#define PHY_5464_AUTO_PWRDWN_EN              (1 << 5) 
#define PHY_5464_AUTO_PWRDWN_WAKEUP_MASK     (0xF << 0) 
#define PHY_5464_AUTO_PWRDWN_WAKEUP_UNIT     84  /* ms */ 
#define PHY_5464_AUTO_PWRDWN_WAKEUP_MAX      1260  /* ms */ 
#define PHY_5464_AUTO_PWRDWN_SLEEP_MASK      (1 << 4)
#define PHY_5464_AUTO_PWRDWN_SLEEP_MAX       5400  /* ms */ 

/* 1000BASE-T/100BASE-TX/10BASE-T LED Selector 1 Reg (ADDR 1ch shadow 01101) */
#define READ_PHY5464_LED_SELECTOR_1r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000d, 0x1c, (_val))
#define WRITE_PHY5464_LED_SELECTOR_1r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000d, 0x1c, (_val))
#define MODIFY_PHY5464_LED_SELECTOR_1r(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000d, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED Selector 2 Reg (ADDR 1ch shadow 01110) */
#define READ_PHY5464_LED_SELECTOR_2r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000e, 0x1c, (_val))
#define WRITE_PHY5464_LED_SELECTOR_2r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000e, 0x1c, (_val))
#define MODIFY_PHY5464_LED_SELECTOR_2r(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000e, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED GPIO Ctrl/Stat (ADDR 1ch shadow 01111) */
#define READ_PHY5464_LED_GPIO_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000F, 0x1c, (_val))
#define WRITE_PHY5464_LED_GPIO_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000F, 0x1c, (_val))
#define MODIFY_PHY5464_LED_GPIO_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000F, 0x1c, \
                           (_val), (_mask))

/* Misc 1000BASE-X Control2 Reg (ADDR 1ch shadow 10110), for 54980 */
#define READ_PHY5464_MISC_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0016, 0x1c, (_val))
#define WRITE_PHY5464_MISC_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0016, 0x1c, (_val))
#define MODIFY_PHY5464_MISC_1000X_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0016, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-X Misc 1000BASE-X Control Reg (ADDR 1ch shadow 10111) */
#define READ_PHY5464_MISC_1000X_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0017, 0x1c, (_val))
#define WRITE_PHY5464_MISC_1000X_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0017, 0x1c, (_val))
#define MODIFY_PHY5464_MISC_1000Xr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0017, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-X Auto-Detect SGMII/Media Converter Reg (ADDR 1ch shadow 11000) */
#define READ_PHY5464_AUTO_DETECT_SGMII_MEDIAr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0018, 0x1c, (_val))
#define WRITE_PHY5464_AUTO_DETECT_SGMII_MEDIAr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0018, 0x1c, (_val))
#define MODIFY_PHY5464_AUTO_DETECT_SGMII_MEDIAr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0018, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-X Auto-neg Debug Reg (ADDR 1ch shadow 11010) */
#define READ_PHY5464_1000X_AN_DEBUGr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001a, 0x1c, (_val))
#define WRITE_PHY5464_1000X_AN_DEBUGr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001a, 0x1c, (_val))
#define MODIFY_PHY5464_1000X_AN_DEBUGr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001a, 0x1c, \
                           (_val), (_mask))

/* Auxiliary 1000BASE-X Control Reg (ADDR 1ch shadow 11011) */
#define READ_PHY5464_AUX_1000X_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001B, 0x1c, (_val))
#define WRITE_PHY5464_AUX_1000X_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001B, 0x1c, (_val))
#define MODIFY_PHY5464_AUX_1000X_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001B, 0x1c, \
                           (_val), (_mask))

/* Auxiliary 1000BASE-X Status Reg (ADDR 1ch shadow 11100) */
#define READ_PHY5464_AUX_1000X_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001C, 0x1c, (_val))
#define WRITE_PHY5464_AUX_1000X_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001C, 0x1c, (_val))
#define MODIFY_PHY5464_AUX_1000X_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001C, 0x1c, \
                           (_val), (_mask))

/* Misc 1000BASE-X Status Reg (ADDR 1ch shadow 11101) */
#define READ_PHY5464_MISC_1000X_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001D, 0x1c, (_val))
#define WRITE_PHY5464_MISC_1000X_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001D, 0x1c, (_val))
#define MODIFY_PHY5464_MISC_1000X_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001D, 0x1c, \
                           (_val), (_mask))

/* Copper/Fiber Auto-Detect Medium Reg (ADDR 1ch shadow 11110) */
#define READ_PHY5464_AUTO_DETECT_MEDIUMr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001E, 0x1c, (_val))
#define WRITE_PHY5464_AUTO_DETECT_MEDIUMr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001E, 0x1c, (_val))
#define MODIFY_PHY5464_AUTO_DETECT_MEDIUMr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001E, 0x1c, \
                           (_val), (_mask))

/* Mode Control Reg (ADDR 1ch shadow 11111) */
#define READ_PHY5464_MODE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001F, 0x1c, (_val))
#define WRITE_PHY5464_MODE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001F, 0x1c, (_val))
#define MODIFY_PHY5464_MODE_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001F, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Master/Slave Seed Reg (ADDR 1dh bit 15 = 0) */
/* 1000BASE-T/100BASE-TX/10BASE-T HDC Status Reg (ADDR 1dh bit 15 = 1) */

/* 1000BASE-T/100BASE-TX/10BASE-T Test Register 1 (ADDR 1eh) */
#define READ_PHY5464_TEST1r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, (_val))
#define WRITE_PHY5464_TEST1r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, (_val))
#define MODIFY_PHY5464_TEST1r(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, \
                           (_val), (_mask))


/*          +------------------------------+
 *          |                              |
 *          |   Primary SerDes Registers   |
 *          |                              |
 *          +------------------------------+
 */
/* 1000BASE-X MII Control Register (Addr 00h) */
#define READ_PHY5464_1000X_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x00, (_val))
#define WRITE_PHY5464_1000X_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                              0x0000, 0x00, (_val))
#define MODIFY_PHY5464_1000X_MII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                               0x0000, 0x00, (_val), (_mask))

/* 1000BASE-X MII Status Register (Addr 01h) */
#define READ_PHY5464_1000X_MII_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x01, (_val))
#define WRITE_PHY5464_1000X_MII_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                              0x0000, 0x01, (_val))
#define MODIFY_PHY5464_1000X_MII_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                              0x0000, 0x01, (_val), (_mask))

/* 1000BASE-X MII Auto-neg Advertise Register (Addr 04h) */
#define READ_PHY5464_1000X_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x04, (_val))
#define WRITE_PHY5464_1000X_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x04, (_val))
#define MODIFY_PHY5464_1000X_MII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x04, (_val), (_mask))

/* 1000BASE-X MII Auto-neg Link Partner Ability Register (Addr 05h) */
#define READ_PHY5464_1000X_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x05, (_val))
#define WRITE_PHY5464_1000X_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x05, (_val))
#define MODIFY_PHY5464_1000X_MII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x05, (_val), (_mask))

/* 1000BASE-X MII Auto-neg Extended Status Register (Addr 06h) */
#define READ_PHY5464_1000X_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                            0x0000, 0x06, (_val))
#define WRITE_PHY5464_1000X_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                            0x0000, 0x06, (_val))
#define MODIFY_PHY5464_1000X_MII_AN_EXPr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                            0x0000, 0x06, (_val), (_mask))

/* 1000BASE-X MII IEEE Extended Status Register (Addr 0fh) */
#define READ_PHY5464_1000X_MII_EXT_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                            0x0000, 0x0f, (_val))
#define WRITE_PHY5464_1000X_MII_EXT_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                            0x0000, 0x0f, (_val))
#define MODIFY_PHY5464_1000X_MII_EXT_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                            0x0000, 0x0f, (_val), (_mask))

/*          +-------------------------+
 *          |                         |
 *          |   Expansion Registers   |
 *          |                         |
 *          +-------------------------+
 */
/* Receive/Transmit Packet Counter Register (Addr 00h) */
#define READ_PHY5464_EXP_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F00, 0x15, (_val))
#define WRITE_PHY5464_EXP_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F00, \
                               0x15, (_val))
#define MODIFY_PHY5464_EXP_PKT_COUNTERr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F00, \
                                0x15, (_val), (_mask))

/* Expansion Interrupt Status Register (Addr 01h) */
#define READ_PHY5464_EXP_INTERRUPT_STATr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F01, 0x15, (_val))
#define WRITE_PHY5464_EXP_r(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F01, \
                               0x15, (_val))
#define MODIFY_PHY5464_EXP_r(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F01, \
                                0x15, (_val), (_mask))

/* Expansion Interrupt Mask Register (Addr 02h) */
#define READ_PHY5464_EXP_INTERRUPT_MASKr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F02, 0x15, (_val))
#define WRITE_PHY5464_EXP_INTERRUPT_MASKr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F02, \
                               0x15, (_val))
#define MODIFY_PHY5464_EXP_INTERRUPT_MASKr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F02, \
                                0x15, (_val), (_mask))

/* Expansion SerDes Pass-Through Enable (Addr 03h) */
#define READ_PHY5464_EXP_SERDES_PASSTHRU_ENr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F03, 0x15, (_val))
#define WRITE_PHY5464_EXP_SERDES_PASSTHRU_ENr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F03, \
                               0x15, (_val))
#define MODIFY_PHY5464_EXP_SERDES_PASSTHRU_ENr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F03, \
                                0x15, (_val), (_mask))

/* Multicolor LED Selector Register (Addr 04h) */
#define READ_PHY5464_EXP_LED_SELECTORr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F04, 0x15, (_val))
#define WRITE_PHY5464_EXP_LED_SELECTORr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F04, \
                               0x15, (_val))
#define MODIFY_PHY5464_EXP_LED_SELECTORr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F04, \
                                0x15, (_val), (_mask))

/* Multicolor LED Flash Rate Controls Register (Addr 05h) */
#define READ_PHY5464_EXP_LED_FLASH_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F05, 0x15, (_val))
#define WRITE_PHY5464_EXP_LED_FLASH_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F05, \
                               0x15, (_val))
#define MODIFY_PHY5464_EXP_LED_FLASH_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F05, \
                                0x15, (_val), (_mask))

/* Multicolor LED Programmable Blink Controls Register (Addr 06h) */
#define READ_PHY5464_EXP_LED_BLINK_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F06, 0x15, (_val))
#define WRITE_PHY5464_EXP_LED_BLINK_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F06, \
                               0x15, (_val))
#define MODIFY_PHY5464_EXP_LED_BLINK_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F06, \
                                0x15, (_val), (_mask))

/* 10BT Controls Register (Addr 08h) */
#define READ_PHY5464_10BT_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F08, 0x15, (_val))
#define WRITE_PHY5464_10BT_CTRLr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F08, 0x15, (_val))
#define MODIFY_PHY5464_10BT_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F08, 0x15, \
                                (_val), (_mask))

/* Register 76h,related to the 10BASE-T LOW TRANSMITTER AMPLITUDE workaround*/
#define READ_PHY5464_EXP_10BaseT_TxAMPr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F76, 0x15, (_val))
#define WRITE_PHY5464_EXP_10BaseT_TxAMPr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F76, \
                               0x15, (_val))
#define MODIFY_PHY5464_EXP_10BaseT_TxAMPr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F76, \
                                0x15, (_val), (_mask))

/* SerDes Passthrough LED Mode Reg (Addr fdh) */
#define READ_PHY5464_EXP_PASSTHRU_LED_MODEr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0FF9, 0x15, (_val))
#define WRITE_PHY5464_EXP_PASSTHRU_LED_MODEr(_unit, _phy_ctrl, _val) \
            PHY5464_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0FF9, \
                               0x15, (_val))
#define MODIFY_PHY5464_EXP_PASSTHRU_LED_MODEr(_unit, _phy_ctrl, _val, _mask) \
            PHY5464_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0FF9, \
                                0x15, (_val), (_mask))

#endif /* _PHY5464_H_ */
