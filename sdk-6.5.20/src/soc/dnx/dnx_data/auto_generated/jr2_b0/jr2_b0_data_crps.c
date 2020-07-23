

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
jr2_b0_dnx_data_crps_engine_valid_address_admission_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_engine;
    int feature_index = dnx_data_crps_engine_valid_address_admission_is_supported;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}









static shr_error_e
jr2_b0_dnx_data_crps_expansion_itm_types_set(
    int unit)
{
    dnx_data_crps_expansion_itm_types_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_expansion;
    int table_index = dnx_data_crps_expansion_table_itm_types;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "bcmStatExpansionTypeTmDropReason, bcmStatExpansionTypeDispositionIsDrop, bcmStatExpansionTypeTrafficClass,                  bcmStatExpansionTypeDropPrecedenceMeterResolved, bcmStatExpansionTypeTmLastCopy, bcmStatExpansionTypeMax";
    table->values[1].default_val = "10";
    table->values[2].default_val = "0";
    table->values[3].default_val = "3";
    table->values[4].default_val = "4";
    table->values[5].default_val = "7";
    table->values[6].default_val = "9";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_expansion_itm_types_t, (1 + 1  ), "data of dnx_data_crps_expansion_table_itm_types");

    
    default_data = (dnx_data_crps_expansion_itm_types_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid_types[0] = bcmStatExpansionTypeTmDropReason;
    default_data->valid_types[1] = bcmStatExpansionTypeDispositionIsDrop;
    default_data->valid_types[2] = bcmStatExpansionTypeTrafficClass;
    default_data->valid_types[3] = bcmStatExpansionTypeDropPrecedenceMeterResolved;
    default_data->valid_types[4] = bcmStatExpansionTypeTmLastCopy;
    default_data->valid_types[5] = bcmStatExpansionTypeMax;
    default_data->total_nof_bits = 10;
    default_data->drop_reason_offset = 0;
    default_data->disposition_offset = 3;
    default_data->tc_offset = 4;
    default_data->final_dp_offset = 7;
    default_data->last_copy_offset = 9;
    
    data = (dnx_data_crps_expansion_itm_types_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_b0_data_crps_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_crps;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_crps_submodule_engine;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_crps_engine_valid_address_admission_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_crps_engine_valid_address_admission_is_supported_set;

    
    
    submodule_index = dnx_data_crps_submodule_expansion;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_crps_expansion_table_itm_types;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_crps_expansion_itm_types_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

