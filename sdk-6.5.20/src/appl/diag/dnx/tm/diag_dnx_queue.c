/** \file diag_dnx_queue.c
 *
 * 
 * diagnostics for ingress queues
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

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

#include <shared/util.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/appl/sal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dbal/dbal.h>

#include <bcm/fabric.h>
#include <bcm/stack.h>
#include <bcm/cosq.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/cosq/ingress/compensation.h>

/** */
#include <soc/dnx/swstate/auto_generated/access/dnx_ipq_access.h>

/** local */
#include "diag_dnx_ingress_congestion.h"
#include "diag_dnx_queue.h"
#include "diag_dnx_ingress_rate.h"

/**
 * { DEFINES
 */
#define DIAG_DNX_QUEUE_NOF_VALID_BUNDLE_SIZES       (2)

#define DIAG_DNX_QUEUE_ID_GET(voq_gport, cos, queue)     \
    do  \
    { \
        if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(voq_gport)) \
        { \
            queue = BCM_GPORT_MCAST_QUEUE_GROUP_QUEUE_GET(voq_gport); \
        } \
        else \
        { \
            queue = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(voq_gport); \
        } \
        queue += cos; \
    } while (0);

/**
 * }
 */

/**
 * \brief MAN for DNX TM Ingress Queue Mapping diagnostics
 */
static sh_sand_man_t sh_dnx_tm_queue_mapping_man = {
    .brief = "Queue Mapping",
    .full = "Dump mapping between system-port, base queue and modport for a specified Core ID. Default is Core ID=All",
    .synopsis = "[core=<Core ID | all>]",
    .examples = "core=0\n" "core=all\n" "core=max\n"
};

/**
 * \brief Options for DNX TM Ingress Queue Mapping diagnostics
 */
static sh_sand_option_t sh_dnx_tm_queue_mapping_options[] = {
/* *INDENT-OFF* */
    /*name          type                  desc                                        default   ext*/
    {NULL}
/* *INDENT-ON* */
};

/*
 * \brief
 *   Struct for holding queue mapping info.
 *   Used by sh_dnx_tm_queue_mapping_cmd.
 *
 *   master_sysport is useful when multiple system ports point to the
 *   same queue (the queue can point to only 1 system port, which is
 *   the master system port).
 */
typedef struct
{
    /** System port */
    uint32 sysport;
    /** Base queue */
    uint32 base_queue;
    /** Is queue asymmetrical */
    uint8 is_asymm;
    /** Master system port */
    uint32 master_sysport;
    /** The destination FAP */
    uint32 fap_id;
    /** The destination Port */
    uint32 port_id;
} sh_dnx_tm_queue_mapping_t;

/*
 * \brief
 *   Compare between base_queue in 2 sh_dnx_tm_queue_mapping_t structs.
 *   Used for sorting the structs in sh_dnx_tm_queue_mapping_cmd.
 */
static int
cmpfunc_base_queue(
    void *a,
    void *b)
{
    return (((sh_dnx_tm_queue_mapping_t *) (a))->base_queue - ((sh_dnx_tm_queue_mapping_t *) (b))->base_queue);
}

/**
 * \brief - Print mapping between system ports, base queues, and modports.
 */
static shr_error_e
sh_dnx_tm_queue_mapping_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 sysport = 0;
    uint32 is_valid = 0;
    uint32 base_queue = 0;
    uint32 base_queue_quartet = 0;
    uint32 master_sysport = 0;
    int table_index = 0;
    int raw_i = 0;
    sh_dnx_tm_queue_mapping_t *mapping_table = { NULL };
    bcm_gport_t master_sysport_gport = 0;
    bcm_gport_t modport_gport = 0;
    uint32 min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    bcm_core_t user_core = 0, current_core = 0;
    uint8 is_asymm = FALSE;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", user_core);

    SHR_ALLOC_SET_ZERO(mapping_table,
                       (sizeof(sh_dnx_tm_queue_mapping_t) * dnx_data_device.general.max_nof_system_ports_get(unit)),
                       "sh_dnx_tm_queue_mapping_cmd.sh_dnx_tm_queue_mapping_t", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_NULL_CHECK(mapping_table, _SHR_E_MEMORY, "Memory allocation failure");

    /*
     * Print table header.
     */
    PRT_TITLE_SET("Ingress Queue Mapping");
    PRT_COLUMN_ADD("System Port");
    PRT_COLUMN_ADD("Core ID");
    PRT_COLUMN_ADD("Base Queue");
    PRT_COLUMN_ADD("Master Sysport");
    PRT_COLUMN_ADD("Dest Fap");
    PRT_COLUMN_ADD("Dest TM Port");

    /*
     * Iterate over the cores
     */
    DNXCMN_CORES_ITER(unit, user_core, current_core)
    {
        /*
         * Reset mapping table and table index
         */
        table_index = 0;
        sal_memset(mapping_table, 0,
                   (sizeof(sh_dnx_tm_queue_mapping_t) * dnx_data_device.general.max_nof_system_ports_get(unit)));

        for (sysport = 0; sysport < dnx_data_device.general.max_nof_system_ports_get(unit); ++sysport)
        {
            /*
             * Get mapping from system-port to base_queue.
             * internal function is required since not all system ports are valid and connected to a queue,
             * so the API will print an error.
             */
            SHR_IF_ERR_CONT(dnx_cosq_ipq_dbal_system_port_to_voq_base_get
                            (unit, current_core, sysport, &base_queue, &is_valid));
            /*
             * Skip the sysport if not mapped to any queue
             */
            if (is_valid == 0)
            {
                continue;
            }

            /*
             * Check if VOQ is symmetrical on all cores.
             * If the queue is symmetrical, current core is 1 and it was not
             * specifically requested by user, we do not want to print anything for it.
             * For symmetrical queues it is enough to print only data for core 0, as it will be the same on core 1.
             */
            SHR_IF_ERR_EXIT(dnx_ipq_db.base_queue_is_asymm.bit_get(unit, base_queue, &is_asymm));
            if ((is_asymm == FALSE) && (current_core != 0) && (user_core == BCM_CORE_ALL))
            {
                continue;
            }

            /*
             * Get mapping from base queue to master sysport.
             */
            base_queue_quartet = base_queue / min_bundle_size;
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_get
                            (unit, current_core, base_queue_quartet, &master_sysport));

            /*
             * Get mapping from system port to modport.
             */
            BCM_GPORT_SYSTEM_PORT_ID_SET(master_sysport_gport, master_sysport);
            SHR_IF_ERR_EXIT(bcm_stk_sysport_gport_get(unit, master_sysport_gport, &modport_gport));

            /*
             * Store mapping information.
             */
            mapping_table[table_index].sysport = sysport;
            mapping_table[table_index].base_queue = base_queue;
            mapping_table[table_index].is_asymm = is_asymm;
            mapping_table[table_index].master_sysport = master_sysport;
            mapping_table[table_index].fap_id = BCM_GPORT_MODPORT_MODID_GET(modport_gport);
            mapping_table[table_index].port_id = BCM_GPORT_MODPORT_PORT_GET(modport_gport);

            ++table_index;
        }

        /*
         * Sort mapping info by base_queue.
         */
        _shr_sort(mapping_table, table_index, sizeof(sh_dnx_tm_queue_mapping_t), &cmpfunc_base_queue);

        /*
         * Print table row.
         */
        for (raw_i = 0; raw_i < table_index; ++raw_i)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);

            PRT_CELL_SET("%u", mapping_table[raw_i].sysport);
            if (mapping_table[raw_i].is_asymm == FALSE)
            {
                PRT_CELL_SET("ALL");
            }
            else
            {
                PRT_CELL_SET("%u", current_core);
            }
            PRT_CELL_SET("%u", mapping_table[raw_i].base_queue);
            PRT_CELL_SET("%u", mapping_table[raw_i].master_sysport);
            PRT_CELL_SET("%u", mapping_table[raw_i].fap_id);
            PRT_CELL_SET("%u", mapping_table[raw_i].port_id);
        }
    }

    PRT_COMMITX;

exit:
    SHR_FREE(mapping_table);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief MAN for DNX TM Ingress Queue Information diagnostics
 */
static sh_sand_man_t sh_dnx_tm_queue_info_man = {
/* *INDENT-OFF* */
    .brief    = "Queue Info",
    .full     = "Dump information about the queue, or about the queues in general, in case no queue is specified",
    .synopsis = "[ID=<queue>] [core=<Core ID | all>]",
    .examples = "\n"
                "id=136 core=0\n"
                "id=144 core=all",
/* *INDENT-ON* */
};

/**
 * \brief Options for DNX TM Ingress Queue Information diagnostics
 */
static sh_sand_option_t sh_dnx_tm_queue_info_options[] = {
/* *INDENT-OFF* */
    /*name          type                  desc                                        default   ext      range     flags */
    {"id",          SAL_FIELD_TYPE_INT32, "Queue number",                             "-1",     NULL},
    /** detailed option is for legacy command support, it is not really used */
    {"detailed",  SAL_FIELD_TYPE_INT32,  "Legacy",          "0",     NULL,    NULL,     SH_SAND_ARG_QUIET},
    {NULL}
/* *INDENT-ON* */
};

/**
 * \brief - Print global info about queues.
 */
static shr_error_e
sh_dnx_tm_queue_global_info_cmd(
    int unit,
    sh_sand_control_t * sand_control)
{
    int min_uc_queue = 0;
    int max_uc_queue = 0;
    int max_mc_queue = -1;
    int is_fmq_sched_mode = 0;
    char *fmq_mode = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get FMQ information.
     */
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_IF_ERR_EXIT(bcm_fabric_control_get(unit, bcmFabricMulticastQueueMax, &max_mc_queue));
        SHR_IF_ERR_EXIT(bcm_fabric_control_get(unit, bcmFabricMulticastSchedulerMode, &is_fmq_sched_mode));
    }

    /*
     * Print Unicast Info.
     */
    PRT_TITLE_SET("Unicast Queues Info");
    PRT_COLUMN_ADD("Min UC Queue");
    PRT_COLUMN_ADD("Max UC Queue");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    min_uc_queue = max_mc_queue + 1;
    max_uc_queue = dnx_data_ipq.queues.nof_queues_get(unit) - 1;
    if (min_uc_queue > max_uc_queue)
    {
        /*
         * All queues are multicast queues.
         */
        PRT_CELL_SET("No Unicast Queues");
        PRT_CELL_SET("No Unicast Queues");
    }
    else
    {
        PRT_CELL_SET("%d", min_uc_queue);
        PRT_CELL_SET("%d", max_uc_queue);
    }
    PRT_COMMITX;

    /*
     * Print FMQ Info.
     */
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        PRT_TITLE_SET("Multicast Queues Info");
        PRT_COLUMN_ADD("FMQ Scheduler Mode");
        PRT_COLUMN_ADD("Min FMQ");
        PRT_COLUMN_ADD("Max FMQ");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        fmq_mode = (is_fmq_sched_mode == TRUE) ? ("Enhanced") : ("Simple");
        PRT_CELL_SET("%s", fmq_mode);
        PRT_CELL_SET("0");
        PRT_CELL_SET("%d", max_mc_queue);
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print queue mapping information
 */
static shr_error_e
sh_dnx_tm_queue_mapping_info_cmd(
    int unit,
    int queue,
    bcm_core_t core,
    sh_sand_control_t * sand_control)
{
    uint32 rate_class, traffic_class, connection_class;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_rate_class_get(unit, core, queue, &rate_class));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_traffic_class_get(unit, core, queue, &traffic_class));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_connection_class_get(unit, core, queue, &connection_class));

    PRT_TITLE_SET("Queue %d Mapping info for Core=%d", queue, core);
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("ID");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Rate Class");
    PRT_CELL_SET("%d", rate_class);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Connection Class");
    PRT_CELL_SET("%d", traffic_class);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Traffic Class");
    PRT_CELL_SET("%d", connection_class);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 */
const char *
diag_dnx_queue_voq_credit_request_state_string_get(
    dbal_enum_value_field_queue_credit_request_state_e key)
{
    char *credit_request_state_string[] = {
        "OFF",
        "SLOW",
        "NORMAL"
    };

    return credit_request_state_string[key];
}

/**
 * \brief - Print queue state information
 */
static shr_error_e
sh_dnx_tm_queue_state_info_cmd(
    int unit,
    int queue,
    bcm_core_t user_core,
    sh_sand_control_t * sand_control)
{
    bcm_gport_t voq_gport;
    bcm_core_t current_core;
    dnx_iqs_voq_state_info_t voq_state_info;
    int base_queue, cos, num_cos;
    int max_mc_queue = -1;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&voq_state_info, 0, sizeof(voq_state_info));

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_IF_ERR_EXIT(bcm_fabric_control_get(unit, bcmFabricMulticastQueueMax, &max_mc_queue));
    }

    /**
     * Start printing the table
     */
    PRT_TITLE_SET("Queue %d credits info", queue);
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("Credit Request State");
    PRT_COLUMN_ADD("Credit Balance [Bytes]");

    DNXCMN_CORES_ITER(unit, user_core, current_core)
    {
        /** Get Base queue on current core */
        SHR_IF_ERR_EXIT(sh_dnx_tm_queue_base_queue_get(unit, queue, current_core, &base_queue, &num_cos));
        if (base_queue < 0)
        {
            /** Queue is invalid for current core, continue to next one */
            continue;
        }

        /** Print the current core */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%d", current_core);

        /** Get the queue cos */
        cos = queue - base_queue;

        /** Set the appropriate VOQ */
        if (queue >= 0 && queue <= max_mc_queue)
        {
            /** Multicast */
            BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, current_core, base_queue);
        }
        else if (queue >= 0 && queue < dnx_data_ipq.queues.nof_queues_get(unit))
        {
            /** Unicast */
            BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, current_core, base_queue);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid queue %d provided", queue);
        }

        SHR_IF_ERR_EXIT(dnx_iqs_voq_state_info_get(unit, voq_gport, cos, &voq_state_info));
        /**
         * Print credit request state - OFF, SLOW or NORMAL
         */
        PRT_CELL_SET("%s", diag_dnx_queue_voq_credit_request_state_string_get(voq_state_info.credit_request_state));

        /**
         * Print Credit balance in Bytes
         */
        PRT_CELL_SET("%d", voq_state_info.credit_balance);
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print queue size information
 */
static shr_error_e
sh_dnx_tm_queue_size_info_cmd(
    int unit,
    int queue,
    bcm_core_t user_core,
    sh_sand_control_t * sand_control)
{
    dnx_ingress_congestion_voq_occupancy_info_t queue_info;
    int current_core;
    int base_queue, num_cos;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Queue %d size", queue);
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("Queue Size [Bytes]");
    PRT_COLUMN_ADD("OCB Queue Size [Bytes]");
    PRT_COLUMN_ADD("Average size [Bytes]");
    PRT_COLUMN_ADD("OCB Buffers");
    PRT_COLUMN_ADD("OCB NOF Packet Descriptors");

    DNXCMN_CORES_ITER(unit, user_core, current_core)
    {
        /** Get Base queue on current core to check if queue is valid */
        SHR_IF_ERR_EXIT(sh_dnx_tm_queue_base_queue_get(unit, queue, current_core, &base_queue, &num_cos));
        if (base_queue < 0)
        {
            /** Queue is invalid for current core, continue to next one */
            continue;
        }
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%d", current_core);

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_occupancy_info_get(unit, current_core, queue, &queue_info));
        PRT_CELL_SET("%u", queue_info.size);
        PRT_CELL_SET("%u", queue_info.size_sram);
        PRT_CELL_SET("%u", queue_info.size_avrg);
        PRT_CELL_SET("%u", queue_info.buffer_size);
        PRT_CELL_SET("%u", queue_info.nof_pds_in_sram);
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print ingress queue scheduler information
 */
static shr_error_e
sh_dnx_tm_iqs_info_cmd(
    int unit,
    bcm_gport_t queue_gport,
    int cos,
    sh_sand_control_t * sand_control)
{
    int credit_request_profile_id, queue;
    bcm_cosq_delay_tolerance_t credit_request_profile;
    bcm_core_t core = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** get queue id and core */
    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(queue_gport))
    {
        queue = BCM_GPORT_MCAST_QUEUE_GROUP_QUEUE_GET(queue_gport);
        core = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(queue_gport);
    }
    else
    {
        queue = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(queue_gport);
        core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(queue_gport);
    }

    /** get queue profile id */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, queue_gport, cos, &credit_request_profile_id, NULL));

    /** get profile info */
    SHR_IF_ERR_EXIT(bcm_cosq_delay_tolerance_level_get(unit, credit_request_profile_id, &credit_request_profile));

    /** print profile */
    PRT_TITLE_SET("Queue %d on core %d --> credit request profile %d", queue + cos, core, credit_request_profile_id);
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Value");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("HIGH PRIORITY");
    PRT_CELL_SET("%u", credit_request_profile.flags & BCM_COSQ_DELAY_TOLERANCE_HIGH_Q_PRIORITY ? 1 : 0);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("HIGH BANDWIDTH");
    PRT_CELL_SET("%u", credit_request_profile.flags & BCM_COSQ_DELAY_TOLERANCE_BANDWIDTH_PROFILE_HIGH ? 1 : 0);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("LOW LATENCY");
    PRT_CELL_SET("%u", credit_request_profile.flags & BCM_COSQ_DELAY_TOLERANCE_IS_LOW_LATENCY ? 1 : 0);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("OCB ONLY");
    PRT_CELL_SET("%u", credit_request_profile.flags & BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY ? 1 : 0);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("BACKOFF (bytes)");
    PRT_CELL_SET("%d-%d", credit_request_profile.credit_request_satisfied_backoff_enter_thresh,
                 credit_request_profile.credit_request_satisfied_backoff_exit_thresh);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("BACKLOG (bytes)");
    PRT_CELL_SET("%d-%d", credit_request_profile.credit_request_satisfied_backlog_enter_thresh,
                 credit_request_profile.credit_request_satisfied_backlog_exit_thresh);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("BACKSLOW (bytes)");
    PRT_CELL_SET("%d-%d", credit_request_profile.credit_request_satisfied_backslow_enter_thresh,
                 credit_request_profile.credit_request_satisfied_backslow_exit_thresh);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("HUNGRY NORMAL->SLOW (bytes)");
    PRT_CELL_SET("%d", credit_request_profile.credit_request_hungry_normal_to_slow_thresh);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("HUNGRY SLOW->NORMAL (bytes)");
    PRT_CELL_SET("%d", credit_request_profile.credit_request_hungry_slow_to_normal_thresh);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("HUNGRY OFF->NORMAL (bytes)");
    PRT_CELL_SET("%d", credit_request_profile.credit_request_hungry_off_to_normal_thresh);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("HUNGRY OFF->SLOW (bytes)");
    PRT_CELL_SET("%d", credit_request_profile.credit_request_hungry_off_to_slow_thresh);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("HUNGRY MULTIPLIER (bytes)");
    PRT_CELL_SET("%d", credit_request_profile.credit_request_hungry_multiplier);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("EMPTY (TH, EXCEED, MAX)  (bytes)");
    PRT_CELL_SET("%d, %d, %d", credit_request_profile.credit_request_satisfied_empty_queue_thresh,
                 credit_request_profile.credit_request_satisfied_empty_queue_exceed_thresh,
                 credit_request_profile.credit_request_satisfied_empty_queue_max_balance_thresh);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("SLOW_LEVELS_UP (bytes)");
    PRT_CELL_SET("%d->%d->%d->%d->%d->%d->%d", credit_request_profile.slow_level_thresh_up[0],
                 credit_request_profile.slow_level_thresh_up[1], credit_request_profile.slow_level_thresh_up[2],
                 credit_request_profile.slow_level_thresh_up[3], credit_request_profile.slow_level_thresh_up[4],
                 credit_request_profile.slow_level_thresh_up[5], credit_request_profile.slow_level_thresh_up[6]);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("SLOW_LEVELS_DOWN (bytes)");
    PRT_CELL_SET("%d<-%d<-%d<-%d<-%d<-%d<-%d", credit_request_profile.slow_level_thresh_down[0],
                 credit_request_profile.slow_level_thresh_down[1], credit_request_profile.slow_level_thresh_down[2],
                 credit_request_profile.slow_level_thresh_down[3], credit_request_profile.slow_level_thresh_down[4],
                 credit_request_profile.slow_level_thresh_down[5], credit_request_profile.slow_level_thresh_down[6]);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("BW LEVEL");
    PRT_CELL_SET("%d", credit_request_profile.bandwidth_level);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("WATCHDOG DELETE TH (milisec)");
    PRT_CELL_SET("%d", credit_request_profile.credit_request_watchdog_delete_queue_thresh);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_tm_queue_compensation_cmd(
    int unit,
    uint32 base_voq,
    int cos,
    bcm_core_t core,
    sh_sand_control_t * sand_control)
{
    int delta;
    uint32 comp_profile;
    bcm_gport_t voq_gport;
    bcm_cosq_pkt_size_adjust_info_t adjust_info;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_voq_compesation_profile_hw_get
                    (unit, core, base_voq + cos, &comp_profile));

    /** print profile */
    PRT_TITLE_SET("Queue %d on core %d --> compensation profile %d", base_voq + cos, core, comp_profile);
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Value");

    /** get compensation from API */
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, core, base_voq);
    adjust_info.gport = voq_gport;
    adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceGlobal;  /** compensation type*/
    adjust_info.flags = 0;
    adjust_info.cosq = cos;
    adjust_info.source_info.source_id = 0;  /** irrelevant */

    SHR_IF_ERR_EXIT(bcm_cosq_gport_pkt_size_adjust_get(unit, &adjust_info, &delta));

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("DELTA");
    PRT_CELL_SET("%d", delta);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to retrieve the base queue of a queue id provided
 */
shr_error_e
sh_dnx_tm_queue_base_queue_get(
    int unit,
    int queue,
    bcm_core_t core,
    int *base_queue,
    int *num_cos)
{
    int base_queue_temp, index;
    uint8 sw_state_num_cos = 0;

    /**
     * So far only 2 Queue Bundle sizes are supported - 4 and 8 queues in a bundle
     */
    int bundle_sizes_arr[DIAG_DNX_QUEUE_NOF_VALID_BUNDLE_SIZES] =
        { dnx_data_ipq.queues.min_bundle_size_get(unit), dnx_data_ipq.queues.min_bundle_size_get(unit) * 2 };
    SHR_FUNC_INIT_VARS(unit);

    *base_queue = -1;
    *num_cos = -1;
    /**
     * So far only 2 Queue Bundle sizes are supported
     */
    for (index = 0; index < DIAG_DNX_QUEUE_NOF_VALID_BUNDLE_SIZES; index++)
    {
        /**
         * Check with the NOF queues in bundle options
         * EXAMPLE: if queue provided is 45:
         * First iteration will be with base_queue_temp = 44;
         * Second iteration will be with base_queue_temp = 40;
         */
        base_queue_temp = (queue / bundle_sizes_arr[index]) * bundle_sizes_arr[index];
        /**
         * Check if in the queue quartate base there is something writen in the SWState.
         * If so and the NUM_COS == to NUM_QUEUES in bundle that's mean that this is the queue base
         */

        SHR_IF_ERR_EXIT(dnx_ipq_db.base_queues.
                        num_cos.get(unit, core, base_queue_temp / dnx_data_ipq.queues.min_bundle_size_get(unit),
                                    &sw_state_num_cos));

        if (sw_state_num_cos != 0)
        {
            *base_queue = base_queue_temp;
            *num_cos = sw_state_num_cos;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print queue specific information
 */
static shr_error_e
sh_dnx_tm_queue_specific_info_cmd(
    int unit,
    int queue,
    sh_sand_control_t * sand_control)
{
    bcm_gport_t voq_gport;
    int rate_class;
    int max_mc_queue = -1, base_queue = 0, num_cos = 0, cos;
    bcm_switch_profile_mapping_t profile_mapping;
    bcm_core_t user_core = 0, current_core = 0;
    uint8 is_asymm = FALSE;
    uint8 valid_queue_exists = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", user_core);

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_IF_ERR_EXIT(bcm_fabric_control_get(unit, bcmFabricMulticastQueueMax, &max_mc_queue));
    }

    DNXCMN_CORES_ITER(unit, user_core, current_core)
    {
        /** Get Base queue on current core */
        SHR_IF_ERR_EXIT(sh_dnx_tm_queue_base_queue_get(unit, queue, current_core, &base_queue, &num_cos));
        if (base_queue < 0)
        {
            /** Queue is invalid for current core, continue to next one */
            continue;
        }
        else
        {
            /** Set indication that the queue is valid on at least one of the cores */
            valid_queue_exists = TRUE;
        }

        /*
         * Check if VOQ is symmetrical on all cores.
         * If the queue is symmetrical, current core is 1 and it was not
         * specifically requested by user, we do not want to print anything for it.
         * For symmetrical queues it is enough to print only data for core 0, as it will be the same on core 1.
         */
        SHR_IF_ERR_EXIT(dnx_ipq_db.base_queue_is_asymm.bit_get(unit, base_queue, &is_asymm));
        if ((is_asymm == FALSE) && (current_core != 0) && (user_core == BCM_CORE_ALL))
        {
            continue;
        }

        /**
         * Get the queue cos
         */
        cos = queue - base_queue;
        if (queue >= 0 && queue <= max_mc_queue)
        {
            /** Multicast */
            BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, current_core, base_queue);
        }
        else if (queue >= 0 && queue < dnx_data_ipq.queues.nof_queues_get(unit))
        {
            /** Unicast */
            BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, current_core, base_queue);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid queue %d provided", queue);
        }

        /** Print general information about the queue */
        SHR_IF_ERR_EXIT(sh_dnx_tm_queue_mapping_info_cmd(unit, queue, current_core, sand_control));

        bcm_switch_profile_mapping_t_init(&profile_mapping);
        profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
        SHR_IF_ERR_EXIT(bcm_cosq_profile_mapping_get(unit, voq_gport, cos, 0, &profile_mapping));
        rate_class = BCM_GPORT_PROFILE_GET(profile_mapping.mapped_profile);

        /** Print queue rate class parameters */
        SHR_IF_ERR_EXIT(sh_dnx_tm_voq_rate_class_info_cmd(unit, rate_class, sand_control));

        /** Print compensation information */
        SHR_IF_ERR_EXIT(sh_dnx_tm_queue_compensation_cmd(unit, base_queue, cos, current_core, sand_control));

        /** Print ingress queue scheduler information */
        SHR_IF_ERR_EXIT(sh_dnx_tm_iqs_info_cmd(unit, voq_gport, cos, sand_control));
    }

    /** Print state and size only if queue is valid on at least one of the cores */
    if (valid_queue_exists == TRUE)
    {
        /** Print queue state information */
        SHR_IF_ERR_EXIT(sh_dnx_tm_queue_state_info_cmd(unit, queue, user_core, sand_control));

        /** Print queue size information */
        SHR_IF_ERR_EXIT(sh_dnx_tm_queue_size_info_cmd(unit, queue, user_core, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print info about the queue(s).
 */
static shr_error_e
sh_dnx_tm_queue_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int queue;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("id", queue);

    if (-1 == queue)
    {
        SHR_IF_ERR_EXIT(sh_dnx_tm_queue_global_info_cmd(unit, sand_control));
    }
    else
    {
        SHR_IF_ERR_EXIT(sh_dnx_tm_queue_specific_info_cmd(unit, queue, sand_control));

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Auto Credit
 * {
 */
sh_sand_enum_t tm_queue_id_table[] = {
    { /** No need to define value if plugin_str was defined */
     .string = "max",
     .desc = "Number of queues supported",
     .value = 0,
     .plugin_str = "DNX_DATA.ipq.queues.nof_queues-1"}
    ,
    {NULL}
};

/**
 * \brief MAN for DNX TM Ingress Queue autocredit diagnostics
 */
static sh_sand_man_t sh_dnx_tm_queue_autocredit_man = {
/* *INDENT-OFF* */
    .brief    = "Ingress TM queues auto credit",
    .full     = "Enable autocredit for a range of ingress queues, if no parameters provided then current status is printed",
    .synopsis = "[rate=<rate in Mbps | 0>] [queue=<queue_id_min-queue_id_max>]",
    .examples = "\n"
                "rate=0\n"
                "rate=100000 queue=2048-3000",
/* *INDENT-ON* */
};

/**
 * \brief Options for DNX TM Ingress Queue autocredit diagnostics
 */
static sh_sand_option_t sh_dnx_tm_queue_autocredit_options[] = {
/* *INDENT-OFF* */
    /*name        type                   desc              default*/
    {"rate",      SAL_FIELD_TYPE_INT32, "rate of autocredit in Mbps. 0 disables autocredit","0"},
    {"queue",     SAL_FIELD_TYPE_INT32, "Queue range",                                      "0-max",    (void *)tm_queue_id_table},
    {NULL}
/* *INDENT-ON* */
};

/**
 * \brief - Print autocredit info
 */
static shr_error_e
dnx_iqs_debug_autocredit_info_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 min_queue, max_queue, rate;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** get info */
    SHR_IF_ERR_EXIT(dnx_iqs_debug_autocredit_rate_get(unit, &min_queue, &max_queue, &rate));

    /** print info */
    PRT_TITLE_SET("Auto-credit status");
    PRT_COLUMN_ADD("Rate");
    PRT_COLUMN_ADD("Min Queue");
    PRT_COLUMN_ADD("Max Queue");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("%u", rate);
    PRT_CELL_SET("%u", min_queue);
    PRT_CELL_SET("%u", max_queue);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print global info about queues.
 */
static shr_error_e
sh_dnx_tm_queue_autocredit_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int queue_min;
    int queue_max;
    int rate = 0;
    int queue_is_present, rate_is_present;

    SHR_FUNC_INIT_VARS(unit);

    /** get parameters */
    SH_SAND_IS_PRESENT("queue", queue_is_present);
    SH_SAND_IS_PRESENT("rate", rate_is_present);
    SH_SAND_GET_INT32_RANGE("queue", queue_min, queue_max);
    SH_SAND_GET_INT32("rate", rate);

    /** if user hasn't provided any parameters then current configuration is printed, otherwise device is configured */
    if ((queue_is_present == FALSE) && (rate_is_present == FALSE))
    {
        /** print info */
        SHR_IF_ERR_EXIT(dnx_iqs_debug_autocredit_info_print(unit, args, sand_control));
    }
    else
    {
        /** set device */
        if (rate)
        {
            SHR_IF_ERR_EXIT(dnx_iqs_debug_autocredit_rate_set(unit, queue_min, queue_max, rate));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_iqs_debug_autocredit_rate_set(unit, 0, 0, rate));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * Credit Watchdog
 * {
 */
/**
 * \brief MAN for DNX TM Ingress Queue credit_watchdog diagnostics
 */
static sh_sand_man_t sh_dnx_tm_queue_credit_watchdog_man = {
/* *INDENT-OFF* */
    .brief    = "Ingress TM queues credit watchdog",
    .full     = "Enable credit watchdog for a range of ingress queues, if no parameters provided then current status is printed",
    .synopsis = "[enable=<1 | 0>] [queue=<queue_id_min-queue_id_max>]",
    .examples = "enable=0\n"
                "enable=1 queue=2048-3000\n"
                "enable=1\n"
/* *INDENT-ON* */
};

/**
 * \brief Options for DNX TM Ingress Queue credit watchdog diagnostics
 */
static sh_sand_option_t sh_dnx_tm_queue_credit_watchdog_options[] = {
/* *INDENT-OFF* */
    /*name        type                   desc                                       default     Extension*/
    {"enable",    SAL_FIELD_TYPE_BOOL,  "Option to enable/disable credit watchdog", "0"},
    {"queue",     SAL_FIELD_TYPE_INT32, "Queue range",                              "0-max",    (void *)tm_queue_id_table},
    {NULL}
/* *INDENT-ON* */
};

/**
 * \brief - Print credit watchdog info
 */
static shr_error_e
sh_dnx_tm_queue_credit_watchdog_info_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_cosq_range_t range;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** get info */
    SHR_IF_ERR_EXIT(bcm_cosq_control_range_get(unit, 0, 0, bcmCosqWatchdogQueue, &range));

    /** print info */
    PRT_TITLE_SET("Credit watchdog status");
    PRT_COLUMN_ADD("Enable");
    PRT_COLUMN_ADD("Min Queue");
    PRT_COLUMN_ADD("Max Queue");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("%s", range.is_enabled ? "TRUE" : "FALSE");
    PRT_CELL_SET("%u", range.range_start);
    PRT_CELL_SET("%u", range.range_end);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print global info about queues.
 */
static shr_error_e
sh_dnx_tm_queue_credit_watchdog_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int queue_min;
    int queue_max;
    int enable = 0;
    int queue_is_present, enable_is_present;
    bcm_cosq_range_t range;

    SHR_FUNC_INIT_VARS(unit);

    /** get parameters */
    SH_SAND_IS_PRESENT("queue", queue_is_present);
    SH_SAND_IS_PRESENT("enable", enable_is_present);
    SH_SAND_GET_INT32_RANGE("queue", queue_min, queue_max);
    SH_SAND_GET_BOOL("enable", enable);

    /** if user hasn't provided any parameters then current configuration is printed, otherwise device is configured */
    if ((queue_is_present == FALSE) && (enable_is_present == FALSE))
    {
        /** print info */
        SHR_IF_ERR_EXIT(sh_dnx_tm_queue_credit_watchdog_info_print(unit, args, sand_control));
    }
    else
    {
        /** set device */
        /** set device */
        range.is_enabled = enable;
        range.range_start = queue_min;
        range.range_end = queue_max;
        SHR_IF_ERR_EXIT(bcm_cosq_control_range_set(unit, 0, 0, bcmCosqWatchdogQueue, &range));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * Last queue diagnostic
 * {
 */

/**
 * \brief MAN for DNX TM Ingress queue last diagnostic
 */
static sh_sand_man_t sh_dnx_tm_queue_last_man = {
/* *INDENT-OFF* */
    .brief    = "Diagnostic displaying info about last enqueued packed",
    .full     = "",
    .synopsis = "",
    .examples = ""
/* *INDENT-ON* */
};

/**
 * \brief Options for DNX TM Ingress queue last diagnostic
 */
static sh_sand_option_t sh_dnx_tm_queue_last_options[] = {
/* *INDENT-OFF* */
    /*name        type                   desc              default*/
    {NULL}
/* *INDENT-ON* */
};

/**
 * \brief - Print last enqueue packet information.
 */
static shr_error_e
sh_dnx_tm_queue_last_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int core;
    dnx_ingress_congestion_last_enqueued_cmd_info_t last_cmd_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /** get last enqueued packet info */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_last_enqueued_cmd_info_get(unit, core, &last_cmd_info));

        /** print profile */
        PRT_TITLE_SET("Last enqueued packet info for Core %d", core);
        PRT_COLUMN_ADD("Queue id");
        PRT_COLUMN_ADD("Packet size");
        PRT_COLUMN_ADD("Compensation");

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%u", last_cmd_info.qnum);
        PRT_CELL_SET("%u", last_cmd_info.pkt_size);
        PRT_CELL_SET("%d", last_cmd_info.compensation);

        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/**
 * \brief DNX TM Ingress Queue diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for TM Ingress Queue diagnostic commands
 */
sh_sand_cmd_t sh_dnx_tm_queue_cmds[] = {
/* *INDENT-OFF* */
    /*keyword,      action,                         command,options,                                man                     callback    legacy*/
    {"mapping",     sh_dnx_tm_queue_mapping_cmd,        NULL,   sh_dnx_tm_queue_mapping_options,        &sh_dnx_tm_queue_mapping_man},
    {"info",        sh_dnx_tm_queue_info_cmd,           NULL,   sh_dnx_tm_queue_info_options,           &sh_dnx_tm_queue_info_man},
    {"autocredit",  sh_dnx_tm_queue_autocredit_cmd,     NULL,   sh_dnx_tm_queue_autocredit_options,     &sh_dnx_tm_queue_autocredit_man},
    {"watchdog",    sh_dnx_tm_queue_credit_watchdog_cmd,NULL,   sh_dnx_tm_queue_credit_watchdog_options,&sh_dnx_tm_queue_credit_watchdog_man},
    {"non_empty",   sh_dnx_tm_queue_non_empty_cmd,      NULL,   sh_dnx_tm_queue_non_empty_options,      &sh_dnx_tm_queue_non_empty_man},
    {"count",       sh_dnx_tm_queue_count_cmd,          NULL,   sh_dnx_tm_queue_count_options,          &sh_dnx_tm_queue_count_man},
    {"last",        sh_dnx_tm_queue_last_cmd,           NULL,   sh_dnx_tm_queue_last_options,           &sh_dnx_tm_queue_last_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {NULL}
/* *INDENT-ON* */
};

/**
 * \brief MAN for DNX TM Ingress Queue diagnostics
 */
sh_sand_man_t sh_dnx_tm_queue_man = {
    .brief = "Queue commands",
    .full = NULL
};
