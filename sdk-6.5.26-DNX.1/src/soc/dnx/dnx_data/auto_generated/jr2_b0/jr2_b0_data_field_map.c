
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
#include <bcm_int/dnx/field/field_map.h>









static shr_error_e
jr2_b0_dnx_data_field_map_qual_params_set(
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
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_RPF_ROUTE_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_RPF_ROUTE_VALID);
        data->offset = 1411;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_PROFILE);
        data->offset = 1412;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_RESULT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_RESULT);
        data->offset = 1419;
        data->size = 96;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_PROFILE);
        data->offset = 1515;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_RESULT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_RESULT);
        data->offset = 1522;
        data->size = 96;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_MP_PROFILE_SEL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_MP_PROFILE_SEL);
        data->offset = 1618;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND);
        data->offset = 1619;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ACL_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ACL_CONTEXT);
        data->offset = 1620;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_CONTEXT);
        data->offset = 1626;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_NASID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_NASID);
        data->offset = 1632;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ENABLE_PP_INJECT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ENABLE_PP_INJECT);
        data->offset = 1637;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_EEI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_EEI);
        data->offset = 1638;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE);
        data->offset = 1662;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE);
        data->offset = 1663;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL);
        data->offset = 1672;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_DESTINATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_ACTION_DESTINATION);
        data->offset = 1691;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_ACTION_STRENGTH);
        data->offset = 1712;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_DOMAIN);
        data->offset = 1716;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VRF_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VRF_VALUE);
        data->offset = 1716;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE);
        data->offset = 1734;
        data->size = 10;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE);
        data->offset = 1734+dnx_data_lif.in_lif.eth_rif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_eth_rif_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO);
        data->offset = 1744;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_0);
        data->offset = 1748;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_1);
        data->offset = 1876;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_2);
        data->offset = 1904;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GENERAL_DATA_3);
        data->offset = 2032;
        data->size = 126;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IEEE1588_COMMAND);
        data->offset = 2258;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP);
        data->offset = 2260;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION);
        data->offset = 2261;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET);
        data->offset = 2262;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IN_LIF_PROFILE_0);
        data->offset = 2270+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IN_LIF_PROFILE_1);
        data->offset = 2278+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GLOB_IN_LIF_0);
        data->offset = 2286;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GLOB_IN_LIF_1);
        data->offset = 2308;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PP_PORT);
        data->offset = 2330;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE);
        data->offset = 2338;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE);
        data->offset = 2343;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_LEARN_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_LEARN_INFO);
        data->offset = 2344;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_MEM_SOFT_ERR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_MEM_SOFT_ERR);
        data->offset = 2504;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_NWK_QOS);
        data->offset = 2505;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_OAM_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_OAM_OFFSET);
        data->offset = 2513;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_OAM_STAMP_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_OAM_STAMP_OFFSET);
        data->offset = 2521;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_OAM_OPCODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_OAM_OPCODE);
        data->offset = 2529;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_OAM_YOUR_DISCR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_OAM_YOUR_DISCR);
        data->offset = 2533;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_MY_CFM_MAC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_MY_CFM_MAC);
        data->offset = 2534;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_OAM < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PACKET_IS_OAM);
        data->offset = 2535;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_BFD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PACKET_IS_BFD);
        data->offset = 2536;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_MDL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_MDL);
        data->offset = 2537;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_LIF_0);
        data->offset = 2540;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_LIF_0_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_LIF_0_VALID);
        data->offset = 2562;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_LIF_1);
        data->offset = 2563;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_LIF_1_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_LIF_1_VALID);
        data->offset = 2585;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_LIF_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_LIF_2);
        data->offset = 2586;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_LIF_2_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_LIF_2_VALID);
        data->offset = 2608;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_PCP_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_PCP_0);
        data->offset = 2609;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_PCP_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_PCP_1);
        data->offset = 2612;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VTT_OAM_PCP_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VTT_OAM_PCP_2);
        data->offset = 2615;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GLOB_OUT_LIF_0);
        data->offset = 2618;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_GLOB_OUT_LIF_1);
        data->offset = 2640;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PRT_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PRT_QUALIFIER);
        data->offset = 2662;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PACKET_HEADER_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PACKET_HEADER_SIZE);
        data->offset = 2665;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_APPLET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PACKET_IS_APPLET);
        data->offset = 2673;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC);
        data->offset = 2674;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_IEEE1588 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PACKET_IS_IEEE1588);
        data->offset = 2675;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_DP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_DP);
        data->offset = 2676;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ECN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ECN);
        data->offset = 2678;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_TC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_TC);
        data->offset = 2680;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_PTC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_PTC);
        data->offset = 2683;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_RPF_DST);
        data->offset = 2691;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_RPF_DST_VALID);
        data->offset = 2712;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_SNOOP_CODE);
        data->offset = 2713;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_SNOOP_STRENGTH);
        data->offset = 2722;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_MIRROR_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_MIRROR_CODE);
        data->offset = 2725;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE);
        data->offset = 2734;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_SRC_SYS_PORT);
        data->offset = 2743;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ID_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ID_0);
        data->offset = 2759;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ID_1);
        data->offset = 2779;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ID_2);
        data->offset = 2799;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ID_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ID_3);
        data->offset = 2819;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ID_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ID_4);
        data->offset = 2839;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ID_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ID_5);
        data->offset = 2859;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ATR_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ATR_0);
        data->offset = 2879;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ATR_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ATR_1);
        data->offset = 2884;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ATR_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ATR_2);
        data->offset = 2889;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ATR_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ATR_3);
        data->offset = 2894;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ATR_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ATR_4);
        data->offset = 2899;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FLP_STATISTICS_ATR_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FLP_STATISTICS_ATR_5);
        data->offset = 2904;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_CMD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VLAN_EDIT_CMD);
        data->offset = 2909;
        data->size = 39;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_TRACE_PACKET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_TRACE_PACKET);
        data->offset = 2948;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IN_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IN_TTL);
        data->offset = 2949;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_LIF_OAM_TRAP_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_LIF_OAM_TRAP_PROFILE);
        data->offset = 2957;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VISIBILITY_CLEAR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VISIBILITY_CLEAR);
        data->offset = 2959;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VISIBILITY);
        data->offset = 2960;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_RPF_OUT_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_RPF_OUT_LIF);
        data->offset = 2961;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ACTION_PROFILE_SA_DROP_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ACTION_PROFILE_SA_DROP_INDEX);
        data->offset = 2983;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ACTION_PROFILE_SA_NOT_FOUND_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ACTION_PROFILE_SA_NOT_FOUND_INDEX);
        data->offset = 2985;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ACTION_PROFILE_DA_NOT_FOUND_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ACTION_PROFILE_DA_NOT_FOUND_INDEX);
        data->offset = 2987;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_SAVED_CONTEXT_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_SAVED_CONTEXT_PROFILE);
        data->offset = 2988;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_VERSION_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_VERSION_ERROR);
        data->offset = 2994;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_CHECKSUM_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_CHECKSUM_ERROR);
        data->offset = 2995;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_TOTAL_LENGTH_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_TOTAL_LENGTH_ERROR);
        data->offset = 2996;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_OPTIONS_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_OPTIONS_ERROR);
        data->offset = 2997;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_SIP_EQUAL_DIP_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_SIP_EQUAL_DIP_ERROR);
        data->offset = 2998;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_DIP_ZERO_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_DIP_ZERO_ERROR);
        data->offset = 2999;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_SIP_IS_MC_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_SIP_IS_MC_ERROR);
        data->offset = 3000;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IPV4_HEADER_LENGTH_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IPV4_HEADER_LENGTH_ERROR);
        data->offset = 3001;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_UNKNOWN_DA_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IN_LIF_UNKNOWN_DA_PROFILE);
        data->offset = 3002;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_NOF_VALID_LM_LIFS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_NOF_VALID_LM_LIFS);
        data->offset = 3004;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_IS_IN_LIF_VALID_FOR_LM < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_IS_IN_LIF_VALID_FOR_LM);
        data->offset = 3006;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_STRENGTH);
        data->offset = 3007;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_STRENGTH);
        data->offset = 3012;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_FOUND);
        data->offset = 3017;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_FOUND);
        data->offset = 3018;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_META_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_STATISTICS_META_DATA);
        data->offset = 3019;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ_CMD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ_CMD);
        data->offset = 3035;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ);
        data->offset = 3040;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_VSI_UNKNOWN_DA_DESTINATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_VSI_UNKNOWN_DA_DESTINATION);
        data->offset = 3060;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ECMP_LB_KEY_0);
        data->offset = 3081;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ECMP_LB_KEY_1);
        data->offset = 3097;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_ECMP_LB_KEY_2);
        data->offset = 3113;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_LAG_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_LAG_LB_KEY);
        data->offset = 3129;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_QUAL_NWK_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_QUAL_NWK_LB_KEY);
        data->offset = 3145;
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
        data->offset = 1907+dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
        data->size = 64-dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE);
        data->offset = 1907;
        data->size = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ALL_ONES < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ALL_ONES);
        data->offset = 3339;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_MEM_SOFT_ERR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_MEM_SOFT_ERR);
        data->offset = 3338;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ACL_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ACL_CONTEXT);
        data->offset = 3332;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_NASID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_NASID);
        data->offset = 3327;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_CONTEXT);
        data->offset = 3321;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_RPF_OUT_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_RPF_OUT_LIF);
        data->offset = 3299;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND);
        data->offset = 3298;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_RPF_ROUTE_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_RPF_ROUTE_VALID);
        data->offset = 3297;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_APPLET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PACKET_IS_APPLET);
        data->offset = 3296;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_2);
        data->offset = 3168;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_1);
        data->offset = 3040;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0);
        data->offset = 2912;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_DOMAIN);
        data->offset = 2894;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VRF_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VRF_VALUE);
        data->offset = 2894;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE);
        data->offset = 2884+dnx_data_lif.in_lif.eth_rif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_eth_rif_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE);
        data->offset = 2884;
        data->size = 10;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_PCP_2);
        data->offset = 2881;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_DEI_2);
        data->offset = 2880;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_2);
        data->offset = 2880;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_VID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_VID_2);
        data->offset = 2868;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_PCP_1);
        data->offset = 2865;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_DEI_1);
        data->offset = 2864;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_1);
        data->offset = 2864;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_VID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_VID_1);
        data->offset = 2852;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_CMD_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_CMD_INDEX);
        data->offset = 2845;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_CMD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VLAN_EDIT_CMD);
        data->offset = 2845;
        data->size = 39;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_SRC_SYS_PORT);
        data->offset = 2829;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_SNOOP_CODE);
        data->offset = 2820;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_SNOOP_STRENGTH);
        data->offset = 2817;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO);
        data->offset = 2813;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_UNKNOWN_ADDRESS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_UNKNOWN_ADDRESS);
        data->offset = 2813;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_RPF_DST);
        data->offset = 2792;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_RPF_DST_VALID);
        data->offset = 2791;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_IEEE1588 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PACKET_IS_IEEE1588);
        data->offset = 2790;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP);
        data->offset = 2789;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IEEE1588_COMMAND);
        data->offset = 2787;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION);
        data->offset = 2786;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET);
        data->offset = 2778;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC);
        data->offset = 2777;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PRT_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PRT_QUALIFIER);
        data->offset = 2774;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_HEADER_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PACKET_HEADER_SIZE);
        data->offset = 2766;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_OAM_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_OAM_ID);
        data->offset = 2747;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_OAM_UP_MEP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_OAM_UP_MEP);
        data->offset = 2746;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_OAM_SUB_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_OAM_SUB_TYPE);
        data->offset = 2742;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_OAM_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_OAM_OFFSET);
        data->offset = 2734;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_OAM_STAMP_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_OAM_STAMP_OFFSET);
        data->offset = 2726;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_MIRROR_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_MIRROR_CODE);
        data->offset = 2717;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_VISIBILITY);
        data->offset = 2716;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXPECTED_WON < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_EXPECTED_WON);
        data->offset = 2711;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_FOUND);
        data->offset = 2710;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_MATCH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_MATCH);
        data->offset = 2709;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_STATION_MOVE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_STATION_MOVE);
        data->offset = 2708;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_VLAN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_VLAN);
        data->offset = 2691;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_VSI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_VSI);
        data->offset = 2673;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_SRC_MAC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_SRC_MAC);
        data->offset = 2625;
        data->size = 48;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_DATA);
        data->offset = 2559;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LEARN_INFO);
        data->offset = 2556;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE);
        data->offset = 2555;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE);
        data->offset = 2554;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GLOB_IN_LIF_1);
        data->offset = 2532;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GLOB_IN_LIF_0);
        data->offset = 2510;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IN_LIF_PROFILE_1);
        data->offset = 2502+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IN_LIF_PROFILE_0);
        data->offset = 2494+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_ACTION_STRENGTH);
        data->offset = 2490;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_DESTINATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_ACTION_DESTINATION);
        data->offset = 2469;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE);
        data->offset = 2460;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL);
        data->offset = 2441;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EEI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EEI);
        data->offset = 2417;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_0);
        data->offset = 2416;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_1);
        data->offset = 2415;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_2);
        data->offset = 2414;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_3);
        data->offset = 2413;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_4);
        data->offset = 2412;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_5 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_5);
        data->offset = 2411;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_6 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_6);
        data->offset = 2410;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ELK_LKP_HIT_7 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ELK_LKP_HIT_7);
        data->offset = 2409;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_1);
        data->offset = 2321;
        data->size = 88;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_0);
        data->offset = 2161;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PP_PORT);
        data->offset = 2153;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_CORE_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_CORE_ID);
        data->offset = 2152;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IN_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IN_PORT);
        data->offset = 2152;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PTC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PTC);
        data->offset = 2144;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_LAG_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_LAG_LB_KEY);
        data->offset = 2128;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_NWK_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_NWK_LB_KEY);
        data->offset = 2112;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ECMP_LB_KEY_2);
        data->offset = 2096;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ECMP_LB_KEY_1);
        data->offset = 2080;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ECMP_LB_KEY_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ECMP_LB_KEY_0);
        data->offset = 2064;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GLOB_OUT_LIF_1);
        data->offset = 2042;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GLOB_OUT_LIF_0);
        data->offset = 2020;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IN_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IN_TTL);
        data->offset = 2012;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_BIER_STR_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_BIER_STR_OFFSET);
        data->offset = 2004;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_BIER_STR_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_BIER_STR_SIZE);
        data->offset = 2002;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_BIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PACKET_IS_BIER);
        data->offset = 2001;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_SAVED_CONTEXT_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_SAVED_CONTEXT_PROFILE);
        data->offset = 1995;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_OAM_METER_DISABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_OAM_METER_DISABLE);
        data->offset = 1994;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE);
        data->offset = 1989;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_TC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_TC);
        data->offset = 1986;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_DP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_DP);
        data->offset = 1984;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_ECN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_ECN);
        data->offset = 1982;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_NWK_QOS);
        data->offset = 1974;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_TRACE_PACKET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_TRACE_PACKET);
        data->offset = 1973;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_HEADER_SIZE_RANGE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_PACKET_HEADER_SIZE_RANGE);
        data->offset = 1971;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_QUAL_IN_PORT_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_IN_PORT_KEY_GEN_VAR);
        data->offset = 1907;
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
        data->offset = 1320;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_PMF1_TCAM_ACTION_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_PMF1_TCAM_ACTION_1);
        data->offset = 1256;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_PMF1_TCAM_ACTION_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_PMF1_TCAM_ACTION_2);
        data->offset = 1192;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_PMF1_TCAM_ACTION_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_PMF1_TCAM_ACTION_3);
        data->offset = 1128;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_PMF1_EXEM_ACTION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_PMF1_EXEM_ACTION);
        data->offset = (1064+4);
        data->size = 60;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_PMF1_DIRECT_ACTION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_PMF1_DIRECT_ACTION);
        data->offset = 944;
        data->size = 120;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_STATE_TABLE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_STATE_TABLE_DATA);
        data->offset = 935;
        data->size = dnx_data_field.state_table.data_size_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_QUAL_STATE_TABLE_ADDRESS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_QUAL_STATE_TABLE_ADDRESS);
        data->offset = 925;
        data->size = dnx_data_field.state_table.address_size_entry_max_get(unit);
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
        data->offset = 436;
        data->size = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT_PMF_GENERAL_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PP_PORT_PMF_GENERAL_DATA);
        data->offset = 372+dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
        data->size = 64-dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE);
        data->offset = 372;
        data->size = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_ALL_ONES < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_ALL_ONES);
        data->offset = 1698;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXPECTED_WON < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_EXPECTED_WON);
        data->offset = 1693;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_FOUND);
        data->offset = 1692;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_MATCH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_MATCH);
        data->offset = 1691;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_STATION_MOVE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_STATION_MOVE);
        data->offset = 1690;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_VLAN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_VLAN);
        data->offset = 1673;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_VSI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_VSI);
        data->offset = 1655;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_SRC_MAC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_SRC_MAC);
        data->offset = 1607;
        data->size = 48;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_DATA);
        data->offset = 1541;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LEARN_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LEARN_INFO);
        data->offset = 1538;
        data->size = 160;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_RPF_ECMP_MODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_RPF_ECMP_MODE);
        data->offset = 1536;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_RPF_ECMP_GROUP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_RPF_ECMP_GROUP);
        data->offset = 1520;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_ADMT_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_ADMT_PROFILE);
        data->offset = 1517;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_BIER_STR_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_BIER_STR_OFFSET);
        data->offset = 1509;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_BIER_STR_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_BIER_STR_SIZE);
        data->offset = 1507;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_BYTES_TO_REMOVE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_BYTES_TO_REMOVE);
        data->offset = 1498;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_WEAK_TM_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_WEAK_TM_VALID);
        data->offset = 1497;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_WEAK_TM_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_WEAK_TM_PROFILE);
        data->offset = 1491;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_STAT_OBJ_LM_READ_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_STAT_OBJ_LM_READ_INDEX);
        data->offset = 1489;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_CONGESTION_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_CONGESTION_INFO);
        data->offset = 1487;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_END_OF_PACKET_EDITING < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_END_OF_PACKET_EDITING);
        data->offset = 1484;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_EEI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_EEI);
        data->offset = 1460;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE);
        data->offset = 1459;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL);
        data->offset = 1440;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE);
        data->offset = 1431;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_DP_METER_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_DP_METER_COMMAND);
        data->offset = 1427;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_DP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_DP);
        data->offset = 1425;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_TC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_TC);
        data->offset = 1422;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_DESTINATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_DESTINATION);
        data->offset = 1401;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_ACTION_STRENGTH);
        data->offset = 1397;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_PAYLOAD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_PAYLOAD);
        data->offset = 1397;
        data->size = 62;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VRF_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VRF_VALUE);
        data->offset = 1379;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_DOMAIN);
        data->offset = 1379;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE);
        data->offset = 1369+dnx_data_lif.in_lif.eth_rif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_eth_rif_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE);
        data->offset = 1369;
        data->size = 10;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA_PMF_CONTAINER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GENERAL_DATA_PMF_CONTAINER);
        data->offset = 1337;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GENERAL_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GENERAL_DATA);
        data->offset = 1241;
        data->size = 128;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IEEE1588_COMMAND);
        data->offset = 1239;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP);
        data->offset = 1238;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION);
        data->offset = 1237;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET);
        data->offset = 1229;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IN_LIF_PROFILE_1);
        data->offset = 1221+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IN_LIF_PROFILE_0);
        data->offset = 1213+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GLOB_IN_LIF_1);
        data->offset = 1191;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GLOB_IN_LIF_0);
        data->offset = 1169;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PP_PORT);
        data->offset = 1161;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_CORE_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_CORE_ID);
        data->offset = 1160;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IN_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IN_PORT);
        data->offset = 1160;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IN_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IN_TTL);
        data->offset = 1152;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE);
        data->offset = 1151;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_ITPP_DELTA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_ITPP_DELTA);
        data->offset = 1143;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LAG_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LAG_LB_KEY);
        data->offset = 1127;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LATENCY_FLOW_ID_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LATENCY_FLOW_ID_VALID);
        data->offset = 1126;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_LATENCY_FLOW_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_LATENCY_FLOW_ID);
        data->offset = 1107;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_MIRROR_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_MIRROR_CODE);
        data->offset = 1098;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_MIRROR_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_MIRROR_QUALIFIER);
        data->offset = 1090;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_NWK_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_NWK_LB_KEY);
        data->offset = 1074;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_NWK_QOS);
        data->offset = 1066;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_OAM_ID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_OAM_ID);
        data->offset = 1047;
        data->size = 19;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_OAM_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_OAM_OFFSET);
        data->offset = 1039;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_OAM_STAMP_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_OAM_STAMP_OFFSET);
        data->offset = 1031;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_OAM_SUB_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_OAM_SUB_TYPE);
        data->offset = 1027;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_OAM_UP_MEP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_OAM_UP_MEP);
        data->offset = 1026;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GLOB_OUT_LIF_3);
        data->offset = 1004;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GLOB_OUT_LIF_2);
        data->offset = 982;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GLOB_OUT_LIF_1);
        data->offset = 960;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_GLOB_OUT_LIF_0);
        data->offset = 938;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_APPLET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PACKET_IS_APPLET);
        data->offset = 937;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_NASID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_NASID);
        data->offset = 932;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_BIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PACKET_IS_BIER);
        data->offset = 931;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_IEEE1588 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PACKET_IS_IEEE1588);
        data->offset = 930;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_EGRESS_PARSING_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_EGRESS_PARSING_INDEX);
        data->offset = 927;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PTC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PTC);
        data->offset = 919;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_EXT_STATISTICS_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_EXT_STATISTICS_VALID);
        data->offset = 918;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PPH_RESERVED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PPH_RESERVED);
        data->offset = 915;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PPH_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PPH_TYPE);
        data->offset = 913;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_RPF_DST);
        data->offset = 892;
        data->size = 21;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_RPF_DST_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_RPF_DST_VALID);
        data->offset = 891;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SNOOP_CODE);
        data->offset = 882;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SNOOP_STRENGTH);
        data->offset = 879;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SNOOP_QUALIFIER);
        data->offset = 871;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SRC_SYS_PORT);
        data->offset = 855;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_ST_VSQ_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_ST_VSQ_PTR);
        data->offset = 847;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE);
        data->offset = 838;
        data->size = 9;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICAL_SAMPLING_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_STATISTICAL_SAMPLING_QUALIFIER);
        data->offset = 830;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_STACKING_ROUTE_HISTORY_BITMAP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_STACKING_ROUTE_HISTORY_BITMAP);
        data->offset = 814;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_META_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_STATISTICS_META_DATA);
        data->offset = 798;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_STATISTICS_OBJECT10 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_STATISTICS_OBJECT10);
        data->offset = 795;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SYSTEM_HEADER_PROFILE_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SYSTEM_HEADER_PROFILE_INDEX);
        data->offset = 791;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_TM_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_TM_PROFILE);
        data->offset = 789;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_1);
        data->offset = 757;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_2);
        data->offset = 725;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_3);
        data->offset = 693;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_4);
        data->offset = 661;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_1_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_1_TYPE);
        data->offset = 659;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_2_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_2_TYPE);
        data->offset = 657;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_3_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_3_TYPE);
        data->offset = 655;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_4_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADER_4_TYPE);
        data->offset = 653;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADERS_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_USER_HEADERS_TYPE);
        data->offset = 653;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VISIBILITY);
        data->offset = 652;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_VID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_VID_2);
        data->offset = 640;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_VID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_VID_1);
        data->offset = 628;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_DEI_2);
        data->offset = 627;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_PCP_2);
        data->offset = 624;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_2);
        data->offset = 624;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_DEI_1);
        data->offset = 623;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_PCP_1);
        data->offset = 620;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_1);
        data->offset = 620;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_CMD_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_CMD_INDEX);
        data->offset = 613;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_VLAN_EDIT_CMD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_VLAN_EDIT_CMD);
        data->offset = 613;
        data->size = 39;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO);
        data->offset = 609;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SLB_LEARN_NEEDED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SLB_LEARN_NEEDED);
        data->offset = 608;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SLB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SLB_KEY);
        data->offset = 528;
        data->size = 80;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_SLB_PAYLOAD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_SLB_PAYLOAD);
        data->offset = 468;
        data->size = 60;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_PTC_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_PTC_KEY_GEN_VAR);
        data->offset = 436;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_QUAL_IN_PORT_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_QUAL_IN_PORT_KEY_GEN_VAR);
        data->offset = 372;
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
        data->offset = 2453;
        data->size = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE);
        data->offset = 2501;
        data->size = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_FWD_LAYER_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_FWD_LAYER_INDEX);
        data->offset = 1164;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_RESERVED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_RESERVED);
        data->offset = 1167;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_VISIBILITY);
        data->offset = 1168;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_TM_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_TM_PROFILE);
        data->offset = 1169;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_ECN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_ECN_ENABLE);
        data->offset = 1171;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_CNI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_CNI);
        data->offset = 1172;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_BIER_BFR_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_BIER_BFR_EXT_PRESENT);
        data->offset = 1173;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_FLOW_ID_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_FLOW_ID_EXT_PRESENT);
        data->offset = 1174;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT);
        data->offset = 1175;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_TM_DESTINATION_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_TM_DESTINATION_EXT_PRESENT);
        data->offset = 1176;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_MCID_OR_OUTLIF_0_OR_MCDB_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_MCID_OR_OUTLIF_0_OR_MCDB_PTR);
        data->offset = 1177;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_TM_ACTION_IS_MC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_TM_ACTION_IS_MC);
        data->offset = 1199;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_PPH_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_PPH_PRESENT);
        data->offset = 1200;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_TSH_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_TSH_EXT_PRESENT);
        data->offset = 1201;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_TM_ACTION_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_TM_ACTION_TYPE);
        data->offset = 1202;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_DP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_DP);
        data->offset = 1204;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PP_DSP < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PP_DSP);
        data->offset = 1206;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_SRC_SYS_PORT);
        data->offset = 1214;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_TC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_TC);
        data->offset = 1230;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FTMH_PACKET_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FTMH_PACKET_SIZE);
        data->offset = 1233;
        data->size = 14;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LB_KEY);
        data->offset = 1247;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_NWK_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_NWK_KEY);
        data->offset = 1255;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_STACKING_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_STACKING_EXT);
        data->offset = 1271;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_BIER_BFR_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_BIER_BFR_EXT);
        data->offset = 1287;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_TM_DESTINATION_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_TM_DESTINATION_EXT);
        data->offset = 1303;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_APPLICATION_SPECIFIC_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_APPLICATION_SPECIFIC_EXT);
        data->offset = 1327;
        data->size = 48;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_TSH_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_TSH_EXT);
        data->offset = 1375;
        data->size = 56;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FWD_ACTION_STRENGTH);
        data->offset = 1431;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_PARSING_START_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_PARSING_START_TYPE);
        data->offset = 1435;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_PARSING_START_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_PARSING_START_OFFSET);
        data->offset = 1440;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_LIF_EXT_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_LIF_EXT_TYPE);
        data->offset = 1447;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_FHEI_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_FHEI_SIZE);
        data->offset = 1450;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_LEARN_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_LEARN_EXT_PRESENT);
        data->offset = 1452;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_TTL);
        data->offset = 1453;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_NWK_QOS);
        data->offset = 1461;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_IN_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_IN_LIF);
        data->offset = 1469;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_FWD_DOMAIN);
        data->offset = 1491;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_VRF_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_VRF_VALUE);
        data->offset = 1491;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_IN_LIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_IN_LIF_PROFILE);
        data->offset = 1509;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_END_OF_PACKET_EDITING < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_END_OF_PACKET_EDITING);
        data->offset = 1517;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_FWD_LAYER_ADDITIONAL_INFO);
        data->offset = 1520;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_VALUE1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_VALUE1);
        data->offset = 1524;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PPH_VALUE2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PPH_VALUE2);
        data->offset = 1532;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FHEI_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FHEI_EXT);
        data->offset = 1540;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LEARN_EXT);
        data->offset = 1604;
        data->size = 152;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXT_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LEARN_EXT_DATA);
        data->offset = 1607;
        data->size = 64;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXT_SRC_MAC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LEARN_EXT_SRC_MAC);
        data->offset = 1673;
        data->size = 48;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXT_VSI < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LEARN_EXT_VSI);
        data->offset = 1721;
        data->size = 18;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LEARN_EXT_VLAN < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LEARN_EXT_VLAN);
        data->offset = 1739;
        data->size = 12;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LIF_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LIF_EXT);
        data->offset = 1756;
        data->size = 72;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_4 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_4);
        data->offset = 1828;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_3);
        data->offset = 1860;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_2);
        data->offset = 1892;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_1);
        data->offset = 1924;
        data->size = 32;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_4_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_4_TYPE);
        data->offset = 1956;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_3_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_3_TYPE);
        data->offset = 1958;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_2_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_2_TYPE);
        data->offset = 1960;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_USER_HEADER_1_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_USER_HEADER_1_TYPE);
        data->offset = 1962;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_IS_TDM < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_IS_TDM);
        data->offset = 1964;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_CUD_OUTLIF_OR_MCDB_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_CUD_OUTLIF_OR_MCDB_PTR);
        data->offset = 1965;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_DISCARD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_DISCARD);
        data->offset = 1987;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_OUT_LIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_OUT_LIF_PROFILE);
        data->offset = 1988;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE);
        data->offset = 1991;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_ETH_TAG_FORMAT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_ETH_TAG_FORMAT);
        data->offset = 1992;
        data->size = 10;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_UPDATED_TPIDS_PACKET_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_UPDATED_TPIDS_PACKET_DATA);
        data->offset = 2002;
        data->size = 144;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_IVE_BYTES_TO_ADD < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_IVE_BYTES_TO_ADD);
        data->offset = 2146;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_IVE_BYTES_TO_REMOVE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_IVE_BYTES_TO_REMOVE);
        data->offset = 2151;
        data->size = 5;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_ETHER_TYPE_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_ETHER_TYPE_CODE);
        data->offset = 2156;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FABRIC_OR_EGRESS_MC < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FABRIC_OR_EGRESS_MC);
        data->offset = 2172;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_ESTIMATED_BTR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_ESTIMATED_BTR);
        data->offset = 2173;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FWD_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FWD_CONTEXT);
        data->offset = 2181;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_GLOB_OUT_LIF_0);
        data->offset = 2187;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_GLOB_OUT_LIF_1);
        data->offset = 2209;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_2 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_GLOB_OUT_LIF_2);
        data->offset = 2231;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_GLOB_OUT_LIF_3 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_GLOB_OUT_LIF_3);
        data->offset = 2253;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_GLOB_IN_LIF_0);
        data->offset = 2275;
        data->size = 22;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_IN_LIF_PROFILE_0);
        data->offset = 2297+dnx_data_lif.in_lif.in_lif_profile_pmf_reserved_start_bit_get(unit);
        data->size = dnx_data_field.profile_bits.nof_ing_in_lif_get(unit);
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_IP_MC_ELIGIBLE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_IP_MC_ELIGIBLE);
        data->offset = 2305;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_IP_MC_SHOULD_BE_BRIDGED < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_IP_MC_SHOULD_BE_BRIDGED);
        data->offset = 2306;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LEARN_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LEARN_VALID);
        data->offset = 2307;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_LOCAL_OUT_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_LOCAL_OUT_LIF);
        data->offset = 2308;
        data->size = 20;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_FWD_ACTION_PROFILE_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_FWD_ACTION_PROFILE_INDEX);
        data->offset = 2328;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_ACTION_PROFILE_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_SNOOP_ACTION_PROFILE_INDEX);
        data->offset = 2331;
        data->size = 2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_SNOOP_STRENGTH);
        data->offset = 2333;
        data->size = 3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_SYSTEM_HEADERS_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_SYSTEM_HEADERS_SIZE);
        data->offset = 2336;
        data->size = 7;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_DST_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_DST_SYS_PORT);
        data->offset = 2343;
        data->size = 16;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_OUT_TM_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_OUT_TM_PORT);
        data->offset = 2359;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_OUT_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_OUT_PP_PORT);
        data->offset = 2367;
        data->size = 8;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PACKET_IS_APPLET < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PACKET_IS_APPLET);
        data->offset = 2375;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_MEM_SOFT_ERR < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_MEM_SOFT_ERR);
        data->offset = 2376;
        data->size = 1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_TC_MAP_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_TC_MAP_PROFILE);
        data->offset = 2377;
        data->size = 4;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_L4_PORT_IN_RANGE < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_L4_PORT_IN_RANGE);
        data->offset = 2381;
        data->size = 24;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PROGRAM_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PROGRAM_INDEX);
        data->offset = 2405;
        data->size = 6;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_QUAL_PER_PORT_TABLE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_QUAL_PER_PORT_TABLE_DATA);
        data->offset = 2411;
        data->size = 144;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_b0_data_field_map_attach(
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
    
    submodule_index = dnx_data_field_map_submodule_qual;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_field_map_qual_table_params;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_field_map_qual_params_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

