/** \file jr2_b0_data_nif.c
 * 
 * DEVICE DATA - NIF
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
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_nif.h>
#include <bcm/port.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnxc/dnxc_port.h>
#include <phymod/phymod.h>
#include <soc/dnx/intr/auto_generated/jr2/jr2_intr.h>
/*
 * }
 */

/*
 * FUNCTIONS:
 * {
 */
/*
 * Submodule: ilkn
 */

/*
 * Features
 */
/*
 * Defines
 */
/**
 * \brief device level function which set define is_28G_speed_supported
 * define info:
 * Indicated if device supports ILKN with speed of 28125
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
jr2_b0_dnx_data_nif_ilkn_is_28G_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_is_28G_speed_supported;
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
jr2_b0_data_nif_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_nif;
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
     * Attach submodule: ilkn
     */
    submodule_index = dnx_data_nif_submodule_ilkn;
    submodule = &module->submodules[submodule_index];

    /*
     * Attach defines: 
     */
    data_index = dnx_data_nif_ilkn_define_is_28G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_nif_ilkn_is_28G_speed_supported_set;

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
