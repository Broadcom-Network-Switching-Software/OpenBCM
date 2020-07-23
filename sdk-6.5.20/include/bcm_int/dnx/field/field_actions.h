
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef FIELD_FIELD_ACTIONS_H_INCLUDED

#define FIELD_FIELD_ACTIONS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif


#include <sal/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_actions_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_actions_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <bcm_int/dnx/field/field_entry.h>




#define FES_INFO_VALID_TYPE_ALWAYS         1

#define FES_INFO_VALID_TYPE_CONDITIONAL    0
#define FES_INFO_VALID_POLARITY_ASSERTED   1
#define FES_INFO_VALID_POLARITY_DEASSERTED 0

#define DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ONE (1)
#define DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ZERO (0)

#define DNX_FIELD_NOF_BITS_2MSB_SELECT 2

#define DNX_FIELD_KEY_SELECT_BIT_GRANULARITY                  32

#define DNX_FIELD_ACTIONS_NUM_LOCATIONS_OF_2MSB_ON_FG  \
    ((((DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) * (DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE)) \
      + (DNX_FIELD_KEY_SELECT_BIT_GRANULARITY + 1)) / (DNX_FIELD_KEY_SELECT_BIT_GRANULARITY))


#define DNX_FIELD_INVALID_FEM_KEY_SELECT     DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_INVALID_FEM_KEY_SELECT


#define DNX_FIELD_FEM_CONTEXT_NOT_OCCUPIED_VERIFY(_unit,_fem_id,_context_id)  \
{  \
      \
    dnx_field_pmf_fem_context_entry_t _loc_dnx_field_pmf_fem_program_entry;  \
      \
    _loc_dnx_field_pmf_fem_program_entry.fem_id = _fem_id;  \
    _loc_dnx_field_pmf_fem_program_entry.context_id = _context_id;  \
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(_unit, &_loc_dnx_field_pmf_fem_program_entry));  \
    if (_loc_dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select !=  \
        DNX_FIELD_INVALID_FEM_KEY_SELECT)  \
    {  \
        SHR_ERR_EXIT(_SHR_E_PARAM,  \
                     "This 'fem_id' (%d) on this fem context_id (%d) is already occupied (fes_key_select %d) while is should have been free. Quit. \n",  \
                     _fem_id, _context_id, _loc_dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select);  \
    }  \
}

#define DNX_FIELD_FEM_CONTEXT_OCCUPIED_VERIFY(_unit,_fem_id,_context_id)  \
{  \
      \
    dnx_field_pmf_fem_context_entry_t _loc_dnx_field_pmf_fem_program_entry;  \
      \
    _loc_dnx_field_pmf_fem_program_entry.fem_id = _fem_id;  \
    _loc_dnx_field_pmf_fem_program_entry.context_id = _context_id;  \
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(_unit, &_loc_dnx_field_pmf_fem_program_entry));  \
    if (_loc_dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select ==  \
        DNX_FIELD_INVALID_FEM_KEY_SELECT)  \
    {  \
        SHR_ERR_EXIT(_SHR_E_PARAM,  \
                     "This 'fem_id' (%d) on this fem context_id (%d) is free (fes_key_select %d) while is shoul have been occupied. Quit. \n",  \
                     _fem_id, _context_id, _loc_dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select);  \
    }  \
}





typedef struct
{
    
    dnx_field_action_type_t action_type;
    
    dnx_field_fes_valid_bits_t valid_bits;
    
    dnx_field_fes_shift_t shift;
    
    dnx_field_fes_type_t type;
    dnx_field_fes_polarity_t polarity;
} __ATTRIBUTE_PACKED__ dnx_field_actions_fes_common_info_fes2msb_t;


typedef struct
{
    
    dnx_field_actions_fes_common_info_fes2msb_t fes2msb_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
} __ATTRIBUTE_PACKED__ dnx_field_actions_fes_common_info_t;


typedef struct
{
    
    uint32 required_mask;
} __ATTRIBUTE_PACKED__ dnx_field_actions_fes_alloc_info_fes2msb_t;


typedef struct
{
    
    dnx_field_actions_fes_alloc_info_fes2msb_t fes2msb_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
    
    dnx_field_action_priority_t priority;
} __ATTRIBUTE_PACKED__ dnx_field_actions_fes_alloc_info_t;


typedef struct
{
    
    dnx_field_fes_key_select_t key_select;
} __ATTRIBUTE_PACKED__ dnx_field_actions_fes_write_info_t;


typedef struct
{
    
    dnx_field_actions_fes_common_info_t common_info;
    
    dnx_field_actions_fes_alloc_info_t alloc_info;
    
    dnx_field_actions_fes_write_info_t write_info;
} __ATTRIBUTE_PACKED__ dnx_field_actions_fes_info_t;


typedef struct
{
    
    dnx_field_action_t dnx_action;
    
    uint8 size;
    
    uint8 lsb;
    
    uint8 dont_use_valid_bit;
    
    uint8 valid_bit_polarity;
    
    dnx_field_fes_mask_t mask;
} dnx_field_actions_fes_single_add_conf_t;


typedef struct
{
    
    dnx_field_qual_t qual_type;
    
    unsigned int val;
} dnx_field_dir_ext_quals_for_msb_bits_t;


typedef struct
{
    
    dnx_field_action_type_t action_type;
    dnx_field_fes_valid_bits_t valid_bits;
    dnx_field_fes_shift_t shift;
    dnx_field_fes_type_t type;
    dnx_field_fes_polarity_t polarity;
    dnx_field_fes_chosen_mask_t chosen_mask;
    
    dnx_field_fes_mask_t mask;
} dnx_field_actions_fes_quartet_get_info_fes2msb_t;


typedef struct
{
    
    dnx_field_fes_id_t fes_id;
    
    dnx_field_fes_pgm_id_t fes_pgm_id;
    
    dnx_field_action_priority_t priority;
    
    dnx_field_group_t fg_id;
    
    uint8 place_in_fg;
    
    dnx_field_fes_key_select_t key_select;
    
    dnx_field_actions_fes_quartet_get_info_fes2msb_t fes2msb_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
} dnx_field_actions_fes_quartet_context_get_info_t;


typedef struct
{
    
    unsigned int nof_fes_quartets;
    
        dnx_field_actions_fes_quartet_context_get_info_t
        fes_quartets[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
} dnx_field_actions_fes_context_get_info_t;


typedef struct
{
    
    dnx_field_fes_id_t fes_id;
    
    dnx_field_fes_pgm_id_t fes_pgm_id;
    
    dnx_field_action_priority_t priority;
    
    dnx_field_fes_key_select_t key_select;
    
    dnx_field_actions_fes_quartet_get_info_fes2msb_t fes2msb_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
} dnx_field_actions_fes_quartet_context_group_get_info_t;


typedef struct
{
    
    dnx_field_actions_fes_quartet_context_group_get_info_t
        initial_fes_quartets[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    
    dnx_field_actions_fes_quartet_context_group_get_info_t
        added_fes_quartets[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
} dnx_field_actions_fes_context_group_get_info_t;


typedef struct
{
    
    dnx_field_actions_fes_quartet_get_info_fes2msb_t fes2msb_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
} dnx_field_actions_fes_pgm_ctx_info_t;


typedef struct
{
    
    uint32 context_bmp[BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS)];

    
    dnx_field_group_t field_group;

    
    uint8 masks_id_used[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];

    
    dnx_field_actions_fes_pgm_ctx_info_t pgm_ctx_2msb_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
} dnx_field_actions_fes_pgm_info_t;


typedef struct
{
    
    dnx_field_fes_mask_t action_masks[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    
    dnx_field_actions_fes_pgm_info_t fes_pgm_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS];
    
    uint8 place_in_fg;
    
    dnx_field_fes_key_select_t key_select;

} dnx_field_actions_fes_id_get_info_t;


typedef struct
{
    
    dnx_field_action_t dnx_action;
    
    dbal_enum_value_field_field_io_e field_io;
    
    unsigned int lsb;
    
    uint8 dont_use_valid_bit;
    
    dnx_field_action_priority_t priority;
    
    uint8 valid_bit_polarity;
} dnx_field_actions_single_fes_add_t;


typedef struct
{
    
    dnx_field_fes_id_t fes_id;
    
    dnx_field_action_type_t action_type;
    dnx_field_fes_valid_bits_t valid_bits;
    dnx_field_fes_shift_t shift;
    dnx_field_fes_type_t type;
    dnx_field_fes_polarity_t polarity;
    dnx_field_fes_chosen_mask_t chosen_mask;
    dnx_field_fes_mask_t mask;
} dnx_field_actions_fes_ace_instr_get_info_t;


typedef struct
{
    
    unsigned int nof_fes_instr;
    
    dnx_field_actions_fes_ace_instr_get_info_t fes_instr[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT];
} dnx_field_actions_fes_ace_get_info_t;


typedef struct
{
    
    dnx_field_action_in_group_info_t actions_on_payload_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
} dnx_field_actions_fg_payload_info_t;


typedef struct
{
    
    dnx_field_action_in_group_info_t actions_on_payload_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
} dnx_field_actions_ace_payload_info_t;




typedef struct
{
    
    uint8 available;
    
    unsigned int lsb;
    
    unsigned int bit_width;
    
    uint32 validity_condition;

} dnx_field_dir_ext_fes_data_2msb_information_t;


typedef struct
{
    
    dnx_field_qual_t qualifier_type;
    
    unsigned int bit_on_fes_data;
} dnx_field_actions_feature_2msb_qualifier_map_info_t;


shr_error_e dnx_field_actions_io_to_replace_select(
    int unit,
    dbal_enum_value_field_field_io_e field_io,
    dbal_enum_value_field_field_fem_replace_lsb_select_e * replace_lsb_select_p);


shr_error_e dnx_field_action_attach_info_single_t_init(
    int unit,
    dnx_field_action_attach_info_t * action_info_p);


shr_error_e dnx_field_action_attach_info_t_init(
    int unit,
    dnx_field_action_attach_info_t action_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP]);


shr_error_e dnx_field_actions_fes_single_add_conf_t_init(
    int unit,
    dnx_field_actions_fes_single_add_conf_t efes_action_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES]);


shr_error_e dnx_field_action_in_group_info_t_init(
    int unit,
    dnx_field_action_in_group_info_t * struct_p);


shr_error_e dnx_field_action_in_group_bit_size(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_action_in_group_info_t * action_p,
    unsigned int *action_size_w_valid_p,
    unsigned int *action_size_wo_valid_p);


shr_error_e dnx_field_actions_place_actions(
    int unit,
    dnx_field_stage_e field_stage,
    unsigned int max_nof_bits_on_payload,
    dnx_field_action_in_group_info_t actions_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP]);


shr_error_e dnx_field_actions_calc_feses(
    int unit,
    dnx_field_stage_e field_stage,
    dbal_enum_value_field_field_io_e field_io,
    dnx_field_action_in_group_info_t actions_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    int template_shift,
    dnx_field_action_attach_info_t action_attach_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP]);


shr_error_e dnx_field_actions_fes_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    int is_nonshareable,
    int is_post_attach,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP]);

shr_error_e dnx_field_actions_fes_ace_set(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP]);


shr_error_e dnx_field_actions_fes_detach(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_fes_id_t fes_id);


shr_error_e dnx_field_actions_fes_ace_detach(
    int unit,
    dnx_field_ace_id_t ace_id);


shr_error_e dnx_field_actions_context_fes_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_actions_fes_context_get_info_t * context_fes_get_info_p);


shr_error_e dnx_field_actions_fes_id_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    uint32 fes_id,
    dnx_field_actions_fes_id_get_info_t * fes_id_get_info_p);


shr_error_e dnx_field_actions_context_fes_info_to_group_fes_info(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_actions_fes_context_get_info_t * fes_context_info_p,
    dnx_field_actions_fes_context_group_get_info_t * fes_group_info_p);


shr_error_e dnx_field_actions_ace_id_nof_mask_state_get(
    int unit,
    uint8 nof_masks[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT]);


shr_error_e dnx_field_actions_ace_id_fes_info_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_actions_fes_ace_get_info_t * ace_id_fes_get_info_p);


shr_error_e dnx_field_efes_action_add(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    bcm_field_action_priority_t action_priority,
    int use_condition,
    int efes_condition_msb,
    dnx_field_actions_fes_single_add_conf_t
    efes_encoded_extraction[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES]);


shr_error_e dnx_field_efes_action_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    bcm_field_action_priority_t action_priority,
    int *use_condition_p,
    int *efes_condition_msb_p,
    dnx_field_actions_fes_single_add_conf_t
    efes_encoded_extraction[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES]);


shr_error_e dnx_field_efes_action_remove(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    bcm_field_action_priority_t action_priority);


shr_error_e dnx_field_efes_action_context_create_fes_update(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t cascaded_from);


shr_error_e dnx_field_efes_action_context_destroy_fes_update(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t cascaded_from);


shr_error_e dnx_field_actions_fes_info_t_init(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP]);




#define DNX_FIELD_INIT_FEM_TABLES_FAST          0

#define DNX_FIELD_FEM_BIT_FOR_MAP_TYPE          (dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_bits_in_fem_field_select - 1)

#define DNX_FIELD_FEM_MASK_FOR_MAP_TYPE       SAL_BIT(DNX_FIELD_FEM_BIT_FOR_MAP_TYPE)

#define DNX_FIELD_FEM_MAP_TYPE_KEY_SELECT    0

#define DNX_FIELD_FEM_MASK_FOR_MAP_DATA        \
    (SAL_UPTO_BIT(dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_bits_in_fem_field_select) & \
    SAL_FROM_BIT(dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->log_nof_bits_in_fem_map_data_field))

#define DNX_FIELD_FEM_MASK_OF_MAP_DATA        0x20

#define DNX_FIELD_FEM_MASK_OF_BIT_VALUE       0x30

#define DNX_FIELD_FEM_NUM_VALUES_ON_BIT_VALUE 2

#define FEM_INFO                     dnx_field_action_sw_db.fem_info
#define FEM_INFO_FG_ID_INFO          FEM_INFO.fg_id_info
#define FEM_INFO_FEM_ENCODED_ACTIONS FEM_INFO.fg_id_info.fem_encoded_actions


#define DEFAULT_FEM_PROGRAM         0

#define FIRST_AVAILABLE_FEM_PROGRAM (DEFAULT_FEM_PROGRAM + 0)



typedef struct
{
    
    dnx_field_fem_id_t fem_id;
    
    dnx_field_fem_program_t fem_program;
    
    dnx_field_fem_condition_t fem_condition;
    
    dnx_field_fem_condition_entry_t fem_condition_entry;
} dnx_field_fem_map_index_entry_t;

typedef struct
{
    
    dnx_field_fem_id_t fem_id;
    
    dnx_field_fem_map_index_t fem_map_index;
    
    dnx_field_fem_action_entry_t fem_action_entry;
} dnx_field_fem_action_info_t;

typedef struct
{
    
    dnx_field_fem_id_t fem_id;
    
    dnx_field_fem_program_t fem_program;
    
    dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit;
} dnx_field_fem_condition_ms_bit_entry_t;

typedef struct
{
    
    dnx_field_fem_program_t fem_program;
    
    dbal_enum_value_field_field_fem_replace_lsb_flag_e replace_lsb_flag;
    
    dbal_enum_value_field_field_fem_replace_lsb_select_e replace_lsb_select;
    
    dbal_enum_value_field_field_pmf_a_fem_key_select_e fem_key_select;
} dnx_field_pmf_fem_program_t;

typedef struct
{
    
    dnx_field_fem_id_t fem_id;
    
    dnx_field_context_t context_id;
    
    dnx_field_pmf_fem_program_t pmf_fem_program;
} dnx_field_pmf_fem_context_entry_t;



shr_error_e dnx_field_actions_fem_key_select_get(
    int unit,
    dbal_enum_value_field_field_io_e actions_block,
    unsigned int action_size,
    unsigned int action_lsb,
    dbal_enum_value_field_field_pmf_a_fem_key_select_e * action_input_select_p,
    unsigned int *required_shift_p,
    int *found_p);

shr_error_e dnx_field_single_fem_prg_sw_state_init(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program);



shr_error_e dnx_field_fem_is_any_fem_occupied_on_fg(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_group_t second_fg_id,
    uint8 active_actions,
    int *none_occupied_p);



shr_error_e dnx_field_all_fems_sw_state_init(
    int unit);


shr_error_e dnx_field_fem_condition_entry_t_init(
    int unit,
    dnx_field_fem_condition_entry_t * struct_p);

shr_error_e dnx_field_fem_condition_entry_t_array_init(
    int unit,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION]);

shr_error_e dnx_field_fem_is_fg_owner(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_group_t fg_id,
    dnx_field_fem_program_t * fem_program_p);


shr_error_e dnx_field_fem_get_next_fg_owner(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_fem_id_t * fem_id_p,
    dnx_field_fem_program_t * fem_program_p);


shr_error_e dnx_field_fem_is_fg_secondary(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_group_t second_fg_id,
    dnx_field_fem_program_t * fem_program_p);


shr_error_e dnx_field_fem_is_fg_secondary_on_any(
    int unit,
    dnx_field_group_t second_fg_id,
    dnx_field_fem_id_t * fem_id_p);


shr_error_e dnx_field_fem_num_action_bits_verify(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_action_t dnx_encoded_fem_action,
    dnx_field_fem_action_entry_t * fem_action_entry_p);

shr_error_e dnx_field_pmf_fem_context_entry_t_init(
    int unit,
    dnx_field_pmf_fem_context_entry_t * struct_p);

shr_error_e dnx_field_fem_action_entry_t_init(
    int unit,
    dnx_field_fem_action_entry_t * struct_p);


shr_error_e dnx_field_fem_is_context_using(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    unsigned int *using_fem_p);


shr_error_e dnx_field_fem_is_context_valid(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_context_t context_id,
    unsigned int *is_valid_p);


shr_error_e dnx_field_fem_is_context_consistent(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_context_t context_id,
    unsigned int *consistent_p);

shr_error_e dnx_field_fem_is_context_valid_replace(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_context_t context_id,
    unsigned int *valid_replace_p);


shr_error_e dnx_field_fem_is_any_context_with_replace(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_context_t * context_id_p);


shr_error_e dnx_field_fem_is_any_context_with_program(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_context_t * context_id_p);


shr_error_e dnx_field_fem_action_remove(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_group_t fg_id);

shr_error_e dnx_field_fem_action_add(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_group_t fg_id,
    uint8 input_offset,
    uint8 input_size,
    dnx_field_group_t second_fg_id,
    dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_action_t fem_encoded_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX]);

shr_error_e dnx_field_actions_fem_defaults_set(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    uint8 ignore_actions);

shr_error_e dnx_field_actions_fem_set(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    uint8 ignore_actions,
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX]);

shr_error_e dnx_field_actions_fem_get(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_fem_condition_ms_bit_t * fem_condition_ms_bit_p,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX]);


shr_error_e dnx_field_actions_single_fem_single_context_defaults_set(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_fem_id_t fem_id);

shr_error_e dnx_field_actions_all_fem_single_context_defaults_set(
    int unit,
    dnx_field_context_t context_id);

shr_error_e dnx_field_actions_all_fem_all_context_defaults_set(
    int unit);

shr_error_e dnx_field_actions_fem_context_hw_set(
    int unit,
    dnx_field_pmf_fem_context_entry_t * dnx_field_pmf_fem_program_entry_p);

shr_error_e dnx_field_actions_fem_context_hw_get(
    int unit,
    dnx_field_pmf_fem_context_entry_t * dnx_field_pmf_fem_program_entry_p);

shr_error_e dnx_field_actions_fem_condition_ms_bit_hw_set(
    int unit,
    dnx_field_fem_condition_ms_bit_entry_t * dnx_field_fem_condition_ms_bit_entry_p);

shr_error_e dnx_field_actions_fem_condition_ms_bit_hw_get(
    int unit,
    dnx_field_fem_condition_ms_bit_entry_t * dnx_field_fem_condition_ms_bit_entry_p);

shr_error_e dnx_field_actions_fem_map_index_hw_set(
    int unit,
    dnx_field_fem_map_index_entry_t * dnx_field_fem_map_index_entry_p);

shr_error_e dnx_field_actions_fem_map_index_hw_get(
    int unit,
    dnx_field_fem_map_index_entry_t * dnx_field_fem_map_index_entry_p);

shr_error_e dnx_field_actions_fem_action_info_hw_set(
    int unit,
    dnx_field_fem_action_info_t * dnx_field_fem_action_info_p);

shr_error_e dnx_field_actions_fem_action_info_hw_get(
    int unit,
    dnx_field_fem_action_info_t * dnx_field_fem_action_info_p);




#endif
