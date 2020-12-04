/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * portctrl soc routines
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>

#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/esw/portctrl.h>
#include <soc/esw/portctrl_internal.h>
#include <soc/tomahawk.h>
#include <soc/phy/phymod_sim.h>
#include <soc/portmod/portmod.h>

#ifdef BCM_TOMAHAWK_SUPPORT
#define SOC_TH_PM4X10_COUNT         1
#define SOC_TH_PM4X25_COUNT         32 

#define SOC_TH_PM4X10_PORT_1        129  /* xe0 = 66, xe1 = 100 */

static uint32_t _th_pm4x10_addr[SOC_TH_PM4X10_COUNT] = { 0x1C1 };
static uint32_t _th_pm4x10_port[SOC_TH_PM4X10_COUNT] = { SOC_TH_PM4X10_PORT_1};

#define SOC_TH_PM4X25_PORT_1        1
#define SOC_TH_PM4X25_PORT_2        5
#define SOC_TH_PM4X25_PORT_3        9
#define SOC_TH_PM4X25_PORT_4        13
#define SOC_TH_PM4X25_PORT_5        17
#define SOC_TH_PM4X25_PORT_6        21
#define SOC_TH_PM4X25_PORT_7        25
#define SOC_TH_PM4X25_PORT_8        29
#define SOC_TH_PM4X25_PORT_9        33
#define SOC_TH_PM4X25_PORT_10       37
#define SOC_TH_PM4X25_PORT_11       41
#define SOC_TH_PM4X25_PORT_12       45
#define SOC_TH_PM4X25_PORT_13       49
#define SOC_TH_PM4X25_PORT_14       53
#define SOC_TH_PM4X25_PORT_15       57
#define SOC_TH_PM4X25_PORT_16       61
#define SOC_TH_PM4X25_PORT_17       65
#define SOC_TH_PM4X25_PORT_18       69
#define SOC_TH_PM4X25_PORT_19       73
#define SOC_TH_PM4X25_PORT_20       77
#define SOC_TH_PM4X25_PORT_21       81
#define SOC_TH_PM4X25_PORT_22       85
#define SOC_TH_PM4X25_PORT_23       89
#define SOC_TH_PM4X25_PORT_24       93
#define SOC_TH_PM4X25_PORT_25       97
#define SOC_TH_PM4X25_PORT_26       101
#define SOC_TH_PM4X25_PORT_27       105
#define SOC_TH_PM4X25_PORT_28       109
#define SOC_TH_PM4X25_PORT_29       113
#define SOC_TH_PM4X25_PORT_30       117
#define SOC_TH_PM4X25_PORT_31       121
#define SOC_TH_PM4X25_PORT_32       125

static uint32_t _th_pm4x25_addr[SOC_TH_PM4X25_COUNT] = {
    0x81,  0x85,  0x89,  0x8D,
    0x91,  0x95,  0xA1,  0xA5,
    0xA9,  0xAD,  0xc1,  0xC5,
    0xC9,  0xCD,  0xD1,  0xD5,
    0xE1,  0xE5,  0xE9,  0xED,
    0xF1,  0xF5,  0x181, 0x185,
    0x189, 0x18D, 0x1A1, 0x1A5,
    0x1A9, 0x1AD, 0x1B1, 0x1B5
};

static uint32_t _th_pm4x25_core_num[SOC_TH_PM4X25_COUNT] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

static uint32_t _th_pm4x10_core_num[SOC_TH_PM4X10_COUNT] = {32};

static uint32_t _th_pm4x25_port[SOC_TH_PM4X25_COUNT] = {
    SOC_TH_PM4X25_PORT_1,  SOC_TH_PM4X25_PORT_2,
    SOC_TH_PM4X25_PORT_3,  SOC_TH_PM4X25_PORT_4,
    SOC_TH_PM4X25_PORT_5,  SOC_TH_PM4X25_PORT_6,
    SOC_TH_PM4X25_PORT_7,  SOC_TH_PM4X25_PORT_8,
    SOC_TH_PM4X25_PORT_9,  SOC_TH_PM4X25_PORT_10,
    SOC_TH_PM4X25_PORT_11, SOC_TH_PM4X25_PORT_12,
    SOC_TH_PM4X25_PORT_13, SOC_TH_PM4X25_PORT_14,
    SOC_TH_PM4X25_PORT_15, SOC_TH_PM4X25_PORT_16,
    SOC_TH_PM4X25_PORT_17, SOC_TH_PM4X25_PORT_18,
    SOC_TH_PM4X25_PORT_19, SOC_TH_PM4X25_PORT_20,
    SOC_TH_PM4X25_PORT_21, SOC_TH_PM4X25_PORT_22,
    SOC_TH_PM4X25_PORT_23, SOC_TH_PM4X25_PORT_24,
    SOC_TH_PM4X25_PORT_25, SOC_TH_PM4X25_PORT_26,
    SOC_TH_PM4X25_PORT_27, SOC_TH_PM4X25_PORT_28,
    SOC_TH_PM4X25_PORT_29, SOC_TH_PM4X25_PORT_30,
    SOC_TH_PM4X25_PORT_31, SOC_TH_PM4X25_PORT_32
};

portmod_pm_instances_t th_pm_types[] = {
#ifdef PORTMOD_PM4X10_SUPPORT
    {portmodDispatchTypePm4x10,  SOC_TH_PM4X10_COUNT}, 
#endif
#ifdef PORTMOD_PM4X25_SUPPORT
    {portmodDispatchTypePm4x25, SOC_TH_PM4X25_COUNT},
#endif
};  

#define SOC_TH_MAX_PHYS               164    /* ((32+1)x4)  132 XLMACS, 32 CLMAC's */

#ifdef PORTMOD_PM4X10_SUPPORT
static portmod_default_user_access_t *pm4x10_th_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM4X25_SUPPORT
static portmod_default_user_access_t *pm4x25_th_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM4X25_SUPPORT */

soc_portctrl_functions_t soc_th_portctrl_func = {
    soc_th_portctrl_pm_portmod_init,
    soc_th_portctrl_pm_portmod_deinit,
    NULL, /*soc_portctrl_pm_port_config_get*/
    soc_th_portctrl_pm_port_phyaddr_get,
    NULL, /* soc_portctrl_port_ability_update*/
    NULL, /* soc_portctrl_pm_type_get*/
    NULL,
    NULL
};

STATIC int
_soc_th_portctrl_pm_init(int unit, int *max_phys,
                         portmod_pm_instances_t **pm_types,
                         int *pms_arr_len)
{
    *max_phys = SOC_TH_MAX_PHYS;
    *pm_types = th_pm_types;
    *pms_arr_len = sizeof(th_pm_types)/sizeof(portmod_pm_instances_t);
    return SOC_E_NONE;
}

STATIC int
_soc_th_portctrl_device_addr_port_get(int unit, int pm_type,
                                      uint32_t **addr,
                                      uint32_t **port,
                                      uint32_t **core_num)
{
    *addr     = NULL;
    *port     = NULL;
    *core_num = NULL;

#ifdef PORTMOD_PM4X10_SUPPORT
    if (pm_type == portmodDispatchTypePm4x10) {
        *addr = _th_pm4x10_addr;
        *port = _th_pm4x10_port;
        *core_num = _th_pm4x10_core_num;
    } 
#endif
#ifdef PORTMOD_PM4X25_SUPPORT
    if (pm_type == portmodDispatchTypePm4x25){
        *addr = _th_pm4x25_addr;
        *port = _th_pm4x25_port;
        *core_num = _th_pm4x25_core_num;
    }
#endif

    return SOC_E_NONE;
}

STATIC int
_soc_th_portctrl_pm_user_acc_flag_set(int unit,
                    portmod_default_user_access_t* pm_user_access)
{
    PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((pm_user_access));
    PORTMOD_USER_ACCESS_REG_VAL_OFFSET_ZERO_CLR((pm_user_access));
    return SOC_E_NONE;
}

/*
 * Polarity get looks for physical port based config first. This will enable
 * the user to define the polarity for all ports weather mapped to logical 
 * port or not.
 *
 * For legacy compatibility - we still look for the logical port
 * config setup.
 */
STATIC void
_soc_th_portctrl_pm_port_polarity_get
           (int unit, int logical_port, int physical_port, int lane,
            uint32 *tx_pol, uint32* rx_pol)
{
    uint32 rx_polarity, tx_polarity, num_lanes = 1;

    if (logical_port != -1) {
        num_lanes = SOC_INFO(unit).port_num_lanes[logical_port];
    }

    rx_polarity = soc_property_phy_get (unit, physical_port, 0, 0, lane, 
                                        spn_PHY_RX_POLARITY_FLIP, 0xFFFFFFFF); 

    if (rx_polarity == 0xFFFFFFFF) {
        rx_polarity = soc_property_port_get(unit, logical_port,
                       spn_PHY_XAUI_RX_POLARITY_FLIP, 0x0);
        *rx_pol = (num_lanes == 1) ? ((rx_polarity & 0x1) << lane): rx_polarity;
    } else {
        *rx_pol = (rx_polarity & 0x1) << lane;
    }

    tx_polarity = soc_property_phy_get (unit, physical_port, 0, 0, lane, 
                                        spn_PHY_TX_POLARITY_FLIP, 0xFFFFFFFF); 
    if (tx_polarity == 0xFFFFFFFF) {
        tx_polarity =  soc_property_port_get(unit, logical_port,
                          spn_PHY_XAUI_TX_POLARITY_FLIP, 0x0);
        *tx_pol = (num_lanes == 1) ? ((tx_polarity & 0x1) << lane) : tx_polarity;
    } else {
        *tx_pol = (tx_polarity & 0x1) << lane;
    }

}

/*
 * lanemap get looks for physical port based config first. This will enable
 * the user to define the lanemap for all ports weather mapped to logical 
 * port or not.
 *
 * For legacy compatibility - we still look for the logical port
 * config setup.
 */
STATIC 
void _soc_th_portctrl_pm_port_lanemap_get
            (int unit, int logical_port, int physical_port, int core_num, 
            uint32 *txlane_map, uint32* rxlane_map)
{

    *rxlane_map = soc_property_phy_get (unit, physical_port, 0, 0, 0, 
                                        spn_PHY_RX_LANE_MAP, 0xFFFFFFFF);

    if (*rxlane_map == 0xFFFFFFFF) {
        *rxlane_map = soc_property_port_suffix_num_get(unit, logical_port, 
                                        core_num, spn_XGXS_RX_LANE_MAP,
                                        "core", 0x3210);
    }

    *txlane_map = soc_property_phy_get (unit, physical_port, 0, 0, 0, 
                                        spn_PHY_TX_LANE_MAP, 0xFFFFFFFF);

    if (*txlane_map == 0xFFFFFFFF) {
        *txlane_map = soc_property_port_suffix_num_get(unit, logical_port, 
                                        core_num, spn_XGXS_TX_LANE_MAP,
                                        "core", 0x3210);
    }
}

STATIC int
_soc_th_portctrl_pm4x10_portmod_init(int unit, int num_of_instances) 
{
#ifdef PORTMOD_PM4X10_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm_info;
    int pmid = 0, blk, is_sim, lane, phy, found, logical_port;
    int first_port, idx, ref_clk_prop = 0, core_num, core_cnt;
    uint32 txlane_map, rxlane_map, *pAddr, *pPort, *pCoreNum;
    uint32 tx_polarity, rx_polarity;

    SOC_IF_ERROR_RETURN
        (_soc_th_portctrl_device_addr_port_get(unit,
                                               portmodDispatchTypePm4x10,
                                               &pAddr, &pPort, &pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* Allocate PMs */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                           num_of_instances,
                                           &pm4x10_th_user_acc[unit]));

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
                                                &(pm4x10_th_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        SOC_IF_ERROR_RETURN(
            _soc_th_portctrl_pm_user_acc_flag_set(unit,
                &(pm4x10_th_user_acc[unit][pmid])));

        pm4x10_th_user_acc[unit][pmid].unit = unit;
        pm4x10_th_user_acc[unit][pmid].blk_id = blk; 
        pm4x10_th_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");
        if (pm4x10_th_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        /* Specific info for PM4x10 */
        rv = phymod_access_t_init(&pm_info.pm_specific_info.pm4x10.access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_info.pm_specific_info.pm4x10.access.user_acc =
            &(pm4x10_th_user_acc[unit][pmid]);
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

       /* Polarity */
       rv = phymod_polarity_t_init
            (&(pm_info.pm_specific_info.pm4x10.polarity));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        for (lane = 0; lane < SOC_PM4X10_NUM_LANES; lane++) {
            phy = first_port+lane;
            logical_port = SOC_INFO(unit).port_p2l_mapping[phy];

            _soc_th_portctrl_pm_port_polarity_get
                (unit, logical_port, phy, lane, &tx_polarity, &rx_polarity);

            pm_info.pm_specific_info.pm4x10.polarity.rx_polarity |= rx_polarity;
            pm_info.pm_specific_info.pm4x10.polarity.tx_polarity |= tx_polarity;

            if (SOC_INFO(unit).port_num_lanes[logical_port] >= SOC_PM4X10_NUM_LANES) break; 
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
            _soc_th_portctrl_pm_port_lanemap_get
                (unit, logical_port, first_port, core_num, &txlane_map, &rxlane_map);
            
            /* possible internal swap to be added */

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
                                spn_XGXS_LCPLL_XTAL_REFCLK, 156);

        if ((ref_clk_prop == 156) ||( ref_clk_prop == 1)){
            pm_info.pm_specific_info.pm4x10.ref_clk = phymodRefClk156Mhz;
        }
        else if (ref_clk_prop == 125){     
            pm_info.pm_specific_info.pm4x10.ref_clk = phymodRefClk125Mhz;
        }
        else {
            LOG_ERROR(BSL_LS_SOC_PORT,
                    (BSL_META_UP(unit, first_port,
                              "ERROR: This %d MHz clk freq is not supported. Only 156 MHz and 125 MHz.\n"),ref_clk_prop));
            return SOC_E_PARAM ;
        }

        pm_info.pm_specific_info.pm4x10.portmod_phy_external_reset
            = soc_esw_portctrl_reset_tsc3_cb;

        /* 4x10 is only used in mgmt ports */
        pm_info.pm_specific_info.pm4x10.port_mode_aux_info = portmodModeInfoNone;
        
        /* Add PM to PortMod */
        rv = portmod_port_macro_add(unit, &pm_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (pmid=0; pmid < num_of_instances; pmid++) {
            if (pm4x10_th_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm4x10_th_user_acc[unit][pmid].mutex);
                pm4x10_th_user_acc[unit][pmid].mutex = NULL;
            }
        }
    }

    return rv;
#else /* PORTMOD_PM4X10_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X10_SUPPORT */
}

STATIC int
_soc_th_portctrl_pm4x25_portmod_init(int unit, int num_of_instances) 
{
#ifdef PORTMOD_PM4X25_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm_info;
    int pmid = 0, blk, is_sim, lane, phy, found, logical_port;
    int first_port, idx, ref_clk_prop, core_num, core_cnt;
    uint32 txlane_map, rxlane_map, *pAddr, *pPort,*pCoreNum;
    uint32 tx_polarity, rx_polarity;
    
    SOC_IF_ERROR_RETURN
        (_soc_th_portctrl_device_addr_port_get(unit,
                                               portmodDispatchTypePm4x25,
                                               &pAddr, &pPort,&pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* Allocate PMs */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                           num_of_instances,
                                               &pm4x25_th_user_acc[unit]));

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
            if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_CLPORT){
                found = 1;
                break;
            }
        }

        if (!found) {
            rv = SOC_E_INTERNAL;
            break;
        } 

        pm_info.type         = portmodDispatchTypePm4x25;

        /* Init user_acc for phymod access struct */
        rv = portmod_default_user_access_t_init(unit,
                                                &(pm4x25_th_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(pm4x25_th_user_acc[unit][pmid])));
        PORTMOD_USER_ACCESS_REG_VAL_OFFSET_ZERO_CLR((&(pm4x25_th_user_acc[unit][pmid])));
        pm4x25_th_user_acc[unit][pmid].unit = unit;
        pm4x25_th_user_acc[unit][pmid].blk_id = blk; 
        pm4x25_th_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");
        if (pm4x25_th_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        /* Specific info for PM4x25 */
        rv = phymod_access_t_init(&pm_info.pm_specific_info.pm4x25.access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_info.pm_specific_info.pm4x25.access.user_acc =
            &(pm4x25_th_user_acc[unit][pmid]);
        pm_info.pm_specific_info.pm4x25.access.addr     = pAddr[pmid];
        pm_info.pm_specific_info.pm4x25.access.bus      = NULL; /* Use default bus */
#if !defined (EXCLUDE_PHYMOD_TSCF_SIM) && defined (PHYMOD_TSCF_SUPPORT)
        rv = soc_physim_check_sim(unit, phymodDispatchTypeTscf, 
                                  &(pm_info.pm_specific_info.pm4x25.access),
                                  0, &is_sim);
#else
        is_sim = 0;
#endif

        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        if (is_sim) {
            pm_info.pm_specific_info.pm4x25.access.bus->bus_capabilities |= 
                (PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL);

            /* Firmward loader : Don't allow FW load on sim enviroment */
            pm_info.pm_specific_info.pm4x25.fw_load_method =
                phymodFirmwareLoadMethodNone;
        } else {
            pm_info.pm_specific_info.pm4x25.fw_load_method = 
                soc_property_suffix_num_get(unit, pmid, 
                                            spn_LOAD_FIRMWARE, "quad", 
                                            phymodFirmwareLoadMethodExternal);
            pm_info.pm_specific_info.pm4x25.fw_load_method &= 0xff;
        }

        /* Use default external loader if NULL */
        pm_info.pm_specific_info.pm4x25.external_fw_loader = NULL;

        /* Polarity */
        rv = phymod_polarity_t_init (&(pm_info.pm_specific_info.pm4x25.polarity));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        for (lane = 0; lane < SOC_PM4X25_NUM_LANES; lane++) {
            phy = first_port+lane;
            logical_port = SOC_INFO(unit).port_p2l_mapping[phy];

            _soc_th_portctrl_pm_port_polarity_get
                (unit, logical_port, phy, lane, &tx_polarity, &rx_polarity);

            pm_info.pm_specific_info.pm4x25.polarity.rx_polarity |= rx_polarity;
            pm_info.pm_specific_info.pm4x25.polarity.tx_polarity |= tx_polarity;

            if (SOC_INFO(unit).port_num_lanes[logical_port] >= SOC_PM4X25_NUM_LANES) break;
        }

        /* Lane map */
        rv = phymod_lane_map_t_init
            (&(pm_info.pm_specific_info.pm4x25.lane_map));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        logical_port = SOC_INFO(unit).port_p2l_mapping[first_port];
        core_cnt = 1; 

        for (core_num = 0; core_num < core_cnt; core_num++) {
            _soc_th_portctrl_pm_port_lanemap_get
                (unit, logical_port, first_port, core_num, &txlane_map, &rxlane_map);

            /* possible internal swap to be added */
            
            pm_info.pm_specific_info.pm4x25.lane_map.num_of_lanes =
                SOC_PM4X25_NUM_LANES;
   
            pm_info.pm_specific_info.pm4x25.core_num = pCoreNum? pCoreNum[pmid] : pmid;
            pm_info.pm_specific_info.pm4x25.core_num_int = 0;
            pm_info.pm_specific_info.pm4x25.rescal = -1;
 
            for(lane=0 ; lane<SOC_PM4X25_NUM_LANES; lane++) {
                pm_info.pm_specific_info.pm4x25.lane_map.lane_map_tx[lane] =  
                    (txlane_map >> (lane * SOC_PM4X25_NUM_LANES)) &
                    SOC_PM4X25_LANE_MASK;
                pm_info.pm_specific_info.pm4x25.lane_map.lane_map_rx[lane] =  
                    (rxlane_map >> (lane * SOC_PM4X25_NUM_LANES)) &
                    SOC_PM4X25_LANE_MASK;
            }
        }

        ref_clk_prop = soc_property_port_get(unit, first_port,
                            spn_XGXS_LCPLL_XTAL_REFCLK, 156);

        if ((ref_clk_prop == 156) ||( ref_clk_prop == 1)){
            pm_info.pm_specific_info.pm4x25.ref_clk = phymodRefClk156Mhz;
        }
        else if (ref_clk_prop == 125){     
            pm_info.pm_specific_info.pm4x25.ref_clk = phymodRefClk125Mhz;
        }
        else {
            LOG_ERROR(BSL_LS_SOC_PORT,
                    (BSL_META_UP(unit, first_port,
                              "ERROR: This %d MHz clk freq is not supported. Only 156 MHz and 125 MHz.\n"),ref_clk_prop));
            return SOC_E_PARAM ;
        }

        /* Add PM to PortMod */
        rv = portmod_port_macro_add(unit, &pm_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (pmid=0; pmid < num_of_instances; pmid++) {
            if (pm4x25_th_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm4x25_th_user_acc[unit][pmid].mutex);
                pm4x25_th_user_acc[unit][pmid].mutex = NULL;
            }
        }
    }

    return rv;
#else /* PORTMOD_PM4X25_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X25_SUPPORT */
}

/*
 * Function:
 *      soc_th_portctrl_pm_portmod_init
 * Purpose:
 *      Initialize PortMod and PortMacro for Tomahawk
 *      Add port macros (PM) to the unit's PortMod Manager (PMM).
 * Parameters:
 *      unit             - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_th_portctrl_pm_portmod_init(int unit)
{
    int rv = SOC_E_NONE, pms_arr_len = 0, count, max_phys= 0;
    portmod_pm_instances_t *pm_types = NULL;

    /* Call PortMod library initialization */
    SOC_IF_ERROR_RETURN(
        _soc_th_portctrl_pm_init(unit, &max_phys,
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
            rv = _soc_th_portctrl_pm4x10_portmod_init
                (unit, pm_types[count].instances);
        } else 
#endif /* PORTMOD_PM4X10_SUPPORT  */
#ifdef PORTMOD_PM4X25_SUPPORT
        if (pm_types[count].type == portmodDispatchTypePm4x25) {
            rv = _soc_th_portctrl_pm4x25_portmod_init
                (unit, pm_types[count].instances);
        } else
#endif /* PORTMOD_PM4X25_SUPPORT  */
        {
            rv = SOC_E_INTERNAL;
        }
    }

    return rv;
}

/*
 * Function:
 *      soc_th_portctrl_pm_portmod_deinit
 * Purpose:
 *      Deinitialize PortMod and PortMacro for Tomahawk.
 *      Free pm user access data.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_NONE
 */
int
soc_th_portctrl_pm_portmod_deinit(int unit)
{
    if (SOC_E_INIT == soc_esw_portctrl_init_check(unit)) {
        return SOC_E_NONE;
    }

    /* Free PMs structures */
#ifdef PORTMOD_PM4X10_SUPPORT
    if (pm4x10_th_user_acc[unit] != NULL) {
        sal_free(pm4x10_th_user_acc[unit]);
        pm4x10_th_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM4X25_SUPPORT
    if (pm4x25_th_user_acc[unit] != NULL) {
        sal_free(pm4x25_th_user_acc[unit]);
        pm4x25_th_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM4X25_SUPPORT */
    return SOC_E_NONE;
}

int
soc_th_portctrl_pm_port_phyaddr_get(int unit, soc_port_t port)
{
    int phy, core_index;
    uint32 i, *pAddr = NULL, *pPort = NULL, *pCoreNum = NULL;

    phy = SOC_INFO(unit).port_l2p_mapping[port];

    /* Find the port belong to which PM. The port has its own core id, using it to compare PM. */
    core_index = (phy - 1) / SOC_PM4X10_NUM_LANES;
#ifdef PORTMOD_PM4X10_SUPPORT
    for (i=0; i<SOC_TH_PM4X10_COUNT; i++) {
        if (core_index == _th_pm4x10_core_num[i]) {
            SOC_IF_ERROR_RETURN
                (_soc_th_portctrl_device_addr_port_get(unit,
                                             portmodDispatchTypePm4x10,
                                             &pAddr, &pPort, &pCoreNum));
            return pAddr[i];
        }
    }
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM4X25_SUPPORT
    /* When port dones't find PM4x10, find PM4x25 again. */
    for (i=0; i<SOC_TH_PM4X25_COUNT; i++) {
        if (core_index == _th_pm4x25_core_num[i]) {
            SOC_IF_ERROR_RETURN
                (_soc_th_portctrl_device_addr_port_get(unit,
                                             portmodDispatchTypePm4x25,
                                             &pAddr, &pPort, &pCoreNum));
            return pAddr[i];
        }
    }
#endif /* PORTMOD_PM4X25_SUPPORT */

    return -1;
}

#endif /* BCM_TOMAHAWK_SUPPORT */
