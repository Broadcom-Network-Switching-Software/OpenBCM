
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_field_map.h>
#include <bcm_int/dnx/field/field_context.h>









static shr_error_e
j2x_a0_dnx_data_field_map_action_params_set(
    int unit)
{
    int stage_index;
    int action_index;
    dnx_data_field_map_action_params_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field_map;
    int submodule_index = dnx_data_field_map_submodule_action;
    int table_index = dnx_data_field_map_action_table_params;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_FIELD_STAGE_NOF;
    table->info_get.key_size[0] = DNX_FIELD_STAGE_NOF;
    table->keys[1].size = DNX_FIELD_ACTION_NOF;
    table->info_get.key_size[1] = DNX_FIELD_ACTION_NOF;

    
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_map_action_params_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_field_map_action_table_params");

    
    default_data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->id = -1;
    default_data->size = -1;
    
    for (stage_index = 0; stage_index < table->keys[0].size; stage_index++)
    {
        for (action_index = 0; action_index < table->keys[1].size; action_index++)
        {
            data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index, action_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_ACL_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_ACL_CONTEXT);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_ACL_CONTEXT;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_ADMT_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_ADMT_PROFILE);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_ADMT_PROFILE;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_BIER_STR_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_BIER_STR_OFFSET);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_OFFSET;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_BIER_STR_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_BIER_STR_SIZE);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_SIZE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_CONGESTION_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_CONGESTION_INFO);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_CONGESTION_INFO;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_DST_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_DST_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_DP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_DP);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_DP;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_DP_METER_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_DP_METER_COMMAND);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_DP_METER_COMMAND;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_EEI < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_EEI);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_EEI;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_EGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_EGRESS_LEARN_ENABLE);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_LEARN_ENABLE;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_ELEPHANT_PAYLOAD < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_ELEPHANT_PAYLOAD);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_ELEPHANT_PAYLOAD;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_TRACE_PACKET_ACT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_TRACE_PACKET_ACT);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_TRACE_PACKET_ACT;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_END_OF_PACKET_EDITING < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_END_OF_PACKET_EDITING);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_END_OF_PACKET_EDITING;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_EXT_STATISTICS_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_EXT_STATISTICS_0);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_0;
        data->size = 23;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_EXT_STATISTICS_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_EXT_STATISTICS_1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_1;
        data->size = 23;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_EXT_STATISTICS_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_EXT_STATISTICS_2);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_2;
        data->size = 23;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_EXT_STATISTICS_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_EXT_STATISTICS_3);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_3;
        data->size = 23;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_TRAP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_TRAP);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_FWD;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_FWD_DOMAIN);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_DOMAIN;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_FWD_DOMAIN_WITH_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_FWD_DOMAIN_WITH_PROFILE);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_DOMAIN;
        data->size = (18+10);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_FWD_LAYER_ADDITIONAL_INFO);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_ADDITIONAL_INFO;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_FWD_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_FWD_CONTEXT);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_CONTEXT;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_FWD_LAYER_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_FWD_LAYER_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_INDEX;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_IEEE1588_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_IEEE1588_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_IEEE1588_DATA;
        data->size = 13;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_INGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_INGRESS_LEARN_ENABLE);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_LEARN_ENABLE;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GLOB_IN_LIF_0);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_GLOB_IN_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GLOB_IN_LIF_1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_1;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_IN_LIF_PROFILE_0);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_0;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_IN_LIF_PROFILE_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_IN_LIF_PROFILE_1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_1;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_PP_PORT);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_IN_PP_PORT;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_IN_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_IN_TTL);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_IN_TTL;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_ITPP_DELTA_AND_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_ITPP_DELTA_AND_VALID);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_ITPP_DELTA;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_LATENCY_FLOW < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_LATENCY_FLOW);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_LATENCY_FLOW;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_OR_TRANSPLANT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_LEARN_OR_TRANSPLANT);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_OR_TRANSPLANT;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_MIRROR_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_MIRROR_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA;
        data->size = 17;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_NWK_QOS);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_NWK_QOS;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_OAM_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_OAM_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_DATA;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_PMF_OAM_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_PMF_OAM_ID);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_ID;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GLOB_OUT_LIF_0);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_0;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GLOB_OUT_LIF_1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_1;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_GLOB_OUT_LIF_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GLOB_OUT_LIF_2);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_2;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_GLOB_OUT_LIF_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GLOB_OUT_LIF_3);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_3;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_OUT_LIF_PUSH < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_OUT_LIF_PUSH);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_OUT_LIF_PUSH;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_PACKET_IS_COMPATIBLE_MC < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_PACKET_IS_COMPATIBLE_MC);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_COMPATIBLE_MC;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_PACKET_IS_BIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_PACKET_IS_BIER);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_BIER;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_EGRESS_PARSING_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_EGRESS_PARSING_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_PARSING_INDEX;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_GENERAL_DATA0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GENERAL_DATA0);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA0;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_GENERAL_DATA1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GENERAL_DATA1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA1;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_GENERAL_DATA2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GENERAL_DATA2);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA2;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_GENERAL_DATA3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GENERAL_DATA3);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA3;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_PPH_RESERVED < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_PPH_RESERVED);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_RESERVED;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_PPH_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_PPH_TYPE);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_TYPE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_RPF_DST < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_RPF_DST);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_RPF_DST_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_RPF_DST_VALID);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST_VALID;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_RPF_OUT_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_RPF_OUT_LIF);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_OUT_LIF;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_PTC < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_PTC);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_PTC;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICAL_SAMPLING < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICAL_SAMPLING);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICAL_SAMPLING;
        data->size = 25;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_SNOOP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_SNOOP);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_SNOOP_DATA;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_SRC_SYS_PORT);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_SRC_SYS_PORT;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STACKING_ROUTE_HISTORY_BITMAP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STACKING_ROUTE_HISTORY_BITMAP);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STACKING_ROUTE_HISTORY_BITMAP;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_SLB_PAYLOAD0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_SLB_PAYLOAD0);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD0;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_SLB_PAYLOAD1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_SLB_PAYLOAD1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD1;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_SLB_KEY0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_SLB_KEY0);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY0;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_SLB_KEY1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_SLB_KEY1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY1;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_SLB_KEY2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_SLB_KEY2);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY2;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_SLB_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_SLB_FOUND);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_FOUND;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATE_ADDRESS_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATE_ADDRESS_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_ADDRESS_DATA;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATE_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_DATA;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_META_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_META_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_META_DATA;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ID_0);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_0;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ID_1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_1;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ID_2);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_2;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ID_3);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_3;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ID_4);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_4;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ID_5);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_5;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_6 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ID_6);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_6;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_7 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ID_7);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_7;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_8 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ID_8);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_8;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_9 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ID_9);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_9;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ATR_0);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_0;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ATR_1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_1;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ATR_2);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_2;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ATR_3);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_3;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ATR_4);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_4;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ATR_5);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_5;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_6 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ATR_6);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_6;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_7 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ATR_7);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_7;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_8 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ATR_8);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_8;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_9 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_ATR_9);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_9;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_OBJECT_10 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STATISTICS_OBJECT_10);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_OBJECT_10;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_ST_VSQ_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_ST_VSQ_PTR);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_ST_VSQ_PTR;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_SYSTEM_HEADER_PROFILE_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_SYSTEM_HEADER_PROFILE_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_SYSTEM_HEADER_PROFILE_INDEX;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_TC < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_TC);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_TC;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_TM_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_TM_PROFILE);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_TM_PROFILE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_USER_HEADER_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_USER_HEADER_1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_USER_HEADER_1_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_USER_HEADER_1_TYPE);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1_TYPE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_USER_HEADER_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_USER_HEADER_2);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_USER_HEADER_2_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_USER_HEADER_2_TYPE);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2_TYPE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_USER_HEADER_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_USER_HEADER_3);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_USER_HEADER_3_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_USER_HEADER_3_TYPE);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3_TYPE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_USER_HEADER_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_USER_HEADER_4);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_USER_HEADER_4_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_USER_HEADER_4_TYPE);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4_TYPE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_VLAN_EDIT_CMD_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_VLAN_EDIT_CMD_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_CMD_INDEX;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_1;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_2);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_2;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_VLAN_EDIT_VID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_VLAN_EDIT_VID_1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_1;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_VLAN_EDIT_VID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_VLAN_EDIT_VID_2);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_2;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_INGRESS_TIME_STAMP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_INGRESS_TIME_STAMP);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_INT_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_INT_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_INT_DATA;
        data->size = 10;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_INGRESS_TIME_STAMP_OVERRIDE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_INGRESS_TIME_STAMP_OVERRIDE);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP_OVERRIDE;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_EVENTOR < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_EVENTOR);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_EVENTOR;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_STAT_OBJ_LM_READ_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_STAT_OBJ_LM_READ_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_STAT_OBJ_LM_READ_INDEX;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_VISIBILITY);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_PRT_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_PRT_QUALIFIER);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_PRT_QUALIFIER;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_INFO_KEY_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_LEARN_INFO_KEY_0);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_0;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_INFO_KEY_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_LEARN_INFO_KEY_1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_1;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_INFO_PAYLOAD_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_LEARN_INFO_PAYLOAD_0);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_0;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_INFO_PAYLOAD_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_LEARN_INFO_PAYLOAD_1);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_1;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_INFO_FORMAT_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_LEARN_INFO_FORMAT_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_FORMAT_INDEX;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_INVALIDATE_NEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_INVALIDATE_NEXT);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALIDATE_NEXT;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_ACTION_ID_INVALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_ID_INVALID);
        data->id = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID;
        data->size = 0;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_ACL_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_ACL_CONTEXT);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_ACL_CONTEXT;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_ADMT_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_ADMT_PROFILE);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_ADMT_PROFILE;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_BIER_STR_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_BIER_STR_OFFSET);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_OFFSET;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_BIER_STR_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_BIER_STR_SIZE);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_SIZE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_BYTES_TO_REMOVE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_BYTES_TO_REMOVE);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_BYTES_TO_REMOVE;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_CONGESTION_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_CONGESTION_INFO);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_CONGESTION_INFO;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_DP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_DP);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_DP;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_DP_METER_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_DP_METER_COMMAND);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_DP_METER_COMMAND;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_EEI < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_EEI);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_EEI;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_EGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_EGRESS_LEARN_ENABLE);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_LEARN_ENABLE;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_END_OF_PACKET_EDITING < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_END_OF_PACKET_EDITING);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_END_OF_PACKET_EDITING;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_EXT_STATISTICS_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_EXT_STATISTICS_0);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_0;
        data->size = 23;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_EXT_STATISTICS_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_EXT_STATISTICS_1);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_1;
        data->size = 23;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_EXT_STATISTICS_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_EXT_STATISTICS_2);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_2;
        data->size = 23;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_EXT_STATISTICS_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_EXT_STATISTICS_3);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_3;
        data->size = 23;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_TRAP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_TRAP);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_FWD;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_DST_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_DST_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_DST_DATA;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_FWD_DOMAIN);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_DOMAIN;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_FWD_LAYER_ADDITIONAL_INFO);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_ADDITIONAL_INFO;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_FWD_LAYER_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_FWD_LAYER_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_INDEX;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_GENERAL_DATA0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GENERAL_DATA0);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_0;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_GENERAL_DATA1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GENERAL_DATA1);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_1;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_GENERAL_DATA2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GENERAL_DATA2);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_2;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_GENERAL_DATA3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GENERAL_DATA3);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_3;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_IEEE1588_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_IEEE1588_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_IEEE1588_DATA;
        data->size = 13;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_INGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_INGRESS_LEARN_ENABLE);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_LEARN_ENABLE;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_INGRESS_TIME_STAMP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_INGRESS_TIME_STAMP);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_INGRESS_TIME_STAMP_OVERRIDE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_INGRESS_TIME_STAMP_OVERRIDE);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP_OVERRIDE;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GLOB_IN_LIF_0);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_0;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_IN_LIF_PROFILE_0);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_0;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_IN_LIF_PROFILE_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_IN_LIF_PROFILE_1);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_1;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_GLOB_IN_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GLOB_IN_LIF_1);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_1;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_PP_PORT);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_IN_PP_PORT;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_INT_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_INT_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_INT_DATA;
        data->size = 10;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_IN_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_IN_TTL);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_IN_TTL;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_ITPP_DELTA_AND_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_ITPP_DELTA_AND_VALID);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_ITPP_DELTA;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_LAG_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_LAG_LB_KEY);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_LAG_LB_KEY;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_LATENCY_FLOW < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_LATENCY_FLOW);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_LATENCY_FLOW;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_OR_TRANSPLANT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_LEARN_OR_TRANSPLANT);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_OR_TRANSPLANT;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_MIRROR_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_MIRROR_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_MIRROR_DATA;
        data->size = 17;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_NWK_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_NWK_LB_KEY);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_LB_KEY;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_NWK_QOS);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_QOS;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_NWK_HEADER_APPEND_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_NWK_HEADER_APPEND_SIZE);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_APPEND_SIZE;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_OAM_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_OAM_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_DATA;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_PMF_OAM_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_PMF_OAM_ID);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_ID;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GLOB_OUT_LIF_0);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_0;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GLOB_OUT_LIF_1);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_1;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_GLOB_OUT_LIF_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GLOB_OUT_LIF_2);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_2;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_GLOB_OUT_LIF_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GLOB_OUT_LIF_3);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_3;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_IS_APPLET < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_IS_APPLET);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_APPLET;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_PACKET_IS_BIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_PACKET_IS_BIER);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_BIER;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_EGRESS_PARSING_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_EGRESS_PARSING_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_PARSING_INDEX;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_PPH_RESERVED < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_PPH_RESERVED);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_RESERVED;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_PPH_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_PPH_TYPE);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_TYPE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_PUSH_OUT_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_PUSH_OUT_LIF);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_PUSH_OUT_LIF;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_SNOOP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_SNOOP);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_SNOOP_DATA;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_SRC_SYS_PORT);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_SRC_SYS_PORT;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STACKING_ROUTE_HISTORY_BITMAP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STACKING_ROUTE_HISTORY_BITMAP);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STACKING_ROUTE_HISTORY_BITMAP;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICAL_SAMPLING < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICAL_SAMPLING);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICAL;
        data->size = 25;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_META_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_META_DATA);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_META_DATA;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ID_0);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_0;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ID_1);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_1;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_OBJECT_10 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_OBJECT_10);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_OBJECT_10;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ID_2);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_2;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ID_3);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_3;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ID_4);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_4;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ID_5);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_5;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_6 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ID_6);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_6;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_7 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ID_7);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_7;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_8 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ID_8);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_8;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ID_9 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ID_9);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_9;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ATR_0);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_0;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ATR_1);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_1;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ATR_2);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_2;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ATR_3);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_3;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ATR_4);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_4;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ATR_5);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_5;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_6 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ATR_6);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_6;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_7 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ATR_7);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_7;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_8 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ATR_8);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_8;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STATISTICS_ATR_9 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STATISTICS_ATR_9);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_9;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_ST_VSQ_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_ST_VSQ_PTR);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_ST_VSQ_PTR;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_SYSTEM_HEADER_PROFILE_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_SYSTEM_HEADER_PROFILE_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_PROFILE_INDEX;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_SYSTEM_HEADER_SIZE_ADJUST < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_SYSTEM_HEADER_SIZE_ADJUST);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_SIZE_ADJUST;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_TC < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_TC);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_TC;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_TM_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_TM_PROFILE);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_TM_PROFILE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_USER_HEADER_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_USER_HEADER_1);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_1;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_USER_HEADER_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_USER_HEADER_2);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_2;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_USER_HEADER_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_USER_HEADER_3);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_3;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_USER_HEADER_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_USER_HEADER_4);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_4;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_USER_HEADERS_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_USER_HEADERS_TYPE);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADERS_TYPE;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_VISIBILITY);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_VISIBILITY;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_VLAN_EDIT_CMD_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_VLAN_EDIT_CMD_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_CMD_INDEX;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_1);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_1;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_2);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_2;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_VLAN_EDIT_VID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_VLAN_EDIT_VID_1);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_1;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_VLAN_EDIT_VID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_VLAN_EDIT_VID_2);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_2;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_PARSING_START_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_PARSING_START_OFFSET);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_OFFSET;
        data->size = 10;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_PARSING_START_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_PARSING_START_TYPE);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_TYPE;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_STAT_OBJ_LM_READ_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_STAT_OBJ_LM_READ_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_STAT_OBJ_LM_READ_INDEX;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_TM_ACTION_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_TM_ACTION_TYPE);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_TM_ACTION_TYPE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_INFO_KEY_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_LEARN_INFO_KEY_0);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_0;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_INFO_KEY_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_LEARN_INFO_KEY_1);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_1;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_INFO_PAYLOAD_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_LEARN_INFO_PAYLOAD_0);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_0;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_INFO_PAYLOAD_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_LEARN_INFO_PAYLOAD_1);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_1;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_LEARN_INFO_FORMAT_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_LEARN_INFO_FORMAT_INDEX);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_FORMAT_INDEX;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_INVALIDATE_NEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_INVALIDATE_NEXT);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_INVALIDATE_NEXT;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_ACTION_ID_INVALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_ID_INVALID);
        data->id = DBAL_ENUM_FVAL_IPMF3_ACTION_INVALID;
        data->size = 0;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_DISCARD < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_DISCARD);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_DISCARD;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_TC_MAP_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_TC_MAP_PROFILE);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_TC_MAP_PROFILE;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_TC < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_TC);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_TC;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_DP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_DP);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_DP;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_CUD_OUTLIF_OR_MCDB_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_CUD_OUTLIF_OR_MCDB_PTR);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_CUD_OUTLIF_OR_MCDB_PTR;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_PP_DSP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_PP_DSP);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_PP_DSP;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_FTMH_PP_DSP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_FTMH_PP_DSP);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_PP_DSP;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_COS_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_COS_PROFILE);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_COS_PROFILE;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_MIRROR_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_MIRROR_PROFILE);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_MIRROR_PROFILE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_SNOOP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_SNOOP);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_PROFILE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_SNOOP_STRENGTH);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_STRENGTH;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_TRAP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_TRAP);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_FWD_ACTION_PROFILE_INDEX;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_FWD_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_FWD_STRENGTH);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_FWD_STRENGTH;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_PMF_FWD_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_PMF_FWD_STRENGTH);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_STRENGTH;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_PMF_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_PMF_SNOOP_STRENGTH);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_STRENGTH;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_PMF_PROFILE);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_PROFILE;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_PMF_SNOOP_OVERRIDE_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_PMF_SNOOP_OVERRIDE_ENABLE);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_OVERRIDE_ENABLE;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_PMF_FWD_OVERRIDE_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_PMF_FWD_OVERRIDE_ENABLE);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_OVERRIDE_ENABLE;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_PMF_COUNTER_0_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_PMF_COUNTER_0_PROFILE);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PROFILE;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_PMF_COUNTER_0_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_PMF_COUNTER_0_PTR);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PTR;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_PMF_COUNTER_1_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_PMF_COUNTER_1_PROFILE);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PROFILE;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_PMF_COUNTER_1_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_PMF_COUNTER_1_PTR);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PTR;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_EBTR_EOP_DELTA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_EBTR_EOP_DELTA);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_EOP;
        data->size = 10;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_EBTA_EOP_DELTA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_EBTA_EOP_DELTA);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_EOP;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_EBTA_SOP_DELTA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_EBTA_SOP_DELTA);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_SOP;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_EBTR_SOP_DELTA < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_EBTR_SOP_DELTA);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_SOP;
        data->size = 10;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_INVALIDATE_NEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_INVALIDATE_NEXT);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_INVALIDATE_NEXT;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_ACTION_ID_INVALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_ACTION_ID_INVALID);
        data->id = DBAL_ENUM_FVAL_EPMF_ACTION_INVALID;
        data->size = 0;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_PP_DSP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_PP_DSP);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PP_DSP;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_TC < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_TC);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TC;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_DP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_DP);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_FTMH_DP;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_TM_ACTION_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_TM_ACTION_TYPE);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TM_ACTION_TYPE;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_CNI < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_CNI);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_FTMH_CNI;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_ECN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_ECN_ENABLE);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_FTMH_ECN_ENABLE;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_VISIBILITY);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_FTMH_VISIBILITY;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_FWD_DOMAIN);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARD_DOMAIN;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_NWK_QOS);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PPH_NWK_QOS;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_PPH_VALUE1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_PPH_VALUE1);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE1;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_PPH_VALUE2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_PPH_VALUE2);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE2;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_FHEI_EXT31TO00 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_FHEI_EXT31TO00);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT31TO00;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_FHEI_EXT63TO32 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_FHEI_EXT63TO32);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT63TO32;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_PPH_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_PPH_TTL);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PPH_TTL;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_PPH_IN_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_PPH_IN_LIF);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_PPH_IN_LIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_PPH_IN_LIF_PROFILE);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF_PROFILE;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_OUT_LIF0_CUD_OUT_LIF0VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_OUT_LIF0_CUD_OUT_LIF0VALID);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF0_CUD_OUT_LIF0VALID;
        data->size = 23;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_OUT_LIF1_CUD_OUT_LIF1VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_OUT_LIF1_CUD_OUT_LIF1VALID);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF1_CUD_OUT_LIF1VALID;
        data->size = 23;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_OUT_LIF2_CUD_OUT_LIF2VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_OUT_LIF2_CUD_OUT_LIF2VALID);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF2_CUD_OUT_LIF2VALID;
        data->size = 23;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_OUT_LIF3_CUD_OUT_LIF3VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_OUT_LIF3_CUD_OUT_LIF3VALID);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF3_CUD_OUT_LIF3VALID;
        data->size = 23;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_FWD_LAYER_ADDITIONAL_INFO);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARDING_LAYER_ADDITIONAL_INFO;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_FWD_LAYER_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_FWD_LAYER_INDEX);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PPH_FWD_LAYER_INDEX;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_FWD_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_FWD_STRENGTH);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWANDING_STRENGTH;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_END_OF_PACKET_EDITING < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_END_OF_PACKET_EDITING);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PPH_END_OF_PACKET_EDITING;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_PPH_APPLICATION_SPECIFIC_EXT_31TO00 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_PPH_APPLICATION_SPECIFIC_EXT_31TO00);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_31TO00;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_PPH_APPLICATION_SPECIFIC_EXT_47TO32 < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_PPH_APPLICATION_SPECIFIC_EXT_47TO32);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_47TO32;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_SNIFF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_SNIFF_PROFILE);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_SNIFF_PROFILE;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_SRC_SYS_PORT);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_FTMH_SOURCE_SYSTEM_PORT;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_ACE_CONTEXT_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_ACE_CONTEXT_VALUE);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_ACE_CONTEXT_VALUE;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_ACE_STAMP_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_ACE_STAMP_VALUE);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAMP_VALUE;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_ACE_STAT_METER_OBJ_CMD_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_ACE_STAT_METER_OBJ_CMD_ID);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_METER_OBJ_CMD_ID;
        data->size = 25;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_ACE_STAT_OBJ_CMD_ID_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_ACE_STAT_OBJ_CMD_ID_VALID);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_OBJ_CMD_ID_VALID;
        data->size = 26;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_TRAP < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_TRAP);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_ACE_FWD_ACTION_PROFILE_VALID_PROFILE;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_INVALIDATE_NEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_INVALIDATE_NEXT);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_INVALIDATE_NEXT;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_ACE < table->keys[0].size && DNX_FIELD_ACTION_ID_INVALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_ACE, DNX_FIELD_ACTION_ID_INVALID);
        data->id = DBAL_ENUM_FVAL_ACE_ACTION_INVALID;
        data->size = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2x_a0_dnx_data_field_map_qual_params_set(
    int unit)
{
    int stage_index;
    int qual_index;
    dnx_data_field_map_qual_params_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field_map;
    int submodule_index = dnx_data_field_map_submodule_qual;
    int table_index = dnx_data_field_map_qual_table_params;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_FIELD_STAGE_NOF;
    table->info_get.key_size[0] = DNX_FIELD_STAGE_NOF;
    table->keys[1].size = DNX_FIELD_QUAL_NOF;
    table->info_get.key_size[1] = DNX_FIELD_QUAL_NOF;

    
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_map_qual_params_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_field_map_qual_table_params");

    
    default_data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->offset = -1;
    default_data->size = -1;
    
    for (stage_index = 0; stage_index < table->keys[0].size; stage_index++)
    {
        for (qual_index = 0; qual_index < table->keys[1].size; qual_index++)
        {
            data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index, qual_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_LAYER_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_LAYER_INDEX);
        data->offset = 1408;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_ACTION_STRENGTH);
        data->offset = 1411;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE);
        data->offset = 1415;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_SNOOP_STRENGTH);
        data->offset = 1425;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_SNOOP_CODE);
        data->offset = 1429;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_0);
        data->offset = 1440;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_1);
        data->offset = 1440+160;
        data->size = 96;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_2);
        data->offset = 1440+160+96;
        data->size = 40;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_3);
        data->offset = 1440+256+40;
        data->size = 40;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_RPF_ROUTE_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_RPF_ROUTE_VALID);
        data->offset = 1950;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_PROFILE);
        data->offset = 1951;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_RESULT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_RESULT);
        data->offset = 1958;
        data->size = 96;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_PROFILE);
        data->offset = 2054;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_RESULT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_RESULT);
        data->offset = 2061;
        data->size = 96;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_MP_PROFILE_SEL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_MP_PROFILE_SEL);
        data->offset = 2157;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND);
        data->offset = 2158;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ACL_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ACL_CONTEXT);
        data->offset = 2159;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_CONTEXT);
        data->offset = 2165;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_NASID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_NASID);
        data->offset = 2171;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ENABLE_PP_INJECT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ENABLE_PP_INJECT);
        data->offset = 2176;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_UNKNOWN_ADDRESS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_UNKNOWN_ADDRESS);
        data->offset = 2177;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_EEI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_EEI);
        data->offset = 2178;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE);
        data->offset = 2202;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL);
        data->offset = 2203;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_DESTINATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_ACTION_DESTINATION);
        data->offset = 2222;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_DOMAIN);
        data->offset = 2243;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VRF_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VRF_VALUE);
        data->offset = 2243;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE);
        data->offset = 2261;
        data->size = 10;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE);
        data->offset = 2261+dnx_data_lif.in_lif.eth_rif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_eth_rif_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO);
        data->offset = 2271;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IEEE1588_COMMAND);
        data->offset = 2275;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP);
        data->offset = 2277;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION);
        data->offset = 2278;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET);
        data->offset = 2279;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IN_LIF_PROFILE_0);
        data->offset = 2287+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IN_LIF_PROFILE_1);
        data->offset = 2295+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GLOB_IN_LIF_0);
        data->offset = 2303;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GLOB_IN_LIF_1);
        data->offset = 2325;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PP_PORT);
        data->offset = 2347;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE);
        data->offset = 2356;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE);
        data->offset = 2361;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_LEARN_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_LEARN_INFO);
        data->offset = 2362;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_MEM_SOFT_ERR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_MEM_SOFT_ERR);
        data->offset = 2522;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_NWK_QOS);
        data->offset = 2523;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_OAM_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_OAM_OFFSET);
        data->offset = 2531;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_OAM_STAMP_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_OAM_STAMP_OFFSET);
        data->offset = 2539;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_OAM_OPCODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_OAM_OPCODE);
        data->offset = 2547;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_OAM_YOUR_DISCR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_OAM_YOUR_DISCR);
        data->offset = 2551;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_MY_CFM_MAC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_MY_CFM_MAC);
        data->offset = 2552;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_OAM < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PACKET_IS_OAM);
        data->offset = 2553;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_BFD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PACKET_IS_BFD);
        data->offset = 2554;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_MDL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_MDL);
        data->offset = 2555;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_LIF_0);
        data->offset = 2558;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_LIF_0_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_LIF_0_VALID);
        data->offset = 2580;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_LIF_1);
        data->offset = 2581;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_LIF_1_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_LIF_1_VALID);
        data->offset = 2603;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_LIF_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_LIF_2);
        data->offset = 2604;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_LIF_2_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_LIF_2_VALID);
        data->offset = 2626;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_PCP_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_PCP_0);
        data->offset = 2627;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_PCP_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_PCP_1);
        data->offset = 2630;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_PCP_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_PCP_2);
        data->offset = 2633;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GLOB_OUT_LIF_0);
        data->offset = 2636;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GLOB_OUT_LIF_1);
        data->offset = 2658;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PRT_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PRT_QUALIFIER);
        data->offset = 2680;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PACKET_HEADER_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PACKET_HEADER_SIZE);
        data->offset = 2683;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_APPLET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PACKET_IS_APPLET);
        data->offset = 2692;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC);
        data->offset = 2693;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_IEEE1588 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PACKET_IS_IEEE1588);
        data->offset = 2694;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_DP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_DP);
        data->offset = 2695;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ECN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ECN);
        data->offset = 2697;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_TC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_TC);
        data->offset = 2700;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PTC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PTC);
        data->offset = 2703;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_RPF_DST);
        data->offset = 2712;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_RPF_DST_VALID);
        data->offset = 2733;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_MIRROR_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_MIRROR_CODE);
        data->offset = 2734;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE);
        data->offset = 2743;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_SRC_SYS_PORT);
        data->offset = 2752;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ID_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ID_0);
        data->offset = 2768;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ID_1);
        data->offset = 2788;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ID_2);
        data->offset = 2808;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ID_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ID_3);
        data->offset = 2828;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ID_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ID_4);
        data->offset = 2848;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ID_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ID_5);
        data->offset = 2868;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ATR_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ATR_0);
        data->offset = 2888;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ATR_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ATR_1);
        data->offset = 2893;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ATR_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ATR_2);
        data->offset = 2898;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ATR_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ATR_3);
        data->offset = 2903;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ATR_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ATR_4);
        data->offset = 2908;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ATR_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ATR_5);
        data->offset = 2915;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_CMD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VLAN_EDIT_CMD);
        data->offset = 2918;
        data->size = 39;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_TRACE_PACKET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_TRACE_PACKET);
        data->offset = 2957;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IN_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IN_TTL);
        data->offset = 2958;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_LIF_OAM_TRAP_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_LIF_OAM_TRAP_PROFILE);
        data->offset = 2966;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VISIBILITY_CLEAR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VISIBILITY_CLEAR);
        data->offset = 2968;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VISIBILITY);
        data->offset = 2969;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_RPF_OUT_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_RPF_OUT_LIF);
        data->offset = 2970;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ACTION_PROFILE_SA_DROP_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ACTION_PROFILE_SA_DROP_INDEX);
        data->offset = 2992;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ACTION_PROFILE_SA_NOT_FOUND_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ACTION_PROFILE_SA_NOT_FOUND_INDEX);
        data->offset = 2994;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ACTION_PROFILE_DA_NOT_FOUND_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ACTION_PROFILE_DA_NOT_FOUND_INDEX);
        data->offset = 2996;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_SAVED_CONTEXT_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_SAVED_CONTEXT_PROFILE);
        data->offset = 2997;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_VERSION_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_VERSION_ERROR);
        data->offset = 3003;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_CHECKSUM_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_CHECKSUM_ERROR);
        data->offset = 3004;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_TOTAL_LENGTH_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_TOTAL_LENGTH_ERROR);
        data->offset = 3005;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_OPTIONS_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_OPTIONS_ERROR);
        data->offset = 3006;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_SIP_EQUAL_DIP_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_SIP_EQUAL_DIP_ERROR);
        data->offset = 3007;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_DIP_ZERO_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_DIP_ZERO_ERROR);
        data->offset = 3008;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_SIP_IS_MC_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_SIP_IS_MC_ERROR);
        data->offset = 3009;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_HEADER_LENGTH_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_HEADER_LENGTH_ERROR);
        data->offset = 3010;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_UNKNOWN_DA_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IN_LIF_UNKNOWN_DA_PROFILE);
        data->offset = 3011;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_NOF_VALID_LM_LIFS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_NOF_VALID_LM_LIFS);
        data->offset = 3013;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IS_IN_LIF_VALID_FOR_LM < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IS_IN_LIF_VALID_FOR_LM);
        data->offset = 3015;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_STRENGTH);
        data->offset = 3016;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_STRENGTH);
        data->offset = 3021;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_FOUND);
        data->offset = 3026;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_FOUND);
        data->offset = 3027;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_META_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_STATISTICS_META_DATA);
        data->offset = 3028;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ_CMD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ_CMD);
        data->offset = 3044;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ);
        data->offset = 3049;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VSI_UNKNOWN_DA_DESTINATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VSI_UNKNOWN_DA_DESTINATION);
        data->offset = 3069;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_INGRESS_PROTOCOL_TRAP_IN_LIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_INGRESS_PROTOCOL_TRAP_IN_LIF_PROFILE);
        data->offset = 3090;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ECMP_LB_KEY_0);
        data->offset = 3098;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ECMP_LB_KEY_1);
        data->offset = 3114;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ECMP_LB_KEY_2);
        data->offset = 3130;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_LAG_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_LAG_LB_KEY);
        data->offset = 3146;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_NWK_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_NWK_LB_KEY);
        data->offset = 3162;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_TM_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_TM_PORT_PMF_PROFILE);
        data->offset = 1866;
        data->size = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT_PMF_GENERAL_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PP_PORT_PMF_GENERAL_DATA);
        data->offset = 1908+dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
        data->size = 64-dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE);
        data->offset = 1908;
        data->size = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ENCODER3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ENCODER3);
        data->offset = 3385;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ENCODER2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ENCODER2);
        data->offset = 3377;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ENCODER1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ENCODER1);
        data->offset = 3369;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ENCODER0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ENCODER0);
        data->offset = 3361;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_UNKNOWN_ADDRESS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_UNKNOWN_ADDRESS);
        data->offset = 3360;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ALL_ONES < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ALL_ONES);
        data->offset = 3344;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_MEM_SOFT_ERR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_MEM_SOFT_ERR);
        data->offset = 3343;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ACL_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ACL_CONTEXT);
        data->offset = 3337;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_NASID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_NASID);
        data->offset = 3332;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_CONTEXT);
        data->offset = 3326;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_RPF_OUT_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_RPF_OUT_LIF);
        data->offset = 3304;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND);
        data->offset = 3303;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_RPF_ROUTE_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_RPF_ROUTE_VALID);
        data->offset = 3302;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_APPLET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PACKET_IS_APPLET);
        data->offset = 3301;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_2);
        data->offset = 3173;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_1);
        data->offset = 3045;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0);
        data->offset = 2917;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_DOMAIN);
        data->offset = 2899;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VRF_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VRF_VALUE);
        data->offset = 2899;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE);
        data->offset = 2889+dnx_data_lif.in_lif.eth_rif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_eth_rif_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE);
        data->offset = 2889;
        data->size = 10;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_PCP_2);
        data->offset = 2886;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_DEI_2);
        data->offset = 2885;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_2);
        data->offset = 2885;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_VID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_VID_2);
        data->offset = 2874;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_PCP_1);
        data->offset = 2870;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_DEI_1);
        data->offset = 2869;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_1);
        data->offset = 2869;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_VID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_VID_1);
        data->offset = 2857;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_CMD_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_CMD_INDEX);
        data->offset = 2850;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_CMD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_CMD);
        data->offset = 2850;
        data->size = 39;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_SRC_SYS_PORT);
        data->offset = 2834;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_SNOOP_CODE);
        data->offset = 2825;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_SNOOP_STRENGTH);
        data->offset = 2822;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO);
        data->offset = 2818;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_RPF_DST);
        data->offset = 2797;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_RPF_DST_VALID);
        data->offset = 2796;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_IEEE1588 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PACKET_IS_IEEE1588);
        data->offset = 2795;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP);
        data->offset = 2794;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IEEE1588_COMMAND);
        data->offset = 2792;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION);
        data->offset = 2791;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET);
        data->offset = 2783;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC);
        data->offset = 2782;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PRT_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PRT_QUALIFIER);
        data->offset = 2779;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_HEADER_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PACKET_HEADER_SIZE);
        data->offset = 2770;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_OAM_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_OAM_ID);
        data->offset = 2751;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_OAM_UP_MEP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_OAM_UP_MEP);
        data->offset = 2750;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_OAM_SUB_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_OAM_SUB_TYPE);
        data->offset = 2746;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_OAM_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_OAM_OFFSET);
        data->offset = 2738;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_OAM_STAMP_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_OAM_STAMP_OFFSET);
        data->offset = 2730;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_MIRROR_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_MIRROR_CODE);
        data->offset = 2721;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VISIBILITY);
        data->offset = 2720;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXPECTED_WON < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_EXPECTED_WON);
        data->offset = 2715;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_FOUND);
        data->offset = 2714;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_MATCH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_MATCH);
        data->offset = 2713;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_STATION_MOVE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_STATION_MOVE);
        data->offset = 2712;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_VLAN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_VLAN);
        data->offset = 2695;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_VSI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_VSI);
        data->offset = 2677;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_SRC_MAC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_SRC_MAC);
        data->offset = 2629;
        data->size = 48;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_DATA);
        data->offset = 2563;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_INFO);
        data->offset = 2560;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE);
        data->offset = 2559;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE);
        data->offset = 2558;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GLOB_IN_LIF_1);
        data->offset = 2536;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GLOB_IN_LIF_0);
        data->offset = 2514;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IN_LIF_PROFILE_1);
        data->offset = 2506+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IN_LIF_PROFILE_0);
        data->offset = 2498+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_ACTION_STRENGTH);
        data->offset = 2494;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_DESTINATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_ACTION_DESTINATION);
        data->offset = 2473;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE);
        data->offset = 2464;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL);
        data->offset = 2445;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EEI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EEI);
        data->offset = 2421;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_0);
        data->offset = 2420;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_1);
        data->offset = 2419;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_2);
        data->offset = 2418;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_3);
        data->offset = 2417;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_4);
        data->offset = 2416;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_5);
        data->offset = 2415;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_6 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_6);
        data->offset = 2414;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_7 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_7);
        data->offset = 2413;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_1);
        data->offset = 2325;
        data->size = 88;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_0);
        data->offset = 2165;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PP_PORT);
        data->offset = 2156;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_CORE_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_CORE_ID);
        data->offset = 2155;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IN_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IN_PORT);
        data->offset = 2156;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PTC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PTC);
        data->offset = 2146;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LAG_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LAG_LB_KEY);
        data->offset = 2130;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_NWK_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_NWK_LB_KEY);
        data->offset = 2114;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ECMP_LB_KEY_2);
        data->offset = 2098;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ECMP_LB_KEY_1);
        data->offset = 2082;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ECMP_LB_KEY_0);
        data->offset = 2066;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GLOB_OUT_LIF_1);
        data->offset = 2044;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GLOB_OUT_LIF_0);
        data->offset = 2022;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IN_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IN_TTL);
        data->offset = 2014;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_BIER_STR_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_BIER_STR_OFFSET);
        data->offset = 2006;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_BIER_STR_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_BIER_STR_SIZE);
        data->offset = 2004;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_BIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PACKET_IS_BIER);
        data->offset = 2003;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_SAVED_CONTEXT_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_SAVED_CONTEXT_PROFILE);
        data->offset = 1997;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_OAM_METER_DISABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_OAM_METER_DISABLE);
        data->offset = 1996;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE);
        data->offset = 1991;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_TC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_TC);
        data->offset = 1988;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_DP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_DP);
        data->offset = 1986;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ECN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ECN);
        data->offset = 1983;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_NWK_QOS);
        data->offset = 1975;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_TRACE_PACKET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_TRACE_PACKET);
        data->offset = 1974;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_HEADER_SIZE_RANGE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PACKET_HEADER_SIZE_RANGE);
        data->offset = 1972;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IN_PORT_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IN_PORT_KEY_GEN_VAR);
        data->offset = 1908;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IN_PORT_MAPPED_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IN_PORT_MAPPED_PP_PORT);
        data->offset = 1898;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PTC_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PTC_KEY_GEN_VAR);
        data->offset = 1866;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VID_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VID_VALID);
        data->offset = 1865;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_L4OPS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_L4OPS);
        data->offset = 1841;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJECT10 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_OBJECT10);
        data->offset = 1838;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_CPU_TRAP_CODE_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_CPU_TRAP_CODE_PROFILE);
        data->offset = 1837;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_TM_FLOW_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_TM_FLOW_ID);
        data->offset = 1817;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EXT_STATISTICS_ID_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXT_STATISTICS_ID_3);
        data->offset = 1797;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EXT_STATISTICS_ID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXT_STATISTICS_ID_2);
        data->offset = 1777;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EXT_STATISTICS_ID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXT_STATISTICS_ID_1);
        data->offset = 1757;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EXT_STATISTICS_ID_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXT_STATISTICS_ID_0);
        data->offset = 1737;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ID_9 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ID_9);
        data->offset = 1717;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ID_8 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ID_8);
        data->offset = 1697;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ID_7 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ID_7);
        data->offset = 1677;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ID_6 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ID_6);
        data->offset = 1657;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ID_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ID_5);
        data->offset = 1637;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ID_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ID_4);
        data->offset = 1617;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ID_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ID_3);
        data->offset = 1597;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ID_2);
        data->offset = 1577;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ID_1);
        data->offset = 1557;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ID_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ID_0);
        data->offset = 1537;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_TM_FLOW_ATR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_TM_FLOW_ATR);
        data->offset = 1532;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EXT_STATISTICS_ATR_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXT_STATISTICS_ATR_3);
        data->offset = 1527;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EXT_STATISTICS_ATR_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXT_STATISTICS_ATR_2);
        data->offset = 1522;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EXT_STATISTICS_ATR_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXT_STATISTICS_ATR_1);
        data->offset = 1517;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EXT_STATISTICS_ATR_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXT_STATISTICS_ATR_0);
        data->offset = 1512;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ATR_9 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ATR_9);
        data->offset = 1507;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ATR_8 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ATR_8);
        data->offset = 1502;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ATR_7 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ATR_7);
        data->offset = 1497;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ATR_6 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ATR_6);
        data->offset = 1492;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ATR_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ATR_5);
        data->offset = 1487;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ATR_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ATR_4);
        data->offset = 1482;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ATR_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ATR_3);
        data->offset = 1477;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ATR_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ATR_2);
        data->offset = 1472;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ATR_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ATR_1);
        data->offset = 1467;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_ATR_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_ATR_0);
        data->offset = 1462;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_META_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_STATISTICS_META_DATA);
        data->offset = 1446;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_CONTEXT_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_CONTEXT_KEY_GEN_VAR);
        data->offset = 1414;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EGRESS_PARSING_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EGRESS_PARSING_INDEX);
        data->offset = 1411;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_LAYER_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_LAYER_INDEX);
        data->offset = 1408;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_PMF1_TCAM_ACTION_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_PMF1_TCAM_ACTION_0);
        data->offset = 1322;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_PMF1_TCAM_ACTION_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_PMF1_TCAM_ACTION_1);
        data->offset = 1258;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_PMF1_TCAM_ACTION_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_PMF1_TCAM_ACTION_2);
        data->offset = 1194;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_PMF1_TCAM_ACTION_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_PMF1_TCAM_ACTION_3);
        data->offset = 1130;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_PMF1_EXEM_ACTION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_PMF1_EXEM_ACTION);
        data->offset = (1066+4);
        data->size = 60;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_PMF1_DIRECT_ACTION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_PMF1_DIRECT_ACTION);
        data->offset = 946;
        data->size = 120;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_STATE_DATA_WRITE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_STATE_DATA_WRITE);
        data->offset = 938;
        data->size = dnx_data_field.state_table.data_size_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_STATE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_STATE_DATA);
        data->offset = 925+(dnx_data_field.state_table.address_size_entry_max_get(unit)-dnx_data_field.state_table.data_size_entry_max_get(unit));
        data->size = dnx_data_field.state_table.data_size_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_KEY_F_INITIAL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_KEY_F_INITIAL);
        data->offset = 765;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_KEY_G_INITIAL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_KEY_G_INITIAL);
        data->offset = 605;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_KEY_H_INITIAL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_KEY_H_INITIAL);
        data->offset = 445;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_KEY_I_INITIAL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_KEY_I_INITIAL);
        data->offset = 285;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_KEY_J_INITIAL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_KEY_J_INITIAL);
        data->offset = 125;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_LAG_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_LAG_LB_KEY);
        data->offset = 109;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_NWK_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_NWK_LB_KEY);
        data->offset = 93;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_ECMP_LB_KEY_2);
        data->offset = 77;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_ECMP_LB_KEY_1);
        data->offset = 61;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_ECMP_LB_KEY_0);
        data->offset = 45;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_TRJ_HASH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_TRJ_HASH);
        data->offset = 29;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_CMP_KEY_0_DECODED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_CMP_KEY_0_DECODED);
        data->offset = 23;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_CMP_KEY_1_DECODED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_CMP_KEY_1_DECODED);
        data->offset = 17;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_CMP_KEY_TCAM_0_DECODED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_CMP_KEY_TCAM_0_DECODED);
        data->offset = 11;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_CMP_KEY_TCAM_1_DECODED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_CMP_KEY_TCAM_1_DECODED);
        data->offset = 5;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_NASID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_NASID);
        data->offset = 0;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_TM_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_TM_PORT_PMF_PROFILE);
        data->offset = 437;
        data->size = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT_PMF_GENERAL_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PP_PORT_PMF_GENERAL_DATA);
        data->offset = 373+dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
        data->size = 64-dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE);
        data->offset = 373;
        data->size = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_BTC_MINUS_BTK < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_BTC_MINUS_BTK);
        data->offset = 1843;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_BTC_OFFSET_SOP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_BTC_OFFSET_SOP);
        data->offset = 1834;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_BTK_OFFSET_SOP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_BTK_OFFSET_SOP);
        data->offset = 1826;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_ECL_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_ECL_VALID);
        data->offset = 1825;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PRT_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PRT_QUALIFIER);
        data->offset = 1822;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_ELEPHANT_LEARN_NEEDED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_ELEPHANT_LEARN_NEEDED);
        data->offset = 1821;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_ELEPHANT_TRAP_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_ELEPHANT_TRAP_INFO);
        data->offset = 1725;
        data->size = 96;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_DUAL_QUEUE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_DUAL_QUEUE);
        data->offset = 1721;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_ALL_ONES < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_ALL_ONES);
        data->offset = 1705;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXPECTED_WON < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_EXPECTED_WON);
        data->offset = 1700;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_FOUND);
        data->offset = 1699;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_MATCH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_MATCH);
        data->offset = 1698;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_STATION_MOVE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_STATION_MOVE);
        data->offset = 1697;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_VLAN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_VLAN);
        data->offset = 1680;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_VSI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_VSI);
        data->offset = 1662;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_SRC_MAC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_SRC_MAC);
        data->offset = 1614;
        data->size = 48;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_DATA);
        data->offset = 1548;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_INFO);
        data->offset = 1545;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_RPF_ECMP_MODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_RPF_ECMP_MODE);
        data->offset = 1543;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_RPF_ECMP_GROUP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_RPF_ECMP_GROUP);
        data->offset = 1527;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_ADMT_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_ADMT_PROFILE);
        data->offset = 1524;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_BIER_STR_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_BIER_STR_OFFSET);
        data->offset = 1516;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_BIER_STR_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_BIER_STR_SIZE);
        data->offset = 1514;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_BYTES_TO_REMOVE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_BYTES_TO_REMOVE);
        data->offset = 1505;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_WEAK_TM_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_WEAK_TM_VALID);
        data->offset = 1504;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_WEAK_TM_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_WEAK_TM_PROFILE);
        data->offset = 1498;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_STAT_OBJ_LM_READ_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_STAT_OBJ_LM_READ_INDEX);
        data->offset = 1496;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_CONGESTION_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_CONGESTION_INFO);
        data->offset = 1493;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_END_OF_PACKET_EDITING < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_END_OF_PACKET_EDITING);
        data->offset = 1490;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_EEI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_EEI);
        data->offset = 1466;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE);
        data->offset = 1465;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL);
        data->offset = 1446;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE);
        data->offset = 1437;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_DP_METER_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_DP_METER_COMMAND);
        data->offset = 1433;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_DP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_DP);
        data->offset = 1431;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_TC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_TC);
        data->offset = 1428;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_DESTINATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_DESTINATION);
        data->offset = 1407;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_STRENGTH);
        data->offset = 1403;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_PAYLOAD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_PAYLOAD);
        data->offset = 1403;
        data->size = 62;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_DOMAIN);
        data->offset = 1385;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VRF_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VRF_VALUE);
        data->offset = 1385;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE);
        data->offset = 1375+dnx_data_lif.in_lif.eth_rif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_eth_rif_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE);
        data->offset = 1375;
        data->size = 10;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_PMF_CONTAINER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GENERAL_DATA_PMF_CONTAINER);
        data->offset = 1343;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GENERAL_DATA);
        data->offset = 1247;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IEEE1588_COMMAND);
        data->offset = 1245;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP);
        data->offset = 1244;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION);
        data->offset = 1243;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET);
        data->offset = 1235;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IN_LIF_PROFILE_1);
        data->offset = 1227+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IN_LIF_PROFILE_0);
        data->offset = 1219+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GLOB_IN_LIF_1);
        data->offset = 1197;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GLOB_IN_LIF_0);
        data->offset = 1175;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PP_PORT);
        data->offset = 1166;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_CORE_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_CORE_ID);
        data->offset = 1165;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IN_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IN_PORT);
        data->offset = 1166;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IN_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IN_TTL);
        data->offset = 1157;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE);
        data->offset = 1156;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_ITPP_DELTA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_ITPP_DELTA);
        data->offset = 1146;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LAG_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LAG_LB_KEY);
        data->offset = 1130;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LATENCY_FLOW_ID_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LATENCY_FLOW_ID_VALID);
        data->offset = 1129;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LATENCY_FLOW_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LATENCY_FLOW_ID);
        data->offset = 1110;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_MIRROR_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_MIRROR_CODE);
        data->offset = 1101;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_MIRROR_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_MIRROR_QUALIFIER);
        data->offset = 1093;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_NWK_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_NWK_LB_KEY);
        data->offset = 1077;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_NWK_QOS);
        data->offset = 1069;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_OAM_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_OAM_ID);
        data->offset = 1050;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_OAM_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_OAM_OFFSET);
        data->offset = 1042;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_OAM_STAMP_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_OAM_STAMP_OFFSET);
        data->offset = 1034;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_OAM_SUB_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_OAM_SUB_TYPE);
        data->offset = 1030;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_OAM_UP_MEP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_OAM_UP_MEP);
        data->offset = 1029;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GLOB_OUT_LIF_3);
        data->offset = 1007;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GLOB_OUT_LIF_2);
        data->offset = 985;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GLOB_OUT_LIF_1);
        data->offset = 963;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GLOB_OUT_LIF_0);
        data->offset = 941;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_APPLET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PACKET_IS_APPLET);
        data->offset = 940;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_NASID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_NASID);
        data->offset = 935;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_BIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PACKET_IS_BIER);
        data->offset = 934;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_IEEE1588 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PACKET_IS_IEEE1588);
        data->offset = 933;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_EGRESS_PARSING_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_EGRESS_PARSING_INDEX);
        data->offset = 930;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PTC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PTC);
        data->offset = 921;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_EXT_STATISTICS_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_EXT_STATISTICS_VALID);
        data->offset = 920;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PPH_RESERVED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PPH_RESERVED);
        data->offset = 917;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PPH_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PPH_TYPE);
        data->offset = 915;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_RPF_DST);
        data->offset = 894;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_RPF_DST_VALID);
        data->offset = 893;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SNOOP_CODE);
        data->offset = 884;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SNOOP_STRENGTH);
        data->offset = 881;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SNOOP_QUALIFIER);
        data->offset = 873;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SRC_SYS_PORT);
        data->offset = 857;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_ST_VSQ_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_ST_VSQ_PTR);
        data->offset = 848;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE);
        data->offset = 839;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICAL_SAMPLING_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_STATISTICAL_SAMPLING_QUALIFIER);
        data->offset = 831;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_STACKING_ROUTE_HISTORY_BITMAP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_STACKING_ROUTE_HISTORY_BITMAP);
        data->offset = 815;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_META_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_STATISTICS_META_DATA);
        data->offset = 799;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJECT10 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_STATISTICS_OBJECT10);
        data->offset = 796;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SYSTEM_HEADER_PROFILE_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SYSTEM_HEADER_PROFILE_INDEX);
        data->offset = 792;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_TM_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_TM_PROFILE);
        data->offset = 790;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_1);
        data->offset = 758;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_2);
        data->offset = 726;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_3);
        data->offset = 694;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_4);
        data->offset = 662;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_1_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_1_TYPE);
        data->offset = 660;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_2_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_2_TYPE);
        data->offset = 658;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_3_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_3_TYPE);
        data->offset = 656;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_4_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_4_TYPE);
        data->offset = 654;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADERS_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADERS_TYPE);
        data->offset = 654;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VISIBILITY);
        data->offset = 653;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_VID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_VID_2);
        data->offset = 641;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_VID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_VID_1);
        data->offset = 629;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_DEI_2);
        data->offset = 628;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_PCP_2);
        data->offset = 625;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_2);
        data->offset = 625;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_DEI_1);
        data->offset = 624;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_PCP_1);
        data->offset = 621;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_1);
        data->offset = 621;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_CMD_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_CMD_INDEX);
        data->offset = 614;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_CMD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_CMD);
        data->offset = 614;
        data->size = 39;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO);
        data->offset = 610;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SLB_LEARN_NEEDED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SLB_LEARN_NEEDED);
        data->offset = 609;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SLB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SLB_KEY);
        data->offset = 529;
        data->size = 80;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SLB_PAYLOAD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SLB_PAYLOAD);
        data->offset = 469;
        data->size = 60;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PTC_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PTC_KEY_GEN_VAR);
        data->offset = 437;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IN_PORT_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IN_PORT_KEY_GEN_VAR);
        data->offset = 373;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IN_PORT_MAPPED_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IN_PORT_MAPPED_PP_PORT);
        data->offset = 363;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_CONTEXT_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_CONTEXT_KEY_GEN_VAR);
        data->offset = 331;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FER_STATISTICS_OBJ < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FER_STATISTICS_OBJ);
        data->offset = 259;
        data->size = 72;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FER_STATISTICS_OBJ_ID_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FER_STATISTICS_OBJ_ID_0);
        data->offset = dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FER_STATISTICS_OBJ)->offset+(0*24);
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FER_STATISTICS_OBJ_ID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FER_STATISTICS_OBJ_ID_1);
        data->offset = dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FER_STATISTICS_OBJ)->offset+(1*24);
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FER_STATISTICS_OBJ_ID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FER_STATISTICS_OBJ_ID_2);
        data->offset = dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FER_STATISTICS_OBJ)->offset+(2*24);
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_LAYER_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_LAYER_INDEX);
        data->offset = 256;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_TM_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_TM_PORT_PMF_PROFILE);
        data->offset = 2455;
        data->size = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE);
        data->offset = 2507;
        data->size = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_FWD_LAYER_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_FWD_LAYER_INDEX);
        data->offset = 1174;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_RESERVED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_RESERVED);
        data->offset = 1177;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_VISIBILITY);
        data->offset = 1178;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_TM_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_TM_PROFILE);
        data->offset = 1179;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_ECN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_ECN_ENABLE);
        data->offset = 1181;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_CNI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_CNI);
        data->offset = 1182;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_BIER_BFR_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_BIER_BFR_EXT_PRESENT);
        data->offset = 1183;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_FLOW_ID_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_FLOW_ID_EXT_PRESENT);
        data->offset = 1184;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT);
        data->offset = 1185;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_TM_DESTINATION_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_TM_DESTINATION_EXT_PRESENT);
        data->offset = 1186;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_MCID_OR_OUTLIF_0_OR_MCDB_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_MCID_OR_OUTLIF_0_OR_MCDB_PTR);
        data->offset = 1187;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_TM_ACTION_IS_MC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_TM_ACTION_IS_MC);
        data->offset = 1209;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_PPH_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_PPH_PRESENT);
        data->offset = 1210;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_TSH_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_TSH_EXT_PRESENT);
        data->offset = 1211;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_TM_ACTION_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_TM_ACTION_TYPE);
        data->offset = 1212;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_DP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_DP);
        data->offset = 1214;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PP_DSP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PP_DSP);
        data->offset = 1216;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_SRC_SYS_PORT);
        data->offset = 1225;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_TC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_TC);
        data->offset = 1241;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_PACKET_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_PACKET_SIZE);
        data->offset = 1244;
        data->size = 14;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LB_KEY);
        data->offset = 1258;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_NWK_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_NWK_KEY);
        data->offset = 1266;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_STACKING_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_STACKING_EXT);
        data->offset = 1282;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_BIER_BFR_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_BIER_BFR_EXT);
        data->offset = 1298;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_TM_DESTINATION_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_TM_DESTINATION_EXT);
        data->offset = 1314;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_APPLICATION_SPECIFIC_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_APPLICATION_SPECIFIC_EXT);
        data->offset = 1338;
        data->size = 48;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_TSH_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_TSH_EXT);
        data->offset = 1386;
        data->size = 56;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FWD_ACTION_STRENGTH);
        data->offset = 1442;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_PARSING_START_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_PARSING_START_TYPE);
        data->offset = 1446;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_PARSING_START_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_PARSING_START_OFFSET);
        data->offset = 1451;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_LIF_EXT_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_LIF_EXT_TYPE);
        data->offset = 1458;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_FHEI_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_FHEI_SIZE);
        data->offset = 1461;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_LEARN_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_LEARN_EXT_PRESENT);
        data->offset = 1463;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_TTL);
        data->offset = 1464;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_NWK_QOS);
        data->offset = 1472;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_IN_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_IN_LIF);
        data->offset = 1480;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_FWD_DOMAIN);
        data->offset = 1502;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_VRF_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_VRF_VALUE);
        data->offset = 1502;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_IN_LIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_IN_LIF_PROFILE);
        data->offset = 1520;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_END_OF_PACKET_EDITING < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_END_OF_PACKET_EDITING);
        data->offset = 1528;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_FWD_LAYER_ADDITIONAL_INFO);
        data->offset = 1531;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_VALUE1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_VALUE1);
        data->offset = 1535;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_VALUE2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_VALUE2);
        data->offset = 1543;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FHEI_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FHEI_EXT);
        data->offset = 1551;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LEARN_EXT);
        data->offset = 1615;
        data->size = 152;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXT_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LEARN_EXT_DATA);
        data->offset = 1618;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXT_SRC_MAC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LEARN_EXT_SRC_MAC);
        data->offset = 1684;
        data->size = 48;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXT_VSI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LEARN_EXT_VSI);
        data->offset = 1732;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXT_VLAN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LEARN_EXT_VLAN);
        data->offset = 1750;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LIF_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LIF_EXT);
        data->offset = 1767;
        data->size = 72;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_4);
        data->offset = 1839;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_3);
        data->offset = 1871;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_2);
        data->offset = 1903;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_1);
        data->offset = 1935;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_4_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_4_TYPE);
        data->offset = 1967;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_3_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_3_TYPE);
        data->offset = 1969;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_2_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_2_TYPE);
        data->offset = 1971;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_1_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_1_TYPE);
        data->offset = 1973;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_IS_TDM < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_IS_TDM);
        data->offset = 1975;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_CUD_OUTLIF_OR_MCDB_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_CUD_OUTLIF_OR_MCDB_PTR);
        data->offset = 1976;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_DISCARD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_DISCARD);
        data->offset = 1998;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_OUT_LIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_OUT_LIF_PROFILE);
        data->offset = 1999;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE);
        data->offset = 2002;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_ETH_TAG_FORMAT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_ETH_TAG_FORMAT);
        data->offset = 2003;
        data->size = 10;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_UPDATED_TPIDS_PACKET_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_UPDATED_TPIDS_PACKET_DATA);
        data->offset = 2013;
        data->size = 144;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_IVE_BYTES_TO_ADD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_IVE_BYTES_TO_ADD);
        data->offset = 2157;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_IVE_BYTES_TO_REMOVE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_IVE_BYTES_TO_REMOVE);
        data->offset = 2162;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_ETHER_TYPE_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_ETHER_TYPE_CODE);
        data->offset = 2167;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FABRIC_OR_EGRESS_MC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FABRIC_OR_EGRESS_MC);
        data->offset = 2183;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FWD_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FWD_CONTEXT);
        data->offset = 2184;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_GLOB_OUT_LIF_0);
        data->offset = 2190;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_GLOB_OUT_LIF_1);
        data->offset = 2212;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_GLOB_OUT_LIF_2);
        data->offset = 2234;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_GLOB_OUT_LIF_3);
        data->offset = 2256;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_GLOB_IN_LIF_0);
        data->offset = 2278;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_IN_LIF_PROFILE_0);
        data->offset = 2300+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_IP_MC_ELIGIBLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_IP_MC_ELIGIBLE);
        data->offset = 2308;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_IP_MC_SHOULD_BE_BRIDGED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_IP_MC_SHOULD_BE_BRIDGED);
        data->offset = 2309;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LEARN_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LEARN_VALID);
        data->offset = 2310;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LOCAL_OUT_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LOCAL_OUT_LIF);
        data->offset = 2311;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_PROFILE_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FWD_ACTION_PROFILE_INDEX);
        data->offset = 2331;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_ACTION_PROFILE_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_SNOOP_ACTION_PROFILE_INDEX);
        data->offset = 2334;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_SNOOP_STRENGTH);
        data->offset = 2336;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_SYSTEM_HEADERS_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_SYSTEM_HEADERS_SIZE);
        data->offset = 2339;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_DST_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_DST_SYS_PORT);
        data->offset = 2346;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_OUT_TM_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_OUT_TM_PORT);
        data->offset = 2362;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_OUT_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_OUT_PP_PORT);
        data->offset = 2371;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_APPLET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PACKET_IS_APPLET);
        data->offset = 2380;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_MEM_SOFT_ERR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_MEM_SOFT_ERR);
        data->offset = 2381;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_TC_MAP_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_TC_MAP_PROFILE);
        data->offset = 2382;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_L4_PORT_IN_RANGE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_L4_PORT_IN_RANGE);
        data->offset = 2386;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PROGRAM_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PROGRAM_INDEX);
        data->offset = 2410;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PER_PORT_TABLE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PER_PORT_TABLE_DATA);
        data->offset = 2416;
        data->size = 145;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_DP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_DP);
        data->offset = 2592;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_ECN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_ECN);
        data->offset = 2589;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_EEI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_EEI);
        data->offset = 2565;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_ELK_LKP_HIT_0);
        data->offset = 2564;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_ELK_LKP_HIT_1);
        data->offset = 2563;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_ELK_LKP_HIT_2);
        data->offset = 2562;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_ELK_LKP_HIT_3);
        data->offset = 2561;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_ELK_LKP_HIT_4);
        data->offset = 2560;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_ELK_LKP_HIT_5);
        data->offset = 2559;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_6 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_ELK_LKP_HIT_6);
        data->offset = 2558;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_7 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_ELK_LKP_HIT_7);
        data->offset = 2557;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_1);
        data->offset = 2469;
        data->size = 88;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_0);
        data->offset = 2309;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_FWD_DOMAIN);
        data->offset = 2291;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO);
        data->offset = 2287;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_GENERAL_DATA_2);
        data->offset = 2159;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_GENERAL_DATA_1);
        data->offset = 2031;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_GENERAL_DATA_0);
        data->offset = 1903;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_GLOB_IN_LIF_1);
        data->offset = 1881;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_GLOB_IN_LIF_0);
        data->offset = 1859;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_NWK_QOS);
        data->offset = 1851;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_GLOB_OUT_LIF_1);
        data->offset = 1829;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_GLOB_OUT_LIF_0);
        data->offset = 1807;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_PRT_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_PRT_QUALIFIER);
        data->offset = 1804;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_RPF_DST);
        data->offset = 1783;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_RPF_DST_VALID);
        data->offset = 1782;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_RPF_OUT_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_RPF_OUT_LIF);
        data->offset = 1760;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_SRC_SYS_PORT);
        data->offset = 1744;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJ_12 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_STATISTICS_OBJ_12);
        data->offset = 1724;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJ_11 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_STATISTICS_OBJ_11);
        data->offset = 1704;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJ_10 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_STATISTICS_OBJ_10);
        data->offset = 1684;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJ_9 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_STATISTICS_OBJ_9);
        data->offset = 1664;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJ_8 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_STATISTICS_OBJ_8);
        data->offset = 1644;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJ_7 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_STATISTICS_OBJ_7);
        data->offset = 1624;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJ_6 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_STATISTICS_OBJ_6);
        data->offset = 1604;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJ_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_STATISTICS_OBJ_5);
        data->offset = 1584;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJ_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_STATISTICS_OBJ_4);
        data->offset = 1564;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJ_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_STATISTICS_OBJ_3);
        data->offset = 1544;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJ_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_STATISTICS_OBJ_2);
        data->offset = 1524;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJ_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_STATISTICS_OBJ_1);
        data->offset = 1504;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJ_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_STATISTICS_OBJ_0);
        data->offset = 1484;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_OAM_STATISTICS_OBJ_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_OAM_STATISTICS_OBJ_2);
        data->offset = 1464;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_OAM_STATISTICS_OBJ_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_OAM_STATISTICS_OBJ_1);
        data->offset = 1444;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_OAM_STATISTICS_OBJ_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_OAM_STATISTICS_OBJ_0);
        data->offset = 1424;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_TC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_TC);
        data->offset = 1421;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_PP_PORT);
        data->offset = 1412;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_CORE_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_CORE_ID);
        data->offset = 1411;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_IN_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_IN_PORT);
        data->offset = 1412;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_L4_OPS < table->keys[0].size && DNX_FIELD_QUAL_FWD_LAYER_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_L4_OPS, DNX_FIELD_QUAL_FWD_LAYER_INDEX);
        data->offset = 1408;
        data->size = 3;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_field_map_qual_special_metadata_qual_set(
    int unit)
{
    int stage_index;
    int qual_index;
    dnx_data_field_map_qual_special_metadata_qual_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field_map;
    int submodule_index = dnx_data_field_map_submodule_qual;
    int table_index = dnx_data_field_map_qual_table_special_metadata_qual;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_FIELD_STAGE_NOF;
    table->info_get.key_size[0] = DNX_FIELD_STAGE_NOF;
    table->keys[1].size = DNX_FIELD_QUAL_NOF;
    table->info_get.key_size[1] = DNX_FIELD_QUAL_NOF;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "{-1}";
    table->values[2].default_val = "{-1}";
    table->values[3].default_val = "NULL";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_map_qual_special_metadata_qual_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_field_map_qual_table_special_metadata_qual");

    
    default_data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_parts = 0;
    for (int i = 0; i < DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS; i++)  
        default_data->offsets[i] = -1;
    for (int i = 0; i < DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS; i++)  
        default_data->sizes[i] = -1;
    default_data->vw_name = NULL;
    
    for (stage_index = 0; stage_index < table->keys[0].size; stage_index++)
    {
        for (qual_index = 0; qual_index < table->keys[1].size; qual_index++)
        {
            data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index, qual_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA);
        data->nof_parts = 2;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS, _SHR_E_INTERNAL, "out of bound access to array")
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40));
        data->offsets[1] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40 + 40));
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS, _SHR_E_INTERNAL, "out of bound access to array")
        data->sizes[0] = 4;
        data->sizes[1] = 24;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 - dnx_data_field.general_data_qualifiers.ac_in_lif_wide_data_extended_nof_bits_get(unit)));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.ac_in_lif_wide_data_extended_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_MACT_ENTRY_GROUPING < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_MACT_ENTRY_GROUPING);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.mact_entry_grouping_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VW_VIP_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VW_VIP_VALID);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + ((160 + 96 + 40 + (dnx_data_field.general_data_qualifiers.vw_member_reference_nof_bits_get(unit))) + dnx_data_field.general_data_qualifiers.vw_vip_id_nof_bits_get(unit)));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.vw_vip_valid_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VW_VIP_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VW_VIP_ID);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40 + (dnx_data_field.general_data_qualifiers.vw_member_reference_nof_bits_get(unit))));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.vw_vip_id_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VW_MEMBER_REFERENCE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VW_MEMBER_REFERENCE);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.vw_member_reference_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VW_PCC_HIT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VW_PCC_HIT);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40 + 40 + 24));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.vw_pcc_hit_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_L3SRCBIND_HIT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_L3SRCBIND_HIT);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40 + 40 + 24));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.l3srcbind_hit_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA);
        data->nof_parts = 2;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS, _SHR_E_INTERNAL, "out of bound access to array")
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40));
        data->offsets[1] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40 + 40));
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS, _SHR_E_INTERNAL, "out of bound access to array")
        data->sizes[0] = 4;
        data->sizes[1] = 24;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_NATIVE_AC_IN_LIF_WIDE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_NATIVE_AC_IN_LIF_WIDE_DATA);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40 + 40 + 24 + 24 - dnx_data_field.general_data_qualifiers.native_ac_in_lif_wide_data_nof_bits_get(unit)));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.native_ac_in_lif_wide_data_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 - dnx_data_field.general_data_qualifiers.ac_in_lif_wide_data_extended_nof_bits_get(unit)));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.ac_in_lif_wide_data_extended_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PWE_IN_LIF_WIDE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PWE_IN_LIF_WIDE_DATA);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40 - dnx_data_field.general_data_qualifiers.pwe_in_lif_wide_data_nof_bits_get(unit)));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.pwe_in_lif_wide_data_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_MACT_ENTRY_GROUPING < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_MACT_ENTRY_GROUPING);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.mact_entry_grouping_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VW_VIP_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VW_VIP_VALID);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + ((160 + 96 + 40 + (dnx_data_field.general_data_qualifiers.vw_member_reference_nof_bits_get(unit))) + dnx_data_field.general_data_qualifiers.vw_vip_id_nof_bits_get(unit)));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.vw_vip_valid_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VW_VIP_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VW_VIP_ID);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40 + (dnx_data_field.general_data_qualifiers.vw_member_reference_nof_bits_get(unit))));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.vw_vip_id_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VW_MEMBER_REFERENCE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VW_MEMBER_REFERENCE);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.vw_member_reference_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VW_PCC_HIT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VW_PCC_HIT);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40 + 40 + 24));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.vw_pcc_hit_nof_bits_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IP_TUNNEL_IN_LIF_WIDE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IP_TUNNEL_IN_LIF_WIDE_DATA);
        data->nof_parts = 2;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS, _SHR_E_INTERNAL, "out of bound access to array")
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40 - 8));
        data->offsets[1] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40 + 40 - 8));
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS, _SHR_E_INTERNAL, "out of bound access to array")
        data->sizes[0] = 8;
        data->sizes[1] = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_L3SRCBIND_HIT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_L3SRCBIND_HIT);
        data->nof_parts = 1;
        data->offsets[0] = (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset + (160 + 96 + 40 + 40 + 24));
        data->sizes[0] = dnx_data_field.general_data_qualifiers.l3srcbind_hit_nof_bits_get(unit);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_field_map_qual_layer_record_info_ingress_set(
    int unit)
{
    int layer_record_index;
    dnx_data_field_map_qual_layer_record_info_ingress_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field_map;
    int submodule_index = dnx_data_field_map_submodule_qual;
    int table_index = dnx_data_field_map_qual_table_layer_record_info_ingress;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_FIELD_LR_QUAL_NOF;
    table->info_get.key_size[0] = DNX_FIELD_LR_QUAL_NOF;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_map_qual_layer_record_info_ingress_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_field_map_qual_table_layer_record_info_ingress");

    
    default_data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    default_data->size = 0;
    default_data->offset = 0;
    
    for (layer_record_index = 0; layer_record_index < table->keys[0].size; layer_record_index++)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, layer_record_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_FIELD_LR_QUAL_OFFSET < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_OFFSET, 0);
        data->valid = TRUE;
        data->size = 8;
        data->offset = 0;
    }
    if (DNX_FIELD_LR_QUAL_PROTOCOL < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_PROTOCOL, 0);
        data->valid = TRUE;
        data->size = 5;
        data->offset = 24;
    }
    if (DNX_FIELD_LR_QUAL_QUALIFIER < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_QUALIFIER, 0);
        data->valid = TRUE;
        data->size = 16;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_ETH_IS_DA_MC < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_IS_DA_MC, 0);
        data->valid = dnx_data_field.dnx_data_internal.lr_eth_is_da_mac_valid_get(unit);
        data->size = 1;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_ETH_IS_DA_BC < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_IS_DA_BC, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_ETH_1ST_TPID_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_1ST_TPID_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_ETH_1ST_TPID_INDEX < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_1ST_TPID_INDEX, 0);
        data->valid = TRUE;
        data->size = 3;
        data->offset = 11;
    }
    if (DNX_FIELD_LR_QUAL_ETH_2ND_TPID_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_2ND_TPID_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 14;
    }
    if (DNX_FIELD_LR_QUAL_ETH_2ND_TPID_INDEX < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_2ND_TPID_INDEX, 0);
        data->valid = TRUE;
        data->size = 3;
        data->offset = 15;
    }
    if (DNX_FIELD_LR_QUAL_ETH_3RD_TPID_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_3RD_TPID_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 18;
    }
    if (DNX_FIELD_LR_QUAL_ETH_3RD_TPID_INDEX < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_3RD_TPID_INDEX, 0);
        data->valid = TRUE;
        data->size = 3;
        data->offset = 19;
    }
    if (DNX_FIELD_LR_QUAL_ETH_FRAME_TYPE < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_FRAME_TYPE, 0);
        data->valid = TRUE;
        data->size = 2;
        data->offset = 22;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_IS_MC < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_IS_MC, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_HAS_OPTION < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_HAS_OPTION, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_IS_FRAGMENTED < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_IS_FRAGMENTED, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_IS_1ST_FRAGMENT < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_IS_1ST_FRAGMENT, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 11;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 12;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 13;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_2ND_ADD_HEADER_EXISTS < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_2ND_ADD_HEADER_EXISTS, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 17;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 18;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE_RAW < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE_RAW, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 18;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_FRAGMENTED_NON_FIRST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_FRAGMENTED_NON_FIRST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 22;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_IS_MC < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_IS_MC, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER, 0);
        data->valid = TRUE;
        data->size = 5;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER_EXISTS < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER_EXISTS, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 15;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER, 0);
        data->valid = TRUE;
        data->size = 5;
        data->offset = 16;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_3RD_ADD_HEADER_EXISTS < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_3RD_ADD_HEADER_EXISTS, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 21;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_FRAGMENTED_NON_FIRST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_FRAGMENTED_NON_FIRST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 22;
    }
    if (DNX_FIELD_LR_QUAL_SRV6_EP_SEGMENT_LEFT_MINUS_ONE < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_SRV6_EP_SEGMENT_LEFT_MINUS_ONE, 0);
        data->valid = TRUE;
        data->size = 8;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_MPLS_NOF_LABELS < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_MPLS_NOF_LABELS, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_BIER_BITSTRINGLENGTH < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_BIER_BITSTRINGLENGTH, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_FCOE_IS_FIP < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_FCOE_IS_FIP, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_FCOE_IS_ENCAP < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_FCOE_IS_ENCAP, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_FCOE_VFT_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_FCOE_VFT_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_ITMH_PPH_TYPE < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ITMH_PPH_TYPE, 0);
        data->valid = TRUE;
        data->size = 2;
        data->offset = 8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_field_map_qual_layer_record_info_egress_set(
    int unit)
{
    int layer_record_index;
    dnx_data_field_map_qual_layer_record_info_egress_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field_map;
    int submodule_index = dnx_data_field_map_submodule_qual;
    int table_index = dnx_data_field_map_qual_table_layer_record_info_egress;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_FIELD_LR_QUAL_NOF;
    table->info_get.key_size[0] = DNX_FIELD_LR_QUAL_NOF;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_map_qual_layer_record_info_egress_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_field_map_qual_table_layer_record_info_egress");

    
    default_data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    default_data->size = 0;
    default_data->offset = 0;
    
    for (layer_record_index = 0; layer_record_index < table->keys[0].size; layer_record_index++)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, layer_record_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_FIELD_LR_QUAL_OFFSET < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_OFFSET, 0);
        data->valid = TRUE;
        data->size = 9;
        data->offset = 0;
    }
    if (DNX_FIELD_LR_QUAL_PROTOCOL < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_PROTOCOL, 0);
        data->valid = TRUE;
        data->size = 5;
        data->offset = 25;
    }
    if (DNX_FIELD_LR_QUAL_QUALIFIER < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_QUALIFIER, 0);
        data->valid = TRUE;
        data->size = 16;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_ETH_IS_DA_MC < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_IS_DA_MC, 0);
        data->valid = dnx_data_field.dnx_data_internal.lr_eth_is_da_mac_valid_get(unit);
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_ETH_IS_DA_BC < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_IS_DA_BC, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_ETH_1ST_TPID_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_1ST_TPID_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 11;
    }
    if (DNX_FIELD_LR_QUAL_ETH_1ST_TPID_INDEX < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_1ST_TPID_INDEX, 0);
        data->valid = TRUE;
        data->size = 3;
        data->offset = 12;
    }
    if (DNX_FIELD_LR_QUAL_ETH_2ND_TPID_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_2ND_TPID_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 15;
    }
    if (DNX_FIELD_LR_QUAL_ETH_2ND_TPID_INDEX < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_2ND_TPID_INDEX, 0);
        data->valid = TRUE;
        data->size = 3;
        data->offset = 16;
    }
    if (DNX_FIELD_LR_QUAL_ETH_3RD_TPID_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_3RD_TPID_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 19;
    }
    if (DNX_FIELD_LR_QUAL_ETH_3RD_TPID_INDEX < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_3RD_TPID_INDEX, 0);
        data->valid = TRUE;
        data->size = 3;
        data->offset = 20;
    }
    if (DNX_FIELD_LR_QUAL_ETH_FRAME_TYPE < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_FRAME_TYPE, 0);
        data->valid = TRUE;
        data->size = 2;
        data->offset = 23;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_IS_MC < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_IS_MC, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_HAS_OPTION < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_HAS_OPTION, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_IS_FRAGMENTED < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_IS_FRAGMENTED, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 11;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_IS_1ST_FRAGMENT < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_IS_1ST_FRAGMENT, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 12;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 13;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 14;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_2ND_ADD_HEADER_EXISTS < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_2ND_ADD_HEADER_EXISTS, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 18;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 19;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE_RAW < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE_RAW, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 19;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_FRAGMENTED_NON_FIRST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_FRAGMENTED_NON_FIRST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 23;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_IS_MC < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_IS_MC, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER, 0);
        data->valid = TRUE;
        data->size = 5;
        data->offset = 11;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER_EXISTS < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER_EXISTS, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 16;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER, 0);
        data->valid = TRUE;
        data->size = 5;
        data->offset = 17;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_3RD_ADD_HEADER_EXISTS < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_3RD_ADD_HEADER_EXISTS, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 22;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_FRAGMENTED_NON_FIRST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_FRAGMENTED_NON_FIRST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 23;
    }
    if (DNX_FIELD_LR_QUAL_SRV6_EP_SEGMENT_LEFT_MINUS_ONE < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_SRV6_EP_SEGMENT_LEFT_MINUS_ONE, 0);
        data->valid = TRUE;
        data->size = 8;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_MPLS_NOF_LABELS < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_MPLS_NOF_LABELS, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_BIER_BITSTRINGLENGTH < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_BIER_BITSTRINGLENGTH, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_FCOE_IS_FIP < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_FCOE_IS_FIP, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_FCOE_IS_ENCAP < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_FCOE_IS_ENCAP, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_FCOE_VFT_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_FCOE_VFT_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 11;
    }
    if (DNX_FIELD_LR_QUAL_ITMH_PPH_TYPE < table->keys[0].size)
    {
        data = (dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ITMH_PPH_TYPE, 0);
        data->valid = TRUE;
        data->size = 2;
        data->offset = 9;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2x_a0_data_field_map_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_field_map;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_field_map_submodule_action;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_field_map_action_table_params;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_field_map_action_params_set;
    
    submodule_index = dnx_data_field_map_submodule_qual;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_field_map_qual_table_params;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_field_map_qual_params_set;
    data_index = dnx_data_field_map_qual_table_special_metadata_qual;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_field_map_qual_special_metadata_qual_set;
    data_index = dnx_data_field_map_qual_table_layer_record_info_ingress;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_field_map_qual_layer_record_info_ingress_set;
    data_index = dnx_data_field_map_qual_table_layer_record_info_egress;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_field_map_qual_layer_record_info_egress_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

