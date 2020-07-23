/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy82780.c
 * Purpose:     Support for Quadra28 external PHY 
 */

/*
 *   This module implements an NTSW SDK Phy driver for the phy82780 External PHY.
 *  
 *   LAYERING.
 *
 *   This driver is built on top of the PHYMOD library, which is a PHY
 *   driver library that can operate on a family of PHYs, including
 *   phy82780 and Sesto and etc..,  PHYMOD can be built in a standalone enviroment and be
 *   provided independently from the SDK.  PHYMOD APIs consist of
 *   "core" APIs and "phy" APIs.
 *
 *
 *   KEY IDEAS AND MAIN FUNCTIONS
 *
 *   Some key ideas in this architecture are:
 *
 *   o A PHMOD "phy" consists of one more more lanes, all residing in a single
 *     core.  An SDK "phy" is a logical port, which can consist of one or
 *     more lanes in a single core, OR, can consist of multiples lanes in
 *     more than one core.
 *   o PHYMOD code is "stateless" in the sense that all calls to PHYMOD
 *     APIs are fully parameterized and no per-phy state is held by a PHYMOD
 *     driver.
 *   o PHYMOD APIs do not map 1-1 with SDK .pd_control_set or .pd_control_get
 *     APIs (nor ".pd_xx" calls, nor to .phy82780_per_lane_control_set and
 *     .phy82780_per_lane_control_get APIs.
 *
 *   The purpose of this code, then, is to provide the necessary translations
 *   between the SDK interfaces and the PHYMOD interfaces.  This includes:
 * 
 *   o Looping over the cores in a logical PHY in order to operate on the
 *     supporting PHMOD PHYs
 *   o Determining the configuration data for the phy, based on programmed
 *     defaults and SOC properties.
 *   o Managing the allocation and freeing of phy data structures required for
 *     working storage.  Locating these structures on procedure invocation.
 *   o Mapping SDK API concepts to PHYMOD APIs.  In some cases, local state is
 *     required in this module in order to present the legacy API.
 *
 * 
 *   MAIN DATA STRUCTURES
 * 
 *   phy_ctrl_t
 *   The PHY control structure defines the SDK notion of a PHY
 *   (existing) structure owned by SDK phy driver modules.  In order
 *   to support PHYMOD PHYs, one addition was made to this structure
 *   (soc_phymod_ctrl_t phymod_ctrl;)
 *
 *   quarda28_config_t
 *   Driver specific data.  This structure is used by quadra28 to hold
 *   logical port specific working storage.
 *
 *   soc_phymod_ctrl_t
 *   PHYMOD drivers.  Resides in phy_ctrl_t specifies how many cores
 *   are in this phy and contains an array of pointers to
 *   soc_phymod_phy_t structures, which define a PHYMOD PHY.
 *
 *   soc_phymod_phy_t
 *   This structure contains a pointer to phymod_phy_access_t and a
 *   pointer to the associated soc_phymod_core_t.  Instances if this
 *   structure are created during device probe/init and are maintained
 *   by the SOC.
 *
 *   soc_phymod_core_t
 *   This structure contains per-core information.  Multiple PHYMOD
 *   PHYS can point to a single core.
 *
 *   phymod_phy_access_t
 *   This structure contains information about how to read/write PHY
 *   registers.  A required parameter for PHYMOD PHY APIs
 * 
 *   phymod_core_access_t
 *   This structure contains information about how to read/write PHY
 *   registers.  A required parameter for PHYMOD core APIs.
 */
#include "phydefs.h"      /* Must include before other phy related includes */ 
#if defined(INCLUDE_PHY_82780)
#include "phyconfig.h"     /* Must include before other phy related includes */

#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/port_ability.h>
#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>
#include <soc/phy/phymod_ids.h>

#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "xgxs.h"


#define PHYMOD_QUADRA28_SUPPORT

#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>
#include <phymod/phymod_diagnostics.h>


#define NUM_LANES                           4    /* num of lanes per core */
#define MAX_NUM_LANES                       4    /* max num of lanes per port */
#define PHY82780_NO_CFG_VALUE               (-1)
#define PHY82780_DBG_REGACC                 0x2
#define PHY82780_DBG_MEM                    0x3
#define PHY82780_SPD_10G                    10000
#define PHY82780_SPD_11G                    11000
#define PHY82780_SPD_40G                    40000
#define PHY82780_SPD_42G                    42000
#define PHY82780_SPD_1G                     1000
#define INTERFACE_SIDE_SHIFT                31
#define Q28_ID_82780                        0x82780
#define Q28_ID_8278F                        0x8278F
#define Q28_ID_82752                        0x82752
#define Q28_ID_82758                        0x82758

#define PHY82780_MAX_LANE                   4
#define PHY82780_CHIP_ID_SIZE               25

typedef struct phy82780_device_aux_mode {
    uint32_t repeater;
    uint32_t ull_datapath;
    uint32_t core_id;
} phy82780_device_aux_mode_t;

typedef struct phy82780_speed_config_s {
    uint32  port_refclk_int;
    int     speed;
    int     port_num_lanes;
    uint32  line_interface;
    int     pll_divider_req  ;
    int     higig_port;
} phy82780_speed_config_t;

typedef enum PHY82780_INTF_SIDE {
    PHY82780_LINE_SIDE,
    PHY82780_SYS_SIDE 
}PHY82780_INTF_SIDE;

/*
   Config - per logical port
*/
typedef struct {
    phymod_polarity_t               phy_polarity_config;
    phymod_phy_reset_t              phy_reset_config;
    phy82780_speed_config_t         speed_config;
    int                             ull_datapath;
    int                             devid;
    char                            dev_name[PHY82780_CHIP_ID_SIZE];
    int                             auto_mod_detect;
    phy82780_device_aux_mode_t        *device_aux_modes ;
} phy82780_config_t;

#define DEVID(_pc)           (((phy82780_config_t *)((_pc)->driver_data))->devid)
#define DEV_NAME(_pc)        (((phy82780_config_t *)((_pc)->driver_data))->dev_name)

#define PHY82780_SET_SYS_SIDE(x)      (x).port_loc = phymodPortLocSys 
#define PHY82780_CLEAR_SYS_SIDE(x)    (x).port_loc = phymodPortLocLine

STATIC int
phy_82780_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability);

STATIC int
phy_82780_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int
_phy_82780_find_soc_phy_lane(soc_phymod_ctrl_t *pmc, uint32_t lane, 
                        soc_phymod_phy_t **p_phy, uint32 *lane_map);

STATIC int
phy_82780_power_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value);
STATIC int
phy_82780_power_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value);
STATIC int
phy_82780_per_lane_tx_get(soc_phymod_ctrl_t *pmc, int32 intf, soc_phy_control_t type, int lane, uint32 *value);
STATIC int
phy_82780_per_lane_tx_set(soc_phymod_ctrl_t *pmc, int32 intf, soc_phy_control_t type, int lane, uint32 value);
STATIC int
phy_82780_cl72_enable_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value);
STATIC int
phy_82780_cl72_status_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value);
STATIC int
phy_82780_cl72_enable_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value);
STATIC int
phy_82780_link_monitor_status_get (soc_phymod_ctrl_t *pmc, int32 intf, int32 lane);
STATIC int
phy_82780_link_monitor_set (soc_phymod_ctrl_t *pmc, int32 intf, int32 lane, uint32 value);
STATIC int
phy_82780_tx_lane_squelch_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value);
STATIC int
phy_82780_per_lane_tx_lane_squelch_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value);
STATIC int 
phy_82780_fec_enable_set(soc_phymod_ctrl_t *pmc, uint32 value);
STATIC int 
phy_82780_loopback_internal_pmd_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value);
STATIC int
phy_82780_an_set(int unit, soc_port_t port, int enable);
STATIC int
phy_82780_control_get(int unit, soc_port_t port, soc_phy_control_t type, uint32 *value);
STATIC int
phy_82780_adoptation_release(phy_ctrl_t *pc, int32 intf);
STATIC int
phy_82780_config_fail_over(phy_ctrl_t *pc, int32 intf, uint32_t value);
STATIC int
phy_82780_config_fail_over_get(phy_ctrl_t *pc, int32 intf, uint32_t *value);


/*
 * Function:
 *      _phy82780_reg_read
 * Doc:
 *      register read operations
 * Parameters:
 *      unit            - (IN)  unit number
 *      core_addr       - (IN)  core address
 *      reg_addr        - (IN)  address to read
 *      val             - (OUT) read value
 */
STATIC int 
_phy82780_reg_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    uint16 data16;
    int rv;
    soc_phymod_core_t *core = (soc_phymod_core_t *)user_acc;

    rv = core->read(core->unit, core_addr, reg_addr, &data16);
    *val = data16;
    PHYMOD_VDBG(PHY82780_DBG_REGACC, NULL,("-22%s: core_addr: 0x%08x reg_addr: 0x%08x, data: 0x%04x\n", __func__, core_addr, reg_addr, *val ));

    return rv;
}

/*
 * Function:
 *      _phy82780_reg_write
 * Doc:
 *      register write operations
 * Parameters:
 *      unit            - (IN)  unit number
 *      core_addr       - (IN)  core address
 *      reg_addr        - (IN)  address to write
 *      val             - (IN)  write value
 */
STATIC int 
_phy82780_reg_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    soc_phymod_core_t *core = (soc_phymod_core_t *)user_acc;
    uint16 data16 = (uint16)val;
    uint16 mask = (uint16)(val >> 16);

    if (mask) {
        if (core->wrmask) {
            return core->wrmask(core->unit, core_addr, reg_addr, data16, mask);
        }
        (void)core->read(core->unit, core_addr, reg_addr, &data16);
        data16 &= ~mask;
        data16 |= (val & mask);
    }
    PHYMOD_VDBG(PHY82780_DBG_REGACC, NULL,("-22%s: core_addr: 0x%08x reg_addr: 0x%08x, data: 0x%04x\n", __func__, core_addr, reg_addr, val ));
    return core->write(core->unit, core_addr, reg_addr, data16);
}

/*
 * Function:
 *      phy82780_speed_to_interface_config_get
 * Purpose:     
 *      Convert speed to interface_config struct
 * Parameters:
 *      unit                - BCM unit number.
 *      port                - Port number.
 *      speed               - speed to convert
 *      interface_config    - output interface config struct
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int 
phy82780_speed_to_interface_config_get(phy82780_speed_config_t* speed_config, phymod_phy_inf_config_t* interface_config)
{
    SOC_IF_ERROR_RETURN(phymod_phy_inf_config_t_init(interface_config));

    interface_config->interface_modes = 0;
    interface_config->data_rate       = speed_config->speed;
    interface_config->pll_divider_req = speed_config->pll_divider_req ;

    switch (speed_config->speed) {
        case PHY82780_SPD_10G:
        case PHY82780_SPD_11G:
            switch(speed_config->line_interface) {
                case SOC_PORT_IF_KR:
                    interface_config->interface_type = phymodInterfaceKR;
                break;
                case SOC_PORT_IF_LR:
                    interface_config->interface_type = phymodInterfaceLR;
                break;
                case SOC_PORT_IF_SR:
                    interface_config->interface_type = phymodInterfaceSR;
                break;
                case SOC_PORT_IF_XFI:
                    interface_config->interface_type = phymodInterfaceXFI;
                break;
                case SOC_PORT_IF_CR:
                    interface_config->interface_type = phymodInterfaceCR;
                break;
                case SOC_PORT_IF_LRM:
                    interface_config->interface_type = phymodInterfaceLRM;
                break;
                case SOC_PORT_IF_ZR:
                    interface_config->interface_type = phymodInterfaceZR;
                break;


                default:
                    return SOC_E_PARAM;
                    
            }    
        break;
        case PHY82780_SPD_1G:
            switch(speed_config->line_interface) {
                case SOC_PORT_IF_KX:
                    interface_config->interface_type = phymodInterfaceKX;
                break;
                case SOC_PORT_IF_SR:
                case SOC_PORT_IF_LR:
                    interface_config->interface_type = phymodInterfaceSR;
                break;
                default:
                    return SOC_E_PARAM;
                    
            }    
        break ;
        case PHY82780_SPD_40G:
        case PHY82780_SPD_42G:
            switch(speed_config->line_interface) {
                case SOC_PORT_IF_KR4:
                    interface_config->interface_type = phymodInterfaceKR4;
                break;
                case SOC_PORT_IF_LR4:
                    interface_config->interface_type = phymodInterfaceLR4;
                break;
                case SOC_PORT_IF_SR4:
                    interface_config->interface_type = phymodInterfaceSR4;
                break;
                case SOC_PORT_IF_CR4:
                    interface_config->interface_type = phymodInterfaceCR4;
                break;
                case SOC_PORT_IF_XLAUI:
                    interface_config->interface_type = phymodInterfaceXLAUI;
                break;

                default:
                    return SOC_E_PARAM;
                    
            }    
        break;
        default:
            return SOC_E_PARAM;
    }

    switch (speed_config->port_refclk_int) {
        case 156:
            interface_config->ref_clock = phymodRefClk156Mhz;
        break;
        case 155:
            interface_config->ref_clock = phymodRefClk155Mhz;
        break;

        default:
            return SOC_E_PARAM;
    }
    if (speed_config->higig_port) {
        PHYMOD_INTF_MODES_HIGIG_SET(interface_config);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy82780_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      speed - link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy82780_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t                *pc;
    phy_ctrl_t                *int_pc;
    phy82780_config_t             *pCfg;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       idx;
    phy82780_speed_config_t       speed_config;
    phymod_phy_inf_config_t   interface_config;
    int int_speed;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL || int_pc == NULL) {
        return SOC_E_INTERNAL;
    }

    if (speed == 0) {
        return SOC_E_NONE; 
    }
    if (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_HURRICANE3) {
        SOC_IF_ERROR_RETURN(PHY_SPEED_SET(int_pc->pd, unit, port, speed));
        SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, &int_speed));
        if (int_speed != speed) {
            return SOC_E_CONFIG;
        }
    } else {
        SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, &int_speed));
        if (int_speed != speed) 
        {
           SOC_IF_ERROR_RETURN(PHY_SPEED_SET(int_pc->pd, unit, port, speed));
           SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, &int_speed));
           if (int_speed != speed) {
               return SOC_E_CONFIG;
           }
        }
    }
    pmc = &pc->phymod_ctrl;
    pCfg = (phy82780_config_t *) pc->driver_data;

    /* take a copy of core and phy configuration values, and set the desired speed */
    sal_memcpy(&speed_config, &pCfg->speed_config, sizeof(speed_config));
    speed_config.speed = speed;
    
    /* determine the interface configuration */
    SOC_IF_ERROR_RETURN
        (phy82780_speed_to_interface_config_get(&speed_config, &interface_config));

    /* now loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        phy = pmc->phy[idx];
        if (phy == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN
            (phymod_phy_interface_config_set(&phy->pm_phy,
                                             0 /* flags */, &interface_config));
    }

    /* record success */
    pCfg->speed_config.speed = speed;
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy82780_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      speed - current link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy82780_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_phy_inf_config_t   interface_config;
    phymod_ref_clk_t ref_clock;
    int flag = 0;
    ref_clock = 0;
    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;

    /* initialize the data structure */
    interface_config.data_rate = 0;

    /* now loop through all cores */
    phy = pmc->phy[pmc->main_phy];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }
    /* note that the flags have an option to indicate whether it's ok to reprogram the PLL */
    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_get(&phy->pm_phy, flag,
                                         ref_clock, 
                                         &interface_config));

    *speed = interface_config.data_rate;
    return (SOC_E_NONE);
}

STATIC int
phy82780_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    phy_ctrl_t      *pc;
    phy82780_config_t *pCfg;
    phy82780_speed_config_t       speed_config;
    phymod_phy_inf_config_t   interface_config;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       idx;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pCfg = (phy82780_config_t *) pc->driver_data;
    pmc = &pc->phymod_ctrl;

    if (pif >= SOC_PORT_IF_COUNT) {
        return SOC_E_PARAM;
    }
    if (pif == SOC_PORT_IF_MII || pif == SOC_PORT_IF_XGMII || pif == SOC_PORT_IF_GMII) {
		return phy_null_interface_set(unit, port, pif);
	}


    /* take a copy of core and phy configuration values, and set the desired speed */
    sal_memcpy(&speed_config, &pCfg->speed_config, sizeof(speed_config));
    speed_config.line_interface = pif;
    SOC_IF_ERROR_RETURN
        (phy82780_speed_to_interface_config_get(&speed_config, &interface_config));

    /* now loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        phy = pmc->phy[idx];
        if (phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN
            (phymod_phy_interface_config_set(&phy->pm_phy,
                                         0 /* flags */, &interface_config));
    }

    pCfg->speed_config.line_interface = pif;
    return SOC_E_NONE;
}

STATIC int
phy82780_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_phy_inf_config_t   interface_config;
    phymod_ref_clk_t ref_clock;
    int flag = 0;
    ref_clock = 0;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;

    /* initialize the data structure */
    sal_memset(&interface_config, 0x0, sizeof(phymod_phy_inf_config_t));

    /* now loop through all cores */
    phy = pmc->phy[pmc->main_phy];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }
    /* note that the flags have an option to indicate whether it's ok to reprogram the PLL */
    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_get(&phy->pm_phy, ref_clock,
                                         flag, &interface_config));
    switch (interface_config.interface_type) {
        case phymodInterfaceKX:
            *pif = SOC_PORT_IF_KX;
        break;
        case phymodInterfaceKR:
            *pif = SOC_PORT_IF_KR; 
        break;
        case phymodInterfaceXFI:
            *pif = SOC_PORT_IF_XFI; 
        break;
        case phymodInterfaceXLAUI:
            *pif = SOC_PORT_IF_XLAUI; 
        break;
        case phymodInterfaceKR4:
            *pif = SOC_PORT_IF_KR4;
        break;
        case phymodInterfaceCR4:
            *pif = SOC_PORT_IF_CR4;
        break;
        case phymodInterfaceSR:
            *pif = SOC_PORT_IF_SR;
        break;
        case phymodInterfaceLR:
            *pif = SOC_PORT_IF_LR; 
        break;
        case phymodInterfaceSR4:
            *pif = SOC_PORT_IF_SR4;
        break;
        case phymodInterfaceLR4:
            *pif = SOC_PORT_IF_LR4;
        break;
        case phymodInterfaceCR:
            *pif = SOC_PORT_IF_CR;
        break;
        case phymodInterfaceLRM:
            *pif = SOC_PORT_IF_LRM;
        break;
        case phymodInterfaceZR:
            *pif = SOC_PORT_IF_ZR;
        break;

        default:
        break;
    }
    return (SOC_E_NONE);

}

/*
 * Function:
 *      phy82780_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      unit         - BCM unit number
 *      port         - Port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy82780_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_reg_data)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx = 0;
    uint8 data8 = 0;


    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    idx = pmc->main_phy ;
    pm_phy = &pmc->phy[idx]->pm_phy;

    if (flags & SOC_PHY_I2C_DATA8) {
        SOC_IF_ERROR_RETURN
            (phymod_phy_i2c_read(pm_phy, 0, SOC_PHY_I2C_DEVAD(phy_reg_addr), SOC_PHY_I2C_REGAD(phy_reg_addr), 1, &data8)); 
        *phy_reg_data = data8; 
    } else {
        SOC_IF_ERROR_RETURN
            (phymod_phy_reg_read(pm_phy, phy_reg_addr, phy_reg_data));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy82780_reg_write
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
phy82780_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_reg_data)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx;
    uint8 data8 = 0;
    data8 = phy_reg_data & 0xFF;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (flags & SOC_PHY_I2C_DATA8) {
            SOC_IF_ERROR_RETURN
                (phymod_phy_i2c_write(pm_phy, 0, SOC_PHY_I2C_DEVAD(phy_reg_addr), SOC_PHY_I2C_REGAD(phy_reg_addr), 1, &data8)); 
        } else {
            SOC_IF_ERROR_RETURN
                (phymod_phy_reg_write(pm_phy, phy_reg_addr, phy_reg_data));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy82780_reg_modify
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
 *      This function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy82780_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    uint32 data32, tmp;
    int idx;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        SOC_IF_ERROR_RETURN
            (phymod_phy_reg_read(pm_phy, phy_reg_addr, &data32));
        tmp = data32;
        data32 &= ~(phy_data_mask);
        data32 |= (phy_data & phy_data_mask);
        if (data32 != tmp) {
            SOC_IF_ERROR_RETURN
                (phymod_phy_reg_write(pm_phy, phy_reg_addr, data32));
        }
    }
    return SOC_E_NONE;
}

STATIC void
phy82780_core_init(phy_ctrl_t *pc, soc_phymod_core_t *core,
                   phymod_bus_t *core_bus, uint32 core_addr)
{
    phymod_core_access_t *pm_core;
    phymod_access_t *pm_acc;

    core->unit = pc->unit;
    core->port = pc->port;
    core->read = pc->read;
    core->write = pc->write;
    core->wrmask = pc->wrmask;

    pm_core = &core->pm_core;
    phymod_core_access_t_init(pm_core);
    pm_acc = &pm_core->access;
    phymod_access_t_init(pm_acc);
    PHYMOD_ACC_USER_ACC(pm_acc) = core;
    PHYMOD_ACC_BUS(pm_acc) = core_bus;
    PHYMOD_ACC_ADDR(pm_acc) = core_addr;

    PHYMOD_ACC_F_CLAUSE45_SET(pm_acc);

    return;
}

STATIC int
_phy82780_device_create_attach(soc_phymod_core_t *core, uint32_t core_id) 
{
    phy82780_device_aux_mode_t *new_device ;
    new_device = sal_alloc(sizeof(phy82780_device_aux_mode_t), "phy82780_device_aux_mode");
    if (new_device == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(new_device, 0 ,sizeof(phy82780_device_aux_mode_t));
    
    new_device->core_id = core_id ;
    core->device_aux_modes = (void *)new_device ;
    return SOC_E_NONE;
}

STATIC int
_phy82780_device_destroy(phy82780_device_aux_mode_t *device)
{
    if (device == NULL) {
        return SOC_E_PARAM;
    }
    sal_free(device);

    return SOC_E_NONE;    
}

STATIC void
phy82780_cleanup(soc_phymod_ctrl_t *pmc)
{
    int idx;
    soc_phymod_phy_t *phy;
    soc_phymod_core_t *core;
    phy82780_device_aux_mode_t *device ;

    for (idx = 0; idx < pmc->num_phys ; idx++) {
        phy = pmc->phy[idx];
        if (phy == NULL) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(pmc->unit,
                                 "phy object is empty")));
            continue;
        }

        core = phy->core;
        if (core != NULL) {
            device = (phy82780_device_aux_mode_t *)core->device_aux_modes ;
        }
        /* Destroy core object if not used anymore */
        if (core && core->ref_cnt) {
            if (--core->ref_cnt == 0) {
                PHYMOD_VDBG(PHY82780_DBG_MEM, NULL,("clean_up device=%p core_p=%p\n", (void *)device, (void *)core)) ;
                _phy82780_device_destroy(device) ;
                soc_phymod_core_destroy(pmc->unit, core);
            }
        }

        /* Destroy phy object */
        if (phy) {
            PHYMOD_VDBG(PHY82780_DBG_MEM, NULL,("clean_up phy=%p\n", (void *)phy)) ;
            soc_phymod_phy_destroy(pmc->unit, phy);
        }
    }
    pmc->num_phys = 0;
}

STATIC int32
_phy_82780_chip_id_get(phymod_phy_access_t *pm_phy, uint32 *chip_id)
{
    uint32 chip_id_msb = 0, chip_id_lsb = 0;
    

    SOC_IF_ERROR_RETURN(
       phymod_phy_reg_read(pm_phy, 0xc803, &chip_id_msb));

    chip_id_msb = (chip_id_msb & 0xF);
 
    SOC_IF_ERROR_RETURN(
       phymod_phy_reg_read(pm_phy, 0xc802, &chip_id_lsb));
    if (chip_id_msb == 0x8) {
        if (chip_id_lsb == 0x2780) {
            *chip_id = 0x82780;
        } else if(chip_id_lsb == 0x2752) {
            *chip_id = 0x82752;
        } else if(chip_id_lsb == 0x2758) {
            *chip_id = 0x82758;
        } else if(chip_id_lsb == 0x278F) {
            *chip_id = 0x8278F;
        }
    } else {
        LOG_WARN(BSL_LS_SOC_PHY, (BSL_META_U(0,
                 "Chip ID not found \n")));
        return SOC_E_NOT_FOUND;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     _phy82780_is_write_disabled
 * Doc:
 *      register write is disable in wormboot
 * Parameters:
 *      unit            - (IN)  unit number
 *      val             - (IN)  is write disable
 */
STATIC int
_phy82780_is_write_disabled(void* user_acc,  uint32_t *val)
{
    #if defined(BCM_WARM_BOOT_SUPPORT)
    soc_phymod_core_t *core = (soc_phymod_core_t *)user_acc;
    int unitV;
    unitV = core->unit;
    *val = SOC_WARM_BOOT(unitV);
    #else
    *val = 0;
    #endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy82780_probe
 * Purpose:
 *      xx
 * Parameters:
 *      pc->phy_id   (IN)
 *      pc->unit     (IN)
 *      pc->port     (IN)
 *      pc->size     (OUT) - memory required by phy port
 *      pc->dev_name (OUT) - set to port device name
 *  
 * Note:
 */
STATIC int
phy82780_probe(int unit, phy_ctrl_t *pc)
{
    int rv, idx;
    uint32 lane_map, num_phys, core_id, phy_id, found;
    phymod_bus_t core_bus;
    phymod_dispatch_type_t phy_type;
    phymod_core_access_t *pm_core;
    phymod_phy_access_t *pm_phy;
    phymod_access_t *pm_acc;
    soc_phymod_ctrl_t *pmc;
    soc_phymod_phy_t *phy;
    soc_phymod_core_t *core;
    soc_phymod_core_t core_probe;
    soc_info_t *si;
    phyident_core_info_t core_info[8];  
    int port;
    int phy_port;  /* physical port number */
    /* Initialize PHY bus */
    SOC_IF_ERROR_RETURN(phymod_bus_t_init(&core_bus));
    core_bus.bus_name = "phy82780"; 
    core_bus.read = _phy82780_reg_read; 
    core_bus.write = _phy82780_reg_write;
    core_bus.is_write_disabled = _phy82780_is_write_disabled;
#ifdef PORTMOD_SUPPORT
    /* If portmod feature is enabled let the 
     * PM take care of phy control*/

    if (SOC_USE_PORTCTRL(unit)) {
        return SOC_E_NOT_FOUND;
    }
#endif/* PORTMOD_SUPPORT */      

    /* Configure PHY bus properties */
    if (pc->wrmask) {
        PHYMOD_BUS_CAP_WR_MODIFY_SET(&core_bus);
        PHYMOD_BUS_CAP_LANE_CTRL_SET(&core_bus);
    }

    port = pc->port;
    pmc = &pc->phymod_ctrl;
    si = &SOC_INFO(unit);
    if (soc_feature(unit, soc_feature_logical_port_num)) {
        phy_port = si->port_l2p_mapping[port];
    } else {
        phy_port = port;
    }

    /* Install clean up function */
    pmc->unit = pc->unit;
    pmc->cleanup = phy82780_cleanup;
    /*pmc->symbols = &bcmi_tscf_xgxs_symbols;*/ 

    
    pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
    pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port));

    /* request memory for the configuration structure */
    pc->size = sizeof(phy82780_config_t);

    /* Bit N corresponds to lane N in lane_map */
    lane_map = 0xf;
    num_phys = 1;
    switch (si->port_num_lanes[port]) {
        case 10:
            pc->phy_mode = PHYCTRL_MULTI_CORE_PORT; 
            break;
        case 4:
            pc->phy_mode = PHYCTRL_QUAD_LANE_PORT; 
            break;
        case 2:
            lane_map = 0x3;
            pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
            break;
        case 1:
        case 0:
            lane_map = 0x1;
            pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
            break;
        default:
            return SOC_E_CONFIG;
    }
    /*lane_map <<= pc->lane_num;*/

    core_info[0].mdio_addr = pc->phy_id;
    phy_type = phymodDispatchTypeQuadra28;

    /* Probe cores */
    for (idx = 0; idx < num_phys ; idx++) {
        phy82780_core_init(pc, &core_probe, &core_bus,
                           core_info[idx].mdio_addr);
        /* Check that core is indeed an quadra28 core */
        pm_core = &core_probe.pm_core;
        pm_core->type = phy_type;
        rv = phymod_core_identify(pm_core, 0, &found);
        if (SOC_FAILURE(rv)) {
            LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "port %d: ERROR!!!\n"),
                  pc->port));

            return rv;
        }
        if (!found) {
            LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "port %d: NOT FOUND!!\n"),
                  pc->port));

            return SOC_E_NOT_FOUND;
        }
    }

    rv = SOC_E_NONE;
    for (idx = 0; idx < num_phys ; idx++) {
        /* Set core and phy IDs based on PHY address */
        core_id = pc->phy_id + idx;
        phy_id = (lane_map << 16) | core_id;

        /* Create PHY object */
        rv = soc_phymod_phy_create(unit, phy_id, &pmc->phy[idx]);
        if (SOC_FAILURE(rv)) {
            break;
        }
        pmc->num_phys++;

        /* Initialize phy object */
        phy = pmc->phy[idx];
        pm_phy = &phy->pm_phy;
        phymod_phy_access_t_init(pm_phy);

        /* Find or create associated core object */
        rv = soc_phymod_core_find_by_id(unit, core_id, &phy->core);
        if (rv == SOC_E_NOT_FOUND) {
            rv = soc_phymod_core_create(unit, core_id, &phy->core);
            rv |= _phy82780_device_create_attach(phy->core, core_id) ;
        }
        if (SOC_FAILURE(rv)) {
            break;
        }        
    }
    if (SOC_FAILURE(rv)) {
        phy82780_cleanup(pmc);
        return rv;
    }

    for (idx = 0; idx < num_phys ; idx++) {
        phy = pmc->phy[idx];
        core = phy->core;
        pm_core = &core->pm_core;

        /* Initialize core object if newly created */
        if (core->ref_cnt == 0) {
            sal_memcpy(&core->pm_bus, &core_bus, sizeof(core->pm_bus));
            phy82780_core_init(pc, core, &core->pm_bus,
                               core_info[idx].mdio_addr);
            /* Set dispatch type */
            pm_core->type = phy_type;
        }
        core->ref_cnt++;        

        /* Initialize phy access based on associated core */
        pm_acc = &phy->pm_phy.access;
        sal_memcpy(pm_acc, &pm_core->access, sizeof(*pm_acc));
        phy->pm_phy.type = phy_type;
        PHYMOD_ACC_LANE_MASK(pm_acc) = lane_map;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy82780_config_init
 * Purpose:     
 *      Determine phy configuration data for purposes of PHYMOD initialization.
 * 
 *      A side effect of this procedure is to save some per-logical port
 *      information in (phy82780_cfg_t *) &pc->driver_data;
 *
 * Parameters:
 *      unit                  - BCM unit number.
 *      port                  - Port number.
 *      logical_lane_offset   - starting logical lane number
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy82780_config_init(int unit, soc_port_t port, int logical_lane_offset,
                 phymod_core_init_config_t *core_init_config, 
                 phymod_phy_init_config_t  *pm_phy_init_config)
{
    phy_ctrl_t *pc;
    phy82780_speed_config_t *speed_config;
    phy82780_config_t *pCfg;
    phymod_firmware_load_method_t fw_ld_method;
    int port_num_lanes;
    int core_num;
    int phy_num_lanes;
    int phy_speed_max;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pCfg = (phy82780_config_t *) pc->driver_data;

    port_num_lanes = SOC_INFO(unit).port_num_lanes[port];

    /* figure out how many lanes are in this phy */
    core_num = (logical_lane_offset / 4);
    phy_num_lanes = (port_num_lanes - logical_lane_offset);
    if (phy_num_lanes > MAX_NUM_LANES) {
       phy_num_lanes = MAX_NUM_LANES;
    }
    
    /*CORE configuration*/
    phymod_core_init_config_t_init(core_init_config);
    fw_ld_method = phymodFirmwareLoadMethodInternal;
    if (soc_property_port_get(pc->unit, pc->port, "82780_glue", 0)) {
        fw_ld_method = phymodFirmwareLoadMethodNone;
    }
    core_init_config->flags = PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY;
    core_init_config->firmware_load_method  = soc_property_port_get(unit, port, 
                                              spn_LOAD_FIRMWARE, fw_ld_method);
    core_init_config->firmware_load_method &= 0xff; /* clear checksum bits */
    core_init_config->lane_map.num_of_lanes = NUM_LANES;
    
    speed_config = &(pCfg->speed_config);
    speed_config->port_refclk_int  = 156;
    speed_config->port_num_lanes   = phy_num_lanes;
    phy_speed_max = soc_property_port_get(unit, port, spn_PHY_INIT_SPEED, pc->speed_max); 

    if (port_num_lanes == 1) {
        if (phy_speed_max == PHY82780_SPD_10G) {
            speed_config->line_interface =  SOC_PORT_IF_SR;
            speed_config->speed = PHY82780_SPD_10G;
        } else if (phy_speed_max == PHY82780_SPD_1G) {
            speed_config->line_interface =  SOC_PORT_IF_SR;
            speed_config->speed = PHY82780_SPD_1G;
        } else if (phy_speed_max == PHY82780_SPD_11G) {
            speed_config->line_interface =  SOC_PORT_IF_SR;
            speed_config->speed = PHY82780_SPD_11G;
        } else {
            speed_config->line_interface =  SOC_PORT_IF_SR;
            speed_config->speed = PHY82780_SPD_10G;
        }
    } else if (port_num_lanes == 4) {
        if (phy_speed_max == PHY82780_SPD_40G) {
           speed_config->speed = PHY82780_SPD_40G;
           speed_config->line_interface =  SOC_PORT_IF_SR4;
        } else if (phy_speed_max == PHY82780_SPD_42G) {
            speed_config->line_interface =  SOC_PORT_IF_SR4;
            speed_config->speed = PHY82780_SPD_42G;
        } else {
            speed_config->line_interface =  SOC_PORT_IF_SR4;
            speed_config->speed = PHY82780_SPD_40G;
        }
    } else {
        LOG_CLI((BSL_META_U(unit,"Invalid number of lanes:%d\n "), port_num_lanes));
        return SOC_E_CONFIG;
    }

    /* PHY configuration */
    phymod_phy_init_config_t_init(pm_phy_init_config);

    pm_phy_init_config->polarity.rx_polarity 
                                 = soc_property_port_get(unit, port,
                                   spn_PHY_RX_POLARITY_FLIP,
                                   0xFFFF);
    pm_phy_init_config->polarity.tx_polarity 
                                 = soc_property_port_get(unit, port,
                                   spn_PHY_TX_POLARITY_FLIP,
                                   0xFFFF);

        
    /* phy_ctrl_t configuration (LOGICAL PORT BASED)
     * Only do this once, for the first core of the logical port*/
    if (core_num == 0) {
        /* phy_mode, PHYCTRL_MDIO_ADDR_SHARE, PHY_FLAGS_INDEPENDENT_LANE */
        if (port_num_lanes == 4) {
            pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        } else if (port_num_lanes == 1) {
            pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        }
    }
        
    return SOC_E_NONE;
}

STATIC int
_phy_82780_init_pass1(int unit, soc_port_t port)
{
    phy_ctrl_t *pc; 
    soc_phymod_ctrl_t *pmc;
    soc_phymod_phy_t *phy = NULL;
    soc_phymod_core_t *core;
    phy82780_config_t *pCfg;
    phy82780_speed_config_t *speed_config;
    phymod_phy_inf_config_t interface_config;
    phymod_core_status_t core_status;
    int idx;
    int logical_lane_offset;
    soc_port_ability_t ability;
    soc_phy_info_t *pi;
    char *dev_name;
    int len = 0;
    uint32 chip_id = 0;
    int phy_force_dload = 0, dload_method = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pc->driver_data = (void*)(pc+1);
    pmc = &pc->phymod_ctrl;
    pCfg = (phy82780_config_t *) pc->driver_data;
    
    sal_memset(pCfg, 0, sizeof(*pCfg));
    speed_config = &(pCfg->speed_config);

    sal_memset(&ability, 0, sizeof(ability));

    dev_name = DEV_NAME(pc);

    phy = pmc->phy[0];

    SOC_IF_ERROR_RETURN(
         _phy_82780_chip_id_get(&phy->pm_phy, &chip_id));
    DEVID(pc) = chip_id;

    pi = &SOC_PHY_INFO(unit, port);
   
    /* Since Bcast is enabled so the device name is same for all the 
     * Q28, In pass 2 dev name will be overwritten*/
    sal_strncpy_s (dev_name, "Quarda28", PHY82780_CHIP_ID_SIZE);
    len = sal_strlen(dev_name);

    dev_name[len++] = '/';
    dev_name[len++] = pc->lane_num + '0';
    dev_name[len] = 0;
    pc->dev_name = dev_name;  /* string terminator */
    pi->phy_name = dev_name;

    /* Loop through all phymod phys that support this SDK phy */
    logical_lane_offset = 0;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        phy = pmc->phy[idx];
        core = phy->core;

        pCfg->device_aux_modes = (phy82780_device_aux_mode_t *)(core->device_aux_modes) ;
        pCfg->device_aux_modes->repeater = soc_property_port_get(unit, port, spn_PHY_PCS_REPEATER, phymodOperationModeRepeater);
        pCfg->device_aux_modes->ull_datapath = soc_property_port_get(unit, port,
                                          spn_PHY_ULL_DATAPATH, phymodDatapathNormal);
        phy_force_dload = soc_property_port_get(unit, port,
                                          spn_PHY_FORCE_FIRMWARE_LOAD, 0x11);
#if defined (BCM_WARM_BOOT_SUPPORT)  /* Warmboot support */
        if ( SOC_WARM_BOOT(pc->unit) || SOC_IS_RELOADING(pc->unit) ) {
            dload_method = phymodFirmwareLoadMethodNone;
            core->init_config.firmware_load_method = phymodFirmwareLoadMethodNone;
        } else
#endif
        {
           dload_method = ((phy_force_dload & 0xF0)>>4);
            phy_force_dload &= 0xf;
        }
            if (!pCfg->device_aux_modes->repeater &&
                pCfg->device_aux_modes->ull_datapath) {

            LOG_CLI((BSL_META_U(unit,"Both ULL datapath and retimer cannot be enabled simultaneously,"
                        "so disabling ULL:%d\n "), port));
            pCfg->device_aux_modes->repeater = phymodOperationModeRetimer;
            pCfg->device_aux_modes->ull_datapath = phymodDatapathNormal;
        }
    
        /* determine configuration data structure to default values, based on SOC properties */
        SOC_IF_ERROR_RETURN
            (phy82780_config_init(unit, port,
                              logical_lane_offset,
                              &core->init_config, &phy->init_config));
        speed_config->higig_port = PBMP_MEMBER(PBMP_HG_ALL(unit), port);
        /* set the port to its max or init_speed */
        SOC_IF_ERROR_RETURN
             (phy82780_speed_to_interface_config_get(speed_config, &interface_config));
        sal_memcpy(&core->init_config.interface, &interface_config, sizeof(interface_config));
        core->init_config.interface.device_aux_modes = (void *)pCfg->device_aux_modes;

        core->init_config.op_datapath =  pCfg->device_aux_modes->ull_datapath;
        if (((DEVID(pc) == Q28_ID_82752) && ((pc->phy_id & 0x1) == 0)) || 
            ((pc->phy_id & 0x3) == 0))   {
             core_status.pmd_active = 0;
             if (dload_method == phymodFirmwareLoadMethodInternal && (phy_force_dload == phymodFirmwareLoadForce ||\
                              phy_force_dload == phymodFirmwareLoadAuto)) { 
                 core->init_config.firmware_load_method = phymodFirmwareLoadMethodInternal;
                 if(phy_force_dload == phymodFirmwareLoadForce)
                 {
                     PHYMOD_CORE_INIT_F_FW_FORCE_DOWNLOAD_SET(&(core->init_config));
                 }else if(phy_force_dload == phymodFirmwareLoadAuto){
                     PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_SET(&(core->init_config));
                 }
                 pc->flags |= PHYCTRL_MDIO_BCST;
             } else if (dload_method == 2 && (phy_force_dload == phymodFirmwareLoadForce ||\
                           phy_force_dload == phymodFirmwareLoadAuto)) {
                 core->init_config.firmware_load_method =  phymodFirmwareLoadMethodProgEEPROM;
                 SOC_IF_ERROR_RETURN (
                     phymod_core_init(&core->pm_core, &core->init_config, &core_status));
             } else if (dload_method == phymodFirmwareLoadMethodNone) {
                 phymod_core_firmware_info_t fw_info;
                 SOC_IF_ERROR_RETURN (
                     phymod_core_firmware_info_get(&core->pm_core, &fw_info));
                 if (fw_info.fw_version == 0) {
                     LOG_CLI((BSL_META_U(unit, "No FW found on the chip, Please use"
                                     "spn_PHY_FORCE_FIRMWARE_LOAD as 0x11 to download FW to chip")));
                     return SOC_E_UNAVAIL;
                 }
             }
             core->init = TRUE;
         }
         logical_lane_offset += core->init_config.lane_map.num_of_lanes;
    }

	/* indicate second pass of init is needed */
    PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);

    return SOC_E_NONE;
}
void phy82780_phyctrl_to_phymod_interface (uint16_t sys_if, phymod_interface_t *sys_intf)
{
    switch (sys_if)
    {
        case SOC_PORT_IF_XFI:
            *sys_intf = phymodInterfaceXFI; 
        break;
        case SOC_PORT_IF_KR:
            *sys_intf = phymodInterfaceKR; 
        break;
        case SOC_PORT_IF_KR4:
            *sys_intf = phymodInterfaceKR4; 
        break;
        case SOC_PORT_IF_CR:
            *sys_intf = phymodInterfaceCR; 
        break;
        case SOC_PORT_IF_CR4:
            *sys_intf = phymodInterfaceCR4;
        break;
        case SOC_PORT_IF_XLAUI:
            *sys_intf = phymodInterfaceXLAUI;
        break;
        case SOC_PORT_IF_SR:
            *sys_intf = phymodInterfaceSR;
        break;
        case SOC_PORT_IF_CAUI:
            *sys_intf = phymodInterfaceCAUI;
        break;
        case SOC_PORT_IF_LR:
            *sys_intf = phymodInterfaceLR;
        break;
        case SOC_PORT_IF_LR4:
            *sys_intf = phymodInterfaceLR4;
        break;
        case SOC_PORT_IF_SR4:
            *sys_intf = phymodInterfaceSR4;
        break;
        case SOC_PORT_IF_KX:
            *sys_intf = phymodInterfaceKX;
        break;
        case SOC_PORT_IF_XLAUI2:
            *sys_intf = phymodInterfaceXLAUI2;
        break;
        default:
            *sys_intf = phymodInterfaceXLAUI;
    }

}

#if defined (BCM_WARM_BOOT_SUPPORT)
void phy82780_phymod_to_phyctrl_interface(uint16_t sys_if, soc_port_if_t *sys_intf)
{
    switch (sys_if)
    {
        case  phymodInterfaceXFI:
            *sys_intf = SOC_PORT_IF_XFI;
        break;
        case phymodInterfaceKX:
            *sys_intf = SOC_PORT_IF_KX;
        break;
        case phymodInterfaceKR:
            *sys_intf = SOC_PORT_IF_KR;
        break;
        case phymodInterfaceKR4:
            *sys_intf = SOC_PORT_IF_KR4;
        break;
        case phymodInterfaceCR:
            *sys_intf = SOC_PORT_IF_CR;
        break;
        case phymodInterfaceCR4:
            *sys_intf = SOC_PORT_IF_CR4;
        break;
        case phymodInterfaceXLAUI:
            *sys_intf = SOC_PORT_IF_XLAUI;
        break;
        case phymodInterfaceSR:
            *sys_intf = SOC_PORT_IF_SR;
        break;
        case phymodInterfaceCAUI:
            *sys_intf = SOC_PORT_IF_CAUI;
        break;
        case phymodInterfaceLR:
            *sys_intf = SOC_PORT_IF_LR;
        break;
        case phymodInterfaceLR4:
            *sys_intf = SOC_PORT_IF_LR4;
        break;
        case phymodInterfaceSR4:
            *sys_intf = SOC_PORT_IF_SR4;
        break;
        case phymodInterfaceXLAUI2:
            *sys_intf = SOC_PORT_IF_XLAUI2;
        break;

        default:
            *sys_intf = 0;
    }

}
#endif

STATIC int
_phy_82780_init_pass2(int unit, soc_port_t port)
{
    phy_ctrl_t *pc; 
    soc_phymod_ctrl_t *pmc;
    soc_phymod_phy_t *phy = NULL;
    phy82780_config_t *pCfg;
    phy82780_speed_config_t *speed_config;
    phymod_phy_inf_config_t interface_config;
    soc_port_ability_t ability;
    uint16_t sys_if = 0;
    phymod_interface_t sys_intf = 0;
    uint16_t temp = 0;
    soc_phy_info_t *pi;
    char *dev_name;
    int len = 0;
#if defined (BCM_WARM_BOOT_SUPPORT)
    soc_port_if_t pif;
#endif

    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    dev_name = DEV_NAME(pc);
    sal_memset(&ability, 0, sizeof(ability));
    pc->driver_data = (void*)(pc+1);
    pmc = &pc->phymod_ctrl;
    pCfg = (phy82780_config_t *) pc->driver_data;
    
    pi = &SOC_PHY_INFO(unit, port);
    if (DEVID(pc) == Q28_ID_82780) { 
        /* coverity[secure_coding] */
        sal_strncpy_s (dev_name, "BCM82780", PHY82780_CHIP_ID_SIZE);
    } else if (DEVID(pc) == Q28_ID_82752) { 
        /* coverity[secure_coding] */
        sal_strncpy_s (dev_name, "BCM82752", PHY82780_CHIP_ID_SIZE);
    } else if (DEVID(pc) == Q28_ID_82758) {
        /* coverity[secure_coding] */
        sal_strncpy_s (dev_name, "BCM82758", PHY82780_CHIP_ID_SIZE);
    } else if (DEVID(pc) == Q28_ID_8278F) {
        /* coverity[secure_coding] */
        sal_strncpy_s (dev_name, "BCM8278F", PHY82780_CHIP_ID_SIZE);
    } else {
        /* coverity[secure_coding] */
        sal_strncpy_s (dev_name, "Quarda28", PHY82780_CHIP_ID_SIZE);
    }
    
    len = sal_strlen(dev_name);

    dev_name[len++] = '/';
    dev_name[len++] = pc->lane_num + '0';
    dev_name[len] = 0;
    pc->dev_name = dev_name;  /* string terminator */
    pi->phy_name = dev_name;

    speed_config = &(pCfg->speed_config);
    phy = pmc->phy[0];
    {
        soc_phymod_core_t *pmc_core;
        phymod_core_access_t *pm_core;
        phymod_core_status_t core_status;

        pmc_core = pmc->phy[0]->core;
        pm_core = &pmc_core->pm_core;
        PHYMOD_CORE_INIT_F_FW_LOAD_END_SET(&(pmc_core->init_config));
        pmc_core->init_config.op_datapath =  pCfg->device_aux_modes->ull_datapath;
        if (((DEVID(pc) == Q28_ID_82752) && ((pc->phy_id & 0x1) == 0)) || 
            ((pc->phy_id & 0x3) == 0))   {
            temp = soc_property_port_get(unit, port,
                                          spn_PHY_FORCE_FIRMWARE_LOAD, 0x11);
#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
            if ( SOC_WARM_BOOT(pc->unit) || SOC_IS_RELOADING(pc->unit) ) {
                /* In case of warm boot do not reinitialize */
            } else
#endif /* BCM_WARM_BOOT_SUPPORT */
            if (((temp & 0xf) == 1) && ((temp & 0xf0) == 0x10)) { 
                SOC_IF_ERROR_RETURN (
                    phymod_core_init(pm_core, &pmc_core->init_config, &core_status));
            }
        }

        SOC_IF_ERROR_RETURN
            (phy82780_speed_to_interface_config_get(speed_config, &interface_config));
        phy->init_config.interface.interface_type=interface_config.interface_type;
        phy->init_config.interface.data_rate=interface_config.data_rate;
        phy->init_config.op_mode = pCfg->device_aux_modes->repeater;
#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
        if ( SOC_WARM_BOOT(pc->unit) || SOC_IS_RELOADING(pc->unit) ) {
            /* In case of warm boot do not reinitialize */
        } else
#endif /* BCM_WARM_BOOT_SUPPORT */
        SOC_IF_ERROR_RETURN
            (phymod_phy_init(&phy->pm_phy, &phy->init_config));
    }
  
    SOC_IF_ERROR_RETURN
        (phy82780_speed_to_interface_config_get(speed_config, &interface_config));
#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
        if ( SOC_WARM_BOOT(pc->unit) || SOC_IS_RELOADING(pc->unit) ) {
            /* In case of warm boot do not reinitialize */
            phy82780_phymod_to_phyctrl_interface(interface_config.interface_type, &pif);
            pCfg->speed_config.line_interface = pif;
            pCfg->speed_config.speed=interface_config.data_rate;
        } else
#endif /* BCM_WARM_BOOT_SUPPORT */
    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_set(&phy->pm_phy,
                             0 /* flags */, &interface_config));
    /* Config Sys interface*/
    sys_if = soc_property_port_get(unit, port, spn_PHY_SYS_INTERFACE, 0);
    if (sys_if) {
        phy82780_phyctrl_to_phymod_interface(sys_if, &sys_intf);
        if (sys_intf == 0) {
            return SOC_E_PARAM;
        }
        PHY82780_SET_SYS_SIDE(phy->pm_phy);
        interface_config.interface_type = sys_intf;
#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
        if ( SOC_WARM_BOOT(pc->unit) || SOC_IS_RELOADING(pc->unit) ) {
            /* In case of warm boot do not reinitialize */
        } else
#endif /* BCM_WARM_BOOT_SUPPORT */
        SOC_IF_ERROR_RETURN
           (phymod_phy_interface_config_set(&phy->pm_phy,
                             0 /* flags */, &interface_config));

        PHY82780_CLEAR_SYS_SIDE(phy->pm_phy);
    }
    
    /* setup the port's an cap */
    SOC_IF_ERROR_RETURN
        (phy_82780_ability_local_get(unit, port, &ability));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy82780_init_pass2: u=%d p=%d\n"), unit, port));

    if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) {
        /* indicate third  pass of init is needed */
        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS3);
    }

    return SOC_E_NONE;
}

STATIC int
_phy_82780_init_pass3(int unit, soc_port_t port)
{
    phy_ctrl_t *pc; 
    phy82780_config_t *pCfg;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pc->driver_data = (void*)(pc+1);
    pCfg = (phy82780_config_t *) pc->driver_data;
    
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY82328 init pass3: u=%d p=%d\n"), unit, port));

	/* If configured, enable module auto detection */
	pCfg->auto_mod_detect = soc_property_port_get(unit, port, spn_PHY_MOD_AUTO_DETECT, 0);
	if (pCfg->auto_mod_detect) {
        /* Nothing added for now*/

	}

	return SOC_E_NONE;
}

/*
 * Function:
 *      phy82780_init
 *  
 *      An SDK "phy" is a logical port, which can consist of from 1-10 lanes,
 *          (A phy with more than 4 lanes requires more than one core).
 *      Per-logical port information is saved in &pc->driver_data.
 *      An SDK phy is implemented as one or more PHYMOD "phy"s.
 *  
 *      A PHYMOD "phy" resides completely within a single core, which can be
 *      from 1 to 4 lanes.
 *      Per-phymod phy information is kept in (soc_phymod_ctrl_t) *pc->phymod_ctrl
 *      A phymod phy points to its core.  Up to 4 phymod phys can be on a single core
 *  
 * Purpose:     
 *      Initialize a phy82780
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_82780_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
     
    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {
        PHY_FLAGS_SET(unit, port,  PHY_FLAGS_FIBER | PHY_FLAGS_C45 | PHY_FLAGS_REPEATER);
        SOC_IF_ERROR_RETURN(_phy_82780_init_pass1(unit, port));
        if (PHYCTRL_INIT_STATE(pc) != PHYCTRL_INIT_STATE_DEFAULT) {
            return SOC_E_NONE;
        }
    }
    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {
        SOC_IF_ERROR_RETURN( _phy_82780_init_pass2(unit, port));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "82780 init pass2 completed: u=%d p=%d\n"), unit, port));
        if (PHYCTRL_INIT_STATE(pc) != PHYCTRL_INIT_STATE_DEFAULT) {
            return SOC_E_NONE;
        }
    }

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS3) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        SOC_IF_ERROR_RETURN(_phy_82780_init_pass3(unit, port));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY82780 init pass3 completed: u=%d p=%d\n"), unit, port));
        PHYCTRL_INIT_STATE_SET(pc, PHYCTRL_INIT_STATE_DEFAULT);
        return SOC_E_NONE;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82780_ability_local_get
 * Purpose:
 *      xx
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      ability - xx
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_82780_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t          *pc; 
    uint32_t pa_speed = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    sal_memset(ability, 0, sizeof(*ability));
    if (pc->speed_max == PHY82780_SPD_40G) {
        pa_speed = SOC_PA_SPEED_40GB | SOC_PA_SPEED_1000MB
                 | SOC_PA_SPEED_10GB | SOC_PA_SPEED_11GB | SOC_PA_SPEED_42GB;
    }  else if (pc->speed_max == PHY82780_SPD_10G) {
        pa_speed = SOC_PA_SPEED_10GB | SOC_PA_SPEED_1000MB;
    } else {
        pa_speed = SOC_PA_SPEED_40GB | SOC_PA_SPEED_10GB | SOC_PA_SPEED_1000MB
                    | SOC_PA_SPEED_11GB | SOC_PA_SPEED_42GB;
    }

    ability->loopback  = SOC_PA_LB_PHY;
    ability->medium    = SOC_PA_MEDIUM_FIBER;
    ability->pause     = 0;
    ability->flags     = SOC_PA_AUTONEG;
    ability->speed_full_duplex = pa_speed;


    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy82780_ability_local_get:unit=%d p=%d sp=%08x\n"),
              unit, port, ability->speed_full_duplex));
    return (SOC_E_NONE);
}

int
phy_82780_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    phymod_phy_access_t       *pm_phy;
    phy_ctrl_t *int_pc;
    int32 int_phy_link = 0;
    *link = 0;
    int_pc = INT_PHY_SW_STATE(unit, port);
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[pmc->main_phy]->pm_phy;

    if (int_pc != NULL) {
		SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, &int_phy_link));
    }

    SOC_IF_ERROR_RETURN
        (phymod_phy_link_status_get(pm_phy, (uint32_t *) link));
    
    /* When PMD lock of 82780 is not Set, Leave the port link as
     * PMD status of 82780. When PMD lock of 82780 is set use
     * PCS status of internal serdes as port link status.
     * When internal serdes is not there use 82780 PMS as link status*/
#ifdef PHY82780_DEBUG    
    if (*link == 1 && int_phy_link == 0) {
        LOG_CLI((BSL_META_U(unit,"PMD UP PCS Down: port:%d\n "), port));
    }
    if (*link == 0 && int_phy_link == 1) {
        LOG_CLI((BSL_META_U(unit,"PMD DOWN PCS UP port:%d\n "), port));
    }
#endif
    if (*link && int_pc) {
        *link = int_phy_link;
    }
    return SOC_E_NONE;
}

STATIC int
phy_82780_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    int32 intf, idx;
    phy_ctrl_t                *int_pc;
    phymod_phy_access_t pm_phy_copy;
    phymod_phy_tx_lane_control_t tx_control;
    phymod_phy_rx_lane_control_t rx_control;
    /* locate phy control */
    int_pc = INT_PHY_SW_STATE(unit, port);
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82780_SYS_SIDE : PHY82780_LINE_SIDE; 

    pmc = &pc->phymod_ctrl;

    if (enable == 1) {
        tx_control = phymodTxSquelchOff;
        rx_control = phymodRxSquelchOff;
    } else {
        tx_control = phymodTxSquelchOn;
        rx_control = phymodRxSquelchOn;
    }
    for (idx = 0; idx < pmc->num_phys; idx++) {
        sal_memcpy(&pm_phy_copy, &pmc->phy[idx]->pm_phy, sizeof(pm_phy_copy));
        if (intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        }
        SOC_IF_ERROR_RETURN
                (phymod_phy_tx_lane_control_set(&pm_phy_copy, tx_control));
        SOC_IF_ERROR_RETURN
                (phymod_phy_rx_lane_control_set(&pm_phy_copy, rx_control));
    }

    SOC_IF_ERROR_RETURN(PHY_ENABLE_SET(int_pc->pd, unit, port, enable));

    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
	} else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    return SOC_E_NONE;
}

STATIC int
phy_82780_enable_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    int32 intf, idx;
    phymod_phy_access_t pm_phy_copy;
    phymod_phy_tx_lane_control_t tx_control;
    phymod_phy_rx_lane_control_t rx_control;

    *enable = 0;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82780_SYS_SIDE : PHY82780_LINE_SIDE; 

    for (idx = 0; idx < pmc->num_phys; idx++) {
        sal_memcpy(&pm_phy_copy, &pmc->phy[idx]->pm_phy, sizeof(pm_phy_copy));
        if (intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        }
        SOC_IF_ERROR_RETURN
                (phymod_phy_tx_lane_control_get(&pm_phy_copy, &tx_control));
        SOC_IF_ERROR_RETURN
                (phymod_phy_rx_lane_control_get(&pm_phy_copy, &rx_control));
    }
    if ((tx_control == phymodTxSquelchOff) && (rx_control == phymodRxSquelchOff)) {
        *enable = 1;
    } else {
        *enable = 0;
    }
    
    return SOC_E_NONE;
}

STATIC int
phy_82780_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t       *pm_phy;

    pc = EXT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[pmc->main_phy]->pm_phy;

    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(pm_phy, phymodLoopbackGlobal, enable));

    return SOC_E_NONE;
}

STATIC int
phy_82780_firmware_set(int unit, int port, int cmd, uint8 *array,int datalen)
{
    phy_ctrl_t                *pc;
    soc_phymod_core_t *pmc_core;
    phymod_core_access_t *pm_core;
    soc_phymod_ctrl_t *pmc;
    phymod_core_status_t core_status;
    
    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl; 

    pmc_core = pmc->phy[0]->core;
    pm_core = &pmc_core->pm_core;
    if (pm_core == NULL) {
        return SOC_E_INTERNAL;
    }
    if (array == NULL) {
        if (cmd == PHYCTRL_UCODE_BCST_SETUP) {
            PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_SET(&(pmc_core->init_config));
            SOC_IF_ERROR_RETURN (
              phymod_core_init(pm_core, &pmc_core->init_config, &core_status));
            PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_CLR(&(pmc_core->init_config));
            return SOC_E_NONE;
        } else if (cmd == PHYCTRL_UCODE_BCST_uC_SETUP) {
            return SOC_E_NONE;
        } else if (cmd == PHYCTRL_UCODE_BCST_ENABLE) {
            return SOC_E_NONE;
        } else if (cmd == PHYCTRL_UCODE_BCST_LOAD) {
            PHYMOD_CORE_INIT_F_EXECUTE_FW_LOAD_SET(&(pmc_core->init_config));
            SOC_IF_ERROR_RETURN (
              phymod_core_init(pm_core, &pmc_core->init_config, &core_status));
            PHYMOD_CORE_INIT_F_EXECUTE_FW_LOAD_CLR(&(pmc_core->init_config));
            return SOC_E_NONE;
        } else if (cmd == PHYCTRL_UCODE_BCST_END) {
            PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_SET(&(pmc_core->init_config));
            SOC_IF_ERROR_RETURN (
              phymod_core_init(pm_core, &pmc_core->init_config, &core_status));
            PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_CLR(&(pmc_core->init_config));
            return SOC_E_NONE;
        }
    } else {
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
   
}

/* 
 * phy_82780_per_lane_loopback_internal_pmd_get (this is the PMD global loopback)
 */
STATIC int 
phy_82780_per_lane_loopback_internal_pmd_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 


    SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(&pm_phy_copy, phymodLoopbackGlobalPMD, &enable));
    *value = enable;
    return(SOC_E_NONE);
}
/* 
 * phy_82780_loopback_internal_pmd_get (this is the PMD global loopback)
 */
STATIC int 
phy_82780_loopback_internal_pmd_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(&pm_phy_copy, phymodLoopbackGlobalPMD, &enable));
    *value = enable;
    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_loopback_remote_get
 */
STATIC int 
phy_82780_per_lane_loopback_remote_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(&pm_phy_copy, phymodLoopbackRemotePMD, &enable));
    *value = enable;
    return(SOC_E_NONE);
}

/* 
 * phy_82780_loopback_remote_get
 */
STATIC int 
phy_82780_loopback_remote_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(&pm_phy_copy, phymodLoopbackRemotePMD, &enable));
    *value = enable;
    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_prbs_tx_poly_get
 */
STATIC int
phy_82780_per_lane_prbs_tx_poly_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;


    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));

    *value = (int) prbs.poly;

    /* convert from PHYMOD enum to SDK enum */
    switch(prbs.poly){
    case phymodPrbsPoly7:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1;
        break;
    case phymodPrbsPoly9:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1;
        break;
    case phymodPrbsPoly15:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1;
        break;
    case phymodPrbsPoly23:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1;
        break;
    case phymodPrbsPoly31:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1;
        break;
    case phymodPrbsPoly11:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1;
        break;
    case phymodPrbsPoly58:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1; 
        break;
    default:
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}
/* 
 * phy_82780_prbs_tx_poly_get
 */
STATIC int
phy_82780_prbs_tx_poly_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));

    *value = (int) prbs.poly;

    /* convert from PHYMOD enum to SDK enum */
    switch(prbs.poly){
    case phymodPrbsPoly7:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1;
        break;
    case phymodPrbsPoly9:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1;
        break;
    case phymodPrbsPoly15:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1;
        break;
    case phymodPrbsPoly23:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1;
        break;
    case phymodPrbsPoly31:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1;
        break;
    case phymodPrbsPoly11:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1;
        break;
    case phymodPrbsPoly58:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1; 
        break;
    default:
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}
/* 
 * phy_82780_per_lane_prbs_rx_poly_get
 */
STATIC int
phy_82780_per_lane_prbs_rx_poly_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;


    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));

    *value = (int) prbs.poly;

    /* convert from PHYMOD enum to SDK enum */
    switch(prbs.poly){
    case phymodPrbsPoly7:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1;
        break;
    case phymodPrbsPoly9:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1;
        break;
    case phymodPrbsPoly15:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1;
        break;
    case phymodPrbsPoly23:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1;
        break;
    case phymodPrbsPoly31:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1;
        break;
    case phymodPrbsPoly11:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1;
        break;
    case phymodPrbsPoly58:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}
/* 
 * phy_82780_prbs_rx_poly_get
 */
STATIC int
phy_82780_prbs_rx_poly_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    phymod_phy_access_t pm_phy_copy;
    uint32_t flags = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));

    *value = (int) prbs.poly;

    /* convert from PHYMOD enum to SDK enum */
    switch(prbs.poly){
    case phymodPrbsPoly7:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1;
        break;
    case phymodPrbsPoly9:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1;
        break;
    case phymodPrbsPoly15:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1;
        break;
    case phymodPrbsPoly23:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1;
        break;
    case phymodPrbsPoly31:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1;
        break;
    case phymodPrbsPoly11:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1;
        break;
    case phymodPrbsPoly58:
        *value = SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

/* 
 * phy_82780_per_lane_prbs_tx_invert_data_get
 */
STATIC int
phy_82780_per_lane_prbs_tx_invert_data_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;

    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));
    *value = prbs.invert;

    return(SOC_E_NONE);
}
/* 
 * phy_82780_prbs_tx_invert_data_get
 */
STATIC int
phy_82780_prbs_tx_invert_data_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));
    *value = prbs.invert;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_prbs_rx_invert_data_get
 */
STATIC int
phy_82780_per_lane_prbs_rx_invert_data_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));
    *value = prbs.invert;

    return(SOC_E_NONE);
}
/* 
 * phy_82780_prbs_rx_invert_data_get
 */
STATIC int
phy_82780_prbs_rx_invert_data_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));
    *value = prbs.invert;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_prbs_tx_enable_get
 */
STATIC int
phy_82780_per_lane_prbs_tx_enable_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_get(&pm_phy_copy, flags, value));

    return(SOC_E_NONE);
}

/* 
 * phy_82780_prbs_tx_enable_get
 */
STATIC int
phy_82780_prbs_tx_enable_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_get(&pm_phy_copy, flags, value));

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_prbs_rx_enable_get
 */
STATIC int
phy_82780_per_lane_prbs_rx_enable_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_get(&pm_phy_copy, flags, value));

    return(SOC_E_NONE);
}
/* 
 * phy_82780_prbs_rx_enable_get
 */
STATIC int
phy_82780_prbs_rx_enable_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_get(&pm_phy_copy, flags, value));

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_prbs_rx_status_get
 */
STATIC int
phy_82780_per_lane_prbs_rx_status_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    phymod_prbs_status_t   prbs_tmp;
    int prbs_lock, lock_loss, error_count ;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    prbs_lock   = 1 ; 
    lock_loss   = 0 ;
    error_count = 0 ;
    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    /* $$$ Need to add diagnostic API */
    SOC_IF_ERROR_RETURN
        (phymod_phy_prbs_status_get(&pm_phy_copy, 0, &prbs_tmp));

    if(prbs_tmp.prbs_lock==0) {
        prbs_lock = 0 ;
    } else {
        if(prbs_tmp.prbs_lock_loss) {
            lock_loss = 1 ;
        } else {
            error_count += prbs_tmp.error_count;
        }
    }
    
    if (prbs_lock == 0) {
        *value = -1;
    } else if ((lock_loss == 1) && (prbs_lock == 1)) {
        *value = -2;
    } else {
        *value = error_count;
    }
    return(SOC_E_NONE);
}
/* 
 * phy_82780_prbs_rx_status_get
 */
STATIC int
phy_82780_prbs_rx_status_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    phymod_prbs_status_t   prbs_tmp;
    int idx, prbs_lock, lock_loss, error_count ;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }

    prbs_lock   = 1 ; 
    lock_loss   = 0 ;
    error_count = 0 ;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 
        /* $$$ Need to add diagnostic API */
        SOC_IF_ERROR_RETURN
            (phymod_phy_prbs_status_get(&pm_phy_copy, 0, &prbs_tmp));

        if(prbs_tmp.prbs_lock==0) {
            prbs_lock = 0 ;
        } else {
            if(prbs_tmp.prbs_lock_loss) {
                lock_loss = 1 ;
            } else {
                error_count += prbs_tmp.error_count;
            }
        }
    }
    
    if (prbs_lock == 0) {
        *value = -1;
    } else if ((lock_loss == 1) && (prbs_lock == 1)) {
        *value = -2;
    } else {
        *value = error_count;
    }
    return(SOC_E_NONE);
}
/* 
 * phy_82780_per_lane_rx_peak_filter_get
 */
STATIC int
phy_82780_per_lane_rx_peak_filter_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
        *value = phymod_rx.peaking_filter.value;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_rx_vga_get
 */
STATIC int
phy_82780_per_lane_rx_vga_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
        *value = phymod_rx.vga.value;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_rx_dfe_tap_control_get
 */
STATIC int
phy_82780_per_lane_rx_dfe_tap_control_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, int tap, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t          phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    if (tap < 0 || tap >= COUNTOF(phymod_rx.dfe)) {
        /* this can only happen with a coding error */
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    *value = phymod_rx.dfe[tap].value;

    return(SOC_E_NONE);
}


/* 
 * phy_82780_per_lane_rx_low_freq_filter_get
 */
STATIC int
phy_82780_per_lane_rx_low_freq_filter_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t          phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    *value = phymod_rx.low_freq_peaking_filter.value;
   return(SOC_E_NONE);
}

/* 
 * phy_82780_rx_peak_filter_get
 */
STATIC int 
phy_82780_rx_peak_filter_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_rx_t          phymod_rx;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    *value = phymod_rx.peaking_filter.value;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_rx_low_peak_filter_get
 */
STATIC int 
phy_82780_rx_low_peak_filter_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_rx_t          phymod_rx;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    *value = phymod_rx.low_freq_peaking_filter.value;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_rx_vga_set
 */
STATIC int 
phy_82780_rx_vga_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_rx_t          phymod_rx;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    *value = phymod_rx.vga.value;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_rx_tap_get
 */
STATIC int 
phy_82780_rx_tap_get(soc_phymod_ctrl_t *pmc, int32 intf, int tap, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_rx_t          phymod_rx;
    phymod_phy_access_t pm_phy_copy;

    if (tap < 0 || tap >= COUNTOF(phymod_rx.dfe)) {
        /* this can only happen with a coding error */
        return SOC_E_INTERNAL;
    }
    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    *value = phymod_rx.dfe[tap].value;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_pi_control_get
 */
STATIC int 
phy_82780_pi_control_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_tx_override_t tx_override;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_override_get(&pm_phy_copy, &tx_override));
    *value = tx_override.phase_interpolator.value;

    return(SOC_E_NONE);
}
/* 
 * phy_82780_rx_seq_done_get
 */
STATIC int
phy_82780_rx_seq_done_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
         return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_rx_pmd_locked_get(&pm_phy_copy, value));

    return(SOC_E_NONE);
}
/* 
 * phy_82780_fec_get
 */
STATIC int 
phy_82780_fec_enable_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_fec_enable_get(pm_phy,  &enable));
    *value = enable;

    return(SOC_E_NONE);
}
/* 
 * phy_82780_tx_fir_pre_get
 */
STATIC int
phy_82780_tx_fir_pre_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    phymod_phy_access_t pm_phy_copy;

    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.pre;

    return(SOC_E_NONE);
}
/* 
 * phy_82780_tx_fir_main_get
 */
STATIC int
phy_82780_tx_fir_main_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    phymod_phy_access_t pm_phy_copy;

    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.main;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_tx_fir_post_get
 */
STATIC int
phy_82780_tx_fir_post_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    phymod_phy_access_t pm_phy_copy;

    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.post;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_tx_fir_post2_get
 */
STATIC int
phy_82780_tx_fir_post2_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    phymod_phy_access_t pm_phy_copy;

    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(pm_phy, &phymod_tx));
    *value = phymod_tx.post2;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_driver_current_get
 */
STATIC int
phy_82780_per_lane_driver_current_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.amp;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_preemphasis_get
 */
STATIC int
phy_82780_per_lane_preemphasis_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    /* 0 to 7 = pre
     * 8 to 15 = main
     * 16 to 24 = post*/
    *value |= (phymod_tx.pre & 0xff);
    *value |= ((phymod_tx.main & 0xff) << 8);
    *value |= ((phymod_tx.post & 0xff) << 16);

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_rx_low_freq_filter_set
 */
STATIC int
phy_82780_per_lane_rx_low_freq_filter_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t          phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    phymod_rx.low_freq_peaking_filter.value = value;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(&pm_phy_copy, &phymod_rx));
   return(SOC_E_NONE);
}
/* 
 * phy_82780_per_lane_power_get
 */
STATIC int
phy_82780_per_lane_power_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_power_t      power;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    phymod_phy_power_t_init(&power);

    SOC_IF_ERROR_RETURN(phymod_phy_power_get(&pm_phy_copy, &power));

    if(power.tx == phymodPowerOn && power.rx == phymodPowerOn) {
        *value = 1;
    } else {
        *value = 0;
    } 

    return(SOC_E_NONE);
}
/* 
 * phy_82780_power_get
 */
STATIC int
phy_82780_power_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_power_t      power;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    phymod_phy_power_t_init(&power);
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_power_get(&pm_phy_copy, &power));

    if(power.tx == phymodPowerOn && power.rx == phymodPowerOn) {
        *value = 1;
    } else {
        *value = 0;
    } 

    return(SOC_E_NONE);
}
/* 
 * phy_82780_per_lane_tx_polarity_get
 */
STATIC int 
phy_82780_per_lane_tx_polarity_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    phymod_polarity_t_init(&polarity);
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(&pm_phy_copy, &polarity));

    *value = polarity.tx_polarity;

    return(SOC_E_NONE);
}
/* 
 * phy_82780_tx_polarity_get
 */
STATIC int 
phy_82780_tx_polarity_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    phymod_polarity_t_init(&polarity);
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(&pm_phy_copy, &polarity));

    *value = polarity.tx_polarity;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_rx_polarity_get
 */
STATIC int 
phy_82780_per_lane_rx_polarity_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    phymod_polarity_t_init(&polarity);
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(&pm_phy_copy, &polarity));

    *value = polarity.rx_polarity;

    return(SOC_E_NONE);
}
/* 
 * phy_82780_rx_polarity_get
 */
STATIC int 
phy_82780_rx_polarity_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    phymod_phy_access_t pm_phy_copy;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    phymod_polarity_t_init(&polarity);
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(&pm_phy_copy, &polarity));

    *value = polarity.rx_polarity;

    return(SOC_E_NONE);
}

/* 
 * phy_82780_gpio_config_get
 */
STATIC int 
phy_82780_gpio_config_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_phy_access_t pm_phy_copy;
    phymod_gpio_mode_t gpio_mode;
    int gpio_pin_no = 0;
    *value = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));

    for (gpio_pin_no = 0; gpio_pin_no < 6; gpio_pin_no++) {
        SOC_IF_ERROR_RETURN(phymod_phy_gpio_config_get(&pm_phy_copy, gpio_pin_no, &gpio_mode));
        *value |= (gpio_mode << (4 * gpio_pin_no));
    }

    return(SOC_E_NONE);
}
/* 
 * phy_82780_gpio_value_get
 */
STATIC int 
phy_82780_gpio_value_get(soc_phymod_ctrl_t *pmc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_phy_access_t pm_phy_copy;
    int gpio_pin_no = 0;
    int pin_value = 0;
    *value = 0;

    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));

    for (gpio_pin_no = 0; gpio_pin_no < 6; gpio_pin_no++) {
        SOC_IF_ERROR_RETURN(phymod_phy_gpio_pin_value_get(&pm_phy_copy, gpio_pin_no, &pin_value));
        *value |= (pin_value << (4 * gpio_pin_no));
    }

    return(SOC_E_NONE);
}
/*
 * Function:
 *      phy82780_control_get
 * Purpose:
 *      Get current control settings of the PHY. 
 * Parameters:
 *      unit  - BCM unit number.
 *      port  - Port number. 
 *      type  - Control to update 
 *      value - (OUT) Current setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_82780_control_get(int unit, soc_port_t port, soc_phy_control_t type, uint32 *value)
{
    int                 rv;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    int32 intf;
    soc_port_t primary= 0;
    int offset = 0;

    PHY_CONTROL_TYPE_CHECK(type);

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;

    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82780_SYS_SIDE : PHY82780_LINE_SIDE; 

    switch(type) {
    /* PRBS */
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
      rv = phy_82780_prbs_tx_poly_get(pmc, intf, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
      rv = phy_82780_prbs_tx_invert_data_get(pmc, intf, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
      rv = phy_82780_prbs_tx_enable_get(pmc, intf, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
      rv = phy_82780_prbs_rx_poly_get(pmc, intf, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
      rv = phy_82780_prbs_rx_invert_data_get(pmc, intf, value);
      break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
      rv = phy_82780_prbs_rx_enable_get(pmc, intf, value);
      break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
      rv = phy_82780_prbs_tx_poly_get(pmc, intf, value);
      break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
      rv = phy_82780_prbs_tx_invert_data_get(pmc, intf, value);
      break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
      rv = phy_82780_prbs_tx_enable_get(pmc, intf, value);
      break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
      rv = phy_82780_prbs_rx_enable_get(pmc, intf, value);
      break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
      rv = phy_82780_prbs_rx_status_get(pmc, intf, value);
      break;
    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
       rv = phy_82780_loopback_remote_get(pmc, intf, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
       rv = phy_82780_loopback_internal_pmd_get(pmc, intf, value);
       break;
    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS:
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        rv = phy_82780_per_lane_preemphasis_get(pmc, intf, 0, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        rv = phy_82780_per_lane_preemphasis_get(pmc, intf, 1, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        rv = phy_82780_per_lane_preemphasis_get(pmc, intf, 2, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        rv = phy_82780_per_lane_preemphasis_get(pmc, intf, 3, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_PRE:
        /* assume they are all the same as lane 0 */
        rv = phy_82780_tx_fir_pre_get(pmc, intf, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
        /* assume they are all the same as lane 0 */
        rv = phy_82780_tx_fir_main_get(pmc, intf, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST:
        /* assume they are all the same as lane 0 */
        rv = phy_82780_tx_fir_post_get(pmc, intf, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        /* assume they are all the same as lane 0 */
        rv = phy_82780_tx_fir_post2_get(pmc, intf, value);
        break;

    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        rv = phy_82780_per_lane_driver_current_get(pmc, intf, 0, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        rv = phy_82780_per_lane_driver_current_get(pmc, intf, 1, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        rv = phy_82780_per_lane_driver_current_get(pmc, intf, 2, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        rv = phy_82780_per_lane_driver_current_get(pmc, intf, 3, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = phy_82780_per_lane_driver_current_get(pmc, intf, 0, value);
        break;

    /* RX PEAK FILTER */
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        rv = phy_82780_rx_peak_filter_get(pmc, intf, value);
        break;

    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        rv = phy_82780_rx_low_peak_filter_get(pmc, intf, value);
        break;
    /* RX VGA */
    case SOC_PHY_CONTROL_RX_VGA:
        rv = phy_82780_rx_vga_get(pmc, intf, value);
        break;

    /* RX TAP */
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = phy_82780_rx_tap_get(pmc, intf, 0, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = phy_82780_rx_tap_get(pmc, intf, 1, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = phy_82780_rx_tap_get(pmc, intf, 2, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = phy_82780_rx_tap_get(pmc, intf, 3, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = phy_82780_rx_tap_get(pmc, intf, 4, value);
        break;

    /* PHASE INTERPOLATOR */
    case SOC_PHY_CONTROL_PHASE_INTERP:
        rv = phy_82780_pi_control_get(pmc, intf, value);
        break;

    case SOC_PHY_CONTROL_RX_SEQ_DONE:
      rv = phy_82780_rx_seq_done_get(pmc, intf, value);
      break;
    /* FEC */
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
      rv = phy_82780_fec_enable_get(pmc, value);
      break;
    case SOC_PHY_CONTROL_RX_POLARITY:
        rv = phy_82780_rx_polarity_get(pmc, intf, value);;
        break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        rv = phy_82780_tx_polarity_get(pmc, intf, value);
        break;
    /* POWER */
    case SOC_PHY_CONTROL_POWER:
       rv = phy_82780_power_get(pmc, intf, value);
       break;
    case SOC_PHY_CONTROL_CL72:
        rv = phy_82780_cl72_enable_get(pmc, intf, value);
    break;
    case SOC_PHY_CONTROL_CL72_STATUS:
        rv = phy_82780_cl72_status_get(pmc, intf, value);
    break;
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = phy_82780_tx_lane_squelch_get(pmc, intf, value);
    break;
    case SOC_PHY_CONTROL_GPIO_CONFIG:
        rv = phy_82780_gpio_config_get(pmc, value);
    break;
    case SOC_PHY_CONTROL_GPIO_VALUE:
        rv = phy_82780_gpio_value_get(pmc, value);
    break;
    case SOC_PHY_CONTROL_FAIL_OVER_MODE: 
        rv = phy_82780_config_fail_over_get(pc, intf, value);
    break;

    case SOC_PHY_CONTROL_PORT_PRIMARY:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_primary_get(unit, port, &primary));
        *value = (uint32) primary;
		rv = SOC_E_NONE;
        break;

    case SOC_PHY_CONTROL_PORT_OFFSET:
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_offset_get(unit, port, &offset));
        *value = (uint32) offset;
		rv = SOC_E_NONE;
        break;


    default:
       rv = SOC_E_UNAVAIL;
       break;
    }
    return rv;
}

/*
 * Function:
 *      phy_82780_per_lane_control_get
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      lane  - lane number
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_82780_per_lane_control_get(int unit, soc_port_t port, int lane,
                     soc_phy_control_t type, uint32 *value)
{
    int                 rv;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    int32 intf;


    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;

    PHY_CONTROL_TYPE_CHECK(type);

    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82780_SYS_SIDE : PHY82780_LINE_SIDE; 

    switch(type) {

    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = phy_82780_per_lane_preemphasis_get(pmc, intf, lane, value);
        break; 
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = phy_82780_per_lane_driver_current_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        rv = phy_82780_per_lane_prbs_tx_poly_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        rv = phy_82780_per_lane_prbs_tx_invert_data_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        rv = phy_82780_per_lane_prbs_tx_enable_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        rv = phy_82780_per_lane_prbs_rx_poly_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        rv = phy_82780_per_lane_prbs_rx_invert_data_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        rv = phy_82780_per_lane_prbs_rx_enable_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = phy_82780_per_lane_prbs_tx_poly_get(pmc, intf, lane, value);
        rv = phy_82780_per_lane_prbs_rx_poly_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = phy_82780_per_lane_prbs_tx_invert_data_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        rv = phy_82780_per_lane_prbs_rx_invert_data_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = phy_82780_per_lane_prbs_tx_enable_get(pmc, intf, lane, value);
        rv = phy_82780_per_lane_prbs_rx_enable_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = phy_82780_per_lane_prbs_rx_enable_get(pmc, intf, lane, value);
        rv = phy_82780_per_lane_prbs_tx_enable_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
        rv = phy_82780_per_lane_prbs_rx_status_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        rv = phy_82780_per_lane_rx_peak_filter_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_VGA:
        rv = phy_82780_per_lane_rx_vga_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = phy_82780_per_lane_rx_dfe_tap_control_get(pmc, intf, lane, 0, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = phy_82780_per_lane_rx_dfe_tap_control_get(pmc, intf, lane, 1, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = phy_82780_per_lane_rx_dfe_tap_control_get(pmc, intf, lane, 2, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = phy_82780_per_lane_rx_dfe_tap_control_get(pmc, intf, lane, 3, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = phy_82780_per_lane_rx_dfe_tap_control_get(pmc, intf, lane, 4, value);
        break;
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        rv = phy_82780_per_lane_rx_low_freq_filter_get(pmc, intf, lane, value);
        break;
    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
       rv = phy_82780_per_lane_loopback_remote_get(pmc, intf, lane, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
       rv = phy_82780_per_lane_loopback_internal_pmd_get(pmc, intf, lane, value);
       break;
    case SOC_PHY_CONTROL_RX_POLARITY:
        rv = phy_82780_per_lane_rx_polarity_get(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        rv = phy_82780_per_lane_tx_polarity_get(pmc, intf, lane, value);
        break;
    /* POWER */
    case SOC_PHY_CONTROL_POWER:
       rv = phy_82780_per_lane_power_get(pmc, intf, lane, value);
       break;
    case SOC_PHY_CONTROL_TX_FIR_PRE:
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
    case SOC_PHY_CONTROL_TX_FIR_POST:
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        rv = phy_82780_per_lane_tx_get(pmc, intf, type, lane, value);
        break;
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = phy_82780_per_lane_tx_lane_squelch_get(pmc, intf, lane, value);
        break;

    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}

/*
 * Function:    
 *      phy_82780_an_set
 * Purpose:     
 *      Enable or disable auto-negotiation on the specified port.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      an   - Boolean, if true, auto-negotiation is enabled 
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:     
 *      SOC_E_XXX  _soc_triumph_tx
 */
STATIC int
phy_82780_an_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t                *pc;
    phy82780_config_t         *pCfg;
    soc_phymod_ctrl_t         *pmc;
    phymod_phy_access_t *pm_phy;
    int idx = 0;
    phymod_autoneg_control_t an;

    sal_memset(&an, 0, sizeof(phymod_autoneg_control_t));
    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;
    pCfg = (phy82780_config_t *) pc->driver_data;
    idx = pmc->main_phy;
    pm_phy = &pmc->phy[idx]->pm_phy;

    if (pCfg->speed_config.speed == 1000 &&
          pCfg->speed_config.line_interface == SOC_PORT_IF_SR) {
        an.an_mode = phymod_AN_MODE_CL37; 
        an.enable = enable; 
        SOC_IF_ERROR_RETURN(
           phymod_phy_autoneg_set(pm_phy, &an));

    }

    return (SOC_E_NONE);
}
/*
 * Function:    
 *      phy_82780_an_get
 * Purpose:     
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      an   - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if an == false.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_82780_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    phymod_autoneg_control_t an_control;
    int idx, an_complete;

    *an = 0;
    *an_done = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;

    sal_memset(&an_control, 0x0, sizeof(an_control));

    idx = pmc->main_phy;
    pm_phy = &pmc->phy[idx]->pm_phy;
    
    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_get(pm_phy, &an_control, (uint32_t *) &an_complete)); 

    if (an_control.enable) {
        *an = 1;
        *an_done = an_complete;
#ifdef PHY82780_DEBUG    
        LOG_CLI((BSL_META_U(unit,"AN complete:%d\n "),*an_done));
#endif        
    } else {
        *an = 0;
        *an_done = 0;
    }
    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_82780_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      ability - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_82780_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       line_interface;
    uint32_t                  an_tech_ability;
    uint32_t                  an_bam37_ability;
    uint32_t                  an_bam73_ability;
    _shr_port_mode_t          speed_full_duplex;
    phymod_autoneg_ability_t  phymod_autoneg_ability; 

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    phymod_autoneg_ability_t_init(&phymod_autoneg_ability);

    pmc = &pc->phymod_ctrl;

    /* only set abilities on the first core */
    phy = pmc->phy[pmc->main_phy];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    an_tech_ability  = 0;
    an_bam37_ability = 0;
    an_bam73_ability = 0;
    speed_full_duplex = ability->speed_full_duplex;
    line_interface = ability->interface;

    if (speed_full_duplex & SOC_PA_SPEED_40GB) {
        if (line_interface & SOC_PORT_IF_CR4) {
            PHYMOD_AN_CAP_40G_CR4_SET(an_tech_ability);
        } else {
            PHYMOD_AN_CAP_40G_KR4_SET(an_tech_ability);
        }
    } else if (speed_full_duplex & SOC_PA_SPEED_10GB) {
        PHYMOD_AN_CAP_10G_KR_SET(an_tech_ability);
    } else {
        an_tech_ability = 0;
    }

    phymod_autoneg_ability.an_cap = an_tech_ability;
    phymod_autoneg_ability.cl73bam_cap = an_bam73_ability; 
    phymod_autoneg_ability.cl37bam_cap = an_bam37_ability;
#ifdef PHY82780_DEBUG    
    LOG_CLI((BSL_META_U(unit,"AN:abilitySet =%x tech ability:%x pause:%d\n"), speed_full_duplex, an_tech_ability,ability->pause));
#endif
    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
        case SOC_PA_PAUSE_TX:
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(&phymod_autoneg_ability);
        break;
        case SOC_PA_PAUSE_RX:
            /* an_adv |= MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE; */
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(&phymod_autoneg_ability);
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(&phymod_autoneg_ability);
        break;
        case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(&phymod_autoneg_ability);
        break;
    }
 
    
             
    phymod_autoneg_ability.sgmii_speed = 2; 

    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_ability_set(&phy->pm_phy, &phymod_autoneg_ability)); 
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82780_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      ability - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_82780_ability_advert_get(int unit, soc_port_t port,
                              soc_port_ability_t *ability)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       reg73_ability;
    int                       reg_ability;
    _shr_port_mode_t          speed_full_duplex;
    phymod_autoneg_ability_t  phymod_autoneg_ability; 

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;

    /* only get abilities from the first core */
    phy = pmc->phy[0];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }

    phymod_autoneg_ability_t_init(&phymod_autoneg_ability);

    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_ability_get(&phy->pm_phy, &phymod_autoneg_ability));

    
    speed_full_duplex = 0;

    /* retrieve CL73 abilities */
    reg73_ability = phymod_autoneg_ability.an_cap;
    speed_full_duplex |= PHYMOD_AN_CAP_40G_CR4_GET(reg73_ability) ? SOC_PA_SPEED_40GB : 0;
    speed_full_duplex |= PHYMOD_AN_CAP_40G_KR4_GET(reg73_ability) ? SOC_PA_SPEED_40GB : 0;
    speed_full_duplex |= PHYMOD_AN_CAP_10G_KR_GET (reg73_ability)  ? SOC_PA_SPEED_10GB : 0;

    if (speed_full_duplex & SOC_PA_SPEED_40GB) {
        if (PHYMOD_AN_CAP_40G_CR4_GET(reg73_ability)) {
            ability->interface = SOC_PORT_IF_CR4;
        } else {
            ability->interface = SOC_PORT_IF_KR4;
        }
    } else if (speed_full_duplex & SOC_PA_SPEED_10GB) {
        ability->interface = SOC_PORT_IF_KR;
    } else {
        ability->interface = 0;
    }

    
    /* retrieve "pause" abilities */
    reg_ability = phymod_autoneg_ability.capabilities;

    ability->pause = 0;
    if (reg_ability == PHYMOD_AN_CAP_ASYM_PAUSE) {
        ability->pause = SOC_PA_PAUSE_TX;
    } else if (reg_ability == (PHYMOD_AN_CAP_SYMM_PAUSE | PHYMOD_AN_CAP_ASYM_PAUSE)) {
        ability->pause = SOC_PA_PAUSE_RX;
    } else if (reg_ability == PHYMOD_AN_CAP_SYMM_PAUSE) {
        ability->pause = (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX);
    }

    ability->speed_full_duplex = speed_full_duplex;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy82780_lb_get
 * Purpose:
 *      Get current PHY loopback mode
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy82780_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    uint32 out_en;

    pc = EXT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;
    SOC_IF_ERROR_RETURN(
        phy_82780_loopback_internal_pmd_get(pmc, PHY82780_LINE_SIDE, &out_en));
    *enable = (int) out_en;

    return SOC_E_NONE;
}

/* 
 * phy_82780_per_lane_loopback_internal_pmd_set
 */
STATIC int 
phy_82780_per_lane_loopback_internal_pmd_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(&pm_phy_copy, phymodLoopbackGlobalPMD, value));
    return(SOC_E_NONE);
}

/* 
 * phy_82780_loopback_internal_pmd_set
 */
STATIC int 
phy_82780_loopback_internal_pmd_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    int                    idx;
    phymod_phy_access_t pm_phy_copy;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(&pm_phy_copy, phymodLoopbackGlobalPMD, value));
    }
    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_loopback_remote_set
 */
STATIC int 
phy_82780_per_lane_loopback_remote_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(&pm_phy_copy, phymodLoopbackRemotePMD, value));
    return(SOC_E_NONE);
}
/* 
 * phy_82780_loopback_remote_set
 */
STATIC int 
phy_82780_loopback_remote_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    int                    idx;
    phymod_phy_access_t pm_phy_copy;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(&pm_phy_copy, phymodLoopbackRemotePMD, value));
    }
    return(SOC_E_NONE);
}

/* 
 * phy_82780_fec_enable_set
 */
STATIC int 
phy_82780_fec_enable_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t    *pm_phy;
    int                     idx;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_fec_enable_set(pm_phy,  value));
    }
    return(SOC_E_NONE);
}

STATIC int
phy_82780_sdk_poly_to_phymod_poly(uint32 sdk_poly, phymod_prbs_poly_t *phymod_poly){
    switch(sdk_poly){
    case SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1:
        *phymod_poly = phymodPrbsPoly7;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1:
        *phymod_poly = phymodPrbsPoly15;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1:
        *phymod_poly = phymodPrbsPoly23;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1:
        *phymod_poly = phymodPrbsPoly31;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X9_X5_1:
        *phymod_poly = phymodPrbsPoly9;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X11_X9_1:
        *phymod_poly = phymodPrbsPoly11;
        break;
    case SOC_PHY_PRBS_POLYNOMIAL_X58_X31_1:
        *phymod_poly = phymodPrbsPoly58;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}
/* 
 * phy_82780_per_lane_prbs_tx_poly_set
 */
STATIC int
phy_82780_per_lane_prbs_tx_poly_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy,  flags, &prbs));
    SOC_IF_ERROR_RETURN(phy_82780_sdk_poly_to_phymod_poly(value, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(&pm_phy_copy, flags, &prbs));
    return(SOC_E_NONE);
}
/* 
 * phy_82780_prbs_tx_poly_set
 */
STATIC int
phy_82780_prbs_tx_poly_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    int                    idx;
    phymod_phy_access_t pm_phy_copy;
    
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 
        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy,  flags, &prbs));
        SOC_IF_ERROR_RETURN(phy_82780_sdk_poly_to_phymod_poly(value, &prbs.poly));
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(&pm_phy_copy, flags, &prbs));
    }
    return(SOC_E_NONE);
}
/* 
 * phy_82780_per_lane_prbs_rx_poly_set
 */
STATIC int
phy_82780_per_lane_prbs_rx_poly_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));
    SOC_IF_ERROR_RETURN(phy_82780_sdk_poly_to_phymod_poly(value, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(&pm_phy_copy, flags, &prbs));
    return(SOC_E_NONE);
}

/* 
 * phy_82780_prbs_rx_poly_set
 */
STATIC int
phy_82780_prbs_rx_poly_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    int                    idx;
    phymod_phy_access_t pm_phy_copy;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 
        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));
        SOC_IF_ERROR_RETURN(phy_82780_sdk_poly_to_phymod_poly(value, &prbs.poly));
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(&pm_phy_copy, flags, &prbs));
    }
    return(SOC_E_NONE);
}


/* 
 * phy_82780_per_lane_prbs_tx_invert_data_set
 */
STATIC int
phy_82780_per_lane_prbs_tx_invert_data_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));
    prbs.invert = value;
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(&pm_phy_copy, flags,  &prbs));
    return(SOC_E_NONE);
}

/* 
 * phy_82780_prbs_tx_invert_data_set
 */
STATIC int
phy_82780_prbs_tx_invert_data_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    int                    idx;
    phymod_phy_access_t pm_phy_copy;
    
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 
        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));
        prbs.invert = value;
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(&pm_phy_copy, flags,  &prbs));
    }
    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_prbs_rx_invert_data_set
 */
STATIC int
phy_82780_per_lane_prbs_rx_invert_data_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags,  &prbs));
    prbs.invert = value;
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(&pm_phy_copy, flags, &prbs));
    return(SOC_E_NONE);
}
/* 
 * phy_82780_prbs_rx_invert_data_set
 */
STATIC int
phy_82780_prbs_rx_invert_data_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    int                    idx;
    phymod_phy_access_t pm_phy_copy;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 
        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags,  &prbs));
        prbs.invert = value;
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(&pm_phy_copy, flags, &prbs));
    }
    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_prbs_tx_enable_set
 */
STATIC int
phy_82780_per_lane_prbs_tx_enable_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(
            phymod_phy_prbs_enable_set(&pm_phy_copy, flags, value));
    return(SOC_E_NONE);
}
/* 
 * phy_82780_prbs_tx_enable_set
 */
STATIC int
phy_82780_prbs_tx_enable_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;
    int                    idx;
    phymod_phy_access_t pm_phy_copy;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 
        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(&pm_phy_copy, flags, value));
    }
    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_prbs_rx_enable_set
 */
STATIC int
phy_82780_per_lane_prbs_rx_enable_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;

    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(&pm_phy_copy, flags,  value));
    return(SOC_E_NONE);
}
/* 
 * phy_82780_prbs_rx_enable_set
 */
STATIC int
phy_82780_prbs_rx_enable_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;
    int                    idx;
    phymod_phy_access_t pm_phy_copy;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 
        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
        SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(&pm_phy_copy, flags,  value));
    }
    return(SOC_E_NONE);
}


/* 
 * given a pc (phymod_ctrl_t) and logical lane number, 
 *    find the correct soc_phymod_phy_t object and lane
 */
STATIC int
_phy_82780_find_soc_phy_lane(soc_phymod_ctrl_t *pmc, uint32_t lane, 
                        soc_phymod_phy_t **p_phy, uint32 *lane_map)
{
    phymod_phy_access_t *pm_phy;
    int idx, lnx, ln_cnt, found;
    uint32 lane_map_copy;

    /* Traverse lanes belonging to this port */
    found = 0;
    ln_cnt = 0;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        lane_map_copy = pm_phy->access.lane_mask;
        for (lnx = 0; lnx < 4; lnx++) {
            if ((1 << lnx) & lane_map_copy) {
                if ((ln_cnt == lane) || (lane == 0)) {
                    found = 1;
                    break;
                }
                ln_cnt++;
            }
        }
        if (found) {
            *p_phy = pmc->phy[idx];
            *lane_map = (1 << lnx);
            break;
        }
    } 

    if (!found) {
        LOG_CLI((BSL_META_U(pmc->unit,"\nInvalid lane \n")));
        /* No such lane */
        return SOC_E_PARAM;
    }
    return (SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_preemphasis_set
 */
STATIC int
phy_82780_per_lane_preemphasis_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    /* 0 to 7 = pre
     * 8 to 15 = main
     * 16 to 24 = post*/
    phymod_tx.pre = (value & 0xff);
    phymod_tx.main = (value & 0xff00) >> 8;
    phymod_tx.post = (value & 0xff0000) >> 16;

    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));

    return(SOC_E_NONE);
}

/* 
 * phy_82780_preemphasis_set
 */
STATIC int
phy_82780_preemphasis_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 
        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        /* 0 to 7 = pre
         * 8 to 15 = main
         * 16 to 24 = post*/
        phymod_tx.pre = (value & 0xff);
        phymod_tx.main = (value & 0xff00) >> 8;
        phymod_tx.post = (value & 0xff0000) >> 16;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));

    }

    return(SOC_E_NONE);
}

/* 
 * phy_82780_tx_fir_pre_set
 */
STATIC int
phy_82780_tx_fir_pre_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.pre = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82780_tx_fir_main_set
 */
STATIC int
phy_82780_tx_fir_main_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;
    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.main = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82780_tx_fir_post_set
 */
STATIC int
phy_82780_tx_fir_post_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.post = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82780_tx_fir_post2_set
 */
STATIC int
phy_82780_tx_fir_post2_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.post2 = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_driver_current_set
 */
STATIC int
phy_82780_per_lane_driver_current_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    phymod_tx.amp = value;
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));

    return(SOC_E_NONE);
}

/* 
 * phy_82780_driver_current_set
 */
STATIC int
phy_82780_driver_current_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_tx_t         phymod_tx;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.amp = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_rx_dfe_tap_control_set
 */
STATIC int
phy_82780_per_lane_rx_dfe_tap_control_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, int tap, int enable, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t          phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

    if (tap < 0 || tap >= COUNTOF(phymod_rx.dfe)) {
        /* this can only happen with a coding error */
        return SOC_E_INTERNAL;
    }
    sal_memset(&phymod_rx, 0, sizeof(phymod_rx));
    phymod_rx.dfe[tap].enable = enable;
    phymod_rx.dfe[tap].value = value;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(&pm_phy_copy, &phymod_rx));

    return(SOC_E_NONE);
}
/*
 * phy_82780_per_lane_tx_lane_squelch
 */
STATIC int
phy_82780_per_lane_tx_lane_squelch(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_tx_lane_control_t  tx_control;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.access.lane_mask = lane_map;
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        if (value == 1) {
            tx_control = phymodTxSquelchOn;
        } else {
            tx_control = phymodTxSquelchOff;
        }
        SOC_IF_ERROR_RETURN
            (phymod_phy_tx_lane_control_set(&pm_phy_copy, tx_control));

    }
    return(SOC_E_NONE);
} 

STATIC int
phy_82780_per_lane_tx_lane_squelch_get(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_tx_lane_control_t  tx_control;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.access.lane_mask = lane_map;
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        SOC_IF_ERROR_RETURN
            (phymod_phy_tx_lane_control_get(&pm_phy_copy, &tx_control));
        if (tx_control == phymodTxSquelchOn) {
            *value = 1;
        } else {
            *value = 0;
        }
    }
    return(SOC_E_NONE);
} 

/* 
 * phy_82780_tx_lane_squelch
 */
STATIC int
phy_82780_tx_lane_squelch(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_tx_lane_control_t  tx_control;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        if (value == 1) {
            tx_control = phymodTxSquelchOn;
        } else {
            tx_control = phymodTxSquelchOff;
        }
        SOC_IF_ERROR_RETURN
            (phymod_phy_tx_lane_control_set(&pm_phy_copy, tx_control));

    }
    return(SOC_E_NONE);
}

/* 
 * phy_82780_tx_lane_squelch_get
 */
STATIC int
phy_82780_tx_lane_squelch_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_tx_lane_control_t  tx_control;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 
        SOC_IF_ERROR_RETURN
            (phymod_phy_tx_lane_control_get(&pm_phy_copy, &tx_control));
        if (tx_control == phymodTxSquelchOn) {
            *value = 1;
        } else {
            *value = 0;
        }
    }
    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_rx_peak_filter_set
 */
STATIC int
phy_82780_per_lane_rx_peak_filter_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, int enable, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    sal_memset(&phymod_rx, 0, sizeof(phymod_rx));
    phymod_rx.peaking_filter.enable = TRUE;
    phymod_rx.peaking_filter.value = value;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(&pm_phy_copy, &phymod_rx));

    return(SOC_E_NONE);
}

/* 
 * phy_82780_rx_peak_filter_set
 */
STATIC int 
phy_82780_rx_peak_filter_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_rx_t         phymod_rx;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        sal_memset(&phymod_rx, 0, sizeof(phymod_rx));
        phymod_rx.peaking_filter.enable = TRUE;
        phymod_rx.peaking_filter.value = value;
        SOC_IF_ERROR_RETURN(phymod_phy_rx_set(&pm_phy_copy, &phymod_rx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_rx_vga_set
 */
STATIC int
phy_82780_per_lane_rx_vga_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, int enable, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    sal_memset(&phymod_rx, 0, sizeof(phymod_rx));
    phymod_rx.vga.enable = TRUE;
    phymod_rx.vga.value = value;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(&pm_phy_copy, &phymod_rx));

    return(SOC_E_NONE);
}

/* 
 * phy_82780_rx_vga_set
 */
STATIC int 
phy_82780_rx_vga_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_rx_t         phymod_rx;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        sal_memset(&phymod_rx, 0, sizeof(phymod_rx));
        phymod_rx.vga.enable = TRUE;
        phymod_rx.vga.value = value;
        SOC_IF_ERROR_RETURN(phymod_phy_rx_set(&pm_phy_copy, &phymod_rx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82780_rx_tap_set
 */
STATIC int 
phy_82780_rx_tap_set(soc_phymod_ctrl_t *pmc, int32 intf, int tap, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_rx_t         phymod_rx;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;

    /* bounds check "tap" */
    if (tap < 0 || tap >= COUNTOF(phymod_rx.dfe)) {
        return SOC_E_INTERNAL;
    }

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        sal_memset(&phymod_rx, 0, sizeof(phymod_rx));
        phymod_rx.num_of_dfe_taps = 5;
        phymod_rx.dfe[tap].enable = TRUE;
        phymod_rx.dfe[tap].value = value;
        SOC_IF_ERROR_RETURN(phymod_phy_rx_set(&pm_phy_copy, &phymod_rx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82780_pi_control_set
 */
STATIC int 
phy_82780_pi_control_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_tx_override_t tx_override;
    int                  idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        phymod_tx_override_t_init(&tx_override);
        tx_override.phase_interpolator.enable = (value == 0) ? 0 : 1;
        tx_override.phase_interpolator.value = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_override_set(&pm_phy_copy, &tx_override));
    }

    return(SOC_E_NONE);
}

STATIC int 
phy_82780_per_lane_tx_polarity_set(soc_phymod_ctrl_t *pmc, phymod_polarity_t *cfg_polarity, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 


    sal_memcpy(&polarity, cfg_polarity, sizeof(polarity));
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(&pm_phy_copy, &polarity));
    polarity.tx_polarity = value;
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_set(&pm_phy_copy, &polarity));

    /* after successfully setting the parity, update the configured value */
    cfg_polarity->tx_polarity = value;

    return(SOC_E_NONE);
}
/* 
 * phy_82780_tx_polarity_set
 */
STATIC int 
phy_82780_tx_polarity_set(soc_phymod_ctrl_t *pmc, phymod_polarity_t *cfg_polarity, int32 intf, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    int                  idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        sal_memcpy(&polarity, cfg_polarity, sizeof(polarity));
        SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(&pm_phy_copy, &polarity));
        polarity.tx_polarity = value;
        SOC_IF_ERROR_RETURN(phymod_phy_polarity_set(&pm_phy_copy, &polarity));

        /* after successfully setting the parity, update the configured value */
        cfg_polarity->tx_polarity = value;
    }

    return(SOC_E_NONE);
}
/* 
 * phy_82780_per_lane_rx_polarity_set
 */
STATIC int 
phy_82780_per_lane_rx_polarity_set(soc_phymod_ctrl_t *pmc, phymod_polarity_t *cfg_polarity, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    sal_memcpy(&polarity, cfg_polarity, sizeof(polarity));
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(&pm_phy_copy, &polarity));
    polarity.rx_polarity = value;
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_set(&pm_phy_copy, &polarity));

    /* after successfully setting the parity, update the configured value */
    cfg_polarity->rx_polarity = value;
    return(SOC_E_NONE);
}

/* 
 * phy_82780_rx_polarity_set
 */

STATIC int 
phy_82780_rx_polarity_set(soc_phymod_ctrl_t *pmc, phymod_polarity_t *cfg_polarity, int32 intf, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    int                  idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 
        sal_memcpy(&polarity, cfg_polarity, sizeof(polarity));
        SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(&pm_phy_copy, &polarity));
        polarity.rx_polarity = value;
        SOC_IF_ERROR_RETURN(phymod_phy_polarity_set(&pm_phy_copy, &polarity));

        /* after successfully setting the parity, update the configured value */
        cfg_polarity->rx_polarity = value;
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82780_rx_reset_set
 */
STATIC int
phy_82780_rx_reset(soc_phymod_ctrl_t *pmc, phymod_phy_reset_t *cfg_reset, int32 intf, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_reset_t  reset;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        sal_memcpy(&reset, cfg_reset, sizeof(reset));
        reset.rx = (phymod_reset_direction_t) value;
        SOC_IF_ERROR_RETURN(phymod_phy_reset_set(&pm_phy_copy, &reset));

        /* after successfully setting the parity, update the configured value */
        cfg_reset->rx = (phymod_reset_direction_t) value;
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82780_tx_reset
 */
STATIC int
phy_82780_tx_reset(soc_phymod_ctrl_t *pmc, phymod_phy_reset_t *cfg_reset, int32 intf, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_reset_t  reset;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        sal_memcpy(&reset, cfg_reset, sizeof(reset));
        reset.tx = (phymod_reset_direction_t) value;
        SOC_IF_ERROR_RETURN(phymod_phy_reset_set(&pm_phy_copy, &reset));

        /* after successfully setting the parity, update the configured value */
        cfg_reset->tx = (phymod_reset_direction_t) value;
    }

    return(SOC_E_NONE);
}


/* 
 * phy_82780_per_lane_power_set
 */
STATIC int
phy_82780_per_lane_power_set(soc_phymod_ctrl_t *pmc, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_power_t      power;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    phymod_phy_power_t_init(&power);
    if (value) {
        power.tx = phymodPowerOn;
        power.rx = phymodPowerOn;
    } 
    else {
        power.tx = phymodPowerOff;
        power.rx = phymodPowerOff;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_power_set(&pm_phy_copy, &power));

    return(SOC_E_NONE);
}
/* 
 * phy_82780_power_set
 */
STATIC int
phy_82780_power_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_power_t      power;
    int                     idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        }
 
        phymod_phy_power_t_init(&power);
        if (value) {
            power.tx = phymodPowerOn;
            power.rx = phymodPowerOn;
        } 
        else {
            power.tx = phymodPowerOff;
            power.rx = phymodPowerOff;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_power_set(&pm_phy_copy, &power));
    }

    return(SOC_E_NONE);
}


/* 
 * phy_82780_rx_low_freq_filter_set
 */
STATIC int
phy_82780_rx_low_freq_filter_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_rx_t          phymod_rx;
    int                  idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        }
 
        sal_memset(&phymod_rx, 0, sizeof(phymod_rx));
        phymod_rx.low_freq_peaking_filter.enable = TRUE;
        phymod_rx.low_freq_peaking_filter.value = value;
        SOC_IF_ERROR_RETURN(phymod_phy_rx_set(&pm_phy_copy, &phymod_rx));
    }

    return(SOC_E_NONE);
}

STATIC int 
phy_82780_rx_seq_restart(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_sequencer_operation_t seq_operation;
    int                  idx;
    soc_phymod_core_t *pmc_core;
    phymod_core_access_t *pm_core;
    phymod_core_access_t pm_core_copy;
    uint32_t flags = 0;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pmc_core = pmc->phy[idx]->core;
        pm_core = &pmc_core->pm_core;
        if (pm_core == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_core_copy, pm_core, sizeof(pm_core_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_core_copy);
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_core_copy);
        } 
        seq_operation = phymodSeqOpRestart; 
        SOC_IF_ERROR_RETURN(phymod_core_pll_sequencer_restart(pm_core, flags, seq_operation));
    }
    return(SOC_E_NONE);
}

STATIC int
phy_82780_phy_dump(soc_phymod_ctrl_t *pmc, int32 intf)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;
    phymod_phy_access_t pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        }
        SOC_IF_ERROR_RETURN(phymod_phy_status_dump(&pm_phy_copy));

    }

    return(SOC_E_NONE);
}
STATIC int
phy_82780_per_lane_phy_dump(soc_phymod_ctrl_t *pmc, int32 intf, int lane)
{
    phymod_phy_access_t     *pm_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_status_dump(&pm_phy_copy));
    return(SOC_E_NONE);
}
/* 
 * phy_82780_gpio_config_set
 */
STATIC int
phy_82780_gpio_config_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    int                  idx;
    phymod_phy_access_t pm_phy_copy;
    int gpio_mode = 0;
    int gpio_pin_no = 0;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
 
        for (gpio_pin_no = 0; gpio_pin_no < 6; gpio_pin_no++) {
            gpio_mode = (value >> (4 * gpio_pin_no)) & 0xF;
            SOC_IF_ERROR_RETURN(phymod_phy_gpio_config_set(&pm_phy_copy, gpio_pin_no, (phymod_gpio_mode_t)gpio_mode));
        }
    }

    return(SOC_E_NONE);
}
/* 
 * phy_82780_gpio_value_set
 */
STATIC int
phy_82780_gpio_value_set(soc_phymod_ctrl_t *pmc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    int                  idx;
    phymod_phy_access_t pm_phy_copy;
    int pin_value = 0;
    int gpio_pin_no = 0;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));

        for (gpio_pin_no = 0; gpio_pin_no < 6; gpio_pin_no++) {
            pin_value = (value >> (4 * gpio_pin_no)) & 0xF;
            SOC_IF_ERROR_RETURN(phymod_phy_gpio_pin_value_set(&pm_phy_copy, gpio_pin_no, pin_value));
        }
    }

    return(SOC_E_NONE);
}
/*
 * Function:
 *      phy_82780_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - BCM unit number.
 *      port  - Port number. 
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_82780_control_set(int unit, soc_port_t port, soc_phy_control_t type, uint32 value)
{
    int                 rv;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    phy82780_config_t       *pCfg;
    int32 intf;

    rv = SOC_E_UNAVAIL;

    PHY_CONTROL_TYPE_CHECK(type);

    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;
    pCfg = (phy82780_config_t *) pc->driver_data;


    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82780_SYS_SIDE : PHY82780_LINE_SIDE; 

    switch(type) {
    /* decoupled PRBS */
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        rv = phy_82780_prbs_tx_poly_set(pmc, intf, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        rv = phy_82780_prbs_tx_invert_data_set(pmc, intf, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        rv = phy_82780_prbs_tx_enable_set(pmc, intf, value);
        break; 
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        rv = phy_82780_prbs_rx_poly_set(pmc, intf, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        rv = phy_82780_prbs_rx_invert_data_set(pmc, intf, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        rv = phy_82780_prbs_rx_enable_set(pmc, intf, value);
        break;
    /* for legacy prbs usage mainly set both tx/rx the same */
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = phy_82780_prbs_tx_poly_set(pmc, intf, value);
        if (!rv) {
            rv = phy_82780_prbs_rx_poly_set(pmc, intf, value);
        }
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = phy_82780_prbs_tx_invert_data_set(pmc, intf, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = phy_82780_prbs_tx_enable_set(pmc, intf, value);
        if (!rv) {
            rv = phy_82780_prbs_rx_enable_set(pmc, intf, value);
        }
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = phy_82780_prbs_tx_enable_set(pmc, intf, value);
        if (!rv) {
            rv = phy_82780_prbs_rx_enable_set(pmc, intf, value);
        }
        break;
    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
        rv = phy_82780_loopback_internal_pmd_set(pmc, intf, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        rv = phy_82780_loopback_remote_set(pmc, intf, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_PRE:
        rv = phy_82780_tx_fir_pre_set(pmc, intf, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
        rv = phy_82780_tx_fir_main_set(pmc, intf, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST:
        rv = phy_82780_tx_fir_post_set(pmc, intf, value);
        break;
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        rv = phy_82780_tx_fir_post2_set(pmc, intf, value);
        break;
    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        rv = phy_82780_per_lane_preemphasis_set(pmc, intf, 0, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        rv = phy_82780_per_lane_preemphasis_set(pmc, intf, 1, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        rv = phy_82780_per_lane_preemphasis_set(pmc, intf, 2, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        rv = phy_82780_per_lane_preemphasis_set(pmc, intf, 3, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = phy_82780_preemphasis_set(pmc, intf, value);
        break;
    case SOC_PHY_CONTROL_PORT_PRIMARY:
        rv = soc_phyctrl_primary_set(unit, port, (soc_port_t)value);
        break;
    case SOC_PHY_CONTROL_PORT_OFFSET:
		rv = soc_phyctrl_offset_set(unit, port, (int)value);
        break;

    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        rv = phy_82780_per_lane_driver_current_set(pmc, intf, 0, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        rv = phy_82780_per_lane_driver_current_set(pmc, intf, 1, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        rv = phy_82780_per_lane_driver_current_set(pmc, intf, 2, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        rv = phy_82780_per_lane_driver_current_set(pmc, intf, 3, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = phy_82780_driver_current_set(pmc, intf, value);
        break;
    /* TX LANE SQUELCH */
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = phy_82780_tx_lane_squelch(pmc, intf, value);
        break;
    /* RX PEAK FILTER */
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        rv = phy_82780_rx_peak_filter_set(pmc, intf, value);
        break;

    /* RX VGA */
    case SOC_PHY_CONTROL_RX_VGA:
        rv = phy_82780_rx_vga_set(pmc, intf, value);
        break;
    /* RX TAP */
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = phy_82780_rx_tap_set(pmc, intf, 0, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = phy_82780_rx_tap_set(pmc, intf, 1, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = phy_82780_rx_tap_set(pmc, intf, 2, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = phy_82780_rx_tap_set(pmc, intf, 3, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = phy_82780_rx_tap_set(pmc, intf, 4, value);
        break;
    /* PHASE INTERPOLATOR */
    case SOC_PHY_CONTROL_PHASE_INTERP:
        rv = phy_82780_pi_control_set(pmc, intf, value);
        break;
    /* POLARITY */
    case SOC_PHY_CONTROL_RX_POLARITY:
        rv = phy_82780_rx_polarity_set(pmc, &pCfg->phy_polarity_config, intf, value);;
        break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        rv = phy_82780_tx_polarity_set(pmc, &pCfg->phy_polarity_config, intf, value);
        break;
    /* RESET */
    case SOC_PHY_CONTROL_RX_RESET:
        rv = phy_82780_rx_reset(pmc, &pCfg->phy_reset_config, intf, value);
        break;
    case SOC_PHY_CONTROL_TX_RESET:
        rv = phy_82780_tx_reset(pmc, &pCfg->phy_reset_config, intf, value);
        break;
    /* POWER */
    case SOC_PHY_CONTROL_POWER:
       {
           phy_ctrl_t                *int_pc;
           int_pc = INT_PHY_SW_STATE(unit, port);
           SOC_IF_ERROR_RETURN(PHY_ENABLE_SET(int_pc->pd, unit, port, value));
           rv = phy_82780_power_set(pmc, intf, value);
       }
       break;
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        rv = phy_82780_rx_low_freq_filter_set(pmc, intf, value); 
    break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
       rv = phy_82780_fec_enable_set(pmc, value);
       break;
    case SOC_PHY_CONTROL_RX_SEQ_TOGGLE:
        rv = phy_82780_rx_seq_restart(pmc, intf, value);
    break;
    case SOC_PHY_CONTROL_DUMP:
        rv = phy_82780_phy_dump(pmc, intf);
    break;
    case SOC_PHY_CONTROL_CL72:
        rv = phy_82780_cl72_enable_set(pmc, intf, value);
    break;
    case SOC_PHY_CONTROL_GPIO_CONFIG:
        rv = phy_82780_gpio_config_set(pmc, value);
    break;
    case SOC_PHY_CONTROL_GPIO_VALUE:
        rv = phy_82780_gpio_value_set(pmc, value);
    break;
    case SOC_PHY_CONTROL_RX_VGA_RELEASE:  
    case SOC_PHY_CONTROL_RX_TAP1_RELEASE: 
    case SOC_PHY_CONTROL_RX_TAP2_RELEASE: 
    case SOC_PHY_CONTROL_RX_TAP3_RELEASE: 
    case SOC_PHY_CONTROL_RX_TAP4_RELEASE: 
    case SOC_PHY_CONTROL_RX_TAP5_RELEASE: 
        rv = phy_82780_adoptation_release(pc, intf);
    break;
    case SOC_PHY_CONTROL_FAIL_OVER_MODE: 
        rv = phy_82780_config_fail_over(pc, intf, value);
    break;

    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_82780_per_lane_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      lane  - lane number
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_82780_per_lane_control_set(int unit, soc_port_t port, int lane, soc_phy_control_t type, uint32 value)
{
    int                 rv = SOC_E_UNAVAIL;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    phy82780_config_t       *pCfg;
    int32 intf;

    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;
    pCfg = (phy82780_config_t *) pc->driver_data;

    PHY_CONTROL_TYPE_CHECK(type);

    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82780_SYS_SIDE : PHY82780_LINE_SIDE; 

    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = phy_82780_per_lane_preemphasis_set(pmc, intf, lane, value);
        break; 
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = phy_82780_per_lane_driver_current_set(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = phy_82780_per_lane_rx_dfe_tap_control_set (pmc, intf, lane, 0 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = phy_82780_per_lane_rx_dfe_tap_control_set (pmc, intf, lane, 1 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = phy_82780_per_lane_rx_dfe_tap_control_set (pmc, intf, lane, 2 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = phy_82780_per_lane_rx_dfe_tap_control_set (pmc, intf, lane, 3 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = phy_82780_per_lane_rx_dfe_tap_control_set (pmc, intf, lane, 4 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        rv = phy_82780_per_lane_prbs_tx_poly_set(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        rv = phy_82780_per_lane_prbs_tx_invert_data_set(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        rv = phy_82780_per_lane_prbs_tx_enable_set(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        rv = phy_82780_per_lane_prbs_rx_poly_set(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        rv = phy_82780_per_lane_prbs_rx_invert_data_set(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        rv = phy_82780_per_lane_prbs_rx_enable_set(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = phy_82780_per_lane_prbs_tx_poly_set(pmc, intf, lane, value);
        if (rv == SOC_E_NONE) {
            rv = phy_82780_per_lane_prbs_rx_poly_set(pmc, intf, lane, value);
        }
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = phy_82780_per_lane_prbs_tx_invert_data_set(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        rv = phy_82780_per_lane_prbs_rx_invert_data_set(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        /* TX_ENABLE does both tx and rx */
        rv = phy_82780_per_lane_prbs_tx_enable_set(pmc, intf, lane, value);
        if (rv == SOC_E_NONE) {
            rv = phy_82780_per_lane_prbs_rx_enable_set(pmc, intf, lane, value);
        }
        break; 
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = phy_82780_per_lane_prbs_tx_enable_set(pmc, intf, lane, value);
        if (rv == SOC_E_NONE) {
            rv = phy_82780_per_lane_prbs_rx_enable_set(pmc, intf, lane, value);
        }
        break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        rv = phy_82780_per_lane_rx_peak_filter_set(pmc, intf, lane, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        rv = phy_82780_per_lane_rx_low_freq_filter_set(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_VGA:
        rv = phy_82780_per_lane_rx_vga_set(pmc, intf, lane, 1 /* enable */, value);
        break;
    /* TX LANE SQUELCH */
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = phy_82780_per_lane_tx_lane_squelch(pmc, intf, lane, value);
        break;
    /* POWER */
    case SOC_PHY_CONTROL_POWER:
       rv = phy_82780_per_lane_power_set(pmc, intf, lane, value);
       break;
    /* POLARITY */
    case SOC_PHY_CONTROL_RX_POLARITY:
        rv = phy_82780_per_lane_rx_polarity_set(pmc, &pCfg->phy_polarity_config, intf, lane, value);;
        break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        rv = phy_82780_per_lane_tx_polarity_set(pmc, &pCfg->phy_polarity_config,
              intf, lane, value);
        break;
    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
        rv = phy_82780_per_lane_loopback_internal_pmd_set(pmc, intf, lane, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        rv = phy_82780_per_lane_loopback_remote_set(pmc, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_DUMP:
        rv = phy_82780_per_lane_phy_dump(pmc, intf, lane);
    break;
    case SOC_PHY_CONTROL_TX_FIR_PRE:
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
    case SOC_PHY_CONTROL_TX_FIR_POST:
    case SOC_PHY_CONTROL_TX_FIR_POST2:
        rv = phy_82780_per_lane_tx_set(pmc, intf, type, lane, value);
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }

    return rv;
}

STATIC int phy_82780_diag_eyescan(soc_port_t port, soc_phymod_ctrl_t *pmc, int32 intf, int32 lane)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx = 0, rv = 0;
    phymod_phy_access_t     pm_phy_copy;
    soc_phymod_phy_t        *p_phy;
    uint32_t lane_map = 0;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        p_phy = pmc->phy[idx];
        
        /* Make a copy of the phy access and overwrite the desired lane */
        pm_phy = &p_phy->pm_phy;
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));

        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY_DIAG_INTF_SYS) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        }
        if (lane == 0xf) {
            /* Get p_phy alone and let lane map be as Phy structure value */
            lane = 1;
            SOC_IF_ERROR_RETURN(
                    _phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));
            lane = 0xf;
        } else {
            SOC_IF_ERROR_RETURN(
                _phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));
        }

        /* Make a copy of the phy access and overwrite the desired lane */
        if (lane != 0xf) {
           pm_phy_copy.access.lane_mask = lane_map;
        }

        rv = phymod_phy_eyescan_run(&pm_phy_copy, PHYMOD_EYESCAN_F_DONE, 0, NULL);
        if (rv != SOC_E_NONE) {
            return rv;
        }
    }
    return SOC_E_NONE;
}
STATIC int 
phy_82780_diag_dsc(soc_phymod_ctrl_t *pmc, int32 intf)
{
    phymod_phy_access_t  *pm_phy;
    int                    idx;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    phymod_phy_diagnostics_t phy_diag;
    uint16_t lane_index = 0;


    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        p_phy = pmc->phy[idx];
        /* Make a copy of the phy access and overwrite the desired lane */
        pm_phy = &p_phy->pm_phy;
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
        if(intf == PHY_DIAG_INTF_SYS) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        }
        LOG_CLI((BSL_META_U(pmc->unit,
                                    "DSC parameters for port %d\n"), p_phy->core->port));
        for (lane_index = 0; lane_index < 4; lane_index ++) {
            if ((pm_phy->access.lane_mask & (1 << lane_index))) {
                pm_phy_copy.access.lane_mask = (1 << lane_index);
                SOC_IF_ERROR_RETURN(phymod_phy_diagnostics_get(&pm_phy_copy, &phy_diag));
     
                /* display */
                LOG_CLI((BSL_META_U(pmc->unit,
                                    "LaneN SIGDET VGABIASREDUCED POSTCMET OSR PMDMODE "
                                    "RXLOCK RXPPM TXPPM CLK90OFFSET CLKP1OFFSET P1LVL1 "
                                    "M1LVL1 DFE1_DCD DFE2_DCD SLICERTARGET PE ZE ME "
                                    "PO ZO MO HLFT HRGT VUPPR VLOWR SMSTS LNKTM "
                                    "MAIN HIZ BST\n")));
                LOG_CLI((BSL_META_U(pmc->unit,
                                    "%05d %03d     %06d        %07d  %03d  %05d  %05d %05d %06d  %05d        %08d "
                                    "  %03d   %05d    %04d   %04d      %04d        %02d %02d %02d %02d %02d %02d"
                                    " %03d  %04d %05d %05d %04d  %05d %04d %03d %03d \n"), 
                         (lane_index+1), phy_diag.signal_detect, phy_diag.vga_bias_reduced,
                         phy_diag.postc_metric, phy_diag.osr_mode, phy_diag.pmd_mode,
                         phy_diag.rx_lock, phy_diag.rx_ppm, phy_diag.tx_ppm,
                         phy_diag.clk90_offset, phy_diag.clkp1_offset,
                         phy_diag.p1_lvl, phy_diag.m1_lvl, phy_diag.dfe1_dcd,
                         phy_diag.dfe2_dcd, phy_diag.slicer_target, 
                         phy_diag.slicer_offset.offset_pe, phy_diag.slicer_offset.offset_ze,
                         phy_diag.slicer_offset.offset_me, phy_diag.slicer_offset.offset_po,
                         phy_diag.slicer_offset.offset_zo,  phy_diag.slicer_offset.offset_mo,
                         phy_diag.eyescan.heye_left, phy_diag.eyescan.heye_right,
                         phy_diag.eyescan.veye_upper, phy_diag.eyescan.veye_lower,
                         phy_diag.state_machine_status, phy_diag.link_time,
                         phy_diag.pf_main, phy_diag.pf_hiz,
                         phy_diag.pf_bst));
                LOG_CLI((BSL_META_U(pmc->unit,
                                    "LOW PF2CTRL VGA DCOFFT P1LVLCTRL "
                                    "DFE1 DFE2 DFE3 DFE4 DFE5 DFE6 PRE MAIN POST1 "
                                    "POST2 POST3 TXAMPCTRL BRPDEN \n")));

                LOG_CLI((BSL_META_U(pmc->unit,
                                    "%03d %07d %03d %06d   %04d    %04d %04d %04d %04d %04d %04d %04d"
                                    " %03d %04d  %05d %05d   %04d    %04d\n\n"), 
                         phy_diag.pf_low,
                         phy_diag.pf2_ctrl, phy_diag.vga,
                         phy_diag.dc_offset, phy_diag.p1_lvl_ctrl,
                         phy_diag.dfe1, phy_diag.dfe2,
                         phy_diag.dfe3, phy_diag.dfe4,
                         phy_diag.dfe5, phy_diag.dfe6,
                         phy_diag.txfir_pre, phy_diag.txfir_main,
                         phy_diag.txfir_post1, phy_diag.txfir_post2,
                         phy_diag.txfir_post3, phy_diag.tx_amp_ctrl,
                         phy_diag.br_pd_en));
            }
        }
    }
    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_82780_diag_ctrl
 * Purpose:
 *      xx
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 

 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_82780_diag_ctrl(
    int unit,        /* unit */
    soc_port_t port, /* port */
    uint32 inst,     /* the specific device block the control action directs to */
    int op_type,     /* operation types: read,write or command sequence */
    int op_cmd,      /* command code */
    void *arg)       /* command argument based on op_type/op_cmd */
{
    int rv = -1;
    int32 lane;
    int32 intf;
    soc_phymod_ctrl_t   *pmc;
    phy_ctrl_t          *pc;
    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;

    lane = PHY_DIAG_INST_LN(inst);
    intf = PHY_DIAG_INST_INTF(inst);
    if (intf == PHY_DIAG_INTF_DFLT) {
        intf = PHY_DIAG_INTF_LINE;
    }
    if (intf == PHY_DIAG_INTF_SYS) {
        pc->flags |= PHYCTRL_SYS_SIDE_CTRL;
    } else {
        pc->flags &= ~PHYCTRL_SYS_SIDE_CTRL;
    }
    switch(op_cmd) {
        case PHY_DIAG_CTRL_DSC:
            rv = phy_82780_diag_dsc(pmc, intf);
            break;
        case PHY_DIAG_CTRL_START_FAST_EYESCAN:
            rv =  phy_82780_diag_eyescan(port, pmc, intf, lane);
        break;
        case PHY_DIAG_CTRL_LINKMON_MODE:
        {
            uint32_t value = PTR_TO_INT(arg);
            rv = phy_82780_link_monitor_set(pmc, intf, lane, value);
        }
        break;
        case PHY_DIAG_CTRL_LINKMON_STATUS:
        {
            rv = phy_82780_link_monitor_status_get(pmc, intf, lane);
        }
        break;

        default:
            if (op_type == PHY_DIAG_CTRL_SET) {
                    rv = phy_82780_control_set(unit,port,op_cmd,PTR_TO_INT(arg));
            } else if (op_type == PHY_DIAG_CTRL_GET) {
                    rv = phy_82780_control_get(unit,port,op_cmd,(uint32 *)arg);
            }
            break ;
    }

    /* clear the pc->flags  to make default as PHY_DIAG_INTF_LINE */
    pc->flags &= ~PHYCTRL_SYS_SIDE_CTRL;

    return rv;
}

int32 phy_82780_core_reset(int32 unit, soc_port_t port, void *not_used)
{
    phy_ctrl_t                *pc;
    soc_phymod_core_t *pmc_core;
    int                  idx;
    phymod_core_access_t *pm_core;
    soc_phymod_ctrl_t *pmc;
    phymod_reset_direction_t reset_direction;
    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    reset_direction = phymodResetDirectionInOut;
    pmc = &pc->phymod_ctrl; 

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pmc_core = pmc->phy[idx]->core;
        pm_core = &pmc_core->pm_core;
        if (pm_core == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_core_reset_set(pm_core, phymodResetModeSoft, reset_direction));
    }
    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_tx_set
 */
STATIC int
phy_82780_per_lane_tx_set(soc_phymod_ctrl_t *pmc, int32 intf, soc_phy_control_t type, int lane, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 
    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    switch (type) {
        case SOC_PHY_CONTROL_TX_FIR_PRE:
            phymod_tx.pre = value;
            break;
        case SOC_PHY_CONTROL_TX_FIR_MAIN:
            phymod_tx.main = value;
            break;
        case SOC_PHY_CONTROL_TX_FIR_POST:
            phymod_tx.post = value;
            break;
        case SOC_PHY_CONTROL_TX_FIR_POST2:
            phymod_tx.post2 = value;
            break;
        default:
            return SOC_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));

    return(SOC_E_NONE);
}

/* 
 * phy_82780_per_lane_tx_get
 */
STATIC int
phy_82780_per_lane_tx_get(soc_phymod_ctrl_t *pmc, int32 intf, soc_phy_control_t type, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.amp;
    switch (type) {
        case SOC_PHY_CONTROL_TX_FIR_PRE:
            *value = phymod_tx.pre;
            break;
        case SOC_PHY_CONTROL_TX_FIR_MAIN:
            *value = phymod_tx.main;
            break;
        case SOC_PHY_CONTROL_TX_FIR_POST:
            *value = phymod_tx.post;
            break;
        case SOC_PHY_CONTROL_TX_FIR_POST2:
            *value = phymod_tx.post2;
            break;
        default:
            return SOC_E_UNAVAIL;
    }

    return(SOC_E_NONE);
}

/*
* Function:
*      _phy_84328_ability_remote_get
* Purpose:
*      Get the device's complete abilities.
* Parameters:
*      unit - StrataSwitch unit #.
*      port - StrataSwitch port #.
*      ability - return device's abilities.
* Returns:
*      SOC_E_XXX
*/
STATIC int
phy_82780_ability_remote_get(int unit, soc_port_t port, 
                                  soc_port_ability_t *ability)
{
    int an, an_done;
    phy_ctrl_t *pc, *int_pc;
    int  line_intf;
    phymod_autoneg_ability_t  phymod_autoneg_ability; 
    uint32_t an_cap = 0;
    soc_phymod_ctrl_t         *pmc;
    phy82780_config_t         *pCfg;
    soc_phymod_phy_t          *phy;
 

    if (ability == NULL) {
        return SOC_E_PARAM;
    }
    pc = EXT_PHY_SW_STATE(unit, port);

    
    /* Only firmware has visibility into remote ability */
	sal_memset(ability, 0, sizeof(soc_port_ability_t));

    pmc = &pc->phymod_ctrl;
    pCfg = (phy82780_config_t *) pc->driver_data;

    /* only set abilities on the first core */
    phy = pmc->phy[pmc->main_phy];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }
    phymod_autoneg_ability_t_init(&phymod_autoneg_ability);


    line_intf = pCfg->speed_config.line_interface;

    if ((line_intf == SOC_PORT_IF_KX) || (line_intf == SOC_PORT_IF_GMII)) {
        int_pc = INT_PHY_SW_STATE(unit, port);
        if (int_pc) {
            SOC_IF_ERROR_RETURN(PHY_ABILITY_REMOTE_GET(int_pc->pd, unit, port, ability));
        }
        return SOC_E_NONE;
    }

    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;

	/* Return what the firmware is setting */
    /* If an done, then return the speed for the port */
    SOC_IF_ERROR_RETURN(phy_82780_an_get(unit, port, &an, &an_done));
    if (an && an_done) {
       SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_remote_ability_get(&phy->pm_phy, &phymod_autoneg_ability));
       an_cap = phymod_autoneg_ability.an_cap;
       ability->speed_full_duplex |= PHYMOD_AN_CAP_40G_CR4_GET(an_cap) ? SOC_PA_SPEED_40GB : 0;
       ability->speed_full_duplex |= PHYMOD_AN_CAP_40G_KR4_GET(an_cap) ? SOC_PA_SPEED_40GB : 0;
       ability->speed_full_duplex |= PHYMOD_AN_CAP_10G_KR_GET(an_cap)  ? SOC_PA_SPEED_10GB : 0;
        
        an_cap = phymod_autoneg_ability.capabilities;
        if (an_cap == PHYMOD_AN_CAP_ASYM_PAUSE) {
            ability->pause = SOC_PA_PAUSE_TX;
        } else if (an_cap == (PHYMOD_AN_CAP_SYMM_PAUSE | PHYMOD_AN_CAP_ASYM_PAUSE)) {
            ability->pause = SOC_PA_PAUSE_RX;
        } else if (an_cap == PHYMOD_AN_CAP_SYMM_PAUSE) {
            ability->pause = (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX);
        }
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_84328_ability_remote_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));
    
    return SOC_E_NONE;
}

STATIC int
phy_82780_cl72_enable_set(soc_phymod_ctrl_t *pmc, int32 intf, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;
    phymod_phy_access_t pm_phy_copy;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 

        if (intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 
#ifdef PHY82780_DEBUG
        LOG_CLI((BSL_META_U(0,"Tx training: %d Flags:0x%x\n"), value, pm_phy_copy.access.flags));
#endif
        SOC_IF_ERROR_RETURN(phymod_phy_cl72_set(&pm_phy_copy, value));
    }

    return(SOC_E_NONE);
}


STATIC int
phy_82780_cl72_status_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;
    phymod_phy_access_t pm_phy_copy;
    phymod_cl72_status_t status;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 

        if (intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        sal_memset(&status, 0, sizeof(status));
        SOC_IF_ERROR_RETURN(phymod_phy_cl72_status_get(&pm_phy_copy, &status));
        *value = status.locked;
    }
#ifdef PHY82780_DEBUG
    LOG_CLI((BSL_META_U(0,"Tx training Sts: %d Flags:0x%x\n"), *value, pm_phy_copy.access.flags));
#endif

    return(SOC_E_NONE);
}

STATIC int
phy_82780_cl72_enable_get(soc_phymod_ctrl_t *pmc, int32 intf, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;
    phymod_phy_access_t pm_phy_copy;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 

        if (intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 

        SOC_IF_ERROR_RETURN(phymod_phy_cl72_get(&pm_phy_copy, value));
    }
    return(SOC_E_NONE);
}

STATIC int
phy_82780_link_monitor_set (soc_phymod_ctrl_t *pmc, int32 intf, int32 lane, uint32 value)
{
    phymod_phy_access_t    *pm_phy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map = 0;
    phymod_phy_access_t pm_phy_copy;
    uint32_t link_mon_mode = 0;

    intf = (intf == PHY_DIAG_INTF_LINE) ? PHY82780_LINE_SIDE : PHY82780_SYS_SIDE;
    if (lane == 0xf) {
        /* Get p_phy alone and let lane map be as Phy structure value */
        lane = 1;
        SOC_IF_ERROR_RETURN(
                _phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));
        lane = 0xf;
    } else {
        SOC_IF_ERROR_RETURN(
                _phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));
    }

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    if (lane != 0xf) {
        pm_phy_copy.access.lane_mask = lane_map;
    }

    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    if (value == 0) {
        SOC_IF_ERROR_RETURN(
            phymod_phy_link_mon_enable_set(&pm_phy_copy, 0 /*When disable Dont care*/, 0));
    } else {
        link_mon_mode = value - 1;
        SOC_IF_ERROR_RETURN(
            phymod_phy_link_mon_enable_set(&pm_phy_copy, link_mon_mode, 1));
    }

    return(SOC_E_NONE);
}

STATIC int
phy_82780_link_monitor_status_get (soc_phymod_ctrl_t *pmc, int32 intf, int32 lane)
{
    phymod_phy_access_t    *pm_phy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map = 0;
    phymod_phy_access_t pm_phy_copy;
    uint32_t lock_status = 0, lock_lost_lh = 0, error_count = 0;

    intf = (intf == PHY_DIAG_INTF_LINE) ? PHY82780_LINE_SIDE : PHY82780_SYS_SIDE;
    if (lane == 0xf) {
        /* Get p_phy alone and let lane map be as Phy structure value */
        lane = 1;
        SOC_IF_ERROR_RETURN(
                _phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));
        lane = 0xf;
    } else {
        SOC_IF_ERROR_RETURN(
                _phy_82780_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));
    }

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    if (lane != 0xf) {
        pm_phy_copy.access.lane_mask = lane_map;
    }

    PHY82780_CLEAR_SYS_SIDE(pm_phy_copy); 
    
    if(intf == PHY82780_SYS_SIDE) {
        PHY82780_SET_SYS_SIDE(pm_phy_copy); 
    } 

    SOC_IF_ERROR_RETURN(
          phymod_phy_link_mon_status_get(&pm_phy_copy, &lock_status, &lock_lost_lh, &error_count));

    if (lane != 0xF) { 
        LOG_CLI((BSL_META_U(0,"Lane:%d Lock Status:%d lock lost:%d Error count:%d\n "), 
                lane, lock_status, lock_lost_lh, error_count));
    } else {
        LOG_CLI((BSL_META_U(0,"Lane Map:0x%x Lock Status:%d lock lost:%d Error count:%d\n "), 
                pm_phy_copy.access.lane_mask, lock_status, lock_lost_lh, error_count));
    }

    return(SOC_E_NONE);

}

STATIC int
phy_82780_adoptation_release(phy_ctrl_t *pc, int32 intf)
{
    phymod_phy_access_t *pm_phy;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_ctrl_t *pmc;
    pmc = &pc->phymod_ctrl;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        if (intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        } 
        SOC_IF_ERROR_RETURN(phymod_phy_rx_adaptation_resume(&pm_phy_copy));
    }
    return(SOC_E_NONE);
}

STATIC int
phy_82780_config_fail_over(phy_ctrl_t *pc, int32 intf, uint32_t value)
{
    phymod_phy_access_t *pm_phy;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_ctrl_t *pmc;
    phymod_failover_mode_t failover_mode;
    pmc = &pc->phymod_ctrl;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        if (intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        }
        failover_mode = value? phymodFailovermodeEnable : phymodFailovermodeNone; 
        SOC_IF_ERROR_RETURN(
                phymod_failover_mode_set(&pm_phy_copy, failover_mode));
    }
    return(SOC_E_NONE);
}

STATIC int
phy_82780_config_fail_over_get(phy_ctrl_t *pc, int32 intf, uint32_t *value)
{
    phymod_phy_access_t *pm_phy;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_ctrl_t *pmc;
    phymod_failover_mode_t failover_mode;
    pmc = &pc->phymod_ctrl;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        if (intf == PHY82780_SYS_SIDE) {
            PHY82780_SET_SYS_SIDE(pm_phy_copy); 
        }
        SOC_IF_ERROR_RETURN(
                phymod_failover_mode_get(&pm_phy_copy, &failover_mode));
        if (failover_mode == phymodFailovermodeEnable) {
            *value = 1;
        } else {
            *value = 0;
        }
    }
    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_82780_precondition_before_probe
 * Purpose:
 *      When the device is in 40G, ie. after a power reset, and the 10G
 *      port being probed does not have the 40G address in PRTAD, the
 *      device will not respond until the port that has the 40G address
 *      is probed. To avoid not programming ports correctly, make
 *      sure the device is in quad (4x10G) PMD mode when the SDK port is 10G.
 *      Need to set "port_phy_precondition_before_probe_xe0/4" in config file.
 * Parameters: 
 *      unit - StrataSwitch unit #.
 *      pc   - phy ctrl descriptor.
 * Returns:
 *      SOC_E_NONE,SOC_E_NOT_FOUND and SOC_E_<error>
 */


STATIC int32
phy_82780_precondition_before_probe(int32 unit, phy_ctrl_t *pc)
{
    uint16_t val;
    int max_lane = 4, index = 0;
    uint16 saved_phy_id;
    int rv = SOC_E_NOT_FOUND;

    if (SOC_INFO((pc)->unit).port_num_lanes[(pc)->port] >= 4) {
        return rv;
    }
    SOC_IF_ERROR_RETURN(
        READ_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(0x1, 0xc802), &val));
    if (val == 0x2752) {
        max_lane = 2;
    } else if (val == 0x2780 || val == 0x2758 || val == 0x278F) {
        max_lane = 4;   
    } else {
        return rv;
    }
    saved_phy_id = pc->phy_id;
    for (index = 0; index < max_lane; index ++) {
        pc->phy_id = (saved_phy_id & ~0x3) + index;
        SOC_IF_ERROR_RETURN(
            READ_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(0x1, 0x2), &val));
        if (val != 0xAE02) {
           continue;
        }
        SOC_IF_ERROR_RETURN(
            READ_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(0x1, 0x3), &val));
        if (val != 0x5250) {
           continue;
        }

        SOC_IF_ERROR_RETURN(
            READ_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(0x1, 0xc843), &val));
        if (((val & 0xf) == 0x4) || ((val & 0xf) == 0x7)) {
            SOC_IF_ERROR_RETURN(
                WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(0x1, 0xc712), 1));

            val &= ~(0x80);
            SOC_IF_ERROR_RETURN(
                WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(0x1, 0xc8d8), val));
            val = 0x8882;
            SOC_IF_ERROR_RETURN(
                WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(0x1, 0xc8d8), val));

            SOC_IF_ERROR_RETURN(
                READ_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(0x1, 0x0), &val));
            val |= (0x8000);
            SOC_IF_ERROR_RETURN(
                WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(0x1, 0x0), val));

            sal_usleep(5000);
        }
        rv = SOC_E_NONE;
        break;
    }
    pc->phy_id = saved_phy_id;

    return rv;
}

STATIC int
phy_82780_multi_get(int unit, soc_port_t port, uint32 flags,
                    uint32 dev_addr, uint32 offset, int max_size, uint8 *data,
                    int *actual_size)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[pmc->main_phy]->pm_phy;

    *actual_size = 0;
    SOC_IF_ERROR_RETURN
        (phymod_phy_i2c_read(pm_phy, 0, dev_addr, offset, max_size, data)); 

    *actual_size = max_size;
    return SOC_E_NONE;
}
/*
 * Variable:
 *      phy82780_drv
 * Purpose:
 *      Phy Driver for phy82780 
 */
phy_driver_t phy82780_drv = {
    /* .drv_name                      = */ "PHY82780 PHYMOD PHY Driver",
    /* .pd_init                       = */ phy_82780_init,
    /* .pd_reset                      = */ phy_82780_core_reset,
    /* .pd_link_get                   = */ phy_82780_link_get,
    /* .pd_enable_set                 = */ phy_82780_enable_set,
    /* .pd_enable_get                 = */ phy_82780_enable_get,
    /* .pd_duplex_set                 = */ phy_null_set,
    /* .pd_duplex_get                 = */ phy_null_duplex_get,
    /* .pd_speed_set                  = */ phy82780_speed_set,
    /* .pd_speed_get                  = */ phy82780_speed_get,
    /* .pd_master_set                 = */ phy_null_set,
    /* .pd_master_get                 = */ phy_null_zero_get,
    /* .pd_an_set                     = */ phy_82780_an_set,
    /* .pd_an_get                     = */ phy_82780_an_get,
    /* .pd_adv_local_set              = */ NULL, /* Deprecated */
    /* .pd_adv_local_get              = */ NULL, /* Deprecated */
    /* .pd_adv_remote_get             = */ NULL, /* Deprecated */ 
    /* .pd_lb_set                     = */ phy_82780_lb_set,
    /* .pd_lb_get                     = */ phy82780_lb_get,
    /* .pd_interface_set              = */ phy82780_interface_set,
    /* .pd_interface_get              = */ phy82780_interface_get,
    /* .pd_ability                    = */ NULL, /* Deprecated */ 
    /* .pd_linkup_evt                 = */ NULL,
    /* .pd_linkdn_evt                 = */ NULL,
    /* .pd_mdix_set                   = */ phy_null_mdix_set,
    /* .pd_mdix_get                   = */ phy_null_mdix_get,
    /* .pd_mdix_status_get            = */ phy_null_mdix_status_get,
    /* .pd_medium_config_set          = */ NULL,
    /* .pd_medium_config_get          = */ NULL,
    /* .pd_medium_get                 = */ phy_null_medium_get,
    /* .pd_cable_diag                 = */ NULL,
    /* .pd_link_change                = */ NULL,
    /* .pd_control_set                = */ phy_82780_control_set,
    /* .pd_control_get                = */ phy_82780_control_get,
    /* .pd_reg_read                   = */ phy82780_reg_read,
    /* .pd_reg_write                  = */ phy82780_reg_write,
    /* .pd_reg_modify                 = */ phy82780_reg_modify,
    /* .pd_notify                     = */ NULL,
    /* .pd_probe                      = */ phy82780_probe,
    /* .pd_ability_advert_set         = */ phy_82780_ability_advert_set, 
    /* .pd_ability_advert_get         = */ phy_82780_ability_advert_get,
    /* .pd_ability_remote_get         = */ phy_82780_ability_remote_get,
    /* .pd_ability_local_get          = */ phy_82780_ability_local_get,
    /* .pd_firmware_set               = */ phy_82780_firmware_set,
    /* .pd_timesync_config_set        = */ NULL,
    /* .pd_timesync_config_get        = */ NULL,
    /* .pd_timesync_control_set       = */ NULL,
    /* .pd_timesync_control_set       = */ NULL,
    /* .pd_diag_ctrl                  = */ phy_82780_diag_ctrl,
    /* .pd_lane_control_set           = */ phy_82780_per_lane_control_set,    
    /* .pd_lane_control_get           = */ phy_82780_per_lane_control_get,
    /* pd_lane_reg_read */                 NULL,                        
    /* pd_lane_reg_write */                NULL,                        
    /* pd_oam_config_set */                NULL,                        
    /* pd_oam_config_get */                NULL,                        
    /* pd_oam_control_set */               NULL,                        
    /* pd_oam_control_get */               NULL,                        
    /* pd_timesync_enhanced_capture_get */ NULL,                        
    /* pd_multi_get */                     phy_82780_multi_get,                        
    /* pd_precondition_before_probe */     phy_82780_precondition_before_probe

};

#else
typedef int _phy82780_not_empty;
#endif /*  INCLUDE_PHY_82780 */

