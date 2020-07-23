/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file diag_dnxf_cmdlist.c
 *
 * Purpose: List of commands specific for SOC_IS_DNXF devices
 */

#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_access.h>
#include <appl/diag/sand/diag_sand_mbist.h>

#ifdef INCLUDE_CTEST
#include <appl/ctest/dnxf/ctest_dnxf_root.h>
#include <appl/ctest/dnxc/ctest_dnxc_system.h>
#endif

#include <soc/dnxc/swstate/sw_state_features.h>
#include <soc/sand/sand_aux_access.h>
#ifdef DNX_SW_STATE_DIAGNOSTIC
#include "swstate/auto_generated/diagnostic/swstate_commandline.h"
#endif

#include <appl/diag/dnxf/diag_dnxf_cmdlist.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include "../dnxc/diag_dnxc.h"

extern sh_sand_legacy_cmd_t sh_dnxf_legacy_commands[];

/* *INDENT-OFF* */
static sh_sand_cmd_t sh_dnxf_commands[] = {
    {"access",      NULL,                 sh_sand_access_cmds,   NULL,                     &sh_sand_access_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"application", NULL,                 sh_dnxc_appl_cmds,     NULL,                     &sh_dnxc_appl_man},
    {"avs",         NULL,                 sh_dnxc_avs_cmds,      NULL,                     &sh_dnxc_avs_man},
    {"clear",       NULL,                 sh_dnxc_clear_cmds,    NULL,                     &sh_dnxc_clear_man},
    {"counter",     cmd_dnxc_counter,     NULL,                  dnxc_counter_options,     &sh_dnxf_counter_man},
    {"data",        NULL,                 sh_dnxc_data_cmds,     NULL,                     &sh_dnxc_data_man},
/** diag is for backward compatibility commands only, no new functionality should use it */
    {"diag",        NULL,                 sh_dnxc_diag_cmds,     NULL,                     &sh_dnxc_diag_man},
    {"fabric",      NULL,                 sh_dnxc_fabric_cmds,   NULL,                     &sh_dnxc_fabric_man},
    {"init_dnx",    sh_dnxc_init_dnx_cmd, NULL,                  sh_dnxc_init_dnx_options, &sh_dnxc_init_dnx_man, NULL, NULL, SH_CMD_SKIP_EXEC},
    {"interrupt",   NULL,                 sh_dnxc_intr_cmds,     NULL,                     &sh_dnxc_intr_man},
    {"linkscan",    cmd_dnxc_linkscan,    NULL,                  dnxc_linkscan_options,    &sh_dnxf_linkscan_man},
    {"mbist",       NULL,                 sh_sand_mbist_cmds,    NULL,                     &sh_sand_mbist_man},
    {"pbmp",        cmd_dnxc_pbmp,        NULL,                  dnxc_pbmp_options,        &sh_dnxc_pbmp_man},
    {"phy",         NULL,                 sh_dnxc_phy_cmds,      NULL,                     &sh_dnxc_phy_man},
    {"port",        NULL,                 sh_dnxc_port_cmds,     NULL,                     &sh_dnxc_port_man},
    {"show",        NULL,                 sh_dnxc_show_cmds,     NULL,                     &sh_dnxc_show_man},
    {"soc",         cmd_dnxc_soc,         NULL,                  NULL,                     &sh_dnxc_soc_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
#ifdef BCM_WARM_BOOT_SUPPORT
    {"warmboot",    NULL,                 sh_dnxc_warmboot_cmds, NULL,                     &sh_dnxc_warmboot_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
#endif
#ifdef INCLUDE_CTEST
    {"FRameWork",   NULL,                  sh_sand_shell_cmds,   NULL,                     &sh_sand_shell_man, NULL, NULL, CTEST_PRECOMMIT},
#endif
    {NULL}      /* This line should always stay as last one */
};

static sh_sand_man_t sh_dnxf_man = {
    .brief = "Invocation point for dnxf specific commands",
    .full  = "DNX specific commands. Use \"dnx cmdname usage\" for details\n"
};

sh_sand_man_t sh_dnxf_swstate_root_man = {
    .brief = "SwState specific commands",
    .full  = "Root command for swstate tree. Use \"swstate usage\" for details\n"
};

/*
 * Invokes field is used to transfer system command array per command, relevant only for root level,
 * where no invokes are used
 */
static sh_sand_cmd_t sh_all_dnxf_commands[] = {
  /* Name     | Action | Node Array           | Options | Usage */
    /* This line must always stay first one - it is used to get to device specific commands */
    {"dnxf",    NULL, sh_dnxf_commands,       NULL, &sh_dnxf_man,              NULL, (sh_sand_invoke_t *)sh_sand_sys_cmds, SH_CMD_VERIFY | SH_CMD_SKIP_ROOT},
#ifdef INCLUDE_CTEST
    {"CTest",   NULL, sh_dnxf_ctest_commands, NULL, &sh_dnxf_ctest_man,        NULL, (sh_sand_invoke_t *)sh_dnxc_sys_ctest_cmds},
#endif
#ifdef DNX_SW_STATE_DIAGNOSTIC
    {"swstate", NULL, sh_dnxf_swstate_cmds,   NULL, &sh_dnxf_swstate_root_man, NULL, (sh_sand_invoke_t *)sh_sand_sys_cmds},
#endif /* DNX_SW_STATE_DIAGNOSTIC */
    {NULL} /* This line should always stay as last one */
};
/* *INDENT-ON* */

shr_error_e
cmd_dnxf_sh_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(sh_sand_root_init(unit, sh_all_dnxf_commands, sh_dnxf_legacy_commands),
                        "DNXF Shell Root Init Failed");
    /*
     * Failure on this routine does not count
     */
    shr_access_device_init(unit);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
cmd_dnxf_sh_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(shr_access_device_deinit(unit), "Aux Access ");
    SHR_CLI_EXIT_IF_ERR(sh_sand_root_deinit(unit), "DNX Shell Root Init Failed");

    /*
     * Init is done on first call to the diag, in order not to allocated
     * memory which is not yet needed.
     * Here we deallocate this memory.
     */
    SHR_IF_ERR_EXIT(diag_dnxc_diag_counter_values_database_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

cmd_t bcm_dnxf_cmd_list[] = {

#if defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)
    {"cpu_regs", cmd_sand_cpu_regs, cmd_sand_cpu_regs_usage, "Read/Write function to cpu regs"},
#endif
    {"DEviceReset", sh_device_reset, sh_device_reset_usage, "Perform different device reset modes/actions."},
    {"DMA", cmd_sand_dma, cmd_sand_dma_usage, "DMA Facilities Interface"},
    {"Dump", cmd_sand_mem_get, cmd_sand_mem_get_usage, "Dump an address space or registers"},
    {"Getreg", cmd_sand_reg_get, cmd_sand_reg_get_usage, "Get register"},
#ifdef INCLUDE_I2C
    {"I2C", cmd_i2c, cmd_i2c_usage, "Inter-Integrated Circuit (I2C) Bus commands"},
#endif
    {"LISTmem", cmd_sand_mem_list, cmd_sand_mem_list_usage, "List the entry format for a given table"},
    {"Listreg", cmd_sand_reg_list, cmd_sand_reg_list_usage, "List register fields"},
    {"MODify", cmd_sand_mem_modify, cmd_sand_mem_modify_usage, "Modify table entry by field names"},
    {"Modreg", cmd_sand_reg_modify, cmd_sand_reg_modify_usage, "Register Read/Modify/Write"},
    {"PacketWatcher", pw_command, pw_usage, "Monitor ports for packets"},
    {"PCIC", cmd_sand_pcic_access, cmd_sand_pcic_access_usage, "Access to PCI configuration space"},
    {"pcie", cmd_sand_pcie_reg, cmd_sand_pcie_reg_usage, "Read/Write form devices via pcie"},
#ifdef PORTMOD_DIAG
    {"PortMod", cmd_portmod_diag, portmod_diag_usage, "portmod diagnostics"},
#endif /* PORTMOD_DIAG */
    {"Set_Device",      cmd_set_device,           cmd_set_device_usage,
     "Common set_device command" },
    {"Setreg", cmd_sand_reg_set, cmd_sand_reg_set_usage, "Set register"},
#ifdef INCLUDE_TEST
    {"TestClear", test_clear, test_clear_usage, "Clear run statistics for a test"},
    {"TestList", test_print_list, test_list_usage, "List loaded tests and status"},
    {"TestMode", test_mode, test_mode_usage, "Set global test run modes"},
    {"TestParameters", test_parameters, test_parameters_usage, "Set test Parameters"},
    {"TestRun", test_run, test_run_usage, "Run a specific or selected tests"},
    {"TestSelect", test_select, test_select_usage, "Select tests for running"},
#endif
    {"Write", cmd_sand_mem_write, cmd_sand_mem_write_usage, "Write entry(s) into a table"},
#if defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)
    {"XXReload", cmd_xxreload, cmd_xxreload_usage, "\"Easy\" Reload control"}
    ,
#endif /* defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT) */
};

int bcm_dnxf_cmd_cnt = COUNTOF(bcm_dnxf_cmd_list);
