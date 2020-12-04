/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phyegphy28.h
 *
 * This driver applies to BCMEGPHY28.
 */

#ifndef   _PHYEGPHY28_H_
#define   _PHYEGPHY28_H_

#include <soc/phy.h>

/* PHY register access */
#define PHYEGPHY28_REG_READ(_unit, _phy_ctrl, _flags, _reg_bank, _reg_addr, _val) \
            phy_reg_ge_read((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                            (_reg_addr), (_val))
#define PHYEGPHY28_REG_WRITE(_unit, _phy_ctrl, _flags, \
                          _reg_bank, _reg_addr, _val) \
            phy_reg_ge_write((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                             (_reg_addr), (_val))
#define PHYEGPHY28_REG_MODIFY(_unit, _phy_ctrl, _flags, _reg_bank, \
                           _reg_addr, _val, _mask) \
            phy_reg_ge_modify((_unit), (_phy_ctrl), (_flags), (_reg_bank), \
                              (_reg_addr), (_val), (_mask))


/* Clause 45 Registers access using Clause 22 register access */
#define PHYEGPHY28_CL45_REG_READ(_unit, _phy_ctrl, _flags, _dev_addr, _reg_addr, _val) \
            _phy_egphy28_cl45_reg_read((_unit), (_phy_ctrl), (_flags), (_dev_addr), \
                            (_reg_addr), (_val))
#define PHYEGPHY28_CL45_REG_WRITE(_unit, _phy_ctrl, _flags, \
                          _dev_addr, _reg_addr, _val) \
            _phy_egphy28_cl45_reg_write((_unit), (_phy_ctrl), (_flags), (_dev_addr), \
                             (_reg_addr), (_val))
#define PHYEGPHY28_CL45_REG_MODIFY(_unit, _phy_ctrl, _flags, _dev_addr, \
                           _reg_addr, _val, _mask) \
            _phy_egphy28_cl45_reg_modify((_unit), (_phy_ctrl), (_flags), (_dev_addr), \
                              (_reg_addr), (_val), (_mask))


/**************************************************
 *
 * RDB/Legacy register accessing macros/functions
 */
#undef PHYEGPHY28_DEFAULT_TO_LEGACY_ADDRESSING   /* default to RDB mode */

/* Registers used to Rd/Wr using RDB mode */
#define PHYEGPHY28_RDB_ADDR_REG_OFFSET        (0x1E)
#define PHYEGPHY28_RDB_ADDR_REG_ADDR          (0xffff)
#define PHYEGPHY28_RDB_DATA_REG_OFFSET        (0x1F)
#define PHYEGPHY28_RDB_DATA_REG_DATA          (0xffff)
#define PHYEGPHY28_REG_1E_SELECT_RDB          (0x0087)
#define PHYEGPHY28_REG_1F_RDB_DIS             (0x8000)
/* Registers used to enable RDB register access mode */
#define PHYEGPHY28_REG_17_OFFSET              (0x17)
#define PHYEGPHY28_REG_17_SELECT_EXP_7E       (0x0F7E)
#define PHYEGPHY28_REG_15_OFFSET              (0x15)
#define PHYEGPHY28_REG_15_RDB_EN              (0x0000)

/*
 *  Macro for activating the RDB Register Addressing mode  *
 *  Enable direct RDB addressing mode, write to Expansion register 0x7E = 0x0000
 *  - MDIO write to reg 0x17 = 0x0F7E
 *  - MDIO write to reg 0x15 = 0x0000
 */
#define PHYEGPHY28_RDB_ACCESS_MODE(_u, _pc)   do {  \
    SOC_IF_ERROR_RETURN(  \
        WRITE_PHY_REG((_u), (_pc), PHYEGPHY28_REG_17_OFFSET, \
                                    PHYEGPHY28_REG_17_SELECT_EXP_7E) ); \
    SOC_IF_ERROR_RETURN(  \
        WRITE_PHY_REG((_u), (_pc), PHYEGPHY28_REG_15_OFFSET,    \
                                    PHYEGPHY28_REG_15_RDB_EN) ); \
} while ( 0 )

/*
 *  Macro for activating the Legacy Register Addressing mode  *
 *  Disable direct RDB addressing mode, write to RDB register 0x87 = 0x8000
 *  - MDIO write to reg 0x1E = 0x0087
 *  - MDIO write to reg 0x1F = 0x8000
 */
#define PHYEGPHY28_LEGACY_ACCESS_MODE(_u, _pc)   do {  \
    SOC_IF_ERROR_RETURN(  \
        WRITE_PHY_REG((_u), (_pc), PHYEGPHY28_RDB_ADDR_REG_OFFSET, \
                                    PHYEGPHY28_REG_1E_SELECT_RDB) );\
    SOC_IF_ERROR_RETURN(  \
        WRITE_PHY_REG((_u), (_pc), PHYEGPHY28_RDB_DATA_REG_OFFSET, \
                                    PHYEGPHY28_REG_1F_RDB_DIS) );   \
} while ( 0 )


/* 1000BASE-T/100BASE-TX/10BASE-T MII Control Register (Addr 00h) */
#define READ_PHYEGPHY28_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val))
#define WRITE_PHYEGPHY28_MII_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, (_val)) 
#define MODIFY_PHYEGPHY28_MII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x00, \
                               (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T MII Status Register (ADDR 01h) */
#define READ_PHYEGPHY28_MII_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val)) 
#define WRITE_PHYEGPHY28_MII_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, (_val)) 
#define MODIFY_PHYEGPHY28_MII_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x01, \
                               (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 02h) */
#define READ_PHYEGPHY28_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define WRITE_PHYEGPHY28_MII_PHY_ID0r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, (_val))
#define MODIFY_PHYEGPHY28_MII_PHY_ID0r(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x02, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Identifier Register (ADDR 03h) */
#define READ_PHYEGPHY28_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define WRITE_PHYEGPHY28_MII_PHY_ID1r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, (_val))
#define MODIFY_PHYEGPHY28_MII_PHY_ID1r(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x03, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Advertisment Register (ADDR 04h) */
#define READ_PHYEGPHY28_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define WRITE_PHYEGPHY28_MII_ANAr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, (_val))
#define MODIFY_PHYEGPHY28_MII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x04, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Link Partner Ability (ADDR 05h) */
#define READ_PHYEGPHY28_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val))
#define WRITE_PHYEGPHY28_MII_ANPr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, (_val)) 
#define MODIFY_PHYEGPHY28_MII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x05, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auto-neg Expansion Register (ADDR 06h) */
#define READ_PHYEGPHY28_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define WRITE_PHYEGPHY28_MII_AN_EXPr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, (_val))
#define MODIFY_PHYEGPHY28_MII_AN_EXPr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x06, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Next Page Transmit Register (ADDR 07h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Link Partner Received Next Page (ADDR 08h) */

/* 1000BASE-T Control Register  (ADDR 09h) */
#define READ_PHYEGPHY28_MII_GB_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define WRITE_PHYEGPHY28_MII_GB_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, (_val))
#define MODIFY_PHYEGPHY28_MII_GB_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x09, \
                             (_val), (_mask))

/* 1000BASE-T Status Register (ADDR 0ah) */
#define READ_PHYEGPHY28_MII_GB_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val)) 
#define WRITE_PHYEGPHY28_MII_GB_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, (_val))
#define MODIFY_PHYEGPHY28_MII_GB_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x0a, \
                             (_val), (_mask))

/* (Addr 0ch-0eh) Reserved (Do not read/write to reserved registers. */

/* 1000BASE-T/100BASE-TX/10BASE-T IEEE Extended Status Register (ADDR 0fh) */

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Extended Control Register (ADDR 10h) */
#define READ_PHYEGPHY28_MII_ECRr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, (_val)) 
#define WRITE_PHYEGPHY28_MII_ECRr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, (_val))
#define MODIFY_PHYEGPHY28_MII_ECRr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x10, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T PHY Extended Status Register (ADDR 11h) */
#define READ_PHYEGPHY28_MII_ESRr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, (_val)) 
#define WRITE_PHYEGPHY28_MII_ESRr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, (_val))
#define MODIFY_PHYEGPHY28_MII_ESRr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x11, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Receive Error Counter Register (ADDR 12h) */
/* 1000BASE-T/100BASE-TX/10BASE-T False Carrier Sense Counter (ADDR 13h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Receive NOT_OK Counter Register (ADDR 14h) */

/* 1000BASE-T/100BASE-TX/10BASE-T Auxiliary Control Reg (ADDR 18h Shadow 000)*/
#define READ_PHYEGPHY28_MII_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, (_val))
#define WRITE_PHYEGPHY28_MII_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, (_val))
#define MODIFY_PHYEGPHY28_MII_AUX_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x18, \
                             (_val), (_mask))

/* 10BASE-T Register (ADDR 18h Shadow 001) */
#define READ_PHYEGPHY28_MII_10BASE_Tr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, (_val))
#define WRITE_PHYEGPHY28_MII_10BASE_Tr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, (_val))
#define MODIFY_PHYEGPHY28_MII_10BASE_Tr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0001, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Power/MII Control Reg (ADDR 18h Shadow 010)*/
#define READ_PHYEGPHY28_MII_POWER_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, (_val))
#define WRITE_PHYEGPHY28_MII_POWER_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, (_val))
#define MODIFY_PHYEGPHY28_MII_POWER_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0002, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Misc Test Register (ADDR 18h Shadow 100) */
#define READ_PHYEGPHY28_MII_MISC_TESTr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, (_val))
#define WRITE_PHYEGPHY28_MII_MISC_TESTr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, (_val))
#define MODIFY_PHYEGPHY28_MII_MISC_TESTr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0004, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Misc Control Register (ADDR 18h Shadow 111)*/
#define READ_PHYEGPHY28_MII_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, (_val))
#define WRITE_PHYEGPHY28_MII_MISC_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, (_val))
#define MODIFY_PHYEGPHY28_MII_MISC_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0007, 0x18, \
                             (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Auxiliary Status Register (ADDR 19h) */
/* 1000BASE-T/100BASE-TX/10BASE-T Interrupt Status Register (ADDR 1ah) */
/* 1000BASE-T/100BASE-TX/10BASE-T Interrupt Control Register (ADDR 1bh) */

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl Reg (ADDR 1ch shadow 00010) */
#define READ_PHYEGPHY28_SPARE_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, (_val))
#define WRITE_PHYEGPHY28_SPARE_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_SPARE_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0002, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl 2 Reg (ADDR 1ch shadow 00100) */
#define READ_PHYEGPHY28_SPARE_CTRL_2r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, (_val))
#define WRITE_PHYEGPHY28_SPARE_CTRL_2r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_SPARE_CTRL_2r(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0004, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Spare Ctrl 3 Reg (ADDR 1ch shadow 00101) */
#define READ_PHYEGPHY28_SPARE_CTRL_3r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, (_val))
#define WRITE_PHYEGPHY28_SPARE_CTRL_3r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_SPARE_CTRL_3r(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0005, 0x1c, \
                           (_val), (_mask))
#define PHY_EGPHY28_SPARE_CTRL3_REG_DLL_AUTO_PWR_DOWN (0x1 << 1)

/* 1000BASE-T/100BASE-TX/10BASE-T LED Status Reg (ADDR 1ch shadow 01000) */
#define READ_PHYEGPHY28_LED_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, (_val))
#define WRITE_PHYEGPHY28_LED_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_LED_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0008, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED Ctrl Reg (ADDR 1ch shadow 01001) */
#define READ_PHYEGPHY28_LED_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, (_val))
#define WRITE_PHYEGPHY28_LED_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_LED_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0009, 0x1c, \
                           (_val), (_mask))

/* Auto Power-Down Reg (ADDR 1ch shadow 01010) */
#define READ_PHYEGPHY28_AUTO_POWER_DOWNr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000a, 0x1c, (_val))
#define WRITE_PHYEGPHY28_AUTO_POWER_DOWNr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000a, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_AUTO_POWER_DOWNr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000a, 0x1c, \
                           (_val), (_mask))

#define PHY_EGPHY28_AUTO_PWRDWN_EN              (1 << 5)
#define PHY_EGPHY28_AUTO_PWRDWN_WAKEUP_MASK     (0xF << 0)
#define PHY_EGPHY28_AUTO_PWRDWN_WAKEUP_UNIT     84  /* ms */
#define PHY_EGPHY28_AUTO_PWRDWN_WAKEUP_MAX      1260  /* ms */
#define PHY_EGPHY28_AUTO_PWRDWN_SLEEP_MASK      (1 << 4)
#define PHY_EGPHY28_AUTO_PWRDWN_SLEEP_MAX       5400  /* ms */

/* SLED_1 Reg (ADDR 1ch shadow 01011) */
#define READ_PHYEGPHY28_SLED_1r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000b, 0x1c, (_val))
#define WRITE_PHYEGPHY28_SLED_1r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000b, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_SLED_1r(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000b, 0x1c, \
                           (_val), (_mask))

/* SLED_2 Reg (ADDR 1ch shadow 01100) */
#define READ_PHYEGPHY28_SLED_2r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000c, 0x1c, (_val))
#define WRITE_PHYEGPHY28_SLED_2r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000c, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_SLED_2r(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000c, 0x1c, \
                           (_val), (_mask))


/* 1000BASE-T/100BASE-TX/10BASE-T LED Selector 1 Reg (ADDR 1ch shadow 01101) */
#define READ_PHYEGPHY28_LED_SELECTOR_1r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000d, 0x1c, (_val))
#define WRITE_PHYEGPHY28_LED_SELECTOR_1r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000d, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_LED_SELECTOR_1r(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000d, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED Selector 2 Reg (ADDR 1ch shadow 01110) */
#define READ_PHYEGPHY28_LED_SELECTOR_2r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000e, 0x1c, (_val))
#define WRITE_PHYEGPHY28_LED_SELECTOR_2r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000e, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_LED_SELECTOR_2r(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000e, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T LED GPIO Ctrl/Stat (ADDR 1ch shadow 01111) */
#define READ_PHYEGPHY28_LED_GPIO_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x000F, 0x1c, (_val))
#define WRITE_PHYEGPHY28_LED_GPIO_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x000F, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_LED_GPIO_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x000F, 0x1c, \
                           (_val), (_mask))

/*  External SerDes Control Reg (ADDR 1ch shadow 10100) */
#define READ_PHYEGPHY28_EXT_SERDES_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0014, 0x1c, (_val))
#define WRITE_PHYEGPHY28_EXT_SERDES_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0014, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_EXT_SERDES_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0014, 0x1c, \
                           (_val), (_mask))

/*  SGMII SLAVE Reg (ADDR 1ch shadow 10101) */
#define READ_PHYEGPHY28_SGMII_SLAVEr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0015, 0x1c, (_val))
#define WRITE_PHYEGPHY28_SGMII_SLAVEr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0015, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_SGMII_SLAVEr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0015, 0x1c, \
                           (_val), (_mask))

/*  Misc. 1000BASE-X Control 2 Reg (ADDR 1ch shadow 10110) */
#define READ_PHYEGPHY28_MISC_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0016, 0x1c, (_val))
#define WRITE_PHYEGPHY28_MISC_1000X_CTRL2r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0016, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_MISC_1000X_CTRL2r(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0016, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-X Auto-Detect SGMII/Media Converter Reg (ADDR 1ch shadow 11000) */
#define READ_PHYEGPHY28_AUTO_DETECT_SGMII_MEDIAr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0018, 0x1c, (_val))
#define WRITE_PHYEGPHY28_AUTO_DETECT_SGMII_MEDIAr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0018, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_AUTO_DETECT_SGMII_MEDIAr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0018, 0x1c, \
                           (_val), (_mask))

/* Auxiliary Control Reg (ADDR 1ch shadow 11011) */
#define READ_PHYEGPHY28_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001B, 0x1c, (_val))
#define WRITE_PHYEGPHY28_AUX_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001B, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_AUX_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001B, 0x1c, \
                           (_val), (_mask))

/* Auxiliary 1000BASE-X Status Reg (ADDR 1ch shadow 11100) */
#define READ_PHYEGPHY28_AUX_1000X_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001C, 0x1c, (_val))
#define WRITE_PHYEGPHY28_AUX_1000X_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001C, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_AUX_1000X_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001C, 0x1c, \
                           (_val), (_mask))

/* Mode Control Reg (ADDR 1ch shadow 11111) */
#define READ_PHYEGPHY28_MODE_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x001F, 0x1c, (_val))
#define WRITE_PHYEGPHY28_MODE_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x001F, 0x1c, (_val))
#define MODIFY_PHYEGPHY28_MODE_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x001F, 0x1c, \
                           (_val), (_mask))

/* 1000BASE-T/100BASE-TX/10BASE-T Master/Slave Seed Reg (ADDR 1dh bit 15 = 0) */
/* 1000BASE-T/100BASE-TX/10BASE-T HDC Status Reg (ADDR 1dh bit 15 = 1) */

/* 1000BASE-T/100BASE-TX/10BASE-T Test Register 1 (ADDR 1eh) */
#define READ_PHYEGPHY28_TEST1r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, (_val))
#define WRITE_PHYEGPHY28_TEST1r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, (_val))
#define MODIFY_PHYEGPHY28_TEST1r(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0000, 0x1e, \
                           (_val), (_mask))

/*          +---------------------+
 *          |                     |
 *          |   SGMII Registers   |
 *          |                     |
 *          +---------------------+
 */
/* SGMII Control Register (Addr 00h) */
#define READ_PHYEGPHY28_SGMII_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x00, (_val))
#define WRITE_PHYEGPHY28_SGMII_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x00, (_val))
#define MODIFY_PHYEGPHY28_SGMII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x00, (_val), (_mask))

/* SGMII Status Register (Addr 01h) */
#define READ_PHYEGPHY28_SGMII_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x01, (_val))
#define WRITE_PHYEGPHY28_SGMII_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x01, (_val))
#define MODIFY_PHYEGPHY28_SGMII_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x01, (_val), (_mask))

/* SGMII Auto-neg Advertise Register (Addr 04h) */
#define READ_PHYEGPHY28_SGMII_ANAr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x04, (_val))
#define WRITE_PHYEGPHY28_SGMII_ANAr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x04, (_val))
#define MODIFY_PHYEGPHY28_SGMII_ANAr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x04, (_val), (_mask))

/* SGMII Auto-neg Link Partner Ability Register (Addr 05h) */
#define READ_PHYEGPHY28_SGMII_ANPr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x05, (_val))
#define WRITE_PHYEGPHY28_SGMII_ANPr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x05, (_val))
#define MODIFY_PHYEGPHY28_SGMII_ANPr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), SOC_PHY_REG_1000X, \
                             0x0000, 0x05, (_val), (_mask))

/*          +-------------------------+
 *          |                         |
 *          |   Expansion Registers   |
 *          |                         |
 *          +-------------------------+
 */
/* Receive/Transmit Packet Counter Register (Addr 00h) */
#define READ_PHYEGPHY28_EXP_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F00, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_PKT_COUNTERr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F00, \
                               0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_PKT_COUNTERr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F00, \
                                0x15, (_val), (_mask))

/* Multicolor LED Selector Register (Addr 04h) */
#define READ_PHYEGPHY28_EXP_LED_SELECTORr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F04, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_LED_SELECTORr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F04, \
                               0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_LED_SELECTORr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F04, \
                                0x15, (_val), (_mask))

/* Multicolor LED Flash Rate Controls Register (Addr 05h) */
#define READ_PHYEGPHY28_EXP_LED_FLASH_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F05, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_LED_FLASH_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F05, \
                               0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_LED_FLASH_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F05, \
                                0x15, (_val), (_mask))

/* Multicolor LED Programmable Blink Controls Register (Addr 06h) */
#define READ_PHYEGPHY28_EXP_LED_BLINK_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F06, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_LED_BLINK_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F06, \
                               0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_LED_BLINK_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F06, \
                                0x15, (_val), (_mask))

/* 100Base-FX Far End Fault egister (Addr 07h) */
#define READ_PHYEGPHY28_EXP_100FX_FAR_END_FAULTr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F07, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_100FX_FAR_END_FAULTr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F07, \
                               0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_100FX_FAR_END_FAULTr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F07, \
                                0x15, (_val), (_mask))

/* Port Interrupt Status Register (Addr 0bh) */
#define READ_PHYEGPHY28_EXP_INTERRUPT_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F0B, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_INTERRUPT_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F0B, \
                               0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_INTERRUPT_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F0B, \
                                0x15, (_val), (_mask))
/* Cable Diag Control Register (Addr 10h) */
#define READ_PHYEGPHY28_EXP_CABLE_DIAG_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F10, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_CABLE_DIAG_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F10, \
                               0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_CABLE_DIAG_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F10, \
                                0x15, (_val), (_mask))

/* Cable Diag Result Register (Addr 11h) */
#define READ_PHYEGPHY28_EXP_CABLE_DIAG_RESULTr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F11, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_CABLE_DIAG_RESULTr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F11, \
                               0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_CABLE_DIAG_RESULTr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F11, \
                                0x15, (_val), (_mask))

/* Cable Diag Length Register (Addr 12h) */
#define READ_PHYEGPHY28_EXP_CABLE_DIAG_LENGTHr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F12, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_CABLE_DIAG_LENGTHr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F12, \
                               0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_CABLE_DIAG_LENGTHr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F12, \
                                0x15, (_val), (_mask))


/* Operating Mode Status Register (Addr 42h) */
#define READ_PHYEGPHY28_EXP_OPT_MODE_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0F42, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_OPT_MODE_STATr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0F42, \
                               0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_OPT_MODE_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0F42, \
                                0x15, (_val), (_mask))

/* SGMII/Lineside Loopback Control Register (Addr 44h) */
#define READ_PHYEGPHY28_EXP_SGMII_LINESIDE_LOOPBACK_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f44, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_SGMII_LINESIDE_LOOPBACK_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f44, 0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_SGMII_LINESIDE_LOOPBACK_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f44, 0x15, \
                               (_val), (_mask))

/* SerDes/SGMII Control Register (Addr 52h) */
#define READ_PHYEGPHY28_EXP_SERDES_SGMII_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0f52, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_SERDES_SGMII_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0f52, 0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_SERDES_SGMII_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0f52, 0x15, \
                               (_val), (_mask))


/* EEE related */

/* Auto EEE MII_Buffer CNTL/Status 2 Register (Addr AEh) */
#define READ_PHYEGPHY28_EXP_MII_BUF_CNTL_STAT2r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0FAE, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_MII_BUF_CNTL_STAT2r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0FAE, \
                               0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_MII_BUF_CNTL_STAT2r(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0FAE, \
                                0x15, (_val), (_mask))

/* Auto EEE MII_Buffer CNTL/Status 1 Register (Addr AFh) */
#define READ_PHYEGPHY28_EXP_MII_BUF_CNTL_STAT1r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0FAF, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_MII_BUF_CNTL_STAT1r(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0FAF, \
                               0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_MII_BUF_CNTL_STAT1r(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0FAF, \
                                0x15, (_val), (_mask))

/* EEE Statistics related */
/* Local LPI request Counter Reg (Addr ach) */
#define READ_PHYEGPHY28_EXP_EEE_TX_EVENTSr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0FAC, 0x15, (_val))

/* Remote LPI request Counter Reg (Addr adh) */
#define READ_PHYEGPHY28_EXP_EEE_RX_EVENTSr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0FAD, 0x15, (_val))

/* Statistic Timer_12_hours_lpi remote Reg (Addr aah) */
#define READ_PHYEGPHY28_EXP_EEE_TX_DURATIONr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0FAA, 0x15, (_val))

/* Statistic Timer_12_hours_lpi_local Local Reg (Addr abh) */
#define READ_PHYEGPHY28_EXP_EEE_RX_DURATIONr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0FAB, 0x15, (_val))

/* IEEE 1588 read start/end control Register (Addr f3h) */
#define READ_PHYEGPHY28_EXP_READ_START_END_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_READ((_unit), (_phy_ctrl), 0x00, 0x0ff3, 0x15, (_val))
#define WRITE_PHYEGPHY28_EXP_READ_START_END_CTRLr(_unit, _phy_ctrl, _val) \
            PHYEGPHY28_REG_WRITE((_unit), (_phy_ctrl), 0x00, 0x0ff3, 0x15, (_val))
#define MODIFY_PHYEGPHY28_EXP_READ_START_END_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHYEGPHY28_REG_MODIFY((_unit), (_phy_ctrl), 0x00, 0x0ff3, 0x15, \
                               (_val), (_mask))

/* EEE Capability Register */
#define READ_PHYEGPHY28_EEE_CAPr(_unit, _phy_ctrl, _val) \
        PHYEGPHY28_CL45_REG_READ(_unit, _phy_ctrl, 0x00, 0x3, 0x14, _val)

/* EEE Advertisement Register */
#define READ_PHYEGPHY28_EEE_ADVr(_unit, _phy_ctrl, _val) \
        PHYEGPHY28_CL45_REG_READ(_unit, _phy_ctrl, 0x00, 0x7, 0x3c, _val)
#define WRITE_PHYEGPHY28_EEE_ADVr(_unit, _phy_ctrl, _val) \
        PHYEGPHY28_CL45_REG_WRITE(_unit, _phy_ctrl, 0x00, 0x7, 0x3c, _val)
#define MODIFY_PHYEGPHY28_EEE_ADVr(_unit, _phy_ctrl, _val, _mask) \
        PHYEGPHY28_CL45_REG_MODIFY(_unit, _phy_ctrl, 0x00, 0x7, 0x3c, _val, _mask)

#define READ_PHYEGPHY28_EEE_803Dr(_unit, _phy_ctrl, _val) \
        PHYEGPHY28_CL45_REG_READ(_unit, _phy_ctrl, 0x00, 0x7, 0x803d, _val)
#define WRITE_PHYEGPHY28_EEE_803Dr(_unit, _phy_ctrl, _val) \
        PHYEGPHY28_CL45_REG_WRITE(_unit, _phy_ctrl, 0x00, 0x7, 0x803d, _val)
#define MODIFY_PHYEGPHY28_EEE_803Dr(_unit, _phy_ctrl, _val, _mask) \
        PHYEGPHY28_CL45_REG_MODIFY(_unit, _phy_ctrl, 0x00, 0x7, 0x803d, _val, _mask)

/* EEE Resolution Status Register */
#define READ_PHYEGPHY28_EEE_RESOLUTION_STATr(_unit, _phy_ctrl, _val) \
        PHYEGPHY28_CL45_REG_READ(_unit, _phy_ctrl, 0x00, 0x7, 0x803e, _val)
#define WRITE_PHYEGPHY28_EEE_RESOLUTION_STATr(_unit, _phy_ctrl, _val) \
        PHYEGPHY28_CL45_REG_WRITE(_unit, _phy_ctrl, 0x00, 0x7, 0x803e, _val)
#define MODIFY_PHYEGPHY28_EEE_RESOLUTION_STATr(_unit, _phy_ctrl, _val, _mask) \
        PHYEGPHY28_CL45_REG_MODIFY(_unit, _phy_ctrl, 0x00, 0x7, 0x803e, _val, _mask)

#define READ_PHYEGPHY28_EEE_TEST_CTRL_Ar(_unit, _phy_ctrl, _val) \
        PHYEGPHY28_CL45_REG_READ(_unit, _phy_ctrl, 0x00, 0x7, 0x8030, _val)
#define WRITE_PHYEGPHY28_EEE_TEST_CTRL_Ar(_unit, _phy_ctrl, _val) \
        PHYEGPHY28_CL45_REG_WRITE(_unit, _phy_ctrl, 0x00, 0x7, 0x8030, _val)
#define MODIFY_PHYEGPHY28_EEE_TEST_CTRL_Ar(_unit, _phy_ctrl, _val, _mask) \
        PHYEGPHY28_CL45_REG_MODIFY(_unit, _phy_ctrl, 0x00, 0x7, 0x8030, _val, _mask)

/* EEE TEST Control Register B*/
#define READ_PHYEGPHY28_EEE_TEST_CTRL_Br(_unit, _phy_ctrl, _val) \
        PHYEGPHY28_CL45_REG_READ(_unit, _phy_ctrl, 0x00, 0x7, 0x8031, _val)
#define WRITE_PHYEGPHY28_EEE_TEST_CTRL_Br(_unit, _phy_ctrl, _val) \
        PHYEGPHY28_CL45_REG_WRITE(_unit, _phy_ctrl, 0x00, 0x7, 0x8031, _val)
#define MODIFY_PHYEGPHY28_EEE_TEST_CTRL_Br(_unit, _phy_ctrl, _val, _mask) \
        PHYEGPHY28_CL45_REG_MODIFY(_unit, _phy_ctrl, 0x00, 0x7, 0x8031, _val, _mask)

typedef struct {
    uint32 flags;
    uint16 phy_id_orig;
    uint16 phy_id_base; /* port 0 addr */
    uint16 phy_slice;
} PHYEGPHY28_DEV_DESC_t;

#define PHYEGPHY28_DEV_PHY_ID_ORIG(_pc) (((PHYEGPHY28_DEV_DESC_t *)((_pc) + 1))->phy_id_orig)
#define PHYEGPHY28_DEV_PHY_ID_BASE(_pc) (((PHYEGPHY28_DEV_DESC_t *)((_pc) + 1))->phy_id_base)
#define PHYEGPHY28_DEV_PHY_SLICE(_pc) (((PHYEGPHY28_DEV_DESC_t *)((_pc) + 1))->phy_slice)
#define PHYEGPHY28_FLAGS(_pc) (((PHYEGPHY28_DEV_DESC_t *)((_pc) + 1))->flags)
#define PHYEGPHY28_PHYA_REV (1U << 0)

#define PHYEGPHY28_DEV_SET_BASE_ADDR(_pc) \
        PHYEGPHY28_DEV_PHY_ID_BASE(_pc) =  (PHYEGPHY28_FLAGS((_pc)) & PHYEGPHY28_PHYA_REV) ?\
                                             PHYEGPHY28_DEV_PHY_ID_ORIG(_pc) + PHYEGPHY28_DEV_PHY_SLICE(_pc) : \
                                             PHYEGPHY28_DEV_PHY_ID_ORIG(_pc) - PHYEGPHY28_DEV_PHY_SLICE(_pc)

#define PHYEGPHY28_SLICE_ADDR(_pc, _slice) \
            (PHYEGPHY28_FLAGS((_pc)) & PHYEGPHY28_PHYA_REV) ? (PHYEGPHY28_DEV_PHY_ID_BASE(_pc) - (_slice)) : \
            (PHYEGPHY28_DEV_PHY_ID_BASE(_pc) + (_slice))

#endif /* _PHYEGPHY28_H_ */
