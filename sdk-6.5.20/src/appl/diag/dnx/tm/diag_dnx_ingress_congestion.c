/** \file diag_dnx_ingress_congestion.c
 * 
 * diagnostics for ingress congestion
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

/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/cosq/latency/cosq_latency.h>
/** soc */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_system_red.h>
#include <soc/dnx/dbal/dbal.h>

/** sal */
#include <sal/appl/sal.h>

/** local */
#include "diag_dnx_ingress_congestion.h"
#include "diag_dnx_ingress_rate.h"
/**  Common explanation for the usage of the VSQ group diag
 *   input */
#define DIAG_DNX_VSQ_GROUPS_LEGEND          "Mandatory input is the VSQ group:\n" \
                                            "  a - category - CT\n" \
                                            "  b - category and traffic class - CTTC\n" \
                                            "  c - category and connection class - CTCC\n" \
                                            "  d - statistical flows - PP \n" \
                                            "  e - source port\n" \
                                            "  f - source port priority_group - PG\n"
/**  Common explanation for the usage of the filters for the
 *   counter diags */
#define DIAG_DNX_VOQ_VSQ_NON_EMPTY_FILTERS_LEGEND   "Filters and parameters:\n" \
                                                    "    MOST - If a number is provided to the option 'most'" \
                                                    "the diagnostic will display the most occupied queues" \
                                                    "specified by this parameter.\n" \
                                                    "    POLL_NOF_times and POLL_Delay - The diagnostic can be called "\
                                                    "several times in depending on these parameters.\n" \
                                                    "    POLL_Delay is specified in MicroSeconds.\n" \
                                                    "    Polls can be used to check how the queues" \
                                                    "occupancy changes over time"
/** Macro for printing the title in the VSQ info diagnostic
 *  in a common format */
#define DIAG_DNX_VSQ_INFO_TITLE_PRINT(title)   \
                                    do { \
                                    PRT_TITLE_SET("VSQ %d (Group %s) %s Info",  \
                                                    vsq_group_idx, \
                                                    sh_sand_enum_str(dnx_vsq_group_enum_table, \
                                                    vsq_group), title); \
                                    } while(0);
/** Print row in the VOQ non empty diag */
#define DIAG_DNX_QUEUE_NON_EMPTY_ROW_PRINT(voq_info)   \
                 do { \
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE); \
                    PRT_CELL_SET("%u", voq_info.id); \
                    PRT_CELL_SET("%u", voq_info.size); \
                    PRT_CELL_SET("%u", voq_info.size_sram); \
                    PRT_CELL_SET("%u", voq_info.buffer_size); \
                    PRT_CELL_SET("%u", voq_info.nof_pds_in_sram); \
                 } while(0);

/* 
 * --------------------------------------------------------------------------
 * dnx tm ingress congestion
 * --------------------------------------------------------------------------
 */

sh_sand_option_t sh_dnx_ingress_congestion_options[] = {
/* *INDENT-OFF* */
    /*name        type                  desc                                        default   ext*/
    {NULL}
/* *INDENT-ON* */
};

sh_sand_man_t sh_dnx_ingress_congestion_man = {
    .brief = "Print ingress congestion parameters",
    .full = NULL,
    .synopsis = NULL,
    .examples = NULL
};

/**
 * \brief - dump reassembly and port termination context table
 */
shr_error_e
sh_dnx_ingress_congestion_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core, index;
    dnx_ingress_congestion_statistics_info_t stats[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];

    /** the order of names should match the order in dnx_ingress_congestion_statistics_types_t enum */
    char *counter_name[] = {
        "Free DRAM BDBs",
        "Free SRAM Buffers",
        "Free SRAM PDs"
    };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_free_resource_counter_get(unit, core, &stats[core]));
    }

    /*
     * Print table header
     */
    PRT_TITLE_SET("Ingress Congestion");
    PRT_COLUMN_ADD("Resource");
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        PRT_COLUMN_ADD("Core %d", core);
    }

    for (index = 0; index < DNX_INGRESS_CONGESTION_STATISTICS_NOF; index++)
    {
        /** Instantaneous counter */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", counter_name[index]);
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
        {
            PRT_CELL_SET("%u", stats[core].free_count[index]);
        }
        /** Minimum  counter */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("Minimum %s", counter_name[index]);
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
        {
            PRT_CELL_SET("%u", stats[core].min_free_count[index]);
        }

    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* 
 * --------------------------------------------------------------------------
 * dnx tm ingress rate_class
 * --------------------------------------------------------------------------
 */

typedef enum
{
    SH_DNX_TM_INGRESS_CONGESTION_RATE_CLASS_TYPE_INVALID = -1,
    SH_DNX_TM_INGRESS_CONGESTION_RATE_CLASS_TYPE_VOQ = 0,
    SH_DNX_TM_INGRESS_CONGESTION_RATE_CLASS_TYPE_VSQ
} sh_dnx_ingress_congestion_rate_class_type_t;

typedef struct
{
    int ocb_only;
    bcm_cosq_gport_size_t size_params[DNX_INGRESS_CONGESTION_RESOURCE_NOF][DNX_COSQ_NOF_DP];
    bcm_cosq_gport_discard_t wred[DNX_COSQ_NOF_DP];
    bcm_cosq_gport_discard_t ecn_config;
    bcm_cosq_fadt_threshold_t dram_bound_ocb_bytes_threshold[DNX_INGRESS_CONGESTION_DRAM_BOUND_THR_TYPE_NOF];
    bcm_cosq_fadt_threshold_t dram_bound_ocb_buffers_threshold[DNX_INGRESS_CONGESTION_DRAM_BOUND_THR_TYPE_NOF];
    bcm_cosq_fadt_threshold_t dram_bound_ocb_pds_threshold[DNX_INGRESS_CONGESTION_DRAM_BOUND_THR_TYPE_NOF];
    uint32 dram_bound_recovery_threshold;
    bcm_cosq_fadt_threshold_t congestion_notification_threshold[DNX_INGRESS_CONGESTION_RESOURCE_NOF];
    bcm_cosq_gport_size_t sys_red_voq_range[DNX_DATA_MAX_SYSTEM_RED_INFO_NOF_RED_Q_SIZE];
    bcm_cosq_gport_discard_t sys_red_low_prob_disc_prms[DNX_COSQ_NOF_DP];
    bcm_cosq_gport_discard_t sys_red_high_prob_disc_prms[DNX_COSQ_NOF_DP];
    uint32 latency_voq_reject_probability[DNX_COSQ_NOF_DP][DNX_DATA_MAX_INGR_CONGESTION_VOQ_LATENCY_BINS];
    uint32 latency_voq_enable[DNX_COSQ_NOF_DP];
    uint32 latency_voq_min_threshold[DNX_COSQ_NOF_DP];
} sh_dnx_ingress_congestion_voq_rate_class_info_t;

static sh_sand_enum_t diag_dnx_ingress_congestion_rate_class_enum_table[] = {
    {"VOQ", SH_DNX_TM_INGRESS_CONGESTION_RATE_CLASS_TYPE_VOQ, "Rate type class"},
    {NULL}
};

sh_sand_option_t sh_dnx_ingress_congestion_rate_class_options[] = {
/* *INDENT-OFF* */
    /*name        type                  desc               default   ext*/
    {"type",      SAL_FIELD_TYPE_ENUM,  "Rate class type", NULL,       (void*)diag_dnx_ingress_congestion_rate_class_enum_table},
    {"id",        SAL_FIELD_TYPE_INT32, "Rate class id",   NULL,     NULL},
    {NULL}
/* *INDENT-ON* */
};

sh_sand_man_t sh_dnx_ingress_congestion_rate_class_man = {
    .brief = "Print ingress rate class parameters",
    .full = "Print parameters for ingress rate class of specified type",
    .synopsis = "type=<VOQ> id=<rate class id>",
    .examples = "type=VOQ id=1"
};

const char *
sh_dnx_tm_ingress_congestion_resource_str(
    dnx_ingress_congestion_resource_type_e resource)
{
    char *resource_name[] = {
        "Bytes",
        "OCB Buffers",
        "OCB Packet Descriptors"
    };

    return resource_name[resource];
}

/**
 * \brief - Print voq rate class information
 */
static shr_error_e
sh_dnx_tm_voq_rate_class_info_print(
    int unit,
    int rate_class,
    sh_dnx_ingress_congestion_voq_rate_class_info_t * info,
    sh_sand_control_t * sand_control)
{
    dnx_ingress_congestion_resource_type_e resource;
    dnx_ingress_congestion_dram_bound_threshold_type_e dram_bound_type;
    int dp;

    int is_sys_red_enabled, sys_red_idx, latency_bin = 0;
    uint32 red_max_q_size = dnx_data_system_red.info.max_red_q_size_get(unit);
    int shared_resource = dnx_data_system_red.info.feature_get(unit, dnx_data_system_red_info_is_share_res_with_latency_based_admission);

    char *dram_bound_threshold_name[] = {
        "DRAM Bound",
        "DRAM Bound Recovery Failure"
    };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Print rate class general information */
    PRT_TITLE_SET("Rate class Info");
    PRT_COLUMN_ADD("Param");
    PRT_COLUMN_ADD("value");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("rate class id");
    PRT_CELL_SET("%d", rate_class);
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("OCB_ONLY");
    PRT_CELL_SET("%s", sh_sand_bool_str(info->ocb_only));

    PRT_COMMITX;

    /*
     * Guaranteed
     */
    PRT_TITLE_SET("Rate class %d Guaranteed Info", rate_class);
    PRT_COLUMN_ADD("Resource");
    PRT_COLUMN_ADD("Guaranteed");
    for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF;
         resource++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", sh_dnx_tm_ingress_congestion_resource_str(resource));
        PRT_CELL_SET("%u", info->size_params[resource][0].size_min); /** guaranteed is not per dp */
    }
    PRT_COMMITX;

    /*
     * Tail Drop
     */
    PRT_TITLE_SET("Rate class %d FADT Drop Info", rate_class);
    PRT_COLUMN_ADD("Resource");
    PRT_COLUMN_ADD("DP");
    PRT_COLUMN_ADD("FADT Min threshold");
    PRT_COLUMN_ADD("FADT Max threshold");
    PRT_COLUMN_ADD("Alpha");

    for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF;
         resource++)
    {
        for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
        {
            PRT_ROW_ADD(dp == (DNX_COSQ_NOF_DP - 1) ? PRT_ROW_SEP_UNDERSCORE : PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", sh_dnx_tm_ingress_congestion_resource_str(resource));
            PRT_CELL_SET("%d", dp);
            PRT_CELL_SET("%u", info->size_params[resource][dp].size_fadt_min);
            PRT_CELL_SET("%u", info->size_params[resource][dp].size_max);
            PRT_CELL_SET("%d", info->size_params[resource][dp].size_alpha_max);
        }
    }
    PRT_COMMITX;

    /*
     * WRED
     */
    PRT_TITLE_SET("Rate class %d WRED Info", rate_class);
    PRT_COLUMN_ADD("Resource");
    PRT_COLUMN_ADD("DP");
    PRT_COLUMN_ADD("Enable");
    PRT_COLUMN_ADD("Min Threshold");
    PRT_COLUMN_ADD("Max Threshold");
    PRT_COLUMN_ADD("Drop Probability");

    for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Bytes");
        PRT_CELL_SET("%d", dp);
        PRT_CELL_SET("%s", (sh_sand_bool_str(info->wred[dp].flags & BCM_COSQ_DISCARD_ENABLE)));
        PRT_CELL_SET("%u", info->wred[dp].min_thresh);
        PRT_CELL_SET("%u", info->wred[dp].max_thresh);
        PRT_CELL_SET("%d", info->wred[dp].drop_probability);
    }
    PRT_COMMITX;

    /*
     * ECN
     */
    PRT_TITLE_SET("Rate class %d ECN Info", rate_class);
    PRT_COLUMN_ADD("Resource");
    PRT_COLUMN_ADD("Max Size");
    PRT_COLUMN_ADD("Enable");
    PRT_COLUMN_ADD("Min Threshold");
    PRT_COLUMN_ADD("Max Threshold");
    PRT_COLUMN_ADD("Drop Probability");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Bytes");
    PRT_CELL_SET("%u", info->ecn_config.ecn_thresh);
    PRT_CELL_SET("%s", (sh_sand_bool_str(info->ecn_config.flags & BCM_COSQ_DISCARD_ENABLE)));
    PRT_CELL_SET("%u", info->ecn_config.min_thresh);
    PRT_CELL_SET("%u", info->ecn_config.max_thresh);
    PRT_CELL_SET("%d", info->ecn_config.drop_probability);
    PRT_COMMITX;

    /*
     * DRAM Bounds
     */
    PRT_TITLE_SET("Rate class %d DRAM Bounds Info", rate_class);
    PRT_COLUMN_ADD("Threshold Type");
    PRT_COLUMN_ADD("Resource");
    PRT_COLUMN_ADD("Min Threshold");
    PRT_COLUMN_ADD("Max Threshold");
    PRT_COLUMN_ADD("Min Free Threshold");
    PRT_COLUMN_ADD("Max Free Threshold");
    PRT_COLUMN_ADD("Alpha");

    for (dram_bound_type = DNX_INGRESS_CONGESTION_DRAM_BOUND;
         dram_bound_type < DNX_INGRESS_CONGESTION_DRAM_BOUND_THR_TYPE_NOF; dram_bound_type++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dram_bound_threshold_name[dram_bound_type]);
        PRT_CELL_SET("OCB Bytes");
        PRT_CELL_SET("%u", info->dram_bound_ocb_bytes_threshold[dram_bound_type].thresh_min);
        PRT_CELL_SET("%u", info->dram_bound_ocb_bytes_threshold[dram_bound_type].thresh_max);
        PRT_CELL_SET("%u", info->dram_bound_ocb_bytes_threshold[dram_bound_type].resource_range_min);
        PRT_CELL_SET("%u", info->dram_bound_ocb_bytes_threshold[dram_bound_type].resource_range_max);
        PRT_CELL_SET("%d", info->dram_bound_ocb_bytes_threshold[dram_bound_type].alpha);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dram_bound_threshold_name[dram_bound_type]);
        PRT_CELL_SET("OCB Buffers");
        PRT_CELL_SET("%u", info->dram_bound_ocb_buffers_threshold[dram_bound_type].thresh_min);
        PRT_CELL_SET("%u", info->dram_bound_ocb_buffers_threshold[dram_bound_type].thresh_max);
        PRT_CELL_SET("%u", info->dram_bound_ocb_buffers_threshold[dram_bound_type].resource_range_min);
        PRT_CELL_SET("%u", info->dram_bound_ocb_buffers_threshold[dram_bound_type].resource_range_max);
        PRT_CELL_SET("%d", info->dram_bound_ocb_buffers_threshold[dram_bound_type].alpha);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", dram_bound_threshold_name[dram_bound_type]);
        PRT_CELL_SET("OCB Packet Descriptors");
        PRT_CELL_SET("%u", info->dram_bound_ocb_pds_threshold[dram_bound_type].thresh_min);
        PRT_CELL_SET("%u", info->dram_bound_ocb_pds_threshold[dram_bound_type].thresh_max);
        PRT_CELL_SET("%u", info->dram_bound_ocb_pds_threshold[dram_bound_type].resource_range_min);
        PRT_CELL_SET("%u", info->dram_bound_ocb_pds_threshold[dram_bound_type].resource_range_max);
        PRT_CELL_SET("%d", info->dram_bound_ocb_pds_threshold[dram_bound_type].alpha);
    }
    PRT_COMMITX;

    PRT_TITLE_SET("Rate class %d DRAM Bound Recovery", rate_class);
    PRT_COLUMN_ADD("Resource");
    PRT_COLUMN_ADD("Recovery Threshold");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Bytes");
    PRT_CELL_SET("%u", info->dram_bound_recovery_threshold);
    PRT_COMMITX;

    /*
     * Congestion Notification
     */
    PRT_TITLE_SET("Rate class %d Congestion Notification Info", rate_class);
    PRT_COLUMN_ADD("Resource");
    PRT_COLUMN_ADD("Min Threshold");
    PRT_COLUMN_ADD("Max Threshold");
    PRT_COLUMN_ADD("Alpha");

    for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF;
         resource++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", sh_dnx_tm_ingress_congestion_resource_str(resource));
        PRT_CELL_SET("%u", info->congestion_notification_threshold[resource].thresh_min);
        PRT_CELL_SET("%u", info->congestion_notification_threshold[resource].thresh_max);
        PRT_CELL_SET("%d", info->congestion_notification_threshold[resource].alpha);
    }
    PRT_COMMITX;

    /*
     * System RED - only print if system RED soc property is enabled
     */
    is_sys_red_enabled = dnx_data_system_red.config.enable_get(unit);
    if (is_sys_red_enabled)
    {
        PRT_TITLE_SET("Rate class %d System RED Info", rate_class);
        PRT_COLUMN_ADD("RED-Q-Size Index");
        PRT_COLUMN_ADD("Max VOQ Size");

        for (sys_red_idx = 0; sys_red_idx <= red_max_q_size; ++sys_red_idx)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%u", sys_red_idx);
            PRT_CELL_SET("%u", info->sys_red_voq_range[sys_red_idx].size_max);
        }
        PRT_COMMITX;

        PRT_TITLE_SET("Rate class %d System RED Discard Params", rate_class);
        PRT_COLUMN_ADD("DP");
        PRT_COLUMN_ADD("Discard Enable");
        PRT_COLUMN_ADD("Admit Thr");
        PRT_COLUMN_ADD("Low Drop Probability");
        PRT_COLUMN_ADD("Prob Thr");
        PRT_COLUMN_ADD("High Drop Probability");
        PRT_COLUMN_ADD("Reject Thr");

        for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", dp);
            /** Enable flag should be equal in both high and low_prob params. */
            PRT_CELL_SET("%s",
                         (sh_sand_bool_str(info->sys_red_low_prob_disc_prms[dp].flags & BCM_COSQ_DISCARD_ENABLE)));
            /** Admit thr is min_thr in low_prob */
            PRT_CELL_SET("%u", info->sys_red_low_prob_disc_prms[dp].min_thresh);
            /** Low drop probability */
            PRT_CELL_SET("0x%x", info->sys_red_low_prob_disc_prms[dp].drop_probability);
            /** Prob thr is max_thr in low_prob */
            PRT_CELL_SET("%u", info->sys_red_low_prob_disc_prms[dp].max_thresh);
            /** High drop probability */
            PRT_CELL_SET("0x%x", info->sys_red_high_prob_disc_prms[dp].drop_probability);
            /** Reject thr is max_thr in high_prob */
            PRT_CELL_SET("%u", info->sys_red_high_prob_disc_prms[dp].max_thresh);
        }
        PRT_COMMITX;
    }
    /*
     * System RED and Latency admission are mutually exclusive - if system red is not enabled, print table for Latency admission test
     */
    if ((!is_sys_red_enabled || !shared_resource)
        && dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_latency_based_admission))
    {
        PRT_TITLE_SET("Rate class %d VOQ Latency Admission Test Info", rate_class);
        PRT_COLUMN_ADD("Color");
        PRT_COLUMN_ADD("Enable");
        PRT_COLUMN_ADD("Min Threshold");
        for (latency_bin = 0; latency_bin < DNX_DATA_MAX_INGR_CONGESTION_VOQ_LATENCY_BINS; latency_bin++)
        {
            PRT_COLUMN_ADD("Drop Probability - %d", latency_bin);
        }

        for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", dp);
            PRT_CELL_SET("%s", (sh_sand_bool_str(info->latency_voq_enable[dp])));
            PRT_CELL_SET("%d", info->latency_voq_min_threshold[dp]);
            for (latency_bin = 0; latency_bin < DNX_DATA_MAX_INGR_CONGESTION_VOQ_LATENCY_BINS; latency_bin++)
            {
                PRT_CELL_SET("%d", info->latency_voq_reject_probability[dp][latency_bin]);
            }
        }
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print voq rate class information
 */
shr_error_e
sh_dnx_tm_voq_rate_class_info_cmd(
    int unit,
    int rate_class,
    sh_sand_control_t * sand_control)
{
    bcm_gport_t rate_class_gport;
    sh_dnx_ingress_congestion_voq_rate_class_info_t *rate_class_info = NULL;

    bcm_cosq_fadt_info_t congestion_notification_info;

    dnx_ingress_congestion_resource_type_e resource;
    dnx_ingress_congestion_dram_bound_threshold_type_e thresh;
    int dp;

    int sys_red_idx, is_sys_red_enabled;
    uint32 sys_red_flag;
    uint32 red_max_q_size = dnx_data_system_red.info.max_red_q_size_get(unit);
    int shared_resource = dnx_data_system_red.info.feature_get(unit, dnx_data_system_red_info_is_share_res_with_latency_based_admission);

    uint32 latency_key = 0;
    int latency_bin = 0;

    int resource_flags[] = {
        BCM_COSQ_GPORT_SIZE_BYTES,
        BCM_COSQ_GPORT_SIZE_BUFFERS | BCM_COSQ_GPORT_SIZE_OCB,
        BCM_COSQ_GPORT_SIZE_PACKET_DESC | BCM_COSQ_GPORT_SIZE_OCB
    };

    int resource_types[] = {
        bcmResourceBytes,
        bcmResourceOcbBuffers,
        bcmResourceOcbPacketDescriptors
    };

    uint32 discard_color_flags[] = {
        BCM_COSQ_DISCARD_COLOR_GREEN,
        BCM_COSQ_DISCARD_COLOR_YELLOW,
        BCM_COSQ_DISCARD_COLOR_RED,
        BCM_COSQ_DISCARD_COLOR_BLACK
    };

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(rate_class_info, sizeof(sh_dnx_ingress_congestion_voq_rate_class_info_t),
              "rate_class_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    BCM_GPORT_PROFILE_SET(rate_class_gport, rate_class);

    SHR_IF_ERR_EXIT(bcm_cosq_control_get(unit, rate_class_gport, 0, bcmCosqControlOCBOnly, &rate_class_info->ocb_only));

    /** Get Guaranteed and Tail Drop thresholds */
    for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF;
         resource++)
    {
        for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
        {
            /*
             * FADT (tail) drop thresholds are obtained by internal functions and not by APIs
             * to get the actual HW value and not the value stored in SW state
             */
            SHR_IF_ERR_EXIT(dnx_voq_rate_class_color_size_dual_get(unit, rate_class_gport, dp,
                                                                   resource_flags[resource], TRUE,
                                                                   &rate_class_info->size_params[resource][dp]));
        }
    }

    /** Get WRED */
    for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
    {
        bcm_cosq_gport_discard_t_init(&rate_class_info->wred[dp]);
        rate_class_info->wred[dp].flags = BCM_COSQ_DISCARD_BYTES | discard_color_flags[dp];

        SHR_IF_ERR_EXIT(bcm_cosq_gport_discard_get(unit, rate_class_gport, 0, &rate_class_info->wred[dp]));
    }

    /** Get ECN */
    bcm_cosq_gport_discard_t_init(&rate_class_info->ecn_config);
    rate_class_info->ecn_config.flags =
        BCM_COSQ_DISCARD_MARK_CONGESTION | BCM_COSQ_DISCARD_BYTES /** queue size in Bytes */ ;
    SHR_IF_ERR_EXIT(bcm_cosq_gport_discard_get(unit, rate_class_gport, 0, &rate_class_info->ecn_config));

    /*
     * DRAM bound threshold are obtained by internal functions and not by APIs
     * to get the actual HW value and not the value stored in SW state
     */

    /** Get Dram Bounds */
    thresh = DNX_INGRESS_CONGESTION_DRAM_BOUND;
    SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_bound_get(unit, DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BYTES,
                                                           rate_class,
                                                           &rate_class_info->dram_bound_ocb_bytes_threshold[thresh]));

    SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_bound_get(unit,
                                                           DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BUFFERS,
                                                           rate_class,
                                                           &rate_class_info->dram_bound_ocb_buffers_threshold[thresh]));

    SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_bound_get(unit, DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_PDS,
                                                           rate_class,
                                                           &rate_class_info->dram_bound_ocb_pds_threshold[thresh]));

    thresh = DNX_INGRESS_CONGESTION_DRAM_BOUND_RECOVERY_FAILURE;
    SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_recovery_fail_get
                    (unit, DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BYTES, rate_class,
                     &rate_class_info->dram_bound_ocb_bytes_threshold[thresh]));

    SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_recovery_fail_get
                    (unit, DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BUFFERS, rate_class,
                     &rate_class_info->dram_bound_ocb_buffers_threshold[thresh]));

    SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_recovery_fail_get
                    (unit, DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_PDS, rate_class,
                     &rate_class_info->dram_bound_ocb_pds_threshold[thresh]));

    SHR_IF_ERR_EXIT(dnx_voq_rate_class_dram_recovery_get(unit, rate_class,
                                                         &rate_class_info->dram_bound_recovery_threshold));

    /*
     * Get Congestion Notification info
     */
    for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF;
         resource++)
    {
        bcm_cosq_fadt_info_t_init(&congestion_notification_info);
        congestion_notification_info.gport = rate_class_gport;
        congestion_notification_info.cosq = 0;
        congestion_notification_info.thresh_type = bcmCosqFadtCongestionNotification;
        congestion_notification_info.resource_type = resource_types[resource];
        SHR_IF_ERR_EXIT(bcm_cosq_gport_fadt_threshold_get(unit, 0, &congestion_notification_info,
                                                          &rate_class_info->congestion_notification_threshold
                                                          [resource]));
    }

    /*
     * Get system RED info - only if systen RED soc property os enabled
     */
    is_sys_red_enabled = dnx_data_system_red.config.enable_get(unit);
    if (is_sys_red_enabled)
    {
        /** Get System Red VOQ range */
        sys_red_flag = BCM_COSQ_GPORT_SIZE_COLOR_SYSTEM_RED;
        for (sys_red_idx = 0; sys_red_idx < red_max_q_size; ++sys_red_idx)
        {
            SHR_IF_ERR_EXIT(bcm_cosq_gport_color_size_get(unit, rate_class_gport, 0, sys_red_idx, sys_red_flag,
                                                          &rate_class_info->sys_red_voq_range[sys_red_idx]));
        }
        /** last RED-Q-Size index MAX is max_res */
        rate_class_info->sys_red_voq_range[red_max_q_size].size_max =
            dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)->max;

        /** Get System RED discard params */
        for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
        {
            rate_class_info->sys_red_low_prob_disc_prms[dp].flags =
                BCM_COSQ_DISCARD_SYSTEM | BCM_COSQ_DISCARD_PROBABILITY1 | discard_color_flags[dp];
            SHR_IF_ERR_EXIT(bcm_cosq_gport_discard_get(unit, rate_class_gport, 0,
                                                       &rate_class_info->sys_red_low_prob_disc_prms[dp]));

            rate_class_info->sys_red_high_prob_disc_prms[dp].flags =
                BCM_COSQ_DISCARD_SYSTEM | BCM_COSQ_DISCARD_PROBABILITY2 | discard_color_flags[dp];
            SHR_IF_ERR_EXIT(bcm_cosq_gport_discard_get(unit, rate_class_gport, 0,
                                                       &rate_class_info->sys_red_high_prob_disc_prms[dp]));
        }
    }
    /*
     * Get Latency VOQ - mutually exclusive with system RED
     */
    if ((!is_sys_red_enabled || !shared_resource)
        && dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_latency_based_admission))
    {
        for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
        {
            BCM_COSQ_GENERIC_KEY_COLOR_SET(latency_key, dp);
            SHR_IF_ERR_EXIT(bcm_cosq_generic_control_get
                            (unit, 0, rate_class_gport, latency_key, bcmCosqGenericControlLatencyVoqRejectEnable,
                             &rate_class_info->latency_voq_enable[dp]));
            SHR_IF_ERR_EXIT(bcm_cosq_generic_control_get
                            (unit, 0, rate_class_gport, latency_key, bcmCosqGenericControlLatencyVoqRejectMinThreshold,
                             &rate_class_info->latency_voq_min_threshold[dp]));
            for (latency_bin = 0; latency_bin < DNX_DATA_MAX_INGR_CONGESTION_VOQ_LATENCY_BINS; latency_bin++)
            {
                BCM_COSQ_GENERIC_KEY_LAT_BIN_COLOR_SET(latency_key, latency_bin, dp);
                SHR_IF_ERR_EXIT(bcm_cosq_generic_control_get
                                (unit, 0, rate_class_gport, latency_key, bcmCosqGenericControlLatencyVoqRejectProb,
                                 &rate_class_info->latency_voq_reject_probability[dp][latency_bin]));
            }
        }
    }

    SHR_IF_ERR_EXIT(sh_dnx_tm_voq_rate_class_info_print(unit, rate_class, rate_class_info, sand_control));

exit:
    SHR_FREE(rate_class_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print rate class info.
 */
shr_error_e
sh_dnx_ingress_congestion_rate_class_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int rate_class;
    sh_dnx_ingress_congestion_rate_class_type_t rate_class_type;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("id", rate_class);
    SH_SAND_GET_ENUM("type", rate_class_type);

    switch (rate_class_type)
    {
        case SH_DNX_TM_INGRESS_CONGESTION_RATE_CLASS_TYPE_VOQ:
            SHR_IF_ERR_EXIT(sh_dnx_tm_voq_rate_class_info_cmd(unit, rate_class, sand_control));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d provided", rate_class_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Queues NON empty 
 * { 
 */

/**
 * \brief MAN for DNX TM Ingress Queue occupancy levels diag
 */
sh_sand_man_t sh_dnx_tm_queue_non_empty_man = {
/* *INDENT-OFF* */
    .brief    = "Ingress TM queues NON Empty diagnostic",
    .full     = "Diagnostic to show the occupancy \n"
                " of non empty queues.\n"
                DIAG_DNX_VOQ_VSQ_NON_EMPTY_FILTERS_LEGEND,
    .synopsis = "[CORE=<core_id>] [MOST=<nof>]"
                "[POLL_NOF_times=<nof_times>] [POLL_Delay=<delay in micro second>]",
    .examples = "core=0\n"
                "most=5 poll_nof_times=2 poll_delay=50",
/* *INDENT-ON* */
};

/**
 * \brief MAN for DNX TM Ingress Queue occupancy levels diag
 */
sh_sand_option_t sh_dnx_tm_queue_non_empty_options[] = {
/* *INDENT-OFF* */
    /*name                  type                   desc                                         default*/
    {"most",                SAL_FIELD_TYPE_INT32,  "NOF of most occupied queues to display",    "0", tm_queue_id_table, "0-max"},
    {"poll_nof_times",      SAL_FIELD_TYPE_INT32,  "Number of polling times",                   "1"},
    {"poll_delay",          SAL_FIELD_TYPE_INT32,  "Delay inbetween pollings",                  "0"},
    {NULL}
/* *INDENT-ON* */
};

/**
 * \brief - Function to return the queue with minimum size from 
 *        the array holding the max N number of most occupied
 *        queues.
 */
static shr_error_e
diag_dnx_tm_most_not_congested_voq_from_array_find(
    int unit,
    dnx_ingress_congestion_voq_occupancy_info_t * voq_non_empty_info_array,
    int nof_voqs_to_display,
    int *current_min_voq_size_idx,
    uint32 *current_min_voq_size)
{
    int voq_idx;

    SHR_FUNC_INIT_VARS(unit);

    /** Find current minimum voq size */
    *current_min_voq_size = voq_non_empty_info_array[0].size;
    for (voq_idx = 1; voq_idx < nof_voqs_to_display; voq_idx++)
    {
        if (voq_non_empty_info_array[voq_idx].size < *current_min_voq_size)
        {
            *current_min_voq_size = voq_non_empty_info_array[voq_idx].size;
            *current_min_voq_size_idx = voq_idx;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Print global info about queues.
 */
shr_error_e
sh_dnx_tm_queue_non_empty_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    shr_error_e rv = 0;
    int core, core_id;
    int poll_nof_times, poll_delay, nof_most_occupied_voqs_to_show;
    int poll_idx = 0, most_provided;
    uint8 error_reading_from_hw = FALSE;
    uint32 entry_handle_id, current_min_voq_size = 0;
    int is_end, voq_idx, current_min_voq_size_idx = 0;
    dnx_ingress_congestion_voq_occupancy_info_t *voq_non_empty_info_array = NULL;
    dnx_ingress_congestion_voq_occupancy_info_t current_voq_occupancy_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get parameters */
    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_INT32("most", nof_most_occupied_voqs_to_show);
    SH_SAND_GET_INT32("poll_nof_times", poll_nof_times);
    SH_SAND_GET_INT32("poll_delay", poll_delay);
    SH_SAND_IS_PRESENT("most", most_provided);

    if (most_provided)
    {
        SHR_ALLOC_SET_ZERO(voq_non_empty_info_array,
                           sizeof(dnx_ingress_congestion_voq_occupancy_info_t) * nof_most_occupied_voqs_to_show,
                           "sh_dnx_tm_queue_non_empty_cmd.dnx_ingress_congestion_voq_occupancy_info_t", "%s%s%s\r\n",
                           EMPTY, EMPTY, EMPTY);
    }

    for (poll_idx = 0; poll_idx < poll_nof_times; poll_idx++)
    {
        if (poll_delay)
        {
            cli_out(" Wait %d Microseconds \n", poll_delay);
            sal_usleep(poll_delay);
        }

        DNXCMN_CORES_ITER(unit, core_id, core)
        {

            PRT_TITLE_SET("MAX VOQ OCCUPANCY: Core %d", core);
            PRT_COLUMN_ADD("VOQ");
            PRT_COLUMN_ADD("Queue Size [Bytes]");
            PRT_COLUMN_ADD("OCB Queue Size [Bytes]");
            PRT_COLUMN_ADD("OCB Buffers");
            PRT_COLUMN_ADD("OCB NOF Packet Descriptors");

            /** Taking a handle to DBAL table */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_CGM_VOQ_OCCUPANCY, &entry_handle_id));

            /** INIT DBAL iterator */

            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
            /** Select the core */
            SHR_IF_ERR_EXIT(dbal_iterator_key_field32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CONDITION_EQUAL_TO, (uint32) core, 0));

            /** Skip all VOQs that have ZERO occupancy*/
            SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                                        (unit, entry_handle_id, DBAL_FIELD_SIZE_IN_WORDS, INST_SINGLE, DBAL_CONDITION_NOT_EQUAL_TO, (uint32) 0, 0));

            /**
             * Get the first entry that is successfully read from HW
             * On high traffic HW access to some of the entries might fail, 
             * users should still be able to read this diagnostic for the successfully 
             * read entries, so we skip HW errors.
             */
            is_end = 0;
            while ((!is_end))
            {
                rv = dbal_iterator_get_next(unit, entry_handle_id, &is_end);
                if (rv != _SHR_E_NONE)
                {
                   /**
                    * If the iterator failed to get the entry, continue with the next one
                    */
                    error_reading_from_hw = TRUE;
                    continue;
                }
                else
                {
                    /**
                     * If the iterator got the entry successful, break the loop
                     */
                    break;
                }
            }

            if (most_provided)
            {
                /** PRINTOUT for the X number of most congested queues */
                /*
                 * Populate the array with the most occupied queues 
                 * { 
                 */
                while (!is_end)
                {
                    /** Get the current voq occupancy info only if the read from HW was successful */
                    if (rv == _SHR_E_NONE)
                    {
                        /** Get cuurent voq occupancy info */
                        SHR_IF_ERR_EXIT(dnx_ingress_congestion_current_voq_occupancy_info_get
                                        (unit, entry_handle_id, &current_voq_occupancy_info));

                        /** Check if current voq occupancy is bigger than the smallest
                         *  currently existing in the most occupied array */
                        if (current_voq_occupancy_info.size < current_min_voq_size)
                        {
                            /** Update the array */
                            voq_non_empty_info_array[current_min_voq_size_idx] = current_voq_occupancy_info;

                            /** After updating the max voq occupancy array find the new
                             *  smallest value in it and its index for later use in next
                             *  iterations */
                            SHR_IF_ERR_EXIT(diag_dnx_tm_most_not_congested_voq_from_array_find
                                            (unit, voq_non_empty_info_array, nof_most_occupied_voqs_to_show,
                                             &current_min_voq_size_idx, &current_min_voq_size));
                        }
                    }

                    /** Go to next non empty voq
                     * On high traffic HW access to some of the entries might fail, 
                     * users should still be able to read this diagnostic for the successfully 
                     * read entries, so we skip HW errors.
                     */
                    rv = dbal_iterator_get_next(unit, entry_handle_id, &is_end);
                    if (rv != _SHR_E_NONE)
                    {
                        error_reading_from_hw = TRUE;
                    }

                }
                /*
                 * } 
                 */
                /** Print the most populated VOQs */
                for (voq_idx = 0; voq_idx < nof_most_occupied_voqs_to_show; voq_idx++)
                {
                    /** Print only if we actually populated all the entries from the
                     *  voq occupancy array */
                    if (voq_non_empty_info_array[voq_idx].size != 0)
                    {
                        DIAG_DNX_QUEUE_NON_EMPTY_ROW_PRINT(voq_non_empty_info_array[voq_idx]);
                    }
                }
            }
            else
            {
                /** PRINTOUT for all voqs that are not empty */
                while (!is_end)
                {
                    /** Get the current voq occupancy info only if the read from HW was successful */
                    if (rv == _SHR_E_NONE)
                    {
                        SHR_IF_ERR_EXIT(dnx_ingress_congestion_current_voq_occupancy_info_get
                                        (unit, entry_handle_id, &current_voq_occupancy_info));

                        DIAG_DNX_QUEUE_NON_EMPTY_ROW_PRINT(current_voq_occupancy_info);
                    }

                    /** Go to next non empty voq
                     * On high traffic HW access to some of the entries might fail, 
                     * users should still be able to read this diagnostic for the successfully 
                     * read entries, so we skip HW errors.
                     */
                    rv = dbal_iterator_get_next(unit, entry_handle_id, &is_end);
                    if (rv != _SHR_E_NONE)
                    {
                        error_reading_from_hw = TRUE;
                    }

                }
            }
            /*
             * } 
             */
            PRT_COMMITX;
        }
        /** If there was an error indication,
         *  need to indicate to the user that not all information was accurately provided.
         */
        if (error_reading_from_hw == TRUE)
        {
            cli_out("WARNING: Access to some VOQs failed!\n");
        }
    }
exit:
    SHR_FREE(voq_non_empty_info_array);
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * Queue count
 * { 
 */
/**
 * \brief MAN for DNX TM Ingress Queue counter info diag
 */
sh_sand_man_t sh_dnx_tm_queue_count_man = {
/* *INDENT-OFF* */
    .brief    = "Queue counter information",
    .full     = "Diagnostic to provide per Queue or per BaseQueue counter information \n"
                "If BaseQueue is provided the info will be returned for the whole queue bundle."
                "If Queue is provided the info will be returned only for the specified queue."
                "The counters provided are: \n"
                "   - SRAM enqueued packets\n"
                "   - SRAM discarded packets\n"
                "   - SRAM to Fabric dequeued packets\n"
                "   - SRAM to DRAM dequeued packets\n"
                "   - SRAM deleted packets\n"
                "   - DRAM dequeued bundles\n"
                "   - DRAM deleted bundles\n"
                "There is an option to count this statistics for a given time interval. \n"
                "The time interval is given in microseconds",
    .synopsis = "[Queue=<id>] [BaseQueue=<id>] [InTerVal=<val>] [core=<id>]",
    .examples = "q=34 \n"
                "bq=32 \n"
                "q=34 itv=100",

/* *INDENT-ON* */
};

/**
 * \brief Options for DNX TM Ingress Queue counter info diag
 */
sh_sand_option_t sh_dnx_tm_queue_count_options[] = {
/* *INDENT-OFF* */
    /*name        type                   desc              default*/
    {"queue",     SAL_FIELD_TYPE_UINT32, "Queue range",     "0", tm_queue_id_table, "0-max"},
    {"basequeue", SAL_FIELD_TYPE_UINT32, "BaseQueue range", "0", tm_queue_id_table, "0-max"},
    {"interval",  SAL_FIELD_TYPE_UINT32, "Time interval",   "0"},
    {NULL}
/* *INDENT-ON* */
};

/**
 * \brief - Print global info about queues.
 */
shr_error_e
sh_dnx_tm_queue_count_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 queue_id, base_queue_id, interval = 0;
    int queue_id_provided, base_queue_id_provided;
    dnx_ingress_congestion_counter_info_t counter_info;
    int core, core_id;
    bcm_gport_t base_gport = BCM_GPORT_INVALID, logical_port = BCM_GPORT_INVALID;
    int num_cos = 1;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;
    uint32 cosq_flags;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("queue", queue_id_provided);
    SH_SAND_IS_PRESENT("basequeue", base_queue_id_provided);

    if (!queue_id_provided && !base_queue_id_provided)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM,
                     "Please choose if Queue or BaseQueue info is requested. See 'tm ingress queue count usage' for more info \n");

    }

    if (queue_id_provided && base_queue_id_provided)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM,
                     "Providing Queue id and BaseQueue id at the same time is not allowed. See 'tm ingress queue count usage' for more info \n");

    }
    /** get input parameters */
    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_UINT32("queue", queue_id);
    SH_SAND_GET_UINT32("basequeue", base_queue_id);
    SH_SAND_GET_UINT32("interval", interval);

    /*
     * Check if base_queue id is provided 
     * If so find from how many queues the bundle has. 
     * If no set base_queue_id to the queue_id provided and 
     * set that the nof queues requested are only one. 
     */
    if (base_queue_id_provided)
    {
        /*
         * Find if the base_queue_id provided is pointing to a UC or MC group.
         *  The purpose of this is to see how many queues are there in the bundle.
         */
        SHR_IF_ERR_EXIT(dnx_ipq_queue_gport_get(unit, BCM_CORE_ALL, base_queue_id, &base_gport));

        
        SHR_IF_ERR_EXIT(bcm_cosq_gport_get(unit, base_gport, &logical_port, &num_cos, &cosq_flags));
    }
    else
    {
        base_queue_id = queue_id;
        num_cos = 1;
    }

    DNXCMN_CORES_ITER(unit, core_id, core)
    {
        if (base_queue_id_provided)
        {
            PRT_TITLE_SET("BaseQueue: %d [NUM COS: %d] Core: %d counter statistics", base_queue_id, num_cos, core);
        }
        else
        {
            PRT_TITLE_SET("Queue: %d Core: %d counter statistics", queue_id, core);
        }
        PRT_COLUMN_ADD("Counter name");
        PRT_COLUMN_ADD("Value");

        /** Configure the programmable counter to track only for this
         *  Queue or QueueBundle */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_programmable_counter_configuration_set
                        (unit, core, base_queue_id, num_cos));

        if (interval != 0)
        {
            gtimer_time.time_units = DNXCMN_TIME_UNIT_USEC;
            COMPILER_64_SET(gtimer_time.time, 0, interval);

            /** get number of cycles in a second and use it in gtimer.
             * this way the the value presented is in units of Hz */
            SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

            SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_CGM, core));
            SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_CGM, core));

        }

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_programmable_counter_info_get(unit, core, &counter_info));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("SRAM Enqueued packets");
        PRT_CELL_SET("%u", counter_info.sram_enq_pkt_cnt);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("SRAM Rejected packets");
        PRT_CELL_SET("%u", counter_info.sram_disc_pkt_cnt);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("SRAM to DRAM Dequeued packets");
        PRT_CELL_SET("%u", counter_info.sram_to_dram_pkt_cnt);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("SRAM to Fabric Dequeued packets");
        PRT_CELL_SET("%u", counter_info.sram_to_fabric_pkt_cnt);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("SRAM Deleted packets");
        PRT_CELL_SET("%u", counter_info.sram_del_pkt_cnt);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("DRAM Dequeued bundles");
        PRT_CELL_SET("%u", counter_info.dram_deq_bundle_cnt);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("DRAM Deleted bundles");
        PRT_CELL_SET("%u", counter_info.dram_del_bundle_cnt);

        PRT_COMMITX;

        /** Disable the counter */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_programmable_counter_voq_init_config_set(unit, core));

    }

exit:
    if (interval != 0)
    {
        SHR_IF_ERR_CONT(dnx_gtimer_clear(unit, SOC_BLK_CGM, -1));
    }
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/* 
 * --------------------------------------------------------------------------
 * dnx tm ingress vsq
 * --------------------------------------------------------------------------
 */

sh_sand_option_t sh_dnx_tm_resource_alloc_options[] = {
/* *INDENT-OFF* */
    /*name        type                  desc               default   ext*/
    {NULL}
/* *INDENT-ON* */
};

sh_sand_man_t sh_dnx_tm_resource_alloc_man = {
    .brief = "Print global VSQ resource allocation",
    .full = NULL,
    .synopsis = NULL,
    .examples = NULL
};

typedef struct
{
    uint32 shared[2];
    uint32 guaranteed;
    uint32 headroom;
    uint32 max_headroom;
    uint32 used;
    uint32 unused;
} sh_dnx_tm_resource_alloc_info_t;

/**
 * \brief - Print VSQ global resource allocation
 */
shr_error_e
sh_dnx_tm_resource_alloc_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sh_dnx_tm_resource_alloc_info_t resource_info[DNX_INGRESS_CONGESTION_RESOURCE_NOF];

    dnx_ingress_congestion_resource_type_e resource;
    int core_index;
    uint32 granularity, unused, maximal;

    bcm_cosq_allocation_entity_t target;
    bcm_cosq_resource_amounts_t amounts;
    int pool_id;

    bcm_cosq_resource_t bcm_resource[] = {
        bcmResourceBytes,
        bcmResourceOcbBuffers,
        bcmResourceOcbPacketDescriptors
    };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_index)
    {
        sal_memset(resource_info, 0, sizeof(resource_info));

        BCM_COSQ_GPORT_VSQ_GL_SET(target.gport, core_index);
        for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
             resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            for (pool_id = 0; pool_id < dnx_data_ingr_congestion.vsq.pool_nof_get(unit); pool_id++)
            {
                target.pool_id = pool_id;
                target.color = 0; /** irrelevant */

                SHR_IF_ERR_EXIT(bcm_cosq_resource_allocation_get(unit, 0, bcm_resource[resource], &target, &amounts));
                resource_info[resource].shared[pool_id] = amounts.max_shared_pool;
                /*
                 * Headroom and max headroom can be defined for lossless pool only
                 * but the API returns it for each pool
                 * headroom of non lossless pool is 0
                 */
                resource_info[resource].headroom += amounts.nominal_headroom;
                resource_info[resource].max_headroom += amounts.max_headroom;
                /*
                 * guaranteed is total guaranteed and it is not per pool
                 * both invocations return the same value
                 */
                resource_info[resource].guaranteed = amounts.reserved;
                /** note, guaranteed is not counted */
                resource_info[resource].used += amounts.max_shared_pool + amounts.nominal_headroom;
            }

            /** now count guaranteed, just once */
            resource_info[resource].used += resource_info[resource].guaranteed;
        }

        PRT_TITLE_SET("Core %d Resource Allocation", core_index);
        PRT_COLUMN_ADD("Parameter");

        for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
             resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            PRT_COLUMN_ADD("%s", sh_dnx_tm_ingress_congestion_resource_str(resource));
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Nominal Headroom");
        for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
             resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            PRT_CELL_SET("%u", resource_info[resource].headroom);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("(Max Headroom)");
        for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
             resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            PRT_CELL_SET("%u", resource_info[resource].max_headroom);
        }

        for (pool_id = 0; pool_id < dnx_data_ingr_congestion.vsq.pool_nof_get(unit); pool_id++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("Pool %d", pool_id);
            for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
                 resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
            {
                PRT_CELL_SET("%u", resource_info[resource].shared[pool_id]);
            }
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Reserved");
        for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
             resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            PRT_CELL_SET("%u", resource_info[resource].guaranteed);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Maximal");
        for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
             resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            /*
             * total amount of descriptors can't be written to HW. 
             * With addition of granularity, there could be unreal difference of granularity (accuracy lost) between total and sum of all used.
             */
            PRT_CELL_SET("%u (0x%x)", dnx_data_ingr_congestion.info.resource_get(unit, resource)->max,
                         dnx_data_ingr_congestion.info.resource_get(unit, resource)->max);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Unused");
        for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
             resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            /*
             * Due to granularity of thresholds, sum of all thresholds might be lower than total available.
             * If the difference is of granularity size, report unused as 0
             */
            granularity =
                (resource ==
                 DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES ? dnx_data_ingr_congestion.
                 info.bytes_threshold_granularity_get(unit) : dnx_data_ingr_congestion.info.
                 threshold_granularity_get(unit));
            maximal = dnx_data_ingr_congestion.info.resource_get(unit, resource)->max;
            unused = (maximal - resource_info[resource].used + 1) / granularity * granularity;
            unused = (unused <= granularity ? 0 : unused);
            PRT_CELL_SET("%u", unused);
        }

        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * VSQ count
 * { 
 */
/**
 * \brief MAN for DNX TM Ingress Queue autocredit diagnostics
 */
static sh_sand_man_t sh_dnx_tm_vsq_count_man = {
/* *INDENT-OFF* */
    .brief    = "Shows queue counter information",
    .full     = "Diagnostic to provide per VSQ or VSQ and queue counter information \n"
                DIAG_DNX_VSQ_GROUPS_LEGEND
                "The counters provided are: \n"
                "   - VSQ enqueued packets\n"
                "   - VSQ discarded packets\n"
                "There is an option to count this statistics for a given time interval. \n"
                "The time interval is given in microseconds",
    .synopsis = "GRouP=<a/b/c/d/e/f> [Queue=<queue_id>] [InTerVal=<val>] [CORE=<id>]",
    .examples = "GRouP=a \n"
                "GRouP=e Queue=5 \n"
                "GRouP=b InTerVal=100",

/* *INDENT-ON* */
};

static sh_sand_enum_t dnx_vsq_group_enum_table[] = {
    {"A", DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY, "VSQ GROUP A - category"},
    {"B", DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_TRAFFIC_CLS, "VSQ GROUP B - category and traffic class"},
    {"C", DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_CNCTN_CLS, "VSQ GROUP C - category and connection class"},
    {"D", DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG, "VSQ GROUP D - statistical flows"},
    {"E", DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT, "VSQ GROUP E - source port"},
    {"F", DNX_INGRESS_CONGESTION_VSQ_GROUP_PG, "VSQ GROUP F - source port priority_group"},
    {NULL}
};

/**
 * \brief Options for DNX TM Ingress VSQ counter diagnostic
 */
static sh_sand_option_t sh_dnx_tm_vsq_count_options[] = {
/* *INDENT-OFF* */
    /*name          type                    desc                             default*/
    {"group",       SAL_FIELD_TYPE_ENUM,    "VSQ Group (a/b/c/d/e/f)",       NULL,   (void *) dnx_vsq_group_enum_table},
    {"queue",       SAL_FIELD_TYPE_UINT32,  "VSQ index",                     "0"},
    {"interval",    SAL_FIELD_TYPE_UINT32,  "Time interval",                 "0"},
    {NULL}
/* *INDENT-ON* */
};

/**
 * \brief - Print vsq counter info.
 */
static shr_error_e
sh_dnx_tm_vsq_count_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 queue_id, interval = 0;
    int queue_id_provided;
    dnx_ingress_congestion_vsq_group_e vsq_group;
    dnx_ingress_congestion_counter_info_t counter_info;
    int core, core_id, max_nof_queue_ids_in_vsq;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("queue", queue_id_provided);

    /** get input parameters */
    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_ENUM("group", vsq_group);
    SH_SAND_GET_UINT32("interval", interval);
    SH_SAND_GET_UINT32("queue", queue_id);

    /** Get max NOF queue_ids per VSQ group */
    max_nof_queue_ids_in_vsq = dnx_data_ingr_congestion.vsq.info_get(unit, vsq_group)->nof;

    if (queue_id_provided && queue_id >= max_nof_queue_ids_in_vsq)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Queue id %d is bigger than MAX number of queue id for this group.\n", queue_id);
    }

    DNXCMN_CORES_ITER(unit, core_id, core)
    {
        if (queue_id_provided)
        {
            PRT_TITLE_SET("VSQ: %s Queue_id: %d Core: %d counter statistics",
                          sh_sand_enum_str(dnx_vsq_group_enum_table, vsq_group), queue_id, core);
        }
        else
        {
            PRT_TITLE_SET("VSQ: %s Core: %d counter statistics", sh_sand_enum_str(dnx_vsq_group_enum_table, vsq_group),
                          core);
        }
        PRT_COLUMN_ADD("Counter name");
        PRT_COLUMN_ADD("Value");

        /** Configure the programmable counter to track all or
         *  specific vsq of a given VSQ group */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_programmable_counter_configuration_set
                        (unit, core, vsq_group, queue_id, queue_id_provided));

        if (interval != 0)
        {
            gtimer_time.time_units = DNXCMN_TIME_UNIT_USEC;
            COMPILER_64_SET(gtimer_time.time, 0, interval);

            /** get number of cycles in a second and use it in gtimer.
             * this way the the value presented is in units of Hz */
            SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

            SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_CGM, core));
            SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_CGM, core));
        }

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_programmable_counter_info_get(unit, core, &counter_info));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("SRAM Enqueued packets");
        PRT_CELL_SET("%u", counter_info.sram_enq_pkt_cnt);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("SRAM Rejected packets");
        PRT_CELL_SET("%u", counter_info.sram_disc_pkt_cnt);

        PRT_COMMITX;

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_programmable_counter_vsq_init_config_set(unit, core));
    }

exit:
    if (interval != 0)
    {
        SHR_IF_ERR_CONT(dnx_gtimer_clear(unit, SOC_BLK_CGM, -1));
    }
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * VSQ NON empty 
 * { 
 */

/**
 * \brief MAN for DNX TM Ingress VSQ occupancy levels diag
 */
static sh_sand_man_t sh_dnx_tm_vsq_non_empty_man = {
/* *INDENT-OFF* */
    .brief    = "Show non empty VSQs",
    .full     = "Diagnostic to show the occupancy of all VSQs\n"
                DIAG_DNX_VOQ_VSQ_NON_EMPTY_FILTERS_LEGEND,
    .synopsis = "GRouP=<a/b/c/d/e/f> [CORE=<core_id>] [MOST=<nof>]"
                "[POLL_NOF_times=<nof_times>] [POLL_Delay=<delay in micro second>]",
    .examples = "GRouP=a CORE=0\n"
                "GRouP=f most=5 POLL_NOF_times=2 POLL_Delay=50",
/* *INDENT-ON* */
};

/**
 * \brief MAN for DNX TM Ingress Queue occupancy levels diag
 */
static sh_sand_option_t sh_dnx_tm_vsq_non_empty_options[] = {
/* *INDENT-OFF* */
    /*name                  type                   desc                                         default*/
    {"group",               SAL_FIELD_TYPE_ENUM,   "VSQ Group (a/b/c/d/e/f)",                   NULL,   (void *) dnx_vsq_group_enum_table},
    {"most",                SAL_FIELD_TYPE_INT32,  "NOF of most occupied queues to display",    "0"},
    {"poll_nof_times",      SAL_FIELD_TYPE_INT32,  "Number of polling times",                   "1"},
    {"poll_delay",          SAL_FIELD_TYPE_INT32,  "Delay inbetween pollings",                  "0"},
    {NULL}
/* *INDENT-ON* */
};

/**
 * \brief - 
 */
static shr_error_e
diag_dnx_tm_most_not_congested_vsq_queue_from_array_find(
    int unit,
    dnx_ingress_congestion_vsq_occupancy_info_t * vsq_non_empty_info_array,
    int nof_queues_to_display,
    int *current_min_vsq_idx,
    uint32 *current_min_vsq_size)
{
    int vsq_idx;

    SHR_FUNC_INIT_VARS(unit);

    /** Find current minimum vsq size */
    *current_min_vsq_size = vsq_non_empty_info_array[0].resources[DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES].total;
    for (vsq_idx = 1; vsq_idx < nof_queues_to_display; vsq_idx++)
    {
        if (vsq_non_empty_info_array[vsq_idx].resources[DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES].total <
            *current_min_vsq_size)
        {
            *current_min_vsq_size =
                vsq_non_empty_info_array[vsq_idx].resources[DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES].total;
            *current_min_vsq_idx = vsq_idx;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Print a row for vsq non_empty diagnostic
 */
static shr_error_e
diag_dnx_tm_vsq_non_empty_row_print(
    int unit,
    int vsq_group,
    dnx_ingress_congestion_vsq_occupancy_info_t current_vsq_occupancy_info,
    prt_control_t * prt_ctr)
{
    int resource = 0, pool_idx, nof_pools;

    SHR_FUNC_INIT_VARS(unit);

    nof_pools =
        (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT ? dnx_data_ingr_congestion.vsq.pool_nof_get(unit) : 1);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("%u", current_vsq_occupancy_info.vsq_id);

    if (vsq_group <= DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG)
    {
        for (resource = 0; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            PRT_CELL_SET("%u", current_vsq_occupancy_info.resources[resource].total);
        }
    }
    else
    {
        /** For BOTH GROUP E and GROUP F */
        for (resource = 0; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            for (pool_idx = 0; pool_idx < nof_pools; pool_idx++)
            {
                PRT_CELL_SET("%u", current_vsq_occupancy_info.resources[resource].guaranteed[pool_idx]);
                PRT_CELL_SET("%u", current_vsq_occupancy_info.resources[resource].shared[pool_idx]);
                PRT_CELL_SET("%u", current_vsq_occupancy_info.resources[resource].headroom[pool_idx]);
            }
            if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
            {
                /** Only for group F print also the occupancy of the headroom extension */
                if (resource != DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)
                {
                    PRT_CELL_SET("%u", current_vsq_occupancy_info.resources[resource].headroom_ex);
                }
            }

            PRT_CELL_SKIP(1);
        }

    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * adds columns in case of vsq a to d in case of vsq non empty cmd
 */
static shr_error_e
column_add_vsq_a_to_d(
    int unit,
    prt_control_t * prt_ctr)
{
    SHR_FUNC_INIT_VARS(unit);
    PRT_COLUMN_ADD("Size [Bytes]");
    PRT_COLUMN_ADD("NOF OCB Buffers");
    PRT_COLUMN_ADD("NOF OCB Packet Descriptors");

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * adds columns in case of vsq e in case of vsq non empty cmd
 */

static shr_error_e
column_add_vsq_e(
    int unit,
    int pool_idx,
    int nof_pools,
    prt_control_t * prt_ctr)
{
    SHR_FUNC_INIT_VARS(unit);
    /** for GROUP E only, need to add columns for pool1 **/
    for (pool_idx = 0; pool_idx < nof_pools; pool_idx++)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Guaranteed Pool %d [Bytes]", pool_idx);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Shared Pool %d [Bytes]", pool_idx);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Headroom Pool %d [Bytes]", pool_idx);
    }
    PRT_COLUMN_ADD("");
    for (pool_idx = 0; pool_idx < nof_pools; pool_idx++)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Guaranteed Pool %d [OCB Buffers]", pool_idx);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Shared Pool %d [OCB Buffers]", pool_idx);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Headroom Pool %d [OCB Buffers]", pool_idx);
    }
    PRT_COLUMN_ADD("");
    for (pool_idx = 0; pool_idx < nof_pools; pool_idx++)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Guaranteed Pool %d [OCB PDS]", pool_idx);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Shared Pool %d [OCB PDS]", pool_idx);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Headroom Pool %d [OCB PDS]", pool_idx);
    }
    PRT_COLUMN_ADD("");
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * adds columns in case of vsq f in case of vsq non empty cmd
 */

static shr_error_e
columns_add_vsq_f(
    int unit,
    prt_control_t * prt_ctr)
{
    SHR_FUNC_INIT_VARS(unit);
    /**DNX_INGRESS_CONGESTION_VSQ_GROUP_PG*/
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Guaranteed [Bytes]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Shared [Bytes]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Headroom [Bytes]");
    PRT_COLUMN_ADD("");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Guaranteed [OCB Buffers]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Shared [OCB Buffers]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Headroom [OCB Buffers]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Headroom Extension [OCB Buffers]");
    PRT_COLUMN_ADD("");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Guaranteed [OCB PDS]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Shared [OCB PDS]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Headroom [OCB PDS]");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Headroom Extension [OCB PDS]");
    PRT_COLUMN_ADD("");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print global occupancy info about VSQs.
 */
static shr_error_e
sh_dnx_tm_vsq_non_empty_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    shr_error_e rv = 0;
    int core, core_id;
    int poll_nof_times, poll_delay, nof_pools, pool_idx = 0;
    int poll_idx = 0, most_provided, nof_vsqs_to_display;
    uint8 error_reading_from_hw = FALSE;
    uint32 entry_handle_id;
    int is_end, vsq_idx;
    dnx_ingress_congestion_vsq_occupancy_info_t *vsq_non_empty_info_array = NULL;
    dnx_ingress_congestion_vsq_occupancy_info_t current_vsq_occupancy_info;
    int max_nof_queue_ids_in_vsq;
    dnx_ingress_congestion_vsq_group_e vsq_group;
    int current_min_vsq_idx = 0;
    uint32 current_min_vsq_size = 0;
    int vsq_occupancy_tables_array[] = {
        DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_A_OCCUPANCY, DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_B_OCCUPANCY,
        DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_C_OCCUPANCY, DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_D_OCCUPANCY,
        DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_E_OCCUPANCY, DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_F_OCCUPANCY
    };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get parameters */
    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_ENUM("group", vsq_group);
    SH_SAND_GET_INT32("most", nof_vsqs_to_display);
    SH_SAND_GET_INT32("poll_nof_times", poll_nof_times);
    SH_SAND_GET_INT32("poll_delay", poll_delay);
    SH_SAND_IS_PRESENT("most", most_provided);

    nof_pools =
        (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT ? dnx_data_ingr_congestion.vsq.pool_nof_get(unit) : 1);

    if (most_provided)
    {
        /** Get max NOF queue_ids per VSQ group */
        max_nof_queue_ids_in_vsq = dnx_data_ingr_congestion.vsq.info_get(unit, vsq_group)->nof;

        /** Check if user did't requested more queues stats than the
         *  VSQ group has */
        if (nof_vsqs_to_display >= max_nof_queue_ids_in_vsq)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "VSQ group %s has only %d queues. Please give smaller number for 'most' input! \n",
                         sh_sand_enum_str(dnx_vsq_group_enum_table, vsq_group), max_nof_queue_ids_in_vsq);
        }

        SHR_ALLOC_SET_ZERO(vsq_non_empty_info_array,
                           sizeof(dnx_ingress_congestion_vsq_occupancy_info_t) * nof_vsqs_to_display,
                           "sh_dnx_tm_queue_non_empty_cmd.dnx_ingress_congestion_vsq_occupancy_info_t", "%s%s%s\r\n",
                           EMPTY, EMPTY, EMPTY);

    }

    for (poll_idx = 0; poll_idx < poll_nof_times; poll_idx++)
    {
        if (poll_delay)
        {
            cli_out(" Wait %d Microseconds \n", poll_delay);
            sal_usleep(poll_delay);
        }

        DNXCMN_CORES_ITER(unit, core_id, core)
        {

            PRT_TITLE_SET("MAX VSQ %s OCCUPANCY: Core %d", sh_sand_enum_str(dnx_vsq_group_enum_table, vsq_group), core);
            PRT_COLUMN_ADD("VSQ ID");

            if (vsq_group <= DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG)
            {
                column_add_vsq_a_to_d(unit, prt_ctr);
            }
            else if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
            {
               /** for GROUP E only, need to add columns for pool1 **/
                column_add_vsq_e(unit, pool_idx, nof_pools, prt_ctr);
            }
            else
            {
                /**DNX_INGRESS_CONGESTION_VSQ_GROUP_PG*/
                columns_add_vsq_f(unit, prt_ctr);
            }

            /** Taking a handle to DBAL table */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, vsq_occupancy_tables_array[vsq_group], &entry_handle_id));

            /** Skip all VSQ queues that have ZERO ocuupancy (field vlaues
             *  are 0 by default) */
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

            /** Select the core */
            SHR_IF_ERR_EXIT(dbal_iterator_key_field32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CONDITION_EQUAL_TO, (uint32) core, 0));
            /**
             * Get the first entry that is successfully read from HW
             * On high traffic HW access to some of the entries might fail, 
             * users should still be able to read this diagnostic for the successfully 
             * read entries, so we skip HW errors.
             */
            is_end = 0;
            while ((!is_end))
            {
                rv = dbal_iterator_get_next(unit, entry_handle_id, &is_end);
                if (rv != _SHR_E_NONE)
                {
                   /**
                    * If the iterator failed to get the entry, continue with the next one
                    */
                    error_reading_from_hw = TRUE;
                    continue;
                }
                else
                {
                    /**
                     * If the iterator got the entry successful, break the loop
                     */
                    break;
                }
            }

            if (most_provided)
            {
                /** PRINTOUT for the X number of most congested queues */
                /*
                 * Populate the array with the most occupied queues
                 * {
                 */
                while (!is_end)
                {
                    /** Get the current VSQ occupancy info only if the read from HW was successful */
                    if (rv == _SHR_E_NONE)
                    {
                        SHR_IF_ERR_EXIT(dnx_ingress_congestion_current_vsq_occupancy_info_get
                                        (unit, entry_handle_id, vsq_group, &current_vsq_occupancy_info));

                        /** Check if current queue occupancy is bigger than the smallest
                         *  currently existing in the most occupied array */
                        if (current_vsq_occupancy_info.resources[DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES].total <
                            current_min_vsq_size)
                        {
                            /** Update the array */
                            vsq_non_empty_info_array[current_min_vsq_idx] = current_vsq_occupancy_info;

                            /** After updating the max VSQ queues occupancy array find the
                             *  new smallest value in it and its index for later use in next
                             *  iterations */
                            SHR_IF_ERR_EXIT(diag_dnx_tm_most_not_congested_vsq_queue_from_array_find
                                            (unit, vsq_non_empty_info_array, nof_vsqs_to_display, &current_min_vsq_idx,
                                             &current_min_vsq_size));
                        }
                    }

                    /**
                     * Go to next non empty VSQ
                     * On high traffic HW access to some of the entries might fail, 
                     * users should still be able to read this diagnostic for the successfully 
                     * read entries, so we skip HW errors.
                     */
                    rv = dbal_iterator_get_next(unit, entry_handle_id, &is_end);
                    if (rv != _SHR_E_NONE)
                    {
                        error_reading_from_hw = TRUE;
                    }
                }
                /*
                 * }
                 */
                /** Print the most populated Queues from this VSQ */
                for (vsq_idx = 0; vsq_idx < nof_vsqs_to_display; vsq_idx++)
                {
                    /** Print current row only if not 0 */
                    if (vsq_non_empty_info_array[vsq_idx].resources[DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES].total)
                    {
                        SHR_IF_ERR_EXIT(diag_dnx_tm_vsq_non_empty_row_print
                                        (unit, vsq_group, vsq_non_empty_info_array[vsq_idx], prt_ctr));
                    }
                }
            }
            else
            {
                /** PRINTOUT for all VSQ queues that are not empty */
                while (!is_end)
                {
                    /** Get the current VSQ occupancy info only if the read from HW was successful */
                    if (rv == _SHR_E_NONE)
                    {
                        SHR_IF_ERR_EXIT(dnx_ingress_congestion_current_vsq_occupancy_info_get
                                        (unit, entry_handle_id, vsq_group, &current_vsq_occupancy_info));

                        /** Print current row */
                        SHR_IF_ERR_EXIT(diag_dnx_tm_vsq_non_empty_row_print
                                        (unit, vsq_group, current_vsq_occupancy_info, prt_ctr));
                    }

                    /** Go to next non empty VSQ
                     * On high traffic HW access to some of the entries might fail, 
                     * users should still be able to read this diagnostic for the successfully 
                     * read entries, so we skip HW errors.
                     */
                    rv = dbal_iterator_get_next(unit, entry_handle_id, &is_end);
                    if (rv != _SHR_E_NONE)
                    {
                        error_reading_from_hw = TRUE;
                    }
                }
            }
            /*
             * Go over all entries different than default (0) }
             */
            PRT_COMMITX;
        }

        /** If there was an error indication,
         *  need to indicate to the user that not all information was accurately provided.
         */
        if (error_reading_from_hw == TRUE)
        {
            cli_out("WARNING: Access to some VSQs failed!\n");
        }
    }
exit:
    SHR_FREE(vsq_non_empty_info_array);
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/* 
 * VSQ info
 * {
 */
/**
 * \brief MAN for DNX TM Ingress VSQ info
 */
static sh_sand_man_t sh_dnx_tm_vsq_info_man = {
/* *INDENT-OFF* */
    .brief    = "Show VSQ threshold, resource and occupancy info",
    .full     = "Diagnostic for printing per VSQ its:\n"
                "\t - allocated resources,\n"
                "\t - DROP and WRED thresholds\n"
                "\t - current occupancy\n"
                DIAG_DNX_VSQ_GROUPS_LEGEND
                "The other mandatory input is the VSQ index\n",
    .synopsis = "GRouP=<a/b/c/d/e/f> Queue=<vsq_id> [CORE=<core_id>]",
    .examples = "GRouP=a Queue=0 \n"
                "GRouP=e Queue=2 CORE=0 \n"
                "GRouP=f Queue=10",
/* *INDENT-ON* */
};

/**
 * \brief Options for DNX TM Ingress VSQ info diagnostics
 */
static sh_sand_option_t sh_dnx_tm_vsq_info_options[] = {
/* *INDENT-OFF* */
    /*name        type                      desc                      default*/
    {"group",     SAL_FIELD_TYPE_ENUM,    "VSQ Group (a/b/c/d/e/f)",  NULL,   (void *) dnx_vsq_group_enum_table},
    {"queue",     SAL_FIELD_TYPE_INT32,   "VSQ index",                NULL},
    {NULL}
/* *INDENT-ON* */
};

/**
 * \brief - Print general info on VSQ
 */
static shr_error_e
sh_dnx_tm_vsq_general_info_print(
    int unit,
    int core,
    int vsq_group,
    int vsq_group_idx,
    sh_sand_control_t * sand_control)
{
    int32 vsq_rt_cls;
    dnx_ingress_congestion_pg_params_t pg_params;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("VSQ %d (Group %s)", vsq_group_idx, sh_sand_enum_str(dnx_vsq_group_enum_table, vsq_group));
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");

    SHR_IF_ERR_EXIT(dnx_vsq_rate_class_mapping_get(unit, core, vsq_group, vsq_group_idx, &vsq_rt_cls));

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Rate class");
    PRT_CELL_SET("%d", vsq_rt_cls);

    if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_pg_params_get(unit, core, vsq_group_idx, &pg_params));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Pool id");
        PRT_CELL_SET("%d", pg_params.pool_id);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Is lossless");
        PRT_CELL_SET("%s", pg_params.is_lossless ? "TRUE" : "FALSE");
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print a guaranteed info for source based VSQs 
 */
static shr_error_e
sh_dnx_tm_source_vsq_rate_class_resource_allocation_info_print(
    int unit,
    int core,
    bcm_gport_t vsq_gport,
    int vsq_group,
    int vsq_group_idx,
    sh_sand_control_t * sand_control)
{
    int nof_pools, pool_idx, resource, color_idx;
    bcm_cosq_resource_amounts_t amounts;
    bcm_cosq_allocation_entity_t target;

    uint32 resources_array[DNX_INGRESS_CONGESTION_RESOURCE_NOF] = {
        bcmReservationResourceBytes,
        bcmResourceOcbBuffers,
        bcmResourceOcbPacketDescriptors
    };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Set number of VSQ group pools */
    nof_pools =
        (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT ? dnx_data_ingr_congestion.vsq.pool_nof_get(unit) : 1);

    for (pool_idx = 0; pool_idx < nof_pools; pool_idx++)
    {
        if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
        {
            PRT_TITLE_SET("VSQ %d (Group %s), Pool %d Resource Allocation Info", vsq_group_idx,
                          sh_sand_enum_str(dnx_vsq_group_enum_table, vsq_group), pool_idx);
        }
        else
        {
            DIAG_DNX_VSQ_INFO_TITLE_PRINT("Resource Allocation");
        }

        PRT_COLUMN_ADD("Resources");
        PRT_COLUMN_ADD("DP");
        PRT_COLUMN_ADD("Guaranteed");
        if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
        {
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Max Shared");
        }
        else
        {
            /** Display FADT info */
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FADT Shared Pool Threshold MAX");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FADT Shared Pool Threshold MIN");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FADT Shared Pool Alpha");
        }

        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Nominal Headroom");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Max Headroom");

        for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF;
             resource++)
        {
            for (color_idx = bcmColorGreen; color_idx < bcmColorCount - 1; color_idx++)
            {
                target.is_ocb_only = 0;
                target.gport = vsq_gport;
                target.color = color_idx;
                target.pool_id = pool_idx;

                SHR_IF_ERR_EXIT(bcm_cosq_resource_allocation_get
                                (unit, 0, resources_array[resource], &target, &amounts));

                PRT_ROW_ADD(color_idx == (DNX_COSQ_NOF_DP - 1) ? PRT_ROW_SEP_UNDERSCORE : PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", sh_dnx_tm_ingress_congestion_resource_str(resource));
                PRT_CELL_SET("%d", color_idx);
                PRT_CELL_SET("%u", amounts.reserved);

                if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
                {
                    PRT_CELL_SET("%u", amounts.max_shared_pool);
                }
                else
                {
                    /** Display also FADT info */
                    PRT_CELL_SET("%u", amounts.shared_pool_fadt.thresh_max);
                    PRT_CELL_SET("%u", amounts.shared_pool_fadt.thresh_min);
                    PRT_CELL_SET("%d", amounts.shared_pool_fadt.alpha);

                }
                PRT_CELL_SET("%u", amounts.nominal_headroom);
                PRT_CELL_SET("%u", amounts.max_headroom);
            }
        }
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print a drop info for Destination Based VSQs
 */
static shr_error_e
sh_dnx_tm_dest_vsq_rate_class_drop_info_print(
    int unit,
    int core,
    bcm_gport_t vsq_gport,
    int vsq_group,
    int vsq_group_idx,
    sh_sand_control_t * sand_control)
{
    int resource, color_idx;
    bcm_cosq_gport_size_t gport_size;

    uint32 resource_flags[] = {
        BCM_COSQ_GPORT_SIZE_BYTES,
        BCM_COSQ_GPORT_SIZE_BUFFERS | BCM_COSQ_GPORT_SIZE_OCB,
        BCM_COSQ_GPORT_SIZE_PACKET_DESC | BCM_COSQ_GPORT_SIZE_OCB
    };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    DIAG_DNX_VSQ_INFO_TITLE_PRINT("Drop");

    PRT_COLUMN_ADD("Resource");
    PRT_COLUMN_ADD("DP");
    PRT_COLUMN_ADD("Maximal queue size");

    for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
         resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
    {
        for (color_idx = bcmColorGreen; color_idx < bcmColorCount - 1; color_idx++)
        {
            SHR_IF_ERR_EXIT(bcm_cosq_gport_color_size_get(unit, vsq_gport, 0, color_idx, resource_flags[resource],
                                                          &gport_size));

            PRT_ROW_ADD(color_idx == (DNX_COSQ_NOF_DP - 1) ? PRT_ROW_SEP_UNDERSCORE : PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", sh_dnx_tm_ingress_congestion_resource_str(resource));
            PRT_CELL_SET("%d", color_idx);
            PRT_CELL_SET("%u", gport_size.size_max);

        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print a VSQ wred info
 */
static shr_error_e
sh_dnx_tm_dest_vsq_rate_class_wred_info_print(
    int unit,
    int core,
    bcm_gport_t vsq_gport,
    int vsq_group,
    int vsq_group_idx,
    sh_sand_control_t * sand_control)
{
    int color_idx;
    bcm_cosq_gport_discard_t discard;
    uint32 color_flags_array[DNX_COSQ_NOF_DP] = {
        BCM_COSQ_DISCARD_COLOR_GREEN,
        BCM_COSQ_DISCARD_COLOR_YELLOW,
        BCM_COSQ_DISCARD_COLOR_RED,
        BCM_COSQ_DISCARD_COLOR_BLACK
    };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    DIAG_DNX_VSQ_INFO_TITLE_PRINT("WRED");

    PRT_COLUMN_ADD("Resource");
    PRT_COLUMN_ADD("DP");
    PRT_COLUMN_ADD("Enabled");
    PRT_COLUMN_ADD("Min Threshold");
    PRT_COLUMN_ADD("Max Threshold");
    PRT_COLUMN_ADD("Drop Probability");

    for (color_idx = bcmColorGreen; color_idx < bcmColorCount - 1; color_idx++)
    {
        discard.flags = color_flags_array[color_idx] | BCM_COSQ_DISCARD_BYTES;
        SHR_IF_ERR_EXIT(bcm_cosq_gport_discard_get(unit, vsq_gport, 0, &discard));

        PRT_ROW_ADD(color_idx == (DNX_COSQ_NOF_DP - 1) ? PRT_ROW_SEP_UNDERSCORE : PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Bytes");
        PRT_CELL_SET("%d", color_idx);
        PRT_CELL_SET("%s", (sh_sand_bool_str(discard.flags & BCM_COSQ_DISCARD_ENABLE)));
        PRT_CELL_SET("%u", discard.min_thresh);
        PRT_CELL_SET("%u", discard.max_thresh);
        PRT_CELL_SET("%d", discard.drop_probability);
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print VSQ FC info
 */
static shr_error_e
sh_dnx_tm_vsq_rate_class_fc_info_print(
    int unit,
    int core,
    bcm_gport_t vsq_gport,
    int vsq_group,
    int vsq_group_idx,
    sh_sand_control_t * sand_control)
{
    int resource;
    bcm_cosq_pfc_config_t config;

    uint32 resource_flags[] = {
        BCM_COSQ_PFC_BYTES,
        BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_BUFFERS,
        BCM_COSQ_PFC_OCB | BCM_COSQ_PFC_PACKET_DESC
    };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    DIAG_DNX_VSQ_INFO_TITLE_PRINT("FC");

    PRT_COLUMN_ADD("Resource");
    if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
    {
        /** Display FADT info */
        PRT_COLUMN_ADD("XOFF FADT Threshold MAX");
        PRT_COLUMN_ADD("XOFF FADT Threshold MIN");
        PRT_COLUMN_ADD("XOFF FADT Alpha");
        PRT_COLUMN_ADD("XON FADT Offset");
    }
    else
    {
        PRT_COLUMN_ADD("XOFF Threshold");
        PRT_COLUMN_ADD("XON Threshold");
    }

    for (resource = DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES;
         resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
    {
        SHR_IF_ERR_EXIT(bcm_cosq_pfc_config_get(unit, vsq_gport, 0, resource_flags[resource], &config));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", sh_dnx_tm_ingress_congestion_resource_str(resource));
        if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
        {
            PRT_CELL_SET("%u", config.xoff_fadt_threshold.thresh_max);
            PRT_CELL_SET("%u", config.xoff_fadt_threshold.thresh_min);
            PRT_CELL_SET("%d", config.xoff_fadt_threshold.alpha);
            PRT_CELL_SET("%u", config.xon_fadt_offset);
        }
        else
        {
            PRT_CELL_SET("%u", config.xoff_threshold);
            PRT_CELL_SET("%u", config.xon_threshold);

        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print a VSQ occupancy info
 */
static shr_error_e
sh_dnx_tm_vsq_occupancy_info_print(
    int unit,
    int core,
    int vsq_group,
    int vsq_group_idx,
    sh_sand_control_t * sand_control)
{

    uint32 entry_handle_id;
    int resource, is_end, pool_idx, nof_pools;
    dnx_ingress_congestion_vsq_occupancy_info_t vsq_occupancy_info;

    int vsq_occupancy_tables_array[] = {
        DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_A_OCCUPANCY, DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_B_OCCUPANCY,
        DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_C_OCCUPANCY, DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_D_OCCUPANCY,
        DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_E_OCCUPANCY, DBAL_TABLE_INGRESS_CONG_VSQ_GROUP_F_OCCUPANCY
    };

    dbal_fields_e vsq_id_field[] = {
        DBAL_FIELD_VSQ_A_ID,
        DBAL_FIELD_VSQ_B_ID,
        DBAL_FIELD_VSQ_C_ID,
        DBAL_FIELD_VSQ_D_ID,
        DBAL_FIELD_VSQ_E_ID,
        DBAL_FIELD_VSQ_F_ID
    };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_pools =
        (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT ? dnx_data_ingr_congestion.vsq.pool_nof_get(unit) : 1);

    sal_memset(&vsq_occupancy_info, 0, sizeof(dnx_ingress_congestion_vsq_occupancy_info_t));

    /** Get key to the dbal table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, vsq_occupancy_tables_array[vsq_group], &entry_handle_id));

    /** Init dbal iterator */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    /** Select the VSQ and the core so iteration will be only one */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CONDITION_EQUAL_TO, (uint32) core, 0));
    SHR_IF_ERR_EXIT(dbal_iterator_key_field32_rule_add(unit, entry_handle_id, vsq_id_field[vsq_group],
                                                       DBAL_CONDITION_EQUAL_TO, (uint32) vsq_group_idx, 0));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_current_vsq_occupancy_info_get
                        (unit, entry_handle_id, vsq_group, &vsq_occupancy_info));
        DIAG_DNX_VSQ_INFO_TITLE_PRINT(" Current Occupancy");

        PRT_COLUMN_ADD("Resource");

        if (vsq_group <= DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG)
        {
            PRT_COLUMN_ADD("Total");
        }
        else if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
        {
            for (pool_idx = 0; pool_idx < DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF; pool_idx++)
            {
                PRT_COLUMN_ADD("Guaranteed Pool %d", pool_idx);
                PRT_COLUMN_ADD("Shared Pool %d", pool_idx);
                PRT_COLUMN_ADD("Headroom Pool %d", pool_idx);
            }
        }
        else
        {
            /**DNX_INGRESS_CONGESTION_VSQ_GROUP_PG*/
            PRT_COLUMN_ADD("Guaranteed");
            PRT_COLUMN_ADD("Shared");
            PRT_COLUMN_ADD("Headroom");
            PRT_COLUMN_ADD("Headroom Extension");
        }

        for (resource = 0; resource < DNX_INGRESS_CONGESTION_RESOURCE_NOF; resource++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", sh_dnx_tm_ingress_congestion_resource_str(resource));

            if (vsq_group <= DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG)
            {
                PRT_CELL_SET("%u", vsq_occupancy_info.resources[resource].total);
            }
            else
            {
                /** For BOTH GROUP E and GROUP F */
                for (pool_idx = 0; pool_idx < nof_pools; pool_idx++)
                {
                    PRT_CELL_SET("%u", vsq_occupancy_info.resources[resource].guaranteed[pool_idx]);
                    PRT_CELL_SET("%u", vsq_occupancy_info.resources[resource].shared[pool_idx]);
                    PRT_CELL_SET("%u", vsq_occupancy_info.resources[resource].headroom[pool_idx]);
                }

                /** Only GROUP F */
                if (vsq_group == DNX_INGRESS_CONGESTION_VSQ_GROUP_PG)
                {
                    PRT_CELL_SET("%u", vsq_occupancy_info.resources[resource].headroom_ex);
                }
            }
        }
        PRT_COMMITX;

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print global info about VSQ.
 */
static shr_error_e
sh_dnx_tm_vsq_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_ingress_congestion_vsq_group_e vsq_group;
    int core_id, core, vsq_group_idx;
    bcm_gport_t vsq_gport, core_all_gport, core_specific_gport;

    SHR_FUNC_INIT_VARS(unit);

    /** get parameters */
    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_ENUM("group", vsq_group);
    SH_SAND_GET_INT32("queue", vsq_group_idx);

    /** Check if user did't requested more queues stats than the
     *  VSQ group has */
    if (vsq_group_idx >= dnx_data_ingr_congestion.vsq.info_get(unit, vsq_group)->nof)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VSQ group %s has only %d queues. Please give smaller number for 'id' input! \n",
                     sh_sand_enum_str(dnx_vsq_group_enum_table, vsq_group), vsq_group_idx);
    }

    /** Greate VSQ gport */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_gport_get(unit, BCM_CORE_ALL, vsq_group, vsq_group_idx,
                                                         &core_all_gport));

    DNXCMN_CORES_ITER(unit, core_id, core)
    {

        cli_out("\n");
        cli_out("***********************\n");
        cli_out("***      Core %d     ***\n", core);
        cli_out("***********************\n");

        /** Greate VSQ gport */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_gport_get(unit, core, vsq_group, vsq_group_idx,
                                                             &core_specific_gport));

        vsq_gport = (vsq_group >= DNX_INGRESS_CONGESTION_VSQ_GROUP_STTSTCS_TAG ? core_specific_gport : core_all_gport);

        /** VSQ general info Print   */
        SHR_IF_ERR_EXIT(sh_dnx_tm_vsq_general_info_print(unit, core, vsq_group, vsq_group_idx, sand_control));

        /** Only for Source Based VSQs */
        if (vsq_group >= DNX_INGRESS_CONGESTION_VSQ_GROUP_SRC_PORT)
        {
            /**VSQ Resource Allocation info*/
            SHR_IF_ERR_EXIT(sh_dnx_tm_source_vsq_rate_class_resource_allocation_info_print
                            (unit, core, vsq_gport, vsq_group, vsq_group_idx, sand_control));
        }
        else
        {
            /*
             * Only for Destination based Based VSQs 
             */
            SHR_IF_ERR_EXIT(sh_dnx_tm_dest_vsq_rate_class_drop_info_print
                            (unit, core, vsq_gport, vsq_group, vsq_group_idx, sand_control));
        }

        /** VSQ WRED info print */
        SHR_IF_ERR_EXIT(sh_dnx_tm_dest_vsq_rate_class_wred_info_print
                        (unit, core, vsq_gport, vsq_group, vsq_group_idx, sand_control));

        /** VSQ FC info print */
        SHR_IF_ERR_EXIT(sh_dnx_tm_vsq_rate_class_fc_info_print
                        (unit, core, vsq_gport, vsq_group, vsq_group_idx, sand_control));

        /** VSQ occupancy info print */
        SHR_IF_ERR_EXIT(sh_dnx_tm_vsq_occupancy_info_print(unit, core, vsq_group, vsq_group_idx, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/**
 * \brief DNX TM Ingress VSQ diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 * This is the entry point for TM Ingress VSQ diagnostic commands 
 */
sh_sand_cmd_t sh_dnx_tm_vsq_cmds[] = {
/* *INDENT-OFF* */
    /*keyword,          action,                         command,    options,                            man  */
    {"resources",       sh_dnx_tm_resource_alloc_cmd,   NULL,       sh_dnx_tm_resource_alloc_options,   &sh_dnx_tm_resource_alloc_man},
    {"count",           sh_dnx_tm_vsq_count_cmd,        NULL,       sh_dnx_tm_vsq_count_options,        &sh_dnx_tm_vsq_count_man},
    {"info",            sh_dnx_tm_vsq_info_cmd,         NULL,       sh_dnx_tm_vsq_info_options,         &sh_dnx_tm_vsq_info_man},
    {"non_empty",       sh_dnx_tm_vsq_non_empty_cmd,    NULL,       sh_dnx_tm_vsq_non_empty_options,    &sh_dnx_tm_vsq_non_empty_man},
    {NULL}
/* *INDENT-ON* */
};

/**
 * \brief MAN for DNX TM Ingress VSQ diagnostics
 */
sh_sand_man_t sh_dnx_tm_vsq_man = {
    .brief = "VSQ commands",
    .full = NULL,
};
