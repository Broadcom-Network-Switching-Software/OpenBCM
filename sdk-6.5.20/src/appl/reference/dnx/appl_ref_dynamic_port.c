/** \file appl_ref_dynamic_port.c
 * 
 *
 * Dynamic port application for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_PORT

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/libc.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm/link.h>
#include <bcm/switch.h>
#include <bcm/stack.h>
#include <src/bcm/dnx/tune/scheduler_tune.h>
#include <src/bcm/dnx/tune/egr_queuing_tune.h>
#include <appl/reference/dnx/appl_ref_dynamic_port.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <appl/reference/dnx/appl_ref_e2e_scheme.h>
#include <appl/reference/dnx/appl_ref_sniff_ports.h>
#include "appl_ref_compensation_init.h"
#include "appl_ref_sys_device.h"
#include <appl/reference/sand/appl_ref_sand.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

 /*
  * Global and Static
  */
/*
 * }
 */

/*
 * See .h files
 */
shr_error_e
appl_dnx_dynamic_port_create(
    int unit,
    int port)
{
    bcm_port_config_t *port_config = NULL;
    system_port_mapping_t sys_port_map;
    appl_dnx_sys_device_t *sys_params;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_resource_t port_resource;
    int system_port_id;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** create system port for the new port */
    SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &port_resource));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info));
    sys_port_map.device_index = sys_params->unit_to_device_index[unit];
    sys_port_map.local_port = port;
    sys_port_map.core = mapping_info.core;
    sys_port_map.tm_port = mapping_info.tm_port;
    sys_port_map.rate = port_resource.speed;

    /** filtered ports don't have system port and VOQs */
    if (!(flags & APPL_DNX_FILTERED_PORTS_FLAGS))
    {
        system_port_mapping_t old_sys_port_map;
        /** Convert device and logical port to system port id */
        SHR_IF_ERR_EXIT(appl_sand_device_to_sysport_convert
                        (unit, sys_port_map.device_index, APPL_DNX_NOF_SYSPORTS_PER_DEVICE,
                         sys_port_map.local_port, &system_port_id));

        /** remove entry from sys port DB if it exists there, entry is not dynamic and we shouldn't address the entry in DB as valid anymore */
        /** this is done as part dyanmic port create and not destory because we currently maintin a DB per CPU and not per FAP. so when
        removing a port dynamically we remove it from only one unit at a time, if at that point we were to delete the port from the DB we 
        would fail when we try to remove the port from the other FAPs. since we didn't want to support a mechansim that knows what was removed 
        from which FAP - nor did we want to create a DB per FAP at this point - we were left with the option to delete from the DB when
        dynamically adding a new port with the same system port - the assumption is that by this point that port was removed from all the FAPs
        in the system. */
        rv = appl_dnx_system_port_db_get(unit, sys_params->system_port_db, system_port_id, &old_sys_port_map);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_EXISTS);
        if (rv == _SHR_E_NONE)
        {
            /** remove entry from sys port DB */
            SHR_IF_ERR_EXIT(appl_dnx_system_port_db_remove
                            (unit, sys_params->system_port_db, system_port_id, &old_sys_port_map));
        }

        /** create system port and VOQs, and connect VOQs to VOQ connectors */
        SHR_IF_ERR_EXIT(appl_dnx_sys_ports_port_create_and_connect(unit, system_port_id, &sys_port_map));

        /** create scheduling scheme for the new port */
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_port_create(unit, system_port_id, &sys_port_map));
    }

    {
        /**  alloc port config on heap to overcome stack overflow since it's a huge struct */
        SHR_ALLOC(port_config, sizeof(bcm_port_config_t), "port config", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(bcm_port_config_get(unit, port_config));

        /** map port (if ethernet) to mirror recycle port */
        if (BCM_PBMP_MEMBER(port_config->e, port) && !BCM_PBMP_MEMBER(port_config->rcy_mirror, port))
        {
            SHR_IF_ERR_EXIT(appl_dnx_sniff_ports_recycle_port_map(unit, port));
        }

        /** add NIF port to linkscan */
        if (BCM_PBMP_MEMBER(port_config->nif, port))
        {
            SHR_IF_ERR_EXIT(bcm_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_SW));
        }

        /** set ingress compensation per port (used by crps and stif) */
        SHR_IF_ERR_EXIT(appl_dnx_compensation_port_delta_set(unit, port, APPL_DNX_REF_COMPENSATION_ING_PORT_DELTA));
    }

    /** configure visibility for the new port */
    SHR_IF_ERR_EXIT(appl_dnx_sys_vis_port_enable(unit, port));

exit:
    SHR_FREE(port_config);
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_dynamic_port_destroy(
    int unit,
    int port)
{
    bcm_gport_t system_port;
    int system_port_id;
    SHR_FUNC_INIT_VARS(unit);

    /** always assume port is local to this unit or symmetric port configuration in application */
    SHR_IF_ERR_EXIT(bcm_stk_gport_sysport_get(unit, port, &system_port));
    if (!BCM_GPORT_IS_SYSTEM_PORT(system_port))
    {
        /** exit with error, returned value is not a system port */
        /** this is mainly to make coverity happy */
        SHR_ERR_EXIT(_SHR_E_PARAM, "port %d is not mapped to a valid system port", port);
    }
    system_port_id = BCM_GPORT_SYSTEM_PORT_ID_GET(system_port);

    /** Destroy scheduling scheme for the port */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_port_destroy(unit, system_port_id, port));

    /** unmap from recycle mirror port */
    SHR_IF_ERR_EXIT(appl_dnx_sniff_ports_recycle_port_unmap(unit, port));

    /** remove ingress compensation per port (used by crps and stif) */
    SHR_IF_ERR_EXIT(appl_dnx_compensation_port_delta_set(unit, port, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set dynamic port eth initial settings for ETH ports
 *
 * \param [in] unit - Unit ID
 * \param [in] port - Port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnx_dyn_port_pp_eth_initial_settings_set(
    int unit,
    bcm_port_t port)
{
    uint32 flags;
    uint32 class_id;
    bcm_port_config_t *port_config = NULL;
    SHR_FUNC_INIT_VARS(unit);

    flags = BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD;
    class_id = port;

    /**  alloc port config on heap to overcome stack overflow since it's a huge struct */
    SHR_ALLOC(port_config, sizeof(bcm_port_config_t), "port config", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Set VLAN domain
     */
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, port, bcmPortClassId, class_id));
    /*
     * Set VLAN membership-namespace per port
     */
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, port, bcmPortClassVlanMember, class_id));
    /*
     * Add the port to BCM_VLAN_DEFAULT
     */
    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, BCM_VLAN_DEFAULT, port, 0));
    /*
     *  Enable Same interface Filter per port
     */
    SHR_IF_ERR_EXIT(bcm_port_control_set(unit, port, bcmPortControlBridge, TRUE));
    /*
     * Set untagged packet default VLAN per port
     */
    SHR_IF_ERR_EXIT(bcm_port_untagged_vlan_set(unit, port, BCM_VLAN_DEFAULT));
    /*
     * Enable learning
     */
    SHR_IF_ERR_EXIT(bcm_port_learn_set(unit, port, flags));

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, port_config));
    /*
     * Configure default stp state as FORWARD
     */
    if (BCM_PBMP_MEMBER(port_config->nif, port))
    {
        SHR_IF_ERR_EXIT(bcm_port_stp_set(unit, port, BCM_STG_STP_FORWARD));
    }

    /*
     * Set port default-LIF to simple bridge
     */
    SHR_IF_ERR_EXIT(bcm_vlan_control_port_set(unit, port, bcmVlanTranslateIngressMissDrop, 0));

exit:
    SHR_FREE(port_config);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_dyn_ports_packet_processing_eth_init(
    int unit)
{
    bcm_port_t port;
    bcm_port_config_t *port_config = NULL;
    bcm_pbmp_t pbmp_eth;

    SHR_FUNC_INIT_VARS(unit);

    /**  alloc port config on heap to overcome stack overflow since it's a huge struct */
    SHR_ALLOC(port_config, sizeof(bcm_port_config_t), "port config", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, port_config));
    BCM_PBMP_CLEAR(pbmp_eth);
    BCM_PBMP_OR(pbmp_eth, port_config->e);
    BCM_PBMP_REMOVE(pbmp_eth, port_config->rcy_mirror);
    /*
     * Take the ETH port
     */
    BCM_PBMP_ITER(pbmp_eth, port)
    {
        /*
         * Set port pp ETH initial settings
         */
        SHR_IF_ERR_EXIT(appl_dnx_dyn_port_pp_eth_initial_settings_set(unit, port));
    }

exit:
    SHR_FREE(port_config);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_dyn_port_add_rcy(
    int unit,
    uint32 port,
    uint32 core,
    int channel,
    int header_type_in,
    int header_type_out)
{
    uint32 flags;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    bcm_port_resource_t resource;
    int commit_changes;
    int is_remove;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * stage 1 - Add RYC port
     */
    flags = BCM_PORT_ADD_CONFIG_CHANNELIZED;

    sal_memset(&interface_info, 0, sizeof(bcm_port_interface_info_t));
    interface_info.phy_port = 1;
    interface_info.interface = BCM_PORT_IF_RCY;
    interface_info.num_lanes = 0;
    interface_info.interface_id = 0;

    bcm_port_mapping_info_t_init(&mapping_info);
    mapping_info.channel = channel;
    mapping_info.core = core;
    mapping_info.tm_port = port;
    mapping_info.pp_port = -1;
    mapping_info.num_priorities = 2;

    SHR_IF_ERR_EXIT(bcm_port_add(unit, port, flags, &interface_info, &mapping_info));

    /*
     * stage 2 - configure NIF properties
     */
    /*
     * Set resource
     */
    resource.speed = 10000;
    resource.port = port;
    resource.flags = 0;
    SHR_IF_ERR_EXIT(bcm_port_resource_set(unit, port, &resource));

    /*
     * stage 3 - configure egr and sch shapers
     */
    commit_changes = 1;
    is_remove = 0;
    /*
     * egr expects speed in Kbits/sec
     */
    SHR_IF_ERR_EXIT(dnx_tune_egr_ofp_rate_set(unit, port, resource.speed * 1000, commit_changes, is_remove));

    /*
     * sch expects speed in Kbits/sec
     */
    SHR_IF_ERR_EXIT(dnx_tune_scheduler_port_rates_set(unit, port, 0, resource.speed * 1000, commit_changes, is_remove));

    /*
     * stage 4 - PP properties
     */
    /*
     * Set incoming header type
     */
    key.type = bcmSwitchPortHeaderType;
    key.index = 1;      /* 0 = Both, 1 = direction IN, 2 = direction OUT */
    value.value = header_type_in;
    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_set(unit, port, key, value));

    /*
     * Set outgoing header type
     */
    key.type = bcmSwitchPortHeaderType;
    key.index = 2;      /* 0 = Both, 1 = direction IN, 2 = direction OUT */
    value.value = header_type_out;
    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_set(unit, port, key, value));

    /*
     * stage 5 - application reference
     * nothing to do as the port is added before the application handles ports
     */

exit:
    SHR_FUNC_EXIT;
}
