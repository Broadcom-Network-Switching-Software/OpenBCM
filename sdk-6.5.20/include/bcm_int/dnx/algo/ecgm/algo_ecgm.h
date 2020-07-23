/**
 * \file algo_ecgm.h
 *
 * Egress congestion algorithms
 *
 * this file holds the function definitions for
 * algorithms requiered by egress congestion module,
 * including template manager functions for creating
 * and managing the port profile and interface profile,
 * holding the thresholds for ecgm managed resources.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_ECGM_H_INCLUDED
/**
 * {
 */
#define ALGO_ECGM_H_INCLUDED

#include <shared/shrextend/shrextend_error.h>
#include <bcm_int/dnx/cosq/egress/ecgm.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * template name defines for algo ECGM
 */
#define DNX_ALGO_ECGM_PORT_PROFILE_TEMPLATE "Egress CGM Port Profile Template"
#define DNX_ALGO_ECGM_INTERFACE_PROFILE_TEMPLATE "Egress CGM Interface Profile Template"

 /**
 * Typedefs for port and interface profiles
 */
typedef struct
{
    dnx_ecgm_uc_thresholds_t unicast_queue[DNX_COSQ_NOF_TC];
    dnx_ecgm_uc_thresholds_t unicast_port;
    dnx_ecgm_mc_queue_resources_t multicast_queue[DNX_COSQ_NOF_TC];
    dnx_ecgm_mc_drop_thresholds_t multicast_port;
} dnx_ecgm_port_profile_info_t;

typedef struct
{
    dnx_ecgm_uc_fc_thresholds_t uc_fc;
    uint32 mc_interface_PDs_drop;
} dnx_ecgm_interface_profile_thresholds_t;

typedef struct
{
    dnx_ecgm_interface_profile_thresholds_t high_priority;
    dnx_ecgm_interface_profile_thresholds_t low_priority;
} dnx_ecgm_interface_profile_info_t;

/**
* port profile template API
*/

/**
* \brief - port profile template create.
*           this function should be called during ecgm module loading
*           to allow the allocation of ecgm port profiles.
*           the port profile template manager creates 16 port profiles per core.
*
* \param [in] unit - chip unit id
*
* \return - shr_error_e
*/

shr_error_e dnx_algo_ecgm_port_profile_template_create(
    int unit);

/**
* \brief - port profile template destroy.
*           this function should be called during device deinit
*           to free resources allocated for ecgm port profiles template.
*
* \param [in] unit - chip unit id
*
* \return - shr_error_e
*/
shr_error_e dnx_algo_ecgm_port_profile_template_destroy(
    int unit);

/**
* \brief - port profile template print callback
*
* \param [in] unit - chip unit id
* \param [in] data - data to be printed. must be of type dnx_ecgm_port_profile_info_t.
*/
void dnx_algo_ecgm_port_profile_print_cb(
    int unit,
    const void *data);

/**
 * interface profile template API
 */

 /**
 * \brief - interface profile template create function.
 *          creates the template manager for interface profile,
 *          that allows allocation and usage of interface profiles.
 *
 * \param [in] unit - chip unit id
 *
 * \return - shr_error_e
 */
shr_error_e dnx_algo_ecgm_interface_profile_template_create(
    int unit);

/**
* \brief - interface profile template destroy.
*           this function should be called during device deinit
*           to free resources allocated for ecgm interface profiles template.
*
* \param [in] unit - chip unit id
*
* \return - shr_error_e
*/
shr_error_e dnx_algo_ecgm_interface_profile_template_destroy(
    int unit);

/**
* \brief - interface profile template print callback
*
* \param [in] unit - chip unit id
* \param [in] data - data to be printed. must be of type dnx_ecgm_interface_profile_info_t.
*/

void dnx_algo_ecgm_interface_profile_print_cb(
    int unit,
    const void *data);

/**
* \brief - print function for port profile in SW state
*
* \param [in] unit - chip unit id
* \param [in] data - data to be printed
*/
void dnx_algo_ecgm_port_profile_info_print(
    int unit,
    dnx_ecgm_port_profile_info_t * data);

/*
 * }
 */
#endif /* ALGO_ECGM_H_INCLUDED */
