/*
 * diag_dnx_system_red.c
 *
 *  Created on: Oct 3, 2018
 *      Author: si888124
 *
 *  Main diagnostics for System RED
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*************
 * INCLUDES  *
 */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

#include <bcm/cosq.h>

#include <bcm_int/dnx/cosq/ingress/system_red.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_system_red.h>

#include "diag_dnx_system_red.h"

/**
 * \brief MAN for DNX TM Ingress System RED free res thresholds diagnostics
 */
static sh_sand_man_t sh_dnx_tm_system_red_free_res_man = {
    .brief = "system RED free resource thresholds",
    .full = "mapping from free resources ranges to RED-Q-Size"
};

/**
 * \brief Options for DNX TM Ingress system RED free res thresholds diagnostics
 */
static sh_sand_option_t sh_dnx_tm_system_red_free_res_options[] = { {NULL} };

static shr_error_e
sh_dnx_tm_system_red_free_res_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int range, thr;
    int max_range = dnx_data_system_red.info.max_free_res_thr_range_get(unit);
    bcm_gport_t gport = 0;
    bcm_cos_queue_t cosq = 0;
    bcm_cosq_threshold_t threshold;

    bcm_cosq_threshold_type_t thr_types[] = {
        bcmCosqThresholdOcbPacketDescriptorBuffers,
        bcmCosqThresholdOcbDbuffs,
        bcmCosqThresholdBufferDescriptorBuffers
    };
    char *thr_names[] = { "OCB PDBs", "OCB Buffers", "DRAM BDBs" };

    uint32 range_flags[] = {
        BCM_COSQ_THRESHOLD_RANGE_0,
        BCM_COSQ_THRESHOLD_RANGE_1,
        BCM_COSQ_THRESHOLD_RANGE_2,
        BCM_COSQ_THRESHOLD_RANGE_3
    };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Resource to RED-Q-Size Map");
    PRT_COLUMN_ADD("Resource");
    for (range = 0; range <= max_range; ++range)
    {
        PRT_COLUMN_ADD("RED Index %d", range);
        PRT_COLUMN_ADD("Thr %d", range);
    }

    for (thr = DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS; thr < DNX_SYSTEM_RED_RESOURCE_NOF; ++thr)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", thr_names[thr]);

        threshold.type = thr_types[thr];

        for (range = 0; range <= max_range; ++range)
        {
            threshold.flags = BCM_COSQ_THRESHOLD_COLOR_SYSTEM_RED | range_flags[range];
            SHR_IF_ERR_EXIT(bcm_cosq_gport_threshold_get(unit, gport, cosq, &threshold));
            PRT_CELL_SET("%u", threshold.dp);
            PRT_CELL_SET("%u", threshold.value);
        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief MAN for DNX TM Ingress System RED status diagnostics
 */
static sh_sand_man_t sh_dnx_tm_system_red_status_man = {
    .brief = "system RED status.",
    .full = "Print Global / per system port RED index.",
    .synopsis = NULL,
    .examples = "system_port=267\n"
};

/**
 * \brief Options for DNX TM Ingress system RED status diagnostics
 */
static sh_sand_option_t sh_dnx_tm_system_red_status_options[] = {
/* *INDENT-OFF* */
    /*name         type                  desc              default   ext*/
    {"system_port",    SAL_FIELD_TYPE_INT32, "system port",    "-1",     NULL},
    {NULL}
/* *INDENT-ON* */
};

static shr_error_e
sh_dnx_tm_system_red_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 is_sysport_present, sysport, current_red_index, max_red_index;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("system_port", is_sysport_present);

    PRT_TITLE_SET("System RED Status");

    if (is_sysport_present)
    {
        SH_SAND_GET_INT32("system_port", sysport);

        PRT_COLUMN_ADD("System Port");
        PRT_COLUMN_ADD("RED Index");

        SHR_IF_ERR_EXIT(dnx_system_red_sysport_red_status_get(unit, sysport, &max_red_index));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", sysport);
        PRT_CELL_SET("%d", max_red_index);
    }
    else
    {
        PRT_COLUMN_ADD("Current RED Index");
        PRT_COLUMN_ADD("Max RED Index");

        SHR_IF_ERR_EXIT(dnx_system_red_global_red_status_get(unit, &current_red_index, &max_red_index));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", current_red_index);
        PRT_CELL_SET("%d", max_red_index);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_tm_sys_red_cmds[] = {
/* *INDENT-OFF* */
    /*keyword,      action,                             command,    options,                                man                     */
    {"resource",    sh_dnx_tm_system_red_free_res_cmd,  NULL,       sh_dnx_tm_system_red_free_res_options,  &sh_dnx_tm_system_red_free_res_man},
    {"status",      sh_dnx_tm_system_red_status_cmd,    NULL,       sh_dnx_tm_system_red_status_options,    &sh_dnx_tm_system_red_status_man},
    {NULL}
/* *INDENT-ON* */
};

sh_sand_man_t sh_dnx_tm_sys_red_man = {
    .brief = "Print System RED free resources thresholds / status.",
    .full = "Print System RED free resource thresholds map to RED-Q-Size, and Current RED index. \n"
};
