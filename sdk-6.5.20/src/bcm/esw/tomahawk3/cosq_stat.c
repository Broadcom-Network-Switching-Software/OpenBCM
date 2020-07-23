/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Cos Counter get/set API implemenation for Tomahawk3
 */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/esw/pfc.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/tomahawk3.h>
#include <soc/feature.h>
#include <soc/drv.h>
#include <soc/types.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/debug.h>
#include <soc/tomahawk3.h>
#include <soc/dma.h>
#include <soc/mmu_config.h>
#include <soc/init/tomahawk3_idb_init.h>

/* Number of COSQs configured for this unit */
#define TH3_PORT_NUM_COS(unit, port) \
    ((IS_CPU_PORT(unit, port)) ? SOC_TH3_NUM_CPU_QUEUES : SOC_TH3_NUM_GP_QUEUES)


STATIC int
_bcm_th3_cosq_mmu_port_queue_counters_get(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 *value, int sync)
{
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg_uc, ctr_reg_mc;
    _bcm_th3_cosq_index_style_t style;
    int startq = 0, ci;
    bcm_port_t local_port = -1;
    uint64 sum, value64;
    soc_info_t *si = &SOC_INFO(unit);
    int num_uc = -1, num_mc = -1;
    int uc_q[2], mc_q[1];
    int loop_idx;

    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
    if (stat == bcmCosqStatOutBytes) {
        ctr_reg_uc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC;
        ctr_reg_mc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
    } else if (stat == bcmCosqStatOutPackets) {
        ctr_reg_uc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC;
        ctr_reg_mc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
    } else if (stat == bcmCosqStatUCOutPackets) {
        ctr_reg_uc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC;
        ctr_reg_mc = INVALIDr;
    } else if (stat == bcmCosqStatMCOutPackets) {
        ctr_reg_uc = INVALIDr;
        ctr_reg_mc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
    } else if (stat == bcmCosqStatUCOutBytes) {
        ctr_reg_uc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC;
        ctr_reg_mc = INVALIDr;
    } else if (stat == bcmCosqStatMCOutBytes) {
        ctr_reg_uc = INVALIDr;
        ctr_reg_mc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
    } else if (stat == bcmCosqStatDroppedBytes) {
        ctr_reg_uc = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC;
        ctr_reg_mc = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;
        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    } else if (stat == bcmCosqStatEgressCongestionDroppedPackets) {
        ctr_reg_uc = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC;
        ctr_reg_mc = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    } else { /* stat == bcmCosqStatTotalDiscardDroppedPackets */
        ctr_reg_uc = SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT;
        ctr_reg_mc = INVALIDr;
        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    }

    /* Use index style cos to get the UC-COS/MC cos offset */
    style = _BCM_TH3_COSQ_INDEX_STYLE_COS;
    BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_index_resolve
            (unit, port, cosq, style, &local_port, &startq, NULL));
    if ((cosq < -1) || (cosq >= TH3_PORT_NUM_COS(unit, local_port))) {
        return BCM_E_PARAM;
    }
    ctrl_info.instance = local_port;
    if (IS_CPU_PORT(unit, local_port)) {
        if (stat == bcmCosqStatTotalDiscardDroppedPackets) {
            return BCM_E_PORT;
        }
    }
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, ctr_reg_uc,
                ctrl_info, sync, startq, value));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, ctr_reg_mc,
                ctrl_info, sync, startq, value));
    } else if (BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN(
                _bcm_th3_cosq_sched_node_child_get(unit, port, &num_uc, uc_q,
                &num_mc, mc_q));
        if (num_uc > 2 || num_mc > 1) {
            return BCM_E_INTERNAL;
        }
        COMPILER_64_ZERO(sum);
        if (num_uc > 0) {
            for (loop_idx = 0; loop_idx < num_uc; loop_idx++) {
                if (ctr_reg_uc != INVALIDr) {
                    BCM_IF_ERROR_RETURN(
                        soc_counter_generic_get(unit, ctr_reg_uc,
                                                     ctrl_info,
                                                     sync, uc_q[loop_idx],
                                                     &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }

            }
        }
        if (num_mc > 0) {
            for (loop_idx = 0; loop_idx < num_mc; loop_idx++) {
                if (ctr_reg_mc != INVALIDr) {
                    BCM_IF_ERROR_RETURN(
                        soc_counter_generic_get(unit, ctr_reg_mc,
                                                     ctrl_info,
                                                     sync, mc_q[loop_idx],
                                                     &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }

            }
        }
        *value = sum;
    } else {
        if (cosq == -1) {
            /* Get UC values for NON CPU port */
            COMPILER_64_ZERO(sum);
            if (!IS_CPU_PORT(unit, local_port) &&
                    (stat != bcmCosqStatMCOutBytes) &&
                    (stat != bcmCosqStatMCOutPackets)) {
                for (ci = 0; ci < si->port_num_uc_cosq[local_port] ; ci++) {
                    if (ctr_reg_uc != INVALIDr) {
                        BCM_IF_ERROR_RETURN
                            (soc_counter_generic_get(unit, ctr_reg_uc,
                                                     ctrl_info,
                                                     sync, ci, &value64));
                        COMPILER_64_ADD_64(sum, value64);
                    }
                }
            }
            /* Get sum of MC values for any port*/
            if ((stat != bcmCosqStatUCOutBytes) &&
                    (stat != bcmCosqStatUCOutPackets) &&
                    (stat != bcmCosqStatTotalDiscardDroppedPackets)) {
                for (ci = 0; ci < si->port_num_cosq[local_port] ; ci++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_generic_get(unit, ctr_reg_mc,
                                          ctrl_info, sync, ci, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
            }
            *value = sum;
        } else {
            if (cosq < si->port_num_uc_cosq[local_port]) {
                /* If queue is unicast */
                if ((stat == bcmCosqStatMCOutBytes) ||
                    (stat == bcmCosqStatMCOutPackets)) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN
                            (soc_counter_generic_get(unit, ctr_reg_uc,
                                                     ctrl_info,
                                               sync, cosq, value));
            } else {
                /* Multicast queue */
                if ((stat == bcmCosqStatUCOutBytes) ||
                    (stat == bcmCosqStatUCOutPackets) ||
                    (stat == bcmCosqStatTotalDiscardDroppedPackets)) {
                    return BCM_E_PARAM;
                }
                ci = cosq - si->port_num_uc_cosq[local_port];
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_get(unit, ctr_reg_mc,
                                             ctrl_info,
                                             sync, ci, value));
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_mmu_port_queue_counters_set(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 value)
{
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg_uc, ctr_reg_mc;
    _bcm_th3_cosq_index_style_t style;
    int startq = 0, ci;
    bcm_port_t local_port = -1;
    soc_info_t *si = &SOC_INFO(unit);
    int num_uc = -1, num_mc = -1;
    int uc_q[2], mc_q[1];
    int loop_idx;

    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;

    if (stat == bcmCosqStatOutBytes) {
        ctr_reg_uc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC;
        ctr_reg_mc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
    } else if (stat == bcmCosqStatOutPackets) {
        ctr_reg_uc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC;
        ctr_reg_mc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
    } else if (stat == bcmCosqStatUCOutPackets) {
        ctr_reg_uc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC;
        ctr_reg_mc = INVALIDr;
    } else if (stat == bcmCosqStatMCOutPackets) {
        ctr_reg_uc = INVALIDr;
        ctr_reg_mc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
    } else if (stat == bcmCosqStatUCOutBytes) {
        ctr_reg_uc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC;
        ctr_reg_mc = INVALIDr;
    } else if (stat == bcmCosqStatMCOutBytes) {
        ctr_reg_uc = INVALIDr;
        ctr_reg_mc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
    } else if (stat == bcmCosqStatDroppedBytes) {
        ctr_reg_uc = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC;
        ctr_reg_mc = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;
        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    } else if (stat == bcmCosqStatEgressCongestionDroppedPackets) {
        ctr_reg_uc = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC;
        ctr_reg_mc = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    } else { /* stat == bcmCosqStatTotalDiscardDroppedPackets */
        ctr_reg_uc = SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT;
        ctr_reg_mc = INVALIDr;
        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    }

    /* Use index style cos to get the UC-COS/MC cos offset */
    style = _BCM_TH3_COSQ_INDEX_STYLE_COS;
    BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_index_resolve
            (unit, port, cosq, style, &local_port, &startq, NULL));
    if ((cosq < -1) || (cosq >= TH3_PORT_NUM_COS(unit, local_port))) {
        return BCM_E_PARAM;
    }
    ctrl_info.instance = local_port;
    if (IS_CPU_PORT(unit, local_port)) {
        if (stat == bcmCosqStatTotalDiscardDroppedPackets) {
            return BCM_E_PORT;
        }
    }
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_set(unit, ctr_reg_uc,
                ctrl_info, 0, startq, value));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_set(unit, ctr_reg_mc,
                ctrl_info, 0, startq, value));
    } else if (BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_th3_cosq_sched_node_child_get(unit, port, &num_uc, uc_q,
                &num_mc, mc_q));
        if (num_uc > 2 || num_mc > 1) {
            return BCM_E_INTERNAL;
        }
        if (num_uc > 0) {
            for (loop_idx = 0; loop_idx < num_uc; loop_idx++) {
                if (ctr_reg_uc != INVALIDr) {
                    BCM_IF_ERROR_RETURN(
                        soc_counter_generic_set(unit, ctr_reg_uc,
                                                     ctrl_info, 0,
                                                     uc_q[loop_idx],
                                                     value));
                }

            }
        }
        if (num_mc > 0) {
            for (loop_idx = 0; loop_idx < num_mc; loop_idx++) {
                if (ctr_reg_mc != INVALIDr) {
                    BCM_IF_ERROR_RETURN(
                        soc_counter_generic_set(unit, ctr_reg_mc,
                                                     ctrl_info, 0,
                                                     mc_q[loop_idx],
                                                     value));
                }

            }
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));
        ctrl_info.instance = local_port;
        if (cosq == -1) {
            /* Get UC values for NON CPU port */
            if (!IS_CPU_PORT(unit, local_port) &&
                    (stat != bcmCosqStatMCOutBytes) &&
                    (stat != bcmCosqStatMCOutPackets)) {
                for (ci = 0; ci < si->port_num_uc_cosq[local_port] ; ci++) {
                    if (ctr_reg_uc != INVALIDr) {
                        BCM_IF_ERROR_RETURN
                            (soc_counter_generic_set(unit, ctr_reg_uc,
                                                     ctrl_info, 0,
                                                     ci, value));
                    }
                }
            }
            if ((stat != bcmCosqStatUCOutBytes) &&
                (stat != bcmCosqStatUCOutPackets) &&
                (stat != bcmCosqStatTotalDiscardDroppedPackets)) {
                for (ci = 0; ci < si->port_num_cosq[local_port] ; ci++) {
                    if (ctr_reg_mc != INVALIDr) {
                        BCM_IF_ERROR_RETURN
                            (soc_counter_generic_set(unit, ctr_reg_mc,
                                          ctrl_info, 0, ci, value));
                    }
                }
            }
        } else {
            if ((!IS_CPU_PORT(unit, local_port)) && (cosq <
                        si->port_num_uc_cosq[local_port])) {
                if ((stat == bcmCosqStatMCOutBytes) ||
                    (stat == bcmCosqStatMCOutPackets)) {
                    return BCM_E_PARAM;
                }
                /* If queue is unicast */
                BCM_IF_ERROR_RETURN
                            (soc_counter_generic_set(unit, ctr_reg_uc,
                                                     ctrl_info,
                                               0, cosq, value));
            } else {
                /* Multicast queue */
                if ((stat == bcmCosqStatUCOutBytes) ||
                    (stat == bcmCosqStatUCOutPackets) ||
                    (stat == bcmCosqStatTotalDiscardDroppedPackets)) {
                    return BCM_E_PARAM;
                }

                ci = cosq - si->port_num_uc_cosq[local_port];
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_set(unit, ctr_reg_mc,
                                             ctrl_info,
                                             0, ci, value));
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_mmu_port_queue_total_drop_counters_get(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 *value, int sync)
{
    soc_ctr_control_info_t ctrl_info;
    _bcm_th3_cosq_index_style_t style;
    int startq = 0, ci;
    bcm_port_t local_port = -1;
    uint64 sum, value64;
    soc_info_t *si = &SOC_INFO(unit);
    int num_uc = -1, num_mc = -1;
    int uc_q[2], mc_q[1];
    int loop_idx;

    /* Use index style cos to get the UC-COS/MC cos offset */
    style = _BCM_TH3_COSQ_INDEX_STYLE_COS;
    BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_index_resolve
            (unit, port, cosq, style, &local_port, &startq, NULL));
    if ((cosq < -1) || (cosq >= TH3_PORT_NUM_COS(unit, local_port))) {
        return BCM_E_PARAM;
    }
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    ctrl_info.instance = local_port;
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
        COMPILER_64_ZERO(sum);
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC,
                ctrl_info, sync, startq, &value64));
        COMPILER_64_ADD_64(sum, value64);
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT,
                ctrl_info, sync, startq, &value64));
        COMPILER_64_ADD_64(sum, value64);
        *value = sum;
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,
                ctrl_info, sync, startq, value));
    } else if (BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_th3_cosq_sched_node_child_get(unit, port, &num_uc, uc_q,
                &num_mc, mc_q));
        if (num_uc > 2 || num_mc > 1) {
            return BCM_E_INTERNAL;
        }
        COMPILER_64_ZERO(sum);
        if (num_uc > 0) {
            for (loop_idx = 0; loop_idx < num_uc; loop_idx++) {
                BCM_IF_ERROR_RETURN(
                    soc_counter_generic_get(unit, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC,
                                                 ctrl_info,
                                                 sync, uc_q[loop_idx],
                                                 &value64));
                COMPILER_64_ADD_64(sum, value64);
                BCM_IF_ERROR_RETURN(
                    soc_counter_generic_get(unit, SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT,
                                                 ctrl_info,
                                                 sync, uc_q[loop_idx],
                                                 &value64));
                COMPILER_64_ADD_64(sum, value64);
            }

        }
        if (num_mc > 0) {
            for (loop_idx = 0; loop_idx < num_mc; loop_idx++) {
                BCM_IF_ERROR_RETURN(
                    soc_counter_generic_get(unit, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,
                                                 ctrl_info,
                                                 sync, mc_q[loop_idx],
                                                 &value64));
                COMPILER_64_ADD_64(sum, value64);

            }
        }
        *value = sum;
    } else {
        if (cosq == -1) {
            /* Get UC values for NON CPU port */
            COMPILER_64_ZERO(sum);
            if (!IS_CPU_PORT(unit, local_port)) {
                for (ci = 0; ci < si->port_num_uc_cosq[local_port] ; ci++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_generic_get(unit, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC,
                                                 ctrl_info,
                                                 sync, ci, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                    BCM_IF_ERROR_RETURN
                        (soc_counter_generic_get(unit, SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT,
                                                 ctrl_info,
                                                 sync, ci, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
            }
            /* Get sum of MC values for any port*/
            for (ci = 0; ci < si->port_num_cosq[local_port] ; ci++) {
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_get(unit, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,
                                      ctrl_info, sync, ci, &value64));
                COMPILER_64_ADD_64(sum, value64);
            }
            *value = sum;
        } else {
            if (cosq < si->port_num_uc_cosq[local_port]) {
                /* If queue is unicast */
                COMPILER_64_ZERO(sum);
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_get(unit,
                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC, ctrl_info, sync,
                     cosq, &value64));
                COMPILER_64_ADD_64(sum, value64);
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_get(unit,
                    SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT, ctrl_info, sync,
                    cosq, &value64));
                COMPILER_64_ADD_64(sum, value64);
                *value = sum;
            } else {
                /* Multicast queue */
                ci = cosq - si->port_num_uc_cosq[local_port];
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_get(unit, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,
                                             ctrl_info,
                                             sync, ci, value));
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_mmu_port_queue_total_drop_counters_set(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 value)
{
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg_uc0, ctr_reg_mc, ctr_reg_uc1;
    _bcm_th3_cosq_index_style_t style;
    int startq = 0, ci;
    bcm_port_t local_port = -1;
    soc_info_t *si = &SOC_INFO(unit);
    int num_uc = -1, num_mc = -1;
    int uc_q[2], mc_q[1];
    int loop_idx;

    ctr_reg_uc0 = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC;
    ctr_reg_uc1 = SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT;
    ctr_reg_mc = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;

    /* Use index style cos to get the UC-COS/MC cos offset */
    style = _BCM_TH3_COSQ_INDEX_STYLE_COS;
    BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_index_resolve
            (unit, port, cosq, style, &local_port, &startq, NULL));
    if ((cosq < -1) || (cosq >= TH3_PORT_NUM_COS(unit, local_port))) {
        return BCM_E_PARAM;
    }
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    ctrl_info.instance = local_port;
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_set(unit, ctr_reg_uc0,
                ctrl_info, 0, startq, value));
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_set(unit, ctr_reg_uc1,
                ctrl_info, 0, startq, value));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_set(unit, ctr_reg_mc,
                ctrl_info, 0, startq, value));
    } else if (BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_th3_cosq_sched_node_child_get(unit, port, &num_uc, uc_q,
                &num_mc, mc_q));
        if (num_uc > 2 || num_mc > 1) {
            return BCM_E_INTERNAL;
        }
        if (num_uc > 0) {
            for (loop_idx = 0; loop_idx < num_uc; loop_idx++) {
                BCM_IF_ERROR_RETURN(
                    soc_counter_generic_set(unit, ctr_reg_uc0,
                                                 ctrl_info, 0,
                                                 uc_q[loop_idx],
                                                 value));
                BCM_IF_ERROR_RETURN(
                    soc_counter_generic_set(unit, ctr_reg_uc1,
                                                 ctrl_info, 0,
                                                 uc_q[loop_idx],
                                                 value));
            }
        }
        if (num_mc > 0) {
            for (loop_idx = 0; loop_idx < num_mc; loop_idx++) {
                if (ctr_reg_mc != INVALIDr) {
                    BCM_IF_ERROR_RETURN(
                        soc_counter_generic_set(unit, ctr_reg_mc,
                                                     ctrl_info, 0,
                                                     mc_q[loop_idx],
                                                     value));
                }

            }
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));
        ctrl_info.instance = local_port;
        if (cosq == -1) {
            /* Get UC values for NON CPU port */
            if (!IS_CPU_PORT(unit, local_port)) {
                for (ci = 0; ci < si->port_num_uc_cosq[local_port] ; ci++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_generic_set(unit, ctr_reg_uc0,
                                                 ctrl_info, 0,
                                                 ci, value));
                    BCM_IF_ERROR_RETURN
                        (soc_counter_generic_set(unit, ctr_reg_uc1,
                                                 ctrl_info, 0,
                                                 ci, value));
                }
            }
            for (ci = 0; ci < si->port_num_cosq[local_port] ; ci++) {
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_set(unit, ctr_reg_mc,
                                  ctrl_info, 0, ci, value));
            }
        } else {
            if (cosq < si->port_num_uc_cosq[local_port]) {
                /* If queue is unicast */
                BCM_IF_ERROR_RETURN
                            (soc_counter_generic_set(unit, ctr_reg_uc0,
                                                     ctrl_info,
                                               0, cosq, value));
                BCM_IF_ERROR_RETURN
                            (soc_counter_generic_set(unit, ctr_reg_uc1,
                                                     ctrl_info,
                                               0, cosq, value));
            } else {
                /* Multicast queue */
                ci = cosq - si->port_num_uc_cosq[local_port];
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_set(unit, ctr_reg_mc,
                                             ctrl_info,
                                             0, ci, value));
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_obm_port_counters_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, uint64 *value, int sync) {
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg;
    int local_port;
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        /* OBM port counters not supported at queue/scheduler level */
        return BCM_E_PARAM;
    }
    if (cosq != -1) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));
    if ((stat == bcmCosqStatHighPriDroppedPackets) ||
        (stat == bcmCosqStatOBMLossyHighPriDroppedPackets)) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI;
    } else if ((stat == bcmCosqStatLowPriDroppedPackets) ||
               (stat == bcmCosqStatOBMLossyLowPriDroppedPackets)) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO;
    } else if ((stat == bcmCosqStatHighPriDroppedBytes) ||
               (stat == bcmCosqStatOBMLossyHighPriDroppedBytes)) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI;
    } else if ((stat == bcmCosqStatLowPriDroppedBytes) ||
               (stat == bcmCosqStatOBMLossyLowPriDroppedBytes)) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO;
    } else if (stat == bcmCosqStatOBMLossless0DroppedPackets) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0;
    } else if (stat == bcmCosqStatOBMLossless1DroppedPackets) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1;
    } else if (stat == bcmCosqStatOBMLossless0DroppedBytes) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0;
    } else if (stat == bcmCosqStatOBMLossless1DroppedBytes) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1;
    } else { /* (stat == bcmCosqStatOBMFlowControlEvents)  */
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS;
    }
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
    ctrl_info.instance = local_port;
    /* Always pass sync mode 0 for OBM counters*/
    BCM_IF_ERROR_RETURN
        (soc_counter_generic_get(unit, ctr_reg, ctrl_info, 0,
                                 0, value));
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_obm_port_counters_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, uint64 value) {
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg;
    int local_port;
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if (cosq != -1) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));
    if ((stat == bcmCosqStatHighPriDroppedPackets) ||
        (stat == bcmCosqStatOBMLossyHighPriDroppedPackets)) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI;
    } else if ((stat == bcmCosqStatLowPriDroppedPackets) ||
               (stat == bcmCosqStatOBMLossyLowPriDroppedPackets)) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO;
    } else if ((stat == bcmCosqStatHighPriDroppedBytes) ||
               (stat == bcmCosqStatOBMLossyHighPriDroppedBytes)) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI;
    } else if ((stat == bcmCosqStatLowPriDroppedBytes) ||
               (stat == bcmCosqStatOBMLossyLowPriDroppedBytes)) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO;
    } else if (stat == bcmCosqStatOBMLossless0DroppedPackets) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0;
    } else if (stat == bcmCosqStatOBMLossless1DroppedPackets) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1;
    } else if (stat == bcmCosqStatOBMLossless0DroppedBytes) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0;
    } else if (stat == bcmCosqStatOBMLossless1DroppedBytes) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1;
    } else { /* (stat == bcmCosqStatOBMFlowControlEvents)  */
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS;
    }
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
    ctrl_info.instance = local_port;
    BCM_IF_ERROR_RETURN
        (soc_counter_generic_set(unit, ctr_reg, ctrl_info, 0,
                                 0, value));
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_mmu_port_queue_wred_color_drop_get(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 *value, int sync) {
    soc_ctr_control_info_t ctrl_info;
    int startq = 0, ci;
    bcm_port_t local_port = -1;
    uint64 sum, value64;
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t ctr_reg;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if (cosq != BCM_COS_INVALID) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));

    ctrl_info.instance = local_port;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    if (stat == bcmCosqStatGreenDiscardDroppedPackets) {
        /*Sum of complete WRED dropped - red WRED - yellow WRED*/
        COMPILER_64_ZERO(sum);
        for (ci = 0; ci < si->port_num_uc_cosq[local_port]; ci++) {
            BCM_IF_ERROR_RETURN
                (soc_counter_generic_get(unit,
                 SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT, ctrl_info, sync,
                 ci, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW,
                                 ctrl_info, sync, startq, &value64));
        COMPILER_64_SUB_64(sum, value64);
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED,
                                 ctrl_info, sync, startq, &value64));
        COMPILER_64_SUB_64(sum, value64);
        *value = sum;
        return BCM_E_NONE;
    }

    if (stat == bcmCosqStatYellowDiscardDroppedPackets) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW;
    } else { /*(stat == bcmCosqStatRedDiscardDroppedPackets) */
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED;
    }

    BCM_IF_ERROR_RETURN
        (soc_counter_generic_get(unit, ctr_reg, ctrl_info,
                                 sync, startq, value));


    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_mmu_port_queue_wred_color_drop_set(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 value) {
    soc_ctr_control_info_t ctrl_info;
    int startq = 0;
    bcm_port_t local_port = -1;
    soc_reg_t ctr_reg;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if (cosq != BCM_COS_INVALID) {
        return BCM_E_PARAM;
    }
    if (stat == bcmCosqStatGreenDiscardDroppedPackets) {
        /* Green Discard drop is combination of complete drops and red/yellow
         * discard. All the elements can be set using other stat Enums */
        return BCM_E_UNAVAIL;
     }
    if (stat == bcmCosqStatYellowDiscardDroppedPackets) {
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW;
    } else { /*(stat == bcmCosqStatRedDiscardDroppedPackets) */
        ctr_reg = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));

    ctrl_info.instance = local_port;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    BCM_IF_ERROR_RETURN
        (soc_counter_generic_set(unit, ctr_reg, ctrl_info,
                                 0, startq, value));

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_mmu_port_queue_color_drop_get(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 *value, int sync) {
    soc_ctr_control_info_t ctrl_info;
    int startq = 0;
    bcm_port_t local_port = -1;
    uint64 sum, value64;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if (cosq != BCM_COS_INVALID) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));

    ctrl_info.instance = local_port;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;

    if (stat == bcmCosqStatYellowCongestionDroppedPackets) {
        /*Sum of UC yellow drops and MC yellow drops*/
        COMPILER_64_ZERO(sum);
        /* MMU_THDO_PORT_DROP_COUNT_MCm.PORT_PACKET_YELLOW_DROPf */
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit,
             SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW, ctrl_info, sync,
             startq, &value64));
        COMPILER_64_ADD_64(sum, value64);
        /* MMU_THDO_PORT_DROP_COUNT_UCm.PORT_PACKET_YELLOW_DROPf */
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit,
             SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW_UC, ctrl_info, sync,
             startq, &value64));
        COMPILER_64_ADD_64(sum, value64);

        *value = sum;
    } else if (stat == bcmCosqStatRedCongestionDroppedPackets) {
        /*Sum of UC red drops and MC red drops*/
        COMPILER_64_ZERO(sum);
        /* MMU_THDO_PORT_DROP_COUNT_MCm.PORT_PACKET_RED_DROPf */
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit,
             SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED, ctrl_info, sync,
             startq, &value64));
        COMPILER_64_ADD_64(sum, value64);
        /* MMU_THDO_PORT_DROP_COUNT_UCm.PORT_PACKET_RED_DROPf */
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit,
             SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED_UC, ctrl_info, sync,
             startq, &value64));
        COMPILER_64_ADD_64(sum, value64);
        *value = sum;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_mmu_port_queue_color_drop_set(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 value) {
    soc_ctr_control_info_t ctrl_info;
    int startq = 0;
    bcm_port_t local_port = -1;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if (cosq != BCM_COS_INVALID) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));

    ctrl_info.instance = local_port;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    if (stat == bcmCosqStatYellowCongestionDroppedPackets) {
        BCM_IF_ERROR_RETURN
        (soc_counter_generic_set(unit, SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW,
                                 ctrl_info, 0, startq, value));
        BCM_IF_ERROR_RETURN
        (soc_counter_generic_set(unit,
                                 SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW_UC,
                                 ctrl_info, 0, startq, value));
    } else if (stat == bcmCosqStatRedCongestionDroppedPackets) {
        BCM_IF_ERROR_RETURN
        (soc_counter_generic_set(unit, SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED,
                                 ctrl_info, 0, startq, value));
        BCM_IF_ERROR_RETURN
        (soc_counter_generic_set(unit, SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED_UC,
                                 ctrl_info, 0, startq, value));

    }
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_queue_current_usage_bytes_get(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 *value, int sync) {
    _bcm_th3_cosq_index_style_t style;
    bcm_port_t local_port = -1;
    soc_info_t *si = &SOC_INFO(unit);
    soc_mem_t mem = INVALIDm;
    int itm, chip_q_num;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 fld_value = 0, itm_map;
    uint64 val64;
    soc_field_t field;

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));
    if ((cosq < 0) || (cosq >= TH3_PORT_NUM_COS(unit, local_port))) {
        return BCM_E_PARAM;
    }

    if (stat == bcmCosqStatEgressMCQueueBytesCurrent ||
        stat == bcmCosqStatEgressMCQueueMinBytesCurrent) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_SCHEDULER(port)) {
            return BCM_E_PARAM;
        }
        style = _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE;
    } else {
        /*(stat == bcmCosqStatEgressUCQueueBytesCurrent ||
         * stat == bcmCosqStatEgressUCQueueMinBytesCurrent)
         */
        if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_SCHEDULER(port)) {
            return BCM_E_PARAM;
        }
        style = _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE;
    }

    if (stat == bcmCosqStatEgressMCQueueBytesCurrent ||
        stat == bcmCosqStatEgressUCQueueBytesCurrent ) {
        field = TOTALCOUNTf;
    } else {
        /*Queue Min Bytes enums*/
        field = MIN_COUNTf;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_index_resolve
        (unit, port, cosq, style, &local_port, &chip_q_num, NULL));

    itm_map = si->itm_map;
    COMPILER_64_ZERO(val64);
    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        if (itm_map & (1 << itm)) {
            mem = SOC_MEM_UNIQUE_ACC_ITM(unit, MMU_THDO_COUNTER_QUEUEm, itm);
            if (mem != INVALIDm) {
                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL, chip_q_num, entry));
                fld_value = soc_mem_field32_get(unit, mem, entry, field);
                COMPILER_64_ADD_32(val64, fld_value);
            }
        }
    }
    *value = val64;
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_pg_current_usage_bytes_get(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 *value, int sync) {
    bcm_port_t local_port = -1;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t field = INVALIDf;
    soc_field_t fld_pg_min[_TH3_MMU_NUM_PG] = {PG0_MIN_COUNTf, PG1_MIN_COUNTf,
        PG2_MIN_COUNTf, PG3_MIN_COUNTf, PG4_MIN_COUNTf, PG5_MIN_COUNTf,
        PG6_MIN_COUNTf, PG7_MIN_COUNTf};
    soc_field_t fld_pg_shr[_TH3_MMU_NUM_PG] = {PG0_SHARED_COUNTf, PG1_SHARED_COUNTf,
        PG2_SHARED_COUNTf, PG3_SHARED_COUNTf, PG4_SHARED_COUNTf, PG5_SHARED_COUNTf,
        PG6_SHARED_COUNTf, PG7_SHARED_COUNTf};
    soc_field_t fld_pg_hdrm[_TH3_MMU_NUM_PG] = {PG0_HDRM_COUNTf, PG1_HDRM_COUNTf,
        PG2_HDRM_COUNTf, PG3_HDRM_COUNTf, PG4_HDRM_COUNTf, PG5_HDRM_COUNTf,
        PG6_HDRM_COUNTf, PG7_HDRM_COUNTf};
    int mmu_local_port, pipe;
    uint32 fld_value;


    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if ((cosq < 0) || (cosq >= _TH3_MMU_NUM_PG)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));
    mmu_local_port = SOC_TH3_MMU_LOCAL_PORT(unit, local_port);

    BCM_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));
    if (stat == bcmCosqStatIngressPortPGMinBytesCurrent) {
        mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_THDI_PORT_PG_MIN_COUNTERm,
                pipe);
        field = fld_pg_min[cosq];
    } else if (stat == bcmCosqStatIngressPortPGSharedBytesCurrent) {
        mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_THDI_PORT_PG_SHARED_COUNTERm,
                pipe);
        field = fld_pg_shr[cosq];
    } else {/* (stat == bcmCosqStatIngressPortPGHeadroomBytesCurrent) */
        mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_THDI_PORT_PG_HDRM_COUNTERm,
                pipe);
        field = fld_pg_hdrm[cosq];
    }

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, mmu_local_port, entry));
    fld_value = soc_mem_field32_get(unit, mem, entry, field);
    COMPILER_64_SET(*value, 0x0, fld_value);

    return BCM_E_NONE;
}


STATIC int
_bcm_th3_sp_current_usage_bytes_get(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 *value, int sync) {
    bcm_port_t local_port = -1;
    int pool = -1;
    soc_field_t sp_shr[_TH3_MMU_NUM_POOL] = {PORTSP0_SHARED_COUNTf,
        PORTSP1_SHARED_COUNTf, PORTSP2_SHARED_COUNTf, PORTSP3_SHARED_COUNTf};
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    int pipe, mmu_local_port;
    uint32 fld_value;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if ((cosq < 0) || (cosq >= _TH3_MMU_NUM_PG)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));
    BCM_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));
    mmu_local_port = SOC_TH3_MMU_LOCAL_PORT(unit, local_port);

    /* Retrieve Ing service pool mapped to {port, PG}*/
    _bcm_th3_cosq_ingress_sp_get_by_pg(unit, port, cosq, &pool);
    if (pool == -1) {
        return BCM_E_INTERNAL;
    }
    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_THDI_PORTSP_COUNTERm, pipe);
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, mmu_local_port, entry));
    fld_value = soc_mem_field32_get(unit, mem, entry, sp_shr[pool]);
    COMPILER_64_SET(*value, 0x0, fld_value);

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_obmhighwatermark_set(int unit, bcm_port_t port, uint64 value)
{
    int pipe, obm_id, subp, pport, reg_id;
    soc_reg_t reg;
    uint64 rval64;
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t obm_high_watermark_reg[_TH3_PBLKS_PER_PIPE][3] = {
        {IDB_OBM0_MAX_USAGEr, IDB_OBM0_MAX_USAGE_1r, IDB_OBM0_MAX_USAGE_2r},
        {IDB_OBM1_MAX_USAGEr, IDB_OBM1_MAX_USAGE_1r, IDB_OBM1_MAX_USAGE_2r},
        {IDB_OBM2_MAX_USAGEr, IDB_OBM2_MAX_USAGE_1r, IDB_OBM2_MAX_USAGE_2r},
        {IDB_OBM3_MAX_USAGEr, IDB_OBM3_MAX_USAGE_1r, IDB_OBM3_MAX_USAGE_2r},
    };

    soc_field_t obm_high_watermark_fields[] = {
        PORT0_MAX_USAGEf, PORT1_MAX_USAGEf,
        PORT2_MAX_USAGEf, PORT3_MAX_USAGEf,
        PORT4_MAX_USAGEf, PORT5_MAX_USAGEf,
        PORT6_MAX_USAGEf, PORT7_MAX_USAGEf
    };
    pport = si->port_l2p_mapping[port];
    obm_id = soc_tomahawk3_get_pipe_pm_from_phy_port(pport);
    subp = soc_tomahawk3_get_subp_from_phy_port(pport);
    reg_id = subp / 3; /*upto 3 subports in each of usage, usage_1 and usage_2
                         regs*/
    if (obm_id >= _TH3_PBLKS_PER_PIPE || (subp < 0 || subp >= _TH3_PORTS_PER_PBLK)) {
            return BCM_E_PARAM;
    }
    if(!COMPILER_64_IS_ZERO(value)) {
        return BCM_E_PARAM;
    }
    pipe = si->port_pipe[port];

    reg = SOC_REG_UNIQUE_ACC(unit, obm_high_watermark_reg[obm_id][reg_id])[pipe];
    COMPILER_64_ZERO(rval64);
    BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval64));

    soc_reg64_field32_set(unit, reg, &rval64, obm_high_watermark_fields[subp], 0);
    BCM_IF_ERROR_RETURN
            (soc_reg64_set(unit, reg, REG_PORT_ANY, 0, rval64));
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_obmhighwatermark_get(int unit, bcm_port_t port, uint64 *value)
{
    int pipe, obm_id, subp, pport, reg_id;
    soc_reg_t reg;
    uint64 rval64;
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t obm_high_watermark_reg[_TH3_PBLKS_PER_PIPE][3] = {
        {IDB_OBM0_MAX_USAGEr, IDB_OBM0_MAX_USAGE_1r, IDB_OBM0_MAX_USAGE_2r},
        {IDB_OBM1_MAX_USAGEr, IDB_OBM1_MAX_USAGE_1r, IDB_OBM1_MAX_USAGE_2r},
        {IDB_OBM2_MAX_USAGEr, IDB_OBM2_MAX_USAGE_1r, IDB_OBM2_MAX_USAGE_2r},
        {IDB_OBM3_MAX_USAGEr, IDB_OBM3_MAX_USAGE_1r, IDB_OBM3_MAX_USAGE_2r},
    };

    soc_field_t obm_high_watermark_fields[] = {
        PORT0_MAX_USAGEf, PORT1_MAX_USAGEf,
        PORT2_MAX_USAGEf, PORT3_MAX_USAGEf,
        PORT4_MAX_USAGEf, PORT5_MAX_USAGEf,
        PORT6_MAX_USAGEf, PORT7_MAX_USAGEf
    };
    pport = si->port_l2p_mapping[port];
    obm_id = soc_tomahawk3_get_pipe_pm_from_phy_port(pport);
    subp = soc_tomahawk3_get_subp_from_phy_port(pport);
    reg_id = subp / 3; /*upto 3 subports in each of usage, usage_1 and usage_2
                         regs*/
    if (obm_id >= _TH3_PBLKS_PER_PIPE || (subp < 0 || subp >= _TH3_PORTS_PER_PBLK)) {
            return BCM_E_PARAM;
    }
    pipe = si->port_pipe[port];

    reg = SOC_REG_UNIQUE_ACC(unit, obm_high_watermark_reg[obm_id][reg_id])[pipe];
    COMPILER_64_ZERO(rval64);
    BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval64));

    *value = soc_reg64_field_get(unit, reg, rval64,
            obm_high_watermark_fields[subp]);
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_obm_buf_use_cnt_get(int unit, bcm_port_t port, bcm_cos_queue_t
        cosq, bcm_cosq_stat_t stat, uint64 *value)
{
    int pipe, obm_id, subp, pport;
    soc_reg_t reg;
    soc_field_t fld;
    uint64 rval64;
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t obm_usage_reg_1[_TH3_PBLKS_PER_PIPE] = {
            IDB_OBM0_USAGE_1r, IDB_OBM1_USAGE_1r,
            IDB_OBM2_USAGE_1r, IDB_OBM3_USAGE_1r,
    };
    soc_reg_t obm_usage_reg[_TH3_PBLKS_PER_PIPE] = {
            IDB_OBM0_USAGEr, IDB_OBM1_USAGEr,
            IDB_OBM2_USAGEr, IDB_OBM3_USAGEr,
    };
    bcm_port_t local_port = -1;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));

    pport = si->port_l2p_mapping[local_port];
    obm_id = soc_tomahawk3_get_pipe_pm_from_phy_port(pport);
    subp = soc_tomahawk3_get_subp_from_phy_port(pport);
    if (obm_id >= _TH3_PBLKS_PER_PIPE || (subp < 0 || subp >= _TH3_PORTS_PER_PBLK)) {
        return BCM_E_PARAM;
    }
    pipe = si->port_pipe[local_port];

    if (stat == bcmCosqStatOBMBufferBytes) {
        reg = SOC_REG_UNIQUE_ACC(unit, obm_usage_reg_1[obm_id])[pipe];
        fld = TOTAL_COUNTf;
    } else if (stat == bcmCosqStatOBMLossless1BufferBytes) {
        reg = SOC_REG_UNIQUE_ACC(unit, obm_usage_reg_1[obm_id])[pipe];
        fld = LOSSLESS1_COUNTf;
    } else if (stat == bcmCosqStatOBMLossless0BufferBytes) {
        reg = SOC_REG_UNIQUE_ACC(unit, obm_usage_reg[obm_id])[pipe];
        fld = LOSSLESS0_COUNTf;
    } else { /* (stat == bcmCosqStatOBMLossyBufferBytes) */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_usage_reg[obm_id])[pipe];
        fld = LOSSY_COUNTf;
    }

    COMPILER_64_ZERO(rval64);
    BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg, REG_PORT_ANY, subp, &rval64));

    *value = soc_reg64_field_get(unit, reg, rval64, fld);
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_rqe_drop_counters_set(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 value) {
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg;
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if ((cosq < 0) || (cosq >= _TH3_MMU_NUM_RQE_QUEUES)) {
        return BCM_E_PARAM;
    }
    if (stat == bcmCosqStatRQEGreenDroppedPackets) {
        /*Combnation of other stats*/
        return BCM_E_UNAVAIL;
    }
    if (stat == bcmCosqStatRQETotalDroppedPackets) {
        ctr_reg = SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT;
    } else if (stat == bcmCosqStatRQETotalDroppedBytes) {
        ctr_reg = SOC_COUNTER_NON_DMA_PRIQ_DROP_BYTE;
    } else if (stat == bcmCosqStatRQERedDroppedPackets) {
        ctr_reg = SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_RED;
    } else { /*(stat == bcmCosqStatRQEYellowDroppedPackets) */
        ctr_reg = SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_YELLOW;
    }
    ctrl_info.instance = 0;
    BCM_IF_ERROR_RETURN
        (soc_counter_generic_set(unit, ctr_reg,
                                 ctrl_info, 0, cosq, value));

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_rqe_drop_counters_get(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 *value, int sync) {
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg;
    uint64 sum, value64;
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if ((cosq < 0) || (cosq >= _TH3_MMU_NUM_RQE_QUEUES)) {
        return BCM_E_PARAM;
    }
    ctrl_info.instance = 0;
    if (stat == bcmCosqStatRQEGreenDroppedPackets) {
        COMPILER_64_ZERO(sum);
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit,
             SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT, ctrl_info, sync,
             cosq, &value64));
        COMPILER_64_ADD_64(sum, value64);
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit,
             SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_YELLOW, ctrl_info, sync,
             cosq, &value64));
        COMPILER_64_SUB_64(sum, value64);
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit,
             SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_RED, ctrl_info, sync,
             cosq, &value64));
        COMPILER_64_SUB_64(sum, value64);
        *value = sum;
        return BCM_E_NONE;
    }
    if (stat == bcmCosqStatRQETotalDroppedPackets) {
        ctr_reg = SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT;
    } else if (stat == bcmCosqStatRQETotalDroppedBytes) {
        ctr_reg = SOC_COUNTER_NON_DMA_PRIQ_DROP_BYTE;
    } else if (stat == bcmCosqStatRQERedDroppedPackets) {
        ctr_reg = SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_RED;
    } else { /* (stat == bcmCosqStatRQEYellowDroppedPackets) */
        ctr_reg = SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_YELLOW;
    }
    BCM_IF_ERROR_RETURN
        (soc_counter_generic_get(unit, ctr_reg,
                         ctrl_info, sync, cosq, value));

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_hp_drop_counter_set(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 value) {
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg;
    int pool = -1;

    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if ((cosq < 0) || (cosq >= _TH3_MMU_NUM_PG)) {
        return BCM_E_PARAM;
    }

    /* Retrieve headroom pool mapped to input {port, PG}*/
    _bcm_th3_cosq_ingress_hdrm_pool_get_by_pg(unit, port, cosq, &pool);
    if (pool == -1) {
        return BCM_E_INTERNAL;
    }
    ctr_reg = SOC_COUNTER_NON_DMA_HDRM_POOL_DROP_PKT;
    ctrl_info.instance = 0;
    BCM_IF_ERROR_RETURN
        (soc_counter_generic_set(unit, ctr_reg,
                         ctrl_info, 0, pool, value));

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_hp_drop_counter_get(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 *value, int sync) {
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg;
    int pool = -1;

    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if ((cosq < 0) || (cosq >= _TH3_MMU_NUM_PG)) {
        return BCM_E_PARAM;
    }

    /* Retrieve headroom pool mapped to input {port, PG}*/
    _bcm_th3_cosq_ingress_hdrm_pool_get_by_pg(unit, port, cosq, &pool);
    if (pool == -1) {
        return BCM_E_INTERNAL;
    }
    ctr_reg = SOC_COUNTER_NON_DMA_HDRM_POOL_DROP_PKT;
    ctrl_info.instance = 0;
    BCM_IF_ERROR_RETURN
        (soc_counter_generic_get(unit, ctr_reg,
                         ctrl_info, sync, pool, value));

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_ing_port_drop_counters_get(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 *value, int sync) {
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg;
    bcm_port_t local_port = -1;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if (cosq != BCM_COS_INVALID) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));

    ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
    ctrl_info.instance = local_port;

    BCM_IF_ERROR_RETURN
        (soc_counter_generic_get(unit, ctr_reg,
                         ctrl_info, sync, cosq, value));

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_ing_port_drop_counters_set(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 value) {
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg;
    bcm_port_t local_port = -1;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if (cosq != BCM_COS_INVALID) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));

    ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
    ctrl_info.instance = local_port;

    BCM_IF_ERROR_RETURN
        (soc_counter_generic_set(unit, ctr_reg,
                         ctrl_info, 0, cosq, value));

    return BCM_E_NONE;
}

int
bcm_th3_cosq_stat_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, uint64 value)
{
    bcm_port_t local_port = -1;
    switch (stat) {
        case bcmCosqStatOutPackets:
        case bcmCosqStatOutBytes:
        case bcmCosqStatDroppedBytes:
        case bcmCosqStatEgressCongestionDroppedPackets:
        case bcmCosqStatTotalDiscardDroppedPackets:
            return _bcm_th3_cosq_mmu_port_queue_counters_set(unit, port,
                    cosq, stat, value);
        case bcmCosqStatUCOutPackets:
        case bcmCosqStatUCOutBytes:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            return _bcm_th3_cosq_mmu_port_queue_counters_set(unit, port,
                    cosq, stat, value);
        case bcmCosqStatMCOutPackets:
        case bcmCosqStatMCOutBytes:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            return _bcm_th3_cosq_mmu_port_queue_counters_set(unit, port,
                    cosq, stat, value);
        case bcmCosqStatDroppedPackets:
            return _bcm_th3_cosq_mmu_port_queue_total_drop_counters_set(unit,
                    port, cosq, stat, value);
        case bcmCosqStatYellowDiscardDroppedPackets:
        case bcmCosqStatRedDiscardDroppedPackets:
        case bcmCosqStatGreenDiscardDroppedPackets:
            return  _bcm_th3_cosq_mmu_port_queue_wred_color_drop_set(unit, port,
                cosq, stat, value);
        case bcmCosqStatYellowCongestionDroppedPackets:
        case bcmCosqStatRedCongestionDroppedPackets:
            return  _bcm_th3_cosq_mmu_port_queue_color_drop_set(unit, port,
                cosq, stat, value);
        case bcmCosqStatHighPriDroppedPackets:
        case bcmCosqStatOBMLossyHighPriDroppedPackets:
        case bcmCosqStatLowPriDroppedPackets:
        case bcmCosqStatOBMLossyLowPriDroppedPackets:
        case bcmCosqStatHighPriDroppedBytes:
        case bcmCosqStatOBMLossyHighPriDroppedBytes:
        case bcmCosqStatLowPriDroppedBytes:
        case bcmCosqStatOBMLossyLowPriDroppedBytes:
        case bcmCosqStatOBMLossless0DroppedPackets:
        case bcmCosqStatOBMLossless1DroppedPackets:
        case bcmCosqStatOBMLossless0DroppedBytes:
        case bcmCosqStatOBMLossless1DroppedBytes:
        case bcmCosqStatOBMFlowControlEvents:
            return _bcm_th3_obm_port_counters_set(unit, port, cosq, stat, value);
        case bcmCosqStatOBMHighWatermark:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_SCHEDULER(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_obmhighwatermark_set(unit, local_port, value));
            return BCM_E_NONE;
        case bcmCosqStatHdrmPoolDroppedPackets:
            return _bcm_th3_hp_drop_counter_set(unit, port, cosq, stat,
                    value);
        case bcmCosqStatSourcePortDroppedPackets:
            return _bcm_th3_ing_port_drop_counters_set(unit, port, cosq, stat,
                    value);
        case bcmCosqStatRQETotalDroppedPackets:
        case bcmCosqStatRQETotalDroppedBytes:
        case bcmCosqStatRQEGreenDroppedPackets:
        case bcmCosqStatRQEYellowDroppedPackets:
        case bcmCosqStatRQERedDroppedPackets:
            return _bcm_th3_rqe_drop_counters_set(unit, port,
                    cosq, stat, value);
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;

}

int
bcm_th3_cosq_stat_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, int sync_mode, uint64 *value)
{

    int sync = sync_mode ? SOC_COUNTER_SYNC_ENABLE : 0;
    bcm_port_t local_port = -1;
    uint64 cell_value;
    COMPILER_64_ZERO(cell_value);
    switch (stat) {
        case bcmCosqStatOutPackets:
        case bcmCosqStatOutBytes:
        case bcmCosqStatDroppedBytes:
        case bcmCosqStatEgressCongestionDroppedPackets:
        case bcmCosqStatTotalDiscardDroppedPackets:
            return _bcm_th3_cosq_mmu_port_queue_counters_get(unit, port,
                    cosq, stat, value, sync);
        case bcmCosqStatUCOutPackets:
        case bcmCosqStatUCOutBytes:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            return _bcm_th3_cosq_mmu_port_queue_counters_get(unit, port,
                    cosq, stat, value, sync);
        case bcmCosqStatMCOutPackets:
        case bcmCosqStatMCOutBytes:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            return _bcm_th3_cosq_mmu_port_queue_counters_get(unit, port,
                    cosq, stat, value, sync);
        case bcmCosqStatDroppedPackets:
            return _bcm_th3_cosq_mmu_port_queue_total_drop_counters_get(unit, port,
                cosq, stat, value, sync);
        case bcmCosqStatYellowDiscardDroppedPackets:
        case bcmCosqStatRedDiscardDroppedPackets:
        case bcmCosqStatGreenDiscardDroppedPackets:
            return _bcm_th3_cosq_mmu_port_queue_wred_color_drop_get(unit, port,
                cosq, stat, value, sync);
        case bcmCosqStatYellowCongestionDroppedPackets:
        case bcmCosqStatRedCongestionDroppedPackets:
            return _bcm_th3_cosq_mmu_port_queue_color_drop_get(unit, port,
                cosq, stat, value, sync);
        case bcmCosqStatEgressUCQueueBytesCurrent:
        case bcmCosqStatEgressMCQueueBytesCurrent:
        case bcmCosqStatEgressMCQueueMinBytesCurrent:
        case bcmCosqStatEgressUCQueueMinBytesCurrent:
            BCM_IF_ERROR_RETURN
                (_bcm_th3_queue_current_usage_bytes_get(unit, port,
                    cosq, stat, &cell_value, sync));
            /*value = *cell_value * _TH3_MMU_BYTES_PER_CELL;*/
            COMPILER_64_ZERO(*value);
            COMPILER_64_ADD_64(*value, cell_value);
            COMPILER_64_UMUL_32(*value, _TH3_MMU_BYTES_PER_CELL);
            return BCM_E_NONE;
        case bcmCosqStatIngressPortPGMinBytesCurrent:
        case bcmCosqStatIngressPortPGSharedBytesCurrent:
        case bcmCosqStatIngressPortPGHeadroomBytesCurrent:
            BCM_IF_ERROR_RETURN
                (_bcm_th3_pg_current_usage_bytes_get(unit, port,
                    cosq, stat, &cell_value, sync));
            /*value = *cell_value * _TH3_MMU_BYTES_PER_CELL;*/
            COMPILER_64_ZERO(*value);
            COMPILER_64_ADD_64(*value, cell_value);
            COMPILER_64_UMUL_32(*value, _TH3_MMU_BYTES_PER_CELL);
            return BCM_E_NONE;
        case bcmCosqStatIngressPortPoolSharedBytesCurrent:
            BCM_IF_ERROR_RETURN
                (_bcm_th3_sp_current_usage_bytes_get(unit, port,
                    cosq, stat, &cell_value, sync));
            /*value = *cell_value * _TH3_MMU_BYTES_PER_CELL;*/
            COMPILER_64_ZERO(*value);
            COMPILER_64_ADD_64(*value, cell_value);
            COMPILER_64_UMUL_32(*value, _TH3_MMU_BYTES_PER_CELL);
            return BCM_E_NONE;
        case bcmCosqStatHighPriDroppedPackets:
        case bcmCosqStatOBMLossyHighPriDroppedPackets:
        case bcmCosqStatLowPriDroppedPackets:
        case bcmCosqStatOBMLossyLowPriDroppedPackets:
        case bcmCosqStatHighPriDroppedBytes:
        case bcmCosqStatOBMLossyHighPriDroppedBytes:
        case bcmCosqStatLowPriDroppedBytes:
        case bcmCosqStatOBMLossyLowPriDroppedBytes:
        case bcmCosqStatOBMLossless0DroppedPackets:
        case bcmCosqStatOBMLossless1DroppedPackets:
        case bcmCosqStatOBMLossless0DroppedBytes:
        case bcmCosqStatOBMLossless1DroppedBytes:
        case bcmCosqStatOBMFlowControlEvents:
            return _bcm_th3_obm_port_counters_get(unit, port, cosq, stat,
                    value, sync);
        case bcmCosqStatOBMHighWatermark:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_SCHEDULER(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_obmhighwatermark_get(unit, local_port, value));
            return BCM_E_NONE;
        case bcmCosqStatHdrmPoolDroppedPackets:
            return _bcm_th3_hp_drop_counter_get(unit, port, cosq, stat,
                    value, sync);
        case bcmCosqStatSourcePortDroppedPackets:
            return _bcm_th3_ing_port_drop_counters_get(unit, port, cosq, stat,
                    value, sync);
        case bcmCosqStatRQETotalDroppedPackets:
        case bcmCosqStatRQETotalDroppedBytes:
        case bcmCosqStatRQEGreenDroppedPackets:
        case bcmCosqStatRQEYellowDroppedPackets:
        case bcmCosqStatRQERedDroppedPackets:
            return _bcm_th3_rqe_drop_counters_get(unit, port,
                    cosq, stat, value, sync);
        case bcmCosqStatOBMLossyBufferBytes:
        case bcmCosqStatOBMLossless0BufferBytes:
        case bcmCosqStatOBMLossless1BufferBytes:
        case bcmCosqStatOBMBufferBytes:
            return _bcm_th3_cosq_obm_buf_use_cnt_get(unit, port, cosq, stat, value);
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;

}



#endif

