/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file diag_dnx_pp_fdt.c
 *
 * Forward Decision Trace PP Diagnostics.
 */

/*************
 * INCLUDES  *
 */
#include "diag_dnx_pp.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>
#include <bcm_int/dnx/instru/instru_visibility.h>

/*************
 * DEFINES   *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

#define MAX_NOF_OUTLIFS (4)
#define MAX_NOF_SECONDARY_OUTLIFS (3)

/*************
 *  MACROS  *
 */

/*************
 * TYPEDEFS  *
 */

/*************
 * GLOBALS   *
 */

/*************
 * FUNCTIONS *
 */

/**
 * \brief - Returns the int value of the first row of a string
  *
 * \par DIRECT_INPUT:
 *   \param [in] str - input string
 *
 * \par DIRECT OUTPUT:
 *   int
 */
static int
sh_dnx_pp_first_row_to_int(
    char *str)
{
    uint32 value = 0;
    int index = 0;
    int rc;

    char first_row[DSIG_MAX_SIZE_STR] = "";

    /** copy first row the the string until enter or end of string */
    while ((*str != '\0') && (*str != '\n'))
    {
        first_row[index] = *str;
        first_row[index + 1] = '\0';

        index++;
        str++;
    }

    /** convert the received subfield value to numeric */
    rc = utilex_str_xstoul(first_row, &value);

    if (rc != 0)
    {
        return -1;
    }

    return value;

}

/**
 * \brief - Provides the trace (changes) of the forwarding decision
 *          for the last packet in the following stages in the Ingress-PP
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism
 *          definition
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
sh_dnx_pp_fdt_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core_in, core_id;

    /** assume default value be FALSE */
    uint8 short_mode = FALSE;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core_in);

    SH_SAND_GET_BOOL("SHORT", short_mode);

    /*
     * Print all signals of the PP PKTTM command for each core
     */

    SH_DNX_CORES_ITER(unit, core_in, core_id)
    {
        char *stage_list_prt[] =
            { "LLR", "VTT1", "VTT2", "VTT3", "VTT4", "VTT5", "FWD1", "FWD2", "IPMF1", "FER", "IPMF3", "LBP" };

        int stage_idx;

        /** this lif cmd will change through the stages */
        char out_lif_sig_name[RHNAME_MAX_SIZE] = "out_lifs";
        char fwd_act_dst_sig_name[RHNAME_MAX_SIZE] = "fwd_action_dst";
        char trap_strength_sig_name[RHNAME_MAX_SIZE] = "fwd_action_strength";

        int num_of_stages = sizeof(stage_list_prt) / sizeof(stage_list_prt[0]);

        /** Print message and continue to next core if not ready */
        DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_IRPP, !short_mode);

        /*
         * Print table titles
         */
        PRT_TITLE_SET("PP_VIS_FDT_Core=%d Results", core_id);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", "FWD_Action_Dest");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", "OutLIFs");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", "EEI");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", "Trap_Code");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", "Trap_Strength");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", "Trap_Qualifier");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        /** if fwd_action_* signals from LBP are not supported, lower number of stages to not include LBP */
        if (!(dnx_data_debug.feature.feature_get(unit, dnx_data_debug_feature_fwd_action_signals_from_lbp_valid)))
        {
            num_of_stages--;
        }

          /** traverse all stages */
        for (stage_idx = 0; stage_idx < num_of_stages; stage_idx++)
        {
            char sig_value_str[DSIG_MAX_SIZE_STR];

            PRT_CELL_SET("%s",
                         ((sal_strcasecmp(stage_list_prt[stage_idx], "IPMF1") ==
                           0) ? "IPMF1-2" : stage_list_prt[stage_idx]));

            /** once we reach LBP stage, the 'fwd_action_dst' command changes to 'tm_cmd.fwd_action_dst' */
            if (sal_strstr(stage_list_prt[stage_idx], "LBP"))
            {
                sal_strncpy(fwd_act_dst_sig_name, "tm_cmd.fwd_action_dst", RHNAME_MAX_SIZE - 1);
            }

            {
                char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { fwd_act_dst_sig_name, NULL };
                SHR_IF_ERR_EXIT(get_sig_hierarchy_str
                                (unit, core_id, "", stage_list_prt[stage_idx], "", sig_name_hieararchy, "", 1,
                                 sig_value_str, FALSE, FALSE, FALSE, 0));

                if (sal_strstr(stage_list_prt[stage_idx], "LLR"))
                {
                    sal_strncpy(sig_value_str, "N/A", RHNAME_MAX_SIZE - 1);
                }

                PRT_CELL_SET("%s", sig_value_str);
            }

              /** once we reach IPMF stage, the 'out_lifs' command changes to 'all_out_lifs' */
            if (sal_strstr(stage_list_prt[stage_idx], "IPMF"))
            {
                sal_strncpy(out_lif_sig_name, "all_out_lifs", RHNAME_MAX_SIZE - 1);
            }

            {
                char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { out_lif_sig_name, NULL };
                SHR_IF_ERR_EXIT(get_sig_hierarchy_str
                                (unit, core_id, "", stage_list_prt[stage_idx], "", sig_name_hieararchy, "", 1,
                                 sig_value_str, FALSE, FALSE, TRUE, 0));

                if ((sal_strstr(stage_list_prt[stage_idx], "LLR")) || (sal_strstr(stage_list_prt[stage_idx], "LBP")))
                {
                    sal_strncpy(sig_value_str, "N/A", RHNAME_MAX_SIZE - 1);
                }

                PRT_CELL_SET("%s", sig_value_str);
            }
            {
                char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { "eei", NULL };
                SHR_IF_ERR_EXIT(get_sig_hierarchy_str
                                (unit, core_id, "", stage_list_prt[stage_idx], "", sig_name_hieararchy, "", 1,
                                 sig_value_str, FALSE, TRUE, FALSE, 0));

                /*
                 *  if the top row of the output string is '0' (top row is the overall EEI value), print '-'
                 *  otherwise it'll do DBAL parsing on '0' value and return seemingly valid result
                 */
                if (sh_dnx_pp_first_row_to_int(sig_value_str) == 0)
                {
                    sal_strncpy(sig_value_str, "0", DSIG_MAX_SIZE_STR - 1);
                }

                if (sal_strstr(stage_list_prt[stage_idx], "LLR") || sal_strstr(stage_list_prt[stage_idx], "LBP"))
                {
                    sal_strncpy(sig_value_str, "N/A", RHNAME_MAX_SIZE - 1);
                }

                PRT_CELL_SET("%s", sig_value_str);
            }

            {
                char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { "fwd_action_cpu_trap_code", NULL };
                SHR_IF_ERR_EXIT(get_sig_hierarchy_str
                                (unit, core_id, "", stage_list_prt[stage_idx], "", sig_name_hieararchy, "", 1,
                                 sig_value_str, FALSE, TRUE, FALSE, 0));

                if (sal_strstr(stage_list_prt[stage_idx], "LBP"))
                {
                    sal_strncpy(sig_value_str, "N/A", RHNAME_MAX_SIZE - 1);
                }

                PRT_CELL_SET("%s", sig_value_str);
            }

            /** once we reach IPMF3 stage, the 'fwd_action_strength' signal changes to 'fwd_strength' */
            if (sal_strstr(stage_list_prt[stage_idx], "IPMF3"))
            {
                sal_strncpy(trap_strength_sig_name, "fwd_strength", RHNAME_MAX_SIZE - 1);
            }
            {
                char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { trap_strength_sig_name, NULL };
                SHR_IF_ERR_EXIT(get_sig_hierarchy_str
                                (unit, core_id, "", stage_list_prt[stage_idx], "", sig_name_hieararchy, "", 1,
                                 sig_value_str, FALSE, TRUE, FALSE, 0));

                if (sal_strstr(stage_list_prt[stage_idx], "LBP"))
                {
                    sal_strncpy(sig_value_str, "N/A", RHNAME_MAX_SIZE - 1);
                }

                PRT_CELL_SET("%s", sig_value_str);
            }
            {
                char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { "fwd_action_cpu_trap_qualifier", NULL };
                SHR_IF_ERR_EXIT(get_sig_hierarchy_str
                                (unit, core_id, "", stage_list_prt[stage_idx], "", sig_name_hieararchy, "", 1,
                                 sig_value_str, FALSE, TRUE, FALSE, 0));

                if (sal_strstr(stage_list_prt[stage_idx], "LBP"))
                {
                    sal_strncpy(sig_value_str, "N/A", RHNAME_MAX_SIZE - 1);
                }

                PRT_CELL_SET("%s", sig_value_str);
            }

              /** add a line up to one before last layer */
            if (stage_idx < num_of_stages - 1)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }

        }   /** of for on stage_idx */

          /** Close the table */
        PRT_COMMITX;

    }   /** of core id for loop*/

exit:

    PRT_FREE;
    SHR_FUNC_EXIT;

}

sh_sand_man_t dnx_pp_fdt_man = {
    .brief = "PP Forward Decision Information",
    .full = "Provides the trace (changes) of the forwarding decision for the last packet in the Ingress-PP.\n"
        "Core ID is supported.\n",
    .init_cb = diag_sand_packet_send_simple,
    .examples = "core=0\n" "SHORT"
};

sh_sand_option_t dnx_pp_fdt_options[] = {
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"}
    ,
    {NULL}
};
