/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    diag_sand_dsig.c
 * Purpose:    Routines for handling debug and internal signals
 */

#include <soc/drv.h>

#include <sal/appl/sal.h>

#include <shared/bitop.h>
#include <shared/utilex/utilex_str.h>

#include <appl/diag/sand/diag_sand_dsig.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_signals.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_packet.h>

#include <soc/sand/sand_signals.h>

#include <bcm/types.h>
#include <bcm/port.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>
#include <soc/dnx/pp_stage.h>

#define DEBUG_SIGNAL_STATUS_IS(flag) ((debug_signal->stage_from != NULL) && (debug_signal->stage_from->status & flag))
#endif

/*
 * Defines
 * {
 */

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*
 * }
 */

static sh_sand_man_t signal_get_man = {
    .brief = "List of signals according to match and output filters",
    .full = "Presents list of signals filtered and shown using miscellaneous arguments\n"
        "Status represents different signal properties\n"
        "  reg  - Register based signal, always represents last packet not depending on visibility settings\n"
        "  glob - Signals not influenced by port visibility settings, behaving according to global mode only\n"
        "  prev - Signal represents previous packet from the one on regular signals",
    .examples =
        "from=NIF tx\n"
        "name=action block=irpp\n"
        "from=FWD1 show=noexpand\n"
        "block=IRPP stage=IPMF3\n"
        "block=IRPP full=no show=detail name=FWD\n"
        "to=IPMF1 nz\n" "stage=VTT1-VTT3 name=ttl pem\n" "name=header order=little status\n" "name=outer desc",
    .init_cb = diag_sand_packet_send_simple,
    .deinit_cb = sh_deinit_cb_void
};

static sh_sand_enum_t order_enum_table[] = {
    {"big", PRINT_BIG_ENDIAN, "Print values in big-endian order"},
    {"little", PRINT_LITTLE_ENDIAN, "Print values in little-endian order"},
    {NULL}
};

static sh_sand_option_t signal_get_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Signal name or its substring", ""},
    {"block", SAL_FIELD_TYPE_STR, "Show only signals from this block", ""},
    {"stage", SAL_FIELD_TYPE_STR, "Show only signals going to or coming from this stage", ""},
    {"to", SAL_FIELD_TYPE_STR, "Show only signals going to this stage", ""},
    {"from", SAL_FIELD_TYPE_STR, "Show only signals coming from this stage", ""},
    {"show", SAL_FIELD_TYPE_STR, "List of comma separated option controlling command output\t"
     "detail    - show full ASIC details of the signal\t"
     "hw        - show ASIC signal name\t" "source    - when filtering by name look at ASIC name as well\t"
     "noexpand  - do not expand signals, no fields should be shown\t"
     "noresolve - do not resolve value with their logical string representation\t"
     "exact     - name filter represents entire signal name\t"
     "nocond    - show signals ignoring conditions fir their presentation\t"
#ifdef ADAPTER_SERVER_MODE
     "asic      - show ASIC signals, values will be always zeroes. Used to view ASIC Signals working with Adapter\t"
#endif
     "lucky     - show only first signal matched by input filter", ""},
    {"order", SAL_FIELD_TYPE_ENUM, "Print values in certain byte order", "big", (void *) order_enum_table},
    {"all", SAL_FIELD_TYPE_BOOL, "Prints all signals ignoring other filtering input", "No", NULL, NULL,
     SH_SAND_ARG_QUIET},
    {"full", SAL_FIELD_TYPE_BOOL, "Show all signals including irregular ones", "Yes"},
    {"pem", SAL_FIELD_TYPE_BOOL, "Include signals From/To PEM", "No"},
    {"status", SAL_FIELD_TYPE_BOOL, "Show signals status, representing special characteristics of the signal or field",
     "No"},
    {"NonZero", SAL_FIELD_TYPE_BOOL, "Show only values different from ZERO", "No"},
    {"Desc", SAL_FIELD_TYPE_BOOL, "Show signal description", "No"},
    {"tx", SAL_FIELD_TYPE_BOOL,
     "Sends predefined packet before fetching signals. Used to have signal list before any packet was sent", "No"},
    {NULL}
};

static shr_error_e
sand_signal_expand_print(
    rhlist_t * field_list,
    int depth,
    prt_control_t * prt_ctr,
    int attr_offset,
    int flags)
{
    signal_output_t *field_output;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    RHITERATOR(field_output, field_list)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(attr_offset);

        if (!ISEMPTY(field_output->expansion) && sal_strcasecmp(RHNAME(field_output), field_output->expansion))
        {
            PRT_CELL_SET_SHIFT(depth, "%s(%s)", RHNAME(field_output), field_output->expansion);
        }
        else
        {
            PRT_CELL_SET_SHIFT(depth, "%s", RHNAME(field_output));
        }
        PRT_CELL_SET("%d", field_output->size);
        if (flags & SIGNALS_PRINT_VALUE)
        {
            PRT_CELL_SET("%s", field_output->print_value);
        }
        if (flags & SIGNALS_PRINT_STATUS)
        {
            PRT_CELL_SET("%s", field_output->status);
        }

        SHR_CLI_EXIT_IF_ERR(sand_signal_expand_print(field_output->field_list, depth + 1, prt_ctr, attr_offset, flags),
                            "");
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef ADAPTER_SERVER_MODE
static shr_error_e
signal_get_adapter_output(
    int unit,
    char *block_n,
    debug_signal_t * debug_signal,
    rhlist_t * adapter_dsig_list,
    signal_output_t ** adapter_signal_output_p)
{
    signal_output_t *adapter_signal_output;
    debug_signal_t *adapter_debug_signal;
    SHR_FUNC_INIT_VARS(unit);

    RHITERATOR(adapter_signal_output, adapter_dsig_list)
    {
        adapter_debug_signal = adapter_signal_output->debug_signal;
        if (!sal_strcmp(adapter_debug_signal->attribute, debug_signal->attribute) &&
            !sal_strcmp(adapter_debug_signal->from, debug_signal->from) &&
            !sal_strcmp(adapter_signal_output->block_n, block_n))
        {
            *adapter_signal_output_p = adapter_signal_output;
            SHR_EXIT();
        }
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}
#endif

static uint8
diag_sand_signals_compare_signal_outputs(
    signal_output_t * dest_signal_output,
    signal_output_t * src_signal_output)
{
    if (!dest_signal_output && !src_signal_output)
    {
        return TRUE;
    }

    if (!dest_signal_output)
    {
        return FALSE;
    }

    if (!src_signal_output)
    {
        return FALSE;
    }

    if (dest_signal_output->core == src_signal_output->core &&
        !sal_strcmp(dest_signal_output->debug_signal->attribute, src_signal_output->debug_signal->attribute) &&
        !sal_strcmp(dest_signal_output->debug_signal->expansion, src_signal_output->debug_signal->expansion) &&
        !sal_strcmp(dest_signal_output->debug_signal->from, src_signal_output->debug_signal->from) &&
        !sal_strcmp(dest_signal_output->debug_signal->to, src_signal_output->debug_signal->to))
    {
        return TRUE;
    }

    return FALSE;

}

static shr_error_e
signal_get_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    match_t match_m;
    int core, core_num = 0;
    int attr_offset = 0, addr_offset = 0;
    int i_addr;
    rhlist_t *dsig_list = NULL;
#ifdef ADAPTER_SERVER_MODE
    rhlist_t *adapter_dsig_list = NULL;
#endif
    debug_signal_t *debug_signal;
    signal_output_t *signal_output;
    int all_flag, print_flags = SIGNALS_PRINT_VALUE, status_flag, desc_flag;
    char *show_str;
    int object_col_id = 0;
    char **btokens = NULL;
    uint32 btokens_num = 0;
    int tx_enable;
    int nonzero;
    int full_flag, pem_flag;
    signal_output_t *prev_signal_output = NULL;

#ifdef BCM_DNX_SUPPORT
    char **tokens = NULL;
    uint32 realtokens = 0;
#endif

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_DPP(unit))
    {
        core_num = SOC_DPP_DEFS_GET(unit, nof_cores);
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        core_num = dnx_data_device.general.nof_cores_get(unit);
    }
#endif

    sal_memset(&match_m, 0, sizeof(match_t));

    match_m.flags = SIGNALS_MATCH_EXPAND;
#if defined(BCM_DNX_SUPPORT) && !defined(ADAPTER_SERVER_MODE)
    print_flags |= SIGNALS_PRINT_STATUS;
#endif
    SH_SAND_GET_STR("name", match_m.name);
    SH_SAND_GET_STR("block", match_m.block);
    SH_SAND_GET_STR("from", match_m.from);
    SH_SAND_GET_STR("to", match_m.to);
    SH_SAND_GET_STR("stage", match_m.stage);
    SH_SAND_GET_BOOL("full", full_flag);
    SH_SAND_GET_BOOL("pem", pem_flag);
#ifdef BCM_DNX_SUPPORT
    if (!ISEMPTY(match_m.stage))
    {
        if ((tokens = utilex_str_split(match_m.stage, "-", 3, &realtokens)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Problem Parsing stage argument:\"%s\"\n", match_m.stage);
        }
        /*
         * Only relate to the case with two tokens - any other 1 or 3 ignore and continue as usual on stage name
         */
        if (realtokens == 2)
        {
            match_m.flags |= SIGNALS_MATCH_STAGE_RANGE;
            match_m.from = tokens[0];
            SHR_CLI_EXIT_IF_ERR(dnx_pp_stage_string_to_id(unit, match_m.from, &match_m.from_id),
                                "Stage:\"%s\" has no id\n", match_m.from);
            match_m.to = tokens[1];
            SHR_CLI_EXIT_IF_ERR(dnx_pp_stage_string_to_id(unit, match_m.to, &match_m.to_id),
                                "Stage:\"%s\" has no id\n", match_m.to);
        }
    }
#endif
    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_BOOL("all", all_flag);
    SH_SAND_GET_BOOL("status", status_flag);
    if (status_flag == TRUE)
    {
        print_flags |= SIGNALS_PRINT_STATUS;
    }
    SH_SAND_GET_ENUM("order", match_m.output_order);
    SH_SAND_GET_BOOL("tx", tx_enable);
    SH_SAND_GET_BOOL("NonZero", nonzero);
    SH_SAND_GET_BOOL("desc", desc_flag);
    if (tx_enable == TRUE)
    {
        /*
         * Send example packet to ensure appearance of meaningful signals
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_tx(unit, 0, 0, example_data_string, SAND_PACKET_RESUME));
    }

    /*
     * Check if there is no core id in block name, like IRPP_0
     * If there is split it to core ID and pure block name
     * Otherwise there should not be any underscore in block name
     */
    btokens = utilex_str_split(match_m.block, "_", 2, &btokens_num);
    if ((btokens != NULL) && (btokens_num == 2))
    {
        /** Now we need to verify that second token is number */
        uint32 core_id;
        if (utilex_str_stoul(btokens[1], &core_id) == _SHR_E_NONE)
        {
            if (core_id >= core_num)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal core ID:%d in block name\n", core_id);
            }
            core = core_id;
            match_m.block = btokens[0];
            /** If block was designated with core - it should be presented with it */
            print_flags |= SIGNALS_PRINT_CORE;
        }
    }

    if (all_flag == TRUE)
    {
        match_m.name = NULL;
        match_m.from = NULL;
        match_m.to = NULL;
        match_m.stage = NULL;
    }
    else if (ISEMPTY(match_m.name) && ISEMPTY(match_m.from) && ISEMPTY(match_m.to) && ISEMPTY(match_m.stage) &&
             ISEMPTY(match_m.block))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Please specify some filtering criteria or use \"all\" to show all signals\n");
    }

#ifdef ADAPTER_SERVER_MODE
    /*
     * Currently adapter do not have signals per core,so we use 0 by default instead of all
     */
    if (core == BCM_CORE_ALL)
    {
        core = 0;
        print_flags |= SIGNALS_PRINT_CORE;
    }
#endif

    SH_SAND_GET_STR("show", show_str);

    if (!ISEMPTY(show_str))
    {   /* Show options parsing */
        char **tokens;
        uint32 realtokens = 0;
        int i_token;

        if ((tokens = utilex_str_split(show_str, ",", 6, &realtokens)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Problem parsing show string\n");
        }

        for (i_token = 0; i_token < realtokens; i_token++)
        {
            if (!sal_strcasecmp(tokens[i_token], "detail"))
                print_flags |= SIGNALS_PRINT_DETAIL;
            else if (!sal_strcasecmp(tokens[i_token], "hw"))
                print_flags |= SIGNALS_PRINT_HW;
            else if (!sal_strcasecmp(tokens[i_token], "source"))
                match_m.flags |= SIGNALS_MATCH_HW;
            else if (!sal_strcasecmp(tokens[i_token], "perm"))
                match_m.flags |= SIGNALS_MATCH_PERM;
            else if (!sal_strcasecmp(tokens[i_token], "noexpand"))
                match_m.flags &= ~SIGNALS_MATCH_EXPAND;
            else if (!sal_strcasecmp(tokens[i_token], "exact"))
                match_m.flags |= SIGNALS_MATCH_EXACT;
            else if (!sal_strcasecmp(tokens[i_token], "strict"))
                match_m.flags |= (SIGNALS_MATCH_EXACT | SIGNALS_MATCH_STRICT);
            else if (!sal_strcasecmp(tokens[i_token], "nocond"))
                match_m.flags |= SIGNALS_MATCH_NOCOND;
            else if (!sal_strcasecmp(tokens[i_token], "noresolve"))
                match_m.flags |= SIGNALS_MATCH_NORESOLVE;
            else if (!sal_strcasecmp(tokens[i_token], "lucky"))
                match_m.flags |= SIGNALS_MATCH_ONCE;
#ifdef ADAPTER_SERVER_MODE
            else if (!sal_strcasecmp(tokens[i_token], "asic"))
                match_m.flags |= SIGNALS_MATCH_ASIC;
            /** Here we want to print adapter signals, but only the ones having asic analogs */
            else if (!sal_strcasecmp(tokens[i_token], "asic_only"))
                print_flags |= SIGNALS_PRINT_ASIC;
#endif
            else
            {
                utilex_str_split_free(tokens, realtokens);
                SHR_CLI_EXIT(_SHR_E_PARAM, "Unknown show option:%s\n", tokens[i_token]);
            }
        }

        utilex_str_split_free(tokens, realtokens);
    }

    if (full_flag == FALSE)
    {
        match_m.flags |= SIGNALS_MATCH_NO_REGS;
    }

    if (pem_flag == TRUE)
    {
        match_m.flags |= SIGNALS_MATCH_PEM;
    }

    if ((dsig_list = utilex_rhlist_create("prt_print", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to create signal list\n");
    }

    if (core == BCM_CORE_ALL)
    {
        if (core_num > 1)
        {
            print_flags |= SIGNALS_PRINT_CORE;
        }
        for (core = 0; core < core_num; core++)
        {
            sand_signal_get(unit, core, &match_m, dsig_list, NULL), "";
        }
    }
    else
    {
        if ((core < 0) || (core >= core_num))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal core ID:%d for device\n", core);
        }
#ifdef ADAPTER_SERVER_MODE
        if (print_flags & SIGNALS_PRINT_ASIC)
        {
            if ((adapter_dsig_list = utilex_rhlist_create("adapter_dsi_list", sizeof(signal_output_t), 0)) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to create adapter signal list\n");
            }
            sand_signal_get(unit, core, &match_m, adapter_dsig_list, NULL);
            match_m.flags |= SIGNALS_MATCH_ASIC;
            sand_signal_get(unit, core, &match_m, dsig_list, NULL);
        }
        else
#endif
            SHR_CLI_EXIT_IF_ERR(sand_signal_get(unit, core, &match_m, dsig_list, NULL), "");
    }

    PRT_TITLE_SET("Signals");

    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    attr_offset = PRT_COLUMN_NUM;

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "Attribute");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Size");
    PRT_COLUMN_ADDX_FLEX(PRT_FLEX_BINARY, PRT_XML_ATTRIBUTE, object_col_id, NULL, "Value");

    if (print_flags & SIGNALS_PRINT_STATUS)
    {
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Status");
    }

    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Block");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "From");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "To");

    if ((print_flags & SIGNALS_PRINT_HW) || (print_flags & SIGNALS_PRINT_DETAIL))
    {
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "HW");
    }
    if (print_flags & SIGNALS_PRINT_DETAIL)
    {
        addr_offset = PRT_COLUMN_NUM;
        PRT_COLUMN_ADD("High");
        PRT_COLUMN_ADD("Low");
        PRT_COLUMN_ADD("MSB");
        PRT_COLUMN_ADD("LSB");
        PRT_COLUMN_ADD("Perm");
    }
    if (desc_flag == TRUE)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");
    }
    {
        RHSAFE_ITERATOR(signal_output, dsig_list)
        {
            int depth = 1;
            if (nonzero == TRUE)
            {
                int i_val;
                /*
                 * Scan the array and check if we have non zero values
                 */
                for (i_val = 0; i_val < BITS2WORDS(signal_output->size); i_val++)
                {
                    if (signal_output->value[i_val] != 0)
                        break;
                }
                /*
                 * if we reached the last meaningful member of array, skip it as zero
                 */
                if (i_val == BITS2WORDS(signal_output->size))
                    continue;
            }
#ifdef ADAPTER_SERVER_MODE
            if ((print_flags & SIGNALS_PRINT_ASIC) && (adapter_dsig_list != NULL))
            {
                signal_output_t *adapter_signal_output = NULL;
                /*
                 * Here we need to check if there is such signal in adapter list and print its value
                 * or in practice switch to adapter signal_output
                 * otherwise skip this signal
                 */
                if (signal_get_adapter_output(unit, signal_output->block_n, signal_output->debug_signal,
                                              adapter_dsig_list, &adapter_signal_output) == _SHR_E_NONE)
                {
                    signal_output = adapter_signal_output;
                }
                else
                {
                    continue;
                }
            }
#endif
            debug_signal = signal_output->debug_signal;

#if defined(BCM_DNX_SUPPORT) && !defined(ADAPTER_SERVER_MODE)
            {
                if ((full_flag == FALSE) &&
                    ((debug_signal->block_id >= DBAL_ENUM_FVAL_PP_ASIC_BLOCK_REGISTER) ||
                     DEBUG_SIGNAL_STATUS_IS(SIGNAL_STAGE_PORT_VIS_IGNORE) ||
                     DEBUG_SIGNAL_STATUS_IS(SIGNAL_STAGE_PREVIOUS)))
                {
                    /*
                     * When full flag is down - do now show any irregular packet
                     */
                    continue;
                }
            }
#endif
            if (!(print_flags & SIGNALS_PRINT_HW) && !(print_flags & SIGNALS_PRINT_DETAIL) &&
                diag_sand_signals_compare_signal_outputs(prev_signal_output, signal_output))
            {
                continue;
            }

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);

            if (!ISEMPTY(debug_signal->expansion) && sal_strcasecmp(debug_signal->attribute, debug_signal->expansion))
            {
                PRT_CELL_SET("%s(%s)", debug_signal->attribute, debug_signal->expansion);
            }
            else
            {
                PRT_CELL_SET("%s", debug_signal->attribute);
            }
            PRT_CELL_SET("%d", debug_signal->size);
            PRT_CELL_SET("%s", signal_output->print_value);

            if (print_flags & SIGNALS_PRINT_STATUS)
            {
#if defined(BCM_DNX_SUPPORT) && !defined(ADAPTER_SERVER_MODE)
                /*
                 * All blocks starting from DBAL_ENUM_FVAL_PP_ASIC_BLOCK_REGISTER are registers
                 * and act as "always" visible
                 */
                if (debug_signal->block_id >= DBAL_ENUM_FVAL_PP_ASIC_BLOCK_REGISTER)
                {
                    sal_strncat(signal_output->status, " reg", RHNAME_MAX_SIZE - sal_strlen(signal_output->status) - 1);
                }
                else if (DEBUG_SIGNAL_STATUS_IS(SIGNAL_STAGE_PORT_VIS_IGNORE))
                {
                    sal_strncat(signal_output->status, " glob",
                                RHNAME_MAX_SIZE - sal_strlen(signal_output->status) - 1);
                }
                else if (DEBUG_SIGNAL_STATUS_IS(SIGNAL_STAGE_PREVIOUS))
                {
                    sal_strncat(signal_output->status, " prev",
                                RHNAME_MAX_SIZE - sal_strlen(signal_output->status) - 1);
                }
#endif
                PRT_CELL_SET("%s", signal_output->status);
            }
            if (print_flags & SIGNALS_PRINT_CORE)
            {
                PRT_CELL_SET("%s_%d", signal_output->block_n, signal_output->core);
            }
            else
            {
                PRT_CELL_SET("%s", signal_output->block_n);
            }
            PRT_CELL_SET("%s", debug_signal->from);
            PRT_CELL_SET("%s", debug_signal->to);

            if ((print_flags & SIGNALS_PRINT_HW) || (print_flags & SIGNALS_PRINT_DETAIL))
            {
                PRT_CELL_SET("%s", debug_signal->hw);
            }
            if (print_flags & SIGNALS_PRINT_DETAIL)
            {
                for (i_addr = 0; i_addr < debug_signal->range_num; i_addr++)
                {
                    if (i_addr != 0)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(addr_offset);
                    }
                    PRT_CELL_SET("%d", debug_signal->address[i_addr].high);
                    PRT_CELL_SET("%d", debug_signal->address[i_addr].low);
                    PRT_CELL_SET("%d", debug_signal->address[i_addr].msb);
                    PRT_CELL_SET("%d", debug_signal->address[i_addr].lsb);
                }
                PRT_CELL_SET("%d", debug_signal->perm);
            }
            if (desc_flag == TRUE)
            {
                if (!ISEMPTY(debug_signal->description))
                {
                    PRT_CELL_SET("%s", debug_signal->description);
                }
                else
                {
                    PRT_CELL_SET("N/A");
                }
            }

            SHR_CLI_EXIT_IF_ERR(sand_signal_expand_print
                                (signal_output->field_list, depth, prt_ctr, attr_offset, print_flags), "");

            
            prev_signal_output = signal_output;

        }       /* of signal iterator */
    }

    PRT_COMMITX;
exit:
    if (dsig_list != NULL)
    {
        sand_signal_list_free(dsig_list);
    }
#ifdef BCM_DNX_SUPPORT
    if (tokens != NULL)
    {
        utilex_str_split_free(tokens, realtokens);
    }
#endif
#ifdef ADAPTER_SERVER_MODE
    if (adapter_dsig_list != NULL)
    {
        sand_signal_list_free(adapter_dsig_list);
    }
#endif
    utilex_str_split_free(btokens, btokens_num);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t signal_compose_man = {
    "Compose data per signal structure",
    "Compose data per signal structure, using either default values or the ones provides as options\n"
        "Pay attention than when there are multiple occurrences of the same name, block/from options should be used to get the right one\n"
        "Value may be provided in any format, although output will be always in hex."
        " In the value string last character considered LSB and padded by O for all lacking in direction of MSB\n"
        "Use \"signal struct\" to view all possible structures",
    "name=<struct> [[field=value]...] [block=<name>] [from=<name>]",
    "name=ETH1 ETH1.DA=01:02:03:04:05:06\n" "name=IPv4 IPv4.protocol=0x11\n"
        "name=MPLS MPLS.LABEL=0x333 MPLS.BOS=0 order=little\n" "name=packet_header block=irpp from=VTT1 to=VTT2"
};

static sh_sand_option_t signal_compose_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Structure which data is be composed", NULL},
    {"block", SAL_FIELD_TYPE_STR, "expansion specific block name", ""},
    {"from", SAL_FIELD_TYPE_STR, "expansion specific from stage name", ""},
    {"to", SAL_FIELD_TYPE_STR, "expansion specific to stage name", ""},
    {"order", SAL_FIELD_TYPE_ENUM, "print values in certain endian order <little/big>", "big",
     (void *) order_enum_table},
    {NULL}
};

static shr_error_e
signal_compose_expansion_get(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p)
{
    SHR_FUNC_INIT_VARS(unit);
/*
    SHR_CLI_EXIT_IF_ERR(sand_signal_parse_exists(unit, keyword), "Expansion:%s does not exist\n", keyword);
*/
    *type_p = SAL_FIELD_TYPE_STR;

    SHR_FUNC_EXIT;
}

static shr_error_e
signal_compose_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    signal_output_t signal_output;
    int output_order;
    char *expansion_n = NULL, *block_n, *from_n, *to_n;
    sh_sand_arg_t *sand_arg;
    int i_uint;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&signal_output, 0, sizeof(signal_output_t));

    SH_SAND_GET_STR("name", expansion_n);
    SH_SAND_GET_STR("block", block_n);
    SH_SAND_GET_STR("from", from_n);
    SH_SAND_GET_STR("to", to_n);
    SH_SAND_GET_ENUM("order", output_order);
    /*
     * Fill Structure Name
     */
    sal_strncpy(RHNAME(&signal_output), expansion_n, RHNAME_MAX_SIZE - 1);
    /*
     * Create list for potential non-default field values \
     */
    if ((signal_output.field_list = utilex_rhlist_create("field_list", sizeof(signal_output_t), 0)) == NULL)
        goto exit;
    /*
     * Read all input fields with values
     */
    SH_SAND_GET_ITERATOR(sand_arg)
    {
        rhhandle_t temp = NULL;
        signal_output_t *field_output;
        char *field_n;

        field_n = SH_SAND_GET_NAME(sand_arg);
        /*
         * Add field to the list
         */
        if (utilex_rhlist_entry_add_tail(signal_output.field_list, field_n, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
            goto exit;
        field_output = (signal_output_t *) temp;
        /*
         * Fill field value inside the list element
         */
        sal_strncpy(field_output->print_value, SH_SAND_ARG_STR(sand_arg), DSIG_MAX_SIZE_STR - 1);
    }
    /*
     * We have structure ready - fetch final buffer
     */
     /* coverity[copy_paste_error : FALSE]  */
    SHR_CLI_EXIT_IF_ERR(sand_signal_compose(unit, &signal_output, from_n, to_n, block_n, output_order), "");
    /*
     * It arrives in 2 forms: string for output
     */
    LOG_CLI((BSL_META("%s=%s\n"), RHNAME(&signal_output), signal_output.print_value));
    /*
     * And as uint32 array
     */
    LOG_CLI((BSL_META("%s\n"), RHNAME(&signal_output)));
    for (i_uint = 0; i_uint < BITS2WORDS(signal_output.size); i_uint++)
    {
        LOG_CLI((BSL_META("%08X\n"), signal_output.value[i_uint]));
    }

exit:
    if (signal_output.field_list != NULL)
    {
        sand_signal_list_free(signal_output.field_list);
    }
    SHR_FUNC_EXIT;
}

static sh_sand_man_t signal_parser_man = {
    "Parse value according to the structure name",
    "Accepts signal structure name and optionally block/from names to parse provided value\n"
        "Pay attention than when there are multiple occurrences of the same name, block/from options should be used to get the right one\n"
        "Value may be provided in any format, although output will be always in hex."
        " In the value string last character considered LSB and padded by O for all lacking in direction of MSB\n"
        "Use \"signal struct\" to view all possible structures",
    "[<struct>=<value>] [block=<name>] [from=<name>] [to=<name>]",
    "IPv6_Qualifier=0x0110\n" "packet_header=0x008000000000050000040036 from=prp to=\n"
        "packet_header=0x008000000000050000040036 BLOCK=IRPP size=112\n"
        "eth0=0x00010203040500060708090a0800 order=little"
};

static sh_sand_option_t signal_parser_options[] = {
    {"size", SAL_FIELD_TYPE_INT32, "value size in bits", "0"},
    {"block", SAL_FIELD_TYPE_STR, "expansion specific block name", ""},
    {"from", SAL_FIELD_TYPE_STR, "expansion specific from stage name", ""},
    {"to", SAL_FIELD_TYPE_STR, "expansion specific to stage name", ""},
    {"order", SAL_FIELD_TYPE_ENUM, "print values in certain endian order <little/big>", "big",
     (void *) order_enum_table},
    {NULL}
};

static shr_error_e
signal_parser_expansion_get(
    int unit,
    char *keyword,
    sal_field_type_e * type_p,
    uint32 *id_p,
    void **ext_ptr_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(sand_signal_parse_exists(unit, keyword), "Expansion:%s does not exist\n", keyword);

    *type_p = SAL_FIELD_TYPE_ARRAY32;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
signal_parser_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int attr_offset;
    rhlist_t *dsig_list = NULL;
    signal_output_t *signal_output;
    int output_order;
    int object_col_id = 0;
    char *expansion_n = NULL, *block_n, *from_n, *to_n;
    sh_sand_arg_t *sand_arg;
    int size_in;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("block", block_n);
    SH_SAND_GET_STR("from", from_n);
    SH_SAND_GET_STR("to", to_n);
    SH_SAND_GET_ENUM("order", output_order);
    SH_SAND_GET_INT32("size", size_in);

    PRT_TITLE_SET("Parsed Signal");

    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    attr_offset = PRT_COLUMN_NUM;

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "Attribute");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Size");
    PRT_COLUMN_ADDX_FLEX(PRT_FLEX_BINARY, PRT_XML_ATTRIBUTE, object_col_id, NULL, "Value");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Block");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "From");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "To");

    if ((dsig_list = utilex_rhlist_create("prt_print", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to create signal list\n");
    }

    SH_SAND_GET_ITERATOR(sand_arg)
    {
        expansion_n = SH_SAND_GET_NAME(sand_arg);
        SHR_CLI_EXIT_IF_ERR(sand_signal_parse_get(unit, expansion_n, block_n, from_n, to_n, output_order,
                                                  SH_SAND_ARG_ARRAY_DATA(sand_arg), size_in, dsig_list), "");
    }

    RHITERATOR(signal_output, dsig_list)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        if (sal_strcasecmp(signal_output->expansion, expansion_n))
        {
            PRT_CELL_SET("%s(%s)", expansion_n, signal_output->expansion);
        }
        else
        {
            PRT_CELL_SET("%s", expansion_n);
        }
        PRT_CELL_SET("%d", signal_output->size);
        PRT_CELL_SET("%s", signal_output->print_value);

        PRT_CELL_SET("%s", block_n);
        PRT_CELL_SET("%s", from_n);
        PRT_CELL_SET("%s", to_n);

        SHR_CLI_EXIT_IF_ERR(sand_signal_expand_print
                            (signal_output->field_list, 1, prt_ctr, attr_offset, SIGNALS_PRINT_VALUE), "");
    }

    PRT_COMMITX;
exit:
    if (dsig_list != NULL)
    {
        sand_signal_list_free(dsig_list);
    }
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t signal_struct_man = {
    .brief = "Signal structures",
    .full = "List of parsing capabilities for certain signals",
    .synopsis = NULL,
    .examples = "name=FTMH show=expand\n" "name=OAM field=no\n" "name=Learn_Data show=exact\n" "packet name=ETH desc\n"
        "block=IRPP\n" "name=key from=TERM"
};

static sh_sand_option_t signal_struct_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Struct name or its substring", ""},
    {"block", SAL_FIELD_TYPE_STR, "expansion specific block name", ""},
    {"from", SAL_FIELD_TYPE_STR, "expansion specific from stage name", ""},
    {"packet", SAL_FIELD_TYPE_BOOL, "show only network protocols", "No"},
    {"desc", SAL_FIELD_TYPE_BOOL, "show additional info", "No"},
    {"show", SAL_FIELD_TYPE_STR, "Misc. options to control filtering/output\t"
     "expand - present dynamic expansion options\t"
     "exact  - name filter represents entire structure name\t"
     "lucky  - show only first structure matched by input filter", ""},
    {"field", SAL_FIELD_TYPE_BOOL, "Control whether fields are presented", "Yes"},
    {NULL}
};

static shr_error_e
signal_struct_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n, *block_n, *from_n;
    device_t *device;
    sigstruct_t *cur_sigstruct;
    sigstruct_field_t *cur_sigstruct_field;
    int packet_flag, desc_flag, field_flag;
    int field_column;
    char *show_str;
    int match_flags = 0;
    int struct_col_id = 0, field_col_id = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "");
    }

    SH_SAND_GET_STR("name", match_n);
    SH_SAND_GET_STR("block", block_n);
    SH_SAND_GET_STR("from", from_n);
    SH_SAND_GET_BOOL("packet", packet_flag);
    SH_SAND_GET_BOOL("desc", desc_flag);
    SH_SAND_GET_BOOL("field", field_flag);

    SH_SAND_GET_STR("show", show_str);

    if (!ISEMPTY(show_str))
    {   /* Show options parsing */
        char **tokens;
        uint32 realtokens = 0;
        int i_token;

        if ((tokens = utilex_str_split(show_str, ",", 6, &realtokens)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Problem parsing show string\n");
        }

        for (i_token = 0; i_token < realtokens; i_token++)
        {
            if (!sal_strcasecmp(tokens[i_token], "perm"))
                match_flags |= SIGNALS_MATCH_PERM;
            else if (!sal_strcasecmp(tokens[i_token], "exact"))
                match_flags |= SIGNALS_MATCH_EXACT;
            else if (!sal_strcasecmp(tokens[i_token], "lucky"))
                match_flags |= SIGNALS_MATCH_ONCE;
            else if (!sal_strcasecmp(tokens[i_token], "expand"))
                match_flags |= SIGNALS_MATCH_EXPAND;
            else
            {
                utilex_str_split_free(tokens, realtokens);
                SHR_CLI_EXIT(_SHR_E_PARAM, "Unknown show option:%s\n", tokens[i_token]);
            }
        }

        utilex_str_split_free(tokens, realtokens);
    }

    PRT_TITLE_SET("Parsed Structures");

    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &struct_col_id, "Signal");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, struct_col_id, NULL, "From");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, struct_col_id, NULL, "Block");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, struct_col_id, NULL, "Size");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, struct_col_id, NULL, "Count");
    if (desc_flag == TRUE)
    {
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, struct_col_id, NULL, "Expansion");
        PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, struct_col_id, NULL, "Plugin");
    }
    field_column = PRT_COLUMN_NUM;

    PRT_COLUMN_ADDX(PRT_XML_CHILD, struct_col_id, &field_col_id, "Field");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, field_col_id, NULL, "Start");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, field_col_id, NULL, "Width");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, field_col_id, NULL, "Type");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, field_col_id, NULL, "Smart");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, field_col_id, NULL, "Condition");

    RHITERATOR(cur_sigstruct, device->struct_list)
    {
        /*
         * Print only structure used to represent network ones
         */
        if ((packet_flag == TRUE) && (cur_sigstruct->order != PRINT_BIG_ENDIAN))
            continue;
        /*
         * If there is specific "from" stage name, only signal from this stage may be matched to it
         */
        if (!ISEMPTY(from_n) && (ISEMPTY(cur_sigstruct->from_n) ||
                                 (sal_strcasestr(cur_sigstruct->from_n, from_n) == NULL)))
            continue;
        /*
         * If there is specific block name, only signal belonging to this block may be matched to it
         */
        if (!ISEMPTY(block_n) && (ISEMPTY(cur_sigstruct->block_n) ||
                                  (sal_strcasestr(cur_sigstruct->block_n, block_n) == NULL)))
            continue;

        if (!ISEMPTY(match_n))
        {
            if (match_flags & SIGNALS_MATCH_EXACT)
            {
                if (sal_strcasecmp(RHNAME(cur_sigstruct), match_n))
                    continue;
            }
            else
            {
                if (sal_strcasestr(RHNAME(cur_sigstruct), match_n) == NULL)
                    continue;
            }
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", RHNAME(cur_sigstruct));
        PRT_CELL_SET("%s", cur_sigstruct->from_n);
        PRT_CELL_SET("%s", cur_sigstruct->block_n);
        PRT_CELL_SET("%d", cur_sigstruct->size);
        PRT_CELL_SET("%d", RHUSERS(cur_sigstruct));

        if (desc_flag == TRUE)
        {
            PRT_CELL_SET("%s", cur_sigstruct->expansion_m.name);
            PRT_CELL_SET("%s", cur_sigstruct->plugin_n);
        }
        /*
         * For non array present expansion option, for array present per field
         */
        if ((match_flags & SIGNALS_MATCH_EXPAND) && (cur_sigstruct->array_count == 0))
        {
            int i_opt;
            expansion_option_t *option = cur_sigstruct->expansion_m.options;

            for (i_opt = 0; i_opt < cur_sigstruct->expansion_m.option_num; i_opt++, option++)
            {
                int i_param;
                attribute_param_t *attribute_param = option->param;
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(field_column);
                PRT_CELL_SET("Exp:%s", option->name);
                for (i_param = 0; i_param < DSIG_OPTION_PARAM_MAX_NUM; i_param++, attribute_param++)
                {
                    if (ISEMPTY(attribute_param->name))
                        break;
                    if (i_param != 0)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(field_column + 1);
                    }
                    PRT_CELL_SET("Opt:%s", attribute_param->name);
                    PRT_CELL_SET("Val:%s", attribute_param->value);
                }
            }
        }

        if (field_flag == TRUE)
        {
            RHITERATOR(cur_sigstruct_field, cur_sigstruct->field_list)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(field_column);

                PRT_CELL_SET("%s", RHNAME(cur_sigstruct_field));
                if (cur_sigstruct->order == PRINT_LITTLE_ENDIAN)
                {
                    PRT_CELL_SET("%d", cur_sigstruct_field->start_bit);
                }
                else
                {
                    PRT_CELL_SET("%d", cur_sigstruct->size - cur_sigstruct_field->end_bit - 1);
                }
                PRT_CELL_SET("%d", cur_sigstruct_field->size);
                PRT_CELL_SET("%s", sal_field_type_str(cur_sigstruct_field->type));
                if (!ISEMPTY(cur_sigstruct_field->expansion_m.name))
                {
                    PRT_CELL_SET("%s(E)", cur_sigstruct_field->expansion_m.name);
                }
                else if (!ISEMPTY(cur_sigstruct_field->resolution))
                {
                    PRT_CELL_SET("%s(R)", cur_sigstruct_field->resolution);
                }
                else
                {
                    PRT_CELL_SKIP(1);
                }
                PRT_CELL_SET("%s", cur_sigstruct_field->cond_attribute);
                if ((match_flags & SIGNALS_MATCH_EXPAND)
                    && !sal_strcasecmp(cur_sigstruct_field->expansion_m.name, "Dynamic"))
                {
                    int i_opt;
                    expansion_option_t *option = cur_sigstruct_field->expansion_m.options;

                    for (i_opt = 0; i_opt < cur_sigstruct_field->expansion_m.option_num; i_opt++, option++)
                    {
                        int i_param;
                        attribute_param_t *attribute_param = option->param;
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(field_column);
                        PRT_CELL_SET_SHIFT(1, "%s", option->name);
                        for (i_param = 0; i_param < DSIG_OPTION_PARAM_MAX_NUM; i_param++, attribute_param++)
                        {
                            if (ISEMPTY(attribute_param->name))
                                break;
                            if (i_param != 0)
                            {
                                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                                PRT_CELL_SKIP(field_column + 1);
                            }
                            PRT_CELL_SET_SHIFT(1, "%s", attribute_param->name);
                            PRT_CELL_SET_SHIFT(1, "%s", attribute_param->value);
                        }
                    }
                }
            }
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t signal_stage_man = {
    .brief = "Lists stages for programmable pipeline",
    .full = "List of stages serves as a facilitator to signal filtering tool",
    .synopsis = NULL,
    .examples = "block=IRPP\n" "name=PMF",
};

static sh_sand_option_t signal_stage_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Full stage name or its substring", ""},
    {"block", SAL_FIELD_TYPE_STR, "Full block name or its substring", ""},
    {NULL}
};

static shr_error_e
signal_stage_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n, *block_n;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("name", match_n);
    SH_SAND_GET_STR("block", block_n);

    PRT_TITLE_SET("SIGNAL STAGES");

    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Stage");

    PRT_COLUMN_ADD("PP ID");
#ifdef ADAPTER_SERVER_MODE
    PRT_COLUMN_ADD("Adapter");
    PRT_COLUMN_ADD("MS ID");
    PRT_COLUMN_ADD("NOF Signals");
#endif

    {
        int i_bl, i_st;
        pp_block_t *cur_pp_block;
        pp_stage_t *cur_pp_stage;
        device_t *device;
        if ((device = sand_signal_device_get(unit)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "");
        }

        for (i_bl = 0; i_bl < device->block_num; i_bl++)
        {
            int first_line = TRUE;
            cur_pp_block = &device->pp_blocks[i_bl];

            if (!ISEMPTY(block_n) && (sal_strcasestr(cur_pp_block->name, block_n) == NULL))
                continue;

            for (i_st = 0; i_st < cur_pp_block->stage_num; i_st++)
            {
                cur_pp_stage = &cur_pp_block->stages[i_st];

                if (!ISEMPTY(match_n) && (sal_strcasestr(cur_pp_stage->name, match_n) == NULL))
                    continue;

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                if (first_line == TRUE)
                {
                    PRT_CELL_SET("%s", cur_pp_block->name);
                    first_line = FALSE;
                }
                else
                {
                    PRT_CELL_SKIP(1);
                }
                PRT_CELL_SET("%s", cur_pp_stage->name);
                PRT_CELL_SET("%d", cur_pp_stage->pp_id);
#ifdef ADAPTER_SERVER_MODE
                PRT_CELL_SET("%s", cur_pp_stage->adapter_name);
                PRT_CELL_SET("%d", cur_pp_stage->ms_id);
                PRT_CELL_SET("%d", cur_pp_stage->stage_signal_num);
#endif
            }
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t signal_param_man = {
    .brief = "List of parameters",
    .full = "List of parameters - names for values of certain signals",
    .synopsis = NULL,
    .examples = "name=Code",
};

static sh_sand_option_t signal_param_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Parameter name or its substring", ""},
    {NULL}
};

static shr_error_e
signal_param_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int value_offset;
    char *match_n;
    device_t *device;
    sigparam_t *cur_sigparam;
    sigparam_value_t *cur_sigparam_value;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "");
    }

    SH_SAND_GET_STR("name", match_n);

    PRT_TITLE_SET("Parameters Decoding");
    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    PRT_COLUMN_ADD("Signal");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size");
    value_offset = 2;
#ifdef BCM_DNX_SUPPORT
    PRT_COLUMN_ADD("DBAL");
    value_offset++;
#endif
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value Name");
    PRT_COLUMN_ADD("Value");

    RHITERATOR(cur_sigparam, device->param_list)
    {
        if (!ISEMPTY(match_n) && (sal_strcasestr(RHNAME(cur_sigparam), match_n) == NULL))
            continue;

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", RHNAME(cur_sigparam));
        PRT_CELL_SET("%d", cur_sigparam->size);
#ifdef BCM_DNX_SUPPORT
        PRT_CELL_SET("%s", cur_sigparam->dbal_n);
#endif

        RHITERATOR(cur_sigparam_value, cur_sigparam->value_list)
        {
            if (cur_sigparam_value != utilex_rhlist_entry_get_first(cur_sigparam->value_list))
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(value_offset);
            }

            PRT_CELL_SET("%s", RHNAME(cur_sigparam_value));
            PRT_CELL_SET("%d", cur_sigparam_value->value);

        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t signal_reset_man = {
    .brief = "Deinit/Init signal DB",
    .full = "Deinit/Init signal DB - use after changing any field in XML DB",
    .synopsis = NULL,
    .examples = NULL
};

static shr_error_e
signal_reset_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

#if !defined(NO_FILEIO) && !defined(STATIC_SIGNAL_TEST)
    SHR_CLI_EXIT_IF_ERR(sand_signal_reread(unit), "");

exit:
#endif
    SHR_FUNC_EXIT;
}

#ifdef ADAPTER_SERVER_MODE

static sh_sand_man_t signal_clear_man = {
    .brief = "Remove cached signals from adapter",
    .full = "Remove cached signals from adapter. Usually cached signals allows to improve sig get performance,"
        "but sometimes you need to refetch signals following different match flags"
};

static shr_error_e
signal_clear_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    sand_adapter_clear_signals(unit);

    SHR_FUNC_EXIT;
}

static sh_sand_man_t signal_compare_man = {
    .brief = "Present mapping between adapter and ASIC signals",
    .full = "Present adapter signals using regular filtering criteria:'name', 'block', 'from stage'."
        "If there is ASIC analog for adapter signal it will be presented along with original HW signal name",
    .examples = "name=Fwd_Action block=IRPP\n" "name=Port from=VTT\n"
};

static sh_sand_option_t signal_compare_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "Signal name or its substring for filtering signals to be presented", ""},
    {"block", SAL_FIELD_TYPE_STR, "Block name/substring to be used for signal filtering", ""},
    {"from", SAL_FIELD_TYPE_STR, "'From stage' name/substring to be used for signal filtering", ""},
    {NULL}
};

static shr_error_e
signal_compare_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    match_t match_m;
    rhlist_t *dsig_list = NULL;
    debug_signal_t *debug_signal, *asic_debug_signal;
    signal_output_t *signal_output;

    device_t *device;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "");
    }

    sal_memset(&match_m, 0, sizeof(match_t));

    SH_SAND_GET_STR("name", match_m.name);
    SH_SAND_GET_STR("block", match_m.block);
    SH_SAND_GET_STR("from", match_m.from);

    if ((dsig_list = utilex_rhlist_create("prt_print", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to create signal list\n");
    }

    SHR_CLI_EXIT_IF_ERR(sand_signal_get(unit, 0, &match_m, dsig_list, NULL), "");

    PRT_TITLE_SET("Signals");

    PRT_COLUMN_ADD("Adapter");
    PRT_COLUMN_ADD("PP");
    PRT_COLUMN_ADD("HW");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("From");
    RHITERATOR(signal_output, dsig_list)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        debug_signal = signal_output->debug_signal;
        /*
         * Actual adapter name is in hw field
         */
        PRT_CELL_SET("%s", debug_signal->hw);
        /*
         * Check if there is asic signal and print its name and hw source
         */
        asic_debug_signal = sand_adapter_extract_signal(unit, device, debug_signal->hw,
                                                        signal_output->block_n, debug_signal->from);
        if (asic_debug_signal != NULL)
        {
            PRT_CELL_SET("%s", asic_debug_signal->attribute);
            PRT_CELL_SET("%s", asic_debug_signal->hw);
        }
        else
        {
            PRT_CELL_SKIP(2);
        }
        PRT_CELL_SET("%d", debug_signal->size);
        PRT_CELL_SET("%s", signal_output->block_n);
        PRT_CELL_SET("%s", debug_signal->from);
    }

    PRT_COMMITX;
exit:
    if (dsig_list != NULL)
    {
        sand_signal_list_free(dsig_list);
    }
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#endif

/* *INDENT-OFF* */
sh_sand_cmd_t sh_sand_signal_cmds[] = {
    {"get",       signal_get_cb,     NULL, signal_get_options,     &signal_get_man},
#ifdef ADAPTER_SERVER_MODE
    {"clear",     signal_clear_cb,   NULL, NULL,                   &signal_clear_man},
    {"adapter",   signal_compare_cb, NULL, signal_compare_options, &signal_compare_man},
#endif
    {"compose",   signal_compose_cb, NULL, signal_compose_options, &signal_compose_man, signal_compose_expansion_get},
    {"parser",    signal_parser_cb,  NULL, signal_parser_options,  &signal_parser_man,  signal_parser_expansion_get},
    {"structure", signal_struct_cb,  NULL, signal_struct_options,  &signal_struct_man},
    {"stage",     signal_stage_cb,   NULL, signal_stage_options,   &signal_stage_man},
    {"parameter", signal_param_cb,   NULL, signal_param_options,   &signal_param_man},
    {"reset",     signal_reset_cb,   NULL, NULL,                   &signal_reset_man},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_sand_signal_man = {
    .brief = cmd_sand_signal_desc
};

cmd_result_t
cmd_sand_signal(
    int unit,
    args_t * args)
{
    cmd_result_t result;
    /*
     * Verify command
     */
    SH_SAND_VERIFY("SIGnal", sh_sand_signal_cmds, result);

    sh_sand_act(unit, args, sh_sand_signal_cmds, sh_sand_sys_cmds, NULL);
    ARG_DISCARD(args);
    /*
     * Always return OK - we provide all help & usage from inside framework
     */
exit:
    return result;

}

const char cmd_sand_signal_usage[] = "Please use \"signal(sig) usage\" for help\n";
/*
 * General shell style description
 */
const char cmd_sand_signal_desc[] = "Miscellaneous facilities serving signal eco-system, central one is 'sig get'";

shr_error_e
sand_signal_find(
    int unit,
    int core,
    int extra_flags,
    char *block,
    char *from,
    char *to,
    char *name,
    rhlist_t ** sig_list)
{
    match_t match;
    int sig_list_old_size;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(sig_list, _SHR_E_PARAM, "sig_list");
    /*
     * Setup match data
     */
    memset(&match, 0, sizeof(match_t));
    match.block = block;
    match.from = from;
    match.to = to;
    match.name = name;
    match.flags = extra_flags | SIGNALS_MATCH_EXPAND | SIGNALS_MATCH_EXACT | SIGNALS_MATCH_RETRY;
    /*
     * If necessary, create result list
     */
    if (*sig_list == NULL)
    {
        if ((*sig_list = utilex_rhlist_create("signals", sizeof(signal_output_t), 0)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for signal:'%s'\n", name);
        }
    }
    /*
     * Save signals list size before looking up the current signal
     */
    sig_list_old_size = RHLNUM(*sig_list);
    /*
     * Find required signal
     */
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_get(unit, core, &match, *sig_list, NULL));
    if ((RHLNUM(*sig_list) - sig_list_old_size) != 1)
    {
        /*
         * Expected exactly 1 result. If the result is not single, consider it not found.
         */
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
hex2int(
    char *hex,
    uint32 *val,
    uint8 verbosity)
{
    int val_size;
    int nibble_count;
    int ptr_delta;

    /** In case the hex string is empty, return 0 which will be interpreted in the calling function as error */
    if (sal_strlen(hex) == 0)
    {
        if (verbosity)
        {
            cli_out("Input hex string: %s length is 0 !\n", hex);
        }
        return _SHR_E_PARAM;
    }

    /** trim padding zeros - because they'll interfere with actual number bytes count (string length without padding zeros)
     *  trim only if there's not the case of a single '0' in string
     *  do this by moving the str pointer up until no '0' is observed
     */

    while ((*hex == '0') && (sal_strlen(hex) > 1))
    {
        hex++;
    }

    /** Each string char represents a byte, so we want to calculate number of uint32 in the hex string */
    val_size = (sal_strlen(hex) + 7) / 8;

    /** For MSB uint32, 'nible_count' counts how many nibbles are already present and when to skip to next uint32
     *  nibble_count initial value would determine after how many bit shifts to jump to next word in MSB word
     *  max uint32 value is FFFF FFFF, i.e 8 bytes
     */
    nibble_count = 8 - sal_strlen(hex) % 8;

    /** value should not be 8, it's out of the margins */
    if (nibble_count == 8)
    {
        nibble_count = 0;
    }

    /** The MSB word location in val array, would be the number of words - 1 */
    ptr_delta = val_size - 1;

    while (*hex)
    {
        /** get current character then increment */
        uint8 byte = *hex++;
        /** transform hex character to the 4bit equivalent number, using the ascii table indexes */
        if (byte >= '0' && byte <= '9')
        {
            byte = byte - '0';
        }
        else if (byte >= 'a' && byte <= 'f')
        {
            byte = byte - 'a' + 10;
        }
        else if (byte >= 'A' && byte <= 'F')
        {
            byte = byte - 'A' + 10;
        }
        else
        {
            if (verbosity)
            {
                cli_out("string: %s contains illegal characters !\n", hex);
            }

            return _SHR_E_PARAM;
        }

        /** shift 4 to make space for new digit, and add the 4 bits of the new digit */
        val[ptr_delta] = (val[ptr_delta] << 4) | (byte & 0xF);

        /** each 8 nibbles we will go to lower significant word - the shift operation opearates only on uint32 scope */
        nibble_count++;
        if (nibble_count == 8)
        {
            nibble_count = 0;
            ptr_delta--;
        }
    }

    return _SHR_E_NONE;
}

shr_error_e
compare_signal_value(
    int unit,
    int in_port,
    int out_port,
    char *block,
    char *from,
    char *to,
    char *name,
    char *sig_exp_value,
    int fail_on_mismatch,
    int strict)
{
    uint32 value[DSIG_MAX_SIZE_UINT32] = { 0 };

    int rc = _SHR_E_NONE;
    int pp_port = out_port;
    int resolved_core_id = -1;
    uint32 flags = 0;
    int match_flags = 0;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    char result_str[DSIG_MAX_SIZE_STR];

    if (strict)
    {
        match_flags = SIGNALS_MATCH_STRICT;
    }

    if (sal_strcasestr(from, "PEM") || sal_strcasestr(to, "PEM"))
    {
        match_flags |= SIGNALS_MATCH_PEM;
    }

    /*
     * Understand if the block is Ingress or Egress
     */
    if (tolower(block[0]) == 'i')
    {
        pp_port = in_port;
    }
    else if (tolower(block[0]) != 'e')
    {
        cli_out("Block name %s is none recognizable - should be ITPP, IRPP, ERPP or ETPP\n", block);
        return _SHR_E_PARAM;
    }

#ifdef BCM_DNX_SUPPORT
#ifdef BCM_WARM_BOOT_API_TEST
    /*
     * temporarily disable warmboot regression so that bcm_port_get called from within this diag will not trigger reboot
     */
    if (SOC_IS_DNX(unit))
    {
        dnxc_wb_no_wb_test_set(unit, 1);
    }
#endif /* BCM_WARM_BOOT_API_TEST */
#endif /* BCM_DNX_SUPPORT */

    /*
     * Resolve using in_port, out_port and if the signal is in Ingress or Egress which Core to use
     */
    rc = bcm_port_get(unit, pp_port, &flags, &interface_info, &mapping_info);

#ifdef BCM_DNX_SUPPORT
#ifdef BCM_WARM_BOOT_API_TEST
    if (SOC_IS_DNX(unit))
    {
        dnxc_wb_no_wb_test_set(unit, 0);
    }
#endif /* BCM_WARM_BOOT_API_TEST */
#endif /* BCM_DNX_SUPPORT */

    if (rc != 0)
    {
        cli_out("Could not resolve Core Id !\n");
        return _SHR_E_PARAM;
    }

    resolved_core_id = mapping_info.core;

    /*
     * Get the signal from the adapter
     */
    rc = hex2int(sig_exp_value, value, TRUE);

    if (rc != 0)
    {
        cli_out("Error in hex2int function !\n");
        return _SHR_E_PARAM;
    }

    rc = sand_signal_verify(unit, resolved_core_id, block, from, to, name, value, DSIG_MAX_SIZE_UINT32, NULL,
                            result_str, match_flags);

    if (rc == _SHR_E_UNAVAIL)
    {
        cli_out("*****************************************************************************\n");
        cli_out("Signal is not supported on device, continuing\n");
        cli_out("Block %s, from %s, to %s, signal name %s name\n", block, from, to, name);
        cli_out("*****************************************************************************\n");
        rc = _SHR_E_NONE;
    }
    else if (rc == _SHR_E_FAIL)
    {
        cli_out("*****************************************************************************\n");
        cli_out("Comparison failed! The signals are not equal\n");
        cli_out("Block %s, from %s, to %s, signal name %s name\n", block, from, to, name);
        cli_out("Expected value: 0x%s\n", sig_exp_value);
        cli_out("Received value: 0x%s\n", result_str);
        cli_out("*****************************************************************************\n");
        if (fail_on_mismatch)
        {
            rc = _SHR_E_PARAM;
        }
        else
        {
            rc = _SHR_E_NONE;
        }
    }
    else if (rc == _SHR_E_NOT_FOUND)
    {
        cli_out("*****************************************************************************\n");
        cli_out("Signal not found on device\n");
        cli_out("Block %s, from %s, to %s, signal name %s name\n", block, from, to, name);
        cli_out("*****************************************************************************\n");
    }
    else if (rc != _SHR_E_NONE)
    {
        /** Unknown  unexpected error */
        cli_out("Error:%d in sand_signal_verify\n", rc);
    }
    else
    {
        /** At this point we know there was match */
        if (0)
        {
            cli_out("Value is as expected\n");
        }
    }
    return rc;

}
