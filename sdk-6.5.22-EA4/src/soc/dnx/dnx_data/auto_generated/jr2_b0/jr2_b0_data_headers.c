/** \file jr2_b0_data_headers.c
 * 
 * DEVICE DATA - HEADERS
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
#define BSL_LOG_MODULE BSL_LS_BCMDNX_BIERDNX
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_headers.h>
/*
 * }
 */

/*
 * FUNCTIONS:
 * {
 */
/*
 * Submodule: general
 */

/*
 * Features
 */
/**
 * \brief Pointer to function (per device) which set values for feature
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
jr2_b0_dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_general;
    int feature_index = dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    /* Set default value */
    feature->default_data = 1;

    /* Set value */
    feature->data = 1;

    /* Set data flags as supported */
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Defines
 */
/*
 * Tables
 */
/*
 * Submodule: feature
 */

/*
 * Features
 */
/**
 * \brief Pointer to function (per device) which set values for feature
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
jr2_b0_dnx_data_headers_feature_unknown_address_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_feature;
    int feature_index = dnx_data_headers_feature_unknown_address;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    /* Set default value */
    feature->default_data = 1;

    /* Set value */
    feature->data = 1;

    /* Set data flags as supported */
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Defines
 */
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
jr2_b0_data_headers_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_headers;
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
     * Attach submodule: general
     */
    submodule_index = dnx_data_headers_submodule_general;
    submodule = &module->submodules[submodule_index];

    /*
     * Attach defines: 
     */

    /*
     * Attach features: 
     */
    data_index = dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx_set;

    /*
     * Attach tables: 
     */
    /*
     * Attach submodule: feature
     */
    submodule_index = dnx_data_headers_submodule_feature;
    submodule = &module->submodules[submodule_index];

    /*
     * Attach defines: 
     */

    /*
     * Attach features: 
     */
    data_index = dnx_data_headers_feature_unknown_address;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_headers_feature_unknown_address_set;

    /*
     * Attach tables: 
     */

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE
/* *INDENT-ON* */
