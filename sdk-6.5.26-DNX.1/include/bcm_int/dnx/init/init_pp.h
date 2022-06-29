/** \file init_pp.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _INIT_PP_INCLUDED__
/*
 * {
 */
#define _INIT_PP_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/** Forwarding additional info in case of MPLS INJECTED OAMP */
#define MPLS_INJECTED_OAMP_FAI                  5
    /** System Scope LIF */
#define DEVICE_SCOPE_LIF_PROFILE    0
    /** System Scope LIF */
#define SYSTEM_SCOPE_LIF_PROFILE    1
    /** Disable Same Interface Filter for Lif profile */
#define DISABLE_SAME_INTERFACE_LIF_PROFILE    2

    /** Device Scope LIF in JR mode */
#define DEVICE_SCOPE_JR_MODE_LIF_PROFILE    0
    /** System Scope LIF in JR mode */
#define SYSTEM_SCOPE_JR_MODE_LIF_PROFILE    1
    /** Disable Same Interface Filter for device scope in JR mode */
#define DEVICE_SCOPE_JR_MODE_LIF_PROFILE_DISABLE    3
    /** Disable Same Interface Filter for system scope in JR mode */
#define SYSTEM_SCOPE_JR_MODE_LIF_PROFILE_DISABLE    4

#define JR_COMP_MODE_ONLY_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES 1
#define JR_COMP_MODE_ONLY_DEV_SCOPE_ENABLED_BY_SOC_PROPERTIES 2
#define JR_COMP_MODE_BOTH_DEV_AND_SYS_SCOPE_ENABLED_BY_SOC_PROPERTIES 3
/*
 * }
 */
/**
 * \brief - return the hw value of layer types dbal enum
 * \param [in] unit - Relevant unit.
 * \param [in] prototype - dbal enum value
 * \return
 *   hw value or error
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_init_pp_layer_types_value_get(
    int unit,
    uint32 prototype);
/**
 * \brief -
 * The function is called on Init time.
 * It invokes forward_code_map_init and configures EGRESS_FORWARD_CODE_SELECTION table
 *
 * \param [in] unit -     Relevant unit.
 *
 * \retval Error indication according to shr_error_e enum
 *
 */

shr_error_e dnx_general_pp_init(
    int unit);
/**
 * \brief - The function configures initial values for
 *        DBAL_TABLE_EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE
 * \param [in] unit - Relevant unit.
 * \param [in] current_protocol_type - Current Protocol Type value
 * \param [in] current_protocol_namespace - Current Protocol Namespace
 * \param [in] current_next_protocol - Current Next Protocol

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_pp_egress_current_next_protocol_config_table_configure(
    int unit,
    dbal_enum_value_field_current_protocol_type_e current_protocol_type,
    dbal_enum_value_field_etpp_next_protocol_namespace_e current_protocol_namespace,
    int current_next_protocol);

/**
 * \brief - Get the current_next_protocol value for
 *        DBAL_TABLE_EGRESS_CURRENT_NEXT_PROTOCOL_MAP_TABLE
 * \param [in] unit - Relevant unit.
 * \param [in] current_protocol_type - Current Protocol Type value
 * \param [in] current_protocol_namespace - Current Protocol Namespace
 * \param [out] current_next_protocol - Current Next Protocol

 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_pp_egress_current_next_protocol_config_table_get(
    int unit,
    int current_protocol_type,
    int current_protocol_namespace,
    int *current_next_protocol);

#endif/*_INIT_PP_INCLUDED__*/
