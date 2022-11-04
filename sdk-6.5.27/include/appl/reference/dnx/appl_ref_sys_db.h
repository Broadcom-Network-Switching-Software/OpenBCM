/** \file appl_ref_sys_db.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef INCLUDE_APPL_REF_SYS_DB_H_INCLUDED
/** { */
#define INCLUDE_APPL_REF_SYS_DB_H_INCLUDED

/** Include files. */
/** { */
#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
/** } */

/**
 * \brief - add a new system port to the DB.
 *
 * \param [in] unit - Unit ID
 * \param [in] system_port_id - index of system port to add to DB
 * \param [in] sys_port_map_info - mapping information of the system port to add
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *  None
 * \see
 *   * None
 */
shr_error_e appl_dnx_system_port_db_add(
    int unit,
    int system_port_id,
    system_port_mapping_t * sys_port_map_info);

/**
 * \brief - remove a system port from DB and return its mapping info.
 *
 * \param [in] unit - Unit ID
 * \param [in] system_port_id - index of system port to remove from DB
 * \param [out] sys_port_map_info - mapping information of the system port that was removed
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *  None
 * \see
 *   * None
 */
shr_error_e appl_dnx_system_port_db_remove(
    int unit,
    int system_port_id,
    system_port_mapping_t * sys_port_map_info);

/**
 * \brief - get a system port map info from DB.
 *
 * \param [in] unit - Unit ID
 * \param [in] system_port_id - index of system port to get from DB
 * \param [out] sys_port_map_info - mapping information of the system port
 *
 * \return
 *   _SHR_E_NONE - on success;
 *   _SHR_E_EXISTS - if system port id is not in DB
 *
 * \remark
 *   *  returns error if system port doesn't exists in the DB
 * \see
 *   * None
 */
shr_error_e appl_dnx_system_port_db_get(
    int unit,
    int system_port_id,
    system_port_mapping_t * sys_port_map_info);


/** } */
#endif /* INCLUDE_APPL_REF_SYS_DB_H_INCLUDED */
