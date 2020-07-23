/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file diag_dnx_pp_pkttm.c
 *
 * Packet TM PP Diagnostics - signals that are passed to the TM block.
 */

/*************
 * INCLUDES  *
 */
#include "diag_dnx_pp.h"

/*************
 * DEFINES   *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

/*************
 *  MACROS  *
 */

/*************
 * TYPEDEFS  *
 */

/** define signals and condition structure for print */
typedef struct sig_cond_struct
{
    char *block;
    char *from;
    char *to;
    char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY];
    char *cond_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY];
    uint32 condition_field_expected_value;
    char *print_name;
} sig_cond_struct_t;

/*************
 * GLOBALS   *
 */

/*************
 * FUNCTIONS *
 */

/**
 * \brief - Diagnostic to provide the information to be
 *          processed and used by Ingress TM blocks
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
sh_dnx_pp_pkttm_cmds(
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

        /** index of the counter/meter objects can be from 0 to 9 */
        int stat_idx;

        /* *INDENT-OFF* */
        /** init the signals and condition signals for table print */
        sig_cond_struct_t signal_condition_arr[] = {
            {"IRPP", "LBP", "", {"TM_Cmd", "Fwd_Action_Dst", NULL},     {"TM_Cmd", "Fwd_Action_Dst_Valid", NULL},  0x1, "Fwd_Action_Destination"},
            {"IRPP", "LBP", "", {"TM_Cmd", "tc", NULL},                         {NULL},                                    0x0,  "Tc"},
            {"IRPP", "LBP", "", {"TM_Cmd", "dp", NULL},                         {NULL},                                    0x0,  "Dp"},
            {"IRPP", "LBP", "", {"TM_Cmd", "int_stat_meter_dp_cmd", NULL},      {NULL},                                    0x0,  "Int_Stat_Meter_Dp_Cmd"},
            {"IRPP", "LBP", "", {"TM_Cmd", "lag_lb_key", NULL},                 {NULL},                                    0x0,  "Lag_Lb_Key"},
            {"IRPP", "LBP", "", {"TM_Cmd", "st_vsq_ptr", NULL},                 {NULL},                                    0x0,  "St_Vsq_Ptr"},
            {"IRPP", "LBP", "", {"TM_Cmd", "visibility", NULL},                 {NULL},                                    0x0,  "Visibility"},
            {"IRPP", "LBP", "", {"TM_Cmd", "system_headers_size", NULL},        {NULL},                                    0x0,  "System_Headers_Size"},
            {"IRPP", "LBP", "", {"TM_Cmd", "nwk_header_truncate_size", NULL},   {NULL},                                    0x0,  "NWK_Header_Truncate_Size"},
            {"IRPP", "LBP", "", {"TM_Cmd", "nwk_header_append_size_ptr", NULL}, {NULL},                                    0x0,  "NWK_Header_Append_Size_Ptr"}
        }; /** of signal condition struct initialization */
        /* *INDENT-ON* */

        int sig_condt_size = sizeof(signal_condition_arr) / sizeof(signal_condition_arr[0]);
        int sig_idx;

        /** Print message and continue to next core if not ready */
        DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_IRPP, !short_mode);

        /*
         * Open a table for all TTM signals
         */
        PRT_TITLE_SET("Packet TM Information Core=%d Results", core_id);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size (bits)");

        /** print all the signals that don't require special treatment */
        for (sig_idx = 0; sig_idx < sig_condt_size; sig_idx++)
        {
            SHR_IF_ERR_EXIT(print_sig_in_table
                            (unit, core_id, signal_condition_arr[sig_idx].block, signal_condition_arr[sig_idx].from,
                             signal_condition_arr[sig_idx].to, signal_condition_arr[sig_idx].sig_name_hieararchy,
                             signal_condition_arr[sig_idx].cond_name_hieararchy,
                             signal_condition_arr[sig_idx].condition_field_expected_value,
                             signal_condition_arr[sig_idx].print_name, prt_ctr));
        }

        /*
         * get the statistics from stat[0..9] objects id, if they're valid, and print if they're meters (only for objects 0..2)
         */

         /** run and print the counters */
        for (stat_idx = 0; stat_idx < 10; stat_idx++)
        {
            char stat_obj_name[RHNAME_MAX_SIZE] = "int_stat0_obj_id";
            char stat_obj_valid[RHNAME_MAX_SIZE] = "int_stat0_obj_valid";
            char print_name[RHNAME_MAX_SIZE] = "Counter_ID_0";

             /** change the number of stat object according to the running index */
            stat_obj_name[8] = stat_idx + '0';
            stat_obj_valid[8] = stat_idx + '0';
            print_name[11] = stat_idx + '0';

            {
                char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { "TM_Cmd", stat_obj_name, NULL };
                char *cond_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { "TM_Cmd", stat_obj_valid, NULL };

                SHR_IF_ERR_EXIT(print_sig_in_table
                                (unit, core_id, "IRPP", "LBP", "", sig_name_hieararchy, cond_name_hieararchy, 0x1,
                                 print_name, prt_ctr));
            }

        }

         /** run and print the meters */
        for (stat_idx = 0; stat_idx < 3; stat_idx++)
        {
            char stat_obj_name[RHNAME_MAX_SIZE] = "int_stat0_obj_id";
            char stat_obj_meter[RHNAME_MAX_SIZE] = "int_stat0_obj_meter";
            char print_name[RHNAME_MAX_SIZE] = "Meter_ID_0";

             /** change the number of stat object according to the running index */
            stat_obj_name[8] = stat_idx + '0';
            stat_obj_meter[8] = stat_idx + '0';
            print_name[9] = stat_idx + '0';

            {
                char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { "TM_Cmd", stat_obj_name, NULL };
                char *cond_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { "TM_Cmd", stat_obj_meter, NULL };

                SHR_IF_ERR_EXIT(print_sig_in_table
                                (unit, core_id, "IRPP", "LBP", "", sig_name_hieararchy, cond_name_hieararchy, 0x1,
                                 print_name, prt_ctr));
            }
        }

         /** stat10 is Ethernet Meter ID*/
        {
            char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { "TM_Cmd", "int_stat10_obj_id", NULL };
            SHR_IF_ERR_EXIT(print_sig_in_table
                            (unit, core_id, "IRPP", "LBP", "", sig_name_hieararchy, NULL, 0x0, "Ethernet_Meter_ID",
                             prt_ctr));
        }

         /** run and print the snif */
        for (stat_idx = 0; stat_idx < 3; stat_idx++)
        {
            char snif_name[][RHNAME_MAX_SIZE] = { "snif0_cmd", "snif0_code", "snif0_qualifier" };
            int arr_idx;

            for (arr_idx = 0; arr_idx < 3; arr_idx++)
            {
                char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { "TM_Cmd", snif_name[arr_idx], NULL };

                 /** change the number of stat object according to the running index */
                snif_name[arr_idx][4] = stat_idx + '0';

                SHR_IF_ERR_EXIT(print_sig_in_table
                                (unit, core_id, "IRPP", "LBP", "", sig_name_hieararchy, NULL, 0x0, snif_name[arr_idx],
                                 prt_ctr));
            }
        }

         /** Close the table */
        PRT_COMMITX;

    } /** of core id for loop*/

exit:

    PRT_FREE;
    SHR_FUNC_EXIT;

}

sh_sand_man_t dnx_pp_pkttm_man = {
    .brief = "Ingress TM Command Information",
    .full = "Provides the information to be processed and used by Ingress TM blocks.\n" "Core ID is supported.\n",
    .init_cb = diag_sand_packet_send_simple,
    .examples = "core=0\n" "SHORT"
};

sh_sand_option_t dnx_pp_pkttm_options[] = {
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"},
    {NULL}
};
