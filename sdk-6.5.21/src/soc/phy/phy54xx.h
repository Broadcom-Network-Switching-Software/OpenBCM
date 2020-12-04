/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy54xx.h
 */

#ifndef   _PHY54XX_H_
#define   _PHY54XX_H_

#include <soc/phy.h>

/* PHY register access */
#define PHY54XX_REG_READ(_unit, _phy_ctrl, _flags, _reg_bank, _reg_addr, _val) \
            phy_reg_ge_read((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                            (_reg_addr), (_val))
#define PHY54XX_REG_WRITE(_unit, _phy_ctrl, _flags, _reg_bank, \
                          _reg_addr, _val) \
            phy_reg_ge_write((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                             (_reg_addr), (_val))
#define PHY54XX_REG_MODIFY(_unit, _phy_ctrl, _flags, _reg_bank, \
                           _reg_addr, _val, _mask) \
            phy_reg_ge_modify((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                              (_reg_addr), (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T MII Control Register (Addr 00h) */
#define READ_PHY54XX_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val))
#define WRITE_PHY54XX_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val)) 
#define MODIFY_PHY54XX_MII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, \
                               (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T MII Status Register (ADDR 01h) */
#define READ_PHY54XX_MII_STATr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val)) 
#define WRITE_PHY54XX_MII_STATr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val)) 
#define MODIFY_PHY54XX_MII_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, \
                               (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 02h) */
#define READ_PHY54XX_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define WRITE_PHY54XX_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define MODIFY_PHY54XX_MII_PHY_ID0r(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 03h) */
#define READ_PHY54XX_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define WRITE_PHY54XX_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define MODIFY_PHY54XX_MII_PHY_ID1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Advertisment Register (ADDR 04h) */
#define READ_PHY54XX_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define WRITE_PHY54XX_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define MODIFY_PHY54XX_MII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Link Partner Ability (ADDR 05h) */
#define READ_PHY54XX_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val))
#define WRITE_PHY54XX_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val)) 
#define MODIFY_PHY54XX_MII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Expansion Register (ADDR 06h) */
#define READ_PHY54XX_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define WRITE_PHY54XX_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define MODIFY_PHY54XX_MII_AN_EXPr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Next Page Transmit Register (ADDR 07h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Link Partner Received Next Page (ADDR 08h) */

/* 1000BASE-T Control Register  (ADDR 09h) */
#define READ_PHY54XX_MII_GB_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define WRITE_PHY54XX_MII_GB_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define MODIFY_PHY54XX_MII_GB_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, \
                             (_val), (_mask))

/* 1000BASE-T Status Register (ADDR 0ah) */
#define READ_PHY54XX_MII_GB_STATr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val)) 
#define WRITE_PHY54XX_MII_GB_STATr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val))
#define MODIFY_PHY54XX_MII_GB_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, \
                             (_val), (_mask))

/* SGMII/1000X Control Register (Addr 0bh) */
#define READ_PHY54XX_SGMII_1000X_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0b, (_val)) 
#define WRITE_PHY54XX_SGMII_1000X_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0b, (_val))
#define MODIFY_PHY54XX_SGMII_1000X_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0b, \
                             (_val), (_mask))

/* SGMII/1000X Status Register (Addr 0ch) */
#define READ_PHY54XX_SGMII_1000X_STATr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0c, (_val)) 
#define WRITE_PHY54XX_SGMII_1000X_STATr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0c, (_val))
#define MODIFY_PHY54XX_SGMII_1000X_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0c, \
                             (_val), (_mask))

/* 1000-X Link Partner Capabilities Register (Addr 0dh) */
#define READ_PHY54XX_1000X_ANPr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0d, (_val)) 
#define WRITE_PHY54XX_1000X_ANPr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0d, (_val))
#define MODIFY_PHY54XX_1000X_ANPr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0d, \
                             (_val), (_mask))

/* SGMII/1000-X Misc Register (Addr 0eh) */
#define READ_PHY54XX_SGMII_1000X_MISCr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0e, (_val)) 
#define WRITE_PHY54XX_SGMII_1000X_MISCr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0e, (_val))
#define MODIFY_PHY54XX_SGMII_1000X_MISCr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0e, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T IEEE Extended Status Register (ADDR 0fh) */

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Extended Control Register (ADDR 10h) */
#define READ_PHY54XX_MII_ECRr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, (_val)) 
#define WRITE_PHY54XX_MII_ECRr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, (_val))
#define MODIFY_PHY54XX_MII_ECRr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Extended Status Register (ADDR 11h) */
#define READ_PHY54XX_MII_ESRr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, (_val)) 
#define WRITE_PHY54XX_MII_ESRr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, (_val))
#define MODIFY_PHY54XX_MII_ESRr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Receive Error Counter Register (ADDR 12h) */
/* 1000BASE-T/100BASE-TX/10BASE-T False Carrier Sense Counter (ADDR 13h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Receive NOT_OK Counter Register (ADDR 14h) */

/* Auxiliary Control Register for Normal Operation (Addr 18h Shadow 000) */
#define READ_PHY54XX_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, (_val))
#define WRITE_PHY54XX_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, (_val))
#define MODIFY_PHY54XX_AUX_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, \
                             (_val), (_mask))

/* 10BASE-T Register (ADDR 18h Shadow 001) */
#define READ_PHY54XX_10BASE_Tr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, (_val))
#define WRITE_PHY54XX_10BASE_Tr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, (_val))
#define MODIFY_PHY54XX_10BASE_Tr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, \
                             (_val), (_mask))

/* Power Control Reg (ADDR 18h Shadow 010)*/
#define READ_PHY54XX_POWER_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, (_val))
#define WRITE_PHY54XX_POWER_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, (_val))
#define MODIFY_PHY54XX_POWER_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Misc Test Register (ADDR 18h Shadow 100) */
#define READ_PHY54XX_MISC_TESTr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, (_val))
#define WRITE_PHY54XX_MISC_TESTr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, (_val))
#define MODIFY_PHY54XX_MISC_TESTr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Misc Control Register (ADDR 18h Shadow 111)*/
#define READ_PHY54XX_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, (_val))
#define WRITE_PHY54XX_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, (_val))
#define MODIFY_PHY54XX_MISC_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, \
                             (_val), (_mask))

/* Auxiliary Status Summary Register (ADDR 19h) */
/* Interrupt Status Register (ADDR 1ah) */
/* Interrupt Mask Register (ADDR 1bh) */

/* Spare Ctrl 1 Reg (ADDR 1ch shadow 00010) */
#define READ_PHY54XX_SPARE_CTRL1r(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, (_val))
#define WRITE_PHY54XX_SPARE_CTRL1r(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, (_val))
#define MODIFY_PHY54XX_SPARE_CTRL1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, \
                           (_val), (_mask))

/* Spare Control 2 Reg (Addr 1ch shadow 00100) */
#define READ_PHY54XX_SPARE_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, (_val))
#define WRITE_PHY54XX_SPARE_CTRL2r(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, (_val))
#define MODIFY_PHY54XX_SPARE_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, \
                           (_val), (_mask))

/* Spare Control 3 Reg (Addr 1ch shadow 00101) */
#define READ_PHY54XX_SPARE_CTRL3r(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, (_val))
#define WRITE_PHY54XX_SPARE_CTRL3r(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, (_val))
#define MODIFY_PHY54XX_SPARE_CTRL3r(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, \
                           (_val), (_mask))

/* LED Status Register (Addr 1ch shadow 01000) */
#define READ_PHY54XX_LED_STATr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, (_val))
#define WRITE_PHY54XX_LED_STATr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, (_val))
#define MODIFY_PHY54XX_LED_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, \
                           (_val), (_mask))

/* LED Control Register (Addr 1ch shadow 01001) */
#define READ_PHY54XX_LED_CTRLr(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, (_val))
#define WRITE_PHY54XX_LED_CTRL3r(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, (_val))
#define MODIFY_PHY54XX_LED_CTRL3r(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, \
                           (_val), (_mask))

/* Auto Power-Down Register (Addr 1ch shadow 01010) */

/* Master/Slave Seed Register (Addr 1dh) */

/* HCD Status Register (Addr 1dh) */

/* Test Register 1 (Addr 1eh) */
#define READ_PHY54XX_TEST1r(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, (_val))
#define WRITE_PHY54XX_TEST1r(_unit, _phy_ctrl, _val) \
            PHY54XX_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, (_val))
#define MODIFY_PHY54XX_TEST1r(_unit, _phy_ctrl, _val, _mask) \
            PHY54XX_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, \
                           (_val), (_mask))

#endif /* _PHY54XX_H_ */
