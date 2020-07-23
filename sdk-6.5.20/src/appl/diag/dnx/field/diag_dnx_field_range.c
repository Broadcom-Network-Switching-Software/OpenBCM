/** \file diag_dnx_field_range.c
 *
 * Diagnostics procedures, for DNX, for 'range' operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDDIAGSDNX
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_field_range.h"
#include <bcm_int/dnx/field/field_group.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include "diag_dnx_field_utils.h"
#include "include/bcm_int/dnx/field/field_range.h"
#include <src/bcm/dnx/field/map/field_map_local.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * Options
 * {
 */
/*
 * }
 */
/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */

/**
 *  'Help' description for Range list (shell commands).
 */
static sh_sand_man_t Field_range_list_man = {
    .brief = "'type'- displays the basic info about a specific range type"
            "on a certain stage (filter by field context id and field stage)",
    .full = "'type' display for a range of field ranges. Field range is specified via 'type'.\r\n"
            "If no 'type' is specified then '0'-'nof_ranges_types-1' is assumed (0-3).\r\n"
            "If no stage is specified then 'IPMF1'-'EPMF' is assumed (0-3).\r\n",
    .synopsis = "[type=<<Lowest range type>-<Highest range type>>]"
                "[stage=<<IPMF1 | IPMF2 | ...>-<<IPMF1 | IPMF2 | ...>>]",
    .examples = "type=L4_SRC stage=ipmf1",
};

static sh_sand_option_t Field_range_list_options[] = {
    {DIAG_DNX_FIELD_OPTION_TYPE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest range type to get its info",    "RANGE_TYPE_LOWEST-RANGE_TYPE_HIGHEST",       (void *)Field_range_list_range_type_enum_table, "RANGE_TYPE_LOWEST-RANGE_TYPE_HIGHEST"},
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field stage to context key info for",   "STAGE_LOWEST-STAGE_HIGHEST",    (void *)Field_range_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {NULL}
};

static sh_sand_man_t Field_range_extended_man = {
    .brief = "'global' displays info for global configuration of extended range types."
             "'entries' displays info for configured entries per range type.",
    .full = "'global' displays info for global configuration of extended range types.\r\n"
            "'entries' displays info for configured entries per range type. Field range is specified via 'type'.\r\n"
            "If no 'type' is specified then '0'-'nof_ranges_types-1' is assumed.\r\n",
    .synopsis = NULL,
    .examples = NULL,
};

static sh_sand_man_t Field_range_extended_global_man = {
    .brief = "'global' displays info for global configuration of extended range types.",
    .full = "'global' displays info for global configuration of extended range types.\r\n",
    .synopsis = NULL,
    .examples = NULL,
};

static sh_sand_option_t Field_range_extended_global_options[] = {
    {NULL}
};

static sh_sand_man_t Field_range_extended_entries_man = {
    .brief = "'entries' displays info for configured entries per range type.",
    .full = "'entries' displays info for configured entries per range type. Field range is specified via 'type'.\r\n"
            "'type' display for a range of field ranges. Field range is specified via 'type'.\r\n"
            "If no 'type' is specified then '0'-'nof_ranges_types-1' is assumed.\r\n",
    .synopsis = "[type=<<RANGE_TYPE_LOWEST>-<RANGE_TYPE_HIGHEST>>]",
    .examples = "type=L4_SRC",
};

static sh_sand_option_t Field_range_extended_entries_options[] = {
    {DIAG_DNX_FIELD_OPTION_TYPE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest range type to get its info",    "RANGE_TYPE_LOWEST-RANGE_TYPE_HIGHEST",       (void *)Field_range_extended_range_type_enum_table, "RANGE_TYPE_LOWEST-RANGE_TYPE_HIGHEST"},
    {NULL}
};

static sh_sand_man_t Field_range_extended_ffc_man = {
    .brief = "'config' displays info for user configured ranges per range config index.",
    .full = "'config' displays info for user configured ranges per range config index. Field range is specified via 'index'.\r\n"
            "'index' display for a range of field range configurable type indices. Field range is specified via 'index'.\r\n"
            "If no 'index' is specified then '0'-'nof_configurable_ranges-1' is assumed.\r\n",
    .synopsis = "[index=<<RANGE_CONFIG_TYPE_LOWEST>-<RANGE_CONFIG_TYPE_HIGHEST>>]",
    .examples = "index=RANGE_CONFIG_TYPE_LOWEST-RANGE_CONFIG_TYPE_HIGHEST",
};

static sh_sand_option_t Field_range_extended_ffc_options[] = {
    {DIAG_DNX_FIELD_OPTION_INDEX,  SAL_FIELD_TYPE_UINT32, "Lowest-highest range config type indices to get its info",    "RANGE_CONFIG_TYPE_LOWEST-RANGE_CONFIG_TYPE_HIGHEST",       (void *)Field_range_extended_range_config_type_enum_table, "RANGE_CONFIG_TYPE_LOWEST-RANGE_CONFIG_TYPE_HIGHEST"},
    {NULL}
};


/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function displays Field ranges per range_type or range of types  (optional), specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. Contains 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_range_list_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 do_display, range_type_display;
    int range_type_lower, range_type_upper;
    dnx_field_stage_e stage_lower, stage_upper, stage_index;
    dnx_field_range_type_e range_type, range_type_index;
    dnx_field_range_info_t range_info;
    bcm_field_stage_t bcm_stage;
    uint32 range_id;
    int max_range_id;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_TYPE, range_type_lower, range_type_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);

    /** Get the max_range_id to iterate over. */
    max_range_id = DNX_DATA_MAX_FIELD_BASE_IPMF1_NOF_L4_OPS_RANGES_LEGACY;

    range_type_display = FALSE;
    do_display = FALSE;
    /** Iterate over all given range_types. */
    for (range_type_index = (DNX_FIELD_RANGE_TYPE_FIRST + range_type_lower); range_type_index <= range_type_upper;
         range_type_index++)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_range_type_str_to_enum_val_conversion
                        (unit, Field_range_list_range_type_enum_table[range_type_index].string, &range_type));

        if (range_type == DNX_FIELD_RANGE_TYPE_L4_SRC_PORT || range_type == DNX_FIELD_RANGE_TYPE_L4_DST_PORT)
        {
            dnx_field_range_type_e range_type_internal;
            range_type_display = FALSE;

            PRT_TITLE_SET("L4SRC and L4DST Ranges");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range ID");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range Field Stage");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range min");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range max");

            /** Iterate over all range_ids. */
            for (range_id = 0; range_id < max_range_id; range_id++)
            {
                uint8 new_range_id_set = TRUE;
                /** Iterate over all given stages. */
                for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
                {
                    uint8 new_stage_id_set = TRUE;
                    dnx_field_stage_e field_stage;
                    int max_range_id_per_stage = 0;
                    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                                    (unit, Field_range_diag_stage_enum_table[stage_index].string, &field_stage));

                    if (dnx_data_field.features.extended_l4_ops_get(unit)
                        && (field_stage == DNX_FIELD_STAGE_IPMF1 || field_stage == DNX_FIELD_STAGE_IPMF2))
                    {
                        continue;
                    }

                    SHR_IF_ERR_EXIT(dnx_field_range_nof_range_ids
                                    (unit, range_type, field_stage, &max_range_id_per_stage));
                    if (range_id >= max_range_id_per_stage)
                    {
                        continue;
                    }

                    for (range_type_internal = DNX_FIELD_RANGE_TYPE_FIRST;
                         range_type_internal <= DNX_FIELD_RANGE_TYPE_L4_DST_PORT; range_type_internal++)
                    {
                        /** Convert DNX to BCM Field Stage. */
                        SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));

                        /** If the stage is not supported for the given range_type continue. */
                        if (range_map_legacy[range_type_internal][field_stage].table_id == DBAL_TABLE_EMPTY)
                        {
                            continue;
                        }

                        /** Init the range info structure. */
                        SHR_IF_ERR_EXIT(dnx_field_range_info_t_init(unit, &range_info));
                        range_info.range_type = range_type_internal;
                        /** Get the range_info. */
                        SHR_IF_ERR_EXIT(dnx_field_range_get(unit, field_stage, &range_id, &range_info));
                        /** Check if min or max_val is set and if it is print the range_id. */
                        if (range_info.min_val != DNX_FIELD_RANGE_VALUE_INVALID
                            || range_info.max_val != DNX_FIELD_RANGE_VALUE_INVALID)
                        {
                            /**
                             * In case the flag new_range_id_set is set to TRUE,
                             * we will print Range ID on the first line of the table.
                             * Otherwise we skip the Range ID cell.
                             */
                            if (new_range_id_set)
                            {
                                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                                PRT_CELL_SET("%d", range_id);
                                new_range_id_set = FALSE;
                            }
                            else
                            {
                                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                                PRT_CELL_SKIP(1);
                            }

                            /**
                             * In case the flag new_stage_id_set is set to TRUE,
                             * we will print Range Stage on current line of the table.
                             * Otherwise we skip the Range Stage cell.
                             */
                            if (new_stage_id_set)
                            {
                                PRT_CELL_SET("%s", dnx_field_bcm_stage_text(bcm_stage));
                                new_stage_id_set = FALSE;
                            }
                            else
                            {
                                PRT_CELL_SKIP(1);
                            }

                            PRT_CELL_SET("%s", dnx_field_range_type_text(range_type_internal));
                            PRT_CELL_SET("%d", range_info.min_val);
                            PRT_CELL_SET("%d", range_info.max_val);

                            range_type_display = TRUE;
                        }
                    }
                }
            }

            if (range_type_display)
            {
                PRT_COMMITX;
                do_display = TRUE;
            }
            else
            {
                PRT_FREE;
            }

            /**
             * We already presented first two types SRC and DST PORTs,
             * then we will continue with the next one after increasing
             * the value of the range_type in the for() loop.
             * Field_range_list_range_type_enum_table[1] should point to 'L4_DST".
             */
            range_type_index = Field_range_list_range_type_enum_table[1].value;
        }
        else
        {
            /**
             * Add a new Table for each range.
             */
            PRT_TITLE_SET("%s Range", range_map_legacy[range_type][DNX_FIELD_STAGE_IPMF1].name);
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range ID");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range Field Stage");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range min");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range max");

            /** Iterate over all range_ids. */
            for (range_id = 0; range_id < max_range_id; range_id++)
            {
                uint8 new_range_id_set = TRUE;
                /** Iterate over all given stages. */
                for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
                {
                    uint8 new_stage_id_set = TRUE;
                    dnx_field_stage_e field_stage;
                    int max_range_id_per_stage = 0;
                    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                                    (unit, Field_range_diag_stage_enum_table[stage_index].string, &field_stage));

                    SHR_IF_ERR_EXIT(dnx_field_range_nof_range_ids
                                    (unit, range_type, field_stage, &max_range_id_per_stage));
                    if (range_id >= max_range_id_per_stage)
                    {
                        continue;
                    }

                    /** Convert DNX to BCM Field Stage. */
                    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));

                    /** If the stage is not supported for the given range_type continue. */
                    if (range_map_legacy[range_type][field_stage].table_id == DBAL_TABLE_EMPTY)
                    {
                        continue;
                    }

                    /** Init the range info structure. */
                    SHR_IF_ERR_EXIT(dnx_field_range_info_t_init(unit, &range_info));
                    range_info.range_type = range_type;
                    /** Get the range_info. */
                    SHR_IF_ERR_EXIT(dnx_field_range_get(unit, field_stage, &range_id, &range_info));
                    /** Check if min or max_val is set and if it is print the range_id. */
                    if (range_info.min_val != DNX_FIELD_RANGE_VALUE_INVALID
                        || range_info.max_val != DNX_FIELD_RANGE_VALUE_INVALID)
                    {
                        /**
                         * In case the flag new_range_id_set is set to TRUE,
                         * we will print Range ID on the first line of the table.
                         * Otherwise we skip the Range ID cell.
                         */
                        if (new_range_id_set)
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                            PRT_CELL_SET("%d", range_id);
                            new_range_id_set = FALSE;
                        }
                        else
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SKIP(1);
                        }

                        /**
                         * In case the flag new_stage_id_set is set to TRUE,
                         * we will print Range Stage on current line of the table.
                         * Otherwise we skip the Range Stage cell.
                         */
                        if (new_stage_id_set)
                        {
                            PRT_CELL_SET("%s", dnx_field_bcm_stage_text(bcm_stage));
                            new_stage_id_set = FALSE;
                        }
                        else
                        {
                            PRT_CELL_SKIP(1);
                        }

                        PRT_CELL_SET("%d", range_info.min_val);
                        PRT_CELL_SET("%d", range_info.max_val);

                        range_type_display = TRUE;
                    }
                }
            }

            if (range_type_display)
            {
                PRT_COMMITX;
                do_display = TRUE;
            }
            else
            {
                PRT_FREE;
            }
        }
    }

    if (!do_display)
    {
        /*
         * If nothing was printed then just clear loaded PRT memory.
         * This could be left to 'exit' but it is clearer here.
         */
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO allocated field ranges were found on specified filters!! %s%s%s%s\r\n\n", EMPTY,
                   EMPTY, EMPTY, EMPTY);
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays global configuration for field ranges.
 *   Only for J2P and above devices where extended_l4_ops is enabled.
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. Contains 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_range_extended_global_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int index;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    bcm_switch_range_operator_t range_operator;
    uint8 index_value_found;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_field.features.extended_l4_ops_get(unit))
    {
        LOG_CLI_EX("\r\n" "Range Extended diagnostic is not available for current device."
                   " Please use 'field range list' diagnostic. %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    index_value_found = FALSE;

    PRT_TITLE_SET("Range Global configuration");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Operator");
    SHR_IF_ERR_EXIT(bcm_switch_control_get(unit, bcmSwitchRangeOperator, &range_operator));
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", dnx_field_bcm_range_operator_text(range_operator));

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

    PRT_CELL_SET("Range Type (index/type)");
    key.type = bcmSwitchRangeTypeSelect;
    for (index = 0; index < dnx_data_field.L4_Ops.nof_ext_types_get(unit); index++)
    {
        key.index = index;
        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_get(unit, key, &value));
        if ((bcm_field_range_type_t) (value.value) == bcmFieldRangeTypeCount)
        {
            continue;
        }
        PRT_CELL_SET("%d / %s", index, dnx_field_bcm_range_type_text((bcm_field_range_type_t) (value.value)));
        if (index < (dnx_data_field.L4_Ops.nof_ext_types_get(unit) - 1))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SKIP(1);
        }
        index_value_found = TRUE;
    }

    if (index_value_found == FALSE)
    {
        PRT_CELL_SET("%s", "-");
    }
    else
    {
        index_value_found = FALSE;
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

    PRT_CELL_SET("Result Map (index/type)");
    key.type = bcmSwitchRangeResultMap;
    for (index = 0; index < dnx_data_field.L4_Ops.nof_ext_encoders_get(unit); index++)
    {
        key.index = index;
        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_get(unit, key, &value));
        if ((bcm_switch_range_result_map_t) (value.value) == bcmSwitchRangeResultMapInvalid)
        {
            continue;
        }
        PRT_CELL_SET("%d / %s", index,
                     dnx_field_bcm_range_result_map_text((bcm_switch_range_result_map_t) (value.value)));
        if (index < (dnx_data_field.L4_Ops.nof_ext_encoders_get(unit) - 1))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SKIP(1);
        }
        index_value_found = TRUE;
    }

    if (index_value_found == FALSE)
    {
        PRT_CELL_SET("%s", "-");
    }
    else
    {
        index_value_found = FALSE;
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field range entries configuration per range_type
 *   or range of types (optional), specified by the caller.
 *   Only for J2P and above devices where extended_l4_ops is enabled.
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. Contains 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_range_extended_entries_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int range_type_lower, range_type_upper;
    dnx_field_range_type_e range_type, range_type_index;
    bcm_field_range_type_t bcm_range_type;
    bcm_field_range_info_t bcm_range_info;
    uint8 do_display;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_field.features.extended_l4_ops_get(unit))
    {
        LOG_CLI_EX("\r\n" "Range Extended diagnostic is not available for current device."
                   " Please use 'field range list' diagnostic. %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_TYPE, range_type_lower, range_type_upper);

    do_display = FALSE;
    /** Iterate over all given range_types. */
    for (range_type_index = range_type_lower; range_type_index <= range_type_upper; range_type_index++)
    {
        bcm_field_range_t range_id;
        int max_range_id = 0;
        uint32 range_indexes[DNX_DATA_MAX_FIELD_L4_OPS_NOF_EXT_TYPES] = { 0 };
        int index;
        int nof_valid_range_indexes = 0;
        char indexes_per_range_type_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
        uint8 range_type_display = FALSE;
        int range_type_min_val;
        int range_type_max_val;
        int skipped_entries = FALSE;

        SHR_IF_ERR_EXIT(diag_dnx_field_utils_range_type_str_to_enum_val_conversion
                        (unit, Field_range_extended_range_type_enum_table[range_type_index].string, &range_type));

        SHR_IF_ERR_EXIT(dnx_field_map_range_type_dnx_to_bcm(unit, range_type, &bcm_range_type));
        SHR_IF_ERR_EXIT(dnx_field_range_type_nof_range_ids_extended(unit, range_type, range_indexes, &max_range_id));

        /** Calculate number of valid indexes in range_indexes[] to iterate on. */
        nof_valid_range_indexes = max_range_id / dnx_data_field.L4_Ops.nof_range_entries_get(unit);
        for (index = 0; index < nof_valid_range_indexes; index++)
        {
            char indexes_per_range_type_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d, ", range_indexes[index]);
            sal_snprintf(indexes_per_range_type_buff, sizeof(indexes_per_range_type_buff), "%d, ",
                         range_indexes[index]);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(indexes_per_range_type_string, "%s", indexes_per_range_type_buff);
            sal_strncat_s(indexes_per_range_type_string, indexes_per_range_type_buff,
                          sizeof(indexes_per_range_type_string));
        }
        /**
         * Extract last 2 symbols of the constructed string
         * to not present comma and space ", " at the end of it.
         */
        indexes_per_range_type_string[sal_strlen(indexes_per_range_type_string) - 2] = '\0';
        PRT_TITLE_SET("%s Range Entries configuration (Index: %s)", dnx_field_bcm_range_type_text(bcm_range_type),
                      indexes_per_range_type_string);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range ID");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value");

        SHR_IF_ERR_EXIT(dnx_field_range_absolute_min_max_values
                        (unit, range_type, &range_type_min_val, &range_type_max_val));
        /** Iterate over all range_ids. */
        for (range_id = 0; range_id < max_range_id; range_id++)
        {
            /** Init the range info structure. */
            bcm_field_range_info_t_init(&bcm_range_info);
            bcm_range_info.range_type = bcm_range_type;
            /** Get the range_info. */
            SHR_IF_ERR_EXIT(bcm_field_range_info_get(unit, bcmFieldStageIngressPMF1, &range_id, &bcm_range_info));

            /** Skip the default enties (full range 0 to max_value). */
            if (bcm_range_info.min_val == range_type_min_val && bcm_range_info.max_val == range_type_max_val)
            {
                skipped_entries = TRUE;
                continue;
            }
            /** Check if min or max_val is set and if it is print the range_id. */
            if (bcm_range_info.min_val != DNX_FIELD_RANGE_VALUE_INVALID
                || bcm_range_info.max_val != DNX_FIELD_RANGE_VALUE_INVALID)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("%d", range_id);
                PRT_CELL_SET("Min: %d\nMax: %d", bcm_range_info.min_val, bcm_range_info.max_val);

                range_type_display = TRUE;
            }
        }

        if (range_type_display)
        {
            PRT_COMMITX;
            do_display = TRUE;
            if (skipped_entries)
            {
                LOG_CLI_EX("\r\n"
                           "All entries within range ID %d-%d are valid, but only entries taht are not 'always hit' are shown! %s%s\r\n\n",
                           0, (max_range_id - 1), EMPTY, EMPTY);
            }
        }
        else
        {
            PRT_FREE;
        }
    }

    if (!do_display)
    {
        /** If nothing was printed then just clear loaded PRT memory. */
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO allocated field ranges were found on specified filters!! %s%s%s%s\r\n\n", EMPTY,
                   EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field range FFC configuration per range_type
 *   or range of types (optional), specified by the caller.
 *   Only for J2P and above devices where extended_l4_ops is enabled.
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. Contains 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_range_extended_ffc_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int range_config_type_lower, range_config_type_upper;
    uint32 range_config_type_index;
    uint8 do_display;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_field.features.extended_l4_ops_get(unit))
    {
        LOG_CLI_EX("\r\n" "Range Extended diagnostic is not available for current device."
                   " Please use 'field range list' diagnostic. %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_INDEX, range_config_type_lower, range_config_type_upper);

    PRT_TITLE_SET("User Configured Ranges");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range Config");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Input Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Input Arg");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Input Offset");

    do_display = FALSE;
    /** Iterate over all given range_types. */
    for (range_config_type_index = range_config_type_lower; range_config_type_index <= range_config_type_upper;
         range_config_type_index++)
    {
        dnx_field_range_type_qual_info_t dnx_qual_info;
        bcm_field_input_types_t bcm_input_type;
        uint32 dnx_qual_size;

        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &dnx_qual_info.qual_info));
        SHR_IF_ERR_EXIT(dnx_field_range_ffc_config_get(unit, 0, range_config_type_index, &dnx_qual_info));

        if (dnx_qual_info.dnx_qual == DNX_FIELD_QUAL_ID_INVALID)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm
                        (unit, dnx_qual_info.qual_info.input_type, &bcm_input_type));
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                        (unit, DNX_FIELD_STAGE_L4_OPS, dnx_qual_info.dnx_qual, &dnx_qual_size));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%d", range_config_type_index);
        PRT_CELL_SET("%s", dnx_field_dnx_qual_text(unit, dnx_qual_info.dnx_qual));
        PRT_CELL_SET("%d", dnx_qual_size);
        PRT_CELL_SET("%s", dnx_field_bcm_input_type_text(bcm_input_type));

        if (dnx_qual_info.qual_info.input_type == DNX_FIELD_INPUT_TYPE_META_DATA ||
            dnx_qual_info.qual_info.input_type == DNX_FIELD_INPUT_TYPE_META_DATA2)
        {
            PRT_CELL_SET("%s", "-");
            if (DNX_QUAL_CLASS(dnx_qual_info.dnx_qual) == DNX_FIELD_QUAL_CLASS_USER)
            {
                PRT_CELL_SET("%d", dnx_qual_info.qual_info.offset);
            }
            else
            {
                PRT_CELL_SET("%s", "-");
            }
        }
        else if (dnx_qual_info.qual_info.input_type == DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD ||
                 dnx_qual_info.qual_info.input_type == DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE)
        {
            PRT_CELL_SET("%d", dnx_qual_info.qual_info.input_arg);
            if (DNX_QUAL_CLASS(dnx_qual_info.dnx_qual) == DNX_FIELD_QUAL_CLASS_USER)
            {
                PRT_CELL_SET("%d", dnx_qual_info.qual_info.offset);
            }
            else
            {
                PRT_CELL_SET("%s", "-");
            }
        }
        else if (dnx_qual_info.qual_info.input_type == DNX_FIELD_INPUT_TYPE_KBP)
        {
            PRT_CELL_SET("%s", "-");
            PRT_CELL_SET("%d", dnx_qual_info.qual_info.offset);
        }
        else if (dnx_qual_info.qual_info.input_type == DNX_FIELD_INPUT_TYPE_CONST)
        {
            PRT_CELL_SET("0x%X", dnx_qual_info.qual_info.input_arg);
            PRT_CELL_SET("%s", "-");
        }
        else
        {
            PRT_CELL_SET("%d", dnx_qual_info.qual_info.input_arg);
            PRT_CELL_SET("%d", dnx_qual_info.qual_info.offset);
        }

        do_display = TRUE;
    }

    if (do_display)
    {
        PRT_COMMITX;
    }
    else
    {
        /** If nothing was printed then just clear loaded PRT memory. */
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO allocated field ranges were found on specified filters!! %s%s%s%s\r\n\n", EMPTY,
                   EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */
/*
 * }
 */
/* *INDENT-ON* */

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'efes list' shell commands
 */
sh_sand_cmd_t Sh_dnx_field_range_extended_cmds[] = {
    {"global", diag_dnx_field_range_extended_global_cb, NULL, Field_range_extended_global_options,
     &Field_range_extended_global_man}
    ,
    {"config", diag_dnx_field_range_extended_ffc_cb, NULL, Field_range_extended_ffc_options,
     &Field_range_extended_ffc_man}
    ,
    {"entries", diag_dnx_field_range_extended_entries_cb, NULL, Field_range_extended_entries_options,
     &Field_range_extended_entries_man}
    ,
    {NULL}
};

/**
 * \brief
 *   List of shell options for 'range' shell commands (list)
 */
sh_sand_cmd_t Sh_dnx_field_range_cmds[] = {
    {"list", diag_dnx_field_range_list_cb, NULL, Field_range_list_options, &Field_range_list_man}
    ,
    {"extended", NULL, Sh_dnx_field_range_extended_cmds, NULL, &Field_range_extended_man}
    ,
    {NULL}
};

/*
 * }
 */
