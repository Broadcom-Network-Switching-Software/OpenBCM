
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

#include <soc/schanmsg.h>
#include <soc/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/sand/sand_aux_access.h>

#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/field/field_context.h>
#include "appl_ref_field_itmh_pph_init.h"

#include <bcm/field.h>

#include <sal/appl/sal.h>
#include <sal/core/libc.h>



/* *INDENT-OFF* */


#define ITMH_PPH_APPL_ITMH_LAYER    0
#define ITMH_PPH_APPL_ASE_TSH_LAYER 1
#define ITMH_PPH_APPL_PPH_LAYER     2

#define ITMH_IS_TSH_INJ     "is_tsh_inj"

#define HEADER_ITMH_SIZE    40

#define HEADER_PPH_SIZE     96
#define VALID_BIT_LENGTH    1

#define FLAG_BIT_ITMH_CONTEXT           0x1
#define FLAG_BIT_PPH_CONTEXT            0x2
#define FLAG_BIT_PPH_FHEI_CONTEXT       0x4
#define FLAG_BIT_PPH_FHEI_VLAN_CONTEXT  0x8
#define FLAG_BIT_ASE_1588_CONTEXT       0x10


#define ITMH_FIELDS_LENGTH      32
#define ITMH_FIELDS_OFFSET      7
#define ITMH_DST_LENGTH         21
#define MIRROR_CODE_LENGTH      9
#define SNOOP_DATA_LENGTH       5

#define ITMH_FWD_STR_LENGTH     3
#define ITMH_FWD_STR_OFFSET     0

#define ASE_INJ_LENGTH          1
#define ASE_INJ_OFFSET          4

#define IS_TSH_INJ_LENGTH       1
#define TSH_INJ_OFFSET          5

#define EXT_PRESENT_LENGTH      1
#define EXT_PRESENT_OFFSET      39

#define OUTLIF_LENGTH           22
#define OUTLIF_OFFSET           (HEADER_ITMH_SIZE + 2)


#define OAM_MEP_LENGTH          1
#define OAM_MEP_OFFSET          4

#define OAM_SUB_TYPE_LENGTH     4
#define OAM_SUB_TYPE_OFFSET     0

#define OAM_OFFSET_LENGTH       8
#define OAM_OFFSET_OFFSET       39


#define USE_INGRESS_TIME_COMPENSATION_LENGTH     1
#define USE_INGRESS_TIME_COMPENSATION_OFFSET     30

#define USE_INGRESS_TIME_STAMP_LENGTH   1
#define USE_INGRESS_TIME_STAMP_OFFSET   31

#define TS_ENCAPSULATION_LENGTH         1
#define TS_ENCAPSULATION_OFFSET         32

#define TS_COMMAND_LENGTH               2
#define TS_COMMAND_OFFSET               34

#define ASE_1588_OFFSET_LENGTH          8
#define ASE_1588_OFFSET_OFFSET          36


#define TIME_STAMP_LENGTH        32

#define TIME_STAMP_OFFSET        -32


#define PPH_FIELDS_LENGTH       71
#define PPH_FIELDS_OFFSET       6

#define PARSING_OFFSET_LENGTH   7
#define PARSING_OFFSET_OFFSET   83

#define PARSING_TYPE_LENGTH     5
#define PARSING_TYPE_OFFSET     90

#define FHEI_SIZE_LENGTH        2
#define FHEI_SIZE_OFFSET        78

#define LIF_EXT_TYPE_LENGTH     3
#define LIF_EXT_TYPE_OFFSET     80

#define PPH_FWD_STR_LENGTH      1
#define PPH_FWD_STR_OFFSET      95


#define TRAP_QUALIFIER_LENGTH   19
#define TRAP_QUALIFIER_OFFSET   (HEADER_PPH_SIZE + 8)

#define TRAP_CODE_LENGTH        9
#define TRAP_CODE_OFFSET        (HEADER_PPH_SIZE + 27)

#define FHEI_TRAP_TYPE_LENGTH   4
#define FHEI_TRAP_TYPE_OFFSET   (HEADER_PPH_SIZE + 36)


#define INNER_VLAN_LENGTH      16
#define INNER_VLAN_OFFSET      (HEADER_PPH_SIZE + 0)

#define OUTER_VLAN_LENGTH       23
#define OUTER_VLAN_OFFSET       (HEADER_PPH_SIZE + 16)

#define FHEI_VLAN_TYPE_LENGTH   1
#define FHEI_VLAN_TYPE_OFFSET   (HEADER_PPH_SIZE + 39)

#define SYS_HDR_PROFILE_LENGTH      4
#define FWD_LAYER_INDEX_LENGTH      3
#define TRAP_STR_LENGTH             4
#define IPMF3_ZERO_PADDING_LENGTH   18
#define EEI_LENGTH                  24
#define TRAP_CODE_AND_STR_LENGTH    (TRAP_CODE_LENGTH + TRAP_STR_LENGTH)

#define PRIORITY_ITMH               BCM_FIELD_ACTION_PRIORITY(0, 2)
#define PRIORITY_1588               BCM_FIELD_ACTION_PRIORITY(0, 9)

#define TRAP_PRIORITY_PPH           BCM_FIELD_ACTION_PRIORITY(0, 10)
#define TRAP_PRIORITY_FHEI_TRAP     BCM_FIELD_ACTION_PRIORITY(0, 20)







static shr_error_e
appl_dnx_field_itmh_pph_user_qual_create(
    int unit,
    char *name,
    int size,
    bcm_field_qualify_t * qual_id)
{
    bcm_field_qualifier_info_create_t qual_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    sal_strncpy_s((char *) (qual_info.name), name, sizeof(qual_info.name));
    qual_info.size = size;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, qual_id));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_user_action_create(
    int unit,
    char *name,
    bcm_field_action_t type,
    bcm_field_stage_t stage,
    int size,
    int prefix_value,
    bcm_field_action_t * action_id)
{
    bcm_field_action_info_t action_info, action_info_get;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = type;
    sal_strncpy_s((char *) action_info.name, name, sizeof(action_info.name));
    action_info.size = size;
    action_info.stage = stage;
    action_info.prefix_value = prefix_value;

    bcm_field_action_info_t_init(&action_info_get);
    SHR_IF_ERR_EXIT(bcm_field_action_info_get(unit, type, stage, &action_info_get));
    action_info.prefix_size = action_info_get.size - size;

    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, action_id));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_qualify_t tc_snoop_dst_dp_mirror_qual, mirror_code_qual, itmh_ext_exist_qual, out_lif_dst_qual;
    bcm_field_action_t snoop_data_action, destination_action, mirror_code_action;

    SHR_FUNC_INIT_VARS(unit);

    
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "tc_snoop_dst_dp_mirror", ITMH_FIELDS_LENGTH, &tc_snoop_dst_dp_mirror_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "mirror_code_default", MIRROR_CODE_LENGTH, &mirror_code_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "itmh_ext_exist", EXT_PRESENT_LENGTH, &itmh_ext_exist_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "out_lif_dst", OUTLIF_LENGTH, &out_lif_dst_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "snoop", bcmFieldActionSnoopRaw, bcmFieldStageIngressPMF2, SNOOP_DATA_LENGTH, 0x0007F,
                     &snoop_data_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "itmh_destination", bcmFieldActionForward, bcmFieldStageIngressPMF2, ITMH_DST_LENGTH, 0,
                     &destination_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "mirror_code_disable", bcmFieldActionMirrorIngressRaw, bcmFieldStageIngressPMF2,
                     MIRROR_CODE_LENGTH, 0, &mirror_code_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH", sizeof(fg_info.name));

    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = tc_snoop_dst_dp_mirror_qual;
    fg_info.qual_types[1] = mirror_code_qual;
    fg_info.qual_types[2] = itmh_ext_exist_qual;
    fg_info.qual_types[3] = out_lif_dst_qual;

    fg_info.nof_actions = 6;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = snoop_data_action;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = destination_action;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionDropPrecedenceRaw;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = mirror_code_action;
    fg_info.action_with_valid_bit[4] = TRUE;
    fg_info.action_types[5] = bcmFieldActionOutVport0Raw;
    fg_info.action_with_valid_bit[5] = TRUE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[0].offset = ITMH_FIELDS_OFFSET;

    
    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[1].input_arg = 0x1e0;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[2].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[2].offset = EXT_PRESENT_OFFSET;

    attach_info->key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info->key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[3].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[3].offset = OUTLIF_OFFSET;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];
    attach_info->payload_info.action_types[3] = fg_info.action_types[3];
    attach_info->payload_info.action_types[4] = fg_info.action_types[4];
    attach_info->payload_info.action_types[5] = fg_info.action_types[5];

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_only_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_qualify_t ase_inj_qual, fwd_layer_index_qual, fbr_header_qual;
    bcm_field_action_t trap_str_zero_padding_action, udh3_ase_action;
    bcm_field_group_info_t fg_info;
    int mapping_bits_index, conditions_id;
    bcm_field_fem_action_info_t fem_action_info;
    
    int forced_values[13] = { 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1 };
    bcm_field_action_priority_t action_priority;
    SHR_FUNC_INIT_VARS(unit);

    
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "is_ase_inj", ASE_INJ_LENGTH, &ase_inj_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "forwarding_layer_index", FWD_LAYER_INDEX_LENGTH, &fwd_layer_index_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "sys_header_profile", SYS_HDR_PROFILE_LENGTH, &fbr_header_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "trap_str_void", bcmFieldActionVoid, bcmFieldStageIngressPMF2, TRAP_STR_LENGTH, 0,
                     &trap_str_zero_padding_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "udh3_1bit_ase", bcmFieldActionUDHData3, bcmFieldStageIngressPMF2, ASE_INJ_LENGTH, 0,
                     &udh3_ase_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-only", sizeof(fg_info.name));

    fg_info.nof_quals = 5;
    
    fg_info.qual_types[0] = bcmFieldQualifyRxTrapStrength;
    
    fg_info.qual_types[1] = bcmFieldQualifyItmhPphType;
    fg_info.qual_types[2] = ase_inj_qual;
    fg_info.qual_types[3] = fwd_layer_index_qual;
    fg_info.qual_types[4] = fbr_header_qual;

    fg_info.nof_actions = 5;
    fg_info.action_types[0] = trap_str_zero_padding_action;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionPphPresentSet;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = udh3_ase_action;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionForwardingLayerIndex;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = bcmFieldActionFabricHeaderSetRaw;
    fg_info.action_with_valid_bit[4] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_fem_action_info_t_init(&fem_action_info);
    
    fem_action_info.fem_input.input_offset = 0;
    fem_action_info.condition_msb = 3;
    
    for (conditions_id = 0; conditions_id < 15; conditions_id++)
    {
        fem_action_info.fem_condition[conditions_id].extraction_id = 0;
        fem_action_info.fem_condition[conditions_id].is_action_valid = 1;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionTrapRaw;
    fem_action_info.fem_extraction[0].increment = 0;
    
    for (mapping_bits_index = 0; mapping_bits_index < 13; mapping_bits_index++)
    {
        fem_action_info.fem_extraction[0].output_bit[mapping_bits_index].offset = 0;
        fem_action_info.fem_extraction[0].output_bit[mapping_bits_index].forced_value =
            forced_values[mapping_bits_index];
        fem_action_info.fem_extraction[0].output_bit[mapping_bits_index].source_type =
            bcmFieldFemExtractionOutputSourceTypeForce;
    }

    action_priority = BCM_FIELD_ACTION_DONT_CARE;
    SHR_IF_ERR_EXIT(bcm_field_fem_action_add(unit, 0, *fg_id, action_priority, &fem_action_info));

    bcm_field_group_attach_info_t_init(attach_info);
    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_ITMH_LAYER;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[2].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[2].offset = ASE_INJ_OFFSET;

    attach_info->key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info->key_info.qual_info[3].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[3].input_arg = 0x0;

    attach_info->key_info.qual_types[4] = fg_info.qual_types[4];
    attach_info->key_info.qual_info[4].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[4].input_arg = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];
    attach_info->payload_info.action_types[3] = fg_info.action_types[3];
    attach_info->payload_info.action_types[4] = fg_info.action_types[4];
    
    attach_info->payload_info.action_info[3].priority = PRIORITY_ITMH;
    attach_info->payload_info.action_info[4].priority = PRIORITY_ITMH;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_pph_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_qualify_t pars_offset_valid_qual, pars_offset_qual, pars_type_valid_qual, pars_type_qual;
    bcm_field_qualify_t ttl_qos_inlif_fwddmn_inlifprfl_tail_fai_qual, fwd_trap_code_qual, itmh_fwd_str_qual,
        pph_fwd_str_qual, is_tsh_inj_qual, hdr_profile_qual;
    bcm_field_action_t container_action, trap_action;
    int trap_code_none;

    SHR_FUNC_INIT_VARS(unit);

    
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "pars_offset_valid", VALID_BIT_LENGTH, &pars_offset_valid_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "pars_offset", PARSING_OFFSET_LENGTH, &pars_offset_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "pars_type_valid", VALID_BIT_LENGTH, &pars_type_valid_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "pars_type", PARSING_TYPE_LENGTH, &pars_type_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "ttl_qos_inlif_fwddmn_inlifprfl_tail_fai", PPH_FIELDS_LENGTH,
                     &ttl_qos_inlif_fwddmn_inlifprfl_tail_fai_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "fwd_trap_code_none", TRAP_CODE_LENGTH, &fwd_trap_code_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "itmh_fwd_str", ITMH_FWD_STR_LENGTH, &itmh_fwd_str_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "pph_fwd_str", PPH_FWD_STR_LENGTH, &pph_fwd_str_qual));
    
    SHR_IF_ERR_EXIT(dnx_field_map_name_to_qual(unit, ITMH_IS_TSH_INJ, &is_tsh_inj_qual));
    SHR_IF_ERR_EXIT(dnx_field_map_name_to_qual(unit, "sys_header_profile", &hdr_profile_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "parsing_container", bcmFieldActionContainer, bcmFieldStageIngressPMF2,
                     VALID_BIT_LENGTH + PARSING_OFFSET_LENGTH + VALID_BIT_LENGTH + PARSING_TYPE_LENGTH, 0,
                     &container_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "trap_code_and_str", bcmFieldActionTrapRaw, bcmFieldStageIngressPMF2,
                     TRAP_CODE_AND_STR_LENGTH, 0, &trap_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-PPH", sizeof(fg_info.name));

    fg_info.nof_quals = 10;
    fg_info.qual_types[0] = pars_offset_valid_qual;
    fg_info.qual_types[1] = pars_offset_qual;
    fg_info.qual_types[2] = pars_type_valid_qual;
    fg_info.qual_types[3] = pars_type_qual;
    fg_info.qual_types[4] = ttl_qos_inlif_fwddmn_inlifprfl_tail_fai_qual;
    fg_info.qual_types[5] = fwd_trap_code_qual;
    fg_info.qual_types[6] = itmh_fwd_str_qual;
    fg_info.qual_types[7] = pph_fwd_str_qual;
    fg_info.qual_types[8] = is_tsh_inj_qual;
    fg_info.qual_types[9] = hdr_profile_qual;

    fg_info.nof_actions = 10;
    fg_info.action_types[0] = container_action;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionTtlSet;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionNetworkQos;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionInVport0Raw;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = bcmFieldActionVrfSetRaw;
    fg_info.action_with_valid_bit[4] = FALSE;
    fg_info.action_types[5] = bcmFieldActionInVportClass0;
    fg_info.action_with_valid_bit[5] = FALSE;
    fg_info.action_types[6] = bcmFieldActionIPTProfile;
    fg_info.action_with_valid_bit[6] = FALSE;
    fg_info.action_types[7] = bcmFieldActionForwardingAdditionalInfo;
    fg_info.action_with_valid_bit[7] = FALSE;
    fg_info.action_types[8] = trap_action;
    fg_info.action_with_valid_bit[8] = FALSE;
    fg_info.action_types[9] = bcmFieldActionFabricHeaderSetRaw;
    fg_info.action_with_valid_bit[9] = TRUE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[0].input_arg = 0x1;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[1].offset = PARSING_OFFSET_OFFSET;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[2].input_arg = 0x1;

    attach_info->key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info->key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[3].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[3].offset = PARSING_TYPE_OFFSET;

    attach_info->key_info.qual_types[4] = fg_info.qual_types[4];
    attach_info->key_info.qual_info[4].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[4].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[4].offset = PPH_FIELDS_OFFSET;

    
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, bcmRxTrapDefault, &trap_code_none));
    attach_info->key_info.qual_types[5] = fg_info.qual_types[5];
    attach_info->key_info.qual_info[5].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[5].input_arg = trap_code_none;

    attach_info->key_info.qual_types[6] = fg_info.qual_types[6];
    attach_info->key_info.qual_info[6].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[6].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[6].offset = ITMH_FWD_STR_OFFSET;

    attach_info->key_info.qual_types[7] = fg_info.qual_types[7];
    attach_info->key_info.qual_info[7].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[7].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[7].offset = PPH_FWD_STR_OFFSET;

    attach_info->key_info.qual_types[8] = fg_info.qual_types[8];
    attach_info->key_info.qual_info[8].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[8].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[8].offset = TSH_INJ_OFFSET;

    attach_info->key_info.qual_types[9] = fg_info.qual_types[9];
    attach_info->key_info.qual_info[9].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[9].input_arg = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];
    attach_info->payload_info.action_types[3] = fg_info.action_types[3];
    attach_info->payload_info.action_types[4] = fg_info.action_types[4];
    attach_info->payload_info.action_types[5] = fg_info.action_types[5];
    attach_info->payload_info.action_types[6] = fg_info.action_types[6];
    attach_info->payload_info.action_types[7] = fg_info.action_types[7];
    attach_info->payload_info.action_types[8] = fg_info.action_types[8];
    attach_info->payload_info.action_info[8].priority = TRAP_PRIORITY_PPH;
    attach_info->payload_info.action_types[9] = fg_info.action_types[9];

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_ase_oam_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_qualify_t oam_stamp_offset_qual, oam_offset_qual, oam_sub_type_qual, oam_mep_qual;

    SHR_FUNC_INIT_VARS(unit);

    
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "oam_header_stamp_offset", OAM_OFFSET_LENGTH, &oam_stamp_offset_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "oam_header_offset", OAM_OFFSET_LENGTH, &oam_offset_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "oam_header_sub_type", OAM_SUB_TYPE_LENGTH, &oam_sub_type_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "oam_header_mep", OAM_MEP_LENGTH, &oam_mep_qual));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-ASE_OAM", sizeof(fg_info.name));

    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = oam_stamp_offset_qual;
    fg_info.qual_types[1] = oam_offset_qual;
    fg_info.qual_types[2] = oam_sub_type_qual;
    fg_info.qual_types[3] = oam_mep_qual;

    fg_info.nof_actions = 1;
    
    fg_info.action_types[0] = bcmFieldActionOamRaw;
    fg_info.action_with_valid_bit[0] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[0].offset = OAM_OFFSET_OFFSET;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[1].offset = OAM_OFFSET_OFFSET;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[2].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[2].offset = OAM_SUB_TYPE_OFFSET;

    attach_info->key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info->key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[3].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[3].offset = OAM_MEP_OFFSET;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_ase_1588_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_qualify_t ase_1588_offset_qual, time_stamp_encapsulation_qual, time_stamp_command_qual,
        use_ingress_time_compensation_qual;
    bcm_field_qualify_t ase_1588_type_qual, fwd_layer_index_qual, is_tsh_inj_qual, hdr_profile_qual;
    SHR_FUNC_INIT_VARS(unit);

    
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "1588_offset", ASE_1588_OFFSET_LENGTH, &ase_1588_offset_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "time_stamp_encapsulation", TS_ENCAPSULATION_LENGTH, &time_stamp_encapsulation_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "time_stamp_command", TS_COMMAND_LENGTH, &time_stamp_command_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "use_ingress_time_compensation", USE_INGRESS_TIME_COMPENSATION_LENGTH,
                     &use_ingress_time_compensation_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "1588_ase_type", 1, &ase_1588_type_qual));
    
    SHR_IF_ERR_EXIT(dnx_field_map_name_to_qual(unit, "forwarding_layer_index", &fwd_layer_index_qual));
    
    SHR_IF_ERR_EXIT(dnx_field_map_name_to_qual(unit, ITMH_IS_TSH_INJ, &is_tsh_inj_qual));
    SHR_IF_ERR_EXIT(dnx_field_map_name_to_qual(unit, "sys_header_profile", &hdr_profile_qual));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-ASE_1588", sizeof(fg_info.name));

    fg_info.nof_quals = 8;
    fg_info.qual_types[0] = ase_1588_offset_qual;
    fg_info.qual_types[1] = time_stamp_encapsulation_qual;
    fg_info.qual_types[2] = time_stamp_command_qual;
    fg_info.qual_types[3] = use_ingress_time_compensation_qual;
    fg_info.qual_types[4] = ase_1588_type_qual;
    fg_info.qual_types[5] = fwd_layer_index_qual;
    fg_info.qual_types[6] = is_tsh_inj_qual;
    fg_info.qual_types[7] = hdr_profile_qual;

    fg_info.nof_actions = 3;
    fg_info.action_types[0] = bcmFieldActionIEEE1588;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionForwardingLayerIndex;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionFabricHeaderSetRaw;
    fg_info.action_with_valid_bit[2] = TRUE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[0].offset = ASE_1588_OFFSET_OFFSET;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[1].offset = TS_ENCAPSULATION_OFFSET;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[2].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[2].offset = TS_COMMAND_OFFSET;

    attach_info->key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info->key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[3].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[3].offset = USE_INGRESS_TIME_COMPENSATION_OFFSET;

    attach_info->key_info.qual_types[4] = fg_info.qual_types[4];
    attach_info->key_info.qual_info[4].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[4].input_arg = 0x1;

    attach_info->key_info.qual_types[5] = fg_info.qual_types[5];
    attach_info->key_info.qual_info[5].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[5].input_arg = 0x1;

    attach_info->key_info.qual_types[6] = fg_info.qual_types[6];
    attach_info->key_info.qual_info[6].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[6].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[6].offset = TSH_INJ_OFFSET;

    attach_info->key_info.qual_types[7] = fg_info.qual_types[7];
    attach_info->key_info.qual_info[7].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[7].input_arg = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];
    attach_info->payload_info.action_info[1].priority = PRIORITY_1588;
    attach_info->payload_info.action_info[2].priority = PRIORITY_1588;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_tsh_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_qualify_t time_stamp_qual, time_stamp_valid_qual;
    bcm_field_group_info_t fg_info;

    SHR_FUNC_INIT_VARS(unit);

    
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, ITMH_IS_TSH_INJ, IS_TSH_INJ_LENGTH, &time_stamp_valid_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "time_stamp_header", TIME_STAMP_LENGTH, &time_stamp_qual));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-TSH", sizeof(fg_info.name));

    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = time_stamp_valid_qual;
    fg_info.qual_types[1] = time_stamp_qual;

    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionIngressTimeStampInsertValid;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionIngressTimeStampInsert;
    fg_info.action_with_valid_bit[1] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);
    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[0].offset = TSH_INJ_OFFSET;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[1].offset = TIME_STAMP_OFFSET;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_flow_id_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_qualify_t is_tsh_inj_qual;
    bcm_field_group_info_t fg_info;
    bcm_field_action_t flow_id_valid_action;

    SHR_FUNC_INIT_VARS(unit);

    
    
    SHR_IF_ERR_EXIT(dnx_field_map_name_to_qual(unit, ITMH_IS_TSH_INJ, &is_tsh_inj_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "flow_id_valid", bcmFieldActionLatencyFlowId, bcmFieldStageIngressPMF2, IS_TSH_INJ_LENGTH, 0,
                     &flow_id_valid_action));

    bcm_field_group_info_t_init(&fg_info);

    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-FLOW_ID", sizeof(fg_info.name));

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = is_tsh_inj_qual;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = flow_id_valid_action;
    fg_info.action_with_valid_bit[0] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[0].offset = TSH_INJ_OFFSET;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_fhei_trap_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_qualify_t fwd_trap_code_qual, itmh_fwd_str_qual, pph_fwd_str_qual, trap_qualifier_qual;

    SHR_FUNC_INIT_VARS(unit);

    
    
    SHR_IF_ERR_EXIT(dnx_field_map_name_to_qual(unit, "fwd_trap_code_none", &fwd_trap_code_qual));
    
    SHR_IF_ERR_EXIT(dnx_field_map_name_to_qual(unit, "itmh_fwd_str", &itmh_fwd_str_qual));
    
    SHR_IF_ERR_EXIT(dnx_field_map_name_to_qual(unit, "pph_fwd_str", &pph_fwd_str_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "cpu_trap_qualifier", TRAP_QUALIFIER_LENGTH, &trap_qualifier_qual));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-FHEI_Trap", sizeof(fg_info.name));

    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = fwd_trap_code_qual;
    fg_info.qual_types[1] = itmh_fwd_str_qual;
    fg_info.qual_types[2] = pph_fwd_str_qual;
    fg_info.qual_types[3] = trap_qualifier_qual;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionTrapRaw;
    fg_info.action_with_valid_bit[0] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[0].offset = TRAP_CODE_OFFSET;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[1].offset = ITMH_FWD_STR_OFFSET;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[2].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[2].offset = PPH_FWD_STR_OFFSET;

    attach_info->key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info->key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[3].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[3].offset = TRAP_QUALIFIER_OFFSET;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_info[0].priority = TRAP_PRIORITY_FHEI_TRAP;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_fhei_vlan_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_qualify_t inner_vlan_qual, outer_vlan_qual, eei_qual;
    bcm_field_group_info_t fg_info;

    SHR_FUNC_INIT_VARS(unit);

    
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "inner_vlan", INNER_VLAN_LENGTH, &inner_vlan_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "outer_vlan", OUTER_VLAN_LENGTH, &outer_vlan_qual));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "modify_eei", EEI_LENGTH, &eei_qual));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-FHEI_Vlan", sizeof(fg_info.name));

    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = inner_vlan_qual;
    fg_info.qual_types[1] = outer_vlan_qual;
    fg_info.qual_types[2] = eei_qual;

    fg_info.nof_actions = 6;
    fg_info.action_types[0] = bcmFieldActionInnerVlanNew;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionInnerVlanPrioNew;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionOuterVlanNew;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionOuterVlanPrioNew;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = bcmFieldActionVlanActionSetNew;
    fg_info.action_with_valid_bit[4] = FALSE;
    fg_info.action_types[5] = bcmFieldActionSystemHeaderSet;
    fg_info.action_with_valid_bit[5] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[0].offset = INNER_VLAN_OFFSET;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[1].offset = OUTER_VLAN_OFFSET;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[2].input_arg = 0xF00000;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];
    attach_info->payload_info.action_types[3] = fg_info.action_types[3];
    attach_info->payload_info.action_types[4] = fg_info.action_types[4];
    attach_info->payload_info.action_types[5] = fg_info.action_types[5];

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_ipmf3_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_action_t pars_offset_action, zero_padding_action;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "parse_start_offset", bcmFieldActionParsingStartOffsetRaw, bcmFieldStageIngressPMF3,
                     PARSING_OFFSET_LENGTH + VALID_BIT_LENGTH, 0, &pars_offset_action));
    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "void_zero_padding", bcmFieldActionVoid, bcmFieldStageIngressPMF3, IPMF3_ZERO_PADDING_LENGTH,
                     0, &zero_padding_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    sal_strncpy_s((char *) fg_info.name, "ITMH-PPH-IPMF3", sizeof(fg_info.name));

    fg_info.nof_quals = 1;
    
    fg_info.qual_types[0] = bcmFieldQualifyContainer;

    fg_info.nof_actions = 3;
    fg_info.action_types[0] = pars_offset_action;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionParsingStartTypeRaw;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = zero_padding_action;
    fg_info.action_with_valid_bit[2] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_ipmf3_const_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_action_t sys_hdr_size_adjust_action, fwd_layer_index_action, egr_fwd_layer_action;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "sys_hdr_size_adjust", bcmFieldActionSystemHeaderSizeAdjust, bcmFieldStageIngressPMF3, 0,
                     0x1, &sys_hdr_size_adjust_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "forwarding_layer_indexx", bcmFieldActionForwardingLayerIndex, bcmFieldStageIngressPMF3, 0,
                     0x3, &fwd_layer_index_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "egr_fwd_layer", bcmFieldActionEgressForwardingIndex, bcmFieldStageIngressPMF3, 0, 0x3,
                     &egr_fwd_layer_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = bcmFieldStageIngressPMF3;
    sal_strncpy_s((char *) fg_info.name, "ITMH-PPH_Const-IPMF3", sizeof(fg_info.name));

    fg_info.nof_actions = 3;
    fg_info.action_types[0] = sys_hdr_size_adjust_action;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = fwd_layer_index_action;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = egr_fwd_layer_action;
    fg_info.action_with_valid_bit[2] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_cmp_key_config(
    int unit)
{
    bcm_field_qualify_t lif_extension_qual, fhei_trap_type_qual, fhei_vlan_type_qual;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_qualifier_info_get_t qual_info_get;
    bcm_field_context_t context_id;
    int pair_id;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF1, &qual_info_get));

    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "lif_extension", LIF_EXT_TYPE_LENGTH, &lif_extension_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "fhei_trap_type", FHEI_TRAP_TYPE_LENGTH, &fhei_trap_type_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "fhei_vlan_type", FHEI_VLAN_TYPE_LENGTH, &fhei_vlan_type_qual));

    
    bcm_field_context_compare_info_t_init(&compare_info);
    
    compare_info.first_key_info.nof_quals = 1;
    compare_info.first_key_info.qual_types[0] = lif_extension_qual;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    compare_info.first_key_info.qual_info[0].offset = LIF_EXT_TYPE_OFFSET;

    compare_info.second_key_info.nof_quals = 1;
    
    compare_info.second_key_info.qual_types[0] = lif_extension_qual;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    compare_info.second_key_info.qual_info[0].offset = qual_info_get.offset;

    
    pair_id = 0;
    context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create
                    (unit, 0, bcmFieldStageIngressPMF1, context_id, pair_id, &compare_info));

    
    bcm_field_context_compare_info_t_init(&compare_info);
    
    compare_info.first_key_info.nof_quals = 2;
    compare_info.first_key_info.qual_types[0] = fhei_trap_type_qual;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    compare_info.first_key_info.qual_info[0].offset = FHEI_TRAP_TYPE_OFFSET;

    compare_info.first_key_info.qual_types[1] = lif_extension_qual;
    compare_info.first_key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[1].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    compare_info.first_key_info.qual_info[1].offset = LIF_EXT_TYPE_OFFSET;

    
    compare_info.second_key_info.nof_quals = 2;
    compare_info.second_key_info.qual_types[0] = fhei_trap_type_qual;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    compare_info.second_key_info.qual_info[0].offset = qual_info_get.offset;

    compare_info.second_key_info.qual_types[1] = lif_extension_qual;
    compare_info.second_key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    compare_info.second_key_info.qual_info[1].offset =
        qual_info_get.offset + FHEI_TRAP_TYPE_LENGTH + FHEI_VLAN_TYPE_LENGTH;

    
    pair_id = 0;
    context_id = dnx_data_field.context.default_itmh_pph_fhei_context_get(unit);
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create
                    (unit, 0, bcmFieldStageIngressPMF1, context_id, pair_id, &compare_info));

    bcm_field_context_compare_info_t_init(&compare_info);

    compare_info.first_key_info.nof_quals = 2;
    compare_info.first_key_info.qual_types[0] = fhei_vlan_type_qual;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    compare_info.first_key_info.qual_info[0].offset = FHEI_VLAN_TYPE_OFFSET;

    compare_info.first_key_info.qual_types[1] = lif_extension_qual;
    compare_info.first_key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[1].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    compare_info.first_key_info.qual_info[1].offset = LIF_EXT_TYPE_OFFSET;

    
    compare_info.second_key_info.nof_quals = 2;
    compare_info.second_key_info.qual_types[0] = fhei_vlan_type_qual;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    compare_info.second_key_info.qual_info[0].offset = qual_info_get.offset + FHEI_TRAP_TYPE_LENGTH;

    compare_info.second_key_info.qual_types[1] = lif_extension_qual;
    compare_info.second_key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    compare_info.second_key_info.qual_info[1].offset =
        qual_info_get.offset + FHEI_TRAP_TYPE_LENGTH + FHEI_VLAN_TYPE_LENGTH;

    
    pair_id = 1;
    context_id = dnx_data_field.context.default_itmh_pph_fhei_context_get(unit);
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create
                    (unit, 0, bcmFieldStageIngressPMF1, context_id, pair_id, &compare_info));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_ipmf1_contexts(
    int unit)
{
    bcm_field_context_t itmh_pph_context_id, itmh_pph_fhei_context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;

    SHR_FUNC_INIT_VARS(unit);

    

    itmh_pph_context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);
    bcm_field_context_info_t_init(&context_info);
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_IPMF1", sizeof(context_info.name));

    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &itmh_pph_context_id));

    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmhPph;

    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, itmh_pph_context_id, &param_info));
    
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x0;
    param_info.param_arg = 0;

    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, itmh_pph_context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 3;
    presel_entry_data.context_id = itmh_pph_context_id;
    presel_entry_data.entry_valid = TRUE;

    
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    presel_entry_data.qual_data[0].qual_mask = -1;
    
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = 0x0001;
    presel_entry_data.qual_data[1].qual_mask = 0x0001;
    
    presel_entry_data.qual_data[2].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[2].qual_arg = 2;
    presel_entry_data.qual_data[2].qual_value = 0x00;
    presel_entry_data.qual_data[2].qual_mask = 0x30;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    
    itmh_pph_fhei_context_id = dnx_data_field.context.default_itmh_pph_fhei_context_get(unit);
    bcm_field_context_info_t_init(&context_info);
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    context_info.context_compare_modes.compare_2_mode = bcmFieldContextCompareTypeDouble;
    sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_FHEI_IPMF1", sizeof(context_info.name));

    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &itmh_pph_fhei_context_id));

    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmhPph;

    SHR_IF_ERR_EXIT(bcm_field_context_param_set
                    (unit, 0, bcmFieldStageIngressPMF1, itmh_pph_fhei_context_id, &param_info));
    
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x5;
    param_info.param_arg = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set
                    (unit, 0, bcmFieldStageIngressPMF1, itmh_pph_fhei_context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_fhei_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 3;
    presel_entry_data.context_id = itmh_pph_fhei_context_id;
    presel_entry_data.entry_valid = TRUE;

    
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    presel_entry_data.qual_data[0].qual_mask = -1;
    
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = 0x0001;
    presel_entry_data.qual_data[1].qual_mask = 0x0001;
    
    presel_entry_data.qual_data[2].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[2].qual_arg = 2;
    presel_entry_data.qual_data[2].qual_value = 0x20;
    presel_entry_data.qual_data[2].qual_mask = 0x30;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_cmp_key_config(unit));
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_ipmf2_contexts(
    int unit)
{
    bcm_field_context_t itmh_pph_fhei_context_id, itmh_pph_vlan_context_id, itmh_pph_context_id,
        itmh_pph_lif_context_id, itmh_pph_fhei_lif_context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;

    SHR_FUNC_INIT_VARS(unit);
    itmh_pph_context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);
    itmh_pph_fhei_context_id = dnx_data_field.context.default_itmh_pph_fhei_context_get(unit);
    itmh_pph_vlan_context_id = dnx_data_field.context.default_itmh_pph_fhei_vlan_ipmf2_context_get(unit);
    itmh_pph_lif_context_id = dnx_data_field.context.default_itmh_pph_lif_ipmf2_context_get(unit);
    itmh_pph_fhei_lif_context_id = dnx_data_field.context.default_itmh_pph_fhei_lif_ipmf2_context_get(unit);

    
    bcm_field_context_param_info_t_init(&param_info);
    
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset0, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, itmh_pph_context_id, &param_info));

    
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_FHEI_Vlan", sizeof(context_info.name));
    context_info.cascaded_from = itmh_pph_fhei_context_id;
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF2, &context_info, &itmh_pph_vlan_context_id));

    bcm_field_context_param_info_t_init(&param_info);
    
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset0, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set
                    (unit, 0, bcmFieldStageIngressPMF2, itmh_pph_vlan_context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_fhei_vlan_presel_id_ipmf2_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF2;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = itmh_pph_vlan_context_id;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyCompareKeysResult1;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = 0x2;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_fhei_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF2;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = itmh_pph_fhei_context_id;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyCompareKeysResult0;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = 0x2;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_context_param_info_t_init(&param_info);
    
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset0, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set
                    (unit, 0, bcmFieldStageIngressPMF2, itmh_pph_fhei_context_id, &param_info));

    
    
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_LIF", sizeof(context_info.name));
    context_info.cascaded_from = itmh_pph_context_id;
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF2, &context_info, &itmh_pph_lif_context_id));

    bcm_field_context_param_info_t_init(&param_info);
    
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset1, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set
                    (unit, 0, bcmFieldStageIngressPMF2, itmh_pph_lif_context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_lif_presel_id_ipmf2_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF2;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = itmh_pph_lif_context_id;
    presel_entry_data.entry_valid = TRUE;

    
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyCompareKeysResult0;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = 0x4;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    
    
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_FHEI_LIF", sizeof(context_info.name));
    context_info.cascaded_from = itmh_pph_fhei_context_id;
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF2, &context_info,
                     &itmh_pph_fhei_lif_context_id));

    bcm_field_context_param_info_t_init(&param_info);
    
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset1, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set
                    (unit, 0, bcmFieldStageIngressPMF2, itmh_pph_fhei_lif_context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_fhei_lif_presel_id_ipmf2_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF2;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = itmh_pph_fhei_lif_context_id;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyCompareKeysResult0;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = 0x1;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = 124;
    presel_entry_id.stage = bcmFieldStageIngressPMF2;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = itmh_pph_fhei_lif_context_id;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyCompareKeysResult0;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = 0x4;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_ipmf3_contexts(
    int unit)
{
    bcm_field_context_t itmh_pph_context_id, itmh_pph_fhei_trap_context_id, itmh_pph_fhei_vlan_context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t presel_entry_id;

    SHR_FUNC_INIT_VARS(unit);
    itmh_pph_context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);
    itmh_pph_fhei_trap_context_id = dnx_data_field.context.default_itmh_pph_fhei_context_get(unit);
    itmh_pph_fhei_vlan_context_id = dnx_data_field.context.default_itmh_pph_fhei_vlan_ipmf2_context_get(unit);

    
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_IPMF3", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF3, &context_info, &itmh_pph_context_id));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);

    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_presel_id_ipmf3_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF3;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = itmh_pph_context_id;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = itmh_pph_context_id;
    presel_entry_data.qual_data[0].qual_mask = 0x3F;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);

    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_presel_id_2nd_ipmf3_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF3;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = itmh_pph_context_id;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = itmh_pph_fhei_trap_context_id;
    presel_entry_data.qual_data[0].qual_mask = 0x3F;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);

    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_presel_id_vlan_ipmf3_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF3;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = itmh_pph_context_id;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = itmh_pph_fhei_vlan_context_id;
    presel_entry_data.qual_data[0].qual_mask = 0x3F;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
field_itmh_context(
    int unit)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_context_t itmh_context_id;

    SHR_FUNC_INIT_VARS(unit);

    itmh_context_id = dnx_data_field.context.default_itmh_context_get(unit);

    
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "ITMH_IPMF1", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &itmh_context_id));
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmh;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, itmh_context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = itmh_context_id;
    presel_entry_data.entry_valid = TRUE;

    
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    presel_entry_data.qual_data[0].qual_mask = -1;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_context_param_info_t_init(&param_info);
    
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset1, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, itmh_context_id, &param_info));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
field_itmh_1588_context(
    int unit)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_context_t context_id = dnx_data_field.context.default_itmh_1588_tsh_context_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "ITMH_1588", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &context_id));
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmhTsh;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_1588_tsh_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;

    
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    presel_entry_data.qual_data[0].qual_mask = -1;

    
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[1].qual_arg = 1;
    presel_entry_data.qual_data[1].qual_value = 0x2;
    presel_entry_data.qual_data[1].qual_mask = 0x7;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_context_param_info_t_init(&param_info);
    
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset1, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &param_info));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_group_context_attach(
    int unit,
    bcm_field_group_t fg_id,
    int context_ids,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_context_t itmh_context_id, itmh_1588_context_id;

    SHR_FUNC_INIT_VARS(unit);

    itmh_context_id = dnx_data_field.context.default_itmh_context_get(unit);
    itmh_1588_context_id = dnx_data_field.context.default_itmh_1588_tsh_context_get(unit);

    if (context_ids & FLAG_BIT_ITMH_CONTEXT)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, itmh_context_id, attach_info));
    }
    {
        if (context_ids & FLAG_BIT_ASE_1588_CONTEXT)
        {
            SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, itmh_1588_context_id, attach_info));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_group_context_attach(
    int unit,
    bcm_field_group_t fg_id,
    int context_ids,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_context_t pph_context_id, vlan_context_id, fhei_context_id, itmh_pph_lif_context_id,
        itmh_pph_fhei_lif_context_id;

    SHR_FUNC_INIT_VARS(unit);

    pph_context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);
    fhei_context_id = dnx_data_field.context.default_itmh_pph_fhei_context_get(unit);
    vlan_context_id = dnx_data_field.context.default_itmh_pph_fhei_vlan_ipmf2_context_get(unit);
    itmh_pph_lif_context_id = dnx_data_field.context.default_itmh_pph_lif_ipmf2_context_get(unit);
    itmh_pph_fhei_lif_context_id = dnx_data_field.context.default_itmh_pph_fhei_lif_ipmf2_context_get(unit);

    if (context_ids & FLAG_BIT_PPH_CONTEXT)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, pph_context_id, attach_info));
    }

    if (context_ids & FLAG_BIT_PPH_FHEI_CONTEXT)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, fhei_context_id, attach_info));
    }

    if (context_ids & FLAG_BIT_PPH_FHEI_VLAN_CONTEXT)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, vlan_context_id, attach_info));
    }

    if (context_ids & FLAG_BIT_ITMH_CONTEXT)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, itmh_pph_lif_context_id, attach_info));
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, itmh_pph_fhei_lif_context_id, attach_info));
    }

    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_context_attach(unit, fg_id, context_ids, attach_info));
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_context_create(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(field_itmh_context(unit));
    {
        SHR_IF_ERR_EXIT(field_itmh_1588_context(unit));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_context_create(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_ipmf1_contexts(unit));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_ipmf2_contexts(unit));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_ipmf3_contexts(unit));

    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_context_create(unit));
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_field_group_create(
    int unit)
{
    bcm_field_group_t fg_id;
    bcm_field_group_attach_info_t attach_info;
    int flow_id = dnx_data_field.init.flow_id_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_context_attach
                    (unit, fg_id,
                     FLAG_BIT_ITMH_CONTEXT | FLAG_BIT_ASE_1588_CONTEXT | FLAG_BIT_PPH_CONTEXT |
                     FLAG_BIT_PPH_FHEI_CONTEXT | FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));

    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_only_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_context_attach
                    (unit, fg_id, FLAG_BIT_ITMH_CONTEXT | FLAG_BIT_ASE_1588_CONTEXT, &attach_info));

    
    SHR_IF_ERR_EXIT(appl_dnx_field_tsh_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_context_attach
                    (unit, fg_id,
                     FLAG_BIT_ASE_1588_CONTEXT | FLAG_BIT_PPH_CONTEXT | FLAG_BIT_PPH_FHEI_CONTEXT |
                     FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));

    if (flow_id)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_flow_id_group_config(unit, &fg_id, &attach_info));
        SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_context_attach
                        (unit, fg_id,
                         FLAG_BIT_ITMH_CONTEXT | FLAG_BIT_ASE_1588_CONTEXT | FLAG_BIT_PPH_CONTEXT |
                         FLAG_BIT_PPH_FHEI_CONTEXT | FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));
    }
    
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_ase_1588_group_config(unit, &fg_id, &attach_info));
        SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_context_attach(unit, fg_id, FLAG_BIT_ASE_1588_CONTEXT, &attach_info));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_dnx_field_itmh_pph_field_group_create(
    int unit)
{
    bcm_field_group_t fg_id;
    bcm_field_group_attach_info_t attach_info;
    int flow_id = dnx_data_field.init.flow_id_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_only_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                    (unit, fg_id, FLAG_BIT_ITMH_CONTEXT | FLAG_BIT_ASE_1588_CONTEXT, &attach_info));

    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                    (unit, fg_id,
                     FLAG_BIT_ITMH_CONTEXT | FLAG_BIT_ASE_1588_CONTEXT | FLAG_BIT_PPH_CONTEXT |
                     FLAG_BIT_PPH_FHEI_CONTEXT | FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));

    
    SHR_IF_ERR_EXIT(appl_dnx_field_tsh_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                    (unit, fg_id,
                     FLAG_BIT_ASE_1588_CONTEXT | FLAG_BIT_PPH_CONTEXT | FLAG_BIT_PPH_FHEI_CONTEXT |
                     FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));

    
    SHR_IF_ERR_EXIT(appl_dnx_field_pph_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                    (unit, fg_id, FLAG_BIT_PPH_CONTEXT | FLAG_BIT_PPH_FHEI_CONTEXT | FLAG_BIT_PPH_FHEI_VLAN_CONTEXT,
                     &attach_info));

    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_fhei_trap_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach(unit, fg_id, FLAG_BIT_PPH_FHEI_CONTEXT, &attach_info));

    
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_fhei_vlan_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                    (unit, fg_id, FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));

    if (flow_id)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_flow_id_group_config(unit, &fg_id, &attach_info));
        SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                        (unit, fg_id,
                         FLAG_BIT_ITMH_CONTEXT | FLAG_BIT_ASE_1588_CONTEXT | FLAG_BIT_PPH_CONTEXT |
                         FLAG_BIT_PPH_FHEI_CONTEXT | FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));
    }
    
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_ase_1588_group_config(unit, &fg_id, &attach_info));
        SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                        (unit, fg_id, FLAG_BIT_ASE_1588_CONTEXT, &attach_info));
    }

    
    SHR_IF_ERR_EXIT(appl_dnx_field_ase_oam_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                    (unit, fg_id, FLAG_BIT_PPH_CONTEXT | FLAG_BIT_PPH_FHEI_CONTEXT | FLAG_BIT_PPH_FHEI_VLAN_CONTEXT,
                     &attach_info));

    
    SHR_IF_ERR_EXIT(appl_dnx_field_ipmf3_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach(unit, fg_id, FLAG_BIT_PPH_CONTEXT, &attach_info));

    SHR_IF_ERR_EXIT(appl_dnx_field_ipmf3_const_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach(unit, fg_id, FLAG_BIT_PPH_CONTEXT, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_dnx_field_itmh_pph_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_context_create(unit));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_field_group_create(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_dnx_field_itmh_init(
    int unit)
{
    uint8 is_alloc;
    bcm_field_context_t itmh_context_id = dnx_data_field.context.default_itmh_context_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, DNX_FIELD_STAGE_IPMF1, itmh_context_id, &is_alloc));

    
    if (is_alloc)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_context_create(unit));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_field_group_create(unit));

exit:
    SHR_FUNC_EXIT;
}


