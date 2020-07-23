/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * portctrl soc routines
 */

#include <sal/core/libc.h>

#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/esw/portctrl.h>
#include <soc/esw/portctrl_internal.h>
#include <soc/greyhound.h>
#include <shared/bsl.h>
#include <soc/phy/phymod_sim.h>
#include <soc/portmod/portmod.h>

#ifdef BCM_GREYHOUND_SUPPORT
#define SOC_GH_PM4X10_COUNT         6
#define SOC_GH_PM4X10_PORT_0        02
#define SOC_GH_PM4X10_PORT_1        18
#define SOC_GH_PM4X10_PORT_2        22
#define SOC_GH_PM4X10_PORT_3        26
#define SOC_GH_PM4X10_PORT_4        30
#define SOC_GH_PM4X10_PORT_5        34

static uint32_t _gh_addr[SOC_GH_PM4X10_COUNT] = {
    0x81, 0xA1, 0xA5, 0xA9, 0xAD, 0xB1};

static uint32_t _gh_port[SOC_GH_PM4X10_COUNT] = {
    SOC_GH_PM4X10_PORT_0,
    SOC_GH_PM4X10_PORT_1,
    SOC_GH_PM4X10_PORT_2,
    SOC_GH_PM4X10_PORT_3,
    SOC_GH_PM4X10_PORT_4,
    SOC_GH_PM4X10_PORT_5,
};

portmod_pm_instances_t gh_pm_types[] = {
#ifdef PORTMOD_PM4X10_SUPPORT
    {portmodDispatchTypePm4x10,  SOC_GH_PM4X10_COUNT},
#endif
};

#define SOC_GH_MAX_PHYS              160   /* 128 XLMACS, 32 CLMAC's */

#ifdef PORTMOD_PM4X10_SUPPORT
static portmod_default_user_access_t *pm4x10_gh_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM4X10_SUPPORT */

soc_portctrl_functions_t soc_gh_portctrl_func = {
    soc_gh_portctrl_pm_portmod_init,
    soc_gh_portctrl_pm_portmod_deinit,
    NULL, /*soc_portctrl_pm_port_config_get*/
    NULL, /*soc_portctrl_pm_port_phyaddr_get*/
    NULL, /*soc_portctrl_port_ability_update*/
    NULL, /*soc_portctrl_pm_type_get*/
    NULL,
    NULL
};

STATIC int
_soc_gh_portctrl_pm_init(int unit, int *max_phys,
                         portmod_pm_instances_t **pm_types,
                         int *pms_arr_len)
{
    *max_phys = SOC_GH_MAX_PHYS;
    *pm_types = gh_pm_types;
    *pms_arr_len = sizeof(gh_pm_types)/sizeof(portmod_pm_instances_t);
    return SOC_E_NONE;
}

STATIC int
_soc_gh_portctrl_device_addr_port_get(int unit, int pm_type,
                                      uint32_t **addr,
                                      uint32_t **port,
                                      uint32_t **core_num)
{
    *addr = _gh_addr;
    *port = _gh_port;
    *core_num = NULL;
    return SOC_E_NONE;
}

STATIC int
_soc_gh_portctrl_pm_user_acc_flag_set(int unit,
                    portmod_default_user_access_t* pm_user_access)
{
    PORTMOD_USER_ACCESS_REG_VAL_OFFSET_ZERO_SET((pm_user_access));
    return SOC_E_NONE;
}

STATIC int
_soc_gh_portctrl_pm4x10_portmod_init(int unit, int num_of_instances) 
{
#ifdef PORTMOD_PM4X10_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm_info;
    int pmid = 0, blk, is_sim, lane, phy, found, logical_port;
    int first_port, idx, ref_clk_prop = 0, core_num, core_cnt;
    uint32 txlane_map, rxlane_map, *pAddr, *pPort, *pCoreNum;
    uint32 tx_polarity, rx_polarity;

    SOC_IF_ERROR_RETURN
        (_soc_gh_portctrl_device_addr_port_get(unit,
                                               portmodDispatchTypePm4x10,
                                            &pAddr, &pPort, &pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* Allocate PMs */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                           num_of_instances,
                                               &pm4x10_gh_user_acc[unit]));

    for (pmid = 0; PORTMOD_SUCCESS(rv) && (pmid < num_of_instances); pmid++) {

        rv = portmod_pm_create_info_t_init(unit, &pm_info);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }
        /* PM info */
        first_port = pPort[pmid];

        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+1);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+2);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+3);

        found = 0;
        for(idx = 0; idx < SOC_DRIVER(unit)->port_num_blktype; idx++){
            blk = SOC_PORT_IDX_INFO(unit, first_port, idx).blk;
            if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_XLPORT){
                found = 1;
                break;
            }
        }

        if (!found) {
            rv = SOC_E_INTERNAL;
            break;
        } 

        pm_info.type         = portmodDispatchTypePm4x10;

        /* Init user_acc for phymod access struct */
        rv = portmod_default_user_access_t_init(unit,
                                                &(pm4x10_gh_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        SOC_IF_ERROR_RETURN(
            _soc_gh_portctrl_pm_user_acc_flag_set(unit,
                &(pm4x10_gh_user_acc[unit][pmid])));

        pm4x10_gh_user_acc[unit][pmid].unit = unit;
        pm4x10_gh_user_acc[unit][pmid].blk_id = blk; 
        pm4x10_gh_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");
        if (pm4x10_gh_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        /* Specific info for PM4x10 */
        rv = phymod_access_t_init(&pm_info.pm_specific_info.pm4x10.access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_info.pm_specific_info.pm4x10.access.user_acc =
            &(pm4x10_gh_user_acc[unit][pmid]);
        pm_info.pm_specific_info.pm4x10.access.addr     = pAddr[pmid];
        pm_info.pm_specific_info.pm4x10.access.bus      = NULL; /* Use default bus */
#if !defined (EXCLUDE_PHYMOD_TSCE_SIM) && defined (PHYMOD_TSCE_SUPPORT)
        rv = soc_physim_check_sim(unit, phymodDispatchTypeTsce,
                                  &(pm_info.pm_specific_info.pm4x10.access),
                                  0, &is_sim);
#else
        is_sim = 0;
#endif
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        if (is_sim) {
            pm_info.pm_specific_info.pm4x10.access.bus->bus_capabilities |=
                (PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL);

            /* Firmward loader : Don't allow FW load on sim enviroment */
            pm_info.pm_specific_info.pm4x10.fw_load_method =
                phymodFirmwareLoadMethodNone;
        } else {
            pm_info.pm_specific_info.pm4x10.fw_load_method =
                soc_property_suffix_num_get(unit, pmid, 
                                            spn_LOAD_FIRMWARE, "quad",
                                            phymodFirmwareLoadMethodExternal);
            pm_info.pm_specific_info.pm4x10.fw_load_method &= 0xff;
        }

        /* Use default external loader if NULL */
        pm_info.pm_specific_info.pm4x10.external_fw_loader = NULL;

       rv = phymod_polarity_t_init
            (&(pm_info.pm_specific_info.pm4x10.polarity));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        /* Polarity */
        for (lane = 0; lane < SOC_PM4X10_NUM_LANES; lane++) {
            /* need to add more conditions */
            phy = first_port+lane;
            rx_polarity =  soc_property_suffix_num_get(unit, phy,
                            spn_PHY_RX_POLARITY_FLIP, "phy", 0xFFFFFFFF);
            tx_polarity = soc_property_suffix_num_get(unit, phy,
                            spn_PHY_TX_POLARITY_FLIP, "phy", 0xFFFFFFFF); 
                               
            logical_port = SOC_INFO(unit).port_p2l_mapping[phy];

            if (rx_polarity  == 0xFFFFFFFF) {
                rx_polarity = soc_property_port_get(unit, logical_port,
                                           spn_PHY_RX_POLARITY_FLIP, 0);
                pm_info.pm_specific_info.pm4x10.polarity.rx_polarity
                            |= rx_polarity;
            } else {
                pm_info.pm_specific_info.pm4x10.polarity.rx_polarity
                            |= ((rx_polarity & 0x1) << lane);
            }             

            if (tx_polarity  == 0xFFFFFFFF) {
                tx_polarity = soc_property_port_get(
                                           unit, logical_port,
                                           spn_PHY_TX_POLARITY_FLIP, 0);
                pm_info.pm_specific_info.pm4x10.polarity.tx_polarity
                            |= tx_polarity;
            } else {
                pm_info.pm_specific_info.pm4x10.polarity.tx_polarity
                            |= ((tx_polarity & 0x1) << lane);
            }
        }

        /* Lane map */
        rv = phymod_lane_map_t_init
            (&(pm_info.pm_specific_info.pm4x10.lane_map));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        logical_port = SOC_INFO(unit).port_p2l_mapping[first_port];
        core_cnt = 1; 

        for (core_num = 0; core_num < core_cnt; core_num++) {
            txlane_map = soc_property_port_suffix_num_get(unit, logical_port,
                                                     core_num,
                                                     spn_XGXS_TX_LANE_MAP,
                                                     "core", 0x3210);
            rxlane_map = soc_property_port_suffix_num_get(unit, logical_port,
                                                     core_num,
                                                     spn_XGXS_RX_LANE_MAP,
                                                     "core", 0x3210);

#ifdef _PORTMOD_VERBOSE_
            soc_esw_portctrl_dump_txrx_lane_map(unit, first_port, logical_port,
                                core_num, txlane_map, rxlane_map,
                                txlane_map, rxlane_map);
#endif /* _PORTMOD_VERBOSE_ */
            pm_info.pm_specific_info.pm4x10.lane_map.num_of_lanes =
                SOC_PM4X10_NUM_LANES;
    
            pm_info.pm_specific_info.pm4x10.core_num = pCoreNum? pCoreNum[pmid] : pmid;
            pm_info.pm_specific_info.pm4x10.core_num_int = 0;
            pm_info.pm_specific_info.pm4x10.rescal = -1;

            for(lane=0 ; lane<SOC_PM4X10_NUM_LANES; lane++) {
                pm_info.pm_specific_info.pm4x10.lane_map.lane_map_tx[lane] =  
                    (txlane_map >> (lane * SOC_PM4X10_NUM_LANES)) &
                    SOC_PM4X10_LANE_MASK;
                pm_info.pm_specific_info.pm4x10.lane_map.lane_map_rx[lane] =  
                    (rxlane_map >> (lane * SOC_PM4X10_NUM_LANES)) &
                    SOC_PM4X10_LANE_MASK;
            }
        }

        ref_clk_prop = soc_property_port_get(unit, first_port,
                                 spn_XGXS_LCPLL_XTAL_REFCLK, phymodRefClk156Mhz);

        if ((ref_clk_prop == 125) ||( ref_clk_prop == 1))
            pm_info.pm_specific_info.pm4x10.ref_clk = phymodRefClk125Mhz;
        else      /* ((ref_clk_prop == 156) ||( ref_clk_prop == 0)) */
            pm_info.pm_specific_info.pm4x10.ref_clk = phymodRefClk156Mhz;

        pm_info.pm_specific_info.pm4x10.portmod_phy_external_reset
            = soc_esw_portctrl_reset_tsc3_cb;

        /* Add PM to PortMod */
        rv = portmod_port_macro_add(unit, &pm_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (pmid=0; pmid < num_of_instances; pmid++) {
            if (pm4x10_gh_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm4x10_gh_user_acc[unit][pmid].mutex);
                pm4x10_gh_user_acc[unit][pmid].mutex = NULL;
            }
        }
    }

    return rv;
#else /* PORTMOD_PM4X10_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X10_SUPPORT */
}

/*
 * Function:
 *      soc_gh_portctrl_pm_portmod_init
 * Purpose:
 *      Initialize PortMod and PortMacro for Greyhound. 
 *      Add port macros (PM) to the unit's PortMod Manager (PMM).
 * Parameters:
 *      unit             - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_gh_portctrl_pm_portmod_init(int unit)
{
    int rv = SOC_E_NONE, pms_arr_len = 0, count, max_phys= 0;
    portmod_pm_instances_t *pm_types = NULL;

    /* Call PortMod library initialization */
    SOC_IF_ERROR_RETURN(
        _soc_gh_portctrl_pm_init(unit, &max_phys,
                                        &pm_types, &pms_arr_len));
    if (pms_arr_len == 0) {
        return SOC_E_UNAVAIL;
    }

    /*
     * If portmod was create - destroy and create again
     * Better way would be to create once and leave it.
     */
    if (SOC_E_NONE == soc_esw_portctrl_init_check(unit)) {
        SOC_IF_ERROR_RETURN(portmod_destroy(unit));
    }

    SOC_IF_ERROR_RETURN
        (portmod_create(unit, 0, SOC_MAX_NUM_PORTS, max_phys,
                        pms_arr_len, pm_types));

    for (count = 0; SOC_SUCCESS(rv) && (count < pms_arr_len); count++) {
#ifdef PORTMOD_PM4X10_SUPPORT
        if (pm_types[count].type == portmodDispatchTypePm4x10) {
            rv = _soc_gh_portctrl_pm4x10_portmod_init
                (unit, pm_types[count].instances);
        } else
#endif
        {
            rv = SOC_E_INTERNAL;
        }
    }

    return rv;
}

/*
 * Function:
 *      soc_gh_portctrl_pm_portmod_deinit
 * Purpose:
 *      Deinitialize PortMod and PortMacro for Greyhound. 
 *      Free pm user access data.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_NONE
 */
int
soc_gh_portctrl_pm_portmod_deinit(int unit)
{
    if (SOC_E_INIT == soc_esw_portctrl_init_check(unit)) {
        return SOC_E_NONE;
    }

    /* Free PMs structures */
#ifdef PORTMOD_PM4X10_SUPPORT
    if (pm4x10_gh_user_acc[unit] != NULL) {
        sal_free(pm4x10_gh_user_acc[unit]);
    }
#endif /* PORTMOD_PM4X10_SUPPORT */
    return SOC_E_NONE;
}

#endif /* BCM_GREYHOUND_SUPPORT */

