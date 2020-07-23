/**
* \file src/bcm/dnx/cosq/egress/ecgm_profile_mgmt.c
* 
* Internal egress congestion resources profile management.
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

#include "ecgm_profile_mgmt.h"
#include "ecgm_dbal.h"
#include <shared/shrextend/shrextend_debug.h>   /* debug defines */
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>        /* for EGQ flush */
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>       /* port api */
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_ecgm_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ecgm_access.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/**
* }
*/

/* static functions declarations */

/* functions used for port profile management */
static shr_error_e dnx_ecgm_profile_mgmt_port_info_set(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle);

static shr_error_e dnx_ecgm_profile_mgmt_port_info_hw_set(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle);

static shr_error_e dnx_ecgm_profile_mgmt_pd_sp_set_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    bcm_cos_queue_t queue,
    int pd_sp);

static shr_error_e dnx_ecgm_profile_mgmt_pd_sp_get_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    bcm_cos_queue_t queue,
    int *pd_sp);

/*
* modify HW values for amount of reserved PDs for a service pool.
* verifing function will fail if new reserves value makes the
* total reserves for both SPs higher than core total PDs,
* and will print warning if the new total is higher than
* core multicast PD drop value.
*/

static shr_error_e dnx_ecgm_profile_mgmt_multicast_queue_sp_pd_reserved_set(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int cosq,
    int value);

static shr_error_e dnx_ecgm_profile_mgmt_sp_reserved_pd_threshold_modify(
    int unit,
    int core_id,
    int sp,
    int reserved_pd,
    dnx_ecgm_threshold_sp_action_e action);

static shr_error_e dnx_ecgm_profile_mgmt_sp_reserved_pd_threshold_modify_verify(
    int unit,
    int core,
    int sp,
    uint32 sp_reserved);

/*
* reserved PDs per service pool has to equal the sum of all reserved PDs
* for each of the queues using that SP.
* this function should be called when a port is added / removed.
* goes through all of the port's queues, and updates the reserved PD accordingly.
*/
static shr_error_e dnx_ecgm_profile_mgmt_pd_sp_reserved_for_each_queue_modify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    dnx_ecgm_threshold_sp_action_e action);

static shr_error_e dnx_ecgm_profile_mgmt_queue_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    bcm_cos_queue_t offset);

/* verifying functions for port threshold setting */
static shr_error_e dnx_ecgm_profile_mgmt_unicast_queue_pd_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int cosq,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_unicast_queue_pd_fc_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int cosq,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_unicast_queue_db_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int cosq,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_unicast_queue_db_fc_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int cosq,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_unicast_port_pd_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_unicast_port_pd_fc_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_unicast_port_db_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_unicast_port_db_fc_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_multicast_queue_pd_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_multicast_queue_db_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_multicast_queue_sp_pd_reserved_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int cosq,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_multicast_port_pd_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_multicast_port_db_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value);

/* functions used for interface profile management */
static shr_error_e dnx_ecgm_profile_mgmt_interface_info_set(
    int unit,
    dnx_ecgm_profile_mgmt_interface_handle_t * interface_profile_handle);

static shr_error_e dnx_ecgm_profile_mgmt_interface_info_hw_set(
    int unit,
    dnx_ecgm_profile_mgmt_interface_handle_t * interface_profile_handle);

/* verifying functions for interface threshold setting */
static shr_error_e dnx_ecgm_profile_mgmt_interface_threshold_verify(
    int unit,
    int cosq,
    int priority);
static shr_error_e dnx_ecgm_profile_mgmt_interface_uc_pd_fc_verify(
    int unit,
    bcm_port_t port,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_interface_uc_db_fc_verify(
    int unit,
    bcm_port_t port,
    int value);
static shr_error_e dnx_ecgm_profile_mgmt_interface_mc_pd_drop_verify(
    int unit,
    bcm_port_t port,
    int value);

shr_error_e
dnx_ecgm_profile_mgmt_port_handle_get(
    int unit,
    bcm_port_t port,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle)
{
    int valid;
    int ref_count;
    const dnx_ecgm_port_profile_info_t *port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db.port_caching.valid.get(unit, &valid));

    port_profile_handle->port = port;

    /*
     * get base q pair, tm port, core, num of priorities, profile id and info.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &port_profile_handle->base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &port_profile_handle->core, &port_profile_handle->tm_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &port_profile_handle->num_priorities));

    if (valid)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_db.port_caching.info.get(unit, &port_info));
        sal_memcpy(&port_profile_handle->info, port_info, sizeof(dnx_ecgm_port_profile_info_t));
        SHR_IF_ERR_EXIT(dnx_ecgm_db.port_caching.profile_id.get(unit, &port_profile_handle->profile_id));

    }
    else
    {

        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_tm_port_to_port_profile_get
                        (unit, port_profile_handle->core, port_profile_handle->tm_port,
                         (uint32 *) &port_profile_handle->profile_id));

        SHR_IF_ERR_EXIT(algo_ecgm_db.ecgm_port_profile.profile_data_get
                        (unit, port_profile_handle->core, port_profile_handle->profile_id, &ref_count,
                         &port_profile_handle->info));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_profile_mgmt_port_handle_set(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int sw_only)
{
    SHR_FUNC_INIT_VARS(unit);

    if (sw_only)
    {
        /*
         * store the profile info in SW state
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_db.port_caching.valid.set(unit, TRUE));
        SHR_IF_ERR_EXIT(dnx_ecgm_db.port_caching.port.set(unit, port_profile_handle->port));
        SHR_IF_ERR_EXIT(dnx_ecgm_db.port_caching.profile_id.set(unit, port_profile_handle->profile_id));
        SHR_IF_ERR_EXIT(dnx_ecgm_db.port_caching.info.set(unit, &port_profile_handle->info));

    }
    else
    {
        /*
         * set the new profile info to the port 
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_info_set(unit, port_profile_handle));

        SHR_IF_ERR_EXIT(dnx_ecgm_db.port_caching.valid.set(unit, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_profile_mgmt_port_handle_destroy(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * decreasing reserved PDs from the queues SP
     */
    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_pd_sp_reserved_for_each_queue_modify
                    (unit, port_profile_handle, DNX_ECGM_THRESHOLD_SP_DECREASE));

    sal_memset(&port_profile_handle->info, 0, sizeof(dnx_ecgm_port_profile_info_t));
    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_info_set(unit, port_profile_handle));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_profile_mgmt_pd_sp_set(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    bcm_cos_queue_t queue,
    int pd_sp)
{
    uint32 enable = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_pd_sp_set_verify(unit, port_profile_handle, queue, pd_sp));

    if (port_profile_handle->info.multicast_queue[queue].pd_sp != pd_sp)
    {
        int old_sp = port_profile_handle->info.multicast_queue[queue].pd_sp;
        port_profile_handle->info.multicast_queue[queue].pd_sp = pd_sp;

        /*
         * save port enable status 
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_get(unit, port_profile_handle->port, &enable));

        /*
         * flush EGQ and disable the port to make sure no reserved PDs from the current SP are left
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port_profile_handle->port, 0, TRUE));

        /*
         * set the port profile with new service pool id
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_handle_set(unit, port_profile_handle, FALSE));

        /*
         * if no failure: update old and new sp reserved PDs
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_sp_reserved_pd_threshold_modify
                        (unit, port_profile_handle->core, old_sp,
                         port_profile_handle->info.multicast_queue[queue].reserved_pds,
                         DNX_ECGM_THRESHOLD_SP_DECREASE));
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_sp_reserved_pd_threshold_modify
                        (unit, port_profile_handle->core, port_profile_handle->info.multicast_queue[queue].pd_sp,
                         port_profile_handle->info.multicast_queue[queue].reserved_pds,
                         DNX_ECGM_THRESHOLD_SP_INCREASE));
        /*
         * re-enable the port if it was enabled previous
         */
        if (enable)
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port_profile_handle->port, 1, FALSE));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_profile_mgmt_pd_sp_get(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    bcm_cos_queue_t queue,
    int *pd_sp)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_pd_sp_get_verify(unit, port_profile_handle, queue, pd_sp));

    *pd_sp = port_profile_handle->info.multicast_queue[queue].pd_sp;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_profile_mgmt_port_set_verify(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data)
{
    int valid;
    bcm_port_t cached_port;

    SHR_FUNC_INIT_VARS(unit);

    if (data->threshold->flags & BCM_COSQ_THRESHOLD_NOT_COMMIT)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_db.port_caching.valid.get(unit, &valid));
        if (valid)
        {
            SHR_IF_ERR_EXIT(dnx_ecgm_db.port_caching.port.get(unit, &cached_port));
            if (port != cached_port)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Configuration for port %d is not commited yet.\n", cached_port);

            }

        }
    }

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_ecgm_profile_mgmt_unicast_threshold_set(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data)
{
    int value = data->threshold->value;
    dnx_ecgm_profile_mgmt_port_handle_t port_profile_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_port_set_verify(unit, port, data));

    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_handle_get(unit, port, &port_profile_handle));

    if (data->cosq == -1)
    {
        switch (data->tag)
        {
            case (PD_DROP):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_unicast_port_pd_drop_verify
                                      (unit, &port_profile_handle, value));
                port_profile_handle.info.unicast_port.pd_drop = value;
                break;
            case (PD_MAX_FC):
            case (PD_FC):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_unicast_port_pd_fc_verify
                                      (unit, &port_profile_handle, value));
                port_profile_handle.info.unicast_port.flow_control.pd_fc.max = value;
                if (data->tag == PD_FC)
                {
                    port_profile_handle.info.unicast_port.flow_control.pd_fc.min = value;
                    port_profile_handle.info.unicast_port.flow_control.pd_fc.alpha = 0;
                }
                break;
            case (PD_MIN_FC):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_pd_threshold_verify(unit, value));
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_min_threshold_verify
                                      (unit, port_profile_handle.info.unicast_port.flow_control.pd_fc.max, value));
                port_profile_handle.info.unicast_port.flow_control.pd_fc.min = value;
                break;
            case (PD_ALPHA_FC):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_alpha_verify(unit, value));
                port_profile_handle.info.unicast_port.flow_control.pd_fc.alpha = value;
                break;
            case (DB_DROP):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_unicast_port_db_drop_verify
                                      (unit, &port_profile_handle, value));
                port_profile_handle.info.unicast_port.db_drop = value;
                break;
            case (DB_MAX_FC):
            case (DB_FC):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_unicast_port_db_fc_verify
                                      (unit, &port_profile_handle, value));
                port_profile_handle.info.unicast_port.flow_control.db_fc.max = value;
                if (data->tag == DB_FC)
                {
                    port_profile_handle.info.unicast_port.flow_control.db_fc.min = value;
                    port_profile_handle.info.unicast_port.flow_control.db_fc.alpha = 0;
                }
                break;
            case (DB_MIN_FC):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_db_threshold_verify(unit, value));
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_min_threshold_verify
                                      (unit, port_profile_handle.info.unicast_port.flow_control.db_fc.max, value));
                port_profile_handle.info.unicast_port.flow_control.db_fc.min = value;
                break;
            case (DB_ALPHA_FC):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_alpha_verify(unit, value));
                port_profile_handle.info.unicast_port.flow_control.db_fc.alpha = value;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "threshold not supported for unicast queue");
        }
    }

    else if (data->cosq >= 0 && data->cosq < 8)
    {
        switch (data->tag)
        {
            case (PD_DROP):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_unicast_queue_pd_drop_verify
                                      (unit, &port_profile_handle, data->cosq, value));
                port_profile_handle.info.unicast_queue[data->cosq].pd_drop = value;
                break;
            case (PD_MAX_FC):
            case (PD_FC):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_unicast_queue_pd_fc_verify
                                      (unit, &port_profile_handle, data->cosq, value));
                port_profile_handle.info.unicast_queue[data->cosq].flow_control.pd_fc.max = value;
                if (data->tag == PD_FC)
                {
                    port_profile_handle.info.unicast_queue[data->cosq].flow_control.pd_fc.min = value;
                    port_profile_handle.info.unicast_queue[data->cosq].flow_control.pd_fc.alpha = 0;
                }
                break;
            case (PD_MIN_FC):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_pd_threshold_verify(unit, value));
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_min_threshold_verify
                                      (unit, port_profile_handle.info.unicast_queue[data->cosq].flow_control.pd_fc.max,
                                       value));
                port_profile_handle.info.unicast_queue[data->cosq].flow_control.pd_fc.min = value;
                break;
            case (PD_ALPHA_FC):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_alpha_verify(unit, value));
                port_profile_handle.info.unicast_queue[data->cosq].flow_control.pd_fc.alpha = value;
                break;
            case (DB_DROP):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_unicast_queue_db_drop_verify
                                      (unit, &port_profile_handle, data->cosq, value));
                port_profile_handle.info.unicast_queue[data->cosq].db_drop = value;
                break;
            case (DB_MAX_FC):
            case (DB_FC):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_unicast_queue_db_fc_verify
                                      (unit, &port_profile_handle, data->cosq, value));
                port_profile_handle.info.unicast_queue[data->cosq].flow_control.db_fc.max = value;
                if (data->tag == DB_FC)
                {
                    port_profile_handle.info.unicast_queue[data->cosq].flow_control.db_fc.min = value;
                    port_profile_handle.info.unicast_queue[data->cosq].flow_control.db_fc.alpha = 0;
                }
                break;
            case (DB_MIN_FC):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_db_threshold_verify(unit, value));
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_min_threshold_verify
                                      (unit, port_profile_handle.info.unicast_queue[data->cosq].flow_control.db_fc.max,
                                       value));
                port_profile_handle.info.unicast_queue[data->cosq].flow_control.db_fc.min = value;
                break;
            case (DB_ALPHA_FC):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_alpha_verify(unit, value));
                port_profile_handle.info.unicast_queue[data->cosq].flow_control.db_fc.alpha = data->threshold->value;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid threshold for unicast gport");
        }
    }

    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid cosq %d", data->cosq);
    }

    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_handle_set(unit, &port_profile_handle,
                                                          data->threshold->flags & BCM_COSQ_THRESHOLD_NOT_COMMIT));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_profile_mgmt_unicast_threshold_get(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data)
{
    dnx_ecgm_profile_mgmt_port_handle_t port_profile_handle;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_handle_get(unit, port, &port_profile_handle));

    if (data->cosq == -1)
    {
        switch (data->tag)
        {
            case (PD_DROP):
                data->threshold->value = port_profile_handle.info.unicast_port.pd_drop;
                break;
            case (PD_FC):
            case (PD_MAX_FC):
                data->threshold->value = port_profile_handle.info.unicast_port.flow_control.pd_fc.max;
                break;
            case (PD_MIN_FC):
                data->threshold->value = port_profile_handle.info.unicast_port.flow_control.pd_fc.min;
                break;
            case (PD_ALPHA_FC):
                data->threshold->value = port_profile_handle.info.unicast_port.flow_control.pd_fc.alpha;
                break;
            case (DB_DROP):
                data->threshold->value = port_profile_handle.info.unicast_port.db_drop;
                break;
            case (DB_FC):
            case (DB_MAX_FC):
                data->threshold->value = port_profile_handle.info.unicast_port.flow_control.db_fc.max;
                break;
            case (DB_MIN_FC):
                data->threshold->value = port_profile_handle.info.unicast_port.flow_control.db_fc.min;
                break;
            case (DB_ALPHA_FC):
                data->threshold->value = port_profile_handle.info.unicast_port.flow_control.db_fc.alpha;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "threshold not supported for unicast queue");
        }
    }

    else if (data->cosq >= 0 && data->cosq < DNX_COSQ_NOF_TC)
    {
        switch (data->tag)
        {
            case (PD_DROP):
                data->threshold->value = port_profile_handle.info.unicast_queue[data->cosq].pd_drop;
                break;
            case (PD_FC):
            case (PD_MAX_FC):
                data->threshold->value = port_profile_handle.info.unicast_queue[data->cosq].flow_control.pd_fc.max;
                break;
            case (PD_MIN_FC):
                data->threshold->value = port_profile_handle.info.unicast_queue[data->cosq].flow_control.pd_fc.min;
                break;
            case (PD_ALPHA_FC):
                data->threshold->value = port_profile_handle.info.unicast_queue[data->cosq].flow_control.pd_fc.alpha;
                break;
            case (DB_DROP):
                data->threshold->value = port_profile_handle.info.unicast_queue[data->cosq].db_drop;
                break;
            case (DB_FC):
            case (DB_MAX_FC):
                data->threshold->value = port_profile_handle.info.unicast_queue[data->cosq].flow_control.db_fc.max;
                break;
            case (DB_MIN_FC):
                data->threshold->value = port_profile_handle.info.unicast_queue[data->cosq].flow_control.db_fc.min;
                break;
            case (DB_ALPHA_FC):
                data->threshold->value = port_profile_handle.info.unicast_queue[data->cosq].flow_control.db_fc.alpha;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid threshold for unicast gport");
        }
    }

    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid cosq %d", data->cosq);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_profile_mgmt_multicast_threshold_set(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data)
{
    int dp = data->threshold->dp;
    int value = data->threshold->value;
    dnx_ecgm_profile_mgmt_port_handle_t port_profile_handle;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_port_set_verify(unit, port, data));

    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_handle_get(unit, port, &port_profile_handle));

    if (data->cosq == -1)
    {
        switch (data->tag)
        {
            case (PD_MAX_DROP):
            case (PD_DROP):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_multicast_port_pd_drop_verify
                                      (unit, &port_profile_handle, value));
                port_profile_handle.info.multicast_port.pd_drop.max = value;
                if (data->tag == PD_DROP)
                {
                    port_profile_handle.info.multicast_port.pd_drop.min = value;
                    port_profile_handle.info.multicast_port.pd_drop.alpha = 0;
                }
                break;
            case (PD_MIN_DROP):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_pd_threshold_verify(unit, value));
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_min_threshold_verify
                                      (unit, port_profile_handle.info.multicast_port.pd_drop.max, value));
                port_profile_handle.info.multicast_port.pd_drop.min = value;
                break;
            case (PD_ALPHA_DROP):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_alpha_verify(unit, value));
                port_profile_handle.info.multicast_port.pd_drop.alpha = value;
                break;
            case (DB_DROP):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_multicast_port_db_drop_verify
                                      (unit, &port_profile_handle, value));
                port_profile_handle.info.multicast_port.db_drop = value;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "threshold not supported for multicast port");
        }
    }

    else if (data->cosq >= 0 && data->cosq < DNX_COSQ_NOF_TC)
    {
        switch (data->tag)
        {
            case (PD_MAX_DROP_PER_DP):
            case (PD_DROP_PER_DP):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_multicast_queue_pd_drop_verify
                                      (unit, &port_profile_handle, value));
                port_profile_handle.info.multicast_queue[data->cosq].drop_thresholds[dp].pd_drop.max = value;
                if (data->tag == PD_DROP_PER_DP)
                {
                    port_profile_handle.info.multicast_queue[data->cosq].drop_thresholds[dp].pd_drop.min = value;
                    port_profile_handle.info.multicast_queue[data->cosq].drop_thresholds[dp].pd_drop.alpha = 0;
                }
                break;
            case (PD_MIN_DROP_PER_DP):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_pd_threshold_verify(unit, value));
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_min_threshold_verify
                                      (unit,
                                       port_profile_handle.info.multicast_queue[data->cosq].drop_thresholds[dp].
                                       pd_drop.max, value));
                port_profile_handle.info.multicast_queue[data->cosq].drop_thresholds[dp].pd_drop.min = value;
                break;
            case (PD_ALPHA_DROP_PER_DP):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_alpha_verify(unit, value));
                port_profile_handle.info.multicast_queue[data->cosq].drop_thresholds[dp].pd_drop.alpha = value;
                break;
            case (DB_DROP_PER_DP):
                SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_multicast_queue_db_drop_verify
                                      (unit, &port_profile_handle, value));
                port_profile_handle.info.multicast_queue[data->cosq].drop_thresholds[dp].db_drop = value;
                break;
            case (SP_PD_RESERVED):
                SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_multicast_queue_sp_pd_reserved_set
                                (unit, &port_profile_handle, data->cosq, value));
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "threshold not supported for multicast queue");
        }
    }

    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid cosq %d", data->cosq);
    }

    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_handle_set(unit, &port_profile_handle,
                                                          data->threshold->flags & BCM_COSQ_THRESHOLD_NOT_COMMIT));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_profile_mgmt_multicast_threshold_get(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data)
{
    int dp = data->threshold->dp;
    dnx_ecgm_profile_mgmt_port_handle_t port_profile_handle;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_handle_get(unit, port, &port_profile_handle));

    if (data->cosq == -1)
    {
        switch (data->tag)
        {
            case (PD_DROP):
            case (PD_MAX_DROP):
                data->threshold->value = port_profile_handle.info.multicast_port.pd_drop.max;
                break;
            case (PD_MIN_DROP):
                data->threshold->value = port_profile_handle.info.multicast_port.pd_drop.min;
                break;
            case (PD_ALPHA_DROP):
                data->threshold->value = port_profile_handle.info.multicast_port.pd_drop.alpha;
                break;
            case (DB_DROP):
                data->threshold->value = port_profile_handle.info.multicast_port.db_drop;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "threshold not supported for multicast port");
        }
    }

    else if (data->cosq >= 0 && data->cosq < DNX_COSQ_NOF_TC)
    {
        switch (data->tag)
        {
            case (PD_DROP_PER_DP):
            case (PD_MAX_DROP_PER_DP):
                data->threshold->value =
                    port_profile_handle.info.multicast_queue[data->cosq].drop_thresholds[dp].pd_drop.max;
                break;
            case (PD_MIN_DROP_PER_DP):
                data->threshold->value =
                    port_profile_handle.info.multicast_queue[data->cosq].drop_thresholds[dp].pd_drop.min;
                break;
            case (PD_ALPHA_DROP_PER_DP):
                data->threshold->value =
                    port_profile_handle.info.multicast_queue[data->cosq].drop_thresholds[dp].pd_drop.alpha;
                break;
            case (DB_DROP_PER_DP):
                data->threshold->value =
                    port_profile_handle.info.multicast_queue[data->cosq].drop_thresholds[dp].db_drop;
                break;
            case (SP_PD_RESERVED):
                data->threshold->value = port_profile_handle.info.multicast_queue[data->cosq].reserved_pds;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "threshold not supported for multicast queue");
        }
    }

    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid cosq %d", data->cosq);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_profile_mgmt_interface_handle_get(
    int unit,
    bcm_port_t port,
    dnx_ecgm_profile_mgmt_interface_handle_t * interface_profile_handle)
{
    int ref_count;
    int valid;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db.interface_caching.valid.get(unit, &valid));

    interface_profile_handle->port = port;

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &interface_profile_handle->core));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &interface_profile_handle->egq_if));

    if (valid)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_db.interface_caching.info.get(unit, &interface_profile_handle->info));
        SHR_IF_ERR_EXIT(dnx_ecgm_db.interface_caching.profile_id.get(unit, &interface_profile_handle->profile_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_egq_if_to_interface_profile_map_get
                        (unit, interface_profile_handle->core, interface_profile_handle->egq_if,
                         (uint32 *) &interface_profile_handle->profile_id));

        SHR_IF_ERR_EXIT(algo_ecgm_db.ecgm_interface_profile.profile_data_get
                        (unit, interface_profile_handle->core, interface_profile_handle->profile_id, &ref_count,
                         &interface_profile_handle->info));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_profile_mgmt_interface_handle_destroy(
    int unit,
    dnx_ecgm_profile_mgmt_interface_handle_t * interface_profile_handle)
{
    int nof_channels;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * destroying interface profile only for the last interface occurence 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, interface_profile_handle->port, &nof_channels));

    if (nof_channels == 1)
    {
        sal_memset(&interface_profile_handle->info, 0, sizeof(dnx_ecgm_interface_profile_info_t));

        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_interface_info_set(unit, interface_profile_handle));
    }

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_ecgm_profile_mgmt_interface_set_verify(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data)
{
    int valid;
    bcm_port_t cached_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_interface_threshold_verify(unit, data->cosq, data->threshold->priority));

    if (data->threshold->flags & BCM_COSQ_THRESHOLD_NOT_COMMIT)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_db.interface_caching.valid.get(unit, &valid));
        if (valid)
        {
            SHR_IF_ERR_EXIT(dnx_ecgm_db.interface_caching.port.get(unit, &cached_port));
            if (port != cached_port)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Configuration for port %d is not commited yet.\n", cached_port);

            }

        }
    }

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_ecgm_profile_mgmt_interface_threshold_set(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data)
{
    int value;
    dnx_ecgm_profile_mgmt_interface_handle_t interface_profile_handle;
    dnx_ecgm_interface_profile_thresholds_t *priority_ptr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_interface_set_verify(unit, port, data));

    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_interface_handle_get(unit, port, &interface_profile_handle));

    /*
     * choosing the right priority 
     */
    priority_ptr =
        (data->threshold->priority == BCM_COSQ_HIGH_PRIORITY) ? &interface_profile_handle.info.high_priority :
        &interface_profile_handle.info.low_priority;
    value = data->threshold->value;

    switch (data->tag)
    {
        case (UC_PD_MAX_FC):
        case (UC_PD_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_interface_uc_pd_fc_verify(unit, port, value));
            priority_ptr->uc_fc.pd_fc.max = value;
            if (data->tag == UC_PD_FC)
            {
                priority_ptr->uc_fc.pd_fc.min = value;
                priority_ptr->uc_fc.pd_fc.alpha = 0;
            }
            break;
        case (UC_PD_MIN_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_pd_threshold_verify(unit, value));
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_min_threshold_verify(unit, priority_ptr->uc_fc.pd_fc.max, value));
            priority_ptr->uc_fc.pd_fc.min = value;
            break;
        case (UC_PD_ALPHA_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_alpha_verify(unit, value));
            priority_ptr->uc_fc.pd_fc.alpha = value;
            break;
        case (UC_DB_MAX_FC):
        case (UC_DB_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_interface_uc_db_fc_verify(unit, port, value));
            priority_ptr->uc_fc.db_fc.max = value;
            if (data->tag == UC_DB_FC)
            {
                priority_ptr->uc_fc.db_fc.min = value;
                priority_ptr->uc_fc.db_fc.alpha = 0;
            }
            break;
        case (UC_DB_MIN_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_db_threshold_verify(unit, value));
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_min_threshold_verify(unit, priority_ptr->uc_fc.db_fc.max, value));
            priority_ptr->uc_fc.db_fc.min = value;
            break;
        case (UC_DB_ALPHA_FC):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_alpha_verify(unit, value));
            priority_ptr->uc_fc.db_fc.alpha = value;
            break;
        case (MC_PD_DROP):
            SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_interface_mc_pd_drop_verify(unit, port, value));
            priority_ptr->mc_interface_PDs_drop = value;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid threshold for interface gport.");
    }

    if (data->threshold->flags & BCM_COSQ_THRESHOLD_NOT_COMMIT)
    {
        /*
         * store the profile info in SW state
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_db.interface_caching.valid.set(unit, TRUE));
        SHR_IF_ERR_EXIT(dnx_ecgm_db.interface_caching.port.set(unit, port));
        SHR_IF_ERR_EXIT(dnx_ecgm_db.interface_caching.profile_id.set(unit, interface_profile_handle.profile_id));
        SHR_IF_ERR_EXIT(dnx_ecgm_db.interface_caching.info.set(unit, interface_profile_handle.info));

    }
    else
    {
        /*
         * set the new profile info to the port 
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_interface_info_set(unit, &interface_profile_handle));

        SHR_IF_ERR_EXIT(dnx_ecgm_db.interface_caching.valid.set(unit, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_profile_mgmt_interface_threshold_get(
    int unit,
    bcm_port_t port,
    dnx_ecgm_threshold_data_t * data)
{
    dnx_ecgm_profile_mgmt_interface_handle_t interface_profile_handle;
    dnx_ecgm_interface_profile_thresholds_t *priority_ptr;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_interface_handle_get(unit, port, &interface_profile_handle));

    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_interface_threshold_verify
                          (unit, data->cosq, data->threshold->priority));

    priority_ptr =
        (data->threshold->priority == BCM_COSQ_HIGH_PRIORITY) ?
        &interface_profile_handle.info.high_priority : &interface_profile_handle.info.low_priority;

    switch (data->tag)
    {
        case (UC_PD_FC):
        case (UC_PD_MAX_FC):
            data->threshold->value = priority_ptr->uc_fc.pd_fc.max;
            break;
        case (UC_PD_MIN_FC):
            data->threshold->value = priority_ptr->uc_fc.pd_fc.min;
            break;
        case (UC_PD_ALPHA_FC):
            data->threshold->value = priority_ptr->uc_fc.pd_fc.alpha;
            break;
        case (UC_DB_FC):
        case (UC_DB_MAX_FC):
            data->threshold->value = priority_ptr->uc_fc.db_fc.max;
            break;
        case (UC_DB_MIN_FC):
            data->threshold->value = priority_ptr->uc_fc.db_fc.min;
            break;
        case (UC_DB_ALPHA_FC):
            data->threshold->value = priority_ptr->uc_fc.pd_fc.alpha;
            break;
        case (MC_PD_DROP):
            data->threshold->value = priority_ptr->mc_interface_PDs_drop;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid threshold for interface gport.");
    }

exit:
    SHR_FUNC_EXIT;
}

/* static functions definitions */

/* functions used for port profile management */

static shr_error_e
dnx_ecgm_profile_mgmt_port_info_set(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle)
{
    uint8 last_reference;
    uint8 first_reference;
    int old_profile = port_profile_handle->profile_id;
    dnx_ecgm_profile_mgmt_port_handle_t clear_profile;  /* to clear hw profile */
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_ecgm_db.ecgm_port_profile.exchange
                    (unit, port_profile_handle->core, 0, &port_profile_handle->info, old_profile, NULL,
                     &port_profile_handle->profile_id, &first_reference, &last_reference));

    if (first_reference)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_info_hw_set(unit, port_profile_handle));
    }

    if (port_profile_handle->profile_id != old_profile) /* map port to new profile */
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_tm_port_to_port_profile_set
                        (unit, port_profile_handle->core, port_profile_handle->tm_port,
                         port_profile_handle->base_q_pair, port_profile_handle->profile_id));
        if (last_reference)     /* reset the old profile */
        {
            clear_profile = *port_profile_handle;
            clear_profile.profile_id = old_profile;
            sal_memset(&clear_profile.info, 0, sizeof(dnx_ecgm_port_profile_info_t));
            SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_port_info_hw_set(unit, &clear_profile));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_profile_mgmt_port_info_hw_set(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle)
{
    int queue_offset, dp;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * set port profile info per unicast and multicast port
     */
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_unicast_port_thresholds_set
                    (unit, port_profile_handle->core, port_profile_handle->profile_id,
                     &port_profile_handle->info.unicast_port));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_multicast_port_thresholds_set
                    (unit, port_profile_handle->core, port_profile_handle->profile_id,
                     &port_profile_handle->info.multicast_port));

    /*
     * set port profile info per unicast and multicast queue
     */
    for (queue_offset = 0; queue_offset < DNX_COSQ_NOF_TC; ++queue_offset)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_unicast_queue_thresholds_set
                        (unit, port_profile_handle->core, port_profile_handle->profile_id, queue_offset,
                         &port_profile_handle->info.unicast_queue[queue_offset]));
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_multicast_queue_to_sp_set
                        (unit, port_profile_handle->core, port_profile_handle->profile_id, queue_offset,
                         port_profile_handle->info.multicast_queue[queue_offset].pd_sp));
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_multicast_queue_tc_thresholds_set
                        (unit, port_profile_handle->core, port_profile_handle->profile_id, queue_offset,
                         port_profile_handle->info.multicast_queue[queue_offset].reserved_pds));

        /*
         * set port profile info per multicast queue drop precedence
         */
        for (dp = 0; dp < DNX_COSQ_NOF_DP; ++dp)
        {
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_multicast_queue_tc_dp_thresholds_set
                            (unit, port_profile_handle->core, port_profile_handle->profile_id, queue_offset, dp,
                             port_profile_handle->info.multicast_queue[queue_offset].drop_thresholds));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_profile_mgmt_pd_sp_set_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    bcm_cos_queue_t queue,
    int pd_sp)
{
    SHR_FUNC_INIT_VARS(unit);
    if (pd_sp != bcmCosqEgressMulticastServicePool0 && pd_sp != bcmCosqEgressMulticastServicePool1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "error: PD service pool %d is invalid. service pool value may be 0 or 1.", pd_sp);
    }

    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_queue_verify(unit, port_profile_handle, queue));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_profile_mgmt_pd_sp_get_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    bcm_cos_queue_t queue,
    int *pd_sp)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(pd_sp, _SHR_E_PARAM, "service pool ptr null");
    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_queue_verify(unit, port_profile_handle, queue));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_profile_mgmt_sp_reserved_pd_threshold_modify(
    int unit,
    int core_id,
    int sp,
    int reserved_pd,
    dnx_ecgm_threshold_sp_action_e action)
{
    uint32 reserved;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_get
                    (unit, core_id, sp, DBAL_FIELD_RESERVED_PD, &reserved));

    switch (action)
    {
        case (DNX_ECGM_THRESHOLD_SP_SET):
            reserved = reserved_pd;
            break;
        case (DNX_ECGM_THRESHOLD_SP_DECREASE):
            reserved -= reserved_pd;
            break;
        case (DNX_ECGM_THRESHOLD_SP_INCREASE):
            reserved += reserved_pd;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid action %d", action);
            break;
    }

    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_sp_reserved_pd_threshold_modify_verify(unit, core_id, sp, reserved));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_set(unit, core_id, sp, DBAL_FIELD_RESERVED_PD, reserved));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_profile_mgmt_sp_reserved_pd_threshold_modify_verify(
    int unit,
    int core,
    int sp,
    uint32 sp_reserved)
{
    int total_reserved, max_pds = dnx_data_ecgm.core_resources.total_pds_get(unit);
    uint32 other_sp_reserved;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_get
                    (unit, core, ((sp == 0) ? 1 : 0), DBAL_FIELD_RESERVED_PD, &other_sp_reserved));

    total_reserved = (other_sp_reserved + sp_reserved);
    if (total_reserved > max_pds)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid new reserved value - would make the sum of reserved PDs for both SPs = %d. must be: 0 <= sum < %d.",
                     total_reserved, max_pds);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_profile_mgmt_pd_sp_reserved_for_each_queue_modify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    dnx_ecgm_threshold_sp_action_e action)
{
    int queue;
    SHR_FUNC_INIT_VARS(unit);
    for (queue = 0; queue < port_profile_handle->num_priorities; ++queue)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_sp_reserved_pd_threshold_modify
                        (unit, port_profile_handle->core, port_profile_handle->info.multicast_queue[queue].pd_sp,
                         port_profile_handle->info.multicast_queue[queue].reserved_pds, action));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_profile_mgmt_queue_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    bcm_cos_queue_t offset)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify cosq is legal (should be before verifying queue)
     */
    if (offset >= port_profile_handle->num_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "error: invalid queue offset. must be smaller than number of priorities.");
    }

exit:
    SHR_FUNC_EXIT;
}

/* verifying functions for port threshold setting */
static shr_error_e
dnx_ecgm_profile_mgmt_unicast_queue_pd_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int cosq,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_unicast_queue_pd_fc_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int cosq,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_unicast_queue_db_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int cosq,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_unicast_queue_db_fc_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int cosq,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_unicast_port_pd_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_unicast_port_pd_fc_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_unicast_port_db_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_unicast_port_db_fc_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_profile_mgmt_multicast_queue_sp_pd_reserved_set(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int cosq,
    int value)
{
    dnx_ecgm_threshold_sp_action_e action;
    uint32 old_value, difference;
    uint32 enable = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_profile_mgmt_multicast_queue_sp_pd_reserved_verify
                          (unit, port_profile_handle, cosq, value));

    /*
     * save port enable status 
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_get(unit, port_profile_handle->port, &enable));

    /*
     * flush EGQ and disable the port to make sure no reserved PDs from the current SP are left
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port_profile_handle->port, 0, TRUE));

    old_value = port_profile_handle->info.multicast_queue[cosq].reserved_pds;
    port_profile_handle->info.multicast_queue[cosq].reserved_pds = value;

    if (old_value > value)
    {
        difference = old_value - value;
        action = DNX_ECGM_THRESHOLD_SP_DECREASE;
    }
    else
    {
        difference = value - old_value;
        action = DNX_ECGM_THRESHOLD_SP_INCREASE;
    }

    SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_sp_reserved_pd_threshold_modify(unit, port_profile_handle->core,
                                                                          port_profile_handle->
                                                                          info.multicast_queue[cosq].pd_sp, difference,
                                                                          action));

    /*
     * re-enable the port
     */
    if (enable)
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port_profile_handle->port, 1, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_profile_mgmt_multicast_queue_pd_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_multicast_queue_db_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_multicast_port_pd_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_multicast_port_db_drop_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_multicast_queue_sp_pd_reserved_verify(
    int unit,
    dnx_ecgm_profile_mgmt_port_handle_t * port_profile_handle,
    int cosq,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}

/* functions used for interface profile management */
static shr_error_e
dnx_ecgm_profile_mgmt_interface_info_set(
    int unit,
    dnx_ecgm_profile_mgmt_interface_handle_t * interface_profile_handle)
{
    uint8 last_reference;
    uint8 first_reference;
    int old_profile = interface_profile_handle->profile_id;
    dnx_ecgm_profile_mgmt_interface_handle_t clear_profile;     /* to clear hw profile */
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_ecgm_db.ecgm_interface_profile.exchange
                    (unit, interface_profile_handle->core, 0, &interface_profile_handle->info, old_profile,
                     NULL, &interface_profile_handle->profile_id, &first_reference, &last_reference));

    if (first_reference)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_interface_info_hw_set(unit, interface_profile_handle));
    }

    if (interface_profile_handle->profile_id != old_profile)    /* map port to new profile */
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_egq_if_to_interface_profile_map_set
                        (unit, interface_profile_handle->core,
                         interface_profile_handle->egq_if, interface_profile_handle->profile_id));

        if (last_reference)     /* reset the old profile */
        {
            clear_profile = *interface_profile_handle;
            clear_profile.profile_id = old_profile;
            sal_memset(&clear_profile.info, 0, sizeof(dnx_ecgm_interface_profile_info_t));
            SHR_IF_ERR_EXIT(dnx_ecgm_profile_mgmt_interface_info_hw_set(unit, &clear_profile));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_profile_mgmt_interface_info_hw_set(
    int unit,
    dnx_ecgm_profile_mgmt_interface_handle_t * interface_profile_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * set mc values
     */
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_interface_multicast_thresholds_set
                    (unit, interface_profile_handle->core, DBAL_ENUM_FVAL_ECGM_PRIORITY_LOW,
                     interface_profile_handle->profile_id,
                     interface_profile_handle->info.low_priority.mc_interface_PDs_drop));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_interface_multicast_thresholds_set
                    (unit, interface_profile_handle->core, DBAL_ENUM_FVAL_ECGM_PRIORITY_HIGH,
                     interface_profile_handle->profile_id,
                     interface_profile_handle->info.high_priority.mc_interface_PDs_drop));

    /*
     * set uc values
     */
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_interface_unicast_thresholds_set
                    (unit, interface_profile_handle->core, DBAL_ENUM_FVAL_ECGM_PRIORITY_LOW,
                     interface_profile_handle->profile_id, &interface_profile_handle->info.low_priority.uc_fc));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_interface_unicast_thresholds_set
                    (unit, interface_profile_handle->core, DBAL_ENUM_FVAL_ECGM_PRIORITY_HIGH,
                     interface_profile_handle->profile_id, &interface_profile_handle->info.high_priority.uc_fc));

exit:
    SHR_FUNC_EXIT;
}

/* verifying functions for interface threshold setting */
static shr_error_e
dnx_ecgm_profile_mgmt_interface_uc_pd_fc_verify(
    int unit,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_interface_uc_db_fc_verify(
    int unit,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_db_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_interface_mc_pd_drop_verify(
    int unit,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_pd_threshold_verify(unit, value));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_ecgm_profile_mgmt_interface_threshold_verify(
    int unit,
    int cosq,
    int priority)
{
    SHR_FUNC_INIT_VARS(unit);

    if (cosq != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid cosq %d. for interface cosq must be = -1.", cosq);
    }

    if (priority != BCM_COSQ_HIGH_PRIORITY && priority != BCM_COSQ_LOW_PRIORITY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "invalid priority %d. must indicate interface priority BCM_COSQ_LOW_PRIORITY / BCM_COSQ_HIGH_PRIORITY.",
                     priority);
    }

exit:
    SHR_FUNC_EXIT;
}
