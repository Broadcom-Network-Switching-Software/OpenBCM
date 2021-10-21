/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * DNXF COSQ
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ
#include <shared/bsl.h>
#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnxf_dispatch.h>
#include <bcm/debug.h>
#include <bcm/cosq.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/mbcm.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>

/**********************************************************/
/*                  Verifications                         */
/**********************************************************/

/**********************************************************/
/*                  Internal functions                    */
/**********************************************************/
STATIC int _bcm_dnxf_cosq_gport_pipe_dynamic_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_dynamic_state_t state,
    int weight);

STATIC int _bcm_dnxf_cosq_gport_pipe_dynamic_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_dynamic_state_t state,
    int *weight);

/**********************************************************/
/*                  Implementation                        */
/**********************************************************/

int
bcm_dnxf_cosq_gport_handle_get(
    int unit,
    bcm_cosq_gport_type_t gport_type,
    bcm_cosq_gport_info_t * gport_info)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (gport_type)
    {
        case bcmCosqGportTypeFabricPipe:
            BCM_COSQ_GPORT_FABRIC_PIPE_SET(gport_info->out_gport, gport_info->in_gport, gport_info->cosq);
            break;
        case bcmCosqGportTypeFabricPipeEgress:
            BCM_COSQ_GPORT_FABRIC_PIPE_EGRESS_SET(gport_info->out_gport, gport_info->in_gport, gport_info->cosq);
            break;
        case bcmCosqGportTypeFabricPipeIngress:
            BCM_COSQ_GPORT_FABRIC_PIPE_INGRESS_SET(gport_info->out_gport, gport_info->in_gport, gport_info->cosq);
            break;
        case bcmCosqGportTypeFabricPipeMiddle:
            BCM_COSQ_GPORT_FABRIC_PIPE_MIDDLE_SET(gport_info->out_gport, gport_info->in_gport, gport_info->cosq);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Handle type (%d) requested\n", gport_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;

}

int
bcm_dnxf_cosq_gport_sched_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_dynamic_state_t state,
    int weight,
    int is_set)
{
    int pipe, nof_pipes;

    SHR_FUNC_INIT_VARS(unit);

    if (!(BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport)
          || BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid gport type (%d) requested\n", unit, gport);
    }

    pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);

    if (pipe > (nof_pipes - 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid pipe id %d\n", unit, pipe);
    }

    if (cosq != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "cosq parameter should be -1 \n");
    }

    if (state != bcmCosqDynamicStateNormal && state != bcmCosqDynamicStateQueue0Congested
        && state != bcmCosqDynamicStateQueue1Congested)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid state %d provided \n", state);
    }

    if (is_set)
    {
        if ((weight > dnxf_data_fabric.congestion.max_wfq_weight_value_get(unit)) || (weight <= 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid weight %d\n", weight);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mode != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mode parameter is irrelevant and should be always 0\n");
    }

    SHR_IF_ERR_EXIT(bcm_dnxf_cosq_gport_dynamic_sched_set(unit, gport, cosq, bcmCosqDynamicStateNormal, weight));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnxf_cosq_gport_dynamic_sched_get(unit, gport, cosq, bcmCosqDynamicStateNormal, weight));

    *mode = 0;

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_dynamic_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_dynamic_state_t state,
    int weight)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnxf_cosq_gport_sched_validate(unit, gport, cosq, state, weight, 1));

    SHR_IF_ERR_EXIT(_bcm_dnxf_cosq_gport_pipe_dynamic_sched_set(unit, gport, cosq, state, weight));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_dynamic_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_dynamic_state_t state,
    int *weight)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnxf_cosq_gport_sched_validate(unit, gport, cosq, state, *weight, 0));

    SHR_IF_ERR_EXIT(_bcm_dnxf_cosq_gport_pipe_dynamic_sched_get(unit, gport, cosq, state, weight));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid API. Please use bcm_fabric_profile_threshold_set().\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid API. Please use bcm_fabric_profile_threshold_get().\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is not supported for this device.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is not supported for this device.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_control_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid control type (%d) requested\n", unit, type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_control_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid control type (%d) requested\n", unit, type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/* internal functions  */

STATIC int
_bcm_dnxf_cosq_gport_pipe_dynamic_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_dynamic_state_t state,
    int weight)
{
    int port, pipe;
    soc_dnxf_cosq_weight_mode_t weight_mode;
    DNXF_UNIT_LOCK_INIT(unit);

    SHR_FUNC_INIT_VARS(unit);

    pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    port = BCM_COSQ_GPORT_FABRIC_PIPE_PORT_GET(gport);

    switch (state)
    {
        case bcmCosqDynamicStateNormal:
            weight_mode = soc_dnxf_cosq_weight_mode_regular;
            break;
        case bcmCosqDynamicStateQueue0Congested:
            weight_mode = soc_dnxf_cosq_weight_mode_dynamic_0;
            break;
        case bcmCosqDynamicStateQueue1Congested:
            weight_mode = soc_dnxf_cosq_weight_mode_dynamic_1;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid mode type parameter\n", unit);
            break;
    }

    DNXF_UNIT_LOCK_TAKE(unit);

    /*
     * dispatch according to type
     */
    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport))   /* ingress */
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cosq_pipe_weight_set,
                                              (unit, soc_dnxf_cosq_pipe_type_ingress, pipe, port, weight_mode,
                                               weight)));

        /*
         * RTP output stage settings
         */
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cosq_pipe_weight_set,
                                              (unit, soc_dnxf_cosq_pipe_type_rtp, pipe, port, weight_mode, weight)));
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport))       /* middle */
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cosq_pipe_weight_set,
                                              (unit, soc_dnxf_cosq_pipe_type_middle, pipe, port, weight_mode, weight)));
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))       /* egress */
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cosq_pipe_weight_set,
                                              (unit, soc_dnxf_cosq_pipe_type_egress, pipe, port, weight_mode, weight)));
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport))      /* global */
    {
        /*
         * ingress settings
         */
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cosq_pipe_weight_set,
                                              (unit, soc_dnxf_cosq_pipe_type_ingress, pipe, port, weight_mode,
                                               weight)));

        /*
         * RTP output stage settings
         */
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cosq_pipe_weight_set,
                                              (unit, soc_dnxf_cosq_pipe_type_rtp, pipe, port, weight_mode, weight)));

        /*
         * middle settings
         */
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cosq_pipe_weight_set,
                                              (unit, soc_dnxf_cosq_pipe_type_middle, pipe, port, weight_mode, weight)));

        /*
         * egress settings
         */
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cosq_pipe_weight_set,
                                              (unit, soc_dnxf_cosq_pipe_type_egress, pipe, port, weight_mode, weight)));
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

STATIC int
_bcm_dnxf_cosq_gport_pipe_dynamic_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_dynamic_state_t state,
    int *weight)
{
    int port, pipe;
    soc_dnxf_cosq_weight_mode_t weight_mode = soc_dnxf_cosq_weight_mode_regular;
    DNXF_UNIT_LOCK_INIT(unit);

    SHR_FUNC_INIT_VARS(unit);

    pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    port = BCM_COSQ_GPORT_FABRIC_PIPE_PORT_GET(gport);

    switch (state)
    {
        case bcmCosqDynamicStateNormal:
            weight_mode = soc_dnxf_cosq_weight_mode_regular;
            break;
        case bcmCosqDynamicStateQueue0Congested:
            weight_mode = soc_dnxf_cosq_weight_mode_dynamic_0;
            break;
        case bcmCosqDynamicStateQueue1Congested:
            weight_mode = soc_dnxf_cosq_weight_mode_dynamic_1;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid mode type parameter\n", unit);
            break;
    }

    DNXF_UNIT_LOCK_TAKE(unit);

    /*
     * dispatch according to type
     */
    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport))   /* ingress */
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cosq_pipe_weight_get,
                                              (unit, soc_dnxf_cosq_pipe_type_ingress, pipe, port, weight_mode,
                                               weight)));
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport))       /* middle */
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cosq_pipe_weight_get,
                                              (unit, soc_dnxf_cosq_pipe_type_middle, pipe, port, weight_mode, weight)));
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))       /* egress */
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cosq_pipe_weight_get,
                                              (unit, soc_dnxf_cosq_pipe_type_egress, pipe, port, weight_mode, weight)));
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport))      /* global */
    {
        /*
         * ingress settings, the assumption is that weight is the same for all fabric stages (rx,mid,tx)
         */
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cosq_pipe_weight_get,
                                              (unit, soc_dnxf_cosq_pipe_type_ingress, pipe, port, weight_mode,
                                               weight)));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport %d provided \n", gport);

    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
