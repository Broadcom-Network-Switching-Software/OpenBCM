/** \file diag_dnx_nif_txrx.c
 *
 * diagnostics for NIF Rx & NIF Tx
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

#include <shared/util.h>
#include <shared/shrextend/shrextend_debug.h>

#include <sal/appl/sal.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/imb/imb.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>

#include "diag_dnx_nif_txrx.h"

/*
 * }
 */

/*
 * Macros
 * {
 */

#define  DNX_DIAG_NIF_TXRX_MAX_STR_LEN    16

 /*
  * }
  */

/**
 * \brief - Dump the NIF Tx Info
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
sh_dnx_nif_tx_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t nif_ports;
    uint32 pm_credits;
    uint32 max_occupancy;
    uint32 fifo_level;
    dnx_algo_port_info_s port_info;
    bcm_port_t logical_port;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("port", nif_ports);

    /*
     * Print table header
     */
    PRT_TITLE_SET("NIF TX INFO:");

    PRT_COLUMN_ADD("Logical Port");
    PRT_COLUMN_ADD("Credits From PM");
    PRT_COLUMN_ADD("Max Occupancy");
    PRT_COLUMN_ADD("Current FIFO Level");

    BCM_PBMP_ITER(nif_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, TRUE))
        {
            continue;
        }
        SHR_IF_ERR_EXIT(imb_port_tx_fifo_status_get(unit, logical_port, &max_occupancy, &fifo_level, &pm_credits));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%d", logical_port);
        PRT_CELL_SET("%d", pm_credits);
        PRT_CELL_SET("%d", max_occupancy);
        PRT_CELL_SET("%d", fifo_level);
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Dump the NIF Rx Info
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
sh_dnx_nif_rx_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t nif_ports;
    uint32 max_occupancy;
    uint32 fifo_level;

    uint32 src_prio;
    bcm_port_nif_scheduler_t sch_prio;
    bcm_port_prio_config_t priority_config;
    int rmc_idx, nof_entries;

    dnx_algo_port_info_s port_info;
    bcm_port_t logical_port;
    dnx_algo_port_ethu_access_info_t cdu_info;
    dnx_algo_port_rmc_info_t rmc_info;
    char *sch_prio_str[] = { "Low", "High", "TDM" };
    char prd_prio_str[DNX_DIAG_NIF_TXRX_MAX_STR_LEN];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("port", nif_ports);

    /*
     * Print table header
     */
    PRT_TITLE_SET("NIF RX INFO:");

    PRT_COLUMN_ADD("Logical Port");
    PRT_COLUMN_ADD("Block Index");
    PRT_COLUMN_ADD("Logical FIFO Index");
    PRT_COLUMN_ADD("FIFO Sch Priority");
    PRT_COLUMN_ADD("FIFO PRD Priority");
    PRT_COLUMN_ADD("FIFO Size (in entries)");
    PRT_COLUMN_ADD("Max Occupancy");
    PRT_COLUMN_ADD("Current FIFO Level");

    BCM_PBMP_ITER(nif_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, FALSE))
        {
            continue;
        }

        /** This condition is used to filter ports that don't have rx path (like stif, unlike stif_data) */
        if (!DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, logical_port, &cdu_info));

        SHR_IF_ERR_EXIT(bcm_port_priority_config_get(unit, logical_port, &priority_config));

        for (rmc_idx = 0; rmc_idx < priority_config.nof_priority_groups; ++rmc_idx)
        {
            /*
             * Get the logical FIFO info from SW DB
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, logical_port, 0, rmc_idx, &rmc_info));

            /*
             * Get nof rmc's, Scheduler and PRD priority
             */
            nof_entries = priority_config.priority_groups[rmc_idx].num_of_entries;
            sch_prio = priority_config.priority_groups[rmc_idx].sch_priority;
            src_prio = priority_config.priority_groups[rmc_idx].source_priority;
            /*
             * Format the PRD Priority string
             */
            sal_memset(prd_prio_str, 0, sizeof(prd_prio_str));
            if (src_prio & BCM_PORT_F_PRIORITY_0)
            {
                utilex_str_append(prd_prio_str, "0");
            }
            if (src_prio & BCM_PORT_F_PRIORITY_1)
            {
                utilex_str_append(prd_prio_str, "1");
            }
            if (src_prio & BCM_PORT_F_PRIORITY_2)
            {
                utilex_str_append(prd_prio_str, "2");
            }
            if (src_prio & BCM_PORT_F_PRIORITY_3)
            {
                utilex_str_append(prd_prio_str, "3");
            }
            utilex_str_escape(prd_prio_str, ',');

            /*
             * Get FIFO status
             */
            SHR_IF_ERR_EXIT(imb_port_rx_fifo_status_get(unit, logical_port, sch_prio, &max_occupancy, &fifo_level));

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);

            if (rmc_idx > 0)
            {
                PRT_CELL_SET("%s", "");
                PRT_CELL_SET("%s", "");
            }
            else
            {
                /*
                 * Print first line for each port.
                 */
                PRT_CELL_SET("%d", logical_port);
                PRT_CELL_SET("%d", cdu_info.ethu_id);
            }
            PRT_CELL_SET("%d", rmc_info.rmc_id);
            PRT_CELL_SET("%s", sch_prio_str[sch_prio]);
            PRT_CELL_SET("%s", prd_prio_str);
            PRT_CELL_SET("%d", nof_entries);
            PRT_CELL_SET("%d", max_occupancy);
            PRT_CELL_SET("%d", fifo_level);
        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Dump the NIF ILKN Tx Info
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
sh_dnx_nif_ilkn_tx_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t ilkn_ports;
    uint32 max_occupancy;
    uint32 fifo_level;
    uint32 pm_credits;
    dnx_algo_port_info_s port_info;
    bcm_port_t logical_port;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("port", ilkn_ports);

    /*
     * Print table header
     */
    PRT_TITLE_SET("NIF ILKN TX INFO:");

    PRT_COLUMN_ADD("Logical Port");
    PRT_COLUMN_ADD("Max Occupancy");
    PRT_COLUMN_ADD("Current FIFO Level");

    BCM_PBMP_ITER(ilkn_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, FALSE, TRUE))
        {
            continue;
        }
        /*
         * Get FIFO status
         */
        SHR_IF_ERR_EXIT(imb_port_tx_fifo_status_get(unit, logical_port, &max_occupancy, &fifo_level, &pm_credits));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%d", logical_port);
        PRT_CELL_SET("%d", max_occupancy);
        PRT_CELL_SET("%d", fifo_level);
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Dump the NIF ILKN Rx Info
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
sh_dnx_nif_ilkn_rx_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t ilkn_ports;
    uint32 max_occupancy;
    uint32 fifo_level;
    int is_tdm;
    dnx_algo_port_info_s port_info;
    bcm_port_t logical_port;
    dnx_algo_port_tdm_mode_e tdm_mode;
    bcm_port_nif_scheduler_t sch_priority;
    char *sch_prio_str[] = { "Low", "High", "TDM" };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("port", ilkn_ports);

    /*
     * Print table header
     */
    PRT_TITLE_SET("NIF ILKN RX INFO:");

    PRT_COLUMN_ADD("Logical Port");
    PRT_COLUMN_ADD("FIFO Sch Priority");
    PRT_COLUMN_ADD("Max Occupancy");
    PRT_COLUMN_ADD("Current FIFO Level");

    BCM_PBMP_ITER(ilkn_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, FALSE, TRUE))
        {
            continue;
        }
        /*
         * Get the channel TDM mode
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, logical_port, &tdm_mode));
        is_tdm = ((tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS) || DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info)) ? 1 : 0;

        if (is_tdm)
        {
            sch_priority = bcmPortNifSchedulerTDM;
        }
        else
        {
            /*
             * Get ILKN scheduler priority from sw state
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_priority_get(unit, logical_port, &sch_priority));
        }
        /*
         * Get FIFO status
         */
        SHR_IF_ERR_EXIT(imb_port_rx_fifo_status_get(unit, logical_port, sch_priority, &max_occupancy, &fifo_level));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%d", logical_port);
        PRT_CELL_SET("%s", sch_prio_str[sch_priority]);
        PRT_CELL_SET("%d", max_occupancy);
        PRT_CELL_SET("%d", fifo_level);
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief DNX NIF Rx and NIF Tx diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for NIF lane map diagnostic commands
 */

/* *INDENT-OFF* */

sh_sand_man_t sh_dnx_nif_tx_man = {
    .brief    = "Diagnostic for printing the NIF Tx info.",
    .full     = "Dump the NIF Tx info. including credits from PM, "
                "Max occupancy and current FIFO level. ",
    .synopsis = "<port>",
    .examples = "xe \n"
                "xe14 \n"
                "13 \n"
                "port=13"
};

sh_sand_option_t sh_dnx_nif_tx_options[] = {
    {"port",  SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "nif", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

sh_sand_man_t sh_dnx_nif_rx_man = {
    .brief    = "Diagnostic for printing the NIF Rx info.",
    .full     = "Dump the NIF Rx info. including credits from PM, "
                "Max occupancy ,current FIFO level, FIFO schedule priority, "
                "FIFO PRD priority and so on. ",
    .synopsis = "<port>",
    .examples = "xe \n"
                "xe14 \n"
                "13 \n"
                "port=13"
};

sh_sand_option_t sh_dnx_nif_rx_options[] = {
    {"port",  SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "nif", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

sh_sand_man_t sh_dnx_nif_ilkn_tx_man = {
    .brief    = "Diagnostic for printing the NIF ILKN Tx info.",
    .full     = "Dump the NIF ILKN Tx info. including Max occupancy and current FIFO level.",
    .synopsis = "<port>",
    .examples = "il6 \n"
                "6 \n"
                "port=6"
};

sh_sand_option_t sh_dnx_nif_ilkn_tx_options[] = {
    {"port",  SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "il", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

sh_sand_man_t sh_dnx_nif_ilkn_rx_man = {
    .brief    = "Diagnostic for printing the NIF ILKN Rx info.",
    .full     = "Dump the NIF Rx info. including Max occupancy, "
                "current FIFO level, FIFO schedule priority.",
    .synopsis = "<port>",
    .examples = "il6 \n"
                "6 \n"
                "port=6"
};

sh_sand_option_t sh_dnx_nif_ilkn_rx_options[] = {
    {"port",  SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "il", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

/* *INDENT-ON* */
/*
 * }
 */
#undef _ERR_MSG_MODULE_NAME
