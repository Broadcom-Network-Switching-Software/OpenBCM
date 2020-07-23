/** \file diag_dnx_field_tcam.c
 *
 * Diagnostics procedures, for DNX, for 'Tcam' operations.
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
#include "diag_dnx_field_tcam.h"
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_manager_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_profile_types.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/field/tcam/tcam_location_algo.h>
#include <bcm_int/dnx/field/tcam/tcam_bank_manager.h>
#include "diag_dnx_field_utils.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/** Get the number of cores from DNX data. */
#define DIAG_DNX_FIELD_TCAM_NOF_CORES  dnx_data_device.general.nof_cores_get(unit)
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

/*
 * }
 */
/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */

/**
 *  'Help' description for Tcam Bank diag (shell commands).
 */
static sh_sand_man_t Field_tcam_bank_info_man = {
    .brief = "'Tcam Bank'- displays all used TCAM banks and relevant info",
    .full = "'Tcam Bank' all used TCAM banks and relevant info.\r\n"
      "If no 'bank' ('0'-'nof_banks-1' --> (0-15)) is specified then info for all banks will be shown.\r\n",
    .synopsis = "[bank=<0-15>]",
    .examples = "bank=0",
};
static sh_sand_option_t Field_tcam_bank_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_BANK,          SAL_FIELD_TYPE_UINT32, "Tcam bank id to get its info",       "MIN-MAX"   , (void *)Field_tcam_bank_id_enum_table, "MIN-MAX"},
    {NULL}
};

/**
 *  'Help' description for Tcam Management List diag (shell commands).
 */
static sh_sand_man_t Field_tcam_management_list_man = {
    .brief = "'Tcam management list'- displays all used TCAM handlers and basic info",
    .full = "'Tcam management list' all used TCAM handlers and basic info.\r\n"
      "No need to specify any input to the function.\r\n",
    .synopsis = "",
    .examples = "",
};
static sh_sand_option_t Field_tcam_management_list_options[] = {
    {NULL}
};

/**
 *  'Help' description for Tcam Management Info diag (shell commands).
 */
static sh_sand_man_t Field_tcam_management_info_man = {
    .brief = "'Tcam management info'- displays the full info about a specific tcam handler (filter by tcam handler id)",
    .full = "'Tcam management info' displays info for a specific tcam handler id.\r\n"
      "If no 'handler' ('0'-'nof_tcam_handlers-1' --> (0-63)) is specified then an error will occur.\r\n",
    .synopsis = "[handler=<0-63>]",
    .examples = "handler=0",
};

static sh_sand_man_t field_tcam_management_man = {
    .brief = "'Tcam management'- displays the full info  tcam handlers",
    .full = "'Tcam management' displays the full info  tcam handlers: list - full list of used TCAM handlers, info - details per specific TCAM handler\r\n",
    .synopsis = NULL,
    .examples = "handler=0",
};

static sh_sand_man_t field_tcam_bank_man = {
    .brief = "Tcam bank' - displays the full info  tcam banks",
    .full = "Tcam bank'  displays the full info  tcam banks: list - full list of used TCAM banks, info - details per specific TCAM bank\r\n",
    .synopsis = NULL,
    .examples = "bank=0",
};

static sh_sand_option_t Field_tcam_management_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_HANDLER,          SAL_FIELD_TYPE_UINT32, "Tcam handler id to get its info",       NULL   , (void *)Field_tcam_handler_id_enum_table, "MIN-MAX"},
    {NULL}
};
/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function displays all used TCAM banks and relevant info.
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
diag_dnx_field_tcam_bank_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int do_display;
    int do_display_bank_usage;
    int bank_id_iter;
    int bank_id_lower, bank_id_upper;
    uint32 bank_entries_count[2][DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS];
    uint32 banks_bmp[1];
    uint32 nof_available_entries;

    uint8 is_fg_allocated;
    dnx_field_group_t fg_id_iter;
    int allocated_fg_iter;
    dnx_field_group_t fg_id_array[DNX_DATA_MAX_FIELD_GROUP_NOF_FGS];
    dnx_field_group_full_info_t *fg_info = NULL;
    uint32 key_size_in_bits;
    bcm_field_stage_t bcm_stage;
    int core = 0;
    uint32 max_nof_fgs;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_BANK, bank_id_lower, bank_id_upper);

    SHR_ALLOC(fg_info, sizeof(dnx_field_group_full_info_t), "fg_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    allocated_fg_iter = 0;
    key_size_in_bits = 0;
    do_display_bank_usage = FALSE;
    max_nof_fgs = dnx_data_field.group.nof_fgs_get(unit);

    /**
     * Check if and get all allocated TCAM FGs, if there is any.
     * Fill their IDs in fg_id_array[].
     */
    for (fg_id_iter = 0; fg_id_iter < max_nof_fgs; fg_id_iter++)
    {
        /**
         * In case the field group is allocated, retrieve all needed information for it. If not
         * continue with the next field group.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_iter, &is_fg_allocated));
        if (!is_fg_allocated)
        {
            continue;
        }
        /** Get information about the given Field Group. */
        SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id_iter, fg_info));
        if (fg_info->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
        {
            fg_id_array[allocated_fg_iter] = fg_id_iter;
            allocated_fg_iter++;
        }
    }

    if (allocated_fg_iter == 0)
    {
        LOG_CLI_EX("\r\n" "No allocated FGs were found!!!%s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    /** Iterate over all TCAM bank IDs and present relevant info. */
    for (bank_id_iter = bank_id_lower; bank_id_iter <= bank_id_upper; bank_id_iter++)
    {
        do_display = TRUE;

        /**
         * Iterate over all allocated FGs and present information about those,
         * which are using the current bank.
         */
        for (fg_id_iter = 0; fg_id_iter < allocated_fg_iter; fg_id_iter++)
        {
            /** Get information about the given Field Group. */
            SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id_array[fg_id_iter], fg_info));

            for (core = 0; core < DIAG_DNX_FIELD_TCAM_NOF_CORES; core++)
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entries_info
                                (unit, core, fg_info->tcam_handler_id, bank_entries_count[core], banks_bmp));
            }

            /** Banks are same per core, then we just check the first array only. */
            if (!(SHR_BITGET(&banks_bmp[0], bank_id_iter)))
            {
                continue;
            }

            /**
             * Only in case of new bank ID we should construct the table and print it.
             * In the end set the do_display to FALSE.
             */
            if (do_display)
            {
                PRT_TITLE_SET("TCAM Bank Info");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Core");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Available Entries (80b)");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Available Entries (160b)");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Available Entries (320b)");
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

                PRT_CELL_SET("%d", bank_id_iter);

                /** Convert DNX to BCM Field Stage */
                SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm
                                (unit, fg_info->group_basic_info.field_stage, &bcm_stage));
                PRT_CELL_SET("%s", dnx_field_bcm_stage_text(bcm_stage));

                for (core = 0; core < DIAG_DNX_FIELD_TCAM_NOF_CORES; core++)
                {
                    /** In case we have more than one cores, we should skip cells related to BANK ID and stage. */
                    if (core != 0)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(2);
                    }
                    PRT_CELL_SET("%d", core);
                    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_available_entries_get
                                    (unit, core, bank_id_iter, DNX_FIELD_TCAM_ENTRY_SIZE_HALF, &nof_available_entries));
                    PRT_CELL_SET("%d", nof_available_entries);
                    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_available_entries_get
                                    (unit, core, bank_id_iter, DNX_FIELD_TCAM_ENTRY_SIZE_SINGLE,
                                     &nof_available_entries));
                    PRT_CELL_SET("%d", nof_available_entries);
                    SHR_IF_ERR_EXIT(dnx_field_tcam_bank_available_entries_get
                                    (unit, core, bank_id_iter, DNX_FIELD_TCAM_ENTRY_SIZE_DOUBLE,
                                     &nof_available_entries));
                    PRT_CELL_SET("%d", nof_available_entries);
                }

                PRT_COMMITX;

                do_display = FALSE;

                PRT_TITLE_SET("FGs in TCAM Bank %d", bank_id_iter);
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG Name");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG ID");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Handler ID");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Size");
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Nof Entries");
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            /**
             * Check if Field Group name is being provided, in case no set N/A as default value,
             * in other case set the provided name.
             */
            if (sal_strncmp(fg_info->group_basic_info.name, "", sizeof(fg_info->group_basic_info.name)))
            {
                PRT_CELL_SET("%s", fg_info->group_basic_info.name);
            }
            else
            {
                PRT_CELL_SET("N/A");
            }

            PRT_CELL_SET("%d", fg_id_array[fg_id_iter]);
            PRT_CELL_SET("%d", fg_info->tcam_handler_id);

            /**
             * Present the key_length_type in bits (80, 160, 320 bits).
             */
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_key_size_conversion
                            (unit, fg_info->group_full_info.key_length_type, &key_size_in_bits));
            PRT_CELL_SET("%d", key_size_in_bits);
            if (DIAG_DNX_FIELD_TCAM_NOF_CORES == 1)
            {
                PRT_CELL_SET("core_0: %d\ncore_1: N/A", bank_entries_count[0][bank_id_iter]);
            }
            else
            {
                PRT_CELL_SET("core_0: %d\ncore_1: %d", bank_entries_count[0][bank_id_iter],
                             bank_entries_count[1][bank_id_iter]);
            }
        }
        if (!do_display)
        {
            PRT_COMMITX;
            do_display_bank_usage = TRUE;
        }
    }
    if (!do_display_bank_usage)
    {
        LOG_CLI_EX("\r\n" "NO used TCAM banks were found!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FREE(fg_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function returns name of the dbal table, which owns the given handler id.
 * \param [in] unit - The unit number.
 * \param [in] handler_id - Handler Id, which owner should be retrieved.
 * \param [out] table_name_p - Name of the dbal table which owns the given handler id.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_tcam_handler_id_dbal_table_name_get(
    int unit,
    uint32 handler_id,
    char *table_name_p)
{
    uint8 table_found;
    uint32 handler_id_get;
    /** Array with supported TCAM dbal table types. Check dbal_table_type_e. */
    dbal_table_type_e tcam_table_types[] =
        { DBAL_TABLE_TYPE_TCAM, DBAL_TABLE_TYPE_TCAM_BY_ID, DBAL_TABLE_TYPE_TCAM_DIRECT };
    uint32 tcam_table_type_index;

    SHR_FUNC_INIT_VARS(unit);

    table_found = FALSE;

    for (tcam_table_type_index = 0; tcam_table_type_index < (sizeof(tcam_table_types) / sizeof(tcam_table_types[0]));
         tcam_table_type_index++)
    {
        dbal_tables_e table_iter = DBAL_TABLE_EMPTY;
        SHR_IF_ERR_EXIT(dbal_tables_next_table_get(unit, table_iter, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_MDB,
                                                   DBAL_PHYSICAL_TABLE_NONE, tcam_table_types[tcam_table_type_index],
                                                   &table_iter));
        while (table_iter != DBAL_TABLE_EMPTY)
        {
            /** Get the handler id of the current table. */
            SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, table_iter, &handler_id_get));

            /**
             * Compare the given handle id and the retrieved by the get function.
             * In case of match return the dbal table_name.
             */
            if (handler_id == handler_id_get)
            {
                sal_strncpy(table_name_p, dbal_logical_table_to_string(unit, table_iter), (DBAL_MAX_STRING_LENGTH - 1));
                table_found = TRUE;
                break;
            }

            SHR_IF_ERR_EXIT(dbal_tables_next_table_get(unit, table_iter, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_MDB,
                                                       DBAL_PHYSICAL_TABLE_NONE,
                                                       tcam_table_types[tcam_table_type_index], &table_iter));
        }

        if (table_found)
        {
            break;
        }
    }

    if (!table_found)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Dbal table for handler_id %d was not found.\r\n", handler_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays list of all used TCAM handlers with basic info abou them,
 *   key size, action size, prefix value/size, Access Profile, BAM (Bank Allocation Mode).
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
diag_dnx_field_tcam_management_list_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_tcam_access_fg_params_t fg_params;
    uint32 handler_id;
    uint32 nof_tcam_handlers;
    int access_profile_id;
    bcm_field_tcam_bank_allocation_mode_t bcm_bank_allocation_mode;
    dnx_field_tcam_handler_state_e state;
    char **tcam_stage_short_str = NULL;
    uint32 tcam_stage_realtokens = 0;
    char tcam_stage_str[DIAG_DNX_FIELD_UTILS_STR_SIZE];
    int do_display;
    uint8 print_handler_table;
    char dbal_table_name[DBAL_MAX_STRING_LENGTH];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    nof_tcam_handlers = dnx_data_field.tcam.nof_tcam_handlers_get(unit);

    do_display = FALSE;
    print_handler_table = TRUE;

    for (handler_id = 0; handler_id < nof_tcam_handlers; handler_id++)
    {
        /** Verify if the given handler_id is in use, if not continue iteration. */
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_state_get(unit, handler_id, &state));
        if (state != DNX_FIELD_TCAM_HANDLER_STATE_OPEN)
        {
            continue;
        }

        /** Get TCAM handler information. */
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
        /** Get Access Profile ID. */
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_access_profile_id_get(unit, handler_id, &access_profile_id));
        /** Convert the retrieved DNX bank allocation mode to the BCM equivalent. */
        dnx_field_map_bank_allocation_mode_dnx_to_bcm(unit, fg_params.bank_allocation_mode, &bcm_bank_allocation_mode);

        sal_strncpy(tcam_stage_str, dnx_field_tcam_stage_e_get_name(fg_params.stage),
                    (DIAG_DNX_FIELD_UTILS_STR_SIZE - 1));
        /** Using "utilex_str_split" to cut the short name of the TCAM stage. */
        tcam_stage_short_str = utilex_str_split(tcam_stage_str, "_", 10, &tcam_stage_realtokens);
        if ((tcam_stage_short_str == NULL) || (tcam_stage_short_str[tcam_stage_realtokens - 1] == NULL))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "No TCAM stage was foudn!!!\r\n\n");
        }
        /**
         * In case TCAM stage is IPMF1 the returned value in "fg_params" is "FIRST",
         * check it and set the relevant value "IPMF1" to be printed on the screen.
         */
        if (!(sal_strncmp(tcam_stage_short_str[tcam_stage_realtokens - 1], "FIRST", sizeof(tcam_stage_str) - 1)))
        {
            sal_strncpy(tcam_stage_str, "IPMF1", (DIAG_DNX_FIELD_UTILS_STR_SIZE - 1));
        }
        else
        {
            sal_strncpy(tcam_stage_str, tcam_stage_short_str[tcam_stage_realtokens - 1],
                        (DIAG_DNX_FIELD_UTILS_STR_SIZE - 1));
        }

        /** We should add the columns and table only once in the first iteration. */
        if (print_handler_table)
        {
            PRT_TITLE_SET("TCAM Handlers List");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
            PRT_COLUMN_ADD("IS_DT");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Size");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Size");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Prefix Value");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Prefix Size");
            PRT_COLUMN_ADD("AP_DB_ID");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bank Allocation Mode");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Dbal Table");

            print_handler_table = FALSE;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("%d", handler_id);
        PRT_CELL_SET("%s", tcam_stage_str);
        if (fg_params.direct_table)
        {
            PRT_CELL_SET("%s", "Yes");
            /**
             * In case dt_bank_id is bigger than number of the big TCAM banks, it means that
             * the returned bank_id is small one.
             */
            if (fg_params.dt_bank_id > dnx_data_field.tcam.nof_big_banks_get(unit))
            {
                
                PRT_CELL_SET("%d", 8);
            }
            else
            {
                PRT_CELL_SET("%d", dnx_data_field.tcam.dt_max_key_size_get(unit));
            }
        }
        else
        {
            PRT_CELL_SET("%s", "No");
            PRT_CELL_SET("%d", fg_params.key_size);
        }
        PRT_CELL_SET("%d", fg_params.action_size);
        if (fg_params.direct_table)
        {
            PRT_CELL_SET("%s", "-");
            PRT_CELL_SET("%s", "-");
        }
        else
        {
            if (fg_params.prefix_value == DNX_FIELD_TCAM_PREFIX_VAL_AUTO)
            {
                PRT_CELL_SET("%s", "Not Allocated");
            }
            else
            {
                /** Buffer to store the five bits of the prefix_value. Which will be presented in binary format. */
                char prefix_value_binary_buffer[6];
                sal_itoa(prefix_value_binary_buffer, fg_params.prefix_value, 2, 0, fg_params.prefix_size);
                PRT_CELL_SET("0b%s", prefix_value_binary_buffer);
            }
            PRT_CELL_SET("%d", fg_params.prefix_size);
        }
        if (fg_params.key_size == 320)
        {
            /**
             * 320b key size use 2 APP_DB_IDs, the definition for double keys
             * (since both us and DBAL xml support only one APP_DB_ID value),
             * is that they use the given APP_DB_ID and the corresponding APP_DB_ID + APP_DB_ID_SIZE/2.
             * So, for the diag we should show, both APP_DB_ID and "APP_DB_ID + APP_DB_ID_SIZE/2".
             */
            PRT_CELL_SET("%d, %d", access_profile_id, DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(access_profile_id));
        }
        else
        {
            PRT_CELL_SET("%d", access_profile_id);
        }
        PRT_CELL_SET("%s", dnx_field_bcm_tcam_bank_allocation_mode_text(bcm_bank_allocation_mode));

        SHR_IF_ERR_EXIT(diag_dnx_field_tcam_handler_id_dbal_table_name_get(unit, handler_id, dbal_table_name));
        PRT_CELL_SET("%s", dbal_table_name);

        do_display = TRUE;
    }
    if (do_display)
    {
        PRT_COMMITX;
    }
    else
    {
        /*
         * If nothing was printed then just clear loaded PRT memory.
         * This could be left to 'exit' but it is clearer here.
         */
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO handlers in use were found!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }
exit:
    utilex_str_split_free(tcam_stage_short_str, tcam_stage_realtokens);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays all info about TCAM management for a given tcam handler id like,
 *   key size, action size, prefix value/size, Access Profile, BAM (Bank Allocation Mode),
 *   banks and entry usage.
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
diag_dnx_field_tcam_management_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_tcam_access_fg_params_t fg_params;
    uint32 handler_id;
    int access_profile_id;
    bcm_field_tcam_bank_allocation_mode_t bcm_bank_allocation_mode;
    uint32 bank_entries_count[2][DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS];
    uint32 banks_bmp[1];
    dnx_field_tcam_handler_state_e state;
    int bank_id_iter;
    int total_num_of_entries[2];
    char **tcam_stage_short_str = NULL;
    uint32 tcam_stage_realtokens = 0;
    char tcam_stage_str[DIAG_DNX_FIELD_UTILS_STR_SIZE];
    int core;
    char dbal_table_name[DBAL_MAX_STRING_LENGTH];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_HANDLER, handler_id);

    sal_memset(total_num_of_entries, 0, sizeof(total_num_of_entries));

    /** Verify if the given handler_id is in use, if not return an error. */
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_state_get(unit, handler_id, &state));
    if (state != DNX_FIELD_TCAM_HANDLER_STATE_OPEN)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "The given TCAM Handler Id %d is not in use!!!\r\n\n", handler_id);
    }

    /** Get TCAM handler information. */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, handler_id, &fg_params));
    /** Get Access Profile ID. */
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_access_profile_id_get(unit, handler_id, &access_profile_id));
    /** Convert the retrieved DNX bank allocation mode to the BCM equivalent. */
    dnx_field_map_bank_allocation_mode_dnx_to_bcm(unit, fg_params.bank_allocation_mode, &bcm_bank_allocation_mode);

    sal_strncpy(tcam_stage_str, dnx_field_tcam_stage_e_get_name(fg_params.stage), (DIAG_DNX_FIELD_UTILS_STR_SIZE - 1));
    /** Using "utilex_str_split" to cut the short name of the TCAM stage. */
    tcam_stage_short_str = utilex_str_split(tcam_stage_str, "_", 10, &tcam_stage_realtokens);
    if ((tcam_stage_short_str == NULL) || (tcam_stage_short_str[tcam_stage_realtokens - 1] == NULL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "No TCAM stage was found!!!\r\n\n");
    }
    /**
     * In case TCAM stage is IPMF1 the returned value in "fg_params" is "FIRST",
     * check it and set the relevant value "IPMF1" to be printed on the screen.
     */
    if (!(sal_strncmp(tcam_stage_short_str[tcam_stage_realtokens - 1], "FIRST", sizeof(tcam_stage_str) - 1)))
    {
        sal_strncpy(tcam_stage_str, "IPMF1", (DIAG_DNX_FIELD_UTILS_STR_SIZE - 1));
    }
    else
    {
        sal_strncpy(tcam_stage_str, tcam_stage_short_str[tcam_stage_realtokens - 1],
                    (DIAG_DNX_FIELD_UTILS_STR_SIZE - 1));
    }
    PRT_TITLE_SET("TCAM Handler %d Info", handler_id);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD("IS_DT");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Size");
    if (!fg_params.direct_table)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Prefix Value");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Prefix Size");
    }
    PRT_COLUMN_ADD("AP_DB_ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bank Allocation Mode");
    if (fg_params.direct_table)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "TCAM Bank");
    }
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Dbal Table");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%s", tcam_stage_str);
    if (fg_params.direct_table)
    {
        PRT_CELL_SET("%s", "Yes");
        /** 
         * In case dt_bank_id is bigger than number of the big TCAM banks, it means that
         * the returned bank_id is small one.
         */
        if (fg_params.dt_bank_id > dnx_data_field.tcam.nof_big_banks_get(unit))
        {
            
            PRT_CELL_SET("%d", 8);
        }
        else
        {
            PRT_CELL_SET("%d", dnx_data_field.tcam.dt_max_key_size_get(unit));
        }
    }
    else
    {
        PRT_CELL_SET("%s", "No");
        PRT_CELL_SET("%d", fg_params.key_size);
    }
    PRT_CELL_SET("%d", fg_params.action_size);
    if (!fg_params.direct_table)
    {
        if (fg_params.prefix_value == DNX_FIELD_TCAM_PREFIX_VAL_AUTO)
        {
            PRT_CELL_SET("%s", "Not Allocated");
        }
        else if (!fg_params.prefix_value)
        {
            PRT_CELL_SET("0b%s", "00000");
        }
        else
        {
            /** Buffer to store the five bits of the prefix_value. Which will be presented in binary format. */
            char prefix_value_binary_buffer[6];
            sal_itoa(prefix_value_binary_buffer, fg_params.prefix_value, 2, 0, 0);
            PRT_CELL_SET("0b%s", prefix_value_binary_buffer);
        }
        PRT_CELL_SET("%d", fg_params.prefix_size);
    }
    PRT_CELL_SET("%d", access_profile_id);
    PRT_CELL_SET("%s", dnx_field_bcm_tcam_bank_allocation_mode_text(bcm_bank_allocation_mode));
    if (fg_params.direct_table)
    {
        PRT_CELL_SET("%d", fg_params.dt_bank_id);
    }

    SHR_IF_ERR_EXIT(diag_dnx_field_tcam_handler_id_dbal_table_name_get(unit, handler_id, dbal_table_name));
    PRT_CELL_SET("%s", dbal_table_name);

    PRT_COMMITX;

    /** In case we are not DT, we should print the bank and entry usage. */
    if (!fg_params.direct_table)
    {
        /** Retrieve information about entries and used TCAM banks. */
        for (core = 0; core < DIAG_DNX_FIELD_TCAM_NOF_CORES; core++)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entries_info
                            (unit, core, handler_id, bank_entries_count[core], banks_bmp));
        }

        PRT_TITLE_SET("Bank and Entry Info");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Banks");

        /** Iterate over all TCAM banks to check which of them is being used. */
        for (bank_id_iter = 0; bank_id_iter < DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS; bank_id_iter++)
        {
            /** In case we have a match in the banks bitmap, a column with the used bank ID will be added. */
            if (SHR_BITGET(banks_bmp, bank_id_iter))
            {
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID %d", bank_id_iter);
            }
        }
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Total");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Nof entries");
        /** Iterate over all TCAM banks to check which of them is being used. */
        for (bank_id_iter = 0; bank_id_iter < DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS; bank_id_iter++)
        {
            /**
             * In case we have a match in the banks bitmap, information about how much entries
             * in this bank have been allocated will be present (taken from bank_entries_count array).
             */
            if (SHR_BITGET(banks_bmp, bank_id_iter))
            {
                int nof_entries_per_core[2];
                for (core = 0; core < DIAG_DNX_FIELD_TCAM_NOF_CORES; core++)
                {
                    nof_entries_per_core[core] = bank_entries_count[core][bank_id_iter];

                    /**
                     * Calculate the total number of entries on all used banks.
                     * Will be exposed in the end of the "Bank and Entry Info" table.
                     */
                    total_num_of_entries[core] += bank_entries_count[core][bank_id_iter];
                }
                if (DIAG_DNX_FIELD_TCAM_NOF_CORES == 1)
                {
                    PRT_CELL_SET("core_0: %d\ncore_1: N/A", nof_entries_per_core[0]);
                }
                else
                {
                    PRT_CELL_SET("core_0: %d\ncore_1: %d", nof_entries_per_core[0], nof_entries_per_core[1]);
                }
            }
        }
        if (DIAG_DNX_FIELD_TCAM_NOF_CORES == 1)
        {
            PRT_CELL_SET("core_0: %d\ncore_1: N/A", total_num_of_entries[0]);
        }
        else
        {
            PRT_CELL_SET("core_0: %d\ncore_1: %d", total_num_of_entries[0], total_num_of_entries[1]);
        }

        PRT_COMMITX;
    }

exit:
    utilex_str_split_free(tcam_stage_short_str, tcam_stage_realtokens);
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
 *   List of shell options for 'tcam bank' shell commands (info)
 */
sh_sand_cmd_t Sh_dnx_field_tcam_bank_cmds[] = {
    {"info", diag_dnx_field_tcam_bank_info_cb, NULL, Field_tcam_bank_info_options, &Field_tcam_bank_info_man}
    ,
    {NULL}
};

/**
 * \brief
 *   List of shell options for 'tcam management' shell commands (list, info)
 */
sh_sand_cmd_t Sh_dnx_field_tcam_management_cmds[] = {
    {"list", diag_dnx_field_tcam_management_list_cb, NULL, Field_tcam_management_list_options,
     &Field_tcam_management_list_man}
    ,
    {"info", diag_dnx_field_tcam_management_info_cb, NULL, Field_tcam_management_info_options,
     &Field_tcam_management_info_man}
    ,
    {NULL}
};

/**
 * \brief
 *   List of shell options for 'tcam' shell commands (bank, management)
 */
sh_sand_cmd_t Sh_dnx_field_tcam_cmds[] = {
    {"bank", NULL, Sh_dnx_field_tcam_bank_cmds, NULL, &field_tcam_bank_man}
    ,
    {"management", NULL, Sh_dnx_field_tcam_management_cmds, NULL, &field_tcam_management_man}
    ,
    {NULL}
};

/*
 * }
 */
