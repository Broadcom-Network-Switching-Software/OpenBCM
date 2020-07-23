
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_ARR

#include <soc/dnx/arr/auto_generated/arr_output.h>
#include <bcm/types.h>
#include <bcm/init.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <include/shared/utilex/utilex_str.h>
#include <include/shared/dbx/dbx_file.h>
#include <include/shared/dbx/dbx_xml.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_arr.h>

shr_error_e
dnx_arr_output_init(
    int unit)
{
    int index;

    SHR_FUNC_INIT_VARS(unit);


    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.AGE_PROFILE_ARR_get(unit);
        const dnx_data_arr_formats_MACT_AGE_PROFILE_ARR_CFG_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.MACT_AGE_PROFILE_ARR_CFG_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, MACT_AGE_PROFILE_ARR_CFGm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->AGE_PROFILE_valid)
                {
                    soc_mem_field32_set(unit, MACT_AGE_PROFILE_ARR_CFGm, data, AGE_PROFILEf, table->AGE_PROFILE);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, MACT_AGE_PROFILE_ARR_CFGm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_1_ARR_ToS_get(unit);
        const dnx_data_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DUAL_HOMING_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, DUAL_HOMING_CONFIGf, table->DUAL_HOMING_CONFIG);
                }
                if (table->EGRESS_LAST_LAYER_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, EGRESS_LAST_LAYER_CONFIGf, table->EGRESS_LAST_LAYER_CONFIG);
                }
                if (table->VLAN_EDIT_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PROFILE_CONFIGf, table->VLAN_EDIT_PROFILE_CONFIG);
                }
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->FORWARD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_CONFIGf, table->FORWARD_DOMAIN_CONFIG);
                }
                if (table->FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_NETWORK_NAME_CONFIGf, table->FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
                }
                if (table->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIGf, table->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_STATISTICS_OBJECT_COMMAND_CONFIGf, table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_STATISTICS_OBJECT_ID_CONFIGf, table->LIF_STATISTICS_OBJECT_ID_CONFIG);
                }
                if (table->MTU_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, MTU_PROFILE_CONFIGf, table->MTU_PROFILE_CONFIG);
                }
                if (table->NWK_QOS_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, NWK_QOS_IDX_CONFIGf, table->NWK_QOS_IDX_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                if (table->SOURCE_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, SOURCE_IDX_CONFIGf, table->SOURCE_IDX_CONFIG);
                }
                if (table->TTL_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, TTL_IDX_CONFIGf, table->TTL_IDX_CONFIG);
                }
                if (table->TTL_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, TTL_MODEL_CONFIGf, table->TTL_MODEL_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_2_ARR_ToS_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DUAL_HOMING_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, DUAL_HOMING_CONFIGf, table->DUAL_HOMING_CONFIG);
                }
                if (table->EGRESS_LAST_LAYER_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, EGRESS_LAST_LAYER_CONFIGf, table->EGRESS_LAST_LAYER_CONFIG);
                }
                if (table->VLAN_EDIT_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PROFILE_CONFIGf, table->VLAN_EDIT_PROFILE_CONFIG);
                }
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->FORWARD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_CONFIGf, table->FORWARD_DOMAIN_CONFIG);
                }
                if (table->FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_NETWORK_NAME_CONFIGf, table->FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
                }
                if (table->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIGf, table->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_STATISTICS_OBJECT_COMMAND_CONFIGf, table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_STATISTICS_OBJECT_ID_CONFIGf, table->LIF_STATISTICS_OBJECT_ID_CONFIG);
                }
                if (table->MTU_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, MTU_PROFILE_CONFIGf, table->MTU_PROFILE_CONFIG);
                }
                if (table->NWK_QOS_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, NWK_QOS_IDX_CONFIGf, table->NWK_QOS_IDX_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                if (table->SOURCE_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, SOURCE_IDX_CONFIGf, table->SOURCE_IDX_CONFIG);
                }
                if (table->TTL_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, TTL_IDX_CONFIGf, table->TTL_IDX_CONFIG);
                }
                if (table->TTL_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, TTL_MODEL_CONFIGf, table->TTL_MODEL_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_3_ARR_ToS_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DUAL_HOMING_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, DUAL_HOMING_CONFIGf, table->DUAL_HOMING_CONFIG);
                }
                if (table->EGRESS_LAST_LAYER_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, EGRESS_LAST_LAYER_CONFIGf, table->EGRESS_LAST_LAYER_CONFIG);
                }
                if (table->VLAN_EDIT_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PROFILE_CONFIGf, table->VLAN_EDIT_PROFILE_CONFIG);
                }
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->FORWARD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_CONFIGf, table->FORWARD_DOMAIN_CONFIG);
                }
                if (table->FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_NETWORK_NAME_CONFIGf, table->FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
                }
                if (table->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIGf, table->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_STATISTICS_OBJECT_COMMAND_CONFIGf, table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_STATISTICS_OBJECT_ID_CONFIGf, table->LIF_STATISTICS_OBJECT_ID_CONFIG);
                }
                if (table->MTU_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, MTU_PROFILE_CONFIGf, table->MTU_PROFILE_CONFIG);
                }
                if (table->NWK_QOS_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, NWK_QOS_IDX_CONFIGf, table->NWK_QOS_IDX_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                if (table->SOURCE_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, SOURCE_IDX_CONFIGf, table->SOURCE_IDX_CONFIG);
                }
                if (table->TTL_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, TTL_IDX_CONFIGf, table->TTL_IDX_CONFIG);
                }
                if (table->TTL_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, TTL_MODEL_CONFIGf, table->TTL_MODEL_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_4_ARR_ToS_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DUAL_HOMING_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, DUAL_HOMING_CONFIGf, table->DUAL_HOMING_CONFIG);
                }
                if (table->EGRESS_LAST_LAYER_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, EGRESS_LAST_LAYER_CONFIGf, table->EGRESS_LAST_LAYER_CONFIG);
                }
                if (table->VLAN_EDIT_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PROFILE_CONFIGf, table->VLAN_EDIT_PROFILE_CONFIG);
                }
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->FORWARD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_CONFIGf, table->FORWARD_DOMAIN_CONFIG);
                }
                if (table->FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_NETWORK_NAME_CONFIGf, table->FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
                }
                if (table->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIGf, table->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_STATISTICS_OBJECT_COMMAND_CONFIGf, table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_STATISTICS_OBJECT_ID_CONFIGf, table->LIF_STATISTICS_OBJECT_ID_CONFIG);
                }
                if (table->MTU_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, MTU_PROFILE_CONFIGf, table->MTU_PROFILE_CONFIG);
                }
                if (table->NWK_QOS_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, NWK_QOS_IDX_CONFIGf, table->NWK_QOS_IDX_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                if (table->SOURCE_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, SOURCE_IDX_CONFIGf, table->SOURCE_IDX_CONFIG);
                }
                if (table->TTL_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, TTL_IDX_CONFIGf, table->TTL_IDX_CONFIG);
                }
                if (table->TTL_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, TTL_MODEL_CONFIGf, table->TTL_MODEL_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_5_ARR_ToS_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DUAL_HOMING_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, DUAL_HOMING_CONFIGf, table->DUAL_HOMING_CONFIG);
                }
                if (table->EGRESS_LAST_LAYER_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, EGRESS_LAST_LAYER_CONFIGf, table->EGRESS_LAST_LAYER_CONFIG);
                }
                if (table->VLAN_EDIT_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PROFILE_CONFIGf, table->VLAN_EDIT_PROFILE_CONFIG);
                }
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->FORWARD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_CONFIGf, table->FORWARD_DOMAIN_CONFIG);
                }
                if (table->FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_NETWORK_NAME_CONFIGf, table->FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
                }
                if (table->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIGf, table->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_STATISTICS_OBJECT_COMMAND_CONFIGf, table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, LIF_STATISTICS_OBJECT_ID_CONFIGf, table->LIF_STATISTICS_OBJECT_ID_CONFIG);
                }
                if (table->MTU_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, MTU_PROFILE_CONFIGf, table->MTU_PROFILE_CONFIG);
                }
                if (table->NWK_QOS_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, NWK_QOS_IDX_CONFIGf, table->NWK_QOS_IDX_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                if (table->SOURCE_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, SOURCE_IDX_CONFIGf, table->SOURCE_IDX_CONFIG);
                }
                if (table->TTL_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, TTL_IDX_CONFIGf, table->TTL_IDX_CONFIG);
                }
                if (table->TTL_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, data, TTL_MODEL_CONFIGf, table->TTL_MODEL_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_ENCAP_ARR_ToS_plus_2_get(unit);
        const dnx_data_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLEm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_FWD_ARR_ToS_get(unit);
        const dnx_data_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ETHERNET_EGRESS_LAST_LAYER_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, ETHERNET_EGRESS_LAST_LAYER_CONFIGf, table->ETHERNET_EGRESS_LAST_LAYER_CONFIG);
                }
                if (table->ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIGf, table->ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG);
                }
                if (table->ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->FORWARDING_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, FORWARDING_DOMAIN_CONFIGf, table->FORWARDING_DOMAIN_CONFIG);
                }
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, LIF_STATISTICS_OBJECT_COMMAND_CONFIGf, table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, LIF_STATISTICS_OBJECT_ID_CONFIGf, table->LIF_STATISTICS_OBJECT_ID_CONFIG);
                }
                if (table->MTU_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, MTU_PROFILE_CONFIGf, table->MTU_PROFILE_CONFIG);
                }
                if (table->NWK_QOS_INDEX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, NWK_QOS_INDEX_CONFIGf, table->NWK_QOS_INDEX_CONFIG);
                }
                if (table->ETHERNET_OUTLIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, ETHERNET_OUTLIF_PROFILE_CONFIGf, table->ETHERNET_OUTLIF_PROFILE_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                if (table->TTL_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, TTL_MODEL_CONFIGf, table->TTL_MODEL_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_FWD_ARR_ToS_get(unit);
        const dnx_data_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ETHERNET_EGRESS_LAST_LAYER_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, ETHERNET_EGRESS_LAST_LAYER_CONFIGf, table->ETHERNET_EGRESS_LAST_LAYER_CONFIG);
                }
                if (table->ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIGf, table->ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG);
                }
                if (table->ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->FORWARDING_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, FORWARDING_DOMAIN_CONFIGf, table->FORWARDING_DOMAIN_CONFIG);
                }
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, LIF_STATISTICS_OBJECT_COMMAND_CONFIGf, table->LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
                }
                if (table->LIF_STATISTICS_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, LIF_STATISTICS_OBJECT_ID_CONFIGf, table->LIF_STATISTICS_OBJECT_ID_CONFIG);
                }
                if (table->MTU_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, MTU_PROFILE_CONFIGf, table->MTU_PROFILE_CONFIG);
                }
                if (table->NWK_QOS_INDEX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, NWK_QOS_INDEX_CONFIGf, table->NWK_QOS_INDEX_CONFIG);
                }
                if (table->ETHERNET_OUTLIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, ETHERNET_OUTLIF_PROFILE_CONFIGf, table->ETHERNET_OUTLIF_PROFILE_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                if (table->TTL_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, data, TTL_MODEL_CONFIGf, table->TTL_MODEL_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0m, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_FWD_ARR_ToS_plus_1_get(unit);
        const dnx_data_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1m, data, ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1m, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1m, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_FWD_ARR_ToS_plus_1_get(unit);
        const dnx_data_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1m, data, ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIGf, table->ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG);
                }
                if (table->ENCAP_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1m, data, ENCAP_DESTINATION_CONFIGf, table->ENCAP_DESTINATION_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1m, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Preprocessing_ARR_get(unit);
        const dnx_data_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPA_PRP_EES_ARR_CONFIGURATION_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->LLVP_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, LLVP_PROFILE_CONFIGf, table->LLVP_PROFILE_CONFIG);
                }
                if (table->EM_ACCESS_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, EM_ACCESS_CMD_CONFIGf, table->EM_ACCESS_CMD_CONFIG);
                }
                if (table->NAME_SPACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, NAME_SPACE_CONFIGf, table->NAME_SPACE_CONFIG);
                }
                if (table->FWD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, FWD_DOMAIN_CONFIGf, table->FWD_DOMAIN_CONFIG);
                }
                if (table->OAM_LIF_SET_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, OAM_LIF_SET_CONFIGf, table->OAM_LIF_SET_CONFIG);
                }
                if (table->OUTLIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, OUTLIF_PROFILE_CONFIGf, table->OUTLIF_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Preprocessing_ARR_get(unit);
        const dnx_data_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPA_PRP_EES_ARR_CONFIGURATION___array_get(unit, 0, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->LLVP_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, LLVP_PROFILE_CONFIGf, table->LLVP_PROFILE_CONFIG);
                }
                if (table->EM_ACCESS_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, EM_ACCESS_CMD_CONFIGf, table->EM_ACCESS_CMD_CONFIG);
                }
                if (table->NAME_SPACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, NAME_SPACE_CONFIGf, table->NAME_SPACE_CONFIG);
                }
                if (table->FWD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, FWD_DOMAIN_CONFIGf, table->FWD_DOMAIN_CONFIG);
                }
                if (table->OAM_LIF_SET_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, OAM_LIF_SET_CONFIGf, table->OAM_LIF_SET_CONFIG);
                }
                if (table->OUTLIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, OUTLIF_PROFILE_CONFIGf, table->OUTLIF_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Preprocessing_ARR_get(unit);
        const dnx_data_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPA_PRP_EES_ARR_CONFIGURATION___array_get(unit, 1, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->LLVP_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, LLVP_PROFILE_CONFIGf, table->LLVP_PROFILE_CONFIG);
                }
                if (table->EM_ACCESS_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, EM_ACCESS_CMD_CONFIGf, table->EM_ACCESS_CMD_CONFIG);
                }
                if (table->NAME_SPACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, NAME_SPACE_CONFIGf, table->NAME_SPACE_CONFIG);
                }
                if (table->FWD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, FWD_DOMAIN_CONFIGf, table->FWD_DOMAIN_CONFIG);
                }
                if (table->OAM_LIF_SET_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, OAM_LIF_SET_CONFIGf, table->OAM_LIF_SET_CONFIG);
                }
                if (table->OUTLIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, OUTLIF_PROFILE_CONFIGf, table->OUTLIF_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Preprocessing_ARR_get(unit);
        const dnx_data_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPA_PRP_EES_ARR_CONFIGURATION___array_get(unit, 2, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, 2, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->LLVP_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, LLVP_PROFILE_CONFIGf, table->LLVP_PROFILE_CONFIG);
                }
                if (table->EM_ACCESS_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, EM_ACCESS_CMD_CONFIGf, table->EM_ACCESS_CMD_CONFIG);
                }
                if (table->NAME_SPACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, NAME_SPACE_CONFIGf, table->NAME_SPACE_CONFIG);
                }
                if (table->FWD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, FWD_DOMAIN_CONFIGf, table->FWD_DOMAIN_CONFIG);
                }
                if (table->OAM_LIF_SET_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, OAM_LIF_SET_CONFIGf, table->OAM_LIF_SET_CONFIG);
                }
                if (table->OUTLIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, OUTLIF_PROFILE_CONFIGf, table->OUTLIF_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, 2, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Preprocessing_ARR_get(unit);
        const dnx_data_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPA_PRP_EES_ARR_CONFIGURATION___array_get(unit, 3, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, 3, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->LLVP_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, LLVP_PROFILE_CONFIGf, table->LLVP_PROFILE_CONFIG);
                }
                if (table->EM_ACCESS_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, EM_ACCESS_CMD_CONFIGf, table->EM_ACCESS_CMD_CONFIG);
                }
                if (table->NAME_SPACE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, NAME_SPACE_CONFIGf, table->NAME_SPACE_CONFIG);
                }
                if (table->FWD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, FWD_DOMAIN_CONFIGf, table->FWD_DOMAIN_CONFIG);
                }
                if (table->OAM_LIF_SET_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, OAM_LIF_SET_CONFIGf, table->OAM_LIF_SET_CONFIG);
                }
                if (table->OUTLIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, data, OUTLIF_PROFILE_CONFIGf, table->OUTLIF_PROFILE_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ETPPA_PRP_EES_ARR_CONFIGURATIONm, 3, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_1_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, ACTION_PROFILE_CONFIGf, table->ACTION_PROFILE_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_1_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, ACTION_PROFILE_CONFIGf, table->ACTION_PROFILE_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_1_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get(unit, 0, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, ACTION_PROFILE_CONFIGf, table->ACTION_PROFILE_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_1_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get(unit, 1, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, ACTION_PROFILE_CONFIGf, table->ACTION_PROFILE_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_1_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get(unit, 2, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 2, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, ACTION_PROFILE_CONFIGf, table->ACTION_PROFILE_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 2, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_1_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get(unit, 3, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 3, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, ACTION_PROFILE_CONFIGf, table->ACTION_PROFILE_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 3, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_1_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get(unit, 4, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 4, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, ACTION_PROFILE_CONFIGf, table->ACTION_PROFILE_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 4, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_1_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get(unit, 5, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 5, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, ACTION_PROFILE_CONFIGf, table->ACTION_PROFILE_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 5, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_1_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get(unit, 6, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 6, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, ACTION_PROFILE_CONFIGf, table->ACTION_PROFILE_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 6, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_1_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get(unit, 7, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 7, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, ACTION_PROFILE_CONFIGf, table->ACTION_PROFILE_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 7, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_1_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get(unit, 8, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 8, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, ACTION_PROFILE_CONFIGf, table->ACTION_PROFILE_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ETPPC_TERM_ETPS_ARR_1_CONFIGURATIONm, 8, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_2_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->NWK_QOS_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, NWK_QOS_IDX_CONFIGf, table->NWK_QOS_IDX_CONFIG);
                }
                if (table->OUTLIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, OUTLIF_PROFILE_CONFIGf, table->OUTLIF_PROFILE_CONFIG);
                }
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                if (table->TTL_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, TTL_MODEL_CONFIGf, table->TTL_MODEL_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_2_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->NWK_QOS_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, NWK_QOS_IDX_CONFIGf, table->NWK_QOS_IDX_CONFIG);
                }
                if (table->OUTLIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, OUTLIF_PROFILE_CONFIGf, table->OUTLIF_PROFILE_CONFIG);
                }
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                if (table->TTL_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, TTL_MODEL_CONFIGf, table->TTL_MODEL_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ETPP_Termination_2_ARR_get(unit);
        const dnx_data_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->NWK_QOS_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, NWK_QOS_IDX_CONFIGf, table->NWK_QOS_IDX_CONFIG);
                }
                if (table->OUTLIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, OUTLIF_PROFILE_CONFIGf, table->OUTLIF_PROFILE_CONFIG);
                }
                if (table->QOS_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, QOS_MODEL_CONFIGf, table->QOS_MODEL_CONFIG);
                }
                if (table->REMARK_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, REMARK_PROFILE_CONFIGf, table->REMARK_PROFILE_CONFIG);
                }
                if (table->TTL_MODEL_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, data, TTL_MODEL_CONFIGf, table->TTL_MODEL_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_2_CONFIGURATIONm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IPPB_ACCEPTED_ARR_get(unit);
        const dnx_data_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPB_ACCEPTED_ARR_FORMAT_CONFIG_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPB_ACCEPTED_ARR_FORMAT_CONFIGm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DATA_1_valid)
                {
                    soc_mem_field32_set(unit, IPPB_ACCEPTED_ARR_FORMAT_CONFIGm, data, DATA_1f, table->DATA_1);
                }
                if (table->DATA_2_valid)
                {
                    soc_mem_field32_set(unit, IPPB_ACCEPTED_ARR_FORMAT_CONFIGm, data, DATA_2f, table->DATA_2);
                }
                if (table->TYPE_valid)
                {
                    soc_mem_field32_set(unit, IPPB_ACCEPTED_ARR_FORMAT_CONFIGm, data, TYPEf, table->TYPE);
                }
                if (table->DESTINATION_valid)
                {
                    soc_mem_field32_set(unit, IPPB_ACCEPTED_ARR_FORMAT_CONFIGm, data, DESTINATIONf, table->DESTINATION);
                }
                if (table->STRENGTH_valid)
                {
                    soc_mem_field32_set(unit, IPPB_ACCEPTED_ARR_FORMAT_CONFIGm, data, STRENGTHf, table->STRENGTH);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPB_ACCEPTED_ARR_FORMAT_CONFIGm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR_get(unit);
        const dnx_data_arr_formats_MACT_MRQ_ARR_CFG_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.MACT_MRQ_ARR_CFG_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, MACT_MRQ_ARR_CFGm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->STRENGTH_valid)
                {
                    soc_mem_field32_set(unit, MACT_MRQ_ARR_CFGm, data, STRENGTHf, table->STRENGTH);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, MACT_MRQ_ARR_CFGm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IPPB_EXPECTED_ARR_get(unit);
        const dnx_data_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPB_EXPECTED_ARR_FORMAT_CONFIG_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPB_EXPECTED_ARR_FORMAT_CONFIGm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_valid)
                {
                    soc_mem_field32_set(unit, IPPB_EXPECTED_ARR_FORMAT_CONFIGm, data, DESTINATIONf, table->DESTINATION);
                }
                if (table->DATA_1_valid)
                {
                    soc_mem_field32_set(unit, IPPB_EXPECTED_ARR_FORMAT_CONFIGm, data, DATA_1f, table->DATA_1);
                }
                if (table->DATA_2_valid)
                {
                    soc_mem_field32_set(unit, IPPB_EXPECTED_ARR_FORMAT_CONFIGm, data, DATA_2f, table->DATA_2);
                }
                if (table->TYPE_valid)
                {
                    soc_mem_field32_set(unit, IPPB_EXPECTED_ARR_FORMAT_CONFIGm, data, TYPEf, table->TYPE);
                }
                if (table->STRENGTH_valid)
                {
                    soc_mem_field32_set(unit, IPPB_EXPECTED_ARR_FORMAT_CONFIGm, data, STRENGTHf, table->STRENGTH);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPB_EXPECTED_ARR_FORMAT_CONFIGm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_FEC_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get(unit, 0, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->OUT_LIF_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_0_CONFIGf, table->OUT_LIF_0_CONFIG);
                }
                if (table->WEAK_TM_FLOW_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, WEAK_TM_FLOW_CONFIGf, table->WEAK_TM_FLOW_CONFIG);
                }
                if (table->MC_RPF_MODE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, MC_RPF_MODE_CONFIGf, table->MC_RPF_MODE_CONFIG);
                }
                if (table->OUT_LIF_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_1_CONFIGf, table->OUT_LIF_1_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_FEC_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get(unit, 1, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->OUT_LIF_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_0_CONFIGf, table->OUT_LIF_0_CONFIG);
                }
                if (table->WEAK_TM_FLOW_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, WEAK_TM_FLOW_CONFIGf, table->WEAK_TM_FLOW_CONFIG);
                }
                if (table->MC_RPF_MODE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, MC_RPF_MODE_CONFIGf, table->MC_RPF_MODE_CONFIG);
                }
                if (table->OUT_LIF_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_1_CONFIGf, table->OUT_LIF_1_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_FEC_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get(unit, 2, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 2, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->OUT_LIF_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_0_CONFIGf, table->OUT_LIF_0_CONFIG);
                }
                if (table->WEAK_TM_FLOW_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, WEAK_TM_FLOW_CONFIGf, table->WEAK_TM_FLOW_CONFIG);
                }
                if (table->MC_RPF_MODE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, MC_RPF_MODE_CONFIGf, table->MC_RPF_MODE_CONFIG);
                }
                if (table->OUT_LIF_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_1_CONFIGf, table->OUT_LIF_1_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 2, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_FEC_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get(unit, 3, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 3, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->OUT_LIF_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_0_CONFIGf, table->OUT_LIF_0_CONFIG);
                }
                if (table->WEAK_TM_FLOW_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, WEAK_TM_FLOW_CONFIGf, table->WEAK_TM_FLOW_CONFIG);
                }
                if (table->MC_RPF_MODE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, MC_RPF_MODE_CONFIGf, table->MC_RPF_MODE_CONFIG);
                }
                if (table->OUT_LIF_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_1_CONFIGf, table->OUT_LIF_1_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 3, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_FEC_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get(unit, 4, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 4, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->OUT_LIF_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_0_CONFIGf, table->OUT_LIF_0_CONFIG);
                }
                if (table->WEAK_TM_FLOW_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, WEAK_TM_FLOW_CONFIGf, table->WEAK_TM_FLOW_CONFIG);
                }
                if (table->MC_RPF_MODE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, MC_RPF_MODE_CONFIGf, table->MC_RPF_MODE_CONFIG);
                }
                if (table->OUT_LIF_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_1_CONFIGf, table->OUT_LIF_1_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 4, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_FEC_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get(unit, 5, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 5, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->OUT_LIF_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_0_CONFIGf, table->OUT_LIF_0_CONFIG);
                }
                if (table->WEAK_TM_FLOW_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, WEAK_TM_FLOW_CONFIGf, table->WEAK_TM_FLOW_CONFIG);
                }
                if (table->MC_RPF_MODE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, MC_RPF_MODE_CONFIGf, table->MC_RPF_MODE_CONFIG);
                }
                if (table->OUT_LIF_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_1_CONFIGf, table->OUT_LIF_1_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 5, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_FEC_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get(unit, 6, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 6, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->OUT_LIF_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_0_CONFIGf, table->OUT_LIF_0_CONFIG);
                }
                if (table->WEAK_TM_FLOW_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, WEAK_TM_FLOW_CONFIGf, table->WEAK_TM_FLOW_CONFIG);
                }
                if (table->MC_RPF_MODE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, MC_RPF_MODE_CONFIGf, table->MC_RPF_MODE_CONFIG);
                }
                if (table->OUT_LIF_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_1_CONFIGf, table->OUT_LIF_1_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_FORMAT_CONFIGURATION_TABLEm, 6, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_FWD1_FWD_ARR_get(unit);
        const dnx_data_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_get(unit, 0, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->OUTLIF_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, OUTLIF_0_CONFIGf, table->OUTLIF_0_CONFIG);
                }
                if (table->OUTLIF_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, OUTLIF_1_CONFIGf, table->OUTLIF_1_CONFIG);
                }
                if (table->IRPP_FWD_RAW_DATA_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, IRPP_FWD_RAW_DATAf, table->IRPP_FWD_RAW_DATA);
                }
                if (table->DEFAULT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, DEFAULT_CONFIGf, table->DEFAULT_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_FWD1_FWD_ARR_get(unit);
        const dnx_data_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_get(unit, 1, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->OUTLIF_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, OUTLIF_0_CONFIGf, table->OUTLIF_0_CONFIG);
                }
                if (table->OUTLIF_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, OUTLIF_1_CONFIGf, table->OUTLIF_1_CONFIG);
                }
                if (table->IRPP_FWD_RAW_DATA_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, IRPP_FWD_RAW_DATAf, table->IRPP_FWD_RAW_DATA);
                }
                if (table->DEFAULT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, data, DEFAULT_CONFIGf, table->DEFAULT_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLEm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_FWD1_RPF_ARR_get(unit);
        const dnx_data_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_get(unit, 0, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->OUTLIF_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, OUTLIF_0_CONFIGf, table->OUTLIF_0_CONFIG);
                }
                if (table->DEFAULT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, DEFAULT_CONFIGf, table->DEFAULT_CONFIG);
                }
                if (table->DEFAULT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, DEFAULT_CONFIGf, table->DEFAULT_CONFIG);
                }
                if (table->SA_DROP_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, SA_DROP_CONFIGf, table->SA_DROP_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_FWD1_RPF_ARR_get(unit);
        const dnx_data_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_get(unit, 1, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->OUTLIF_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, OUTLIF_0_CONFIGf, table->OUTLIF_0_CONFIG);
                }
                if (table->DEFAULT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, DEFAULT_CONFIGf, table->DEFAULT_CONFIG);
                }
                if (table->DEFAULT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, DEFAULT_CONFIGf, table->DEFAULT_CONFIG);
                }
                if (table->SA_DROP_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, data, SA_DROP_CONFIGf, table->SA_DROP_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLEm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_SUPER_FEC_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_get(unit, 0, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->LEFT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_CONFIGf, table->LEFT_CONFIG);
                }
                if (table->LEFT_STAT_OBJECT_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_STAT_OBJECT_CMD_CONFIGf, table->LEFT_STAT_OBJECT_CMD_CONFIG);
                }
                if (table->LEFT_STAT_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_STAT_OBJECT_ID_CONFIGf, table->LEFT_STAT_OBJECT_ID_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                if (table->RIGHT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_CONFIGf, table->RIGHT_CONFIG);
                }
                if (table->RIGHT_STAT_OBJECT_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_STAT_OBJECT_CMD_CONFIGf, table->RIGHT_STAT_OBJECT_CMD_CONFIG);
                }
                if (table->RIGHT_STAT_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_STAT_OBJECT_ID_CONFIGf, table->RIGHT_STAT_OBJECT_ID_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_SUPER_FEC_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_get(unit, 1, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->LEFT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_CONFIGf, table->LEFT_CONFIG);
                }
                if (table->LEFT_STAT_OBJECT_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_STAT_OBJECT_CMD_CONFIGf, table->LEFT_STAT_OBJECT_CMD_CONFIG);
                }
                if (table->LEFT_STAT_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_STAT_OBJECT_ID_CONFIGf, table->LEFT_STAT_OBJECT_ID_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                if (table->RIGHT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_CONFIGf, table->RIGHT_CONFIG);
                }
                if (table->RIGHT_STAT_OBJECT_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_STAT_OBJECT_CMD_CONFIGf, table->RIGHT_STAT_OBJECT_CMD_CONFIG);
                }
                if (table->RIGHT_STAT_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_STAT_OBJECT_ID_CONFIGf, table->RIGHT_STAT_OBJECT_ID_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_SUPER_FEC_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_get(unit, 2, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, 2, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->LEFT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_CONFIGf, table->LEFT_CONFIG);
                }
                if (table->LEFT_STAT_OBJECT_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_STAT_OBJECT_CMD_CONFIGf, table->LEFT_STAT_OBJECT_CMD_CONFIG);
                }
                if (table->LEFT_STAT_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_STAT_OBJECT_ID_CONFIGf, table->LEFT_STAT_OBJECT_ID_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                if (table->RIGHT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_CONFIGf, table->RIGHT_CONFIG);
                }
                if (table->RIGHT_STAT_OBJECT_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_STAT_OBJECT_CMD_CONFIGf, table->RIGHT_STAT_OBJECT_CMD_CONFIG);
                }
                if (table->RIGHT_STAT_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_STAT_OBJECT_ID_CONFIGf, table->RIGHT_STAT_OBJECT_ID_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, 2, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_SUPER_FEC_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_get(unit, 3, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, 3, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->LEFT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_CONFIGf, table->LEFT_CONFIG);
                }
                if (table->LEFT_STAT_OBJECT_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_STAT_OBJECT_CMD_CONFIGf, table->LEFT_STAT_OBJECT_CMD_CONFIG);
                }
                if (table->LEFT_STAT_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_STAT_OBJECT_ID_CONFIGf, table->LEFT_STAT_OBJECT_ID_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                if (table->RIGHT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_CONFIGf, table->RIGHT_CONFIG);
                }
                if (table->RIGHT_STAT_OBJECT_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_STAT_OBJECT_CMD_CONFIGf, table->RIGHT_STAT_OBJECT_CMD_CONFIG);
                }
                if (table->RIGHT_STAT_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_STAT_OBJECT_ID_CONFIGf, table->RIGHT_STAT_OBJECT_ID_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, 3, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_SUPER_FEC_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_get(unit, 4, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, 4, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->LEFT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_CONFIGf, table->LEFT_CONFIG);
                }
                if (table->LEFT_STAT_OBJECT_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_STAT_OBJECT_CMD_CONFIGf, table->LEFT_STAT_OBJECT_CMD_CONFIG);
                }
                if (table->LEFT_STAT_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_STAT_OBJECT_ID_CONFIGf, table->LEFT_STAT_OBJECT_ID_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                if (table->RIGHT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_CONFIGf, table->RIGHT_CONFIG);
                }
                if (table->RIGHT_STAT_OBJECT_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_STAT_OBJECT_CMD_CONFIGf, table->RIGHT_STAT_OBJECT_CMD_CONFIG);
                }
                if (table->RIGHT_STAT_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_STAT_OBJECT_ID_CONFIGf, table->RIGHT_STAT_OBJECT_ID_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, 4, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_SUPER_FEC_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_get(unit, 5, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, 5, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->LEFT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_CONFIGf, table->LEFT_CONFIG);
                }
                if (table->LEFT_STAT_OBJECT_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_STAT_OBJECT_CMD_CONFIGf, table->LEFT_STAT_OBJECT_CMD_CONFIG);
                }
                if (table->LEFT_STAT_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, LEFT_STAT_OBJECT_ID_CONFIGf, table->LEFT_STAT_OBJECT_ID_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                if (table->RIGHT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_CONFIGf, table->RIGHT_CONFIG);
                }
                if (table->RIGHT_STAT_OBJECT_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_STAT_OBJECT_CMD_CONFIGf, table->RIGHT_STAT_OBJECT_CMD_CONFIG);
                }
                if (table->RIGHT_STAT_OBJECT_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, data, RIGHT_STAT_OBJECT_ID_CONFIGf, table->RIGHT_STAT_OBJECT_ID_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLEm, 5, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_VSI_ARR_get(unit);
        const dnx_data_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DA_NOT_FOUND_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, data, DA_NOT_FOUND_DESTINATION_CONFIGf, table->DA_NOT_FOUND_DESTINATION_CONFIG);
                }
                if (table->ECN_MAPPING_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, data, ECN_MAPPING_PROFILE_CONFIGf, table->ECN_MAPPING_PROFILE_CONFIG);
                }
                if (table->MY_MAC_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, data, MY_MAC_CONFIGf, table->MY_MAC_CONFIG);
                }
                if (table->MY_MAC_PREFIX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, data, MY_MAC_PREFIX_CONFIGf, table->MY_MAC_PREFIX_CONFIG);
                }
                if (table->PROPAGATION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, data, PROPAGATION_PROFILE_CONFIGf, table->PROPAGATION_PROFILE_CONFIG);
                }
                if (table->QOS_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, data, QOS_PROFILE_CONFIGf, table->QOS_PROFILE_CONFIG);
                }
                if (table->TOPOLOGY_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, data, TOPOLOGY_ID_CONFIGf, table->TOPOLOGY_ID_CONFIG);
                }
                if (table->VRF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, data, VRF_CONFIGf, table->VRF_CONFIG);
                }
                if (table->VRID_BITMAP_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, data, VRID_BITMAP_CONFIGf, table->VRID_BITMAP_CONFIG);
                }
                if (table->VSI_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, data, VSI_PROFILE_CONFIGf, table->VSI_PROFILE_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_VSI_ARR_get(unit);
        const dnx_data_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPF_VSI_FORMAT_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DA_NOT_FOUND_DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, data, DA_NOT_FOUND_DESTINATION_CONFIGf, table->DA_NOT_FOUND_DESTINATION_CONFIG);
                }
                if (table->ECN_MAPPING_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, data, ECN_MAPPING_PROFILE_CONFIGf, table->ECN_MAPPING_PROFILE_CONFIG);
                }
                if (table->MY_MAC_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, data, MY_MAC_CONFIGf, table->MY_MAC_CONFIG);
                }
                if (table->MY_MAC_PREFIX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, data, MY_MAC_PREFIX_CONFIGf, table->MY_MAC_PREFIX_CONFIG);
                }
                if (table->PROPAGATION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, data, PROPAGATION_PROFILE_CONFIGf, table->PROPAGATION_PROFILE_CONFIG);
                }
                if (table->QOS_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, data, QOS_PROFILE_CONFIGf, table->QOS_PROFILE_CONFIG);
                }
                if (table->TOPOLOGY_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, data, TOPOLOGY_ID_CONFIGf, table->TOPOLOGY_ID_CONFIG);
                }
                if (table->VRF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, data, VRF_CONFIGf, table->VRF_CONFIG);
                }
                if (table->VRID_BITMAP_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, data, VRID_BITMAP_CONFIGf, table->VRID_BITMAP_CONFIG);
                }
                if (table->VSI_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, data, VSI_PROFILE_CONFIGf, table->VSI_PROFILE_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPF_VSI_FORMAT_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_VTT_ARR_get(unit);
        const dnx_data_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_get(unit, 3, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, 3, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, ACTION_PROFILE_IDX_CONFIGf, table->ACTION_PROFILE_IDX_CONFIG);
                }
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->ECN_MAPPING_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, ECN_MAPPING_PROFILE_CONFIGf, table->ECN_MAPPING_PROFILE_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->FORWARD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_CONFIGf, table->FORWARD_DOMAIN_CONFIG);
                }
                if (table->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIGf, table->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG);
                }
                if (table->SYS_INLIF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, SYS_INLIF_CONFIGf, table->SYS_INLIF_CONFIG);
                }
                if (table->OUT_LIF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_CONFIGf, table->OUT_LIF_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->LIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_PROFILE_CONFIGf, table->LIF_PROFILE_CONFIG);
                }
                if (table->LEARN_EN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_EN_CONFIGf, table->LEARN_EN_CONFIG);
                }
                if (table->LEARN_INFO_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_INFO_CONFIGf, table->LEARN_INFO_CONFIG);
                }
                if (table->LEARN_PAYLOAD_CONTEXT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_PAYLOAD_CONTEXT_CONFIGf, table->LEARN_PAYLOAD_CONTEXT_CONFIG);
                }
                if (table->LIF_GENERIC_DATA_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_GENERIC_DATA_0_CONFIGf, table->LIF_GENERIC_DATA_0_CONFIG);
                }
                if (table->LIF_GENERIC_DATA_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_GENERIC_DATA_1_CONFIGf, table->LIF_GENERIC_DATA_1_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                if (table->NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, NEXT_LAYER_NETWORK_DOMAIN_CONFIGf, table->NEXT_LAYER_NETWORK_DOMAIN_CONFIG);
                }
                if (table->NEXT_PARSER_CONTEXT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, NEXT_PARSER_CONTEXT_CONFIGf, table->NEXT_PARSER_CONTEXT_CONFIG);
                }
                if (table->OAM_LIF_SET_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, OAM_LIF_SET_CONFIGf, table->OAM_LIF_SET_CONFIG);
                }
                if (table->PROPAGATION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROPAGATION_PROFILE_CONFIGf, table->PROPAGATION_PROFILE_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->QOS_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, QOS_PROFILE_CONFIGf, table->QOS_PROFILE_CONFIG);
                }
                if (table->SERVICE_TYPE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, SERVICE_TYPE_CONFIGf, table->SERVICE_TYPE_CONFIG);
                }
                if (table->TERMINATION_TYPE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, TERMINATION_TYPE_CONFIGf, table->TERMINATION_TYPE_CONFIG);
                }
                if (table->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PCP_DEI_PROFILE_CONFIGf, table->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG);
                }
                if (table->VLAN_EDIT_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PROFILE_CONFIGf, table->VLAN_EDIT_PROFILE_CONFIG);
                }
                if (table->VLAN_EDIT_VID_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_VID_1_CONFIGf, table->VLAN_EDIT_VID_1_CONFIG);
                }
                if (table->VLAN_EDIT_VID_2_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_VID_2_CONFIGf, table->VLAN_EDIT_VID_2_CONFIG);
                }
                if (table->VLAN_MEMBER_IF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBER_IF_CONFIGf, table->VLAN_MEMBER_IF_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, 3, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_VTT_ARR_get(unit);
        const dnx_data_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_get(unit, 4, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, 4, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, ACTION_PROFILE_IDX_CONFIGf, table->ACTION_PROFILE_IDX_CONFIG);
                }
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->ECN_MAPPING_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, ECN_MAPPING_PROFILE_CONFIGf, table->ECN_MAPPING_PROFILE_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->FORWARD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_CONFIGf, table->FORWARD_DOMAIN_CONFIG);
                }
                if (table->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIGf, table->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG);
                }
                if (table->SYS_INLIF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, SYS_INLIF_CONFIGf, table->SYS_INLIF_CONFIG);
                }
                if (table->OUT_LIF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_CONFIGf, table->OUT_LIF_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->LIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_PROFILE_CONFIGf, table->LIF_PROFILE_CONFIG);
                }
                if (table->LEARN_EN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_EN_CONFIGf, table->LEARN_EN_CONFIG);
                }
                if (table->LEARN_INFO_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_INFO_CONFIGf, table->LEARN_INFO_CONFIG);
                }
                if (table->LEARN_PAYLOAD_CONTEXT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_PAYLOAD_CONTEXT_CONFIGf, table->LEARN_PAYLOAD_CONTEXT_CONFIG);
                }
                if (table->LIF_GENERIC_DATA_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_GENERIC_DATA_0_CONFIGf, table->LIF_GENERIC_DATA_0_CONFIG);
                }
                if (table->LIF_GENERIC_DATA_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_GENERIC_DATA_1_CONFIGf, table->LIF_GENERIC_DATA_1_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                if (table->NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, NEXT_LAYER_NETWORK_DOMAIN_CONFIGf, table->NEXT_LAYER_NETWORK_DOMAIN_CONFIG);
                }
                if (table->NEXT_PARSER_CONTEXT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, NEXT_PARSER_CONTEXT_CONFIGf, table->NEXT_PARSER_CONTEXT_CONFIG);
                }
                if (table->OAM_LIF_SET_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, OAM_LIF_SET_CONFIGf, table->OAM_LIF_SET_CONFIG);
                }
                if (table->PROPAGATION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROPAGATION_PROFILE_CONFIGf, table->PROPAGATION_PROFILE_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->QOS_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, QOS_PROFILE_CONFIGf, table->QOS_PROFILE_CONFIG);
                }
                if (table->SERVICE_TYPE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, SERVICE_TYPE_CONFIGf, table->SERVICE_TYPE_CONFIG);
                }
                if (table->TERMINATION_TYPE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, TERMINATION_TYPE_CONFIGf, table->TERMINATION_TYPE_CONFIG);
                }
                if (table->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PCP_DEI_PROFILE_CONFIGf, table->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG);
                }
                if (table->VLAN_EDIT_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PROFILE_CONFIGf, table->VLAN_EDIT_PROFILE_CONFIG);
                }
                if (table->VLAN_EDIT_VID_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_VID_1_CONFIGf, table->VLAN_EDIT_VID_1_CONFIG);
                }
                if (table->VLAN_EDIT_VID_2_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_VID_2_CONFIGf, table->VLAN_EDIT_VID_2_CONFIG);
                }
                if (table->VLAN_MEMBER_IF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBER_IF_CONFIGf, table->VLAN_MEMBER_IF_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_FORMAT_CONFIGURATION_TABLEm, 4, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_VTT_ARR_get(unit);
        const dnx_data_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_get(unit, 0, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, ACTION_PROFILE_IDX_CONFIGf, table->ACTION_PROFILE_IDX_CONFIG);
                }
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->ECN_MAPPING_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, ECN_MAPPING_PROFILE_CONFIGf, table->ECN_MAPPING_PROFILE_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->FORWARD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_CONFIGf, table->FORWARD_DOMAIN_CONFIG);
                }
                if (table->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIGf, table->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG);
                }
                if (table->SYS_INLIF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, SYS_INLIF_CONFIGf, table->SYS_INLIF_CONFIG);
                }
                if (table->OUT_LIF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_CONFIGf, table->OUT_LIF_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->LIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_PROFILE_CONFIGf, table->LIF_PROFILE_CONFIG);
                }
                if (table->LEARN_EN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_EN_CONFIGf, table->LEARN_EN_CONFIG);
                }
                if (table->LEARN_INFO_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_INFO_CONFIGf, table->LEARN_INFO_CONFIG);
                }
                if (table->LEARN_PAYLOAD_CONTEXT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_PAYLOAD_CONTEXT_CONFIGf, table->LEARN_PAYLOAD_CONTEXT_CONFIG);
                }
                if (table->LIF_GENERIC_DATA_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_GENERIC_DATA_0_CONFIGf, table->LIF_GENERIC_DATA_0_CONFIG);
                }
                if (table->LIF_GENERIC_DATA_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_GENERIC_DATA_1_CONFIGf, table->LIF_GENERIC_DATA_1_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                if (table->NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, NEXT_LAYER_NETWORK_DOMAIN_CONFIGf, table->NEXT_LAYER_NETWORK_DOMAIN_CONFIG);
                }
                if (table->NEXT_PARSER_CONTEXT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, NEXT_PARSER_CONTEXT_CONFIGf, table->NEXT_PARSER_CONTEXT_CONFIG);
                }
                if (table->OAM_LIF_SET_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, OAM_LIF_SET_CONFIGf, table->OAM_LIF_SET_CONFIG);
                }
                if (table->PROPAGATION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROPAGATION_PROFILE_CONFIGf, table->PROPAGATION_PROFILE_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->QOS_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, QOS_PROFILE_CONFIGf, table->QOS_PROFILE_CONFIG);
                }
                if (table->SERVICE_TYPE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, SERVICE_TYPE_CONFIGf, table->SERVICE_TYPE_CONFIG);
                }
                if (table->TERMINATION_TYPE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, TERMINATION_TYPE_CONFIGf, table->TERMINATION_TYPE_CONFIG);
                }
                if (table->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PCP_DEI_PROFILE_CONFIGf, table->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG);
                }
                if (table->VLAN_EDIT_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PROFILE_CONFIGf, table->VLAN_EDIT_PROFILE_CONFIG);
                }
                if (table->VLAN_EDIT_VID_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_VID_1_CONFIGf, table->VLAN_EDIT_VID_1_CONFIG);
                }
                if (table->VLAN_EDIT_VID_2_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_VID_2_CONFIGf, table->VLAN_EDIT_VID_2_CONFIG);
                }
                if (table->VLAN_MEMBER_IF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBER_IF_CONFIGf, table->VLAN_MEMBER_IF_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, 0, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_VTT_ARR_get(unit);
        const dnx_data_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_get(unit, 1, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, ACTION_PROFILE_IDX_CONFIGf, table->ACTION_PROFILE_IDX_CONFIG);
                }
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->ECN_MAPPING_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, ECN_MAPPING_PROFILE_CONFIGf, table->ECN_MAPPING_PROFILE_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->FORWARD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_CONFIGf, table->FORWARD_DOMAIN_CONFIG);
                }
                if (table->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIGf, table->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG);
                }
                if (table->SYS_INLIF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, SYS_INLIF_CONFIGf, table->SYS_INLIF_CONFIG);
                }
                if (table->OUT_LIF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_CONFIGf, table->OUT_LIF_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->LIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_PROFILE_CONFIGf, table->LIF_PROFILE_CONFIG);
                }
                if (table->LEARN_EN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_EN_CONFIGf, table->LEARN_EN_CONFIG);
                }
                if (table->LEARN_INFO_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_INFO_CONFIGf, table->LEARN_INFO_CONFIG);
                }
                if (table->LEARN_PAYLOAD_CONTEXT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_PAYLOAD_CONTEXT_CONFIGf, table->LEARN_PAYLOAD_CONTEXT_CONFIG);
                }
                if (table->LIF_GENERIC_DATA_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_GENERIC_DATA_0_CONFIGf, table->LIF_GENERIC_DATA_0_CONFIG);
                }
                if (table->LIF_GENERIC_DATA_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_GENERIC_DATA_1_CONFIGf, table->LIF_GENERIC_DATA_1_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                if (table->NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, NEXT_LAYER_NETWORK_DOMAIN_CONFIGf, table->NEXT_LAYER_NETWORK_DOMAIN_CONFIG);
                }
                if (table->NEXT_PARSER_CONTEXT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, NEXT_PARSER_CONTEXT_CONFIGf, table->NEXT_PARSER_CONTEXT_CONFIG);
                }
                if (table->OAM_LIF_SET_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, OAM_LIF_SET_CONFIGf, table->OAM_LIF_SET_CONFIG);
                }
                if (table->PROPAGATION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROPAGATION_PROFILE_CONFIGf, table->PROPAGATION_PROFILE_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->QOS_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, QOS_PROFILE_CONFIGf, table->QOS_PROFILE_CONFIG);
                }
                if (table->SERVICE_TYPE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, SERVICE_TYPE_CONFIGf, table->SERVICE_TYPE_CONFIG);
                }
                if (table->TERMINATION_TYPE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, TERMINATION_TYPE_CONFIGf, table->TERMINATION_TYPE_CONFIG);
                }
                if (table->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PCP_DEI_PROFILE_CONFIGf, table->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG);
                }
                if (table->VLAN_EDIT_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PROFILE_CONFIGf, table->VLAN_EDIT_PROFILE_CONFIG);
                }
                if (table->VLAN_EDIT_VID_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_VID_1_CONFIGf, table->VLAN_EDIT_VID_1_CONFIG);
                }
                if (table->VLAN_EDIT_VID_2_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_VID_2_CONFIGf, table->VLAN_EDIT_VID_2_CONFIG);
                }
                if (table->VLAN_MEMBER_IF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBER_IF_CONFIGf, table->VLAN_MEMBER_IF_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, 1, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.IRPP_VTT_ARR_get(unit);
        const dnx_data_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_get(unit, 2, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, 2, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->ACTION_PROFILE_IDX_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, ACTION_PROFILE_IDX_CONFIGf, table->ACTION_PROFILE_IDX_CONFIG);
                }
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->ECN_MAPPING_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, ECN_MAPPING_PROFILE_CONFIGf, table->ECN_MAPPING_PROFILE_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->FORWARD_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_CONFIGf, table->FORWARD_DOMAIN_CONFIG);
                }
                if (table->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIGf, table->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG);
                }
                if (table->SYS_INLIF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, SYS_INLIF_CONFIGf, table->SYS_INLIF_CONFIG);
                }
                if (table->OUT_LIF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, OUT_LIF_CONFIGf, table->OUT_LIF_CONFIG);
                }
                if (table->STAT_OBJ_CMD_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_CMD_CONFIGf, table->STAT_OBJ_CMD_CONFIG);
                }
                if (table->STAT_OBJ_ID_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, STAT_OBJ_ID_CONFIGf, table->STAT_OBJ_ID_CONFIG);
                }
                if (table->LIF_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_PROFILE_CONFIGf, table->LIF_PROFILE_CONFIG);
                }
                if (table->LEARN_EN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_EN_CONFIGf, table->LEARN_EN_CONFIG);
                }
                if (table->LEARN_INFO_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_INFO_CONFIGf, table->LEARN_INFO_CONFIG);
                }
                if (table->LEARN_PAYLOAD_CONTEXT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LEARN_PAYLOAD_CONTEXT_CONFIGf, table->LEARN_PAYLOAD_CONTEXT_CONFIG);
                }
                if (table->LIF_GENERIC_DATA_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_GENERIC_DATA_0_CONFIGf, table->LIF_GENERIC_DATA_0_CONFIG);
                }
                if (table->LIF_GENERIC_DATA_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, LIF_GENERIC_DATA_1_CONFIGf, table->LIF_GENERIC_DATA_1_CONFIG);
                }
                if (table->PROTECTION_PTR_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PTR_CONFIGf, table->PROTECTION_PTR_CONFIG);
                }
                if (table->NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, NEXT_LAYER_NETWORK_DOMAIN_CONFIGf, table->NEXT_LAYER_NETWORK_DOMAIN_CONFIG);
                }
                if (table->NEXT_PARSER_CONTEXT_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, NEXT_PARSER_CONTEXT_CONFIGf, table->NEXT_PARSER_CONTEXT_CONFIG);
                }
                if (table->OAM_LIF_SET_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, OAM_LIF_SET_CONFIGf, table->OAM_LIF_SET_CONFIG);
                }
                if (table->PROPAGATION_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROPAGATION_PROFILE_CONFIGf, table->PROPAGATION_PROFILE_CONFIG);
                }
                if (table->PROTECTION_PATH_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, PROTECTION_PATH_CONFIGf, table->PROTECTION_PATH_CONFIG);
                }
                if (table->QOS_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, QOS_PROFILE_CONFIGf, table->QOS_PROFILE_CONFIG);
                }
                if (table->SERVICE_TYPE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, SERVICE_TYPE_CONFIGf, table->SERVICE_TYPE_CONFIG);
                }
                if (table->TERMINATION_TYPE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, TERMINATION_TYPE_CONFIGf, table->TERMINATION_TYPE_CONFIG);
                }
                if (table->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PCP_DEI_PROFILE_CONFIGf, table->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG);
                }
                if (table->VLAN_EDIT_PROFILE_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_PROFILE_CONFIGf, table->VLAN_EDIT_PROFILE_CONFIG);
                }
                if (table->VLAN_EDIT_VID_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_VID_1_CONFIGf, table->VLAN_EDIT_VID_1_CONFIG);
                }
                if (table->VLAN_EDIT_VID_2_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_EDIT_VID_2_CONFIGf, table->VLAN_EDIT_VID_2_CONFIG);
                }
                if (table->VLAN_MEMBER_IF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, data, VLAN_MEMBER_IF_CONFIGf, table->VLAN_MEMBER_IF_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_FORMAT_CONFIGURATION_TABLEm, 2, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ITPP_MACT_LEARN_JR1_TRANSLATE_ARR_get(unit);
        const dnx_data_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.IPPD_LEARN_ARR_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPD_LEARN_ARR_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_LEARN_ARR_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_LEARN_ARR_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->OUTLIF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, IPPD_LEARN_ARR_CONFIGURATION_TABLEm, data, OUTLIF_CONFIGf, table->OUTLIF_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPD_LEARN_ARR_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.ITPP_MACT_LEARN_JR1_TRANSLATE_ARR_get(unit);
        const dnx_data_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.OLP_LEARN_ARR_CONFIGURATION_TABLE_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, OLP_LEARN_ARR_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->DESTINATION_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, OLP_LEARN_ARR_CONFIGURATION_TABLEm, data, DESTINATION_CONFIGf, table->DESTINATION_CONFIG);
                }
                if (table->EEI_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, OLP_LEARN_ARR_CONFIGURATION_TABLEm, data, EEI_CONFIGf, table->EEI_CONFIG);
                }
                if (table->OUTLIF_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, OLP_LEARN_ARR_CONFIGURATION_TABLEm, data, OUTLIF_CONFIGf, table->OUTLIF_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, OLP_LEARN_ARR_CONFIGURATION_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    
    {
        soc_reg_above_64_val_t data;
        int max_nof_entries = dnx_data_arr.max_nof_entries.PPMC_ERPP_ARR_get(unit);
        const dnx_data_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t* table;

        for(index = 0; index < max_nof_entries; index++)
        {
            table = dnx_data_arr.formats.ERPP_EGRESS_MCDB_FORMAT_CFG_get(unit, index);
            if(table->valid)
            {
                SOC_REG_ABOVE_64_CLEAR(data);
                SHR_IF_ERR_EXIT(soc_mem_read(unit, ERPP_EGRESS_MCDB_FORMAT_CFGm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
                if (table->MCDB_OUTLIF_0_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ERPP_EGRESS_MCDB_FORMAT_CFGm, data, MCDB_OUTLIF_0_CONFIGf, table->MCDB_OUTLIF_0_CONFIG);
                }
                if (table->MCDB_OUTLIF_1_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ERPP_EGRESS_MCDB_FORMAT_CFGm, data, MCDB_OUTLIF_1_CONFIGf, table->MCDB_OUTLIF_1_CONFIG);
                }
                if (table->MCDB_OUTLIF_2_CONFIG_valid)
                {
                    soc_mem_field32_set(unit, ERPP_EGRESS_MCDB_FORMAT_CFGm, data, MCDB_OUTLIF_2_CONFIGf, table->MCDB_OUTLIF_2_CONFIG);
                }
                SHR_IF_ERR_EXIT(soc_mem_write(unit, ERPP_EGRESS_MCDB_FORMAT_CFGm, IHP_BLOCK(unit, SOC_BLOCK_ALL), index, data));
            }
        }
    }
    

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_2_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_3_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_4_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPB_ENC_5_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_ENC_1_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_FORWARDING_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPA_PRP_EES_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_9_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 9, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.ETPP_ETPS_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 6 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 6 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 6 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 6 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 6 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 6 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 6 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 6 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 6 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 6 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 6 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_FEC_ARR_PREFIX_INDEX_TABLEm, 6 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPD_SUPER_ARR_PREFIX_INDEX_TABLEm, 5 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_FWD_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_FWD_STAGE_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPB_FLP_RPF_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 3 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPA_VTT_PREFIX_INDEX_TABLEm, 4 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 0 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 1 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_8_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 8, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_10_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 10, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_11_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 11, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_12_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 12, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_13_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 13, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_14_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 14, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
        soc_mem_field32_set(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_IN_LIF_FORMATS___prefix_15_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_array_write(unit, IPPF_VTT_PREFIX_INDEX_TABLEm, 2 ,IHP_BLOCK(unit, SOC_BLOCK_ALL), 15, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPA_VSI_PREFIX_INDEX_TABLE_1m, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
    }

    {
        soc_reg_above_64_val_t data;
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        soc_mem_field32_set(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_0_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 0, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        soc_mem_field32_set(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_1_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 1, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        soc_mem_field32_set(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_2_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 2, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        soc_mem_field32_set(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_3_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 3, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        soc_mem_field32_set(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_4_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 4, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        soc_mem_field32_set(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_5_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 5, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        soc_mem_field32_set(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_6_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 6, data));
        SOC_REG_ABOVE_64_CLEAR(data);
        SHR_IF_ERR_EXIT(soc_mem_read(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
        soc_mem_field32_set(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, data, PREFIXf, dnx_data_arr.prefix.IRPP_VSI_FORMATS___prefix_7_get(unit));
        SHR_IF_ERR_EXIT(soc_mem_write(unit, IPPF_VSI_PREFIX_INDEX_TABLEm, IHP_BLOCK(unit, SOC_BLOCK_ALL), 7, data));
    }


exit:
    SHR_FUNC_EXIT;
}
