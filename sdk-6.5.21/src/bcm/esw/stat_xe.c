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
 *      _bcm_stat_xe_get
 * Description:
 *      Get the specified statistic for an XE port on the StrataSwitch family
 *      of devices.
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
_bcm_stat_xe_get_set(int unit, bcm_port_t port, 
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
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRBYTr, &count);
        break;
    case snmpIfInUcastPkts:
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            if (SOC_REG_IS_VALID(unit, IRUCAr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, 
                                  reg_op, IRUCAr, &count);
            } else if (SOC_REG_IS_VALID(unit, IRUCr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, 
                                  reg_op, IRUCr, &count);
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                  RUCr, &count);  /* unicast pkts rcvd */
            }
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRPKTr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRMCAr, &count); /* - multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRBCAr, &count); /* - broadcast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRFCSr, &count); /* - bad FCS */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRXCFr, &count); /* - good FCS, all MAC ctrl */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRXPFr, &count); /* - not included in IRXCFr */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                              IRJBRr, &count); /* - oversize, bad FCS */
            if (COUNT_OVR_ERRORS(unit)) {
                if (SOC_REG_IS_VALID(unit, CLMIB_ROVRr)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  CLMIB_ROVRr, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  IROVRr, &count); /* - oversize, good FCS */
                }
            }
        }
        break;
    case snmpIfInNUcastPkts:    /* Multicast frames plus broadcast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          IRMCAr, &count); /* + multicast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          IRBCAr, &count); /* + broadcast */
        break;
    case snmpIfInBroadcastPkts:  /* broadcast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          IRBCAr, &count); /* + broadcast */
        break;
    case snmpIfInMulticastPkts:  /* Multicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          IRMCAr, &count); /* + multicast */
        break;
    case snmpIfInDiscards:    /* Dropped packets including aborted */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          RDBGC0r, &count); /* Ingress drop conditions */
        BCM_IF_ERROR_RETURN
            (_bcm_stat_counter_non_dma_extra_get(
                unit,
                SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING,
                port, &count1));
        COMPILER_64_ADD_64(count, count1);
        break;
    case snmpIfInErrors: /* RX Errors or Receive packets - non-error frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRFCSr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRJBRr, &count);
        if (COUNT_OVR_ERRORS(unit)) {
            if (SOC_REG_IS_VALID(unit, CLMIB_ROVRr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              CLMIB_ROVRr, &count);
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              IROVRr, &count);
            }
        }
		if (soc_feature(unit, soc_feature_stat_jumbo_adj)) { 
			BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRMEGr, &count); 
			BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRMEBr, &count); 
		} 
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRUNDr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRFRGr, &count);
        break;
    case snmpIfInUnknownProtos:
        break;
    case snmpIfOutOctets:    /* TX bytes */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITBYTr, &count);
        break;
    case snmpIfOutUcastPkts:    /* ALL - mcast - bcast */
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            /* This register has a different name on some devices.
             * Only one value is added here. */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              ITUCr, &count);  /* unicast pkts sent */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              ITUCAr, &count);  /* unicast pkts sent */
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITPKTr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              ITMCAr, &count); /* - multicast */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              ITBCAr, &count); /* - broadcast */
        }
        break;
    case snmpIfOutNUcastPkts:    /* broadcast frames plus multicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          ITMCAr, &count); /* + multicast */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          ITBCAr, &count); /* + broadcast */
        break;
    case snmpIfOutBroadcastPkts:    /* broadcast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          ITBCAr, &count); /* + broadcast */
        break;
    case snmpIfOutMulticastPkts:    /* multicast frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                          ITMCAr, &count); /* + multicast */
        break;
    case snmpIfOutDiscards:    /* Aged packet counter */
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            if (SOC_REG_IS_VALID(unit, HOLDr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                  HOLDr, &count);  /* L2 MTU drops */
            } else if (SOC_REG_IS_VALID(unit, HOL_DROPr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                  HOL_DROPr, &count);
            }
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC3r, &count);
            BCM_IF_ERROR_RETURN
                (_bcm_stat_counter_extra_get(unit, EGRDROPPKTCOUNTr,
                                             port, &count1));
            COMPILER_64_ADD_64(count, count1);
            BCM_IF_ERROR_RETURN
                (bcm_esw_cosq_stat_get(unit, port, BCM_COS_INVALID, 
                                   bcmCosqStatDroppedPackets, &count1));
            COMPILER_64_ADD_64(count, count1);
        }
        break;
    case snmpIfOutErrors:   /* Error packets could not be xmitted */
        
        break;
    case snmpIfOutQLen: {
        uint32  qcount;
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
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC4r, &count);
        }
        break;
    case snmpIpInDiscards:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE4r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD4r, &count);
        break;

        /* *** RFC 1493 *** */

    case snmpDot1dBasePortDelayExceededDiscards:
        break;
    case snmpDot1dBasePortMtuExceededDiscards:
        if (soc_feature(unit, soc_feature_stat_jumbo_adj)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRMEGr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRMEBr, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRJBRr, &count);
        }
        break;
    case snmpDot1dTpPortInFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRPKTr, &count);
        break;
    case snmpDot1dTpPortOutFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITPKTr, &count);
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
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRBYTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITBYTr, &count);
        break;
    case snmpEtherStatsPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITPKTr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          IRUNDr, &count); /* Runts */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                          IRFRGr, &count); /* Fragments */
        break;
    case snmpEtherStatsBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRBCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITBCAr, &count);
        break;
    case snmpEtherStatsMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRMCAr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITMCAr, &count);
        break;
    case snmpEtherStatsCRCAlignErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRFCSr, &count);
        break;
    case snmpEtherStatsUndersizePkts:  /* Undersize frames */
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRUNDr, &count);
        break;
    case snmpEtherStatsOversizePkts:
            if (SOC_REG_IS_VALID(unit, CLMIB_ROVRr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, CLMIB_ROVRr, &count);
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, CLMIB_TOVRr, &count);
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IROVRr, &count);
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITOVRr, &count);
            }
        break;
    case snmpEtherRxOversizePkts:
            if (SOC_REG_IS_VALID(unit, CLMIB_ROVRr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, CLMIB_ROVRr, &count);
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IROVRr, &count);
            }
        break;
    case snmpEtherTxOversizePkts:
            if (SOC_REG_IS_VALID(unit, CLMIB_TOVRr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, CLMIB_TOVRr, &count);
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITOVRr, &count);
            }
        break;
    case snmpEtherStatsFragments:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRFRGr, &count);
        if (SOC_REG_IS_VALID(unit, ITFRGr)) {
           BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITFRGr, &count);
        }
        break;
    case snmpEtherStatsJabbers:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRJBRr, &count);
        break;
    case snmpEtherStatsCollisions:
        break;
    case snmpEtherStatsPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT64r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR64r, &count);
        break;
    case snmpEtherStatsPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT127r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR127r, &count);
        break;
    case snmpEtherStatsPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT255r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR255r, &count);
        break;
    case snmpEtherStatsPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT511r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR511r, &count);
        break;
    case snmpEtherStatsPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT1023r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR1023r, &count);
        break;
    case snmpEtherStatsPkts1024to1518Octets:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT1518r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR1518r, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT2047r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR2047r, &count);
        }
        break;

        /* *** not actually in rfc1757 *** */

    case snmpBcmEtherStatsPkts1519to1522Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT2047r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR2047r, &count);
        break;
    case snmpBcmEtherStatsPkts1522to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT2047r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR2047r, &count);
        break;
    case snmpBcmEtherStatsPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT4095r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR4095r, &count);
        break;
    case snmpBcmEtherStatsPkts4095to9216Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR9216r, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT9216r, &count);
        break;

    case snmpBcmReceivedPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR64r, &count);
        break;
    case snmpBcmReceivedPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR127r, &count);
        break;
    case snmpBcmReceivedPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR255r, &count);
        break;
    case snmpBcmReceivedPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR511r, &count);
        break;
    case snmpBcmReceivedPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR1023r, &count);
        break;
    case snmpBcmReceivedPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR1518r, &count);
        break;
    case snmpBcmReceivedPkts1519to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR2047r, &count);
        break;
    case snmpBcmReceivedPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR4095r, &count);
        break;
    case snmpBcmReceivedPkts4095to9216Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR9216r, &count);
        break;
    case snmpBcmReceivedPkts9217to16383Octets:
        if (SOC_REG_IS_VALID(unit, IR16383r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IR16383r, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case snmpBcmTransmittedPkts64Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT64r, &count);
        break;
    case snmpBcmTransmittedPkts65to127Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT127r, &count);
        break;
    case snmpBcmTransmittedPkts128to255Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT255r, &count);
        break;
    case snmpBcmTransmittedPkts256to511Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT511r, &count);
        break;
    case snmpBcmTransmittedPkts512to1023Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT1023r, &count);
        break;
    case snmpBcmTransmittedPkts1024to1518Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT1518r, &count);
        break;
    case snmpBcmTransmittedPkts1519to2047Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT2047r, &count);
        break;
    case snmpBcmTransmittedPkts2048to4095Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT4095r, &count);
        break;
    case snmpBcmTransmittedPkts4095to9216Octets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT9216r, &count);
        break;
    case snmpBcmTransmittedPkts9217to16383Octets:
        if (SOC_REG_IS_VALID(unit, IT16383r)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IT16383r, &count);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case snmpEtherStatsTXNoErrors:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              ITPOKr, &count); /* All good packets */
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITPKTr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              ITFRGr, &count);
            if (COUNT_OVR_ERRORS(unit)) {
                if (SOC_REG_IS_VALID(unit, CLMIB_TOVRr)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  CLMIB_TOVRr, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  ITOVRr, &count);
                }
            }
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              ITUFLr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              ITERRr, &count);
        }
        break;
    case snmpEtherStatsRXNoErrors:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRPOKr, &count);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        {
            /*
             * IRPKT - (IRFCS + IRJBR + IROVR + IRUND + IRFLR +
             *           IRFRG + IRERPKT)
             */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRPKTr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRFCSr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRJBRr, &count);
            if (COUNT_OVR_ERRORS(unit)) {
                if (SOC_REG_IS_VALID(unit, CLMIB_ROVRr)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              CLMIB_ROVRr, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  IROVRr, &count);
                }
            }
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRUNDr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRFRGr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRERPKTr, &count);
        }
        break;

        /* *** RFC 2665 *** */

    case snmpDot3StatsAlignmentErrors:
        break;
    case snmpDot3StatsFCSErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRFCSr, &count);
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
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITUFLr, &count);
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITERRr, &count);
        break;
    case snmpDot3StatsCarrierSenseErrors:
        break;
    case snmpDot3StatsFrameTooLongs:
        if ((soc_feature(unit, soc_feature_stat_jumbo_adj)) &&
            (!SOC_IS_TRIUMPH(unit)) ) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRMEGr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRMEBr, &count);
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRJBRr, &count);
        }
        break;
    case snmpDot3StatsInternalMacReceiveErrors:
        break;
    case snmpDot3StatsSymbolErrors:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRERBYTr, &count);
        break;
    case snmpDot3ControlInUnknownOpcodes:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRXUOr, &count);
        break;
    case snmpDot3InPauseFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRXPFr, &count);
        break;
    case snmpDot3OutPauseFrames:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITXPFr, &count);
        break;

        /* *** RFC 2233 high capacity versions of RFC1213 objects *** */

    case snmpIfHCInOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRBYTr, &count);
        break;
    case snmpIfHCInUcastPkts:
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            if (SOC_REG_IS_VALID(unit, IRUCAr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                                  IRUCAr, &count);
            } else if (SOC_REG_IS_VALID(unit, IRUCr)) {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRUCr, &count);
            } else {
                BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RUCr, &count);  /* unicast pkts rcvd */
            }
        } else {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRPKTr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRMCAr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRBCAr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRFCSr, &count); /* - bad FCS */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRXCFr, &count); /* - good FCS, all MAC ctrl */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRXPFr, &count); /* - not included in IRXCFr */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              IRJBRr, &count); /* - oversize, bad FCS */
            if (COUNT_OVR_ERRORS(unit)) {
                if (SOC_REG_IS_VALID(unit, CLMIB_ROVRr)) {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  CLMIB_ROVRr, &count);
                } else {
                    BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op,
                                  IROVRr, &count); /* - oversize, good FCS */
                }
            }
        }
        break;
    case snmpIfHCInMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRMCAr, &count);
        break;
    case snmpIfHCInBroadcastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRBCAr, &count);
        break;
    case snmpIfHCOutOctets:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITBYTr, &count);
        break;
    case snmpIfHCOutUcastPkts:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        if (soc_feature(unit, soc_feature_hw_stats_calc)) {
            /* This register has a different name on some devices.
             * Only one value is added here. */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              ITUCr, &count); /* All good packets */
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, 
                              ITUCAr, &count);  /* unicast pkts sent */
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
        {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITPKTr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              ITMCAr, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_sub_op, 
                              ITBCAr, &count);
        }
        break;
    case snmpIfHCOutMulticastPkts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITMCAr, &count);
        break;
    case snmpIfHCOutBroadcastPckts:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITBCAr, &count);
        break;

        /* *** RFC 2465 *** */

    case snmpIpv6IfStatsInReceives:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPC6r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6r, &count);
        }
        break;
    case snmpIpv6IfStatsInHdrErrors:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
        }
        break;
    case snmpIpv6IfStatsInAddrErrors:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE6r, &count);
        }
        break;
    case snmpIpv6IfStatsInDiscards:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPHE6r, &count);
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, RIPD6r, &count);
        }
        break;
    case snmpIpv6IfStatsOutForwDatagrams:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC0r, &count);
        }
        break;
    case snmpIpv6IfStatsOutDiscards:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC1r, &count);
        }
        break;
    case snmpIpv6IfStatsInMcastPkts:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
             BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IMRP6r, &count);
        }
        break;
    case snmpIpv6IfStatsOutMcastPkts:
        if (soc_feature(unit, soc_feature_stat_xgs3)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, TDBGC2r, &count);
        }
        break;

        /* *** IEEE 802.1bb *** */
    case snmpIeee8021PfcRequests:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ITXPPr, &count);
        break;
    case snmpIeee8021PfcIndications:
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, IRXPPr, &count);
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
        if (soc_feature(unit, soc_feature_stat_xgs3)) { 
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

    case snmpBcmTxE2ECCControlFrames:
        if (SOC_REG_IS_VALID(unit, XTHOLr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, XTHOLr,
                              &count);
        }
        break;

    case snmpBcmRxDot3LengthMismatches:
        if (SOC_REG_IS_VALID(unit, IRFLRr)) {
            BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op,
                              IRFLRr, &count); /* Good FCS, bad length */
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
                                "_bcm_stat_xe_get: Statistic not supported: %d\n"), type));
        return BCM_E_PARAM;
    }

    if (stat_op == _BCM_STAT_GET) {
        *val = count;
    }

    return BCM_E_NONE;
}
