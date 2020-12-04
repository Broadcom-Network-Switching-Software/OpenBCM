/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy82109.c
 * Purpose:     Support for Huracan external PHY 
 */

/*
 *   This module implements an NTSW SDK Phy driver for the phy82109 External PHY.
 *  
 *   LAYERING.
 *
 *   This driver is built on top of the PHYMOD library, which is a PHY
 *   driver library that can operate on a family of PHYs, including
 *   phy82109 and Sesto and etc..,  PHYMOD can be built in a standalone enviroment and be
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
 *     APIs (nor ".pd_xx" calls, nor to .phy82109_per_lane_control_set and
 *     .phy82109_per_lane_control_get APIs.
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
 *   xxx_config_t
 *   Driver specific data.  This structure is used by Huracan to hold
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
#if defined(INCLUDE_PHY_82109)
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

#define PHYMOD_HURACAN_SUPPORT


#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>
#include <phymod/phymod_diagnostics.h>

#define NUM_LANES                           4    /* num of lanes per core */
#define MAX_NUM_LANES                       4    /* max num of lanes per port */
#define PHY82109_NO_CFG_VALUE               (-1)
#define PHY82109_DBG_REGACC                 0x2
#define PHY82109_DBG_MEM                    0x3
#define PHY82109_PORT_NO_LANES_4            4
#define PHY82109_PORT_NO_LANES_2            2
#define PHY82109_PORT_NO_LANES_1            1
#define PHY82109_SPD_100G                   100000
#define PHY82109_SPD_10G                    10000
#define PHY82109_SPD_20G                    20000
#define PHY82109_SPD_25G                    25000
#define PHY82109_SPD_40G                    40000
#define PHY82109_SPD_50G                    50000
#define PHY82109_SPD_6G                     6000
#define INTERFACE_SIDE_SHIFT                31
#define HURACAN_ID_82109                    0x82109
#define PHY82109_MAX_LANE                   8
#define PHY82109_CHIP_ID_SIZE               25
typedef struct phy82109_speed_config_s {
    uint32  port_refclk_int;
    int     speed;
    int     port_num_lanes;
    uint32  line_interface;
    int     pll_divider_req  ;
    int     higig_port;
} phy82109_speed_config_t;

typedef enum PHY82109_INTF_SIDE {
    PHY82109_LINE_SIDE,
    PHY82109_SYS_SIDE 
}PHY82109_INTF_SIDE;


#define HURACAN_IS_SIMPLEX(devid)\
((devid) == HURACAN_ID_82109)

#define HURACAN_IS_DUPLEX(devid)\
((devid) != HURACAN_ID_82109)

/*
   Config - per logical port
*/
typedef struct {
    phymod_polarity_t               phy_polarity_config;
    phymod_phy_reset_t              phy_reset_config;
    phy82109_speed_config_t         speed_config;
    int                             an_master_lane;
    int                             ull_datapath;
    uint32_t                             devid;
    char                            dev_name[PHY82109_CHIP_ID_SIZE];
    int                             auto_mod_detect;
    int                             simplex_tx;
    unsigned int                    port_phy_lane_mask;
} phy82109_config_t;

#define DEVID(_pc)           (((phy82109_config_t *)((_pc) + 1))->devid)
#define DEV_NAME(_pc)        (((phy82109_config_t *)((_pc) + 1))->dev_name)
#define SIMPLEX_TX(_pc)   (((phy82109_config_t *)((_pc) + 1))->simplex_tx) 
#define PORT_PHY_LANE_MASK(_pc)   (((phy82109_config_t *)((_pc) + 1))->port_phy_lane_mask) 
/* int is_eye_scan_enabled = 0; */

STATIC int
phy_82109_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability);

STATIC int
phy_82109_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability);
STATIC int
_phy_82109_find_soc_phy_lane(soc_phymod_ctrl_t *pmc, uint32_t lane, 
                        soc_phymod_phy_t **p_phy, uint32 *lane_map);

STATIC int
phy_82109_power_set(phy_ctrl_t *pc, int32 intf, uint32 value);
STATIC int
phy_82109_power_get(phy_ctrl_t *pc, int32 intf, uint32 *value);
STATIC int
phy_82109_tx_lane_squelch_get(phy_ctrl_t *pc, int32 intf, uint32 *value);
STATIC int
phy_82109_per_lane_tx_lane_squelch_get(phy_ctrl_t *pc, int32 intf, int lane, uint32 *value);
STATIC int
_phy_82109_get_intf_side(uint32_t chip_id, int32 intf, uint32 simplex_tx, uint32 tx_rx, uint32_t *if_side);

/*
 * Function:
 *      _phy82109_reg_read
 * Doc:
 *      register read operations
 * Parameters:
 *      unit            - (IN)  unit number
 *      core_addr       - (IN)  core address
 *      reg_addr        - (IN)  address to read
 *      val             - (OUT) read value
 */
STATIC int 
_phy82109_reg_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    uint16 data16;
    int rv;
    soc_phymod_core_t *core = (soc_phymod_core_t *)user_acc;

    rv = core->read(core->unit, core_addr, reg_addr, &data16);
    *val = data16;
    PHYMOD_VDBG(PHY82109_DBG_REGACC, NULL,("-22%s: core_addr: 0x%08x reg_addr: 0x%08x, data: 0x%04x\n", __func__, core_addr, reg_addr, *val ));

    return rv;
}

/*
 * Function:
 *      _phy82109_reg_write
 * Doc:
 *      register write operations
 * Parameters:
 *      unit            - (IN)  unit number
 *      core_addr       - (IN)  core address
 *      reg_addr        - (IN)  address to write
 *      val             - (IN)  write value
 */
STATIC int 
_phy82109_reg_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
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
    PHYMOD_VDBG(PHY82109_DBG_REGACC, NULL,("-22%s: core_addr: 0x%08x reg_addr: 0x%08x, data: 0x%04x\n", __func__, core_addr, reg_addr, val ));
    return core->write(core->unit, core_addr, reg_addr, data16);
}

/*
 * Function:
 *      phy82109_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      speed - link speed in Mbps
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy82109_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t                *int_pc;
    int int_speed;
    /* locate phy control */
    int_pc = INT_PHY_SW_STATE(unit, port);
    if (int_pc == NULL) {
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
    return (SOC_E_NONE);
}


/*
 * Function:
 *      phy82109_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      speed - current link speed in Mbps
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy82109_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t               *int_pc;
    int int_speed;
    
    /* locate phy control */
    int_pc = INT_PHY_SW_STATE(unit, port);
    if (int_pc == NULL) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN
        (PHY_SPEED_GET(int_pc->pd, unit, port, &int_speed));
    *speed = int_speed;
    return (SOC_E_NONE);

}

/*
 * Function:
 *      phy82109_interface_set
 * Purpose:
 *      Set PHY interface type
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      interface -  soc interface type
 * Returns:     
 *      SOC_E_XXX
 */


STATIC int
phy82109_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    phy_ctrl_t      *int_pc;
    int_pc = INT_PHY_SW_STATE(unit,port);
    if (int_pc == NULL) {
        return SOC_E_INTERNAL;
    }
    if (pif >= SOC_PORT_IF_COUNT) {
        return SOC_E_PARAM;
    }
    if (pif == SOC_PORT_IF_MII || pif == SOC_PORT_IF_XGMII || pif == SOC_PORT_IF_GMII) {
		return phy_null_interface_set(unit, port, pif);
	}
    SOC_IF_ERROR_RETURN
        (PHY_INTERFACE_SET(int_pc->pd,unit,port,pif));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy82109_interface_get
 * Purpose:
 *      Get PHY interface type
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      pif - (OUT) interface type
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy82109_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    phy_ctrl_t                *int_pc;
    /* locate phy control */
    int_pc = INT_PHY_SW_STATE(unit,port);
    if (int_pc == NULL) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN
        (PHY_INTERFACE_GET(int_pc->pd,unit,port,pif));
    return (SOC_E_NONE);

}

/*
 * Function:
 *      phy82109_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      unit         - BCM unit number
 *      port         - Port number
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Returns:
 *          SOC_E_XXX
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy82109_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_reg_data)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx = 0;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    idx = pmc->main_phy ;
    pm_phy = &pmc->phy[idx]->pm_phy;
    
    SOC_IF_ERROR_RETURN
        (phymod_phy_reg_read(pm_phy, phy_reg_addr, phy_reg_data));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy82109_reg_write
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - Value write to PHY register
 * Returns:
 *          SOC_E_XXX
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy82109_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_reg_data)
{
    phy_ctrl_t *pc;
    soc_phymod_ctrl_t *pmc;
    phymod_phy_access_t *pm_phy;
    int idx;
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pmc = &pc->phymod_ctrl;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        SOC_IF_ERROR_RETURN
            (phymod_phy_reg_write(pm_phy, phy_reg_addr, phy_reg_data));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy82109_reg_modify
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_mo_data  - New value for the bits specified in phy_mo_mask
 *      phy_mo_mask  - Bit mask to modify
 * Returns:
 *          SOC_E_XXX
 * Note:
 *      This function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy82109_reg_modify(int unit, soc_port_t port, uint32 flags,
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

/*
 * Function:
 *      phy82109_core_init
 * Purpose:
 *         initializes phy mode core object
 * Parameters:
 *      pc - pointer to phy control structure
 *      core - pointer to soc phy mode core object
 *      core_bus - pointer to phy mode bus driver struct
 *      core_addr - address of the phy mode core
 * Returns:
 *          none
 */  
STATIC void
phy82109_core_init(phy_ctrl_t *pc, soc_phymod_core_t *core,
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

    if (soc_property_port_get(pc->unit, pc->port, "phy82109", 0) == 45) {
        PHYMOD_ACC_F_CLAUSE45_SET(pm_acc);
    }

    return;
}
/*
 * Function:
 *      phy82109_cleanup
 * Purpose:
 *         clean up phy device and phy mode core object
 * Parameters:
 *      pmc - pointer to soc phy mode control structure 
 * Returns:
 *          SOC_E_XXX
 */  
STATIC void
phy82109_cleanup(soc_phymod_ctrl_t *pmc)
{
    int idx;
    soc_phymod_phy_t *phy;
    soc_phymod_core_t *core;
    for (idx = 0; idx < pmc->num_phys ; idx++) {
        phy = pmc->phy[idx];
        if (phy == NULL) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(pmc->unit,
                                 "phy object is empty")));
            continue;
        }

        core = phy->core;
        /* Destroy core object if not used anymore */
        if (core && core->ref_cnt) {
            if (--core->ref_cnt == 0) {
                PHYMOD_VDBG(PHY82109_DBG_MEM, NULL,("clean_up core_p=%p\n", (void *)core)) ;
                
                soc_phymod_core_destroy(pmc->unit, core);
            }
        }

        /* Destroy phy object */
        if (phy) {
            PHYMOD_VDBG(PHY82109_DBG_MEM, NULL,("clean_up phy=%p\n", (void *)phy)) ;
            soc_phymod_phy_destroy(pmc->unit, phy);
        }
    }
    pmc->num_phys = 0;
}

/*
 * Function:
 *      _phy82109_chip_id_get
 * Purpose:
 *         read phy chip id
 * Parameters:
 *      pm_phy - pointer to phy mode phy structure 
 *      unit   -  switch unit number 
 *      chip_id -    (OUT) chip id after read
 * Returns:
 *          SOC_E_XXX
 */  
STATIC int32
_phy_82109_chip_id_get(phymod_phy_access_t *pm_phy, uint32 *chip_id)
{
    uint32 chip_id_msb = 0, chip_id_lsb = 0;

    chip_id_msb = (chip_id_msb & 0xF000) >> 12;
 
    SOC_IF_ERROR_RETURN(
       phymod_phy_reg_read(pm_phy, 0x8000, &chip_id_lsb));

    if (chip_id_msb == 0x0) {
        if (chip_id_lsb == 0x2109) {
            *chip_id = 0x82109;
        } 
    } else {
        LOG_WARN(BSL_LS_SOC_PHY, (BSL_META_U(0,
                 "Chip ID not found for \n")));
        return SOC_E_NOT_FOUND;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy82109_probe
 * Purpose:
 *      xx
 * Parameters:
 *      pc->phy_id   (IN)
 *      pc->unit     (IN)
 *      pc->port     (IN)
 *      pc->size     (OUT) - memory required by phy port
 *      pc->dev_name (OUT) - set to port device name
 * Returns:
 *          SOC_E_XXX
 *  
 * Note:
 */
STATIC int
phy82109_probe(int unit, phy_ctrl_t *pc)
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
    int array_max = 8;
    int array_size = 0;
    int port;
    int phy_port;  /* physical port number */
    uint16 chip_id = 0;
    /* Initialize PHY bus */
    SOC_IF_ERROR_RETURN(phymod_bus_t_init(&core_bus));
    core_bus.bus_name = "phy82109"; 
    core_bus.read = _phy82109_reg_read; 
    core_bus.write = _phy82109_reg_write;

    /* Configure PHY bus properties */
    if (pc->wrmask) {
        PHYMOD_BUS_CAP_WR_MODIFY_SET(&core_bus);
        PHYMOD_BUS_CAP_LANE_CTRL_SET(&core_bus);
    }

    LOG_WARN(BSL_LS_SOC_PHY,
         (BSL_META_U(unit,
                     "port %d: probe  82109  enter !!\n"),
          pc->port));

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
    pmc->cleanup = phy82109_cleanup;
    /*pmc->symbols = &bcmi_tscf_xgxs_symbols;*/ 
    
    pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
    pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port));

    /* request memory for the configuration structure */
    pc->size = sizeof(phy82109_config_t);
    /* Read the lane_mask config variable */
    PORT_PHY_LANE_MASK(pc) = soc_property_port_get(unit, port, spn_PORT_PHY_LANE_MASK, 0);

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
    SOC_IF_ERROR_RETURN(
        READ_PHY_REG(unit, pc,0x18000 ,&chip_id));

    if (!PORT_PHY_LANE_MASK(pc)) {
        if (chip_id == 0x2109) {
            lane_map <<= (pc->lane_num | (4 * (pc->chip_num%2)));
        } else {
            lane_map <<= pc->lane_num;
        }
    } else {
        lane_map = PORT_PHY_LANE_MASK(pc);
    }
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

    phy_type = phymodDispatchTypeHuracan;

    /* Probe cores */
    for (idx = 0; idx < num_phys ; idx++) {
        phy82109_core_init(pc, &core_probe, &core_bus,
                           core_info[idx].mdio_addr);
        /* Check that core is indeed an Huracan core */
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
        }
        if (SOC_FAILURE(rv)) {
            break;
        }        
    }
    if (SOC_FAILURE(rv)) {
        phy82109_cleanup(pmc);
        return rv;
    }

    for (idx = 0; idx < num_phys ; idx++) {
        phy = pmc->phy[idx];
        core = phy->core;
        pm_core = &core->pm_core;

        /* Initialize core object if newly created */
        if (core->ref_cnt == 0) {
            sal_memcpy(&core->pm_bus, &core_bus, sizeof(core->pm_bus));
            phy82109_core_init(pc, core, &core->pm_bus,
                               core_info[idx].mdio_addr);
            /* Set dispatch type */
            pm_core->type = phy_type;
        }
        core->port = pc->port;  
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
 *      phy82109_config_init
 * Purpose:     
 *      Determine phy configuration data for purposes of PHYMOD initialization.
 * 
 *      A side effect of this procedure is to save some per-logical port
 *      information in (phy82109_cfg_t *) &pc->driver_data;
 *
 * Parameters:
 *      unit                  - BCM unit number.
 *      port                  - Port number.
 *      logical_lane_offset   - starting logical lane number
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy82109_config_init(int unit, soc_port_t port, int logical_lane_offset,
                 phymod_core_init_config_t *core_init_config, 
                 phymod_phy_init_config_t  *pm_phy_init_config)
{
    phy_ctrl_t *pc;
    phy82109_speed_config_t *speed_config;
    phy82109_config_t *pCfg;
    int port_num_lanes;
    int core_num;
    int phy_num_lanes;

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    pCfg = (phy82109_config_t *) pc->driver_data;

    port_num_lanes = SOC_INFO(unit).port_num_lanes[port];

    /* figure out how many lanes are in this phy */
    core_num = (logical_lane_offset / 4);
    phy_num_lanes = (port_num_lanes - logical_lane_offset);
    if (phy_num_lanes > MAX_NUM_LANES) {
       phy_num_lanes = MAX_NUM_LANES;
    }
    
    SIMPLEX_TX(pc) = soc_property_port_get(unit, port, spn_PORT_PHY_MODE_REVERSE, 0);
    /*CORE configuration*/
    phymod_core_init_config_t_init(core_init_config);
    core_init_config->firmware_load_method  = phymodFirmwareLoadMethodNone;
    core_init_config->lane_map.num_of_lanes = NUM_LANES;
    core_init_config->flags = PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY;

    speed_config = &(pCfg->speed_config);
    speed_config->port_refclk_int  = 156;
    speed_config->port_num_lanes   = phy_num_lanes;
    if (port_num_lanes == 1) {
        if (pc->speed_max == PHY82109_SPD_25G) {
            speed_config->line_interface =  SOC_PORT_IF_LR;
            speed_config->speed = PHY82109_SPD_25G;
        } else {
            speed_config->line_interface =  SOC_PORT_IF_SR;
            speed_config->speed = PHY82109_SPD_10G;
        }
    } else if (port_num_lanes == 4) {
        if (pc->speed_max == PHY82109_SPD_40G) {
           speed_config->speed = PHY82109_SPD_40G;
           speed_config->line_interface =  SOC_PORT_IF_SR4;
        } else {
           speed_config->speed = PHY82109_SPD_100G;
           speed_config->line_interface =  SOC_PORT_IF_LR4;
        }
    } else if (port_num_lanes == 2) {
        if (pc->speed_max == PHY82109_SPD_50G) {
            speed_config->speed = PHY82109_SPD_50G;
            speed_config->line_interface =  SOC_PORT_IF_KR2;
        } else {
            speed_config->speed = PHY82109_SPD_20G;
            speed_config->line_interface =  SOC_PORT_IF_LR;
        }
    } else {
        LOG_CLI((BSL_META_U(unit,"Invalid number of lanes:%d\n "), port_num_lanes));
        return SOC_E_CONFIG;
    }
        LOG_CLI((BSL_META_U(unit,"number of lanes:%d\n "), port_num_lanes));

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

     /* Only do this once, for the first core of the logical port*/
    if (core_num == 0) {
        /* phy_mode, PHYCTRL_MDIO_ADDR_SHARE, PHY_FLAGS_INDEPENDENT_LANE */
        if (port_num_lanes == 4) {
            pc->phy_mode = PHYCTRL_QUAD_LANE_PORT;
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        } else if (port_num_lanes == 2) {
            pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
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

/*
 * Function:
 *      _phy82109_init_pass1
 * Purpose:     
 *      pass1 of multipass phy initialization.
 * 
 * Parameters:
 *      unit                  - BCM unit number.
 *      port                  - Port number.
 * Returns:     
 *      SOC_E_XXX
 */      
STATIC int
_phy_82109_init_pass1(int unit, soc_port_t port)
{
    phy_ctrl_t *pc; 
    soc_phymod_ctrl_t *pmc;
    soc_phymod_phy_t *phy = NULL;
    soc_phymod_core_t *core;
    phy82109_config_t *pCfg;
    phymod_phy_inf_config_t interface_config;
    phymod_core_status_t core_status;
    phymod_core_info_t core_info;
    int idx;
    int logical_lane_offset;
    soc_port_ability_t ability;
    soc_phy_info_t *pi;
    char *dev_name;
    int len = 0;
    uint32 chip_id = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    pc->driver_data = (void*)(pc+1);
    pmc = &pc->phymod_ctrl;
    pCfg = (phy82109_config_t *) pc->driver_data;
    
    sal_memset(pCfg, 0, sizeof(*pCfg));

    sal_memset(&ability, 0, sizeof(ability));
    sal_memset(&interface_config, 0, sizeof(interface_config));

    dev_name = DEV_NAME(pc);
    phy = pmc->phy[0];

    SOC_IF_ERROR_RETURN(
         _phy_82109_chip_id_get(&phy->pm_phy, &chip_id));
    DEVID(pc) = chip_id;

    pi = &SOC_PHY_INFO(unit, port);
    if (DEVID(pc) == HURACAN_ID_82109) { 
        /* coverity[secure_coding] */
        sal_strncpy_s (dev_name, "BCM82109", PHY82109_CHIP_ID_SIZE);
    } else {
        /* coverity[secure_coding] */
        sal_strncpy_s (dev_name, "Huracan", PHY82109_CHIP_ID_SIZE);
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
        /* determine configuration data structure to default values, based on SOC properties */
        SOC_IF_ERROR_RETURN
            (phy82109_config_init(unit, port,
                              logical_lane_offset,
                              &core->init_config, &phy->init_config));
        /* set the port to its max or init_speed */
        interface_config.interface_type = phymodInterfaceXFI;
        interface_config.ref_clock = phymodRefClk106Mhz;
        sal_memcpy(&core->init_config.interface, &interface_config, sizeof(interface_config));
        
        if (!core->init) {
           core_status.pmd_active = 0;
           SOC_IF_ERROR_RETURN
               (phymod_core_init(&core->pm_core, &core->init_config, &core_status));
           core->init = TRUE;
        }
        
        /*phy->init_config.an_link_qualifier*/
        SOC_IF_ERROR_RETURN
            (phymod_phy_init(&phy->pm_phy, &phy->init_config));
       
        /*read serdes id info */
        SOC_IF_ERROR_RETURN
            (phymod_core_info_get(&core->pm_core, &core_info)); 

        /* for multicore phys, need to ratchet up to the next batch of lanes */
        logical_lane_offset += core->init_config.lane_map.num_of_lanes;
    }
	/* indicate second pass of init is needed */
    PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS2);

    return SOC_E_NONE;
}
/*
 * Function:
 *      _phy82109_init_pass2
 * Purpose:     
 *      pass2 of multipass phy initialization.
 * 
 * Parameters:
 *      unit                  - BCM unit number.
 *      port                  - Port number.
 * Returns:     
 *      SOC_E_XXX
 */      
STATIC int
_phy_82109_init_pass2(int unit, soc_port_t port)
{
    phy_ctrl_t *pc; 
    soc_phymod_ctrl_t *pmc;
    soc_phymod_phy_t *phy = NULL;
    phymod_phy_inf_config_t interface_config;
    soc_port_ability_t ability;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }

    sal_memset(&interface_config, 0, sizeof(interface_config));
    sal_memset(&ability, 0, sizeof(ability));
    pc->driver_data = (void*)(pc+1);
    pmc = &pc->phymod_ctrl;
    phy = pmc->phy[0];
    
    /*
     * even though huracan does not has any interface type
     * phymod layer expects a valid interface config, without
     * which core initialization can not happen, so 
     * initializing interface type with valid values for 
     * phymod. Huracan does not make use of this interface
     * settings.
     */
    
    interface_config.interface_type = phymodInterfaceXFI;
    interface_config.ref_clock = phymodRefClk106Mhz;
    SOC_IF_ERROR_RETURN
        (phymod_phy_interface_config_set(&phy->pm_phy,
                             0 /* flags */, &interface_config));
    /* setup the port's an cap */
    SOC_IF_ERROR_RETURN
        (phy_82109_ability_local_get(unit, port, &ability));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy82109_init_pass2: u=%d p=%d\n"), unit, port));

    if (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) {
        /* indicate third  pass of init is needed */
        PHYCTRL_INIT_STATE_SET(pc,PHYCTRL_INIT_STATE_PASS3);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy82109_init_pass3
 * Purpose:     
 *      pass3 of multipass phy initialization.
 * 
 * Parameters:
 *      unit                  - BCM unit number.
 *      port                  - Port number.
 * Returns:     
 *      SOC_E_XXX
 */      
STATIC int
_phy_82109_init_pass3(int unit, soc_port_t port)
{
    phy_ctrl_t *pc; 
    pc = EXT_PHY_SW_STATE(unit, port);

    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
	return SOC_E_NONE;
}

/*
 * Function:
 *      phy82109_init
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
 *      Initialize a phy82109
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_init(int unit, soc_port_t port)
{
    phy_ctrl_t *pc; 

    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS1) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {
        
        PHY_FLAGS_SET(unit, port,  PHY_FLAGS_FIBER | PHY_FLAGS_C45 | PHY_FLAGS_REPEATER);
        SOC_IF_ERROR_RETURN(_phy_82109_init_pass1(unit, port));

        if (PHYCTRL_INIT_STATE(pc) != PHYCTRL_INIT_STATE_DEFAULT) {
            return SOC_E_NONE;
        }
    }
    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS2) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        SOC_IF_ERROR_RETURN( _phy_82109_init_pass2(unit, port));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "82109 init pass2 completed: u=%d p=%d\n"), unit, port));
        if (PHYCTRL_INIT_STATE(pc) != PHYCTRL_INIT_STATE_DEFAULT) {
            return SOC_E_NONE;
        }
    }

    if ((PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_PASS3) ||
        (PHYCTRL_INIT_STATE(pc) == PHYCTRL_INIT_STATE_DEFAULT)) {

        SOC_IF_ERROR_RETURN(_phy_82109_init_pass3(unit, port));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "PHY82109 init pass3 completed: u=%d p=%d\n"), unit, port));
        PHYCTRL_INIT_STATE_SET(pc, PHYCTRL_INIT_STATE_DEFAULT);
        return SOC_E_NONE;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82109_ability_local_get
 * Purpose:
 *         returns soc local port ability
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      ability - (OUT) soc port ability structure
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t          *pc; 
    uint32_t pa_speed = 0;

    pc = EXT_PHY_SW_STATE(unit, port);

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    sal_memset(ability, 0, sizeof(*ability));
    if (pc->speed_max == PHY82109_SPD_100G) {
        pa_speed = SOC_PA_SPEED_100GB | SOC_PA_SPEED_40GB | SOC_PA_SPEED_20GB
            | SOC_PA_SPEED_25GB | SOC_PA_SPEED_50GB | SOC_PA_SPEED_10GB 
            | SOC_PA_SPEED_6000MB | SOC_PA_SPEED_1000MB;
    } else if (pc->speed_max == PHY82109_SPD_50G) {
        pa_speed = SOC_PA_SPEED_50GB | SOC_PA_SPEED_40GB | SOC_PA_SPEED_20GB
            | SOC_PA_SPEED_25GB | SOC_PA_SPEED_10GB | SOC_PA_SPEED_6000MB | SOC_PA_SPEED_1000MB;
    } else if (pc->speed_max == PHY82109_SPD_40G) {
        pa_speed = SOC_PA_SPEED_40GB | SOC_PA_SPEED_20GB
            | SOC_PA_SPEED_25GB | SOC_PA_SPEED_10GB | SOC_PA_SPEED_6000MB | 
            SOC_PA_SPEED_1000MB;
    } else if (pc->speed_max == PHY82109_SPD_25G) {
        pa_speed = SOC_PA_SPEED_20GB | SOC_PA_SPEED_6000MB | SOC_PA_SPEED_1000MB
            | SOC_PA_SPEED_25GB | SOC_PA_SPEED_10GB;
    } else if (pc->speed_max == PHY82109_SPD_20G) {
        pa_speed = SOC_PA_SPEED_20GB | SOC_PA_SPEED_10GB | SOC_PA_SPEED_6000MB | SOC_PA_SPEED_1000MB;
    } else if (pc->speed_max == PHY82109_SPD_10G) {
        pa_speed = SOC_PA_SPEED_10GB | SOC_PA_SPEED_6000MB | SOC_PA_SPEED_1000MB;
    } else {
        pa_speed = SOC_PA_SPEED_100GB | SOC_PA_SPEED_40GB | SOC_PA_SPEED_20GB
            | SOC_PA_SPEED_25GB | SOC_PA_SPEED_50GB | SOC_PA_SPEED_10GB 
            | SOC_PA_SPEED_6000MB | SOC_PA_SPEED_1000MB;
    }

    ability->loopback  = SOC_PA_LB_PHY;
    ability->medium    = SOC_PA_MEDIUM_COPPER;
    ability->pause     = 0;
    ability->flags     = SOC_PA_AUTONEG;
    ability->speed_full_duplex = pa_speed;


    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(pc->unit,
                         "phy82109_ability_local_get:unit=%d p=%d sp=%08x\n"),
              unit, port, ability->speed_full_duplex));
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_82109_link_get
 * Purpose: gets the phy link
 *          state
 *      
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      link - (OUT) link state
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    phymod_phy_access_t       *pm_phy;
    phy_ctrl_t *int_pc;
    int32 int_phy_link = 0;
    uint32 chip_id = 0;
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
    chip_id = DEVID(pc); 
    if (HURACAN_IS_SIMPLEX(chip_id) && (!SIMPLEX_TX(pc))) {
        /* why we need to clear it ???*/
       pm_phy->access.flags &= ~(1 << INTERFACE_SIDE_SHIFT);
       pm_phy->access.flags |= (1 << INTERFACE_SIDE_SHIFT);
    } 
    /* Get the external phy link status if external phy is duplex or simplex package connected in Rx side */ 
    if (HURACAN_IS_DUPLEX(chip_id) || (HURACAN_IS_SIMPLEX(chip_id) && (!SIMPLEX_TX(pc)))) {
        SOC_IF_ERROR_RETURN
            (phymod_phy_link_status_get(pm_phy, (uint32_t *) link));
        /* When PMD lock of 82109 is not Set, Leave the port link as
         * PMD status of 82109. When PMD lock of 82109 is set use
         * PCS status of internal serdes as port link status.
         * When internal serdes is not there use 82109 PMS as link status*/
#ifdef PHY82109_DEBUG    
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
    } else {
        *link = int_phy_link;
    }
    pm_phy->access.flags &= ~(1 << INTERFACE_SIDE_SHIFT);
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82109_enable_set
 * Purpose: set's Phy's power 
 *          state
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - value to be set
 *               (on/off)
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_enable_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t                *pc;
    int32 intf;
    phy_ctrl_t                *int_pc;
    /* locate phy control */
    int_pc = INT_PHY_SW_STATE(unit, port);
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82109_SYS_SIDE : PHY82109_LINE_SIDE; 


    SOC_IF_ERROR_RETURN
        (phy_82109_power_set(pc, intf, enable));
    SOC_IF_ERROR_RETURN(PHY_ENABLE_SET(int_pc->pd, unit, port, enable));

    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
	} else {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82109_enable_get
 * Purpose: read's Phy's power 
 *          state
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - (OUT)returned value
 *               (on/off)
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_enable_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t                *pc;
    int32 intf;
    uint32_t ena_dis;
    *enable = 0;

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82109_SYS_SIDE : PHY82109_LINE_SIDE; 
    SOC_IF_ERROR_RETURN
        (phy_82109_power_get(pc, intf, &ena_dis));
    *enable = ena_dis;
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82109_lb_set
 * Purpose: sets loopback 
 *          
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - (OUT)value to set
 *               (on/off)
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_lb_set(int unit, soc_port_t port, int enable)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82109_firmware_set
 * Purpose:
 *      Sets firmware for phy download. 
 * Parameters:
 *      unit - BCM unit number
 *      port - Port number
 *      array - pointer to firmware c array
 *      offset  - byte offset in the array .
 *      datalen - length of the firmware array 
 * Returns:     
 *      SOC_E_XXX
 */      
STATIC int
phy_82109_firmware_set(int unit, int port, int offset, uint8 *array,int datalen)
{
    return SOC_E_UNAVAIL;
   
}

/*
 * Function:
 *      phy_82109_rx_seq_done_get
 * Purpose:
 *      Reads the rx pmd lock status. 
 * Parameters:
 *      pc  - phy control structure.
 *      value - (OUT) value returned 
 *      after read
 *      intf - interface side
 * Returns:     
 *      SOC_E_XXX
 */      
STATIC int
phy_82109_rx_seq_done_get(phy_ctrl_t *pc, int32 intf, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_phy_access_t pm_phy_copy;
    uint32_t if_side = 0;
    uint32_t chip_id = 0;
    uint32 simplex_tx = 0;
    soc_phymod_ctrl_t *pmc;
    pmc = &pc->phymod_ctrl;


    /* just take the value from the first phy */
    if (pmc->phy[0] == NULL) {
        return SOC_E_INTERNAL;
    }
    pm_phy = &pmc->phy[0]->pm_phy;

    if (pm_phy == NULL) {
         return SOC_E_INTERNAL;
    }

    simplex_tx = SIMPLEX_TX(pc);
    chip_id = DEVID(pc);
    SOC_IF_ERROR_RETURN
        (_phy_82109_get_intf_side(chip_id, intf, simplex_tx, 1, &if_side));

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.flags &= ~(1 << INTERFACE_SIDE_SHIFT); 
    pm_phy_copy.access.flags |= if_side; 

    SOC_IF_ERROR_RETURN(phymod_phy_rx_pmd_locked_get(&pm_phy_copy, value));

    return(SOC_E_NONE);
}
/*
 * Function:
 *      phy_82109_rx_signal_detect_get
 * Purpose:
 *      Reads the rx signal detect status. 
 * Parameters:
 *      intf - interface side
 *      pc  - phy control structure.
 *      value - (OUT) value returned 
 *      after read
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_rx_signal_detect_get(phy_ctrl_t *pc, int32 intf, uint32 *value)
{
   /* $$$ Need to add diagnostic API */
   return(SOC_E_UNAVAIL);
}
/* 
 * phy_82109_rx_ppm_get
 */
STATIC int
phy_82109_rx_ppm_get(phy_ctrl_t *pc, int32 intf, uint32 *value)
{
    /* $$$ Need to add diagnostic API */
    return(SOC_E_UNAVAIL);
}
/*
 * Function:
 *      phy_82109_per_lane_power_get
 * Purpose:
 *      Reads the lanewise power state of phy. 
 * Parameters:
 *      intf - interface side
 *      pc  - phy control structure.
 *      value - (OUT) value returned 
 *      after read
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_per_lane_power_get(phy_ctrl_t *pc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_power_t      power;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    uint32_t if_side = 0;
    uint32_t chip_id = 0;
    uint32 simplex_tx = 0;
    soc_phymod_ctrl_t *pmc;
    pmc = &pc->phymod_ctrl;
    *value = 0;
    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82109_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;

    simplex_tx = SIMPLEX_TX(pc);
    chip_id = DEVID(pc);
    if(HURACAN_IS_SIMPLEX(chip_id)){
        if (!simplex_tx) { /* Rx simplex package */
            if (intf == PHY82109_SYS_SIDE) {
                if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
            } else {
                if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
                if_side |= (1 << INTERFACE_SIDE_SHIFT);
            } 
        } else { /* Tx simplex package */
            if (intf == PHY82109_SYS_SIDE) {
                if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
                if_side |= (1 << INTERFACE_SIDE_SHIFT); 
            } else {
                if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
            } 
        } 
    } else {
        if (intf == PHY82109_SYS_SIDE) {
            if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
            if_side |= (1 << INTERFACE_SIDE_SHIFT); 
        } else {
            if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
        }
    } 
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.access.flags &= ~(1 << INTERFACE_SIDE_SHIFT); 
    pm_phy_copy.access.flags |= if_side; 

    phymod_phy_power_t_init(&power);

    SOC_IF_ERROR_RETURN(phymod_phy_power_get(&pm_phy_copy, &power));

    if(HURACAN_IS_SIMPLEX(chip_id)) {
        if(simplex_tx) {
            if(intf == PHY82109_SYS_SIDE) {
                if(power.rx == phymodPowerOn)
                    *value = 1;
            } else {
                if(power.tx == phymodPowerOn)
                    *value = 1;
            }
        } else {
            if(intf == PHY82109_SYS_SIDE) {
                if(power.tx == phymodPowerOn)
                    *value = 1;
            } else {
                if(power.rx == phymodPowerOn)
                    *value = 1;
            }
        }
    } else {
        if(power.tx == phymodPowerOn && power.rx == phymodPowerOn) {
            *value = 1;
        } else {
            *value = 0;
        }
    }
    return(SOC_E_NONE);
}
/*
 * Function:
 *      phy_82109_power_get
 * Purpose:
 *      Reads the power phy's state. 
 * Parameters:
 *      intf - interface side
 *      pc  - phy control structure.
 *      value - (OUT) value returned 
 *      after read
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_power_get(phy_ctrl_t *pc, int32 intf, uint32 *value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_power_t      power;
    phymod_phy_access_t pm_phy_copy;
    uint32_t if_side = 0;
    uint32_t chip_id = 0;
    uint32 simplex_tx = 0;
    soc_phymod_ctrl_t *pmc;
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


    simplex_tx = SIMPLEX_TX(pc);
    chip_id = DEVID(pc);
    if(HURACAN_IS_SIMPLEX(chip_id)){
        if (!simplex_tx) { /* Rx simplex package */
            if (intf == PHY82109_SYS_SIDE) {
                if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
            } else {
                if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
                if_side |= (1 << INTERFACE_SIDE_SHIFT);
            } 
        } else { /* Tx simplex package */
            if (intf == PHY82109_SYS_SIDE) {
                if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
                if_side |= (1 << INTERFACE_SIDE_SHIFT); 
            } else {
                if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
            } 
        } 
    } else {
        if (intf == PHY82109_SYS_SIDE) {
            if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
            if_side |= (1 << INTERFACE_SIDE_SHIFT); 
        } else {
            if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
        }
    } 

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    phymod_phy_power_t_init(&power);
    pm_phy_copy.access.flags &= ~(1 << INTERFACE_SIDE_SHIFT); 
    pm_phy_copy.access.flags |= if_side; 

    SOC_IF_ERROR_RETURN(phymod_phy_power_get(&pm_phy_copy, &power));
   
    if(HURACAN_IS_SIMPLEX(chip_id)) {
        if(simplex_tx) {
            if(intf == PHY82109_SYS_SIDE) {
                if(power.rx == phymodPowerOn)
                    *value = 1;
            } else {
                if(power.tx == phymodPowerOn)
                    *value = 1;
            }
        } else {
            if(intf == PHY82109_SYS_SIDE) {
                if(power.tx == phymodPowerOn)
                    *value = 1;
            } else {
                if(power.rx == phymodPowerOn)
                    *value = 1;
            }
        }
    } else {
        if(power.tx == phymodPowerOn && power.rx == phymodPowerOn) {
            *value = 1;
        } else {
            *value = 0;
        }
    }
    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_82109_gpio_config_get
 * Purpose:
 *      Reads the gpio line config. 
 * Parameters:
 *      pc  - phy control structure.
 *      value - (OUT) value returned 
 *      after read
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int 
phy_82109_gpio_config_get(phy_ctrl_t *pc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_phy_access_t pm_phy_copy;
    phymod_gpio_mode_t gpio_mode;
    int gpio_pin_no = 0;
    soc_phymod_ctrl_t *pmc;
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

    for (gpio_pin_no = 0; gpio_pin_no < 2; gpio_pin_no++) {
        SOC_IF_ERROR_RETURN(phymod_phy_gpio_config_get(&pm_phy_copy, gpio_pin_no, &gpio_mode));
        *value |= (gpio_mode << (4 * gpio_pin_no));
    }

    return(SOC_E_NONE);
}
/*
 * Function:
 *      phy_82109_gpio_value_get
 * Purpose:
 *      Programmes the gpio pin. 
 * Parameters:
 *      pc  - phy control structure.
 *      value - (OUT)returned value after 
 *      read 
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int 
phy_82109_gpio_value_get(phy_ctrl_t *pc, uint32 *value)
{
    phymod_phy_access_t  *pm_phy;
    phymod_phy_access_t pm_phy_copy;
    int gpio_pin_no = 0;
    int pin_value = 0;
    soc_phymod_ctrl_t *pmc;
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

    for (gpio_pin_no = 0; gpio_pin_no < 2; gpio_pin_no++) {
        SOC_IF_ERROR_RETURN(phymod_phy_gpio_pin_value_get(&pm_phy_copy, gpio_pin_no, &pin_value));
        *value |= (pin_value << (4 * gpio_pin_no));
    }

    return(SOC_E_NONE);
}
/*
 * Function:
 *      phy82109_control_get
 * Purpose:
 *      Get current control settings of the PHY. 
 * Parameters:
 *      unit  - BCM unit number.
 *      port  - Port number. 
 *      type  - Control to update 
 *      value - (OUT) Current setting for the control 
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_control_get(int unit, soc_port_t port, soc_phy_control_t type, uint32 *value)
{
    int                 rv = SOC_E_UNAVAIL;
    phy_ctrl_t          *pc;
    int32 intf;
    *value = 0;
    PHY_CONTROL_TYPE_CHECK(type);

    /* locate phy control */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82109_SYS_SIDE : PHY82109_LINE_SIDE; 

    switch(type) {
    /* RX SIGNAL DETECT */
    case SOC_PHY_CONTROL_RX_SIGNAL_DETECT:
      rv = phy_82109_rx_signal_detect_get(pc, intf, value);
      break;
    case SOC_PHY_CONTROL_RX_SEQ_DONE:
      rv = phy_82109_rx_seq_done_get(pc, intf, value);
      break;
    case SOC_PHY_CONTROL_RX_PPM:
      rv = phy_82109_rx_ppm_get(pc, intf, value);
      break;
    case SOC_PHY_CONTROL_POWER:
       rv = phy_82109_power_get(pc, intf, value);
       break;
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = phy_82109_tx_lane_squelch_get(pc, intf, value);
    break;
    case SOC_PHY_CONTROL_GPIO_CONFIG:
        rv = phy_82109_gpio_config_get(pc, value);
    break;
    case SOC_PHY_CONTROL_GPIO_VALUE:
        rv = phy_82109_gpio_value_get(pc, value);
    break;
    default:
       rv = SOC_E_UNAVAIL;
       break;
    }
    return rv;
}

/*
 * Function:
 *      phy_82109_per_lane_control_get
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      lane  - lane number
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_per_lane_control_get(int unit, soc_port_t port, int lane,
                     soc_phy_control_t type, uint32 *value)
{
    int                 rv = SOC_E_UNAVAIL;
    phy_ctrl_t          *pc;
    int32 intf;
    *value = 0;

    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    PHY_CONTROL_TYPE_CHECK(type);
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82109_SYS_SIDE : PHY82109_LINE_SIDE; 
    
    switch(type) {
    case SOC_PHY_CONTROL_POWER:

       rv = phy_82109_per_lane_power_get(pc, intf, lane, value);
       break;
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = phy_82109_per_lane_tx_lane_squelch_get(pc, intf, lane, value);
        break;

    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}

/*
 * Function:
 *      phy82109_duplex_get
 * Purpose:
 *      Get PHY duplex mode
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      duplex - current duplex mode
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy82109_duplex_get(int unit, soc_port_t port, int *duplex)
{
    *duplex = TRUE;
    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_82109_an_set
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
phy_82109_an_set(int unit, soc_port_t port, int enable)
{
    return (SOC_E_NONE);
}
/*
 * Function:    
 *      phy_82109_an_get
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
phy_82109_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    *an = 0;
    *an_done = 0;
    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_82109_ability_advert_set
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
phy_82109_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_82109_ability_advert_get
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
phy_82109_ability_advert_get(int unit, soc_port_t port,
                              soc_port_ability_t *ability)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy82109_lb_get
 * Purpose:
 *      Get current PHY loopback mode
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy82109_lb_get(int unit, soc_port_t port, int *enable)
{
    *enable = 0;
    return SOC_E_NONE;
}

STATIC int
_phy_82109_get_intf_side(uint32_t chip_id, int32 intf, uint32 simplex_tx, uint32 tx_rx, uint32_t *if_side)
{
    *if_side = 0;
    if (HURACAN_IS_SIMPLEX(chip_id)) {  /* Simplex package */
        if (tx_rx) { /* Rx side */
            if (!simplex_tx) { /* Simplex Rx package */
                if (intf == PHY82109_SYS_SIDE) {
                    LOG_CLI((BSL_META_U(0,"No Rx available at SYS SIDE in Simplex Rx package\n")));
                    return SOC_E_UNAVAIL; 
                } else {
                    *if_side |= (1 << INTERFACE_SIDE_SHIFT);
                } 
            } else { /* Simplex Tx package */
                if (intf == PHY82109_SYS_SIDE) {
                    *if_side |= (1 << INTERFACE_SIDE_SHIFT);
                } else {
                    LOG_CLI((BSL_META_U(0,"No Rx available at LINE SIDE in Simplex Tx package\n")));
                    return SOC_E_UNAVAIL; 
                }
            } 
        } else { /* Tx side */
            if (!simplex_tx) { /* Simplex Rx package */
                if (intf == PHY82109_SYS_SIDE) {
                    *if_side &= ~(1 << INTERFACE_SIDE_SHIFT);
                } else {
                    LOG_CLI((BSL_META_U(0,"No Tx available at LINE SIDE in Simplex Rx package\n")));
                    return SOC_E_UNAVAIL; 
                } 
            } else {  /* Simplex Tx package */
                if (intf == PHY82109_SYS_SIDE) {
                    LOG_CLI((BSL_META_U(0, "No Tx available at SYS SIDE in Simplex Tx package\n")));
                    return SOC_E_UNAVAIL; 
                } else {
                    *if_side &= ~(1 << INTERFACE_SIDE_SHIFT);
                } 
            } 
        } 
    } else { /* Duplex package */
        if (intf == PHY82109_SYS_SIDE) {
            *if_side |= (1 << INTERFACE_SIDE_SHIFT);
        } else {
            *if_side &= ~(1 << INTERFACE_SIDE_SHIFT);
        }
    }
    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_82109_find_soc_phy_lane
 * Purpose:
 *          given a pc (phymod_ctrl_t) and logical lane number, 
 *          find the correct soc_phymod_phy_t object and lane
 * Parameters:
 *      pmc  - soc phy control structure.
 *      lane - lane number
 *      lane_map - lane map
 *      p_phy - (OUT)soc phy mod phy structure
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
_phy_82109_find_soc_phy_lane(soc_phymod_ctrl_t *pmc, uint32_t lane, 
                        soc_phymod_phy_t **p_phy, uint32 *lane_map)
{
    phymod_phy_access_t *pm_phy;
    int idx, lnx, ln_cnt, found;
    uint32 lane_map_copy;
    uint32 chip_id = 0;
    int num_lanes = 0;
    /* Traverse lanes belonging to this port */
    found = 0;
    ln_cnt = 0;
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;
        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        lane_map_copy = pm_phy->access.lane_mask;
        SOC_IF_ERROR_RETURN(
             _phy_82109_chip_id_get(pm_phy, &chip_id));
        if(HURACAN_IS_SIMPLEX(chip_id)){
           num_lanes = 8;
        } else {
           num_lanes = 4;
        }
        for (lnx = 0; lnx < num_lanes; lnx++) {
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
 * Function:
 *      phy_82109_per_lane_tx_lane_squelch
 * Purpose:
 *      sets/clears lanewise tx squelch 
 *      
 * Parameters:
 *      pc  - phy control structure.
 *      intf - interface side
 *      lane - lane number
 *      value - value to set
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_per_lane_tx_lane_squelch(phy_ctrl_t *pc, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_tx_lane_control_t  tx_control;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    uint32_t if_side = 0;
    uint32_t chip_id = 0;
    uint32 simplex_tx = 0;
    soc_phymod_ctrl_t *pmc;
    pmc = &pc->phymod_ctrl;


    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82109_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        simplex_tx = SIMPLEX_TX(pc);
        chip_id = DEVID(pc);
        SOC_IF_ERROR_RETURN
            (_phy_82109_get_intf_side(chip_id, intf, simplex_tx, 0, &if_side));

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.access.lane_mask = lane_map;
        pm_phy_copy.access.flags &= ~(1 << INTERFACE_SIDE_SHIFT); 
        pm_phy_copy.access.flags |= if_side; 

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
 * Function:
 *      phy_82109_per_lane_tx_lane_squelch_get
 * Purpose:
 *      gets lanewise tx squelch status 
 *      
 * Parameters:
 *      pc  - phy control structure.
 *      intf - interface side
 *      lane - lane number
 *      value - (OUT) returned value after read
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_per_lane_tx_lane_squelch_get(phy_ctrl_t *pc, int32 intf, int lane, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_tx_lane_control_t  tx_control;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    uint32              lane_map;
    uint32_t if_side = 0;
    uint32_t chip_id = 0;
    uint32 simplex_tx = 0;
    soc_phymod_ctrl_t *pmc;
    pmc = &pc->phymod_ctrl;


    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82109_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        simplex_tx = SIMPLEX_TX(pc);
        chip_id = DEVID(pc);
        SOC_IF_ERROR_RETURN
            (_phy_82109_get_intf_side(chip_id, intf, simplex_tx, 0, &if_side));

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.access.lane_mask = lane_map;
        pm_phy_copy.access.flags &= ~(1 << INTERFACE_SIDE_SHIFT); 
        pm_phy_copy.access.flags |= if_side; 

        tx_control = phymodTxSquelchOn;
        SOC_IF_ERROR_RETURN
            (phymod_phy_tx_lane_control_get(&pm_phy_copy, &tx_control));
        *value = tx_control;
    }
    return(SOC_E_NONE);
} 

/*
 * Function:
 *      phy_82109_tx_lane_squelch
 * Purpose:
 *      sets/clear's tx lane squelch 
 *      
 * Parameters:
 *      pc  - phy control structure.
 *      intf - interface side
 *      lane - 
 *      value - value to be programmed (on/off)
 * Returns:     
 *      SOC_E_XXX
 */      
STATIC int
phy_82109_tx_lane_squelch(phy_ctrl_t *pc, int32 intf, uint32 value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_tx_lane_control_t  tx_control;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;
    uint32_t if_side = 0;
    uint32_t chip_id = 0;
    uint32 simplex_tx = 0;
    soc_phymod_ctrl_t *pmc;
    pmc = &pc->phymod_ctrl;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        simplex_tx = SIMPLEX_TX(pc);
        chip_id = DEVID(pc);
        SOC_IF_ERROR_RETURN
            (_phy_82109_get_intf_side(chip_id, intf, simplex_tx, 0, &if_side));
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.access.flags &= ~(1 << INTERFACE_SIDE_SHIFT); 
        pm_phy_copy.access.flags |= if_side; 

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
 * Function:
 *      phy_82109_per_lane_power_set
 * Purpose:
 *      Sets the lanewise interface side 
 *      power state. 
 * Parameters:
 *      pc  - phy control structure.
 *      intf - interface side
 *      lane - lane number
 *      value - value to be programmed (on/off)
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_tx_lane_squelch_get(phy_ctrl_t *pc, int32 intf, uint32 *value)
{
    phymod_phy_access_t *pm_phy;
    phymod_phy_tx_lane_control_t  tx_control;
    int                 idx;
    phymod_phy_access_t pm_phy_copy;
    uint32_t if_side = 0;
    uint32_t chip_id = 0;
    uint32 simplex_tx = 0;
    soc_phymod_ctrl_t *pmc;
    pmc = &pc->phymod_ctrl;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }
        simplex_tx = SIMPLEX_TX(pc);
        chip_id = DEVID(pc);
        SOC_IF_ERROR_RETURN
            (_phy_82109_get_intf_side(chip_id, intf, simplex_tx, 0, &if_side));
        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.access.flags &= ~(1 << INTERFACE_SIDE_SHIFT); 
        pm_phy_copy.access.flags |= if_side; 
        tx_control = phymodTxSquelchOn;
        SOC_IF_ERROR_RETURN
            (phymod_phy_tx_lane_control_get(&pm_phy_copy, &tx_control));
        *value = tx_control;
    }
    return(SOC_E_NONE);
}
/*
 * Function:
 *      phy_82109_per_lane_power_set
 * Purpose:
 *      Sets the lanewise interface side 
 *      power state. 
 * Parameters:
 *      pc  - phy control structure.
 *      intf - interface side
 *      lane - lane number
 *      value - value to be programmed (on/off)
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_per_lane_power_set(phy_ctrl_t *pc, int32 intf, int lane, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_power_t      power;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    uint32_t if_side = 0;
    uint32_t chip_id = 0;
    uint32 simplex_tx = 0;
    soc_phymod_ctrl_t *pmc;
    pmc = &pc->phymod_ctrl;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82109_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;

    simplex_tx = SIMPLEX_TX(pc);
    chip_id = DEVID(pc);

    phymod_phy_power_t_init(&power);
    if(HURACAN_IS_SIMPLEX(chip_id)){
        if (!simplex_tx) { /* Rx simplex package */
            if (intf == PHY82109_SYS_SIDE) {
               if (value) {
                   power.tx = phymodPowerOn;
                   power.rx = phymodPowerOn;
               } else {
                   power.tx = phymodPowerOff;
                   power.rx = phymodPowerOn;
               }
               if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
            } else {
                if (value) {
                   power.rx = phymodPowerOn;
                   power.tx = phymodPowerOn;
                } else {
                   power.rx = phymodPowerOff;
                   power.tx = phymodPowerOn;
                }
                if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
                if_side |= (1 << INTERFACE_SIDE_SHIFT); 
            } 
        } else { /* Tx simplex package */
            if (intf == PHY82109_SYS_SIDE) {
               if (value) {
                   power.rx = phymodPowerOn;
                   power.tx = phymodPowerOn;
               } else {
                   power.rx = phymodPowerOff;
                   power.tx = phymodPowerOn;
               }
               if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
               if_side |= (1 << INTERFACE_SIDE_SHIFT); 
            } else {
                if (value) {
                   power.tx = phymodPowerOn;
                   power.rx = phymodPowerOn;
                } else {
                   power.tx = phymodPowerOff;
                   power.rx = phymodPowerOn;
                }
                if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
            } 
        } 
    } else {
        if (value) {
            power.tx = phymodPowerOn;
            power.rx = phymodPowerOn;
        } else {
            power.tx = phymodPowerOff;
            power.rx = phymodPowerOff;
        }
        if (intf == PHY82109_SYS_SIDE) {
           if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
           if_side |= (1 << INTERFACE_SIDE_SHIFT); 
        } else {
           if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
        }
    } 

    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.access.flags &= ~(1 << INTERFACE_SIDE_SHIFT); 
    pm_phy_copy.access.flags |= if_side; 

    SOC_IF_ERROR_RETURN(phymod_phy_power_set(&pm_phy_copy, &power));

    return(SOC_E_NONE);
}
/*
 * Function:
 *      phy_82109_power_set
 * Purpose:
 *      Sets the interface side power state. 
 * Parameters:
 *      pc  - phy control structure.
 *      intf - interface side
 *      value - value to be programmed (on/off)
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_power_set(phy_ctrl_t *pc, int32 intf, uint32 value)
{
    phymod_phy_access_t     *pm_phy;
    phymod_phy_power_t      power;
    int                     idx;
    phymod_phy_access_t pm_phy_copy;
    uint32_t if_side = 0;
    uint32_t chip_id = 0;
    uint32 simplex_tx = 0;
    soc_phymod_ctrl_t *pmc;
    pmc = &pc->phymod_ctrl;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        simplex_tx = SIMPLEX_TX(pc);
        chip_id = DEVID(pc);

        phymod_phy_power_t_init(&power);
        if(HURACAN_IS_SIMPLEX(chip_id)){
            if (!simplex_tx) { /* Rx simplex package */
                if (intf == PHY82109_SYS_SIDE) {
                    if (value) {
                        power.tx = phymodPowerOn;
                    } else {
                        power.tx = phymodPowerOff;
                    }
                    power.rx = phymodPowerNoChange;
                    if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
                } else {
                    if (value) {
                        power.rx = phymodPowerOn;
                    } else {
                        power.rx = phymodPowerOff;
                    }
                    power.tx = phymodPowerNoChange;
                    if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
                    if_side |= (1 << INTERFACE_SIDE_SHIFT); 
                } 
            } else { /* Tx simplex package */
                if (intf == PHY82109_SYS_SIDE) {
                    if (value) {
                        power.rx = phymodPowerOn;
                    } else {
                        power.rx = phymodPowerOff;
                    }
                    power.tx = phymodPowerNoChange;
                    if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
                    if_side |= (1 << INTERFACE_SIDE_SHIFT); 
                } else {
                    if (value) {
                        power.tx = phymodPowerOn;
                    } else {
                        power.tx = phymodPowerOff;
                    }
                    power.rx = phymodPowerNoChange;
                    if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
                } 
            } 
        } else {
            if (value) {
                power.tx = phymodPowerOn;
                power.rx = phymodPowerOn;
            } else {
                power.tx = phymodPowerOff;
                power.rx = phymodPowerOff;
            }
            if (intf == PHY82109_SYS_SIDE) {
                if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
                if_side |= (1 << INTERFACE_SIDE_SHIFT); 
            } else {
                if_side &= ~(1 << INTERFACE_SIDE_SHIFT); 
            }
        } 

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
        pm_phy_copy.access.flags &= ~(1 << INTERFACE_SIDE_SHIFT); 
        pm_phy_copy.access.flags |= if_side; 
        /*sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));*/
 
        SOC_IF_ERROR_RETURN(phymod_phy_power_set(&pm_phy_copy, &power));
    }

    return(SOC_E_NONE);
}
/*
 * Function:
 *      phy_82109_phy_dump
 * Purpose:
 *      Reads the phy registers. 
 * Parameters:
 *      pc  - phy control structure.
 *      intf - interface side
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_phy_dump(phy_ctrl_t *pc, int32 intf)
{
    phymod_phy_access_t     *pm_phy;
    int                     idx;
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
        pm_phy_copy.access.flags &= ~(1 << INTERFACE_SIDE_SHIFT); 
        if(intf == PHY82109_SYS_SIDE) {
            pm_phy_copy.access.flags |= (1 << INTERFACE_SIDE_SHIFT); 
        }
        SOC_IF_ERROR_RETURN(phymod_phy_status_dump(&pm_phy_copy));

    }

    return(SOC_E_NONE);
}
/*
 * Function:
 *      phy_82109_per_lane_phy_dump
 * Purpose:
 *      Reads the lanewise phy registers. 
 * Parameters:
 *      pc  - phy control structure.
 *      lane - lane number 
 *      intf - interface side
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_per_lane_phy_dump(phy_ctrl_t *pc, int32 intf, int lane)
{
    phymod_phy_access_t     *pm_phy;
    uint32              lane_map;
    phymod_phy_access_t pm_phy_copy;
    soc_phymod_phy_t    *p_phy;
    soc_phymod_ctrl_t *pmc;
    pmc = &pc->phymod_ctrl;

    /* locate the desired phy and lane */
    SOC_IF_ERROR_RETURN(_phy_82109_find_soc_phy_lane(pmc, lane, &p_phy, &lane_map));

    /* Make a copy of the phy access and overwrite the desired lane */
    pm_phy = &p_phy->pm_phy;
    sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
    pm_phy_copy.access.lane_mask = lane_map;
    pm_phy_copy.access.flags &= ~(1 << INTERFACE_SIDE_SHIFT); 
    if(intf == PHY82109_SYS_SIDE) {
        pm_phy_copy.access.flags |= (1 << INTERFACE_SIDE_SHIFT); 
    } 

    SOC_IF_ERROR_RETURN(phymod_phy_status_dump(&pm_phy_copy));
    return(SOC_E_NONE);
}
/*
 * Function:
 *      phy_82109_gpio_config_set
 * Purpose:
 *      Configures the gpio line. 
 * Parameters:
 *      pc  - phy control structure.
 *      value - value to be programmed 
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_gpio_config_set(phy_ctrl_t *pc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    int                  idx;
    phymod_phy_access_t pm_phy_copy;
    int gpio_mode = 0;
    int gpio_pin_no = 0;
    soc_phymod_ctrl_t *pmc;
    pmc = &pc->phymod_ctrl;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));
 
        for (gpio_pin_no = 0; gpio_pin_no < 2; gpio_pin_no++) {
            gpio_mode = (value >> (4 * gpio_pin_no)) & 0xF;
            SOC_IF_ERROR_RETURN(phymod_phy_gpio_config_set(&pm_phy_copy, gpio_pin_no, (phymod_gpio_mode_t)gpio_mode));
        }
    }

    return(SOC_E_NONE);
}

/*
 * Function:
 *      phy_82109_gpio_value_set
 * Purpose:
 *      Programmes the gpio pin. 
 * Parameters:
 *      pc  - phy control structure.
 *      value - value to be programmed 
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_gpio_value_set(phy_ctrl_t *pc, uint32 value)
{
    phymod_phy_access_t  *pm_phy;
    int                  idx;
    phymod_phy_access_t pm_phy_copy;
    int pin_value = 0;
    int gpio_pin_no = 0;
    soc_phymod_ctrl_t *pmc;
    pmc = &pc->phymod_ctrl;

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pm_phy = &pmc->phy[idx]->pm_phy;

        if (pm_phy == NULL) {
            return SOC_E_INTERNAL;
        }

        sal_memcpy(&pm_phy_copy, pm_phy, sizeof(pm_phy_copy));

        for (gpio_pin_no = 0; gpio_pin_no < 2; gpio_pin_no++) {
            pin_value = (value >> (4 * gpio_pin_no)) & 0xF;
            SOC_IF_ERROR_RETURN(phymod_phy_gpio_pin_value_set(&pm_phy_copy, gpio_pin_no, pin_value));
        }
    }

    return(SOC_E_NONE);
}
/*
 * Function:
 *      phy_82109_control_set
 * Purpose:
 *      Configure PHY device specific control function. 
 * Parameters:
 *      unit  - BCM unit number.
 *      port  - Port number. 
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_control_set(int unit, soc_port_t port, soc_phy_control_t type, uint32 value)
{
    int                 rv;
    phy_ctrl_t          *pc;
    int32 intf;
    rv = SOC_E_UNAVAIL;
    PHY_CONTROL_TYPE_CHECK(type);
    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82109_SYS_SIDE : PHY82109_LINE_SIDE;
    switch(type) {
    /* TX LANE SQUELCH */
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = phy_82109_tx_lane_squelch(pc, intf, value);
        break;
    /* POWER */
    case SOC_PHY_CONTROL_POWER:
       {
           phy_ctrl_t                *int_pc;
           int_pc = INT_PHY_SW_STATE(unit, port);
           SOC_IF_ERROR_RETURN(PHY_ENABLE_SET(int_pc->pd, unit, port, value));
           rv = phy_82109_power_set(pc, intf, value);
       }
       break;
    case SOC_PHY_CONTROL_DUMP:
        rv = phy_82109_phy_dump(pc, intf);
    break;
    case SOC_PHY_CONTROL_GPIO_CONFIG:
        rv = phy_82109_gpio_config_set(pc, value);
    break;
    case SOC_PHY_CONTROL_GPIO_VALUE:
        rv = phy_82109_gpio_value_set(pc, value);
    break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}
/*
 * Function:
 *      phy_82109_per_lane_control_set
 * Purpose:
 *      Configure PHY device specific control function. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      lane  - lane number
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
phy_82109_per_lane_control_set(int unit, soc_port_t port, int lane, soc_phy_control_t type, uint32 value)
{
    int                 rv = SOC_E_UNAVAIL;
    phy_ctrl_t          *pc;
    int32 intf;
    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    PHY_CONTROL_TYPE_CHECK(type);

    intf = (pc->flags & PHYCTRL_SYS_SIDE_CTRL) ? PHY82109_SYS_SIDE : PHY82109_LINE_SIDE; 

    switch(type) {
    /* TX LANE SQUELCH */
    case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
        rv = phy_82109_per_lane_tx_lane_squelch(pc, intf, lane, value);
        break;
    /*POWER */
    case SOC_PHY_CONTROL_POWER:
       rv = phy_82109_per_lane_power_set(pc, intf, lane, value);
       break;
    case SOC_PHY_CONTROL_DUMP:
        rv = phy_82109_per_lane_phy_dump(pc, intf, lane);
    break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }

    return rv;
}
/*
 * Function:
 *      phy_82109_diag_ctrl
 * Purpose:
 *      xx
 * Parameters:
 *      unit - BCM unit number.
 *      port - Port number. 

 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_82109_diag_ctrl(
    int unit,        /* unit */
    soc_port_t port, /* port */
    uint32 inst,     /* the specific device block the control action directs to */
    int op_type,     /* operation types: read,write or command sequence */
    int op_cmd,      /* command code */
    void *arg)       /* command argument based on op_type/op_cmd */
{
    int rv = -1;
    phy_ctrl_t          *pc;
    /* locate phy control, phymod control, and the configuration data */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return SOC_E_INTERNAL;
    }
    switch(op_cmd) {
        case PHY_DIAG_CTRL_DSC:
        case PHY_DIAG_CTRL_LINKMON_MODE:
        case PHY_DIAG_CTRL_LINKMON_STATUS:
            rv = SOC_E_UNAVAIL;
            break;
        default:
            if (op_type == PHY_DIAG_CTRL_SET) {
                    rv = phy_82109_control_set(unit,port,op_cmd,PTR_TO_INT(arg));
            } else if (op_type == PHY_DIAG_CTRL_GET) {
                    rv = phy_82109_control_get(unit,port,op_cmd,(uint32 *)arg);
            }
            break ;
        }
    return rv;
}

/*
* Function:
*      _phy_82109_core_reset
* Purpose:
*      Reset's the phy core.
* Parameters:
*      unit - StrataSwitch unit #.
*      port - StrataSwitch port #.
*      not_used - don't care.
* Returns:
*      SOC_E_XXX
*/
int32 phy_82109_core_reset(int32 unit, soc_port_t port, void *not_used)
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
    reset_direction = phymodResetDirectionIn;
    pmc = &pc->phymod_ctrl; 

    /* loop through all cores */
    for (idx = 0; idx < pmc->num_phys; idx++) {
        pmc_core = pmc->phy[idx]->core;
        pm_core = &pmc_core->pm_core;
        if (pm_core == NULL) {
            return SOC_E_INTERNAL;
        }
        /* Soft reset is not supported in Huracan */
        SOC_IF_ERROR_RETURN(phymod_core_reset_set(pm_core, phymodResetModeHard, reset_direction));
    }
    return(SOC_E_NONE);
}
/*
* Function:
*      _phy_82109_ability_remote_get
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
phy_82109_ability_remote_get(int unit, soc_port_t port, 
                                  soc_port_ability_t *ability)
{
    return SOC_E_NONE;
}
/*
 * Variable:
 *      phy82109_drv
 * Purpose:
 *      Phy Driver for phy82109 
 */
phy_driver_t phy82109_drv = {
    /* .drv_name                      = */ "PHY82109 PHYMOD PHY Driver",
    /* .pd_init                       = */ phy_82109_init,
    /* .pd_reset                      = */ phy_82109_core_reset,
    /* .pd_link_get                   = */ phy_82109_link_get,
    /* .pd_enable_set                 = */ phy_82109_enable_set,
    /* .pd_enable_get                 = */ phy_82109_enable_get,
    /* .pd_duplex_set                 = */ phy_null_set,
    /* .pd_duplex_get                 = */ phy82109_duplex_get,
    /* .pd_speed_set                  = */ phy82109_speed_set,
    /* .pd_speed_get                  = */ phy82109_speed_get,
    /* .pd_master_set                 = */ phy_null_set,
    /* .pd_master_get                 = */ phy_null_zero_get,
    /* .pd_an_set                     = */ phy_82109_an_set,
    /* .pd_an_get                     = */ phy_82109_an_get,

    /* .pd_adv_local_set              = */ NULL, /* Deprecated */
    /* .pd_adv_local_get              = */ NULL, /* Deprecated */
    /* .pd_adv_remote_get             = */ NULL, /* Deprecated */ 
    /* .pd_lb_set                     = */ phy_82109_lb_set,
    /* .pd_lb_get                     = */ phy82109_lb_get,
    /* .pd_interface_set              = */ phy82109_interface_set,
    /* .pd_interface_get              = */ phy82109_interface_get,
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
    /* .pd_control_set                = */ phy_82109_control_set,
    /* .pd_control_get                = */ phy_82109_control_get,
    /* .pd_reg_read                   = */ phy82109_reg_read,
    /* .pd_reg_write                  = */ phy82109_reg_write,
    /* .pd_reg_modify                 = */ phy82109_reg_modify,
    /* .pd_notify                     = */ NULL,
    /* .pd_probe                      = */ phy82109_probe,
    /* .pd_ability_advert_set         = */ phy_82109_ability_advert_set, 
    /* .pd_ability_advert_get         = */ phy_82109_ability_advert_get,
    /* .pd_ability_remote_get         = */ phy_82109_ability_remote_get,
    /* .pd_ability_local_get          = */ phy_82109_ability_local_get,
    /* .pd_firmware_set               = */ phy_82109_firmware_set,
    /* .pd_timesync_config_set        = */ NULL,
    /* .pd_timesync_config_get        = */ NULL,
    /* .pd_timesync_control_set       = */ NULL,
    /* .pd_timesync_control_set       = */ NULL,
    /* .pd_diag_ctrl                  = */ phy_82109_diag_ctrl,
    /* .pd_lane_control_set           = */ phy_82109_per_lane_control_set,    
    /* .pd_lane_control_get           = */ phy_82109_per_lane_control_get 
};

#else
typedef int _phy82109_not_empty; /* Make ISO compilers happy. */

#endif /*  INCLUDE_PHY_82109 */

