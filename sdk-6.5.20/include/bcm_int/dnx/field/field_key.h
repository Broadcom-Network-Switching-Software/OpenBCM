
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef FIELD_FIELD_KEY_H_INCLUDED

#define FIELD_FIELD_KEY_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <include/bcm_int/dnx/field/field.h>
#include <include/bcm_int/dnx/field/field_map.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>






#define DNX_FIELD_KEY_LOOKUP_STARTING_OFFSET 0

#define DNX_FIELD_KEY_DE_STARTING_OFFSET 0

#define DNX_FIELD_KEY_HASH_STARTING_OFFSET 0

#define DNX_FIELD_KEY_COMPARE_STARTING_OFFSET(_unit) \
       (dnx_data_field.tcam.key_size_single_get(_unit) - \
        dnx_data_field.stage.stage_info_get(_unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size)


#define DNX_FIELD_KEY_MDB_DT_STARTING_OFFSET(_unit) \
    (dnx_data_field.tcam.key_size_single_get(_unit) - dnx_data_field.map.key_size_get(_unit))



typedef struct
{
    
    uint32 size;
    
    int offset;
    
    int index;
    
    dnx_field_ffc_type_e ffc_type;
    
    uint8 ranges;
    
    dnx_field_input_type_e input_type;
    
    int native_pbus;
} dnx_field_key_attached_qual_info_t;


typedef struct
{
    
    dnx_field_stage_e field_stage;
    
    dnx_field_group_type_e fg_type;

    
    dnx_field_key_template_t key_template;

    
    dnx_field_key_length_type_e key_length;
    
    dnx_field_app_db_id_t app_db_id;
    
    dnx_field_qual_attach_info_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];

    
    dnx_field_group_compare_id_e compare_id;

} dnx_field_key_attach_info_in_t;


typedef struct
{
    
    int bit_range_valid;
    
    int bit_range_align_lsb;
    
    int bit_range_align_lsb_step;
    
    int bit_range_lsb_aligned_offset;
    
    int bit_range_align_msb;
    
    int bit_range_align_msb_step;
} dnx_field_key_de_bit_allocation_info_t;


typedef struct
{
    
    dbal_fields_e ffc_type;
    
    int offset;
    
    int field_index;
    
    uint32 key_offset;
    
    uint32 size;
    
    uint32 full_instruction;
} dnx_field_key_ffc_instruction_info_t;


typedef struct
{
    
    uint32 ffc_id;
    
    dbal_enum_value_field_field_key_e key_id;
    
    dnx_field_key_ffc_instruction_info_t ffc_instruction;
} dnx_field_key_ffc_hw_info_t;


typedef struct
{
    
    dnx_field_key_ffc_hw_info_t ffc;
    
    uint32 key_dest_offset;
} dnx_field_key_initial_ffc_info_t;


typedef struct
{
    
    dnx_field_key_ffc_hw_info_t ffc;
    
    dnx_field_key_initial_ffc_info_t ffc_initial;
} dnx_field_key_ffc_and_initial_ffc_info_t;


typedef struct
{
    
    dnx_field_qual_t qual_type;
    
    dnx_field_key_ffc_and_initial_ffc_info_t ffc_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL];
} dnx_field_key_qualifier_ffc_info_t;


typedef struct
{
    
    dnx_field_key_qualifier_ffc_info_t qualifier_ffc_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
} dnx_field_key_group_key_info_t;


typedef struct
{
    
    dnx_field_key_group_key_info_t key;
} dnx_field_key_group_ffc_info_t;



typedef struct
{
    
    uint32 is_key_id_valid;
    
    uint32 offset_start;
    
    uint32 offset_end;
} dnx_field_key_ffc_sharing_info_t;




shr_error_e dnx_field_key_kbr_ffc_array_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC]);

shr_error_e dnx_field_key_template_t_init(
    int unit,
    dnx_field_key_template_t * key_template_p);


shr_error_e dnx_field_key_attach_info_in_t_init(
    int unit,
    dnx_field_key_attach_info_in_t * key_in_info_p);


shr_error_e dnx_field_qual_attach_info_t_init(
    int unit,
    dnx_field_qual_attach_info_t * qual_info_p);


shr_error_e dnx_field_key_id_t_init(
    int unit,
    dnx_field_key_id_t *key_p);


shr_error_e dnx_field_key_bit_range_t_init(
    int unit,
    dnx_field_bit_range_t * bit_range_p);


shr_error_e dnx_field_key_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p);


shr_error_e dnx_field_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_de_bit_allocation_info_t * bit_range_allocation_info_p,
    dnx_field_key_id_t *key_id_p);


shr_error_e dnx_field_key_context_feature_key_detach(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p,
    dnx_field_key_template_t * key_template_p);


shr_error_e dnx_field_key_context_feature_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p);


shr_error_e dnx_field_key_compare_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_key_info_t * compare_key_info_p);


shr_error_e dnx_field_key_qual_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_info_p,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p);


shr_error_e dnx_field_key_qual_info_uses_up_to_one_ffc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_info_p,
    uint8 *is_up_to_one_ffc_p);


shr_error_e dnx_field_key_template_create(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t * qual_types_p,
    uint32 starting_offset,
    dnx_field_key_template_t * key_template_p);


shr_error_e dnx_field_key_single_initial_ffc_get(
    int unit,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e initial_key_id,
    uint32 ffc_id,
    dnx_field_key_initial_ffc_info_t * ipmf1_ffc_initial_p);


shr_error_e dnx_field_key_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_id_t *key_id_p,
    dnx_field_key_template_t * key_template_p,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p,
    dnx_field_app_db_id_t * app_db_id_p);



shr_error_e dnx_field_key_sw_state_init(
    int unit);


shr_error_e dnx_field_key_compare_mode_single_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p,
    dnx_field_key_id_t *tcam_key_id_p);

shr_error_e dnx_field_key_compare_mode_single_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p,
    dnx_field_key_id_t *tcam_key_id_p);

shr_error_e dnx_field_key_compare_mode_double_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_1_p,
    dnx_field_key_id_t *initial_key_id_2_p);

shr_error_e dnx_field_key_compare_mode_double_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_1_p,
    dnx_field_key_id_t *initial_key_id_2_p);

shr_error_e dnx_field_key_hash_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id);

shr_error_e dnx_field_key_hash_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p);

shr_error_e dnx_field_key_state_table_resources_reserve(
    int unit,
    dnx_field_stage_e context_field_stage,
    dnx_field_context_t context_id);

shr_error_e dnx_field_key_state_table_resources_free(
    int unit,
    dnx_field_stage_e context_field_stage,
    dnx_field_context_t context_id);

shr_error_e dnx_field_key_qual_attach_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    dnx_field_qual_attach_info_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG]);


shr_error_e dnx_field_key_kbp_fwd_zero_padding_fill(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    int segment_id,
    dnx_field_group_kbp_segment_info_t * segment_info_p,
    int is_zero_padding,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id_that_uses_zero_padding,
    int ffc_id);


shr_error_e dnx_field_key_kbp_acl_compare_qualifiers_for_sharing(
    int unit,
    int nof_quals_1,
    dnx_field_qual_t dnx_qual_1[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_attach_info_1[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    int nof_quals_2,
    dnx_field_qual_t dnx_qual_2[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_attach_info_2[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    int *fully_shareable_p,
    int *partially_shareable_p,
    int *partial_sharing_lsb_offset_p);


shr_error_e dnx_field_key_kbp_segment_position(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    uint8 fwd_context,
    dnx_field_context_t acl_context_id,
    int seg_idx_on_master_key,
    dbal_enum_value_field_kbp_kbr_idx_e * key_kbp_id_p,
    int *lsb_on_key_p,
    int *size_on_key_p);


shr_error_e dnx_field_key_kbp_fwd_segment_unified_position(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    int seg_idx_on_master_key,
    dbal_enum_value_field_kbp_kbr_idx_e * key_kbp_id_p,
    int *lsb_on_key_p,
    int *size_on_key_p);


shr_error_e dnx_field_key_kbp_master_offset_to_key(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    int offset_on_master_from_msb,
    int size_on_key,
    dbal_enum_value_field_kbp_kbr_idx_e * key_kbp_id_last_p,
    uint32 *keys_bmp_p,
    int *offset_on_last_key_p);


shr_error_e dnx_field_key_find_common_available_ffc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t contexts_to_share_ffc[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS],
    unsigned int nof_context_to_share_ffc,
    int *ffc_id_p);

 
shr_error_e dnx_field_key_kbp_set(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    uint8 fwd_context,
    dnx_field_context_t acl_context_id,
    dnx_field_group_kbp_info_t * kbp_opcode_info_p);


shr_error_e dnx_field_key_fwd2_to_acl_key_id_convert(
    int unit,
    dbal_enum_value_field_field_key_e key_id,
    dbal_enum_value_field_kbp_kbr_idx_e * kbp_kbr_id_p);


shr_error_e dnx_field_key_acl_to_fwd2_key_id_convert(
    int unit,
    dbal_enum_value_field_kbp_kbr_idx_e kbp_kbr_id,
    dbal_enum_value_field_field_key_e * key_id_p);


shr_error_e dnx_field_key_kbp_ffc_info_get(
    int unit,
    dnx_field_group_t fg_id,
    int opcode_id,
    uint8 fwd_context,
    dnx_field_context_t acl_context_id,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p);


shr_error_e dnx_field_key_ifwd2_acl_ffc_update_for_new_context(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e base_opcode_id,
    uint8 fwd_context_id,
    dnx_field_context_t acl_context_id);


shr_error_e dnx_field_key_ifwd2_acl_ffc_init(
    int unit);


shr_error_e dnx_field_key_ifwd2_acl_key_verify(
    int unit);


shr_error_e dnx_field_key_l4_ops_ffc_set(
    int unit,
    dnx_field_qual_t qual_type,
    uint32 ffc_id,
    dnx_field_qual_attach_info_t * qual_attach_info_p);



#endif
