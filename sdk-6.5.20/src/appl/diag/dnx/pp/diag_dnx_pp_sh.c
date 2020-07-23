/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file diag_dnx_pp_sh.c
 *
 * System Header PP Diagnostics File
 * Resolves only the system headers that's passed from Ingress to Egress
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

/*************
 * GLOBALS   *
 */

/*************
 * FUNCTIONS *
 */
/**
 * \brief - main function of showing the system headers command
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
sh_dnx_pp_sh_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core_in, core_id;
    signal_output_t *list_element = NULL;

    /** assume default value be FALSE */
    uint8 short_mode = FALSE;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core_in);

    SH_SAND_GET_BOOL("SHORT", short_mode);

    SH_DNX_CORES_ITER(unit, core_in, core_id)
    {
        signal_output_t *sub_list_element;

        char sig_from[RHNAME_MAX_SIZE] = "fabric";
        char sig_root_name[RHNAME_MAX_SIZE] = "pkt_header";
        char sig_block_name[RHNAME_MAX_SIZE] = "ERPP";

        int sig_offset = 0;

        /** Print message and continue to next core if not ready */
        DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_ETPP, !short_mode);

        /*
         * - In adapter take the signal from applet, since from=fabric returns all zeros signal
         *   this means that when a packet isn't received on Egress, applet doesn't exist and
         *   SYSH diagnostics will not show
         */
#ifdef ADAPTER_SERVER_MODE
        sal_strncpy(sig_from, "applet", RHNAME_MAX_SIZE - 1);
        sal_strncpy(sig_block_name, "ETPP", RHNAME_MAX_SIZE - 1);
#endif

        /** get the signal output */
        SHR_IF_ERR_EXIT(sand_signal_output_find
                        (unit, core_id, 0, sig_block_name, sig_from, "", sig_root_name, &list_element));

        /*
         * Open a table for all
         */
        PRT_TITLE_SET("PP VISiblity SYSH Core=%d Results", core_id);

        /** Creating columns which support spaces with no shrinking for hierarchy showing */
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Value");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Size (bits)");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII_ORIG, "Offset (bits)");

        /** iterate through all headers */
        RHITERATOR(sub_list_element, list_element->field_list)
        {
            /** current layer name - careful not to take the expansion name */
            char *name = RHNAME(sub_list_element);
            int nvs_idx;
            int sig_size;
            char *sig_name_hieararchy[SH_DNX_PP_MAX_SIGNAL_HIERARCHY] = { sig_root_name, name, NULL };
            /*
             *  check that the header is system header, if not break
             */
            if (string_in_array(name, FALSE, dnx_pp_system_header_names) == -1)
            {
                break;
            }
            /*
             * Collect the recursive Name, Value, Size and Offset per Each signal and print
             * Iterate on NVS values (Name, Value, Size, Offset)
             */
            sig_size = sub_list_element->size;
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);

            for (nvs_idx = 0; nvs_idx < 4; nvs_idx++)
            {
                char sig_result_str[DSIG_MAX_SIZE_STR] = { 0 };

                /** get the hierarchical name/value/size/offset */
                SHR_IF_ERR_EXIT(get_sig_hierarchy_str
                                (unit, core_id, "", sig_from, "", sig_name_hieararchy, name, nvs_idx,
                                 sig_result_str, FALSE, TRUE, FALSE, sig_offset));

                /** print the hierarchical name/value/size/offset of this system header */
                PRT_CELL_SET("%s", sig_result_str);

            } /** of for NVS */

            /** update the offset for next RHITERATION with current offset + current overall system header size */
            sig_offset = sig_offset + sig_size;

        } /** of RHITERATOR */

        /** close the table per Core_Id */
        PRT_COMMITX;
    } /** of for of core_id */

exit:
    sand_signal_output_free(list_element);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_pp_sh_man = {
    .brief = "PP VISibility SYS_Header diagnostics",
    .full = "Shows System Headers information, received at the ERPP",
    .init_cb = diag_sand_packet_send_simple,
    .examples = "\n" "SHORT"
};

sh_sand_option_t dnx_pp_sh_options[] = {
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"},
    {NULL}
};
