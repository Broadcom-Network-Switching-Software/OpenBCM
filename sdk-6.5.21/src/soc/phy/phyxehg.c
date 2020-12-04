/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phyxehg.c
 * Purpose:     Defines common PHY driver routines for Broadcom 10G PHYs.
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_XEHG)
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */

#include <sal/types.h>
#include <sal/core/spl.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/ll.h>
#include <soc/phy/phyctrl.h>
#include "phyreg.h"
#include "phyxehg.h"

int
phy_xehg_speed_set(int unit, soc_port_t port, int speed)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (speed != 10000) {
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

int
phy_xehg_speed_get(int unit, soc_port_t port, int *speed)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *speed = 10000;
   
    return SOC_E_NONE;
}

int
phy_xehg_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_XAUI;

    return SOC_E_NONE;
}

int
phy_xehg_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *mode = (SOC_PM_10GB_FD | SOC_PM_XGMII | SOC_PM_LB_PHY |
             SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM);

    return SOC_E_NONE;
}
#else /* INCLUDE_PHY_XEHG */
typedef int _phy_xehg_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_XEHG */

