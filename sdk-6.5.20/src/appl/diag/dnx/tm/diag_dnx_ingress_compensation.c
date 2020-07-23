/** \file diag_dnx_ingress_compensation.c
 *
 * Main diagnostics for ingress compensation
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*************
 * INCLUDES  *
 */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm/types.h>
#include <bcm/stat.h>
#include <bcm/cosq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ipq_access.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <sal/appl/sal.h>
#include <src/bcm/dnx/stat/crps/crps_mgmt_internal.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include "diag_dnx_ingress_compensation.h"
#include "diag_dnx_queue.h"

/**
 * \brief - Print a row for header truncate compensation
 *        component
 */
static shr_error_e
diag_dnx_ingress_compensation_header_truncate_get(
    int unit,
    int core_id,
    int database_id,
    int *hdr_truncate_enabled)
{
    bcm_stat_counter_database_t database;
    bcm_stat_counter_interface_t config;
    bcm_stat_counter_command_id_key_t key;
    int engine_id;
    int flags;
    SHR_FUNC_INIT_VARS(unit);

    bcm_stat_counter_command_id_key_t_init(&key);
    flags = (database_id == -1) ? BCM_STAT_EXTERNAL : 0;
    /** If STIF database param wont be set and will be -1*/
    /** CRPS case */
    if (database_id != -1)
    {
        database.core_id = core_id;
        database.database_id = database_id;
        if (_SHR_E_NONE != dnx_crps_mgmt_database_structure_verify(unit, &database, FALSE))
        {
            LOG_CLI((BSL_META("database verification failed: unit=%d core_id=%d database_id=%d\n"),
                     unit, database.core_id, database.database_id));
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_id, database_id, &engine_id));
        if (engine_id == DNX_CRPS_MGMT_ENGINE_INVALID)
        {
            LOG_CLI((BSL_META("No engines attached to the database.\n")));
            SHR_EXIT();
        }
        /** Get the interface of the database */
        SHR_IF_ERR_EXIT(bcm_stat_counter_interface_get(unit, 0, &database, &config));
        /** Get if header truncate is enabled */
        key.command_id = config.command_id;
        key.source = config.source;
    }
    key.core_id = core_id;
    SHR_IF_ERR_EXIT(bcm_stat_pkt_size_adjust_select_get
                    (unit, flags, &key, bcmStatPktSizeSelectCounterIngressHeaderTruncate, hdr_truncate_enabled));

exit:
    SHR_FUNC_EXIT;

}

/**
 * See diag_dnx_ingress_compensation
 */
shr_error_e
sh_dnx_ingress_compensation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int user_core, current_core, database_id;
    int qid, base_qid, num_cos, hdr_app;
    bcm_cos_queue_t cosq = 0;
    int is_sch, is_crps, is_stif, is_general;
    int is_qid_present, is_port_present, is_hdr_app_present, is_database_present;
    int delta;
    int is_hdr_truncate_enabled = 1;
    char *title;
    bcm_gport_t gport;
    bcm_cosq_pkt_size_adjust_info_t adjust_info;
    int sch_per_port_comp_enabled = 1;
    int port;
    uint8 is_asymm = FALSE;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("queue", is_qid_present);
    SH_SAND_IS_PRESENT("port", is_port_present);
    SH_SAND_IS_PRESENT("header_append", is_hdr_app_present);
    SH_SAND_IS_PRESENT("database", is_database_present);

    SH_SAND_GET_INT32("queue", qid);
    SH_SAND_GET_INT32("port", port);
    SH_SAND_GET_INT32("header_append", hdr_app);
    SH_SAND_GET_INT32("database", database_id);
    SH_SAND_GET_BOOL("scheduler", is_sch);
    SH_SAND_GET_BOOL("crps", is_crps);
    SH_SAND_GET_BOOL("stif", is_stif);
    SH_SAND_GET_INT32("core", user_core);

    adjust_info.flags = 0;
    adjust_info.cosq = cosq;
    adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceGlobal;
    adjust_info.source_info.source_id = 0;

    /** Check chosen options:
     *  allowed are : some/none/all paramters
     *  queue/port/header_append and user can set either one
     *  schduler/crps/stif
     *
     *  Only for crps parameter database is required in order to
     *  print the compensation template that was chosen
     *
     *  As queue and header append are disabled for STIF and CRPS -
     *  queue and header append options are not allowed for stif and
     *  crps
     *
     *  Example:
     *  tm ingress compensation sch queue=32 header_append=0 port=1
     *  tm ingress compensation crps database=10 core=0
     *  tm ingress compendation stif core=0 port=1 */

    /**************** VERIFICATION OF THE PARAMETERS*/
    /** verify that either one or none options is chosen */
    if ((is_sch || is_crps) && (is_crps || is_stif) && (is_stif || is_sch))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM,
                     "unit %d for options please give either SCHEDULER or CRPS or STIF or don'f set this option at all \n",
                     unit);
    }
    title = is_sch ? "Scheduler" : (is_crps ? "Counter Processor" : (is_stif ? "Statistic Interface" : "General"));
    is_general = !(is_sch || is_crps || is_stif); /** if none is set - general print*/
    /** verify that database parameter is provided only if crps
     *  option is selected */
    if (is_crps == 0 && is_database_present == 1)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d parameter database is required only for CRPS \n", unit);
    }
    /** verify that queue and header_append are not selected */
    if (is_sch == 0 && (is_qid_present || is_hdr_app_present))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d parameter queue/header_append is required only for SCH\n", unit);
    }
    /** verify that stif is enabled if selected as option */
    if (is_stif && dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_stif_enable) == FALSE)
    {
        LOG_CLI((BSL_META("Statistic Interface is not enabled.\n")));
        SHR_EXIT();
    }
    /**************** PRINT TABLE - INFO and COLUMNS*/
    PRT_TITLE_SET("Ingress Compensation - %s", title);
    if (is_general)
    {
        PRT_INFO_ADD("For more information select compensation type - sch/crps/stif.");
    }
    PRT_COLUMN_ADD("Compensation component");
    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("Enabled");
    PRT_COLUMN_ADD("Delta Value [Bytes]");

    /**************** PRINT TABLE - ROW HEADER TRUNCATE*/
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Header Truncate");
    PRT_CELL_SET(" -- ");
    if (is_stif || is_crps)
    {
        if (is_database_present || is_stif)
        {
            SHR_IF_ERR_EXIT(diag_dnx_ingress_compensation_header_truncate_get
                            (unit, user_core, database_id, &is_hdr_truncate_enabled));
            PRT_CELL_SET("%s", (sh_sand_bool_str(is_hdr_truncate_enabled)));
            if (is_hdr_truncate_enabled)
            {
                PRT_CELL_SET("Automatic");
            }
            else
            {
                PRT_CELL_SET(" -- ");
            }
        }
        else
        {
            PRT_CELL_SET("Database specific");
            PRT_CELL_SET(" -- ");
        }
    }
    else
    {
        if (is_general)
        {
            PRT_CELL_SET("SCH/CRPS/STIF specific");
            PRT_CELL_SET(" -- ");
        }
        else
        {
            PRT_CELL_SET("%s", "True");
            PRT_CELL_SET("Automatic");
        }
    }

    /**************** PRINT TABLE - ROW IRPP DELTA*/
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("IRPP Delta");
    PRT_CELL_SET(" -- ");
    PRT_CELL_SET("True");
    PRT_CELL_SET("Automatic");

    /**************** PRINT TABLE - ROW QUEUE*/
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    if (is_qid_present)
    {
        DNXCMN_CORES_ITER(unit, user_core, current_core)
        {
            /** Get base queue on current core */

            SHR_IF_ERR_EXIT(sh_dnx_tm_queue_base_queue_get(unit, qid, current_core, &base_qid, &num_cos));
            if (base_qid < 0)
            {
                /** Queue is invalid for current core, continue to next one */
                continue;
            }
            /*
             * Check if VOQ is symmetrical on all cores.
             * If the queue is symmetrical, current core is 1 and it was not
             * specifically requested by user, we do not want to print anything for it.
             * For symmetrical queues it is enough to print only data for core 0, as it will be the same on core 1.
             */
            SHR_IF_ERR_EXIT(dnx_ipq_db.base_queue_is_asymm.bit_get(unit, base_qid, &is_asymm));
            if ((is_asymm == FALSE) && (current_core != 0) && (user_core == BCM_CORE_ALL))
            {
                continue;
            }
            /** get queue bundle gport */
            BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(gport, current_core, base_qid);
            adjust_info.gport = gport;
            adjust_info.cosq = qid - base_qid;
            SHR_IF_ERR_EXIT(bcm_cosq_gport_pkt_size_adjust_get(unit, &adjust_info, &delta));
            if (is_asymm == FALSE)
            {
                PRT_CELL_SET("Queue");
            }
            else
            {
                PRT_CELL_SET("Queue on core %d", current_core);
            }

            PRT_CELL_SET("%d", qid);
            PRT_CELL_SET("True");
            PRT_CELL_SET("%d", delta);
            /**************** PRINT TABLE - ROW INPUT PORT*/
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        }
    }
    else
    {
        /** VOQ credit class is disabled for crps and stif/ enabled
         *  for scheduler */
        PRT_CELL_SET("Queue");
        PRT_CELL_SET(" -- ");
        if (is_general)
        {
            PRT_CELL_SET("SCH/CRPS/STIF specific");
        }
        else
        {
            PRT_CELL_SET("%s", (sh_sand_bool_str((is_crps == 0) && (is_stif == 0))));
        }
        PRT_CELL_SET(" -- ");
        /**************** PRINT TABLE - ROW INPUT PORT*/
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    }

    PRT_CELL_SET("Input Port");
    /** First check for scheduler -
     *  compensation per Input port can be enabled/disabled as this
     *  is done for all ports */
    if (is_sch)
    {
        SHR_IF_ERR_EXIT(bcm_cosq_control_get
                        (unit, 0, 0, bcmCosqControlSourcePortSchedCompensationEnable, &sch_per_port_comp_enabled));
    }
    if (is_port_present)
    {
        PRT_CELL_SET("%d", port);
        BCM_GPORT_LOCAL_SET(gport, port);
        adjust_info.gport = gport;
        SHR_IF_ERR_EXIT(bcm_cosq_gport_pkt_size_adjust_get(unit, &adjust_info, &delta));
        PRT_CELL_SET("%s", (sh_sand_bool_str(sch_per_port_comp_enabled)));
        if (sch_per_port_comp_enabled)
        {
            PRT_CELL_SET("%d", delta);
        }
        else
        {
            /** if the port is disabled we dont want to print delta value */
            PRT_CELL_SET(" -- ");
        }
    }
    else
    {
        /** if no port paramter is given - print only that this
         *  compensation component is enabled
         *  For Scheduler enable/disable is done per port so if no port
         *  is given no value will be printed */
        PRT_CELL_SET(" -- ");
        if (is_general)
        {
            PRT_CELL_SET("SCH/CRPS/STIF specific");
        }
        else
        {
            PRT_CELL_SET("%s", (sh_sand_bool_str(sch_per_port_comp_enabled)));
        }
        PRT_CELL_SET(" -- ");
    }

    /**************** PRINT TABLE - ROW HEADER APPEND*/
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("Header Append Profile");
    if (is_hdr_app_present)
    {
        PRT_CELL_SET("%d", hdr_app);
        BCM_GPORT_PROFILE_SET(gport, hdr_app);
        adjust_info.gport = gport;
        SHR_IF_ERR_EXIT(bcm_cosq_gport_pkt_size_adjust_get(unit, &adjust_info, &delta));
        PRT_CELL_SET("True");
        PRT_CELL_SET("%d", delta);
    }
    else
    {
        /** Header Append is disabled for crps and stif/ enabled
         *  for scheduler */
        PRT_CELL_SET(" -- ");
        if (is_general)
        {
            PRT_CELL_SET("SCH/CRPS/STIF specific");
        }
        else
        {
            PRT_CELL_SET("%s", (sh_sand_bool_str((is_crps == 0) && (is_stif == 0))));
        }
        PRT_CELL_SET(" -- ");
    }

    PRT_COMMITX;

    LOG_CLI((BSL_META
             ("\n*Note that configured compensation values affect all Compensation types\n(SCH/CRPS/STIF) which have enabled that compensation component.\n\n")));
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_option_t sh_dnx_ingress_compensation_options[] = {
    {"queue", SAL_FIELD_TYPE_INT32, "queue id - relevant for SCH option only", "-1"}
    ,
    {"port", SAL_FIELD_TYPE_INT32, "port id", "-1"}
    ,
    {"header_append", SAL_FIELD_TYPE_INT32, "header append pointer - relevant for SCH option only", "-1"}
    ,
    {"database", SAL_FIELD_TYPE_INT32, "database id - relevant for CRPS option only", "-1"}
    ,
    {"scheduler", SAL_FIELD_TYPE_BOOL, "indicate to print information for scheduler", "false"}
    ,
    {"crps", SAL_FIELD_TYPE_BOOL, "indicate to print information for CRPS", "false"}
    ,
    {"stif", SAL_FIELD_TYPE_BOOL, "indicate to print information for STIF", "false"}
    ,
    {NULL}
};

sh_sand_man_t sh_dnx_ingress_compensation_man = {
    .brief = "Print ingress compensation values - global/crps/stif/scheduler.",
    .full =
        "'compensation command prints for specific compensation type (sch/stif/crps) or globally the relevant compensation values. \n",
    .synopsis = NULL,
    .examples = "\n" "sch header_append=10 queue=32\n" "crps port=1\n" "crps database=10\n" "stif"
};
