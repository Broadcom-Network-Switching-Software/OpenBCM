/** \file diag_dnx_ingress_prd.c
 *
 * diagnostics for Port Priority Drop
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
#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/appl/sal.h>

#include <bcm/switch.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_ofr_prd.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
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
        sal_strncat_s(prd_prio_str, tmp_str, sizeof(prd_prio_str)); \
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

#define PRD_GLOBAL_COUNTERS_ROW_PRINT(priority, priority_group, drop_count)    \
    do {                                                                    \
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);                                \
        PRT_CELL_SET("%s", priority);                                       \
        PRT_CELL_SET("%s", priority_group);                                 \
        PRT_CELL_SET("%s", drop_count);                                     \
    } while (0)

/*
 * }
 */

/**
* Functions
* {
*/

/**
 * \brief - adds global port drop counter entry to table
 *
 * \param [in] unit - chip unit id
 * \param [in] priority - global port drop counter priority
 * \param [in] priority_group - global port drop counter priority group
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
sh_dnx_ingress_global_port_drop_add_row(
    int unit,
    uint32 priority,
    uint32 priority_group,
    prt_control_t * prt_ctr)
{
    char drop_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char priority_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char *sch_prio_str[] = { "High", "Low" };
    uint64 drop_count = COMPILER_64_INIT(0, 0);
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get drop count and format data
     */
    sal_sprintf(priority_str, "%d", priority);
#ifdef BCM_DNX2_SUPPORT
    SHR_IF_ERR_EXIT(dnx_ofr_prd_global_drop_count_get(unit, priority, priority_group, &drop_count));
#endif
    format_uint64_decimal(drop_str, drop_count, ',');

    PRD_GLOBAL_COUNTERS_ROW_PRINT(priority_str, sch_prio_str[priority_group], drop_str);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - prints the global port drop counter table
 *
 * \param [in] unit - chip unit id
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
sh_dnx_ingress_global_port_drop(
    int unit,
    prt_control_t * prt_ctr)
{
    SHR_FUNC_INIT_VARS(unit)
        /*
         * create table entry for each priority & priority group combination
         */
        for (int i = 0; i < dnx_data_nif.prd.nof_priorities_get(unit); i++)
    {
        for (int j = 0; j < dnx_data_nif.eth.priority_groups_nof_get(unit); j++)
        {
            SHR_IF_ERR_EXIT(sh_dnx_ingress_global_port_drop_add_row(unit, i, j, prt_ctr));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX2_SUPPORT
/**
 * \brief - maps a counter to a port and priority group
 *
 * \param [in] unit - chip unit id
 * \param [in] counter - counter id
 * \param [in] port - port number
 * \param [in] priority_group - port priority group
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
sh_dnx_map_counter(
    int unit,
    uint32 counter,
    uint32 port,
    uint32 priority_group)
{
    uint64 drop_count = COMPILER_64_INIT(0, 0);
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Reset drop count of counter before mapping by reading its current value
     */
    SHR_IF_ERR_EXIT(dnx_ofr_prd_rmc_drop_count_get(unit, counter, &drop_count));
    /*
     * map a port priority group to a counter
     */
    SHR_IF_ERR_EXIT(dnx_ofr_prd_port_counter_map_set(unit, counter, port, priority_group));

exit:
    SHR_FUNC_EXIT;
}
#endif

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
 * \param [in] counter_id - counter id for current port(only for mapped devices)
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
    prt_control_t * prt_ctr,
    int counter_id)
{
    int priority;
    int prio_count;
    char *sch_prio_str[] = { "Low", "High", "TDM" };
    char prd_prio_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char threshold_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char drop_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char counter_id_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char fifo_size_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    char port_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN];
    dnx_algo_port_info_s port_info;
    uint32 threshold, prio_bmp[1];
    uint64 fifo_size_64;
    uint64 port_64;
    uint32 rmc_fifo_resolution, max_threshold, is_ofr;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * if OFR we need to set the counter id column of the diagnostic
     */
    is_ofr = dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported) ? 1 : 0;

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

    /*
     * get port info and format counter mapping
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0))
    {
        sal_sprintf(counter_id_str, "AUTOMAPPED");

    }
    else if (counter_id >= 0)
    {
        sal_sprintf(counter_id_str, "%d", counter_id);
    }
    else
    {
        sal_sprintf(counter_id_str, "UNMAPPED");
    }

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
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0))
        {
            PRD_ROW_PRINT(port_str, header_str, "-", sch_prio_str[sch_prio], fifo_size_str, "unlimited", drop_str);
            if (is_ofr)
            {
                PRT_CELL_SET("%s", counter_id_str);
            }
        }
        else
        {
            PRD_ROW_PRINT("", "", "TDM", sch_prio_str[sch_prio], fifo_size_str, "unlimited", drop_str);
            if (is_ofr)
            {
                PRT_CELL_SET("%s", counter_id_str);
            }
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
            rmc_fifo_resolution = dnx_data_nif.prd.rmc_fifo_2_threshold_resolution_get(unit);
            max_threshold = fifo_size * rmc_fifo_resolution;
            if (threshold >= max_threshold)
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
                PRD_ROW_PRINT(port_str, header_str, prd_prio_str, sch_prio_str[sch_prio], fifo_size_str, threshold_str,
                              drop_str);
                if (is_ofr)
                {
                    PRT_CELL_SET("%s", counter_id_str);
                }
            }
            else if ((prio_group_idx != 0) && (prio_count == 0))
            {
                /** Print the first row for every subsequent priority group on the same port*/
                PRD_ROW_PRINT("", "", prd_prio_str, sch_prio_str[sch_prio], fifo_size_str, threshold_str, drop_str);
                if (is_ofr)
                {
                    PRT_CELL_SET("%s", "");
                }
            }
            else
            {
                /** Print other rows/priorities with same priority group */
                PRD_ROW_PRINT("", "", "", "", "", threshold_str, "");
                if (is_ofr)
                {
                    PRT_CELL_SET("%s", "");
                }
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
    SHR_IF_ERR_EXIT(sh_dnx_ingress_port_drop_add_row(unit, port, 0 /* hrf_idx */ , src_prio, sch_prio, hrf_size, drop_count, header_str, prt_ctr, -1    /* counter 
                                                                                                                                                         * id 
                                                                                                                                                         */ ));

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
    int priority_group, nof_priority_groups, counter, counter_id = -1;
    uint32 src_prio, fifo_size, is_ofr, logical_port;
    uint64 drop_count = COMPILER_64_INIT(0, 0);
    bcm_port_nif_scheduler_t sch_prio;
    char header_str[DNX_DIAG_PORT_PRD_MAX_STR_LEN] = "\0";
    SHR_FUNC_INIT_VARS(unit);

    /*
     * if OFR is supported then get prd drop count only for priority groups(RMC's) that have counters mapped to them.
     * otherwise get drop count for all ports and priority groups
     */
    is_ofr = dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported) ? 1 : 0;

    SHR_IF_ERR_EXIT(bcm_port_priority_config_get(unit, port, &priority_config));
    nof_priority_groups = priority_config.nof_priority_groups;
    for (priority_group = 0; priority_group < nof_priority_groups; ++priority_group)
    {
        /*
         * Get the PRD Priority
         */
        src_prio = priority_config.priority_groups[priority_group].source_priority;
        sch_prio = priority_config.priority_groups[priority_group].sch_priority;

        /*
         * Get the PRD drop count for OFR devices and verify port is actually mapped to a counter.
         */
        if (is_ofr)
        {
            /*
             * verify rmc is actually mapped to a counter and get drop count
             */
            counter_id = -1;
            for (counter = 0; counter < dnx_data_nif.prd.nof_prd_counters_get(unit); counter++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db.counter_port_map.port.get(unit, counter, &logical_port));
                if (logical_port == port)
                {
                    counter_id = counter;
                }
            }
            /*
             * if rmc is mapped then get its drop count
             */
            if (counter_id >= 0)
            {
#ifdef BCM_DNX2_SUPPORT
                SHR_IF_ERR_EXIT(dnx_ofr_prd_rmc_drop_count_get(unit, counter_id, &drop_count));
#endif
            }
        }

        else
        {
            SHR_IF_ERR_EXIT(imb_prd_drop_count_get(unit, port, sch_prio, &drop_count));
        }

        /*
         * Get Fifo size
         */
        fifo_size = priority_config.priority_groups[priority_group].num_of_entries;

        SHR_IF_ERR_EXIT(sh_dnx_ingress_port_drop_header_type_get(unit, port, header_str));
        SHR_IF_ERR_EXIT(sh_dnx_ingress_port_drop_add_row
                        (unit, port, priority_group, src_prio, sch_prio, fifo_size, drop_count, header_str, prt_ctr,
                         counter_id));
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
sh_dnx_ingress_port_drop_map_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 port, priority_group, counter;
    uint32 port_exists, prio_group_exists, counter_exists;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify all arguments are provided
     */
    SH_SAND_IS_PRESENT("port", port_exists);
    SH_SAND_IS_PRESENT("priority_group", prio_group_exists);
    SH_SAND_IS_PRESENT("counter", counter_exists);

    if (!counter_exists || !port_exists || !prio_group_exists)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Missing arguments. Please provide port, priority group and counter values.\n");
    }

    /*
     * extract and store argument values
     */
    SH_SAND_GET_UINT32("port", port);
    SH_SAND_GET_UINT32("counter", counter);
    SH_SAND_GET_UINT32("priority_group", priority_group);

#ifdef BCM_DNX2_SUPPORT
    /*
     * map counter to port and priority group
     */
    SHR_IF_ERR_EXIT(sh_dnx_map_counter(unit, counter, port, priority_group));
#endif

    /*
     * save mapping in swstate
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.counter_port_map.port.set(unit, counter, port));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.counter_port_map.priority_group.set(unit, counter, priority_group));

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
sh_dnx_ingress_port_drop_show_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t all_pbmp;
    uint32 port, is_ofr;
    dnx_algo_port_info_s port_info;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    is_ofr = dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported);
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

    /*
     * For OFR devices show which counter is mapped to current port
     */
    if (is_ofr)
    {
    PRT_COLUMN_ADD("Counter ID")}

    /*
     * iterate through all ports and populate diagnostic
     */
    SH_SAND_GET_PORT("port", all_pbmp);
    BCM_PBMP_ITER(all_pbmp, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        /*
         * Skip the irrelevant port
         */
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0)
            || DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE))
        {
            SHR_IF_ERR_EXIT(sh_dnx_ingress_port_drop_eth_flexe(unit, port, prt_ctr));
        }
        else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0))
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

    /*
     * if ofr is supported then create another table with global counters
     */
    if (dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported))
    {
        /*
         * create a new table for global counters
         */
        PRT_TITLE_SET("Ingress Global Port Drop Count");
        PRT_COLUMN_ADD("PRD Priority");
        PRT_COLUMN_ADD("Priority Group");
        PRT_COLUMN_ADD("Drop Count");
        SHR_IF_ERR_EXIT(sh_dnx_ingress_global_port_drop(unit, prt_ctr));
        PRT_COMMITX;
    }

    /*
     * Create last packet prd priority and tdm indication table for NON-OFR devices.
     */
    if (!dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported))
    {
        PRT_TITLE_SET("Last Packet Priority");
        PRT_COLUMN_ADD("Port");
        PRT_COLUMN_ADD("Last PRD Priority");
        PRT_COLUMN_ADD("TDM Indication");
        SHR_IF_ERR_EXIT(sh_dnx_ingress_prd_last_packet_priority(unit, prt_ctr, sand_control));
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;

}

/**
 * \brief - Iterate through all active ports and populate diag rows with
 * latest TDM and PRD priority for latest packet of each port
 *
 * \param [in] unit - chip unit id
 * \param [in] prt_ctr - diagnostic framework mandatory pointer
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
sh_dnx_ingress_prd_last_packet_priority(
    int unit,
    prt_control_t * prt_ctr,
    sh_sand_control_t * sand_control)
{
    uint32 priority;
    uint32 tdm;
    uint32 valid;
    dnx_algo_port_info_s port_info;
    bcm_port_t port;
    bcm_pbmp_t all_pbmp;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("port", all_pbmp);

    BCM_PBMP_ITER(all_pbmp, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0)
            || DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE))
        {
            /*
             * Retrieve latest PRD priority and TDM indication of port
             */
            SHR_IF_ERR_EXIT(imb_prd_port_last_packet_priority_get(unit, port, &priority, &tdm, &valid));

            /*
             * If port prd info is valid create table row with retrieved diagnostic data
             */
            if (valid)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE))
                {
                    PRT_CELL_SET("%s", "FEU Unit");

                }
                else
                {
                    PRT_CELL_SET("%d", port);
                }

                PRT_CELL_SET("%d", priority);
                PRT_CELL_SET("%d", tdm);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 *  if the device doesn't have OFR then mapping is not supported for tm ingress portdrop diagnostic.
 *  This function is used as a conditional used to determine if mapping diagnostic cmd should be available in current device
 *
 */
shr_error_e
sh_cmd_mapping_available(
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
 * \brief DNX Port PRD dignostics
 * List of the supported commands, pointer to command function and command usage function.
 */

/* *INDENT-OFF* */


sh_sand_option_t sh_dnx_ingress_port_drop_show_options[] = {
    /*keyword,   action,                    command, options,                            man*/
    {"port",      SAL_FIELD_TYPE_PORT, "port # / logical port type / port name",         "all",      NULL},
    {NULL}
};

sh_sand_option_t sh_dnx_ingress_port_drop_map_options[] = {
    /*keyword,         type,                    description,                                                default,          man*/
    {"port",           SAL_FIELD_TYPE_UINT32,   "port # / logical port type / port name",                   "0",              NULL},
    {"priority_group", SAL_FIELD_TYPE_UINT32,   "priority group # / rmc low|high / scheduler priority",     "0",              NULL},
    {"counter",        SAL_FIELD_TYPE_UINT32,   "counter # / id of counter / counter name",                 "0",              NULL},
    {NULL}
};

sh_sand_man_t sh_dnx_ingress_port_drop_show_man = {
    .brief    = "Diagnostic pack for NIF PRD",
    .full     = "Diagnostic pack for NIF PRD \nIncludes dropped packet counters and recent assigned packet priorities and tdm indications",
    .synopsis = "[port=<integer|port_name|nif>]",
    .examples = "\n"
                "port=1\n"
                "port=xe\n"
                "port=nif"
};

sh_sand_man_t sh_dnx_ingress_port_drop_mapping_man = {
    .brief    = "Maps a counter to a port and priority group",
    .full     = "Maps a counter to a port and priority group\n After a mapping operation a counter will start counting the port's priority group packet drops.",
    .synopsis = "port=<integer> priority_group=<integer> counter=<integer>",
    .examples = "port=1 priority_group=0 counter=1\n"
                "port=13 priority_group=1 counter=7"
};

/**
 * \brief DNX TM Ingress VSQ diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for TM Ingress VSQ diagnostic commands
 */
sh_sand_cmd_t sh_dnx_tm_ingress_prd_cmds[] = {
/* *INDENT-OFF* */
    /*keyword,          action,                                  command,    options,                                     man  */
    {"show",            sh_dnx_ingress_port_drop_show_cmd,       NULL,       sh_dnx_ingress_port_drop_show_options,       &sh_dnx_ingress_port_drop_show_man},
    {"mapping",         sh_dnx_ingress_port_drop_map_cmd,        NULL,       sh_dnx_ingress_port_drop_map_options,        &sh_dnx_ingress_port_drop_mapping_man, NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_mapping_available},
    {NULL}
/* *INDENT-ON* */
};

/* *INDENT-ON* */
/*
 * }
 */
#undef _ERR_MSG_MODULE_NAME
