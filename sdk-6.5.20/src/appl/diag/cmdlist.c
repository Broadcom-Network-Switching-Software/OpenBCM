/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * socdiag command list
 */

#include <soc/drv.h>
#include <appl/diag/system.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
/*
 * The following are always turned off in portability mode
 */
#ifdef NO_SAL_APPL

#undef INCLUDE_I2C

#endif /* NO_SAL_APPL */

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
#include <appl/diag/dcmn/cmdlist.h>
#endif

#if defined(BCM_DNX_SUPPORT)
#include <appl/diag/dnx/diag_dnx_cmdlist.h>
#endif

#if defined(BCM_DNXF_SUPPORT)
#include <appl/diag/dnxf/diag_dnxf_cmdlist.h>
#endif

#if defined(BCM_SAND_SUPPORT) && (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__))
#include <appl/diag/sand/diag_sand_access.h>
#endif


#if defined(BCM_LTSW_SUPPORT)
#include <appl/diag/ltsw/cmdlist.h>
#endif

#include <appl/diag/diag.h>
#include <appl/diag/cmdlist.h>

cmd_t *cur_cmd_list[SOC_MAX_NUM_DEVICES];
int cur_cmd_cnt[SOC_MAX_NUM_DEVICES];
int cur_mode[SOC_MAX_NUM_DEVICES];

cmd_t * dyn_cmd_list[SOC_MAX_NUM_DEVICES];
int     dyn_cmd_cnt[SOC_MAX_NUM_DEVICES];

#if defined(BCM_ESW_SUPPORT)
static cmd_result_t (*_bcm_trunk_ref)(int unit, args_t *a) = _bcm_diag_trunk_show;
#endif

#if defined(INCLUDE_LIB_CPUDB) && defined(INCLUDE_LIB_CPUTRANS) && \
    defined(INCLUDE_LIB_DISCOVER) && defined(INCLUDE_LIB_STKTASK)
#define TKS_SUPPORT
#endif

#ifndef BCM_SHARED_LIB_SDK

/* This is the list of commands that can be executed independent of mode */
cmd_t bcm_cmd_common[] = {
    {"?",               sh_help_short,          sh_help_short_usage,
     "Display list of commands"},
    {"??",              sh_help,                sh_help_usage,
     "@help" },
#if defined(INCLUDE_LIB_AEDEV)
    {"AEDev",  cmd_aedev, cmd_aedev_usage, "AE Development Environment Utility."},
    {"SWDev",  cmd_swdev, cmd_swdev_usage, "SW Development Environment Utility."},
#endif
#if defined(INCLUDE_APIMODE)
    {"API",       cmd_api,      cmd_api_usage,
     "Control API command mode parsing."},
#endif
    {"ASSert",       cmd_assert,      cmd_assert_usage,
     "Assert"},
#if defined(BCM_ASYNC_SUPPORT)
    {"ASYNC",       cmd_async,      cmd_async_usage,
     "Control BCM API Async daemon."},
#endif
    {"Attach",          sh_attach,              sh_attach_usage,
     "Attach SOC device(s)" },
    {"BackGround",      sh_bg,                 sh_bg_usage,
     "Execute a command in the background."},
#if defined(INCLUDE_BOARD)
    {"Board",           board_cmd,              board_cmd_usage,
     "New Board support."},
#endif
#if defined(BROADCOM_DEBUG)
    {"break",           sh_break,          sh_break_usage,        "place to hang a breakpoint" },
#endif
#if defined(BCM_CMICM_SUPPORT)
    {"BroadSync",       cmd_broadsync,          cmd_broadsync_usage,     "Manage Time API BroadSync endpoints"},
#endif
#if defined(INCLUDE_GDPLL)
    {"GDPLL",       cmd_gdpll,          cmd_gdpll_usage,     "GDPLL Channel Management"},
#endif
    {"CASE",            sh_case,                sh_case_usage,
     "Execute command based on string match"},
#ifndef NO_SAL_APPL
#ifndef NO_UNIX_ACCESS
    {"CD",              sh_cd,                  sh_cd_usage,
     "Change current working directory"},
#endif
#if defined(INCLUDE_LIB_CINT)
    {"cint",            cmd_cint,                cmd_cint_usage,
     "Enter the C interpreter",
    },
#endif
#endif
#if defined(BCM_API_VERBOSE_LOGGING)
    {"CintApiLogging",   cint_logger_cmd,  cint_logger_usage,
     "Configure the CINT API Logger functionality"},
#endif
    {"ClearScreen",         sh_clear_screen,             sh_clear_screen_usage,
     "Clear terminal output"},
#ifndef NO_SAL_APPL
    {"CONFig",          sh_config,              sh_config_usage,
     "Configure Management interface"},
    {"CONSole",         sh_console,             sh_console_usage,
     "Control console options"},
#ifndef NO_UNIX_ACCESS
    {"CoPy",            sh_copy,                sh_copy_usage,
     "Copy a file"},
#endif
#endif /* NO_SAL_APPL */
#if defined(BCM_SAND_SUPPORT) && (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__))
    {"cpu_i2c", cmd_sand_cpu_i2c, cmd_sand_cpu_i2c_usage, "Read/Write I2C via cpu"},
    {"cpu_regs", cmd_sand_cpu_regs, cmd_sand_cpu_regs_usage, "Read/Write function to cpu regs"},
#endif
#if defined(TKS_SUPPORT)
    {"CPUDB",           ct_cpudb,               ct_cpudb_usage,
     "Update the CPU database"},
    {"CTEcho",          ct_echo,                ct_echo_usage,
     "Send an echo request using CPUTRANS"},
    {"CTInstall",       ct_install,             ct_install_usage,
     "Set up transport pointers in CPU transports"},
    {"CTSetup",         ct_setup,               ct_setup_usage,
     "Modify the CPUTRANS setup"},
#endif
#ifndef NO_SAL_APPL
    {"DATE",            sh_date,                sh_date_usage,
     "Set or display current date"},
#endif
#if defined(TKS_SUPPORT)
    {"DBDump",          cmd_st_db_dump,         cmd_st_db_dump_usage,
     "Dump the current StackTask CPUDB"},
    {"DBParse",         cmd_cpudb_parse,        cmd_cpudb_parse_usage,
     "Parse a line of CPUDB dumped code"},
#endif
    {"DeBug",           sh_debug,               sh_debug_usage,
     "Enable/Disable debug output"},
    {"DELAY",           sh_delay,               sh_delay_usage,
     "Put CLI task in a busy-wait loop for some amount of time"},
#if defined(BCM_XGS_SUPPORT)
    {"DEVice",          cmd_device,             cmd_device_usage,
     "Device add/remove"},
#endif /* BCM_XGS_SUPPORT */
#if !defined(NO_SAL_APPL) && !defined(NO_UNIX_ACCESS)
    {"DIR",             sh_ls,                  sh_ls_usage,
     "@ls" },
#endif
    {"DISPatch",    cmd_dispatch,       cmd_dispatch_usage,
     "BCM Dispatch control."},
     {"Echo",            sh_echo,               sh_echo_usage,
     "Echo command line"},
#ifndef NO_SAL_APPL
#ifdef INCLUDE_TCL
    {"ErrorExit",         sh_error_exit,             sh_error_exit_usage,
     "Control error exit options"},
#endif /* INCLUDE_TCL */
#endif /* NO_SAL_APPL */
#ifndef NO_UNIX_ACCESS
    {"EXIT",            sh_exit,                sh_exit_usage,
     "Exit the current shell (and possibly reset)" },
#endif
    {"EXPR",            sh_expr,                sh_expr_usage,
     "Evaluate infix expression" },
#ifndef NO_SAL_APPL
    {"FLASHINIT",       sh_flashinit,           sh_flashinit_usage,
     "Initialize on board flash as a file system"},
    {"FLASHSYNC",       sh_flashsync,           sh_flashsync_usage,
     "Sync up on board flash with file system"},
#endif
    {"FOR",             sh_for,                 sh_for_usage,
     "Execute a series of commands in a loop"},
    {"Help",            sh_help,                sh_help_usage,
     "Print this list OR usage for a specific command" },
    {"HISTory",         sh_history,             sh_history_usage,
     "List command history"},
#if defined(BCM_ESW_SUPPORT) && defined(BCM_HLA_SUPPORT)
    {"HLA",             hla_cmd,                hla_usage,
     "Command to load Hardware feature license file"},
#endif
#if !defined(NO_SAL_APPL) && !defined(NO_UNIX_ACCESS)
#endif
    {"IF",              sh_if,                  sh_if_usage,
     "Conditionally execute commands"},
#ifndef NO_SAL_APPL
#ifndef SAL_CONFIG_FILE_DISABLE
#ifndef NO_FILEIO
     {"IMPORT",         sh_import,              sh_import_usage,
      "Load commands from a file"},
#endif
#endif
#endif
#ifdef BCM_IPROC_SUPPORT
    {"IPROCRead",       iprocread_cmd,          iprocread_usage,
     "Read from IPROC Area"},
    {"IPROCWrite",      iprocwrite_cmd,         iprocwrite_usage,
     "Write to IPROC Area"},
#endif
    {"JOBS",            sh_jobs,                sh_jobs_usage,
     "List current background jobs"},
    {"KILL",            sh_kill,                sh_kill_usage,
     "Terminate a background job"},
/*
 * Change 'LED' clause: This was for BCM_PETRA_SUPPORT (Arad and Jericho
 * family)and was changed to BCM_SAND_SUPPORT (Jericho-2 family, DNX) to
 * enable using a similar configuration file (For DNX: dnx.doc)
 * In the future, this may be changed as per actual DNX board details.
 */
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(BCM_LTSW_SUPPORT)
    {"LED",             ledproc_cmd,            ledproc_usage,
     "Control/Load LED processor"},
#endif
    {"LOCal",           var_local,              var_local_usage,
     "Create/Delete a variable in the local scope"},
#if !defined(NO_SAL_APPL) && !defined(NO_UNIX_ACCESS)
    {"LOG",             sh_log,                 sh_log_usage,
     "Enable/Disable logging and set log file"},
#endif
    {"LOOP",            sh_loop,                sh_loop_usage,
     "Execute a series of commands in a loop"},
#if !defined(NO_SAL_APPL) && !defined(NO_UNIX_ACCESS)
    {"LS",              sh_ls,                  sh_ls_usage,
     "List current directory"},
#endif
#ifdef BCM_CMICX_SUPPORT
    {"M0",             m0_cmd,                m0_usage,          "M0 commands for led/linkscan fw"},
#endif
#ifdef INCLUDE_MACSEC
    {"MacSec",           sh_macsec,               sh_macsec_usage,
     "Set/Get MACSEC register/table and call MACSEC APIs"},
#endif /* INCLUDE_MACSEC */
#ifdef BCM_CMICM_SUPPORT
    {"MCSCmd",          mcscmd_cmd,             mcscmd_usage,           "Execute cmd on uC"},
    {"MCSDump",         mcsdump_cmd,            mcsdump_usage,          "Create MCS dumpfile"},
    {"MCSLoad",         mcsload_cmd,            mcsload_usage,          "Load hexfile to MCS memory"},
    {"MCSMsg",          mcsmsg_cmd,             mcsmsg_usage,           "Start/stop messaging with MCs"},
    {"MCSStatus",       mcsstatus_cmd,          mcsstatus_usage,        "Show MCS fault status"},
    {"MCSTimeStamp",    mcstimestamp_cmd,       mcstimestamp_usage,     "Print MCS timestamp data"},
#endif
#if !defined(NO_SAL_APPL) && !defined(NO_UNIX_ACCESS)
    {"MKDIR",           sh_mkdir,               sh_mkdir_usage,
     "Make a directory"},
#endif
    {"MODE",            cmd_mode,               shell_mode_usage,
     "Set shell mode" },
#if !defined(NO_SAL_APPL) && !defined(NO_UNIX_ACCESS)
    {"MORe",            sh_more,                sh_more_usage,
     "Copy a file to the console"},
    {"MoVe",            sh_rename,              sh_rename_usage,
     "Rename a file on a file system"},
#ifdef INCLUDE_NFS
    {"NFS",             cmd_nfs,                cmd_nfs_usage,
     "NFS Control"},
#endif
#endif /* !defined(NO_SAL_APPL) && !defined(NO_UNIX_ACCESS) */
    {"NOEcho",          sh_noecho,              sh_noecho_usage,
     "Ignore command line"},
#ifndef NO_SAL_APPL
    {"Pause",           sh_pause,               sh_pause_usage,
     "Pause command processing and wait for input"},
#endif
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) || (defined(BCM_ESW_SUPPORT) && defined(PCIEPHY_DIAG_SUPPORT))
    {"PCIEPHY",         cmd_pciephy,            pciephy_usage,
     "PCIe PHY speicific utilities"},
#endif
#ifndef NO_SAL_APPL
#endif
#ifdef INCLUDE_LIB_PIONEER_HOST
    {"PIOneer",         cmd_pioneer,            cmd_pioneer_usage,
     "Run a pioneer host command"},
#endif
#ifdef PORTMOD_DIAG
    {"PortMod",          cmd_portmod_diag,      portmod_diag_usage,    "Portmod Diagnostics"},
#endif
    {"PRINTENV",        var_display,            var_display_usage,
     "Display current variable list"},
    {"PROBE",           sh_probe,               sh_probe_usage,
     "Probe for available SOC units"},
#ifdef BCM_BPROF_STATS
    {"PROF",             sh_prof,              sh_prof_usage,           "Profile statistics info "},
#endif
#ifdef BCM_CMICM_SUPPORT
    {"PSCAN",           pscan_cmd,              pscan_usage,
     "Control uKernel port scanning."},
#endif
#ifdef INCLUDE_PTP
    {"PTP",             cmd_ptp,                cmd_ptp_usage,           "PTP stack configuration"},
#endif
#if defined(SOC_UKERNEL_DEBUG)
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
    {"UCDBG",           cmd_cmic_ucdebug,       cmd_cmic_ucdebug_usage,  "Ukernel Debug"},
#endif
#endif
#if !defined(NO_SAL_APPL) && !defined(NO_UNIX_ACCESS)
    {"PWD",             sh_pwd,                 sh_pwd_usage,
     "Print platform dependent working directory"},
#endif
#ifndef NO_UNIX_ACCESS
    {"QUIT",            sh_exit,                sh_exit_usage,
     "@exit"},
#endif
#ifndef NO_SAL_APPL
    {"RCCache",         sh_rccache,             sh_rccache_usage,
     "Save contents of an rc file in memory"},
    {"RCLoad",          sh_rcload,              sh_rcload_usage,
     "Load commands from a file"},
#ifndef NO_UNIX_ACCESS
    {"REBOOT",          sh_reboot,              sh_reboot_usage,
     "Reboot the processor"},
    {"REName",          sh_rename,              sh_rename_usage,
     "@move" },
    {"RESET",           sh_reboot,              sh_reboot_usage,
     "@reboot"},
    {"RM",              sh_remove,              sh_remove_usage,
     "Remove a file from a file system"},
    {"RMDIR",           sh_rmdir,               sh_rmdir_usage,
     "Remove a directory"},
#endif
#endif  /* NO_SAL_APPL */

#if defined(BCM_RPC_SUPPORT)
    {"RPC",     cmd_rpc,        cmd_rpc_usage,
     "Control BCM API RPC daemon."},
#endif
#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
    {"SalProfile",      cmd_sal_profile,        cmd_sal_profile_usage,
     "Displays current SAL resource usage"},
#endif /* INCLUDE_BCM_SAL_PROFILE */
#endif /* BROADCOM_DEBUG */
#ifdef BCM_SAT_SUPPORT
    {"SAT",             cmd_sat,                cmd_sat_usage,
     "Service activation test"},
#endif /* BCM_SAT_SUPPORT */
#if !defined(NO_SAL_APPL) && !defined(NO_UNIX_ACCESS)
    {"SAVE",            sh_write,               sh_write_usage,
     "Write data to a file"},
#endif
    {"SET",             sh_set,                 sh_set_usage,
     "Set various configuration options"},
    {"SETENV",          var_export,             var_export_usage,
     "Create/Delete a variable in the global scope" },

#if !defined(NO_SAL_APPL) && !defined(NO_UNIX_ACCESS)
    {"SHell",           sh_shell,               sh_shell_usage,
     "Invoke a system dependent shell"},
#endif /* !defined(NO_SAL_APPL) && !defined(NO_UNIX_ACCESS) */

    {"SLeep",           sh_sleep,               sh_sleep_usage,
     "Suspend the CLI task for specified amount of time"},
#ifndef NO_SAL_APPL
#ifdef INCLUDE_TCL
    {"TCL",             cmd_tcl,                tcl_usage,
     "Run a TCL shell or TCL script"},
#endif
#endif
#ifdef INCLUDE_PTP
    {"TDPLL",           cmd_tdpll,              cmd_tdpll_usage,
     "T-DPLL configuration and management"},
#endif
    {"TIME",            sh_time,                sh_time_usage,
     "Time the execution of one or more commands"},
#ifdef BSL_TRACE_INCLUDE
    {"TRaCe",           sh_trace,               sh_trace_usage,
     "Control console options"},
#endif
    {"Version",         sh_version,             sh_version_usage,
     "Print version and build information"},
};

#else /* BCM_SHARED_LIB_SDK */

cmd_t bcm_cmd_common[] = {
    {"?",               sh_help_short,          sh_help_short_usage,
     "Display list of commands"},
    {"??",              sh_help,                sh_help_usage,
     "@help" },
    {"cpu_i2c", cmd_sand_cpu_i2c, cmd_sand_cpu_i2c_usage, "Read/Write I2C via cpu"},
    {"CONFig",          sh_config,              sh_config_usage,
     "Configure Management interface"},
    {"DeBug",           sh_debug,               sh_debug_usage,
     "Enable/Disable debug output"},
    {"Echo",            sh_echo,                sh_echo_usage,
     "Echo command line"},
    {"EXIT",            sh_exit,                sh_exit_usage,
     "Exit the current shell (and possibly reset)" },
    {"EXPR",            sh_expr,                sh_expr_usage,
     "Evaluate infix expression" },
    {"IF",              sh_if,                  sh_if_usage,
     "Conditionally execute commands"},
    {"IMPORT",         sh_import,              sh_import_usage,
      "Load commands from a file"},
    {"LED",            ledproc_cmd,            ledproc_usage,
     "Control/Load LED processor"},
    {"LOCal",          var_local,              var_local_usage,
     "Create/Delete a variable in the local scope"},
    {"M0",             m0_cmd,                 m0_usage,          "M0 commands for led/linkscan fw"},
    {"SET",            sh_set,                 sh_set_usage,
     "Set various configuration options"},
};

#endif /* BCM_SHARED_LIB_SDK */


int bcm_cmd_common_cnt = COUNTOF(bcm_cmd_common);

#if defined(BCM_ESW_SUPPORT)
/* This is the list of commands that can be executed only in ESW mode */
static cmd_t bcm_esw_cmd_list[] = {
/*                                                                                                                      */
/*    Command   |       Function            |   Usage String      |     Description (Short help)                        */
/*  ------------+---------------------------+---------------------+---------------------------------------------------- */
#ifdef INCLUDE_I2C
    {"ADC",             cmd_adc,                cmd_adc_usage,          "Show MAX127 A/D Conversions" },
#endif
    {"AGE",             cmd_esw_age,            cmd_age_esw_usage,           "Set ESW hardware age timer" },
#if defined(ALPM_ENABLE)
    {"ALPM",            cmd_esw_alpm,           cmd_esw_alpm_usage,     "ALPM trouble shooting" },
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    {"ASF",             if_esw_asf,             if_esw_asf_usage,       "ASF (cut-through) Mode" },
#endif
    {"Attach",          sh_attach,              sh_attach_usage,        "Attach SOC device(s)" },
    {"Auth",            cmd_esw_auth,           auth_esw_cmd_usage,     "Port-based network access control"},
#ifdef INCLUDE_AVS
    {"AVS",             cmd_avs,                cmd_avs_usage,          "AVS (Adaptive Voltage Scaling)" },
#endif
#ifdef INCLUDE_I2C
    {"BaseBoard",       cmd_bb,                 cmd_bb_usage,           "Configure baseboard system parameters."},
#endif
#ifdef BCM_KATANA_SUPPORT
#if defined(INCLUDE_L3)
#if defined(INCLUDE_BFD)
    {"BFD",             cmd_esw_bfd,            cmd_esw_bfd_usage,     "Manage BFD endpoints"},
#endif
#endif
#endif
    {"BIST",            cmd_bist,               bist_usage,             "Run on-chip memory built-in self tests"},
    {"BKPmon",          dbg_bkpmon,             bkpmon_usage,           ".Monitor incoming BKP Discard Messages" },
    {"BPDU",            if_esw_bpdu,            if_esw_bpdu_usage,          "Manage BPDU addresses" },
    {"BTiMeout",        cmd_btimeout,           btimeout_usage,         "Set BIST operation timeout in microseconds" },
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    {"BUFfer",          cmd_buffer,             cmd_buffer_usage,       "MMU config" },
#endif /* BCM_TRIDENT_SUPPORT || BCM_TRIUMPH3_SUPPORT */
    {"CABLEdiag",       cmd_esw_cablediag,      cmd_esw_cablediag_usage,"Run Cable Diagnotics" },
    {"CACHE",           mem_esw_cache,          mem_esw_cache_usage,    "Turn on/off software caching of tables" },
#if defined(CANCUN_SUPPORT)
#ifndef NO_SAL_APPL
    {"CanCun",          cmd_cancun,             cmd_cancun_usage,       "Cancun file and status operations"},
#endif /* NO_SAL_APPL */
#endif /* CANCUN_SUPPORT */
    {"CFAPINIT",        mem_cfapinit,           cfapinit_usage,         ".Init MMU cell free address table" },
#if defined(MBZ)
    {"CFMFailover",     if_cfm_failover,        if_cfm_failover_usage,  "Activate CFM failover" },
#endif
    {"CHecK",           cmd_check,              check_usage,            "Check a sorted memory table" },
    {"CLEAR",           cmd_esw_clear,          cmd_esw_clear_usage,    "Clear a memory table or counters" },
#ifdef INCLUDE_I2C
    {"CLOCKS",          cmd_clk,                cmd_clk_usage,          "Set core clock frequency." },
#endif
    {"COLOR",           cmd_color,              cmd_color_usage,        "Manage packet color"},
    {"COMBO",           if_esw_combo,           if_esw_combo_usage,     "Control combination copper/fiber ports"},
    {"COS",             cmd_esw_cos,            cmd_esw_cos_usage,      "Manage classes of service"},
    {"CounTeR",         cmd_esw_counter,        cmd_esw_counter_usage,  "Enable/disable counter collection"},
#if defined (BCM_FIRELIGHT_SUPPORT)
    {"CoupledMemWrite", cmd_esw_coupled_mem_write, cmd_esw_coupled_mem_write_usage, "Write entry(s) into coupled memories" },
#endif
    {"CustomSTAT",         cmd_esw_custom_stat,        cmd_esw_custom_stat_usage,  "Enable/disable counter collection"},
#ifdef INCLUDE_I2C
    {"DAC",             cmd_dac,                cmd_dac_usage,          "Set DAC register"},
#endif
#ifdef BCM_DDR3_SUPPORT
    {"DDRMemRead",      cmd_ddr_mem_read,       cmd_ddr_mem_read_usage,  "Indirect reads to DDR"},
    {"DDRMemWrite",     cmd_ddr_mem_write,      cmd_ddr_mem_write_usage, "Indirect writes to DDR"},
    {"DDRPhyRead",      cmd_ddr_phy_read,       cmd_ddr_phy_read_usage,  "Read DDR40 phy registers"},
    {"DDRPhyTune",      cmd_ddr_phy_tune,       cmd_ddr_phy_tune_usage,  "Tune DDR40 phy registers"},
    {"DDRPhyWrite",     cmd_ddr_phy_write,      cmd_ddr_phy_write_usage, "Write DDR40 phy registers"},
#endif
    {"DELete",          mem_delete,             delete_usage,           "Delete entry by key from a sorted table" },
    {"DETach",          sh_detach,              sh_detach_usage,        "Detach SOC device(s)" },
    {"DMA",             cmd_esw_dma,            cmd_esw_dma_usage,      "DMA Facilities Interface" },
    {"DmaRomTest",      cmd_drt,                cmd_drt_usage,          "Simple test of the SOC DMA ROM API" },
    {"DMIRror",         if_dmirror,             if_dmirror_usage,       "Manage directed port mirroring"},
#ifdef BCM_CMICM_SUPPORT
    {"DPLL",            dpll_cmd,               dpll_usage,             "DPLL operations on SPI bus"},
#endif
    {"DSCP",            if_esw_dscp,            if_esw_dscp_usage,      "Map Diffserv Code Points" },
    {"DTAG",            if_esw_dtag,            if_esw_dtag_usage,      "Double Tagging" },
    {"Dump",            cmd_esw_dump,           cmd_esw_dump_usage,     "Dump an address space or registers" },
    {"EditReg",         cmd_esw_reg_edit,       cmd_esw_reg_edit_usage, "Edit each field of SOC internal register"},
    {"EGRess",          if_egress,              if_egress_usage,        "Manage source-based egress enabling"},
    {"EthernetAV",      cmd_esw_eav,            cmd_esw_eav_usage,      "Set/Display the Ethernet AV characteristics"},

#ifdef INCLUDE_EXAMPLES
    {"EXample",         cmd_example_exec,                cmd_example_exec_usage,   "Execute example codes "},
#endif

#ifdef BCM_TRIUMPH_SUPPORT
#ifndef NO_MEMTUNE
    {"EXTernalTuning",  cmd_extt,               cmd_extt_usage,         "External memory automatic tuning" },
    {"EXTernalTuning2", cmd_extt2,              cmd_extt2_usage,    "External memory automatic tuning 2" },
    {"EXTernalTuningSum",  cmd_extts,       cmd_extts_usage,        "External memory automatic tuning (summary)" },
#endif /* NO_MEMTUNE */
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef INCLUDE_FCMAP
    {"FCOE",             if_esw_fcoe,             if_esw_fcoe_usage, "FCoE NPV Service (Debug Only)"},
#endif

#ifdef BCM_FIELD_SUPPORT
    {"FieldProcessor",  if_esw_field_proc,       if_esw_field_proc_usage,"Manage Field Processor"},
#endif /* BCM_FIELD_SUPPORT */
#ifdef BCM_XGS5_SWITCH_PORT_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
    {"FLEXport",        cmd_if_flexport,        cmd_if_flexport_usage_th3,  "Flexport"},
#elif defined(BCM_TRIDENT3_SUPPORT)
    {"FLEXport",        cmd_if_flexport,        cmd_if_flexport_usage_td3,  "Flexport"},
#else
    {"FLEXport",        cmd_if_flexport,        cmd_if_flexport_usage,  "Flexport"},
#endif
#endif /* BCM_XGS5_SWITCH_PORT_SUPPORT */
#if defined(CANCUN_SUPPORT)
#ifndef NO_SAL_APPL
    {"FlowDb",          cmd_flow_db,            cmd_flow_db_usage,      "Flow Database for flexible tables"},
#endif /* NO_SAL_APPL */
#endif /* CANCUN_SUPPORT */
    {"FlowTracker",     cmd_ft,                 cmd_ft_usage,           "Flowtracker commands"},
    {"Qcm",             cmd_qcm,                cmd_qcm_usage,          "QCM commands"},
#ifdef BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT
    {"FlowTrackerMON",        cmd_ftmon,        cmd_ftmon_usage,  "FlowtracketMon"},
    {"FlowTrackerRemoteMON",  cmd_ft_rmon_command,    cmd_ft_rmon_usage, "Monitor ports for packets"},
#endif /*  BCM_FLOWTRACKER_EXPORT_FIFO_SUPPORT */
    {"Getreg",          cmd_esw_reg_get,        cmd_esw_reg_get_usage,  "Get register" },
#ifdef BCM_KATANA_SUPPORT
    {"GlobalMeter",     cmd_esw_policer_global_meter, cmd_esw_policer_global_meter_usage,  "Global meter - policer commands"},
#endif
    {"GPORT",           if_esw_gport,           if_esw_gport_usage,     "Get a GPORT id" },
#ifdef BCM_XGS_SUPPORT
    {"H2HIGIG",         if_h2higig,             if_h2higig_usage,       "Convert hex words to higig info" },
#ifdef BCM_HIGIG2_SUPPORT
    {"H2HIGIG2",        if_h2higig2,            if_h2higig2_usage,      "Convert hex words to higig2 info" },
#endif
#endif
#ifdef BCM_XGS_SWITCH_SUPPORT
    {"HASH",            cmd_hash,               hash_usage,             "Get or set hardware hash modes" },
#endif
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_ENDURO_SUPPORT) || \
    defined(BCM_GREYHOUND2_SUPPORT)
    {"HashDestination", cmd_hash_destination,   hash_destination_usage, "Display Hash Destination" },
#endif
#ifdef INCLUDE_I2C
    {"HClksel",         cmd_hclksel,           cmd_hclksel_usage,       "Set I2C HClk (MUX for clock-chip-selects)" },
#endif
    {"HeaderMode",      cmd_hdr_mode,           cmd_hdr_mode_usage,     "Get or set packet tx header mode" },
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    {"HSP",             cmd_hsp,                cmd_hsp_usage,          "MMU HSP hierarchy" },
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef INCLUDE_I2C
    {"I2C",             cmd_i2c,                cmd_i2c_usage,          "Inter-Integrated Circuit (I2C) Bus commands"},
#endif
#ifdef BCM_HERCULES_SUPPORT
    {"IBDump",          if_ibdump,              if_ibdump_usage,        "Display packets pending in the Ingress Buffer" },
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    {"IbodSync",        cmd_ibod_sync,          ibod_sync_usage,        "Enable/Disable IBOD sync process" },
#endif /* BCM_TRIUHMPH3_SUPPORT */
    {"INIT",            sh_init,                sh_init_usage,          "Initialize SOC and S/W"},
    {"Insert",          mem_insert,             insert_usage,           "Insert into a sorted table" },
    {"INTR",            cmd_intr,               cmd_intr_usage,         "Enable, disable, show interrupts" },
    {"IPFIX",           cmd_ipfix,              cmd_ipfix_usage,        "IPFIX"},
    {"IPG",             if_esw_ipg,             if_esw_ipg_usage,       "Set default IPG values for ports" },
#ifdef INCLUDE_L3
#if defined(BCM_XGS_SWITCH_SUPPORT)
    {"IPMC",            if_ipmc,                if_ipmc_usage,          "Manage IPMC (IP Multicast) addresses"},
#endif /* BCM_XGS_SWITCH_SUPPORT */
#endif /* INCLUDE_L3 */
#ifdef INCLUDE_KNET
    {"KNETctrl",        cmd_knet_ctrl,          cmd_knet_ctrl_usage,    "Manage kernel network functions"},
#endif
    {"L2",              if_esw_l2,              if_esw_l2_usage,         "Manage L2 (MAC) addresses"},
#ifdef BCM_TOMAHAWK3_SUPPORT
    {"L2LEARN",          cmd_l2learn,           l2learn_usage,          "Control Learn Thread Execution" },
#endif
#ifdef BCM_XGS_SWITCH_SUPPORT
    {"L2MODE",          cmd_l2mode,             l2mode_usage,           "Change ARL handling mode" },
#endif
#ifdef INCLUDE_L3
    {"L3",              if_l3,                  if_l3_usage,            "Manage L3 (IP) addresses"},
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    {"LATency",			if_esw_latency,			if_esw_latency_usage,	"Switch Latency Bypass Diagnostics"},
#endif
#ifdef INCLUDE_I2C
    {"LCDMSG",          cmd_lcdmsg,             cmd_lcdmsg_usage,       "Print message on Matrix Orbital LCD display (via I2C)"},
#endif
    {"LINKscan",        if_esw_linkscan,        if_esw_linkscan_usage,  "Configure/Display link scanning" },
    {"LISTmem",         cmd_esw_mem_list,       cmd_esw_mem_list_usage, "List the entry format for a given table" },
    {"Listreg",         cmd_esw_reg_list,       cmd_esw_reg_list_usage,  "List register fields"},
#ifdef BCM_HERCULES_SUPPORT
    {"LLAINIT",         mem_llainit,            llainit_usage,          ".Init MMU LLA pointers" },
#endif
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA_SUPPORT)
    {"LLS",             cmd_lls,                cmd_lls_usage,          "MMU LLS hierarchy" },
#endif /* (BCM_TRIUMPH3_SUPPORT | BCM_KATANA_SUPPORT) */
    {"LOOKup",          mem_lookup,             lookup_usage,           "Look up a table entry"},
    {"MCAST",           if_esw_mcast,           if_esw_mcast_usage,     "Manage multicast table"},
    {"MemFirst",        mem_first,              memfirst_usage,          "Displays first valid memory" },
    {"MemNext",         mem_next,               memnext_usage,          "Displays next valid memory" },
#ifdef INCLUDE_MEM_SCAN
    {"MemSCAN",         mem_scan,               memscan_usage,          "Turn on/off software memory error scanning" },
#endif
    {"MemWatch",        mem_watch,              memwatch_usage,          "Turn on/off memory snooping" },
#if defined(INCLUDE_L3) && defined(BCM_TRIUMPH2_SUPPORT)
    {"MIM",             if_tr2_mim,             if_tr2_mim_usage,
"Manage XGS4 Mac-in-MAC"},
#endif
    {"MIRror",          if_esw_mirror,          if_esw_mirror_usage,     "Manage port mirroring"},
#ifdef BCM_HERCULES_SUPPORT
    {"MmuConFiG",       if_mmu_cfg,             if_mmu_cfg_usage,       "Configure MMU mode" },
#endif
    {"MmuDeBuG",        mem_mmudebug,           mmudebug_usage,         ".Place SOC MMU in debug mode" },
    {"MODify",          cmd_esw_mem_modify,     cmd_esw_mem_modify_usage,"Modify table entry by field names" },
#ifdef BCM_XGS_SUPPORT
    {"ModMap",          cmd_modmap,             modmap_cmd_usage,       "MODID Remapping" },
#endif
    {"Modreg",          cmd_esw_reg_mod,        cmd_esw_reg_mod_usage,  "Read/modify/write register"},
#if defined(INCLUDE_L3) && defined(BCM_TRIUMPH_SUPPORT)
    {"MPLS",            if_tr_mpls,             if_tr_mpls_usage,
"Manage XGS4 MPLS"},
#endif
#ifdef BCM_CMICM_SUPPORT
    {"MSPI",            mspi_cmd,               mspi_usage,             "MasterSPI Read / Write"},
#endif
    {"MTiMeout",        cmd_mtimeout,           mtimeout_usage,         "Set MIIM operation timeout in usec" },
    {"MultiCast",       cmd_multicast,          cmd_multicast_usage,    "Manage multicast operation" },
#ifdef INCLUDE_I2C
    {"MUXsel",          cmd_muxsel,             cmd_muxsel_usage,       "Set I2C LPT state (MUX for clock-chip-selects)" },
#endif
#ifdef INCLUDE_I2C
    {"NVram",           cmd_nvram,              cmd_nvram_usage,        "Manipulate Nonvolatile memory"},
#endif
    {"OAM",             cmd_esw_oam,            cmd_esw_oam_usage,     "Manage OAM groups and endpoints"},
    {"PacketWatcher",   pw_command,             pw_usage,               "Monitor ports for packets"},
#if defined(MOUSSE) || defined(BMW) || defined(GTO)
    {"PANIC",           cmd_panic,              panic_usage,            "Set system panic mode"},
#endif
    {"PBMP",            cmd_esw_pbmp,           cmd_esw_pbmp_usage,     "Convert port bitmap string to hex"},
#ifdef INCLUDE_I2C
    {"PCIE",           cmd_pcie,              cmd_pcie_usage,        "R/W PCIE core registers"},
#endif
#ifdef PE_SDK
    {"PEAPP",           cmd_peapp,              cmd_peapp_usage,        "Port Extender Lib Commands"},
#endif /* PE_SDK */
#ifdef PE_MODE_CMD
    {"PEMODE",          cmd_pemode,             cmd_pemode_usage,       "Port Extender Mode"},
#endif /* PE_MODE_CMD */
    {"PHY",             if_esw_phy,             if_esw_phy_usage,       "Set/Display phy characteristics"},
#ifdef INCLUDE_I2C
#ifdef SHADOW_SVK
    {"PIO",             cmd_pio,                cmd_pio_usage,          "Set/Display parallel port."},
#endif
#endif
    {"PKTIO",           cmd_esw_pktio,          cmd_esw_pktio_usage,    "Set/Display TX/PacketWatcher with streamlined pktio driver type"},
#ifdef INCLUDE_I2C
    {"POE",             cmd_poe,                cmd_poe_usage,          "Configure PowerOverEthernet controllers."},
    {"POESel",          cmd_poesel,             cmd_poesel_usage,       "Set I2C POE (MUX for poe-chip-selects)" },
#endif
    {"POP",             mem_pop,                pop_usage,              "Pop an entry from a FIFO" },
    {"PORT",            if_esw_port,            if_esw_port_usage,      "Set/Display port characteristics"},
#ifdef BCM_KATANA_SUPPORT
    {"PortPolicer",     if_esw_port_policer,    if_esw_port_policer_usage, "Display port policer status in table"},
#endif
    {"PortRate",        if_esw_port_rate,       if_esw_port_rate_usage, "Set/Display port rate metering characteristics"},
    {"PortSampRate",    if_port_samp_rate,      if_port_samp_rate_usage,"Set/Display sflow port sampling rate"},
    {"PortStat",        if_esw_port_stat,       if_esw_port_stat_usage, "Display port status in table"},
#ifdef INCLUDE_I2C
#ifdef BCM_ESW_SUPPORT
    {"PPDclk",          cmd_ppdclk,             cmd_ppdclk_usage,       "Show PPD clock delay" },
#endif
#endif
    {"PROBE",           sh_probe,               sh_probe_usage,         "Probe for available SOC units"},
    {"PUSH",            mem_push,               push_usage,             "Push an entry onto a FIFO" },
    {"PUTREG",          cmd_esw_reg_set,        cmd_esw_reg_set_usage,  "@setreg" },
    {"PVlan",           if_esw_pvlan,           if_esw_pvlan_usage,     "Port VLAN settings" },
    {"RATE",            cmd_esw_rate,           cmd_esw_rate_usage,     "Manage packet rate controls"},
    {"RateBw",          cmd_bw_rate,            cmd_bw_rate_usage,      "Set/Display port bandwidth rate metering characteristics"},
    {"RegCMp",          reg_cmp,                regcmp_usage,           "Test a register value"},
#ifdef INCLUDE_REGEX
    {"REgex",        cmd_regex,           cmd_regex_usage,     "Regex command" },
#endif
    {"RegWatch",        reg_watch,              regwatch_usage,          "Turn on/off register snooping" },
    {"REMove",          mem_remove,             remove_usage,           "Delete entry by index from a sorted table" },
    {"ResTest",     cmd_sh_restest,         cmd_sh_restest_usage,   "Tests for resource manager"},
    {"RX",              cmd_rx,                 cmd_rx_usage,           ".Receive some number of packets"},
    {"RXCfg",           cmd_esw_rx_cfg,         cmd_esw_rx_cfg_usage,   "Configure RX settings"},
    {"RXInit",          cmd_esw_rx_init,        cmd_esw_rx_init_usage,  "Call bcm_rx_init"},
    {"RXMon",           cmd_esw_rx_mon,         cmd_esw_rx_mon_usage,   "Register an RX handler to dump received packets"},
#if defined(INCLUDE_SIM8051)
    {"S51",             sim8051_ent,            sim8051_usage,          "Enter 8051 simulator" },
#endif
    {"SCHan",           cmd_schan,              schan_usage,            "Send raw S-Channel message, get response" },
    {"SEArch",          mem_search,             search_usage,           "Search a table for a byte pattern" },
#if defined(SER_TR_TEST_SUPPORT)
    {"SER",  cmd_esw_ser,   cmd_esw_ser_usage,     "Performs operations related to Soft Error Recovery"},
#endif /*defined(SER_TR_TEST_SUPPORT)*/
    {"Setreg",          cmd_esw_reg_set,        cmd_esw_reg_set_usage,  "Set register" },
    {"SHOW",            cmd_esw_show,           cmd_esw_show_usage,     "Show information on a subsystem" },
#ifdef PLISIM
    {"SHUTDOWN",        cmd_shutd,              NULL,                   "Tell plisim to shutdown" },
#endif
    {"SOC",             cmd_esw_soc,            cmd_esw_soc_usage,      "Print internal Driver control information"},
    {"SOCRES",          cmd_socres,             socres_usage,           ".SOC reset (SCP channel)" },
#ifdef BCM_TRIUMPH_SUPPORT
    {"SRAM",            cmd_sram,               cmd_sram_usage,         "External DDR2_SRAM test control" },
#endif
#ifdef BCM_SRAM_SCAN_SUPPORT
    {"SramSCAN",        sram_scan,              sramscan_usage,         "Turn on/off software SRAM error scanning" },
#endif
    {"STACKMode",       cmd_stk_mode,           cmd_stk_mode_usage,     "Set/get the stack mode"},
    {"StackPortGet",    cmd_stk_port_get,       cmd_stk_port_get_usage,  "Get stacking characteristics of a port"},
    {"StackPortSet",    cmd_stk_port_set,       cmd_stk_port_set_usage,  "Set stacking characteristics of a port"},
#ifdef PLISIM
    {"StartSim",        cmd_simstart,           NULL,                   "Tell plisim to activate" },
#endif
    {"STG",             if_esw_stg,             if_esw_stg_usage,       "Manage spanning tree groups" },
    {"STiMeout",        cmd_stimeout,           stimeout_usage,         "Set S-Channel timeout in microseconds" },
    {"STKMode",         cmd_stkmode,            cmd_stkmode_usage,       "Hardware Stacking Mode Control"},
#if defined(TKS_SUPPORT)
    {"StkTask",         tks_stk_task,           tks_stk_task_usage,      "Stack task control" },
#endif
#ifdef SW_AUTONEG_SUPPORT    
    {"SW_AN", if_esw_swAutoneg, if_esw_swAutoneg_usage, "Enable/Disable SW AN Thread"},
#endif    
    {"SwitchControl",   cmd_switch_control,     cmd_switch_control_usage, "General switch control"},
#ifdef INCLUDE_I2C
#ifdef BCM_ESW_SUPPORT
    {"SYnth",           cmd_synth,              cmd_synth_usage,        "Show synthesizer frequency" },
#endif
#endif
#ifdef INCLUDE_TEST
    {"SystemSnake",     cmd_ss,                 cmd_ss_usage,           "Cycle packets through selected system"},
#endif
#ifdef BCM_TRIUMPH_SUPPORT
     {"TCAM",           cmd_tcam,               cmd_tcam_usage,           "TCAM control" },
#endif
#if defined(BCM_ESW_SUPPORT)
    {"TeCHSupport",     command_techsupport,    command_techsupport_usage,  "Collects information required to "
                                                                            "debug a given feature or subfeature" },
#endif
#ifdef INCLUDE_I2C
    {"TEMPerature",     cmd_temperature,        cmd_temperature_usage,  "Show environmental conditions"},
#endif
#ifdef INCLUDE_TEST
    {"TestClear",       test_clear,             test_clear_usage,       "Clear run statisistics for a test"},
    {"TestList",        test_print_list,        test_list_usage,        "List loaded tests and status"},
    {"TestMode",        test_mode,              test_mode_usage,        "Set global test run modes"},
    {"TestParameters",  test_parameters,        test_parameters_usage,  "Set test Parameters"},
    {"TestRun",         test_run,               test_run_usage,         "Run a specific or selected tests"},
    {"TestSelect",      test_select,            test_select_usage,      "Select tests for running"},
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    {"TMCheck",          cmd_check_tm_settings,  cmd_check_tm_usage,       "TM config check" },
    {"TMDumpMem",        cmd_tm_dump_mem,        cmd_tm_dump_mem_usage,        "TM dump memories" },
    {"TMShow",           cmd_tm_show_hw_config,  cmd_tm_show_usage,        "TM config show"},
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef INCLUDE_PTP
#ifdef PTP_KEYSTONE_STACK
    {"TOPLoad",         cmd_topload,            cmd_topload_usage,      "Load hexfile to ToP memory for PTP"},
    {"TOPInfo",         cmd_topinfo,            cmd_topinfo_usage,      "Print information about ToP Firmware"},
#endif
#endif
    {"TRUNK",           if_esw_trunk,           if_esw_trunk_usage,     "Manage port aggregation"},
#ifdef BCM_HERCULES15_SUPPORT
    {"TrunkPool",       cmd_trunkpool,          trunkpool_cmd_usage,    "Trunk pool table configuration"},
#endif
    {"TX",              cmd_esw_tx,             cmd_esw_tx_usage,       "Transmit one or more packets"},
    {"TXBeacon",        cmd_esw_txbeacon,       cmd_esw_txbeacon_usage, "txbeacon tests"},
    {"TXCount",         cmd_esw_tx_count,       cmd_esw_tx_count_usage, "Print current TX statistics"},
#ifdef PLISIM
    {"TXEN",            cmd_txen,               NULL,                   "Enable packet transmission" },
#endif
    {"TXSTArt",         cmd_esw_tx_start,       cmd_esw_tx_usage,       "Transmit one or more packets in background"},
    {"TXSTOp",          cmd_esw_tx_stop,        cmd_esw_tx_stop_usage,  "Terminate a previous \"txstart\" command"},
    {"VLAN",            if_esw_vlan,            if_esw_vlan_usage,      "Manage virtual LANs"},
    {"WARMBOOT",        sh_warmboot,            sh_warmboot_usage,      "Optionally boot warm"},
#if defined(INCLUDE_L3) && defined(BCM_TRIUMPH2_SUPPORT)
    {"WLAN",            if_tr2_wlan,            if_tr2_wlan_usage,
"Manage XGS4 WLAN"},
#endif
    {"Write",           cmd_esw_mem_write,      cmd_esw_mem_write_usage, "Write entry(s) into a table" },
#ifdef BCM_XGS3_SWITCH_SUPPORT
    {"XAUI",            cmd_xaui,                cmd_xaui_usage,         "Run XAUI BERT on specified port pair" },
#endif
#ifdef INCLUDE_I2C
    {"XClocks",         cmd_xclk,               cmd_xclk_usage,         "Configure clocks for PCI, SDRAM, Core clock"},
#endif
#ifdef INCLUDE_I2C
    {"XPoe",            cmd_xpoe,               cmd_xpoe_usage,         "Communication with PD63000 PowerOverEthernet MCU."},
#endif
#ifdef BCM_HERCULES_SUPPORT
    {"XQDump",          if_xqdump,              if_xqdump_usage,        "Display packets pending in the XQ" },
    {"XQErr",           if_xqerr,               if_xqerr_usage,         "Inject bit errors into packets pending in XQ" },
#endif
#ifdef BCM_EASY_RELOAD_SUPPORT
    {"XXReload",        cmd_xxreload,           cmd_xxreload_usage,     "\"Easy\" Reload control" },
#endif
};

int bcm_esw_cmd_cnt = COUNTOF(bcm_esw_cmd_list);
#endif

#if defined(BCM_LTSW_SUPPORT)
/* This is the list of commands that can be executed only in LTSW mode */
static cmd_t bcm_ltsw_cmd_list[] = {
    {"ALPM",            cmd_ltsw_alpm,          cmd_ltsw_alpm_usage,        "ALPM info" },
    {"ASF",             if_ltsw_asf,            if_ltsw_asf_usage,          "ASF (cut-through) Mode" },
    {"BcmltSHell",      cmd_bcmlt_shell,        cmd_bcmlt_shell_usage,      "Run bcm LT shell"},
    {"CLEAR",           cmd_ltsw_clear,         cmd_ltsw_clear_usage,       "Clear a memory table or counters"},
    {"COS",             cmd_ltsw_cos,           cmd_ltsw_cos_usage,         "Manage classes of service"},
    {"CounTeR",         cmd_ltsw_counter,       cmd_ltsw_counter_usage,     "Enable/disable counter collection"},
    {"CustomSTAT",      cmd_ltsw_custom_stat,   cmd_ltsw_custom_stat_usage, "Enable/disable debug counter collection"},
    {"DebugSHell",      cmd_sdklt_debug_shell,  cmd_sdklt_debug_shell_usage,"Run SDKLT debug shell"},
    {"DETach",          sh_detach,              sh_detach_usage,            "Detach SOC device(s)" },
    {"Dump",            cmd_ltsw_dump,          cmd_ltsw_dump_usage,        "Dump information" },
    {"EventSTAT",       cmd_ltsw_event_stat,    cmd_ltsw_event_stat_usage,  "Display drop/trace event counters" },
    {"FieldProcessor",  if_ltsw_field_proc,     if_ltsw_field_proc_usage,   "Display Field Processor information."},
    {"FlexCTR",         cmd_ltsw_flexctr,       cmd_ltsw_flexctr_usage,     "Manage flex counter"},
    {"FlexDigest",      cmd_ltsw_flexdigest,    cmd_ltsw_flexdigest_usage,  "Manage Flex Digest"},
    {"FLEXport",        cmd_ltsw_flexport,      cmd_ltsw_flexport_usage,    "Flexport"},
    {"FlowDb",          cmd_ltsw_flow_db,       cmd_ltsw_flow_db_usage,     "FlowDB info"},
    {"HA",              cmd_ltsw_ha,            cmd_ltsw_ha_usage,          "HA memory usage and associate information."},
    {"INIT",            cmd_ltsw_init,          cmd_ltsw_init_usage,        "Initialize SOC and S/W"},
    {"IPMC",            cmd_ltsw_ipmc,          cmd_ltsw_ipmc_usage,        "Manage IPMC (IP Multicast) addresses"},
    {"L2",              cmd_ltsw_l2,            cmd_ltsw_l2_usage,          "Manage L2 (MAC) addresses"},
    {"L3",              cmd_ltsw_l3,            cmd_ltsw_l3_usage,          "Manage L3 (IP) addresses"},
    {"LINKscan",        if_ltsw_linkscan,       if_ltsw_linkscan_usage,     "Configure/Display link scanning" },
    {"MemSCAN",         cmd_ltsw_mem_scan,      cmd_ltsw_mem_scan_usage,    "Turn on/off software memory error scanning" },
    {"MIRror",          cmd_ltsw_mirror,        cmd_ltsw_mirror_usage,      "Manage port mirroring"},
    {"MPLS",            cmd_ltsw_mpls,          cmd_ltsw_mpls_usage,        "Manage MPLS"},
    {"MultiCast",       cmd_ltsw_multicast,     cmd_ltsw_multicast_usage,   "Manage multicast operation" },
    {"PacketWatcher",   cmd_ltsw_pcktwatch,     cmd_ltsw_pcktwatch_usage,   "Monitor ports for packets"},
    {"PBMP",            cmd_ltsw_pbmp,          cmd_ltsw_pbmp_usage,        "Convert port bitmap string to hex"},
    {"PORT",            cmd_ltsw_port,          cmd_ltsw_port_usage,        "Set/Display port characteristics"},
    {"PortStat",        cmd_ltsw_port_stat,     cmd_ltsw_port_stat_usage,   "Display port status in table"},
    {"PVlan",           cmd_ltsw_pvlan,         cmd_ltsw_pvlan_usage,       "Port VLAN settings" },
    {"RATE",            cmd_ltsw_rate,          cmd_ltsw_rate_usage,        "Manage packet rate controls"},
    {"RATEBW",          cmd_ltsw_ratebw,        cmd_ltsw_ratebw_usage,      "Set/Display port bandwidth rate metering characteristics"},
    {"SER",             cmd_ltsw_ser,           cmd_ltsw_ser_usage,         "Performs operations related to Soft Error Recovery"},
    {"SHOW",            cmd_ltsw_show,          cmd_ltsw_show_usage,        "Show information on a subsystem" },
    {"SramSCAN",        cmd_ltsw_sram_scan,     cmd_ltsw_sram_scan_usage,   "Turn on/off software SRAM error scanning" },
    {"StackPortGet",    cmd_ltsw_stk_port_get,  cmd_ltsw_stk_port_get_usage,"Get stacking characteristics of a port"},
    {"StackPortSet",    cmd_ltsw_stk_port_set,  cmd_ltsw_stk_port_set_usage,"Set stacking characteristics of a port"},
    {"STG",             cmd_ltsw_stg,           cmd_ltsw_stg_usage,          "Manage spanning tree groups" },
    {"STKMode",         cmd_ltsw_stkmode,       cmd_ltsw_stkmode_usage,     "Hardware Stacking Mode Control"},
    {"SwitchControl",   cmd_ltsw_switch_control,cmd_ltsw_switch_control_usage, "General switch control"},
    {"TRace",           cmd_ltsw_trace,         cmd_ltsw_trace_usage,       "Packet trace"},
    {"TRUNK",           if_ltsw_trunk,          if_ltsw_trunk_usage,        "Manage port aggregation"},
    {"TX",              cmd_ltsw_txn,           cmd_ltsw_txn_usage,         "Transmit one or more packets"},
    {"VLAN",            cmd_ltsw_vlan,          cmd_ltsw_vlan_usage,        "Manage VLAN usage"},
    {"WARMBOOT",        sh_warmboot,            sh_warmboot_usage,          "Optionally boot warm"},
};

int bcm_ltsw_cmd_cnt = COUNTOF(bcm_ltsw_cmd_list);

#endif /* BCM_LTSW_SUPPORT */

void
cmdlist_init(void)
{
    int         i;
    int         unit;

#if defined(BCM_ESW_SUPPORT)
    for (i = 1; i < bcm_esw_cmd_cnt; i++) {
        if (sal_strcasecmp(bcm_esw_cmd_list[i].c_cmd,
        bcm_esw_cmd_list[i - 1].c_cmd) <= 0) {
            cli_out("WARNING: bcm esw command %s not alphabetized\n",
                    bcm_esw_cmd_list[i].c_cmd);
        }
    }
#endif

    for (i = 1; i < bcm_cmd_common_cnt; i++) {
        if (sal_strcasecmp(bcm_cmd_common[i].c_cmd,
                       bcm_cmd_common[i - 1].c_cmd) <= 0) {
            cli_out("WARNING: bcm common command %s not alphabetized\n",
                    bcm_cmd_common[i].c_cmd);
        }
    }

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
    cmd_dpp_diag_usage_update();
    for (i = 1; i < bcm_dpp_cmd_cnt; i++) {
        if (sal_strcasecmp(bcm_dpp_cmd_list[i].c_cmd,
        bcm_dpp_cmd_list[i - 1].c_cmd) <= 0) {
            cli_out("WARNING: bcm dpp command %s not alphabetized\n",
                    bcm_dpp_cmd_list[i].c_cmd);
        }
    }
#endif

#if defined(BCM_DNX_SUPPORT)
    for (i = 1; i < bcm_dnx_cmd_cnt; i++) {
        if (sal_strcasecmp(bcm_dnx_cmd_list[i].c_cmd,
        bcm_dnx_cmd_list[i - 1].c_cmd) <= 0) {
            cli_out("WARNING: bcm dnx command %s not alphabetized\n",
                    bcm_dnx_cmd_list[i].c_cmd);
        }
    }
#endif

#if defined(BCM_DNXF_SUPPORT)
    for (i = 1; i < bcm_dnxf_cmd_cnt; i++) {
        if (sal_strcasecmp(bcm_dnxf_cmd_list[i].c_cmd,
        bcm_dnxf_cmd_list[i - 1].c_cmd) <= 0) {
            cli_out("WARNING: bcm dnxf command %s not alphabetized\n",
                    bcm_dnxf_cmd_list[i].c_cmd);
        }
    }
#endif
#if defined(BCM_LTSW_SUPPORT)
    for (i = 1; i < bcm_ltsw_cmd_cnt; i++) {
        if (sal_strcasecmp(bcm_ltsw_cmd_list[i].c_cmd,
                           bcm_ltsw_cmd_list[i - 1].c_cmd) <= 0) {
            cli_out("WARNING: bcm ltsw command %s not alphabetized\n",
                    bcm_ltsw_cmd_list[i].c_cmd);
        }
    }
#endif
    for(i = 0; i < SOC_MAX_NUM_DEVICES; i++) {
        cur_cmd_list[i] = NULL;
        cur_cmd_cnt[i] = 0;
        cur_mode[i] = ILLEGAL_CMD_MODE;
        dyn_cmd_list[i] = NULL;
        dyn_cmd_cnt[i] = 0;
    }

    for (i = 0; i < soc_ndev; i++) {
        unit = SOC_NDEV_IDX2DEV(i);
        if(!soc_attached(unit)) {
            command_mode_set(unit, ESW_CMD_MODE);
        }
#if defined(BCM_DNX_SUPPORT)
        else if(SOC_IS_DNX(unit) ) {
            command_mode_set(unit, DNX_CMD_MODE);
            /** initialize DNX specific dynamic shell commands */
            if(diag_sand_error_get(cmd_dnx_sh_init(unit)) != CMD_OK)
            {
                cli_out("Error: Problem while initiating DNX Dynamic commands\n");
            }
        }
#endif /* BCM_DNX_SUPPORT */

#if defined(BCM_DNXF_SUPPORT)
        else if(SOC_IS_DNXF(unit) ) {
            command_mode_set(unit, DNXF_CMD_MODE);
            /** initialize DNXF specific dynamic shell commands */
            if(diag_sand_error_get(cmd_dnxf_sh_init(unit)) != CMD_OK)
            {
                cli_out("Error: Problem while initiating DNX Dynamic commands\n");
            }
        }
#endif /* BCM_DNXF_SUPPORT */

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
        else if(SOC_IS_DPP(unit) || SOC_IS_DFE(unit)) {
            command_mode_set(unit, DPP_CMD_MODE);
        }
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#if defined(BCM_ESW_SUPPORT)
        else if(SOC_IS_ESW(unit)) {
            command_mode_set(unit, ESW_CMD_MODE);
        }
#endif /* BCM_ESW_SUPPORT */

#if defined(BCM_LTSW_SUPPORT)
        if(SOC_IS_LTSW(unit)) {
            command_mode_set(unit, LTSW_CMD_MODE);
        }
#endif /* BCM_LTSW_SUPPORT */

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_PKTIO)
        if (soc_feature(unit, soc_feature_sdklt_pktio)) {
            diag_pktio_mode_enable(unit);
        } else {
            diag_pktio_mode_disable(unit);
        }
#endif
    }

#if defined(BCM_ESW_SUPPORT)
    _bcm_trunk_ref = NULL;
#endif
}

/*
 * Add a dynamic command to the shell
 */
int
cmdlist_add(int unit, cmd_t* cmd)
{

    if((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) {
        return CMD_FAIL;
    }
    /*
     * Allow the dynamic command list to be cleared by passing NULL
     */
    if(cmd == NULL) {
        dyn_cmd_cnt[unit] = 0;
        return CMD_OK;
    }

    /*
     * Allow the dynamic command memory to be allocated when needed.
     * Also allows it to be called prior to diag_shell invocation if necessary.
     */

    if(dyn_cmd_list[unit] == NULL) {
        dyn_cmd_list[unit] = sal_alloc(sizeof(cmd_t)*BCM_SHELL_MAX_DYNAMIC_CMDS, "DYN CMD LIST");
        if(dyn_cmd_list[unit] == NULL) {
            return CMD_FAIL;
        }
        else {
            dyn_cmd_cnt[unit] = 0;
        }
    }

    /*
     * Attempt to add the command
     */
    if(dyn_cmd_cnt[unit] >= BCM_SHELL_MAX_DYNAMIC_CMDS) {
        /* Full */
        return CMD_FAIL;
    }
    else {
        dyn_cmd_list[unit][dyn_cmd_cnt[unit]] = *cmd;
        dyn_cmd_cnt[unit]++;
        return CMD_OK;
    }
}

int
cmdlist_remove(int unit, cmd_t* cmd)
{
    cmd_t* p;
    int i;

    if((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) {
        return CMD_FAIL;
    }
    /*
     * Find this command in the array
     */
    for(i = 0, p = dyn_cmd_list[unit]; i < dyn_cmd_cnt[unit]; i++, p++) {
        if((p->c_f == cmd->c_f) && !sal_strcasecmp(p->c_cmd, cmd->c_cmd)) {

            /*
             * This command should be removed.
             */
            if(i == dyn_cmd_cnt[unit] - 1) {
                /* This command is the last entry */
                dyn_cmd_cnt[unit]--;
            }
            else {
                /* Swap the last command into this slot */
                dyn_cmd_list[unit][i] = dyn_cmd_list[unit][dyn_cmd_cnt[unit]-1];
                dyn_cmd_cnt[unit]--;
            }
            if(dyn_cmd_cnt[unit] == 0)
            {
                sal_free(dyn_cmd_list[unit]);
                dyn_cmd_list[unit] = NULL;
            }
            return CMD_OK;
        }
    }

    /* Not found in the list */
    return CMD_NFND;
}


void
command_mode_set(int unit, int mode)
{
    if((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) {
        cli_out("WARNING: unit:%d in command_mode_set is out of range\n", unit);
        return;
    }

    if (mode == cur_mode[unit]) {
        return;
    }
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
    cmd_dpp_diag_usage_update();
#endif

    switch (mode) {
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
    case DPP_CMD_MODE:
        cur_cmd_list[unit] = bcm_dpp_cmd_list;
        cur_cmd_cnt[unit] = bcm_dpp_cmd_cnt;
        break;
#endif

#if defined(BCM_DNX_SUPPORT)
    case DNX_CMD_MODE:
        cur_cmd_list[unit] = bcm_dnx_cmd_list;
        cur_cmd_cnt[unit] = bcm_dnx_cmd_cnt;
        break;
#endif

#if defined(BCM_DNXF_SUPPORT)
    case DNXF_CMD_MODE:
        cur_cmd_list[unit] = bcm_dnxf_cmd_list;
        cur_cmd_cnt[unit] = bcm_dnxf_cmd_cnt;
        break;
#endif

#if defined(BCM_LTSW_SUPPORT)
    case LTSW_CMD_MODE:
        cur_cmd_list[unit] = bcm_ltsw_cmd_list;
        cur_cmd_cnt[unit] = bcm_ltsw_cmd_cnt;
        break;
#endif

#if defined(BCM_ESW_SUPPORT)
    case ESW_CMD_MODE:
    default:   /* BCM is default command mode */
        cur_cmd_list[unit] = bcm_esw_cmd_list;
        cur_cmd_cnt[unit] = bcm_esw_cmd_cnt;
        break;
#endif
    }
    cur_mode[unit] = mode;
}

int
command_mode_get(int unit)
{
    if((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES)) {
        cli_out("WARNING: unit:%d in command_mode_get is out of range\n", unit);
        return ILLEGAL_CMD_MODE;
    }

    return cur_mode[unit];
}

/*
 * Function:
 *     subcommand_execute
 * Purpose:
 *     Parse subcommand and execute function defined in given command list
 *     table.
 *
 *     This function provides an alternate way of parsing subcommands using
 *     currently defined parse functions and typedefs.  It is not limited
 *     to parsing subcommands of a command, it can be called from a subcommand
 *     as long as the correct table is given.
 *
 * Params:
 *     unit     - Unit number (passed-in to the initial command function)
 *     args     - Pointer to the argument structure
 *                Current argument MUST point to the subcommand to be parsed
 *     cmd_list - Table of subcommands for a given command
 *     cmd_cnt  - Number of subcommands in table
 */
cmd_result_t
subcommand_execute(int unit, args_t *args, cmd_t *cmd_list, int cmd_cnt)
{
    char            *initial_cmd = ARG_CMD(args);
    char            *key;
    cmd_t           *cmd;
    char            *param;
    cmd_result_t    rv = CMD_OK;

    if ((key = ARG_GET(args)) == NULL) {
        sal_printf("%s:  Subcommand required\n", initial_cmd);
        return CMD_USAGE;
    }

    cmd = (cmd_t *) parse_lookup(key, cmd_list, sizeof(cmd_t), cmd_cnt);

    if (cmd == NULL)
    {
        sal_printf("%s: Unknown subcommand %s\n", initial_cmd, key);
        return CMD_USAGE;
    }

    /* If next param is "?", just display help for command */
    param = ARG_CUR(args);
    if (param != NULL) {
        if (sal_strcmp(param, "?") == 0)
        {
            ARG_GET(args);    /* Consume "?" */
            if ((cmd->c_usage != NULL) &&
                (soc_property_get(unit, spn_HELP_CLI_ENABLE, 1))) {
                sal_printf("Usage:  %s\n", cmd->c_usage);
            }
            if ((cmd->c_help != NULL) &&
                soc_property_get(unit, spn_HELP_CLI_ENABLE, 1)) {
                sal_printf("Help :  %s\n", cmd->c_help);
            }
            return rv;
        }
    }

    rv = cmd->c_f(unit, args);

    return rv;
}
