/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/drv.h>
#include <soc/counter.h>
#include <soc/debug.h>
#include <shared/bsl.h>
#include <bcm/stat.h>
#include <bcm/error.h>
#include <bcm_int/esw/stat.h>
#include <bcm_int/esw_dispatch.h>


/*
 * Function:
 *      bcmi_stat_preempt_ge_get_set
 * Description:
 *      Get the specified statistic for a port with preempt mib counter support shared
 *      by unimac.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - zero-based port number
 *      sync_mode - if 1 read hw counter else sw accumualated counter
 *      type - SNMP statistics type (see bcm/stat.h)
 *      val  - (OUT) 64 bit statistic counter value
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_PARAM - Illegal parameter.
 *      BCM_E_INTERNAL - Chip access failure.
 */
int
bcmi_stat_preempt_ge_get_set(int unit, bcm_port_t port,
                             int sync_mode, int stat_op,
                             bcm_stat_val_t type, uint64 *val,
                             int incl_non_ge_stat)
{
    uint64 count, count1;
    int reg_op = (stat_op == _BCM_STAT_GET)? _BCM_STAT_REG_ADD:
                                             _BCM_STAT_REG_CLEAR;
    int reg_sub_op = (stat_op == _BCM_STAT_GET)? _BCM_STAT_REG_SUB:
                                                 _BCM_STAT_REG_CLEAR;

    COMPILER_REFERENCE(&count);  /* Work around PPC compiler bug */
    COMPILER_64_ZERO(count);

    switch (type) {
        /* *** RFC 1213 *** */

    case snmpIfInOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRBYTr, &count);  /* bytes rcvd */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRRBYTr, &count); /* Runt bytes */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRBYTPr, &count);  /* bytes rcvd (preempt counter) */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRRBYTPr, &count); /* Runt bytes (preempt counter) */
        break;
    case snmpIfInUcastPkts:    /* Unicast packets received */
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GRUCr, &count);
                /* preempt counter */
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GRUCPr, &count);
                /* + oversize, good FCS */
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GROVRr, &count);
                /* + oversize, good FCS (preempt counter) */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GROVRPr, &count);
            } else {
                if (incl_non_ge_stat) {
                    /* RUC switch register has the count of all received packets,
                     * add the count when incl_non_ge_stat flag is set.
                     */
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RUCr,
                                      &count);  /* unicast pkts rcvd */
                }
            }
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRPKTr, &count); /* all pkts rcvd */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRMCAr, &count); /* - multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRBCAr, &count); /* - broadcast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRALNr, &count); /* - bad FCS, dribble bit  */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRFCSr, &count); /* - bad FCS, no dribble bit */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRJBRr, &count); /* - oversize, bad FCS */

            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRPKTPr, &count); /* all pkts rcvd (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRMCAPr, &count); /* - multicast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRBCAPr, &count); /* - broadcast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRALNPr, &count); /* - bad FCS, dribble bit (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRFCSPr, &count); /* - bad FCS, no dribble bit (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRJBRPr, &count); /* - oversize, bad FCS (preempt counter) */
            if (SOC_REG_IS_VALID(unit, GRMTUEr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRMTUEr,
                                  &count); /* - mtu exceeded, good FCS */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRMTUEPr,
                                  &count); /* - mtu exceeded, good FCS (preempt counter) */
            }

            if ((SOC_REG_IS_VALID(unit, GROVRr)) && (COUNT_OVR_ERRORS(unit))) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GROVRr,
                                  &count); /* - oversize, good FCS */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GROVRPr,
                                  &count); /* - oversize, good FCS (preempt counter) */
            }
        }
        break;
    case snmpIfInNUcastPkts:    /* Non Unicast packets received */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRMCAr, &count); /* multicast pkts rcvd */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRBCAr, &count); /* broadcast pkts rcvd */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRMCAPr, &count); /* multicast pkts rcvd (preempt counter) */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRBCAPr, &count); /* broadcast pkts rcvd (preempt counter) */
        break;
    case snmpIfInBroadcastPkts: /* Broadcast packets received */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRBCAr, &count);  /* broadcast pkts rcvd */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRBCAPr, &count);  /* broadcast pkts rcvd (preempt counter) */
        break;
    case snmpIfInMulticastPkts:  /* Multicast packets received */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRMCAr, &count);  /* multicast pkts rcvd */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRMCAPr, &count);  /* multicast pkts rcvd (preempt counter) */
        break;
    case snmpIfInDiscards:    /* Dropped packets including aborted */
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  RDBGC0r, &count); /* Ingress drop conditions */
                BCM_IF_ERROR_RETURN
                    (_bcm_stat_counter_non_dma_extra_get(unit,
                                                         SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING,
                                                         port, &count1));
                COMPILER_64_ADD_64(count, count1);
            }
        }
        break;
    case snmpIfInErrors:    /* Receive packets containing errors */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRUNDr, &count);  /* undersize pkts, good FCS */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRFRGr, &count);  /* undersize pkts, bad FCS */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRFCSr, &count);  /* FCS errors */

        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRUNDPr, &count);  /* undersize pkts, good FCS (preempt counter) */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRFRGPr, &count);  /* undersize pkts, bad FCS (preempt counter) */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRFCSPr, &count);  /* FCS errors (preempt counter) */
        if (SOC_REG_IS_VALID(unit, GRMTUEr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRMTUEr, &count); /* mtu exceeded pkts, good FCS */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRMTUEPr, &count); /* mtu exceeded pkts, good FCS (preempt counter) */
        } else if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GROVRr, &count);    /* oversize pkts, good FCS */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GROVRPr, &count);    /* oversize pkts, good FCS (preempt counter) */
        }
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRJBRr, &count);  /* oversize pkts, bad FCS */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRJBRPr, &count);  /* oversize pkts, bad FCS (preempt counter) */
        break;
    case snmpIfInUnknownProtos:
        break;
    case snmpIfOutOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTBYTr, &count);  /* transmit bytes */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTBYTPr, &count);  /* transmit bytes (preempt counter) */
        break;
    case snmpIfOutUcastPkts:
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GTUCr, &count);  /* unicast pkts sent */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GTUCPr, &count);  /* unicast pkts sent (preempt counter) */
            if (!COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GTOVRr, &count); /* + oversize, good FCS */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GTOVRPr, &count); /* + oversize, good FCS (preempt counter) */
            }
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GTPKTr, &count); /* all pkts xmited */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTMCAr, &count); /* - multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTBCAr, &count); /* - broadcast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTFCSr, &count); /* - bad FCS */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTJBRr, &count); /* - oversize, bad FCS */

            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GTPKTPr, &count); /* all pkts xmited (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTMCAPr, &count); /* - multicast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTBCAPr, &count); /* - broadcast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTFCSPr, &count); /* - bad FCS (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTJBRPr, &count); /* - oversize, bad FCS (preempt counter) */
            if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GTOVRr, &count); /* - oversize, good FCS */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GTOVRPr, &count); /* - oversize, good FCS (preempt counter) */
            }
        }
        break;
    case snmpIfOutNUcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTMCAr, &count);  /* multicast pkts */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTBCAr, &count);  /* broadcast pkts */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTMCAPr, &count);  /* multicast pkts (preempt counter) */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTBCAPr, &count);  /* broadcast pkts (preempt counter) */
        break;
    case snmpIfOutBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTBCAr, &count);  /* broadcast pkts */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTBCAPr, &count);  /* broadcast pkts (preempt counter) */
        break;
    case snmpIfOutMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTMCAr, &count);  /* multicast pkts */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTMCAPr, &count);  /* multicast pkts (preempt counter) */
        break;
    case snmpIfOutDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTEDFr, &count);  /* multiple deferral */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTEDFPr, &count);  /* multiple deferral (preempt counter) */
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, HOLDr)) {  /* L2 MTU drops */
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                      HOLDr, &count);
                } else if (SOC_REG_IS_VALID(unit, HOL_DROPr)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                      HOL_DROPr, &count);
                }
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  TDBGC3r, &count);
                BCM_IF_ERROR_RETURN
                    (_bcm_stat_counter_extra_get(unit, EGRDROPPKTCOUNTr,
                                                 port, &count1));
                COMPILER_64_ADD_64(count, count1);
                BCM_IF_ERROR_RETURN
                    (bcm_esw_cosq_stat_get(unit, port, BCM_COS_INVALID,
                                           bcmCosqStatDroppedPackets, &count1));
                COMPILER_64_ADD_64(count, count1);
            }
        }
        /* other causes of discards? */
        break;
    case snmpIfOutErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTXCLr, &count);  /* excessive collisions */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTXCLPr, &count);  /* excessive collisions (preempt counter) */
        break;
    case snmpIfOutQLen:
        {
            uint32 qcount;
            if (bcm_esw_port_queued_count_get(unit, port, &qcount) >= 0) {
                COMPILER_64_ADD_32(count, qcount);
            }
        }
        break;
    case snmpIpInReceives:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  RIPC4r, &count);
            }
        }
        break;
    case snmpIpInHdrErrors:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  RIPD4r, &count);
            }
        }
        break;
/* ipInAddrErrors */
    case snmpIpForwDatagrams:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  TDBGC4r, &count);
            }
        }
        break;
/* ipInUnknownProtos */
    case snmpIpInDiscards:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  RIPHE4r, &count);
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  RIPD4r, &count);
            }
        }
        break;
/* ipInDelivers */
/* ipOutRequests */
/* ipOutDiscards */
/* ipOutNoRoutes */

        /* *** RFC 1493 *** */

    case snmpDot1dBasePortDelayExceededDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTEDFr, &count);  /* multiple deferral */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTEDFPr, &count);  /* multiple deferral (preempt counter) */
        break;
    case snmpDot1dBasePortMtuExceededDiscards:
        if (SOC_REG_IS_VALID(unit, GRMTUEr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRMTUEr, &count); /* mtu exceeded pkts */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRMTUEPr, &count); /* mtu exceeded pkts (preempt counter) */
        } else if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GROVRr, &count);  /* oversize pkts */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GROVRPr, &count);  /* oversize pkts (preempt counter) */
        }
        break;
    case snmpDot1dTpPortInFrames:  /* should be only bridge mgmt */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPKTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPKTPr, &count);
        break;
    case snmpDot1dTpPortOutFrames:  /* should be only bridge mgmt */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTPKTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTPKTPr, &count);
        break;
    case snmpDot1dPortInDiscards:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDISCr, &count);
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4r, &count);
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPORTDr, &count);
            }
        }
        break;

        /* *** RFC 1757 *** */

    case snmpEtherStatsDropEvents:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDISCr, &count);
            }
        }
        break;
    case snmpEtherStatsOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRRBYTr, &count); /* Runt bytes */

        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRBYTPr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTBYTPr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRRBYTPr, &count); /* Runt bytes */
        break;
    case snmpEtherStatsPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRRPKTr, &count); /* Runt packets */

        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPKTPr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTPKTPr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRRPKTPr, &count); /* Runt packets */
        break;
    case snmpEtherStatsBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRBCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTBCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRBCAPr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTBCAPr, &count);
        break;
    case snmpEtherStatsMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRMCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTMCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRMCAPr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTMCAPr, &count);
        break;
    case snmpEtherStatsCRCAlignErrors:  /* CRC errors + alignment errors */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRFCSr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRFCSPr, &count);
        break;
    case snmpEtherStatsUndersizePkts:  /* Undersize frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRUNDr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRUNDPr, &count);
        break;
    case snmpEtherStatsOversizePkts:
        if (soc_feature(unit, soc_feature_stat_jumbo_adj)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GROVRr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTOVRr, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GROVRPr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTOVRPr, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRJBRr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTJBRr, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRJBRPr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTJBRPr, &count);
        }
        break;
    case snmpEtherRxOversizePkts:
        if (soc_feature(unit, soc_feature_stat_jumbo_adj)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GROVRr, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GROVRPr, &count);
        }
        break;
    case snmpEtherTxOversizePkts:
        if (soc_feature(unit, soc_feature_stat_jumbo_adj)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTOVRr, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTOVRPr, &count);
        }
        break;
    case snmpEtherStatsFragments:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRFRGr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTFRGr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRFRGPr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTFRGPr, &count);
        break;
    case snmpEtherStatsJabbers:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRJBRr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTJBRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRJBRPr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTJBRPr, &count);
        break;
    case snmpEtherStatsCollisions:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTNCLr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTNCLPr, &count);
        break;
    case snmpEtherStatsPkts64Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR64r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT64r, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR64Pr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT64Pr, &count);
        }
        break;
    case snmpEtherStatsPkts65to127Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR127r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT127r, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR127Pr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT127Pr, &count);
        }
        break;
    case snmpEtherStatsPkts128to255Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR255r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT255r, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR255Pr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT255Pr, &count);
        }
        break;
    case snmpEtherStatsPkts256to511Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR511r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT511r, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR511Pr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT511Pr, &count);
        }
        break;
    case snmpEtherStatsPkts512to1023Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR1023r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT1023r, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR1023Pr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT1023Pr, &count);
        }
        break;
    case snmpEtherStatsPkts1024to1518Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR1518r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT1518r, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR1518Pr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT1518Pr, &count);
        }
        break;

        /* *** not actually in rfc1757 *** */

    case snmpBcmEtherStatsPkts1519to1522Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRMGVr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTMGVr, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRMGVPr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTMGVPr, &count);
        }
        break;
    case snmpBcmEtherStatsPkts1522to2047Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR2047r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRMGVr, &count);

            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT2047r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GTMGVr, &count);

            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR2047Pr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRMGVPr, &count);

            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT2047Pr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GTMGVPr, &count);
        }
        break;
    case snmpBcmEtherStatsPkts2048to4095Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR4095r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT4095r, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR4095Pr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT4095Pr, &count);
        }
        break;
    case snmpBcmEtherStatsPkts4095to9216Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR9216r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT9216r, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR9216Pr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT9216Pr, &count);
        }
        break;
    case snmpBcmReceivedPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR64r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR64Pr, &count);
        break;
    case snmpBcmReceivedPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR127r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR127Pr, &count);
        break;
    case snmpBcmReceivedPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR255r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR255Pr, &count);
        break;
    case snmpBcmReceivedPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR511r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR511Pr, &count);
        break;
    case snmpBcmReceivedPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR1023r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR1023Pr, &count);
        break;
    case snmpBcmReceivedPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR1518r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR1518Pr, &count);
        break;
    case snmpBcmReceivedPkts1519to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR2047r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR2047Pr, &count);
        break;
    case snmpBcmReceivedPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR4095r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR4095Pr, &count);
        break;
    case snmpBcmReceivedPkts4095to9216Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR9216r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR9216Pr, &count);
        break;

    case snmpBcmTransmittedPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT64r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT64Pr, &count);
        break;
    case snmpBcmTransmittedPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT127r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT127Pr, &count);
        break;
    case snmpBcmTransmittedPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT255r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT255Pr, &count);
        break;
    case snmpBcmTransmittedPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT511r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT511Pr, &count);
        break;
    case snmpBcmTransmittedPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT1023r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT1023Pr, &count);
        break;
    case snmpBcmTransmittedPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT1518r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT1518Pr, &count);
        break;
    case snmpBcmTransmittedPkts1519to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT2047r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT2047Pr, &count);
        break;
    case snmpBcmTransmittedPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT4095r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT4095Pr, &count);
        break;
    case snmpBcmTransmittedPkts4095to9216Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT9216r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT9216Pr, &count);
        break;
    case snmpEtherStatsTXNoErrors:
        /* FE = TPKT - (TNCL + TOVR + TFRG + TUND) */
        /* GE = GTPKT - (GTOVR + GTIPD + GTABRT) */
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              GTPOKr, &count); /* All good packets */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              GTPOKPr, &count); /* All good packets (preempt counter) */
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              GTPKTr, &count); /* All Packets */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              GTPKTPr, &count); /* All Packets (preempt counter) */
            if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                                  GTOVRr, &count); /* Oversize */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                                  GTOVRPr, &count); /* Oversize (preempt counter) */
            }
        }
        break;
    case snmpEtherStatsRXNoErrors:
        /* Some chips have a dedicated register for this stat */
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPOKr, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPOKPr, &count);
        } else {
            /*
             * GE = GRPKT - (GRFCS + GRXUO + GRFLR + GRCDE + GRFCR +
             *                GRUND + GROVR + GRJBR + GRIPCHK/GRIPHE)
             */

            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPKTr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRFCSr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRXUOr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRCDEr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRFCRr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRUNDr, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPKTPr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRFCSPr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRCDEPr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRFCRPr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRUNDPr, &count);
            if (SOC_REG_IS_VALID(unit, GRMTUEr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GRMTUEr, &count); /* mtu exceeded pkts */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GRMTUEPr, &count); /* mtu exceeded pkts (preempt counter) */
            } else if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GROVRr, &count);  /* oversize pkts */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GROVRPr, &count);  /* oversize pkts (preempt counter) */
            }
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRJBRr, &count);
        }
        break;
        /* *** RFC 2665 *** */

    case snmpDot3StatsAlignmentErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRALNr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRALNPr, &count);
        break;
    case snmpDot3StatsFCSErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRFCSr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRFCSPr, &count);
        break;
    case snmpDot3StatsSingleCollisionFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTSCLr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTSCLPr, &count);
        break;
    case snmpDot3StatsMultipleCollisionFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTMCLr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTMCLPr, &count);
        break;
    case snmpDot3StatsSQETTestErrors:
        /* always 0 */
        break;
    case snmpDot3StatsDeferredTransmissions:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTDFRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTDFRPr, &count);
        break;
    case snmpDot3StatsLateCollisions:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTLCLr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTLCLPr, &count);
        break;
    case snmpDot3StatsExcessiveCollisions:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTXCLr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTXCLPr, &count);
        break;
    case snmpDot3StatsInternalMacTransmitErrors:
        /* always 0 */
        break;
    case snmpDot3StatsCarrierSenseErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRFCRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRFCRPr, &count);
        break;
    case snmpDot3StatsFrameTooLongs:
        if (soc_feature(unit, soc_feature_stat_jumbo_adj)) {
            if (SOC_REG_IS_VALID(unit, GRMTUEr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GRMTUEr, &count); /* mtu exceeded pkts */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GRMTUEPr, &count); /* mtu exceeded pkts (preempt counter) */
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GROVRr, &count); /* oversized pkts */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GROVRPr, &count); /* oversized pkts (preempt counter) */
            }
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRJBRr, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRJBRPr, &count);
        }
        break;
    case snmpDot3StatsInternalMacReceiveErrors:
        /* always 0 */
        break;
    case snmpDot3StatsSymbolErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRCDEr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRCDEPr, &count);
        break;
    case snmpDot3ControlInUnknownOpcodes:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRXUOr, &count);
        break;
    case snmpDot3InPauseFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRXPFr, &count);
        break;
    case snmpDot3OutPauseFrames:
        if (incl_non_ge_stat) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTXPFr, &count);
        } /* Else avoid double-counting of pause frames on GXMACs */
        break;

        /* *** RFC 2233 high capacity versions of RFC1213 objects *** */

    case snmpIfHCInOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRBYTr, &count);  /* bytes rcvd */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRRBYTr, &count); /* Runt bytes */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRBYTPr, &count);  /* bytes rcvd (preempt counter) */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRRBYTPr, &count); /* Runt bytes (preempt counter) */
        break;
    case snmpIfHCInUcastPkts:
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            if (SOC_REG_IS_VALID(unit, GRUCr) && !SOC_IS_STACK_PORT(unit, port)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GRUCr, &count);
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GRUCPr, &count); /* preempt counter */
                if (!COUNT_OVR_ERRORS(unit)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                      GROVRr, &count);  /* + oversize pkts */
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                      GROVRPr, &count);  /* + oversize pkts (preempt counter) */
                }
            } else {
                if (incl_non_ge_stat) {
                    /* RUC switch register has the count of all received packets,
                     * add the count when incl_non_ge_stat flag is set.
                     */
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                      RUCr, &count);  /* unicast pkts rcvd */
                }
            }
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRPKTr, &count); /* all pkts rcvd */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRMCAr, &count); /* - multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRBCAr, &count); /* - broadcast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRALNr, &count); /* - bad FCS, dribble bit  */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRFCSr, &count); /* - bad FCS, no dribble bit */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRJBRr, &count); /* - oversize, bad FCS */

            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRPKTPr, &count); /* all pkts rcvd (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRMCAPr, &count); /* - multicast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRBCAPr, &count); /* - broadcast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRALNPr, &count); /* - bad FCS, dribble bit (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRFCSPr, &count); /* - bad FCS, no dribble bit (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GRJBRPr, &count); /* - oversize, bad FCS (preempt counter) */
            if (SOC_REG_IS_VALID(unit, GRMTUEr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GRMTUEr, &count); /* mtu exceeded pkts */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GRMTUEPr, &count); /* mtu exceeded pkts (preempt counter) */
            } else if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GROVRr, &count);  /* oversize pkts */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GROVRPr, &count);  /* oversize pkts (preempt counter) */
            }
        }
        break;
    case snmpIfHCInMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRMCAr, &count);  /* multicast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRMCAPr, &count);  /* multicast (preempt counter) */
        break;
    case snmpIfHCInBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRBCAr, &count);  /* broadcast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GRBCAPr, &count);  /* broadcast (preempt counter) */
        break;
    case snmpIfHCOutOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTBYTr, &count);  /* transmit bytes */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTBYTPr, &count);  /* transmit bytes (preempt counter) */
        break;
    case snmpIfHCOutUcastPkts:
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GTUCr, &count); /* All good packets */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GTUCPr, &count); /* All good packets (preempt counter) */
            if (!COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GTOVRr, &count);  /* + oversize pkts */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  GTOVRPr, &count);  /* + oversize pkts (preempt counter) */
            }
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GTPKTr, &count); /* all pkts xmited */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTMCAr, &count); /* - multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTBCAr, &count); /* - broadcast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTFCSr, &count); /* - bad FCS */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTJBRr, &count); /* - oversize, bad FCS */

            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GTPKTPr, &count); /* all pkts xmited (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTMCAPr, &count); /* - multicast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTBCAPr, &count); /* - broadcast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTFCSPr, &count); /* - bad FCS (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              GTJBRPr, &count); /* - oversize, bad FCS (preempt counter) */
            if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GTOVRr, &count); /* - oversize, good FCS */
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GTOVRPr, &count); /* - oversize, good FCS (preempt counter) */
            }
        }
        break;
    case snmpIfHCOutMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTMCAr, &count);  /* multicast xmited */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTMCAPr, &count);  /* multicast xmited (preempt counter) */
        break;
    case snmpIfHCOutBroadcastPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTBCAr, &count);  /* broadcast xmited */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          GTBCAPr, &count);  /* broadcast xmited (preempt counter) */
        break;

        /* *** RFC 2465 *** */

    case snmpIpv6IfStatsInReceives:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPC6r, &count);
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6r, &count);
            }
        }
        break;
    case snmpIpv6IfStatsInHdrErrors:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
            }
        }
        break;
    case snmpIpv6IfStatsInAddrErrors:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  RIPHE6r, &count);
            }
        }
        break;
    case snmpIpv6IfStatsInDiscards:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  RIPHE6r, &count);
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  RIPD6r, &count);
            }
        }
        break;
    case snmpIpv6IfStatsOutForwDatagrams:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  TDBGC0r, &count);
            }
        }
        break;
    case snmpIpv6IfStatsOutDiscards:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  TDBGC1r, &count);
            }
        }
        break;
    case snmpIpv6IfStatsInMcastPkts:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  IMRP6r, &count);
            }
        }
        break;
    case snmpIpv6IfStatsOutMcastPkts:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                  TDBGC2r, &count);
            }
        }
        break;

        /* *** IEEE 802.1bb *** */
    case snmpIeee8021PfcRequests:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTXPPr, &count);
        break;
    case snmpIeee8021PfcIndications:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRXPPr, &count);
        break;

        /* *** RFC 1284 - unsupported in XGS *** */
    case snmpDot3StatsInRangeLengthError:
        break;

        /* *** RFC 4837 - unsupported in XGS *** */
    case snmpDot3OmpEmulationCRC8Errors:
    case snmpDot3MpcpRxGate:
    case snmpDot3MpcpRxRegister:
    case snmpDot3MpcpTxRegRequest:
    case snmpDot3MpcpTxRegAck:
    case snmpDot3MpcpTxReport:
    case snmpDot3EponFecCorrectedBlocks:
    case snmpDot3EponFecUncorrectableBlocks:
        break;

        /* IPMC counters (broadcom specific) */

    case snmpBcmIPMCBridgedPckts:
        if (soc_feature(unit, soc_feature_ip_mcast)) {
            if (soc_feature(unit, soc_feature_stat_xgs3)) {
                if (incl_non_ge_stat) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                      RDBGC1r, &count);
                }
            }
        }
        break;
    case snmpBcmIPMCRoutedPckts:
        if (soc_feature(unit, soc_feature_ip_mcast)) {
            if (soc_feature(unit, soc_feature_stat_xgs3)) {
                if (incl_non_ge_stat) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                      IMRP4r, &count);
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                      IMRP6r, &count);
                }
            }
        }
        break;
    case snmpBcmIPMCInDroppedPckts:
        if (soc_feature(unit, soc_feature_ip_mcast)) {
            if (soc_feature(unit, soc_feature_stat_xgs3)) {
                if (incl_non_ge_stat) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                      RDBGC2r, &count);
                }
            }
        }
        break;
    case snmpBcmIPMCOutDroppedPckts:
        if (soc_feature(unit, soc_feature_ip_mcast)) {
            if (soc_feature(unit, soc_feature_stat_xgs3)) {
                if (incl_non_ge_stat) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                      TDBGC5r, &count);
                }
            }
        }
        break;

        /* EA (broadcom specific) - unsupported in XGS */
    case snmpBcmPonInDroppedOctets:
    case snmpBcmPonOutDroppedOctets:
    case snmpBcmPonInDelayedOctets:
    case snmpBcmPonOutDelayedOctets:
    case snmpBcmPonInDelayedHundredUs:
    case snmpBcmPonOutDelayedHundredUs:
    case snmpBcmPonInFrameErrors:
    case snmpBcmPonInOamFrames:
    case snmpBcmPonOutOamFrames:
    case snmpBcmPonOutUnusedOctets:
        break;

    case snmpFcmPortClass3RxFrames:
    case snmpFcmPortClass3TxFrames:
    case snmpFcmPortClass3Discards:
    case snmpFcmPortClass2RxFrames:
    case snmpFcmPortClass2TxFrames:
    case snmpFcmPortClass2Discards:
    case snmpFcmPortInvalidCRCs:
    case snmpFcmPortDelimiterErrors:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeTxFrag:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GTNFFPr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeTxVerifyFrame:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GTVMFPr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeTxReplyFrame:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GTRMFPr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFrameAssErrors:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GTMFAEPr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFrameSmdErrors:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRMSEPr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFrameAss:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRMFAOPr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFrag:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRNFFPr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxVerifyFrame:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRVMFPr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxReplyFrame:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRRMFPr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFinalFragSizeError:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRFFSEPr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFragSizeError:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRNFSEPr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxDiscard:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRDMFPr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeHoldCount:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              MACMERGERHOLDCOUNTr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    /* Receive PFC Frame PriorityX XON to XOFF */
    case snmpBcmRxPFCFrameXonPriority0:
        if (SOC_REG_IS_VALID(unit, GRPFCOFF0r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRPFCOFF0r, &count);
        }
        break;

    case snmpBcmRxPFCFrameXonPriority1:
        if (SOC_REG_IS_VALID(unit, GRPFCOFF1r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRPFCOFF1r, &count);
        }
        break;

    case snmpBcmRxPFCFrameXonPriority2:
        if (SOC_REG_IS_VALID(unit, GRPFCOFF2r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRPFCOFF2r, &count);
        }
        break;

    case snmpBcmRxPFCFrameXonPriority3:
        if (SOC_REG_IS_VALID(unit, GRPFCOFF3r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRPFCOFF3r, &count);
        }
        break;

    case snmpBcmRxPFCFrameXonPriority4:
        if (SOC_REG_IS_VALID(unit, GRPFCOFF4r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRPFCOFF4r, &count);
        }
        break;

    case snmpBcmRxPFCFrameXonPriority5:
        if (SOC_REG_IS_VALID(unit, GRPFCOFF5r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRPFCOFF5r, &count);
        }
        break;

    case snmpBcmRxPFCFrameXonPriority6:
        if (SOC_REG_IS_VALID(unit, GRPFCOFF6r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRPFCOFF6r, &count);
        }
        break;

    case snmpBcmRxPFCFrameXonPriority7:
        if (SOC_REG_IS_VALID(unit, GRPFCOFF7r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRPFCOFF7r, &count);
        }
        break;

    /* Receive PFC Frame Priority */
    case snmpBcmRxPFCFramePriority0:
        if (SOC_REG_IS_VALID(unit, GRPFC0r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPFC0r, &count);
        }
        break;

    case snmpBcmRxPFCFramePriority1:
        if (SOC_REG_IS_VALID(unit, GRPFC1r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPFC1r, &count);
        }
        break;

    case snmpBcmRxPFCFramePriority2:
        if (SOC_REG_IS_VALID(unit, GRPFC2r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPFC2r, &count);
        }
        break;

    case snmpBcmRxPFCFramePriority3:
        if (SOC_REG_IS_VALID(unit, GRPFC3r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPFC3r, &count);
        }
        break;

    case snmpBcmRxPFCFramePriority4:
        if (SOC_REG_IS_VALID(unit, GRPFC4r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPFC4r, &count);
        }
        break;

    case snmpBcmRxPFCFramePriority5:
        if (SOC_REG_IS_VALID(unit, GRPFC5r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPFC5r, &count);
        }
        break;

    case snmpBcmRxPFCFramePriority6:
        if (SOC_REG_IS_VALID(unit, GRPFC6r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPFC6r, &count);
        }
        break;

    case snmpBcmRxPFCFramePriority7:
        if (SOC_REG_IS_VALID(unit, GRPFC7r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPFC7r, &count);
        }
        break;

    case snmpBcmRxDot3LengthMismatches:
        if (SOC_REG_IS_VALID(unit, GRFLRr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRFLRr, &count); /* Good FCS, bad length */
        } else {
            return BCM_E_UNAVAIL;
        }

        /* Preempt counter */
        if (SOC_REG_IS_VALID(unit, GRFLRPr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRFLRPr, &count); /* Good FCS, bad length */
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    default:
        if (type < snmpValCount) {
            return BCM_E_UNAVAIL;
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "bcmi_stat_preempt_ge_get_set: Statistic not supported: %d\n"), \
                                type));
        return BCM_E_PARAM;
    }

    if (stat_op == _BCM_STAT_GET) {
        *val = count;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_stat_preempt_get_set
 * Description:
 *      Get the specified statistic for a port with preempt mib counter support shared
 *      by XL/CL MAC.
 * Parameters:
 *      unit - PCI device unit number (driver internal)
 *      port - zero-based port number
 *      sync_mode - if 1 read hw counter else sw accumualated counter
 *      type - SNMP statistics type (see stat.h)
 *      val  - (OUT) 64 bit statistic counter value
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_PARAM - Illegal parameter.
 *      BCM_E_INTERNAL - Chip access failure.
 */
int
bcmi_stat_preempt_get_set(int unit, bcm_port_t port,
                             int sync_mode, int stat_op,
                             bcm_stat_val_t type, uint64 *val)
{
    uint64 count, count1;
    int reg_op = (stat_op == _BCM_STAT_GET)? _BCM_STAT_REG_ADD:
                                             _BCM_STAT_REG_CLEAR;
    int reg_sub_op = (stat_op == _BCM_STAT_GET)? _BCM_STAT_REG_SUB:
                                                 _BCM_STAT_REG_CLEAR;

    COMPILER_REFERENCE(&count);  /* Work around PPC compiler bug */
    COMPILER_64_ZERO(count);

    switch (type) {
        /* *** RFC 1213 *** */
    case snmpIfInOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RRBYTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RRBYTr, &count);
        break;
    case snmpIfInUcastPkts:             /* Unicast frames */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RUCAr, &count); /*unicast pkts rcvd */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RUCAr, &count); /*unicast pkts rcvd (preempt counter) */
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RUCr, &count); /* unicast pkts rcvd */
        }
        break;
    case snmpIfInNUcastPkts:             /* Non-unicast frames */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RMCAr, &count); /* + multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RBCAr, &count); /* + broadcast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RMCAr, &count); /* + multicast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RBCAr, &count); /* + broadcast (preempt counter) */
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RUCAr, &count); /* + unicast excl */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RMCAr, &count); /* + multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RBCAr, &count); /* + broadcast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              ROVRr, &count); /* + oversize */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RUCr, &count); /* - unicast */

            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RUCAr, &count); /* + unicast excl (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RMCAr, &count); /* + multicast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RBCAr, &count); /* + broadcast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_ROVRr, &count); /* + oversize (preempt counter) */
        }
        break;
    case snmpIfInBroadcastPkts:          /* Broadcast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RBCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RBCAr, &count);
        break;
    case snmpIfInMulticastPkts:          /* Multicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RMCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RMCAr, &count);
        break;
    case snmpIfInDiscards:           /* Dropped packets including aborted */
        if (SOC_REG_IS_VALID(unit, RDBGC0r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          RDBGC0r, &count); /* Ingress drop conditions */
        }
        BCM_IF_ERROR_RETURN
            (_bcm_stat_counter_non_dma_extra_get(unit,
                                   SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING,
                                                 port, &count1));
        COMPILER_64_ADD_64(count, count1);
        break;
    case snmpIfInErrors: /* RX Errors or Receive packets - non-error frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RFCSr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RJBRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RFCSr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RJBRr, &count);
        if (SOC_REG_IS_VALID(unit, RMTUEr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RMTUEr, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RMTUEr, &count);
        } else if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ROVRr, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_ROVRr, &count);
        }
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RRPKTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RRPKTr, &count);
        break;
    case snmpIfInUnknownProtos:
        break;
    case snmpIfOutOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TBYTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TBYTr, &count);
        break;
    case snmpIfOutUcastPkts:             /* Unicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TUCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TUCAr, &count);
        break;
    case snmpIfOutNUcastPkts:            /* Non-unicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          TMCAr, &count); /* + multicast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          TBCAr, &count); /* + broadcast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          P_TMCAr, &count); /* + multicast (preempt counter) */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          P_TBCAr, &count); /* + broadcast (preempt counter) */
        break;
    case snmpIfOutBroadcastPkts:         /* Broadcast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TBCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TBCAr, &count);
        break;
    case snmpIfOutMulticastPkts:         /* Multicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TMCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TMCAr, &count);
        break;
    case snmpIfOutDiscards:              /* Aged packet counter */
        if (SOC_REG_IS_VALID(unit, HOLDr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              HOLDr, &count);  /* L2 MTU drops */
        } else if (SOC_REG_IS_VALID(unit, HOL_DROPr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, HOL_DROPr, &count);
        }
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC3r, &count);
        BCM_IF_ERROR_RETURN(
            bcm_esw_cosq_stat_get(unit, port, BCM_COS_INVALID,
                                  bcmCosqStatDroppedPackets, &count1));
        COMPILER_64_ADD_64(count, count1);
        break;
    case snmpIfOutErrors:   /* Error packets could not be xmitted */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TERRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TERRr, &count);
        break;
    case snmpIfOutQLen:
        {
            uint32 qcount;
            if (bcm_esw_port_queued_count_get(unit, port, &qcount) >= 0) {
                COMPILER_64_ADD_32(count, qcount);
            }
        }
        break;
    case snmpIpInReceives:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPC4r, &count);
        break;
    case snmpIpInHdrErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4r, &count);
        break;
    case snmpIpForwDatagrams:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC4r, &count);
        break;
    case snmpIpInDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE4r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4r, &count);
        break;

        /* *** RFC 1493 *** */

    case snmpDot1dBasePortDelayExceededDiscards:
        break;
    case snmpDot1dBasePortMtuExceededDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RMTUEr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RMTUEr, &count);
        break;
    case snmpDot1dTpPortInFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPKTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RPKTr, &count);
        break;
    case snmpDot1dTpPortOutFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TPKTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TPKTr, &count);
        break;
    case snmpDot1dPortInDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDISCr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPORTDr, &count);
        break;

        /* *** RFC 1757 *** */

    case snmpEtherStatsDropEvents:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDISCr, &count);
        break;
    case snmpEtherStatsOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          RRBYTr, &count); /* Runts bytes */

        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          P_RRBYTr, &count); /* Runts bytes */
        break;
    case snmpEtherStatsPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          RRPKTr, &count); /* Runts */

        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          P_RRPKTr, &count); /* Runts */
        break;
    case snmpEtherStatsBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RBCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TBCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RBCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TBCAr, &count);
        break;
    case snmpEtherStatsMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RMCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TMCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RMCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TMCAr, &count);
        break;
    case snmpEtherStatsCRCAlignErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RFCSr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RFCSr, &count);
        break;
    case snmpEtherStatsUndersizePkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RUNDr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RUNDr, &count);
        break;
    case snmpEtherStatsOversizePkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ROVRr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TOVRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_ROVRr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TOVRr, &count);
        break;
    case snmpEtherRxOversizePkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ROVRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_ROVRr, &count);
        break;
    case snmpEtherTxOversizePkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TOVRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TOVRr, &count);
        break;
    case snmpEtherStatsFragments:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RFRGr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RFRGr, &count);
        break;
    case snmpEtherStatsJabbers:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RJBRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RJBRr, &count);
        break;
    case snmpEtherStatsCollisions:
        break;

        /* *** rfc1757 definition counts receive packet only *** */

    case snmpEtherStatsPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R64r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T64r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R64r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T64r, &count);
        break;
    case snmpEtherStatsPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R127r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T127r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R127r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T127r, &count);
        break;
    case snmpEtherStatsPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R255r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T255r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R255r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T255r, &count);
        break;
    case snmpEtherStatsPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R511r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T511r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R511r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T511r, &count);
        break;
    case snmpEtherStatsPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R1023r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T1023r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R1023r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T1023r, &count);
        break;
    case snmpEtherStatsPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R1518r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T1518r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R1518r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T1518r, &count);
        break;
    case snmpBcmEtherStatsPkts1519to1522Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RMGVr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TMGVr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RMGVr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TMGVr, &count);
        break;
    case snmpBcmEtherStatsPkts1522to2047Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R2047r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, RMGVr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T2047r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, TMGVr, &count);

        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R2047r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, P_RMGVr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T2047r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, P_TMGVr, &count);
        break;
    case snmpBcmEtherStatsPkts2048to4095Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R4095r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T4095r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R4095r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T4095r, &count);
        break;
    case snmpBcmEtherStatsPkts4095to9216Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R9216r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T9216r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R9216r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T9216r, &count);
        break;
    case snmpBcmEtherStatsPkts9217to16383Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R16383r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T16383r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R16383r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T16383r, &count);
        break;

    case snmpBcmReceivedPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R64r, &count);
        /* preempt counter*/
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R64r, &count);
        break;
    case snmpBcmReceivedPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R127r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R127r, &count);
        break;
    case snmpBcmReceivedPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R255r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R255r, &count);
        break;
    case snmpBcmReceivedPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R511r, &count);
        /* preempt counter*/ 
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R511r, &count);
        break;
    case snmpBcmReceivedPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R1023r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R1023r, &count);
        break;
    case snmpBcmReceivedPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R1518r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R1518r, &count);
        break;
    case snmpBcmReceivedPkts1519to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R2047r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R2047r, &count);
        break;
    case snmpBcmReceivedPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R4095r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R4095r, &count);
        break;
    case snmpBcmReceivedPkts4095to9216Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R9216r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R9216r, &count);
        break;
    case snmpBcmReceivedPkts9217to16383Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R16383r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R16383r, &count);
        break;

    case snmpBcmTransmittedPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T64r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T64r, &count);
        break;
    case snmpBcmTransmittedPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T127r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T127r, &count);
        break;
    case snmpBcmTransmittedPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T255r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T255r, &count);
        break;
    case snmpBcmTransmittedPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T511r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T511r, &count);
        break;
    case snmpBcmTransmittedPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T1023r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T1023r, &count);
        break;
    case snmpBcmTransmittedPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T1518r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T1518r, &count);
        break;
    case snmpBcmTransmittedPkts1519to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T2047r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T2047r, &count);
        break;
    case snmpBcmTransmittedPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T4095r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T4095r, &count);
        break;
    case snmpBcmTransmittedPkts4095to9216Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T9216r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T9216r, &count);
        break;
    case snmpBcmTransmittedPkts9217to16383Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, T16383r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T16383r, &count);
        break;

        /* preempt only counters */
#ifdef BCM_MONTEREY_SUPPORT
    case snmpBcmReceivedPreemptPkts64Octets:
        /* preempt counter*/
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R64r, &count);
        break;
    case snmpBcmReceivedPreemptPkts65to127Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R127r, &count);
        break;
    case snmpBcmReceivedPreemptPkts128to255Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R255r, &count);
        break;
    case snmpBcmReceivedPreemptPkts256to511Octets:
        /* preempt counter*/
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R511r, &count);
        break;
    case snmpBcmReceivedPreemptPkts512to1023Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R1023r, &count);
        break;
    case snmpBcmReceivedPreemptPkts1024to1518Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R1518r, &count);
        break;
    case snmpBcmReceivedPreemptPkts1519to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R2047r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R2047r, &count);
        break;
    case snmpBcmReceivedPreemptPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, R4095r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R4095r, &count);
        break;
    case snmpBcmReceivedPreemptPkts4095to9216Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R9216r, &count);
        break;
    case snmpBcmReceivedPreemptPkts9217to16383Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_R16383r, &count);
        break;

    case snmpBcmTransmittedPreemptPkts64Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T64r, &count);
        break;
    case snmpBcmTransmittedPreemptPkts65to127Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T127r, &count);
        break;
    case snmpBcmTransmittedPreemptPkts128to255Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T255r, &count);
        break;
    case snmpBcmTransmittedPreemptPkts256to511Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T511r, &count);
        break;
    case snmpBcmTransmittedPreemptPkts512to1023Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T1023r, &count);
        break;
    case snmpBcmTransmittedPreemptPkts1024to1518Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T1518r, &count);
        break;
    case snmpBcmTransmittedPreemptPkts1519to2047Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T2047r, &count);
        break;
    case snmpBcmTransmittedPreemptPkts2048to4095Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T4095r, &count);
        break;
    case snmpBcmTransmittedPreemptPkts4095to9216Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T9216r, &count);
        break;
    case snmpBcmTransmittedPreemptPkts9217to16383Octets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_T16383r, &count);
        break;
    case snmpBcmIfInUcastPreemptPkts:
        /* preempt counter */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RUCAr, &count); /*unicast pkts rcvd (preempt counter) */
        }
        break;
    case snmpBcmIfInPreemptOctets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RRBYTr, &count);
        break;
    case snmpBcmIfInPreemptErrors:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RFCSr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RJBRr, &count);
        if (SOC_REG_IS_VALID(unit, RMTUEr)) {
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RMTUEr, &count);
        } else if (COUNT_OVR_ERRORS(unit)) {
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_ROVRr, &count);
        }
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RRPKTr, &count);
        break;
    case snmpBcmIfInMulticastPreemptPkts:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RMCAr, &count);
        break;
    case snmpBcmIfInBroadcastPreemptPkts:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RBCAr, &count);
        break;
    case snmpBcmIfOutPreemptOctets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TBYTr, &count);
        break;
    case snmpBcmIfOutUcastPreemptPkts:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TUCAr, &count);
        break;
    case snmpBcmIfOutPreemptErrors:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TERRr, &count);
        break;
    case snmpBcmIfOutBroadcastPreemptPkts:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TBCAr, &count);
        break;
    case snmpBcmIfOutMulticastPreemptPkts:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TMCAr, &count);
        break;
    case snmpBcmEtherStatsBroadcastPreemptPkts:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RBCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TBCAr, &count);
        break;
    case snmpBcmEtherStatsMulticastPreemptPkts:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RMCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TMCAr, &count);
        break;
    case snmpBcmEtherStatsCRCAlignPreemptErrors:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RFCSr, &count);
        break;
    case snmpBcmEtherStatsUndersizePreemptPkts:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RUNDr, &count);
        break;
    case snmpBcmEtherStatsOversizePreemptPkts:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_ROVRr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TOVRr, &count);
        break;
    case snmpBcmEtherStatsPreemptFragments:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RFRGr, &count);
        break;
    case snmpBcmEtherStatsPreemptJabbers:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RJBRr, &count);
        break;
    case snmpBcmEtherStatsPreemptPkts:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          P_RRPKTr, &count); /* Runts */
        break;
    case snmpBcmEtherStatsPreemptOctets:
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          P_RRBYTr, &count); /* Runts bytes */
        break;
#endif
    case snmpEtherStatsTXNoErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TPOKr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TPOKr, &count);
        break;
    case snmpEtherStatsRXNoErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPOKr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RPOKr, &count);
        break;

        /* *** RFC 2665 *** */

    case snmpDot3StatsAlignmentErrors:
        break;
    case snmpDot3StatsFCSErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RFCSr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RFCSr, &count);
        break;
    case snmpDot3StatsSingleCollisionFrames:
    case snmpDot3StatsMultipleCollisionFrames:
    case snmpDot3StatsSQETTestErrors:
    case snmpDot3StatsDeferredTransmissions:
    case snmpDot3StatsLateCollisions:
    case snmpDot3StatsExcessiveCollisions:
        break;
    case snmpDot3StatsInternalMacTransmitErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TUFLr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TERRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TUFLr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TERRr, &count);
        break;
    case snmpDot3StatsCarrierSenseErrors:
        break;
    case snmpDot3StatsFrameTooLongs:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RMTUEr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RMTUEr, &count);
        break;
    case snmpDot3StatsInternalMacReceiveErrors:
        break;
    case snmpDot3StatsSymbolErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RERPKTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RERPKTr, &count);
        break;
    case snmpDot3ControlInUnknownOpcodes:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RXUOr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RXUOr, &count);
        break;
    case snmpDot3InPauseFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RXPFr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RXPFr, &count);
        break;
    case snmpDot3OutPauseFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TXPFr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TXPFr, &count);
        break;

        /* *** RFC 2233 high capacity versions of RFC1213 objects *** */

    case snmpIfHCInOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RRBYTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RRBYTr, &count);
        break;
    case snmpIfHCInUcastPkts:
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RUCAr, &count);  /* unicast pkts rcvd */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RUCAr, &count);  /* unicast pkts rcvd (preempt counter) */
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RUCr, &count);  /* unicast pkts rcvd */
        }
        break;
    case snmpIfHCInMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RMCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RMCAr, &count);
        break;
    case snmpIfHCInBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RBCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RBCAr, &count);
        break;
    case snmpIfHCOutOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TBYTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TBYTr, &count);
        break;
    case snmpIfHCOutUcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TUCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TUCAr, &count);
        break;
    case snmpIfHCOutMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TMCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TMCAr, &count);
        break;
    case snmpIfHCOutBroadcastPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TBCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TBCAr, &count);
        break;

        /* *** RFC 2465 *** */

    case snmpIpv6IfStatsInReceives:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPC6r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6r, &count);
        break;
    case snmpIpv6IfStatsInHdrErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
        break;
    case snmpIpv6IfStatsInAddrErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE6r, &count);
        break;
    case snmpIpv6IfStatsInDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE6r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
        break;
    case snmpIpv6IfStatsOutForwDatagrams:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC0r, &count);
        break;
    case snmpIpv6IfStatsOutDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC1r, &count);
        break;
    case snmpIpv6IfStatsInMcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6r, &count);
        break;
    case snmpIpv6IfStatsOutMcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC2r, &count);
        break;

        /* *** IEEE 802.1bb *** */
    case snmpIeee8021PfcRequests:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TXPPr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TXPPr, &count);
        break;
    case snmpIeee8021PfcIndications:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RXPPr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RXPPr, &count);
        break;

        /* *** RFC 1284 - unsupported in XGS *** */
    case snmpDot3StatsInRangeLengthError:
        break;

        /* *** RFC 4837 - unsupported in XGS *** */
    case snmpDot3OmpEmulationCRC8Errors:
    case snmpDot3MpcpRxGate:
    case snmpDot3MpcpRxRegister:
    case snmpDot3MpcpTxRegRequest:
    case snmpDot3MpcpTxRegAck:
    case snmpDot3MpcpTxReport:
    case snmpDot3EponFecCorrectedBlocks:
    case snmpDot3EponFecUncorrectableBlocks:
        break;

        /* IPMC counters (broadcom specific) */

    case snmpBcmIPMCBridgedPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC1r, &count);
        break;
    case snmpBcmIPMCRoutedPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP4r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6r, &count);
        break;
    case snmpBcmIPMCInDroppedPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC2r, &count);
        break;
    case snmpBcmIPMCOutDroppedPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC5r, &count);
        break;

        /* EA (broadcom specific) - unsupported in XGS */
    case snmpBcmPonInDroppedOctets:
    case snmpBcmPonOutDroppedOctets:
    case snmpBcmPonInDelayedOctets:
    case snmpBcmPonOutDelayedOctets:
    case snmpBcmPonInDelayedHundredUs:
    case snmpBcmPonOutDelayedHundredUs:
    case snmpBcmPonInFrameErrors:
    case snmpBcmPonInOamFrames:
    case snmpBcmPonOutOamFrames:
    case snmpBcmPonOutUnusedOctets:
        break;

    /* Vlan Tag Frame Counters */
    case snmpBcmRxVlanTagFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RVLNr, &count);
        break;
    case snmpBcmRxDoubleVlanTagFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDVLNr, &count);
        break;
    case snmpBcmTxVlanTagFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TVLNr, &count);
        break;
    case snmpBcmTxDoubleVlanTagFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDVLNr, &count);
        break;

    /* PFC Control Frame Counters */
    case snmpBcmRxPFCControlFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RXPPr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RXPPr, &count);
        break;

    case snmpBcmTxPFCControlFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TXPPr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TXPPr, &count);
        break;

    /* Receive PFC Frame Priority 0 XON to XOFF */
    case snmpBcmRxPFCFrameXonPriority0:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF0r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority1:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF1r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority2:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF2r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority3:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF3r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority4:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF4r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority5:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF5r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority6:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF6r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority7:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF7r, &count);
        break;

    /* Receive PFC Frame Priority */
    case snmpBcmRxPFCFramePriority0:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFC0r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RPFC0r, &count);
        break;

    case snmpBcmRxPFCFramePriority1:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFC1r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RPFC1r, &count);
        break;

    case snmpBcmRxPFCFramePriority2:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFC2r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RPFC2r, &count);
        break;

    case snmpBcmRxPFCFramePriority3:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFC3r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RPFC3r, &count);
        break;

    case snmpBcmRxPFCFramePriority4:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFC4r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RPFC4r, &count);
        break;

    case snmpBcmRxPFCFramePriority5:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFC5r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RPFC5r, &count);
        break;

    case snmpBcmRxPFCFramePriority6:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFC6r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RPFC6r, &count);
        break;

    case snmpBcmRxPFCFramePriority7:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPFC7r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_RPFC7r, &count);
        break;

    /* Transmit PFC Frame Priority */
    case snmpBcmTxPFCFramePriority0:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TPFC0r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TPFC0r, &count);
        break;

    case snmpBcmTxPFCFramePriority1:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TPFC1r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TPFC1r, &count);
        break;

    case snmpBcmTxPFCFramePriority2:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TPFC2r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TPFC2r, &count);
        break;

    case snmpBcmTxPFCFramePriority3:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TPFC3r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TPFC3r, &count);
        break;

    case snmpBcmTxPFCFramePriority4:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TPFC4r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TPFC4r, &count);
        break;

    case snmpBcmTxPFCFramePriority5:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TPFC5r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TPFC5r, &count);
        break;

    case snmpBcmTxPFCFramePriority6:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TPFC6r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TPFC6r, &count);
        break;

    case snmpBcmTxPFCFramePriority7:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TPFC7r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, P_TPFC7r, &count);
        break;

    case snmpFcmPortClass3RxFrames:
    case snmpFcmPortClass3TxFrames:
    case snmpFcmPortClass3Discards:
    case snmpFcmPortClass2RxFrames:
    case snmpFcmPortClass2TxFrames:
    case snmpFcmPortClass2Discards:
    case snmpFcmPortInvalidCRCs:
    case snmpFcmPortDelimiterErrors:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeTxFrag:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_TMFCr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeTxVerifyFrame:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              TVERIFYr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeTxReplyFrame:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              TVERIFYr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFrameAssErrors:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RMFAEr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFrameSmdErrors:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RMFSEr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFrameAss:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RMFAOr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFrag:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RMFCr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxVerifyFrame:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RVERIFYr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxReplyFrame:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RREPLYr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFinalFragSizeError:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RFFMSEr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFragSizeError:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RNFFMSEr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxDiscard:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RMDISr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeHoldCount:
            return BCM_E_UNAVAIL;
        break;

    case snmpBcmRxDot3LengthMismatches:
        if (SOC_REG_IS_VALID(unit, P_RFLRr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              P_RFLRr, &count); /* Good FCS, bad length */
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    default:
        if (type < snmpValCount) {
            return BCM_E_UNAVAIL;
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "bcmi_stat_preempt_xl_get_set: Statistic not supported: %d\n"), \
                                type));
        return BCM_E_PARAM;
    }

    if (stat_op == _BCM_STAT_GET) {
        *val = count;
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      bcmi_stat_preempt_xl_get_set
 * Description:
 *      Get the specified statistic for a port with preempt mib counter support shared
 *      by XL MAC.
 * Parameters:
 *      unit - PCI device unit number (driver internal)
 *      port - zero-based port number
 *      sync_mode - if 1 read hw counter else sw accumualated counter
 *      type - SNMP statistics type (see stat.h)
 *      val  - (OUT) 64 bit statistic counter value
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_PARAM - Illegal parameter.
 *      BCM_E_INTERNAL - Chip access failure.
 */
int
bcmi_stat_preempt_xl_get_set(int unit, bcm_port_t port,
                             int sync_mode, int stat_op,
                             bcm_stat_val_t type, uint64 *val)
{
    uint64 count, count1;
    int reg_op = (stat_op == _BCM_STAT_GET)? _BCM_STAT_REG_ADD:
                                             _BCM_STAT_REG_CLEAR;
    int reg_sub_op = (stat_op == _BCM_STAT_GET)? _BCM_STAT_REG_SUB:
                                                 _BCM_STAT_REG_CLEAR;

    COMPILER_REFERENCE(&count);  /* Work around PPC compiler bug */
    COMPILER_64_ZERO(count);

    switch (type) {
        /* *** RFC 1213 *** */
    case snmpIfInOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RRBYTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RRBYTr, &count);
        break;
    case snmpIfInUcastPkts:             /* Unicast frames */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLMIB_RUCAr, &count); /* unicast pkts rcvd */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RUCAr, &count); /* unicast pkts rcvd (preempt counter) */
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RUCr, &count); /* unicast pkts rcvd */
        }
        break;
    case snmpIfInNUcastPkts:             /* Non-unicast frames */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLMIB_RMCAr, &count); /* + multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLMIB_RBCAr, &count); /* + broadcast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RMCAr, &count); /* + multicast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RBCAr, &count); /* + broadcast (preempt counter) */
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLMIB_RUCAr, &count); /* + unicast excl */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLMIB_RMCAr, &count); /* + multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLMIB_RBCAr, &count); /* + broadcast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLMIB_ROVRr, &count); /* + oversize */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RUCr, &count); /* - unicast */

            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RUCAr, &count); /* + unicast excl (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RMCAr, &count); /* + multicast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RBCAr, &count); /* + broadcast (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_ROVRr, &count); /* + oversize (preempt counter) */
        }
        break;
    case snmpIfInBroadcastPkts:          /* Broadcast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RBCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RBCAr, &count);
        break;
    case snmpIfInMulticastPkts:          /* Multicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RMCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RMCAr, &count);
        break;
    case snmpIfInDiscards:           /* Dropped packets including aborted */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          RDBGC0r, &count); /* Ingress drop conditions */
        BCM_IF_ERROR_RETURN
            (_bcm_stat_counter_non_dma_extra_get(unit,
                                   SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING,
                                                 port, &count1));
        COMPILER_64_ADD_64(count, count1);
        break;
    case snmpIfInErrors: /* RX Errors or Receive packets - non-error frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RFCSr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RJBRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RFCSr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RJBRr, &count);
        if (SOC_REG_IS_VALID(unit, XLMIB_RMTUEr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RMTUEr, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RMTUEr, &count);
        } else if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_ROVRr, &count);
            /* preempt counter */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_ROVRr, &count);
        }
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RRPKTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RRPKTr, &count);
        break;
    case snmpIfInUnknownProtos:
        break;
    case snmpIfOutOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TBYTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TBYTr, &count);
        break;
    case snmpIfOutUcastPkts:             /* Unicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TUCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TUCAr, &count);
        if (!COUNT_OVR_ERRORS(unit)) {
            /* + oversize,  good FCS */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TOVRr, &count);
            /* + oversize,  good FCS (preempt counter) */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TOVRr, &count);
        }
        break;
    case snmpIfOutNUcastPkts:            /* Non-unicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          XLMIB_TMCAr, &count); /* + multicast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          XLMIB_TBCAr, &count); /* + broadcast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          XLPMIB_TMCAr, &count); /* + multicast (preempt counter) */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          XLPMIB_TBCAr, &count); /* + broadcast (preempt counter) */
        break;
    case snmpIfOutBroadcastPkts:         /* Broadcast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TBCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TBCAr, &count);
        break;
    case snmpIfOutMulticastPkts:         /* Multicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TMCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TMCAr, &count);
        break;
    case snmpIfOutDiscards:              /* Aged packet counter */
        if (SOC_REG_IS_VALID(unit, HOLDr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              HOLDr, &count);  /* L2 MTU drops */
        } else if (SOC_REG_IS_VALID(unit, HOL_DROPr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, HOL_DROPr, &count);
        }
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC3r, &count);
        BCM_IF_ERROR_RETURN(
            bcm_esw_cosq_stat_get(unit, port, BCM_COS_INVALID,
                                  bcmCosqStatDroppedPackets, &count1));
        COMPILER_64_ADD_64(count, count1);
        break;
    case snmpIfOutErrors:   /* Error packets could not be xmitted */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TERRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TERRr, &count);
        break;
    case snmpIfOutQLen:
        {
            uint32 qcount;
            if (bcm_esw_port_queued_count_get(unit, port, &qcount) >= 0) {
                COMPILER_64_ADD_32(count, qcount);
            }
        }
        break;
    case snmpIpInReceives:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPC4r, &count);
        break;
    case snmpIpInHdrErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4r, &count);
        break;
    case snmpIpForwDatagrams:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC4r, &count);
        break;
    case snmpIpInDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE4r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4r, &count);
        break;

        /* *** RFC 1493 *** */

    case snmpDot1dBasePortDelayExceededDiscards:
        break;
    case snmpDot1dBasePortMtuExceededDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RMTUEr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RMTUEr, &count);
        break;
    case snmpDot1dTpPortInFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPKTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RPKTr, &count);
        break;
    case snmpDot1dTpPortOutFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TPKTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TPKTr, &count);
        break;
    case snmpDot1dPortInDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDISCr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPORTDr, &count);
        break;

        /* *** RFC 1757 *** */

    case snmpEtherStatsDropEvents:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDISCr, &count);
        break;
    case snmpEtherStatsOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          XLMIB_RRBYTr, &count); /* Runts bytes */

        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          XLPMIB_RRBYTr, &count); /* Runts bytes */
        break;
    case snmpEtherStatsPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          XLMIB_RRPKTr, &count); /* Runts */

        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          XLPMIB_RRPKTr, &count); /* Runts */
        break;
    case snmpEtherStatsBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RBCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TBCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RBCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TBCAr, &count);
        break;
    case snmpEtherStatsMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RMCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TMCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RMCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TMCAr, &count);
        break;
    case snmpEtherStatsCRCAlignErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RFCSr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RFCSr, &count);
        break;
    case snmpEtherStatsUndersizePkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RUNDr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RUNDr, &count);
        break;
    case snmpEtherStatsOversizePkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_ROVRr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TOVRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_ROVRr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TOVRr, &count);
        break;
    case snmpEtherRxOversizePkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_ROVRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_ROVRr, &count);
        break;
    case snmpEtherTxOversizePkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TOVRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TOVRr, &count);
        break;
    case snmpEtherStatsFragments:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RFRGr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RFRGr, &count);
        break;
    case snmpEtherStatsJabbers:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RJBRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RJBRr, &count);
        break;
    case snmpEtherStatsCollisions:
        break;

        /* *** rfc1757 definition counts receive packet only *** */

    case snmpEtherStatsPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R64r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T64r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R64r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T64r, &count);
        break;
    case snmpEtherStatsPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R127r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T127r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R127r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T127r, &count);
        break;
    case snmpEtherStatsPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R255r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T255r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R255r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T255r, &count);
        break;
    case snmpEtherStatsPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R511r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T511r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R511r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T511r, &count);
        break;
    case snmpEtherStatsPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R1023r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T1023r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R1023r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T1023r, &count);
        break;
    case snmpEtherStatsPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R1518r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T1518r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R1518r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T1518r, &count);
        break;
    case snmpBcmEtherStatsPkts1519to1522Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RMGVr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TMGVr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RMGVr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TMGVr, &count);
        break;
    case snmpBcmEtherStatsPkts1522to2047Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R2047r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, XLMIB_RMGVr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T2047r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, XLMIB_TMGVr, &count);

        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R2047r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, XLPMIB_RMGVr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T2047r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, XLPMIB_TMGVr, &count);
        break;
    case snmpBcmEtherStatsPkts2048to4095Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R4095r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T4095r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R4095r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T4095r, &count);
        break;
    case snmpBcmEtherStatsPkts4095to9216Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R9216r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T9216r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R9216r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T9216r, &count);
        break;
    case snmpBcmEtherStatsPkts9217to16383Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R16383r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T16383r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R16383r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T16383r, &count);
        break;

    case snmpBcmReceivedPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R64r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R64r, &count);
        break;
    case snmpBcmReceivedPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R127r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R127r, &count);
        break;
    case snmpBcmReceivedPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R255r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R255r, &count);
        break;
    case snmpBcmReceivedPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R511r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R511r, &count);
        break;
    case snmpBcmReceivedPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R1023r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R1023r, &count);
        break;
    case snmpBcmReceivedPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R1518r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R1518r, &count);
        break;
    case snmpBcmReceivedPkts1519to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R2047r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R2047r, &count);
        break;
    case snmpBcmReceivedPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R4095r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R4095r, &count);
        break;
    case snmpBcmReceivedPkts4095to9216Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R9216r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R9216r, &count);
        break;
    case snmpBcmReceivedPkts9217to16383Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_R16383r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_R16383r, &count);
        break;

    case snmpBcmTransmittedPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T64r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T64r, &count);
        break;
    case snmpBcmTransmittedPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T127r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T127r, &count);
        break;
    case snmpBcmTransmittedPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T255r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T255r, &count);
        break;
    case snmpBcmTransmittedPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T511r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T511r, &count);
        break;
    case snmpBcmTransmittedPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T1023r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T1023r, &count);
        break;
    case snmpBcmTransmittedPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T1518r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T1518r, &count);
        break;
    case snmpBcmTransmittedPkts1519to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T2047r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T2047r, &count);
        break;
    case snmpBcmTransmittedPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T4095r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T4095r, &count);
        break;
    case snmpBcmTransmittedPkts4095to9216Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T9216r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T9216r, &count);
        break;
    case snmpBcmTransmittedPkts9217to16383Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_T16383r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_T16383r, &count);
        break;

    case snmpEtherStatsTXNoErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TPOKr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TPOKr, &count);
        break;
    case snmpEtherStatsRXNoErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPOKr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RPOKr, &count);
        break;

        /* *** RFC 2665 *** */

    case snmpDot3StatsAlignmentErrors:
        break;
    case snmpDot3StatsFCSErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RFCSr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RFCSr, &count);
        break;
    case snmpDot3StatsSingleCollisionFrames:
    case snmpDot3StatsMultipleCollisionFrames:
    case snmpDot3StatsSQETTestErrors:
    case snmpDot3StatsDeferredTransmissions:
    case snmpDot3StatsLateCollisions:
    case snmpDot3StatsExcessiveCollisions:
        break;
    case snmpDot3StatsInternalMacTransmitErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TUFLr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TERRr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TUFLr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TERRr, &count);
        break;
    case snmpDot3StatsCarrierSenseErrors:
        break;
    case snmpDot3StatsFrameTooLongs:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RMTUEr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RMTUEr, &count);
        break;
    case snmpDot3StatsInternalMacReceiveErrors:
        break;
    case snmpDot3StatsSymbolErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RERPKTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RERPKTr, &count);
        break;
    case snmpDot3ControlInUnknownOpcodes:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RXUOr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RXUOr, &count);
        break;
    case snmpDot3InPauseFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RXPFr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RXPFr, &count);
        break;
    case snmpDot3OutPauseFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TXPFr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TXPFr, &count);
        break;

        /* *** RFC 2233 high capacity versions of RFC1213 objects *** */

    case snmpIfHCInOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RRBYTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RRBYTr, &count);
        break;
    case snmpIfHCInUcastPkts:
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLMIB_RUCAr, &count);  /* unicast pkts rcvd */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RUCAr, &count);  /* unicast pkts rcvd (preempt counter) */
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RUCr, &count);  /* unicast pkts rcvd */
        }
        break;
    case snmpIfHCInMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RMCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RMCAr, &count);
        break;
    case snmpIfHCInBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RBCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RBCAr, &count);
        break;
    case snmpIfHCOutOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TBYTr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TBYTr, &count);
        break;
    case snmpIfHCOutUcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TUCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TUCAr, &count);
        break;
    case snmpIfHCOutMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TMCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TMCAr, &count);
        break;
    case snmpIfHCOutBroadcastPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TBCAr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TBCAr, &count);
        break;

        /* *** RFC 2465 *** */

    case snmpIpv6IfStatsInReceives:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPC6r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6r, &count);
        break;
    case snmpIpv6IfStatsInHdrErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
        break;
    case snmpIpv6IfStatsInAddrErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE6r, &count);
        break;
    case snmpIpv6IfStatsInDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE6r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
        break;
    case snmpIpv6IfStatsOutForwDatagrams:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC0r, &count);
        break;
    case snmpIpv6IfStatsOutDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC1r, &count);
        break;
    case snmpIpv6IfStatsInMcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6r, &count);
        break;
    case snmpIpv6IfStatsOutMcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC2r, &count);
        break;

        /* *** IEEE 802.1bb *** */
    case snmpIeee8021PfcRequests:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TXPPr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TXPPr, &count);
        break;
    case snmpIeee8021PfcIndications:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RXPPr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RXPPr, &count);
        break;

        /* *** RFC 1284 - unsupported in XGS *** */
    case snmpDot3StatsInRangeLengthError:
        break;

        /* *** RFC 4837 - unsupported in XGS *** */
    case snmpDot3OmpEmulationCRC8Errors:
    case snmpDot3MpcpRxGate:
    case snmpDot3MpcpRxRegister:
    case snmpDot3MpcpTxRegRequest:
    case snmpDot3MpcpTxRegAck:
    case snmpDot3MpcpTxReport:
    case snmpDot3EponFecCorrectedBlocks:
    case snmpDot3EponFecUncorrectableBlocks:
        break;

        /* IPMC counters (broadcom specific) */

    case snmpBcmIPMCBridgedPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC1r, &count);
        break;
    case snmpBcmIPMCRoutedPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP4r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6r, &count);
        break;
    case snmpBcmIPMCInDroppedPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC2r, &count);
        break;
    case snmpBcmIPMCOutDroppedPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC5r, &count);
        break;

        /* EA (broadcom specific) - unsupported in XGS */
    case snmpBcmPonInDroppedOctets:
    case snmpBcmPonOutDroppedOctets:
    case snmpBcmPonInDelayedOctets:
    case snmpBcmPonOutDelayedOctets:
    case snmpBcmPonInDelayedHundredUs:
    case snmpBcmPonOutDelayedHundredUs:
    case snmpBcmPonInFrameErrors:
    case snmpBcmPonInOamFrames:
    case snmpBcmPonOutOamFrames:
    case snmpBcmPonOutUnusedOctets:
        break;

    /* Vlan Tag Frame Counters */
    case snmpBcmRxVlanTagFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RVLNr, &count);
        break;
    case snmpBcmRxDoubleVlanTagFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDVLNr, &count);
        break;
    case snmpBcmTxVlanTagFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TVLNr, &count);
        break;
    case snmpBcmTxDoubleVlanTagFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDVLNr, &count);
        break;

    /* PFC Control Frame Counters */
    case snmpBcmRxPFCControlFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RXPPr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RXPPr, &count);
        break;

    case snmpBcmTxPFCControlFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TXPPr, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TXPPr, &count);
        break;

    /* Receive PFC Frame Priority 0 XON to XOFF */
    case snmpBcmRxPFCFrameXonPriority0:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFCOFF0r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority1:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFCOFF1r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority2:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFCOFF2r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority3:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFCOFF3r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority4:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFCOFF4r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority5:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFCOFF5r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority6:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFCOFF6r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority7:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFCOFF7r, &count);
        break;

    /* Receive PFC Frame Priority */
    case snmpBcmRxPFCFramePriority0:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFC0r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RPFC0r, &count);
        break;

    case snmpBcmRxPFCFramePriority1:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFC1r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RPFC1r, &count);
        break;

    case snmpBcmRxPFCFramePriority2:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFC2r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RPFC2r, &count);
        break;

    case snmpBcmRxPFCFramePriority3:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFC3r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RPFC3r, &count);
        break;

    case snmpBcmRxPFCFramePriority4:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFC4r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RPFC4r, &count);
        break;

    case snmpBcmRxPFCFramePriority5:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFC5r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RPFC5r, &count);
        break;

    case snmpBcmRxPFCFramePriority6:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFC6r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RPFC6r, &count);
        break;

    case snmpBcmRxPFCFramePriority7:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_RPFC7r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_RPFC7r, &count);
        break;

    /* Transmit PFC Frame Priority */
    case snmpBcmTxPFCFramePriority0:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TPFC0r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TPFC0r, &count);
        break;

    case snmpBcmTxPFCFramePriority1:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TPFC1r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TPFC1r, &count);
        break;

    case snmpBcmTxPFCFramePriority2:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TPFC2r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TPFC2r, &count);
        break;

    case snmpBcmTxPFCFramePriority3:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TPFC3r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TPFC3r, &count);
        break;

    case snmpBcmTxPFCFramePriority4:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TPFC4r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TPFC4r, &count);
        break;

    case snmpBcmTxPFCFramePriority5:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TPFC5r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TPFC5r, &count);
        break;

    case snmpBcmTxPFCFramePriority6:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TPFC6r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TPFC6r, &count);
        break;

    case snmpBcmTxPFCFramePriority7:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLMIB_TPFC7r, &count);
        /* preempt counter */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XLPMIB_TPFC7r, &count);
        break;

    case snmpFcmPortClass3RxFrames:
    case snmpFcmPortClass3TxFrames:
    case snmpFcmPortClass3Discards:
    case snmpFcmPortClass2RxFrames:
    case snmpFcmPortClass2TxFrames:
    case snmpFcmPortClass2Discards:
    case snmpFcmPortInvalidCRCs:
    case snmpFcmPortDelimiterErrors:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeTxFrag:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_TMFCr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeTxVerifyFrame:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLMIB_TVERIFYr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeTxReplyFrame:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLMIB_TREPLYr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFrameAssErrors:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RMFAEr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFrameSmdErrors:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RMFSEr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFrameAss:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RMFAOr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFrag:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RMFCr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxVerifyFrame:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLMIB_RVERIFYr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxReplyFrame:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLMIB_RREPLYr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFinalFragSizeError:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RFFMSEr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxFragSizeError:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RNFFMSEr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeRxDiscard:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RMDISr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmMacMergeHoldCount:
        if (soc_feature(unit, soc_feature_preemption)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              MACMERGERHOLDCOUNTr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmRxDot3LengthMismatches:
        if (SOC_REG_IS_VALID(unit, XLMIB_RFLRr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLMIB_RFLRr, &count); /* Good FCS, bad length */
        } else {
            return BCM_E_UNAVAIL;
        }

        /* Preempt counter */
        if (SOC_REG_IS_VALID(unit, XLPMIB_RFLRr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              XLPMIB_RFLRr, &count); /* Good FCS, bad length */
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    default:
        if (type < snmpValCount) {
            return BCM_E_UNAVAIL;
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "bcmi_stat_preempt_xl_get_set: Statistic not supported: %d\n"), \
                                type));
        return BCM_E_PARAM;
    }

    if (stat_op == _BCM_STAT_GET) {
        *val = count;
    }

    return BCM_E_NONE;
  }
