/** \file diag_dnx_ingress_prd.c
 *
 * diagnostics for Port Priority Drop
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COSQ

/*
 * Include files.
 * {
 */
/** allow drv.h include excplictly for system*/
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <appl/diag/system.h> /** FORMAT_PBMP_MAX */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <shared/util.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/appl/sal.h>

#include <bcm/switch.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/port/imb/imb.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>

#include "diag_dnx_ingress_prd.h"
/*
 * }
 */

/*
 * Macros
 * {
 */

#define DNX_DIAG_PORT_PRD_MAX_STR_LEN            32
/*
 * Assemble the PRD priority string, for exampe if the
 * available priority is 0 and 1, the final string should be "0, 1"
 */
#define DNX_DIAG_PORT_PRD_PRIORITY_STR_ASSEMBLE(prd_prio_str, prio_bmp, priority)  \
    do {                                                                           \
        char tmp_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN] = "";                          \
        sal_sprintf(tmp_str, (*prio_bmp == 0) ? "%d" : ",%d", priority);           \
        sal_strncat(prd_prio_str, tmp_str, sizeof(prd_prio_str) -  sal_strlen(prd_prio_str) - 1); \
        SHR_BITSET(prio_bmp, priority);                                            \
    } while (0)

#define PRD_ROW_PRINT(port_str, header_type_string, prd_prio_string, sch_string, fifo_size_str, threshold_string, drop_string)    \
    do {                                                                    \
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);                                \
        PRT_CELL_SET("%s", port_str);                                       \
        PRT_CELL_SET("%s", header_type_string);                             \
        PRT_CELL_SET("%s", prd_prio_string);                                \
        PRT_CELL_SET("%s", sch_string);                                     \
        PRT_CELL_SET("%s", fifo_size_str);                                  \
        PRT_CELL_SET("%s", threshold_string);                               \
        PRT_CELL_SET("%s", drop_string);                                    \
    } while (0)

/*
 * }
 */

/**
* Functions
* {
*/

/**
 * \brief - Determine header type for port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - port number
 * \param [in] prio_group_idx - Number of currently parsed prio group/RMC/HRF
 * \param [in] src_prio - source priority from Parser
 * \param [in] sch_prio - Scheduler priority
 * \param [in] fifo_size - fifo size
 * \param [in] drop_count - drop count
 * \param [in] header_str - Header type string
 * \param [in] prt_ctr - argument needed by diag framework
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
sh_dnx_ingress_port_drop_add_row(
    int unit,
    int port,
    int prio_group_idx,
    uint32 src_prio,
    bcm_port_nif_scheduler_t sch_prio,
    uint32 fifo_size,
    uint64 drop_count,
    char *header_str,
    prt_control_t * prt_ctr)
{
    int priority;
    int prio_count;
    char *sch_prio_str[] = { "Low", "High", "TDM" };
    char prd_prio_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char threshold_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char drop_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char fifo_size_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char port_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    dnx_algo_port_info_s port_info;
    uint32 threshold, prio_bmp[1];
    uint64 fifo_size_64;
    uint64 port_64;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Format drop count string
     */
    format_uint64_decimal(drop_str, drop_count, ',');

    /*
     * Format fifo size string
     */
    COMPILER_64_SET(fifo_size_64, 0, fifo_size);
    format_uint64_decimal(fifo_size_str, fifo_size_64, ',');

    /*
     * Format port string
     */
    COMPILER_64_SET(port_64, 0, port);
    format_uint64_decimal(port_str, port_64, ',');

    /** Clear the variable */
    *prio_bmp = 0;
    sal_memset(prd_prio_str, '\0', sizeof(prd_prio_str));

    /** Assemble the priority string. */
    if (src_prio & BCM_PORT_F_PRIORITY_0)
    {
        DNX_DIAG_PORT_PRD_PRIORITY_STR_ASSEMBLE(prd_prio_str, prio_bmp, 0);
    }
    if (src_prio & BCM_PORT_F_PRIORITY_1)
    {
        DNX_DIAG_PORT_PRD_PRIORITY_STR_ASSEMBLE(prd_prio_str, prio_bmp, 1);
    }
    if (src_prio & BCM_PORT_F_PRIORITY_2)
    {
        DNX_DIAG_PORT_PRD_PRIORITY_STR_ASSEMBLE(prd_prio_str, prio_bmp, 2);
    }
    if (src_prio & BCM_PORT_F_PRIORITY_3)
    {
        DNX_DIAG_PORT_PRD_PRIORITY_STR_ASSEMBLE(prd_prio_str, prio_bmp, 3);
    }

    /*
     * Get the Threshold (except for TDM) and Print the table for each row
     */
    prio_count = 0;
    if ((src_prio & BCM_PORT_F_PRIORITY_TDM) == src_prio)
    {
        /** Print a row for TDM when only TDM bit is present in source priority.*/
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, FALSE, FALSE))
        {
            PRD_ROW_PRINT(port_str, header_str, "-", sch_prio_str[sch_prio], fifo_size_str, "unlimited", drop_str);
        }
        else
        {
            PRD_ROW_PRINT("", "", "TDM", sch_prio_str[sch_prio], fifo_size_str, "unlimited", drop_str);
        }
    }
    else
    {
        SHR_BIT_ITER(prio_bmp, dnx_data_nif.prd.nof_priorities_get(unit), priority)
        {
            SHR_IF_ERR_EXIT(bcm_cosq_ingress_port_drop_threshold_get(unit, port, 0, priority, &threshold));
            /*
             * If threshold is bigger than fifo size it means that default value for threshold is configured.
             * User is restricted by the API to set threshold bigger that the actual fifo size.
             * To avoid confusion we will show unlimited when threshold is max.
             */
            if (threshold > fifo_size)
            {
                sal_sprintf(threshold_str, "%d - unlimited", priority);
            }
            else
            {
                sal_sprintf(threshold_str, "%d - %u", priority, threshold);
            }

            if ((prio_group_idx == 0) && (prio_count == 0))
            {
                /** Print the first row for priority 0 on each port */
                PRD_ROW_PRINT(port_str, header_str, prd_prio_str, sch_prio_str[sch_prio], fifo_size_str,
                              threshold_str, drop_str);
            }
            else if ((prio_group_idx != 0) && (prio_count == 0))
            {
                /** Print the first row for every subsequent priority group on the same port*/
                PRD_ROW_PRINT("", "", prd_prio_str, sch_prio_str[sch_prio], fifo_size_str, threshold_str, drop_str);
            }
            else
            {
                /** Print other rows/priorities with same priority group */
                PRD_ROW_PRINT("", "", "", "", "", threshold_str, "");
            }
            prio_count++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Determine header type for port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - port number
 * \param [in] header_str - header type string
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
sh_dnx_ingress_port_drop_header_type_get(
    int unit,
    int port,
    char *header_str)
{
    bcm_switch_control_key_t switch_control_key;
    bcm_switch_control_info_t switch_control_info;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;

    SHR_FUNC_INIT_VARS(unit);

    /** Get TDM mode. It will be used to check if TDM is in use.*/
    SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, port, &if_tdm_mode));
    /*
     * Get the PRD header type
     */
    switch_control_key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN;
    switch_control_key.type = bcmSwitchPortHeaderType;
    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_get(unit, port, switch_control_key, &switch_control_info));
    switch (switch_control_info.value)
    {
        case BCM_SWITCH_PORT_HEADER_TYPE_ETH:
            switch (if_tdm_mode)
            {
                case DNX_ALGO_PORT_IF_TDM_ONLY:
                    sal_strncpy(header_str, "TDM", DNX_DIAG_PORT_PRD_MAX_STR_LEN - 1);
                    break;
                case DNX_ALGO_PORT_IF_TDM_HYBRID:
                    sal_strncpy(header_str, "TDM Hybrid", DNX_DIAG_PORT_PRD_MAX_STR_LEN - 1);
                    break;
                default:
                    sal_strncpy(header_str, "ETH", DNX_DIAG_PORT_PRD_MAX_STR_LEN - 1);
            }
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_TM:
            sal_strncpy(header_str, "ITMH", DNX_DIAG_PORT_PRD_MAX_STR_LEN - 1);
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_STACKING:
            sal_strncpy(header_str, "FTMH", DNX_DIAG_PORT_PRD_MAX_STR_LEN - 1);
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP:
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2:
            sal_strncpy(header_str, "PTCH_2", DNX_DIAG_PORT_PRD_MAX_STR_LEN - 1);
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_NONE:
            sal_strncpy(header_str, "NONE", DNX_DIAG_PORT_PRD_MAX_STR_LEN - 1);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported header type %d\n", switch_control_info.value);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Dump the port PRD info for ILKN ports.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - port number
 * \param [in] prt_ctr - argument needed by diag framework
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * ILKN ports will only use Data or TDM HRF at a time.
 *   * bcm_port_priority_config_get is not supported on ILKN ports.
 * \see
 *   * None
 */
shr_error_e
sh_dnx_ingress_port_drop_ilkn(
    int unit,
    int port,
    prt_control_t * prt_ctr)
{
    uint32 src_prio;
    int hrf_size;
    uint64 drop_count = COMPILER_64_INIT(0, 0);

    char header_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN] = "\0";
    bcm_port_nif_scheduler_t sch_prio;
    dnx_algo_port_tdm_mode_e tdm_mode;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(header_str, '\0', DNX_DIAG_PORT_PRD_MAX_STR_LEN);

    /** Get port TDM mode. It will be used to check if TDM is in use.*/
    SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, port, &tdm_mode));

    if (tdm_mode > DNX_ALGO_PORT_TDM_MODE_NONE && tdm_mode < DNX_ALGO_PORT_TDM_MODE_NOF)
    {
        /*
         * If ILKN port is used as TDM, src priority is irrelevant but we
         * still set it to BCM_PORT_F_PRIORITY_TDM because it is needed for the
         * add row print function.
         */
        sch_prio = bcmPortNifSchedulerTDM;
        src_prio = BCM_PORT_F_PRIORITY_TDM;
    }
    else
    {
        /*
         * Get ILKN scheduler priority from sw state
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_priority_get(unit, port, &sch_prio));
        src_prio = IMB_PRD_PRIORITY_ALL & ~BCM_PORT_F_PRIORITY_TDM;
    }

    /*
     * Get the PRD drop count
     */
    SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, sch_prio, &drop_count));

    /*
     * Get HRF size
     */
    SHR_IF_ERR_EXIT(imb_ilu_prd_hrf_size_get(unit, port, &hrf_size));

    SHR_IF_ERR_EXIT(sh_dnx_ingress_port_drop_header_type_get(unit, port, header_str));

    /*
     * Each ILKN port can use single HRF at a time (DATA or TDM)
     */
    SHR_IF_ERR_EXIT(sh_dnx_ingress_port_drop_add_row
                    (unit, port, 0 /* hrf_idx */ , src_prio, sch_prio, hrf_size, drop_count, header_str, prt_ctr));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Dump the port PRD info for Ethernet and FlexE ports
 *
 * \param [in] unit - chip unit id
 * \param [in] port - port number
 * \param [in] prt_ctr - argument needed by diag framework
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
sh_dnx_ingress_port_drop_eth_flexe(
    int unit,
    int port,
    prt_control_t * prt_ctr)
{
    bcm_port_prio_config_t priority_config;
    int priority_group, nof_priority_groups;
    uint32 src_prio, fifo_size;
    uint64 drop_count = COMPILER_64_INIT(0, 0);
    bcm_port_nif_scheduler_t sch_prio;
    char header_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN] = "\0";

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_priority_config_get(unit, port, &priority_config));
    nof_priority_groups = priority_config.nof_priority_groups;
    for (priority_group = 0; priority_group < nof_priority_groups; ++priority_group)
    {
        /*
         * Get the PRD Priority
         */
        src_prio = priority_config.priority_groups[priority_group].source_priority;

        /*
         * Get the PRD drop count
         */
        sch_prio = priority_config.priority_groups[priority_group].sch_priority;
        SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, sch_prio, &drop_count));

        /*
         * Get Fifo size
         */
        fifo_size = priority_config.priority_groups[priority_group].num_of_entries;

        SHR_IF_ERR_EXIT(sh_dnx_ingress_port_drop_header_type_get(unit, port, header_str));

        SHR_IF_ERR_EXIT(sh_dnx_ingress_port_drop_add_row
                        (unit, port, priority_group, src_prio, sch_prio, fifo_size, drop_count, header_str, prt_ctr));
    }
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
sh_dnx_ingress_port_drop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t all_pbmp;
    bcm_port_t port;
    dnx_algo_port_info_s port_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("Ingress Port Drop");

    PRT_COLUMN_ADD("port");
    PRT_COLUMN_ADD("PRD Port Type");
    PRT_COLUMN_ADD("PRD Priority");
    PRT_COLUMN_ADD("Sch Priority");
    PRT_COLUMN_ADD("FIFO Size(in entries)");
    PRT_COLUMN_ADD("Threshold");
    PRT_COLUMN_ADD("PRD Drop count");

    SH_SAND_GET_PORT("port", all_pbmp);

    BCM_PBMP_ITER(all_pbmp, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        /*
         * Skip the irrelevant port
         */
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, FALSE /* stif */ , FALSE /* L1 */ )
            || DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, FALSE))
        {
            SHR_IF_ERR_EXIT(sh_dnx_ingress_port_drop_eth_flexe(unit, port, prt_ctr));
        }
        else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, FALSE, FALSE))
        {
            SHR_IF_ERR_EXIT(sh_dnx_ingress_port_drop_ilkn(unit, port, prt_ctr));
        }
        else
        {
            /*
             * Do nothing on port types we are not interested in.
             */
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;

}

/**
 * \brief DNX Port PRD dignostics
 * List of the supported commands, pointer to command function and command usage function.
 */

/* *INDENT-OFF* */


sh_sand_option_t sh_dnx_ingress_port_drop_options[] = {
    /*keyword,   action,                    command, options,                            man*/
    {"port",      SAL_FIELD_TYPE_PORT, "port # / logical port type / port name",    "all",      NULL},
    {NULL}
};

sh_sand_man_t sh_dnx_ingress_port_drop_man = {
    .brief    = "Diagnostic pack for NIF PRD",
    .full     = "Diagnostic pack for NIF PRD",
    .synopsis = "[port=<integer|port_name|nif>]",
    .examples = "\n"
                "port=1\n"
                "port=xe\n"
                "port=nif"
};

/* *INDENT-ON* */
/*
 * }
 */
#undef _ERR_MSG_MODULE_NAME
