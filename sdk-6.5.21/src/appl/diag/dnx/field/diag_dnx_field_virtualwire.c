/** \file diag_dnx_field_virtualwire.c
 *
 * Diagnostics procedures, for DNX, for 'virtualwire' operations.
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
#include <sal/appl/sal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_context.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <include/soc/dnx/pemladrv/pemladrv_virtual_wires.h>
#include "diag_dnx_field_virtualwire.h"
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

/**
 *  'Help' description for VirtualWire Info (shell commands).
 */
static sh_sand_man_t Field_virtualwire_info_man = {
    .brief = "'VirtualWire Info'- displays VWs related info for a certain stage (filter by field stage).",
    .full = "'VirtualWire Info' displays VWs related info for a certain stage.\r\n"
            "If no stage 'STAGE_LOWEST'-'STAGE_HIGHEST' is specified then all stages will be printed.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF3 | EXTERNAL>]",
    .examples = "stage=IPMF1",
};
static sh_sand_option_t Field_virtualwire_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Field stage to show virtualwires info for",   "STAGE_LOWEST-STAGE_HIGHEST",    (void *)Field_virtualwire_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {NULL}
};

/**
 *  'Help' description for VirtualWire Signals (shell commands).
 */
static sh_sand_man_t Field_virtualwire_signals_man = {
    .brief = "'VirtualWire Signals'- displays signals used by VWs for a certain stage (filter by field stage).",
    .full = "'VirtualWire Signals' displays signals used by VWs for a certain stage.\r\n"
            "If no stage 'STAGE_LOWEST'-'STAGE_HIGHEST' is specified then all stages will be printed.\r\n"
            "Can be filtered by signal name or substring of the name.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF2 | IPMF3 | EXTERNAL>] [signal=<signal name>]\n",
    .examples = "stage=IPMF1 signal=statistics",
};
static sh_sand_option_t Field_virtualwire_signals_options[] = {
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Field stage to show virtualwires signals for",   "STAGE_LOWEST-STAGE_HIGHEST",    (void *)Field_virtualwire_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {DIAG_DNX_FIELD_OPTION_SIGNAL,  SAL_FIELD_TYPE_STR,  "Filter signals by comparing this substring to the signal name", ""},
    {NULL}
};

/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function displays signals, qualifiers and actions allocate for the VWs for
 *   a certain stage, specified by the caller.
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
diag_dnx_field_virtualwire_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;
    int do_display;
    int support_qual;
    uint8 stage_for_qual;
    int vw_iterator;
    VirtualWireInfo *vw_info_p;
    int mapping_index;
    dnx_pp_stage_e pp_stages[1];
    int pemla_stage;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);

    do_display = FALSE;

    PRT_TITLE_SET("Virtual Wire Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "VWire");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Signals [bits on signal]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Actions (id) [bits on vw]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qualifiers (id)");

    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        dnx_field_stage_e field_stage;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                        (unit, Field_virtualwire_diag_stage_enum_table[stage_index].string, &field_stage));

        vw_iterator = PEMLADRV_ITERATOR_BY_STAGE_INIT;
        if (field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            pp_stages[0] = dnx_field_map_stage_info[DNX_FIELD_STAGE_IPMF1].pp_stage;
        }
        else
        {
            pp_stages[0] = dnx_field_map_stage_info[field_stage].pp_stage;
        }

        while (1)
        {
            char signals_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            char actions_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            char qualifiers_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            int nof_ids = 0;
            uint32 ids[BCM_FIELD_NAME_TO_ID_MAX_IDS];

            SHR_IF_ERR_EXIT(pemladrv_vw_wire_iterator_by_stage_get_next
                            (unit, 1, pp_stages, &vw_iterator, &vw_info_p, &stage_for_qual));
            if (vw_iterator == PEMLADRV_ITERATOR_BY_STAGE_END)
            {
                break;
            }

            SHR_IF_ERR_EXIT(pemladrv_vw_stages_sdk_to_pemla(unit, pp_stages[0], &pemla_stage));

            support_qual = TRUE;

            if (vw_info_p->width_in_bits > dnx_data_field.qual.max_bits_in_qual_get(unit))
            {
                support_qual = FALSE;
            }

            for (mapping_index = 0; mapping_index < vw_info_p->nof_mappings; mapping_index++)
            {
                char signals_str_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
                int bits_on_signal_lsb = vw_info_p->vw_mappings_arr[mapping_index].physical_wire_lsb;
                int bits_on_signal_msb = (bits_on_signal_lsb + (vw_info_p->width_in_bits - 1));

                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%s [%d:%d]\n",
                                                      vw_info_p->vw_mappings_arr[mapping_index].physical_wire_name,
                                                      bits_on_signal_lsb, bits_on_signal_msb);
                sal_snprintf(signals_str_buff, sizeof(signals_str_buff), "%s [%d:%d]\n",
                             vw_info_p->vw_mappings_arr[mapping_index].physical_wire_name, bits_on_signal_lsb,
                             bits_on_signal_msb);
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(signals_string, "%s", signals_str_buff);
                sal_strncat_s(signals_string, signals_str_buff, sizeof(signals_string));
            }

            SHR_IF_ERR_EXIT(dnx_field_map_name_to_action(unit, vw_info_p->virtual_wire_name, &nof_ids, ids));

            if (nof_ids <= 0 || field_stage == DNX_FIELD_STAGE_EXTERNAL)
            {
                sal_strncpy_s(actions_string, "-", sizeof(actions_string));
            }
            else
            {
                int action_id;
                int bits_on_signal_lsb = 0;
                int bits_on_signal_msb;
                for (action_id = 0; action_id < nof_ids; action_id++)
                {
                    char actions_str_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
                    dnx_field_action_t dnx_action;
                    unsigned int action_size;
                    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit, field_stage, ids[action_id], &dnx_action));
                    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, field_stage, dnx_action, &action_size));
                    bits_on_signal_msb = (bits_on_signal_lsb + (action_size - 1));
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%s (%d) [%d:%d]\n",
                                                          dnx_field_dnx_action_text(unit, dnx_action), ids[action_id],
                                                          bits_on_signal_lsb, bits_on_signal_msb);
                    sal_snprintf(actions_str_buff, sizeof(actions_str_buff), "%s (%d) [%d:%d]\n",
                                 dnx_field_dnx_action_text(unit, dnx_action), ids[action_id], bits_on_signal_lsb,
                                 bits_on_signal_msb);
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(actions_string, "%s", actions_str_buff);
                    sal_strncat_s(actions_string, actions_str_buff, sizeof(actions_string));
                    bits_on_signal_lsb += action_size;
                }
            }

            if (support_qual)
            {
                bcm_field_qualify_t bcm_qual;
                SHR_IF_ERR_EXIT(dnx_field_map_name_to_qual(unit, vw_info_p->virtual_wire_name, &bcm_qual));

                if (bcm_qual != bcmFieldQualifyCount)
                {
                    dnx_field_qual_t dnx_qual;
                    if ((dnx_vw_qual_info[(bcm_qual - dnx_data_field.qual.vw_1st_get(unit))].stage_bmp &
                         (SAL_BIT(field_stage))) == 0)
                    {
                        sal_strncpy_s(qualifiers_string, "-", sizeof(qualifiers_string));
                        break;
                    }
                    SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, field_stage, bcm_qual, &dnx_qual));
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%s (%d)", dnx_field_dnx_qual_text(unit, dnx_qual),
                                                          bcm_qual);
                    sal_snprintf(qualifiers_string, sizeof(qualifiers_string), "%s (%d)",
                                 dnx_field_dnx_qual_text(unit, dnx_qual), bcm_qual);
                }
                else
                {
                    sal_strncpy_s(qualifiers_string, "-", sizeof(qualifiers_string));
                }
            }
            else
            {
                sal_strncpy_s(qualifiers_string, "-", sizeof(qualifiers_string));
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            /** Set 'VWire' cell. */
            PRT_CELL_SET("%s", vw_info_p->virtual_wire_name);
            /** Set 'Stage' cell. */
            if (vw_info_p->start_stage == pemla_stage)
            {
                PRT_CELL_SET("%s (first)", dnx_field_stage_text(unit, field_stage));
            }
            else if (vw_info_p->end_stage == pemla_stage)
            {
                PRT_CELL_SET("%s (last)", dnx_field_stage_text(unit, field_stage));
            }
            else
            {
                PRT_CELL_SET("%s", dnx_field_stage_text(unit, field_stage));
            }
            /** Set 'Signals [bits on signal]' cell. */
            PRT_CELL_SET("%s", signals_string);
            /** Set 'Actions (id) [bits on vw]' cell. */
            PRT_CELL_SET("%s", actions_string);
            /** Set 'Qualifiers (id)' cell. */
            PRT_CELL_SET("%s", qualifiers_string);

            do_display = TRUE;
        }
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
        LOG_CLI_EX("\r\n" "No info was found for the given filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays information about signals used by VWs for
 *   a certain stage, specified by the caller.
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
diag_dnx_field_virtualwire_signals_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;
    char *signal_match_n;
    int do_display;
    int signals_nof = dnx_data_field.virtual_wire.signals_nof_get(unit);
    int signal_index;
    const dnx_data_field_virtual_wire_signal_mapping_t *dnx_data_signals_struct_p;
    uint8 is_qual_available;
    int action_index;
    int max_nof_actions = dnx_data_field.virtual_wire.actions_per_signal_nof_get(unit);
    uint8 action_found;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);
    SH_SAND_GET_STR(DIAG_DNX_FIELD_OPTION_SIGNAL, signal_match_n);

    do_display = FALSE;

    PRT_TITLE_SET("Virtual Wire Signals");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Signal");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Can be used by qualifier");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bits covered by actions");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");

    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        dnx_field_stage_e field_stage;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                        (unit, Field_virtualwire_diag_stage_enum_table[stage_index].string, &field_stage));

        for (signal_index = 0; signal_index < signals_nof; signal_index++)
        {
            char bits_covered_by_act_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

            is_qual_available = FALSE;
            action_found = FALSE;

            if (field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                dnx_data_signals_struct_p =
                    dnx_data_field.virtual_wire.signal_mapping_get(unit, DNX_FIELD_STAGE_IPMF1, signal_index);
            }
            else
            {
                dnx_data_signals_struct_p =
                    dnx_data_field.virtual_wire.signal_mapping_get(unit, field_stage, signal_index);
            }
            /** The empty signal slots per stage will not be present. */
            if (0 == sal_strncmp(dnx_data_signals_struct_p->signal_name, "EMPTY", MAX_VW_NAME_LENGTH))
            {
                continue;
            }

            /** Check if the signal is maching on the given user string. */
            if (!ISEMPTY(signal_match_n) && !sal_strcasestr(dnx_data_signals_struct_p->signal_name, signal_match_n))
            {
                continue;
            }

            if (dnx_data_signals_struct_p->mapped_qual != DNX_FIELD_QUAL_ID_INVALID)
            {
                is_qual_available = TRUE;
            }

            for (action_index = 0; action_index < max_nof_actions || field_stage == DNX_FIELD_STAGE_EXTERNAL;
                 action_index++)
            {
                int bits_by_act_lsb;
                int bits_by_act_msb;
                char bits_by_act_str_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
                unsigned int action_size;
                dnx_field_action_t dnx_action;

                if (dnx_data_signals_struct_p->mapped_action[action_index] <= 0)
                {
                    break;
                }

                dnx_action =
                    DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage,
                               dnx_data_signals_struct_p->mapped_action[action_index]);
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, field_stage, dnx_action, &action_size));

                bits_by_act_lsb = dnx_data_signals_struct_p->action_offset[action_index];
                bits_by_act_msb = (bits_by_act_lsb + (action_size - 1));

                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "[%d:%d],", bits_by_act_lsb, bits_by_act_msb);
                sal_snprintf(bits_by_act_str_buff, sizeof(bits_by_act_str_buff), "[%d:%d],",
                             bits_by_act_lsb, bits_by_act_msb);
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(bits_covered_by_act_string, "%s", bits_by_act_str_buff);
                sal_strncat_s(bits_covered_by_act_string, bits_by_act_str_buff, sizeof(bits_covered_by_act_string));
                action_found = TRUE;
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            /** Set 'Signal' cell. */
            PRT_CELL_SET("%s", dnx_data_signals_struct_p->signal_name);
            if (is_qual_available)
            {
                /** Set 'Can be used by qualifier' cell. */
                PRT_CELL_SET("%s", "Yes");
            }
            else
            {
                /** Set 'Can be used by qualifier' cell. */
                PRT_CELL_SET("%s", "No");
            }
            if (action_found)
            {
                /**
                 * Extract last symbol of the constructed string
                 * to not present an additional comma "," at the end of it.
                 */
                bits_covered_by_act_string[sal_strlen(bits_covered_by_act_string) - 1] = '\0';
                /** Set 'Bits covered by actions' cell. */
                PRT_CELL_SET("%s", bits_covered_by_act_string);
            }
            else
            {
                /** Set 'Bits covered by actions' cell. */
                PRT_CELL_SET("%s", "-");
            }
            /** Set 'Stage' cell. */
            PRT_CELL_SET("%s", dnx_field_stage_text(unit, field_stage));

            do_display = TRUE;
        }
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
        LOG_CLI_EX("\r\n" "No info was found for the given filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'virtualwire' shell commands (info)
 */
sh_sand_cmd_t Sh_dnx_field_virtualwire_cmds[] = {
    {"info", diag_dnx_field_virtualwire_info_cb, NULL, Field_virtualwire_info_options, &Field_virtualwire_info_man}
    ,
    {"signals", diag_dnx_field_virtualwire_signals_cb, NULL, Field_virtualwire_signals_options,
     &Field_virtualwire_signals_man}
    ,
    {NULL}
};

/*
 * }
 */
