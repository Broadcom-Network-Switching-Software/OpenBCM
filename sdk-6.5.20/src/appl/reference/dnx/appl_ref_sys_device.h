/** \file appl_ref_sys_device.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_SYS_DEVICE_H_INCLUDED
/*
 * {
 */
#define APPL_REF_SYS_DEVICE_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <shared/error.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <soc/defs.h>
#include "appl_ref_sys_db.h"

/*
 * }
 */

/*
 * Typedefs
 * {
 */

/*
 * DEFINEs
 * {
 */

/**
 * \brief - Each device represents to modid's (cores)
 */
#define APPL_DNX_NOF_MODIDS_PER_DEVICE(sys_params)  ((sys_params)->nof_modids_per_device)
#define APPL_DNX_NOF_REMOTE_CORES 2
#define APPL_DNX_NOF_PORTS_PER_MODID 256
/** changed value to hard coded 1024 - this should be suffice to accommodate all of our MGMT
 * system type until a better solution is implemented that is not so wasteful. systems with
 * multiple CPU agents have to define system ports using appl_param soc properties. */
#define APPL_DNX_NOF_SYSPORTS_PER_DEVICE 1024
#define APPL_DNX_NOF_MODIDS 2048

#define APPL_DNX_MAX_NOF_MODID_BASES_PER_DEVICE 4

/**
 * List of dynamic port flags (API 'bcm_port_add()') that represent special filtered out ports from application
 */
#define APPL_DNX_FILTERED_PORTS_FLAGS (BCM_PORT_ADD_KBP_PORT | BCM_PORT_ADD_STIF_PORT |                             \
                                       BCM_PORT_ADD_STIF_AND_DATA_PORT | BCM_PORT_ADD_FLEXE_PHY |                   \
                                       BCM_PORT_ADD_CROSS_CONNECT)
/*
 * }
 */

/** mapping info of modid */
typedef struct
{
    /** device modid is connected to, this is an index to devices_array array */
    int device_index;
    /** base modid index in the device, this is an index to base_modids array */
    int base_modid_index;
} appl_dnx_modid_map_t;

/** this struct contains system info about a device */
typedef struct
{
    /** device_type is provided using appl_param_device_type_X soc property */
    uint32 device_type;
    /** derived from device type */
    /** { */
    int nof_modids;
    int nof_modids_per_core;
    int nof_cores;
    /** } */
    /**
     * this is the base modids used for each modid per core, provided by soc property "appl_param_device_1_modid_2_map = 0x100"
     * modids for other cores in this device will be consecutive to the relevant base modids.
     * meaning: modid(core, modid_index_in_core) = base_modid(modid_index_in_core) + core
     */
    int base_modids[APPL_DNX_MAX_NOF_MODID_BASES_PER_DEVICE];
} appl_dnx_device_info_t;

/**
 * \brief dnx system device parameters,
 * this structure exists for each unit in the system
 */
typedef struct
{
    int nof_devices;

    /**
     * each device in the system is init as if it is the only device in the system.
     * This is used for systems where mgmt is used - however each device is its own TM Domain
     */
    int stand_alone_configuration;

    /** array containing information on each device in the system (both on local and remote CPU agents) */
    appl_dnx_device_info_t *devices_array;

    /** device index for each unit on this CPU agent */
    int unit_to_device_index[SOC_MAX_NUM_DEVICES];

    /** used to verify validity of modids provided in a quick manner. invalid entries have 0xffffffff as device_index */
    appl_dnx_modid_map_t modid_to_index[APPL_DNX_NOF_MODIDS];

    /** pointer to DB containing mapping info on each system port in the system. */
    /** this DB is not valid when using stand_alone_configuration */
    system_port_db_t *system_port_db;
} appl_dnx_sys_device_t;

/*
 * }
 */

/**
 * \brief - Configure general device level attributes:
 * 1. Confiugre my_modid.
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
shr_error_e appl_dnx_sys_device_init(
    int unit);

/**
 * \brief - Return pointer to the struct that represents system device parameters
 *
 * \param [in] unit - Unit ID
 * \param [out] sys_params - Pointer to the device system parameters
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_sys_device_params_get(
    int unit,
    appl_dnx_sys_device_t ** sys_params);

/**
 * \brief - Return a bitmap of all confiugred ports
 *
 * \param [in] unit - Unit ID
 * \param [in] pbmp - bitmap of ports
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_sys_device_ports_get(
    int unit,
    bcm_pbmp_t * pbmp);

/**
 * \brief - Parse and verify system parameters
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * Note that this step is skipped in WB in order to not free the already allocated sys_params strcuture.
 * \see
 *   * None
 */
shr_error_e appl_dnx_sys_params_parse(
    int unit);

/**
 * \brief - free system parameters
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
shr_error_e appl_dnx_sys_params_free(
    int unit);

/**
 * \brief - Enable traffic (should be the last applications):
 * 1. Enable traffic within the fap
 * 2. Enable control cells
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
shr_error_e appl_dnx_sys_traffic_enable(
    int unit);

/**
 * \brief - Configure visibility for the port
 *
 * 1. Enable a port
 * 2. Force a port
 *
 * \param [in] unit - Unit ID
 * \param [in] logical_port - port ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_sys_vis_port_enable(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief - Configure visibility
 * 1. Enable all ports besides OLP
 * 2. Force all ports besides OLP
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
shr_error_e appl_dnx_sys_vis_enable(
    int unit);

/**
 * \brief - Init Done step, used to update relevant modules that
 *          the application init was done.
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
shr_error_e appl_dnx_init_done_init(
    int unit);

/**
 * \brief - Deinit Done step, used to update relevant modules that
 *          the application deinit was done.
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
shr_error_e appl_dnx_init_done_deinit(
    int unit);

/**
 * \brief - Convert FAP index + TM port to
 * modid
 */
shr_error_e appl_dnx_fap_and_tm_port_to_modid_convert(
    int unit,
    int fap_index,
    int core_id,
    int tm_port,
    int *modid);
/**
 * \brief - Convert FAP index + TM port to
 * modid
 */
shr_error_e appl_dnx_modid_to_fap_and_core_convert(
    int unit,
    int modid,
    int *fap_index,
    int *core_id);

/**
 * \brief - Convert FAP index + TM port to
 * modid + ftmh_pp_dsp
 */
shr_error_e appl_dnx_fap_and_tm_port_to_modport_convert(
    int unit,
    int fap_index,
    int core_id,
    int tm_port,
    int *modid,
    int *ftmh_pp_dsp);

/*
 * }
 */
#endif /* APPL_REF_SYS_DEVICE_H_INCLUDED */
