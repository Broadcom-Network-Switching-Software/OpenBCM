/*! \file template_manager_types.h
 *
 * Internal DNX template manager APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef ALGO_TEMPLATE_MNGR_TYPES_INCLUDED
/*
 * {
 */
#define ALGO_TEMPLATE_MNGR_TYPES_INCLUDED

/**
 *
 *   \brief General types for template manager.
 */
#include <soc/dnxc/swstate/dnx_sw_state_pretty_dump.h>
/*
 * Each of these enum members represents an advanced algorithm used by the template manager.
 */
typedef enum
{
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_INVALID = -1,
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC = 0,
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_SMART_TEMPLATE,
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_L3_SOURCE_TABLE,
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_TRAP_ETPP_LIF_PROFILE,
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_SMART_TEMPLATE_EVEN_DISTRIBUTION,
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_COUNT
} dnx_algo_template_advanced_algorithms_e;

/**
 * \brief The name of the template manager instance, used with all template manager functions.
 */
typedef char *dnx_algo_template_name_t;

/**
 * \brief Callback to print the data stored in template manager.
 *      Use the framework starting with \ref SW_STATE_PRETTY_PRINT_INIT_VARIABLES
 *      to print your resource data.
 *      See \ref dnx_algo_template_test_print_cb for example.
 *
 *  \par DIRECT INPUT:
 *   \param [in] unit -
 *     Identifier of the device to access.
 *    \param [in] data -
 *      Pointer of the struct to be printed.
 *  \par DIRECT OUTPUT:
 *      None
 *  \remark
 *    None
 *  \see
 *    dnx_algo_template_dump
 */
typedef void (
    *dnx_algo_template_print_data_cb) (
    int unit,
    const void *data);

/*
 * }
 */
#endif /*_ALGO_TEMPLATE_MNGR_CALLBACKS_INCLUDED__*/
