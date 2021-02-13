/** \file jr2_a0_data_failover.c
 * 
 * DEVICE DATA - FAILOVER
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
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FAILOVER
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_failover.h>
/*
 * }
 */

/*
 * FUNCTIONS:
 * {
 */
/*
 * Submodule: path_select
 */

/*
 * Features
 */
/*
 * Defines
 */
/**
 * \brief device level function which set define egress_size
 * define info:
 * size of egress path select table
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
jr2_a0_dnx_data_failover_path_select_egress_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_egress_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 128*1024;

    /* Set value */
    define->data = 128*1024;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set define ingress_size
 * define info:
 * size of ingress path select table
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
jr2_a0_dnx_data_failover_path_select_ingress_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_ingress_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 16*1024;

    /* Set value */
    define->data = 16*1024;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set define fec_size
 * define info:
 * size of fec path select table
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
jr2_a0_dnx_data_failover_path_select_fec_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_fec_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 256*1024;

    /* Set value */
    define->data = 256*1024;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set define egr_no_protection
 * define info:
 * protection path select pointer dedicate for no egress protection
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
jr2_a0_dnx_data_failover_path_select_egr_no_protection_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_egr_no_protection;
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
 * \brief device level function which set define ing_no_protection
 * define info:
 * protection path select pointer dedicate for no ingress protection
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
jr2_a0_dnx_data_failover_path_select_ing_no_protection_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_ing_no_protection;
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
 * \brief device level function which set define fec_no_protection
 * define info:
 * protection path select pointer dedicate for no fec protection
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
jr2_a0_dnx_data_failover_path_select_fec_no_protection_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_fec_no_protection;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = dnx_data_failover.path_select.fec_size_get(unit)-1;

    /* Set value */
    define->data = dnx_data_failover.path_select.fec_size_get(unit)-1;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set define fec_facility_protection
 * define info:
 * protection path select pointer dedicate for fec facility protection
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
jr2_a0_dnx_data_failover_path_select_fec_facility_protection_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_fec_facility_protection;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = dnx_data_failover.path_select.fec_size_get(unit)-2;

    /* Set value */
    define->data = dnx_data_failover.path_select.fec_size_get(unit)-2;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set define nof_fec_path_select_banks
 * define info:
 * number of banks included in fec protection path select table
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
jr2_a0_dnx_data_failover_path_select_nof_fec_path_select_banks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_nof_fec_path_select_banks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 16;

    /* Set value */
    define->data = 16;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set define fec_bank_size
 * define info:
 * failover fec bank size
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
jr2_a0_dnx_data_failover_path_select_fec_bank_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_fec_bank_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = dnx_data_failover.path_select.fec_size_get(unit)/dnx_data_failover.path_select.nof_fec_path_select_banks_get(unit);

    /* Set value */
    define->data = dnx_data_failover.path_select.fec_size_get(unit)/dnx_data_failover.path_select.nof_fec_path_select_banks_get(unit);

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set define in_lif_protection_pointer_nof_msb
 * define info:
 * Number MS bits from protection pointer used for HW access to the fields(0-3) of memory IPPA_VTT_PATH_SELECT.
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
jr2_a0_dnx_data_failover_path_select_in_lif_protection_pointer_nof_msb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_msb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 2;

    /* Set value */
    define->data = 2;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set define in_lif_protection_pointer_nof_lsb
 * define info:
 * Number LS bits from protection pointer used for HW access to the entries (0-4095) of memory IPPA_VTT_PATH_SELECT.
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
jr2_a0_dnx_data_failover_path_select_in_lif_protection_pointer_nof_lsb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_path_select;
    int define_index = dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_lsb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 12;

    /* Set value */
    define->data = 12;

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
 * Submodule: facility
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
jr2_a0_dnx_data_failover_facility_accelerated_mode_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_failover;
    int submodule_index = dnx_data_failover_submodule_facility;
    int feature_index = dnx_data_failover_facility_accelerated_mode_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    /* Set default value */
    feature->default_data = 0;

    /* Set value */
    feature->data = 0;

    /* Set data flags as supported */
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    /* Property */
    feature->property.name = spn_BCM886XX_FEC_ACCELERATED_REROUTE_MODE;
    feature->property.doc = 
        "\n"
        "Enable/disable facility accelerated protection.\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_enable;
    feature->property.method_str = "enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    /* Set data flags as property */
    feature->flags |= (DNXC_DATA_F_PROPERTY);


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
jr2_a0_data_failover_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_failover;
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
     * Attach submodule: path_select
     */
    submodule_index = dnx_data_failover_submodule_path_select;
    submodule = &module->submodules[submodule_index];

    /*
     * Attach defines: 
     */
    data_index = dnx_data_failover_path_select_define_egress_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_egress_size_set;
    data_index = dnx_data_failover_path_select_define_ingress_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_ingress_size_set;
    data_index = dnx_data_failover_path_select_define_fec_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_fec_size_set;
    data_index = dnx_data_failover_path_select_define_egr_no_protection;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_egr_no_protection_set;
    data_index = dnx_data_failover_path_select_define_ing_no_protection;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_ing_no_protection_set;
    data_index = dnx_data_failover_path_select_define_fec_no_protection;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_fec_no_protection_set;
    data_index = dnx_data_failover_path_select_define_fec_facility_protection;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_fec_facility_protection_set;
    data_index = dnx_data_failover_path_select_define_nof_fec_path_select_banks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_nof_fec_path_select_banks_set;
    data_index = dnx_data_failover_path_select_define_fec_bank_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_fec_bank_size_set;
    data_index = dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_msb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_in_lif_protection_pointer_nof_msb_set;
    data_index = dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_lsb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_failover_path_select_in_lif_protection_pointer_nof_lsb_set;

    /*
     * Attach features: 
     */

    /*
     * Attach tables: 
     */
    /*
     * Attach submodule: facility
     */
    submodule_index = dnx_data_failover_submodule_facility;
    submodule = &module->submodules[submodule_index];

    /*
     * Attach defines: 
     */

    /*
     * Attach features: 
     */
    data_index = dnx_data_failover_facility_accelerated_mode_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_failover_facility_accelerated_mode_enable_set;

    /*
     * Attach tables: 
     */

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE
/* *INDENT-ON* */
