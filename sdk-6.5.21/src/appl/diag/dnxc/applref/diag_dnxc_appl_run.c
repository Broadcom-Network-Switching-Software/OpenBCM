/** \file diag_dnxc_appl_run.c
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
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/reference/dnxc/appl_ref_init_deinit.h>
/** bcm */
/** sal */
#include <sal/appl/sal.h>
/** local */
#include "diag_dnxc_appl_run.h"
/*
 * }
 */

/**
 * \brief - Invoke application by its name
 */
static shr_error_e
sh_dnxc_appl_run_name_cmd(
    int unit,
    char *appl_name,
    int is_deinit)
{
    const appl_dnxc_init_step_t *appl_steps_list;
    appl_dnxc_init_param_t appl_params;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&appl_params, 0x0, sizeof(appl_dnxc_init_param_t));

    /** get the list of steps to run */
    SHR_IF_ERR_EXIT(appl_dnxc_step_list_get(unit, &appl_steps_list));

    if (sal_strcasecmp(appl_name, "all") == 0)/** run full init or full deinit depending on "deinit" argument */
    {
        if (is_deinit == TRUE) /** run full deinit sequence */
        {
            appl_params.no_deinit = 0;
            appl_params.no_init = 1;
        }
        else
        {   /** if (is_deinit == FALSE), run full init sequence*/
            appl_params.no_deinit = 1;
            appl_params.no_init = 0;
        }
        SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_run(unit, &appl_params));
    }
    else
    {   /** run a specific application init or deinit*/
        SHR_IF_ERR_EXIT(appl_dnxc_step_list_name_run(unit, appl_name, appl_steps_list, !is_deinit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Invoke application by its id
 */
static shr_error_e
sh_dnxc_appl_run_id_cmd(
    int unit,
    int appl_id,
    int is_deinit)
{
    const appl_dnxc_init_step_t *appl_steps_list;

    SHR_FUNC_INIT_VARS(unit);

    /** get the list of steps to run from */
    SHR_IF_ERR_EXIT(appl_dnxc_step_list_get(unit, &appl_steps_list));

    /** run the step */
    if (is_deinit) /** run deinit */
    {
        SHR_IF_ERR_EXIT(appl_dnxc_step_list_id_run(unit, appl_id, appl_steps_list, FALSE));
    }
    else
    {  /** run init */
        SHR_IF_ERR_EXIT(appl_dnxc_step_list_id_run(unit, appl_id, appl_steps_list, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - parse args and call to diag_dnxc_port_mgmt_dump() or diag_dnxc_port_mgmt_full_dump with the requested data params
 */
shr_error_e
sh_dnxc_appl_run_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *appl_name = NULL;
    int is_deinit, appl_id;
    appl_dnxc_init_param_t appl_params;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&appl_params, 0x0, sizeof(appl_dnxc_init_param_t));

    SH_SAND_GET_STR("name", appl_name);
    SH_SAND_GET_INT32("id", appl_id);
    SH_SAND_GET_BOOL("deinit", is_deinit);

    /*
     *  Run applications according to provided arguments.
     *  An application can be invoked either by name or by id.
     */

    if ((appl_id == -1) && ISEMPTY(appl_name)) /** run full deinit and init sequence */
    {
        appl_params.no_deinit = 0;
        appl_params.no_init = 0;

        /** run the applications list */
        SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_run(unit, &appl_params));
    }
    else if (!ISEMPTY(appl_name))       /* run application by string */
    {
        SHR_IF_ERR_EXIT(sh_dnxc_appl_run_name_cmd(unit, appl_name, is_deinit));
    }
    else if (appl_id != -1)     /* run application by id */
    {
        SHR_IF_ERR_EXIT(sh_dnxc_appl_run_id_cmd(unit, appl_id, is_deinit));
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Both application ID and name can't be given together at the same time\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
/* *INDENT-OFF* */ 
sh_sand_option_t sh_dnxc_appl_run_options[] = {
    /*name        type                  desc                             default */
    {"id",        SAL_FIELD_TYPE_INT32, "Invoke application by id",      "-1",  },
    {"name",      SAL_FIELD_TYPE_STR,   "Invoke application by name",    "",    },
    {"deinit",    SAL_FIELD_TYPE_BOOL,  "Whether to run init or deinit", "false"},
    {NULL}
};

sh_sand_man_t sh_dnxc_appl_run_man = {
    .brief    = "Diagnostic pack for running init/deinit of reference applications by name or by id",
    .full     = "Diagnostic pack for running init/deinit of reference applications by name or by id"
                "To obtain a list of application names and its IDs use 'dnx application list'"
                "Use command without parameters to run full init/deinit sequence",
    .synopsis = "[name=<application_name|'cosq'|'l2'|'sysports'|'all'>] [id=<application_id|1|8|>] [init]",
    .examples = "\n"
                "name=\"Interrupt Init\"\n"
                "name=\"Interrupt Init\" deinit\n"
                "id=3\n"
                "id=3 deinit"
/*
    .examples = "* 'dnx application run' - run full deinit and init sequence\n"
    "* 'dnx application run name=all deinit' - run full deinit application sequence\n"
    "* 'dnx application run name=all' - run full init application sequence\n"
    "* 'dnx application run name=sysports' - run sysports init application\n"
    "* 'dnx application run name=sysports deinit' - run sysports deinit application\n"
    "* 'dnx application run id=3 deinit' - run applicaion 3 deinit\n"
*/
};


/* *INDENT-ON* */
/*
 * }
 */
