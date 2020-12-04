/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       portphy.h
 */

#ifndef _PORTPHY_H_
#define _PORTPHY_H_

/* control commands used by firmware_set for ucode broadcast. Internal use only */
/* Command should be executed once. Assume all devices in bcst mode */
#define PHYCTRL_UCODE_BCST_ONEDEV  0x10000

/* Command is executed on each device. Assume all devices in non-bcst mode */
#define PHYCTRL_UCODE_BCST_ALLDEV  0x20000

/* setup the broadcast mode for firmware download */
#define PHYCTRL_UCODE_BCST_SETUP   (0 | PHYCTRL_UCODE_BCST_ALLDEV)

/* Prepare micro-controller ready for firmware broadcast */
#define PHYCTRL_UCODE_BCST_uC_SETUP  (1 | PHYCTRL_UCODE_BCST_ONEDEV)

/* Re-Enable MDIO broadcast mode */
#define PHYCTRL_UCODE_BCST_ENABLE  (2 | PHYCTRL_UCODE_BCST_ALLDEV)

/* Load firmware */
#define PHYCTRL_UCODE_BCST_LOAD    (3 | PHYCTRL_UCODE_BCST_ONEDEV)

/* Load FC firmware */
#define PHYCTRL_UCODE_BCST_LOAD2   (4 | PHYCTRL_UCODE_BCST_ONEDEV)

/* Verify firmware download */
#define PHYCTRL_UCODE_BCST_END     (5 | PHYCTRL_UCODE_BCST_ALLDEV)

#define _SHR_PORT_PHY_CLAUSE45_ADDR(_devad, _regad)     \
            ((((_devad) & 0x3F) << 16) |                \
             ((_regad) & 0xFFFF))
#define SOC_PHY_CLAUSE45_ADDR(_devad, _regad) \
            _SHR_PORT_PHY_CLAUSE45_ADDR(_devad, _regad)

#define PHY_C45_DEV_PMA_PMD     0x01
#define MII_PHY_ID0_REG         0x02    /* MII PHY ID register: r/w */
#define MII_PHY_ID1_REG         0x03    /* MII PHY ID register: r/w */

#endif /* _PORTPHY_H_ */
