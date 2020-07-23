
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TYPES_H__
#define __DNX_FIELD_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/bcm/field.h>
#include <include/soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <include/soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>



#define DNX_FIELD_MAX_FFC_PER_PASS_IN_UINT32 (((DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC+SAL_UINT32_NOF_BITS-1)/SAL_UINT32_NOF_BITS))


#define DNX_FIELD_MAX_NOF_PMF_PROGS_IN_UINT32 ((DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS+SAL_UINT32_NOF_BITS-1)/SAL_UINT32_NOF_BITS)


#define DNX_FIELD_MAX_NOF_PRESEL_ID_IN_UINT32 ((DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROGRAM_SELECTION_LINES+SAL_UINT32_NOF_BITS-1)/SAL_UINT32_NOF_BITS)


#define DNX_FIELD_KEY_ID_INVALID -1


#define DNX_FIELD_GROUP_INVALID (dnx_field_group_t)(-1)


#define DNX_FIELD_ACE_FORMAT_INVALID (dnx_field_ace_id_t)(-1)


#define DNX_FIELD_CONTEXT_ID_INVALID (dnx_field_context_t)(-1)


#define DNX_FIELD_QUAL_ID_INVALID 0


#define DNX_FIELD_QUAL_ID_FIRST 1


#define DNX_FIELD_ACTION_TYPE_INVALID ((dnx_field_action_type_t)(-1))


#define DNX_ACTION_CLASS_SHIFT 24


#define DNX_ACTION_STAGE_SHIFT 16


#define DNX_ACTION(dnx_class, dnx_stage, dnx_index) ((dnx_class << DNX_ACTION_CLASS_SHIFT) | (dnx_stage << DNX_ACTION_STAGE_SHIFT) | (dnx_index))


#define DNX_FIELD_ACTION_INVALID DNX_ACTION(DNX_FIELD_ACTION_CLASS_INVALID, DNX_FIELD_STAGE_INVALID, 127)


#define DNX_QUAL_CLASS_SHIFT 24


#define DNX_QUAL_STAGE_SHIFT 16


#define DNX_QUAL(dnx_class, dnx_stage, dnx_index) ((dnx_class << DNX_QUAL_CLASS_SHIFT) | (dnx_stage << DNX_QUAL_STAGE_SHIFT) | (dnx_index))


#define DNX_FIELD_QUAL_TYPE_INVALID DNX_QUAL(DNX_FIELD_QUAL_CLASS_INVALID, DNX_FIELD_STAGE_INVALID, DNX_FIELD_QUAL_ID_INVALID)


#define DNX_FIELD_TCAM_BANK_ID_INVALID (-1)


#define DNX_FIELD_TCAM_HANDLER_ID_INVALID 0x1023


#define DNX_FIELD_PRESEL_ID_INVALID -1


#define DNX_FIELD_DB_ID_INVALID -1


#define DNX_FIELD_FFC_ID_INVALID -1


#define DNX_FIELD_ENTRY_ACCESS_ID_INVALID -1


#define DNX_FIELD_TCAM_HANDLER_INVALID -1


#define DNX_FIELD_RANGE_ID_INVALID -1


#define DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID -1


#define DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID -1


#define DNX_FIELD_APP_DB_ID_INVALID ((dnx_field_app_db_id_t)(-1))


#define DNX_FIELD_FEM_ID_INVALID ((dnx_field_fem_id_t)(-1))


#define DNX_FIELD_EFES_ID_INVALID ((dnx_field_fes_id_t)(-1))


#define DNX_FIELD_APPTYPE_INVALID ((bcm_field_AppType_t)(-1))


#define DNX_FIELD_ACTION_PRIORITY_INVALID (~(1 << BCM_FIELD_ACTION_VALID_OFFSET))




typedef uint32 dnx_field_context_t;


typedef int dnx_field_group_t;


typedef int dnx_field_presel_t;


typedef uint32 dnx_field_action_priority_t;


typedef uint16 dnx_field_action_numerical_priority_t;


typedef uint16 dnx_field_action_position_t;


typedef uint8 dnx_field_action_array_t;


typedef uint8 dnx_field_fes_id_t;


typedef uint8 dnx_field_fes_pgm_id_t;


typedef uint8 dnx_field_fes_mask_id_t;


typedef uint32 dnx_field_fes_mask_t;


typedef uint8 dnx_field_fes_type_t;


typedef uint8 dnx_field_fes_polarity_t;


typedef uint8 dnx_field_fes_chosen_mask_t;


typedef uint8 dnx_field_fes_shift_t;


typedef uint8 dnx_field_fes_valid_bits_t;


typedef uint8 dnx_field_fes_key_select_t;


typedef uint32 dnx_field_qual_t;


typedef int dnx_field_qual_id_t;


typedef uint32 dnx_field_action_t;


typedef uint8 dnx_field_action_type_t;


typedef int dnx_field_action_id_t;


typedef uint32 dnx_field_app_db_id_t;


typedef uint32 dnx_field_ace_key_t;


typedef uint8 dnx_field_ace_id_t;


typedef uint8 dnx_field_fem_bit_index_t;


typedef uint8 dnx_field_fem_condition_t;


typedef uint8 dnx_field_fem_id_t;




typedef enum {
    
    DNX_FIELD_STAGE_INVALID = -1,
    DNX_FIELD_STAGE_FIRST = 0,
    
    DNX_FIELD_STAGE_IPMF1 = DNX_FIELD_STAGE_FIRST,
    
    DNX_FIELD_STAGE_IPMF2,
    
    DNX_FIELD_STAGE_IPMF3,
    
    DNX_FIELD_STAGE_EPMF,
    
    DNX_FIELD_STAGE_ACE,
    
    DNX_FIELD_STAGE_EXTERNAL,
    
    DNX_FIELD_STAGE_L4_OPS,
    
    DNX_FIELD_STAGE_NOF
} dnx_field_stage_e;


typedef enum {
    
    DNX_FIELD_GROUP_TYPE_INVALID = -1,
    DNX_FIELD_GROUP_TYPE_FIRST = 0,
    
    DNX_FIELD_GROUP_TYPE_TCAM = DNX_FIELD_GROUP_TYPE_FIRST,
    
    DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM,
    
    DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB,
    
    DNX_FIELD_GROUP_TYPE_EXEM,
    
    DNX_FIELD_GROUP_TYPE_STATE_TABLE,
    
    DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION,
    
    DNX_FIELD_GROUP_TYPE_CONST,
    
    DNX_FIELD_GROUP_TYPE_KBP,
    
    DNX_FIELD_GROUP_TYPE_NOF
} dnx_field_group_type_e;


typedef enum {
    DNX_FIELD_KEY_LENGTH_TYPE_INVALID = -1,
    
    DNX_FIELD_KEY_LENGTH_TYPE_DT = 0,
    
    DNX_FIELD_KEY_LENGTH_TYPE_HALF = 1,
    
    DNX_FIELD_KEY_LENGTH_TYPE_SINGLE = 2,
    
    DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE = 3,
    
    DNX_FIELD_KEY_LENGTH_TYPE_NONE = 4,
    DNX_FIELD_KEY_LENGTH_TYPE_NOF = 5
} dnx_field_key_length_type_e;


typedef enum {
    DNX_FIELD_ACTION_LENGTH_TYPE_INVALID = -1,
    
    DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF = 0,
    
    DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_SINGLE = 1,
    
    DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE = 2,
    
    DNX_FIELD_ACTION_LENGTH_TYPE_LEXEM = 3,
    
    DNX_FIELD_ACTION_LENGTH_TYPE_SEXEM = 4,
    
    DNX_FIELD_ACTION_LENGTH_TYPE_ACE = 5,
    
    DNX_FIELD_ACTION_LENGTH_TYPE_DIR_EXT = 6,
    
    DNX_FIELD_ACTION_LENGTH_TYPE_KBP = 7,
    
    DNX_FIELD_ACTION_LENGTH_TYPE_MDB_DT = 8,
    
    DNX_FIELD_ACTION_LENGTH_TYPE_NONE = 9,
    
    DNX_FIELD_ACTION_LENGTH_TYPE_STATE_TABLE = 10,
    DNX_FIELD_ACTION_LENGTH_TYPE_NOF = 11
} dnx_field_action_length_type_e;


typedef enum {
    
    DNX_FIELD_LAYER_INDEX_INVALID = -1,
    
    DNX_FIELD_LAYER_INDEX_0 = 0,
    DNX_FIELD_LAYER_INDEX_1 = 1,
    DNX_FIELD_LAYER_INDEX_2 = 2,
    DNX_FIELD_LAYER_INDEX_3 = 3,
    DNX_FIELD_LAYER_INDEX_4 = 4,
    DNX_FIELD_LAYER_INDEX_5 = 5,
    DNX_FIELD_LAYER_INDEX_6 = 6,
    DNX_FIELD_LAYER_INDEX_7 = 7,
    
    DNX_FIELD_LAYER_NOF = 8
} dnx_field_layer_index_e;


typedef enum {
    
    DNX_FIELD_INPUT_TYPE_INVALID = -1,
    
    DNX_FIELD_INPUT_TYPE_LAYER_FWD = 0,
    
    DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE = 1,
    
    DNX_FIELD_INPUT_TYPE_META_DATA = 2,
    
    DNX_FIELD_INPUT_TYPE_META_DATA2 = 3,
    
    DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD = 4,
    
    DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE = 5,
    
    DNX_FIELD_INPUT_TYPE_KBP = 6,
    
    DNX_FIELD_INPUT_TYPE_CASCADED = 7,
    
    DNX_FIELD_INPUT_TYPE_CONST = 8,
    
    DNX_FIELD_INPUT_TYPE_NOF = 9
} dnx_field_input_type_e;


typedef enum {
    DNX_FIELD_ACTION_CLASS_INVALID = 0,
    DNX_FIELD_ACTION_CLASS_FIRST = 1,
    
    DNX_FIELD_ACTION_CLASS_STATIC = DNX_FIELD_ACTION_CLASS_FIRST,
    
    DNX_FIELD_ACTION_CLASS_USER,
    
    DNX_FIELD_ACTION_CLASS_SW,
    
    DNX_FIELD_ACTION_CLASS_VW,
    
    DNX_FIELD_ACTION_CLASS_NOF
} dnx_field_action_class_e;


typedef enum {
    DNX_FIELD_QUAL_CLASS_INVALID = 0,
    DNX_FIELD_QUAL_CLASS_FIRST = 1,
    
    DNX_FIELD_QUAL_CLASS_META = DNX_FIELD_QUAL_CLASS_FIRST,
    
    DNX_FIELD_QUAL_CLASS_META2,
    
    DNX_FIELD_QUAL_CLASS_LAYER_RECORD,
    
    DNX_FIELD_QUAL_CLASS_USER,
    
    DNX_FIELD_QUAL_CLASS_HEADER,
    
    DNX_FIELD_QUAL_CLASS_SW,
    
    DNX_FIELD_QUAL_CLASS_VW,
    
    DNX_FIELD_QUAL_CLASS_NOF
} dnx_field_qual_class_e;




typedef struct {
    
    dnx_field_input_type_e input_type;
    
    int input_arg;
    
    int offset;
} dnx_field_qual_attach_info_t;


typedef struct {
    
    dnx_field_action_priority_t priority;
    
    uint8 valid_bit_polarity;
} dnx_field_action_attach_info_t;


#endif 
