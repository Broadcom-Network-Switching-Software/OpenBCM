/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy54680.h
 *
 * This driver applies to both BCM54680 and BCM54680S.
 * Both parts support driving Copper or SERDES interfaces.
 *
 * The BCM54680S also has a SERDES pass-through mode, making it possible
 * to auto-select copper or fiber based on signal detects.
 */

#ifndef   _PHY54680_H_
#define   _PHY54680_H_

#include <soc/phy.h>

/* PHY register access */
#define PHY54680_REG_READ(_unit, _phy_ctrl, _flags, _reg_bank, _reg_addr, _val) \
            phy_reg_ge_read((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                            (_reg_addr), (_val))
#define PHY54680_REG_WRITE(_unit, _phy_ctrl, _flags, \
                          _reg_bank, _reg_addr, _val) \
            phy_reg_ge_write((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                             (_reg_addr), (_val))
#define PHY54680_REG_MODIFY(_unit, _phy_ctrl, _flags, _reg_bank, \
                           _reg_addr, _val, _mask) \
            phy_reg_ge_modify((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                              (_reg_addr), (_val), (_mask))


/* Clause 45 Registers access using Clause 22 register access */
#define PHY54680E_CL45_REG_READ(_unit, _phy_ctrl, _flags, _dev_addr, _reg_addr, _val) \
            _phy_54680e_cl45_reg_read((_unit), (_phy_ctrl), (_flags), (_dev_addr), \
                            (_reg_addr), (_val))
#define PHY54680E_CL45_REG_WRITE(_unit, _phy_ctrl, _flags, \
                          _dev_addr, _reg_addr, _val) \
            _phy_54680e_cl45_reg_write((_unit), (_phy_ctrl), (_flags), (_dev_addr), \
                             (_reg_addr), (_val))
#define PHY54680E_CL45_REG_MODIFY(_unit, _phy_ctrl, _flags, _dev_addr, \
                           _reg_addr, _val, _mask) \
            _phy_54680e_cl45_reg_modify((_unit), (_phy_ctrl), (_flags), (_dev_addr), \
                              (_reg_addr), (_val), (_mask))


/* 1000BASE-T/100BASE-TX/10BASE-T MII Control Register (Addr 00h) */
#define READ_PHY54680_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val))
#define WRITE_PHY54680_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val)) 
#define MODIFY_PHY54680_MII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, \
                               (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T MII Status Register (ADDR 01h) */
#define READ_PHY54680_MII_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val)) 
#define WRITE_PHY54680_MII_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val)) 
#define MODIFY_PHY54680_MII_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, \
                               (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 02h) */
#define READ_PHY54680_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define WRITE_PHY54680_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define MODIFY_PHY54680_MII_PHY_ID0r(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 03h) */
#define READ_PHY54680_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define WRITE_PHY54680_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define MODIFY_PHY54680_MII_PHY_ID1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Advertisment Register (ADDR 04h) */
#define READ_PHY54680_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define WRITE_PHY54680_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define MODIFY_PHY54680_MII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Link Partner Ability (ADDR 05h) */
#define READ_PHY54680_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val))
#define WRITE_PHY54680_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val)) 
#define MODIFY_PHY54680_MII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Expansion Register (ADDR 06h) */
#define READ_PHY54680_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define WRITE_PHY54680_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define MODIFY_PHY54680_MII_AN_EXPr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Next Page Transmit Register (ADDR 07h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Link Partner Received Next Page (ADDR 08h) */

/* 1000BASE-T Control Register  (ADDR 09h) */
#define READ_PHY54680_MII_GB_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define WRITE_PHY54680_MII_GB_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define MODIFY_PHY54680_MII_GB_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, \
                             (_val), (_mask))

/* 1000BASE-T Status Register (ADDR 0ah) */
#define READ_PHY54680_MII_GB_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val)) 
#define WRITE_PHY54680_MII_GB_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val))
#define MODIFY_PHY54680_MII_GB_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, \
                             (_val), (_mask))

/* (Addr 0ch-0eh) Reserved (Do not read/write to reserved registers. */

/* 1000BASE-T/100BASE-TX/10BASE-T IEEE Extended Status Register (ADDR 0fh) */

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Extended Control Register (ADDR 10h) */
#define READ_PHY54680_MII_ECRr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, (_val)) 
#define WRITE_PHY54680_MII_ECRr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, (_val))
#define MODIFY_PHY54680_MII_ECRr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Extended Status Register (ADDR 11h) */
#define READ_PHY54680_MII_ESRr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, (_val)) 
#define WRITE_PHY54680_MII_ESRr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, (_val))
#define MODIFY_PHY54680_MII_ESRr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Receive Error Counter Register (ADDR 12h) */
/* 1000BASE-T/100BASE-TX/10BASE-T False Carrier Sense Counter (ADDR 13h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Receive NOT_OK Counter Register (ADDR 14h) */

/* 1000BASE-T/100BASE-TX/10BASE-T Auxiliary Control Reg (ADDR 18h Shadow 000)*/
#define READ_PHY54680_MII_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, (_val))
#define WRITE_PHY54680_MII_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, (_val))
#define MODIFY_PHY54680_MII_AUX_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, \
                             (_val), (_mask))

/* 10BASE-T Register (ADDR 18h Shadow 001) */
#define READ_PHY54680_MII_10BASE_Tr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, (_val))
#define WRITE_PHY54680_MII_10BASE_Tr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, (_val))
#define MODIFY_PHY54680_MII_10BASE_Tr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Power/MII Control Reg (ADDR 18h Shadow 010)*/
#define READ_PHY54680_MII_POWER_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, (_val))
#define WRITE_PHY54680_MII_POWER_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, (_val))
#define MODIFY_PHY54680_MII_POWER_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Misc Test Register (ADDR 18h Shadow 100) */
#define READ_PHY54680_MII_MISC_TESTr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, (_val))
#define WRITE_PHY54680_MII_MISC_TESTr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, (_val))
#define MODIFY_PHY54680_MII_MISC_TESTr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Misc Control Register (ADDR 18h Shadow 111)*/
#define READ_PHY54680_MII_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, (_val))
#define WRITE_PHY54680_MII_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, (_val))
#define MODIFY_PHY54680_MII_MISC_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auxiliary Status Register (ADDR 19h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Interrupt Status Register (ADDR 1ah) */
/* 1000BASE-T/100BASE-TX/10BASE-T Interrupt Control Register (ADDR 1bh) */

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl Reg (ADDR 1ch shadow 00010) */
#define READ_PHY54680_SPARE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, (_val))
#define WRITE_PHY54680_SPARE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, (_val))
#define MODIFY_PHY54680_SPARE_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl 2 Reg (ADDR 1ch shadow 00100) */
#define READ_PHY54680_SPARE_CTRL_2r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, (_val))
#define WRITE_PHY54680_SPARE_CTRL_2r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, (_val))
#define MODIFY_PHY54680_SPARE_CTRL_2r(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl 3 Reg (ADDR 1ch shadow 00101) */
#define READ_PHY54680_SPARE_CTRL_3r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, (_val))
#define WRITE_PHY54680_SPARE_CTRL_3r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, (_val))
#define MODIFY_PHY54680_SPARE_CTRL_3r(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, \
                           (_val), (_mask))
#define PHY_54680_SPARE_CTRL3_REG_DLL_AUTO_PWR_DOWN (0x1 << 1)

/* 1000BASE-T/100BASE-TX/10BASE-T LED Status Reg (ADDR 1ch shadow 01000) */
#define READ_PHY54680_LED_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, (_val))
#define WRITE_PHY54680_LED_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, (_val))
#define MODIFY_PHY54680_LED_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED Ctrl Reg (ADDR 1ch shadow 01001) */
#define READ_PHY54680_LED_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, (_val))
#define WRITE_PHY54680_LED_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, (_val))
#define MODIFY_PHY54680_LED_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, \
                           (_val), (_mask))

/* Auto Power-Down Reg (ADDR 1ch shadow 01010) */
#define READ_PHY54680_AUTO_POWER_DOWNr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000a, 0x1c, (_val))
#define WRITE_PHY54680_AUTO_POWER_DOWNr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000a, 0x1c, (_val))
#define MODIFY_PHY54680_AUTO_POWER_DOWNr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000a, 0x1c, \
                           (_val), (_mask))

#define PHY_54680_AUTO_PWRDWN_EN              (1 << 5)
#define PHY_54680_AUTO_PWRDWN_WAKEUP_MASK     (0xF << 0)
#define PHY_54680_AUTO_PWRDWN_WAKEUP_UNIT     84  /* ms */
#define PHY_54680_AUTO_PWRDWN_WAKEUP_MAX      1260  /* ms */
#define PHY_54680_AUTO_PWRDWN_SLEEP_MASK      (1 << 4)
#define PHY_54680_AUTO_PWRDWN_SLEEP_MAX       5400  /* ms */

/* SLED_1 Reg (ADDR 1ch shadow 01011) */
#define READ_PHY54680_SLED_1r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000b, 0x1c, (_val))
#define WRITE_PHY54680_SLED_1r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000b, 0x1c, (_val))
#define MODIFY_PHY54680_SLED_1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000b, 0x1c, \
                           (_val), (_mask))

/* SLED_2 Reg (ADDR 1ch shadow 01100) */
#define READ_PHY54680_SLED_2r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000c, 0x1c, (_val))
#define WRITE_PHY54680_SLED_2r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000c, 0x1c, (_val))
#define MODIFY_PHY54680_SLED_2r(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000c, 0x1c, \
                           (_val), (_mask))


/* 1000BASE-T/100BASE-TX/10BASE-T LED Selector 1 Reg (ADDR 1ch shadow 01101) */
#define READ_PHY54680_LED_SELECTOR_1r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000d, 0x1c, (_val))
#define WRITE_PHY54680_LED_SELECTOR_1r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000d, 0x1c, (_val))
#define MODIFY_PHY54680_LED_SELECTOR_1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000d, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED Selector 2 Reg (ADDR 1ch shadow 01110) */
#define READ_PHY54680_LED_SELECTOR_2r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000e, 0x1c, (_val))
#define WRITE_PHY54680_LED_SELECTOR_2r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000e, 0x1c, (_val))
#define MODIFY_PHY54680_LED_SELECTOR_2r(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000e, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED GPIO Ctrl/Stat (ADDR 1ch shadow 01111) */
#define READ_PHY54680_LED_GPIO_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000F, 0x1c, (_val))
#define WRITE_PHY54680_LED_GPIO_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000F, 0x1c, (_val))
#define MODIFY_PHY54680_LED_GPIO_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000F, 0x1c, \
                           (_val), (_mask))

/*  External SerDes Control Reg (ADDR 1ch shadow 10100) */
#define READ_PHY54680_EXT_SERDES_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0014, 0x1c, (_val))
#define WRITE_PHY54680_EXT_SERDES_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0014, 0x1c, (_val))
#define MODIFY_PHY54680_EXT_SERDES_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0014, 0x1c, \
                           (_val), (_mask))

/*  SGMII SLAVE Reg (ADDR 1ch shadow 10101) */
#define READ_PHY54680_SGMII_SLAVEr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0015, 0x1c, (_val))
#define WRITE_PHY54680_SGMII_SLAVEr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0015, 0x1c, (_val))
#define MODIFY_PHY54680_SGMII_SLAVEr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0015, 0x1c, \
                           (_val), (_mask))

/*  Misc. 1000BASE-X Control 2 Reg (ADDR 1ch shadow 10110) */
#define READ_PHY54680_MISC_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0016, 0x1c, (_val))
#define WRITE_PHY54680_MISC_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0016, 0x1c, (_val))
#define MODIFY_PHY54680_MISC_1000X_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0016, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-X Auto-Detect SGMII/Media Converter Reg (ADDR 1ch shadow 11000) */
#define READ_PHY54680_AUTO_DETECT_SGMII_MEDIAr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0018, 0x1c, (_val))
#define WRITE_PHY54680_AUTO_DETECT_SGMII_MEDIAr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0018, 0x1c, (_val))
#define MODIFY_PHY54680_AUTO_DETECT_SGMII_MEDIAr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0018, 0x1c, \
                           (_val), (_mask))

/* Auxiliary Control Reg (ADDR 1ch shadow 11011) */
#define READ_PHY54680_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001B, 0x1c, (_val))
#define WRITE_PHY54680_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001B, 0x1c, (_val))
#define MODIFY_PHY54680_AUX_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001B, 0x1c, \
                           (_val), (_mask))

/* Auxiliary 1000BASE-X Status Reg (ADDR 1ch shadow 11100) */
#define READ_PHY54680_AUX_1000X_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001C, 0x1c, (_val))
#define WRITE_PHY54680_AUX_1000X_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001C, 0x1c, (_val))
#define MODIFY_PHY54680_AUX_1000X_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001C, 0x1c, \
                           (_val), (_mask))

/* Mode Control Reg (ADDR 1ch shadow 11111) */
#define READ_PHY54680_MODE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001F, 0x1c, (_val))
#define WRITE_PHY54680_MODE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001F, 0x1c, (_val))
#define MODIFY_PHY54680_MODE_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001F, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Master/Slave Seed Reg (ADDR 1dh bit 15 = 0) */
/* 1000BASE-T/100BASE-TX/10BASE-T HDC Status Reg (ADDR 1dh bit 15 = 1) */

/* 1000BASE-T/100BASE-TX/10BASE-T Test Register 1 (ADDR 1eh) */
#define READ_PHY54680_TEST1r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, (_val))
#define WRITE_PHY54680_TEST1r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, (_val))
#define MODIFY_PHY54680_TEST1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, \
                           (_val), (_mask))

/*          +---------------------+
 *          |                     |
 *          |   SGMII Registers   |
 *          |                     |
 *          +---------------------+
 */
/* SGMII Control Register (Addr 00h) */
#define READ_PHY54680_SGMII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x00, (_val))
#define WRITE_PHY54680_SGMII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x00, (_val))
#define MODIFY_PHY54680_SGMII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x00, (_val), (_mask))

/* SGMII Status Register (Addr 01h) */
#define READ_PHY54680_SGMII_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x01, (_val))
#define WRITE_PHY54680_SGMII_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x01, (_val))
#define MODIFY_PHY54680_SGMII_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x01, (_val), (_mask))

/* SGMII Auto-neg Advertise Register (Addr 04h) */
#define READ_PHY54680_SGMII_ANAr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x04, (_val))
#define WRITE_PHY54680_SGMII_ANAr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x04, (_val))
#define MODIFY_PHY54680_SGMII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x04, (_val), (_mask))

/* SGMII Auto-neg Link Partner Ability Register (Addr 05h) */
#define READ_PHY54680_SGMII_ANPr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x05, (_val))
#define WRITE_PHY54680_SGMII_ANPr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x05, (_val))
#define MODIFY_PHY54680_SGMII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x05, (_val), (_mask))

/*          +-------------------------+
 *          |                         |
 *          |   Expansion Registers   |
 *          |                         |
 *          +-------------------------+
 */
/* Receive/Transmit Packet Counter Register (Addr 00h) */
#define READ_PHY54680_EXP_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F00, 0x15, (_val))
#define WRITE_PHY54680_EXP_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F00, \
                               0x15, (_val))
#define MODIFY_PHY54680_EXP_PKT_COUNTERr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F00, \
                                0x15, (_val), (_mask))

/* Multicolor LED Selector Register (Addr 04h) */
#define READ_PHY54680_EXP_LED_SELECTORr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F04, 0x15, (_val))
#define WRITE_PHY54680_EXP_LED_SELECTORr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F04, \
                               0x15, (_val))
#define MODIFY_PHY54680_EXP_LED_SELECTORr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F04, \
                                0x15, (_val), (_mask))

/* Multicolor LED Flash Rate Controls Register (Addr 05h) */
#define READ_PHY54680_EXP_LED_FLASH_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F05, 0x15, (_val))
#define WRITE_PHY54680_EXP_LED_FLASH_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F05, \
                               0x15, (_val))
#define MODIFY_PHY54680_EXP_LED_FLASH_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F05, \
                                0x15, (_val), (_mask))

/* Multicolor LED Programmable Blink Controls Register (Addr 06h) */
#define READ_PHY54680_EXP_LED_BLINK_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F06, 0x15, (_val))
#define WRITE_PHY54680_EXP_LED_BLINK_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F06, \
                               0x15, (_val))
#define MODIFY_PHY54680_EXP_LED_BLINK_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F06, \
                                0x15, (_val), (_mask))

/* 100Base-FX Far End Fault egister (Addr 07h) */
#define READ_PHY54680_EXP_100FX_FAR_END_FAULTr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F07, 0x15, (_val))
#define WRITE_PHY54680_EXP_100FX_FAR_END_FAULTr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F07, \
                               0x15, (_val))
#define MODIFY_PHY54680_EXP_100FX_FAR_END_FAULTr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F07, \
                                0x15, (_val), (_mask))

/* Port Interrupt Status Register (Addr 0bh) */
#define READ_PHY54680_EXP_INTERRUPT_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F0B, 0x15, (_val))
#define WRITE_PHY54680_EXP_INTERRUPT_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F0B, \
                               0x15, (_val))
#define MODIFY_PHY54680_EXP_INTERRUPT_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F0B, \
                                0x15, (_val), (_mask))
/* Cable Diag Control Register (Addr 10h) */
#define READ_PHY54680_EXP_CABLE_DIAG_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F10, 0x15, (_val))
#define WRITE_PHY54680_EXP_CABLE_DIAG_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F10, \
                               0x15, (_val))
#define MODIFY_PHY54680_EXP_CABLE_DIAG_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F10, \
                                0x15, (_val), (_mask))

/* Cable Diag Result Register (Addr 11h) */
#define READ_PHY54680_EXP_CABLE_DIAG_RESULTr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F11, 0x15, (_val))
#define WRITE_PHY54680_EXP_CABLE_DIAG_RESULTr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F11, \
                               0x15, (_val))
#define MODIFY_PHY54680_EXP_CABLE_DIAG_RESULTr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F11, \
                                0x15, (_val), (_mask))

/* Cable Diag Length Register (Addr 12h) */
#define READ_PHY54680_EXP_CABLE_DIAG_LENGTHr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F12, 0x15, (_val))
#define WRITE_PHY54680_EXP_CABLE_DIAG_LENGTHr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F12, \
                               0x15, (_val))
#define MODIFY_PHY54680_EXP_CABLE_DIAG_LENGTHr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F12, \
                                0x15, (_val), (_mask))


/* Operating Mode Status Register (Addr 42h) */
#define READ_PHY54680_EXP_OPT_MODE_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F42, 0x15, (_val))
#define WRITE_PHY54680_EXP_OPT_MODE_STATr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F42, \
                               0x15, (_val))
#define MODIFY_PHY54680_EXP_OPT_MODE_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F42, \
                                0x15, (_val), (_mask))

/* SGMII/Lineside Loopback Control Register (Addr 44h) */
#define READ_PHY54680_EXP_SGMII_LINESIDE_LOOPBACK_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f44, 0x15, (_val))
#define WRITE_PHY54680_EXP_SGMII_LINESIDE_LOOPBACK_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f44, 0x15, (_val))
#define MODIFY_PHY54680_EXP_SGMII_LINESIDE_LOOPBACK_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f44, 0x15, \
                               (_val), (_mask))

/* SerDes/SGMII Control Register (Addr 52h) */
#define READ_PHY54680_EXP_SERDES_SGMII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f52, 0x15, (_val))
#define WRITE_PHY54680_EXP_SERDES_SGMII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f52, 0x15, (_val))
#define MODIFY_PHY54680_EXP_SERDES_SGMII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f52, 0x15, \
                               (_val), (_mask))


/* EEE related */

/* Auto EEE MII_Buffer CNTL/Status 2 Register (Addr AEh) */
#define READ_PHY54680_EXP_MII_BUF_CNTL_STAT2r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0FAE, 0x15, (_val))
#define WRITE_PHY54680_EXP_MII_BUF_CNTL_STAT2r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0FAE, \
                               0x15, (_val))
#define MODIFY_PHY54680_EXP_MII_BUF_CNTL_STAT2r(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0FAE, \
                                0x15, (_val), (_mask))

/* Auto EEE MII_Buffer CNTL/Status 1 Register (Addr AFh) */
#define READ_PHY54680_EXP_MII_BUF_CNTL_STAT1r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0FAF, 0x15, (_val))
#define WRITE_PHY54680_EXP_MII_BUF_CNTL_STAT1r(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0FAF, \
                               0x15, (_val))
#define MODIFY_PHY54680_EXP_MII_BUF_CNTL_STAT1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0FAF, \
                                0x15, (_val), (_mask))

/* EEE Statistics related */
/* Local LPI request Counter Reg (Addr ach) */
#define READ_PHY54680_EXP_EEE_TX_EVENTSr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0FAC, 0x15, (_val))

/* Remote LPI request Counter Reg (Addr adh) */
#define READ_PHY54680_EXP_EEE_RX_EVENTSr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0FAD, 0x15, (_val))

/* Statistic Timer_12_hours_lpi remote Reg (Addr aah) */
#define READ_PHY54680_EXP_EEE_TX_DURATIONr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0FAA, 0x15, (_val))

/* Statistic Timer_12_hours_lpi_local Local Reg (Addr abh) */
#define READ_PHY54680_EXP_EEE_RX_DURATIONr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0FAB, 0x15, (_val))

/* IEEE 1588 read start/end control Register (Addr f3h) */
#define READ_PHY54680_EXP_READ_START_END_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0ff3, 0x15, (_val))
#define WRITE_PHY54680_EXP_READ_START_END_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54680_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0ff3, 0x15, (_val))
#define MODIFY_PHY54680_EXP_READ_START_END_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54680_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0ff3, 0x15, \
                               (_val), (_mask))

/* EEE Capability Register */
#define READ_PHY54680_EEE_CAPr(_unit, _phy_ctrl, _val) \
        PHY54680E_CL45_REG_READ(_unit, _phy_ctrl, 0x00, 0x3, 0x14, _val)

/* EEE Advertisement Register */
#define READ_PHY54680_EEE_ADVr(_unit, _phy_ctrl, _val) \
        PHY54680E_CL45_REG_READ(_unit, _phy_ctrl, 0x00, 0x7, 0x3c, _val)
#define WRITE_PHY54680_EEE_ADVr(_unit, _phy_ctrl, _val) \
        PHY54680E_CL45_REG_WRITE(_unit, _phy_ctrl, 0x00, 0x7, 0x3c, _val)
#define MODIFY_PHY54680_EEE_ADVr(_unit, _phy_ctrl, _val, _mask) \
        PHY54680E_CL45_REG_MODIFY(_unit, _phy_ctrl, 0x00, 0x7, 0x3c, _val, _mask)

#define READ_PHY54680_EEE_803Dr(_unit, _phy_ctrl, _val) \
        PHY54680E_CL45_REG_READ(_unit, _phy_ctrl, 0x00, 0x7, 0x803d, _val)
#define WRITE_PHY54680_EEE_803Dr(_unit, _phy_ctrl, _val) \
        PHY54680E_CL45_REG_WRITE(_unit, _phy_ctrl, 0x00, 0x7, 0x803d, _val)
#define MODIFY_PHY54680_EEE_803Dr(_unit, _phy_ctrl, _val, _mask) \
        PHY54680E_CL45_REG_MODIFY(_unit, _phy_ctrl, 0x00, 0x7, 0x803d, _val, _mask)

/* EEE Resolution Status Register */
#define READ_PHY54680_EEE_RESOLUTION_STATr(_unit, _phy_ctrl, _val) \
        PHY54680E_CL45_REG_READ(_unit, _phy_ctrl, 0x00, 0x7, 0x803e, _val)
#define WRITE_PHY54680_EEE_RESOLUTION_STATr(_unit, _phy_ctrl, _val) \
        PHY54680E_CL45_REG_WRITE(_unit, _phy_ctrl, 0x00, 0x7, 0x803e, _val)
#define MODIFY_PHY54680_EEE_RESOLUTION_STATr(_unit, _phy_ctrl, _val, _mask) \
        PHY54680E_CL45_REG_MODIFY(_unit, _phy_ctrl, 0x00, 0x7, 0x803e, _val, _mask)

#define READ_PHY54680_EEE_TEST_CTRL_Ar(_unit, _phy_ctrl, _val) \
        PHY54680E_CL45_REG_READ(_unit, _phy_ctrl, 0x00, 0x7, 0x8030, _val)
#define WRITE_PHY54680_EEE_TEST_CTRL_Ar(_unit, _phy_ctrl, _val) \
        PHY54680E_CL45_REG_WRITE(_unit, _phy_ctrl, 0x00, 0x7, 0x8030, _val)
#define MODIFY_PHY54680_EEE_TEST_CTRL_Ar(_unit, _phy_ctrl, _val, _mask) \
        PHY54680E_CL45_REG_MODIFY(_unit, _phy_ctrl, 0x00, 0x7, 0x8030, _val, _mask)

/* EEE TEST Control Register B*/
#define READ_PHY54680_EEE_TEST_CTRL_Br(_unit, _phy_ctrl, _val) \
        PHY54680E_CL45_REG_READ(_unit, _phy_ctrl, 0x00, 0x7, 0x8031, _val)
#define WRITE_PHY54680_EEE_TEST_CTRL_Br(_unit, _phy_ctrl, _val) \
        PHY54680E_CL45_REG_WRITE(_unit, _phy_ctrl, 0x00, 0x7, 0x8031, _val)
#define MODIFY_PHY54680_EEE_TEST_CTRL_Br(_unit, _phy_ctrl, _val, _mask) \
        PHY54680E_CL45_REG_MODIFY(_unit, _phy_ctrl, 0x00, 0x7, 0x8031, _val, _mask)

typedef struct {
    uint32 flags;
    uint16 phy_id_orig;
    uint16 phy_id_base; /* port 0 addr */
    uint16 phy_slice;
} PHY54680_DEV_DESC_t;

#define PHY54680_DEV_PHY_ID_ORIG(_pc) (((PHY54680_DEV_DESC_t *)((_pc) + 1))->phy_id_orig)
#define PHY54680_DEV_PHY_ID_BASE(_pc) (((PHY54680_DEV_DESC_t *)((_pc) + 1))->phy_id_base)
#define PHY54680_DEV_PHY_SLICE(_pc) (((PHY54680_DEV_DESC_t *)((_pc) + 1))->phy_slice)
#define PHY54680_FLAGS(_pc) (((PHY54680_DEV_DESC_t *)((_pc) + 1))->flags)
#define PHY54680_DEV_PHY_SLICE_MASK         0x07
#define PHY54680_PHYA_REV (1U << 0)

#define PHY54680_DEV_SET_BASE_ADDR(_pc) \
        PHY54680_DEV_PHY_ID_BASE(_pc) =  (PHY54680_FLAGS((_pc)) & PHY54680_PHYA_REV) ?\
                                             PHY54680_DEV_PHY_ID_ORIG(_pc) + PHY54680_DEV_PHY_SLICE(_pc) : \
                                             PHY54680_DEV_PHY_ID_ORIG(_pc) - PHY54680_DEV_PHY_SLICE(_pc)

#define PHY54680_SLICE_ADDR(_pc, _slice) \
            (PHY54680_FLAGS((_pc)) & PHY54680_PHYA_REV) ? (PHY54680_DEV_PHY_ID_BASE(_pc) - (_slice)) : \
            (PHY54680_DEV_PHY_ID_BASE(_pc) + (_slice))

#endif /* _PHY54680_H_ */
