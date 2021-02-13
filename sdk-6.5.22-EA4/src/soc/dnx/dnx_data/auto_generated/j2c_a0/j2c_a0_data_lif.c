/** \file j2c_a0_data_lif.c
 * 
 * DEVICE DATA - LIF
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
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_lif.h>
/*
 * }
 */

/*
 * FUNCTIONS:
 * {
 */
/*
 * Submodule: global_lif
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
j2c_a0_dnx_data_lif_global_lif_prevent_tunnel_update_rif_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_global_lif;
    int feature_index = dnx_data_lif_global_lif_prevent_tunnel_update_rif;
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
/**
 * \brief device level function which set define glem_rif_optimization_enabled
 * define info:
 * Amount of out RIFs one GLEM entry can hold for optimized GLEM usage is 4.
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
j2c_a0_dnx_data_lif_global_lif_glem_rif_optimization_enabled_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_global_lif;
    int define_index = dnx_data_lif_global_lif_define_glem_rif_optimization_enabled;
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

/**
 * \brief device level function which set define null_lif
 * define info:
 * ERPP GLEM trap requires NULL LIF to be added to GLEM by default.
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
j2c_a0_dnx_data_lif_global_lif_null_lif_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_global_lif;
    int define_index = dnx_data_lif_global_lif_define_null_lif;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 0;

    /* Set value */
    define->data = 0;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set numeric egress_in_lif_null_value
 * numeric info:
 * IN_LIF null value in egress stage.
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
j2c_a0_dnx_data_lif_global_lif_egress_in_lif_null_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_global_lif;
    int define_index = dnx_data_lif_global_lif_define_egress_in_lif_null_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 0x0;

    /* Set value */
    define->data = 0x0;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;
    /* Property */
    define->property.name = spn_SYSTEM_HEADERS_MODE;
    define->property.doc = 
        "IN_LIF null value in egress stage\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "0";
    define->property.mapping[0].val = 0x0;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "1";
    define->property.mapping[1].val = 0x0;
    define->property.mapping[1].is_default = 1 ;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    /* Set data flags as property */
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}

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
j2c_a0_dnx_data_lif_feature_in_lif_tunnel_wide_data_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_feature;
    int feature_index = dnx_data_lif_feature_in_lif_tunnel_wide_data;
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
j2c_a0_data_lif_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_lif;
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
     * Attach submodule: global_lif
     */
    submodule_index = dnx_data_lif_submodule_global_lif;
    submodule = &module->submodules[submodule_index];

    /*
     * Attach defines: 
     */
    data_index = dnx_data_lif_global_lif_define_glem_rif_optimization_enabled;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_lif_global_lif_glem_rif_optimization_enabled_set;
    data_index = dnx_data_lif_global_lif_define_null_lif;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_lif_global_lif_null_lif_set;
    data_index = dnx_data_lif_global_lif_define_egress_in_lif_null_value;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_lif_global_lif_egress_in_lif_null_value_set;

    /*
     * Attach features: 
     */
    data_index = dnx_data_lif_global_lif_prevent_tunnel_update_rif;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_lif_global_lif_prevent_tunnel_update_rif_set;

    /*
     * Attach tables: 
     */
    /*
     * Attach submodule: feature
     */
    submodule_index = dnx_data_lif_submodule_feature;
    submodule = &module->submodules[submodule_index];

    /*
     * Attach defines: 
     */

    /*
     * Attach features: 
     */
    data_index = dnx_data_lif_feature_in_lif_tunnel_wide_data;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_lif_feature_in_lif_tunnel_wide_data_set;

    /*
     * Attach tables: 
     */

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE
/* *INDENT-ON* */
