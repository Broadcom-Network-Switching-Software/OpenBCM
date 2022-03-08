/**
 * \file egr_tm_dispatch.c
 *
 * dispatch for egress TM functions that are common between versions.
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
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm_int/dnx/cosq/egress/egr_tm_dispatch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing_v1.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/egress/egq_ps_db.h>
#include <bcm_int/dnx/cosq/egress/esb.h>
#include <bcm_int/dnx/cosq/egress/rate_measurement.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cosq/egress/cosq_egq.h>
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>

/** see header file */
shr_error_e
dnx_egr_tm_queuing_port_add_handle(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_add_handle(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_queuing_port_remove_handle(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_remove_handle(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_queuing_port_disable_handle(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_disable(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_interface_allocate(
    int unit,
    bcm_port_t port,
    int master_logical_port,
    bcm_core_t core,
    bcm_port_interface_info_t * interface_info,
    int is_channelized_interface,
    int flexe_channel_id,
    int *egr_if_id,
    int *ch_egr_if_id)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_interface_alloc
                        (unit, port, master_logical_port, core, interface_info->interface, is_channelized_interface,
                         flexe_channel_id, egr_if_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_base_queue_allocate(
    int unit,
    uint32 flags,
    bcm_port_t port,
    bcm_core_t core,
    int egr_interface_id,
    int nof_priorities,
    int is_channelized,
    int is_rcy,
    int *base_queue)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID))
        {
            SHR_IF_ERR_EXIT(dnx_ps_db_base_qpair_alloc_with_id
                            (unit, port, egr_interface_id, is_channelized, core, nof_priorities, *base_queue));
        }
        else
        {
            int priority_propagation_enable = _SHR_IS_FLAG_SET(flags, BCM_PORT_ADD_SCH_PRIORITY_PROPAGATION_EN);
            SHR_IF_ERR_EXIT(dnx_ps_db_base_qpair_alloc
                            (unit, port, egr_interface_id, is_channelized, core, nof_priorities,
                             priority_propagation_enable, base_queue));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_cosq_control_port_tc_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egq_control_egress_port_tc_set(unit, gport, cosq, type, arg));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_cosq_control_port_tc_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egq_control_egress_port_tc_get(unit, gport, cosq, type, arg));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_port_priority_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_is_high_set(unit, gport, cosq, mode, weight));
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_port_priority_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_is_high_get(unit, gport, cosq, mode, weight));
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_port_credit_init(
    int unit,
    bcm_port_t logical_port)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_credit_default_set(unit, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_port_speed_set_config(
    int unit,
    bcm_port_t logical_port,
    int speed)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /** get port_info */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    /** if not egress TM port, nothing to do - get out */
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        SHR_EXIT();
    }

    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_if_thr_set(unit, logical_port));
        SHR_IF_ERR_EXIT(dnx_egr_queuing_if_sub_calendar_set(unit, logical_port));
        if (dnx_data_egr_queuing.rate_measurement.feature_get(unit, dnx_data_egr_queuing_rate_measurement_is_supported))
        {
            /** re-calculate egress rate measure parameters according to the new port rate */
            SHR_IF_ERR_EXIT(dnx_rate_measurement_port_rate_update_set(unit, logical_port));
        }

        /** Egress Shared Buffers allocate per port*/
        if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support))
        {
            SHR_IF_ERR_EXIT(dnx_esb_port_speed_config_add(unit, logical_port));
        }

        /** Update credit size for special interfaces */
        /** Nif interfaces is being updated when the ports are enabled */
        if ((!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, STIF, L1, FLEXE, MGMT)))
            && (!DNX_ALGO_PORT_TYPE_IS_FRAMER(unit, port_info)))
        {
            int total_special_if_speed = 0;
            if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
            {
                /** For RCY interfaces, this function must be called with the total speed for both RCY interfaces */
                bcm_pbmp_t all_rcy_master_ports;
                bcm_port_t rcy_port_i;
                bcm_core_t core;
                int rcy_speed_i = 0;
                int has_speed;
                SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
                SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                                (unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_RCY, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                                 &all_rcy_master_ports));
                BCM_PBMP_ITER(all_rcy_master_ports, rcy_port_i)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, rcy_port_i, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
                    if (has_speed == TRUE)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get
                                        (unit, rcy_port_i, DNX_ALGO_PORT_SPEED_F_TX, &rcy_speed_i));
                        total_special_if_speed += rcy_speed_i;
                    }
                }
            }
            else
            {
                total_special_if_speed = speed;
            }
            SHR_IF_ERR_EXIT(dnx_egr_queuing_special_if_credit_set(unit, logical_port, total_special_if_speed));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_scheduling_calendar_switch(
    int unit,
    bcm_core_t core)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_calendar_activate(unit, core));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_scheduling_stand_by_calendar_set(
    int unit,
    bcm_core_t core)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_calendar_refresh(unit, core));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_port_enable_and_flush_set(
    int unit,
    bcm_port_t logical_port,
    uint32 enable,
    uint32 flush)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, logical_port, enable, flush));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_queue_wfq_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egq_queue_wfq_set(unit, gport, cosq, mode, weight));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_queue_wfq_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egq_queue_wfq_get(unit, gport, cosq, mode, weight));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_rcy_eir_weight_get(
    int unit,
    bcm_gport_t gport,
    int *weight)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        int is_mirror = BCM_COSQ_GPORT_IS_RCY_MIRR(gport);
        SHR_IF_ERR_EXIT(dnx_egr_recycle_weight_get(unit, BCM_COSQ_GPORT_CORE_GET(gport), is_mirror, (uint32 *) weight));
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_rcy_eir_weight_set(
    int unit,
    bcm_gport_t gport,
    int weight)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        int is_mirror = BCM_COSQ_GPORT_IS_RCY_MIRR(gport);
        SHR_IF_ERR_EXIT(dnx_egr_recycle_weight_set(unit, BCM_COSQ_GPORT_CORE_GET(gport), is_mirror, weight));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_cosq_gport_egress_map_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    bcm_cos_queue_t offset)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egq_gport_egress_queue_map_set(unit, gport, ingress_pri, ingress_dp, offset));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_cosq_gport_egress_map_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t ingress_pri,
    bcm_color_t ingress_dp,
    bcm_cos_queue_t * offset)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egq_gport_egress_map_get(unit, gport, ingress_pri, ingress_dp, offset));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_interface_shaper_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egq_gport_egress_interface_scheduler_bandwidth_set(unit, gport, cosq,
                                                                                    kbits_sec_min, kbits_sec_max,
                                                                                    flags));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_port_shaper_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egq_ofp_rates_port_bandwidth_set(unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_interface_shaper_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egq_gport_egress_interface_scheduler_bandwidth_get(unit, gport, cosq,
                                                                                    kbits_sec_min, kbits_sec_max,
                                                                                    flags));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_port_shaper_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egq_ofp_rates_port_bandwidth_get(unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_queue_shaper_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egq_gport_egress_port_tc_bandwidth_set(unit, gport, cosq,
                                                                        kbits_sec_min, kbits_sec_max, flags));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_egr_tm_queue_shaper_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_egr_queuing_v1.general.feature_get(unit, dnx_data_egr_queuing_v1_general_v1_supported))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_egq_gport_egress_port_tc_bandwidth_get(unit, gport, cosq,
                                                                        kbits_sec_min, kbits_sec_max, flags));
    }

exit:
    SHR_FUNC_EXIT;
}
