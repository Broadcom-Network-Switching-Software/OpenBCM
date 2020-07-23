/*
 * diag_dnx_group.c
 *
 *  Created on: Aug 21, 2018
 *      Author: si888124
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COSQ

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

#include <shared/util.h>
#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/gtimer/gtimer.h>

#include <sal/compiler.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_scheduler_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>

#include "diag_dnx_group.h"
/**
 * { DEFINES
 */

/**
 * }
 */

sh_sand_option_t sh_dnx_tm_sch_group_curr_bw_options[] = {
/* *INDENT-OFF* */
    /*name        type                    desc                    default*/
    {"group",     SAL_FIELD_TYPE_INT32,   "color group",          "-1"},
    {NULL}
/* *INDENT-ON* */
};

sh_sand_man_t sh_dnx_tm_sch_group_curr_bw_man = {
    .brief = "Print current bandwidth per color group",
    .full = "Diagnostic to print the current bandwidth per color group.\n" "uses gtimer.\n",
    .synopsis = "group=<color_group>",
    .examples = "group=2"
};

/**
 * \brief - Print current BW per color group.
 */
shr_error_e
sh_dnx_tm_sch_group_curr_bw_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core_id, core, group, group_id, first_group, last_group, is_group_match, is_flow_match, flow, measured_sec = 1;
    dnxcmn_time_t gtimer_time;
    uint32 nof_clock_cycles, credit_val, is_ovf, credit_kbps;
    int credit_size = dnx_data_iqs.credit.worth_get(unit);
    uint64 credit_bps;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(credit_bps);

    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_INT32("group", group_id);

    if (group_id == -1)
    {
        /**no group was chosen - iterate over all groups */
        first_group = 0;
        last_group = dnx_data_sch.se.nof_color_group_get(unit) - 1;
    }
    else
    {
        first_group = last_group = group_id;
    }

    DNXCMN_CORES_ITER(unit, core_id, core)
    {
        PRT_TITLE_SET("Core %d - Current BandWidth [Kbps]", core);

        for (group = first_group; group <= last_group; ++group)
        {
            PRT_COLUMN_ADD("Group %d", group);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        for (group = first_group; group <= last_group; ++group)
        {
            /** configure counter to count rate for the specified group */
            is_group_match = TRUE;
            is_flow_match = FALSE;
            flow = 0;
            SHR_IF_ERR_EXIT(dnx_sch_dbal_credit_counter_config_set
                            (unit, core, DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_FLOW, is_group_match,
                             is_flow_match, group, flow));

            /** get number of cycles in a second and use it in gtimer.
             * this way the the value presented is in units of Hz */
            gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
            COMPILER_64_SET(gtimer_time.time, 0, measured_sec);
            SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

            /** Enable Gtimer */
            SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_SCH, core));

            /** Wait Gtimer until it reset */
            SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_SCH, core));

            /** Read counter value */
            SHR_IF_ERR_EXIT(dnx_sch_dbal_credit_counter_get(unit, core, &credit_val, &is_ovf));

            /** Calculate the BW in Kbps */
            COMPILER_64_ADD_32(credit_bps, credit_val);
            COMPILER_64_UMUL_32(credit_bps, credit_size * UTILEX_NOF_BITS_IN_BYTE);
            COMPILER_64_UDIV_32(credit_bps, measured_sec);
            COMPILER_64_SHR(credit_bps, UTILEX_BIT_KB_SHIFT);
            credit_kbps = COMPILER_64_LO(credit_bps);

            PRT_CELL_SET("%u", credit_kbps);

            SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_SCH, core));
        }
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_tm_sch_group_cmds[] = {
    /*
     * keyword, action, command, options, man 
     */
    {"bandwidth", sh_dnx_tm_sch_group_curr_bw_cmd, NULL, sh_dnx_tm_sch_group_curr_bw_options,
     &sh_dnx_tm_sch_group_curr_bw_man}
    ,
    {NULL}
};

/**
 * \brief MAN for DNX TM Scheduler Group diagnostics
 */
sh_sand_man_t sh_dnx_tm_sch_group_man = {
    .brief = "Scheduler Group commands",
    .full = NULL
};
