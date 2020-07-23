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

#define FE_CNTR(_fe)	(_fe)

/*
 * Function:
 *      _bcm_stat_fe_get
 * Description:
 *      Get the specified statistic for a FE port or GE port in 10/100 Mb
 *      mode on the StrataSwitch family of devices.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
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
_bcm_stat_fe_get_set(int unit, bcm_port_t port, int sync_mode, 
                     int stat_op, bcm_stat_val_t type, uint64 *val)
{
    uint64      count, count1;
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
                          FE_CNTR(RBYTr), &count);
        break;
    case snmpIfInUcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RPKTr), &count); /* all pkts rcvd */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(RMCAr), &count); /* - multicast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(RBCAr), &count); /* - broadcast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(RALNr), &count); /* - bad FCS, dribble bit */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, FE_CNTR(RFCSr), 
                          &count); /* - bad FCS, no dribble bit */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, FE_CNTR(RFLRr), 
                          &count); /* - good FCS, bad length */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, FE_CNTR(RJBRr), 
                          &count); /* - oversize, bad FCS */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, FE_CNTR(ROVRr),
                              &count); /* - oversize, good FCS */
        }
        break;
    case snmpIfInNUcastPkts:  /* Multicast frames plus broadcast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RMCAr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RBCAr), &count);
        break;
    case snmpIfInBroadcastPkts:  /* broadcast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RBCAr), &count);
        break;
    case snmpIfInMulticastPkts:  /* Multicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RMCAr), &count);
        break;
    case snmpIfInDiscards:  /* Dropped packets including aborted */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RDISCr), &count);
        break;
    case snmpIfInErrors:        /* received packts containing errors */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RUNDr), &count);  /* undersize, ok FCS */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RFRGr), &count);  /* ..., bad FCS */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RFCSr), &count);  /* FCS errors */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              FE_CNTR(ROVRr), &count);  /* oversize, ok FCS */
        }
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RJBRr), &count);  /* ..., bad FCS */
        break;
    case snmpIfInUnknownProtos:
        /* always 0 */
        break;
    case snmpIfOutOctets:        /* TX bytes */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TBYTr), &count);
        break;
    case snmpIfOutUcastPkts:        /* ALL - mcast - bcast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TPKTr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(TMCAr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(TBCAr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(TFCSr), &count); /* bad FCS */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(TJBRr), &count); /* oversize, bad FCS */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              FE_CNTR(TOVRr), &count);  /* oversize, good FCS */
        }
        break;
    case snmpIfOutNUcastPkts:  /* broadcast frames plus multicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TMCAr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TBCAr), &count);
        break;
    case snmpIfOutBroadcastPkts:        /* broadcast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TBCAr), &count);
        break;
    case snmpIfOutMulticastPkts:        /* multicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TMCAr), &count);
        break;
    case snmpIfOutDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TEDFr), &count);  /* multiple deferral */
        _bcm_stat_counter_extra_get(unit, EGRDROPPKTCOUNTr, port, &count1);
        COMPILER_64_ADD_64(count, count1);
        /* other causes of discards? */
        break;
    case snmpIfOutErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TXCLr), &count);  /* excessive coll */
        break;
    case snmpIfOutQLen: {
        uint32  qcount;
        if (bcm_esw_port_queued_count_get(unit, port, &qcount) >= 0) {
            COMPILER_64_ADD_32(count, qcount);
        }
    }
    break;
    case snmpIpInReceives:
        /* XGS2 specific */
        break;
    case snmpIpInHdrErrors:
        break;
    case snmpIpForwDatagrams:
        /* XGS2 specific */
        break;
    case snmpIpInDiscards:
        /* XGS2 specific */
        break;

        /* *** RFC 1493 *** */

    case snmpDot1dBasePortDelayExceededDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TEDFr), &count);  /* multiple deferral */
        break;
    case snmpDot1dBasePortMtuExceededDiscards:
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              FE_CNTR(ROVRr), &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              FE_CNTR(TOVRr), &count);
        }
        break;
    case snmpDot1dTpPortInFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RPKTr), &count);
        break;
    case snmpDot1dTpPortOutFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TPKTr), &count);
        break;
    case snmpDot1dPortInDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RDISCr), &count);
        break;

        /* *** RFC 1757 *** */

    case snmpEtherStatsDropEvents:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RDISCr), &count);
        break;
    case snmpEtherStatsOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RBYTr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TBYTr), &count);
        break;
    case snmpEtherStatsPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RPKTr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TPKTr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          FE_CNTR(RUNDr), &count); /* Runt packets */
        break;
    case snmpEtherStatsBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RBCAr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TBCAr), &count);
        break;
    case snmpEtherStatsMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RMCAr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TMCAr), &count);
        break;
    case snmpEtherStatsCRCAlignErrors:  /* CRC errors + alignment errors */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RFCSr), &count);
        break;
    case snmpEtherStatsUndersizePkts:  /* Undersize frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RUNDr), &count);
        break;
    case snmpEtherStatsOversizePkts:
        if (soc_feature(unit, soc_feature_stat_jumbo_adj)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              FE_CNTR(ROVRr), &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              FE_CNTR(TOVRr), &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              FE_CNTR(RJBRr), &count);
        }
        break;
    case snmpEtherRxOversizePkts:
        if (soc_feature(unit, soc_feature_stat_jumbo_adj)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                  FE_CNTR(ROVRr), &count);
        }
        break;
    case snmpEtherTxOversizePkts:
        if (soc_feature(unit, soc_feature_stat_jumbo_adj)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                  FE_CNTR(TOVRr), &count);
            }
        break;
     case snmpEtherStatsFragments:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          FE_CNTR(RFRGr), &count);
        if (SOC_REG_IS_VALID(unit,TFRGr)) {
           BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                             FE_CNTR(TFRGr), &count);
        }
        break;
    case snmpEtherStatsJabbers:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          FE_CNTR(RJBRr), &count);
        break;
    case snmpEtherStatsCollisions:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TNCLr), &count);
        break;
    case snmpEtherStatsPkts64Octets:
    case snmpEtherStatsPkts65to127Octets:
    case snmpEtherStatsPkts128to255Octets:
    case snmpEtherStatsPkts256to511Octets:
    case snmpEtherStatsPkts512to1023Octets:
        /* XGS2 specific */
        break;
    case snmpEtherStatsPkts1024to1518Octets:
        break;

        /* *** not actually in rfc1757 *** */

    case snmpBcmEtherStatsPkts1519to1522Octets:
    case snmpBcmEtherStatsPkts1522to2047Octets:
    case snmpBcmEtherStatsPkts2048to4095Octets:
    case snmpBcmEtherStatsPkts4095to9216Octets:
    case snmpBcmEtherStatsPkts9217to16383Octets:
    case snmpBcmReceivedPkts64Octets:
    case snmpBcmReceivedPkts65to127Octets:
    case snmpBcmReceivedPkts128to255Octets:
    case snmpBcmReceivedPkts256to511Octets:
    case snmpBcmReceivedPkts512to1023Octets:
    case snmpBcmReceivedPkts1024to1518Octets:
    case snmpBcmReceivedPkts1519to2047Octets:
    case snmpBcmReceivedPkts2048to4095Octets:
    case snmpBcmReceivedPkts4095to9216Octets:
    case snmpBcmReceivedPkts9217to16383Octets:
    case snmpBcmTransmittedPkts64Octets:
    case snmpBcmTransmittedPkts65to127Octets:
    case snmpBcmTransmittedPkts128to255Octets:
    case snmpBcmTransmittedPkts256to511Octets:
    case snmpBcmTransmittedPkts512to1023Octets:
    case snmpBcmTransmittedPkts1024to1518Octets:
    case snmpBcmTransmittedPkts1519to2047Octets:
    case snmpBcmTransmittedPkts2048to4095Octets:
    case snmpBcmTransmittedPkts4095to9216Octets:
    case snmpBcmTransmittedPkts9217to16383Octets:
        break;

    case snmpEtherStatsTXNoErrors:
        /*  TPKT - (TNCL + TOVR + TFRG + TUND) */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TPKTr), &count);  /* All Packets */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(TNCLr), &count);  /* collision */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              FE_CNTR(TOVRr), &count);  /* Oversize */
        }
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(TFRGr), &count);  /* Fragments */
        break;
    case snmpEtherStatsRXNoErrors:
        /* RPKT - ( RFCS + RXUO + ROVR + RFLR) */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RPKTr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(RFCSr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(RXUOr), &count);
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              FE_CNTR(ROVRr), &count);
        }
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(RFLRr), &count);
        break;

        /* *** RFC 2665 *** */

    case snmpDot3StatsAlignmentErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RALNr), &count);
        break;
    case snmpDot3StatsFCSErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RFCSr), &count);
        break;
    case snmpDot3StatsSingleCollisionFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TSCLr), &count);
        break;
    case snmpDot3StatsMultipleCollisionFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TMCLr), &count);
        break;
    case snmpDot3StatsSQETTestErrors:
        /* always 0 */
        break;
    case snmpDot3StatsDeferredTransmissions:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TDFRr), &count);
        break;
    case snmpDot3StatsLateCollisions:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TLCLr), &count);
        break;
    case snmpDot3StatsExcessiveCollisions:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TXCLr), &count);
        break;
    case snmpDot3StatsInternalMacTransmitErrors:
        /* always 0 */
        break;
    case snmpDot3StatsCarrierSenseErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          FE_CNTR(RFCRr), &count);
        break;
    case snmpDot3StatsFrameTooLongs:
        if (soc_feature(unit, soc_feature_stat_jumbo_adj)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              FE_CNTR(ROVRr), &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              FE_CNTR(RJBRr), &count);
        }
        break;
    case snmpDot3StatsInternalMacReceiveErrors:
        /* always 0 */
        break;
    case snmpDot3StatsSymbolErrors:
        /* XGS2 specific */
        break;
    case snmpDot3ControlInUnknownOpcodes:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RXUOr), &count);
        break;
    case snmpDot3InPauseFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RXPFr), &count);
        break;
    case snmpDot3OutPauseFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TXPFr), &count);
        break;

        /* *** RFC 2233 high capacity versions of RFC1213 objects *** */

    case snmpIfHCInOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RBYTr), &count);
        break;
    case snmpIfHCInUcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RPKTr), &count); /* all pkts rcvd */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(RMCAr), &count); /* - multicast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(RBCAr), &count); /* - broadcast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(RALNr), &count); /* - bad FCS, dribble bit  */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, FE_CNTR(RFCSr), 
                          &count); /* - bad FCS, no dribble bit */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(RFLRr), &count); /* - good FCS, bad length */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(RJBRr), &count); /* - oversize, bad FCS */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, FE_CNTR(ROVRr),
                              &count); /* - oversize, good FCS */
        }
        break;
    case snmpIfHCInMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RMCAr), &count);
        break;
    case snmpIfHCInBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(RBCAr), &count);
        break;
    case snmpIfHCOutOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TBYTr), &count);
        break;
    case snmpIfHCOutUcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TPKTr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(TMCAr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(TBCAr), &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(TFCSr), &count); /* bad FCS */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                          FE_CNTR(TJBRr), &count); /* oversize, bad FCS */
        if (COUNT_OVR_ERRORS(unit)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              FE_CNTR(TOVRr), &count);  /* oversize, good FCS */
        }
        break;
    case snmpIfHCOutMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TMCAr), &count);
        break;
    case snmpIfHCOutBroadcastPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          FE_CNTR(TBCAr), &count);
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

        /* IPMC counters (broadcom XGS2 specific) */
    case snmpBcmIPMCBridgedPckts:
    case snmpBcmIPMCRoutedPckts:
    case snmpBcmIPMCInDroppedPckts:
    case snmpBcmIPMCOutDroppedPckts:
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
                              EGR_FCOE_DELIMITER_ERROR_FRAMESr, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case snmpBcmTxE2ECCControlFrames:
        if (SOC_REG_IS_VALID(unit, XTHOLr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XTHOLr,
                              &count);
        }
        break;

    case snmpBcmRxDot3LengthMismatches:
        if (SOC_REG_IS_VALID(unit, RFLRr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, FE_CNTR(RFLRr),
                              &count); /* Good FCS, bad length */
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
