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

#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_rhlist.h>

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_dsig.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#endif

#include <soc/sand/sand_signals.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

static int
dsig_expand_print(
    rhlist_t * field_list,
    int depth,
    prt_control_t * prt_ctr,
    int attr_offset,
    int flags)
{
    char tmp[DSIG_MAX_SIZE_BYTES];
    signal_output_t *field_output;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    RHITERATOR(field_output, field_list)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(attr_offset);

        utilex_str_fill(tmp, depth * 3, ' ');
        if (!ISEMPTY(field_output->expansion) && sal_strcasecmp(RHNAME(field_output), field_output->expansion))
        {
            PRT_CELL_SET("%s%s(%s)", tmp, RHNAME(field_output), field_output->expansion);
        }
        else
        {
            PRT_CELL_SET("%s%s", tmp, RHNAME(field_output));
        }
        PRT_CELL_SET("%d", field_output->size);
        if (flags & SIGNALS_PRINT_VALUE)
        {
            PRT_CELL_SET("%s", field_output->print_value);
        }

        dsig_expand_print(field_output->field_list, depth + 1, prt_ctr, attr_offset, flags);
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dsig_print(
    int unit,
    rhlist_t * dsig_list,
    int flags)
{
    int j;
    int attr_offset = 0, addr_offset = 0;
    debug_signal_t *debug_signal;
    signal_output_t *signal_output;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (dsig_list == NULL)
    {
        cli_out("No signals found\n");
        return _SHR_E_INIT;
    }

    PRT_TITLE_SET("Signals");

    /*
     * Prepare header, pay attention to put header items and content in the same order 
     */
    if (flags & SIGNALS_PRINT_DEVICE)
    {
        PRT_COLUMN_ADD("Device");
    }
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("From");
    PRT_COLUMN_ADD("To");

    attr_offset = PRT_COLUMN_NUM;

    PRT_COLUMN_ADD("Attribute");
    PRT_COLUMN_ADD("Size");

    if (flags & SIGNALS_PRINT_VALUE)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_BINARY, "Value");
    }

    if (flags & SIGNALS_PRINT_DETAIL)
    {
        PRT_COLUMN_ADD("Perm");
        PRT_COLUMN_ADD("HW");
        addr_offset = PRT_COLUMN_NUM;
        PRT_COLUMN_ADD("High");
        PRT_COLUMN_ADD("Low");
        PRT_COLUMN_ADD("MSB");
        PRT_COLUMN_ADD("LSB");
    }
    else if (flags & SIGNALS_PRINT_HW)
    {
        PRT_COLUMN_ADD("HW");
    }

    RHITERATOR(signal_output, dsig_list)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        debug_signal = signal_output->debug_signal;
        if (flags & SIGNALS_PRINT_DEVICE)
        {
            if (signal_output->device)
            {
                PRT_CELL_SET("%s", RHNAME(signal_output->device));
            }
            else
            {
                PRT_CELL_SKIP(1);
            }
        }
        if (flags & SIGNALS_PRINT_CORE)
        {
            PRT_CELL_SET("%s(%d)", signal_output->block_n, signal_output->core);
        }
        else
        {
            PRT_CELL_SET("%s", signal_output->block_n);
        }
        PRT_CELL_SET("%s", debug_signal->from);
        PRT_CELL_SET("%s", debug_signal->to);
        if (!ISEMPTY(debug_signal->expansion) && sal_strcasecmp(debug_signal->attribute, debug_signal->expansion))
        {
            PRT_CELL_SET("%s(%s)", debug_signal->attribute, debug_signal->expansion);
        }
        else
        {
            PRT_CELL_SET("%s", debug_signal->attribute);
        }
        PRT_CELL_SET("%d", debug_signal->size);

        if (flags & SIGNALS_PRINT_VALUE)
        {
            PRT_CELL_SET("%s", signal_output->print_value);
        }

        if (flags & SIGNALS_PRINT_DETAIL)
        {
            PRT_CELL_SET("%d", debug_signal->perm);
            PRT_CELL_SET("%s", debug_signal->hw);
            for (j = 0; j < debug_signal->range_num; j++)
            {
                if (j != 0)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(addr_offset);
                }
                PRT_CELL_SET("%d", debug_signal->address[j].high);
                PRT_CELL_SET("%d", debug_signal->address[j].low);
                PRT_CELL_SET("%d", debug_signal->address[j].msb);
                PRT_CELL_SET("%d", debug_signal->address[j].lsb);
            }
        }
        else if (flags & SIGNALS_PRINT_HW)
        {
            PRT_CELL_SET("%s", debug_signal->hw);
        }

        if (dsig_expand_print(signal_output->field_list, 1, prt_ctr, attr_offset, flags) != _SHR_E_NONE)
        {
            return _SHR_E_MEMORY;
        }
    }

    PRT_COMMIT;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static int
dsig_struct_print(
    int unit,
    device_t * device,
    char *match_n,
    int flags)
{
    int field_shift = 0;
    sigstruct_t *cur_sigstruct;
    sigstruct_field_t *cur_sigstruct_field;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Parsed Structures");

    /*
     * Prepare header, pay attention to put header items and content in the same order 
     */
    if (flags & SIGNALS_PRINT_DEVICE)
    {
        PRT_COLUMN_ADD("Device");
        field_shift++;
    }
    PRT_COLUMN_ADD("Signal");
    field_shift++;
    PRT_COLUMN_ADD("Size");
    field_shift++;
    PRT_COLUMN_ADD("Field");
    PRT_COLUMN_ADD("Start");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("Condition");

    RHITERATOR(cur_sigstruct, device->struct_list)
    {
        if (!ISEMPTY(match_n) && (sal_strcasestr(RHNAME(cur_sigstruct), match_n) == NULL))
            continue;

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        if (flags & SIGNALS_PRINT_DEVICE)
        {
            PRT_CELL_SET("%s", RHNAME(device));
        }

        PRT_CELL_SET("%s", RHNAME(cur_sigstruct));
        PRT_CELL_SET("%d", cur_sigstruct->size);

        RHITERATOR(cur_sigstruct_field, cur_sigstruct->field_list)
        {
            if (cur_sigstruct_field != utilex_rhlist_entry_get_first(cur_sigstruct->field_list))
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(field_shift);
            }

            PRT_CELL_SET("%s", RHNAME(cur_sigstruct_field));
            PRT_CELL_SET("%d", cur_sigstruct_field->start_bit);
            PRT_CELL_SET("%d", cur_sigstruct_field->size);
            PRT_CELL_SET("%s", cur_sigstruct_field->cond_attribute);
        }
    }

    PRT_COMMIT;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static int
dsig_param_print(
    int unit,
    device_t * device,
    char *match_n,
    int flags)
{
    int value_shift = 0;
    sigparam_t *cur_sigparam;
    sigparam_value_t *cur_sigparam_value;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Parameters Decoding");

    /*
     * Prepare header, pay attention to put header items and content in the same order 
     */
    if (flags & SIGNALS_PRINT_DEVICE)
    {
        PRT_COLUMN_ADD("Device");
        value_shift++;
    }
    PRT_COLUMN_ADD("Signal");
    value_shift++;
    PRT_COLUMN_ADD("Size");
    value_shift++;
    PRT_COLUMN_ADD("Value Name");
    PRT_COLUMN_ADD("Value");

    RHITERATOR(cur_sigparam, device->param_list)
    {
        if (!ISEMPTY(match_n) && (sal_strcasestr(RHNAME(cur_sigparam), match_n) == NULL))
            continue;

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        if (flags & SIGNALS_PRINT_DEVICE)
        {
            PRT_CELL_SET("%s", RHNAME(device));
        }
        PRT_CELL_SET("%s", RHNAME(cur_sigparam));
        PRT_CELL_SET("%d", cur_sigparam->size);

        RHITERATOR(cur_sigparam_value, cur_sigparam->value_list)
        {
            if (cur_sigparam_value != utilex_rhlist_entry_get_first(cur_sigparam->value_list))
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(value_shift);
            }

            PRT_CELL_SET("%s", RHNAME(cur_sigparam_value));
            PRT_CELL_SET("%d", cur_sigparam_value->value);
        }
    }

    PRT_COMMIT;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static int
dsig_stage_print(
    int unit,
    device_t * device,
    char *match_n,
    int flags)
{
    int i_dev, i_bl, i_st;
    pp_block_t *cur_pp_block;
    pp_stage_t *cur_pp_stage;
    int stage_skip = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("SIGNAL STAGES");

    /*
     * Prepare header, pay attention to put header items and content in the same order 
     */
    if (flags & SIGNALS_PRINT_DEVICE)
    {
        PRT_COLUMN_ADD("Device");
        stage_skip++;
    }
    PRT_COLUMN_ADD("Block");
    stage_skip++;
    PRT_COLUMN_ADD("Stage");

    for (i_bl = 0; i_bl < device->block_num; i_bl++)
    {
        cur_pp_block = &device->pp_blocks[i_bl];
        i_dev = 0;

        for (i_st = 0; i_st < cur_pp_block->stage_num; i_st++)
        {
            cur_pp_stage = &cur_pp_block->stages[i_st];

            if (!ISEMPTY(match_n) && (sal_strcasestr(cur_pp_stage->name, match_n) == NULL))
                continue;

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);

            if (i_dev == 0)
            {
                if (flags & SIGNALS_PRINT_DEVICE)
                {
                    PRT_CELL_SET("%s", RHNAME(device));
                }

                PRT_CELL_SET("%s", cur_pp_block->name);
                i_dev = 1;
            }
            else
            {
                PRT_CELL_SKIP(stage_skip);
            }

            PRT_CELL_SET("%s", cur_pp_stage->name);
        }
    }

    PRT_COMMIT;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static void
dsig_usage(
    void)
{
    char diag_pp_sig_usage[] =
        "diag pp sig [block=Block] [name=Signal] [from=Stage] [to=Stage] [stage=Stage] [order=little/big] [core=0/1]\n"
        "            [show=show options list]\n"
        "    name, from, to, stage - filtering fields for signal representation\n"
        "        Signal   - name (or any sub-string of name) of specific signal from\n"
        "                   specifications, may be multiple signals from different stages.\n "
        "                   Case insensitive. Search include sub-fields\n"
        "        Stage    - use in \"from\", \"to\" or \"stage\", filters signals that were\n"
        "                   fetched in specific place. See show=stage option for list.\n"
        "                   Any sub-string of actual stage name may be used. Use " " for names\n"
        "                   with space. Using \"stage\" here will lead to all signals where\n"
        "                   stage is either \"from\" or \"to\"\n"
        "    show options - multiple comma separated options may be used\n"
        "        all      - show all signals, ignoring filters\n"
        "        exact    - look for attributes and sub-field with exact match\n"
        "        stage    - presents all PP blocks and stages that may be used in to and from options\n"
        "        noexpand - by default all signals will show also sub-signals were possible.\n"
        "                   using this option will disable expansion\n"
        "        nocond   - does not take into account validity of certain signals.\n"
        "                   By default such invalid signals will not be presented\n"
        "        lucky    - provides first signal matched only\n"
        "        struct   - presents available expansions for signals. May be used with\n"
        "                   name option to limit the output for desired expansions only\n"
        "        param    - presents available resolutions for signal values.  May be\n"
        "                   used with name option to limit the output for desired resolutions only\n"
        "    order\n"
        "        big      - presents MSB first, useful for variable representation\n"
        "                   [default value - no need to use explicitly]\n"
        "        little   - presents LSB first, useful for packet/memory signals\n";

    cli_out(diag_pp_sig_usage);
}

cmd_result_t
cmd_sand_dsig_show(
    int unit,
    args_t * a)
{
    parse_table_t pt;
    cmd_result_t res = CMD_OK;
    int ret;

    char *block_n = NULL, *stage_n = NULL, *from_n = NULL, *to_n = NULL, *show_n = NULL, *match_n = NULL, *order_n =
        "big";

    match_t match_m;
    rhlist_t *dsig_list;
    device_t *device;

    SIG_COMMAND command;
    int print_flags;
    int prm_core = -1;

    char **tokens;
    uint32 realtokens = 0;
    int i;

    /*
     * Get parameters 
     */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "block", PQ_STRING, 0, (void *) &block_n, NULL);
    parse_table_add(&pt, "stage", PQ_STRING, 0, (void *) &stage_n, NULL);
    parse_table_add(&pt, "from", PQ_STRING, 0, (void *) &from_n, NULL);
    parse_table_add(&pt, "to", PQ_STRING, 0, (void *) &to_n, NULL);
    parse_table_add(&pt, "name", PQ_STRING, 0, (void *) &match_n, NULL);
    parse_table_add(&pt, "show", PQ_STRING, 0, (void *) &show_n, NULL);
    parse_table_add(&pt, "order", PQ_STRING, 0, (void *) &order_n, NULL);
    parse_table_add(&pt, "core", PQ_DFL | PQ_INT, 0, &prm_core, NULL);

    ret = parse_arg_eq(a, &pt);
    if (ret <= 0)
    {
        dsig_usage();
        goto exit;
    }

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        goto exit;
    }

    /*
     * Set defaults 
     */
    print_flags = SIGNALS_PRINT_VALUE;
    command = SIG_COMMAND_DEBUG;
    sal_memset(&match_m, 0, sizeof(match_t));

    if (!sal_strcasecmp(order_n, "little"))
        match_m.output_order = PRINT_LITTLE_ENDIAN;
    else
        match_m.output_order = PRINT_BIG_ENDIAN;

    match_m.block = block_n;
    match_m.stage = stage_n;
    match_m.from = from_n;
    match_m.to = to_n;
    match_m.name = match_n;
    match_m.flags = SIGNALS_MATCH_EXPAND;

    tokens = utilex_str_split(show_n, ",", 6, &realtokens);
    if (realtokens && (tokens == NULL))
    {
        cli_out("Inconsistent input command");
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    for (i = 0; i < realtokens; i++)
    {
        if (!sal_strcasecmp(tokens[i], "all"))
        {
            match_m.from = NULL;
            match_m.to = NULL;
            match_m.name = NULL;
        }
        else if (!sal_strcasecmp(tokens[i], "struct"))
            command = SIG_COMMAND_STRUCT;
        else if (!sal_strcasecmp(tokens[i], "param"))
            command = SIG_COMMAND_PARAM;
        else if (!sal_strcasecmp(tokens[i], "noexpand"))
            match_m.flags &= ~SIGNALS_MATCH_EXPAND;
        else if (!sal_strcasecmp(tokens[i], "exact"))
            match_m.flags |= SIGNALS_MATCH_EXACT;
        else if (!sal_strcasecmp(tokens[i], "nocond"))
            match_m.flags |= SIGNALS_MATCH_NOCOND;
        else if (!sal_strcasecmp(tokens[i], "lucky"))
            match_m.flags |= SIGNALS_MATCH_ONCE;
        else if (!sal_strcasecmp(tokens[i], "stage"))
            command = SIG_COMMAND_STAGE;
        else
            cli_out("Unknown show option:%s\n", tokens[i]);
    }

    if (tokens != NULL)
        utilex_str_split_free(tokens, realtokens);

    switch (command)
    {
        case SIG_COMMAND_STAGE:
            res = dsig_stage_print(unit, device, match_n, print_flags);
            break;
        case SIG_COMMAND_STRUCT:
            res = dsig_struct_print(unit, device, match_n, print_flags);
            break;
        case SIG_COMMAND_PARAM:
            res = dsig_param_print(unit, device, match_n, print_flags);
            break;
        case SIG_COMMAND_DEBUG:
            if ((dsig_list = utilex_rhlist_create("prt_print", sizeof(signal_output_t), 0)) == NULL)
            {
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
#ifdef BCM_PETRA_SUPPORT
            if (prm_core == -1)
            {
                if (SOC_DPP_DEFS_GET(unit, nof_cores) > 1)
                {
                    print_flags |= SIGNALS_PRINT_CORE;
                }
                SOC_DPP_CORES_ITER(SOC_CORE_ALL, prm_core)
                {
                    sand_signal_get(unit, prm_core, &match_m, dsig_list, NULL);
                }
            }
            else
            {
                if ((prm_core < 0) || (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores)))
                {
                    cli_out("Illegal core ID:%d for device\n", prm_core);
                    sand_signal_list_free(dsig_list);
                    parse_arg_eq_done(&pt);
                    return CMD_FAIL;
                }
                sand_signal_get(unit, prm_core, &match_m, dsig_list, NULL);
            }
#endif

            res = dsig_print(unit, dsig_list, print_flags);
            sand_signal_list_free(dsig_list);
            break;
        default:
            break;
    }
exit:
    parse_arg_eq_done(&pt);
    return res;
}
