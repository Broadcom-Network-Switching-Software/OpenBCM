/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/drv.h>
#include <soc/counter.h>
#include <soc/debug.h>

#include <bcm/stat.h>
#include <bcm/error.h>
#include <bcm_int/esw/stat.h>
#include <bcm_int/esw_dispatch.h>
#include <shared/bsl.h>

/*
 * Function:
 *      _bcm_stat_mxq_get
 * Description:
 *      Get the specified statistic for a port with common counter shared
 *      by both XE and GE MAC.
 * Parameters:
 *      unit - PCI device unit number (driver internal)
 *      port - zero-based port number
 *      sync_mode, reg_op - if 1 read hw counter else sw accumualated counter
 *      type - SNMP statistics type (see stat.h)
 *      val  - (OUT) 64 bit statistic counter value
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_PARAM - Illegal parameter.
 *      BCM_E_INTERNAL - Chip access failure.
 */
int
_bcm_stat_mxq_get_set(int unit, bcm_port_t port, 
                     int sync_mode, int stat_op,
                     bcm_stat_val_t type, uint64 *val)
{
    uint64 count, count1;
    int value = 0;
    int reg_op = (stat_op == _BCM_STAT_GET)? _BCM_STAT_REG_ADD: 
                                             _BCM_STAT_REG_CLEAR;
    int reg_sub_op = (stat_op == _BCM_STAT_GET)? _BCM_STAT_REG_SUB: 
                                             _BCM_STAT_REG_CLEAR;

    COMPILER_REFERENCE(&count);  /* Work around PPC compiler bug */
    COMPILER_64_ZERO(count);


    switch (type) {
        /* *** RFC 1213 *** */
    case snmpIfInOctets:

        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RRBYTr, &count);
        break;
    case snmpIfInUcastPkts:             /* Unicast frames */
        if (SOC_IS_SABER2(unit) && soc_feature(unit, soc_feature_olp)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_control_get(unit, port, bcmPortControlOlpEnable,
                                         &value));
        }
        if (COUNT_OVR_ERRORS(unit) || value) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RUCAr, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RUCr, &count);
        }
        break;
    case snmpIfInNUcastPkts:             /* Non-unicast frames */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RMCAr,
                                    &count); /* + multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RBCAr,
                                    &count); /* + broadcast */

        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RUCAr,
                                    &count); /* + unicast excl */
                                                          /*   oversize */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RMCAr,
                                    &count); /* + multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RBCAr,
                                    &count); /* + broadcast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_ROVRr,
                                    &count); /* + oversize */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, RUCr,
                                    &count); /* - unicast */

       }
        break;
    case snmpIfInBroadcastPkts:          /* Broadcast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RBCAr, &count);
        break;
    case snmpIfInMulticastPkts:          /* Multicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RMCAr, &count);
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
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RFCSr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RJBRr, &count);
        if (SOC_REG_IS_VALID(unit,RMTUEr)) {
           BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RMTUEr, &count);
        } else if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_ROVRr, &count);
        }
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RRPKTr, &count);
        break;
    case snmpIfInUnknownProtos:
        break;
    case snmpIfOutOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TBYTr, &count);
        break;
    case snmpIfOutUcastPkts:             /* Unicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TUCAr, &count);
        if (!COUNT_OVR_ERRORS(unit)) {
            /* Add oversize packets if not considered as error */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TOVRr, &count);
        }
        break;
    case snmpIfOutNUcastPkts:            /* Non-unicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TMCAr,
                                    &count); /* + multicast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TBCAr,
                                    &count); /* + broadcast */
        break;
    case snmpIfOutBroadcastPkts:         /* Broadcast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TBCAr, &count);
        break;
    case snmpIfOutMulticastPkts:         /* Multicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TMCAr, &count);
        break;
    case snmpIfOutDiscards:              /* Aged packet counter */
        if (SOC_REG_IS_VALID(unit, HOLDr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, HOLDr,
                                        &count);  /* L2 MTU drops */
        } else if (SOC_REG_IS_VALID(unit, HOL_DROPr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, HOL_DROPr, &count);
        }
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC3r, &count);
        BCM_IF_ERROR_RETURN(
            bcm_esw_cosq_stat_get(unit, port, BCM_COS_INVALID, bcmCosqStatDroppedPackets, &count1));
        COMPILER_64_ADD_64(count, count1);
        break;
    case snmpIfOutErrors:   /* Error packets could not be xmitted */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TERRr, &count);
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
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RMTUEr, &count);
        break;
    case snmpDot1dTpPortInFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPKTr, &count);
        break;
    case snmpDot1dTpPortOutFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TPKTr, &count);
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
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                 MXQ_RRBYTr, &count); /* Runts bytes */
        break;
    case snmpEtherStatsPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                MXQ_RRPKTr, &count); /* Runts */
        break;
    case snmpEtherStatsBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RBCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TBCAr, &count);
        break;
    case snmpEtherStatsMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RMCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TMCAr, &count);
        break;
    case snmpEtherStatsCRCAlignErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RFCSr, &count);
        break;
    case snmpEtherStatsUndersizePkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RUNDr, &count);
        break;
    case snmpEtherStatsOversizePkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_ROVRr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TOVRr, &count);
        break;
    case snmpEtherRxOversizePkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_ROVRr, &count);
        break;
    case snmpEtherTxOversizePkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TOVRr, &count);
        break;
    case snmpEtherStatsFragments:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RFRGr, &count);
        break;
    case snmpEtherStatsJabbers:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RJBRr, &count);
        break;
    case snmpEtherStatsCollisions:
        break;

        /* *** rfc1757 definition counts receive packet only *** */

    case snmpEtherStatsPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R64r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T64r, &count);
        break;
    case snmpEtherStatsPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R127r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T127r, &count);
        break;
    case snmpEtherStatsPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R255r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T255r, &count);
        break;
    case snmpEtherStatsPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R511r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T511r, &count);
        break;
    case snmpEtherStatsPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R1023r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T1023r, &count);
        break;
    case snmpEtherStatsPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R1518r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T1518r, &count);
        break;
    case snmpBcmEtherStatsPkts1519to1522Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RMGVr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TMGVr, &count);
        break;
    case snmpBcmEtherStatsPkts1522to2047Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R2047r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, MXQ_RMGVr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T2047r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, MXQ_TMGVr, &count);
        break;
    case snmpBcmEtherStatsPkts2048to4095Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R4095r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T4095r, &count);
        break;
    case snmpBcmEtherStatsPkts4095to9216Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R9216r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T9216r, &count);
        break;
    case snmpBcmEtherStatsPkts9217to16383Octets: /* not in rfc1757 */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R16383r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T16383r, &count);
        break;

    case snmpBcmReceivedPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R64r, &count);
        break;
    case snmpBcmReceivedPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R127r, &count);
        break;
    case snmpBcmReceivedPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R255r, &count);
        break;
    case snmpBcmReceivedPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R511r, &count);
        break;
    case snmpBcmReceivedPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R1023r, &count);
        break;
    case snmpBcmReceivedPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R1518r, &count);
        break;
    case snmpBcmReceivedPkts1519to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R2047r, &count);
        break;
    case snmpBcmReceivedPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R4095r, &count);
        break;
    case snmpBcmReceivedPkts4095to9216Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R9216r, &count);
        break;
    case snmpBcmReceivedPkts9217to16383Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_R16383r, &count);
        break;

    case snmpBcmTransmittedPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T64r, &count);
        break;
    case snmpBcmTransmittedPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T127r, &count);
        break;
    case snmpBcmTransmittedPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T255r, &count);
        break;
    case snmpBcmTransmittedPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T511r, &count);
        break;
    case snmpBcmTransmittedPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T1023r, &count);
        break;
    case snmpBcmTransmittedPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T1518r, &count);
        break;
    case snmpBcmTransmittedPkts1519to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T2047r, &count);
        break;
    case snmpBcmTransmittedPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T4095r, &count);
        break;
    case snmpBcmTransmittedPkts4095to9216Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T9216r, &count);
        break;
    case snmpBcmTransmittedPkts9217to16383Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_T16383r, &count);
        break;

    case snmpEtherStatsTXNoErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TPOKr, &count);
        break;
    case snmpEtherStatsRXNoErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPOKr, &count);
        break;

        /* *** RFC 2665 *** */

    case snmpDot3StatsAlignmentErrors:
        break;
    case snmpDot3StatsFCSErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RFCSr, &count);
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
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TUFLr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TERRr, &count);
        break;
    case snmpDot3StatsCarrierSenseErrors:
        break;
    case snmpDot3StatsFrameTooLongs:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RMTUEr, &count);
        break;
    case snmpDot3StatsInternalMacReceiveErrors:
        break;
    case snmpDot3StatsSymbolErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RERPKTr, &count);
        break;
    case snmpDot3ControlInUnknownOpcodes:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RXUOr, &count);
        break;
    case snmpDot3InPauseFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RXPFr, &count);
        break;
    case snmpDot3OutPauseFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TXPFr, &count);
        break;

        /* *** RFC 2233 high capacity versions of RFC1213 objects *** */

    case snmpIfHCInOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RBYTr, &count);
        break;
    case snmpIfHCInUcastPkts:
        if (SOC_IS_SABER2(unit) && soc_feature(unit, soc_feature_olp)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_control_get(unit, port, bcmPortControlOlpEnable,
                                         &value));
        }
        if (COUNT_OVR_ERRORS(unit) || value) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                    MXQ_RUCAr, &count);  /* unicast pkts rcvd */
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                                    RUCr, &count);  /* unicast pkts rcvd */
        }
        break;
    case snmpIfHCInMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RMCAr, &count);
        break;
    case snmpIfHCInBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RBCAr, &count);
        break;
    case snmpIfHCOutOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TBYTr, &count);
        break;
    case snmpIfHCOutUcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TUCAr, &count);
        break;
    case snmpIfHCOutMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TMCAr, &count);
        break;
    case snmpIfHCOutBroadcastPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TBCAr, &count);
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
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TXPPr, &count);
        break;
    case snmpIeee8021PfcIndications:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RXPPr, &count);
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
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RVLNr, &count);
        break;
    case snmpBcmRxDoubleVlanTagFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RDVLNr, &count);
        break;
    case snmpBcmTxVlanTagFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TVLNr, &count);
        break;
    case snmpBcmTxDoubleVlanTagFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TDVLNr, &count);
        break;

    /* PFC Control Frame Counters */
    case snmpBcmRxPFCControlFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RXPPr, &count);
        break;

    case snmpBcmTxPFCControlFrame:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TXPPr, &count);
        break;

    /* Receive PFC Frame Priority 0 XON to XOFF */
    case snmpBcmRxPFCFrameXonPriority0:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFCOFF0r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority1:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFCOFF1r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority2:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFCOFF2r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority3:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFCOFF3r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority4:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFCOFF4r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority5:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFCOFF5r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority6:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFCOFF6r, &count);
        break;

    case snmpBcmRxPFCFrameXonPriority7:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFCOFF7r, &count);
        break;

    /* Receive PFC Frame Priority */
    case snmpBcmRxPFCFramePriority0:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFC0r, &count);
        break;

    case snmpBcmRxPFCFramePriority1:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFC1r, &count);
        break;

    case snmpBcmRxPFCFramePriority2:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFC2r, &count);
        break;

    case snmpBcmRxPFCFramePriority3:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFC3r, &count);
        break;

    case snmpBcmRxPFCFramePriority4:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFC4r, &count);
        break;

    case snmpBcmRxPFCFramePriority5:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFC5r, &count);
        break;

    case snmpBcmRxPFCFramePriority6:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFC6r, &count);
        break;

    case snmpBcmRxPFCFramePriority7:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_RPFC7r, &count);
        break;

    /* Transmit PFC Frame Priority */
    case snmpBcmTxPFCFramePriority0:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TPFC0r, &count);
        break;

    case snmpBcmTxPFCFramePriority1:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TPFC1r, &count);
        break;

    case snmpBcmTxPFCFramePriority2:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TPFC2r, &count);
        break;

    case snmpBcmTxPFCFramePriority3:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TPFC3r, &count);
        break;

    case snmpBcmTxPFCFramePriority4:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TPFC4r, &count);
        break;

    case snmpBcmTxPFCFramePriority5:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TPFC5r, &count);
        break;

    case snmpBcmTxPFCFramePriority6:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TPFC6r, &count);
        break;

    case snmpBcmTxPFCFramePriority7:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, MXQ_TPFC7r, &count);
        break;

    case snmpFcmPortClass3RxFrames:
        if (soc_feature(unit, soc_feature_fcoe)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC3r, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpFcmPortClass3TxFrames:
        if (soc_feature(unit, soc_feature_fcoe)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC6r, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpFcmPortClass3Discards:
        if (soc_feature(unit, soc_feature_fcoe)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC4r, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpFcmPortClass2RxFrames:
        if (soc_feature(unit, soc_feature_fcoe)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC5r, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpFcmPortClass2TxFrames:
        if (soc_feature(unit, soc_feature_fcoe)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC7r, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpFcmPortClass2Discards:
        if (soc_feature(unit, soc_feature_fcoe)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RDBGC6r, &count);
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

    case snmpBcmRxDot3LengthMismatches:
        if (SOC_REG_IS_VALID(unit, MXQ_RFLRr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              MXQ_RFLRr, &count); /* Good FCS, bad length */
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    default:
        if (type < snmpValCount) {
            return BCM_E_UNAVAIL;
        }
        LOG_VERBOSE(BSL_LS_BCM_STAT,  
                    (BSL_META_U(unit, 
                   "_bcm_stat_mxq_get: Statistic not supported: %d\n"), type));
        return BCM_E_PARAM;
    }

    if (stat_op == _BCM_STAT_GET) {
        *val = count;
    }
    return BCM_E_NONE;
}
