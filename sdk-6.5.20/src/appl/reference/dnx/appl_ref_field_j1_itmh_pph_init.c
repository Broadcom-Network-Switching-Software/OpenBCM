
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/algo/field/algo_field.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_seq.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "appl_ref_field_j1_itmh_pph_init.h"


#include <bcm/field.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_context.h>

#include <sal/appl/sal.h>
#include <sal/core/libc.h>
#include <soc/sand/sand_aux_access.h>










static field_j1_itmh_pph_qual_info_t  field_j1_itmh_pph_ipmf2_qual_info_array[J1_ITMH_PPH_NOF_IPMF2_QUALIFIERS] = {
  
    {"j1_itmh_pph_type_q", 0, J1_ITMH_PPH_PPH_TYPE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, J1_ITMH_PPH_PPH_TYPE_QUAL_OFFSET}},
    {"j1_itmh_pph_dst_q", 0, J1_ITMH_PPH_DP_DST_SNOOP_TC_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, J1_ITMH_PPH_DP_DST_SNOOP_TC_QUAL_OFFSET}},
    {"j1_itmh_pph_ext_ex_q", 0, J1_ITMH_PPH_EXT_PRESENT_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, J1_ITMH_PPH_EXT_PRESENT_QUAL_OFFSET}},
    
    {"j1_itmh_pph_outlif_q", 0, J1_ITMH_PPH_OUT_LIF_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, J1_ITMH_PPH_OUT_LIF_QUAL_OFFSET}},
    {"j1_itmh_pph_mirror_q", 0, J1_ITMH_PPH_MIRROR_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, J1_ITMH_PPH_MIRROR_QUAL_OFFSET}},
    {"j1_itmh_pph_mir_0", 0, 1,
        {bcmFieldInputTypeConst, 0x0, 0}},
    {"j1_itmh_pph_fwd_0", 0, J1_ITMH_PPH_TRAP_CODE_ZEROS_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 0x0, J1_ITMH_PPH_TRAP_CODE_ZEROS_QUAL_OFFSET}},
    {"j1_fwd_str_res_bit", 0, J1_ITMH_PPH_FWD_STR_RES_BIT_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 2, J1_ITMH_PPH_FWD_STR_RES_BIT_QUAL_OFFSET}},
    {"j1_fwd_str_q_pph", 0, J1_ITMH_PPH_FWD_STR_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 2, J1_ITMH_PPH_FWD_STR_QUAL_OFFSET}},
    {"j1_itmh_pph_oam_valdi_bit", 0, J1_ITMH_PPH_OAM_VALID_BIT_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, J1_ITMH_PPH_OAM_VALID_BIT_QUAL_OFFSET}},
    {"j1_itmh_pph_oam_st_q", 0, J1_ITMH_PPH_OAM_STAMP_OFFSET_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 0, J1_ITMH_PPH_OAM_STAMP_OFFSET_QUAL_OFFSET}},
    {"j1_itmh_pph_oam_of_q", 0, J1_ITMH_PPH_OAM_OFFSET_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 1, J1_ITMH_PPH_OAM_OFFSET_QUAL_OFFSET}},
    {"j1_itmh_pph_oam_ty_q", 0, J1_ITMH_PPH_OAM_SUB_TYPE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 1, J1_ITMH_PPH_OAM_SUB_TYPE_QUAL_OFFSET}},
    {"j1_itmh_pph_oam_mep_q", 0, J1_ITMH_PPH_OAM_MEP_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 1, J1_ITMH_PPH_OAM_MEP_QUAL_OFFSET}}
};


static field_j1_itmh_pph_qual_info_t field_j1_itmh_pph_ipmf2_fg2_qual_info_array[J1_ITMH_PPH_NOF_IPMF2_FG2_QUALIFIERS] = {
  
    {"j1_itmh_pph_pars_val", 0, J1_ITMH_PPH_PARSING_OFFSET_VALID_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 1, 0}},
    {"j1_itmh_pph_pars_off", 0, J1_ITMH_PPH_PARSING_OFFSET_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 2, J1_ITMH_PPH_PARSING_OFFSET_QUAL_OFFSET}},
    {"j1_itmh_pph_pars_t_v", 0, J1_ITMH_PPH_PARSING_TYPE_VALID_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 1, 0}},
    {"j1_itmh_pph_pars_ty", 0, J1_ITMH_PPH_PARSING_TYPE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 2, J1_ITMH_PPH_PARSING_TYPE_QUAL_OFFSET}},
    {"j1_itmh_pph_adj_size", 0, J1_ITMH_PPH_SYS_HDR_ADJUST_SIZE_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 0x1, 0}},
    {"j1_itmh_pph_fwd_add", 0, J1_ITMH_PPH_FWD_ADDITIONAL_INFO_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 2, J1_ITMH_PPH_FWD_ADDITIONAL_INFO_QUAL_OFFSET}},
    {"j1_itmh_pph_in_lif", 0, J1_ITMH_PPH_IN_LIF_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 2, J1_ITMH_PPH_IN_LIF_QUAL_OFFSET}},
    {"j1_itmh_pph_fwd_dom", 0, J1_ITMH_PPH_FWD_DOMAIN_QUAL_LENGTH,
            {bcmFieldInputTypeLayerAbsolute, 2, J1_ITMH_PPH_FWD_DOMAIN_QUAL_OFFSET}}

};



static field_j1_itmh_pph_qual_info_t field_j1_itmh_pph_ipmf2_fg3_qual_info_array[J1_ITMH_PPH_NOF_IPMF2_FG3_QUALIFIERS] = {
  
    {"j1_itmh_pph_1stCmp", 0, 1,
        {bcmFieldInputTypeMetaData, 0, 0}},
    {"j1_itmh_pph_2ndCmp", 0, 1,
        {bcmFieldInputTypeMetaData, 0, 0}},
    {"j1_itmh_pph_fhei_siz", 0, J1_ITMH_PPH_FHEI_SIZE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 2, J1_ITMH_PPH_FHEI_SIZE_QUAL_OFFSET}},
    {"j1_itmh_pph_fhei_ttl", 0, J1_ITMH_PPH_TTL_SET_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 2, J1_ITMH_PPH_TTL_SET_QUAL_OFFSET}},
    {"j1_itmh_pph_fhei_net", 0, J1_ITMH_PPH_NETWORK_QOS_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 2, J1_ITMH_PPH_NETWORK_QOS_QUAL_OFFSET}},
    {"j1_itmh_pph_inlif_p", 0, J1_ITMH_PPH_IN_LIF_PROFILE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 2, J1_ITMH_PPH_IN_LIF_PROFILE_QUAL_OFFSET}},
    {"j1_itmh_pph_fwd_l_idx", 0, J1_ITMH_PPH_FWD_LAYER_INDEX_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 0x3, 0}}
};


static field_j1_itmh_pph_qual_info_t field_j1_itmh_pph_ipmf3_qual_info_array[J1_ITMH_PPH_NOF_IPMF3_QUALIFIERS] = {
  
    {NULL, bcmFieldQualifyContainer, 0, {bcmFieldInputTypeMetaData, 0, J1_ITMH_PPH_PARS_OFFSET_TYPE_SH_ADJUST_QUAL_OFFSET}},
    {"j1_itmh_pph_udh_qual", bcmFieldQualifyUDHData0, 6, {bcmFieldInputTypeMetaData, 0, 0}}
};


static field_j1_itmh_pph_qual_info_t field_j1_itmh_pph_cmp_key_qual_info_array[J1_ITMH_PPH_NOF_CMP_KEY_QUALIFIERS] = {
  
    {"j1_itmh_pph_fh_type_q", 0, J1_ITMH_PPH_FHEI_TYPE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 2, J1_ITMH_PPH_FHEI_TYPE_QUAL_OFFSET}},
    {"j1_itmh_pph_min", 0, J1_ITMH_PPH_FHEI_TYPE_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 0x1, 0}},
    {"j1_itmh_pph_max", 0, J1_ITMH_PPH_FHEI_TYPE_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 0x4, 0}}
};


static field_j1_itmh_pph_action_info_t field_j1_itmh_pph_ipmf2_action_info_array[J1_ITMH_PPH_NOF_IPMF2_ACTIONS] = {

    {NULL, bcmFieldActionPphPresentSet, {0}, FALSE},
    {NULL, bcmFieldActionPrioIntNew, {0}, FALSE},
    {"j1_itmh_pph_act_snp", 0, {0, bcmFieldActionSnoop, J1_ITMH_PPH_SNOOP_DATA_ACTION_LENGTH,
        16, 0x003E, {0}}, FALSE},
    {"j1_itmh_pph_dest_void", 0, {0, bcmFieldActionVoid, J1_ITMH_PPH_DST_ACTION_LENGTH,
        13, 0, {0}}, FALSE},
    {NULL, bcmFieldActionDropPrecedence, {0}, FALSE},
    {"j1_itmh_pph_act_outlif", 0, {0, bcmFieldActionOutVport0, J1_ITMH_PPH_OUT_LIF_ACTION_LENGTH, 2, 0, {0}}, TRUE},
    {"j1_itmh_pph_act_mirror", 0, {0, bcmFieldActionMirrorIngress, J1_ITMH_PPH_MIRROR_ACTION_LENGTH,
        16, 0xF0, {0}}, TRUE},
    {"j1_itmh_pph_act_str", 0, {0, bcmFieldActionTrap, J1_ITMH_PPH_FWD_STR_ACTION_LENGTH,
            20, 0x0, {0}}, FALSE},
    {"j1_itmh_pph_oam", 0, {0, bcmFieldActionOam, 20,
            1, 0x0, {0}}, TRUE}
};


static field_j1_itmh_pph_action_info_t field_j1_itmh_pph_ipmf2_fg2_action_info_array[J1_ITMH_PPH_NOF_IPMF2_FG2_ACTIONS] = {

    {"j1_itmh_pph_a_pars", 0, {0, bcmFieldActionContainer, J1_ITMH_PPH_PARS_START_SH_ADJUST_ACTION_LENGTH,
        18, 0x0, {0}}, FALSE},
    {NULL, bcmFieldActionForwardingAdditionalInfo, {0}, FALSE},
    {"j1_itmh_pph_a_inlif", 0, {0, bcmFieldActionInVport0, 18, 4, 0, {0}}, FALSE},
    {"j1_itmh_pph_a_fwd_dom", 0, {0, bcmFieldActionVSwitchNew, 16, 2, 0, {0}}, FALSE},

};


static field_j1_itmh_pph_action_info_t field_j1_itmh_pph_ipmf2_fg3_action_info_array[J1_ITMH_PPH_NOF_IPMF2_FG3_ACTIONS] = {

    {"j1_itmh_pph_a_void", 0,  {0, bcmFieldActionVoid, 22, 10, 0, {0}}, FALSE},
    {NULL, bcmFieldActionForwardingLayerIndex, {0}, FALSE}
};



static field_j1_itmh_pph_action_info_t field_j1_itmh_pph_ipmf3_action_info_array[J1_ITMH_PPH_NOF_IPMF3_ACTIONS] = {

    {"j1_itmh_pph_parsing_start_offset_a", 0, {0, bcmFieldActionParsingStartOffset, J1_ITMH_PPH_PARSING_START_OFFSET_ACTION_LENGTH, 0, 0, {0}}, FALSE},
    {"j1_itmh_pph_void_a_pars", 0,  {0, bcmFieldActionVoid, 5, 27, 0, {0}}, FALSE},
    {"j1_itmh_pph_sys_h_a", 0, {0, bcmFieldActionSystemHeaderSizeAdjust, J1_ITMH_PPH_SYS_HDR_ADJUST_SIZE_ACTION_LENGTH, 6, 0, {0}}, FALSE},
    {"j1_itmh_pph_0_act", 0, {0, bcmFieldActionVoid, J1_ITMH_PPH_IPMF3_ZERO_ACTION_LENGTH, 14, 0, {0}}, FALSE},
    {NULL, bcmFieldActionParsingStartType, {0}, FALSE},
};





static field_j1_itmh_pph_fem_action_info_t field_j1_itmh_pph_ipmf2_fg3_fem_action_info_array[J1_ITMH_PPH_NOF_IPMF2_FEMS] = {

    
    {4, {4, 5, 6, 7}, 1, 8, {1, 2}, {{0, 0}, 3, {{0, 1}, {0, 1}, {0, 1}, {0, 1}},
        
        {{bcmFieldActionTtlSet, {
            
            {bcmFieldFemExtractionOutputSourceTypeKey, 4, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 5, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 6, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 7, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 8, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 9, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 10, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 11, 0}}, 0}}}},
    
    {4, {4, 5, 6, 7}, 1, 8, {1, 3}, {{0, 0}, 3, {{0, 1}, {0, 1}, {0, 1}, {0, 1}},
        {{bcmFieldActionNetworkQos, {
            
            {bcmFieldFemExtractionOutputSourceTypeKey, 12, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 13, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 14, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 15, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 16, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 17, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 18, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 19, 0}}, 0}}}},
    
    {4, {4, 5, 6, 7}, 1, 8, {1, 4}, {{0, 0}, 3, {{0, 1}, {0, 1}, {0, 1}, {0, 1}},
        {{bcmFieldActionInVportClass0, {
            {bcmFieldFemExtractionOutputSourceTypeKey, 20, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 21, 0}}, 0}}}}
};


static field_j1_itmh_pph_fem_action_info_t field_j1_itmh_pph_ipmf2_fg2_fem_action_info_array[1] = {

    
    {4, {0,1,3,5}, 4, 6, {1, 5}, {{0, 0}, 12, {{0, 1}, {1, 1}, {2, 1}, {3, 1}},
        
        {{bcmFieldActionUDHData0, {
            
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0}}, 0},
        {bcmFieldActionUDHData0, {
            
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0}}, 0},
        {bcmFieldActionUDHData0, {
            
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0}}, 0},
        {bcmFieldActionUDHData0, {
            
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0}}, 0}}}}
};

static field_j1_itmh_pph_fem_action_info_t field_j1_itmh_pph_ipmf2_fg1_fem_action_info_array[2] = {

    
    {16, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, 4, 21, {1, 6}, {{0, 0}, 28,
        {{3, 1}, {3, 1}, {2, 1}, {2, 1}, {1, 1}, {1, 1}, {0, 1}, {0, 1}, {3, 1}, {3, 1}, {2, 1}, {2, 1}, {1, 1}, {1, 1}, {0, 1}, {0, 1}},
        
        {{bcmFieldActionForward, {
            
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
            {bcmFieldFemExtractionOutputSourceTypeKey, 24, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},}, 0},
        {bcmFieldActionForward, {
            
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
            {bcmFieldFemExtractionOutputSourceTypeKey, 24, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1}}, 0},
        {bcmFieldActionForward, {
            
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
            {bcmFieldFemExtractionOutputSourceTypeKey, 24, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0}}, 0},
        {bcmFieldActionForward, {
            
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
            {bcmFieldFemExtractionOutputSourceTypeKey, 24, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0}}, 0}}}},
    
    {8, {2, 3, 6, 7, 10, 11, 14, 15}, 1, 4, {1, 7}, {{64, 0}, 3,
        {{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}},
        
        {{bcmFieldActionFabricHeaderSet, {
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0}}, 0}}}}
};


static field_j1_itmh_pph_fg_info_t j1_itmh_pph_fg_info_array[J1_ITMH_PPH_NOF_FG] = {

    
    {
        J1_ITMH_PPH_IPMF2_FG_ID,   
        bcmFieldStageIngressPMF2,   
        bcmFieldGroupTypeDirectExtraction,  
        J1_ITMH_PPH_NOF_IPMF2_QUALIFIERS,  
        J1_ITMH_PPH_NOF_IPMF2_ACTIONS, 
        field_j1_itmh_pph_ipmf2_qual_info_array,   
        field_j1_itmh_pph_ipmf2_action_info_array, 
        2,
        field_j1_itmh_pph_ipmf2_fg1_fem_action_info_array
    },

    
    {
        0,   
        bcmFieldStageIngressPMF2,   
        bcmFieldGroupTypeDirectExtraction,  
        J1_ITMH_PPH_NOF_IPMF2_FG2_QUALIFIERS,  
        J1_ITMH_PPH_NOF_IPMF2_FG2_ACTIONS, 
        field_j1_itmh_pph_ipmf2_fg2_qual_info_array,   
        field_j1_itmh_pph_ipmf2_fg2_action_info_array, 
        1,
        field_j1_itmh_pph_ipmf2_fg2_fem_action_info_array
    },

    
    {
        0,   
        bcmFieldStageIngressPMF2,   
        bcmFieldGroupTypeDirectExtraction,  
        J1_ITMH_PPH_NOF_IPMF2_FG3_QUALIFIERS,  
        J1_ITMH_PPH_NOF_IPMF2_FG3_ACTIONS, 
        field_j1_itmh_pph_ipmf2_fg3_qual_info_array,   
        field_j1_itmh_pph_ipmf2_fg3_action_info_array, 
        J1_ITMH_PPH_NOF_IPMF2_FEMS,
        field_j1_itmh_pph_ipmf2_fg3_fem_action_info_array
    },

    
    {
        0,   
        bcmFieldStageIngressPMF3,   
        bcmFieldGroupTypeDirectExtraction,  
        J1_ITMH_PPH_NOF_IPMF3_QUALIFIERS,  
        J1_ITMH_PPH_NOF_IPMF3_ACTIONS, 
        field_j1_itmh_pph_ipmf3_qual_info_array,   
        field_j1_itmh_pph_ipmf3_action_info_array, 
        0,
        NULL
    }

};











static shr_error_e
field_j1_itmh_pph_user_qual_config(
    int unit,
    field_j1_itmh_pph_qual_info_t * j1_itmh_pph_qual_info)
{
    bcm_field_qualifier_info_create_t qual_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    sal_strncpy_s((char *) (qual_info.name), j1_itmh_pph_qual_info->name, sizeof(qual_info.name));
    qual_info.size = j1_itmh_pph_qual_info->qual_length;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &(j1_itmh_pph_qual_info->qual_id)));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
field_j1_itmh_pph_user_action_config(
    int unit,
    bcm_field_stage_t field_stage,
    field_j1_itmh_pph_action_info_t * j1_itmh_pph_action_info)
{
    bcm_field_action_info_t action_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_action_info_t_init(&action_info);

    action_info.action_type = j1_itmh_pph_action_info->action_info.action_type;
    sal_strncpy_s((char *) (action_info.name), j1_itmh_pph_action_info->name, sizeof(action_info.name));
    
    if (!sal_strncmp
        (j1_itmh_pph_action_info->name, "j1_itmh_pph_parsing_start_offset_a", (BCM_FIELD_MAX_SHORT_NAME_LEN - 1)))
    {
        bcm_field_action_info_t pars_offset_action_info;
        int pars_offset_act_size;

        bcm_field_action_info_t_init(&pars_offset_action_info);
        SHR_IF_ERR_EXIT(bcm_field_action_info_get
                        (unit, j1_itmh_pph_action_info->action_info.action_type, field_stage,
                         &pars_offset_action_info));
        pars_offset_act_size = pars_offset_action_info.size;
        action_info.size = j1_itmh_pph_action_info->action_info.size;
        action_info.prefix_size = (pars_offset_act_size - j1_itmh_pph_action_info->action_info.size);
    }
    else
    {
        action_info.size = j1_itmh_pph_action_info->action_info.size;
        action_info.prefix_size = j1_itmh_pph_action_info->action_info.prefix_size;
    }
    action_info.stage = field_stage;
    action_info.prefix_value = j1_itmh_pph_action_info->action_info.prefix_value;

    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &(j1_itmh_pph_action_info->action_id)));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_j1_itmh_pph_cmp_key_config(
    int unit)
{
    int qual_index;
    bcm_field_context_t context_id;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_qualifier_info_get_t qual_info_get;

    SHR_FUNC_INIT_VARS(unit);

    
    for (qual_index = 0; qual_index < J1_ITMH_PPH_NOF_CMP_KEY_QUALIFIERS; qual_index++)
    {
        if (field_j1_itmh_pph_cmp_key_qual_info_array[qual_index].name != NULL)
        {
            SHR_IF_ERR_EXIT(field_j1_itmh_pph_user_qual_config
                            (unit, &(field_j1_itmh_pph_cmp_key_qual_info_array[qual_index])));
        }
    }

    context_id = dnx_data_field.context.default_j1_itmh_pph_context_get(unit);

    
    bcm_field_context_compare_info_t_init(&compare_info);
    
    compare_info.first_key_info.nof_quals = 1;
    compare_info.first_key_info.qual_types[0] = field_j1_itmh_pph_cmp_key_qual_info_array[0].qual_id;
    compare_info.first_key_info.qual_info[0].input_type =
        field_j1_itmh_pph_cmp_key_qual_info_array[0].qual_attach_info.input_type;
    compare_info.first_key_info.qual_info[0].input_arg =
        field_j1_itmh_pph_cmp_key_qual_info_array[0].qual_attach_info.input_arg;
    compare_info.first_key_info.qual_info[0].offset =
        field_j1_itmh_pph_cmp_key_qual_info_array[0].qual_attach_info.offset;

    
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF1, &qual_info_get));

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = field_j1_itmh_pph_cmp_key_qual_info_array[1].qual_id;
    compare_info.second_key_info.qual_info[0].input_type =
        field_j1_itmh_pph_cmp_key_qual_info_array[1].qual_attach_info.input_type;
    compare_info.second_key_info.qual_info[0].input_arg =
        field_j1_itmh_pph_cmp_key_qual_info_array[1].qual_attach_info.input_arg;
    compare_info.second_key_info.qual_info[0].offset =
        field_j1_itmh_pph_cmp_key_qual_info_array[1].qual_attach_info.offset;

   
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 0, &compare_info));

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = field_j1_itmh_pph_cmp_key_qual_info_array[2].qual_id;
    compare_info.second_key_info.qual_info[0].input_type =
        field_j1_itmh_pph_cmp_key_qual_info_array[2].qual_attach_info.input_type;
    compare_info.second_key_info.qual_info[0].input_arg =
        field_j1_itmh_pph_cmp_key_qual_info_array[2].qual_attach_info.input_arg;
    compare_info.second_key_info.qual_info[0].offset = field_j1_itmh_pph_cmp_key_qual_info_array[2].qual_attach_info.offset      ;

   
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 1, &compare_info));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
field_j1_itmh_pph_context(
    int unit,
    bcm_field_context_t context_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    int udh_type_len;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "J1_ITMH_PPH_IPMF1", sizeof(context_info.name));
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    context_info.context_compare_modes.compare_2_mode = bcmFieldContextCompareTypeDouble;
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &context_id));
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmhPph;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id, &param_info));

    SHR_IF_ERR_EXIT(appl_dnx_field_j1_itmh_pph_cmp_key_config(unit));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    
    presel_entry_id.presel_id = dnx_data_field.preselector.default_j1_itmh_pph_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;

    
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTmLegacy;
    presel_entry_data.qual_data[0].qual_mask = 0x1F;

    
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = 0x0002;
    presel_entry_data.qual_data[1].qual_mask = 0x0002;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    
    presel_entry_id.presel_id = dnx_data_field.preselector.default_j1_itmh_pph_oamp_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = dnx_data_field.context.default_j1_itmh_context_get(unit);;
    presel_entry_data.entry_valid = TRUE;

    
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTmLegacy;
    presel_entry_data.qual_data[0].qual_mask = 0x1F;

    
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyPtch;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = 7;
    presel_entry_data.qual_data[1].qual_mask = 0x7;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "J1_ITMH_PPH_IPMF3", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF3, &context_info, &context_id));
    bcm_field_context_param_info_t_init(&param_info);
    
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    udh_type_len =
        BITS2BYTES(dnx_data_field.udh.field_class_id_size_0_get(unit) +
                   dnx_data_field.udh.field_class_id_size_2_get(unit)) +
        BITS2BYTES(dnx_data_field.udh.field_class_id_size_1_get(unit) +
                   dnx_data_field.udh.field_class_id_size_3_get(unit));
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset0, -udh_type_len);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);

    presel_entry_id.presel_id = dnx_data_field.preselector.default_j1_itmh_pph_presel_id_ipmf3_get(unit);
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
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_fem_action_config(
    int unit,
    bcm_field_group_t fg_id,
    field_j1_itmh_pph_fem_action_info_t * itmh_j1_pph_fem_action_info)
{
    int mapping_bits_index, conditions_index, extractions_index;
    bcm_field_fem_action_info_t fem_action_info;
    int *condition_id = itmh_j1_pph_fem_action_info->condition_ids;
    bcm_field_action_priority_t action_priority;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_fem_action_info_t_init(&fem_action_info);

    fem_action_info.fem_input.input_offset = itmh_j1_pph_fem_action_info->fem_action_info.fem_input.input_offset;
    fem_action_info.condition_msb = itmh_j1_pph_fem_action_info->fem_action_info.condition_msb;
    for (conditions_index = 0; conditions_index < itmh_j1_pph_fem_action_info->nof_conditions; conditions_index++)
    {
        fem_action_info.fem_condition[condition_id[conditions_index]].extraction_id =
            itmh_j1_pph_fem_action_info->fem_action_info.fem_condition[conditions_index].extraction_id;
        fem_action_info.fem_condition[condition_id[conditions_index]].is_action_valid =
            itmh_j1_pph_fem_action_info->fem_action_info.fem_condition[conditions_index].is_action_valid;
    }
    for (extractions_index = 0; extractions_index < itmh_j1_pph_fem_action_info->nof_extractions; extractions_index++)
    {
        fem_action_info.fem_extraction[extractions_index].action_type =
            itmh_j1_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].action_type;
        fem_action_info.fem_extraction[extractions_index].increment =
            itmh_j1_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].increment;
        for (mapping_bits_index = 0; mapping_bits_index < itmh_j1_pph_fem_action_info->nof_mapping_bits;
             mapping_bits_index++)
        {
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].offset =
                itmh_j1_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].offset;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].forced_value =
                itmh_j1_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].forced_value;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].source_type =
                itmh_j1_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].source_type;
        }
    }

    action_priority = BCM_FIELD_ACTION_POSITION(itmh_j1_pph_fem_action_info->field_array_n_position.array_id,
                                                itmh_j1_pph_fem_action_info->
                                                field_array_n_position.fem_position_in_array);
    SHR_IF_ERR_EXIT(bcm_field_fem_action_add(unit, 0, fg_id, action_priority, &fem_action_info));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
field_j1_itmh_pph_group_config(
    int unit,
    field_j1_itmh_pph_fg_info_t * j1_itmh_pph_fg_info,
    bcm_field_context_t context_id)
{
    int qual_index, action_index, fem_index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_get_t qual_info_get;
    int bigger_or_eq_op_offset, smaller_or_eq_op_offset;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    fg_info.fg_type = j1_itmh_pph_fg_info->fg_type;
    fg_info.stage = j1_itmh_pph_fg_info->stage;
    sal_strncpy((char *) (fg_info.name), "J1_ITMH_PPH", sizeof(fg_info.name));
    fg_info.nof_quals = j1_itmh_pph_fg_info->nof_quals;
    fg_info.nof_actions = j1_itmh_pph_fg_info->nof_actions;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    
    for (qual_index = 0; qual_index < fg_info.nof_quals; qual_index++)
    {
        if (j1_itmh_pph_fg_info->itmh_qual_info[qual_index].name != NULL)
        {
            SHR_IF_ERR_EXIT(field_j1_itmh_pph_user_qual_config
                            (unit, &(j1_itmh_pph_fg_info->itmh_qual_info[qual_index])));
        }
        fg_info.qual_types[qual_index] = j1_itmh_pph_fg_info->itmh_qual_info[qual_index].qual_id;
        attach_info.key_info.qual_types[qual_index] = j1_itmh_pph_fg_info->itmh_qual_info[qual_index].qual_id;
        attach_info.key_info.qual_info[qual_index].input_type =
            j1_itmh_pph_fg_info->itmh_qual_info[qual_index].qual_attach_info.input_type;
        attach_info.key_info.qual_info[qual_index].input_arg =
            j1_itmh_pph_fg_info->itmh_qual_info[qual_index].qual_attach_info.input_arg;
        attach_info.key_info.qual_info[qual_index].offset =
            j1_itmh_pph_fg_info->itmh_qual_info[qual_index].qual_attach_info.offset;
    }

    
    for (action_index = 0; action_index < fg_info.nof_actions; action_index++)
    {
        if (j1_itmh_pph_fg_info->itmh_action_info[action_index].name != NULL)
        {
            SHR_IF_ERR_EXIT(field_j1_itmh_pph_user_action_config
                            (unit, fg_info.stage, &(j1_itmh_pph_fg_info->itmh_action_info[action_index])));
        }
        fg_info.action_types[action_index] = j1_itmh_pph_fg_info->itmh_action_info[action_index].action_id;
        fg_info.action_with_valid_bit[action_index] =
            j1_itmh_pph_fg_info->itmh_action_info[action_index].action_with_valid_bit;
        attach_info.payload_info.action_types[action_index] =
            j1_itmh_pph_fg_info->itmh_action_info[action_index].action_id;

    }

    SHR_IF_ERR_EXIT(bcm_field_group_add
                    (unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &(j1_itmh_pph_fg_info->fg_id)));

    
    if (fg_info.stage == bcmFieldStageIngressPMF2 && j1_itmh_pph_fg_info->itmh_pph_fem_action_info != NULL)
    {
        for (fem_index = 0; fem_index < j1_itmh_pph_fg_info->nof_fems; fem_index++)
        {
            SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_fem_action_config
                            (unit, j1_itmh_pph_fg_info->fg_id,
                             &(j1_itmh_pph_fg_info->itmh_pph_fem_action_info[fem_index])));
        }
    }

    
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyCompareKeysResult0, bcmFieldStageIngressPMF2, &qual_info_get));

   
    SHR_IF_ERR_EXIT(bcm_field_compare_operand_offset_get
                    (unit, 0, bcmFieldCompareOperandFirstKeyNotSmaller, &bigger_or_eq_op_offset));
    SHR_IF_ERR_EXIT(bcm_field_compare_operand_offset_get
                    (unit, 1, bcmFieldCompareOperandFirstKeyNotBigger, &smaller_or_eq_op_offset));

    
    if ((j1_itmh_pph_fg_info->itmh_qual_info[0].qual_attach_info.input_type == bcmFieldInputTypeMetaData) &&
        (j1_itmh_pph_fg_info->itmh_qual_info[0].name != NULL))
    {
        attach_info.key_info.qual_info[0].offset =
            j1_itmh_pph_fg_info->itmh_qual_info[0].qual_attach_info.offset + qual_info_get.offset +
            bigger_or_eq_op_offset;

        attach_info.key_info.qual_info[1].offset =
            j1_itmh_pph_fg_info->itmh_qual_info[1].qual_attach_info.offset + qual_info_get.offset +
            smaller_or_eq_op_offset;

    }
    if (fg_info.stage == bcmFieldStageIngressPMF3)
    {
        SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                        (unit, bcmFieldQualifyUDHData0, bcmFieldStageIngressPMF3, &qual_info_get));

        attach_info.key_info.qual_info[1].offset =
            j1_itmh_pph_fg_info->itmh_qual_info[1].qual_attach_info.offset + qual_info_get.offset;

    }
    
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, j1_itmh_pph_fg_info->fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
appl_dnx_field_j1_itmh_pph_cb(
    int unit,
    int *dynamic_flags)
{

    if ((dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)))
    {
        *dynamic_flags = 0;
    }
    else
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP;
    }

    return _SHR_E_NONE;

}


shr_error_e
appl_dnx_field_j1_itmh_pph_init(
    int unit)
{
    int fg_index;
    bcm_field_context_t context_id;
    SHR_FUNC_INIT_VARS(unit);

    context_id = dnx_data_field.context.default_j1_itmh_pph_context_get(unit);

    SHR_IF_ERR_EXIT(field_j1_itmh_pph_context(unit, context_id));

    
    for (fg_index = 0; fg_index < J1_ITMH_PPH_NOF_FG; fg_index++)
    {
        SHR_IF_ERR_EXIT(field_j1_itmh_pph_group_config(unit, &j1_itmh_pph_fg_info_array[fg_index], context_id));
    }

exit:
    SHR_FUNC_EXIT;
}


