/** \file ramon_a0_data_linkscan.c
 * 
 * DEVICE DATA - LINKSCAN
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
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LINKSCANDNX
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_linkscan.h>
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
/*
 * Defines
 */
/**
 * \brief device level function which set numeric interval
 * numeric info:
 * linkscan interval in microseconds
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
ramon_a0_dnxf_data_linkscan_general_interval_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_linkscan;
    int submodule_index = dnxf_data_linkscan_submodule_general;
    int define_index = dnxf_data_linkscan_general_define_interval;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 250000;

    /* Set value */
    define->data = 250000;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;
    /* Property */
    define->property.name = spn_BCM_LINKSCAN_INTERVAL;
    define->property.doc = 
        "\n"
        "Configure linkscan interval in microseconds\n"
        "bcm_linkscan_interval= 0~0xffffffff\n"
        "Default: 250,000\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 0xffffffff;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    /* Set data flags as property */
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set numeric error_delay
 * numeric info:
 * the amount of time in microseconds for which the bcm_linkscan module will suspend a port from further update processing after 'max_error' errors are detected.  After this delay, the error state for the port is cleared and normal linkscan processing resumes on the port
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
ramon_a0_dnxf_data_linkscan_general_error_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_linkscan;
    int submodule_index = dnxf_data_linkscan_submodule_general;
    int define_index = dnxf_data_linkscan_general_define_error_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 10000000;

    /* Set value */
    define->data = 10000000;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;
    /* Property */
    define->property.name = spn_BCM_LINKSCAN_ERRDELAY;
    define->property.doc = 
        "\n"
        "The amount of time in microseconds for which the bcm_linkscan module will suspend\n"
        "a port from further update processing after 'bcm_linkscan_maxerr' errors are detected.\n"
        "After this delay, the error state for the port is cleared and normal linkscan processing\n"
        "resumes on the port\n"
        "bcm_linkscan_errdelay= 0~0xffffffff\n"
        "Default: 10,000,000\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 0xffffffff;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    /* Set data flags as property */
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set numeric max_error
 * numeric info:
 * The number of port update errors which will cause the bcm_linkscan module to suspend a port from update processing for the period of time set in 'error_delay'
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
ramon_a0_dnxf_data_linkscan_general_max_error_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_linkscan;
    int submodule_index = dnxf_data_linkscan_submodule_general;
    int define_index = dnxf_data_linkscan_general_define_max_error;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 5;

    /* Set value */
    define->data = 5;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;
    /* Property */
    define->property.name = spn_BCM_LINKSCAN_MAXERR;
    define->property.doc = 
        "\n"
        "The number of port update errors which will cause the bcm_linkscan module to suspend\n"
        "a port from update processing for the period of time set in 'bcm_linkscan_errdelay'\n"
        "bcm_linkscan_maxerr= 0~0xffffffff\n"
        "Default: 5\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 0xffffffff;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    /* Set data flags as property */
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set numeric thread_priority
 * numeric info:
 * specifies the priority of the BCM Linkscan thread
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
ramon_a0_dnxf_data_linkscan_general_thread_priority_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_linkscan;
    int submodule_index = dnxf_data_linkscan_submodule_general;
    int define_index = dnxf_data_linkscan_general_define_thread_priority;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 50;

    /* Set value */
    define->data = 50;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;
    /* Property */
    define->property.name = spn_LINKSCAN_THREAD_PRI;
    define->property.doc = 
        "\n"
        "Specifies the priority of the BCM Linkscan thread\n"
        "linkcan_thread_pri= 0~0xffffffff\n"
        "Default: 50\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 1;
    define->property.range_max = 99;

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    /* Set data flags as property */
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief device level function which set numeric m0_pause_enable
 * numeric info:
 * specifies the pause m0 firmware at Linkscan thread
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
ramon_a0_dnxf_data_linkscan_general_m0_pause_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_linkscan;
    int submodule_index = dnxf_data_linkscan_submodule_general;
    int define_index = dnxf_data_linkscan_general_define_m0_pause_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    /* Set default value */
    define->default_data = 1;

    /* Set value */
    define->data = 1;

    /* Set data flags as supported */
    define->flags |= DNXC_DATA_F_SUPPORTED;
    /* Property */
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "custom_feature_linkscan_m0_pause='0' or '1'\n"
        "if 1 will pause firmware while linkscan thread process.\n"
        "Default - '0'\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "linkscan_m0_pause";

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
/**
 * \brief device level function which set values for table pbmp
 * Module - 'linkscan', Submodule - 'general', table - 'pbmp'
 * linkscan pbmp
 * The function set relevant table structure in _dnxf_data
 * 
 * \param [in] unit - Unit #
 * 
 * \return
 *     err - See shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
static shr_error_e
ramon_a0_dnxf_data_linkscan_general_pbmp_set(
    int unit)
{
    dnxf_data_linkscan_general_pbmp_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_linkscan;
    int submodule_index = dnxf_data_linkscan_submodule_general;
    int table_index = dnxf_data_linkscan_general_table_pbmp;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    /* Set data flags as supported */
    table->flags |= DNXC_DATA_F_SUPPORTED;

    /* Info - default values */
    table->values[0].default_val = "0x0";
    table->values[1].default_val = "0x0";
    /* Allocate data buffer */
    DNXC_DATA_ALLOC(table->data, dnxf_data_linkscan_general_pbmp_t, (1 + 1 /* to store default value */ ), "data of dnxf_data_linkscan_general_table_pbmp");

    /* Store Default Values */
    default_data = (dnxf_data_linkscan_general_pbmp_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    _SHR_PBMP_WORD_SET(default_data->sw, 0, 0x0);
    _SHR_PBMP_WORD_SET(default_data->hw, 0, 0x0);
    /* Set Default Values */
    data = (dnxf_data_linkscan_general_pbmp_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    /*
     * Value Property - sw
     */
    table->values[0].property.name = spn_BCM_LINKSCAN_PBMP;
    table->values[0].property.doc =
        "\n"
        "sw linkscan bitmap\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_pbmp;
    table->values[0].property.method_str = "suffix_pbmp";
    table->values[0].property.suffix = "sw";
    /*
     * Value Property - hw
     */
    table->values[1].property.name = spn_BCM_LINKSCAN_PBMP;
    table->values[1].property.doc =
        "\n"
        "hw linkscan bitmap\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_suffix_pbmp;
    table->values[1].property.method_str = "suffix_pbmp";
    table->values[1].property.suffix = "hw";

    /* Override data with dynamic data (if required) */
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnxf_data_linkscan_general_pbmp_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->sw));
    data = (dnxf_data_linkscan_general_pbmp_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, -1, &data->hw));
    /* Set data flags as property */
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}

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
ramon_a0_data_linkscan_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnxf_data_module_linkscan;
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
    submodule_index = dnxf_data_linkscan_submodule_general;
    submodule = &module->submodules[submodule_index];

    /*
     * Attach defines: 
     */
    data_index = dnxf_data_linkscan_general_define_interval;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_linkscan_general_interval_set;
    data_index = dnxf_data_linkscan_general_define_error_delay;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_linkscan_general_error_delay_set;
    data_index = dnxf_data_linkscan_general_define_max_error;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_linkscan_general_max_error_set;
    data_index = dnxf_data_linkscan_general_define_thread_priority;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_linkscan_general_thread_priority_set;
    data_index = dnxf_data_linkscan_general_define_m0_pause_enable;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_linkscan_general_m0_pause_enable_set;

    /*
     * Attach features: 
     */

    /*
     * Attach tables: 
     */
    data_index = dnxf_data_linkscan_general_table_pbmp;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_linkscan_general_pbmp_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE
/* *INDENT-ON* */
