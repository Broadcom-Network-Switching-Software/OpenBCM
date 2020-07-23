
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 

#include <soc/schanmsg.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/sand/sand_aux_access.h>

#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include "appl_ref_field_itmh_pph_init.h"


#include <bcm/field.h>

#include <sal/appl/sal.h>
#include <sal/core/libc.h>








static field_itmh_pph_qual_info_t field_itmh_pph_ipmf2_qual_info_array[ITMH_PPH_NOF_IPMF2_QUALIFIERS] = {
  
    {"tc_sn_dst_dp_mr_pph_q_pph", 0, TC_SNP_DST_DP_MIR_PPH_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, TC_SNP_DST_DP_MIR_PPH_QUAL_OFFSET}},
    {"ext_exist_q_pph", 0, EXT_PRESENT_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, EXT_PRESENT_QUAL_OFFSET}},
    {"out_lif_dst_q_pph", 0, OUTLIF_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, OUTLIF_QUAL_OFFSET}},
    {"ase_inj_q_pph", 0, ASE_INJ_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, ASE_INJ_QUAL_OFFSET}},
    {"time_stamp_q_pph", 0, TIME_STAMP_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 2, TIME_STAMP_QUAL_OFFSET}},
    {"fwd_trap_code_zero_q_pph", 0, TRAP_CODE_ZERO_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 0, TRAP_CODE_ZERO_QUAL_OFFSET}},
    {"fwd_str_res_bit_q_1", 0, FWD_STR_RES_BIT_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, FWD_STR_RES_BIT_QUAL_OFFSET}},
    {"fwd_str_q_pph", 0, FWD_STR_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, FWD_STR_QUAL_OFFSET}},
    {"oam_stamp_offset_q_pph", 0, OAM_STAMP_OFFSET_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 1, OAM_OFFSET_QUAL_OFFSET}},
    {"oam_offset_q_pph", 0, OAM_OFFSET_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 1, OAM_OFFSET_QUAL_OFFSET}},
    {"oam_sub_type_q_pph", 0, OAM_SUB_TYPE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 1, OAM_SUB_TYPE_QUAL_OFFSET}},
    {"oam_mep_q_pph", 0, OAM_MEP_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 1, OAM_MEP_QUAL_OFFSET}},
    {NULL, bcmFieldQualifyLayerRecordQualifier, 0,
        {bcmFieldInputTypeLayerRecordsAbsolute, 0, 0}},
    {"fhei_bridge_5B_innerVlan_q_pph", 0, FHEI_BRIDGE_5B_INNER_VLAN_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, FHEI_BRIDGE_5B_INNER_VLAN_QUAL_OFFSET}},
};


static field_itmh_pph_qual_info_t field_itmh_pph_ipmf2_fg2_qual_info_array[ITMH_PPH_NOF_IPMF2_FG2_QUALIFIERS] = {
  
    {"pars_offset_valid_q_pph", 0, PARSING_OFFSET_VALID_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 1, 0}},
    {"parsing_offset_q_pph", 0, PARSING_OFFSET_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 0, 0}},
    {"pars_type_valid_q_pph", 0, PARSING_TYPE_VALID_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 1, 0}},
    {"parsing_type_q_pph", 0, PARSING_TYPE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, PARSING_TYPE_QUAL_OFFSET}},
    {"adj_size_layer_idx_egr_prs_idx_q_pph", 0, SYS_HDR_ADJUST_SIZE_FWD_LAYER_INDEX_EGR_PRS_IDX_QUAL_LENGTH,
        {bcmFieldInputTypeMetaData, 0, 0}},
    {"ttl_set_q", 0, TTL_SET_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, TTL_SET_QUAL_OFFSET}},
    {"in_lif_profile_q_pph", 0, IN_LIF_PROFILE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, IN_LIF_PROFILE_QUAL_OFFSET}},
    {"fwd_additional_info_q_pph", 0, FWD_ADDITIONAL_INFO_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, FWD_ADDITIONAL_INFO_QUAL_OFFSET}},
    {"NWK_QoS_q_pph", 0, NWK_QOS_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, NWK_QOS_QUAL_OFFSET}},
    {"End_Of_Pkt_Edit_q_pph", 0, END_OF_PKT_EDIT_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, END_OF_PKT_EDIT_QUAL_OFFSET}},
    {"in_lif_0_q_pph", 0, IN_LIF_0_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, IN_LIF_0_QUAL_OFFSET}},
    {"fwd_domain_q_pph", 0, FWD_DOMAIN_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, FWD_DOMAIN_QUAL_OFFSET}}
};


static field_itmh_pph_qual_info_t field_itmh_pph_ipmf2_fg3_qual_info_array[ITMH_PPH_NOF_IPMF2_FG3_QUALIFIERS] = {
  
    {"1bitCmpResult_q_pph", 0, 1,
        {bcmFieldInputTypeMetaData, 0, 0}},
    {"fwd_trap_code_q_pph", 0, TRAP_CODE_FHEI_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, TRAP_CODE_FHEI_QUAL_OFFSET}},
    {"fwd_str_res_bit_q_2", 0, FWD_STR_RES_BIT_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, FWD_STR_RES_BIT_QUAL_OFFSET}},
    {"fwd_str_q", 0, FWD_STR_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, FWD_STR_QUAL_OFFSET}},
    {"fwd_qualifier_q_pph", 0, FWD_QUALIFIER_QUAL_LENGTH,
            {bcmFieldInputTypeLayerAbsolute, 3, FWD_QUALIFIER_QUAL_OFFSET}},
    {"fhei_bridge_5B_Vact_outV_q_pph", 0, FHEI_BRIDGE_5B_VLAN_ACT_OUT_VLAN_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, FHEI_BRIDGE_5B_VLAN_ACT_OUT_VLAN_QUAL_OFFSET}}
};


static field_itmh_pph_qual_info_t field_itmh_pph_cmp_key_qual_info_array[ITMH_PPH_NOF_CMP_KEY_QUALIFIERS] = {
  
    {"fhei_type_q_pph", 0, FHEI_TYPE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, FHEI_TYPE_QUAL_OFFSET}},
    {"fhei_size_q_pph", 0, FHEI_SIZE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 3, FHEI_SIZE_QUAL_OFFSET}},
    {"key_gen_var_q_pph", 0, 6,
        {bcmFieldInputTypeMetaData, 0, 0}}
};


static field_itmh_pph_qual_info_t field_itmh_pph_ipmf3_qual_info_array[ITMH_PPH_NOF_IPMF3_QUALIFIERS] = {
  
    {NULL, bcmFieldQualifyContainer, 0, {bcmFieldInputTypeMetaData, 0, PARS_OFFSET_TYPE_SH_ADJUST_FWD_LAYER_EGR_PRS_IDX_QUAL_OFFSET}},
    
    {"sys_hdr_const_q_pph", 0, 24, {bcmFieldInputTypeConst, 0xF00000, 0}}
};


static field_itmh_pph_action_info_t field_itmh_pph_ipmf2_action_info_array[ITMH_PPH_NOF_IPMF2_ACTIONS] = {

    {NULL, bcmFieldActionPrioIntNew, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"snoop_data_a_pph", 0, {0, bcmFieldActionSnoopRaw, SNOOP_DATA_ACTION_LENGTH,
        15, 0x0007F, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"destination_a_pph", 0, {0, bcmFieldActionForward, DST_ACTION_LENGTH,
        11, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionDropPrecedenceRaw, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE },
    {"mirror_command_a_pph", 0, {0, bcmFieldActionVoid, MIRROR_ACTION_LENGTH,
        31, 0x00000000, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionPphPresentSet, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"ext_exist_a_pph", 0, {0, bcmFieldActionVoid, EXT_EXIST_ACTION_LENGTH,
        31, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"out_lif_a_pph", 0, {0, bcmFieldActionVoid, OUT_LIF_ACTION_LENGTH,
        10, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"udh3_1bit_ase_a_pph", 0, {0, bcmFieldActionUDHData2, ASE_INJ_ACTION_LENGTH,
        31, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionIngressTimeStampInsert, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"fwd_str_a_pph", 0, {0, bcmFieldActionTrapRaw, FWD_STR_ACTION_LENGTH,
        19, 0x0, {0}}, FALSE, BCM_FIELD_ACTION_PRIORITY(0,10)},
    {NULL, bcmFieldActionOamRaw, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"lr_qual_a_pph", 0, {0, bcmFieldActionVoid, 16,
        16, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionInnerVlanPrioNew, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionInnerVlanNew, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE}
};


static field_itmh_pph_action_info_t field_itmh_pph_ipmf2_fg2_action_info_array[ITMH_PPH_NOF_IPMF2_FG2_ACTIONS] = {

    {"pars_start_sys_adjust_a_pph", 0, {0, bcmFieldActionContainer, PARS_START_SH_ADJUST_FWD_LAYER_EGR_PRS_IDX_ACTION_LENGTH,
        11, 0x0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionTtlSet, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionInVportClass0, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionForwardingAdditionalInfo, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionNetworkQos, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionIPTProfile, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionInVport0Raw, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionVrfSetRaw, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE}
};


static field_itmh_pph_action_info_t field_itmh_pph_ipmf2_fg3_action_info_array[ITMH_PPH_NOF_IPMF2_FG3_ACTIONS] = {

    {NULL, bcmFieldActionTrapRaw, {0}, TRUE, BCM_FIELD_ACTION_PRIORITY(0,20)},
    {NULL, bcmFieldActionVlanActionSetNew, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionOuterVlanPrioNew, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionOuterVlanNew, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE}

};


static field_itmh_pph_action_info_t field_itmh_pph_ipmf3_action_info_array[ITMH_PPH_NOF_IPMF3_ACTIONS] = {

    {"parsing_start_offset_a_pph", 0, {0, bcmFieldActionParsingStartOffsetRaw, PARSING_START_OFFSET_ACTION_LENGTH, 0, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionParsingStartTypeRaw, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"act_s_h_adjust_size_a_pph", 0, {0, bcmFieldActionSystemHeaderSizeAdjust, SYS_HDR_ADJUST_SIZE_ACTION_LENGTH, 6, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionForwardingLayerIndex, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionEgressForwardingIndex, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"zero_action_a_pph", 0, {0, bcmFieldActionVoid, IPMF3_ZERO_ACTION_LENGTH, 21, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionSystemHeaderSet, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE}
};


static field_itmh_pph_fem_action_info_t field_itmh_pph_ipmf2_fem_action_info_array[ITMH_PPH_NOF_IPMF2_FEMS] = {

    
    {8, {4, 5, 6, 7, 12, 13, 14, 15}, 1, 22, {1, 2}, {{32, 0}, 3, {{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}},
        
        {{bcmFieldActionOutVport0Raw, {
            
            {bcmFieldFemExtractionOutputSourceTypeKey, 3, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 4, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 5, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 6, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 7, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 8, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 9, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 10, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 11, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 12, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 13, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 14, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 15, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 16, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 17, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 18, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 19, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 20, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 21, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 22, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 23, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 24, 0}}, 0}}}},
    
    {8, {2, 3, 6, 7, 10, 11, 14, 15}, 1, 9, {1, 3}, {{0, 0}, 3, {{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}},
        {{bcmFieldActionMirrorIngressRaw, {
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1}},0}}}},
    
    {8, {1, 9, 3, 11, 5, 13, 7, 15}, 2, 4, {1, 4}, {{96, 0}, 31, {{0, 1}, {0, 1}, {1, 1}, {1, 1}, {0, 1}, {0, 1}, {1, 1}, {1, 1}},
        {{bcmFieldActionFabricHeaderSetRaw, {
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0}}, 0},
         {bcmFieldActionFabricHeaderSetRaw, {
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0}}, 0}}}}
};


static field_itmh_pph_fg_info_t itmh_pph_fg_info_array[ITMH_PPH_NOF_FG] = {
    
    {
        0,   
        bcmFieldStageIngressPMF2,   
        bcmFieldGroupTypeDirectExtraction,  
        ITMH_PPH_NOF_IPMF2_QUALIFIERS,  
        ITMH_PPH_NOF_IPMF2_ACTIONS, 
        field_itmh_pph_ipmf2_qual_info_array,   
        field_itmh_pph_ipmf2_action_info_array, 
        field_itmh_pph_ipmf2_fem_action_info_array  
    },
    
    {
        0,   
        bcmFieldStageIngressPMF2,   
        bcmFieldGroupTypeDirectExtraction,  
        ITMH_PPH_NOF_IPMF2_FG2_QUALIFIERS,  
        ITMH_PPH_NOF_IPMF2_FG2_ACTIONS, 
        field_itmh_pph_ipmf2_fg2_qual_info_array,   
        field_itmh_pph_ipmf2_fg2_action_info_array, 
        NULL  
    },
    
    {
        0,   
        bcmFieldStageIngressPMF2,   
        bcmFieldGroupTypeDirectExtraction,  
        ITMH_PPH_NOF_IPMF2_FG3_QUALIFIERS,  
        ITMH_PPH_NOF_IPMF2_FG3_ACTIONS, 
        field_itmh_pph_ipmf2_fg3_qual_info_array,   
        field_itmh_pph_ipmf2_fg3_action_info_array, 
        NULL  
    },
    
    {
        0,   
        bcmFieldStageIngressPMF3,   
        bcmFieldGroupTypeDirectExtraction,  
        ITMH_PPH_NOF_IPMF3_QUALIFIERS,  
        ITMH_PPH_NOF_IPMF3_ACTIONS, 
        field_itmh_pph_ipmf3_qual_info_array,   
        field_itmh_pph_ipmf3_action_info_array, 
        NULL  
    }
};








static shr_error_e
appl_dnx_field_itmh_pph_user_qual_config(
    int unit,
    field_itmh_pph_qual_info_t * itmh_pph_qual_info)
{
    bcm_field_qualifier_info_create_t qual_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    sal_strncpy_s((char *) (qual_info.name), itmh_pph_qual_info->name, sizeof(qual_info.name));
    qual_info.size = itmh_pph_qual_info->qual_length;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &(itmh_pph_qual_info->qual_id)));

exit:
    SHR_FUNC_EXIT;
}

 

static shr_error_e
appl_dnx_field_itmh_pph_cmp_key_config(
    int unit)
{
    int qual_index;
    bcm_field_context_t context_id;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_qualifier_info_get_t qual_info_get;

    SHR_FUNC_INIT_VARS(unit);

    for (qual_index = 0; qual_index < ITMH_PPH_NOF_CMP_KEY_QUALIFIERS; qual_index++)
    {
        if (field_itmh_pph_cmp_key_qual_info_array[qual_index].name != NULL)
        {
            SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_config
                            (unit, &(field_itmh_pph_cmp_key_qual_info_array[qual_index])));
        }
    }

    context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);

    
    bcm_field_context_compare_info_t_init(&compare_info);
    
    compare_info.first_key_info.nof_quals = 2;
    compare_info.first_key_info.qual_types[0] = field_itmh_pph_cmp_key_qual_info_array[0].qual_id;
    compare_info.first_key_info.qual_info[0].input_type =
        field_itmh_pph_cmp_key_qual_info_array[0].qual_attach_info.input_type;
    compare_info.first_key_info.qual_info[0].input_arg =
        field_itmh_pph_cmp_key_qual_info_array[0].qual_attach_info.input_arg;
    compare_info.first_key_info.qual_info[0].offset = field_itmh_pph_cmp_key_qual_info_array[0].qual_attach_info.offset;
    compare_info.first_key_info.qual_types[1] = field_itmh_pph_cmp_key_qual_info_array[1].qual_id;
    compare_info.first_key_info.qual_info[1].input_type =
        field_itmh_pph_cmp_key_qual_info_array[1].qual_attach_info.input_type;
    compare_info.first_key_info.qual_info[1].input_arg =
        field_itmh_pph_cmp_key_qual_info_array[1].qual_attach_info.input_arg;
    compare_info.first_key_info.qual_info[1].offset = field_itmh_pph_cmp_key_qual_info_array[1].qual_attach_info.offset;

    
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF1, &qual_info_get));

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = field_itmh_pph_cmp_key_qual_info_array[2].qual_id;
    compare_info.second_key_info.qual_info[0].input_type =
        field_itmh_pph_cmp_key_qual_info_array[2].qual_attach_info.input_type;
    compare_info.second_key_info.qual_info[0].offset =
        qual_info_get.offset + SYS_HDR_ADJUST_SIZE_FWD_LAYER_INDEX_EGR_PRS_IDX_QUAL_LENGTH;

   
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 0, &compare_info));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_context(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    if (stage == bcmFieldStageIngressPMF2)
    {
        stage = bcmFieldStageIngressPMF1;
        
        SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, DNX_FIELD_STAGE_IPMF1, context_id, &is_allocated));
        if (is_allocated)
        {
            goto exit;
        }
    }

    bcm_field_context_info_t_init(&context_info);
    if (stage == bcmFieldStageIngressPMF1)
    {
        
        context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
        sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_IPMF1", sizeof(context_info.name));
    }
    else
    {
        sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_IPMF3", sizeof(context_info.name));
    }

    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, BCM_FIELD_FLAG_WITH_ID, stage, &context_info, &context_id));

    bcm_field_context_param_info_t_init(&param_info);
    if (stage == bcmFieldStageIngressPMF1)
    {
        param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
        param_info.param_val = bcmFieldSystemHeaderProfileFtmhTshPph;
    }
    else
    {
        
        param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
        param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset0, 0);
    }
    if (stage == bcmFieldStageIngressPMF1)
    {
        SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, stage, context_id, &param_info));
        
        bcm_field_context_param_info_t_init(&param_info);
        param_info.param_type = bcmFieldContextParamTypeKeyVal;
        param_info.param_val = 0x12C9;
        param_info.param_arg = 0;
        SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, stage, context_id, &param_info));

        SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_cmp_key_config(unit));

        bcm_field_presel_entry_id_info_init(&presel_entry_id);
        
        presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_presel_id_ipmf1_get(unit);
        presel_entry_id.stage = stage;

        bcm_field_presel_entry_data_info_init(&presel_entry_data);
        presel_entry_data.nof_qualifiers = 2;
        presel_entry_data.context_id = context_id;
        presel_entry_data.entry_valid = TRUE;

        
        presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
        presel_entry_data.qual_data[0].qual_arg = 0;
        presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
        presel_entry_data.qual_data[0].qual_mask = 0x1F;
        
        presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
        presel_entry_data.qual_data[1].qual_arg = 0;
        presel_entry_data.qual_data[1].qual_value = 0x0001;
        presel_entry_data.qual_data[1].qual_mask = 0x0001;
        SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    }
    else
    {
        
        SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &param_info));

        bcm_field_presel_entry_id_info_init(&presel_entry_id);

        presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_presel_id_ipmf3_get(unit);
        presel_entry_id.stage = bcmFieldStageIngressPMF3;

        bcm_field_presel_entry_data_info_init(&presel_entry_data);
        presel_entry_data.entry_valid = TRUE;
        presel_entry_data.context_id = context_id;
        presel_entry_data.nof_qualifiers = 1;

        presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
        presel_entry_data.qual_data[0].qual_arg = 0;
        presel_entry_data.qual_data[0].qual_value = context_id;
        presel_entry_data.qual_data[0].qual_mask = 0x3F;
        SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_user_action_config(
    int unit,
    bcm_field_stage_t field_stage,
    field_itmh_pph_action_info_t * itmh_pph_action_info)
{
    bcm_field_action_info_t action_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_action_info_t_init(&action_info);

    action_info.action_type = itmh_pph_action_info->action_info.action_type;
    sal_strncpy_s((char *) action_info.name, itmh_pph_action_info->name, sizeof(action_info.name));
    
    if (!sal_strncmp(itmh_pph_action_info->name, "parsing_start_offset_a_pph", (BCM_FIELD_MAX_SHORT_NAME_LEN - 1)))
    {
        bcm_field_action_info_t pars_offset_action_info;
        int pars_offset_act_size;

        bcm_field_action_info_t_init(&pars_offset_action_info);
        SHR_IF_ERR_EXIT(bcm_field_action_info_get
                        (unit, itmh_pph_action_info->action_info.action_type, field_stage, &pars_offset_action_info));
        pars_offset_act_size = pars_offset_action_info.size;
        action_info.size = itmh_pph_action_info->action_info.size;
        action_info.prefix_size = (pars_offset_act_size - itmh_pph_action_info->action_info.size);
    }
    else
    {
        action_info.size = itmh_pph_action_info->action_info.size;
        action_info.prefix_size = itmh_pph_action_info->action_info.prefix_size;
    }
    action_info.stage = field_stage;
    action_info.prefix_value = itmh_pph_action_info->action_info.prefix_value;

    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &(itmh_pph_action_info->action_id)));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_fem_action_config(
    int unit,
    bcm_field_group_t fg_id,
    field_itmh_pph_fem_action_info_t * itmh_pph_fem_action_info)
{
    int mapping_bits_index, conditions_index, extractions_index;
    bcm_field_fem_action_info_t fem_action_info;
    int *condition_id = itmh_pph_fem_action_info->condition_ids;
    bcm_field_action_priority_t action_priority;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_fem_action_info_t_init(&fem_action_info);

    fem_action_info.fem_input.input_offset = itmh_pph_fem_action_info->fem_action_info.fem_input.input_offset;
    fem_action_info.condition_msb = itmh_pph_fem_action_info->fem_action_info.condition_msb;
    for (conditions_index = 0; conditions_index < itmh_pph_fem_action_info->nof_conditions; conditions_index++)
    {
        fem_action_info.fem_condition[condition_id[conditions_index]].extraction_id =
            itmh_pph_fem_action_info->fem_action_info.fem_condition[conditions_index].extraction_id;
        fem_action_info.fem_condition[condition_id[conditions_index]].is_action_valid =
            itmh_pph_fem_action_info->fem_action_info.fem_condition[conditions_index].is_action_valid;
    }
    for (extractions_index = 0; extractions_index < itmh_pph_fem_action_info->nof_extractions; extractions_index++)
    {
        fem_action_info.fem_extraction[extractions_index].action_type =
            itmh_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].action_type;
        fem_action_info.fem_extraction[extractions_index].increment =
            itmh_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].increment;
        for (mapping_bits_index = 0; mapping_bits_index < itmh_pph_fem_action_info->nof_mapping_bits;
             mapping_bits_index++)
        {
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].offset =
                itmh_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].offset;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].forced_value =
                itmh_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].forced_value;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].source_type =
                itmh_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].source_type;
        }
    }

    action_priority = BCM_FIELD_ACTION_POSITION(itmh_pph_fem_action_info->field_array_n_position.array_id,
                                                itmh_pph_fem_action_info->field_array_n_position.fem_position_in_array);
    SHR_IF_ERR_EXIT(bcm_field_fem_action_add(unit, 0, fg_id, action_priority, &fem_action_info));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_group_config(
    int unit,
    field_itmh_pph_fg_info_t * itmh_pph_fg_info)
{
    int qual_index, action_index, fem_index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_t context_id;
    bcm_field_qualifier_info_get_t qual_info_get;
    int op_offset;

    SHR_FUNC_INIT_VARS(unit);

    context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);

    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_context(unit, itmh_pph_fg_info->stage, context_id));

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    fg_info.fg_type = itmh_pph_fg_info->fg_type;
    fg_info.stage = itmh_pph_fg_info->stage;
    if (fg_info.stage == bcmFieldStageIngressPMF2)
    {
        sal_strncpy_s((char *) (fg_info.name), "ITMH_PPH_IPMF2", sizeof(fg_info.name));
    }
    else
    {
        sal_strncpy_s((char *) (fg_info.name), "ITMH_PPH_IPMF3", sizeof(fg_info.name));
    }
    fg_info.nof_quals = itmh_pph_fg_info->nof_quals;
    fg_info.nof_actions = itmh_pph_fg_info->nof_actions;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    
    for (qual_index = 0; qual_index < fg_info.nof_quals; qual_index++)
    {
        if (itmh_pph_fg_info->itmh_pph_qual_info[qual_index].name != NULL)
        {
            SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_config
                            (unit, &(itmh_pph_fg_info->itmh_pph_qual_info[qual_index])));
        }
        fg_info.qual_types[qual_index] = itmh_pph_fg_info->itmh_pph_qual_info[qual_index].qual_id;
        attach_info.key_info.qual_types[qual_index] = itmh_pph_fg_info->itmh_pph_qual_info[qual_index].qual_id;
        attach_info.key_info.qual_info[qual_index].input_type =
            itmh_pph_fg_info->itmh_pph_qual_info[qual_index].qual_attach_info.input_type;
        attach_info.key_info.qual_info[qual_index].input_arg =
            itmh_pph_fg_info->itmh_pph_qual_info[qual_index].qual_attach_info.input_arg;
        
        if ((itmh_pph_fg_info->itmh_pph_qual_info[qual_index].name != NULL) &&
            (!sal_strncmp
             (itmh_pph_fg_info->itmh_pph_qual_info[qual_index].name, "adj_size_layer_idx_egr_prs_idx_q_pph",
              (BCM_FIELD_MAX_SHORT_NAME_LEN - 1))))
        {
            bcm_field_qualifier_info_get_t keygenvar_qual_info_get;
            SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                            (unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF1, &keygenvar_qual_info_get));
            attach_info.key_info.qual_info[qual_index].offset = keygenvar_qual_info_get.offset;
        }
        else
        {
            attach_info.key_info.qual_info[qual_index].offset =
                itmh_pph_fg_info->itmh_pph_qual_info[qual_index].qual_attach_info.offset;
        }
    }

    
    for (action_index = 0; action_index < fg_info.nof_actions; action_index++)
    {
        if (itmh_pph_fg_info->itmh_pph_action_info[action_index].name != NULL)
        {
            SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_config
                            (unit, fg_info.stage, &(itmh_pph_fg_info->itmh_pph_action_info[action_index])));
        }
        fg_info.action_types[action_index] = itmh_pph_fg_info->itmh_pph_action_info[action_index].action_id;
        fg_info.action_with_valid_bit[action_index] =
            itmh_pph_fg_info->itmh_pph_action_info[action_index].action_with_valid_bit;
        attach_info.payload_info.action_types[action_index] =
            itmh_pph_fg_info->itmh_pph_action_info[action_index].action_id;
        attach_info.payload_info.action_info[action_index].priority =
            itmh_pph_fg_info->itmh_pph_action_info[action_index].action_priority;

    }

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &(itmh_pph_fg_info->fg_id)));

    
    if (fg_info.stage == bcmFieldStageIngressPMF2 && itmh_pph_fg_info->itmh_pph_fem_action_info != NULL)
    {
        for (fem_index = 0; fem_index < ITMH_PPH_NOF_IPMF2_FEMS; fem_index++)
        {
            SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_fem_action_config
                            (unit, itmh_pph_fg_info->fg_id, &(itmh_pph_fg_info->itmh_pph_fem_action_info[fem_index])));
        }
    }

    
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyCompareKeysResult0, bcmFieldStageIngressPMF2, &qual_info_get));

   
    SHR_IF_ERR_EXIT(bcm_field_compare_operand_offset_get(unit, 0, bcmFieldCompareOperandEqual, &op_offset));

    
    if ((itmh_pph_fg_info->itmh_pph_qual_info[0].qual_attach_info.input_type == bcmFieldInputTypeMetaData) &&
        (itmh_pph_fg_info->itmh_pph_qual_info[0].name != NULL))
    {
        attach_info.key_info.qual_info[0].offset =
            itmh_pph_fg_info->itmh_pph_qual_info[0].qual_attach_info.offset + qual_info_get.offset + op_offset;

    }

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, itmh_pph_fg_info->fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
appl_dnx_field_itmh_pph_init(
    int unit)
{
    int fg_index;

    SHR_FUNC_INIT_VARS(unit);

    
    for (fg_index = 0; fg_index < ITMH_PPH_NOF_FG; fg_index++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_config(unit, &itmh_pph_fg_info_array[fg_index]));
    }

exit:
    SHR_FUNC_EXIT;
}


