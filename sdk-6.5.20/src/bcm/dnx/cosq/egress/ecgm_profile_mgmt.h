/** \file src/bcm/dnx/cosq/egress/ecgm_profile_mgmt.h
* 
*
* internal API for Egress Congestion resources management.
*
* ecgm resources thresholds per port are kept in port profiles and interface profiles.
* this file holds all the functionality for profile management, using the structs:
* dnx_ecgm_profile_mgmt_port_handle_t
* and
* dnx_ecgm_profile_mgmt_interface_handle_t
* defined in this file, which hold the requiered information to access the template manager
* and the profile related dbal functions.
*
* for each of these types:
* init function must be called on port add.
* get function must be called before every other usage of the type.
* on port remove - the destroy function must be called.
* for more information - see the function description.
*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _EGRESS_CONGESTION_PROFILE_MGMT_H_INCLUDED_
/** { */
#define _EGRESS_CONGESTION_PROFILE_MGMT_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/** Included files. { */
#include <bcm/types.h>  /* uint32 */
#include <bcm_int/dnx/algo/ecgm/algo_ecgm.h>    /* ecgm port & interface profiles */
#include "ecgm_thresholds_internal.h"
/** } */

/** Typedefs { */
typedef struct
{
    bcm_port_t port;
    bcm_core_t core;
    uint32 tm_port;
    int base_q_pair;
    int profile_id;
    int num_priorities;
    dnx_ecgm_port_profile_info_t info;
} dnx_ecgm_profile_mgmt_port_handle_t;

typedef struct
{
    bcm_port_t port;
    bcm_core_t core;
    int egq_if;
    int profile_id;
    dnx_ecgm_interface_profile_info_t info;
} dnx_ecgm_profile_mgmt_interface_handle_t;
/** } */

/** Port Profile Functionality { */

/**
* \brief - Get port handle.
*          sets the handle variable with all the
*          profile requiered data.
*          NOTE that a variable of type dnx_ecgm_profile_mgmt_port_handle_t
*          is not initialized to use before calling this function.
*
* \param [in] unit - device unit number.
* \param [in] port - port number.
* \param [out] port_profile_handle - a handle to be
*                   used for this modules functionality.
*
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_profile_mgmt_port_handle_get(
    int unit,
    bcm_port_t port,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle);

/**
* \brief - Destroy profile.
*          cleans up any SW and HW data if needed.
*          MUST be called during port removal with an
*          initialized variable.
*
* \param [in] unit - device unit number.
* \param [out] port_profile_handle - a handle to be
*                   used for this modules functionality.
*
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_profile_mgmt_port_handle_destroy(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle);

/**
* \brief - Set packet descriptors service pool for a port's multicast queue.
*          if a new SP is given - the function will modify the total reserved
*          PDs for the old and the new pool, according to the queue reserves.
*          NOTE that in order to change service pools - the queue must be empty,
*          thus - the function will FLUSH AND DISABLE the port temporarily.
*
* \param [in] unit - device unit number.
* \param [in] port_profile_handle - initialized handle.
*                                   \see dnx_ecgm_profile_mgmt_port_handle_get
* \param [in] queue - queue number to set.
* \param [in] pd_sp - requiered service pool.
*
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_profile_mgmt_pd_sp_set(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    bcm_cos_queue_t queue,
    int pd_sp);

/**
* \brief - Get packet descriptors service pool for a port's multicast queue.
*
* \param [in] unit - device unit number.
* \param [in] port_profile_handle - initialized handle.
*                                   \see dnx_ecgm_profile_mgmt_port_handle_get
* \param [in] queue - queue number.
* \param [out] pd_sp - mapped service pool.
*
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_profile_mgmt_pd_sp_get(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    bcm_cos_queue_t queue,
    int *pd_sp);

/**
* \brief - Set port unicast thresholds.
*
* \param [in] unit - device unit number.
* \param [in] port - port number.
* \param [in] data - struct containig the necessary data for threshold.
*                    \see ecgm_thresholds_internal.h
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_profile_mgmt_unicast_threshold_set(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data);

/**
* \brief - Get port unicast thresholds.
*
* \param [in] unit - device unit number.
* \param [in] port - port number.
* \param [out] data - struct containig the necessary data for threshold.
*                     the requested value will be set in data->threshold->value.
*                    \see ecgm_thresholds_internal.h
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_profile_mgmt_unicast_threshold_get(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data);

/**
* \brief - Set port multicast thresholds.
*
* \param [in] unit - device unit number.
* \param [in] port - port number.
* \param [in] data - struct containig the necessary data for threshold.
*                    \see ecgm_thresholds_internal.h
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_profile_mgmt_multicast_threshold_set(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data);

/**
* \brief - Get port multicast thresholds.
*
* \param [in] unit - device unit number.
* \param [in] port - port number.
* \param [out] data - struct containig the necessary data for threshold.
*                     the requested value will be set in data->threshold->value.
*                    \see ecgm_thresholds_internal.h
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_profile_mgmt_multicast_threshold_get(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data);

/** } */

/**
* \brief - Get port handle.
*          sets the handle variable with all the
*          profile requiered data.
*          NOTE that a variable of type dnx_ecgm_profile_mgmt_interface_handle_t
*          is not initialized to use before calling this function.
*
* \param [in] unit - device unit number.
* \param [in] port - port number.
* \param [out] interface_profile_handle - a handle to be
*                   used for this modules functionality.
*
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_profile_mgmt_interface_handle_get(
    int unit,
    bcm_port_t port,
    dnx_ecgm_profile_mgmt_interface_handle_t * interface_profile_handle);

/**
* \brief - Destroy profile.
*          cleans up any SW and HW data if needed.
*          MUST be called during port removal with an
*          initialized variable.
*
* \param [in] unit - device unit number.
* \param [out] interface_profile_handle - a handle to be
*                   used for this modules functionality.
*
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_profile_mgmt_interface_handle_destroy(
    int unit,
    dnx_ecgm_profile_mgmt_interface_handle_t * interface_profile_handle);

/**
* \brief - Set interface thresholds.
*
* \param [in] unit - device unit number.
* \param [in] port - port number.
* \param [in] data - struct containig the necessary data for threshold.
*                    \see ecgm_thresholds_internal.h
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_profile_mgmt_interface_threshold_set(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data);

/**
* \brief - Get interface thresholds.
*
* \param [in] unit - device unit number.
* \param [in] port - port number.
* \param [out] data - struct containig the necessary data for threshold.
*                     the requested value will be set in data->threshold->value.
*                    \see ecgm_thresholds_internal.h
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_profile_mgmt_interface_threshold_get(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data);

/** } */

/** } */

#endif  /** _EGRESS_CONGESTION_PROFILE_MGMT_H_INCLUDED_ */
