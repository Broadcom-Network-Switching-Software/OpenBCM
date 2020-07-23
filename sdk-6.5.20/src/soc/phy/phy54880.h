/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy54880.h
 *
 * This driver applies to both BCM54880 and BCM54880S.
 * Both parts support driving Copper or SERDES interfaces.
 *
 * The BCM54880S also has a SERDES pass-through mode, making it possible
 * to auto-select copper or fiber based on signal detects.
 */

#ifndef   _PHY54880_H_
#define   _PHY54880_H_

#include <soc/phy.h>

/* PHY register access */
#define PHY54880_REG_READ(_unit, _phy_ctrl, _flags, _reg_bank, _reg_addr, _val) \
            phy_reg_ge_read((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                            (_reg_addr), (_val))
#define PHY54880_REG_WRITE(_unit, _phy_ctrl, _flags, \
                          _reg_bank, _reg_addr, _val) \
            phy_reg_ge_write((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                             (_reg_addr), (_val))
#define PHY54880_REG_MODIFY(_unit, _phy_ctrl, _flags, _reg_bank, \
                           _reg_addr, _val, _mask) \
            phy_reg_ge_modify((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                              (_reg_addr), (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T MII Control Register (Addr 00h) */
#define READ_PHY54880_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val))
#define WRITE_PHY54880_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val)) 
#define MODIFY_PHY54880_MII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, \
                               (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T MII Status Register (ADDR 01h) */
#define READ_PHY54880_MII_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val)) 
#define WRITE_PHY54880_MII_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val)) 
#define MODIFY_PHY54880_MII_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, \
                               (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 02h) */
#define READ_PHY54880_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define WRITE_PHY54880_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define MODIFY_PHY54880_MII_PHY_ID0r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 03h) */
#define READ_PHY54880_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define WRITE_PHY54880_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define MODIFY_PHY54880_MII_PHY_ID1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Advertisment Register (ADDR 04h) */
#define READ_PHY54880_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define WRITE_PHY54880_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define MODIFY_PHY54880_MII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Link Partner Ability (ADDR 05h) */
#define READ_PHY54880_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val))
#define WRITE_PHY54880_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val)) 
#define MODIFY_PHY54880_MII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Expansion Register (ADDR 06h) */
#define READ_PHY54880_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define WRITE_PHY54880_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define MODIFY_PHY54880_MII_AN_EXPr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Next Page Transmit Register (ADDR 07h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Link Partner Received Next Page (ADDR 08h) */

/* 1000BASE-T Control Register  (ADDR 09h) */
#define READ_PHY54880_MII_GB_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define WRITE_PHY54880_MII_GB_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define MODIFY_PHY54880_MII_GB_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, \
                             (_val), (_mask))

/* 1000BASE-T Status Register (ADDR 0ah) */
#define READ_PHY54880_MII_GB_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val)) 
#define WRITE_PHY54880_MII_GB_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val))
#define MODIFY_PHY54880_MII_GB_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, \
                             (_val), (_mask))

/* (Addr 0ch-0eh) Reserved (Do not read/write to reserved registers. */

/* 1000BASE-T/100BASE-TX/10BASE-T IEEE Extended Status Register (ADDR 0fh) */

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Extended Control Register (ADDR 10h) */
#define READ_PHY54880_MII_ECRr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, (_val)) 
#define WRITE_PHY54880_MII_ECRr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, (_val))
#define MODIFY_PHY54880_MII_ECRr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Extended Status Register (ADDR 11h) */
#define READ_PHY54880_MII_ESRr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, (_val)) 
#define WRITE_PHY54880_MII_ESRr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, (_val))
#define MODIFY_PHY54880_MII_ESRr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Receive Error Counter Register (ADDR 12h) */
/* 1000BASE-T/100BASE-TX/10BASE-T False Carrier Sense Counter (ADDR 13h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Receive NOT_OK Counter Register (ADDR 14h) */

/* 1000BASE-T/100BASE-TX/10BASE-T Auxiliary Control Reg (ADDR 18h Shadow 000)*/
#define READ_PHY54880_MII_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, (_val))
#define WRITE_PHY54880_MII_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, (_val))
#define MODIFY_PHY54880_MII_AUX_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, \
                             (_val), (_mask))

/* 10BASE-T Register (ADDR 18h Shadow 001) */
#define READ_PHY54880_MII_10BASE_Tr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, (_val))
#define WRITE_PHY54880_MII_10BASE_Tr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, (_val))
#define MODIFY_PHY54880_MII_10BASE_Tr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Power/MII Control Reg (ADDR 18h Shadow 010)*/
#define READ_PHY54880_MII_POWER_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, (_val))
#define WRITE_PHY54880_MII_POWER_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, (_val))
#define MODIFY_PHY54880_MII_POWER_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Misc Test Register (ADDR 18h Shadow 100) */
#define READ_PHY54880_MII_MISC_TESTr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, (_val))
#define WRITE_PHY54880_MII_MISC_TESTr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, (_val))
#define MODIFY_PHY54880_MII_MISC_TESTr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Misc Control Register (ADDR 18h Shadow 111)*/
#define READ_PHY54880_MII_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, (_val))
#define WRITE_PHY54880_MII_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, (_val))
#define MODIFY_PHY54880_MII_MISC_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auxiliary Status Register (ADDR 19h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Interrupt Status Register (ADDR 1ah) */
/* 1000BASE-T/100BASE-TX/10BASE-T Interrupt Control Register (ADDR 1bh) */

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl Reg (ADDR 1ch shadow 00010) */
#define READ_PHY54880_SPARE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, (_val))
#define WRITE_PHY54880_SPARE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, (_val))
#define MODIFY_PHY54880_SPARE_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl 2 Reg (ADDR 1ch shadow 00100) */
#define READ_PHY54880_SPARE_CTRL_2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, (_val))
#define WRITE_PHY54880_SPARE_CTRL_2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, (_val))
#define MODIFY_PHY54880_SPARE_CTRL_2r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl 3 Reg (ADDR 1ch shadow 00101) */
#define READ_PHY54880_SPARE_CTRL_3r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, (_val))
#define WRITE_PHY54880_SPARE_CTRL_3r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, (_val))
#define MODIFY_PHY54880_SPARE_CTRL_3r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED Status Reg (ADDR 1ch shadow 01000) */
#define READ_PHY54880_LED_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, (_val))
#define WRITE_PHY54880_LED_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, (_val))
#define MODIFY_PHY54880_LED_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED Ctrl Reg (ADDR 1ch shadow 01001) */
#define READ_PHY54880_LED_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, (_val))
#define WRITE_PHY54880_LED_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, (_val))
#define MODIFY_PHY54880_LED_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, \
                           (_val), (_mask))

/* Auto Power-Down Reg (ADDR 1ch shadow 01010) */
#define READ_PHY54880_AUTO_POWER_DOWNr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000a, 0x1c, (_val))
#define WRITE_PHY54880_AUTO_POWER_DOWNr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000a, 0x1c, (_val))
#define MODIFY_PHY54880_AUTO_POWER_DOWNr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000a, 0x1c, \
                           (_val), (_mask))

#define PHY_54880_AUTO_PWRDWN_EN              (1 << 5)
#define PHY_54880_AUTO_PWRDWN_WAKEUP_MASK     (0xF << 0)
#define PHY_54880_AUTO_PWRDWN_WAKEUP_UNIT     84  /* ms */
#define PHY_54880_AUTO_PWRDWN_WAKEUP_MAX      1260  /* ms */
#define PHY_54880_AUTO_PWRDWN_SLEEP_MASK      (1 << 4)
#define PHY_54880_AUTO_PWRDWN_SLEEP_MAX       5400  /* ms */

/* SLED_1 Reg (ADDR 1ch shadow 01011) */
#define READ_PHY54880_SLED_1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000b, 0x1c, (_val))
#define WRITE_PHY54880_SLED_1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000b, 0x1c, (_val))
#define MODIFY_PHY54880_SLED_1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000b, 0x1c, \
                           (_val), (_mask))

/* SLED_2 Reg (ADDR 1ch shadow 01100) */
#define READ_PHY54880_SLED_2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000c, 0x1c, (_val))
#define WRITE_PHY54880_SLED_2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000c, 0x1c, (_val))
#define MODIFY_PHY54880_SLED_2r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000c, 0x1c, \
                           (_val), (_mask))


/* 1000BASE-T/100BASE-TX/10BASE-T LED Selector 1 Reg (ADDR 1ch shadow 01101) */
#define READ_PHY54880_LED_SELECTOR_1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000d, 0x1c, (_val))
#define WRITE_PHY54880_LED_SELECTOR_1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000d, 0x1c, (_val))
#define MODIFY_PHY54880_LED_SELECTOR_1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000d, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED Selector 2 Reg (ADDR 1ch shadow 01110) */
#define READ_PHY54880_LED_SELECTOR_2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000e, 0x1c, (_val))
#define WRITE_PHY54880_LED_SELECTOR_2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000e, 0x1c, (_val))
#define MODIFY_PHY54880_LED_SELECTOR_2r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000e, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED GPIO Ctrl/Stat (ADDR 1ch shadow 01111) */
#define READ_PHY54880_LED_GPIO_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000F, 0x1c, (_val))
#define WRITE_PHY54880_LED_GPIO_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000F, 0x1c, (_val))
#define MODIFY_PHY54880_LED_GPIO_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000F, 0x1c, \
                           (_val), (_mask))

/*  SGMII SLAVE Reg (ADDR 1ch shadow 10101) */
#define READ_PHY54880_SGMII_SLAVEr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0015, 0x1c, (_val))
#define WRITE_PHY54880_SGMII_SLAVEr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0015, 0x1c, (_val))
#define MODIFY_PHY54880_SGMII_SLAVEr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0015, 0x1c, \
                           (_val), (_mask))

/*  Misc. 1000BASE-X Control 2 Reg (ADDR 1ch shadow 10110) */
#define READ_PHY54880_MISC_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0016, 0x1c, (_val))
#define WRITE_PHY54880_MISC_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0016, 0x1c, (_val))
#define MODIFY_PHY54880_MISC_1000X_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0016, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-X Auto-Detect SGMII/Media Converter Reg (ADDR 1ch shadow 11000) */
#define READ_PHY54880_AUTO_DETECT_SGMII_MEDIAr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0018, 0x1c, (_val))
#define WRITE_PHY54880_AUTO_DETECT_SGMII_MEDIAr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0018, 0x1c, (_val))
#define MODIFY_PHY54880_AUTO_DETECT_SGMII_MEDIAr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0018, 0x1c, \
                           (_val), (_mask))

/* Auxiliary Control Reg (ADDR 1ch shadow 11011) */
#define READ_PHY54880_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001B, 0x1c, (_val))
#define WRITE_PHY54880_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001B, 0x1c, (_val))
#define MODIFY_PHY54880_AUX_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001B, 0x1c, \
                           (_val), (_mask))

/* Auxiliary 1000BASE-X Status Reg (ADDR 1ch shadow 11100) */
#define READ_PHY54880_AUX_1000X_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001C, 0x1c, (_val))
#define WRITE_PHY54880_AUX_1000X_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001C, 0x1c, (_val))
#define MODIFY_PHY54880_AUX_1000X_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001C, 0x1c, \
                           (_val), (_mask))

/* Mode Control Reg (ADDR 1ch shadow 11111) */
#define READ_PHY54880_MODE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001F, 0x1c, (_val))
#define WRITE_PHY54880_MODE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001F, 0x1c, (_val))
#define MODIFY_PHY54880_MODE_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001F, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Master/Slave Seed Reg (ADDR 1dh bit 15 = 0) */
/* 1000BASE-T/100BASE-TX/10BASE-T HDC Status Reg (ADDR 1dh bit 15 = 1) */

/* 1000BASE-T/100BASE-TX/10BASE-T Test Register 1 (ADDR 1eh) */
#define READ_PHY54880_TEST1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, (_val))
#define WRITE_PHY54880_TEST1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, (_val))
#define MODIFY_PHY54880_TEST1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, \
                           (_val), (_mask))

/*          +---------------------+
 *          |                     |
 *          |   SGMII Registers   |
 *          |                     |
 *          +---------------------+
 */
/* SGMII Control Register (Addr 00h) */
#define READ_PHY54880_SGMII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x00, (_val))
#define WRITE_PHY54880_SGMII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x00, (_val))
#define MODIFY_PHY54880_SGMII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x00, (_val), (_mask))

/* SGMII Status Register (Addr 01h) */
#define READ_PHY54880_SGMII_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x01, (_val))
#define WRITE_PHY54880_SGMII_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x01, (_val))
#define MODIFY_PHY54880_SGMII_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x01, (_val), (_mask))

/* SGMII Auto-neg Advertise Register (Addr 04h) */
#define READ_PHY54880_SGMII_ANAr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x04, (_val))
#define WRITE_PHY54880_SGMII_ANAr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x04, (_val))
#define MODIFY_PHY54880_SGMII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x04, (_val), (_mask))

/* SGMII Auto-neg Link Partner Ability Register (Addr 05h) */
#define READ_PHY54880_SGMII_ANPr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x05, (_val))
#define WRITE_PHY54880_SGMII_ANPr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x05, (_val))
#define MODIFY_PHY54880_SGMII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x05, (_val), (_mask))

/*          +-------------------------+
 *          |                         |
 *          |   Expansion Registers   |
 *          |                         |
 *          +-------------------------+
 */
/* Receive/Transmit Packet Counter Register (Addr 00h) */
#define READ_PHY54880_EXP_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F00, 0x15, (_val))
#define WRITE_PHY54880_EXP_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F00, \
                               0x15, (_val))
#define MODIFY_PHY54880_EXP_PKT_COUNTERr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F00, \
                                0x15, (_val), (_mask))

/* Multicolor LED Selector Register (Addr 04h) */
#define READ_PHY54880_EXP_LED_SELECTORr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F04, 0x15, (_val))
#define WRITE_PHY54880_EXP_LED_SELECTORr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F04, \
                               0x15, (_val))
#define MODIFY_PHY54880_EXP_LED_SELECTORr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F04, \
                                0x15, (_val), (_mask))

/* Multicolor LED Flash Rate Controls Register (Addr 05h) */
#define READ_PHY54880_EXP_LED_FLASH_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F05, 0x15, (_val))
#define WRITE_PHY54880_EXP_LED_FLASH_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F05, \
                               0x15, (_val))
#define MODIFY_PHY54880_EXP_LED_FLASH_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F05, \
                                0x15, (_val), (_mask))

/* Multicolor LED Programmable Blink Controls Register (Addr 06h) */
#define READ_PHY54880_EXP_LED_BLINK_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F06, 0x15, (_val))
#define WRITE_PHY54880_EXP_LED_BLINK_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F06, \
                               0x15, (_val))
#define MODIFY_PHY54880_EXP_LED_BLINK_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F06, \
                                0x15, (_val), (_mask))

/* 100Base-FX Far End Fault egister (Addr 07h) */
#define READ_PHY54880_EXP_100FX_FAR_END_FAULTr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F07, 0x15, (_val))
#define WRITE_PHY54880_EXP_100FX_FAR_END_FAULTr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F07, \
                               0x15, (_val))
#define MODIFY_PHY54880_EXP_100FX_FAR_END_FAULTr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F07, \
                                0x15, (_val), (_mask))

/* Port Interrupt Status Register (Addr 0bh) */
#define READ_PHY54880_EXP_INTERRUPT_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F0B, 0x15, (_val))
#define WRITE_PHY54880_EXP_INTERRUPT_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F0B, \
                               0x15, (_val))
#define MODIFY_PHY54880_EXP_INTERRUPT_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F0B, \
                                0x15, (_val), (_mask))
/* Cable Diag Control Register (Addr 10h) */
#define READ_PHY54880_EXP_CABLE_DIAG_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F10, 0x15, (_val))
#define WRITE_PHY54880_EXP_CABLE_DIAG_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F10, \
                               0x15, (_val))
#define MODIFY_PHY54880_EXP_CABLE_DIAG_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F10, \
                                0x15, (_val), (_mask))

/* Cable Diag Result Register (Addr 11h) */
#define READ_PHY54880_EXP_CABLE_DIAG_RESULTr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F11, 0x15, (_val))
#define WRITE_PHY54880_EXP_CABLE_DIAG_RESULTr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F11, \
                               0x15, (_val))
#define MODIFY_PHY54880_EXP_CABLE_DIAG_RESULTr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F11, \
                                0x15, (_val), (_mask))

/* Cable Diag Length Register (Addr 12h) */
#define READ_PHY54880_EXP_CABLE_DIAG_LENGTHr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F12, 0x15, (_val))
#define WRITE_PHY54880_EXP_CABLE_DIAG_LENGTHr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F12, \
                               0x15, (_val))
#define MODIFY_PHY54880_EXP_CABLE_DIAG_LENGTHr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F12, \
                                0x15, (_val), (_mask))


/* Operating Mode Status Register (Addr 42h) */
#define READ_PHY54880_EXP_OPT_MODE_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F42, 0x15, (_val))
#define WRITE_PHY54880_EXP_OPT_MODE_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F42, \
                               0x15, (_val))
#define MODIFY_PHY54880_EXP_OPT_MODE_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F42, \
                                0x15, (_val), (_mask))

/* SerDes/SGMII Control Register (Addr 52h) */
#define READ_PHY54880_EXP_SERDES_SGMII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f52, 0x15, (_val))
#define WRITE_PHY54880_EXP_SERDES_SGMII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f52, 0x15, (_val))
#define MODIFY_PHY54880_EXP_SERDES_SGMII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f52, 0x15, \
                               (_val), (_mask))

/* BroadR-Reach LRE */

/* LRE Control Register (Addr 00h) */
#define READ_PHY54880_LRE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val))
#define WRITE_PHY54880_LRE_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val))
#define MODIFY_PHY54880_LRE_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, \
                               (_val), (_mask))

/* LRE Status Register (ADDR 01h) */
#define READ_PHY54880_LRE_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val))
#define WRITE_PHY54880_LRE_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val))
#define MODIFY_PHY54880_LRE_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, \
                               (_val), (_mask))

/* LRE PHY Identifier Register (ADDR 02h) */
#define READ_PHY54880_LRE_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define WRITE_PHY54880_LRE_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define MODIFY_PHY54880_LRE_PHY_ID0r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, \
                             (_val), (_mask))

/* LRE PHY Identifier Register (ADDR 03h) */
#define READ_PHY54880_LRE_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define WRITE_PHY54880_LRE_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define MODIFY_PHY54880_LRE_PHY_ID1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, \
                             (_val), (_mask))

/* LDS Advertised Ability Register (ADDR 04h) */
#define READ_PHY54880_LDS_ADVAr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define WRITE_PHY54880_LDS_ADVAr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define MODIFY_PHY54880_LDS_ADVAr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, \
                             (_val), (_mask))

/* LDS Advertised Control Register (ADDR 05h) */
#define READ_PHY54880_LDS_ADVCr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val))
#define WRITE_PHY54880_LDS_ADVCr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val))
#define MODIFY_PHY54880_LDS_ADVCr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, \
                             (_val), (_mask))

/* LDS Ability Next Page Register (ADDR 06h) */
#define READ_PHY54880_LDS_ABNPr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define WRITE_PHY54880_LDS_ABNPr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define MODIFY_PHY54880_LDS_ABNPr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, \
                             (_val), (_mask))

/* LDS Link Partner Ability Register (ADDR 07h) */
#define READ_PHY54880_LDS_LPABr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x07, (_val))
#define WRITE_PHY54880_LDS_LPABr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x07, (_val))
#define MODIFY_PHY54880_LDS_LPABr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x07, \
                             (_val), (_mask))

/* LDS Link Partner Next Page Message Register (ADDR 08h) */
#define READ_PHY54880_LDS_LPNPMr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x08, (_val))
#define WRITE_PHY54880_LDS_LPNPMr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x08, (_val))
#define MODIFY_PHY54880_LDS_LPNPMr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x08, \
                             (_val), (_mask))

/* LDS Ability Next Page Register (ADDR 09h) */
#define READ_PHY54880_LDS_LPABNPr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define WRITE_PHY54880_LDS_LPABNPr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define MODIFY_PHY54880_LDS_LPABNPr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, \
                             (_val), (_mask))

/* LDS Expansion Register (ADDR 0ah) */
#define READ_PHY54880_LDS_EXPr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val))
#define WRITE_PHY54880_LDS_EXPr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val))
#define MODIFY_PHY54880_LDS_EXPr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, \
                             (_val), (_mask))

/* LDS Access Register (ADDR 0eh) */
#define READ_PHY54880_LDS_ACCr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0e, (_val))
#define WRITE_PHY54880_LDS_ACCr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0e, (_val))
#define MODIFY_PHY54880_LDS_ACCr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0e, \
                             (_val), (_mask))

/* LRE Extended Status Register (ADDR 0fh) */
#define READ_PHY54880_LRE_EXTEN_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0f, (_val))
#define WRITE_PHY54880_LRE_EXTEN_STATr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0f, (_val))
#define MODIFY_PHY54880_LRE_EXTEN_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0f, \
                             (_val), (_mask))

/* BroadR-Reach LRE Misc Control Register1 (Addr 90h) */
#define READ_PHY54880_EXP_BR_LRE_MISC_CTRL1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f90, 0x15, (_val))
#define WRITE_PHY54880_EXP_BR_LRE_MISC_CTRL1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f90, 0x15, (_val))
#define MODIFY_PHY54880_EXP_BR_LRE_MISC_CTRL1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f90, 0x15, \
                               (_val), (_mask))

/* BroadR-Reach LRE Misc Control Register2 (Addr 91h) */
#define READ_PHY54880_EXP_BR_LRE_MISC_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f91, 0x15, (_val))
#define WRITE_PHY54880_EXP_BR_LRE_MISC_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f91, 0x15, (_val))
#define MODIFY_PHY54880_EXP_BR_LRE_MISC_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f91, 0x15, \
                               (_val), (_mask))

/* BroadR-Reach LRE Misc Control Register3 (Addr 92h) */
#define READ_PHY54880_EXP_BR_LRE_MISC_CTRL3r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f92, 0x15, (_val))
#define WRITE_PHY54880_EXP_BR_LRE_MISC_CTRL3r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f92, 0x15, (_val))
#define MODIFY_PHY54880_EXP_BR_LRE_MISC_CTRL3r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f92, 0x15, \
                               (_val), (_mask))

/* BroadR-Reach LDS Control Register (Addr 93h) */
#define READ_PHY54880_EXP_BR_LDS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f93, 0x15, (_val))
#define WRITE_PHY54880_EXP_BR_LDS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f93, 0x15, (_val))
#define MODIFY_PHY54880_EXP_BR_LDS_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f93, 0x15, \
                               (_val), (_mask))

/* BroadR-Reach LDS RX Control Register1 (Addr 94h) */
#define READ_PHY54880_EXP_BR_LDS_RX_CTRL1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f94, 0x15, (_val))
#define WRITE_PHY54880_EXP_BR_LDS_RX_CTRL1r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f94, 0x15, (_val))
#define MODIFY_PHY54880_EXP_BR_LDS_RX_CTRL1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f94, 0x15, \
                               (_val), (_mask))

/* BroadR-Reach LDS Scan Register (Addr 96h) */
#define READ_PHY54880_EXP_BR_LDS_SCANr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f96, 0x15, (_val))
#define WRITE_PHY54880_EXP_BR_LDS_SCANr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f96, 0x15, (_val))
#define MODIFY_PHY54880_EXP_BR_LDS_SCANr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f96, 0x15, \
                               (_val), (_mask))

#define PHY54880_EXP_BR_LDS_SCAN_SS_MASK       0xf /* Speed Selection Mask */
#define PHY54880_EXP_BR_LDS_SCAN_SS_10         0x0 /* 10Mbps */
#define PHY54880_EXP_BR_LDS_SCAN_SS_50         0x1 /* 50Mbps */
#define PHY54880_EXP_BR_LDS_SCAN_SS_33         0x2 /* 33Mbps */
#define PHY54880_EXP_BR_LDS_SCAN_SS_25         0x3 /* 25Mbps */
#define PHY54880_EXP_BR_LDS_SCAN_SS_20         0x4 /* 20Mbps */
#define PHY54880_EXP_BR_LDS_SCAN_SS_100        0x8 /* 100Mbps */

/* BroadR-Reach LDS RX Control Register2 (Addr 9fh) */
#define READ_PHY54880_EXP_BR_LDS_RX_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f9f, 0x15, (_val))
#define WRITE_PHY54880_EXP_BR_LDS_RX_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f9f, 0x15, (_val))
#define MODIFY_PHY54880_EXP_BR_LDS_RX_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f9f, 0x15, \
                               (_val), (_mask))




/*
 * 
 *  BCM54811 specific regs 
 */


/* BroadR-Reach LRE Misc Control (Addr 90h) */
#define READ_PHY54811_EXP_BR_LRE_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f90, 0x15, (_val))
#define WRITE_PHY54811_EXP_BR_LRE_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f90, 0x15, (_val))
#define MODIFY_PHY54811_EXP_BR_LRE_MISC_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f90, 0x15, \
                               (_val), (_mask))


/* BroadR-Reach LDS Misc Control 2 (Addr 91h) */
#define READ_PHY54811_EXP_BR_LRE_MISC_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f91, 0x15, (_val))
#define WRITE_PHY54811_EXP_BR_LRE_MISC_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f91, 0x15, (_val))
#define MODIFY_PHY54811_EXP_BR_LRE_MISC_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f91, 0x15, \
                               (_val), (_mask))

/* BroadR-Reach LDS Timer Control (Addr 99h) */
#define READ_PHY54811_EXP_BR_LDS_TIMER_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f99, 0x15, (_val))
#define WRITE_PHY54811_EXP_BR_LDS_TIMER_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f99, 0x15, (_val))
#define MODIFY_PHY54811_EXP_BR_LDS_TIMER_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f99, 0x15, \
                               (_val), (_mask))

/* BroadR-Reach LDS Channel Status (Addr 9Ah) */
#define READ_PHY54811_EXP_BR_LDS_CHAN_STATUSr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f9a, 0x15, (_val))
#define WRITE_PHY54811_EXP_BR_LDS_CHAN_STATUSr(_unit, _phy_ctrl, _val) \
            PHY54880_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f9a, 0x15, (_val))
#define MODIFY_PHY54811_EXP_BR_LDS_CHAN_STATUSr(_unit, _phy_ctrl, _val, _mask) \
            PHY54880_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f9a, 0x15, \
                               (_val), (_mask))
#define PHY54811_LDS_BR_IEEE_REG_ACCESS_OVERRIDE_EN  (0x8000)
#define PHY54811_LDS_BR_IEEE_REG_ACCESS_OVERRIDE_VAL (0x4000)
#define PHY54811_LDS_BR_IEEE_REG_ACCESS_CTRL_STATUS  (0x2000)



#endif /* _PHY54880_H_ */
