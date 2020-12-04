
/** \file diag_dnx_dbal_internal.h
 * Purpose: Extern declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_DBAL_INTERNAL_H_INCLUDED
#define DIAG_DNX_DBAL_INTERNAL_H_INCLUDED

/*************
 * INCLUDES  *
 */
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

/*************
 *  DEFINES  *
 */
/**
 * minimum number of entries that checked in logical table test
 */
#  define DIAG_DBAL_MIN_IN_LIF_TO_ALLOCATE       (30)
#  define DIAG_DBAL_DEFAULT_NOF_ENTRIES          (0)
#  define CMD_MAX_STRING_LENGTH                  (64)
#  define CMD_MAX_NOF_INPUTS                     (5)
/**
 * Maximum number of entries checked in logical table test
 */
#define DIAG_DBAL_MAX_NOF_ENTRIES_PER_TABLE       (32)
/*************
 *  MACROES  *
 */
/**
* \brief
* print to the consule:
*   string name
*   ===========
* if name not exists use "\0" or ""
*  \par DIRECT INPUT:
*    \param [in] header - string\n
*    \param [in] name - string \n
 */
#  define DIAG_DBAL_HEADER_DUMP(header, name)                         \
{                                                                     \
  int iter = 0, iter_num = sal_strlen(header)+sal_strlen(name);       \
  LOG_CLI((BSL_META("\n%s %s\n"), header, name));                     \
  for(iter=0;iter<iter_num+1;iter++)                                  \
  {                                                                   \
    LOG_CLI((BSL_META("=")));                                         \
  }                                                                   \
  LOG_CLI((BSL_META("\n\n")));                                        \
}

 /**
 * \brief
 * print to the consule:
 *   string name
 *   **********
 * if name not exists use "\0" or ""
 *  \par DIRECT INPUT:
 *    \param [in] header - string\n
 *    \param [in] name - string \n
 */

#  define DIAG_DBAL_SUBHEADER_DUMP(header, name)                        \
{                                                                     \
  int iter = 0, iter_num = sal_strlen(header)+sal_strlen(name);       \
  LOG_CLI((BSL_META("%s %s\n"), header, name));                       \
  for(iter=0;iter<iter_num+1;iter++)                                  \
  {                                                                   \
    LOG_CLI((BSL_META("~")));                                         \
  }                                                                   \
  LOG_CLI((BSL_META("\n")));                                          \
}

/**
 *  setting the maximum value for N bits field
 *  UINT/INT support
 */
#define DIAG_DBAL_MAX_FIELD_VAL(field_arr,field_len) \
{                                                               \
  int iter_num = (field_len-1)/32;                              \
  int residue = field_len%32;                                   \
  int iter = 0;                                                 \
  for(iter=0;iter<iter_num;iter++)                              \
  {                                                             \
    field_arr[iter] = 0xFFFFFFFF;                               \
  }                                                             \
  if(residue == 0)                                              \
  {                                                             \
    field_arr[iter_num] = 0xFFFFFFFF;                           \
  }                                                             \
  else                                                          \
  {                                                             \
    field_arr[iter_num] = ((1<<residue)-1);                     \
  }                                                             \
}

/**
 *  setting the minimum value for N bits field
 *  UINT/INT support
 */
#define DIAG_DBAL_MIN_FIELD_VAL(field_arr,field_len,field_type) \
{                                                               \
  int iter_num = (field_len-1)/32;                              \
  int iter = 0;                                                 \
  for(iter=0;iter<=iter_num;iter++)                             \
  {                                                             \
    field_arr[iter] = 0x0;                                      \
  }                                                             \
}

/**
 *  setting random value for N bits field
 *  random cannot set minimum or maximum values (except of field
 *  size is 1bit), in that case - for result field the maximum
 *  should be returned
 */
#define DIAG_DBAL_RANDOM_FIELD_VAL(field_arr,field_len,is_key)        \
{                                                                     \
  int iter_num = (field_len+31)/32;                                   \
  int residue = field_len%32;                                         \
  int iter = 0;                                                       \
  uint32 mask = 0xFFFFFFFF;                                           \
  for(iter=0;iter<iter_num;iter++)                                    \
  {                                                                   \
    field_arr[iter] = (sal_rand() + ((sal_rand()&0x4000)<<1))         \
                    + ((sal_rand() + ((sal_rand()&0x4000)<<1))<<16);  \
  }                                                                   \
  iter = iter_num - 1;                                                \
  if(residue != 0)                                                    \
  {                                                                   \
    mask = ((1<<residue)-1);                                          \
    field_arr[iter] &= mask;                                          \
  }                                                                   \
  if(field_len == 1)                                                  \
  {                                                                   \
    if(is_key == 0)                                                   \
    {                                                                 \
      field_arr[0] = 1;                                               \
    }                                                                 \
  }                                                                   \
  else                                                                \
  {                                                                   \
    if(field_arr[iter] == mask)                                       \
    {                                                                 \
      field_arr[iter] &= 0xFFFFFFFE;                                  \
    }                                                                 \
    if(field_arr[iter] == 0)                                          \
    {                                                                 \
      field_arr[iter] = 1;                                            \
    }                                                                 \
  }                                                                   \
}

/*************
 * TYPEDEFS  *
 */
 /**
 * \brief
 * Typedef:    dbal_ut_modes_t
 * Purpose:    In DBAL UT we can keep table content
 *             after test end or restore table content.
 */
typedef enum dbal_ut_modes_e
{
    DBAL_UT_KEEP_TABLE = 0,     /* keep table values at the end of the test */
    DBAL_UT_RESTORE_TABLE,      /* restore table values at the end of the test */
} dbal_ut_modes_t;

typedef enum
{
    MIN_FIELD_VAL,
    RANDOM_FIELD_VAL,
    MAX_FIELD_VAL
} fields_values_types_e;

typedef enum
{
    LTT_FLAGS_DEFAULT = 0,
    LTT_FLAGS_RUN_ITERATOR = SAL_BIT(0),
    LTT_FLAGS_TABLE_CLEAR_END_OF_TEST = SAL_BIT(1),
    LTT_FLAGS_DO_NOT_REMOVE_ENTRIES = SAL_BIT(2),
    LTT_FLAGS_RANDOM_VALUES = SAL_BIT(3),
    LTT_FLAGS_FULL_ITERATOR_TABLE_CLEAR_TEST = SAL_BIT(4),
    LTT_FLAGS_MEASUREMENTS = SAL_BIT(6),
    LTT_FLAGS_ERR_RECOVERY_CLEAN_UP = SAL_BIT(7),
    LTT_FLAGS_MULTIPLE_RESULTS = SAL_BIT(8),
    LTT_FLAGS_SW_FIELDS = SAL_BIT(9),
    LTT_FLAGS_SNAPSHOT_NEGATIVE = SAL_BIT(10),
    LTT_FLAGS_NOF_FLAGS = SAL_BIT(11)
} dbal_ltt_flags_e;

typedef enum
{
    LTT_DYN_FLAGS_RANDOM_INIT = SAL_BIT(0),
    LTT_DYN_FLAGS_MDB_CONFIG_INIT = SAL_BIT(1),
    LTT_DYN_FLAGS_FULL_TEST = SAL_BIT(3),
    LTT_DYN_FLAGS_NOF_FLAGS = SAL_BIT(4)
} dbal_ltt_dyn_flags_e;

/** Utility structure for storing the DBAL LTT inputs */
typedef struct
{
    /** The table that will be tested; Use DBAL_TABLE_EMPTY for all tables */
    dbal_tables_e dbal_table;

    /** The table access filter; Use DBAL_NOF_ACCESS_METHODS for all access methods */
    dbal_access_method_e access_method;

    /** The table type filter; Use DBAL_NOF_TABLE_TYPES for all table types */
    dbal_table_type_e table_type;

    /** The flags that will be used in the test */
    dbal_ltt_flags_e flags;

    /** Number of entries to run the test. 0 means internal default value */
    uint32 nof_entries;

} dnx_dbal_ltt_input_t;

/*************************************/
/****** DBAL LTT timers section  *****/
/*************************************/
typedef enum
{
    LTT_TIMER_SET,
    LTT_TIMER_GET,
    LTT_TIMER_ITERATOR,
    LTT_TIMER_ENTRY_CLEAR,
    LTT_TIMER_TABLE_CLEAR,
    LTT_TIMER_ITERATOR_EMPTY_TABLE,
    LTT_TIMER_ENTIRE_LTT_TEST,
    LTT_NOF_TIMERS
} dbal_ltt_timers_types_e;

typedef struct
{
    int is_active;
    uint32 total_time;
    uint32 nof_hits;
    char *name;

} dbal_ltt_timers_info_t;

uint32 ltt_timers_group;
dbal_ltt_timers_info_t ltt_timers_info[LTT_NOF_TIMERS];

#define LTT_TIMERS_ALLOC()                                                              \
do                                                                                      \
{                                                                                       \
    SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate("DBAL_LTT", &ltt_timers_group));     \
}while(0)

#define LTT_TIMERS_FREE()                                           \
do                                                                  \
{                                                                   \
    SHR_IF_ERR_EXIT(utilex_ll_timer_group_free(ltt_timers_group));  \
}while(0)

#define LTT_TIMERS_CLEAR_ALL()                                      \
do                                                                  \
{                                                                   \
    SHR_IF_ERR_EXIT(utilex_ll_timer_clear_all(ltt_timers_group));   \
}while(0)

#define LTT_TIMERS_TIMER_SET(name, timer_idx)                                   \
do                                                                              \
{                                                                               \
    SHR_IF_ERR_EXIT(utilex_ll_timer_set(name, ltt_timers_group, timer_idx));    \
}while(0)

#define LTT_TIMERS_TIMER_STOP(timer_idx)                                    \
do                                                                          \
{                                                                           \
    SHR_IF_ERR_EXIT(utilex_ll_timer_stop(ltt_timers_group, timer_idx));     \
}while(0)

#define LTT_TIMERS_TIMER_GET_INFO(timer_idx)                                                    \
do                                                                                              \
{                                                                                               \
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(ltt_timers_group, timer_idx,                       \
                                                     &ltt_timers_info[timer_idx].is_active,     \
                                                     &ltt_timers_info[timer_idx].name,          \
                                                     &ltt_timers_info[timer_idx].nof_hits,      \
                                                     &ltt_timers_info[timer_idx].total_time));  \
}while(0)

#define LTT_TIMERS_ALL_GET_INFO()                                                               \
do                                                                                              \
{                                                                                               \
    int ii;                                                                                     \
    for(ii=0; ii < LTT_NOF_TIMERS; ii++)                                                        \
    {                                                                                           \
        LTT_TIMERS_TIMER_GET_INFO(ii);                                                          \
    }                                                                                           \
}while(0)

#define LTT_TIMERS_TIMER_GET_TIME_US(timer_idx)                                         \
        (ltt_timers_info[timer_idx].nof_hits == 0) ? 0 :                                \
        (ltt_timers_info[timer_idx].total_time / ltt_timers_info[timer_idx].nof_hits)

#define LTT_TIMERS_CHECK_FOR_EXCEPTIONS(unit, table_id)   diag_dnx_dbal_timers_check_exceptions(unit, table_id)

/*************
 * FUNCTIONS *
 */

/**
 * \brief -
 * This API check the sets of measured time per table in LTT
 * test. It compares the measured times to a limited hard-coded
 * times, defined in dbal_ltt_timer_limits, If one of the
 * measured times is exceeding, it returns an error
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   \param [in] table_id -
 * \par INDIRECT INPUT:
 *  \b dbal_ltt_timer_limits
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 */
shr_error_e diag_dnx_dbal_timers_check_exceptions(
    int unit,
    dbal_tables_e table_id);

/**
* \brief
* Get a random/maximum/minimum value for field.
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] field_id
*    \param [in] type_of_value \n random/max/min
*    \param [in] field_min_value
*    \param [in] field_max_value
*    \param [in] field_len \n length in bit
*    \param [in] is_key \n key or reault indication
*    \param [in] arr_prefix \n key or reault indication
*    \param [in] arr_prefix_size \n key or reault indication
*    \param [in] field_value \n ptr to return value
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      result cmd
 */
shr_error_e dbal_fields_get_value_for_field(
    int unit,
    dbal_fields_e field_id,
    fields_values_types_e type_of_value,
    uint32 field_min_value,
    uint32 field_max_value,
    uint32 field_len,
    uint8 is_key,
    uint32 arr_prefix,
    uint32 arr_prefix_size,
    uint32 *field_value);

/** list of SW tables that has allocator field but allocation is not needed for them */
int dbal_table_is_allcator_not_needed(
    int unit,
    dbal_tables_e table_id);
/**
* \brief
* compare two fields array values, each field represented as array32.
* returnes 0 if two fields value equal
* otherwise -1
*/
int compare_fields_values(
    int unit,
    CONST dbal_logical_table_t * table,
    uint32 field_val[][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint32 field_val_get[][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    int res_type_idx,
    uint8 is_key);

/**
* \brief
* dump to consule logical table information.
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] table_id - table ID to print\n
*    \param [in] print_access  - 0 dump only interface part, 1 dump also logical to phy mapping\n
*    \param [in] sand_control  - sand control command
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      result cmd
 */
shr_error_e diag_dbal_logical_table_dump(
    int unit,
    dbal_tables_e table_id,
    int print_access,
    sh_sand_control_t * sand_control);

shr_error_e diag_dbal_table_indications_dump(
    int unit,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t * table,
    sh_sand_control_t * sand_control);

shr_error_e diag_dnx_dbal_print_table_info(
    int unit,
    dbal_tables_e table_id,
    int print_access,
    sh_sand_control_t * sand_control);

/**
* \brief
* dump to consule physical table information.
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] phy_table_id - field type to print\n
*    \param [in] sand_control  - sand control command
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      result cmd
*/
shr_error_e diag_dbal_physical_tables_info_dump(
    int unit,
    dbal_physical_tables_e phy_table_id,
    sh_sand_control_t * sand_control);

shr_error_e dbal_field_types_label_is_related_get(
    int unit,
    dbal_field_types_defs_e field_type,
    dbal_labels_e label,
    uint8 *is_related);

/**
* \brief
* dump to consule field type information.
* If label is none, print all related labels.
* If label is not none, print list of related tables
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] field_type - field type to print\n
*    \param [in] label  - dbal label related\n
*    \param [in] sand_control  - sand control command
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      result cmd
*/
shr_error_e diag_dbal_field_type_print_info(
    int unit,
    dbal_field_types_defs_e field_type,
    dbal_labels_e label,
    sh_sand_control_t * sand_control);

/**
* \brief
* dump to consule field value according to type.
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] field_id - requested field ID \n
*    \param [in] table_id - related table \n
*    \param [in] field_val - field value \n
*    \param [in] field_mask - field maske \n
*    \param [in] is_key - indication for key of result field\n
*    \param [in] result_type_idx - in case of result field, result type index\n
*    \param [in] in_offset - used only if the value is not aligned to start of the buffer\n
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      result cmd
 */
shr_error_e diag_dbal_field_dump(
    int unit,
    dbal_fields_e field_id,
    dbal_tables_e table_id,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES],
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES],
    uint8 is_key,
    int result_type_idx,
    int in_offset /* offset in buffer */ );

/**
* \brief
* creates a string that represents the encode information.
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] encode_info - encoding info to use\n
*    \param [in] str  - pointer to the requested string created\n
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      result cmd
* \par INDIRECT OUTPUT
*       str  - filled the str \n
 */
void diag_dbal_offset_encode_info_to_str(
    int unit,
    CONST dbal_offset_encode_info_t * encode_info,
    char *str);

/**
* \brief
* Check if a table has at least one entry which is not read-only or trigger type
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] entry_handle_id - \n
*       handle_id
*    \param [out] has_standard_entries - \n
*       check result
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
 */
shr_error_e diag_dbal_table_has_standard_entries(
    int unit,
    int entry_handle_id,
    int *has_standard_entries);

/**
* \brief
* a basic logical tests for a given logical table
* the test inserts entries to the table, retrieves them and compares the retrieved entry to the expected one
* tests are:
* for Hard Logic tables, first validate HW and permissions
* adding MAX and MIN entries and getting the entries
* adding x random values entries and getting each entry
* running traverse to validate that there is only x entries.
* deleting the entries at the end of the regression.
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] table_id - \n
*       table_id
*    \param [in] ltt_input - \n
*       test input
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
 */
shr_error_e ctest_dbal_table_test_run(
    int unit,
    dbal_tables_e table_id,
    dnx_dbal_ltt_input_t * ltt_input);

/**
* \brief
* count entries in a table using dbal iterator
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] table_id - \n
*       table_id
*    \param [in] counter - \n
*       ptr to be updated with counter result
*  \par DIRECT OUTPUT:
*    cmd_result_t - \n
*      result cmd
*  \par INDIRECT OUTPUT:
*    counter - \n
*      See in direct input section
 */
shr_error_e diag_dbal_iterator_count_entries(
    int unit,
    dbal_tables_e table_id,
    int *counter);

cmd_result_t test_dnx_dbal_partial_access(
    int unit,
    dbal_tables_e table_id,
    dbal_ut_modes_t mode,
    uint8 is_rand);

#endif /* DIAG_DNX_DBAL_INTERNAL_H_INCLUDED */
