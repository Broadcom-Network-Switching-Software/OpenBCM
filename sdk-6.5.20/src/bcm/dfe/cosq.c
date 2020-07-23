/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE COSQ
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_COSQ
#include <shared/bsl.h>
#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dfe_dispatch.h>
#include <bcm/debug.h>
#include <bcm/cosq.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/mbcm.h>


/**********************************************************/
/*                  Verifications                         */
/**********************************************************/\



/**********************************************************/
/*                  Internal functions                    */
/**********************************************************/
STATIC int
_bcm_dfe_cosq_gport_pipe_dynamic_sched_set(int unit,
                                           bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           bcm_cosq_dynamic_state_t state,
                                           int weight);

STATIC int
_bcm_dfe_cosq_gport_pipe_dynamic_sched_get(int unit,
                                           bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           bcm_cosq_dynamic_state_t state,
                                           int *weight);

STATIC int
_bcm_dfe_cosq_gport_pipe_threshold_set(int unit,
                                       bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       bcm_cosq_threshold_t *threshold);

STATIC int
_bcm_dfe_cosq_gport_pipe_threshold_get(int unit,
                                       bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       bcm_cosq_threshold_t *threshold);

STATIC int
_bcm_dfe_cosq_gport_pipe_bandwidth_set(int unit,
                                       bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       uint32 kbits_sec_min,
                                       uint32 kbits_sec_max,
                                       uint32 flags);

STATIC int
_bcm_dfe_cosq_gport_pipe_bandwidth_get(int unit,
                                       bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       uint32 *kbits_sec_min,
                                       uint32 *kbits_sec_max,
                                       uint32 *flags);

STATIC int
_bcm_dfe_cosq_pipe_control_set(int unit,
                               bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_control_t type,
                               int arg);

STATIC int
_bcm_dfe_cosq_pipe_control_get(int unit,
                               bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_control_t type,
                               int *arg);
/**********************************************************/
/*                  Implementation                        */
/**********************************************************/

int
bcm_dfe_cosq_gport_handle_get(int unit,
                              bcm_cosq_gport_type_t gport_type,
                              bcm_cosq_gport_info_t *gport_info)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

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
            rc = BCM_E_PARAM;
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;    

}

int
bcm_dfe_cosq_gport_sched_set(int unit,
                             bcm_gport_t gport,
                             bcm_cos_queue_t cosq,
                             int mode,
                             int weight)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_dfe_cosq_gport_dynamic_sched_set(unit, gport, cosq, bcmCosqDynamicStateNormal, weight);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dfe_cosq_gport_sched_get(int unit,
                             bcm_gport_t gport,
                             bcm_cos_queue_t cosq,
                             int *mode,
                             int *weight)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_dfe_cosq_gport_dynamic_sched_get(unit, gport, cosq, bcmCosqDynamicStateNormal, weight);
    BCMDNX_IF_ERR_EXIT(rc);

    *mode = 0;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dfe_cosq_gport_dynamic_sched_set(int unit,
                                     bcm_gport_t gport,
                                     bcm_cos_queue_t cosq,
                                     bcm_cosq_dynamic_state_t state,
                                     int weight)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport)
        || BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        rc = _bcm_dfe_cosq_gport_pipe_dynamic_sched_set(unit, gport, cosq, state, weight);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid gport type (%d) requested\n"), unit, gport));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dfe_cosq_gport_dynamic_sched_get(int unit,
                                     bcm_gport_t gport,
                                     bcm_cos_queue_t cosq,
                                     bcm_cosq_dynamic_state_t state,
                                     int *weight)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport)
        || BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        rc = _bcm_dfe_cosq_gport_pipe_dynamic_sched_get(unit, gport, cosq, state, weight);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid gport type (%d) requested\n"), unit, gport));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dfe_cosq_gport_threshold_set(int unit,
                                 bcm_gport_t gport,
                                 bcm_cos_queue_t cosq,
                                 bcm_cosq_threshold_t *threshold)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (threshold->type ==  bcmCosqThresholdDynamicWeight) 
    {
        if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport) 
            || BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
        {
            rc = _bcm_dfe_cosq_gport_pipe_threshold_set(unit, gport, cosq, threshold);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid gport type (%d) requested\n"), unit, gport));
        }
    }
    else
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid threshold type parameter\n"), unit));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dfe_cosq_gport_threshold_get(int unit,
                                 bcm_gport_t gport,
                                 bcm_cos_queue_t cosq,
                                 bcm_cosq_threshold_t *threshold)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (threshold->type ==  bcmCosqThresholdDynamicWeight) 
    {
        if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport) 
            || BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
        {
            rc = _bcm_dfe_cosq_gport_pipe_threshold_get(unit, gport, cosq, threshold);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid gport type (%d) requested\n"), unit, gport));
        }
    }
    else
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid threshold type parameter\n"), unit));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dfe_cosq_gport_bandwidth_set(int unit,
                                 bcm_gport_t gport,
                                 bcm_cos_queue_t cosq,
                                 uint32 kbits_sec_min,
                                 uint32 kbits_sec_max,
                                 uint32 flags)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        rc = _bcm_dfe_cosq_gport_pipe_bandwidth_set(unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid gport type (%d) requested\n"), unit, gport));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dfe_cosq_gport_bandwidth_get(int unit,
                                 bcm_gport_t gport,
                                 bcm_cos_queue_t cosq,
                                 uint32 *kbits_sec_min,
                                 uint32 *kbits_sec_max,
                                 uint32 *flags)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport) || BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        rc = _bcm_dfe_cosq_gport_pipe_bandwidth_get(unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid gport type (%d) requested\n"), unit, gport));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dfe_cosq_control_set(int unit,
                         bcm_gport_t port,
                         bcm_cos_queue_t cosq,
                         bcm_cosq_control_t type,
                         int arg)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    switch (type) {
        case bcmCosqControlEgressBandwidthEnable:
            if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(port))
            {
                rc = _bcm_dfe_cosq_pipe_control_set(unit, port, cosq, type, arg);
                BCMDNX_IF_ERR_EXIT(rc);
            }
            else
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid gport type (%d) requested\n"), unit, port));
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid control type (%d) requested\n"), unit, type));
            break;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dfe_cosq_control_get(int unit,
                         bcm_gport_t port,
                         bcm_cos_queue_t cosq,
                         bcm_cosq_control_t type,
                         int *arg)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    switch (type) {
        case bcmCosqControlEgressBandwidthEnable:
            if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(port))
            {
                rc = _bcm_dfe_cosq_pipe_control_get(unit, port, cosq, type, arg);
                BCMDNX_IF_ERR_EXIT(rc);
            }
            else
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid gport type (%d) requested\n"), unit, port));
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid control type (%d) requested\n"), unit, type));
            break;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* internal functions  */

STATIC int
_bcm_dfe_cosq_gport_pipe_dynamic_sched_set(int unit,
                                           bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           bcm_cosq_dynamic_state_t state,
                                           int weight)
{
    int port, pipe;
    bcm_error_t rc = BCM_E_NONE;
    soc_dfe_cosq_weight_mode_t weight_mode;
    int nof_pipes;

    BCMDNX_INIT_FUNC_DEFS;

    pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    port = BCM_COSQ_GPORT_FABRIC_PIPE_PORT_GET(gport);
    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    if (pipe > (nof_pipes -1) )
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid pipe id\n"), unit));
    }

     switch (state) {
        case bcmCosqDynamicStateNormal:
            weight_mode = soc_dfe_cosq_weight_mode_regular;
            break;
        case bcmCosqDynamicStateQueue0Congested:
            weight_mode = soc_dfe_cosq_weight_mode_dynamic_0;
            break;
        case bcmCosqDynamicStateQueue1Congested:
            weight_mode = soc_dfe_cosq_weight_mode_dynamic_1;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid mode type parameter\n"), unit));
            break;
     }


    DFE_UNIT_LOCK_TAKE(unit);
    /* dispatch according to type*/
    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport)) /* ingress*/
    {
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_rx_weight_set,(unit, pipe, port, weight_mode, weight));
        BCMDNX_IF_ERR_EXIT(rc);
    }

    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport)) /* middle */
    {
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_mid_weight_set,(unit, pipe, port, weight_mode, weight));
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport)) /* egress */
    {
        if (port == -1) /* iterate over all fabric links */
        {
            PBMP_SFI_ITER(unit , port)
            {
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_tx_weight_set,(unit, pipe, port, weight_mode, weight));
                BCMDNX_IF_ERR_EXIT(rc);
            }
        }
        else /* specific port */
        {
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_tx_weight_set,(unit, pipe, port, weight_mode, weight));
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport)) /* global */
    {
        /* ingress settings*/
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_rx_weight_set,(unit, pipe, port, weight_mode, weight));
        BCMDNX_IF_ERR_EXIT(rc);

        /* middle settings */
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_mid_weight_set,(unit, pipe, port, weight_mode, weight));
        BCMDNX_IF_ERR_EXIT(rc);

        /* egress settings */
        PBMP_SFI_ITER(unit , port)
        {
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_tx_weight_set,(unit, pipe, port, weight_mode, weight));
            BCMDNX_IF_ERR_EXIT(rc);
        }                
    }

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dfe_cosq_gport_pipe_dynamic_sched_get(int unit,
                                           bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           bcm_cosq_dynamic_state_t state,
                                           int *weight)
{
    int port, pipe;
    bcm_error_t rc = BCM_E_NONE;
    soc_dfe_cosq_weight_mode_t weight_mode = soc_dfe_cosq_weight_mode_regular;
    int nof_pipes;

    BCMDNX_INIT_FUNC_DEFS;

    pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    port = BCM_COSQ_GPORT_FABRIC_PIPE_PORT_GET(gport);
    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    if (pipe > (nof_pipes -1) )
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid pipe id\n"), unit));
    }

     switch (state) {
        case bcmCosqDynamicStateNormal:
            weight_mode = soc_dfe_cosq_weight_mode_regular;
            break;
        case bcmCosqDynamicStateQueue0Congested:
            weight_mode = soc_dfe_cosq_weight_mode_dynamic_0;
            break;
        case bcmCosqDynamicStateQueue1Congested:
            weight_mode = soc_dfe_cosq_weight_mode_dynamic_1;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid mode type parameter\n"), unit));
            break;
     }

    DFE_UNIT_LOCK_TAKE(unit);
    /* dispatch according to type*/
    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport)) /* ingress*/
    {
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_rx_weight_get,(unit, pipe, port, weight_mode, weight));
        BCMDNX_IF_ERR_EXIT(rc);
    }

    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport)) /* middle */
    {
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_mid_weight_get,(unit, pipe, port, weight_mode, weight));
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport)) /* egress */
    {
        if (port == -1) /* iterate over all fabric links */
        {
            PBMP_SFI_ITER(unit , port)
            {
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_tx_weight_get,(unit, pipe, port, weight_mode, weight));
                BCMDNX_IF_ERR_EXIT(rc);
                /* the assumption is that weight is the same for all fabric links */
                break;
            }
        }
        else /* specific port */
        {
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_tx_weight_get,(unit, pipe, port, weight_mode, weight));
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE(gport)) /* global */
    {
        /* ingress settings, the assumption is that weight is the same for all fabric stages (rx,mid,tx)*/
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_rx_weight_get,(unit, pipe, port, weight_mode, weight));
        BCMDNX_IF_ERR_EXIT(rc);             
    }

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dfe_cosq_gport_pipe_threshold_set(int unit,
                                       bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       bcm_cosq_threshold_t *threshold)
{
    bcm_error_t rc = BCM_E_NONE;
    int port, pipe;
    int nof_pipes;

    BCMDNX_INIT_FUNC_DEFS;


    pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    port = BCM_COSQ_GPORT_FABRIC_PIPE_PORT_GET(gport);
    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;


    if (pipe > (nof_pipes -1) )
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid pipe id\n"), unit));
    }

    DFE_UNIT_LOCK_TAKE(unit);
    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport))
    {
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_rx_threshold_set,(unit, pipe, port, threshold->value));
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport))
    {
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_mid_threshold_set,(unit, pipe, port, threshold->value));
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_tx_threshold_set,(unit, pipe, port, threshold->value));
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dfe_cosq_gport_pipe_threshold_get(int unit,
                                       bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       bcm_cosq_threshold_t *threshold)
{
    bcm_error_t rc = BCM_E_NONE;
    int port, pipe;
    int nof_pipes;

    BCMDNX_INIT_FUNC_DEFS;


    pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    port = BCM_COSQ_GPORT_FABRIC_PIPE_PORT_GET(gport);
    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    if (pipe > (nof_pipes -1) )
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid pipe id\n"), unit));
    }

    DFE_UNIT_LOCK_TAKE(unit);
    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport))
    {
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_rx_threshold_get,(unit, pipe, port, &threshold->value));
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_MIDDLE(gport))
    {
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_mid_threshold_get,(unit, pipe, port, &threshold->value));
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_tx_threshold_get,(unit, pipe, port, &threshold->value));
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dfe_cosq_gport_pipe_bandwidth_set(int unit,
                                       bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       uint32 kbits_sec_min,
                                       uint32 kbits_sec_max,
                                       uint32 flags)
{
    bcm_error_t rc = BCM_E_NONE;
    int port, pipe;
    int nof_pipes;
    soc_dfe_cosq_shaper_mode_t shaper_mode;

    BCMDNX_INIT_FUNC_DEFS;


    pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    port = BCM_COSQ_GPORT_FABRIC_PIPE_PORT_GET(gport);
    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    if (cosq != -1)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid cosq id\n"), unit));
    }

    DFE_UNIT_LOCK_TAKE(unit);
    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport))
    {
        /* only shaping per cell should be used for ingress */
        if (flags != BCM_COSQ_BW_PACKET_MODE) 
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags parameter, only BCM_COSQ_BW_PACKET_MODE flag is allowed\n"), unit));
        }
        shaper_mode = soc_dfe_cosq_shaper_mode_packet;

        if (pipe > (nof_pipes -1) )
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid pipe id\n"), unit));
        }

        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_rx_rate_set,(unit, pipe, port, shaper_mode, kbits_sec_max));
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        if (flags == 0) 
        {
            shaper_mode = soc_dfe_cosq_shaper_mode_byte;
        }
        else if (flags == BCM_COSQ_BW_PACKET_MODE) 
        {
            shaper_mode = soc_dfe_cosq_shaper_mode_packet;
        }
        else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags parameter\n"), unit));
        }

        if (pipe != -1)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid pipe id\n"), unit));
        }

        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_tx_rate_set,(unit, port, shaper_mode, kbits_sec_max));
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dfe_cosq_gport_pipe_bandwidth_get(int unit,
                                       bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       uint32 *kbits_sec_min,
                                       uint32 *kbits_sec_max,
                                       uint32 *flags)
{
    bcm_error_t rc = BCM_E_NONE;
    int port, pipe;
    int nof_pipes;
    soc_dfe_cosq_shaper_mode_t shaper_mode;

    BCMDNX_INIT_FUNC_DEFS;


    pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    port = BCM_COSQ_GPORT_FABRIC_PIPE_PORT_GET(gport);
    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    if (cosq != -1)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid cosq id\n"), unit));
    }

    DFE_UNIT_LOCK_TAKE(unit);
    if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_INGRESS(gport))
    {

        if (pipe > (nof_pipes -1) )
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid pipe id\n"), unit));
        }

        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_rx_rate_get,(unit, pipe, port, &shaper_mode, kbits_sec_max));
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else if (BCM_COSQ_GPORT_IS_FABRIC_PIPE_EGRESS(gport))
    {
        if (pipe != -1)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid pipe id\n"), unit));
        }

        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_tx_rate_get,(unit, port, &shaper_mode, kbits_sec_max));
        BCMDNX_IF_ERR_EXIT(rc);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid gport\n"), unit));
    }

    *kbits_sec_min = 0;

    if (shaper_mode == soc_dfe_cosq_shaper_mode_packet)
    {
        *flags = BCM_COSQ_BW_PACKET_MODE;
    }
    else
    {
        *flags = 0;
    }

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dfe_cosq_pipe_control_set(int unit,
                               bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_control_t type,
                               int arg)
{
    bcm_error_t rc = BCM_E_NONE;
    int port, pipe;
    int nof_pipes;

    BCMDNX_INIT_FUNC_DEFS;


    pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    port = BCM_COSQ_GPORT_FABRIC_PIPE_PORT_GET(gport);
    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;


    if (pipe > (nof_pipes -1) )
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid pipe id\n"), unit));
    }

    DFE_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_tx_rate_enable_set,(unit, pipe, port, arg));
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dfe_cosq_pipe_control_get(int unit,
                               bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_control_t type,
                               int *arg)
{
    bcm_error_t rc = BCM_E_NONE;
    int port, pipe;
    int nof_pipes;

    BCMDNX_INIT_FUNC_DEFS;


    pipe = BCM_COSQ_GPORT_FABRIC_PIPE_PIPE_GET(gport);
    port = BCM_COSQ_GPORT_FABRIC_PIPE_PORT_GET(gport);
    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    if (pipe > (nof_pipes -1) )
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid pipe id\n"), unit));
    }

    DFE_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_tx_rate_enable_get,(unit, pipe, port, arg));
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}
#undef _ERR_MSG_MODULE_NAME
    
