/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy82764.c
 * Purpose:     Support for sesto external PHY 
 */

/*
 *   This module implements an NTSW SDK Phy driver for the phy82764 External PHY.
 *  
 *   LAYERING.
 *
 *   This driver is built on top of the PHYMOD library, which is a PHY
 *   driver library that can operate on a family of PHYs, including
 *   phy82764 and Sesto and etc..,  PHYMOD can be built in a standalone enviroment and be
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
 *     APIs (nor ".pd_xx" calls, nor to .phy82764_per_lane_control_set and
 *     .phy82764_per_lane_control_get APIs.
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
 *   sesto_config_t
 *   Driver specific data.  This structure is used by sesto to hold
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
#if defined(INCLUDE_PHY_82764)
#include "phyconfig.h"     /* Must include before other phy related includes */

#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/spl.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/eyescan.h>
#include <soc/port_ability.h>
#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>
#include <soc/phy/phymod_ids.h>

#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "xgxs.h"


#define PHYMOD_SESTO_SUPPORT



#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/chip/sesto.h>


#define NUM_LANES                           8     /* num of lanes per core */
#define MAX_NUM_LANES                       4     /* max num of lanes per port */
#define PHY82764_NO_CFG_VALUE               (-1)
#define PHY82764_DBG_REGACC                 0x2
#define PHY82764_DBG_MEM                    0x3
#define PHY82764_PORT_NO_LANES_4            4
#define PHY82764_PORT_NO_LANES_2            2
#define PHY82764_PORT_NO_LANES_1            1
#define PHY82764_SPD_1G                     1000
#define PHY82764_SPD_10G                    10000
#define PHY82764_SPD_11G                    11000
#define PHY82764_SPD_20G                    20000
#define PHY82764_SPD_21G                    21000
#define PHY82764_SPD_25G                    25000
#define PHY82764_SPD_40G                    40000
#define PHY82764_SPD_42G                    42000
#define PHY82764_SPD_50G                    50000
#define PHY82764_SPD_100G                   100000
#define PHY82764_SPD_106G                   106000
#define SESTO_ID_82764                      0x82764
#define PHY82764_CHIP_ID_SIZE               25

typedef enum PHY82764_CORE_TYPE {
    PHY82764_10G_CORE = 0,
    PHY82764_25G_CORE = 1
}PHY82764_CORE_TYPE;

typedef struct phy82764_device_cfg_aux_mode{
    uint8 gearbox_enable;
    uint8 pin_compatibility_enable;
    uint8 phy_mode_reverse;
}phy82764_device_cfg_aux_mode_t;

typedef struct phy82764_device_aux_mode {
    uint32_t repeater;
    uint32_t core_id;
} phy82764_device_aux_mode_t;

typedef struct phy82764_speed_config_s {
    uint32  port_refclk_int;
    int     speed;
    int     port_num_lanes;
    uint32  line_interface;
    int     pll_divider_req  ;
    int     higig_port;
    int     otn_port;
} phy82764_speed_config_t;

typedef enum PHY82764_INTF_SIDE {
    PHY82764_NO_LOC,
    PHY82764_LINE_SIDE,
    PHY82764_SYS_SIDE 
}PHY82764_INTF_SIDE;

/*
   Config - per logical port
*/
typedef struct {
    phymod_polarity_t               phy_polarity_config;
    phymod_phy_reset_t              phy_reset_config;
    phy82764_speed_config_t         speed_config;
    int                             an_master_lane;
    int                             ull_datapath;
    int                             devid;
    char                            dev_name[PHY82764_CHIP_ID_SIZE];
    int                             auto_mod_detect;
    int                             sys_lane_map;  
    phy82764_device_aux_mode_t      *device_aux_modes;
    phy82764_device_cfg_aux_mode_t  device_cfg_aux_modes;
} phy82764_config_t;

#define DEVID(_pc)           (((phy82764_config_t *)((_pc) + 1))->devid)
#define DEV_NAME(_pc)        (((phy82764_config_t *)((_pc) + 1))->dev_name)

int eye_scan_enabled = 0;

STATIC int
phy_82764_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability);

STATIC int
phy_82764_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int
_phy_82764_find_soc_phy_lane(soc_phymod_ctrl_t *pmc, soc_port_t port, uint32_t lane,
                             soc_phymod_phy_t **p_phy, uint32 *lane_map);

STATIC int
phy_82764_power_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value);
STATIC int
phy_82764_power_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value);
STATIC int
phy_82764_per_lane_tx_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, soc_phy_control_t type, int lane, uint32 *value);
STATIC int
phy_82764_per_lane_tx_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, soc_phy_control_t type, int lane, uint32 value);
STATIC int
phy_82764_per_lane_short_chn_mode_enable_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value);
STATIC int
phy_82764_per_lane_short_chn_mode_status_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value);
STATIC int
phy_82764_per_lane_short_chn_mode_enable_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value);
STATIC int
phy_82764_per_lane_cl72_enable_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value);
STATIC int
phy_82764_per_lane_cl72_status_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value);
STATIC int
phy_82764_per_lane_cl72_enable_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value);
STATIC int
phy_82764_cl72_enable_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value);
STATIC int
phy_82764_cl72_status_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value);
STATIC int
phy_82764_cl72_enable_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value);
STATIC int
phy_82764_per_lane_tx_lane_squelch_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value);
STATIC int
phy_82764_loopback_internal_pmd_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value);
STATIC int
phy_82764_loopback_internal_pmd_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value);
STATIC int
phy_82764_link_monitor_status_get (soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int32 lane);
STATIC int
phy_82764_link_monitor_set (soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int32 lane, uint32 value);
STATIC int
phy_82764_intr_enable_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value);
STATIC int
phy_82764_intr_enable_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value);
STATIC int
phy_82764_intr_status_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value);
STATIC int
phy_82764_tx_fir_post_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value);
STATIC int
phy_82764_driver_current_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value);
STATIC int
phy_82764_tx_fir_main_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value);
STATIC int
phy_82764_tx_fir_pre_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value);
STATIC int
phy_82764_tx_fir_post2_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value);
STATIC int
phy_82764_firmware_lp_dfe_enable_get(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 *value);
STATIC int
phy_82764_unreliable_los_get(phy_ctrl_t *pc, soc_port_t port, int32 intf,  uint32 *value);
STATIC int
phy_82764_firmware_dfe_enable_get(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 *value);
STATIC int
phy_82764_firmware_mode_get(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 *value);
STATIC int
phy_82764_firmware_lp_dfe_enable_set(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 enable);
STATIC int
phy_82764_unreliable_los_set(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 enable);
STATIC int
phy_82764_firmware_dfe_enable_set(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 enable);
STATIC int
phy_82764_firmware_mode_set(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 value);
STATIC int
_phy82764_speed_to_lane_map_get(phy_ctrl_t *pc, uint32 *line_lane_map, uint32 *sys_lane_map);
STATIC int
phy_82764_per_lane_control_set(int unit, soc_port_t port, int lane, soc_phy_control_t type, uint32 value);
STATIC int
phy_82764_per_lane_control_get(int unit, soc_port_t port, int lane, soc_phy_control_t type, uint32 *value);

/* C To Interger */
STATIC int my_ctoi(const char *s, int lane)
{
    if (s == 0) {
        return 0;
    }

    if (*s == '0') {
        s++;
        if (*s == 'x' || *s == 'X') {
            s++;
        }
    }
    s = s + (sal_strlen(s) - (lane + 1));

    return (*s -'0');
}
/* no of lanes on line side per port */
int port_num_lanes_line_side_get(int unit, soc_port_t port, int speed_max)
{
    if (speed_max == PHY82764_SPD_100G || speed_max == PHY82764_SPD_106G) {
        return 10;
    } else if (speed_max == PHY82764_SPD_40G || speed_max == PHY82764_SPD_42G) {
        return 4;
    } else if (speed_max == PHY82764_SPD_20G || speed_max == PHY82764_SPD_21G) {
        return 2;
    } else {
        return 1;
    }
}

/*
 * Function:
 *      _phy82764_sys_side_lane_map_get
 * Purpose:
 *      Mapping to speed to lane map based on config
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number.
 *      phymod_phy_access_t - [OUT] Line Lane map
 * Returns:
 *      SOC_E_NONE
 */


STATIC int
_phy82764_sys_side_lane_map_get(int unit, soc_port_t port, phymod_phy_access_t *pm_phy_copy)
{
    phy_ctrl_t *pc;
    phy82764_config_t *pCfg;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pCfg = (phy82764_config_t *) pc->driver_data;

    pm_phy_copy->access.lane_mask = pCfg->sys_lane_map;

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy82764_reg_read
 * Doc:
 *      register read operations
 * Parameters:
 *      unit            - (IN)  unit number
 *      core_addr       - (IN)  core address
 *      reg_addr        - (IN)  address to read
 *      val             - (OUT) read value
 */
STATIC int 
_phy82764_reg_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    uint16 data16 = 0;
    int rv = 0;
    soc_phymod_core_t *core = (soc_phymod_core_t *)user_acc;

    rv = core->read(core->unit, core_addr, reg_addr, &data16);
    *val = data16;
    PHYMOD_VDBG(PHY82764_DBG_REGACC, NULL,("-22%s: core_addr: 0x%08x reg_addr: 0x%08x, data: 0x%04x\n", __func__, core_addr, reg_addr, *val ));

    return rv;
}

/*
 * Function:
 *      _phy82764_reg_write
 * Doc:
 *      register write operations
 * Parameters:
 *      unit            - (IN)  unit number
 *      core_addr       - (IN)  core address
 *      reg_addr        - (IN)  address to write
 *      val             - (IN)  write value
 */
STATIC int 
_phy82764_reg_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
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
    PHYMOD_VDBG(PHY82764_DBG_REGACC, NULL,("-22%s: core_addr: 0x%08x reg_addr: 0x%08x, data: 0x%04x\n", __func__, core_addr, reg_addr, val ));
    return core->write(core->unit, core_addr, reg_addr, data16);
}

/*
 * Function:
 *      phy82764_speed_to_interface_config_get
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
phy82764_speed_to_interface_config_get(phy82764_speed_config_t* speed_config, phymod_phy_inf_config_t* interface_config)
{
    SOC_IF_ERROR_RETURN(phymod_phy_inf_config_t_init(interface_config));

    interface_config->interface_modes = 0;
    interface_config->data_rate       = speed_config->speed;
    interface_config->pll_divider_req = speed_config->pll_divider_req ;

    switch (speed_config->speed) {
        case PHY82764_SPD_1G:
            switch(speed_config->line_interface) {
                case SOC_PORT_IF_SR:
                    interface_config->interface_type = phymodInterfaceSR;
                break;
                default:
                    return SOC_E_PARAM;

            }
        break;
        case PHY82764_SPD_10G:
        case PHY82764_SPD_11G:
        case PHY82764_SPD_25G:
            switch(speed_config->line_interface) {
                case SOC_PORT_IF_KR:
                    interface_config->interface_type = phymodInterfaceKR;
                break;
                case SOC_PORT_IF_LR:
                    interface_config->interface_type = phymodInterfaceLR;
                break;
                case SOC_PORT_IF_ER:
                    interface_config->interface_type = phymodInterfaceER;
                break;
                case SOC_PORT_IF_SR:
                    interface_config->interface_type = phymodInterfaceSR;
                break;
                case SOC_PORT_IF_CX:
                    interface_config->interface_type = phymodInterfaceCX;
                break;
                case SOC_PORT_IF_CR:
                    interface_config->interface_type = phymodInterfaceCR;
                break;
                case SOC_PORT_IF_SFI:
                    interface_config->interface_type = phymodInterfaceSFI;
                break;
                case SOC_PORT_IF_XFI:
                    interface_config->interface_type = phymodInterfaceXFI;
                break;
                default:
                    return SOC_E_PARAM;
                    
            }
        break;
        case PHY82764_SPD_20G:
        case PHY82764_SPD_21G:
            switch(speed_config->line_interface) {
                case SOC_PORT_IF_KR2:
                    interface_config->interface_type = phymodInterfaceKR2;
                break;
                case SOC_PORT_IF_LR2:
                    interface_config->interface_type = phymodInterfaceLR2;
                break;
                case SOC_PORT_IF_SR2:
                    interface_config->interface_type = phymodInterfaceSR2;
                break;
                case SOC_PORT_IF_CR2:
                    interface_config->interface_type = phymodInterfaceCR2;
                break;
                case SOC_PORT_IF_ER2:
                    interface_config->interface_type = phymodInterfaceER2;
                break;
                /* This changes are to support speed change from 2 lane 20G to 40G and vice versa */
                case SOC_PORT_IF_KR4:
                    interface_config->interface_type = phymodInterfaceKR2;
                break;
                case SOC_PORT_IF_LR4:
                    interface_config->interface_type = phymodInterfaceLR2;
                break;
                case SOC_PORT_IF_SR4:
                    interface_config->interface_type = phymodInterfaceSR2;
                break;
                case SOC_PORT_IF_CR4:
                    interface_config->interface_type = phymodInterfaceCR2;
                break;
                case SOC_PORT_IF_ER4:
                    interface_config->interface_type = phymodInterfaceER2;
                break;
                default:
                    return SOC_E_PARAM;

            }
        break;
        case PHY82764_SPD_40G:
        case PHY82764_SPD_42G:
        case PHY82764_SPD_50G:
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
                case SOC_PORT_IF_ER4:
                    interface_config->interface_type = phymodInterfaceER4;
                break;
                case SOC_PORT_IF_XLAUI:
                    interface_config->interface_type = phymodInterfaceXLAUI;
                break;
                case SOC_PORT_IF_XLPPI:
                    interface_config->interface_type = phymodInterfaceXLPPI;
                break;
                /* This changes are to support speed change from 2 lane 20G to 40G and vice versa */
                case SOC_PORT_IF_KR2:
                    interface_config->interface_type = phymodInterfaceKR4;
                break;
                case SOC_PORT_IF_SR2:
                    interface_config->interface_type = phymodInterfaceSR4;
                break;
                case SOC_PORT_IF_CR2:
                    interface_config->interface_type = phymodInterfaceCR4;
                break;
                case SOC_PORT_IF_ER2:
                    interface_config->interface_type = phymodInterfaceER4;
                break;
                case SOC_PORT_IF_LR2:
                    interface_config->interface_type = phymodInterfaceLR4;
                break;
                default:
                    return SOC_E_PARAM;
                    
            }    
        break;
        case PHY82764_SPD_100G:
        case PHY82764_SPD_106G:
            switch(speed_config->line_interface) {
                case SOC_PORT_IF_SR10:
                    interface_config->interface_type = phymodInterfaceSR10;
                break;
                case SOC_PORT_IF_KR10:
                    interface_config->interface_type = phymodInterfaceKR10;
                break;
                case SOC_PORT_IF_LR10:
                    interface_config->interface_type = phymodInterfaceLR10;
                break;
                case SOC_PORT_IF_CR10:
                    interface_config->interface_type = phymodInterfaceCR10;
                break;
                case SOC_PORT_IF_CAUI:
                    interface_config->interface_type = phymodInterfaceCAUI;
                break;
                case SOC_PORT_IF_VSR:
                    interface_config->interface_type = phymodInterfaceVSR;
                break;
                case SOC_PORT_IF_CAUI_C2C:
                    interface_config->interface_type = phymodInterfaceCAUI4_C2C;
                break;
                case SOC_PORT_IF_CAUI_C2M:
                    interface_config->interface_type = phymodInterfaceCAUI4_C2M;
                break;
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
                case SOC_PORT_IF_ER4:
                    interface_config->interface_type = phymodInterfaceER4;
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
        case 106:
            interface_config->ref_clock = phymodRefClk106Mhz;
        break;
        case 156:
            interface_config->ref_clock = phymodRefClk156Mhz;
        break;
        case 125:
            interface_config->ref_clock = phymodRefClk125Mhz;
        break;
        case 161:
            interface_config->ref_clock = phymodRefClk161Mhz;
        break;
        case 174:
            interface_config->ref_clock = phymodRefClk174Mhz;
        break;
        case 312:
            interface_config->ref_clock = phymodRefClk312Mhz;
        break;
        case 322:
            interface_config->ref_clock = phymodRefClk322Mhz;
        break;
        case 644:
            interface_config->ref_clock = phymodRefClk644Mhz;
        break;
        case 349:
            interface_config->ref_clock = phymodRefClk349Mhz;
        break;
        case 698:
            interface_config->ref_clock = phymodRefClk698Mhz;
        break;
        default:
            return SOC_E_PARAM;
    }
    if (speed_config->higig_port) {
        PHYMOD_INTF_MODES_HIGIG_SET(interface_config);
    }
    if (speed_config->otn_port) {
        PHYMOD_INTF_MODES_OTN_SET(interface_config);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy82764_speed_set
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
phy82764_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t                *pc;
    phy_ctrl_t                *int_pc;
    phy82764_config_t         *pCfg;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       idx;
    int                       int_speed;
    phy82764_speed_config_t   speed_config;
    phymod_phy_inf_config_t   interface_config;
    phymod_phy_inf_config_t   intf_config;
    phy82764_device_cfg_aux_mode_t *device_cfg_aux_modes;
    uint32 line_lane_map, sys_lane_map;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL || int_pc == NULL) {
        return SOC_E_INTERNAL;
    }

    if (speed == 0) {
        return SOC_E_NONE; 
    }
    SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, &int_speed));
    if (int_speed != speed) {
       SOC_IF_ERROR_RETURN(PHY_SPEED_SET(int_pc->pd, unit, port, speed));
       SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, &int_speed));
       if (int_speed != speed) {
           return SOC_E_CONFIG;
       }
    }
    pmc = &pc->phymod_ctrl;
    pCfg = (phy82764_config_t *) pc->driver_data;
    device_cfg_aux_modes = &(pCfg->device_cfg_aux_modes);

    /* take a copy of core and phy configuration values, and set the desired speed */
    sal_memcpy(&speed_config, &pCfg->speed_config, sizeof(speed_config));
    speed_config.speed = speed;

    /* determine the interface configuration */
    SOC_IF_ERROR_RETURN
        (phy82764_speed_to_interface_config_get(&speed_config, &interface_config));

    /* now loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        phy = pmc->phy[idx];
        if (phy == NULL) {
            return SOC_E_INTERNAL;
        }

        phy->pm_phy.device_op_mode = 0;
        /* soc read for phy device operation mode */
        device_cfg_aux_modes->gearbox_enable = (soc_property_port_get(unit, port, spn_PHY_GEARBOX_ENABLE, FALSE));
        device_cfg_aux_modes->pin_compatibility_enable = soc_property_port_get(unit, port,
                                   spn_PHY_PIN_COMPATIBILITY_ENABLE, FALSE);
        device_cfg_aux_modes->phy_mode_reverse = soc_property_port_get(unit, port, spn_PORT_PHY_MODE_REVERSE, 0);

        /* copy the soc config to phymod_core_access */
        /* bit 0 is for gearbox_enable */
        /* bit 1 is for pin_compatibility_enable */
        /* bit 2 is for phy_mode_reverse */
        phy->pm_phy.device_op_mode |= device_cfg_aux_modes->gearbox_enable;
        phy->pm_phy.device_op_mode |= (device_cfg_aux_modes->pin_compatibility_enable << 1);
        phy->pm_phy.device_op_mode |= (device_cfg_aux_modes->phy_mode_reverse << 2);

        if (pc->speed_max != speed) {
            pc->speed_max = speed;
        }
        SOC_IF_ERROR_RETURN
            (_phy82764_speed_to_lane_map_get(pc, &line_lane_map, &sys_lane_map));
        pCfg->sys_lane_map = sys_lane_map;
        phy->pm_phy.access.lane_mask = line_lane_map;

        SOC_IF_ERROR_RETURN
            (phymod_phy_interface_config_set(&phy->pm_phy,
                                             0 /* flags */, &interface_config));
        SOC_IF_ERROR_RETURN
            (phymod_phy_interface_config_get(&phy->pm_phy,
                                             0 /* flags */,  0, &intf_config));
    }

    /* record success */
    pCfg->speed_config.speed = speed;

    /* speed notify form ext phy */
    if (speed != intf_config.data_rate) {
        speed = intf_config.data_rate;
        SOC_IF_ERROR_RETURN(PHY_SPEED_SET(int_pc->pd, unit, port, speed));
        SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, &int_speed));
        if (int_speed != speed) {
            return SOC_E_CONFIG;
        }

        /* record success on speed notify form ext phy*/
        pCfg->speed_config.speed = speed;
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy82764_speed_get
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
phy82764_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phy82764_config_t         *pCfg;
    phymod_phy_inf_config_t   interface_config;
    phy82764_speed_config_t   speed_config;
    phymod_ref_clk_t ref_clock;
    int flag = 0;
    ref_clock = 0;
    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;

    pCfg = (phy82764_config_t *) pc->driver_data;

    /* take a copy of core and phy configuration values, and set the desired speed */
    sal_memcpy(&speed_config, &pCfg->speed_config, sizeof(speed_config));
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
    if(speed_config.speed != interface_config.data_rate) {
        *speed = speed_config.speed;
    } else {
        *speed = interface_config.data_rate;
    }

    return (SOC_E_NONE);
}


STATIC int
phy82764_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    phy_ctrl_t      *pc;
    phy82764_config_t *pCfg;
    phy82764_speed_config_t   speed_config;
    phymod_phy_inf_config_t   interface_config;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    int                       idx = 0;
    phy82764_device_cfg_aux_mode_t *device_cfg_aux_modes;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pCfg = (phy82764_config_t *) pc->driver_data;
    pmc = &pc->phymod_ctrl;
    device_cfg_aux_modes = &(pCfg->device_cfg_aux_modes);

    if (pif >= SOC_PORT_IF_COUNT) {
        return SOC_E_PARAM;
    }
    if (pif == SOC_PORT_IF_MII || pif == SOC_PORT_IF_XGMII || pif == SOC_PORT_IF_GMII) {
		return phy_null_interface_set(unit, port, pif);
	}

    /*Work around for to support flex of 10G To 40G Mux as part of jira PHY-1805 */
    if (((pc->speed_max == PHY82764_SPD_20G) && (device_cfg_aux_modes->gearbox_enable == TRUE)) ||
        (pc->speed_max == PHY82764_SPD_25G) || (pc->speed_max == PHY82764_SPD_50G)){
        return SOC_E_NONE;
    }

    /* take a copy of core and phy configuration values, and set the desired speed */
    sal_memcpy(&speed_config, &pCfg->speed_config, sizeof(speed_config));
    speed_config.line_interface = pif;
    SOC_IF_ERROR_RETURN
        (phy82764_speed_to_interface_config_get(&speed_config, &interface_config));

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
phy82764_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t                *pc;
    phy82764_config_t         *pCfg;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_phy_inf_config_t   interface_config;
    phymod_ref_clk_t          ref_clock;
    phy82764_speed_config_t   speed_config;
    int flag = 0;
    ref_clock = 0;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    pCfg = (phy82764_config_t *) pc->driver_data;
    sal_memcpy(&speed_config, &pCfg->speed_config, sizeof(speed_config));

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
            if ((speed_config.speed == PHY82764_SPD_100G) || (speed_config.speed == PHY82764_SPD_40G) ||
                (speed_config.speed == PHY82764_SPD_106G) || (speed_config.speed == PHY82764_SPD_42G)) {
                *pif = SOC_PORT_IF_KR4;
            } else {
                *pif = SOC_PORT_IF_KR;
            }
        break;
        case phymodInterfaceSR:
            if ((speed_config.speed == PHY82764_SPD_100G) || (speed_config.speed == PHY82764_SPD_40G) ||
                (speed_config.speed == PHY82764_SPD_106G) || (speed_config.speed == PHY82764_SPD_42G)) {
                *pif = SOC_PORT_IF_SR4;
            } else {
                *pif = SOC_PORT_IF_SR;
            }
        break;
        case phymodInterfaceLR:
            if ((speed_config.speed == PHY82764_SPD_100G) || (speed_config.speed == PHY82764_SPD_40G) ||
                (speed_config.speed == PHY82764_SPD_106G) || (speed_config.speed == PHY82764_SPD_42G)) {
                *pif = SOC_PORT_IF_LR4;
            } else {
                *pif = SOC_PORT_IF_LR;
            }
        break;
        case phymodInterfaceCR:
            if ((speed_config.speed == PHY82764_SPD_100G) || (speed_config.speed == PHY82764_SPD_40G) ||
                (speed_config.speed == PHY82764_SPD_106G) || (speed_config.speed == PHY82764_SPD_42G)) {
                *pif = SOC_PORT_IF_CR4;
            } else {
                *pif = SOC_PORT_IF_CR;
            }
        break;
        case phymodInterfaceER:
            if ((speed_config.speed == PHY82764_SPD_100G) || (speed_config.speed == PHY82764_SPD_40G) ||
                (speed_config.speed == PHY82764_SPD_106G) || (speed_config.speed == PHY82764_SPD_42G)) {
                *pif = SOC_PORT_IF_ER4;
            } else {
                *pif = SOC_PORT_IF_ER;
            }
        break;
        case phymodInterfaceSR2:
            *pif = SOC_PORT_IF_SR2;
        break;
        case phymodInterfaceLR2:
            *pif = SOC_PORT_IF_LR2;
        break;
        case phymodInterfaceKR2:
            *pif = SOC_PORT_IF_KR2;
        break;
        case phymodInterfaceCR2:
            *pif = SOC_PORT_IF_CR2;
        break;
        case phymodInterfaceER2:
            *pif = SOC_PORT_IF_ER2;
        break;
        case phymodInterfaceKR4:
            *pif = SOC_PORT_IF_KR4;
        break;
        case phymodInterfaceCR4:
            *pif = SOC_PORT_IF_CR4;
        break;
        case phymodInterfaceSR4:
            *pif = SOC_PORT_IF_SR4;
        break;
        case phymodInterfaceER4:
            *pif = SOC_PORT_IF_ER4;
        break;
        case phymodInterfaceLR4:
            *pif = SOC_PORT_IF_LR4;
        break;
        case phymodInterfaceSR10:
            *pif = SOC_PORT_IF_SR10;
        break;
        case phymodInterfaceKR10:
            *pif = SOC_PORT_IF_KR10;
        break;
        case phymodInterfaceLR10:
            *pif = SOC_PORT_IF_LR10;
        break;
        case phymodInterfaceCR10:
            *pif = SOC_PORT_IF_CR10;
        break;
        case phymodInterfaceVSR:
            *pif = SOC_PORT_IF_VSR;
        break;
        case phymodInterfaceCAUI4_C2C:
            *pif = SOC_PORT_IF_CAUI_C2C;
        break;
        case phymodInterfaceCAUI4_C2M:
            *pif = SOC_PORT_IF_CAUI_C2M;
        break;
        case phymodInterfaceSFI:
            *pif = SOC_PORT_IF_SFI;
        break;
        case phymodInterfaceXFI:
            *pif = SOC_PORT_IF_XFI;
        break;
        case phymodInterfaceCAUI:
            *pif = SOC_PORT_IF_CAUI;
        break;
        case phymodInterfaceXLAUI:
            *pif = SOC_PORT_IF_XLAUI;
        break;
        case phymodInterfaceXLPPI:
            *pif = SOC_PORT_IF_XLPPI;
        break;
        case phymodInterfaceXLAUI2:
            *pif = SOC_PORT_IF_XLAUI2;
        break;
        default:
        break;
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy82764_reg_read
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
phy82764_reg_read(int unit, soc_port_t port, uint32 flags,
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
 *      phy82764_reg_write
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
phy82764_reg_write(int unit, soc_port_t port, uint32 flags,
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
 *      phy82764_reg_modify
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
phy82764_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    uint32 data32 = 0, tmp = 0;
    int idx = 0;

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
phy82764_core_init(phy_ctrl_t *pc, soc_phymod_core_t *core,
                   phymod_bus_t *core_bus, uint32 core_addr)
{
    phymod_core_access_t *pm_core;
    phymod_access_t *pm_acc;

    core->unit = pc->unit;
    core->port = pc->port;
    core->read = pc->read;
    core->write = pc->wb_write ? pc->wb_write : pc->write;
    core->wrmask = pc->wrmask;

    pm_core = &core->pm_core;
    phymod_core_access_t_init(pm_core);
    pm_acc = &pm_core->access;
    phymod_access_t_init(pm_acc);
    PHYMOD_ACC_USER_ACC(pm_acc) = core;
    PHYMOD_ACC_BUS(pm_acc) = core_bus;
    PHYMOD_ACC_ADDR(pm_acc) = core_addr;

    if (soc_property_port_get(pc->unit, pc->port, "phy82764", 0) == 45) {
        PHYMOD_ACC_F_CLAUSE45_SET(pm_acc);
    }

    return;
}

STATIC int
_phy82764_device_create_attach(soc_phymod_core_t *core, uint32_t core_id) 
{
    phy82764_device_aux_mode_t *new_device ;
    new_device = sal_alloc(sizeof(phy82764_device_aux_mode_t), "phy82764_device_aux_mode");
    if (new_device == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(new_device, 0 ,sizeof(phy82764_device_aux_mode_t));
    
    new_device->core_id = core_id ;
    core->device_aux_modes = (void *)new_device ;

    return SOC_E_NONE;
}

STATIC int
_phy82764_device_destroy(phy82764_device_aux_mode_t *device)
{
    if (device == NULL) {
        return SOC_E_PARAM;
    }
    sal_free(device);

    return SOC_E_NONE;    
}

STATIC void
phy82764_cleanup(soc_phymod_ctrl_t *pmc)
{
    int idx = 0;
    soc_phymod_phy_t *phy;
    soc_phymod_core_t *core;
    phy82764_device_aux_mode_t *device ;

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
            device = (phy82764_device_aux_mode_t *)core->device_aux_modes ;
        }
        /* Destroy core object if not used anymore */
        if (core && core->ref_cnt) {
            if (--core->ref_cnt == 0) {
                PHYMOD_VDBG(PHY82764_DBG_MEM, NULL,("clean_up device=%p core_p=%p\n", (void *)device, (void *)core)) ;
                _phy82764_device_destroy(device) ;
                soc_phymod_core_destroy(pmc->unit, core);
            }
        }

        /* Destroy phy object */
        if (phy) {
            PHYMOD_VDBG(PHY82764_DBG_MEM, NULL,("clean_up phy=%p\n", (void *)phy)) ;
            soc_phymod_phy_destroy(pmc->unit, phy);
        }
    }
    pmc->num_phys = 0;
}

STATIC int32
_phy_82764_chip_id_get(phymod_phy_access_t *pm_phy, uint32 *chip_id)
{
    uint32 chip_id_msb = 0, chip_id_lsb = 0;
    

    SOC_IF_ERROR_RETURN(
       phymod_phy_reg_read(pm_phy, 0x8a01, &chip_id_msb));

    chip_id_msb = (chip_id_msb & 0xF000) >> 12;
 
    SOC_IF_ERROR_RETURN(
       phymod_phy_reg_read(pm_phy, 0x8a00, &chip_id_lsb));

    if (chip_id_msb == 0x8) {
        if (chip_id_lsb == 0x2764) {
            *chip_id = 0x82764;
        } 
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy82764_speed_to_lane_map_get
 * Purpose:
 *      Mapping to speed to lane map based on config
 * Parameters:
 *      pc                  - phy_ctrl_t
 *      line_lane_map       - [OUT] Line Lane map
 *      sys_lane_map        - [OUT] Sys Lane map
 * Returns:
 *      SOC_E_NONE
 */


STATIC int
_phy82764_speed_to_lane_map_get(phy_ctrl_t *pc, uint32 *line_lane_map, uint32 *sys_lane_map)
{
    int lane = 0;
    int phy_gearbox_enable = 0;
    int phy_pin_compatibility_enable = 0;
    char                *lane_swap = NULL;

    phy_gearbox_enable = soc_property_port_get(pc->unit, pc->port, spn_PHY_GEARBOX_ENABLE, FALSE);
    phy_pin_compatibility_enable = soc_property_port_get(pc->unit, pc->port, spn_PHY_PIN_COMPATIBILITY_ENABLE, FALSE);
    lane_swap = soc_property_port_get_str(pc->unit, pc->port, spn_PHY_XSW_LANE_MAP);

    /* phy_gearbox_enable should be "1" or "0" */
    /* phy_pin_compatibility_enable should be "1" or "0" */
    if (((phy_gearbox_enable != TRUE) && (phy_gearbox_enable != FALSE)) ||
        ((phy_pin_compatibility_enable != TRUE) && (phy_pin_compatibility_enable != FALSE))) {
        LOG_ERROR(BSL_LS_SOC_PHY,(BSL_META_U(0,
                        "Error: Could not set port %d information: Invalid parameter.\n"),
                         pc->port));
        return SOC_E_PARAM;
    }

    *line_lane_map = 0xF;
    *sys_lane_map = 0xF;
    switch(pc->speed_max) {
        case PHY82764_SPD_100G:
        case PHY82764_SPD_106G:
            *line_lane_map = 0x3ff;
            *sys_lane_map = 0xf;
        break;
        case PHY82764_SPD_40G:
        case PHY82764_SPD_42G:
        case PHY82764_SPD_50G:
            /* phy_gearbox_enable should be "1" or "0" */
            /* phy_pin_compatibility_enable should be "1" or "0" */
            /* phy_pin_compatibility_enable = 1 is valid only when pass_thru is set */
            if ((phy_gearbox_enable != TRUE) && (phy_pin_compatibility_enable != TRUE)) {
                *line_lane_map = 0x33;
                *sys_lane_map = 0xf; 
                pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
            } else if ((phy_gearbox_enable != TRUE) && (phy_pin_compatibility_enable == TRUE)) {
                *line_lane_map = 0xf;
                *sys_lane_map = 0xf;
                pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
            } else if ((phy_gearbox_enable == TRUE) && (phy_pin_compatibility_enable != TRUE)) {
                if (pc->lane_num > 1) {
                    *line_lane_map = 0xf0;
                    *sys_lane_map = 0xc;
                } else {
                    *line_lane_map = 0xf;
                    *sys_lane_map = 0x3;
                }  
                pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
            } else {
                LOG_ERROR(BSL_LS_SOC_PHY,(BSL_META_U(0,
                        "Error: Could not set port %d information: Invalid parameter.\n"),
                         pc->port));
                return SOC_E_PARAM;
            }
        break;
        case PHY82764_SPD_20G:
        case PHY82764_SPD_21G:
            /* phy_gearbox_enable should be "1" or "0" */
            if (phy_gearbox_enable != TRUE) {
                if (pc->lane_num > 1) {
                    *line_lane_map = 0x30;
                    *sys_lane_map = 0xc;
                } else {
                    *line_lane_map = 0x3;
                    *sys_lane_map = 0x3;
                }
                pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
            } else if (phy_gearbox_enable == TRUE) {
                if (pc->lane_num > 1) {
                    *line_lane_map = 0x30;
                    *sys_lane_map = 0x4;
                } else {
                    *line_lane_map = 0x3;
                    *sys_lane_map = 0x1;
                }
                pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
            } else {
                LOG_ERROR(BSL_LS_SOC_PHY,(BSL_META_U(0,
                        "Error: Could not set port %d information: Invalid parameter.\n"),
                         pc->port));
                return SOC_E_PARAM;
            }
        break;
        case PHY82764_SPD_1G:
        case PHY82764_SPD_10G:
        case PHY82764_SPD_11G:
        case PHY82764_SPD_25G:
            if (lane_swap != NULL) {
                lane = my_ctoi(lane_swap, lane);
            }
            if (phy_pin_compatibility_enable == TRUE) {
              *line_lane_map = 0x1;
              *sys_lane_map = 0x1;
              *line_lane_map <<= ((lane_swap != NULL) ? lane : pc->lane_num);
              *sys_lane_map <<= pc->lane_num;
            } else {
                if (pc->lane_num > 1) {
                    *line_lane_map = 0x10;
                    *line_lane_map <<= ((lane_swap != NULL) ? (lane - 2): (pc->lane_num - 2));
                    *sys_lane_map = 0x1;
                    *sys_lane_map <<= pc->lane_num;
                } else {
                    *line_lane_map = 0x1;
                    *line_lane_map <<= ((lane_swap != NULL) ? lane : pc->lane_num);
                    *sys_lane_map = 0x1;
                    *sys_lane_map <<= pc->lane_num;
                }
            }
            pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
        break;
        default:
            /* changed default case retrun value from SOC_E_PARAM to SOC_E_NONE as per jira PHY-1805 */
            /* As upper layers does not handle errors retruned incase of Unsupported speed */
            LOG_ERROR(BSL_LS_SOC_PHY,(BSL_META_U(0,
                    "Error: Could not set port %d information: Unsupported speed.\n"),
                     pc->port));
            return SOC_E_NONE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy82764_probe
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
phy82764_probe(int unit, phy_ctrl_t *pc)
{
    int rv, idx;
    uint32 num_phys, core_id, phy_id, found = 0;
    uint32 line_lane_map, sys_lane_map;
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
    int array_max = 8;
    int array_size = 0;
    int port;
    int phy_port;  /* physical port number */
    /* Initialize PHY bus */
    SOC_IF_ERROR_RETURN(phymod_bus_t_init(&core_bus));
    core_bus.bus_name = "phy82764"; 
    core_bus.read = _phy82764_reg_read; 
    core_bus.write = _phy82764_reg_write;

#ifdef PORTMOD_SUPPORT
    /* If portmod feature is enabled let the 
     *      * PM take care of phy control*/

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
    pmc->cleanup = phy82764_cleanup;
    /*pmc->symbols = &bcmi_tscf_xgxs_symbols;*/ 

    
    pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
    pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port));

    /* request memory for the configuration structure */
    pc->size = sizeof(phy82764_config_t);

    /* Bit N corresponds to lane N in lane_map */
    num_phys = 1;
    SOC_IF_ERROR_RETURN
          (_phy82764_speed_to_lane_map_get(pc, &line_lane_map, &sys_lane_map));

    /* we need to get the other core id if more than 1 core per port */
    if (num_phys > 1) {
        /* get the other core address */
        SOC_IF_ERROR_RETURN
            /*
             * COVERITY
             * This is unreachable. It is kept intentionally as a defensive 
             * default for future development.
             */
            /* coverity[dead_error_begin] */
            (soc_phy_addr_multi_get(unit, port, array_max, &array_size, &core_info[0]));
    } else {
        core_info[0].mdio_addr = pc->phy_id;
    }

    phy_type = phymodDispatchTypeSesto;

    /* Probe cores */
    for (idx = 0; idx < num_phys ; idx++) {
        phy82764_core_init(pc, &core_probe, &core_bus,
                           core_info[idx].mdio_addr);
        /* Check that core is indeed an sesto core */
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
        phy_id = (line_lane_map << 16) | core_id;

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
            rv |= _phy82764_device_create_attach(phy->core, core_id) ;
        }
        if (SOC_FAILURE(rv)) {
            break;
        }        
    }
    if (SOC_FAILURE(rv)) {
        phy82764_cleanup(pmc);
        return rv;
    }

    for (idx = 0; idx < pmc->num_phys ; idx++) {
        phy = pmc->phy[idx];
        core = phy->core;
        pm_core = &core->pm_core;

        /* Initialize core object if newly created */
        if (core->ref_cnt == 0) {
            sal_memcpy(&core->pm_bus, &core_bus, sizeof(core->pm_bus));
            phy82764_core_init(pc, core, &core->pm_bus,
                               core_info[idx].mdio_addr);
            /* Set dispatch type */
            pm_core->type = phy_type;
        }
        core->ref_cnt++;        

        /* Initialize phy access based on associated core */
        pm_acc = &phy->pm_phy.access;
        sal_memcpy(pm_acc, &pm_core->access, sizeof(*pm_acc));
        phy->pm_phy.type = phy_type;
        pm_core->access.lane_mask = line_lane_map;
        PHYMOD_ACC_LANE_MASK(pm_acc) = line_lane_map;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy82764_config_init
 * Purpose:     
 *      Determine phy configuration data for purposes of PHYMOD initialization.
 * 
 *      A side effect of this procedure is to save some per-logical port
 *      information in (phy82764_cfg_t *) &pc->driver_data;
 *
 * Parameters:
 *      unit                  - BCM unit number.
 *      port                  - Port number.
 *      logical_lane_offset   - starting logical lane number
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy82764_config_init(int unit, soc_port_t port, int logical_lane_offset,
                 phymod_core_init_config_t *core_init_config, 
                 phymod_phy_init_config_t  *pm_phy_init_config)
{
    phy_ctrl_t *pc;
    phy82764_speed_config_t *speed_config;
    phy82764_config_t *pCfg;
    int port_num_lanes;
    int core_num;
    int phy_num_lanes;
     uint32_t preemphasis, driver_current,i;


    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pCfg = (phy82764_config_t *) pc->driver_data;

    port_num_lanes = SOC_INFO(unit).port_num_lanes[port];

    /* figure out how many lanes are in this phy */
    core_num = (logical_lane_offset / 4);
    phy_num_lanes = (port_num_lanes - logical_lane_offset);
    if (phy_num_lanes > MAX_NUM_LANES) {
       phy_num_lanes = MAX_NUM_LANES;
    }
    
    /*CORE configuration*/
    phymod_core_init_config_t_init(core_init_config);

    core_init_config->flags = PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY;
    core_init_config->firmware_load_method &= 0xff; /* clear checksum bits */
    core_init_config->lane_map.num_of_lanes = NUM_LANES;
    
    speed_config = &(pCfg->speed_config);
    speed_config->port_refclk_int  = 156;
    speed_config->port_num_lanes   = phy_num_lanes;
    speed_config->speed = pc->speed_max;

    if (port_num_lanes == 4) {
        if (pc->speed_max == PHY82764_SPD_40G || pc->speed_max == PHY82764_SPD_42G) {
            speed_config->line_interface =  SOC_PORT_IF_SR4;
        } else if (pc->speed_max == PHY82764_SPD_100G || pc->speed_max == PHY82764_SPD_106G) {
            speed_config->line_interface =  SOC_PORT_IF_SR10;
        } else {
            speed_config->line_interface = SOC_PORT_IF_SR4;
        }
    } else if (port_num_lanes == 1) {
        if (pc->speed_max == PHY82764_SPD_20G || pc->speed_max == PHY82764_SPD_21G) {
            speed_config->line_interface =  SOC_PORT_IF_SR2;
        } else if (pc->speed_max == PHY82764_SPD_10G || pc->speed_max == PHY82764_SPD_11G) {
            speed_config->line_interface =  SOC_PORT_IF_SR;
        } else {
            speed_config->line_interface =  SOC_PORT_IF_SR;
        }
    } else if (port_num_lanes == 2) {
        if (pc->speed_max == PHY82764_SPD_40G || pc->speed_max == PHY82764_SPD_42G) {
            speed_config->line_interface =  SOC_PORT_IF_SR4;
        } else if (pc->speed_max == PHY82764_SPD_20G || pc->speed_max == PHY82764_SPD_21G) {
            speed_config->line_interface =  SOC_PORT_IF_SR2;
        } else {
            speed_config->line_interface = SOC_PORT_IF_SR4;
        }
    } else {
        LOG_CLI((BSL_META_U(unit,"Invalid number of lanes:%d\n "), port_num_lanes));
        return SOC_E_CONFIG;
    }

    /* PHY configuration */
    phymod_phy_init_config_t_init(pm_phy_init_config);

    pm_phy_init_config->polarity.rx_polarity
                                 = soc_property_port_get(unit, port,
                                   spn_PHY_RX_POLARITY_FLIP, 0);
    pm_phy_init_config->polarity.tx_polarity
                                 = soc_property_port_get(unit, port,
                                   spn_PHY_TX_POLARITY_FLIP, 0);
    preemphasis = 0x0;
    preemphasis = soc_property_port_get(unit, port,
            spn_PHY_PREEMPHASIS,
            0x007F7F7F);

    driver_current = 0x0;
    driver_current = soc_property_port_get(unit, port,
            spn_PHY_DRIVER_CURRENT,
            0xF);
    
    for (i = 0; i < MAX_NUM_LANES; i++) {
        pm_phy_init_config->tx[i].pre = preemphasis & 0xff;
        pm_phy_init_config->tx[i].main = (preemphasis & 0xff00) >> 8;
        pm_phy_init_config->tx[i].post = (preemphasis & 0xff0000) >> 16;
        pm_phy_init_config->tx[i].amp = driver_current;
    }


    /* phy_ctrl_t configuration (LOGICAL PORT BASED)
     * Only do this once, for the first core of the logical port*/
    if (core_num == 0) {
        /* phy_mode, PHYCTRL_MDIO_ADDR_SHARE, PHY_FLAGS_INDEPENDENT_LANE */
        if (port_num_lanes == 4) {
            pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        } else if (port_num_lanes == 2) {
            pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        } else if (port_num_lanes == 1) {
            pc->phy_mode = PHYCTRL_ONE_LANE_PORT;
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        }

    }

    return SOC_E_NONE;
}

STATIC int
_phy_82764_init_pass1(int unit, soc_port_t port)
{
    phy_ctrl_t *pc; 
    soc_phymod_ctrl_t *pmc;
    soc_phymod_phy_t *phy = NULL;
    soc_phymod_core_t *core;
    phy82764_config_t *pCfg;
    phy82764_speed_config_t *speed_config;
    phy82764_device_cfg_aux_mode_t *device_cfg_aux_modes;
    phymod_phy_inf_config_t interface_config;
    phymod_core_status_t core_status;
    phymod_core_info_t core_info;
    int idx, logical_lane_offset;
    soc_port_ability_t ability;
    soc_phy_info_t *pi;
    char *dev_name;
    int len = 0;
    uint32 chip_id = 0, is_identified = 0;
    int phy_force_dload = 0, fw_ld_method = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pc->driver_data = (void*)(pc+1);
    pmc = &pc->phymod_ctrl;
    pCfg = (phy82764_config_t *) pc->driver_data;
    
    sal_memset(pCfg, 0, sizeof(*pCfg));
    speed_config = &(pCfg->speed_config);
    device_cfg_aux_modes = &(pCfg->device_cfg_aux_modes);

    sal_memset(&ability, 0, sizeof(ability));

    dev_name = DEV_NAME(pc);

    phy = pmc->phy[0];

    SOC_IF_ERROR_RETURN(
         _phy_82764_chip_id_get(&phy->pm_phy, &chip_id));
    DEVID(pc) = chip_id;

    pi = &SOC_PHY_INFO(unit, port);
    if (DEVID(pc) == SESTO_ID_82764) { 
        /* coverity[secure_coding] */
        sal_strncpy_s (dev_name, "BCM82764", PHY82764_CHIP_ID_SIZE);
    } else {
        /* coverity[secure_coding] */
        sal_strncpy_s (dev_name, "Sesto", PHY82764_CHIP_ID_SIZE);
    }
    
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

        pCfg->device_aux_modes = (phy82764_device_aux_mode_t *)(core->device_aux_modes) ;
        pCfg->device_aux_modes->repeater = soc_property_port_get(unit, port, spn_PHY_PCS_REPEATER, TRUE);

        /* determine configuration data structure to default values, based on SOC properties */
        SOC_IF_ERROR_RETURN
            (phy82764_config_init(unit, port,
                              logical_lane_offset,
                              &core->init_config, &phy->init_config));
        speed_config->higig_port = PBMP_MEMBER(PBMP_HG_ALL(unit), port);

        /* set the port to its max or init_speed */
        SOC_IF_ERROR_RETURN
             (phy82764_speed_to_interface_config_get(speed_config, &interface_config));
        sal_memcpy(&core->init_config.interface, &interface_config, sizeof(interface_config));
        sal_memcpy(&phy->init_config.interface, &interface_config, sizeof(interface_config));

        core->pm_core.device_op_mode = 0;
        /* soc read for phy device operation mode */
        device_cfg_aux_modes->gearbox_enable = (soc_property_port_get(unit, port, spn_PHY_GEARBOX_ENABLE, FALSE));
        device_cfg_aux_modes->pin_compatibility_enable = soc_property_port_get(unit, port,
                                   spn_PHY_PIN_COMPATIBILITY_ENABLE, FALSE);
        device_cfg_aux_modes->phy_mode_reverse = soc_property_port_get(unit, port, spn_PORT_PHY_MODE_REVERSE, 0);

        /* copy the soc config to phymod_core_access */
        /* bit 0 is for gearbox_enable */
        /* bit 1 is for pin_compatibility_enable */
        /* bit 2 is for phy_mode_reverse */
        core->pm_core.device_op_mode |= device_cfg_aux_modes->gearbox_enable;
        core->pm_core.device_op_mode |= (device_cfg_aux_modes->pin_compatibility_enable << 1);
        core->pm_core.device_op_mode |= (device_cfg_aux_modes->phy_mode_reverse << 2);
        sal_memcpy(&phy->pm_phy.device_op_mode, &core->pm_core.device_op_mode, sizeof(core->pm_core.device_op_mode));


        /*lower nibble to represent Force FW load and upper nibble to represent load method */
        phy_force_dload = 0x11;
        phy_force_dload = soc_property_port_get(unit, port,
                                         spn_PHY_FORCE_FIRMWARE_LOAD, phy_force_dload);

        fw_ld_method = ((phy_force_dload & 0xF0) >> 4);
        phy_force_dload &= 0xf;
        switch (fw_ld_method) {
            case 0:
                core->init_config.firmware_load_method = phymodFirmwareLoadMethodNone;
            break;
            case 1:
                core->init_config.firmware_load_method = phymodFirmwareLoadMethodInternal;
            break;
            case 2:
                core->init_config.firmware_load_method = phymodFirmwareLoadMethodProgEEPROM;
            break;
            default:
                core->init_config.firmware_load_method = phymodFirmwareLoadMethodInternal;
            break;
        }
        if ((!core->init) && !SOC_WARM_BOOT(unit)) {
           core_status.pmd_active = 0;
           SOC_IF_ERROR_RETURN(phymod_core_identify(&core->pm_core, 0, &is_identified));
           if ((is_identified & 0x80000000) &&
               (core->init_config.firmware_load_method == phymodFirmwareLoadMethodInternal)) {
                pc->flags |= PHYCTRL_MDIO_BCST;
           } else if (core->init_config.firmware_load_method == phymodFirmwareLoadMethodNone) {
               phymod_core_firmware_info_t fw_info;
               SOC_IF_ERROR_RETURN (
                   phymod_core_firmware_info_get(&core->pm_core, &fw_info));
                if (fw_info.fw_version == 0x1) {
                   LOG_CLI((BSL_META_U(unit, "No FW found on the chip, Please use "
                                 "spn_PHY_FORCE_FIRMWARE_LOAD as 0x11 to download FW to chip\n")));
                   return SOC_E_UNAVAIL;
                }
            } else {
               if (core->init_config.firmware_load_method == phymodFirmwareLoadMethodInternal) {
                   if (phy_force_dload == phymodFirmwareLoadForce) {
                       PHYMOD_CORE_INIT_F_FW_FORCE_DOWNLOAD_SET(&(core->init_config));
                   } else if(phy_force_dload == phymodFirmwareLoadAuto) {
                       PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_SET(&(core->init_config));
                   }
               }
               PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&(core->init_config));
               SOC_IF_ERROR_RETURN
                   (phymod_core_init(&core->pm_core, &core->init_config, &core_status));
               PHYMOD_CORE_INIT_F_EXECUTE_PASS1_CLR(&(core->init_config));
           }
           core->init = TRUE;
        }

        /*read serdes id info */
        SOC_IF_ERROR_RETURN
            (phymod_core_info_get(&core->pm_core, &core_info)); 

        /* for multicore phys, need to ratchet up to the next batch of lanes */
        logical_lane_offset += core->init_config.lane_map.num_of_lanes;
    }

    /* indicate second pass of init is needed */
    if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) {
        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);
    }

    return SOC_E_NONE;
}
void phy82764_phyctrl_to_phymod_interface (uint16_t sys_if, phymod_interface_t *sys_intf)
{
    switch (sys_if)
    {
        case SOC_PORT_IF_SFI:
            *sys_intf = phymodInterfaceSFI;
        break;
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
        case SOC_PORT_IF_XLPPI:
            *sys_intf = phymodInterfaceXLPPI;
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
        case SOC_PORT_IF_ER4:
            *sys_intf = phymodInterfaceER4;
        break;
        case SOC_PORT_IF_SR4:
            *sys_intf = phymodInterfaceSR4;
        break;
        case SOC_PORT_IF_KX:
            *sys_intf = phymodInterfaceKX;
        break;
        case SOC_PORT_IF_KR2:
            *sys_intf = phymodInterfaceKR2;
        break;
        case SOC_PORT_IF_CR2:
            *sys_intf = phymodInterfaceCR2;
        break;
        case SOC_PORT_IF_XLAUI2:
            *sys_intf = phymodInterfaceXLAUI2;
        break;
        case SOC_PORT_IF_CAUI_C2C:
            *sys_intf = phymodInterfaceCAUI4_C2C;
        break;

        default:
            *sys_intf = phymodInterfaceKR;
    }

}

STATIC int
_phy_82764_init_pass2(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    soc_phymod_phy_t *phy = NULL;
    phy82764_config_t *pCfg;
    phy82764_speed_config_t *speed_config;
    phymod_phy_inf_config_t interface_config;
    phy82764_device_cfg_aux_mode_t *device_cfg_aux_modes;
    soc_port_ability_t ability;
    uint16_t sys_if = 0;
    phymod_interface_t sys_intf = 0;
    uint32 line_lane_map, sys_lane_map;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memset(&ability, 0, sizeof(ability));
    pc->driver_data = (void*)(pc+1);
    pmc = &pc->phymod_ctrl;
    pCfg = (phy82764_config_t *) pc->driver_data;
    
    speed_config = &(pCfg->speed_config);
    device_cfg_aux_modes = &(pCfg->device_cfg_aux_modes);
    phy = pmc->phy[0];

    /*Work around for to support flex of 10G To 40G Mux as part of jira PHY-1805 */
    if (((pc->speed_max == PHY82764_SPD_20G) && (device_cfg_aux_modes->gearbox_enable == TRUE)) ||
        (pc->speed_max == PHY82764_SPD_25G) || (pc->speed_max == PHY82764_SPD_50G)){
        return SOC_E_NONE;
    }

    if (!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(
            phy_82764_driver_current_set(pmc, port, PHY82764_LINE_SIDE, 0xf));
        SOC_IF_ERROR_RETURN(
            phy_82764_tx_fir_pre_set(pmc, port, PHY82764_LINE_SIDE, 0));
        SOC_IF_ERROR_RETURN(
            phy_82764_tx_fir_post_set(pmc, port, PHY82764_LINE_SIDE, 0));
        SOC_IF_ERROR_RETURN(
            phy_82764_tx_fir_post2_set(pmc, port, PHY82764_LINE_SIDE, 0));
        SOC_IF_ERROR_RETURN(
            phy_82764_tx_fir_main_set(pmc, port, PHY82764_LINE_SIDE, 0x3c));



        phy->init_config.op_mode = pCfg->device_aux_modes->repeater;
        SOC_IF_ERROR_RETURN
            (phymod_phy_init(&phy->pm_phy, &phy->init_config));


        SOC_IF_ERROR_RETURN
             (phy82764_speed_to_interface_config_get(speed_config, &interface_config));

        SOC_IF_ERROR_RETURN
               (_phy82764_speed_to_lane_map_get(pc, &line_lane_map, &sys_lane_map));
        pCfg->sys_lane_map = sys_lane_map;
        phy->pm_phy.access.lane_mask = line_lane_map;

        SOC_IF_ERROR_RETURN
            (phymod_phy_interface_config_set(&phy->pm_phy,
                                 0 /* flags */, &interface_config));
        /* Config Sys interface*/
        sys_if = soc_property_port_get(unit, port, spn_PHY_SYS_INTERFACE, 0);
        if (sys_if) {
            phy82764_phyctrl_to_phymod_interface(sys_if, &sys_intf);
            if (sys_intf == 0) {
                return SOC_E_PARAM;
            }
            phy->pm_phy.port_loc = phymodPortLocSys;
            phy->pm_phy.access.lane_mask = sys_lane_map;
            interface_config.interface_type = sys_intf;

            SOC_IF_ERROR_RETURN
               (phymod_phy_interface_config_set(&phy->pm_phy,
                                 0 /* flags */, &interface_config));

            phy->pm_phy.port_loc = phymodPortLocLine;
            phy->pm_phy.access.lane_mask = line_lane_map;
        }

        /* setup the port's an cap */
        SOC_IF_ERROR_RETURN
            (phy_82764_ability_local_get(unit, port, &ability));
    } /* !SOC_WARM_BOOT  */

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy82764_init_pass2: u=%d p=%d\n"), unit, port));

    if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) {
        /* indicate third  pass of init is needed */
        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS3);
    }

    return SOC_E_NONE;
}

STATIC int
_phy_82764_init_pass3(int unit, soc_port_t port)
{
    phy_ctrl_t *pc; 
    phy82764_config_t *pCfg;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pc->driver_data = (void*)(pc+1);
    pCfg = (phy82764_config_t *) pc->driver_data;
    
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "PHY82764 init pass3: u=%d p=%d\n"), unit, port));

    /* If configured, enable module auto detection */
    pCfg->auto_mod_detect = soc_property_port_get(unit, port, spn_PHY_MOD_AUTO_DETECT, 0);
    if (pCfg->auto_mod_detect) {

    }

	return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82764_init
 *  
 *      An SDK "phy" is a logical port, which can consist of from 1-10 lanes,
 *          (A phy with more than 4 lanes requires more than one core).
 *      Per-logical port information is saved in &pc->driver_data.
 *      An SDK phy is implemented as one or more PHYMOD "phy"s.
 *  
 *      A PHYMOD "phy" resides completely within a single core, which can be
 *      from 1 to 8 lanes.
 *      Per-phymod phy information is kept in (soc_phymod_ctrl_t) *pc->phymod_ctrl
 *      A phymod phy points to its core.  Up to 4 phymod phys can be on a single core
 *  
 * Purpose:     
 *      Initialize a phy82764
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_82764_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
     
    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {
        PHY_FLAGS_SET(unit, port,  PHY_FLAGS_FIBER | PHY_FLAGS_C45 | PHY_FLAGS_REPEATER);
        SOC_IF_ERROR_RETURN(_phy_82764_init_pass1(unit, port));

        if (PHYCTRL_INIT_STATE(pc) != PHYCTRL_INIT_STATE_DEFAULT) {
            return SOC_E_NONE;
        }
    }
    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        SOC_IF_ERROR_RETURN( _phy_82764_init_pass2(unit, port));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "82764 init pass2 completed: u=%d p=%d\n"), unit, port));
        if (PHYCTRL_INIT_STATE(pc) != PHYCTRL_INIT_STATE_DEFAULT) {
            return SOC_E_NONE;
        }
    }

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS3) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        SOC_IF_ERROR_RETURN(_phy_82764_init_pass3(unit, port));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY82764 init pass3 completed: u=%d p=%d\n"), unit, port));
        PHYCTRL_INIT_STATE_SET(pc, PHYCTRL_INIT_STATE_DEFAULT);
        return SOC_E_NONE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82764_ability_local_get
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
phy_82764_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc; 
    uint32_t   pa_speed = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    sal_memset(ability, 0, sizeof(*ability));
    if ((pc->speed_max == PHY82764_SPD_106G) ||
        (pc->speed_max == PHY82764_SPD_100G)) {
        pa_speed = SOC_PA_SPEED_106GB | SOC_PA_SPEED_100GB |
                   SOC_PA_SPEED_42GB | SOC_PA_SPEED_40GB |
                   SOC_PA_SPEED_21GB | SOC_PA_SPEED_20GB |
                   SOC_PA_SPEED_11GB  | SOC_PA_SPEED_10GB;
    } else if ((pc->speed_max == PHY82764_SPD_42G) ||
                (pc->speed_max == PHY82764_SPD_40G)) {
        pa_speed = SOC_PA_SPEED_42GB | SOC_PA_SPEED_40GB |
                   SOC_PA_SPEED_21GB | SOC_PA_SPEED_20GB |
                   SOC_PA_SPEED_11GB  | SOC_PA_SPEED_10GB;
    } else if ((pc->speed_max == PHY82764_SPD_20G) ||
               (pc->speed_max == PHY82764_SPD_20G)) {
        pa_speed = SOC_PA_SPEED_21GB | SOC_PA_SPEED_20GB |
                   SOC_PA_SPEED_11GB  | SOC_PA_SPEED_10GB;
    } else if ((pc->speed_max == PHY82764_SPD_11G) ||
               (pc->speed_max == PHY82764_SPD_10G)) {
        pa_speed = SOC_PA_SPEED_10GB;
    } else {
        pa_speed = SOC_PA_SPEED_106GB | SOC_PA_SPEED_100GB |
                   SOC_PA_SPEED_42GB | SOC_PA_SPEED_40GB |
                   SOC_PA_SPEED_21GB | SOC_PA_SPEED_20GB |
                   SOC_PA_SPEED_11GB  | SOC_PA_SPEED_10GB;
    }

    ability->loopback  = SOC_PA_LB_PHY;
    ability->medium    = SOC_PA_MEDIUM_COPPER;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_XGMII;
    ability->flags     = SOC_PA_AUTONEG;
    ability->speed_full_duplex = pa_speed;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy82764_ability_local_get:unit=%d p=%d sp=%08x\n"),
              unit, port, ability->speed_full_duplex));

    return (SOC_E_NONE);
}

STATIC int
phy_82764_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    phymod_phy_access_t       *pm_phy;
    phy_ctrl_t *int_pc;
    int32 int_phy_link = 0;
    int32 speed = 0;
    soc_port_if_t interface = 0;
    *link = 0;
    int_pc = INT_PHY_SW_STATE(unit, port);
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[pmc->main_phy]->pm_phy;
    if (eye_scan_enabled) {
        /* Return Success if eyescan is in progress*/
        if (int_pc != NULL) {
		    SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, &int_phy_link));
        }
        *link = int_phy_link;
#ifdef PHY82764_DEBUG    
        if (*link == 0) {
            LOG_CLI((BSL_META_U(unit,"PCS Down: port:%d\n "), port));
        }
#endif        
        return SOC_E_NONE;
    }

    if (int_pc != NULL) {
		SOC_IF_ERROR_RETURN(PHY_LINK_GET(int_pc->pd, unit, port, &int_phy_link));
        SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, &speed));
        SOC_IF_ERROR_RETURN(PHY_INTERFACE_GET(int_pc->pd, unit, port, &interface));
    } 
    
    SOC_IF_ERROR_RETURN
        (phymod_phy_link_status_get(pm_phy, (uint32_t *) link));

    /* When PMD lock of 82764 is not Set, Leave the port link as
     * PMD status of 82764. When PMD lock of 82764 is set use
     * PCS status of internal serdes as port link status.
     * When internal serdes is not there use 82764 PMS as link status*/
#ifdef PHY82764_DEBUG    
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
phy_82764_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    int32 intf;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    if (eye_scan_enabled) {
        /* Return Success if eyescan is in progress*/
        return SOC_E_NONE;
    }
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82764_SYS_SIDE : PHY82764_LINE_SIDE; 

    pmc = &pc->phymod_ctrl;

    SOC_IF_ERROR_RETURN
        (phy_82764_power_set(pmc, port, intf, enable));

    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    return SOC_E_NONE;
}

STATIC int
phy_82764_enable_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    int32 intf;
    uint32_t ena_dis;
    *enable = 0;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;
    if (eye_scan_enabled) {
        /* Return Success if eyescan is in progress*/
        *enable =1;
        return SOC_E_NONE;
    }
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82764_SYS_SIDE : PHY82764_LINE_SIDE; 
    SOC_IF_ERROR_RETURN
        (phy_82764_power_get(pmc, port, intf, &ena_dis));
    *enable = ena_dis;

    return SOC_E_NONE;
}

STATIC int
phy_82764_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;

    pc = EXT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;
    SOC_IF_ERROR_RETURN (
        phy_82764_loopback_internal_pmd_set(pmc, port, PHY82764_LINE_SIDE, enable));

    return SOC_E_NONE;
}

STATIC int
phy_82764_firmware_set(int unit, int port, int cmd, uint8 *array,int datalen)
{
    phy_ctrl_t                *pc;
    soc_phymod_core_t *pmc_core;
    phymod_core_access_t *pm_core;
    soc_phymod_ctrl_t *pmc;
    phymod_core_status_t core_status;
    int phy_force_dload = 0;

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

    phy_force_dload = 0x11;
    phy_force_dload = soc_property_port_get(unit, port,
                                    spn_PHY_FORCE_FIRMWARE_LOAD, phy_force_dload);
    phy_force_dload &= 0xf;

    if(phy_force_dload == phymodFirmwareLoadForce) {
        PHYMOD_CORE_INIT_F_FW_FORCE_DOWNLOAD_SET(&(pmc_core->init_config));
    } else if(phy_force_dload == phymodFirmwareLoadAuto) {
        PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_SET(&(pmc_core->init_config));
    }

    if (array == NULL) {
        if (cmd == PHYCTRL_UCODE_BCST_SETUP) {
            PHYMOD_CORE_INIT_F_RESET_CORE_FOR_FW_LOAD_SET(&(pmc_core->init_config));
            SOC_IF_ERROR_RETURN (
              phymod_core_init(pm_core, &pmc_core->init_config, &core_status));
            PHYMOD_CORE_INIT_F_RESET_CORE_FOR_FW_LOAD_CLR(&(pmc_core->init_config));
            return SOC_E_NONE;
        } else if (cmd == PHYCTRL_UCODE_BCST_uC_SETUP) {
            return SOC_E_NONE;
        } else if (cmd == PHYCTRL_UCODE_BCST_ENABLE) {
            PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_SET(&(pmc_core->init_config));
            SOC_IF_ERROR_RETURN (
              phymod_core_init(pm_core, &pmc_core->init_config, &core_status));
            PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_CLR(&(pmc_core->init_config));
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
 * phy_82764_per_lane_loopback_internal_pmd_get (this is the PMD global loopback)
 */
STATIC int 
phy_82764_per_lane_loopback_internal_pmd_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(&pm_phy_copy, phymodLoopbackGlobalPMD, &enable));
    *value = enable;

    return(SOC_E_NONE);
}
/* 
 * phy_82764_loopback_internal_pmd_get (this is the PMD global loopback)
 */
STATIC int 
phy_82764_loopback_internal_pmd_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value)
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
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
        SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
    } 
    SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(&pm_phy_copy, phymodLoopbackGlobalPMD, &enable));
    *value = enable;

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_loopback_remote_get
 */
STATIC int 
phy_82764_per_lane_loopback_remote_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t    *pm_phy;
    uint32_t               enable;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_loopback_get(&pm_phy_copy, phymodLoopbackRemotePMD, &enable));
    *value = enable;

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_prbs_tx_poly_get
 */
STATIC int
phy_82764_per_lane_prbs_tx_poly_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;


    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
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
 * phy_82764_per_lane_prbs_rx_poly_get
 */
STATIC int
phy_82764_per_lane_prbs_rx_poly_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;


    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
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
 * phy_82764_per_lane_prbs_tx_invert_data_get
 */
STATIC int
phy_82764_per_lane_prbs_tx_invert_data_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;

    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));
    *value = prbs.invert;

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_prbs_rx_invert_data_get
 */
STATIC int
phy_82764_per_lane_prbs_rx_invert_data_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));
    *value = prbs.invert;

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_prbs_tx_enable_get
 */
STATIC int
phy_82764_per_lane_prbs_tx_enable_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_get(&pm_phy_copy, flags, value));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_prbs_rx_enable_get
 */
STATIC int
phy_82764_per_lane_prbs_rx_enable_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_get(&pm_phy_copy, flags, value));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_prbs_rx_status_get
 */
STATIC int
phy_82764_per_lane_prbs_rx_status_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
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
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
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
 * phy_82764_per_lane_rx_peak_filter_get
 */
STATIC int
phy_82764_per_lane_rx_peak_filter_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
        *value = phymod_rx.peaking_filter.value;

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_rx_vga_get
 */
STATIC int
phy_82764_per_lane_rx_vga_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
        *value = phymod_rx.vga.value;

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_rx_dfe_tap_control_get
 */
STATIC int
phy_82764_per_lane_rx_dfe_tap_control_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, int tap, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t          phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
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
 * phy_82764_per_lane_rx_low_freq_filter_get
 */
STATIC int
phy_82764_per_lane_rx_low_freq_filter_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t          phymod_rx;

    *value = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_rx_get(&pm_phy_copy, &phymod_rx));
    *value = phymod_rx.low_freq_peaking_filter.value;

    return(SOC_E_NONE);
}

/* 
 * phy_82764_pi_control_get
 */
STATIC int 
phy_82764_pi_control_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value)
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
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
        SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_override_get(&pm_phy_copy, &tx_override));
    *value = tx_override.phase_interpolator.value;

    return(SOC_E_NONE);
}
/*
 * phy_82764_per_lane_rx_seq_done_get
 */
STATIC int
phy_82764_per_lane_rx_seq_done_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_rx_pmd_locked_get(&pm_phy_copy, value));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_fec_get
 */
STATIC int 
phy_82764_fec_enable_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_access_t pm_phy_copy;

    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
        SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
    }

    SOC_IF_ERROR_RETURN(phymod_phy_fec_enable_get(&pm_phy_copy, value));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_driver_current_get
 */
STATIC int
phy_82764_per_lane_driver_current_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.amp;

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_preemphasis_get
 */
STATIC int
phy_82764_per_lane_preemphasis_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    *value = phymod_tx.pre;

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_rx_low_freq_filter_set
 */
STATIC int
phy_82764_per_lane_rx_low_freq_filter_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t          phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    sal_memset(&phymod_rx, 0, sizeof(phymod_rx_t));
    phymod_rx.low_freq_peaking_filter.enable = TRUE;
    phymod_rx.low_freq_peaking_filter.value = value;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(&pm_phy_copy, &phymod_rx));

    return(SOC_E_NONE);
}
/* 
 * phy_82764_per_lane_power_get
 */
STATIC int
phy_82764_per_lane_power_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_power_t  power;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
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
 * phy_82764_power_get
 */
STATIC int
phy_82764_power_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value)
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
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
        SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
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
 * phy_82764_per_lane_tx_polarity_get
 */
STATIC int 
phy_82764_per_lane_tx_polarity_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    phymod_polarity_t_init(&polarity);
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(&pm_phy_copy, &polarity));

    *value = polarity.tx_polarity;

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_rx_polarity_get
 */
STATIC int 
phy_82764_per_lane_rx_polarity_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    phymod_polarity_t_init(&polarity);
    SOC_IF_ERROR_RETURN(phymod_phy_polarity_get(&pm_phy_copy, &polarity));

    *value = polarity.rx_polarity;

    return(SOC_E_NONE);
}

/*
 * phy_82764_gpio_config_get
 */
STATIC int
phy_82764_gpio_config_get(soc_phymod_ctrl_t *pmc, soc_port_t port, uint32 *value)
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

    for (gpio_pin_no = 0; gpio_pin_no < 4; gpio_pin_no++) {
        SOC_IF_ERROR_RETURN(phymod_phy_gpio_config_get(&pm_phy_copy, gpio_pin_no, &gpio_mode));
        *value |= (gpio_mode << (4 * gpio_pin_no));
    }

    return(SOC_E_NONE);
}

/*
 * phy_82764_gpio_value_get
 */
STATIC int
phy_82764_gpio_value_get(soc_phymod_ctrl_t *pmc, soc_port_t port, uint32 *value)
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

    for (gpio_pin_no = 0; gpio_pin_no < 4; gpio_pin_no++) {
        SOC_IF_ERROR_RETURN(phymod_phy_gpio_pin_value_get(&pm_phy_copy, gpio_pin_no, &pin_value));
        *value |= (pin_value << (4 * gpio_pin_no));
    }

    return(SOC_E_NONE);
}
/*
 * Function:
 *     phy_82764_per_lane_eye_margin_est_get
 * Parameters:
 *     pmc             -   Phymod control
 *     port            -   Port no
 *     intf            -   Interface side Line/System side
 *     lane            -   Lane number
 *     eye_margin_mode -   eyescan mode type (left/right/upper/lower)
 *     value           -   Value (Out)
 */ 

int phy_82764_per_lane_eye_margin_est_get(soc_phymod_ctrl_t *pmc,
                        soc_port_t port,
                        int32 intf,
                        int lane, 
                        phymod_eye_margin_mode_t eye_margin_mode,
                        uint32 *value) 
{
      
    phymod_phy_access_t  *pm_phy;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    }
    SOC_IF_ERROR_RETURN(phymod_phy_eye_margin_est_get(&pm_phy_copy, eye_margin_mode, value));
    return (SOC_E_NONE); 
}

/*
 * Function:
 *      phy82764_control_get
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
phy_82764_control_get(int unit, soc_port_t port, soc_phy_control_t type, uint32 *value)
{
    int                 rv = 0;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    int32               intf;
    uint32              val;
    int                 lane = 0, num_lanes = 0;

    PHY_CONTROL_TYPE_CHECK(type);

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    *value = 0;
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82764_SYS_SIDE : PHY82764_LINE_SIDE; 
    /* get number of lanes per port on line side */
    num_lanes = (intf == PHY82764_LINE_SIDE) ? port_num_lanes_line_side_get(unit, port, pc->speed_max):
                                               SOC_INFO(unit).port_num_lanes[port];
  if (!((type == SOC_PHY_CONTROL_INTR_MASK) || (type == SOC_PHY_CONTROL_GPIO_CONFIG) ||
        (type == SOC_PHY_CONTROL_GPIO_VALUE) || (type == SOC_PHY_CONTROL_FIRMWARE_MODE) ||
        (type == SOC_PHY_CONTROL_INTR_STATUS) || (type == SOC_PHY_CONTROL_PHASE_INTERP) ||
        (type == SOC_PHY_CONTROL_POWER) || (type == SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION) ||
        (type == SOC_PHY_CONTROL_FIRMWARE_DFE_ENABLE) || (type == SOC_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE) ||
        (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE0) || (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE1) ||
        (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE2) || (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE3) ||
        (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0) || (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1) ||
        (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2) || (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3) ||
        (type == SOC_PHY_CONTROL_CL72) || (type == SOC_PHY_CONTROL_CL72_STATUS) ||
        (type == SOC_PHY_CONTROL_UNRELIABLE_LOS))) {
      for (lane = 0; lane < num_lanes; lane++) {
         rv |= phy_82764_per_lane_control_get(unit, port, lane, type, &val);
         *value |= val;
         if (rv != 0) {
             return SOC_E_UNAVAIL;
         }
      }
  } else {
    switch(type) {
    case SOC_PHY_CONTROL_CL72:
        rv = phy_82764_cl72_enable_get(pmc, port, intf, value);
        break;
    case SOC_PHY_CONTROL_CL72_STATUS:
        rv = phy_82764_cl72_status_get(pmc, port, intf, value);
        break;
    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        rv = phy_82764_per_lane_preemphasis_get(pmc, port, intf, 0, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        rv = phy_82764_per_lane_preemphasis_get(pmc, port, intf, 1, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        rv = phy_82764_per_lane_preemphasis_get(pmc, port, intf, 2, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        rv = phy_82764_per_lane_preemphasis_get(pmc, port, intf, 3, value);
        break;
    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        rv = phy_82764_per_lane_driver_current_get(pmc, port, intf, 0, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        rv = phy_82764_per_lane_driver_current_get(pmc, port, intf, 1, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        rv = phy_82764_per_lane_driver_current_get(pmc, port, intf, 2, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        rv = phy_82764_per_lane_driver_current_get(pmc, port, intf, 3, value);
        break;
    /* PHASE INTERPOLATOR */
    case SOC_PHY_CONTROL_PHASE_INTERP:
        rv = phy_82764_pi_control_get(pmc, port, intf, value);
        break;
    /* FEC */
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
      rv = phy_82764_fec_enable_get(pmc, port, intf, value);
      break;
    /* POWER */
    case SOC_PHY_CONTROL_POWER:
       rv = phy_82764_power_get(pmc, port, intf, value);
       break;
    case SOC_PHY_CONTROL_INTR_MASK:
        rv = phy_82764_intr_enable_get(pmc, port, intf, value);
        break;
    case SOC_PHY_CONTROL_INTR_STATUS:
        rv = phy_82764_intr_status_get(pmc, port, intf, value);
        break;
    case SOC_PHY_CONTROL_GPIO_CONFIG:
        rv = phy_82764_gpio_config_get(pmc, port, value);
        break;
    case SOC_PHY_CONTROL_GPIO_VALUE:
        rv = phy_82764_gpio_value_get(pmc, port, value);
        break;
    /* FIRMWARE MODE */
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
       rv = phy_82764_firmware_mode_get(pc, port, intf, value);
       break;
    case SOC_PHY_CONTROL_FIRMWARE_DFE_ENABLE:
       rv = phy_82764_firmware_dfe_enable_get(pc, port, intf, value);
       break;
    case SOC_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE:
       rv = phy_82764_firmware_lp_dfe_enable_get(pc, port, intf, value);
       break;
    case SOC_PHY_CONTROL_UNRELIABLE_LOS:
        rv = phy_82764_unreliable_los_get(pc, port, intf, value);
        break;
       /*
        * COVERITY
        * This is unreachable. It is kept intentionally as a defensive
        * default for future development.
        */
       /* coverity[dead_error_begin] */
    default:
       rv = SOC_E_UNAVAIL;
       break;
    }
  }

    return rv;
}

/*
 * Function:
 *      phy_82764_per_lane_control_get
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
phy_82764_per_lane_control_get(int unit, soc_port_t port, int lane,
                     soc_phy_control_t type, uint32 *value)
{
    int                 rv = SOC_E_UNAVAIL;
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

    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82764_SYS_SIDE : PHY82764_LINE_SIDE; 

    switch(type) {
    /* RM PMD LOCK */
    case SOC_PHY_CONTROL_RX_SEQ_DONE:
        rv = phy_82764_per_lane_rx_seq_done_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = phy_82764_per_lane_preemphasis_get(pmc, port, intf, lane, value);
        break; 
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = phy_82764_per_lane_driver_current_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        rv = phy_82764_per_lane_prbs_tx_poly_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        rv = phy_82764_per_lane_prbs_tx_invert_data_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        rv = phy_82764_per_lane_prbs_tx_enable_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        rv = phy_82764_per_lane_prbs_rx_poly_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        rv = phy_82764_per_lane_prbs_rx_invert_data_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        rv = phy_82764_per_lane_prbs_rx_enable_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = phy_82764_per_lane_prbs_tx_poly_get(pmc, port, intf, lane, value);
        rv = phy_82764_per_lane_prbs_rx_poly_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = phy_82764_per_lane_prbs_tx_invert_data_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        rv = phy_82764_per_lane_prbs_rx_invert_data_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = phy_82764_per_lane_prbs_tx_enable_get(pmc, port, intf, lane, value);
        rv = phy_82764_per_lane_prbs_rx_enable_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = phy_82764_per_lane_prbs_rx_enable_get(pmc, port, intf, lane, value);
        rv = phy_82764_per_lane_prbs_tx_enable_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
        rv = phy_82764_per_lane_prbs_rx_status_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        rv = phy_82764_per_lane_rx_peak_filter_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_VGA:
        rv = phy_82764_per_lane_rx_vga_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = phy_82764_per_lane_rx_dfe_tap_control_get(pmc, port, intf, lane, 0, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = phy_82764_per_lane_rx_dfe_tap_control_get(pmc, port, intf, lane, 1, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = phy_82764_per_lane_rx_dfe_tap_control_get(pmc, port, intf, lane, 2, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = phy_82764_per_lane_rx_dfe_tap_control_get(pmc, port, intf, lane, 3, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = phy_82764_per_lane_rx_dfe_tap_control_get(pmc, port, intf, lane, 4, value);
        break;
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        rv = phy_82764_per_lane_rx_low_freq_filter_get(pmc, port, intf, lane, value);
        break;
    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
       rv = phy_82764_per_lane_loopback_remote_get(pmc, port, intf, lane, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
       rv = phy_82764_per_lane_loopback_internal_pmd_get(pmc, port, intf, lane, value);
       break;
    case SOC_PHY_CONTROL_RX_POLARITY:
        rv = phy_82764_per_lane_rx_polarity_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        rv = phy_82764_per_lane_tx_polarity_get(pmc, port, intf, lane, value);
        break;
    /* POWER */
    case SOC_PHY_CONTROL_POWER:
       rv = phy_82764_per_lane_power_get(pmc, port, intf, lane, value);
       break;
    case SOC_PHY_CONTROL_TX_FIR_PRE:
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
    case SOC_PHY_CONTROL_TX_FIR_POST:
    case SOC_PHY_CONTROL_TX_FIR_POST2:
    case SOC_PHY_CONTROL_TX_FIR_POST3:
        rv = phy_82764_per_lane_tx_get(pmc, port, intf, type, lane, value);
        break;
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = phy_82764_per_lane_tx_lane_squelch_get(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_CL72:
        rv = phy_82764_per_lane_cl72_enable_get(pmc, port, intf, lane, value);
    break;
    case SOC_PHY_CONTROL_CL72_STATUS:
        rv = phy_82764_per_lane_cl72_status_get(pmc, port, intf, lane,  value);
    break;
    case SOC_PHY_CONTROL_SHORT_CHANNEL_MODE:
        rv = phy_82764_per_lane_short_chn_mode_enable_get(pmc, port, intf, lane, value);
    break;
    case SOC_PHY_CONTROL_SHORT_CHANNEL_MODE_STATUS:
        rv = phy_82764_per_lane_short_chn_mode_status_get(pmc, port, intf, lane, value);
    break;
    case SOC_PHY_CONTROL_EYE_VAL_HZ_L:
        rv=phy_82764_per_lane_eye_margin_est_get(pmc,port,intf,lane, phymod_eye_marign_HZ_L, value);
        break ;
    case SOC_PHY_CONTROL_EYE_VAL_HZ_R:
        rv=phy_82764_per_lane_eye_margin_est_get(pmc,port,intf,lane, phymod_eye_marign_HZ_R, value);
        break ;
    case SOC_PHY_CONTROL_EYE_VAL_VT_D:
        rv=phy_82764_per_lane_eye_margin_est_get(pmc,port,intf,lane, phymod_eye_marign_VT_D, value);
        break ;
    case SOC_PHY_CONTROL_EYE_VAL_VT_U:
        rv=phy_82764_per_lane_eye_margin_est_get(pmc,port,intf,lane, phymod_eye_marign_VT_U, value);
        break ;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }

    return rv;
}

/*
 * Function:
 *      phy82764_duplex_get
 * Purpose:
 *      Get PHY duplex mode
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      duplex - current duplex mode
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy82764_duplex_get(int unit, soc_port_t port, int *duplex)
{
    *duplex = TRUE;
    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_82764_an_set
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
phy_82764_an_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    phymod_autoneg_control_t  an;
    soc_info_t *si;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    phymod_autoneg_control_t_init(&an);
    pmc = &pc->phymod_ctrl;
    si = &SOC_INFO(unit);

    /* only request autoneg on the first core */
    phy = pmc->phy[pmc->main_phy];
    if (phy == NULL) {
        return SOC_E_INTERNAL;
    }
    an.enable = enable;
    an.num_lane_adv = si->port_num_lanes[port];
    an.an_mode = phymod_AN_MODE_CL73;
    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_set(&phy->pm_phy, &an));

    return (SOC_E_NONE);
}
/*
 * Function:    
 *      phy_82764_an_get
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
phy_82764_an_get(int unit, soc_port_t port, int *an, int *an_done)
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
#ifdef PHY82764_DEBUG
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
 *      phy_82764_ability_advert_set
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
phy_82764_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;
    soc_port_if_t             line_interface;
    int                       an = 0, an_done = 0;
    uint8_t                   an_enabled = 0;
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

    /* if ability->interface is NULL, get the configured interface */
    if (!line_interface) {
        SOC_IF_ERROR_RETURN(phy82764_interface_get(unit, port, &line_interface));
    }
    /* When ability_advert_set called disable AN and set the ability */
    SOC_IF_ERROR_RETURN(phy_82764_an_get(unit, port, &an, &an_done));
    if (an) {
        SOC_IF_ERROR_RETURN(phy_82764_an_set(unit, port, FALSE));
        an_enabled = 1;
    }

    if (speed_full_duplex & SOC_PA_SPEED_40GB) {
        if (line_interface == SOC_PORT_IF_KR4) {
            PHYMOD_AN_CAP_40G_KR4_SET(an_tech_ability);
        } else {
            PHYMOD_AN_CAP_40G_CR4_SET(an_tech_ability);
        }
    } else if(speed_full_duplex & SOC_PA_SPEED_100GB) {
        if (line_interface == SOC_PORT_IF_KR4) {
            PHYMOD_AN_CAP_100G_KR4_SET(an_tech_ability);
        } else {
            PHYMOD_AN_CAP_100G_CR4_SET(an_tech_ability);
        }
    } else if (speed_full_duplex & SOC_PA_SPEED_10GB) {
        PHYMOD_AN_CAP_10G_KR_SET(an_tech_ability);
    } else {
        an_tech_ability = 0;
    }

    phymod_autoneg_ability.an_cap = an_tech_ability;
    phymod_autoneg_ability.cl73bam_cap = an_bam73_ability;
    phymod_autoneg_ability.cl37bam_cap = an_bam37_ability;
#ifdef PHY82764_DEBUG
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

    
    phymod_autoneg_ability.an_master_lane = soc_property_port_get(unit, port,
                                                "phy_autoneg_master_lane",
                                                phymod_autoneg_ability.an_master_lane);
    phymod_autoneg_ability.sgmii_speed = 2;
    phymod_autoneg_ability.an_cl72 = soc_property_port_get(unit, port, spn_PHY_AN_C72, TRUE);

    SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_ability_set(&phy->pm_phy, &phymod_autoneg_ability));

    /* Enable the AN after ability set */
    if (an_enabled) {
        SOC_IF_ERROR_RETURN(phy_82764_an_set(unit, port, TRUE));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82764_ability_advert_get
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
phy_82764_ability_advert_get(int unit, soc_port_t port,
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
    speed_full_duplex |= PHYMOD_AN_CAP_100G_CR4_GET(reg73_ability) ? SOC_PA_SPEED_100GB : 0;
    speed_full_duplex |= PHYMOD_AN_CAP_100G_KR4_GET(reg73_ability) ? SOC_PA_SPEED_100GB : 0;
    speed_full_duplex |= PHYMOD_AN_CAP_40G_CR4_GET(reg73_ability) ? SOC_PA_SPEED_40GB : 0;
    speed_full_duplex |= PHYMOD_AN_CAP_40G_KR4_GET(reg73_ability) ? SOC_PA_SPEED_40GB : 0;
    speed_full_duplex |= PHYMOD_AN_CAP_10G_KR_GET (reg73_ability)  ? SOC_PA_SPEED_10GB : 0;

    if (speed_full_duplex & SOC_PA_SPEED_100GB) {
        if (PHYMOD_AN_CAP_100G_CR4_GET(reg73_ability)) {
            ability->interface = SOC_PORT_IF_CR4;
        } else {
            ability->interface = SOC_PORT_IF_KR4;
        }
    } else if (speed_full_duplex & SOC_PA_SPEED_40GB) {
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
 *      phy82764_lb_get
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
phy82764_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t* pc; 
    soc_phymod_ctrl_t *pmc;
    uint32 out_en = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    pmc = &pc->phymod_ctrl;
    SOC_IF_ERROR_RETURN(
        phy_82764_loopback_internal_pmd_get(pmc, port, PHY82764_LINE_SIDE, &out_en));
    *enable = (int) out_en;

    return SOC_E_NONE;
}

/* 
 * phy_82764_per_lane_loopback_internal_pmd_set
 */
STATIC int 
phy_82764_per_lane_loopback_internal_pmd_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(&pm_phy_copy, phymodLoopbackGlobalPMD, value));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_loopback_internal_pmd_set
 */
STATIC int 
phy_82764_loopback_internal_pmd_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        } 

        SOC_IF_ERROR_RETURN
            (phymod_phy_loopback_set(&pm_phy_copy, phymodLoopbackGlobalPMD, value));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_loopback_remote_set
 */
STATIC int 
phy_82764_per_lane_loopback_remote_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    SOC_IF_ERROR_RETURN
        (phymod_phy_loopback_set(&pm_phy_copy, phymodLoopbackRemotePMD, value));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_fec_enable_set
 */
STATIC int 
phy_82764_fec_enable_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        }

        SOC_IF_ERROR_RETURN(phymod_phy_fec_enable_set(&pm_phy_copy, value));
    }

    return(SOC_E_NONE);
}

STATIC int
phy_82764_sdk_poly_to_phymod_poly(uint32 sdk_poly, phymod_prbs_poly_t *phymod_poly){
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
 * phy_82764_per_lane_prbs_tx_poly_set
 */
STATIC int
phy_82764_per_lane_prbs_tx_poly_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 
    
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy,  flags, &prbs));
    SOC_IF_ERROR_RETURN(phy_82764_sdk_poly_to_phymod_poly(value, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(&pm_phy_copy, flags, &prbs));

    return(SOC_E_NONE);
}
/* 
 * phy_82764_per_lane_prbs_rx_poly_set
 */
STATIC int
phy_82764_per_lane_prbs_rx_poly_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));
    SOC_IF_ERROR_RETURN(phy_82764_sdk_poly_to_phymod_poly(value, &prbs.poly));
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(&pm_phy_copy, flags, &prbs));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_prbs_tx_invert_data_set
 */
STATIC int
phy_82764_per_lane_prbs_tx_invert_data_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 
    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags, &prbs));
    prbs.invert = value;
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(&pm_phy_copy, flags,  &prbs));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_prbs_rx_invert_data_set
 */
STATIC int
phy_82764_per_lane_prbs_rx_invert_data_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_prbs_t        prbs;
    uint32_t flags = 0;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_get(&pm_phy_copy, flags,  &prbs));
    prbs.invert = value;
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_config_set(&pm_phy_copy, flags, &prbs));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_prbs_tx_enable_set
 */
STATIC int
phy_82764_per_lane_prbs_tx_enable_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(&pm_phy_copy, flags, value));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_prbs_rx_enable_set
 */
STATIC int
phy_82764_per_lane_prbs_rx_enable_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    uint32_t flags = 0;

    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 
    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SOC_IF_ERROR_RETURN(phymod_phy_prbs_enable_set(&pm_phy_copy, flags,  value));

    return(SOC_E_NONE);
}

/* 
 * given a pc (phymod_ctrl_t) and logical lane number, 
 *    find the correct soc_phymod_phy_t object and lane
 */
STATIC int
_phy_82764_find_soc_phy_lane(soc_phymod_ctrl_t *pmc, soc_port_t port, uint32_t lane, 
                             soc_phymod_phy_t **p_phy, uint32 *lane_map)
{
    phy82764_config_t   *pCfg;
    phy_ctrl_t          *pc;
    uint32_t            intf;
    int                 idx, found = 0;
    char                *lane_swap = NULL;
    phy82764_device_cfg_aux_mode_t *device_cfg_aux_modes;

    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(pmc->unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pCfg = (phy82764_config_t *) pc->driver_data;
    device_cfg_aux_modes = &(pCfg->device_cfg_aux_modes);
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82764_SYS_SIDE : PHY82764_LINE_SIDE;

    if (intf == PHY82764_LINE_SIDE) {
        lane_swap = (soc_property_port_get_str(pmc->unit, port, spn_PHY_XSW_LANE_MAP));
        if (lane_swap != NULL) {
            lane = my_ctoi(lane_swap, lane);
        }
    }

    /* Traverse lanes belonging to this port */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if(intf == PHY82764_SYS_SIDE) {
            switch (pc->speed_max) {    
                case PHY82764_SPD_100G:
                case PHY82764_SPD_106G:
                    if (lane > 3) {
                        return SOC_E_CONFIG;
                    }
                    *lane_map = 0x1 << lane;
                    found = 1;
                break;
                case PHY82764_SPD_40G: 
                case PHY82764_SPD_42G:
                    if ((!device_cfg_aux_modes->gearbox_enable) ||
                        (device_cfg_aux_modes->pin_compatibility_enable)) {
                        if (lane > 3) {
                            return SOC_E_CONFIG;
                        }
                        *lane_map = 0x1 << lane; 
                        found = 1;
                    } else {
                        if (lane > 1) {
                            return SOC_E_CONFIG;
                        }
                        if (pc->lane_num > 1) {   
                            *lane_map = 0x4 << lane;      
                            found = 1;
                        } else {
                            *lane_map = 0x1 << lane;
                            found = 1;
                        }
                    }
                break;
                case PHY82764_SPD_20G:
                case PHY82764_SPD_21G:
                    if (!device_cfg_aux_modes->gearbox_enable) {
                        if (lane > 1) {
                            return SOC_E_CONFIG;
                        }
                        if (pc->lane_num > 1) {
                            *lane_map = 0x4 << lane;
                            found = 1;
                        } else {
                            *lane_map = 0x1 << lane;
                            found = 1;
                        }
                    } else {
                        if (lane > 0) {
                            return SOC_E_CONFIG;
                        }
                        if (pc->lane_num > 0) {
                            *lane_map = 0x4;
                            found = 1;
                        } else {
                            *lane_map = 0x1;
                            found = 1;
                        }
                    }
                break;
                case PHY82764_SPD_1G:
                case PHY82764_SPD_10G:
                case PHY82764_SPD_11G:
                    if (lane > 0) {
                        return SOC_E_CONFIG;
                    }
                    *lane_map = 0x1 << pc->lane_num;
                    found = 1;
                break;
                default:
                    return SOC_E_CONFIG;
                   
            }
        } else {
            switch (pc->speed_max) {    
                case PHY82764_SPD_100G:
                case PHY82764_SPD_106G:
                    if (lane > 9) {
                        return SOC_E_CONFIG;
                    }
                    *lane_map = 0x1 << lane;
                    found = 1;
                break;
                case PHY82764_SPD_40G: 
                case PHY82764_SPD_42G:
                    if (lane > 3) {
                        return SOC_E_CONFIG;
                    }
                    if ((!device_cfg_aux_modes->gearbox_enable) &&
                        (!device_cfg_aux_modes->pin_compatibility_enable)) {
                        if (lane > 1) {
                            *lane_map = 0x10 << (lane - 2);
                            found = 1;
                        } else {
                            *lane_map = 0x1 << lane; 
                            found = 1;
                        }
                    } else if (device_cfg_aux_modes->pin_compatibility_enable) {
                        *lane_map = 0x1 << lane;   
                        found = 1;
                    } else {
                        if (pc->lane_num > 1) {
                            *lane_map = 0x10 << lane;
                            found = 1;
                        } else {
                            *lane_map = 0x1 << lane;
                            found = 1;
                        }
                    }     
                break;
                case PHY82764_SPD_20G:
                case PHY82764_SPD_21G:
                    if (!device_cfg_aux_modes->gearbox_enable) {
                        if (pc->lane_num > 1) {
                            *lane_map = 0x10 << ((lane_swap != NULL) ? (lane - 2) : lane);
                            found = 1;
                        } else {
                            *lane_map = 0x1 << lane;
                            found = 1;
                        }
                    } else {
                        if (pc->lane_num > 0) {
                            *lane_map = 0x10 << ((lane_swap != NULL) ? (lane - 2) : lane);
                            found = 1;
                        } else {
                            *lane_map = 0x1 << lane;
                            found = 1;
                        }
                    }
                break;
                case PHY82764_SPD_1G:
                case PHY82764_SPD_10G:
                case PHY82764_SPD_11G:
                    if (device_cfg_aux_modes->pin_compatibility_enable) {
                        *lane_map = 0x1 << ((lane_swap != NULL) ? lane : pc->lane_num);
                        found = 1;
                    } else {
                        if (pc->lane_num > 1) {
                            *lane_map = 0x10 << ((lane_swap != NULL) ? (lane - 2) : (pc->lane_num - 2));
                            found = 1;
                        } else {
                            *lane_map = 0x1 << (pc->lane_num);
                            found = 1;
                        }
                    }
                break;
                default:
                    return SOC_E_CONFIG;
            }
        }
        /* coverity[dead_error_condition] */ 
        if (found) {
            *p_phy = pmc->phy[idx];
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
 * phy_82764_per_lane_preemphasis_set
 */
STATIC int
phy_82764_per_lane_preemphasis_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    phymod_tx.pre = value;
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_tx_fir_pre_set
 */
STATIC int
phy_82764_tx_fir_pre_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        } 

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.pre = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82764_tx_fir_main_set
 */
STATIC int
phy_82764_tx_fir_main_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        } 

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.main = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82764_tx_fir_post_set
 */
STATIC int
phy_82764_tx_fir_post_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        } 

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.post = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82764_tx_fir_post2_set
 */
STATIC int
phy_82764_tx_fir_post2_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        } 

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.post2 = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_driver_current_set
 */
STATIC int
phy_82764_per_lane_driver_current_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
    phymod_tx.amp = value;
    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_driver_current_set
 */
STATIC int
phy_82764_driver_current_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        } 

        SOC_IF_ERROR_RETURN(phymod_phy_tx_get(&pm_phy_copy, &phymod_tx));
        phymod_tx.amp = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_rx_dfe_tap_control_set
 */
STATIC int
phy_82764_per_lane_rx_dfe_tap_control_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, int tap, int enable, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t          phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    if (tap < 0 || tap >= COUNTOF(phymod_rx.dfe)) {
        /* this can only happen with a coding error */
        return SOC_E_INTERNAL;
    }
    sal_memset(&phymod_rx, 0, sizeof(phymod_rx));
    phymod_rx.dfe[tap].enable = enable;
    phymod_rx.dfe[tap].value = value;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(&pm_phy_copy, &phymod_rx));
    if (!enable) {
        SOC_IF_ERROR_RETURN(phymod_phy_rx_adaptation_resume(&pm_phy_copy));
    }

    return(SOC_E_NONE);
}
/*
 * phy_82764_per_lane_tx_lane_squelch
 */
STATIC int
phy_82764_per_lane_tx_lane_squelch(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_tx_lane_control_t  tx_control;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.access.lane_mask = lane_map;
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
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
phy_82764_per_lane_tx_lane_squelch_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_tx_lane_control_t  tx_control;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;

    if (pm_phy == NULL) {
        return SOC_E_INTERNAL;
    }
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    }

    tx_control = phymodTxSquelchOn;
    SOC_IF_ERROR_RETURN
        (phymod_phy_tx_lane_control_get(&pm_phy_copy, &tx_control));
    if (tx_control == phymodTxSquelchOn) {
        *value = 1;
    } else {
        *value = 0;
    }

    return(SOC_E_NONE);
} 

/* 
 * phy_82764_per_lane_rx_peak_filter_set
 */
STATIC int
phy_82764_per_lane_rx_peak_filter_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, int enable, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 
    sal_memset(&phymod_rx, 0, sizeof(phymod_rx));
    phymod_rx.peaking_filter.enable = TRUE;
    phymod_rx.peaking_filter.value = value;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(&pm_phy_copy, &phymod_rx));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_rx_vga_set
 */
STATIC int
phy_82764_per_lane_rx_vga_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, int enable, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_rx_t         phymod_rx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    sal_memset(&phymod_rx, 0, sizeof(phymod_rx));
    phymod_rx.vga.enable = TRUE;
    phymod_rx.vga.value = value;
    SOC_IF_ERROR_RETURN(phymod_phy_rx_set(&pm_phy_copy, &phymod_rx));
    if (!enable) {
        SOC_IF_ERROR_RETURN(phymod_phy_rx_adaptation_resume(&pm_phy_copy));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82764_pi_control_set
 */
STATIC int 
phy_82764_pi_control_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        } 

        phymod_tx_override_t_init(&tx_override);
        tx_override.phase_interpolator.enable = (value == 0) ? 0 : 1;
        tx_override.phase_interpolator.value = value;
        SOC_IF_ERROR_RETURN(phymod_phy_tx_override_set(&pm_phy_copy, &tx_override));
    }

    return(SOC_E_NONE);
}

STATIC int 
phy_82764_per_lane_tx_polarity_set(soc_phymod_ctrl_t *pmc, soc_port_t port, phymod_polarity_t *cfg_polarity, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
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
 * phy_82764_per_lane_rx_polarity_set
 */
STATIC int 
phy_82764_per_lane_rx_polarity_set(soc_phymod_ctrl_t *pmc, soc_port_t port, phymod_polarity_t *cfg_polarity, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_polarity_t    polarity;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
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
 * phy_82764_rx_reset
 */
STATIC int
phy_82764_rx_reset(soc_phymod_ctrl_t *pmc, soc_port_t port, phymod_phy_reset_t *cfg_reset, int32 intf, uint32 value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
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
 * phy_82764_tx_reset
 */
STATIC int
phy_82764_tx_reset(soc_phymod_ctrl_t *pmc, soc_port_t port, phymod_phy_reset_t *cfg_reset, int32 intf, uint32 value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
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
 * phy_82764_per_lane_power_set
 */
STATIC int
phy_82764_per_lane_power_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_phy_power_t   power;
    uint32               lane_map;
    phymod_phy_access_t  pm_phy_copy;
    soc_phymod_phy_t     *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
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
 * phy_82764_power_set
 */
STATIC int
phy_82764_power_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_power_t      power;
    int                     idx;
    phymod_phy_access_t     pm_phy_copy;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
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

STATIC int 
phy_82764_rx_seq_restart(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value)
{
    int                  idx;
    uint32_t             flags = 0;
    soc_phymod_core_t    *pmc_core;
    phymod_core_access_t *pm_core;
    phymod_core_access_t pm_core_copy;
    phymod_phy_access_t  pm_phy_copy;
    phymod_sequencer_operation_t seq_operation;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pmc_core = pmc->phy[idx]->core;
        pm_core = &pmc_core->pm_core;
        if (pm_core == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_core_copy, pm_core, sizeof(pm_core_copy));
        pm_core_copy.port_loc = phymodPortLocLine; 
        if(intf == PHY82764_SYS_SIDE) {
            pm_core_copy.port_loc = phymodPortLocSys; 
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
            pm_core_copy.access.lane_mask = pm_phy_copy.access.lane_mask;
        } 
        seq_operation = phymodSeqOpRestart; 
        SOC_IF_ERROR_RETURN(phymod_core_pll_sequencer_restart(&pm_core_copy, flags, seq_operation));
    }

    return(SOC_E_NONE);
}

STATIC int
phy_82764_per_lane_phy_dump(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane)
{
    phymod_phy_access_t *pm_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_status_dump(&pm_phy_copy));

    return(SOC_E_NONE);
}
/*
 * phy_82764_gpio_config_set
 */
STATIC int
phy_82764_gpio_config_set(soc_phymod_ctrl_t *pmc, soc_port_t port, uint32 value)
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

        for (gpio_pin_no = 0; gpio_pin_no < 4; gpio_pin_no++) {
            gpio_mode = (value >> (4 * gpio_pin_no)) & 0xF;
            SOC_IF_ERROR_RETURN(phymod_phy_gpio_config_set(&pm_phy_copy, gpio_pin_no, (phymod_gpio_mode_t)gpio_mode));
        }
    }

    return(SOC_E_NONE);
}

/*
 * phy_82764_gpio_value_set
 */
STATIC int
phy_82764_gpio_value_set(soc_phymod_ctrl_t *pmc, soc_port_t port, uint32 value)
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

        for (gpio_pin_no = 0; gpio_pin_no < 4; gpio_pin_no++) {
            pin_value = (value >> (4 * gpio_pin_no)) & 0xF;
            SOC_IF_ERROR_RETURN(phymod_phy_gpio_pin_value_set(&pm_phy_copy, gpio_pin_no, pin_value));
        }
    }

    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_82764_control_set
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
phy_82764_control_set(int unit, soc_port_t port, soc_phy_control_t type, uint32 value)
{
    int                 rv = 0;
    int32               intf;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    phy82764_config_t   *pCfg;
    int                 lane, num_lanes;

    PHY_CONTROL_TYPE_CHECK(type);

    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;
    pCfg = (phy82764_config_t *) pc->driver_data;
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82764_SYS_SIDE : PHY82764_LINE_SIDE; 

    /* get number of lanes per port on line side */
    num_lanes = (intf == PHY82764_LINE_SIDE) ? port_num_lanes_line_side_get(unit, port, pc->speed_max):
                                               SOC_INFO(unit).port_num_lanes[port];
  if (!((type == SOC_PHY_CONTROL_RX_RESET) || (type == SOC_PHY_CONTROL_TX_RESET) ||
        (type == SOC_PHY_CONTROL_INTR_MASK) || (type == SOC_PHY_CONTROL_GPIO_CONFIG) ||
        (type == SOC_PHY_CONTROL_GPIO_VALUE) || (type == SOC_PHY_CONTROL_FIRMWARE_MODE) ||
        (type == SOC_PHY_CONTROL_RX_SEQ_TOGGLE) || (type == SOC_PHY_CONTROL_PHASE_INTERP) ||
        (type == SOC_PHY_CONTROL_POWER) || (type == SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION) ||
        (type == SOC_PHY_CONTROL_FIRMWARE_DFE_ENABLE) || (type == SOC_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE) ||
        (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE0) || (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE1) ||
        (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE2) || (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE3) ||
        (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0) || (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1) ||
        (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2) || (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3) ||
        (type == SOC_PHY_CONTROL_CL72) || (type == SOC_PHY_CONTROL_UNRELIABLE_LOS))) {
      for (lane = 0; lane < num_lanes; lane++) {
         rv |= phy_82764_per_lane_control_set(unit, port, lane, type, value);
      }
  } else {
    switch(type) {
    case SOC_PHY_CONTROL_CL72:
        rv = phy_82764_cl72_enable_set(pmc, port, intf, value);
        break;
    /* PREEMPHASIS */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        rv = phy_82764_per_lane_preemphasis_set(pmc, port, intf, 0, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        rv = phy_82764_per_lane_preemphasis_set(pmc, port, intf, 1, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        rv = phy_82764_per_lane_preemphasis_set(pmc, port, intf, 2, value);
        break;
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        rv = phy_82764_per_lane_preemphasis_set(pmc, port, intf, 3, value);
        break;
    /* DRIVER CURRENT */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        rv = phy_82764_per_lane_driver_current_set(pmc, port, intf, 0, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        rv = phy_82764_per_lane_driver_current_set(pmc, port, intf, 1, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        rv = phy_82764_per_lane_driver_current_set(pmc, port, intf, 2, value);
        break;
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        rv = phy_82764_per_lane_driver_current_set(pmc, port, intf, 3, value);
        break;
    /* PHASE INTERPOLATOR */
    case SOC_PHY_CONTROL_PHASE_INTERP:
        rv = phy_82764_pi_control_set(pmc, port, intf, value);
        break;
    /* RESET */
    case SOC_PHY_CONTROL_RX_RESET:
        rv = phy_82764_rx_reset(pmc, port, &pCfg->phy_reset_config, intf, value);
        break;
    case SOC_PHY_CONTROL_TX_RESET:
        rv = phy_82764_tx_reset(pmc, port, &pCfg->phy_reset_config, intf, value);
        break;
    /* POWER */
    case SOC_PHY_CONTROL_POWER:
       {
           phy_ctrl_t                *int_pc;
           int_pc = INT_PHY_SW_STATE(unit, port);
           if (int_pc != NULL) {
               SOC_IF_ERROR_RETURN(PHY_ENABLE_SET(int_pc->pd, unit, port, value));
               rv = phy_82764_power_set(pmc, port, intf, value);
           }
       }
       break;
    case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
       rv = phy_82764_fec_enable_set(pmc, port, intf, value);
       break;
    case SOC_PHY_CONTROL_RX_SEQ_TOGGLE:
        rv = phy_82764_rx_seq_restart(pmc, port, intf, value);
    break;
    case SOC_PHY_CONTROL_INTR_MASK:
        rv = phy_82764_intr_enable_set(pmc, port, intf, value);
    break;
    case SOC_PHY_CONTROL_GPIO_CONFIG:
        rv = phy_82764_gpio_config_set(pmc, port, value);
    break;
    case SOC_PHY_CONTROL_GPIO_VALUE:
        rv = phy_82764_gpio_value_set(pmc, port, value);
    break;
    /* FIRMWARE MODE */
    case SOC_PHY_CONTROL_FIRMWARE_MODE:
        rv = phy_82764_firmware_mode_set(pc, port, intf, value);
        break;
    case SOC_PHY_CONTROL_FIRMWARE_DFE_ENABLE:
        rv = phy_82764_firmware_dfe_enable_set(pc, port, intf, value);
        break;
    case SOC_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE:
        rv = phy_82764_firmware_lp_dfe_enable_set(pc, port, intf, value);
        break;
    case SOC_PHY_CONTROL_UNRELIABLE_LOS:
        rv = phy_82764_unreliable_los_set(pc, port, intf, value);
        break;
       /*
        * COVERITY
        * This is unreachable. It is kept intentionally as a defensive
        * default for future development.
        */
       /* coverity[dead_error_begin] */
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
  }

    return rv;
}
/*
 * Function:
 *      phy_82764_per_lane_control_set
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
phy_82764_per_lane_control_set(int unit, soc_port_t port, int lane, soc_phy_control_t type, uint32 value)
{
    int                 rv = -1;
    int32               intf;
    phy_ctrl_t          *pc;
    soc_phymod_ctrl_t   *pmc;
    phy82764_config_t   *pCfg;

    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pmc = &pc->phymod_ctrl;
    pCfg = (phy82764_config_t *) pc->driver_data;

    PHY_CONTROL_TYPE_CHECK(type);

    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82764_SYS_SIDE : PHY82764_LINE_SIDE; 

    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        rv = phy_82764_per_lane_preemphasis_set(pmc, port, intf, lane, value);
        break; 
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        rv = phy_82764_per_lane_driver_current_set(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP1:
        rv = phy_82764_per_lane_rx_dfe_tap_control_set (pmc, port, intf, lane, 0 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP1_RELEASE:
        rv = phy_82764_per_lane_rx_dfe_tap_control_set (pmc, port, intf, lane, 0 /* tap */, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_RX_TAP2:
        rv = phy_82764_per_lane_rx_dfe_tap_control_set (pmc, port, intf, lane, 1 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP2_RELEASE:
        rv = phy_82764_per_lane_rx_dfe_tap_control_set (pmc, port, intf, lane, 1 /* tap */, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_RX_TAP3:
        rv = phy_82764_per_lane_rx_dfe_tap_control_set (pmc, port, intf, lane, 2 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP3_RELEASE:
        rv = phy_82764_per_lane_rx_dfe_tap_control_set (pmc, port, intf, lane, 2 /* tap */, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_RX_TAP4:
        rv = phy_82764_per_lane_rx_dfe_tap_control_set (pmc, port, intf, lane, 3 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP4_RELEASE:
        rv = phy_82764_per_lane_rx_dfe_tap_control_set (pmc, port, intf, lane, 3 /* tap */, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_RX_TAP5:
        rv = phy_82764_per_lane_rx_dfe_tap_control_set (pmc, port, intf, lane, 4 /* tap */, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_TAP5_RELEASE:
        rv = phy_82764_per_lane_rx_dfe_tap_control_set (pmc, port, intf, lane, 4 /* tap */, 0 /* release */, 0x8000);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        rv = phy_82764_per_lane_prbs_tx_poly_set(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        rv = phy_82764_per_lane_prbs_tx_invert_data_set(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        rv = phy_82764_per_lane_prbs_tx_enable_set(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        rv = phy_82764_per_lane_prbs_rx_poly_set(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        rv = phy_82764_per_lane_prbs_rx_invert_data_set(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        rv = phy_82764_per_lane_prbs_rx_enable_set(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = phy_82764_per_lane_prbs_tx_poly_set(pmc, port, intf, lane, value);
        rv = phy_82764_per_lane_prbs_rx_poly_set(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = phy_82764_per_lane_prbs_tx_invert_data_set(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        rv = phy_82764_per_lane_prbs_rx_invert_data_set(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        /* TX_ENABLE does both tx and rx */
        rv = phy_82764_per_lane_prbs_tx_enable_set(pmc, port, intf, lane, value);
        rv = phy_82764_per_lane_prbs_rx_enable_set(pmc, port, intf, lane, value);
        break; 
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = phy_82764_per_lane_prbs_tx_enable_set(pmc, port, intf, lane, value);
        rv = phy_82764_per_lane_prbs_rx_enable_set(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        rv = phy_82764_per_lane_rx_peak_filter_set(pmc, port, intf, lane, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        rv = phy_82764_per_lane_rx_low_freq_filter_set(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_RX_VGA:
        rv = phy_82764_per_lane_rx_vga_set(pmc, port, intf, lane, 1 /* enable */, value);
        break;
    case SOC_PHY_CONTROL_RX_VGA_RELEASE:
        rv = phy_82764_per_lane_rx_vga_set(pmc, port, intf, lane, 0 /* release */, 0x8000);
        break;
    /* TX LANE SQUELCH */
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = phy_82764_per_lane_tx_lane_squelch(pmc, port, intf, lane, value);
        break;
    /* POWER */
    case SOC_PHY_CONTROL_POWER:
       rv = phy_82764_per_lane_power_set(pmc, port, intf, lane, value);
       break;
    /* POLARITY */
    case SOC_PHY_CONTROL_RX_POLARITY:
        rv = phy_82764_per_lane_rx_polarity_set(pmc, port, &pCfg->phy_polarity_config, intf, lane, value);;
        break;
    case SOC_PHY_CONTROL_TX_POLARITY:
        rv = phy_82764_per_lane_tx_polarity_set(pmc, port, &pCfg->phy_polarity_config, intf, lane, value);
        break;
    /* LOOPBACK */
    case SOC_PHY_CONTROL_LOOPBACK_PMD:
        rv = phy_82764_per_lane_loopback_internal_pmd_set(pmc, port, intf, lane, value);
       break;
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
    case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
        rv = phy_82764_per_lane_loopback_remote_set(pmc, port, intf, lane, value);
        break;
    case SOC_PHY_CONTROL_DUMP:
        rv = phy_82764_per_lane_phy_dump(pmc, port, intf, lane);
    break;
    case SOC_PHY_CONTROL_TX_FIR_PRE:
    case SOC_PHY_CONTROL_TX_FIR_MAIN:
    case SOC_PHY_CONTROL_TX_FIR_POST:
    case SOC_PHY_CONTROL_TX_FIR_POST2:
    case SOC_PHY_CONTROL_TX_FIR_POST3:
        rv = phy_82764_per_lane_tx_set(pmc, port, intf, type, lane, value);
    break;
    case SOC_PHY_CONTROL_CL72:
        rv = phy_82764_per_lane_cl72_enable_set(pmc, port, intf, lane, value);
    break;
    break;
    case SOC_PHY_CONTROL_SHORT_CHANNEL_MODE:
        rv = phy_82764_per_lane_short_chn_mode_enable_set(pmc, port, intf, lane, value);
    break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }

    return rv;
}
/*
 * phy_82764_firmware_mode_set
 */
STATIC int
phy_82764_firmware_mode_set(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 value)
{
    int idx;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    phymod_phy_access_t pm_phy_copy;
    phymod_firmware_lane_config_t fw_config;
    pmc = &pc->phymod_ctrl;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return(SOC_E_INTERNAL);
        }
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return(SOC_E_INTERNAL);
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        }

        sal_memset(&fw_config, 0, sizeof(fw_config));
        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

        switch (value) {
            case SOC_PHY_FIRMWARE_DEFAULT:
                fw_config.DfeOn = 0xF;
                fw_config.LpDfeOn = 0xF;
                fw_config.ForceBrDfe = 0xF;
                break;
            case SOC_PHY_FIRMWARE_FORCE_OSDFE:
                fw_config.DfeOn = 1;
                fw_config.ForceBrDfe = 0;
                break;
            case SOC_PHY_FIRMWARE_SFP_DAC:
                fw_config.MediaType = phymodFirmwareMediaTypeCopperCable;
                break;
            case SOC_PHY_FIRMWARE_XLAUI:
                fw_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                break;
            case SOC_PHY_FIRMWARE_SFP_OPT_SR4:
                fw_config.MediaType = phymodFirmwareMediaTypeOptics;
                break;
            case SOC_PHY_FIRMWARE_LP_DFE_ENABLE:
                fw_config.LpDfeOn = 1;
                fw_config.DfeOn = 1;
                break;
            case SOC_PHY_FIRMWARE_DFE_ENABLE:
                fw_config.DfeOn = 1;
                break;
            default:
                return SOC_E_UNAVAIL;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_set(&pm_phy_copy, fw_config));
    }

    return(SOC_E_NONE);
}

/*
 * phy_82764_firmware_dfe_enable_set
 */
STATIC int
phy_82764_firmware_dfe_enable_set(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 enable)
{
    int idx;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t pm_phy_copy;
    phymod_phy_access_t *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    pmc = &pc->phymod_ctrl;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return(SOC_E_INTERNAL);
        }

        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return(SOC_E_INTERNAL);
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        }

        sal_memset(&fw_config, 0, sizeof(fw_config));
        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

        if (enable) {
            fw_config.DfeOn = 1;
        } else {
            fw_config.DfeOn = 0;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_set(&pm_phy_copy, fw_config));
    }

    return(SOC_E_NONE);
}

/*
 * phy_82764_firmware_lp_dfe_enable_set
 */
STATIC int
phy_82764_firmware_lp_dfe_enable_set(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 enable)
{
    int idx;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t pm_phy_copy;
    phymod_phy_access_t *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    pmc = &pc->phymod_ctrl;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return(SOC_E_INTERNAL);
        }

        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return(SOC_E_INTERNAL);
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        }

        sal_memset(&fw_config, 0, sizeof(fw_config));
        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

        if (enable) {
            fw_config.DfeOn = 1;
            fw_config.LpDfeOn = 1;
        } else {
            fw_config.LpDfeOn = 0;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_set(&pm_phy_copy, fw_config));
    }

    return(SOC_E_NONE);
}

/*
 * phy_82764_unreliable_los_set
 */
STATIC int
phy_82764_unreliable_los_set(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 enable)
{
    int idx;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t pm_phy_copy;
    phymod_phy_access_t *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    pmc = &pc->phymod_ctrl;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return(SOC_E_INTERNAL);
        }

        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return(SOC_E_INTERNAL);
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        }

        sal_memset(&fw_config, 0, sizeof(fw_config));
        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

        if (enable) {
            fw_config.UnreliableLos = 1;
        } else {
            fw_config.UnreliableLos = 0;
        }

        SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_set(&pm_phy_copy, fw_config));
    }

    return(SOC_E_NONE);
}

/*
 * phy_82764_firmware_mode_get
 */
STATIC int
phy_82764_firmware_mode_get(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 *value)
{
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t pm_phy_copy;
    phymod_phy_access_t *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    pmc = &pc->phymod_ctrl;
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
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
        SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
    }

    sal_memset(&fw_config, 0, sizeof(fw_config));
    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

    if ((fw_config.LpDfeOn) && (fw_config.DfeOn)) {
        *value = SOC_PHY_FIRMWARE_LP_DFE_ENABLE;
    } else if (fw_config.MediaType == phymodFirmwareMediaTypeCopperCable) {
        *value = SOC_PHY_FIRMWARE_SFP_DAC;
    } else if (fw_config.MediaType == phymodFirmwareMediaTypePcbTraceBackPlane) {
        *value = SOC_PHY_FIRMWARE_XLAUI;
    } else if (fw_config.MediaType == phymodFirmwareMediaTypeOptics){
        *value = SOC_PHY_FIRMWARE_SFP_OPT_SR4;
    } else if ((fw_config.DfeOn) && (fw_config.ForceBrDfe == 0) && (fw_config.LpDfeOn == 0)) {
        *value = SOC_PHY_FIRMWARE_DFE_ENABLE;
    } else if ((fw_config.DfeOn) && (fw_config.ForceBrDfe == 0)) {
        *value = SOC_PHY_FIRMWARE_FORCE_OSDFE;
    } else {
        *value = SOC_PHY_FIRMWARE_DEFAULT;
    }

    return(SOC_E_NONE);
}
/*
 * phy_82764_firmware_dfe_enable_get
 */
STATIC int
phy_82764_firmware_dfe_enable_get(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 *value)
{
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t pm_phy_copy;
    phymod_phy_access_t *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    pmc = &pc->phymod_ctrl;
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
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
        SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
    }

    sal_memset(&fw_config, 0, sizeof(fw_config));
    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

    if (fw_config.DfeOn) {
        *value = 1;
    } else {
        *value = 0;
    }

    return(SOC_E_NONE);
}
/*
 * phy_82764_firmware_lp_dfe_enable_get
 */
STATIC int
phy_82764_firmware_lp_dfe_enable_get(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 *value)
{
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    phymod_phy_access_t pm_phy_copy;
    pmc = &pc->phymod_ctrl;
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
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
        SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
    }

    sal_memset(&fw_config, 0, sizeof(fw_config));
    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));

    if (fw_config.LpDfeOn && fw_config.DfeOn) {
        *value = 1;
    } else {
        *value = 0;
    }

    return(SOC_E_NONE);
}

/*
 * phy_82764_unreliable_los_get
 */
STATIC int
phy_82764_unreliable_los_get(phy_ctrl_t *pc, soc_port_t port, int32 intf, uint32 *value)
{
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    phymod_firmware_lane_config_t fw_config;
    phymod_phy_access_t pm_phy_copy;
    pmc = &pc->phymod_ctrl;
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
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
        SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
    }

    sal_memset(&fw_config, 0, sizeof(fw_config));
    SOC_IF_ERROR_RETURN(phymod_phy_firmware_lane_config_get(&pm_phy_copy, &fw_config));
    if (fw_config.UnreliableLos) {
        *value = 1;
    } else {
        *value = 0;
    }

    return(SOC_E_NONE);
}

STATIC int phy_82764_diag_eyescan(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int32 lane)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx = 0, rv = 0;
    phymod_phy_access_t     pm_phy_copy;
    soc_phymod_phy_t        *p_phy;
    uint32_t                lane_map = 0;


    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        p_phy = pmc->phy[idx];
        /* Make a copy of the phy access and overwrite the desired lane */
        pm_phy = &p_phy->pm_phy;
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY_DIAG_INTF_SYS) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        }
        if (lane == 0xf) {
            /* Get p_phy alone and let lane map be as Phy structure value */
            lane = 1;
            SOC_IF_ERROR_RETURN(
                    _phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));
            lane = 0xf;
        } else {
            SOC_IF_ERROR_RETURN(
                _phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));
        }

        /* Make a copy of the phy access and overwrite the desired lane */
        if (lane != 0xf) {
           pm_phy_copy.access.lane_mask = lane_map;
        }

        eye_scan_enabled = 1;
        rv = phymod_phy_eyescan_run(&pm_phy_copy, PHYMOD_EYESCAN_F_DONE, 0, NULL);
        if (rv != SOC_E_NONE) {
            eye_scan_enabled = 0;
            return rv;
        }
    }
    eye_scan_enabled = 0;

    return SOC_E_NONE;
}

STATIC int phy_82764_diag_ber(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int32 lane, void *params)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx = 0, rv = 0;
    phymod_phy_access_t     pm_phy_copy;
    soc_phymod_phy_t        *p_phy;
    uint32_t                lane_map = 0;
    phymod_phy_eyescan_options_t eyescan_options;

    for (idx = 0; idx < pmc->num_phys; idx++) {
        if (pmc->phy[idx] == NULL) {
            return SOC_E_INTERNAL;
        }
        p_phy = pmc->phy[idx];
        /* Make a copy of the phy access and overwrite the desired lane */
        pm_phy = &p_phy->pm_phy;
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY_DIAG_INTF_SYS) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        }
        if (lane == 0xf) {
            /* Get p_phy alone and let lane map be as Phy structure value */
            lane = 1;
            SOC_IF_ERROR_RETURN(
                    _phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));
            lane = 0xf;
        } else {
            SOC_IF_ERROR_RETURN(
                _phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));
        }

        /* Make a copy of the phy access and overwrite the desired lane */
        if (lane != 0xf) {
           pm_phy_copy.access.lane_mask = lane_map;
        }

        eye_scan_enabled = 1;
        /* following param are used in BER projection */
        sal_memset(&eyescan_options, 0, sizeof(phymod_phy_eyescan_options_t));
        eyescan_options.ber_proj_scan_mode = ((soc_port_phy_eyescan_params_t *)params)->ber_proj_scan_mode;
        eyescan_options.ber_proj_timer_cnt = ((soc_port_phy_eyescan_params_t *)params)->ber_proj_timer_cnt;
        eyescan_options.ber_proj_err_cnt = ((soc_port_phy_eyescan_params_t *)params)->ber_proj_err_cnt;
        rv = phymod_phy_eyescan_run(&pm_phy_copy, PHYMOD_EYESCAN_F_DONE, phymodEyescanModeBERProj, &eyescan_options);
        if (rv != SOC_E_NONE) {
            eye_scan_enabled = 0;
            return rv;
        }
    }
    eye_scan_enabled = 0;

    return SOC_E_NONE;
}
STATIC int 
phy_82764_diag_dsc(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf)
{
    phymod_phy_access_t      *pm_phy;
    int                      idx;
    phymod_phy_access_t      pm_phy_copy;
    soc_phymod_phy_t         *p_phy;
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
        pm_phy_copy.port_loc = phymodPortLocLine;
        if(intf == PHY_DIAG_INTF_SYS) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        } 

        LOG_CLI((BSL_META_U(pmc->unit,
                                    "DSC parameters for port %d\n"), port));
        for (lane_index = 0; lane_index < 10; lane_index ++) {
            if ((pm_phy_copy.access.lane_mask & (1 << lane_index))) {
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
                         lane_index, phy_diag.signal_detect, phy_diag.vga_bias_reduced,
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
 *      phy_82764_diag_ctrl
 * Purpose:
 *      xx
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 

 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_82764_diag_ctrl(
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
            rv = phy_82764_diag_dsc(pmc, port, intf);
            break;
        case PHY_DIAG_CTRL_START_FAST_EYESCAN:
            rv =  phy_82764_diag_eyescan(pmc, port, intf, lane);
        break;
        case PHY_DIAG_CTRL_BER:
            rv =  phy_82764_diag_ber(pmc, port, intf, lane, arg);
        break;
        case PHY_DIAG_CTRL_LINKMON_MODE:
        {
            uint32_t value = PTR_TO_INT(arg);
            rv = phy_82764_link_monitor_set(pmc, port, intf, lane, value);
        }
        break;
        case PHY_DIAG_CTRL_LINKMON_STATUS:
        {
            rv = phy_82764_link_monitor_status_get(pmc, port, intf, lane);
        }
        break;
        default:
            if (op_type == PHY_DIAG_CTRL_SET) {
                    rv = phy_82764_control_set(unit,port,op_cmd,PTR_TO_INT(arg));
            } else if (op_type == PHY_DIAG_CTRL_GET) {
                    rv = phy_82764_control_get(unit,port,op_cmd,(uint32 *)arg);
            }
            break ;
    }

    /* clear the pc->flags  to make default as PHY_DIAG_INTF_LINE */
    pc->flags &= ~PHYCTRL_SYS_SIDE_CTRL;

    /* clear the pc->flags  to make default as PHY_DIAG_INTF_LINE */
    pc->flags &= ~PHYCTRL_SYS_SIDE_CTRL;

    return rv;
}

int32 phy_82764_core_reset(int32 unit, soc_port_t port, void *not_used)
{
    phy_ctrl_t                *pc;
    soc_phymod_core_t *pmc_core;
    int                  idx;
    phymod_core_access_t *pm_core;
    soc_phymod_ctrl_t *pmc;
    phymod_core_status_t core_status;
    phymod_reset_direction_t reset_direction;
    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    reset_direction = phymodResetDirectionIn;
    pmc = &pc->phymod_ctrl; 

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pmc_core = pmc->phy[idx]->core;
        pm_core = &pmc_core->pm_core;
        if (pm_core == NULL) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(phymod_core_reset_set(pm_core, phymodResetModeHard, reset_direction));
        core_status.pmd_active = 0;
        SOC_IF_ERROR_RETURN
            (phymod_core_init(&pmc_core->pm_core, &pmc_core->init_config, &core_status));
    }

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_tx_set
 */
STATIC int
phy_82764_per_lane_tx_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, soc_phy_control_t type, int lane, uint32 value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
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
        case SOC_PHY_CONTROL_TX_FIR_POST3:
            phymod_tx.post3 = value;
            break;
        default:
            return SOC_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_tx_set(&pm_phy_copy, &phymod_tx));

    return(SOC_E_NONE);
}

/* 
 * phy_82764_per_lane_tx_get
 */
STATIC int
phy_82764_per_lane_tx_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, soc_phy_control_t type, int lane, uint32 *value)
{
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy, *pm_phy;
    phymod_tx_t         phymod_tx;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
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
        case SOC_PHY_CONTROL_TX_FIR_POST3:
            *value = phymod_tx.post3;
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
phy_82764_ability_remote_get(int unit, soc_port_t port, 
                                  soc_port_ability_t *ability)
{
    int  an = 0, an_done = 0;
    int  line_intf;
    uint32_t an_cap = 0;
    phy_ctrl_t *pc, *int_pc;
    soc_phymod_ctrl_t         *pmc;
    phy82764_config_t         *pCfg;
    soc_phymod_phy_t          *phy;
    phymod_autoneg_ability_t  phymod_autoneg_ability;


    if (ability == NULL) {
        return SOC_E_PARAM;
    }
    pc = EXT_PHY_SW_STATE(unit, port);


    /* Only firmware has visibility into remote ability */
    sal_memset(ability, 0, sizeof(soc_port_ability_t));

    pmc = &pc->phymod_ctrl;
    pCfg = (phy82764_config_t *) pc->driver_data;

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
    SOC_IF_ERROR_RETURN(phy_82764_an_get(unit, port, &an, &an_done));
    if (an && an_done) {
       SOC_IF_ERROR_RETURN
        (phymod_phy_autoneg_remote_ability_get(&phy->pm_phy, &phymod_autoneg_ability));
       an_cap = phymod_autoneg_ability.an_cap;
       ability->speed_full_duplex |= PHYMOD_AN_CAP_100G_CR4_GET(an_cap) ? SOC_PA_SPEED_100GB : 0;
       ability->speed_full_duplex |= PHYMOD_AN_CAP_100G_KR4_GET(an_cap) ? SOC_PA_SPEED_100GB : 0;
       ability->speed_full_duplex |= PHYMOD_AN_CAP_40G_CR4_GET(an_cap) ? SOC_PA_SPEED_40GB : 0;
       ability->speed_full_duplex |= PHYMOD_AN_CAP_40G_KR4_GET(an_cap) ? SOC_PA_SPEED_40GB : 0;

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
                         "phy_82764_ability_remote_get: u=%d p=%d speed(FD)=0x%x pause=0x%x\n"),
              unit, port, ability->speed_full_duplex, ability->pause));

    return SOC_E_NONE;
}

STATIC int
phy_82764_per_lane_short_chn_mode_enable_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_short_chn_mode_enable_set(&pm_phy_copy, value));

    return(SOC_E_NONE);
}

STATIC int
phy_82764_per_lane_short_chn_mode_enable_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map, status = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_short_chn_mode_enable_get(&pm_phy_copy, value, &status));

    return(SOC_E_NONE);
}

STATIC int
phy_82764_per_lane_short_chn_mode_status_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map, enable = 0;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_short_chn_mode_enable_get(&pm_phy_copy, &enable, value));

    return(SOC_E_NONE);
}

STATIC int
phy_82764_per_lane_cl72_enable_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    }
    LOG_CLI((BSL_META_U(0,"Tx training: %d port loc:0x%x\n"), value, pm_phy_copy.port_loc));

    SOC_IF_ERROR_RETURN(phymod_phy_cl72_set(&pm_phy_copy, value));

    return(SOC_E_NONE);
}

STATIC int
phy_82764_per_lane_cl72_status_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_phy_access_t  pm_phy_copy;
    phymod_cl72_status_t status;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    }

    sal_memset(&status, 0, sizeof(status));
    SOC_IF_ERROR_RETURN(phymod_phy_cl72_status_get(&pm_phy_copy, &status));
    *value = status.locked;

    LOG_CLI((BSL_META_U(0,"Tx training Sts: %d Flags:0x%x\n"), *value, pm_phy_copy.access.flags));

    return(SOC_E_NONE);
}

STATIC int
phy_82764_per_lane_cl72_enable_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
    }

    SOC_IF_ERROR_RETURN(phymod_phy_cl72_get(&pm_phy_copy, value));

    return(SOC_E_NONE);
}

STATIC int
phy_82764_link_monitor_set (soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int32 lane, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map = 0;
    phymod_phy_access_t pm_phy_copy;
    uint32_t            link_mon_mode = 0;

    intf = (intf == PHY_DIAG_INTF_LINE) ? PHY82764_LINE_SIDE : PHY82764_SYS_SIDE;

    if (lane == 0xf) {
        /* Get p_phy alone and let lane map be as Phy structure value */
        lane = 1;
        SOC_IF_ERROR_RETURN(
                _phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));
        lane = 0xf;
    } else {
        SOC_IF_ERROR_RETURN(
                _phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));
    }
    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    if (lane != 0xf) {
        pm_phy_copy.access.lane_mask = lane_map;
    }

    pm_phy_copy.port_loc = phymodPortLocLine;
    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
        SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
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
phy_82764_link_monitor_status_get (soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, int32 lane)
{
    phymod_phy_access_t *pm_phy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map = 0;
    phymod_phy_access_t pm_phy_copy;
    uint32_t lock_status = 0, lock_lost_lh = 0, error_count = 0;

    intf = (intf == PHY_DIAG_INTF_LINE) ? PHY82764_LINE_SIDE : PHY82764_SYS_SIDE;

    if (lane == 0xf) {
        /* Get p_phy alone and let lane map be as Phy structure value */
        lane = 1;
        SOC_IF_ERROR_RETURN(
                _phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));
        lane = 0xf;
    } else {
        SOC_IF_ERROR_RETURN(
                _phy_82764_find_soc_phy_lane(pmc, port, lane, &p_phy, &lane_map));
    }
    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    if (lane != 0xf) {
        pm_phy_copy.access.lane_mask = lane_map;
    }

    pm_phy_copy.port_loc = phymodPortLocLine;

    if(intf == PHY82764_SYS_SIDE) {
        pm_phy_copy.port_loc = phymodPortLocSys;
        SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
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
phy_82764_intr_enable_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;

        if (intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        }

        SOC_IF_ERROR_RETURN(phymod_phy_intr_enable_set(&pm_phy_copy, value));
    }

    return(SOC_E_NONE);
}

STATIC int
phy_82764_intr_enable_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;
    phymod_phy_access_t pm_phy_copy;

    *value = 0;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.port_loc = phymodPortLocLine;

        if (intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        }

        SOC_IF_ERROR_RETURN(phymod_phy_intr_enable_get(&pm_phy_copy, value));
    }

    return(SOC_E_NONE);
}

STATIC int
phy_82764_intr_status_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;
    phymod_phy_access_t pm_phy_copy;

    *value = 0;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        SOC_IF_ERROR_RETURN(phymod_phy_intr_status_get(&pm_phy_copy, value));
        SOC_IF_ERROR_RETURN(phymod_phy_intr_status_clear(&pm_phy_copy, *value));
    }

    return(SOC_E_NONE);
}

STATIC int
phy_82764_multi_get(int unit, soc_port_t port, uint32 flags,
                    uint32 dev_addr, uint32 offset, int max_size, uint8 *data,
                    int *actual_size)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    phymod_multi_data_t multi_data; 
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    if (data == NULL) {
        return SOC_E_PARAM;
    }

    pmc = &pc->phymod_ctrl;
    pm_phy = &pmc->phy[pmc->main_phy]->pm_phy;
    multi_data.flags=flags;
    multi_data.dev_addr=dev_addr;
    multi_data.offset=offset;
    multi_data.max_size=max_size;
    multi_data.data=data;
    multi_data.actual_size=(uint32_t *) actual_size;     
    SOC_IF_ERROR_RETURN
        (phymod_phy_multi_get(pm_phy,&multi_data));
    if(actual_size){      
        *actual_size=(*(multi_data.actual_size));
    } 
    return SOC_E_NONE;
}

STATIC int
phy_82764_cl72_enable_set(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;

        if (intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        }
#ifdef PHY82764_DEBUG
        LOG_CLI((BSL_META_U(0,"Tx training: %d port loc:0x%x\n"), value, pm_phy_copy.port_loc));
#endif
        SOC_IF_ERROR_RETURN(phymod_phy_cl72_set(&pm_phy_copy, value));
    }

    return(SOC_E_NONE);
}

STATIC int
phy_82764_cl72_enable_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;

        if (intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        }

        SOC_IF_ERROR_RETURN(phymod_phy_cl72_get(&pm_phy_copy, value));
    }

    return(SOC_E_NONE);
}

STATIC int
phy_82764_cl72_status_get(soc_phymod_ctrl_t *pmc, soc_port_t port, int32 intf, uint32 *value)
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
        pm_phy_copy.port_loc = phymodPortLocLine;

        if (intf == PHY82764_SYS_SIDE) {
            pm_phy_copy.port_loc = phymodPortLocSys;
            SOC_IF_ERROR_RETURN(_phy82764_sys_side_lane_map_get(pmc->unit, port, &pm_phy_copy));
        }

        sal_memset(&status, 0, sizeof(status));
        SOC_IF_ERROR_RETURN(phymod_phy_cl72_status_get(&pm_phy_copy, &status));
        *value = status.locked;
    }

    LOG_CLI((BSL_META_U(0,"Tx training Sts: %d Flags:0x%x\n"), *value, pm_phy_copy.access.flags));

    return(SOC_E_NONE);
}


/*
 * Variable:
 *      phy82764_drv
 * Purpose:
 *      Phy Driver for phy82764 
 */
phy_driver_t phy82764_drv = {
    /* .drv_name                      = */ "PHY82764 PHYMOD PHY Driver",
    /* .pd_init                       = */ phy_82764_init,
    /* .pd_reset                      = */ phy_82764_core_reset,
    /* .pd_link_get                   = */ phy_82764_link_get,
    /* .pd_enable_set                 = */ phy_82764_enable_set,
    /* .pd_enable_get                 = */ phy_82764_enable_get,
    /* .pd_duplex_set                 = */ phy_null_set,
    /* .pd_duplex_get                 = */ phy82764_duplex_get,
    /* .pd_speed_set                  = */ phy82764_speed_set,
    /* .pd_speed_get                  = */ phy82764_speed_get,
    /* .pd_master_set                 = */ phy_null_set,
    /* .pd_master_get                 = */ phy_null_zero_get,
    /* .pd_an_set                     = */ phy_82764_an_set,
    /* .pd_an_get                     = */ phy_82764_an_get,
    /* .pd_adv_local_set              = */ NULL, /* Deprecated */
    /* .pd_adv_local_get              = */ NULL, /* Deprecated */
    /* .pd_adv_remote_get             = */ NULL, /* Deprecated */ 
    /* .pd_lb_set                     = */ phy_82764_lb_set,
    /* .pd_lb_get                     = */ phy82764_lb_get,
    /* .pd_interface_set              = */ phy82764_interface_set,
    /* .pd_interface_get              = */ phy82764_interface_get,
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
    /* .pd_control_set                = */ phy_82764_control_set,
    /* .pd_control_get                = */ phy_82764_control_get,
    /* .pd_reg_read                   = */ phy82764_reg_read,
    /* .pd_reg_write                  = */ phy82764_reg_write,
    /* .pd_reg_modify                 = */ phy82764_reg_modify,
    /* .pd_notify                     = */ NULL,
    /* .pd_probe                      = */ phy82764_probe,
    /* .pd_ability_advert_set         = */ phy_82764_ability_advert_set, 
    /* .pd_ability_advert_get         = */ phy_82764_ability_advert_get,
    /* .pd_ability_remote_get         = */ phy_82764_ability_remote_get,
    /* .pd_ability_local_get          = */ phy_82764_ability_local_get,
    /* .pd_firmware_set               = */ phy_82764_firmware_set,
    /* .pd_timesync_config_set        = */ NULL,
    /* .pd_timesync_config_get        = */ NULL,
    /* .pd_timesync_control_set       = */ NULL,
    /* .pd_timesync_control_set       = */ NULL,
    /* .pd_diag_ctrl                  = */ phy_82764_diag_ctrl,
    /* .pd_lane_control_set           = */ phy_82764_per_lane_control_set,
    /* .pd_lane_control_get           = */ phy_82764_per_lane_control_get,
    /* pd_lane_reg_read */                 NULL,
    /* pd_lane_reg_write */                NULL,
    /* pd_oam_config_set */                NULL,
    /* pd_oam_config_get */                NULL,
    /* pd_oam_control_set */               NULL,
    /* pd_oam_control_get */               NULL,
    /* pd_timesync_enhanced_capture_get */ NULL,
    /* pd_multi_get */                     phy_82764_multi_get,
    /* pd_precondition_before_probe */     NULL,
};

#else
typedef int _phy82764_not_empty; /* Make ISO compilers happy. */
#endif /*  INCLUDE_PHY_82764 */

