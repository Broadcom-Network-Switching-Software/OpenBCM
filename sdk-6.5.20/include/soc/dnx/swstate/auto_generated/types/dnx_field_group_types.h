
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_GROUP_TYPES_H__
#define __DNX_FIELD_GROUP_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <include/soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <include/soc/dnx/dbal/dbal_structures.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_max_elk.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_actions_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>



#define DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT SAL_UINT8_NOF_BITS


#define DNX_FIELD_GROUP_EXTERNAL_PACKED_QUAL_INDEX_INVALID ((uint8)(-1))




typedef enum {
    
    DNX_FIELD_GROUP_ADD_FLAG_WITH_ID = 0x1,
    
    DNX_FIELD_GROUP_ADD_FLAG_EXTERNAL_COUNTERS = 0x2,
    
    DNX_FIELD_GROUP_ADD_FLAG_PACK_QUALIFIERS = 0x4,
    DNX_FIELD_GROUP_ADD_FLAG_NOF
} dnx_field_group_add_flags_e;


typedef enum {
    
    DNX_FIELD_GROUP_CONTEXT_ATTACH_FLAG_32_RESULT_MSB_ALIGN = 0x1,
    DNX_FIELD_GROUP_CONTEXT_ATTACH_FLAG_NOF
} dnx_field_group_context_attach_flags_e;


typedef enum {
    DNX_FIELD_GROUP_COMPARE_ID_FIRST = 0x0,
    DNX_FIELD_GROUP_COMPARE_ID_NONE = DNX_FIELD_GROUP_COMPARE_ID_FIRST,
    
    DNX_FIELD_GROUP_COMPARE_ID_CMP_1 = 0x1,
    
    DNX_FIELD_GROUP_COMPARE_ID_CMP_2 = 0x2,
    DNX_FIELD_GROUP_COMPARE_ID_NOF
} dnx_field_group_compare_id_e;


typedef enum {
    
    DNX_FIELD_ACE_FORMAT_ADD_FLAG_WITH_ID = 0x1,
    DNX_FIELD_ACE_FORMAT_ADD_FLAG_NOF
} dnx_field_ace_format_add_flags_e;




typedef struct {
    
    uint8 context_info_valid;
    
    dnx_field_key_id_t key_id;
    
    dnx_field_qual_attach_info_t qual_attach_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    
    uint16 nof_cascading_refs;
    
    uint32 payload_offset;
    
    uint32 payload_id;
    
    dnx_field_group_context_attach_flags_e flags;
} dnx_field_attach_context_info_t;


typedef struct {
    
    int auto_tcam_bank_select;
    
    int nof_tcam_banks;
    
    int* tcam_bank_ids;
} dnx_field_group_fg_tcam_sw_info_t;

typedef struct {
    char value[DBAL_MAX_STRING_LENGTH];
} field_group_name_t;


typedef struct {
    
    dnx_field_stage_e field_stage;
    
    dnx_field_group_type_e fg_type;
    
    field_group_name_t name;
    
    dnx_field_attach_context_info_t context_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
    
    dnx_field_key_template_t key_template;
    
    dnx_field_key_length_type_e key_length_type;
    
    dnx_field_action_length_type_e action_length_type;
    
    dnx_field_actions_fg_payload_sw_info_t actions_payload_info;
    
    dbal_tables_e dbal_table_id;
    
    dnx_field_group_add_flags_e flags;
    
    dnx_field_group_fg_tcam_sw_info_t tcam_info;
} dnx_field_group_fg_sw_info_t;

typedef struct {
    char value[DBAL_MAX_STRING_LENGTH];
} field_ace_format_name_t;


typedef struct {
    
    dnx_field_actions_ace_payload_sw_info_t actions_payload_info;
    
    dnx_field_ace_format_add_flags_e flags;
    
    field_ace_format_name_t name;
} dnx_field_ace_format_sw_info_t;


typedef struct {
    
    uint8 qual_idx[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT];
    
    dnx_field_group_t fg_id;
} dnx_field_group_kbp_segment_info_t;


typedef struct {
    
    dnx_field_group_kbp_segment_info_t segment_info[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    
    uint8 nof_fwd_segments;
} dnx_field_group_kbp_master_key_info_t;


typedef struct {
    
    dnx_field_group_kbp_master_key_info_t master_key_info;
    
    uint8 is_valid;
} dnx_field_group_kbp_info_t;


typedef struct {
    
    dnx_field_group_fg_sw_info_t* fg_info;
    
    dnx_field_ace_format_sw_info_t* ace_format_info;
    
    dnx_field_group_kbp_info_t kbp_info[DNX_KBP_MAX_NOF_OPCODES];
} dnx_field_group_sw_t;


#endif 
