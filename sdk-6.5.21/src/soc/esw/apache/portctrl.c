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
#include <soc/bondoptions.h>
#include <soc/mem.h>
#include <soc/esw/portctrl.h>
#include <soc/esw/portctrl_internal.h>
#include <soc/apache.h>
#include <shared/bsl.h>
#include <soc/phy/phymod_sim.h>
#include <soc/portmod/portmod.h>

#ifdef BCM_APACHE_SUPPORT

#define SOC_AP_PM4X10_COUNT         8
#define SOC_AP_PM4X25_COUNT         4
#define SOC_AP_PM12X10_COUNT        2

#define SOC_AP_MAX_PORTS_PER_TSC    4

#define SOC_AP_PM4X10_PORT_1        1
#define SOC_AP_PM4X10_PORT_2        5
#define SOC_AP_PM4X10_PORT_3        9
#define SOC_AP_PM4X10_PORT_4        13
#define SOC_AP_PM4X10_PORT_5        37
#define SOC_AP_PM4X10_PORT_6        41
#define SOC_AP_PM4X10_PORT_7        45
#define SOC_AP_PM4X10_PORT_8        49

#define SOC_AP_PM12X10_PORT_1        17
#define SOC_AP_PM12X10_PORT_2        53

#define SOC_AP_PM4X25_PORT_1         29
#define SOC_AP_PM4X25_PORT_2         33
#define SOC_AP_PM4X25_PORT_3         65
#define SOC_AP_PM4X25_PORT_4         69

static uint32_t _ap_pm4x10_addr[SOC_AP_PM4X10_COUNT] = {
    0x81, 0x85, 0x89, 0x8D, 0xA9, 0xAD, 0xB1, 0xC1
};

static uint32_t _ap_pm4x10_core_num[SOC_AP_PM4X10_COUNT] = {
    0, 1, 2, 3, 9, 10, 11, 12
};

static uint32_t _ap_pm4x10_port[SOC_AP_PM4X10_COUNT] = {
    SOC_AP_PM4X10_PORT_1,
    SOC_AP_PM4X10_PORT_2,
    SOC_AP_PM4X10_PORT_3,
    SOC_AP_PM4X10_PORT_4,
    SOC_AP_PM4X10_PORT_5,
    SOC_AP_PM4X10_PORT_6,
    SOC_AP_PM4X10_PORT_7,
    SOC_AP_PM4X10_PORT_8
};

static uint32_t _ap_pm12x10_addr[SOC_AP_PM12X10_COUNT * SOC_PM12X10_PM4X10_COUNT] = {
    0x91,  0x95,  0x99,           /* PM12X10 Inst 0, Core 4-6   */
    0xC5,  0xC9,  0xCD            /* PM12X10 Inst 1, Core 12-14 */
};
static uint32_t _ap_pm12x10_core_num[SOC_AP_PM12X10_COUNT * SOC_PM12X10_PM4X10_COUNT] = {
    4, 5, 6, 13, 14, 15
};

static uint32_t _ap_pm12x10_port[SOC_AP_PM12X10_COUNT] = {
    SOC_AP_PM12X10_PORT_1,
    SOC_AP_PM12X10_PORT_2
};

static uint32_t _ap_pm4x25_addr[SOC_AP_PM4X25_COUNT] = {
    0xA1, 0xA5, 0xD1, 0xD5
};

static uint32_t _ap_pm4x25_core_num[SOC_AP_PM4X25_COUNT] = {
    7, 8, 16, 17
};

static uint32_t _ap_pm4x25_port[SOC_AP_PM4X25_COUNT] = {
    SOC_AP_PM4X25_PORT_1,
    SOC_AP_PM4X25_PORT_2,
    SOC_AP_PM4X25_PORT_3,
    SOC_AP_PM4X25_PORT_4
};


#define SOC_AP_MAX_PHYS            80    /* ?? 56 XLMACS, 24 CLMAC's */


static portmod_pm_instances_t ap_pm_types[] = {
#ifdef PORTMOD_PM4X10_SUPPORT
        {portmodDispatchTypePm4x10,  SOC_AP_PM4X10_COUNT},
#endif
#ifdef PORTMOD_PM4X25_SUPPORT
        {portmodDispatchTypePm4x25,  SOC_AP_PM4X25_COUNT},
#endif
#ifdef PORTMOD_PM12X10_XGS_SUPPORT
        {portmodDispatchTypePm12x10_xgs, SOC_AP_PM12X10_COUNT},
#endif
};


#ifdef PORTMOD_PM4X10_SUPPORT
static portmod_default_user_access_t *pm4x10_ap_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM12X10_XGS_SUPPORT
static portmod_default_user_access_t *pm12x10_ap_user_acc[SOC_MAX_NUM_DEVICES];
static portmod_default_user_access_t *pm12_4x10_ap_user_acc[SOC_MAX_NUM_DEVICES];
static portmod_default_user_access_t *pm12_4x25_ap_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM12X10_XGS_SUPPORT */

#ifdef PORTMOD_PM4X25_SUPPORT
static portmod_default_user_access_t *pm4x25_ap_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM4X25_SUPPORT */


soc_portctrl_functions_t soc_ap_portctrl_func = {
    soc_ap_portctrl_pm_portmod_init,
    soc_ap_portctrl_pm_portmod_deinit,
    soc_ap_portctrl_pm_port_config_get,
    soc_ap_portctrl_pm_port_phyaddr_get,
    soc_ap_portctrl_port_ability_update,
    soc_ap_portctrl_pm_type_get,
    NULL,
    NULL
};

STATIC int
_soc_ap_portctrl_pm_init(int unit, int *max_phys,
                           portmod_pm_instances_t **pm_types,
                           int *pms_arr_len)
{
    *max_phys = SOC_AP_MAX_PHYS;
    *pm_types = ap_pm_types;
    *pms_arr_len = (sizeof(ap_pm_types) / sizeof(portmod_pm_instances_t));

    return SOC_E_NONE;
}

STATIC int
_soc_ap_portctrl_device_addr_port_get(int unit, int pm_type,
                                        uint32_t **addr,
                                        uint32_t **port,
                                        uint32_t **core_num)
{
    *addr     = NULL;
    *port     = NULL;
    *core_num = NULL;

    if (pm_type == portmodDispatchTypePm4x10) {
        *addr = _ap_pm4x10_addr;
        *port = _ap_pm4x10_port;
        *core_num = _ap_pm4x10_core_num;
    } else if (pm_type == portmodDispatchTypePm4x25){
        *addr = _ap_pm4x25_addr;
        *port = _ap_pm4x25_port;
        *core_num = _ap_pm4x25_core_num;
    } else if (pm_type == portmodDispatchTypePm12x10_xgs){
        *addr = _ap_pm12x10_addr;
        *port = _ap_pm12x10_port;
        *core_num = _ap_pm12x10_core_num;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_ap_portctrl_pm_user_acc_flag_set(int unit,
                    portmod_default_user_access_t* pm_user_access)
{
    return SOC_E_NONE;
}


#define NIBBLE_SWAP(data) (\
             (((data) & 0x0f) << 12) | \
             (((data) & 0xf0) << 4 ) | \
             (((data) >> 4) & 0xf0 ) | \
             (((data) >> 12) & 0x0f ))

#define LANE_SWAP(data)  \
            ((0x3-(data&0x0f)) | (0x30-(data&0xf0)) | \
            (0x300-(data&0xf00)) | (0x3000-(data&0xf000)))

#define NIBBLE_GET(data, num)                      \
            (num == 0) ?  (data & 0x0f)          : \
            (num == 1) ?  ((data & 0xf0)   >> 4) : \
            (num == 2) ?  ((data & 0xf00)  >> 8) : \
            (num == 3) ?  ((data & 0xf000) >> 12): \
            -1

/* Indexed by direction (rx=0; tx=1) and physical port number */
static int apache_def_pkg_rx_polarity_flip[73] = {0};
static int apache_def_pkg_tx_polarity_flip[73] = {0};
static int apache_tdp_pkg_rx_polarity_flip[73] = {0};
static int apache_tdp_pkg_tx_polarity_flip[73] = {0};
static int apache_td2_pkg_rx_polarity_flip[73] = {
    -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 /* Only TSC2 lanes 2 & 3 needs polarity inversion */
};
static int apache_td2_pkg_tx_polarity_flip[73] = {
    -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1  /* Only TSC2 lanes 2 & 3 needs polarity inversion */
};

/* Indexed by direction (rx=0; tx=1) and TSC instance */
static int apache_def_pkg_rx_lane_swap[18] = {0};
static int apache_def_pkg_tx_lane_swap[18] = {0};
static int apache_tdp_pkg_rx_lane_swap[18] = {
    0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1
};
static int apache_tdp_pkg_tx_lane_swap[18] = {
    1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0
};
static int apache_td2_pkg_rx_lane_swap[18] = {
    0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0
};
static int apache_td2_pkg_tx_lane_swap[18] = {
    1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1
};

STATIC void
_soc_apache_portctrl_pkg_config_set(int unit, int phy_port, uint32 *rx_pol, uint32 *tx_pol,
                                    uint32 *rxlane_map, uint32 *txlane_map)
{
    int *pkg_rx_lane_swap, *pkg_tx_lane_swap;
    int *pkg_rx_polarity_flip, *pkg_tx_polarity_flip;
    int tsc_num, lane, i;

    tsc_num = (phy_port - 1) / SOC_PM4X10_NUM_LANES;

    if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureApachePkg50mmTDpCompat))  {
        /* socBondInfoFeatureApachePkg50mmTDpCompat
         * 50mm x 50mm TD+ compatible package */
        pkg_rx_lane_swap = &(apache_tdp_pkg_rx_lane_swap[0]);
        pkg_tx_lane_swap = &(apache_tdp_pkg_tx_lane_swap[0]);
        pkg_rx_polarity_flip = &(apache_tdp_pkg_rx_polarity_flip[0]);
        pkg_tx_polarity_flip = &(apache_tdp_pkg_tx_polarity_flip[0]);
    } else if ((SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureApachePkg50mmTD2pCompat)) ||
               (SOC_BOND_INFO(unit)->dev_id == BCM56765_DEVICE_ID))  {
        /* socBondInfoFeatureApachePkg50mmTD2pCompat
         * 50mm x 50mm TD2 compatible package */
        pkg_rx_lane_swap = &(apache_td2_pkg_rx_lane_swap[0]);
        pkg_tx_lane_swap = &(apache_td2_pkg_tx_lane_swap[0]);
        pkg_rx_polarity_flip = &(apache_td2_pkg_rx_polarity_flip[0]);
        pkg_tx_polarity_flip = &(apache_td2_pkg_tx_polarity_flip[0]);
    } else  {
        /* socBondInfoFeatureApachePkg42p5mm
         * 42.5mm x 42.5mm original/base package */
        pkg_rx_lane_swap = &(apache_def_pkg_rx_lane_swap[0]);
        pkg_tx_lane_swap = &(apache_def_pkg_tx_lane_swap[0]);
        pkg_rx_polarity_flip = &(apache_def_pkg_rx_polarity_flip[0]);
        pkg_tx_polarity_flip = &(apache_def_pkg_tx_polarity_flip[0]);
    }

    for (i = 0; i < SOC_PM4X10_NUM_LANES; i++) {
        if (pkg_rx_polarity_flip[phy_port + i]) {
            if (pkg_rx_lane_swap[tsc_num] == 0) {
                lane = NIBBLE_GET(*rxlane_map, i);
                *rx_pol = (*rx_pol) ^ (0x1 << lane);
            }
        }
        if (pkg_tx_polarity_flip[phy_port + i]) {
            if (pkg_tx_lane_swap[tsc_num] == 0) {
                lane = NIBBLE_GET(*txlane_map, i);
                *tx_pol = (*tx_pol) ^ (0x1 << lane);
            }
        }
    }

    if (pkg_rx_lane_swap[tsc_num] == 1) {
        for (i = 0; i < SOC_PM4X10_NUM_LANES; i++) {
            if (pkg_rx_polarity_flip[phy_port + i]) {
                lane = NIBBLE_GET(*rxlane_map, ((SOC_PM4X10_NUM_LANES - 1) - i));
                *rx_pol = (*rx_pol) ^ (0x1 << lane);
            }
        }
        *rxlane_map = LANE_SWAP(*rxlane_map);
    }
    if (pkg_tx_lane_swap[tsc_num] == 1) {
        for (i = 0; i < SOC_PM4X10_NUM_LANES; i++) {
            if (pkg_tx_polarity_flip[phy_port + i]) {
                lane = NIBBLE_GET(*txlane_map, ((SOC_PM4X10_NUM_LANES - 1) - i));
                *tx_pol = (*tx_pol) ^ (0x1 << lane);
            }
        }
        *txlane_map = LANE_SWAP(*txlane_map);
    }

    return;
}
/*
 * Polarity get looks for physical port based config first. This will enable
 * the user to define the polarity for all ports weather mapped to logical 
 * port or not.
 *
 * For legacy compatibility - we still look for the logical port
 * config setup.
 */
STATIC
void apache_pm_port_polarity_get(int unit, int logical_port, int physical_port, int lane,
                                 int core_num, uint32 *tx_pol, uint32* rx_pol)
{
    uint32 rx_polarity, tx_polarity, num_lanes=1;

    if (logical_port != -1) {
        num_lanes = SOC_INFO(unit).port_num_lanes[logical_port];
    }
    rx_polarity = soc_property_phy_get (unit, physical_port, 0, 0, lane,
                     spn_PHY_CHAIN_RX_POLARITY_FLIP_PHYSICAL, 0xFFFFFFFF);
    if (rx_polarity == 0xFFFFFFFF) {
        rx_polarity = soc_property_port_get(unit, logical_port,
                         spn_PHY_XAUI_RX_POLARITY_FLIP, 0);
        *rx_pol = (num_lanes == 1) ? ((rx_polarity & 0x1) << lane): rx_polarity;
        if (SOC_PORT_VALID(unit, logical_port) &&
            IS_CXX_PORT(unit, logical_port) && IS_CL_PORT(unit, logical_port)) {
            *rx_pol =  ((rx_polarity >> (core_num * SOC_PM4X10_NUM_LANES)) & 0xf);
        }
    } else {
        *rx_pol = (rx_polarity & 0x1) << lane;
    }

    tx_polarity = soc_property_phy_get (unit, physical_port, 0, 0, lane,
                     spn_PHY_CHAIN_TX_POLARITY_FLIP_PHYSICAL, 0xFFFFFFFF);
    if (tx_polarity == 0xFFFFFFFF) {
        tx_polarity =  soc_property_port_get(unit, logical_port,
                          spn_PHY_XAUI_TX_POLARITY_FLIP, 0);
        *tx_pol = (num_lanes == 1) ? ((tx_polarity & 0x1) << lane) : tx_polarity;
        if (SOC_PORT_VALID(unit, logical_port) &&
            IS_CXX_PORT(unit, logical_port) && IS_CL_PORT(unit, logical_port)) {
            *tx_pol =  ((tx_polarity >> (core_num * SOC_PM4X10_NUM_LANES)) & 0xf);
        }
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
void apache_pm_port_lanemap_get (int unit, int logical_port, int physical_port,
                                 int core_num, uint32 *txlane_map, uint32* rxlane_map)
{

    *rxlane_map = soc_property_phy_get (unit, physical_port, 0, 0, 0, 
                                        spn_PHY_CHAIN_RX_LANE_MAP_PHYSICAL, 0xFFFFFFFF);

    if (*rxlane_map == 0xFFFFFFFF) {
        *rxlane_map = soc_property_phy_get (unit, physical_port, 0, 0, 0, 
                                        spn_PHY_RX_LANE_MAP, 0xFFFFFFFF);
    }
    if (*rxlane_map == 0xFFFFFFFF) {
        *rxlane_map = soc_property_port_suffix_num_get(unit, logical_port, core_num,
                                                 spn_XGXS_RX_LANE_MAP,
                                                 "core", 0x3210);
    }

    *txlane_map = soc_property_phy_get (unit, physical_port, 0, 0, 0, 
                                        spn_PHY_CHAIN_TX_LANE_MAP_PHYSICAL, 0xFFFFFFFF);

    if (*txlane_map == 0xFFFFFFFF) {
        *txlane_map = soc_property_phy_get (unit, physical_port, 0, 0, 0, 
                                        spn_PHY_TX_LANE_MAP, 0xFFFFFFFF);
    }
    if (*txlane_map == 0xFFFFFFFF) {
        *txlane_map = soc_property_port_suffix_num_get(unit, logical_port, core_num,
                                                 spn_XGXS_TX_LANE_MAP,
                                                 "core", 0x3210);
    }
}


STATIC int
_soc_ap_portctrl_pm4x10_portmod_init(int unit, int num_of_instances)
{
#ifdef PORTMOD_PM4X10_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm_info;
    int pmid = 0, blk, is_sim, found, logical_port;
    int first_port, idx, core_num, core_cnt;
    uint32 *pAddr, *pPort, *pCoreNum;
    int mode;

    SOC_IF_ERROR_RETURN
        (_soc_ap_portctrl_device_addr_port_get(unit,
                                               portmodDispatchTypePm4x10,
                                               &pAddr, &pPort, &pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* Allocate PMs */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                           num_of_instances,
                                           &pm4x10_ap_user_acc[unit]));

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
            if (SOC_BLOCK_INFO(unit, blk).type ==
                (soc_feature(unit, soc_feature_xlportb0) ? SOC_BLK_XLPORTB0 : SOC_BLK_XLPORT)){
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
                                                &(pm4x10_ap_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        SOC_IF_ERROR_RETURN(
            _soc_ap_portctrl_pm_user_acc_flag_set(unit,
                &(pm4x10_ap_user_acc[unit][pmid])));

        PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(pm4x10_ap_user_acc[unit][pmid])));
        pm4x10_ap_user_acc[unit][pmid].unit = unit;
        pm4x10_ap_user_acc[unit][pmid].blk_id = blk;
        pm4x10_ap_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");
        if (pm4x10_ap_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        /* Specific info for PM4x10 */
        rv = phymod_access_t_init(&pm_info.pm_specific_info.pm4x10.access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_info.pm_specific_info.pm4x10.access.user_acc =
            &(pm4x10_ap_user_acc[unit][pmid]);
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
        }

        /* Use default external loader if NULL */
        pm_info.pm_specific_info.pm4x10.external_fw_loader = NULL;

        logical_port = SOC_INFO(unit).port_p2l_mapping[first_port];
        core_cnt = 1;

        for (core_num = 0; core_num < core_cnt; core_num++) {
            pm_info.pm_specific_info.pm4x10.core_num = pCoreNum? pCoreNum[pmid] : pmid;
            pm_info.pm_specific_info.pm4x10.core_num_int = 0;
            pm_info.pm_specific_info.pm4x10.rescal = -1;
        }

        SOC_IF_ERROR_RETURN(soc_apache_port_mode_get(unit, logical_port, &mode));
        if (mode == portmodPortModeTri012 || mode == portmodPortModeTri023) {
            pm_info.pm_specific_info.pm4x10.port_mode_aux_info = portmodModeInfoThreePorts;
        }

        /* Add PM to PortMod */
        rv = portmod_port_macro_add(unit, &pm_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (pmid=0; pmid < num_of_instances; pmid++) {
            if (pm4x10_ap_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm4x10_ap_user_acc[unit][pmid].mutex);
                pm4x10_ap_user_acc[unit][pmid].mutex = NULL;
            }
        }
    }

    return rv;
#else /* PORTMOD_PM4X10_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X10_SUPPORT */
}

STATIC int
_soc_ap_portctrl_pm4x25_portmod_init(int unit, int num_of_instances)
{
#ifdef PORTMOD_PM4X25_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm_info;
    int pmid = 0, blk, is_sim, found, logical_port;
    int first_port, idx, core_num, core_cnt;
    uint32 *pAddr, *pPort,*pCoreNum;
    int mode;

    SOC_IF_ERROR_RETURN
        (_soc_ap_portctrl_device_addr_port_get(unit,
                                                 portmodDispatchTypePm4x25,
                                                 &pAddr, &pPort,&pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* Allocate PMs */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                           num_of_instances,
                                           &pm4x25_ap_user_acc[unit]));

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
            if (SOC_BLOCK_INFO(unit, blk).type ==
                (soc_feature(unit, soc_feature_clport_gen2) ? SOC_BLK_CLG2PORT : SOC_BLK_CLPORT)){
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
                                                &(pm4x25_ap_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(pm4x25_ap_user_acc[unit][pmid])));
        pm4x25_ap_user_acc[unit][pmid].unit = unit;
        pm4x25_ap_user_acc[unit][pmid].blk_id = blk;
        pm4x25_ap_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");
        if (pm4x25_ap_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        /* Specific info for PM4x25 */
        rv = phymod_access_t_init(&pm_info.pm_specific_info.pm4x25.access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_info.pm_specific_info.pm4x25.access.user_acc =
            &(pm4x25_ap_user_acc[unit][pmid]);
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
        }

        /* Use default external loader if NULL */
        pm_info.pm_specific_info.pm4x25.external_fw_loader = NULL;

        logical_port = SOC_INFO(unit).port_p2l_mapping[first_port];
        core_cnt = 1;

        for (core_num = 0; core_num < core_cnt; core_num++) {
            pm_info.pm_specific_info.pm4x25.core_num = pCoreNum? pCoreNum[pmid] : pmid;
            pm_info.pm_specific_info.pm4x25.core_num_int = 0;
            pm_info.pm_specific_info.pm4x25.rescal = -1;
        }

        SOC_IF_ERROR_RETURN(soc_apache_port_mode_get(unit, logical_port, &mode));
        if (mode == portmodPortModeTri012 || mode == portmodPortModeTri023) {
            pm_info.pm_specific_info.pm4x10.port_mode_aux_info = portmodModeInfoThreePorts;
        }

        /* Add PM to PortMod */
        rv = portmod_port_macro_add(unit, &pm_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (pmid=0; pmid < num_of_instances; pmid++) {
            if (pm4x25_ap_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm4x25_ap_user_acc[unit][pmid].mutex);
                pm4x25_ap_user_acc[unit][pmid].mutex = NULL;
            }
        }
    }

    return rv;
#else /* PORTMOD_PM4X25_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X25_SUPPORT */
}


STATIC int
_soc_ap_portctrl_pm12x10_portmod_init(int unit, int num_of_instances)
{
#if defined(PORTMOD_PM12X10_XGS_SUPPORT)
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t  pm_info;
    portmod_pm4x10_create_info_t *pm4x10_info;
    portmod_pm4x25_create_info_t *pm4x25_info;
    int pmid = 0, blk, is_sim, phy, found, logical_port, f_logical_port;
    int first_port, idx, core_num;
    uint32 *pAddr, *pPort, *pCoreNum;
    int mode;

    SOC_IF_ERROR_RETURN
        (_soc_ap_portctrl_device_addr_port_get(unit,
                                                 portmodDispatchTypePm12x10_xgs,
                                                 &pAddr, &pPort, &pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                           num_of_instances,
                                           &pm12x10_ap_user_acc[unit]));

    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                           num_of_instances*SOC_PM12X10_PM4X10_COUNT,
                                           &pm12_4x10_ap_user_acc[unit]));

    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                           num_of_instances,
                                           &pm12_4x25_ap_user_acc[unit]));


    /* Register PM12x10 */
    for (pmid = 0; PORTMOD_SUCCESS(rv) && (pmid < num_of_instances); pmid++) {
        int core_idx, core_blk;

        rv = portmod_pm_create_info_t_init(unit, &pm_info);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_info.pm_specific_info.pm12x10.refclk_source = 0;

        /* PM info */
        first_port = pPort[pmid];

        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+1);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+2);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+3);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+4);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+5);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+6);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+7);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+8);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+9);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+10);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+11);

        found = 0;
        for(idx = 0; idx < SOC_DRIVER(unit)->port_num_blktype; idx++){
            blk = SOC_PORT_IDX_INFO(unit, first_port, idx).blk;
            if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_CXXPORT) {
                found = 1;
                break;
            }
        }

        if (!found) {
             rv = SOC_E_INTERNAL;
             break;
        }
        pm_info.type         = portmodDispatchTypePm12x10_xgs;
        PORTMOD_PM12x10_F_USE_PM_XGS_SET(pm_info.pm_specific_info.pm12x10.flags);


        /* init user_acc for phymod access struct */
        rv = portmod_default_user_access_t_init(unit,
                                                &(pm12x10_ap_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm12x10_ap_user_acc[unit][pmid].unit = unit;
        pm12x10_ap_user_acc[unit][pmid].blk_id = blk;
        pm_info.pm_specific_info.pm12x10.blk_id = blk;
        pm12x10_ap_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");
        if (pm12x10_ap_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        for (core_num = 0;
             PORTMOD_SUCCESS(rv) && (core_num < SOC_PM12X10_PM4X10_COUNT);
             core_num++) {

            /* PM4X10 INFO UPDATE */
            pm4x10_info = &pm_info.pm_specific_info.pm12x10.pm4x10_infos[core_num];

            /* Specific info for PM12x10 */
            rv = phymod_access_t_init(&pm4x10_info->access);
            if (PORTMOD_FAILURE(rv)) {
                break;
            }
            pm12_4x10_ap_user_acc[unit][(pmid*3)+core_num] = pm12x10_ap_user_acc[unit][pmid];

            found = 0;
            for(core_idx = 0; core_idx < SOC_DRIVER(unit)->port_num_blktype; core_idx++){
                core_blk = SOC_PORT_IDX_INFO(unit, (first_port+(4*core_num)), core_idx).blk;
                if (SOC_BLOCK_INFO(unit, core_blk).type == SOC_BLK_XLPORT){
                    found = 1;
                    break;
                }
            }

            if (!found) {
                /* Internal Error */
                break;
            }

            pm12_4x10_ap_user_acc[unit][(pmid*3)+core_num].blk_id = core_blk;
            pm4x10_info->access.user_acc = &(pm12_4x10_ap_user_acc[unit][(pmid*3)+core_num]);
            pm4x10_info->access.addr     = pAddr[(pmid*3) + core_num];
            pm4x10_info->access.bus      = NULL;
            pm4x10_info->core_num        = pCoreNum? pCoreNum[(pmid*3)+core_num] : ((pmid*3)+core_num);
            pm4x10_info->core_num_int    = core_num;
            pm4x10_info->in_pm_12x10     = TRUE;
            pm4x10_info->rescal          = -1;

#if !defined (EXCLUDE_PHYMOD_TSCE_SIM) && defined (PHYMOD_TSCE_SUPPORT)
            rv = soc_physim_check_sim(unit, phymodDispatchTypeTsce,
                                      &(pm4x10_info->access), 0, &is_sim);
#else
            is_sim = 0;
#endif
            if (PORTMOD_FAILURE(rv)) {
                break;
            }

            if (is_sim) {
                pm4x10_info->access.bus->bus_capabilities |=
                    (PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL);

                /* Firmward loader : Don't allow FW load on sim enviroment */
                pm4x10_info->fw_load_method = phymodFirmwareLoadMethodNone;
            }

            /* Use default external loader */
            pm4x10_info->external_fw_loader = NULL;

            f_logical_port = SOC_INFO(unit).port_p2l_mapping[first_port];

            /* Three Port Mode */
            {
                phy = first_port+(core_num*4);
                logical_port = SOC_INFO(unit).port_p2l_mapping[phy];

                if (-1 != logical_port) {
                    SOC_IF_ERROR_RETURN(soc_apache_port_mode_get(unit, logical_port, &mode));
                    if (mode == portmodPortModeTri012 || mode == portmodPortModeTri023) {
                        pm4x10_info->port_mode_aux_info = portmodModeInfoThreePorts;
                    }
                }
            }

            if (core_num && (logical_port == -1)) {
                if (IS_CXX_PORT(unit, f_logical_port) && IS_CL_PORT(unit, f_logical_port)) {
                    logical_port = f_logical_port;
                }
            }
        }


        /* PM4X25 INFO UPDATE */
        pm4x25_info = &pm_info.pm_specific_info.pm12x10.pm4x25_info;

        /* Specific info for PM12x10 */
        rv = phymod_access_t_init(&pm4x25_info->access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }
        pm12_4x25_ap_user_acc[unit][pmid] = pm12x10_ap_user_acc[unit][pmid];
        found = 0;
        first_port = pPort[pmid];
        for(core_idx = 0; core_idx < SOC_DRIVER(unit)->port_num_blktype; core_idx++){
            core_blk = SOC_PORT_IDX_INFO(unit, first_port, core_idx).blk;
            if (SOC_BLOCK_INFO(unit, core_blk).type == SOC_BLK_CLPORT){
                found = 1;
                break;
            }
        }

        if (!found) {
            rv = SOC_E_INTERNAL;
            break;
        }

        pm12_4x25_ap_user_acc[unit][pmid].blk_id = core_blk;
        pm4x25_info->lane_map.num_of_lanes    = SOC_PM4X25_NUM_LANES;
        pm4x25_info->access.user_acc          = &(pm12_4x25_ap_user_acc[unit][pmid]);
        pm4x25_info->access.addr              = pAddr[pmid];
        pm4x25_info->access.bus               = NULL;
        pm4x25_info->core_num                 = pmid;
        pm4x25_info->external_fw_loader       = NULL;
        pm4x25_info->fw_load_method           = phymodFirmwareLoadMethodNone;
        pm4x25_info->in_pm_12x10              = TRUE;
        pm4x25_info->rescal                   = -1;

        
        rv  = portmod_port_macro_add(unit, &pm_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (pmid=0; pmid < num_of_instances; pmid++)  {
            if (pm12x10_ap_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm12x10_ap_user_acc[unit][pmid].mutex);
                pm12x10_ap_user_acc[unit][pmid].mutex = NULL;
            }
        }
    }
    return rv;
#else /* PORTMOD_PM12X10_XGS_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM12X10_XGS_SUPPORT */
}

/*
 * Function:
 *      soc_ap_portctrl_pm_portmod_init
 * Purpose:
 *      Initialize PortMod and PortMacro for Trident2Plus
 *      Add port macros (PM) to the unit's PortMod Manager (PMM).
 * Parameters:
 *      unit             - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_ap_portctrl_pm_portmod_init(int unit)
{
    uint32 flags = 0;
    int rv = SOC_E_NONE, pms_arr_len = 0, count, max_phys= 0;
    portmod_pm_instances_t *pm_types = NULL;

    /* Call PortMod library initialization */
    SOC_IF_ERROR_RETURN(
        _soc_ap_portctrl_pm_init(unit, &max_phys,
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

    PORTMOD_CREATE_F_PM_NULL_SET(flags);

    SOC_IF_ERROR_RETURN
        (portmod_create(unit, flags, SOC_MAX_NUM_PORTS, max_phys,
                        pms_arr_len, pm_types));

    for (count = 0; SOC_SUCCESS(rv) && (count < pms_arr_len); count++) {
#ifdef PORTMOD_PM4X10_SUPPORT
        if (pm_types[count].type == portmodDispatchTypePm4x10) {
            rv = _soc_ap_portctrl_pm4x10_portmod_init
                (unit, pm_types[count].instances);
        } else
#endif /* PORTMOD_PM4X10_SUPPORT  */
#ifdef PORTMOD_PM4X25_SUPPORT
        if (pm_types[count].type == portmodDispatchTypePm4x25) {
            rv = _soc_ap_portctrl_pm4x25_portmod_init
                (unit, pm_types[count].instances);
        } else
#endif /* PORTMOD_PM4X25_SUPPORT  */
#ifdef PORTMOD_PM12X10_XGS_SUPPORT
        if (pm_types[count].type == portmodDispatchTypePm12x10_xgs) {
            rv = _soc_ap_portctrl_pm12x10_portmod_init
                (unit, pm_types[count].instances);
        } else
#endif /* PORTMOD_PM12X10_XGS_SUPPORT  */
        {
            rv = SOC_E_INTERNAL;
        }
    }

    return rv;
}

/*
 * Function:
 *      soc_ap_portctrl_pm_portmod_deinit
 * Purpose:
 *      Deinitialize PortMod and PortMacro for Trident2Plus.
 *      Free pm user access data.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_NONE
 */
int
soc_ap_portctrl_pm_portmod_deinit(int unit)
{
    if (SOC_E_INIT == soc_esw_portctrl_init_check(unit)) {
        return SOC_E_NONE;
    }

    /* Free PMs structures */
#ifdef PORTMOD_PM4X10_SUPPORT
    if (pm4x10_ap_user_acc[unit] != NULL) {
        sal_free(pm4x10_ap_user_acc[unit]);
        pm4x10_ap_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM12X10_XGS_SUPPORT
    if (pm12x10_ap_user_acc[unit] != NULL) {
        sal_free(pm12x10_ap_user_acc[unit]);
        pm12x10_ap_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM12X10_XGS_SUPPORT */

#ifdef PORTMOD_PM4X25_SUPPORT
    if (pm4x25_ap_user_acc[unit] != NULL) {
        sal_free(pm4x25_ap_user_acc[unit]);
        pm4x25_ap_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM4X25_SUPPORT */

    SOC_IF_ERROR_RETURN(portmod_destroy(unit));

    return SOC_E_NONE;
}

int
soc_ap_portctrl_pm_port_config_get(int unit, soc_port_t port, void *port_config)
{
#ifdef PORTMOD_PM4X10_SUPPORT
    int rv = SOC_E_NONE;
    int pmid = 0, lane, phy, phy_port, logical_port, f_logical_port, port_mode;
    int first_port, core_num = 0, core_index, is_sim, type;
    uint32 txlane_map, rxlane_map, rxlane_map_l, i;
    uint32 *pAddr = NULL, *pPort = NULL, *pCoreNum = NULL;
    uint32 tx_polarity, rx_polarity;
    portmod_port_init_config_t *port_config_info;
    phymod_firmware_load_method_t fw_load_method;
    phymod_polarity_t polarity; /**< Lanes Polarity */
    phymod_lane_map_t lane_map;
    uint8_t pm_found=0;

    port_config_info = (portmod_port_init_config_t *)port_config;

    SOC_IF_ERROR_RETURN(soc_apache_port_mode_get(unit, port, &port_mode));
    if (port_mode == portmodPortModeTri012 || port_mode == portmodPortModeTri023) {
        port_config_info->port_mode_aux_info = portmodModeInfoThreePorts;
    }

    phy = SOC_INFO(unit).port_l2p_mapping[port];

    /* Find the port belong to which PM. The port has its own core id, using it to compare PM. */
    pm_found = 0;
    core_index = (phy - 1) / SOC_PM4X10_NUM_LANES;
    for (i=0; i<SOC_AP_PM4X10_COUNT; i++) {
        if (core_index == _ap_pm4x10_core_num[i]) {
            type = portmodDispatchTypePm4x10;
            SOC_IF_ERROR_RETURN
                (_soc_ap_portctrl_device_addr_port_get(unit,
                                                 portmodDispatchTypePm4x10,
                                                 &pAddr, &pPort, &pCoreNum));
            pmid = i;
            core_num = 1;
            pm_found = 1;
            break;
        }
    }
    /* When port dones't find PM4x10, find PM4x25 again. */
    if (!pm_found) {
        for (i=0; i<SOC_AP_PM4X25_COUNT; i++) {
            if (core_index == _ap_pm4x25_core_num[i]) {
                type = portmodDispatchTypePm4x25;
                SOC_IF_ERROR_RETURN
                    (_soc_ap_portctrl_device_addr_port_get(unit,
                                                 portmodDispatchTypePm4x25,
                                                 &pAddr, &pPort, &pCoreNum));
                /* Because one PM4x25 has three cores and one corresponding pmid. */
                pmid = i;
                core_num = 1;
                pm_found = 1;
                break;
            }
        }
    }

    /* When port dones't find PM4x25, find PM12x10 again. */
    if (!pm_found) {
        for (i=0; i<SOC_AP_PM12X10_COUNT*SOC_PM12X10_PM4X10_COUNT; i++) {
            if (core_index == _ap_pm12x10_core_num[i]) {
                type = portmodDispatchTypePm12x10_xgs;
                SOC_IF_ERROR_RETURN
                    (_soc_ap_portctrl_device_addr_port_get(unit,
                                                 portmodDispatchTypePm12x10_xgs,
                                                 &pAddr, &pPort, &pCoreNum));
                /* Because one PM12x10 has three cores and one corresponding pmid. */
                pmid = i / SOC_PM12X10_PM4X10_COUNT;
                core_num = SOC_PM12X10_PM4X10_COUNT;
                pm_found = 1;
                break;
            }
        }
    }

    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* PM info */
    first_port = pPort[pmid];

    for (core_index = 0; core_index < core_num; core_index++) {
        fw_load_method = phymodFirmwareLoadMethodExternal;

        soc_physim_enable_get(unit, &is_sim);

        if (is_sim) {
            /* Firmward loader : Don't allow FW load on sim enviroment */
            fw_load_method = phymodFirmwareLoadMethodNone;
        } else {
            fw_load_method = soc_property_suffix_num_get(unit, pmid,
                                            spn_LOAD_FIRMWARE, "quad",
                                            phymodFirmwareLoadMethodExternal);
            fw_load_method &= 0xff;
        }

        rv = phymod_polarity_t_init(&polarity);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        f_logical_port = SOC_INFO(unit).port_p2l_mapping[first_port];

        /* Polarity */
        for (lane = 0; lane < SOC_PM4X10_NUM_LANES; lane++) {
            phy_port = first_port + lane + (core_index*4);
            logical_port = SOC_INFO(unit).port_p2l_mapping[phy_port];

            /* in case of 100G - there is no logical port for core1, core2       */
            /* so if the logical port is -1, check 100G and use core0            */
            /* This could be a 100G port. so mapping is available for core0 only */
            if (core_index && (logical_port == -1)) {
                if (IS_CXX_PORT(unit, f_logical_port) && IS_CL_PORT(unit, f_logical_port)) {
                    logical_port = f_logical_port;
                }
            }

            apache_pm_port_polarity_get(unit, logical_port, phy_port, lane, core_index,  &tx_polarity, &rx_polarity);

            polarity.rx_polarity |= rx_polarity;
            polarity.tx_polarity |= tx_polarity;
        }

        /* Lane map */
        rv = phymod_lane_map_t_init(&lane_map);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        phy_port = first_port + (core_index*4);
        logical_port = SOC_INFO(unit).port_p2l_mapping[phy_port];

        if (core_index && (logical_port == -1)) {
            if (IS_CXX_PORT(unit, f_logical_port) && IS_CL_PORT(unit, f_logical_port)) {
                logical_port = f_logical_port;
            }
        }

        apache_pm_port_lanemap_get(unit, logical_port, phy_port, core_index, &txlane_map, &rxlane_map);
        /* Handle package level polarity flips and lane swaps */
        if (!SAL_BOOT_SIMULATION) {
            _soc_apache_portctrl_pkg_config_set(unit, phy_port, &rx_polarity, &tx_polarity,
                                                &rxlane_map, &txlane_map);
        }

        polarity.rx_polarity |= rx_polarity;
        polarity.tx_polarity |= tx_polarity;

        rxlane_map_l = rxlane_map ;
        /* For TSC-E/F family, both lane_map_rx and lane_map_tx are logic lane base */
        /* TD2/TD2+ xgxs_rx_lane_map is phy-lane base */
        if(SOC_IS_APACHE(unit)) { /* use TD2 legacy notion system */
            rxlane_map_l = 0 ;
            for (i=0; i<SOC_PM4X10_NUM_LANES; i++) {
                rxlane_map_l |= i << SOC_PM4X10_NUM_LANES *((rxlane_map >> (i*SOC_PM4X10_NUM_LANES))& SOC_PM4X10_LANE_MASK) ;
            }
        }

        if (type == portmodDispatchTypePm4x25) {
            lane_map.num_of_lanes = SOC_PM4X25_NUM_LANES;
        } else {
            lane_map.num_of_lanes = SOC_PM4X10_NUM_LANES;
        }

        for(lane = 0 ; lane < SOC_PM4X10_NUM_LANES; lane++) {
            lane_map.lane_map_tx[lane] =
                (txlane_map >> (lane * SOC_PM4X10_NUM_LANES)) &
                SOC_PM4X10_LANE_MASK;
            lane_map.lane_map_rx[lane] =
                (rxlane_map >> (lane * SOC_PM4X10_NUM_LANES)) &
                SOC_PM4X10_LANE_MASK;
        }

        sal_memcpy(&port_config_info->fw_load_method[core_index], &fw_load_method,
                    sizeof(phymod_firmware_load_method_t));
        port_config_info->fw_load_method_overwrite = 1;
        sal_memcpy(&port_config_info->polarity[core_index], &polarity,
                    sizeof(phymod_polarity_t));
        port_config_info->polarity_overwrite = 1;
        sal_memcpy(&port_config_info->lane_map[core_index], &lane_map,
                    sizeof(phymod_lane_map_t));
        port_config_info->lane_map_overwrite = 1;
    }

    return rv;
#else /* PORTMOD_PM4X10_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X10_SUPPORT */
}

int
soc_ap_portctrl_pm_port_phyaddr_get(int unit, soc_port_t port)
{
    int phy, core_index;
    uint32 i, *pAddr = NULL, *pPort = NULL, *pCoreNum = NULL;

    phy = SOC_INFO(unit).port_l2p_mapping[port];

    /* Find the port belong to which PM. The port has its own core id, using it to compare PM. */
    core_index = (phy - 1) / SOC_PM4X10_NUM_LANES;
#ifdef PORTMOD_PM4X10_SUPPORT
    for (i=0; i<SOC_AP_PM4X10_COUNT; i++) {
        if (core_index == _ap_pm4x10_core_num[i]) {
            SOC_IF_ERROR_RETURN
                (_soc_ap_portctrl_device_addr_port_get(unit,
                                             portmodDispatchTypePm4x10,
                                             &pAddr, &pPort, &pCoreNum));
            return pAddr[i];
        }
    }
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM4X25_SUPPORT
    /* When port dones't find PM4x10, find PM4x25 again. */
    for (i=0; i<SOC_AP_PM4X25_COUNT; i++) {
        if (core_index == _ap_pm4x25_core_num[i]) {
            SOC_IF_ERROR_RETURN
                (_soc_ap_portctrl_device_addr_port_get(unit,
                                             portmodDispatchTypePm4x25,
                                             &pAddr, &pPort, &pCoreNum));
            return pAddr[i];
        }
    }
#endif /* PORTMOD_PM4X25_SUPPORT */

#ifdef PORTMOD_PM12X10_XGS_SUPPORT
    /* When port dones't find PM4x25, find PM12x10 again. */
    for (i=0; i<SOC_AP_PM12X10_COUNT*SOC_PM12X10_PM4X10_COUNT; i++) {
        if (core_index == _ap_pm12x10_core_num[i]) {
            SOC_IF_ERROR_RETURN
                (_soc_ap_portctrl_device_addr_port_get(unit,
                                             portmodDispatchTypePm12x10_xgs,
                                             &pAddr, &pPort, &pCoreNum));
            return pAddr[i];
        }
    }
#endif /* PORTMOD_PM12X10_XGS_SUPPORT */

    return -1;
}

int
soc_ap_portctrl_port_ability_update(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int phy_port, tsc_id;
    uint32 force_hg;

    if (!soc_feature(unit, soc_feature_untethered_otp)) {
        return SOC_E_NONE;
    }
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    /* Each bit in FORCE_HG bond option represents one TSC/Port Macro
    * 1 - All ports in specified TSC are Higig only.
    * 0 - No restrictions
    */
    force_hg = SOC_BOND_INFO(unit)->tsc_force_hg;
    tsc_id = (phy_port - 1) / SOC_AP_MAX_PORTS_PER_TSC;

    if (force_hg & (1 << tsc_id)) {
        ability->encap &= ~SOC_PA_ENCAP_IEEE;
    }
    return SOC_E_NONE;
}

int
soc_ap_portctrl_pm_type_get(int unit, soc_port_t phy_port,
                            int* pm_type)
{
    if ((phy_port < 0) || (phy_port > 72)) {
        return SOC_E_PARAM;
    }

#if defined(PORTMOD_PM4X10_SUPPORT) && defined (PORTMOD_PM4X25_SUPPORT)
    if (PORT_IS_PM4x25_PORT_PHY(unit, phy_port)) {
        *pm_type = portmodDispatchTypePm4x25;
    } else {
        *pm_type = portmodDispatchTypePm4x10;
    }
#else
    *pm_type = -1;
#endif

    return ((*pm_type == -1) ? SOC_E_UNAVAIL : SOC_E_NONE);
}

#endif /* BCM_APACHE_SUPPORT */
