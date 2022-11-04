
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_crps.h>







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
jr2_b0_dnx_data_crps_expansion_source_metadata_types_set(
    int unit)
{
    int source_index;
    int metadata_type_index;
    dnx_data_crps_expansion_source_metadata_types_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_expansion;
    int table_index = dnx_data_crps_expansion_table_source_metadata_types;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = bcmStatCounterInterfaceCount;
    table->info_get.key_size[0] = bcmStatCounterInterfaceCount;
    table->keys[1].size = bcmStatExpansionTypeMax;
    table->info_get.key_size[1] = bcmStatExpansionTypeMax;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_expansion_source_metadata_types_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_crps_expansion_table_source_metadata_types");

    
    default_data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->offset = -1;
    default_data->nof_bits = -1;
    
    for (source_index = 0; source_index < table->keys[0].size; source_index++)
    {
        for (metadata_type_index = 0; metadata_type_index < table->keys[1].size; metadata_type_index++)
        {
            data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, source_index, metadata_type_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeter2Value < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDropPrecedenceMeter2Value);
        data->offset = 0;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeter2Valid < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDropPrecedenceMeter2Valid);
        data->offset = 2;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypDropPrecedenceMeter1Value < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypDropPrecedenceMeter1Value);
        data->offset = 3;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeter1Valid < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDropPrecedenceMeter1Valid);
        data->offset = 5;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeter0Value < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDropPrecedenceMeter0Value);
        data->offset = 6;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeter0Valid < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDropPrecedenceMeter0Valid);
        data->offset = 8;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeTmDropReason < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeTmDropReason);
        data->offset = 9;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDispositionIsDrop < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDispositionIsDrop);
        data->offset = 13;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeterResolved < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeDropPrecedenceMeterResolved);
        data->offset = 14;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeTrafficClass < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeTrafficClass);
        data->offset = 16;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size && bcmStatExpansionTypeMetaData < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, bcmStatExpansionTypeMetaData);
        data->offset = 19;
        data->nof_bits = 16;
    }
    if (bcmStatCounterInterfaceIngressTransmitPp < table->keys[0].size && bcmStatExpansionTypeLatencyBin < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressTransmitPp, bcmStatExpansionTypeLatencyBin);
        data->offset = 0;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceIngressTransmitPp < table->keys[0].size && bcmStatExpansionTypeTrafficClass < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressTransmitPp, bcmStatExpansionTypeTrafficClass);
        data->offset = 3;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceIngressTransmitPp < table->keys[0].size && bcmStatExpansionTypeLatencyFlowProfile < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressTransmitPp, bcmStatExpansionTypeLatencyFlowProfile);
        data->offset = 6;
        data->nof_bits = 5;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size && bcmStatExpansionTypeTmDropReason < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, bcmStatExpansionTypeTmDropReason);
        data->offset = 0;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size && bcmStatExpansionTypeDispositionIsDrop < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, bcmStatExpansionTypeDispositionIsDrop);
        data->offset = 3;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size && bcmStatExpansionTypeTrafficClass < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, bcmStatExpansionTypeTrafficClass);
        data->offset = 4;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeterResolved < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, bcmStatExpansionTypeDropPrecedenceMeterResolved);
        data->offset = 7;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size && bcmStatExpansionTypeTmLastCopy < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, bcmStatExpansionTypeTmLastCopy);
        data->offset = 9;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeMetaData < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeMetaData);
        data->offset = 0;
        data->nof_bits = 4;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeterResolved < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeDropPrecedenceMeterResolved);
        data->offset = 4;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeTrafficClass < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeTrafficClass);
        data->offset = 6;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeSystemMultiCast < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeSystemMultiCast);
        data->offset = 9;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeEgressTmActionType < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeEgressTmActionType);
        data->offset = 10;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypePpDropReason < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypePpDropReason);
        data->offset = 12;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeDiscardPp < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeDiscardPp);
        data->offset = 14;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size && bcmStatExpansionTypeDiscardTm < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, bcmStatExpansionTypeDiscardTm);
        data->offset = 15;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size && bcmStatExpansionTypeDiscardPp < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, bcmStatExpansionTypeDiscardPp);
        data->offset = 0;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size &&  bcmStatExpansionTypeDiscardTm < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue,  bcmStatExpansionTypeDiscardTm);
        data->offset = 1;
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeterResolved < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, bcmStatExpansionTypeDropPrecedenceMeterResolved);
        data->offset = 2;
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size && bcmStatExpansionTypeTrafficClass < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, bcmStatExpansionTypeTrafficClass);
        data->offset = 4;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size && bcmStatExpansionTypeTmDropReason < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, bcmStatExpansionTypeTmDropReason);
        data->offset = 7;
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypeDropPrecedenceMeterResolved < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypeDropPrecedenceMeterResolved);
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypeTrafficClass < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypeTrafficClass);
        data->nof_bits = 3;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypeSystemMultiCast < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypeSystemMultiCast);
        data->nof_bits = 1;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypeEcnEligibleAndCni < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypeEcnEligibleAndCni);
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypePortMetaData < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypePortMetaData);
        data->nof_bits = 8;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypePpDropReason < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypePpDropReason);
        data->nof_bits = 2;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size && bcmStatExpansionTypeMetaData < table->keys[1].size)
    {
        data = (dnx_data_crps_expansion_source_metadata_types_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, bcmStatExpansionTypeMetaData);
        data->nof_bits = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_crps_expansion_source_expansion_info_set(
    int unit)
{
    int source_index;
    dnx_data_crps_expansion_source_expansion_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_crps;
    int submodule_index = dnx_data_crps_submodule_expansion;
    int table_index = dnx_data_crps_expansion_table_source_expansion_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = bcmStatCounterInterfaceCount;
    table->info_get.key_size[0] = bcmStatCounterInterfaceCount;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_crps_expansion_source_expansion_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_crps_expansion_table_source_expansion_info");

    
    default_data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->expansion_size = 0;
    default_data->supports_expansion_per_type = 0;
    
    for (source_index = 0; source_index < table->keys[0].size; source_index++)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, source_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (bcmStatCounterInterfaceIngressReceivePp < table->keys[0].size)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressReceivePp, 0);
        data->expansion_size = 36;
        data->supports_expansion_per_type = 1;
    }
    if (bcmStatCounterInterfaceIngressTransmitPp < table->keys[0].size)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressTransmitPp, 0);
        data->expansion_size = 11;
        data->supports_expansion_per_type = 0;
    }
    if (bcmStatCounterInterfaceIngressVoq < table->keys[0].size)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceIngressVoq, 0);
        data->expansion_size = 10;
        data->supports_expansion_per_type = 0;
    }
    if (bcmStatCounterInterfaceEgressReceivePp < table->keys[0].size)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceivePp, 0);
        data->expansion_size = 16;
        data->supports_expansion_per_type = 1;
    }
    if (bcmStatCounterInterfaceEgressReceiveQueue < table->keys[0].size)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressReceiveQueue, 0);
        data->expansion_size = 10;
        data->supports_expansion_per_type = 0;
    }
    if (bcmStatCounterInterfaceEgressTransmitPp < table->keys[0].size)
    {
        data = (dnx_data_crps_expansion_source_expansion_info_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmStatCounterInterfaceEgressTransmitPp, 0);
        data->expansion_size = 24;
        data->supports_expansion_per_type = 1;
    }

    
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

    

    

    
    data_index = dnx_data_crps_expansion_table_source_metadata_types;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_crps_expansion_source_metadata_types_set;
    data_index = dnx_data_crps_expansion_table_source_expansion_info;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_crps_expansion_source_expansion_info_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

