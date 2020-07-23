/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:     null.c
 * Purpose:    Defines NULL PHY driver routines
 */

#include <sal/types.h>
#include <sal/core/thread.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#ifdef PORTMOD_SUPPORT
#include "phynull.h"
#include "phy_portmod_dispatch.h"
#include "phyfege.h"
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>

/* typedef unsigned int portmod_port_mode_t; */
/* typedef unsigned int portmod_pa_encap_t; */
/* typedef int portmod_port_t */


/*
 * Functions:
 *    phy_portmod_dispatch_XXX
 *
 * Purpose:    
 *    Some dummy routines for null and no
 *      connection PHY drivers.
 */

int
_phy_portmod_dispatch_port_ability_init(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{

    int max_speed = SOC_INFO(unit).port_speed_max[port];

    if ( max_speed == 0) {
        if (IS_GE_PORT(unit,port)) {
             max_speed = 1000;
        } else if (IS_FE_PORT(unit,port)) {
             max_speed = 100;
        }
    }

    switch (max_speed) {
    case 127000:
        ability->speed_full_duplex |= SOC_PA_SPEED_127GB;
        /* fall through */
    case 120000:
        ability->speed_full_duplex |= SOC_PA_SPEED_120GB;
        /* fall through */
    case 106000:
        ability->speed_full_duplex |= SOC_PA_SPEED_106GB;
        /* fall through */
    case 100000:
        ability->speed_full_duplex |= SOC_PA_SPEED_100GB;
        /* fall through */
    case 53000:
        ability->speed_full_duplex |= SOC_PA_SPEED_53GB;
        /* fall through */
    case 50000:
        ability->speed_full_duplex |= SOC_PA_SPEED_50GB;
        /* fall through */
    case 42000:
        ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
        /* fall through */
    case 40000:
        ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
        /* fall through */
    case 32000:
        ability->speed_full_duplex |= SOC_PA_SPEED_32GB;
        /* fall through */
    case 30000:
        ability->speed_full_duplex |= SOC_PA_SPEED_30GB;
        /* fall through */
    case 27000:
        ability->speed_full_duplex |= SOC_PA_SPEED_27GB;
        /* fall through */
    case 25000:
        ability->speed_full_duplex |= SOC_PA_SPEED_25GB;
        /* fall through */
    case 24000:
        ability->speed_full_duplex |= SOC_PA_SPEED_24GB;
        /* fall through */
    case 21000:
        ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
        /* fall through */
    case 20000:
        ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
        /* fall through */
    case 16000:
        ability->speed_full_duplex |= SOC_PA_SPEED_16GB;
        /* fall through */
    case 15000:
        ability->speed_full_duplex |= SOC_PA_SPEED_15GB;
        /* fall through */
    case 13000:
        ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
        /* fall through */
    case 12500:
        ability->speed_full_duplex |= SOC_PA_SPEED_12P5GB;
        /* fall through */
    case 12000:
        ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
        /* fall through */
    case 11000:
        ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
        /* fall through */
    case 10000:
        ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
        /* fall through */
    case 6000:
        ability->speed_full_duplex |= SOC_PA_SPEED_6000MB;
        /* fall through */
    case 5000:
        ability->speed_full_duplex |= SOC_PA_SPEED_5000MB;
        /* fall through */
    case 3000:
        ability->speed_full_duplex |= SOC_PA_SPEED_3000MB;
        /* fall through */
    case 2500:
        ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
        /* fall through */
    case 1000:
        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
        /* fall through */
    case 100:
        ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
        /* fall through */
    case 10:
        ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
        /* fall through */
    default:
        break;
    }

    ability->speed_half_duplex =
        SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB; 

    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port) ||
        IS_CE_PORT(unit, port)) {
        ability->interface = SOC_PA_INTF_XGMII;
    } else if (IS_GE_PORT(unit, port)) {
        ability->interface = SOC_PA_INTF_MII | SOC_PA_INTF_GMII;
    } else {
        ability->interface = SOC_PA_INTF_MII;
    }

    return SOC_E_NONE;
}

int
phy_portmod_dispatch_init(int unit, soc_port_t port)
{
    return SOC_E_NONE;
}

int
phy_portmod_dispatch_reset(int unit, soc_port_t port, void *user_arg)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(user_arg);

    return SOC_E_NONE;
}

int
phy_portmod_dispatch_set(int unit, soc_port_t port, int parm)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(parm);

    return SOC_E_NONE;
}

int
phy_portmod_dispatch_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    int rv = SOC_E_NONE;
    phymod_autoneg_control_t  an_ctrl;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    phymod_autoneg_control_t_init(&an_ctrl);
    an_ctrl.enable = 0;
    an_ctrl.num_lane_adv = si->port_num_lanes[port];
    an_ctrl.an_mode = phymod_AN_MODE_NONE;
    an_ctrl.flags   = 0 ;

    /* only SGMIII AN is supported when there is an external PHY */
    if (IS_GE_PORT(unit,port))
        an_ctrl.an_mode = phymod_AN_MODE_CL37;

    rv = portmod_ext_to_int_cmd_get(unit, port, portmodExtToInt_CtrlCode_AN, &an_ctrl);
    if (rv != SOC_E_NONE) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "phy_portmod_dispatch_an_get: portmod_ext_to_int_cmd_get failed")));
        return rv;
    }

    *an = an_ctrl.enable;
    /* Need to fix  this as the portmod curently does 
    not return an_done state */
    *an_done = 0;
    if(an_ctrl.enable)
        *an_done = 1;

    return rv;
}

int
phy_portmod_dispatch_an_set(int unit, soc_port_t port, int an_enable) {
    int rv = SOC_E_NONE;
    phymod_autoneg_control_t  an;
    soc_info_t *si;
    int fiber_pref;

    si = &SOC_INFO(unit);
    phymod_autoneg_control_t_init(&an);
    an.enable = an_enable;
    an.num_lane_adv = si->port_num_lanes[port];
    an.an_mode = phymod_AN_MODE_NONE;
    an.flags   = 0 ;
    fiber_pref=0;

    if (IS_GE_PORT(unit,port)) {
        fiber_pref = soc_property_port_get(unit, port,spn_SERDES_FIBER_PREF, fiber_pref);
        if(fiber_pref) { 
            an.an_mode = phymod_AN_MODE_CL37;
        } else {   
            an.an_mode = phymod_AN_MODE_SGMII;
        }
        PHYMOD_AN_F_IGNORE_MEDIUM_CHECK_SET(&an);
        an.num_lane_adv = 1;
    }

    an.enable = an_enable;

    rv = portmod_ext_to_int_cmd_set(unit, port, portmodExtToInt_CtrlCode_AN, &an);
    if (rv != SOC_E_NONE) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "phy_portmod_dispatch_an_set: portmod_ext_to_int_cmd_set failed")));
        return rv;    
    }
    return rv;
}

int
phy_portmod_dispatch_advert_ability_set(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int rv = SOC_E_NONE;

    rv = portmod_ext_to_int_cmd_set(unit, port, portmodExtToInt_CtrlCode_AbilityAdvert, ability);
    if (rv != SOC_E_NONE) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "phy_portmod_dispatch_adv_ability_set: portmod_ext_to_int_cmd_set failed")));
        return rv;
    }
    return rv;
}

int
phy_portmod_dispatch_one_get(int unit, soc_port_t port, int *parm)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *parm = 1;

    return SOC_E_NONE;
}

int
phy_portmod_dispatch_zero_get(int unit, soc_port_t port, int *parm)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *parm = 0;

    return SOC_E_NONE;
}


 int 
 phy_portmod_dispatch_link_get(int unit, soc_port_t port, int *up) 
 {
    int rv = SOC_E_NONE; 
    rv = portmod_ext_to_int_cmd_get(unit, port, portmodExtToInt_CtrlCode_Link, (void *)up);
    return rv;

 } 

int
phy_portmod_dispatch_enable_set(int unit, soc_port_t port, int enable)
{
    int rv = SOC_E_NONE;
    
    rv = portmod_ext_to_int_cmd_set(unit, port, portmodExtToInt_CtrlCode_Enable, (void *)(&enable)); 
    return rv;
}

int
phy_portmod_dispatch_enable_get(int unit, soc_port_t port, int *enable)
{
    int rv = SOC_E_NONE;
    
    rv = portmod_ext_to_int_cmd_get(unit, port, portmodExtToInt_CtrlCode_Enable, enable); 
    return rv;
}



int
phy_portmod_dispatch_mode_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(mode);

    return SOC_E_NONE;
}

int
phy_portmod_dispatch_mode_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
        *mode = SOC_PM_10GB_FD | SOC_PM_XGMII;
    } else if (IS_GE_PORT(unit, port)) {
        *mode = SOC_PM_1000MB_FD | SOC_PM_GMII |
                SOC_PM_100MB_FD | SOC_PM_100MB_HD |
                SOC_PM_10MB_FD | SOC_PM_10MB_HD | SOC_PM_MII;
    } else {
        *mode = SOC_PM_100MB_FD | SOC_PM_100MB_HD |
                SOC_PM_10MB_FD | SOC_PM_10MB_HD | SOC_PM_MII;
    }

    return SOC_E_NONE;
}

int
phy_portmod_dispatch_remote_ability_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    portmod_port_ability_t remote_ability;

    sal_memset(&remote_ability, 0, sizeof(portmod_port_ability_t));
    portmod_ext_to_int_cmd_get(unit, port, portmodExtToInt_CtrlCode_AbilityRemote, &remote_ability);
    sal_memcpy(ability, &remote_ability, sizeof(soc_port_ability_t));

    return SOC_E_NONE;
}

int
phy_portmod_dispatch_advert_ability_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    portmod_port_ability_t advert_ability;

    sal_memset(&advert_ability, 0, sizeof(portmod_port_ability_t));
    portmod_ext_to_int_cmd_get(unit, port, portmodExtToInt_CtrlCode_AbilityAdvert, &advert_ability);
    sal_memcpy(ability, &advert_ability, sizeof(soc_port_ability_t));

    return SOC_E_NONE;
}

int
phy_portmod_dispatch_local_ability_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    portmod_port_ability_t local_ability;

    portmod_ext_to_int_cmd_get(unit, port, portmodExtToInt_CtrlCode_AbilityLocal, &local_ability);
    sal_memcpy(ability, &local_ability, sizeof(soc_port_ability_t));

    return SOC_E_NONE;
}

int
phy_portmod_dispatch_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    int rv = SOC_E_NONE;

    rv = portmod_ext_to_int_cmd_set(unit, port, portmodExtToInt_CtrlCode_Interface, (void *)(&pif));

    return rv;
}

int
phy_portmod_dispatch_duplex_set(int unit, soc_port_t port, int duplex)
{
    /* Draco Quickturn specific code. */
    if (SAL_BOOT_QUICKTURN && IS_GE_PORT(unit, port)) {
        return phy_fe_ge_duplex_set(unit, port, duplex);
    }

    return SOC_E_NONE;
}

int
phy_portmod_dispatch_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint8   phy_addr;
    uint16  mii_ctrl;

    if (SAL_BOOT_QUICKTURN && IS_GE_PORT(unit, port)) {
        phy_addr = PORT_TO_PHY_ADDR(unit, port);
        /* Quickturn (may) have mii hooks for reconfiguring */
        SOC_IF_ERROR_RETURN(
            soc_miim_read(unit, phy_addr, MII_CTRL_REG, &mii_ctrl));
        *duplex = (mii_ctrl & MII_CTRL_FD) ? 1 : 0;
        return SOC_E_NONE;
    }

    /* Otherwise, just get speed from mac */
    *duplex = 1;
    return SOC_E_NONE;
}

int
phy_portmod_dispatch_speed_set(int unit, soc_port_t port, int speed)
{
  int rv = SOC_E_NONE;

  rv = portmod_ext_to_int_cmd_set(unit, port, portmodExtToInt_CtrlCode_Speed, (void*)&speed);
  return rv;
    
}

int
phy_portmod_dispatch_speed_get(int unit, soc_port_t port, int *speed)
{
int rv = SOC_E_NONE;

  rv = portmod_ext_to_int_cmd_get(unit, port, portmodExtToInt_CtrlCode_Speed, (void*)speed);
  return rv;


}


int
phy_portmod_dispatch_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    int rv = SOC_E_NONE;

    rv = portmod_ext_to_int_cmd_get(unit, port, portmodExtToInt_CtrlCode_Interface, (void *)pif);

    return rv;    
}




int
phy_portmod_dispatch_nocxn_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    portmod_pdata_t pdata;

    portmod_ext_to_int_cmd_get(unit, port, portmodExtToInt_CtrlCode_Interface, &pdata);
    sal_memcpy (pif , &(pdata.pif), sizeof(soc_port_if_t));

    return SOC_E_NONE;
}

/*
 * Function:
 *  phy_portmod_dispatch_mdix_set
 * Description:
 *  Set the Auto-MDIX mode of a port/PHY
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  mode - One of:
 *      BCM_PORT_MDIX_AUTO
 *          Enable auto-MDIX when autonegotiation is enabled
 *      BCM_PORT_MDIX_FORCE_AUTO
 *          Enable auto-MDIX always
 *      BCM_PORT_MDIX_NORMAL
 *          Disable auto-MDIX
 *      BCM_PORT_MDIX_XOVER
 *          Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *  BCM_E_UNAVAIL - feature unsupported by hardware
 *  BCM_E_XXX - other error
 */
int
phy_portmod_dispatch_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (mode == SOC_PORT_MDIX_NORMAL) {
        return SOC_E_NONE;
    } else {
        return SOC_E_UNAVAIL;
    }
}    

/*
 * Function:
 *  phy_portmod_dispatch_mdix_get
 * Description:
 *  Get the Auto-MDIX mode of a port/PHY
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  mode - (Out) One of:
 *      BCM_PORT_MDIX_AUTO
 *          Enable auto-MDIX when autonegotiation is enabled
 *      BCM_PORT_MDIX_FORCE_AUTO
 *          Enable auto-MDIX always
 *      BCM_PORT_MDIX_NORMAL
 *          Disable auto-MDIX
 *      BCM_PORT_MDIX_XOVER
 *          Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *  BCM_E_UNAVAIL - feature unsupported by hardware
 *  BCM_E_XXX - other error
 */
int
phy_portmod_dispatch_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (mode == NULL) {
        return SOC_E_PARAM;
    }

    *mode = SOC_PORT_MDIX_NORMAL;

    return SOC_E_NONE;
}    

/*
 * Function:
 *  phy_portmod_dispatch_mdix_status_get
 * Description:
 *  Get the current MDIX status on a port/PHY
 * Parameters:
 *  unit    - Device number
 *  port    - Port number
 *  status  - (OUT) One of:
 *      BCM_PORT_MDIX_STATUS_NORMAL
 *          Straight connection
 *      BCM_PORT_MDIX_STATUS_XOVER
 *          Crossover has been performed
 * Return Value:
 *  BCM_E_UNAVAIL - feature unsupported by hardware
 *  BCM_E_XXX - other error
 */
int
phy_portmod_dispatch_mdix_status_get(int unit, soc_port_t port, 
                         soc_port_mdix_status_t *status)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (status == NULL) {
        return SOC_E_PARAM;
    }

    *status = SOC_PORT_MDIX_STATUS_NORMAL;

    return SOC_E_NONE;
}    

/*
 * Function:
 *      phy_portmod_dispatch_medium_get
 * Description:
 *      Get the currently chosen medium for the dual-medium PHY
 * Parameters:
 *      unit    -- Device number
 *      port    -- Port number
 *      medium  -- (Out) One of
 *                 SOC_PORT_MEDIUM_NONE  
 *                 SOC_PORT_MEDIUM_COPPER
 *                 SOC_PORT_MEDIUM_FIBER
 *
 * Return value:
 *      BCM_E_NONE
 *      BCM_E_PARAM
 *      BCM_E_UNAVAIL
 */
int
phy_portmod_dispatch_medium_get(int unit, soc_port_t port,
                    soc_port_medium_t *medium)
{
    int rv;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (medium != NULL) {
        *medium = SOC_PORT_MEDIUM_NONE;
        rv = SOC_E_NONE;
    } else {    
        rv = SOC_E_PARAM;
    }
    
    return (rv);
}

/*
 * Function:
 *      phy_portmod_dispatch_control_set
 * Description:
 *      Get phy control
 * Parameters:
 *      unit    -- Device number
 *      port    -- Port number
 *      param  
 *
 * Return value:
 *      BCM_E_NONE
 *      BCM_E_PARAM
 *      BCM_E_UNAVAIL
 */
int
phy_portmod_dispatch_control_set(int unit, soc_port_t port, soc_phy_control_t phy_ctrl, uint32 param)
{

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(phy_ctrl);
    COMPILER_REFERENCE(param);
    
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_portmod_dispatch_control_get
 * Description:
 *      Get phy control
 * Parameters:
 *      unit    -- Device number
 *      port    -- Port number
 *      param  
 *
 * Return value:
 *      BCM_E_NONE
 *      BCM_E_PARAM
 *      BCM_E_UNAVAIL
 */
int
phy_portmod_dispatch_control_get(int unit, soc_port_t port, soc_phy_control_t phy_ctrl, uint32 *param)
{
    int rv;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(phy_ctrl);

    if (param != NULL) {
        *param = 0;
        rv = SOC_E_NONE;
    } else {    
        rv = SOC_E_PARAM;
    }
    
    return (rv);
}

int
phy_portmod_dispatch_notify(int unit, soc_port_t port,
                       soc_phy_event_t event, uint32 value)
{
int cur_speed;
int rv;

    if(event == phyEventSpeed) {
        rv = portmod_ext_to_int_cmd_get(unit, port, portmodExtToInt_CtrlCode_Speed, (void*)&cur_speed);
        if (rv != SOC_E_NONE) {
            LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit, "phy_portmod_dispatch_notify: portmod_ext_to_int_cmd_get failed")));
            return rv;
        }

        if(cur_speed != value){
            rv = portmod_ext_to_int_cmd_set(unit, port, portmodExtToInt_CtrlCode_Speed, (void*)&value);
            if (rv != SOC_E_NONE) {
                LOG_VERBOSE(BSL_LS_BCM_PORT,
                        (BSL_META_U(unit, "phy_portmod_dispatch_notify: portmod_ext_to_int_cmd_get failed")));
                return rv;
            }
        }
    }
    if(event == phyEventSpeedLine) {
        rv = portmod_ext_to_int_cmd_set(unit, port, portmodExtToInt_CtrlCode_SpeedLine, (void*)&value);
        if (rv != SOC_E_NONE) {
            LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit, "phy_portmod_dispatch_notify: portmod_ext_to_int_cmd_get failed")));
            return rv;
        }

    }

    return SOC_E_NONE;
}


/*
 * Variable:    phy_null
 * Purpose:    Phy driver callouts for legacy ext phy to portmod int phy 
 */
phy_driver_t phy_ext_to_int_drv = {
    "Ext to Int PHY Driver conduit",
    phy_portmod_dispatch_init,              /* phy_init                     */
    phy_portmod_dispatch_reset,             /* phy_reset                    */
    phy_portmod_dispatch_link_get,          /* phy_link_get                 */
    phy_portmod_dispatch_enable_set,        /* phy_enable_set               */
    phy_portmod_dispatch_enable_get,        /* phy_enable_get               */
    phy_portmod_dispatch_duplex_set,        /* phy_duplex_set               */
    phy_portmod_dispatch_duplex_get,        /* phy_duplex_get               */
    phy_portmod_dispatch_speed_set,         /* phy_speed_set                */
    phy_portmod_dispatch_speed_get,         /* phy_speed_get                */
    phy_portmod_dispatch_set,               /* phy_master_set               */
    phy_portmod_dispatch_zero_get,          /* phy_master_get               */
    phy_portmod_dispatch_an_set,            /* phy_an_set                   */
    phy_portmod_dispatch_an_get,            /* phy_an_get                   */
    phy_portmod_dispatch_mode_set,          /* phy_adv_local_set            */
    phy_portmod_dispatch_mode_get,          /* phy_adv_local_get            */
    phy_portmod_dispatch_mode_get,          /* phy_adv_remote_get           */
    phy_portmod_dispatch_set,               /* phy_lb_set                   */
    phy_portmod_dispatch_zero_get,          /* phy_lb_get                   */
    phy_portmod_dispatch_interface_set,     /* phy_interface_set            */
    phy_portmod_dispatch_interface_get,     /* phy_interface_get            */
    phy_portmod_dispatch_mode_get,          /* phy_ability                  */
    NULL,                       /* phy_linkup_evt               */
    NULL,                       /* phy_linkdn_evt               */
    phy_portmod_dispatch_mdix_set,          /* phy_mdix_set                 */
    phy_portmod_dispatch_mdix_get,          /* phy_mdix_get                 */
    phy_portmod_dispatch_mdix_status_get,   /* phy_mdix_status_get          */
    NULL,                       /* phy_medium_config_set        */
    NULL,                       /* phy_medium_config_get        */
    phy_portmod_dispatch_medium_get,        /* phy_medium_get               */
    NULL,                       /* phy_cable_diag               */
    NULL,                       /* phy_link_change              */
    phy_portmod_dispatch_control_set,       /* phy_control_set              */
    phy_portmod_dispatch_control_get,       /* phy_control_get              */
    NULL,                       /* phy_reg_read                 */
    NULL,                       /* phy_reg_write                */
    NULL,                       /* phy_reg_modify               */
    phy_portmod_dispatch_notify,/* phy_notify                   */
    NULL,                       /* phy_probe                    */
    phy_portmod_dispatch_advert_ability_set,/* phy_ability_advert_set       */
    phy_portmod_dispatch_advert_ability_get,/* phy_ability_advert_get       */
    phy_portmod_dispatch_remote_ability_get,/* phy_ability_remote_get       */
    phy_portmod_dispatch_local_ability_get, /* phy_ability_local_get        */
    NULL,                       /* phy_firmware_set             */
    NULL,                       /* phy_timesync_config_set      */
    NULL,                       /* phy_timesync_config_get      */
    NULL,                       /* phy_timesync_control_set     */
    NULL,                       /* phy_timesync_control_get     */
    NULL                        /* phy_diag_ctrl                */
};
#endif /* PORTMOD_SUPPORT */
