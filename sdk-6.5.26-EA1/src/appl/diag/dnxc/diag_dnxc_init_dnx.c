/** \file diag_dnxc_init_dnx.c
 *
 * Diagnostics device reset.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*************
 * INCLUDES  *
 *************/
/*shared*/
#include <shared/bsl.h>
/*appl*/
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/reference/dnxc/appl_ref_init_deinit.h>
#include <shared/shr_thread_manager.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/drv.h>
#include <soc/dnxc/drv_dnxc_utils.h>

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

shr_thread_manager_handler_t init_thread[SOC_MAX_NUM_DEVICES] = { 0 };
appl_dnxc_init_param_t params[SOC_MAX_NUM_DEVICES] = { {0}
};

/*************
 * FUNCTIONS *
 *************/

static shr_error_e
sh_dnxc_init_run(
    int unit,
    void *info)
{
    appl_dnxc_init_param_t *params;
    SHR_FUNC_INIT_VARS(unit);

    params = (appl_dnxc_init_param_t *) info;

    /** invoke common(dnx and dnxf) init sequence */
    SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_run(unit, params));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
sh_dnxc_init_dnx_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int unit_present, join_present;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&params[unit], 0, sizeof(params[unit]));
    SH_SAND_GET_BOOL("NoInit", params[unit].no_init);
    SH_SAND_GET_BOOL("NoDeinit", params[unit].no_deinit);
    SH_SAND_GET_BOOL("WarmBoot", params[unit].warmboot);
    SH_SAND_IS_PRESENT("Join", join_present);

    /** parse unit to init if given explicitly, otherwise use the provided unit */
    SH_SAND_IS_PRESENT("Unit", unit_present);
    if (unit_present)
    {
        SH_SAND_GET_UINT32("Unit", unit);
    }

    if (join_present)
    {
        for (int current_unit = 0; current_unit < SOC_MAX_NUM_DEVICES; current_unit++)
        {
            if (init_thread[current_unit] != 0)
            {
                SHR_IF_ERR_EXIT(shr_thread_manager_join(&init_thread[current_unit], SHR_THREAD_MANAGER_MAX_TIMEOUT));
                sal_memset(&init_thread[unit], 0, sizeof(shr_thread_manager_handler_t));
            }
        }
        SHR_EXIT();
    }

    if (dnx_drv_soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "parallel_mgmt_init", 0))
    {
        shr_thread_manager_config_t thread_config_info;
        sal_memset(&init_thread[unit], 0, sizeof(shr_thread_manager_handler_t));

        shr_thread_manager_config_t_init(&thread_config_info);
        thread_config_info.name = "init";
        thread_config_info.callback = sh_dnxc_init_run;
        thread_config_info.type = SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION;
        thread_config_info.thread_priority = 1;
        thread_config_info.joinable = TRUE;
        thread_config_info.stop_timeout_usec = SHR_THREAD_MANAGER_MAX_TIMEOUT;
        thread_config_info.bsl_module = BSL_LOG_MODULE;
        thread_config_info.user_data = &params[unit];
        SHR_IF_ERR_EXIT(shr_thread_manager_create(unit, &thread_config_info, &init_thread[unit]));
    }
    else
    {
        SHR_IF_ERR_EXIT(sh_dnxc_init_run(unit, &params[unit]));
    }
    SHR_EXIT();

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
    {"Join", SAL_FIELD_TYPE_BOOL, "Join all active init sequence treads", "No"},
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
            "    Join all active init sequence threads: init_dnx join\n"
            "    Add [warmboot=true] to the above options to perform the actions in warmboot mode."
            "    ",
    .synopsis = "[NoInit=<true/false>] [NoDeinit=<true/false>] [warmboot=<true/false>]",
    .examples = "\n"
                "NoDeinit=false NoInit=true Unit=0\n"
                "NoDeinit=false\n"
                "NoDeinit=false warmboot=true\n"
                "JOIN",
};
