/** \file diag_dnx_field_entry.c
 *
 * Diagnostics procedures, for DNX, for 'entry' operations.
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
#include "diag_dnx_field_entry.h"
#include <bcm_int/dnx/field/field_entry.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/field/tcam/tcam_location_algo.h>
#include <bcm_int/dnx/field/tcam/tcam_bank_manager.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <sal/core/libc.h>
#include <soc/sand/sand_signals.h>
#include "diag_dnx_field_utils.h"

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

/*
 * }
 */
/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */

/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function allocates some resources to be used
 *   for testing during the 'field entry list/info exec' command.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_init(
    int unit)
{
    bcm_field_group_t fg_id;
    bcm_field_entry_t entry_handle;
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t entry_info;
    void *dest_char;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_entry_info_t_init(&entry_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "ENTRY_DIAG_INIT", sizeof(fg_info.name));
    fg_info.nof_quals = 3;
    fg_info.nof_actions = 3;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;

    fg_info.qual_types[0] = bcmFieldQualifyGeneratedTtl;
    fg_info.qual_types[1] = bcmFieldQualifyIntPriority;
    fg_info.qual_types[2] = bcmFieldQualifyPacketIsIEEE1588;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 1;
    entry_info.entry_qual[0].mask[0] = 0xFF;
    entry_info.entry_qual[1].type = fg_info.qual_types[1];
    entry_info.entry_qual[1].value[0] = 2;
    entry_info.entry_qual[1].mask[0] = 0x7;
    entry_info.entry_qual[2].type = fg_info.qual_types[2];
    entry_info.entry_qual[2].value[0] = 1;
    entry_info.entry_qual[2].mask[0] = 0x1;

    fg_info.action_types[0] = bcmFieldActionAdmitProfile;
    fg_info.action_with_valid_bit[0] = TRUE;
    fg_info.action_types[1] = bcmFieldActionPrioIntNew;
    fg_info.action_with_valid_bit[1] = TRUE;
    fg_info.action_types[2] = bcmFieldActionPphPresentSet;
    fg_info.action_with_valid_bit[2] = TRUE;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = 3;
    entry_info.entry_action[1].type = fg_info.action_types[1];
    entry_info.entry_action[1].value[0] = 4;
    entry_info.entry_action[2].type = fg_info.action_types[2];
    entry_info.entry_action[2].value[0] = 1;

    fg_id = 70;
    SHR_IF_ERR_EXIT(bcm_field_group_add
                    (unit, (BCM_FIELD_FLAG_MSB_RESULT_ALIGN | BCM_FIELD_FLAG_WITH_ID), &fg_info, &fg_id));
    entry_handle = DNX_FIELD_TCAM_ENTRY(200, DNX_FIELD_TCAM_CORE_ALL);
    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, BCM_FIELD_FLAG_WITH_ID, fg_id, &entry_info, &entry_handle));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays all entries for given TCAM FG.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core ID.
 * \param [in] fg_id - Id of the given TCAM FG.
 * \param [in] fg_type - Type of the given External FG.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_list_tcam_cb(
    int unit,
    int core_id,
    dnx_field_group_t fg_id,
    dnx_field_group_type_e fg_type,
    sh_sand_control_t * sand_control)
{
    uint32 entry_iter;
    uint32 next_iter;
    int bank_id_iter;
    uint32 tcam_handler_id;
    dbal_tables_e dbal_table_id;
    uint32 bank_entries_count[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS];
    uint32 banks_bmp[1];
    uint32 nof_available_entries;
    int tmp_buff_length;
    int entry_ids_string_length;
    int do_display, do_bank_display;
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    int core_index, nof_cores;
    dnx_field_tcam_location_t location;
    dnx_field_key_length_type_e key_length_type;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    tmp_buff_length = 0;
    entry_ids_string_length = 0;

    do_display = FALSE;
    do_bank_display = FALSE;

    /** Get the number of cores from DNX data. */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_key_length_type_get(unit, fg_id, &key_length_type));

    /** Retrieve information about all entries, which are being add to the current FG, and used TCAM banks. */
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table_id, &tcam_handler_id));

    for (core_index = 0; core_index < nof_cores; core_index++)
    {
        /**
         * In case not ALL, set the core_index to be equal to
         * the cire_id given by the user.
         */
        if (core_id != _SHR_CORE_ALL)
        {
            core_index = core_id;
        }

        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entries_info
                        (unit, core_index, tcam_handler_id, bank_entries_count, banks_bmp));

        /** Iterate over all TCAM banks and print those, which are used by the current FG. */
        for (bank_id_iter = 0; bank_id_iter < DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS; bank_id_iter++)
        {
            char entry_ids_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            if (SHR_BITGET(banks_bmp, bank_id_iter))
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_bank_available_entries_get
                                (unit, core_index, bank_id_iter, 0, &nof_available_entries));
                if (key_length_type == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
                {
                    PRT_TITLE_SET("FG ID %d (%s) | CORE ID %d | BANK ID %d, %d", fg_id, converted_fg_type, core_index,
                                  bank_id_iter, (bank_id_iter + 1));
                }
                else
                {
                    PRT_TITLE_SET("FG ID %d (%s) | CORE ID %d | BANK ID %d", fg_id, converted_fg_type, core_index,
                                  bank_id_iter);
                }
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Number of Free Entries: %d       Used Entry IDs:",
                                    nof_available_entries);
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);

                /*
                 * TCAM handler provides an iterator to loop on all entries for the field group
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_first(unit, core_index, tcam_handler_id, &entry_iter));
                while (entry_iter != DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
                {
                    char ent_id_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_next
                                    (unit, core_index, tcam_handler_id, entry_iter, &next_iter));

                    /** Check if the current entry belongs to the current bank ID. */
                    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_location(unit, core_index, entry_iter, &location));
                    if (location.bank_id != bank_id_iter)
                    {
                        entry_iter = next_iter;
                        continue;
                    }

                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "0x%06X, ", entry_iter);
                    sal_snprintf(ent_id_buff, sizeof(ent_id_buff), "0x%06X, ", entry_iter);
                    entry_ids_string_length = sal_strlen(entry_ids_string);
                    tmp_buff_length = sal_strlen(ent_id_buff);
                    if ((entry_ids_string_length + tmp_buff_length) < RHSTRING_MAX_SIZE)
                    {
                        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(entry_ids_string, "%s", ent_id_buff);
                        sal_strncat_s(entry_ids_string, ent_id_buff, sizeof(entry_ids_string));
                    }
                    else
                    {
                        PRT_CELL_SET("%s", entry_ids_string);
                        sal_strncpy(entry_ids_string, "", sizeof(entry_ids_string) - 1);
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(entry_ids_string, "%s", ent_id_buff);
                        sal_strncat_s(entry_ids_string, ent_id_buff, sizeof(entry_ids_string));
                    }
                    entry_iter = next_iter;
                }
                do_display = TRUE;
                do_bank_display = TRUE;
                if (key_length_type == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
                {
                    bank_id_iter++;
                }
            }
            if (do_bank_display)
            {
                /**
                 * Extract last 2 symbols of the constructed string
                 * to not present comma and space ", " at the end of it.
                 */
                entry_ids_string[sal_strlen(entry_ids_string) - 2] = '\0';
                PRT_CELL_SET("%s", entry_ids_string);
                PRT_COMMITX;

                do_bank_display = FALSE;
            }
        }

        /**
         * If core_id is not all cores, it means that we need to present info only for a specific
         * core, which is given by the user, in such case break after the first iteration.
         */
        if (core_id != _SHR_CORE_ALL)
        {
            break;
        }
    }

    if (!do_display)
    {
        /*
         * If nothing was printed then just clear loaded PRT memory.
         * This could be left to 'exit' but it is clearer here.
         */
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO entries were found on a specific filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays all entries for given External FG.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Id of the given External FG.
 * \param [in] fg_type - Type of the given External FG.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_list_external_cb(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_type_e fg_type,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 access_id;
    dbal_tables_e dbal_table_id;
    char entry_ids_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    int tmp_buff_length;
    int entry_ids_string_length;
    int do_display;
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    uint32 db_id;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    tmp_buff_length = 0;
    entry_ids_string_length = 0;

    do_display = FALSE;

    SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, dbal_table_id, &db_id, NULL));

    PRT_TITLE_SET("FG ID %d (%s) | DB ID %d", fg_id, converted_fg_type, db_id);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Used Entry IDs:");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    /** External (KBP) field groups can only have entries after the KBP device lock. */
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (DNX_KBP_STATUS_IS_LOCKED(kbp_mngr_status))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            char ent_id_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];

            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_get(unit, entry_handle_id, &access_id));

            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                  "0x%08X, ", DNX_FIELD_ENTRY(DNX_FIELD_TCAM_EXTERNAL, access_id, fg_id,
                                                                              0));
            sal_snprintf(ent_id_buff, sizeof(ent_id_buff), "0x%08X, ",
                         DNX_FIELD_ENTRY(DNX_FIELD_TCAM_EXTERNAL, access_id, fg_id, 0));
            entry_ids_string_length = sal_strlen(entry_ids_string);
            tmp_buff_length = sal_strlen(ent_id_buff);
            if ((entry_ids_string_length + tmp_buff_length) < RHSTRING_MAX_SIZE)
            {
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(entry_ids_string, "%s", ent_id_buff);
                sal_strncat_s(entry_ids_string, ent_id_buff, sizeof(entry_ids_string));
            }
            else
            {
                PRT_CELL_SET("%s", entry_ids_string);
                sal_strncpy(entry_ids_string, "", sizeof(entry_ids_string) - 1);
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(entry_ids_string, "%s", ent_id_buff);
                sal_strncat_s(entry_ids_string, ent_id_buff, sizeof(entry_ids_string));
            }

            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

            do_display = TRUE;
        }
    }
    if (do_display)
    {
        entry_ids_string[sal_strlen(entry_ids_string) - 2] = '\0';
        PRT_CELL_SET("%s", entry_ids_string);
        PRT_COMMITX;
    }
    else
    {
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO entries were found on specified filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays all entries for given DT FG.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core ID.
 * \param [in] fg_id - Id of the given TCAM FG.
 * \param [in] fg_type - Type of the given External FG.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_list_dt_cb(
    int unit,
    int core_id,
    dnx_field_group_t fg_id,
    dnx_field_group_type_e fg_type,
    sh_sand_control_t * sand_control)
{
    uint32 entry_iter, entry_handle;
    int bank_id_iter;
    uint32 tcam_handler_id;
    dbal_tables_e dbal_table_id;
    uint32 bank_entries_count[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS];
    uint32 banks_bmp[1];
    uint32 nof_available_entries;
    int tmp_buff_length;
    int entry_ids_string_length;
    int do_display, do_bank_display;
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    int core_index, nof_cores;
    uint32 max_nof_dt_entries;
    dnx_field_entry_t entry_info;
    uint8 entry_found;
    dnx_field_tcam_location_t location;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    tmp_buff_length = 0;
    entry_ids_string_length = 0;

    do_display = FALSE;
    do_bank_display = FALSE;

    /** Get the number of cores from DNX data. */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    /** Set max number of DT entries, to iterate on, depends on the max DT key size. */
    max_nof_dt_entries = utilex_power_of_2(dnx_data_field.tcam.dt_max_key_size_get(unit));

    SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));

    /** Retrieve information about all entries, which are being add to the current FG, and used TCAM banks. */
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table_id, &tcam_handler_id));

    for (core_index = 0; core_index < nof_cores; core_index++)
    {
        /**
         * In case not ALL, set the core_index to be equal to
         * the cire_id given by the user.
         */
        if (core_id != _SHR_CORE_ALL)
        {
            core_index = core_id;
        }

        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entries_info
                        (unit, core_index, tcam_handler_id, bank_entries_count, banks_bmp));

        /** Iterate over all TCAM banks and print those, which are used by the current FG. */
        for (bank_id_iter = 0; bank_id_iter < DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS; bank_id_iter++)
        {
            char entry_ids_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            if (SHR_BITGET(banks_bmp, bank_id_iter))
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_utils_dt_available_entries_get
                                (unit, fg_id, core_index, &nof_available_entries));
                PRT_TITLE_SET("FG ID %d (%s) | CORE ID %d | BANK ID %d", fg_id, converted_fg_type, core_index,
                              bank_id_iter);
                PRT_COLUMN_ADD("Number of Free Entries: %d       Used Entry IDs:", nof_available_entries);
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);

                for (entry_iter = 0; entry_iter < max_nof_dt_entries; entry_iter++)
                {
                    char ent_id_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];

                    entry_handle = DNX_FIELD_TCAM_ENTRY(entry_iter, core_index);

                    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_id_exists
                                    (unit, core_index, entry_handle, &entry_found));
                    if (!entry_found)
                    {
                        continue;
                    }

                    /** Check if the current entry belongs to the current bank ID. */
                    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_location(unit, core_index, entry_handle, &location));
                    if (location.bank_id != bank_id_iter)
                    {
                        continue;
                    }

                    /** Get information about the given Entry. */
                    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
                    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_get(unit, fg_id, entry_handle, &entry_info));

                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "0x%06X, ", entry_handle);
                    sal_snprintf(ent_id_buff, sizeof(ent_id_buff), "0x%06X, ", entry_handle);
                    entry_ids_string_length = sal_strlen(entry_ids_string);
                    tmp_buff_length = sal_strlen(ent_id_buff);
                    if ((entry_ids_string_length + tmp_buff_length) < RHSTRING_MAX_SIZE)
                    {
                        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(entry_ids_string, "%s", ent_id_buff);
                        sal_strncat_s(entry_ids_string, ent_id_buff, sizeof(entry_ids_string));
                    }
                    else
                    {
                        PRT_CELL_SET("%s", entry_ids_string);
                        sal_strncpy(entry_ids_string, "", sizeof(entry_ids_string) - 1);
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(entry_ids_string, "%s", ent_id_buff);
                        sal_strncat_s(entry_ids_string, ent_id_buff, sizeof(entry_ids_string));
                    }
                }
                do_display = TRUE;
                do_bank_display = TRUE;
            }
            if (do_bank_display)
            {
                /**
                 * Extract last 2 symbols of the constructed string
                 * to not present comma and space ", " at the end of it.
                 */
                entry_ids_string[sal_strlen(entry_ids_string) - 2] = '\0';
                PRT_CELL_SET("%s", entry_ids_string);
                PRT_COMMITX;

                do_bank_display = FALSE;
            }
        }

        /**
         * If core_id is not all cores, it means that we need to present info only for a specific
         * core, which is given by the user, in such case break after the first iteration.
         */
        if (core_id != _SHR_CORE_ALL)
        {
            break;
        }
    }

    if (!do_display)
    {
        /*
         * If nothing was printed then just clear loaded PRT memory.
         * This could be left to 'exit' but it is clearer here.
         */
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO entries were found on a specific filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays all entries for given FG ID, specified by the caller.
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
diag_dnx_field_entry_list_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_allocated;
    dnx_field_group_t fg_id;
    dnx_field_group_type_e fg_type;
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    bcm_core_t core_id;
    char fg_name[DBAL_MAX_STRING_LENGTH];

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_GROUP, fg_id);
    SH_SAND_GET_INT32(DIAG_DNX_FIELD_OPTION_CORE, core_id);

    /** Check if the specified FG ID or rage of FG IDs are allocated. */
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (!is_allocated)
    {
        LOG_CLI_EX("\r\n" "The given FG Id %d is not allocated!!!%s%s%s\r\n\n", fg_id, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    /** Print Entry information only for supported FG Types. */
    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_entry_list_tcam_cb(unit, core_id, fg_id, fg_type, sand_control));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_entry_list_dt_cb(unit, core_id, fg_id, fg_type, sand_control));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_entry_list_external_cb(unit, fg_id, fg_type, sand_control));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            /** Get Field Group's name */
            SHR_IF_ERR_EXIT(dnx_field_group_name_get(unit, fg_id, fg_name));
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));
            LOG_CLI_EX("\r\n"
                       "Please use DBAL diagnostics instead to query on entries of Field groups of type %s, DBAL table name: %s%s%s\r\n\n",
                       converted_fg_type, fg_name, EMPTY, EMPTY);
            break;
        }
        default:
        {
            /** None of the supported types. */
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));

            LOG_CLI_EX("\r\n" "Field Group Type %s is not supported by Entry Info diagnostic!! %s%s%s\r\n\n",
                       converted_fg_type, EMPTY, EMPTY, EMPTY);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints basic info about a TCAM entry like FG id, entry id, bank id and bank offset.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Id of the current FG.
 * \param [in] dt_bank_id - Id of the current DT FG.
 * \param [in] entry_id - Id of the current Entry.
 * \param [in] entry_info_p - Pointer, which contains all entry related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_info_tcam_print(
    int unit,
    dnx_field_group_t fg_id,
    uint32 dt_bank_id,
    uint32 entry_id,
    dnx_field_entry_t * entry_info_p,
    sh_sand_control_t * sand_control)
{
    dnx_field_tcam_location_t location;
    dnx_field_tcam_core_e core_id, core_index;

    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    dnx_field_group_type_e fg_type;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    /** None of the supported types. */
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));

    PRT_TITLE_SET("Entry Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bank ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bank Offset");
    if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry Priority");
    }
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Core ID");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%d", fg_id);
    PRT_CELL_SET("%s", converted_fg_type);
    PRT_CELL_SET("0x%06X", entry_id);

    /** Get the core_id from the given entry_handle. */
    core_id = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry_id);

    if (core_id == DNX_FIELD_TCAM_CORE_ALL)
    {
        core_index = 0;
    }
    else
    {
        core_index = core_id;
    }

    if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        PRT_CELL_SET("%d", dt_bank_id);
        PRT_CELL_SET("%d", DNX_FIELD_ENTRY_ACCESS_ID(entry_id) * 2);
    }
    else
    {
        dnx_field_key_length_type_e key_length_type;
        /** Take and present the bank id and bank offset of the current entry. */
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_location(unit, core_index, entry_id, &location));
        SHR_IF_ERR_EXIT(dnx_field_group_key_length_type_get(unit, fg_id, &key_length_type));

        /** In case of double key we should present all used banks. */
        if (key_length_type == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
        {
            PRT_CELL_SET("%d, %d", location.bank_id, (location.bank_id + 1));
        }
        else
        {
            PRT_CELL_SET("%d", location.bank_id);
        }
        PRT_CELL_SET("%d", location.bank_offset);

        PRT_CELL_SET("%d", entry_info_p->priority);
    }

    if (core_id == DNX_FIELD_TCAM_CORE_ALL)
    {
        PRT_CELL_SET("%s", "All");
    }
    else
    {
        PRT_CELL_SET("%d", core_id);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints basic info about an External entry like FG id.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Id of the current FG.
 * \param [in] entry_id - Id of the current Entry.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_info_external_print(
    int unit,
    dnx_field_group_t fg_id,
    uint32 entry_id,
    sh_sand_control_t * sand_control)
{
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    dnx_field_group_type_e fg_type;
    dbal_tables_e dbal_table_id;
    uint32 db_id;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    /** None of the supported types. */
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, dbal_table_id, &db_id, NULL));

    PRT_TITLE_SET("Entry Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DB ID");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%d", fg_id);
    PRT_CELL_SET("%s", converted_fg_type);
    PRT_CELL_SET("0x%X", entry_id);
    PRT_CELL_SET("%d", db_id);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints basic info about a EXEM entry like FG id, EXEM Type, Stage.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Id of the current FG.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_info_exem_print(
    int unit,
    dnx_field_group_t fg_id,
    sh_sand_control_t * sand_control)
{
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    dnx_field_group_type_e fg_type;
    dnx_field_stage_e fg_stage;
    bcm_field_stage_t bcm_stage;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    /** None of the supported types. */
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &fg_stage));
    /** Convert DNX to BCM Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, fg_stage, &bcm_stage));

    PRT_TITLE_SET("Entry Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EXEM Type");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%d", fg_id);
    PRT_CELL_SET("%s", converted_fg_type);
    PRT_CELL_SET("%s", dnx_field_bcm_stage_text(bcm_stage));

    if (fg_stage == DNX_FIELD_STAGE_IPMF1 || fg_stage == DNX_FIELD_STAGE_EPMF)
    {
        PRT_CELL_SET("%s", "LEXEM");
    }
    else
    {
        PRT_CELL_SET("%s", "SEXEM");
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all needed information about used qualifiers by a specific Entry.
 * \param [in] unit - The unit number.
 * \param [in] qual_iter - Id of the current qualifier.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] entry_info_p - Pointer, which contains all Entry related information.
 * \param [out] start_bit_p - In-out parameter, which is used to store the offset where the
 *  next qualifier value should be written to qual_raw_data_buff_p.
 * \param [out] qual_raw_data_val_buff_p - The qualifier raw data buffer, which is used to store
 *  the values for all entry quals.
 * \param [out] qual_raw_data_mask_buff_p - The qualifier raw data buffer, which is used to store
 *  the mask values for all entry quals.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_info_qual_print(
    int unit,
    int qual_iter,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_entry_t * entry_info_p,
    int *start_bit_p,
    uint32 *qual_raw_data_val_buff_p,
    uint32 *qual_raw_data_mask_buff_p,
    prt_control_t * prt_ctr)
{
    int qual_val_mask_iter;
    int qual_valid;
    char qual_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char qual_max_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char qual_mask_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    int ent_qual_iter;
    int max_val_iter;

    SHR_FUNC_INIT_VARS(unit);

    qual_valid = FALSE;
    ent_qual_iter = 0;
    max_val_iter = 0;

    /** Print all needed info about current qualifier. */
    PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                 (unit, fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type));
    PRT_CELL_SET("%d", fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size);
    if (fg_info_p->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_KBP)
    {
        /*
         * Should print real lsb from the dbal. this affects kbp, 80bit high key and others.
         */
        PRT_CELL_SET("%d", fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb);
    }

    if (fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_EXEM)
    {
        qual_valid = TRUE;
        ent_qual_iter = qual_iter;
    }
    else if (fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        /** Check if the current qual is valid for the entry. */
        qual_valid = utilex_bitstream_have_one_in_range(entry_info_p->key_info.qual_info[qual_iter].qual_value,
                                                        0,
                                                        WORDS2BITS(DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY) -
                                                        1);
        ent_qual_iter = qual_iter;
    }
    else
    {
        /** Iterate over entry actions array and check, which qualifiers are valid. */
        for (ent_qual_iter = 0; ent_qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             entry_info_p->key_info.qual_info[ent_qual_iter].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID; ent_qual_iter++)
        {
            if (fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type ==
                entry_info_p->key_info.qual_info[ent_qual_iter].dnx_qual)
            {
                qual_valid = TRUE;
                break;
            }
        }
    }

    /** If current qual is valid for the entry print its value and mask, if not valid print '-'. */
    if (qual_valid == TRUE)
    {
        uint8 is_qual_ranged = fg_info_p->group_basic_info.qual_is_ranged[qual_iter];

        /** Set the right, max value to iterate on, depends on current qual_size. */
        max_val_iter = BITS2WORDS(fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size);

        sal_strncpy(qual_val_string, "0x", sizeof(qual_val_string) - 1);
        sal_strncpy(qual_max_val_string, "0x", sizeof(qual_max_val_string) - 1);
        sal_strncpy(qual_mask_string, "0x", sizeof(qual_mask_string) - 1);
        for (qual_val_mask_iter = max_val_iter - 1; qual_val_mask_iter >= 0; qual_val_mask_iter--)
        {
            char qual_val_mas_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];

            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                  "%08X",
                                                  entry_info_p->key_info.
                                                  qual_info[ent_qual_iter].qual_value[qual_val_mask_iter]);
            sal_snprintf(qual_val_mas_buff, sizeof(qual_val_mas_buff), "%08X",
                         entry_info_p->key_info.qual_info[ent_qual_iter].qual_value[qual_val_mask_iter]);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_val_string, "%s", qual_val_mas_buff);
            sal_strncat_s(qual_val_string, qual_val_mas_buff, sizeof(qual_val_string));

            if (is_qual_ranged)
            {
                sal_strncpy(qual_val_mas_buff, "", sizeof(qual_val_mas_buff) - 1);
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                      "%08X",
                                                      entry_info_p->key_info.
                                                      qual_info[ent_qual_iter].qual_max_value[qual_val_mask_iter]);
                sal_snprintf(qual_val_mas_buff, sizeof(qual_val_mas_buff), "%08X",
                             entry_info_p->key_info.qual_info[ent_qual_iter].qual_max_value[qual_val_mask_iter]);
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_max_val_string, "%s", qual_val_mas_buff);
                sal_strncat_s(qual_max_val_string, qual_val_mas_buff, sizeof(qual_max_val_string));
            }

            sal_strncpy(qual_val_mas_buff, "", sizeof(qual_val_mas_buff) - 1);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                  "%08X",
                                                  entry_info_p->key_info.
                                                  qual_info[ent_qual_iter].qual_mask[qual_val_mask_iter]);
            sal_snprintf(qual_val_mas_buff, sizeof(qual_val_mas_buff), "%08X",
                         entry_info_p->key_info.qual_info[ent_qual_iter].qual_mask[qual_val_mask_iter]);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_mask_string, "%s", qual_val_mas_buff);
            sal_strncat_s(qual_mask_string, qual_val_mas_buff, sizeof(qual_mask_string));
        }

        /** Store the qual values and masks in the qual_raw_data_val_buff_p and qual_raw_data_mask_buff_p buffers. */
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(entry_info_p->key_info.qual_info[ent_qual_iter].qual_value,
                                                       *start_bit_p,
                                                       fg_info_p->group_full_info.key_template.
                                                       key_qual_map[qual_iter].size, qual_raw_data_val_buff_p));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (entry_info_p->key_info.qual_info[ent_qual_iter].qual_mask, *start_bit_p,
                         fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size,
                         qual_raw_data_mask_buff_p));
        *start_bit_p += fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size;

        if (is_qual_ranged)
        {
            PRT_CELL_SET("Min: %s\nMax: %s", qual_val_string, qual_max_val_string);
        }
        else
        {
            if (sal_strncmp(qual_val_string, "0x", sizeof(qual_val_string)))
            {
                PRT_CELL_SET("%s", qual_val_string);
            }
            else
            {
                PRT_CELL_SET("%s", "0x0");
            }

        }
        if (fg_info_p->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_EXEM &&
            fg_info_p->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
        {
            if (sal_strncmp(qual_mask_string, "0x", sizeof(qual_mask_string)))
            {
                PRT_CELL_SET("%s", qual_mask_string);
            }
            else
            {
                PRT_CELL_SET("%s", "0x0");
            }
        }
    }
    else
    {
        PRT_CELL_SET("%s", "-");
        if (fg_info_p->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_EXEM &&
            fg_info_p->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
        {
            PRT_CELL_SET("%s", "-");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all needed information about used actions by a specific Entry.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Id of the current FG.
 * \param [in] action_iter - Id of the current action.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] entry_info_p - Pointer, which contains all Entry related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_info_action_print(
    int unit,
    dnx_field_group_t fg_id,
    int action_iter,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_entry_t * entry_info_p,
    prt_control_t * prt_ctr)
{
    int action_offset;
    unsigned int action_size;
    int act_val_iter;
    char action_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    int ent_action_iter;
    int do_cell_set;

    SHR_FUNC_INIT_VARS(unit);

    action_offset = 0;

    /** Get action size for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, fg_info_p->group_basic_info.field_stage,
                                                  fg_info_p->group_full_info.
                                                  actions_payload_info.actions_on_payload_info[action_iter].dnx_action,
                                                  &action_size));

    /** Take the offset for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_group_action_offset_get(unit, fg_id,
                                                      fg_info_p->group_full_info.
                                                      actions_payload_info.actions_on_payload_info[action_iter].
                                                      dnx_action, &action_offset));

    do_cell_set = FALSE;

    /** Iterate over entry actions array and check, which actions are valid. */
    for (ent_action_iter = 0; ent_action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         entry_info_p->payload_info.action_info[ent_action_iter].dnx_action != DNX_FIELD_ACTION_INVALID;
         ent_action_iter++)
    {
        if (fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[action_iter].dnx_action ==
            entry_info_p->payload_info.action_info[ent_action_iter].dnx_action)
        {
            do_cell_set = TRUE;
            break;
        }
    }

    /** Print all needed info about current action. */
    PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit,
                                                 fg_info_p->group_full_info.
                                                 actions_payload_info.actions_on_payload_info[action_iter].dnx_action));
    PRT_CELL_SET("%d", action_size);
    PRT_CELL_SET("%d", action_offset);

    if (do_cell_set)
    {
        sal_strncpy(action_val_string, "0x", sizeof(action_val_string) - 1);
        for (act_val_iter = DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY - 1; act_val_iter >= 0; act_val_iter--)
        {
            char act_val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                  "%X",
                                                  entry_info_p->payload_info.
                                                  action_info[ent_action_iter].action_value[act_val_iter]);
            sal_snprintf(act_val_buff, sizeof(act_val_buff), "%X",
                         entry_info_p->payload_info.action_info[ent_action_iter].action_value[act_val_iter]);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(action_val_string, "%s", act_val_buff);
            sal_strncat_s(action_val_string, act_val_buff, sizeof(action_val_string));
        }
        PRT_CELL_SET("%s", action_val_string);
    }
    else
    {
        PRT_CELL_SET("%s", "-");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints qualifier raw data (value and mask) about a specific Entry.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID to retrieve the its type, later stage can be retrieved as well.
 * \param [in] qual_raw_data_val_buff_p - The qualifier values raw data buffer.
 * \param [in] qual_raw_data_mask_buff_p - The qualifier masks raw data buffer.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_info_qual_raw_data_print(
    int unit,
    dnx_field_group_t fg_id,
    uint32 *qual_raw_data_val_buff_p,
    uint32 *qual_raw_data_mask_buff_p,
    sh_sand_control_t * sand_control)
{
    int qual_val_mask_iter;
    char qual_raw_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char qual_raw_mask_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    dnx_field_group_type_e fg_type;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    PRT_TITLE_SET("Entry Qual Raw Data");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Raw Value");
    if (fg_type != DNX_FIELD_GROUP_TYPE_EXEM && fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Raw Mask");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    sal_strncpy(qual_raw_val_string, "0x", sizeof(qual_raw_val_string) - 1);
    sal_strncpy(qual_raw_mask_string, "0x", sizeof(qual_raw_mask_string) - 1);
    for (qual_val_mask_iter =
         ((DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY * DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) - 1);
         qual_val_mask_iter >= 0; qual_val_mask_iter--)
    {
        char qual_val_mask_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%08X", qual_raw_data_val_buff_p[qual_val_mask_iter]);
        sal_snprintf(qual_val_mask_buff, sizeof(qual_val_mask_buff), "%08X",
                     qual_raw_data_val_buff_p[qual_val_mask_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_raw_val_string, "%s", qual_val_mask_buff);
        sal_strncat_s(qual_raw_val_string, qual_val_mask_buff, sizeof(qual_raw_val_string));

        sal_strncpy(qual_val_mask_buff, "", sizeof(qual_val_mask_buff) - 1);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%08X", qual_raw_data_mask_buff_p[qual_val_mask_iter]);
        sal_snprintf(qual_val_mask_buff, sizeof(qual_val_mask_buff), "%08X",
                     qual_raw_data_mask_buff_p[qual_val_mask_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_raw_mask_string, "%s", qual_val_mask_buff);
        sal_strncat_s(qual_raw_mask_string, qual_val_mask_buff, sizeof(qual_raw_mask_string));
    }

    PRT_CELL_SET("%s", qual_raw_val_string);
    if (fg_type != DNX_FIELD_GROUP_TYPE_EXEM && fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        PRT_CELL_SET("%s", qual_raw_mask_string);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints key and payload information about a specific Entry.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Id of the current FG.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] entry_info_p - Pointer, which contains all Entry related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_info_key_and_payload_print(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_entry_t * entry_info_p,
    sh_sand_control_t * sand_control)
{
    int qual_action_iter;
    int max_num_qual_actions;
    int qual_left, action_left;
    int start_bit;
    /** Multiply the max num of params per entry to the max kum of double keys, to store the possible 320 bits. */
    uint32 qual_raw_data_val_buff[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY *
                                  DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX] = { 0 };
    uint32 qual_raw_data_mask_buff[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY *
                                   DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(qual_raw_data_val_buff, 0, sizeof(qual_raw_data_val_buff));
    sal_memset(qual_raw_data_mask_buff, 0, sizeof(qual_raw_data_mask_buff));

    start_bit = 0;
    /** Used as flags to indicate if we have any quals/actions to print. */
    qual_left = action_left = TRUE;
    /** The max number of qualifiers and actions per field group (32). */
    max_num_qual_actions = MAX(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG, DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG);

    PRT_TITLE_SET("Key & Payload Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");
    if (fg_info_p->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_KBP)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Lsb");
    }
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Val");
    if (fg_info_p->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_EXEM &&
        fg_info_p->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Mask");
    }
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, " ");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Lsb");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Val");

    /** Iterate over maximum number of qualifiers and actions per FG, to retrieve information about these, which are valid. */
    for (qual_action_iter = 0; qual_action_iter < max_num_qual_actions; qual_action_iter++)
    {
        /**
         * Check if the qual_action_iter is still in the qualifiers range (DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) and if the
         * current qualifier is invalid (DNX_FIELD_QUAL_TYPE_INVALID), we should set the qual_left to FALSE,
         * which will indicate that we should stop printing any info related to qualifiers.
         */
        if (qual_action_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG
            && fg_info_p->group_full_info.key_template.key_qual_map[qual_action_iter].qual_type ==
            DNX_FIELD_QUAL_TYPE_INVALID)
        {
            qual_left = FALSE;
        }
        /**
         * Check if the qual_action_iter is still in the actions range (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) and if the
         * current action is invalid (DNX_FIELD_ACTION_INVALID), we should set the action_left to FALSE,
         * which will indicate that we should stop printing any info related to actions.
         */
        if (qual_action_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG
            && fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[qual_action_iter].dnx_action ==
            DNX_FIELD_ACTION_INVALID)
        {
            action_left = FALSE;
        }
        /**
         * In case we reached the end of both qualifiers and actions arrays,
         * which means that qual_left and action_left flags are being set to FALSE.
         * We have to break and to stop printing information for both.
         */
        if (!qual_left && !action_left)
        {
            break;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        /**
         * If we still have any valid qualifiers (qual_left), then print information about them.
         * Otherwise skip the cells, which are relevant to the qualifiers,
         * as it is mentioned in the 'else' statement.
         */
        if (qual_left)
        {
            /** Print the qualifiers info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_entry_info_qual_print
                            (unit, qual_action_iter, fg_info_p, entry_info_p, &start_bit, qual_raw_data_val_buff,
                             qual_raw_data_mask_buff, prt_ctr));
        }
        else
        {
            if (fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_EXEM ||
                fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM ||
                fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_KBP)
            {
                PRT_CELL_SKIP(4);
            }
            else
            {
                /**
                 * Skip 5 cells (Qual, Size, Lsb, Val, Mask) about qualifier info if no more valid qualifiers were found,
                 * but we still have valid actions to be printed.
                 */
                PRT_CELL_SKIP(5);
            }
        }

        /** Set an empty column to separate qualifiers and actions. */
        PRT_CELL_SET(" ");

        /**
         * If we still have any valid actions (action_left), then print information about them.
         * Otherwise skip the cells, which are relevant to the actions,
         * as it is mentioned in the 'else' statement.
         */
        if (action_left)
        {
            /** Print the actions info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_entry_info_action_print
                            (unit, fg_id, qual_action_iter, fg_info_p, entry_info_p, prt_ctr));
        }
        else
        {
            /**
             * Skip 4 cells (Actions, Size, Lsb, Val) about action info if no more valid actions were found,
             * but we still have valid qualifiers to be printed.
             */
            PRT_CELL_SKIP(4);
        }
    }

    PRT_COMMITX;

    /** Print the entry qualifiers raw data (value and mask). */
    SHR_IF_ERR_EXIT(diag_dnx_field_entry_info_qual_raw_data_print
                    (unit, fg_id, qual_raw_data_val_buff, qual_raw_data_mask_buff, sand_control));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints key and payload information about EXEM Entries.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Id of the current FG.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_info_exem_key_and_payload_print(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    int is_end;
    int fg_qual_iter;
    int fg_act_iter;
    dnx_field_entry_t entry_info;
    uint32 entry_id;
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    unsigned int nof_quals;
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    unsigned int nof_actions;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    entry_id = 0;

    /** Get the information about all of the entries of the given EXEM DB. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, fg_info_p->group_full_info.dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_qual_arr_get(unit, fg_id, dnx_quals, &nof_quals));
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_action_arr_get(unit, fg_id, dnx_actions, &nof_actions));
    /** Get the first entry of the table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    /** Until is_end is FALSE, iterate over all entries end present information about them. */
    while (!is_end)
    {
        int entry_act_iter = 0;
        SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
        /** Iterate over number of key fields for the EXEM DB, which are corresponding to the key qualifiers. */
        for (fg_qual_iter = 0; fg_qual_iter < nof_quals; fg_qual_iter++)
        {
            dbal_fields_e field_id;
            /** Set the qualifier type, in entry info, as it is in the field group. */
            entry_info.key_info.qual_info[fg_qual_iter].dnx_qual = fg_info_p->group_basic_info.dnx_quals[fg_qual_iter];

            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                            (unit, fg_info_p->group_basic_info.field_stage,
                             entry_info.key_info.qual_info[fg_qual_iter].dnx_qual, &field_id));
            /** Get the value of the current qualifier. */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, field_id,
                                                                  entry_info.key_info.
                                                                  qual_info[fg_qual_iter].qual_value));
        }

        for (fg_act_iter = 0; fg_act_iter < nof_actions; fg_act_iter++)
        {
            dbal_fields_e field_id;
            shr_error_e rv;

            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                            (unit, fg_info_p->group_basic_info.field_stage, dnx_actions[fg_act_iter], &field_id));
            rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE,
                                                         entry_info.payload_info.
                                                         action_info[entry_act_iter].action_value);
            if (rv == _SHR_E_NONE)
            {
                /** Set actions only valid to the entry. */
                entry_info.payload_info.action_info[entry_act_iter].dnx_action = dnx_actions[fg_act_iter];
                entry_act_iter++;
            }
            else if (rv == _SHR_E_NOT_FOUND)
            {
                continue;
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }

        bsl_printf("****************************** ENTRY %d ******************************\r\n\n", entry_id);

        /**
         * Prepare and print a table, which contains information about key and payload
         * of a specific entry of given EXEM field group.
         */
        SHR_IF_ERR_EXIT(diag_dnx_field_entry_info_key_and_payload_print
                        (unit, fg_id, fg_info_p, &entry_info, sand_control));

        entry_id++;
        /** Get the next entry of the table. If not exist is_end will be TRUE. */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    if (entry_id == 0)
    {
        LOG_CLI_EX("\r\n" "No entries were found for the give FG ID %d!!!%s%s%s\r\n\n", fg_id, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays full info about given TCAM Entry, specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] entry_id - Id of the given TCAM Entry.
 * \param [in] fg_id - Id of the given TCAM FG.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_info_tcam_cb(
    int unit,
    uint32 entry_id,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    dnx_field_entry_t entry_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Get information about the given Entry. */
    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_get(unit, fg_id, entry_id, &entry_info));

    /**
     * Prepare and print a table, which contains basic information about a specific entry of TCAM FG, like
     * FG id, entry id, bank id and bank offset.
     * In case of DT we parse the bank_id, which was returned in FG_INFO. Parsing only the first ID,
     * because DT FG can be allocated only on one bank.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_entry_info_tcam_print
                    (unit, fg_id, fg_info_p->group_full_info.tcam_info.tcam_bank_ids[0], entry_id, &entry_info,
                     sand_control));

    /**
     * Prepare and print a table, which contains information about key and payload
     * of a specific entry of given TCAM field group.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_entry_info_key_and_payload_print(unit, fg_id, fg_info_p, &entry_info, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays full info about given TCAM Entry, specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] entry_id - Id of the given TCAM Entry.
 * \param [in] fg_id - Id of the given TCAM FG.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_info_external_cb(
    int unit,
    uint32 entry_id,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    dnx_field_entry_t entry_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Get information about the given Entry. */
    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
    SHR_SET_CURRENT_ERR(dnx_field_entry_kbp_get(unit, fg_id, entry_id, &entry_info));
    if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
    {
        LOG_CLI_EX("\r\n" "The given entry %d (0x%08X) is invalid.\r\n%s%s", entry_id, entry_id, EMPTY, EMPTY);
        SHR_EXIT();
    }
    else if (SHR_GET_CURRENT_ERR() == _SHR_E_CONFIG)
    {
        LOG_CLI_EX("\r\n" "No external TCAM entries, as the device hasn't been locked yet.\r\n%s%s%s%s",
                   EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    /**
     * Prepare and print a table, which contains basic information about a specific entry of External FG, like
     * FG id, entry id.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_entry_info_external_print(unit, fg_id, entry_id, sand_control));

    /**
     * Prepare and print a table, which contains information about key and payload
     * of a specific entry of given External field group.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_entry_info_key_and_payload_print(unit, fg_id, fg_info_p, &entry_info, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays full info about given EXEM Entry, specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Id of the given EXEM FG.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_entry_info_exem_cb(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Prepare and print a table, which contains basic information about a specific entry of EXEM FG, like
     * FG id, EXEM Type, Stage.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_entry_info_exem_print(unit, fg_id, sand_control));

    /**
     * Prepare and print a table, which contains information about key and payload
     * of a specific entry of given EXEM field group.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_entry_info_exem_key_and_payload_print(unit, fg_id, fg_info_p, sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field Group full info about given Entry of a specific FG, specified by the caller.
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
diag_dnx_field_entry_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_allocated;
    uint32 entry_id;
    int entry_core;
    uint8 entry_found;

    dnx_field_group_t fg_id, fg_id_returned;
    dnx_field_group_full_info_t *fg_info = NULL;
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_GROUP, fg_id);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_ENTRY, entry_id);

    /** Check if the specified FG ID or rage of FG IDs are allocated. */
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (!is_allocated)
    {
        LOG_CLI_EX("\r\n" "The given FG Id %d is not allocated!!!%s%s%s\r\n\n", fg_id, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    SHR_ALLOC(fg_info, sizeof(dnx_field_group_full_info_t), "fg_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Get information about the given Field Group. */
    SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id, fg_info));

    /** Print Entry information only for supported FG Types. */
    switch (fg_info->group_basic_info.fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            if (entry_id == DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
            {
                LOG_CLI_EX("\r\n" "Presence of option:\"ENTry\" is requested!%s%s%s%s\r\n\n",
                           EMPTY, EMPTY, EMPTY, EMPTY);
                SHR_EXIT();
            }
            SHR_IF_ERR_EXIT(dnx_field_group_tcam_entry_fg_id(unit, entry_id, &fg_id_returned));
            if (fg_id != fg_id_returned)
            {
                LOG_CLI_EX("\r\n" "The given Entry Id %d (0x%08X) doesn't belong to the given FG Id %d !!!%s\r\n\n",
                           entry_id, entry_id, fg_id, EMPTY);
                SHR_EXIT();
            }
            entry_core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry_id);
            if (entry_core == DNX_FIELD_TCAM_CORE_ALL)
            {
                entry_core = DNX_FIELD_TCAM_CORE_FIRST;
            }
            SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_id_exists(unit, entry_core, entry_id, &entry_found));
            if (!entry_found)
            {
                LOG_CLI_EX("\r\n" "The given entry %d (0x%08X) does not exist.\r\n%s%s", entry_id, entry_id, EMPTY,
                           EMPTY);
                SHR_EXIT();
            }

            SHR_IF_ERR_EXIT(diag_dnx_field_entry_info_tcam_cb(unit, entry_id, fg_id, fg_info, sand_control));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            if (entry_id == DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
            {
                LOG_CLI_EX("\r\n" "Presence of option:\"ENTry\" is requested!%s%s%s%s\r\n\n",
                           EMPTY, EMPTY, EMPTY, EMPTY);
                SHR_EXIT();
            }
            fg_id_returned = DNX_FIELD_TCAM_EXTERNAL_ENTRY_FG_ID(entry_id);
            if (fg_id != fg_id_returned)
            {
                LOG_CLI_EX("\r\n" "The given Entry Id %d (0x%08X) doesn't belong to the given FG Id %d !!!%s\r\n\n",
                           entry_id, entry_id, fg_id, EMPTY);
                SHR_EXIT();
            }

            SHR_IF_ERR_EXIT(diag_dnx_field_entry_info_external_cb(unit, entry_id, fg_id, fg_info, sand_control));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_entry_info_exem_cb(unit, fg_id, fg_info, sand_control));
            break;
        }
        default:
        {
            /** None of the supported types. */
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion
                            (unit, fg_info->group_basic_info.fg_type, converted_fg_type));

            LOG_CLI_EX("\r\n" "Field Group Type %s is not supported by Entry Info diagnostic!! %s%s%s\r\n\n",
                       converted_fg_type, EMPTY, EMPTY, EMPTY);
            break;
        }
    }

exit:
    SHR_FREE(fg_info);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */
/**
 *  'Help' description for Entry display (shell commands).
 */
static sh_sand_man_t Field_entry_list_man = {
    .brief = "'Entry'- displays a list of entries which are being allocated for specific "
            "field group (filter by field group id and core id).",
    .full = "'Entry' displays a list of entries which are being allocated for specific \r\n"
            "field group (filter by field group id and core id). \r\n"
            "If no 'group' is specified then an error will occur.\r\n"
            "If no 'core' is specified then info for entries, which are valid for both core will be shown.\r\n"
            "For example: 'group=20'.",
    .synopsis = "[group=<0-127> core<0|1|ALL>]",
    .examples = "group=70 core=all",
    .init_cb = diag_dnx_field_entry_init,
};
static sh_sand_option_t Field_entry_list_options[] = {
    {DIAG_DNX_FIELD_OPTION_GROUP,  SAL_FIELD_TYPE_UINT32, "Field group to get its entries",       NULL,       (void *)Field_group_enum_table, "MIN-MAX"},
    {NULL}
};

/**
 *  'Help' description for Entry display (shell commands).
 */
static sh_sand_man_t Field_entry_info_man = {
    .brief = "'Entry'- displays the full info about a specific Entry of a specific FG (filter by field group id and entry id)",
    .full = "'Entry' displays info about a specific entry of a specific FG.\r\n"
      "If no 'group' and 'entry' are specified then an error will occur.\r\n"
      "'Entry' id is taken from the entry list diagnostic.\r\n"
      "For example 'group=50 entry=3276803'.",
    .synopsis = "[group=<0-127>] [entry=<entry_id>]",
    .examples = "group=70 entry=0x2000C8",
    .init_cb = diag_dnx_field_entry_init,
};
static sh_sand_option_t Field_entry_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_GROUP,          SAL_FIELD_TYPE_UINT32, "Field group to get its entry info",           NULL   ,(void *)Field_group_enum_table, "MIN-MAX"},
    {DIAG_DNX_FIELD_OPTION_ENTRY,          SAL_FIELD_TYPE_UINT32, "Entry to get its info",           "0xFFFFFFFF"   , NULL},
    {NULL}
};
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
 *   List of shell options for 'entry' shell commands (list, info)
 */
sh_sand_cmd_t Sh_dnx_field_entry_cmds[] = {
    {"list", diag_dnx_field_entry_list_cb, NULL, Field_entry_list_options, &Field_entry_list_man}
    ,
    {"info", diag_dnx_field_entry_info_cb, NULL, Field_entry_info_options, &Field_entry_info_man}
    ,
    {NULL}
};

/*
 * }
 */
