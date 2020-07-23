/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file defines common network port modes.
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to define port modes.
 */

#ifndef _SOC_TIMESYNC_H
#define _SOC_TIMESYNC_H

#include <shared/port.h>

typedef enum soc_port_phy_timesync_timer_mode_e { 
    SOC_PORT_PHY_TIMESYNC_TIMER_MODE_NONE =
                            _SHR_PORT_PHY_TIMESYNC_TIMER_MODE_NONE,
    SOC_PORT_PHY_TIMESYNC_TIMER_MODE_32BIT =
                            _SHR_PORT_PHY_TIMESYNC_TIMER_MODE_32BIT,
    SOC_PORT_PHY_TIMESYNC_TIMER_MODE_48BIT =
                            _SHR_PORT_PHY_TIMESYNC_TIMER_MODE_48BIT,
    SOC_PORT_PHY_TIMESYNC_TIMER_MODE_64BIT =
                            _SHR_PORT_PHY_TIMESYNC_TIMER_MODE_64BIT,
    SOC_PORT_PHY_TIMESYNC_TIMER_MODE_80BIT =
                            _SHR_PORT_PHY_TIMESYNC_TIMER_MODE_80BIT
} soc_port_phy_timesync_timer_mode_t;

/* Definitions for compatibility with old drivers */

#define SOC_PORT_TIMESYNC_ENABLE                 _SHR_PORT_PHY_TIMESYNC_ENABLE
#define SOC_PORT_TIMESYNC_CAPTURE_TS_ENABLE      _SHR_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE
#define SOC_PORT_TIMESYNC_HEARTBEAT_TS_ENABLE    _SHR_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE
#define SOC_PORT_TIMESYNC_RX_CRC_ENABLE          _SHR_PORT_PHY_TIMESYNC_RX_CRC_ENABLE
#define SOC_PORT_TIMESYNC_8021AS_ENABLE          _SHR_PORT_PHY_TIMESYNC_8021AS_ENABLE
#define SOC_PORT_TIMESYNC_L2_ENABLE              _SHR_PORT_PHY_TIMESYNC_L2_ENABLE
#define SOC_PORT_TIMESYNC_IP4_ENABLE             _SHR_PORT_PHY_TIMESYNC_IP4_ENABLE
#define SOC_PORT_TIMESYNC_IP6_ENABLE             _SHR_PORT_PHY_TIMESYNC_IP6_ENABLE
#define SOC_PORT_TIMESYNC_CLOCK_SRC_EXT          _SHR_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT


#define SOC_PORT_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE
#define SOC_PORT_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD
#define SOC_PORT_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN
#define SOC_PORT_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP

#define SOC_PORT_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE
#define SOC_PORT_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD
#define SOC_PORT_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP
#define SOC_PORT_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME

#define SOC_PORT_TIMESYNC_MODE_FREE \
        _SHR_PORT_PHY_TIMESYNC_MODE_FREE
#define SOC_PORT_TIMESYNC_MODE_SYNCIN \
        _SHR_PORT_PHY_TIMESYNC_MODE_SYNCIN
#define SOC_PORT_TIMESYNC_MODE_CPU \
        _SHR_PORT_PHY_TIMESYNC_MODE_CPU

#define SOC_PORT_CONTROL_TIMESYNC_CAPTURE_TIMESTAMP \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP
#define SOC_PORT_CONTROL_TIMESYNC_HEARTBEAT_TIMESTAMP \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP
#define SOC_PORT_CONTROL_TIMESYNC_NCOADDEND \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND
#define SOC_PORT_CONTROL_TIMESYNC_FRAMESYNC \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_FRAMESYNC
#define SOC_PORT_CONTROL_TIMESYNC_LOCAL_TIME \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME

typedef _shr_port_phy_timesync_event_message_egress_mode_t soc_port_timesync_event_message_egress_mode_t;
typedef _shr_port_phy_timesync_event_message_ingress_mode_t soc_port_timesync_event_message_ingress_mode_t;
typedef _shr_port_phy_timesync_global_mode_t soc_port_timesync_global_mode_t;
typedef _shr_port_control_phy_timesync_t soc_port_control_timesync_t;

/* End of compatibility definitions */

#define SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_NONE
#define SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_UPDATE_CORRECTIONFIELD
#define SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_REPLACE_CORRECTIONFIELD_ORIGIN
#define SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_EGRESS_MODE_CAPTURE_TIMESTAMP

#define SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_NONE
#define SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_UPDATE_CORRECTIONFIELD
#define SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_TIMESTAMP
#define SOC_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME \
        _SHR_PORT_PHY_TIMESYNC_EVENT_MESSAGE_INGRESS_MODE_INSERT_DELAYTIME

#define SOC_PORT_PHY_TIMESYNC_MODE_FREE _SHR_PORT_PHY_TIMESYNC_MODE_FREE
#define SOC_PORT_PHY_TIMESYNC_MODE_SYNCIN _SHR_PORT_PHY_TIMESYNC_MODE_SYNCIN
#define SOC_PORT_PHY_TIMESYNC_MODE_CPU _SHR_PORT_PHY_TIMESYNC_MODE_CPU

#define SOC_PORT_PHY_TIMESYNC_FRAMESYNC_NONE _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_NONE
#define SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0 _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN0
#define SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1 _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCIN1
#define SOC_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_SYNCOUT
#define SOC_PORT_PHY_TIMESYNC_FRAMESYNC_CPU _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_CPU


#define SOC_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE _SHR_PORT_PHY_TIMESYNC_SYNCOUT_DISABLE
#define SOC_PORT_PHY_TIMESYNC_SYNCOUT_ONE_TIME _SHR_PORT_PHY_TIMESYNC_SYNCOUT_ONE_TIME
#define SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN _SHR_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN
#define SOC_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN_WITH_SYNC _SHR_PORT_PHY_TIMESYNC_SYNCOUT_PULSE_TRAIN_WITH_SYNC


#define SOC_PORT_PHY_TIMESYNC_VALID_FLAGS   _SHR_PORT_PHY_TIMESYNC_VALID_FLAGS /* Flags field is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_ITPID   _SHR_PORT_PHY_TIMESYNC_VALID_ITPID /* itpid is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_OTPID   _SHR_PORT_PHY_TIMESYNC_VALID_OTPID /* otpid is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_OTPID2  _SHR_PORT_PHY_TIMESYNC_VALID_OTPID2 /* otpid2 is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_GMODE   _SHR_PORT_PHY_TIMESYNC_VALID_GMODE /* gmode is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE _SHR_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE /* framesync mode is
                                                          valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE _SHR_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE /* syncout mode is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER _SHR_PORT_PHY_TIMESYNC_VALID_TS_DIVIDER /* ts divider mode is
                                                          valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE _SHR_PORT_PHY_TIMESYNC_VALID_ORIGINAL_TIMECODE /* original timecode is
                                                          valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET _SHR_PORT_PHY_TIMESYNC_VALID_TX_TIMESTAMP_OFFSET /* tx timestamp offset is
                                                          valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET _SHR_PORT_PHY_TIMESYNC_VALID_RX_TIMESTAMP_OFFSET /* rx timestamp offset is
                                                          valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE _SHR_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE /* tx sync mode is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE _SHR_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE /* tx delay request mode
                                                          is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE _SHR_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_REQUEST_MODE /* tx pdelay request mode
                                                          is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE _SHR_PORT_PHY_TIMESYNC_VALID_TX_PDELAY_RESPONSE_MODE /* tx pdelay response
                                                          mode is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE _SHR_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE /* rx sync mode is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE _SHR_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE /* rx delay request mode
                                                          is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE _SHR_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_REQUEST_MODE /* rx pdelay request mode
                                                          is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE _SHR_PORT_PHY_TIMESYNC_VALID_RX_PDELAY_RESPONSE_MODE /* rx pdelay response
                                                          mode is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL _SHR_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL /* mpls control is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY _SHR_PORT_PHY_TIMESYNC_VALID_RX_LINK_DELAY /* rx link delay is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_SYNC_FREQ _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_SYNC_FREQ /* sync freq is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1 _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1 /* phy 1588 dpll k1 is
                                                          valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2 _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2 /* phy 1588 dpll k2 is
                                                          valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3 _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3 /* phy 1588 dpll k3 is
                                                          valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_LOOP_FILTER _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_LOOP_FILTER /* phy 1588 dpll loop
                                                          filter is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE /* phy 1588 dpll ref
                                                          phase is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA /* phy 1588 dpll ref
                                                          phase delta is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL _SHR_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL /* inband ts control is valid */
#define SOC_PORT_PHY_TIMESYNC_VALID_TIMER_ADJUSTMENT _SHR_PORT_PHY_TIMESYNC_VALID_TIMER_ADJUSTMENT /* direct timer adjustment is valid */

#define SOC_PORT_PHY_TIMESYNC_ENABLE        _SHR_PORT_PHY_TIMESYNC_ENABLE /* Enable Time Synchronization Interface */
#define SOC_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE _SHR_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE /* Enable One step Timesamping in Serdes */
#define SOC_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE _SHR_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE /* Enable Packet Timestamping */
#define SOC_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE _SHR_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE /* Enable Heartbeat Timestamping */
#define SOC_PORT_PHY_TIMESYNC_RX_CRC_ENABLE _SHR_PORT_PHY_TIMESYNC_RX_CRC_ENABLE /* Enable CRC checking */
#define SOC_PORT_PHY_TIMESYNC_8021AS_ENABLE _SHR_PORT_PHY_TIMESYNC_8021AS_ENABLE /* Enable IEEE802.1AS mode */
#define SOC_PORT_PHY_TIMESYNC_L2_ENABLE     _SHR_PORT_PHY_TIMESYNC_L2_ENABLE /* IEEE1588 over Layer 2 */
#define SOC_PORT_PHY_TIMESYNC_IP4_ENABLE    _SHR_PORT_PHY_TIMESYNC_IP4_ENABLE /* IEEE1588 over IPV4 UDP */
#define SOC_PORT_PHY_TIMESYNC_IP6_ENABLE    _SHR_PORT_PHY_TIMESYNC_IP6_ENABLE /* IEEE1588 over IPV6 UDP */
#define SOC_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT _SHR_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT /* Use external clock source (DPLL) */
#define SOC_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT_MODE _SHR_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT_MODE /* DPLL mode,
                                                          0 - phase lock, 1 - frequency lock */
#define SOC_PORT_PHY_TIMESYNC_1588_ENCRYPTED_MODE          _SHR_PORT_PHY_TIMESYNC_1588_ENCRYPTED_MODE
#define SOC_PORT_PHY_TIMESYNC_FOLLOW_UP_ASSIST_ENABLE      _SHR_PORT_PHY_TIMESYNC_FOLLOW_UP_ASSIST_ENABLE
#define SOC_PORT_PHY_TIMESYNC_DELAY_RESP_ASSIST_ENABLE     _SHR_PORT_PHY_TIMESYNC_DELAY_RESP_ASSIST_ENABLE
#define SOC_PORT_PHY_TIMESYNC_64BIT_TIMESTAMP_ENABLE       _SHR_PORT_PHY_TIMESYNC_64BIT_TIMESTAMP_ENABLE
#define SOC_PORT_PHY_TIMESYNC_1588_OVER_HSR_ENABLE         _SHR_PORT_PHY_TIMESYNC_1588_OVER_HSR_ENABLE
#define SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_SYNC         _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_SYNC
#define SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_DELAY_REQ    _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_DELAY_REQ
#define SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_PDELAY_REQ   _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_PDELAY_REQ
#define SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_PDELAY_RESP  _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_TX_PDELAY_RESP
#define SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_SYNC         _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_SYNC
#define SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_DELAY_REQ    _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_DELAY_REQ
#define SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_PDELAY_REQ   _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_PDELAY_REQ
#define SOC_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_PDELAY_RESP  _SHR_PORT_PHY_TIMESYNC_CAPTURE_TIMESTAMP_RX_PDELAY_RESP

#define SOC_PORT_PHY_TIMESYNC_CAPTURE_FULL_SOURCE_PORT     0xfffe
#define SOC_PORT_PHY_TIMESYNC_CAPTURE_LONG_TIMESTAMP       0xffff

#define SOC_PORT_PHY_TIMESYNC_CAP_MPLS                     _SHR_PORT_PHY_TIMESYNC_CAP_MPLS
#define SOC_PORT_PHY_TIMESYNC_CAP_ENHANCED_TSFIFO          _SHR_PORT_PHY_TIMESYNC_CAP_ENHANCED_TSFIFO
#define SOC_PORT_PHY_TIMESYNC_CAP_INBAND_TS                _SHR_PORT_PHY_TIMESYNC_CAP_INBAND_TS
#define SOC_PORT_PHY_TIMESYNC_CAP_FOLLOW_UP_ASSIST         _SHR_PORT_PHY_TIMESYNC_CAP_FOLLOW_UP_ASSIST
#define SOC_PORT_PHY_TIMESYNC_CAP_DELAY_RESP_ASSIST        _SHR_PORT_PHY_TIMESYNC_CAP_DELAY_RESP_ASSIST
#define SOC_PORT_PHY_TIMESYNC_CAP_CAPTURE_TIMESTAMP_MSG    _SHR_PORT_PHY_TIMESYNC_CAP_CAPTURE_TIMESTAMP_MSG

#define SOC_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_CHECK      _SHR_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_CHECK
#define SOC_PORT_PHY_TIMESYNC_INBAND_SYNC_ENABLE         _SHR_PORT_PHY_TIMESYNC_INBAND_SYNC_ENABLE
#define SOC_PORT_PHY_TIMESYNC_INBAND_DELAY_RQ_ENABLE     _SHR_PORT_PHY_TIMESYNC_INBAND_DELAY_RQ_ENABLE
#define SOC_PORT_PHY_TIMESYNC_INBAND_PDELAY_RQ_ENABLE    _SHR_PORT_PHY_TIMESYNC_INBAND_PDELAY_RQ_ENABLE
#define SOC_PORT_PHY_TIMESYNC_INBAND_PDELAY_RESP_ENABLE  _SHR_PORT_PHY_TIMESYNC_INBAND_PDELAY_RESP_ENABLE
#define SOC_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_UPDATE     _SHR_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_UPDATE
#define SOC_PORT_PHY_TIMESYNC_INBAND_FOLLOW_UP_ASSIST    _SHR_PORT_PHY_TIMESYNC_INBAND_FOLLOW_UP_ASSIST
#define SOC_PORT_PHY_TIMESYNC_INBAND_DELAY_RESP_ASSIST   _SHR_PORT_PHY_TIMESYNC_INBAND_DELAY_RESP_ASSIST
#define SOC_PORT_PHY_TIMESYNC_INBAND_TIMER_MODE_SELECT   _SHR_PORT_PHY_TIMESYNC_INBAND_TIMER_MODE_SELECT

#define SOC_PORT_PHY_TIMESYNC_INBAND_CAP_SRC_PORT_CLK_ID _SHR_PORT_PHY_TIMESYNC_INBAND_CAP_SRC_PORT_CLK_ID
#define SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_VLAN_ID       _SHR_PORT_PHY_TIMESYNC_INBAND_MATCH_VLAN_ID
#define SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_SRC_PORT_NUM  _SHR_PORT_PHY_TIMESYNC_INBAND_MATCH_SRC_PORT_NUM
#define SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_MAC_ADDR      _SHR_PORT_PHY_TIMESYNC_INBAND_MATCH_MAC_ADDR
#define SOC_PORT_PHY_TIMESYNC_INBAND_MATCH_IP_ADDR       _SHR_PORT_PHY_TIMESYNC_INBAND_MATCH_IP_ADDR


#define SOC_PORT_PHY_TIMESYNC_MSG_SYNC                     _SHR_PORT_PHY_TIMESYNC_MSG_SYNC
#define SOC_PORT_PHY_TIMESYNC_MSG_DELAY_REQ                _SHR_PORT_PHY_TIMESYNC_MSG_DELAY_REQ
#define SOC_PORT_PHY_TIMESYNC_MSG_PDELAY_REQ               _SHR_PORT_PHY_TIMESYNC_MSG_PDELAY_REQ
#define SOC_PORT_PHY_TIMESYNC_MSG_PDELAY_RESP              _SHR_PORT_PHY_TIMESYNC_MSG_PDELAY_RESP

#define SOC_PORT_PHY_TIMESYNC_PROT_LAYER2                  _SHR_PORT_PHY_TIMESYNC_PROT_LAYER2
#define SOC_PORT_PHY_TIMESYNC_PROT_IPV4_UDP                _SHR_PORT_PHY_TIMESYNC_PROT_IPV4_UDP
#define SOC_PORT_PHY_TIMESYNC_PROT_IPV6_UDP                _SHR_PORT_PHY_TIMESYNC_PROT_IPV6_UDP

#define SOC_PORT_PHY_TIMESYNC_DIR_EGRESS                   _SHR_PORT_PHY_TIMESYNC_DIR_EGRESS
#define SOC_PORT_PHY_TIMESYNC_DIR_INGRESS                  _SHR_PORT_PHY_TIMESYNC_DIR_INGRESS


#define SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_IN _SHR_PORT_PHY_TIMESYNC_MPLS_LABEL_IN /* MPLS label applies to
                                                          ingress packets */
#define SOC_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT _SHR_PORT_PHY_TIMESYNC_MPLS_LABEL_OUT /* MPLS label applies to
                                                          egress packets */
#define SOC_PORT_PHY_TIMESYNC_MPLS_ENABLE   _SHR_PORT_PHY_TIMESYNC_MPLS_ENABLE /* Enable processing of
                                                          MPLS 1588 packets */
#define SOC_PORT_PHY_TIMESYNC_MPLS_ENTROPY_ENABLE _SHR_PORT_PHY_TIMESYNC_MPLS_ENTROPY_ENABLE /* Enable MPLS entropy on
                                                          1588 packets */
#define SOC_PORT_PHY_TIMESYNC_MPLS_SPECIAL_LABEL_ENABLE _SHR_PORT_PHY_TIMESYNC_MPLS_SPECIAL_LABEL_ENABLE /* Enable MPLS special
                                                          label on 1588 packets */
#define SOC_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE _SHR_PORT_PHY_TIMESYNC_MPLS_CONTROL_WORD_ENABLE /* Enable MPLS control
                                                          word on 1588 packets */
#define SOC_PORT_PHY_TIMESYNC_TN_LOAD       _SHR_PORT_PHY_TIMESYNC_TN_LOAD /* Load Tn upon framesync */
#define SOC_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_TN_ALWAYS_LOAD /* Always load Tn upon
                                                          framesync */
#define SOC_PORT_PHY_TIMESYNC_TIMECODE_LOAD _SHR_PORT_PHY_TIMESYNC_TIMECODE_LOAD /* Load timecode upon
                                                          framesync */
#define SOC_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_TIMECODE_ALWAYS_LOAD /* Always load timecode
                                                          upon framesync */
#define SOC_PORT_PHY_TIMESYNC_SYNCOUT_LOAD  _SHR_PORT_PHY_TIMESYNC_SYNCOUT_LOAD /* Load syncout upon
                                                          framesync */
#define SOC_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_SYNCOUT_ALWAYS_LOAD /* Always load syncout
                                                          upon framesync */
#define SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD _SHR_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD /* Load NCO divider upon
                                                          framesync */
#define SOC_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_NCO_DIVIDER_ALWAYS_LOAD /* Always load NCO
                                                          divider upon framesync */
#define SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD _SHR_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD /* Load local time upon
                                                          framesync */
#define SOC_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_LOCAL_TIME_ALWAYS_LOAD /* Always load local time
                                                          upon framesync */
#define SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD _SHR_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD /* Load NCO addend upon
                                                          framesync */
#define SOC_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_NCO_ADDEND_ALWAYS_LOAD /* Always load NCO addend
                                                          upon framesync */
#define SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD /* Load DPLL loop filter
                                                          upon framesync */
#define SOC_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_ALWAYS_LOAD /* Always load DPLL loop
                                                          filter upon framesync */
#define SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD /* Load DPLL ref phase
                                                          upon framesync */
#define SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_ALWAYS_LOAD /* Always load DPLL ref
                                                          phase upon framesync */
#define SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD /* Load DPLL ref phase
                                                          delta upon framesync */
#define SOC_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_ALWAYS_LOAD /* Always load DPLL ref
                                                          phase delta upon
                                                          framesync */
#define SOC_PORT_PHY_TIMESYNC_DPLL_K3_LOAD  _SHR_PORT_PHY_TIMESYNC_DPLL_K3_LOAD /* Load DPLL coefficient
                                                          K3 upon framesync */
#define SOC_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_K3_ALWAYS_LOAD /* Always load DPLL
                                                          coefficient K3 upon
                                                          framesync */
#define SOC_PORT_PHY_TIMESYNC_DPLL_K2_LOAD  _SHR_PORT_PHY_TIMESYNC_DPLL_K2_LOAD /* Load DPLL coefficient
                                                          K2 upon framesync */
#define SOC_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_K2_ALWAYS_LOAD /* Always load DPLL
                                                          coefficient K2 upon
                                                          framesync */
#define SOC_PORT_PHY_TIMESYNC_DPLL_K1_LOAD  _SHR_PORT_PHY_TIMESYNC_DPLL_K1_LOAD /* Load DPLL coefficient
                                                          K1 upon framesync */
#define SOC_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD _SHR_PORT_PHY_TIMESYNC_DPLL_K1_ALWAYS_LOAD /* Always load DPLL
                                                          coefficient K1 upon
                                                          framesync */
#define SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT /* Packet timestamp
                                                          interrupt */
#define SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_0 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_0 /* Packet 
                                                          timestamp interrupt from port 0 */
#define SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_1 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_1 /* Packet 
                                                          timestamp interrupt from port 1 */
#define SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_2 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_2 /* Packet 
                                                          timestamp interrupt from port 2 */
#define SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_3 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_3 /* Packet 
                                                          timestamp interrupt from port 3 */
#define SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_4 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_4 /* Packet 
                                                          timestamp interrupt from port 4 */
#define SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_5 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_5 /* Packet 
                                                          timestamp interrupt from port 5 */
#define SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_6 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_6 /* Packet 
                                                          timestamp interrupt from port 6 */
#define SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_7 _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_7 /* Packet 
                                                          timestamp interrupt from port 7 */
#define SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT /* Framesync interrupt */
#define SOC_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK _SHR_PORT_PHY_TIMESYNC_TIMESTAMP_INTERRUPT_MASK /* Packet timestamp
                                                          interrupt mask */
#define SOC_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK _SHR_PORT_PHY_TIMESYNC_FRAMESYNC_INTERRUPT_MASK /* Framesync interrupt
                                                          mask */

#define SOC_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_CAPTURE_TIMESTAMP
#define SOC_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_HEARTBEAT_TIMESTAMP
#define SOC_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_NCOADDEND
#define SOC_PORT_CONTROL_PHY_TIMESYNC_FRAMESYNC \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_FRAMESYNC
#define SOC_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_LOCAL_TIME
#define SOC_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_LOAD_CONTROL
#define SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT
#define SOC_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_INTERRUPT_MASK
#define SOC_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_TX_TIMESTAMP_OFFSET
#define SOC_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_RX_TIMESTAMP_OFFSET
#define SOC_PORT_CONTROL_PHY_TIMESYNC_NSE_TIME_OFFSET \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_NSE_TIME_OFFSET
#define SOC_PORT_CONTROL_PHY_TIMESYNC_TIMESTAMP_OFFSET \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_TIMESTAMP_OFFSET
#define SOC_PORT_CONTROL_PHY_TIMESYNC_TIMESTAMP_ADJUST \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_TIMESTAMP_ADJUST
#define SOC_PORT_CONTROL_PHY_TIMESYNC_ONE_STEP_ENABLE \
        _SHR_PORT_CONTROL_PHY_TIMESYNC_ONE_STEP_ENABLE

#define SOC_PORT_PHY_TIMESYNC_COMPENSATION_MODE_NONE \
        _SHR_PORT_PHY_TIMESYNC_COMPENSATION_MODE_NONE
#define SOC_PORT_PHY_TIMESYNC_COMPENSATION_MODE_EARLIEST_LANE \
        _SHR_PORT_PHY_TIMESYNC_COMPENSATION_MODE_EARLIEST_LANE
#define SOC_PORT_PHY_TIMESYNC_COMPENSATION_MODE_LATEST_LANE \
        _SHR_PORT_PHY_TIMESYNC_COMPENSATION_MODE_LATEST_LANE

/* time_spec type */
typedef _shr_time_spec_t soc_time_spec_t;

/* Actions on Egress event messages */
typedef _shr_port_phy_timesync_event_message_egress_mode_t soc_port_phy_timesync_event_message_egress_mode_t;
/* Actions on ingress event messages */
typedef _shr_port_phy_timesync_event_message_ingress_mode_t soc_port_phy_timesync_event_message_ingress_mode_t;

/* Global mode actions */
typedef _shr_port_phy_timesync_global_mode_t soc_port_phy_timesync_global_mode_t;
/* Fast call actions */
typedef _shr_port_control_phy_timesync_t soc_port_control_phy_timesync_t;

typedef _shr_port_phy_timesync_framesync_mode_t soc_port_phy_timesync_framesync_mode_t;
typedef _shr_port_phy_timesync_syncout_mode_t soc_port_phy_timesync_syncout_mode_t;

typedef _shr_port_phy_timesync_framesync_t soc_port_phy_timesync_framesync_t;
typedef _shr_port_phy_timesync_syncout_t soc_port_phy_timesync_syncout_t;
typedef _shr_port_phy_timesync_mpls_control_t soc_port_phy_timesync_mpls_control_t;

typedef _shr_port_phy_timesync_timer_adjust_t soc_port_phy_timesync_timer_adjust_t;
typedef _shr_port_phy_timesync_inband_control_t soc_port_phy_timesync_inband_control_t;

typedef _shr_port_phy_timesync_msg_t soc_port_phy_timesync_msg_t;
typedef _shr_port_phy_timesync_prot_t soc_port_phy_timesync_prot_t;
typedef _shr_port_phy_timesync_dir_t soc_port_phy_timesync_dir_t;
typedef _shr_port_phy_timesync_enhanced_capture_t soc_port_phy_timesync_enhanced_capture_t;

/* Base timesync configuration type. */
typedef struct soc_port_phy_timesync_config_s {
    uint32 capabilities;                /* Flags BCM_PORT_PHY_TIMESYNC_CAP_* */
    uint32 validity_mask;               /* Flags BCM_PORT_PHY_TIMESYNC_VALID_* */
    uint32 flags;                       /* Flags BCM_PORT_PHY_TIMESYNC_* */
    uint16 itpid;                       /* 1588 inner tag */
    uint16 otpid;                       /* 1588 outer tag */
    uint16 otpid2;                      /* 1588 outer tag2 */
    soc_port_phy_timesync_timer_adjust_t   timer_adjust;   /* Timer adjusting mode & delta */
    soc_port_phy_timesync_inband_control_t inband_control; /* Inband TS control */
    soc_port_phy_timesync_global_mode_t gmode; /* Global mode */
    soc_port_phy_timesync_framesync_t framesync; /* Framesync */
    soc_port_phy_timesync_syncout_t        syncout;   /* SyncIN/SyncOut mode */
    uint16 ts_divider;                  /* Timestamp divider */
    soc_time_spec_t original_timecode;  /* Original timecode to be inserted */
    uint32 tx_timestamp_offset;         /* TX AFE delay in ns - per port */
    uint32 rx_timestamp_offset;         /* RX AFE delay in ns - per port */
    uint32 rx_link_delay;               /* RX link delay */
    soc_port_phy_timesync_event_message_egress_mode_t tx_sync_mode; /* sync */
    soc_port_phy_timesync_event_message_egress_mode_t tx_delay_request_mode; /* delay request */
    soc_port_phy_timesync_event_message_egress_mode_t tx_pdelay_request_mode; /* pdelay request */
    soc_port_phy_timesync_event_message_egress_mode_t tx_pdelay_response_mode; /* pdelay response */
    soc_port_phy_timesync_event_message_ingress_mode_t rx_sync_mode; /* sync */
    soc_port_phy_timesync_event_message_ingress_mode_t rx_delay_request_mode; /* delay request */
    soc_port_phy_timesync_event_message_ingress_mode_t rx_pdelay_request_mode; /* pdelay request */
    soc_port_phy_timesync_event_message_ingress_mode_t rx_pdelay_response_mode; /* pdelay response */
    soc_port_phy_timesync_mpls_control_t mpls_control; /* MPLS control */
    uint32 sync_freq;                     /* sync frequency */
    uint16 phy_1588_dpll_k1;              /* DPLL K1 */
    uint16 phy_1588_dpll_k2;              /* DPLL K2 */
    uint16 phy_1588_dpll_k3;              /* DPLL K3 */
    uint64 phy_1588_dpll_loop_filter;     /* DPLL loop filter */
    uint64 phy_1588_dpll_ref_phase;       /* DPLL ref phase */
    uint32 phy_1588_dpll_ref_phase_delta; /* DPLL ref phase delta */
} soc_port_phy_timesync_config_t;


/* soc_port_phy_timesync_config_set */
extern int soc_port_phy_timesync_config_set(
    int unit, 
    soc_port_t port, 
    soc_port_phy_timesync_config_t *conf);

/* soc_port_phy_timesync_config_get */
extern int soc_port_phy_timesync_config_get(
    int unit, 
    soc_port_t port, 
    soc_port_phy_timesync_config_t *conf);

/* soc_port_control_phy_timesync_set */
extern int soc_port_control_phy_timesync_set(
    int unit, 
    soc_port_t port, 
    soc_port_control_phy_timesync_t type, 
    uint64 value);

/* soc_port_control_phy_timesync_get */
extern int soc_port_control_phy_timesync_get(
    int unit, 
    soc_port_t port, 
    soc_port_control_phy_timesync_t type, 
    uint64 *value);

/* soc_port_phy_timesync_enhanced_capture_get */
extern int soc_port_phy_timesync_enhanced_capture_get(
    int unit, 
    soc_port_t port, 
    soc_port_phy_timesync_enhanced_capture_t *value);

#endif  /* !_SOC_TIMESYNC_H */
