/** \file diag_dnx_nif_status.c
 *
 * diagnostics for NIF Status
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
 * Include files.
 * {
 */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/diag.h>
#include <sal/core/boot.h>

#include <shared/util.h>
#include <shared/shrextend/shrextend_debug.h>

#include <sal/appl/sal.h>

#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_verify.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/synce/synce.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#include <soc/dnx/pll/pll.h>
#include <soc/dnx/dbal/dbal.h>

#include "diag_dnx_nif_lane_map.h"

/*
 * }
 */

/*
 * Macros
 * {
 */
#define  DNX_DIAG_NIF_STATUS_MAX_STRING_LEN    32
 /*
  * }
  */

/**
 * \brief - Convert phy pbmp to lane string
 *     For example:
 *       0x2F ---> 00-03, 05
 */
static char *
sh_dnx_phy_pbmp_to_lane_string(
    bcm_pbmp_t * pbmp,
    int is_over_fabric,
    char *lane_str)
{
    int is_range = 0, range_num = 0;
    int last_added = -1;
    int lane;
    char tmp_str1[DNX_DIAG_NIF_STATUS_MAX_STRING_LEN] = "";
    char tmp_str2[DNX_DIAG_NIF_STATUS_MAX_STRING_LEN] = "";

    BCM_PBMP_ITER(*pbmp, lane)
    {
        /*
         * Handle the first lane.
         */
        if (last_added == -1)
        {
            last_added = lane;
            if (is_over_fabric)
            {
                sal_sprintf(tmp_str1, "fabric:%02u", last_added);
            }
            else
            {
                sal_sprintf(tmp_str1, "%02u", last_added);
            }
            sal_strncat(tmp_str2, tmp_str1, sizeof(tmp_str2) - sal_strlen(tmp_str2) - 1);
            continue;
        }
        /*
         * Handle range.
         * If the lane numbers are range, for example 4,5,6,7
         * expect to print a string "04-07".
         */
        if (last_added == lane - 1)
        {
            last_added = lane;
            is_range = 1;
            ++range_num;
            continue;
        }
        else
        {
            is_range = 0;
        }
        /*
         * End of one range
         */
        if (range_num >= 1)
        {
            sal_sprintf(tmp_str1, "-%02u", last_added);
            sal_strncat(tmp_str2, tmp_str1, sizeof(tmp_str2) - sal_strlen(tmp_str2) - 1);
        }
        last_added = lane;
        sal_sprintf(tmp_str1, ", %02u", last_added);
        sal_strncat(tmp_str2, tmp_str1, sizeof(tmp_str2) - sal_strlen(tmp_str2) - 1);
        range_num = 0;
    }
    /*
     * Handle the last range
     */
    if (is_range)
    {
        sal_sprintf(tmp_str1, "-%02u", last_added);
        sal_strncat(tmp_str2, tmp_str1, sizeof(tmp_str2) - sal_strlen(tmp_str2) - 1);
    }
    sal_snprintf(lane_str, DNX_DIAG_NIF_STATUS_MAX_STRING_LEN, "%s", tmp_str2);

    return lane_str;
}

/**
 * \brief - Dump the NIF status info
 *
 * \param [in] unit - chip unit id
 * \param [in] args - diag command arguments
 * \param [in] sand_control - diag comman control
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

shr_error_e
sh_dnx_nif_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int no_rates = 0, speed, lane_speed;
    int lane, is_over_fabric, phy_count;
    int port_enable, is_pam4 = 0;
    int is_signal_lock, pll_index, synce_enable;
    bcm_core_t core;
    bcm_port_t port, lane_port, master_port;
    dnx_algo_port_info_s port_info;
    bcm_pbmp_t nif_pbmp, nif_phys;
    bcm_port_resource_t port_resource;
    soc_dnxc_port_phy_measure_t phy_measure;
    DNX_SERDES_REF_CLOCK_TYPE ref_clk_index;
    uint32 serdes_freq_arr[SOC_MAX_NUM_PORTS];
    uint32 serdes_freq_remain_arr[SOC_MAX_NUM_PORTS];
    uint32 lane_seq_done = 0, rx_seq_done;
    char *ref_clk_str[] = { "125", "156.25", "312.5" };
    char lane_str[DNX_DIAG_NIF_STATUS_MAX_STRING_LEN];
    char port_info_str[DNX_DIAG_NIF_STATUS_MAX_STRING_LEN];
    char speed_str[DNX_DIAG_NIF_STATUS_MAX_STRING_LEN];
    char serdes_freq_str[DNX_DIAG_NIF_STATUS_MAX_STRING_LEN];
    char comment_str[DNX_DIAG_NIF_STATUS_MAX_STRING_LEN];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("no_rates", no_rates);

    /*
     * Print table header
     */
    PRT_TITLE_SET("NIF Status:");

    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Port Type");
    PRT_COLUMN_ADD("Lanes");
    PRT_COLUMN_ADD("PLL Lock");
    PRT_COLUMN_ADD("Link up (Signal Lock)");
    PRT_COLUMN_ADD("SerDes Rate [GBps]");
    PRT_COLUMN_ADD("Ref Clock [Mhz]");
    PRT_COLUMN_ADD("Comments");

    if (!dnx_init_is_init_done_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm init not done!");
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &nif_pbmp));

    for (port = 0; port < SOC_MAX_NUM_PORTS; ++port)
    {
        serdes_freq_arr[port] = -1;
        serdes_freq_remain_arr[port] = -1;
    }

    BCM_PBMP_ITER(nif_pbmp, port)
    {

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        /*
         * Get the port speed and type
         */
        SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &port_resource));
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, TRUE)
            || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
        {
            sal_sprintf(port_info_str, "ETH %s", if_fmt_speed(speed_str, port_resource.speed));
        }
        else
        {
            sal_sprintf(port_info_str, "ILKN %s", if_fmt_speed(speed_str, port_resource.speed));
        }
        /*
         * Get NIF phys
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &nif_phys));

        /*
         * Get the PLL Lock (Rx sequence done) info
         */
        phy_count = 0;
        rx_seq_done = 0;
        BCM_PBMP_ITER(nif_phys, lane)
        {
            /*
             * Get the per lane gport. Then check the rx seq done info for each lane.
             */
            BCM_PHY_GPORT_LANE_PORT_SET(lane_port, phy_count, port);
            SHR_IF_ERR_EXIT(bcm_port_phy_control_get
                            (unit, lane_port, BCM_PORT_PHY_CONTROL_RX_SEQ_DONE, &lane_seq_done));
            rx_seq_done |= lane_seq_done << phy_count;
            phy_count++;
        }
        /*
         * Check if the signalling mode is PAM4
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &speed));
        lane_speed = DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1) ? speed : (speed / phy_count);
        is_pam4 = DNXC_PORT_PHY_SPEED_IS_PAM4(lane_speed);

        /*
         * Get the Signal Lock info
         */
        SHR_IF_ERR_EXIT(bcm_port_link_status_get(unit, port, &is_signal_lock));

        /*
         * Measure Serdes rate
         */
        SHR_IF_ERR_EXIT(bcm_port_enable_get(unit, port, &port_enable));

        is_over_fabric = 0;
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
        }

        /*
         * Get SyncE enable status
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_nif_synce_enable_get(unit, core, &synce_enable));

        if (!no_rates && port_enable && !synce_enable && !SAL_BOOT_PLISIM)
        {
            /** Get the master port */
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
            /*
             * For channelized ports, only need to calculate the serdes rate for Master port
             * serdes_freq_arr[master_port] == -1 means the serdes rate for master has not been calculated.
             * serdes_freq_arr[master_port] != -1 means the serdes rate for master has been calculated, no
             * need to calculate again.
             */
            if (serdes_freq_arr[master_port] == -1)
            {
                SHR_IF_ERR_EXIT(dnx_port_phy_measure_get(unit, port, 0, &phy_measure));
                serdes_freq_arr[master_port] = phy_measure.serdes_freq_int;
                serdes_freq_remain_arr[master_port] = phy_measure.serdes_freq_remainder;
            }
            sal_sprintf(serdes_freq_str, "%d.%03d", serdes_freq_arr[master_port], serdes_freq_remain_arr[master_port]);
        }
        else
        {
            sal_sprintf(serdes_freq_str, "N/A");
        }

        /*
         * Reference clock
         */
        if (dnx_data_pll.pll3.nof_pll3_get(unit) > 0)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_pll_index_get(unit, port, &pll_index));
        }
        else
        {
            pll_index = 0;
        }
        ref_clk_index = dnx_data_pll.general.nif_pll_get(unit, pll_index)->out_freq;

        if (ref_clk_index == DNX_SERDES_REF_CLOCK_BYPASS)
        {
            /*
             * If out_freq is Bypassed, it means the out_freq is equal to the in_freq.
             */
            ref_clk_index = dnx_data_pll.general.nif_pll_get(unit, pll_index)->in_freq;
        }

        /*
         * Additional Comments
         */
        sal_memset(comment_str, 0, sizeof(comment_str));
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info))
        {
            utilex_str_append(comment_str, "ELK");
        }
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info))
        {
            utilex_str_append(comment_str, "STIF");
        }
        if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
        {
            utilex_str_append(comment_str, "FLEXE_PHY");
        }
        if (is_over_fabric)
        {
            utilex_str_append(comment_str, "OVER_FABRIC");
        }

        if (is_pam4)
        {
            utilex_str_append(comment_str, "PAM4");
        }

        if (DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info))
        {
            utilex_str_append(comment_str, "CROSS_CONNECT");
        }
        utilex_str_escape(comment_str, ',');

        PRT_CELL_SET("%d", port);
        PRT_CELL_SET("%s", port_info_str);
        PRT_CELL_SET("%s", sh_dnx_phy_pbmp_to_lane_string(&nif_phys, is_over_fabric, lane_str));
        PRT_CELL_SET("0x%x", rx_seq_done);
        PRT_CELL_SET("%s", is_signal_lock ? "+" : "-");
        PRT_CELL_SET("%s", serdes_freq_str);
        PRT_CELL_SET("%s", ref_clk_str[ref_clk_index]);
        PRT_CELL_SET("%s", comment_str);
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief DNX NIF Status diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for NIF lane map diagnostic commands
 */

/* *INDENT-OFF* */

sh_sand_man_t sh_dnx_nif_status_man = {
    .brief    = "Diagnostic for printing the NIF status for all Ethernet and ILKN ports.",
    .full     = "Dump the NIF status for all the Eth and ILKN ports, including port type, "
                "Lane info, PLL status, Link status, Serdes rate, Reference clock and "
                "some other comments for the ports.",
    .synopsis = "[No_Rates]",
    .examples = "\n"
                "no_rates\n"
                "nr"
};

sh_sand_option_t sh_dnx_nif_status_options[] = {
    {"no_rates",  SAL_FIELD_TYPE_BOOL,  "Decide if calculating the serdes rate",  "false", NULL},
    {NULL}
};

/* *INDENT-ON* */
/*
 * }
 */
#undef _ERR_MSG_MODULE_NAME
