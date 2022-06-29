/** \file diag_dnx_nif_txrx.c
 *
 * diagnostics for NIF Rx & NIF Tx
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
#include <soc/sand/shrextend/shrextend_debug.h>

#include <sal/appl/sal.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_ofr.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>

#include "diag_dnx_nif_txrx.h"
#include <appl/diag/dnx/diag_dnx_nif.h>

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
 * See .h file
 */

int
sh_dnx_nif_rx_fifo_status_get(
    int unit,
    bcm_port_t logical_port,
    uint32 sch_prio,
    uint32 *max_occupancy,
    uint32 *fifo_level,
    uint32 *mapped_port)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get FIFO status
     */
    SHR_IF_ERR_EXIT(imb_port_rx_fifo_status_get(unit, logical_port, sch_prio, max_occupancy, fifo_level, mapped_port));
exit:
    SHR_FUNC_EXIT;
}
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
sh_dnx_nif_tx_show_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t nif_ports;
    uint32 pm_credits;
    uint32 max_occupancy;
    uint32 fifo_level;
    uint32 is_ofr;
    uint32 mapped_port;
    dnx_algo_port_info_s port_info;
    bcm_port_t logical_port;
    char mapped[DNX_DIAG_NIF_TXRX_MAX_STR_LEN];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    is_ofr = dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported) ? 1 : 0;
    SH_SAND_GET_PORT("port", nif_ports);

    /*
     * Print table header
     */
    PRT_TITLE_SET("NIF TX INFO:");

    PRT_COLUMN_ADD("Logical Port");
    PRT_COLUMN_ADD("Credits From PM");
    PRT_COLUMN_ADD("Max Occupancy");
    PRT_COLUMN_ADD("Current FIFO Level");
    /*
     * For devices with counter mapping add a column that states if port is mapped
     * to a counter
     */
    if (is_ofr)
    {
        PRT_COLUMN_ADD("COUNTER MAPPING");
    }

    BCM_PBMP_ITER(nif_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, MGMT)))
        {
            continue;
        }

        /*
         * get FIFO status
         */
        SHR_IF_ERR_EXIT(imb_port_tx_fifo_status_get
                        (unit, logical_port, &max_occupancy, &fifo_level, &pm_credits, &mapped_port));

        if (mapped_port == 1)
        {
            sal_snprintf(mapped, sizeof(mapped), "MAPPED");
        }
        else
        {
            sal_snprintf(mapped, sizeof(mapped), "UNMAPPED");
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%d", logical_port);
        PRT_CELL_SET("%d", pm_credits);
        PRT_CELL_SET("%d", max_occupancy);
        PRT_CELL_SET("%d", fifo_level);

        if (is_ofr)
        {
            PRT_CELL_SET("%s", mapped);
        }

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
sh_dnx_nif_rx_show_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t nif_ports;
    uint32 max_occupancy;
    uint32 fifo_level;
    uint32 mapped_port;
    uint32 src_prio, is_ofr;
    bcm_port_nif_scheduler_t sch_prio;
    bcm_port_prio_config_t priority_config;
    int rmc_idx, nof_entries;

    dnx_algo_port_info_s port_info;
    bcm_port_t logical_port;
    dnx_algo_port_ethu_access_info_t ethu_info;
    dnx_algo_port_rmc_info_t rmc_info;
    char *sch_prio_str[] = { "Low", "High", "TDM" };
    char prd_prio_str[DNX_DIAG_NIF_TXRX_MAX_STR_LEN];
    char mapped[DNX_DIAG_NIF_TXRX_MAX_STR_LEN];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    is_ofr = dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported) ? 1 : 0;
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
    /*
     * For devices with counter mapping add a column that states if port is mapped
     * to a counter
     */
    if (is_ofr)
    {
        PRT_COLUMN_ADD("COUNTER MAPPING");
    }

    BCM_PBMP_ITER(nif_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, MGMT)))
        {
            continue;
        }

        /** This condition is used to filter ports that don't have rx path (like stif, unlike stif_data) */
        if (!DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, logical_port, &ethu_info));

        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_MGMT(unit, port_info))
        {
            /** No priorities for MGU port */
            int rmc_idx = 0;
            /*
             * Get the logical FIFO info from SW DB
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, logical_port, 0, rmc_idx, &rmc_info));

            /*
             * get FIFO status
             */
            sch_prio = 0;
            SHR_IF_ERR_EXIT(sh_dnx_nif_rx_fifo_status_get
                            (unit, logical_port, sch_prio, &max_occupancy, &fifo_level, &mapped_port));

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", logical_port);
            PRT_CELL_SET("%d", ethu_info.ethu_id);

            PRT_CELL_SET("%d", rmc_info.rmc_id);
            PRT_CELL_SET("%s", "N/A");
            PRT_CELL_SET("%s", "N/A");
            PRT_CELL_SET("%d", rmc_info.nof_entries);
            PRT_CELL_SET("%d", max_occupancy);
            PRT_CELL_SET("%d", fifo_level);
            continue;
        }

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
             * get FIFO status
             */
            SHR_IF_ERR_EXIT(sh_dnx_nif_rx_fifo_status_get
                            (unit, logical_port, sch_prio, &max_occupancy, &fifo_level, &mapped_port));

            if (mapped_port == 1)
            {
                sal_snprintf(mapped, sizeof(mapped), "MAPPED");
            }
            else
            {
                sal_snprintf(mapped, sizeof(mapped), "UNMAPPED");
            }

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
                PRT_CELL_SET("%d", ethu_info.ethu_id);
            }
            PRT_CELL_SET("%d", rmc_info.rmc_id);
            PRT_CELL_SET("%s", sch_prio_str[sch_prio]);
            PRT_CELL_SET("%s", prd_prio_str);
            PRT_CELL_SET("%d", nof_entries);
            PRT_CELL_SET("%d", max_occupancy);
            PRT_CELL_SET("%d", fifo_level);

            if (is_ofr)
            {
                PRT_CELL_SET("%s", mapped);
            }

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
    uint32 mapped;
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

        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
        {
            continue;
        }
        /*
         * Get FIFO status
         */
        SHR_IF_ERR_EXIT(imb_port_tx_fifo_status_get
                        (unit, logical_port, &max_occupancy, &fifo_level, &pm_credits, &mapped));

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
    uint32 mapped;
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

        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
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
        SHR_IF_ERR_EXIT(imb_port_rx_fifo_status_get
                        (unit, logical_port, sch_priority, &max_occupancy, &fifo_level, &mapped));

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
 * \brief - Dump the port PRD info
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
sh_dnx_nif_rx_map_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 port, counter;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * extract and store argument values
     */
    SH_SAND_GET_UINT32("port", port);
    SH_SAND_GET_UINT32("counter", counter);

    /*
     * Map port to debug counter and store in swstate
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_rx_fifo_status_counter_map(unit, counter, port));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.rx_fifo_counter_port_map.port.set(unit, counter, port));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Dump the port PRD info
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
sh_dnx_nif_tx_map_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 port, counter;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * extract and store argument values
     */
    SH_SAND_GET_UINT32("port", port);
    SH_SAND_GET_UINT32("counter", counter);

    /*
     * Map port to debug counter and store in swstate
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_tx_fifo_status_counter_map(unit, counter, port));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tx_fifo_counter_port_map.port.set(unit, counter, port));

exit:
    SHR_FUNC_EXIT;

}

/**
 *  if the device doesn't have OFR then mapping is not supported.
 *  This function is used as a conditional used to determine if mapping diagnostic cmd should be available in current device
 *
 */
shr_error_e
sh_cmd_fifo_mapping_available(
    int unit,
    rhlist_t * list)
{
    SHR_FUNC_INIT_VARS(unit);
    if (!dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }
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

sh_sand_man_t sh_dnx_nif_rx_show_man = {
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

sh_sand_man_t sh_dnx_nif_tx_show_man = {
    .brief    = "Diagnostic for printing the NIF Tx info.",
    .full     = "Dump the NIF Rx info. including credits from PM, "
                "Max occupancy ,current FIFO level, FIFO schedule priority, "
                "FIFO PRD priority and so on. ",
    .synopsis = "<port>",
    .examples = "xe \n"
                "xe14 \n"
                "13 \n"
                "port=13"
};

sh_sand_option_t sh_dnx_nif_rx_show_options[] = {
    {"port",  SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "nif", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

sh_sand_option_t sh_dnx_nif_tx_show_options[] = {
    {"port",  SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "nif", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

sh_sand_option_t sh_dnx_nif_rx_map_options[] = {
    /*keyword,         type,                    description,                                                default,          man*/
    {"port",           SAL_FIELD_TYPE_UINT32,   "port # / logical port type / port name",                   NULL,              NULL},
    {"counter",        SAL_FIELD_TYPE_UINT32,   "counter # / id of counter / counter name",                 NULL,              NULL},
    {NULL}
};

sh_sand_option_t sh_dnx_nif_tx_map_options[] = {
    /*keyword,         type,                    description,                                                default,          man*/
    {"port",           SAL_FIELD_TYPE_UINT32,   "port # / logical port type / port name",                   NULL,              NULL},
    {"counter",        SAL_FIELD_TYPE_UINT32,   "counter # / id of counter / counter name",                 NULL,              NULL},
    {NULL}
};

sh_sand_man_t sh_dnx_nif_rx_mapping_man = {
    .brief    = "Maps a debug counter to a port",
    .full     = "Maps a debug counter to a port\n After a mapping operation a counter will start tracking the port's RX max occupancy and fifo level.",
    .synopsis = "port=<integer> counter=<integer>",
    .examples = "port=1 counter=1\n"
                "port=13 counter=7"
};

sh_sand_man_t sh_dnx_nif_tx_mapping_man = {
    .brief    = "Maps a debug counter to a port",
    .full     = "Maps a debug counter to a port\n After a mapping operation a counter will start tracking the port's TX max occupancy and fifo level.",
    .synopsis = "port=<integer> counter=<integer>",
    .examples = "port=1 counter=1\n"
                "port=13 counter=7"
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

/**
 * \brief DNX NIF RX diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for TM Ingress RX diagnostic commands
 */
sh_sand_cmd_t sh_dnx_nif_rx_cmds[] = {
/* *INDENT-OFF* */
    /*keyword,          action,                                  command,    options,                                     man  */
    {"show",            sh_dnx_nif_rx_show_cmd,                  NULL,       sh_dnx_nif_rx_show_options,                  &sh_dnx_nif_rx_show_man},
    {"mapping",         sh_dnx_nif_rx_map_cmd,                   NULL,       sh_dnx_nif_rx_map_options,                   &sh_dnx_nif_rx_mapping_man, NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_fifo_mapping_available},
    {NULL}
/* *INDENT-ON* */
};

/**
 * \brief DNX NIF TX diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for TM Ingress RX diagnostic commands
 */
sh_sand_cmd_t sh_dnx_nif_tx_cmds[] = {
/* *INDENT-OFF* */
    /*keyword,          action,                                  command,    options,                                     man  */
    {"show",            sh_dnx_nif_tx_show_cmd,                  NULL,       sh_dnx_nif_tx_show_options,                  &sh_dnx_nif_tx_show_man},
    {"mapping",         sh_dnx_nif_tx_map_cmd,                   NULL,       sh_dnx_nif_tx_map_options,                   &sh_dnx_nif_tx_mapping_man, NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_fifo_mapping_available},
    {NULL}
/* *INDENT-ON* */
};

sh_sand_option_t sh_dnx_nif_ilkn_rx_options[] = {
    {"port", SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "il", NULL, NULL, SH_SAND_ARG_FREE}
    ,
    {NULL}
};

/* *INDENT-ON* */
/*
 * }
 */
#undef _ERR_MSG_MODULE_NAME
