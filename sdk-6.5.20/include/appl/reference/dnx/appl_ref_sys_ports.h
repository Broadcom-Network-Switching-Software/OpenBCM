/** \file appl_ref_sys_ports.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_SYS_PORTS_H_INCLUDED
/*
 * {
 */
#define APPL_REF_SYS_PORTS_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

/*
 * Typedefs
 * {
 */

typedef enum
{
    APPL_PORT_TYPE_FIRST = 0,
    APPL_PORT_TYPE_NONE = APPL_PORT_TYPE_FIRST,
    APPL_PORT_TYPE_TDM,
    APPL_PORT_TYPE_OLP,
    APPL_PORT_TYPE_NOF_TYPES,
    APPL_PORT_TYPE_INVALID = APPL_PORT_TYPE_NOF_TYPES
} appl_port_type_t;

/** this struct contains mapping info of a system port. */
typedef struct
{
    int device_index;
    bcm_port_t local_port;
    bcm_core_t core;
    bcm_port_t tm_port;
    int rate;
    appl_port_type_t port_type;
    int system_port_id;
} system_port_mapping_t;

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** system port ID allocation is done implicitly according to a formula : device_idx * nof_sys_ports_per_device + local_port */
#define APPL_DNX_SYS_PORT_ID_IMPLICIT_ALLOC_FLAG 0x1

/*
 * }
 */

/**
 * \brief - system ports application. Create system ports for all ports in the system:
 * 1. Each port in the system should get a uniaque ID (system port) 
 * 2. Map the ID to the physical port (modid, pp_dsp)
 * 
 * \param [in] unit - Unit ID
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
appl_dnx_sys_ports_init(
    int unit);

/**
 * \brief - Create a system port for a given port, create its VOQs and connect them to VOQ connector
 *
 * \param [in] unit - Unit ID
 * \param [in] system_port_id - used in case system port id is allocated explicitly
 * \param [in] sys_port_map - system port mapping info
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
appl_dnx_sys_ports_port_create_and_connect(
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map);

/**
 * \brief - Create a system port for a given port
 * 
 * \param [in] unit - Unit ID
 * \param [in] system_port_id - used in case system port id is allocated explicitly
 * \param [in] sys_port_map - system port mapping
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
appl_dnx_sys_ports_port_create(
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map);

/**
 * \brief - create VOQs for given system port
 *
 * \param [in] unit - Unit ID
 * \param [in] system_port_id - system port ID
 * \param [in] sys_port_map - system port mapping info
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
appl_dnx_sys_port_voq_create(
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map);

/**
 * \brief - destroy VOQs for given system port
 *
 * \param [in] unit - Unit ID
 * \param [in] system_port_id - system port ID
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
appl_dnx_sys_ports_voq_destroy(
    int unit,
    int system_port_id);

/**
 * \brief - Convert modid andlogical port to system port id.
 *
 * \param [in] unit - Unit ID
 * \param [in] modid - module id
 * \param [in] port - logical port
 * \param [Out] sysport - Returned system port index
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
appl_dnx_logical_port_to_sysport_get(
    int unit,
    bcm_module_t modid,
    bcm_port_t port,
    int *sysport);


/*
 * }
 */
#endif /* APPL_REF_SYS_PORTS_H_INCLUDED */
