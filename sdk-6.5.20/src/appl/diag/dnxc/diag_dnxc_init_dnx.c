/** \file diag_dnxc_init_dnx.c
 *
 * Diagnostics device reset.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*************
 * INCLUDES  *
 */
/*shared*/
#include <shared/bsl.h>
/*appl*/
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/reference/dnxc/appl_ref_init_deinit.h>

/*************
 *  DEFINES  *
 */

/*************
 *  MACROES  *
 */

/*************
 * FUNCTIONS *
 */

/*
 * See .h file
 */
shr_error_e
sh_dnxc_init_dnx_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    appl_dnxc_init_param_t params;
    int unit_present;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&params, 0, sizeof(params));
    SH_SAND_GET_BOOL("NoInit", params.no_init);
    SH_SAND_GET_BOOL("NoDeinit", params.no_deinit);
    SH_SAND_GET_BOOL("WarmBoot", params.warmboot);

    /** parse unit to init if given explicitly, otherwise use the provided unit */
    SH_SAND_IS_PRESENT("Unit", unit_present);
    if (unit_present)
    {
        SH_SAND_GET_UINT32("Unit", unit);
    }

    /** invoke common(dnx and dnxf) init sequence */
    SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_run(unit, &params));

exit:
    SHR_FUNC_EXIT;
}

/**
 * List of the supported commands, pointer to command function and command usage function.
 */

/* *INDENT-OFF* */
sh_sand_option_t sh_dnxc_init_dnx_options[] = {
    {"NoInit", SAL_FIELD_TYPE_BOOL, "Don't run Init sequence", "No"},
    {"NoDeinit", SAL_FIELD_TYPE_BOOL, "Don't run Deinit sequence", "Yes"},
    {"WarmBoot", SAL_FIELD_TYPE_BOOL, "Enable Warmboot", "No"},
    {"Unit", SAL_FIELD_TYPE_UINT32, "Unit ID", "0"},
    {NULL}
};

sh_sand_man_t sh_dnxc_init_dnx_man = {
    .brief = "DNX Init-Deinit device",
    .full = "Set each parameter to true or false, according to required init-deinit sequence: init only, deinit only, "
            "deinit-init and whether the deinit-init is to be done in warmboot mode. "
            "NoDeinit parameter is true by default. The other parameters are false by default. "
            "This means that with no arguments performs init only. "
            "In order to perform deinit, need to add [NoDeinit=false] explicitly.\n"
            "    Options:\n"
            "    Init only: init_dnx\n"
            "    Deinit only: init_dnx NoDeinit=false NoInit=true\n"
            "    Deinit-init: init_dnx NoDeinit=false\n"
            "    Add [warmboot=true] to the above options to perform the actions in warmboot mode.",
    .synopsis = "[NoInit=<true/false>] [NoDeinit=<true/false>] [warmboot=<true/false>]",
    .examples = "\n"
                "NoDeinit=false NoInit=true Unit=0\n"
                "NoDeinit=false\n"
                "NoDeinit=false warmboot=true\n",
};
