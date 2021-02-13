/** \file j2c_a0_data_adapter.c
 * 
 * DEVICE DATA - ADAPTER
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
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TX
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_adapter.h>
#include <src/bcm/dnx/rx/auto_generated/rx_internal_j2c_a0.h>
/*
 * }
 */

/*
 * FUNCTIONS:
 * {
 */
/*
 * Submodule: rx
 */

/*
 * Features
 */
/*
 * Defines
 */
/**
 * \brief device level function which set numeric packet_header_signal_id
 * numeric info:
 * The adater signal id to identify the packet header
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
j2c_a0_dnx_data_adapter_rx_packet_header_signal_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_rx;
    int define_index = dnx_data_adapter_rx_define_packet_header_signal_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = PACKET_HEADER_SIGNAL_ID_J2C_A0;

    /* Set value */
    define->data = PACKET_HEADER_SIGNAL_ID_J2C_A0;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set numeric constant_header_size
 * numeric info:
 * Number of bytes in the constant header of a rx packet
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
j2c_a0_dnx_data_adapter_rx_constant_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_rx;
    int define_index = dnx_data_adapter_rx_define_constant_header_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 28;

    /* Set value */
    define->data = 28;

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
 * Submodule: general
 */

/*
 * Features
 */
/*
 * Defines
 */
/**
 * \brief device level function which set numeric lib_ver
 * numeric info:
 * Adapter library version
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
j2c_a0_dnx_data_adapter_general_lib_ver_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_general;
    int define_index = dnx_data_adapter_general_define_lib_ver;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 5;

    /* Set value */
    define->data = 5;

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
j2c_a0_data_adapter_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_adapter;
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
     * Attach submodule: rx
     */
    submodule_index = dnx_data_adapter_submodule_rx;
    submodule = &module->submodules[submodule_index];

    /*
     * Attach defines: 
     */
    data_index = dnx_data_adapter_rx_define_packet_header_signal_id;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_adapter_rx_packet_header_signal_id_set;
    data_index = dnx_data_adapter_rx_define_constant_header_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_adapter_rx_constant_header_size_set;

    /*
     * Attach features: 
     */

    /*
     * Attach tables: 
     */
    /*
     * Attach submodule: general
     */
    submodule_index = dnx_data_adapter_submodule_general;
    submodule = &module->submodules[submodule_index];

    /*
     * Attach defines: 
     */
    data_index = dnx_data_adapter_general_define_lib_ver;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_adapter_general_lib_ver_set;

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
