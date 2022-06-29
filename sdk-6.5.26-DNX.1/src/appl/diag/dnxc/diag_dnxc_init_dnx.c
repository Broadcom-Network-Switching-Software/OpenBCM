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
#include <soc/defs.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#ifdef BCM_DNX_SUPPORT
#include <appl/diag/dnx/diag_dnx_cmdlist.h>
extern sh_sand_cmd_t sh_all_dnx_commands[];
extern sh_sand_legacy_cmd_t sh_dnx_legacy_commands[];
#endif /* BCM_DNX_SUPPORT */
#ifdef BCM_DNXF_SUPPORT
#include <appl/diag/dnxf/diag_dnxf_cmdlist.h>
extern sh_sand_cmd_t sh_all_dnxf_commands[];
extern sh_sand_legacy_cmd_t sh_dnxf_legacy_commands[];
#endif /* BCM_DNXF_SUPPORT */
#ifdef INCLUDE_AUTOCOMPLETE
#include <src/appl/diag/sand/diag_sand_framework_autocomplete.h>
#endif /* INCLUDE_AUTOCOMPLETE */

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

    if (dnx_drv_soc_property_get(unit, "diag_parallel", 0))
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


/**
 * \brief Recursively traverses command tree from given node, adding the SH_CMD_INIT_REQUIRED flag to all commands.
 * \param [in] unit        - unit id
 * \param [in] sh_sand_cmd - shell command pointer
 * \return
 *   * None
 */
static void
sh_dnxc_traverse_add_init_required_flag(
    int unit,
    sh_sand_cmd_t *sh_sand_cmd)
{
    sh_sand_cmd_t * sh_sand_cmd_curr;
    /*
     * Iterate through all of the commands in the provided shell command array.
     */
    for (sh_sand_cmd_curr = sh_sand_cmd; sh_sand_cmd_curr != NULL && sh_sand_cmd_curr->keyword != NULL; sh_sand_cmd_curr++)
    {
        /*
         * If current command is parent node, recursively add the flag to each of its descendants.
         */
        if (sh_sand_cmd_curr->child_cmd_a)
        {
            sh_dnxc_traverse_add_init_required_flag(unit, sh_sand_cmd_curr->child_cmd_a);
        }
        /*
         * Note that this set function first checks whether the flag is already set so as not to duplicate.
         */
        utilex_prime_flag_set(&(sh_sand_cmd_curr->flags), SH_CMD_INIT_REQUIRED);

#ifdef INCLUDE_AUTOCOMPLETE
        /*
         * Deinit the autocomplete tree whose root is the current command.
         */
        if (utilex_is_prime_flag_set(sh_sand_cmd_curr->flags, SH_CMD_SKIP_ROOT))
        {
            sh_sand_cmd_autocomplete_deinit(unit, sh_sand_cmd_curr->child_cmd_a);
        }
#endif /* INCLUDE_AUTOCOMPLETE */
    }
}

/**
 * \brief
 *  Calibrate the shell command tree without verifications, treating all shell commands as required for init.
 * \param [in] unit         - Device Id
 * \param [in] args         - Command line arguments, may be NULL if called from outside the shell
 * \param [in] sand_control - Shell command control structure
 * \return
 *   int - Error Type
 * \remark
 *   * This command's purpose is to enable invoking shell commands in case the init was not completed successfully.
 *   * Therefore, there is no guarantee that all commands will run successfully when invoked.
 * \see
 *   * None
 */
shr_error_e
sh_dnxc_calibrate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
#ifdef INCLUDE_AUTOCOMPLETE
    sh_sand_cmd_t *sh_sand_cmd;
#endif /* INCLUDE_AUTOCOMPLETE */

    SHR_FUNC_INIT_VARS(unit);

#ifdef INCLUDE_AUTOCOMPLETE
    /*
     * Get the root command of the shell command tree.
     */
    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_root_get(unit, &sh_sand_cmd), "");

    /*
     * Deinit the autocomplete tree whose root is the root of the shell command tree.
     */
    sh_sand_cmd_autocomplete_deinit(unit, sh_sand_cmd);
#endif /* INCLUDE_AUTOCOMPLETE */

    if (!dnxc_init_is_init_done_get(unit))
    {
        /*
         * Recursively add the SH_CMD_INIT_REQUIRED flag to all of the system commands and their descendants.
         */
        sh_dnxc_traverse_add_init_required_flag(unit, sh_sand_sys_cmds);
    }

#ifdef BCM_DNX_SUPPORT
    if (soc_is(unit, DNX_DEVICE))
    {
        if (!dnxc_init_is_init_done_get(unit))
        {
            /*
             * Recursively add the SH_CMD_INIT_REQUIRED flag to all of the DNX commands and their descendants.
             */
            sh_dnxc_traverse_add_init_required_flag(unit, sh_all_dnx_commands);
        }

        /*
         * Calibrate the DNX shell command tree.
         */
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_calibrate(unit, sh_all_dnx_commands, sh_dnx_legacy_commands),
                            "DNX Shell Root Calibration Failed\n");
    }
#endif

#ifdef BCM_DNXF_SUPPORT
    if (soc_is(unit, DNXF_DEVICE))
    {
        if (!dnxc_init_is_init_done_get(unit))
        {
            /*
             * Recursively add the SH_CMD_INIT_REQUIRED flag to all of the DNXF commands and their descendants.
             */
            sh_dnxc_traverse_add_init_required_flag(unit, sh_all_dnxf_commands);
        }

        /*
         * Calibrate the DNXF shell command tree.
         */
        SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_calibrate(unit, sh_all_dnxf_commands, sh_dnxf_legacy_commands),
                            "DNXF Shell Root Calibration Failed\n");
    }
#endif

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_dnxc_calibrate_man = {
    .brief = "DNX/F Calibrate Shell Commands",
    .full = "Iterate through entire command tree, initializing all available commands and options.\n"
            "The calibration occurs automatically at the end of the BCM init stage.\n"
            "Thus, this command is primarily useful for users when debugging errors in the BCM init sequence itself.\n"
};
