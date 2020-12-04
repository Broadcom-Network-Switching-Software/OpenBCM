/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_GREYHOUND_SUPPORT)
#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/greyhound.h>
#include <bcm_int/esw_dispatch.h>

int
bcm_gh_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg)
{
    bcm_port_t port;
    int num_cosq;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_gh_cosq_control_set: unit=%d gport=0x%x cosq=%d \
                         type=%d arg=%d\n"),
              unit, gport, cosq, type, arg));

    if (soc_feature(unit, soc_feature_wh2)) {
        /* Get the configured max cosq number */
        BCM_IF_ERROR_RETURN(bcm_esw_cosq_config_get(unit, &num_cosq));
    } else {
        num_cosq = NUM_COS(unit);
    }

    if ((cosq < 0) || (cosq >= num_cosq)) {
        return BCM_E_PARAM;
    }

    switch (type) {
    case bcmCosqControlPFCBackpressureEnable:
        if (soc_feature(unit, soc_feature_priority_flow_control) &&
            soc_feature(unit, soc_feature_gh_style_pfc_config)) {
            uint32 rval;
            uint32 fval;
            if (!BCM_COSQ_QUEUE_VALID(unit, cosq)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, gport, &port));
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN(READ_MMU_FC_RX_ENr(unit, port, &rval));
            fval = soc_reg_field_get(unit, MMU_FC_RX_ENr, 
                                     rval, MMU_FC_RX_ENABLEf);
            if (arg) {
                fval |= (1 << cosq);
            } else {
                fval &= ~(1 << cosq);
            }
            soc_reg_field_set(unit, MMU_FC_RX_ENr, 
                              &rval, MMU_FC_RX_ENABLEf, fval);
            BCM_IF_ERROR_RETURN(WRITE_MMU_FC_RX_ENr(unit, port, rval));
            return BCM_E_NONE;
        }
        break;
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

int
bcm_gh_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int *arg)
{
    bcm_port_t port;
    int num_cosq;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_gh_cosq_control_get: unit=%d gport=0x%x cosq=%d type=%d\n"),
              unit, gport, cosq, type));

    if (soc_feature(unit, soc_feature_wh2)) {
        /* Get the configured max cosq number */
        BCM_IF_ERROR_RETURN(bcm_esw_cosq_config_get(unit, &num_cosq));
    } else {
        num_cosq = NUM_COS(unit);
    }

    if ((cosq < 0) || (cosq >= num_cosq)) {
        return BCM_E_PARAM;
    }

    switch (type) {
    case bcmCosqControlPFCBackpressureEnable:
        if (soc_feature(unit, soc_feature_priority_flow_control) &&
            soc_feature(unit, soc_feature_gh_style_pfc_config)) {
            uint32 value;
            if (!BCM_COSQ_QUEUE_VALID(unit, cosq)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, gport, &port));
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN(READ_MMU_FC_RX_ENr(unit, port, &value));
            if (value & (1 << cosq)) {
                *arg = 1;
            } else {
                *arg = 0;
            }
            return BCM_E_NONE;
        }
        break;
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

#endif /* BCM_GREYHOUND_SUPPORT */

