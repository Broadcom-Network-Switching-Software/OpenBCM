/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/counter.h>
#include <soc/debug.h>

#include <soc/dpp/mbcm.h>

#include <bcm/stat.h>
#include <bcm/error.h>
#include <bcm_int/dpp/stat.h>
#include <bcm_int/petra_dispatch.h>

/*
 * Function:
 *      _bcm_stat_generic_get
 * Description:
 *      Get the specified statistic for a port with common counter shared
 *      by both XE and GE MAC.
 * Parameters:
 *      unit - PCI device unit number (driver internal)
 *      port - zero-based port number
 *      type - SNMP statistics type (see stat.h)
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_PARAM - Illegal parameter.
 *      BCM_E_INTERNAL - Chip access failure.
 */
int
_bcm_petra_stat_ge_get(int unit, bcm_port_t port, bcm_stat_val_t type,
                      uint64 *val)
{
    uint64 count;
    REG_MATH_DECL;       /* Required for use of the REG_* macros */

    COMPILER_REFERENCE(&count);  /* PPC compiler */
    COMPILER_64_ZERO(count);

    switch (type) {
        /* *** RFC 1213 *** */
    case snmpIfInOctets:
        REG_ADD(unit, port, GRBYTr, count);
        REG_ADD(unit, port, GRRBYTr, count);
        break;
    case snmpIfInUcastPkts:             /* Unicast frames */
        REG_ADD(unit, port, GRUCr, count);  /* unicast pkts rcvd */
        break;
    case snmpIfInNUcastPkts:             /* Non-unicast frames */
        REG_ADD(unit, port, GRMCAr, count); /* + multicast */
        REG_ADD(unit, port, GRBCAr, count); /* + broadcast */
        break;
    case snmpIfInBroadcastPkts:          /* Broadcast frames */
        REG_ADD(unit, port, GRBCAr, count);
        break;
    case snmpIfInMulticastPkts:          /* Multicast frames */
        REG_ADD(unit, port, GRMCAr, count);
        break;
    case snmpIfInDiscards:           /* Dropped packets including aborted */
    
    return BCM_E_UNAVAIL;
        
        break;
    case snmpIfInErrors: /* RX Errors or Receive packets - non-error frames */
        REG_ADD(unit, port, GRUNDr, count); /* undersize pkts, good FCS */
        REG_ADD(unit, port, GRFRGr, count); /* undersize pkts, bad FCS */
        REG_ADD(unit, port, GRFCSr, count); /* FCS errors */
        REG_ADD(unit, port, GRJBRr, count); /* oversize pkts, bad FCS */
        REG_ADD(unit, port, GRMTUEr, count);
        break;
    case snmpIfInUnknownProtos:
        break;
    case snmpIfOutOctets:
        REG_ADD(unit, port, GTBYTr, count);
        break;
    case snmpIfOutUcastPkts:             /* Unicast frames */
        REG_ADD(unit, port, GTUCr, count);
        break;
    case snmpIfOutNUcastPkts:            /* Non-unicast frames */
        REG_ADD(unit, port, GTMCAr, count); /* + multicast */
        REG_ADD(unit, port, GTBCAr, count); /* + broadcast */
        break;
    case snmpIfOutBroadcastPkts:         /* Broadcast frames */
        REG_ADD(unit, port, GTBCAr, count);
        break;
    case snmpIfOutMulticastPkts:         /* Multicast frames */
        REG_ADD(unit, port, GTMCAr, count);
        break;
    case snmpIfOutDiscards:              /* Aged packet counter */
        
        return BCM_E_UNAVAIL;
        break;
    case snmpIfOutErrors:
        REG_ADD(unit, port, GTXCLr, count);
        break;
    case snmpIpInReceives:
    case snmpIpInHdrErrors:
    case snmpIpForwDatagrams:
    case snmpIpInDiscards:
        /*IP counters are not supported*/
        return BCM_E_UNAVAIL;
        break;

        /* *** RFC 1493 *** */

    case snmpDot1dBasePortDelayExceededDiscards:
        REG_ADD(unit, port, GTEDFr, count);
        break;
    case snmpDot1dBasePortMtuExceededDiscards:
        REG_ADD(unit, port, GRMTUEr, count);
        break;
    case snmpDot1dTpPortInFrames:
        REG_ADD(unit, port, GRPKTr, count);
        break;
    case snmpDot1dTpPortOutFrames:
        REG_ADD(unit, port, GTPKTr, count);
        break;
    case snmpDot1dPortInDiscards:
        
        return BCM_E_UNAVAIL;
        break;

        /* *** RFC 1757 *** */

    case snmpEtherStatsDropEvents:
        
        return BCM_E_UNAVAIL;

        break;
    case snmpEtherStatsOctets:
        REG_ADD(unit, port, GRBYTr, count);
        REG_ADD(unit, port, GTBYTr, count);
        REG_ADD(unit, port, GRRBYTr, count);
        break;
    case snmpEtherStatsPkts:
        REG_ADD(unit, port, GRPKTr, count);
        REG_ADD(unit, port, GRFLRr, count);
        REG_ADD(unit, port, GTPKTr, count);
        REG_ADD(unit, port, GRRPKTr, count); /* Runts */
        break;
    case snmpEtherStatsBroadcastPkts:
        REG_ADD(unit, port, GRBCAr, count);
        REG_ADD(unit, port, GTBCAr, count);
        break;
    case snmpEtherStatsMulticastPkts:
        REG_ADD(unit, port, GRMCAr, count);
        REG_ADD(unit, port, GTMCAr, count);
        break;
    case snmpEtherStatsCRCAlignErrors:
        REG_ADD(unit, port, GRFCSr, count);
        break;
    case snmpEtherStatsUndersizePkts:
        REG_ADD(unit, port, GRUNDr, count);
        break;
    case snmpEtherStatsOversizePkts:
        REG_ADD(unit, port, GROVRr, count);
        REG_ADD(unit, port, GTOVRr, count);
        break;
    case snmpEtherRxOversizePkts:
        REG_ADD(unit, port, GROVRr, count);
        break;
    case snmpEtherTxOversizePkts:
        REG_ADD(unit, port, GTOVRr, count);
        break;
    case snmpEtherStatsFragments:
        REG_ADD(unit, port, GRFRGr, count);
        break;
    case snmpEtherStatsJabbers:
        REG_ADD(unit, port, GRJBRr, count);
        break;
    case snmpEtherStatsCollisions:
        REG_ADD(unit, port, GTNCLr, count);
        break;

        /* *** rfc1757 definition counts receive packet only *** */

    case snmpEtherStatsPkts64Octets:
        REG_ADD(unit, port, GR64r, count);
        REG_ADD(unit, port, GT64r, count);
        break;
    case snmpEtherStatsPkts65to127Octets:
        REG_ADD(unit, port, GR127r, count);
        REG_ADD(unit, port, GT127r, count);
        break;
    case snmpEtherStatsPkts128to255Octets:
        REG_ADD(unit, port, GR255r, count);
        REG_ADD(unit, port, GT255r, count);
        break;
    case snmpEtherStatsPkts256to511Octets:
        REG_ADD(unit, port, GR511r, count);
        REG_ADD(unit, port, GT511r, count);
        break;
    case snmpEtherStatsPkts512to1023Octets:
        REG_ADD(unit, port, GR1023r, count);
        REG_ADD(unit, port, GT1023r, count);
        break;
    case snmpEtherStatsPkts1024to1518Octets:
        REG_ADD(unit, port, GR1518r, count);
        REG_ADD(unit, port, GT1518r, count);
        break;
    case snmpBcmEtherStatsPkts1519to1522Octets: /* not in rfc1757 */
        REG_ADD(unit, port, GRMGVr, count);
        REG_ADD(unit, port, GTMGVr, count);
        break;
    case snmpBcmEtherStatsPkts1522to2047Octets: /* not in rfc1757 */
        REG_ADD(unit, port, GR2047r, count);
        REG_SUB(unit, port, GRMGVr, count);
        REG_ADD(unit, port, GT2047r, count);
        REG_SUB(unit, port, GTMGVr, count);
        break;
    case snmpBcmEtherStatsPkts2048to4095Octets: /* not in rfc1757 */
        REG_ADD(unit, port, GR4095r, count);
        REG_ADD(unit, port, GT4095r, count);
        break;
    case snmpBcmEtherStatsPkts4095to9216Octets: /* not in rfc1757 */
        REG_ADD(unit, port, GR9216r, count);
        REG_ADD(unit, port, GT9216r, count);
        break;

    case snmpBcmEtherStatsPkts9217to16383Octets: /* not in rfc1757 */
        break;
    case snmpBcmReceivedPkts64Octets:
        REG_ADD(unit, port, GR64r, count);
        break;
    case snmpBcmReceivedPkts65to127Octets:
        REG_ADD(unit, port, GR127r, count);
        break;
    case snmpBcmReceivedPkts128to255Octets:
        REG_ADD(unit, port, GR255r, count);
        break;
    case snmpBcmReceivedPkts256to511Octets:
        REG_ADD(unit, port, GR511r, count);
        break;
    case snmpBcmReceivedPkts512to1023Octets:
        REG_ADD(unit, port, GR1023r, count);
        break;
    case snmpBcmReceivedPkts1024to1518Octets:
        REG_ADD(unit, port, GR1518r, count);
        break;
    case snmpBcmReceivedPkts1519to2047Octets:
        REG_ADD(unit, port, GR2047r, count);
        break;
    case snmpBcmReceivedPkts2048to4095Octets:
        REG_ADD(unit, port, GR4095r, count);
        break;
    case snmpBcmReceivedPkts4095to9216Octets:
        REG_ADD(unit, port, GR9216r, count);
        break;
    case snmpBcmReceivedPkts9217to16383Octets:
        break;
    case snmpBcmTransmittedPkts64Octets:
        REG_ADD(unit, port, GT64r, count);
        break;
    case snmpBcmTransmittedPkts65to127Octets:
        REG_ADD(unit, port, GT127r, count);
        break;
    case snmpBcmTransmittedPkts128to255Octets:
        REG_ADD(unit, port, GT255r, count);
        break;
    case snmpBcmTransmittedPkts256to511Octets:
        REG_ADD(unit, port, GT511r, count);
        break;
    case snmpBcmTransmittedPkts512to1023Octets:
        REG_ADD(unit, port, GT1023r, count);
        break;
    case snmpBcmTransmittedPkts1024to1518Octets:
        REG_ADD(unit, port, GT1518r, count);
        break;
    case snmpBcmTransmittedPkts1519to2047Octets:
        REG_ADD(unit, port, GT2047r, count);
        break;
    case snmpBcmTransmittedPkts2048to4095Octets:
        REG_ADD(unit, port, GT4095r, count);
        break;
    case snmpBcmTransmittedPkts4095to9216Octets:
        REG_ADD(unit, port, GT9216r, count);
        break;
    case snmpBcmTransmittedPkts9217to16383Octets:
        break;
    case snmpEtherStatsTXNoErrors:
        REG_ADD(unit, port, GTPOKr, count);
        break;
    case snmpEtherStatsRXNoErrors:
        REG_ADD(unit, port, GRPOKr, count);
        REG_ADD(unit, port, GRFLRr, count);
        break;

        /* *** RFC 2665 *** */

    case snmpDot3StatsAlignmentErrors:
        REG_ADD(unit, port, GRALNr, count);
        break;
    case snmpDot3StatsFCSErrors:
        REG_ADD(unit, port, GRFCSr, count);
        break;
    case snmpDot3StatsSingleCollisionFrames:
        
        break;
    case snmpDot3StatsMultipleCollisionFrames:
        REG_ADD(unit, port, GTMCLr, count);
        break;
    case snmpDot3StatsSQETTestErrors:
        break;
    case snmpDot3StatsDeferredTransmissions:
        REG_ADD(unit, port, GTDFRr, count);
        break;
    case snmpDot3StatsLateCollisions:
        REG_ADD(unit, port, GTLCLr, count);
        break;
    case snmpDot3StatsExcessiveCollisions:
        REG_ADD(unit, port, GTXCFr, count);
        break;
    case snmpDot3StatsInternalMacTransmitErrors:
        /* always 0 */
        break;
    case snmpDot3StatsCarrierSenseErrors:
        REG_ADD(unit, port, GRFCRr, count);
        break;
    case snmpDot3StatsFrameTooLongs:
        REG_ADD(unit, port, GRMTUEr, count);
        break;
    case snmpDot3StatsInternalMacReceiveErrors:
        /* always 0 */
        break;
    case snmpDot3StatsSymbolErrors:
        REG_ADD(unit, port, GRCDEr, count);
        break;
    case snmpDot3ControlInUnknownOpcodes:
        REG_ADD(unit, port, GRXUOr, count);
        break;
    case snmpDot3InPauseFrames:
        REG_ADD(unit, port, GRXPFr, count);
        break;
    case snmpDot3OutPauseFrames:
        REG_ADD(unit, port, GTXPFr, count);
        break;

        /* *** RFC 2233 high capacity versions of RFC1213 objects *** */

    case snmpIfHCInOctets:
        REG_ADD(unit, port, GRBYTr, count);
        REG_ADD(unit, port, GRRBYTr, count);
        break;
    case snmpIfHCInUcastPkts:
        REG_ADD(unit, port, GRUCr, count);  /* unicast pkts rcvd */
        break;
    case snmpIfHCInMulticastPkts:
        REG_ADD(unit, port, GRMCAr, count);
        break;
    case snmpIfHCInBroadcastPkts:
        REG_ADD(unit, port, GRBCAr, count);
        break;
    case snmpIfHCOutOctets:
        REG_ADD(unit, port, GTBYTr, count);
        break;
    case snmpIfHCOutUcastPkts:
        REG_ADD(unit, port, GTUCr, count);
        break;
    case snmpIfHCOutMulticastPkts:
        REG_ADD(unit, port, GTMCAr, count);
        break;
    case snmpIfHCOutBroadcastPckts:
        REG_ADD(unit, port, GTBCAr, count);
        break;

        /* *** RFC 2465 *** */

    case snmpIpv6IfStatsInReceives:
    case snmpIpv6IfStatsInHdrErrors:
    case snmpIpv6IfStatsInAddrErrors:
    case snmpIpv6IfStatsInDiscards:
    case snmpIpv6IfStatsOutForwDatagrams:
    case snmpIpv6IfStatsOutDiscards:
    case snmpIpv6IfStatsInMcastPkts:
    case snmpIpv6IfStatsOutMcastPkts:
        /*ip counters - unsupported*/
        return BCM_E_UNAVAIL;
        break;

        /* *** IEEE 802.1bb *** */
    case snmpIeee8021PfcRequests:
        REG_ADD(unit, port, GTXPPr, count);
        break;
    case snmpIeee8021PfcIndications:
        REG_ADD(unit, port, GRXPPr, count);
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
    case snmpBcmIPMCRoutedPckts:
    case snmpBcmIPMCInDroppedPckts:
    case snmpBcmIPMCOutDroppedPckts:
        /*ip counters - unsupported*/
        return BCM_E_UNAVAIL;
        break;

        /* EA (broadcom specific) - unsupported in XGS */
	case snmpBcmReceivedUndersizePkts:
		REG_ADD(unit, port, GRRPKTr, count);
		break;
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

    default:
        if (type < snmpValCount) {
            return BCM_E_UNAVAIL;
        }
        LOG_WARN(BSL_LS_BCM_STAT,
                 (BSL_META_U(unit,
                             "bcm_stat_get: Statistic not supported: %d\n"), type));
        return BCM_E_PARAM;
    }

    *val = count;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_stat_mxq_ge_get
 * Description:
 *      Get the specified statistic for a port with common counter shared
 *      by both GE on MXQ MAC.
 * Parameters:
 *      unit - PCI device unit number (driver internal)
 *      port - zero-based port number
 *      type - SNMP statistics type (see stat.h)
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_PARAM - Illegal parameter.
 *      BCM_E_INTERNAL - Chip access failure.
 */
int
_bcm_petra_stat_mxq_ge_get(int unit, bcm_port_t port, bcm_stat_val_t type,
                      uint64 *val)
{
    uint64 count;    
    int pad_size;
    int rv = BCM_E_NONE;

    REG_MATH_DECL;       /* Required for use of the REG_* macros */
    COMPILER_REFERENCE(&count);  /* PPC compiler */
    COMPILER_64_ZERO(count);

    switch (type) {
        /* *** RFC 1213 *** */
    case snmpIfInOctets:
        REG_ADD(unit, port, MXQ_RBYTr, count);
        REG_ADD(unit, port, MXQ_RRBYTr, count);
        break;
    case snmpIfInUcastPkts:             /* Unicast frames */
        REG_ADD(unit, port, MXQ_RUCAr, count);  /* unicast pkts rcvd */
        break;
    case snmpIfInNUcastPkts:             /* Non-unicast frames */
        REG_ADD(unit, port, MXQ_RMCAr, count); /* + multicast */
        REG_ADD(unit, port, MXQ_RBCAr, count); /* + broadcast */
        break;
    case snmpIfInBroadcastPkts:          /* Broadcast frames */
        REG_ADD(unit, port, MXQ_RBCAr, count);
        break;
    case snmpIfInMulticastPkts:          /* Multicast frames */
        REG_ADD(unit, port, MXQ_RMCAr, count);
        break;
    case snmpIfInDiscards:           /* Dropped packets including aborted */
    
    return BCM_E_UNAVAIL;
        
        break;
    case snmpIfInErrors: /* RX Errors or Receive packets - non-error frames */
        REG_ADD(unit, port, MXQ_RFCSr, count);
        REG_ADD(unit, port, MXQ_RJBRr, count);
        REG_ADD(unit, port, MXQ_RRPKTr, count);
        REG_ADD(unit, port, MXQ_RMTUEr, count);
        break;
    case snmpIfInUnknownProtos:
        break;
    case snmpIfOutOctets:
        REG_ADD(unit, port, MXQ_TBYTr, count);
        break;
    case snmpIfOutUcastPkts:             /* Unicast frames */
        REG_ADD(unit, port, MXQ_TUCAr, count);
        break;
    case snmpIfOutNUcastPkts:            /* Non-unicast frames */
        REG_ADD(unit, port, MXQ_TMCAr, count); /* + multicast */
        REG_ADD(unit, port, MXQ_TBCAr, count); /* + broadcast */
        break;
    case snmpIfOutBroadcastPkts:         /* Broadcast frames */
        REG_ADD(unit, port, MXQ_TBCAr, count);
        break;
    case snmpIfOutMulticastPkts:         /* Multicast frames */
        REG_ADD(unit, port, MXQ_TMCAr, count);
        break;
    case snmpIfOutDiscards:              /* Aged packet counter */
        
        return BCM_E_UNAVAIL;
        break;
    case snmpIfOutErrors:
        if ((rv = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_pad_size_get,(unit, port, &pad_size))) != BCM_E_NONE)
        {
            return rv;
        }
        if ((pad_size >= 64 && pad_size <= 96)) {
            REG_ADD(unit, port, MXQ_TFCSr, count);
            REG_ADD(unit, port, MXQ_TJBRr, count);
        } else {
            REG_ADD(unit, port, MXQ_TRPKTr, count);
            REG_ADD(unit, port, MXQ_TFCSr, count);
            REG_ADD(unit, port, MXQ_TJBRr, count);
        }
        break;
    case snmpIpInReceives:
    case snmpIpInHdrErrors:
    case snmpIpForwDatagrams:
    case snmpIpInDiscards:
        /*IP counters are not supported*/
        return BCM_E_UNAVAIL;
        break;

        /* *** RFC 1493 *** */

    case snmpDot1dBasePortDelayExceededDiscards:
        break;
    case snmpDot1dBasePortMtuExceededDiscards:
        REG_ADD(unit, port, MXQ_RMTUEr, count);
        break;
    case snmpDot1dTpPortInFrames:
        REG_ADD(unit, port, MXQ_RPKTr, count);
        break;
    case snmpDot1dTpPortOutFrames:
        REG_ADD(unit, port, MXQ_TPKTr, count);
        break;
    case snmpDot1dPortInDiscards:
        
        return BCM_E_UNAVAIL;
        break;

        /* *** RFC 1757 *** */

    case snmpEtherStatsDropEvents:
        
        return BCM_E_UNAVAIL;

        break;
    case snmpEtherStatsOctets:
        REG_ADD(unit, port, MXQ_RBYTr, count);
        REG_ADD(unit, port, MXQ_TBYTr, count);
        REG_ADD(unit, port, MXQ_RRBYTr, count);
        break;
    case snmpEtherStatsPkts:
        REG_ADD(unit, port, MXQ_RPKTr, count);
        REG_ADD(unit, port, MXQ_RFLRr, count);
        REG_ADD(unit, port, MXQ_TPKTr, count);
        REG_ADD(unit, port, MXQ_RRPKTr, count); /* Runts */
        break;
    case snmpEtherStatsBroadcastPkts:
        REG_ADD(unit, port, MXQ_RBCAr, count);
        REG_ADD(unit, port, MXQ_TBCAr, count);
        break;
    case snmpEtherStatsMulticastPkts:
        REG_ADD(unit, port, MXQ_RMCAr, count);
        REG_ADD(unit, port, MXQ_TMCAr, count);
        break;
    case snmpEtherStatsCRCAlignErrors:
        REG_ADD(unit, port, MXQ_RFCSr, count);
        break;
    case snmpEtherStatsTxCRCAlignErrors:
        REG_ADD(unit, port, MXQ_TFCSr, count);
        break;
    case snmpEtherStatsUndersizePkts:
        REG_ADD(unit, port, MXQ_RUNDr, count);
        break;
    case snmpEtherStatsOversizePkts:
        REG_ADD(unit, port, MXQ_ROVRr, count);
        REG_ADD(unit, port, MXQ_TOVRr, count);
        break;
    case snmpEtherRxOversizePkts:
        REG_ADD(unit, port, MXQ_ROVRr, count);
        break;
    case snmpEtherTxOversizePkts:
        REG_ADD(unit, port, MXQ_TOVRr, count);
        break;
    case snmpEtherStatsFragments:
        REG_ADD(unit, port, MXQ_RFRGr, count);
        break;
    case snmpEtherStatsJabbers:
        REG_ADD(unit, port, MXQ_RJBRr, count);
        break;
    case snmpEtherStatsTxJabbers:
        REG_ADD(unit, port, MXQ_TJBRr, count);
        break;
    case snmpEtherStatsCollisions:
        break;

        /* *** rfc1757 definition counts receive packet only *** */

    case snmpEtherStatsPkts64Octets:
        REG_ADD(unit, port, MXQ_R64r, count);
        REG_ADD(unit, port, MXQ_T64r, count);
        break;
    case snmpEtherStatsPkts65to127Octets:
        REG_ADD(unit, port, MXQ_R127r, count);
        REG_ADD(unit, port, MXQ_T127r, count);
        break;
    case snmpEtherStatsPkts128to255Octets:
        REG_ADD(unit, port, MXQ_R255r, count);
        REG_ADD(unit, port, MXQ_T255r, count);
        break;
    case snmpEtherStatsPkts256to511Octets:
        REG_ADD(unit, port, MXQ_R511r, count);
        REG_ADD(unit, port, MXQ_T511r, count);
        break;
    case snmpEtherStatsPkts512to1023Octets:
        REG_ADD(unit, port, MXQ_R1023r, count);
        REG_ADD(unit, port, MXQ_T1023r, count);
        break;
    case snmpEtherStatsPkts1024to1518Octets:
        REG_ADD(unit, port, MXQ_R1518r, count);
        REG_ADD(unit, port, MXQ_T1518r, count);
        break;
    case snmpBcmEtherStatsPkts1519to1522Octets: /* not in rfc1757 */
        REG_ADD(unit, port, MXQ_RMGVr, count);
        REG_ADD(unit, port, MXQ_TMGVr, count);
        break;
    case snmpBcmEtherStatsPkts1522to2047Octets: /* not in rfc1757 */
        REG_ADD(unit, port, MXQ_R2047r, count);
        REG_SUB(unit, port, MXQ_RMGVr, count);
        REG_ADD(unit, port, MXQ_T2047r, count);
        REG_SUB(unit, port, MXQ_TMGVr, count);
        break;
    case snmpBcmEtherStatsPkts2048to4095Octets: /* not in rfc1757 */
        REG_ADD(unit, port, MXQ_R4095r, count);
        REG_ADD(unit, port, MXQ_T4095r, count);
        break;
    case snmpBcmEtherStatsPkts4095to9216Octets: /* not in rfc1757 */
        REG_ADD(unit, port, MXQ_R9216r, count);
        REG_ADD(unit, port, MXQ_T9216r, count);
        break;

    case snmpBcmEtherStatsPkts9217to16383Octets: /* not in rfc1757 */
        REG_ADD(unit, port, MXQ_R16383r, count);
        REG_ADD(unit, port, MXQ_T16383r, count);
        break;
    case snmpBcmReceivedPkts64Octets:
        REG_ADD(unit, port, MXQ_R64r, count);
        break;
    case snmpBcmReceivedPkts65to127Octets:
        REG_ADD(unit, port, MXQ_R127r, count);
        break;
    case snmpBcmReceivedPkts128to255Octets:
        REG_ADD(unit, port, MXQ_R255r, count);
        break;
    case snmpBcmReceivedPkts256to511Octets:
        REG_ADD(unit, port, MXQ_R511r, count);
        break;
    case snmpBcmReceivedPkts512to1023Octets:
        REG_ADD(unit, port, MXQ_R1023r, count);
        break;
    case snmpBcmReceivedPkts1024to1518Octets:
        REG_ADD(unit, port, MXQ_R1518r, count);
        break;
    case snmpBcmReceivedPkts1519to2047Octets:
        REG_ADD(unit, port, MXQ_R2047r, count);
        break;
    case snmpBcmReceivedPkts2048to4095Octets:
        REG_ADD(unit, port, MXQ_R4095r, count);
        break;
    case snmpBcmReceivedPkts4095to9216Octets:
        REG_ADD(unit, port, MXQ_R9216r, count);
        break;
    case snmpBcmReceivedPkts9217to16383Octets:
        REG_ADD(unit, port, MXQ_R16383r, count);
        break;
    case snmpBcmTransmittedPkts64Octets:
        REG_ADD(unit, port, MXQ_T64r, count);
        break;
    case snmpBcmTransmittedPkts65to127Octets:
        REG_ADD(unit, port, MXQ_T127r, count);
        break;
    case snmpBcmTransmittedPkts128to255Octets:
        REG_ADD(unit, port, MXQ_T255r, count);
        break;
    case snmpBcmTransmittedPkts256to511Octets:
        REG_ADD(unit, port, MXQ_T511r, count);
        break;
    case snmpBcmTransmittedPkts512to1023Octets:
        REG_ADD(unit, port, MXQ_T1023r, count);
        break;
    case snmpBcmTransmittedPkts1024to1518Octets:
        REG_ADD(unit, port, MXQ_T1518r, count);
        break;
    case snmpBcmTransmittedPkts1519to2047Octets:
        REG_ADD(unit, port, MXQ_T2047r, count);
        break;
    case snmpBcmTransmittedPkts2048to4095Octets:
        REG_ADD(unit, port, MXQ_T4095r, count);
        break;
    case snmpBcmTransmittedPkts4095to9216Octets:
        REG_ADD(unit, port, MXQ_T9216r, count);
        break;
    case snmpBcmTransmittedPkts9217to16383Octets:
        REG_ADD(unit, port, MXQ_T16383r, count);
        break;
    case snmpEtherStatsTXNoErrors:
        REG_ADD(unit, port, MXQ_TPOKr, count);
        break;
    case snmpEtherStatsRXNoErrors:
        REG_ADD(unit, port, MXQ_RPOKr, count);
        REG_ADD(unit, port, MXQ_RFLRr, count);
        break;

        /* *** RFC 2665 *** */

    case snmpDot3StatsAlignmentErrors:
        break;
    case snmpDot3StatsFCSErrors:
        REG_ADD(unit, port, MXQ_RFCSr, count);
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
        REG_ADD(unit, port, MXQ_TUFLr, count);
        REG_ADD(unit, port, MXQ_TERRr, count);
        break;
    case snmpDot3StatsCarrierSenseErrors:
        break;
    case snmpDot3StatsFrameTooLongs:
        REG_ADD(unit, port, MXQ_RMTUEr, count);
        break;
    case snmpDot3StatsInternalMacReceiveErrors:
        REG_ADD(unit, port, MXQ_RRPKTr, count);
        REG_ADD(unit, port, RERPKTr, count);
        break;
    case snmpDot3StatsSymbolErrors:
        REG_ADD(unit, port, MXQ_RERPKTr, count);
        break;
    case snmpDot3ControlInUnknownOpcodes:
        REG_ADD(unit, port, MXQ_RXUOr, count);
        break;
    case snmpDot3InPauseFrames:
        REG_ADD(unit, port, MXQ_RXPFr, count);
        break;
    case snmpDot3OutPauseFrames:
        REG_ADD(unit, port, MXQ_TXPFr, count);
        break;

        /* *** RFC 2233 high capacity versions of RFC1213 objects *** */

    case snmpIfHCInOctets:
        REG_ADD(unit, port, MXQ_RBYTr, count);
        REG_ADD(unit, port, MXQ_RRBYTr, count);
        break;
    case snmpIfHCInUcastPkts:

        REG_ADD(unit, port, MXQ_RUCAr, count);  /* unicast pkts rcvd */
        break;
    case snmpIfHCInMulticastPkts:
        REG_ADD(unit, port, MXQ_RMCAr, count);
        break;
    case snmpIfHCInBroadcastPkts:
        REG_ADD(unit, port, MXQ_RBCAr, count);
        break;
    case snmpIfHCOutOctets:
        REG_ADD(unit, port, MXQ_TBYTr, count);
        break;
    case snmpIfHCOutUcastPkts:
        REG_ADD(unit, port, MXQ_TUCAr, count);
        break;
    case snmpIfHCOutMulticastPkts:
        REG_ADD(unit, port, MXQ_TMCAr, count);
        break;
    case snmpIfHCOutBroadcastPckts:
        REG_ADD(unit, port, MXQ_TBCAr, count);
        break;

        /* *** RFC 2465 *** */

    case snmpIpv6IfStatsInReceives:
    case snmpIpv6IfStatsInHdrErrors:
    case snmpIpv6IfStatsInAddrErrors:
    case snmpIpv6IfStatsInDiscards:
    case snmpIpv6IfStatsOutForwDatagrams:
    case snmpIpv6IfStatsOutDiscards:
    case snmpIpv6IfStatsInMcastPkts:
    case snmpIpv6IfStatsOutMcastPkts:
        /*ip counters - unsupported*/
        return BCM_E_UNAVAIL;
        break;

        /* *** IEEE 802.1bb *** */
    case snmpIeee8021PfcRequests:
        REG_ADD(unit, port, MXQ_TXPPr, count);
        break;
    case snmpIeee8021PfcIndications:
        REG_ADD(unit, port, MXQ_RXPPr, count);
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
    case snmpBcmIPMCRoutedPckts:
    case snmpBcmIPMCInDroppedPckts:
    case snmpBcmIPMCOutDroppedPckts:
        /*ip counters - unsupported*/
        return BCM_E_UNAVAIL;
        break;

        /* EA (broadcom specific) - unsupported in XGS */
	case snmpBcmReceivedUndersizePkts:
		REG_ADD(unit, port, MXQ_RRPKTr, count);
		break;
    case snmpBcmTransmittedUndersizePkts:
        REG_ADD(unit, port, MXQ_TRPKTr, count);
        break;
    /* Receive PFC Frame Priority */
    case snmpBcmRxPFCFramePriority0:
        REG_ADD(unit, port, MXQ_RPFC0r, count);
        break;
    case snmpBcmRxPFCFramePriority1:
        REG_ADD(unit, port, MXQ_RPFC1r, count);
        break;
    case snmpBcmRxPFCFramePriority2:
        REG_ADD(unit, port, MXQ_RPFC2r, count);
        break;
    case snmpBcmRxPFCFramePriority3:
        REG_ADD(unit, port, MXQ_RPFC3r, count);
        break;
    case snmpBcmRxPFCFramePriority4:
        REG_ADD(unit, port, MXQ_RPFC4r, count);
        break;
    case snmpBcmRxPFCFramePriority5:
        REG_ADD(unit, port, MXQ_RPFC5r, count);
        break;
    case snmpBcmRxPFCFramePriority6:
        REG_ADD(unit, port, MXQ_RPFC6r, count);
        break;
    case snmpBcmRxPFCFramePriority7:
        REG_ADD(unit, port, MXQ_RPFC7r, count);
        break;
    /* Transmit PFC Frame Priority */
    case snmpBcmTxPFCFramePriority0:
        REG_ADD(unit, port, MXQ_TPFC0r, count);
        break;
    case snmpBcmTxPFCFramePriority1:
        REG_ADD(unit, port, MXQ_TPFC1r, count);
        break;
    case snmpBcmTxPFCFramePriority2:
        REG_ADD(unit, port, MXQ_TPFC2r, count);
        break;
    case snmpBcmTxPFCFramePriority3:
        REG_ADD(unit, port, MXQ_TPFC3r, count);
        break;
    case snmpBcmTxPFCFramePriority4:
        REG_ADD(unit, port, MXQ_TPFC4r, count);
        break;
    case snmpBcmTxPFCFramePriority5:
        REG_ADD(unit, port, MXQ_TPFC5r, count);
        break;
    case snmpBcmTxPFCFramePriority6:
        REG_ADD(unit, port, MXQ_TPFC6r, count);
        break;
    case snmpBcmTxPFCFramePriority7:
        REG_ADD(unit, port, MXQ_TPFC7r, count);
        break;
    /* Receive Truncated Frame */
    case snmpBcmReceivedTruncatedPkts:
        REG_ADD(unit, port, MXQ_RTRFUr, count);
        break;
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

    default:
        if (type < snmpValCount) {
            return BCM_E_UNAVAIL;
        }
        LOG_WARN(BSL_LS_BCM_STAT,
                 (BSL_META_U(unit,
                             "_bcm_stat_generic_get: Statistic not supported: %d\n"), type));
        return BCM_E_PARAM;
    }

    *val = count;

    return BCM_E_NONE;

}

