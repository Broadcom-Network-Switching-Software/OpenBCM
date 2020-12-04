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
#include <soc/helix5.h>
#include <shared/bsl.h>
#include <soc/phy/phymod_sim.h>
#include <soc/portmod/portmod.h>

#ifdef BCM_HELIX5_SUPPORT

#define SOC_HX5_PM4X10_COUNT         4
#define SOC_HX5_PM4X10_QTC_COUNT     3
#define SOC_HX5_PM4X25_COUNT         3

static uint32_t _hx5_pm4x10_addr[SOC_HX5_PM4X10_COUNT] = {
    0xA1, 0xA5, 0xA9, 0xAD
};

static uint32_t _hx5_pm4x10_core_num[SOC_HX5_PM4X10_COUNT] = {
    3, 4, 5, 6
};

static uint32_t _hx5_pm4x10_port[SOC_HX5_PM4X10_COUNT] = {
    49, 53, 57, 61
};
#ifdef PORTMOD_PM4X10_QTC_SUPPORT
static uint32_t _hx5_pm4x10q_addr[SOC_HX5_PM4X10_QTC_COUNT] = {
    0x81, /* PM4x10Q Inst 0 */
    0x85, /* PM4x10Q Inst 1 */
    0x89  /* PM4x10Q Inst 2 */
};

static uint32_t _hx5_pm4x10q_core_num[SOC_HX5_PM4X10_QTC_COUNT] = {
    0, 1, 2
};

static uint32_t _hx5_pm4x10q_port[SOC_HX5_PM4X10_QTC_COUNT] = {
    1, 17, 33
};
#endif

static uint32_t _hx5_pm4x25_addr[SOC_HX5_PM4X25_COUNT] = {
    0xC1, 0xC5, 0xC9
};

static uint32_t _hx5_pm4x25_core_num[SOC_HX5_PM4X25_COUNT] = {
    7, 8, 9
};

static uint32_t _hx5_pm4x25_port[SOC_HX5_PM4X25_COUNT] = {
    65, 69, 73
};


#define SOC_HX5_MAX_PHYS            88    /*  48 GXPORTS + 28 XLPORTS + 12 CLPORTS */
#define SOC_HX5_SUB_PHYS_IN_QSGMII      4

static portmod_pm_instances_t hx5_pm_types[] = {
#ifdef PORTMOD_PM4X10_SUPPORT
        {portmodDispatchTypePm4x10,  SOC_HX5_PM4X10_COUNT},
#endif
#ifdef PORTMOD_PM4X25_SUPPORT
        {portmodDispatchTypePm4x25,  SOC_HX5_PM4X25_COUNT},
#endif
#ifdef PORTMOD_PM4X10_QTC_SUPPORT
        {portmodDispatchTypePm4x10_qtc, SOC_HX5_PM4X10_QTC_COUNT},
#endif
};

#ifdef PORTMOD_PM4X10_SUPPORT
static portmod_default_user_access_t *pm4x10_hx5_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM4X10_QTC_SUPPORT
static portmod_default_user_access_t *pm4x10q_hx5_user_acc[SOC_MAX_NUM_DEVICES];
static portmod_default_user_access_t *pm4x10q_4x10_hx5_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM4X10_QTC_SUPPORT */

#ifdef PORTMOD_PM4X25_SUPPORT
static portmod_default_user_access_t *pm4x25_hx5_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM4X25_SUPPORT */

#define SOC_HX5_CORE_FREQ_KHZ_DEFAULT 50000  /* HX5 default core freq is 50MHz*/

extern int soc_hx5_portctrl_pm_portmod_init(int unit);
extern int soc_hx5_portctrl_pm_portmod_deinit(int unit);
extern int soc_hx5_portctrl_pm_port_config_get(int unit, soc_port_t port, void *port_config_info);
extern int soc_hx5_portctrl_pm_port_phyaddr_get(int unit, soc_port_t port);
extern int soc_hx5_portctrl_pm_type_get(int unit, soc_port_t phy_port, int* pm_type);

soc_portctrl_functions_t soc_hx5_portctrl_func = {
    soc_hx5_portctrl_pm_portmod_init,
    soc_hx5_portctrl_pm_portmod_deinit,
    soc_hx5_portctrl_pm_port_config_get,
    soc_hx5_portctrl_pm_port_phyaddr_get,
    NULL /* soc_hx5_portctrl_port_ability_update*/,
    soc_hx5_portctrl_pm_type_get,
    NULL,
    NULL
};

STATIC int
_soc_hx5_portctrl_pm_init(int unit, int *max_phys,
                           portmod_pm_instances_t **pm_types,
                           int *pms_arr_len)
{
    *max_phys = SOC_HX5_MAX_PHYS;
    *pm_types = hx5_pm_types;
    *pms_arr_len = (sizeof(hx5_pm_types) / sizeof(portmod_pm_instances_t));

    return SOC_E_NONE;
}

STATIC int
_soc_hx5_portctrl_device_addr_port_get(int unit, int pm_type,
                                        uint32_t **addr,
                                        uint32_t **port,
                                        uint32_t **core_num)
{
    *addr     = NULL;
    *port     = NULL;
    *core_num = NULL;

    if (pm_type == portmodDispatchTypePm4x10) {
        *addr = _hx5_pm4x10_addr;
        *port = _hx5_pm4x10_port;
        *core_num = _hx5_pm4x10_core_num;
    } else if (pm_type == portmodDispatchTypePm4x25){
        *addr = _hx5_pm4x25_addr;
        *port = _hx5_pm4x25_port;
        *core_num = _hx5_pm4x25_core_num;
#ifdef PORTMOD_PM4X10_QTC_SUPPORT
    } else if (pm_type == portmodDispatchTypePm4x10_qtc){
        *addr = _hx5_pm4x10q_addr;
        *port = _hx5_pm4x10q_port;
        *core_num = _hx5_pm4x10q_core_num;
#endif
    }

    return SOC_E_NONE;
}

STATIC int
_soc_hx5_portctrl_pm_user_acc_flag_set(int unit,
                    portmod_default_user_access_t* pm_user_access)
{
    return SOC_E_NONE;
}

/*
 * Polarity get looks for physical port based config first. This will enable
 * the user to define the polarity for all ports weather mapped to logical 
 * port or not.
 */
STATIC
void helix5_pm_port_polarity_get(int unit, int physical_port, int lane,
                                 uint32 *tx_pol, uint32* rx_pol)
{
    uint32 rx_polarity, tx_polarity;

    rx_polarity = soc_property_phy_get (unit, physical_port, 0, 0, lane,
                     spn_PHY_CHAIN_RX_POLARITY_FLIP_PHYSICAL, 0x0);
    *rx_pol = (rx_polarity & 0x1) << lane;

    tx_polarity = soc_property_phy_get (unit, physical_port, 0, 0, lane,
                     spn_PHY_CHAIN_TX_POLARITY_FLIP_PHYSICAL, 0x0);
    *tx_pol = (tx_polarity & 0x1) << lane;
}

/*
 * lanemap get looks for physical port based config first. This will enable
 * the user to define the lanemap for all ports weather mapped to logical 
 * port or not.
 */
STATIC 
void helix5_pm_port_lanemap_get (int unit, int physical_port,
                                 uint32 *txlane_map, uint32* rxlane_map)
{

    *rxlane_map = soc_property_phy_get (unit, physical_port, 0, 0, 0, 
                                        spn_PHY_CHAIN_RX_LANE_MAP_PHYSICAL, 0x3210);


    *txlane_map = soc_property_phy_get (unit, physical_port, 0, 0, 0, 
                                        spn_PHY_CHAIN_TX_LANE_MAP_PHYSICAL, 0x3210);

}


STATIC int
_soc_hx5_portctrl_pm4x10_portmod_init(int unit, int num_of_instances)
{
#ifdef PORTMOD_PM4X10_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm_info;
    int pmid = 0, blk, is_sim, found;
    int first_port, idx, core_num, core_cnt;
    uint32 *pAddr, *pPort, *pCoreNum;


    SOC_IF_ERROR_RETURN
        (_soc_hx5_portctrl_device_addr_port_get(unit,
                                               portmodDispatchTypePm4x10,
                                               &pAddr, &pPort, &pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* Allocate PMs */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                           num_of_instances,
                                           &pm4x10_hx5_user_acc[unit]));

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
            if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_XLPORT) {
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
                                                &(pm4x10_hx5_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        SOC_IF_ERROR_RETURN(
            _soc_hx5_portctrl_pm_user_acc_flag_set(unit,
                &(pm4x10_hx5_user_acc[unit][pmid])));

        PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(pm4x10_hx5_user_acc[unit][pmid])));
        pm4x10_hx5_user_acc[unit][pmid].unit = unit;
        pm4x10_hx5_user_acc[unit][pmid].blk_id = blk;
        pm4x10_hx5_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");
        if (pm4x10_hx5_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        /* Specific info for PM4x10 */
        rv = phymod_access_t_init(&pm_info.pm_specific_info.pm4x10.access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_info.pm_specific_info.pm4x10.access.user_acc =
            &(pm4x10_hx5_user_acc[unit][pmid]);
        pm_info.pm_specific_info.pm4x10.access.addr     = pAddr[pmid];
        pm_info.pm_specific_info.pm4x10.access.bus      = NULL; /* Use default bus */
        pm_info.pm_specific_info.pm4x10.lane_map.num_of_lanes              = SOC_PM4X10_NUM_LANES;
        pm_info.pm_specific_info.pm4x10.rescal                             = -1;
#if !defined (EXCLUDE_PHYMOD_TSCE16_SIM) && defined (PHYMOD_TSCE16_SUPPORT)
        rv = soc_physim_check_sim(unit, phymodDispatchTypeTsce16,
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

        core_cnt = 1;

        for (core_num = 0; core_num < core_cnt; core_num++) {
            pm_info.pm_specific_info.pm4x10.core_num = pCoreNum? pCoreNum[pmid] : pmid;
            pm_info.pm_specific_info.pm4x10.core_num_int = 0;
        }

        /* Add PM to PortMod */
        rv = portmod_port_macro_add(unit, &pm_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (pmid=0; pmid < num_of_instances; pmid++) {
            if (pm4x10_hx5_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm4x10_hx5_user_acc[unit][pmid].mutex);
                pm4x10_hx5_user_acc[unit][pmid].mutex = NULL;
            }
        }
    }

    return rv;
#else /* PORTMOD_PM4X10_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X10_SUPPORT */
}

STATIC int
_soc_hx5_portctrl_pm4x10_qtc_pm_qtc_init(int unit, int instance_id, portmod_pm_qtc_create_info_t *pm_qtc_info)
{
#ifdef PORTMOD_PM4X10_QTC_SUPPORT
    int rv = SOC_E_NONE;
    int blk, is_sim, found;
    int blktype;
    uint32 *pAddr, *pPort, *pCoreNum;

    SOC_IF_ERROR_RETURN
        (_soc_hx5_portctrl_device_addr_port_get(unit,
                                               portmodDispatchTypePm4x10_qtc,
                                               &pAddr, &pPort, &pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* Pm_qtc information */
    found = 0;
    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++) {
        blktype = SOC_BLOCK_INFO(unit, blk).type;
        if ((blktype == SOC_BLK_PMQPORT) && (SOC_BLOCK_INFO(unit, blk).number == instance_id)) {
            found = 1;
            break;
        }
    }

    if (!found) {
        return SOC_E_INTERNAL;
    }

    /* Init user_acc for phymod access struct */
    rv = portmod_default_user_access_t_init(unit,
                                            &(pm4x10q_hx5_user_acc[unit][instance_id]));
    if (PORTMOD_FAILURE(rv)) {
        return rv;
    }

    /* for Qmode.  */
    pm4x10q_hx5_user_acc[unit][instance_id].unit = unit;
    pm4x10q_hx5_user_acc[unit][instance_id].blk_id = blk;
    pm4x10q_hx5_user_acc[unit][instance_id].mutex = sal_mutex_create("core mutex");
    if (pm4x10q_hx5_user_acc[unit][instance_id].mutex == NULL) {
        return SOC_E_MEMORY;
    }

    /* Specific info for PM_QTC */
    rv = phymod_access_t_init(&pm_qtc_info->access);
    if (PORTMOD_FAILURE(rv)) {
        return rv;
    }
    PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(pm4x10q_hx5_user_acc[unit][instance_id])));
    pm_qtc_info->access.user_acc = &pm4x10q_hx5_user_acc[unit][instance_id];
    pm_qtc_info->access.addr     = pAddr[instance_id];
    pm_qtc_info->access.bus      = NULL;
    pm_qtc_info->lane_map.num_of_lanes = SOC_PM4X10_QTC_NUM_LANES;
    pm_qtc_info->core_num        = pCoreNum[instance_id];
    pm_qtc_info->core_clock_khz  = SOC_HX5_CORE_FREQ_KHZ_DEFAULT;

#if !defined (EXCLUDE_PHYMOD_QTCE16_SIM) && defined (PHYMOD_QTCE16_SUPPORT)
    rv = soc_physim_check_sim(unit, phymodDispatchTypeQtce16,
                              &(pm_qtc_info->access),
                              0, &is_sim);
#else
    is_sim = 0;
#endif
    if (PORTMOD_FAILURE(rv)) {
        return rv;
    }

    if (is_sim) {
        pm_qtc_info->access.bus->bus_capabilities |=
            (PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL);

        /* Firmward loader : Don't allow FW load on sim enviroment */
        pm_qtc_info->fw_load_method = phymodFirmwareLoadMethodNone;
    }

    /* Use default external loader */
    pm_qtc_info->external_fw_loader = NULL;

    return rv;
#else /* PORTMOD_PM4X10_QTC_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X10_QTC_SUPPORT */
}

STATIC int
_soc_hx5_portctrl_pm4x10_qtc_pm4x10_init(int unit, int instance_id, portmod_pm4x10_create_info_t *pm4x10_info)
{
#ifdef PORTMOD_PM4X10_QTC_SUPPORT
    int rv = SOC_E_NONE;
    int blk, is_sim, found;
    int first_port, idx;
    uint32 *pAddr, *pPort, *pCoreNum;

    SOC_IF_ERROR_RETURN
        (_soc_hx5_portctrl_device_addr_port_get(unit,
                                               portmodDispatchTypePm4x10_qtc,
                                               &pAddr, &pPort, &pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    first_port = pPort[instance_id];

    /* Pm4x10 information */
    found = 0;
    for (idx = 0; idx < SOC_DRIVER(unit)->port_num_blktype; idx++) {
        blk = SOC_PORT_IDX_INFO(unit, first_port , idx).blk;
        if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_XLPORT) {
            found = 1;
            break;
        }
    }

    if (!found) {
        return SOC_E_INTERNAL;
    }

    /* Init user_acc for phymod access struct */
    rv = portmod_default_user_access_t_init(unit,
                                            &(pm4x10q_4x10_hx5_user_acc[unit][instance_id]));
    if (PORTMOD_FAILURE(rv)) {
        return rv;
    }
    pm4x10q_4x10_hx5_user_acc[unit][instance_id].unit = unit;
    pm4x10q_4x10_hx5_user_acc[unit][instance_id].blk_id = blk;
    pm4x10q_4x10_hx5_user_acc[unit][instance_id].mutex = sal_mutex_create("core mutex");
    if (pm4x10q_4x10_hx5_user_acc[unit][instance_id].mutex == NULL) {
        return SOC_E_MEMORY;
    }

    /* Specific info for PM4x10 */
    rv = phymod_access_t_init(&pm4x10_info->access);
    if (PORTMOD_FAILURE(rv)) {
        return rv;
    }
    PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(pm4x10q_4x10_hx5_user_acc[unit][instance_id])));
    pm4x10_info->access.user_acc = &pm4x10q_4x10_hx5_user_acc[unit][instance_id];
    pm4x10_info->access.addr     = pAddr[instance_id];
    pm4x10_info->access.bus      = NULL;
    pm4x10_info->lane_map.num_of_lanes = SOC_PM4X10_NUM_LANES;
    pm4x10_info->core_num        = pCoreNum[instance_id];
    pm4x10_info->core_num_int    = 0;
    pm4x10_info->rescal          = -1;
#if !defined (EXCLUDE_PHYMOD_TSCE16_SIM) && defined (PHYMOD_TSCE16_SUPPORT)
    rv = soc_physim_check_sim(unit, phymodDispatchTypeTsce16,
                              &(pm4x10_info->access),
                              2, &is_sim);
#else
    is_sim = 0;
#endif
    if (PORTMOD_FAILURE(rv)) {
        return rv;
    }

    if (is_sim) {
        pm4x10_info->access.bus->bus_capabilities |=
            (PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL);

        /* Firmward loader : Don't allow FW load on sim enviroment */
        pm4x10_info->fw_load_method =
            phymodFirmwareLoadMethodNone;
    }
    return rv;
#else /* PORTMOD_PM4X10_QTC_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X10_QTC_SUPPORT */
}

STATIC int
_soc_hx5_portctrl_pm4x10_qtc_portmod_init(int unit, int num_of_instances)
{
#ifdef PORTMOD_PM4X10_QTC_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm4x10_qtc_info;
    int instance_id = 0;
    int first_port;
    uint32 *pAddr, *pPort, *pCoreNum;

    SOC_IF_ERROR_RETURN
        (_soc_hx5_portctrl_device_addr_port_get(unit,
                                               portmodDispatchTypePm4x10_qtc,
                                               &pAddr, &pPort, &pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* Allocate PMs */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit, num_of_instances,
                                               &pm4x10q_hx5_user_acc[unit]));
    /* Allocate PMs */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit, num_of_instances,
                                               &pm4x10q_4x10_hx5_user_acc[unit]));

    for (instance_id = 0; PORTMOD_SUCCESS(rv) && (instance_id < num_of_instances); instance_id++) {
        /* Pm4x10_qtc information*/
        rv = portmod_pm_create_info_t_init(unit, &pm4x10_qtc_info);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }
        /* PM info */
        first_port = pPort[instance_id];

        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+1);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+2);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+3);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+4);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+5);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+6);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+7);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+8);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+9);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+10);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+11);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+12);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+13);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+14);
        PORTMOD_PBMP_PORT_ADD(pm4x10_qtc_info.phys, first_port+15);

        pm4x10_qtc_info.type         = portmodDispatchTypePm4x10_qtc;

        /*pm_qtc infomration*/
        rv = _soc_hx5_portctrl_pm4x10_qtc_pm_qtc_init(unit, instance_id,
                                &pm4x10_qtc_info.pm_specific_info.pm4x10_qtc.pm_qtc_info);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        /*pm4x10 infomration*/
        rv = _soc_hx5_portctrl_pm4x10_qtc_pm4x10_init(unit, instance_id,
                                &pm4x10_qtc_info.pm_specific_info.pm4x10_qtc.pm4x10_info);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        /* Add PM_QTC to PortMod */
        rv = portmod_port_macro_add(unit, &pm4x10_qtc_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (instance_id=0; instance_id < num_of_instances; instance_id++) {
            if (pm4x10q_hx5_user_acc[unit][instance_id].mutex) {
                sal_mutex_destroy(pm4x10q_hx5_user_acc[unit][instance_id].mutex);
                pm4x10q_hx5_user_acc[unit][instance_id].mutex = NULL;
            }
       
            if (pm4x10q_4x10_hx5_user_acc[unit][instance_id].mutex) {
                sal_mutex_destroy(pm4x10q_4x10_hx5_user_acc[unit][instance_id].mutex);
                pm4x10q_4x10_hx5_user_acc[unit][instance_id].mutex = NULL;
            }
        }
    }

    return rv;
#else /* PORTMOD_PM4X10_QTC_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X10_QTC_SUPPORT */
}

STATIC int
_soc_hx5_portctrl_pm4x25_portmod_init(int unit, int num_of_instances)
{
#ifdef PORTMOD_PM4X25_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm_info;
    int pmid = 0, blk, is_sim, found;
    int first_port, idx, core_num, core_cnt; 
    uint32 *pAddr, *pPort,*pCoreNum;

    SOC_IF_ERROR_RETURN
        (_soc_hx5_portctrl_device_addr_port_get(unit,
                                                 portmodDispatchTypePm4x25,
                                                 &pAddr, &pPort,&pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* Allocate PMs */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                           num_of_instances,
                                           &pm4x25_hx5_user_acc[unit]));

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
            if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_CLPORT) {
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
                                                &(pm4x25_hx5_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(pm4x25_hx5_user_acc[unit][pmid])));
        pm4x25_hx5_user_acc[unit][pmid].unit = unit;
        pm4x25_hx5_user_acc[unit][pmid].blk_id = blk;
        pm4x25_hx5_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");
        if (pm4x25_hx5_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        /* Specific info for PM4x25 */
        rv = phymod_access_t_init(&pm_info.pm_specific_info.pm4x25.access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_info.pm_specific_info.pm4x25.access.user_acc =
            &(pm4x25_hx5_user_acc[unit][pmid]);
        pm_info.pm_specific_info.pm4x25.access.addr     = pAddr[pmid];
        pm_info.pm_specific_info.pm4x25.access.bus      = NULL; /* Use default bus */
        pm_info.pm_specific_info.pm4x25.lane_map.num_of_lanes              = SOC_PM4X10_NUM_LANES;
        pm_info.pm_specific_info.pm4x25.rescal                             = -1;

#if !defined (EXCLUDE_PHYMOD_TSCF16_SIM) && defined (PHYMOD_TSCF16_SUPPORT)
        rv = soc_physim_check_sim(unit, phymodDispatchTypeTscf16,
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

        core_cnt = 1;

        for (core_num = 0; core_num < core_cnt; core_num++) {
            pm_info.pm_specific_info.pm4x25.core_num = pCoreNum? pCoreNum[pmid] : pmid;
            pm_info.pm_specific_info.pm4x25.core_num_int = 0;
        }

        /* Add PM to PortMod */
        rv = portmod_port_macro_add(unit, &pm_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (pmid=0; pmid < num_of_instances; pmid++) {
            if (pm4x25_hx5_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm4x25_hx5_user_acc[unit][pmid].mutex);
                pm4x25_hx5_user_acc[unit][pmid].mutex = NULL;
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
 *      soc_hx5_portctrl_pm_portmod_init
 * Purpose:
 *      Initialize PortMod and PortMacro for Trident2Plus
 *      Add port macros (PM) to the unit's PortMod Manager (PMM).
 * Parameters:
 *      unit             - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_hx5_portctrl_pm_portmod_init(int unit)
{
    uint32 flags = 0;
    int rv = SOC_E_NONE, pms_arr_len = 0, count, max_phys= 0;
    portmod_pm_instances_t *pm_types = NULL;

    /* Call PortMod library initialization */
    SOC_IF_ERROR_RETURN(
        _soc_hx5_portctrl_pm_init(unit, &max_phys,
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
            rv = _soc_hx5_portctrl_pm4x10_portmod_init
                (unit, pm_types[count].instances);
        } else
#endif /* PORTMOD_PM4X10_SUPPORT  */
#ifdef PORTMOD_PM4X10_QTC_SUPPORT
        if (pm_types[count].type == portmodDispatchTypePm4x10_qtc) {
            rv = _soc_hx5_portctrl_pm4x10_qtc_portmod_init
                (unit, pm_types[count].instances);
        } else
#endif /* PORTMOD_PM4X10_QTC_SUPPORT  */
#ifdef PORTMOD_PM4X25_SUPPORT
        if (pm_types[count].type == portmodDispatchTypePm4x25) {
            rv = _soc_hx5_portctrl_pm4x25_portmod_init
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
 *      soc_hx5_portctrl_pm_portmod_deinit
 * Purpose:
 *      Deinitialize PortMod and PortMacro for Trident2Plus.
 *      Free pm user access data.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_NONE
 */
int
soc_hx5_portctrl_pm_portmod_deinit(int unit)
{
    if (SOC_E_INIT == soc_esw_portctrl_init_check(unit)) {
        return SOC_E_NONE;
    }

    /* Free PMs structures */
#ifdef PORTMOD_PM4X10_SUPPORT
    if (pm4x10_hx5_user_acc[unit] != NULL) {
        sal_free(pm4x10_hx5_user_acc[unit]);
        pm4x10_hx5_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM4X10_SUPPORT */

    /* Free PMs structures */
#ifdef PORTMOD_PM4X10_QTC_SUPPORT
    if (pm4x10q_hx5_user_acc[unit] != NULL) {
        sal_free(pm4x10q_hx5_user_acc[unit]);
        pm4x10q_hx5_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM4X25_SUPPORT
    if (pm4x25_hx5_user_acc[unit] != NULL) {
        sal_free(pm4x25_hx5_user_acc[unit]);
        pm4x25_hx5_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM4X25_SUPPORT */

    SOC_IF_ERROR_RETURN(portmod_destroy(unit));

    return SOC_E_NONE;
}

int
soc_hx5_portctrl_pm_port_config_get(int unit, soc_port_t port, void *port_config)
{
#ifdef PORTMOD_PM4X10_SUPPORT
    int pmid = 0, lane, phy_port;
    int first_port, core_index, is_sim, type;
    uint32 txlane_map, rxlane_map, i;
    uint32 *pAddr = NULL, *pPort = NULL, *pCoreNum = NULL;
    uint32 tx_polarity, rx_polarity;
    portmod_port_init_config_t *port_config_info;
    phymod_firmware_load_method_t fw_load_method;
    phymod_polarity_t polarity; /**< Lanes Polarity */
    phymod_lane_map_t lane_map;
    uint8_t pm_found=0;
    int max_index;

    port_config_info = (portmod_port_init_config_t *)port_config;

    /* Find the port belong to which PM. The port has its own core id, using it to compare PM. */
    pm_found = 0;
    core_index = SOC_INFO(unit).port_serdes[port];

#ifdef PORTMOD_PM4X10_QTC_SUPPORT
    for (i=0; i<SOC_HX5_PM4X10_QTC_COUNT; i++) {
        if (core_index == _hx5_pm4x10q_core_num[i]) {
            type = portmodDispatchTypePm4x10_qtc;
            SOC_IF_ERROR_RETURN
                (_soc_hx5_portctrl_device_addr_port_get(unit, type,
                                             &pAddr, &pPort, &pCoreNum));
            pmid = i;
            pm_found = 1;
            break;
        }
    }
#endif /* PORTMOD_PM4X10_QTC_SUPPORT */

    if (!pm_found) {
        /* When port dones't find PM4x10Q, find PM4x10 again. */
        for (i=0; i<SOC_HX5_PM4X10_COUNT; i++) {
            if (core_index == _hx5_pm4x10_core_num[i]) {
                type = portmodDispatchTypePm4x10;
                SOC_IF_ERROR_RETURN
                    (_soc_hx5_portctrl_device_addr_port_get(unit, type,
                                                &pAddr, &pPort, &pCoreNum));
                pmid = i;
                pm_found = 1;
                break;
            }
        }
    }

    /* When port dones't find PM4x10, find PM4x25 again. */
    if (!pm_found) {
        for (i=0; i<SOC_HX5_PM4X25_COUNT; i++) {
            if (core_index == _hx5_pm4x25_core_num[i]) {
                type = portmodDispatchTypePm4x25;
                SOC_IF_ERROR_RETURN
                    (_soc_hx5_portctrl_device_addr_port_get(unit, type,
                                                 &pAddr, &pPort, &pCoreNum));
                pmid = i;
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

    fw_load_method = phymodFirmwareLoadMethodExternal;

    soc_physim_enable_get(unit, &is_sim);

    if (is_sim) {
        /* Firmward loader : Don't allow FW load on sim enviroment */
        fw_load_method = phymodFirmwareLoadMethodNone;
    } else {
        fw_load_method = soc_property_suffix_num_get(unit, pmid,
                                        spn_LOAD_FIRMWARE, "quad",
                                        phymodFirmwareLoadMethodInternal);
        fw_load_method &= 0xff;
    }

    PORTMOD_IF_ERROR_RETURN(phymod_polarity_t_init(&polarity));

    max_index = IS_QSGMII_PORT(unit, port) ? SOC_PM_QTC_NUM_LANES : SOC_PM4X10_NUM_LANES;

    /* Polarity */
    for (lane = 0; lane < max_index; lane++) {
        phy_port = first_port + lane;

        helix5_pm_port_polarity_get(unit, phy_port, lane, &tx_polarity, &rx_polarity);

        polarity.rx_polarity |= rx_polarity;
        polarity.tx_polarity |= tx_polarity;
    }

    /* Lane map */
    PORTMOD_IF_ERROR_RETURN(phymod_lane_map_t_init(&lane_map));

    phy_port = first_port;

    helix5_pm_port_lanemap_get(unit, phy_port, &txlane_map, &rxlane_map);

    polarity.rx_polarity |= rx_polarity;
    polarity.tx_polarity |= tx_polarity;

    lane_map.num_of_lanes = SOC_PM4X10_NUM_LANES;

    for(lane = 0 ; lane < SOC_PM4X10_NUM_LANES; lane++) {
        lane_map.lane_map_tx[lane] = (txlane_map >> (lane * SOC_PM4X10_NUM_LANES)) &
                                     SOC_PM4X10_LANE_MASK;
        lane_map.lane_map_rx[lane] = (rxlane_map >> (lane * SOC_PM4X10_NUM_LANES)) &
                                     SOC_PM4X10_LANE_MASK;
    }
    sal_memcpy(&port_config_info->fw_load_method[0], &fw_load_method,
                sizeof(phymod_firmware_load_method_t));
    port_config_info->fw_load_method_overwrite = 1;
    sal_memcpy(&port_config_info->polarity[0], &polarity,
                sizeof(phymod_polarity_t));
    port_config_info->polarity_overwrite = 1;
    sal_memcpy(&port_config_info->lane_map[0], &lane_map,
                sizeof(phymod_lane_map_t));
    port_config_info->lane_map_overwrite = 1;

    return SOC_E_NONE;
#else /* PORTMOD_PM4X10_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X10_SUPPORT */
}

int
soc_hx5_portctrl_pm_port_phyaddr_get(int unit, soc_port_t port)
{
    int core_index;
    uint32 i, *pAddr = NULL, *pPort = NULL, *pCoreNum = NULL;

    /* Find the port belong to which PM. The port has its own core id, using it to compare PM. */
    core_index = SOC_INFO(unit).port_serdes[port];

#ifdef PORTMOD_PM4X10_SUPPORT
    for (i=0; i<SOC_HX5_PM4X10_COUNT; i++) {
        if (core_index == _hx5_pm4x10_core_num[i]) {
            SOC_IF_ERROR_RETURN
                (_soc_hx5_portctrl_device_addr_port_get(unit,
                                             portmodDispatchTypePm4x10,
                                             &pAddr, &pPort, &pCoreNum));
            return pAddr[i];
        }
    }
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM4X10_QTC_SUPPORT
    for (i=0; i<SOC_HX5_PM4X10_QTC_COUNT;  i++) {
        if (core_index == _hx5_pm4x10q_core_num[i]) {
            SOC_IF_ERROR_RETURN
                (_soc_hx5_portctrl_device_addr_port_get(unit,
                                             portmodDispatchTypePm4x10_qtc,
                                             &pAddr, &pPort, &pCoreNum));
            return pAddr[i];
        }
    }
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM4X25_SUPPORT
    /* When port dones't find PM4x10, find PM4x25 again. */
    for (i=0; i<SOC_HX5_PM4X25_COUNT; i++) {
        if (core_index == _hx5_pm4x25_core_num[i]) {
            SOC_IF_ERROR_RETURN
                (_soc_hx5_portctrl_device_addr_port_get(unit,
                                             portmodDispatchTypePm4x25,
                                             &pAddr, &pPort, &pCoreNum));
            return pAddr[i];
        }
    }
#endif /* PORTMOD_PM4X25_SUPPORT */

    return -1;
}

#define PORT_IS_FALCON(unit, phy_port) \
        ((phy_port  >= 65) && (phy_port <= 76)) ? 1 : 0

#define PORT_IS_PM4X10(unit, phy_port) \
        ((phy_port  >= 49) && (phy_port <= 64)) ? 1 : 0

#define PORT_IS_PM4X10_QTC(unit, phy_port) \
        ((phy_port  >= 1) && (phy_port <= 48)) ? 1 : 0

int
soc_hx5_portctrl_pm_type_get(int unit, soc_port_t phy_port,
                            int* pm_type)
{
    if ((phy_port < 0) || (phy_port > 76)) {
        return SOC_E_PARAM;
    }

    if (PORT_IS_FALCON(unit, phy_port)) {
        *pm_type = portmodDispatchTypePm4x25;
    } else if (PORT_IS_PM4X10(unit, phy_port)) {
        *pm_type = portmodDispatchTypePm4x10;
#ifdef PORTMOD_PM4X10_QTC_SUPPORT
    } else if (PORT_IS_PM4X10_QTC(unit, phy_port)) {
        *pm_type = portmodDispatchTypePm4x10_qtc;
#endif
    } else {
        *pm_type = -1;
    }

    return ((*pm_type == -1) ? SOC_E_UNAVAIL : SOC_E_NONE);
}

#endif /* BCM_HELIX5_SUPPORT */
