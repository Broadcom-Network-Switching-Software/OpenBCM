/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy8703.h
 */

#ifndef   _PHY8703_H_
#define   _PHY8703_H_

#include <soc/phy.h>

#define MII_CTRL_PMA_LOOPBACK      (1 << 0)

#define PHY8703_C45_DEV_PMA_PMD     0x01
#define PHY8703_C45_DEV_PCS         0x03
#define PHY8703_C45_DEV_PHYXS       0x04
#define PHY8703_C45_DEV_DTEXS       0x05

#define PHY8703_REG_READ(_unit, _phy_ctrl, _addr, _val) \
            READ_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8703_REG_WRITE(_unit, _phy_ctrl, _addr, _val) \
            WRITE_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val))
#define PHY8703_REG_MODIFY(_unit, _phy_ctrl, _addr, _val, _mask) \
            MODIFY_PHY_REG((_unit), (_phy_ctrl), (_addr), (_val), (_mask))

/* PMA/PMD Device (Dev Addr 1) */

/* Control Register (Addr 0000h) */
#define READ_PHY8703_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 0), (_val))
#define WRITE_PHY8703_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 0), (_val))
#define MODIFY_PHY8703_PMA_PMD_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8703_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 0), (_val), (_mask))

/* Status Register (Addr 0001h) */
#define READ_PHY8703_PMA_PMD_STATr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 1), (_val))
#define WRITE_PHY8703_PMA_PMD_STATr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 1), (_val))
#define MODIFY_PHY8703_PMA_PMD_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8703_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 1), (_val), (_mask))

/* ID0 Register (Addr 0002h) */
/* ID1 Register (Addr 0003h) */
/* Speed Ability Register (Addr 0004h) */
/* Devices in Package 1 Register (Addr 0005h) */
/* Devices in Package 2 Register (Addr 0006h) */
/* Control 2 Register (Addr 0007h) */
/* Status 2 Register (Addr 0008h) */

/* Transmit Disable Register (Addr 0009h) */
#define READ_PHY8703_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 9), (_val))
#define WRITE_PHY8703_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 9), (_val))
#define MODIFY_PHY8703_PMA_PMD_TX_DISABLEr(_unit, _phy_ctrl, _val, _mask) \
            PHY8703_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 9), (_val), (_mask))


/* Receive Signal Detect Register (Addr 000Ah) */
/* Organizationlly Unique Identifier 0 Register (Addr 000Eh) */
/* Organizationlly Unique Identifier 1 Register (Addr 000Fh) */

/* WIS Device (Dev Addr 2) */
/* Control 1 Register (Addr 0000h) */
#define READ_PHY8703_WIS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_WIS, 0), (_val))
#define WRITE_PHY8703_WIS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_WIS, 0), (_val))
#define MODIFY_PHY8703_WIS_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8703_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_WIS, 0), (_val), (_mask))

/* Status 1 Register (Addr 0001h) */
#define READ_PHY8703_WIS_STATr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_WIS, 1), (_val))
#define WRITE_PHY8703_WIS_STATr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_WIS, 1), (_val))
#define MODIFY_PHY8703_WIS_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8703_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_WIS, 1), (_val), (_mask))

/* ID0 Register (Addr 0002h) */
/* ID1 Register (Addr 0003h) */
/* Speed Ability Register (Addr 0004h) */

/* Devices in Package 1 Register (Addr 0005h) */
#define READ_PHY8703_WIS_DEV_IN_PKGr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_WIS, 5), (_val))
#define WRITE_PHY8703_WIS_DEV_IN_PKGr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_WIS, 5), (_val))
#define MODIFY_PHY8703_WIS_DEV_IN_PKGr(_unit, _phy_ctrl, _val, _mask) \
            PHY8703_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_WIS, 5), (_val), (_mask))

/* Devices in Package 2 Register (Addr 0006h) */
/* Control 2 Register (Addr 0007h) */
/* Status 2 Register (Addr 0008h) */
/* Test Pattern Error Counter (Addr 0009h) */
/* Package Identifier 0 Register (Addr 000Eh) */
/* Package Identifier 1 Register (Addr 000Fh) */
/* Status 3 Register (Addr 0021h) */
/* Far End Path Block Error Counter (Addr 0025h) */

/* PCS Device (Dev Addr 3) */
/* Control 1 Register (Addr 0000h) */
#define READ_PHY8703_PCS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PCS, 0), (_val))
#define WRITE_PHY8703_PCS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PCS, 0), (_val))
#define MODIFY_PHY8703_PCS_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8703_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PCS, 0), (_val), (_mask))

/* Status 1 Register (Addr 0001h) */
#define READ_PHY8703_PCS_STATr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PCS, 1), (_val))
#define WRITE_PHY8703_PCS_STATr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PCS, 1), (_val))
#define MODIFY_PHY8703_PCS_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8703_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PCS, 1), (_val), (_mask))

/* XGXS Device (Dev Addr 4) */
/* Control 1 Register (Addr 0000h) */
#define READ_PHY8703_PHYXS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PHYXS, 0), (_val))
#define WRITE_PHY8703_PHYXS_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PHYXS, 0), (_val))
#define MODIFY_PHY8703_PHYXS_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8703_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PHYXS, 0), (_val), (_mask))

/* Status 1 Register (Addr 0001h) */
#define READ_PHY8703_PHYXS_STATr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PHYXS, 1), (_val))
#define WRITE_PHY8703_PHYXS_STATr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PHYXS, 1), (_val))
#define MODIFY_PHY8703_PHYXS_STATr(_unit, _phy_ctrl, _val, _mask) \
            PHY8703_REG_MODIFY((_unit), (_phy_ctrl), \
             SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PHYXS, 1), (_val), (_mask))


/* User Defined Registers */

#define READ_PHY8703_IDENTIFIERr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 0xc800), (_val))
#define WRITE_PHY8703_IDENTIFIERr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 0xc800), (_val))
#define MODIFY_PHY8703_IDENTIFIERr(_unit, _phy_ctrl, _val, _mask) \
            PHY8703_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 0xc800), \
              (_val), (_mask))

#define READ_PHY8703_PMD_TX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_READ((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 0xc803), (_val))
#define WRITE_PHY8703_PMD_TX_CTRLr(_unit, _phy_ctrl, _val) \
            PHY8703_REG_WRITE((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 0xc803), (_val))
#define MODIFY_PHY8703_PMD_TX_CTRLr(_unit, _phy_ctrl, _val, _mask) \
            PHY8703_REG_MODIFY((_unit), (_phy_ctrl), \
              SOC_PHY_CLAUSE45_ADDR(PHY8703_C45_DEV_PMA_PMD, 0xc803), \
              (_val), (_mask))

#endif  /* _phy8703_H_ */
