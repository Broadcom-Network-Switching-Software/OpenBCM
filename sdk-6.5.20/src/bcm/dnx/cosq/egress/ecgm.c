/** \file ecgm.c
* 
* Egress congestion functionality for DNX.
*
* this file contains the implementation of
* ECGM internal API functions.
*/

/*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/**
* Included files.
* {
*/

#include <bcm/types.h>  /* uint32 */
#include <shared/shrextend/shrextend_debug.h>   /* debug defines */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>      /* ecgm dnx data */
#include <bcm_int/dnx/algo/ecgm/algo_ecgm.h>    /* ecgm port & interface profiles */
#include <bcm_int/dnx/tune/ecgm_tune.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>       /* port api */
#include <bcm_int/dnx/cosq/cosq.h>      /* typedefs */
#include <bcm_int/dnx/cmn/dnxcmn.h>     /* core iteration */
#include <bcm_int/dnx/cosq/egress/ecgm.h>       /* h file for this module */
#include "ecgm_dbal.h"  /* internal module dbal functions */
#include "ecgm_profile_mgmt.h"  /* internal profile management */
#include "ecgm_thresholds_internal.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_ecgm_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ecgm_access.h>

/**
* }
*/

/**
* Static functions declarations
* {
*/
static shr_error_e dnx_ecgm_resources_init(
    int unit,
    int core);

/* verify functions for external API */
static shr_error_e dnx_ecgm_core_multicast_config_verify(
    int unit,
    bcm_cos_t system_tc,
    bcm_color_t system_dp,
    bcm_cosq_egress_multicast_config_t * config);

static shr_error_e dnx_ecgm_gport_threshold_get_verify(
    int unit,
    bcm_cosq_threshold_t * threshold);

/**
* delete fifo
* fill structs with dnx data default values and
* write this configuration to hw
*/
static void dnx_ecgm_delete_fifo_default_thresholds_fill(
    int unit,
    ecgm_delete_fifo_thresholds_t * short_thresholds,
    ecgm_delete_fifo_thresholds_t * long_lcd_thresholds);

static shr_error_e dnx_ecgm_delete_fifo_config(
    int unit,
    bcm_core_t core_id);

/**
* Thresholds management
*/

/*
* \brief - supporting function for API dnx_ecgm_gport_threshold
*          gets the flags and type for a certain threshold and returns
*          one of dnx_ecgm_threshold_tag_t tags into tag.
*          in case the tag is SP related - will fill sp parameter with the sp id.
*          else - sp will be set to -1.
*          if the type + flags combination is not supported - will return _SHR_E_PARAM.
*/
static shr_error_e dnx_ecgm_threshold_tag_get(
    int unit,
    dnx_ecgm_threshold_data_t * data);

/* 
 * core threshold management:
 * calling core threshold function
 * according to given cosq.
 */
static shr_error_e dnx_ecgm_threshold_core_threshold_set(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);
static shr_error_e dnx_ecgm_threshold_core_threshold_get(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);

/* 
 * core global thresholds:
 * global drop/flow control values for PD and DB
 * per core, unicast, multicast, and service pool.
 */
static shr_error_e dnx_ecgm_threshold_core_no_cosq_set(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);
static shr_error_e dnx_ecgm_threshold_core_no_cosq_get(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);

/* 
 * core MC-TC thresholds:
 * drop/flow control values per MC-TC,
 * in multicast level or service pool level.
 */
static shr_error_e dnx_ecgm_threshold_core_w_cosq_set(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);
static shr_error_e dnx_ecgm_threshold_core_w_cosq_get(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);

/**
 * \brief - setting amount of DB reserves per TC in a given service pool.
 *          verifing function will fail if the value is not between 0-core max DBs,
 *          and will print error if the value is higher
 *          than the sp drop value for that TC.
 */
static shr_error_e dnx_ecgm_core_sp_tc_db_reserved_set(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);
static shr_error_e dnx_ecgm_core_sp_tc_db_reserved_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);

/* 
 * core flow control thresholds setting - verify :
 * for each of the following verify functions, 
 * validating legal PD/DB range
 * and prints warning if the equivalent drop threshold is lower
 * than the new given flow control value. 
 * other warnings may be printed according to threshold type,
 * for more information - consult the user manual.
 */
static shr_error_e dnx_ecgm_core_global_pd_fc_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_global_db_fc_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_uc_pd_fc_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_uc_db_fc_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_mc_pd_fc_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_mc_db_fc_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_mc_tc_pd_fc_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_mc_tc_db_fc_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_sp_pd_fc_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);
static shr_error_e dnx_ecgm_core_sp_db_fc_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);

/* 
 * core drop thresholds setting - verify :
 * for each of the following verify functions, 
 * validating legal PD/DB range
 * and prints warning if the equivalent flow control threshold is higher
 * than the new given drop value.
 * other warnings may be printed according to threshold type,
 * for more information - consult the user manual.
 */
static shr_error_e dnx_ecgm_core_global_pd_drop_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_global_db_drop_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_uc_pd_drop_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_uc_db_drop_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_mc_pd_drop_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_mc_db_drop_verify(
    int unit,
    bcm_core_t core,
    int value);
static shr_error_e dnx_ecgm_core_sp_pd_drop_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);
static shr_error_e dnx_ecgm_core_sp_db_drop_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);
static shr_error_e dnx_ecgm_core_sp_tc_pd_drop_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);
static shr_error_e dnx_ecgm_core_sp_tc_db_drop_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);
static shr_error_e dnx_ecgm_core_sp_tc_db_min_drop_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data);

/**
* }
*/

/**
* Module Functions
* \see descriptions in .h file
* {
*/

shr_error_e
dnx_ecgm_init(
    int unit)
{
    int core_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init the sw state ECGM.
     */
    SHR_IF_ERR_EXIT(algo_ecgm_db.init(unit));
    SHR_IF_ERR_EXIT(dnx_ecgm_db.init(unit));
    /*
     * create template managers for port profile and interface profile
     */
    SHR_IF_ERR_EXIT(dnx_algo_ecgm_port_profile_template_create(unit));
    SHR_IF_ERR_EXIT(dnx_algo_ecgm_interface_profile_template_create(unit));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_dynamic_memory_access_set(unit, TRUE));
    /*
     * setting HW values for each core:
     *      configure delete fifo defaults
     *      configure operating mode for service pools TC thresholds
     *      enable multicast queue to PD service pool mapping
     *      initialize threshold registers
     */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_index)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_delete_fifo_config(unit, core_index));
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_multicast_queue_to_sp_enable_set(unit, core_index, 1));
        SHR_IF_ERR_EXIT(dnx_ecgm_resources_init(unit, core_index));
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_tc_sp_threshold_mode_set
                        (unit, core_index, ECGM_TC_SP_THRESHOLD_MODE_STRICT_PRIORITY,
                         ECGM_TC_SP_THRESHOLD_MODE_STRICT_PRIORITY));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * destroy port & interface profile template managers
     */
    SHR_IF_ERR_CONT(dnx_algo_ecgm_port_profile_template_destroy(unit));
    SHR_IF_ERR_CONT(dnx_algo_ecgm_interface_profile_template_destroy(unit));

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_port_add(
    int unit)
{
    bcm_port_t logical_port;
    bcm_port_t master_port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, 0, &master_port));
    if (logical_port != master_port)
    {
        int has_tx_speed = 0;
        dnx_algo_port_info_s port_info;
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, master_port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
        if (has_tx_speed && DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            int tx_speed;
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, master_port, DNX_ALGO_PORT_SPEED_F_TX, &tx_speed));
            SHR_IF_ERR_EXIT(dnx_ecgm_tune_single_port(unit, logical_port, tx_speed));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_port_remove(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_info_s port_info;
    dnx_ecgm_profile_mgmt_port_handle_t port_profile_handle;
    dnx_ecgm_profile_mgmt_interface_handle_t interface_profile_handle;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));

    /*
     * remove port for TM ports only (ports with base queue pair)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_handle_get(unit, port, &port_profile_handle));
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_interface_handle_get(unit, port, &interface_profile_handle));

        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_handle_destroy(unit, &port_profile_handle));
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_interface_handle_destroy(unit, &interface_profile_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_map_mc_queue_to_pd_sp_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t queue_offset,
    int pd_service_pool)
{
    bcm_port_t port;
    dnx_ecgm_profile_mgmt_port_handle_t port_profile_handle;

    SHR_FUNC_INIT_VARS(unit);

    port = BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_GET(gport);
    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_handle_get(unit, port, &port_profile_handle));
    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_pd_sp_set(unit, &port_profile_handle, queue_offset, pd_service_pool));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_map_mc_queue_to_pd_sp_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t queue_offset,
    int *pd_service_pool)
{
    bcm_port_t port;
    dnx_ecgm_profile_mgmt_port_handle_t port_profile_handle;
    SHR_FUNC_INIT_VARS(unit);

    port = BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_GET(gport);
    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_handle_get(unit, port, &port_profile_handle));

    /*
     * get port profile data
     */
    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_pd_sp_get(unit, &port_profile_handle, queue_offset, pd_service_pool));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_core_multicast_config_set(
    int unit,
    bcm_cos_t system_tc,
    bcm_color_t system_dp,
    bcm_cosq_egress_multicast_config_t * config)
{
    bcm_core_t core_iterator;
    ecgm_cos_params_t cos_params;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Validate input params
     */
    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_multicast_config_verify(unit, system_tc, system_dp, config));

    cos_params.mc_tc = config->priority;        /* multicast TC */
    cos_params.se_dp = system_dp;       /* shared resource eligibility (per dp) */
    cos_params.mc_sp = config->unscheduled_sp;  /* db service pool */

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_iterator)
    {
        /*
         * map system TC and system DP to MC TC
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_sys_tc_dp_to_mc_cos_params_set
                        (unit, core_iterator, system_tc, system_dp, &cos_params));

        /*
         * map system DP and DB SP to eligibility to use resources
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_mc_se_set
                        (unit, core_iterator, system_dp, cos_params.mc_sp, (uint32) config->unscheduled_se));
    }

    /*
     * map MC TC to a data buffer service pool
     */
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_mc_tc_to_db_sp_set(unit, cos_params.mc_tc, cos_params.mc_sp));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_core_multicast_config_get(
    int unit,
    bcm_cos_t system_tc,
    bcm_color_t system_dp,
    bcm_cosq_egress_multicast_config_t * config)
{
    bcm_core_t core = 0;        /* dbal functions require core - but info is same for both cores */
    ecgm_cos_params_t cos_params;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Validate input params
     */
    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_multicast_config_verify(unit, system_tc, system_dp, config));

    /*
     * getting cos_params
     */
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_sys_tc_dp_to_mc_cos_params_get(unit, core, system_tc, system_dp, &cos_params));

    /*
     * getting eligibility to use resources
     */
    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_dbal_mc_se_get
                          (unit, core, system_dp, cos_params.mc_sp, (uint32 *) &config->unscheduled_se));

    config->priority = cos_params.mc_tc;
    config->unscheduled_sp = cos_params.mc_sp;  /* db service pool */

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_gport_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    bcm_core_t core;
    bcm_port_t port;
    dnx_ecgm_threshold_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    data.cosq = cosq;
    data.threshold = threshold;
    SHR_IF_ERR_EXIT(dnx_ecgm_threshold_tag_get(unit, &data));

    if (BCM_COSQ_GPORT_IS_CORE(gport) || gport == 0)
    {
        core = (gport == 0) ? BCM_CORE_ALL : BCM_COSQ_GPORT_CORE_GET(gport);
        SHR_IF_ERR_EXIT(dnx_ecgm_threshold_core_threshold_set(unit, core, &data));
    }

    else if (BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport))
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_multicast_threshold_set
                        (unit, BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_GET(gport), &data));
    }

    else if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport))
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_unicast_threshold_set
                        (unit, BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_GET(gport), &data));
    }

    else if (BCM_GPORT_IS_LOCAL_INTERFACE(gport))
    {
        SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &port));
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_interface_threshold_set(unit, port, &data));
    }

    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "error: gport not supported.");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_gport_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    bcm_core_t core;
    bcm_port_t port;
    dnx_ecgm_threshold_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_gport_threshold_get_verify(unit, threshold));

    data.cosq = cosq;
    data.threshold = threshold;
    SHR_IF_ERR_EXIT(dnx_ecgm_threshold_tag_get(unit, &data));

    if (BCM_COSQ_GPORT_IS_CORE(gport) || gport == 0)
    {
        core = (gport == 0) ? 0 : BCM_COSQ_GPORT_CORE_GET(gport);
        SHR_IF_ERR_EXIT(dnx_ecgm_threshold_core_threshold_get(unit, core, &data));
    }

    else if (BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport))
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_multicast_threshold_get
                        (unit, BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_GET(gport), &data));
    }
    else if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport))
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_unicast_threshold_get
                        (unit, BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_GET(gport), &data));
    }

    else if (BCM_GPORT_IS_LOCAL_INTERFACE(gport))
    {
        SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &port));
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_interface_threshold_get(unit, port, &data));
    }

    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "error: gport not supported.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* static functions definitions
* {
*/
static shr_error_e
dnx_ecgm_resources_init(
    int unit,
    int core)
{
    int sp;
    uint32 core_total_pds = dnx_data_ecgm.core_resources.total_pds_get(unit);
    uint32 core_total_dbs = dnx_data_ecgm.core_resources.total_dbs_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_global_threshold_set(unit, core, DBAL_FIELD_PD_DROP, core_total_pds));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_global_threshold_set(unit, core, DBAL_FIELD_PD_FLOW_CONTROL, core_total_pds));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_global_threshold_set(unit, core, DBAL_FIELD_DB_DROP, core_total_dbs));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_global_threshold_set(unit, core, DBAL_FIELD_DB_FLOW_CONTROL, core_total_dbs));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_unicast_threshold_set(unit, core, DBAL_FIELD_PD_DROP, core_total_pds));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_unicast_threshold_set(unit, core, DBAL_FIELD_PD_FLOW_CONTROL, core_total_pds));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_unicast_threshold_set(unit, core, DBAL_FIELD_DB_DROP, core_total_dbs));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_unicast_threshold_set(unit, core, DBAL_FIELD_DB_FLOW_CONTROL, core_total_dbs));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_threshold_set(unit, core, DBAL_FIELD_PD_DROP, core_total_pds));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_threshold_set(unit, core, DBAL_FIELD_PD_FLOW_CONTROL, core_total_pds));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_threshold_set(unit, core, DBAL_FIELD_DB_DROP, core_total_dbs));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_threshold_set(unit, core, DBAL_FIELD_DB_FLOW_CONTROL, core_total_dbs));

    for (sp = 0; sp < dnx_data_ecgm.core_resources.nof_sp_get(unit); ++sp)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_set
                        (unit, core, sp, DBAL_FIELD_PD_DROP, core_total_pds));
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_set
                        (unit, core, sp, DBAL_FIELD_PD_FLOW_CONTROL, core_total_pds));
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_set
                        (unit, core, sp, DBAL_FIELD_DB_DROP, core_total_dbs));
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_set
                        (unit, core, sp, DBAL_FIELD_DB_FLOW_CONTROL, core_total_dbs));
        /*
         * resetting the total reserves for both service pools 
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_set(unit, core, sp, DBAL_FIELD_RESERVED_PD, 0));
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_set(unit, core, sp, DBAL_FIELD_RESERVED_DB, 0));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_core_multicast_config_verify(
    int unit,
    bcm_cos_t system_tc,
    bcm_color_t system_dp,
    bcm_cosq_egress_multicast_config_t * config)
{
    uint32 old_sp;
    int reserved_dbs;
    bcm_core_t core_iterator;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    if (system_tc < 0 || system_tc >= DNX_COSQ_NOF_TC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: traffic class %d", system_tc);
    }

    if (system_dp < 0 || system_dp >= DNX_COSQ_NOF_DP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: drop precedence %d", system_dp);
    }

    if (config->priority < 0 || config->priority > DNX_COSQ_NOF_TC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: priority %d", config->priority);
    }

    if (config->unscheduled_se != 0 && config->unscheduled_se != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: unscheduled_se %d", config->unscheduled_se);
    }

    if (config->unscheduled_sp != bcmCosqEgressMulticastServicePool0 &&
        config->unscheduled_sp != bcmCosqEgressMulticastServicePool1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: unscheduled_sp %d", config->unscheduled_sp);
    }

    /*
     * if changing the SP - verify that the MC-TC reserved value (for each core) is Zero.
     */
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_mc_tc_to_db_sp_get(unit, config->priority, &old_sp));

    if (config->unscheduled_sp != old_sp)
    {
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_iterator)
        {
            /*
             * get the mc tc db reserved 
             */
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_get
                            (unit, core_iterator, config->priority, old_sp, DBAL_FIELD_RESERVED_DB, &reserved_dbs));

            if (reserved_dbs != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid parameter: reserved DBs for MC-TC %d in SP %d is %d. Can't change MC-TC to SP mapping when reserved value is not 0.",
                             config->priority, old_sp, reserved_dbs);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static void
dnx_ecgm_delete_fifo_default_thresholds_fill(
    int unit,
    ecgm_delete_fifo_thresholds_t * short_thresholds,
    ecgm_delete_fifo_thresholds_t * long_lcd_thresholds)
{
    if (short_thresholds)
    {
        short_thresholds->fqp_high_priority =
            dnx_data_ecgm.delete_fifo.thresholds_get(unit, ECGM_TDM_AND_SHORT_PACKETS)->fqp_high_priority;
        short_thresholds->stop_mc_low =
            dnx_data_ecgm.delete_fifo.thresholds_get(unit, ECGM_TDM_AND_SHORT_PACKETS)->stop_mc_low;
        short_thresholds->stop_mc_high =
            dnx_data_ecgm.delete_fifo.thresholds_get(unit, ECGM_TDM_AND_SHORT_PACKETS)->stop_mc_high;
        short_thresholds->stop_all =
            dnx_data_ecgm.delete_fifo.thresholds_get(unit, ECGM_TDM_AND_SHORT_PACKETS)->stop_all;
    }

    /*
     * the defaults for long packets and lcd fifo are identicle for JR2 - the data is taken only once
     */
    if (long_lcd_thresholds)
    {
        long_lcd_thresholds->fqp_high_priority =
            dnx_data_ecgm.delete_fifo.thresholds_get(unit, ECGM_LONG_PACKETS_FIFO)->fqp_high_priority;
        long_lcd_thresholds->stop_mc_low =
            dnx_data_ecgm.delete_fifo.thresholds_get(unit, ECGM_LONG_PACKETS_FIFO)->stop_mc_low;
        long_lcd_thresholds->stop_mc_high =
            dnx_data_ecgm.delete_fifo.thresholds_get(unit, ECGM_LONG_PACKETS_FIFO)->stop_mc_high;
        long_lcd_thresholds->stop_all =
            dnx_data_ecgm.delete_fifo.thresholds_get(unit, ECGM_LONG_PACKETS_FIFO)->stop_all;
    }
}

static shr_error_e
dnx_ecgm_delete_fifo_config(
    int unit,
    bcm_core_t core_id)
{
    ecgm_delete_fifo_thresholds_t short_thresholds, long_lcd_thresholds;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get the default values from dnx data
     */
    dnx_ecgm_delete_fifo_default_thresholds_fill(unit, &short_thresholds, &long_lcd_thresholds);

    /*
     * set both cores to short & long packet (tdm is not supported)
     */
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_delete_fifo_mode_set(unit, core_id, ECGM_SHORT_AND_LONG_PACKETS));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_delete_fifo_thresholds_set
                    (unit, core_id, ECGM_TDM_AND_SHORT_PACKETS, &short_thresholds));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_delete_fifo_thresholds_set
                    (unit, core_id, ECGM_LONG_PACKETS_FIFO, &long_lcd_thresholds));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_delete_fifo_thresholds_set(unit, core_id, ECGM_LCD_FIFO, &long_lcd_thresholds));

exit:
    SHR_FUNC_EXIT;
}

/* Thresholds management */
static shr_error_e
dnx_ecgm_threshold_tag_get(
    int unit,
    dnx_ecgm_threshold_data_t * data)
{
    uint32 flags = data->threshold->flags & ~BCM_COSQ_THRESHOLD_NOT_COMMIT;
    SHR_FUNC_INIT_VARS(unit);

    data->sp = -1;
    flags -= BCM_COSQ_THRESHOLD_EGRESS;

    if (flags & BCM_COSQ_THRESHOLD_DROP)
    {
        flags -= BCM_COSQ_THRESHOLD_DROP;
        if (flags == BCM_COSQ_THRESHOLD_PER_DP)
        {
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_dp_verify(unit, data->threshold->dp));

            switch (data->threshold->type)
            {
                case (bcmCosqThresholdPacketDescriptors):
                    data->tag = PD_DROP_PER_DP;
                    break;
                case (bcmCosqThresholdPacketDescriptorsMin):
                    data->tag = PD_MIN_DROP_PER_DP;
                    break;
                case (bcmCosqThresholdPacketDescriptorsMax):
                    data->tag = PD_MAX_DROP_PER_DP;
                    break;
                case (bcmCosqThresholdPacketDescriptorsAlpha):
                    data->tag = PD_ALPHA_DROP_PER_DP;
                    break;
                case (bcmCosqThresholdDataBuffers):
                    data->tag = DB_DROP_PER_DP;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "flags %d and type %d are not a valid threshold", flags,
                                 data->threshold->type);
            }
        }

        else if (flags == 0)
        {
            switch (data->threshold->type)
            {
                case (bcmCosqThresholdPacketDescriptors):
                    data->tag = PD_DROP;
                    break;
                case (bcmCosqThresholdPacketDescriptorsMin):
                    data->tag = PD_MIN_DROP;
                    break;
                case (bcmCosqThresholdPacketDescriptorsMax):
                    data->tag = PD_MAX_DROP;
                    break;
                case (bcmCosqThresholdPacketDescriptorsAlpha):
                    data->tag = PD_ALPHA_DROP;
                    break;
                case (bcmCosqThresholdDataBuffers):
                    data->tag = DB_DROP;
                    break;
                case (bcmCosqThresholdAvailablePacketDescriptors):
                    data->tag = SP_PD_RESERVED;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "flags %d and type %d are not a valid threshold", flags,
                                 data->threshold->type);
            }
        }
        else if (flags == BCM_COSQ_THRESHOLD_UNICAST)
        {
            switch (data->threshold->type)
            {
                case (bcmCosqThresholdPacketDescriptors):
                    data->tag = UC_PD_DROP;
                    break;
                case (bcmCosqThresholdDataBuffers):
                    data->tag = UC_DB_DROP;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "flags %d and type %d are not a valid threshold", flags,
                                 data->threshold->type);
            }
        }
        else if (flags == BCM_COSQ_THRESHOLD_MULTICAST)
        {
            switch (data->threshold->type)
            {
                case (bcmCosqThresholdPacketDescriptors):
                    data->tag = MC_PD_DROP;
                    break;
                case (bcmCosqThresholdDataBuffers):
                    data->tag = MC_DB_DROP;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "flags %d and type %d are not a valid threshold", flags,
                                 data->threshold->type);
            }
        }
        else if (flags == BCM_COSQ_THRESHOLD_MULTICAST_SP0 || flags == BCM_COSQ_THRESHOLD_MULTICAST_SP1)
        {
            switch (data->threshold->type)
            {
                case (bcmCosqThresholdPacketDescriptors):
                    data->tag = SP_PD_DROP;
                    break;
                case (bcmCosqThresholdDataBuffers):
                    data->tag = SP_DB_DROP;
                    break;
                case (bcmCosqThresholdDataBuffersMin):
                    data->tag = SP_DB_MIN_DROP;
                    break;
                case (bcmCosqThresholdDataBuffersMax):
                    data->tag = SP_DB_MAX_DROP;
                    break;
                case (bcmCosqThresholdDataBuffersAlpha):
                    data->tag = SP_DB_ALPHA_0_DROP;
                    break;
                case (bcmCosqThresholdDataBuffersAlpha1):
                    data->tag = SP_DB_ALPHA_1_DROP;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "flags %d and type %d are not a valid threshold", flags,
                                 data->threshold->type);
            }

            data->sp =
                (flags ==
                 BCM_COSQ_THRESHOLD_MULTICAST_SP0) ? bcmCosqEgressMulticastServicePool0 :
                bcmCosqEgressMulticastServicePool1;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid flag combination");
        }
    }

    else if (flags & BCM_COSQ_THRESHOLD_FLOW_CONTROL)
    {
        flags -= BCM_COSQ_THRESHOLD_FLOW_CONTROL;
        if (flags == 0)
        {
            switch (data->threshold->type)
            {
                case (bcmCosqThresholdPacketDescriptors):
                    data->tag = PD_FC;
                    break;
                case (bcmCosqThresholdPacketDescriptorsMin):
                    data->tag = PD_MIN_FC;
                    break;
                case (bcmCosqThresholdPacketDescriptorsMax):
                    data->tag = PD_MAX_FC;
                    break;
                case (bcmCosqThresholdPacketDescriptorsAlpha):
                    data->tag = PD_ALPHA_FC;
                    break;
                case (bcmCosqThresholdDataBuffers):
                    data->tag = DB_FC;
                    break;
                case (bcmCosqThresholdDataBuffersMin):
                    data->tag = DB_MIN_FC;
                    break;
                case (bcmCosqThresholdDataBuffersMax):
                    data->tag = DB_MAX_FC;
                    break;
                case (bcmCosqThresholdDataBuffersAlpha):
                    data->tag = DB_ALPHA_FC;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "flags %d and type %d are not a valid threshold", flags,
                                 data->threshold->type);
            }
        }
        else if (flags == BCM_COSQ_THRESHOLD_UNICAST)
        {
            switch (data->threshold->type)
            {
                case (bcmCosqThresholdPacketDescriptors):
                    data->tag = UC_PD_FC;
                    break;
                case (bcmCosqThresholdPacketDescriptorsMin):
                    data->tag = UC_PD_MIN_FC;
                    break;
                case (bcmCosqThresholdPacketDescriptorsMax):
                    data->tag = UC_PD_MAX_FC;
                    break;
                case (bcmCosqThresholdPacketDescriptorsAlpha):
                    data->tag = UC_PD_ALPHA_FC;
                    break;
                case (bcmCosqThresholdDataBuffers):
                    data->tag = UC_DB_FC;
                    break;
                case (bcmCosqThresholdDataBuffersMin):
                    data->tag = UC_DB_MIN_FC;
                    break;
                case (bcmCosqThresholdDataBuffersMax):
                    data->tag = UC_DB_MAX_FC;
                    break;
                case (bcmCosqThresholdDataBuffersAlpha):
                    data->tag = UC_DB_ALPHA_FC;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "flags %d and type %d are not a valid threshold", flags,
                                 data->threshold->type);
            }
        }
        else if (flags == BCM_COSQ_THRESHOLD_MULTICAST)
        {
            switch (data->threshold->type)
            {
                case (bcmCosqThresholdPacketDescriptors):
                    data->tag = MC_PD_FC;
                    break;
                case (bcmCosqThresholdDataBuffers):
                    data->tag = MC_DB_FC;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "flags %d and type %d are not a valid threshold", flags,
                                 data->threshold->type);
            }
        }
        else if (flags == BCM_COSQ_THRESHOLD_MULTICAST_SP0 || flags == BCM_COSQ_THRESHOLD_MULTICAST_SP1)
        {
            switch (data->threshold->type)
            {
                case (bcmCosqThresholdPacketDescriptors):
                    data->tag = SP_PD_FC;
                    break;
                case (bcmCosqThresholdDataBuffers):
                    data->tag = SP_DB_FC;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "flags %d and type %d are not a valid threshold", flags,
                                 data->threshold->type);
            }

            data->sp =
                (flags ==
                 BCM_COSQ_THRESHOLD_MULTICAST_SP0) ? bcmCosqEgressMulticastServicePool0 :
                bcmCosqEgressMulticastServicePool1;
        }
        else
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid flag combination");
    }

    else if (flags == BCM_COSQ_THRESHOLD_MULTICAST_SP0 || flags == BCM_COSQ_THRESHOLD_MULTICAST_SP1)
    {
        if (data->threshold->type == bcmCosqThresholdAvailableDataBuffers)
        {
            data->tag = SP_DB_RESERVED;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags %d and type %d are not a valid threshold", flags, data->threshold->type);
        }

        data->sp =
            (flags ==
             BCM_COSQ_THRESHOLD_MULTICAST_SP0) ? bcmCosqEgressMulticastServicePool0 :
            bcmCosqEgressMulticastServicePool1;
    }
    else
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid flag combination");
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_gport_threshold_get_verify(
    int unit,
    bcm_cosq_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold ptr null");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_threshold_core_threshold_set(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    bcm_core_t core_index;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, core, core_index)
    {
        if (data->cosq == -1)
        {
            SHR_IF_ERR_EXIT(dnx_ecgm_threshold_core_no_cosq_set(unit, core_index, data));
        }
        else if (data->cosq >= 0 && data->cosq < DNX_COSQ_NOF_TC)
        {
            SHR_IF_ERR_EXIT(dnx_ecgm_threshold_core_w_cosq_set(unit, core_index, data));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid cosq %d", data->cosq);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_threshold_core_threshold_get(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (data->cosq == -1)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_threshold_core_no_cosq_get(unit, core, data));
    }
    else if (data->cosq >= 0 && data->cosq < DNX_COSQ_NOF_TC)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_threshold_core_w_cosq_get(unit, core, data));
    }

    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid cosq %d", data->cosq);
    }

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_threshold_core_no_cosq_set(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    int value = data->threshold->value;
    SHR_FUNC_INIT_VARS(unit);

    switch (data->tag)
    {
            /*
             * Core General Thresholds
             */
        case (PD_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_global_pd_drop_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_global_threshold_set(unit, core, DBAL_FIELD_PD_DROP, value));
            break;
        case (PD_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_global_pd_fc_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_global_threshold_set(unit, core, DBAL_FIELD_PD_FLOW_CONTROL, value));
            break;
        case (DB_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_global_db_drop_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_global_threshold_set(unit, core, DBAL_FIELD_DB_DROP, value));
            break;
        case (DB_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_global_db_fc_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_global_threshold_set(unit, core, DBAL_FIELD_DB_FLOW_CONTROL, value));
            break;
            /*
             * Core Unicast Thresholds
             */
        case (UC_PD_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_uc_pd_drop_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_unicast_threshold_set(unit, core, DBAL_FIELD_PD_DROP, value));
            break;
        case (UC_PD_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_uc_pd_fc_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_unicast_threshold_set(unit, core, DBAL_FIELD_PD_FLOW_CONTROL, value));
            break;
        case (UC_DB_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_uc_db_drop_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_unicast_threshold_set(unit, core, DBAL_FIELD_DB_DROP, value));
            break;
        case (UC_DB_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_uc_db_fc_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_unicast_threshold_set(unit, core, DBAL_FIELD_DB_FLOW_CONTROL, value));
            break;
            /*
             * Core Multicast Thresholds
             */
        case (MC_PD_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_mc_pd_drop_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_threshold_set(unit, core, DBAL_FIELD_PD_DROP, value));
            break;
        case (MC_PD_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_mc_pd_fc_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_threshold_set(unit, core, DBAL_FIELD_PD_FLOW_CONTROL, value));
            break;
        case (MC_DB_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_mc_db_drop_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_threshold_set(unit, core, DBAL_FIELD_DB_DROP, value));
            break;
        case (MC_DB_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_mc_db_fc_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_threshold_set(unit, core, DBAL_FIELD_DB_FLOW_CONTROL, value));
            break;
            /*
             * Core SP Thresholds
             */
        case (SP_PD_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_sp_pd_drop_verify(unit, core, data));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_set
                            (unit, core, data->sp, DBAL_FIELD_PD_DROP, value));
            break;
        case (SP_PD_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_sp_pd_fc_verify(unit, core, data));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_set
                            (unit, core, data->sp, DBAL_FIELD_PD_FLOW_CONTROL, value));
            break;
        case (SP_DB_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_sp_db_drop_verify(unit, core, data));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_set
                            (unit, core, data->sp, DBAL_FIELD_DB_DROP, value));
            break;
        case (SP_DB_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_sp_db_fc_verify(unit, core, data));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_set
                            (unit, core, data->sp, DBAL_FIELD_DB_FLOW_CONTROL, value));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "threshold not supported for core gport");
    }
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_threshold_core_no_cosq_get(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (data->tag)
    {
        case (PD_DROP):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_global_threshold_get
                            (unit, core, DBAL_FIELD_PD_DROP, (uint32 *) &data->threshold->value));
            break;
        case (PD_FC):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_global_threshold_get
                            (unit, core, DBAL_FIELD_PD_FLOW_CONTROL, (uint32 *) &data->threshold->value));
            break;
        case (DB_DROP):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_global_threshold_get
                            (unit, core, DBAL_FIELD_DB_DROP, (uint32 *) &data->threshold->value));
            break;
        case (DB_FC):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_global_threshold_get
                            (unit, core, DBAL_FIELD_DB_FLOW_CONTROL, (uint32 *) &data->threshold->value));
            break;

        case (UC_PD_DROP):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_unicast_threshold_get
                            (unit, core, DBAL_FIELD_PD_DROP, (uint32 *) &data->threshold->value));
            break;
        case (UC_PD_FC):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_unicast_threshold_get
                            (unit, core, DBAL_FIELD_PD_FLOW_CONTROL, (uint32 *) &data->threshold->value));
            break;
        case (UC_DB_DROP):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_unicast_threshold_get
                            (unit, core, DBAL_FIELD_DB_DROP, (uint32 *) &data->threshold->value));
            break;
        case (UC_DB_FC):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_unicast_threshold_get
                            (unit, core, DBAL_FIELD_DB_FLOW_CONTROL, (uint32 *) &data->threshold->value));
            break;

        case (MC_PD_DROP):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_threshold_get
                            (unit, core, DBAL_FIELD_PD_DROP, (uint32 *) &data->threshold->value));
            break;
        case (MC_PD_FC):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_threshold_get
                            (unit, core, DBAL_FIELD_PD_FLOW_CONTROL, (uint32 *) &data->threshold->value));
            break;
        case (MC_DB_DROP):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_threshold_get
                            (unit, core, DBAL_FIELD_DB_DROP, (uint32 *) &data->threshold->value));
            break;
        case (MC_DB_FC):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_threshold_get
                            (unit, core, DBAL_FIELD_DB_FLOW_CONTROL, (uint32 *) &data->threshold->value));
            break;

        case (SP_PD_DROP):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_get
                            (unit, core, data->sp, DBAL_FIELD_PD_DROP, (uint32 *) &data->threshold->value));
            break;
        case (SP_PD_FC):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_get
                            (unit, core, data->sp, DBAL_FIELD_PD_FLOW_CONTROL, (uint32 *) &data->threshold->value));
            break;
        case (SP_DB_DROP):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_get
                            (unit, core, data->sp, DBAL_FIELD_DB_DROP, (uint32 *) &data->threshold->value));
            break;
        case (SP_DB_FC):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_get
                            (unit, core, data->sp, DBAL_FIELD_DB_FLOW_CONTROL, (uint32 *) &data->threshold->value));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "threshold not supported for core gport");
    }
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_threshold_core_w_cosq_set(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    int value = data->threshold->value;
    SHR_FUNC_INIT_VARS(unit);

    switch (data->tag)
    {
        case (MC_PD_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_mc_tc_pd_fc_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_threshold_set
                            (unit, core, data->cosq, DBAL_FIELD_PD_FLOW_CONTROL, value));
            break;
        case (MC_DB_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_mc_tc_db_fc_verify(unit, core, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_threshold_set
                            (unit, core, data->cosq, DBAL_FIELD_DB_FLOW_CONTROL, value));
            break;
        case (SP_PD_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_sp_tc_pd_drop_verify(unit, core, data));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_set
                            (unit, core, data->cosq, data->sp, DBAL_FIELD_PD_DROP, value));
            break;
        case (SP_DB_MAX_DROP):
        case (SP_DB_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_sp_tc_db_drop_verify(unit, core, data));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_set
                            (unit, core, data->cosq, data->sp, DBAL_FIELD_DB_DROP_MAX, value));
            if (data->tag == SP_DB_DROP)
            {
                SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_set
                                (unit, core, data->cosq, data->sp, DBAL_FIELD_DB_DROP_MIN, value));
                SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_set
                                (unit, core, data->cosq, data->sp, DBAL_FIELD_DB_DROP_ALPHA_0, 0));
                SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_set
                                (unit, core, data->cosq, data->sp, DBAL_FIELD_DB_DROP_ALPHA_1, 0));
            }
            break;
        case (SP_DB_MIN_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_sp_tc_db_min_drop_verify(unit, core, data));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_set
                            (unit, core, data->cosq, data->sp, DBAL_FIELD_DB_DROP_MIN, value));
            break;
        case (SP_DB_ALPHA_0_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_alpha_verify(unit, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_set
                            (unit, core, data->cosq, data->sp, DBAL_FIELD_DB_DROP_ALPHA_0, value));
            break;
        case (SP_DB_ALPHA_1_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_alpha_verify(unit, value));
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_set
                            (unit, core, data->cosq, data->sp, DBAL_FIELD_DB_DROP_ALPHA_1, value));
            break;
        case (SP_DB_RESERVED):
            SHR_IF_ERR_EXIT(dnx_ecgm_core_sp_tc_db_reserved_set(unit, core, data));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "threshold not supported for core gport");
    }

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_threshold_core_w_cosq_get(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (data->tag)
    {
        case (MC_PD_FC):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_threshold_get
                            (unit, core, data->cosq, DBAL_FIELD_PD_FLOW_CONTROL, (uint32 *) &data->threshold->value));
            break;
        case (MC_DB_FC):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_threshold_get
                            (unit, core, data->cosq, DBAL_FIELD_DB_FLOW_CONTROL, (uint32 *) &data->threshold->value));
            break;

        case (SP_PD_DROP):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_get
                            (unit, core, data->cosq, data->sp, DBAL_FIELD_PD_DROP, &data->threshold->value));
            break;

        case (SP_DB_MIN_DROP):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_get
                            (unit, core, data->cosq, data->sp, DBAL_FIELD_DB_DROP_MIN, &data->threshold->value));
            break;
        case (SP_DB_DROP):
        case (SP_DB_MAX_DROP):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_get
                            (unit, core, data->cosq, data->sp, DBAL_FIELD_DB_DROP_MAX, &data->threshold->value));
            break;
        case (SP_DB_ALPHA_0_DROP):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_get
                            (unit, core, data->cosq, data->sp, DBAL_FIELD_DB_DROP_ALPHA_0, &data->threshold->value));
            break;
        case (SP_DB_ALPHA_1_DROP):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_get
                            (unit, core, data->cosq, data->sp, DBAL_FIELD_DB_DROP_ALPHA_1, &data->threshold->value));
            break;
        case (SP_DB_RESERVED):
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_get
                            (unit, core, data->cosq, data->sp, DBAL_FIELD_RESERVED_DB, &data->threshold->value));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "threshold not supported for core gport");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_core_global_pd_drop_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_global_pd_fc_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_global_db_drop_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_global_db_fc_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_uc_pd_drop_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_uc_pd_fc_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_uc_db_drop_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_uc_db_fc_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_core_mc_pd_drop_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_mc_pd_fc_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_mc_db_drop_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_mc_db_fc_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_core_sp_pd_drop_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, data->threshold->value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_sp_pd_fc_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, data->threshold->value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_sp_db_drop_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, data->threshold->value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_sp_db_fc_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, data->threshold->value));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_core_mc_tc_pd_fc_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_mc_tc_db_fc_verify(
    int unit,
    bcm_core_t core,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_core_sp_tc_pd_drop_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, data->threshold->value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_sp_tc_db_drop_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, data->threshold->value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_sp_tc_db_min_drop_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    int max_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, data->threshold->value));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_get
                    (unit, core, data->cosq, data->sp, DBAL_FIELD_DB_DROP_MAX, &max_val));

    SHR_IF_ERR_EXIT(dnx_ecgm_min_threshold_verify(unit, max_val, data->threshold->value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_sp_tc_db_reserved_verify(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, data->threshold->value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_core_sp_tc_db_reserved_set(
    int unit,
    bcm_core_t core,
    dnx_ecgm_threshold_data_t * data)
{
    dnx_ecgm_threshold_sp_action_e action;
    int old_value, difference;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_core_sp_tc_db_reserved_verify(unit, core, data));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_get
                    (unit, core, data->cosq, data->sp, DBAL_FIELD_RESERVED_DB, &old_value));

    if (old_value == data->threshold->value)
    {
        SHR_FUNC_EXIT;
    }

    /*
     * modifying the total DB for the given SP
     */
    if (old_value > data->threshold->value)
    {
        difference = old_value - data->threshold->value;
        action = DNX_ECGM_THRESHOLD_SP_DECREASE;
    }
    else
    {
        difference = data->threshold->value - old_value;
        action = DNX_ECGM_THRESHOLD_SP_INCREASE;
    }
    SHR_IF_ERR_EXIT(dnx_ecgm_sp_reserved_db_threshold_modify(unit, core, data->sp, difference, action));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_tc_multicast_sp_threshold_set
                    (unit, core, data->cosq, data->sp, DBAL_FIELD_RESERVED_DB, data->threshold->value));

exit:
    SHR_FUNC_EXIT;
}
