/** \file diag_dnxc_appl_list.c
 * 
 * DNX APPL RUN DIAG PACK - diagnostic pack for running reference applications
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/reference/dnxc/appl_ref_init_deinit.h>
/** bcm */
/** sal */
#include <sal/appl/sal.h>
/** local */
#include "diag_dnxc_appl_list.h"
/*
 * }
 */

/**
 * \brief - List all application steps
 */
shr_error_e
sh_dnxc_appl_list_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const appl_dnxc_init_step_t *appl_steps_list;
    const appl_dnxc_init_step_t *sub_list;
    int appl_id, sub_appl_id;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("Applications list");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("ID");

    /** get the list of steps to run */
    SHR_IF_ERR_EXIT(appl_dnxc_step_list_get(unit, &appl_steps_list));

    /*
     * Iterate over the applications and print their names and IDs
     */
    appl_id = 0;
    while (appl_steps_list[appl_id].step_id != UTILEX_SEQ_STEP_LAST)
    {
        if (appl_steps_list[appl_id].sub_list != NULL)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        }

        /** Dump table line */
        PRT_CELL_SET("%s", appl_steps_list[appl_id].name);
        PRT_CELL_SET("%d", appl_steps_list[appl_id].step_id);

        /** print sublist applications if exists */
        if (appl_steps_list[appl_id].sub_list != NULL)
        {
            sub_list = appl_steps_list[appl_id].sub_list;

            sub_appl_id = 0;
            while (sub_list[sub_appl_id].step_id != UTILEX_SEQ_STEP_LAST)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                /** Dump table line */
                PRT_CELL_SET("  %s", sub_list[sub_appl_id].name);
                PRT_CELL_SET("%d", sub_list[sub_appl_id].step_id);
                sub_appl_id++;
            }
        }

        appl_id++;
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * }
 */
/* *INDENT-OFF* */ 
sh_sand_man_t sh_dnxc_appl_list_man = {
    .brief    = "List all application names and their IDs",
    .full     = "List all application names and their IDs. Using listing information, a specific application can "
                "be invoked through 'run' command"
};
/* *INDENT-ON* */
/*
 * }
 */
