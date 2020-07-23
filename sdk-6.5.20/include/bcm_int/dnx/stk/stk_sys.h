/** \file stk/stk_sys.h
 *
 * Switch system procedures.
 *
 * FAP_ID, System Ports, Module Ports ...
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DNX_STK_SYS_H_INCLUDED
/*
 * {
 */
#define DNX_STK_SYS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/stack.h>

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>

/*
 * }
 */

/**
 * \brief - Set dedicated FMQ system port. The system port should be mapped to local fap modid since enhanced FMQ
 * credits are recieved from local fap.
 * \param [in] unit - Unit #
 * \param [in] core - Core #
 * \param [in] my_modid - Local modid.
 */
shr_error_e dnx_stk_sys_fmq_sysport_set(
    int unit,
    bcm_core_t core,
    uint32 my_modid);

/**
 * \brief
 * Set core modid (FAP ID).
 * Setting modid must be the first step after 'bcm_init()' in user application.
 *
 * \param [in] unit - Unit #
 * \param [in] core - Core #
 * \param [in] fap_id_index - Index of FAP ID inside the core
 * \param [out] modid - modid.
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_modid_add(
    int unit,
    bcm_core_t core,
    int fap_id_index,
    uint32 modid);

/**
 * \brief
 * Get core modid (FAP_ID)
 *
 * \param [in] unit - Unit #
 * \param [in] core - Core #
 * \param [in] fap_id_index - FAP ID index inside the core #
 * \param [out] modid - module id
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_modid_get(
    int unit,
    bcm_core_t core,
    int fap_id_index,
    uint32 *modid);

/**
 * \brief
 * Get all FAP IDs of specific core
 *
 * \param [in] unit - Unit #
 * \param [in] core - core ID #
 * \param [out] modid - array of modid(s)
 * \param [out] modid_count - number of returned modid(s)
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * modid array should be at least of size dnx_data_device.general.max_nof_fap_ids_per_core_get(unit)
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_modid_get_all(
    int unit,
    bcm_core_t core,
    uint32 *modid,
    int *modid_count);

/**
 * \brief
 * Get the core set to modid, or -1 if modid is not local.
 *
 * \param [in] unit - Unit #
 * \param [in] modid - required modid #
 * \param [out] core - local core # or -1
 * \param [out] fap_id_index - index of modid inside the core
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_modid_local_core_get(
    int unit,
    uint32 modid,
    bcm_core_t * core,
    int *fap_id_index);

/**
 * \brief
 * Return TRUE iff modid is local
 *
 * \param [in] unit - Unit #
 * \param [in] modid - required modid #
 * \param [out] is_local - is modid local
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_modid_is_local_get(
    int unit,
    uint32 modid,
    int *is_local);

/**
 * \brief
 * Convert core and TM port to modport gport
 *
 * \param [in] unit - Unit #
 * \param [in] core_id - local core #
 * \param [in] tm_port - TM port #
 * \param [out] modid - modid #
 * \param [out] ftmh_pp_dsp - FTMH PP DSP #
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_tm_port_to_modport_convert(
    int unit,
    bcm_core_t core_id,
    int tm_port,
    int *modid,
    int *ftmh_pp_dsp);

/**
 * \brief
 * Convert core and TM port to modport gport
 *
 * \param [in] unit - Unit #
 * \param [in] core_id - local core #
 * \param [in] tm_port - TM port #
 * \param [out] modport_gport - modport gport #
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_tm_port_to_modport_gport_convert(
    int unit,
    bcm_core_t core_id,
    int tm_port,
    bcm_gport_t * modport_gport);
/**
 * \brief
 * Convert modport gport to core and TM port
 *
 * \param [in] unit - Unit #
 * \param [in] modport_gport - modport gport #
 * \param [out] core_id - local core #
 * \param [out] tm_port - TM port #
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_modport_gport_to_tm_port_convert(
    int unit,
    bcm_gport_t modport_gport,
    bcm_core_t * core_id,
    int *tm_port);

/**
 * \brief
 * Convert modport gport to core and TM port
 *
 * \param [in] unit - Unit #
 * \param [in] modid - destination FAP
 * \param [in] ftmh_pp_dsp - FTMH PP DSP
 * \param [out] core_id - local core #
 * \param [out] tm_port - TM port #
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_modport_to_tm_port_convert(
    int unit,
    int modid,
    int ftmh_pp_dsp,
    bcm_core_t * core_id,
    int *tm_port);

/**
 * \brief
 * Map system port to module port.
 * This API configures mapping relevant for local ports and mapping relevant for remote ports.
 * This API configures both TM and PP mapping.
 *
 * \param [in] unit - Unit #
 * \param [in] core - Core #
 * \param [in] sysport - system port (not system gport)
 * \param [in] modid - destination FAP
 * \param [in] tm_port - destination tm port
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_sysport_modport_map_set(
    int unit,
    int core,
    uint32 sysport,
    uint32 modid,
    uint32 tm_port);

/**
 * \brief
 * Get module port given system port.
 *
 * \param [in] unit - Unit #
 * \param [in] sysport - system port (not system gport)
 * \param [out] modid - destination FAP
 * \param [out] tm_port - destination tm port
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_sysport_modport_map_get(
    int unit,
    uint32 sysport,
    uint32 *modid,
    uint32 *tm_port);
/**
 * \brief
 * Map logical port to systemport.
 *
 * \param [in] unit - Unit #
 * \param [in] logical_port - logical_port #
 * \param [in] sysport - system port (not system gport)
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_logical_port_to_sysport_map_set(
    int unit,
    bcm_port_t logical_port,
    uint32 sysport);
/**
 * \brief
 * Get system port given a logical port.
 *
 * \param [in] unit - Unit #
 * \param [in] logical_port - logical_port #
 * \param [out] sysport - system port (not system gport)
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_logical_port_to_sysport_map_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *sysport);
/**
 * \brief
 * Clear all local tm ports that mapped to system port.
 *
 * \param [in] unit - Unit #
 * \param [in] sysport - system port (not system gport)
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_sysport_map_clear(
    int unit,
    uint32 sysport);

/**
 * \brief - Disable device traffic during init sequence (Stop receiving and transmitting traffic).
 * This function is used as a wrapper for init sequence since init sequence invokes functions only with
 * 'unit' argument.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  This function should be invoked during init sequence as one of the first steps (before HW is accessed).
 *  Meaning that this function should be invoked right after access is initiated.
 *  The reason to disable traffic before any access is because some HW configurations must not be done during traffic,
 *  so it's expected from the application to re enable traffic after it's done with all the required configurations.
 *
 * \see
 *   * dnx_stk_sys_traffic_enable()
 */
shr_error_e dnx_stk_sys_traffic_disable(
    int unit);

/**
 * \brief - Enable/Disable device traffic. Stop/Start receiving and transmitting traffic.
 *
 * \param [in] unit - Unit ID
 * \param [in] enable - 0 means disable traffic, 1 means enable
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  Traffic should be disabled before changing HW configuration that should not be changed during traffic.
 *  After the changes are done, the user should re enable traffic.
 *
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_traffic_enable_set(
    int unit,
    int enable);

/**
 * \brief - Get whether device traffic is Enabled/Disabled.
 *
 * \param [in] unit - Unit ID
 * \param [out] enable - 0 means traffic disabled, 1 means enabled
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   * dnx_stk_sys_traffic_enable_set
 */
shr_error_e dnx_stk_sys_traffic_enable_get(
    int unit,
    int *enable);

/**
 * \brief
 *   Exclude/Include modid from all-reachable vector.
 *   Up to 16 modids can be excluded.
 *   There are 2 ways to exclude modids from all-reachable vector:
 *     1. Set maximum modid to be included in the vector. All modids
 *        in the range above max won't be included in the vector.
 *     2. Exclude specific modids from the vector.
 *
 *   This function implements option 2 .
 * \param [in] unit -
 *   The unit number.
 * \param [in] module -
 *   The module id to be excluded/included.
 * \param [in] enable -
 *   - True  - Exclude modid from all-reachable vector.
 *   - False - Include modid on all-reachable vector.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * dnx_stk_sys_module_max_all_reachable_set.
 */
shr_error_e dnx_stk_sys_module_all_reachable_ignore_id_set(
    int unit,
    bcm_module_t module,
    int enable);

/**
 * \brief
 *   Get whether a modid is excluded from all-reachable vector.
 * \param [in] unit -
 *   The unit number.
 * \param [in] module -
 *   The module id in question.
 * \param [out] enable -
 *   True  - Modid is excluded from all-reachable vector.
 *   False - Modid is included in all-reachable vector.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * dnx_stk_sys_module_all_reachable_ignore_id_set.
 */
shr_error_e dnx_stk_sys_module_all_reachable_ignore_id_get(
    int unit,
    bcm_module_t module,
    int *enable);

/**
 * \brief
 *   Set maximum modid to be included in all-reachable vector.
 *   There are 2 ways to exclude modids from all-reachable vector:
 *     1. Set maximum modid to be included in the vector. All modids
 *        in the range above max won't be included in the vector.
 *     2. Exclude specific modids from the vector.
 *
 *   This function implements option 1 .
 * \param [in] unit -
 *   The unit number.
 * \param [in] max_module -
 *   The maximum modid to be included in the all-reachable vector.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Max module for all-reachable vector must be <= max module in the system.
 * \see
 *   * dnx_stk_sys_module_all_reachable_ignore_id_set.
 */
shr_error_e dnx_stk_sys_module_max_all_reachable_set(
    int unit,
    bcm_module_t max_module);

/**
 * \brief
 *   Get maximum modid to be included in all-reachable vector.
 * \param [in] unit -
 *   The unit number.
 * \param [out] max_module -
 *   The maximum modid included in the all-reachable vector.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_stk_sys_module_max_all_reachable_set.
 */
shr_error_e dnx_stk_sys_module_max_all_reachable_get(
    int unit,
    bcm_module_t * max_module);

/**
 * \brief
 *   Set maximum allowed modid in the system.
 * \param [in] unit -
 *   The unit number.
 * \param [in] max_module -
 *   The maximum modid to be configured.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Max modid in the system must be >= max modid in all-reachable vector.
 *     If max_module is set to be lower than max modid in all-reachable vector,
 *     then max modid in all-reachable vector will be update to be the same as
 *     max modid in the system, and a warning will be printed to the user.
 * \see
 *   * dnx_stk_sys_module_max_all_reachable_set.
 */
shr_error_e dnx_stk_sys_module_max_set(
    int unit,
    bcm_module_t max_module);

/**
 * \brief
 *   Get maximum allowed modid in the system.
 * \param [in] unit -
 *   The unit number.
 * \param [out] max_module -
 *   The maximum modid allowed in the system.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_stk_sys_module_max_set.
 */
shr_error_e dnx_stk_sys_module_max_get(
    int unit,
    bcm_module_t * max_module);

/**
 * \brief
 *   Initalize Stk Sys module
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_init(
    int unit);

/**
 * \brief
 *   Map Fap ID to DQCQ context in mesh mode
 * \param [in] unit -
 *   The unit number.
 * \param [in] fap_id_lsb -
 *   LSBs of FAP ID. Used as a key to the table
 * \param [in] dqcq_context -
 *   DQCQ context to be mapped
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_mesh_dqcq_map_set(
    int unit,
    int fap_id_lsb,
    dbal_enum_value_field_system_mesh_context_e dqcq_context);

/**
 * \brief
 *   Map Fap ID to DQCQ context in clos mode
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   core ID
 * \param [in] local_indx -
 *   local index. Used as a key to the table
 * \param [in]  modid -
 *  modid for local index
 * \param [in]  mask -
 *  mask for local index
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_clos_dqcq_map_set(
    int unit,
    bcm_core_t core,
    int local_indx,
    uint32 modid,
    uint32 mask);

/**
 * \brief
 *   Get actual number of FAP IDs per core
 * \param [in] unit -
 *   The unit number.
 * \param [out] nof_fap_ids_per_core -
 *   number of FAP IDs per core
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_stk_sys_nof_fap_ids_per_core_get(
    int unit,
    int *nof_fap_ids_per_core);

#endif /* DNX_STK_SYS_H_INCLUDED */
