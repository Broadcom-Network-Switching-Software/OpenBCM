/** \file diag_dnx_field_ace.c
 *
 * Diagnostics procedures, for DNX, for 'ace' operations.
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
#include "diag_dnx_field_ace.h"
#include <bcm_int/dnx/field/field_group.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/dnx/mdb.h>
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
 *   for testing during the 'field ace entry info exec' command.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_ace_init(
    int unit)
{
    bcm_field_ace_format_t ace_format_id;
    uint32 entry_handle;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    void *dest_char;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_ace_format_info_t_init(&ace_format_info);
    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    dest_char = &(ace_format_info.name[0]);
    sal_strncpy_s(dest_char, "ACE_ENT_DIAG_INIT", sizeof(ace_format_info.name));
    ace_format_info.nof_actions = 2;
    ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;

    ace_format_info.action_types[0] = bcmFieldActionPrioIntNew;
    ace_format_info.action_with_valid_bit[0] = FALSE;
    ace_format_info.action_types[1] = bcmFieldActionTtlSet;
    ace_format_info.action_with_valid_bit[1] = FALSE;
    ace_entry_info.entry_action[0].type = ace_format_info.action_types[0];
    ace_entry_info.entry_action[0].value[0] = 3;
    ace_entry_info.entry_action[1].type = ace_format_info.action_types[1];
    ace_entry_info.entry_action[1].value[0] = 4;

    ace_format_id = 50;
    SHR_IF_ERR_EXIT(bcm_field_ace_format_add(unit, BCM_FIELD_FLAG_WITH_ID, &ace_format_info, &ace_format_id));
    entry_handle = 0x3C0100;
    SHR_IF_ERR_EXIT(bcm_field_ace_entry_add
                    (unit, BCM_FIELD_FLAG_WITH_ID, ace_format_id, &ace_entry_info, &entry_handle));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets and presents information about used used EFES instructions and
 *   number of presented ACE Formats.
 * \param [in] unit - The unit number.
 * \param [in] max_ace_formats_present - The number of ACE Format IDs,
 *  which are being presented.
 * \param [in] total_fes_mask_string_p - String, which is presenting used EFES instructions.
 * \param [in] total_fes_mask_usage_iter - Number of used EFES instructions.
 * \param [in] not_present_fes_mask_string_p - String, which is presenting not used EFES instructions.
 * \param [in] fes_masks_not_presented - Number of not used EFES instructions
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_ace_format_efes_masks_print(
    int unit,
    int max_ace_formats_present,
    char *total_fes_mask_string_p,
    int total_fes_mask_usage_iter,
    char *not_present_fes_mask_string_p,
    int fes_masks_not_presented,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("ACE Format List");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Total num of ACE formats presented");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Total EFES Mask usage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Masks not presented");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%d", max_ace_formats_present);
    PRT_CELL_SET("%d ( %s )", total_fes_mask_usage_iter, total_fes_mask_string_p);
    PRT_CELL_SET("%d ( %s )", fes_masks_not_presented, not_present_fes_mask_string_p);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays list of allocated ACE Formats per given Format ID or range of IDs, specified by the caller.
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
diag_dnx_field_ace_format_list_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_ace_allocated;
    int do_display;

    unsigned int payload_size_in_bits;

    dnx_field_ace_id_t format_id;
    dnx_field_ace_id_t format_id_lower, format_id_upper;
    dnx_field_ace_format_full_info_t *ace_format_id_info = NULL;
    uint8 nof_masks[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT];
    int max_nof_formats_present;

    /**
     * Following strings are used to store and present information about EFES instructions.
     * Their sizes are calculated using maximum number if FES instructions (8) per ACE ID plus one,
     * because of the terminating zero at the end of the arrays.
     */
    char fes_mask_string[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT + 1] = { 0 };
    char total_fes_mask_string[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT + 1] = { 0 };
    char not_present_fes_mask_string[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT + 1] = { 0 };
    int fes_inst_iter;
    int fes_mask_usage_iter;
    int total_fes_mask_usage_iter;
    int fes_masks_not_presented;

    mdb_em_entry_encoding_e entry_encoding;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_FORMAT, format_id_lower, format_id_upper);

    SHR_ALLOC(ace_format_id_info, sizeof(dnx_field_ace_format_full_info_t), "ace_format_id_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    do_display = FALSE;

    /**
     * Initialize following variable, which will be calculated and parsed
     * to field_ace_format_efes_masks_print().
     */
    max_nof_formats_present = 0;
    total_fes_mask_usage_iter = 0;
    fes_masks_not_presented = 0;
    sal_strncpy_s(total_fes_mask_string, "--------", sizeof(total_fes_mask_string) - 1);
    sal_strncpy_s(not_present_fes_mask_string, "--------", sizeof(not_present_fes_mask_string) - 1);

    PRT_TITLE_SET("ACE Format List");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ACE Format ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "MDB Entry Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Mask Usage");

    for (format_id = format_id_lower; format_id <= format_id_upper; format_id++)
    {
        /**
         * Check if the specified ACE Format ID or range of ACE Format IDs are allocated.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_ace_id_is_allocated(unit, format_id, &is_ace_allocated));
        if (!is_ace_allocated)
        {
            continue;
        }
        /** Get info for the allocated ACE Formats. */
        SHR_IF_ERR_EXIT(dnx_field_ace_format_get(unit, format_id, ace_format_id_info));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("%d", format_id);
        /**
         * Check if ACE Format name is being provided, in case no set N/A as default value,
         * in other case set the provided name.
         */
        if (sal_strncmp
            (ace_format_id_info->format_basic_info.name, "", sizeof(ace_format_id_info->format_basic_info.name)))
        {
            PRT_CELL_SET("%s", ace_format_id_info->format_basic_info.name);
        }
        else
        {
            PRT_CELL_SET("N/A");
        }

        /** Calculates the "used" payload_size, by the user. */
        SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size
                        (unit, DNX_FIELD_STAGE_ACE,
                         ace_format_id_info->format_full_info.actions_payload_info.actions_on_payload_info,
                         &(payload_size_in_bits)));

        /** Get the smallest entry_encoding that can support the entry in the DBAL_PHYSICAL_TABLE_PPMC table. */
        SHR_IF_ERR_EXIT(mdb_em_get_min_entry_encoding
                        (unit, DBAL_PHYSICAL_TABLE_PPMC, DNX_DATA_MAX_FIELD_ACE_KEY_SIZE, payload_size_in_bits, 0,
                         DNX_DATA_MAX_FIELD_ACE_APP_DB_ID_SIZE, DNX_DATA_MAX_FIELD_ACE_ACE_ID_SIZE, &entry_encoding));

        /**
         * The value of MDB Entry size is calculated in following format:
         * Max ACE row size (DNX_DATA_MAX_FIELD_ACE_ROW_SIZE) divided by
         * returned value from entry_encoding enum (mdb_em_entry_encoding_e).
         */
        if (entry_encoding != MDB_EM_ENTRY_ENCODING_EMPTY)
        {
            PRT_CELL_SET("%d", (DNX_DATA_MAX_FIELD_ACE_ROW_SIZE / (1 << entry_encoding)));
        }
        else
        {
            PRT_CELL_SET("EMPTY");
        }
        /** Retrieve number of used EFES masks. */
        SHR_IF_ERR_EXIT(dnx_field_actions_ace_id_nof_mask_state_get(unit, nof_masks));

        /**
         * Initialize the iterator of used FES instructions and the string, which will present
         * present the used FES instructions. Their calculation is done in the loop below.
         */
        fes_mask_usage_iter = 0;
        sal_strncpy_s(fes_mask_string, "--------", sizeof(fes_mask_string) - 1);
        for (fes_inst_iter = 0; fes_inst_iter < ace_format_id_info->actions_fes_info.nof_fes_instr; fes_inst_iter++)
        {
            /**
             * If the mask for current FES ID is '0', then increase the not presented masks value
             * and continue to check the next FES.
             */
            if (nof_masks[ace_format_id_info->actions_fes_info.fes_instr[fes_inst_iter].fes_id] == 0)
            {
                fes_masks_not_presented++;
                continue;
            }

            /**
             * If mask is presented for the current FES id, then we should change the value of the
             * FES mask string to '|' (pipe), which will present that this FES instructions is being used.
             * Increase the number of used FES instructions.
             * Do the same and store the values in total variables, which are going to be parsed to
             * field_ace_format_efes_masks_print(), where they will be presented to the screen.
             */
            fes_mask_string[ace_format_id_info->actions_fes_info.fes_instr[fes_inst_iter].fes_id] = '|';
            total_fes_mask_string[ace_format_id_info->actions_fes_info.fes_instr[fes_inst_iter].fes_id] = '|';
            fes_mask_usage_iter++;
            total_fes_mask_usage_iter++;
        }
        PRT_CELL_SET("%d ( %s )", fes_mask_usage_iter, fes_mask_string);

        /** Calculate the value of all presented ACE Formats. */
        max_nof_formats_present++;

        do_display = TRUE;
    }

    if (do_display)
    {
        PRT_COMMITX;

        /** If any ACE Formats are being allocated, then print the used and not used FES instructions. */
        SHR_IF_ERR_EXIT(diag_dnx_field_ace_format_efes_masks_print(unit, max_nof_formats_present, total_fes_mask_string,
                                                                   total_fes_mask_usage_iter,
                                                                   not_present_fes_mask_string, fes_masks_not_presented,
                                                                   sand_control));
    }
    else
    {
        /*
         * If nothing was printed then just clear loaded PRT memory.
         * This could be left to 'exit' but it is clearer here.
         */
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO allocated ACE Formats were found on specified filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY,
                   EMPTY, EMPTY);
    }

exit:
    SHR_FREE(ace_format_id_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets cell information about a single ACE Format action, which is going to be presented on the screen.
 * \param [in] unit - The unit number.
 * \param [in] action_iter - The valid ACE action ID, to be presented.
 * \param [in] ace_format_info_p - Pointer, which contains the information about the ACE action,
 *  to be fill in the relevant cells and presented on the screen later.
 * \param [out] action_size_p - Will return the size of the current action (action_iter), to calculate
 *  and present the total size of all valid ACE actions, in the calling procedure.
 * \param [out] action_valid_bits_p - Will return the '1' in case the current action is using valid bit,
 *  used to calculate and present the total number of used valid bits by valid ACE actions, in the calling precedure.
 * \param [out] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_ace_format_action_print(
    int unit,
    int action_iter,
    dnx_field_ace_format_full_info_t * ace_format_info_p,
    int *action_size_p,
    int *action_valid_bits_p,
    prt_control_t * prt_ctr)
{
    unsigned int action_size;

    SHR_FUNC_INIT_VARS(unit);

    /** Get action size for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                    (unit, DNX_FIELD_STAGE_ACE, ace_format_info_p->format_basic_info.dnx_actions[action_iter],
                     &action_size));

    /** Print all needed info about current action, like action_type, size, lsb and valid bit. */
    PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit, ace_format_info_p->format_basic_info.dnx_actions[action_iter]));
    PRT_CELL_SET("%d", action_size);
    PRT_CELL_SET("%d",
                 ace_format_info_p->format_full_info.actions_payload_info.actions_on_payload_info[action_iter].lsb);
    PRT_CELL_SET("%d",
                 !(ace_format_info_p->format_full_info.actions_payload_info.
                   actions_on_payload_info[action_iter].dont_use_valid_bit));

    /**
     * Set values of action size and valid bit to the output parameters,
     * which will be parsed to the calling procedure and present there.
     */
    *action_size_p = action_size;
    if (!
        (ace_format_info_p->format_full_info.actions_payload_info.
         actions_on_payload_info[action_iter].dont_use_valid_bit))
    {
        *action_valid_bits_p = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function sets and prints general information about a given 'ace_id', like
 *  name, total payload size and number of entries.
 * \param [in] unit - The unit number.
 * \param [in] ace_id - Given allocated ACE Format ID, for which information will be presented.
 * \param [in] ace_format_info_p - Pointer, which contains the information about the ACE action's array,
 *  to be used for calculating the total payload size used by current ACE Format (ace_id).
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_ace_format_info_print(
    int unit,
    int ace_id,
    dnx_field_ace_format_full_info_t * ace_format_info_p,
    sh_sand_control_t * sand_control)
{
    unsigned int payload_size_in_bits;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Calculates the "used" payload_size, by the user. */
    SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size
                    (unit, DNX_FIELD_STAGE_ACE,
                     ace_format_info_p->format_full_info.actions_payload_info.actions_on_payload_info,
                     &(payload_size_in_bits)));

    PRT_TITLE_SET("ACE Format Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ACE Format ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Total Payload Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Number of Entries");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%d", ace_id);
    /**
     * Check if ACE Format name is being provided, in case no set N/A as default value,
     * in other case set the provided name.
     */
    if (sal_strncmp(ace_format_info_p->format_basic_info.name, "", sizeof(ace_format_info_p->format_basic_info.name)))
    {
        PRT_CELL_SET("%s", ace_format_info_p->format_basic_info.name);
    }
    else
    {
        PRT_CELL_SET("N/A");
    }
    PRT_CELL_SET("%d", payload_size_in_bits);
    PRT_CELL_SET("-");

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function presents information about all valid ACE Format actions,
 *   which are going to be presented on the screen.
 * \param [in] unit - The unit number.
 * \param [in] ace_format_info_p - Pointer, which contains the information about the ACE action' array,
 *  to be parsed to the internal function (field_ace_format_action_print),
 *  which will present the actions in a appropriate way.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_ace_format_info_actions_print(
    int unit,
    dnx_field_ace_format_full_info_t * ace_format_info_p,
    sh_sand_control_t * sand_control)
{
    int total_action_size, total_action_valid_bits;
    int action_size, action_valid_bits;
    int action_iter;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Used to store the returned action size and valid bits by field_ace_format_action_print(). */
    action_size = 0;
    action_valid_bits = 0;
    /** Used to store the sum of returned action sizes and valid bits by field_ace_format_action_print(). */
    total_action_size = 0;
    total_action_valid_bits = 0;

    PRT_TITLE_SET("ACE Format Actions Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Lsb");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Valid Bit");

    /** Iterate over maximum number of actions per FG, to retrieve information about these, which are valid. */
    for (action_iter = 0; action_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG; action_iter++)
    {
        /**
         * In case current action is not an invalid action, then all the internal procedure field_ace_format_action_print(),
         * which will set cells information about it. Calculate the total action size and valid bits,
         * returned by field_ace_format_action_print(), to be used in case we reach an INVALID action.
         */
        if (ace_format_info_p->format_basic_info.dnx_actions[action_iter] != DNX_FIELD_ACTION_INVALID)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            /** Set cell information about a valid ACE Format action. */
            SHR_IF_ERR_EXIT(diag_dnx_field_ace_format_action_print
                            (unit, action_iter, ace_format_info_p, &action_size, &action_valid_bits, prt_ctr));

            /** Store the returned values of actions size and valid bit. */
            total_action_size += action_size;
            total_action_valid_bits += action_valid_bits;
        }
        else
        {
            /**
             * If we reach the last element of action array we should print a line, which presents,
             * the total sum of all presented ACE action sizes and valid bits.
             */
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            PRT_CELL_SET("%s", "Total");
            PRT_CELL_SET("%d", total_action_size);
            PRT_CELL_SET("%s", "-");
            PRT_CELL_SET("%d", total_action_valid_bits);

            break;
        }

    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function presents information about ACE Format EFES configuration, like
 *   action_type, efes mask ID and the relevant efes mask, which are going to be presented on the screen.
 * \param [in] unit - The unit number.
 * \param [in] ace_format_info_p - Pointer, which contains the information about FES instructions,
 *  used by the current allocated ACE Format.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_ace_format_info_efes_config_print(
    int unit,
    dnx_field_ace_format_full_info_t * ace_format_info_p,
    sh_sand_control_t * sand_control)
{
    int fes_inst_iter;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("EFES Configuration");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Mask ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "EFES Mask");

    /**
     * Iterate over the number of valid FES instructions used by the ACE ID,
     * and use it as number of valid elements in the array fes_instr.
     */
    for (fes_inst_iter = 0; fes_inst_iter < ace_format_info_p->actions_fes_info.nof_fes_instr; fes_inst_iter++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("%d", ace_format_info_p->actions_fes_info.fes_instr[fes_inst_iter].fes_id);
        PRT_CELL_SET("%s",
                     dnx_field_dnx_action_text(unit,
                                               DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_ACE,
                                                          ace_format_info_p->actions_fes_info.
                                                          fes_instr[fes_inst_iter].action_type)));
        PRT_CELL_SET("%d", ace_format_info_p->actions_fes_info.fes_instr[fes_inst_iter].chosen_mask);
        PRT_CELL_SET("0x%X", ace_format_info_p->actions_fes_info.fes_instr[fes_inst_iter].mask);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This functions displays info about a specific ACE Format, its actions and EFES configuration
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
diag_dnx_field_ace_format_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_ace_allocated;
    dnx_field_ace_id_t ace_format_id;
    dnx_field_ace_format_full_info_t *ace_format_id_info = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_FORMAT, ace_format_id);

    /** Use DMA allocation for ace_format_id_info structure, to avoid large stack usage issues. */
    SHR_ALLOC(ace_format_id_info, sizeof(dnx_field_ace_format_full_info_t), "ace_format_id_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    /**
     * Check if the specified ACE ID is allocated.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_ace_id_is_allocated(unit, ace_format_id, &is_ace_allocated));
    if (!is_ace_allocated)
    {
        LOG_CLI_EX("\r\n" "ACE Format ID %d is not allocated!!%s%s%s\r\n\n", ace_format_id, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_field_ace_format_get(unit, ace_format_id, ace_format_id_info));

    /**
     * Set and print general information about a given 'ace_format_id'.
     * Like name, total payload size and number of entries.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_ace_format_info_print(unit, ace_format_id, ace_format_id_info, sand_control));

    /** Present information about all valid ACE Format actions. */
    SHR_IF_ERR_EXIT(diag_dnx_field_ace_format_info_actions_print(unit, ace_format_id_info, sand_control));

    /**
     * Present information about ACE Format EFES configuration, like
     * action_type, efes mask ID and the relevant efes mask.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_ace_format_info_efes_config_print(unit, ace_format_id_info, sand_control));

exit:
    SHR_FREE(ace_format_id_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function presents information about ACE Entry payload, like action_type, size, lsb, value and valid bits.
 * \param [in] unit - The unit number.
 * \param [in] ace_format_id - ACE Format ID, on which the ACE Entry is being added..
 * \param [in] entry_handle - ACE Entry, which info will be presented.
 * \param [in] ace_format_id_info_p - Pointer, which contains the information about the ACE Format action array,
 *  to be presented.
 * \param [in] entry_payload_p - Pointer, which contains the information about the ACE Entry payload like,
 *  action types, action values. Used to be compared with ACE format action array to see, which actions
 *  are valid for the given entry_handle.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_ace_entry_info_print(
    int unit,
    dnx_field_ace_id_t ace_format_id,
    dnx_field_ace_key_t entry_handle,
    dnx_field_ace_format_full_info_t * ace_format_id_info_p,
    dnx_field_entry_payload_t * entry_payload_p,
    sh_sand_control_t * sand_control)
{
    int action_iter, ent_action_iter;
    int act_val_iter;
    int do_cell_set;
    char action_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    unsigned int action_size;
    char action_name[DBAL_MAX_STRING_LENGTH];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("ACE Format %d |  ACE Entry 0x%X", ace_format_id, entry_handle);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Lsb");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Valid bit");

    for (action_iter = 0; action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         ace_format_id_info_p->format_basic_info.dnx_actions[action_iter] != DNX_FIELD_ACTION_INVALID; action_iter++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        /** Get action size and name for the current action. */
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                        (unit, DNX_FIELD_STAGE_ACE, ace_format_id_info_p->format_basic_info.dnx_actions[action_iter],
                         &action_size));
        sal_strncpy_s(action_name,
                      dnx_field_dnx_action_text(unit, ace_format_id_info_p->format_basic_info.dnx_actions[action_iter]),
                      sizeof(action_name));

        PRT_CELL_SET("%s", action_name);
        PRT_CELL_SET("%d", action_size);
        PRT_CELL_SET("%d",
                     ace_format_id_info_p->format_full_info.actions_payload_info.
                     actions_on_payload_info[action_iter].lsb);

        do_cell_set = FALSE;

        /** Iterate over entry actions array and check, which actions are valid. */
        for (ent_action_iter = 0; ent_action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP;
             ent_action_iter++)
        {
            /**
             * In case we have no match between current ACE Format action and any of ACE Entry actions,
             * this means that the action is not valid for the entry,
             * we should print '-' (dash) after the for loop.
             */
            if (entry_payload_p->action_info[ent_action_iter].dnx_action == DNX_FIELD_ACTION_INVALID)
            {
                break;
            }
            else if (ace_format_id_info_p->format_full_info.actions_payload_info.
                     actions_on_payload_info[action_iter].dnx_action ==
                     entry_payload_p->action_info[ent_action_iter].dnx_action)
            {
                /**
                 * In case we have match between current ACE Format action and any of ACE Entry actions,
                 * this means that the action is valid for the entry. Set the do_cell_set to be TRUE and
                 * prepare the action value for printing after the for loop.
                 */
                do_cell_set = TRUE;
                break;
            }
        }

        if (do_cell_set)
        {
            /** Iterate over the action value array and get the value of the valid entry action, to be presented. */
            sal_strncpy(action_val_string, "0x", sizeof(action_val_string) - 1);
            for (act_val_iter = DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY - 1; act_val_iter >= 0;
                 act_val_iter--)
            {
                char act_val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                      "%X",
                                                      entry_payload_p->
                                                      action_info[ent_action_iter].action_value[act_val_iter]);
                sal_snprintf(act_val_buff, sizeof(act_val_buff), "%X",
                             entry_payload_p->action_info[ent_action_iter].action_value[act_val_iter]);
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(action_val_string, "%s", act_val_buff);
                sal_strncat_s(action_val_string, act_val_buff, sizeof(action_val_string));
            }
            PRT_CELL_SET("%s", action_val_string);
            PRT_CELL_SET("%d",
                         !(ace_format_id_info_p->format_full_info.actions_payload_info.
                           actions_on_payload_info[action_iter].dont_use_valid_bit));
        }
        else
        {
            PRT_CELL_SET("%s", "-");
            PRT_CELL_SET("%s", "-");
        }

    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays info about a specific ACE Entry, of a specific ACE Format, and its action info.
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
diag_dnx_field_ace_entry_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_ace_key_t entry_handle;
    dnx_field_entry_payload_t entry_payload;

    uint8 is_ace_allocated;
    uint32 input_ace_format_id;
    dnx_field_ace_id_t ace_format_id;
    dnx_field_ace_format_full_info_t *ace_format_id_info = NULL;
    dnx_field_ace_id_t ace_format_id_retrieved;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_FORMAT, input_ace_format_id);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_ENTRY, entry_handle);

    SHR_ALLOC(ace_format_id_info, sizeof(dnx_field_ace_format_full_info_t), "ace_format_id_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_entry_payload_t_init(unit, &entry_payload));
    SHR_SET_CURRENT_ERR(dnx_field_ace_entry_get(unit, entry_handle, &entry_payload, &ace_format_id_retrieved));
    if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
    {
        LOG_CLI_EX("\r\n" "The given entry %d (0x%08X) is invalid.%s%s\r\n", entry_handle, entry_handle, EMPTY, EMPTY);
        SHR_EXIT();
    }
    else if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
    {
        LOG_CLI_EX("\r\n" "An error occurred while retrieving entry %d (0x%08X).%s%s\r\n",
                   entry_handle, entry_handle, EMPTY, EMPTY);
        SHR_EXIT();
    }

    if (input_ace_format_id == -1)
    {
        ace_format_id = ace_format_id_retrieved;
        /** Check if the returned ACE ID is allocated. Sanity check. */
        SHR_IF_ERR_EXIT(dnx_algo_field_ace_id_is_allocated(unit, ace_format_id, &is_ace_allocated));
        if (!is_ace_allocated)
        {
            LOG_CLI_EX("\r\n" "ACE Entry 0x%X belongs to an ACE Format ID %d that doesn't exist!!%s%s\r\n\n",
                       entry_handle, ace_format_id, EMPTY, EMPTY);
            SHR_EXIT();
        }
    }
    else
    {
        ace_format_id = (dnx_field_ace_id_t) input_ace_format_id;
        /** Check if the specified ACE ID is allocated. */
        SHR_IF_ERR_EXIT(dnx_algo_field_ace_id_is_allocated(unit, ace_format_id, &is_ace_allocated));
        if (!is_ace_allocated)
        {
            LOG_CLI_EX("\r\n" "ACE Format ID %d is not allocated!!%s%s%s\r\n\n", ace_format_id, EMPTY, EMPTY, EMPTY);
            SHR_EXIT();
        }
        if (ace_format_id_retrieved != ace_format_id)
        {
            LOG_CLI_EX("\r\n" "ACE entry 0x%X is a member of ACE Format ID %d and not %d!!%s\r\n\n",
                       entry_handle, ace_format_id_retrieved, ace_format_id, EMPTY);
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT(dnx_field_ace_format_get(unit, ace_format_id, ace_format_id_info));

    /** Present information about ACE Entry payload, like action_type, size, lsb, value and valid bits. */
    SHR_IF_ERR_EXIT(diag_dnx_field_ace_entry_info_print
                    (unit, ace_format_id, entry_handle, ace_format_id_info, &entry_payload, sand_control));

exit:
    SHR_FREE(ace_format_id_info);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */

/**
 *  'Help' description for ACE Format List (shell commands).
 */
static sh_sand_man_t Field_ace_format_list_man = {
    .brief = "'ACE Format list'- displays a list of allocated ACE Formats by given 'format' or range of 'formats "
             "(filtered by ACE Format id)",
    .full = "'ACE Format list' displays a list of allocated ACE Formats by given 'format' or range of 'formats. Range is specified via 'format'.\r\n"
            "The diagnostic displays also, info for used EFESs and size of the MDB Entry.\r\n"
            "If no 'format' is specified then '1'-'nof_ace_id-1' is assumed (1-63).\r\n"
            "If only one value is specified for 'format' then this 'single value' range is assumed.\r\n",
    .synopsis = "[format=<lower value>-<upper value>]",
    .examples = "format=50",
    .init_cb = diag_dnx_field_ace_init,
};
static sh_sand_option_t Field_ace_format_list_options[] = {
    {DIAG_DNX_FIELD_OPTION_FORMAT,  SAL_FIELD_TYPE_UINT32, "Lowest-highest ACE Format ID to get its info",       "MIN-MAX",       (void *)Field_ace_format_id_enum_table, "MIN-MAX"},
    {NULL}
};

/**
 *  'Help' description for ACE Format Info (shell commands).
 */
static sh_sand_man_t Field_ace_format_info_man = {
    .brief = "'ACE Format info'- displays info for a specific ACE Format, filtered by ACE Format id specified by the user.",
    .full = "'ACE Format info' displays info about a specific ACE Format, its actions and EFES configuration.\r\n"
            "If no 'format' (1-63) is specified then an error will occur.\r\n",
    .synopsis = "[format=<Should be between 1-63>]",
    .examples = "format=50",
    .init_cb = diag_dnx_field_ace_init,
};
static sh_sand_option_t Field_ace_format_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_FORMAT,          SAL_FIELD_TYPE_UINT32, "ACE Format ID to get its info",           NULL   ,(void *)Field_ace_format_id_enum_table, "MIN-MAX"},
    {NULL}
};

/**
 *  'Help' description for ACE Entry Info (shell commands).
 */
static sh_sand_man_t Field_ace_entry_info_man = {
    .brief = "'ACE Entry info'- displays info for a specific ACE Entry, filtered by ACE Format id  and ACE Entry id specified by the user.",
    .full = "'ACE Entry info' displays info about a specific ACE Entry and its action info.\r\n"
            "Option 'format' (1-63) is optional.\r\n"
            "If no 'entry' (0-0x3fffff) is specified then an error will occur.\r\n"
            "For example 'format=1 entry=0'.",
    .synopsis = "[format=<Should be between 1-63>]"
                "[entry=<Should be between 0-0x3fffff>]",
    .examples = "format=50 entry=0x3C0100",
    .init_cb = diag_dnx_field_ace_init,
};
static sh_sand_option_t Field_ace_entry_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_FORMAT,          SAL_FIELD_TYPE_UINT32, "ACE Format ID to get its entry info",        NULL   ,(void *)Field_ace_format_id_enum_table, "MIN-MAX"},
    {DIAG_DNX_FIELD_OPTION_ENTRY,           SAL_FIELD_TYPE_UINT32, "ACE Entry ID to get its info",                        NULL   ,(void *)Field_ace_entry_id_enum_table, "MIN-MAX"},
    {NULL}
};

/**
 *  'Help' description for ACE Format (shell commands).
 */
static sh_sand_man_t Field_ace_format_man = {
    .brief = "'ACE Format'- displays info for a specific ACE Format.",
    .full = "'ACE Format' displays info about a specific ACE Format, its actions and EFES configuration.\r\n",
    .synopsis = NULL,
    .examples = NULL,
};
/**
 *  'Help' description for ACE Entry (shell commands).
 */
static sh_sand_man_t Field_ace_entry_man = {
    .brief = "'ACE Entry'- displays info for a specific ACE Entry.",
    .full = "'ACE Entry ' displays info about a specific ACE Entry and its action info.\r\n",
    .synopsis = NULL,
    .examples = NULL,
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
 *   List of shell options for 'ace format' shell commands (list, info)
 */
sh_sand_cmd_t Sh_dnx_field_ace_format_cmds[] = {
    {"list", diag_dnx_field_ace_format_list_cb, NULL, Field_ace_format_list_options, &Field_ace_format_list_man}
    ,
    {"info", diag_dnx_field_ace_format_info_cb, NULL, Field_ace_format_info_options, &Field_ace_format_info_man}
    ,
    {NULL}
};

/**
 * \brief
 *   List of shell options for 'ace entry' shell commands (info)
 */
sh_sand_cmd_t Sh_dnx_field_ace_entry_cmds[] = {
    {"info", diag_dnx_field_ace_entry_info_cb, NULL, Field_ace_entry_info_options, &Field_ace_entry_info_man, NULL,
     NULL}
    ,
    {NULL}
};

/**
 * \brief
 *   List of shell options for 'ace' shell commands (list, info)
 */
sh_sand_cmd_t Sh_dnx_field_ace_cmds[] = {
    {"format", NULL, Sh_dnx_field_ace_format_cmds, NULL, &Field_ace_format_man}
    ,
    {"entry", NULL, Sh_dnx_field_ace_entry_cmds, NULL, &Field_ace_entry_man}
    ,
    {NULL}
};
/*
 * }
 */
