/** \file diag_dnx_dram_firmware.c
 *
 * Diagnostics for dram firmware
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_dram.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/dram/dram.h>
#include <bcm_int/dnx/dram/hbmc/hbmc_firmware.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_dram.h"
#include "diag_dnx_dram_firmware.h"

/* *INDENT-OFF* */
/* { */
/** This enum has all the available firmware operations */
static sh_sand_enum_t sand_diag_dram_firmware_operation_enum[] = {
    /** String                           Value                                                        Description */
    {"reset_phy_and_hbm",                DNX_HBMC_FIRMWARE_OPERATION_RESET_PHY_AND_HBM,               ""},
    {"reset_phy_fifo_pointers",          DNX_HBMC_FIRMWARE_OPERATION_RESET_PHY_FIFO_POINTERS,         ""},
    {"reset_hbm",                        DNX_HBMC_FIRMWARE_OPERATION_RESET_HBM,                       ""},
    {"power_on_flow",                    DNX_HBMC_FIRMWARE_OPERATION_POWER_ON_FLOW,                   ""},
    {"bypass_test",                      DNX_HBMC_FIRMWARE_OPERATION_BYPASS_TEST,                     ""},
    {"read_device_id",                   DNX_HBMC_FIRMWARE_OPERATION_READ_DEVICE_ID,                  ""},
    {"aword_test",                       DNX_HBMC_FIRMWARE_OPERATION_AWORD_TEST,                      ""},
    {"aerr_test",                        DNX_HBMC_FIRMWARE_OPERATION_AERR_TEST,                       ""},
    {"dword_test",                       DNX_HBMC_FIRMWARE_OPERATION_DWORD_TEST,                      ""},
    {"derr_test",                        DNX_HBMC_FIRMWARE_OPERATION_DERR_TEST,                       ""},
    {"lane_repair",                      DNX_HBMC_FIRMWARE_OPERATION_LANE_REPAIR,                     ""},
    {"aword_ilb_test",                   DNX_HBMC_FIRMWARE_OPERATION_AWORD_ILB_TEST,                  ""},
    {"dword_ilb_test",                   DNX_HBMC_FIRMWARE_OPERATION_DWORD_ILB_TEST,                  ""},
    {"read_device_temp",                 DNX_HBMC_FIRMWARE_OPERATION_READ_DEVICE_TEMP,                ""},
    {"burn_hard_lane_repairs",           DNX_HBMC_FIRMWARE_OPERATION_BURN_HARD_LANE_REPAIRS,          ""},
    {"apply_hard_lane_repairs_to_phy",   DNX_HBMC_FIRMWARE_OPERATION_APPLY_HARD_LANE_REPAIRS_TO_PHY,  ""},
    {"run_samsung_mbist",                DNX_HBMC_FIRMWARE_OPERATION_RUN_SAMSUNG_MBIST,               ""},
    {"run_sk_hynix_mbist",               DNX_HBMC_FIRMWARE_OPERATION_RUN_SK_HYNIX_MBIST,              ""},
    {"reset_mode_registers",             DNX_HBMC_FIRMWARE_OPERATION_RESET_MODE_REGISTERS,            ""},
    {"reset_phy_config",                 DNX_HBMC_FIRMWARE_OPERATION_RESET_PHY_CONFIG,                ""},
    {"aword_slb_test",                   DNX_HBMC_FIRMWARE_OPERATION_AWORD_SLB_TEST,                  ""},
    {"dword_slb_test",                   DNX_HBMC_FIRMWARE_OPERATION_DWORD_SLB_TEST,                  ""},
    {"chipping_test",                    DNX_HBMC_FIRMWARE_OPERATION_CHIPPING_TEST,                   ""},
    {"reserved",                         DNX_HBMC_FIRMWARE_OPERATION_RESERVED,                        ""},
    {"aerr_ilb_test",                    DNX_HBMC_FIRMWARE_OPERATION_AERR_ILB_TEST,                   ""},
    {"aerr_slb_test",                    DNX_HBMC_FIRMWARE_OPERATION_AERR_SLB_TEST,                   ""},
    {"derr_ilb_test",                    DNX_HBMC_FIRMWARE_OPERATION_DERR_ILB_TEST,                   ""},
    {"derr_slb_test",                    DNX_HBMC_FIRMWARE_OPERATION_DERR_SLB_TEST,                   ""},
    {"run_mmt",                          DNX_HBMC_FIRMWARE_OPERATION_RUN_MMT,                         ""},
    {"lane_verify",                      DNX_HBMC_FIRMWARE_OPERATION_LANE_VERIFY,                     ""},
    {"start_mmt",                        DNX_HBMC_FIRMWARE_OPERATION_START_MMT,                       ""},
    {"stop_mmt",                         DNX_HBMC_FIRMWARE_OPERATION_STOP_MMT,                        ""},
    {"load_tmrs_code",                   DNX_HBMC_FIRMWARE_OPERATION_LOAD_TMRS_CODE,                  ""},
    {"cattrip_test",                     DNX_HBMC_FIRMWARE_OPERATION_CATTRIP_TEST,                    ""},
    {"hbm_run_cell_repair_mode",         DNX_HBMC_FIRMWARE_OPERATION_HBM_RUN_CELL_REPAIR_MODE,        ""},
    {"hbm_run_fuse_scan",                DNX_HBMC_FIRMWARE_OPERATION_HBM_RUN_FUSE_SCAN,               ""},
    {"hbm_report_lane_repairs",          DNX_HBMC_FIRMWARE_OPERATION_HBM_REPORT_LANE_REPAIRS,         ""},
    {"hbm_report_new_lane_repairs",      DNX_HBMC_FIRMWARE_OPERATION_HBM_REPORT_NEW_LANE_REPAIRS,     ""},
    {"hbm_skh_tm",                       DNX_HBMC_FIRMWARE_OPERATION_HBM_SKH_TM,                      ""},
    {"hbm_reapply_cell_repairs",         DNX_HBMC_FIRMWARE_OPERATION_HBM_REAPPLY_CELL_REPAIRS,        ""},
    {"hbm_repair_lane_repairs_by_mode",  DNX_HBMC_FIRMWARE_OPERATION_HBM_REPAIR_LANE_REPAIRS_BY_MODE, ""},
    {NULL}
};

/** Define dram 'max' */
static sh_sand_enum_t sand_diag_dram_firmware_enum[] = {
    /**String       Value                              Description                              Plugin String*/
    {"max",         0,                                 "Maximum dram index",                    "DNX_DATA.dram.general_info.max_nof_drams-1"},
    {NULL}
};

/** Define 'all' channels and 'max' channel */
static sh_sand_enum_t sand_diag_dram_firmware_channel_enum[] = {
    /**String       Value                              Description                              Plugin String*/
    {"all",         DNX_HBMC_FIRMWARE_ALL_CHANNELS,    "Run operation on all channels"},
    {"max",         0,                                 "Maximum channel index",                 "DNX_DATA.dram.general_info.nof_channels-1"},
    {NULL}
};

/** Params that can be used for the for the 'firmware operation' diag */
static sh_sand_option_t dnx_firmware_operation_option_list[] = {
    /*
     * Option name Option Type Option Description Option Default Option Extension Valid Range Flags 
     */
    {"dram",        SAL_FIELD_TYPE_UINT32, "dram interface",        "0-max",    (void *) sand_diag_dram_firmware_enum,           "0-max"},
    {"channel",     SAL_FIELD_TYPE_INT32,  "channel in interface",  "all",      (void *) sand_diag_dram_firmware_channel_enum,   "all-max"},
    {"operation",   SAL_FIELD_TYPE_UINT32, "firmware operation",    NULL,       (void *) sand_diag_dram_firmware_operation_enum, NULL},
    {NULL}
};

/** 'firmware operation' man page */
static sh_sand_man_t sh_dnx_firmware_operation_man = {
    .brief = "Run HBM PHY firmware operation\n",
    .full = "Run HBM PHY firmware operation.\n"
            "Please refer to the HBM PHY Firmware User Manual for more information.\n",
    .examples = "operation=0x0\n" \
                "operation=reset_phy_and_hbm"
};

/** Is device support HBM PHY Firmware */
shr_error_e
sh_cmd_is_hbm_with_apd_phy_available(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Skip check on init */
    if (!dnx_init_is_init_done_get(unit))
    {
        SHR_EXIT();
    }

    /** Currently only APD phy devices has firmware */
    if (!dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_apd_phy))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_operation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_index;
    uint32 dram_start;
    uint32 dram_end;
    int channel_index;
    dnx_hbmc_firmware_operation_e operation;
    shr_error_e rv;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);
    SH_SAND_GET_INT32("channel", channel_index);
    SH_SAND_GET_UINT32("operation", operation);

    for (dram_index = dram_start; dram_index <= dram_end; ++dram_index)
    {
        dnx_hbmc_firmware_operation_result_hbm_spare_t hbm_spare;
        uint32 nof_hbm_spares = dnx_data_dram.firmware.nof_hbm_spare_data_results_get(unit);
        int is_valid = 0;

        /** Skip non-valid drams */
        SHR_IF_ERR_EXIT(dnx_dram_index_is_valid(unit, dram_index, &is_valid));
        if (!is_valid)
        {
            LOG_CLI((BSL_META("Dram index %d is not valid - and is skipped\n"), dram_index));
            continue;
        }

        /** Run the requested firmware operation */
        rv = dnx_hbmc_firmware_launch_operation(unit, dram_index, channel_index, operation);
        /**
         * Some operations have results that can be printed.
         * For the following operations need to print results only when they fail.
         */
        if (SHR_FAILURE(rv)
            && (operation == DNX_HBMC_FIRMWARE_OPERATION_LANE_REPAIR
                || operation == DNX_HBMC_FIRMWARE_OPERATION_LANE_VERIFY))
        {
            uint32 nof_lanes_with_faults;
            uint32 lane_index;

            SHR_IF_ERR_EXIT(dnx_hbmc_firmware_operation_result_spare_get(unit, dram_index, nof_hbm_spares, &hbm_spare));
            nof_lanes_with_faults = hbm_spare.hbm_spare_data[0];

            PRT_TITLE_SET("Lanes with faults");
            PRT_COLUMN_ADD("Dram");
            PRT_COLUMN_ADD("Lane");
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("0x%x", dram_index);
            for (lane_index = 1; lane_index < nof_lanes_with_faults; ++lane_index)
            {
                PRT_CELL_SET("%d", hbm_spare.hbm_spare_data[lane_index]);
            }
            PRT_COMMITX;
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }

        /**
         * Some operations have results that can be printed.
         * See firmware PG to see which operations have results and how
         * to analyze them.
         */
        SHR_IF_ERR_EXIT(dnx_hbmc_firmware_operation_result_spare_get(unit, dram_index, nof_hbm_spares, &hbm_spare));
        switch (operation)
        {
            /** Print vendor info */
            case DNX_HBMC_FIRMWARE_OPERATION_READ_DEVICE_ID:
            {
                dnx_dram_vendor_info_t vendor_info;
                SHR_IF_ERR_EXIT(dnx_dram_vendor_info_get(unit, dram_index, &vendor_info));
                PRT_TITLE_SET("Dram Vendor Info");
                SHR_IF_ERR_EXIT(sh_dnx_dram_vendor_info_print(unit, dram_index, &vendor_info, prt_ctr));
                PRT_COMMITX;
                break;
            }
            /** Report lanes with repairs */
            case DNX_HBMC_FIRMWARE_OPERATION_HBM_REPORT_LANE_REPAIRS:
            case DNX_HBMC_FIRMWARE_OPERATION_HBM_REPORT_NEW_LANE_REPAIRS:
            {
                uint32 nof_lanes_with_repairs = hbm_spare.hbm_spare_data[0];
                uint32 lane_index;
                PRT_TITLE_SET("Lanes with repairs");
                PRT_COLUMN_ADD("Dram");
                PRT_COLUMN_ADD("Lane");
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("0x%x", dram_index);
                for (lane_index = 1; lane_index < nof_lanes_with_repairs; ++lane_index)
                {
                    PRT_CELL_SET("%d", hbm_spare.hbm_spare_data[lane_index]);
                }
                PRT_COMMITX;
                break;
            }
            /** Print device temperature */
            case DNX_HBMC_FIRMWARE_OPERATION_READ_DEVICE_TEMP:
            {
                uint32 hbm_temperature = hbm_spare.hbm_spare_data[0];
                PRT_TITLE_SET("Dram Temperature");
                PRT_COLUMN_ADD("Dram");
                PRT_COLUMN_ADD("Temperature");
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("0x%x", dram_index);
                PRT_CELL_SET("%d C ", hbm_temperature);
                PRT_COMMITX;
                break;
            }
            /** Print the number of Samsung MBIST repairs */
            case DNX_HBMC_FIRMWARE_OPERATION_RUN_SAMSUNG_MBIST:
            {
                uint32 nof_mbist_repairs = hbm_spare.hbm_spare_data[0];
                PRT_TITLE_SET("Number of MBIST repairs");
                PRT_COLUMN_ADD("Dram");
                PRT_COLUMN_ADD("Number of repairs");
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("0x%x", dram_index);
                PRT_CELL_SET("%d", nof_mbist_repairs);
                PRT_COMMITX;
                break;
            }
            /** Print number of repairs per mode */
            case DNX_HBMC_FIRMWARE_OPERATION_HBM_REPAIR_LANE_REPAIRS_BY_MODE:
            {
                PRT_TITLE_SET("Repairs count");
                PRT_COLUMN_ADD("Dram");
                PRT_COLUMN_ADD("Total lane repair");
                PRT_COLUMN_ADD("Dword mode1 repair");
                PRT_COLUMN_ADD("Dword mode2 repair");
                PRT_COLUMN_ADD("Aword repair");
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("0x%x", dram_index);
                PRT_CELL_SET("%d", hbm_spare.hbm_spare_data[0]);
                PRT_CELL_SET("%d", hbm_spare.hbm_spare_data[1]);
                PRT_CELL_SET("%d", hbm_spare.hbm_spare_data[2]);
                PRT_CELL_SET("%d", hbm_spare.hbm_spare_data[3]);
                PRT_COMMITX;
                break;
            }
            default:
            {
                /** Nothing to do since other operations don't have results */
                break;
            }
        }
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** This enum has all the available firmware operations */
static sh_sand_enum_t sand_diag_dram_firmware_parameter_enum[] = {
    /** String                                  Value                                                           Description */
    {"hbm_reserved",                             DNX_HBMC_FIRMWARE_PARAM_HBM_RESERVED,                           ""},
    {"hbm_max_timeout",                          DNX_HBMC_FIRMWARE_PARAM_HBM_MAX_TIMEOUT,                        ""},
    {"hbm_tinit1_cycles",                        DNX_HBMC_FIRMWARE_PARAM_HBM_TINIT1_CYCLES,                      ""},
    {"hbm_tinit2_cycles",                        DNX_HBMC_FIRMWARE_PARAM_HBM_TINIT2_CYCLES,                      ""},
    {"hbm_tinit3_cycles",                        DNX_HBMC_FIRMWARE_PARAM_HBM_TINIT3_CYCLES,                      ""},
    {"hbm_tinit4_cycles",                        DNX_HBMC_FIRMWARE_PARAM_HBM_TINIT4_CYCLES,                      ""},
    {"hbm_tinit5_cycles",                        DNX_HBMC_FIRMWARE_PARAM_HBM_TINIT5_CYCLES,                      ""},
    {"hbm_rw_latency_offset",                    DNX_HBMC_FIRMWARE_PARAM_HBM_RW_LATENCY_OFFSET,                  ""},
    {"hbm_latency_odd_n_even",                   DNX_HBMC_FIRMWARE_PARAM_HBM_LATENCY_ODD_N_EVEN,                 ""},
    {"hbm_mode_register0",                       DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER0,                     ""},
    {"hbm_mode_register1",                       DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER1,                     ""},
    {"hbm_mode_register2",                       DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER2,                     ""},
    {"hbm_mode_register3",                       DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER3,                     ""},
    {"hbm_mode_register4",                       DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER4,                     ""},
    {"hbm_mode_register5",                       DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER5,                     ""},
    {"hbm_mode_register6",                       DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER6,                     ""},
    {"hbm_mode_register7",                       DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER7,                     ""},
    {"hbm_mode_register8",                       DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER8,                     ""},
    {"hbm_phy_config0",                          DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG0,                        ""},
    {"hbm_phy_config1",                          DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG1,                        ""},
    {"hbm_phy_config2",                          DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG2,                        ""},
    {"hbm_phy_config3",                          DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG3,                        ""},
    {"hbm_phy_config4",                          DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG4,                        ""},
    {"hbm_phy_config5",                          DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG5,                        ""},
    {"hbm_phy_config6",                          DNX_HBMC_FIRMWARE_PARAM_HBM_PHY_CONFIG6,                        ""},
    {"hbm_lbp_drv_imp",                          DNX_HBMC_FIRMWARE_PARAM_HBM_LBP_DRV_IMP,                        ""},
    {"hbm_delay_config_dll",                     DNX_HBMC_FIRMWARE_PARAM_HBM_DELAY_CONFIG_DLL,                   ""},
    {"hbm_ignore_phyupd_handshake",              DNX_HBMC_FIRMWARE_PARAM_HBM_IGNORE_PHYUPD_HANDSHAKE,            ""},
    {"hbm_tupdmrs_cycles",                       DNX_HBMC_FIRMWARE_PARAM_HBM_TUPDMRS_CYCLES,                     ""},
    {"hbm_t_rdlat_offset",                       DNX_HBMC_FIRMWARE_PARAM_HBM_T_RDLAT_OFFSET,                     ""},
    {"hbm_mbist_repair_mode",                    DNX_HBMC_FIRMWARE_PARAM_HBM_MBIST_REPAIR_MODE,                  ""},
    {"hbm_mbist_pattern",                        DNX_HBMC_FIRMWARE_PARAM_HBM_MBIST_PATTERN,                      ""},
    {"hbm_samsung_mbist_hard_repair_cycles",     DNX_HBMC_FIRMWARE_PARAM_HBM_SAMSUNG_MBIST_HARD_REPAIR_CYCLES,   ""},
    {"hbm_hard_lane_repair_cycles",              DNX_HBMC_FIRMWARE_PARAM_HBM_HARD_LANE_REPAIR_CYCLES,            ""},
    {"hbm_mbist_bank_address_end",               DNX_HBMC_FIRMWARE_PARAM_HBM_MBIST_BANK_ADDRESS_END,             ""},
    {"hbm_mbist_row_address_end",                DNX_HBMC_FIRMWARE_PARAM_HBM_MBIST_ROW_ADDRESS_END,              ""},
    {"hbm_mbist_column_address_end",             DNX_HBMC_FIRMWARE_PARAM_HBM_MBIST_COLUMN_ADDRESS_END,           ""},
    {"hbm_freq",                                 DNX_HBMC_FIRMWARE_PARAM_HBM_FREQ,                               ""},
    {"hbm_div_mode",                             DNX_HBMC_FIRMWARE_PARAM_HBM_DIV_MODE,                           ""},
    {"hbm_cke_exit_state",                       DNX_HBMC_FIRMWARE_PARAM_HBM_CKE_EXIT_STATE,                     ""},
    {"hbm_test_mode_register0",                  DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER0,                ""},
    {"hbm_test_mode_register1",                  DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER1,                ""},
    {"hbm_test_mode_register2",                  DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER2,                ""},
    {"hbm_test_mode_register3",                  DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER3,                ""},
    {"hbm_test_mode_register4",                  DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER4,                ""},
    {"hbm_test_mode_register5",                  DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER5,                ""},
    {"hbm_test_mode_register6",                  DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER6,                ""},
    {"hbm_test_mode_register7",                  DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER7,                ""},
    {"hbm_test_mode_register8",                  DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER8,                ""},
    {"mmt_dword_misr0",                          DNX_HBMC_FIRMWARE_PARAM_MMT_DWORD_MISR0,                        ""},
    {"mmt_dword_misr1",                          DNX_HBMC_FIRMWARE_PARAM_MMT_DWORD_MISR1,                        ""},
    {"mmt_dword_misr2",                          DNX_HBMC_FIRMWARE_PARAM_MMT_DWORD_MISR2,                        ""},
    {"mmt_dword_misr3",                          DNX_HBMC_FIRMWARE_PARAM_MMT_DWORD_MISR3,                        ""},
    {"mmt_dword_misr4",                          DNX_HBMC_FIRMWARE_PARAM_MMT_DWORD_MISR4,                        ""},
    {"mmt_lfsr_compare_sticky",                  DNX_HBMC_FIRMWARE_PARAM_MMT_LFSR_COMPARE_STICKY,                ""},
    {"hbm_test_t_rdlat_offset",                  DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_T_RDLAT_OFFSET,                ""},
    {"hbm_mode_register15",                      DNX_HBMC_FIRMWARE_PARAM_HBM_MODE_REGISTER15,                    ""},
    {"hbm_test_mode_register15",                 DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_MODE_REGISTER15,               ""},
    {"hbm_bypass_testmode_reset",                DNX_HBMC_FIRMWARE_PARAM_HBM_BYPASS_TESTMODE_RESET,              ""},
    {"hbm_disable_addr_lane_repair",             DNX_HBMC_FIRMWARE_PARAM_HBM_DISABLE_ADDR_LANE_REPAIR,           ""},
    {"hbm_mmt_configuration",                    DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_CONFIGURATION,                  ""},
    {"hbm_bypass_repair_on_reset",               DNX_HBMC_FIRMWARE_PARAM_HBM_BYPASS_REPAIR_ON_RESET,             ""},
    {"hbm_stack_height",                         DNX_HBMC_FIRMWARE_PARAM_HBM_STACK_HEIGHT,                       ""},
    {"hbm_manufacturing_id",                     DNX_HBMC_FIRMWARE_PARAM_HBM_MANUFACTURING_ID,                   ""},
    {"hbm_density",                              DNX_HBMC_FIRMWARE_PARAM_HBM_DENSITY,                            ""},
    {"hbm_manually_configure_id",                DNX_HBMC_FIRMWARE_PARAM_HBM_MANUALLY_CONFIGURE_ID,              ""},
    {"hbm_parity_latency",                       DNX_HBMC_FIRMWARE_PARAM_HBM_PARITY_LATENCY,                     ""},
    {"hbm_test_parity_latency",                  DNX_HBMC_FIRMWARE_PARAM_HBM_TEST_PARITY_LATENCY,                ""},
    {"mmt_config_dword0",                        DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD0,                      ""},
    {"mmt_config_dword1",                        DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD1,                      ""},
    {"mmt_config_dword2",                        DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD2,                      ""},
    {"mmt_config_dword3",                        DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD3,                      ""},
    {"mmt_config_dword4",                        DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD4,                      ""},
    {"mmt_config_dword5",                        DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD5,                      ""},
    {"mmt_config_dword6",                        DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_DWORD6,                      ""},
    {"mmt_config_aword0",                        DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_AWORD0,                      ""},
    {"mmt_config_aword1",                        DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_AWORD1,                      ""},
    {"mmt_config_aword2",                        DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_AWORD2,                      ""},
    {"mmt_config_aword3",                        DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_AWORD3,                      ""},
    {"mmt_config_aword4",                        DNX_HBMC_FIRMWARE_PARAM_MMT_CONFIG_AWORD4,                      ""},
    {"mmt_aword_misr0",                          DNX_HBMC_FIRMWARE_PARAM_MMT_AWORD_MISR0,                        ""},
    {"mmt_aword_misr1",                          DNX_HBMC_FIRMWARE_PARAM_MMT_AWORD_MISR1,                        ""},
    {"mmt_t_rdlat_offset",                       DNX_HBMC_FIRMWARE_PARAM_MMT_T_RDLAT_OFFSET,                     ""},
    {"mmt_mode_register0",                       DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER0,                     ""},
    {"mmt_mode_register1",                       DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER1,                     ""},
    {"mmt_mode_register2",                       DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER2,                     ""},
    {"mmt_mode_register3",                       DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER3,                     ""},
    {"mmt_mode_register4",                       DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER4,                     ""},
    {"mmt_mode_register5",                       DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER5,                     ""},
    {"mmt_mode_register6",                       DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER6,                     ""},
    {"mmt_mode_register7",                       DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER7,                     ""},
    {"mmt_mode_register8",                       DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER8,                     ""},
    {"mmt_mode_register16",                      DNX_HBMC_FIRMWARE_PARAM_MMT_MODE_REGISTER16,                    ""},
    {"hbm_samsung_mbist_workaround",             DNX_HBMC_FIRMWARE_PARAM_HBM_SAMSUNG_MBIST_WORKAROUND,           ""},
    {"hbm_dword_op",                             DNX_HBMC_FIRMWARE_PARAM_HBM_DWORD_OP,                           ""},
    {"hbm_mmt_tcr_setting",                      DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_TCR_SETTING,                    ""},
    {"hbm_mmt_supply_conditioning",              DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_SUPPLY_CONDITIONING,            ""},
    {"hbm_model_number",                         DNX_HBMC_FIRMWARE_PARAM_HBM_MODEL_NUMBER,                       ""},
    {"hbm_date_code",                            DNX_HBMC_FIRMWARE_PARAM_HBM_DATE_CODE,                          ""},
    {"hbm_mbist_verify",                         DNX_HBMC_FIRMWARE_PARAM_HBM_MBIST_VERIFY,                       ""},
    {"hbm_save_soft_lane_repairs",               DNX_HBMC_FIRMWARE_PARAM_HBM_SAVE_SOFT_LANE_REPAIRS,             ""},
    {"hbm_mmt_read_repeat",                      DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_READ_REPEAT,                    ""},
    {"hbm_mmt_column_range",                     DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_COLUMN_RANGE,                   ""},
    {"hbm_mmt_burst_length",                     DNX_HBMC_FIRMWARE_PARAM_HBM_MMT_BURST_LENGTH,                   ""},
    {"hbm_aword_mode",                           DNX_HBMC_FIRMWARE_PARAM_HBM_AWORD_MODE,                         ""},
    {"hbm_spico_freq",                           DNX_HBMC_FIRMWARE_PARAM_HBM_SPICO_FREQ,                         ""},
    {"hbm_tm0",                                  DNX_HBMC_FIRMWARE_PARAM_HBM_TM0,                                ""},
    {"hbm_tm1",                                  DNX_HBMC_FIRMWARE_PARAM_HBM_TM1,                                ""},
    {"hbm_tm2",                                  DNX_HBMC_FIRMWARE_PARAM_HBM_TM2,                                ""},
    {"hbm_tm3",                                  DNX_HBMC_FIRMWARE_PARAM_HBM_TM3,                                ""},
    {"hbm_tm4",                                  DNX_HBMC_FIRMWARE_PARAM_HBM_TM4,                                ""},
    {"hbm_mode1_lane_repair_limit",              DNX_HBMC_FIRMWARE_PARAM_HBM_MODE1_LANE_REPAIR_LIMIT,            ""},
    {"hbm_total_lane_repair_limit",              DNX_HBMC_FIRMWARE_PARAM_HBM_TOTAL_LANE_REPAIR_LIMIT,            ""},
    {NULL}
};

/** Params that can be used for the for the 'firmware parameter set' diag */
static sh_sand_option_t dnx_firmware_parameter_set_option_list[] = {
    /*
     * Option name | Option Type            | Option Description      | Option Default | Option Extension                               | Valid Range
     */
    {"dram",         SAL_FIELD_TYPE_UINT32,   "dram interface",         "0-max",         (void *) sand_diag_dram_firmware_enum,           "0-max"},
    {"parameter",    SAL_FIELD_TYPE_UINT32,   "firmware parameter",     NULL,            (void *) sand_diag_dram_firmware_parameter_enum, NULL},
    {"value",        SAL_FIELD_TYPE_UINT32,   "parameter value",        NULL,            NULL,                                            NULL},
    {NULL}
};

/** Params that can be used for the for the 'firmware parameter set' diag */
static sh_sand_option_t dnx_firmware_parameter_get_option_list[] = {
    /*
     * Option name | Option Type            | Option Description      | Option Default | Option Extension                               | Valid Range
     */
    {"dram",         SAL_FIELD_TYPE_UINT32,   "dram interface",         "0-max",         (void *) sand_diag_dram_firmware_enum,           "0-max"},
    {"parameter",    SAL_FIELD_TYPE_UINT32,   "firmware parameter",     NULL,            (void *) sand_diag_dram_firmware_parameter_enum, NULL},
    {NULL}
};

/** 'firmware parameter' man page */
static sh_sand_man_t sh_dnx_firmware_parameter_man = {
    .brief = "Set/Get HBM PHY firmware parameter\n",
};

/** 'firmware parameter set' man page */
static sh_sand_man_t sh_dnx_firmware_parameter_set_man = {
    .brief = "Set HBM PHY firmware parameter\n",
    .full = "Set HBM PHY firmware parameter.\n"
            "Please refer to the HBM PHY Firmware User Manual for more information.\n",
    .examples = "parameter=0x0 value=0x0\n"
};

/** 'firmware parameter get' man page */
static sh_sand_man_t sh_dnx_firmware_parameter_get_man = {
    .brief = "Get HBM PHY firmware parameter\n",
    .full = "Get HBM PHY firmware parameter.\n"
            "Please refer to the HBM PHY Firmware User Manual for more information.\n",
    .examples = "parameter=0x0\n"
};

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_parameter_set_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_index;
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_param_e parameter;
    uint32 value;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);
    SH_SAND_GET_UINT32("parameter", parameter);
    SH_SAND_GET_UINT32("value", value);

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

        /** Set the parameter value */
        SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_set(unit, dram_index, parameter, value));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_parameter_get_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_index;
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_param_e parameter;
    uint32 value;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);
    SH_SAND_GET_UINT32("parameter", parameter);

    PRT_TITLE_SET("Firmware parameter value");
    PRT_COLUMN_ADD("Dram");
    PRT_COLUMN_ADD("Parameter");
    PRT_COLUMN_ADD("Value");

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

        /** Set the parameter value */
        SHR_IF_ERR_EXIT(dnx_hbmc_firmware_param_get(unit, dram_index, parameter, &value));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("0x%x", dram_index);
        PRT_CELL_SET("%s", SH_SAND_GET_ENUM_STR("parameter", parameter));
        PRT_CELL_SET("%d", value);
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_cmd_t sh_dnx_dram_firmware_parameter_cmds[] = {
    /* Name  | Leaf Action                               | Junction Array | Options for Leaf                          | Usage */
    {"Set",    sh_dnx_dram_firmware_parameter_set_cmd,     NULL,            dnx_firmware_parameter_set_option_list,     &sh_dnx_firmware_parameter_set_man},
    {"Get",    sh_dnx_dram_firmware_parameter_get_cmd,     NULL,            dnx_firmware_parameter_get_option_list,     &sh_dnx_firmware_parameter_get_man},
    {NULL}
};

/** Params that can be used for the for the 'firmware info' diag */
static sh_sand_option_t dnx_firmware_info_option_list[] = {
    /*
     * Option name | Option Type            | Option Description      | Option Default | Option Extension                               | Valid Range
     */
    {"dram",         SAL_FIELD_TYPE_UINT32,   "dram interface",         "0-max",         (void *) sand_diag_dram_firmware_enum,           "0-max"},
    {NULL}
};

/** 'firmware info' man page */
static sh_sand_man_t sh_dnx_firmware_info_man = {
    .brief = "HBM PHY firmware info\n",
    .full = "HBM PHY firmware info\n"
            "Please refer to the HBM PHY Firmware User Manual for more information.\n",
    .examples = "dram=0\n"
};

/** see man section */
static shr_error_e
sh_dnx_dram_firmware_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_index;
    uint32 dram_start;
    uint32 dram_end;
    dnx_hbmc_firmware_info_t firmware_info;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);

    PRT_TITLE_SET("Firmware information");
    PRT_COLUMN_ADD("Dram");
    PRT_COLUMN_ADD("Revision");
    PRT_COLUMN_ADD("Build");

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

        /** Get firmware info */
        SHR_IF_ERR_EXIT(dnx_hbmc_firmware_info_get(unit, dram_index, &firmware_info));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("0x%x", dram_index);
        PRT_CELL_SET("0x%x", firmware_info.revision);
        PRT_CELL_SET("0x%x", firmware_info.build_id);
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** Commands supported by the 'dram firmware' diag */
sh_sand_cmd_t sh_dnx_dram_firmware_cmds[] = {
    /** Name      | Leaf Action                         | Junction Array                       | Options for Leaf                     | Usage */
    {"OPeRation",   sh_dnx_dram_firmware_operation_cmd,   NULL,                                  dnx_firmware_operation_option_list,    &sh_dnx_firmware_operation_man},
    {"PARAMeter",   NULL,                                 sh_dnx_dram_firmware_parameter_cmds,   NULL,                                  &sh_dnx_firmware_parameter_man},
    {"INFo",        sh_dnx_dram_firmware_info_cmd,        NULL,                                  dnx_firmware_info_option_list,         &sh_dnx_firmware_info_man},
    {NULL}
};

/** man page for the 'dram firmware' diag */
sh_sand_man_t sh_dnx_dram_firmware_man = {
    .brief = "DRAM PHY Firmware diagnostics and commands",
    .compatibility = "DRAM PHY Firmware diagnostics are not supported for devices without HBM PHY Firmware"
};
/* } */
/* *INDENT-ON* */
