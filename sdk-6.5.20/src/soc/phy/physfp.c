/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    physfp.c
 * Purpose: Provide PHY driver support for the PHY device embedded in the 
 *          copper SFP module. It is assumed that PHY device's register 
 *          implementation conforms to IEEE 802.3 MII register specification. 
 *          Device features implemented in the vendor specific registers generally
 *          will not be supported.
 *          This driver has tested with Broadcom 54810S copper SFP and 
 *          Finisars 1000BASE-T SFP (FCxx-8520/1-3).
 */

#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/phy.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "phyfege.h"


#define PHY_BRCM_54810S_ID0   0x0362 
#define PHY_BRCM_54810S_ID1   0x5d01
#define PHY_MRVL_88E1111_ID0  0x0141
#define PHY_MRVL_88E1111_ID1  0x0cc1

STATIC int
phy_copper_sfp_an_set(int unit, soc_port_t port, int an);
STATIC int
phy_copper_sfp_init(int unit, soc_port_t port);
STATIC int
phy_copper_sfp_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode);
STATIC int
phy_copper_sfp_enable_set(int unit, soc_port_t port, int enable);
STATIC int
phy_copper_sfp_speed_set(int unit, soc_port_t port, int speed);


/*
 * Variable:
 *  phy_drv_copper_sfp
 * Purpose:
 *  Phy driver callouts for a Gigabit phy (IEEE 802.3)
 */
phy_driver_t phy_copper_sfp_drv = {
    "Copper SFP PHY Driver",
    phy_copper_sfp_init,
    phy_fe_ge_reset,
    phy_fe_ge_link_get,
    phy_copper_sfp_enable_set,
    phy_fe_ge_enable_get,
    phy_fe_ge_duplex_set,
    phy_fe_ge_duplex_get,
    phy_copper_sfp_speed_set,
    phy_fe_ge_speed_get,
    phy_fe_ge_master_set,
    phy_fe_ge_master_get,
    phy_copper_sfp_an_set,
    phy_fe_ge_an_get,
    phy_ge_adv_local_set,
    phy_ge_adv_local_get,
    phy_ge_adv_remote_get,
    phy_fe_ge_lb_set,
    phy_fe_ge_lb_get,
    phy_ge_interface_set,
    phy_ge_interface_get,
    phy_copper_sfp_ability_get,
    NULL,
    NULL,
    phy_null_mdix_set,
    phy_null_mdix_get,
    phy_null_mdix_status_get,
    NULL,
    NULL,
    phy_fe_ge_medium_get,
    NULL,                       /* phy_cable_diag  */
    NULL,                       /* phy_link_change */
    NULL,                       /* .pd_control_set */ 
    NULL,                       /* .pd_control_get */ 
    NULL,                       /* .pd_reg_read    */
    NULL,                       /* .pd_reg_write   */
    NULL,                       /* .pd_reg_modify  */
    NULL,                       /* .pd_notify      */
    NULL                        /* .pd_probe       */ 
};

 
/*
 * Function:    
 *  phy_finisar_sfp_init
 * Purpose: 
 *  Initialize the PHY (MII mode) to a known good state.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 * Returns: 
 *  SOC_E_XXX
 */

STATIC int
phy_finisar_sfp_init(int unit, soc_port_t port)
{
    phy_ctrl_t          *pc;
    pc = EXT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_finisar_sfp_init : u=%d p=%d\n"),
              unit, port));

    if (PHY_SGMII_AUTONEG_MODE(unit, port)) {
        /* if it is in SGMII mode */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit,pc,0x1B,0x9084));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit,pc,0x9,0x0f00));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit,pc,0x0,0x8140));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit,pc,0x4,0x0de1));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit,pc,0x0,0x9140));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit,pc,0x1B,0x9088));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit,pc,0x9,0x0e00));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit,pc,0x4,0x0c01));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit,pc,0x0,0x8140));
    }

    return(SOC_E_NONE);
}

/*
 * Function:
 *  phy_copper_sfp_init
 * Purpose:
 *  Initialize the PHY (MII mode) to a known good state.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #.
 * Returns:
 *  SOC_E_XXX

 * Notes:
 *  No synchronization performed at this level.
 */

STATIC int
phy_copper_sfp_init(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);

    if ((pc->phy_id0 == PHY_MRVL_88E1111_ID0) && 
        (pc->phy_id1 == PHY_MRVL_88E1111_ID1) ) {
         rv = phy_finisar_sfp_init(unit,port);
    } else if ((pc->phy_id0 == PHY_BRCM_54810S_ID0) && 
               (pc->phy_id1 == PHY_BRCM_54810S_ID1) ) {
         rv = phy_ge_init(unit,port);
         
         /* disable the broadreach mode for now */
         SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit,pc,0x17,0xf90));
         SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit,pc,0x15,0));
                  
    } else {
         rv = phy_ge_init(unit,port);
    }
    return rv;         
}

/*
 * Function:
 *  phy_copper_sfp_an_set
 * Purpose:
 *  Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #.
 *  an   - Boolean, if true, auto-negotiation is enabled
 *      (and/or restarted). If false, autonegotiation is disabled.
 * Returns:
 *  SOC_E_XXX
 */

STATIC int
phy_copper_sfp_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    int rv;

    pc = EXT_PHY_SW_STATE(unit, port);

    rv = phy_fe_ge_an_set(unit,port,an);
    if ((pc->phy_id0 == PHY_MRVL_88E1111_ID0) &&
        (pc->phy_id1 == PHY_MRVL_88E1111_ID1) ) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHYFEGE_MII_CTRLr(unit, pc, MII_CTRL_RESET,MII_CTRL_RESET));
    } else if ((pc->phy_id0 == PHY_BRCM_54810S_ID0) &&
               (pc->phy_id1 == PHY_BRCM_54810S_ID1) ) {
    } else {
    }
    return rv;
}

/*
 * Function:
 *  phy_ge_ability_get
 * Purpose:
 *  Get the abilities of the local gigabit phy.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #.
 *  mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *  SOC_E_XXX
 */

STATIC int
phy_copper_sfp_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    uint16  mii_esr;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
    (phy_fe_ability_get(unit, port, mode));

    *mode |= (SOC_PM_PAUSE | SOC_PM_PAUSE_ASYMM | SOC_PM_GMII | SOC_PM_SGMII);

    SOC_IF_ERROR_RETURN
        (READ_PHYFEGE_MII_ESRr(unit, pc, &mii_esr));

#if 0
    

    if (mii_esr & MII_ESR_1000_T_FD)    *mode |= SOC_PM_1000MB_FD;
    if (mii_esr & MII_ESR_1000_T_HD)    *mode |= SOC_PM_1000MB_HD;
    if (mii_esr & MII_ESR_1000_X_FD)    *mode |= SOC_PM_1000MB_FD;
    if (mii_esr & MII_ESR_1000_X_HD)    *mode |= SOC_PM_1000MB_HD;
#else
    *mode |= SOC_PM_1000MB_HD | SOC_PM_1000MB_FD;
#endif

    return(SOC_E_NONE);
}

/*
 * Function:
 *  phy_copper_sfp_enable_set
 * Purpose:
 *  Enable or disable the physical interface.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #.
 *  enable - Boolean, true = enable PHY, false = disable.
 * Returns:
 *  SOC_E_XXX
 */

STATIC int
phy_copper_sfp_enable_set(int unit, soc_port_t port, int enable)
{
    int value;
    phy_ctrl_t          *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    value = enable? 0: MII_CTRL_PD;
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYFEGE_MII_CTRLr(unit, pc,value,MII_CTRL_PD));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_copper_sfp_enable_set: u=%d p=%d value=%d\n"),
              unit, port,value));
    return (SOC_E_NONE);
}

/*
 * Function:
 *  phy_copper_sfp_speed_set
 * Purpose:
 *  Set the current operating speed (forced).
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #.
 *  duplex - (OUT) Boolean, true indicates full duplex, false
 *      indicates half.
 * Returns:
 *  SOC_E_XXX
 */

STATIC int
phy_copper_sfp_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t          *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_fe_ge_speed_set(unit,port,speed));

    if ((pc->phy_id0 == PHY_MRVL_88E1111_ID0) &&
        (pc->phy_id1 == PHY_MRVL_88E1111_ID1) ) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHYFEGE_MII_CTRLr(unit, pc, MII_CTRL_RESET,MII_CTRL_RESET));
    }
    return SOC_E_NONE;
}    
