/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        portctrl.c
 * Purpose:     SDK Port Control Layer
 *
 *              The purpose is to encapsulate port functionality
 *              related to the xxPORT block (i.e. XLPORT, CPORT)
 *              MAC and PHY.
 *
 *              Currently, only the PortMod library is being supported.
 *              The PortMod library provides support for the MAC, PHY,
 *              and xxPORT registers.
 */

#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/esw/portctrl.h>
#include <soc/esw/portctrl_internal.h>
#include <soc/esw/port.h>
#include <bcm_int/esw/port.h>
#ifdef BCM_HURRICANE4_SUPPORT
#include <soc/hurricane4.h>
#endif

#ifdef PORTMOD_SUPPORT
#include <soc/phy/phymod_sim.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/phy/phymod_port_control.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_legacy_phy.h>
#include <soc/phy/phyctrl.h>

/* Indicates if the Port Control module has been initalized */
static int portctrl_init[SOC_MAX_NUM_DEVICES];

/*
 * Define:
 *      PORTCTRL_PORT_TO_PPORT
 * Purpose:
 *      Converts a SOC port into port type used in PortMod functions '_pport'
 */
#define PORTCTRL_PORT_TO_PPORT(_port, _pport) do {  \
        _pport = _port;                             \
    } while (0)

static phymod_bus_t portmod_ext_default_bus = {
    "MDIO Bus",
    portmod_ext_phy_mdio_c45_reg_read,
    portmod_ext_phy_mdio_c45_reg_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    NULL,
    NULL,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

extern uint32_t phymod_dbg_mask;
extern uint32_t phymod_dbg_addr;
extern uint32_t phymod_dbg_lane;

/* Used by timestamp adjust */
typedef struct {
    int speed;
    char *props[2];
} portctrl_ts_adjust_cfg_t;

static portctrl_ts_adjust_cfg_t ts_adjust_props[] = {
{10, {spn_TIMESTAMP_ADJUST_10MBE_NS, spn_TIMESTAMP_TSTS_ADJUST_10MBE_NS}},
{100, {spn_TIMESTAMP_ADJUST_100MBE_NS, spn_TIMESTAMP_TSTS_ADJUST_100MBE_NS}},
{1000, {spn_TIMESTAMP_ADJUST_1GBE_NS, spn_TIMESTAMP_TSTS_ADJUST_1GBE_NS}},
{2500, {spn_TIMESTAMP_ADJUST_2_5GBE_NS, spn_TIMESTAMP_TSTS_ADJUST_2_5GBE_NS}},
{10000, {spn_TIMESTAMP_ADJUST_10GBE_NS, spn_TIMESTAMP_TSTS_ADJUST_10GBE_NS}},
{25000, {spn_TIMESTAMP_ADJUST_25GBE_NS, spn_TIMESTAMP_TSTS_ADJUST_25GBE_NS}},
{40000, {spn_TIMESTAMP_ADJUST_40GBE_NS, spn_TIMESTAMP_TSTS_ADJUST_40GBE_NS}},
{50000, {spn_TIMESTAMP_ADJUST_50GBE_NS, spn_TIMESTAMP_TSTS_ADJUST_50GBE_NS}},
{100000, {spn_TIMESTAMP_ADJUST_100GBE_NS, spn_TIMESTAMP_TSTS_ADJUST_100GBE_NS}},
{-1, {spn_TIMESTAMP_ADJUST_NS, spn_TIMESTAMP_TSTS_ADJUST_NS}}
};

/*
 * Function:
 *      soc_esw_portctrl_init_check
 * Purpose:
 *      check if portctrl is already initialized.
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_NONE  : Initialized
 *      SOC_E_INIT  : Not Initialized.
 */
int
soc_esw_portctrl_init_check(int unit)
{
    if (SOC_PORTCTRL_NOT_INITIALIZED == portctrl_init[unit]) {
        return SOC_E_INIT;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_esw_portctrl_pm_user_access_alloc
 * Purpose:
 *      Allocate SW data structure for the port macros (PM).
 * Parameters:
 *      unit        - (IN) Unit number.
 *      num         - (IN) Number of PM port macros in device.
 *      user_acc    - (OUT) Returns allocated PMs structures.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_portctrl_pm_user_access_alloc(int unit, int num,
                                      portmod_default_user_access_t **user_acc)
{
    if (*user_acc == NULL) {
        *user_acc = sal_alloc(sizeof(portmod_default_user_access_t) * num,
                             "PortMod PM");
        if (*user_acc == NULL) {
            return SOC_E_MEMORY;
        }
    }
    sal_memset(*user_acc, 0, sizeof(portmod_default_user_access_t) * num);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_esw_portctrl_dump_txrx_lane_map
 * Purpose:
 *      Utility function to dump tx, rx lane map
 * Parameters:
 *      unit        - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
void
soc_esw_portctrl_dump_txrx_lane_map(int unit, int first_port, int logical_port,
              int core_num, uint32 txlane_map_b, uint32 rxlane_map_b,
              uint32 txlane_map, uint32 rxlane_map)
{
    static int hdr_flag =0;
    if (!hdr_flag) {
        LOG_ERROR(BSL_LS_SOC_PORT,
            (BSL_META_UP(unit, logical_port,
            "+-------+-------+-------+-------+------+--------+--------+\n")));
        LOG_ERROR(BSL_LS_SOC_PORT,
            (BSL_META_UP(unit, logical_port,
            "| PPORT | LPORT | TXMAP | RXMAP | CORE | TXSWAP | RXSWAP |\n")));
        LOG_ERROR(BSL_LS_SOC_PORT,
            (BSL_META_UP(unit, logical_port,
            "+-------+-------+-------+-------+------+--------+--------+\n")));
        hdr_flag=1;
    }
    LOG_ERROR(BSL_LS_SOC_PORT,
        (BSL_META_UP(unit, logical_port,
         "| %04d  | %04d  | %04X  | %04X  | %02d   | %04X   | %04X   |\n"),
         first_port, logical_port, txlane_map_b, rxlane_map_b, core_num,
         txlane_map, rxlane_map));

    LOG_ERROR(BSL_LS_SOC_PORT,
        (BSL_META_UP(unit, logical_port,
        "+-------+-------+-------+-------+------+--------+--------+\n")));
    return;
}

/*
 * Function:
 *      soc_esw_portctrl_ext_phy_config_parameter_get
 * Purpose:
 *      To extract external phy parameter from config.bcm.
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      port             - (IN)  Logical Port Number
 *      phy_addr         - (OUT) Ext Phy MDIO address. 0xFF mean no ext phy
 *      shift            - (IN)  Phy Port Map Shift from internal map. ( default 0)
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
soc_esw_portctrl_ext_phy_config_parameter_get (int unit, int port,
                   uint32 *phy_addr, uint32 *num_int_cores, uint32 shift[3])
{
    char *config_str, *sub_str, *sub_str_end;
    int idx;

    /* set default return values. */
    *num_int_cores  = 1;
    shift[0]        = 0;
    shift[1]        = 0;
    shift[2]        = 0;

    config_str = soc_property_port_get_str(unit, port, spn_PORT_PHY_ADDR);
    if (config_str == NULL) {
        *phy_addr = 0xFF; /* NO PHY */
        return SOC_E_NONE;
    }

    /*
    * port_phy_addr_<port>=<phy addr>:<num_int_cores>:<shift0>:<shift1>:<shift2>
    */
    sub_str = config_str;

    /* Parse phy address  number */
    *phy_addr  = sal_ctoi(sub_str, &sub_str_end);

    if (*sub_str_end == '\0')
    {
        return SOC_E_NONE ;
    }

    /* Skip ':' between physical port number and num_internal_cores */
    sub_str = sub_str_end;
    if (*sub_str != '\0') {
        if (*sub_str != ':') {
            LOG_CLI((BSL_META_U(unit, "Port %d: Bad config string \"%s\"\n"),
                         port, config_str));
                return SOC_E_FAIL;
        }
        sub_str++;
    } else {
        return SOC_E_NONE;
    }

    /* parse num of cores. */
    *num_int_cores = sal_ctoi(sub_str, &sub_str_end);
    if (*num_int_cores > SOC_PM12X10_PM4X10_COUNT) {
        LOG_CLI((BSL_META_U(unit,
                "Port %d: Bad config string bad num of cores \"%s\" %d \n"),
                port, config_str, *num_int_cores));
        return SOC_E_FAIL;

    }

    /* check for end of string. */
    if( *sub_str_end == '\0' )
    {
        return SOC_E_NONE ;
    }

    for (idx=0; idx < *num_int_cores; idx++) {
        /* Skip ':' between physical port number and num_internal_cores */
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit, "Port %d: Bad config string \"%s\"\n"),
                         port, config_str));
                return SOC_E_FAIL;
            }
            sub_str++;
        }
        shift[idx] = sal_ctoi(sub_str, &sub_str_end);
        /* check for end of string. */
        if( *sub_str_end == '\0' )
        {
            return SOC_E_NONE ;
        }
    }
    return SOC_E_NONE;
}

typedef struct soc_esw_portctrl_phy_toplogy_s {
    uint16_t pport;        /* Physical port number */
    uint16_t xphy_index;   /* position of the external phy in the phy chain */
    uint16_t xphy_id;      /* Mdio address of the external phy */
    int16_t sys_lane;     /* system side lane number connected to the internal pport */
    int16_t line_lane;    /* corresponding line side lane number */
    uint16_t is_default;
} soc_esw_portctrl_phy_toplogy_t;

/*
 * Function:
 *      soc_esw_portctrl_ext_phy_config_topology_get
 * Purpose:
 *      To extract external phy topology from config.bcm.
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      topology         - (OUT) topology information of the physical port
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
soc_esw_portctrl_ext_phy_config_topology_get (int unit, soc_esw_portctrl_phy_toplogy_t *topology)
{

    char *config_str, *sub_str, *sub_str_end;
    char pport[8];

    topology->is_default = 0;
    sal_itoa(pport, topology->pport, 10, 0, 0);
    
    config_str = soc_property_suffix_num_str_get(unit, topology->xphy_index, spn_PHY_TOPOLOGY,
                                                 sal_strcat(pport, "_"));

     if (config_str == NULL) {
        /* No config found. There is a logical to logical port
           mapping present in the system */
        topology->xphy_id = 0xFF;
        topology->sys_lane = -1;
        topology->line_lane = -1;
        topology->is_default = 1;
        return SOC_E_NONE;
    }

    /*
    * phy_topology_<pport_xphy_index>=<xphy_id>:<sys_lane>:<line_lane>
    */
    sub_str = config_str;

    /* Parse phy address  number */
    topology->xphy_id  = sal_ctoi(sub_str, &sub_str_end);

    /* Skip ':' between phy address and sys lane */
    sub_str = sub_str_end;
    if (*sub_str != '\0') {
        if (*sub_str != ':') {
            LOG_CLI((BSL_META_U(unit, "Physical Port %d: Bad config string \"%s\"\n"),
                         topology->pport, config_str));
            return SOC_E_FAIL ;
        }
        sub_str++;
    } else {
        LOG_CLI((BSL_META_U(unit, "Physical Port %d: Bad config string \"%s\"\n"),
                         topology->pport, config_str));
        return SOC_E_FAIL ;
    }

    /* parse sys lane */
    topology->sys_lane = sal_ctoi(sub_str, &sub_str_end);
    sub_str = sub_str_end;

    if (*sub_str != '\0') {
        if (*sub_str != ':') {
            LOG_CLI((BSL_META_U(unit, "Physical Port %d: Bad config string \"%s\"\n"),
                         topology->pport, config_str));
            return SOC_E_FAIL ;
        }
        sub_str++;
    } else {
        LOG_CLI((BSL_META_U(unit, "Physical Port %d: Bad config string \"%s\"\n"),
                         topology->pport, config_str));
        return SOC_E_FAIL;
    }

    /* parse line lane */
    topology->line_lane = sal_ctoi(sub_str, &sub_str_end);
    sub_str = sub_str_end;

    /* look for end of config string */
    if (*sub_str != '\0') {
        LOG_CLI((BSL_META_U(unit, "Physical Port %d: Bad config string \"%s\"\n"),
                         topology->pport, config_str));
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

int
soc_esw_portctrl_xphy_default_core_access_get(int unit, phymod_core_access_t *core_access)
{
    int rv = SOC_E_NONE;
    portmod_default_user_access_t*  local_user_access;

    PORTMOD_IF_ERROR_RETURN(phymod_core_access_t_init(core_access));
    core_access->access.bus = &portmod_ext_default_bus;
    core_access->type = phymodDispatchTypeInvalid; /* Make sure it is invalid. */
    local_user_access = sal_alloc(sizeof(portmod_default_user_access_t),
                              "xphy_user_acc");
    if(local_user_access == NULL){
        LOG_CLI((BSL_META_U(unit, "soc_esw_portctrl_xphy_default_core_access_get local_user_access is NULL.\n")));
        return SOC_E_FAIL;
    }
    sal_memset(local_user_access, 0, sizeof(portmod_default_user_access_t));
    local_user_access->unit = unit;
    core_access->access.user_acc = local_user_access;
    return rv;

}

static int
soc_esw_portctrl_core_config_update(int unit, int lport, int pport, int lane, uint32_t xphy_addr,  int phy_idx) {

    int rv = SOC_E_NONE;
    int fw_ld_method = 0x1, i;
    uint32_t rx_polarity, tx_polarity;
    uint32_t rx_lane_map, tx_lane_map;
    uint32_t psc_repeater_mode = 0;
    uint32_t port_phy_clause = 45;

    uint8_t gearbox_enable;
    uint8_t pin_compatibility_enable;
    uint8_t phy_mode_reverse;
    int     force_fw_load, is_legacy_phy;
    phymod_firmware_load_method_t fw_load_method;
    phymod_core_access_t core_access;
    phymod_polarity_t  polarity;
    phymod_lane_map_t lane_map;

    rx_polarity = 0;
    tx_polarity = 0;
    rx_lane_map = 0;
    tx_lane_map = 0;

    /* soc read for phy device operation mode */
    gearbox_enable = (soc_property_port_get(unit, lport, spn_PHY_GEARBOX_ENABLE, FALSE));
    pin_compatibility_enable = soc_property_port_get(unit, lport,
                                   spn_PHY_PIN_COMPATIBILITY_ENABLE, FALSE);
    psc_repeater_mode = soc_property_port_get(unit, lport, spn_PHY_PCS_REPEATER, 0);
    phy_mode_reverse = soc_property_port_get(unit, lport, spn_PORT_PHY_MODE_REVERSE, 0);
    port_phy_clause = soc_property_port_get(unit, lport, spn_PORT_PHY_CLAUSE, 45);

    PORTMOD_IF_ERROR_RETURN(portmod_xphy_core_access_get(unit, xphy_addr, &core_access, &is_legacy_phy));
    core_access.device_op_mode = 0;

    if(gearbox_enable) {
        PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_SET(&core_access);
    }

    if (pin_compatibility_enable) {
        PHYMOD_INTF_CONFIG_PHY_PIN_COMPATIBILITY_ENABLE_SET(&core_access);
    }

    if (phy_mode_reverse) {
        PHYMOD_INTF_CONFIG_PORT_PHY_MODE_REVERSE_SET(&core_access);
    }

    if (psc_repeater_mode) {
        PHYMOD_INTF_CONFIG_PHY_PSC_REPEATER_MODE_SET(&core_access);
    }

    if (45 == port_phy_clause) {
        PHYMOD_ACC_F_CLAUSE45_SET(&core_access.access);
    }

    /* since core_access got change, save it. device_op_mode is also save to
       wb buffer inside portmod_xphy_core_access_set. */

    PORTMOD_IF_ERROR_RETURN(portmod_xphy_core_access_set(unit, xphy_addr, &core_access, is_legacy_phy));

    /*lower nibble to represent Force FW load and upper nibble to represent
      load method */
    fw_ld_method = 0x11;
    fw_ld_method = soc_property_port_get(unit, lport,
                                         spn_PHY_FORCE_FIRMWARE_LOAD, fw_ld_method);
    switch ((fw_ld_method >> 4) & 0xf) {
        case 0:
            fw_load_method  = phymodFirmwareLoadMethodNone;
        break;
        case 1:
            fw_load_method  = phymodFirmwareLoadMethodInternal;
        break;
        case 2:
            fw_load_method  = phymodFirmwareLoadMethodProgEEPROM;
        break;
        default:
            fw_load_method  = phymodFirmwareLoadMethodInternal;
        break;
    }

    PORTMOD_IF_ERROR_RETURN(portmod_xphy_fw_load_method_set(unit, xphy_addr, fw_load_method));
    switch (fw_ld_method & 0xf) {
        case 0:
            /* skip download */
            force_fw_load = phymodFirmwareLoadSkip;
        break;
        case 1:
            /* force download */
            force_fw_load = phymodFirmwareLoadForce;
        break;
        case 2:
            /* auto download. download firware if two versions are diffirent */
            force_fw_load = phymodFirmwareLoadAuto;
        break;
        default:
            force_fw_load = phymodFirmwareLoadSkip;
        break;
    }
    PORTMOD_IF_ERROR_RETURN(portmod_xphy_force_fw_load_set(unit, xphy_addr, force_fw_load));

    PORTMOD_IF_ERROR_RETURN(portmod_xphy_polarity_get(unit, xphy_addr, &polarity));
    /* get the plarity settings for the core. look for the legacy config (logical port based) if
     * not available then look for a new config mode */
     rx_polarity = soc_property_phy_get (unit, pport, phy_idx, 0, 0,
                                        spn_PHY_CHAIN_RX_POLARITY_FLIP_PHYSICAL, 0xFFFFFFFF);
    if (rx_polarity == 0xFFFFFFFF) {
        rx_polarity = soc_property_port_get(unit, lport,
                            spn_PHY_RX_POLARITY_FLIP, 0);
    } else {
        rx_polarity = (rx_polarity & 0x1) << lane; ;
    }

    tx_polarity = soc_property_phy_get (unit, pport, phy_idx, 0, 0,
                                        spn_PHY_CHAIN_TX_POLARITY_FLIP_PHYSICAL, 0xFFFFFFFF);
    if (tx_polarity == 0xFFFFFFFF) {
        tx_polarity =  soc_property_port_get(unit, lport,
                          spn_PHY_TX_POLARITY_FLIP, 0);
    } else {
        tx_polarity = (tx_polarity & 0x1) << lane; ;
    }

    polarity.rx_polarity |= rx_polarity;
    polarity.tx_polarity |= tx_polarity;
    PORTMOD_IF_ERROR_RETURN(portmod_xphy_polarity_set(unit, xphy_addr, polarity));

    /* get the lane map information */
    rx_lane_map = soc_property_phy_get (unit, pport, phy_idx, 0, 0,
                                        spn_PHY_CHAIN_RX_LANE_MAP_PHYSICAL, 0xFFFFFFFF);
    if (rx_lane_map == 0xFFFFFFFF) {
        rx_lane_map = soc_property_phy_get(unit, pport, phy_idx, 0, 0,
                                            spn_PHY_RX_LANE_MAP, 0xFFFFFFFF);
    }
    if (rx_lane_map == 0xFFFFFFFF) {
        rx_lane_map = soc_property_port_get(unit, lport,
                                            spn_PHY_RX_LANE_MAP, 0x3210);
    }

    /* get the lane map information */
    tx_lane_map = soc_property_phy_get (unit, pport, phy_idx, 0, 0,
                                        spn_PHY_CHAIN_TX_LANE_MAP_PHYSICAL, 0xFFFFFFFF);
    if (tx_lane_map == 0xFFFFFFFF) {
        tx_lane_map = soc_property_phy_get(unit, pport, phy_idx, 0, 0,
                                            spn_PHY_TX_LANE_MAP, 0xFFFFFFFF);
    }
    if (tx_lane_map == 0xFFFFFFFF) {
        tx_lane_map = soc_property_port_get(unit, lport,
                                            spn_PHY_TX_LANE_MAP, 0x3210);
    }

    /*FIX harcoding the number of lanes to 4 for now
     * need to figure out the better way to fix this */
    lane_map.num_of_lanes = 4;
    for (i = 0; i < 4; i++) {
        lane_map.lane_map_rx[i] = ((rx_lane_map >> (i*4)) & 0xf);
        lane_map.lane_map_tx[i] = ((tx_lane_map >> (i*4)) & 0xf);
    }

    PORTMOD_IF_ERROR_RETURN(portmod_xphy_lane_map_set(unit, xphy_addr, lane_map));

    return rv;
}

/*
 * Function:
 *      soc_esw_portctrl_xphy_wb_db_restore(nt unit, soc_pbmp_t phy_ports)
 * Purpose:
 *      To restore all external phy data bases on warmboot.
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      phy_ports       - (IN)  lane bit map of the phy.
 * Returns:
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_xphy_wb_db_restore(int unit)
{

    xphy_pbmp_t valid_phys;
    phymod_core_access_t core_access;
    int xphy_idx;
    int xphy_addr;

    PORTMOD_IF_ERROR_RETURN(portmod_xphy_all_valid_phys_get(unit, &valid_phys));

    /* iterate all the external phy that need to be restored. */
    XPHY_PBMP_ITER(valid_phys, xphy_idx){

        PORTMOD_IF_ERROR_RETURN(portmod_xphy_db_addr_get(unit, xphy_idx, &xphy_addr));
        PORTMOD_IF_ERROR_RETURN(soc_esw_portctrl_xphy_default_core_access_get(unit, &core_access));
        PORTMOD_IF_ERROR_RETURN(portmod_xphy_wb_db_restore(unit,xphy_idx, xphy_addr,&core_access));
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_esw_portctrl_setup_ext_phy_add(int unit, int port, portmod_pbmp_t phy_ports)
 * Purpose:
 *      Add External PHY info.
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      phy_ports       - (IN)  lane bit map of the phy.
 * Returns:
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_setup_ext_phy_add(int unit, int port, portmod_pbmp_t *phy_ports)
{
    int     rv, chain_idx, first_port;
    int     pport;
    uint8_t is_100G_port;
    uint32  ext_phy_addr, xphy_gearbox_mode;
    uint32  num_int_cores;
    uint32  first_phy_lane[3];
    int     phy_chain_length;
    uint32 primary_core_num=0;
    portmod_dispatch_type_t         type;
    soc_esw_portctrl_phy_toplogy_t  topology;
    soc_100g_lane_config_t          lane_config;

    phymod_core_access_t            core_access;
    phymod_ref_clk_t                ref_clk;
    phymod_firmware_load_method_t   fw_load_method;
    portmod_xphy_lane_connection_t  xphy_lane_connection;
    int xphy_idx = PORTMOD_XPHY_EXISTING_IDX;

    /* add Ext Phy to portmod. */

    /* identify if the pport is part of 100G port and 12x10 PM
       if true then get the mapping inf0 3:4:3 or 4:4:2 */
    lane_config = SOC_LANE_CONFIG_100G_4_4_2;
    pport = 0;
    is_100G_port = 0;

    /* assumption is that first pport of the PM12x10 is
    specified as a 100G port in portmap_lport=pport:100:map_info */
    PORTMOD_PBMP_ITER(*phy_ports, pport) {

        rv = portmod_phy_pm_type_get(unit, pport, &type);
        if (rv) return (rv);

        /*
         * Check to see if the port we are using is 100G or greater
         * and is pm12x10 type
         */
#if defined(PORTMOD_PM12X10_XGS_SUPPORT) && defined(PORTMOD_PM12X10_SUPPORT)
        if ( (type != portmodDispatchTypePm12x10) && (type != portmodDispatchTypePm12x10_xgs) ) {
            continue;
        }
#elif defined(PORTMOD_PM12X10_SUPPORT)
        if ( (type != portmodDispatchTypePm12x10) ) {
            continue;
        }
#else
        if ( (type != portmodDispatchTypeCount) ) {
            continue;
        }
#endif


        if ((port != -1) && (SOC_INFO(unit).port_speed_max[port] >= 100000)) {
            /* read the portmap config to get the map info */
            lane_config = SOC_INFO(unit).port_100g_lane_config[port];
            is_100G_port = 1;
        }
    }


    pport = 0;

    /* for each port in the input bit map read get the physical port number and call
     the new config get function to identify the physical topology with the external phy */
    first_port = -1;

    PORTMOD_PBMP_ITER(*phy_ports, pport) {

        if (first_port == -1) first_port = pport;

        /* breadth of the phy chain  Assumption here is that all the physical ports in a
           logical port will have same phy chain length */
        ext_phy_addr = 0xFF;
        chain_idx = 0;

        rv = soc_esw_portctrl_ext_phy_config_parameter_get(unit, port,
                    &ext_phy_addr, &num_int_cores,  first_phy_lane);
        phy_chain_length = soc_property_port_get(unit, port, spn_PHY_CHAIN_LENGTH, 0);
        primary_core_num = soc_property_port_get(unit, ext_phy_addr, spn_XPHY_PRIMARY_CORE_NUM, ext_phy_addr);
        xphy_gearbox_mode = soc_property_port_get(unit, port, spn_PHY_GEARBOX_ENABLE, 0);

        while((ext_phy_addr != 0xFF) || (phy_chain_length != 0)) {
            phy_chain_length = 0;
            num_int_cores = 0;

            chain_idx++;

            portmod_xphy_lane_connection_t_init(unit, &xphy_lane_connection);

            
            phy_chain_length = soc_property_port_get(unit, port, spn_PHY_CHAIN_LENGTH, 0);

            /* phy chain length and ext_phy_addr configs for a port are mutually exclusive */
            if (phy_chain_length && (chain_idx > phy_chain_length)) break;

           /* rv = soc_esw_portctrl_ext_phy_config_parameter_get(unit, xphy_logical_port,
                    &ext_phy_addr, &num_int_cores,  first_phy_lane);
           */
            topology.pport = pport;
            topology.xphy_index = chain_idx;
            topology.sys_lane = -1;
            topology.line_lane = -1;
            soc_esw_portctrl_ext_phy_config_topology_get(unit, &topology);

            /* only for the iner most external phys we have an alternate config */
            xphy_lane_connection.ss_lane_mask = -1;
            xphy_lane_connection.ls_lane_mask = -1;
            if (topology.is_default && (chain_idx == 1)) {
                topology.xphy_id = ext_phy_addr;
                xphy_lane_connection.xphy_id = ext_phy_addr;
                /* below logic should be applied only to inner most external phy*/
                if (is_100G_port) {
                    xphy_lane_connection.ss_lane_mask = 0x1 << (pport - first_port);
                    xphy_lane_connection.ls_lane_mask = 0x1 << (pport - first_port);
                    switch (lane_config) {
                        case SOC_LANE_CONFIG_100G_4_4_2:
                            if ((pport - first_port) == 10 ||
                                (pport - first_port) == 11) {
                                xphy_lane_connection.ss_lane_mask = -1;
                                xphy_lane_connection.ls_lane_mask = -1;
                            }
                            /* Temp fix for sesto gearbox */
                            if ((((pport - first_port) > 3) &&
                               ((pport - first_port) < 8))) {
                                xphy_lane_connection.ls_lane_mask = 0x1 << ((pport - first_port)-4);
                            } else {
                                xphy_lane_connection.ls_lane_mask = -1;
                            }

                        break;
                        case SOC_LANE_CONFIG_100G_3_4_3:
                            if ((pport - first_port) == 3 ||
                                    (pport - first_port) == 11) {
                                    xphy_lane_connection.ss_lane_mask = -1;
                                    xphy_lane_connection.ls_lane_mask = -1;
                            }
                            if (((pport - first_port) > 3) &&
                                ((pport - first_port) < 11)) {
                                xphy_lane_connection.ss_lane_mask = 0x1 << ((pport - first_port) - 1);
                                xphy_lane_connection.ls_lane_mask = 0x1 << ((pport - first_port) - 1);
                            }
                            /* Temp fix for sesto gearbox */
                            if ((((pport - first_port) > 3) &&
                               ((pport - first_port) < 8))) {
                                xphy_lane_connection.ls_lane_mask = 0x1 << ((pport - first_port)-4);
                            } else {
                                xphy_lane_connection.ls_lane_mask = -1;
                            }


                        break;
                        case SOC_LANE_CONFIG_100G_2_4_4:
                            if ((pport - first_port) == 2 ||
                                    (pport - first_port) == 3) {
                                    xphy_lane_connection.ss_lane_mask = -1;
                                    xphy_lane_connection.ls_lane_mask = -1;
                            }
                            if ((pport - first_port) > 3) {
                                xphy_lane_connection.ss_lane_mask = 0x1 << ((pport - first_port) - 2);
                                xphy_lane_connection.ls_lane_mask = 0x1 << ((pport - first_port) - 2);
                            }
                            /* Temp fix for sesto gearbox */
                            if ((pport - first_port) > 5) {
                                xphy_lane_connection.ls_lane_mask = -1;
                            }
                            if ((((pport - first_port) > 3) &&
                               ((pport - first_port) < 8))) {
                                xphy_lane_connection.ls_lane_mask = 0x1 << ((pport - first_port)-4);
                            } else {
                                xphy_lane_connection.ls_lane_mask = -1;
                            }

                        break;
                        default:
                            xphy_lane_connection.ss_lane_mask = 0x1 << (pport - first_port);
                            xphy_lane_connection.ls_lane_mask = 0x1 << (pport - first_port);
                        break;
                    }
                    /* if the system is on a Gearbox mode then line side port
                     beyond 3 should be set to -1 on an external phy */
                } else {
                    /* for non 100G port like 10G and 40G */
                    /*This only support 2x25G to 4x10G gearbox.*/
                    if(pport > 0){
                       if (xphy_gearbox_mode) {
                           xphy_lane_connection.ss_lane_mask = 0x1 << ((pport - 1) % 4);
                           xphy_lane_connection.ls_lane_mask = 0x3 << (((pport - 1) % 4) * 2);
                       } else {
                           xphy_lane_connection.ss_lane_mask = 0x1 << ((pport - 1) % 4);
                           xphy_lane_connection.ls_lane_mask = 0x1 << ((pport - 1) % 4);
                       }
                    }
                }

            } else {
                ext_phy_addr = topology.xphy_id;
                xphy_lane_connection.xphy_id = topology.xphy_id;
                if (topology.sys_lane != -1) {
                    xphy_lane_connection.ss_lane_mask = 0x1 << topology.sys_lane;
                } else {
                    xphy_lane_connection.ss_lane_mask = -1;
                }
                if (topology.line_lane != -1) {
                    xphy_lane_connection.ls_lane_mask = 0x1 << topology.line_lane;
                } else {
                    xphy_lane_connection.ls_lane_mask = -1;
                }
            }

            if((PORTMOD_SUCCESS(rv)) && ((ext_phy_addr != 0xFF) || (phy_chain_length >= chain_idx))) {

                /* get default xphy  core_access and updated with xphy_addr. */
                soc_esw_portctrl_xphy_default_core_access_get(unit, &core_access);
                core_access.access.addr = ext_phy_addr;

                PORTMOD_IF_ERROR_RETURN(portmod_xphy_add(unit, ext_phy_addr, &core_access, &xphy_idx));

                /* if entry is not added, release user_acc from core_access. */
                if (xphy_idx == PORTMOD_XPHY_EXISTING_IDX){
                    if (core_access.access.user_acc != NULL){
                        sal_free(core_access.access.user_acc);
                    }
                } else {
                    /* record the user_acc  */
                    portmod_chain_xphy_user_access_store(unit, xphy_idx, core_access.access.user_acc);
                }
                /* call external phy lane attach */
                portmod_xphy_lane_attach(unit, pport, topology.xphy_index, &xphy_lane_connection);

                /* update the value from config */
                ref_clk = phymodRefClk156Mhz;
                fw_load_method = phymodFirmwareLoadMethodExternal;
                /*  xphy_core_info. polarity*/
                PORTMOD_IF_ERROR_RETURN(portmod_xphy_primary_core_num_set(unit, ext_phy_addr, primary_core_num));
                PORTMOD_IF_ERROR_RETURN(portmod_xphy_ref_clk_set(unit, ext_phy_addr, ref_clk));
                PORTMOD_IF_ERROR_RETURN(portmod_xphy_fw_load_method_set(unit, ext_phy_addr, fw_load_method));
                soc_esw_portctrl_core_config_update(unit, port, pport, (pport - first_port),  ext_phy_addr, chain_idx);
            }

            /*
             * For  only 1 ext phy allowd, if additional phy needed, need to parse the config
             * to get external phy addr.
             */
            ext_phy_addr = 0xFF; /* NO Additional PHY */
        } /* while each phy at index */
    } /* SOC_PBMP_ITER */
    return SOC_E_NONE;
}



/*
 * Function:
 *      soc_esw_portctrl_reset_tsc0_cb
 * Purpose:
 *      Reset TSC by setting TSC control register.
 *      Index 0 means TSC core index0, first TSC ctrl register index,
 *      which resides in PGW block.
 *      For TD2+ case, the register is PGW_TSC0_CTRL_REG
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      port             - (IN)  Port number.
 *      in_reset         - (IN)  reset status
 * Returns:
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_reset_tsc0_cb(int unit, int port, uint32 in_reset)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    return (!in_reset? soc_tsc_xgxs_reset(unit, port, 0) : SOC_E_NONE);
#else
    return SOC_E_NONE;
#endif
}

/*
 * Function:
 *      soc_esw_portctrl_reset_tsc1_cb
 * Purpose:
 *      Reset TSC by setting TSC control register.
 *      Index 1 means TSC core index1, 2nd TSC ctrl register index,
 *      which resides in PGW block.
 *      For TD2+ case, the register is PGW_TSC1_CTRL_REG
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      port             - (IN)  Port number.
 *      in_reset         - (IN)  reset status
 * Returns:
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_reset_tsc1_cb(int unit, int port, uint32 in_reset)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    return (!in_reset? soc_tsc_xgxs_reset(unit, port, 1) : SOC_E_NONE);
#else
    return SOC_E_NONE;
#endif
}

/*
 * Function:
 *      soc_esw_portctrl_reset_tsc2_cb
 * Purpose:
 *      Reset TSC by setting TSC control register.
 *      Index 2 means TSC core index2, 3rd TSC ctrl register index,
 *      which resides in PGW block.
 *      For TD2+ case, the register is PGW_TSC2_CTRL_REG
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      port             - (IN)  Port number.
 *      in_reset         - (IN)  reset status
 * Returns:
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_reset_tsc2_cb(int unit, int port, uint32 in_reset)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    return (!in_reset? soc_tsc_xgxs_reset(unit, port, 2) : SOC_E_NONE);
#else
    return SOC_E_NONE;
#endif
}

/*
 * Function:
 *      soc_esw_portctrl_reset_tsc3_cb
 * Purpose:
 *      Reset TSC by setting TSC control register.
 *      Index 3 means TSC core index3, 4th TSC ctrl register index,
 *      which resides in PGW block.
 *      For TD2+ case, the register is PGW_TSC3_CTRL_REG
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      port             - (IN)  Port number.
 *      in_reset         - (IN)  reset status
 * Returns:
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_reset_tsc3_cb(int unit, int port, uint32 in_reset)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    return (!in_reset? soc_tsc_xgxs_reset(unit, port, 3) : SOC_E_NONE);
#else
    return SOC_E_NONE;
#endif
}

#ifdef BCM_MONTEREY_SUPPORT
static int
_check_cpri_speed (
    int speedi, portmod_encap_t int_mode)
{
    int speedo;
    if(int_mode == _SHR_PORT_ENCAP_RSVD4) {
        switch(speedi) {
        case 3072:
            speedo = cprimodRsvd4SpdMult4X;
            break;
        case 6144:
            speedo = cprimodRsvd4SpdMult8X;
            break;
        default:
            speedo = -1;
            break;
        }
    } else if(int_mode == _SHR_PORT_ENCAP_CPRI) {
        switch(speedi) {
        case 1228:
            speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_1228P8;
            break;
        case 2457:
            speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_2457P6;
            break;
        case 3072:
            speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_3072P0;
            break;
        case 4915:
            speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_4915P2;
            break;
        case 6144:
            speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_6144P0;
            break;
        case 9830:
            speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_9830P4;
            break;
        case 10137:
            speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_10137P6;
            break;
        case 12165:
            speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_12165P12;
            break;
        case 24330:
            speedo = CPRIMOD_CPRI_SUPPORTED_PORT_SPEED_24330P24;
            break;
        default:
            speedo = -1;
            break;
        }
    } else {
        speedo = -1;
    }
    return speedo;
}
#endif



/*
 * Function:
 *      soc_esw_portctrl_config_get
 * Purpose:
 *      Get interface and initialization configuration
 *      for given logical port.
 * Parameters:
 *      unit              - (IN) Unit number.
 *      port              - (IN) Logical port number.
 *      interface_config  - (IN/OUT) Port interface configuration.
 *      init_config       - (IN/OUT) Port initialization configuration.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_portctrl_config_get(int unit, soc_port_t port,
                            portmod_port_interface_config_t* interface_config,
                            portmod_port_init_config_t* init_config,
                            phymod_operation_mode_t *phy_op_mode)
{
    /* soc_port_if_t interface_type; */
    int port_num_lanes, fiber_pref = 0, is_scrambler = 0, i;
    int fs_cl72=0, max_speed, speed;
    /* phymod_tx_t *p_tx; */
    uint32  preemphasis=0xffffffff, driver_current=0xffffffff;
    uint32  port_preemphasis, port_driver_current;
    uint32  lane_preemphasis, lane_driver_current;
    /*int rxaui_mode = 0; */
    int ref_clk_prop = 0, encap_mode;
    int adjust_num = 0;
#ifdef BCM_MONTEREY_SUPPORT
    int port_speed_cpri = 0;
    int roe_speed_rsvd4, roe_speed_cpri;
#endif

    /* Initialize both interface config and init config */
    PORTMOD_IF_ERROR_RETURN
        (portmod_port_init_config_t_init(unit, init_config));
    PORTMOD_IF_ERROR_RETURN
        (portmod_port_interface_config_t_init(unit, interface_config));

    port_num_lanes = SOC_INFO(unit).port_num_lanes[port];
    interface_config->port_num_lanes = port_num_lanes;

    *phy_op_mode = soc_property_port_get(unit, port, spn_PHY_PCS_REPEATER,
                                         phymodOperationModeRetimer);

    init_config->is_hg = (PBMP_MEMBER(SOC_HG2_PBM(unit), port) ||
                          PBMP_MEMBER(PBMP_HG_ALL(unit), port));
    if (init_config->is_hg) {
        PHYMOD_INTF_MODES_HIGIG_SET(interface_config);
    }

    fiber_pref = soc_property_port_get(unit, port,
                                       spn_SERDES_FIBER_PREF, fiber_pref);
    if (fiber_pref) {
        PHYMOD_INTF_MODES_FIBER_SET(interface_config);
    }



    if (IS_C_PORT(unit, port) ||
        (IS_CXX_PORT(unit, port) && IS_CL_PORT(unit, port))) {
        PHYMOD_INTF_MODES_TRIPLE_CORE_SET(interface_config);

        switch(SOC_INFO(unit).port_100g_lane_config[port]) {
            case SOC_LANE_CONFIG_100G_4_4_2:
                PHYMOD_INTF_MODES_TC_442_SET(interface_config);
                break ;
            case SOC_LANE_CONFIG_100G_2_4_4:
                PHYMOD_INTF_MODES_TC_244_SET(interface_config);
                break ;
            case SOC_LANE_CONFIG_100G_3_4_3:
                PHYMOD_INTF_MODES_TC_343_SET(interface_config);
                break ;
            default:
                PHYMOD_INTF_MODES_TC_343_SET(interface_config);
                break ;
        }
        init_config->port_fallback_lane =
            SOC_INFO(unit).port_fallback_lane[port];
    }


    /* Should be changed based on the speed later in the flow  */
    if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port))
        interface_config->interface =  SOC_PORT_IF_NULL;
    else
        interface_config->interface = soc_property_port_get(unit, port,
                                                        spn_SERDES_IF_TYPE,
                                                        SOC_PORT_IF_XFI);
    max_speed = SOC_INFO(unit).port_speed_max[port];
    interface_config->max_speed = max_speed ;

    /* Fill in init config */
    init_config->an_cl37 = soc_property_port_get(unit, port,
                                    spn_PHY_AN_C37, 0);

    /* default AN mode would be cl73 with no BAM */
    init_config->an_cl73 = soc_property_port_get(unit, port,
                                    spn_PHY_AN_C73, PORTMOD_CL73_WO_BAM);
    init_config->cl37_sgmii_RESTART_CNT = soc_property_port_get(unit, port,
                                            spn_SERDES_CL37_SGMII_RESTART_COUNT,
                                            init_config->cl37_sgmii_RESTART_CNT);

    init_config->an_master_lane = soc_property_port_get(unit, port,
                                                        spn_PHY_AUTONEG_MASTER_LANE,
                                                        0);

    /* an_cl72 means whether to turn on CL72 when AN is on. Default value
     * should be 1 according to the IEEE spec.
     */
    init_config->an_cl72 = soc_property_port_get(unit, port,
                                                 spn_PHY_AN_C72, 1);

    /*  Skip the TXFIR initialization in a restart event during the AN link training.
     */
    init_config->an_cl72_tx_init_skip_on_restart = soc_property_port_get(unit, port,
                                                 spn_PHY_AN_C72_TX_INIT_SKIP_ON_RESTART, 0);

   /* Get the timestamp adjust */
    adjust_num = COUNTOF(ts_adjust_props);
    for (i = 0; i < adjust_num; i++) {
         init_config->port_ts_adjust[i].speed = ts_adjust_props[i].speed;
         init_config->port_ts_adjust[i].osts_adjust = soc_property_port_get(unit, port, ts_adjust_props[i].props[0], 0);
         init_config->port_ts_adjust[i].tsts_adjust = soc_property_port_get(unit, port, ts_adjust_props[i].props[1], 0);
    }

    init_config->txpi_mode = soc_property_port_get(unit, port, spn_SERDES_TXPI_MODE, 0);
    init_config->txpi_sdm_scheme = soc_property_port_get(unit, port, spn_TXPI_SDM_SCHEME, 0);

#if 0
    if (SOC_IS_TOMAHAWKX(unit)) {
        if(an_cl73) {
            init_config->an_mode =
               ((an_cl73 == 1) ? phymod_AN_MODE_CL73 : phymod_AN_MODE_CL73BAM);
            init_config->an_cl72 = 1;
        } else if (an_cl37) {
            init_config->an_mode =
               ((an_cl37 == 1) ? phymod_AN_MODE_CL37BAM : phymod_AN_MODE_CL37);
            init_config->an_cl72 = 0;
        } else {
            /* in TH/TH2, only management ports are pm4x10, others are pm4x25 */
            if (IS_MANAGEMENT_PORT(unit, port)) {
                init_config->an_mode = phymod_AN_MODE_CL37;
                init_config->an_cl72 = 0;
            } else {
                init_config->an_mode =
                    ((is_hg) ? phymod_AN_MODE_CL73BAM : phymod_AN_MODE_CL73);
                init_config->an_cl72 = 1;
            }
        }
    }
#endif

    init_config->an_fec = soc_property_port_get(unit, port,
                                                spn_PHY_AN_FEC,
                                                init_config->an_fec);

    init_config->serdes_fec_enable = soc_property_port_get(unit, port,
                                             spn_SERDES_FEC_ENABLE,
                                             init_config->serdes_fec_enable);

    fs_cl72  = soc_property_port_get(unit, port, spn_PORT_INIT_CL72,
                                                fs_cl72);
    if(fs_cl72) {
        init_config->fs_cl72 = PHYMOD_INTF_F_CL72_REQUESTED_BY_CNFG ;
    }

    speed = SOC_INFO(unit).port_init_speed[port] > 0 ?
            SOC_INFO(unit).port_init_speed[port] :
            SOC_INFO(unit).port_speed_max[port];

    /* TSCE:for the  Dual lane 10G mode scrambler should be enabled
     * to get the link up. To interop with legacy devices like TD+
     * that does not require scrambler at dual lane 10g mode
     * spn_SERDES_SCRAMBLER_ENABLE should be explicitly set to "0" from
     * the config
     */
    is_scrambler = 0;
    if (port_num_lanes == 2 && speed == 10000) {
        is_scrambler = 1;
    }

    is_scrambler = soc_property_port_get(unit, port,
                                         spn_SERDES_SCRAMBLER_ENABLE, is_scrambler);
    if (is_scrambler) {
        PHYMOD_INTF_MODES_SCR_SET(interface_config);
    }


    interface_config->speed =
        soc_property_port_get(unit, port, spn_PORT_INIT_SPEED, speed);

    init_config->pll_divider_req      =
        soc_property_port_get(unit, port, spn_XGXS_PHY_PLL_DIVIDER, 0xA);

#if (defined BCM_HELIX5_SUPPORT) || (defined BCM_MAVERICK2_SUPPORT) || (defined BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_HELIX5(unit) ||
        SOC_IS_HURRICANE4(unit) ||
        (soc_feature(unit, soc_feature_vco_freq_config_enable)) ||
        (SOC_IS_MAVERICK2(unit) && SOC_INFO(unit).bandwidth == 2000000) ||
        SOC_IS_FIRELIGHT(unit)) {
        init_config->serdes_1000x_at_25g_vco = 1;
        init_config->serdes_10g_at_25g_vco = 1;
    }
#endif
    init_config->serdes_1000x_at_6250_vco =
        soc_property_port_get(unit, port,
                              spn_SERDES_1000X_AT_6250_VCO,
                              init_config->serdes_1000x_at_6250_vco);

    init_config->serdes_1000x_at_12500_vco =
        soc_property_port_get(unit, port,
                      spn_SERDES_1000X_AT_12500_VCO,
                      init_config->serdes_1000x_at_12500_vco);

    init_config->serdes_1000x_at_25g_vco =
        soc_property_port_get(unit, port,
                      spn_SERDES_1000X_AT_25G_VCO,
                      init_config->serdes_1000x_at_25g_vco);

    init_config->serdes_10g_at_25g_vco =
        soc_property_port_get(unit, port,
                      spn_SERDES_10G_AT_25G_VCO,
                      init_config->serdes_10g_at_25g_vco);

    if (init_config->serdes_1000x_at_12500_vco) {
      /*
           * VCO = (PLL_DIV) * (REF_CLK)
           * REF_CLK = 156.25
           * VCO     = 12500
           * PLL_DIV = 80
           */
        init_config->pll_divider_req = 80;
        interface_config->pll_divider_req = 80;
    } else if (init_config->serdes_1000x_at_6250_vco) {
        /*
         * VCO = (PLL_DIV) * (REF_CLK)
         * REF_CLK = 156.25
         * VCO     = 6250
         * PLL_DIV = 40
         */
        init_config->pll_divider_req = 40;
        interface_config->pll_divider_req = 40;
    } else if (init_config->serdes_1000x_at_25g_vco ||
               init_config->serdes_10g_at_25g_vco) {
            /*
             * VCO = (PLL_DIV) * (REF_CLK)
             * REF_CLK = 156.25
             * VCO     = 25G
             * PLL_DIV = 165
             */
            init_config->pll_divider_req = 165;
            interface_config->pll_divider_req = 165;
        }
    init_config->cx4_10g = soc_property_port_get(unit, port,
                                                 spn_10G_IS_CX4, TRUE);

    SOC_IF_ERROR_RETURN(portmod_common_default_interface_get(interface_config));

#ifdef BCM_HELIX5_SUPPORT
    /* HX5 supports QSGMII and USXGMII mode in Pm4x10_qtc core. */
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        int phy_port, base_port, port_gmii_mode;

        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        if (SOC_PORT_BLOCK_TYPE(unit, phy_port) == SOC_BLK_GXPORT) {
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                base_port = SOC_HR4_PORT_BLOCK_BASE_PORT(port);
                port_gmii_mode = soc_property_phys_port_get(unit, base_port,
                                                            spn_PORT_GMII_MODE, 0);
            } else
#endif /* BCM_HURRICANE4_SUPPORT */
            {
                base_port = phy_port - (phy_port - 1) % 16;
                port_gmii_mode = soc_property_phys_port_get(unit, base_port,
                                                            spn_PORT_GMII_MODE, 0);
            }
            switch (port_gmii_mode) {
                case 0:
                    init_config->qtc_mode = portmodQtcModeEthernet;
                    break;
                case 1:
                    init_config->qtc_mode = portmodQtcModeQsgmii;
                    break;
                case 2:
                    init_config->qtc_mode = portmodQtcModeUsxgmii;
                    break;
                default:
                    init_config->qtc_mode = portmodQtcModeInvalid;
                    break;
            }
            if ((init_config->qtc_mode == portmodQtcModeQsgmii) ||
                (init_config->qtc_mode == portmodQtcModeUsxgmii)) {
                interface_config->interface = SOC_PORT_IF_SGMII;
            }
        }
    }
#endif

#ifdef BCM_GREYHOUND2_SUPPORT
    /* FL supports QSGMII and USXGMII mode in Pm4x10_qtc core. */
    if (SOC_IS_FIRELIGHT(unit)) {
        int phy_port, base_port, port_gmii_mode;

        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        if (SOC_PORT_BLOCK_TYPE(unit, phy_port) == SOC_BLK_GPORT) {
            base_port = phy_port - (phy_port - 2) % 16;
            port_gmii_mode = soc_property_phys_port_get(unit, base_port,
                                                            spn_PORT_GMII_MODE, 0);
            switch (port_gmii_mode) {
                case 0:
                    init_config->qtc_mode = portmodQtcModeEthernet;
                    break;
                case 1:
                    init_config->qtc_mode = portmodQtcModeQsgmii;
                    break;
                case 2:
                    init_config->qtc_mode = portmodQtcModeUsxgmii;
                    break;
                default:
                    init_config->qtc_mode = portmodQtcModeInvalid;
                    break;
            }
            if ((init_config->qtc_mode == portmodQtcModeQsgmii) ||
                (init_config->qtc_mode == portmodQtcModeUsxgmii)) {
                interface_config->interface = SOC_PORT_IF_SGMII;
            }
        }
    }
#endif

    if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port))
        interface_config->interface =  SOC_PORT_IF_NULL;

    /* get internal serdes preemphasis and current property */
    port_preemphasis = 0xffffffff;
    port_driver_current = 0xffffffff;
    port_preemphasis = soc_property_port_get(unit, port,
                                        spn_SERDES_PREEMPHASIS, port_preemphasis);
    port_driver_current = soc_property_port_get(unit, port,
                                           spn_SERDES_DRIVER_CURRENT,
                                           port_driver_current);
    for (i=0; i<port_num_lanes; i++){
        init_config->tx_params_user_flag[i] = 0;
    }
    for (i=0; i<port_num_lanes; i++){
        lane_preemphasis = 0xffffffff;
        lane_preemphasis = soc_property_port_suffix_num_get(unit, port, i,
                              spn_SERDES_PREEMPHASIS, "lane", lane_preemphasis);
        if (lane_preemphasis != 0xffffffff){
            preemphasis = lane_preemphasis;
        }else if (port_preemphasis != 0xffffffff){
            preemphasis = port_preemphasis;
        }else {
            preemphasis = 0xffffffff;
        }

        if(preemphasis != 0xffffffff){
            init_config->tx_params_user_flag[i] |=
                PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG;
            init_config->tx_params[i].pre  = preemphasis & 0xff;
            init_config->tx_params[i].main = (preemphasis & 0xff00) >> 8;
            init_config->tx_params[i].post = (preemphasis & 0xff0000) >> 16;
        }
    }
    for (i=0; i<port_num_lanes; i++){
        lane_driver_current = 0xffffffff;
        lane_driver_current = soc_property_port_suffix_num_get(unit, port, i,
                                 spn_SERDES_DRIVER_CURRENT, "lane", lane_driver_current);
        if (lane_driver_current != 0xffffffff){
            driver_current = lane_driver_current;
        }else if (port_driver_current != 0xffffffff){
            driver_current = port_driver_current;
        }else {
            driver_current = 0xffffffff;
        }

        if(driver_current != 0xffffffff){
            init_config->tx_params_user_flag[i] |= PORTMOD_USER_SET_TX_AMP_BY_CONFIG;
            init_config->tx_params[i].amp  = driver_current;
        }
    }

    /* get external phy preemphasis and current property */
    port_preemphasis = 0xffffffff;
    port_driver_current = 0xffffffff;
    port_preemphasis = soc_property_port_get(unit, port,
                                        spn_PHY_PREEMPHASIS, preemphasis);
    port_driver_current = soc_property_port_get(unit, port,
                                           spn_PHY_DRIVER_CURRENT,
                                           driver_current);
    for (i=0; i<port_num_lanes; i++){
        init_config->ext_phy_tx_params_user_flag[i] = 0;
    }
    for (i=0; i<port_num_lanes; i++){
        lane_preemphasis = 0xffffffff;
        lane_preemphasis = soc_property_port_suffix_num_get(unit, port, i,
                              spn_PHY_PREEMPHASIS, "lane", lane_preemphasis);
        if (lane_preemphasis != 0xffffffff){
            preemphasis = lane_preemphasis;
        }else if (port_preemphasis != 0xffffffff){
            preemphasis = port_preemphasis;
        }else {
            preemphasis = 0xffffffff;
        }

        if(preemphasis != 0xffffffff){
            init_config->ext_phy_tx_params_user_flag[i] |=
                PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG;
            init_config->ext_phy_tx_params[i].pre  = preemphasis & 0xff;
            init_config->ext_phy_tx_params[i].main = (preemphasis & 0xff00) >> 8;
            init_config->ext_phy_tx_params[i].post = (preemphasis & 0xff0000) >> 16;
        }
    }
    for (i=0; i<port_num_lanes; i++){
        lane_driver_current = 0xffffffff;
        lane_driver_current = soc_property_port_suffix_num_get(unit, port, i,
                                 spn_PHY_DRIVER_CURRENT, "lane", lane_driver_current);
        if (lane_driver_current != 0xffffffff){
            driver_current = lane_driver_current;
        }else if (port_driver_current != 0xffffffff){
            driver_current = port_driver_current;
        }else {
            driver_current = 0xffffffff;
        }

        if(driver_current != 0xffffffff){
            init_config->ext_phy_tx_params_user_flag[i] |= PORTMOD_USER_SET_TX_AMP_BY_CONFIG;
            init_config->ext_phy_tx_params[i].amp  = driver_current;
        }
    }

    if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
        (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_port_config_get)) {
         SOC_IF_ERROR_RETURN
            (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_port_config_get(
                unit, port, (portmod_port_init_config_t *)init_config));
    }

    ref_clk_prop = soc_property_port_get(unit, port,
                                spn_XGXS_LCPLL_XTAL_REFCLK, 156);

    if ((ref_clk_prop == 156) ||( ref_clk_prop == 0)) {
        init_config->ref_clk = phymodRefClk156Mhz;     /**< 156.25MHz */
    } else if (ref_clk_prop == 125) {
        init_config->ref_clk = phymodRefClk125Mhz;     /**< 125MHz */
    } else {
        LOG_ERROR(BSL_LS_SOC_PORT,
                (BSL_META_UP(unit, port,
                          "ERROR: This %d MHz clk freq is not supported. Only 156 MHz and 125 MHz.\n"),ref_clk_prop));
    }
    init_config->ref_clk_overwrite = 1;

    /* Internal Serdes interface setting if user want to overwrite. */
    interface_config->serdes_interface =
        soc_property_port_get(unit, port, spn_SERDES_IF_TYPE,
                              SOC_PORT_IF_NULL);

    if (IS_HG_PORT(unit, port)) {
        if (soc_feature(unit, soc_feature_no_higig_plus)) {
            encap_mode = SOC_ENCAP_HIGIG2;
        } else {
            encap_mode = soc_property_port_get(unit, port,
                         spn_HIGIG2_HDR_MODE, 0) ?
                         SOC_ENCAP_HIGIG2 : SOC_ENCAP_HIGIG;
        }
        interface_config->encap_mode = encap_mode;
    }
#ifdef BCM_MONTEREY_SUPPORT
    if (IS_ROE_PORT(unit, port)) {
        port_speed_cpri = _check_cpri_speed(SOC_INFO(unit).port_init_speed[port], _SHR_PORT_ENCAP_CPRI);
        if(port_speed_cpri >= 0) {
            if (IS_RSVD4_PORT(unit, port)) {
                roe_speed_rsvd4 = _check_cpri_speed(SOC_INFO(unit).port_init_speed[port], _SHR_PORT_ENCAP_RSVD4);
                roe_speed_cpri = -1;
            } else {
                roe_speed_cpri = _check_cpri_speed(SOC_INFO(unit).port_init_speed[port], _SHR_PORT_ENCAP_CPRI);
                roe_speed_rsvd4 = -1;
            }
        } else {
            roe_speed_cpri = soc_property_port_get(unit, port, spn_CPRI_PORT_INIT_SPEED_ID, -1);
            roe_speed_rsvd4 =  soc_property_port_get(unit, port, spn_RSVD4_PORT_INIT_SPEED_ID, -1);
        }
        if ((roe_speed_cpri == -1)&&(roe_speed_rsvd4 == -1)) {
            LOG_WARN(BSL_LS_SOC_PORT,
                (BSL_META_UP(unit, port,
                "WARNING: For cpri port speed is not specified moving to 1228.8 M speed\n")));
            roe_speed_cpri = 0;
        }
        if ((roe_speed_cpri != -1)&&(roe_speed_rsvd4 != -1)) {
            LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_UP(unit, port,
                "ERROR: For cpri port : two speeds are specified : can not have cpri_port_init_speed_id_%d and rsvd4_port_init_speed_id_%d on same port\n"), port, port));
            return SOC_E_PARAM;
        }
        if (roe_speed_cpri != -1) {
            interface_config->encap_mode = _SHR_PORT_ENCAP_CPRI;
            interface_config->speed = roe_speed_cpri;
        } else if (roe_speed_rsvd4 != -1) {
            interface_config->encap_mode = _SHR_PORT_ENCAP_RSVD4;
            interface_config->speed = roe_speed_rsvd4;
        }
        init_config->roe_reserved_bit = soc_property_port_get(unit, port,
                         spn_ROE_RESERVED_BIT, 0);
        init_config->roe_stuffing_bit= soc_property_port_get(unit, port,
                         spn_ROE_STUFFING_BIT, 0);
        interface_config->serdes_scrambler_seed = soc_property_port_get(unit, port,
                         spn_SERDES_SCRAMBLER_SEED, 0x55);
        interface_config->serdes_scrambler_enable = soc_property_port_get(unit, port,
                         spn_SERDES_SCRAMBLER_ENABLE, 0);
    }
#endif
    init_config->parity_enable = soc_property_port_get(unit, port,
                                                      spn_PARITY_ENABLE, 0);

    
    interface_config->port_op_mode =
            soc_property_port_get(unit, port, spn_PHY_PCS_REPEATER, TRUE);

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_portctrl_pm_portmod_init
 *
 * Purpose:
 *      Call device specific portmod init function
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_UNAVAIL : if fuction vector is not correctly set
 *      rv            : return rv
 */
STATIC int
soc_portctrl_pm_portmod_init(int unit)
{
    int rv = SOC_E_NONE;

    if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
        (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_init)) {
        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_init(unit);
    } else {
        rv = SOC_E_UNAVAIL;
    }
    return rv;
}

/*
 * Function:
 *      soc_portctrl_pm_portmod_deinit
 *
 * Purpose:
 *      Call device specific portmod deinit function
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_UNAVAIL : if fuction vector is not correctly set
 *      rv            : return rv
 */
STATIC int
soc_portctrl_pm_portmod_deinit(int unit)
{
    int rv = SOC_E_NONE;

    if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
        (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_deinit)) {
        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_deinit(unit);
    } else {
        rv = SOC_E_UNAVAIL;
    }
    return rv;
}

/*
 * Function:
 *      soc_portctrl_pm_type_get
 *
 * Purpose:
 *      Call device specific portmod pm_type get function
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      phy_port - (IN) Physical port.
 *      pm_type  - (OUT) pm_type
 * Returns:
 *      SOC_E_UNAVAIL : if fuction vector is not correctly set
 *      rv            : return rv
 */
int
soc_portctrl_pm_type_get(int unit, soc_port_t phy_port,
                         int* pm_type)
{
    int rv = SOC_E_NONE;

    if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
        (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_type_get)) {
        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_type_get(
                 unit, phy_port, pm_type);
    } else {
        rv = SOC_E_UNAVAIL;
    }
    return rv;
}


extern soc_portctrl_functions_t soc_td2p_portctrl_func;
extern soc_portctrl_functions_t soc_th_portctrl_func;
extern soc_portctrl_functions_t soc_gh_portctrl_func;
extern soc_portctrl_functions_t soc_ap_portctrl_func;
extern soc_portctrl_functions_t soc_hr3_portctrl_func;
extern soc_portctrl_functions_t soc_mn_portctrl_func;
extern soc_portctrl_functions_t soc_th2_portctrl_func;
extern soc_portctrl_functions_t soc_th3_portctrl_func;
extern soc_portctrl_functions_t soc_hx5_portctrl_func;

/*
 * Function:
 *      soc_portctrl_functions_register
 *
 * Purpose:
 *      This will set given function vector to soc_control->soc_portctrl_functions
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      func     - (IN) device specific portctrl function vectors
 * Returns:
 *      SOC_E_NONE
 */
int
soc_esw_portctrl_functions_register(int unit, soc_driver_t *drv)
{
    switch (drv->type) {
#ifdef BCM_MAVERICK2_SUPPORT
    case SOC_CHIP_BCM56770_A0:
    case SOC_CHIP_BCM56771_A0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_mv2_portctrl_func;
        break;
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
    case SOC_CHIP_BCM56470_A0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_fb6_portctrl_func;
        break;
#endif
#ifdef BCM_HELIX5_SUPPORT
    case SOC_CHIP_BCM56370_A0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_hx5_portctrl_func;
        break;
#endif
#ifdef BCM_HURRICANE4_SUPPORT
    case SOC_CHIP_BCM56275_A0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_hr4_portctrl_func;
        break;
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    case SOC_CHIP_BCM56870_A0:
    case SOC_CHIP_BCM56873_A0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_td3_portctrl_func;
        break;
#endif
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    case SOC_CHIP_BCM56860_A0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_td2p_portctrl_func;
        break;
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    case SOC_CHIP_BCM56960_A0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_th_portctrl_func;
        break;
#endif
#ifdef BCM_GREYHOUND_SUPPORT
    case SOC_CHIP_BCM53400_A0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_gh_portctrl_func;
        break;
#endif
#ifdef BCM_FIRELIGHT_SUPPORT
    case SOC_CHIP_BCM56070_A0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_fl_portctrl_func;
        break;
#endif
#ifdef BCM_APACHE_SUPPORT
    case SOC_CHIP_BCM56560_A0:
    case SOC_CHIP_BCM56560_B0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_ap_portctrl_func;
        break;
#endif
#ifdef BCM_MONTEREY_SUPPORT
    case SOC_CHIP_BCM56670_A0:
    case SOC_CHIP_BCM56670_B0:
    case SOC_CHIP_BCM56670_C0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_mn_portctrl_func;
        break;
#endif
#ifdef BCM_TOMAHAWK2_SUPPORT
    case SOC_CHIP_BCM56970_A0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_th2_portctrl_func;
        break;
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    case SOC_CHIP_BCM56980_A0:
    case SOC_CHIP_BCM56980_B0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_th3_portctrl_func;
        break;
#endif
    default:
        break;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_portctrl_xport_type_verify
 * Purpose:
 *      Check if the encapsulation is changed from configured selection.
 *      Update the SOC port data structures to match the HW encapsulation selection.
 * Parameters:
 *      unit - XGS unit #.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      WARM_BOOT mode only
 */
STATIC int
soc_portctrl_xport_type_verify(int unit)
{
    int        mode;
    soc_port_t port;

    PBMP_PORT_ITER(unit, port) {
        if (!SOC_PORT_USE_PORTCTRL(unit, port)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_encap_get(unit, port, &mode));

        if ((IS_XE_PORT(unit, port) || IS_CE_PORT(unit, port))
             && (mode != SOC_ENCAP_IEEE)) {
            if (mode == SOC_ENCAP_CPRI || mode == SOC_ENCAP_RSVD4) {
                soc_xport_type_update(unit, port, mode);
            } else {
               soc_xport_type_update(unit, port, TRUE);
            }
        } else if (IS_HG_PORT(unit, port)) {
           if (mode == SOC_ENCAP_IEEE) {
               soc_xport_type_update(unit, port, FALSE);
           }  else if (mode == SOC_ENCAP_CPRI || mode == SOC_ENCAP_RSVD4) {
               soc_xport_type_update(unit, port, mode);
           }
        } else if (IS_CPRI_PORT(unit, port) && (mode != SOC_ENCAP_CPRI)) {

                   if ( mode == SOC_ENCAP_RSVD4) {
                       soc_xport_type_update(unit, port, mode);
                   } else if (mode == SOC_ENCAP_IEEE) {
                       soc_xport_type_update(unit, port, FALSE);
                   } else {
                       soc_xport_type_update(unit, port,TRUE);
                   }
        } else if (IS_RSVD4_PORT(unit, port) && (mode != SOC_ENCAP_RSVD4)) {
                   if ( mode == SOC_ENCAP_CPRI) {
                       soc_xport_type_update(unit, port, mode);
                   } else if (mode == SOC_ENCAP_IEEE) {
                       soc_xport_type_update(unit, port, FALSE);
                   } else {
                       soc_xport_type_update(unit, port,TRUE);
                   }
        }


        if (mode == SOC_ENCAP_HIGIG2) {
            if (IS_HG_PORT(unit, port)) {
                SOC_HG2_ENABLED_PORT_ADD(unit, port);
            } else {
                /* This should not happen */
                return SOC_E_INTERNAL;
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_portctrl_phy_lane_config_fill
 * Purpose:
 *      Parses the serdes_lane_config soc property's suffixes to fill in the
 *      integer that's sent to portmod
 * Parameters:
 *      unit - XGS unit #.
 *      port - port of config being parsed
 *      speed_config - pointer to structure containing the int being filled out
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
_soc_portctrl_phy_lane_config_fill(int unit, soc_port_t port, void *speed_config)
{
#ifdef PORTMOD_SUPPORT
    int dfe = 1;
    int dfe_lp = 0;
    char *dfe_str;
    char *dfe_types[] = {"off", "on", "lp", NULL};
    int unreliable_los;
    int cl72_auto_polarity_en;
    int cl72_restart_timeout_en;
    int force_nr = 1;
    int force_er = 0;
    char *channel_mode_str;
    char *channel_mode_types[] = {"force_nr", "force_er", NULL};
    int lp_has_prec_en;
    int default_lane_config = 0;
    int i, found_valid;
    int media_type_int = 0;
    char *media_type;
    char *media_types[] = {"backplane", "copper", "optics", NULL};
    portmod_speed_config_t *config = (portmod_speed_config_t *)speed_config;

    /* fetch values from config file if user filled them in */
    dfe_str = soc_property_port_suffix_num_get_str(unit, port, 0,
            spn_SERDES_LANE_CONFIG, "dfe");

    if (dfe_str != NULL) {
        found_valid = 0;
        for (i = 0; dfe_types[i] != NULL; i++) {
            if (!sal_strncmp(dfe_types[i], dfe_str,
                        sal_strlen(dfe_types[i]))) {
                switch(i) {
                    case 0:
                        dfe = 0;
                        dfe_lp = 0;
                        found_valid = 1;
                        break;
                    case 1:
                        dfe = 1;
                        dfe_lp = 0;
                        found_valid = 1;
                        break;
                    case 2:
                        dfe = 1;
                        dfe_lp = 1;
                        found_valid = 1;
                        break;
                    default:
                        return SOC_E_INTERNAL;
                }
            }
        }

        if (found_valid == 0) {
            return SOC_E_CONFIG;
        }
    }

    media_type = soc_property_port_suffix_num_get_str(unit, port, 0,
            spn_SERDES_LANE_CONFIG, "media_type");

    /* get the media type integer from the string */
    if (media_type != NULL) {
        found_valid = 0;
        for (i = 0; media_types[i] != NULL; i++) {
            if (!sal_strncmp(media_types[i], media_type,
                            sal_strlen(media_types[i]))) {
                media_type_int = i;
                found_valid = 1;
                break;
            }
        }
        if (found_valid == 0) {
            return SOC_E_CONFIG;
        }
    }

    unreliable_los = soc_property_port_suffix_num_get(unit, port, 0,
            spn_SERDES_LANE_CONFIG, "unreliable_los", 0);

    cl72_auto_polarity_en =
            soc_property_port_suffix_num_get(unit, port, 0,
            spn_SERDES_LANE_CONFIG, "cl72_auto_polarity_en",
            0);

    cl72_restart_timeout_en =
            soc_property_port_suffix_num_get(unit, port, 0,
            spn_SERDES_LANE_CONFIG, "cl72_restart_timeout_en",
            0);

    channel_mode_str = soc_property_port_suffix_num_get_str(unit, port, 0,
            spn_SERDES_LANE_CONFIG, "channel_mode");

    if (channel_mode_str != NULL) {
        found_valid = 0;
        for (i = 0; channel_mode_types[i] != NULL; i++) {
            if (!sal_strncmp(channel_mode_types[i], channel_mode_str,
                            sal_strlen(channel_mode_types[i]))) {
                switch(i) {
                    case 0:
                        force_er = 0;
                        force_nr = 1;
                        found_valid = 1;
                        break;
                    case 1:
                        force_er = 1;
                        force_nr = 0;
                        found_valid = 1;
                        break;
                    default:
                        return SOC_E_INTERNAL;
                }
                break;
            }
        }
        if (found_valid == 0) {
            return SOC_E_CONFIG;
        }
    }


    lp_has_prec_en = soc_property_port_suffix_num_get(unit, port, 0,
            spn_SERDES_LANE_CONFIG, "lp_has_prec_en", 0);

    /* first set it to the default values */
    config->lane_config = default_lane_config;

    /* Insert the modified values into the phy lane config */
    config->lane_config |= ((dfe & 0x1) << 2);
    config->lane_config |= ((dfe_lp & 0x1) << 3);
    config->lane_config |= ((media_type_int & 0x3) << 5);
    config->lane_config |= ((unreliable_los & 0x1) << 7);
    config->lane_config |= ((cl72_auto_polarity_en & 0x1) << 9);
    config->lane_config |= ((cl72_restart_timeout_en & 0x1) << 10);
    config->lane_config |= ((force_nr & 0x1) << 12);
    config->lane_config |= ((force_er & 0x1) << 11);
    config->lane_config |= ((lp_has_prec_en & 0x1) << 13);

    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      soc_esw_portctrl_speed_config_fill
 * Purpose: This function fills in the speed_config structure for portmod.
 *          Starting with PM8x50, the port macro needs this information to
 *          set the speed of a port.
 *
 *          NOTE: If speed, num_lane, fec, link_training, or lane_config are
 *                passed as -1 parameters, then "default" values will be used.
 *
 *                For fec, link_training and lane_config, their values will
 *                try to be read from the config file first.
 *
 *                Otherwise, if those parameters are >= 0, they will be used.
 *
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      port            - (IN)  logical port
 *      speed           - (IN)  Port Speed
 *      num_lane        - (IN)  Number of lanes for port
 *      fec             - (IN)  Forward Error Correction
 *      link_training   - (IN)  link training field
 *      lane_config     - (IN)  lane config field
 *      speed_config    - (OUT) the speed config structure to pass to portmod
 *
 * Returns: SOC_E_X
 */
int
soc_esw_portctrl_speed_config_fill(int unit, soc_port_t port,
                int speed,
                int num_lane,
                int fec,
                int link_training,
                int lane_config,
                void *speed_config)
{
#ifdef PORTMOD_SUPPORT
    soc_info_t *si = &SOC_INFO(unit);
    portmod_speed_config_t *config = (portmod_speed_config_t *)speed_config;

    if (speed >= 0) {
        config->speed = speed;
    } else {
        if (!SOC_PBMP_PORT_VALID(port) || IS_CPU_PORT(unit, port) ||
                IS_MANAGEMENT_PORT(unit, port) || IS_LB_PORT(unit, port)) {
                return SOC_E_PARAM;
        }
        config->speed = si->port_speed_max[port];
    }

    if (num_lane >= 0) {
        config->num_lane = num_lane;
    } else {
        if (!SOC_PBMP_PORT_VALID(port) || IS_CPU_PORT(unit, port) ||
                IS_MANAGEMENT_PORT(unit, port) || IS_LB_PORT(unit, port)) {
                return SOC_E_PARAM;
        }
        config->num_lane = si->port_num_lanes[port];
    }

    if (fec >= 0) {
        config->fec = fec;
    } else {
        if (!SOC_PBMP_PORT_VALID(port) || IS_CPU_PORT(unit, port) ||
                IS_MANAGEMENT_PORT(unit, port) || IS_LB_PORT(unit, port)) {
                return SOC_E_PARAM;
        }

        /* Read in FEC from config file if it was specified */
        fec = soc_property_port_get(unit, port, spn_PORT_FEC, -1);

        if (fec >= 0) {
            /* Mapping user config 'port_fec' to soc_port_phy_fec_t */
            switch (fec) {
                case 0:
                    config->fec = (portmod_fec_t)socPortPhyFecNone;
                    break;
                case 1:
                    config->fec = (portmod_fec_t)socPortPhyFecBaseR;
                    break;
                case 2:
                    config->fec = (portmod_fec_t)socPortPhyFecRsFec;
                    break;
                case 3:
                    config->fec = (portmod_fec_t)socPortPhyFecRs544;
                    break;
                case 4:
                    config->fec = (portmod_fec_t)socPortPhyFecRs272;
                    break;
                case 9:
                    config->fec = (portmod_fec_t)socPortPhyFecRs544_2xN;
                    break;
                case 10:
                    config->fec = (portmod_fec_t)socPortPhyFecRs272_2xN;
                    break;
                default:
                    return SOC_E_CONFIG;
            }
        } else {
            /* Don't ever let FEC be invalid */
            /* 400G requires RS 544, so default to that */
            if (config->speed == 400000) {
                config->fec = (portmod_fec_t)socPortPhyFecRs544_2xN;
            /* Default to no FEC if speed is not 400G */
            } else if ((config->speed == 50000) && (config->num_lane == 1)) {
                config->fec = (portmod_fec_t)socPortPhyFecRs544;
            } else {
                config->fec = (portmod_fec_t)socPortPhyFecNone;
            }
        }
    }

    if (link_training >= 0) {
        config->link_training = link_training;
    } else {
        if (!SOC_PBMP_PORT_VALID(port) || IS_CPU_PORT(unit, port) ||
                IS_MANAGEMENT_PORT(unit, port) || IS_LB_PORT(unit, port)) {
                return SOC_E_PARAM;
        }
        config->link_training =
                soc_property_port_get(unit, port, spn_PORT_INIT_CL72, 0);
    }

    if (lane_config >= 0) {
        config->lane_config = lane_config;
    } else if (lane_config == -2) {
        /* User passed in "auto" for lane config, so pass it in to portmod */
        config->lane_config = -1;
    } else {
        /* Use config file values */
        if (!SOC_PBMP_PORT_VALID(port) || IS_CPU_PORT(unit, port) ||
                IS_MANAGEMENT_PORT(unit, port) || IS_LB_PORT(unit, port)) {
                return SOC_E_PARAM;
        } else {
            SOC_IF_ERROR_RETURN(
                    _soc_portctrl_phy_lane_config_fill(unit, port, config));
        }
    }

    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_pm_update_vco_soc_info
 * Purpose:
 *      The PM8x50 has 3 VCOs and a FEC type to select from that determine
 *      the speed of the ports in use for each individual port macro. These
 *      VCOs are determined by the port's speed, the number of lanes and FEC
 *      used.
 *
 *      Example (Different use of FEC may impact the VCO. 50G IEEE mode uses
 *      PAM-4, but 50G BRCM mode 25G-NRZ):
 *      400G - 8 x 50G => PAM-4
 *      200G - 4 x 50G => PAM-4
 *      100G - 2 x 50G => PAM-4
 *      100G - 4 x 25G => 25G-NRZ
 *      50G  - 1 x 50G => PAM-4
 *      50G  - 2 x 25G => 25G-NRZ
 *      40G  - 4 x 10G => 10G-NRZ
 *      25G  - 1 x 25G => 25G-NRZ
 *      10G  - 1 x 10G => 10G-NRZ
 *
 *      The FEC type can be derived from the config file
 *
 *      This function accepts the parameters below because the parameters
 *      may be coming from SOC_INFO during init or from a
 *      soc_port_schedule_state_t datatype during flexport.
 *
 *      This function modifies the soc_info fields tvco and ovco for the
 *      specified pm_id
 *
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      phy_ports        - (IN)  array of physical ports corresponding to ONE
 *                               PM8x50
 *      num_ports   - (IN)  Number of ports in the array
 *      flags       - (IN)  Flags passed in to Portmod
 * Returns: SOC_E_X
 *
 * NOTE: PORTCTRL does not need to be initialized to use this function.
 *       However, SOC_INFO must have parsed the config file in port_config_init.
 *
 *       This is only called at init!
 */
int
soc_esw_portctrl_pm_update_vco_soc_info(int unit,
                                const int *phy_ports,
                                int num_ports,
                                int pm_id,
                                portmod_dispatch_type_t pm_type,
                                uint32 flags)
{
    int port;
    int num_speeds = 0;
    int rv;
    int i, j;
    int *starting_lane_list;
    int *speed_for_pcs_bypass_port = NULL;
    portmod_pm_vco_setting_t vco_setting;
    portmod_speed_config_t speed_config_temp;
    portmod_speed_config_t *speed_config_list = NULL;

    soc_info_t * si = &SOC_INFO(unit);
    rv = SOC_E_NONE;

    /* This was designed with the PM8x50 in mind. Function needs updating if used for
     * a different PM */
#ifdef PORTMOD_PM8X50_SUPPORT
    if (pm_type != portmodDispatchTypePm8x50) {
        rv = SOC_E_UNAVAIL;
        LOG_WARN(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "%s: found a different PM than PM8x50 %d\n"),
                             FUNCTION_NAME(), rv));
        return rv;
    }
#endif

    /* Find the port speeds in this PM */
    /* allocate space for the speeds we're sending to portmod */
    speed_config_list = sal_alloc(sizeof(portmod_speed_config_t) * num_ports,
                                    "speed_config_list");
    /*
     * Allocate space for flag that indicates if the
     * speed is for PCS bypass port.
     */
    speed_for_pcs_bypass_port = sal_alloc(sizeof(int) * num_ports,
                                          "speed_for_pcs_bypass_port");
    starting_lane_list = sal_alloc(sizeof(int) * num_ports,
                                    "starting_lane_list");

    if (speed_config_list == NULL) {
        rv = SOC_E_MEMORY;
        goto vco_cleanup;
    }
    if (speed_for_pcs_bypass_port == NULL) {
        rv = SOC_E_MEMORY;
        goto vco_cleanup;
    }
    if (starting_lane_list == NULL) {
        rv = SOC_E_MEMORY;
        goto vco_cleanup;
    }

    sal_memset(speed_config_list, 0,
            sizeof(portmod_speed_config_t) * num_ports);
    sal_memset(speed_for_pcs_bypass_port, 0, sizeof(int) * num_ports);
    sal_memset(starting_lane_list, 0, sizeof(int) * num_ports);

    for (i = 0; i < num_ports; i++) {
        port = si->port_p2l_mapping[phy_ports[i]];

        /* if port is unmapped */
        if (port == -1) {
            continue;
        }

        sal_memset(&speed_config_temp, 0, sizeof(portmod_speed_config_t));

        /* If we're doing warmboot, all the values have been loaded into
         * SOC_INFO at this point, so fill the speed_config structure with
         * those values rather than reading from the config file */
        if (SOC_WARM_BOOT(unit)) {
            rv = soc_esw_portctrl_speed_config_fill(unit, port,
                                            si->port_speed_max[port],
                                            si->port_num_lanes[port],
                                            si->port_fec_type[port],
                                            si->port_link_training[port],
                                            si->port_phy_lane_config[port],
                                            &speed_config_temp);
        } else {
            rv = soc_esw_portctrl_speed_config_fill(unit, port,
                                            -1,
                                            -1,
                                            -1,
                                            -1,
                                            -1,
                                            &speed_config_temp);
        }
        if (SOC_FAILURE(rv)) {
            goto vco_cleanup;
        }

        /* look through our list of speeds for a free spot. Add the port's speed
         * and starting lane to the list */
        for (j = 0; j < num_ports; j++) {

            /* Empty entry if speed is 0 */
            if (speed_config_list[j].speed == 0) {
                sal_memcpy(&speed_config_list[j], &speed_config_temp,
                                sizeof(portmod_speed_config_t));
                starting_lane_list[j] = i;
                num_speeds++;

                /* Speed added to empty slot. Continue to next port */
                break;
            }
        }

        /* The above for loop will always break out with the port's current
         * settings. It will either break out when it has found a duplicate,
         * or if it has added a new entry to speed_config_list. So set
         * that info into SOC_INFO
         */
        si->port_fec_type[port] = speed_config_list[j].fec;
        si->port_link_training[port] = speed_config_list[j].link_training;
        si->port_phy_lane_config[port] = speed_config_list[j].lane_config;
    }

    /* Set the structure being passed to portmod, which will return
     * with the tvco and ovco */
    /* coverity[leaked_storage] */
    rv = portmod_pm_vco_setting_t_init(unit, &vco_setting);
    if (SOC_FAILURE(rv)) {
        goto vco_cleanup;
    }

    vco_setting.speed_config_list = speed_config_list;
    vco_setting.speed_for_pcs_bypass_port = speed_for_pcs_bypass_port;
    vco_setting.port_starting_lane_list = starting_lane_list;
    vco_setting.num_speeds = num_speeds;

    rv = portmod_pm_vcos_get(unit, pm_type, flags, &vco_setting);

    /* Update the VCOs for this port macro */
    si->pm_vco_info[pm_id].tvco = vco_setting.tvco;
    si->pm_vco_info[pm_id].ovco = vco_setting.ovco;

vco_cleanup:
    if (speed_config_list != NULL) {
        sal_free(speed_config_list);
    }
    if (speed_for_pcs_bypass_port != NULL) {
        sal_free(speed_for_pcs_bypass_port);
    }
    if (starting_lane_list != NULL) {
        sal_free(starting_lane_list);
    }

    return rv;
}

/*
 * Function:
 *      _soc_esw_portctrl_pm_flex_fill_speed_config_list
 * Purpose:
 *      This function fills speed_config_list with speeds in the port
 *      macro. Each speed corresponds to a port.
 * Parameters:
 *      unit               - (IN)  Unit number.
 *      lanes_per_pm       - (IN)  Number of lanes in the PM
 *      phy_ports_in       - (IN)  Array of phy ports in PM
 *      port_iter          - (IN)  Index for phy_ports_in
 *      starting_lane_list - (IN/OUT) The list containing starting lane for port
 *      speed_config_list  - (IN/OUT) The speed config list to fill
 *      speeds             - (OUT) count of the speeds. This MUST
 *                              be cleared in the calling function
 *
 * Returns: SOC_E_X
 */
STATIC int
_soc_esw_portctrl_pm_flex_fill_speed_config_list(int unit,
                        int lanes_per_pm,
                        soc_port_resource_t **phy_ports_in,
                        int port_iter,
                        int *starting_lane_list,
                        portmod_speed_config_t *speed_config_list,
                        int *speeds)
{
    int i;
    int count = 0;
    int rv = SOC_E_NONE;

    for (i = 0; i < lanes_per_pm; i++) {
        /* Empty entry if speed is 0 */
        if (speed_config_list[i].speed == 0) {
            /* Since lane config can equal -1 to mean auto, we don't
             * want the following function to misinterpret that as an
             * instruction to read the config file value for lane config.
             * So, if lane_config == -1, set it to -2 to tell this function
             * that the value needs to be auto
             */
            if (phy_ports_in[port_iter]->phy_lane_config == -1) {
                phy_ports_in[port_iter]->phy_lane_config = -2;
            }
            rv = soc_esw_portctrl_speed_config_fill(unit,
                                            phy_ports_in[port_iter]->logical_port,
                                            phy_ports_in[port_iter]->speed,
                                            phy_ports_in[port_iter]->num_lanes,
                                            phy_ports_in[port_iter]->fec_type,
                                            phy_ports_in[port_iter]->link_training,
                                            phy_ports_in[port_iter]->phy_lane_config,
                                            &speed_config_list[i]);
            /* Set the starting lane equal to the port iterator that was passed
             * in mod lanes per pm. This is they physical lane for the pm */
            starting_lane_list[i] = (port_iter - 1) % lanes_per_pm;

            if (SOC_FAILURE(rv)) {
                return rv;
            }
            count++;
            break;
        }
    }
    /* Add count to the total */
    *speeds += count;
    return rv;
}


/*
 * Function:
 *      _soc_esw_portctrl_pm_flex_vco_validation
 * Purpose:
 *      This function validates that a SINGLE port macro's flex configuration
 *      is valid for the flexport scenario requested.
 *
 *      It gathers the port and speed info requested and compares it with the
 *      existing configuration (if there are ports that are persisting through
 *      the flex), and validates that info.
 *
 *      If everything is ok, it stores the port macro VCO information so that
 *      it doesn't have to be recalculated again later.  This VCO information
 *      is then written to SOC_INFO at reconfiguration time.
 *
 * Parameters:
 *      unit                 - (IN) Unit number.
 *      port_schedule_state  - (IN) Contains information for current PM state
 *      phy_port             - (IN) The physical port being flexed, which is
 *                                  used to determine the PM affected
 *      lanes_per_pm         - (IN) The number of lanes in the port macro
 *      phy_ports_in         - (IN) An array of pointers to the soc_port_resource_t
 *                                  of the physical ports being flexed.
 *                                  Non-flexing port indexes will be NULL
 *      phy_deleted_ports    - (IN) an array of ports that will be deleted
 *                                  in the flexport operation. Indexed by phy port
 *
 * Note:
 *      phy_ports_in is essentially a hash where the key is physical ports. If
 *      a port is flexing, the pointer will point to the flex operation. If the
 *      entry is NULL, it means that port is not flexing to a new port.
 *
 * Returns: SOC_E_X
 */

STATIC int
_soc_esw_portctrl_pm_flex_vco_validation(int unit,
                                soc_port_schedule_state_t *port_schedule_state,
                                int phy_port,
                                int lanes_per_pm,
                                soc_port_resource_t **phy_ports_in,
                                const uint8 *phy_deleted_ports)
{
#if defined(PORTMOD_SUPPORT) && defined(PORTMOD_PM8X50_SUPPORT)
    int port_iter;
    int pm_id, portmod_pm_id;
    int count;
    int base_phy_port;
    int rv;
    int logical_port;
    int pll_switch_flag;
    int *starting_lane_list = NULL;
    int *speed_for_pcs_bypass_port = NULL;
    soc_pmvco_info_t pm_vco_info;
    portmod_pm_vco_setting_t pm_vco_setting;
    portmod_pbmp_t flexing_ports;
    portmod_speed_config_t *speed_config_list = NULL;
    soc_port_map_type_t *in_port_map = &(port_schedule_state->in_port_map);

    /*
     * Allocate space for flag that indicates if the
     * speed is for PCS bypass port.
     */
    speed_for_pcs_bypass_port = sal_alloc(sizeof(int) * lanes_per_pm,
                                          "speed_for_pcs_bypass_port");
    if (speed_for_pcs_bypass_port == NULL) {
        return SOC_E_MEMORY;
    }
    speed_config_list = sal_alloc(lanes_per_pm * sizeof(portmod_speed_config_t),
                        "speed_config_list array");
    if (speed_config_list == NULL) {
        rv = SOC_E_MEMORY;
        goto cleanup;
    }
    starting_lane_list = sal_alloc(sizeof(int) * lanes_per_pm,
                                    "starting_lane_list");
    if (starting_lane_list == NULL) {
        rv = SOC_E_MEMORY;
        goto cleanup;
    }

    /* Iterate through the affected ports in the PM.  Verify
     * that the maximum number of VCOs isn't exceeded
     */
    pm_id = (phy_port - 1) / lanes_per_pm;
    base_phy_port = (pm_id * lanes_per_pm) + 1;

    sal_memset(speed_for_pcs_bypass_port, 0, sizeof(int) * lanes_per_pm);
    sal_memset(speed_config_list, 0, lanes_per_pm * sizeof(portmod_speed_config_t));
    sal_memset(starting_lane_list, 0, sizeof(int) * lanes_per_pm);
    sal_memset(&pm_vco_setting, 0, sizeof(pm_vco_setting));
    count = 0;
    PORTMOD_PBMP_CLEAR(flexing_ports);
    pll_switch_flag = PORTMOD_PM_SPEED_VALIDATE_F_TWO_PLL_SWITCH_ALLOWED;

    for (port_iter = base_phy_port; port_iter < (base_phy_port + lanes_per_pm);
                                                                port_iter++) {

        if (phy_ports_in[port_iter] == NULL) {
            /* Not creating a new port */

            /* Skip if the port is being deleted */
            if (phy_deleted_ports[port_iter] != 0) {
                continue;
            }

            logical_port = in_port_map->port_p2l_mapping[port_iter];

            /* Port might not be mapped.  Skip if it isn't mapped */
            if (SOC_PBMP_PORT_VALID(logical_port)) {

                /* Since not all ports on this PM are changing, only allow
                 * one PLL switch
                 */
                pll_switch_flag = PORTMOD_PM_SPEED_VALIDATE_F_ONE_PLL_SWITCH_ALLOWED;
            }
        } else {
            /* Creating a new port, fill the config list */
            rv = _soc_esw_portctrl_pm_flex_fill_speed_config_list(
                    unit,
                    lanes_per_pm,
                    phy_ports_in,
                    port_iter,
                    starting_lane_list,
                    speed_config_list,
                    &count);

            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }

    pm_vco_setting.speed_for_pcs_bypass_port = speed_for_pcs_bypass_port;
    pm_vco_setting.speed_config_list = speed_config_list;
    pm_vco_setting.port_starting_lane_list = starting_lane_list;
    pm_vco_setting.num_speeds = count;

    /* Fill in the flexing ports port bitmap with all ports being deleted.
     * If a port is changing, it has to have been deleted
     */
    for (port_iter = 1; port_iter < SOC_MAX_NUM_PORTS; port_iter++) {
        /* Check if the physical port is being deleted */
        if (phy_deleted_ports[port_iter]) {
            /* Only add ports on the PM in question */
            if (((port_iter - 1) / lanes_per_pm) == pm_id) {
                logical_port = in_port_map->port_p2l_mapping[port_iter];
                PORTMOD_PBMP_PORT_ADD(flexing_ports, logical_port);
            }
        }
    }


    /* Portmod's indexing of the port macros differs from SDK's */
    portmod_phy_pm_id_get(unit, phy_port, &portmod_pm_id);

    rv = portmod_pm_speed_config_validate(unit, portmod_pm_id, &flexing_ports,
                                pll_switch_flag, &pm_vco_setting);

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "  VCO Validation "
                                "Failed for Port Macro: %d\n"
                                "Current VCOS: %d %d\n"
                                "pll_switch_flag: %d\n"),
                     pm_id,
                     SOC_INFO(unit).pm_vco_info[pm_id].tvco,
                     SOC_INFO(unit).pm_vco_info[pm_id].ovco,
                     pll_switch_flag
                     ));
        goto cleanup;
    }


    /* Save this information so that it doesn't have to be validated later */
    pm_vco_info.tvco = pm_vco_setting.tvco;
    pm_vco_info.ovco = pm_vco_setting.ovco;
    /* Set is_tvco_new and is_ovco_new to 1,
     * indicating the vco in pm_vco_info is updated. */
    pm_vco_info.is_tvco_new = 1;
    pm_vco_info.is_ovco_new = 1;

    rv = soc_esw_portctrl_pm_vco_store(unit, pm_id, &pm_vco_info);

cleanup:
    if (speed_for_pcs_bypass_port != NULL) {
        sal_free(speed_for_pcs_bypass_port);
        speed_for_pcs_bypass_port = NULL;
    }
    if (speed_config_list != NULL) {
        sal_free(speed_config_list);
        speed_config_list = NULL;
    }
    if (starting_lane_list != NULL) {
        sal_free(starting_lane_list);
        starting_lane_list = NULL;
    }

    return rv;
#else
    return SOC_E_UNAVAIL;
#endif /* defined(PORTMOD_SUPPORT && defined(PORTMOD_PM8X50_SUPPORT) */
}


/*
 * Function:
 *      soc_esw_portctrl_pm_vco_store
 * Purpose:
 *      Calls a function vector, if it exists, to store PM VCO info so that
 *      it won't have to be recalculated later
 *
 * Parameters:
 *      unit                 - (IN) Unit number.
 *      index                - (IN) Port Macro index
 *      pm_vco_info          - (IN) VCO info want to store
 *
 * Note:
 *      Does not directly affect SOC_INFO data
 *
 * Returns: SOC_E_X
 */
int
soc_esw_portctrl_pm_vco_store(int unit, int index, const void *pm_vco_info)
{
    int rv;

    if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
        (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_vco_store)) {
        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_vco_store(
                 unit, index, pm_vco_info);
    } else {
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}

#endif /* PORTMOD_SUPPORT */

/*
 * Function:
 *      soc_esw_portctrl_pm_vco_reconfigure
 * Purpose:
 *      Called to Reconfigure the VCOs in Port Macros affected during a flex
 *      operation.  This is called AFTER SOC_INFO has been updated with the new
 *      VCO information.
 * Parameters:
 *      unit                 - (IN) Unit number.
 *
 * Note:
 *
 * Returns: SOC_E_X
 */
int
soc_esw_portctrl_pm_vco_reconfigure(int unit)
{
    int rv;

    if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
        (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_vco_reconfigure)) {
        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_vco_reconfigure(
                 unit);
    } else {
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}

int
soc_esw_portctrl_sbus_bcast_setup(int unit)
{
    int rv;

    if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
        (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_sbus_bcast_setup)) {
        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_sbus_bcast_setup(
                 unit);
    } else {
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *      soc_esw_portctrl_pm_vco_fetch
 * Purpose:
 *      Called to fetch the VCO information saved from a flex operation
 *      calculation for use in setting SOC_INFO
 * Parameters:
 *      unit                 - (IN) Unit number.
 *
 * Note:
 *      Does not directly affect SOC_INFO data
 *
 * Returns: SOC_E_X
 */
int
soc_esw_portctrl_pm_vco_fetch(int unit, int index, void *pm_vco_info)
{
#if defined(PORTMOD_SUPPORT) && defined(PORTMOD_PM8X50_SUPPORT)

    int rv;

    if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
        (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_vco_fetch)) {
        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_vco_fetch(
                 unit, index, pm_vco_info);
    } else {
        rv = SOC_E_UNAVAIL;
    }

    return rv;
#else
    return SOC_E_UNAVAIL;
#endif /* defined(PORTMOD_SUPPORT && defined(PORTMOD_PM8X50_SUPPORT) */
}


/*
 * Function:
 *      soc_esw_portctrl_flex_vco_validation
 * Purpose:
 *      Called to validate the speed and vco configuration for all port macros
 *      in the device.  This function will examine the flexport operation
 *      requested by the user, and will compare it with existing settings
 *      in the device.  It will then make sure that those configurations are
 *      valid.
 *
 * Parameters:
 *      unit                 - (IN) Unit number.
 *      port_schedule_state  - (IN) The flex information
 * Note:
 *
 * Returns: SOC_E_X
 */
int
soc_esw_portctrl_flex_vco_validation(int unit,
                    soc_port_schedule_state_t *port_schedule_state)
{
#if defined(PORTMOD_SUPPORT) && defined(PORTMOD_PM8X50_SUPPORT)

    int i;
    int num_ports;
    int max_num_ports;
    int pm_id;
    int lanes_per_pm;
    portmod_dispatch_type_t pm_type;
    portmod_pm_capability_t pm_cap;
    /* Create a SOC_MAX_NUM_PORTS port array so that we can keep track of physical ports.
     * soc_pbmp_t only goes up to 256 bits, so it's not big enough */
    uint8 phy_deleted_ports[SOC_MAX_NUM_PORTS];
    int rv = SOC_E_NONE;
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *resource = port_schedule_state->resource;
    /* array of pointers indexed by physical ports seen in soc_port_resource_t
     * array the user sent
     * NULL = physical port was unseen
     * != NULL = pointer to the soc_port_resource_t
     */
    soc_port_resource_t **phy_ports_in = NULL;

    /* Physical Ports for front panel are index offset by 1
     * (start at physical port 1). Physical port 0 is CPU port.
     * So if Physical ports 1 - 256 are usable as front panel ports,
     * we want to be able to access index 256, so allocate 257 slots (0-256) */
    max_num_ports = (SOC_MAX_NUM_PORTS + 1);

    phy_ports_in = sal_alloc(max_num_ports * sizeof(soc_port_resource_t *),
                            "Array of soc_port_resource_t *");
    if (phy_ports_in == NULL) {
        return SOC_E_MEMORY;
    }

    sal_memset(phy_ports_in, 0, max_num_ports * sizeof(soc_port_resource_t *));

    /* Keep track of the deleted ports */
    sal_memset(phy_deleted_ports, 0, sizeof(phy_deleted_ports));

    num_ports = port_schedule_state->nport;

    /* Mark the incoming physical ports in the phy_ports_in array. Indexes
     * that are NULL means that those ports are NOT flexing.
     */
    for (i = 0; i < num_ports; i++) {
        /* skip ports being deleted */
        if (resource[i].physical_port == -1) {
            /* mark the port as being deleted by marking it in the array
             */
            phy_deleted_ports[si->port_l2p_mapping[resource[i].logical_port]]
                                = 1;
            continue;
        }
        phy_ports_in[resource[i].physical_port] = &resource[i];
    }

    /* Validate for the post flex configuration that each PM doesn't exceed its
     * maximum number of VCOs
     */
    for (i = 1; i < max_num_ports; i++) {
        /* Skip physical ports not seen */
        if (phy_ports_in[i] == NULL) {
            continue;
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        /* Tomahawk 3's Management Port Macro requires a spacing between
         * the lanes, so even though the two management ports' physical
         * ports are 257 and 258, the lane needs to be the next one,
         * so logical port 118 being mapped to physical port 258 needs
         * to pass 259 to portmod because portmod only recognizes lane
         * 0 (physical port 257) and lane 2 (physial port 259).
         */
        if (SOC_IS_TOMAHAWK3(unit) && (i == 258)) {
            i = 259;
        }
#endif

        rv = (portmod_phy_pm_type_get(unit, i, &pm_type));
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
        switch (pm_type) {
            case portmodDispatchTypePm8x50:
                rv = (portmod_pm_capability_get(unit, pm_type, &pm_cap));
                if (SOC_FAILURE(rv)) {
                    goto cleanup;
                }

                lanes_per_pm = pm_cap.pm_capability.pm8x50_cap.num_lanes;
                break;
            default:
                /* Skip Ports that are in PMs that are not PM8x50 */
                continue;
        }
        /* This will verify that the post flex speeds on the port macro
         * are acceptable
         */
        rv = _soc_esw_portctrl_pm_flex_vco_validation(unit,
                                port_schedule_state,
                                i,
                                lanes_per_pm,
                                phy_ports_in,
                                phy_deleted_ports);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }
        /* get the port macro id that this port belongs to */
        pm_id = ((i - 1) / lanes_per_pm);

        /* Go to the last port in the port macro. No sense in checking the
         * same port macro over and over again.
         * Example: We just finished PM0, which starts at physical port 1, so
         * go to PM1: ((0 + 1) * 8) = 8. The loop will increment to 9, which
         * is the start of the next PM
         */
        i = ((pm_id + 1) * lanes_per_pm);
    }


cleanup:
    if (phy_ports_in != NULL) {
        sal_free(phy_ports_in);
    }

    return rv;
#else
    return SOC_E_UNAVAIL;
#endif /* defined(PORTMOD_SUPPORT && defined(PORTMOD_PM8X50_SUPPORT) */
}


/*
 * Function:
 *      soc_esw_portctrl_pm_vco_store_clear
 * Purpose:
 *      Clears the calculated VCO information from a flexport operation
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port_schedule_state  - (IN) The flex information
 * Note:
 *      Does not directly affect SOC_INFO data
 *
 * Returns: SOC_E_X
 */
int
soc_esw_portctrl_pm_vco_store_clear(int unit)
{
#ifdef PORTMOD_SUPPORT
    int rv;

    if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
        (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_vco_store_clear)) {
        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_vco_store_clear(
                 unit);
            } else {
        rv = SOC_E_UNAVAIL;
    }

    return rv;
#else
    return SOC_E_NONE;
#endif
}

/*
 * Function:
 *      soc_esw_portctrl_pm_ports_delete
 * Purpose:
 *      Remove ports from the corresponding Port Macros.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes all data is Port Resource arrays has been validated.
 *      - Must be called before SOC_INFO is updated.
 */
int
soc_esw_portctrl_pm_ports_delete(int unit,
                               int nport,
                               soc_port_resource_t *resource)
{
#ifdef PORTMOD_SUPPORT
    int i;
    soc_port_resource_t *pr;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "SOC Port Macro Delete\n")));

    /* Check that PortCtrl has been initialized */
    SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "  Port Macro delete "
                                "logical_port=%d physical_port=%d\n"),
                     pr->logical_port, pr->physical_port));

        /* portmod detach */
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_delete(unit, pr->logical_port));
    }

    return SOC_E_NONE;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}


/*
 * Function:
 *      soc_esw_portctrl_pm_ports_add
 * Purpose:
 *      Add ports and configure the relevant Port Macros for the given
 *      the FlexPort configuration.
 *
 *      This function reconfigures all the blocks inside
 *      the PortMod library, this is xxPORT, MAC, and PHY.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes all data is Port Resource arrays has been validated.
 *      - Must be called after SOC_INFO has been updated.
 *      - Assumes ports has been already deleted.
 *      - Caller must provide ALL ports (deleted and added), because
 *        for it needs to 'add' NULL driver on cases where ports
 *        become inactive.
 */
int
soc_esw_portctrl_pm_ports_add(int unit,
                               int nport,
                               soc_port_resource_t *resource)
{
#ifdef PORTMOD_SUPPORT
    int i, lane=0, num_lanes, physical_port;
    soc_port_resource_t *pr;
    soc_info_t *si = &SOC_INFO(unit);
    portmod_pbmp_t phys;
    pbmp_t pbmp;
#ifdef FW_BCAST_DOWNLOAD
    int bcast_flag, crc_en;
#endif

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "SOC Port Macro Add\n")));

    /* Check that PortCtrl has been initialized */
    SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

    PORTMOD_PBMP_CLEAR(phys);
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /*
         * Check if 'delete' port needs to be added (NULL driver).
         */
        if ((pr->physical_port == -1) &&
            !SOC_PBMP_MEMBER(si->all.disabled_bitmap, pr->logical_port)) {
            /* Port is deleted and is not inactive, nothing to add */
            continue;
        }

        num_lanes = SOC_INFO(unit).port_num_lanes[pr->logical_port];
        if(num_lanes == 10) num_lanes=12;
        for (lane = 0; lane < num_lanes; lane++) {
            physical_port = SOC_INFO(unit).port_l2p_mapping[pr->logical_port];
#ifdef BCM_TRIDENT3_SUPPORT
            /*To support the 2nd MGMT port, convert its physical port to the dummy port number 131 on TD3.*/
            if((SOC_IS_TRIDENT3(unit)) && (physical_port == 128) && (SOC_PBMP_MEMBER(SOC_INFO(unit).management_pbm, pr->logical_port))){
                physical_port = 131;
            }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
            /* Tomahawk 3's Management Port Macro requires a spacing between
             * the lanes, so even though the two management ports' physical
             * ports are 257 and 258, the lane needs to be the next one,
             * so logical port 118 being mapped to physical port 258 needs
             * to pass 259 to portmod because portmod only recognizes lane
             * 0 (physical port 257) and lane 2 (physial port 259).
             */
            if (SOC_IS_TOMAHAWK3(unit) && pr->logical_port == 118) {
                physical_port = 259;
            }
#endif
            SOC_IF_ERROR_RETURN(
                portmod_xphy_lane_detach(unit, physical_port + lane, 1));
            SOC_PBMP_PORT_ADD(phys, physical_port + lane);
        }
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_setup_ext_phy_add(unit, pr->logical_port, &phys));
    }

    /* Configure port in PortMod */
    SOC_PBMP_CLEAR(pbmp);
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /*
         * In some cases, ports to be deleted become inactive.
         * In this case, the PortMod NULL driver needs to be installed
         * for the inactive port.
         *
         * The function soc_esw_portctrl_add() sets the right
         * information for the PortMod to install the correct
         * driver.
         */

        /*
         * Check if 'delete' port needs to be added (NULL driver).
         */
        if ((pr->physical_port == -1) &&
            !SOC_PBMP_MEMBER(si->all.disabled_bitmap, pr->logical_port)) {
            /* Port is deleted and is not inactive, nothing to add */
            continue;
        }

        /* Add port */
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "  Port Macro: Add "
                                "logical_port=%d physical_port=%d\n"),
                     pr->logical_port, pr->physical_port));

        SOC_IF_ERROR_RETURN(soc_esw_portctrl_add(unit, pr->logical_port, PORTMOD_PORT_ADD_F_INIT_CORE_PROBE, NULL));
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_add(unit, pr->logical_port, PORTMOD_PORT_ADD_F_INIT_PASS1, NULL));
        SOC_PBMP_PORT_ADD(pbmp, pr->logical_port);
    }

#ifdef FW_BCAST_DOWNLOAD
    bcast_flag = soc_property_get(unit, spn_LOAD_FIRMWARE, 0);
    if (bcast_flag & 0xff0000) {
        crc_en = (bcast_flag >> 8) & 0xFF;
        SOC_IF_ERROR_RETURN(portmod_common_serdes_fw_bcst(unit, pbmp, crc_en));
    }
#endif

    if (!(SOC_IS_TOMAHAWK2(unit) || SOC_IS_TOMAHAWK3(unit))) {
    SOC_IF_ERROR_RETURN(portmod_legacy_ext_phy_init(unit, pbmp));

    /* broadcast firmware download to all phymod based phys */
    SOC_IF_ERROR_RETURN(portmod_common_ext_phy_fw_bcst(unit, &pbmp));
    }


    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        /*
         * Check if 'delete' port needs to be added (NULL driver).
         */
        if ((pr->physical_port == -1) &&
            !SOC_PBMP_MEMBER(si->all.disabled_bitmap, pr->logical_port)) {
            /* Port is deleted and is not inactive, nothing to add */
            continue;
        }

        /* Add port */
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_add(unit, pr->logical_port, PORTMOD_PORT_ADD_F_INIT_PASS2, NULL));
        /* Leave port disabled */
        SOC_IF_ERROR_RETURN(portmod_port_enable_set(unit, pr->logical_port, PORTMOD_PORT_ENABLE_PHY, 0));
    }

    return SOC_E_NONE;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_init
 * Purpose:
 *      Initialize the Port Control component and
 *      corresponding library (PortMod library).
 *      1. call soc_portctrl_pm_portmod_init
 *      2. set portctrl_init status to SOC_PORTCTRL_INITIALIZED.
 *
 *      This is to be called by the SOC Port module initialization
 *      function soc_misc_init().
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_portctrl_init(int unit)
{
#ifdef PORTMOD_SUPPORT
    int rv;

    /* Call Device specific PortMod library initialization */
    rv = soc_portctrl_pm_portmod_init(unit);
    /* Successful */
    if (SOC_SUCCESS(rv)) {
        portctrl_init[unit] = SOC_PORTCTRL_INITIALIZED;

        if (SOC_WARM_BOOT(unit)) {
            rv = soc_portctrl_xport_type_verify(unit);
        }
    }
    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_deinit
 * Purpose:
 *      Uninitialize the Port Control component and
 *      corresponding library (PortMod library).
 *      1. call soc_portctrl_pm_portmod_deinit
 *      2. set portctrl_init status to SOC_PORTCTRL_NOT_INITIALIZED.
 *
 *      This function must be called by the BCM Port module
 *      deinit (or detach) function.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_portctrl_deinit(int unit)
{
#ifdef PORTMOD_SUPPORT
    int rv;

    if (SOC_PORTCTRL_NOT_INITIALIZED == portctrl_init[unit]) {
        return SOC_E_NONE;
    }

    /* Call Device specific PortMod library uninitialization */
    rv = soc_portctrl_pm_portmod_deinit(unit);
    portctrl_init[unit] = SOC_PORTCTRL_NOT_INITIALIZED;

    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}


/*
 * Function:
 *      soc_esw_portctrl_add
 * Purpose:
 *      Add given logical port to PM (Port Macro).
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Port number.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assume port is disabled.
 *      Assume port has been removed from PM.
 *      Assume caller has checked that PortCtrl was initialized.
 *      SOC_INFO SW data structure has been updated.
 */
int
soc_esw_portctrl_add(int unit, soc_port_t port, int init_flag, void *port_add_info)
{
#ifdef PORTMOD_SUPPORT
    portctrl_pport_t pport;
    portmod_port_add_info_t *add_info = NULL;
    portmod_port_add_info_t *local_add_info = NULL;
    int phy_port;
    int num_lanes;
    int speed;
    int lane, fwload_verify;
    int duplex;
    int ifg;
    soc_ipg_t *si;
    int rv = SOC_E_NONE;
    uint32 mtu_size;
#ifdef PORTMOD_PM8X50_SUPPORT
    int is_init = 0;
    int pm_id, portmod_pm_id, phy_lane_config;
    portmod_pm_capability_t pm_cap;
    portmod_dispatch_type_t pm_type = portmodDispatchTypeCount;
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
    int mode;
#endif

    /* Convert port to PortMod port */
    PORTCTRL_PORT_TO_PPORT(port, pport);

    if (port_add_info != NULL) {
        add_info = (portmod_port_add_info_t*)port_add_info;
    } else {
        local_add_info = sal_alloc(sizeof(portmod_port_add_info_t),
                         "portmod_port_add_info_t");
        if (local_add_info == NULL) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "Mem Alloc Fail: portmod_port_add_info_t")));
            rv = SOC_E_MEMORY;
            return rv;
        }

        add_info = local_add_info;
    }

    if (((port_add_info!=NULL) &&
        (init_flag == PORTMOD_PORT_ADD_F_INIT_CORE_PROBE || !init_flag))
        || (port_add_info==NULL)) {

        rv = portmod_port_add_info_t_init(unit, add_info);
        if (PORTMOD_FAILURE(rv)) {
            if (local_add_info != NULL) {
                sal_free(local_add_info);
            }
            return rv;
        }

        rv = soc_esw_portctrl_config_get(unit, port,
                                     &(add_info->interface_config),
                                     &(add_info->init_config),
                                     &(add_info->phy_op_mode));
        if (SOC_FAILURE(rv)) {
            if (local_add_info != NULL) {
                sal_free(local_add_info);
            }
        return rv;
    }

    add_info->phy_op_datapath= soc_property_port_get(unit, port,
                                                    spn_PHY_ULL_DATAPATH,
                                                    phymodDatapathNormal);

    /*
     * Add port to PM
     */
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    num_lanes = SOC_INFO(unit).port_num_lanes[port];
    if (num_lanes == 10) {
        num_lanes = 12;
    }

#ifdef BCM_TRIDENT3_SUPPORT
        
        if((SOC_IS_TRIDENT3(unit)) && (phy_port == 128)) {
            int pbm_count=0;
            SOC_PBMP_COUNT(SOC_INFO(unit).management_pbm, pbm_count);
            if (pbm_count == 2) {
                phy_port = 131;
            }
        }
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
        if (SOC_IS_FIREBOLT6(unit)) {
            add_info->init_config.port_mode_aux_info = 0;
            /* Figure out the port mode */
            SOC_IF_ERROR_RETURN(soc_td3_port_mode_get(unit, port, &mode));
            if (mode == portmodPortModeTri012 || mode == portmodPortModeTri023) {
                add_info->init_config.port_mode_aux_info = portmodModeInfoThreePorts;
            }
        }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
        /* Tomahawk 3's Management Port Macro requires a spacing between
         * the lanes, so even though the two management ports' physical
         * ports are 257 and 258, the lane needs to be the next one,
         * so logical port 118 being mapped to physical port 258 needs
         * to pass 259 to portmod because portmod only recognizes lane
         * 0 (physical port 257) and lane 2 (physial port 259).
         */
        if (SOC_IS_TOMAHAWK3(unit) && (phy_port == 258)) {
            phy_port = 259;
        }
#endif

    /*
     * Set lane information
     * Assume lanes are arranged consecutively with respect to
     * the physical port number.
     */
    for (lane = 0 ; lane < num_lanes; lane++) {
        PORTMOD_PBMP_PORT_ADD(add_info->phys, phy_port + lane);
    }
    }

    add_info->flags = 0;

    /* By default choose external fw load to speed up */
    /* byte 0 defines fw load method, byte1 defines to verify the load */
    fwload_verify = soc_property_get(unit, spn_LOAD_FIRMWARE, 2);

    if ((fwload_verify >> 8) & 0xF) {
        PORTMOD_PORT_ADD_F_BYPASS_FW_CRC_CHECK_CLR(add_info);
    } else {
        PORTMOD_PORT_ADD_F_BYPASS_FW_CRC_CHECK_SET(add_info);
    }

    if ((fwload_verify >> 12) & 0xF) {
        PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_SET(add_info);
    }

#ifdef FW_BCAST_DOWNLOAD
    if (((fwload_verify >> 16) & 0xFF)) {
        PORTMOD_PORT_ADD_F_FIRMWARE_DOWNLOAD_BCAST_SET(add_info);
    }
#endif

    if (init_flag == PORTMOD_PORT_ADD_F_INIT_CORE_PROBE) {
        PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_SET(add_info);
    }

    if (init_flag == PORTMOD_PORT_ADD_F_INIT_PASS1) {
        PORTMOD_PORT_ADD_F_INIT_PASS1_SET(add_info);
    }

    if (init_flag == PORTMOD_PORT_ADD_F_INIT_PASS2) {
        PORTMOD_PORT_ADD_F_INIT_PASS2_SET(add_info);
    }

    if (SAL_BOOT_SIMULATION) {
        PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(add_info);
    }

    if (soc_feature(unit, soc_feature_clmac_16byte_interface_mode)) {
        PORTMOD_PORT_ADD_F_SYS_16B_INTF_MODE_SET(add_info);
    }
#if (defined BCM_MONTEREY_SUPPORT) || (defined BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_MONTEREY(unit) || SOC_IS_FIRELIGHT(unit)){
        PORTMOD_PORT_ADD_F_TX_REPLACE_CRC_SET (add_info);
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        PORTMOD_PORT_ADD_F_TX_REPLACE_CRC_SET(add_info);
    }
#endif
#ifdef PORTMOD_PM8X50_SUPPORT
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];

#ifdef BCM_TOMAHAWK3_SUPPORT
        /* Tomahawk 3's Management Port Macro requires a spacing between
         * the lanes, so even though the two management ports' physical
         * ports are 257 and 258, the lane needs to be the next one,
         * so logical port 118 being mapped to physical port 258 needs
         * to pass 259 to portmod because portmod only recognizes lane
         * 0 (physical port 257) and lane 2 (physial port 259).
         */
        if (SOC_IS_TOMAHAWK3(unit) && (phy_port == 258)) {
            phy_port = 259;
        }
#endif

    rv = portmod_phy_pm_type_get(unit, phy_port, &pm_type);

    if (rv != SOC_E_UNAVAIL && rv < 0) {
        if (local_add_info != NULL) {
            sal_free(local_add_info);
        }
        return rv;
    }

    /* for PM8x50 core init and adding ports to the PM are separated,
     * so we don't call portmod_port_add for both init and port add.
     * Just initialize the core here (if needed) and return.
     */
    if (pm_type == portmodDispatchTypePm8x50) {
        /* Portmod's indexing of the port macros differs from SDK's */
        portmod_phy_pm_id_get(unit, phy_port, &portmod_pm_id);

        /* Check if the core is initialized */
        rv = portmod_pm_is_initialized(unit, portmod_pm_id, &is_init);
        if (rv != SOC_E_UNAVAIL && rv < 0) {
            if (local_add_info != NULL) {
                sal_free(local_add_info);
            }
            return rv;
        }

        /* Get the PM capability so we can figure out the # of lanes
         * per PM */
        rv = portmod_pm_capability_get(unit, pm_type, &pm_cap);
        if (rv != SOC_E_UNAVAIL && rv < 0) {
            if (local_add_info != NULL) {
                sal_free(local_add_info);
            }
            return rv;
        }

        /* PM 0 = phy ports 1-8
         * PM 1 = phy ports 9-16
         * PM 2 = phy ports 17-24
         * etc. */
        pm_id = (phy_port - 1) / pm_cap.pm_capability.pm8x50_cap.num_lanes;

        /* Set the TVCO and OVCO of these ports corresponding to
         * the PM they're a member of */
        add_info->tvco = SOC_INFO(unit).pm_vco_info[pm_id].tvco;
        add_info->ovco = SOC_INFO(unit).pm_vco_info[pm_id].ovco;

        /* Since lane config can equal -1 to mean auto, we don't
         * want the following function to misinterpret that as an
         * instruction to read the config file value for lane config.
         * So, if lane_config == -1, set it to -2 to tell this function
         * that the value needs to be auto
         */
        phy_lane_config = SOC_INFO(unit).port_phy_lane_config[port];
        if (phy_lane_config == -1) {
            phy_lane_config = -2;
        }

        if (SOC_INFO(unit).port_init_speed[port] != 0) {
            speed = SOC_INFO(unit).port_init_speed[port];
        } else {
            speed = SOC_INFO(unit).port_speed_max[port];
        }

        /* Set the speed_config information */
        rv = soc_esw_portctrl_speed_config_fill(unit,
                                    port,
                                    speed,
                                    SOC_INFO(unit).port_num_lanes[port],
                                    SOC_INFO(unit).port_fec_type[port],
                                    SOC_INFO(unit).port_link_training[port],
                                    phy_lane_config,
                                    &(add_info->speed_config));


        /* if the core is not initialized for this port, initialize it here */
        if (!PORTMOD_CORE_INIT_FLAG_INITIALZIED_GET(is_init)) {
            
            /* core init will be called for probe, pass 1, and pass 2
             * as instructed by the portmod team.
             */
            rv = portmod_core_add(unit, portmod_pm_id, 0, add_info);
            if (rv != SOC_E_UNAVAIL && rv < 0) {
                if (local_add_info != NULL) {
                    sal_free(local_add_info);
                }
                return rv;
            }

        }

        /* Only allow the call to the below portmod_port_add on pass 2
         * because that means the core is init
         */
        if (!((init_flag == PORTMOD_PORT_ADD_F_INIT_PASS2) || (init_flag == 0))) {
            if (local_add_info != NULL) {
                sal_free(local_add_info);
            }
            return rv;
        }

        /* Clear the pass 2 flags because we want to call the port add
         * only once.
         */
        add_info->flags &= ~PORTMOD_PORT_ADD_F_INIT_PASS2;
    }
#endif

    if (soc_feature(unit, soc_feature_clmac_16byte_interface_mode)) {
        PORTMOD_PORT_ADD_F_SYS_16B_INTF_MODE_SET(add_info);
    }

#if (defined BCM_HELIX5_SUPPORT) || (defined BCM_MAVERICK2_SUPPORT) || (defined BCM_FIREBOLT6_SUPPORT) || (defined BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_HELIX5(unit) || SOC_IS_MAVERICK2(unit) ||
        SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit) ||
        SOC_IS_FIRELIGHT(unit)) {
        PORTMOD_PORT_ADD_F_IS_ENHANCED_PM4X25_GEN2_SET(add_info);
    }
#endif

#if (defined BCM_MAVERICK2_SUPPORT)
    if (SOC_IS_MAVERICK2(unit)) {
        PORTMOD_PORT_ADD_F_DEFAULT_OSR2P5_SET(add_info);
    }
#endif
    rv = portmod_port_add(unit, pport, add_info);
    if (PORTMOD_FAILURE(rv)) {
        if (local_add_info != NULL) {
            sal_free(local_add_info);
        }
        return rv;
    }

    if ((init_flag == PORTMOD_PORT_ADD_F_INIT_PASS2)  || (!init_flag)){
        rv = soc_esw_portctrl_duplex_get(unit, port, &duplex);
        if (PORTMOD_FAILURE(rv)) {
            if (local_add_info != NULL) {
                sal_free(local_add_info);
            }
            return rv;
        }

        si = &SOC_PERSIST(unit)->ipg[port];
        if (IS_HG_PORT(unit, port)) {
            si->fd_hg = ifg = SOC_AVERAGE_IPG_HG;
        } else {
            si->fd_xe = ifg = SOC_AVERAGE_IPG_IEEE;
        }

#ifdef PORTMOD_PM8X50_SUPPORT
        if (pm_type == portmodDispatchTypePm8x50) {
            speed = add_info->speed_config.speed;
        } else
#endif
        {
        speed = add_info->interface_config.speed;
        }

        if (IS_ROE_PORT(unit, port)) {
            return SOC_E_NONE;
        }
        rv = soc_esw_portctrl_ifg_set(unit, port, speed, duplex, ifg);

        if (PORTMOD_FAILURE(rv)) {
            if (local_add_info != NULL) {
                sal_free(local_add_info);
            }
            return rv;
        }

        mtu_size = SOC_INFO(unit).max_mtu;
        if (IS_XE_PORT(unit, port) || IS_GE_PORT(unit, port)
            || IS_CE_PORT(unit, port)
            || IS_CDE_PORT(unit, port)
            || (IS_C_PORT(unit, port) && IS_E_PORT(unit,port))) {
            /* For VLAN tagged packets */
            mtu_size -= 4;
        }

        rv = soc_esw_portctrl_frame_max_set(unit, pport, mtu_size);
        if (PORTMOD_FAILURE(rv)) {
            if (local_add_info != NULL) {
                sal_free(local_add_info);
            }
            return rv;
        }

#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3(unit)) {
            soc_info_t *si = &SOC_INFO(unit);
            if (((add_info->interface_config.speed == 27000) ||
                 (add_info->interface_config.speed == 53000))&&
                 (!(si->oversub_mode))) {
#if 1
                LOG_ERROR(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                                      "27G/53G speeed in Line rate config is not supported.\n")));
                if (local_add_info != NULL) {
                    sal_free(local_add_info);
                }
                return SOC_E_CONFIG;
#endif
            }
        }

        if (SOC_IS_FIREBOLT6(unit)) {
            soc_info_t *si = &SOC_INFO(unit);

            if (si->frequency == 1012) {

                /* 1012 indicates BCM56472 P/N 1 in 1012Mhz core frequency. Set TX_THRESHOLD to 2 to avoid MAC underrun.
                 */

                uint64 reg_val;
                rv = READ_CLMAC_TX_CTRLr(unit, port, &reg_val);
                if (SOC_FAILURE(rv)) {
                    if (local_add_info != NULL) {
                        sal_free(local_add_info);
                    }
                    return rv;
                }
                soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, TX_THRESHOLDf, 2);
                rv = WRITE_CLMAC_TX_CTRLr(unit, port, reg_val);
                if (SOC_FAILURE(rv)) {
                    if (local_add_info != NULL) {
                        sal_free(local_add_info);
                    }
                    return rv;
                }
            }
        }
#endif /* BCM_TRIDENT3_SUPPORT */

#if defined (BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        uint64 reg_val;
        /* MACsec output will be bursty due to a variety of factors.
         * Used the X/CLMAC_TX_CTRL.TX_THRESHOLD to prevent underflows.
         */
        soc_info_t *si = &SOC_INFO(unit);

        if (IS_CL_PORT(unit, port)) {
            rv = READ_CLMAC_TX_CTRLr(unit, port, &reg_val);
            if (SOC_FAILURE(rv)) {
                if (local_add_info != NULL) {
                    sal_free(local_add_info);
                }
                return rv;
            }

            soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, TX_THRESHOLDf, 4);
            rv = WRITE_CLMAC_TX_CTRLr(unit, port, reg_val);
            if (SOC_FAILURE(rv)) {
                if (local_add_info != NULL) {
                    sal_free(local_add_info);
                }
                return rv;
            }
        } else if (IS_XL_PORT(unit, port)) {
            uint32 reg_val32, ge_xl_sel;

            rv = READ_TOP_MACSEC_CTRLr(unit, &reg_val32);
            if (SOC_FAILURE(rv)) {
                if (local_add_info != NULL) {
                    sal_free(local_add_info);
                }
                return rv;
            }

            rv = READ_XLMAC_TX_CTRLr(unit, port, &reg_val);
            if (SOC_FAILURE(rv)) {
                if (local_add_info != NULL) {
                    sal_free(local_add_info);
                }
                return rv;
            }

            ge_xl_sel = soc_reg_field_get(unit, TOP_MACSEC_CTRLr, reg_val32,
                                          MACSEC_GE_XL_SELf);
            if ((ge_xl_sel & (1 << SOC_PORT_BLOCK_NUMBER(unit, si->port_l2p_mapping[port]))) != 0) {
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &reg_val,
                                    TX_THRESHOLDf, 6);
            } else {
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &reg_val,
                                    TX_THRESHOLDf, 1);
            }

            rv = WRITE_XLMAC_TX_CTRLr(unit, port, reg_val);
            if (SOC_FAILURE(rv)) {
                if (local_add_info != NULL) {
                    sal_free(local_add_info);
                }
                return rv;
            }
        }
    }
#endif
        rv = soc_esw_portctrl_port_fc_config_init(unit, pport);
        if (PORTMOD_FAILURE(rv)) {
            if (local_add_info != NULL) {
                sal_free(local_add_info);
            }
            return rv;
        }
    }

    if (local_add_info != NULL) {
        sal_free(local_add_info);
    }

    return SOC_E_NONE;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}


/*
 * Function:
 *      soc_esw_portctrl_delete
 * Purpose:
 *      Remove given logical port from PM (Port Macro).
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Port number.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assume port is disabled.
 *      Assume caller has checked that PortCtrl was initialized.
 *      SOC_INFO SW data structure has been updated.
 */
int
soc_esw_portctrl_delete(int unit, soc_port_t port)
{
#ifdef PORTMOD_SUPPORT
    portctrl_pport_t pport;
    int valid;

    /* Convert port to PortMod port */
    PORTCTRL_PORT_TO_PPORT(port, pport);

    /* Remove port */
    PORTMOD_IF_ERROR_RETURN(portmod_port_is_valid(unit, pport, &valid));
    if (valid) {
        PORTMOD_IF_ERROR_RETURN(portmod_port_remove(unit, pport));
    }

    return SOC_E_NONE;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}


/*
 * Function:
 *      soc_esw_portctrl_pgw_reconfigure
 * Purpose:
 *      This function is called during reconfiguration of PGW during
 *      flexing operation.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Logical Port number.
 *      phy_port  - (IN) Physical Port number.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes port is disabled.
 *      Assumes caller has checked that PortCtrl was initialized.
 *      SOC_INFO SW data structure has been updated.
 */
int
soc_esw_portctrl_pgw_reconfigure(int unit, soc_port_t port,
                                 int phy_port,
                                 soc_esw_portctrl_pgw_t *pgw_data)
{
#ifdef PORTMOD_SUPPORT
    portmod_port_mode_info_t info;

    portmod_port_mode_info_t_init(unit, &info);

    info.cur_mode = pgw_data->mode;
    info.lanes = pgw_data->lanes;
    info.port_index = pgw_data->port_index;

    pgw_data->flags = PORTMOD_PORT_PGW_CONFIGURE;

    /* Note this portmod API requires logical port */
    PORTMOD_IF_ERROR_RETURN(portmod_port_pgw_reconfig(unit, port, &info,
                                             phy_port, pgw_data->flags));

    return SOC_E_NONE;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_duplex_get
 * Purpose:
 *      Get the port duplex settings
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      duplex - (OUT) Duplex setting, one of SOC_PORT_DUPLEX_xxx
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_duplex_get(int unit, soc_port_t port, int *duplex)
{
#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    portctrl_pport_t pport;
    int port_duplex;
    int is_legacy_phy_present;

    SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

    PORTCTRL_PORT_TO_PPORT(port, pport);

    SOC_IF_ERROR_RETURN(portmod_port_is_legacy_ext_phy_present(unit, pport, &is_legacy_phy_present));

    if (is_legacy_phy_present) {
        rv = portmod_port_legacy_duplex_get(unit, pport, &port_duplex);
        if (rv == SOC_E_UNAVAIL) {
            rv = portmod_port_duplex_get(unit, pport, &port_duplex);
        }
    } else {
        rv = portmod_port_duplex_get(unit, pport, &port_duplex);
    }

    if (PORTMOD_SUCCESS(rv)) {
        *duplex = port_duplex ? SOC_PORT_DUPLEX_FULL : SOC_PORT_DUPLEX_HALF;
    } else {
        *duplex = SOC_PORT_DUPLEX_FULL;
    }

    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_duplex_set
 * Purpose:
 *      Set the port duplex
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 *      duplex - Duplex setting, one of SOC_PORT_DUPLEX_xxx
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_duplex_set(int unit, soc_port_t port, int duplex)
{
#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    portctrl_pport_t pport;
    int is_legacy_phy_present;

    SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

    PORTCTRL_PORT_TO_PPORT(port, pport);

    SOC_IF_ERROR_RETURN(portmod_port_duplex_set(unit, pport, duplex));

    SOC_IF_ERROR_RETURN(portmod_port_is_legacy_ext_phy_present(unit, pport, &is_legacy_phy_present));

    if (is_legacy_phy_present) {
        rv = portmod_port_legacy_duplex_set(unit, pport, duplex);
        /*Don`t return error, if the external PHY doesn`t support duplex*/
        if (rv == SOC_E_UNAVAIL) {
            rv = SOC_E_NONE;
        }
    }

    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}


/*
 * Function:
 *      soc_esw_portctrl_port_fc_config_init
 * Purpose:
 *      Configure default flow control configuration
 *      in the port macro. Depending on the each chip's
 *      support level, we will either have to program
 *      PARALLEL_FC_EN to 1 or 0. Depending on this config
 *      either the serial interface (mmu flow control) or
 *      the parallel interface (for obm flow control) is used.
 *      For devices where the flow control is to be used from
 *      both the interfaces, we need to enable merge mode (and
 *      not configure parallel_fc_en).
 *
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_port_fc_config_init(int unit, soc_port_t port)
{
#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    portctrl_pport_t pport;

    /*
     * Note that MMU uses serial interface (legacy) and OBM uses parallel
     * interface for pause control. If pause from only OBM is required, then
     * we should enable parallel_fc mode. Disabling parallel fc mode enables
     * the legacy serial interface. When pause from both the interfaces are
     * desired, then merge_mode_en should be enabled.
     *
     * For Apache, FC signals from MMU and OBM must be honored for
     * proper flow control handling.
 */
    if (soc_feature(unit, soc_feature_pm_fc_merge_mode) && !IS_QSGMII_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

        PORTCTRL_PORT_TO_PPORT(port, pport);

        rv = portmod_port_flow_control_set(unit, pport,
                                           1 /* merge_mode  */,
                                           0 /* parallel_fc */);
    }

    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}


int
soc_esw_portctrl_frame_max_set(int unit, soc_port_t port, int size)
{
#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    portctrl_pport_t pport;
    int max_size = SOC_INFO(unit).max_mtu;
    soc_reg_t reg;
    egr_mtu_entry_t mtu;
    uint32 rval;
    uint32 mtu_size;

    SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

    PORTCTRL_PORT_TO_PPORT(port, pport);

    if (size < 0) {
        return SOC_E_PARAM;
    }

    if (IS_XE_PORT(unit, port) || IS_GE_PORT(unit, port)
        || IS_CE_PORT(unit, port)
        || IS_CDE_PORT(unit, port)
        || (IS_C_PORT(unit, port) && IS_E_PORT(unit,port))) {
        /* For VLAN tagged packets */
#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit) && IS_QSGMII_PORT(unit, port)) {
            /*
             * For FL GPORT, the programmed FRM_LENGTH corresponds to
             * packets without VLANs. With 1 and 2 VLANs, the max packet size
             * increases by 4 and 8 respectively. So don't need to add
             * VLAN tag size in FRM_LENGTH for FL GPORT.
             */
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            size += 4;
        }
    }

    if (size > max_size) {
        return SOC_E_PARAM;
    }

    rv = portmod_port_max_packet_size_set(unit, pport, size);

    if (PORTMOD_SUCCESS(rv)) {
        soc_mem_t mem = EGR_MTUm;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            if (soc_feature(unit, soc_feature_egr_all_profile)) {
                int field_cnt = 0;
                soc_field_t fields[2];
                uint32 values[2];

                fields[field_cnt] = MTU_SIZEf;
                values[field_cnt++] = size;
                fields[field_cnt] = MTU_ENABLEf;
                values[field_cnt++] = 1;
                rv = bcm_esw_port_egr_lport_fields_set(unit, port, mem,
                                                       field_cnt, fields, values);
            } else {
                rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &mtu);
                if (SOC_SUCCESS(rv)) {
                    mtu_size = size;
                    soc_mem_field32_set(unit, mem, &mtu, MTU_SIZEf, mtu_size);
                    if (soc_mem_field_valid(unit, EGR_MTUm, MTU_ENABLEf)) {
                        soc_mem_field32_set(unit, EGR_MTUm, &mtu, MTU_ENABLEf, 1);
                    }
                    rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, port, &mtu);
                }
            }
        } else {
            reg = SOC_REG_IS_VALID(unit, EGR_MTUr) ? EGR_MTUr : EGR_MTU_SIZEr;

            rv = soc_reg32_get(unit, reg, port, 0, &rval);
            if (SOC_SUCCESS(rv)) {
                soc_reg_field_set(unit, reg, &rval, MTU_SIZEf, size);
                if (soc_reg_field_valid(unit, reg, MTU_ENABLEf)) {
                    soc_reg_field_set(unit, reg, &rval, MTU_ENABLEf, 1);
                }
                rv = soc_reg32_set(unit, reg, port, 0, rval);
            }
        }
    }

    return rv;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}


int
soc_esw_portctrl_frame_max_get(int unit, soc_port_t port, int *size)
{
#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    portctrl_pport_t pport;

    SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

    PORTCTRL_PORT_TO_PPORT(port, pport);

    rv = portmod_port_max_packet_size_get(unit, pport, size);

    if (IS_XE_PORT(unit, port) || IS_GE_PORT(unit, port)
        || IS_CE_PORT(unit, port)
        || IS_CDE_PORT(unit, port)
        || (IS_C_PORT(unit, port) && IS_E_PORT(unit,port))) {
        /* For VLAN tagged packets */
#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit) && IS_QSGMII_PORT(unit, port)) {
            /*
             * For FL GPORT, the programmed FRM_LENGTH corresponds to
             * packets without VLANs. With 1 and 2 VLANs, the max packet size
             * increases by 4 and 8 respectively. So don't need to add
             * VLAN tag size in FRM_LENGTH for FL GPORT.
             */
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            *size -= 4;
        }
    }

    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

int
soc_esw_portctrl_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
#ifdef PORTMOD_SUPPORT
    int rv;
    portctrl_pport_t pport;
    portmod_pause_control_t control;

    SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

    PORTCTRL_PORT_TO_PPORT(port, pport);

    SOC_IF_ERROR_RETURN(portmod_pause_control_t_init(unit, &control));

    rv = portmod_port_pause_control_get(unit, pport, &control);
    if (PORTMOD_SUCCESS(rv)) {
        control.tx_enable = pause_tx ? TRUE : FALSE;
        control.rx_enable = pause_rx ? TRUE : FALSE;
        rv = portmod_port_pause_control_set(unit, pport, &control);
    }
    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

int
soc_esw_portctrl_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
#ifdef PORTMOD_SUPPORT
    int rv;
    portctrl_pport_t pport;
    portmod_pause_control_t control;

    SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

    PORTCTRL_PORT_TO_PPORT(port, pport);

    SOC_IF_ERROR_RETURN(portmod_pause_control_t_init(unit, &control));

    rv = portmod_port_pause_control_get(unit, pport, &control);
    if (PORTMOD_SUCCESS(rv)) {
        *pause_tx = control.tx_enable ? TRUE : FALSE;
        *pause_rx = control.rx_enable ? TRUE : FALSE;
    }
    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_ifg_set
 * Purpose:
 *      Set Inter Frame Gap
 * Parameters:
 *      unit    - StrataSwitch unit number.
 *      port    - Port.
 *      speed   - Speed
 *      duplex  - Duplex Setting
 *      ifg     - InterFrame Gap
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL - internal error.
 */
int
soc_esw_portctrl_ifg_set(int unit, soc_port_t port, int speed,
                         soc_port_duplex_t duplex, int ifg)
{
#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    portctrl_pport_t pport;
    soc_ipg_t *si;
    int real_ifg = 0;
    int         cur_speed;
    int         cur_duplex;
    portmod_port_ability_t ability;
    uint32      pa_flag;
#ifdef PORTMOD_PM8X50_SUPPORT
    portmod_dispatch_type_t pm_type = portmodDispatchTypeCount;
    int phy_port;
#endif

    SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

    PORTCTRL_PORT_TO_PPORT(port, pport);

#ifdef PORTMOD_PM8X50_SUPPORT
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
         SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_type_get) {
        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_type_get(
                                        unit, phy_port, ((int *)&pm_type));
    }

    /* PM8x50 doesn't support ability local get */
    if (pm_type != portmodDispatchTypePm8x50)
#endif
    {
    pa_flag = SOC_PA_SPEED(speed);
    sal_memset(&ability, 0, sizeof(portmod_port_ability_t));

    rv = portmod_port_ability_local_get(unit, pport, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &ability);
    SOC_IF_ERROR_RETURN(rv);

    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }
    }

    SOC_IF_ERROR_RETURN(portmod_port_duplex_get(unit, pport, &cur_duplex));
    SOC_IF_ERROR_RETURN(portmod_port_speed_get(unit, pport, &cur_speed));

    /* XLMAC_MODE supports only 4 speeds with 4 being max as LINK_10G_PLUS */
    if ((speed > 10000) && (cur_speed == 10000)) {
        cur_speed = speed;
    }

    if (cur_speed == speed &&
        cur_duplex == (duplex == SOC_PORT_DUPLEX_FULL ? TRUE : FALSE)) {
            rv = portmod_port_tx_average_ipg_set (unit, pport, ifg);
            SOC_IF_ERROR_RETURN(rv);
    }

    if (PORTMOD_SUCCESS(rv)) {
        SOC_IF_ERROR_RETURN(portmod_port_tx_average_ipg_get(unit, pport, &real_ifg));
        si = &SOC_PERSIST(unit)->ipg[port];
        if (IS_HG_PORT(unit, port)) {
            si->fd_hg = real_ifg ;
        } else {
#if defined(BCM_FIRELIGHT_SUPPORT)
            if (SOC_IS_FIRELIGHT(unit) && IS_QSGMII_PORT(unit, port)) {
                if (duplex == BCM_PORT_DUPLEX_FULL) {
                    switch (speed) {
                        case 10:
                            si->fd_10 = real_ifg;
                            break;
                        case 100:
                            si->fd_100 = real_ifg;
                            break;
                        case 1000:
                            si->fd_1000 = real_ifg;
                            break;
                        case 2500:
                            si->fd_2500 = real_ifg;
                            break;
                        default:
                            return BCM_E_PARAM;
                    }
                } else {
                    switch (speed) {
                        case 10:
                            si->hd_10 = real_ifg;
                            break;
                        case 100:
                            si->hd_100 = real_ifg;
                            break;
                        case 1000:
                            si->hd_1000 = real_ifg;
                            break;
                        case 2500:
                            si->hd_2500 = real_ifg;
                            break;
                        default:
                            return BCM_E_PARAM;
                    }
                }
            } else
#endif
            {
                si->fd_xe = real_ifg ;
            }
        }
    }

    return rv;
#else /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_ifg_get
 * Purpose:
 *      Get Inter Frame Gap
 * Parameters:
 *      unit    - StrataSwitch unit number.
 *      port    - Port.
 *      speed   - Speed
 *      duplex  - Duplex Setting
 *      ifg     - InterFrame Gap
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL - internal error.
 */
int
soc_esw_portctrl_ifg_get(int unit, soc_port_t port, int speed,
                         soc_port_duplex_t duplex, int *ifg)
{
#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    soc_ipg_t  *si;
    uint32      pa_flag;
    portctrl_pport_t pport;
    portmod_port_ability_t ability;

    SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

    PORTCTRL_PORT_TO_PPORT(port, pport);

    if (!SOC_IS_TOMAHAWK3(unit)) {
    pa_flag = SOC_PA_SPEED(speed);
    sal_memset(&ability, 0, sizeof(portmod_port_ability_t));

    rv = portmod_port_ability_local_get(unit, pport, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &ability);
    SOC_IF_ERROR_RETURN(rv);

    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }
    }
    si = &SOC_PERSIST(unit)->ipg[port];
    if (IS_HG_PORT(unit, port)) {
        *ifg = si->fd_hg;
    } else {
#if defined(BCM_FIRELIGHT_SUPPORT)
        if (SOC_IS_FIRELIGHT(unit) && IS_QSGMII_PORT(unit, port)) {
            if (duplex == BCM_PORT_DUPLEX_FULL) {
                switch (speed) {
                    case 10:
                        *ifg = si->fd_10;
                        break;
                    case 100:
                        *ifg = si->fd_100;
                        break;
                    case 1000:
                        *ifg = si->fd_1000;
                        break;
                    case 2500:
                        *ifg = si->fd_2500;
                        break;
                    default:
                        return BCM_E_PARAM;
                }
            } else {
                switch (speed) {
                    case 10:
                        *ifg = si->hd_10;
                        break;
                    case 100:
                        *ifg = si->hd_100;
                        break;
                    case 1000:
                        *ifg = si->hd_1000;
                        break;
                    case 2500:
                        *ifg = si->hd_2500;
                        break;
                    default:
                        return BCM_E_PARAM;
                }
            }
        } else
#endif
        {
            *ifg = si->fd_xe;
        }
    }

    return rv;
#else /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_mac_reset_set
 * Purpose:
 *      Configure MAC_CTRL.SOFT_RESET.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Port number.
 *      val  -   (IN) Value.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_portctrl_mac_reset_set(int unit, soc_port_t port, int val)
{
#ifdef PORTMOD_SUPPORT
    portctrl_pport_t pport;

    PORTCTRL_PORT_TO_PPORT(port, pport);

    return portmod_port_mac_reset_set(unit, pport, val);

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_led_chain_config
 * Purpose:
 *      Set the port mode
 * Parameters:
 *      unit - SOC Unit #
 *      port - Port #
 *      value - Intra-Port delay for each subports in PORT block
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_led_chain_config(int unit, int port, int value)
{
#ifdef PORTMOD_SUPPORT
    return portmod_port_led_chain_config(unit, port, value);

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_phy_name_get
 * Purpose:
 *      Get the phy name of the given port
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port #.
 *      phy_name - PHY name string. Note: The max length of PHY name is 32.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_phy_name_get(int unit, int port, char *phy_name)
{
#ifdef PORTMOD_SUPPORT
    phymod_core_access_t core_acc;
    phymod_core_info_t core_info;
    int nof_cores = 0;
    char    *pname = NULL, namelen;
    portmod_port_diag_info_t diag_info;
    int phy = 0, core_num = 0, pcount = 0;
    int range_start = 0;
    int rv;

    if (NULL == phy_name) {
        return SOC_E_PARAM;
    }
    sal_memset(&diag_info, 0, sizeof(portmod_port_diag_info_t));
    sal_memset(phy_name, 0, sizeof(char)*SOC_PM_PHY_NAME_MAX_LEN);

    portmod_port_core_access_get(unit, port, -1, 1, &core_acc, &nof_cores, NULL);
    if (nof_cores == 0) {
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                 (BSL_META_UP(unit, port,
                              "ERROR: getting port%d information.\n"),
                  port));
        sal_snprintf(phy_name, SOC_PM_PHY_NAME_MAX_LEN, "%s", "<nophy>");
        return SOC_E_NONE;
    }

    /*
     * COVERITY
     *
     * It doesn't check return value.
     */
    /* coverity[check_return] */
    phymod_core_info_get(&core_acc, &core_info);
    rv = portmod_port_diag_info_get(unit, port, &diag_info);
    if (SOC_FAILURE(rv)) {
        return rv;
    }
    rv = portmod_port_core_num_get(unit, port, &core_num);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    PORTMOD_PBMP_ITER(diag_info.phys, phy){
        range_start = phy;
        break;
    }

    PORTMOD_PBMP_COUNT(diag_info.phys, pcount);
    pname = phymod_core_version_t_mapping[core_info.core_version].key;
    namelen = strlen(pname);

    sal_snprintf(phy_name, SOC_PM_PHY_NAME_MAX_LEN, "%s", pname);
    sal_snprintf(phy_name+namelen, SOC_PM_PHY_NAME_MAX_LEN-namelen, "/%02d/", core_num);

    pname = phy_name+strlen(phy_name);
    if (pcount == 4)
       sal_snprintf(pname, SOC_PM_PHY_NAME_MAX_LEN-namelen-4, "%d", pcount);
    else if (pcount == 2)
       sal_snprintf(pname, SOC_PM_PHY_NAME_MAX_LEN-namelen-4, "%d-%d", (range_start-1)%4, ((range_start-1)%4)+1);
    else
       sal_snprintf(pname, SOC_PM_PHY_NAME_MAX_LEN-namelen-4, "%d", (range_start-1)%4);

    return SOC_E_NONE;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_port_mode_set
 * Purpose:
 *      Set the port mode
 * Parameters:
 *      unit - SOC Unit #
 *      port - Port #
 *      mode - port mode
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_port_mode_set(int unit, int port, int mode)
{
#ifdef PORTMOD_SUPPORT
    portmod_port_mode_info_t port_mode_info;

    portmod_port_mode_info_t_init(unit, &port_mode_info);

    port_mode_info.cur_mode = mode;

    return portmod_port_mode_set(unit, port, &port_mode_info);

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_port_mode_get
 * Purpose:
 *      Get the port mode
 * Parameters:
 *      unit - SOC Unit #
 *      port - Port #
 *      mode - port mode
 *      lanes - lanes of the port
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_port_mode_get(int unit, int port, int *mode, int *lanes)
{
#ifdef PORTMOD_SUPPORT
    int rv;
    portmod_port_mode_info_t port_mode_info;

    portmod_port_mode_info_t_init(unit, &port_mode_info);

    rv = portmod_port_mode_get(unit, port, &port_mode_info);

    if (SOC_SUCCESS(rv)) {
        *mode = port_mode_info.cur_mode;
        *lanes = port_mode_info.lanes;
    }

    return rv;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_phy_control_set
 * Purpose:
 *      Set PHY specific configuration
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phyn - Phy number
 *      phy_lane - Lane number
 *      sys  - Control set to be applied
 *             on system side(1)or line side(0).
 *      phy_ctrl - PHY control type to change
 *      value    - New setting for the PHY control
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_phy_control_set(int unit, soc_port_t port, int phyn,
                             int phy_lane, int sys_side,
                             soc_phy_control_t phy_ctrl, uint32 value)
{
#ifdef PORTMOD_SUPPORT
    phymod_phy_access_t pm_acc;
    phymod_phy_access_t pm_acc12[3];
    uint32_t saved_lane_mask[3];
    int i=0;
    int num_pm_acc;
    portmod_access_get_params_t params;
    int is_legacy_ext_phy_present = 0;
    int per_lane_ctrl = 1;
    int ref_clk;
    int pport;
    portmod_dispatch_type_t pm_type;
    uint32_t port_dynamic_state = 0;
#if defined(PORTMOD_PM12X10_SUPPORT)
    int is_most_ext = 0, phy_chain = 0, core_count;
    phymod_phy_access_t temp_pm_acc12[3];
#endif

    SOC_IF_ERROR_RETURN
        (portmod_access_get_params_t_init(unit, &params));

    /*
        phyn ==  0  ( outermost )
        phyn ==  1  ( internal )
        phyn ==  >1 ( ext phy )

        portmod internal
        phyn == -1 (outermost )
        phyn == 0 ( internal )
        phyn == >=1 external phy#
    */

    if( phyn >= 1) {
        params.phyn = phyn-1;
    } else {
        params.phyn = -1;
    }

    params.lane = phy_lane;
    params.sys_side = (sys_side == 1) ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;
    params.apply_lane_mask = 1;

    if (IS_C_PORT(unit, port) ||
        (IS_CXX_PORT(unit, port) && IS_CL_PORT(unit, port))) {
        SOC_IF_ERROR_RETURN
            (portmod_port_phy_lane_access_get(unit, port, &params,
                                              3, &pm_acc12[0], &num_pm_acc, NULL));
        /* if the phys  are identical, consolidate lane mask and run  once. */
        if ( num_pm_acc == 3 ){
            for(i=0; i < num_pm_acc; i++) {
                saved_lane_mask[i] = pm_acc12[i].access.lane_mask;
                pm_acc12[i].access.lane_mask = 0; /* need this to do compare. */
            }

            if( !sal_memcmp( &pm_acc12[0],&pm_acc12[1], sizeof(phymod_phy_access_t)) &&
                !sal_memcmp( &pm_acc12[0],&pm_acc12[2], sizeof(phymod_phy_access_t))){
                pm_acc12[0].access.lane_mask = ( saved_lane_mask[0]| saved_lane_mask[1]| saved_lane_mask[2]);
                phymod_access_t_init(&pm_acc12[1].access);
                phymod_access_t_init(&pm_acc12[2].access);
                /* cli_out("Consolidating entries.\n");*/
                num_pm_acc = 1;
            } else {
                /* restore lane mask */
                for(i=0; i < num_pm_acc; i++) {
                    pm_acc12[i].access.lane_mask = saved_lane_mask[i];
                }
            }
        }

    } else {
        SOC_IF_ERROR_RETURN
            (portmod_port_phy_lane_access_get(unit, port, &params,
                                              1, &pm_acc, &num_pm_acc, NULL));
    }

    if (num_pm_acc == 0) {
        return SOC_E_FAIL;
    }

    SOC_IF_ERROR_RETURN
        (portmod_port_ref_clk_get(unit, port, &ref_clk));

    switch (phy_ctrl) {
        /* Not per lane control */
        case SOC_PHY_CONTROL_LANE_SWAP:
            per_lane_ctrl = 0;
            break;
        default:
            break;
    }

    /* only check if phy is legacy when phyn is not 0 (in portmod internal phy is always not legacy) */
    if(params.phyn != 0) {
      SOC_IF_ERROR_RETURN(
        portmod_port_is_legacy_ext_phy_present(unit, port, &is_legacy_ext_phy_present));
    }

    if(!is_legacy_ext_phy_present) {
        if (IS_C_PORT(unit, port) ||
            (IS_CXX_PORT(unit, port) && IS_CL_PORT(unit, port))) {
            if (phy_ctrl == SOC_PHY_CONTROL_CL72) {
                SOC_IF_ERROR_RETURN
                    (portmod_port_pm_type_get(unit, port, &pport, &pm_type));
#if defined(PORTMOD_PM12X10_SUPPORT)
                if (pm_type == portmodDispatchTypePm12x10) {
                    while (!is_most_ext) {
                        params.phyn = phy_chain;
                        SOC_IF_ERROR_RETURN
                            (portmod_port_phy_lane_access_get(unit, port, &params,
                                                              3, &temp_pm_acc12[0], &core_count, &is_most_ext));

                        phy_chain++;
                    }
                    /*Invoke portmod_port_cl72_set for TSCE to configure CL72.
                      For external PHY CL72, invoke soc_port_control_set_wrapper.
                     */
                    if (phyn == 1 || phy_chain ==1) {
                        SOC_IF_ERROR_RETURN (
                            portmod_port_cl72_set(unit, port, value));
                        return SOC_E_NONE;
                    }
                }
#endif
            }
            SOC_IF_ERROR_RETURN(
                soc_port_control_set_wrapper(unit, ref_clk, per_lane_ctrl,
                                             &pm_acc12[0], num_pm_acc, phy_ctrl, value));
        } else if (IS_CPRI_PORT(unit,port) ||
                   IS_RSVD4_PORT(unit,port)) {
            switch(phy_ctrl) {
                case SOC_PHY_CONTROL_FEC_BYPASS_INDICATION_ENABLE:
                    LOG_CLI((BSL_META_U(unit, "This particular Phy Control is Not supported. Use CPRI API.\n")));
                    return SOC_E_UNAVAIL;
                break;

                default:
                    /*
                     * This call is needed for PMD related control.
                     */
                    SOC_IF_ERROR_RETURN
                        (soc_port_control_set_wrapper(unit, ref_clk, per_lane_ctrl,
                                             &pm_acc, 1, phy_ctrl, value));
                break;
            }
        } else {
            if (phy_ctrl != SOC_PHY_CONTROL_AUTONEG_MODE) {
                SOC_IF_ERROR_RETURN(
                    soc_port_control_set_wrapper(unit, ref_clk, per_lane_ctrl,
                                                 &pm_acc, 1, phy_ctrl, value));
            }
        }
    } else {
        SOC_IF_ERROR_RETURN(
            portmod_port_ext_phy_control_set(unit, port, phyn, phy_lane, sys_side,
                                             phy_ctrl, value));
    }

    /* update the state in portmod */
    switch(phy_ctrl) {
        case SOC_PHY_CONTROL_AUTONEG_MODE:
             PORTMOD_PORT_AUTONEG_MODE_UPDATED_SET(port_dynamic_state);
             port_dynamic_state |= value << 16;
             portmod_port_update_dynamic_state(unit, port, port_dynamic_state);
        break;
        case SOC_PHY_CONTROL_TX_FIR_PRE:
        case SOC_PHY_CONTROL_TX_FIR_MAIN:
        case SOC_PHY_CONTROL_TX_FIR_POST:
        case SOC_PHY_CONTROL_TX_FIR_POST2:
        case SOC_PHY_CONTROL_TX_FIR_POST3:
        case SOC_PHY_CONTROL_PREEMPHASIS:
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            PORTMOD_PORT_DEFAULT_TX_PARAMS_UPDATED_SET(port_dynamic_state);
            portmod_port_update_dynamic_state(unit, port, port_dynamic_state);
        break;
        default:
            break;
    }

    return SOC_E_NONE;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_phy_control_get
 * Purpose:
 *      Get PHY specific configuration
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phyn - Phy number
 *      phy_lane - Lane number
 *      sys  - Control get to be applied
 *             on system side(1)or line side(0).
 *      phy_ctrl - PHY control type to read
 *      value    - Current setting for the PHY control
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_phy_control_get(int unit, soc_port_t port, int phyn,
                             int phy_lane, int sys_side,
                             soc_phy_control_t phy_ctrl, uint32 *value)
{
#ifdef PORTMOD_SUPPORT
    phymod_phy_access_t pm_acc, pm12_acc[3];
    uint32_t saved_lane_mask[3];
    int i=0;
    int num_pm_acc;
    portmod_access_get_params_t params;
    int is_legacy_ext_phy = 0;
    int per_lane_ctrl = 1;
    int ref_clk;
    phy_ctrl_t  *ext_pc = NULL;

    SOC_IF_ERROR_RETURN
        (portmod_access_get_params_t_init(unit, &params));

    /*
        phyn == -1  ( outermost phy GPORT 0 get converted to -1)
        phyn ==  1  ( internal )
        phyn ==  >1 ( ext phy )

        portmod internal
        phyn == -1 (outermost )
        phyn == 0 ( internal )
        phyn == >=1 external phy#
    */
    if( phyn >= 1) {
        params.phyn = phyn-1;
    } else  {
        params.phyn = -1;
    }

    params.lane = phy_lane;
    params.apply_lane_mask = 1;
    params.sys_side = (sys_side == 1) ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;

    if (IS_C_PORT(unit, port) ||
        (IS_CXX_PORT(unit, port) && IS_CL_PORT(unit, port))) {
        SOC_IF_ERROR_RETURN
            (portmod_port_phy_lane_access_get(unit, port, &params,
                                          3, &pm12_acc[0], &num_pm_acc, NULL));

        /* if the phys  are identical, consolidate lane mask and run  once. */
        if ( num_pm_acc == 3 ){
            for(i=0; i < num_pm_acc; i++) {
                saved_lane_mask[i] = pm12_acc[i].access.lane_mask;
                pm12_acc[i].access.lane_mask = 0; /* need this to do compare. */
            }
            if( !sal_memcmp( &pm12_acc[0],&pm12_acc[1], sizeof(phymod_phy_access_t)) &&
                !sal_memcmp( &pm12_acc[0],&pm12_acc[2], sizeof(phymod_phy_access_t))){
                pm12_acc[0].access.lane_mask = ( saved_lane_mask[0]| saved_lane_mask[1]| saved_lane_mask[2]);
                phymod_access_t_init(&pm12_acc[1].access);
                phymod_access_t_init(&pm12_acc[2].access);
                /* cli_out("Consolidating entries.\n");*/
                num_pm_acc = 1;
            } else {
                /* restore lane mask */
                for(i=0; i < num_pm_acc; i++) {
                    pm12_acc[i].access.lane_mask = saved_lane_mask[i];
                }
            }
        }
    } else {
        SOC_IF_ERROR_RETURN
            (portmod_port_phy_lane_access_get(unit, port, &params,
                                          1,  &pm_acc, &num_pm_acc, NULL));
    }
    if (IS_ROE_PORT(unit, port)) {
        if(phy_ctrl==SOC_PHY_CONTROL_EEE) {
            return SOC_E_UNAVAIL;
        }
    }
    if (num_pm_acc == 0) {
        return SOC_E_FAIL;
    }

    SOC_IF_ERROR_RETURN
        (portmod_port_ref_clk_get(unit, port, &ref_clk));

    switch (phy_ctrl) {
        /* Not per lane control */
        case SOC_PHY_CONTROL_LANE_SWAP:
            per_lane_ctrl = 0;
            break;
        case SOC_PHY_CONTROL_FEC_CORRECTED_CODEWORD_COUNT:
            per_lane_ctrl = 0;
            break;
        case SOC_PHY_CONTROL_FEC_UNCORRECTED_CODEWORD_COUNT:
            per_lane_ctrl = 0;
            break;
        default:
            break;
    }

    /* only check if phy is legacy when phyn is not 0 (in portmod internal phy is always not legacy) */
    if(params.phyn != 0) {
        SOC_IF_ERROR_RETURN(
            portmod_port_is_legacy_ext_phy_present(unit, port, &is_legacy_ext_phy));
    }

    if (!is_legacy_ext_phy) {
        if (IS_C_PORT(unit, port) ||
            (IS_CXX_PORT(unit, port) && IS_CL_PORT(unit, port))) {
            SOC_IF_ERROR_RETURN(
                soc_port_control_get_wrapper(unit, ref_clk, per_lane_ctrl,
                                             &pm12_acc[0], num_pm_acc, phy_ctrl, value));
        } else if (IS_CPRI_PORT(unit,port) ||
                   IS_RSVD4_PORT(unit,port)) {
            switch(phy_ctrl) {
                case SOC_PHY_CONTROL_FEC_BYPASS_INDICATION_ENABLE:
                    LOG_CLI((BSL_META_U(unit, "This particular Phy Control is Not supported. Use CPRI API.\n")));
                    return SOC_E_UNAVAIL;
                break;

                default:
                    /*
                     * This call is needed for PMD related control.
                     */
                    SOC_IF_ERROR_RETURN
                        (soc_port_control_get_wrapper(unit, ref_clk, per_lane_ctrl,
                                             &pm_acc, 1, phy_ctrl, value));
                break;
            }
        } else {
            SOC_IF_ERROR_RETURN(
                soc_port_control_get_wrapper(unit, ref_clk, per_lane_ctrl,
                                             &pm_acc, 1, phy_ctrl, value));
        }
    } else {
        if (phy_ctrl == SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91) {
            ext_pc = EXT_PHY_SW_STATE(unit, port);
            if (ext_pc == NULL) {
                return SOC_E_FAIL;
            }

            /* If not Montreal2 external PHY */
            if ((ext_pc->phy_id0 != 0xae02) || (ext_pc->phy_id1 != 0x52c0)) {
                /* Legacy ext PHYs do not support CL91, below we return SOC_E_UNAVAIL */
                return SOC_E_UNAVAIL;
            }
        }

        SOC_IF_ERROR_RETURN(
            portmod_port_ext_phy_control_get(unit, port, phyn, phy_lane, sys_side,
                                             phy_ctrl, value));
    }

    return SOC_E_NONE;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_phy_timesync_config_set
 * Purpose:
 *      Set timesync config
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phyn - Phy number
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_phy_timesync_config_set(int unit, soc_port_t port,
                                     soc_port_phy_timesync_config_t *conf)
{
#ifdef PORTMOD_SUPPORT
    int         rv;

    INT_MCU_LOCK(unit);
    rv = portmod_port_timesync_config_set(unit, port, conf);
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "_soc_portctrl_phy_timesync_config_set failed %d\n"), rv));
    }
    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_phy_timesync_config_set
 * Purpose:
 *      Set timesync config
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phyn - Phy number
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_phy_timesync_config_get(int unit, soc_port_t port,
                                     soc_port_phy_timesync_config_t *conf)
{
#ifdef PORTMOD_SUPPORT
    int         rv;

    INT_MCU_LOCK(unit);
    rv = portmod_port_timesync_config_get(unit, port, conf);
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "_soc_portctrl_phy_timesync_config_get failed %d\n"), rv));
    }
    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_control_phy_timesync_config_set
 * Purpose:
 *      Set timesync config
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phyn - Phy number
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_control_phy_timesync_set(int unit, soc_port_t port,
                                      soc_port_control_phy_timesync_t type,
                                      uint64 value)
{
#ifdef PORTMOD_SUPPORT
    int         rv;

    INT_MCU_LOCK(unit);
    rv = portmod_port_control_phy_timesync_set(unit, port, type, value);
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "_soc_portctrl_control_phy_timesync_set failed %d\n"), rv));
    }
    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_control_phy_timesync_config_get
 * Purpose:
 *      Set timesync config
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phyn - Phy number
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_control_phy_timesync_get(int unit, soc_port_t port,
                                      soc_port_control_phy_timesync_t type,
                                      uint64 *value)
{
#ifdef PORTMOD_SUPPORT
    int         rv;

    INT_MCU_LOCK(unit);
    rv = portmod_port_control_phy_timesync_get(unit, port, type, value);
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "_soc_portctrl_phy_timesync_set failed %d\n"), rv));
    }
    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_encap_get
 * Purpose:
 *      Get the port encapsulation mode.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Port number.
 *      mode   - (OUT) One of _SHR_PORT_ENCAP_xxx.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_portctrl_encap_get(int unit, soc_port_t port, int *mode)
{
#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    int flags = 0;
    portctrl_pport_t pport;
    portmod_encap_t encap;

    PORTCTRL_PORT_TO_PPORT(port, pport);

    SOC_CONTROL_LOCK(unit);
    rv = portmod_port_encap_get(unit, pport, &flags, &encap);
    SOC_CONTROL_UNLOCK(unit);

    if (PORTMOD_SUCCESS(rv)) {
        *mode = encap;
    }

    return rv;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_port_to_phyaddr
 * Purpose:
 *      This function gets phy address associated with the port
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Port number.
 * Returns:
 *      Phy address value
 */
int
soc_esw_portctrl_port_to_phyaddr(int unit, soc_port_t port, uint8* phy_addr)
{
#ifdef PORTMOD_SUPPORT
    int addr;
    portctrl_pport_t pport;
    int rv = SOC_E_NONE;

    PORTCTRL_PORT_TO_PPORT(port, pport);

    SOC_CONTROL_LOCK(unit);
    addr = portmod_port_to_phyaddr(unit, pport);
    SOC_CONTROL_UNLOCK(unit);

    /* Portmod API returns phy address on successful execution, and
     * negative value in case of error.
     * So we program phy_addr, rv accordingly
     */
    /* coverity[negative_returns] */
    if (addr >= 0) {
        *phy_addr = addr;
    } else {
        rv = addr;
    }

    return rv;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_speed_get
 * Purpose:
 *      Get the port speed.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Port number.
 *      speed  - (OUT) Port speed.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_portctrl_speed_get(int unit, soc_port_t port, int *speed)
{
#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    portctrl_pport_t pport;
    portmod_port_interface_config_t portmod_if_config;

    PORTCTRL_PORT_TO_PPORT(port, pport);

    /*
     * Temporary solution until PortMod null driver is in place
     */
    if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port)) {
        *speed = SOC_INFO(unit).port_speed_max[port];
        return SOC_E_NONE;
    }

    SOC_CONTROL_LOCK(unit);
    rv = portmod_port_interface_config_get(unit, pport, &portmod_if_config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY);
    SOC_CONTROL_UNLOCK(unit);

    if (PORTMOD_SUCCESS(rv)) {
        *speed = portmod_if_config.speed;

        if (IS_HG_PORT(unit, port) && *speed < 5000) {
            *speed = 0;
        }
    }

    return rv;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *     _soc_esw_portctrl_speed_slots_get
 * Purpose:
 *     Get number of slots required for given port speed
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_esw_portctrl_speed_slots_get(int unit, int speed)
{
    int slot_num = 0;

    switch(speed) {
    case 10:
    case 100:
    case 1000:
    case 2500:
        slot_num = 1;
        break;
    case 5000:
    case 10000:
    case 11000:
        slot_num = 4;
        break;
    case 13000:
    case 16000:
        if(SOC_IS_TITAN2PLUS(unit)){
            slot_num = 8;
        } else {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      ("Unsupported port speed %d\n", speed));
        }
        break;
    case 20000:
    case 21000:
        slot_num = 8;
        break;
    case 25000:
    case 25450:
    case 27000:
        slot_num = 10;
        break;
    case 40000:
    case 42000:
        slot_num = 16;
        break;
    case 50000:
    case 53000:
        slot_num = 20;
        break;
    case 100000:
    case 106000:
        slot_num = 40;
        break;
    case 120000:
    case 127000:
        slot_num = 48;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT,
                  ("Unsupported port speed %d\n", speed));
        break;
    }

    return (slot_num);
}

/*
 * Function:
 *      soc_port_is_same_speed_class
 * Purpose:
 *      Check if pre,post speeds are same speed class
 *      to know TDM reconfiguration is required or not.
 *      Speed Class is defined as below
 *      -------------------------
 *      | Group    | Speeds     |
 *      |=======================|
 *      | 0        | 1G, 2.5G   |
 *      | 1        | 10G, 11G   |
 *      | 2        | 20G, 21G   |
 *      | 3        | 25G, 27G   |
 *      | 4        | 40G, 42G   |
 *      | 5        | 100G, 106G |
 *      | 6        | 120G, 127G |
 *      -------------------------
 *      If speed group is different, then need to reconfigure TDM
 */
int
soc_esw_portctrl_is_same_speed_class(int unit, int pre_speed, int post_speed)
{
    int rv;
    int pre_slot, post_slot;

    pre_slot = _soc_esw_portctrl_speed_slots_get(unit, pre_speed);
    post_slot = _soc_esw_portctrl_speed_slots_get(unit, post_speed);

    if (pre_slot == post_slot) {
        rv = TRUE;
    } else {
        rv = FALSE;
    }

    return rv;
}

/*
 * Function:
 *      soc_esw_portctrl_port_ability_update
 * Purpose:
 *      Update supported abilities of a port
 * Parameters:
 *      unit - StrataSwitch unit #
 *      port - StrataSwitch port #
 *      ability - (OUT) Returns updated soc_port_ability_t structure information.
 *
 * Returns:
 *      SOC_E_XXX
 */
soc_error_t
soc_esw_portctrl_port_ability_update(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int rv = SOC_E_NONE;
#ifdef PORTMOD_SUPPORT
    if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
        (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_port_ability_update)) {
        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_port_ability_update(unit, port, ability);
    } else {
        rv = SOC_E_NONE;
    }
    return rv;
#else
    return rv;
#endif
}

/*
 * Function:
 *      soc_esw_portctrl_pll_div_get
 * Purpose:
 *      Get VCO frequency(PLL div) for port.
 *      PLL DIV value can be converted to VCO frequency by following equation.
 *      VCO = (PLL_DIV) * (REF_CLK)
 *
 *      Here is the complete table for TSCE(TD2+)
 *    ----------------------------------------------------------
 *    | encap type | speed | lanes | VCO     | PLL_DIV | REF_CLK
 *    | IEEE       | 1     | 1     | 10312.5 | 66      | 156.25
 *    | IEEE       | 1     | 1     | 6250    | 40      | 156.25
 *    | IEEE       | 2.5   | 1     | 6250    | 40      | 156.25
 *    | IEEE       | 10    | 1     | 10312.5 | 66      | 156.25
 *    | HIGIG2     | 10    | 1     | 10312.5 | 66      | 156.25
 *    | HIGIG2     | 11    | 1     | 10937.5 | 70      | 156.25
 *    | IEEE       | 20    | 2     | 10312.5 | 66      | 156.25
 *    | HIGIG2     | 20    | 2     | 10312.5 | 66      | 156.25
 *    | HIGIG2     | 21    | 2     | 10937.5 | 70      | 156.25
 *    | IEEE       | 10    | 4     | 6250    | 40      | 156.25
 *    | HIGIG2     | 10    | 4     | 6250    | 40      | 156.25
 *    | HIGIG2     | 11    | 4     | 6562.5  | 42      | 156.25
 *    | IEEE       | 40    | 4     | 10312.5 | 66      | 156.25
 *    | HIGIG2     | 40    | 4     | 10312.5 | 66      | 156.25
 *    | HIGIG2     | 42    | 4     | 10937.5 | 70      | 156.25
 *    | IEEE       | 100   | 10    | 10312.5 | 66      | 156.25
 *    | HIGIG2     | 100   | 10    | 10312.5 | 66      | 156.25
 *    | HIGIG2     | 106   | 10    | 10937.5 | 70      | 156.25
 *    | IEEE       | 120   | 12    | 10312.5 | 66      | 156.25
 *    | HIGIG2     | 127   | 12    | 10937.5 | 70      | 156.25
 *    ----------------------------------------------------------
 *
 * Parameters:
 *      unit     - (IN) Unit number
 *      logic_port - (IN) logical port number
 *      phy_port   - (IN) Physical port number
 *      speed    - (IN) Speed
 *      num_lane - (IN) Number of lanes
 *      encap    - (IN) Encap mode
 *      pll_div  - (OUT) pll div
 * Returns:
 * Note:
 */
/*In flex port, there are some logic ports are generated. And the new ports is not initialized in Add SOC_INFO(unit).port_p2l_mapping.
  That is to say we can`t retrieve the logical port via physical port.
 */
int
soc_esw_portctrl_pll_div_get(int unit, soc_port_t logic_port, soc_port_t phy_port,
                             int speed, int num_lane, int encap,
                             uint32 *pll_div)
{
#ifdef PORTMOD_SUPPORT
    int rv;
    portmod_port_resources_t pr;
    int serdes_1000x_at_25g_vco = 0;
    int serdes_10g_at_25g_vco = 0;
    int serdes_1000x_at_6250_vco = 0;
    int serdes_1000x_at_12500_vco = 0;

#if (defined BCM_HELIX5_SUPPORT) || (defined BCM_MAVERICK2_SUPPORT) || (defined BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_HELIX5(unit) ||
        SOC_IS_HURRICANE4(unit) ||
        (soc_feature(unit, soc_feature_vco_freq_config_enable)) ||
        (SOC_IS_MAVERICK2(unit) && SOC_INFO(unit).bandwidth == 2000000) ||
        SOC_IS_FIRELIGHT(unit)) {
        serdes_1000x_at_25g_vco = 1;
        serdes_10g_at_25g_vco = 1;
    }
#endif
    pr.flag = 0;
    /*In flex port, customer can configure the following properties to select pll DIV for the new generated ports.
      So we need to parse these properties here directly.
     */
    serdes_1000x_at_6250_vco =
        soc_property_port_get(unit, logic_port,
                              spn_SERDES_1000X_AT_6250_VCO, serdes_1000x_at_6250_vco);
    if (serdes_1000x_at_6250_vco) {
        PORTMOD_SERDES_1000X_AT_6250_SET(pr.flag);
    }

    serdes_1000x_at_12500_vco =
        soc_property_port_get(unit, logic_port,
                      spn_SERDES_1000X_AT_12500_VCO, serdes_1000x_at_12500_vco);
    if (serdes_1000x_at_12500_vco) {
        PORTMOD_SERDES_1000X_AT_12500_SET(pr.flag);
    }

    serdes_1000x_at_25g_vco =
        soc_property_port_get(unit, logic_port,
                      spn_SERDES_1000X_AT_25G_VCO, serdes_1000x_at_25g_vco);
    if (serdes_1000x_at_25g_vco) {
        PORTMOD_SERDES_1000X_AT_25G_SET(pr.flag);
    }

    serdes_10g_at_25g_vco =
        soc_property_port_get(unit, logic_port,
                      spn_SERDES_10G_AT_25G_VCO, serdes_10g_at_25g_vco);
    if (serdes_10g_at_25g_vco) {
        PORTMOD_SERDES_10G_AT_25G_SET(pr.flag);
    }

    pr.speed = speed;
    pr.num_lane = num_lane;
    pr.encap_mode = encap;

    SOC_CONTROL_LOCK(unit);
    rv = portmod_pm_port_pll_div_get(unit, logic_port, phy_port, &pr, pll_div);
    SOC_CONTROL_UNLOCK(unit);
    if (PORTMOD_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "Failed to get pll_div\n")));
        return rv;
    }

    return SOC_E_NONE;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portmod_port_vcos_get
 * Purpose:
 *      Verify if the given port combination is supported on a port macro
 *      Only used by monterey
 * Parameters:
 *      unit - StrataSwitch unit #
 *      phy_port - array of ports in given port macro #
 *      lanes - lanes for each port in given port macro #
 *      speed - speed of each port in port macro #
 *      encap - encap of each  port in port macro #
 *      size -  no of ports in port macro #
 *      flex -  When 1 it the call is part on flex call (coming from mixed_sisters_validate)
 *              when 0 it is part of init call (coming from mixed_sister_speed_validate)
 *      flexing_ports - List of ports that are deleted by (-1) in resource.
 *                      Only checked when flex is 1
 * Returns:
 *      SOC_E_XXX
 */
#ifdef PORTMOD_SUPPORT
int
soc_esw_portmod_port_vcos_get(int unit, soc_port_t phy_port,
                              portmod_dispatch_type_t pm_type, int* lanes,
                              int* speed, int* encap, int size, int flex, portmod_pbmp_t flexing_ports)
{

     #ifdef PORTMOD_CPM4X25_SUPPORT
     portmod_vcos_speed_config_t speed_config_list[4];
     portmod_speed_config_t flex_speed_config_list[4];
     portmod_core_port_enet_cpri_type_t port_type_list[4];
     portmod_dual_vcos_t dual_vco;
     portmod_pm_vco_setting_t pm_vco_setting;
     int ethernet = 0 ;
     int cpri = 0 ;
     int i = 0 ;
     int rv = 0;
     int portmod_pm_id;
     int check_both_vco = 0;

     sal_memset(&pm_vco_setting, 0, sizeof(pm_vco_setting));

     if ((pm_type != portmodDispatchTypePm4x25) && (pm_type != portmodDispatchTypeCpm4x25)) {
          LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "soc_esw_portmod_port_vcos_get not supported for pm_type %d\n"), pm_type));
     }
     for (i = 0 ; i < size ; i++) {
          speed_config_list[i].speed = speed[i];
          speed_config_list[i].num_lanes = lanes[i];
          flex_speed_config_list[i].speed = speed[i];
          flex_speed_config_list[i].num_lane = lanes[i];
          speed_config_list[i].eth_of_mixed_mode = 0;
          speed_config_list[i].higig_mode = 0;
          speed_config_list[i].ieee_mode = 0;
          if (encap[i] ==  SOC_ENCAP_IEEE) {
              speed_config_list[i].ieee_mode = 1;
              speed_config_list[i].port_type = portmodCpmPrtEthMode;
              port_type_list[i] = portmodCpmPrtEthMode;
              ethernet = 1;
          } else if (encap[i] ==  SOC_ENCAP_CPRI) {
              speed_config_list[i].port_type = portmodCpmPrtCpriMode;
              port_type_list[i] = portmodCpmPrtCpriMode;
              cpri = 1 ;
          } else if (encap[i] ==  SOC_ENCAP_RSVD4) {
              speed_config_list[i].port_type = portmodCpmPrtRsvd4Mode;
              port_type_list[i] = portmodCpmPrtRsvd4Mode;
              cpri = 1 ;
          } else if ((encap[i] == SOC_ENCAP_HIGIG) || (encap[i] == SOC_ENCAP_HIGIG2)) {
             speed_config_list[i].higig_mode = 1;
             speed_config_list[i].port_type = portmodCpmPrtEthMode;
             port_type_list[i] = portmodCpmPrtEthMode;
             ethernet = 1;
          }

     }
    if (ethernet && cpri) {
        for (i = 0 ; i < size ; i++) {
             if ((encap[i] ==  SOC_ENCAP_IEEE) || (encap[i] == SOC_ENCAP_HIGIG) || (encap[i] == SOC_ENCAP_HIGIG2) ) {
                 speed_config_list[i].eth_of_mixed_mode = 1;
                 speed_config_list[i].port_type = portmodCpmPrtEthOfMixedMode;
                 port_type_list[i] = portmodCpmPrtEthOfMixedMode;
             } else if (speed_config_list[i].port_type == portmodCpmPrtCpriMode) {
                 speed_config_list[i].port_type = portmodCpmPrtCpriOfMixedMode;
                 port_type_list[i] = portmodCpmPrtCpriOfMixedMode;
             } else if (speed_config_list[i].port_type == portmodCpmPrtRsvd4Mode) {
                 speed_config_list[i].port_type = portmodCpmPrtRsvd4OfMixedMode;
                 port_type_list[i] = portmodCpmPrtRsvd4OfMixedMode;
             }
        }
    }
    /* In flex port case called from coming from mixed_sisters_validate */
    if(flex) {
        /* When all ports are CPRI check_both_vco = 1
         * In all CPRI case cpri can use any PLL for its VCO
         * That require special checking if given ports vco matches which any of existing vco
         * In Ethernet and Mix mode there is Fixed configuration for PLL
         * All Ethernet PLL1 is always 25G and Mix mode PLL0 is CPRI and PLL1 is Ethernet
         * check_both_vco helps lower level code compere vcos
         */
        if((!ethernet) && cpri) {
            check_both_vco = 1;
        } else {
            check_both_vco = 0;
        }
        portmod_phy_pm_id_get(unit, phy_port, &portmod_pm_id);
        pm_vco_setting.num_speeds = size;
        pm_vco_setting.speed_config_list = flex_speed_config_list;
        pm_vco_setting.port_type_list = port_type_list;
        /* speed_config_validate checks existing PLL usage and determine it the flex
         * is allowed
         */
        rv = portmod_pm_speed_config_validate(unit, portmod_pm_id, &flexing_ports,
                                              check_both_vco, &pm_vco_setting);
    } else {
        portmod_dual_vcos_t_init(unit , &dual_vco);
        rv = portmod_pm_supported_vcos_get(unit, phy_port, pm_type, speed_config_list, size, &dual_vco);
    }
    if (PORTMOD_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Failed to get dual_vcos\n")));
            return rv;
    }
    return SOC_E_NONE;
#else  /* PORTMOD_CPM4X25_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_CPM4X25_SUPPORT */
}
#endif /* PORTMOD_SUPPORT */

int
soc_esw_portctrl_phy_tx_set(int unit, soc_port_t port,
                           int phyn, int phy_lane,
                           int sys_side, soc_port_phy_tx_t *tx)
{
#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    phymod_tx_t phy_tx;
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;

    sal_memset(&phy_tx, 0, sizeof(phymod_tx_t));
    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));
    phymod_phy_access_t_init(&phy_access);

    params.lane = phy_lane;
    SOC_IF_ERROR_RETURN
        (portmod_port_phy_lane_access_get(unit, port, &params, 1, &phy_access, &nof_phys, NULL));

    /* convert from soc structure to phymod structure */
    phy_tx.pre = tx->pre;
    phy_tx.main = tx->main;
    phy_tx.post = tx->post;
    phy_tx.post2 = tx->post2;
    phy_tx.post3 = tx->post3;
    phy_tx.pre2  = tx->pre2;

    /* Enums for tap mode and signalling mode differ between SDK and portmod */
    switch(tx->tx_tap_mode) {
        case socPortPhyTxTapMode3Tap:
            phy_tx.tap_mode = phymodTxTapMode3Tap;
            break;
        case socPortPhyTxTapMode6Tap:
            phy_tx.tap_mode = phymodTxTapMode6Tap;
            break;
        default:
            return SOC_E_PARAM;
    }
    switch(tx->signalling_mode) {
        case socPortPhySignallingModeNRZ:
            phy_tx.sig_method = phymodSignallingMethodNRZ;
            break;
        case socPortPhySignallingModePAM4:
            phy_tx.sig_method = phymodSignallingMethodPAM4;
            break;
        default:
            return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN
        (soc_portctrl_phy_tx_set(unit, &phy_access, &phy_tx));

    return rv;
#else
    return SOC_E_UNAVAIL;
#endif
}

int
soc_esw_portctrl_phy_tx_get(int unit, soc_port_t port,
                            int phyn, int phy_lane,
                            int sys_side, soc_port_phy_tx_t *tx)
{
#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    phymod_tx_t phy_tx;
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;

    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));
    phymod_phy_access_t_init(&phy_access);

    params.lane = phy_lane;
    SOC_IF_ERROR_RETURN
        (portmod_port_phy_lane_access_get(unit, port, &params, 1, &phy_access, &nof_phys, NULL));

    SOC_IF_ERROR_RETURN
        (soc_portctrl_phy_tx_get(unit, &phy_access, &phy_tx));

    /* convert from phymod structure to soc structure */
    if (SOC_SUCCESS(rv)) {
        tx->pre2 = phy_tx.pre2;
        tx->pre  = phy_tx.pre;
        tx->main = phy_tx.main;
        tx->post = phy_tx.post;
        tx->post2 = phy_tx.post2;
        tx->post3 = phy_tx.post3;

        /* Enums for tap mode and signalling mode differ between
         * SDK and portmod */
        switch(phy_tx.tap_mode) {
            case phymodTxTapMode3Tap:
                tx->tx_tap_mode = socPortPhyTxTapMode3Tap;
                break;
            case phymodTxTapMode6Tap:
                tx->tx_tap_mode = socPortPhyTxTapMode6Tap;
                break;
            default:
                return SOC_E_INTERNAL;
        }
        switch(phy_tx.sig_method) {
            case phymodSignallingMethodNRZ:
                tx->signalling_mode = socPortPhySignallingModeNRZ;
                break;
            case phymodSignallingMethodPAM4:
                tx->signalling_mode = socPortPhySignallingModePAM4;
                break;
            default:
                return SOC_E_INTERNAL;
        }
    }
    return rv;
#else
    return SOC_E_UNAVAIL;
#endif
}

int
soc_esw_portctrl_portmod_callback_unregister(int unit, soc_port_t port)
{
#ifdef PORTMOD_SUPPORT
#ifdef PORTMOD_PM8X50_SUPPORT
    int rv = SOC_E_NONE, pport;
    portmod_dispatch_type_t pm_type;

    rv = portmod_port_pm_type_get(unit, port, &pport, &pm_type);

    if (PORTMOD_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "Failed to get pm type\n")));
        return rv;
    }

    if (pm_type == portmodDispatchTypePm8x50) {
        rv = portmod_port_callback_unregister(unit, port);
        if (PORTMOD_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Failed to unregister pm callback\n")));
            return rv;
        }
    }
#endif
    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}
