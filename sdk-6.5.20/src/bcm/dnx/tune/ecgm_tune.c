/** \file ecgm_tune.c
*
* Tuning functionality for egress congestion module.
*
* This section uses bcm API functions to set the default values
* for the ECGM module.
* {
*/

/*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
* Include files.
* {
*/
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx_dispatch.h>
#include "ecgm_tune.h"
#include <bcm_int/dnx/tune/ecgm_tune.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cosq/cosq.h>      /* for NOF_TC & NOF_DP */
#include <bcm/cosq.h>   /* for bcm_cosq_gport_egress_multicast_config_set */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>      /* ecgm dnx data */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tune.h>      /* tune dnx data */
#include <shared/utilex/utilex_integer_arithmetic.h>
/*
* }
* Include files.
*/

/*
* Typedefs.
* {
*/

/* Jericho 2 ecgm resources */
#define DNX_ECGM_TUNE_CORE_TOTAL_PD(unit) dnx_data_ecgm.core_resources.total_pds_get(unit)
#define DNX_ECGM_TUNE_CORE_TOTAL_DB(unit) dnx_data_ecgm.core_resources.total_dbs_get(unit)

/* Default total resources for unicast and multicast */
#define DNX_ECGM_TUNE_CORE_UNICAST_PD(unit) (dnx_data_ecgm.core_resources.total_dbs_get(unit)/2)
#define DNX_ECGM_TUNE_CORE_UNICAST_DB(unit) (dnx_data_ecgm.core_resources.total_dbs_get(unit)/2)
#define DNX_ECGM_TUNE_CORE_MULTICAST_PD(unit) (DNX_ECGM_TUNE_CORE_TOTAL_PD(unit) - DNX_ECGM_TUNE_CORE_UNICAST_PD(unit))
#define DNX_ECGM_TUNE_CORE_MULTICAST_DB(unit) (DNX_ECGM_TUNE_CORE_TOTAL_DB(unit) - DNX_ECGM_TUNE_CORE_UNICAST_DB(unit))

/* Interface priority */
#define DNX_ECGM_TUNE_INTERFACE_PRIORITY_LOW (0)
#define DNX_ECGM_TUNE_INTERFACE_PRIORITY_HIGH (1)

 /*
  * \brief
  * core thresholds: global, multicast and unicast
  */
typedef struct
{
    int pd_drop;
    int pd_fc;
    int db_drop;
    int db_fc;
} dnx_ecgm_tune_core_global_thrsholds_t;

/*
 * \brief
 * core service pool multicast tc thresholds.
*/
typedef struct
{
    int pd_drop[DNX_COSQ_NOF_TC];
    int db_drop_max[DNX_COSQ_NOF_TC];
    int db_drop_min[DNX_COSQ_NOF_TC];
    int db_drop_alpha_0[DNX_COSQ_NOF_TC];
    int db_drop_alpha_1[DNX_COSQ_NOF_TC];
    int reserved_db[DNX_COSQ_NOF_TC];
} dnx_ecgm_tune_core_sp_tc_thresholds_t;

/*
* \brief
* all interface thresholds - per high/low priority.
*/
typedef struct
{
    int uc_pd_fc_max[2];
    int uc_pd_fc_min[2];
    int uc_pd_fc_alpha[2];
    int uc_db_fc_max[2];
    int uc_db_fc_min[2];
    int uc_db_fc_alpha[2];
    int mc_pd_drop[2];
} dnx_ecgm_tune_interface_thresolds;

/*
* \brief
* unicast thresholds - for port / queue.
*/
typedef struct
{
    int pd_drop;
    int db_drop;

    int pd_fc_max;
    int pd_fc_min;
    int pd_fc_alpha;

    int db_fc_max;
    int db_fc_min;
    int db_fc_alpha;
} dnx_ecgm_tune_port_unicast_thresolds;

/*
* \brief
* multicast port thresholds.
*/
typedef struct
{
    int db_drop;

    int pd_drop_max;
    int pd_drop_min;
    int pd_drop_alpha;
} dnx_ecgm_tune_port_multicast_thresolds;

/*
* \brief
* multicast queue thresholds.
*/
typedef struct
{
    int db_drop[DNX_COSQ_NOF_DP];
    int pd_drop_max[DNX_COSQ_NOF_DP];
    int pd_drop_min[DNX_COSQ_NOF_DP];
    int pd_drop_alpha[DNX_COSQ_NOF_DP];
    int pd_reserved;
} dnx_ecgm_tune_queue_multicast_thresolds;

/*
* }
* Typedefs.
*/

/*
* Static function declarations.
* {
*/
/*
* setting default multicast COS params mappings:
* each combination of system tc and system dp will be mapped to an MC-TC of the same value as the system tc.
* MC-TC 0-3 will be mapped to DB SP0
* MC-TC 4-7 will be mapped to DB SP1
* all with positive shared resource eligibility.
*/
static shr_error_e dnx_ecgm_tune_unit_mc_cos_params_mappings_set(
    int unit);
/*
* setting default values per core: pd and db drop and flow control:
* globally (at core level), for unicast, multicast, for sp and per sp tc
*/
static shr_error_e dnx_ecgm_tune_unit_core_global_thresholds_set(
    int unit);
/*
* setting default values per interface priority
* 1. unicast flow control thresholds for pd and db (may be set as fadt - not used in default tuning).
* 2. multicast pd drop threshold.
*/
static shr_error_e dnx_ecgm_tune_port_interface_thresholds_set(
    int unit,
    bcm_gport_t gport);
/*
* setting default values per unicast port:
* 1. pd and db drop values.
* 2. pd and db flow control values (set as FADT).
*/
static shr_error_e dnx_ecgm_tune_unicast_port_thresholds_set(
    int unit,
    bcm_gport_t gport,
    int default_rate,
    int commit_last);
/*
* setting default values per unicast queue:
* 1. pd and db drop values.
* 2. pd and db flow control values (set as FADT).
*/
static shr_error_e dnx_ecgm_tune_unicast_queue_thresholds_set(
    int unit,
    bcm_gport_t gport,
    int default_rate,
    int num_priorities,
    int commit_last);
/*
* setting default values per multicast port:
* default port resources are defined as the total amount of
* multicast resources divided by the maximum number of ports
* for the given port rate.
*/
static shr_error_e dnx_ecgm_tune_multicast_port_thresholds_set(
    int unit,
    bcm_gport_t gport,
    int default_rate,
    int commit_last);
/*
* setting default values per multicast queue:
* 1. PD and DB drop values per DP (calculated as a ratio from the total MC resources)
*      NOTE: PD drop is set as static threshold (may also be set as FADT).
* 2. setting the reserved PD for the queue.
*     reserves per queue are calculated as a ratio of the estimated number of port PDs.
*/
static shr_error_e dnx_ecgm_tune_multicast_queue_thresholds_set(
    int unit,
    bcm_gport_t gport,
    int default_rate,
    int num_priorities,
    int commit_last);

/**
 * \brief
 * map each multicast queue to PD service pool
 */
static shr_error_e dnx_ecgm_tune_multicast_port_map_queue_to_pd_sp_set(
    int unit,
    bcm_gport_t gport,
    int num_priorities);
/*
* }
* Static function declarations.
*/

/*
* Tuning Functions - for brief see .h file
* {
*/
shr_error_e
dnx_ecgm_tune_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * map multicast COS parameters:
     * default mapping:
     * 1. sets all system TC and system DB as eligible to use SP resources.
     * 2. maps MC-TC equal to system TC.
     * 3. maps MC-TC 0-3 to DB SP0 and MC-TC 4-7 to DB SP1.
     */
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_unit_mc_cos_params_mappings_set(unit));

    /*
     * set default core thresholds
     */
    /*
     * global drop / flow control thresholds per core
     * are set by default to the maximal value - the total
     * number of resources for the core.
     */
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_unit_core_global_thresholds_set(unit));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_ecgm_tune_single_port(
    int unit,
    bcm_port_t local_port,
    int port_rate)
{
    int num_priorities;
    bcm_gport_t gport;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, local_port, &num_priorities));

    /*
     * service pools should be mapped to MC-TCs and MC queues before
     * configuring reserved resources thresholds
     */
    BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(gport, local_port);
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_multicast_port_map_queue_to_pd_sp_set(unit, gport, num_priorities));

    /** set unicast thresholds */
    BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(gport, local_port);
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_unicast_port_thresholds_set(unit, gport, port_rate, FALSE));
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_unicast_queue_thresholds_set(unit, gport, port_rate, num_priorities, FALSE));

    /** set multicast thresholds and multicast queues PD SP default mapping */
    BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(gport, local_port);
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_multicast_port_thresholds_set(unit, gport, port_rate, FALSE));
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_multicast_queue_thresholds_set(unit, gport, port_rate, num_priorities, TRUE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_tune_port_init(
    int unit,
    bcm_port_t port,
    int port_rate)
{
    bcm_gport_t gport;
    bcm_gport_t interface_gport;
    bcm_cosq_gport_info_t gport_info;
    bcm_pbmp_t logical_ports_bitmap_on_interface;
    bcm_port_t port_iter;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * set interface defaults
     */
    gport_info.in_gport = port;
    gport_info.cosq = 0;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPort, &gport_info));
    gport = gport_info.out_gport;
    SHR_IF_ERR_EXIT(bcm_dnx_fabric_port_get(unit, gport, 0, &interface_gport));
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_port_interface_thresholds_set(unit, interface_gport));

    /** set ports and queues ecgm thresholds to all ports on interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_get
                    (unit, port, DNX_ALGO_PORT_CHANNELS_F_NON_L1_ONLY, &logical_ports_bitmap_on_interface));
    /** iterate on all ports of the interface */
    _SHR_PBMP_ITER(logical_ports_bitmap_on_interface, port_iter)
    {
        /** set port and queue ecgm thresholds for port */
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_single_port(unit, port_iter, port_rate));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
* }
* Tuning functions.
*/

/*
* Static Functions:
* {
*/
/* helper function to set core global pd drop threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_pd_drop_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core global pd flow control threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_pd_fc_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core global db drop threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_db_drop_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core global db flow control threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_db_fc_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}

/* helper function to set core unicast pd drop threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_uc_pd_drop_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_UNICAST | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core unicast pd flow control threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_uc_pd_fc_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_UNICAST | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core unicast db drop threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_uc_db_drop_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_UNICAST | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core unicast db flow control threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_uc_db_fc_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_UNICAST | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}

/* helper function to set core multicast pd drop threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_mc_pd_drop_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core multicast pd flow control threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_mc_pd_fc_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core multicast db drop threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_mc_db_drop_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core multicast db flow control threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_mc_db_fc_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}

/* helper function to set core service pool 0 pd drop threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_mc_sp_0_pd_drop_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP0 | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core service pool 1 pd drop threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_mc_sp_1_pd_drop_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP1 | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core service pool 0 pd flow control threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_mc_sp_0_pd_fc_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP0 | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core service pool 1 pd flow control threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_mc_sp_1_pd_fc_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP1 | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core service pool 0 db drop threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_mc_sp_0_db_drop_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP0 | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core service pool 1 db drop threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_mc_sp_1_db_drop_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP1 | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core service pool 0 db flow control threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_mc_sp_0_db_fc_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP0 | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core service pool 1 db flow control threshold */
static shr_error_e
dnx_ecgm_tune_threshold_core_global_mc_sp_1_db_fc_set(
    int unit,
    bcm_gport_t gport,
    int value)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP1 | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core multicast pd flow control value per tc */
static shr_error_e
dnx_ecgm_tune_threshold_core_tc_mc_pd_fc_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core multicast db flow control value per tc */
static shr_error_e
dnx_ecgm_tune_threshold_core_tc_mc_db_fc_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core multicast service pool 0 pd drop value per tc */
static shr_error_e
dnx_ecgm_tune_threshold_core_tc_mc_sp0_pd_drop_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP0 | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core multicast service pool 1 pd drop value per tc */
static shr_error_e
dnx_ecgm_tune_threshold_core_tc_mc_sp1_pd_drop_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP1 | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core multicast service pool 0 db drop value per tc */
static shr_error_e
dnx_ecgm_tune_threshold_core_tc_mc_sp0_db_drop_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP0 | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core multicast service pool 1 db drop value per tc */
static shr_error_e
dnx_ecgm_tune_threshold_core_tc_mc_sp1_db_drop_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP1 | BCM_COSQ_THRESHOLD_DROP;
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core multicast service pool 0 db reserved per tc */
static shr_error_e
dnx_ecgm_tune_threshold_core_tc_mc_sp0_db_reserved_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP0;
    threshold.type = bcmCosqThresholdAvailableDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set core multicast service pool 1 db reserved per tc */
static shr_error_e
dnx_ecgm_tune_threshold_core_tc_mc_sp1_db_reserved_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST_SP1;
    threshold.type = bcmCosqThresholdAvailableDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set interface unicast pd flow control value */
static shr_error_e
dnx_ecgm_tune_threshold_interface_uc_pd_fc_set(
    int unit,
    bcm_gport_t gport,
    int priority,
    int value,
    int commit)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.priority = priority;
    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_UNICAST | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set interface unicast db flow control value */
static shr_error_e
dnx_ecgm_tune_threshold_interface_uc_db_fc_set(
    int unit,
    bcm_gport_t gport,
    int priority,
    int value,
    int commit)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.priority = priority;
    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_UNICAST | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set interface multicast pd drop value */
static shr_error_e
dnx_ecgm_tune_threshold_interface_mc_pd_drop_set(
    int unit,
    bcm_gport_t gport,
    int priority,
    int value,
    int commit)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.priority = priority;
    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST | BCM_COSQ_THRESHOLD_DROP;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}

/* helper function to set pd drop value for a unicast port */
static shr_error_e
dnx_ecgm_tune_threshold_uc_port_pd_drop_set(
    int unit,
    bcm_gport_t gport,
    int value,
    int commit)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_DROP;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set pd flow control min value for a unicast port */
static shr_error_e
dnx_ecgm_tune_threshold_uc_port_pd_min_fc_set(
    int unit,
    bcm_gport_t gport,
    int value,
    int commit)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdPacketDescriptorsMin;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set pd flow control max value for a unicast port */
static shr_error_e
dnx_ecgm_tune_threshold_uc_port_pd_max_fc_set(
    int unit,
    bcm_gport_t gport,
    int value,
    int commit)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdPacketDescriptorsMax;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set pd flow control alpha value for a unicast port */
static shr_error_e
dnx_ecgm_tune_threshold_uc_port_pd_alpha_fc_set(
    int unit,
    bcm_gport_t gport,
    int value,
    int commit)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdPacketDescriptorsAlpha;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set db drop value for a unicast port */
static shr_error_e
dnx_ecgm_tune_threshold_uc_port_db_drop_set(
    int unit,
    bcm_gport_t gport,
    int value,
    int commit)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_DROP;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set db flow control min value for a unicast port */
static shr_error_e
dnx_ecgm_tune_threshold_uc_port_db_min_fc_set(
    int unit,
    bcm_gport_t gport,
    int value,
    int commit)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdDataBuffersMin;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set db flow control max value for a unicast port */
static shr_error_e
dnx_ecgm_tune_threshold_uc_port_db_max_fc_set(
    int unit,
    bcm_gport_t gport,
    int value,
    int commit)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdDataBuffersMax;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set db flow control alpha value for a unicast port */
static shr_error_e
dnx_ecgm_tune_threshold_uc_port_db_alpha_fc_set(
    int unit,
    bcm_gport_t gport,
    int value,
    int commit)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdDataBuffersAlpha;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set pd drop value for a unicast queue */
static shr_error_e
dnx_ecgm_tune_threshold_uc_queue_pd_drop_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value,
    int commit)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_DROP;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set pd flow control min value for a unicast queue */
static shr_error_e
dnx_ecgm_tune_threshold_uc_queue_pd_min_fc_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value,
    int commit)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdPacketDescriptorsMin;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set pd flow control max value for a unicast queue */
static shr_error_e
dnx_ecgm_tune_threshold_uc_queue_pd_max_fc_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value,
    int commit)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdPacketDescriptorsMax;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set pd flow control alpha value for a unicast queue */
static shr_error_e
dnx_ecgm_tune_threshold_uc_queue_pd_alpha_fc_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value,
    int commit)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdPacketDescriptorsAlpha;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set db drop value for a unicast queue */
static shr_error_e
dnx_ecgm_tune_threshold_uc_queue_db_drop_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value,
    int commit)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_DROP;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set db flow control min value for a unicast queue */
static shr_error_e
dnx_ecgm_tune_threshold_uc_queue_db_min_fc_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value,
    int commit)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdDataBuffersMin;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set db flow control max value for a unicast queue */
static shr_error_e
dnx_ecgm_tune_threshold_uc_queue_db_max_fc_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value,
    int commit)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdDataBuffersMax;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set db flow control alpha value for a unicast queue */
static shr_error_e
dnx_ecgm_tune_threshold_uc_queue_db_alpha_fc_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value,
    int commit)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_FLOW_CONTROL;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdDataBuffersAlpha;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set pd drop value for multicast port */
static shr_error_e
dnx_ecgm_tune_threshold_mc_port_pd_drop_set(
    int unit,
    bcm_gport_t gport,
    int value,
    int commit)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_DROP;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set db drop value for multicast port */
static shr_error_e
dnx_ecgm_tune_threshold_mc_port_db_drop_set(
    int unit,
    bcm_gport_t gport,
    int value,
    int commit)
{
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_DROP;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set pd drop value for multicast queue */
static shr_error_e
dnx_ecgm_tune_threshold_mc_queue_pd_drop_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int dp,
    int value,
    int commit)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_DROP | BCM_COSQ_THRESHOLD_PER_DP;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.dp = dp;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set db drop value for multicast queue */
static shr_error_e
dnx_ecgm_tune_threshold_mc_queue_db_drop_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int dp,
    int value,
    int commit)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_DROP | BCM_COSQ_THRESHOLD_PER_DP;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.dp = dp;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}
/* helper function to set pd reserved value for service pool 0 for multicast queue */
static shr_error_e
dnx_ecgm_tune_threshold_mc_queue_sp_pd_drop_reserved_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int value,
    int commit)
{
    bcm_cosq_threshold_t threshold;
    SHR_FUNC_INIT_VARS(unit);

    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_DROP;
    if (!commit)
    {
        threshold.flags |= BCM_COSQ_THRESHOLD_NOT_COMMIT;
    }
    threshold.type = bcmCosqThresholdAvailablePacketDescriptors;
    threshold.value = value;
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_threshold_set(unit, gport, cosq, &threshold));
exit:
    SHR_FUNC_EXIT;
}

/* helper function to get the JR2 defined maximum ports per default port rate */
static shr_error_e
dnx_ecgm_tune_max_ports_per_rate_get(
    int unit,
    int rate,
    int *max_ports)
{
    SHR_FUNC_INIT_VARS(unit);

    *max_ports = UTILEX_DIV_ROUND_UP(dnx_data_ecgm.core_resources.max_core_bandwidth_Mbps_get(unit), rate);
    *max_ports = UTILEX_MIN((*max_ports), dnx_data_ecgm.port_resources.max_nof_ports_get(unit));

/** exit:*/
    SHR_FUNC_EXIT;
}

/* \see declaration for brief */
static shr_error_e
dnx_ecgm_tune_unit_mc_cos_params_mappings_set(
    int unit)
{
    int sys_dp, sys_tc, tcs_to_pool_0;
    bcm_gport_t gport;
    bcm_cosq_egress_multicast_config_t config;
    SHR_FUNC_INIT_VARS(unit);

    BCM_COSQ_GPORT_CORE_SET(gport, BCM_CORE_ALL);
    tcs_to_pool_0 = DNX_COSQ_NOF_TC / 2;
    config.unscheduled_se = 1;  /* shared resource eligibility */

    config.unscheduled_sp = bcmCosqEgressMulticastServicePool0; /* DB pool */
    for (sys_tc = 0; sys_tc < tcs_to_pool_0; sys_tc++)
    {
        config.priority = sys_tc;       /* MC-TC */
        for (sys_dp = 0; sys_dp < DNX_COSQ_NOF_DP; sys_dp++)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_egress_multicast_config_set
                            (unit, gport, sys_tc, sys_dp, BCM_COSQ_MULTICAST_UNSCHEDULED, &config));
        }
    }

    config.unscheduled_sp = bcmCosqEgressMulticastServicePool1; /* DB pool */
    for (sys_tc = tcs_to_pool_0; sys_tc < DNX_COSQ_NOF_TC; sys_tc++)
    {
        config.priority = sys_tc;       /* MC-TC */
        for (sys_dp = 0; sys_dp < DNX_COSQ_NOF_DP; sys_dp++)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_egress_multicast_config_set
                            (unit, gport, sys_tc, sys_dp, BCM_COSQ_MULTICAST_UNSCHEDULED, &config));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/* \see declaration for brief */
static shr_error_e
dnx_ecgm_tune_unit_core_global_thresholds_set(
    int unit)
{
    int mc_tc;
    bcm_core_t core;
    bcm_gport_t gport;
    int mc_tc_pd_fc[DNX_COSQ_NOF_TC];
    int mc_tc_db_fc[DNX_COSQ_NOF_TC];

    dnx_ecgm_tune_core_global_thrsholds_t core_global;
    dnx_ecgm_tune_core_global_thrsholds_t core_unicast;
    dnx_ecgm_tune_core_global_thrsholds_t core_multicast;
    dnx_ecgm_tune_core_global_thrsholds_t core_sp;

    dnx_ecgm_tune_core_sp_tc_thresholds_t sp_thresholds;

    /*
     * percentage is calculated by: (100 / 8) * (8 - tc) / 100
     */
    int sp_resource_percentage[DNX_COSQ_NOF_TC] = { 100, 88, 75, 63, 50, 38, 25, 13 };

    SHR_FUNC_INIT_VARS(unit);

    core_global.pd_drop = DNX_ECGM_TUNE_CORE_TOTAL_PD(unit);
    core_global.pd_fc = (DNX_ECGM_TUNE_CORE_TOTAL_PD(unit) *
                         dnx_data_tune.ecgm.core_flow_control_percentage_ratio_get(unit) / 100);
    core_global.db_drop = DNX_ECGM_TUNE_CORE_TOTAL_DB(unit);
    core_global.db_fc = (DNX_ECGM_TUNE_CORE_TOTAL_DB(unit) *
                         dnx_data_tune.ecgm.core_flow_control_percentage_ratio_get(unit) / 100);

    core_unicast.pd_drop = DNX_ECGM_TUNE_CORE_UNICAST_PD(unit);
    core_unicast.pd_fc = (DNX_ECGM_TUNE_CORE_UNICAST_PD(unit) *
                          dnx_data_tune.ecgm.core_flow_control_percentage_ratio_get(unit) / 100);
    core_unicast.db_drop = DNX_ECGM_TUNE_CORE_UNICAST_DB(unit);
    core_unicast.db_fc = (DNX_ECGM_TUNE_CORE_UNICAST_DB(unit) *
                          dnx_data_tune.ecgm.core_flow_control_percentage_ratio_get(unit) / 100);

    core_multicast.pd_drop = DNX_ECGM_TUNE_CORE_MULTICAST_PD(unit);
    core_multicast.pd_fc = (DNX_ECGM_TUNE_CORE_MULTICAST_PD(unit) *
                            dnx_data_tune.ecgm.core_flow_control_percentage_ratio_get(unit) / 100);
    core_multicast.db_drop = DNX_ECGM_TUNE_CORE_MULTICAST_DB(unit);
    core_multicast.db_fc = (DNX_ECGM_TUNE_CORE_MULTICAST_DB(unit) *
                            dnx_data_tune.ecgm.core_flow_control_percentage_ratio_get(unit) / 100);

    core_sp.pd_drop = core_multicast.pd_drop / 2;
    core_sp.pd_fc = core_multicast.pd_fc / 2;
    core_sp.db_drop = core_multicast.db_drop / 2;
    core_sp.db_fc = core_multicast.db_fc / 2;

    for (mc_tc = 0; mc_tc < DNX_COSQ_NOF_TC; ++mc_tc)
    {
        sp_thresholds.pd_drop[mc_tc] = (core_sp.pd_drop * sp_resource_percentage[mc_tc] / 100);
        sp_thresholds.db_drop_max[mc_tc] = (core_sp.db_drop * sp_resource_percentage[mc_tc] / 100);
        sp_thresholds.db_drop_min[mc_tc] = sp_thresholds.db_drop_max[mc_tc];
        sp_thresholds.db_drop_alpha_0[mc_tc] = 0;
        sp_thresholds.db_drop_alpha_1[mc_tc] = 0;
        sp_thresholds.reserved_db[mc_tc] = 300;

        mc_tc_pd_fc[mc_tc] = (core_sp.pd_fc * sp_resource_percentage[mc_tc] / 100);
        mc_tc_db_fc[mc_tc] = (core_sp.db_fc * sp_resource_percentage[mc_tc] / 100);
    }

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        BCM_COSQ_GPORT_CORE_SET(gport, core);

        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_pd_fc_set(unit, gport, core_global.pd_fc));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_db_fc_set(unit, gport, core_global.db_fc));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_uc_pd_fc_set(unit, gport, core_unicast.pd_fc));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_uc_db_fc_set(unit, gport, core_unicast.db_fc));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_mc_pd_fc_set(unit, gport, core_multicast.pd_fc));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_mc_db_fc_set(unit, gport, core_multicast.db_fc));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_mc_sp_0_pd_fc_set(unit, gport, core_sp.pd_fc));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_mc_sp_1_pd_fc_set(unit, gport, core_sp.pd_fc));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_mc_sp_0_db_fc_set(unit, gport, core_sp.db_fc));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_mc_sp_1_db_fc_set(unit, gport, core_sp.db_fc));

        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_pd_drop_set(unit, gport, core_global.pd_drop));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_db_drop_set(unit, gport, core_global.db_drop));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_uc_pd_drop_set(unit, gport, core_unicast.pd_drop));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_uc_db_drop_set(unit, gport, core_unicast.db_drop));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_mc_pd_drop_set(unit, gport, core_multicast.pd_drop));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_mc_db_drop_set(unit, gport, core_multicast.db_drop));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_mc_sp_0_pd_drop_set(unit, gport, core_sp.pd_drop));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_mc_sp_1_pd_drop_set(unit, gport, core_sp.pd_drop));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_mc_sp_0_db_drop_set(unit, gport, core_sp.db_drop));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_global_mc_sp_1_db_drop_set(unit, gport, core_sp.db_drop));

        /*
         * DB threshold configuration
         * Setting Service pool thresholds per TC - for MC-TC 0-3.
         * MC-TC 0-3 are mapped by default to SP0 - Therefore, the calculated values from 'sp.thresholds',
         * will be set only for SP0. SP1 thresholds will be set with Zeros.
         */
        for (mc_tc = 0; mc_tc < 4; ++mc_tc)
        {
            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_tc_mc_sp0_db_drop_set
                            (unit, gport, mc_tc, sp_thresholds.db_drop_max[mc_tc]));

            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_tc_mc_sp1_db_drop_set(unit, gport, mc_tc, 0));

            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_tc_mc_sp0_db_reserved_set
                            (unit, gport, mc_tc, sp_thresholds.reserved_db[mc_tc]));
            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_tc_mc_sp1_db_reserved_set(unit, gport, mc_tc, 0));
        }

        /*
         * DB threshold configuration
         * Setting Service pool thresholds per TC - for MC-TC 4-7.
         * MC-TC 4-7 are mapped by default to SP1 - Therefore, the calculated values from 'sp.thresholds',
         * will be set only for SP1. SP0 thresholds will be set with Zeros.
         */
        for (mc_tc = 4; mc_tc < 8; ++mc_tc)
        {
            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_tc_mc_sp0_db_drop_set(unit, gport, mc_tc, 0));

            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_tc_mc_sp1_db_drop_set
                            (unit, gport, mc_tc, sp_thresholds.db_drop_max[mc_tc]));

            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_tc_mc_sp0_db_reserved_set(unit, gport, mc_tc, 0));
            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_tc_mc_sp1_db_reserved_set
                            (unit, gport, mc_tc, sp_thresholds.reserved_db[mc_tc]));
        }

        for (mc_tc = 0; mc_tc < DNX_COSQ_NOF_TC; ++mc_tc)
        {
            /*
             * for PD thresholds sp0 and sp1 need to be configured the same
             * This is due to the fact that the SP is chosen by queue and therefore the same TC can hit different SP thresholds
             */
            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_tc_mc_sp0_pd_drop_set
                            (unit, gport, mc_tc, sp_thresholds.pd_drop[mc_tc]));
            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_tc_mc_sp1_pd_drop_set
                            (unit, gport, mc_tc, sp_thresholds.pd_drop[mc_tc]));
            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_tc_mc_pd_fc_set(unit, gport, mc_tc, mc_tc_pd_fc[mc_tc]));
            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_core_tc_mc_db_fc_set(unit, gport, mc_tc, mc_tc_db_fc[mc_tc]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/* \see declaration for brief */
static shr_error_e
dnx_ecgm_tune_port_interface_thresholds_set(
    int unit,
    bcm_gport_t gport)
{
    int priority;
    dnx_ecgm_tune_interface_thresolds interface_thresholds;
    SHR_FUNC_INIT_VARS(unit);

    interface_thresholds.uc_pd_fc_max[DNX_ECGM_TUNE_INTERFACE_PRIORITY_LOW] = (DNX_ECGM_TUNE_CORE_UNICAST_PD(unit) *
                                                                               dnx_data_tune.ecgm.
                                                                               core_flow_control_percentage_ratio_get
                                                                               (unit) / 100);
    interface_thresholds.uc_db_fc_max[DNX_ECGM_TUNE_INTERFACE_PRIORITY_LOW] =
        (DNX_ECGM_TUNE_CORE_UNICAST_DB(unit) * dnx_data_tune.ecgm.core_flow_control_percentage_ratio_get(unit) / 100);
    interface_thresholds.mc_pd_drop[DNX_ECGM_TUNE_INTERFACE_PRIORITY_LOW] = DNX_ECGM_TUNE_CORE_MULTICAST_PD(unit);

    interface_thresholds.uc_pd_fc_max[DNX_ECGM_TUNE_INTERFACE_PRIORITY_HIGH] = (DNX_ECGM_TUNE_CORE_UNICAST_PD(unit) *
                                                                                dnx_data_tune.ecgm.
                                                                                core_flow_control_percentage_ratio_get
                                                                                (unit) / 100);
    interface_thresholds.uc_db_fc_max[DNX_ECGM_TUNE_INTERFACE_PRIORITY_HIGH] =
        (DNX_ECGM_TUNE_CORE_UNICAST_DB(unit) * dnx_data_tune.ecgm.core_flow_control_percentage_ratio_get(unit) / 100);
    interface_thresholds.mc_pd_drop[DNX_ECGM_TUNE_INTERFACE_PRIORITY_HIGH] = DNX_ECGM_TUNE_CORE_MULTICAST_PD(unit);

    priority = BCM_COSQ_LOW_PRIORITY;
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_interface_uc_pd_fc_set
                    (unit, gport, priority, interface_thresholds.uc_pd_fc_max[DNX_ECGM_TUNE_INTERFACE_PRIORITY_LOW],
                     FALSE));
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_interface_uc_db_fc_set
                    (unit, gport, priority, interface_thresholds.uc_db_fc_max[DNX_ECGM_TUNE_INTERFACE_PRIORITY_LOW],
                     FALSE));
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_interface_mc_pd_drop_set
                    (unit, gport, priority, interface_thresholds.mc_pd_drop[DNX_ECGM_TUNE_INTERFACE_PRIORITY_LOW],
                     FALSE));

    priority = BCM_COSQ_HIGH_PRIORITY;
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_interface_uc_pd_fc_set
                    (unit, gport, priority, interface_thresholds.uc_pd_fc_max[DNX_ECGM_TUNE_INTERFACE_PRIORITY_HIGH],
                     FALSE));
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_interface_uc_db_fc_set
                    (unit, gport, priority, interface_thresholds.uc_db_fc_max[DNX_ECGM_TUNE_INTERFACE_PRIORITY_HIGH],
                     FALSE));
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_interface_mc_pd_drop_set
                    (unit, gport, priority, interface_thresholds.mc_pd_drop[DNX_ECGM_TUNE_INTERFACE_PRIORITY_HIGH],
                     TRUE));

exit:
    SHR_FUNC_EXIT;
}
/* \see declaration for brief */
static shr_error_e
dnx_ecgm_tune_unicast_port_thresholds_set(
    int unit,
    bcm_gport_t gport,
    int default_rate,
    int commit_last)
{
    static int fadt_max_multiplier = 8;

    int max_num_of_ports_per_rate;
    dnx_ecgm_tune_port_unicast_thresolds port_uc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_tune_max_ports_per_rate_get(unit, default_rate, &max_num_of_ports_per_rate));

    port_uc.pd_drop = DNX_ECGM_TUNE_CORE_UNICAST_PD(unit);
    port_uc.db_drop = DNX_ECGM_TUNE_CORE_UNICAST_PD(unit);

    port_uc.pd_fc_max = DNX_ECGM_TUNE_CORE_UNICAST_PD(unit) / max_num_of_ports_per_rate * fadt_max_multiplier;
    port_uc.pd_fc_max = (port_uc.pd_fc_max > DNX_ECGM_TUNE_CORE_UNICAST_PD(unit)) ?
        DNX_ECGM_TUNE_CORE_UNICAST_PD(unit) : port_uc.pd_fc_max;
    port_uc.pd_fc_min = DNX_ECGM_TUNE_CORE_UNICAST_PD(unit) / max_num_of_ports_per_rate /
        dnx_data_tune.ecgm.port_uc_flow_control_min_divider_get(unit);
    port_uc.pd_fc_alpha = 0;

    port_uc.db_fc_max = DNX_ECGM_TUNE_CORE_UNICAST_DB(unit) / max_num_of_ports_per_rate * fadt_max_multiplier;
    port_uc.db_fc_max = (port_uc.db_fc_max > DNX_ECGM_TUNE_CORE_UNICAST_DB(unit)) ?
        DNX_ECGM_TUNE_CORE_UNICAST_DB(unit) : port_uc.db_fc_max;
    port_uc.db_fc_min = DNX_ECGM_TUNE_CORE_UNICAST_DB(unit) / max_num_of_ports_per_rate /
        dnx_data_tune.ecgm.port_uc_flow_control_min_divider_get(unit);
    port_uc.db_fc_alpha = 0;

    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_port_pd_drop_set(unit, gport, port_uc.pd_drop, FALSE));
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_port_db_drop_set(unit, gport, port_uc.db_drop, FALSE));

    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_port_pd_max_fc_set(unit, gport, port_uc.pd_fc_max, FALSE));
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_port_pd_min_fc_set(unit, gport, port_uc.pd_fc_min, FALSE));
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_port_pd_alpha_fc_set(unit, gport, port_uc.pd_fc_alpha, FALSE));

    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_port_db_max_fc_set(unit, gport, port_uc.db_fc_max, FALSE));
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_port_db_min_fc_set(unit, gport, port_uc.db_fc_min, FALSE));
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_port_db_alpha_fc_set(unit, gport, port_uc.db_fc_alpha, commit_last));

exit:
    SHR_FUNC_EXIT;
}
/* \see declaration for brief */
static shr_error_e
dnx_ecgm_tune_unicast_queue_thresholds_set(
    int unit,
    bcm_gport_t gport,
    int default_rate,
    int num_priorities,
    int commit_last)
{
    int cosq;
    static int fadt_max_multiplier = 4;

    int max_num_of_ports_per_rate;
    dnx_ecgm_tune_port_unicast_thresolds queue_uc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_tune_max_ports_per_rate_get(unit, default_rate, &max_num_of_ports_per_rate));

    queue_uc.pd_drop = DNX_ECGM_TUNE_CORE_UNICAST_PD(unit);
    queue_uc.db_drop = DNX_ECGM_TUNE_CORE_UNICAST_PD(unit);

    queue_uc.pd_fc_max = DNX_ECGM_TUNE_CORE_UNICAST_PD(unit) / max_num_of_ports_per_rate * fadt_max_multiplier;
    queue_uc.pd_fc_min = DNX_ECGM_TUNE_CORE_UNICAST_PD(unit) / max_num_of_ports_per_rate /
        dnx_data_tune.ecgm.port_uc_flow_control_min_divider_get(unit);
    queue_uc.pd_fc_alpha = -1;

    queue_uc.db_fc_max = DNX_ECGM_TUNE_CORE_UNICAST_DB(unit) / max_num_of_ports_per_rate * fadt_max_multiplier;
    queue_uc.db_fc_min = DNX_ECGM_TUNE_CORE_UNICAST_DB(unit) / max_num_of_ports_per_rate /
        dnx_data_tune.ecgm.port_uc_flow_control_min_divider_get(unit);
    queue_uc.db_fc_alpha = -1;

    for (cosq = 0; cosq < num_priorities; ++cosq)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_queue_pd_drop_set(unit, gport, cosq, queue_uc.pd_drop, FALSE));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_queue_db_drop_set(unit, gport, cosq, queue_uc.db_drop, FALSE));

        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_queue_pd_max_fc_set(unit, gport, cosq, queue_uc.pd_fc_max, FALSE));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_queue_pd_min_fc_set(unit, gport, cosq, queue_uc.pd_fc_min, FALSE));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_queue_pd_alpha_fc_set(unit, gport, cosq, queue_uc.pd_fc_alpha,
                                                                         FALSE));

        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_queue_db_max_fc_set(unit, gport, cosq, queue_uc.db_fc_max, FALSE));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_queue_db_min_fc_set(unit, gport, cosq, queue_uc.db_fc_min, FALSE));
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_uc_queue_db_alpha_fc_set(unit, gport, cosq, queue_uc.db_fc_alpha,
                                                                         (cosq == (num_priorities - 1) ?
                                                                          commit_last : FALSE)));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * map multicast queue to PD service pool
 */
static shr_error_e
dnx_ecgm_tune_multicast_port_map_queue_to_pd_sp_set(
    int unit,
    bcm_gport_t gport,
    int num_priorities)
{
    int sp, cosq;

    SHR_FUNC_INIT_VARS(unit);

    for (cosq = 0; cosq < num_priorities; ++cosq)
    {
        sp = (cosq < num_priorities / 2) ? 0 : 1;
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_set(unit, gport, cosq, bcmCosqControlMulticastQueueToPdSpMap, sp));
    }

exit:
    SHR_FUNC_EXIT;
}

/* \see declaration for brief */
static shr_error_e
dnx_ecgm_tune_multicast_port_thresholds_set(
    int unit,
    bcm_gport_t gport,
    int default_rate,
    int commit_last)
{
    int max_num_of_ports_per_rate;
    dnx_ecgm_tune_port_multicast_thresolds port_mc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_tune_max_ports_per_rate_get(unit, default_rate, &max_num_of_ports_per_rate));

    port_mc.db_drop = DNX_ECGM_TUNE_CORE_MULTICAST_DB(unit) / max_num_of_ports_per_rate;
    port_mc.pd_drop_max = DNX_ECGM_TUNE_CORE_MULTICAST_PD(unit) / max_num_of_ports_per_rate;

    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_mc_port_db_drop_set(unit, gport, port_mc.db_drop, FALSE));
    SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_mc_port_pd_drop_set(unit, gport, port_mc.pd_drop_max, commit_last));

exit:
    SHR_FUNC_EXIT;
}
/* \see declaration for brief */
static shr_error_e
dnx_ecgm_tune_multicast_queue_thresholds_set(
    int unit,
    bcm_gport_t gport,
    int default_rate,
    int num_priorities,
    int commit_last)
{
    int cosq, dp;
    static int port_reserved_pd_factor_1p = 5;
    static int port_reserved_pd_factor_2p = 10;
    static int port_reserved_pd_factor_8p = 20;
    int port_reserved_pd_factor;
    int max_num_of_ports_per_rate;
    int port_pd_reserved;
    dnx_ecgm_tune_queue_multicast_thresolds queue_mc;
    SHR_FUNC_INIT_VARS(unit);

    port_reserved_pd_factor = (num_priorities == 8 ? port_reserved_pd_factor_8p :
                               (num_priorities >= 2 ? port_reserved_pd_factor_2p : port_reserved_pd_factor_1p));

    SHR_IF_ERR_EXIT(dnx_ecgm_tune_max_ports_per_rate_get(unit, default_rate, &max_num_of_ports_per_rate));
    port_pd_reserved = DNX_ECGM_TUNE_CORE_MULTICAST_PD(unit) /
        max_num_of_ports_per_rate * port_reserved_pd_factor / 100;

    /*
     * calculating the threshold values
     */
    for (dp = 0; dp < DNX_COSQ_NOF_DP; ++dp)
    {
        queue_mc.db_drop[dp] = DNX_ECGM_TUNE_CORE_MULTICAST_DB(unit) /
            max_num_of_ports_per_rate * dnx_data_tune.ecgm.port_dp_ratio_get(unit, dp)->percentage_ratio / 100;
        queue_mc.pd_drop_max[dp] = DNX_ECGM_TUNE_CORE_MULTICAST_PD(unit) /
            max_num_of_ports_per_rate * dnx_data_tune.ecgm.port_dp_ratio_get(unit, dp)->percentage_ratio / 100;
    }
    queue_mc.pd_reserved = port_pd_reserved / num_priorities;

    /*
     * setting the queues with the calculated values per DP.
     * mapping the queues to a PD service pool (half of the queues to sp 0 and half to sp 1)
     * setting the PD reserved threshold per queue
     */
    for (cosq = 0; cosq < num_priorities; ++cosq)
    {
        for (dp = 0; dp < DNX_COSQ_NOF_DP; ++dp)
        {
            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_mc_queue_pd_drop_set
                            (unit, gport, cosq, dp, queue_mc.pd_drop_max[dp], FALSE));
            SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_mc_queue_db_drop_set(unit, gport, cosq, dp,
                                                                         queue_mc.db_drop[dp], FALSE));
        }

        SHR_IF_ERR_EXIT(dnx_ecgm_tune_threshold_mc_queue_sp_pd_drop_reserved_set
                        (unit, gport, cosq, queue_mc.pd_reserved,
                         (cosq == (num_priorities - 1) ? commit_last : FALSE)));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
* }
* Static functions.
*/

/*
* }
*/
