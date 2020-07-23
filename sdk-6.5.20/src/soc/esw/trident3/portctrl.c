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
#include <shared/bsl.h>
#include <soc/phy/phymod_sim.h>
#include <soc/portmod/portmod.h>
#include <soc/trident3.h>

#ifdef BCM_TRIDENT3_SUPPORT

#define SOC_TD3_PM4X10_COUNT         1
static uint32_t _td3_pm4x10_addr[SOC_TD3_PM4X10_COUNT] = { 0x1c1 };

#define SOC_TD3_PM4X25_COUNT  32
#define SOC_TD3_MAX_PHYS      137  /* 32*4 CLMAC's + 4 XLMAC */

static uint32_t _td3_pm4x25_addr[SOC_TD3_PM4X25_COUNT] = {
    0x81,  0x85,  0x89,  0x8D,
    0x91,  0x95,  0xA1,  0xA5,
    0xA9,  0xAD,  0xc1,  0xC5,
    0xC9,  0xCD,  0xD1,  0xD5,
    0xE1,  0xE5,  0xE9,  0xED,
    0xF1,  0xF5,  0x181, 0x185,
    0x189, 0x18D, 0x1A1, 0x1A5,
    0x1A9, 0x1AD, 0x1B1, 0x1B5
};


static portmod_pm_instances_t td3_pm_types[] = {
#ifdef PORTMOD_PM4X25_SUPPORT
        {portmodDispatchTypePm4x25,  SOC_TD3_PM4X25_COUNT},
#endif
#ifdef PORTMOD_PM4X10_SUPPORT
        {portmodDispatchTypePm4x10,  SOC_TD3_PM4X10_COUNT},
#endif
};

#ifdef PORTMOD_PM4X25_SUPPORT
static portmod_default_user_access_t *pm4x25_td3_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM4X25_SUPPORT */

#ifdef PORTMOD_PM4X10_SUPPORT
static portmod_default_user_access_t *pm4x10_td3_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM4X25_SUPPORT */



/*
 * Polarity get looks for physical port based config first. This will enable
 * the user to define the polarity for all ports weather mapped to logical 
 * port or not.
 *
 * For legacy compatibility - we still look for the logical port
 * config setup.
 */
STATIC void
td3_pm_port_polarity_get (int unit, int logical_port, int physical_port, 
                          int lane, uint32 *tx_pol, uint32* rx_pol)
{
    uint32 rx_polarity, tx_polarity;

    rx_polarity = soc_property_phy_get(unit, physical_port, 0, 0, lane,
                         spn_PHY_CHAIN_RX_POLARITY_FLIP_PHYSICAL, 0x0);
    *rx_pol = (rx_polarity & 0x1) << lane;

    tx_polarity = soc_property_phy_get(unit, physical_port, 0, 0, lane,
                         spn_PHY_CHAIN_TX_POLARITY_FLIP_PHYSICAL, 0x0);
    *tx_pol = (tx_polarity & 0x1) << lane;
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
void td3_pm_port_lanemap_get (int unit, int logical_port, int physical_port, 
                           uint32 *txlane_map, uint32* rxlane_map)       
{
    *rxlane_map = soc_property_phy_get(unit, physical_port, 0, 0, 0,
                        spn_PHY_CHAIN_RX_LANE_MAP_PHYSICAL, 0x3210);

    *txlane_map = soc_property_phy_get(unit, physical_port, 0, 0, 0,
                        spn_PHY_CHAIN_TX_LANE_MAP_PHYSICAL, 0x3210);
}


STATIC int
_soc_td3_portctrl_pm4x25_portmod_init(int unit, int second_mgmt_port)
{
#ifdef PORTMOD_PM4X25_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm_create_info;
    int pmid = 0, blk, is_sim, found, logical_port;
    int first_port, idx, mode;
    uint32 pm_mask = 0xFFFFFFFF;

    /* Allocate user access for all PMs */
    SOC_IF_ERROR_RETURN (soc_esw_portctrl_pm_user_access_alloc(unit, 
                  SOC_TD3_PM4X25_COUNT, &pm4x25_td3_user_acc[unit]));

    /*
     * in 2T SKU, the following PM's are removed between PM0-PM31
     * PM2, PM4, PM6, ---- PM9, PM11, PM13, 
     * PM18, PM20, PM22, ---  PM25, PM27, PM29,
     * These PM's correspond to the bit mask (cleared bits) below.
     */
    if (soc_feature(unit, soc_feature_td3_reduced_pm)) {
        pm_mask = 0xD5AFD5AF;
    }

    for (pmid = 0; PORTMOD_SUCCESS(rv) && (pmid < SOC_TD3_PM4X25_COUNT); pmid++) {

        if (!((0x01 << pmid) & pm_mask)) {
            continue;
        }

        rv = portmod_pm_create_info_t_init(unit, &pm_create_info);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_create_info.type         = portmodDispatchTypePm4x25;

        /* 1 based port number */
        first_port = 1 + (pmid << 2);

        SOC_PBMP_PORT_ADD(pm_create_info.phys, first_port);
        SOC_PBMP_PORT_ADD(pm_create_info.phys, first_port+1);
        SOC_PBMP_PORT_ADD(pm_create_info.phys, first_port+2);
        SOC_PBMP_PORT_ADD(pm_create_info.phys, first_port+3);

        /* Identify the block for this particular port */
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


        /* Init user_acc for phymod access struct */
        rv = portmod_default_user_access_t_init(unit,
                                                &(pm4x25_td3_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(pm4x25_td3_user_acc[unit][pmid])));
        PORTMOD_USER_ACCESS_REG_VAL_OFFSET_ZERO_CLR((&(pm4x25_td3_user_acc[unit][pmid])));
        pm4x25_td3_user_acc[unit][pmid].unit = unit;
        pm4x25_td3_user_acc[unit][pmid].blk_id = blk;
        pm4x25_td3_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");

        if (pm4x25_td3_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        /* Specific info for PM4x25 */
        rv = phymod_access_t_init(&pm_create_info.pm_specific_info.pm4x25.access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_create_info.pm_specific_info.pm4x25.access.user_acc = &(pm4x25_td3_user_acc[unit][pmid]);
        pm_create_info.pm_specific_info.pm4x25.access.addr     = _td3_pm4x25_addr[pmid];
        pm_create_info.pm_specific_info.pm4x25.access.bus      = NULL; /* Use default bus */
        pm_create_info.pm_specific_info.pm4x25.rescal          = -1;
#if !defined (EXCLUDE_PHYMOD_TSCF_SIM) && defined (PHYMOD_TSCF_SUPPORT)
        rv = soc_physim_check_sim(unit, phymodDispatchTypeTscf,
                                  &(pm_create_info.pm_specific_info.pm4x25.access),
                                  0, &is_sim);
#else
        is_sim = 0;
#endif

        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        if (is_sim) {
            /* Firmward loader : Don't allow FW load on sim enviroment */
            pm_create_info.pm_specific_info.pm4x25.fw_load_method =
                phymodFirmwareLoadMethodNone;

            PHYMOD_ACC_F_CLAUSE45_SET(&pm_create_info.pm_specific_info.pm4x25.access);
        }

        /* Use default external loader if NULL */
        pm_create_info.pm_specific_info.pm4x25.external_fw_loader = NULL;

        logical_port = SOC_INFO(unit).port_p2l_mapping[first_port];

        pm_create_info.pm_specific_info.pm4x25.core_num = pmid; 
        pm_create_info.pm_specific_info.pm4x25.core_num_int = 0;

        /* Figure out the port mode */
        SOC_IF_ERROR_RETURN(soc_td3_port_mode_get(unit, logical_port, &mode));
        if (mode == portmodPortModeTri012 || mode == portmodPortModeTri023) {
            pm_create_info.pm_specific_info.pm4x25.port_mode_aux_info = portmodModeInfoThreePorts;
        }

        /* Add PM to PortMod */
        rv = portmod_port_macro_add(unit, &pm_create_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (pmid=0; pmid < SOC_TD3_PM4X25_COUNT; pmid++) {
            if (pm4x25_td3_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm4x25_td3_user_acc[unit][pmid].mutex);
                pm4x25_td3_user_acc[unit][pmid].mutex = NULL;
            }
        }
    }

    return rv;
#else /* PORTMOD_PM4X25_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X25_SUPPORT */
}

#define TRIDENT3_FIRST_MGMT_PHYSICAL_PORT_NUM   129
#define TRIDENT3_SECOND_MGMT_PHYSICAL_PORT_NUM  131
STATIC int
_soc_td3_portctrl_pm4x10_portmod_init(int unit, int second_mgmt_port)
{
#if defined PORTMOD_PM4X10_SUPPORT && defined BCM_56870_A0
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm_create_info;
    int pmid = 0, blk, is_sim, found;
    int idx, first_port;
    int port_num_lanes = 0;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    /* Allocate user access for all PMs */
    SOC_IF_ERROR_RETURN (soc_esw_portctrl_pm_user_access_alloc(unit, 
                  SOC_TD3_PM4X10_COUNT, &pm4x10_td3_user_acc[unit]));

    for (pmid = 0; PORTMOD_SUCCESS(rv) && (pmid < SOC_TD3_PM4X10_COUNT); pmid++) {

        rv = portmod_pm_create_info_t_init(unit, &pm_create_info);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_create_info.type         = portmodDispatchTypePm4x10;
        first_port = TRIDENT3_FIRST_MGMT_PHYSICAL_PORT_NUM;
        SOC_PBMP_PORT_ADD(pm_create_info.phys, first_port); /* first mgmt port */
        SOC_PBMP_PORT_ADD(pm_create_info.phys, first_port + 1); /* SW Workaround: Need to add.
                                                                   the dummy port so that lane_index
                                                                   is incremented to 2 for the second
                                                                   management port to detect ext PHY */

        if (second_mgmt_port) {
             SOC_PBMP_PORT_ADD(pm_create_info.phys, first_port + 2);
            /* Second mgmt reuse physical port number 128 */
            port_num_lanes = si->port_num_lanes[si->port_p2l_mapping[128]];
            if (port_num_lanes == 2) {
                 SOC_PBMP_PORT_ADD(pm_create_info.phys, first_port + 3);
            }
        }
        /* Check if mgmt ports is configured as 4 lanes */
        port_num_lanes = si->port_num_lanes[si->port_p2l_mapping[TRIDENT3_FIRST_MGMT_PHYSICAL_PORT_NUM]];
        if (port_num_lanes == 4) {
            SOC_PBMP_PORT_ADD(pm_create_info.phys, first_port + 2);
            SOC_PBMP_PORT_ADD(pm_create_info.phys, first_port + 3);
        }

        /* Identify the block for this particular port */
        found = 0;

        for(idx = 0; idx < SOC_DRIVER(unit)->port_num_blktype; idx++){
            blk = SOC_PORT_IDX_INFO(unit, TRIDENT3_FIRST_MGMT_PHYSICAL_PORT_NUM, idx).blk;
            if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_XLPORT){
                found = 1;
                break;
            }
        }

        if (!found) {
            rv = SOC_E_INTERNAL;
            break;
        }


        /* Init user_acc for phymod access struct */
        rv = portmod_default_user_access_t_init(unit,
                                                &(pm4x10_td3_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(pm4x10_td3_user_acc[unit][pmid])));
        PORTMOD_USER_ACCESS_REG_VAL_OFFSET_ZERO_CLR((&(pm4x10_td3_user_acc[unit][pmid])));

        pm4x10_td3_user_acc[unit][pmid].unit = unit;
        pm4x10_td3_user_acc[unit][pmid].blk_id = blk;
        pm4x10_td3_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");

        if (pm4x10_td3_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        /* Specific info for PM4x10 */
        rv = phymod_access_t_init(&pm_create_info.pm_specific_info.pm4x10.access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_create_info.pm_specific_info.pm4x10.access.user_acc = &(pm4x10_td3_user_acc[unit][pmid]);
        pm_create_info.pm_specific_info.pm4x10.access.addr     = _td3_pm4x10_addr[pmid];
        pm_create_info.pm_specific_info.pm4x10.access.bus      = NULL; /* Use default bus */
        pm_create_info.pm_specific_info.pm4x10.rescal          = -1;
#if !defined (EXCLUDE_PHYMOD_TSCE16_SIM) && defined (PHYMOD_TSCE16_SUPPORT)
        rv = soc_physim_check_sim(unit, phymodDispatchTypeTsce16,
                                  &(pm_create_info.pm_specific_info.pm4x10.access),
                                  0, &is_sim);
#else
        is_sim = 0;
#endif

        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        if (is_sim) {
            /* Firmward loader : Don't allow FW load on sim enviroment */
            pm_create_info.pm_specific_info.pm4x10.fw_load_method =
                phymodFirmwareLoadMethodNone;

            /* TSCE sim supports CL45 mode to read/write registers. */
            PHYMOD_ACC_F_CLAUSE45_SET(&pm_create_info.pm_specific_info.pm4x10.access);
        }

        /* Use default external loader if NULL */
        pm_create_info.pm_specific_info.pm4x10.external_fw_loader = NULL;

        pm_create_info.pm_specific_info.pm4x10.core_num = pmid; 
        pm_create_info.pm_specific_info.pm4x10.core_num_int = 0;

        /*
         * Set port mode as dual-port mode for all speed modes(XFI/RXAUI/XAUI),
         * it is WAR for shim issue on Merlin16 MGMT port
         */
        pm_create_info.pm_specific_info.pm4x10.port_mode_aux_info
                = portmodModeInfoTwoDualModePorts;
        pm_create_info.pm_specific_info.pm4x10.portmod_phy_external_reset
             = soc_esw_portctrl_reset_tsc3_cb;

        /* Add PM to PortMod */
        rv = portmod_port_macro_add(unit, &pm_create_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (pmid=0; pmid < SOC_TD3_PM4X10_COUNT; pmid++) {
            if (pm4x10_td3_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm4x10_td3_user_acc[unit][pmid].mutex);
                pm4x10_td3_user_acc[unit][pmid].mutex = NULL;
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
 *      soc_td3_portctrl_pm_portmod_init
 * Purpose:
 *      Initialize PortMod and PortMacro for Trident2Plus
 *      Add port macros (PM) to the unit's PortMod Manager (PMM).
 * Parameters:
 *      unit             - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_td3_portctrl_pm_portmod_init(int unit)
{
    int    rv = SOC_E_NONE;
    uint32 flags = 0, mgmt_port_count = 0;

    /*
     * If portmod was create - destroy and create again
     * Better way would be to create once and leave it.
     */
    if (SOC_E_NONE == soc_esw_portctrl_init_check(unit)) {
        SOC_IF_ERROR_RETURN(portmod_destroy(unit));
    }

    PORTMOD_CREATE_F_PM_NULL_SET(flags);

    SOC_IF_ERROR_RETURN
        (portmod_create(unit, flags, SOC_MAX_NUM_PORTS, SOC_TD3_MAX_PHYS, 
                         sizeof(td3_pm_types)/sizeof(portmod_pm_instances_t), td3_pm_types));

    SOC_PBMP_COUNT(SOC_INFO(unit).management_pbm, mgmt_port_count);

    rv = _soc_td3_portctrl_pm4x25_portmod_init (unit, (mgmt_port_count == 2) ? 1 : 0);
    if (rv) return (rv);

    return(_soc_td3_portctrl_pm4x10_portmod_init (unit, (mgmt_port_count == 2) ? 1 : 0));
}

/*
 * Function:
 *      soc_td3_portctrl_pm_portmod_deinit
 * Purpose:
 *      Deinitialize PortMod and PortMacro for Trident2Plus.
 *      Free pm user access data.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_NONE
 */
int
soc_td3_portctrl_pm_portmod_deinit(int unit)
{
    if (SOC_E_INIT == soc_esw_portctrl_init_check(unit)) {
        return SOC_E_NONE;
    }

    /* Free PMs structures */
#ifdef PORTMOD_PM4X25_SUPPORT
    if (pm4x25_td3_user_acc[unit] != NULL) {
        sal_free(pm4x25_td3_user_acc[unit]);
        pm4x25_td3_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM4X25_SUPPORT */

#ifdef PORTMOD_PM4X10_SUPPORT
    if (pm4x10_td3_user_acc[unit] != NULL) {
        sal_free(pm4x10_td3_user_acc[unit]);
        pm4x10_td3_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM4X10_SUPPORT */

    SOC_IF_ERROR_RETURN(portmod_destroy(unit));

    return SOC_E_NONE;
}

int
soc_td3_portctrl_pm_port_config_get(int unit, soc_port_t port, void *port_config)
{
#ifdef PORTMOD_PM4X25_SUPPORT
    int rv = SOC_E_NONE;
    int pmid = 0, lane, phy, phy_port, logical_port, port_mode;
    int first_port = 0, core_num, core_count = 0, core_index, is_sim, type;
    uint32 txlane_map, rxlane_map, i;
    uint32 tx_polarity, rx_polarity;
    uint32 mgmt_port_count;
    portmod_port_init_config_t *port_config_info;
    phymod_firmware_load_method_t fw_load_method;
    phymod_polarity_t polarity; /**< Lanes Polarity */
    phymod_lane_map_t lane_map;

    port_config_info = (portmod_port_init_config_t *)port_config;
    SOC_IF_ERROR_RETURN(soc_td3_port_mode_get(unit, port, &port_mode));
    if (port_mode == portmodPortModeTri012 || port_mode == portmodPortModeTri023) {
        port_config_info->port_mode_aux_info = portmodModeInfoThreePorts;
    }

    phy = SOC_INFO(unit).port_l2p_mapping[port];

    /* Find the port belong to which PM. The port has its own core id, using it to compare PM. */
    core_num = (phy - 1) / SOC_PM4X25_NUM_LANES;

    for (i=0; i<SOC_TD3_PM4X25_COUNT; i++) {
        if (core_num == i) {
            type   = portmodDispatchTypePm4x25;
            pmid = i;
            first_port = 1 + (pmid << 2);
            core_count = 1;
            break;
        }
    }

    /* When port doesn't find PM4x25, find PM4x10 again. */
    SOC_PBMP_COUNT(SOC_INFO(unit).management_pbm, mgmt_port_count);

    if ((i == SOC_TD3_PM4X25_COUNT) ||
        ((mgmt_port_count == 2) && (phy == 128))) {
        type = portmodDispatchTypePm4x10;
        pmid = 0;
        core_count = 1;
        first_port = 129;
    }

    for (core_index = 0; core_index < core_count; core_index++) {
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

        /* Polarity */
        for (lane = 0; lane < SOC_PM4X25_NUM_LANES; lane++) {
            phy_port = first_port + lane + (core_index*4);
            logical_port = SOC_INFO(unit).port_p2l_mapping[phy_port];
        
            td3_pm_port_polarity_get (unit, logical_port, phy_port, lane,
                                      &tx_polarity, &rx_polarity);

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

        td3_pm_port_lanemap_get (unit, logical_port, phy_port,
                                 &txlane_map, &rxlane_map);

        lane_map.num_of_lanes = (type == portmodDispatchTypePm4x25) ?
                                 SOC_PM4X25_NUM_LANES : SOC_PM4X10_NUM_LANES;

        for(lane = 0 ; lane < lane_map.num_of_lanes; lane++) {
            lane_map.lane_map_tx[lane] =
                (txlane_map >> (lane * lane_map.num_of_lanes)) &
                SOC_PM4X25_LANE_MASK;
            lane_map.lane_map_rx[lane] =
                (rxlane_map >> (lane * lane_map.num_of_lanes)) &
                SOC_PM4X25_LANE_MASK;
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
#else /* PORTMOD_PM4X25_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X25_SUPPORT */
}

int
soc_td3_portctrl_pm_port_phyaddr_get(int unit, soc_port_t port)
{
    int phy, core_index;
    uint32 pmid;

    phy = SOC_INFO(unit).port_l2p_mapping[port];

    /* Find the port belong to which PM. The port has its own core id, using it to compare PM. */
    core_index = (phy - 1) / SOC_PM4X25_NUM_LANES;

#ifdef PORTMOD_PM4X25_SUPPORT
    /* When port dones't find PM4x10, find PM4x25 again. */
    for (pmid=0; pmid<SOC_TD3_PM4X25_COUNT; pmid++) {
        if (core_index == pmid) {
            return _td3_pm4x25_addr[pmid];
        }
    }
#endif /* PORTMOD_PM4X25_SUPPORT */

#ifdef PORTMOD_PM4X10_SUPPORT
    for (pmid=0; pmid<SOC_TD3_PM4X10_COUNT; pmid++) {
        if (core_index == (31+pmid)) {
            return _td3_pm4x10_addr[pmid];
        }
    }
#endif /* PORTMOD_PM4X10_SUPPORT */
    return -1;
}

#define _TD3_PHY_PORT_MNG0              129
int
soc_td3_portctrl_pm_type_get(int unit, soc_port_t phy_port, int* pm_type)
{
    *pm_type = -1;

    if (phy_port >= 1 && phy_port <= 128) {
#ifdef PORTMOD_PM4X25_SUPPORT
        *pm_type = portmodDispatchTypePm4x25;
        return (SOC_E_NONE);
#endif
    } else if (phy_port == _TD3_PHY_PORT_MNG0) {
#ifdef PORTMOD_PM4X10_SUPPORT
        *pm_type = portmodDispatchTypePm4x10;
        return (SOC_E_NONE);
#endif
    } else {
        return(SOC_E_PARAM);
    }

    return SOC_E_UNAVAIL;
}

soc_portctrl_functions_t soc_td3_portctrl_func = {
    soc_td3_portctrl_pm_portmod_init,
    soc_td3_portctrl_pm_portmod_deinit,
    soc_td3_portctrl_pm_port_config_get,
    soc_td3_portctrl_pm_port_phyaddr_get,
    NULL,
    soc_td3_portctrl_pm_type_get,
    NULL,
    NULL
};

#endif /* BCM_TRIDENT3_SUPPORT */
