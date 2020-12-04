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

#include "phynull.h"
#include "phyfege.h"

typedef struct phy_null_saved_data_s
{
    int speed[SOC_MAX_NUM_PORTS];
    soc_port_ability_t null_phy_ability[SOC_MAX_NUM_PORTS];
} phy_null_saved_data_t;

static phy_null_saved_data_t *saved_data[SOC_MAX_NUM_DEVICES];

/*
 * Functions:
 *    phy_null_XXX
 *
 * Purpose:    
 *    Some dummy routines for null and no
 *      connection PHY drivers.
 */

int
_phy_null_port_ability_init(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{

    int max_speed = 0;

 #if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        max_speed = SOC_INFO(unit).port_init_speed[port] ?
                    SOC_INFO(unit).port_init_speed[port] :
                    SOC_INFO(unit).port_speed_max[port];
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
    {
        max_speed = SOC_INFO(unit).port_speed_max[port];
    }

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
phy_null_init(int unit, soc_port_t port)
{
    if (unit < SOC_MAX_NUM_DEVICES) {
        if (NULL == saved_data[unit]) {
            saved_data[unit] = sal_alloc(sizeof(*saved_data[unit]), 
                                         "NULL PHY driver data");
            if (NULL == saved_data[unit]) {
                return SOC_E_MEMORY;
            }
            sal_memset(saved_data[unit], 0, sizeof(*saved_data[unit]));
        }

        if (SOC_PORT_VALID(unit, port)) {
            saved_data[unit]->speed[port] = 0;
            _phy_null_port_ability_init(unit, port,
                                &(saved_data[unit]->null_phy_ability[port]));
        }
    }

    return SOC_E_NONE;
}

int
phy_null_reset(int unit, soc_port_t port, void *user_arg)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(user_arg);

    return SOC_E_NONE;
}

int
phy_null_set(int unit, soc_port_t port, int parm)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(parm);

    return SOC_E_NONE;
}

int
phy_null_one_get(int unit, soc_port_t port, int *parm)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *parm = 1;

    return SOC_E_NONE;
}

int
phy_null_zero_get(int unit, soc_port_t port, int *parm)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *parm = 0;

    return SOC_E_NONE;
}

int
phy_null_link_get(int unit, soc_port_t port, int *up)
{
    /* link is up if phy is enabled and down otherwise */
    return phy_null_enable_get(unit, port, up);
}

int
phy_null_enable_set(int unit, soc_port_t port, int enable)
{
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }
    return SOC_E_NONE;
}

int
phy_null_enable_get(int unit, soc_port_t port, int *enable)
{
    *enable = !PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE);
    return SOC_E_NONE;
}

int
phy_null_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *an = 0;
    *an_done = 0;

    return SOC_E_NONE;
}

int
phy_null_mode_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(mode);

    return SOC_E_NONE;
}

int
phy_null_mode_get(int unit, soc_port_t port, soc_port_mode_t *mode)
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
phy_null_ability_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    if (NULL != saved_data[unit]) {
        *ability = saved_data[unit]->null_phy_ability[port];
    } else {
        /* Only the NULL PHY should call this function. */
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

int
phy_null_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    soc_phy_info_t *pi;

    pi = &SOC_PHY_INFO(unit, port);

    /* coverity[dead_error_condition:FALSE] */
    if (NULL == pi) {
        /* coverity[dead_error_line] */
        return SOC_E_INIT;
    }

    if (pif == SOC_PORT_IF_TBI) {
        pi->phy_flags |= PHY_FLAGS_10B;
    } else {
        pi->phy_flags &= ~PHY_FLAGS_10B;
    }

    return SOC_E_NONE;
}

int
phy_null_duplex_set(int unit, soc_port_t port, int duplex)
{
    /* Draco Quickturn specific code. */
    if (SAL_BOOT_QUICKTURN && IS_GE_PORT(unit, port) &&
        !soc_feature(unit, soc_feature_no_qt_gport)) {
        return phy_fe_ge_duplex_set(unit, port, duplex);
    }

    return SOC_E_NONE;
}

int
phy_null_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint8   phy_addr;
    uint16  mii_ctrl;

    if (SAL_BOOT_QUICKTURN && IS_GE_PORT(unit, port) &&
        !soc_feature(unit, soc_feature_no_qt_gport))  {
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
phy_null_speed_set(int unit, soc_port_t port, int speed)
{
    uint8   phy_addr;
    uint16  mii_ctrl;

    /* Quickturn specific code. */
    if (SAL_BOOT_QUICKTURN && IS_GE_PORT(unit, port) &&
        !soc_feature(unit, soc_feature_no_qt_gport)) {
        if (0 == speed) {
           return SOC_E_NONE;
        } 
        phy_addr = PORT_TO_PHY_ADDR(unit, port);

        SOC_IF_ERROR_RETURN(
            soc_miim_read(unit, phy_addr, MII_CTRL_REG, &mii_ctrl));
        mii_ctrl &= ~(MII_CTRL_SS_LSB | MII_CTRL_SS_MSB | (1 << 9));
        switch(speed) {
        case 10:
            mii_ctrl |= MII_CTRL_SS_10;
            break;
        case 100:
            mii_ctrl |= MII_CTRL_SS_100;
            break;
        case 1000:
            mii_ctrl |= MII_CTRL_SS_1000;
            break;
        case 2500:
            /* Quickturn uses bit 9 to simulate 2.5Gbps mode */ 
            mii_ctrl |= MII_CTRL_SS_1000 | (1 << 9);
            break;
        default:
            return(SOC_E_CONFIG);
        }
        SOC_IF_ERROR_RETURN(
            soc_miim_write(unit, phy_addr, MII_CTRL_REG, mii_ctrl));
    }

    if(unit < SOC_MAX_NUM_DEVICES && port < SOC_MAX_NUM_PORTS) {
        if (NULL != saved_data[unit]) {
            saved_data[unit]->speed[port] = speed;
        } /* Else some other PHY is using this function */
        return SOC_E_NONE;
    } else {
        return SOC_E_PARAM;
    }
    
}

int
phy_null_speed_get(int unit, soc_port_t port, int *speed)
{
#if  defined(BCM_ESW_SUPPORT)
    uint8   phy_addr;
    uint16  mii_ctrl;
#endif


#ifdef BCM_ESW_SUPPORT
    if(SOC_IS_ESW(unit)) {
        if (SAL_BOOT_QUICKTURN && IS_GE_PORT(unit, port) &&
            !soc_feature(unit, soc_feature_no_qt_gport)) {
            phy_addr  = PORT_TO_PHY_ADDR(unit, port);
            /* Quickturn (may) have mii hooks for reconfiguring */
            SOC_IF_ERROR_RETURN(
                soc_miim_read(unit, phy_addr, MII_CTRL_REG, &mii_ctrl));
        switch(MII_CTRL_SS(mii_ctrl)) {
        case MII_CTRL_SS_10:
            *speed = 10;
            break;
        case MII_CTRL_SS_100:
            *speed = 100;
            break;
        case MII_CTRL_SS_1000:
            *speed = 1000;
            break; 
            /* Quickturn uses bit 9 to simulate 2.5Gbps mode */ 
            case (MII_CTRL_SS_1000 | (1 << 9)):
                *speed = 2500;
                break;
        default:
               return SOC_E_UNAVAIL; 
        }
        return SOC_E_NONE;
        }

        if (!SOC_IS_TOMAHAWKX(unit)) {
            /* Otherwise, just get speed from mac */
            if (SOC_INFO(unit).port_speed_max[port] <= 10000) {
                return SOC_E_UNAVAIL;
            } /* Else MAC can't distinguish between speeds > 10G */
        }
    }
#endif

    if(unit < SOC_MAX_NUM_DEVICES && port < SOC_MAX_NUM_PORTS) {
        if (NULL != saved_data[unit]) {
#ifdef BCM_ESW_SUPPORT
            if(SOC_IS_ESW(unit)) {
               if (saved_data[unit]->speed[port] == 0) { /* Not Set Yet */
                   saved_data[unit]->speed[port] =
                           SOC_INFO(unit).port_init_speed[port] ?
                           SOC_INFO(unit).port_init_speed[port] :
                           SOC_INFO(unit).port_speed_max[port];
               }
            }
#endif
            *speed = saved_data[unit]->speed[port];
        } else {
            *speed = 0;
        } /* Else some other PHY is using this function */
        return SOC_E_NONE;
    } else {
        return SOC_E_PARAM;
    }
}

int
phy_null_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    soc_phy_info_t *pi;

    pi = &SOC_PHY_INFO(unit, port);
    /* coverity[dead_error_condition:FALSE] */
    if (NULL == pi) {
        /* coverity[dead_error_line] */
        return SOC_E_INIT;
    }

    if (pi->phy_flags & PHY_FLAGS_10B) {
        *pif = SOC_PORT_IF_TBI;
    } else if (IS_GE_PORT(unit, port)) {
        *pif = SOC_PORT_IF_GMII;
    } else if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
        *pif = SOC_PORT_IF_XGMII;
    } else {
        *pif = SOC_PORT_IF_MII;
    }

    return SOC_E_NONE;
}



int
phy_nocxn_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(pif);

    return SOC_E_NONE;
}

int
phy_nocxn_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_NOCXN;

    return SOC_E_NONE;
}

/*
 * Function:
 *  phy_null_mdix_set
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
phy_null_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
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
 *  phy_null_mdix_get
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
phy_null_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
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
 *  phy_null_mdix_status_get
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
phy_null_mdix_status_get(int unit, soc_port_t port, 
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
 *      phy_null_medium_get
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
phy_null_medium_get(int unit, soc_port_t port,
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
 *      phy_null_control_set
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
phy_null_control_set(int unit, soc_port_t port, soc_phy_control_t phy_ctrl, uint32 param)
{

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(phy_ctrl);
    COMPILER_REFERENCE(param);
    
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_null_control_get
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
phy_null_control_get(int unit, soc_port_t port, soc_phy_control_t phy_ctrl, uint32 *param)
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

/*
 * Function:
 *      phy_null_notify
 * Description:
 *      Get a notification
 * Parameters:
 *      unit    -- Device number
 *      port    -- Port number
 *      event  -- event 
 *      value  -- value
 *
 * Return value:
 *      SOC_E_NONE
 */
int
phy_null_notify(int unit, soc_port_t port,
                soc_phy_event_t event, uint32 value)
{
    int rv = SOC_E_NONE;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(event);
    COMPILER_REFERENCE(value);

    return (rv);
}

/*
 * Variable:    phy_null
 * Purpose:    Phy driver callouts for a null phy (e.g. quickturn)
 * Notes:       This is the driver for a direct connection without a phy.
 */
phy_driver_t phy_null = {
    "Null PHY Driver",
    phy_null_init,              /* phy_init                     */
    phy_null_reset,             /* phy_reset                    */
    phy_null_link_get,          /* phy_link_get                 */
    phy_null_enable_set,        /* phy_enable_set               */
    phy_null_enable_get,        /* phy_enable_get               */
    phy_null_duplex_set,        /* phy_duplex_set               */
    phy_null_duplex_get,        /* phy_duplex_get               */
    phy_null_speed_set,         /* phy_speed_set                */
    phy_null_speed_get,         /* phy_speed_get                */
    phy_null_set,               /* phy_master_set               */
    phy_null_zero_get,          /* phy_master_get               */
    phy_null_set,               /* phy_an_set                   */
    phy_null_an_get,            /* phy_an_get                   */
    phy_null_mode_set,          /* phy_adv_local_set            */
    phy_null_mode_get,          /* phy_adv_local_get            */
    phy_null_mode_get,          /* phy_adv_remote_get           */
    phy_null_set,               /* phy_lb_set                   */
    phy_null_zero_get,          /* phy_lb_get                   */
    phy_null_interface_set,     /* phy_interface_set            */
    phy_null_interface_get,     /* phy_interface_get            */
    phy_null_mode_get,          /* phy_ability                  */
    NULL,                       /* phy_linkup_evt               */
    NULL,                       /* phy_linkdn_evt               */
    phy_null_mdix_set,          /* phy_mdix_set                 */
    phy_null_mdix_get,          /* phy_mdix_get                 */
    phy_null_mdix_status_get,   /* phy_mdix_status_get          */
    NULL,                       /* phy_medium_config_set        */
    NULL,                       /* phy_medium_config_get        */
    phy_null_medium_get,        /* phy_medium_get               */
    NULL,                       /* phy_cable_diag               */
    NULL,                       /* phy_link_change              */
    phy_null_control_set,       /* phy_control_set              */
    phy_null_control_get,       /* phy_control_get              */
    NULL,                       /* phy_reg_read                 */
    NULL,                       /* phy_reg_write                */
    NULL,                       /* phy_reg_modify               */
    phy_null_notify,            /* phy_notify                   */
    NULL,                       /* phy_probe                    */
    NULL,                       /* phy_ability_advert_set       */
    phy_null_ability_get,       /* phy_ability_advert_get       */
    phy_null_ability_get,       /* phy_ability_remote_get       */
    phy_null_ability_get,       /* phy_ability_local_get        */
    NULL,                       /* phy_firmware_set             */
    NULL,                       /* phy_timesync_config_set      */
    NULL,                       /* phy_timesync_config_get      */
    NULL,                       /* phy_timesync_control_set     */
    NULL,                       /* phy_timesync_control_get     */
    NULL                        /* phy_diag_ctrl                */
};

/*
 * Variable:    phy_nocxn
 * Purpose:    Phy driver callouts for no connection
 * Notes:       This is the driver used when a port is
 *              de-initialized (eg, hotswap card is pulled.)
 */
phy_driver_t phy_nocxn = {
    "No-connection PHY Driver",
    phy_null_init,              /* phy_init */
    phy_null_reset,             /* phy_reset */
    phy_null_zero_get,          /* phy_link_get */
    phy_null_enable_set,        /* phy_enable_set */
    phy_null_enable_get,        /* phy_enable_get */
    phy_null_set,               /* phy_duplex_set */
    phy_null_one_get,           /* phy_duplex_get */
    phy_null_set,               /* phy_speed_set */
    phy_null_zero_get,          /* phy_speed_get */
    phy_null_set,               /* phy_master_set */
    phy_null_zero_get,          /* phy_master_get */
    phy_null_set,               /* phy_an_set */
    phy_null_an_get,            /* phy_an_get */
    phy_null_mode_set,          /* phy_adv_local_set */
    phy_null_mode_get,          /* phy_adv_local_get */
    phy_null_mode_get,          /* phy_adv_remote_get */
    phy_null_set,               /* phy_lb_set */
    phy_null_zero_get,          /* phy_lb_get */
    phy_nocxn_interface_set,    /* phy_interface_set */
    phy_nocxn_interface_get,    /* phy_interface_get */
    phy_null_mode_get,          /* phy_ability */
    NULL,                       /* phy_linkup_evt */
    NULL,                       /* phy_linkdn_evt */
    phy_null_mdix_set,          /* phy_mdix_set */
    phy_null_mdix_get,          /* phy_mdix_get */
    phy_null_mdix_status_get,   /* phy_mdix_status_get */
    NULL,                       /* phy_medium_config_set */
    NULL,                       /* phy_medium_config_get */
    phy_null_medium_get,        /* phy_medium_get        */
    NULL,                       /* phy_cable_diag */
    NULL                        /* phy_link_change */
};
