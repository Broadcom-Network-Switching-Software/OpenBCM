/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy522x.c
 * Purpose:    Broadcom 522x phy driver
 *        Supports 5218, 5220/5221, 5226, 5228, 5238, 5248
 */

#ifdef INCLUDE_PHY_522X

#include <sal/types.h>
#include <sal/core/thread.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "phyident.h"
#include "phyreg.h"
#include "phyfege.h"
#include "phy522x.h"

extern int 
phy_522x_cable_diag(int unit, soc_port_t port,
            soc_port_cable_diag_t *status);

#define LINK_WAIT_TIMEOUT 100

#define EFX_HALF_THRESHOLD_MODE 0

/*
 * Function:
 *    phy_522x_init
 * Purpose:
 *    Initialize phy
 * Parameters:
 *    unit - device number
 *    port - port number
 * Returns:
 *    SOC_E_XXX
 */
STATIC int
phy_522x_init(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    uint16       led1;
    uint16       led2;
    uint16       led_mode;
    int          phy_index;
    soc_pbmp_t pbmp_100fx;
    uint16      tmp = 0;


    pc = EXT_PHY_SW_STATE(unit, port);
   
    /* BCM5248 after a hardware reset or s/w reset does not 
       guarantee internal automatic power down (APD) timer to be 
       reset for all ports. It could result in waking up of 
       all inactive ports at the same time causing excessive noise 
       on the power supplies causing active ports to have performance
       issues. To avoid this reset all the MII registers.
       Refer - Design Guide for more detials 
     */
    if(PHY_IS_BCM5248(pc)) {
        /* Set all the MII registers to their default values */
        SOC_IF_ERROR_RETURN
            (PHY522X_REG_WRITE(unit, pc, 0x0000, 0x1f, 0x0200));
       /* Delay 100 microseconds */
       sal_usleep(100); 
    }

    SOC_IF_ERROR_RETURN(phy_fe_init(unit, port));

    SOC_IF_ERROR_RETURN
        (WRITE_PHY_REG(unit, pc, 0x16, 0x0002)); 

    switch ((pc->phy_model << 16) | pc->phy_oui) {
    case (PHY_BCM5228_MODEL << 16) | PHY_BCM5228_OUI:
    case (PHY_BCM5238_MODEL << 16) | PHY_BCM5238_OUI:
    case (PHY_BCM5248_MODEL << 16) | PHY_BCM5248_OUI:

        /* enable jumbo packets */
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY522X_AUX_MODE2r(unit, pc, 0x0400, 0x0400));
        break;
    }

    if (PHY_IS_BCM5248(pc)) {
        /* Find out which port is the first port of the octal PHY */
        phy_index = soc_property_port_get(unit, port, 
                                          spn_PHY_OCTAL_PORT_FIRST, 0);

        phy_index = (port - phy_index) % 8;
        if (0 == phy_index) {
            /* Link */
            led1 = soc_property_port_get(unit, port, spn_PHY_LED1_MODE, 0); 

            /* Activity */
            led2 = soc_property_port_get(unit, port, spn_PHY_LED2_MODE, 1);

            /* Enable parallel LED mode and program LED 1 and LED2 */
            SOC_IF_ERROR_RETURN
                (READ_PHY522X_AUX_MODE4r(unit, pc, &led_mode));
            led_mode = ~(0x023f);
            led_mode |= (1 << 9) |             /* Enable parallel LED mode */
                        (led2 << 3) | (led1);  /* LED1 and LED2 mode select */ 
            SOC_IF_ERROR_RETURN
                (WRITE_PHY522X_AUX_MODE4r(unit, pc, led_mode));
        }

        if (2 == phy_index) {
            /* Disable Extended Parallel LED Mode */
            SOC_IF_ERROR_RETURN
                (READ_PHY522X_AUX_MODE4r(unit, pc, &led_mode));
            led_mode = ~(1 << 9);
            SOC_IF_ERROR_RETURN
                (WRITE_PHY522X_AUX_MODE4r(unit, pc, led_mode));
        }
    }

    /*
      * Get 100-FX ports from config.bcm.
      */
    pbmp_100fx = soc_property_get_pbmp(unit, spn_PBMP_FE_100FX, 0);
    if (SOC_PBMP_MEMBER(pbmp_100fx, port)) {
        /* Speed/Duplex selection and auto-negotiation Disable */
        SOC_IF_ERROR_RETURN             
            (WRITE_PHY522X_MII_CTRLr(unit, pc, 0x2100));

        /* Scrambler and Descrambler Disable and Far-End-Fault Enable */
        SOC_IF_ERROR_RETURN            
            (READ_PHY522X_AUX_CTRLr(unit, pc, &tmp));
        tmp |= 0x0220;
        SOC_IF_ERROR_RETURN            
            (WRITE_PHY522X_AUX_CTRLr(unit, pc, tmp));

        /* Configure Transmit and Receive for Binary Signaling */
        SOC_IF_ERROR_RETURN
            (PHY522X_REG_READ(unit, pc, 0x0000, 0x17, &tmp));
        tmp |= 0x20;
        SOC_IF_ERROR_RETURN
            (PHY522X_REG_WRITE(unit, pc, 0x0000, 0x17, 0x20));

        /* Enable Internal EFX Signal Detect Function */
        /* Enable the shadow register */
        SOC_IF_ERROR_RETURN
            (PHY522X_REG_WRITE(unit, pc, 0x0000, 0x1f, 0x008b));
        /* Enable the special Signal Detect function */
        SOC_IF_ERROR_RETURN
            (PHY522X_REG_WRITE(unit, pc, 0x0000, 0x19, 0x0200));
        /* Configure the transmit amplitude for 1V pk-pk differential */
        SOC_IF_ERROR_RETURN
            (PHY522X_REG_WRITE(unit, pc, 0x0000, 0x1d, 0x0084));
        /* Exist the shadow register */
        SOC_IF_ERROR_RETURN
            (PHY522X_REG_WRITE(unit, pc, 0x0000, 0x1f, 0x000b));
#if EFX_HALF_THRESHOLD_MODE
        /* Enable the shadow register */
        SOC_IF_ERROR_RETURN
            (PHY522X_REG_WRITE(unit, pc, 0x0000, 0x1f, 0x008b));
        /* Set the threshold to the Half Threshold mode*/
        SOC_IF_ERROR_RETURN
            (PHY522X_REG_WRITE(unit, pc, 0x0000, 0x11, 0x0a01));
        /* Exist the shadow register */
        SOC_IF_ERROR_RETURN
            (PHY522X_REG_WRITE(unit, pc, 0x0000, 0x1f, 0x000b));
#endif
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     phy_522x_enable_set
 * Description:
 *     Enable or disable the physical interface
 * Parameters:
 *     unit   - Device number
 *     port   - Port number
 *     enable - Boolean, true = enable, false = enable.
 * Returns:
 *     SOC_E_XXX
 */
STATIC int
phy_522x_enable_set(int unit, soc_port_t port, int enable)
{
    int           rv;         /* Return value */
    uint16        data;       /* New value to write to PHY register */
    phy_ctrl_t   *pc;

    pc    = EXT_PHY_SW_STATE(unit, port);
    data  = enable ? 0 : MII_ECR_TD; /* Transmitt enable/disable */

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY522X_AUX_CTRLr(unit, pc, data, MII_ECR_TD));

    data  = enable ? 0 : PHY522X_SUPER_ISOLATE_MODE; 

    /* Device needs to be put in super-isolate mode in order to disable 
     * the link in 10BaseT mode
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY522X_AUX_MULTIPLE_PHYr(unit, pc, data,
                                          PHY522X_SUPER_ISOLATE_MODE));

    rv = phy_fe_ge_enable_set(unit, port, enable);

    return rv;
}

/*
 * Function:
 *    phy_522x_mdix_set
 * Description:
 *    Set the Auto-MDIX mode of a port/PHY
 * Parameters:
 *    unit - Device number
 *    port - Port number
 *    mode - One of:
 *            SOC_PORT_MDIX_AUTO
 *            Enable auto-MDIX when autonegotiation is enabled
 *            SOC_PORT_MDIX_FORCE_AUTO
 *            Enable auto-MDIX always
 *        SOC_PORT_MDIX_NORMAL
 *            Disable auto-MDIX
 *        SOC_PORT_MDIX_XOVER
 *            Disable auto-MDIX, and swap cable pairs
 * Returns:
 *    SOC_E_XXX
 */
STATIC int
phy_522x_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{
    uint16       tmp;
    uint16       miscreg;
    int          force_auto;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (PHY_IS_BCM5218(pc)) {
        if (mode == SOC_PORT_MDIX_NORMAL) {
            return SOC_E_NONE;
        } else {
            return SOC_E_UNAVAIL;
        }
    }
    force_auto = PHY_IS_BCM5248(pc);

    tmp = miscreg = 0;
    switch (mode) {
    case SOC_PORT_MDIX_AUTO:
                               /* clear manual swap and disable */
                               /* clear force auto */
        break;
    case SOC_PORT_MDIX_FORCE_AUTO:
        if (!force_auto) {
            return SOC_E_UNAVAIL;
        }
                              /* clear manual swap and disable */
        miscreg = 0x4000;    /* set force auto */
        break;

    case SOC_PORT_MDIX_XOVER:
        tmp = 0x1800;         /* force MDIX */
                              /* disable auto-MDIX */
        break;
    case SOC_PORT_MDIX_NORMAL:
        tmp = 0x0800;         /* force mode to normal */
                              /* disable auto-MDIX */
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    if (force_auto) {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY522X_MISC_CTRLr(unit, pc, miscreg, 0x4000));
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY522X_AUX_ERR_GENERAL_STATr(unit, pc, tmp, 0x1800));
  
    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_522x_mdix_get
 * Description:
 *    Get the Auto-MDIX mode of a port/PHY
 * Parameters:
 *    unit - Device number
 *    port - Port number
 *    mode - (Out) One of:
 *            SOC_PORT_MDIX_AUTO
 *            Enable auto-MDIX when autonegotiation is enabled
 *            SOC_PORT_MDIX_FORCE_AUTO
 *            Enable auto-MDIX always
 *        SOC_PORT_MDIX_NORMAL
 *            Disable auto-MDIX
 *        SOC_PORT_MDIX_XOVER
 *            Disable auto-MDIX, and swap cable pairs
 * Returns:
 *    SOC_E_XXX
 */
STATIC int
phy_522x_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    uint16       tmp;
    phy_ctrl_t  *pc;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (mode == NULL) {
        return SOC_E_PARAM;
    }

    if (PHY_IS_BCM5218(pc)) {
        *mode = SOC_PORT_MDIX_NORMAL;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY522X_AUX_ERR_GENERAL_STATr(unit, pc, &tmp));

    switch (tmp & 0x1800) {
    case 0x0000:
    case 0x1000:
        *mode = SOC_PORT_MDIX_AUTO;
        break;
    case 0x0800:
        *mode = SOC_PORT_MDIX_NORMAL;
        break;
    case 0x1800:
        *mode = SOC_PORT_MDIX_XOVER;
        break;
    }

    if (PHY_IS_BCM5248(pc) &&
        (*mode == SOC_PORT_MDIX_AUTO)) {

        SOC_IF_ERROR_RETURN
            (READ_PHY522X_MISC_CTRLr(unit, pc, &tmp));
        if (tmp & 0x4000) {
            *mode = SOC_PORT_MDIX_FORCE_AUTO;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *    phy_522x_mdix_status_get
 * Description:
 *    Get the current MDIX status on a port/PHY
 * Parameters:
 *    unit    - Device number
 *    port    - Port number
 *    status  - (OUT) One of:
 *            SOC_PORT_MDIX_STATUS_NORMAL
 *            Straight connection
 *            SOC_PORT_MDIX_STATUS_XOVER
 *            Crossover has been performed
 * Returns:
 *    SOC_E_XXX
 */
STATIC int
phy_522x_mdix_status_get(int unit, soc_port_t port,
                         soc_port_mdix_status_t *status)
{
    phy_ctrl_t  *pc;
    uint16       tmp;

    if (status == NULL) {
        return SOC_E_PARAM;
    }

    pc = EXT_PHY_SW_STATE(unit, port);
    if (PHY_IS_BCM5218(pc)) {
        *status = SOC_PORT_MDIX_STATUS_NORMAL;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_PHY522X_AUX_ERR_GENERAL_STATr(unit, pc, &tmp));
    if (tmp & 0x2000) {
        *status = SOC_PORT_MDIX_STATUS_XOVER;
    } else {
        *status = SOC_PORT_MDIX_STATUS_NORMAL;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_522x_reg_read
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
STATIC int
phy_522x_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t           *pc;      /* PHY software state */

    EXT_PHY_INIT_CHECK(unit, port);

    pc        = EXT_PHY_SW_STATE(unit, port);
    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    SOC_IF_ERROR_RETURN
        (phy_reg_fe_read(unit, pc, reg_bank, reg_addr, &data));

   *phy_data = data;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_522x_reg_write
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
STATIC int
phy_522x_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t           *pc;      /* PHY software state */

    EXT_PHY_INIT_CHECK(unit, port);

    pc   = EXT_PHY_SW_STATE(unit, port);
    data = (uint16) (phy_data & 0x0000FFFF);

    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    return phy_reg_fe_write(unit, pc, reg_bank, reg_addr, data);
}
/*
 * Function:
 *      phy_522x_reg_modify
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
STATIC int
phy_522x_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data, uint32 phy_data_mask)
{
    uint16               reg_bank;
    uint8                reg_addr;
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;     /* Temporary holder for phy_data_mask */
    phy_ctrl_t           *pc;      /* PHY software state */

    EXT_PHY_INIT_CHECK(unit, port);

    pc   = EXT_PHY_SW_STATE(unit, port);
    data = (uint16) (phy_data & 0x0000FFFF);
    mask = (uint16) (phy_data_mask & 0x0000FFFF);

    reg_bank  = SOC_PHY_REG_BANK(phy_reg_addr);
    reg_addr  = SOC_PHY_REG_ADDR(phy_reg_addr);

    return phy_reg_fe_modify(unit, pc, reg_bank, reg_addr, data, mask);
}

/*
 * Function:
 *    phy_522x_control_set
 * Description:
 *     Set PHY specific properties 
 * Parameters:
 *     unit        device number
 *     port        port number
 *     type        configuration type
 *     value       new value for the configuration ; ms 
 * Return:
 *     BCM_E_XXX
 */
STATIC int
phy_522x_control_set(int unit, soc_port_t port, 
                             soc_phy_control_t type, uint32 value)
{
    int rv = SOC_E_NONE;
    uint16 data16;
    phy_ctrl_t *pc;
    
    pc = EXT_PHY_SW_STATE(unit, port);

    if (!PHY_IS_BCM5248(pc)) {
        return SOC_E_UNAVAIL;
    }

    PHY_CONTROL_TYPE_CHECK(type);


    switch(type) {
        case SOC_PHY_CONTROL_POWER:
            if (pc->power_mode == value) {
                return SOC_E_NONE;
            } else if (value == SOC_PHY_CONTROL_POWER_AUTO) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY522X_AUX_STAT2r(unit,pc,
                        PHY_522X_AUTO_PWRDWN_EN,
                        PHY_522X_AUTO_PWRDWN_EN));
                pc->power_mode = value;
            } else if ((value == SOC_PHY_CONTROL_POWER_FULL)
                   || (value == SOC_PHY_CONTROL_POWER_AUTO_DISABLE)) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_PHY522X_AUX_STAT2r(unit,pc,
                        0,
                        PHY_522X_AUTO_PWRDWN_EN));
                pc->power_mode = value;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case SOC_PHY_CONTROL_POWER_AUTO_WAKE_TIME:
            if (value <= PHY_522X_AUTO_PWRDWN_WAKEUP_MAX) {
                /* value = 0 mean force stay in low-power mode */
                if (value < PHY_522X_AUTO_PWRDWN_WAKEUP_UNIT) {
                    value = PHY_522X_AUTO_PWRDWN_WAKEUP_UNIT;
                }
            } else { /* anything more then max, set to the max */
                value = PHY_522X_AUTO_PWRDWN_WAKEUP_MAX;
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY522X_AUX_STAT2r(unit,pc,
                    value/PHY_522X_AUTO_PWRDWN_WAKEUP_UNIT,
                    PHY_522X_AUTO_PWRDWN_WAKEUP_MASK));
            break;
        case SOC_PHY_CONTROL_POWER_AUTO_SLEEP_TIME:
            /* 
             * sleep time configuration is either 2.5s or 5.0 s, 
             * default is 2.5s 
             */
            if (value < PHY_522X_AUTO_PWRDWN_SLEEP_MAX) {
                data16 = 0; /* anything less than 5.0s, default to 2.5s */
            } else {
                data16 = PHY_522X_AUTO_PWRDWN_SLEEP_MASK;
            }
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY522X_AUX_STAT2r(unit,pc,
                    data16, PHY_522X_AUTO_PWRDWN_SLEEP_MASK));
            break;
        default:
            rv = SOC_E_UNAVAIL;
            break;
    }
    return rv;
}    

/*
 * Function:
 *    phy_522x_control_get
 * Description:
 *     Get PHY specific properties 
 * Parameters:
 *     unit        device number
 *     port        port number
 *     type        configuration type
 *     value       new value for the configuration ; ms 
 * Return:
 *     BCM_E_XXX
 */
STATIC int
phy_522x_control_get(int unit, soc_port_t port, 
                             soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t *pc;
    uint16 data16;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (!PHY_IS_BCM5248(pc)) {
        return SOC_E_UNAVAIL;
    }

    PHY_CONTROL_TYPE_CHECK(type);

    rv = SOC_E_NONE;
    switch(type) {
        case SOC_PHY_CONTROL_POWER:
            *value = pc->power_mode;
            break;
        case SOC_PHY_CONTROL_POWER_AUTO_SLEEP_TIME:
            SOC_IF_ERROR_RETURN
                (READ_PHY522X_AUX_STAT2r(unit,pc, &data16));

            if (data16 & PHY_522X_AUTO_PWRDWN_SLEEP_MASK) {
                *value = PHY_522X_AUTO_PWRDWN_SLEEP_MAX;
            } else {
                *value = 2500;
            }
            break;

        case SOC_PHY_CONTROL_POWER_AUTO_WAKE_TIME:
            SOC_IF_ERROR_RETURN
                (READ_PHY522X_AUX_STAT2r(unit,pc, &data16));

            data16 &= PHY_522X_AUTO_PWRDWN_WAKEUP_MASK;
            *value = data16 * PHY_522X_AUTO_PWRDWN_WAKEUP_UNIT;
            break;

        default:
            rv = SOC_E_UNAVAIL;
            break;
    }
    return rv;
}

/*
 * Variable:    phy_522xdrv_fe
 * Purpose:    Phy Driver for 522X
 */
phy_driver_t phy_522xdrv_fe = {
    "Broadcom 522X 10/100 Fast Ethernet PHY Driver",
    phy_522x_init,
    phy_fe_ge_reset,
    phy_fe_ge_link_get,
    phy_522x_enable_set,
    phy_fe_ge_enable_get,
    phy_fe_ge_duplex_set,
    phy_fe_ge_duplex_get,
    phy_fe_ge_speed_set,
    phy_fe_ge_speed_get,
    phy_fe_ge_master_set,
    phy_fe_ge_master_get,
    phy_fe_ge_an_set,
    phy_fe_ge_an_get,
    phy_fe_adv_local_set,
    phy_fe_adv_local_get,
    phy_fe_adv_remote_get,
    phy_fe_ge_lb_set,
    phy_fe_ge_lb_get,
    phy_fe_interface_set,
    phy_fe_interface_get,
    phy_fe_ability_get,
    NULL,                       /* Link up event */
    NULL,                       /* Link down event */
    phy_522x_mdix_set,
    phy_522x_mdix_get,
    phy_522x_mdix_status_get,
    NULL,                       /* Medium config set */
    NULL,                       /* Medium config get */
    phy_fe_ge_medium_get,
    phy_522x_cable_diag,
    NULL,                       /* phy_link_change */
    phy_522x_control_set,       /* phy_control_set */
    phy_522x_control_get,       /* phy_control_get */
    phy_522x_reg_read,
    phy_522x_reg_write, 
    phy_522x_reg_modify,   
    NULL                        /* Phy event notify */
};

#endif /* INCLUDE_PHY_522X */

typedef int _soc_phy_522x_not_empty; /* Make ISO compilers happy. */
