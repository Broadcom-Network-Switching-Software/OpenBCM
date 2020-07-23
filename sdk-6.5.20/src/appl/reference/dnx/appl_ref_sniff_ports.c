/** \file appl_ref_sys_ports.c
 * 
 *
 * System ports level application procedures for DNX. 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/property.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/mirror.h>
#include <bcm/port.h>
#include "appl_ref_sys_device.h"
#include <appl/reference/dnx/appl_ref_sniff_ports.h>

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
appl_dnx_sniff_ports_init(
    int unit)
{
    bcm_port_config_t *port_config = NULL;
    bcm_pbmp_t ether_ports;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    /**  alloc port config on heap to overcome stack overflow since it's a huge struct */
    SHR_ALLOC(port_config, sizeof(bcm_port_config_t), "port config", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** get ports */
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, port_config));

    /** construct ethernet ports bitmap */
    BCM_PBMP_ASSIGN(ether_ports, port_config->e);
    BCM_PBMP_REMOVE(ether_ports, port_config->rcy_mirror);

    /** map each ethernet port to mirror recycle port */
    BCM_PBMP_ITER(ether_ports, port)
    {
        SHR_IF_ERR_EXIT(appl_dnx_sniff_ports_recycle_port_map(unit, port));
    }

exit:
    SHR_FREE(port_config);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Find an unused mirror recycle port. If no free port found return BCM_PORT_INVALID.
 */
static shr_error_e
appl_dnx_sniff_ports_free_rcy_port_get(
    int unit,
    int core,
    bcm_port_t * mirror_port)
{
    bcm_pbmp_t used_rcy_ports, free_rcy_ports, all_ports;
    bcm_port_t port;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info;
    bcm_port_config_t *port_config = NULL;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    /**  alloc port config on heap to overcome stack overflow since it's a huge struct */
    SHR_ALLOC(port_config, sizeof(bcm_port_config_t), "port config", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    *mirror_port = BCM_PORT_INVALID;

    /** get all ports */
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, port_config));

    /** construct bitmap of used mirror recycle ports */
    {
        BCM_PBMP_CLEAR(used_rcy_ports);
        BCM_PBMP_ASSIGN(all_ports, port_config->all);
        /** remove rcy_mirror ports from all_ports bitmap since there is no need to iterate over them */
        BCM_PBMP_REMOVE(all_ports, port_config->rcy_mirror);
        BCM_PBMP_ITER(all_ports, port)
        {
            /** get the mapped recycle mirror port (if exists such) */
            SHR_IF_ERR_EXIT(bcm_mirror_port_to_rcy_port_map_get(unit, 0, port, &rcy_map_info));
            if (rcy_map_info.rcy_mirror_port != BCM_PORT_INVALID)
            {
                BCM_PBMP_PORT_ADD(used_rcy_ports, rcy_map_info.rcy_mirror_port);
            }
        }
    }

    /** find free mirror recycle port on the required core */
    {
        BCM_PBMP_ASSIGN(free_rcy_ports, port_config->rcy_mirror);
        /** construct unused recycle mirror ports bitmap (remove used ports) */
        BCM_PBMP_REMOVE(free_rcy_ports, used_rcy_ports);

        BCM_PBMP_ITER(free_rcy_ports, port)
        {
            /** Check core */
            SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info));
            if (core != mapping_info.core)
            {
                continue;
            }
            else
            {
                *mirror_port = port;
                break;
            }
        }
    }

exit:
    SHR_FREE(port_config);
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_sniff_ports_recycle_port_map(
    int unit,
    bcm_port_t port)
{
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_port_t mirror_port;
    int core;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    /** verify port isn't already mapped to a mirror recycle port */
    SHR_IF_ERR_EXIT(bcm_mirror_port_to_rcy_port_map_get(unit, 0, port, &rcy_map_info));
    if (rcy_map_info.rcy_mirror_port != BCM_PORT_INVALID)
    {
        /** exit if recycle mirror port already exists */
        SHR_EXIT();
    }

    /** verify port has egress TM attributes (not all ethenet ports are egress ports, ie stif_data port) */
    SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info));
    if (flags & BCM_PORT_ADD_STIF_AND_DATA_PORT)
    {
        /** exit for non egress ports */
        SHR_EXIT();
    }
    core = mapping_info.core;

    /** get free mirror recycle port */
    SHR_IF_ERR_EXIT(appl_dnx_sniff_ports_free_rcy_port_get(unit, core, &mirror_port));

    /** map forward port to the recycle mirror port */
    if (mirror_port != BCM_PORT_INVALID)
    {
        rcy_map_info.priority_bitmap = BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_NORMAL |
            BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_HIGH;

        rcy_map_info.rcy_mirror_port = mirror_port;

        SHR_IF_ERR_EXIT(bcm_mirror_port_to_rcy_port_map_set(unit, 0, port, &rcy_map_info));
    }
    else
    {
        LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "No free mirror recycle ports are left for port:%d \n"), port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_sniff_ports_recycle_port_unmap(
    int unit,
    bcm_port_t port)
{
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info;

    SHR_FUNC_INIT_VARS(unit);

    /** unmap forward port in case it's already mapped */
    SHR_IF_ERR_EXIT(bcm_mirror_port_to_rcy_port_map_get(unit, 0, port, &rcy_map_info));
    if (rcy_map_info.rcy_mirror_port != BCM_PORT_INVALID)
    {
        rcy_map_info.priority_bitmap = 0;

        rcy_map_info.rcy_mirror_port = BCM_PORT_INVALID;

        SHR_IF_ERR_EXIT(bcm_mirror_port_to_rcy_port_map_set(unit, 0, port, &rcy_map_info));
    }

exit:
    SHR_FUNC_EXIT;
}
