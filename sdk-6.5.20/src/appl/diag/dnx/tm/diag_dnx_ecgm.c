/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * File:                diag_dnx_ecgm.c
 * Content:             Implementation of diag shell commands for Egress Congestion
 * Author:              Omri Grinshpan
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COSQ

/**
 * includes
 * {
 */
/**  soc  */
#include <soc/intr.h>
/** appl  */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/**  sal  */
#include <sal/appl/sal.h>
#include <sal/limits.h>
/** port */
#include <include/bcm_int/dnx/algo/port/algo_port_mgmt.h>                    /** dnx_algo_port_base_q_pair_get() - get base_q_pair from logical_port    */
#include <include/bcm_int/dnx/algo/port/algo_port_utils.h>                   /** dnx_algo_port_logicals_get() - get all valid ports                     */
#include <include/bcm/types.h>                                               /** BCM_PBMP_ITER() - iterate all valid ports                                 */
/** other */
#include "diag_dnx_ecgm.h"
#include <bcm_int/dnx/cosq/egress/ecgm.h>
#include <include/bcm_int/dnx/cosq/cosq.h>                                   /** DNX_COSQ_NOF_TC() macro                                                 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>                   /** ECGM DNX data - for num of things (e.g. Num of Service Pool)             */
#include <include/appl/diag/sand/diag_sand_utils.h>                          /** for SHR_CLI_EXIT() macro                                                */
#include <include/bcm_int/dnx/cmn/dnxcmn.h>                                  /** DNXCMN_CORES_ITER() macro                                                */
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>    /** num of interface and num of q_pairs                                    */
/**
 * }
 */

/**
 * typedefs
 * {
 */
/** These function pointers typedefs are used as parameters in the static functions prt_per_X_resources_table() below.
    Each function pointer is capable of holding both DBAL current values get function and DBAL max values get function.*/
typedef shr_error_e(
    *dbal_core_resources_get_t) (
    uint32,
    uint32,
    ecgm_core_global_resources_statistics_t *);
typedef shr_error_e(
    *dbal_interface_resources_get_t) (
    uint32,
    uint32,
    uint32,
    dbal_enum_value_field_ecgm_priority_e,
    uint32,
    uint32 *,
    uint32 *);
typedef shr_error_e(
    *dbal_port_resources_get_t) (
    uint32,
    uint32,
    uint32,
    uint32,
    uint32 *,
    uint32 *);
typedef shr_error_e(
    *dbal_queue_resources_get_t) (
    uint32,
    uint32,
    uint32,
    uint32,
    uint32 *,
    uint32 *);
typedef shr_error_e(
    *dbal_service_pool_resources_get_t) (
    uint32,
    uint32,
    uint32,
    ecgm_service_pool_resources_statistics_t *);
typedef shr_error_e(
    *dbal_service_pool_tc_resources_get_t) (
    uint32,
    uint32,
    uint32,
    uint32,
    uint32 *,
    uint32 *);
/**
 * }
 */

/**
 * options
 * {
 */
sh_sand_option_t sh_dnx_tm_egress_congestion_options[] = {
    {"all", SAL_FIELD_TYPE_BOOL,
     "Prints information regarding the ECGM resources: Current state and max value reached since last CPU read. The information is presented in tables per level (e.g. Core, Interface).",
     "No"},
    {"current", SAL_FIELD_TYPE_BOOL,
     "Prints a set of tables containing the current values of Packet Descriptors and Data Buffers. The set contains one table per each of the following levels: Core, Interface, Port, Queue, Service Pool and Service Pool per Traffic Class.",
     "No"},
    {"max", SAL_FIELD_TYPE_BOOL,
     "Prints a set of tables containing the maximum number of allocated Packet Descriptors and Data Buffers since last CPU read. The set contains one table per each of the following levels: Core, Interface, Port, Queue, Service Pool and Service Pool per Traffic Class.",
     "No"},
    {NULL}
};
/**
 * }
 */

/**
 * manuals
 * {
 */
sh_sand_man_t sh_dnx_tm_egress_congestion_man = {
    .brief =
        "Prints information regarding the ECGM resources: Current state and max value reached since last CPU read. The information is presented in tables per level (e.g. Core, Interface).",
    .full =
        "Prints multiple tables regarding the ECGM resources: Packet Descriptors(PDs) and Data Buffers(DBs) (note: DBs size is 256B and are per replication). The command is divided to two set of tables: The first set display the number of PDs and DBs that are currently allocated. The second set display the max number of allocation of PDs and DBs since last CPU read. Each set consists of one table per each of the following levels: Core, Interface, Port(only valid ports), Queue, Service Pool and Service Pool per Traffic Class. NOTE: If all values in a row are 0, it will not be printed.",
    .synopsis = "[ALL(default)/CURrent/MAX]",
    .examples = "ALL \n" "CURrent \n" "MAX"
};
/**
 * }
 */

/**
 * static functions declarations
 * {
 */
/**
* \brief -  Prints all tables that presents the current ECGM resources values.
*/
static shr_error_e sh_dnx_tm_egress_congestion_current_option(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
/**
* \brief -  Prints all tables that presents the maximum values of ECGM resources that have been reached since last CPU read.
*/
static shr_error_e sh_dnx_tm_egress_congestion_max_option(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
/**
* \brief -  Prints a single table of the ECGM Resource per core.
*             Can be used to print either current values or max values depending on the dbal_resource_get_func argument.
*/
static shr_error_e sh_dnx_per_core_resources_table_print(
    int unit,
    const char *title,
    dbal_core_resources_get_t dbal_resource_get_func,
    sh_sand_control_t * sand_control);
/**
* \brief -  Prints a single table of the ECGM Resource per Element.
*           This function is used to print the same data of interface, port, queue table_prtin function but
*           instead of printing 3 different tables it prints a single table with 3 sub-titles for each element.
*             Can be used to print either current values or max values depending on the dbal_resource_get_func argument.
*/
static shr_error_e sh_dnx_per_element_resources_table_print(
    int unit,
    const char *title,
    dbal_interface_resources_get_t dbal_resource_interface_get_func,
    dbal_port_resources_get_t dbal_resource_port_get_func,
    dbal_queue_resources_get_t dbal_resource_queue_get_func,
    sh_sand_control_t * sand_control);
/**
* \brief -  Prints a single table of the ECGM Resource per interface.
*             Can be used to print either current values or max values depending on the dbal_resource_get_func argument.
*/
/*static shr_error_e sh_dnx_per_interface_resources_table_print(
    int unit,
    const char *title,
    dbal_interface_resources_get_t dbal_resource_get_func);*/
/**
* \brief -  Gets the rows data from DBAL API and prints them in a table that was created prior.
*/
static shr_error_e sh_dnx_interface_resources_table_fill(
    int unit,
    prt_control_t * prt_ctr,
    dbal_interface_resources_get_t dbal_resource_get_func);
/**
* \brief -  Prints a single table of the ECGM Resource per port.
*             Can be used to print either current values or max values depending on the dbal_resource_get_func argument.
*/
/*static shr_error_e sh_dnx_per_port_resources_table_print(
    int unit,
    const char *title,
    dbal_port_resources_get_t dbal_resource_get_func);*/
/**
* \brief -  Gets the rows data from DBAL API and prints them in a table that was created prior.
*/
static shr_error_e sh_dnx_port_resources_table_fill(
    int unit,
    prt_control_t * prt_ctr,
    dbal_port_resources_get_t dbal_resource_get_func);
/**
* \brief -  Prints a single table of the ECGM Resource per queue.
*             Can be used to print either current values or max values depending on the dbal_resource_get_func argument.
*/
/*static shr_error_e sh_dnx_per_queue_resources_table_print(
    int unit,
    const char *title,
    dbal_queue_resources_get_t dbal_resource_get_func);*/
/**
* \brief -  Gets the rows data from DBAL API and prints them in a table that was created prior.
*/
static shr_error_e sh_dnx_queue_resources_table_fill(
    int unit,
    prt_control_t * prt_ctr,
    dbal_queue_resources_get_t dbal_resource_get_func);
/**
* \brief -  Prints a single table of the ECGM Resource per Service Pool.
*             Can be used to print either current values or max values depending on the dbal_resource_get_func argument.
*/
static shr_error_e sh_dnx_per_service_pool_resources_table_print(
    int unit,
    const char *title,
    dbal_service_pool_resources_get_t dbal_resource_get_func,
    uint32 is_cur,
    sh_sand_control_t * sand_control);
/**
* \brief -  Prints a single table of the ECGM Resource per Service Pool per TC.
*             Can be used to print either current values or max values depending on the dbal_resource_get_func argument.
*/
static shr_error_e sh_dnx_per_service_pool_tc_resources_table_print(
    int unit,
    const char *title,
    dbal_service_pool_tc_resources_get_t dbal_resource_get_func,
    sh_sand_control_t * sand_control);
/**
* \brief -  Disables statistics tracking in the ECGM block for all available cores.
*/
static shr_error_e sh_dnx_statistics_tracking_disable(
    int unit);
/**
* \brief -  Enables statistics tracking in the ECGM block for all available cores.
*/ static shr_error_e sh_dnx_statistics_tracking_enable(
    int unit);
/**
 * }
 */

/**
 * command
 * {
 */
shr_error_e
sh_dnx_tm_egress_congestion_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /** flags are used to manage the command's options. */
    int all_flag, current_flag, max_flag;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("all", all_flag);
    SH_SAND_GET_BOOL("current", current_flag);
    SH_SAND_GET_BOOL("max", max_flag);

    /** enter if user entered "TM EGress CoNGeSTion" or "TM EGress CoNGeSTion ALL" */
    if (current_flag == FALSE && max_flag == FALSE)
    {
        SHR_IF_ERR_EXIT(sh_dnx_tm_egress_congestion_current_option(unit, args, sand_control));
        SHR_IF_ERR_EXIT(sh_dnx_tm_egress_congestion_max_option(unit, args, sand_control));
    }
    /** enter if user entered "TM EGress CoNGeSTion CURrent" */
    else if (all_flag == FALSE && current_flag == TRUE && max_flag == FALSE)
    {
        SHR_IF_ERR_EXIT(sh_dnx_tm_egress_congestion_current_option(unit, args, sand_control));
    }
    /** enter if user entered "TM EGress CoNGeSTion MAX" */
    else if (all_flag == FALSE && current_flag == FALSE && max_flag == TRUE)
    {
        SHR_IF_ERR_EXIT(sh_dnx_tm_egress_congestion_max_option(unit, args, sand_control));
    }
    /** enter else if user typed an illegal command (e.g. "TM EGress CoNGeSTion MAX ALL") */
    else
    {
        SHR_CLI_EXIT(_SHR_E_PARAM,
                     "Illegal input: Only one option may be specified with TM EGress CoNGeSTion command.\n");

    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * }
 */

/**
 * static functions
 * {
 */
static shr_error_e
sh_dnx_tm_egress_congestion_current_option(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /** each function call in the block prints a single table */
    SHR_IF_ERR_EXIT(sh_dnx_per_core_resources_table_print
                    (unit, "ECGM Resources Per Core - Current Value",
                     dnx_ecgm_dbal_core_global_resources_statistics_current_get, sand_control));
    /** The function calls below are commented out because a new function which
     * prints all 3 Elements is used instead.
     */
/*    SHR_IF_ERR_EXIT(sh_dnx_per_interface_resources_table_print
                    (unit, "ECGM Resources Per Interface - Current Value",
                     dnx_ecgm_dbal_interface_resources_statistics_current_get));
    SHR_IF_ERR_EXIT(sh_dnx_per_port_resources_table_print
                    (unit, "ECGM Resources Per Logical Port - Current Value",
                     dnx_ecgm_dbal_port_resources_statistics_current_get));
    SHR_IF_ERR_EXIT(sh_dnx_per_queue_resources_table_print
                    (unit, "ECGM Resources Per Queue(Port + TC) - Current Value",
                     dnx_ecgm_dbal_queue_resources_statistics_current_get));*/
    SHR_IF_ERR_EXIT(sh_dnx_per_element_resources_table_print(unit, "ECGM Resources Per Element - Current Value",
                                                             dnx_ecgm_dbal_interface_resources_statistics_current_get,
                                                             dnx_ecgm_dbal_port_resources_statistics_current_get,
                                                             dnx_ecgm_dbal_queue_resources_statistics_current_get,
                                                             sand_control));
    SHR_IF_ERR_EXIT(sh_dnx_per_service_pool_resources_table_print
                    (unit, "ECGM Resources Per Service Pool - Current Value",
                     dnx_ecgm_dbal_service_pool_resources_statistics_current_get, 1, sand_control));
    SHR_IF_ERR_EXIT(sh_dnx_per_service_pool_tc_resources_table_print
                    (unit, "ECGM Resources Per Service Pool Per TC - Current Value",
                     dnx_ecgm_dbal_service_pool_tc_resources_statistics_current_get, sand_control));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_tm_egress_congestion_max_option(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Because max counters are updated continuously
        it is recommended to disable statistics tracking when reading the current statistics records,
        so the CPU will get a coherent snapshot of the system. */
    SHR_IF_ERR_EXIT(sh_dnx_statistics_tracking_disable(unit));

    /** each function call in the block prints a single table */
    SHR_IF_ERR_EXIT(sh_dnx_per_core_resources_table_print
                    (unit, "ECGM Resources Per Core - Max Value",
                     dnx_ecgm_dbal_core_global_resources_statistics_max_get, sand_control));
    /** The function calls below are commented out because a new function which
     * prints all 3 Elements is used instead.
     */
/*    SHR_IF_ERR_EXIT(sh_dnx_per_interface_resources_table_print
                    (unit, "ECGM Resources Per Interface - Max Value",
                     dnx_ecgm_dbal_interface_resources_statistics_max_get));
    SHR_IF_ERR_EXIT(sh_dnx_per_port_resources_table_print
                    (unit, "ECGM Resources Per Logical Port - Max Value",
                     dnx_ecgm_dbal_port_resources_statistics_max_get));
    SHR_IF_ERR_EXIT(sh_dnx_per_queue_resources_table_print
                    (unit, "ECGM Resources Per Queue(Port + TC) - Max Value",
                     dnx_ecgm_dbal_queue_resources_statistics_max_get));*/
    SHR_IF_ERR_EXIT(sh_dnx_per_element_resources_table_print(unit, "ECGM Resources Per Element - Max Value",
                                                             dnx_ecgm_dbal_interface_resources_statistics_max_get,
                                                             dnx_ecgm_dbal_port_resources_statistics_max_get,
                                                             dnx_ecgm_dbal_queue_resources_statistics_max_get,
                                                             sand_control));
    /*
     * clear max statistics after read
     */
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_resources_statistics_max_clear(unit));

    SHR_IF_ERR_EXIT(sh_dnx_per_service_pool_resources_table_print
                    (unit, "ECGM Resources Per Service Pool - Max Value",
                     dnx_ecgm_dbal_service_pool_resources_statistics_max_get, 0, sand_control));
    SHR_IF_ERR_EXIT(sh_dnx_per_service_pool_tc_resources_table_print
                    (unit, "ECGM Resources Per Service Pool Per TC - Max Value",
                     dnx_ecgm_dbal_service_pool_tc_resources_statistics_max_get, sand_control));

    /** After finishing reading and writing to the max counters the stat tracking should be enabled again. */
    SHR_IF_ERR_EXIT(sh_dnx_statistics_tracking_enable(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_per_core_resources_table_print(
    int unit,
    const char *title,
    dbal_core_resources_get_t dbal_resource_get_func,
    sh_sand_control_t * sand_control)
{
    shr_error_e rv = _SHR_E_NONE;
    ecgm_core_global_resources_statistics_t core_resource;
    int core_index;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Set "Core Global Resources" table format */
    PRT_TITLE_SET("%s", title);
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("total PDs");
    PRT_COLUMN_ADD("UC PDs");
    PRT_COLUMN_ADD("MC PDs");
    PRT_COLUMN_ADD("total DBs");
    PRT_COLUMN_ADD("UC DBs");
    PRT_COLUMN_ADD("MC DBs");

    /** Fill "Core Global Resources" table content */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_index)
    {
        rv = dbal_resource_get_func(unit, core_index, &core_resource);

        /** print all non zero rows */
        if (core_resource.total_pd != 0 ||
            core_resource.uc_pd != 0 ||
            core_resource.mc_pd != 0 ||
            core_resource.total_db != 0 || core_resource.uc_db != 0 || core_resource.mc_db != 0)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", core_index);
            if (rv == _SHR_E_NONE)
            {
                PRT_CELL_SET("%d", core_resource.total_pd);
                PRT_CELL_SET("%d", core_resource.uc_pd);
                PRT_CELL_SET("%d", core_resource.mc_pd);
                PRT_CELL_SET("%d", core_resource.total_db);
                PRT_CELL_SET("%d", core_resource.uc_db);
                PRT_CELL_SET("%d", core_resource.mc_db);
            }
            else
            {
                PRT_CELL_SET("%s", "N/A");
                PRT_CELL_SET("%s", "N/A");
                PRT_CELL_SET("%s", "N/A");
                PRT_CELL_SET("%s", "N/A");
                PRT_CELL_SET("%s", "N/A");
                PRT_CELL_SET("%s", "N/A");
            }
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_per_element_resources_table_print(
    int unit,
    const char *title,
    dbal_interface_resources_get_t dbal_resource_interface_get_func,
    dbal_port_resources_get_t dbal_resource_port_get_func,
    dbal_queue_resources_get_t dbal_resource_queue_get_func,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Set "Element Resources" table format. */
    PRT_TITLE_SET("%s", title);
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("Element Type");
    PRT_COLUMN_ADD("Element ID");
    PRT_COLUMN_ADD("UC PDs");
    PRT_COLUMN_ADD("MC PDs");
    PRT_COLUMN_ADD("UC DBs");
    PRT_COLUMN_ADD("MC DBs");

    SHR_IF_ERR_EXIT(sh_dnx_interface_resources_table_fill(unit, prt_ctr, dbal_resource_interface_get_func));
    SHR_IF_ERR_EXIT(sh_dnx_port_resources_table_fill(unit, prt_ctr, dbal_resource_port_get_func));
    SHR_IF_ERR_EXIT(sh_dnx_queue_resources_table_fill(unit, prt_ctr, dbal_resource_queue_get_func));

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*static shr_error_e
sh_dnx_per_interface_resources_table_print(
    int unit,
    const char *title,
    dbal_interface_resources_get_t dbal_resource_get_func)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    * Set "Interface Resources" table format.
    PRT_TITLE_SET("%s", title);
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("Interface");
    PRT_COLUMN_ADD("UC PDs");
    PRT_COLUMN_ADD("MC PDs");
    PRT_COLUMN_ADD("UC DBs");
    PRT_COLUMN_ADD("MC DBs");

    SHR_IF_ERR_EXIT(sh_dnx_interface_resources_table_fill(unit, prt_ctr, dbal_resource_get_func));

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}*/

static shr_error_e
sh_dnx_interface_resources_table_fill(
    int unit,
    prt_control_t * prt_ctr,
    dbal_interface_resources_get_t dbal_resource_get_func)
{
    shr_error_e res1 = _SHR_E_NONE;
    shr_error_e res2 = _SHR_E_NONE;
    uint32 if_pd_priority_low;
    uint32 if_pd_priority_high;
    uint32 if_db_priority_low;
    uint32 if_db_priority_high;
    uint32 uc_pd = UTILEX_UINT_MAX;
    uint32 uc_db = UTILEX_UINT_MAX;
    uint32 mc_pd = UTILEX_UINT_MAX;
    uint32 mc_db = UTILEX_UINT_MAX;
    int core_index;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_index)
    {
        uint32 interface;
        for (interface = 0; interface < dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit); interface++)
        {
            /** get uc_pd and uc_db values */
            res1 = dbal_resource_get_func
                (unit, core_index, 0, DBAL_ENUM_FVAL_ECGM_PRIORITY_LOW, interface, &if_pd_priority_low,
                 &if_db_priority_low);
            res2 = dbal_resource_get_func
                (unit, core_index, 0, DBAL_ENUM_FVAL_ECGM_PRIORITY_HIGH, interface, &if_pd_priority_high,
                 &if_db_priority_high);

            if ((res1 == _SHR_E_NONE) && (res2 == _SHR_E_NONE))
            {
                /** since the print is agnostic to the priority level, both priority values are combined to represent total UC resources per Interface*/
                uc_pd = if_pd_priority_low + if_pd_priority_high;
                uc_db = if_db_priority_low + if_db_priority_high;
            }

            /** get mc_pd and mc_db values */
            res1 = dbal_resource_get_func
                (unit, core_index, 1, DBAL_ENUM_FVAL_ECGM_PRIORITY_LOW, interface, &if_pd_priority_low,
                 &if_db_priority_low);
            res2 = dbal_resource_get_func
                (unit, core_index, 1, DBAL_ENUM_FVAL_ECGM_PRIORITY_HIGH, interface, &if_pd_priority_high,
                 &if_db_priority_high);

            if ((res1 == _SHR_E_NONE) && (res2 == _SHR_E_NONE))
            {
                /** since the print is agnostic to the priority level, both priority values are combined to represent total MC resources per Interface */
                mc_pd = if_pd_priority_low + if_pd_priority_high;
                mc_db = if_db_priority_low + if_db_priority_high;
            }

            /** print all non zero rows */
            if (uc_pd != 0 || uc_db != 0 || mc_pd != 0 || mc_db != 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%d", core_index);
                PRT_CELL_SET("%s", "Interface");
                PRT_CELL_SET("%d", interface);
                if (uc_pd != UTILEX_UINT_MAX)
                {
                    PRT_CELL_SET("%d", uc_pd);
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
                if (mc_pd != UTILEX_UINT_MAX)
                {
                    PRT_CELL_SET("%d", mc_pd);
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
                if (uc_db != UTILEX_UINT_MAX)
                {
                    PRT_CELL_SET("%d", uc_db);
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
                if (mc_db != UTILEX_UINT_MAX)
                {
                    PRT_CELL_SET("%d", mc_db);
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*static shr_error_e
sh_dnx_per_port_resources_table_print(
    int unit,
    const char *title,
    dbal_port_resources_get_t dbal_resource_get_func)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    * Set "Per Port Resources" table format
    PRT_TITLE_SET("%s", title);
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("%s", "Port");
    PRT_COLUMN_ADD("UC PDs");
    PRT_COLUMN_ADD("MC PDs");
    PRT_COLUMN_ADD("UC DBs");
    PRT_COLUMN_ADD("MC DBs");

    SHR_IF_ERR_EXIT(sh_dnx_port_resources_table_fill(unit, prt_ctr, dbal_resource_get_func));

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}*/

static shr_error_e
sh_dnx_port_resources_table_fill(
    int unit,
    prt_control_t * prt_ctr,
    dbal_port_resources_get_t dbal_resource_get_func)
{
    shr_error_e res1 = _SHR_E_NONE;
    shr_error_e res2 = _SHR_E_NONE;
    uint32 uc_pd;
    uint32 mc_pd;
    uint32 uc_db;
    uint32 mc_db;
    int core_index;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_index)
    {
        bcm_pbmp_t ports_bmp;
        bcm_port_t logical_port;
        int base_q_pair;

        /** get valid ports bitmap */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, core_index, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &ports_bmp));

        /** iterate all valid ports */
        BCM_PBMP_ITER(ports_bmp, logical_port)
        {
            /** get Base QueuePair value from valid port */
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));

            res1 = dbal_resource_get_func(unit, core_index, 0, base_q_pair, &uc_pd, &uc_db);
            res2 = dbal_resource_get_func(unit, core_index, 1, base_q_pair, &mc_pd, &mc_db);

            /** print all non zero rows */
            if (uc_pd != 0 || uc_db != 0 || mc_pd != 0 || mc_db != 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%d", core_index);
                PRT_CELL_SET("%s", "Logical Port");
                PRT_CELL_SET("%d", logical_port);
                if (res1 == _SHR_E_NONE)
                {
                    PRT_CELL_SET("%d", uc_pd);
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
                if (res2 == _SHR_E_NONE)
                {
                    PRT_CELL_SET("%d", mc_pd);
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
                if (res1 == _SHR_E_NONE)
                {
                    PRT_CELL_SET("%d", uc_db);
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
                if (res2 == _SHR_E_NONE)
                {
                    PRT_CELL_SET("%d", mc_db);
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*static shr_error_e
sh_dnx_per_queue_resources_table_print(
    int unit,
    const char *title,
    dbal_queue_resources_get_t dbal_resource_get_func)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    * Set "Per Queue Resources" table format
    PRT_TITLE_SET("%s", title);
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("%s", "Queue");
    PRT_COLUMN_ADD("UC PDs");
    PRT_COLUMN_ADD("MC PDs");
    PRT_COLUMN_ADD("UC DBs");
    PRT_COLUMN_ADD("MC DBs");

    SHR_IF_ERR_EXIT(sh_dnx_queue_resources_table_fill(unit, prt_ctr, dbal_resource_get_func));

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}*/

static shr_error_e
sh_dnx_queue_resources_table_fill(
    int unit,
    prt_control_t * prt_ctr,
    dbal_queue_resources_get_t dbal_resource_get_func)
{
    shr_error_e res1 = _SHR_E_NONE;
    shr_error_e res2 = _SHR_E_NONE;
    uint32 uc_pd;
    uint32 mc_pd;
    uint32 uc_db;
    uint32 mc_db;
    int core_index;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_index)
    {
        uint32 q_pair;
        for (q_pair = 0; q_pair < dnx_data_egr_queuing.params.nof_q_pairs_get(unit); q_pair++)
        {
            res1 = dbal_resource_get_func(unit, core_index, 0, q_pair, &uc_pd, &uc_db);
            res2 = dbal_resource_get_func(unit, core_index, 1, q_pair, &mc_pd, &mc_db);

            /** print all non zero rows */
            if (uc_pd != 0 || uc_db != 0 || mc_pd != 0 || mc_db != 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%d", core_index);
                PRT_CELL_SET("%s", "Queue");
                PRT_CELL_SET("%d", q_pair);
                if (res1 == _SHR_E_NONE)
                {
                    PRT_CELL_SET("%d", uc_pd);
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
                if (res2 == _SHR_E_NONE)
                {
                    PRT_CELL_SET("%d", mc_pd);
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
                if (res1 == _SHR_E_NONE)
                {
                    PRT_CELL_SET("%d", uc_db);
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
                if (res2 == _SHR_E_NONE)
                {
                    PRT_CELL_SET("%d", mc_db);
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_per_service_pool_resources_table_print(
    int unit,
    const char *title,
    dbal_service_pool_resources_get_t dbal_resource_get_func,
    uint32 is_cur,
    sh_sand_control_t * sand_control)
{
    shr_error_e rv = _SHR_E_NONE;
    ecgm_service_pool_resources_statistics_t sp_resource = { 0 };
    int core_index;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Set "Per Service Pool Resources" table format */
    PRT_TITLE_SET("%s", title);
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("Service Pool");
    PRT_COLUMN_ADD("PDs");
    PRT_COLUMN_ADD("DBs");
    if (is_cur)
    {
        PRT_COLUMN_ADD("Reserved PDs");
        PRT_COLUMN_ADD("Reserved DBs");
    }

    /** Fill "Per Service Pool Resources" table content */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_index)
    {
        int sp;
        for (sp = 0; sp < dnx_data_ecgm.core_resources.nof_sp_get(unit); sp++)
        {
            rv = dbal_resource_get_func(unit, core_index, sp, &sp_resource);

            /** print all non zero rows */
            if (sp_resource.pd != 0 ||
                sp_resource.db != 0 || sp_resource.reserved_pd != 0 || sp_resource.reserved_db != 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%d", core_index);
                PRT_CELL_SET("%d", sp);
                if (rv == _SHR_E_NONE)
                {
                    PRT_CELL_SET("%d", sp_resource.pd);
                    PRT_CELL_SET("%d", sp_resource.db);

                    /** reserved resources are only relevant to current counters */
                    if (is_cur)
                    {
                        PRT_CELL_SET("%d", sp_resource.reserved_pd);
                        PRT_CELL_SET("%d", sp_resource.reserved_db);
                    }
                }
                else
                {
                    PRT_CELL_SET("%s", "N/A");
                    PRT_CELL_SET("%s", "N/A");
                    PRT_CELL_SET("%s", "N/A");
                    PRT_CELL_SET("%s", "N/A");
                }
            }
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_per_service_pool_tc_resources_table_print(
    int unit,
    const char *title,
    dbal_service_pool_tc_resources_get_t dbal_resource_get_func,
    sh_sand_control_t * sand_control)
{
    shr_error_e rv = _SHR_E_NONE;
    uint32 sp_tc_pd;
    uint32 sp_tc_db;
    int core_index;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Set "Per Service Pool TC Resources" table format */
    PRT_TITLE_SET("%s", title);
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("Service Pool");
    PRT_COLUMN_ADD("TC");
    PRT_COLUMN_ADD("PDs");
    PRT_COLUMN_ADD("DBs");

    /** Fill "Per Service Pool TC Resources" table content */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_index)
    {
        int sp;
        for (sp = 0; sp < dnx_data_ecgm.core_resources.nof_sp_get(unit); sp++)
        {
            int tc;
            for (tc = 0; tc < DNX_COSQ_NOF_TC; tc++)
            {
                rv = dbal_resource_get_func(unit, core_index, sp, tc, &sp_tc_pd, &sp_tc_db);

                /** print all non zero rows */
                if (sp_tc_pd != 0 || sp_tc_db != 0)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%d", core_index);
                    PRT_CELL_SET("%d", sp);
                    PRT_CELL_SET("%d", tc);
                    if (rv == _SHR_E_NONE)
                    {
                        PRT_CELL_SET("%d", sp_tc_pd);
                        PRT_CELL_SET("%d", sp_tc_db);
                    }
                    else
                    {
                        PRT_CELL_SET("%s", "N/A");
                        PRT_CELL_SET("%s", "N/A");
                    }
                }
            }
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_statistics_tracking_disable(
    int unit)
{
    int core_index;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_index)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_disable_max_statistics_tracking_set(unit, core_index, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_statistics_tracking_enable(
    int unit)
{
    int core_index;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_index)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_disable_max_statistics_tracking_set(unit, core_index, 0));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * }
 */
