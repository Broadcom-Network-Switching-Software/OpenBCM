/** \file appl_ref_sys_db.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_SYS_DB_H_INCLUDED
/** { */
#define APPL_REF_SYS_DB_H_INCLUDED

/** Include files. */
/** { */
#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
/** } */

/** CB prototype for iterator */
typedef shr_error_e(
    *appl_dnx_system_port_db_iter_cb) (
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map_info,
    void *opaque_data_pointer);

/** opaque declaration of system port db to allow usage as pointers from other places */
/** definition is private and found in c file. */
typedef struct system_port_db_t system_port_db_t;

/**
 * \brief - Create (allocate) a system port DB and init to 0
 *
 * \param [in] unit - Unit ID
 * \param [out] system_port_db_ptr - pointer to pointer pointing to allocated DB
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_system_port_db_create(
    int unit,
    system_port_db_t ** system_port_db_ptr);

/**
 * \brief - destroy (free) a system port DB
 *
 * \param [in] unit - Unit ID
 * \param [in] system_port_db_ptr - pointer to pointer pointing to allocated DB
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnx_system_port_db_destroy(
    int unit,
    system_port_db_t ** system_port_db_ptr);

/**
 * \brief - add a new system port to the DB.
 *
 * \param [in] unit - Unit ID
 * \param [in] system_port_db - pointer to DB
 * \param [in] system_port_id - index of system port to add to DB
 * \param [in] sys_port_map_info - mapping information of the system port to add
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *  each new member is added to the array of system ports.
 *      each new member is the new HEAD of the linked list maintained for
 *      quick iterate over the array (usually this array is pretty sparse)
 * \see
 *   * None
 */
shr_error_e appl_dnx_system_port_db_add(
    int unit,
    system_port_db_t * system_port_db,
    int system_port_id,
    system_port_mapping_t * sys_port_map_info);

/**
 * \brief - remove a system port from DB and return its mapping info.
 *
 * \param [in] unit - Unit ID
 * \param [in] system_port_db - pointer to DB
 * \param [in] system_port_id - index of system port to remove from DB
 * \param [out] sys_port_map_info - mapping information of the system port that was removed
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *  returns error if system port doesn't exists in the DB
 * \see
 *   * None
 */
shr_error_e appl_dnx_system_port_db_remove(
    int unit,
    system_port_db_t * system_port_db,
    int system_port_id,
    system_port_mapping_t * sys_port_map_info);

/**
 * \brief - get a system port map info from DB.
 *
 * \param [in] unit - Unit ID
 * \param [in] system_port_db - pointer to DB
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
    system_port_db_t * system_port_db,
    int system_port_id,
    system_port_mapping_t * sys_port_map_info);

/**
 * \brief - get multiple system port mappings info from DB. according to matching criteria
 *
 * \param [in]  unit -                  Unit ID
 * \param [in]  system_port_db -        pointer to DB
 * \param [in]  matching_criteria -     matching criteria according to which to get port mappings
 * \param [in]  max_nof_ports -         size of array of returned mapping
 * \param [out] sys_port_map_array -    pointer to array to return result
 * \param [out] nof_ports -             number of ports matching the criteria
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *  currently only search criteria supported is port_type. in general, support of any other
 *      match criteria can be added. to skip field for match criteria, use the appropriate INVALID
 *      value for that field:
 *          port_type - APPL_PORT_TYPE_INVALID
 *
 * \see
 *   * appl_dnx_system_port_db_port_mapping_is_match - for adding matching criteria
 */
shr_error_e appl_dnx_system_port_db_get_multiple(
    int unit,
    system_port_db_t * system_port_db,
    const system_port_mapping_t * matching_criteria,
    int max_nof_ports,
    system_port_mapping_t * sys_port_map_array,
    int *nof_ports);

/**
 * \brief - iterate over all system ports and run callback.
 *
 * \param [in] unit - Unit ID
 * \param [in] system_port_db - pointer to DB
 * \param [in] callback_function - callback function to run on each system port
 * \param [in] opaque_data_pointer - opaque pointer to pass additional args to callback
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *  none
 * \see
 *   * appl_dnx_system_port_db_iter_cb
 */
shr_error_e appl_dnx_system_port_db_iterate(
    int unit,
    system_port_db_t * system_port_db,
    appl_dnx_system_port_db_iter_cb callback_function,
    void *opaque_data_pointer);

/**
 * \brief - populate db with entries based on sys port appl soc properties.
 *
 * \param [in] unit - Unit ID
 * \param [in] system_port_db - pointer to DB
 * \param [in] nof_devices - number of devices in the system, used to verify input
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *  none
 * \see
 *   *  none
 */

shr_error_e appl_dnx_system_port_db_init(
    int unit,
    system_port_db_t * system_port_db,
    int nof_devices);

/** } */
#endif /* APPL_REF_SYS_DB_H_INCLUDED */
