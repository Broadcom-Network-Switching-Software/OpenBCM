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
#include <bcm_int/esw/subport.h>

/*
 * Function:
 *      _bcm_stat_generic_get
 * Description:
 *      Get the specified statistic for a port with common counter shared
 *      by both XE and GE MAC.
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
_bcm_stat_generic_get_set(int unit, bcm_port_t port, 
                          int sync_mode, int stat_op,
                          bcm_stat_val_t type, uint64 *val)
{
    uint64 count, count1;
    int reg_valid;
    int reg_op = (stat_op == _BCM_STAT_GET)? _BCM_STAT_REG_ADD: 
                                             _BCM_STAT_REG_CLEAR;
    int reg_sub_op = (stat_op == _BCM_STAT_GET)? _BCM_STAT_REG_SUB: 
                                             _BCM_STAT_REG_CLEAR;

#ifdef BCM_TOMAHAWK_SUPPORT
    soc_ctr_control_info_t ctrl_info;
    ctrl_info.instance = -1;
#endif /* BCM_TOMAHAWK_SUPPORT */

    COMPILER_REFERENCE(&count);  /* Work around PPC compiler bug */
    COMPILER_64_ZERO(count);

    switch (type) {
        /* *** RFC 1213 *** */
    case snmpIfInOctets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RBYT, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RRBYT, &count);
        break;
    case snmpIfInUcastPkts:             /* Unicast frames */
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                                  RUCA, &count); /*unicast pkts rcvd */
            } else {
                if (SOC_REG_IS_VALID(unit, RUC_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                      RUC_64r, &count); /* unicast pkts rcvd */
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                      RUCr, &count); /* unicast pkts rcvd */
                }
            }
        } else {
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPKT, &count);

            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RMCA, &count); /* - multicast */
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RBCA, &count); /* - broadcast */
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RFCS, &count); /* - bad FCS */
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RFLR, &count); /* - good FCS, bad length */
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RXCF, &count); /* - good FCS, all MAC ctrl */
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RJBR, &count); /* - oversize, bad FCS */
            if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  ROVR, &count); /* - oversize, good FCS */
            }
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RMTUE, &count); /* - MTU errors */
        }
        break;
    case snmpIfInNUcastPkts:             /* Non-unicast frames */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                              RMCA, &count); /* + multicast */
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                              RBCA, &count); /* + broadcast */
        } else {
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                              RUCA, &count); /* + unicast excl */
                                              /*   oversize */
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                              RMCA, &count); /* + multicast */
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                              RBCA, &count); /* + broadcast */
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                              ROVR, &count); /* + oversize */
            if (SOC_REG_IS_VALID(unit, RUC_64r)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                                  RUC_64r, &count); /* - unicast */
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                                  RUCr, &count); /* - unicast */
            }
        }
        break;
    case snmpIfInBroadcastPkts:          /* Broadcast frames */
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RBCA, &count);
        break;
    case snmpIfInMulticastPkts:          /* Multicast frames */
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RMCA, &count);
        break;
    case snmpIfInDiscards:           /* Dropped packets including aborted */
        if (SOC_REG_IS_VALID(unit, RDBGC0_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC0_64r, &count);
        } else {
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
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RFCS, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RJBR, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RRPKT, &count);

        BCM_STAT_GENERIC_REG_VALIDATE(unit, port, RMTUE, reg_valid);
        if (reg_valid) {
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RMTUE, &count);
        } else if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, ROVR, &count);
        }
        break;
    case snmpIfInUnknownProtos:
        break;
    case snmpIfOutOctets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TBYT, &count);
        break;
    case snmpIfOutUcastPkts:             /* Unicast frames */
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TUCA, &count);
        if (!COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                              TOVR, &count); /* + oversized pkts */
        }
        break;
    case snmpIfOutNUcastPkts:            /* Non-unicast frames */
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                          TMCA, &count); /* + multicast */
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                          TBCA, &count); /* + broadcast */
        break;
    case snmpIfOutBroadcastPkts:         /* Broadcast frames */
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TBCA, &count);
        break;
    case snmpIfOutMulticastPkts:         /* Multicast frames */
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TMCA, &count);
        break;
    case snmpIfOutDiscards:              /* Aged packet counter */
        if (SOC_REG_IS_VALID(unit, HOLDr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              HOLDr, &count);  /* L2 MTU drops */
        } else if (SOC_REG_IS_VALID(unit, HOL_DROPr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, HOL_DROPr, &count);
        }
        if (SOC_REG_IS_VALID(unit, TDBGC3_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC3_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC3r, &count);
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_cosq_stat_get(unit, port, BCM_COS_INVALID, 
                                  bcmCosqStatDroppedPackets, &count1));
        COMPILER_64_ADD_64(count, count1);
        break;
    case snmpIfOutErrors:   /* Error packets could not be xmitted */
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TERR, &count);
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
        if (SOC_REG_IS_VALID(unit, RIPC4_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPC4_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPC4r, &count);
        }
        break;
    case snmpIpInHdrErrors:
        if (SOC_REG_IS_VALID(unit, RIPD4_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4r, &count);
        }
        break;
    case snmpIpForwDatagrams:
        if (SOC_REG_IS_VALID(unit, TDBGC4_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC4_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC4r, &count);
        }
        break;
    case snmpIpInDiscards:
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
        break;

        /* *** RFC 1493 *** */

    case snmpDot1dBasePortDelayExceededDiscards:
        break;
    case snmpDot1dBasePortMtuExceededDiscards:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RMTUE, &count);
        break;
    case snmpDot1dTpPortInFrames:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPKT, &count);
        break;
    case snmpDot1dTpPortOutFrames:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TPKT, &count);
        break;
    case snmpDot1dPortInDiscards:
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
        break;

        /* *** RFC 1757 *** */

    case snmpEtherStatsDropEvents:
        if (SOC_REG_IS_VALID(unit, RDISC_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDISC_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDISCr, &count);
        }
        break;
    case snmpEtherStatsOctets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RBYT, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TBYT, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                          RRBYT, &count); /* Runts bytes */
        break;
    case snmpEtherStatsPkts:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPKT, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TPKT, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                          RRPKT, &count); /* Runts */
        break;
    case snmpEtherStatsBroadcastPkts:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RBCA, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TBCA, &count);
        break;
    case snmpEtherStatsMulticastPkts:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RMCA, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TMCA, &count);
        break;
    case snmpEtherStatsCRCAlignErrors:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RFCS, &count);
        break;
    case snmpEtherStatsUndersizePkts:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RUND, &count);
        break;
    case snmpEtherStatsOversizePkts:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, ROVR, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TOVR, &count);
        break;
    case snmpEtherRxOversizePkts:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, ROVR, &count);
        break;
    case snmpEtherTxOversizePkts:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TOVR, &count);
        break;
    case snmpEtherStatsFragments:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RFRG, &count);
        break;
    case snmpEtherStatsJabbers:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RJBR, &count);
        break;
    case snmpEtherStatsCollisions:
        break;

        /* *** rfc1757 definition counts receive packet only *** */

    case snmpEtherStatsPkts64Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R64, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T64, &count);
        break;
    case snmpEtherStatsPkts65to127Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R127, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T127, &count);
        break;
    case snmpEtherStatsPkts128to255Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R255, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T255, &count);
        break;
    case snmpEtherStatsPkts256to511Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R511, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T511, &count);
        break;
    case snmpEtherStatsPkts512to1023Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R1023, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T1023, &count);
        break;
    case snmpEtherStatsPkts1024to1518Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R1518, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T1518, &count);
        break;
    case snmpBcmEtherStatsPkts1519to1522Octets: /* not in rfc1757 */
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RMGV, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TMGV, &count);
        break;
    case snmpBcmEtherStatsPkts1522to2047Octets: /* not in rfc1757 */
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R2047, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op, RMGV, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T2047, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op, TMGV, &count);
        break;
    case snmpBcmEtherStatsPkts2048to4095Octets: /* not in rfc1757 */
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R4095, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T4095, &count);
        break;
    case snmpBcmEtherStatsPkts4095to9216Octets: /* not in rfc1757 */
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R9216, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T9216, &count);
        break;
    case snmpBcmEtherStatsPkts9217to16383Octets: /* not in rfc1757 */
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R16383, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T16383, &count);
        break;

    case snmpBcmReceivedPkts64Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R64, &count);
        break;
    case snmpBcmReceivedPkts65to127Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R127, &count);
        break;
    case snmpBcmReceivedPkts128to255Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R255, &count);
        break;
    case snmpBcmReceivedPkts256to511Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R511, &count);
        break;
    case snmpBcmReceivedPkts512to1023Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R1023, &count);
        break;
    case snmpBcmReceivedPkts1024to1518Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R1518, &count);
        break;
    case snmpBcmReceivedPkts1519to2047Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R2047, &count);
        break;
    case snmpBcmReceivedPkts2048to4095Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R4095, &count);
        break;
    case snmpBcmReceivedPkts4095to9216Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R9216, &count);
        break;
    case snmpBcmReceivedPkts9217to16383Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, R16383, &count);
        break;

    case snmpBcmTransmittedPkts64Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T64, &count);
        break;
    case snmpBcmTransmittedPkts65to127Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T127, &count);
        break;
    case snmpBcmTransmittedPkts128to255Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T255, &count);
        break;
    case snmpBcmTransmittedPkts256to511Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T511, &count);
        break;
    case snmpBcmTransmittedPkts512to1023Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T1023, &count);
        break;
    case snmpBcmTransmittedPkts1024to1518Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T1518, &count);
        break;
    case snmpBcmTransmittedPkts1519to2047Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T2047, &count);
        break;
    case snmpBcmTransmittedPkts2048to4095Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T4095, &count);
        break;
    case snmpBcmTransmittedPkts4095to9216Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T9216, &count);
        break;
    case snmpBcmTransmittedPkts9217to16383Octets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, T16383, &count);
        break;

    case snmpEtherStatsTXNoErrors:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TPOK, &count);
        break;
    case snmpEtherStatsRXNoErrors:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPOK, &count);
        break;

        /* *** RFC 2665 *** */

    case snmpDot3StatsAlignmentErrors:
        /* SDK-133312:
           lack of CDMIB_RALNf, can't use BCM_STAT_GENERIC_REG_OPER */
        if (soc_feature(unit, soc_feature_cxl_mib)) {
            int phy_port;
            soc_reg_t ctr_reg_adj = RALNr;

            if (soc_feature(unit, soc_feature_logical_port_num)) {
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            } else {
                phy_port = port;
            }
            if (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port),
                                 SOC_BLK_CLPORT)) {
                if (SOC_REG_IS_VALID(unit, CLMIB_RALNr)) {
                    ctr_reg_adj = CLMIB_RALNr;
                }
            } else if (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port),
                                        SOC_BLK_XLPORT)) {
                if (SOC_REG_IS_VALID(unit, XLMIB_RALNr)) {
                    ctr_reg_adj = XLMIB_RALNr;
                }
            }
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ctr_reg_adj, val);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RALNr, val);
        }
        break;
    case snmpDot3StatsFCSErrors:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RFCS, &count);
        break;
    case snmpDot3StatsSingleCollisionFrames:
        break;
    case snmpDot3StatsMultipleCollisionFrames:
        break;
    case snmpDot3StatsSQETTestErrors:
        break;
    case snmpDot3StatsDeferredTransmissions:
        break;
    case snmpDot3StatsLateCollisions:
        break;
    case snmpDot3StatsExcessiveCollisions:
        break;
    case snmpDot3StatsInternalMacTransmitErrors:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TUFL, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TERR, &count);
        break;
    case snmpDot3StatsCarrierSenseErrors:
        /* SDK-133312:
           lack of CDMIB_RFCRf, can't use BCM_STAT_GENERIC_REG_OPER */
        if (soc_feature(unit, soc_feature_cxl_mib)) {
            int phy_port;
            soc_reg_t ctr_reg_adj = RFCRr;

            if (soc_feature(unit, soc_feature_logical_port_num)) {
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            } else {
                phy_port = port;
            }
            if (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port),
                                 SOC_BLK_CLPORT)) {
                if (SOC_REG_IS_VALID(unit, CLMIB_RFCRr)) {
                    ctr_reg_adj = CLMIB_RFCRr;
                }
            } else if (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port),
                                        SOC_BLK_XLPORT)) {
                if (SOC_REG_IS_VALID(unit, XLMIB_RFCRr)) {
                    ctr_reg_adj = XLMIB_RFCRr;
                }
            }
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ctr_reg_adj, val);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RFCRr, val);
        }
        break;
    case snmpDot3StatsFrameTooLongs:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RMTUE, &count);
        break;
    case snmpDot3StatsInternalMacReceiveErrors:
        break;
    case snmpDot3StatsSymbolErrors:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RERPKT, &count);
        break;
    case snmpDot3ControlInUnknownOpcodes:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RXUO, &count);
        break;
    case snmpDot3InPauseFrames:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RXPF, &count);
        break;
    case snmpDot3OutPauseFrames:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TXPF, &count);
        break;

        /* *** RFC 2233 high capacity versions of RFC1213 objects *** */

    case snmpIfHCInOctets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RBYT, &count);
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RRBYT, &count);
        break;
    case snmpIfHCInUcastPkts:
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                                  RUCA, &count);  /* unicast pkts rcvd */
            } else {
                if (SOC_REG_IS_VALID(unit, RUC_64r)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                      RUC_64r, &count);  /* unicast pkts rcvd */
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                      RUCr, &count);  /* unicast pkts rcvd */
                }
            }
        } else {
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPKT, &count);
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RMCA, &count);
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RBCA, &count);
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RFCS, &count); /* - bad FCS */
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RFLR, &count); /* - good FCS, bad length */
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RXCF, &count); /* - good FCS, all MAC ctrl */
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RXPF, &count); /* - not included in RXCFr */
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RJBR, &count); /* - oversize, bad FCS */
            if (COUNT_OVR_ERRORS(unit)) {
                BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  ROVR, &count); /* - oversize, good FCS */
            }
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              RMTUE, &count); /* - MTU errors */
        }
        break;
    case snmpIfHCInMulticastPkts:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RMCA, &count);
        break;
    case snmpIfHCInBroadcastPkts:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RBCA, &count);
        break;
    case snmpIfHCOutOctets:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TBYT, &count);
        break;
    case snmpIfHCOutUcastPkts:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TUCA, &count);
        if (!COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op,
                              TOVR, &count); /* + oversized pkts */
        }
        break;
    case snmpIfHCOutMulticastPkts:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TMCA, &count);
        break;
    case snmpIfHCOutBroadcastPckts:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TBCA, &count);
        break;

        /* *** RFC 2465 *** */

    case snmpIpv6IfStatsInReceives:
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
        break;
    case snmpIpv6IfStatsInHdrErrors:
        if (SOC_REG_IS_VALID(unit, RIPD6_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
        }
        break;
    case snmpIpv6IfStatsInAddrErrors:
        if (SOC_REG_IS_VALID(unit, RIPHE6_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE6_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE6r, &count);
        }
        break;
    case snmpIpv6IfStatsInDiscards:
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
        break;
    case snmpIpv6IfStatsOutForwDatagrams:
        if (SOC_REG_IS_VALID(unit, TDBGC0_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC0_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC0r, &count);
        }
        break;
    case snmpIpv6IfStatsOutDiscards:
        if (SOC_REG_IS_VALID(unit, TDBGC1_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC1_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC1r, &count);
        }
        break;
    case snmpIpv6IfStatsInMcastPkts:
        if (SOC_REG_IS_VALID(unit, IMRP6_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6r, &count);
        }
        break;
    case snmpIpv6IfStatsOutMcastPkts:
        if (SOC_REG_IS_VALID(unit, TDBGC2_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC2_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC2r, &count);
        }
        break;

        /* *** IEEE 802.1bb *** */
    case snmpIeee8021PfcRequests:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TXPP, &count);
        break;
    case snmpIeee8021PfcIndications:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RXPP, &count);
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
        if (SOC_REG_IS_VALID(unit, RDBGC1_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC1_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC1r, &count);
        }
        break;
    case snmpBcmIPMCRoutedPckts:
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
        break;
    case snmpBcmIPMCInDroppedPckts:
        if (SOC_REG_IS_VALID(unit, RDBGC2_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC2_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC2r, &count);
        }
        break;
    case snmpBcmIPMCOutDroppedPckts:
        if (SOC_REG_IS_VALID(unit, TDBGC5_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC5_64r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC5r, &count);
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

    /* Vlan Tag Frame Counters */
    case snmpBcmRxVlanTagFrame:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RVLN, &count);
        break;
    case snmpBcmRxDoubleVlanTagFrame:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RDVLN, &count);
        break;
    case snmpBcmTxVlanTagFrame:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TVLN, &count);
        break;
    case snmpBcmTxDoubleVlanTagFrame:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TDVLN, &count);
        break;

    /* PFC Control Frame Counters */
    case snmpBcmRxPFCControlFrame:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RXPP, &count);
        break;

    case snmpBcmTxPFCControlFrame:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TXPP, &count);
        break;

    /* Receive PFC Frame Priority 0 XON to XOFF */
    case snmpBcmRxPFCFrameXonPriority0:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF0, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority1:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF1, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority2:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF2, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority3:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF3, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority4:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF4, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority5:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF5, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority6:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF6, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority7:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFCOFF7, &count);
        break;

    /* Receive PFC Frame Priority */
    case snmpBcmRxPFCFramePriority0:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFC0, &count);
        break;

    case snmpBcmRxPFCFramePriority1:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFC1, &count);
        break;

    case snmpBcmRxPFCFramePriority2:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFC2, &count);
        break;

    case snmpBcmRxPFCFramePriority3:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFC3, &count);
        break;

    case snmpBcmRxPFCFramePriority4:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFC4, &count);
        break;

    case snmpBcmRxPFCFramePriority5:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFC5, &count);
        break;

    case snmpBcmRxPFCFramePriority6:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFC6, &count);
        break;

    case snmpBcmRxPFCFramePriority7:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, RPFC7, &count);
        break;

    /* Transmit PFC Frame Priority */
    case snmpBcmTxPFCFramePriority0:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TPFC0, &count);
        break;

    case snmpBcmTxPFCFramePriority1:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TPFC1, &count);
        break;

    case snmpBcmTxPFCFramePriority2:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TPFC2, &count);
        break;

    case snmpBcmTxPFCFramePriority3:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TPFC3, &count);
        break;

    case snmpBcmTxPFCFramePriority4:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TPFC4, &count);
        break;

    case snmpBcmTxPFCFramePriority5:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TPFC5, &count);
        break;

    case snmpBcmTxPFCFramePriority6:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TPFC6, &count);
        break;

    case snmpBcmTxPFCFramePriority7:
        BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, TPFC7, &count);
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
                              EGR_FCOE_DELIMITER_ERROR_FRAMESr, &count);
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

    case snmpBcmTxEcnErrors:
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        if ((SOC_MEM_IS_VALID(unit, CEGR_ECN_COUNTERm)) &&
            (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))) {
            uint32 lval = 0; /* local value. */
            int index = 0;
            int rv = 0;
            soc_mem_t mem = CEGR_ECN_COUNTERm;
            cegr_ecn_counter_entry_t c_ech_entry;

            BCM_IF_ERROR_RETURN(
               _bcm_coe_subtag_subport_port_subport_num_get(unit, port, NULL, &index));
            /* Lock the memory. */
            soc_mem_lock(unit, mem);

            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &c_ech_entry);
            if (SOC_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return (rv);
            }

            if (stat_op == _BCM_STAT_GET) {

                soc_mem_field_get(unit, mem, (uint32 *)&c_ech_entry, COUNTf, &lval);
                COMPILER_64_SET(count, 0, lval);
            } else {

                soc_mem_field_set(unit, mem, (uint32 *)&c_ech_entry, COUNTf, &lval);
                rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &c_ech_entry);
                if (SOC_FAILURE(rv)) {
                    soc_mem_unlock(unit, mem);
                    return (rv);
                }
            }
            soc_mem_unlock(unit, mem);

        } else
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */
        if (SOC_REG_IS_VALID(unit, EGR_ECN_COUNTER_64r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, EGR_ECN_COUNTER_64r, &count);
        } else if (SOC_REG_IS_VALID(unit, EGR_ECN_COUNTERr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, EGR_ECN_COUNTERr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmRxDot3LengthMismatches:
        if (SOC_REG_IS_VALID(unit, RFLRr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              RFLRr, &count); /* Good FCS, bad length */
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
                                "_bcm_stat_generic_get: Statistic not supported: %d\n"), type));
        return BCM_E_PARAM;
    }

    if (stat_op == _BCM_STAT_GET) {
        *val = count;
    }

    return BCM_E_NONE;
}
