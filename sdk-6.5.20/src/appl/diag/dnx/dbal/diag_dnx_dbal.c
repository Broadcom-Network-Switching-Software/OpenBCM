/** \file diag_dnx_dbal.c
 *
 * Main diagnostics for dbal applications All CLI commands, that are related to DBAL, are gathered in this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDBALDNX

/** allow memregs.h include excplictly */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif

/*************
 * INCLUDES  *
 */
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <soc/mcm/memregs.h>
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for access*/
#include <appl/diag/diag.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include "diag_dnx_dbal_internal.h"
#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/sand/sand_aux_access.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/sand/sand_signals.h>

#include "include/soc/dnx/pemladrv/auto_generated/pemladrv.h"
#include "diag_dnx_dbal.h"

/*************
 * TYPEDEFS  *
 */

#define DNX_DBAL_LOGGER_OFF bslSeverityOff
#define DNX_DBAL_LOGGER_INFO bslSeverityInfo
#define DNX_DBAL_LOGGER_WARNING bslSeverityWarn
#define DNX_DBAL_LOGGER_ERROR bslSeverityError
#define DNX_DBAL_LOGGER_VERBOSE bslSeverityVerbose
#define DNX_DBAL_LOGGER_PRINT (DNX_DBAL_LOGGER_VERBOSE + 1)
/** this is not the acctual size, just an esstimation.. */

#define DBAL_HASH_TABLE_ENTRY_SIZE  BITS2BYTES(DBAL_SW_HASH_TABLES_MAX_HASH_KEY_SIZE_BITS)+sizeof(uint32)

/**
 * those parameters are used to save the pre action severity
 * it is used in the APIs Rdiag_dbal_pre_action_logger_close(int unit) and diag_dbal_post_action_logger_restore(int unit)
 */
bsl_severity_t api_severity;
bsl_severity_t access_severity;
bsl_severity_t diag_severity;
bsl_severity_t mdb_severity;

int dbal_logger_closed = 0;
int mdb_logger_closed = 0;

/********************
 * EXTERN FUNCTIONS *
 */

extern const char *strcaseindex(
    const char *s,
    const char *sub);

/*************
 * FUNCTIONS *
 */

shr_error_e
dbal_tables_field_is_key_const_val_num_get(
    int unit,
    dbal_tables_e table_id,
    uint8 *num_const)
{
    int field_index;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    *num_const = 0;

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        if (SHR_BITGET(table->keys_info[field_index].field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID))
        {
            *num_const = *num_const + 1;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_is_key_const_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 *is_key_const)
{
    int field_index;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    *is_key_const = FALSE;

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        if (table->keys_info[field_index].field_id == field_id)
        {
            if (SHR_BITGET(table->keys_info[field_index].field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID))
            {
                *is_key_const = TRUE;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * See diag_dnx_dbal_internal.h
 */
shr_error_e
diag_dbal_iterator_count_entries(
    int unit,
    dbal_tables_e table_id,
    int *counter)
{
    uint32 entry_handle_id = 0;
    int is_end;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Counting entries\n")));

    (*counter) = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_NON_DEFAULT_PERMISSION_WRITE));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        (*counter)++;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Found %d entries\n"), *counter));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function is used to detarmined if we need to define
 * allocator for a specific table
 *
 *    \param [in] unit - Relevant unit.
 *    \param [in] table_id - related table_id.
 *
 * \return
 *    \retval One - we need to define allocator for this
 *            table_id
 *    \retval Zero - we don't need to define allocator for this
 *            table_id
 */
int
dbal_table_is_allcator_not_needed(
    int unit,
    dbal_tables_e table_id)
{
    switch (table_id)
    {
        case DBAL_TABLE_EGRESS_LIF_TO_EGRESS_POINTED_MAP_SW:
        case DBAL_TABLE_EGRESS_POINTED_TO_EGRESS_LIF_MAP_SW:
        case DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW:
        case DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW:
        case DBAL_TABLE_ESEM_DUAL_HOMING_PER_INTERFACE_NAMESPACE_DB:
        case DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB:
            return 1;

        default:
            return 0;
    }
}

/**
 * \brief
 * This method is used to turn off the logger before performing a negative test that can dump information to console.
 * After calling to this function API, ACCESS, DIAG severity logger will be FATAL and the current severities will be
 * saved. To restore the logger severities, call diag_dbal_post_action_logger_restore()
 */
cmd_result_t
diag_dbal_pre_action_logger_close(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dbal_logger_closed)
    {
        api_severity = bslenable_get(bslLayerSocdnx, bslSourceDbaldnx);
        access_severity = bslenable_get(bslLayerSocdnx, bslSourceDbalaccessdnx);
        diag_severity = bslenable_get(bslLayerSocdnx, bslSourceDiagdbaldnx);

        bslenable_set(bslLayerSocdnx, bslSourceDbaldnx, bslSeverityFatal);
        bslenable_set(bslLayerSocdnx, bslSourceDbalaccessdnx, bslSeverityFatal);
        bslenable_set(bslLayerSocdnx, bslSourceDiagdbaldnx, bslSeverityError);

        dbal_logger_closed = 1;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 * this method restores the logger severities (after temporary closing them), it is called after
 * diag_dbal_pre_action_logger_close.
 */
cmd_result_t
diag_dbal_post_action_logger_restore(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dbal_logger_closed)
    {
        bslenable_set(bslLayerSocdnx, bslSourceDbaldnx, api_severity);
        bslenable_set(bslLayerSocdnx, bslSourceDbalaccessdnx, access_severity);
        bslenable_set(bslLayerSocdnx, bslSourceDiagdbaldnx, diag_severity);

        dbal_logger_closed = 0;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 * this method is used to turn off the MDB logger before
 * performing a negative test that can dump information to
 * consule. after calling to this function MDB logger will be
 * FATAL and the current severity will be saved. to restore
 * the logger severity, call
 * diag_dbal_post_action_mdb_logger_restore()
 */
cmd_result_t
diag_dbal_pre_action_mdb_logger_close(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!mdb_logger_closed)
    {
        mdb_severity = bslenable_get(bslLayerSocdnx, bslSourceMdbdnx);

        bslenable_set(bslLayerSocdnx, bslSourceMdbdnx, bslSeverityFatal);

        mdb_logger_closed = 1;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 * this method restores the MDB logger severity (after
 * temporary closing it), it is called after
 * diag_dbal_pre_action_mdb_logger_close.
 */
cmd_result_t
diag_dbal_post_action_mdb_logger_restore(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mdb_logger_closed)
    {
        bslenable_set(bslLayerSocdnx, bslSourceMdbdnx, mdb_severity);

        mdb_logger_closed = 0;
    }

    SHR_FUNC_EXIT;
}

/*************
* FUNCTIONS *
 */

 /*
  * LOCAL DIAG PACK:
  * {
  */
/**********************************************************************************************************************
 *  DBAL DIAGNOSTIC PACK:
 *  STRUCTURE:
 *  MAIN MENU CMD: define under sh_dnx_dbal_cmds
 *     TABLE - SUB MENU defined under sh_dnx_dbal_table_cmds
 *     ENTRY - SUB MENU defined under sh_dnx_dbal_entry_cmds
 *     TYPES - SUB MENU defined under sh_dnx_dbal_field_types_cmds
 *     LABELSDUMP
 *     LOGSEVERITY
 *     HANDLESSTATUS
 */

/**********************************************************************************************************************
 *  DBAL DIAGNOSTIC PACK:
 *  TABLE SUB MENU function & infrastructure Definitions - START
 *  STURCTURE:
 *  1. cmd function definition
 *  2. cmd option and man table
 *  3. TABLE SUB MENU cmd table
 */
/*
 * enum table for dynamic fiels support. size is doubled for supporting both shot enum name and full enum name.
 */
sh_sand_enum_t dbal_test_enum_table[2 * DBAL_FIELD_MAX_NUM_OF_ENUM_VALUES + 1];

shr_error_e
dbal_builds_enum_sand_table(
    int unit,
    dbal_fields_e field_id)
{
    dbal_field_types_basic_info_t *field_info;
    int enum_entry_index = 0;
    int enum_val_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_info));
    for (enum_val_index = 0; enum_val_index < field_info->nof_enum_values; enum_val_index++)
    {

        dbal_test_enum_table[enum_entry_index].string = field_info->name;
        dbal_test_enum_table[enum_entry_index].value = enum_val_index;
        dbal_test_enum_table[enum_entry_index].plugin_str = NULL;
        enum_entry_index++;
        dbal_test_enum_table[enum_entry_index].string = field_info->enum_val_info[enum_val_index].name;
        dbal_test_enum_table[enum_entry_index].value = enum_val_index;
        dbal_test_enum_table[enum_entry_index].plugin_str = NULL;
        enum_entry_index++;
    }
    dbal_test_enum_table[enum_entry_index].string = NULL;
    dbal_test_enum_table[enum_entry_index].value = enum_val_index;
    dbal_test_enum_table[enum_entry_index].plugin_str = NULL;
exit:

    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  used to map each dbal field type to sal field type
 */
sal_field_type_e dbal_field_type_to_sal_field_type[DBAL_NOF_FIELD_PRINT_TYPES] = {
    SAL_FIELD_TYPE_NONE,    /** DBAL_FIELD_PRINT_TYPE_NONE,        */
    SAL_FIELD_TYPE_BOOL,    /** DBAL_FIELD_PRINT_TYPE_BOOL,        */
    SAL_FIELD_TYPE_UINT32,  /** DBAL_FIELD_PRINT_TYPE_UINT32,      */
    SAL_FIELD_TYPE_IP4,     /** DBAL_FIELD_PRINT_TYPE_IPV4,        */
    SAL_FIELD_TYPE_IP6,     /** DBAL_FIELD_PRINT_TYPE_IPV6,        */
    SAL_FIELD_TYPE_MAC,     /** DBAL_FIELD_PRINT_TYPE_MAC,         */
    SAL_FIELD_TYPE_ARRAY32, /** DBAL_FIELD_PRINT_TYPE_ARRAY8,      */
    SAL_FIELD_TYPE_ARRAY32, /** DBAL_FIELD_PRINT_TYPE_ARRAY32,     */
    SAL_FIELD_TYPE_ARRAY32,  /** DBAL_FIELD_PRINT_TYPE_BITMAP,      */
    SAL_FIELD_TYPE_ENUM,    /** DBAL_FIELD_PRINT_TYPE_ENUM,        */
    SAL_FIELD_TYPE_INT32,   /** DBAL_FIELD_PRINT_TYPE_SYSTEM_CORE, */
    SAL_FIELD_TYPE_ARRAY32,  /** DBAL_FIELD_PRINT_TYPE_STRUCTURE,      */
    SAL_FIELD_TYPE_ARRAY32,  /** DBAL_FIELD_PRINT_TYPE_HEX,         */
    SAL_FIELD_TYPE_UINT32,  /** DBAL_FIELD_PRINT_TYPE_DBAL_TABLE,  */
    SAL_FIELD_TYPE_STR          /** DBAL_FIELD_PRINT_TYPE_STR,      */
};

/**********************************************************************************************************************
 *  DBAL DIAGNOSTIC PACK:
 *  FIELD SUB MENU function & infrastructure Definitions - START
 *  STURCTURE:
 *  1. cmd function definition
 *  2. cmd option and man table
 *  3. FIELD SUB MENU cmd table
 */
/**
* \brief
* Return the next field type that matches tonthe input string,
* (next refers to the curr_field_type) if the subsstring is
* fully match to the field_type name  than is_full_match=1 use
* curr_field_type = -1 to start form the first existing type
*/
static cmd_result_t
dbal_string_to_next_field_type_get(
    int unit,
    char *substr_match,
    dbal_field_types_defs_e curr_field_type,
    dbal_field_types_defs_e * field_type,
    uint8 *is_full_match)
{
    int iter;
    CONST dbal_field_types_basic_info_t *field_type_info;

    SHR_FUNC_INIT_VARS(unit);

    *field_type = DBAL_NOF_FIELD_TYPES_DEF;
    *is_full_match = 0;
    curr_field_type++;

    if ((substr_match == NULL) || (curr_field_type > DBAL_NOF_FIELD_TYPES_DEF))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input substr_match not exists or field_type out of range\n");
    }

    for (iter = curr_field_type; iter < DBAL_NOF_FIELD_TYPES_DEF; iter++)
    {
        SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, iter, &field_type_info));

        if (sal_strcasecmp(substr_match, field_type_info->name) == 0)
        {
            *is_full_match = 1;
            *field_type = iter;
            break;
        }

        if (strcaseindex(field_type_info->name, substr_match))
        {
            *field_type = iter;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Print field types names fit current substring
*/
static shr_error_e
dbal_print_field_types_names(
    int unit,
    char *field_type_name)
{
    CONST dbal_field_types_basic_info_t *field_type_info;
    dbal_field_types_defs_e field_type;
    uint8 found = FALSE;
    uint8 is_full_match = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    dbal_string_to_next_field_type_get(unit, field_type_name, 0, &field_type, &is_full_match);

    LOG_CLI((BSL_META("\tOptinal field types are: \n")));
    while (field_type != DBAL_NOF_FIELD_TYPES_DEF)
    {
        found = TRUE;
        SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, field_type, &field_type_info));
        LOG_CLI((BSL_META("\t%s \n"), field_type_info->name));
        dbal_string_to_next_field_type_get(unit, field_type_name, field_type, &field_type, &is_full_match);
    }

    if (found == FALSE)
    {
        LOG_CLI((BSL_META("\tNone. Use \"dbal types dump\" to dump all field types \n")));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_types_label_is_related_get(
    int unit,
    dbal_field_types_defs_e field_type,
    dbal_labels_e label,
    uint8 *is_related)
{
    int ii, jj, kk;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    *is_related = FALSE;
    if ((field_type < DBAL_NOF_FIELD_TYPES_DEF) && (label < DBAL_NOF_LABEL_TYPES))
    {
        /** Check labels from tables that use this type */
        for (ii = 0; ii < DBAL_NOF_TABLES; ii++)
        {
            uint8 table_has_label = FALSE;
            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, ii, &table));

            for (jj = 0; jj < table->nof_labels; jj++)
            {
                if (table->table_labels[jj] == label)
                {
                    table_has_label = TRUE;
                    break;
                }
            }

            if (!table_has_label)
            {
                continue;
            }

            for (jj = 0; jj < table->nof_key_fields; jj++)
            {
                if (table->keys_info[jj].field_type == field_type)
                {
                    *is_related = TRUE;
                    SHR_EXIT();
                }
            }

            for (jj = 0; jj < table->nof_result_types; jj++)
            {
                multi_res_info_t *res_info;
                res_info = &table->multi_res_info[jj];
                for (kk = 0; kk < res_info->nof_result_fields; kk++)
                {
                    if (res_info->results_info[kk].field_type == field_type)
                    {
                        *is_related = TRUE;
                        SHR_EXIT();
                    }
                }
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal field_type %d or label %d."
                     "DBAL_NOF_FIELD_TYPES_DEF=%d, DBAL_NOF_LABEL_TYPES=%d\n", field_type, label,
                     DBAL_NOF_FIELD_TYPES_DEF, DBAL_NOF_LABEL_TYPES);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
* receives input_strings[0] = label name
* dump all fields information that related to specific label,
* if label == DBAL_NOF_LABEL_TYPES or DBAL_LABEL_NONE
* dump all fields.
*/
static shr_error_e
cmd_dnx_dbal_field_types_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int ii;
    dbal_labels_e label = DBAL_LABEL_NONE;
    char *label_name = NULL;
    char *field_type_name = NULL;
    char *print_type_name = NULL;
    CONST dbal_field_types_basic_info_t *field_type_info;
    dbal_field_print_type_e print_type;
    int dynamic = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("type", field_type_name);
    SH_SAND_GET_STR("label", label_name);
    SH_SAND_GET_STR("print", print_type_name);
    SH_SAND_GET_BOOL("dynamic", dynamic);

    if (dynamic && (!ISEMPTY(field_type_name) || !ISEMPTY(label_name) || !ISEMPTY(print_type_name)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "dynamic can only be set as a sole parameter");
    }

    if (dynamic)
    {
        PRT_TITLE_SET("DYNAMIC FIELD TYPE LIST");
    }
    else if (ISEMPTY(label_name))
    {
        label = DBAL_LABEL_NONE;
        PRT_TITLE_SET("FIELD TYPE LIST");
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_label_string_to_id(unit, label_name, &label));
        PRT_TITLE_SET("FIELD TYPE LIST RELATED TO LABEL %s", dbal_label_to_string(unit, label));
    }

    if (ISEMPTY(print_type_name))
    {
        print_type = DBAL_FIELD_PRINT_TYPE_NONE;
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_field_print_type_string_to_id(unit, print_type_name, &print_type));
        PRT_INFO_ADD("showing only field types with type: %s", print_type_name);
    }

    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Bit Size");
    PRT_COLUMN_ADD("Min Value");
    PRT_COLUMN_ADD("Max Value");
    PRT_COLUMN_ADD("Print type");
    if (print_type == DBAL_FIELD_PRINT_TYPE_STRUCTURE)
    {
        PRT_COLUMN_ADD("nof struct fields");
    }
    else if (print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
    {
        PRT_COLUMN_ADD("nof enum values");
    }
    PRT_COLUMN_ADD("Encode mode");
    PRT_COLUMN_ADD("Encode input param");

    for (ii = DBAL_FIELD_TYPE_DEF_EMPTY + 1; ii < DBAL_NOF_FIELD_TYPES_DEF; ii++)
    {
        uint8 name_related = FALSE;
        uint8 label_related = FALSE;

        SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, ii, &field_type_info));

        if (dynamic)
        {
            if (!SHR_BITGET(field_type_info->field_indication_bm, DBAL_FIELD_IND_IS_DYNAMIC))
            {
                continue;
            }
        }

        if (ISEMPTY(field_type_name))
        {
            name_related = TRUE;
        }
        else
        {
            if (strcaseindex(field_type_info->name, field_type_name))
            {
                name_related = TRUE;
            }
        }

        if (label == DBAL_LABEL_NONE)
        {
            label_related = TRUE;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_field_types_label_is_related_get(unit, ii, label, &label_related));
        }

        if (print_type != DBAL_FIELD_PRINT_TYPE_NONE)
        {
            if (field_type_info->print_type != print_type)
            {
                continue;
            }
        }

        if ((name_related && label_related) || dynamic)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", field_type_info->name);
            PRT_CELL_SET("%d", field_type_info->max_size);

            if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_HEX)
            {
                PRT_CELL_SET("0x%x", field_type_info->min_value);
                PRT_CELL_SET("0x%x", field_type_info->max_value);
            }
            else
            {
                PRT_CELL_SET("%d", field_type_info->min_value);
                PRT_CELL_SET("%d", field_type_info->max_value);
            }
            PRT_CELL_SET("%s", dbal_field_print_type_to_string(unit, field_type_info->print_type));
            if (print_type == DBAL_FIELD_PRINT_TYPE_STRUCTURE)
            {
                PRT_CELL_SET("%d", field_type_info->nof_struct_fields);
            }
            else if (print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
            {
                PRT_CELL_SET("%d", field_type_info->nof_enum_values);
            }
            if (field_type_info->encode_info.encode_mode != DBAL_VALUE_FIELD_ENCODE_NONE)
            {
                PRT_CELL_SET("%s", dbal_field_encode_type_to_string(unit, field_type_info->encode_info.encode_mode));
                PRT_CELL_SET("%d", field_type_info->encode_info.input_param);
            }
            else
            {
                PRT_CELL_SET("NONE");
                PRT_CELL_SET(" - ");
            }
        }
    }

    PRT_COMMITX;
    LOG_CLI((BSL_META("\n")));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_dbal_field_types_dump_options[] = {
    {"TYPe", SAL_FIELD_TYPE_STR, "dbal field type", ""},
    {"LaBeL", SAL_FIELD_TYPE_STR, "dbal label name", ""},
    {"Print", SAL_FIELD_TYPE_STR, "dbal print types", ""},
    {"Dynamic", SAL_FIELD_TYPE_BOOL, "dbal print dynamic types", "No"},
    {NULL}
};

static sh_sand_man_t dnx_dbal_field_types_dump_man = {
    .brief = "Field types Dump operation manue",
    .full = "Dump all field types\n"
        "If a Type set, dump only field types which is a sub-string of Type\n"
        "If a Label set, dump only field types which is relate to the label\n"
        "If a print set, dump only field types which is relate to the print type\n"
        "If a dynamic set, dump only field types which are dynamically loaded",
    .synopsis = "Type=<field type name> Label=<Label>",
    .examples = "Type=profile Label=L3\n" "Type=profile print=STRUCTURE\n" "type=fec\n" "dynamic"
};

/**
* \brief
* receives input_strings[0] = label name
* dump all fields information that related to specific label,
* if label == DBAL_NOF_LABEL_TYPES or DBAL_LABEL_NONE
* dump all fields.
 */
static shr_error_e
cmd_dnx_dbal_field_types_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int rv;
    dbal_labels_e label;
    dbal_field_types_defs_e field_type;
    char *label_name, *field_type_name;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("type", field_type_name);
    SH_SAND_GET_STR("label", label_name);

    if (ISEMPTY(field_type_name))
    {
        DIAG_DBAL_HEADER_DUMP("Example: dbal types info type=FEC", "\0");
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(diag_dbal_pre_action_logger_close(unit));
        rv = dbal_field_types_def_string_to_id(unit, field_type_name, &field_type);
        SHR_IF_ERR_EXIT(diag_dbal_post_action_logger_restore(unit));

        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(dbal_print_field_types_names(unit, field_type_name));
            SHR_EXIT();
        }
    }

    if (ISEMPTY(label_name))
    {
        label = DBAL_LABEL_NONE;
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_label_string_to_id(unit, label_name, &label));
    }

    SHR_IF_ERR_EXIT(diag_dbal_field_type_print_info(unit, field_type, label, sand_control));

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_dbal_field_types_info_options[] = {
    {"TYPe", SAL_FIELD_TYPE_STR, "dbal field type", ""},
    {"LaBeL", SAL_FIELD_TYPE_STR, "dbal label name", ""},
    {NULL}
};

static sh_sand_man_t dnx_dbal_field_types_info_man = {
    .brief = "Field types info operation manue",
    .full = "If Type is not fully matched to any field type, a list of the optional types will be printed\n"
        "In addition to field type info:\n"
        "\tIf a Label is not set - print all related Labels and tables to the Type\n"
        "\tIf a Label is set - print all related tables to the Type and the Label",
    .synopsis = "Type=<field type name> Label=<Label>",
    .examples = "Type=IPV4 Label=MPLS\n" "type=fec"
};

sh_sand_cmd_t sh_dnx_dbal_field_types_cmds[] = {
  /************************************************************************************************************************
   *   CMD_NAME    *     CMD_ACTION           * Next  *        Options                    *            MAN                *
   *               *                          * Level *                                   *                               *
   *               *                          * CMD   *                                   *                               *
 */
    {"Info", cmd_dnx_dbal_field_types_info_cmd, NULL, dnx_dbal_field_types_info_options,
     &dnx_dbal_field_types_info_man},
    {"DuMP", cmd_dnx_dbal_field_types_dump_cmd, NULL, dnx_dbal_field_types_dump_options,
     &dnx_dbal_field_types_dump_man},
    {NULL}
};

/**********************************************************************************************************************
 *  DBAL DIAGNOSTIC PACK:
 *  LABELS SUB MENU function & infrastructure Definitions - START
 *  STURCTURE:
 *  1. cmd function definition
 *  2. cmd option and man table
 *  3. FIELD SUB MENU cmd table
 */

/**
 * \brief
 * dump all dbal labels
 * no input parameter is used
 */
static shr_error_e
cmd_dbal_labels_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_labels_e label;

    SHR_FUNC_INIT_VARS(unit);

    DIAG_DBAL_HEADER_DUMP("Existing labels", "\0");

    for (label = 0; label < DBAL_NOF_LABEL_TYPES; label++)
    {
        LOG_CLI((BSL_META("\t%s \n"), dbal_label_to_string(unit, label)));
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_dbal_lables_dump_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_dbal_lables_dump_man = {
    .brief = "labels operation manue",
    .full = NULL,
    .synopsis = NULL,
    .examples = NULL
};

static sh_sand_cmd_t sh_dnx_dbal_labels_cmds[] = {
  /********************************************************************************************************************
   *   CMD_NAME    *     CMD_ACTION       * Next  *        Options                    *            MAN                *
   *               *                      * Level *                                   *                               *
   *               *                      * CMD   *                                   *                               *
 */
    {"DuMP", cmd_dbal_labels_dump, NULL, dnx_dbal_lables_dump_options, &dnx_dbal_lables_dump_man},
    {NULL}
};

/**********************************************************************************************************************
 *  DBAL DIAGNOSTIC PACK:
 *  HANDLES SUB MENU function & infrastructure Definitions - START
 *  STURCTURE:
 *  1. cmd function definition
 *  2. cmd option and man table
 *  3. FIELD SUB MENU cmd table
 */

/**
* \brief
* dump information of DBAL entries handles.
* if id arg = valid handle ID than prints also
* extended information about the requested handle ID.
 */
static shr_error_e
cmd_dbal_handles_info_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int iter, free_handle_counter = 0, handle_id = 0;
    dbal_entry_handle_t *entry_handle;
    int nof_collection_handles = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ID", handle_id);

    for (iter = 0; iter < DBAL_SW_NOF_ENTRY_HANDLES; iter++)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, iter, &entry_handle));
        if (entry_handle->handle_status > DBAL_HANDLE_STATUS_AVAILABLE)
        {
            LOG_CLI((BSL_META("Handle %d is been used by table %s\n"), iter, entry_handle->table->table_name));
        }
        else
        {
            free_handle_counter++;
        }
    }
    LOG_CLI((BSL_META("Total available handles %d of %d handles \n"), free_handle_counter, DBAL_SW_NOF_ENTRY_HANDLES));

    if ((handle_id >= DBAL_SW_NOF_ENTRY_HANDLES) || (handle_id < -1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input handle_id out of range %d \n", handle_id);
    }

    if (handle_id != -1)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, handle_id, &entry_handle));
        if (entry_handle->handle_status != DBAL_HANDLE_STATUS_AVAILABLE)
        {
            LOG_CLI((BSL_META("handle id=%d, status=%d, core=%d, error indication=%d, table:%s\n"),
                     handle_id, entry_handle->handle_status, entry_handle->core_id,
                     entry_handle->error_info.error_exists, entry_handle->table->table_name));
        }
    }

    LOG_CLI((BSL_META("\n collection buffer status:\n")));
    for (iter = DBAL_SW_NOF_ENTRY_HANDLES; iter < DBAL_SW_NOF_ENTRY_HANDLES + DBAL_SW_NOF_ENTRY_HANDLES_FOR_COLLECTION;
         iter++)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, iter, &entry_handle));
        if (entry_handle->handle_status == DBAL_HANDLE_STATUS_ACTION_COLLECTED)
        {
            nof_collection_handles++;
        }
    }

    LOG_CLI((BSL_META("\t pool size %d \n"), dnx_data_dbal.db_init.dbal_collection_pool_size_get(unit)));
    LOG_CLI((BSL_META("\t nof pools %d \n"), DBAL_NOF_COLLECTION_POOLS));
    LOG_CLI((BSL_META("\t total number of busy handles %d \n"), nof_collection_handles));

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_dbal_handles_status_options[] = {
    {"ID", SAL_FIELD_TYPE_INT32, "handle ID", "-1"},
    {NULL}
};

static sh_sand_man_t dnx_dbal_handles_status_man = {
    .brief = "Entry handles operation manue",
    .full = NULL,
    .synopsis = NULL,
    .examples = "ID=0"
};

sh_sand_cmd_t sh_dnx_dbal_handles_cmds[] = {
  /*******************************************************************************************************************
   * CMD_NAME *     CMD_ACTION             * Next  *        Options                  *            MAN                *
   *          *                            * Level *                                 *                               *
   *          *                            * CMD   *                                 *                               *
 */
    {"StaTuS", cmd_dbal_handles_info_dump, NULL, dnx_dbal_handles_status_options, &dnx_dbal_handles_status_man},
    {NULL}
};

static shr_error_e
cmd_dbal_hl_unique_hw_element(
    int unit)
{
    int ii;
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    CONST dbal_logical_table_t *table;
    int nof_fields = 0, access_type;
    soc_reg_t reg;
    soc_mem_t memory;
    dbal_hl_l2p_info_t *l2p_hl_info;
    int table_count = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, DBAL_TABLE_EMPTY, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_HARD_LOGIC, DBAL_PHYSICAL_TABLE_NONE,
                     DBAL_TABLE_TYPE_NONE, &table_id));

    LOG_CLI((BSL_META("\nDumping all tables that has unique HW element \n")));

    while (table_id != DBAL_TABLE_EMPTY)
    {
        nof_fields = 0;
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

        nof_fields = table->hl_mapping_multi_res[0].l2p_hl_info[DBAL_HL_ACCESS_MEMORY].num_of_access_fields;
        if ((nof_fields > 0) &&
            (table->hl_mapping_multi_res[0].l2p_hl_info[DBAL_HL_ACCESS_REGISTER].num_of_access_fields > 0))
        {
            /** mapped to both register and memory */
            SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                            (unit, table_id, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_HARD_LOGIC, DBAL_PHYSICAL_TABLE_NONE,
                             DBAL_TABLE_TYPE_NONE, &table_id));
            continue;
        }

        if ((nof_fields == 0)
            && (table->hl_mapping_multi_res[0].l2p_hl_info[DBAL_HL_ACCESS_REGISTER].num_of_access_fields == 0))
        {
            /** reg and mem are zero, means that only SW is mapped */
            SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                            (unit, table_id, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_HARD_LOGIC, DBAL_PHYSICAL_TABLE_NONE,
                             DBAL_TABLE_TYPE_NONE, &table_id));
            continue;
        }

        access_type = DBAL_HL_ACCESS_MEMORY;
        if (nof_fields == 0)
        {
            access_type = DBAL_HL_ACCESS_REGISTER;
            nof_fields = table->hl_mapping_multi_res[0].l2p_hl_info[DBAL_HL_ACCESS_REGISTER].num_of_access_fields;
        }

        l2p_hl_info = &(table->hl_mapping_multi_res[0].l2p_hl_info[access_type]);

        if (access_type == DBAL_HL_ACCESS_REGISTER)
        {
            reg = l2p_hl_info->l2p_fields_info[0].reg[0];

            for (ii = 1; ii < nof_fields; ii++)
            {
                if (reg != l2p_hl_info->l2p_fields_info[ii].reg[0])
                {
                    break;
                }
            }

            if (ii == nof_fields)
            {
                int nof_fields_in_reg;
                soc_field_info_t *fields_array_in_reg;
                int jj;
                int size_of_reg = 0;

                LOG_CLI((BSL_META("Table %s mapped to unique Register %s"), table->table_name,
                         SOC_REG_NAME(unit, reg)));
                table_count++;

                /** check if is fully or partially mapped.
                 *  This is used to identify candidates tables for
                 *  packedFields */
                /** calculate the register size  */
                nof_fields_in_reg = SOC_REG_INFO(unit, reg).nFields;
                fields_array_in_reg = SOC_REG_INFO(unit, reg).fields;

                for (jj = 0; jj < nof_fields_in_reg; jj++)
                {
                    size_of_reg += fields_array_in_reg[jj].len;
                }

                if (size_of_reg == table->max_payload_size)
                {
                    LOG_CLI((BSL_META("and has result size is the same as register size (%d) \n"), size_of_reg));
                }
                else
                {
                    LOG_CLI((BSL_META("\n")));
                }
            }
        }
        else
        {
            memory = l2p_hl_info->l2p_fields_info[0].memory[0];

            for (ii = 1; ii < nof_fields; ii++)
            {
                if (memory != l2p_hl_info->l2p_fields_info[ii].memory[0])
                {
                    break;
                }
            }

            if (ii == nof_fields)
            {
                int nof_fields_in_mem;
                soc_field_info_t *fields_array_in_mem;
                int jj;
                int size_of_mem = 0;

                LOG_CLI((BSL_META("Table %s mapped to unique Memory %s"), table->table_name,
                         SOC_MEM_NAME(unit, memory)));
                table_count++;

                /** check if is fully or partially mapped.
                 *  This is used to identify candidates tables for
                 *  packedFields */
                /** calculate the register size  */
                nof_fields_in_mem = SOC_MEM_INFO(unit, memory).nFields;
                fields_array_in_mem = SOC_MEM_INFO(unit, memory).fields;

                for (jj = 0; jj < nof_fields_in_mem; jj++)
                {
                    /** We skip the last field (ECC) */
                    char *field_name = SOC_FIELD_NAME(unit, fields_array_in_mem[jj].field);
                    if (sal_strcasecmp(field_name, "ecc") == 0)
                    {
                        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                                        (unit, table_id, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_HARD_LOGIC,
                                         DBAL_PHYSICAL_TABLE_NONE, DBAL_TABLE_TYPE_NONE, &table_id));
                        continue;
                    }
                    size_of_mem += fields_array_in_mem[jj].len;
                }

                if (size_of_mem == table->max_payload_size)
                {
                    LOG_CLI((BSL_META(" and has result size is the same as mem size (%d)\n"), size_of_mem));
                }
                else
                {
                    LOG_CLI((BSL_META("\n")));
                }
            }
        }

        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_id, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_HARD_LOGIC, DBAL_PHYSICAL_TABLE_NONE,
                         DBAL_TABLE_TYPE_NONE, &table_id));
    }

    LOG_CLI((BSL_META("\nTotal nof tables %d \n"), table_count));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* dump information of DBAL access.
* dump all tables that has HW element (memory/register/HW_field)
* that related (substring) to the input string.
*/
static shr_error_e
cmd_dbal_hl_info_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int iter, hl_iter;
    dbal_tables_e table_iter = DBAL_TABLE_EMPTY;
    char *hw_element_name = NULL;
    CONST dbal_logical_table_t *table;
    dbal_hl_l2p_info_t *l2p_hl_info;
    int curr_res_type = 0, index;
    int found_match = 0;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("name", hw_element_name);

    if (ISEMPTY(hw_element_name))
    {
        cmd_dbal_hl_unique_hw_element(unit);
        SHR_EXIT();

    }
    /** building the table output structure */
    PRT_TITLE_SET("HW elements in DBAL Tables");

    PRT_COLUMN_ADD("Table Name");
    PRT_COLUMN_ADD("Element");
    PRT_COLUMN_ADD("Element type");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    /** running over all tables and looking for HW element that
     *  related to the input */

    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, DBAL_TABLE_EMPTY, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_HARD_LOGIC, DBAL_PHYSICAL_TABLE_NONE,
                     DBAL_TABLE_TYPE_NONE, &table_iter));

    while (table_iter != DBAL_TABLE_EMPTY)
    {
        found_match = 0;
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_iter, &table));

        for (curr_res_type = 0; curr_res_type < table->nof_result_types; curr_res_type++)
        {
            if (found_match)
            {
                break;
            }
            for (iter = 0; iter < DBAL_NOF_HL_ACCESS_TYPES; iter++)
            {
                if (found_match)
                {
                    break;
                }
                if (table->hl_mapping_multi_res[curr_res_type].l2p_hl_info[iter].num_of_access_fields > 0)
                {
                    l2p_hl_info = &(table->hl_mapping_multi_res[curr_res_type].l2p_hl_info[iter]);
                    switch (iter)
                    {
                        case DBAL_HL_ACCESS_MEMORY:
                            for (hl_iter = 0; hl_iter < l2p_hl_info->num_of_access_fields; hl_iter++)
                            {
                                if (found_match)
                                {
                                    break;
                                }
                                for (index = 0; index < DBAL_MAX_NUMBER_OF_HW_ELEMENTS; index++)
                                {
                                    if (l2p_hl_info->l2p_fields_info[hl_iter].memory[index] != INVALIDm)
                                    {
                                        if (strcaseindex
                                            (SOC_MEM_NAME
                                             (unit, l2p_hl_info->l2p_fields_info[hl_iter].memory[index]),
                                             hw_element_name))
                                        {
                                            PRT_CELL_SET("%s", table->table_name);
                                            PRT_CELL_SET("%s",
                                                         SOC_MEM_NAME(unit,
                                                                      l2p_hl_info->
                                                                      l2p_fields_info[hl_iter].memory[index]));
                                            PRT_CELL_SET("%s", "Memory");
                                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                                            found_match = 1;
                                            break;
                                        }
                                        if (l2p_hl_info->l2p_fields_info[hl_iter].alias_memory != INVALIDm)
                                        {
                                            if (strcaseindex
                                                (SOC_MEM_NAME
                                                 (unit, l2p_hl_info->l2p_fields_info[hl_iter].alias_memory),
                                                 hw_element_name))
                                            {
                                                PRT_CELL_SET("%s", table->table_name);
                                                PRT_CELL_SET("%s",
                                                             SOC_MEM_NAME(unit,
                                                                          l2p_hl_info->
                                                                          l2p_fields_info[hl_iter].alias_memory));
                                                PRT_CELL_SET("%s", "Alias Memory");
                                                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                                                found_match = 1;
                                                break;
                                            }
                                        }

                                        if (l2p_hl_info->l2p_fields_info[hl_iter].hw_field != INVALIDf)
                                        {
                                            if (strcaseindex
                                                (SOC_FIELD_NAME
                                                 (unit, l2p_hl_info->l2p_fields_info[hl_iter].hw_field),
                                                 hw_element_name))
                                            {
                                                PRT_CELL_SET("%s", table->table_name);
                                                PRT_CELL_SET("%s",
                                                             SOC_FIELD_NAME(unit,
                                                                            l2p_hl_info->
                                                                            l2p_fields_info[hl_iter].hw_field));
                                                PRT_CELL_SET("%s", "Field");
                                                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                                                found_match = 1;
                                                break;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                            }
                            break;
                        case DBAL_HL_ACCESS_REGISTER:
                            for (hl_iter = 0; hl_iter < l2p_hl_info->num_of_access_fields; hl_iter++)
                            {
                                if (found_match)
                                {
                                    break;
                                }
                                for (index = 0; index < DBAL_MAX_NUMBER_OF_HW_ELEMENTS; index++)
                                {
                                    if (l2p_hl_info->l2p_fields_info[hl_iter].reg[index] != INVALIDr)
                                    {
                                        if (strcaseindex
                                            (SOC_REG_NAME
                                             (unit, l2p_hl_info->l2p_fields_info[hl_iter].reg[index]), hw_element_name))
                                        {
                                            PRT_CELL_SET("%s", table->table_name);
                                            PRT_CELL_SET("%s",
                                                         SOC_REG_NAME(unit,
                                                                      l2p_hl_info->
                                                                      l2p_fields_info[hl_iter].reg[index]));
                                            PRT_CELL_SET("%s", "Register");
                                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                                            found_match = 1;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                if (l2p_hl_info->l2p_fields_info[hl_iter].hw_field != INVALIDf)
                                {
                                    if (strcaseindex
                                        (SOC_FIELD_NAME(unit, l2p_hl_info->l2p_fields_info[hl_iter].hw_field),
                                         hw_element_name))
                                    {
                                        PRT_CELL_SET("%s", table->table_name);
                                        PRT_CELL_SET("%s",
                                                     SOC_FIELD_NAME(unit,
                                                                    l2p_hl_info->l2p_fields_info[hl_iter].hw_field));
                                        PRT_CELL_SET("%s", "Field");
                                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                                        found_match = 1;
                                        break;
                                    }
                                }
                            }
                            break;
                        case DBAL_HL_ACCESS_SW:
                        default:
                            continue;
                    }
                }
            }
        }

        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_iter, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_HARD_LOGIC, DBAL_PHYSICAL_TABLE_NONE,
                         DBAL_TABLE_TYPE_NONE, &table_iter));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Return the next group id that matches to the input string,
* (next refers to the curr_group_id) if the subsstring is fully
* match to the group name  than is_full_match=1 use
* curr_group_id = -1 to start form the first existing group
*/
cmd_result_t
dnx_dbal_group_string_to_next_group_id_get(
    int unit,
    char *substr_match,
    dbal_hw_entity_group_e curr_group_id,
    dbal_hw_entity_group_e * group_id,
    int *is_full_match)
{
    int group_index;

    SHR_FUNC_INIT_VARS(unit);

    *group_id = DBAL_NOF_HW_ENTITY_GROUPS;
    *is_full_match = 0;

    curr_group_id++;

    if ((substr_match == NULL) || (curr_group_id > DBAL_NOF_HW_ENTITY_GROUPS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input substr_match not exists or group_id too big\n");
    }

    for (group_index = curr_group_id; group_index < DBAL_NOF_HW_ENTITY_GROUPS; group_index++)
    {
        if (!sal_strcasecmp(substr_match, dbal_hw_entity_group_to_string(unit, group_index)))
        {
            *is_full_match = 1;
            *group_id = group_index;
            break;
        }

        if (strcaseindex(dbal_hw_entity_group_to_string(unit, group_index), substr_match))
        {
            *group_id = group_index;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** returns the Group ID that matches to the string, if there is more than one Group ID that matches to the string
 *  dump all Group IDs that related if no unique Group was found return Group_id = DBAL_NOF_HW_ENTITY_GROUPS */
static shr_error_e
dnx_dbal_hw_entity_group_from_string(
    int unit,
    char *group_name,
    dbal_hw_entity_group_e * group_id,
    sh_sand_control_t * sand_control)
{
    int is_full_match = 0;
    dbal_tables_e first_group_id;
    int nof_groups_found = 0;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Possible Groups");

    PRT_COLUMN_ADD("Group Name");

    SHR_IF_ERR_EXIT(dnx_dbal_group_string_to_next_group_id_get(unit, group_name, -1, group_id, &is_full_match));
    if ((*group_id) == DBAL_NOF_HW_ENTITY_GROUPS)
    {
        LOG_CLI((BSL_META("No matching groups found\n\n")));
        SHR_EXIT();
    }

    first_group_id = (*group_id);

    while ((*group_id) != DBAL_NOF_HW_ENTITY_GROUPS)
    {
        nof_groups_found++;
        /** full match - use this group */
        if (is_full_match)
        {
            SHR_EXIT();
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dbal_hw_entity_group_to_string(unit, (*group_id)));
        SHR_IF_ERR_EXIT(dnx_dbal_group_string_to_next_group_id_get
                        (unit, group_name, (*group_id), group_id, &is_full_match));
    }

        /** only one group found use it */
    if (nof_groups_found == 1)
    {
        (*group_id) = first_group_id;
        SHR_EXIT();
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * dump all tables that use a given hw entity group
 */
static shr_error_e
diag_dbal_hl_group_related_tables_dump(
    int unit,
    dbal_hw_entity_group_e group_id,
    sh_sand_control_t * sand_control)
{
    int iter, hl_iter;
    dbal_tables_e table_iter = DBAL_TABLE_EMPTY;
    CONST dbal_logical_table_t *table;
    dbal_hl_l2p_info_t *l2p_hl_info;
    int curr_res_type = 0, index;
    int found_match = 0;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    /** building the table output structure */
    PRT_TITLE_SET("Tables including HW entity group");

    PRT_COLUMN_ADD("Table Name");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    /** running over all tables and looking for hw entity groups that
    *  related to the input */

    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, DBAL_TABLE_EMPTY, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_HARD_LOGIC, DBAL_PHYSICAL_TABLE_NONE,
                     DBAL_TABLE_TYPE_NONE, &table_iter));

    while (table_iter != DBAL_TABLE_EMPTY)
    {
        found_match = FALSE;
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_iter, &table));

        for (curr_res_type = 0; curr_res_type < table->nof_result_types; curr_res_type++)
        {
            if (found_match)
            {
                break;
            }
            for (iter = 0; iter < DBAL_NOF_HL_ACCESS_TYPES; iter++)
            {
                if (found_match)
                {
                    break;
                }
                if (table->hl_mapping_multi_res[curr_res_type].l2p_hl_info[iter].num_of_access_fields > 0)
                {
                    l2p_hl_info = &(table->hl_mapping_multi_res[curr_res_type].l2p_hl_info[iter]);
                    for (hl_iter = 0; hl_iter < l2p_hl_info->num_of_access_fields; hl_iter++)
                    {
                        if (found_match)
                        {
                            break;
                        }
                        for (index = 0; index < DBAL_MAX_NUMBER_OF_HW_ELEMENTS; index++)
                        {
                            if (l2p_hl_info->l2p_fields_info[hl_iter].hw_entity_group_id == group_id)
                            {
                                found_match = TRUE;
                                PRT_CELL_SET("%s", table->table_name);
                                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                                break;
                            }
                        }
                    }
                }
            }
        }
        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_iter, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_HARD_LOGIC, DBAL_PHYSICAL_TABLE_NONE,
                         DBAL_TABLE_TYPE_NONE, &table_iter));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/**
* \brief
* dump group information of DBAL access.
* dump all groups that has HW element (memory/register/HW_field)
* that related (substring) to the input string.
*/
static shr_error_e
cmd_dbal_hl_hw_entity_group_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    hl_group_info_t *group_info;
    dbal_hw_entity_group_e group_id;
    uint8 hw_entity_index;
    char *group_name = NULL;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("Name", group_name);

    SHR_IF_ERR_EXIT(dnx_dbal_hw_entity_group_from_string(unit, group_name, &group_id, sand_control));
    if (group_id == DBAL_NOF_HW_ENTITY_GROUPS)
    {
        SHR_EXIT();
    }

    /** building the Group output structure */
    PRT_TITLE_SET("HW elements in DBAL Group %s", dbal_hw_entity_group_to_string(unit, group_id));

    PRT_COLUMN_ADD("Element");
    PRT_COLUMN_ADD("Element type");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    SHR_IF_ERR_EXIT(dbal_tables_group_info_get(unit, group_id, &group_info));

    /** running over all group entities */

    for (hw_entity_index = 0; hw_entity_index < group_info->group_hw_entity_num; hw_entity_index++)
    {
        if (group_info->is_reg)
        {
            PRT_CELL_SET("%s", SOC_REG_NAME(unit, group_info->hw_entity_id[hw_entity_index]));
            PRT_CELL_SET("%s", "Register");
        }
        else
        {
            PRT_CELL_SET("%s", SOC_MEM_NAME(unit, group_info->hw_entity_id[hw_entity_index]));
            PRT_CELL_SET("%s", "Memory");
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    }

    PRT_COMMITX;

    /** dump all tables using hw entity group */
    SHR_IF_ERR_EXIT(diag_dbal_hl_group_related_tables_dump(unit, group_id, sand_control));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Return the next field type that matches tonthe input string,
* (next refers to the curr_field_type) if the subsstring is
* fully match to the field_type name  than is_full_match=1 use
* curr_phy_table = -1 to start form the first existing phy_table
*/
static cmd_result_t
dbal_string_to_next_physical_table_get(
    int unit,
    char *substr_match,
    dbal_physical_tables_e curr_phy_table,
    dbal_physical_tables_e * phy_table,
    uint8 *is_full_match)
{
    int iter;

    SHR_FUNC_INIT_VARS(unit);

    *phy_table = DBAL_NOF_PHYSICAL_TABLES;
    *is_full_match = 0;
    curr_phy_table++;

    if ((substr_match == NULL) || (curr_phy_table > DBAL_NOF_PHYSICAL_TABLES))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal input substr_match not exists or field_type out of range\n");
    }

    for (iter = curr_phy_table; iter < DBAL_NOF_PHYSICAL_TABLES; iter++)
    {
        char *curr_str = dbal_physical_table_to_string(unit, iter);

        if (sal_strcasecmp(substr_match, curr_str) == 0)
        {
            *is_full_match = 1;
            *phy_table = iter;
            break;
        }

        if (strcaseindex(curr_str, substr_match))
        {
            *phy_table = iter;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Print field types names fit current substring
*/
static shr_error_e
dbal_print_physical_tables_names(
    int unit,
    char *phy_table_name)
{
    dbal_physical_tables_e phy_table_id;
    uint8 found = FALSE;
    uint8 is_full_match = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    dbal_string_to_next_physical_table_get(unit, phy_table_name, -1, &phy_table_id, &is_full_match);

    LOG_CLI((BSL_META("\tOptinal physical tables are: \n")));
    while (phy_table_id != DBAL_NOF_PHYSICAL_TABLES)
    {
        found = TRUE;
        LOG_CLI((BSL_META("\t%s \n"), dbal_physical_table_to_string(unit, phy_table_id)));
        dbal_string_to_next_physical_table_get(unit, phy_table_name, phy_table_id, &phy_table_id, &is_full_match);
    }

    if (found == FALSE)
    {
        LOG_CLI((BSL_META("\tNone. Use \"dbal access mdb\" to dump all physical tables\n")));
    }
    else
    {
        LOG_CLI((BSL_META("\tNone. Use \"dbal access mdb phydb=[Phy DB name]\" to get the Phy DB info\n")));
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dbal_access_mdb_info_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int rv;
    char *phy_db_name;
    dbal_physical_tables_e phy_table_id;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("phydb", phy_db_name);

    SHR_IF_ERR_EXIT(diag_dbal_pre_action_logger_close(unit));
    rv = dbal_physical_table_string_to_id(unit, phy_db_name, &phy_table_id);
    SHR_IF_ERR_EXIT(diag_dbal_post_action_logger_restore(unit));

    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(dbal_print_physical_tables_names(unit, phy_db_name));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(diag_dbal_physical_tables_info_dump(unit, phy_table_id, sand_control));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dbal_access_mdb_shadow(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 to_skip = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("SKiP", to_skip);

    if ((to_skip != 0) && (to_skip != 1))
    {
    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal Skip value. Can be Yes(1) or No(0)")}

    SHR_IF_ERR_EXIT(dbal_db.mdb_access.skip_read_from_shadow.set(unit, to_skip));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
cmd_dbal_access_sw_info_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int table_iter, size_example_tables = 0, size_real_tables = 0, nof_example = 0;
    CONST dbal_logical_table_t *table;
    uint32 sw_nof_entries = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("SW tables info");

    PRT_COLUMN_ADD("Table Name");
    PRT_COLUMN_ADD("Maturity");
    PRT_COLUMN_ADD("Type");/** direct/non-direct*/
    PRT_COLUMN_ADD("Key size");
    PRT_COLUMN_ADD("Capacity");
    PRT_COLUMN_ADD("Entry size Bytes");
    PRT_COLUMN_ADD("Direct Size KB");
    PRT_COLUMN_ADD("Hash Size KB");

    for (table_iter = 0; table_iter < dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit); table_iter++)
    {
        int size_in_kb = 0;
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_iter, &table));

        if (table->access_method != DBAL_ACCESS_METHOD_SW_STATE)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_sw_table_nof_entries_calculate(unit, (dbal_logical_table_t *) table, &sw_nof_entries));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", table->table_name);
        PRT_CELL_SET("%s", (table->maturity_level < 2) ? "Medium" : "High");
        PRT_CELL_SET("%s", (table->table_type == DBAL_TABLE_TYPE_DIRECT) ? "direct" : "hash");
        PRT_CELL_SET("%d", table->key_size);
        PRT_CELL_SET("%d", sw_nof_entries);
        PRT_CELL_SET("%d", table->sw_access_info.sw_payload_length_bytes);
        PRT_CELL_SET("%d", (table->sw_access_info.sw_payload_length_bytes * sw_nof_entries) / 1000);
        if (table->table_type == DBAL_TABLE_TYPE_DIRECT)
        {
            PRT_CELL_SET("%d", 0);
        }
        else
        {
            size_in_kb = (DBAL_HASH_TABLE_ENTRY_SIZE * sw_nof_entries);
            PRT_CELL_SET("%d", size_in_kb / 1000);
        }

        if (strcaseindex(table->table_name, "example"))
        {
            size_example_tables += table->sw_access_info.sw_payload_length_bytes * sw_nof_entries;
            nof_example++;
        }
        else
        {
            size_real_tables += table->sw_access_info.sw_payload_length_bytes * sw_nof_entries + size_in_kb;
        }
    }

    PRT_INFO_ADD("Sizes: real Tables: %d bytes, example Tables: %d bytes", size_real_tables, size_example_tables);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dbal_access_pemla_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int pem_reg_index;
    pemladrv_reg_info *pemla_reg_info = NULL;
    int pemla_reg_info_size = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Pemla tables info");

    SHR_IF_ERR_EXIT(dnx_pemladrv_reg_info_allocate(unit, &pemla_reg_info, &pemla_reg_info_size));

    PRT_COLUMN_ADD("Table Name");
    PRT_COLUMN_ADD("IsBinded");
    PRT_COLUMN_ADD("regId");
    PRT_COLUMN_ADD("NofFields");

    for (pem_reg_index = 0; pem_reg_index < pemla_reg_info_size; pem_reg_index++)
    {
        pemladrv_reg_info *pem_reg_info_ptr = &pemla_reg_info[pem_reg_index];

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", pem_reg_info_ptr->reg_name);

        if (pem_reg_info_ptr->is_binded)
        {
            PRT_CELL_SET("Binded");
        }
        else
        {
            PRT_CELL_SET("NotBinded");
        }

        PRT_CELL_SET("%d", pem_reg_info_ptr->reg_id);

        PRT_CELL_SET("%d", pem_reg_info_ptr->nof_fields);
    }

    PRT_COMMITX;

exit:
    dnx_pemladrv_reg_info_de_allocate(unit, pemla_reg_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dbal_access_pemla_info(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *table_name;
    pemladrv_reg_info *pemla_reg_info = NULL;
    int pemla_reg_info_size;
    int pem_reg_index, pem_field_index;

    pemladrv_reg_field_info *pem_field_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get parameters
     */
    SH_SAND_GET_STR("table", table_name);

    if (ISEMPTY(table_name))
    {
        DIAG_DBAL_HEADER_DUMP("Example: dbal access pemla TaBLe=<ucode_table_name>", "\0");
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_pemladrv_reg_info_allocate(unit, &pemla_reg_info, &pemla_reg_info_size));

    /*
     * Find Pemla reg for table
     */
    for (pem_reg_index = 0; pem_reg_index < pemla_reg_info_size; pem_reg_index++)
    {
        if (sal_strcasecmp(pemla_reg_info[pem_reg_index].reg_name, table_name) == 0)
        {
            break;
        }
    }

    if (pem_reg_index == pemla_reg_info_size)
    {
        /*
         * Table info not found in Pemla driver nothing to dispay.
         */
        LOG_CLI((BSL_META("Table info not found in Pemla driver nothing to dispay\n")));
        SHR_EXIT();

    }

    PRT_TITLE_SET("%s", table_name);

    PRT_COLUMN_ADD("Field Name");
    PRT_COLUMN_ADD("HW Field Id");
    PRT_COLUMN_ADD("Field size in bits");
    for (pem_field_index = 0; pem_field_index < pemla_reg_info[pem_reg_index].nof_fields; pem_field_index++)
    {
        pem_field_info = &pemla_reg_info[pem_reg_index].ptr[pem_field_index];

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", pem_field_info->field_name);
        PRT_CELL_SET("%d", pem_field_info->field_id);
        PRT_CELL_SET("%d", pem_field_info->field_size_in_bits);
    }

    PRT_COMMITX;

exit:
    if (pemla_reg_info)
    {
        dnx_pemladrv_reg_info_de_allocate(unit, pemla_reg_info);
    }
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function is used to write PEMLA Vr-registers.
 */

static shr_error_e
cmd_dbal_access_pemla_write(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *reg_name;
    pemladrv_reg_info *pemla_reg_info = NULL;
    int pemla_reg_info_size;
    int pem_reg_index;
    uint32 *value;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get parameters
     */
    SH_SAND_GET_STR("NaMe", reg_name);

    if (ISEMPTY(reg_name))
    {
        DIAG_DBAL_HEADER_DUMP("Name is needed dbal access pemla write name=<ucode_reg_name>", "\0");
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_pemladrv_reg_info_allocate(unit, &pemla_reg_info, &pemla_reg_info_size));

    /*
     * Find Pemla reg
     */
    for (pem_reg_index = 0; pem_reg_index < pemla_reg_info_size; pem_reg_index++)
    {
        if (sal_strcasecmp(pemla_reg_info[pem_reg_index].reg_name, reg_name) == 0)
        {
            break;
        }
    }

    if (pem_reg_index == pemla_reg_info_size)
    {
        /*
         * Table info not found in Pemla driver nothing to dispay.
         */
        LOG_CLI((BSL_META("Reg name not found in Pemla driver\n")));
        SHR_EXIT();
    }

    SH_SAND_GET_ARRAY32("VaLue", value);

    SHR_IF_ERR_EXIT(dnx_pemladrv_full_reg_write(unit, pem_reg_index, -1, value));

exit:
    if (pemla_reg_info)
    {
        dnx_pemladrv_reg_info_de_allocate(unit, pemla_reg_info);
    }
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_dbal_access_hl_info_options[] = {
    {"NaMe", SAL_FIELD_TYPE_STR, "dbal access HL HWElement Name", ""},
    {NULL}
};

static sh_sand_option_t dnx_dbal_access_hl_group_options[] = {
    {"NaMe", SAL_FIELD_TYPE_STR, "dbal access HL Group Name", ""},
    {NULL}
};

static sh_sand_option_t dnx_dbal_access_mdb_info_options[] = {
    {"PhyDB", SAL_FIELD_TYPE_STR, "get Physical DB related tables and info", ""},
    {NULL}
};

static sh_sand_option_t dnx_dbal_access_mdb_shadow_options[] = {
    {"SKiP", SAL_FIELD_TYPE_BOOL, "flag to skip reads form shadow", "No"},
    {NULL}
};

static sh_sand_option_t dnx_dbal_access_pemla_status_options[] = { {NULL} };

static sh_sand_option_t dnx_dbal_access_sw_status_options[] = { {NULL} };

static sh_sand_option_t dnx_dbal_access_pemla_dump_options[] = {
    {NULL}
};

static sh_sand_option_t dnx_dbal_access_pemla_info_options[] = {
    {"TaBLe", SAL_FIELD_TYPE_STR, "get Pemla table physical DB mapping info", ""},
    {NULL}
};

static sh_sand_option_t dnx_dbal_access_pemla_write_options[] = {
    {"NaMe", SAL_FIELD_TYPE_STR, "Write Pemla Virtual register directly, by name", NULL},
    {"VaLue", SAL_FIELD_TYPE_ARRAY32, "Data to write", NULL},
    {NULL}
};

static sh_sand_man_t dnx_dbal_access_mdb_man = {
    .brief = "MDB access diagnostic operation menu",
};

static sh_sand_man_t dnx_dbal_access_hl_man = {
    .brief = "HL access diagnostic operation menu",
};

static sh_sand_man_t dnx_dbal_access_hl_info_man = {
    .brief = "dump all tables related to HW element(register/memory/field)",
    .full = "in-case that HW element was inserted dump all tables related to HW element\n"
        "otherwise dump all tables that has unique HW entity",
    .synopsis = "Name=<element_name>",
    .examples = "Name=IPPA_VTT_IN_PP_PORT_CONFIG\n" "Name=IPPE_PP_PORT_TO_VDm"
};

static sh_sand_man_t dnx_dbal_access_hl_group_man = {
    .brief = "dump all HW element(register/memory/field) related to group",
    .full = "in-case that Group name was inserted dump all hw elements related to group\n" "otherwise dump all groups ",
    .synopsis = "NaMe=<group_name>",
    .examples = "NaMe=INGRESS_VLAN_MEMBERSHIP_IF_ATTR"
};

static sh_sand_man_t dnx_dbal_access_mdb_info_man = {
    .brief = "Dump information of the PhyDB",
    .full = "Print all related logical tables\n"
        "If PhyDB is not fully matched to any physical db name, a list of the optional DBs will be printed",
    .synopsis = "PhyDB=<phy_db_name>",
    .examples = "PhyDB=INLIF_1\n" "PhyDB=ISEM2"
};

static sh_sand_man_t dnx_dbal_access_mdb_shadow_man = {
    .brief = "Contain information regarding DBAL MDB shadow access",
    .full = "MDB EM is by default read information from shadow. setting the skip flag, \n"
        "will lead to HW reads only.",
    .synopsis = "skip=Yes/No",
    .examples = "Skip=Yes\n"
};

static sh_sand_man_t dnx_dbal_access_sw_status_man = {
    .brief = "Dump information of all SW tables",
    .full = "dump all SW tables and details about them",
};

static sh_sand_man_t dnx_dbal_access_pemla_status_man = {
    .brief = "Dump information of all PEMLA tables",
    .full = "dump all PEMLA tables and details about them",
};

static sh_sand_man_t dnx_dbal_access_pemla_dump_man = {
    .brief = "Dump information of all PEMLA tables",
    .full = "dump all PEMLA tables and details about them",
};

static sh_sand_man_t dnx_dbal_access_pemla_info_man = {
    .brief = "Dump information of a PEMLA table",
    .full = "dump full PEMLA table details ",
    .synopsis = "TaBLe=<ucode tabel name>",
    .examples = "TaBLe=PEMLA_vrTRAP"
};

static sh_sand_man_t dnx_dbal_access_pemla_write_man = {
    .brief = "Write register",
    .full = "Write full virtual register from ucode",
    .synopsis = "NaMe=<string>, VaLue=<int>",
    .examples = "NaMe=PEMLA_vrTRAP VaLue=1"
};

sh_sand_cmd_t sh_dnx_dbal_access_hl_cmds[] = {
  /*******************************************************************************************************************
   * CMD_NAME *     CMD_ACTION             * Next  *        Options                  *            MAN                *
   *          *                            * Level *                                 *                               *
   *          *                            * CMD   *                                 *                               *
 */
    {"HWElement", cmd_dbal_hl_info_dump, NULL, dnx_dbal_access_hl_info_options, &dnx_dbal_access_hl_info_man},
    {"Group", cmd_dbal_hl_hw_entity_group_dump, NULL, dnx_dbal_access_hl_group_options, &dnx_dbal_access_hl_group_man},
    {NULL}
};

sh_sand_cmd_t sh_dnx_dbal_access_mdb_cmds[] = {
  /*******************************************************************************************************************
   * CMD_NAME *     CMD_ACTION             * Next  *        Options                  *            MAN                *
   *          *                            * Level *                                 *                               *
   *          *                            * CMD   *                                 *                               *
 */
    {"Info", cmd_dbal_access_mdb_info_dump, NULL, dnx_dbal_access_mdb_info_options, &dnx_dbal_access_mdb_info_man},
    {"ShaDoW", cmd_dbal_access_mdb_shadow, NULL, dnx_dbal_access_mdb_shadow_options, &dnx_dbal_access_mdb_shadow_man},
    {NULL}
};

sh_sand_cmd_t diag_dbal_access_pemala_cmds[] = {
   /******************************************************************************************************************
    * CMD    *     CMD_ACTION           * Next *        Options               *            MAN             * CB      *
    * NAME   *                          * Level*                              *                            *         *
    *        *                          * CMD  *                              *                            *         *
 */
    {"Dump", cmd_dbal_access_pemla_dump, NULL, dnx_dbal_access_pemla_dump_options, &dnx_dbal_access_pemla_dump_man},
    {"Info", cmd_dbal_access_pemla_info, NULL, dnx_dbal_access_pemla_info_options, &dnx_dbal_access_pemla_info_man},
    {"Write", cmd_dbal_access_pemla_write, NULL, dnx_dbal_access_pemla_write_options, &dnx_dbal_access_pemla_write_man},
    {NULL}
};

sh_sand_cmd_t sh_dnx_dbal_access_cmds[] = {
  /*******************************************************************************************************************
   * CMD_NAME *     CMD_ACTION             * Next  *        Options                  *            MAN                *
   *          *                            * Level *                                 *                               *
   *          *                            * CMD   *                                 *                               *
 */
    {"HL", NULL, sh_dnx_dbal_access_hl_cmds, NULL, &dnx_dbal_access_hl_man, NULL,
     NULL, SH_CMD_NO_XML_VERIFY},
    {"MDB", NULL, sh_dnx_dbal_access_mdb_cmds, NULL, &dnx_dbal_access_mdb_man, NULL,
     NULL, SH_CMD_NO_XML_VERIFY},
    {"SW", cmd_dbal_access_sw_info_dump, NULL, dnx_dbal_access_sw_status_options, &dnx_dbal_access_sw_status_man, NULL,
     NULL, SH_CMD_NO_XML_VERIFY},
    {"PEMLA", NULL, diag_dbal_access_pemala_cmds, dnx_dbal_access_pemla_status_options,
     &dnx_dbal_access_pemla_status_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {NULL}
};
/**********************************************************************************************************************
 *  DBAL DIAGNOSTIC PACK:
 *  DBAL MAIN MENU function & infrastructure Definitions - START
 *  STURCTURE:
 *  1. cmd function definition
 *  2. cmd option and man table
 *  3. DBAL MAIN MENU cmd table
 */
extern shr_error_e dbal_hl_pre_access_calculate(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_hl_l2p_field_info_t * curr_l2p_info,
    uint8 *is_condition_pass,
    uint32 *mem_array_offset,
    uint32 *mem_entry_offset,
    uint32 *field_offset,
    uint32 *group_offset);

shr_error_e
related_entry_search_register(
    int unit,
    dbal_tables_e table_id,
    char *hw_element_name,
    int hw_entry_offset,
    uint32 entry_handle_id,
    int *is_found)
{
    dbal_entry_handle_t *entry_handle;
    int is_end, iter;
    dbal_hl_l2p_info_t *l2p_hl_info;
    dbal_hl_l2p_field_info_t *curr_l2p_info = NULL;
    uint32 field_offset = 0, reg_array_offset = 0, group_offset;
    uint8 is_condition_pass = 0;
    soc_reg_t curr_reg;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    (*is_found) = 0;

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    table = entry_handle->table;

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    l2p_hl_info = &table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_REGISTER];

    while (!is_end)
    {
        for (iter = 0; iter < l2p_hl_info->num_of_access_fields; iter++)
        {
            curr_l2p_info = &l2p_hl_info->l2p_fields_info[iter];

            SHR_IF_ERR_EXIT(dbal_hl_pre_access_calculate(unit, entry_handle, curr_l2p_info, &is_condition_pass,
                                                         &reg_array_offset, NULL, &field_offset, &group_offset));
            if (is_condition_pass)
            {
                curr_reg = (group_offset == -1) ? curr_l2p_info->reg[0] : curr_l2p_info->reg[group_offset];

                if (strcaseindex(SOC_REG_NAME(unit, curr_reg), hw_element_name))
                {
                     /** related entry found */
                    (*is_found) = 1;
                    break;
                }
            }
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
related_entry_search_memory(
    int unit,
    dbal_tables_e table_id,
    char *hw_element_name,
    int hw_entry_offset,
    uint32 entry_handle_id,
    int *is_found)
{
    dbal_entry_handle_t *entry_handle;
    int is_end, iter;
    dbal_hl_l2p_info_t *l2p_hl_info;
    dbal_hl_l2p_field_info_t *curr_l2p_info = NULL;
    uint32 field_offset = 0, reg_array_offset = 0, group_offset, mem_offset;
    uint8 is_condition_pass = 0;
    soc_mem_t curr_mem;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    (*is_found) = 0;

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    table = entry_handle->table;

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    l2p_hl_info = &table->hl_mapping_multi_res[entry_handle->cur_res_type].l2p_hl_info[DBAL_HL_ACCESS_MEMORY];

    while (!is_end)
    {
        for (iter = 0; iter < l2p_hl_info->num_of_access_fields; iter++)
        {
            curr_l2p_info = &l2p_hl_info->l2p_fields_info[iter];

            SHR_IF_ERR_EXIT(dbal_hl_pre_access_calculate(unit, entry_handle, curr_l2p_info, &is_condition_pass,
                                                         &reg_array_offset, &mem_offset, &field_offset, &group_offset));
            if (is_condition_pass)
            {
                curr_mem = (group_offset == -1) ? curr_l2p_info->memory[0] : curr_l2p_info->memory[group_offset];
                if (strcaseindex(SOC_MEM_NAME(unit, curr_mem), hw_element_name))
                {
                    if (mem_offset == hw_entry_offset)
                    {
                        /** related entry found */
                        (*is_found) = 1;
                        break;
                    }
                }
            }
        }

        if ((*is_found) == 1)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief enum represents the handle status
 */
typedef enum
{
    DBAL_HW_ELEMENT_TYPE_NONE,
    DBAL_HW_ELEMENT_TYPE_REGISTER,
    DBAL_HW_ELEMENT_TYPE_MEMORY,
    DBAL_HW_ELEMENT_TYPE_FOUND_MEM_ENTRY_NOT_FOUND,
    DBAL_HW_ELEMENT_TYPE_FOUND_REG_ENTRY_NOT_FOUND,

    DBAL_NOF_ELEMENT_TYPE
} dbal_diag_hw_element_type_e;

/** running over all the access fields in Memory/register and checking if the hw_element_name == mem/reg name and
 *  returning the type dbal_diag_hw_element_type_e related. incase the handle ID is not -1, calling
 *  related_entry_search_xxx to find the exact entry related to the mem/reg (it is returned in the handle) */
shr_error_e
diag_dbal_is_hw_element_related_to_table(
    int unit,
    dbal_tables_e table_id,
    char *hw_element_name,
    int hw_entry_offset,
    uint32 entry_handle_id,
    dbal_diag_hw_element_type_e * hw_type)
{

    int iter, hl_iter, curr_res_type, index;
    dbal_hl_l2p_info_t *l2p_hl_info;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    (*hw_type) = DBAL_HW_ELEMENT_TYPE_NONE;

    /** building the table output structure */

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    for (curr_res_type = 0; curr_res_type < table->nof_result_types; curr_res_type++)
    {
        for (iter = 0; iter < DBAL_NOF_HL_ACCESS_TYPES; iter++)
        {
            if (table->hl_mapping_multi_res[curr_res_type].l2p_hl_info[iter].num_of_access_fields > 0)
            {
                l2p_hl_info = &(table->hl_mapping_multi_res[curr_res_type].l2p_hl_info[iter]);
                switch (iter)
                {
                    case DBAL_HL_ACCESS_MEMORY:
                        for (hl_iter = 0; hl_iter < l2p_hl_info->num_of_access_fields; hl_iter++)
                        {
                            for (index = 0; index < DBAL_MAX_NUMBER_OF_HW_ELEMENTS; index++)
                            {
                                if (l2p_hl_info->l2p_fields_info[hl_iter].memory[index] != INVALIDm)
                                {
                                    if (strcaseindex
                                        (SOC_MEM_NAME(unit, l2p_hl_info->l2p_fields_info[hl_iter].memory[index]),
                                         hw_element_name))
                                    {
                                        int is_found;
                                        (*hw_type) = DBAL_HW_ELEMENT_TYPE_FOUND_MEM_ENTRY_NOT_FOUND;
                                        if (entry_handle_id != -1)
                                        {
                                            SHR_IF_ERR_EXIT(related_entry_search_memory(unit, table_id, hw_element_name,
                                                                                        hw_entry_offset,
                                                                                        entry_handle_id, &is_found));
                                            if (is_found)
                                            {
                                                (*hw_type) = DBAL_HW_ELEMENT_TYPE_MEMORY;
                                                SHR_EXIT();
                                            }
                                        }
                                    }
                                    if (l2p_hl_info->l2p_fields_info[hl_iter].alias_memory != INVALIDm)
                                    {
                                        if (strcaseindex
                                            (SOC_MEM_NAME(unit, l2p_hl_info->l2p_fields_info[hl_iter].alias_memory),
                                             hw_element_name))
                                        {
                                            (*hw_type) = DBAL_HW_ELEMENT_TYPE_MEMORY;
                                            SHR_EXIT();
                                        }
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }
                        break;
                    case DBAL_HL_ACCESS_REGISTER:
                        for (hl_iter = 0; hl_iter < l2p_hl_info->num_of_access_fields; hl_iter++)
                        {
                            for (index = 0; index < DBAL_MAX_NUMBER_OF_HW_ELEMENTS; index++)
                            {
                                if (l2p_hl_info->l2p_fields_info[hl_iter].reg[index] != INVALIDr)
                                {
                                    if (strcaseindex
                                        (SOC_REG_NAME(unit, l2p_hl_info->l2p_fields_info[hl_iter].reg[index]),
                                         hw_element_name))
                                    {
                                        int is_found;
                                        (*hw_type) = DBAL_HW_ELEMENT_TYPE_FOUND_REG_ENTRY_NOT_FOUND;
                                        if (entry_handle_id != -1)
                                        {
                                            SHR_IF_ERR_EXIT(related_entry_search_register
                                                            (unit, table_id, hw_element_name, hw_entry_offset,
                                                             entry_handle_id, &is_found));
                                            if (is_found)
                                            {
                                                (*hw_type) = DBAL_HW_ELEMENT_TYPE_REGISTER;
                                                SHR_EXIT();
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }
                        break;
                    case DBAL_HL_ACCESS_SW:
                    default:
                        continue;
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_nof_occupied_dynamic_tables_get(
    int unit,
    int *nof_occupied_tables)
{
    dbal_table_status_e table_status;
    int table_id;

    SHR_FUNC_INIT_VARS(unit);

    (*nof_occupied_tables) = 0;

    for (table_id = DBAL_NOF_TABLES; table_id < dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit); table_id++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
        if (table_status != DBAL_TABLE_NOT_INITIALIZED)
        {
            (*nof_occupied_tables)++;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

typedef enum
{
    key_size,
    nof_key_fields,
    res_size,
    nof_res_types,
    nof_res_fields_per_rt,
    total_nof_res_fields,
    nof_table_params,
} table_params_e;

typedef enum
{
    struct_fields,
    defines,
    child_fields,
    enum_values,
    nof_field_params,
} field_params_e;

static shr_error_e
cmd_dbal_statistics_set(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int used_dynamic_fields, used_dynamic_tables, ii;
    int table_id, total_sw_alloc = 0, total_res_fields = 0, total_non_direct = 0;
    CONST dbal_logical_table_t *table;
    CONST dbal_field_types_basic_info_t *field_type_info;
    uint32 sw_nof_entries = 0;
    int mutex_id;

    char *table_param_names[] = { "key size", "nof key fields", "res size", "nof result types", "nof res fields per RT",
        "total nof res fields"
    };
    char *field_param_names[] = { "struct fields", "defines", "child fields", "enum values" };
    int table_params[nof_table_params] = { 0 };
    int field_params[nof_field_params] = { 0 };
    int table_ids[nof_table_params] = { 0 };
    int field_ids[nof_field_params] = { 0 };
    int table_max_val[] = { DBAL_PHYSICAL_KEY_SIZE_IN_BITS, DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS,
        DBAL_PHYSICAL_RES_SIZE_IN_BITS, DBAL_MAX_NUMBER_OF_RESULT_TYPES,
        DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE, 0
    };
    int field_max_val[] = { DBAL_FIELD_MAX_NUM_OF_STRUCT_FIELDS, DBAL_FIELD_MAX_NUM_OF_DEFINE_VALUES,
        DBAL_FIELD_MAX_NUM_OF_CHILD_FIELDS, DBAL_FIELD_MAX_NUM_OF_ENUM_VALUES
    };

    SHR_FUNC_INIT_VARS(unit);

    DIAG_DBAL_HEADER_DUMP("DBAL Statistics info", "\0");

    SHR_IF_ERR_EXIT(dbal_fields_nof_occupied_dynamic_fields_get(unit, &used_dynamic_fields));
    SHR_IF_ERR_EXIT(dbal_fields_nof_occupied_dynamic_tables_get(unit, &used_dynamic_tables));

    LOG_CLI((BSL_META("General:\n")));
    LOG_CLI((BSL_META("Device DBAL state: %s\n"),
             dbal_device_state_to_string(unit, dnx_data_dbal.db_init.dbal_device_state_get(unit))));
    LOG_CLI((BSL_META("DBAL entry handle size in Bytes: %d\n"), (int) (sizeof(dbal_entry_handle_t))));

    LOG_CLI((BSL_META("Per table mutex total available: %d\n"), DBAL_NOF_SHARED_TABLE_MUTEXES));
    for (mutex_id = 0; mutex_id < DBAL_NOF_SHARED_TABLE_MUTEXES; mutex_id++)
    {
        CONST dbal_specific_table_protection_info_t *protection_info;
        SHR_IF_ERR_EXIT(dbal_per_table_mutex_info_get(unit, mutex_id, &protection_info));
        if (protection_info->table_id != DBAL_TABLE_EMPTY)
        {
            LOG_CLI((BSL_META("/tMutex ID %d realated to table %s\n"), mutex_id,
                     dbal_logical_table_to_string(unit, protection_info->table_id)));
        }
    }

    LOG_CLI((BSL_META("\nTables:\n")));
    LOG_CLI((BSL_META("Number of static tables %d\n"), DBAL_NOF_TABLES));
    LOG_CLI((BSL_META("Number of dynamic tables %d out of %d\n"), used_dynamic_tables,
             dnx_data_dbal.table.nof_dynamic_tables_get(unit)));
    LOG_CLI((BSL_META("Pre-Allocated table size in bytes %d (not is SW state)\n"), (int) sizeof(dbal_logical_table_t)));
    LOG_CLI((BSL_META("table properties SW state size in bytes (per table) %d\n"),
             (int) sizeof(dbal_sw_state_table_properties_t)));

    LOG_CLI((BSL_META("\nTable Sizes:\n")));
    for (table_id = 0; table_id < dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit); table_id++)
    {
        dbal_table_status_e table_status;
        total_res_fields = 0;

        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

        if ((table_status == DBAL_TABLE_NOT_INITIALIZED))
        {
            continue;
        }

        if (table->sw_access_info.sw_payload_length_bytes)
        {
            SHR_IF_ERR_EXIT(dbal_sw_table_nof_entries_calculate(unit, (dbal_logical_table_t *) table, &sw_nof_entries));
            total_sw_alloc += sw_nof_entries * table->sw_access_info.sw_payload_length_bytes;
            if (table->table_type != DBAL_TABLE_TYPE_DIRECT)
            {
                total_non_direct += sw_nof_entries * DBAL_HASH_TABLE_ENTRY_SIZE;
            }
        }

        if (table->key_size > table_params[key_size])
        {
            table_ids[key_size] = table_id;
            table_params[key_size] = table->key_size;
        }
        if (table->nof_key_fields > table_params[nof_key_fields])
        {
            table_ids[nof_key_fields] = table_id;
            table_params[nof_key_fields] = table->nof_key_fields;
        }
        if (table->max_payload_size > table_params[res_size])
        {
            table_ids[res_size] = table_id;
            table_params[res_size] = table->max_payload_size;
        }
        if (table->nof_result_types > table_params[nof_res_types])
        {
            table_ids[nof_res_types] = table_id;
            table_params[nof_res_types] = table->nof_result_types;
        }

        for (ii = 0; ii < table->nof_result_types; ii++)
        {
            total_res_fields += table->multi_res_info[ii].nof_result_fields;
            if (table->multi_res_info[ii].nof_result_fields > table_params[nof_res_fields_per_rt])
            {
                table_ids[nof_res_fields_per_rt] = table_id;
                table_params[nof_res_fields_per_rt] = table->max_nof_result_fields;
            }
        }

        if (total_res_fields > table_params[total_nof_res_fields])
        {
            table_ids[total_nof_res_fields] = table_id;
            table_params[total_nof_res_fields] = total_res_fields;
        }

    }

    LOG_CLI((BSL_META("Direct SW state allocated bytes for table entries: %d KB\n"), total_sw_alloc / 1000));
    LOG_CLI((BSL_META("Hash SW state allocated bytes for table entries: %d KB\n"), total_non_direct / 1000));
    for (ii = 0; ii < nof_table_params; ii++)
    {
        LOG_CLI((BSL_META("Max %s is %d in table %s - max define value %d\n"), table_param_names[ii], table_params[ii],
                 dbal_logical_table_to_string(unit, table_ids[ii]), table_max_val[ii]));
    }

    LOG_CLI((BSL_META("\nFields:\n")));
    LOG_CLI((BSL_META("Number of field types %d\n"), DBAL_NOF_FIELD_TYPES_DEF));
    LOG_CLI((BSL_META("Number of static fields %d\n"), DBAL_NOF_FIELDS));
    LOG_CLI((BSL_META("Number of dynamic fields %d out of %d\n"), used_dynamic_fields, DBAL_NOF_DYNAMIC_FIELDS));
    LOG_CLI((BSL_META("Pre-Allocated Field type size in bytes %d (not is SW state)\n"),
             (int) sizeof(dbal_field_types_basic_info_t)));
    LOG_CLI((BSL_META("Field Type  properties SW state size (per type) %d\n"), (int) sizeof(dbal_dynamic_fields_t)));
    LOG_CLI((BSL_META("\nFields Sizes:\n")));

    for (ii = DBAL_FIELD_TYPE_DEF_EMPTY + 1; ii < DBAL_NOF_FIELD_TYPES_DEF; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, ii, &field_type_info));

        if (field_type_info->nof_child_fields > field_params[child_fields])
        {
            field_ids[child_fields] = ii;
            field_params[child_fields] = field_type_info->nof_child_fields;
        }
        if (field_type_info->nof_struct_fields > field_params[struct_fields])
        {
            field_ids[struct_fields] = ii;
            field_params[struct_fields] = field_type_info->nof_struct_fields;
        }
        if (field_type_info->nof_enum_values > field_params[enum_values])
        {
            if (ii != DBAL_FIELD_TYPE_DEF_INGRESS_TRAP_ID)
            {
                field_ids[enum_values] = ii;
                field_params[enum_values] = field_type_info->nof_enum_values;
            }
        }
        if (field_type_info->nof_defines_values > field_params[defines])
        {
            field_ids[defines] = ii;
            field_params[defines] = field_type_info->nof_defines_values;
        }
    }

    for (ii = 0; ii < nof_field_params; ii++)
    {
        LOG_CLI((BSL_META("Max %s is %d in field type %s - max code define value %d\n"), field_param_names[ii],
                 field_params[ii], dbal_field_types_def_to_string(unit, field_ids[ii]), field_max_val[ii]));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* used for hit indications
* valid parameters:  candidate, table_name, hw_entity
*/
static shr_error_e
cmd_dbal_last_set(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
#ifdef ADAPTER_SERVER_MODE
    rhlist_t *hit_list;
    rhlist_t *hit_list_first;
    dbal_tables_e table_iter;
    shr_hit_entry_t *hit_entry;
    CONST dbal_logical_table_t *table;
    uint32 entry_handle_id, hw_entity_valid = 0;
    uint8 candidate = FALSE;
    char *table_name, *hw_entity;
    dbal_tables_e table_id = DBAL_NOF_TABLES;
    PRT_INIT_VARS;
#endif

    SHR_FUNC_INIT_VARS(unit);

#ifndef ADAPTER_SERVER_MODE
    SHR_ERR_EXIT(_SHR_E_PARAM, "API not supported when not working in adapter \n");
#else

    DBAL_FUNC_INIT_VARS(unit);
    SH_SAND_GET_BOOL("candidate", candidate);
    SH_SAND_GET_STR("Table", table_name);
    SH_SAND_GET_STR("HWElement", hw_entity);

    if (!ISEMPTY(hw_entity))
    {
        hw_entity_valid = 1;
    }

    if (!ISEMPTY(table_name))
    {
        int is_full_match;
        diag_dbal_table_string_to_next_table_id_get(unit, table_name, -1, &table_id, &is_full_match);
        if (table_id == DBAL_NOF_TABLES)
        {
            LOG_CLI((BSL_META("no matching tables found\n\n")));
        }
        else
        {
            if (is_full_match)
            {
                candidate = TRUE;
            }
            else
            {
                LOG_CLI((BSL_META("More than one match to table name try be more specific, showing result for %s\n\n"),
                         dbal_logical_table_to_string(unit, table_id)));
            }
        }

    }

    /**running over all tables and dump the matches to the hit signals */
    if (table_id == DBAL_NOF_TABLES)
    {
        int is_valid = 0;
        PRT_TITLE_SET("Last packet hitbit tables");
        PRT_COLUMN_ADD("Table Name");
        PRT_COLUMN_ADD("HW Element hit");
        PRT_COLUMN_ADD("Hw entry");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        SHR_IF_ERR_EXIT(shr_access_hit_get(unit, &hit_list_first, HIT_AVOID_REPEAT));

        hit_list = hit_list_first;
        RHITERATOR(hit_entry, hit_list)
        {
            is_valid = 0;
            if (hw_entity_valid)
            {
                if (strcaseindex(RHNAME(hit_entry), hw_entity))
                {
                    is_valid = 1;
                }
            }
            else
            {
                is_valid = 1;
            }
            if (strcaseindex(RHNAME(hit_entry), "_CONTEXT_SELECTION_CAM"))
            {
                is_valid = 0;
            }
            if (is_valid)
            {
                for (table_iter = 0; table_iter < DBAL_NOF_TABLES; table_iter++)
                {
                    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_iter, &table));
                    if (table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC && table->maturity_level == 2)
                    {
                        dbal_diag_hw_element_type_e hw_type = DBAL_HW_ELEMENT_TYPE_NONE;
                        SHR_IF_ERR_EXIT(diag_dbal_is_hw_element_related_to_table
                                        (unit, table_iter, RHNAME(hit_entry), hit_entry->offset, -1, &hw_type));
                        if (hw_type != DBAL_HW_ELEMENT_TYPE_NONE)
                        {
                            PRT_CELL_SET(" %s", table->table_name);
                            PRT_CELL_SET("%s%s", RHNAME(hit_entry),
                                         (hw_type == DBAL_HW_ELEMENT_TYPE_FOUND_REG_ENTRY_NOT_FOUND) ? "r" : "m");
                            PRT_CELL_SET(" %d", hit_entry->offset);
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        }
                    }
                }
            }
        }

    }
    else
    {
        int field_iter;
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
            /** building the table output structure */
        PRT_TITLE_SET("Last packet hitbit tables");

        PRT_COLUMN_ADD("Table Name");
        PRT_COLUMN_ADD("HW Element hit");
        PRT_COLUMN_ADD("Block");
        PRT_COLUMN_ADD("HW entry");

        for (field_iter = 0; field_iter < table->nof_key_fields; field_iter++)
        {
            PRT_COLUMN_ADD("%s", dbal_field_to_string(unit, table->keys_info[field_iter].field_id));
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        SHR_IF_ERR_EXIT(shr_access_hit_get(unit, &hit_list, HIT_AVOID_REPEAT));

        RHITERATOR(hit_entry, hit_list)
        {
            if (table->access_method != DBAL_ACCESS_METHOD_HARD_LOGIC || table->maturity_level < 2)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "cannot perform action for table %s \n", table->table_name);
            }
            else
            {
                dbal_diag_hw_element_type_e hw_type = DBAL_HW_ELEMENT_TYPE_NONE;
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
                SHR_IF_ERR_EXIT(diag_dbal_is_hw_element_related_to_table(unit, table_id, RHNAME(hit_entry),
                                                                         hit_entry->offset, entry_handle_id, &hw_type));
                if (hw_type != DBAL_HW_ELEMENT_TYPE_NONE)
                {
                    if (hw_type == DBAL_HW_ELEMENT_TYPE_FOUND_MEM_ENTRY_NOT_FOUND
                        || hw_type == DBAL_HW_ELEMENT_TYPE_FOUND_REG_ENTRY_NOT_FOUND)
                    {
                        /** in thi case couldnwt find an entry that feats to the HW hit but the memory related to the table */
                        if (candidate)
                        {
                            PRT_CELL_SET(" Candidate %s", table->table_name);
                            PRT_CELL_SET("%s", RHNAME(hit_entry));
                            PRT_CELL_SET("%s", hit_entry->block_n);
                            PRT_CELL_SET("%d", hit_entry->offset);

                            for (field_iter = 0; field_iter < table->nof_key_fields; field_iter++)
                            {
                                PRT_CELL_SET(" ??? ");
                            }
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        }
                    }
                    else
                    {
                        PRT_CELL_SET("%s", table->table_name);
                        PRT_CELL_SET("%s%s", RHNAME(hit_entry), (hw_type == DBAL_HW_ELEMENT_TYPE_REGISTER) ? "r" : "m");
                        PRT_CELL_SET("%s", hit_entry->block_n);
                        PRT_CELL_SET("%d", hit_entry->offset);

                        for (field_iter = 0; field_iter < table->nof_key_fields; field_iter++)
                        {
                            char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
                            uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

                            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id,
                                                                                  table->keys_info[field_iter].field_id,
                                                                                  field_val));

                            SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get
                                            (unit, table->keys_info[field_iter].field_id, table_id, field_val,
                                             NULL, 0, TRUE, FALSE, buffer_to_print));

                            PRT_CELL_SET("%s ", buffer_to_print);
                        }
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    }
                }
                DBAL_HANDLE_FREE(unit, entry_handle_id);
            }
        }
        LOG_CLI((BSL_META("\n\n")));
    }
    PRT_COMMITX;
#endif

exit:
#ifdef ADAPTER_SERVER_MODE
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
#endif
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dbal_logger_status_print(
    int unit,
    sh_sand_control_t * sand_control)
{
    CONST dbal_logger_info_t *logger_info;
    bsl_severity_t curr_api_severity;
    bsl_severity_t curr_access_severity;
    bsl_severity_t curr_diag_severity;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    dbal_logger_info_get_internal(unit, &logger_info);
    curr_api_severity = bslenable_get(bslLayerSocdnx, bslSourceDbaldnx);
    curr_access_severity = bslenable_get(bslLayerSocdnx, bslSourceDbalaccessdnx);
    curr_diag_severity = bslenable_get(bslLayerSocdnx, bslSourceDiagdbaldnx);

    PRT_TITLE_SET("Logger status");

    PRT_COLUMN_ADD("API severity");
    PRT_COLUMN_ADD("Access severity");
    PRT_COLUMN_ADD("Diag severity");
    PRT_COLUMN_ADD("File");
    PRT_COLUMN_ADD("Mode");
    PRT_COLUMN_ADD("Table locked");
    PRT_COLUMN_ADD("Internal locked");
    PRT_COLUMN_ADD("Flags");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("%s", bsl_severity2str(curr_api_severity));
    PRT_CELL_SET("%s", bsl_severity2str(curr_access_severity));
    PRT_CELL_SET("%s", bsl_severity2str(curr_diag_severity));
    PRT_CELL_SET("%s", (logger_info->dbal_file != NULL) ? "ON" : "OFF");
    if (logger_info->logger_mode == DBAL_LOGGER_MODE_REGULAR)
    {
        PRT_CELL_SET("Regular");
    }
    else
    {
        PRT_CELL_SET("Write only");
    }
    if (logger_info->user_log_locked_table != DBAL_TABLE_EMPTY)
    {
        PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, logger_info->user_log_locked_table));
    }
    else
    {
        PRT_CELL_SET("%s", "NONE");
    }

    if (logger_info->internal_log_locked_table != DBAL_TABLE_EMPTY)
    {
        PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, logger_info->internal_log_locked_table));
    }
    else
    {
        PRT_CELL_SET("%s", "NONE");
    }

    PRT_CELL_SET("%s", logger_info->disable_logger ? "Disabled" : "None");

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Change the severity of the DBAL module, or lock the logger to specific table
* if no parameter than dump the current logger info
 */
static shr_error_e
cmd_dbal_logger_set(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_dbal_logger_type_e dbal_logger_type;
    bsl_severity_t severity = bslSeverityOff;
    uint8 print_status = FALSE;
    char *table_name = NULL, *file_name = NULL, *mode = NULL;
    dbal_tables_e table_id = DBAL_NOF_TABLES;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * coverity[mixed_enums]
     */
    SH_SAND_GET_ENUM("Type", dbal_logger_type);
    /*
     * coverity[mixed_enums]
     */
    SH_SAND_GET_ENUM("Severity", severity);
    SH_SAND_GET_STR("Table", table_name);
    SH_SAND_GET_STR("File", file_name);
    SH_SAND_GET_BOOL("Status", print_status);
    SH_SAND_GET_STR("Mode", mode);

    if (!ISEMPTY(file_name))
    {
        if (sal_strcasecmp("close", file_name) == 0)
        {
            SHR_IF_ERR_EXIT(dbal_logger_file_close(unit));
            LOG_CLI((BSL_META(" logger filed closed.\n")));
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_logger_file_open(unit, file_name));
            SHR_EXIT();
        }
    }

    if (!ISEMPTY(table_name))
    {
        if (sal_strcasecmp("off", table_name) == 0)
        {
            dbal_logger_table_lock(unit, DBAL_TABLE_EMPTY);
            LOG_CLI((BSL_META(" logger released.\n")));
            SHR_EXIT();
        }

        if (dbal_logical_table_string_to_id(unit, table_name, &table_id) == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "No table found matching to string %s \n", table_name);
        }
        else
        {
            dbal_logger_table_lock(unit, table_id);
            bslenable_set(bslLayerSocdnx, bslSourceDbaldnx, bslSeverityInfo);
            bslenable_set(bslLayerSocdnx, bslSourceDbalaccessdnx, bslSeverityInfo);
            LOG_CLI((BSL_META("Logger is Locked on table %s with severity info\n"),
                     dbal_logical_table_to_string(unit, table_id)));
            SHR_EXIT();
        }
    }

    if (!ISEMPTY(mode))
    {
        if (sal_strcasecmp("write", mode) == 0)
        {
            dbal_logger_table_mode_set(unit, DBAL_LOGGER_MODE_WRITE_ONLY);
            LOG_CLI((BSL_META(" logger mode set to write only.\n")));
        }

        if (sal_strcasecmp("regular", mode) == 0)
        {
            dbal_logger_table_mode_set(unit, DBAL_LOGGER_MODE_REGULAR);
            LOG_CLI((BSL_META(" logger mode set to regular.\n")));
        }
    }

    if (print_status == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_logger_status_print(unit, sand_control));
        SHR_EXIT();
    }

    if ((severity != DNX_DBAL_LOGGER_PRINT))
    {
        if ((dbal_logger_type == DNX_DBAL_LOGGER_TYPE_API) || (dbal_logger_type == DNX_DBAL_LOGGER_TYPE_LAST))
        {
            bslenable_set(bslLayerSocdnx, bslSourceDbaldnx, severity);
        }
        if ((dbal_logger_type == DNX_DBAL_LOGGER_TYPE_ACCESS) || (dbal_logger_type == DNX_DBAL_LOGGER_TYPE_LAST))
        {
            bslenable_set(bslLayerSocdnx, bslSourceDbalaccessdnx, severity);
        }
        else if (dbal_logger_type == DNX_DBAL_LOGGER_TYPE_DIAG)
        {
            bslenable_set(bslLayerSocdnx, bslSourceDiagdbaldnx, severity);
        }

        if (dbal_logger_type == DNX_DBAL_LOGGER_TYPE_LAST)
        {
            LOG_CLI((BSL_META("Logger severity %s for type API and ACCESS was set\n"), bsl_severity2str(severity)));
        }
        else
        {
            LOG_CLI((BSL_META("Logger severity %s for type %s was set\n"), bsl_severity2str(severity),
                     dbal_logger_type_to_string(unit, dbal_logger_type)));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** \brief prints the result of dbal decoding */
shr_error_e
dnx_dbal_decode_print(
    int unit,
    rhlist_t * parsed_info,
    sh_sand_control_t * sand_control)
{
    int field_col_id;
    signal_output_t *field;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Parsed entries");

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &field_col_id, "Type");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, field_col_id, NULL,
                    "Decoded Value - EnumVal/ChildField/Define/Struct member value");

    RHITERATOR(field, parsed_info)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", field->entry.name);
        PRT_CELL_SET("%s", field->print_value);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *      Implementation of the 'dbal decode' diag command.
 *  \see
 *      dbal_decode
 */
static shr_error_e
cmd_dbal_decode(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 *data;
    uint32 size;
    char *field_name;
    int field_col_id;
    signal_output_t *field;

    rhlist_t *parsed_info = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Read parameters
     */
    SH_SAND_GET_ARRAY32("data", data);
    SH_SAND_GET_UINT32("size", size);
    SH_SAND_GET_STR("name", field_name);

    /*
     * Call parsing service
     */
    SHR_IF_ERR_EXIT(dbal_decode(unit, SOC_CORE_DEFAULT, field_name, data, size, &parsed_info, NULL, NULL));

    if (NULL == parsed_info)
    {
        /*
         * Decoding is unsuccessful. Return an error.
         */
        SHR_ERR_EXIT(_SHR_E_FAIL, "Could not find decoding for dbal field: %s and data: %d\n", field_name, *data);
    }

    LOG_CLI((BSL_META("Decoding data=%d according to field type name=%s\n"), *data, field_name));
    /*
     * Print result list
     */
    PRT_TITLE_SET("Parsed entries");

    PRT_INFO_ADD("Decoding data=%d according to field type name=%s", *data, field_name);
    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &field_col_id, "Type");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, field_col_id, NULL,
                    "Decoded Value - EnumVal/ChildField/Define/Struct member value");

    RHITERATOR(field, parsed_info)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", field->entry.name);
        PRT_CELL_SET("%s", field->print_value);
    }

    PRT_COMMITX;

exit:
    if (parsed_info)
    {
        sand_signal_list_free(parsed_info);
    }
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnx_dbal_table_man = {
    .brief = "table operation menu",
    .full = NULL
};

static sh_sand_man_t dnx_dbal_signal_man = {
    .brief = "signal operation menu",
    .full = NULL
};

static sh_sand_man_t dnx_dbal_entry_man = {
    .brief = "entry operation menu",
    .full = NULL
};

static sh_sand_man_t dnx_dbal_field_types_man = {
    .brief = "Field types diagnostics menu",
    .full = NULL
};

static sh_sand_man_t dnx_dbal_lables_man = {
    .brief = "Labels operation menu",
    .full = NULL
};

static sh_sand_enum_t sand_test_dbal_logger_type_enum_table[] = {
    {"API", DNX_DBAL_LOGGER_TYPE_API, "Logging API activity"},
    {"ACCESS", DNX_DBAL_LOGGER_TYPE_ACCESS, "Logging access activity"},
    {"DIAG", DNX_DBAL_LOGGER_TYPE_DIAG, "Logging diagnostics activity"},
    {"EMPTY", DNX_DBAL_LOGGER_TYPE_LAST, "dbal logger type is EMPTY"},
    {NULL}
};

static sh_sand_enum_t sand_test_dbal_logger_severity_enum_table[] = {
    {"OFF", DNX_DBAL_LOGGER_OFF, "Logger is off"},
    {"ERROR", DNX_DBAL_LOGGER_ERROR, "Logger active level is ERROR"},
    {"WARN", DNX_DBAL_LOGGER_WARNING, "Logger active level is WARNING"},
    {"INFO", DNX_DBAL_LOGGER_INFO, "Logger active level is INFO"},
    {"VERB", DNX_DBAL_LOGGER_VERBOSE, "Logger active level is VERBOSE"},
    {"PRINT", DNX_DBAL_LOGGER_PRINT, "Logger active level is PRINT"},
    {NULL}
};

static sh_sand_option_t dnx_dbal_logger_options[] = {
    {"Type", SAL_FIELD_TYPE_ENUM, "dbal activity type", "EMPTY", (void *) sand_test_dbal_logger_type_enum_table},
    {"SeVerity", SAL_FIELD_TYPE_ENUM, "logger active level", "PRINT",
     (void *) sand_test_dbal_logger_severity_enum_table},
    {"Table", SAL_FIELD_TYPE_STR, "assign logger level to specific table", ""},
    {"Status", SAL_FIELD_TYPE_BOOL, "show status", "No"},
    {"Mode", SAL_FIELD_TYPE_STR, "write / regular", ""},
    {NULL}
};

static sh_sand_man_t dnx_dbal_logger_man = {
    .brief = "Logger operation commands",
    .full =
        "change the severity of the logger for each dbal log type (API/ACCESS/DIAG), locking the logger to specific table, change the severity of the logger Off/warn/info/Verbose",
    .synopsis =
        "[Type=<API/ACCESS/DIAG>] [SeVerity=<level>] [table=<table_name/off>] [file=<file_name/close>] [status] [Mode=<write/regular>] ",
    .examples =
        "File=dbal_log.txt\n" "File=close\n"
        "Type=API SeVerity=VERB\n" "Type=ACCESS SeVerity=warn\n" "table=EGRESS_PP_PORT\n" "table=off\n"
        "Type=API SeVerity=warn mode=Write\n" "Type=API SeVerity=warn\n" "status"
};

static sh_sand_option_t dnx_dbal_last_options[] = {
    {"Table", SAL_FIELD_TYPE_STR, "dbal last", ""},
    {"candidate", SAL_FIELD_TYPE_BOOL, "dbal last", "No"},
    {"HWElement", SAL_FIELD_TYPE_STR, "dbal last", ""},
    {NULL}
};

static sh_sand_option_t dnx_dbal_dcd_options[] = {
    {"data", SAL_FIELD_TYPE_ARRAY32, "data that should be decoded", NULL},
    {"size", SAL_FIELD_TYPE_UINT32, "dbal field size in bits", NULL},
    {"name", SAL_FIELD_TYPE_STR, "dbal field name", NULL},
    {NULL}
};

static sh_sand_man_t dnx_dbal_last_man = {
    .brief = "Last packet operation menu",
    .full =
        "Returns information regarding to the tables that are Suspicious to be hit by the last packet (only appicable for HL tables).",
    .synopsis =
        "dbal Last - all tables. dbal Last table=table_name (only for specific table). dbal Last HWElement=element_name dumps all tables related to the element (can be partial name), dbal Last candidate=true also to recieve candidate tables",
    .examples = "\n" "table=INGRESS_PP_PORT\n" "HWElement=IPPF_VTT_ST_ONE_L\n" "candidate=NO"
};

static sh_sand_man_t dnx_dbal_stats_man = {
    .brief = "DBAL statistics information operation menu.",
    .full = NULL,
    .synopsis = NULL,
    .examples = NULL
};

static sh_sand_man_t dnx_dbal_handles_man = {
    .brief = "handles operation menu",
    .full = NULL
};

static sh_sand_man_t dnx_dbal_access_man = {
    .brief = "Access operation menu, gives info related to the access method",
    .full = NULL
};

sh_sand_man_t sh_dnx_dbal_man = {
    .brief = "dbal diagnostic commands",
    .full = NULL
};

static sh_sand_man_t dnx_dbal_dcd_man = {
    .brief = "Decode signal value using DBAL",
    .full = "Decode signal value using DBAL. Decode enum values, defines, child fields",
    .synopsis = "data=<int> size=<int> name=<string>",
    .examples =
        "data=32576 size=21 name=destination\n" "data=2 size=3 name=LEARN_KEY_CONTEXT\n"
        "data=65535 size=16 name=FIELD_DEFINE_EXAMPLE"
};

/**
 * \brief DNX DBAL diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for DBAL diagnostic commands
 */
sh_sand_cmd_t sh_dnx_dbal_cmds[] = {
   /********************************************************************************************************************************
    * CMD_NAME *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *          *                           * Level                   *                                *                            *
    *          *                           * CMD                     *                                *                            *
 */
    {"TaBLe", NULL, diag_dbal_table_cmds, NULL, &dnx_dbal_table_man},
    {"ENTry", NULL, diag_dbal_entry_cmds, NULL, &dnx_dbal_entry_man},
    {"SIGnal", NULL, diag_dbal_signal_cmds, NULL, &dnx_dbal_signal_man},
    {"TYPes", NULL, sh_dnx_dbal_field_types_cmds, NULL, &dnx_dbal_field_types_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"LaBeL", NULL, sh_dnx_dbal_labels_cmds, NULL, &dnx_dbal_lables_man},
    {"LoGger", cmd_dbal_logger_set, NULL, dnx_dbal_logger_options, &dnx_dbal_logger_man},
    {"HanDLe", NULL, sh_dnx_dbal_handles_cmds, NULL, &dnx_dbal_handles_man},
    {"ACCess", NULL, sh_dnx_dbal_access_cmds, NULL, &dnx_dbal_access_man},
    {"Last", cmd_dbal_last_set, NULL, dnx_dbal_last_options, &dnx_dbal_last_man, NULL, NULL, SH_CMD_SKIP_EXEC},
    {"STATS", cmd_dbal_statistics_set, NULL, NULL, &dnx_dbal_stats_man, NULL, NULL},
    {"Decode", cmd_dbal_decode, NULL, dnx_dbal_dcd_options, &dnx_dbal_dcd_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {NULL}
};
