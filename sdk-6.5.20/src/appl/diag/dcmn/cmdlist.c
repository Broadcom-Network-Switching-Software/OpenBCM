/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cmdlist.c
 * Purpose:     List of commands available in DPP mode
 * Requires:
 */

#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include <appl/diag/dcmn/cmdlist.h> 
#include <appl/diag/sand/diag_sand_access.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_signals.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_mbist.h>

/*
 * If PEM_DIAG is set non-zero then CLI menu includes PEM operations.
 * Set to '0' on final version.
 */
#define PEM_DIAG 1

cmd_t bcm_dpp_cmd_list[] = {
 
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)) && defined(BCM_PETRA_SUPPORT)
    {"arad",      cmd_dpp_arad_card,           cmd_dpp_arad_card_usage, "Arad card specific commands" },
    {"ardon",      cmd_dpp_ardon_card,           cmd_dpp_ardon_card_usage, "Ardon card specific commands" },
#endif
    {"Attach",      sh_attach,                sh_attach_usage,
     "Attach SOC device(s)" },
    {"AVS",        cmd_avs,      cmd_avs_usage,        "AVS - get AVS (Adjustable Voltage Scaling) value"  },
    {"CLEAR",           cmd_dpp_clear,          cmd_dpp_clear_usage,
        "Clear a memory table or counters" },
#ifdef BCM_PETRA_SUPPORT
    {"COSQ",       cmd_dpp_cosq,        cmd_dpp_cosq_usage,
     "Set/Get cosq Parameters" },
#endif 
    {"CounTeR",     cmd_dpp_counter,          cmd_dpp_counter_usage,
     "Enable/disable counter collection"},
#ifdef BCM_PETRA_SUPPORT
    {"CounTeRProc", cmd_dpp_ctr_proc,    cmd_dpp_ctr_proc_usage,
     "Counter processor diagnostics"},
#endif /* BCM_PETRA_SUPPORT */
#if defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)
    {"cpu_regs",    cmd_sand_cpu_regs,         cmd_sand_cpu_regs_usage, "Read/Write function to cpu regs" },
#endif
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_DDR3_SUPPORT)
    {"DDRPhyCDR",       cmd_arad_ddr_phy_cdr,        cmd_arad_ddr_phy_cdr_usage,   "Control Combo28 DDR CDR"},
    {"DDRPhyRegs",      cmd_arad_ddr_phy_regs,       cmd_arad_ddr_phy_regs_usage,  "Read/Write DDR40 phy registers"},
    {"DDRPhyTune",      cmd_arad_ddr_phy_tune,       cmd_arad_ddr_phy_tune_usage,  "Tune DDR40 phy registers"},
#endif
#endif
    {"DEInit",          sh_deinit,              sh_deinit_usage, "Deinit SW modules" },
    {"DETach",          sh_detach,              sh_detach_usage,        
      "Detach SOC device(s)" },
#if defined(BCM_PETRA_SUPPORT)
    {"DEviceReset",          sh_device_reset,              sh_device_reset_usage, "Perform different device reset modes/actions." },      
#endif
    {"DIAG",        cmd_dpp_diag,             cmd_dpp_diag_usage,
     "Display diagnostic information" },
    {"DMA",         cmd_sand_dma,              cmd_sand_dma_usage,
     "DMA Facilities Interface" },
#if defined(BCM_PETRA_SUPPORT)
    {"DRAMBuf", cmd_arad_dram_buf, cmd_arad_dram_buf_usage, "Manage and get information on dram buffers"},
    {"DramMmuIndAccess", cmd_arad_dram_mmu_ind_access, cmd_arad_dram_mmu_ind_access_usage, "Perform MMU indirect reading and writing"},
#endif /* BCM_PETRA_SUPPORT */
    {"Dump",        cmd_sand_mem_get,             cmd_sand_mem_get_usage,
     "Dump an address space or registers" },
#if defined(INCLUDE_DUNE_UI) && defined (BCM_PETRA_SUPPORT)
    {"DUNE_ui",     cmd_dpp_ui,               cmd_dpp_ui_usage,
     "Open DUNE UI shell for DPP commands"},
#endif /* INCLUDE_DUNE_UI && BCM_PETRA_SUPPORT */
#if defined(BCM_PETRA_SUPPORT) && !defined(NO_FILEIO)
    {"export",      cmd_dpp_export,           cmd_dpp_export_usage_str, "Data Export commands" },
#endif /* BCM_PETRA_SUPPORT && !NO_FILEIO */
    {"Fabric",      diag_dnx_fabric_diag_pack,           diag_dnx_fabric_diag_pack_usage_str,
     "DNX fabric diagnostic pack" },
#ifdef BCM_PETRA_SUPPORT
    {"Fc",          cmd_dpp_fc,               cmd_dpp_fc_usage,
     "Show Flow-control status"},
#endif /* BCM_PETRA_SUPPORT */
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__)) && defined(BCM_DFE_SUPPORT)
    {"fe1600",      cmd_dpp_fe1600_card,           cmd_dpp_fe1600_card_usage, "Fe1600 card specific commands" },
    {"fe3200",      cmd_dpp_fe3200_card,           cmd_dpp_fe3200_card_usage, "Fe3200 card specific commands" },
#endif
    {"Getreg",      cmd_sand_reg_get,          cmd_sand_reg_get_usage,
     "Get register" },
    {"GPort",       cmd_dpp_gport,            cmd_dpp_gport_usage,
     "Show the current queue gports set up in the system"},
#ifdef BCM_PETRA_SUPPORT
    {"Gtimer",      cmd_dpp_gtimer,           cmd_dpp_gtimer_usage,
     "Manage gtimer"},
#endif /* BCM_PETRA_SUPPORT */
#ifdef INCLUDE_I2C
    {"I2C",             cmd_i2c,            cmd_i2c_usage,              "Inter-Integrated Circuit (I2C) Bus commands"},
#endif  
    {"INIT",        sh_init,                  sh_init_usage,      "Initialize SOC and S/W"},

    {"init_dnx",            cmd_init_dnx,                appl_dcmn_init_usage,          "Initialize/deinitialize DNX S/W"},

{"intr",        cmd_sand_intr,             cmd_sand_intr_usage,
     "Interrupt Controling" },
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)) && defined(BCM_JERICHO_SUPPORT)
    {"jericho",      cmd_dpp_jericho_card,           cmd_dpp_jericho_card_usage,
     "Jericho card specific commands" },
#endif
#if defined(BCM_PETRA_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    {"KBP", cmd_dpp_kbp, cmd_dpp_kbp_usage, "Perform Access to KBP data"},
#endif   
#ifdef INCLUDE_KNET
    {"KNETctrl",        cmd_dpp_knet,          cmd_dpp_knet_usage,    "Manage kernel network functions"},
#endif
#ifdef BCM_PETRA_SUPPORT
    {"L2",          cmd_dpp_l2,               cmd_dpp_l2_usage,
     "Manage L2 (MAC) addresses"},
    {"L3",          cmd_dpp_l3,               cmd_dpp_l3_usage,
     "Manage L3 "},
#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_LB_SUPPORT
    {"LB",          cmd_dpp_lb,               cmd_dpp_lb_usage,
     "Show Link-bonding status"},
#endif /* BCM_LB_SUPPORT */
    {"LINKscan",    if_esw_linkscan,          if_esw_linkscan_usage,
     "Configure/Display link scanning" },
    {"LISTmem",     cmd_sand_mem_list,         cmd_sand_mem_list_usage,
     "List the entry format for a given table" },
    {"Listreg",     cmd_sand_reg_list,         cmd_sand_reg_list_usage,
     "List register fields"},

#if defined(__DUNE_GTO_BCM_CPU__) && defined(BCM_PETRA_SUPPORT) && defined(BCM_CMICM_SUPPORT)
    {"LLm",     cmd_dpp_llm,         cmd_dpp_llm_usage,
     "LLM Application"},
#endif /* defined(__DUNE_GTO_BCM_CPU__) && defined(BCM_PETRA_SUPPORT) && defined(BCM_CMICM_SUPPORT) */

#ifndef SAL_CONFIG_FILE_DISABLE
    {"MBIST",      cmd_sand_mbist,     cmd_sand_mbist_usage, cmd_sand_mbist_desc},
#endif

    {"MODify",      cmd_sand_mem_modify,       cmd_sand_mem_modify_usage,
     "Modify table entry by field names" },
    {"Modreg",      cmd_sand_reg_modify,       cmd_sand_reg_modify_usage,
     "Register Read/Modify/Write" },
#ifdef BCM_PETRA_SUPPORT
#if defined(__DUNE_GTO_BCM_CPU__)
    {"negev",      cmd_dcmn_negev_chassis,           cmd_dcmn_negev_chassis_usage, "Negev chassis specific commands" },
#endif
#endif
    {"PacketWatcher",   pw_command,             pw_usage,                   "Monitor ports for packets"},
#ifdef BCM_JERICHO_SUPPORT
    {"PaRSer",      cmd_dpp_parser,     cmd_dpp_parser_usage, "Present available parser resources"},
#endif
    {"PBMP",        cmd_dpp_pbmp,             cmd_dpp_pbmp_usage,
     "Convert port bitmap string to hex"},
    {"PCIC",        cmd_sand_pcic_access,      cmd_sand_pcic_access_usage,        "Access to PCI configuration space"  },
    {"pcie",        cmd_sand_pcie_reg,         cmd_sand_pcie_reg_usage,
     "Read/Write form devices via pcie" },

#if defined(BCM_88270_A0)
#if PEM_DIAG
    {"PEM",      cmd_dpp_pem /* cmd_sand_reg_get */,          cmd_dpp_pem_usage /* cmd_sand_reg_get_usage */,
     "PEM operations" },
#endif
#endif
    {"PHY",         if_dpp_phy,             if_dpp_phy_usage,
            "Set/Display phy characteristics"},
    {"PORT",        if_dpp_port,              if_dpp_port_usage,      "Set/Display port characteristics"},
#ifdef PORTMOD_DIAG
    {"PortMod", cmd_portmod_diag, portmod_diag_usage, "portmod diagnostics"},
#endif
    {"PortStat",    if_dpp_port_stat,         if_dpp_port_stat_usage,
     "Display port status in table"},
    {"PROBE",       sh_probe,                 sh_probe_usage,
     "Probe for available SOC units"},
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)) && defined(BCM_88375_A0)
    {"qmx",      cmd_dpp_qmx_card,           cmd_dpp_qmx_card_usage,
     "Qmx card specific commands" },
#endif
#ifdef  BCM_PETRA_SUPPORT
     {"REINIT",        sh_reinit,              sh_init_usage,
     "ReInitialize SOC and S/W"},
#endif
 /* don't use shr_res or shr_mres directly here 
#ifdef BCM_ESW_SUPPORT 
    {"ResTest",     cmd_sh_restest,           cmd_sh_restest_usage,
     "Tests for resource manager"},
#endif  
 */
    {"Set_Device",      cmd_set_device,           cmd_set_device_usage,
     "Common set_device command" },
    {"Setreg",      cmd_sand_reg_set,          cmd_sand_reg_set_usage,
     "Set register" },
    {"SHOW",        cmd_dpp_show,             cmd_dpp_show_usage,
     "Show information on a subsystem" },
    {"SOC",         cmd_dpp_soc,              cmd_dpp_soc_usage,
     "Print internal driver control information"},
#ifdef BCM_WARM_BOOT_API_TEST
    {"STATE",       sh_state,                 sh_state_usage,
    "state diagnostic tool"},
#endif
#ifdef BCM_PETRA_SUPPORT
    {"STG",         if_dpp_stg,               if_dpp_stg_usage,      
     "Manage spanning tree groups"},
#endif
    {"STKMode",     cmd_stkmode,              cmd_stkmode_usage,
     "Hardware Stacking Mode Control"},
    {"SwitchControl", cmd_sand_switch_control, cmd_sand_switch_control_usage,
     "General switch control"},
#ifdef BCM_PETRA_SUPPORT
    {"Tdm",         cmd_dpp_tdm,              cmd_dpp_tdm_usage,
     "Manage tdm"},
#endif /* BCM_PETRA_SUPPORT */
#ifdef INCLUDE_TEST
    {"TestClear",   test_clear,               test_clear_usage,
     "Clear run statisistics for a test"},
    {"TestList",    test_print_list,          test_list_usage,
     "List loaded tests and status"},
    {"TestMode",    test_mode,                test_mode_usage,
     "Set global test run modes"},
    {"TestParameters", test_parameters,       test_parameters_usage,
     "Set test Parameters"},
    {"TestRun",     test_run,                 test_run_usage,
     "Run a specific or selected tests"},
    {"TestSelect",  test_select,              test_select_usage,
     "Select tests for running"},
#endif
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)) && defined(BCM_PETRA_SUPPORT)
    {"TimeMeasurement",          cmd_dpp_time_measure,               cmd_dpp_time_measure_usage, "Time Measurement"},
#endif
#ifdef BCM_PETRA_SUPPORT
    {"Tx",          cmd_dpp_tx,               cmd_dpp_tx_usage,
     "Transmit packets"},
    {"Vlan",        cmd_dpp_vlan,             cmd_dpp_vlan_usage,
     "Manage VLANs"},
#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT
    {"WARMBOOT",        sh_warmboot,            sh_warmboot_usage,
    "Optionally boot warm"},
#endif /*BCM_WARM_BOOT_SUPPORT*/

    {"Write",       cmd_sand_mem_write,        cmd_sand_mem_write_usage,
     "Write entry(s) into a table" },
#if defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)
    {"XXReload",        cmd_xxreload,           cmd_xxreload_usage,     "\"Easy\" Reload control" },
#endif /*defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)*/
};

int bcm_dpp_cmd_cnt = COUNTOF(bcm_dpp_cmd_list);

