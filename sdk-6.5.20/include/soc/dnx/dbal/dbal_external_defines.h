
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DBAL_EXTERNAL_DEFINES_H_INCLUDED

#define DBAL_EXTERNAL_DEFINES_H_INCLUDED

#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_field_types_defs.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_system.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_hw_entity_groups.h>
#include <shared/shrextend/shrextend_debug.h>

#define DBAL_FUNC_NOF_ENTRY_HANDLES                     10

#define INST_SINGLE                                   (-1)

#define INST_ALL                                      (-2)

#define DBAL_RANGE_ALL                                (-1)

#define DBAL_MAX_PRINTABLE_BUFFER_SIZE                        1024

#define DBAL_FIELD_ARRAY_MAX_SIZE_IN_BITS                     512

#define DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES                    (BITS2BYTES(DBAL_FIELD_ARRAY_MAX_SIZE_IN_BITS))

#define DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS                    (BITS2WORDS(DBAL_FIELD_ARRAY_MAX_SIZE_IN_BITS))

#define DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS                      43

#define DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE      255

#define DBAL_TABLE_DEFAULT_MAX_NOF_INSTANCES                    32

#define DBAL_MAX_STRING_LENGTH                                  72

#define DBAL_MAX_EXTRA_LONG_STRING_LENGTH                       400

#define DBAL_MAX_LONG_STRING_LENGTH                             (2*DBAL_MAX_EXTRA_LONG_STRING_LENGTH + DBAL_MAX_STRING_LENGTH)

typedef enum
{

    DBAL_PREDEF_VAL_MIN_VALUE,

    DBAL_PREDEF_VAL_MAX_VALUE,

    DBAL_PREDEF_VAL_DEFAULT_VALUE,

    DBAL_PREDEF_VAL_RESET_VALUE,

    DBAL_NOF_PREDEFINE_VALUES
} dbal_field_predefine_value_type_e;

typedef enum
{
    DBAL_PERMISSION_ALL,
    DBAL_PERMISSION_READONLY,
    DBAL_PERMISSION_WRITEONLY,
    DBAL_PERMISSION_TRIGGER,
    DBAL_NOF_PERMISSIONS
} dbal_field_permission_e;

typedef enum
{

    DBAL_COLLECTION_TRIGGER_PROCESS_AND_WAIT,

    DBAL_COLLECTION_TRIGGER_PROCESS,

    DBAL_NOF_COLLECTION_TRIGGER_TYPES
} dbal_collection_trigger_type_e;

typedef enum
{

    DBAL_COMMIT = SAL_BIT(1),

    DBAL_COMMIT_UPDATE = SAL_BIT(2),

    DBAL_COMMIT_FORCE = SAL_BIT(3),

    DBAL_GET_ALL_FIELDS = SAL_BIT(4),

    DBAL_COMMIT_OVERRUN = SAL_BIT(5),

    DBAL_COMMIT_OVERRIDE_DEFAULT = SAL_BIT(6),

    DBAL_COMMIT_DISABLE_ACTION_PRINTS = SAL_BIT(7),

    DBAL_COMMIT_VALIDATE_OTHER_CORE = SAL_BIT(8),

    DBAL_COMMIT_IGNORE_ALLOC_ERROR = SAL_BIT(9),

    DBAL_COMMIT_NOF_OPCODES
} dbal_entry_action_flags_e;

typedef enum
{

    DBAL_ITER_MODE_ALL,

    DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT,

    DBAL_ITER_MODE_ACTION_NON_BLOCKING,

    DBAL_ITER_MODE_GET_ENTRIES_FROM_DMA_NO_TRIGGER,

    DBAL_ITER_MODE_GET_NON_DEFAULT_PERMISSION_WRITE,

    DBAL_NOF_ITER_TYPES
} dbal_iterator_mode_e;

typedef enum
{

    DBAL_CONDITION_NONE,

    DBAL_CONDITION_BIGGER_THAN,

    DBAL_CONDITION_LOWER_THAN,

    DBAL_CONDITION_EQUAL_TO,

    DBAL_CONDITION_NOT_EQUAL_TO,

    DBAL_CONDITION_IS_EVEN,

    DBAL_CONDITION_IS_ODD,

    DBAL_NOF_CONDITION_TYPES
} dbal_condition_types_e;

typedef enum
{

    DBAL_ITER_ACTION_UPDATE = SAL_BIT(0),

    DBAL_ITER_ACTION_DELETE = SAL_BIT(1),

    DBAL_ITER_ACTION_GET = SAL_BIT(2),

    DBAL_ITER_ACTION_HIT_CLEAR = SAL_BIT(3),

    DBAL_ITER_ACTION_HIT_GET = SAL_BIT(4),

    DBAL_ITER_ACTION_READ_TO_DMA = SAL_BIT(5)
} dbal_iterator_action_types_e;

typedef enum
{

    DBAL_ACCESS_METHOD_MDB,

    DBAL_ACCESS_METHOD_TCAM_CS,

    DBAL_ACCESS_METHOD_HARD_LOGIC,

    DBAL_ACCESS_METHOD_SW_STATE,

    DBAL_ACCESS_METHOD_PEMLA,

    DBAL_ACCESS_METHOD_KBP,

    DBAL_NOF_ACCESS_METHODS
} dbal_access_method_e;

typedef enum
{

    DBAL_TABLE_TYPE_NONE,

    DBAL_TABLE_TYPE_EM,

    DBAL_TABLE_TYPE_TCAM,

    DBAL_TABLE_TYPE_TCAM_DIRECT,

    DBAL_TABLE_TYPE_TCAM_BY_ID,

    DBAL_TABLE_TYPE_LPM,

    DBAL_TABLE_TYPE_DIRECT,

    DBAL_NOF_TABLE_TYPES
} dbal_table_type_e;

typedef enum
{

    DBAL_PHYSICAL_TABLE_NONE,

    DBAL_PHYSICAL_TABLE_TCAM,

    DBAL_PHYSICAL_TABLE_KAPS_1,

    DBAL_PHYSICAL_TABLE_KAPS_2,

    DBAL_PHYSICAL_TABLE_ISEM_1,

    DBAL_PHYSICAL_TABLE_INLIF_1,

    DBAL_PHYSICAL_TABLE_IVSI,

    DBAL_PHYSICAL_TABLE_ISEM_2,

    DBAL_PHYSICAL_TABLE_ISEM_3,

    DBAL_PHYSICAL_TABLE_INLIF_2,

    DBAL_PHYSICAL_TABLE_INLIF_3,

    DBAL_PHYSICAL_TABLE_LEM,

    DBAL_PHYSICAL_TABLE_IOEM_1,

    DBAL_PHYSICAL_TABLE_IOEM_2,

    DBAL_PHYSICAL_TABLE_MAP,

    DBAL_PHYSICAL_TABLE_FEC_1,

    DBAL_PHYSICAL_TABLE_FEC_2,

    DBAL_PHYSICAL_TABLE_FEC_3,

    DBAL_PHYSICAL_TABLE_PPMC,

    DBAL_PHYSICAL_TABLE_GLEM_1,

    DBAL_PHYSICAL_TABLE_GLEM_2,

    DBAL_PHYSICAL_TABLE_EEDB_1,

    DBAL_PHYSICAL_TABLE_EEDB_2,

    DBAL_PHYSICAL_TABLE_EEDB_3,

    DBAL_PHYSICAL_TABLE_EEDB_4,

    DBAL_PHYSICAL_TABLE_EEDB_5,

    DBAL_PHYSICAL_TABLE_EEDB_6,

    DBAL_PHYSICAL_TABLE_EEDB_7,

    DBAL_PHYSICAL_TABLE_EEDB_8,

    DBAL_PHYSICAL_TABLE_EOEM_1,

    DBAL_PHYSICAL_TABLE_EOEM_2,

    DBAL_PHYSICAL_TABLE_ESEM,

    DBAL_PHYSICAL_TABLE_EVSI,

    DBAL_PHYSICAL_TABLE_SEXEM_1,

    DBAL_PHYSICAL_TABLE_SEXEM_2,

    DBAL_PHYSICAL_TABLE_SEXEM_3,

    DBAL_PHYSICAL_TABLE_LEXEM,

    DBAL_PHYSICAL_TABLE_RMEP_EM,

    DBAL_PHYSICAL_TABLE_KBP,

    DBAL_NOF_PHYSICAL_TABLES
} dbal_physical_tables_e;

typedef enum
{
    DBAL_LOGGER_MODE_REGULAR,
    DBAL_LOGGER_MODE_WRITE_ONLY,

    DBAL_NOF_LOGGER_MODES
} dbal_logger_mode_e;

typedef enum
{
    DBAL_DEVICE_STATE_NOT_SUPPORTED,
    DBAL_DEVICE_STATE_ON_PRODUCTION,
    DBAL_DEVICE_STATE_UNDER_DEVELOPMENT,

    DBAL_NOF_DEVICE_STATE
} dbal_device_state_e;

typedef struct
{

    dbal_fields_e field_id;

    dbal_field_types_defs_e field_type;

    int field_nof_bits;

    uint32 min_value;

    uint32 max_value;

    uint32 const_value;

    int offset_in_logical_field;

    uint32 field_indication_bm[1];

    int bits_offset_in_buffer;

    int nof_instances;

    dbal_field_permission_e permission;

    uint32 arr_prefix;

    uint8 arr_prefix_size;

} dbal_table_field_info_t;

typedef struct
{

    dbal_fields_e field_id;

    uint32 field_nof_bits;

    uint8 is_valid_indication_needed;

    uint8 is_ranged;

    uint8 is_packed;

    uint8 arr_prefix;

    uint8 arr_prefix_size;
} dbal_table_field_input_info_t;

typedef struct
{
    dbal_fields_e field_id;
    char field_name[DBAL_MAX_LONG_STRING_LENGTH];
    char field_print_value[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
} dbal_printable_field_t;

typedef struct
{
    int nof_key_fields;
    int nof_res_fields;
    dbal_printable_field_t key_fields_info[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS];
    dbal_printable_field_t res_fields_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE];
} dbal_printable_entry_t;

#endif
