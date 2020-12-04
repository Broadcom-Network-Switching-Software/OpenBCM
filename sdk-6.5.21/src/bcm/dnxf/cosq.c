/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
/**********************************************************/\



/**********************************************************/
/*                  Internal functions                    */
/**********************************************************/
STATIC int
_bcm_dnxf_cosq_gport_pipe_dynamic_sched_set(int unit,
                                           bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           bcm_cosq_dynamic_state_t state,
                                           int weight);

STATIC int
_bcm_dnxf_cosq_gport_pipe_dynamic_sched_get(int unit,
                                           bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           bcm_cosq_dynamic_state_t state,
                                           int *weight);

/**********************************************************/
/*                  Implementation                        */
/**********************************************************/

int
bcm_dnxf_cosq_gport_handle_get(int unit,
                              bcm_cosq_gport_type_t gport_type,
                              bcm_cosq_gport_info_t *gport_info)
{
    shr_error_e rc = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    switch (gport_type) {
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
        default :
            LOG_ERROR(BSL_LS_BCM_COSQ,
                      (BSL_META_U(unit,
                                  "Invalid Handle type (%d) requested\n"), gport_type));
            rc = _SHR_E_PARAM;
            break;
    }

    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;    

}

int
bcm_dnxf_cosq_gport_sched_set(int unit,
                             bcm_gport_t gport,
                             bcm_cos_queue_t cosq,
                             int mode,
                             int weight)
{
    shr_error_e rc = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    rc = bcm_dnxf_cosq_gport_dynamic_sched_set(unit, gport, cosq, bcmCosqDynamicStateNormal, weight);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_sched_get(int unit,
                             bcm_gport_t gport,
                             bcm_cos_queue_t cosq,
                             int *mode,
                             int *weight)
{
    shr_error_e rc = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    rc = bcm_dnxf_cosq_gport_dynamic_sched_get(unit, gport, cosq, bcmCosqDynamicStateNormal, weight);
    SHR_IF_ERR_EXIT(rc);

    *mode = 0;

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_dynamic_sched_set(int unit,
                                     bcm_gport_t gport,
                                     bcm_cos_queue_t cosq,
                                     bcm_cosq_dynamic_state_t state,
                                     int weight)
{
    shr_error_e rc = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport)
        || BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        rc = _bcm_dnxf_cosq_gport_pipe_dynamic_sched_set(unit, gport, cosq, state, weight);
        SHR_IF_ERR_EXIT(rc);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid gport type (%d) requested\n", unit, gport);
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_dynamic_sched_get(int unit,
                                     bcm_gport_t gport,
                                     bcm_cos_queue_t cosq,
                                     bcm_cosq_dynamic_state_t state,
                                     int *weight)
{
    shr_error_e rc = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport)
        || BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        rc = _bcm_dnxf_cosq_gport_pipe_dynamic_sched_get(unit, gport, cosq, state, weight);
        SHR_IF_ERR_EXIT(rc);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid gport type (%d) requested\n", unit, gport);
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_threshold_set(int unit,
                                 bcm_gport_t gport,
                                 bcm_cos_queue_t cosq,
                                 bcm_cosq_threshold_t *threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid API. Please use bcm_fabric_profile_threshold_set().\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_threshold_get(int unit,
                                 bcm_gport_t gport,
                                 bcm_cos_queue_t cosq,
                                 bcm_cosq_threshold_t *threshold)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid API. Please use bcm_fabric_profile_threshold_get().\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_gport_bandwidth_set(int unit,
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
bcm_dnxf_cosq_gport_bandwidth_get(int unit,
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
bcm_dnxf_cosq_control_set(int unit,
                         bcm_gport_t port,
                         bcm_cos_queue_t cosq,
                         bcm_cosq_control_t type,
                         int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type) {
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid control type (%d) requested\n", unit, type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_cosq_control_get(int unit,
                         bcm_gport_t port,
                         bcm_cos_queue_t cosq,
                         bcm_cosq_control_t type,
                         int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (type) {
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid control type (%d) requested\n", unit, type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}


/* internal functions  */

STATIC int
_bcm_dnxf_cosq_gport_pipe_dynamic_sched_set(int unit,
                                           bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           bcm_cosq_dynamic_state_t state,
                                           int weight)
{
    int port, pipe;
    shr_error_e rc = _SHR_E_NONE;
    soc_dnxf_cosq_weight_mode_t weight_mode;
    int nof_pipes;
    DNXF_UNIT_LOCK_INIT(unit);

    SHR_FUNC_INIT_VARS(unit);

    pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    port = BCM_COSQ_GPORT_FABRIC_PIPE_PORT_GET(gport);
    nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);

    if (pipe > (nof_pipes -1) )
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid pipe id\n", unit);
    }

     switch (state) {
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

    /* dispatch according to type*/
    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport)) /* ingress */
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cosq_pipe_weight_set,(unit, soc_dnxf_cosq_pipe_type_ingress, pipe, port, weight_mode, weight));
        SHR_IF_ERR_EXIT(rc);

        /* RTP output stage settings */
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cosq_pipe_weight_set,(unit, soc_dnxf_cosq_pipe_type_rtp, pipe, port, weight_mode, weight));
        SHR_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport)) /* middle */
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cosq_pipe_weight_set,(unit, soc_dnxf_cosq_pipe_type_middle, pipe, port, weight_mode, weight));
        SHR_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport)) /* egress */
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cosq_pipe_weight_set,(unit, soc_dnxf_cosq_pipe_type_egress, pipe, port, weight_mode, weight));
        SHR_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport)) /* global */
    {
        /* ingress settings */
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cosq_pipe_weight_set,(unit, soc_dnxf_cosq_pipe_type_ingress, pipe, port, weight_mode, weight));
        SHR_IF_ERR_EXIT(rc);

        /* RTP output stage settings */
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cosq_pipe_weight_set,(unit, soc_dnxf_cosq_pipe_type_rtp, pipe, port, weight_mode, weight));
        SHR_IF_ERR_EXIT(rc);

        /* middle settings */
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cosq_pipe_weight_set,(unit, soc_dnxf_cosq_pipe_type_middle, pipe, port, weight_mode, weight));
        SHR_IF_ERR_EXIT(rc);

        /* egress settings */
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cosq_pipe_weight_set,(unit, soc_dnxf_cosq_pipe_type_egress, pipe, port, weight_mode, weight));
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

STATIC int
_bcm_dnxf_cosq_gport_pipe_dynamic_sched_get(int unit,
                                           bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           bcm_cosq_dynamic_state_t state,
                                           int *weight)
{
    int port, pipe;
    shr_error_e rc = _SHR_E_NONE;
    soc_dnxf_cosq_weight_mode_t weight_mode = soc_dnxf_cosq_weight_mode_regular;
    int nof_pipes;
    DNXF_UNIT_LOCK_INIT(unit);

    SHR_FUNC_INIT_VARS(unit);

    pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    port = BCM_COSQ_GPORT_FABRIC_PIPE_PORT_GET(gport);
    nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);

    if (pipe > (nof_pipes -1) )
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid pipe id\n", unit);
    }

     switch (state) {
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

    /* dispatch according to type*/
    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport)) /* ingress */
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cosq_pipe_weight_get,(unit, soc_dnxf_cosq_pipe_type_ingress, pipe, port, weight_mode, weight));
        SHR_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport)) /* middle */
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cosq_pipe_weight_get,(unit, soc_dnxf_cosq_pipe_type_middle, pipe, port, weight_mode, weight));
        SHR_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport)) /* egress */
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cosq_pipe_weight_get,(unit, soc_dnxf_cosq_pipe_type_egress, pipe, port, weight_mode, weight));
        SHR_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport)) /* global */
    {
        /* ingress settings, the assumption is that weight is the same for all fabric stages (rx,mid,tx)*/
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cosq_pipe_weight_get,(unit, soc_dnxf_cosq_pipe_type_ingress, pipe, port, weight_mode, weight));
        SHR_IF_ERR_EXIT(rc);             
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
    
