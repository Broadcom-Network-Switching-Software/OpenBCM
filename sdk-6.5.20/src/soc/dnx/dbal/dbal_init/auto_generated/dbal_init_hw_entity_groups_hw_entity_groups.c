
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_tdm_egress_configuration_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_TDM_EGRESS_CONFIGURATION];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_TDM_GENERAL_CONFIGURATIONr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_TDM_GENERAL_CONFIGURATIONr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_qos_propag_profile_mapping_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_QOS_PROPAG_PROFILE_MAPPING];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_QOS_PROPAG_PROFILE_MAPPINGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_QOS_PROPAG_PROFILE_MAPPINGr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_special_label_values_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_SPECIAL_LABEL_VALUES];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_SPECIAL_LABEL_VALUESr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_SPECIAL_LABEL_VALUESr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vsi_profile_attributes_0_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VSI_PROFILE_ATTRIBUTES_0];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VSI_PROFILE_ATTRIBUTES_0m );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VSI_PROFILE_ATTRIBUTES_0m );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_itpp_general_cfg_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_ITPP_GENERAL_CFG];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ITPPD_ITPP_GENERAL_CFGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ITPP_ITPP_GENERAL_CFGr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_arr_prefix_group_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_ARR_PREFIX_GROUP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 8 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPA_PRP_EES_ARR_PREFIX_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_TERM_ETPS_ARR_PREFIX_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , ETPPB_ENC_3_PREFIX_INDEX_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , ETPPB_ENC_4_PREFIX_INDEX_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , ETPPB_ENC_5_PREFIX_INDEX_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , ETPPB_ENC_2_PREFIX_INDEX_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , ETPPC_ENC_1_PREFIX_INDEX_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , ETPPC_FORWARDING_PREFIX_INDEX_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_itpp_mapping_fwd_info_to_en_term_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ITPP_MAPPING_FWD_INFO_TO_EN_TERM];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ITPP_MAPPING_FWD_INFO_TO_EN_TERMr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ITPPD_MAPPING_FWD_INFO_TO_EN_TERMr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vsi_profile_attributes_1_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VSI_PROFILE_ATTRIBUTES_1];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VSI_PROFILE_ATTRIBUTES_1m );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VSI_PROFILE_ATTRIBUTES_1m );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_learn_key_context_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_LEARN_KEY_CONTEXT];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_LEARN_KEY_CONTEXTr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_LEARN_KEY_CONTEXTr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdum_rx_fifo_range_rmc_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDUM_RX_FIFO_RANGE_RMC];
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_J2P_A0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDBM_RX_FIFO_RANGE_RMC_0r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDBM_RX_FIFO_RANGE_RMC_1r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDBM_RX_FIFO_RANGE_RMC_2r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDBM_RX_FIFO_RANGE_RMC_3r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDBM_RX_FIFO_RANGE_RMC_4r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDBM_RX_FIFO_RANGE_RMC_5r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDBM_RX_FIFO_RANGE_RMC_6r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDBM_RX_FIFO_RANGE_RMC_7r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CDBM_RX_FIFO_RANGE_RMC_8r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CDBM_RX_FIFO_RANGE_RMC_9r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CDBM_RX_FIFO_RANGE_RMC_10r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CDBM_RX_FIFO_RANGE_RMC_11r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CDBM_RX_FIFO_RANGE_RMC_12r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CDBM_RX_FIFO_RANGE_RMC_13r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CDBM_RX_FIFO_RANGE_RMC_14r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CDBM_RX_FIFO_RANGE_RMC_15r );
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDUM_RX_FIFO_RANGE_RMC_0r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDUM_RX_FIFO_RANGE_RMC_1r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDUM_RX_FIFO_RANGE_RMC_2r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDUM_RX_FIFO_RANGE_RMC_3r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDUM_RX_FIFO_RANGE_RMC_4r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDUM_RX_FIFO_RANGE_RMC_5r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDUM_RX_FIFO_RANGE_RMC_6r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDUM_RX_FIFO_RANGE_RMC_7r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CDUM_RX_FIFO_RANGE_RMC_8r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CDUM_RX_FIFO_RANGE_RMC_9r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CDUM_RX_FIFO_RANGE_RMC_10r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CDUM_RX_FIFO_RANGE_RMC_11r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CDUM_RX_FIFO_RANGE_RMC_12r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CDUM_RX_FIFO_RANGE_RMC_13r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CDUM_RX_FIFO_RANGE_RMC_14r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CDUM_RX_FIFO_RANGE_RMC_15r );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_default_svtag_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_DEFAULT_SVTAG];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_DEFAULT_SVTAGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_DEFAULT_SVTAGr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_source_adrs_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_SOURCE_ADRS_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 3 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_SOURCE_ADRS_MAP_0m );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPB_SOURCE_ADRS_MAP_1m );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , ETPPC_SOURCE_ADRS_MAPm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_enc_device_configs_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_ENC_DEVICE_CONFIGS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_ENC_DEVICE_CONFIGSr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_ENC_DEVICE_CONFIGSr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_additional_headers_profile_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_ADDITIONAL_HEADERS_PROFILE_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_ADDITIONAL_HEADERS_PROFILE_MAPm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_ADDITIONAL_HEADERS_PROFILE_MAPm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_ingress_trapped_by_fhei_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_INGRESS_TRAPPED_BY_FHEI];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPA_INGRESS_TRAPPED_BY_FHEIr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ERPP_INGRESS_TRAPPED_BY_FHEIr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdum_mac_reset_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDUM_MAC_RESET];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDUM_MAC_0_RESETr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDUM_MAC_1_RESETr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdu_mac_reset_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDU_MAC_RESET];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDU_MAC_0_RESETr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDU_MAC_1_RESETr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_clu_pm_reset_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CLU_PM_RESET];
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_J2C_A0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 4 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CLU_PM_0_RESETNr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CLU_PM_1_RESETNr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CLU_PM_2_RESETNr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CLU_PM_3_RESETNr );
    }
    else if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 3 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CLU_PM_0_RESETNr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CLU_PM_1_RESETNr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CLU_PM_2_RESETNr );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdu_rx_fifo_range_rmc_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDU_RX_FIFO_RANGE_RMC];
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_J2P_A0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDB_RX_FIFO_RANGE_RMC_0r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDB_RX_FIFO_RANGE_RMC_1r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDB_RX_FIFO_RANGE_RMC_2r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDB_RX_FIFO_RANGE_RMC_3r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDB_RX_FIFO_RANGE_RMC_4r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDB_RX_FIFO_RANGE_RMC_5r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDB_RX_FIFO_RANGE_RMC_6r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDB_RX_FIFO_RANGE_RMC_7r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CDB_RX_FIFO_RANGE_RMC_8r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CDB_RX_FIFO_RANGE_RMC_9r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CDB_RX_FIFO_RANGE_RMC_10r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CDB_RX_FIFO_RANGE_RMC_11r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CDB_RX_FIFO_RANGE_RMC_12r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CDB_RX_FIFO_RANGE_RMC_13r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CDB_RX_FIFO_RANGE_RMC_14r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CDB_RX_FIFO_RANGE_RMC_15r );
    }
    else
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDU_RX_FIFO_RANGE_RMC_0r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDU_RX_FIFO_RANGE_RMC_1r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDU_RX_FIFO_RANGE_RMC_2r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDU_RX_FIFO_RANGE_RMC_3r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDU_RX_FIFO_RANGE_RMC_4r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDU_RX_FIFO_RANGE_RMC_5r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDU_RX_FIFO_RANGE_RMC_6r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDU_RX_FIFO_RANGE_RMC_7r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CDU_RX_FIFO_RANGE_RMC_8r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CDU_RX_FIFO_RANGE_RMC_9r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CDU_RX_FIFO_RANGE_RMC_10r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CDU_RX_FIFO_RANGE_RMC_11r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CDU_RX_FIFO_RANGE_RMC_12r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CDU_RX_FIFO_RANGE_RMC_13r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CDU_RX_FIFO_RANGE_RMC_14r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CDU_RX_FIFO_RANGE_RMC_15r );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_clu_rx_fifo_range_rmc_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CLU_RX_FIFO_RANGE_RMC];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CLU_RX_FIFO_RANGE_RMC_0r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CLU_RX_FIFO_RANGE_RMC_1r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CLU_RX_FIFO_RANGE_RMC_2r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CLU_RX_FIFO_RANGE_RMC_3r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CLU_RX_FIFO_RANGE_RMC_4r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CLU_RX_FIFO_RANGE_RMC_5r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CLU_RX_FIFO_RANGE_RMC_6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CLU_RX_FIFO_RANGE_RMC_7r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CLU_RX_FIFO_RANGE_RMC_8r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CLU_RX_FIFO_RANGE_RMC_9r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CLU_RX_FIFO_RANGE_RMC_10r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CLU_RX_FIFO_RANGE_RMC_11r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CLU_RX_FIFO_RANGE_RMC_12r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CLU_RX_FIFO_RANGE_RMC_13r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CLU_RX_FIFO_RANGE_RMC_14r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CLU_RX_FIFO_RANGE_RMC_15r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdum_rx_rmc_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDUM_RX_RMC_MAP];
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_J2P_A0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDBM_RX_RMC_0_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDBM_RX_RMC_1_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDBM_RX_RMC_2_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDBM_RX_RMC_3_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDBM_RX_RMC_4_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDBM_RX_RMC_5_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDBM_RX_RMC_6_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDBM_RX_RMC_7_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CDBM_RX_RMC_8_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CDBM_RX_RMC_9_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CDBM_RX_RMC_10_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CDBM_RX_RMC_11_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CDBM_RX_RMC_12_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CDBM_RX_RMC_13_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CDBM_RX_RMC_14_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CDBM_RX_RMC_15_MAPr );
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDUM_RX_RMC_0_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDUM_RX_RMC_1_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDUM_RX_RMC_2_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDUM_RX_RMC_3_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDUM_RX_RMC_4_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDUM_RX_RMC_5_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDUM_RX_RMC_6_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDUM_RX_RMC_7_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CDUM_RX_RMC_8_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CDUM_RX_RMC_9_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CDUM_RX_RMC_10_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CDUM_RX_RMC_11_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CDUM_RX_RMC_12_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CDUM_RX_RMC_13_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CDUM_RX_RMC_14_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CDUM_RX_RMC_15_MAPr );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdu_rx_rmc_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDU_RX_RMC_MAP];
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_J2P_A0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDB_RX_RMC_0_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDB_RX_RMC_1_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDB_RX_RMC_2_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDB_RX_RMC_3_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDB_RX_RMC_4_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDB_RX_RMC_5_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDB_RX_RMC_6_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDB_RX_RMC_7_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CDB_RX_RMC_8_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CDB_RX_RMC_9_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CDB_RX_RMC_10_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CDB_RX_RMC_11_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CDB_RX_RMC_12_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CDB_RX_RMC_13_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CDB_RX_RMC_14_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CDB_RX_RMC_15_MAPr );
    }
    else
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDU_RX_RMC_0_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDU_RX_RMC_1_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDU_RX_RMC_2_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDU_RX_RMC_3_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDU_RX_RMC_4_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDU_RX_RMC_5_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDU_RX_RMC_6_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDU_RX_RMC_7_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CDU_RX_RMC_8_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CDU_RX_RMC_9_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CDU_RX_RMC_10_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CDU_RX_RMC_11_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CDU_RX_RMC_12_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CDU_RX_RMC_13_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CDU_RX_RMC_14_MAPr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CDU_RX_RMC_15_MAPr );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_clu_rx_rmc_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CLU_RX_RMC_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CLU_RX_RMC_0_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CLU_RX_RMC_1_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CLU_RX_RMC_2_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CLU_RX_RMC_3_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CLU_RX_RMC_4_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CLU_RX_RMC_5_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CLU_RX_RMC_6_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CLU_RX_RMC_7_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CLU_RX_RMC_8_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CLU_RX_RMC_9_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CLU_RX_RMC_10_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CLU_RX_RMC_11_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CLU_RX_RMC_12_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CLU_RX_RMC_13_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CLU_RX_RMC_14_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CLU_RX_RMC_15_MAPr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdum_rx_port_map_to_rmc_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDUM_RX_PORT_MAP_TO_RMC];
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_J2P_A0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 8 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDBM_RX_PORT_0_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDBM_RX_PORT_1_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDBM_RX_PORT_2_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDBM_RX_PORT_3_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDBM_RX_PORT_4_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDBM_RX_PORT_5_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDBM_RX_PORT_6_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDBM_RX_PORT_7_MAP_TO_RMCr );
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 8 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDUM_RX_PORT_0_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDUM_RX_PORT_1_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDUM_RX_PORT_2_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDUM_RX_PORT_3_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDUM_RX_PORT_4_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDUM_RX_PORT_5_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDUM_RX_PORT_6_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDUM_RX_PORT_7_MAP_TO_RMCr );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdu_rx_port_map_to_rmc_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDU_RX_PORT_MAP_TO_RMC];
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_J2P_A0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 8 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDB_RX_PORT_0_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDB_RX_PORT_1_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDB_RX_PORT_2_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDB_RX_PORT_3_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDB_RX_PORT_4_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDB_RX_PORT_5_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDB_RX_PORT_6_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDB_RX_PORT_7_MAP_TO_RMCr );
    }
    else
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 8 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDU_RX_PORT_0_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDU_RX_PORT_1_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDU_RX_PORT_2_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDU_RX_PORT_3_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDU_RX_PORT_4_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDU_RX_PORT_5_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDU_RX_PORT_6_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDU_RX_PORT_7_MAP_TO_RMCr );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_clu_rx_port_map_to_rmc_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CLU_RX_PORT_MAP_TO_RMC];
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_J2C_A0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CLU_RX_PORT_0_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CLU_RX_PORT_1_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CLU_RX_PORT_2_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CLU_RX_PORT_3_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CLU_RX_PORT_4_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CLU_RX_PORT_5_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CLU_RX_PORT_6_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CLU_RX_PORT_7_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CLU_RX_PORT_8_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CLU_RX_PORT_9_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CLU_RX_PORT_10_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CLU_RX_PORT_11_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CLU_RX_PORT_12_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CLU_RX_PORT_13_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CLU_RX_PORT_14_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CLU_RX_PORT_15_MAP_TO_RMCr );
    }
    else if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 12 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CLU_RX_PORT_0_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CLU_RX_PORT_1_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CLU_RX_PORT_2_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CLU_RX_PORT_3_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CLU_RX_PORT_4_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CLU_RX_PORT_5_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CLU_RX_PORT_6_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CLU_RX_PORT_7_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CLU_RX_PORT_8_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CLU_RX_PORT_9_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CLU_RX_PORT_10_MAP_TO_RMCr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CLU_RX_PORT_11_MAP_TO_RMCr );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_llvp_classification_group_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_LLVP_CLASSIFICATION_GROUP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_LLVPm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPE_LLR_LLVPm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_cfg_inlif_dp_profile_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_CFG_INLIF_DP_PROFILE_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 3 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_CFG_INLIF_DP_PROFILE_MAPm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_CFG_INLIF_DP_PROFILE_MAPm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , ETPPC_CFG_INLIF_DP_PROFILE_MAPm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_pcp_dei_dp_mapping_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_PCP_DEI_DP_MAPPING_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 3 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_PCP_DEI_DP_MAPPING_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_PCP_DEI_DP_MAPPING_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , ETPPC_PCP_DEI_DP_MAPPING_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vlan_edit_command_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VLAN_EDIT_COMMAND_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_INGRESS_VLAN_EDIT_COMMAND_TABLE_1m );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_INGRESS_VLAN_EDIT_COMMAND_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_ivec_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_IVEC_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 3 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_IVEC_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_IVEC_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , ETPPC_IVEC_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_vlan_edit_profile_map_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_VLAN_EDIT_PROFILE_MAP_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_VLAN_EDIT_PROFILE_MAP_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_VLAN_EDIT_PROFILE_MAP_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_vlan_range_compression_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_VLAN_RANGE_COMPRESSION_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VLAN_RANGE_COMPRESSION_TABLE_1m );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VLAN_RANGE_COMPRESSION_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_vlan_domain_attributes_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_VLAN_DOMAIN_ATTRIBUTES];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPF_VTT_VLAN_DOMAIN_ATTRIBUTESm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPA_VTT_VLAN_DOMAIN_ATTRIBUTES_1m );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_jericho_compatible_registers_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_JERICHO_COMPATIBLE_REGISTERS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_JERICHO_COMPATIBLE_REGISTERSr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_JERICHO_COMPATIBLE_REGISTERSr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_map_port_to_membership_if_vec_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_MAP_PORT_TO_MEMBERSHIP_IF_VEC];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_MAP_PORT_TO_MEMBERSHIP_IF_VECm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_MAP_PORT_TO_MEMBERSHIP_IF_VECm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_system_headers_cg_1_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_SYSTEM_HEADERS_CG_1];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_SYSTEM_HEADERS_CG_1r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_SYSTEM_HEADERS_CG_1r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_pph_base_fields_cg_2_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_PPH_BASE_FIELDS_CG_2];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_PPH_BASE_FIELDS_CG_2r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_PPH_BASE_FIELDS_CG_2r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_device_cfg_parser_enablers_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_DEVICE_CFG_PARSER_ENABLERS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_DEVICE_CFG_PARSER_ENABLERSr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_DEVICE_CFG_PARSER_ENABLERSr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vlan_membership_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VLAN_MEMBERSHIP_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VLAN_MEMBERSHIP_TABLE_1m );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VLAN_MEMBERSHIP_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_initial_pcp_dei_map_type_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_INITIAL_PCP_DEI_MAP_TYPE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_INITIAL_PCP_DEI_MAP_TYPEr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_INITIAL_PCP_DEI_MAP_TYPEr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vlan_edit_pcp_dei_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VLAN_EDIT_PCP_DEI_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VLAN_EDIT_PCP_DEI_MAPm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VLAN_EDIT_PCP_DEI_MAPm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_utility_regs_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_UTILITY_REGS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPA_UTILITY_REGSr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ERPP_UTILITY_REGSr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_lif_extension_format_cfg_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_LIF_EXTENSION_FORMAT_CFG];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_LIF_EXTENSION_FORMAT_CFGm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_LIF_EXTENSION_FORMAT_CFGm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_global_tpid_0_1_regs_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_GLOBAL_TPID_0_1_REGS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 5 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_REG_00E8r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_REG_00E8r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , ETPPC_REG_00E8r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , ERPP_REG_00E8r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , ECI_FAP_GLOBAL_PP_2r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_global_tpid_2_3_regs_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_GLOBAL_TPID_2_3_REGS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 5 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_REG_00E9r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_REG_00E9r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , ETPPC_REG_00E9r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , ERPP_REG_00E9r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , ECI_FAP_GLOBAL_PP_3r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_global_tpid_4_5_regs_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_GLOBAL_TPID_4_5_REGS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 5 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_REG_00F1r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_REG_00F1r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , ETPPC_REG_00F1r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , ERPP_REG_00F1r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , ECI_FAP_GLOBAL_PP_11r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_global_tpid_6_regs_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_GLOBAL_TPID_6_REGS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 5 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_REG_00F2r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_REG_00F2r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , ETPPC_REG_00F2r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , ERPP_REG_00F2r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , ECI_FAP_GLOBAL_PP_12r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_layer_protocol_mpls_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_CFG_LAYER_PROTOCOL_MPLS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPA_CFG_LAYER_PROTOCOL_MPLSr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CFG_LAYER_PROTOCOL_MPLSr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_layer_protocol_ipv4_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_CFG_LAYER_PROTOCOL_IPV4];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPA_CFG_LAYER_PROTOCOL_IPV4r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CFG_LAYER_PROTOCOL_IPV4r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_layer_protocol_ipv6_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_CFG_LAYER_PROTOCOL_IPV6];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPA_CFG_LAYER_PROTOCOL_IPV6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CFG_LAYER_PROTOCOL_IPV6r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_trap_strengths_3_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_TRAP_STRENGTHS_3];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_TRAP_STRENGTHS_3r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_TRAP_STRENGTHS_3r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_trap_strengths_4_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_TRAP_STRENGTHS_4];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_TRAP_STRENGTHS_4r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_TRAP_STRENGTHS_4r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_trap_strengths_5_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_TRAP_STRENGTHS_5];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_TRAP_STRENGTHS_5r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_TRAP_STRENGTHS_5r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_trap_strengths_6_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_TRAP_STRENGTHS_6];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_TRAP_STRENGTHS_6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_TRAP_STRENGTHS_6r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_trap_strengths_7_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_TRAP_STRENGTHS_7];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_TRAP_STRENGTHS_7r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_TRAP_STRENGTHS_7r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_trap_strengths_8_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_TRAP_STRENGTHS_8];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_TRAP_STRENGTHS_8r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_TRAP_STRENGTHS_8r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_action_profile_acceptable_frame_types_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPES];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPESr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPESr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_pipe_mapped_ttl_variable_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_CFG_PIPE_MAPPED_TTL_VARIABLE_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_CFG_PIPE_MAPPED_TTL_VARIABLE_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CFG_PIPE_MAPPED_TTL_VARIABLE_MAPr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_ttl_model_pipe_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_CFG_TTL_MODEL_PIPE_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_CFG_TTL_MODEL_PIPE_MAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CFG_TTL_MODEL_PIPE_MAPr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_qos_dp_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_QOS_DP_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_QOS_DP_MAPm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_QOS_DP_MAPm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_new_qos_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_NEW_QOS_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_NEW_QOS_MAPm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_NEW_QOS_MAPm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_enc_qos_var_type_mapping_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_ENC_QOS_VAR_TYPE_MAPPING];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 5 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPC_ENC_1_QOS_VAR_TYPE_MAPPING_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPB_ENC_2_QOS_VAR_TYPE_MAPPING_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , ETPPB_ENC_3_QOS_VAR_TYPE_MAPPING_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , ETPPB_ENC_4_QOS_VAR_TYPE_MAPPING_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , ETPPB_ENC_5_QOS_VAR_TYPE_MAPPING_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_map_fwd_layer_protocol_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_CFG_MAP_FWD_LAYER_PROTOCOL];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPA_CFG_MAP_FWD_LAYER_PROTOCOLr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CFG_MAP_FWD_LAYER_PROTOCOLr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_remark_protocol_profile_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_CFG_REMARK_PROTOCOL_PROFILE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPA_CFG_REMARK_PROTOCOL_PROFILEr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CFG_REMARK_PROTOCOL_PROFILEr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_layer_nwk_qos_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_LAYER_NWK_QOS_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPA_LAYER_NWK_QOS_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_LAYER_NWK_QOS_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_map_remark_profile_qos_var_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_CFG_MAP_REMARK_PROFILE_QOS_VAR];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_CFG_MAP_REMARK_PROFILE_QOS_VARr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CFG_MAP_REMARK_PROFILE_QOS_VARr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_map_remark_profile_qos_dp_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_CFG_MAP_REMARK_PROFILE_QOS_DP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_CFG_MAP_REMARK_PROFILE_QOS_DPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CFG_MAP_REMARK_PROFILE_QOS_DPr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_qos_model_mapping_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_QOS_MODEL_MAPPING_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_QOS_MODEL_MAPPING_TABLEr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_QOS_MODEL_MAPPING_TABLEr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_nwk_qos_idx_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_NWK_QOS_IDX_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_NWK_QOS_IDX_MAPm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_NWK_QOS_IDX_MAPm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_termination_profile_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_TERMINATION_PROFILE_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPF_TERMINATION_PROFILE_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPA_TERMINATION_PROFILE_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cfg_protocol_is_eth_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_CFG_PROTOCOL_IS_ETH];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_CFG_PROTOCOL_IS_ETHr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_CFG_PROTOCOL_IS_ETHr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cfg_protocol_is_ipv4_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_CFG_PROTOCOL_IS_IPV4];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_CFG_PROTOCOL_IS_IPV4r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_CFG_PROTOCOL_IS_IPV4r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cfg_protocol_is_ipv6_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_CFG_PROTOCOL_IS_IPV6];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_CFG_PROTOCOL_IS_IPV6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_CFG_PROTOCOL_IS_IPV6r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cfg_protocol_is_mpls_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_CFG_PROTOCOL_IS_MPLS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_CFG_PROTOCOL_IS_MPLSr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_CFG_PROTOCOL_IS_MPLSr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cfg_protocol_is_mac_in_mac_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_CFG_PROTOCOL_IS_MAC_IN_MAC];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_CFG_PROTOCOL_IS_MAC_IN_MACr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_CFG_PROTOCOL_IS_MAC_IN_MACr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_ethernet_termination_action_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_ETHERNET_TERMINATION_ACTION];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_ETHERNET_TERMINATION_ACTIONr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_ETHERNET_TERMINATION_ACTIONr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdum_rx_rmc_prd_thresholds_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDUM_RX_RMC_PRD_THRESHOLDS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDUM_RX_RMC_0_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDUM_RX_RMC_1_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDUM_RX_RMC_2_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDUM_RX_RMC_3_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDUM_RX_RMC_4_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDUM_RX_RMC_5_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDUM_RX_RMC_6_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDUM_RX_RMC_7_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CDUM_RX_RMC_8_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CDUM_RX_RMC_9_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CDUM_RX_RMC_10_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CDUM_RX_RMC_11_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CDUM_RX_RMC_12_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CDUM_RX_RMC_13_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CDUM_RX_RMC_14_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CDUM_RX_RMC_15_PRD_THRESHOLDS_CONFIGr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdu_rx_rmc_prd_thresholds_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDU_RX_RMC_PRD_THRESHOLDS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDU_RX_RMC_0_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDU_RX_RMC_1_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDU_RX_RMC_2_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDU_RX_RMC_3_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDU_RX_RMC_4_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDU_RX_RMC_5_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDU_RX_RMC_6_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDU_RX_RMC_7_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CDU_RX_RMC_8_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CDU_RX_RMC_9_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CDU_RX_RMC_10_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CDU_RX_RMC_11_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CDU_RX_RMC_12_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CDU_RX_RMC_13_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CDU_RX_RMC_14_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CDU_RX_RMC_15_PRD_THRESHOLDS_CONFIGr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdbm_rx_rmc_prd_thresholds_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDBM_RX_RMC_PRD_THRESHOLDS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDBM_RX_RMC_0_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDBM_RX_RMC_1_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDBM_RX_RMC_2_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDBM_RX_RMC_3_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDBM_RX_RMC_4_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDBM_RX_RMC_5_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDBM_RX_RMC_6_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDBM_RX_RMC_7_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CDBM_RX_RMC_8_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CDBM_RX_RMC_9_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CDBM_RX_RMC_10_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CDBM_RX_RMC_11_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CDBM_RX_RMC_12_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CDBM_RX_RMC_13_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CDBM_RX_RMC_14_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CDBM_RX_RMC_15_PRD_THRESHOLDS_CONFIGr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdb_rx_rmc_prd_thresholds_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDB_RX_RMC_PRD_THRESHOLDS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDB_RX_RMC_0_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDB_RX_RMC_1_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDB_RX_RMC_2_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDB_RX_RMC_3_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CDB_RX_RMC_4_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CDB_RX_RMC_5_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CDB_RX_RMC_6_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CDB_RX_RMC_7_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CDB_RX_RMC_8_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CDB_RX_RMC_9_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CDB_RX_RMC_10_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CDB_RX_RMC_11_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CDB_RX_RMC_12_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CDB_RX_RMC_13_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CDB_RX_RMC_14_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CDB_RX_RMC_15_PRD_THRESHOLDS_CONFIGr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_clu_rx_rmc_prd_thresholds_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CLU_RX_RMC_PRD_THRESHOLDS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CLU_RX_RMC_0_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CLU_RX_RMC_1_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CLU_RX_RMC_2_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CLU_RX_RMC_3_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CLU_RX_RMC_4_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CLU_RX_RMC_5_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CLU_RX_RMC_6_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CLU_RX_RMC_7_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CLU_RX_RMC_8_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CLU_RX_RMC_9_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CLU_RX_RMC_10_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CLU_RX_RMC_11_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CLU_RX_RMC_12_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CLU_RX_RMC_13_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CLU_RX_RMC_14_PRD_THRESHOLDS_CONFIGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CLU_RX_RMC_15_PRD_THRESHOLDS_CONFIGr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ilu_rx_hrf_prd_thresholds_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ILU_RX_HRF_PRD_THRESHOLDS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ILU_HRF_RX_PRD_THRESHOLDS_CONFIG_0r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ILU_HRF_RX_PRD_THRESHOLDS_CONFIG_1r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_clu_rx_port_map_to_parser_profile_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CLU_RX_PORT_MAP_TO_PARSER_PROFILE];
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_J2C_A0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CLU_RX_PORT_0_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CLU_RX_PORT_1_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CLU_RX_PORT_2_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CLU_RX_PORT_3_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CLU_RX_PORT_4_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CLU_RX_PORT_5_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CLU_RX_PORT_6_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CLU_RX_PORT_7_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CLU_RX_PORT_8_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CLU_RX_PORT_9_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CLU_RX_PORT_10_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CLU_RX_PORT_11_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , CLU_RX_PORT_12_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , CLU_RX_PORT_13_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , CLU_RX_PORT_14_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , CLU_RX_PORT_15_MAP_TO_PARSER_PROFILEr );
    }
    else if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 12 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CLU_RX_PORT_0_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CLU_RX_PORT_1_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CLU_RX_PORT_2_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CLU_RX_PORT_3_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , CLU_RX_PORT_4_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , CLU_RX_PORT_5_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , CLU_RX_PORT_6_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , CLU_RX_PORT_7_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , CLU_RX_PORT_8_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , CLU_RX_PORT_9_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , CLU_RX_PORT_10_MAP_TO_PARSER_PROFILEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , CLU_RX_PORT_11_MAP_TO_PARSER_PROFILEr );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdum_parser_soft_stage_tcam_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDUM_PARSER_SOFT_STAGE_TCAM];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 4 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDUM_PARSER_SOFT_TCAM_0_TO_7r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDUM_PARSER_SOFT_TCAM_8_TO_15r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDUM_PARSER_SOFT_TCAM_16_TO_23r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDUM_PARSER_SOFT_TCAM_24_TO_31r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdu_parser_soft_stage_tcam_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDU_PARSER_SOFT_STAGE_TCAM];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 4 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDU_PARSER_SOFT_TCAM_0_TO_7r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDU_PARSER_SOFT_TCAM_8_TO_15r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDU_PARSER_SOFT_TCAM_16_TO_23r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDU_PARSER_SOFT_TCAM_24_TO_31r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdbm_parser_soft_stage_tcam_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDBM_PARSER_SOFT_STAGE_TCAM];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 4 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDBM_PARSER_SOFT_TCAM_0_TO_7r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDBM_PARSER_SOFT_TCAM_8_TO_15r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDBM_PARSER_SOFT_TCAM_16_TO_23r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDBM_PARSER_SOFT_TCAM_24_TO_31r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cdb_parser_soft_stage_tcam_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CDB_PARSER_SOFT_STAGE_TCAM];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 4 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CDB_PARSER_SOFT_TCAM_0_TO_7r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CDB_PARSER_SOFT_TCAM_8_TO_15r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CDB_PARSER_SOFT_TCAM_16_TO_23r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CDB_PARSER_SOFT_TCAM_24_TO_31r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_clu_parser_soft_stage_tcam_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CLU_PARSER_SOFT_STAGE_TCAM];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 4 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , CLU_PARSER_SOFT_TCAM_0_TO_7r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , CLU_PARSER_SOFT_TCAM_8_TO_15r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , CLU_PARSER_SOFT_TCAM_16_TO_23r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , CLU_PARSER_SOFT_TCAM_24_TO_31r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_feu_parser_soft_stage_tcam_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_FEU_PARSER_SOFT_STAGE_TCAM];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 4 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , FPRD_PARSER_SOFT_TCAM_0_TO_7r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , FPRD_PARSER_SOFT_TCAM_8_TO_15r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , FPRD_PARSER_SOFT_TCAM_16_TO_23r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , FPRD_PARSER_SOFT_TCAM_24_TO_31r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ilu_parser_soft_stage_tcam_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ILU_PARSER_SOFT_STAGE_TCAM];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 4 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ILU_PARSER_SOFT_TCAM_0_TO_7r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ILU_PARSER_SOFT_TCAM_8_TO_15r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , ILU_PARSER_SOFT_TCAM_16_TO_23r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , ILU_PARSER_SOFT_TCAM_24_TO_31r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_ethernet_fwd_code_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_ETHERNET_FWD_CODE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPA_ETHERNET_FWD_CODEr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_ETHERNET_FWD_CODEr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_sit_profile_map_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_SIT_PROFILE_MAP_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_SIT_PROFILE_MAP_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_SIT_PROFILE_MAP_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vlan_membership_if_attr_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VLAN_MEMBERSHIP_IF_ATTR];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VLAN_MEMBERSHIP_IF_ATTRm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VLAN_MEMBERSHIP_IF_ATTRm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_map_port_to_learn_lif_vec_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_MAP_PORT_TO_LEARN_LIF_VEC];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_MAP_PORT_TO_LEARN_LIF_VECr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_MAP_PORT_TO_LEARN_LIF_VECr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_map_port_to_learn_lif_vec_low_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_MAP_PORT_TO_LEARN_LIF_VEC_LOW];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_MAP_PORT_TO_LEARN_LIF_VEC_LSBr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_MAP_PORT_TO_LEARN_LIF_VEC_LSBr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_map_port_to_learn_lif_vec_high_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_MAP_PORT_TO_LEARN_LIF_VEC_HIGH];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_MAP_PORT_TO_LEARN_LIF_VEC_MSBr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_MAP_PORT_TO_LEARN_LIF_VEC_MSBr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_current_next_protocol_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_CURRENT_NEXT_PROTOCOL_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_CURRENT_NEXT_PROTOCOL_MAPm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CURRENT_NEXT_PROTOCOL_MAPm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ettp_main_header_map_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETTP_MAIN_HEADER_MAP_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPC_MAIN_HEADER_MAP_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPB_MAIN_HEADER_MAP_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_additional_headers_map_table_0_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_ADDITIONAL_HEADERS_MAP_TABLE_0];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPC_ADDITIONAL_HEADERS_MAP_TABLE_0m );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPB_ADDITIONAL_HEADERS_MAP_TABLE_0m );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_additional_headers_map_table_1_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_ADDITIONAL_HEADERS_MAP_TABLE_1];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPC_ADDITIONAL_HEADERS_MAP_TABLE_1m );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPB_ADDITIONAL_HEADERS_MAP_TABLE_1m );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_forward_code_ipv4_or_ipv6_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_FORWARD_CODE_IPV4_OR_IPV6];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_CFG_FORWARD_CODE_IPV4_OR_IPV6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CFG_FORWARD_CODE_IPV4_OR_IPV6r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_additional_headers_map_table_2_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_ADDITIONAL_HEADERS_MAP_TABLE_2];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPC_ADDITIONAL_HEADERS_MAP_TABLE_2m );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPB_ADDITIONAL_HEADERS_MAP_TABLE_2m );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_fwd_map_protocol_qos_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_CFG_FWD_MAP_PROTOCOL_QOS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 3 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPC_CFG_FWD_MAP_PROTOCOL_QOSm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CFG_ENC_MAP_PROTOCOL_QOSm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , ETPPB_CFG_ENC_MAP_PROTOCOL_QOSm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_special_labels_profile_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_SPECIAL_LABELS_PROFILE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_SPECIAL_LABELS_PROFILEr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_SPECIAL_LABELS_PROFILEr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_mpls_special_label_attributes_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_MPLS_SPECIAL_LABEL_ATTRIBUTES];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_MPLS_SPECIAL_LABEL_ATTRIBUTESr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_MPLS_SPECIAL_LABEL_ATTRIBUTESr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_mpls_control_word_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_CFG_MPLS_CONTROL_WORD];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_CFG_MPLS_CONTROL_WORDr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CFG_MPLS_CONTROL_WORDr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cs_inlif_profile_map_lsb_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_CS_INLIF_PROFILE_MAP_LSB];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPF_CS_INLIF_PROFILE_MAP_LSBr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPA_CS_INLIF_PROFILE_MAP_LSBr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_fwd_domain_cs_profile_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_FWD_DOMAIN_CS_PROFILE_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_FWD_DOMAIN_CS_PROFILE_MAPm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_FWD_DOMAIN_CS_PROFILE_MAPm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cs_inlif_profile_map_msb_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_CS_INLIF_PROFILE_MAP_MSB];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPF_CS_INLIF_PROFILE_MAP_MSBr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPA_CS_INLIF_PROFILE_MAP_MSBr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_lif_profile_attributes_1_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_LIF_PROFILE_ATTRIBUTES_1];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_LIF_PROFILE_ATTRIBUTES_1m );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_LIF_PROFILE_ATTRIBUTES_1m );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_itpp_sniff_sys_header_info_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ITPP_SNIFF_SYS_HEADER_INFO];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ITPP_SNIFF_SYS_HEADER_INFOm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ITPPD_SNIFF_SYS_HEADER_INFOm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_mapping_queue_type_to_sniff_packet_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_MAPPING_QUEUE_TYPE_TO_SNIFF_PACKET];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ITPP_MAPPING_QUEUE_TYPE_TO_SNIFF_PACKETr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ITPPD_MAPPING_QUEUE_TYPE_TO_SNIFF_PACKETr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_eci_fap_global_sys_header_cfg_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ECI_FAP_GLOBAL_SYS_HEADER_CFG];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 13 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ECI_FAP_GLOBAL_SYS_HEADER_CFGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ERPP_REG_00C6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , ETPPA_REG_00C6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , ETPPB_REG_00C6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , ETPPC_REG_00C6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , IPPD_REG_00C6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , IPT_REG_00C6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , IRE_REG_00C6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , ITPP_REG_00C6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , ITPPD_REG_00C6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , OAMP_REG_00C6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , RQP_REG_00C6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , DDP_REG_00C6r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_oamp_pe_prog_tcam_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_OAMP_PE_PROG_TCAM];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , OAMP_PE_0_PROG_TCAMm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , OAMP_PE_1_PROG_TCAMm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_oamp_pe_gen_mem_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_OAMP_PE_GEN_MEM];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 1 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , OAMP_PE_GEN_MEMm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vd_to_vlan_classification_profile_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VD_TO_VLAN_CLASSIFICATION_PROFILE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VD_TO_VLAN_CLASSIFICATION_PROFILEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VD_TO_VLAN_CLASSIFICATION_PROFILEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vrid_my_mac_tcam_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VRID_MY_MAC_TCAM];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPF_VRID_MY_MAC_TCAMm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPA_VRID_MY_MAC_TCAM_1m );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_map_vrid_to_ip_version_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_MAP_VRID_TO_IP_VERSION];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPF_MAP_VRID_TO_IP_VERSIONr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPA_MAP_VRID_TO_IP_VERSIONr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vrid_my_mac_mapping_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VRID_MY_MAC_MAPPING];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPF_VRID_MY_MAC_MAPPINGm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPA_VRID_MY_MAC_MAPPING_1m );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_cfg_fhei_upper_layer_type_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_CFG_FHEI_UPPER_LAYER_TYPE_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_CFG_FHEI_UPPER_LAYER_TYPE_MAPm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_CFG_FHEI_UPPER_LAYER_TYPE_MAPm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_cfg_inlif_profile_mapped_in_jericho_mode_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_CFG_INLIF_PROFILE_MAPPED_IN_JERICHO_MODE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_CFG_INLIF_PROFILE_MAPPED_IN_JERICHO_MODEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_CFG_INLIF_PROFILE_MAPPED_IN_JERICHO_MODEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_parsing_start_type_mapping_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_PARSING_START_TYPE_MAPPING_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_PARSING_START_TYPE_MAPPING_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_PARSING_START_TYPE_MAPPING_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_fhei_mpls_command_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_FHEI_MPLS_COMMAND];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_FHEI_MPLS_COMMANDr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_FHEI_MPLS_COMMANDr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_cfg_fhei_model_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_CFG_FHEI_MODEL];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_CFG_FHEI_MODELr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_CFG_FHEI_MODELr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_fwd_layer_additional_info_to_ip_mc_eligible_mapping_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_FWD_LAYER_ADDITIONAL_INFO_TO_IP_MC_ELIGIBLE_MAPPING];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPA_FWD_LAYER_ADDITIONAL_INFO_TO_IP_MC_ELIGIBLE_MAPPINGr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ERPP_FWD_LAYER_ADDITIONAL_INFO_TO_IP_MC_ELIGIBLE_MAPPINGr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_fhei_mpls_upper_layer_protocol_to_pes_mapping_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_FHEI_MPLS_UPPER_LAYER_PROTOCOL_TO_PES_MAPPING_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPA_FHEI_MPLS_UPPER_LAYER_PROTOCOL_TO_PES_MAPPING_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ERPP_FHEI_MPLS_UPPER_LAYER_PROTOCOL_TO_PES_MAPPING_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_mtu_map_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_MTU_MAP_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_MTU_MAP_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_MTU_MAP_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_etpp_protocol_type_compressed_map_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_ETPP_PROTOCOL_TYPE_COMPRESSED_MAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_PROTOCOL_TYPE_COMPRESSED_MAPm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_PROTOCOL_TYPE_COMPRESSED_MAPm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_egress_acceptable_frame_type_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_EGRESS_ACCEPTABLE_FRAME_TYPE_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ERPP_ACCEPTABLE_FRAME_TYPE_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPA_ACCEPTABLE_FRAME_TYPE_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_lif_traps_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_LIF_TRAPS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPF_LIF_TRAPSr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPA_LIF_TRAPSr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_ethernet_traps_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_ETHERNET_TRAPS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_ETHERNET_TRAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_ETHERNET_TRAPr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_ethernet_trap_action_profile_vector_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_ETHERNET_TRAP_ACTION_PROFILE_VECTOR];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_ETHERNET_TRAP_ACTION_PROFILE_VECTORr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_ETHERNET_TRAP_ACTION_PROFILE_VECTORr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_map_statistic_object_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_MAP_STATISTIC_OBJECT_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPC_MAP_STATISTIC_OBJECTm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPD_MAP_STATISTIC_OBJECTm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_mpls_label_range_profile_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_MPLS_LABEL_RANGE_PROFILE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_MPLS_LABEL_RANGE_PROFILEr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_MPLS_LABEL_RANGE_PROFILEr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_mpls_label_range_table_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_VTT_MPLS_LABEL_RANGE_TABLE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , FALSE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_MPLS_LABEL_RANGE_TABLEm );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_MPLS_LABEL_RANGE_TABLEm );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_statistics_info_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_STATISTICS_INFO];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_STATISTICS_INFOr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_STATISTICS_INFOr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_ingress_stat_obj_profile_attr_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INGRESS_STAT_OBJ_PROFILE_ATTR];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_STAT_OBJ_PROFILE_ATTRr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_STAT_OBJ_PROFILE_ATTRr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_fabric_mesh_topology_links_status_info_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_FABRIC_MESH_TOPOLOGY_LINKS_STATUS_INFO];
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_J2C_A0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 24 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , MESH_TOPOLOGY_AUTO_DOC_NAME_38r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , MESH_TOPOLOGY_AUTO_DOC_NAME_39r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , MESH_TOPOLOGY_AUTO_DOC_NAME_40r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , MESH_TOPOLOGY_AUTO_DOC_NAME_41r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , MESH_TOPOLOGY_AUTO_DOC_NAME_42r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , MESH_TOPOLOGY_AUTO_DOC_NAME_43r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , MESH_TOPOLOGY_AUTO_DOC_NAME_44r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , MESH_TOPOLOGY_AUTO_DOC_NAME_45r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , MESH_TOPOLOGY_AUTO_DOC_NAME_46r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , MESH_TOPOLOGY_AUTO_DOC_NAME_47r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , MESH_TOPOLOGY_AUTO_DOC_NAME_48r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , MESH_TOPOLOGY_AUTO_DOC_NAME_49r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , MESH_TOPOLOGY_AUTO_DOC_NAME_50r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , MESH_TOPOLOGY_AUTO_DOC_NAME_51r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , MESH_TOPOLOGY_AUTO_DOC_NAME_52r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , MESH_TOPOLOGY_AUTO_DOC_NAME_53r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 16 , MESH_TOPOLOGY_AUTO_DOC_NAME_54r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 17 , MESH_TOPOLOGY_AUTO_DOC_NAME_55r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 18 , MESH_TOPOLOGY_AUTO_DOC_NAME_56r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 19 , MESH_TOPOLOGY_AUTO_DOC_NAME_57r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 20 , MESH_TOPOLOGY_AUTO_DOC_NAME_58r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 21 , MESH_TOPOLOGY_AUTO_DOC_NAME_59r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 22 , MESH_TOPOLOGY_AUTO_DOC_NAME_60r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 23 , MESH_TOPOLOGY_AUTO_DOC_NAME_61r );
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 56 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , MESH_TOPOLOGY_AUTO_DOC_NAME_38r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , MESH_TOPOLOGY_AUTO_DOC_NAME_39r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , MESH_TOPOLOGY_AUTO_DOC_NAME_40r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , MESH_TOPOLOGY_AUTO_DOC_NAME_41r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , MESH_TOPOLOGY_AUTO_DOC_NAME_42r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , MESH_TOPOLOGY_AUTO_DOC_NAME_43r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , MESH_TOPOLOGY_AUTO_DOC_NAME_44r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , MESH_TOPOLOGY_AUTO_DOC_NAME_45r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , MESH_TOPOLOGY_AUTO_DOC_NAME_46r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , MESH_TOPOLOGY_AUTO_DOC_NAME_47r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , MESH_TOPOLOGY_AUTO_DOC_NAME_48r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , MESH_TOPOLOGY_AUTO_DOC_NAME_49r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , MESH_TOPOLOGY_AUTO_DOC_NAME_50r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , MESH_TOPOLOGY_AUTO_DOC_NAME_51r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , MESH_TOPOLOGY_AUTO_DOC_NAME_52r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , MESH_TOPOLOGY_AUTO_DOC_NAME_53r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 16 , MESH_TOPOLOGY_AUTO_DOC_NAME_54r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 17 , MESH_TOPOLOGY_AUTO_DOC_NAME_55r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 18 , MESH_TOPOLOGY_AUTO_DOC_NAME_56r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 19 , MESH_TOPOLOGY_AUTO_DOC_NAME_57r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 20 , MESH_TOPOLOGY_AUTO_DOC_NAME_58r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 21 , MESH_TOPOLOGY_AUTO_DOC_NAME_59r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 22 , MESH_TOPOLOGY_AUTO_DOC_NAME_60r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 23 , MESH_TOPOLOGY_AUTO_DOC_NAME_61r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 24 , MESH_TOPOLOGY_AUTO_DOC_NAME_62r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 25 , MESH_TOPOLOGY_AUTO_DOC_NAME_63r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 26 , MESH_TOPOLOGY_AUTO_DOC_NAME_64r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 27 , MESH_TOPOLOGY_AUTO_DOC_NAME_65r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 28 , MESH_TOPOLOGY_AUTO_DOC_NAME_66r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 29 , MESH_TOPOLOGY_AUTO_DOC_NAME_67r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 30 , MESH_TOPOLOGY_AUTO_DOC_NAME_68r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 31 , MESH_TOPOLOGY_AUTO_DOC_NAME_69r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 32 , MESH_TOPOLOGY_AUTO_DOC_NAME_70r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 33 , MESH_TOPOLOGY_AUTO_DOC_NAME_71r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 34 , MESH_TOPOLOGY_AUTO_DOC_NAME_72r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 35 , MESH_TOPOLOGY_AUTO_DOC_NAME_73r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 36 , MESH_TOPOLOGY_AUTO_DOC_NAME_74r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 37 , MESH_TOPOLOGY_AUTO_DOC_NAME_75r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 38 , MESH_TOPOLOGY_AUTO_DOC_NAME_76r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 39 , MESH_TOPOLOGY_AUTO_DOC_NAME_77r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 40 , MESH_TOPOLOGY_AUTO_DOC_NAME_78r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 41 , MESH_TOPOLOGY_AUTO_DOC_NAME_79r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 42 , MESH_TOPOLOGY_AUTO_DOC_NAME_80r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 43 , MESH_TOPOLOGY_AUTO_DOC_NAME_81r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 44 , MESH_TOPOLOGY_AUTO_DOC_NAME_82r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 45 , MESH_TOPOLOGY_AUTO_DOC_NAME_83r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 46 , MESH_TOPOLOGY_AUTO_DOC_NAME_84r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 47 , MESH_TOPOLOGY_AUTO_DOC_NAME_85r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 48 , MESH_TOPOLOGY_AUTO_DOC_NAME_86r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 49 , MESH_TOPOLOGY_AUTO_DOC_NAME_87r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 50 , MESH_TOPOLOGY_AUTO_DOC_NAME_88r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 51 , MESH_TOPOLOGY_AUTO_DOC_NAME_89r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 52 , MESH_TOPOLOGY_AUTO_DOC_NAME_90r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 53 , MESH_TOPOLOGY_AUTO_DOC_NAME_91r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 54 , MESH_TOPOLOGY_AUTO_DOC_NAME_92r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 55 , MESH_TOPOLOGY_AUTO_DOC_NAME_93r );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_vtt_mpls_preprocessing_bos_or_ttl_trap_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_VTT_MPLS_PREPROCESSING_BOS_OR_TTL_TRAP];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , IPPA_VTT_MPLS_PREPROCESSING_BOS_OR_TTL_TRAPr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , IPPF_VTT_MPLS_PREPROCESSING_BOS_OR_TTL_TRAPr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_fabric_dtq_base_address_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_FABRIC_DTQ_BASE_ADDRESS];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 6 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , FDT_DTQ_BADDR_0r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , FDT_DTQ_BADDR_1r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , FDT_DTQ_BADDR_2r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , FDT_DTQ_BADDR_3r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , FDT_DTQ_BADDR_4r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , FDT_DTQ_BADDR_5r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_sch_if_calendar_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_SCH_IF_CALENDAR];
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 32 , FALSE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_0m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_1m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_2m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_3m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_4m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_5m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_6m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_7m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_8m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_9m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_10m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_11m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_12m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_13m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_14m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_15m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 16 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_16m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 17 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_17m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 18 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_18m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 19 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_19m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 20 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_20m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 21 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_21m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 22 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_22m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 23 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_23m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 24 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_24m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 25 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_25m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 26 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_26m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 27 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_27m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 28 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_28m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 29 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_29m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 30 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_30m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 31 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_31m );
    }
    else
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 16 , FALSE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_0m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_1m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_2m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_3m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_4m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_5m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_6m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_7m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 8 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_8m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 9 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_9m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 10 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_10m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 11 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_11m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 12 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_12m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 13 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_13m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 14 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_14m );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 15 , SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_15m );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_cfg_stat_metadata_current_protocol_type_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_CFG_STAT_METADATA_CURRENT_PROTOCOL_TYPE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 2 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , ETPPB_CFG_STAT_METADATA_CURRENT_PROTOCOL_TYPEr );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , ETPPC_CFG_STAT_METADATA_CURRENT_PROTOCOL_TYPEr );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_macsec_isec_mgmtrule_cfg_da_type_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_MACSEC_ISEC_MGMTRULE_CFG_DA_TYPE];
    SHR_FUNC_INIT_VARS(unit);
    dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 8 , TRUE );
    
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , MACSEC_ISEC_MGMTRULE_CFG_DA_0r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , MACSEC_ISEC_MGMTRULE_CFG_DA_1r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , MACSEC_ISEC_MGMTRULE_CFG_DA_2r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , MACSEC_ISEC_MGMTRULE_CFG_DA_3r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , MACSEC_ISEC_MGMTRULE_CFG_DA_4r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 5 , MACSEC_ISEC_MGMTRULE_CFG_DA_5r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 6 , MACSEC_ISEC_MGMTRULE_CFG_DA_6r );
    dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 7 , MACSEC_ISEC_MGMTRULE_CFG_DA_7r );
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_hw_entity_groups_hw_entity_groups_indirect_force_bubble_ignore_pulse_width_init(
int unit,
hl_group_info_t * group_info)
{
    hl_group_info_t * group_info_entry = &group_info[DBAL_HW_ENTITY_GROUP_INDIRECT_FORCE_BUBBLE_IGNORE_PULSE_WIDTH];
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_J2P_A0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 5 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , DDHA_INDIRECT_FORCE_BUBBLEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , DDHB_INDIRECT_FORCE_BUBBLEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , MDB_INDIRECT_FORCE_BUBBLEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , MACT_INDIRECT_FORCE_BUBBLEr );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , KAPS_INDIRECT_FORCE_BUBBLEr );
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_db_init_hw_entity_group_general_info_set(unit, group_info_entry, 5 , TRUE );
        
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 0 , DDHA_RESERVED_SPARE_1r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 1 , DDHB_RESERVED_SPARE_1r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 2 , MDB_RESERVED_SPARE_1r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 3 , MACT_RESERVED_SPARE_1r );
        dbal_db_init_hw_entity_group_reg_mem_name_set(unit, group_info_entry, 4 , KAPS_RESERVED_SPARE_1r );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_hw_entity_groups_hw_entity_groups_init(
int unit,
hl_group_info_t * group_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_tdm_egress_configuration_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_qos_propag_profile_mapping_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_special_label_values_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vsi_profile_attributes_0_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_itpp_general_cfg_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_arr_prefix_group_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_itpp_mapping_fwd_info_to_en_term_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vsi_profile_attributes_1_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_learn_key_context_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdum_rx_fifo_range_rmc_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_default_svtag_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_source_adrs_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_enc_device_configs_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_additional_headers_profile_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_ingress_trapped_by_fhei_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdum_mac_reset_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdu_mac_reset_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_clu_pm_reset_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdu_rx_fifo_range_rmc_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_clu_rx_fifo_range_rmc_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdum_rx_rmc_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdu_rx_rmc_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_clu_rx_rmc_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdum_rx_port_map_to_rmc_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdu_rx_port_map_to_rmc_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_clu_rx_port_map_to_rmc_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_llvp_classification_group_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_cfg_inlif_dp_profile_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_pcp_dei_dp_mapping_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vlan_edit_command_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_ivec_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_vlan_edit_profile_map_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_vlan_range_compression_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_vlan_domain_attributes_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_jericho_compatible_registers_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_map_port_to_membership_if_vec_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_system_headers_cg_1_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_pph_base_fields_cg_2_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_device_cfg_parser_enablers_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vlan_membership_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_initial_pcp_dei_map_type_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vlan_edit_pcp_dei_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_utility_regs_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_lif_extension_format_cfg_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_global_tpid_0_1_regs_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_global_tpid_2_3_regs_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_global_tpid_4_5_regs_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_global_tpid_6_regs_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_layer_protocol_mpls_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_layer_protocol_ipv4_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_layer_protocol_ipv6_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_trap_strengths_3_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_trap_strengths_4_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_trap_strengths_5_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_trap_strengths_6_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_trap_strengths_7_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_trap_strengths_8_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_action_profile_acceptable_frame_types_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_pipe_mapped_ttl_variable_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_ttl_model_pipe_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_qos_dp_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_new_qos_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_enc_qos_var_type_mapping_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_map_fwd_layer_protocol_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_remark_protocol_profile_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_layer_nwk_qos_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_map_remark_profile_qos_var_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_map_remark_profile_qos_dp_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_qos_model_mapping_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_nwk_qos_idx_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_termination_profile_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cfg_protocol_is_eth_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cfg_protocol_is_ipv4_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cfg_protocol_is_ipv6_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cfg_protocol_is_mpls_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cfg_protocol_is_mac_in_mac_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_ethernet_termination_action_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdum_rx_rmc_prd_thresholds_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdu_rx_rmc_prd_thresholds_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdbm_rx_rmc_prd_thresholds_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdb_rx_rmc_prd_thresholds_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_clu_rx_rmc_prd_thresholds_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ilu_rx_hrf_prd_thresholds_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_clu_rx_port_map_to_parser_profile_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdum_parser_soft_stage_tcam_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdu_parser_soft_stage_tcam_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdbm_parser_soft_stage_tcam_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cdb_parser_soft_stage_tcam_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_clu_parser_soft_stage_tcam_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_feu_parser_soft_stage_tcam_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ilu_parser_soft_stage_tcam_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_ethernet_fwd_code_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_sit_profile_map_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vlan_membership_if_attr_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_map_port_to_learn_lif_vec_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_map_port_to_learn_lif_vec_low_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_map_port_to_learn_lif_vec_high_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_current_next_protocol_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ettp_main_header_map_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_additional_headers_map_table_0_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_additional_headers_map_table_1_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_forward_code_ipv4_or_ipv6_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_additional_headers_map_table_2_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_fwd_map_protocol_qos_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_special_labels_profile_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_mpls_special_label_attributes_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_cfg_mpls_control_word_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cs_inlif_profile_map_lsb_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_fwd_domain_cs_profile_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_cs_inlif_profile_map_msb_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_lif_profile_attributes_1_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_itpp_sniff_sys_header_info_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_mapping_queue_type_to_sniff_packet_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_eci_fap_global_sys_header_cfg_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_oamp_pe_prog_tcam_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_oamp_pe_gen_mem_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vd_to_vlan_classification_profile_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vrid_my_mac_tcam_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_map_vrid_to_ip_version_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vrid_my_mac_mapping_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_cfg_fhei_upper_layer_type_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_cfg_inlif_profile_mapped_in_jericho_mode_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_parsing_start_type_mapping_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_fhei_mpls_command_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_cfg_fhei_model_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_fwd_layer_additional_info_to_ip_mc_eligible_mapping_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_fhei_mpls_upper_layer_protocol_to_pes_mapping_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_mtu_map_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_etpp_protocol_type_compressed_map_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_egress_acceptable_frame_type_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_lif_traps_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_ethernet_traps_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_ethernet_trap_action_profile_vector_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_map_statistic_object_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_mpls_label_range_profile_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_vtt_mpls_label_range_table_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_statistics_info_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_ingress_stat_obj_profile_attr_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_fabric_mesh_topology_links_status_info_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_vtt_mpls_preprocessing_bos_or_ttl_trap_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_fabric_dtq_base_address_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_sch_if_calendar_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_cfg_stat_metadata_current_protocol_type_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_macsec_isec_mgmtrule_cfg_da_type_init(unit, group_info));
    SHR_IF_ERR_EXIT(_dbal_init_hw_entity_groups_hw_entity_groups_indirect_force_bubble_ignore_pulse_width_init(unit, group_info));
exit:
    SHR_FUNC_EXIT;
}
