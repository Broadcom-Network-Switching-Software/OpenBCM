/**
 * \file diag_dnx_utils_sbusdma_desc.c Contains all of the descriptor DMA diag commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DMA
#include <shared/bsl.h>

/*************
 * INCLUDES  *
 */
#include <shared/bsl.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <soc/dnx/utils/dnx_sbusdma_desc.h>

#include <appl/diag/diag.h>

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#include "diag_dnx_utils_sbusdma_desc.h"

/*************
 * DEFINES   *
 */

/*************
* FUNCTIONS *
 */

static sh_sand_option_t dnx_sbusdma_desc_enable_options[] = {
    {"ENAble", SAL_FIELD_TYPE_BOOL, "Enable descriptor DMA", "yes"},
    {"LENgth", SAL_FIELD_TYPE_UINT32, "The chain_length_max value", "1000"},
    {"SiZe", SAL_FIELD_TYPE_UINT32, "The buff_size_kb value", "100"},
    {"TO", SAL_FIELD_TYPE_UINT32, "The timeout_usec value", "1000"},
    {NULL}
};

static sh_sand_man_t dnx_sbusdma_desc_enable_man = {
    .brief = "Enable or disable the descriptor DMA.",
    .full =
        "To enable descriptor DMA, provide the three parameters which correspond to the three descriptor DMA soc properties. "
        "To disable descriptor DMA, call with enable=0.",
    .synopsis = "descDMA ENAble [ENAble=<yes/no>] [LENgth=VALUE] [SiZe=VALUE] [TO=VALUE]",
    .examples = "enable=0\n" "length=1000 size=100 to=1000"
};

/**
* \brief
* Change the severity of the MDB module,
* if no parameter than dump the current severity
 */
static shr_error_e
cmd_sbusdma_desc_enable_set(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int enable;
    uint32 chain_length_max, buff_size_kb, timeout_usec;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("ENAble", enable);
    SH_SAND_GET_UINT32("LENgth", chain_length_max);
    SH_SAND_GET_UINT32("SiZe", buff_size_kb);
    SH_SAND_GET_UINT32("TO", timeout_usec);

    if (enable == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_sbusdma_desc_init_with_params(unit, buff_size_kb, chain_length_max, timeout_usec));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_sbusdma_desc_deinit(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_sbusdma_desc_stats_options[] = {
    {"CLear", SAL_FIELD_TYPE_BOOL, "Clear all the statistics counters", "no"},
    {NULL}
};

static sh_sand_man_t dnx_sbusdma_desc_stats_man = {
    .brief = "Print descriptor DMA statistics.",
    .full =
        "Print descriptor DMA statistics. Showing the max and average used values of the chain_length_max and buff_size_kb for the given timeout_usec. Allowing to more easily recognize bottlenecks and fine-tune the parameters for a specific use-case.",
    .synopsis = "descDMA stats [clear=<yes/no>]",
    .examples = "clear\n" ""
};

/**
* \brief
* Change the severity of the MDB module,
* if no parameter than dump the current severity
 */
static shr_error_e
cmd_sbusdma_desc_stats_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int clear;
    dnx_sbusdma_desc_stats_t desc_stats;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("CLear", clear);

    SHR_IF_ERR_EXIT(dnx_sbusdma_desc_get_stats(unit, clear, &desc_stats));

    PRT_TITLE_SET("Current descriptor DMA configuration");
    PRT_COLUMN_ADD("Enabled");
    PRT_COLUMN_ADD("chain_length_max");
    PRT_COLUMN_ADD("buff_size_kb");
    PRT_COLUMN_ADD("timeout_usec");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", desc_stats.enabled == TRUE ? "yes" : "no");
    PRT_CELL_SET("%d", desc_stats.chain_length_max);
    PRT_CELL_SET("%d", desc_stats.buff_size_kb);
    PRT_CELL_SET("%d", desc_stats.timeout_usec);

    PRT_COMMITX;

    if (desc_stats.enabled == TRUE)
    {
        PRT_TITLE_SET("Descriptor DMA statistics");
        PRT_COLUMN_ADD("Commit counter");
        PRT_COLUMN_ADD("Max chain_length");
        PRT_COLUMN_ADD("Avg chain_length");
        PRT_COLUMN_ADD("Max buff_size_kb");
        PRT_COLUMN_ADD("Avg buff_size_kb");

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", desc_stats.commit_counter);
        PRT_CELL_SET("%d", desc_stats.chain_length_counter_max);
        PRT_CELL_SET("%d",
                     (desc_stats.commit_counter !=
                      0 ? UTILEX_DIV_ROUND_UP(desc_stats.chain_length_counter_total, desc_stats.commit_counter) : 0));
        PRT_CELL_SET("%d", desc_stats.buff_size_kb_max);
        PRT_CELL_SET("%d",
                     (desc_stats.commit_counter !=
                      0 ? UTILEX_DIV_ROUND_UP(desc_stats.buff_size_kb_total, desc_stats.commit_counter) : 0));

        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_dnx_utils_sbusdma_desc_man = {
    .brief = "Descriptor DMA diagnostics commands",
    .full = "The dispatcher for the different Descriptor DMA diagnostics commands.",
};

/**
 * \brief Descriptor DMA diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for Descriptor DMA diagnostic commands
 */
sh_sand_cmd_t sh_dnx_utils_sbusdma_desc_cmds[] = {
   /*************************************************************************************************************************************
    *   CMD_NAME    *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *               *                           * Level                   *                                *                            *
    *               *                           * CMD                     *                                *                            *
 */
    {"ENAble", cmd_sbusdma_desc_enable_set, NULL, dnx_sbusdma_desc_enable_options, &dnx_sbusdma_desc_enable_man, NULL,
     NULL, SH_CMD_NO_XML_VERIFY}
    ,
    {"STATS", cmd_sbusdma_desc_stats_print, NULL, dnx_sbusdma_desc_stats_options, &dnx_sbusdma_desc_stats_man, NULL,
     NULL, SH_CMD_NO_XML_VERIFY}
    ,
    {NULL}
};
