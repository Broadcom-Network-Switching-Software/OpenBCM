/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        xgxs.c
 * Purpose:     Defines common PHY driver routines for Broadcom XGXS core.
 */

#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_PHY_XGXS)
#include "phyconfig.h"    /* Must be the first phy include after phydefs.h */
#include <sal/types.h>
#include <sal/core/spl.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>


#include "phyreg.h"
#include "xgxs.h"

/*
 *      phy_xgxs_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
int
phy_xgxs_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t           *pc;      /* PHY software state */

    COMPILER_REFERENCE(flags);

    pc   = INT_PHY_SW_STATE(unit, port);

    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    SOC_IF_ERROR_RETURN
        (phy_reg_xgxs_read(unit, pc, reg_bank, reg_addr, &data));

   *phy_data = data;

    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_xgxs_reg_write
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - Value write to PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
int
phy_xgxs_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t           *pc;      /* PHY software state */

    COMPILER_REFERENCE(flags);

    pc   = INT_PHY_SW_STATE(unit, port);
    data = (uint16) (phy_data & 0x0000FFFF);

    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    return phy_reg_xgxs_write(unit, pc, reg_bank, reg_addr, data);
} 
/*
 * Function:
 *      phy_xgxs_reg_modify
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_mo_data  - New value for the bits specified in phy_mo_mask
 *      phy_mo_mask  - Bit mask to modify
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
int
phy_xgxs_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask)
{
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;     /* Temporary holder for phy_data_mask */
    phy_ctrl_t           *pc;      /* PHY software state */

    COMPILER_REFERENCE(flags);

    pc   = INT_PHY_SW_STATE(unit, port);
    data = (uint16) (phy_data & 0x0000FFFF);
    mask = (uint16) (phy_data_mask & 0x0000FFFF);

    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    return phy_reg_xgxs_modify(unit, pc, reg_bank, reg_addr, data, mask);
}

#else /* INCLUDE_PHY_XGXS */
typedef int _phy_xgxs_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_XGXS */

