/** \file diag_dnx_dram_firmware_aapl.c
 *
 * Diagnostics for dram firmware
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

#include <sal/core/boot.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/dram/dram.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/dram/hbmc/hbmc_firmware.h>

#ifdef INCLUDE_AAPL
/** 'hbm_diag' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_hbm_diag_man = {
    .brief = "Run full diagnostics on the HBM interface",
    .full = "Run full diagnostics on the HBM interface\n",
};

/** 'operation' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_operation_man = {
    .brief = "Run an HBM operation",
    .full = "Run an HBM operation\n",
};

/** 'operation start' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_operation_start_man = {
    .brief = "Start an HBM operation",
    .full = "Start an HBM operation\n",
};

/** 'operation results' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_operation_results_man = {
    .brief = "Check results for an HBM operation",
    .full = "Check results for an HBM operation\n",
};

/** 'device_id' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_device_id_man = {
    .brief = "Display the HBM device ID",
    .full = "Display the HBM device ID\n",
};

/** 'temperature' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_temperature_man = {
    .brief = "Display the HBM temperature",
    .full = "Display the HBM temperature\n",
};

/** 'temperature_diag' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_temperature_diag_man = {
    .brief = "Run HBM temperature diagnostics.  Loops <count> times",
    .full = "Run HBM temperature diagnostics.  Loops <count> times\n",
    .examples = "count=2\n"
};

/** 'init_nwl' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_init_nwl_man = {
    .brief = "Initializes the NWL memory controller",
    .full = "Initializes the NWL memory controller\n",
};

/** 'ctc_init' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_ctc_init_man = {
    .brief = "Start HBM CTC running in continuous mode. Runs reset and NWL init",
    .full = "Start HBM CTC running in continuous mode. Runs reset and NWL init\n",
};

/** 'ctc_start' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_ctc_start_man = {
    .brief = "Start HBM CTC running in continuous mode. No reset or NWL init performed",
    .full = "Start HBM CTC running in continuous mode. No reset or NWL init performed\n",
};

/** 'ctc_stop' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_ctc_stop_man = {
    .brief = "Stop HBM CTC",
    .full = "Stop HBM CTC\n",
};

/** 'ctc_status' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_ctc_status_man = {
    .brief = "Print the HBM CTC status",
    .full = "Print the HBM CTC status\n",
};

/** 'ctc_diag' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_ctc_diag_man = {
    .brief = "Runs full CTC diagnostics with reset and NWL initialization",
    .full = "Runs full CTC diagnostics with reset and NWL initialization\n",
};

/** 'mmt_run' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_mmt_run_man = {
    .brief = "Run all MMT patterns once, starting with the defined pattern",
    .full = "Run all MMT patterns once, starting with the defined pattern\n",
};

/** 'mmt_start' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_mmt_start_man = {
    .brief = "Start HBM MMT running in continuous mode",
    .full = "Start HBM MMT running in continuous mode\n",
};

/** 'mmt_stop' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_mmt_stop_man = {
    .brief = "Stop HBM MMT",
    .full = "Stop HBM MMT\n",
};

/** 'mmt_status' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_mmt_status_man = {
    .brief = "Print the HBM MMT status with optional state information",
    .full = "Print the HBM MMT status with optional state information\n",
};

/** 'mmt_loop' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_mmt_loop_man = {
    .brief = "Loop on mmt <count> times",
    .full = "Loop on mmt <count> times\n",
};

/** 'mmt_margin' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_mmt_margin_man = {
    .brief = "Measures the setup/hold margin on the HBM interface",
    .full = "Measures the setup/hold margin on the HBM interface\n",
};

/** 'mmt_diag' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_mmt_diag_man = {
    .brief = "Runs full MMT diagnostics",
    .full = "Runs full MMT diagnostics\n",
};

/** 'mrs' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_mrs_man = {
    .brief = "Read the HBM mode register values",
    .full = "Read the HBM mode register values\n",
};

/** 'tmrs' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_tmrs_man = {
    .brief = "Load a TMRS code to one or all channels with optional safety",
    .full = "Load a TMRS code to one or all channels with optional safety\n",
};

/** 'reset' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_reset_man = {
    .brief = "Resets the PHY/HBM interface",
    .full = "Resets the PHY/HBM interface\n",
};

/** 'param' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_param_man = {
    .brief = "Sets an HBM parameter and value pair",
    .full = "Sets an HBM parameter and value pair.\n" "If 'value' is not supplied then read the HBM parameter value.\n",
};

/** 'read_spares' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_read_spares_man = {
    .brief = "Reads the spare result registers",
    .full = "Reads the spare result registers\n",
};

/** 'read_lane_repairs' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_read_lane_repairs_man = {
    .brief = "Reads the current lane repair codes",
    .full = "Reads the current lane repair codes\n",
};

/** 'aerr' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_aerr_man = {
    .brief = "Runs the aerr bist test and diagnostics",
    .full = "Runs the aerr bist test and diagnostics\n",
};

/** 'aerr_ilb' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_aerr_ilb_man = {
    .brief = "Runs the aerr ilb bist test and diagnostics",
    .full = "Runs the aerr ilb bist test and diagnostics\n",
};

/** 'aerr_slb' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_aerr_slb_man = {
    .brief = "Runs the aerr slb bist test and diagnostics",
    .full = "Runs the aerr slb bist test and diagnostics\n",
};

/** 'aword' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_aword_man = {
    .brief = "Runs the aword bist test and diagnostics",
    .full = "Runs the aword bist test and diagnostics\n",
};

/** 'aword_ilb' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_aword_ilb_man = {
    .brief = "Runs the aword ilb bist test and diagnostics",
    .full = "Runs the aword ilb bist test and diagnostics\n",
};

/** 'aword_slb' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_aword_slb_man = {
    .brief = "Runs the aword slb bist test and diagnostics",
    .full = "Runs the aword slb bist test and diagnostics\n",
};

/** 'derr' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_derr_man = {
    .brief = "Runs the derr bist test and diagnostics",
    .full = "Runs the derr bist test and diagnostics\n",
};

/** 'derr_ilb' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_derr_ilb_man = {
    .brief = "Runs the derr ilb bist test and diagnostics",
    .full = "Runs the derr ilb bist test and diagnostics\n",
};

/** 'derr_slb' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_derr_slb_man = {
    .brief = "Runs the derr slb bist test and diagnostics",
    .full = "Runs the derr slb bist test and diagnostics\n",
};

/** 'dword' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_dword_man = {
    .brief = "Runs the dword bist test and diagnostics",
    .full = "Runs the dword bist test and diagnostics\n",
};

/** 'dword_ilb' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_dword_ilb_man = {
    .brief = "Runs the dword ilb bist test and diagnostics",
    .full = "Runs the dword ilb bist test and diagnostics\n",
};

/** 'dword_slb' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_dword_slb_man = {
    .brief = "Runs the dword slb bist test and diagnostics",
    .full = "Runs the dword slb bist test and diagnostics\n",
};

/** 'dword_read' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_dword_read_man = {
    .brief = "Runs the dword read bist test and diagnostics",
    .full = "Runs the dword read bist test and diagnostics\n",
};

/** 'dword_write' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_dword_write_man = {
    .brief = "Runs the dword write bist test and diagnostics",
    .full = "Runs the dword write bist test and diagnostics\n",
};

/** 'lane_repair' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_lane_repair_man = {
    .brief = "Runs the lane repair operation",
    .full = "Runs the lane repair operation\n",
};

/** 'burn_lane_repairs' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_burn_lane_repairs_man = {
    .brief = "Burns any soft repairs as hard repairs",
    .full = "Burns any soft repairs as hard repairs\n",
};

/** 'cattrip' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_cattrip_man = {
    .brief = "Runs the cattrip test to verify cattrip toggling",
    .full = "Runs the cattrip test to verify cattrip toggling\n",
};

/** 'reset_mrs' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_reset_mrs_man = {
    .brief = "Updates the hbm mrs values via 1500",
    .full = "Updates the hbm mrs values via 1500\n",
};

/** 'reset_phy_config' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_reset_phy_config_man = {
    .brief = "Updates the phy_config wdr values",
    .full = "Updates the phy_config wdr values\n",
};

/** 'samsung_chipping' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_samsung_chipping_man = {
    .brief = "Runs the Samsung chipping test",
    .full = "Runs the Samsung chipping test\n",
};

/** 'cell_repair' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_cell_repair_man = {
    .brief = "Creates a hbm bitcell repair to the specific address",
    .full = "Creates a hbm bitcell repair to the specific address\n",
};

/** 'fuse_scan' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_fuse_scan_man = {
    .brief = "Runs hbm fuse scan on the address specific",
    .full = "Runs hbm fuse scan on the address specific\n",
};

/** 'soft_lane_repair' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_soft_lane_repair_man = {
    .brief = "Creates a soft lane repair on the specific channel",
    .full = "Creates a soft lane repair on the specific channel\n",
};

/** 'param_diag' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_param_diag_man = {
    .brief = "Prints the values for firmware parameters",
    .full = "Prints the values for firmware parameters\n",
};

/** 'mbist_diag' man page */
static sh_sand_man_t sh_dnx_firmware_aapl_mbist_diag_man = {
    .brief = "Run HBM MBIST diagnostics",
    .full = "Run HBM MBIST diagnostics\n",
};

/** Define aapl 'default value' */
static sh_sand_enum_t sand_diag_dram_firmware_aapl_default_enum[] = {
/* *INDENT-OFF* */
    /**String       Value                                   Description                         Plugin String*/
    {"default",     DNX_HBMC_FIRMWARE_AAPL_CMD_DEFAULT,     "Use default value",                NULL},
    {NULL}
/* *INDENT-ON* */
};

/** Define dram 'max' */
static sh_sand_enum_t sand_diag_dram_firmware_aapl_dram_enum[] = {
/* *INDENT-OFF* */
    /**String       Value                                   Description                         Plugin String*/
    {"max",         0,                                      "Maximum dram index",               "DNX_DATA.dram.general_info.max_nof_drams-1"},
    {NULL}
/* *INDENT-ON* */
};

/** Define 'all' and 'max' channel */
static sh_sand_enum_t sand_diag_dram_firmware_aapl_channel_enum[] = {
/* *INDENT-OFF* */
    /**String       Value                                   Description                         Plugin String*/
    {"all",         0xf,                                    "Run operation on all channels"},
    {"max",         0,                                      "Maximum channel index",            "DNX_DATA.dram.general_info.nof_channels-1"},
    {NULL}
/* *INDENT-ON* */
};

/* *INDENT-OFF* */
#define DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST \
    {"dram",                    SAL_FIELD_TYPE_UINT32,   "dram interface",                                          "0-max",        (void *) sand_diag_dram_firmware_aapl_dram_enum,        "0-max", SH_SAND_ARG_QUIET}, \
    {"op_timeout",              SAL_FIELD_TYPE_UINT32,   "Changes the default operation timeout",                   "0",            NULL,                                                   NULL,    SH_SAND_ARG_QUIET}, \
    {"frequency",               SAL_FIELD_TYPE_UINT32,   "Sets the hbm_freq parameter (Mhz)",                       "default",      (void *) sand_diag_dram_firmware_aapl_default_enum,     NULL,    SH_SAND_ARG_QUIET}, \
    {"spico_frequency",         SAL_FIELD_TYPE_UINT32,   "Sets the hbm_spico_freq parameter (Mhz)",                 "default",      (void *) sand_diag_dram_firmware_aapl_default_enum,     NULL,    SH_SAND_ARG_QUIET}, \
    {"save_soft_lane_repairs",  SAL_FIELD_TYPE_UINT32,   "Sets the save_soft_lane_repairs parameter",               "default",      (void *) sand_diag_dram_firmware_aapl_default_enum,     NULL,    SH_SAND_ARG_QUIET}, \
    {"div_mode",                SAL_FIELD_TYPE_UINT32,   "Sets the hbm_div_mode parameter",                         "default",      (void *) sand_diag_dram_firmware_aapl_default_enum,     NULL,    SH_SAND_ARG_QUIET}, \
    {"paritylatency",           SAL_FIELD_TYPE_UINT32,   "Sets the hbm_parity_latency parameter",                   "default",      (void *) sand_diag_dram_firmware_aapl_default_enum,     NULL,    SH_SAND_ARG_QUIET}, \
    {"t_rdlat_offset",          SAL_FIELD_TYPE_UINT32,   "Sets the t_rdlat_offset parameter",                       "default",      (void *) sand_diag_dram_firmware_aapl_default_enum,     NULL,    SH_SAND_ARG_QUIET}, \
    {"hbm_drvstr",              SAL_FIELD_TYPE_UINT32,   "Sets the hbm drive strength mrs parameter",               "default",      (void *) sand_diag_dram_firmware_aapl_default_enum,     NULL,    SH_SAND_ARG_QUIET}, \
    {"phy_drvstr",              SAL_FIELD_TYPE_UINT32,   "Sets the phy drive strength phy_config parameters",       "default",      (void *) sand_diag_dram_firmware_aapl_default_enum,     NULL,    SH_SAND_ARG_QUIET}, \
    {"dll",                     SAL_FIELD_TYPE_UINT32,   "Sets the dll delay_config parameter for stb and clk",     "default",      (void *) sand_diag_dram_firmware_aapl_default_enum,     NULL,    SH_SAND_ARG_QUIET}, \
    {"qc_rd",                   SAL_FIELD_TYPE_UINT32,   "Sets the phy read quarter cycle phy_config parametes",    "default",      (void *) sand_diag_dram_firmware_aapl_default_enum,     NULL,    SH_SAND_ARG_QUIET}, \
    {"qc_wr",                   SAL_FIELD_TYPE_UINT32,   "Sets the phy read quarter cycle phy_config parametes",    "default",      (void *) sand_diag_dram_firmware_aapl_default_enum,     NULL,    SH_SAND_ARG_QUIET}, \
    {"qc_ck",                   SAL_FIELD_TYPE_UINT32,   "Sets the phy read quarter cycle phy_config parametes",    "default",      (void *) sand_diag_dram_firmware_aapl_default_enum,     NULL,    SH_SAND_ARG_QUIET}, \
    {"qc",                      SAL_FIELD_TYPE_UINT32,   "Sets all phy quarter cycle phy_config parameters",        "default",      (void *) sand_diag_dram_firmware_aapl_default_enum,     NULL,    SH_SAND_ARG_QUIET}, \
    {"reset",                   SAL_FIELD_TYPE_BOOL,     "Resets the PHY/HBM interface",                            "FALSE",        NULL,                                                   NULL,    SH_SAND_ARG_QUIET}
/* *INDENT-ON* */

/** Params that can be used for the for the 'aapl' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type          | Option Description  | Option Default | Option Extension | Valid Range
     */
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};
/** Params that can be used for the for the 'aapl operation' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_operation_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type          | Option Description  | Option Default | Option Extension | Valid Range | flags
     */
    {"operation",    SAL_FIELD_TYPE_UINT32, "firmware operation", NULL,            NULL,              NULL,         SH_SAND_ARG_QUIET},
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};

/** Params that can be used for the for the 'temp_diag' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_temp_diag_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type          | Option Description  | Option Default | Option Extension | Valid Range
     */
    {"count",        SAL_FIELD_TYPE_UINT32, "loops count",        "1",             NULL,              NULL},
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};

/** Params that can be used for the for the 'ctc' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_ctc_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type          | Option Description  | Option Default | Option Extension | Valid Range | flags
     */
    {"pattern",      SAL_FIELD_TYPE_UINT32, "pattern",            NULL,            NULL,              NULL,         SH_SAND_ARG_QUIET},
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};

/* *INDENT-OFF* */
#define DNX_DRAM_FIRMWARE_AAPL_MMT_OPTION_LIST \
    {"mmt_rl",              SAL_FIELD_TYPE_UINT32,   "Sets the hbm_rl_override parameter",      "default",  (void *) sand_diag_dram_firmware_aapl_default_enum, NULL, SH_SAND_ARG_QUIET}, \
    {"mmt_dbi",             SAL_FIELD_TYPE_UINT32,   "Sets the hbm_dbi parameter",              "default",  (void *) sand_diag_dram_firmware_aapl_default_enum, NULL, SH_SAND_ARG_QUIET}, \
    {"mmt_read_repeat",     SAL_FIELD_TYPE_UINT32,   "Sets the mmt_read_repeat parameter",      "default",  (void *) sand_diag_dram_firmware_aapl_default_enum, NULL, SH_SAND_ARG_QUIET}, \
    {"mmt_t_rdlat_offset",  SAL_FIELD_TYPE_UINT32,   "Sets the mmt_t_rdlat_offset parameter",   "default",  (void *) sand_diag_dram_firmware_aapl_default_enum, NULL, SH_SAND_ARG_QUIET}, \
    {"mmt_rddata_en",       SAL_FIELD_TYPE_UINT32,   "Sets the mmt_rddata_en parameter",        "default",  (void *) sand_diag_dram_firmware_aapl_default_enum, NULL, SH_SAND_ARG_QUIET}
/* *INDENT-ON* */

/** Params that can be used for the for the 'mmt' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_mmt_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type          | Option Description  | Option Default | Option Extension | Valid Range
     */
    DNX_DRAM_FIRMWARE_AAPL_MMT_OPTION_LIST,
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};

/** Params that can be used for the for the 'mmt_start' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_mmt_start_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type          | Option Description  | Option Default | Option Extension | Valid Range | flags
     */
    {"pattern",      SAL_FIELD_TYPE_UINT32, "pattern",            "-1",            NULL,              NULL,         SH_SAND_ARG_QUIET},
    DNX_DRAM_FIRMWARE_AAPL_MMT_OPTION_LIST,
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};

/** Params that can be used for the for the 'mmt_status' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_mmt_status_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type          | Option Description  | Option Default | Option Extension | Valid Range | flags
     */
    {"state",        SAL_FIELD_TYPE_BOOL,   "state",              "0",             NULL,              NULL,         SH_SAND_ARG_QUIET},
    DNX_DRAM_FIRMWARE_AAPL_MMT_OPTION_LIST,
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};

/** Params that can be used for the for the 'mmt_loop' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_mmt_loop_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type          | Option Description  | Option Default | Option Extension | Valid Range | flags
     */
    {"count",        SAL_FIELD_TYPE_UINT32, "loops count",        "1",             NULL,              NULL,         SH_SAND_ARG_QUIET},
    {"pattern",      SAL_FIELD_TYPE_UINT32, "pattern",            "-1",            NULL,              NULL,         SH_SAND_ARG_QUIET},
    DNX_DRAM_FIRMWARE_AAPL_MMT_OPTION_LIST,
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};

/** Params that can be used for the for the 'mrs' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_mrs_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type          | Option Description    | Option Default | Option Extension                                  | Valid Range | flags
     */
    {"channel",      SAL_FIELD_TYPE_UINT32, "channel in interface", "all",           (void *) sand_diag_dram_firmware_aapl_channel_enum, "0-all",      SH_SAND_ARG_QUIET},
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};

/** Params that can be used for the for the 'tmrs' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_tmrs_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type          | Option Description    | Option Default | Option Extension                                  | Valid Range | flags
     */
    {"channel",      SAL_FIELD_TYPE_UINT32, "channel in interface", "all",           (void *) sand_diag_dram_firmware_aapl_channel_enum, "0-all",      SH_SAND_ARG_QUIET},
    {"safety",       SAL_FIELD_TYPE_BOOL,   "apply the safety bit", "0",             NULL,                                               NULL,         SH_SAND_ARG_QUIET},
    {"code",         SAL_FIELD_TYPE_STR,    "tmrs code",            NULL,            NULL,                                               NULL,         SH_SAND_ARG_QUIET},
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};

/** Params that can be used for the for the 'param' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_param_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type          | Option Description  | Option Default | Option Extension | Valid Range | flags
     */
    {"parameter",    SAL_FIELD_TYPE_UINT32, "parameter",          NULL,            NULL,              NULL,        SH_SAND_ARG_QUIET},
    {"value",        SAL_FIELD_TYPE_UINT32, "parameter value",    "-1",            NULL,              NULL,        SH_SAND_ARG_QUIET},
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};

/** Params that can be used for the for the 'cell_repair' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_cell_repair_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type          | Option Description    | Option Default | Option Extension                                  | Valid Range | flags
     */
    {"channel",        SAL_FIELD_TYPE_UINT32, "channel in interface", NULL,            (void *) sand_diag_dram_firmware_aapl_channel_enum, "0-all",    SH_SAND_ARG_QUIET},
    {"pseudo_channel", SAL_FIELD_TYPE_UINT32, "pseudo channel",       NULL,            NULL,                                               NULL,       SH_SAND_ARG_QUIET},
    {"sid",            SAL_FIELD_TYPE_UINT32, "SID",                  NULL,            NULL,                                               NULL,       SH_SAND_ARG_QUIET},
    {"bank",           SAL_FIELD_TYPE_UINT32, "Bank",                 NULL,            NULL,                                               NULL,       SH_SAND_ARG_QUIET},
    {"row",            SAL_FIELD_TYPE_UINT32, "Row",                  NULL,            NULL,                                               NULL,       SH_SAND_ARG_QUIET},
    {"hard_repair",    SAL_FIELD_TYPE_BOOL,   "Hard repair",          "0",             NULL,                                               NULL,       SH_SAND_ARG_QUIET},
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};

/** Params that can be used for the for the 'fuse_scan' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_fuse_scan_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type          | Option Description    | Option Default | Option Extension                                  | Valid Range  | flags
     */
    {"channel",        SAL_FIELD_TYPE_UINT32, "channel in interface", NULL,            (void *) sand_diag_dram_firmware_aapl_channel_enum, "0-all",     SH_SAND_ARG_QUIET},
    {"pseudo_channel", SAL_FIELD_TYPE_UINT32, "pseudo channel",       NULL,            NULL,                                               NULL,        SH_SAND_ARG_QUIET},
    {"sid",            SAL_FIELD_TYPE_UINT32, "SID",                  NULL,            NULL,                                               NULL,        SH_SAND_ARG_QUIET},
    {"bank",           SAL_FIELD_TYPE_UINT32, "Bank",                 NULL,            NULL,                                               NULL,        SH_SAND_ARG_QUIET},
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};

/** Params that can be used for the for the 'soft_lane_repair' diag */
static sh_sand_option_t dnx_dram_firmware_aapl_soft_lane_repair_option_list[] = {
/* *INDENT-OFF* */
    /*
     * Option name | Option Type           | Option Description          | Option Default | Option Extension                                  | Valid Range  | flags
     */
    {"channel",      SAL_FIELD_TYPE_UINT32,  "channel in interface",       NULL,            (void *) sand_diag_dram_firmware_aapl_channel_enum, "0-all",       SH_SAND_ARG_QUIET},
    {"code",         SAL_FIELD_TYPE_ARRAY32, "Repair codes (3 hex words)", NULL,            NULL,                                               NULL,          SH_SAND_ARG_QUIET},
    DNX_DRAM_FIRMWARE_AAPL_COMMON_OPTION_LIST,
    {NULL}
/* *INDENT-ON* */
};

/** Get common params */
static shr_error_e
sh_dnx_dram_firmware_aapl_common_params_get(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    uint32 *dram_start,
    uint32 *dram_end,
    dnx_hbmc_firmware_aapl_params_t * aapl_params)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(aapl_params, 0, sizeof(dnx_hbmc_firmware_aapl_params_t));

    SH_SAND_GET_UINT32_RANGE("dram", *dram_start, *dram_end);
    SH_SAND_GET_UINT32("op_timeout", aapl_params->operation_timeout);
    SH_SAND_GET_UINT32("frequency", aapl_params->freq);
    SH_SAND_GET_UINT32("spico_frequency", aapl_params->spico_freq);
    SH_SAND_GET_UINT32("save_soft_lane_repairs", aapl_params->save_soft_lane_repairs);
    SH_SAND_GET_UINT32("div_mode", aapl_params->div_mode);
    SH_SAND_GET_UINT32("paritylatency", aapl_params->pl);
    SH_SAND_GET_UINT32("t_rdlat_offset", aapl_params->t_rdlat_offset);
    SH_SAND_GET_UINT32("hbm_drvstr", aapl_params->hbm_drvstr);
    SH_SAND_GET_UINT32("phy_drvstr", aapl_params->phy_drvstr);
    SH_SAND_GET_UINT32("dll", aapl_params->dll);
    SH_SAND_GET_UINT32("qc_rd", aapl_params->qc_rd);
    SH_SAND_GET_UINT32("qc_wr", aapl_params->qc_wr);
    SH_SAND_GET_UINT32("qc_ck", aapl_params->qc_ck);
    SH_SAND_GET_UINT32("qc", aapl_params->qc);
    SH_SAND_GET_BOOL("reset", aapl_params->reset);

    /** Set optional mmt params to default, in case we call commands that don't use them */
    aapl_params->mmt_rl = DNX_HBMC_FIRMWARE_AAPL_CMD_DEFAULT;
    aapl_params->mmt_dbi = DNX_HBMC_FIRMWARE_AAPL_CMD_DEFAULT;
    aapl_params->mmt_read_repeat = DNX_HBMC_FIRMWARE_AAPL_CMD_DEFAULT;
    aapl_params->mmt_t_rdlat_offset = DNX_HBMC_FIRMWARE_AAPL_CMD_DEFAULT;
    aapl_params->mmt_rddata_en = DNX_HBMC_FIRMWARE_AAPL_CMD_DEFAULT;

exit:
    SHR_FUNC_EXIT;
}

/** Get common mmt params */
static shr_error_e
sh_dnx_dram_firmware_aapl_common_mmt_params_get(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    dnx_hbmc_firmware_aapl_params_t * aapl_params)
{
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("mmt_rl", aapl_params->mmt_rl);
    SH_SAND_GET_UINT32("mmt_dbi", aapl_params->mmt_dbi);
    SH_SAND_GET_UINT32("mmt_read_repeat", aapl_params->mmt_read_repeat);
    SH_SAND_GET_UINT32("mmt_t_rdlat_offset", aapl_params->mmt_t_rdlat_offset);
    SH_SAND_GET_UINT32("mmt_rddata_en", aapl_params->mmt_rddata_en);

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_hbm_diag_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_index;
    uint32 dram_start;
    uint32 dram_end;
    int is_cattrip_masked = FALSE;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    if (!SAL_BOOT_PLISIM)
    {
        SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                        (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

        for (dram_index = dram_start; dram_index <= dram_end; ++dram_index)
        {
            int is_valid = 0;

            /** Skip non-valid drams */
            SHR_IF_ERR_EXIT(dnx_dram_index_is_valid(unit, dram_index, &is_valid));
            if (!is_valid)
            {
                LOG_CLI((BSL_META("Dram index %d is not valid - and is skipped\n"), dram_index));
                continue;
            }

            /**
             * The tests package under "hbm -hbm-diag" includes lane_repair test which might
             * cause false cattrip indication, so we need to mask the cattrip indication
             * when running this specific tests package.
             */
            SHR_IF_ERR_EXIT(dnx_hbmc_cattrip_indication_mask_set(unit, dram_index, 1));
            is_cattrip_masked = TRUE;

            /** Run AAPL command */
            SHR_IF_ERR_EXIT(dnx_hbmc_firmware_aapl_cmd_run
                            (unit, dram_index, DNX_HBMC_FIRMWARE_AAPL_CMD_HBM_DIAG, &aapl_params));
        }
    }
exit:
    if (is_cattrip_masked)
    {
        for (dram_index = dram_start; dram_index <= dram_end; ++dram_index)
        {
            int is_valid = 0;
            SHR_IF_ERR_CONT(dnx_dram_index_is_valid(unit, dram_index, &is_valid));
            if (!is_valid)
            {
                continue;
            }

            SHR_IF_ERR_CONT(dnx_hbmc_cattrip_indication_mask_set(unit, dram_index, 0));
        }
    }
    SHR_FUNC_EXIT;
}

/** Generic function to run common aapl commands */
static shr_error_e
sh_dnx_dram_firmware_aapl_generic_cmd(
    int unit,
    uint32 dram_start,
    uint32 dram_end,
    dnx_hbmc_firmware_aapl_cmd_e aapl_cmd,
    dnx_hbmc_firmware_aapl_params_t * aapl_params)
{
    uint32 dram_index;
    SHR_FUNC_INIT_VARS(unit);

    if (!SAL_BOOT_PLISIM)
    {
        for (dram_index = dram_start; dram_index <= dram_end; ++dram_index)
        {
            int is_valid = 0;

            /** Skip non-valid drams */
            SHR_IF_ERR_EXIT(dnx_dram_index_is_valid(unit, dram_index, &is_valid));
            if (!is_valid)
            {
                LOG_CLI((BSL_META("Dram index %d is not valid - and is skipped\n"), dram_index));
                continue;
            }

            /** Run AAPL command */
            SHR_IF_ERR_EXIT(dnx_hbmc_firmware_aapl_cmd_run(unit, dram_index, aapl_cmd, aapl_params));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_operation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SH_SAND_GET_UINT32("operation", aapl_params.operation);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_OPERATION, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_operation_start_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SH_SAND_GET_UINT32("operation", aapl_params.operation);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_OPERATION_START, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_operation_results_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SH_SAND_GET_UINT32("operation", aapl_params.operation);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_OPERATION_RESULTS, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_device_id_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_DEVICE_ID, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_temp_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_TEMP, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_temp_diag_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SH_SAND_GET_UINT32("count", aapl_params.count);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_TEMP_DIAG, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_init_nwl_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_INIT_NWL, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_ctc_init_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SH_SAND_GET_UINT32("pattern", aapl_params.pattern);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_CTC_INIT, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_ctc_start_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SH_SAND_GET_UINT32("pattern", aapl_params.pattern);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_CTC_START, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_ctc_stop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_CTC_STOP, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_ctc_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_CTC_STATUS, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_ctc_diag_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_CTC_DIAG, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_mmt_run_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_mmt_params_get(unit, args, sand_control, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_MMT_RUN, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_mmt_start_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_mmt_params_get(unit, args, sand_control, &aapl_params));
    SH_SAND_GET_UINT32("pattern", aapl_params.pattern);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_MMT_START, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_mmt_stop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_mmt_params_get(unit, args, sand_control, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_MMT_STOP, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_mmt_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_mmt_params_get(unit, args, sand_control, &aapl_params));
    SH_SAND_GET_BOOL("state", aapl_params.state);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_MMT_STATUS, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_mmt_loop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_mmt_params_get(unit, args, sand_control, &aapl_params));
    SH_SAND_GET_UINT32("count", aapl_params.count);
    SH_SAND_GET_UINT32("pattern", aapl_params.pattern);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_MMT_STATUS, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_mmt_margin_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_mmt_params_get(unit, args, sand_control, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_MMT_MARGIN, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_mmt_diag_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_mmt_params_get(unit, args, sand_control, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_MMT_DIAG, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_mrs_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SH_SAND_GET_UINT32("channel", aapl_params.channel);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_MRS, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_tmrs_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SH_SAND_GET_UINT32("channel", aapl_params.channel);
    SH_SAND_GET_BOOL("safety", aapl_params.safety);
    SH_SAND_GET_STR("code", aapl_params.tmrs_code);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_TMRS, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_reset_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_RESET, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_param_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SH_SAND_GET_UINT32("parameter", aapl_params.param);
    SH_SAND_GET_UINT32("value", aapl_params.param_value);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_PARAM, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_read_spares_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_READ_SPARES, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_read_lane_repairs_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_READ_LANE_REPAIRS, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_aerr_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_AERR, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_aerr_ilb_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_AERR_ILB, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_aerr_slb_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_AERR_SLB, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_aword_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_AWORD, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_aword_ilb_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_AWORD_ILB, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_aword_slb_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_AWORD_SLB, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_derr_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_DERR, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_derr_ilb_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_DERR_ILB, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_derr_slb_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_DERR_SLB, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_dword_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_DWORD, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_dword_ilb_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_DWORD_ILB, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_dword_slb_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_DWORD_SLB, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_dword_read_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_DWORD_READ, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_dword_write_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_DWORD_WRITE, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_lane_repair_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_LANE_REPAIR, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_burn_lane_repairs_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_BURN_LANE_REPAIRS, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_cattrip_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_CATTRIP, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_reset_mrs_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_RESET_MRS, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_reset_phy_config_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_RESET_PHY_CONFIG, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_samsung_chipping_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_SAMSUNG_CHIPPING, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_cell_repair_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SH_SAND_GET_UINT32("channel", aapl_params.channel);
    SH_SAND_GET_UINT32("pseudo_channel", aapl_params.pc);
    SH_SAND_GET_UINT32("sid", aapl_params.sid);
    SH_SAND_GET_UINT32("bank", aapl_params.bank);
    SH_SAND_GET_UINT32("row", aapl_params.row);
    SH_SAND_GET_BOOL("hard_repair", aapl_params.hard_repair);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_CELL_REPAIR, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_fuse_scan_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SH_SAND_GET_UINT32("channel", aapl_params.channel);
    SH_SAND_GET_UINT32("pseudo_channel", aapl_params.pc);
    SH_SAND_GET_UINT32("sid", aapl_params.sid);
    SH_SAND_GET_UINT32("bank", aapl_params.bank);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_FUSE_SCAN, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_soft_lane_repair_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));
    SH_SAND_GET_UINT32("channel", aapl_params.channel);
    SH_SAND_GET_ARRAY32("code", aapl_params.repair_codes);

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_SOFT_LANE_REPAIR, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_mbist_diag_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_MBIST_DIAG, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_aapl_param_diag_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_aapl_params_t aapl_params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_common_params_get
                    (unit, args, sand_control, &dram_start, &dram_end, &aapl_params));

    /** Run AAPL command on all specified DRAMs */
    SHR_IF_ERR_EXIT(sh_dnx_dram_firmware_aapl_generic_cmd
                    (unit, dram_start, dram_end, DNX_HBMC_FIRMWARE_AAPL_CMD_PARAM_DIAG, &aapl_params));

exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_dram_firmware_aapl_cmds[] = {
/* *INDENT-OFF* */
    /* Name               | Leaf Action                                       | Junction Array | Options for Leaf                                     | Usage                                        | option_cb | invokes | flags*/
    {"HBM_DIAG",            sh_dnx_dram_firmware_aapl_hbm_diag_cmd,             NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_hbm_diag_man},
    {"OPeRation",           sh_dnx_dram_firmware_aapl_operation_cmd,            NULL,            dnx_dram_firmware_aapl_operation_option_list,          &sh_dnx_firmware_aapl_operation_man,           NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"OPeRation_Start",     sh_dnx_dram_firmware_aapl_operation_start_cmd,      NULL,            dnx_dram_firmware_aapl_operation_option_list,          &sh_dnx_firmware_aapl_operation_start_man,     NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"OPeRation_Results",   sh_dnx_dram_firmware_aapl_operation_results_cmd,    NULL,            dnx_dram_firmware_aapl_operation_option_list,          &sh_dnx_firmware_aapl_operation_results_man,   NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"ID",                  sh_dnx_dram_firmware_aapl_device_id_cmd,            NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_device_id_man},
    {"TEMPerature",         sh_dnx_dram_firmware_aapl_temp_cmd,                 NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_temperature_man},
    {"TEMPerature_Diag",    sh_dnx_dram_firmware_aapl_temp_diag_cmd,            NULL,            dnx_dram_firmware_aapl_temp_diag_option_list,          &sh_dnx_firmware_aapl_temperature_diag_man},
    {"INIT_NWL",            sh_dnx_dram_firmware_aapl_init_nwl_cmd,             NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_init_nwl_man,            NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"CTC_INIT",            sh_dnx_dram_firmware_aapl_ctc_init_cmd,             NULL,            dnx_dram_firmware_aapl_ctc_option_list,                &sh_dnx_firmware_aapl_ctc_init_man,            NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"CTC_START",           sh_dnx_dram_firmware_aapl_ctc_start_cmd,            NULL,            dnx_dram_firmware_aapl_ctc_option_list,                &sh_dnx_firmware_aapl_ctc_start_man,           NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"CTC_STOP",            sh_dnx_dram_firmware_aapl_ctc_stop_cmd,             NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_ctc_stop_man,            NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"CTC_STATUS",          sh_dnx_dram_firmware_aapl_ctc_status_cmd,           NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_ctc_status_man,          NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"CTC_DIAG",            sh_dnx_dram_firmware_aapl_ctc_diag_cmd,             NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_ctc_diag_man,            NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"MMT_RUN",             sh_dnx_dram_firmware_aapl_mmt_run_cmd,              NULL,            dnx_dram_firmware_aapl_mmt_option_list,                &sh_dnx_firmware_aapl_mmt_run_man,             NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"MMT_START",           sh_dnx_dram_firmware_aapl_mmt_start_cmd,            NULL,            dnx_dram_firmware_aapl_mmt_start_option_list,          &sh_dnx_firmware_aapl_mmt_start_man,           NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"MMT_STOP",            sh_dnx_dram_firmware_aapl_mmt_stop_cmd,             NULL,            dnx_dram_firmware_aapl_mmt_option_list,                &sh_dnx_firmware_aapl_mmt_stop_man,            NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"MMT_STATUS",          sh_dnx_dram_firmware_aapl_mmt_status_cmd,           NULL,            dnx_dram_firmware_aapl_mmt_status_option_list,         &sh_dnx_firmware_aapl_mmt_status_man,          NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"MMT_LOOP",            sh_dnx_dram_firmware_aapl_mmt_loop_cmd,             NULL,            dnx_dram_firmware_aapl_mmt_loop_option_list,           &sh_dnx_firmware_aapl_mmt_loop_man,            NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"MMT_MARGIN",          sh_dnx_dram_firmware_aapl_mmt_margin_cmd,           NULL,            dnx_dram_firmware_aapl_mmt_option_list,                &sh_dnx_firmware_aapl_mmt_margin_man,          NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"MMT_DIAG",            sh_dnx_dram_firmware_aapl_mmt_diag_cmd,             NULL,            dnx_dram_firmware_aapl_mmt_option_list,                &sh_dnx_firmware_aapl_mmt_diag_man,            NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"MRS",                 sh_dnx_dram_firmware_aapl_mrs_cmd,                  NULL,            dnx_dram_firmware_aapl_mrs_option_list,                &sh_dnx_firmware_aapl_mrs_man,                 NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"TMRS",                sh_dnx_dram_firmware_aapl_tmrs_cmd,                 NULL,            dnx_dram_firmware_aapl_tmrs_option_list,               &sh_dnx_firmware_aapl_tmrs_man,                NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"ReSeT",               sh_dnx_dram_firmware_aapl_reset_cmd,                NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_reset_man,               NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"PARAMeter",           sh_dnx_dram_firmware_aapl_param_cmd,                NULL,            dnx_dram_firmware_aapl_param_option_list,              &sh_dnx_firmware_aapl_param_man,               NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"ReaD_SPaRes",         sh_dnx_dram_firmware_aapl_read_spares_cmd,          NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_read_spares_man,         NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"ReaD_LaNe_RePaiRs",   sh_dnx_dram_firmware_aapl_read_lane_repairs_cmd,    NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_read_lane_repairs_man,   NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"AERR",                sh_dnx_dram_firmware_aapl_aerr_cmd,                 NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_aerr_man,                NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"AERR_ILB",            sh_dnx_dram_firmware_aapl_aerr_ilb_cmd,             NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_aerr_ilb_man,            NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"AERR_SLB",            sh_dnx_dram_firmware_aapl_aerr_slb_cmd,             NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_aerr_slb_man,            NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"DERR",                sh_dnx_dram_firmware_aapl_derr_cmd,                 NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_derr_man,                NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"DERR_ILB",            sh_dnx_dram_firmware_aapl_derr_ilb_cmd,             NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_derr_ilb_man,            NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"DERR_SLB",            sh_dnx_dram_firmware_aapl_derr_slb_cmd,             NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_derr_slb_man,            NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"AWORD",               sh_dnx_dram_firmware_aapl_aword_cmd,                NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_aword_man,               NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"AWORD_ILB",           sh_dnx_dram_firmware_aapl_aword_ilb_cmd,            NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_aword_ilb_man,           NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"AWORD_SLB",           sh_dnx_dram_firmware_aapl_aword_slb_cmd,            NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_aword_slb_man,           NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"DWORD",               sh_dnx_dram_firmware_aapl_dword_cmd,                NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_dword_man,               NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"DWORD_ILB",           sh_dnx_dram_firmware_aapl_dword_ilb_cmd,            NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_dword_ilb_man,           NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"DWORD_SLB",           sh_dnx_dram_firmware_aapl_dword_slb_cmd,            NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_dword_slb_man,           NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"DWORD_READ",          sh_dnx_dram_firmware_aapl_dword_read_cmd,           NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_dword_read_man,          NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"DWORD_WRITE",         sh_dnx_dram_firmware_aapl_dword_write_cmd,          NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_dword_write_man,         NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"LANE_REPAIR",         sh_dnx_dram_firmware_aapl_lane_repair_cmd,          NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_lane_repair_man,         NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"BURN_LANE_REPAIRS",   sh_dnx_dram_firmware_aapl_burn_lane_repairs_cmd,    NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_burn_lane_repairs_man,   NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"CATTRIP",             sh_dnx_dram_firmware_aapl_cattrip_cmd,              NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_cattrip_man,             NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"RESET_MRS",           sh_dnx_dram_firmware_aapl_reset_mrs_cmd,            NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_reset_mrs_man,           NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"RESET_PHY_ConFiG",    sh_dnx_dram_firmware_aapl_reset_phy_config_cmd,     NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_reset_phy_config_man,    NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"SaMSuNG_CHIPping",    sh_dnx_dram_firmware_aapl_samsung_chipping_cmd,     NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_samsung_chipping_man,    NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"CELL_RePaiR",         sh_dnx_dram_firmware_aapl_cell_repair_cmd,          NULL,            dnx_dram_firmware_aapl_cell_repair_option_list,        &sh_dnx_firmware_aapl_cell_repair_man,         NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"FUSE_SCAN",           sh_dnx_dram_firmware_aapl_fuse_scan_cmd,            NULL,            dnx_dram_firmware_aapl_fuse_scan_option_list,          &sh_dnx_firmware_aapl_fuse_scan_man,           NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"SoFT_LaNe_RePaiR",    sh_dnx_dram_firmware_aapl_soft_lane_repair_cmd,     NULL,            dnx_dram_firmware_aapl_soft_lane_repair_option_list,   &sh_dnx_firmware_aapl_soft_lane_repair_man,    NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {"PARAMeter_DIAG",      sh_dnx_dram_firmware_aapl_param_diag_cmd,           NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_param_diag_man},
    {"MBIST_DIAG",          sh_dnx_dram_firmware_aapl_mbist_diag_cmd,           NULL,            dnx_dram_firmware_aapl_option_list,                    &sh_dnx_firmware_aapl_mbist_diag_man,          NULL,       NULL,  SH_CMD_SKIP_EXEC},
    {NULL}
/* *INDENT-ON* */
};

/** man page for the 'dram firmware aapl' diag */
sh_sand_man_t sh_dnx_dram_firmware_aapl_man = {
    .brief = "DRAM PHY Firmware AAPL diagnostics and commands",
    .compatibility = "DRAM PHY Firmware AAPL diagnostics are not supported for devices without HBM PHY Firmware"
};
#endif /* INCLUDE_AAPL */
