/**
 * \file diag_dnx_dbal_signals_db_init.h
 * 
 *
 * Main functions for init the dbal signals tables DB
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*************
 * INCLUDES  *
 */

#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/util.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_rhlist.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/sand/sand_aux_access.h>

/**
 * \brief
 * path to signal tables xml file(s) (relative to DB path 
 * folder) 
 */
#  define DB_INIT_SIGNALS_PATH          "dbal/Signals"
/**
 *  generic key field info for signals table
 */
typedef struct
{

    /** The Signal name  */
    char signal_name[DBAL_MAX_STRING_LENGTH];

    /** the field id  */
    dbal_fields_e field_id;

    /** full field size in table. */
    int nof_bits;
    int offset;
} dbal_signals_table_key_field_info_t;

/**
 *  generic result field info for signals table
 */
typedef struct
{

    /** the field id  */
    dbal_fields_e field_id;
} dbal_signals_table_result_field_info_t;

/**
 *  generic field info for table
 */
typedef struct
{
    char stage_name[DBAL_MAX_STRING_LENGTH];
    dnx_pp_stage_e stage_id;
    int nof_key_fields;
    dbal_signals_table_key_field_info_t *signals_key_info;
    int nof_result_fields;
    dbal_signals_table_result_field_info_t *result_field_info;
} dbal_signals_stage_info_t;

/**
 *  \brief signals table structure definition
 */
typedef struct
{

    /** General table parameters  */
    char table_name[DBAL_MAX_STRING_LENGTH];

    /** Interface parameters information regarding the key and result fields  */
    int nof_stages;
    dbal_signals_stage_info_t *signals_stage_info;
} dbal_signals_table_t;

shr_error_e diag_dnx_dbal_signals_db_init_tables(
    int unit);

shr_error_e diag_dnx_dbal_signals_db_deinit_tables(
    int unit);

/**
 *  generic result field info for signals table
 */
typedef struct
{

    /** the field name  */
    char result_field_name[DBAL_MAX_STRING_LENGTH];

    /*
     *  field_id 
     */
} table_signals_db_result_field_info_t;

typedef struct
{
    char stage_name[DBAL_MAX_STRING_LENGTH];
    int nof_key_fields;
    dbal_signals_table_key_field_info_t signals_key_info[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS];
    int nof_result_fields;
    table_signals_db_result_field_info_t result_field_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE];
} table_signals_db_stage_info_struct_t;

#define MAX_NUM_SIGNALS_DIAG_STAGES 10

typedef struct
{

    /** General table parameters  */
    char table_name[DBAL_MAX_STRING_LENGTH];

    /** Interface parameters information regarding the key and result fields  */
    int nof_stages;
    table_signals_db_stage_info_struct_t signals_stage_info[MAX_NUM_SIGNALS_DIAG_STAGES];
} table_signals_db_struct_t;
