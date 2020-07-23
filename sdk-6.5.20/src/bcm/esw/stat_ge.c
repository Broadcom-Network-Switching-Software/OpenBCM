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
 *      _bcm_stat_ge_get_set
 * Description:
 *      Get the specified statistic for a GE port in 1000 Mb mode on the
 *      StrataSwitch family of devices.
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
_bcm_stat_ge_get_set(int unit, bcm_port_t port, 
                     int sync_mode, int stat_op, 
                     bcm_stat_val_t type, uint64 *val, 
                     int incl_non_ge_stat)
{
    uint64 count, count1;
    int reg_op = (stat_op == _BCM_STAT_GET)? _BCM_STAT_REG_ADD: 
                                             _BCM_STAT_REG_CLEAR;
    int reg_sub_op = (stat_op == _BCM_STAT_GET)? _BCM_STAT_REG_SUB: 
                                             _BCM_STAT_REG_CLEAR;

#ifdef BCM_TOMAHAWK_SUPPORT
    soc_ctr_control_info_t ctrl_info;
    ctrl_info.instance = -1;
#endif /* BCM_TOMAHAWK_SUPPORT */

    COMPILER_REFERENCE(&count);  /* Work around PPC compiler bug */
    COMPILER_REFERENCE(&count1);
    COMPILER_64_ZERO(count);
    COMPILER_64_ZERO(count1);

    switch (type) {
        /* *** RFC 1213 *** */

    case snmpIfInOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRBYTr, &count);  /* bytes rcvd */
        if (SOC_REG_IS_VALID(unit, GRRBYTr)) {
           BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                            GRRBYTr, &count); /* Runt bytes */
        } else { 
           BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                            RRBYTr, &count); /* Runt bytes */
        }
        break;
    case snmpIfInUcastPkts:    /* Unicast packets received */
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            if (SOC_REG_IS_VALID(unit, GRUCr)
                && !((SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit)) 
                      && SOC_IS_STACK_PORT(unit, port))) {
                if (COUNT_OVR_ERRORS(unit)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRUCr, &count);
                } else {
                    if (incl_non_ge_stat && SOC_REG_IS_VALID(unit, RUC_64r)) {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RUC_64r,
                                &count); /* unicast pkts rcvd */
                    } else {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRUCr, &count);
                        BCM_STAT_REG_OPER(unit, port,
                                sync_mode, reg_op,
                                GROVRr, &count); /* + oversize, good FCS */
                    }
                }
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
            if (SOC_REG_IS_VALID(unit, GRMTUEr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRMTUEr, 
                                 &count); /* - mtu exceeded, good FCS */
            }
            if (SOC_REG_IS_VALID(unit, GRFLRr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GRFLRr, &count); /* - good FCS, bad length */
            }
            if (SOC_REG_IS_VALID(unit, GRXCFr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GRXCFr, &count); /* - good FCS, all MAC ctrl */
            }

            if ((SOC_REG_IS_VALID(unit, GROVRr)) &&
                (COUNT_OVR_ERRORS(unit))) {
               BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GROVRr, 
                                &count); /* - oversize, good FCS */
            }
        }
        break;
    case snmpIfInNUcastPkts:    /* Non Unicast packets received */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                    GRMCAr, &count); /*  multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                    GRBCAr, &count); /*  broadcast */
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                    GRUCr, &count); /*  unicast excl */
                                    /*   oversize */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                    GRMCAr, &count); /*  multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                    GRBCAr, &count); /*  broadcast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                    GROVRr, &count); /*  oversize */
            if (SOC_REG_IS_VALID(unit, RUC_64r)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, RUC_64r, &count); /* - unicast */
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, RUCr, &count); /* - unicast */
            }
        }
        break;
    case snmpIfInBroadcastPkts: /* Broadcast packets received */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GRBCAr, &count);  /* broadcast pkts rcvd */
        break;
    case snmpIfInMulticastPkts:  /* Multicast packets received */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GRMCAr, &count);  /* multicast pkts rcvd */
        break;
    case snmpIfInDiscards:    /* Dropped packets including aborted */
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, RDBGC0_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                      RDBGC0_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                      RDBGC0r, &count); /* Ingress drop conditions */
                }
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
        if (SOC_REG_IS_VALID(unit, GRMTUEr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                             GRMTUEr, &count); /* mtu exceeded pkts, good FCS */
        } else if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                             GROVRr, &count);    /* oversize pkts, good FCS */
        }
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GRJBRr, &count);  /* oversize pkts, bad FCS */
        break;
    case snmpIfInUnknownProtos:
        break;
    case snmpIfOutOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GTBYTr, &count);  /* transmit bytes */
        break;
    case snmpIfOutUcastPkts:
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                             GTUCr, &count);  /* unicast pkts sent */
            if (!COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                 GTOVRr, &count); /* + oversize, good FCS */
            }
        } else 
#endif /* BCM_RAPTOR_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        {
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
            if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                                 GTOVRr, &count); /* - oversize, good FCS */
            }
        }
        break;
    case snmpIfOutNUcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GTMCAr, &count);  /* multicast pkts */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GTBCAr, &count);  /* broadcast pkts */
        break;
    case snmpIfOutBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GTBCAr, &count);  /* broadcast pkts */
        break;
    case snmpIfOutMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GTMCAr, &count);  /* multicast pkts */
        break;
    case snmpIfOutDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GTEDFr, &count);  /* multiple deferral */
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, HOLDr)) {  /* L2 MTU drops */
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                     HOLDr, &count);
                } else if (SOC_REG_IS_VALID(unit, HOL_DROPr)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                     HOL_DROPr, &count);
                }
                if (SOC_REG_IS_VALID(unit, TDBGC3_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC3_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC3r, &count);
                }
                if (SOC_REG_IS_VALID(unit, EGRDROPPKTCOUNTr)) {
                    BCM_IF_ERROR_RETURN(_bcm_stat_counter_extra_get
                        (unit, EGRDROPPKTCOUNTr, port, &count1));
                }
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
        break;
    case snmpIfOutQLen: {
        uint32  qcount;
        if (incl_non_ge_stat) {
            if (bcm_esw_port_queued_count_get(unit, port, &qcount) >= 0) { 
                COMPILER_64_ADD_32(count, qcount);
            }
        }
    }
    break;
    case snmpIpInReceives:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, RIPC4_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPC4_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPC4r, &count);
                }
            }
        }
        break;
    case snmpIpInHdrErrors:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, RIPD4_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4r, &count);
                }
            }
        }
        break;
/* ipInAddrErrors */
    case snmpIpForwDatagrams:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, TDBGC4_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC4_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC4r, &count);
                }
            }
        }
        break;
/* ipInUnknownProtos */
    case snmpIpInDiscards:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, RIPHE4_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE4_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE4r, &count);
                }
                if (SOC_REG_IS_VALID(unit, RIPD4_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4r, &count);
                }
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
        break;
    case snmpDot1dBasePortMtuExceededDiscards:
        if (SOC_REG_IS_VALID(unit, GRMTUEr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                             GRMTUEr, &count); /* mtu exceeded pkts */
        } else if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                             GROVRr, &count);  /* oversize pkts */
        }
        break;
    case snmpDot1dTpPortInFrames:  /* should be only bridge mgmt */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPKTr, &count);
        break;
    case snmpDot1dTpPortOutFrames:  /* should be only bridge mgmt */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTPKTr, &count);
        break;
    case snmpDot1dPortInDiscards:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, RDISC_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDISC_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDISCr, &count);
                }
                if (SOC_REG_IS_VALID(unit, RIPD4_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4r, &count);
                }
                if (SOC_REG_IS_VALID(unit, RIPD6_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
                }
                if (SOC_REG_IS_VALID(unit, RPORTD_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPORTD_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RPORTDr, &count);
                }
            }
        }
        break;

        /* *** RFC 1757 *** */

    case snmpEtherStatsDropEvents:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, RDISC_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDISC_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDISCr, &count);
                }
            }
        }
        break;
    case snmpEtherStatsOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTBYTr, &count);
        if (SOC_REG_IS_VALID(unit, GRRBYTr)) {
           BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                            GRRBYTr, &count); /* Runt bytes */
        } else { 
           BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                            RRBYTr, &count); /* Runt bytes */
        }
        break;
    case snmpEtherStatsPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTPKTr, &count);
        if (SOC_REG_IS_VALID(unit, GRRPKTr)) {
           BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                            GRRPKTr, &count); /* Runt packets */
        } else {
           BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                            RRPKTr, &count); /* Runt packets */
        }
        break;
    case snmpEtherStatsBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRBCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTBCAr, &count);
        break;
    case snmpEtherStatsMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRMCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTMCAr, &count);
        break;
    case snmpEtherStatsCRCAlignErrors:  /* CRC errors + alignment errors */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRFCSr, &count);
        break;
    case snmpEtherStatsUndersizePkts:  /* Undersize frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRUNDr, &count);
        break;
    case snmpEtherStatsOversizePkts:
        if (soc_feature(unit, soc_feature_stat_jumbo_adj)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GROVRr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTOVRr, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRJBRr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTJBRr, &count);
        }
        break;
    case snmpEtherRxOversizePkts:
        if (soc_feature(unit, soc_feature_stat_jumbo_adj)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GROVRr, &count);
        }
        break;
    case snmpEtherTxOversizePkts:
        if (soc_feature(unit, soc_feature_stat_jumbo_adj)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTOVRr, &count);
        }
        break;
    case snmpEtherStatsFragments:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRFRGr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTFRGr, &count);
        break;
    case snmpEtherStatsJabbers:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRJBRr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTJBRr, &count);
        break;
    case snmpEtherStatsCollisions:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTNCLr, &count);
        break;
    case snmpEtherStatsPkts64Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {  
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR64r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT64r, &count);
        }
        break;
    case snmpEtherStatsPkts65to127Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR127r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT127r, &count);
        }
        break;
    case snmpEtherStatsPkts128to255Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR255r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT255r, &count);
        }
        break;
    case snmpEtherStatsPkts256to511Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR511r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT511r, &count);
        }
        break;
    case snmpEtherStatsPkts512to1023Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR1023r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT1023r, &count);
        }
        break;
    case snmpEtherStatsPkts1024to1518Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR1518r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT1518r, &count);
        }
        break;

        /* *** not actually in rfc1757 *** */

    case snmpBcmEtherStatsPkts1519to1522Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRMGVr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTMGVr, &count);
        }
        break;
    case snmpBcmEtherStatsPkts1522to2047Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR2047r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRMGVr, &count);

            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT2047r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GTMGVr, &count);
        }
        break;
    case snmpBcmEtherStatsPkts2048to4095Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR4095r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT4095r, &count);
        }
        break;
    case snmpBcmEtherStatsPkts4095to9216Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR9216r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT9216r, &count);
        }
        break;
#ifdef BCM_KATANA_SUPPORT
    case snmpBcmEtherStatsPkts9217to16383Octets: /* not in rfc1757 */
        if(SOC_IS_KATANA(unit))
        {
            if (soc_feature(unit, soc_feature_stat_xgs3)) {
               BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                GR16383r, &count);
               BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                GT16383r, &count);
            }
        }
        break;
#endif
    case snmpBcmReceivedPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR64r, &count);
        break;
    case snmpBcmReceivedPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR127r, &count);
        break;
    case snmpBcmReceivedPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR255r, &count);
        break;
    case snmpBcmReceivedPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR511r, &count);
        break;
    case snmpBcmReceivedPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR1023r, &count);
        break;
    case snmpBcmReceivedPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR1518r, &count);
        break;
    case snmpBcmReceivedPkts1519to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR2047r, &count);
        break;
    case snmpBcmReceivedPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR4095r, &count);
        break;
    case snmpBcmReceivedPkts4095to9216Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR9216r, &count);
        break;
    case snmpBcmReceivedPkts9217to16383Octets:
        if (SOC_REG_IS_VALID(unit, GR16383r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GR16383r, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case snmpBcmTransmittedPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT64r, &count);
        break;
    case snmpBcmTransmittedPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT127r, &count);
        break;
    case snmpBcmTransmittedPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT255r, &count);
        break;
    case snmpBcmTransmittedPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT511r, &count);
        break;
    case snmpBcmTransmittedPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT1023r, &count);
        break;
    case snmpBcmTransmittedPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT1518r, &count);
        break;
    case snmpBcmTransmittedPkts1519to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT2047r, &count);
        break;
    case snmpBcmTransmittedPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT4095r, &count);
        break;
    case snmpBcmTransmittedPkts4095to9216Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT9216r, &count);
        break;
    case snmpBcmTransmittedPkts9217to16383Octets:
        if (SOC_REG_IS_VALID(unit, GT16383r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GT16383r, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case snmpEtherStatsTXNoErrors:
        /* FE = TPKT - (TNCL + TOVR + TFRG + TUND) */
        /* GE = GTPKT - (GTOVR + GTIPD + GTABRT) */
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                             GTPOKr, &count); /* All good packets */
        } else 
#endif /* BCM_RAPTOR_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                             GTPKTr, &count); /* All Packets */
            if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                                 GTOVRr, &count); /* Oversize */
            }
        }
        break;
    case snmpEtherStatsRXNoErrors:
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
        /* Some chips have a dedicated register for this stat */
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPOKr, &count);
        } else 
#endif /* BCM_RAPTOR_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        {
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
            if (SOC_REG_IS_VALID(unit, GRMTUEr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                                 GRMTUEr, &count); /* mtu exceeded pkts */
            } else if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                                 GROVRr, &count);  /* oversize pkts */
            }
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, GRJBRr, &count);
        }
        break;
        /* *** RFC 2665 *** */

    case snmpDot3StatsAlignmentErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRALNr, &count);
        break;
    case snmpDot3StatsFCSErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRFCSr, &count);
        break;
    case snmpDot3StatsSingleCollisionFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTSCLr, &count);
        break;
    case snmpDot3StatsMultipleCollisionFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTMCLr, &count);
        break;
    case snmpDot3StatsSQETTestErrors:
        /* always 0 */
        break;
    case snmpDot3StatsDeferredTransmissions:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTDFRr, &count);
        break;
    case snmpDot3StatsLateCollisions:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTLCLr, &count);
        break;
    case snmpDot3StatsExcessiveCollisions:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTXCLr, &count);
        break;
    case snmpDot3StatsInternalMacTransmitErrors:
        /* always 0 */
        break;
    case snmpDot3StatsCarrierSenseErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRFCRr, &count);
        break;
    case snmpDot3StatsFrameTooLongs:
        if (soc_feature(unit, soc_feature_stat_jumbo_adj)) {
            if (SOC_REG_IS_VALID(unit, GRMTUEr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                 GRMTUEr, &count); /* mtu exceeded pkts */
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                 GROVRr, &count); /* oversized pkts */
            }
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRJBRr, &count);
        }
        break;
    case snmpDot3StatsInternalMacReceiveErrors:
        /* always 0 */
        break;
    case snmpDot3StatsSymbolErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRCDEr, &count);
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
        if (SOC_REG_IS_VALID(unit, GRRBYTr)) {
           BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                            GRRBYTr, &count); /* Runt bytes */
        } else { 
           BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                            RRBYTr, &count); /* Runt bytes */
        }
        break;
    case snmpIfHCInUcastPkts:
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            if (SOC_REG_IS_VALID(unit, GRUCr)
                && !((SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit)) 
                      && SOC_IS_STACK_PORT(unit, port))) {
                if (COUNT_OVR_ERRORS(unit)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRUCr, &count);
                } else {
                    if (incl_non_ge_stat && SOC_REG_IS_VALID(unit, RUC_64r)) {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RUC_64r,
                                &count); /* unicast pkts rcvd */
                    } else {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRUCr, &count);
                        BCM_STAT_REG_OPER(unit, port,
                                sync_mode, reg_op,
                                GROVRr, &count); /* + oversize, good FCS */
                    }
                }
            } else {
                if (incl_non_ge_stat) {
                    /* RUC switch register has the count of all received packets, 
                     * add the count when incl_non_ge_stat flag is set.
                     */
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RUCr, &count);  /* unicast pkts rcvd */
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
            if (SOC_REG_IS_VALID(unit, GRMTUEr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                                 GRMTUEr, &count); /* mtu exceeded pkts */
            } else if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                                 GROVRr, &count);  /* oversize pkts */
            }
            if (SOC_REG_IS_VALID(unit, GRFLRr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GRFLRr, &count); /* - good FCS, bad length */
            }
            if (SOC_REG_IS_VALID(unit, GRXCFr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  GRXCFr, &count); /* - good FCS, all MAC ctrl */
            }
        }
        break;
    case snmpIfHCInMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GRMCAr, &count);  /* multicast */
        break;
    case snmpIfHCInBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GRBCAr, &count);  /* broadcast */
        break;
    case snmpIfHCOutOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GTBYTr, &count);  /* transmit bytes */
        break;
    case snmpIfHCOutUcastPkts:
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                             GTUCr, &count); /* All good packets */
            if (!COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                  GTOVRr, &count);  /* + oversize pkts */
            }
        } else 
#endif /* BCM_RAPTOR_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        { 
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
            if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                                 GTOVRr, &count); /* - oversize, good FCS */
            }
        }
        break;
    case snmpIfHCOutMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GTMCAr, &count);  /* multicast xmited */
        break;
    case snmpIfHCOutBroadcastPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                         GTBCAr, &count);  /* broadcast xmited */
        break;

        /* *** RFC 2465 *** */

    case snmpIpv6IfStatsInReceives:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, RIPC6_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPC6_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPC6r, &count);
                }
                if (SOC_REG_IS_VALID(unit, IMRP6_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6r, &count);
                }
            }
        }
        break;
    case snmpIpv6IfStatsInHdrErrors:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, RIPD6_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
                }
            }
        }
        break;
    case snmpIpv6IfStatsInAddrErrors:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, RIPHE6_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE6_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE6r, &count);
                }
            }
        }
        break;
    case snmpIpv6IfStatsInDiscards:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, RIPHE6_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE6_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE6r, &count);
                }
                if (SOC_REG_IS_VALID(unit, RIPD6_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
                }
            }
        }
        break;
    case snmpIpv6IfStatsOutForwDatagrams:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, TDBGC0_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC0_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC0r, &count);
                }
            }
        }
        break;
    case snmpIpv6IfStatsOutDiscards:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, TDBGC1_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC1_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC1r, &count);
                }
            }
        }
        break;
    case snmpIpv6IfStatsInMcastPkts:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, IMRP6_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6r, &count);
                }
            }
        }
        break;
    case snmpIpv6IfStatsOutMcastPkts:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (incl_non_ge_stat) {
                if (SOC_REG_IS_VALID(unit, TDBGC2_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC2_64r, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC2r, &count);
                }
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
                    if (SOC_REG_IS_VALID(unit, RDBGC1_64r)) {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC1_64r, &count);
                    } else {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC1r, &count);
                    }
                }
            }
        }
        break;
    case snmpBcmIPMCRoutedPckts:
        if (soc_feature(unit, soc_feature_ip_mcast)) {
            if (soc_feature(unit, soc_feature_stat_xgs3)) {
                if (incl_non_ge_stat) {
                    if (SOC_REG_IS_VALID(unit, IMRP4_64r)) {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP4_64r, &count);
                    } else {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP4r, &count);
                    }
                    if (SOC_REG_IS_VALID(unit, IMRP6_64r)) {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6_64r, &count);
                    } else {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6r, &count);
                    }
                }
            }
        }
        break;
    case snmpBcmIPMCInDroppedPckts:
        if (soc_feature(unit, soc_feature_ip_mcast)) {
            if (soc_feature(unit, soc_feature_stat_xgs3)) {
                if (incl_non_ge_stat) {
                    if (SOC_REG_IS_VALID(unit, RDBGC2_64r)) {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC2_64r, &count);
                    } else {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC2r, &count);
                    }
                }
            }
        }
        break;
    case snmpBcmIPMCOutDroppedPckts:
        if (soc_feature(unit, soc_feature_ip_mcast)) {
            if (soc_feature(unit, soc_feature_stat_xgs3)) {
                if (incl_non_ge_stat) {
                    if (SOC_REG_IS_VALID(unit, TDBGC5_64r)) {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC5_64r, &count);
                    } else {
                        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC5r, &count);
                    }
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

    /* PFC Control Frame Counters */
    case snmpBcmRxPFCControlFrame:
        if (SOC_REG_IS_VALID(unit, GRPFCr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GRPFCr, &count);
        }
        break;

    case snmpBcmTxPFCControlFrame:
        if (SOC_REG_IS_VALID(unit, GTPFCr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, GTPFCr, &count);
        }
        break;

    case snmpFcmPortClass3RxFrames:
        if (soc_feature(unit, soc_feature_fcoe)) {
            if (SOC_REG_IS_VALID(unit, RDBGC3_64r)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC3_64r, &count);
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC3r, &count);
            }
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpFcmPortClass3TxFrames:
        if (soc_feature(unit, soc_feature_fcoe)) {
            if (SOC_REG_IS_VALID(unit, TDBGC6_64r)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC6_64r, &count);
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC6r, &count);
            }
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpFcmPortClass3Discards:
        if (soc_feature(unit, soc_feature_fcoe)) {
            if (SOC_REG_IS_VALID(unit, RDBGC4_64r)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC4_64r, &count);
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC4r, &count);
            }
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpFcmPortClass2RxFrames:
        if (soc_feature(unit, soc_feature_fcoe)) {
            if (SOC_REG_IS_VALID(unit, RDBGC5_64r)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC5_64r, &count);
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC5r, &count);
            }
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpFcmPortClass2TxFrames:
        if (soc_feature(unit, soc_feature_fcoe)) {
            if (SOC_REG_IS_VALID(unit, TDBGC7_64r)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC7_64r, &count);
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC7r, &count);
            }
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpFcmPortClass2Discards:
        if (soc_feature(unit, soc_feature_fcoe)) {
            if (SOC_REG_IS_VALID(unit, RDBGC6_64r)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC6_64r, &count);
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC6r, &count);
            }
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpFcmPortInvalidCRCs:
        if (soc_feature(unit, soc_feature_fcoe)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                             EGR_FCOE_INVALID_CRC_FRAMESr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpFcmPortDelimiterErrors:
        if (soc_feature(unit, soc_feature_fcoe)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                             EGR_FCOE_DELIMITER_ERROR_FRAMESr, 
                    &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
#ifdef BCM_TOMAHAWK_SUPPORT
    case snmpBcmSampleIngressPkts:
    case snmpBcmSampleIngressSnapshotPkts:
    case snmpBcmSampleIngressSampledPkts:
        if (soc_feature(unit, soc_feature_sflow_counters)) {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_SFLOW;

            if (type == snmpBcmSampleIngressPkts) {
                ctrl_info.instance = SOC_SFLOW_CTR_TYPE_SAMPLE_POOL;
            } else if (type == snmpBcmSampleIngressSnapshotPkts) {
                ctrl_info.instance = SOC_SFLOW_CTR_TYPE_SAMPLE_POOL_SNAPSHOT;
            } else { /* snmpBcmSampleIngressSampledPkts */
                ctrl_info.instance = SOC_SFLOW_CTR_TYPE_SAMPLE_COUNT;
            }

            if (stat_op == _BCM_STAT_GET) {
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_get(unit,
                                             SOC_COUNTER_NON_DMA_SFLOW_ING_PKT,
                                             ctrl_info, sync_mode, port,
                                             &count));
            } else { /* _BCM_STAT_CLEAR */
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_set(unit,
                                             SOC_COUNTER_NON_DMA_SFLOW_ING_PKT,
                                             ctrl_info, 0, port, count));
            }
        }
        break;
    case snmpBcmSampleFlexPkts:
    case snmpBcmSampleFlexSnapshotPkts:
    case snmpBcmSampleFlexSampledPkts:
        if (soc_feature(unit, soc_feature_sflow_counters))  {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_SFLOW;

            if (type == snmpBcmSampleFlexPkts) {
                ctrl_info.instance = SOC_SFLOW_CTR_TYPE_SAMPLE_POOL;
            } else if (type == snmpBcmSampleFlexSnapshotPkts) {
                ctrl_info.instance = SOC_SFLOW_CTR_TYPE_SAMPLE_POOL_SNAPSHOT;
            } else { /* snmpBcmSampleFlexSampledPkts */
                ctrl_info.instance = SOC_SFLOW_CTR_TYPE_SAMPLE_COUNT;
            }

            if (stat_op == _BCM_STAT_GET) {
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_get(unit,
                                        SOC_COUNTER_NON_DMA_SFLOW_FLEX_PKT,
                                        ctrl_info, sync_mode, port, &count));
            } else { /* _BCM_STAT_CLEAR */
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_set(unit,
                                        SOC_COUNTER_NON_DMA_SFLOW_FLEX_PKT,
                                        ctrl_info, 0, port, count));
            }
        }
        break;
#endif /* BCM_TOMAHAWK_SUPPORT */

    case snmpBcmTxE2ECCControlFrames:
        if (SOC_REG_IS_VALID(unit, XTHOLr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XTHOLr,
                              &count);
        }
        break;

    case snmpBcmRxDot3LengthMismatches:
        if (SOC_REG_IS_VALID(unit, GRFLRr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              GRFLRr, &count); /* Good FCS, bad length */
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
                                "bcm_stat_get: Statistic not supported: %d\n"), type));
        return BCM_E_PARAM;
    }

    if (stat_op == _BCM_STAT_GET) {
        *val = count;
    }

    return BCM_E_NONE;
}
