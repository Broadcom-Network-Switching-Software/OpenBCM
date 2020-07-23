/*! \file stat.c
 *
 * Stat Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <sal/core/boot.h>

#include <bcm/stat.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/stat.h>
#include <bcm_int/ltsw/stat_int.h>
#include <bcm_int/ltsw/mbcm/stat.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/event_mgr.h>
#include <bcm_int/ltsw/cosq_stat.h>

#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>
#include <sal/sal_mutex.h>
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_STAT

/* Mutex lock for stat sync operation. */
static sal_mutex_t stat_sync_mutex[BCM_MAX_NUM_UNITS];

/* Stat sync timeout. */
#define STAT_SYNC_TIMEOUT_USEC 20000000

/* Stat sync timeout of simulation. */
#define STAT_SYNC_TIMEOUT_USEC_SIM 60000000

/*!
 * \brief Stat MAC counters mapping table.
 */
static bcmint_stat_ctr_map_info_t ltsw_stat_mac_ctr_map_tab[] = {
    /*
     * RFC 1213 MIBs
     *
     * The following RFC 1213 MIBs are not supported:
     *
     * snmpIfInDiscards = 3
     * snmpIfInUnknownProtos = 5
     * snmpIfOutDiscards = 9
     * snmpIfOutQLen = 11
     * snmpIpInReceives = 12
     * snmpIpInHdrErrors = 13
     * snmpIpForwDatagrams = 14
     * snmpIpInDiscards = 15
     */
    /* snmpIfInOctets = 0 */
    {snmpIfInOctets, CTR_MACs, RX_BYTESs, FALSE},
    {snmpIfInOctets, CTR_MACs, RX_RUNT_PKT_BYTESs, FALSE},
    /* snmpIfInUcastPkts = 1 */
    {snmpIfInUcastPkts, CTR_MACs, RX_UC_PKTs, FALSE},
    /* snmpIfInNUcastPkts = 2 */
    {snmpIfInNUcastPkts, CTR_MACs, RX_MC_PKTs, FALSE},
    {snmpIfInNUcastPkts, CTR_MACs, RX_BC_PKTs, FALSE},
    /* snmpIfInErrors = 4 */
    {snmpIfInErrors, CTR_MAC_ERRs, RX_FCS_ERR_PKTs, FALSE},
    {snmpIfInErrors, CTR_MAC_ERRs, RX_JABBER_PKTs, FALSE},
    {snmpIfInErrors, CTR_MAC_ERRs, RX_MTU_CHECK_ERR_PKTs, FALSE},
    {snmpIfInErrors, CTR_MACs, RX_RUNT_PKTs, FALSE},
    /* snmpIfOutOctets = 6 */
    {snmpIfOutOctets, CTR_MACs, TX_BYTESs, FALSE},
    /* snmpIfOutUcastPkts = 7 */
    {snmpIfOutUcastPkts, CTR_MACs, TX_UC_PKTs, FALSE},
    /* snmpIfOutNUcastPkts = 8 */
    {snmpIfOutNUcastPkts, CTR_MACs, TX_MC_PKTs, FALSE},
    {snmpIfOutNUcastPkts, CTR_MACs, TX_BC_PKTs, FALSE},
    /* snmpIfOutErrors = 10 */
    {snmpIfOutErrors, CTR_MAC_ERRs, TX_ERR_PKTs, FALSE},

    /*
     * RFC 1493 MIBs
     *
     * The following RFC 1493 MIBs are not supported:
     *
     * snmpDot1dBasePortDelayExceededDiscards = 16
     * snmpDot1dPortInDiscards = 20
     */
    /* snmpDot1dBasePortMtuExceededDiscards = 17 */
    {snmpDot1dBasePortMtuExceededDiscards, CTR_MAC_ERRs,
     RX_MTU_CHECK_ERR_PKTs, FALSE},
    /* snmpDot1dTpPortInFrames = 18 */
    {snmpDot1dTpPortInFrames, CTR_MACs, RX_PKTs, FALSE},
    /* snmpDot1dTpPortOutFrames = 19 */
    {snmpDot1dTpPortOutFrames, CTR_MACs, TX_PKTs, FALSE},

    /*
     * RFC 1757 MIBs
     *
     * The following RFC 1757 MIBs are not supported:
     *
     * snmpEtherStatsDropEvents = 21
     * snmpEtherStatsCollisions =38
 */
    /* snmpEtherStatsMulticastPkts = 22 */
    {snmpEtherStatsMulticastPkts, CTR_MACs, RX_MC_PKTs, FALSE},
    {snmpEtherStatsMulticastPkts, CTR_MACs, TX_MC_PKTs, FALSE},
    /* snmpEtherStatsBroadcastPkts = 23 */
    {snmpEtherStatsBroadcastPkts, CTR_MACs, RX_BC_PKTs, FALSE},
    {snmpEtherStatsBroadcastPkts, CTR_MACs, TX_BC_PKTs, FALSE},
    /* snmpEtherStatsUndersizePkts = 24 */
    {snmpEtherStatsUndersizePkts, CTR_MAC_ERRs, RX_UNDER_SIZE_PKTs, FALSE},
    /* snmpEtherStatsFragments = 25 */
    {snmpEtherStatsFragments, CTR_MACs, RX_FRAGMENT_PKTs, FALSE},
    /* snmpEtherStatsPkts64Octets = 26 */
    {snmpEtherStatsPkts64Octets, CTR_MACs, RX_64B_PKTs, FALSE},
    {snmpEtherStatsPkts64Octets, CTR_MACs, TX_64B_PKTs, FALSE},
    /* snmpEtherStatsPkts65to127Octets = 27 */
    {snmpEtherStatsPkts65to127Octets, CTR_MACs, RX_127B_PKTs, FALSE},
    {snmpEtherStatsPkts65to127Octets, CTR_MACs, TX_127B_PKTs, FALSE},
    /* snmpEtherStatsPkts128to255Octets = 28 */
    {snmpEtherStatsPkts128to255Octets, CTR_MACs, RX_255B_PKTs, FALSE},
    {snmpEtherStatsPkts128to255Octets, CTR_MACs, TX_255B_PKTs, FALSE},
    /* snmpEtherStatsPkts256to511Octets = 29 */
    {snmpEtherStatsPkts256to511Octets, CTR_MACs, RX_511B_PKTs, FALSE},
    {snmpEtherStatsPkts256to511Octets, CTR_MACs, TX_511B_PKTs, FALSE},
    /* snmpEtherStatsPkts512to1023Octets = 30 */
    {snmpEtherStatsPkts512to1023Octets, CTR_MACs, RX_1023B_PKTs, FALSE},
    {snmpEtherStatsPkts512to1023Octets, CTR_MACs, TX_1023B_PKTs, FALSE},
    /* snmpEtherStatsPkts1024to1518Octets = 31 */
    {snmpEtherStatsPkts1024to1518Octets, CTR_MACs, RX_1518B_PKTs, FALSE},
    {snmpEtherStatsPkts1024to1518Octets, CTR_MACs, TX_1518B_PKTs, FALSE},
    /* snmpEtherStatsOversizePkts = 32 */
    {snmpEtherStatsOversizePkts, CTR_MAC_ERRs, RX_OVER_SIZE_PKTs, FALSE},
    {snmpEtherStatsOversizePkts, CTR_MAC_ERRs, TX_OVER_SIZE_PKTs, FALSE},
    /* snmpEtherRxOversizePkts = 33 */
    {snmpEtherRxOversizePkts, CTR_MAC_ERRs, RX_OVER_SIZE_PKTs, FALSE},
    /* snmpEtherTxOversizePkts = 34 */
    {snmpEtherTxOversizePkts, CTR_MAC_ERRs, TX_OVER_SIZE_PKTs, FALSE},
    /* snmpEtherStatsJabbers = 35 */
    {snmpEtherStatsJabbers, CTR_MAC_ERRs, RX_JABBER_PKTs, FALSE},
    /* snmpEtherStatsOctets = 36 */
    {snmpEtherStatsOctets, CTR_MACs, RX_BYTESs, FALSE},
    {snmpEtherStatsOctets, CTR_MACs, TX_BYTESs, FALSE},
    {snmpEtherStatsOctets, CTR_MACs, RX_RUNT_PKT_BYTESs, FALSE},
    /* snmpEtherStatsPkts = 37 */
    {snmpEtherStatsPkts, CTR_MACs, RX_PKTs, FALSE},
    {snmpEtherStatsPkts, CTR_MACs, TX_PKTs, FALSE},
    {snmpEtherStatsPkts, CTR_MACs, RX_RUNT_PKTs, FALSE},
    /* snmpEtherStatsCRCAlignErrors = 39 */
    {snmpEtherStatsCRCAlignErrors, CTR_MAC_ERRs, RX_FCS_ERR_PKTs, FALSE},
    /* snmpEtherStatsTXNoErrors = 40 */
    {snmpEtherStatsTXNoErrors, CTR_MACs, TX_OK_PKTs, FALSE},
    /* snmpEtherStatsRXNoErrors = 41 */
    {snmpEtherStatsRXNoErrors, CTR_MACs, RX_OK_PKTs, FALSE},

    /*
     * RFC 2665 MIBs
     *
     * The following RFC 2665 MIBs are not supported:
     *
     * snmpDot3StatsSingleCollisionFrames = 44
     * snmpDot3StatsMultipleCollisionFrames = 45
     * snmpDot3StatsSQETTestErrors = 46
     * snmpDot3StatsDeferredTransmissions = 47
     * snmpDot3StatsLateCollisions = 48
     * snmpDot3StatsExcessiveCollisions = 49
     * snmpDot3StatsInternalMacReceiveErrors = 53
 */
    /* snmpDot3StatsAlignmentErrors = 42 */
    {snmpDot3StatsAlignmentErrors, CTR_MAC_ERRs, RX_ALIGN_ERR_PKTs, FALSE},
    /* snmpDot3StatsFCSErrors = 43 */
    {snmpDot3StatsFCSErrors,   CTR_MAC_ERRs, RX_FCS_ERR_PKTs, FALSE},
    /* snmpDot3StatsInternalMacTransmitErrors = 50 */
    {snmpDot3StatsInternalMacTransmitErrors, CTR_MAC_ERRs,
     TX_ERR_PKTs, FALSE},
    {snmpDot3StatsInternalMacTransmitErrors, CTR_MAC_ERRs,
     TX_FIFO_UNDER_RUN_PKTs, FALSE},
    /* snmpDot3StatsCarrierSenseErrors = 51 */
    {snmpDot3StatsCarrierSenseErrors, CTR_MAC_ERRs,
     RX_FALSE_CARRIER_PKTs, FALSE},
    /* snmpDot3StatsFrameTooLongs = 52 */
    {snmpDot3StatsFrameTooLongs, CTR_MAC_ERRs, RX_MTU_CHECK_ERR_PKTs, FALSE},
    /* snmpDot3StatsSymbolErrors = 54 */
    {snmpDot3StatsSymbolErrors, CTR_MAC_ERRs, RX_CODE_ERR_PKTs, FALSE},
    /* snmpDot3ControlInUnknownOpcodes = 55 */
    {snmpDot3ControlInUnknownOpcodes, CTR_MAC_ERRs,
     RX_PFC_UNSUPPORTED_OPCODE_PKTs, FALSE},
    /* snmpDot3InPauseFrames = 56 */
    {snmpDot3InPauseFrames, CTR_MACs, RX_PAUSE_CTL_PKTs, FALSE},
    /* snmpDot3OutPauseFrames = 57 */
    {snmpDot3OutPauseFrames, CTR_MACs, TX_PAUSE_CTL_PKTs, FALSE},

    /*
     * RFC 2233 MIBs
 */
    /* snmpIfHCInOctets = 58 */
    {snmpIfHCInOctets, CTR_MACs, RX_BYTESs, FALSE},
    {snmpIfHCInOctets, CTR_MACs, RX_RUNT_PKT_BYTESs, FALSE},
    /* snmpIfHCInUcastPkts = 59 */
    {snmpIfHCInUcastPkts, CTR_MACs, RX_UC_PKTs, FALSE},
    /* snmpIfHCInMulticastPkts = 60 */
    {snmpIfHCInMulticastPkts, CTR_MACs, RX_MC_PKTs, FALSE},
    /* snmpIfHCInBroadcastPkts = 61 */
    {snmpIfHCInBroadcastPkts, CTR_MACs, RX_BC_PKTs, FALSE},
    /* snmpIfHCOutOctets = 62 */
    {snmpIfHCOutOctets, CTR_MACs, TX_BYTESs, FALSE},
    /* snmpIfHCOutUcastPkts = 63 */
    {snmpIfHCOutUcastPkts, CTR_MACs, TX_UC_PKTs, FALSE},
    /* snmpIfHCOutMulticastPkts = 64 */
    {snmpIfHCOutMulticastPkts, CTR_MACs, TX_MC_PKTs, FALSE},
    /* snmpIfHCOutBroadcastPckts = 65 */
    {snmpIfHCOutBroadcastPckts, CTR_MACs, TX_BC_PKTs, FALSE},

    /*
     * RFC 2465 MIBs
     *
     * The following RFC 2465 MIBs are not supported:
     *
     * snmpIpv6IfStatsInReceives = 66
     * snmpIpv6IfStatsInHdrErrors = 67
     * snmpIpv6IfStatsInAddrErrors = 68
     * snmpIpv6IfStatsInDiscards = 69
     * snmpIpv6IfStatsOutForwDatagrams = 70
     * snmpIpv6IfStatsOutDiscards = 71
     * snmpIpv6IfStatsInMcastPkts = 72
     * snmpIpv6IfStatsOutMcastPkts = 73
 */

    /*
     * RFC 1573 MIBs
     */
    /* snmpIfInBroadcastPkts = 74 */
    {snmpIfInBroadcastPkts, CTR_MACs, RX_BC_PKTs, FALSE},
    /* snmpIfInMulticastPkts = 75 */
    {snmpIfInMulticastPkts, CTR_MACs, RX_MC_PKTs, FALSE},
    /* snmpIfOutBroadcastPkts = 76 */
    {snmpIfOutBroadcastPkts, CTR_MACs, TX_BC_PKTs, FALSE},
    /* snmpIfOutMulticastPkts = 77 */
    {snmpIfOutMulticastPkts, CTR_MACs, TX_MC_PKTs, FALSE},

    /*
     * RFC IEEE 802.1bb MIBs
     */
    /* snmpIeee8021PfcRequests = 78 */
    {snmpIeee8021PfcRequests, CTR_MACs, TX_PER_PRI_PAUSE_CTL_PKTs, FALSE},
    /* snmpIeee8021PfcIndications = 79 */
    {snmpIeee8021PfcIndications, CTR_MACs, RX_PER_PRI_PAUSE_CTL_PKTs, FALSE},

    /*
     * Broadcom-specific MIBs
     *
     * The following Broadcom-specific MIBs are not supported:
     *
     * snmpBcmReceivedUndersizePkts = 80
     * snmpBcmTransmittedUndersizePkts = 81
     * snmpBcmIPMCBridgedPckts = 82
     * snmpBcmIPMCRoutedPckts = 83
     * snmpBcmIPMCInDroppedPckts = 84
     * snmpBcmIPMCOutDroppedPckts = 85
     * snmpBcmTxControlCells = 108
     * snmpBcmTxDataCells = 109
     * snmpBcmTxDataBytes = 110
     * snmpBcmRxCrcErrors = 111
     * snmpBcmRxFecCorrectable = 112
     * snmpBcmRxBecCrcErrors = 113
     * snmpBcmRxDisparityErrors = 114
     * snmpBcmRxControlCells = 115
     * snmpBcmRxDataCells = 116
     * snmpBcmRxDataBytes = 117
     * snmpBcmRxDroppedRetransmittedControl = 118
     * snmpBcmTxBecRetransmit = 119
     * snmpBcmRxBecRetransmit = 120
     * snmpBcmTxAsynFifoRate = 121
     * snmpBcmRxAsynFifoRate = 122
     * snmpBcmRxFecUncorrectable = 123
     * snmpBcmRxBecRxFault = 124
     * snmpBcmRxCodeErrors = 125
     * snmpBcmRxRsFecBitError = 126
     * snmpBcmRxRsFecSymbolError = 127
     * snmpBcmRxLlfcPrimary = 128
     * snmpBcmRxLlfcSecondary = 129
     * snmpBcmCustomReceive0 = 130
     * snmpBcmCustomReceive1 = 131
     * snmpBcmCustomReceive2 = 132
     * snmpBcmCustomReceive3 = 133
     * snmpBcmCustomReceive4 = 134
     * snmpBcmCustomReceive5 = 135
     * snmpBcmCustomReceive6 = 136
     * snmpBcmCustomReceive7 = 137
     * snmpBcmCustomReceive8 = 138
     * snmpBcmCustomTransmit0 = 139
     * snmpBcmCustomTransmit1 = 140
     * snmpBcmCustomTransmit2 = 141
     * snmpBcmCustomTransmit3 = 142
     * snmpBcmCustomTransmit4 = 143
     * snmpBcmCustomTransmit5 = 144
     * snmpBcmCustomTransmit6 = 145
     * snmpBcmCustomTransmit7 = 146
     * snmpBcmCustomTransmit8 = 147
     * snmpBcmCustomTransmit9 = 148
     * snmpBcmCustomTransmit10 = 149
     * snmpBcmCustomTransmit11 = 150
     * snmpBcmCustomTransmit12 = 151
     * snmpBcmCustomTransmit13 = 152
     * snmpBcmCustomTransmit14 = 153
 */
    /* snmpBcmEtherStatsPkts1519to1522Octets = 86 */
    {snmpBcmEtherStatsPkts1519to1522Octets, CTR_MACs, RX_VLAN_PKTs, FALSE},
    {snmpBcmEtherStatsPkts1519to1522Octets, CTR_MACs, TX_VLAN_PKTs, FALSE},
    /* snmpBcmEtherStatsPkts1522to2047Octets = 87 */
    {snmpBcmEtherStatsPkts1522to2047Octets, CTR_MACs, RX_2047B_PKTs, FALSE},
    {snmpBcmEtherStatsPkts1522to2047Octets, CTR_MACs, RX_VLAN_PKTs, TRUE},
    {snmpBcmEtherStatsPkts1522to2047Octets, CTR_MACs, TX_2047B_PKTs, FALSE},
    {snmpBcmEtherStatsPkts1522to2047Octets, CTR_MACs, TX_VLAN_PKTs, TRUE},
    /* snmpBcmEtherStatsPkts2048to4095Octets = 88 */
    {snmpBcmEtherStatsPkts2048to4095Octets, CTR_MACs, RX_4095B_PKTs, FALSE},
    {snmpBcmEtherStatsPkts2048to4095Octets, CTR_MACs, TX_4095B_PKTs, FALSE},
    /* snmpBcmEtherStatsPkts4095to9216Octets = 89 */
    {snmpBcmEtherStatsPkts4095to9216Octets, CTR_MACs, RX_9216B_PKTs, FALSE},
    {snmpBcmEtherStatsPkts4095to9216Octets, CTR_MACs, TX_9216B_PKTs, FALSE},
    /* snmpBcmReceivedPkts64Octets = 90 */
    {snmpBcmReceivedPkts64Octets, CTR_MACs, RX_64B_PKTs, FALSE},
    /* snmpBcmReceivedPkts65to127Octets = 91 */
    {snmpBcmReceivedPkts65to127Octets, CTR_MACs, RX_127B_PKTs, FALSE},
    /* snmpBcmReceivedPkts128to255Octets = 92 */
    {snmpBcmReceivedPkts128to255Octets, CTR_MACs, RX_255B_PKTs, FALSE},
    /* snmpBcmReceivedPkts256to511Octets = 93 */
    {snmpBcmReceivedPkts256to511Octets, CTR_MACs, RX_511B_PKTs, FALSE},
    /* snmpBcmReceivedPkts512to1023Octets = 94 */
    {snmpBcmReceivedPkts512to1023Octets, CTR_MACs, RX_1023B_PKTs, FALSE},
    /* snmpBcmReceivedPkts1024to1518Octets = 95 */
    {snmpBcmReceivedPkts1024to1518Octets, CTR_MACs, RX_1518B_PKTs, FALSE},
    /* snmpBcmReceivedPkts1519to2047Octets = 96 */
    {snmpBcmReceivedPkts1519to2047Octets, CTR_MACs, RX_2047B_PKTs, FALSE},
    /* snmpBcmReceivedPkts2048to4095Octets = 97 */
    {snmpBcmReceivedPkts2048to4095Octets, CTR_MACs, RX_4095B_PKTs, FALSE},
    /* snmpBcmReceivedPkts4095to9216Octets = 98 */
    {snmpBcmReceivedPkts4095to9216Octets, CTR_MACs, RX_9216B_PKTs, FALSE},
    /* snmpBcmTransmittedPkts64Octets = 99 */
    {snmpBcmTransmittedPkts64Octets, CTR_MACs, TX_64B_PKTs, FALSE},
    /* snmpBcmTransmittedPkts65to127Octets = 100 */
    {snmpBcmTransmittedPkts65to127Octets, CTR_MACs, TX_127B_PKTs, FALSE},
    /* snmpBcmTransmittedPkts128to255Octets = 101 */
    {snmpBcmTransmittedPkts128to255Octets, CTR_MACs, TX_255B_PKTs, FALSE},
    /* snmpBcmTransmittedPkts256to511Octets = 102 */
    {snmpBcmTransmittedPkts256to511Octets, CTR_MACs, TX_511B_PKTs, FALSE},
    /* snmpBcmTransmittedPkts512to1023Octets = 103 */
    {snmpBcmTransmittedPkts512to1023Octets, CTR_MACs, TX_1023B_PKTs, FALSE},
    /* snmpBcmTransmittedPkts1024to1518Octets = 104 */
    {snmpBcmTransmittedPkts1024to1518Octets, CTR_MACs, TX_1518B_PKTs, FALSE},
    /* snmpBcmTransmittedPkts1519to2047Octets = 105 */
    {snmpBcmTransmittedPkts1519to2047Octets, CTR_MACs, TX_2047B_PKTs, FALSE},
    /* snmpBcmTransmittedPkts2048to4095Octets = 106 */
    {snmpBcmTransmittedPkts2048to4095Octets, CTR_MACs, TX_4095B_PKTs, FALSE},
    /* snmpBcmTransmittedPkts4095to9216Octets = 107 */
    {snmpBcmTransmittedPkts4095to9216Octets, CTR_MACs, TX_9216B_PKTs, FALSE},

    /*
     * RFC 4837 MIBs
     *
     * The following RFC 4837 MIBs are not supported:
     *
     * snmpDot3StatsInRangeLengthError = 154
     * snmpDot3OmpEmulationCRC8Errors = 155
     * snmpDot3MpcpRxGate = 156
     * snmpDot3MpcpRxRegister = 157
     * snmpDot3MpcpTxRegRequest = 158
     * snmpDot3MpcpTxRegAck = 159
     * snmpDot3MpcpTxReport = 160
     * snmpDot3EponFecCorrectedBlocks = 161
     * snmpDot3EponFecUncorrectableBlocks = 162
 */

    /*
     * Broadcom-specific MIBs
     *
     * The following Broadcom-specific MIBs are not supported:
     *
     * snmpBcmPonInDroppedOctets = 163
     * snmpBcmPonOutDroppedOctets = 164
     * snmpBcmPonInDelayedOctets = 165
     * snmpBcmPonOutDelayedOctets = 166
     * snmpBcmPonInDelayedHundredUs = 167
     * snmpBcmPonOutDelayedHundredUs = 168
     * snmpBcmPonInFrameErrors = 169
     * snmpBcmPonInOamFrames = 170
     * snmpBcmPonOutOamFrames = 171
     * snmpBcmPonOutUnusedOctets = 172
     */
    /* snmpBcmEtherStatsPkts9217to16383Octets =173 */
    {snmpBcmEtherStatsPkts9217to16383Octets, CTR_MACs, RX_16383B_PKTs, FALSE},
    {snmpBcmEtherStatsPkts9217to16383Octets, CTR_MACs, TX_16383B_PKTs, FALSE},
    /* snmpBcmReceivedPkts9217to16383Octets = 174 */
    {snmpBcmReceivedPkts9217to16383Octets, CTR_MACs, RX_16383B_PKTs, FALSE},
    /* snmpBcmTransmittedPkts9217to16383Octets = 175 */
    {snmpBcmTransmittedPkts9217to16383Octets, CTR_MACs, TX_16383B_PKTs, FALSE},
    /* snmpBcmRxVlanTagFrame = 176 */
    {snmpBcmRxVlanTagFrame, CTR_MACs, RX_VLAN_TAG_PKTs, FALSE},
    /* snmpBcmRxDoubleVlanTagFrame = 177 */
    {snmpBcmRxDoubleVlanTagFrame, CTR_MACs, RX_DOUBLE_VLAN_PKTs, FALSE},
    /* snmpBcmTxVlanTagFrame = 178 */
    {snmpBcmTxVlanTagFrame, CTR_MACs, TX_VLAN_TAG_PKTs, FALSE},
    /* snmpBcmTxDoubleVlanTagFrame = 179 */
    {snmpBcmTxDoubleVlanTagFrame, CTR_MACs, TX_DOUBLE_VLAN_PKTs, FALSE},

    /*
     * PFC MIBs
 */
    /* snmpBcmRxPFCControlFrame = 180 */
    {snmpBcmRxPFCControlFrame, CTR_MACs, RX_PER_PRI_PAUSE_CTL_PKTs, FALSE},
    /* snmpBcmTxPFCControlFrame = 181 */
    {snmpBcmTxPFCControlFrame, CTR_MACs, TX_PER_PRI_PAUSE_CTL_PKTs, FALSE},
    /* snmpBcmRxPFCFrameXonPriority0 = 182*/
    {snmpBcmRxPFCFrameXonPriority0, CTR_MACs, RX_PFC_OFF_PKT_PRI0s, FALSE},
    /* snmpBcmRxPFCFrameXonPriority1 = 183*/
    {snmpBcmRxPFCFrameXonPriority1, CTR_MACs, RX_PFC_OFF_PKT_PRI1s, FALSE},
    /* snmpBcmRxPFCFrameXonPriority2 = 184 */
    {snmpBcmRxPFCFrameXonPriority2, CTR_MACs, RX_PFC_OFF_PKT_PRI2s, FALSE},
    /* snmpBcmRxPFCFrameXonPriority3 = 185*/
    {snmpBcmRxPFCFrameXonPriority3, CTR_MACs, RX_PFC_OFF_PKT_PRI3s, FALSE},
    /* snmpBcmRxPFCFrameXonPriority4 = 186*/
    {snmpBcmRxPFCFrameXonPriority4, CTR_MACs, RX_PFC_OFF_PKT_PRI4s, FALSE},
    /* snmpBcmRxPFCFrameXonPriority5 = 187*/
    {snmpBcmRxPFCFrameXonPriority5, CTR_MACs, RX_PFC_OFF_PKT_PRI5s, FALSE},
    /* snmpBcmRxPFCFrameXonPriority6 = 188*/
    {snmpBcmRxPFCFrameXonPriority6, CTR_MACs, RX_PFC_OFF_PKT_PRI6s, FALSE},
    /* snmpBcmRxPFCFrameXonPriority7 = 189*/
    {snmpBcmRxPFCFrameXonPriority7, CTR_MACs, RX_PFC_OFF_PKT_PRI7s, FALSE},
    /* snmpBcmRxPFCFramePriority0 = 190 */
    {snmpBcmRxPFCFramePriority0, CTR_MACs, RX_PFC_PKT_PRI0s, FALSE},
    /* snmpBcmRxPFCFramePriority1 = 191 */
    {snmpBcmRxPFCFramePriority1, CTR_MACs, RX_PFC_PKT_PRI1s, FALSE},
    /* snmpBcmRxPFCFramePriority2 = 192 */
    {snmpBcmRxPFCFramePriority2, CTR_MACs, RX_PFC_PKT_PRI2s, FALSE},
    /* snmpBcmRxPFCFramePriority3 = 193 */
    {snmpBcmRxPFCFramePriority3, CTR_MACs, RX_PFC_PKT_PRI3s, FALSE},
    /* snmpBcmRxPFCFramePriority4 = 194 */
    {snmpBcmRxPFCFramePriority4, CTR_MACs, RX_PFC_PKT_PRI4s, FALSE},
    /* snmpBcmRxPFCFramePriority5 = 195 */
    {snmpBcmRxPFCFramePriority5, CTR_MACs, RX_PFC_PKT_PRI5s, FALSE},
    /* snmpBcmRxPFCFramePriority6 = 196 */
    {snmpBcmRxPFCFramePriority6, CTR_MACs, RX_PFC_PKT_PRI6s, FALSE},
    /* snmpBcmRxPFCFramePriority7 = 197 */
    {snmpBcmRxPFCFramePriority7, CTR_MACs, RX_PFC_PKT_PRI7s, FALSE},
    /* snmpBcmTxPFCFramePriority0 = 198 */
    {snmpBcmTxPFCFramePriority0, CTR_MACs, TX_PFC_PKT_PRI0s, FALSE},
    /* snmpBcmTxPFCFramePriority1 = 199 */
    {snmpBcmTxPFCFramePriority1, CTR_MACs, TX_PFC_PKT_PRI1s, FALSE},
    /* snmpBcmTxPFCFramePriority2 = 200 */
    {snmpBcmTxPFCFramePriority2, CTR_MACs, TX_PFC_PKT_PRI2s, FALSE},
    /* snmpBcmTxPFCFramePriority3 = 201 */
    {snmpBcmTxPFCFramePriority3, CTR_MACs, TX_PFC_PKT_PRI3s, FALSE},
    /* snmpBcmTxPFCFramePriority4 = 202 */
    {snmpBcmTxPFCFramePriority4, CTR_MACs, TX_PFC_PKT_PRI4s, FALSE},
    /* snmpBcmTxPFCFramePriority5 = 203 */
    {snmpBcmTxPFCFramePriority5, CTR_MACs, TX_PFC_PKT_PRI5s, FALSE},
    /* snmpBcmTxPFCFramePriority6 = 204 */
    {snmpBcmTxPFCFramePriority6, CTR_MACs, TX_PFC_PKT_PRI6s, FALSE},
    /* snmpBcmTxPFCFramePriority7 = 205 */
    {snmpBcmTxPFCFramePriority7, CTR_MACs, TX_PFC_PKT_PRI7s, FALSE},

    /*
     * RFC 4044 MIBs
     *
     * The following RFC 4044 MIBs are not supported:
     *
     * snmpFcmPortClass3RxFrames = 206
     * snmpFcmPortClass3TxFrames = 207
     * snmpFcmPortClass3Discards = 208
     * snmpFcmPortClass2RxFrames = 209
     * snmpFcmPortClass2TxFrames = 210
     * snmpFcmPortClass2Discards = 211
     * snmpFcmPortInvalidCRCs = 212
     * snmpFcmPortDelimiterErrors = 213
 */

    /*
     * Broadcom-specific MIBs
     *
     * The following Broadcom-specific MIBs are not supported:
     *
     * snmpBcmSampleIngressPkts = 214
     * snmpBcmSampleIngressSnapshotPkts = 215
     * snmpBcmSampleIngressSampledPkts = 216
     * snmpBcmSampleFlexPkts = 217
     * snmpBcmSampleFlexSnapshotPkts = 218
     * snmpBcmSampleFlexSampledPkts = 219
     * snmpBcmEgressProtectionDataDrop = 220
     * snmpBcmE2EHOLDropPkts = 222
     */
    /* snmpBcmTxE2ECCControlFrames = 221 */
    {snmpBcmTxE2ECCControlFrames, CTR_MAC_ERRs, TX_HOL_BLOCK_PKTs, FALSE},

    /*
     * RFC 1757 MIBs
     *
     * The following RFC 1757 MIBs are not supported:
     *
     * snmpEtherStatsTxCRCAlignErrors = 223
     * snmpEtherStatsTxJabbers = 224
 */

    /*
     * Broadcom-specific MIBs
     *
     * The following Broadcom-specific MIBs are not supported:
     *
     * snmpBcmMacMergeTxFrag = 225
     * snmpBcmMacMergeTxVerifyFrame = 226
     * snmpBcmMacMergeTxReplyFrame = 227
     * snmpBcmMacMergeRxFrameAssErrors = 228
     * snmpBcmMacMergeRxFrameSmdErrors = 229
     * snmpBcmMacMergeRxFrameAss = 230
     * snmpBcmMacMergeRxFrag = 231
     * snmpBcmMacMergeRxVerifyFrame = 232
     * snmpBcmMacMergeRxReplyFrame = 233
     * snmpBcmMacMergeRxFinalFragSizeError = 234
     * snmpBcmMacMergeRxFragSizeError = 235
     * snmpBcmMacMergeRxDiscard = 236
     * snmpBcmMacMergeHoldCount = 237
     * snmpBcmRxBipErrorCount = 238
     * snmpBcmTxEcnErrors = 239
 */
};

/*!
 * \brief PC_PORT LT notify info.
 */
typedef struct port_notify_s {
    /*! Operation code. */
    bcmlt_opcode_t opcode;

    /*! PORT_ID key field. */
    int port_id;

} port_notify_t;

/******************************************************************************
 * Private functions
 */

static int stat_inited[BCM_MAX_NUM_UNITS];

static int stat_mac_index[BCM_MAX_NUM_UNITS][snmpValCount];

/*!
 * \brief Initializes the common MAC counters mapping table.
 *
 * \param [in] unit Unit Number.
 */
static void
ltsw_stat_mac_index_init(int unit)
{
    int index;
    bcm_stat_val_t stat_id;

    memset(stat_mac_index[unit], -1, sizeof(stat_mac_index[unit]));

    index = 0;
    while (index < COUNTOF(ltsw_stat_mac_ctr_map_tab)) {
        stat_id = ltsw_stat_mac_ctr_map_tab[index].type;
        if (stat_mac_index[unit][stat_id] == -1) {
            stat_mac_index[unit][stat_id] = index;
        }
        index++;
    }
}

/*!
 * \brief De-initializes the stat LT entries.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_stat_lt_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Remove CTR_MAC LT entries. */
    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_MACs));

    /* Remove CTR_MAC_ERR LT entries. */
    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_MAC_ERRs));

    /* De-initializes the stat LT entries for chip-specific. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_lt_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initializes the MAC counters LT with specified logical table name.
 *
 * \param [in] unit Unit Number.
 * \param [in] table_name Logical table name.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_stat_mac_counter_init(
    int unit,
    const char *table_name)
{
    int port;
    uint32_t port_type;
    bcm_pbmp_t pbmp;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {NULL, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    port_type = BCMI_LTSW_PORT_TYPE_PORT | BCMI_LTSW_PORT_TYPE_MGMT;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, port_type, &pbmp));
    if (BCM_PBMP_IS_NULL(pbmp)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Port Bitmap is empty!\n")));
        SHR_EXIT();
    }

    fields[0].fld_name = PORT_IDs;
    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    BCM_PBMP_ITER(pbmp, port) {
        fields[0].u.val = port;
        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_insert(unit, table_name, &lt_entry, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the stat sync state.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_stat_sync_state_get(int unit, const char **state)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, CTR_EVENT_SYNC_STATEs, &entry_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, CTR_EVENT_SYNC_STATE_IDs, ALLs));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, STATEs, state));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief stat sync on.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_stat_sync_on(int unit)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    shr_timeout_t timeout;
    const char *state = NULL;
    bool is_sim = SAL_BOOT_BCMSIM;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, CTR_EVENT_SYNC_STATE_CONTROLs, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, CTR_EVENT_SYNC_STATE_CONTROL_IDs,
                                      ALLs));

    /* Trigger stat sync. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, STATEs, STARTs));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

    /* Wait for stat sync complete. */
    if (is_sim) {
        shr_timeout_init(&timeout, STAT_SYNC_TIMEOUT_USEC_SIM, 0);
    } else {
        shr_timeout_init(&timeout, STAT_SYNC_TIMEOUT_USEC, 0);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_sync_state_get(unit, &state));
    while (sal_strcmp(state, COMPLETEs) != 0) {
        if (shr_timeout_check(&timeout)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Sate sync is not completed!\n")));
            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_stat_sync_state_get(unit, &state));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief stat sync off.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_stat_sync_off(int unit)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    shr_timeout_t timeout;
    const char *state = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, CTR_EVENT_SYNC_STATE_CONTROLs, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, CTR_EVENT_SYNC_STATE_CONTROL_IDs,
                                      ALLs));

    /* Turn off stat sync. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, STATEs, OFFs));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

    /* Wait for stat sync off complete. */
    shr_timeout_init(&timeout, 100000, 0);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_sync_state_get(unit, &state));
    while (sal_strcmp(state, OFFs) != 0) {
        if (shr_timeout_check(&timeout)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Sate sync is not OFF!\n")));
            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_stat_sync_state_get(unit, &state));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/disable the port based counter collection.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] enable TRUE: enable, FALSE: disable.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_stat_counter_collection_enable(
    int unit,
    int port,
    bool enable)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_field_def_t field_def;
    uint64_t port_id[BCM_PBMP_PORT_MAX] = {0};
    uint32_t num_array;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, CTR_CONTROLs, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, CTR_CONTROLs, PORT_IDs, &field_def));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get(entry_hdl, PORT_IDs, 0, port_id,
                                     field_def.elements, &num_array));
    if ((enable ^ port_id[port]) == 0) {
        /* Already enabled, don't need to update. */
        SHR_EXIT();
    }
    port_id[port] = enable;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, PORT_IDs, 0, port_id,
                                     field_def.elements));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initializes the stat index for counter mapping table.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_stat_index_init(int unit)
{
    const bcmint_stat_dev_info_t *dev_info;

    SHR_FUNC_ENTER(unit);

    ltsw_stat_mac_index_init(unit);

    /* Get the chip specific device information for counter mapping tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_dev_info_get(unit, &dev_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_index_init(unit, dev_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initializes the stat module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_stat_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Insert CTR_MAC LT entries. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_mac_counter_init(unit, CTR_MACs));

    /* Insert CTR_MAC_ERR LT entries. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_mac_counter_init(unit, CTR_MAC_ERRs));

    /* Initializing the stat for chip-specific. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the specified statistic from the port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] sync_mode Sync mode..
 * \param [in] stat_id SNMP statistic type.
 * \param [out] val 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_stat_get(
    int unit,
    bcm_port_t port,
    bool sync_mode,
    bcm_stat_val_t stat_id,
    uint64_t *val)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    uint64_t val64;
    uint32_t val32;
    int index;
    const char *table_name;
    const char *field_name;
    uint32_t entry_attr = 0;
    bcmpkt_dev_stat_t stats;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(val, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (stat_id < 0 || stat_id >= snmpValCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (stat_id == snmpIfOutQLen) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_queued_count_get(unit, port, &val32));
        *val = 0;
        *val += val32;
        SHR_EXIT();
    }

    if (bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_CPU)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_dev_stats_get(dunit, &stats));
        switch (stat_id) {
            case snmpIfInUcastPkts:
                *val = stats.rx_packets;
                break;
            case snmpIfOutUcastPkts:
                *val = stats.tx_packets;
                break;
            case snmpIfInDiscards:
                *val = stats.rx_dropped;
                break;
            case snmpIfOutDiscards:
                *val = stats.tx_dropped;
                break;
            default:
                *val = 0;
                break;
        }
        SHR_EXIT();
    }

    /* Check chip-specific counters if it is not common MAC counters */
    if (stat_mac_index[unit][stat_id] < 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_stat_get(unit, port, sync_mode, stat_id, val));
    } else {
        *val = 0;

        for (index = stat_mac_index[unit][stat_id];
             index < COUNTOF(ltsw_stat_mac_ctr_map_tab); index++) {
            if (ltsw_stat_mac_ctr_map_tab[index].type != stat_id) {
                break;
            }
            table_name = ltsw_stat_mac_ctr_map_tab[index].table_name;
            field_name = ltsw_stat_mac_ctr_map_tab[index].field_name;
            entry_attr = sync_mode ? BCMLT_ENT_ATTR_GET_FROM_HW : 0;
            val64 = 0;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_stat_lt_counter_get(unit, table_name, field_name, port,
                                            entry_attr, &val64));

            if (ltsw_stat_mac_ctr_map_tab[index].minus) {
                *val -= val64;
            } else {
                *val += val64;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get multiple statistics from the port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] sync_mode Sync mode.
 * \param [in] nstat Number of elements in stat array.
 * \param [in] stat_arr Array of SNMP statistic types.
 * \param [out] value_arr 64-bit collected counter values.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_stat_multi_get(
    int unit,
    bcm_port_t port,
    bool sync_mode,
    int nstat,
    bcm_stat_val_t *stat_arr,
    uint64_t *value_arr)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_info_t entry_info;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    const char *table_name;
    const char *field_name;
    int index = 0;
    int i = 0;
    uint32_t entry_attr = 0;
    uint64_t field_val = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_CPU)) {
        for (i = 0; i < nstat; i++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_stat_get(unit, port, sync_mode, stat_arr[i],
                                  &(value_arr[i])));
        }
        SHR_EXIT();
    }

    entry_attr = sync_mode ? BCMLT_ENT_ATTR_GET_FROM_HW : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, CTR_MACs, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_attrib_set(entry_hdl, entry_attr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_LOOKUP, entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, CTR_MAC_ERRs, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_attrib_set(entry_hdl, entry_attr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_LOOKUP, entry_hdl));
    entry_hdl = BCMLT_INVALID_HDL;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL));

    for (i = 0; i < nstat; i++) {
        if (stat_arr[i] < 0 || stat_arr[i] >= snmpValCount) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (stat_arr[i] == snmpIfOutQLen) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_stat_get(unit, port, sync_mode, stat_arr[i],
                               &value_arr[i]));
        } else if (stat_mac_index[unit][stat_arr[i]] < 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_stat_get(unit, port, sync_mode, stat_arr[i],
                                    &value_arr[i]));
        } else {
            value_arr[i] = 0;
            for (index = stat_mac_index[unit][stat_arr[i]];
                 index < COUNTOF(ltsw_stat_mac_ctr_map_tab); index++) {
                if (ltsw_stat_mac_ctr_map_tab[index].type != stat_arr[i]) {
                    break;
                }

                table_name = ltsw_stat_mac_ctr_map_tab[index].table_name;
                field_name = ltsw_stat_mac_ctr_map_tab[index].field_name;
                if (sal_strcmp(table_name, CTR_MACs) == 0) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_transaction_entry_num_get(trans_hdl, 0, &entry_info));
                } else if (sal_strcmp(table_name, CTR_MAC_ERRs) == 0) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_transaction_entry_num_get(trans_hdl, 1, &entry_info));
                } else {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                entry_hdl = entry_info.entry_hdl;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(entry_hdl, field_name, &field_val));

                if (ltsw_stat_mac_ctr_map_tab[index].minus) {
                    value_arr[i] -= field_val;
                } else {
                    value_arr[i] += field_val;
                }
            }
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Clear the port based counters with specified logical table name.
 *
 * \param [in] unit Unit Number.
 * \param [in] table_name Logical table name.
 * \param [in] port Port Number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_stat_lt_counter_clear(
    int unit,
    const char *table_name,
    int port)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_opcode_t opcode_update = BCMLT_OPCODE_UPDATE;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_field_def_t *table_fields = NULL;
    uint32_t i;
    uint32_t num_fields;
    const char *field_name;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(table_name, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_table_field_defs_get(dunit, table_name, 0, NULL, &num_fields));

    SHR_ALLOC(table_fields, sizeof(bcmlt_field_def_t) * num_fields,
              "ltswStatDefFields");
    SHR_NULL_CHECK(table_fields, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_table_field_defs_get(dunit, table_name, num_fields, table_fields,
                                    &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));

    for (i = 0; i < num_fields; i++) {
        if (table_fields[i].symbol || table_fields[i].key) {
            continue;
        }
        field_name = (char *)table_fields[i].name;
        /* Set field to 0 */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, field_name, 0));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(entry_hdl, opcode_update, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FREE(table_fields);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert the port based counters with specified logical table name.
 *
 * \param [in] unit Unit Number.
 * \param [in] table_name Logical table name.
 * \param [in] port Port Number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_stat_lt_counter_insert(
    int unit,
    const char *table_name,
    bcm_port_t port)
{
    int rv = SHR_E_NONE;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(table_name, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));
    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_EXISTS) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete the port based counters with specified logical table name.
 *
 * \param [in] unit Unit Number.
 * \param [in] table_name Logical table name.
 * \param [in] port Port Number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_stat_lt_counter_delete(
    int unit,
    const char *table_name,
    bcm_port_t port)
{
    int rv = SHR_E_NONE;
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(table_name, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));
    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the port based counters with specified logical table name and field name.
 *
 * \param [in] unit Unit Number.
 * \param [in] table_name Logical table name.
 * \param [in] field_name Logical field name.
 * \param [in] port Port Number.
 * \param [in] entry_attr Sync mode.
 * \param [out] field_val 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_stat_lt_counter_get(
    int unit,
    const char *table_name,
    const char *field_name,
    int port,
    uint32_t entry_attr,
    uint64_t *field_val)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_opcode_t opcode_lookup = BCMLT_OPCODE_LOOKUP;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(table_name, SHR_E_PARAM);
    SHR_NULL_CHECK(field_name, SHR_E_PARAM);
    SHR_NULL_CHECK(field_val, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_attrib_set(entry_hdl, entry_attr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(entry_hdl, opcode_lookup, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, field_name, field_val));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initializes the stat module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_init(int unit)
{
    bcm_pbmp_t pbmp;
    int port;
    uint32_t port_type;

    SHR_FUNC_ENTER(unit);

    if (!stat_inited[unit]) {
        SHR_IF_ERR_EXIT
            (ltsw_stat_index_init(unit));
    }

    if (stat_sync_mutex[unit] == NULL) {
        stat_sync_mutex[unit] = sal_mutex_create("bcmLtswStatSyncMutex");
        SHR_NULL_CHECK(stat_sync_mutex[unit], SHR_E_MEMORY);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_stat_counter_control_init(unit));

    if (bcmi_warmboot_get(unit)) {
        stat_inited[unit] = TRUE;
        SHR_EXIT();
    }

    port_type = BCMI_LTSW_PORT_TYPE_PORT | BCMI_LTSW_PORT_TYPE_MGMT |
                BCMI_LTSW_PORT_TYPE_CPU;
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_port_bitmap(unit, port_type, &pbmp));

    /* Disable counter collection for all front pannel ports. */
    BCM_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (ltsw_stat_counter_collection_enable(unit, port, FALSE));
    }

    SHR_IF_ERR_EXIT
        (ltsw_stat_lt_detach(unit));

    /* Initializing the stat module. */
    SHR_IF_ERR_EXIT
        (ltsw_stat_init(unit));

    /* Enable counter collection for all front pannel ports. */
    BCM_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (ltsw_stat_counter_collection_enable(unit, port, TRUE));
    }

    stat_inited[unit] = TRUE;

exit:
    if (SHR_FUNC_ERR()) {
        if (stat_sync_mutex[unit] != NULL) {
            sal_mutex_destroy(stat_sync_mutex[unit]);
            stat_sync_mutex[unit] = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initializes the stat module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_stat_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_EXIT();
    }

    if (stat_sync_mutex[unit] != NULL) {
        sal_mutex_destroy(stat_sync_mutex[unit]);
        stat_sync_mutex[unit] = NULL;
    }
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_stat_detach(unit));

    stat_inited[unit] = FALSE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initializes the statistic on port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Logical port number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_stat_port_attach(int unit, int port)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Disable counter collection on port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_counter_collection_enable(unit, port, FALSE));

    /* Insert CTR_MAC LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_stat_lt_counter_insert(unit, CTR_MACs, port));
    /* Insert CTR_MAC_ERR LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_stat_lt_counter_insert(unit, CTR_MAC_ERRs, port));

    /* Insert the stat LT entry for chip-specific. */
    rv = mbcm_ltsw_stat_port_attach(unit, port);
    if (rv != SHR_E_UNAVAIL) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Enable counter collection on port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_counter_collection_enable(unit, port, TRUE));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initializes the statistic on port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Logical port number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_stat_port_detach(int unit, int port)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Disable counter collection on port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_counter_collection_enable(unit, port, FALSE));

    /* Delete CTR_MAC LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_stat_lt_counter_delete(unit, CTR_MACs, port));
    /* Delete CTR_MAC_ERR LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_stat_lt_counter_delete(unit, CTR_MAC_ERRs, port));

    /* Delete the stat LT entry for chip-specific. */
    rv = mbcm_ltsw_stat_port_detach(unit, port);
    if (rv != SHR_E_UNAVAIL) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the port based statistics.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_clear(int unit, bcm_port_t port)
{
    int rv = SHR_E_NONE;
    int dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_stat_clear(unit, port));

    if (bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_PORT) ||
        bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_MGMT)) {
        /* Clear CTR_MAC LT entry fields. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_stat_lt_counter_clear(unit, CTR_MACs, port));
        /* Clear CTR_MAC_ERR LT entry fields. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_stat_lt_counter_clear(unit, CTR_MAC_ERRs, port));

        /* Clear the stat LT entry fields for chip-specific. */
        rv = mbcm_ltsw_stat_clear(unit, port);
        if (rv != SHR_E_UNAVAIL) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    } else if (bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_CPU)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpkt_dev_stats_clear(dunit));
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Invalid port number(%d).\n"), port));
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the specific statistic from the port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] stat_id SNMP statistic type.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_clear_single(int unit, bcm_port_t port,
                           bcm_stat_val_t stat_id)
{
    int index;
    const char *table_name;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] = {
        {PORT_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        {NULL, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (stat_id < 0 || stat_id >= snmpValCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!(bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_PORT) ||
          bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_MGMT))) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Invalid port number(%d).\n"), port));
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    /* Check chip-specific counters if it is not common MAC counters */
    if (stat_mac_index[unit][stat_id] < 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_stat_clear_single(unit, port, stat_id));
    } else {
        fields[0].u.val = port;
        lt_entry.fields = fields;
        lt_entry.nfields = COUNTOF(fields);
        lt_entry.attr = 0;

        for (index = stat_mac_index[unit][stat_id];
             index < COUNTOF(ltsw_stat_mac_ctr_map_tab); index++) {
            if (ltsw_stat_mac_ctr_map_tab[index].type != stat_id) {
                break;
            }
            table_name = ltsw_stat_mac_ctr_map_tab[index].table_name;
            fields[1].fld_name = ltsw_stat_mac_ctr_map_tab[index].field_name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_update(unit, table_name, &lt_entry, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Synchronize software counters with hardware.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_sync(int unit)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bool locked = FALSE;
    const char *state = NULL;

    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (sal_mutex_take(stat_sync_mutex[unit], SAL_MUTEX_FOREVER));
    locked = TRUE;

    /* Get the state sync state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_sync_state_get(unit, &state));
    if (sal_strcmp(state, OFFs) != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Wrong initial sync state: %s!\n"),
                     state));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Get the state sync control state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, CTR_EVENT_SYNC_STATE_CONTROLs, &entry_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, CTR_EVENT_SYNC_STATE_CONTROL_IDs,
                                      ALLs));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, STATEs, &state));
    if (sal_strcmp(state, OFFs) != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Wrong initial sync control state: %s!\n"),
                     state));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Trigger stat sync. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_sync_on(unit));

    /* Turn off stat sync. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_sync_off(unit));

exit:
    if (SHR_FUNC_ERR()) {
        (void)ltsw_stat_sync_off(unit);
    }
    if (locked) {
        sal_mutex_give(stat_sync_mutex[unit]);
    }
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the specified statistic from the port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] type SNMP statistic type.
 * \param [out] val 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_get(int unit, bcm_port_t port, bcm_stat_val_t type,
                  uint64_t *val)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_get(unit, port, FALSE, type, val));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the specified statistics from the port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] nstat Number of elements in stat array.
 * \param [in] stat_arr Array of SNMP statistic types.
 * \param [out] value_arr 64-bit collected counter values.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_multi_get(int unit, bcm_port_t port, int nstat,
                        bcm_stat_val_t *stat_arr, uint64_t *value_arr)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (nstat <= 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(stat_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(value_arr, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_multi_get(unit, port, FALSE, nstat, stat_arr, value_arr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the specified statistic from the port with sync mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] type SNMP statistic type.
 * \param [out] val 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_sync_get(int unit, bcm_port_t port, bcm_stat_val_t type,
                       uint64_t *val)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_get(unit, port, TRUE, type, val));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the specified statistics from the port with sync mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] nstat Number of elements in stat array.
 * \param [in] stat_arr Array of SNMP statistic types.
 * \param [out] value_arr 64-bit collected counter values.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_sync_multi_get(int unit, bcm_port_t port, int nstat,
                             bcm_stat_val_t *stat_arr, uint64_t *value_arr)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (nstat <= 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(stat_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(value_arr, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_stat_multi_get(unit, port, TRUE, nstat, stat_arr, value_arr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the dedicated counter associated with a given drop event.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Drop event type.
 * \param [out] val 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_drop_event_counter_get(int unit,
                                     bcm_pkt_drop_event_t type,
                                     uint64_t *val)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_drop_event_counter_get(unit, FALSE, type, val));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the dedicated counter associated with a given drop event with sync mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Drop event type.
 * \param [out] val 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_drop_event_counter_sync_get(int unit,
                                          bcm_pkt_drop_event_t type,
                                          uint64_t *val)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_drop_event_counter_get(unit, TRUE, type, val));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the dedicated counter associated with a given drop event.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Drop event type.
 * \param [in] val 64-bit counter value. Set val to 0 to clear the counter.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_drop_event_counter_set(int unit,
                                     bcm_pkt_drop_event_t type,
                                     uint64_t val)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_drop_event_counter_set(unit, type, val));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the dedicated counter associated witch a given trace event.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Trace event type.
 * \param [out] val 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_trace_event_counter_get(int unit,
                                      bcm_pkt_trace_event_t type,
                                      uint64_t *val)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_trace_event_counter_get(unit, FALSE, type, val));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the dedicated counter associated with a given trace event with sync mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Trace event type.
 * \param [out] val 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_trace_event_counter_sync_get(int unit,
                                           bcm_pkt_trace_event_t type,
                                           uint64_t *val)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_trace_event_counter_get(unit, TRUE, type, val));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the dedicated counter associated with a given trace event.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Trace event type.
 * \param [in] val 64-bit counter value. Set val to 0 to clear the counter.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_trace_event_counter_set(int unit,
                                      bcm_pkt_trace_event_t type,
                                      uint64_t val)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_trace_event_counter_set(unit, type, val));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the counter control information structure.
 *
 * \param [in] unit Unit Number.
 * \param [out] info Counter control information structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_stat_counter_control_get(int unit,
                                   bcmi_ltsw_stat_counter_control_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_counter_control_get(unit, info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a certain packet type to debug counter to count.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number (unused).
 * \param [in] type SNMP statistics type.
 * \param [in] trigger The counter select value
 *                     (see include/bcm/stat.h for bit definitions).
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_custom_add(int unit, bcm_port_t port, bcm_stat_val_t type,
                         bcm_custom_stat_trigger_t trigger)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_custom_add(unit, port, type, trigger));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if certain packet types is part of debug counter.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number (unused).
 * \param [in] type SNMP statistics type.
 * \param [in] trigger The counter select value
 *                     (see include/bcm/stat.h for bit definitions).
 * \param [out] result Result of a query. 0 if positive, -1 if negative.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_custom_check(int unit, bcm_port_t port, bcm_stat_val_t type,
                           bcm_custom_stat_trigger_t trigger, int *result)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_custom_check(unit, port, type, trigger, result));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a certain packet type from debug counter.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number (unused).
 * \param [in] type SNMP statistics type.
 * \param [in] trigger The counter select value
 *                     (see include/bcm/stat.h for bit definitions).
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_custom_delete(int unit, bcm_port_t port,bcm_stat_val_t type,
                            bcm_custom_stat_trigger_t trigger)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_custom_delete(unit, port, type, trigger));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all packet types from debug counter.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number (unused).
 * \param [in] type SNMP statistics type.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_stat_custom_delete_all(int unit, bcm_port_t port, bcm_stat_val_t type)
{
    SHR_FUNC_ENTER(unit);

    if (stat_inited[unit] == FALSE) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stat_custom_delete_all(unit, port, type));

exit:
    SHR_FUNC_EXIT();
}

