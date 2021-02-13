/** \file jr2_a0_data_tunnel.c
 * 
 * DEVICE DATA - TUNNEL
 * 
 * Device Data
 * SW component that maintains per device data
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 *        
 *     
 * 
 * AUTO-GENERATED BY AUTOCODER!
 * DO NOT EDIT THIS FILE!
 */
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tunnel.h>
/*
 * }
 */

/*
 * FUNCTIONS:
 * {
 */
/*
 * Submodule: ipv6
 */

/*
 * Features
 */
/*
 * Defines
 */
/**
 * \brief device level function which set define nof_terminated_sips
 * define info:
 * Number of ipv6 tunnel sip profile id
 * 
 * \param [in] unit - Unit #
 * 
 * \return
 *     err - see shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
static shr_error_e
jr2_a0_dnx_data_tunnel_ipv6_nof_terminated_sips_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_ipv6;
    int define_index = dnx_data_tunnel_ipv6_define_nof_terminated_sips;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 65536;

    /* Set value */
    define->data = 65536;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}

/*
 * Tables
 */
/*
 * Submodule: udp
 */

/*
 * Features
 */
/*
 * Defines
 */
/**
 * \brief device level function which set define nof_udp_ports_profiles
 * define info:
 * Number of udp ports profiles in the additional header profiles table.
 * 
 * \param [in] unit - Unit #
 * 
 * \return
 *     err - see shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
static shr_error_e
jr2_a0_dnx_data_tunnel_udp_nof_udp_ports_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_udp;
    int define_index = dnx_data_tunnel_udp_define_nof_udp_ports_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 8;

    /* Set value */
    define->data = 8;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set define first_udp_ports_profile
 * define info:
 * Lowest udp ports profiles in the additional header profiles table that's not the default profile.
 * 
 * \param [in] unit - Unit #
 * 
 * \return
 *     err - see shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
static shr_error_e
jr2_a0_dnx_data_tunnel_udp_first_udp_ports_profile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_udp;
    int define_index = dnx_data_tunnel_udp_define_first_udp_ports_profile;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 1;

    /* Set value */
    define->data = 1;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}

/*
 * Tables
 */
/*
 * Device attach func
 */
/**
 * \brief Attach device to module - attach set function to data structure
 * 
 * \param [in] unit - Unit #
 * 
 * \return
 *     err - 
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e
jr2_a0_data_tunnel_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    /*
     * Attach submodule: ipv6
     */
    submodule_index = dnx_data_tunnel_submodule_ipv6;
    submodule = &module->submodules[submodule_index];

    /*
     * Attach defines: 
     */
    data_index = dnx_data_tunnel_ipv6_define_nof_terminated_sips;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_ipv6_nof_terminated_sips_set;

    /*
     * Attach features: 
     */

    /*
     * Attach tables: 
     */
    /*
     * Attach submodule: udp
     */
    submodule_index = dnx_data_tunnel_submodule_udp;
    submodule = &module->submodules[submodule_index];

    /*
     * Attach defines: 
     */
    data_index = dnx_data_tunnel_udp_define_nof_udp_ports_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_udp_nof_udp_ports_profiles_set;
    data_index = dnx_data_tunnel_udp_define_first_udp_ports_profile;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_udp_first_udp_ports_profile_set;

    /*
     * Attach features: 
     */

    /*
     * Attach tables: 
     */

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE
/* *INDENT-ON* */
