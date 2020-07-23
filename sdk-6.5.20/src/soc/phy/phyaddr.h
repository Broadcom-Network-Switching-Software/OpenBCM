/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phyaddr.h
 * Purpose:     Basic defines for PHY configuration
 */

#ifndef _PHY_ADDR_H
#define _PHY_ADDR_H

#define _XGS12_FABRIC_PHY_ADDR_DEFAULT(_unit, _port, _phy_addr, _phy_addr_int) \
        do {                            \
            *(_phy_addr)     = (_port); \
            *(_phy_addr_int) = (_port); \
        } while (0)


#define _DRACO_PHY_ADDR_DEFAULT(_unit, _port, _phy_addr, _phy_addr_int) \
        do {                                           \
            if (IS_GE_PORT((_unit), (_port))) {        \
                *(_phy_addr) = (_port) + 1;            \
                *(_phy_addr_int) = (_port) + 0x80;     \
            } else {                                   \
                *(_phy_addr) = (_port) + 0x80;         \
                *(_phy_addr_int) = (_port) + 0x80;     \
            }                                          \
        } while (0)

#define _LYNX_PHY_ADDR_DEFAULT(_unit, _port, _phy_addr, _phy_addr_int) \
        do {                                           \
            *(_phy_addr) = (_port) + 1;                \
            *(_phy_addr_int) = (_port) + 0x80;         \
        } while (0)

#define _RAPTOR_PHY_ADDR_DEFAULT(_unit, _port, _phy_addr, _phy_addr_int)    \
        do {                                                                \
           /*                                                               \
            * MDIO address and MDIO bus select for External Phy             \
            * port 0  - 0  CPU (_port) no external MDIO address             \
            * port 1  - 2  MDIO address 2, 3 (0x2 - 0x3)                    \
            * port 3  - 3  No internal MDIO                                 \
            * port 4  - 5  MDIO address 4, 6 (0x4 - 0x6)                    \
            * port 6  - 29 MDIO address 7,30 (0x7 - 0x1e)                   \
            * port 30 - 53 MDIO address 1,24 (0x41 - 0x58 with Bus sel set) \
            */                                                              \
            *(_phy_addr) = (_port) +                                        \
                           (((_port) > 29) ? (0x40 - 29) : (1));            \
          /*                                                                \
           * MDIO address and MDIO bus select for Internal Phy (Serdes)     \
           * port 0  - 0  CPU (_port) no internal MDIO address              \
           * port 1  - 2  MDIO address 1,2 (0x81 - 0x82 with Intenal sel set) \
           * port 3  - 3  No internal MDIO                                    \
           * port 4  - 5  MDIO address 4, 5 (0x84 - 0x85 with Intenal sel set)\
           * port 6  - 29 MDIO address 6,29 (0x86 - 0x9D with Intenal sel set)\
           * port 30 - 53 MDIO address 0,23 (0xc0 - 0xd7 with Intenal sel set)\
           */                                                               \
            *(_phy_addr_int) = (_port) +                                    \
                               (((_port) > 29) ? (0xc0 - 30) : (0x80));     \
         } while (0)

#define _TUCANA_PHY_ADDR_DEFAULT(_unit, _port, _phy_addr, _phy_addr_int) \
        do {                                           \
            if (IS_HG_PORT(unit, (_port))) {           \
                *(_phy_addr) = (_port) + 0x80;         \
                *(_phy_addr_int) = (_port) + 0x80;     \
            } else {                                   \
                *(_phy_addr) = (_port) + 1;            \
                *(_phy_addr_int) = 0xff;               \
            }                                          \
         } while (0)

#define _STRATA_PHY_ADDR_DEFAULT(_unit, _port, _phy_addr, _phy_addr_int) \
        do {                                           \
            *(_phy_addr) = (_port) + 1;                \
            *(_phy_addr_int) = 0xff;                   \
        } while (0)

#endif /* _PHY_ADDR_H */
