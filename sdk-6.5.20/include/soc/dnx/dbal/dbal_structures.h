

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DBAL_STRUCTURES_H_INCLUDED
#  define DBAL_STRUCTURES_H_INCLUDED

#  ifndef BCM_DNX_SUPPORT
#    error "This file is for use by DNX (JR2) family only!"
#  endif



#ifdef DNX_DATA_INTERNAL
#undef DNX_DATA_INTERNAL
#endif


#include <soc/dnx/dbal/dbal_external_defines.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/mcm/allenum.h>

#include <soc/dnx/swstate/auto_generated/access/mdb_kaps_access.h>
#if defined(INCLUDE_KBP)
#include <soc/dnx/swstate/auto_generated/types/kbp_types.h>
#endif 



 


#  define DBAL_FIELD_MAX_NUM_OF_STRUCT_FIELDS                   160


#  define DBAL_MAX_NOF_ENTITY_LABEL_TYPES                       10


#  define DBAL_FIELD_MAX_NUM_OF_CONDITIONS                      5


#  define DBAL_MAX_NUMBER_OF_RESULT_TYPES                       25


#  define DBAL_PHYSICAL_KEY_SIZE_IN_BITS                        480


#  define DBAL_PHYSICAL_RES_SIZE_IN_BITS                        1600


#  define DBAL_SW_NOF_ENTRY_HANDLES                             30


#  define DBAL_NOF_SHARED_TABLE_MUTEXES                         10


#  define DBAL_PHYSICAL_KEY_SIZE_IN_BYTES               (BITS2BYTES(DBAL_PHYSICAL_KEY_SIZE_IN_BITS))


#  define DBAL_PHYSICAL_KEY_SIZE_IN_WORDS               (BITS2WORDS(DBAL_PHYSICAL_KEY_SIZE_IN_BITS))


#  define DBAL_PHYSICAL_RES_SIZE_IN_BYTES               (BITS2BYTES(DBAL_PHYSICAL_RES_SIZE_IN_BITS))


#  define DBAL_PHYSICAL_RES_SIZE_IN_WORDS               (BITS2WORDS(DBAL_PHYSICAL_RES_SIZE_IN_BITS))


#define DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS            (80)


#define DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_BYTES            (DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS*sizeof(uint32))


#  define DBAL_TABLE_DIRECT_MAX_KEY_NOF_BITS              32

#define DBAL_MAX_NUM_OF_CORES                           (dnx_data_device.general.nof_cores_get(unit))


#define DBAL_TABLE_BUFFER_IN_BYTES(table)               WORDS2BYTES(BITS2WORDS(table->max_payload_size))


#define DBAL_TABLE_IS_NONE_DIRECT(table)                 ((table->table_type != DBAL_TABLE_TYPE_DIRECT) && (table->table_type != DBAL_TABLE_TYPE_TCAM_DIRECT))
#define DBAL_TABLE_IS_TCAM(table)                        ((table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT) || (table->table_type == DBAL_TABLE_TYPE_TCAM) || (table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID))


#define DBAL_TABLE_IS_KEY_MASK_REQUIRED(table)           (DBAL_TABLE_IS_TCAM(table) || (table->table_type == DBAL_TABLE_TYPE_LPM) || (table->table_type == DBAL_TABLE_TYPE_EM))



#define DBAL_TABLE_IS_TCAM_BY_ID(table, is_commit)          ((table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT) || (table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID)) || (is_commit && (table->table_type == DBAL_TABLE_TYPE_TCAM) &&  (table->access_method == DBAL_ACCESS_METHOD_MDB))


#define DBAL_HANDLE_IS_MERGE_NEEDED(entry_handle)        (entry_handle->nof_result_fields != entry_handle->table->multi_res_info[entry_handle->cur_res_type].nof_result_fields)

#define DBAL_ACTIONS_IS_TCAM_BY_KEY(table) (table->table_type == DBAL_TABLE_TYPE_TCAM)


#define DBAL_RES_INFO            entry_handle->table->multi_res_info[entry_handle->cur_res_type]


#define ETM_PP_DESCRIPTOR_EXPANSION_NOF_STATIC_RES_TYPES 1



#define DBAL_HANDLE_TAKE_ALLOC 0
#define DBAL_HANDLE_TAKE_CLEAR 1



#define DBAL_FIELD_MAX_NUM_OF_ENUM_VALUES               512


#define DBAL_FIELD_MAX_NUM_OF_CHILD_FIELDS              20


#define DBAL_FIELD_MAX_NUM_OF_DEFINE_VALUES             40



#define DBAL_MAX_SHORT_STRING_LENGTH                    30


#define DBAL_USE_DEFAULT_SIZE                           (-2)


#define DBAL_SUPERSET_RESULT_TYPE                        DBAL_MAX_NUMBER_OF_RESULT_TYPES - 1




#  define DBAL_DB_MAX_NUM_OF_ILLEGAL_VALUES             10


# define DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS            14


#define DBAL_MAX_NOF_HW_VALUES_TO_RESULT_TYPE           8


#define DBAL_MAX_NUMBER_OF_HW_ELEMENTS                  100

#define DBAL_MAX_NUMBER_OF_HW_PORTING_ELEMENTS          50


#define DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE                 2


#define DBAL_NOF_APP_IDS_PER_PHYSICAL_TABLE             64


#define DBAL_APP_ID_BASIC_NOF_BITS                      6





#  define DBAL_PHY_DB_DEFAULT_INDEX                     0


#  define DBAL_INVALID_PARAM                            (-1)


#  define DBAL_RESULT_TYPE_NOT_INITIALIZED              (-1)


#  define DBAL_NOF_DYNAMIC_AND_STATIC_FIELDS            (DBAL_NOF_DYNAMIC_FIELDS + DBAL_NOF_FIELDS)


#  define DBAL_NOF_DYNAMIC_FIELDS                       (DNX_DATA_MAX_FIELD_QUAL_USER_NOF +\
                                                        DNX_DATA_MAX_FIELD_ACTION_USER_NOF)

#  define DBAL_NOF_DYNAMIC_TABLE_RESULT_FIELDS          (32)




#define DBAL_MAX_SIZE_FOR_GLOBAL_ZERO_BUFFER                                        \
    (UTILEX_MAX                                                                     \
    (UTILEX_MAX(DBAL_PHYSICAL_RES_SIZE_IN_WORDS, DBAL_PHYSICAL_RES_SIZE_IN_WORDS),  \
         DBAL_HL_MAX_MEMORY_LINE_SIZE_IN_WORDS))



#define DBAL_DB_INVALID                (-1)


#  define DBAL_PHYSICAL_KEY_HITBIT_NONE                   (0)

#  define DBAL_PHYSICAL_KEY_HITBIT_GET                    SAL_BIT(4)

#  define DBAL_PHYSICAL_KEY_HITBIT_CLEAR                  SAL_BIT(5)

#  define DBAL_PHYSICAL_KEY_HITBIT_ACTION                 (DBAL_PHYSICAL_KEY_HITBIT_GET | DBAL_PHYSICAL_KEY_HITBIT_CLEAR)

#  define DBAL_PHYSICAL_KEY_HITBIT_PRIMARY                SAL_BIT(6)

#  define DBAL_PHYSICAL_KEY_HITBIT_SECONDARY              SAL_BIT(7)


#  define DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A     SAL_BIT(0)

#  define DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B     SAL_BIT(1)

#  define DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED     (DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A | DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B)


#  define DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_A   SAL_BIT(2)

#  define DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_B   SAL_BIT(3)

#  define DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED     (DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_A | DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_B)

#  define DBAL_PHYSICAL_KEY_PRIMARY_AND_SECONDARY_HITBIT_ACCESSED     (DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A |   \
                                                                       DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B |   \
                                                                       DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_A | \
                                                                       DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_B)


#  define DBAL_NOF_HITBIT_COMBINATIONS (16)
#  define DBAL_NOF_FEC_HITBIT_COMBINATIONS (16)
#  define DBAL_NOF_KAPS_HITBIT_COMBINATIONS (4)
#  define DBAL_NOF_LEM_HITBIT_COMBINATIONS (4)
#  define DBAL_NOF_TCAM_HITBIT_COMBINATIONS (SAL_BIT(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES))

#  define DBAL_HITBIT_NOT_SET_RULE (0)
#  define DBAL_HITBIT_SET_RULE     (1)



#  define DBAL_PHYSICAL_KEY_AGE_NONE                   (0)

#  define DBAL_PHYSICAL_KEY_AGE_GET                    SAL_BIT(1)


#define DBAL_OPTIMIZED_ITERATOR_DIVIDE_32_SHIFT 5
#define DBAL_OPTIMIZED_ITERATOR_MOD_32_MASK     0x1F
#define DBAL_OPTIMIZED_ITERATOR_GET_WORD_INDEX(_key) (_key >> DBAL_OPTIMIZED_ITERATOR_DIVIDE_32_SHIFT)
#define DBAL_OPTIMIZED_ITERATOR_GET_BIT_OFFSET(_key) (_key & DBAL_OPTIMIZED_ITERATOR_MOD_32_MASK)


#define DBAL_FIELD_IND_IS_PARENT_FIELD                  0
#define DBAL_FIELD_IND_IS_FIELD_ENCODED                 1
#define DBAL_FIELD_IND_IS_CONST_VALID                   2
#define DBAL_FIELD_IND_IS_INSTANCE_REVERSE              3
#define DBAL_FIELD_IND_IS_VALID_INDICATION              4
#define DBAL_FIELD_IND_IS_FIELD_ADDRESS                 5 
#define DBAL_FIELD_IND_IS_FIELD_ENUM                    6
#define DBAL_FIELD_IND_IS_ALLOCATOR                     7
#define DBAL_FIELD_IND_IS_RANGED                        8
#define DBAL_FIELD_IND_IS_DYNAMIC                       9
#define DBAL_FIELD_IND_IS_DEFAULT_VALUE_VALID           10

#define DBAL_FIELD_IND_IS_COMMON                        11

#define DBAL_FIELD_IND_IS_DEFAULT                       12
#define DBAL_FIELD_IND_IS_STANDARD_1                    13

#define DBAL_FIELD_IND_IS_PACKED                        14

#define DBAL_FIELD_IND_IS_VALID                         15
#define DBAL_NOF_FIELD_IND                             (16)





#define DBAL_COLLECTION_POOL_SIZE                   dnx_data_dbal.db_init.dbal_collection_pool_size_get(unit)


#define DBAL_NOF_COLLECTION_POOLS                   2


#define DBAL_SW_NOF_ENTRY_HANDLES_FOR_COLLECTION    DBAL_COLLECTION_POOL_SIZE*DBAL_NOF_COLLECTION_POOLS




typedef enum {
    DBAL_TABLE_IND_IS_HITBIT_EN,                        
    DBAL_TABLE_IND_IS_HOOK_ACTIVE,                      

    DBAL_TABLE_IND_HAS_RESULT_TYPE,                     
    DBAL_TABLE_IND_RANGE_SET_SUPPORTED,                 
    DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW,            
    DBAL_TABLE_IND_IS_PRIORITY_SUPPORTED,               
    DBAL_TABLE_IND_IS_LEARNING_EN,                      
    DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED,            
    DBAL_TABLE_IND_IS_IMAGE_SPECIFIC_TABLE,             
    DBAL_TABLE_IND_IS_TABLE_DIRTY,                      
    DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED,               
    DBAL_TABLE_IND_IS_DEFAULT_NON_STANDARD,             
    DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED,           
    DBAL_TABLE_IND_NO_VALIDATIONS,                       

    DBAL_TABLE_IND_COLLECT_MODE,                        
    DBAL_NOF_TABLE_IND
} dbal_table_indications_e;



typedef enum
{
    DBAL_FORMULA_OPERATION_ADD,
    DBAL_FORMULA_OPERATION_SUBTRACT,
    DBAL_FORMULA_OPERATION_MULTIPLY,
    DBAL_FORMULA_OPERATION_DIVIDE,
    DBAL_FORMULA_OPERATION_MODULO,
    
    DBAL_FORMULA_OPERATION_F_LOG2, 
    DBAL_NOF_FORMULA_OPERATION
} dbal_formula_operations_e;


typedef enum
{
    DBAL_ACTION_ACCESS_TYPE_SW_ONLY,
    DBAL_ACTION_ACCESS_TYPE_READ,
    DBAL_ACTION_ACCESS_TYPE_WRITE,

    DBAL_NOF_ACTION_ACCESS_TYPES
} dbal_action_access_type_e;



typedef enum
{
    
    DBAL_KEY_IS_VALID,
    
    DBAL_KEY_IS_OUT_OF_ITERATOR_RULE,
    
    DBAL_KEY_IS_INVALID
} dbal_key_value_validity_e;



typedef enum
{
    DBAL_ACTION_ACCESS_FUNC_COMMIT_CONSEC,
    DBAL_ACTION_ACCESS_FUNC_CLEAR_CONSEC,
    DBAL_ACTION_ACCESS_FUNC_RESULT_TYPE_RES,
    DBAL_ACTION_ACCESS_FUNC_ENTRY_COMMIT,
    DBAL_ACTION_ACCESS_FUNC_ENTRY_GET,
    DBAL_ACTION_ACCESS_FUNC_ENTRY_CLEAR,
    DBAL_ACTION_ACCESS_FUNC_TABLE_CLEAR,
    DBAL_ACTION_ACCESS_FUNC_ITERATOR_INIT,
    DBAL_ACTION_ACCESS_FUNC_ITERATOR_DESTROY,
    DBAL_ACTION_ACCESS_FUNC_ITERATOR_GET_NEXT,
    DBAL_ACTION_ACCESS_FUNC_ITERATOR_ACCESS_ID_GET,
    
    DBAL_ACTION_ACCESS_FUNC_ITERATOR_FLUSH_MACHINE_GET,
    DBAL_NOF_ACTION_ACCESS_FUNCS
} dbal_action_access_func_e;


#define DBAL_ENTRY_ATTR_PRIORITY         SAL_BIT(0)
#define DBAL_ENTRY_ATTR_AGE              SAL_BIT(1)
#define DBAL_ENTRY_ATTR_HIT_GET          SAL_BIT(2)   
#define DBAL_ENTRY_ATTR_HIT_CLEAR        SAL_BIT(3)
#define DBAL_ENTRY_ATTR_HIT_PRIMARY      SAL_BIT(4)
#define DBAL_ENTRY_ATTR_HIT_SECONDARY    SAL_BIT(5)
#define DBAL_NOF_ENTRY_ATTR_TYPE         (6)


typedef enum
{
    DBAL_ENTRY_ATTR_RES_PRIORITY,
    DBAL_ENTRY_ATTR_RES_AGE,
    DBAL_ENTRY_ATTR_RES_HIT,
    DBAL_NOF_ENTRY_ATTR_RES_TYPE
} dbal_entry_attr_res_type_e;


typedef enum
{
    DBAL_BULK_MODULE_NONE,
    DBAL_BULK_MODULE_MDB_LEM_FLUSH,
    DBAL_BULK_MODULE_MDB_LEM_NON_BLOCKING_FLUSH,
    DBAL_BULK_MODULE_MDB_KAPS,
    DBAL_BULK_MODULE_KBP_FWD,
    DBAL_BULK_MODULE_KBP_ACL,

    DBAL_NOF_BULK_MODULE
} dbal_bulk_module_e;


typedef enum
{
    DBAL_STATUS_NOT_INITIALIZED,
    DBAL_STATUS_DBAL_INIT_DONE,
    DBAL_STATUS_DEVICE_INIT_DONE,

    DBAL_NOF_STATUS
} dbal_status_e;



typedef enum dnx_dbal_logger_type_t
{
    DNX_DBAL_LOGGER_TYPE_API,
    DNX_DBAL_LOGGER_TYPE_ACCESS,
    DNX_DBAL_LOGGER_TYPE_DIAG,
    DNX_DBAL_LOGGER_TYPE_LAST
} dnx_dbal_logger_type_e;


typedef enum
{
    
    DBAL_STAGE_NONE,
    DBAL_STAGE_PRT,
    DBAL_STAGE_LLR,
    DBAL_STAGE_VT1,
    DBAL_STAGE_VT2,
    DBAL_STAGE_VT3,
    DBAL_STAGE_VT4,
    DBAL_STAGE_VT5,
    DBAL_STAGE_FWD1,
    DBAL_STAGE_FWD2,
    DBAL_STAGE_IPMF1,
    DBAL_STAGE_IPMF2,
    DBAL_STAGE_IPMF3,
    DBAL_STAGE_EPMF,
    DBAL_NOF_STAGES
} dbal_stage_e;


typedef enum {
    DBAL_INDIRECT_COMMIT_MODE_NORMAL,
    DBAL_INDIRECT_COMMIT_MODE_UPDATE,
    DBAL_INDIRECT_COMMIT_MODE_FORCE,
    DBAL_NOF_INDIRECT_COMMIT_MODE
} dbal_indirect_commit_mode_e;

typedef struct
{
    dbal_stage_e stage;

    
    int line_length_key_only;
    int line_length_with_key_size_prefix;
    int supports_half_entries;

    
    soc_mem_t cmd_memory;
    soc_field_t cmd_write_field;
    soc_field_t cmd_read_field;
    soc_field_t cmd_compare_field;
    soc_field_t cmd_valid_field;
    soc_field_t cmd_key_field;
    soc_field_t cmd_mask_field;

    
    soc_mem_t reply_memory;
    soc_field_t reply_valid_field;
    soc_field_t reply_data_out_field;

    
    soc_reg_t ecc_fix_en_reg;
} dbal_tcam_cs_stage_info_t;


typedef enum
{
    
    DBAL_FIELD_PRINT_TYPE_NONE,

    
    DBAL_FIELD_PRINT_TYPE_BOOL,

    
    DBAL_FIELD_PRINT_TYPE_UINT32,

    
    DBAL_FIELD_PRINT_TYPE_IPV4,

    
    DBAL_FIELD_PRINT_TYPE_IPV6,

    
    DBAL_FIELD_PRINT_TYPE_MAC,

    
    DBAL_FIELD_PRINT_TYPE_ARRAY8,

    
    DBAL_FIELD_PRINT_TYPE_ARRAY32,

    
    DBAL_FIELD_PRINT_TYPE_BITMAP,

    
    DBAL_FIELD_PRINT_TYPE_ENUM,

    
    DBAL_FIELD_PRINT_TYPE_SYSTEM_CORE,

    
    DBAL_FIELD_PRINT_TYPE_STRUCTURE,

    
    DBAL_FIELD_PRINT_TYPE_HEX,

    
    DBAL_FIELD_PRINT_TYPE_DBAL_TABLE,

    
    DBAL_FIELD_PRINT_TYPE_STR,

    DBAL_NOF_FIELD_PRINT_TYPES
} dbal_field_print_type_e;


typedef enum
{
    
    DBAL_POINTER_TYPE_NONE,

    
    DBAL_POINTER_TYPE_UINT8,

    
    DBAL_POINTER_TYPE_UINT16,

    
    DBAL_POINTER_TYPE_UINT32,

    
    DBAL_POINTER_TYPE_ARR_UINT8,

    
    DBAL_POINTER_TYPE_ARR_UINT32,

    
    DBAL_POINTER_TYPE_UINT64,

    DBAL_NOF_POINTER_TYPES
} dbal_pointer_type_e;


typedef enum
{
    
    DBAL_HANDLE_STATUS_AVAILABLE,
	
    
    DBAL_HANDLE_STATUS_IN_USE,
	
    
    DBAL_HANDLE_STATUS_ACTION_COLLECTED,
	
	
    DBAL_HANDLE_STATUS_ACTION_PREFORMED,

    DBAL_NOF_ENTRY_HANDLE_STATUSES
} dbal_entry_handle_status_e;


typedef enum
{
    DBAL_HL_ENTITY_FIELD,
    DBAL_HL_ENTITY_REGISTER,
    DBAL_HL_ENTITY_MEMORY,

    DBAL_NOF_HL_ENTITIES
} dbal_hl_entity_type_e;


typedef enum
{
    
    DBAL_VALUE_FIELD_ENCODE_NONE,

    
    DBAL_VALUE_FIELD_ENCODE_PREFIX,

    
    DBAL_VALUE_FIELD_ENCODE_SUFFIX,

    
    DBAL_VALUE_FIELD_ENCODE_SUBTRACT,

    
    DBAL_VALUE_FIELD_ENCODE_ADD,

    
    DBAL_VALUE_FIELD_ENCODE_MULTIPLY,

    
    DBAL_VALUE_FIELD_ENCODE_DIVIDE,

    
    DBAL_VALUE_FIELD_ENCODE_MODULO,

    
    DBAL_VALUE_FIELD_ENCODE_HARD_VALUE,

    
    DBAL_VALUE_FIELD_ENCODE_BITWISE_NOT,

    
    DBAL_VALUE_FIELD_ENCODE_VALID_IND,

    

    
    DBAL_VALUE_FIELD_ENCODE_ENUM,

    
    DBAL_VALUE_FIELD_ENCODE_STRUCTURE,

    DBAL_NOF_VALUE_FIELD_ENCODE_TYPES
} dbal_value_field_encode_types_e;


typedef enum
{
    
    DBAL_HL_ACCESS_MEMORY,

    
    DBAL_HL_ACCESS_REGISTER,

    
    DBAL_HL_ACCESS_SW,

    DBAL_NOF_HL_ACCESS_TYPES
} dbal_hard_logic_access_types_e;


typedef enum
{
    
    DBAL_CORE_NONE,

    
    DBAL_CORE_MODE_DPC,

    
    DBAL_CORE_MODE_SBC,

    DBAL_NOF_CORE_MODE_TYPES
} dbal_core_mode_e;


#  define DBAL_CORE_ALL           (-17) 


#  define DBAL_CORE_DEFAULT        (0) 


#  define DBAL_CORE_NOT_INTIATED   (-1) 


#define DBAL_CORE_SIZE_IN_BITS     (1)  


typedef enum
{
    
    DBAL_MATURITY_LOW,

    
    DBAL_MATURITY_PARTIALLY_FUNCTIONAL,

    
    DBAL_MATURITY_HIGH,

    
    DBAL_MATURITY_HIGH_SKIP_ADAPTER,

    
    DBAL_MATURITY_HIGH_SKIP_DEVICE,

    DBAL_NOF_MATURITY_LEVELS
} dbal_maturity_level_e;



typedef enum
{
    DBAL_ACTION_ENTRY_COMMIT,
    DBAL_ACTION_ENTRY_COMMIT_UPDATE,
    DBAL_ACTION_ENTRY_COMMIT_FORCE,
    DBAL_ACTION_ENTRY_COMMIT_RANGE,
    DBAL_ACTION_ENTRY_GET,
    DBAL_ACTION_ENTRY_GET_ALL,
    DBAL_ACTION_ENTRY_GET_ACCESS_ID,
    DBAL_ACTION_ITERATOR_GET,
    DBAL_ACTION_ENTRY_CLEAR,
    DBAL_ACTION_ENTRY_CLEAR_RANGE,  
    DBAL_ACTION_TABLE_CLEAR,
    DBAL_ACTION_SKIP, 

    DBAL_NOF_ACTIONS
} dbal_actions_e;



typedef enum
{
    DBAL_MDB_ACTION_APPLY_ALL = 0,
    DBAL_MDB_ACTION_APPLY_NONE = 1,
    DBAL_MDB_ACTION_APPLY_HW_ONLY = 2,
    DBAL_MDB_ACTION_APPLY_SW_SHADOW = 3,
    DBAL_NOF_MDB_ACTION_APPLY_TYPES
} dbal_mdb_action_apply_type_e;


typedef struct
{
    dbal_fields_e field_id;
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
} dbal_field_data_t;


typedef shr_error_e (
    *dnx_dbal_formula_offset_cb) (
    int unit,
    void *entry_handle,
    dbal_fields_e current_mapped_field_id,
    uint32 *offset);


typedef shr_error_e (
    *dnx_dbal_field_type_illegal_value_cb) (
    int unit,
    uint32 value,
    uint8 *is_illegal);


typedef struct dbal_offset_formula_t
{
    char as_string[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];

    int nof_vars;
    uint8 key_val;
    uint8 result_size;
    uint8 instance_index;
    uint32 val;
    dbal_fields_e field_id;
    int field_pos_in_table;
    int is_result;
    dbal_formula_operations_e action[DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1];
    struct dbal_offset_formula_t *val_as_formula;
    dnx_dbal_formula_offset_cb formula_offset_cb;
} dbal_offset_formula_t;


typedef struct dbal_int_or_cb_s
{
    uint32 val;
    dnx_dbal_formula_offset_cb cb;
} dbal_int_or_cb_t;


typedef struct
{
    uint32 internal_inparam;
    dbal_int_or_cb_t *formula;
} dbal_offset_encode_info_t;


typedef struct
{
    dbal_value_field_encode_types_e encode_mode;
    uint32 input_param;
} dbal_field_type_encode_info_t;


typedef struct
{
    dbal_fields_e sub_field_id;
    dbal_field_type_encode_info_t encode_info;
} dbal_sub_field_info_t;


typedef struct
{
    dbal_fields_e struct_field_id;
    int offset;
    int length;
    
    uint32 arr_prefix;

    
    uint8 arr_prefix_size;

    
    int arr_prefix_table_entry_width;

     
    int arr_prefix_table_entry_value;

} dbal_sub_struct_field_info_t;


typedef struct
{
    uint32 value;
    int is_invalid;
    char name[2*DBAL_MAX_STRING_LENGTH];
} dbal_enum_decoding_info_t;


typedef struct
{
    uint32 value;
    char name[DBAL_MAX_STRING_LENGTH];
} dbal_defiens_info_t;


typedef struct
{
    dbal_condition_types_e type;
    uint32 value[UTILEX_MAX(DBAL_PHYSICAL_RES_SIZE_IN_WORDS, DBAL_PHYSICAL_KEY_SIZE_IN_WORDS)];
    uint32 mask[UTILEX_MAX(DBAL_PHYSICAL_RES_SIZE_IN_WORDS, DBAL_PHYSICAL_KEY_SIZE_IN_WORDS)];
    dbal_fields_e field_id;
    int field_pos_in_table; 
} dbal_access_condition_info_t;


typedef struct
{
    dbal_iterator_action_types_e action_type;
    uint32 value[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    dbal_fields_e field_id;
} dbal_iterator_actions_info_t;


typedef struct
{
    
    char name[DBAL_MAX_STRING_LENGTH];

    
    uint32 max_size;

    
    dbal_field_print_type_e print_type;

    
    uint32 field_indication_bm[1];
    
    
    uint32 default_value;

    
    uint32 max_value;

    
    uint32 min_value;

    
    uint32 const_value;

    
    int nof_illegal_values;
    uint32 illegal_values[DBAL_DB_MAX_NUM_OF_ILLEGAL_VALUES];

    
    int nof_child_fields;
    dbal_sub_field_info_t *sub_field_info;

    
    int nof_struct_fields;
    dbal_sub_struct_field_info_t *struct_field_info;

    
    int nof_enum_values;
    int nof_invalid_enum_values;
    dbal_enum_decoding_info_t *enum_val_info;

    
    dnx_dbal_field_type_illegal_value_cb illegal_value_cb;

    
    int nof_defines_values;
    dbal_defiens_info_t *defines_info;

    
    dbal_field_types_defs_e reference_field_id;

    
    dbal_field_type_encode_info_t encode_info;
} dbal_field_types_basic_info_t;


typedef enum
{
    DBAL_HL_TCAM_ACCESS_TYPE_KEY,
    DBAL_HL_TCAM_ACCESS_TYPE_KEY_MASK,
    DBAL_HL_TCAM_ACCESS_TYPE_RESULT,
    DBAL_NOF_HL_TCAM_ACCESS_TYPES
} dbal_hl_tcam_access_type_e;


typedef enum
{
    
    DBAL_RESULT_TYPE_UPDATE_MODE_STRICT,
    
    DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE,
    
    DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE_DEFAULT,
    DBAL_NOF_RESULT_TYPE_UPDATE_MODES
} dbal_result_type_update_mode_e;


typedef struct
{
    dbal_fields_e field_id;

        
    dbal_hl_tcam_access_type_e hl_tcam_access_type;

    
    uint32 access_nof_bits;
    uint32 access_offset;

    

    dbal_field_type_encode_info_t encode_info;

    
    int field_pos_in_interface;

    
    uint32 nof_bits_in_interface;

    
    uint32 offset_in_interface;

    
    soc_mem_t *memory;
    soc_reg_t *reg;

    
    int is_default_non_standard;

    soc_field_t hw_field;
 
    
    dbal_hw_entity_group_e hw_entity_group_id; 


    
    dbal_offset_encode_info_t array_offset_info;

    
    dbal_offset_encode_info_t entry_offset_info;

    
    dbal_offset_encode_info_t data_offset_info;

    
    dbal_offset_encode_info_t block_index_info;

        
    dbal_offset_encode_info_t group_offset_info;

    
    soc_mem_t alias_memory;

    
    soc_reg_t alias_reg;

    
    dbal_offset_encode_info_t alias_data_offset_info;

    int nof_conditions;

    
    dbal_access_condition_info_t *mapping_condition;

} dbal_hl_l2p_field_info_t;

typedef struct
{
    int num_of_access_fields;
    uint8 is_packed_fields;
    dbal_hl_l2p_field_info_t *l2p_fields_info; 
} dbal_hl_l2p_info_t;

typedef struct
{
    soc_mem_t memory;
} dbal_cs_access_info_t;

typedef struct
{
    
    int sw_payload_length_bytes;

    
    char *table_size_str;

} dbal_sw_access_info_t;

typedef struct
{
    
    int is_default_non_standard;

    dbal_hl_l2p_info_t l2p_hl_info[DBAL_NOF_HL_ACCESS_TYPES];
} dbal_hl_access_info_t;


#define DBAL_PEMLA_FIELD_MAPPING_INVALID (0xFFFFFFFF)

typedef struct
{
    uint32 *key_fields_mapping;
    uint32 *result_fields_mapping;
    uint32 reg_mem_id;
    
} dbal_pemla_db_mapping_info_t;

typedef struct
{
    
    int entry_hw_id;

    
    dbal_indirect_commit_mode_e indirect_commit_mode;

    
    uint8 hitbit;

    
    uint32 age;

    dbal_mdb_action_apply_type_e mdb_action_apply;

    
    uint32 prefix_length;

    uint32 key_size;
    uint32 payload_size;
    
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    uint32 k_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];

    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 p_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
} dbal_physical_entry_t;

typedef struct
{
    uint32 app_id;              
    uint32 mdb_entry_index;     
    int mdb_cluster_index;      
    uint32 payload_basic_size;  

    
    uint8 continue_calling_mdb_get_next;

    

    
    uint8 has_rt_rule;

    
    uint8 iterate_in_flush_machine;
    
    int nof_flush_rules;
    
    uint32 result_types_in_flush_bitmap;

    
    uint8 start_non_flush_iteration;
    
    int nof_non_flush_rules;
    
    uint32 result_types_in_non_flush_bitmap;
    
    uint8 hit_bit_flags;
    
    uint8 age_flags;
#if defined(INCLUDE_KBP)
    kbp_db_t_p kbp_lpm_db_p;       
    kbp_ad_db_t_p kbp_lpm_ad_db_p; 
    struct kbp_entry_iter *kbp_lpm_iter;        
#endif                          

    mdb_kaps_db_t_p mdb_lpm_db_p;       
    mdb_kaps_ad_db_t_p mdb_lpm_ad_db_p; 
    struct kaps_entry_iter *mdb_lpm_iter;        

    SW_STATE_HASH_TABLE_ITER mdb_em_htb_iter;     
    SW_STATE_HASH_TABLE_ITER mdb_em_key_size;
    
    SW_STATE_HASH_TABLE_ITER mdb_em_payload_size;

    uint32 mdb_em_ratios; 
    uint32 mdb_em_way_index; 
    uint32 mdb_em_emc_bank_enable; 
    uint32 mdb_em_emp_bank_enable; 

    dbal_mdb_action_apply_type_e mdb_action_apply;

} dbal_physical_entry_iterator_t;

typedef struct
{
    SW_STATE_HASH_TABLE_PTR hash_table_id;
    SW_STATE_HASH_TABLE_ITER hash_entry_index;

} dbal_sw_table_iterator_t;

typedef struct
{
    dbal_pointer_type_e pointer_type;
    void *value_returned_pointer;
    void *mask_returned_pointer;
} dbal_user_output_info_t;


typedef struct
{
    int result_type_nof_hw_values;

    uint32 result_type_hw_value[DBAL_MAX_NOF_HW_VALUES_TO_RESULT_TYPE];
    
    char result_type_name[DBAL_MAX_STRING_LENGTH];

    
    int entry_payload_size;

    
    int nof_result_fields;

    
    dbal_table_field_info_t *results_info;

    
    dbal_field_types_defs_e reference_field_id;

    uint8 is_disabled;

    
    uint8 has_link_list;
} multi_res_info_t;


typedef struct
{
    
    char name[DBAL_MAX_SHORT_STRING_LENGTH];

    
    int is_pre_acces_prints;

    
    uint32 action_validation_flags;

    
    uint32 supported_flags;
} dbal_action_info_t;

typedef struct
{
    int nof_physical_tables;
    dbal_physical_tables_e physical_db_id[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE];
    uint32 tcam_handler_id;
    uint32 app_id;
    int app_id_size;

} dbal_table_mdb_access_info_t;

typedef struct
{
    int sw_state_nof_entries;
    int sw_payload_length_bytes;

} dbal_table_sw_access_info_t;


typedef struct
{
    
    char table_name[DBAL_MAX_STRING_LENGTH];
    dbal_maturity_level_e maturity_level;
    int nof_labels;
    dbal_labels_e *table_labels;
    dbal_table_type_e table_type;

    
    int max_capacity;

    
    uint32 indications_bm[1];

    
    int nof_key_fields;
    dbal_table_field_info_t *keys_info;

    
    uint32 key_size;

    
    uint32 core_id_nof_bits;

    
    dbal_fields_e allocator_field_id;

    
    uint32 **iterator_optimized;

    
    int nof_result_types;
    multi_res_info_t *multi_res_info;

    int max_payload_size;
    int max_nof_result_fields;

    dbal_core_mode_e core_mode;
    dbal_access_method_e access_method; 

    
    uint8 mutex_id;

    

    
    dbal_sw_access_info_t sw_access_info;
    
    
    int nof_physical_tables;
    dbal_physical_tables_e physical_db_id[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE];

    uint32 app_id; 
    int app_id_size;

    
    dbal_stage_e dbal_stage;

    
    dbal_hl_access_info_t *hl_mapping_multi_res;

    
    dbal_cs_access_info_t *tcam_cs_mapping;

    
    dbal_pemla_db_mapping_info_t pemla_mapping;

    
    void *kbp_handles;

} dbal_logical_table_t;


typedef struct
{
    char table_name[DBAL_MAX_STRING_LENGTH];
    char table_status[DBAL_MAX_SHORT_STRING_LENGTH];
    char maturity_level[DBAL_MAX_SHORT_STRING_LENGTH];
    char labels[DBAL_MAX_NOF_ENTITY_LABEL_TYPES * DBAL_MAX_SHORT_STRING_LENGTH];
    char table_type[DBAL_MAX_SHORT_STRING_LENGTH];
    char core_mode[DBAL_MAX_SHORT_STRING_LENGTH];
    char access_method[DBAL_MAX_SHORT_STRING_LENGTH];
    char range_set_supported[DBAL_MAX_SHORT_STRING_LENGTH];

} dbal_table_string_t;


typedef struct
{
    
    int error_exists;
    
    dbal_fields_e field_id;
} field_error_info_t;


typedef struct
{
    
    uint8 hit_bit_rule_valid;

    
    uint8 hit_bit_rule_is_hit;

    
    uint8 hit_bit_rule_type;

    
    uint8 hit_bit_action_get;

    
    uint8 hit_bit_action_get_type;

    
    uint8 hit_bit_action_clear;

    
    uint8 hit_bit_action_clear_type;

    
    uint8 age_rule_valid;

    
    uint32 age_rule_entry_value;

    
    uint32 age_rule_compare_value;

}dbal_iterator_attribute_info_t;

typedef struct
{
    
    uint8 is_init;

    
    uint8 is_end;

    
    uint8 physical_db_index;

    
    uint8 used_first_key;

    
    uint32 entries_counter;

    
    uint32 nof_key_rules;

    
    dbal_access_condition_info_t *key_rules_info;

    
    uint32 nof_val_rules;

    
    dbal_access_condition_info_t *val_rules_info;

    
    dbal_iterator_attribute_info_t attrib_info;

    
    uint32 nof_actions;

    
    uint32 actions_bitmap;

    
    dbal_iterator_actions_info_t *actions_info;

    
    uint32 max_num_of_iterations;

    
    dbal_sw_table_iterator_t sw_iterator;

    
    dbal_physical_entry_iterator_t mdb_iterator;

    dbal_iterator_mode_e mode;

} dbal_iterator_info_t;

typedef struct
{
    int handle_id;

    dbal_tables_e table_id;

    
    dbal_logical_table_t *table;

    
    dbal_fields_e key_field_ids[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS];

    
    uint32 key_field_ranges[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS];

    
    dbal_fields_e value_field_ids[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE];

    uint8 nof_key_fields;

    uint8 nof_ranged_fields;
    uint8 nof_result_fields;
    uint8 entry_merged;

    
    dbal_entry_handle_status_e handle_status;

    
    int core_id;

    
    int cur_res_type;

    
    dbal_user_output_info_t *user_output_info;

    
    uint32 *attribute_info[DBAL_NOF_ENTRY_ATTR_RES_TYPE];

    
    field_error_info_t error_info;

    
    uint8 overrun_entry;

    
    uint8 get_all_fields;

    
    uint8 access_id_set;

    
    uint8 er_flags;

    
    dbal_physical_entry_t phy_entry;

} dbal_entry_handle_t;

typedef struct
{
    char name[DBAL_MAX_STRING_LENGTH];
    uint32 val;
    uint32 min_bit;
    uint32 max_bit;
    uint32 len;
} dbal_sub_field_signal_info_t;







typedef enum {
    DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS,       
    
    DBAL_NOF_DBAL_MDB_ENTRY_FLAGS
} dbal_mdb_entry_flags;


typedef shr_error_e(
    *PHYSICAL_TABLE_ENTRY_ADD) (
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

typedef shr_error_e(
    *PHYSICAL_TABLE_ENTRY_GET) (
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

typedef shr_error_e(
    *PHYSICAL_TABLE_ENTRY_DELETE) (
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

typedef shr_error_e(
    *PHYSICAL_TABLE_CLEAR) (
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id);

typedef shr_error_e(
    *PHYSICAL_TABLE_DEFAULT_VALUES_SET) (
    int unit);

typedef shr_error_e(
    *PHYSICAL_TABLE_INIT) (
    int unit);

typedef shr_error_e(
    *PHYSICAL_TABLE_DEINIT) (
    int unit);

typedef shr_error_e(
    *PHYSICAL_TABLE_ITERATOR_INIT) (
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

typedef shr_error_e(
    *PHYSICAL_TABLE_ITERATOR_GET_NEXT) (
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end);

typedef shr_error_e(
    *PHYSICAL_TABLE_ITERATOR_DEINIT) (
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

typedef struct
{
    PHYSICAL_TABLE_ENTRY_ADD entry_add;
    PHYSICAL_TABLE_ENTRY_GET entry_get;
    PHYSICAL_TABLE_ENTRY_DELETE entry_delete;
    PHYSICAL_TABLE_CLEAR table_clear;
    PHYSICAL_TABLE_DEFAULT_VALUES_SET table_default_values_set;
    PHYSICAL_TABLE_INIT table_init;
    PHYSICAL_TABLE_DEINIT table_deinit;
    PHYSICAL_TABLE_ITERATOR_INIT iterator_init;
    PHYSICAL_TABLE_ITERATOR_GET_NEXT iterator_get_next;
    PHYSICAL_TABLE_ITERATOR_DEINIT iterator_deinit;

} dbal_physical_table_actions_t;

typedef struct
{
    char physical_name[DBAL_MAX_STRING_LENGTH];
    dbal_physical_tables_e physical_db_type;
    dbal_core_mode_e physical_core_mode;
    dbal_physical_table_actions_t *table_actions;
} dbal_physical_table_def_t;

shr_error_e dbal_physical_table_get(
    int unit,
    dbal_physical_tables_e physical_table_id,
    dbal_physical_table_def_t ** physical_table);

const char *dbal_physical_table_name_get(
    int unit,
    dbal_physical_tables_e physical_table_id);

typedef struct
{
    dbal_physical_table_def_t physical_tables[DBAL_NOF_PHYSICAL_TABLES];

} dbal_physical_mngr_info_t;

 

typedef struct
{
    uint32 nof_memories;
    soc_mem_t memory_ids[DBAL_MAX_NUMBER_OF_HW_PORTING_ELEMENTS];
    soc_mem_t memory_mapped_ids[DBAL_MAX_NUMBER_OF_HW_PORTING_ELEMENTS];

    uint32 nof_registers;
    soc_reg_t register_ids[DBAL_MAX_NUMBER_OF_HW_PORTING_ELEMENTS];
    soc_reg_t register_mapped_ids[DBAL_MAX_NUMBER_OF_HW_PORTING_ELEMENTS];
} hl_porting_info_t;

typedef struct
{
    uint8 group_hw_entity_num;
    int   hw_entity_id[DBAL_MAX_NUMBER_OF_HW_ELEMENTS];
    uint8 is_reg;
} hl_group_info_t;

typedef struct
{
    uint32 nof_groups;
    hl_group_info_t *group_info;
} hl_groups_info_t;

typedef struct
{
    
    int nof_tables_with_error;

    
    hl_porting_info_t hl_porting_info;

    dbal_logical_table_t logical_tables[DBAL_NOF_DYNAMIC_AND_STATIC_TABLES];

    
    hl_groups_info_t groups_info;

} dbal_logical_tables_info_t;

typedef struct
{
    
    uint8 disable_logger;

    
    dbal_tables_e user_log_locked_table;

    
    dbal_tables_e internal_log_locked_table;

    
    FILE *dbal_file;

    
    dbal_logger_mode_e logger_mode;

} dbal_logger_info_t;


typedef struct
{
    sal_mutex_t pre_table_mutex;
    dbal_tables_e table_id; 
} dbal_specific_table_protection_info_t;

typedef struct
{
    uint32 action_types_btmp;
    uint8 hit_indication_rule;
    uint8 hit_indication_rule_mask;
    uint32 key_rule_buf[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    uint32 key_rule_mask_buf[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    uint32 value_rule_buf[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 value_rule_mask_buf[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 value_action_buf[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 value_action_mask_buf[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 key_rule_size;
    uint32 value_rule_size;
    uint32 value_action_size;
} dbal_flush_shadow_info_t;

typedef struct
{
    
    dbal_entry_handle_t *entry_handles_pool;
    dbal_iterator_info_t iterators_pool[DBAL_SW_NOF_ENTRY_HANDLES];
    dbal_logger_info_t logger_info;
    dbal_status_e status;
    int is_std_1;

    
    int collection_counter;

    
    sal_mutex_t collection_pool_mutex_array[DBAL_NOF_COLLECTION_POOLS];

    
    sal_mutex_t collection_worker_thread_sync;

    
    sal_sem_t   collection_semaphore_array[DBAL_NOF_COLLECTION_POOLS];

    
    int         collection_signals[DBAL_NOF_COLLECTION_POOLS];

    
    dbal_status_e pemla_status; 

    
    sal_mutex_t handle_mutex;

    
    dbal_specific_table_protection_info_t protect_info[DBAL_NOF_SHARED_TABLE_MUTEXES]; 
} dbal_mngr_info_t;

#endif 
